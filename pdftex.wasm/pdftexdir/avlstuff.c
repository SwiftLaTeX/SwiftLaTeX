/*
Copyright 2004-2014 Han The Thanh, <thanh@pdftex.org>

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This file is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

#include "ptexlib.h"
#include "avl.h"

static struct avl_table *PdfObjTree[pdfobjtypemax + 1] =
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/**********************************************************************/
/* memory management functions for AVL */

static void *avl_xmalloc(struct libavl_allocator *allocator, size_t size)
{
    assert(allocator != NULL && size > 0);
    return xmalloc(size);
}

static void avl_xfree(struct libavl_allocator *allocator, void *block)
{
    assert(allocator != NULL && block != NULL);
    xfree(block);
}

struct libavl_allocator avl_xallocator = {
    avl_xmalloc,
    avl_xfree
};

/**********************************************************************/
/* general AVL comparison functions */

int comp_int_entry(const void *pa, const void *pb, void *p)
{
    cmp_return(*(const int *) pa, *(const int *) pb);
    return 0;
}

int comp_string_entry(const void *pa, const void *pb, void *p)
{
    return strcmp((const char *) pa, (const char *) pb);
}

/**********************************************************************/
/* One AVL tree for each obj_type 0...pdfobjtypemax */


typedef struct oentry_ {
    integer int0;
    integer objptr;
} oentry;

/* AVL sort oentry into avl_table[] */

static int compare_info(const void *pa, const void *pb, void *param)
{
    integer a, b;
    int as, ae, bs, be, al, bl;

    a = ((const oentry *) pa)->int0;
    b = ((const oentry *) pb)->int0;
    if (a < 0 && b < 0) {       /* string comparison */
        as = strstart[-a];
        ae = strstart[-a + 1];  /* start of next string in pool */
        bs = strstart[-b];
        be = strstart[-b + 1];
        al = ae - as;
        bl = be - bs;
        if (al < bl)            /* compare first by string length */
            return -1;
        if (al > bl)
            return 1;
        for (; as < ae; as++, bs++) {
            if (strpool[as] < strpool[bs])
                return -1;
            if (strpool[as] > strpool[bs])
                return 1;
        }
    } else {                    /* integer comparison */
        if (a < b)
            return -1;
        if (a > b)
            return 1;
    }
    return 0;
}

void avlputobj(integer objptr, integer t)
{
    static void **pp;
    static oentry *oe;

    if (PdfObjTree[t] == NULL) {
        PdfObjTree[t] = avl_create(compare_info, NULL, &avl_xallocator);
        if (PdfObjTree[t] == NULL)
            pdftex_fail("avlstuff.c: avl_create() PdfObjTree failed");
    }
    oe = xtalloc(1, oentry);
    oe->int0 = objtab[objptr].int0;
    oe->objptr = objptr;        /* allows to relocate objtab */
    pp = avl_probe(PdfObjTree[t], oe);
    if (pp == NULL)
        pdftex_fail("avlstuff.c: avl_probe() out of memory in insertion");
}


/* replacement for linear search pascal function "find_obj()" */

integer avlfindobj(integer t, integer i, integer byname)
{
    static oentry *p;
    static oentry tmp;

    if (byname > 0)
        tmp.int0 = -i;
    else
        tmp.int0 = i;
    if (PdfObjTree[t] == NULL)
        return 0;
    p = (oentry *) avl_find(PdfObjTree[t], &tmp);
    if (p == NULL)
        return 0;
    return p->objptr;
}

/**********************************************************************/

struct avl_table *mf_tree = NULL;

typedef struct {
    char *tfm_name;
    internalfontnumber fontnum;
} mf_entry;

/**********************************************************************/
/* cleaning up... */

#if 0 /* unused */
static void destroy_oentry(void *pa, void *pb)
{
    oentry *p = (oentry *) pa;
    xfree(p);
}

static void PdfObjTree_free(void)
{
    int i;

    for (i = 0; i <= pdfobjtypemax; i++) {
        if (PdfObjTree[i] != NULL)
            avl_destroy(PdfObjTree[i], destroy_oentry);
    }
}
#endif

/**********************************************************************/
