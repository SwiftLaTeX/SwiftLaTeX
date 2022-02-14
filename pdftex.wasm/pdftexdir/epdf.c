/*
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
#include <string.h>


int is_subsetable(fm_entry * fm)
{
    assert(is_included(fm));
    return is_subsetted(fm);
}

fd_entry *epdf_create_fontdescriptor(fm_entry * fm, int stemV)
{
    fd_entry *fd;
    if ((fd = lookup_fd_entry(fm->ff_name, fm->slant, fm->extend)) == NULL) {
        fm->in_use = true;
        fd = new_fd_entry();
        fd->fm = fm;
        register_fd_entry(fd);
        fd->fd_objnum = pdfnewobjnum();
        assert(fm->ps_name != NULL);
        fd->fontname = xstrdup(fm->ps_name);    /* just fallback */
        // stemV must be copied
        fd->font_dim[STEMV_CODE].val = stemV;
        fd->font_dim[STEMV_CODE].set = true;
        fd->gl_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
        assert(fd->gl_tree != NULL);
    }
    return fd;
}

int get_fd_objnum(fd_entry * fd)
{
    assert(fd->fd_objnum != 0);
    return fd->fd_objnum;
}

int get_fn_objnum(fd_entry * fd)
{
    if (fd->fn_objnum == 0)
        fd->fn_objnum = pdfnewobjnum();
    return fd->fn_objnum;
}

/***********************************************************************
 * Mark glyphs used by embedded PDF file:
 * 1. build fontdescriptor, if not yet existing
 * 2. mark glyphs directly there
 *
 * Input charset from xpdf is a string of glyph names including
 * leading slashes, with optional generic spaces, including tabs,
 * \n, \r, and \f, preceding or between them:
   /a/b/c or /a /b /c or /a/b   /c, etc.
***********************************************************************/

#define Isgenericspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')

void epdf_mark_glyphs(fd_entry * fd, char *charset)
{
    char *p, *q, *s;
    char *glyph;
    void **aa;
    if (charset == NULL)
        return;
    assert(fd != NULL);
    while (Isgenericspace(*charset))
        charset++;
    for (s = charset + 1, q = charset + strlen(charset); s < q; s = p + 1) {
        for (p = s; *p != '\0' && *p != '/' && !Isgenericspace(*p); p++);
        if (Isgenericspace(*p)) {
            *p = '\0';
            p++;
            while (Isgenericspace(*p))
                p++;
        }
        *p = '\0';
        if ((char *) avl_find(fd->gl_tree, s) == NULL) {
            glyph = xstrdup(s);
            aa = avl_probe(fd->gl_tree, glyph);
            assert(aa != NULL);
        }
    }
}

void embed_whole_font(fd_entry * fd)
{
    fd->all_glyphs = true;
}

void epdf_free(void)
{
    epdf_check_mem();
}
