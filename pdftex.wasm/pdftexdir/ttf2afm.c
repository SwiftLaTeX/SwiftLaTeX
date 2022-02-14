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
#include <kpathsea/c-auto.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-limits.h>
#include <kpathsea/c-memstr.h>
#include <kpathsea/c-std.h>
#include <kpathsea/c-unistd.h>
#include <stdarg.h>
#include <kpathsea/getopt.h>
*/
#include <kpathsea/kpathsea.h>
#include <time.h>
#include <pdftexdir/ptexmac.h>
#include <pdftexdir/writettf.h>
#include <string.h>

/* constants used for print_glyph */
#define AS_NAME         0
#define AS_INDEX        1
#define AS_UNICODE      2

#define VERSION         "1.02"

/* FIXME */
#define NOGLYPH_ASSIGNED_YET USHRT_MAX

#define enc_getchar()   xgetc(encfile)
#define enc_eof()       feof(encfile)
#define pdftex_fail     ttf_fail

#define print_str(S)    if (S != NULL) fprintf(outfile, #S " %s\n", escape_eol(S))
#define print_dimen(N)  if (N != 0) fprintf(outfile, #N " %i\n", (int)get_ttf_funit(N))

#define get_ttf_funit(n) \
    (n < 0 ? -((-n/upem)*1000 + ((-n%upem)*1000)/upem) :\
    ((n/upem)*1000 + ((n%upem)*1000)/upem))

typedef struct _unicode_entry {
    TTF_USHORT code;
    struct _unicode_entry *next;
} unicode_entry;

typedef struct {
    TTF_ULONG wx;
    const char *name;
    TTF_USHORT index;
    TTF_LONG bbox[4];
    TTF_LONG offset;
    char found;
    unicode_entry *unicode_list;
} mtx_entry;

typedef struct _kern_entry {
    TTF_FWORD value;
    TTF_USHORT adjacent;
    struct _kern_entry *next;
} kern_entry;


char *FontName = NULL;
char *FullName = NULL;
char *FamilyName = NULL;
char *Notice = NULL;
char *Version = NULL;
char *Weight = NULL;
TTF_LONG ItalicAngle = 0;
TTF_LONG IsFixedPitch = 0;
TTF_LONG FontBBox1 = 0;
TTF_LONG FontBBox2 = 0;
TTF_LONG FontBBox3 = 0;
TTF_LONG FontBBox4 = 0;
TTF_LONG UnderlinePosition = 0;
TTF_LONG UnderlineThickness = 0;
TTF_LONG CapHeight = 0;
TTF_LONG XHeight = 0;
TTF_LONG Ascender = 0;
TTF_LONG Descender = 0;

char *cur_file_name = NULL;
char *bname = NULL;
FILE *fontfile, *encfile, *outfile = NULL;
char enc_line[ENC_BUF_SIZE];
int print_glyph = AS_NAME;      /* print glyph as names by default */
int print_cmap = 0;
int use_ext_enc = 0;            /* use external encoding? */
int select_unicode = 1;         /* use the first unicode mapping by default */
int printing_enc = 0;           /* set to 1 while printing encodings */


TTF_USHORT upem;
TTF_USHORT ntabs;
int nhmtx;
int post_format;
int loca_format;
long nglyphs;
int nkernpairs = 0;
int names_count = 0;
char *ps_glyphs_buf = NULL;
dirtab_entry *dir_tab;
mtx_entry *mtx_tab;
kern_entry *kern_tab;
char *enc_names[256];

cmap_entry *cmap_tab;
TTF_USHORT ncmapsubtabs;
long cmap_offset;

TTF_USHORT unicode_map[0xFFFF];

#include "macnames.c"

#if defined __GNUC__ && __GNUC__ >=3
__attribute__((__noreturn__))
#endif
static void ttf_fail(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\nError: ttf2afm");
    if (cur_file_name)
        fprintf(stderr, " (file %s)", cur_file_name);
    fprintf(stderr, ": ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(-1);
}

static void ttf_warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\nWarning: ttf2afm");
    if (cur_file_name)
        fprintf(stderr, " (file %s)", cur_file_name);
    fprintf(stderr, ": ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static int xgetc(FILE * stream)
{
    int c = getc(stream);
    if (c < 0 && c != EOF)
        ttf_fail("getc() failed");
    return c;
}

static long ttf_getnum(int s)
{
    long i = 0;
    int c;
    while (s > 0) {
        if ((c = xgetc(fontfile)) < 0)
            ttf_fail("unexpected EOF");
        i = (i << 8) + c;
        s--;
    }
    return i;
}

static dirtab_entry *name_lookup(const char *s)
{
    dirtab_entry *p;
    for (p = dir_tab; p - dir_tab < ntabs; p++)
        if (strncmp(p->tag, s, 4) == 0)
            break;
    if (p - dir_tab == ntabs)
        p = NULL;
    return p;
}

static void ttf_seek_tab(const char *name, TTF_LONG offset)
{
    dirtab_entry *p = name_lookup(name);
    if (p == NULL)
        ttf_fail("can't find table `%s'", name);
    if (fseek(fontfile, p->offset + offset, SEEK_SET) < 0)
        ttf_fail("fseek() failed while reading `%s' table", name);
}

static void ttf_seek_off(const char *name, TTF_LONG offset)
{
    if (fseek(fontfile, offset, SEEK_SET) < 0)
        ttf_fail("fseek() failed while reading `%s' table", name);
}

static void store_kern_value(TTF_USHORT i, TTF_USHORT j, TTF_FWORD v)
{
    kern_entry *pk;
    for (pk = kern_tab + i; pk->next != NULL; pk = pk->next);
    pk->next = xtalloc(1, kern_entry);
    pk = pk->next;
    pk->next = NULL;
    pk->adjacent = j;
    pk->value = v;
}

#if 0 /* unused */
static TTF_FWORD get_kern_value(TTF_USHORT i, TTF_USHORT j)
{
    kern_entry *pk;
    for (pk = kern_tab + i; pk->next != NULL; pk = pk->next)
        if (pk->adjacent == j)
            return pk->value;
    return 0;
}
#endif

static void free_tabs(void)
{
    int i;
    kern_entry *p, *q, *r;
    unicode_entry *u, *v;
    mtx_entry *pm;
    xfree(ps_glyphs_buf);
    xfree(dir_tab);
    xfree(cmap_tab);
    for (pm = mtx_tab; pm - mtx_tab < nglyphs; pm++)
        if (pm->unicode_list != NULL) {
            for (u = pm->unicode_list; u != NULL; u = v) {
                v = u->next;
                xfree(u);
            }
        }
    xfree(mtx_tab);
    for (i = 0; i < 256; i++)
        if (enc_names[i] != notdef)
            free(enc_names[i]);
    if (kern_tab == NULL)
        return;
    for (p = kern_tab; p - kern_tab < nglyphs; p++)
        if (p->next != NULL) {
            for (q = p->next; q != NULL; q = r) {
                r = q->next;
                xfree(q);
            }
        }
    xfree(kern_tab);
}

static void enc_getline(void)
{
    char *p;
    int c;
  restart:
    if (enc_eof())
        ttf_fail("unexpected end of file");
    p = enc_line;
    do {
        c = enc_getchar();
        append_char_to_buf(c, p, enc_line, ENC_BUF_SIZE);
    } while (c != 10);
    append_eol(p, enc_line, ENC_BUF_SIZE);
    if (p - enc_line <= 2 || *enc_line == '%')
        goto restart;
}

static void read_encoding(char *encname)
{
    char buf[ENC_BUF_SIZE], *q, *r;
    int i;
    cur_file_name = encname;
    if ((encfile = kpse_open_file(encname, kpse_enc_format)) == NULL)
        ttf_fail("can't open encoding file for reading");
    enc_getline();
    if (*enc_line != '/' || (r = strchr(enc_line, '[')) == 0)
        ttf_fail("invalid encoding vector: name or `[' missing:\n%s", enc_line);
    for (i = 0; i < 256; i++)
        enc_names[i] = notdef;
    if (r[1] == 32)
        r += 2;
    else
        r++;
    for (;;) {
        while (*r == '/') {
            for (q = buf, r++; *r != ' ' && *r != 10 && *r != ']' && *r != '/';
                 *q++ = *r++);
            *q = 0;
            if (*r == ' ')
                r++;
            if (strcmp(buf, notdef) != 0)
                enc_names[names_count] = xstrdup(buf);
            if (++names_count > 256)
                ttf_fail("encoding vector contains more than 256 names");
        }
        if (*r != 10 && *r != '%') {
            if (str_prefix(r, "] def"))
                goto done;
            else
                ttf_fail
                    ("invalid encoding vector: a name or `] def' expected:\n%s",
                     enc_line);
        }
        enc_getline();
        r = enc_line;
    }
  done:
    xfclose(encfile, cur_file_name);
    if (names_count != 256)
        ttf_warn("encoding vector contains only %i names (expected %i)",
                 names_count, 256);
}

static void append_unicode(long glyph_index, TTF_USHORT code)
{
    mtx_entry *m;
    unicode_entry *u, *v;
    assert(glyph_index >= 0 && glyph_index < nglyphs);
    u = xtalloc(1, unicode_entry);
    m = mtx_tab + glyph_index;
    u->next = NULL;
    u->code = code;
    if (m->unicode_list == NULL)
        m->unicode_list = u;
    else {
        for (v = m->unicode_list; v->next != NULL; v = v->next);
        v->next = u;
    }
}

static void read_cmap(void)
{
    cmap_entry *e;
    seg_entry *seg_tab, *s;
    TTF_USHORT *glyphId, format, segCount;
    long int n, i, k, length, index;
    int unicode_map_count = 0;
    ttf_seek_tab("cmap", TTF_USHORT_SIZE);      /* skip the table version number (=0) */
    ncmapsubtabs = get_ushort();
    cmap_offset = xftell(fontfile, cur_file_name) - 2 * TTF_USHORT_SIZE;
    cmap_tab = xtalloc(ncmapsubtabs, cmap_entry);
    for (e = cmap_tab; e - cmap_tab < ncmapsubtabs; e++) {
        e->platform_id = get_ushort();
        e->encoding_id = get_ushort();
        e->offset = get_ulong();
    }
    for (i = 0; i < 0xFFFF; ++i)
        unicode_map[i] = NOGLYPH_ASSIGNED_YET;
    for (e = cmap_tab; e - cmap_tab < ncmapsubtabs; e++) {
        ttf_seek_off("cmap", cmap_offset + e->offset);
        format = get_ushort();
        if (is_unicode_mapping(e) && format == 4) {
            ++unicode_map_count;
            if (unicode_map_count == select_unicode)
                goto read_unicode_mapping;
        }
        continue;
      read_unicode_mapping:
        length = get_ushort();  /* length of subtable */
        (void) get_ushort();    /* skip the version number */
        segCount = get_ushort() / 2;
        (void) get_ushort();    /* skip searchRange */
        (void) get_ushort();    /* skip entrySelector */
        (void) get_ushort();    /* skip rangeShift */
        seg_tab = xtalloc(segCount, seg_entry);
        for (s = seg_tab; s - seg_tab < segCount; s++)
            s->endCode = get_ushort();
        (void) get_ushort();    /* skip reversedPad */
        for (s = seg_tab; s - seg_tab < segCount; s++)
            s->startCode = get_ushort();
        for (s = seg_tab; s - seg_tab < segCount; s++)
            s->idDelta = get_ushort();
        for (s = seg_tab; s - seg_tab < segCount; s++)
            s->idRangeOffset = get_ushort();
        length -= 8 * TTF_USHORT_SIZE + 4 * segCount * TTF_USHORT_SIZE;
        n = length / TTF_USHORT_SIZE;   /* number of glyphID's */
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
                    if (index < 0 || index >= nglyphs)
                        ttf_fail("cmap: glyph index out of range [0..%i)",
                                 nglyphs);
                    if (unicode_map[i] != NOGLYPH_ASSIGNED_YET)
                        ttf_fail
                            ("cmap: unicode %.4X is mapped to multiple glyphs",
                             i);
                    if (unicode_map[i] == 0)
                        ttf_warn("unicode %.4X is mapped to glyph 0", i);
                    unicode_map[i] = index;
                    append_unicode(index, i);
                }
            }
        }
        xfree(seg_tab);
        xfree(glyphId);
        break;
    }
    if (e - cmap_tab == ncmapsubtabs)
        ttf_fail("Invalid argument `-m %i': out of range [1..%i]",
                 select_unicode, unicode_map_count);
}

static void read_font(void)
{
    long i, j, k, l, n, m, platform_id, encoding_id;
    TTF_FWORD kern_value;
    char buf[1024], *p;
    dirtab_entry *pd;
    kern_entry *pk;
    mtx_entry *pm;
    ttf_skip(TTF_FIXED_SIZE);
    ntabs = get_ushort();
    ttf_skip(3 * TTF_USHORT_SIZE);
    dir_tab = xtalloc(ntabs, dirtab_entry);
    for (pd = dir_tab; pd - dir_tab < ntabs; pd++) {
        pd->tag[0] = get_char();
        pd->tag[1] = get_char();
        pd->tag[2] = get_char();
        pd->tag[3] = get_char();
        ttf_skip(TTF_ULONG_SIZE);
        pd->offset = get_ulong();
        pd->length = get_ulong();
    }
    ttf_seek_tab("head",
                 2 * TTF_FIXED_SIZE + 2 * TTF_ULONG_SIZE + TTF_USHORT_SIZE);
    upem = get_ushort();
    ttf_skip(16);
    FontBBox1 = get_fword();
    FontBBox2 = get_fword();
    FontBBox3 = get_fword();
    FontBBox4 = get_fword();
    ttf_skip(TTF_USHORT_SIZE);
    ttf_skip(TTF_USHORT_SIZE + TTF_SHORT_SIZE);
    loca_format = get_short();
    ttf_seek_tab("maxp", TTF_FIXED_SIZE);
    nglyphs = get_ushort();
    mtx_tab = xtalloc(nglyphs + 1, mtx_entry);
    for (pm = mtx_tab; pm - mtx_tab < nglyphs + 1; pm++) {
        pm->name = NULL;        /* notdef */
        pm->found = 0;
        pm->unicode_list = NULL;
    }
    ttf_seek_tab("hhea", TTF_FIXED_SIZE);
    Ascender = get_fword();
    Descender = get_fword();
    ttf_skip(TTF_FWORD_SIZE + TTF_UFWORD_SIZE + 3 * TTF_FWORD_SIZE +
             8 * TTF_SHORT_SIZE);
    nhmtx = get_ushort();
    ttf_seek_tab("hmtx", 0);
    for (pm = mtx_tab; pm - mtx_tab < nhmtx; pm++) {
        pm->wx = get_ufword();
        ttf_skip(TTF_FWORD_SIZE);
    }
    i = pm[-1].wx;
    for (; pm - mtx_tab < nglyphs; pm++)
        pm->wx = i;
    ttf_seek_tab("post", 0);
    post_format = get_fixed();
    ItalicAngle = get_fixed();
    UnderlinePosition = get_fword();
    UnderlineThickness = get_fword();
    IsFixedPitch = get_ulong();
    ttf_skip(4 * TTF_ULONG_SIZE);
    switch (post_format) {
    case 0x00010000:
        for (pm = mtx_tab; pm - mtx_tab < NMACGLYPHS; pm++)
            pm->name = mac_glyph_names[pm - mtx_tab];
        break;
    case 0x00020000:
        l = get_ushort();       /* some fonts have this value different from nglyphs */
        for (pm = mtx_tab; pm - mtx_tab < l; pm++)
            pm->index = get_ushort();
        if ((pd = name_lookup("post")) == NULL)
            ttf_fail("can't find table `post'");
        n = pd->length - (xftell(fontfile, cur_file_name) - pd->offset);
        ps_glyphs_buf = xtalloc(n + 1, char);
        for (m = 0, p = ps_glyphs_buf; p - ps_glyphs_buf < n;) {
            for (i = get_byte(); i > 0; i--)
                *p++ = get_char();
            *p++ = 0;
            m++;
        }
        for (pm = mtx_tab; pm - mtx_tab < l; pm++) {
            if (pm->index < NMACGLYPHS)
                pm->name = mac_glyph_names[pm->index];
            else {
                k = pm->index - NMACGLYPHS;
                if (k < m) {
                    for (p = ps_glyphs_buf; k > 0; k--)
                        p = (char *) strend(p) + 1;
                    pm->name = p;
                } else {
                    pm->name = NULL;    /* index out of valid range, fix name to notdef */
                }
            }
        }
        break;
    default:
        ttf_warn("unsupported format (%.8X) of `post' table, assuming 3.0",
                 post_format);
    case 0x00030000:
        if (print_glyph == AS_NAME) {
            ttf_warn
                ("no names available in `post' table, print glyph names as indices");
            print_glyph = AS_INDEX;
        }
    }
    ttf_seek_tab("loca", 0);
    if (loca_format == 1) {
        for (pm = mtx_tab; pm - mtx_tab < nglyphs + 1; pm++)
            pm->offset = get_ulong();
    } else {
        for (pm = mtx_tab; pm - mtx_tab < nglyphs + 1; pm++)
            pm->offset = get_ushort() << 1;
    }
    if ((pd = name_lookup("glyf")) == NULL)
        ttf_fail("can't find table `glyf'");
    for (n = pd->offset, pm = mtx_tab; pm - mtx_tab < nglyphs; pm++) {
        ttf_seek_off("glyf", n + pm->offset);
        ttf_skip(TTF_SHORT_SIZE);
        pm->bbox[0] = get_fword();
        pm->bbox[1] = get_fword();
        pm->bbox[2] = get_fword();
        pm->bbox[3] = get_fword();
    }

    ttf_seek_tab("name", 0);
    i = ftell(fontfile);
    (void) get_ushort();        /* skip Format selector (=0) */
    n = get_ushort();           /* number of name records */
    j = get_ushort() + i;       /* start of string storage */
    i += 3 * TTF_USHORT_SIZE;   /* update the current offset */
    while (n-- > 0) {
        ttf_seek_off("name", i);
        platform_id = get_ushort();
        encoding_id = get_ushort();
        (void) get_ushort();    /* skip language_id */
        k = get_ushort();       /* name_id */
        l = get_ushort();       /* string length */
        if ((platform_id == 1 && encoding_id == 0) &&
            (k == 0 || k == 1 || k == 4 || k == 5 || k == 6)) {
            ttf_seek_off("name", j + get_ushort());
            for (p = buf; l-- > 0; p++)
                *p = get_char();
            *p++ = 0;
            p = xstrdup(buf);
            switch (k) {
            case 0:
                Notice = p;
                break;
            case 1:
                FamilyName = p;
                break;
            case 4:
                FullName = p;
                break;
            case 5:
                Version = p;
                break;
            case 6:
                FontName = p;
                break;
            }
            if (Notice != NULL && FamilyName != NULL &&
                FullName != NULL && FontName != NULL && Version != NULL)
                break;
        }
        i += 6 * TTF_USHORT_SIZE;
    }
    if ((pd = name_lookup("PCLT")) != NULL) {
        ttf_seek_off("PCLT",
                     pd->offset + TTF_FIXED_SIZE + TTF_ULONG_SIZE +
                     TTF_USHORT_SIZE);
        XHeight = get_ushort();
        if (XHeight > FontBBox4) {
            ttf_warn("XHeight is too big (%i)\n"
                     "This is likely a font bug, so I discarded this parameter.",
                     (int) get_ttf_funit(XHeight));
            XHeight = 0;
        }
        ttf_skip(2 * TTF_USHORT_SIZE);
        CapHeight = get_ushort();
        if (CapHeight > FontBBox4) {
            ttf_warn("CapHeight is too big (%i)\n"
                     "This is likely a font bug, so I discarded this parameter.",
                     (int) get_ttf_funit(CapHeight));
            CapHeight = 0;
        }
    }
    if ((pd = name_lookup("OS/2")) != NULL) {
        ttf_seek_off("OS/2", pd->offset + TTF_USHORT_SIZE + TTF_SHORT_SIZE);
        switch (get_ushort()) {
        case 100:
            Weight = xstrdup("Thin");
            break;
        case 200:
            Weight = xstrdup("ExtraLight");
            break;
        case 300:
            Weight = xstrdup("Light");
            break;
        case 400:
            Weight = xstrdup("Normal");
            break;
        case 500:
            Weight = xstrdup("Medium");
            break;
        case 600:
            Weight = xstrdup("SemiBold");
            break;
        case 700:
            Weight = xstrdup("Bold");
            break;
        case 800:
            Weight = xstrdup("ExtraBold");
            break;
        case 900:
            Weight = xstrdup("Black");
            break;
        }
    }
    read_cmap();
    if ((pd = name_lookup("kern")) == NULL)
        return;
    kern_tab = xtalloc(nglyphs, kern_entry);
    for (pk = kern_tab; pk - kern_tab < nglyphs; pk++) {
        pk->next = NULL;
        pk->value = 0;
    }
    ttf_seek_off("kern", pd->offset + TTF_USHORT_SIZE);
    for (n = get_ushort(); n > 0; n--) {
        ttf_skip(2 * TTF_USHORT_SIZE);
        k = get_ushort();
        if (!(k & 1) || (k & 2) || (k & 4))
            return;
        if (k >> 8 != 0) {
            ttf_warn("warning: only format 0 supported of `kern' \
                 subtables, others are ignored\n");
            continue;
        }
        k = get_ushort();
        ttf_skip(3 * TTF_USHORT_SIZE);
        while (k-- > 0) {
            i = get_ushort();
            j = get_ushort();
            kern_value = get_fword();
            if (kern_value != 0) {
                store_kern_value(i, j, kern_value);
                nkernpairs++;
            }
        }
    }
}

static int null_glyph(const char *s)
{
    return (s == NULL || s == notdef);
    /*
       strcmp(s, ".null") == 0 ||
       strcmp(s, ".notdef") == 0 ||
       strcmp(s, "CR") == 0 ||
       strcmp(s, "nonmarkingreturn") == 0
     */
}

static void print_glyph_name(FILE * f, long glyph_index, int convention)
{
    unicode_entry *u;
    static char buf[1024];
    const char *n;
    assert(glyph_index >= 0 && glyph_index < nglyphs);
    n = mtx_tab[glyph_index].name;
    if (printing_enc && (n == notdef || glyph_index == 0)) {
        fputs(notdef, f);
        return;
    }
    switch (convention) {
    case AS_NAME:
        if (!null_glyph(n))
            fprintf(f, "%s", mtx_tab[glyph_index].name);
        else if (n == notdef && glyph_index == 0)
            fputs(notdef, f);
        else
            fprintf(f, "%s%li", GLYPH_PREFIX_INDEX, glyph_index);
        break;
    case AS_INDEX:
        fprintf(f, "%s%li", GLYPH_PREFIX_INDEX, glyph_index);
        break;
    case AS_UNICODE:
        u = mtx_tab[glyph_index].unicode_list;
        if (glyph_index == 0 || u == NULL)
            fprintf(f, "%s%li", GLYPH_PREFIX_INDEX, glyph_index);
        else {
            fprintf(f, "%s%.4X", GLYPH_PREFIX_UNICODE, u->code);
            if (u->next != NULL) {
                *buf = 0;
                for (; u != NULL; u = u->next) {
                    assert(strlen(buf) + strlen(GLYPH_PREFIX_UNICODE) + 4 <
                           sizeof(buf));
                    sprintf(strend(buf), "%s%.4X ", GLYPH_PREFIX_UNICODE,
                            u->code);
                }
                ttf_warn
                    ("glyph %li has multiple encodings (the first one being used): %s",
                     glyph_index, buf);
            }
        }
        break;
    }
}

static void print_char_metric(FILE * f, int charcode, long glyph_index)
{
    assert(glyph_index >= 0 && glyph_index < nglyphs);
    fprintf(f, "C %i ; WX %i ; N ", (int) charcode,
            (int) get_ttf_funit(mtx_tab[glyph_index].wx));
    print_glyph_name(f, glyph_index, print_glyph);
    fprintf(f, " ; B %i %i %i %i ;\n",
            (int) get_ttf_funit(mtx_tab[glyph_index].bbox[0]),
            (int) get_ttf_funit(mtx_tab[glyph_index].bbox[1]),
            (int) get_ttf_funit(mtx_tab[glyph_index].bbox[2]),
            (int) get_ttf_funit(mtx_tab[glyph_index].bbox[3]));
}

static char * escape_eol(char *s)
{
    char *p, *e;
    char *dest, *d;

    if (s == NULL)
        return NULL;

    dest = d = xtalloc(2*strlen(s), char);
    e = strend(s);
    for (p = s; p < e; p++) {
        if (*p == 10 || *p == 13) {
            *d++ = '\\';
            *d++ = 'n';
        }
        else
            *d++ = *p;
    }
    *d = 0;
    return dest;
}

static void print_afm(char *date, char *fontname)
{
    int ncharmetrics;
    mtx_entry *pm;
    long mtx_index[256], *idx;
    unsigned int index;
    char **pe;
    kern_entry *pk, *qk;
    char *p;
    double d;
    long l;
    fputs("StartFontMetrics 2.0\n", outfile);
    fprintf(outfile,
            "Comment Converted at %s by ttf2afm from font file `%s'\n", date,
            fontname);
    print_str(FontName);
    print_str(FullName);
    print_str(FamilyName);
    print_str(Weight);
    l = ItalicAngle >> 16;
    if (l > 0x8000)
        l = l - 0x10000;
    d = (ItalicAngle & 0xffff)/65536.0;
    if (d >= 0.1)
        fprintf(outfile, "ItalicAngle %.1f", d + l);
    else
        fprintf(outfile, "ItalicAngle %li", l);
    fputs("\n", outfile);
    fprintf(outfile, "IsFixedPitch %s\n", IsFixedPitch ? "true" : "false");
    fprintf(outfile, "FontBBox %i %i %i %i\n",
            (int) get_ttf_funit(FontBBox1),
            (int) get_ttf_funit(FontBBox2),
            (int) get_ttf_funit(FontBBox3), (int) get_ttf_funit(FontBBox4));
    print_dimen(UnderlinePosition);
    print_dimen(UnderlineThickness);
    print_str(Version);
    /* remove trailing whitespaces from Notice */
    if (Notice != NULL) {
        for (p = strend(Notice); p > Notice && isspace((unsigned char)*p); p--);
        *p = 0;
    }
    print_str(Notice);
    fputs("EncodingScheme FontSpecific\n", outfile);
    print_dimen(CapHeight);
    print_dimen(XHeight);
    print_dimen(Ascender);
    print_dimen(Descender);
    ncharmetrics = nglyphs;
    if (use_ext_enc == 0) {     /* external encoding vector not given */
        fprintf(outfile, "StartCharMetrics %u\n", ncharmetrics);
        for (pm = mtx_tab; pm - mtx_tab < nglyphs; pm++) {
            pm->found = 1;
            print_char_metric(outfile, -1, pm - mtx_tab);
        }
    } else {                    /* external encoding vector given */
        for (idx = mtx_index; idx - mtx_index < 256; *idx++ = 0);
        for (pe = enc_names; pe - enc_names < names_count; pe++) {
            if (*pe == notdef)
                continue;
            /* scan form `uniABCD' */
            if (sscanf(*pe, GLYPH_PREFIX_UNICODE "%4X", &index) == 1) {
                if (unicode_map[index] != NOGLYPH_ASSIGNED_YET) {
                    pm = mtx_tab + unicode_map[index];
                    mtx_index[pe - enc_names] = pm - mtx_tab;
                    pm->found = 1;
                } else
                    ttf_warn("`unicode %s%.4X' is not mapped to any glyph",
                             GLYPH_PREFIX_UNICODE, index);
                continue;
            }
            /* scan form `index123' */
            if (sscanf(*pe, GLYPH_PREFIX_INDEX "%u", &index) == 1) {
                if (index >= (unsigned int) nglyphs)
                    ttf_fail("`%s' out of valid range [0..%i)", *pe, nglyphs);
                pm = mtx_tab + index;
                mtx_index[pe - enc_names] = pm - mtx_tab;
                pm->found = 1;
                continue;
            }
            for (pm = mtx_tab; pm - mtx_tab < nglyphs; pm++)
                if (pm->name != NULL && strcmp(*pe, pm->name) == 0)
                    break;
            if (pm - mtx_tab < nglyphs) {
                mtx_index[pe - enc_names] = pm - mtx_tab;
                pm->found = 1;
                continue;
            } else
                ttf_warn("glyph `%s' not found", *pe);
        }
        fprintf(outfile, "StartCharMetrics %u\n", ncharmetrics);
        for (idx = mtx_index; idx - mtx_index < 256; idx++) {
            if (*idx != 0 && mtx_tab[*idx].found == 1)
                print_char_metric(outfile, idx - mtx_index, *idx);
        }
        for (pm = mtx_tab; pm - mtx_tab < nglyphs; pm++) {
            if (pm->found == 0)
                print_char_metric(outfile, -1, pm - mtx_tab);
        }
    }
    fputs("EndCharMetrics\n", outfile);
    if (nkernpairs == 0)
        goto end_kerns;
    fprintf(outfile, "StartKernData\nStartKernPairs %i\n", nkernpairs);
    for (pk = kern_tab; pk - kern_tab < nglyphs; pk++)
        for (qk = pk; qk != NULL; qk = qk->next)
            if (qk->value != 0) {
                fputs("KPX ", outfile);
                print_glyph_name(outfile, pk - kern_tab, print_glyph);
                fputs(" ", outfile);
                print_glyph_name(outfile, qk->adjacent, print_glyph);
                fprintf(outfile, " %i\n", get_ttf_funit(qk->value));
            }
    fputs("EndKernPairs\nEndKernData\n", outfile);
  end_kerns:
    fputs("EndFontMetrics\n", outfile);
}

static void print_encoding(char *fontname)
{
    long int i, k, first_code, length;
    FILE *file;
    TTF_USHORT format;
    char *enc_name, *end_enc_name;
    cmap_entry *e;
    printing_enc = 1;
    enc_name = xtalloc(strlen(bname) + 20, char);
    strcpy(enc_name, bname);
    end_enc_name = strend(enc_name);
    for (e = cmap_tab; e - cmap_tab < ncmapsubtabs; e++) {
        ttf_seek_off("cmap", cmap_offset + e->offset);
        format = get_ushort();
        if (format != 0 && format != 4 && format != 6) {
            ttf_warn("format %i of encoding subtable unsupported",
                     (int) format);
            continue;
        }
        sprintf(end_enc_name, ".e%i%i",
                (int) e->platform_id, (int) e->encoding_id);
        if ((file = xfopen(enc_name, FOPEN_W_MODE)) == NULL)
            ttf_fail("cannot open file for writing (%s)\n", enc_name);
        fprintf(file, "%% Encoding table from font file %s\n", fontname);
        fprintf(file, "%% Platform ID %i", (int) e->platform_id);
        switch (e->platform_id) {
        case 0:
            fprintf(file, " (Apple Unicode)");
            break;
        case 1:
            fprintf(file, " (Macintosh)");
            break;
        case 2:
            fprintf(file, " (ISO)");
            break;
        case 3:
            fprintf(file, " (Microsoft)");
            break;
        }
        fprintf(file, "\n");
        fprintf(file, "%% Encoding ID %i", (int) e->encoding_id);
        if (e->platform_id == 1 && e->encoding_id == 0)
            fprintf(file, " (Roman)");
        if (e->platform_id == 3)
            switch (e->encoding_id) {
            case 0:
                fprintf(file, " (Symbol)");
                break;
            case 1:
                fprintf(file, " (Unicode)");
                break;
            }
        fprintf(file, "\n");
        fprintf(file, "%% Format %i", (int) (format));
        switch (format) {
        case 0:
            fprintf(file, " (byte encoding table)");
            break;
        case 4:
            fprintf(file, " (segment mapping to delta values)");
            break;
        case 6:
            fprintf(file, " (trimmed table mapping)");
            break;
        }
        fprintf(file, "\n");
        fprintf(file, "/Encoding%i [\n", (int) (e - cmap_tab + 1));
        switch (format) {
        case 0:
            (void) get_ushort();        /* skip length */
            (void) get_ushort();        /* skip version number */
            for (i = 0; i < 256; i++) {
                fputs("/", file);
                print_glyph_name(file, get_byte(), print_glyph);
                fputs("\n", file);
            }
            break;
        case 4:
            for (i = 0; i < nglyphs; ++i) {
                fprintf(file, "%% Glyph %li -> ", i);
                print_glyph_name(file, i, AS_UNICODE);
                fputs("\n", file);
            }
            break;
        case 6:
            (void) get_ushort();        /* skip table length */
            (void) get_ushort();        /* skip version number */
            first_code = get_ushort();  /* first character code */
            for (i = 0; i < first_code; ++i)
                fprintf(file, "/%s\n", notdef);
            length = get_ushort();      /* number of character codes */
            for (i = first_code; i < first_code + length; i++) {
                k = get_ushort();
                if (i > 255)
                    fputs("% ", file);
                fputs("/", file);
                print_glyph_name(file, k, print_glyph);
                fputs("\n", file);
            }
            for (i = first_code + length; i < 256; i++)
                fprintf(file, "/%s\n", notdef);
            break;
        default:
            ttf_warn("format %i of encoding subtable unsupported",
                     (int) format);
        }
        fprintf(file, "] def\n");
    }
    xfree(enc_name);
}

static void usage(void)
{
    cur_file_name = NULL;
    fprintf(stderr,
            "Usage: ttf2afm [-i|-u|-c|-v] [-e enc] [-o filename] [-m NUM] fontfile\n"
            "    -i:          force printing glyph names in form 'index123'\n"
            "    -u:          force printing glyph names in form 'uniABCD'\n"
            "    -c:          print encoding tables to `basename.e<platformID><encodingID>'\n"
            "    -v:          print version\n"
            "    -e enc:      encode the AFM output using encoding vector from `enc'\n"
            "    -o filename: write output to file `filename' instead of stdout\n"
            "    -m NUM:      select unicode mapping (default = 1, ie the first)\n"
            "    fontfile:    the TrueType fontfile\n"
            "\nPlease send bug reports or feature requests to <pdftex@tug.org>\n");
    _exit(-1);
}

int main(int argc, char **argv)
{
    char date[128], *s;
    time_t t = time(&t);
    int c;
    kpse_set_program_name(argv[0], "ttf2afm");
    kpse_init_prog("ttf2afm", 0, 0, 0);
    while ((c = getopt(argc, argv, "iucve:o:m:")) != -1)
        switch (c) {
        case 'i':
            print_glyph = AS_INDEX;
            break;
        case 'u':
            print_glyph = AS_UNICODE;
            break;
        case 'c':
            print_cmap = 1;
            break;
        case 'e':
            cur_file_name = optarg;
            read_encoding(cur_file_name);
            use_ext_enc = 1;
            break;
        case 'o':
            cur_file_name = optarg;
            outfile = xfopen(cur_file_name, FOPEN_W_MODE);
            if (outfile == NULL)
                ttf_fail("cannot open file for writing");
            break;
        case 'm':
            select_unicode = atoi(optarg);
            break;
        case 'v':
            fprintf(stderr,
                    "ttf2afm version " VERSION "\n"
                    "Copyright (C) 1997-2014 Han The Thanh.\n"
                    "There is NO warranty.  Redistribution of this software is\n"
                    "covered by the terms of both the pdfTeX copyright and\n"
                    "the GNU General Public License.\n"
                    "For more information about these matters, see the files\n"
                    "named COPYING and the pdfTeX source.\n"
                    "Primary author of ttf2afm: Han The Thanh.\n");
            _exit(0);
        default:
            usage();
        }
    if (argc - optind != 1)
        usage();
    sprintf(date, "%s\n", ctime(&t));
    *(char *) strchr(date, '\n') = 0;
    cur_file_name = argv[optind];
    if (print_cmap) {
        bname = xstrdup(xbasename(cur_file_name));
        if ((s = strrchr(bname, '.')) != NULL)
            *s = 0;
    }
    if ((fontfile =
         kpse_open_file(cur_file_name, kpse_truetype_format)) == NULL)
        ttf_fail("can't open font file for reading");
    read_font();
    if (outfile == NULL)
        outfile = stdout;
    print_afm(date, cur_file_name);
    if (print_cmap)
        print_encoding(cur_file_name);
    xfree(FontName);
    xfree(FullName);
    xfree(FamilyName);
    xfree(Notice);
    xfree(Version);
    xfree(Weight);
    free_tabs();
    xfclose(fontfile, cur_file_name);
    return 0;
}
