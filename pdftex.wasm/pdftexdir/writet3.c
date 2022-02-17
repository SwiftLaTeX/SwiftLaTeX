/* writet3.c: write a Type 3 (bitmap PK) font.
Copyright 1996-2019 Han The Thanh, <thanh@pdftex.org>

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
#ifndef WEBASSEMBLY_BUILD
#include <kpathsea/tex-glyph.h>
#include <kpathsea/magstep.h>
#endif
#include <string.h>

#define T3_BUF_SIZE   1024

typedef char t3_line_entry;
define_array(t3_line);

FILE *t3_file;
static boolean t3_image_used;

static integer t3_char_procs[256];
static float t3_char_widths[256];
static int t3_glyph_num;
static float t3_font_scale;
static integer t3_b0, t3_b1, t3_b2, t3_b3;
static boolean is_pk_font;

#define t3_open()       \
    open_input(&t3_file, kpse_miscfonts_format, FOPEN_RBIN_MODE)
#define t3_close()      xfclose(t3_file, cur_file_name)
#define t3_getchar()    xgetc(t3_file)
#define t3_eof()        feof(t3_file)
#define t3_prefix(s)    str_prefix(t3_line_array, s)
#define t3_putchar(c)   pdfout(c)

#define t3_check_eof()                                     \
    if (t3_eof())                                          \
        pdftex_fail("unexpected end of file");

static void t3_getline(void)
{
    int c;
  restart:
    t3_line_ptr = t3_line_array;
    c = t3_getchar();
    while (!t3_eof()) {
        alloc_array(t3_line, 1, T3_BUF_SIZE);
        append_char_to_buf(c, t3_line_ptr, t3_line_array, t3_line_limit);
        if (c == 10)
            break;
        c = t3_getchar();
    }
    alloc_array(t3_line, 2, T3_BUF_SIZE);
    append_eol(t3_line_ptr, t3_line_array, T3_BUF_SIZE);
    if (t3_line_ptr - t3_line_array < 2 || *t3_line_array == '%') {
        if (!t3_eof())
            goto restart;
    }
}

static void t3_putline(void)
{
    char *p = t3_line_array;
    while (p < t3_line_ptr)
        t3_putchar(*p++);
}

static void update_bbox(integer llx, integer lly, integer urx, integer ury,
                        boolean is_first_glyph)
{
    if (is_first_glyph) {
        t3_b0 = llx;
        t3_b1 = lly;
        t3_b2 = urx;
        t3_b3 = ury;
    } else {
        if (llx < t3_b0)
            t3_b0 = llx;
        if (lly < t3_b1)
            t3_b1 = lly;
        if (urx > t3_b2)
            t3_b2 = urx;
        if (ury > t3_b3)
            t3_b3 = ury;
    }
}

static void t3_write_glyph(internalfontnumber f)
{
    static char t3_begin_glyph_str[] = "\\pdfglyph";
    static char t3_end_glyph_str[] = "\\endglyph";
    int glyph_index;
    int width, height, depth, llx, lly, urx, ury;
    char *p;
    t3_getline();
    if (t3_prefix(t3_begin_glyph_str)) {
        if (sscanf(t3_line_array + strlen(t3_begin_glyph_str) + 1,
                   "%i %i %i %i %i %i %i %i =", &glyph_index,
                   &width, &height, &depth, &llx, &lly, &urx, &ury) != 8) {
            remove_eol(p, t3_line_array);
            pdftex_fail("invalid glyph preamble: `%s'", t3_line_array);
        }
        if (glyph_index < fontbc[f] || glyph_index > fontec[f])
            return;
    } else
        return;
    if (!pdfcharmarked(f, glyph_index)) {
        while (!t3_prefix(t3_end_glyph_str)) {
            t3_check_eof();
            t3_getline();
        }
        return;
    }
    update_bbox(llx, lly, urx, ury, t3_glyph_num == 0);
    t3_glyph_num++;
    pdfnewdict(0, 0, 0);
    t3_char_procs[glyph_index] = objptr;
    if (width == 0)
        t3_char_widths[glyph_index] =
            (getcharwidth(f, glyph_index) / t3_font_scale) / pdffontsize[f];
    else
        t3_char_widths[glyph_index] = width;
    pdfbeginstream();
    t3_getline();
    pdf_printf("%i 0 %i %i %i %i d1\nq\n",
               (int) t3_char_widths[glyph_index], (int) llx,
               (int) lly, (int) urx, (int) ury);
    while (!t3_prefix(t3_end_glyph_str)) {
        t3_check_eof();
        if (t3_prefix("BI"))
            t3_image_used = true;
        t3_putline();
        t3_getline();
    }
    pdf_puts("Q\n");
    pdfendstream();
}

static integer get_pk_font_scale(internalfontnumber f)
{
    return
        dividescaled(pkscalefactor,
                     dividescaled(pdffontsize[f], onehundredbp,
                                  fixeddecimaldigits + 2), 0);
}

static integer pk_char_width(internalfontnumber f, scaled w)
{
    return
        dividescaled(dividescaled(w, pdffontsize[f], 7),
                     get_pk_font_scale(f), 0);
}

scaled getpkcharwidth(internalfontnumber f, scaled w)
{
    return (get_pk_font_scale(f) / 100000.0) *
        (pk_char_width(f, w) / 100.0) * pdffontsize[f];
}

#ifdef WEBASSEMBLY_BUILD
static int
magstep (int n,  int bdpi)
{
   double t;
   int step;
   int neg = 0;

   if (n < 0)
     {
       neg = 1;
       n = -n;
     }

   if (n & 1)
     {
       n &= ~1;
       t = 1.095445115;
     }
    else
      t = 1.0;

   while (n > 8)
     {
       n -= 8;
       t = t * 2.0736;
     }

   while (n > 0)
     {
       n -= 2;
       t = t * 1.2;
     }

   /* Unnecessary casts to shut up stupid compilers. */
   step = (int)(0.5 + (neg ? bdpi / t : bdpi * t));
   return step;
}
#define MAGSTEP_MAX 40
#define ABSL(expr) ((expr) < 0 ? -(expr) : (expr))
unsigned
kpse_magstep_fix ( unsigned dpi,  unsigned bdpi,  int *m_ret)
{
  int m;
  int mdpi = -1;
  unsigned real_dpi = 0;
  int sign = dpi < bdpi ? -1 : 1; /* negative or positive magsteps? */


  for (m = 0; !real_dpi && m < MAGSTEP_MAX; m++) /* don't go forever */
    {
      mdpi = magstep (m * sign, bdpi);
      if (ABSL (mdpi - (int) dpi) <= 1) /* if this magstep matches, quit */
        real_dpi = mdpi;
      else if ((mdpi - (int) dpi) * sign > 0) /* if gone too far, quit */
        real_dpi = dpi;
    }

  /* If requested, return the encoded magstep (the loop went one too far).  */
  /* More unnecessary casts. */
  if (m_ret)
    *m_ret = real_dpi == (unsigned)(mdpi ? (m - 1) * sign : 0);

  /* Always return the true dpi found.  */
  return real_dpi ? real_dpi : dpi;
}
#endif

static boolean writepk(internalfontnumber f)
{
#ifndef WEBASSEMBLY_BUILD
    kpse_glyph_file_type font_ret;
#endif
    integer llx, lly, urx, ury;
    integer cw, rw, i, j;
    halfword *row;
    char *name;
    chardesc cd;
    boolean is_null_glyph, check_preamble;
    integer dpi;
    dpi =
        kpse_magstep_fix(round
                         (fixedpkresolution *
                          (((float) pdffontsize[f]) / fontdsize[f])),
                         fixedpkresolution, NULL);
    cur_file_name = makecstring(fontname[f]);
#ifdef WEBASSEMBLY_BUILD
    name = kpse_find_pk(cur_file_name, (unsigned) dpi);\
    if (name == NULL) {
        pdftex_fail("Font %s at %i not found", cur_file_name, (int) dpi);
    }
#else
    name = kpse_find_pk(cur_file_name, (unsigned) dpi, &font_ret);
    if (name == NULL ||
        !FILESTRCASEEQ(cur_file_name, font_ret.name) ||
        !kpse_bitmap_tolerance((float) font_ret.dpi, (float) dpi)) {
        pdftex_fail("Font %s at %i not found", cur_file_name, (int) dpi);
    }
#endif
    
    t3_file = xfopen(name, FOPEN_RBIN_MODE);
    recorder_record_input(name);
    t3_image_used = true;
    is_pk_font = true;
    tex_printf(" <%s", (char *) name);
    cd.rastersize = 256;
    cd.raster = xtalloc(cd.rastersize, halfword);
    check_preamble = true;
    while (readchar(check_preamble, &cd) != 0) {
        check_preamble = false;
        if (!pdfcharmarked(f, cd.charcode))
            continue;
        t3_char_widths[cd.charcode] =
            pk_char_width(f, getcharwidth(f, cd.charcode));
        if (cd.cwidth < 1 || cd.cheight < 1) {
            cd.xescape = cd.cwidth = round(t3_char_widths[cd.charcode] / 100.0);
            cd.cheight = 1;
            cd.xoff = 0;
            cd.yoff = 0;
            is_null_glyph = true;
        } else
            is_null_glyph = false;
        llx = -cd.xoff;
        lly = cd.yoff - cd.cheight + 1;
        urx = cd.cwidth + llx + 1;
        ury = cd.cheight + lly;
        update_bbox(llx, lly, urx, ury, t3_glyph_num == 0);
        t3_glyph_num++;
        pdfnewdict(0, 0, 0);
        t3_char_procs[cd.charcode] = objptr;
        pdfbeginstream();
        pdfprintreal(t3_char_widths[cd.charcode], 2);
        pdf_printf(" 0 %i %i %i %i d1\n",
                   (int) llx, (int) lly, (int) urx, (int) ury);
        if (is_null_glyph)
            goto end_stream;
        pdf_printf("q\n%i 0 0 %i %i %i cm\nBI\n", (int) cd.cwidth,
                   (int) cd.cheight, (int) llx, (int) lly);
        pdf_printf("/W %i\n/H %i\n", (int) cd.cwidth, (int) cd.cheight);
        pdf_puts("/IM true\n/BPC 1\n/D [1 0]\nID ");
        cw = (cd.cwidth + 7) / 8;
        rw = (cd.cwidth + 15) / 16;
        row = cd.raster;
        for (i = 0; i < cd.cheight; i++) {
            for (j = 0; j < rw - 1; j++) {
                pdfout(*row / 256);
                pdfout(*row % 256);
                row++;
            }
            pdfout(*row / 256);
            if (2 * rw == cw)
                pdfout(*row % 256);
            row++;
        }
        pdf_puts("\nEI\nQ\n");
      end_stream:
        pdfendstream();
    }
    xfree(cd.raster);
    cur_file_name = NULL;
    if (name) {
        xfree(name);
    }
    return true;
}

/* We must remove duplicate glyph names since the duplicates render the
   output invalid. We give a warning, since this should only happen with
   ill-defined .enc files that should get fixed. */
static void remove_duplicate_glyph_names(char **g, const char *encname)
{
    struct avl_table *gl_tree;
    char *aa;
    int i;
    gl_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(gl_tree != NULL);
    for (i = 0; i < 256; i++) {
        if (g[i] == notdef)
            continue;
        aa = (char *) avl_find(gl_tree, g[i]);
        if (aa == NULL) {
            aa = (char *) avl_probe(gl_tree, g[i]);
            assert(aa != NULL);
        } else {
            pdftex_warn("%s: duplicate glyph name at position %d: %s",
                        encname, i, g[i]);
            xfree(g[i]);
            g[i] = (char *) notdef;
        }
    }
    avl_destroy(gl_tree, NULL);
}

void writet3(fm_entry * fm, int objnum, internalfontnumber f)
{
    static char t3_font_scale_str[] = "\\pdffontscale";
    int i;
    integer wptr, eptr, cptr;
    int first_char, last_char;
    integer pk_font_scale;
    boolean is_notdef;
    fe_entry *fe;
    char **glyph_names;
    integer tounicode_objnum;
    t3_glyph_num = 0;
    t3_image_used = false;
    for (i = 0; i < 256; i++) {
        t3_char_procs[i] = 0;
        t3_char_widths[i] = 0;
    }
    fe = fm && fm->encname ? get_fe_entry(fm->encname) : NULL;
    glyph_names = fe ? fe->glyph_names : NULL;
    if (glyph_names)
        remove_duplicate_glyph_names(glyph_names, fm->encname);
    packfilename(fontname[f], getnullstr(), maketexstring(".pgc"));
    cur_file_name = makecstring(makenamestring());
    is_pk_font = false;
    if (!t3_open()) {
        if (writepk(f))
            goto write_font_dict;
        else {
            cur_file_name = NULL;
            return;
        }
    }
    tex_printf("<%s", nameoffile + 1);
    t3_getline();
    if (!t3_prefix(t3_font_scale_str) ||
        sscanf(t3_line_array + strlen(t3_font_scale_str) + 1, "%g",
               &t3_font_scale) < 1 || t3_font_scale <= 0
        || t3_font_scale > 1000) {
        pdftex_fail("missing or invalid font scale");
    }
    while (!t3_eof())
        t3_write_glyph(f);
  write_font_dict:
    for (i = fontbc[f]; i <= fontec[f]; i++)
        if (pdfcharmarked(f, i))
            break;
    first_char = i;
    for (i = fontec[f]; i > first_char; i--)
        if (pdfcharmarked(f, i))
            break;
    last_char = i;
    /* write ToUnicode entry if we can */
    if (fixedgentounicode > 0 && !pdffontnobuiltintounicode[f] && fe != NULL)
        tounicode_objnum = write_tounicode(glyph_names, fm->tfm_name, fe->name);
    else
        tounicode_objnum = 0;
    pdfbegindict(objnum, 1);    /* Type 3 font dictionary */
    pdf_puts("/Type /Font\n/Subtype /Type3\n");
    pdf_printf("/Name /F%i\n", (int) f);
    if (pdffontattr[f] != getnullstr()) {
        pdfprint(pdffontattr[f]);
        pdf_puts("\n");
    }
    if (is_pk_font) {
        pk_font_scale = get_pk_font_scale(f);
        pdf_puts("/FontMatrix [");
        pdfprintreal(pk_font_scale, 5);
        pdf_puts(" 0 0 ");
        pdfprintreal(pk_font_scale, 5);
        pdf_puts(" 0 0]\n");
    } else
        pdf_printf("/FontMatrix [%g 0 0 %g 0 0]\n",
                   (double) t3_font_scale, (double) t3_font_scale);
    pdf_printf("/%s [ %i %i %i %i ]\n",
               font_key[FONTBBOX1_CODE].pdfname,
               (int) t3_b0, (int) t3_b1, (int) t3_b2, (int) t3_b3);
    pdf_printf("/Resources << /ProcSet [ /PDF %s] >>\n",
               t3_image_used ? "/ImageB " : "");
    pdf_printf("/FirstChar %i\n/LastChar %i\n", first_char, last_char);
    wptr = pdfnewobjnum();
    eptr = pdfnewobjnum();
    cptr = pdfnewobjnum();
    pdf_printf("/Widths %i 0 R\n/Encoding %i 0 R\n/CharProcs %i 0 R\n",
               (int) wptr, (int) eptr, (int) cptr);
    if (tounicode_objnum != 0)
        pdf_printf("/ToUnicode %i 0 R\n", (int) tounicode_objnum);
    pdfenddict();
    pdfbeginobj(wptr, 1);       /* chars width array */
    pdf_puts("[");
    if (is_pk_font)
        for (i = first_char; i <= last_char; i++) {
            pdfprintreal(t3_char_widths[i], 2);
            pdf_puts(" ");
    } else
        for (i = first_char; i <= last_char; i++)
            pdf_printf("%i ", (int) t3_char_widths[i]);
    pdf_puts("]\n");
    pdfendobj();
    pdfbegindict(eptr, 1);      /* encoding dictionary */
    pdf_printf("/Type /Encoding\n/Differences [%i", first_char);
    if (t3_char_procs[first_char] == 0) {
        pdf_printf("/%s", notdef);
        is_notdef = true;
    } else {
        if (glyph_names
            && glyph_names[first_char]
            && glyph_names[first_char] != notdef)
            pdf_printf("/%s", glyph_names[first_char]);
        else
            pdf_printf("/a%i", first_char);
        is_notdef = false;
    }
    for (i = first_char + 1; i <= last_char; i++) {
        if (t3_char_procs[i] == 0) {
            if (!is_notdef) {
                pdf_printf(" %i/%s", i, notdef);
                is_notdef = true;
            }
        } else {
            if (is_notdef) {
                pdf_printf(" %i", i);
                is_notdef = false;
            }
            if (glyph_names && glyph_names[i] && glyph_names[i] != notdef)
                pdf_printf("/%s", glyph_names[i]);
            else
                pdf_printf("/a%i", i);
        }
    }
    pdf_puts("]\n");
    pdfenddict();
    pdfbegindict(cptr, 1);      /* CharProcs dictionary */
    for (i = first_char; i <= last_char; i++)
        if (t3_char_procs[i] != 0) {
            if (glyph_names && glyph_names[i] && glyph_names[i] != notdef)
                pdf_printf("/%s %i 0 R\n", glyph_names[i],
                           (int) t3_char_procs[i]);
            else
                pdf_printf("/a%i %i 0 R\n", (int) i,
                           (int) t3_char_procs[i]);
        }
    pdfenddict();
    t3_close();
    tex_printf(">");
    cur_file_name = NULL;
}
