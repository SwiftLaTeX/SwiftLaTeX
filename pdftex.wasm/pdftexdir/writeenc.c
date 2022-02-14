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

/**********************************************************************/
/* All encoding entries go into AVL tree for fast search by name. */

struct avl_table *fe_tree = NULL;

/* AVL sort fe_entry into fe_tree by name */

static int comp_fe_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const fe_entry *) pa)->name, ((const fe_entry *) pb)->name);
}

static fe_entry *new_fe_entry(void)
{
    fe_entry *fe;
    fe = xtalloc(1, fe_entry);
    fe->name = NULL;
    fe->fe_objnum = 0;
    fe->glyph_names = NULL;     /* encoding file not yet read in */
    fe->tx_tree = NULL;
    return fe;
}

static fe_entry *lookup_fe_entry(char *s)
{
    fe_entry fe;
    assert(s != NULL);
    fe.name = s;
    if (fe_tree == NULL) {
        fe_tree = avl_create(comp_fe_entry, NULL, &avl_xallocator);
        assert(fe_tree != NULL);
    }
    return (fe_entry *) avl_find(fe_tree, &fe);
}

static void register_fe_entry(fe_entry * fe)
{
    void **aa;
    if (fe_tree == NULL) {
        fe_tree = avl_create(comp_fe_entry, NULL, &avl_xallocator);
        assert(fe_tree != NULL);
    }
    assert(fe != NULL);
    assert(fe->name != NULL);
    assert(lookup_fe_entry(fe->name) == NULL);  /* encoding not yet registered */
    aa = avl_probe(fe_tree, fe);
    assert(aa != NULL);
}

fe_entry *get_fe_entry(char *s)
{
    fe_entry *fe;
    char **gl;
    if ((fe = lookup_fe_entry(s)) == NULL && (gl = load_enc_file(s)) != NULL) {
        fe = new_fe_entry();
        fe->name = s;
        fe->glyph_names = gl;
        register_fe_entry(fe);
    }
    return fe;
}

/**********************************************************************/

void epdf_write_enc(char **glyph_names, int fe_objnum)
{
    int i, i_old;
    assert(glyph_names != NULL);
    assert(fe_objnum != 0);
    pdfbegindict(fe_objnum, 1);
    pdf_puts("/Type /Encoding\n");
    pdf_puts("/Differences [");
    for (i = 0, i_old = -2; i < 256; i++)
        if (glyph_names[i] != notdef) {
            if (i == i_old + 1) /* no gap */
                pdf_printf("/%s", glyph_names[i]);
            else {
                if (i_old == -2)
                    pdf_printf("%i/%s", i, glyph_names[i]);
                else
                    pdf_printf(" %i/%s", i, glyph_names[i]);
            }
            i_old = i;
        }
    pdf_puts("]\n");
    pdfenddict();
}

static void write_enc(char **glyph_names, struct avl_table *tx_tree, int fe_objnum)
{
    int i_old, *p;
    struct avl_traverser t;
    assert(glyph_names != NULL);
    assert(tx_tree != NULL);
    assert(fe_objnum != 0);
    pdfbegindict(fe_objnum, 1);
    pdf_puts("/Type /Encoding\n");
    pdf_puts("/Differences [");
    avl_t_init(&t, tx_tree);
    for (i_old = -2, p = (int *) avl_t_first(&t, tx_tree); p != NULL;
         p = (int *) avl_t_next(&t)) {
        if (*p == i_old + 1)    /* no gap */
            pdf_printf("/%s", glyph_names[*p]);
        else {
            if (i_old == -2)
                pdf_printf("%i/%s", *p, glyph_names[*p]);
            else
                pdf_printf(" %i/%s", *p, glyph_names[*p]);
        }
        i_old = *p;
    }
    pdf_puts("]\n");
    pdfenddict();
}

static void write_fontencoding(fe_entry * fe)
{
    assert(fe != NULL);
    write_enc(fe->glyph_names, fe->tx_tree, fe->fe_objnum);
}

void write_fontencodings(void)
{
    fe_entry *fe;
    struct avl_traverser t;
    if (fe_tree == NULL)
        return;
    avl_t_init(&t, fe_tree);
    for (fe = (fe_entry *) avl_t_first(&t, fe_tree); fe != NULL;
         fe = (fe_entry *) avl_t_next(&t))
        if (fe->fe_objnum != 0)
            write_fontencoding(fe);
}

/**********************************************************************/
/* cleaning up... */

static void destroy_fe_entry(void *pa, void *pb)
{
    fe_entry *p;
    int i;
    p = (fe_entry *) pa;
    xfree(p->name);
    if (p->glyph_names != NULL)
        for (i = 0; i < 256; i++)
            if (p->glyph_names[i] != notdef)
                xfree(p->glyph_names[i]);
    xfree(p->glyph_names);
    xfree(p);
}

void enc_free(void)
{
    if (fe_tree != NULL)
        avl_destroy(fe_tree, destroy_fe_entry);
    fe_tree = NULL;
}

/**********************************************************************/
