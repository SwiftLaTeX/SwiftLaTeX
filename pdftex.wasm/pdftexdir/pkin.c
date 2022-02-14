/*
Copyright 1996-2014 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * NAME
 * pkin.c - implementation of readchar()
 * DESCRIPTION
 * This implementation of readchar() uses parts of the program dvips
 * written by Tomas Rokicki--the inventor of the pkformat--(loadfont.c,
 * download.c and unpack.c). Dvips in turn is derived from pktype.
 * Pktype(TeX) is described in debt in ``The PKtype processor'',
 * which is available as pktype.weave as part of the METAFONTware.
 * What was needed to implement readchar() is rearranged in pkfile.c to
 * get more modularity in the style of MODULA2.
 * BUGFIXES
 *      May 1997: Eric Delaunay <delaunay@lix.polytechnique.fr> reports a
 *      problem with huge fonts (greater than 1008 DPI). The code for
 *      handling PK characters in `extended format' was wrongly derived
 *      from dvips. Made some minor improvements regarding error handling.
 * REDESIGN
 * Piet Tutelaers
 * rcpt@urc.tue.nl
 *
 *  Modified for use with pdftex by Han The Thanh <thanh@fi.muni.cz>.
 */

#include "ptexlib.h"

/*
 *   Now we have some routines to get stuff from the pk file.  pkbyte returns
 *   the next byte from the pk file.
 */


/*
static FILE *pkfile ;
*/
#define pkfile t3_file

static shalfword pkbyte(void)
{
    register shalfword i;

    if ((i = xgetc(pkfile)) == EOF)
        pdftex_fail("unexpected eof in pk file");
    return (i);
}

static integer pkduo(void)
{
    register integer i;

    i = pkbyte();
    if (i > 127)
        i -= 256;
    i = i * 256 + pkbyte();
    return (i);
}

static integer pktrio(void)
{
    register integer i;

    i = pkbyte();
    if (i > 127)
        i -= 256;
    i = i * 256 + pkbyte();
    i = i * 256 + pkbyte();
    return (i);
}

static integer pkquad(void)
{
    register integer i;

    i = pkbyte();
    if (i > 127)
        i -= 256;
    i = i * 256 + pkbyte();
    i = i * 256 + pkbyte();
    i = i * 256 + pkbyte();
    return (i);
}

/*
 *   The next part is devoted to unpacking the character data.
 */

/*
 *   We need procedures to get a nybble, bit, and packed word from the
 *   packed data structure.
 */

static halfword inputbyte, flagbyte;
static halfword bitweight;
static halfword dynf;
static halfword repeatcount;

static shalfword getnyb(void)
{
    halfword temp;
    if (bitweight == 0) {
        bitweight = 16;
        inputbyte = pkbyte();
        temp = inputbyte >> 4;
    } else {
        bitweight = 0;
        temp = inputbyte & 15;
    }
    return (temp);
}

static boolean getbit(void)
{
    bitweight >>= 1;
    if (bitweight == 0) {
        inputbyte = pkbyte();
        bitweight = 128;
    }
    return (inputbyte & bitweight);
}

static halfword(*realfunc) (void);
long pk_remainder;
static halfword handlehuge(halfword i, halfword k);

static halfword pkpackednum(void)
{
    register halfword i, j;
    i = getnyb();
    if (i == 0) {
        do {
            j = getnyb();
            i++;
        } while (!(j != 0));
        if (i > 3) {
/*
 *   Damn, we got a huge count!  We *fake* it by giving an artificially
 *   large repeat count.
 */
            return (handlehuge(i, j));
        } else {
            while (i > 0) {
                j = j * 16 + getnyb();
                i--;
            }
            return (j - 15 + (13 - dynf) * 16 + dynf);
        }
    } else if (i <= dynf)
        return (i);
    else if (i < 14)
        return ((i - dynf - 1) * 16 + getnyb() + dynf + 1);
    else {
        if (i == 14)
            repeatcount = pkpackednum();
        else
            repeatcount = 1;
        return ((*realfunc) ());
    }
}

static halfword rest(void)
{
    halfword i;

    if (pk_remainder < 0) {
        pk_remainder = -pk_remainder;
        return (0);
    } else if (pk_remainder > 0) {
        if (pk_remainder > 4000) {
            pk_remainder = 4000 - pk_remainder;
            return (4000);
        } else {
            i = pk_remainder;
            pk_remainder = 0;
            realfunc = pkpackednum;
            return (i);
        }
    } else {
        pdftex_fail("shouldn't happen");
        return 0;
     /*NOTREACHED*/}
}

static halfword handlehuge(halfword i, halfword k)
{
    register long j = k;

    while (i) {
        j = (j << 4L) + getnyb();
        i--;
    }
    pk_remainder = j - 15 + (13 - dynf) * 16 + dynf;
    realfunc = rest;
    return (rest());
}

/*
 *   And now we have our unpacking routine.
 */

static halfword gpower[17] = { 0, 1, 3, 7, 15, 31, 63, 127,
    255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535
};

static void unpack(chardesc * cd)
{
    register integer i, j;
    register halfword word, wordweight;
    halfword *raster;
    shalfword rowsleft;
    boolean turnon;
    shalfword hbit;
    halfword count;
    shalfword wordwidth;

    wordwidth = (cd->cwidth + 15) / 16;
    i = 2 * cd->cheight * (long) wordwidth;
    if (i <= 0)
        i = 2;
    if (i > cd->rastersize) {
        xfree(cd->raster);
        cd->rastersize = i;
        cd->raster = xtalloc(cd->rastersize, halfword);
    }
    raster = cd->raster;
    realfunc = pkpackednum;
    dynf = flagbyte / 16;
    turnon = flagbyte & 8;
    if (dynf == 14) {
        bitweight = 0;
        for (i = 1; i <= cd->cheight; i++) {
            word = 0;
            wordweight = 32768;
            for (j = 1; j <= cd->cwidth; j++) {
                if (getbit())
                    word += wordweight;
                wordweight >>= 1;
                if (wordweight == 0) {
                    *raster++ = word;
                    word = 0;
                    wordweight = 32768;
                }
            }
            if (wordweight != 32768)
                *raster++ = word;
        }
    } else {
        rowsleft = cd->cheight;
        hbit = cd->cwidth;
        repeatcount = 0;
        wordweight = 16;
        word = 0;
        bitweight = 0;
        while (rowsleft > 0) {
            count = (*realfunc) ();
            while (count != 0) {
                if ((count < wordweight) && (count < hbit)) {
                    if (turnon)
                        word += gpower[wordweight] - gpower[wordweight - count];
                    hbit -= count;
                    wordweight -= count;
                    count = 0;
                } else if ((count >= hbit) && (hbit <= wordweight)) {
                    if (turnon)
                        word += gpower[wordweight] - gpower[wordweight - hbit];
                    *raster++ = word;
                    for (i = 1; i <= repeatcount; i++) {
                        for (j = 1; j <= wordwidth; j++) {
                            *raster = *(raster - wordwidth);
                            raster++;
                        }
                    }
                    rowsleft -= repeatcount + 1;
                    repeatcount = 0;
                    word = 0;
                    wordweight = 16;
                    count -= hbit;
                    hbit = cd->cwidth;
                } else {
                    if (turnon)
                        word += gpower[wordweight];
                    *raster++ = word;
                    word = 0;
                    count -= wordweight;
                    hbit -= wordweight;
                    wordweight = 16;
                }
            }
            turnon = !turnon;
        }
        if ((rowsleft != 0) || ((integer) hbit != cd->cwidth))
            pdftex_fail("error while unpacking; more bits than required");
    }
}

/*
 *   readchar(): the main routine
 *   Reads the character definition of character `c' into `cd' if available,
 *   return FALSE (0) otherwise.
 */

/*
 *   readchar(): the main routine
 *   check pk preamble if necessary,
 *   read the next character definition into `cd',
 *   return EOF if no character definition is available
 */

int readchar(boolean check_preamble, chardesc * cd)
{
    register shalfword i;
    register integer k;
    register integer length = 0;

/*
 *   Check the preamble of the pkfile
 */
    if (check_preamble) {
        if (pkbyte() != 247)
            pdftex_fail("bad pk file, expected pre");
        if (pkbyte() != 89)
            pdftex_fail("bad version of pk file");
        for (i = pkbyte(); i > 0; i--)  /* creator of pkfile */
            (void) pkbyte();
        (void) pkquad();        /* design size */
        k = pkquad();           /* checksum    */
        k = pkquad();           /* hppp        */
        k = pkquad();           /* vppp   */
    }
/*
 *   Now we skip to the desired character definition
 */
    while ((flagbyte = pkbyte()) != 245) {
        if (flagbyte < 240) {
            switch (flagbyte & 7) {
            case 0:
            case 1:
            case 2:
            case 3:
                length = (flagbyte & 7) * 256 + pkbyte() - 3;
                cd->charcode = pkbyte();
                (void) pktrio();        /* TFMwidth */
                cd->xescape = pkbyte(); /* pixel width */
                cd->cwidth = pkbyte();
                cd->cheight = pkbyte();
                cd->xoff = pkbyte();
                cd->yoff = pkbyte();
                if (cd->xoff > 127)
                    cd->xoff -= 256;
                if (cd->yoff > 127)
                    cd->yoff -= 256;
                break;
            case 4:
            case 5:
            case 6:
                length = (flagbyte & 3) * 65536L + pkbyte() * 256L;
                length = length + pkbyte() - 4L;
                cd->charcode = pkbyte();
                (void) pktrio();        /* TFMwidth */
                cd->xescape = pkduo();  /* pixelwidth */
                cd->cwidth = pkduo();
                cd->cheight = pkduo();
                cd->xoff = pkduo();
                cd->yoff = pkduo();
                break;
            case 7:
                length = pkquad() - 9L;
                cd->charcode = pkquad();
                (void) pkquad();        /* TFMwidth */
                cd->xescape = pkquad(); /* pixelwidth */
                k = pkquad();
                cd->cwidth = pkquad();
                cd->cheight = pkquad();
                cd->xoff = pkquad();
                cd->yoff = pkquad();
            }
            if (length <= 0)
                pdftex_fail("packet length (%i) too small", (int) length);
            unpack(cd);
            return 1;
        } else {
            k = 0;
            switch (flagbyte) {
            case 243:
                k = pkbyte();
                if (k > 127)
                    k -= 256;
            case 242:
                k = k * 256 + pkbyte();
            case 241:
                k = k * 256 + pkbyte();
            case 240:
                k = k * 256 + pkbyte();
                while (k-- > 0)
                    i = pkbyte();
                break;
            case 244:
                k = pkquad();
                break;
            case 246:
                break;
            default:
                pdftex_fail("unexpected command (%i)", (int) flagbyte);
            }
        }
    }
    return 0;                   /* character not found */
}
