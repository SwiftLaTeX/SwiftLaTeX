/*
Copyright 2006-2019 Han The Thanh, <thanh@pdftex.org>

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

#define isXdigit(c) (isdigit(c) || ('A' <= (c) && (c) <= 'F'))
#define UNI_UNDEF          -1
#define UNI_STRING         -2   /* string allocated by deftounicode() */
#define UNI_EXTRA_STRING   -3   /* string allocated by set_glyph_unicode() */

static struct avl_table *glyph_unicode_tree = NULL;

static int comp_glyph_unicode_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const glyph_unicode_entry *) pa)->name,
                  ((const glyph_unicode_entry *) pb)->name);
}

static glyph_unicode_entry *new_glyph_unicode_entry(void)
{
    glyph_unicode_entry *e;
    e = xtalloc(1, glyph_unicode_entry);
    e->name = NULL;
    e->code = UNI_UNDEF;
    e->unicode_seq = NULL;
    return e;
}

static void destroy_glyph_unicode_entry(void *pa, void *pb)
{
    glyph_unicode_entry *e = (glyph_unicode_entry *) pa;
    xfree(e->name);
    if (e->code == UNI_STRING) {
        assert(e->unicode_seq != NULL);
        xfree(e->unicode_seq);
    }
}

void glyph_unicode_free(void)
{
    if (glyph_unicode_tree != NULL)
        avl_destroy(glyph_unicode_tree, destroy_glyph_unicode_entry);
}

void deftounicode(strnumber glyph, strnumber unistr)
{
    char buf[SMALL_BUF_SIZE], *p;
    char buf2[SMALL_BUF_SIZE], *q;
    int valid_unistr;           /* 0: invalid; 1: unicode value; 2: string */
    int i, l;
    glyph_unicode_entry *gu, t;
    void **aa;

    p = makecstring(glyph);
    assert(strlen(p) < SMALL_BUF_SIZE);
    strcpy(buf, p);             /* copy the result to buf before next call of makecstring() */
    p = makecstring(unistr);
    while (*p == ' ')
        p++;                    /* ignore leading spaces */
    l = strlen(p);
    while (l > 0 && p[l - 1] == ' ')
        l--;                    /* ignore trailing spaces */
    valid_unistr = 1;           /* a unicode value is the most common case */
    for (i = 0; i < l; i++) {
        if (p[i] == ' ')
            valid_unistr = 2;   /* if a space occurs we treat this entry as a string */
        else if (!isXdigit((unsigned char)p[i])) {
            valid_unistr = 0;
            break;
        }
    }
    if (l == 0 || valid_unistr == 0 || strlen(buf) == 0
        || strcmp(buf, notdef) == 0) {
        pdftex_warn("ToUnicode: invalid parameter(s): `%s' => `%s'", buf, p);
        return;
    }
    if (glyph_unicode_tree == NULL) {
        glyph_unicode_tree =
            avl_create(comp_glyph_unicode_entry, NULL, &avl_xallocator);
        assert(glyph_unicode_tree != NULL);
    }
    t.name = buf;
    /* allow overriding existing entries */
    if ((gu = (glyph_unicode_entry *) avl_find(glyph_unicode_tree, &t)) != NULL) {
        if (gu->code == UNI_STRING) {
            assert(gu->unicode_seq != NULL);
            xfree(gu->unicode_seq);
        }
    } else {                    /* make new entry */
        gu = new_glyph_unicode_entry();
        gu->name = xstrdup(buf);
    }
    if (valid_unistr == 2) {    /* a string with space(s) */
        /* copy p to buf2, ignoring spaces */
        for (q = buf2; *p != 0; p++)
            if (*p != ' ')
                *q++ = *p;
        *q = 0;
        gu->code = UNI_STRING;
        gu->unicode_seq = xstrdup(buf2);
    } else {
        i = sscanf(p, "%lX", &(gu->code));
        assert(i == 1);
        if (gu->code < 0 || gu->code > 0x10FFFF) {
            pdftex_warn("ToUnicode: value out of range [0,10FFFF]: %lX",
                        gu->code);
            gu->code = UNI_UNDEF;
        }
    }
    aa = avl_probe(glyph_unicode_tree, gu);
    assert(aa != NULL);
}


static long check_unicode_value(const char *s, boolean multiple_value)
{
    int l = strlen(s);
    int i;
    long code;

    if (l == 0)
        return UNI_UNDEF;
    if (multiple_value && l % 4 != 0)
        return UNI_UNDEF;
    if (!multiple_value && !(4 <= l && l <= 6))
        return UNI_UNDEF;

    for (i = 0; i < l; i++) {
        if (!isXdigit((unsigned char)s[i]))
            return UNI_UNDEF;
        if (multiple_value) {
            if (i % 4 == 3) {
                if (sscanf(s + i - 3, "%4lX", &code) != 1)
                    return UNI_UNDEF;
                if (!((0x0000 <= code && code <= 0xD7FF) ||
                      (0xE000 <= code && code <= 0xFFFF)))
                    return UNI_UNDEF;
            }
        } else {                /* single value */
            if (i == l - 1) {
                if (sscanf(s, "%lX", &code) != 1)
                    return UNI_UNDEF;
                if (!((0x0000 <= code && code <= 0xD7FF) ||
                      (0xE000 <= code && code <= 0x10FFFF)))
                    return UNI_UNDEF;
            }
        }
    }
    return code;
}

static char *utf16be_str(long code)
{
    static char buf[SMALL_BUF_SIZE];
    long v;
    unsigned vh, vl;

    assert(code >= 0);

    if (code <= 0xFFFF)
        sprintf(buf, "%04lX", code);
    else {
        v = code - 0x10000;
        vh = v / 0x400 + 0xD800;
        vl = v % 0x400 + 0xDC00;
        sprintf(buf, "%04X%04X", vh, vl);
    }
    return buf;
}


/* this function writes /ToUnicode data to *gp based on glyph name s and
 * taking into account tfmname; in case it returns
 * gp->code == UNI_EXTRA_STRING then the caller is responsible for freeing
 * gp->unicode_seq too */
static void set_glyph_unicode(const char *s, const char* tfmname, 
                              glyph_unicode_entry *gp)
{
    char buf[SMALL_BUF_SIZE], buf2[SMALL_BUF_SIZE], *p;
    const char *p2; /* p2 points in s; p above points in writable copies */
    long code;
    boolean last_component;
    glyph_unicode_entry tmp, *ptmp;

    /* skip dummy entries */
    if (s == NULL || s == notdef)
        return;

    /* strip everything after the first dot */
    p = strchr(s, '.');
    if (p != NULL) {
        *buf = 0;
        strncat(buf, s, p - s);
        s = buf;
    }

    if (strlen(s) == 0)
        return;

    /* check for case of multiple components separated by '_' */
    p = strchr(s, '_');
    if (p != NULL) {
        assert(strlen(s) < sizeof(buf));
        if (s != buf) {
            strcpy(buf, s);
            p = strchr(buf, '_');
            s = buf;
        }
        *buf2 = 0;
        last_component = false;
        for (;;) {
            *p = 0;
            tmp.code = UNI_UNDEF;
            set_glyph_unicode(s, tfmname, &tmp);
            switch (tmp.code) {
            case UNI_UNDEF:    /* not found, do nothing */
                break;
            case UNI_STRING:   /* s matched an entry with string value in the database */
                assert(tmp.unicode_seq != NULL);
                assert(strlen(buf2) + strlen(tmp.unicode_seq) < sizeof(buf2));
                strcat(buf2, tmp.unicode_seq);
                break;
            case UNI_EXTRA_STRING:     /* s is a multiple value of form "uniXXXX" */
                assert(strlen(buf2) + strlen(tmp.unicode_seq) < sizeof(buf2));
                strcat(buf2, tmp.unicode_seq);
                xfree(tmp.unicode_seq);
                break;
            default:           /* s matched an entry with numeric value in the
                                   database, or a value derived from "uXXXX" */
                assert(tmp.code >= 0);
                strcat(buf2, utf16be_str(tmp.code));
            }
            if (last_component)
                break;
            s = p + 1;
            p = strchr(s, '_');
            if (p == NULL) {
                p = strend(s);
                last_component = true;
            }
        }
        gp->code = UNI_EXTRA_STRING;
        gp->unicode_seq = xstrdup(buf2);
        return;
    }

    /* Glyph name search strategy: first look up the glyph name in the
       tfm's namespace, failing that look it up in the main database. */
    /* Note: buf may alias s in the code below, but s and buf2 are
       guaranteed to be distinct because the code changing buf2 above
       always returns before reaching the code below. */

    /* lookup for glyph name in the tfm's namespace */
    snprintf(buf2, SMALL_BUF_SIZE, "tfm:%s/%s", tfmname, s);
    tmp.name = buf2;
    tmp.code = UNI_UNDEF;
    ptmp = (glyph_unicode_entry *) avl_find(glyph_unicode_tree, &tmp);
    if (ptmp != NULL) {
        gp->code = ptmp->code;
        gp->unicode_seq = ptmp->unicode_seq;
        return;
    }

    /* lookup for glyph name in the main database */
    snprintf(buf2, SMALL_BUF_SIZE, "%s", s);
    tmp.name = buf2;
    tmp.code = UNI_UNDEF;
    ptmp = (glyph_unicode_entry *) avl_find(glyph_unicode_tree, &tmp);
    if (ptmp != NULL) {
        gp->code = ptmp->code;
        gp->unicode_seq = ptmp->unicode_seq;
        return;
    }

    /* check for case of "uniXXXX" (multiple 4-hex-digit values allowed) */
    if (str_prefix(s, "uni")) {
        p2 = s + strlen("uni");
        code = check_unicode_value(p2, true);
        if (code != UNI_UNDEF) {
            if (strlen(p2) == 4) /* single value */
                gp->code = code;
            else {              /* multiple value */
                gp->code = UNI_EXTRA_STRING;
                gp->unicode_seq = xstrdup(p2);
            }
        }
        return;                 /* since the last case cannot happen */
    }

    /* check for case of "uXXXX" (single value up to 6 hex digits) */
    if (str_prefix(s, "u")) {
        p2 = s + strlen("u");
        code = check_unicode_value(p2, false);
        if (code != UNI_UNDEF) {
            assert(code >= 0);
            gp->code = code;
        }
    }
}


/* tfmname is without .tfm extension, but encname ends in .enc; */
integer write_tounicode(char **glyph_names, const char *tfmname,
                        const char* encname)
{
    char buf[SMALL_BUF_SIZE], *p;
    static char builtin_suffix[] = "builtin";
    short range_size[257];
    glyph_unicode_entry gtab[257];
    integer objnum;
    int i, j;
    int bfchar_count, bfrange_count, subrange_count;

    if (glyph_unicode_tree == NULL) {
        pdftex_warn("no GlyphToUnicode entry has been inserted yet!");
        fixedgentounicode = 0;
        return 0;
    }

    /* build the name for this CMap as <tfmname>-<encname>; if encname is NULL
     * then this is a builtin encoding so use instead builtin_suffix */
    strcpy(buf, tfmname);
    strcat(buf, "-");
    if (encname) {
        assert(strlen(tfmname) + strlen(encname) + 1 < SMALL_BUF_SIZE);
        strcat(buf, encname);
        if ((p = strrchr(buf, '.')) != NULL && strcmp(p, ".enc") == 0)
            *p = 0;                 /* strip ".enc" from encoding name */
        else /* some silly encoding file name not ending in enc; use as-is */
            pdftex_warn("Dubious encoding file name: `%s'", encname);
    } else { /* this is a builtin encoding, so name is e.g. "cmr10-builtin" */
        assert(strlen(tfmname) + strlen(builtin_suffix) + 1 < SMALL_BUF_SIZE);
        strcat(buf, builtin_suffix);    
    }

    objnum = pdfnewobjnum();
    pdfbegindict(objnum, 0);
    pdfbeginstream();
    pdf_printf("%%!PS-Adobe-3.0 Resource-CMap\n"
               "%%%%DocumentNeededResources: ProcSet (CIDInit)\n"
               "%%%%IncludeResource: ProcSet (CIDInit)\n"
               "%%%%BeginResource: CMap (TeX-%s-0)\n"
               "%%%%Title: (TeX-%s-0 TeX %s 0)\n"
               "%%%%Version: 1.000\n"
               "%%%%EndComments\n"
               "/CIDInit /ProcSet findresource begin\n"
               "12 dict begin\n"
               "begincmap\n"
               "/CIDSystemInfo\n"
               "<< /Registry (TeX)\n"
               "/Ordering (%s)\n"
               "/Supplement 0\n"
               ">> def\n"
               "/CMapName /TeX-%s-0 def\n"
               "/CMapType 2 def\n"
               "1 begincodespacerange\n"
               "<00> <FF>\n" "endcodespacerange\n", buf, buf, buf, buf, buf);

    /* set gtab */
    for (i = 0; i < 256; ++i) {
        gtab[i].code = UNI_UNDEF;
        set_glyph_unicode(glyph_names[i], tfmname, &gtab[i]);
    }
    gtab[256].code = UNI_UNDEF;

    /* set range_size */
    for (i = 0; i < 256;) {
        if (gtab[i].code == UNI_STRING || gtab[i].code == UNI_EXTRA_STRING) {
            range_size[i] = 1;  /* single entry */
            i++;
        } else if (gtab[i].code == UNI_UNDEF) {
            range_size[i] = 0;  /* no entry */
            i++;
        } else {                /* gtab[i].code >= 0 */
            j = i;
            while (i < 256 && gtab[i + 1].code >= 0 &&
                   gtab[i].code + 1 == gtab[i + 1].code)
                i++;
            /* at this point i is the last entry of the subrange */
            i++;                /* move i to the next entry */
            range_size[j] = i - j;
        }
    }

    /* calculate bfrange_count and bfchar_count */
    bfrange_count = 0;
    bfchar_count = 0;
    for (i = 0; i < 256;) {
        if (range_size[i] == 1) {
            bfchar_count++;
            i++;
        } else if (range_size[i] > 1) {
            bfrange_count++;
            i += range_size[i];
        } else
            i++;
    }

    /* write out bfrange */
    i = 0;
  write_bfrange:
    if (bfrange_count > 100)
        subrange_count = 100;
    else
        subrange_count = bfrange_count;
    bfrange_count -= subrange_count;
    pdf_printf("%i beginbfrange\n", subrange_count);
    for (j = 0; j < subrange_count; j++) {
        while (range_size[i] <= 1 && i < 256)
            i++;
        assert(i < 256);
        pdf_printf("<%02X> <%02X> <%s>\n", i, i + range_size[i] - 1,
                   utf16be_str(gtab[i].code));
        i += range_size[i];
    }
    pdf_printf("endbfrange\n");
    if (bfrange_count > 0)
        goto write_bfrange;

    /* write out bfchar */
    i = 0;
  write_bfchar:
    if (bfchar_count > 100)
        subrange_count = 100;
    else
        subrange_count = bfchar_count;
    bfchar_count -= subrange_count;
    pdf_printf("%i beginbfchar\n", subrange_count);
    for (j = 0; j < subrange_count; j++) {
        while (i < 256) {
            if (range_size[i] > 1)
                i += range_size[i];
            else if (range_size[i] == 0)
                i++;
            else                /* range_size[i] == 1 */
                break;
        }
        assert(i < 256 && gtab[i].code != UNI_UNDEF);
        if (gtab[i].code == UNI_STRING || gtab[i].code == UNI_EXTRA_STRING) {
            assert(gtab[i].unicode_seq != NULL);
            pdf_printf("<%02X> <%s>\n", i, gtab[i].unicode_seq);
        } else
            pdf_printf("<%02X> <%s>\n", i, utf16be_str(gtab[i].code));
        i++;
    }
    pdf_printf("endbfchar\n");
    if (bfchar_count > 0)
        goto write_bfchar;

    /* free strings allocated by set_glyph_unicode() */
    for (i = 0; i < 256; ++i) {
        if (gtab[i].code == UNI_EXTRA_STRING)
            xfree(gtab[i].unicode_seq);
    }

    pdf_printf("endcmap\n"
               "CMapName currentdict /CMap defineresource pop\n"
               "end\n" "end\n" "%%%%EndResource\n" "%%%%EOF\n");
    pdfendstream();
    return objnum;
}
