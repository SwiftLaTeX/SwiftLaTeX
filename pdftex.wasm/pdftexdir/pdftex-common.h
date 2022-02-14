/*
Copyright 2010-2014 Han The Thanh, <thanh@pdftex.org>

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

/* This file contains declarations used in C code (via ptexlib.h) as
   well as in pdftoepdf.cc (where we can not #include "ptexlib.h").
*/

#ifndef PDFTEX_COMMON_H
#  define PDFTEX_COMMON_H

/* Declarations generated from pdftex.web are included in ptexlib.h via pdftexd.h,
   we repeat them here for pdftoepdf.cc and as consistency check.

   pdftexd.h - generated from pdftex.web */
#ifdef __cplusplus
typedef unsigned char eightbits;
#endif

extern integer pdfboxspecmedia;
extern integer pdfboxspeccrop;
extern integer pdfboxspecbleed;
extern integer pdfboxspectrim;
extern integer pdfboxspecart;

extern longinteger pdfstreamlength;
extern integer pdfptr;
extern integer poolptr;
extern eightbits *pdfbuf;
extern integer pdfbufsize;
extern boolean pdfosmode;
extern eightbits pdflastbyte;
extern integer fixedinclusioncopyfont;
extern integer pdfpagegroupval;

/* pdftexcoerce.h - generated from pdftex.web */
extern integer pdfnewobjnum(void);
extern void pdfbeginstream(void);
extern void pdfendobj(void);
extern void pdfendstream(void);
extern void pdfflush(void);
extern void zpdfosgetosbuf(integer);
#define pdfosgetosbuf(s) zpdfosgetosbuf((integer) (s))
extern void zpdfbeginobj(integer, integer);
#define pdfbeginobj(i, pdfoslevel) zpdfbeginobj((integer) (i), (integer) (pdfoslevel))

/* epdf.c */
#ifdef __cplusplus
# define fd_entry struct fd_entry
# define fm_entry struct fm_entry
#endif
extern void embed_whole_font(fd_entry *);
extern fd_entry *epdf_create_fontdescriptor(fm_entry *, int);
extern void epdf_free(void);
extern void epdf_mark_glyphs(fd_entry *, char *);
extern int get_fd_objnum(fd_entry *);
extern int get_fn_objnum(fd_entry *);
extern int is_subsetable(fm_entry *);

/* macnames.c */
extern char notdef[];

/* mapfile.c */
extern fm_entry *lookup_fontmap(char *);
#undef fd_entry
#undef fm_entry

/* pdftoepdf.cc */
extern void epdf_check_mem(void);
extern void epdf_delete(void);
extern int read_pdf_info(char *, char *, int, int, int, int, int);

/* utils.c */
extern char *convertStringToPDFString(const char *in, int len);
extern void pdf_newline(void);
__attribute__ ((format(printf, 1, 2)))
extern void pdf_printf(const char *, ...);
extern void pdf_puts(const char *);
__attribute__ ((noreturn, format(printf, 1, 2)))
extern void pdftex_fail(const char *, ...);
__attribute__ ((format(printf, 1, 2)))
extern void pdftex_warn(const char *, ...);
extern char *stripzeros(char *a);
__attribute__ ((format(printf, 1, 2)))
extern void tex_printf(const char *, ...);
extern void write_epdf(void);

/* writeenc.c */
extern void epdf_write_enc(char **, int);

/* writeimg.c */
extern float epdf_width;
extern float epdf_height;
extern float epdf_orig_x;
extern float epdf_orig_y;
extern float epdf_rotate;
extern int epdf_selected_page;
extern int epdf_num_pages;
extern int epdf_page_box;
extern void *epdf_doc;
extern int epdf_has_page_group;

#endif /* PDFTEX_COMMON_H */
