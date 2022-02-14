/* writefont.c: font descriptors and writing Type 1 fonts.
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

/**********************************************************************/

struct avl_table *fo_tree = NULL;       /* tree of font dictionaries */
struct avl_table *fd_tree = NULL;       /* tree of font descriptor objects */

static int comp_fo_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const fo_entry *) pa)->fm->tfm_name,
                  ((const fo_entry *) pb)->fm->tfm_name);
}

static int comp_fd_entry(const void *pa, const void *pb, void *p)
{
    int i;
    const fd_entry *p1 = (const fd_entry *) pa, *p2 = (const fd_entry *) pb;
    assert(p1->fm != NULL && is_fontfile(p1->fm) &&
           p2->fm != NULL && is_fontfile(p2->fm));
    if ((i = strcmp(p1->fm->ff_name, p2->fm->ff_name)) != 0)
        return i;
    cmp_return(p1->fm->slant, p2->fm->slant);
    cmp_return(p1->fm->extend, p2->fm->extend);
    return 0;
}

/**********************************************************************/
/* initialize data structure for /Type /Font */

static fo_entry *new_fo_entry(void)
{
    fo_entry *fo;
    fo = xtalloc(1, fo_entry);
    fo->fo_objnum = 0;
    fo->tex_font = 0;
    fo->fm = NULL;
    fo->fd = NULL;
    fo->fe = NULL;
    fo->cw = NULL;
    fo->first_char = 1;
    fo->last_char = 0;
    fo->tounicode_objnum = 0;
    return fo;
}

/* initialize data structure for /Type /FontDescriptor */

fd_entry *new_fd_entry(void)
{
    fd_entry *fd;
    int i;
    fd = xtalloc(1, fd_entry);
    fd->fd_objnum = 0;
    fd->fontname = NULL;
    fd->subset_tag = NULL;
    fd->ff_found = false;
    fd->ff_objnum = 0;
    fd->fn_objnum = 0;
    fd->all_glyphs = false;
    fd->write_ttf_glyph_names = false;
    for (i = 0; i < FONT_KEYS_NUM; i++) {
        fd->font_dim[i].val = 0;
        fd->font_dim[i].set = false;
    }
    fd->fe = NULL;
    fd->builtin_glyph_names = NULL;
    fd->fm = NULL;
    fd->tx_tree = NULL;
    fd->gl_tree = NULL;
    return fd;
}

/* initialize data structure for /Widths array object */

static cw_entry *new_cw_entry(void)
{
    cw_entry *cw;
    cw = xtalloc(1, cw_entry);
    cw->cw_objnum = 0;
    cw->width = NULL;
    return cw;
}

/**********************************************************************/
/*
 * Only fallback values of font metrics are taken from the TFM info
 * of f by preset_fontmetrics(). During reading of the font file,
 * these values are replaced by metrics from the font, if available.
 */

static void preset_fontmetrics(fd_entry * fd, internalfontnumber f)
{
    int i;
    fd->font_dim[ITALIC_ANGLE_CODE].val =
        dividescaled(-atan(getslant(f) / 65536.0) * (180 / M_PI),
                     pdffontsize[f], 3);
    fd->font_dim[ASCENT_CODE].val =
        dividescaled(getcharheight(f, 'h'), pdffontsize[f], 3);
    fd->font_dim[CAPHEIGHT_CODE].val =
        dividescaled(getcharheight(f, 'H'), pdffontsize[f], 3);
    i = -dividescaled(getchardepth(f, 'y'), pdffontsize[f], 3);
    fd->font_dim[DESCENT_CODE].val = i < 0 ? i : 0;
    fd->font_dim[STEMV_CODE].val =
        dividescaled(getcharwidth(f, '.') / 3, pdffontsize[f], 3);
    fd->font_dim[XHEIGHT_CODE].val =
        dividescaled(getxheight(f), pdffontsize[f], 3);
    fd->font_dim[FONTBBOX1_CODE].val = 0;
    fd->font_dim[FONTBBOX2_CODE].val = fd->font_dim[DESCENT_CODE].val;
    fd->font_dim[FONTBBOX3_CODE].val =
        dividescaled(getquad(f), pdffontsize[f], 3);
    fd->font_dim[FONTBBOX4_CODE].val =
        fd->font_dim[CAPHEIGHT_CODE].val > fd->font_dim[ASCENT_CODE].val ?
        fd->font_dim[CAPHEIGHT_CODE].val : fd->font_dim[ASCENT_CODE].val;
    for (i = 0; i < INT_KEYS_NUM; i++)
        fd->font_dim[i].set = true;
}

static void fix_fontmetrics(fd_entry * fd)
{
    intparm *p = (intparm *) fd->font_dim;
    if (!p[FONTBBOX1_CODE].set || !p[FONTBBOX2_CODE].set ||
        !p[FONTBBOX3_CODE].set || !p[FONTBBOX4_CODE].set) {
        pdftex_warn("font `%s' doesn't have a BoundingBox", fd->fm->ff_name);
        return;
    }
    if (!p[ASCENT_CODE].set) {
        p[ASCENT_CODE].val = p[FONTBBOX4_CODE].val;
        p[ASCENT_CODE].set = true;
    }
    if (!p[DESCENT_CODE].set) {
        p[DESCENT_CODE].val = p[FONTBBOX2_CODE].val;
        p[DESCENT_CODE].set = true;
    }
    if (!p[CAPHEIGHT_CODE].set) {
        p[CAPHEIGHT_CODE].val = p[FONTBBOX4_CODE].val;
        p[CAPHEIGHT_CODE].set = true;
    }
}

static void write_fontmetrics(fd_entry * fd)
{
    int i;
    fix_fontmetrics(fd);
    if (fd->font_dim[FONTBBOX1_CODE].set && fd->font_dim[FONTBBOX2_CODE].set
        && fd->font_dim[FONTBBOX3_CODE].set && fd->font_dim[FONTBBOX4_CODE].set)
        pdf_printf("/%s [%i %i %i %i]\n", font_key[FONTBBOX1_CODE].pdfname,
                   (int) fd->font_dim[FONTBBOX1_CODE].val,
                   (int) fd->font_dim[FONTBBOX2_CODE].val,
                   (int) fd->font_dim[FONTBBOX3_CODE].val,
                   (int) fd->font_dim[FONTBBOX4_CODE].val);
    for (i = 0; i < GEN_KEY_NUM; i++)
        if (fd->font_dim[i].set)
            pdf_printf("/%s %i\n", font_key[i].pdfname, fd->font_dim[i].val);
}

/**********************************************************************/

static void preset_fontname(fo_entry * fo)
{
    if (fo->fm->ps_name != NULL)
        fo->fd->fontname = xstrdup(fo->fm->ps_name);    /* just fallback */
    else
        fo->fd->fontname = xstrdup(fo->fm->tfm_name);
}

static void write_fontname(fd_entry * fd, const char *key)
{
    assert(fd->fontname != NULL);
    pdf_puts("/");
    if (key != NULL)
        pdf_printf("%s /", key);
    if (fd->subset_tag != NULL)
        pdf_printf("%s+", fd->subset_tag);
    pdf_printf("%s\n", fd->fontname);
}

static void write_fontname_object(fd_entry * fd)
{
    assert(fd->fn_objnum != 0);
    pdfbeginobj(fd->fn_objnum, 1);
    write_fontname(fd, NULL);
    pdfendobj();
}

/**********************************************************************/

fd_entry *lookup_fd_entry(char *s, integer slant, integer extend)
{
    fd_entry fd;
    fm_entry fm;
    assert(s != NULL);
    fm.ff_name = s;
    fm.slant = slant;
    fm.extend = extend;
    fd.fm = &fm;
    if (fd_tree == NULL) {
        fd_tree = avl_create(comp_fd_entry, NULL, &avl_xallocator);
        assert(fd_tree != NULL);
    }
    return (fd_entry *) avl_find(fd_tree, &fd);
}

static fd_entry *lookup_fontdescriptor(fo_entry * fo)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(is_fontfile(fo->fm));
    return lookup_fd_entry(fo->fm->ff_name, fo->fm->slant, fo->fm->extend);
}

void register_fd_entry(fd_entry * fd)
{
    void **aa;
    if (fd_tree == NULL) {
        fd_tree = avl_create(comp_fd_entry, NULL, &avl_xallocator);
        assert(fd_tree != NULL);
    }
    assert(fd != NULL && fd->fm != NULL && is_fontfile(fd->fm));
    assert(lookup_fd_entry(fd->fm->ff_name, fd->fm->slant, fd->fm->extend) == NULL);    /* font descriptor not yet registered */
    aa = avl_probe(fd_tree, fd);
    assert(aa != NULL);
}

static void create_fontdescriptor(fo_entry * fo, internalfontnumber f)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fd == NULL);
    fo->fd = new_fd_entry();
    preset_fontname(fo);
    preset_fontmetrics(fo->fd, f);
    fo->fd->fe = fo->fe;        /* encoding needed by TrueType writing */
    fo->fd->fm = fo->fm;        /* map entry needed by TrueType writing */
    fo->fd->gl_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(fo->fd->gl_tree != NULL);
}

/**********************************************************************/
/*
 * For all used characters of TeX font f, get corresponding glyph names
 * from external reencoding (.enc) file and collect these in the glyph
 * tree gl_tree of font descriptor fd referenced by font dictionary fo.
 */

static void mark_reenc_glyphs(fo_entry * fo, internalfontnumber f)
{
    int i;
    char **g;
    void **aa;
    assert(fo->fe != NULL);
    if (is_subsetted(fo->fm)) {
        assert(is_included(fo->fm));
        /* mark glyphs from TeX (externally reencoded characters) */
        g = fo->fe->glyph_names;
        for (i = fo->first_char; i <= fo->last_char; i++) {
            if (pdfcharmarked(f, i) && g[i] != notdef
                && (char *) avl_find(fo->fd->gl_tree, g[i]) == NULL) {
                aa = avl_probe(fo->fd->gl_tree, xstrdup(g[i]));
                assert(aa != NULL);
            }
        }
    }
}

/*
 * Function mark_chars() has 2 uses:
 * 1. Mark characters as chars on TeX level.
 * 2. Mark encoding pairs used by TeX to optimize encoding vector.
 */

static struct avl_table *mark_chars(fo_entry * fo, struct avl_table *tx_tree,
                             internalfontnumber f)
{
    int i, *j;
    void **aa;
    if (tx_tree == NULL) {
        tx_tree = avl_create(comp_int_entry, NULL, &avl_xallocator);
        assert(tx_tree != NULL);
    }
    for (i = fo->first_char; i <= fo->last_char; i++) {
        if (pdfcharmarked(f, i) && (int *) avl_find(tx_tree, &i) == NULL) {
            j = xtalloc(1, int);
            *j = i;
            aa = avl_probe(tx_tree, j);
            assert(aa != NULL);
        }
    }
    return tx_tree;
}

/**********************************************************************/

static void get_char_range(fo_entry * fo, internalfontnumber f)
{
    int i;
    assert(fo != NULL);
    for (i = fontbc[f]; i <= fontec[f]; i++)    /* search for first_char and last_char */
        if (pdfcharmarked(f, i))
            break;
    fo->first_char = i;
    for (i = fontec[f]; i >= fontbc[f]; i--)
        if (pdfcharmarked(f, i))
            break;
    fo->last_char = i;
    if ((fo->first_char > fo->last_char)
        || !pdfcharmarked(f, fo->first_char)) { /* no character used from this font */
        fo->last_char = 0;
        fo->first_char = fo->last_char + 1;
    }
}

static void create_charwidth_array(fo_entry * fo, internalfontnumber f)
{
    int i;
    assert(fo != NULL);
    assert(fo->cw == NULL);
    fo->cw = new_cw_entry();
    fo->cw->width = xtalloc(256, integer);
    for (i = 0; i < fo->first_char; i++)
        fo->cw->width[i] = 0;
    for (i = fo->first_char; i <= fo->last_char; i++)
        fo->cw->width[i] = dividescaled(getcharwidth(f, i), pdffontsize[f], 4);
    for (i = fo->last_char + 1; i < 256; i++)
        fo->cw->width[i] = 0;
}

static void write_charwidth_array(fo_entry * fo)
{
    int i, j;
    assert(fo->cw != NULL);
    assert(fo->cw->cw_objnum == 0);
    fo->cw->cw_objnum = pdfnewobjnum();
    pdfbeginobj(fo->cw->cw_objnum, 1);
    pdf_puts("[");
    for (i = fo->first_char; i <= fo->last_char; i++) {
        pdf_printf("%i", (int) fo->cw->width[i] / 10);  /* see adv_char_width() in pdftex.web */
        if ((j = fo->cw->width[i] % 10) != 0)
            pdf_printf(".%i", j);
        if (i != fo->last_char)
            pdf_puts(" ");
    }
    pdf_puts("]\n");
    pdfendobj();
}

/**********************************************************************/
/*
 * Remark: Font objects from embedded PDF files are never registered
 * into fo_tree; they are individually written out.
 */

static fo_entry *lookup_fo_entry(char *s)
{
    fo_entry fo;
    fm_entry fm;
    assert(s != NULL);
    fm.tfm_name = s;
    fo.fm = &fm;
    if (fo_tree == NULL) {
        fo_tree = avl_create(comp_fo_entry, NULL, &avl_xallocator);
        assert(fo_tree != NULL);
    }
    return (fo_entry *) avl_find(fo_tree, &fo);
}

static void register_fo_entry(fo_entry * fo)
{
    void **aa;
    if (fo_tree == NULL) {
        fo_tree = avl_create(comp_fo_entry, NULL, &avl_xallocator);
        assert(fo_tree != NULL);
    }
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fm->tfm_name != NULL);
    assert(lookup_fo_entry(fo->fm->tfm_name) == NULL);
    aa = avl_probe(fo_tree, fo);
    assert(aa != NULL);
}

/**********************************************************************/

static void write_fontfile(fd_entry * fd)
{
    assert(is_included(fd->fm));
    if (is_type1(fd->fm))
        writet1(fd);
    else if (is_truetype(fd->fm))
        writettf(fd);
    else if (is_opentype(fd->fm))
        writeotf(fd);
    else
        assert(0);
    if (!fd->ff_found)
        return;
    assert(fd->ff_objnum == 0);
    fd->ff_objnum = pdfnewobjnum();
    pdfbegindict(fd->ff_objnum, 0);     /* font file stream */
    if (is_type1(fd->fm))
        pdf_printf("/Length1 %i\n/Length2 %i\n/Length3 %i\n",
                   (int) t1_length1, (int) t1_length2, (int) t1_length3);
    else if (is_truetype(fd->fm))
        pdf_printf("/Length1 %i\n", (int) ttf_length);
    else if (is_opentype(fd->fm))
        pdf_puts("/Subtype /Type1C\n");
    else
        assert(0);
    pdfbeginstream();
    fb_flush();
    pdfendstream();
}

/**********************************************************************/

static void write_fontdescriptor(fd_entry * fd)
{
    static const int std_flags[] = {
        /* indices for << start with 0, but bits start with 1, so the numbers 
         * for << are 1 lower than the bits in table 5.20 */
        /* *INDENT-OFF* */
        1 + 2 + (1 << 5),                       /* Courier */
        1 + 2 + (1 << 5)            + (1 << 18),/* Courier-Bold */
        1 + 2 + (1 << 5) + (1 << 6),            /* Courier-Oblique */
        1 + 2 + (1 << 5) + (1 << 6) + (1 << 18),/* Courier-BoldOblique */
                (1 << 5),                       /* Helvetica */
                (1 << 5)            + (1 << 18),/* Helvetica-Bold */
                (1 << 5) + (1 << 6),            /* Helvetica-Oblique */
                (1 << 5) + (1 << 6) + (1 << 18),/* Helvetica-BoldOblique */
              4,                                /* Symbol */
            2 + (1 << 5),                       /* Times-Roman */
            2 + (1 << 5)            + (1 << 18),/* Times-Bold */
            2 + (1 << 5) + (1 << 6),            /* Times-Italic */
            2 + (1 << 5) + (1 << 6) + (1 << 18),/* Times-BoldItalic */
              4                                 /* ZapfDingbats */
        /* *INDENT-ON* */
    };
    char *glyph;
    struct avl_traverser t;
    int fd_flags;
    assert(fd != NULL && fd->fm != NULL);

    if (is_fontfile(fd->fm))
        write_fontfile(fd);     /* this will set fd->ff_found if font file is found */
    if (fd->fn_objnum != 0)
        write_fontname_object(fd);
    if (fd->fd_objnum == 0)
        fd->fd_objnum = pdfnewobjnum();
    pdfbegindict(fd->fd_objnum, 1);
    pdf_puts("/Type /FontDescriptor\n");
    write_fontname(fd, "FontName");
    if (fd->fm->fd_flags != FD_FLAGS_NOT_SET_IN_MAPLINE)
        fd_flags = (int) fd->fm->fd_flags;
    else if (fd->ff_found)
        fd_flags = FD_FLAGS_DEFAULT_EMBED;
    else {
        fd_flags = is_std_t1font(fd->fm)
            ? std_flags[check_std_t1font(fd->fm->ps_name)]
            : FD_FLAGS_DEFAULT_NON_EMBED;
        pdftex_warn
            ("No flags specified for non-embedded font `%s' (%s) (I'm using %i): "
             "fix your map entry.",
             fd->fm->ps_name != NULL ? fd->fm->ps_name : "No name given",
             fd->fm->tfm_name, fd_flags);
    }
    pdf_printf("/Flags %i\n", fd_flags);
    write_fontmetrics(fd);
    if (fd->ff_found) {
        if (getpdfomitcharset() == 0 && is_subsetted(fd->fm) && is_type1(fd->fm)) {
            /* We don't get CharSet right. For some PDF standards,
               CharSet is optional, but if it appears, it must be
               correct. Unfortunately, there seems to be no practical
               way we can guarantee correctness with precomposed accent
               characters in our usual fonts (EC, TX, etc.):
              https://mailman.ntg.nl/pipermail/ntg-pdftex/2018-June/004251.html
              
               But for PDF/A-1, apparently it is required, regardless:
              https://mailman.ntg.nl/pipermail/ntg-pdftex/2019-January/004264.html
               
               Whereas for PDF/A-2 and PDF/A-3, it can be omitted:
              https://github.com/veraPDF/veraPDF-validation-profiles/wiki/PDFA-Parts-2-and-3-rules#rule-62114-3

               Therefore, whether it is output can be controlled by the
               user at runtime via \pdfincludecharset. */
            assert(fd->gl_tree != NULL);
            avl_t_init(&t, fd->gl_tree);
            /* Names may appear in any order. */
            pdf_puts("/CharSet (");
            for (glyph = (char *) avl_t_first(&t, fd->gl_tree); glyph != NULL;
                 glyph = (char *) avl_t_next(&t))
                pdf_printf("/%s", glyph);
            pdf_puts(")\n");
        }

        if (is_type1(fd->fm))
            pdf_printf("/FontFile %i 0 R\n", (int) fd->ff_objnum);
        else if (is_truetype(fd->fm))
            pdf_printf("/FontFile2 %i 0 R\n", (int) fd->ff_objnum);
        else if (is_opentype(fd->fm))
            pdf_printf("/FontFile3 %i 0 R\n", (int) fd->ff_objnum);
        else
            assert(0);
    }
    pdfenddict();
}

static void write_fontdescriptors(void)
{
    fd_entry *fd;
    struct avl_traverser t;
    if (fd_tree == NULL)
        return;
    avl_t_init(&t, fd_tree);
    for (fd = (fd_entry *) avl_t_first(&t, fd_tree); fd != NULL;
         fd = (fd_entry *) avl_t_next(&t))
        write_fontdescriptor(fd);
}

/**********************************************************************/

static void write_fontdictionary(fo_entry * fo)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fo_objnum != 0); /* reserved as pdf_font_num[f] in pdftex.web */
    assert(fo->tex_font != 0);

    /* write ToUnicode entry if needed */
    if ((fixedgentounicode > 0 && fo->fd != NULL 
         && !pdffontnobuiltintounicode[fo->tex_font])
        || (fo->fd != NULL && fo->fm->tfm_name != NULL 
            && strcmp(fo->fm->tfm_name, "dummy-space")) == 0) {
        if (fo->fe != NULL) {
            fo->tounicode_objnum =
                write_tounicode(fo->fe->glyph_names, fo->fm->tfm_name, fo->fe->name);
        } else if (is_type1(fo->fm)) {
            assert(fo->fd->builtin_glyph_names != NULL);
            fo->tounicode_objnum =
                write_tounicode(fo->fd->builtin_glyph_names, fo->fm->tfm_name, NULL);
        }
    }

    pdfbegindict(fo->fo_objnum, 1);
    pdf_puts("/Type /Font\n");
    pdf_puts("/Subtype /");
    if (is_type1(fo->fm))
        pdf_printf("%s\n", "Type1");
    else if (is_truetype(fo->fm))
        pdf_printf("%s\n", "TrueType");
    else if (is_opentype(fo->fm))
        pdf_printf("%s\n", "Type1");
    else
        assert(0);
    assert(fo->fd != NULL && fo->fd->fd_objnum != 0);
    write_fontname(fo->fd, "BaseFont");
    pdf_printf("/FontDescriptor %i 0 R\n", (int) fo->fd->fd_objnum);
    assert(fo->cw != NULL);
    pdf_printf("/FirstChar %i\n/LastChar %i\n/Widths %i 0 R\n",
               (int) fo->first_char, (int) fo->last_char,
               (int) fo->cw->cw_objnum);
    if ((is_type1(fo->fm) || is_opentype(fo->fm)) && fo->fe != NULL
        && fo->fe->fe_objnum != 0)
        pdf_printf("/Encoding %i 0 R\n", (int) fo->fe->fe_objnum);
    if (fo->tounicode_objnum != 0)
        pdf_printf("/ToUnicode %i 0 R\n", (int) fo->tounicode_objnum);
    if (pdffontattr[fo->tex_font] != getnullstr()) {
        pdfprint(pdffontattr[fo->tex_font]);
        pdf_puts("\n");
    }
    pdfenddict();
}

static void write_fontdictionaries(void)
{
    fo_entry *fo;
    struct avl_traverser t;
    if (fo_tree == NULL)
        return;
    avl_t_init(&t, fo_tree);
    for (fo = (fo_entry *) avl_t_first(&t, fo_tree); fo != NULL;
         fo = (fo_entry *) avl_t_next(&t))
        write_fontdictionary(fo);
}

/**********************************************************************/
/*
 * Final flush of all font related stuff by call from
 * @<Output fonts definition@>= in pdftex.web.
 */

void writefontstuff(void)
{
    write_fontdescriptors();
    write_fontencodings();      /* see writeenc.c */
    write_fontdictionaries();
}

/**********************************************************************/

static void create_fontdictionary(fm_entry * fm, integer font_objnum,
                           internalfontnumber f)
{
    fo_entry *fo = new_fo_entry();
    get_char_range(fo, f);      /* set fo->first_char and fo->last_char from f */
    assert(fo->last_char >= fo->first_char);
    fo->fm = fm;
    fo->fo_objnum = font_objnum;
    fo->tex_font = f;
    if (is_reencoded(fo->fm)) { /* at least the map entry tells so */
        fo->fe = get_fe_entry(fo->fm->encname); /* returns NULL if .enc file couldn't be opened */
        if (fo->fe != NULL && (is_type1(fo->fm) || is_opentype(fo->fm))) {
            if (fo->fe->fe_objnum == 0)
                fo->fe->fe_objnum = pdfnewobjnum();     /* then it will be written out */
            /* mark encoding pairs used by TeX to optimize encoding vector */
            fo->fe->tx_tree = mark_chars(fo, fo->fe->tx_tree, f);
        }
    }
    if (!is_builtin(fo->fm)) {
        if (is_type1(fo->fm)) {
            if ((fo->fd = lookup_fontdescriptor(fo)) == NULL) {
                create_fontdescriptor(fo, f);
                register_fd_entry(fo->fd);
            }
        } else
            create_fontdescriptor(fo, f);
        create_charwidth_array(fo, f);
        write_charwidth_array(fo);
        if (fo->fe != NULL) {
            mark_reenc_glyphs(fo, f);
            if (!is_type1(fo->fm)) {
                /* mark reencoded characters as chars on TeX level */
                assert(fo->fd->tx_tree == NULL);
                fo->fd->tx_tree = mark_chars(fo, fo->fd->tx_tree, f);
                if (is_truetype(fo->fm))
                    fo->fd->write_ttf_glyph_names = true;
            }
        } else
            /* mark non-reencoded characters as chars on TeX level */
            fo->fd->tx_tree = mark_chars(fo, fo->fd->tx_tree, f);
        if (!is_type1(fo->fm))
            write_fontdescriptor(fo->fd);
    } else {
        /* builtin fonts still need the /Widths array and /FontDescriptor
         * (to avoid error 'font FOO contains bad /BBox')
         */
        create_charwidth_array(fo, f);
        write_charwidth_array(fo);
        create_fontdescriptor(fo, f);
        write_fontdescriptor(fo->fd);
        if (!is_std_t1font(fo->fm))
            pdftex_warn("font `%s' is not a standard font; "
                        "I suppose it is available to your PDF viewer then",
                        fo->fm->ps_name);
    }
    if (is_type1(fo->fm))
        register_fo_entry(fo);
    else
        write_fontdictionary(fo);
}



/* This is called font_has_subset in luatex, but it returns 1 if any
   characters from the font are used, and 0 if not (using fontbc and
   fontec as the endpoints to check), i.e., whether any characters are
   actually used from the font. */

static int
font_is_used(internalfontnumber f)
{
    int i, s;
    /* search for |first_char| and |last_char| */
    for (i = fontbc[f]; i <= fontec[f]; i++)
        if (pdfcharmarked(f, i))
            break;
    s = i;
    for (i = fontec[f]; i >= fontbc[f]; i--)
        if (pdfcharmarked(f, i))
            break;
    if (s > i)
        return 0;
    else
        return 1;
}

void
dopdffont(integer font_objnum, internalfontnumber f)
{
    fm_entry *fm;
    if (!font_is_used(f))
        return; /* avoid failed assertion in create_fontdictionary,
                mailman.ntg.nl/pipermail/ntg-pdftex/2018-January/004209.html */
       

    fm = hasfmentry(f) ? (fm_entry *) pdffontmap[f] : NULL;
    if (fm == NULL || is_pk(fm))
        writet3(fm, font_objnum, f);
    else
        create_fontdictionary(fm, font_objnum, f);
}

// vim: ts=4
