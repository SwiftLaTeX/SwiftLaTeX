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

#include <png.h>

/* JPG_IMAGE_INFO is main structure for interchange of image data */

#define JPG_UINT16      unsigned int
#define JPG_UINT32      unsigned long
#define JPG_UINT8       unsigned char

typedef struct {
    int color_space;            /* used color space. See JPG_ constants */
    JPG_UINT8 bits_per_component;       /* bits per component                         */
    JPG_UINT32 length;          /* length of file/data                        */
    FILE *file;                 /* jpg file                                   */
} JPG_IMAGE_INFO;

typedef struct {
    png_structp png_ptr;
    png_infop info_ptr;
} png_image_struct;

typedef struct {
    integer orig_x;
    integer orig_y;
    integer selected_page;
    integer page_box;
    void *doc;
} pdf_image_struct;

typedef struct {
    integer selected_page;
    FILE *file;
} JBIG2_IMAGE_INFO;

typedef struct {
    char *image_name;
    int image_type;
    int color_type;
    integer width;
    integer height;
    integer rotate;
    integer x_res;
    integer y_res;
    integer num_pages;
    integer colorspace_ref;
    integer group_ref;          // if it's <=0, the page has no group
    union {
        pdf_image_struct *pdf;
        png_image_struct png;
        JPG_IMAGE_INFO *jpg;
        JBIG2_IMAGE_INFO *jbig2;
    } image_struct;
} image_entry;

extern image_entry *image_ptr, *image_array;
extern integer image_max;

#define IMAGE_TYPE_NONE 0
#define IMAGE_TYPE_PDF  1
#define IMAGE_TYPE_PNG  2
#define IMAGE_TYPE_JPG  3
#define IMAGE_TYPE_TIF  4
#define IMAGE_TYPE_JBIG2 5

#define IMAGE_COLOR_B   1
#define IMAGE_COLOR_C   2
#define IMAGE_COLOR_I   4

#define img_ptr(N)      (image_array + (N))
#define img_name(N)     (img_ptr(N)->image_name)
#define img_type(N)     (img_ptr(N)->image_type)
#define img_color(N)    (img_ptr(N)->color_type)
#define img_colorspace_ref(N) (img_ptr(N)->colorspace_ref)
#define img_group_ref(N) (img_ptr(N)->group_ref)
#define img_pages(N)    (img_ptr(N)->num_pages)
#define img_width(N)    (img_ptr(N)->width)
#define img_height(N)   (img_ptr(N)->height)
#define img_rotate(N)   (img_ptr(N)->rotate)
#define img_xres(N)     (img_ptr(N)->x_res)
#define img_yres(N)     (img_ptr(N)->y_res)
#define png_ptr(N)      (img_ptr(N)->image_struct.png.png_ptr)
#define png_info(N)     (img_ptr(N)->image_struct.png.info_ptr)
#define pdf_ptr(N)      (img_ptr(N)->image_struct.pdf)
#define jpg_ptr(N)      (img_ptr(N)->image_struct.jpg)
#define jbig2_ptr(N)    (img_ptr(N)->image_struct.jbig2)
#define tif_ptr(N)      (img_ptr(N)->image_struct.tif)

extern void read_png_info(integer);
extern void write_png(integer);
extern void read_jpg_info(integer);
extern void write_jpg(integer);
extern void read_jbig2_info(integer);
extern void write_jbig2(integer);
