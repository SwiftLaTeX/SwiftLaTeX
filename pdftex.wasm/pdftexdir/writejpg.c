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

#include "ptexlib.h"
#include "image.h"

#define JPG_GRAY  1             /* Gray color space, use /DeviceGray  */
#define JPG_RGB   3             /* RGB color space, use /DeviceRGB    */
#define JPG_CMYK  4             /* CMYK color space, use /DeviceCMYK  */

typedef enum {                  /* JPEG marker codes                    */
    M_SOF0 = 0xc0,              /* baseline DCT                         */
    M_SOF1 = 0xc1,              /* extended sequential DCT              */
    M_SOF2 = 0xc2,              /* progressive DCT                      */
    M_SOF3 = 0xc3,              /* lossless (sequential)                */

    M_SOF5 = 0xc5,              /* differential sequential DCT          */
    M_SOF6 = 0xc6,              /* differential progressive DCT         */
    M_SOF7 = 0xc7,              /* differential lossless                */

    M_JPG = 0xc8,               /* JPEG extensions                      */
    M_SOF9 = 0xc9,              /* extended sequential DCT              */
    M_SOF10 = 0xca,             /* progressive DCT                      */
    M_SOF11 = 0xcb,             /* lossless (sequential)                */

    M_SOF13 = 0xcd,             /* differential sequential DCT          */
    M_SOF14 = 0xce,             /* differential progressive DCT         */
    M_SOF15 = 0xcf,             /* differential lossless                */

    M_DHT = 0xc4,               /* define Huffman tables                */

    M_DAC = 0xcc,               /* define arithmetic conditioning table */

    M_RST0 = 0xd0,              /* restart                              */
    M_RST1 = 0xd1,              /* restart                              */
    M_RST2 = 0xd2,              /* restart                              */
    M_RST3 = 0xd3,              /* restart                              */
    M_RST4 = 0xd4,              /* restart                              */
    M_RST5 = 0xd5,              /* restart                              */
    M_RST6 = 0xd6,              /* restart                              */
    M_RST7 = 0xd7,              /* restart                              */

    M_SOI = 0xd8,               /* start of image                       */
    M_EOI = 0xd9,               /* end of image                         */
    M_SOS = 0xda,               /* start of scan                        */
    M_DQT = 0xdb,               /* define quantization tables           */
    M_DNL = 0xdc,               /* define number of lines               */
    M_DRI = 0xdd,               /* define restart interval              */
    M_DHP = 0xde,               /* define hierarchical progression      */
    M_EXP = 0xdf,               /* expand reference image(s)            */

    M_APP0 = 0xe0,              /* application marker, used for JFIF    */
    M_APP1 = 0xe1,              /* application marker                   */
    M_APP2 = 0xe2,              /* application marker                   */
    M_APP3 = 0xe3,              /* application marker                   */
    M_APP4 = 0xe4,              /* application marker                   */
    M_APP5 = 0xe5,              /* application marker                   */
    M_APP6 = 0xe6,              /* application marker                   */
    M_APP7 = 0xe7,              /* application marker                   */
    M_APP8 = 0xe8,              /* application marker                   */
    M_APP9 = 0xe9,              /* application marker                   */
    M_APP10 = 0xea,             /* application marker                   */
    M_APP11 = 0xeb,             /* application marker                   */
    M_APP12 = 0xec,             /* application marker                   */
    M_APP13 = 0xed,             /* application marker                   */
    M_APP14 = 0xee,             /* application marker, used by Adobe    */
    M_APP15 = 0xef,             /* application marker                   */

    M_JPG0 = 0xf0,              /* reserved for JPEG extensions         */
    M_JPG13 = 0xfd,             /* reserved for JPEG extensions         */
    M_COM = 0xfe,               /* comment                              */

    M_TEM = 0x01,               /* temporary use                        */

    M_ERROR = 0x100             /* dummy marker, internal use only      */
} JPEG_MARKER;

static JPG_UINT16 read2bytes(FILE * f)
{
    int c = xgetc(f);
    return (c << 8) + xgetc(f);
}

static unsigned char
get_unsigned_byte (FILE *file)
{
    int ch;
    ch = fgetc (file);
    return (unsigned char) ch;
}

static unsigned short
get_unsigned_pair (FILE *file)
{
    unsigned short pair = get_unsigned_byte(file);
    pair = (pair << 8) | get_unsigned_byte(file);
    return pair;
}

static unsigned int
read_exif_bytes(unsigned char **p, int n, int b)
{
    unsigned int rval = 0;
    unsigned char *pp = *p;
    if (b) {
        switch (n) {
        case 4:
            rval += *pp++; rval <<= 8;
            rval += *pp++; rval <<= 8;
        case 2:
            rval += *pp++; rval <<= 8;
            rval += *pp;
            break;
        }
    } else {
        pp += n;
        switch (n) {
        case 4:
            rval += *--pp; rval <<= 8;
            rval += *--pp; rval <<= 8;
        case 2:
            rval += *--pp; rval <<= 8;
            rval += *--pp;
            break;
        }
    }
    *p += n;
    return rval;
}

#define NEW(n, t)  (t*)xmalloc(n * sizeof(t))

static void
read_APP1_Exif (FILE *fp, unsigned short length, int *xx, int *yy)
{
    /* this doesn't save the data, just reads the tags we need */
    /* based on info from http://www.exif.org/Exif2-2.PDF */
    unsigned char *buffer = NEW(length, unsigned char);
    unsigned char *p, *rp;
    unsigned char *tiff_header;
    char bigendian;
    int i;
    int num_fields, tag, type;
    int value = 0, num = 0, den = 0;	/* silence uninitialized warnings */
    double xres = 72.0;
    double yres = 72.0;
    double res_unit = 1.0;
    fread(buffer, length, 1, fp);
    p = buffer;
    while ((p < buffer + length) && (*p == 0))
        ++p;
    tiff_header = p;
    if ((*p == 'M') && (*(p+1) == 'M'))
        bigendian = 1;
    else if ((*p == 'I') && (*(p+1) == 'I'))
        bigendian = 0;
    else
        goto err;
    p += 2;
    i = read_exif_bytes(&p, 2, bigendian);
    if (i != 42)
        goto err;
    i = read_exif_bytes(&p, 4, bigendian);
    p = tiff_header + i;
    num_fields = read_exif_bytes(&p, 2, bigendian);
    while (num_fields-- > 0) {
        tag = read_exif_bytes(&p, 2, bigendian);
        type = read_exif_bytes(&p, 2, bigendian);
        read_exif_bytes(&p, 4, bigendian);
        switch (type) {
        case 1: /* byte */
            value = *p++;
            p += 3;
            break;
        case 3: /* short */
            value = read_exif_bytes(&p, 2, bigendian);
            p += 2;
            break;
        case 4: /* long */
        case 9: /* slong */
            value = read_exif_bytes(&p, 4, bigendian);
            break;
        case 5: /* rational */
        case 10: /* srational */
            value = read_exif_bytes(&p, 4, bigendian);
            rp = tiff_header + value;
            num = read_exif_bytes(&rp, 4, bigendian);
            den = read_exif_bytes(&rp, 4, bigendian);
            break;
        case 7: /* undefined */
            value = *p++;
            p += 3;
            break;
        case 2: /* ascii */
        default:
            p += 4;
            break;
	}
        switch (tag) {
        case 282: /* x res */
            if (den != 0)
                xres = num / den;
            break;
        case 283: /* y res */
            if (den != 0)
                yres = num / den;
            break;
        case 296: /* res unit */
            switch (value) {
            case 2:
                res_unit = 1.0;
                break;
            case 3:
                res_unit = 2.54;
                break;
            }
        }
    }

    *xx = (int)(xres * res_unit);
    *yy = (int)(yres * res_unit);

err:
    free(buffer);
    return;
}

void read_jpg_info(integer img)
{
    int i, units = 0;
    unsigned short appmk, length;
    unsigned char jpg_id[] = "JFIF";
    img_xres(img) = img_yres(img) = 0;
    jpg_ptr(img)->file = xfopen(img_name(img), FOPEN_RBIN_MODE);
    /* no LFS needed, as JPEG is limited to <4GiB */
    xfseek(jpg_ptr(img)->file, 0, SEEK_END, cur_file_name);
    jpg_ptr(img)->length = xftell(jpg_ptr(img)->file, cur_file_name);
    xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
    if (read2bytes(jpg_ptr(img)->file) != 0xFFD8)
        pdftex_fail("reading JPEG image failed (no JPEG header found)");
    /* currently JFIF and Exif files allow extracting img_xres and img_yres */
    appmk = read2bytes(jpg_ptr(img)->file);
    if (appmk == 0xFFE0) {     /* check for JFIF */
        (void) read2bytes(jpg_ptr(img)->file);
        for (i = 0; i < 5; i++) {
            if (xgetc(jpg_ptr(img)->file) != jpg_id[i])
                break;
        }
        if (i == 5) {           /* it's JFIF */
            read2bytes(jpg_ptr(img)->file);
            units = xgetc(jpg_ptr(img)->file);
            img_xres(img) = read2bytes(jpg_ptr(img)->file);
            img_yres(img) = read2bytes(jpg_ptr(img)->file);
            switch (units) {
            case 1:
                break;          /* pixels per inch */
            case 2:
                img_xres(img) *= 2.54;
                img_yres(img) *= 2.54;
                break;          /* pixels per cm */
            default:
                img_xres(img) = img_yres(img) = 0;
                break;
            }
        }
        /* if either xres or yres is 0 but the other isn't,
           set it to the value of the other */
        if ((img_xres(img) == 0) && (img_yres(img) != 0)) {
            img_xres(img) = img_yres(img);
        }
        if ((img_yres(img) == 0) && (img_xres(img) != 0)) {
            img_yres(img) = img_xres(img);
        }
    } else if (appmk == 0xFFE1) {  /* check for Exif */
        FILE *fp = jpg_ptr(img)->file;
        int xxres = 0;
        int yyres = 0;
        char app_sig[32];
        length = get_unsigned_pair(fp) - 2;
        if (length > 5) {
            if (fread(app_sig, sizeof(char), 5, fp) != 5)
                return;
            length -= 5;
            if (!memcmp(app_sig, "Exif\000", 5)) {
                read_APP1_Exif(fp, length, &xxres, &yyres);
            }
        }
        img_xres(img) = xxres;
        img_yres(img) = yyres;
    }
    xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
    while (1) {
        if (feof(jpg_ptr(img)->file))
            pdftex_fail("reading JPEG image failed (premature file end)");
        if (fgetc(jpg_ptr(img)->file) != 0xFF)
            pdftex_fail("reading JPEG image failed (no marker found)");
        switch (xgetc(jpg_ptr(img)->file)) {
        case M_SOF5:
        case M_SOF6:
        case M_SOF7:
        case M_SOF9:
        case M_SOF10:
        case M_SOF11:
        case M_SOF13:
        case M_SOF14:
        case M_SOF15:
            pdftex_fail("unsupported type of compression");
        case M_SOF2:
            if (fixedpdfminorversion <= 2)
                pdftex_fail("cannot use progressive DCT with PDF-1.2");
        case M_SOF0:
        case M_SOF1:
        case M_SOF3:
            (void) read2bytes(jpg_ptr(img)->file);      /* read segment length  */
            jpg_ptr(img)->bits_per_component = xgetc(jpg_ptr(img)->file);
            img_height(img) = read2bytes(jpg_ptr(img)->file);
            img_width(img) = read2bytes(jpg_ptr(img)->file);
            jpg_ptr(img)->color_space = xgetc(jpg_ptr(img)->file);
            xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
            switch (jpg_ptr(img)->color_space) {
            case JPG_GRAY:
                img_color(img) = IMAGE_COLOR_B;
                break;
            case JPG_RGB:
                img_color(img) = IMAGE_COLOR_C;
                break;
            case JPG_CMYK:
                img_color(img) = IMAGE_COLOR_C;
                break;
            default:
                pdftex_fail("Unsupported color space %i",
                            (int) jpg_ptr(img)->color_space);
            }
            return;
        case M_SOI:            /* ignore markers without parameters */
        case M_EOI:
        case M_TEM:
        case M_RST0:
        case M_RST1:
        case M_RST2:
        case M_RST3:
        case M_RST4:
        case M_RST5:
        case M_RST6:
        case M_RST7:
            break;
        default:               /* skip variable length markers */
            xfseek(jpg_ptr(img)->file, read2bytes(jpg_ptr(img)->file) - 2,
                   SEEK_CUR, cur_file_name);
            break;
        }
    }
}

void write_jpg(integer img)
{
    long unsigned l;
    FILE *f;
    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n/Length %i\n",
               (int) img_width(img),
               (int) img_height(img),
               (int) jpg_ptr(img)->bits_per_component,
               (int) jpg_ptr(img)->length);
    pdf_puts("/ColorSpace ");
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        switch (jpg_ptr(img)->color_space) {
        case JPG_GRAY:
            pdf_puts("/DeviceGray\n");
            break;
        case JPG_RGB:
            pdf_puts("/DeviceRGB\n");
            break;
        case JPG_CMYK:
            pdf_puts("/DeviceCMYK\n/Decode [1 0 1 0 1 0 1 0]\n");
            break;
        default:
            pdftex_fail("Unsupported color space %i",
                        (int) jpg_ptr(img)->color_space);
        }
    }
    pdf_puts("/Filter /DCTDecode\n>>\nstream\n");
    for (l = jpg_ptr(img)->length, f = jpg_ptr(img)->file; l > 0; l--)
        pdfout(xgetc(f));
    pdfendstream();
}
