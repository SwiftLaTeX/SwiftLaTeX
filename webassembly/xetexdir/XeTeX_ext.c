/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009, 2011 by Jonathan Kew
 Copyright (c) 2012-2015 by Khaled Hosny
 Copyright (c) 2012, 2013 by Jiang Jiang

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

/* XeTeX_ext.c
 * additional plain C extensions for XeTeX - mostly platform-neutral
 */

#include <w2c/config.h>

#ifndef WEBASSEMBLY_BUILD
#ifndef POPPLER_VERSION
#include <poppler-config.h>
#endif
#include <png.h>
#include <zlib.h>
#include <graphite2/Font.h>
#endif

#ifdef _MSC_VER
#undef timezone
#endif

#include <time.h> /* For `struct tm'.  */
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined (HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

#define EXTERN extern
#include "xetexd.h"

#include "XeTeX_ext.h"

#include <teckit/TECkit_Engine.h>

#ifndef WEBASSEMBLY_BUILD
#include <kpathsea/c-ctype.h>
#include <kpathsea/line.h>
#include <kpathsea/readable.h>
#include <kpathsea/variable.h>
#include <kpathsea/absolute.h>
#if defined(WIN32)
#include <kpathsea/concatn.h>
#endif
#endif

#include <math.h> /* for fabs() */

#if defined(__STDC__)
#include <locale.h>
#endif

#include <signal.h> /* Catch interrupts.  */

#include "XeTeXLayoutInterface.h"
#include "XeTeXHTML.h"
#include "XeTeXswap.h"

#include <unicode/ubidi.h>
#include <unicode/ubrk.h>
#include <unicode/ucnv.h>

#include <assert.h>
#include <libgen.h>

struct meta_struct{
    unsigned short line;
    unsigned short column;
    unsigned char group;
    unsigned char fid;
    unsigned short cs;
};

#define MAXFILESNO 128
static int lastSeenIndex = -1;
static int lastUsedSlot = 0;
static int usedFileSlots = 0;
static char *usedFilenameStack[MAXFILESNO] = {0};
static const char* placeholderImage = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mM8f+jQJwAIAgNHZJLRewAAAABJRU5ErkJggg==";
unsigned long long getcharsigslot(int index, int line, int column, int group, int cs)
{

    /* Check available space */
    if(usedFileSlots > MAXFILESNO - 1)
    {
        return 0;
    }

    /* Need to search a slot */
    if(lastSeenIndex != index)
    {
        lastSeenIndex = index;
        char *rfn = gettexstring(fullsourcefilenamestack[index]);
        int rfnlen = strlen(rfn);
        // Sanity Check
        if(rfnlen <= 3 || rfn[rfnlen - 1] != 'x' || rfn[rfnlen - 2] != 'e' || rfn[rfnlen - 3] != 't' || rfn[0] == '/')
        {
            free(rfn);
            lastUsedSlot = 0;
            /* Early Return */
            return 0;
        }
        // Search stack
        lastUsedSlot = 0;
        for(int j = 0; j < usedFileSlots; j ++)
        {
            if(strcmp(usedFilenameStack[j], rfn) == 0)
            {
                /* We found it */
                free(rfn);
                lastUsedSlot = j + 1;
                break;
            }
        }
        // No found, allocate a new ID
        if(lastUsedSlot == 0)
        {
            usedFilenameStack[usedFileSlots] = rfn; /* Hold it */
            usedFileSlots += 1;
            lastUsedSlot = usedFileSlots;
        }
    }

    /* Cleanup, Do the best already, if we can't find, ~~we will give up */
    if(lastUsedSlot == 0) {
        return 0;
    }

    /* Fill the signature */
    unsigned long long outputB = 0;
    struct meta_struct *tmp = (struct meta_struct *)&outputB;
    tmp->line = line;
    tmp->column = column;
    tmp->group = group;
    tmp->cs = cs;
    tmp->fid = lastUsedSlot;
    // printf("%d %d\n", line, column);
    // printf("->%lld\n", outputB);
    return outputB;
}

void fillcsinfo(int n) {
    struct meta_struct *tmp = (struct meta_struct*)(&curchrsig);
    if(curchrsig == 0 || tmp->cs  == 0) return;

        int p = n;
        while(p > 0)
        {
            unsigned long long pmeta = zmem[p].hh.me;
            if(pmeta == 0) //Easy Copy
            {
                zmem[p].hh.me = curchrsig;
            }
            else
            {
                struct meta_struct* tmpinsde = (struct meta_struct*)(&pmeta);
                if(tmpinsde->cs == 0)
                {
                    
                }
                else 
                {
                    if(tmpinsde->fid != tmp->fid) //Prefer a late appear
                    {
                        zmem[p].hh.me = curchrsig;
                    }
                    else
                    {
                        if(tmp->line > tmpinsde->line  || (tmp->line == tmpinsde->line && tmp->column >= tmpinsde->column))
                        {
                            zmem[p].hh.me = curchrsig;
                        }
                    }
                }
            }
            
            p = zmem[p].hh.rh;
        }
}

static int map2UTF8(unsigned int u, char *buf, int bufSize) 
{
    if (u <= 0x0000007f) {
        if (bufSize < 1) {
            return 0;
        }
        buf[0] = (char)u;
        return 1;
    } else if (u <= 0x000007ff) {
        if (bufSize < 2) {
            return 0;
        }
        buf[0] = (char)(0xc0 + (u >> 6));
        buf[1] = (char)(0x80 + (u & 0x3f));
        return 2;
    } else if (u <= 0x0000ffff) {
        if (bufSize < 3) {
            return 0;
        }
        buf[0] = (char)(0xe0 + (u >> 12));
        buf[1] = (char)(0x80 + ((u >> 6) & 0x3f));
        buf[2] = (char)(0x80 + (u & 0x3f));
        return 3;
    } else if (u <= 0x0010ffff) {
        if (bufSize < 4) {
            return 0;
        }
        buf[0] = (char)(0xf0 + (u >> 18));
        buf[1] = (char)(0x80 + ((u >> 12) & 0x3f));
        buf[2] = (char)(0x80 + ((u >> 6) & 0x3f));
        buf[3] = (char)(0x80 + (u & 0x3f));
        return 4;
    } else {
        return 0;
    }
}

static unsigned int map2Private(unsigned int code)
{
    unsigned int private_mapping = (unsigned int)(code + 0xE000);
    if(private_mapping > 0xF8FF)
    {
        private_mapping = (unsigned int)((private_mapping - 0xF8FF) + 0xF0000);
        if(private_mapping > 0xFFFFD)
        {
            private_mapping = (unsigned int)((private_mapping - 0xFFFFD) + 0x100000);
            if(private_mapping > 0x10FFFD)
            {
                printf("Mapping overflow detected %d->%d\n", code, private_mapping);
            }
        }
    }
    //printf("Mapping %d to %d\n", code, private_mapping);
    return private_mapping;
}

static int isIllegalUnicode(unsigned int c)
{
    return (c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c == 0xAD)
            || (c == 0x061C) || (c == 0x1361)
            || (c >= 0x200B && c <= 0x200F) || (c == 0x2028) || (c == 0x2029)
            || (c >= 0x202A && c <= 0x202E) || (c >= 0x2066 && c <= 0x2069)
            || (c >= 0xD800 && c <= 0xDFFF) || (c == 0xFEFF) || (c == 0xFFFC)
            || (c == 0xFFFE) || (c == 0xFFFF);
}

static void _writeUnicode(int character) {
    char tmp[8];
    if (isIllegalUnicode(character)) {
        character = map2Private(character);
    }
    // printf("writting %d %c\n", character, character);
    int tmpLen = map2UTF8(character, tmp, 8);
    for(int i = 0; i < tmpLen; i ++) {
        fputc(tmp[i], dvifile);
    }
}

#define TOPT(A) (float)((A)/65536.0)
#define CURVPOS (curv + curvoffset)
#define CURHPOS (curh + curhoffset)
static int in_page = 0;
// static int group_count = 0;
static int svg_depth = 0;
static int btrans_depth = 0;
#define SVGBUF 1024
void htmlbegin() {
    if (nopdfoutput) return;
    fprintf(dvifile, "<doc>\n");
}

void htmlend() {
	if (nopdfoutput) return;
	for(int i = 0; i < usedFileSlots; i ++) {
		fprintf(dvifile, "<file fid='%d' url='%s'/>\n", i, usedFilenameStack[i]);
	}
	
    fprintf(dvifile, "</doc>\n");
}

void htmlbeginpage() {
    if (nopdfoutput) return;
	// printf("Page begin %d\n", totalpages);
    #ifdef WEBASSEMBLY_BUILD
    fprintf(dvifile, "<page><![CDATA[");
    #endif
    fprintf(dvifile, "<svg id='page%d' width='%.2f' height='%.2f'>\n", (int)(totalpages), TOPT(curpagewidth), TOPT(curpageheight));
    in_page = 1;
    // group_count = 0;
    svg_depth = 0;
    btrans_depth = 0;
}

void htmlendpage() {
    if (nopdfoutput) return;
	// printf("Page End\n");
    
    fprintf(dvifile, "</svg><!--page %d-->\n", (int)(totalpages));
    #ifdef WEBASSEMBLY_BUILD
    fprintf(dvifile, "]]></page>");
    #endif
    int r = 0, g = 0, b = 0;
    getBackgroundColor(&r, &g, &b);
    if(r != 255 || g!=255 || b!=255) {
        fprintf(dvifile, "<style>#page%d {background: rgb(%d,%d,%d)}</style>\n", (int)(totalpages), r,g,b);
    }
    in_page = 0;
}

void htmlhlistbegin() {
    // if (nopdfoutput) return;
    // group_count ++;
    // printf("HList Begin %d %d %d\n", curs, dviv, dvih);
    // fprintf(dvifile, "<g id='p%ld_g%d_d%ld' data-dir='horizontal'>\n", totalpages, group_count, curs);

}
void htmlhlistend() {
    // if (nopdfoutput) return;
    // printf("HList End %d %d %d\n", curs, dviv, dvih);
    // fprintf(dvifile, "</g>\n");
}
void htmlvlistbegin() {
    // if (nopdfoutput) return;
    // group_count ++;
    // printf("VList Begin %d %d %d\n", curs, dviv, dvih);
    // fprintf(dvifile, "<g id='p%ld_g%d_d%ld' data-dir='vertical'>\n", totalpages, group_count, curs);

}
void htmlvlistend() {
     if (nopdfoutput) return;
    // printf("VList End %d %d %d\n", curs, dviv, dvih);
    // fprintf(dvifile, "</g>\n");

}
void htmlnativewordbegin(int font) {
     if (nopdfoutput) return;
    int r = 0, g = 0, b = 0;
    getDocumentColor(&r, &g, &b);
    // printf("NativeWord Begin %d %.2f %.2f\n", font, dviv/65536.0, dvih/65536.0);
    fprintf(dvifile, "<text class='native ff%d' y='%.2f' x='%.2f' style='fill:rgb(%d,%d,%d);'>\n", font, TOPT(CURVPOS), TOPT(CURHPOS), r,g,b);
}

void htmlnativecharout(int character, int font, long long sig) {
     if (nopdfoutput) return;
    // printf("native asscent %d depth_base %d %c\n", heightbase[font], depthbase[font], character);
    struct meta_struct *p = (struct meta_struct *)(&(sig));
    fprintf(dvifile, "<tspan l='%d' c='%d' f='%d' t='%d'>", p->line, p->column, p->fid, p->cs);
    _writeUnicode(character);
    fprintf(dvifile, "</tspan>");
    // printf("NativeWord Out %c %d %d %d %p\n", character, p->line, p->column, p->cs, &(zmem[(pointer) + 7]));
}

void htmlnativeglyphout(int glyph, int font, int pointer) {
     if (nopdfoutput) return;
        struct meta_struct *p = (struct meta_struct *)(&(zmem[pointer].hh.me));
        fprintf(dvifile, "<tspan l='%d' c='%d' f='%d' t='%d'>", p->line, p->column, p->fid, p->cs);
        int unic = read_charglyph_map(font, glyph);
        if (unic) {
            // printf("glyph %d %d\n", glyph, unic);
            _writeUnicode(unic);
        } else {
            printf("!!!!!unicode missing for %d\n", glyph);
        }
        fprintf(dvifile, "</tspan>");
        // printf("unicode: %d glyph: %d, %c\n", unic, glyph, unic);
     // _writeUnicode(glyph);
    // printf("Native glyph Out %d %d\n", glyph, unic);
}

void htmlnativewordend() {
     if (nopdfoutput) return;
    // printf("NativeWord End %d %d\n", dviv, dvih);
    fprintf(dvifile, "\n</text>\n");
}


void htmlspacerout(int spaceLength, int pointer) {
     if (nopdfoutput) return;
    struct meta_struct *p = (struct meta_struct *)(&(zmem[pointer].hh.me));
    if (spaceLength > 0 && p->line > 0) {
        fprintf(dvifile, "<text class='spacer ff%d' y='%.2f' x='%.2f' xml:space='preserve'><tspan sl='%.2f' l='%d' c='%d' f='%d' t='%d'> </tspan></text>", 
        dvif, TOPT(CURVPOS), TOPT(CURHPOS), TOPT(spaceLength), p->line, p->column, p->fid, p->cs);
    }
}

void htmltexwordbegin() {
     // if (nopdfoutput) return;
    // printf("tex depth_base %d\n", depthbase[font]);
    // printf("NativeWord Begin %d %.2f %.2f\n", font, dviv/65536.0, dvih/65536.0);
    
    // printf("Tex Word Begin %d %d %d\n", font, dviv, dvih);
}

void htmltexwordend() {
     // if (nopdfoutput) return;
    // printf("Tex Word End %d %d\n", dviv, dvih);
}

void htmltexcharout(int character, int font, int pointer) {
     if (nopdfoutput) return;
    struct meta_struct *p = (struct meta_struct *)(&(zmem[pointer].hh.me));
    int r = 0, g = 0, b = 0;
    getDocumentColor(&r, &g, &b);
    fprintf(dvifile, "<text class='tex ff%d' y='%.2f' x='%.2f' style='fill:rgb(%d,%d,%d);'>\n", font,  TOPT(CURVPOS), TOPT(CURHPOS),
     r,g,b);
    fprintf(dvifile, "<tspan l='%d' c='%d' f='%d' t='%d' r='%d'>", p->line, p->column, p->fid, p->cs, character);
    _writeUnicode(character);
    fprintf(dvifile, "</tspan>");
    fprintf(dvifile, "</text><!--texword-->\n");
    
    // printf("Tex Char Out %c\n", character);
}

void htmlpicout(int pic_height, int pic_width, int pointer) {
     if (nopdfoutput) return;
    char *filename = ((char*)&(zmem[pointer+pic_node_size]));
    unsigned int filename_length = zmem[pointer+4].hh.b0;
    char buf[SVGBUF] = {0};
    if (filename_length > SVGBUF - 1) {
        return;
    }
    strncpy(buf, filename, filename_length);
    fprintf(dvifile, "<image class='userPic' y='%.2f' x='%.2f' width='%.2f' height='%.2f' href='%s' url='%s' preserveAspectRatio='none' />\n", 
    TOPT(CURVPOS - pic_height), TOPT(CURHPOS), TOPT(pic_width), TOPT(pic_height), placeholderImage, buf);
}

void htmlruleout(int direction, int depth, int width, int height) {
     if (nopdfoutput) return;
    int r = 0, g = 0, b = 0;
    getDocumentColor(&r, &g, &b);
    fprintf(dvifile, "<rect y='%.2f' x='%.2f' width='%.2f' height='%.2f' style='fill:rgb(%d,%d,%d);'></rect>\n", 
        TOPT(CURVPOS - height), TOPT(CURHPOS), TOPT(width), TOPT(height), r,g,b);
    // printf("Rule Out Dir:%d depth:%d, width:%d, height:%d\n", direction, depth, width, height);
}

void htmlnativefontout(int font, int font_size, const char *font_name, float extend, float slant, float embolden, int rbga) {
     if (nopdfoutput) return;
    if (in_page) return;
    
    #ifndef WEBASSEMBLY_BUILD
	    #define MAXFONTNAMELEN 512
	    char fontname_copy[MAXFONTNAMELEN] = {0};
	    strncpy(fontname_copy, font_name, MAXFONTNAMELEN - 1);
	    char *fontname_base = basename(fontname_copy);
	    char *ext = rindex(fontname_base, '.');
	    if(ext) {
	        *ext = 0;
	    }
	    int isCustomFont = (font_name[0] == '.');
	    
	    
	    if (!isCustomFont) {
	    	fprintf(dvifile, "<style>@font-face {font-family:%s; src:url(https://texlive.swiftlatex.com/%s.otf);} .ff%d {font-family: %s; font-size:%.2fpx}</style>\n", \
	    	fontname_base, fontname_base, font, fontname_base, TOPT(font_size));
	    } else {
	    	fprintf(dvifile, "<style class='userFont'>@font-face {font-family:%s; src:url(%s);} .ff%d {font-family: %s; font-size:%.2fpx}</style>\n", \
	    	fontname_base, font_name, font, fontname_base, TOPT(font_size));
	    }
	#else
	    fprintf(dvifile,  "<fontdef fonttype='native' fontid='%d' fonturl='%s' fontsize='%.2f'/>", font, font_name, TOPT(font_size));
    #endif
}

void htmltexfontout(int font, int size, int dsize) {
     if (nopdfoutput) return;
    if (in_page) return;
    char*fn = gettexstring(fontname[font]);
    #ifndef WEBASSEMBLY_BUILD
	fprintf(dvifile, "<style>@font-face {font-family:%s; src:url(https://texlive.swiftlatex.com/fonts/%s.woff);} .ff%d {font-family: %s; font-size:%.2fpx}</style>\n", \
    fn, fn, font, fn, TOPT(size));
    #else 
    fprintf(dvifile,  "<fontdef fonttype='tex' fontid='%d' fonturl='%s' fontsize='%.2f'/>", font, fn, TOPT(size));
    #endif
    free(fn);
}

void htmlsvgout(char* svg) {
     if (nopdfoutput) return;
    if(strstr(svg, "<svg>") != NULL) {
        svg_depth += 1;
    } else if (strstr(svg, "</svg>") != NULL){
        svg_depth -= 1;
    } 

    char *transformX = strstr(svg, "{?x}");
    char *transformY = strstr(svg, "{?y}");
    if(transformY != NULL && transformX != NULL) {
        transformX[0] = '%';
        transformX[1] = 'f';
        transformX[2] = ' ';
        transformX[3] = ' ';
        transformY[0] = '%';
        transformY[1] = 'f';
        transformY[2] = ' ';
        transformY[3] = ' ';
        fprintf(dvifile, svg, TOPT(CURHPOS), TOPT(CURVPOS));
        fprintf(dvifile, "\n");
    } else {
        fprintf(dvifile, "%s\n", svg);
    }
    // fprintf(dvifile, "%s\n", svg);
}

void htmlspecialout(int char_count, int str_ptr) {
     if (nopdfoutput) return;
    /* We copy specical string out of memory, it is just for better callback handling */
 //    unsigned int byteSize = (char_count + 1) * 2;
	// char *special = malloc(byteSize);
	// memset(special, 0, byteSize);
	// memcpy(special, (unsigned short*)(strpool) + str_ptr, byteSize - 2); //We allocated two more
 // 	processSpecical(byteSize, special);
 //    free(special);
    char *special = malloc(char_count + 1);
    for(int i = 0; i < char_count; i ++) {
        special[i] = ((unsigned short*)strpool + str_ptr)[i];
    }
    special[char_count] = 0;
    
    if(memcmp(special, "color ", 6) == 0) {
        processColorSpecial(special);
    } else if(memcmp(special, "background ", 11) == 0) {
        processBackgroundSpecial(special);
    } else if(memcmp(special, "pdf:image ", 10) == 0) {
        char buf[SVGBUF + 1] = {0};
        processPDFSpecial(special, TOPT(CURHPOS), TOPT(CURVPOS), buf, SVGBUF);
        // printf("special command %s\n", buf);
        fprintf(dvifile, "<image class='userPic' preserveAspectRatio='none' href='%s' %s />\n", placeholderImage, buf);
    } else if(memcmp(special, "dvisvgm:raw ", 12) == 0) {
    	// fprintf(stderr, "SwiftLaTeX does not support svg yet.\n");
        htmlsvgout(special+12);
    } else if(memcmp(special, "pdf:btrans", 10) == 0) {
        if(special[10] == ' ') { /*Hacky, because it is assuming the next one is x:scale*/
            char buf[SVGBUF + 1] = {0};
            processPDFSpecial(special, 0, 0, buf, SVGBUF);
            btrans_depth += 1;
            fprintf(dvifile, "<g transform-origin='%.2f %.2f' %s>", TOPT(CURHPOS), TOPT(CURVPOS), buf);
        }
    } else if(memcmp(special, "pdf:etrans", 10) == 0) {
        if (btrans_depth> 0) {
            btrans_depth -= 1;
            fprintf(dvifile, "</g>");
        }
    } else if(memcmp(special, "x:scale ", 8) == 0) { /*hacky*/
        fprintf(dvifile, "<g transform-origin='%.2f %.2f' transform='scale(%s)'>", TOPT(CURHPOS), TOPT(CURVPOS), special + 8);
        btrans_depth += 1;
    } else {
        printf("Unhandled special command %s\n", special);
    }
    free(special);
}

/* for reading input files, we don't need the default locking routines
   as xetex is a single-threaded program */
#ifdef WIN32
#ifdef __MINGW32__
/* MinGW (both 32- and 64-bit) has problems with _getc_nolock() and/or _ungetc_nolock() */
#define GETC(f)      getc(f)
#define UNGETC(c,f)  ungetc(c,f)
#else
#define GETC(f)      _getc_nolock(f)
#define UNGETC(c,f)  _ungetc_nolock(c,f)
#endif
#else
#define GETC(f)      getc_unlocked(f)
#define UNGETC(c,f)  ungetc(c,f)
#endif

/* tables/values used in UTF-8 interpretation -
   code is based on ConvertUTF.[ch] sample code
   published by the Unicode consortium */
const uint32_t
offsetsFromUTF8[6] =    {
    0x00000000UL,
    0x00003080UL,
    0x000E2080UL,
    0x03C82080UL,
    0xFA082080UL,
    0x82082080UL
};

const uint8_t
bytesFromUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

const uint8_t
firstByteMark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

const int halfShift                 = 10;
const uint32_t halfBase             = 0x0010000UL;
const uint32_t halfMask             = 0x3FFUL;
const uint32_t kSurrogateHighStart  = 0xD800UL;
const uint32_t kSurrogateHighEnd        = 0xDBFFUL;
const uint32_t kSurrogateLowStart       = 0xDC00UL;
const uint32_t kSurrogateLowEnd     = 0xDFFFUL;
const uint32_t byteMask             = 0x000000BFUL;
const uint32_t byteMark             = 0x00000080UL;


/* if the user specifies a paper size or output driver program */
const char *papersize;
const char *outputdriver = "xdvipdfmx -q -E"; /* default to portable xdvipdfmx driver */


void initversionstring(char **versions)
{
#ifdef WEBASSEMBLY_BUILD
    int len = 64; 
    *versions = (char *) xmalloc(len + 1);
    sprintf(*versions, "Javascript TeX v1");
#else

#ifndef XETEX_MAC
    int fc_version = FcGetVersion();
#endif
    FT_Int ftMajor, ftMinor, ftPatch;
    int grMajor, grMinor, grBugfix;
    UVersionInfo icuVersion;
    char icu_version[U_MAX_VERSION_STRING_LENGTH] = "";

    const_string fmt =
        "Compiled with ICU version %s; using %s\n"
        "Compiled with zlib version %s; using %s\n"
        "Compiled with FreeType2 version %d.%d.%d; using %d.%d.%d\n"
        "Compiled with Graphite2 version %d.%d.%d; using %d.%d.%d\n"
        "Compiled with HarfBuzz version %s; using %s\n"
        "Compiled with libpng version %s; using %s\n"
        "Compiled with poppler version %s\n"
#ifdef XETEX_MAC
        "Using Mac OS X Core Text and Cocoa frameworks\n"
#else
        "Compiled with fontconfig version %d.%d.%d; using %d.%d.%d\n"
#endif
        ;

    int len = strlen(fmt)
            + strlen(U_ICU_VERSION)
            + strlen(icu_version)
            + strlen(ZLIB_VERSION)
            + strlen(zlib_version)
            + strlen(HB_VERSION_STRING)
            + strlen(hb_version_string())
            + strlen(PNG_LIBPNG_VER_STRING)
            + strlen(png_libpng_ver)
            + strlen(POPPLER_VERSION)
#ifndef XETEX_MAC
            + 6 * 3 /* for fontconfig version #s (won't really need 3 digits per field!) */
#endif
            + 6 * 3 /* for graphite2 version #s (ditto) */
            + 6 * 3; /* for freetype version #s (ditto) */

    *versions = (char *) xmalloc(len + 1);
        /* len will be more than enough, because of the placeholder chars in fmt
            that get replaced by the arguments */

    u_getVersion(icuVersion);
    u_versionToString(icuVersion, icu_version);

    if (gFreeTypeLibrary == 0 && FT_Init_FreeType(&gFreeTypeLibrary) != 0) {
        fprintf(stderr, "FreeType initialization failed!\n");
        exit(9);
    }
    FT_Library_Version(gFreeTypeLibrary, &ftMajor, &ftMinor, &ftPatch);

    gr_engine_version(&grMajor, &grMinor, &grBugfix);

    (void)sprintf(*versions, fmt,
        U_ICU_VERSION, icu_version,
        ZLIB_VERSION, zlib_version,
        FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH,
        ftMajor, ftMinor, ftPatch,
        GR2_VERSION_MAJOR, GR2_VERSION_MINOR, GR2_VERSION_BUGFIX,
        grMajor, grMinor, grBugfix,
        HB_VERSION_STRING, hb_version_string(),
        PNG_LIBPNG_VER_STRING, png_libpng_ver, POPPLER_VERSION
#ifndef XETEX_MAC
        ,
        FC_VERSION / 10000, (FC_VERSION % 10000) / 100, FC_VERSION % 100,
        fc_version / 10000, (fc_version % 10000) / 100, fc_version % 100
#endif
        );
#endif
}


void
setinputfileencoding(UFILE* f, integer mode, integer encodingData)
{

    if ((f->encodingMode == ICUMAPPING) && (f->conversionData != NULL)) {
#ifndef WEBASSEMBLY_BUILD
        ucnv_close((UConverter*)(f->conversionData));
#endif
    }
    f->conversionData = 0;

    switch (mode) {
        case UTF8:
        case UTF16BE:
        case UTF16LE:
        case RAW:
            f->encodingMode = mode;
            break;
#ifndef WEBASSEMBLY_BUILD
        case ICUMAPPING:
            {
                char* name = gettexstring(encodingData);
                UErrorCode err = U_ZERO_ERROR;
                UConverter* cnv = ucnv_open(name, &err);
                if (cnv == NULL) {
                    begindiagnostic();
                    printnl('E');
                    printcstring("rror ");
                    printint(err);
                    printcstring(" creating Unicode converter for `");
                    printcstring(name);
                    printcstring("'; reading as raw bytes");
                    enddiagnostic(1);
                    f->encodingMode = RAW;
                } else {
                    f->encodingMode = ICUMAPPING;
                    f->conversionData = cnv;
                }
                free(name);
            }
            break;
#else
        default:
            f->encodingMode = mode;
            break;
#endif
    }
}

static void
buffer_overflow(void)
{
    fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",
                             (unsigned) bufsize);
    fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
    uexit (3);
}

static void
conversion_error(int errcode)
{
    begindiagnostic();
    printnl('U');
    printcstring("nicode conversion failed (ICU error code = ");
    printint(errcode);
    printcstring(") discarding any remaining text");
    enddiagnostic(1);
}

#ifdef WORDS_BIGENDIAN
#define NATIVE_UTF32    kForm_UTF32BE
#else
#define NATIVE_UTF32    kForm_UTF32LE
#endif

static void
apply_normalization(uint32_t* buf, int len, int norm)
{
    static TECkit_Converter normalizers[2] = { NULL, NULL };

    TECkit_Status status;
    UInt32 inUsed, outUsed;
    TECkit_Converter *normPtr = &normalizers[norm - 1];
    if (*normPtr == NULL) {
        status = TECkit_CreateConverter(NULL, 0, 1,
            NATIVE_UTF32, NATIVE_UTF32 | (norm == 1 ? kForm_NFC : kForm_NFD),
            &*normPtr);
        if (status != kStatus_NoError) {
            fprintf(stderr, "! Failed to create normalizer: error code = %d\n", (int)status);
            uexit (3);
        }
    }

    status = TECkit_ConvertBuffer(*normPtr, (Byte*)buf, len * sizeof(UInt32), &inUsed,
                (Byte*)&buffer[first], sizeof(*buffer) * (bufsize - first), &outUsed, 1);
    if (status != kStatus_NoError)
        buffer_overflow();
    last = first + outUsed / sizeof(*buffer);
}

#ifdef WORDS_BIGENDIAN
#define UCNV_UTF32_NativeEndian UCNV_UTF32_BigEndian
#else
#define UCNV_UTF32_NativeEndian UCNV_UTF32_LittleEndian
#endif

int
input_line(UFILE* f)
{
static char* byteBuffer = NULL;
static uint32_t *utf32Buf = NULL;
    int i, tmpLen;
    int norm = getinputnormalizationstate();
#ifdef WIN32
    const int fd = fileno(f->f);
    if (fd == _fileno(stdin) && _isatty(fd)) {
        f->encodingMode = WIN32CONSOLE;
    }
#endif

    last = first;

    if (f->encodingMode == ICUMAPPING) {
#ifndef WEBASSEMBLY_BUILD
        uint32_t bytesRead = 0;
        UConverter* cnv;
        int outLen;
        UErrorCode errorCode = U_ZERO_ERROR;

        if (byteBuffer == NULL)
            byteBuffer = (char*) xmalloc(bufsize + 1);

        /* Recognize either LF or CR as a line terminator; skip initial LF if prev line ended with CR.  */
        i = GETC(f->f);
        if (f->skipNextLF) {
            f->skipNextLF = 0;
            if (i == '\n')
                i = GETC(f->f);
        }

        if (i != EOF && i != '\n' && i != '\r')
            byteBuffer[bytesRead++] = i;
        if (i != EOF && i != '\n' && i != '\r')
            while (bytesRead < bufsize && (i = GETC(f->f)) != EOF && i != '\n' && i != '\r')
                byteBuffer[bytesRead++] = i;

        if (i == EOF && errno != EINTR && bytesRead == 0)
            return false;

        if (i != EOF && i != '\n' && i != '\r')
            buffer_overflow();

        /* now apply the mapping to turn external bytes into Unicode characters in buffer */
        cnv = (UConverter*)(f->conversionData);
        switch (norm) {
            case 1: // NFC
            case 2: // NFD
                if (utf32Buf == NULL)
                    utf32Buf = (uint32_t*) xcalloc(bufsize, sizeof(uint32_t));
                tmpLen = ucnv_toAlgorithmic(UCNV_UTF32_NativeEndian, cnv,
                                            (char*)utf32Buf, bufsize * sizeof(*utf32Buf),
                                            byteBuffer, bytesRead, &errorCode);
                if (errorCode != 0) {
                    conversion_error((int)errorCode);
                    return false;
                }
                apply_normalization(utf32Buf, tmpLen / sizeof(*utf32Buf), norm); // sets 'last' correctly
                break;

            default: // none
                outLen = ucnv_toAlgorithmic(UCNV_UTF32_NativeEndian, cnv,
                                            (char*)&buffer[first], sizeof(*buffer) * (bufsize - first),
                                            byteBuffer, bytesRead, &errorCode);
                if (errorCode != 0) {
                    conversion_error((int)errorCode);
                    return false;
                }
                outLen /= sizeof(*buffer);
                last = first + outLen;
                break;
        }
#endif
    } else {
        /* Recognize either LF or CR as a line terminator; skip initial LF if prev line ended with CR.  */
        i = get_uni_c(f);
        if (f->skipNextLF) {
            f->skipNextLF = 0;
            if (i == '\n')
                i = get_uni_c(f);
        }

        switch (norm) {
            case 1: // NFC
            case 2: // NFD
                // read Unicode chars into utf32Buf as UTF32
                if (utf32Buf == NULL)
                    utf32Buf = (uint32_t*) xcalloc(bufsize, sizeof(uint32_t));
                tmpLen = 0;
                if (i != EOF && i != '\n' && i != '\r')
                    utf32Buf[tmpLen++] = i;
                if (i != EOF && i != '\n' && i != '\r')
                    while (tmpLen < bufsize && (i = get_uni_c(f)) != EOF && i != '\n' && i != '\r')
                        utf32Buf[tmpLen++] = i;

                if (i == EOF && errno != EINTR && tmpLen == 0)
                    return false;

                /* We didn't get the whole line because our buffer was too small.  */
                if (i != EOF && i != '\n' && i != '\r')
                    buffer_overflow();
                apply_normalization(utf32Buf, tmpLen, norm);
                break;

            default: // none
#ifdef WIN32
                if (f->encodingMode == WIN32CONSOLE && i == 0x1a) /* Ctrl+Z */
                    return false;
#endif
                if (last < bufsize && i != EOF && i != '\n' && i != '\r')
                    buffer[last++] = i;
                if (i != EOF && i != '\n' && i != '\r')
                    while (last < bufsize && (i = get_uni_c(f)) != EOF && i != '\n' && i != '\r')
                        buffer[last++] = i;

                if (i == EOF && errno != EINTR && last == first)
                    return false;

                /* We didn't get the whole line because our buffer was too small.  */
                if (i != EOF && i != '\n' && i != '\r')
                    buffer_overflow();
                break;
        }
    }

    /* If line ended with CR, remember to skip following LF. */
    if (i == '\r')
        f->skipNextLF = 1;

    buffer[last] = ' ';
    if (last >= maxbufstack)
        maxbufstack = last;

    /* Trim trailing space or EOL characters. */
#define IS_SPC_OR_EOL(c) ((c) == ' ' || (c) == '\r' || (c) == '\n')
    while (last > first && IS_SPC_OR_EOL(buffer[last - 1]))
        --last;

    return true;
}

static void die(const_string s, int i)
{
    fprintf(stderr, s, i);
    fprintf(stderr, " - exiting\n");
    exit(3);
}


__attribute__((noreturn)) 
static void dieWithReason(const_string reason)
{
    printf("\n\n! LaTeX Error: %s\n", reason);
    printf("See the LaTeX manual or LaTeX Companion for explanation.\nType  H <return>  for immediate help.\n  ...\n\n");
    printf("l.1 \\begin{swiftlatex}\n\n");
    abort();
}


static UBreakIterator* brkIter = NULL;
static int brkLocaleStrNum = 0;

void
linebreakstart(int f, integer localeStrNum, uint16_t* text, integer textLength)
{
#ifndef WEBASSEMBLY_BUILD
    UErrorCode status = U_ZERO_ERROR;
    char* locale = (char*)gettexstring(localeStrNum);

    if (fontarea[f] == OTGR_FONT_FLAG && strcmp(locale, "G") == 0) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine) fontlayoutengine[f];
        if (initGraphiteBreaking(engine, text, textLength))
            /* user asked for Graphite line breaking and the font supports it */
            return;
    }

    if ((localeStrNum != brkLocaleStrNum) && (brkIter != NULL)) {
        ubrk_close(brkIter);
        brkIter = NULL;
    }

    if (brkIter == NULL) {
        brkIter = ubrk_open(UBRK_LINE, locale, NULL, 0, &status);
        if (U_FAILURE(status)) {
            begindiagnostic();
            printnl('E');
            printcstring("rror ");
            printint(status);
            printcstring(" creating linebreak iterator for locale `");
            printcstring(locale);
            printcstring("'; trying default locale `en_us'.");
            enddiagnostic(1);
            if (brkIter != NULL)
                ubrk_close(brkIter);
            status = U_ZERO_ERROR;
            brkIter = ubrk_open(UBRK_LINE, "en_us", NULL, 0, &status);
        }
        free(locale);
        brkLocaleStrNum = localeStrNum;
    }

    if (brkIter == NULL) {
        die("! failed to create linebreak iterator, status=%d", (int)status);
    }

    ubrk_setText(brkIter, (UChar*) text, textLength, &status);
#else
    dieWithReason("WEBASSEMBLY_BUILD does not support ICU linebreaking yet");
   
#endif
}

int
linebreaknext(void)
{
#ifndef WEBASSEMBLY_BUILD
    if (brkIter != NULL)
        return ubrk_next((UBreakIterator*)brkIter);
    else
        return findNextGraphiteBreak();
#else
    dieWithReason("WEBASSEMBLY_BUILD does not support ICU linebreaking yet");
#endif
}

int
getencodingmodeandinfo(integer* info)
{
    /* \XeTeXinputencoding "enc-name"
     *   -> name is packed in |nameoffile| as a C string, starting at [1]
     * Check if it's a built-in name; if not, try to open an ICU converter by that name
     */
    UErrorCode err = U_ZERO_ERROR;
    UConverter* cnv;
    char* name = (char*)nameoffile + 1;
    *info = 0;
    if (strcasecmp(name, "auto") == 0) {
        return AUTO;
    }
    if (strcasecmp(name, "utf8") == 0) {
        return UTF8;
    }
    if (strcasecmp(name, "utf16") == 0) {   /* depends on host platform */
#ifdef WORDS_BIGENDIAN
        return UTF16BE;
#else
        return UTF16LE;
#endif
    }
    if (strcasecmp(name, "utf16be") == 0) {
        return UTF16BE;
    }
    if (strcasecmp(name, "utf16le") == 0) {
        return UTF16LE;
    }
    if (strcasecmp(name, "bytes") == 0) {
        return RAW;
    }
#ifndef WEBASSEMBLY_BUILD
    /* try for an ICU converter */
    cnv = ucnv_open(name, &err);
    if (cnv == NULL) {
        begindiagnostic();
        printnl('U'); /* ensure message starts on a new line */
        printcstring("nknown encoding `");
        printcstring(name);
        printcstring("'; reading as raw bytes");
        enddiagnostic(1);
        return RAW;
    } else {
        ucnv_close(cnv);
        *info = maketexstring(name);
        return ICUMAPPING;
    }
#else
    printf("Unknown encoding \n");
    return RAW;
#endif
}

void
printutf8str(const unsigned char* str, int len)
{
    while (len-- > 0)
        printrawchar(*(str++), true); /* bypass utf-8 encoding done in print_char() */
}

void
printchars(const unsigned short* str, int len)
{
    while (len-- > 0)
        printchar(*(str++));
}

#ifdef WORDS_BIGENDIAN
#define UTF16_NATIVE kForm_UTF16BE
#else
#define UTF16_NATIVE kForm_UTF16LE
#endif

static void*
load_mapping_file(const char* s, const char* e, char byteMapping)
{
    char* mapPath;
    TECkit_Converter cnv = 0;
    char* buffer = (char*) xmalloc(e - s + 5);
    strncpy(buffer, s, e - s);
    buffer[e - s] = 0;
    strcat(buffer, ".tec");
    mapPath = kpse_find_file(buffer, kpse_miscfonts_format, 1);

    if (mapPath) {
        FILE* mapFile = fopen(mapPath, FOPEN_RBIN_MODE);
        free(mapPath);
        if (mapFile) {
            uint32_t mappingSize;
            Byte* mapping;
            /* TECkit_Status status; */
            fseek(mapFile, 0, SEEK_END);
            mappingSize = ftell(mapFile);
            fseek(mapFile, 0, SEEK_SET);
            mapping = (Byte*) xmalloc(mappingSize);
            fread(mapping, 1, mappingSize, mapFile);
            fclose(mapFile);
            if (byteMapping != 0)
                /* status = */ TECkit_CreateConverter(mapping, mappingSize,
                                            false,
                                            UTF16_NATIVE, kForm_Bytes,
                                            &cnv);
            else
                /* status = */ TECkit_CreateConverter(mapping, mappingSize,
                                            true,
                                            UTF16_NATIVE, UTF16_NATIVE,
                                            &cnv);
            free(mapping);
        }
        if (cnv == NULL)
            fontmappingwarning(buffer, strlen(buffer), 2); /* not loadable */
        else if (gettracingfontsstate() > 1)
            fontmappingwarning(buffer, strlen(buffer), 0); /* tracing */
    } else {
        fontmappingwarning(buffer, strlen(buffer), 1); /* not found */
    }

    free(buffer);

    return cnv;
}

char *saved_mapping_name = NULL;
void
checkfortfmfontmapping(void)
{
    char* cp = strstr((char*)nameoffile + 1, ":mapping=");
    if (saved_mapping_name != NULL) {
        free(saved_mapping_name);
        saved_mapping_name = NULL;
    }
    if (cp != NULL) {
        *cp = 0;
        cp += 9;
        while (*cp && *cp <= ' ')
            ++cp;
        if (*cp)
            saved_mapping_name = xstrdup(cp);
    }
}

void*
loadtfmfontmapping(void)
{
    void* rval = NULL;
    if (saved_mapping_name != NULL) {
        rval = load_mapping_file(saved_mapping_name,
                saved_mapping_name + strlen(saved_mapping_name), 1);
        free(saved_mapping_name);
        saved_mapping_name = NULL;
    }
    return rval;
}

int
applytfmfontmapping(void* cnv, int c)
{
    UniChar in = c;
    Byte out[2];
    UInt32 inUsed, outUsed;
    /* TECkit_Status status; */
    /* status = */ TECkit_ConvertBuffer((TECkit_Converter)cnv,
            (const Byte*)&in, sizeof(in), &inUsed, out, sizeof(out), &outUsed, 1);
    if (outUsed < 1)
        return 0;
    else
        return out[0];
}

double
read_double(const char** s)
{
    int neg = 0;
    double val = 0.0;
    const char* cp = *s;

    while (*cp == ' '|| *cp == '\t')
        ++cp;
    if (*cp == '-') {
        neg = 1;
        ++cp;
    } else if (*cp == '+') {
        ++cp;
    }

    while (*cp >= '0' && *cp <= '9') {
        val = val * 10.0 + *cp - '0';
        ++cp;
    }
    if (*cp == '.') {
        double dec = 10.0;
        ++cp;
        while (*cp >= '0' && *cp <= '9') {
            val = val + (*cp - '0') / dec;
            ++cp;
            dec = dec * 10.0;
        }
    }
    *s = cp;

    return neg ? -val : val;
}

static hb_tag_t
read_tag_with_param(const char* cp, int* param)
{
    const char* cp2;
    hb_tag_t tag;

    cp2 = cp;
    while (*cp2 && (*cp2 != ':') && (*cp2 != ';') && (*cp2 != ',') && (*cp2 != '='))
        ++cp2;

    tag = hb_tag_from_string(cp, cp2 - cp);

    cp = cp2;
    if (*cp == '=') {
        int neg = 0;
        ++cp;
        if (*cp == '-') {
            ++neg;
            ++cp;
        }
        while (*cp >= '0' && *cp <= '9') {
            *param = *param * 10 + *cp - '0';
            ++cp;
        }
        if (neg)
            *param = -(*param);
    }

    return tag;
}

unsigned int
read_rgb_a(const char** cp)
{
    uint32_t rgbValue = 0;
    uint32_t alpha = 0;
    int i;
    for (i = 0; i < 6; ++i) {
        if ((**cp >= '0') && (**cp <= '9'))
            rgbValue = (rgbValue << 4) + **cp - '0';
        else if ((**cp >= 'A') && (**cp <= 'F'))
            rgbValue = (rgbValue << 4) + **cp - 'A' + 10;
        else if ((**cp >= 'a') && (**cp <= 'f'))
            rgbValue = (rgbValue << 4) + **cp - 'a' + 10;
        else
            return 0x000000FF;
        (*cp)++;
    }
    rgbValue <<= 8;
    for (i = 0; i < 2; ++i) {
        if ((**cp >= '0') && (**cp <= '9'))
            alpha = (alpha << 4) + **cp - '0';
        else if ((**cp >= 'A') && (**cp <= 'F'))
            alpha = (alpha << 4) + **cp - 'A' + 10;
        else if ((**cp >= 'a') && (**cp <= 'f'))
            alpha = (alpha << 4) + **cp - 'a' + 10;
        else
            break;
        (*cp)++;
    }
    if (i == 2)
        rgbValue += alpha;
    else
        rgbValue += 0xFF;
    return rgbValue;
}

int
readCommonFeatures(const char* feat, const char* end, float* extend, float* slant, float* embolden, float* letterspace, uint32_t* rgbValue)
    // returns 1 to go to next_option, -1 for bad_option, 0 to continue
{
    const char* sep;
    if (strncmp(feat, "mapping", 7) == 0) {
        sep = feat + 7;
        if (*sep != '=')
            return -1;
        loadedfontmapping = load_mapping_file(sep + 1, end, 0);
        return 1;
    }

    if (strncmp(feat, "extend", 6) == 0) {
        sep = feat + 6;
        if (*sep != '=')
            return -1;
        ++sep;
        *extend = read_double(&sep);
        return 1;
    }

    if (strncmp(feat, "slant", 5) == 0) {
        sep = feat + 5;
        if (*sep != '=')
            return -1;
        ++sep;
        *slant = read_double(&sep);
        return 1;
    }

    if (strncmp(feat, "embolden", 8) == 0) {
        sep = feat + 8;
        if (*sep != '=')
            return -1;
        ++sep;
        *embolden = read_double(&sep);
        return 1;
    }

    if (strncmp(feat, "letterspace", 11) == 0) {
        sep = feat + 11;
        if (*sep != '=')
            return -1;
        ++sep;
        *letterspace = read_double(&sep);
        return 1;
    }

    if (strncmp(feat, "color", 5) == 0) {
        const char* s;
        sep = feat + 5;
        if (*sep != '=')
            return -1;
        ++sep;
        s = sep;
        *rgbValue = read_rgb_a(&sep);
        if ((sep == s+6) || (sep == s+8))
            loadedfontflags |= FONT_FLAGS_COLORED;
        else
            return -1;
        return 1;
    }

    return 0;
}

static bool
readFeatureNumber(const char* s, const char* e, hb_tag_t* f, int* v)
    /* s...e is a "id=setting" string; */
{
    *f = 0;
    *v = 0;
    if (*s < '0' || *s > '9')
        return false;
    while (*s >= '0' && *s <= '9')
        *f = *f * 10 + *s++ - '0';
    while ((*s == ' ') || (*s == '\t'))
        ++s;
    if (*s++ != '=')
        /* no setting was specified */
        return false;

    if (*s < '0' || *s > '9')
        return false;
    while (*s >= '0' && *s <= '9')
        *v = *v * 10 + *s++ - '0';
    while ((*s == ' ') || (*s == '\t'))
        ++s;
    if (s != e)
        return false;
    return true;
}

static void*
loadOTfont(PlatformFontRef fontRef, XeTeXFont font, Fixed scaled_size, char* cp1)
{
    XeTeXLayoutEngine engine = NULL;
    hb_tag_t script = HB_TAG_NONE;
    char * language = NULL;
    hb_feature_t* features = NULL;
    char** shapers = NULL; /* NULL-terminated array */
    int nFeatures = 0;
    int nShapers = 0;

    char* cp2;
    char* cp3;

    hb_tag_t tag;

    uint32_t rgbValue = 0x000000FF;

    float extend = 1.0;
    float slant = 0.0;
    float embolden = 0.0;
    float letterspace = 0.0;

    int i;

    char reqEngine = getReqEngine();

    #ifdef WEBASSEMBLY_BUILD
    if (reqEngine == 'G') {
        dieWithReason("WEBASSEMBLY_BUILD does not graphite");
    }
    #endif

    if (reqEngine == 'O' || reqEngine == 'G') {
        shapers = (char**) xrealloc(shapers, (nShapers + 1) * sizeof(char *));
        if (reqEngine == 'O') {
            static char ot_const[] = "ot";
            shapers[nShapers] = ot_const;
        } else if (reqEngine == 'G') {
            static char graphite2_const[] = "graphite2";
            shapers[nShapers] = graphite2_const;
        }
        nShapers++;
    }

    if (reqEngine == 'G') {
        char* tmpShapers[] = {shapers[0]};
        /* create a default engine so we can query the font for Graphite features;
         * because of font caching, it's cheap to discard this and create the real one later */
        engine = createLayoutEngine(fontRef, font, script, language,
                features, nFeatures, tmpShapers, rgbValue, extend, slant, embolden);

        if (engine == NULL)
            return NULL;
    }

    /* scan the feature string (if any) */
    if (cp1 != NULL) {
        while (*cp1) {
            if ((*cp1 == ':') || (*cp1 == ';') || (*cp1 == ','))
                ++cp1;
            while ((*cp1 == ' ') || (*cp1 == '\t')) /* skip leading whitespace */
                ++cp1;
            if (*cp1 == 0)  /* break if end of string */
                break;

            cp2 = cp1;
            while (*cp2 && (*cp2 != ':') && (*cp2 != ';') && (*cp2 != ','))
                ++cp2;

            if (strncmp(cp1, "script", 6) == 0) {
                cp3 = cp1 + 6;
                if (*cp3 != '=')
                    goto bad_option;
                ++cp3;
                script = hb_tag_from_string(cp3, cp2 - cp3);
                goto next_option;
            }

            if (strncmp(cp1, "language", 8) == 0) {
                cp3 = cp1 + 8;
                if (*cp3 != '=')
                    goto bad_option;
                ++cp3;
                language = (char*)xmalloc(cp2 - cp3 + 1);
                language[cp2 - cp3] = '\0';
                memcpy(language, cp3, cp2 - cp3);
                goto next_option;
            }

            if (strncmp(cp1, "shaper", 6) == 0) {
                cp3 = cp1 + 6;
                if (*cp3 != '=')
                    goto bad_option;
                ++cp3;
                shapers = (char**) xrealloc(shapers, (nShapers + 1) * sizeof(char *));
                /* some dumb systems have no strndup() */
                shapers[nShapers] = strdup(cp3);
                shapers[nShapers][cp2 - cp3] = '\0';
                nShapers++;
                goto next_option;
            }

            i = readCommonFeatures(cp1, cp2, &extend, &slant, &embolden, &letterspace, &rgbValue);
            if (i == 1)
                goto next_option;
            else if (i == -1)
                goto bad_option;

            if (reqEngine == 'G') {
                int value = 0;
                if (readFeatureNumber(cp1, cp2, &tag, &value)
                 || findGraphiteFeature(engine, cp1, cp2, &tag, &value)) {
                    features = (hb_feature_t*) xrealloc(features, (nFeatures + 1) * sizeof(hb_feature_t));
                    features[nFeatures].tag = tag;
                    features[nFeatures].value = value;
                    features[nFeatures].start = 0;
                    features[nFeatures].end = (unsigned int) -1;
                    nFeatures++;
                    goto next_option;
                }
            }

            if (*cp1 == '+') {
                int param = 0;
                tag = read_tag_with_param(cp1 + 1, &param);
                features = (hb_feature_t*) xrealloc(features, (nFeatures + 1) * sizeof(hb_feature_t));
                features[nFeatures].tag = tag;
                features[nFeatures].start = 0;
                features[nFeatures].end = (unsigned int) -1;
                // for backward compatibility with pre-0.9999 where feature
                // indices started from 0
                if (param >= 0)
                    param++;
                features[nFeatures].value = param;
                nFeatures++;
                goto next_option;
            }

            if (*cp1 == '-') {
                ++cp1;
                tag = hb_tag_from_string(cp1, cp2 - cp1);
                features = (hb_feature_t*) xrealloc(features, (nFeatures + 1) * sizeof(hb_feature_t));
                features[nFeatures].tag = tag;
                features[nFeatures].start = 0;
                features[nFeatures].end = (unsigned int) -1;
                features[nFeatures].value = 0;
                nFeatures++;
                goto next_option;
            }

            if (strncmp(cp1, "vertical", 8) == 0) {
                cp3 = cp2;
                if (*cp3 == ';' || *cp3 == ':' || *cp3 == ',')
                    --cp3;
                while (*cp3 == '\0' || *cp3 == ' ' || *cp3 == '\t')
                    --cp3;
                if (*cp3)
                    ++cp3;
                if (cp3 == cp1 + 8) {
                    loadedfontflags |= FONT_FLAGS_VERTICAL;
                    goto next_option;
                }
            }

        bad_option:
            fontfeaturewarning((void*) cp1, cp2 - cp1, 0, 0);

        next_option:
            cp1 = cp2;
        }
    }

    if (shapers != NULL) {
        shapers = (char**) xrealloc(shapers, (nShapers + 1) * sizeof(char *));
        shapers[nShapers] = NULL;
    }

    if (embolden != 0.0)
        embolden = embolden * Fix2D(scaled_size) / 100.0;

    if (letterspace != 0.0)
        loadedfontletterspace = (letterspace / 100.0) * scaled_size;

    if ((loadedfontflags & FONT_FLAGS_COLORED) == 0)
        rgbValue = 0x000000FF;

    if ((loadedfontflags & FONT_FLAGS_VERTICAL) != 0)
        setFontLayoutDir(font, 1);

    engine = createLayoutEngine(fontRef, font, script, language,
                    features, nFeatures, shapers, rgbValue, extend, slant, embolden);

    if (engine == 0) {
        // only free these if creation failed, otherwise the engine now owns them
        free(features);
        free(shapers);
    } else {
        nativefonttypeflag = OTGR_FONT_FLAG;
    }

    return engine;
}

static void
splitFontName(char* name, char** var, char** feat, char** end, int* index)
{
    *var = NULL;
    *feat = NULL;
    *index = 0;
    if (*name == '[') {
        int withinFileName = 1;
#ifdef WIN32
        char* start = name + 1;
#endif
        ++name;
        while (*name) {
            if (withinFileName && *name == ']') {
                withinFileName = 0;
                if (*var == NULL)
                    *var = name;
            } else if (*name == ':') {
                if (withinFileName && *var == NULL
#ifdef WIN32
                    && !((name - start == 1) && isalpha(*start))
#endif
                    ) {
                    *var = name;
                    ++name;
                    while (*name >= '0' && *name <= '9')
                        *index = *index * 10 + *name++ - '0';
                    --name;
                } else if (!withinFileName && *feat == NULL)
                    *feat = name;
            }
            ++name;
        }
        *end = name;
    } else {
        while (*name) {
            if (*name == '/' && *var == NULL && *feat == NULL)
                *var = name;
            else if (*name == ':' && *feat == NULL)
                *feat = name;
            ++name;
        }
        *end = name;
    }
    if (*feat == NULL)
        *feat = name;
    if (*var == NULL)
        *var = *feat;
}

#ifdef WEBASSEMBLY_BUILD
char* fontconfig_search_font(const char* fontname, const char *varString);
#endif

void*
findnativefont(unsigned char* uname, integer scaled_size)
    /* scaled_size here is in TeX points, or is a negative integer for 'scaled' */
{
    void* rval = NULL;
    char* nameString;
    char* var;
    char* feat;
    char* end;
    char* name = (char*)uname;
    char* varString = NULL;
    char* featString = NULL;
    PlatformFontRef fontRef;
    XeTeXFont font = NULL;
    int index = 0;

    loadedfontmapping = NULL;
    loadedfontflags = 0;
    loadedfontletterspace = 0;

    splitFontName(name, &var, &feat, &end, &index);
    nameString = (char*) xmalloc(var - name + 1);
    strncpy(nameString, name, var - name);
    nameString[var - name] = 0;

    if (feat > var) {
        varString = (char*) xmalloc(feat - var);
        strncpy(varString, var + 1, feat - var - 1);
        varString[feat - var - 1] = 0;
    }

    if (end > feat) {
        featString = (char*) xmalloc(end - feat);
        strncpy(featString, feat + 1, end - feat - 1);
        featString[end - feat - 1] = 0;
    }


    // printf("%s nope: %s\n", nameString, varString);
    // check for "[filename]" form, don't search maps in this case
    if (nameString[0] == '[') {
        char* path = kpse_find_file(nameString + 1, kpse_opentype_format, 0);
        if (path == NULL)
            path = kpse_find_file(nameString + 1, kpse_truetype_format, 0);
        if (path == NULL)
            path = kpse_find_file(nameString + 1, kpse_type1_format, 0);
        if (path != NULL) {
            if (scaled_size < 0) {
                font = createFontFromFile(path, index, 655360L);
                if (font != NULL) {
                    Fixed dsize = D2Fix(getDesignSize(font));
                    if (scaled_size == -1000)
                        scaled_size = dsize;
                    else
                        scaled_size = zxnoverd(dsize, -scaled_size, 1000);
                    deleteFont(font);
                }
            }
            font = createFontFromFile(path, index, scaled_size);
            if (font != NULL) {
                loadedfontdesignsize = D2Fix(getDesignSize(font));

                /* This is duplicated in XeTeXFontMgr::findFont! */
                setReqEngine(0);
                if (varString) {
                    if (strncmp(varString, "/AAT", 4) == 0)
                        setReqEngine('A');
                    else if ((strncmp(varString, "/OT", 3) == 0) || (strncmp(varString, "/ICU", 4) == 0))
                        setReqEngine('O');
                    else if (strncmp(varString, "/GR", 3) == 0) {
                        #ifdef WEBASSEMBLY_BUILD
                        dieWithReason("WEBASSEMBLY_BUILD does not graphite");
                        #endif
                        setReqEngine('G');
                    }
                }

                rval = loadOTfont(0, font, scaled_size, featString);
                if (rval == NULL)
                    deleteFont(font);
                if (rval != NULL && gettracingfontsstate() > 0) {
                    begindiagnostic();
                    zprintnl(' ');
                    printcstring("-> ");
                    printcstring(path);
                    zenddiagnostic(0);
                }
            }
        }
    } else {
#ifndef WEBASSEMBLY_BUILD
        // printf("Finding %s %s %lf\n", nameString, varString, Fix2D(scaled_size));
        fontRef = findFontByName(nameString, varString, Fix2D(scaled_size));

        if (fontRef != 0) {
            /* update nameoffile to the full name of the font, for error messages during font loading */
            const char* fullName = getFullName(fontRef);
            namelength = strlen(fullName);
            if (featString != NULL)
                namelength += strlen(featString) + 1;
            if (varString != NULL)
                namelength += strlen(varString) + 1;
            free(nameoffile);
            nameoffile = xmalloc(namelength + 4); /* +2 would be correct: initial space, final NUL */
            nameoffile[0] = ' ';
            strcpy((char*)nameoffile + 1, fullName);            
            if (scaled_size < 0) {
                font = createFont(fontRef, scaled_size);
                if (font != NULL) {
                    Fixed dsize = D2Fix(getDesignSize(font));
                    if (scaled_size == -1000)
                        scaled_size = dsize;
                    else
                        scaled_size = zxnoverd(dsize, -scaled_size, 1000);
                    deleteFont(font);
                }
            }

            font = createFont(fontRef, scaled_size);
            if (font != NULL) {
                rval = loadOTfont(fontRef, font, scaled_size, featString);
                if (rval == NULL)
                    deleteFont(font);
            }

            /* append the style and feature strings, so that \show\fontID will give a full result */
            if (varString != NULL && *varString != 0) {
                strcat((char*)nameoffile + 1, "/");
                strcat((char*)nameoffile + 1, varString);
            }
            if (featString != NULL && *featString != 0) {
                strcat((char*)nameoffile + 1, ":");
                strcat((char*)nameoffile + 1, featString);
            }
            namelength = strlen((char*)nameoffile + 1);
        }
#else  //No WebAssembly BUILD
        char *fullName = fontconfig_search_font(nameString, varString);
        if (fullName) {
            namelength = strlen(fullName);
            free(nameoffile);
            nameoffile = xmalloc(namelength + 8); /* +2 would be correct: initial space, final NUL */
            nameoffile[0] = ' ';
            strcpy((char*)nameoffile + 1, fullName);

            if (scaled_size < 0) {
                font = createFontFromFile(fullName, index, 655360L);
                if (font != NULL) {
                    Fixed dsize = D2Fix(getDesignSize(font));
                    if (scaled_size == -1000)
                        scaled_size = dsize;
                    else
                        scaled_size = zxnoverd(dsize, -scaled_size, 1000);
                    deleteFont(font);
                }
            }

            font = createFontFromFile(fullName, index, scaled_size);
            if (font != NULL) {
                loadedfontdesignsize = D2Fix(getDesignSize(font));

                /* This is duplicated in XeTeXFontMgr::findFont! */
                setReqEngine('O');

                rval = loadOTfont(0, font, scaled_size, featString);
                if (rval == NULL)
                    deleteFont(font);
            }

            namelength = strlen((char*)nameoffile + 1);
            free(fullName);
        }
#endif // HAS WebAssembly BUILD
    }

    if (varString != NULL)
        free(varString);

    if (featString != NULL)
        free(featString);

    free(nameString);
    return rval;
}

void
releasefontengine(void* engine, int type_flag)
{
#ifdef XETEX_MAC
    if (type_flag == AAT_FONT_FLAG) {
        CFRelease((CFDictionaryRef)engine);
    } else
#endif
    if (type_flag == OTGR_FONT_FLAG) {
        deleteLayoutEngine((XeTeXLayoutEngine)engine);
    }
}

/* params are given as 'integer' in the header file, but are really TeX scaled integers */
void
otgetfontmetrics(void* pEngine, scaled* ascent, scaled* descent, scaled* xheight, scaled* capheight, scaled* slant)
{
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    float a, d;

    getAscentAndDescent(engine, &a, &d);
    *ascent = D2Fix(a);
    *descent = D2Fix(d);

    *slant = D2Fix(Fix2D(getSlant(getFont(engine))) * getExtendFactor(engine)
                    + getSlantFactor(engine));

    /* get cap and x height from OS/2 table */
    getCapAndXHeight(engine, &a, &d);
    *capheight = D2Fix(a);
    *xheight = D2Fix(d);

    /* fallback in case the font does not have OS/2 table */
    if (*xheight == 0) {
        int glyphID = mapCharToGlyph(engine, 'x');
        if (glyphID != 0) {
            getGlyphHeightDepth(engine, glyphID, &a, &d);
            *xheight = D2Fix(a);
        } else {
            *xheight = *ascent / 2; /* arbitrary figure if there's no 'x' in the font */
        }
    }

    if (*capheight == 0) {
        int glyphID = mapCharToGlyph(engine, 'X');
        if (glyphID != 0) {
            getGlyphHeightDepth(engine, glyphID, &a, &d);
            *capheight = D2Fix(a);
        } else {
            *capheight = *ascent; /* arbitrary figure if there's no 'X' in the font */
        }
    }
}

integer
otfontget(integer what, void* pEngine)
{
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    XeTeXFont fontInst = getFont(engine);
    switch (what) {
        case XeTeX_count_glyphs:
            return countGlyphs(fontInst);
            break;

        case XeTeX_count_features: /* ie Graphite features */
            return countGraphiteFeatures(engine);
            break;

        case XeTeX_OT_count_scripts:
            return countScripts(fontInst);
            break;
    }
    return 0;
}


integer
otfontget1(integer what, void* pEngine, integer param)
{
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    XeTeXFont fontInst = getFont(engine);
    switch (what) {
        case XeTeX_OT_count_languages:
            return countLanguages(fontInst, param);
            break;

        case XeTeX_OT_script_code:
            return getIndScript(fontInst, param);
            break;

        /* for graphite fonts...*/
        case XeTeX_feature_code:
            return getGraphiteFeatureCode(engine, param);
            break;
        case XeTeX_is_exclusive_feature:
            return 1;
            break;
        case XeTeX_count_selectors:
            return countGraphiteFeatureSettings(engine, param);
            break;
    }
    return 0;
}


integer
otfontget2(integer what, void* pEngine, integer param1, integer param2)
{
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    XeTeXFont fontInst = getFont(engine);
    switch (what) {
        case XeTeX_OT_language_code:
            return getIndLanguage(fontInst, param1, param2);
            break;

        case XeTeX_OT_count_features:
            return countFeatures(fontInst, param1, param2);
            break;

        /* for graphite fonts */
        case XeTeX_selector_code:
            return getGraphiteFeatureSettingCode(engine, param1, param2);
            break;
        case XeTeX_is_default_selector:
            return getGraphiteFeatureDefaultSetting(engine, param1) == param2;
            break;
    }

    return 0;
}


integer
otfontget3(integer what, void* pEngine, integer param1, integer param2, integer param3)
{
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    XeTeXFont fontInst = getFont(engine);
    switch (what) {
        case XeTeX_OT_feature_code:
            return getIndFeature(fontInst, param1, param2, param3);
            break;
    }

    return 0;
}

void
grprintfontname(integer what, void* pEngine, integer param1, integer param2)
{
#ifndef WEBASSEMBLY_BUILD
    char* name = NULL;
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    switch (what) {
        case XeTeX_feature_name:
            name = getGraphiteFeatureLabel(engine, param1);
            break;
        case XeTeX_selector_name:
            name = getGraphiteFeatureSettingLabel(engine, param1, param2);
            break;
    }

    if (name != NULL) {
        printcstring(name);
        gr_label_destroy(name);
    }
#endif
}

integer
grfontgetnamed(integer what, void* pEngine)
{
    long rval = -1;
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    switch (what) {
        case XeTeX_find_feature_by_name:
            rval = findGraphiteFeatureNamed(engine, (const char*)nameoffile + 1, namelength);
            break;
    }
    return rval;
}

integer
grfontgetnamed1(integer what, void* pEngine, integer param)
{
    long rval = -1;
    XeTeXLayoutEngine engine = (XeTeXLayoutEngine)pEngine;
    switch (what) {
        case XeTeX_find_selector_by_name:
            rval = findGraphiteFeatureSettingNamed(engine, param, (const char*)nameoffile + 1, namelength);
            break;
    }
    return rval;
}

#define XDV_FLAG_VERTICAL       0x0100
#define XDV_FLAG_COLORED        0x0200
#define XDV_FLAG_EXTEND         0x1000
#define XDV_FLAG_SLANT          0x2000
#define XDV_FLAG_EMBOLDEN       0x4000

#ifdef XETEX_MAC
static UInt32
cgColorToRGBA32(CGColorRef color)
{
    const CGFloat *components = CGColorGetComponents(color);

    UInt32 rval = (UInt8)(components[0] * 255.0 + 0.5);
    rval <<= 8;
    rval += (UInt8)(components[1] * 255.0 + 0.5);
    rval <<= 8;
    rval += (UInt8)(components[2] * 255.0 + 0.5);
    rval <<= 8;
    rval += (UInt8)(components[3] * 255.0 + 0.5);
    return rval;
}
#endif

static int xdvBufSize = 0;

int
makeXDVGlyphArrayData(void* pNode)
{
    unsigned char* cp;
    uint16_t* glyphIDs;
    memoryword* p = (memoryword*) pNode;
    void* glyph_info;
    FixedPoint* locations;
    Fixed width;
    uint16_t glyphCount = native_glyph_count(p);

    int i = glyphCount * native_glyph_info_size + 8; /* to guarantee enough space in the buffer */
    if (i > xdvBufSize) {
        if (xdvbuffer != NULL)
            free(xdvbuffer);
        xdvBufSize = ((i / 1024) + 1) * 1024;
        xdvbuffer = (char*) xmalloc(xdvBufSize);
    }

    glyph_info = native_glyph_info_ptr(p);
    locations = (FixedPoint*)glyph_info;
    glyphIDs = (uint16_t*)(locations + glyphCount);

    cp = (unsigned char*)xdvbuffer;

    width = node_width(p);
    *cp++ = (width >> 24) & 0xff;
    *cp++ = (width >> 16) & 0xff;
    *cp++ = (width >> 8) & 0xff;
    *cp++ = width & 0xff;

    *cp++ = (glyphCount >> 8) & 0xff;
    *cp++ = glyphCount & 0xff;

    for (i = 0; i < glyphCount; ++i) {
        Fixed x = locations[i].x;
        Fixed y = locations[i].y;
        *cp++ = (x >> 24) & 0xff;
        *cp++ = (x >> 16) & 0xff;
        *cp++ = (x >> 8) & 0xff;
        *cp++ = x & 0xff;
        *cp++ = (y >> 24) & 0xff;
        *cp++ = (y >> 16) & 0xff;
        *cp++ = (y >> 8) & 0xff;
        *cp++ = y & 0xff;
    }

    for (i = 0; i < glyphCount; ++i) {
        uint16_t g = glyphIDs[i];
        *cp++ = (g >> 8) & 0xff;
        *cp++ = g & 0xff;
    }

    return ((char*)cp - xdvbuffer);
}

int
makefontdef(integer f)
{
    uint16_t flags = 0;
    uint32_t rgba;
    Fixed size;
    char* filename;
    uint32_t index;
    uint8_t filenameLen;
    int fontDefLength;
    char* cp;
    /* PlatformFontRef fontRef = 0; */
    float extend = 1.0;
    float slant = 0.0;
    float embolden = 0.0;

#ifdef XETEX_MAC
    CFDictionaryRef attributes = NULL;

    if (fontarea[f] == AAT_FONT_FLAG) {
        CTFontRef font;
        CGColorRef color;
        CGAffineTransform t;
        CFNumberRef emboldenNumber;
        CGFloat fSize;

        attributes = (CFDictionaryRef) fontlayoutengine[f];
        font = CFDictionaryGetValue(attributes, kCTFontAttributeName);

        filename = getFileNameFromCTFont(font, &index);
        assert(filename);

        if (CFDictionaryGetValue(attributes, kCTVerticalFormsAttributeName))
            flags |= XDV_FLAG_VERTICAL;

        color = (CGColorRef) CFDictionaryGetValue(attributes, kCTForegroundColorAttributeName);
        if (color)
            rgba = cgColorToRGBA32(color);

        t = CTFontGetMatrix(font);
        extend = t.a;
        slant = t.c;

        emboldenNumber = CFDictionaryGetValue(attributes, kXeTeXEmboldenAttributeName);
        if (emboldenNumber)
            CFNumberGetValue(emboldenNumber, kCFNumberFloatType, &embolden);

        fSize = CTFontGetSize(font);
        size = D2Fix(fSize);
    } else
#endif
    if (fontarea[f] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine;

        engine = (XeTeXLayoutEngine)fontlayoutengine[f];
        /* fontRef = */ getFontRef(engine);
        filename = getFontFilename(engine, &index);
        assert(filename);

        rgba = getRgbValue(engine);
        if ((fontflags[f] & FONT_FLAGS_VERTICAL) != 0)
            flags |= XDV_FLAG_VERTICAL;

        extend = getExtendFactor(engine);
        slant = getSlantFactor(engine);
        embolden = getEmboldenFactor(engine);

        size = D2Fix(getPointSize(engine));
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `make_font_def'\n");
        exit(3);
    }

    filenameLen = strlen(filename);

    /* parameters after internal font ID:
    //  size[4]
    //  flags[2]
    //  l[1] n[l]
    //  if flags & COLORED:
    //      c[4]
    */

    fontDefLength
        = 4 /* size */
        + 2 /* flags */
        + 1 /* name length */
        + filenameLen
        + 4 /* face index */;

    if ((fontflags[f] & FONT_FLAGS_COLORED) != 0) {
        fontDefLength += 4; /* 32-bit RGBA value */
        flags |= XDV_FLAG_COLORED;
    }

    if (extend != 1.0) {
        fontDefLength += 4;
        flags |= XDV_FLAG_EXTEND;
    }
    if (slant != 0.0) {
        fontDefLength += 4;
        flags |= XDV_FLAG_SLANT;
    }
    if (embolden != 0.0) {
        fontDefLength += 4;
        flags |= XDV_FLAG_EMBOLDEN;
    }

    if (fontDefLength > xdvBufSize) {
        if (xdvbuffer != NULL)
            free(xdvbuffer);
        xdvBufSize = ((fontDefLength / 1024) + 1) * 1024;
        xdvbuffer = (char*) xmalloc(xdvBufSize);
    }
    cp = xdvbuffer;

    *(Fixed*)cp = SWAP32(size);
    cp += 4;

    *(uint16_t*)cp = SWAP16(flags);
    cp += 2;

    *(uint8_t*)cp = filenameLen;
    cp += 1;
    memcpy(cp, filename, filenameLen);
    cp += filenameLen;

    *(uint32_t*)cp = SWAP32(index);
    cp += 4;

    if ((fontflags[f] & FONT_FLAGS_COLORED) != 0) {
        *(uint32_t*)cp = SWAP32(rgba);
        cp += 4;
    }

    if (flags & XDV_FLAG_EXTEND) {
        Fixed f = D2Fix(extend);
        *(uint32_t*)(cp) = SWAP32(f);
        cp += 4;
    }
    if (flags & XDV_FLAG_SLANT) {
        Fixed f = D2Fix(slant);
        *(uint32_t*)(cp) = SWAP32(f);
        cp += 4;
    }
    if (flags & XDV_FLAG_EMBOLDEN) {
        Fixed f = D2Fix(embolden);
        *(uint32_t*)(cp) = SWAP32(f);
        cp += 4;
    }

    htmlnativefontout(f, size, filename, extend, slant, embolden, rgba);
    free((char*) filename);

    return fontDefLength;
}

int
applymapping(void* pCnv, uint16_t* txtPtr, int txtLen)
{
    TECkit_Converter cnv = (TECkit_Converter)pCnv;
    UInt32 inUsed, outUsed;
    TECkit_Status status;
    static UInt32 outLength = 0;

    /* allocate outBuffer if not big enough */
    if (outLength < txtLen * sizeof(UniChar) + 32) {
        if (mappedtext != 0)
            free(mappedtext);
        outLength = txtLen * sizeof(UniChar) + 32;
        mappedtext = xmalloc(outLength);
    }

    /* try the mapping */
retry:
    status = TECkit_ConvertBuffer(cnv,
            (Byte*)txtPtr, txtLen * sizeof(UniChar), &inUsed,
            (Byte*)mappedtext, outLength, &outUsed, true);

    switch (status) {
        case kStatus_NoError:
            txtPtr = (UniChar*)mappedtext;
            return outUsed / sizeof(UniChar);

        case kStatus_OutputBufferFull:
            outLength += (txtLen * sizeof(UniChar)) + 32;
            free(mappedtext);
            mappedtext = xmalloc(outLength);
            goto retry;

        default:
            return 0;
    }
}

static void
snap_zone(scaled* value, scaled snap_value, scaled fuzz)
{
    scaled difference = *value - snap_value;
    if (difference <= fuzz && difference >= -fuzz)
        *value = snap_value;
}

void
getnativecharheightdepth(integer font, integer ch, scaled* height, scaled* depth)
{
#define QUAD(f)         fontinfo[6+parambase[f]].cint
#define X_HEIGHT(f)     fontinfo[5+parambase[f]].cint
#define CAP_HEIGHT(f)   fontinfo[8+parambase[f]].cint

    float ht = 0.0;
    float dp = 0.0;
    Fixed fuzz;

#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG) {
        CFDictionaryRef attributes = (CFDictionaryRef)(fontlayoutengine[font]);
        int gid = MapCharToGlyph_AAT(attributes, ch);
        GetGlyphHeightDepth_AAT(attributes, gid, &ht, &dp);
    } else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[font];
        int gid = mapCharToGlyph(engine, ch);
        getGlyphHeightDepth(engine, gid, &ht, &dp);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `get_native_char_height_depth`\n");
        exit(3);
    }

    *height = D2Fix(ht);
    *depth = D2Fix(dp);

    /* snap to "known" zones for baseline, x-height, cap-height if within 4% of em-size */
    fuzz = QUAD(font) / 25;
    snap_zone(depth, 0, fuzz);
    snap_zone(height, 0, fuzz);
    snap_zone(height, X_HEIGHT(font), fuzz);
    snap_zone(height, CAP_HEIGHT(font), fuzz);
}

scaled
getnativecharht(integer f, integer c)
{
    scaled h, d;
    getnativecharheightdepth(f, c, &h, &d);
    return h;
}

scaled
getnativechardp(integer f, integer c)
{
    scaled h, d;
    getnativecharheightdepth(f, c, &h, &d);
    return d;
}

void
getnativecharsidebearings(integer font, integer ch, scaled* lsb, scaled* rsb)
{
    float l, r;

#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG) {
        CFDictionaryRef attributes = (CFDictionaryRef)(fontlayoutengine[font]);
        int gid = MapCharToGlyph_AAT(attributes, ch);
        GetGlyphSidebearings_AAT(attributes, gid, &l, &r);
    } else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[font];
        int gid = mapCharToGlyph(engine, ch);
        getGlyphSidebearings(engine, gid, &l, &r);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `get_native_char_side_bearings'\n");
        exit(3);
    }

    *lsb = D2Fix(l);
    *rsb = D2Fix(r);
}

scaled
getglyphbounds(integer font, integer edge, integer gid)
{
/* edge codes 1,2,3,4 => L T R B */
    float a, b;

#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG) {
        CFDictionaryRef attributes = (CFDictionaryRef)(fontlayoutengine[font]);
        if (edge & 1)
            GetGlyphSidebearings_AAT(attributes, gid, &a, &b);
        else
            GetGlyphHeightDepth_AAT(attributes, gid, &a, &b);
    } else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[font];
        if (edge & 1)
            getGlyphSidebearings(engine, gid, &a, &b);
        else
            getGlyphHeightDepth(engine, gid, &a, &b);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `get_glyph_bounds'\n");
        exit(3);
    }
    return D2Fix((edge <= 2) ? a : b);
}

scaled
getnativecharic(integer f, integer c)
{
    scaled lsb, rsb;
    getnativecharsidebearings(f, c, &lsb, &rsb);
    if (rsb < 0)
        return fontletterspace[f] - rsb;
    else
        return fontletterspace[f];
}

scaled
getnativecharwd(integer f, integer c)
{
    scaled wd = 0;
#ifdef XETEX_MAC
    if (fontarea[f] == AAT_FONT_FLAG) {
        CFDictionaryRef attributes = (CFDictionaryRef)(fontlayoutengine[f]);
        int gid = MapCharToGlyph_AAT(attributes, c);
        wd = D2Fix(GetGlyphWidth_AAT(attributes, gid));
    } else
#endif
    if (fontarea[f] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[f];
        int gid = mapCharToGlyph(engine, c);
        wd = D2Fix(getGlyphWidthFromEngine(engine, gid));
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `get_native_char_wd'\n");
        exit(3);
    }
    return wd;
}

uint16_t
get_native_glyph(void* pNode, unsigned index)
{
    memoryword* node = (memoryword*)pNode;
    FixedPoint* locations = (FixedPoint*)native_glyph_info_ptr(node);
    uint16_t* glyphIDs = (uint16_t*)(locations + native_glyph_count(node));
    if (index >= native_glyph_count(node)){
        return 0;
    }
    else {
        uint16_t res = glyphIDs[index];
        if(index == 0) { //Our patch only works when index = 0
          unsigned short* text = (unsigned short*)(node + native_node_size);
          int txtLen = native_length(node);
          unsigned int unic = text[0];
          if(txtLen == 2) {
            unic = ((text[0] - 0xD800) * 0x400) + (text[1] - 0xDC00) + 0x10000;
          } 
          store_charglyph_map(native_font(node), res, unic);
          // printf("!!store, glyph %d char %d\n",res, unic);
        } 
        return res;
    }
}

void
store_justified_native_glyphs(void* pNode)
{
    memoryword* node = (memoryword*)pNode;
    unsigned f = native_font(node);

#ifdef XETEX_MAC /* separate Mac-only codepath for AAT fonts */
    if (fontarea[f] == AAT_FONT_FLAG) {
        (void)DoAATLayout(node, 1);
        return;
    }
#endif

    /* save desired width */
    int savedWidth = node_width(node);

    measure_native_node(node, 0);

    if (node_width(node) != savedWidth) {
        /* see how much adjustment is needed overall */
        double justAmount = Fix2D(savedWidth - node_width(node));

        /* apply justification to spaces (or if there are none, distribute it to all glyphs as a last resort) */
        FixedPoint* locations = (FixedPoint*)native_glyph_info_ptr(node);
        uint16_t* glyphIDs = (uint16_t*)(locations + native_glyph_count(node));
        int glyphCount = native_glyph_count(node);
        int spaceCount = 0, i;

        int spaceGlyph = mapchartoglyph(f, ' ');
        for (i = 0; i < glyphCount; ++i)
            if (glyphIDs[i] == spaceGlyph)
                spaceCount++;

        if (spaceCount > 0) {
            double adjustment = 0;
            int spaceIndex = 0;
            for (i = 0; i < glyphCount; ++i) {
                locations[i].x = D2Fix(Fix2D(locations[i].x) + adjustment);
                if (glyphIDs[i] == spaceGlyph) {
                    spaceIndex++;
                    adjustment = justAmount * spaceIndex / spaceCount;
                }
            }
        } else {
            for (i = 1; i < glyphCount; ++i)
                locations[i].x = D2Fix(Fix2D(locations[i].x) + justAmount * i / (glyphCount - 1));
        }

        node_width(node) = savedWidth;
    }
}

void
measure_native_node(void* pNode, int use_glyph_metrics)
{
    memoryword* node = (memoryword*)pNode;
    int txtLen = native_length(node);
    uint16_t* txtPtr = (uint16_t*)(node + native_node_size);

    unsigned f = native_font(node);

#ifdef XETEX_MAC
    if (fontarea[f] == AAT_FONT_FLAG) {
        /* we're using this font in AAT mode, so fontlayoutengine[f] is actually a CFDictionaryRef */
        DoAATLayout(node, 0);
    } else
#endif
    if (fontarea[f] == OTGR_FONT_FLAG) {
        /* using this font in OT Layout mode, so fontlayoutengine[f] is actually a XeTeXLayoutEngine */

        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)(fontlayoutengine[f]);

        FixedPoint* locations = NULL;
        uint16_t* glyphIDs;
        Fixed* glyphAdvances = NULL;
        int totalGlyphCount = 0;

        /* need to find direction runs within the text, and call layoutChars separately for each */

        UBiDiDirection dir;
        void* glyph_info = 0;
        static FloatPoint* positions = 0;
        static float* advances = 0;
        static uint32_t* glyphs = 0;
#ifndef WEBASSEMBLY_BUILD
        UBiDi* pBiDi = ubidi_open();

        UErrorCode errorCode = U_ZERO_ERROR;
        ubidi_setPara(pBiDi, (const UChar*) txtPtr, txtLen, getDefaultDirection(engine), NULL, &errorCode);

        dir = ubidi_getDirection(pBiDi);
#else
        dir = UBIDI_LTR;
#endif

        if (dir == UBIDI_MIXED) {
            /* we actually do the layout twice here, once to count glyphs and then again to get them;
               which is inefficient, but i figure that MIXED is a relatively rare occurrence, so i can't be
               bothered to deal with the memory reallocation headache of doing it differently
            */
#ifndef WEBASSEMBLY_BUILD
            int nRuns = ubidi_countRuns(pBiDi, &errorCode);
            double width = 0;
            int i, runIndex;
            int32_t logicalStart, length;
            for (runIndex = 0; runIndex < nRuns; ++runIndex) {
                dir = ubidi_getVisualRun(pBiDi, runIndex, &logicalStart, &length);
                totalGlyphCount += layoutChars(engine, txtPtr, logicalStart, length, txtLen, (dir == UBIDI_RTL));
            }

            if (totalGlyphCount > 0) {
                double x, y;
                glyph_info = xcalloc(totalGlyphCount, native_glyph_info_size);
                locations = (FixedPoint*)glyph_info;
                glyphIDs = (uint16_t*)(locations + totalGlyphCount);
                glyphAdvances = (Fixed*) xcalloc(totalGlyphCount, sizeof(Fixed));
                totalGlyphCount = 0;

                x = y = 0.0;
                for (runIndex = 0; runIndex < nRuns; ++runIndex) {
                    int nGlyphs;
                    dir = ubidi_getVisualRun(pBiDi, runIndex, &logicalStart, &length);
                    nGlyphs = layoutChars(engine, txtPtr, logicalStart, length, txtLen,
                                            (dir == UBIDI_RTL));

                    glyphs = (uint32_t*) xcalloc(nGlyphs, sizeof(uint32_t));
                    positions = (FloatPoint*) xcalloc(nGlyphs + 1, sizeof(FloatPoint));
                    advances = (float*) xcalloc(nGlyphs, sizeof(float));

                    getGlyphs(engine, glyphs);
                    getGlyphAdvances(engine, advances);
                    getGlyphPositions(engine, positions);

                    for (i = 0; i < nGlyphs; ++i) {
                        glyphIDs[totalGlyphCount] = glyphs[i];
                        locations[totalGlyphCount].x = D2Fix(positions[i].x + x);
                        locations[totalGlyphCount].y = D2Fix(positions[i].y + y);
                        glyphAdvances[totalGlyphCount] = D2Fix(advances[i]);
                        ++totalGlyphCount;
                    }
                    x += positions[nGlyphs].x;
                    y += positions[nGlyphs].y;

                    free(glyphs);
                    free(positions);
                    free(advances);
                }
                width = x;
            }

            node_width(node) = D2Fix(width);
            native_glyph_count(node) = totalGlyphCount;
            native_glyph_info_ptr(node) = glyph_info;
#endif
        } else {
            double width = 0;
            totalGlyphCount = layoutChars(engine, txtPtr, 0, txtLen, txtLen, (dir == UBIDI_RTL));

            glyphs = (uint32_t*) xcalloc(totalGlyphCount, sizeof(uint32_t));
            positions = (FloatPoint*) xcalloc(totalGlyphCount + 1, sizeof(FloatPoint));
            advances = (float*) xcalloc(totalGlyphCount, sizeof(float));

            getGlyphs(engine, glyphs);
            getGlyphAdvances(engine, advances);
            getGlyphPositions(engine, positions);

            if (totalGlyphCount > 0) {
                int i;
                glyph_info = xcalloc(totalGlyphCount, native_glyph_info_size);
                locations = (FixedPoint*)glyph_info;
                glyphIDs = (uint16_t*)(locations + totalGlyphCount);
                glyphAdvances = (Fixed*) xcalloc(totalGlyphCount, sizeof(Fixed));
                for (i = 0; i < totalGlyphCount; ++i) {
                    glyphIDs[i] = glyphs[i];
                    glyphAdvances[i] = D2Fix(advances[i]);
                    locations[i].x = D2Fix(positions[i].x);
                    locations[i].y = D2Fix(positions[i].y);
                }
                width = positions[totalGlyphCount].x;
            }

            node_width(node) = D2Fix(width);
            native_glyph_count(node) = totalGlyphCount;
            native_glyph_info_ptr(node) = glyph_info;

            free(glyphs);
            free(positions);
            free(advances);
        }

#ifndef WEBASSEMBLY_BUILD
        ubidi_close(pBiDi);
#endif

        if (fontletterspace[f] != 0) {
            Fixed lsDelta = 0;
            Fixed lsUnit = fontletterspace[f];
            int i;
            for (i = 0; i < totalGlyphCount; ++i) {
                if (glyphAdvances[i] == 0 && lsDelta != 0)
                    lsDelta -= lsUnit;
                locations[i].x += lsDelta;
                lsDelta += lsUnit;
            }
            if (lsDelta != 0) {
                lsDelta -= lsUnit;
                node_width(node) += lsDelta;
            }
        }
        free(glyphAdvances);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `measure_native_node'\n");
        exit(3);
    }

    if (use_glyph_metrics == 0 || native_glyph_count(node) == 0) {
        /* for efficiency, height and depth are the font's ascent/descent,
            not true values based on the actual content of the word,
            unless use_glyph_metrics is non-zero */
        node_height(node) = heightbase[f];
        node_depth(node) = depthbase[f];
    } else {
        /* this iterates over the glyph data whether it comes from AAT or OT layout */
        FixedPoint* locations = (FixedPoint*)native_glyph_info_ptr(node);
        uint16_t* glyphIDs = (uint16_t*)(locations + native_glyph_count(node));
        float yMin = 65536.0;
        float yMax = -65536.0;
        int i;
        for (i = 0; i < native_glyph_count(node); ++i) {
            float ht, dp;
            float y = Fix2D(-locations[i].y); /* NB negative is upwards in locations[].y! */

            GlyphBBox bbox;
            if (getCachedGlyphBBox(f, glyphIDs[i], &bbox) == 0) {
#ifdef XETEX_MAC
                if (fontarea[f] == AAT_FONT_FLAG)
                    GetGlyphBBox_AAT((CFDictionaryRef)(fontlayoutengine[f]), glyphIDs[i], &bbox);
                else
#endif
                if (fontarea[f] == OTGR_FONT_FLAG)
                    getGlyphBounds((XeTeXLayoutEngine)(fontlayoutengine[f]), glyphIDs[i], &bbox);

                cacheGlyphBBox(f, glyphIDs[i], &bbox);
            }

            ht = bbox.yMax;
            dp = -bbox.yMin;

            if (y + ht > yMax)
                yMax = y + ht;
            if (y - dp < yMin)
                yMin = y - dp;
        }
        node_height(node) = D2Fix(yMax);
        node_depth(node) = -D2Fix(yMin);
    }
}

Fixed
get_native_italic_correction(void* pNode)
{
    memoryword* node = (memoryword*) pNode;
    unsigned f = native_font(node);
    unsigned n = native_glyph_count(node);
    if (n > 0) {
        FixedPoint* locations = (FixedPoint*)native_glyph_info_ptr(node);
        uint16_t* glyphIDs = (uint16_t*)(locations + n);

#ifdef XETEX_MAC
        if (fontarea[f] == AAT_FONT_FLAG)
            return D2Fix(GetGlyphItalCorr_AAT((CFDictionaryRef)(fontlayoutengine[f]), glyphIDs[n-1]))
                    + fontletterspace[f];
#endif
        if (fontarea[f] == OTGR_FONT_FLAG)
            return D2Fix(getGlyphItalCorr((XeTeXLayoutEngine)(fontlayoutengine[f]), glyphIDs[n-1]))
                    + fontletterspace[f];
    }

    return 0;
}


Fixed
get_native_glyph_italic_correction(void* pNode)
{
    memoryword* node = (memoryword*) pNode;
    uint16_t gid = native_glyph(node);
    unsigned f = native_font(node);

#ifdef XETEX_MAC
    if (fontarea[f] == AAT_FONT_FLAG)
        return D2Fix(GetGlyphItalCorr_AAT((CFDictionaryRef)(fontlayoutengine[f]), gid));
#endif
    if (fontarea[f] == OTGR_FONT_FLAG) {
        return D2Fix(getGlyphItalCorr((XeTeXLayoutEngine)(fontlayoutengine[f]), gid));
    }

    return 0;   /* can't actually happen */
}

void
measure_native_glyph(void* pNode, int use_glyph_metrics)
{
    memoryword* node = (memoryword*) pNode;
    uint16_t gid = native_glyph(node);
    unsigned f = native_font(node);

    float ht = 0.0;
    float dp = 0.0;

#ifdef XETEX_MAC
    if (fontarea[f] == AAT_FONT_FLAG) {
        CFDictionaryRef attributes = (CFDictionaryRef)(fontlayoutengine[f]);
        node_width(node) = D2Fix(GetGlyphWidth_AAT(attributes, gid));
        if (use_glyph_metrics)
            GetGlyphHeightDepth_AAT(attributes, gid, &ht, &dp);
    } else
#endif
    if (fontarea[f] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[f];
        XeTeXFont fontInst = getFont(engine);
        node_width(node) = D2Fix(getGlyphWidth(fontInst, gid));
        if (use_glyph_metrics)
            getGlyphHeightDepth(engine, gid, &ht, &dp);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `measure_native_glyph'\n");
        exit(3);
    }

    if (use_glyph_metrics) {
        node_height(node) = D2Fix(ht);
        node_depth(node) = D2Fix(dp);
    } else {
        node_height(node) = heightbase[f];
        node_depth(node) = depthbase[f];
    }
}

integer
mapchartoglyph(integer font, integer ch)
{
    if (ch > 0x10ffff || ((ch >= 0xd800) && (ch <= 0xdfff)))
        return 0;
#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG)
        return MapCharToGlyph_AAT((CFDictionaryRef)(fontlayoutengine[font]), ch);
    else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG) {
        integer res = mapCharToGlyph((XeTeXLayoutEngine)(fontlayoutengine[font]), ch);
        // store_charglyph_map(font, res, ch);
        // printf("Mapping glyph %d  c %d\n", res, ch);
        return res;
    }
    else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `map_char_to_glyph'\n");
        exit(3);
    }
}

integer
mapglyphtoindex(integer font)
    /* glyph name is at nameoffile+1 */
{
#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG)
        return MapGlyphToIndex_AAT((CFDictionaryRef)(fontlayoutengine[font]), (const char*)nameoffile + 1);
    else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG)
        return mapGlyphToIndex((XeTeXLayoutEngine)(fontlayoutengine[font]), (const char*)nameoffile + 1);
    else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `map_glyph_to_index'\n");
        exit(3);
    }
}

integer
getfontcharrange(integer font, int first)
{
#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG)
        return GetFontCharRange_AAT((CFDictionaryRef)(fontlayoutengine[font]), first);
    else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG)
        return getFontCharRange((XeTeXLayoutEngine)(fontlayoutengine[font]), first);
    else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `get_font_char_range'\n");
        exit(3);
    }
}

Fixed D2Fix(double d)
{
    Fixed rval = (int)(d * 65536.0 + 0.5);
    return rval;
}

double Fix2D(Fixed f)
{
    double rval = f / 65536.0;
    return rval;
}

/* these are here, not XeTeX_mac.c, because we need stubs on other platforms */
void
aatgetfontmetrics(CFDictionaryRef attributes, integer* ascent, integer* descent, integer* xheight, integer* capheight, integer* slant)
{
#ifdef XETEX_MAC
    CTFontRef font = fontFromAttributes(attributes);

    *ascent = D2Fix(CTFontGetAscent(font));
    *descent = D2Fix(CTFontGetDescent(font));
    *xheight = D2Fix(CTFontGetXHeight(font));
    *capheight = D2Fix(CTFontGetCapHeight(font));
    *slant = D2Fix(tan(-CTFontGetSlantAngle(font) * M_PI / 180.0));
#endif
}

int
aatfontget(int what, CFDictionaryRef attributes)
{
    int rval = -1;

#ifdef XETEX_MAC
    CTFontRef font = fontFromAttributes(attributes);
    CFArrayRef list;

    switch (what) {
        case XeTeX_count_glyphs:
            rval = CTFontGetGlyphCount(font);
            break;

        case XeTeX_count_features:
            list = CTFontCopyFeatures(font);
            if (list) {
                rval = CFArrayGetCount(list);
                CFRelease(list);
            }
            break;
    }
#endif
    return rval;
}

int
aatfontget1(int what, CFDictionaryRef attributes, int param)
{
    int rval = -1;

#ifdef XETEX_MAC
    CTFontRef font = fontFromAttributes(attributes);

    switch (what) {
        case XeTeX_feature_code:
        {
            CFArrayRef features = CTFontCopyFeatures(font);
            if (features) {
                if (CFArrayGetCount(features) > param) {
                    CFDictionaryRef feature = CFArrayGetValueAtIndex(features, param);
                    CFNumberRef identifier = CFDictionaryGetValue(feature, kCTFontFeatureTypeIdentifierKey);
                    if (identifier)
                        CFNumberGetValue(identifier, kCFNumberIntType, &rval);
                }
                CFRelease(features);
            }
            break;
        }

        case XeTeX_is_exclusive_feature:
        {
            CFArrayRef features = CTFontCopyFeatures(font);
            if (features) {
                CFBooleanRef value;
                CFDictionaryRef feature = findDictionaryInArrayWithIdentifier(features, kCTFontFeatureTypeIdentifierKey, param);
                Boolean found = CFDictionaryGetValueIfPresent(feature, kCTFontFeatureTypeExclusiveKey, (const void **)&value);
                if (found)
                    rval = CFBooleanGetValue(value);
                CFRelease(features);
            }
            break;
        }

        case XeTeX_count_selectors:
        {
            CFArrayRef features = CTFontCopyFeatures(font);
            if (features) {
                CFDictionaryRef feature = findDictionaryInArrayWithIdentifier(features, kCTFontFeatureTypeIdentifierKey, param);
                if (feature) {
                    CFArrayRef selectors = CFDictionaryGetValue(feature, kCTFontFeatureTypeSelectorsKey);
                    if (selectors)
                        rval = CFArrayGetCount(selectors);
                }
                CFRelease(features);
            }
            break;
        }
    }
#endif

    return rval;
}

int
aatfontget2(int what, CFDictionaryRef attributes, int param1, int param2)
{
    int rval = -1;

#ifdef XETEX_MAC
    CTFontRef font = fontFromAttributes(attributes);
    CFArrayRef features = CTFontCopyFeatures(font);
    if (features) {
        CFDictionaryRef feature = findDictionaryInArrayWithIdentifier(features, kCTFontFeatureTypeIdentifierKey, param1);
        if (feature) {
            CFArrayRef selectors = CFDictionaryGetValue(feature, kCTFontFeatureTypeSelectorsKey);
            if (selectors) {
                CFDictionaryRef selector;
                switch (what) {
                    case XeTeX_selector_code:
                        if (CFArrayGetCount(selectors) > param2) {
                            CFNumberRef identifier;
                            selector = CFArrayGetValueAtIndex(selectors, param2);
                            identifier = CFDictionaryGetValue(selector, kCTFontFeatureSelectorIdentifierKey);
                            if (identifier)
                                CFNumberGetValue(identifier, kCFNumberIntType, &rval);
                        }
                        break;
                    case XeTeX_is_default_selector:
                        selector = findDictionaryInArrayWithIdentifier(selectors, kCTFontFeatureSelectorIdentifierKey, param2);
                        if (selector) {
                            CFBooleanRef isDefault;
                            Boolean found = CFDictionaryGetValueIfPresent(selector, kCTFontFeatureSelectorDefaultKey, (const void **)&isDefault);
                            if (found)
                                rval = CFBooleanGetValue(isDefault);
                        }
                        break;
                }
            }
        }
        CFRelease(features);
    }
#endif

    return rval;
}

int
aatfontgetnamed(int what, CFDictionaryRef attributes)
{
    int rval = -1;

#ifdef XETEX_MAC
    if (what == XeTeX_find_feature_by_name)
        {
            CTFontRef font = fontFromAttributes(attributes);
            CFArrayRef features = CTFontCopyFeatures(font);
            if (features) {
                CFDictionaryRef feature = findDictionaryInArray(features, kCTFontFeatureTypeNameKey,
                                                                (const char*)nameoffile + 1, namelength);
                if (feature) {
                    CFNumberRef identifier = CFDictionaryGetValue(feature, kCTFontFeatureTypeIdentifierKey);
                    CFNumberGetValue(identifier, kCFNumberIntType, &rval);
                }
                CFRelease(features);
            }
        }
#endif

    return rval;
}

int
aatfontgetnamed1(int what, CFDictionaryRef attributes, int param)
{
    int rval = -1;

#ifdef XETEX_MAC
    CTFontRef font = fontFromAttributes(attributes);

    if (what == XeTeX_find_selector_by_name) {
        CFArrayRef features = CTFontCopyFeatures(font);
        if (features) {
            CFDictionaryRef feature = findDictionaryInArrayWithIdentifier(features, kCTFontFeatureTypeIdentifierKey, param);
            if (feature) {
                CFNumberRef selector = findSelectorByName(feature, (const char*)nameoffile + 1, namelength);
                if (selector)
                    CFNumberGetValue(selector, kCFNumberIntType, &rval);
            }
            CFRelease(features);
        }
    }
#endif

    return rval;
}

void
aatprintfontname(int what, CFDictionaryRef attributes, int param1, int param2)
{
#ifdef XETEX_MAC
    CFStringRef name = NULL;
    if (what == XeTeX_feature_name || what == XeTeX_selector_name) {
        CTFontRef font = fontFromAttributes(attributes);
        CFArrayRef features = CTFontCopyFeatures(font);
        if (features) {
            CFDictionaryRef feature = findDictionaryInArrayWithIdentifier(features,
                                                                          kCTFontFeatureTypeIdentifierKey,
                                                                          param1);
            if (feature) {
                if (what == XeTeX_feature_name)
                    name = CFDictionaryGetValue(feature, kCTFontFeatureTypeNameKey);
                else {
                    CFArrayRef selectors = CFDictionaryGetValue(feature, kCTFontFeatureTypeSelectorsKey);
                    CFDictionaryRef selector = findDictionaryInArrayWithIdentifier(selectors,
                                                                                   kCTFontFeatureSelectorIdentifierKey,
                                                                                   param2);
                    if (selector)
                        name = CFDictionaryGetValue(selector, kCTFontFeatureSelectorNameKey);
                }
            }
            CFRelease(features);
        }
    }

    if (name) {
        CFIndex len = CFStringGetLength(name);
        UniChar* buf = xcalloc(len, sizeof(UniChar));
        CFStringGetCharacters(name, CFRangeMake(0, len), buf);
        printchars(buf, len);
        free(buf);
    }
#endif
}

void
printglyphname(integer font, integer gid)
{
    const char* s;
    int len = 0;
#ifdef XETEX_MAC
    if (fontarea[font] == AAT_FONT_FLAG) {
        s = GetGlyphNameFromCTFont(fontFromInteger(font), gid, &len);
    } else
#endif
    if (fontarea[font] == OTGR_FONT_FLAG) {
        XeTeXLayoutEngine engine = (XeTeXLayoutEngine)fontlayoutengine[font];
        s = getGlyphName(getFont(engine), gid, &len);
    } else {
        fprintf(stderr, "\n! Internal error: bad native font flag in `print_glyph_name'\n");
        exit(3);
    }
    while (len-- > 0)
        printchar(*s++);
}

int
u_open_in(unicodefile* f, integer filefmt, const_string fopen_mode, integer mode, integer encodingData)
{
    boolean rval;
    *f = (unicodefile) xmalloc(sizeof(UFILE));
    (*f)->encodingMode = 0;
    (*f)->conversionData = 0;
    (*f)->savedChar = -1;
    (*f)->skipNextLF = 0;
    rval = open_input (&((*f)->f), filefmt, fopen_mode);
    if (rval) {
        int B1, B2;
        if (mode == AUTO) {
            /* sniff encoding form */
            B1 = GETC((*f)->f);
            B2 = GETC((*f)->f);
            if (B1 == 0xfe && B2 == 0xff)
                mode = UTF16BE;
            else if (B2 == 0xfe && B1 == 0xff)
                mode = UTF16LE;
            else if (B1 == 0 && B2 != 0) {
                mode = UTF16BE;
                rewind((*f)->f);
            } else if (B2 == 0 && B1 != 0) {
                mode = UTF16LE;
                rewind((*f)->f);
            } else if (B1 == 0xef && B2 == 0xbb) {
                int B3 = GETC((*f)->f);
                if (B3 == 0xbf)
                    mode = UTF8;
            }
            if (mode == AUTO) {
                rewind((*f)->f);
                mode = UTF8;
            }
        }

        setinputfileencoding(*f, mode, encodingData);
    }
    return rval;
}

void
u_close_inout(unicodefile* f)
{
    if (f != 0) {
        fclose((*f)->f);
        if (((*f)->encodingMode == ICUMAPPING) && ((*f)->conversionData != NULL)) {
#ifndef WEBASSEMBLY_BUILD
            ucnv_close((*f)->conversionData);
#endif
        }
        free(*f);
    }
}

#if defined(WIN32)
static int
Isspace(char c)
{
    return (c == ' ' || c == '\t');
}
#endif

int
open_dvi_output(FILE** fptr)
{
    // if (nopdfoutput) {
        return open_output(fptr, FOPEN_WBIN_MODE);
//     } else {
//         const char *p = (const char*)nameoffile+1;
//         char    *cmd, *q, *bindir = NULL;
//         int len = strlen(p);
//         while (*p)
//             if (*p++ == '\"')
//                 ++len;
//         len += strlen(outputdriver);
// #ifndef WIN32
//         if (!kpse_absolute_p(outputdriver, true))
//             bindir = kpse_var_value("SELFAUTOLOC");
//         if (bindir)
//             len += strlen(bindir) + 1;
// #endif
//         if (output_directory)
//             len += strlen(output_directory);
//         len += 10; /* space for -o flag, quotes, NUL */
//         for (p = (const char*)nameoffile+1; *p; p++)
//             if (*p == '\"')
//                 ++len;  /* allow extra space to escape quotes in filename */
//         cmd = xmalloc(len);
// #ifdef WIN32
//         strcpy(cmd, outputdriver);
// #else
//         if (bindir) {
//             strcpy(cmd, bindir);
//             strcat(cmd, "/");
//             strcat(cmd, outputdriver);
//         } else {
//             strcpy(cmd, outputdriver);
//         }
// #endif
//         strcat(cmd, " -o \"");
//         if (output_directory) {
//             len = strlen(output_directory);
//             if (IS_DIR_SEP(output_directory[len-1]))
//                 output_directory[len-1] = '\0';
//             strcat(cmd, output_directory);
//             strcat(cmd, "/");
//         }
//         q = cmd + strlen(cmd);
//         for (p = (const char*)nameoffile+1; *p; p++) {
//             if (*p == '\"')
//                 *q++ = '\\';
//             *q++ = *p;
//         }
//         *q++ = '\"';
//         *q = '\0';
//         if (papersize != 0) {
//             char* cmd2 = concat3(cmd, " -p ", papersize);
//             free(cmd);
//             cmd = cmd2;
//         }
//         if (output_directory) {
//             char *fullname = concat3(output_directory, "/", (const char*)nameoffile+1);
//             free(nameoffile);
//             namelength = strlen(fullname);
//             nameoffile = (unsigned char*) xmalloc(namelength + 2);
//             strcpy((char*)nameoffile+1, fullname);
//             free(fullname);
//         }
// #if defined(WIN32)
//         {
//             wchar_t *tmp1w;
//             char *p, *pp, *fullcmd, *prgnam;
//             bindir = kpse_var_value("SELFAUTOLOC");
//             for(pp = bindir; *pp; pp++) {
//                 if(*pp == '/') *pp = '\\';
//             }
//             pp = cmd;
//             while(Isspace(*pp))
//                 pp++;
//             prgnam = xmalloc(strlen(cmd));
//             p = prgnam;
//             while(!Isspace(*pp)) {
//                 *p++ = *pp++;
//             }
//             *p = '\0';
//             fullcmd = concatn("\"\"", bindir, "\\", prgnam, "\"", pp, "\"", NULL);
//             tmp1w = get_wstring_from_mbstring(CP_UTF8, (const char *)fullcmd, tmp1w=NULL);
//             *fptr = _wpopen(tmp1w, L"wb");
//             free(bindir);
//             free(prgnam);
//             free(fullcmd);
//             free(tmp1w);
//         }
// #else
//         *fptr = popen(cmd, "w");
// #endif
//         free(cmd);
//         return (*fptr != 0);
//     }
}

int
dviclose(FILE* fptr)
{
    if (fclose(fptr) != 0)
            return errno;
    // if (nopdfoutput) {
    //     if (fclose(fptr) != 0)
    //         return errno;
    // } else {
    //     return pclose(fptr);
    // }
    return 0;
}

int
get_uni_c(UFILE* f)
{
    int rval;
    int c;
#ifdef WIN32
    HANDLE hStdin;
    DWORD ret;
    wint_t wc[1];
#endif

    if (f->savedChar != -1) {
        rval = f->savedChar;
        f->savedChar = -1;
        return rval;
    }

    switch (f->encodingMode) {
        case UTF8:
            c = rval = GETC(f->f);
            if (rval != EOF) {
                uint16_t extraBytes = bytesFromUTF8[rval];
                switch (extraBytes) {   /* note: code falls through cases! */
                    case 3: c = GETC(f->f);
                        if (c < 0x80 || c >= 0xc0) goto bad_utf8;
                        rval <<= 6; rval += c;
                    case 2: c = GETC(f->f);
                        if (c < 0x80 || c >= 0xc0) goto bad_utf8;
                        rval <<= 6; rval += c;
                    case 1: c = GETC(f->f);
                        if (c < 0x80 || c >= 0xc0) goto bad_utf8;
                        rval <<= 6; rval += c;
                    case 0:
                        break;

                    bad_utf8:
                        if (c != EOF)
                            UNGETC(c, f->f);
                    case 5:
                    case 4:
                        badutf8warning();
                        return 0xfffd;      /* return without adjusting by offsetsFromUTF8 */
                };
                rval -= offsetsFromUTF8[extraBytes];
                if (rval < 0 || rval > 0x10ffff) {
                    badutf8warning();
                    return 0xfffd;
                }
            }
            break;

        case UTF16BE:
            rval = GETC(f->f);
            if (rval != EOF) {
                rval <<= 8;
                rval += GETC(f->f);
                if (rval >= 0xd800 && rval <= 0xdbff) {
                    int lo = GETC(f->f);
                    lo <<= 8;
                    lo += GETC(f->f);
                    if (lo >= 0xdc00 && lo <= 0xdfff)
                        rval = 0x10000 + (rval - 0xd800) * 0x400 + (lo - 0xdc00);
                    else {
                        rval = 0xfffd;
                        f->savedChar = lo;
                    }
                } else if (rval >= 0xdc00 && rval <= 0xdfff)
                    rval = 0xfffd;
            }
            break;

        case UTF16LE:
            rval = GETC(f->f);
            if (rval != EOF) {
                rval += (GETC(f->f) << 8);
                if (rval >= 0xd800 && rval <= 0xdbff) {
                    int lo = GETC(f->f);
                    lo += (GETC(f->f) << 8);
                    if (lo >= 0xdc00 && lo <= 0xdfff)
                        rval = 0x10000 + (rval - 0xd800) * 0x400 + (lo - 0xdc00);
                    else {
                        rval = 0xfffd;
                        f->savedChar = lo;
                    }
                } else if (rval >= 0xdc00 && rval <= 0xdfff)
                    rval = 0xfffd;
            }
            break;

#ifdef WIN32
        case WIN32CONSOLE:
            hStdin = GetStdHandle(STD_INPUT_HANDLE);
            if (ReadConsoleW(hStdin, wc, 1, &ret, NULL) == 0) {
                rval = EOF;
                break;
            }
            rval = wc[0];
            if (rval >= 0xd800 && rval <= 0xdbff) {
                int lo;
                if (ReadConsoleW(hStdin, wc, 1, &ret, NULL) == 0) {
                    rval = EOF;
                    break;
                }
                lo = wc[0];
                if (lo >= 0xdc00 && lo <= 0xdfff)
                    rval = 0x10000 + (rval - 0xd800) * 0x400 + (lo - 0xdc00);
                else {
                    rval = 0xfffd;
                    f->savedChar = lo;
                }
            } else if (rval >= 0xdc00 && rval <= 0xdfff)
                rval = 0xfffd;
            break;
#endif

        case RAW:
            rval = GETC(f->f);
            break;

        default:
            /* this can't happen */
            fprintf(stderr, "! Internal error---file input mode=%d.\n", f->encodingMode);
            uexit(3);
    }

    return rval;
}

void
makeutf16name(void)
{
    unsigned char* s = nameoffile + 1;
    uint32_t rval;
    uint16_t* t;
    static int name16len = 0;
    if (name16len <= namelength) {
        if (nameoffile16 != 0)
            free(nameoffile16);
        name16len = namelength + 10;
        nameoffile16 = (uint16_t*) xcalloc(name16len, sizeof(uint16_t));
    }
    t = nameoffile16;
    while (s <= nameoffile + namelength) {
        uint16_t extraBytes;
        rval = *(s++);
        extraBytes = bytesFromUTF8[rval];
        switch (extraBytes) {   /* note: code falls through cases! */
            case 5: rval <<= 6; if (*s) rval += *(s++);
            case 4: rval <<= 6; if (*s) rval += *(s++);
            case 3: rval <<= 6; if (*s) rval += *(s++);
            case 2: rval <<= 6; if (*s) rval += *(s++);
            case 1: rval <<= 6; if (*s) rval += *(s++);
            case 0: ;
        };
        rval -= offsetsFromUTF8[extraBytes];
        if (rval > 0xffff) {
            rval -= 0x10000;
            *(t++) = 0xd800 + rval / 0x0400;
            *(t++) = 0xdc00 + rval % 0x0400;
        } else {
            *(t++) = rval;
        }
    }
    namelength16 = t - nameoffile16;
}


integer get_native_word_cp(void* pNode, int side)
{
    memoryword* node = (memoryword*)pNode;
    FixedPoint* locations = (FixedPoint*)native_glyph_info_ptr(node);
    uint16_t* glyphIDs = (uint16_t*)(locations + native_glyph_count(node));
    uint16_t glyphCount = native_glyph_count(node);
    integer f = native_font(node);
    uint16_t actual_glyph;

    if (glyphCount == 0)
        return 0;

    switch (side) {
    case LEFT_SIDE:
        actual_glyph = *glyphIDs;
        break;
    case RIGHT_SIDE:
        actual_glyph = glyphIDs[glyphCount - 1];
        break;
    default:
        assert(0); // we should not reach this point
    }
    return get_cp_code(f, actual_glyph, side);
}

