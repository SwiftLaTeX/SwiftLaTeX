/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009 by Jonathan Kew

 SIL Author(s): Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

/* additional declarations we want to slip in for xetex */

#define native_node_size                        6
#define native_node_text(p)                     ((unsigned short*)(&(mem[(p) + native_node_size])))

#define getnativechar(p,i)                      native_node_text(p)[i]
#define setnativechar(p,i,v)                    native_node_text(p)[i] = v

#define getnativeusv(p,i) \
  ((native_node_text(p)[i] >= 0xd800 && native_node_text(p)[i] < 0xdc00) ? \
    0x10000 + (native_node_text(p)[i] - 0xd800) * 0x400 + native_node_text(p)[(i)+1] - 0xdc00 : \
    native_node_text(p)[i])

/* p is native_word node; g is XeTeX_use_glyph_metrics flag */
#define setnativemetrics(p,g)                   measure_native_node(&(mem[p]), g)

#define setnativeglyphmetrics(p,g)              measure_native_glyph(&(mem[p]), g)

#define setjustifiednativeglyphs(p)             store_justified_native_glyphs(&(mem[p]))

#define getnativeitaliccorrection(p)            get_native_italic_correction(&(mem[p]))
#define getnativeglyphitaliccorrection(p)       get_native_glyph_italic_correction(&(mem[p]))

#define getnativeglyph(p,i)                     get_native_glyph(&(mem[p]), i)

#define makexdvglypharraydata(p)                makeXDVGlyphArrayData(&(mem[p]))
#define xdvbufferbyte(i)                        xdvbuffer[i]

#define getcpcode       get_cp_code
#define setcpcode       set_cp_code
#define getnativewordcp(p,s)                    get_native_word_cp(&(mem[p]), s)

#define pic_node_size                           9

#define deref(p)                                (*(p))

#define findpicfile(a,b,c,d)                    find_pic_file(a, b, c, d)

#define picpathbyte(p,i)                        ((unsigned char*)&(mem[p+pic_node_size]))[i]

#define dviopenout(f)                           open_dvi_output(&(f))

#define nullptr                                 (NULL)
#define glyphinfobyte(p,k)                      ((unsigned char*)p)[k]
#define casttoushort(x)                         (unsigned short)(x)

/* easier to do the bit-twiddling here than in Pascal */
/* read fields from a 32-bit math code */
#define mathfamfield(x)                         (((unsigned)(x) >> 24) & 0xFF)
#define mathclassfield(x)                       (((unsigned)(x) >> 21) & 0x07)
#define mathcharfield(x)                        ((unsigned)(x) & 0x1FFFFF)
/* calculate pieces to assign to a math code */
#define setfamilyfield(x)                       (((unsigned)(x) & 0xFF) << 24)
#define setclassfield(x)                        (((unsigned)(x) & 0x07) << 21)

/* aliases */
#define getnativemathsyparam                    get_native_mathsy_param
#define getnativemathexparam                    get_native_mathex_param
#define getotmathconstant                       get_ot_math_constant
#define getotmathvariant                        get_ot_math_variant
#define getotassemblyptr                        get_ot_assembly_ptr
#define freeotassembly                          free_ot_assembly
#define getotmathitalcorr                       get_ot_math_ital_corr
#define getotmathaccentpos                      get_ot_math_accent_pos
#define getotmathkern                           get_ot_math_kern
#define otpartcount                             ot_part_count
#define otpartglyph                             ot_part_glyph
#define otpartisextender                        ot_part_is_extender
#define otpartstartconnector                    ot_part_start_connector
#define otpartendconnector                      ot_part_end_connector
#define otpartfulladvance                       ot_part_full_advance
#define otminconnectoroverlap                   ot_min_connector_overlap
/* prototypes used in xetex.web */
#include "XeTeXOTMath.h"

/* Unicode file reading modes */
#define AUTO                                    0       /* default: will become one of 1..3 at file open time, after sniffing */
#define UTF8                                    1
#define UTF16BE                                 2
#define UTF16LE                                 3
#define RAW                                     4
#define ICUMAPPING                              5
#ifdef WIN32
#define WIN32CONSOLE                            6
#endif

/* we don't use xchr, so change the cpascal.h definition of this... */
#undef Xchr
#define Xchr(x)                                 (x)

#include "trans.h"                      /* functions for affine transform operations */
#include <teckit/TECkit_Common.h>       /* include this before XeTeX_ext.h */
#include "XeTeX_ext.h"                  /* other extension functions */

#include <math.h>
/* apparently M_PI isn't defined by <math.h> under VC++ */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
