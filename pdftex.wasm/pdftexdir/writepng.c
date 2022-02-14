/*
Copyright 1996-2017 Han The Thanh, <thanh@pdftex.org>

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
#include "image.h"

static int transparent_page_group = 0;

static void warn(png_structp png_ptr, png_const_charp msg)
{
  (void)png_ptr; (void)msg; /* Make compiler happy */
}

void read_png_info(integer img)
{
    FILE *png_file = xfopen(img_name(img), FOPEN_RBIN_MODE);

    if ((png_ptr(img) = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               NULL, NULL, warn)) == NULL)
        pdftex_fail("libpng: png_create_read_struct() failed");
    if ((png_info(img) = png_create_info_struct(png_ptr(img))) == NULL)
        pdftex_fail("libpng: png_create_info_struct() failed");
    if (setjmp(png_jmpbuf(png_ptr(img))))
        pdftex_fail("libpng: internal error");
#if PNG_LIBPNG_VER >= 10603
    /* ignore possibly incorrect CMF bytes */
    png_set_option(png_ptr(img), PNG_MAXIMUM_INFLATE_WINDOW, PNG_OPTION_ON);
#endif
    png_init_io(png_ptr(img), png_file);
    png_read_info(png_ptr(img), png_info(img));
    /* resolution support */
    img_width(img) = png_get_image_width(png_ptr(img), png_info(img));
    img_height(img) = png_get_image_height(png_ptr(img), png_info(img));
    if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_pHYs)) {
        img_xres(img) =
            round(0.0254 *
                  png_get_x_pixels_per_meter(png_ptr(img), png_info(img)));
        img_yres(img) =
            round(0.0254 *
                  png_get_y_pixels_per_meter(png_ptr(img), png_info(img)));
    }
    switch (png_get_color_type(png_ptr(img), png_info(img))) {
    case PNG_COLOR_TYPE_PALETTE:
        img_color(img) = IMAGE_COLOR_C | IMAGE_COLOR_I;
        break;
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        img_color(img) = IMAGE_COLOR_B;
        break;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        img_color(img) = IMAGE_COLOR_C;
        break;
    default:
        pdftex_fail("unsupported type of color_type <%i>",
                    png_get_color_type(png_ptr(img), png_info(img)));
    }
    if ((fixedpdfmajorversion > 1 || fixedpdfminorversion >= 4)
        && (png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_GRAY_ALPHA
            || png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_RGB_ALPHA)) {
        /* png with alpha channel in device colours; we have to add a Page
         * Group to make Adobe happy, so we have to create a dummy group object
         */
        if (transparent_page_group == 0) {
            transparent_page_group = pdfnewobjnum();
        }
        if (pdfpagegroupval == 0) {
            pdfpagegroupval = transparent_page_group;
        }
        img_group_ref(img) = pdfpagegroupval;
    }
}


#define write_gray_pixel_16(r)                           \
  if (j % 4 == 0||j % 4 == 1)  pdfbuf[pdfptr++]  = *r++; \
  else                        smask[smask_ptr++] = *r++

#define write_gray_pixel_8(r)                   \
    if (j % 2 == 0)  pdfbuf[pdfptr++]   = *r++; \
    else             smask[smask_ptr++] = *r++


#define write_rgb_pixel_16(r)                                 \
    if (!(j % 8 == 6||j % 8 == 7)) pdfbuf[pdfptr++]  = *r++;  \
    else                           smask[smask_ptr++] = *r++

#define write_rgb_pixel_8(r)                                 \
    if (j % 4 != 3)      pdfbuf[pdfptr++]  = *r++;           \
    else                 smask[smask_ptr++] = *r++

#define write_simple_pixel(r)    pdfbuf[pdfptr++] = *r++


#define write_noninterlaced(outmac)                    \
  for (i = 0; i < (int)png_get_image_height(png_ptr(img), png_info(img)); i++) {   \
    png_read_row(png_ptr(img), row, NULL);             \
    r = row;                                           \
    k = png_get_rowbytes(png_ptr(img), png_info(img)); \
    while(k > 0) {                                     \
        l = (k > pdfbufsize)? pdfbufsize : k;          \
                pdfroom(l);                            \
                for (j = 0; j < l; j++) {              \
                  outmac;                              \
                }                                      \
                k -= l;                                \
            }                                          \
        }

#define write_interlaced(outmac)                       \
  for (i = 0; i < (int)png_get_image_height(png_ptr(img), png_info(img)); i++) {   \
            row = rows[i];                             \
            k = png_get_rowbytes(png_ptr(img), png_info(img));  \
            while(k > 0) {                             \
                l = (k > pdfbufsize)? pdfbufsize : k;  \
                pdfroom(l);                            \
                for (j = 0; j < l; j++) {              \
                  outmac;                              \
                }                                      \
                k -= l;                                \
            }                                          \
            xfree(rows[i]);                            \
        }


static void write_png_palette(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer palette_objnum = 0;
    png_colorp palette;
    int num_palette;

    png_get_PLTE(png_ptr(img), png_info(img), &palette, &num_palette);

    pdfcreateobj(0, 0);
    palette_objnum = objptr;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                   num_palette -1, (int) palette_objnum);
    }
    pdfbeginstream();
    if (png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height(png_ptr(img), png_info(img))
            * png_get_rowbytes(png_ptr(img), png_info(img)) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height(png_ptr(img), png_info(img)), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height(png_ptr(img), png_info(img)); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
    if (palette_objnum > 0) {
        pdfbegindict(palette_objnum, 0);
        pdfbeginstream();
        for (i = 0; (unsigned) i < num_palette; i++) {
            pdfroom(3);
            pdfbuf[pdfptr++] = palette[i].red;
            pdfbuf[pdfptr++] = palette[i].green;
            pdfbuf[pdfptr++] = palette[i].blue;
        }
        pdfendstream();
    }
}

static void write_png_gray(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdfbeginstream();
    if (png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height(png_ptr(img), png_info(img))
            * png_get_rowbytes(png_ptr(img), png_info(img)) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height(png_ptr(img), png_info(img)), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height(png_ptr(img), png_info(img)); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
}



static void write_png_gray_alpha(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdfcreateobj(0, 0);
    smask_objnum = objptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (png_get_rowbytes(png_ptr(img), png_info(img)) / 2)
                 * png_get_image_height(png_ptr(img), png_info(img));
    smask = xtalloc(smask_size, png_byte);
    pdfbeginstream();
    if (png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        if ((png_get_bit_depth(png_ptr(img), png_info(img)) == 16) && fixedimagehicolor) {
            write_noninterlaced(write_gray_pixel_16(r));
        } else {
            write_noninterlaced(write_gray_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_get_image_height(png_ptr(img), png_info(img))
            * png_get_rowbytes(png_ptr(img), png_info(img)) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height(png_ptr(img), png_info(img)), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height(png_ptr(img), png_info(img)); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        png_read_image(png_ptr(img), rows);
        if ((png_get_bit_depth(png_ptr(img), png_info(img)) == 16) && fixedimagehicolor) {
            write_interlaced(write_gray_pixel_16(row));
        } else {
            write_interlaced(write_gray_pixel_8(row));
        }
        xfree(rows);
    }
    pdfendstream();
    pdfflush();
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = (int) png_get_bit_depth(png_ptr(img), png_info(img));
        pdfbegindict(smask_objnum, 0);
        pdf_puts("/Type /XObject\n/Subtype /Image\n");
        pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) png_get_image_width(png_ptr(img), png_info(img)),
                   (int) png_get_image_height(png_ptr(img), png_info(img)),
                   (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdfbeginstream();
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdfroom(8);
            pdfbuf[pdfptr++] = smask[i];
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdfendstream();
    }
}

static void write_png_rgb(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdfbeginstream();
    if (png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height(png_ptr(img), png_info(img))
            * png_get_rowbytes(png_ptr(img), png_info(img)) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height(png_ptr(img), png_info(img)), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height(png_ptr(img), png_info(img)); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
}

static void write_png_rgb_alpha(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdfcreateobj(0, 0);
    smask_objnum = objptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (png_get_rowbytes(png_ptr(img), png_info(img)) / 4)
                 * png_get_image_height(png_ptr(img), png_info(img));
    smask = xtalloc(smask_size, png_byte);
    pdfbeginstream();
    if (png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        if ((png_get_bit_depth(png_ptr(img), png_info(img)) == 16) && fixedimagehicolor) {
            write_noninterlaced(write_rgb_pixel_16(r));
        } else {
            write_noninterlaced(write_rgb_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_get_image_height(png_ptr(img), png_info(img))
            * png_get_rowbytes(png_ptr(img), png_info(img)) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height(png_ptr(img), png_info(img)), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height(png_ptr(img), png_info(img)); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_ptr(img), png_info(img)), png_byte);
        png_read_image(png_ptr(img), rows);
        if ((png_get_bit_depth(png_ptr(img), png_info(img)) == 16) && fixedimagehicolor) {
            write_interlaced(write_rgb_pixel_16(row));
        } else {
            write_interlaced(write_rgb_pixel_8(row));
        }
        xfree(rows);
    }
    pdfendstream();
    pdfflush();
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = (int) png_get_bit_depth(png_ptr(img), png_info(img));
        pdfbegindict(smask_objnum, 0);
        pdf_puts("/Type /XObject\n/Subtype /Image\n");
        pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) png_get_image_width(png_ptr(img), png_info(img)),
                   (int) png_get_image_height(png_ptr(img), png_info(img)),
                   (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdfbeginstream();
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdfroom(8);
            pdfbuf[pdfptr++] = smask[i];
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdfendstream();
    }
}


/**********************************************************************/
/*
 *
 * The |copy_png| function is from Hartmut Henkel. The goal is to use
 * pdf's native FlateDecode support if that is possible.
 *
 * Only a subset of the png files allows this, but when possible it
 * greatly improves inclusion speed.
 *
 */

/* Code cheerfully gleaned from Thomas Merz' PDFlib, file p_png.c "SPNG - Simple PNG" */

static int spng_getint(FILE * fp)
{
    unsigned char buf[4];
    if (fread(buf, 1, 4, fp) != 4)
        pdftex_fail("writepng: reading chunk type failed");
    return ((((((int) buf[0] << 8) + buf[1]) << 8) + buf[2]) << 8) + buf[3];
}

#define SPNG_CHUNK_IDAT 0x49444154
#define SPNG_CHUNK_IEND 0x49454E44

static void copy_png(integer img)
{
    FILE *fp = (FILE *) png_get_io_ptr(png_ptr(img));
    int i, len, type, streamlength = 0;
    boolean endflag = false;
    int idat = 0;               /* flag to check continuous IDAT chunks sequence */
    /* 1st pass to find overall stream /Length */
    if (fseek(fp, 8, SEEK_SET) != 0)
        pdftex_fail("writepng: fseek in PNG file failed");
    do {
        len = spng_getint(fp);
        type = spng_getint(fp);
        switch (type) {
        case SPNG_CHUNK_IEND:
            endflag = true;
            break;
        case SPNG_CHUNK_IDAT:
            streamlength += len;
        default:
            if (fseek(fp, len + 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
        }
    } while (endflag == false);
    pdf_printf("/Length %d\n"
               "/Filter/FlateDecode\n"
               "/DecodeParms<<"
               "/Colors %d"
               "/Columns %d"
               "/BitsPerComponent %i"
               "/Predictor 10>>\n>>\nstream\n", streamlength,
               png_get_color_type(png_ptr(img), png_info(img)) == 2 ? 3 : 1,
               (int) png_get_image_width(png_ptr(img), png_info(img)),
               (int) png_get_bit_depth(png_ptr(img), png_info(img)));
    /* 2nd pass to copy data */
    endflag = false;
    if (fseek(fp, 8, SEEK_SET) != 0)
        pdftex_fail("writepng: fseek in PNG file failed");
    do {
        len = spng_getint(fp);
        type = spng_getint(fp);
        switch (type) {
        case SPNG_CHUNK_IDAT:  /* do copy */
            if (idat == 2)
                pdftex_fail("writepng: IDAT chunk sequence broken");
            idat = 1;
            while (len > 0) {
                i = (len > pdfbufsize) ? pdfbufsize : len;
                pdfroom(i);
                fread(&pdfbuf[pdfptr], 1, i, fp);
                pdfptr += i;
                len -= i;
            }
            if (fseek(fp, 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
            break;
        case SPNG_CHUNK_IEND:  /* done */
            pdfendstream();
            endflag = true;
            break;
        default:
            if (idat == 1)
                idat = 2;
            if (fseek(fp, len + 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
        }
    } while (endflag == false);
}

static boolean last_png_needs_page_group;
static boolean transparent_page_group_was_written = false;

/* Called after the xobject generated by write_png has been finished; used to
 * write out additional objects */
static void write_additional_png_objects(void)
{
    if (last_png_needs_page_group) {
        if (!transparent_page_group_was_written && transparent_page_group > 0) {
            // create new group object
            transparent_page_group_was_written = true;
            pdfbeginobj(transparent_page_group, 2);
            if (getpdfcompresslevel() == 0) {
                pdf_puts("%PTEX Group needed for transparent pngs\n");
            }
            pdf_puts("<</Type/Group /S/Transparency /CS/DeviceRGB /I true>>\n");
            pdfendobj();
        }
    }
}

void write_png(integer img)
{

    boolean png_copy = true;
    double gamma = 0.0;
    png_fixed_point int_file_gamma = 0;
#ifndef PNG_FP_1
   /* for libpng < 1.5.0 */
#define PNG_FP_1    100000
#endif
    int i;
    integer palette_objnum = 0;
    png_colorp palette;
    int num_palette;
    last_png_needs_page_group = false;

    png_get_PLTE(png_ptr(img), png_info(img), &palette, &num_palette);

    if (fixedpdfmajorversion == 1 && fixedpdfminorversion < 5)
        fixedimagehicolor = 0;

    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    /* simple transparency support */
    if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr(img));
        png_copy = false;
    }
    /* alpha channel support */
    if (fixedpdfmajorversion == 1 && fixedpdfminorversion < 4
        && png_get_color_type(png_ptr(img), png_info(img)) | PNG_COLOR_MASK_ALPHA) {
        png_set_strip_alpha(png_ptr(img));
        png_copy = false;
    }
    /* 16 bit depth support */
    if (fixedpdfmajorversion == 1 && fixedpdfminorversion < 5)
        fixedimagehicolor = 0;
    if ((png_get_bit_depth(png_ptr(img), png_info(img)) == 16) && (fixedimagehicolor == 0)) {
        png_set_strip_16(png_ptr(img));
        png_copy = false;
    }
    /* gamma support */
    if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_gAMA)) {
        png_get_gAMA(png_ptr(img), png_info(img), &gamma);
        png_get_gAMA_fixed(png_ptr(img), png_info(img), &int_file_gamma);
    }
    if (fixedimageapplygamma) {
        if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_gAMA))
            png_set_gamma(png_ptr(img), fixedgamma / 1000.0, gamma);
        else
            png_set_gamma(png_ptr(img), fixedgamma / 1000.0,
                          1000.0 / fixedimagegamma);
        png_copy = false;
    }
    /* reset structure */
    (void) png_set_interlace_handling(png_ptr(img));
    png_read_update_info(png_ptr(img), png_info(img));

    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) png_get_image_width(png_ptr(img), png_info(img)),
               (int) png_get_image_height(png_ptr(img), png_info(img)),
               (int) png_get_bit_depth(png_ptr(img), png_info(img)));
    pdf_puts("/ColorSpace ");
    if (png_copy && (fixedpdfmajorversion > 1 || fixedpdfminorversion > 1)
        && png_get_interlace_type(png_ptr(img), png_info(img)) == PNG_INTERLACE_NONE
        && (png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_GRAY
            || png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_RGB)
        && !fixedimageapplygamma
        && (!png_get_valid(png_ptr(img), png_info(img), PNG_INFO_gAMA)
            || int_file_gamma == PNG_FP_1)
        && !png_get_valid(png_ptr(img), png_info(img),
                          PNG_INFO_cHRM | PNG_INFO_iCCP | PNG_INFO_sBIT | PNG_INFO_sRGB
                          | PNG_INFO_bKGD | PNG_INFO_hIST | PNG_INFO_tRNS | PNG_INFO_sPLT)
        ) {
        /* Copy PNG */
        if (img_colorspace_ref(img) != 0) {
            pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
        } else {
            switch (png_get_color_type(png_ptr(img), png_info(img))) {
            case PNG_COLOR_TYPE_PALETTE:
                pdfcreateobj(0, 0);
                palette_objnum = objptr;
                pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                           num_palette - 1, (int) palette_objnum);
                break;
            case PNG_COLOR_TYPE_GRAY:
                pdf_puts("/DeviceGray\n");
                break;
            default:           /* RGB */
                pdf_puts("/DeviceRGB\n");
            };
        }
        tex_printf(" (PNG copy)");
        copy_png(img);
        if (palette_objnum > 0) {
            pdfbegindict(palette_objnum, 0);
            pdfbeginstream();
            for (i = 0; i < num_palette; i++) {
                pdfroom(3);
                pdfbuf[pdfptr++] = palette[i].red;
                pdfbuf[pdfptr++] = palette[i].green;
                pdfbuf[pdfptr++] = palette[i].blue;
            }
            pdfendstream();
        }
    } else {
        if (0) {
            tex_printf(" *** PNG copy skipped because:");
            if (!png_copy)
                tex_printf(" !png_copy");
            if (fixedpdfmajorversion == 1 && fixedpdfminorversion <= 1)
                tex_printf(" minorversion=%d (and majorversion=1)",
                           (int) fixedpdfminorversion);
            if (png_get_interlace_type(png_ptr(img), png_info(img)) != PNG_INTERLACE_NONE)
                tex_printf(" interlaced");
            if (!((png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_GRAY)
                  || (png_get_color_type(png_ptr(img), png_info(img)) == PNG_COLOR_TYPE_RGB)))
                tex_printf(" colortype");
            if (fixedimageapplygamma)
                tex_printf(" apply gamma");
            if (!(!png_get_valid(png_ptr(img), png_info(img), PNG_INFO_gAMA)
                  || int_file_gamma == PNG_FP_1))
                tex_printf(" gamma");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_cHRM))
                tex_printf(" cHRM");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_iCCP))
                tex_printf(" iCCP");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_sBIT))
                tex_printf(" sBIT");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_sRGB))
                tex_printf(" sRGB");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_bKGD))
                tex_printf(" bKGD");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_hIST))
                tex_printf(" hIST");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_tRNS))
                tex_printf(" tRNS");
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_sPLT))
                tex_printf(" sPLT");
        }
        switch (png_get_color_type(png_ptr(img), png_info(img))) {
        case PNG_COLOR_TYPE_PALETTE:
            write_png_palette(img);
            break;
        case PNG_COLOR_TYPE_GRAY:
            write_png_gray(img);
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (fixedpdfminorversion >= 4 || fixedpdfmajorversion > 1) {
                write_png_gray_alpha(img);
                last_png_needs_page_group = true;
            } else
                write_png_gray(img);
            break;
        case PNG_COLOR_TYPE_RGB:
            write_png_rgb(img);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (fixedpdfminorversion >= 4 || fixedpdfmajorversion > 1) {
                write_png_rgb_alpha(img);
                last_png_needs_page_group = true;
            } else
                write_png_rgb(img);
            break;
        default:
            pdftex_fail("unsupported type of color_type <%i>",
                        png_get_color_type(png_ptr(img), png_info(img)));
        }
    }
    pdfflush();
    write_additional_png_objects();
}
