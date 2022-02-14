/* Produced by texiweb from libavl.w. */

/* libavl - library for manipulation of binary trees.
   Copyright (C) 2004-2014 Han The Thanh, <thanh@pdftex.org>
   Modified for pdfTeX, contact pdfTeX maintainers with any problems or
   questions, not the original authors.
   
   Copyright (C) 1998-2002, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.

   Original author: Ben Pfaff.
*/

#ifndef AVL_H
#define AVL_H 1

#include <stddef.h>

/* Function types. */
typedef int avl_comparison_func (const void *avl_a, const void *avl_b,
                                 void *avl_param);
typedef void avl_item_func (void *avl_item, void *avl_param);
typedef void *avl_copy_func (void *avl_item, void *avl_param);

#ifndef LIBAVL_ALLOCATOR
#define LIBAVL_ALLOCATOR
/* Memory allocator. */
struct libavl_allocator
  {
    void *(*libavl_malloc) (struct libavl_allocator *, size_t libavl_size);
    void (*libavl_free) (struct libavl_allocator *, void *libavl_block);
  };
#endif

/* Default memory allocator. */
extern struct libavl_allocator avl_allocator_default;
void *avl_malloc (struct libavl_allocator *, size_t);
void avl_free (struct libavl_allocator *, void *);

/* Maximum AVL height. */
#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT 32
#endif

/* Tree data structure. */
struct avl_table
  {
    struct avl_node *avl_root;          /* Tree's root. */
    avl_comparison_func *avl_compare;   /* Comparison function. */
    void *avl_param;                    /* Extra argument to |avl_compare|. */
    struct libavl_allocator *avl_alloc; /* Memory allocator. */
    size_t avl_count;                   /* Number of items in tree. */
    unsigned long avl_generation;       /* Generation number. */
  };

/* An AVL tree node. */
struct avl_node
  {
    struct avl_node *avl_link[2];  /* Subtrees. */
    void *avl_data;                /* Pointer to data. */
    signed char avl_balance;       /* Balance factor. */
  };

/* AVL traverser structure. */
struct avl_traverser
  {
    struct avl_table *avl_table;        /* Tree being traversed. */
    struct avl_node *avl_node;          /* Current node in tree. */
    struct avl_node *avl_stack[AVL_MAX_HEIGHT];
                                        /* All the nodes above |avl_node|. */
    size_t avl_height;                  /* Number of nodes in |avl_parent|. */
    unsigned long avl_generation;       /* Generation number. */
  };

/* Table functions. */
struct avl_table *avl_create (avl_comparison_func *, void *,
                              struct libavl_allocator *);
struct avl_table *avl_copy (const struct avl_table *, avl_copy_func *,
                            avl_item_func *, struct libavl_allocator *);
void avl_destroy (struct avl_table *, avl_item_func *);
void **avl_probe (struct avl_table *, void *);
void *avl_insert (struct avl_table *, void *);
void *avl_replace (struct avl_table *, void *);
void *avl_delete (struct avl_table *, const void *);
void *avl_find (const struct avl_table *, const void *);
void avl_assert_insert (struct avl_table *, void *);
void *avl_assert_delete (struct avl_table *, void *);

#define avl_count(table) ((size_t) (table)->avl_count)

/* Table traverser functions. */
void avl_t_init (struct avl_traverser *, struct avl_table *);
void *avl_t_first (struct avl_traverser *, struct avl_table *);
void *avl_t_last (struct avl_traverser *, struct avl_table *);
void *avl_t_find (struct avl_traverser *, struct avl_table *, void *);
void *avl_t_insert (struct avl_traverser *, struct avl_table *, void *);
void *avl_t_copy (struct avl_traverser *, const struct avl_traverser *);
void *avl_t_next (struct avl_traverser *);
void *avl_t_prev (struct avl_traverser *);
void *avl_t_cur (struct avl_traverser *);
void *avl_t_replace (struct avl_traverser *, void *);

#endif /* avl.h */
