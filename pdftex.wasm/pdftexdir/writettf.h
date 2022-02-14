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

typedef signed char TTF_CHAR;
typedef unsigned char TTF_BYTE;
typedef signed short TTF_SHORT;
typedef unsigned short TTF_USHORT;
typedef int32_t TTF_LONG;
typedef uint32_t TTF_ULONG;
typedef uint32_t TTF_FIXED;
typedef unsigned short TTF_FUNIT;
typedef signed short TTF_FWORD;
typedef unsigned short TTF_UFWORD;
typedef unsigned short TTF_F2DOT14;

#define TTF_CHAR_SIZE    1
#define TTF_BYTE_SIZE    1
#define TTF_SHORT_SIZE   2
#define TTF_USHORT_SIZE  2
#define TTF_LONG_SIZE    4
#define TTF_ULONG_SIZE   4
#define TTF_FIXED_SIZE   4
#define TTF_FWORD_SIZE   2
#define TTF_UFWORD_SIZE  2
#define TTF_F2DOT14_SIZE 2

#define ARG_1_AND_2_ARE_WORDS       (1<<0)
#define ARGS_ARE_XY_VALUES          (1<<1)
#define ROUND_XY_TO_GRID            (1<<2)
#define WE_HAVE_A_SCALE             (1<<3)
#define RESERVED                    (1<<4)
#define MORE_COMPONENTS             (1<<5)
#define WE_HAVE_AN_X_AND_Y_SCALE    (1<<6)
#define WE_HAVE_A_TWO_BY_TWO        (1<<7)
#define WE_HAVE_INSTRUCTIONS        (1<<8)
#define USE_MY_METRICS              (1<<9)

#define get_type(t)     ((t)ttf_getnum(t##_SIZE))
#define ttf_skip(n)     ttf_getnum(n)

#define get_byte()      get_type(TTF_BYTE)
#define get_char()      get_type(TTF_CHAR)
#define get_ushort()    get_type(TTF_USHORT)
#define get_short()     get_type(TTF_SHORT)
#define get_ulong()     get_type(TTF_ULONG)
#define get_long()      get_type(TTF_LONG)
#define get_fixed()     get_type(TTF_FIXED)
#define get_funit()     get_type(TTF_FUNIT)
#define get_fword()     get_type(TTF_FWORD)
#define get_ufword()    get_type(TTF_UFWORD)
#define get_f2dot14()   get_type(TTF_F2DOT14)

#define put_num(t,n)    ((t)ttf_putnum(t##_SIZE, n))

#define put_char(n)     (void)put_num(TTF_CHAR, n)
#define put_byte(n)     (void)put_num(TTF_BYTE, n)
#define put_short(n)    put_num(TTF_SHORT, n)
#define put_ushort(n)   put_num(TTF_USHORT, n)
#define put_long(n)     put_num(TTF_LONG, n)
#define put_ulong(n)    (void)put_num(TTF_ULONG, n)
#define put_fixed(n)    (void)put_num(TTF_FIXED, n)
#define put_funit(n)    put_num(TTF_FUNIT, n)
#define put_fword(n)    put_num(TTF_FWORD, n)
#define put_ufword(n)   (void)put_num(TTF_UFWORD, n)
#define put_f2dot14(n)  put_num(TTF_F2DOT14, n)

#define copy_byte()     put_byte(get_byte())
#define copy_char()     put_char(get_char())
#define copy_ushort()   put_ushort(get_ushort())
#define copy_short()    put_short(get_short())
#define copy_ulong()    put_ulong(get_ulong())
#define copy_long()     put_long(get_long())
#define copy_fixed()    put_fixed(get_fixed())
#define copy_funit()    put_funit(get_funit())
#define copy_fword()    put_fword(get_fword())
#define copy_ufword()   put_ufword(get_ufword())
#define copy_f2dot14()  put_f2dot14(get_f2dot14())

#define is_unicode_mapping(e) \
    (e->platform_id == 0 || (e->platform_id == 3 || e->encoding_id == 1))


#define NMACGLYPHS      258
#define TABDIR_OFF      12
#define ENC_BUF_SIZE    1024

#define GLYPH_PREFIX_INDEX    "index"
#define GLYPH_PREFIX_UNICODE  "uni"

typedef struct {
    char tag[4];
    TTF_ULONG checksum;
    TTF_ULONG offset;
    TTF_ULONG length;
} dirtab_entry;

typedef struct {
    TTF_USHORT platform_id;
    TTF_USHORT encoding_id;
    TTF_ULONG offset;
    TTF_USHORT format;
} cmap_entry;

typedef struct {
    TTF_USHORT endCode;
    TTF_USHORT startCode;
    TTF_USHORT idDelta;
    TTF_USHORT idRangeOffset;
} seg_entry;
