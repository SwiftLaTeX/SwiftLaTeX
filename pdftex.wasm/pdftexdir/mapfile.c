/* mapfile.c: handling of map files/lines

Copyright 1996-2018 Han The Thanh, <thanh@pdftex.org>

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
#include <math.h>
#include <string.h>

#define FM_BUF_SIZE     1024

static FILE *fm_file;

#define fm_open()       \
    open_input (&fm_file, kpse_fontmap_format, FOPEN_RBIN_MODE)
#define fm_close()      xfclose(fm_file, cur_file_name)
#define fm_getchar()    xgetc(fm_file)
#define fm_eof()        feof(fm_file)

typedef enum { FM_DUPIGNORE, FM_REPLACE, FM_DELETE } updatemode;
typedef enum { MAPFILE, MAPLINE } maptype;

typedef struct mitem {
    updatemode mode;            /* FM_DUPIGNORE or FM_REPLACE or FM_DELETE */
    maptype type;               /* map file or map line */
    char *line;                 /* pointer to map file name or map line */
    int lineno;                 /* line number in map file */
} mapitem;
mapitem *mitem = NULL;

static const char nontfm[] = "<nontfm>";

#define read_field(r, q, buf, buf_size) do {  \
    q = buf;                        \
    while (*r != ' ' && *r != '<' && *r != '"' && *r != '\0') { \
        check_buf(q - (buf) + 1, (buf_size)); \
        *q++ = *r++;                \
    }                               \
    *q = '\0';                      \
    skip (r, ' ');                  \
} while (0)

#define set_field(F) do {           \
    if (q > buf)                    \
        fm->F = xstrdup(buf);       \
    if (*r == '\0')                 \
        goto done;                  \
} while (0)

fm_entry *new_fm_entry(void)
{
    fm_entry *fm;
    fm = xtalloc(1, fm_entry);
    fm->tfm_name = NULL;
    fm->sfd_name = NULL;
    fm->ps_name = NULL;
    fm->fd_flags = FD_FLAGS_NOT_SET_IN_MAPLINE;
    fm->ff_name = NULL;
    fm->encname = NULL;
    fm->type = 0;
    fm->slant = 0;
    fm->extend = 0;
    fm->links = 0;
    fm->pid = -1;
    fm->eid = -1;
    fm->subfont = NULL;
    fm->in_use = false;
    return fm;
}

void delete_fm_entry(fm_entry * fm)
{
    xfree(fm->tfm_name);
    xfree(fm->sfd_name);
    xfree(fm->ps_name);
    xfree(fm->ff_name);
    xfree(fm);
}

static ff_entry *new_ff_entry(void)
{
    ff_entry *ff;
    ff = xtalloc(1, ff_entry);
    ff->ff_name = NULL;
    ff->ff_path = NULL;
    return ff;
}

static void delete_ff_entry(ff_entry * ff)
{
    xfree(ff->ff_name);
    xfree(ff->ff_path);
    xfree(ff);
}

static fm_entry *dummy_fm_entry(void)
{
    static fm_entry const_fm_entry;
    return & const_fm_entry;
}

/**********************************************************************/

struct avl_table *tfm_tree = NULL;
struct avl_table *ps_tree = NULL;
struct avl_table *ff_tree = NULL;
struct avl_table *encname_tree = NULL;

/* AVL sort fm_entry into tfm_tree by tfm_name */

static int comp_fm_entry_tfm(const void *pa, const void *pb, void *p)
{
    return strcmp(((const fm_entry *) pa)->tfm_name,
                  ((const fm_entry *) pb)->tfm_name);
}

/* AVL sort fm_entry into ps_tree by ps_name, slant, and extend */

static int comp_fm_entry_ps(const void *pa, const void *pb, void *p)
{
    int i;
    const fm_entry *p1 = (const fm_entry *) pa, *p2 = (const fm_entry *) pb;
    assert(p1->ps_name != NULL && p2->ps_name != NULL);
    if ((i = strcmp(p1->ps_name, p2->ps_name)) != 0)
        return i;
    cmp_return(p1->slant, p2->slant);
    cmp_return(p1->extend, p2->extend);
    return 0;
}

/* AVL sort ff_entry into ff_tree by ff_name */

static int comp_ff_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const ff_entry *) pa)->ff_name,
                  ((const ff_entry *) pb)->ff_name);
}

static void create_avl_trees(void)
{
    assert(tfm_tree == NULL);
    tfm_tree = avl_create(comp_fm_entry_tfm, NULL, &avl_xallocator);
    assert(tfm_tree != NULL);
    assert(ps_tree == NULL);
    ps_tree = avl_create(comp_fm_entry_ps, NULL, &avl_xallocator);
    assert(ps_tree != NULL);
    assert(ff_tree == NULL);
    ff_tree = avl_create(comp_ff_entry, NULL, &avl_xallocator);
    assert(ff_tree != NULL);
    assert(encname_tree == NULL);
    encname_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(encname_tree != NULL);
}

/**********************************************************************/
/*
The function avl_do_entry() is not completely symmetrical with regards
to tfm_name and ps_name handling, e. g. a duplicate tfm_name gives a
"goto exit", and no ps_name link is tried. This is to keep it compatible
with the original version.
*/

int avl_do_entry(fm_entry * fm, int mode)
{
    fm_entry *p;
    void *a;
    void **aa;
    boolean suppress_warn = (getpdfsuppresswarningdupmap() > 0);

    /* handle tfm_name link */

    if (strcmp(fm->tfm_name, nontfm) != 0) {
        p = (fm_entry *) avl_find(tfm_tree, fm);
        if (p != NULL) {
            switch (mode) {
            case FM_DUPIGNORE:
                if (!suppress_warn)
                    pdftex_warn
                        ("fontmap entry for `%s' already exists, duplicates ignored",
                         fm->tfm_name);
                goto exit;
                break;
            case FM_REPLACE:
            case FM_DELETE:
                if (p->in_use) {
                    pdftex_warn
                        ("fontmap entry for `%s' has been used, replace/delete not allowed",
                         fm->tfm_name);
                    goto exit;
                }
                a = avl_delete(tfm_tree, p);
                assert(a != NULL);
                unset_tfmlink(p);
                if (!has_pslink(p))
                    delete_fm_entry(p);
                break;
            default:
                assert(0);
            }
        }
        if (mode != FM_DELETE) {
            aa = avl_probe(tfm_tree, fm);
            assert(aa != NULL);
            set_tfmlink(fm);
        }
    }

    /* handle ps_name link */

    if (fm->ps_name != NULL) {
        p = (fm_entry *) avl_find(ps_tree, fm);
        if (p != NULL) {
            switch (mode) {
            case FM_DUPIGNORE:
                goto exit;
                break;
            case FM_REPLACE:
            case FM_DELETE:
                if (p->in_use)
                    goto exit;
                a = avl_delete(ps_tree, p);
                assert(a != NULL);
                unset_pslink(p);
                if (!has_tfmlink(p))
                    delete_fm_entry(p);
                break;
            default:
                assert(0);
            }
        }
        if (mode != FM_DELETE && is_t1fontfile(fm) && is_included(fm)) {
            aa = avl_probe(ps_tree, fm);
            assert(aa != NULL);
            set_pslink(fm);
        }
    }
  exit:
    if (!has_tfmlink(fm) && !has_pslink(fm))    /* e. g. after FM_DELETE */
        return 1;               /* deallocation of fm_entry structure required */
    else
        return 0;
}

/* add the encoding name to an AVL tree. this has nothing to do with writeenc.c */

static char *add_encname(char *s)
{
    char *p;
    void **aa;
    assert(s != NULL);
    assert(encname_tree != NULL);
    if ((p = (char *) avl_find(encname_tree, s)) == NULL) {     /* encoding name not yet registered */
        p = xstrdup(s);
        aa = avl_probe(encname_tree, p);
        assert(aa != NULL);
    }
    return p;
}


/**********************************************************************/
/* consistency check for map entry, with warn flag; return zero if ok,
   nonzero bits of warnings otherwise. */

static int check_fm_entry(fm_entry * fm, boolean warn)
{
    int a = 0;
    assert(fm != NULL);
    assert(fm->tfm_name);

    if (is_fontfile(fm) && !is_included(fm)) {
        if (warn)
            pdftex_warn
              ("ambiguous entry for `%s': font file present but not included, "
               "will be treated as font file not present", fm->tfm_name);
        xfree(fm->ff_name);
        /* do not set variable |a| as this entry will still be accepted */
    }

    /* if no tfm name, nothing to do here; a bare tfm is ok */
    if (*(fm->tfm_name) == '\0') {
        if (warn)
            pdftex_warn ("invalid map entry: tfm missing");
        a += 1;
    }

    /* TrueType fonts cannot be reencoded without subsetting */
    if (is_truetype(fm) && is_reencoded(fm) && !is_subsetted(fm)) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': only subsetted TrueType fonts can be reencoded",
                 fm->tfm_name);
        a += 2;
    }

    /* SlantFont and ExtendFont can be used only with Type1 fonts */
    if ((fm->slant != 0 || fm->extend != 0)
        && (strlen(fm->tfm_name) == 0
            || !(is_t1fontfile(fm) && is_included(fm)))) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': SlantFont/ExtendFont can be used only with embedded Type1 fonts",
                 fm->tfm_name);
        a += 4;
    }

    /* the value of SlantFont and ExtendFont must be reasonable */
    if (abs(fm->slant) > 1000) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': SlantFont value too big: %g",
                 fm->tfm_name, fm->slant / 1000.0);
        a += 8;
    }
    if (abs(fm->extend) > 2000) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': ExtendFont value too big: %g",
                 fm->tfm_name, fm->extend / 1000.0);
        a += 16;
    }

    /* subfonts must be used with subsetted non-reencoded TrueType fonts */
    if (fm->pid != -1 &&
        !(is_truetype(fm) && is_subsetted(fm) && !is_reencoded(fm))) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': PidEid can be used only with subsetted non-reencoded TrueType fonts",
                 fm->tfm_name);
        a += 32;
    }

    /* font file for bitmap PK fonts is determinated by kpse and depends
       on current font size; writet3.c ignores font file for bitmap PK fonts */
    if (is_fontfile(fm) && is_pk(fm)) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': "
                 "FontFile cannot be specified for bitmap PK font: %s",
                 fm->tfm_name, fm_fontfile(fm));
        a += 64;
    }

    /* ps name cannot be stored into PDF file for PDF Type3 fonts */
    if (fm->ps_name != NULL && is_pk(fm)) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': "
                 "PsName cannot be specified for bitmap PK font: %s",
                 fm->tfm_name, fm->ps_name);
        a += 128;
    }

    return a;
}

/**********************************************************************/
/* returns the font number if s is one of the 14 std. font names, -1 otherwise; speed-trimmed. */

int check_std_t1font(char *s)
{
    static const char *std_t1font_names[] = {
        "Courier",              /* 0:7 */
        "Courier-Bold",         /* 1:12 */
        "Courier-Oblique",      /* 2:15 */
        "Courier-BoldOblique",  /* 3:19 */
        "Helvetica",            /* 4:9 */
        "Helvetica-Bold",       /* 5:14 */
        "Helvetica-Oblique",    /* 6:17 */
        "Helvetica-BoldOblique",        /* 7:21 */
        "Symbol",               /* 8:6 */
        "Times-Roman",          /* 9:11 */
        "Times-Bold",           /* 10:10 */
        "Times-Italic",         /* 11:12 */
        "Times-BoldItalic",     /* 12:16 */
        "ZapfDingbats"          /* 13:12 */
    };
    static const int index[] =
        { -1, -1, -1, -1, -1, -1, 8, 0, -1, 4, 10, 9, -1, -1, 5, 2, 12, 6, -1,
        3, -1, 7
    };
    size_t n;
    int k = -1;
    assert(s != NULL);
    n = strlen(s);
    if (n > 21)
        return -1;
    if (n == 12) {              /* three names have length 12 */
        switch (*s) {
        case 'C':
            k = 1;              /* Courier-Bold */
            break;
        case 'T':
            k = 11;             /* Times-Italic */
            break;
        case 'Z':
            k = 13;             /* ZapfDingbats */
            break;
        default:
            return -1;
        }
    } else
        k = index[n];
    if (k > -1 && strcmp(std_t1font_names[k], s) == 0)
        return k;
    return -1;
};

/**********************************************************************/

static void fm_scan_line(void)
{
    int a, b, c, j, u = 0, v = 0;
    float d;
    fm_entry *fm;
    char fm_line[FM_BUF_SIZE], buf[FM_BUF_SIZE];
    char *p, *q, *r, *s;
    switch (mitem->type) {
    case MAPFILE:
        p = fm_line;
        do {
            c = fm_getchar();
            append_char_to_buf(c, p, fm_line, FM_BUF_SIZE);
        }
        while (c != 10);
        *(--p) = '\0';
        r = fm_line;
        break;
    case MAPLINE:
        r = mitem->line;        /* work on string from makecstring() */
        break;
    default:
        assert(0);
    }
    if (*r == '\0' || is_cfg_comment(*r))
        return;
    fm = new_fm_entry();
    read_field(r, q, buf, FM_BUF_SIZE);
    set_field(tfm_name);
    if (fm->tfm_name == NULL) /* used in messages, so don't let be null */
      fm->tfm_name = xstrdup ("");
    if (!isdigit((unsigned char)*r)) {         /* 2nd field ps_name may not start with a digit */
        read_field(r, q, buf, FM_BUF_SIZE);
        set_field(ps_name);
    }
    if (isdigit((unsigned char)*r)) {          /* font descriptor /Flags given? */
        for (s = r; isdigit((unsigned char)*s); s++);
        if (*s == ' ' || *s == '"' || *s == '<' || *s == '\0') {        /* not e. g. 8r.enc */
            fm->fd_flags = atoi(r);
            while (isdigit((unsigned char)*r))
                r++;
        }
    }
    while (1) {                 /* loop through "specials", encoding, font file */
        skip(r, ' ');
        switch (*r) {
        case '\0':
            goto done;
        case '"':              /* opening quote */
            r++;
            u = v = 0;
            do {
                skip(r, ' ');
                if (sscanf(r, "%f %n", &d, &j) > 0) {
                    s = r + j;  /* jump behind number, eat also blanks, if any */
                    if (*(s - 1) == 'E' || *(s - 1) == 'e')
                        s--;    /* e. g. 0.5ExtendFont: %f = 0.5E */
                    if (str_prefix(s, "SlantFont")) {
                        d *= 1000.0;    /* correct rounding also for neg. numbers */
                        fm->slant = (integer) (d > 0 ? d + 0.5 : d - 0.5);
                        r = s + strlen("SlantFont");
                    } else if (str_prefix(s, "ExtendFont")) {
                        d *= 1000.0;
                        fm->extend = (integer) (d > 0 ? d + 0.5 : d - 0.5);
                        if (fm->extend == 1000)
                            fm->extend = 0;
                        r = s + strlen("ExtendFont");
                    } else {    /* unknown name */
                        for (r = s; *r != ' ' && *r != '"' && *r != '\0'; r++); /* jump over name */
                        c = *r; /* remember char for temporary end of string */
                        *r = '\0';
                        pdftex_warn
                            ("invalid entry for `%s': unknown name `%s' ignored",
                             fm->tfm_name, s);
                        *r = c;
                    }
                } else
                    for (; *r != ' ' && *r != '"' && *r != '\0'; r++);
            }
            while (*r == ' ');
            if (*r == '"')      /* closing quote */
                r++;
            else {
                pdftex_warn
                    ("invalid entry for `%s': closing quote missing",
                     fm->tfm_name);
                goto bad_line;
            }
            break;
        case 'P':              /* handle cases for subfonts like 'PidEid=3,1' */
            if (sscanf(r, "PidEid=%i, %i %n", &a, &b, &c) >= 2) {
                fm->pid = a;
                fm->eid = b;
                r += c;
                break;
            }
        default:               /* encoding or font file specification */
            a = b = 0;
            if (*r == '<') {
                a = *r++;
                if (*r == '<' || *r == '[')
                    b = *r++;
            }
            read_field(r, q, buf, FM_BUF_SIZE);
            /* encoding, formats: '8r.enc' or '<8r.enc' or '<[8r.enc' */
            if (strlen(buf) > 4 && strcasecmp(strend(buf) - 4, ".enc") == 0) {
                fm->encname = add_encname(buf);
                u = v = 0;      /* u, v used if intervening blank: "<< foo" */
            } else if (strlen(buf) > 0) {       /* file name given */
                /* font file, formats:
                 * subsetting:    '<cmr10.pfa'
                 * no subsetting: '<<cmr10.pfa'
                 * no embedding:  'cmr10.pfa'
                 */
                if (a == '<' || u == '<') {
                    set_included(fm);
                    if ((a == '<' && b == 0) || (a == 0 && v == 0))
                        set_subsetted(fm);
                    /* otherwise b == '<' (or '[') => no subsetting */
                }
                set_field(ff_name);
                u = v = 0;
            } else {
                u = a;
                v = b;
            }
        }
    }
  done:
    if (fm->ps_name != NULL && (check_std_t1font(fm->ps_name) >= 0))
        set_std_t1font(fm);
    if (is_fontfile(fm) && strlen(fm_fontfile(fm)) > 3) {
        if (strcasecmp(strend(fm_fontfile(fm)) - 4, ".ttf") == 0)
            set_truetype(fm);
        else if (strcasecmp(strend(fm_fontfile(fm)) - 4, ".ttc") == 0)
            set_truetype(fm);
        else if (strcasecmp(strend(fm_fontfile(fm)) - 4, ".otf") == 0)
            set_opentype(fm);
        else
            set_type1(fm);
    } else if (fm->ps_name == NULL) {
        set_pk(fm);             /* font without ps_name and without fontfile,
                                   it can be only bitmap PK font */
    } else
        set_type1(fm);          /* assume a builtin font is Type1 */

    if (check_fm_entry(fm, true) != 0)
        goto bad_line;
    /*
       If we get here, the map line has been completely scanned without errors;
       fm points to a valid, freshly filled-out fm_entry structure.
       Now follows the actual work of registering/deleting.
     */
    if (handle_subfont_fm(fm, mitem->mode))     /* is this a subfont? */
        return;
    if (avl_do_entry(fm, mitem->mode) == 0)     /* if success */
        return;
  bad_line:
    delete_fm_entry(fm);
}

/**********************************************************************/

void fm_read_info(void)
{
    if (tfm_tree == NULL)
        create_avl_trees();
    if (mitem->line == NULL)    /* nothing to do */
        return;
    mitem->lineno = 1;
    switch (mitem->type) {
    case MAPFILE:
        set_cur_file_name(mitem->line);
        if (!fm_open()) {
            pdftex_warn("cannot open font map file");
        } else {
            cur_file_name = (char *) nameoffile + 1;
            tex_printf("{%s", cur_file_name);
            while (!fm_eof()) {
                fm_scan_line();
                mitem->lineno++;
            }
            fm_close();
            tex_printf("}");
            fm_file = NULL;
        }
        break;
    case MAPLINE:
        cur_file_name = NULL;   /* makes pdftex_warn() shorter */
        fm_scan_line();
        break;
    default:
        assert(0);
    }
    mitem->line = NULL;         /* done with this line */
    cur_file_name = NULL;
    return;
}

/**********************************************************************/

static fmentryptr fmlookup(internalfontnumber f)
{
    char *tfm;
    fm_entry *fm;
    fm_entry tmp;
    if (tfm_tree == NULL)
        fm_read_info();         /* only to read default map file */
    tfm = makecstring(fontname[f]);
    assert(strcmp(tfm, nontfm) != 0);

    /* Look up for full <tfmname>[+-]<expand> */
    tmp.tfm_name = tfm;
    fm = (fm_entry *) avl_find(tfm_tree, &tmp);
    if (fm != NULL) {
        fm->in_use = true;
        return (fmentryptr) fm;
    }
    return (fmentryptr) dummy_fm_entry();
}

boolean hasfmentry(internalfontnumber f)
{
    if (pdffontmap[f] == NULL)
        pdffontmap[f] = fmlookup(f);
    assert(pdffontmap[f] != NULL);
    return pdffontmap[f] != (fmentryptr) dummy_fm_entry();
}

boolean isscalable(internalfontnumber f)
{
    return hasfmentry(f) && (!is_pk((fm_entry *)pdffontmap[f]));
}

/* check whether a map entry is valid for font replacement */

static boolean fm_valid_for_font_replacement(fm_entry * fm)
{
    ff_entry *ff;

    assert(fm != NULL);
    assert(is_fontfile(fm));    /* ps_tree should contain only entries with font file */
    assert(is_type1(fm));       /* ps_tree should contain only Type1 entries */

    ff = check_ff_exist(fm->ff_name, false);
    assert(ff != NULL);
    if (ff->ff_path == NULL)    /* ...there is no font file available */
        return false;
    return true;                /* all tests passed */
}

/**********************************************************************/
/*
 * lookup fontmap by ps_name;
 * used for Type1 font replacement when embedding of PDF files
 */

fm_entry *lookup_fontmap(char *ps_name)
{
    fm_entry *fm, *fm2, tmp;
    char *a, *b, *c, *d, *e, *s;
    int i, sl, ex;
    struct avl_traverser t, t2;
    if (tfm_tree == NULL)
        fm_read_info();         /* only to read default map file */
    assert(ps_name != NULL);
    s = ps_name;
    if (strlen(ps_name) > 7) {  /* check for subsetted name tag */
        for (i = 0; i < 6; i++, s++)
            if (*s < 'A' || *s > 'Z')
                break;
        if (i == 6 && *s == '+')
            s++;                /* if name tag found, skip behind it */
        else
            s = ps_name;
    }

    /*
     * Scan -Slant_<slant> and -Extend_<extend> font name extensions;
     * three valid formats:
     * <fontname>-Slant_<slant>
     * <fontname>-Slant_<slant>-Extend_<extend>
     * <fontname>-Extend_<extend>
     * Slant entry must come _before_ Extend entry
     */

    tmp.slant = 0;
    tmp.extend = 0;
    if ((a = strstr(s, "-Slant_")) != NULL) {
        b = a + strlen("-Slant_");
        sl = (int) strtol(b, &e, 10);
        if ((e != b) && (e == strend(b))) {
            tmp.slant = sl;
            *a = '\0';          /* ps_name string ends before "-Slant_" */
        } else {
            if (e != b) {       /* only if <slant> is valid number */
                if ((c = strstr(e, "-Extend_")) != NULL) {
                    d = c + strlen("-Extend_");
                    ex = (int) strtol(d, &e, 10);
                    if ((e != d) && (e == strend(d))) {
                        tmp.slant = sl;
                        tmp.extend = ex;
                        *a = '\0';      /* ps_name string ends before "-Slant_" */
                    }
                }
            }
        }
    } else {
        if ((a = strstr(s, "-Extend_")) != NULL) {
            b = a + strlen("-Extend_");
            ex = (int) strtol(b, &e, 10);
            if ((e != b) && (e == strend(b))) {
                tmp.extend = ex;
                *a = '\0';      /* ps_name string ends before "-Extend_" */
            }
        }
    }
    tmp.ps_name = s;

    fm = (fm_entry *) avl_t_find(&t, ps_tree, &tmp);
    if (fm == NULL)
        return NULL;            /* no entry found */

    /* at this point we know there is at least one fm_entry with given ps_name;
     * we test all such entries and return the first one that is valid for font
     * replacement */

    t2 = t;
    fm2 = (fm_entry *) avl_t_prev(&t2);

    /* search forward */
    do {
        if (fm_valid_for_font_replacement(fm))
            return fm;
        fm = (fm_entry *) avl_t_next(&t);
    } while (fm != NULL && comp_fm_entry_ps(fm, &tmp, NULL) == 0);

    /* search backward */
    while (fm2 != NULL && comp_fm_entry_ps(fm2, &tmp, NULL) == 0) {
        if (fm_valid_for_font_replacement(fm2))
            return fm2;
        fm2 = (fm_entry *) avl_t_prev(&t2);
    }

    return NULL;
}

/**********************************************************************/
/*
 * Process map file given by its name or map line contents. Items not
 * beginning with [+-=] flush default map file, if it has not yet been
 * read. Leading blanks and blanks immediately following [+-=] are
 * ignored.
 */

static void process_map_item(char *s, int type)
{
    char *p;
    int mode;
    if (*s == ' ')
        s++;                    /* ignore leading blank */
    switch (*s) {
    case '+':                  /* +mapfile.map, +mapline */
        mode = FM_DUPIGNORE;    /* insert entry, if it is not duplicate */
        s++;
        break;
    case '=':                  /* =mapfile.map, =mapline */
        mode = FM_REPLACE;      /* try to replace earlier entry */
        s++;
        break;
    case '-':                  /* -mapfile.map, -mapline */
        mode = FM_DELETE;       /* try to delete entry */
        s++;
        break;
    default:
        mode = FM_DUPIGNORE;    /* like +, but also: */
        mitem->line = NULL;     /* flush default map file name */
    }
    if (*s == ' ')
        s++;                    /* ignore blank after [+-=] */
    p = s;                      /* map item starts here */
    switch (type) {
    case MAPFILE:              /* remove blank at end */
        while (*p != '\0' && *p != ' ')
            p++;
        *p = '\0';
        break;
    case MAPLINE:              /* blank at end allowed */
        break;
    default:
        assert(0);
    }
    if (mitem->line != NULL)    /* read default map file first */
        fm_read_info();
    if (*s != '\0') {           /* only if real item to process */
        mitem->mode = mode;
        mitem->type = type;
        mitem->line = s;
        fm_read_info();
    }
}

void pdfmapfile(integer t)
{
    process_map_item(makecstring(tokenstostring(t)), MAPFILE);
    flushstr(lasttokensstring);
}

void pdfmapline(integer t)
{
    process_map_item(makecstring(tokenstostring(t)), MAPLINE);
    flushstr(lasttokensstring);
}

void pdfinitmapfile(const_string map_name)
{
    assert(mitem == NULL);
    mitem = xtalloc(1, mapitem);
    mitem->mode = FM_DUPIGNORE;
    mitem->type = MAPFILE;
    mitem->line = xstrdup(map_name);
}

/**********************************************************************/
/*
 * Early check whether a font file exists. Search tree ff_tree is used
 * in 1st instance, as it may be faster than the kpse_find_file(), and
 * kpse_find_file() is called only once per font file name + expansion
 * parameter. This might help keeping speed, if many PDF pages with
 * same fonts are to be embedded.
 *
 * The ff_tree contains only font files, which are actually needed,
 * so this tree typically is much smaller than the tfm_tree or ps_tree.
 */

ff_entry *check_ff_exist(char *ff_name, boolean is_tt)
{
    ff_entry *ff;
    ff_entry tmp;
    void **aa;

    assert(ff_name != NULL);
    tmp.ff_name = ff_name;
    ff = (ff_entry *) avl_find(ff_tree, &tmp);
    if (ff == NULL) {           /* not yet in database */
        ff = new_ff_entry();
        ff->ff_name = xstrdup(ff_name);
        if (is_tt)
            ff->ff_path = kpse_find_file(ff_name, kpse_truetype_format, 0);
        else
            ff->ff_path = kpse_find_file(ff_name, kpse_type1_format, 0);
        aa = avl_probe(ff_tree, ff);
        assert(aa != NULL);
    }
    return ff;
}

/**********************************************************************/
/* cleaning up... */

static void destroy_fm_entry_tfm(void *pa, void *pb)
{
    fm_entry *fm;
    fm = (fm_entry *) pa;
    if (!has_pslink(fm))
        delete_fm_entry(fm);
    else
        unset_tfmlink(fm);
}

static void destroy_fm_entry_ps(void *pa, void *pb)
{
    fm_entry *fm;
    fm = (fm_entry *) pa;
    if (!has_tfmlink(fm))
        delete_fm_entry(fm);
    else
        unset_pslink(fm);
}

static void destroy_ff_entry(void *pa, void *pb)
{
    ff_entry *ff;
    ff = (ff_entry *) pa;
    delete_ff_entry(ff);
}

void fm_free(void)
{
    if (tfm_tree != NULL) {
        avl_destroy(tfm_tree, destroy_fm_entry_tfm);
        tfm_tree = NULL;
    }
    if (ps_tree != NULL) {
        avl_destroy(ps_tree, destroy_fm_entry_ps);
        ps_tree = NULL;
    }
    if (ff_tree != NULL) {
        avl_destroy(ff_tree, destroy_ff_entry);
        ff_tree = NULL;
    }
}

/**********************************************************************/
/* end of mapfile.c */
// vim: ts=4
