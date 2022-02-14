/*
Copyright 2002-2014 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

pdfTeX is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.

This is experimental JBIG2 image support to pdfTeX. JBIG2 image decoding
is part of Adobe PDF-1.4, and requires Acroread 5.0 or later.

References
==========

* 14492 FCD: Information technology -- coded representation of picture
and audio information -- lossy/lossless coding of bi-level images /
JBIG committee, 1999 July 16. This JBIG2 Working Draft is available from
http://www.jpeg.org/public/fcd14492.pdf. The references in the C-code
correspond to the sections of this document.

* PDF Reference, 5th edition, version 1.6, 1985--2005 Adobe Systems
Incorporated. Available online:

http://partners.adobe.com/public/developer/en/pdf/PDFReference16.pdf

News
====

31 May 2006: no need to wait for endoffileflag in sequential access
organization.

10 May 2006: ygetc() for some catching of broken JBIG2 files; modify to
accept Example 3.4 from PDFRef 5th ed. with short end-of-file segment.

09 May 2006: pages_maketree() and segments_maketree() by AVL tree,
some cleaning up.

06 May 2006: File list replaced by AVL tree; new_fileinfo(),
new_pageinfo().

04 May 2006: Updated for pdftex-1.40-beta-20060213.

08 Jan. 2003: Added flushjbig2page0objects() function. Now at the end
of the pdfTeX run all pending page0 objects are written out.

08 Jan. 2003: Release on private webpage.

04 Jan. 2003: Completely rewritten. Now with some data structures.
Rudimentary local file and image bookkeeping. Multiple image inclusion
from one JBIG2 file. Only required page0 segments are marked for
inclusion.

13 Nov. 2002: pdfcrypting removed.

08 Dec. 2002: bug in page 0 stream writing repaired.
Strategy for multiple page inclusion from same JBIG2 file: When writing
1st image, create fresh PDF object for page 0, and include any page
0 segments from complete file (even if these segments are not needed
for image). When writing next image, check by filename comparison if
PDF object for page 0 of this JBIG2 file has already been written. This
can only remember the file name for the direct predecessor JBIG2 image
(but images of other types might come inbetween). If such page 0 PDF
object exists, reference it. Else create fresh one.

09 Dec. 2002: JBIG2 seg. page numbers > 0 are now set to 1, see PDF Ref.

***********************************************************************/

#include "ptexlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "ptexmac.h"
#include "image.h"

/* 7.3 Segment types */
#define M_SymbolDictionary 0
#define M_IntermediateTextRegion 4
#define M_ImmediateTextRegion 6
#define M_ImmediateLosslessTextRegion 7
#define M_PatternDictionary 16
#define M_IntermediateHalftoneRegion 20
#define M_ImmediateHalftoneRegion 22
#define M_ImmediateLosslessHalftoneRegion 23
#define M_IntermediateGenericRegion 36
#define M_ImmediateGenericRegion 38
#define M_ImmediateLosslessGenericRegion 39
#define M_IntermediateGenericRefinementRegion 40
#define M_ImmediateGenericRefinementRegion 42
#define M_ImmediateLosslessGenericRefinementRegion 43
#define M_PageInformation 48
#define M_EndOfPage 49
#define M_EndOfStripe 50
#define M_EndOfFile 51
#define M_Profiles 52
#define M_Tables 53
#define M_Extension 62

/**********************************************************************/

typedef enum { INITIAL, HAVEINFO, WRITEPDF } PHASE;

typedef struct _LITEM {
    struct _LITEM *prev;
    struct _LITEM *next;
    void *d;                    /* data */
} LITEM;

typedef struct _LIST {
    LITEM *first;
    LITEM *last;
    struct avl_table *tree;
} LIST;

typedef struct _SEGINFO {
    unsigned long segnum;
    boolean isrefered;
    boolean refers;
    unsigned int seghdrflags;   /* set by readseghdr() */
    boolean pageassocsizeflag;  /* set by readseghdr() */
    unsigned int reftosegcount; /* set by readseghdr() */
    unsigned int countofrefered;        /* set by readseghdr() */
    unsigned int fieldlen;      /* set by readseghdr() */
    unsigned int segnumwidth;   /* set by readseghdr() */
    long segpage;               /* set by readseghdr() */
    unsigned long segdatalen;   /* set by readseghdr() */
    off_t hdrstart;             /* set by readseghdr() */
    off_t hdrend;               /* set by readseghdr() */
    off_t datastart;
    off_t dataend;
    boolean endofstripeflag;    /* set by checkseghdrflags() */
    boolean endofpageflag;      /* set by checkseghdrflags() */
    boolean pageinfoflag;       /* set by checkseghdrflags() */
    boolean endoffileflag;      /* set by checkseghdrflags() */
} SEGINFO;

typedef struct _PAGEINFO {
    LIST segments;              /* segments associated with page */
    unsigned long pagenum;
    unsigned int width;
    unsigned int height;
    unsigned int xres;
    unsigned int yres;
    unsigned int pagesegmentflags;
    unsigned int stripinginfo;
    unsigned int stripedheight;
} PAGEINFO;

typedef struct _FILEINFO {
    FILE *file;
    char *filename;
    off_t filesize;
    LIST pages;                 /* not including page0 */
    LIST page0;
    unsigned int filehdrflags;  /* set by readfilehdr() */
    boolean sequentialaccess;   /* set by readfilehdr() */
    unsigned long numofpages;   /* set by readfilehdr() */
    off_t streamstart;          /* set by get_jbig2_info() */
    unsigned long pdfpage0objnum;
    PHASE phase;
} FILEINFO;

/**********************************************************************/

static void initlinkedlist(LIST * lp);
static void checkseghdrflags(SEGINFO * sip);
static void markpage0seg(FILEINFO * fip, unsigned long referedseg);

/**********************************************************************/

struct avl_table *file_tree = NULL;

static int comp_file_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const FILEINFO *) pa)->filename,
                  ((const FILEINFO *) pb)->filename);
}

static int comp_page_entry(const void *pa, const void *pb, void *p)
{
    return ((const PAGEINFO *) pa)->pagenum - ((const PAGEINFO *) pb)->pagenum;
}

static int comp_segment_entry(const void *pa, const void *pb, void *p)
{
    return ((const SEGINFO *) pa)->segnum - ((const SEGINFO *) pb)->segnum;
}

/**********************************************************************/

static int ygetc(FILE * stream)
{
    int c = getc(stream);
    if (c < 0) {
        if (c == EOF)
            pdftex_fail("getc() failed; premature end of JBIG2 image file");
        else
            pdftex_fail("getc() failed (can't happen)");
    }
    return c;
}

/**********************************************************************/

static FILEINFO *new_fileinfo(void)
{
    FILEINFO *fip;
    fip = xtalloc(1, FILEINFO);
    fip->file = NULL;
    fip->filename = NULL;
    fip->filesize = 0;
    initlinkedlist(&(fip->pages));
    initlinkedlist(&(fip->page0));
    fip->filehdrflags = 0;
    fip->sequentialaccess = false;
    fip->numofpages = 0;
    fip->streamstart = 0;
    fip->pdfpage0objnum = 0;
    fip->phase = INITIAL;
    return fip;
}

static PAGEINFO *new_pageinfo(void)
{
    PAGEINFO *pip;
    pip = xtalloc(1, PAGEINFO);
    initlinkedlist(&(pip->segments));
    pip->pagenum = 0;
    pip->width = 0;
    pip->height = 0;
    pip->xres = 0;
    pip->yres = 0;
    pip->pagesegmentflags = 0;
    pip->stripinginfo = 0;
    pip->stripedheight = 0;
    return pip;
}

static void init_seginfo(SEGINFO * sip)
{
    sip->segnum = 0;
    sip->isrefered = false;
    sip->refers = false;
    sip->seghdrflags = 0;
    sip->pageassocsizeflag = false;
    sip->reftosegcount = 0;
    sip->countofrefered = 0;
    sip->fieldlen = 0;
    sip->segnumwidth = 0;
    sip->segpage = 0;
    sip->segdatalen = 0;
    sip->hdrstart = 0;
    sip->hdrend = 0;
    sip->datastart = 0;
    sip->dataend = 0;
    sip->endofstripeflag = false;
    sip->endofpageflag = false;
    sip->pageinfoflag = false;
    sip->endoffileflag = false;
}

/**********************************************************************/

static void initlinkedlist(LIST * lp)
{
    lp->first = NULL;
    lp->last = NULL;
    lp->tree = NULL;
}

static LIST *litem_append(LIST * lp)
{
    LITEM *ip;
    ip = xtalloc(1, LITEM);
    if (lp->first == NULL) {
        lp->first = ip;
        ip->prev = NULL;
    } else {
        lp->last->next = ip;
        ip->prev = lp->last;
    }
    lp->last = ip;
    ip->next = NULL;
    ip->d = NULL;
    return lp;
}

/**********************************************************************/

static void pages_maketree(LIST * plp)
{
    LITEM *ip;
    void **aa;
    assert(plp->tree == NULL);
    plp->tree = avl_create(comp_page_entry, NULL, &avl_xallocator);
    assert(plp->tree != NULL);
    for (ip = plp->first; ip != NULL; ip = ip->next) {
        aa = avl_probe(plp->tree, (PAGEINFO *) ip->d);
        assert(aa != NULL);
    }
}

static void segments_maketree(LIST * slp)
{
    LITEM *ip;
    void **aa;
    assert(slp->tree == NULL);
    slp->tree = avl_create(comp_segment_entry, NULL, &avl_xallocator);
    assert(slp->tree != NULL);
    for (ip = slp->first; ip != NULL; ip = ip->next) {
        aa = avl_probe(slp->tree, (SEGINFO *) ip->d);
        assert(aa != NULL);
    }
}

/**********************************************************************/

static PAGEINFO *find_pageinfo(LIST * plp, unsigned long pagenum)
{
    PAGEINFO tmp;
    tmp.pagenum = pagenum;
    assert(plp->tree != NULL);
    return (PAGEINFO *) avl_find(plp->tree, &tmp);
}

static SEGINFO *find_seginfo(LIST * slp, unsigned long segnum)
{
    SEGINFO tmp;
    tmp.segnum = segnum;
    assert(slp->tree != NULL);
    return (SEGINFO *) avl_find(slp->tree, &tmp);
}

/**********************************************************************/

static unsigned int read2bytes(FILE * f)
{
    unsigned int c = ygetc(f);
    return (c << 8) + ygetc(f);
}

static unsigned long read4bytes(FILE * f)
{
    unsigned int l = read2bytes(f);
    return (l << 16) + read2bytes(f);
}

/**********************************************************************/

static off_t getstreamlen(LITEM * slip, boolean refer)
{
    SEGINFO *sip;
    off_t len = 0;
    for (; slip != NULL; slip = slip->next) {
        sip = slip->d;
        if (refer || sip->isrefered)
            len += (sip->hdrend - sip->hdrstart) + (sip->dataend - sip->datastart);
    }
    return len;
}

/**********************************************************************/

static void readfilehdr(FILEINFO * fip)
{
    unsigned int i;
    /* Annex D.4 File header syntax */
    /* Annex D.4.1 ID string */
    unsigned char jbig2_id[] = { 0x97, 'J', 'B', '2', 0x0d, 0x0a, 0x1a, 0x0a };
    xfseeko(fip->file, (off_t)0, SEEK_SET, fip->filename);
    for (i = 0; i < 8; i++)
        if (ygetc(fip->file) != jbig2_id[i])
            pdftex_fail
                ("readfilehdr(): reading JBIG2 image file failed: ID string missing");
    /* Annex D.4.2 File header flags */
    fip->filehdrflags = ygetc(fip->file);
    fip->sequentialaccess = (fip->filehdrflags & 0x01) ? true : false;
    if (fip->sequentialaccess) {        /* Annex D.1 vs. Annex D.2 */
        xfseeko(fip->file, (off_t)0, SEEK_END, fip->filename);
        fip->filesize = xftello(fip->file, fip->filename);
        xfseeko(fip->file, (off_t)9, SEEK_SET, fip->filename);
    }
    /* Annex D.4.3 Number of pages */
    if (!(fip->filehdrflags >> 1) & 0x01)       /* known number of pages */
        fip->numofpages = read4bytes(fip->file);
    /* --- at end of file header --- */
}

/**********************************************************************/
/* for first reading of file; return value tells if header been read */

static boolean readseghdr(FILEINFO * fip, SEGINFO * sip)
{
    unsigned int i;
    sip->hdrstart = xftello(fip->file, fip->filename);
    if (fip->sequentialaccess && sip->hdrstart == fip->filesize)
        return false;           /* no endoffileflag is ok for sequentialaccess */
    /* 7.2.2 Segment number */
    sip->segnum = read4bytes(fip->file);
    /* 7.2.3 Segment header flags */
    sip->seghdrflags = ygetc(fip->file);
    checkseghdrflags(sip);
    if (fip->sequentialaccess && sip->endoffileflag)    /* accept shorter segment, */
        return true;            /* makes it compliant with Example 3.4 of PDFRef. 5th ed. */
    sip->pageassocsizeflag = ((sip->seghdrflags >> 6) & 0x01) ? true : false;
    /* 7.2.4 Referred-to segment count and retention flags */
    sip->reftosegcount = (unsigned int) ygetc(fip->file);
    sip->countofrefered = sip->reftosegcount >> 5;
    if (sip->countofrefered < 5)
        sip->fieldlen = 1;
    else {
        sip->fieldlen = 5 + sip->countofrefered / 8;
        xfseeko(fip->file, (off_t) sip->fieldlen - 1, SEEK_CUR, fip->filename);
    }
    /* 7.2.5 Referred-to segment numbers */
    if (sip->segnum <= 256)
        sip->segnumwidth = 1;
    else if (sip->segnum <= 65536)
        sip->segnumwidth = 2;
    else
        sip->segnumwidth = 4;
    for (i = 0; i < sip->countofrefered; i++) {
        switch (sip->segnumwidth) {
        case 1:
            (void) ygetc(fip->file);
            break;
        case 2:
            (void) read2bytes(fip->file);
            break;
        case 4:
            (void) read4bytes(fip->file);
            break;
        }
    }
    /* 7.2.6 Segment page association */
    if (sip->pageassocsizeflag)
        sip->segpage = read4bytes(fip->file);
    else
        sip->segpage = ygetc(fip->file);
    /* 7.2.7 Segment data length */
    sip->segdatalen = read4bytes(fip->file);
    sip->hdrend = xftello(fip->file, fip->filename);
    /* ---- at end of segment header ---- */
    return true;
}

/**********************************************************************/
/* for writing, marks refered page0 segments, sets segpage > 0 to 1 */

static void writeseghdr(FILEINFO * fip, SEGINFO * sip)
{
    unsigned int i;
    unsigned long referedseg = 0;
    /* 7.2.2 Segment number */
    /* 7.2.3 Segment header flags */
    /* 7.2.4 Referred-to segment count and retention flags */
    for (i = 0; i < 5 + sip->fieldlen; i++)
        pdfout(ygetc(fip->file));
    /* 7.2.5 Referred-to segment numbers */
    for (i = 0; i < sip->countofrefered; i++) {
        switch (sip->segnumwidth) {
        case 1:
            referedseg = ygetc(fip->file);
            pdfout(referedseg);
            break;
        case 2:
            referedseg = read2bytes(fip->file);
            pdfout((referedseg >> 8) & 0xff);
            pdfout(referedseg & 0xff);
            break;
        case 4:
            referedseg = read4bytes(fip->file);
            pdfout((referedseg >> 24) & 0xff);
            pdfout((referedseg >> 16) & 0xff);
            pdfout((referedseg >> 8) & 0xff);
            pdfout(referedseg & 0xff);
            break;
        }
        if (fip->page0.last != NULL && !sip->refers)
            markpage0seg(fip, referedseg);
    }
    if (sip->countofrefered > 0)
        sip->refers = true;
    /* 7.2.6 Segment page association */
    if (sip->pageassocsizeflag)
        for (i = 0; i < 3; i++) {
            (void) ygetc(fip->file);
            pdfout(0);
        }
    (void) ygetc(fip->file);
    pdfout((sip->segpage > 0) ? 1 : 0);
    /* 7.2.7 Segment data length */
    for (i = 0; i < 4; i++)
        pdfout(ygetc(fip->file));
    /* ---- at end of segment header ---- */
}

/**********************************************************************/
/* for recursive marking of refered page0 segments */

static void checkseghdr(FILEINFO * fip, SEGINFO * sip)
{
    unsigned int i;
    unsigned long referedseg = 0;
    /* 7.2.2 Segment number */
    /* 7.2.3 Segment header flags */
    /* 7.2.4 Referred-to segment count and retention flags */
    xfseeko(fip->file, (off_t) sip->fieldlen + 5, SEEK_CUR, fip->filename);
    /* 7.2.5 Referred-to segment numbers */
    for (i = 0; i < sip->countofrefered; i++) {
        switch (sip->segnumwidth) {
        case 1:
            referedseg = ygetc(fip->file);
            break;
        case 2:
            referedseg = read2bytes(fip->file);
            break;
        case 4:
            referedseg = read4bytes(fip->file);
            break;
        }
        if (!sip->refers)
            markpage0seg(fip, referedseg);
    }
    if (sip->countofrefered > 0)
        sip->refers = true;
    /* 7.2.6 Segment page association */
    /* 7.2.7 Segment data length */
    if (sip->pageassocsizeflag)
        xfseeko(fip->file, 8, SEEK_CUR, fip->filename);
    else
        xfseeko(fip->file, 5, SEEK_CUR, fip->filename);
    /* ---- at end of segment header ---- */
}

/**********************************************************************/

static void checkseghdrflags(SEGINFO * sip)
{
    sip->endofstripeflag = false;
    sip->endofpageflag = false;
    sip->pageinfoflag = false;
    sip->endoffileflag = false;
    /* 7.3 Segment types */
    switch (sip->seghdrflags & 0x3f) {
    case M_SymbolDictionary:
    case M_IntermediateTextRegion:
    case M_ImmediateTextRegion:
    case M_ImmediateLosslessTextRegion:
    case M_PatternDictionary:
    case M_IntermediateHalftoneRegion:
    case M_ImmediateHalftoneRegion:
    case M_ImmediateLosslessHalftoneRegion:
    case M_IntermediateGenericRegion:
    case M_ImmediateGenericRegion:
    case M_ImmediateLosslessGenericRegion:
    case M_IntermediateGenericRefinementRegion:
    case M_ImmediateGenericRefinementRegion:
    case M_ImmediateLosslessGenericRefinementRegion:
        break;
    case M_PageInformation:
        sip->pageinfoflag = true;
        break;
    case M_EndOfPage:
        sip->endofpageflag = true;
        break;
    case M_EndOfStripe:
        sip->endofstripeflag = true;
        break;
    case M_EndOfFile:
        sip->endoffileflag = true;
        break;
    case M_Profiles:
    case M_Tables:
    case M_Extension:
        break;
    default:
        pdftex_fail
            ("checkseghdrflags(): unknown segment type in JBIG2 image file");
        break;
    }
}

/**********************************************************************/

static void markpage0seg(FILEINFO * fip, unsigned long referedseg)
{
    PAGEINFO *pip;
    SEGINFO *sip;
    pip = fip->page0.first->d;
    sip = find_seginfo(&(pip->segments), referedseg);
    if (sip != NULL) {
        if (!sip->refers && sip->countofrefered > 0)
            checkseghdr(fip, sip);
        sip->isrefered = true;
    }
}

/**********************************************************************/

static off_t findstreamstart(FILEINFO * fip)
{
    SEGINFO tmp;
    assert(!fip->sequentialaccess);     /* D.2 Random-access organisation */
    do                          /* find random-access stream start */
        (void) readseghdr(fip, &tmp);
    while (!tmp.endoffileflag);
    fip->streamstart = tmp.hdrend;
    readfilehdr(fip);
    return fip->streamstart;
}

/**********************************************************************/

static void rd_jbig2_info(FILEINFO * fip)
{
    off_t seekdist = 0;         /* for sequential-access only */
    off_t streampos = 0;        /* for random-access only */
    unsigned long currentpage = 0;
    boolean sipavail = false;
    PAGEINFO *pip;
    SEGINFO *sip = NULL;
    LIST *plp, *slp;
    fip->file = xfopen(fip->filename, FOPEN_RBIN_MODE);
    readfilehdr(fip);
    if (!fip->sequentialaccess) /* D.2 Random-access organisation */
        streampos = findstreamstart(fip);
    while (true) {              /* loop over segments */
        if (!sipavail) {
            sip = xtalloc(1, SEGINFO);
            sipavail = true;
        }
        init_seginfo(sip);
        if (!readseghdr(fip, sip) || sip->endoffileflag)
            break;
        if (sip->segpage > 0) {
            if (sip->segpage > currentpage) {
                plp = litem_append(&(fip->pages));
                plp->last->d = new_pageinfo();
                currentpage = sip->segpage;
            }
            pip = fip->pages.last->d;
        } else {
            if (fip->page0.last == NULL) {
                plp = litem_append(&(fip->page0));
                plp->last->d = new_pageinfo();
            }
            pip = fip->page0.last->d;
        }
        if (!sip->endofpageflag) {
            slp = litem_append(&(pip->segments));
            slp->last->d = sip;
            sipavail = false;
        }
        if (!fip->sequentialaccess)
            sip->datastart = streampos;
        else
            sip->datastart = sip->hdrend;
        sip->dataend = sip->datastart + sip->segdatalen;
        if (!fip->sequentialaccess
            && (sip->pageinfoflag || sip->endofstripeflag))
            xfseeko(fip->file, sip->datastart, SEEK_SET, fip->filename);
        seekdist = (off_t) sip->segdatalen;
        /* 7.4.8 Page information segment syntax */
        if (sip->pageinfoflag) {
            pip->pagenum = sip->segpage;
            pip->width = read4bytes(fip->file);
            pip->height = read4bytes(fip->file);
            pip->xres = read4bytes(fip->file);
            pip->yres = read4bytes(fip->file);
            pip->pagesegmentflags = ygetc(fip->file);
            /* 7.4.8.6 Page striping information */
            pip->stripinginfo = read2bytes(fip->file);
            seekdist -= 19;
        }
        if (sip->endofstripeflag) {
            pip->stripedheight = read4bytes(fip->file);
            seekdist -= 4;
        }
        if (!fip->sequentialaccess
            && (sip->pageinfoflag || sip->endofstripeflag))
            xfseeko(fip->file, sip->hdrend, SEEK_SET, fip->filename);
        if (!fip->sequentialaccess)
            streampos += sip->segdatalen;
        if (fip->sequentialaccess)
            xfseeko(fip->file, seekdist, SEEK_CUR, fip->filename);
        if (sip->endofpageflag && currentpage && (pip->stripinginfo >> 15))
            pip->height = pip->stripedheight;
    }
    fip->phase = HAVEINFO;
    if (sipavail)
        xfree(sip);
    xfclose(fip->file, fip->filename);
}

/**********************************************************************/

static void wr_jbig2(FILEINFO * fip, unsigned long page)
{
    LITEM *slip;
    PAGEINFO *pip;
    SEGINFO *sip;
    off_t i;
    if (page > 0) {
        pip = find_pageinfo(&(fip->pages), page);
        assert(pip != NULL);
        pdf_puts("/Type /XObject\n");
        pdf_puts("/Subtype /Image\n");
        pdf_printf("/Width %i\n", pip->width);
        pdf_printf("/Height %i\n", pip->height);
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdf_puts("/BitsPerComponent 1\n");
        pdf_printf("/Length %" LONGINTEGER_PRI "d\n",
                   (LONGINTEGER_TYPE) getstreamlen(pip->segments.first, true));
        pdf_puts("/Filter [/JBIG2Decode]\n");
        if (fip->page0.last != NULL) {
            if (fip->pdfpage0objnum == 0) {
                pdfcreateobj(0, 0);
                fip->pdfpage0objnum = objptr;
            }
            pdf_printf("/DecodeParms [<< /JBIG2Globals %lu 0 R >>]\n",
                       fip->pdfpage0objnum);
        }
    } else {
        pip = find_pageinfo(&(fip->page0), page);
        assert(pip != NULL);
        pdfbegindict(fip->pdfpage0objnum, 0);
        pdf_printf("/Length %" LONGINTEGER_PRI "d\n",
                   (LONGINTEGER_TYPE) getstreamlen(pip->segments.first, false));
    }
    pdf_puts(">>\n");
    pdf_puts("stream\n");
    fip->file = xfopen(fip->filename, FOPEN_RBIN_MODE);
    for (slip = pip->segments.first; slip != NULL; slip = slip->next) { /* loop over page segments */
        sip = slip->d;
        if (sip->isrefered || page > 0) {
            xfseeko(fip->file, sip->hdrstart, SEEK_SET, fip->filename);
            /* mark refered-to page 0 segments, change segpages > 1 to 1 */
            writeseghdr(fip, sip);
            xfseeko(fip->file, sip->datastart, SEEK_SET, fip->filename);
            for (i = sip->datastart; i < sip->dataend; i++)
                pdfout(ygetc(fip->file));
        }
    }
    pdfendstream();
    xfclose(fip->file, fip->filename);
}

/**********************************************************************/

void read_jbig2_info(integer img)
{
    FILEINFO *fip, tmp;
    PAGEINFO *pip;
    void **aa;
    if (jbig2_ptr(img)->selected_page < 1)
        pdftex_fail
            ("read_jbig2_info(): page %d not in JBIG2 image file; page must be > 0",
             (int) jbig2_ptr(img)->selected_page);
    if (file_tree == NULL) {
        file_tree = avl_create(comp_file_entry, NULL, &avl_xallocator);
        assert(file_tree != NULL);
    }
    tmp.filename = img_name(img);
    fip = (FILEINFO *) avl_find(file_tree, &tmp);
    if (fip == NULL) {
        fip = new_fileinfo();
        fip->filename = xstrdup(img_name(img));
        aa = avl_probe(file_tree, fip);
        assert(aa != NULL);
    }
    if (fip->phase == INITIAL) {
        rd_jbig2_info(fip);
        pages_maketree(&(fip->pages));
        if (fip->page0.last != NULL) {
            pages_maketree(&(fip->page0));
            pip = fip->page0.first->d;
            segments_maketree(&(pip->segments));
        }
    }
    pip = find_pageinfo(&(fip->pages), jbig2_ptr(img)->selected_page);
    if (pip == NULL)
        pdftex_fail("read_jbig2_info(): page %d not found in JBIG2 image file",
                    (int) jbig2_ptr(img)->selected_page);
    img_pages(img) = fip->numofpages;
    img_width(img) = pip->width;
    img_height(img) = pip->height;
    img_xres(img) = (int) (pip->xres * 0.0254 + 0.5);
    img_yres(img) = (int) (pip->yres * 0.0254 + 0.5);
}

/**********************************************************************/

void write_jbig2(integer img)
{
    FILEINFO *fip, tmp;
    PAGEINFO *pip;
    assert(file_tree != NULL);
    tmp.filename = img_name(img);
    fip = (FILEINFO *) avl_find(file_tree, &tmp);
    assert(fip != NULL);
    assert(fip->phase == HAVEINFO);     /* don't write before rd_jbig2_info() call */
    pip = find_pageinfo(&(fip->pages), jbig2_ptr(img)->selected_page);
    assert(pip != NULL);
    wr_jbig2(fip, pip->pagenum);
}

/**********************************************************************/

void flushjbig2page0objects(void)
{
    FILEINFO *fip;
    struct avl_traverser t;
    if (file_tree != NULL) {
        avl_t_init(&t, file_tree);
        for (fip = avl_t_first(&t, file_tree); fip != NULL;
             fip = avl_t_next(&t)) {
            if (fip->page0.last != NULL)
                wr_jbig2(fip, 0);
        }
    }
}

/**********************************************************************/
