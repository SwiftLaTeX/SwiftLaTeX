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
#include "writettf.h"
#include <string.h>

#define DEFAULT_NTABS       14
#define NEW_CMAP_SIZE       2

#define ttf_putchar     fb_putchar
#define ttf_offset      fb_offset
#define ttf_seek_outbuf fb_seek

#define INFILE ttf_file

#define ttf_open()      \
    open_input(&ttf_file, kpse_truetype_format, FOPEN_RBIN_MODE)
#define ttf_close()     xfclose(ttf_file, cur_file_name)
#define ttf_getchar()   xgetc(ttf_file)
#define ttf_eof()       feof(ttf_file)

typedef struct {
    TTF_LONG offset;
    TTF_LONG newoffset;
    TTF_UFWORD advWidth;
    TTF_FWORD lsb;
    const char *name;           /* name of glyph */
    TTF_SHORT newindex;         /* new index of glyph in output file */
    TTF_USHORT name_index;      /* index of name as read from font file */
} glyph_entry;

typedef struct {
    const char *name;           /* name of glyph */
    long code;                  /* charcode in case of subfonts */
    long newindex;              /* new index of glyph in output file */
} ttfenc_entry;

typedef struct {
    TTF_USHORT platform_id;
    TTF_USHORT encoding_id;
    TTF_USHORT language_id;
    TTF_USHORT name_id;
    TTF_USHORT length;
    TTF_USHORT offset;
    TTF_USHORT new_offset;
    TTF_USHORT new_length;
} name_record;

typedef struct {
    char *ttf_name;
    TTF_USHORT pid;
    TTF_USHORT eid;
    long *table;
} ttf_cmap_entry;

static TTF_USHORT ntabs;
static TTF_USHORT upem;
static TTF_FIXED post_format;
static TTF_SHORT loca_format;
static TTF_ULONG last_glyf_offset;
static TTF_USHORT glyphs_count;
static TTF_USHORT new_glyphs_count;
static TTF_USHORT nhmtxs;
static TTF_USHORT new_ntabs;

static glyph_entry *glyph_tab;
static long *glyph_index;
static cmap_entry *cmap_tab, new_cmap_tab[NEW_CMAP_SIZE];
static name_record *name_tab;
static int name_record_num;
static char *name_buf;
static int name_buf_size;
static dirtab_entry *dir_tab;
static char *glyph_name_buf;
static TTF_ULONG checksum;
static TTF_ULONG tab_length;
static TTF_ULONG tmp_ulong;
static TTF_ULONG checkSumAdjustment_offset;
static FILE *ttf_file;
static ttfenc_entry ttfenc_tab[256];

static fd_entry *fd_cur;        /* pointer to the current font descriptor */

static struct avl_table *ttf_cmap_tree = NULL;

integer ttf_length;

#include "macnames.c"

static const char *newtabnames[] = {
    "OS/2",
    "PCLT",
    "cmap",
    "cvt ",
    "fpgm",
    "glyf",
    "head",
    "hhea",
    "hmtx",
    "loca",
    "maxp",
    "name",
    "post",
    "prep"
};

static ttf_cmap_entry *new_ttf_cmap_entry(void)
{
    ttf_cmap_entry *e;
    e = xtalloc(1, ttf_cmap_entry);
    e->ttf_name = NULL;
    e->table = NULL;
    return e;
}

static void destroy_ttf_cmap_entry(void *pa, void *pb)
{
    ttf_cmap_entry *p;
    p = (ttf_cmap_entry *) pa;
    xfree(p->ttf_name);
    xfree(p->table);
    xfree(p);
}

void ttf_free(void)
{
    if (ttf_cmap_tree != NULL)
        avl_destroy(ttf_cmap_tree, destroy_ttf_cmap_entry);
}

static int comp_ttf_cmap_entry(const void *pa, const void *pb, void *p)
{
    const ttf_cmap_entry *p1 = (const ttf_cmap_entry *) pa,
        *p2 = (const ttf_cmap_entry *) pb;
    int i;
    assert(p1->ttf_name != NULL && p2->ttf_name != NULL);
    if ((i = strcmp(p1->ttf_name, p2->ttf_name)) != 0)
        return i;
    cmp_return(p1->pid, p2->pid);
    cmp_return(p1->eid, p2->eid);
    return 0;
}

static unsigned char ttf_addchksm(unsigned char b)
{
    tmp_ulong = (tmp_ulong << 8) + b;
    tab_length++;
    if (tab_length % 4 == 0) {
        checksum += tmp_ulong;
        tmp_ulong = 0;
    }
    return b;
}

static TTF_ULONG ttf_getchksm(void)
{
    while (tab_length % 4 != 0)
        ttf_putchar(ttf_addchksm(0));   /* ttf_addchksm updates tab_length */
    return checksum;
}

static long ttf_putnum(int s, long n)
{
    long i = n;
    char buf[TTF_LONG_SIZE + 1], *p = buf;
    while (s-- > 0) {
        *p++ = i & 0xFF;
        i >>= 8;
    }
    p--;
    while (p >= buf)
        ttf_putchar(ttf_addchksm(*p--));
    return n;
}

static long ttf_getnum(int s)
{
    long i = 0;
    int c;
    while (s > 0) {
        if ((c = ttf_getchar()) < 0)
            pdftex_fail("unexpected EOF");
        i = (i << 8) + c;
        s--;
    }
    return i;
}

static long ttf_funit(long n)
{
    if (n < 0)
        return -((-n / upem) * 1000 + ((-n % upem) * 1000) / upem);
    else
        return (n / upem) * 1000 + ((n % upem) * 1000) / upem;
}

static void ttf_ncopy(int n)
{
    while (n-- > 0)
        copy_byte();
}

static dirtab_entry *ttf_name_lookup(const char *s, boolean required)
{
    dirtab_entry *tab;
    for (tab = dir_tab; tab - dir_tab < ntabs; tab++)
        if (strncmp(tab->tag, s, 4) == 0)
            break;
    if (tab - dir_tab == ntabs) {
        if (required)
            pdftex_fail("can't find table `%s'", s);
        else
            tab = NULL;
    }
    return tab;
}

static dirtab_entry *ttf_seek_tab(const char *name, TTF_LONG offset)
{
    dirtab_entry *tab = ttf_name_lookup(name, true);
    xfseek(INFILE, tab->offset + offset, SEEK_SET, cur_file_name);
    return tab;
}

static void ttf_seek_off(TTF_LONG offset)
{
    xfseek(INFILE, offset, SEEK_SET, cur_file_name);
}

static void ttf_copy_encoding(void)
{
    int i, *q;
    void **aa;
    char **glyph_names;
    long *charcodes;
    static char buf[SMALL_BUF_SIZE];
    struct avl_traverser t;
    ttfenc_entry *e = ttfenc_tab;

    assert(fd_cur->tx_tree != NULL);    /* this must be set in create_fontdictionary */

    if (fd_cur->fe != NULL) {
        glyph_names = fd_cur->fe->glyph_names;
        assert(glyph_names != NULL);

        for (i = 0; i < 256; i++)
            ttfenc_tab[i].name = notdef;

        /* a workaround for a bug of AcroReader 4.0 */
        if (strcmp(glyph_names[97], "a") == 0) {
            q = xtalloc(1, int);
            *q = 'a';
            aa = avl_probe(fd_cur->tx_tree, q);
            assert(aa != NULL);
        }
        /* end of workaround */

        /* take over collected characters from TeX, reencode them */
        avl_t_init(&t, fd_cur->tx_tree);
        for (q = (int *) avl_t_first(&t, fd_cur->tx_tree); q != NULL;
             q = (int *) avl_t_next(&t)) {
            assert(*q >= 0 && *q < 256);
            ttfenc_tab[*q].name = glyph_names[*q];
        }
        make_subset_tag(fd_cur);
    } else if (is_subfont(fd_cur->fm)) {
        charcodes = fd_cur->fm->subfont->charcodes;
        assert(charcodes != NULL);

        for (i = 0; i < 256; i++)
            ttfenc_tab[i].code = -1;

        /* take over collected characters from TeX */
        avl_t_init(&t, fd_cur->tx_tree);
        for (q = (int *) avl_t_first(&t, fd_cur->tx_tree); q != NULL;
             q = (int *) avl_t_next(&t)) {
            assert(*q >= 0 && *q < 256);
            e = ttfenc_tab + *q;
            e->code = charcodes[*q];
            if (e->code == -1)
                pdftex_warn
                    ("character %i in subfont %s is not mapped to any charcode",
                     *q, fd_cur->fm->tfm_name);
            else {
                assert(e->code < 0x10000);
                sprintf(buf, "/c%4.4X", (int) e->code);
                aa = avl_probe(fd_cur->gl_tree, xstrdup(buf));
                assert(aa != NULL);
            }
        }
        make_subset_tag(fd_cur);
    } else
        assert(0);
}

#define ttf_append_byte(B)\
do {\
    if (name_tab[i].platform_id == 3)\
        *q++ = 0;\
    *q++ = B;\
} while (0)

static char *strip_spaces_and_delims(char *s, int l)
{
    static char buf[SMALL_BUF_SIZE];
    char *p = buf;
    int i;

    assert(l >= 0 && l < (int) sizeof(buf));

    for (i = 0; i < l; s++, i++) {
        if (*s == '(' || *s == ')' || *s == '<' || *s == '>' ||
            *s == '[' || *s == ']' || *s == '{' || *s == '}' ||
            *s == '/' || *s == '%' || isspace((unsigned char)*s))
            continue;
        *p++ = *s;
    }
    *p = 0;
    return buf;
}

static void ttf_read_name(void)
{
    int i, j;
    dirtab_entry *tab = ttf_seek_tab("name", TTF_USHORT_SIZE);
    char *p, buf[SMALL_BUF_SIZE];
    name_record_num = get_ushort();
    name_tab = xtalloc(name_record_num, name_record);
    name_buf_size = tab->length -
        (3 * TTF_USHORT_SIZE + name_record_num * 6 * TTF_USHORT_SIZE);
    name_buf = xtalloc(name_buf_size, char);
    ttf_skip(TTF_USHORT_SIZE);
    for (i = 0; i < name_record_num; i++) {
        name_tab[i].platform_id = get_ushort();
        name_tab[i].encoding_id = get_ushort();
        name_tab[i].language_id = get_ushort();
        name_tab[i].name_id = get_ushort();
        name_tab[i].length = get_ushort();
        name_tab[i].offset = get_ushort();
    }
    for (p = name_buf; p - name_buf < name_buf_size; p++)
        *p = get_char();
    /* look for PS font name */
    for (i = 0; i < name_record_num; i++) {
        if (name_tab[i].platform_id == 1 &&
            name_tab[i].encoding_id == 0 && name_tab[i].name_id == 6) {
            xfree(fd_cur->fontname);
            fd_cur->fontname =
                xstrdup(strip_spaces_and_delims(name_buf + name_tab[i].offset,
                                                name_tab[i].length));
            fd_cur->font_dim[FONTNAME_CODE].set = true;
            break;
        }
    }
    if (!fd_cur->font_dim[FONTNAME_CODE].set) {
        for (i = 0; i < name_record_num; i++) {
            if (name_tab[i].platform_id == 3 &&
                (name_tab[i].encoding_id == 0 || name_tab[i].encoding_id == 1) && 
                name_tab[i].name_id == 6) {
                xfree(fd_cur->fontname);
                assert(name_tab[i].length < sizeof(buf));
                for (j = 0, p = buf; j < name_tab[i].length; j += 2)
                    *p++ = name_buf[name_tab[i].offset + j + 1];
                *p = 0;
                fd_cur->fontname = xstrdup(strip_spaces_and_delims(buf, strlen(buf)));
                fd_cur->font_dim[FONTNAME_CODE].set = true;
                break;
            }
        }
    }
}

static void ttf_read_mapx(void)
{
    glyph_entry *glyph;
    ttf_seek_tab("maxp", TTF_FIXED_SIZE);
    glyph_tab = xtalloc(1 + (glyphs_count = get_ushort()), glyph_entry);
    for (glyph = glyph_tab; glyph - glyph_tab < glyphs_count; glyph++) {
        glyph->newindex = -1;
        glyph->newoffset = 0;
        glyph->name_index = 0;
        glyph->name = notdef;
    }
    glyph_index = xtalloc(glyphs_count + 1, long);
    glyph_index[0] = 0;         /* index of ".notdef" glyph */
    glyph_index[1] = 1;         /* index of ".null" glyph */
}

static void ttf_read_head(void)
{
    ttf_seek_tab("head",
                 2 * TTF_FIXED_SIZE + 2 * TTF_ULONG_SIZE + TTF_USHORT_SIZE);
    upem = get_ushort();
    ttf_skip(16);
    fd_cur->font_dim[FONTBBOX1_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[FONTBBOX2_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[FONTBBOX3_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[FONTBBOX4_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[FONTBBOX1_CODE].set = true;
    fd_cur->font_dim[FONTBBOX2_CODE].set = true;
    fd_cur->font_dim[FONTBBOX3_CODE].set = true;
    fd_cur->font_dim[FONTBBOX4_CODE].set = true;
    ttf_skip(2 * TTF_USHORT_SIZE + TTF_SHORT_SIZE);
    loca_format = get_short();
}

static void ttf_read_hhea(void)
{
    ttf_seek_tab("hhea", TTF_FIXED_SIZE);
    fd_cur->font_dim[ASCENT_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[DESCENT_CODE].val = ttf_funit(get_fword());
    fd_cur->font_dim[ASCENT_CODE].set = true;
    fd_cur->font_dim[DESCENT_CODE].set = true;
    ttf_skip(TTF_FWORD_SIZE + TTF_UFWORD_SIZE + 3 * TTF_FWORD_SIZE +
             8 * TTF_SHORT_SIZE);
    nhmtxs = get_ushort();
}

static void ttf_read_pclt(void)
{
    if (ttf_name_lookup("PCLT", false) == NULL)
        return;
    ttf_seek_tab("PCLT", TTF_FIXED_SIZE + TTF_ULONG_SIZE + TTF_USHORT_SIZE);
    fd_cur->font_dim[XHEIGHT_CODE].val = ttf_funit(get_ushort());
    ttf_skip(2 * TTF_USHORT_SIZE);
    fd_cur->font_dim[CAPHEIGHT_CODE].val = ttf_funit(get_ushort());
    fd_cur->font_dim[XHEIGHT_CODE].set = true;
    fd_cur->font_dim[CAPHEIGHT_CODE].set = true;
}

static void ttf_read_hmtx(void)
{
    glyph_entry *glyph;
    TTF_UFWORD last_advWidth;
    ttf_seek_tab("hmtx", 0);
    for (glyph = glyph_tab; glyph - glyph_tab < nhmtxs; glyph++) {
        glyph->advWidth = get_ufword();
        glyph->lsb = get_ufword();
    }
    if (nhmtxs < glyphs_count) {
        last_advWidth = glyph[-1].advWidth;
        for (; glyph - glyph_tab < glyphs_count; glyph++) {
            glyph->advWidth = last_advWidth;
            glyph->lsb = get_ufword();
        }
    }
}

static void ttf_read_post(void)
{
    int k, nnames;
    long length;
    long int_part, frac_part;
    int sign = 1;
    TTF_FIXED italic_angle;
    char *p;
    glyph_entry *glyph;
    const dirtab_entry *tab = ttf_seek_tab("post", 0);
    post_format = get_fixed();
    italic_angle = get_fixed();
    int_part = italic_angle >> 16;
    if (int_part > 0x7FFF) {    /* a negative number */
        int_part = 0x10000 - int_part;
        sign = -1;
    }
    frac_part = italic_angle % 0x10000;
    fd_cur->font_dim[ITALIC_ANGLE_CODE].val =
        sign * (int_part + frac_part * 1.0 / 0x10000);
    fd_cur->font_dim[ITALIC_ANGLE_CODE].set = true;
    if (glyph_tab == NULL)
        return;                 /* being called from writeotf() */
    ttf_skip(2 * TTF_FWORD_SIZE + 5 * TTF_ULONG_SIZE);
    switch (post_format) {
    case 0x10000:
        for (glyph = glyph_tab; glyph - glyph_tab < NMACGLYPHS; glyph++) {
            glyph->name = mac_glyph_names[glyph - glyph_tab];
            glyph->name_index = glyph - glyph_tab;
        }
        break;
    case 0x20000:
        nnames = get_ushort();  /* some fonts have this value different from nglyphs */
        for (glyph = glyph_tab; glyph - glyph_tab < nnames; glyph++)
            glyph->name_index = get_ushort();
        length = tab->length - (xftell(INFILE, cur_file_name) - tab->offset);
        glyph_name_buf = xtalloc(length, char);
        for (p = glyph_name_buf; p - glyph_name_buf < length;) {
            for (k = get_byte(); k > 0; k--)
                *p++ = get_char();
            *p++ = 0;
        }
        for (glyph = glyph_tab; glyph - glyph_tab < nnames; glyph++) {
            if (glyph->name_index < NMACGLYPHS)
                glyph->name = mac_glyph_names[glyph->name_index];
            else {
                p = glyph_name_buf;
                k = glyph->name_index - NMACGLYPHS;
                for (; k > 0; k--)
                    p = strend(p) + 1;
                glyph->name = p;
            }
        }
        break;
    default:
        pdftex_warn("unsupported format (%.8X) of `post' table, assuming 3.0",
                    (unsigned int) post_format);
    case 0x00030000:
        for (glyph = glyph_tab; glyph - glyph_tab < NMACGLYPHS; glyph++) {
            glyph->name_index = glyph - glyph_tab;
        }
    }
}

static void ttf_read_loca(void)
{
    glyph_entry *glyph;
    ttf_seek_tab("loca", 0);
    if (loca_format != 0)
        for (glyph = glyph_tab; glyph - glyph_tab < glyphs_count + 1; glyph++)
            glyph->offset = get_ulong();
    else
        for (glyph = glyph_tab; glyph - glyph_tab < glyphs_count + 1; glyph++)
            glyph->offset = get_ushort() << 1;
}

static void ttf_read_tabdir(void)
{
    int i;
    dirtab_entry *tab;
    ttf_skip(TTF_FIXED_SIZE);   /* ignore the sfnt number */
    dir_tab = xtalloc(ntabs = get_ushort(), dirtab_entry);
    ttf_skip(3 * TTF_USHORT_SIZE);
    for (tab = dir_tab; tab - dir_tab < ntabs; tab++) {
        for (i = 0; i < 4; i++)
            tab->tag[i] = get_char();
        tab->checksum = get_ulong();
        tab->offset = get_ulong();
        tab->length = get_ulong();
    }
}

static ttf_cmap_entry *ttf_read_cmap(char *ttf_name, int pid, int eid,
                                     boolean warn)
{
    seg_entry *seg_tab, *s;
    TTF_USHORT *glyphId, format, segCount;
    TTF_USHORT ncmapsubtabs, tmp_pid, tmp_eid;
    TTF_ULONG cmap_offset, tmp_offset;
    long n, i, k, length, index;
    ttf_cmap_entry tmp_e, *p;
    void **aa;

    /* loop up in ttf_cmap_tree first, return if found */
    tmp_e.ttf_name = ttf_name;
    tmp_e.pid = pid;
    tmp_e.eid = eid;
    if (ttf_cmap_tree == NULL) {
        ttf_cmap_tree = avl_create(comp_ttf_cmap_entry, NULL, &avl_xallocator);
        assert(ttf_cmap_tree != NULL);
    }
    p = (ttf_cmap_entry *) avl_find(ttf_cmap_tree, &tmp_e);
    if (p != NULL)
        return p;

    /* not found, have to read it */
    ttf_seek_tab("cmap", TTF_USHORT_SIZE);      /* skip the table version number (=0) */
    ncmapsubtabs = get_ushort();
    cmap_offset = xftell(INFILE, cur_file_name) - 2 * TTF_USHORT_SIZE;
    cmap_tab = xtalloc(ncmapsubtabs, cmap_entry);
    for (i = 0; i < ncmapsubtabs; ++i) {
        tmp_pid = get_ushort();
        tmp_eid = get_ushort();
        tmp_offset = get_ulong();
        if (tmp_pid == pid && tmp_eid == eid) {
            ttf_seek_off(cmap_offset + tmp_offset);
            format = get_ushort();
            if (format == 4)
                goto read_cmap_format_4;
            else {
                if (warn)
                    pdftex_warn("cmap format %i unsupported", format);
                return NULL;
            }
        }
    }
    if (warn)
        pdftex_warn("cannot find cmap subtable for (pid,eid) = (%i, %i)",
                    pid, eid);
    return NULL;
  read_cmap_format_4:
    /* initialize the new entry */
    p = new_ttf_cmap_entry();
    p->ttf_name = xstrdup(ttf_name);
    p->pid = pid;
    p->eid = eid;
    p->table = xtalloc(0x10000, long);
    for (i = 0; i < 0x10000; ++i)
        p->table[i] = -1;       /* unassigned yet */

    /* read the subtable */
    length = get_ushort();      /* length of subtable */
    (void) get_ushort();        /* skip the version number */
    segCount = get_ushort() / 2;
    (void) get_ushort();        /* skip searchRange */
    (void) get_ushort();        /* skip entrySelector */
    (void) get_ushort();        /* skip rangeShift */
    seg_tab = xtalloc(segCount, seg_entry);
    for (s = seg_tab; s - seg_tab < segCount; s++)
        s->endCode = get_ushort();
    (void) get_ushort();        /* skip reversedPad */
    for (s = seg_tab; s - seg_tab < segCount; s++)
        s->startCode = get_ushort();
    for (s = seg_tab; s - seg_tab < segCount; s++)
        s->idDelta = get_ushort();
    for (s = seg_tab; s - seg_tab < segCount; s++)
        s->idRangeOffset = get_ushort();
    length -= 8 * TTF_USHORT_SIZE + 4 * segCount * TTF_USHORT_SIZE;
    n = length / TTF_USHORT_SIZE;       /* number of glyphID's */
    glyphId = xtalloc(n, TTF_USHORT);
    for (i = 0; i < n; i++)
        glyphId[i] = get_ushort();
    for (s = seg_tab; s - seg_tab < segCount; s++) {
        for (i = s->startCode; i <= s->endCode; i++) {
            if (i == 0xFFFF)
                break;
            if (s->idRangeOffset != 0xFFFF) {
                if (s->idRangeOffset == 0)
                    index = (s->idDelta + i) & 0xFFFF;
                else {
                    k = (i - s->startCode) + s->idRangeOffset / 2 +
                        (s - seg_tab) - segCount;
                    assert(k >= 0 && k < n);
                    index = glyphId[k];
                    if (index != 0)
                        index = (index + s->idDelta) & 0xFFFF;
                }
                if (index >= glyphs_count)
                    pdftex_fail("cmap: glyph index %li out of range [0..%i)",
                                index, glyphs_count);
                if (p->table[i] != -1)
                    pdftex_warn
                        ("cmap: multiple glyphs are mapped to unicode %.4lX, "
                         "only %li will be used (glyph %li being ignored)", i,
                         p->table[i], index);
                else
                    p->table[i] = index;
            }
        }
    }
    xfree(seg_tab);
    xfree(glyphId);
    aa = avl_probe(ttf_cmap_tree, p);
    assert(aa != NULL);
    return p;
}

static void ttf_read_font(void)
{
    ttf_read_tabdir();
    if (ttf_name_lookup("PCLT", false) == NULL)
        new_ntabs--;
    if (ttf_name_lookup("fpgm", false) == NULL)
        new_ntabs--;
    if (ttf_name_lookup("cvt ", false) == NULL)
        new_ntabs--;
    if (ttf_name_lookup("prep", false) == NULL)
        new_ntabs--;
    ttf_read_mapx();
    ttf_read_head();
    ttf_read_hhea();
    ttf_read_pclt();
    ttf_read_hmtx();
    ttf_read_post();
    ttf_read_loca();
    ttf_read_name();
}

static void ttf_reset_chksm(dirtab_entry * tab)
{
    checksum = 0;
    tab_length = 0;
    tmp_ulong = 0;
    tab->offset = ttf_offset();
    if (tab->offset % 4 != 0)
        pdftex_warn("offset of `%4.4s' is not a multiple of 4", tab->tag);
}


static void ttf_set_chksm(dirtab_entry * tab)
{
    tab->length = ttf_offset() - tab->offset;
    tab->checksum = ttf_getchksm();
}

static void ttf_copytab(const char *name)
{
    long i;
    dirtab_entry *tab = ttf_seek_tab(name, 0);
    ttf_reset_chksm(tab);
    for (i = tab->length; i > 0; i--)
        copy_char();
    ttf_set_chksm(tab);
}

#define BYTE_ENCODING_LENGTH  \
    ((256)*TTF_BYTE_SIZE + 3*TTF_USHORT_SIZE)

static void ttf_byte_encoding(void)
{
    ttfenc_entry *e;
    (void) put_ushort(0);       /* format number (0: byte encoding table) */
    (void) put_ushort(BYTE_ENCODING_LENGTH);    /* length of table */
    (void) put_ushort(0);       /* version number */
    for (e = ttfenc_tab; e - ttfenc_tab < 256; e++)
        if (e->newindex < 256) {
            put_byte(e->newindex);
        } else {
            if (e->name != notdef)
                pdftex_warn
                    ("glyph `%s' has been mapped to `%s' in `ttf_byte_encoding' cmap table",
                     e->name, notdef);
            put_byte(0);        /* notdef */
        }
}

#define TRIMMED_TABLE_MAP_LENGTH (TTF_USHORT_SIZE*(5 + (256)))

static void ttf_trimmed_table_map(void)
{
    ttfenc_entry *e;
    (void) put_ushort(6);       /* format number (6): trimmed table mapping */
    (void) put_ushort(TRIMMED_TABLE_MAP_LENGTH);
    (void) put_ushort(0);       /* version number (0) */
    (void) put_ushort(0);       /* first character code */
    (void) put_ushort(256);     /* number of character code in table */
    for (e = ttfenc_tab; e - ttfenc_tab < 256; e++)
        (void) put_ushort(e->newindex);
}

#define SEG_MAP_DELTA_LENGTH ((16 + (256))*TTF_USHORT_SIZE)

static void ttf_seg_map_delta(void)
{
    ttfenc_entry *e;
    (void) put_ushort(4);       /* format number (4: segment mapping to delta values) */
    (void) put_ushort(SEG_MAP_DELTA_LENGTH);
    (void) put_ushort(0);       /* version number */
    (void) put_ushort(4);       /* 2*segCount */
    (void) put_ushort(4);       /* searchRange */
    (void) put_ushort(1);       /* entrySelector */
    (void) put_ushort(0);       /* rangeShift */
    (void) put_ushort(0xF0FF);  /* endCount[0] */
    (void) put_ushort(0xFFFF);  /* endCount[1] */
    (void) put_ushort(0);       /* reversedPad */
    (void) put_ushort(0xF000);  /* startCount[0] */
    (void) put_ushort(0xFFFF);  /* startCount[1] */
    (void) put_ushort(0);       /* idDelta[0] */
    (void) put_ushort(1);       /* idDelta[1] */
    (void) put_ushort(2 * TTF_USHORT_SIZE);     /* idRangeOffset[0] */
    (void) put_ushort(0);       /* idRangeOffset[1] */
    for (e = ttfenc_tab; e - ttfenc_tab < 256; e++)
        (void) put_ushort(e->newindex);
}

#define CMAP_ENTRY_LENGTH (2*TTF_USHORT_SIZE + TTF_ULONG_SIZE)

static void ttf_select_cmap(void)
{
    assert(sizeof(new_cmap_tab) <= NEW_CMAP_SIZE * sizeof(cmap_entry));
    new_cmap_tab[0].platform_id = 1;    /* Macintosh */
    new_cmap_tab[0].encoding_id = 0;    /* Symbol; ignore code page */
    new_cmap_tab[0].format = new_glyphs_count < 256 ? 0 /* byte encoding */
        : 6;                    /* trimmed table mapping */
    new_cmap_tab[1].platform_id = 3;    /* Microsoft */
    new_cmap_tab[1].encoding_id = 0;    /* Symbol; ignore code page */
    new_cmap_tab[1].format = 4; /* segment mapping to delta */
}

static void ttf_write_cmap(void)
{
    cmap_entry *ce;
    long offset;
    dirtab_entry *tab = ttf_name_lookup("cmap", true);
    ttf_select_cmap();
    ttf_reset_chksm(tab);
    (void) put_ushort(0);       /* table version number (0) */
    (void) put_ushort(NEW_CMAP_SIZE);   /* number of encoding tables */
    offset = 2 * TTF_USHORT_SIZE + NEW_CMAP_SIZE * CMAP_ENTRY_LENGTH;
    for (ce = new_cmap_tab; ce - new_cmap_tab < NEW_CMAP_SIZE; ce++) {
        ce->offset = offset;
        switch (ce->format) {
        case 0:
            offset += BYTE_ENCODING_LENGTH;
            break;
        case 4:
            offset += SEG_MAP_DELTA_LENGTH;
            break;
        case 6:
            offset += TRIMMED_TABLE_MAP_LENGTH;
            break;
        default:
            pdftex_fail("invalid format (it should not have happened)");
        }
        (void) put_ushort(ce->platform_id);
        (void) put_ushort(ce->encoding_id);
        put_ulong(ce->offset);
    }
    for (ce = new_cmap_tab; ce - new_cmap_tab < NEW_CMAP_SIZE; ce++) {
        switch (ce->format) {
        case 0:
            ttf_byte_encoding();
            break;
        case 4:
            ttf_seg_map_delta();
            break;
        case 6:
            ttf_trimmed_table_map();
            break;
        }
    }
    ttf_set_chksm(tab);
}

static int prepend_subset_tags(int index, char *p)
{
    boolean is_unicode;
    int i;
    assert(index >= 0 && index < name_record_num && fd_cur->subset_tag != NULL);
    is_unicode = (name_tab[index].platform_id == 3);
    if (is_unicode) {
        for (i = 0; i < 6; ++i) {
            *p++ = 0;
            *p++ = fd_cur->subset_tag[i];
        }
        *p++ = 0;
        *p++ = '+';
        return 14;
    } else {
        strncpy(p, fd_cur->subset_tag, 6);
        p += 6;
        *p++ = '+';
        return 7;
    }
}


static void ttf_write_name(void)
{
    int i, l;
    char *p;
    int new_name_buf_size;
    char *new_name_buf;
    name_record *n;
    dirtab_entry *tab = ttf_name_lookup("name", true);
    if (is_subsetted(fd_cur->fm)) {
        l = 0;
        for (i = 0; i < name_record_num; i++)
            l += name_tab[i].length + 14;       /* maximum lengh of new stogare area */
        new_name_buf = xtalloc(l, char);
        /* additional space for subset tags */
        p = new_name_buf;
        for (i = 0; i < name_record_num; i++) {
            n = name_tab + i;
            n->new_offset = p - new_name_buf;
            if ((n->name_id == 1 || n->name_id == 3 ||
                 n->name_id == 4 || n->name_id == 6) &&
                ((n->platform_id == 1 && n->encoding_id == 0) ||
                 (n->platform_id == 3 && n->encoding_id == 0) ||
                 (n->platform_id == 3 && n->encoding_id == 1))) {
                l = prepend_subset_tags(i, p);
                p += l;
            } else
                l = 0;
            memcpy(p, name_buf + n->offset, n->length);
            p += n->length;
            n->new_length = n->length + l;
        }
        new_name_buf_size = p - new_name_buf;
    } else {
        new_name_buf = name_buf;
        new_name_buf_size = name_buf_size;
    }
    ttf_reset_chksm(tab);
    (void) put_ushort(0);       /* Format selector */
    (void) put_ushort(name_record_num);
    (void) put_ushort(3 * TTF_USHORT_SIZE +
                      name_record_num * 6 * TTF_USHORT_SIZE);
    for (i = 0; i < name_record_num; i++) {
        (void) put_ushort(name_tab[i].platform_id);
        (void) put_ushort(name_tab[i].encoding_id);
        (void) put_ushort(name_tab[i].language_id);
        (void) put_ushort(name_tab[i].name_id);
        (void) put_ushort(name_tab[i].new_length);
        (void) put_ushort(name_tab[i].new_offset);
    }
    for (p = new_name_buf; p - new_name_buf < new_name_buf_size; p++)
        put_char(*p);
    ttf_set_chksm(tab);
    if (new_name_buf != name_buf)
        xfree(new_name_buf);
}

static void ttf_write_dirtab(void)
{
    dirtab_entry *tab;
    TTF_ULONG i, k;
    char *p;
    const uint32_t save_offset = ttf_offset();
    ttf_seek_outbuf(TABDIR_OFF);
    if (is_subsetted(fd_cur->fm)) {
        for (i = 0; i < DEFAULT_NTABS; i++) {
            tab = ttf_name_lookup(newtabnames[i], false);
            if (tab == NULL)
                continue;
            for (k = 0; k < 4; k++)
                put_char(tab->tag[k]);
            put_ulong(tab->checksum);
            put_ulong(tab->offset);
            put_ulong(tab->length);
        }
    } else {
        for (tab = dir_tab; tab - dir_tab < ntabs; tab++) {
            for (k = 0; k < 4; k++)
                put_char(tab->tag[k]);
            put_ulong(tab->checksum);
            put_ulong(tab->offset);
            put_ulong(tab->length);
        }
    }
    /* adjust checkSumAdjustment */
    tmp_ulong = 0;
    checksum = 0;
    for (p = fb_array, i = 0; i < save_offset;) {
        tmp_ulong = (tmp_ulong << 8) + *p++;
        i++;
        if (i % 4 == 0) {
            checksum += tmp_ulong;
            tmp_ulong = 0;
        }
    }
    if (i % 4 != 0) {
        pdftex_warn("font length is not a multiple of 4 (%d)", i);
        checksum <<= 8 * (4 - i % 4);
    }
    k = 0xB1B0AFBA - checksum;
    ttf_seek_outbuf(checkSumAdjustment_offset);
    put_ulong(k);
    ttf_seek_outbuf(save_offset);
}

static void ttf_write_glyf(void)
{
    long *id, k;
    TTF_USHORT idx;
    TTF_USHORT flags;
    dirtab_entry *tab = ttf_name_lookup("glyf", true);
    const long glyf_offset = tab->offset;
    const long new_glyf_offset = ttf_offset();
    ttf_reset_chksm(tab);
    for (id = glyph_index; id - glyph_index < new_glyphs_count; id++) {
        glyph_tab[*id].newoffset = ttf_offset() - new_glyf_offset;
        if (glyph_tab[*id].offset != glyph_tab[*id + 1].offset) {
            ttf_seek_off(glyf_offset + glyph_tab[*id].offset);
            k = copy_short();
            ttf_ncopy(4 * TTF_FWORD_SIZE);
            if (k < 0) {
                do {
                    flags = copy_ushort();
                    idx = get_ushort();
                    if (glyph_tab[idx].newindex < 0) {
                        glyph_tab[idx].newindex = new_glyphs_count;
                        glyph_index[new_glyphs_count++] = idx;
                        /*
                           N.B.: Here we change `new_glyphs_count',
                           which appears in the condition of the `for' loop
                         */
                    }
                    (void) put_ushort(glyph_tab[idx].newindex);
                    if (flags & ARG_1_AND_2_ARE_WORDS)
                        ttf_ncopy(2 * TTF_SHORT_SIZE);
                    else
                        ttf_ncopy(TTF_USHORT_SIZE);
                    if (flags & WE_HAVE_A_SCALE)
                        ttf_ncopy(TTF_F2DOT14_SIZE);
                    else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
                        ttf_ncopy(2 * TTF_F2DOT14_SIZE);
                    else if (flags & WE_HAVE_A_TWO_BY_TWO)
                        ttf_ncopy(4 * TTF_F2DOT14_SIZE);
                } while (flags & MORE_COMPONENTS);
                if (flags & WE_HAVE_INSTRUCTIONS)
                    ttf_ncopy(copy_ushort());
            } else
                ttf_ncopy(glyph_tab[*id + 1].offset - glyph_tab[*id].offset -
                          TTF_USHORT_SIZE - 4 * TTF_FWORD_SIZE);
        }
    }
    last_glyf_offset = ttf_offset() - new_glyf_offset;
    ttf_set_chksm(tab);
}

static void ttf_reindex_glyphs(void)
{
    ttfenc_entry *e;
    glyph_entry *glyph;
    int index;
    int n;
    long *t;
    ttf_cmap_entry *cmap = NULL;
    boolean cmap_not_found = false;

    /*
     * reindexing glyphs: we append index of used glyphs to `glyph_index'
     * while going through `ttfenc_tab'. After appending a new entry to
     * `glyph_index' we set field `newindex' of corresponding entries in both
     * `glyph_tab' and `ttfenc_tab' to the newly created index
     */
    for (e = ttfenc_tab; e - ttfenc_tab < 256; e++) {
        e->newindex = 0;        /* index of ".notdef" glyph */

        /* handle case of subfonts first */
        if (is_subfont(fd_cur->fm)) {
            if (e->code == -1)
                continue;
            assert(fd_cur->fm->pid != -1 && fd_cur->fm->eid != -1);
            if (cmap == NULL && !cmap_not_found) {
                cmap =
                    ttf_read_cmap(fd_cur->fm->ff_name, fd_cur->fm->pid,
                                  fd_cur->fm->eid, true);
                if (cmap == NULL)
                    cmap_not_found = true;
            }
            if (cmap == NULL)
                continue;
            t = cmap->table;
            assert(t != NULL && e->code < 0x10000);
            if (t[e->code] < 0) {
                pdftex_warn
                    ("subfont %s: wrong mapping: character %li --> 0x%4.4lX --> .notdef",
                     fd_cur->fm->tfm_name, (long) (e - ttfenc_tab), e->code);
                continue;
            }
            assert(t[e->code] >= 0 && t[e->code] < glyphs_count);       /* t has been read from ttf */
            glyph = glyph_tab + t[e->code];
            goto append_new_glyph;
        }

        /* handle case of reencoded fonts */
        if (e->name == notdef)
            continue;

        /* look up by name first */
        for (glyph = glyph_tab; glyph - glyph_tab < glyphs_count; glyph++)
            if (glyph->name != notdef && strcmp(glyph->name, e->name) == 0)
                goto append_new_glyph;

        /* scan form `uniABCD' */
        n = -1;
        sscanf(e->name, GLYPH_PREFIX_UNICODE "%X%n", &index, &n);
        if (n == strlen(e->name)) {
            if (cmap == NULL && !cmap_not_found) {
                /* need to read the unicode mapping, ie (pid,eid) = (3,1) or (0,3) */
                cmap = ttf_read_cmap(fd_cur->fm->ff_name, 3, 1, false);
                if (cmap == NULL)
                    cmap = ttf_read_cmap(fd_cur->fm->ff_name, 0, 3, false);
                if (cmap == NULL) {
                    pdftex_warn
                        ("no unicode mapping found, all `uniXXXX' names will be ignored");
                    cmap_not_found = true;      /* once only */
                }
            }
            if (cmap == NULL)
                continue;
            t = cmap->table;
            assert(t != NULL);
            if (t[index] != -1) {
                if (t[index] >= glyphs_count) {
                    pdftex_warn
                        ("`%s' is mapped to index %li which is out of valid range [0..%i)",
                         e->name, t[index], glyphs_count);
                    continue;
                }
                glyph = glyph_tab + t[index];
                goto append_new_glyph;
            } else {
                pdftex_warn("`unicode %s%.4X' is not mapped to any glyph",
                            GLYPH_PREFIX_UNICODE, index);
                continue;
            }
        }

        /* scan form `index123' */
        n = -1;
        sscanf(e->name, GLYPH_PREFIX_INDEX "%i%n", &index, &n);
        if (n == strlen(e->name)) {
            if (index >= glyphs_count) {
                pdftex_warn("`%s' out of valid range [0..%i)",
                            e->name, glyphs_count);
                continue;
            }
            glyph = glyph_tab + index;
            goto append_new_glyph;
        }

        /* not found */
        pdftex_warn("glyph `%s' not found", e->name);
        continue;

      append_new_glyph:
        assert(glyph > glyph_tab && glyph - glyph_tab < glyphs_count);
        if (glyph->newindex < 0) {
            glyph_index[new_glyphs_count] = glyph - glyph_tab;
            glyph->newindex = new_glyphs_count;
            new_glyphs_count++;
        }
        e->newindex = glyph->newindex;
    }
}

static void ttf_write_head(void)
{
    dirtab_entry *tab;
    tab = ttf_seek_tab("head", 0);
    ttf_reset_chksm(tab);
    ttf_ncopy(2 * TTF_FIXED_SIZE);
    checkSumAdjustment_offset = ttf_offset();
    put_ulong(0);
    ttf_skip(TTF_ULONG_SIZE);   /* skip checkSumAdjustment */
    ttf_ncopy(TTF_ULONG_SIZE + 2 * TTF_USHORT_SIZE + 16 +
              4 * TTF_FWORD_SIZE + 2 * TTF_USHORT_SIZE + TTF_SHORT_SIZE);
    if (is_subsetted(fd_cur->fm)) {
        (void) put_short(loca_format);
        (void) put_short(0);
    } else
        ttf_ncopy(2 * TTF_SHORT_SIZE);
    ttf_set_chksm(tab);
}

static void ttf_write_hhea(void)
{
    dirtab_entry *tab;
    tab = ttf_seek_tab("hhea", 0);
    ttf_reset_chksm(tab);
    ttf_ncopy(TTF_FIXED_SIZE + 3 * TTF_FWORD_SIZE + TTF_UFWORD_SIZE +
              3 * TTF_FWORD_SIZE + 8 * TTF_SHORT_SIZE);
    (void) put_ushort(new_glyphs_count);
    ttf_set_chksm(tab);
}

static void ttf_write_htmx(void)
{
    long *id;
    dirtab_entry *tab = ttf_seek_tab("hmtx", 0);
    ttf_reset_chksm(tab);
    for (id = glyph_index; id - glyph_index < new_glyphs_count; id++) {
        put_ufword(glyph_tab[*id].advWidth);
        put_ufword(glyph_tab[*id].lsb);
    }
    ttf_set_chksm(tab);
}

static void ttf_write_loca(void)
{
    long *id;
    dirtab_entry *tab = ttf_seek_tab("loca", 0);
    ttf_reset_chksm(tab);
    loca_format = 0;
    if (last_glyf_offset >= 0x00020000 || (last_glyf_offset & 1))
        loca_format = 1;
    else
        for (id = glyph_index; id - glyph_index < new_glyphs_count; id++)
            if (glyph_tab[*id].newoffset & 1) {
                loca_format = 1;
                break;
            }
    if (loca_format != 0) {
        for (id = glyph_index; id - glyph_index < new_glyphs_count; id++)
            put_ulong(glyph_tab[*id].newoffset);
        put_ulong(last_glyf_offset);
    } else {
        for (id = glyph_index; id - glyph_index < new_glyphs_count; id++)
            (void) put_ushort(glyph_tab[*id].newoffset / 2);
        (void) put_ushort(last_glyf_offset / 2);
    }
    ttf_set_chksm(tab);
}

static void ttf_write_mapx(void)
{
    dirtab_entry *tab = ttf_seek_tab("maxp", TTF_FIXED_SIZE + TTF_USHORT_SIZE);
    ttf_reset_chksm(tab);
    put_fixed(0x00010000);
    (void) put_ushort(new_glyphs_count);
    ttf_ncopy(13 * TTF_USHORT_SIZE);
    ttf_set_chksm(tab);
}

static void ttf_write_OS2(void)
{
    dirtab_entry *tab = ttf_seek_tab("OS/2", 0);
    TTF_USHORT version;
    ttf_reset_chksm(tab);
    version = get_ushort();
    if (version > 3)
        pdftex_fail("unknown version of OS/2 table (%.4X)", version);
    (void) put_ushort(0x0001);  /* fix version to 1 */
    ttf_ncopy(2 * TTF_USHORT_SIZE + 13 * TTF_SHORT_SIZE + 10 * TTF_BYTE_SIZE);
    ttf_skip(4 * TTF_ULONG_SIZE);       /* ulUnicodeRange 1--4 */
    put_ulong(0x00000003);      /* Basic Latin + Latin-1 Supplement (0x0000--0x00FF) */
    put_ulong(0x10000000);      /* Private Use (0xE000--0xF8FF) */
    put_ulong(0x00000000);
    put_ulong(0x00000000);
    ttf_ncopy(4 * TTF_CHAR_SIZE + TTF_USHORT_SIZE);     /* achVendID + fsSelection */
    ttf_skip(2 * TTF_USHORT_SIZE);
    (void) put_ushort(0x0000);  /* usFirstCharIndex */
    (void) put_ushort(0xF0FF);  /* usLastCharIndex */
    ttf_ncopy(5 * TTF_USHORT_SIZE);
    /* for version 0 the OS/2 table ends here, the rest is for version 1 */
    put_ulong(0x80000000);      /* Symbol Character Set---don't use any code page */
    put_ulong(0x00000000);
    ttf_set_chksm(tab);
}

static boolean unsafe_name(const char *s)
{
    const char **p;
    for (p = ambiguous_names; *p != NULL; p++)
        if (strcmp(s, *p) == 0)
            return true;
    return false;
}

static void ttf_write_post(void)
{
    dirtab_entry *tab = ttf_seek_tab("post", TTF_FIXED_SIZE);
    glyph_entry *glyph;
    const char *s;
    long *id;
    int l;
    ttf_reset_chksm(tab);
    if (!fd_cur->write_ttf_glyph_names || post_format == 0x00030000) {
        put_fixed(0x00030000);
        ttf_ncopy(TTF_FIXED_SIZE + 2 * TTF_FWORD_SIZE + 5 * TTF_ULONG_SIZE);
    } else {
        put_fixed(0x00020000);
        ttf_ncopy(TTF_FIXED_SIZE + 2 * TTF_FWORD_SIZE + 5 * TTF_ULONG_SIZE);
        (void) put_ushort(new_glyphs_count);
        k = 0;
        for (id = glyph_index; id - glyph_index < new_glyphs_count; id++) {
            glyph = glyph_tab + *id;
            if (glyph->name_index >= NMACGLYPHS || unsafe_name(glyph->name))
                glyph->name_index = NMACGLYPHS + k++;
            (void) put_ushort(glyph->name_index);
        }
        for (id = glyph_index; id - glyph_index < new_glyphs_count; id++) {
            glyph = glyph_tab + *id;
            if (glyph->name_index >= NMACGLYPHS) {
                s = glyph->name;
                l = strlen(s);
                put_byte(l);
                while (l-- > 0)
                    put_char(*s++);
            }
        }
    }
    ttf_set_chksm(tab);
}

static void ttf_init_font(int n)
{
    int i, k;
    for (i = 1, k = 0; i <= n; i <<= 1, k++);
    put_fixed(0x00010000);      /* font version */
    (void) put_ushort(n);       /* number of tables */
    (void) put_ushort(i << 3);  /* search range */
    (void) put_ushort(k - 1);   /* entry selector */
    (void) put_ushort((n << 4) - (i << 3));     /* range shift */
    ttf_seek_outbuf(TABDIR_OFF + n * 4 * TTF_ULONG_SIZE);
}

static void ttf_subset_font(void)
{
    ttf_init_font(new_ntabs);
    if (ttf_name_lookup("PCLT", false) != NULL)
        ttf_copytab("PCLT");
    if (ttf_name_lookup("fpgm", false) != NULL)
        ttf_copytab("fpgm");
    if (ttf_name_lookup("cvt ", false) != NULL)
        ttf_copytab("cvt ");
    if (ttf_name_lookup("prep", false) != NULL)
        ttf_copytab("prep");
    ttf_reindex_glyphs();
    ttf_write_glyf();
    ttf_write_loca();
    ttf_write_OS2();
    ttf_write_head();
    ttf_write_hhea();
    ttf_write_htmx();
    ttf_write_mapx();
    ttf_write_name();
    ttf_write_post();
    ttf_write_cmap();
    ttf_write_dirtab();
}

static void ttf_copy_font(void)
{
    dirtab_entry *tab;
    ttf_init_font(ntabs);
    for (tab = dir_tab; tab - dir_tab < ntabs; tab++) {
        if (strncmp(tab->tag, "head", 4) == 0)
            ttf_write_head();
        else
            ttf_copytab(tab->tag);
    }
    ttf_write_dirtab();
}

void writettf(fd_entry * fd)
{
    fd_cur = fd;                /* fd_cur is global inside writettf.c */
    assert(fd_cur->fm != NULL);
    assert(is_truetype(fd_cur->fm));
    assert(is_included(fd_cur->fm));

    set_cur_file_name(fd_cur->fm->ff_name);
    if (is_subsetted(fd_cur->fm) && (fd_cur->fe == NULL)
        && !is_subfont(fd_cur->fm)) {
        pdftex_fail("Subset TrueType must be a reencoded or a subfont");
    }
    if (!ttf_open()) {
        pdftex_fail("cannot open TrueType font file for reading");
    }
    cur_file_name = (char *) nameoffile + 1;

    /* skip ttc header, prepare for reading first font  */
    if (strcasecmp(strchr(cur_file_name, 0) - 4, ".ttc") == 0) {
        if (get_ulong() != 0x74746366 /* ttcf */) {
            xfseek(INFILE, 0, SEEK_SET, cur_file_name);
        } else {
            ttf_skip(TTF_FIXED_SIZE);   /* ignore the version   */
            ttf_skip(TTF_ULONG_SIZE);   /* ignore the numFonts  */

            /* goto first font  */
            xfseek(INFILE, get_ulong(), SEEK_SET, cur_file_name);
        }
    }

    if (is_subsetted(fd_cur->fm))
        tex_printf("<%s", cur_file_name);
    else
        tex_printf("<<%s", cur_file_name);
    fd_cur->ff_found = true;
    new_glyphs_count = 2;
    new_ntabs = DEFAULT_NTABS;
    dir_tab = NULL;
    glyph_tab = NULL;
    glyph_index = NULL;
    glyph_name_buf = NULL;
    name_tab = NULL;
    name_buf = NULL;
    ttf_read_font();

    pdfsaveoffset = pdfoffset();
    pdfflush();
    if (is_subsetted(fd_cur->fm)) {
        ttf_copy_encoding();
        ttf_subset_font();
    } else
        ttf_copy_font();
    ttf_length = ttf_offset();

    xfree(dir_tab);
    xfree(glyph_tab);
    xfree(glyph_index);
    xfree(glyph_name_buf);
    xfree(name_tab);
    xfree(name_buf);
    ttf_close();
    if (is_subsetted(fd_cur->fm))
        tex_printf(">");
    else
        tex_printf(">>");
    cur_file_name = NULL;
}

void writeotf(fd_entry * fd)
{
    dirtab_entry *tab;
    long i;

    fd_cur = fd;                /* fd_cur is global inside writettf.c */
    assert(fd_cur->fm != NULL);
    assert(is_opentype(fd_cur->fm));
    assert(is_included(fd_cur->fm));

    set_cur_file_name(fd_cur->fm->ff_name);
    if (is_subsetted(fd_cur->fm))
        pdftex_fail("OTF fonts must be included entirely");
    if (!open_input(&ttf_file, kpse_opentype_format, FOPEN_RBIN_MODE)) {
        pdftex_fail("cannot open OpenType font file for reading");
    }
    cur_file_name = (char *) nameoffile + 1;
    tex_printf("<<%s", cur_file_name);
    fd_cur->ff_found = true;
    dir_tab = NULL;
    glyph_tab = NULL;
    ttf_read_tabdir();
    /* read font parameters */
    if (ttf_name_lookup("head", false) != NULL)
        ttf_read_head();
    if (ttf_name_lookup("hhea", false) != NULL)
        ttf_read_hhea();
    if (ttf_name_lookup("PCLT", false) != NULL)
        ttf_read_pclt();
    if (ttf_name_lookup("post", false) != NULL)
        ttf_read_post();
    /* copy font file */
    tab = ttf_seek_tab("CFF ", 0);
    for (i = tab->length; i > 0; i--)
        copy_char();
    xfree(dir_tab);
    ttf_close();
    tex_printf(">>");
    cur_file_name = NULL;
}

/*
 To calculate the checkSum for the 'head' table which itself includes the
   checkSumAdjustment entry for the entire font, do the following:

     * Set the checkSumAdjustment to 0.
     * Calculate the checksum for all the tables including the 'head' table
       and enter that value into the table directory.
     * Calculate the checksum for the entire font.
     * Subtract that value from the hex value B1B0AFBA.
     * Store the result in checkSumAdjustment.

   The checkSum for the 'head table which includes the checkSumAdjustment
   entry for the entire font is now incorrect. That is not a problem. Do not
   change it. An application attempting to verify that the 'head' table has
   not changed should calculate the checkSum for that table by not including
   the checkSumAdjustment value, and compare the result with the entry in the
   table directory.

   The table directory also includes the offset of the associated tagged
   table from the beginning of the font file and the length of that table.
 */
