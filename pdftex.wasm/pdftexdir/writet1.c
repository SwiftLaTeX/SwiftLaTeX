/*
Copyright 1996-2018 Han The Thanh <thanh@pdftex.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see  <http://www.gnu.org/licenses/>.  */

#include "ptexlib.h"
#include <stdarg.h>
#include <string.h>

#define t1_log(str)      tex_printf("%s",str)
#define get_length1()    t1_length1 = t1_offset() - t1_save_offset
#define get_length2()    t1_length2 = t1_offset() - t1_save_offset
#define get_length3()    t1_length3 = fixedcontent? t1_offset() - t1_save_offset : 0
#define save_offset()    t1_save_offset = t1_offset()

#define t1_open()        open_input(&t1_file, kpse_type1_format, FOPEN_RBIN_MODE)
#define t1_close()       xfclose(t1_file, cur_file_name)
#define t1_getchar()     getc(t1_file)
#define t1_putchar       fb_putchar
#define t1_offset        fb_offset
#define t1_ungetchar(c)  ungetc(c, t1_file)
#define t1_eof()         feof(t1_file)

#define t1_prefix(s)     str_prefix(t1_line_array, s)
#define t1_buf_prefix(s) str_prefix(t1_buf_array, s)
#define t1_suffix(s)     str_suffix(t1_line_array, t1_line_ptr, s)
#define t1_buf_suffix(s) str_suffix(t1_buf_array, t1_buf_ptr, s)
#define t1_charstrings() strstr(t1_line_array, charstringname)
#define t1_subrs()       t1_prefix("/Subrs")
#define t1_end_eexec()   t1_suffix("mark currentfile closefile")
#define t1_cleartomark() t1_prefix("cleartomark")

#define enc_open()       open_input(&enc_file, kpse_enc_format, FOPEN_RBIN_MODE)
#define enc_close()      xfclose(enc_file, cur_file_name)
#define enc_getchar()    getc(enc_file)
#define enc_eof()        feof(enc_file)

#define valid_code(c)    (c >= 0 && c < 256)
#define fixedcontent     false /* false for pdfTeX, true for dvips */

static const char *standard_glyph_names[256] = {
    /* 0x00 */
    notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    /* 0x10 */
    notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    /* 0x20 */
    "space", "exclam", "quotedbl", "numbersign", "dollar", "percent",
    "ampersand", "quoteright", "parenleft", "parenright", "asterisk",
    "plus", "comma", "hyphen", "period", "slash",
    /* 0x30 */
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight",
    "nine", "colon", "semicolon", "less", "equal", "greater", "question",
    /* 0x40 */
    "at", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
    "O",
    /* 0x50 */
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "bracketleft",
    "backslash", "bracketright", "asciicircum", "underscore",
    /* 0x60 */
    "quoteleft", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
    "m", "n", "o",
    /* 0x70 */
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "braceleft", "bar",
    "braceright", "asciitilde", notdef,
    /* 0x80 */
    notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    /* 0x90 */
    notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    /* 0xa0 */
    notdef, "exclamdown", "cent", "sterling", "fraction", "yen", "florin",
    "section", "currency", "quotesingle", "quotedblleft", "guillemotleft",
    "guilsinglleft", "guilsinglright", "fi", "fl",
    /* 0xb0 */
    notdef, "endash", "dagger", "daggerdbl", "periodcentered", notdef,
    "paragraph", "bullet", "quotesinglbase", "quotedblbase",
    "quotedblright", "guillemotright", "ellipsis", "perthousand", notdef,
    "questiondown",
    /* 0xc0 */
    notdef, "grave", "acute", "circumflex", "tilde", "macron", "breve",
    "dotaccent", "dieresis", notdef,
    "ring", "cedilla", notdef, "hungarumlaut", "ogonek", "caron",
    /* 0xd0 */
    "emdash", notdef, notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    notdef, notdef, notdef, notdef, notdef, notdef, notdef,
    /* 0xe0 */
    notdef, "AE", notdef, "ordfeminine", notdef, notdef, notdef, notdef,
    "Lslash", "Oslash", "OE", "ordmasculine", notdef, notdef, notdef,
    notdef,
    /* 0xf0 */
    notdef, "ae", notdef, notdef, notdef, "dotlessi", notdef, notdef, "lslash",
    "oslash", "oe", "germandbls", notdef, notdef, notdef, notdef
};

integer t1_length1, t1_length2, t1_length3;
static integer t1_save_offset;
static integer t1_fontname_offset;
static fd_entry *fd_cur;

static char charstringname[] = "/CharStrings";

enum { ENC_STANDARD, ENC_BUILTIN } t1_encoding;

#define T1_BUF_SIZE      0x10
#define ENC_BUF_SIZE     0x1000

#define CS_HSTEM         1
#define CS_VSTEM         3
#define CS_VMOVETO       4
#define CS_RLINETO       5
#define CS_HLINETO       6
#define CS_VLINETO       7
#define CS_RRCURVETO     8
#define CS_CLOSEPATH     9
#define CS_CALLSUBR      10
#define CS_RETURN        11
#define CS_ESCAPE        12
#define CS_HSBW          13
#define CS_ENDCHAR       14
#define CS_RMOVETO       21
#define CS_HMOVETO       22
#define CS_VHCURVETO     30
#define CS_HVCURVETO     31
#define CS_1BYTE_MAX     (CS_HVCURVETO + 1)

#define CS_DOTSECTION    CS_1BYTE_MAX + 0
#define CS_VSTEM3        CS_1BYTE_MAX + 1
#define CS_HSTEM3        CS_1BYTE_MAX + 2
#define CS_SEAC          CS_1BYTE_MAX + 6
#define CS_SBW           CS_1BYTE_MAX + 7
#define CS_DIV           CS_1BYTE_MAX + 12
#define CS_CALLOTHERSUBR CS_1BYTE_MAX + 16
#define CS_POP           CS_1BYTE_MAX + 17
#define CS_SETCURRENTPOINT CS_1BYTE_MAX + 33
#define CS_2BYTE_MAX     (CS_SETCURRENTPOINT + 1)
#define CS_MAX           CS_2BYTE_MAX

typedef unsigned char byte;

typedef struct {
    byte nargs;                 /* number of arguments */
    boolean bottom;             /* take arguments from bottom of stack? */
    boolean clear;              /* clear stack? */
    boolean valid;
} cc_entry;                     /* CharString Command */

typedef struct {
    char *name;                 /* glyph name (or notdef for Subrs entry) */
    byte *data;
    unsigned short len;         /* length of the whole string */
    unsigned short cslen;       /* length of the encoded part of the string */
    boolean used;
    boolean valid;
} cs_entry;

static unsigned short t1_dr, t1_er;
static const unsigned short t1_c1 = 52845, t1_c2 = 22719;
static unsigned short t1_cslen;
static short t1_lenIV;
static char enc_line[ENC_BUF_SIZE];

/* define t1_line_ptr, t1_line_array & t1_line_limit */
typedef char t1_line_entry;
define_array(t1_line);

/* define t1_buf_ptr, t1_buf_array & t1_buf_limit */
typedef char t1_buf_entry;
define_array(t1_buf);

static int cs_start;

static cs_entry *cs_tab, *cs_ptr, *cs_notdef;
static char *cs_dict_start, *cs_dict_end;
static int cs_count, cs_size, cs_size_pos;

static cs_entry *subr_tab;
static char *subr_array_start, *subr_array_end;
static int subr_max, subr_size, subr_size_pos;

/* This list contains the begin/end tokens commonly used in the */
/* /Subrs array of a Type 1 font.                               */

static const char *cs_token_pairs_list[][2] = {
    {" RD", "NP"},
    {" -|", "|"},
    {" RD", "noaccess put"},
    {" -|", "noaccess put"},
    {NULL, NULL}
};
static const char **cs_token_pair;

static boolean t1_pfa, t1_cs, t1_scan, t1_eexec_encrypt, t1_synthetic;
static int t1_in_eexec;         /* 0 before eexec-encrypted, 1 during, 2 after */
static long t1_block_length;
static int last_hexbyte;
static FILE *t1_file;
static FILE *enc_file;

static void enc_getline(void)
{
    char *p;
    int c;
  restart:
    if (enc_eof())
        pdftex_fail("unexpected end of file");
    p = enc_line;
    do {
        c = enc_getchar();
        append_char_to_buf(c, p, enc_line, ENC_BUF_SIZE);
    } while (c != 10);
    append_eol(p, enc_line, ENC_BUF_SIZE);
    if (p - enc_line < 2 || *enc_line == '%')
        goto restart;
}

/* read encoding from .enc file, return glyph_names array, or pdffail() */

char **load_enc_file(char *enc_name)
{
    char buf[ENC_BUF_SIZE], *p, *r;
    int i, names_count;
    char **glyph_names;
    set_cur_file_name(enc_name);
    if (!enc_open()) {
        pdftex_fail("cannot open encoding file for reading");
    }
    glyph_names = xtalloc(256, char *);
    for (i = 0; i < 256; i++)
        glyph_names[i] = notdef;
    t1_log("{");
    t1_log(cur_file_name = (char *) nameoffile + 1);
    enc_getline();
    if (*enc_line != '/' || (r = strchr(enc_line, '[')) == NULL) {
        remove_eol(r, enc_line);
        pdftex_fail
           ("invalid encoding vector (a name or `[' missing): `%s'", enc_line);
    }
    names_count = 0;
    r++;                        /* skip '[' */
    skip(r, ' ');
    for (;;) {
        while (*r == '/') {
            for (p = buf, r++;
                 *r != ' ' && *r != 10 && *r != ']' && *r != '/'; *p++ = *r++);
            *p = 0;
            skip(r, ' ');
            if (names_count > 255)
                pdftex_fail("encoding vector contains more than 256 names");
            if (strcmp(buf, notdef) != 0)
                glyph_names[names_count] = xstrdup(buf);
            names_count++;
        }
        if (*r != 10 && *r != '%') {
            if (str_prefix(r, "] def"))
                goto done;
            else {
                remove_eol(r, enc_line);
                pdftex_fail
       ("invalid encoding vector: a name or `] def' expected: `%s'", enc_line);
            }
        }
        enc_getline();
        r = enc_line;
    }
  done:
    enc_close();
    t1_log("}");
    cur_file_name = NULL;
    return glyph_names;
}

static void t1_check_pfa(void)
{
    const int c = t1_getchar();
    t1_pfa = (c != 128) ? true : false;
    t1_ungetchar(c);
}

static int t1_getbyte(void)
{
    int c = t1_getchar();
    if (t1_pfa)
        return c;
    if (t1_block_length == 0) {
        if (c != 128)
            pdftex_fail("invalid marker");
        c = t1_getchar();
        if (c == 3) {
            while (!t1_eof())
                t1_getchar();
            return EOF;
        }
        t1_block_length = t1_getchar() & 0xff;
        t1_block_length |= (t1_getchar() & 0xff) << 8;
        t1_block_length |= (t1_getchar() & 0xff) << 16;
        t1_block_length |= (t1_getchar() & 0xff) << 24;
        c = t1_getchar();
    }
    t1_block_length--;
    return c;
}

static int hexval(int c)
{
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= '0' && c <= '9')
        return c - '0';
    else
        return -1;
}

static byte edecrypt(byte cipher)
{
    byte plain;
    if (t1_pfa) {
        while (cipher == 10 || cipher == 13)
            cipher = t1_getbyte();
        last_hexbyte = cipher = (hexval(cipher) << 4) + hexval(t1_getbyte());
    }
    plain = (cipher ^ (t1_dr >> 8));
    t1_dr = (cipher + t1_dr) * t1_c1 + t1_c2;
    return plain;
}

static byte cdecrypt(byte cipher, unsigned short *cr)
{
    const byte plain = (cipher ^ (*cr >> 8));
    *cr = (cipher + *cr) * t1_c1 + t1_c2;
    return plain;
}

static byte eencrypt(byte plain)
{
    const byte cipher = (plain ^ (t1_er >> 8));
    t1_er = (cipher + t1_er) * t1_c1 + t1_c2;
    return cipher;
}

static byte cencrypt(byte plain, unsigned short *cr)
{
    const byte cipher = (plain ^ (*cr >> 8));
    *cr = (cipher + *cr) * t1_c1 + t1_c2;
    return cipher;
}

static char *eol(char *s)
{
    char *p = strend(s);
    if (p - s > 1 && p[-1] != 10) {
        *p++ = 10;
        *p = 0;
    }
    return p;
}

static float t1_scan_num(char *p, char **r)
{
    float f;
    skip(p, ' ');
    if (sscanf(p, "%g", &f) != 1) {
        remove_eol(p, t1_line_array);
        pdftex_fail("a number expected: `%s'", t1_line_array);
    }
    if (r != NULL) {
        for (; isdigit((unsigned char)*p) || *p == '.' ||
             *p == 'e' || *p == 'E' || *p == '+' || *p == '-'; p++);
        *r = p;
    }
    return f;
}

static boolean str_suffix(const char *begin_buf, const char *end_buf,
                          const char *s)
{
    const char *s1 = end_buf - 1, *s2 = strend(s) - 1;
    if (*s1 == 10)
        s1--;
    while (s1 >= begin_buf && s2 >= s) {
        if (*s1-- != *s2--)
            return false;
    }
    return s2 < s;
}

static void t1_getline(void)
{
    int c, l, eexec_scan;
    char *p;
    static const char eexec_str[] = "currentfile eexec";
    static int eexec_len = 17;  /* strlen(eexec_str) */
  restart:
    if (t1_eof())
        pdftex_fail("unexpected end of file");
    t1_line_ptr = t1_line_array;
    alloc_array(t1_line, 1, T1_BUF_SIZE);
    t1_cslen = 0;
    eexec_scan = 0;
    c = t1_getbyte();
    if (c == EOF)
        goto exit;
    while (!t1_eof()) {
        if (t1_in_eexec == 1)
            c = edecrypt((byte)c);
        alloc_array(t1_line, 1, T1_BUF_SIZE);
        append_char_to_buf(c, t1_line_ptr, t1_line_array, t1_line_limit);
        if (t1_in_eexec == 0 && eexec_scan >= 0 && eexec_scan < eexec_len) {
            if (t1_line_array[eexec_scan] == eexec_str[eexec_scan])
                eexec_scan++;
            else
                eexec_scan = -1;
        }
        if (c == 10 || (t1_pfa && eexec_scan == eexec_len && c == 32))
            break;
        if (t1_cs && t1_cslen == 0 && (t1_line_ptr - t1_line_array > 4) &&
            (t1_suffix(" RD ") || t1_suffix(" -| "))) {
            p = t1_line_ptr - 5;
            while (*p != ' ')
                p--;
            t1_cslen = l = t1_scan_num(p + 1, 0);
            cs_start = t1_line_ptr - t1_line_array;     /* cs_start is an index now */
            alloc_array(t1_line, l, T1_BUF_SIZE);
            while (l-- > 0)
                *t1_line_ptr++ = edecrypt((byte)t1_getbyte());
        }
        c = t1_getbyte();
    }
    alloc_array(t1_line, 2, T1_BUF_SIZE);       /* append_eol can append 2 chars */
    append_eol(t1_line_ptr, t1_line_array, t1_line_limit);
    if (t1_line_ptr - t1_line_array < 2)
        goto restart;
    if (eexec_scan == eexec_len)
        t1_in_eexec = 1;
  exit:
    /* ensure that t1_buf_array has as much room as t1_line_array */
    t1_buf_ptr = t1_buf_array;
    alloc_array(t1_buf, t1_line_limit, t1_line_limit);
}

static void t1_putline(void)
{
    char *p = t1_line_array;
    if (t1_line_ptr - t1_line_array <= 1)
        return;
    if (t1_eexec_encrypt) {
        while (p < t1_line_ptr)
            t1_putchar(eencrypt(*p++));
    } else
        while (p < t1_line_ptr)
            t1_putchar(*p++);
}

static void t1_puts(const char *s)
{
    if (s != t1_line_array)
        strcpy(t1_line_array, s);
    t1_line_ptr = strend(t1_line_array);
    t1_putline();
}

__attribute__ ((format(printf, 1, 2)))
static void t1_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(t1_line_array, fmt, args);
    t1_puts(t1_line_array);
    va_end(args);
}

static void t1_init_params(const char *open_name_prefix)
{
    t1_log(open_name_prefix);
    t1_log(cur_file_name);
    t1_lenIV = 4;
    t1_dr = 55665;
    t1_er = 55665;
    t1_in_eexec = 0;
    t1_cs = false;
    t1_scan = true;
    t1_synthetic = false;
    t1_eexec_encrypt = false;
    t1_block_length = 0;
    t1_check_pfa();
}

static void t1_close_font_file(const char *close_name_suffix)
{
    t1_log(close_name_suffix);
    t1_close();
    cur_file_name = NULL;
}

static void t1_check_block_len(boolean decrypt)
{
    int l, c;
    if (t1_block_length == 0)
        return;
    c = t1_getbyte();
    if (decrypt)
        c = edecrypt((byte)c);
    l = t1_block_length;
    if (!(l == 0 && (c == 10 || c == 13))) {
        pdftex_fail("%i bytes more than expected", l + 1);
    }
}

static void t1_start_eexec(void)
{
    int i;
    assert(is_included(fd_cur->fm));
    get_length1();
    save_offset();
    if (!t1_pfa)
        t1_check_block_len(false);
    for (t1_line_ptr = t1_line_array, i = 0; i < 4; i++) {
        edecrypt((byte)t1_getbyte());
        *t1_line_ptr++ = 0;
    }
    t1_eexec_encrypt = true;
    t1_putline();               /* to put the first four bytes */
}

static void t1_stop_eexec(void)
{
    int c;
    assert(is_included(fd_cur->fm));
    get_length2();
    save_offset();
    t1_eexec_encrypt = false;
    if (!t1_pfa)
        t1_check_block_len(true);
    else {
        c = edecrypt((byte)t1_getbyte());
        if (!(c == 10 || c == 13)) {
            if (last_hexbyte == 0)
                t1_puts("00");
            else
                pdftex_fail("unexpected data after eexec");
        }
    }
    t1_cs = false;
    t1_in_eexec = 2;
}

/* macros for various transforms; currently only slant and extend are used */

#define do_xshift(x,a) {x[4]+=a;}
#define do_yshift(x,a) {x[5]+=a;}
#define do_xscale(x,a) {x[0]*=a; x[2]*=a; x[4]*=a;}
#define do_yscale(x,a) {x[1]*=a; x[3]*=a; x[5]*=a;}
#define do_extend(x,a) {do_xscale(x,a);}
#define do_scale(x,a)  {do_xscale(x,a); do_yscale(x,a);}
#define do_slant(x,a)  {x[0]+=x[1]*(a); x[2]+=x[3]*(a); x[4]+=x[5]*(a);}
#define do_shear(x,a)  {x[1]+=x[0]*(a); x[3]+=x[2]*(a); x[5]+=x[4]*(a);}
#define do_rotate(x,a)          \
  {float t, u=cos(a), v=sin(a); \
  t    =x[0]*u+x[1]*-v;         \
  x[1] =x[0]*v+x[1]* u; x[0]=t; \
  t    =x[2]*u+x[3]*-v;         \
  x[3] =x[2]*v+x[3]* u; x[2]=t; \
  t    =x[4]*u+x[5]*-v;         \
  x[5] =x[4]*v+x[5]* u; x[4]=t;}

static void t1_modify_fm(void)
{
    /*
     * font matrix is given as six numbers a0..a5, which stands for the matrix
     *
     *           a0 a1 0
     *     M =   a2 a3 0
     *           a4 a5 1
     *
     * ExtendFont is given as
     *
     *           e 0 0
     *     E =   0 1 0
     *           0 0 1
     *
     * SlantFont is given as
     *
     *           1 0 0
     *     S =   s 1 0
     *           0 0 1
     *
     * The slant transform must be done _before_ the extend transform
     * for compatibility!
     */
    float a[6];
    int i, c;
    char *p, *q, *r;
    if ((p = strchr(t1_line_array, '[')) == 0)
        if ((p = strchr(t1_line_array, '{')) == 0) {
            remove_eol(p, t1_line_array);
            pdftex_fail("FontMatrix: an array expected: `%s'", t1_line_array);
        }
    c = *p++;                   /* save the character '[' resp. '{' */
    strncpy(t1_buf_array, t1_line_array, (size_t) (p - t1_line_array));
    r = t1_buf_array + (p - t1_line_array);
    for (i = 0; i < 6; i++) {
        a[i] = t1_scan_num(p, &q);
        p = q;
    }
    if (fm_slant(fd_cur->fm) != 0)
        do_slant(a, fm_slant(fd_cur->fm) * 1E-3);
    if (fm_extend(fd_cur->fm) != 0)
        do_extend(a, fm_extend(fd_cur->fm) * 1E-3);
    for (i = 0; i < 6; i++) {
        sprintf(r, "%g ", a[i]);
        r = strend(r);
    }
    if (c == '[') {
        while (*p != ']' && *p != 0)
            p++;
    } else {
        while (*p != '}' && *p != 0)
            p++;
    }
    if (*p == 0) {
        remove_eol(p, t1_line_array);
        pdftex_fail
            ("FontMatrix: cannot find the corresponding character to '%c': `%s'",
             c, t1_line_array);
    }
    strcpy(r, p);
    strcpy(t1_line_array, t1_buf_array);
    t1_line_ptr = eol(t1_line_array);
}

static void t1_modify_italic(void)
{
    float a;
    char *p, *r;
    if (fm_slant(fd_cur->fm) == 0)
        return;
    p = strchr(t1_line_array, ' ');
    strncpy(t1_buf_array, t1_line_array, (size_t) (p - t1_line_array + 1));
    a = t1_scan_num(p + 1, &r);
    a -= atan(fm_slant(fd_cur->fm) * 1E-3) * (180 / M_PI);
    sprintf(t1_buf_array + (p - t1_line_array + 1), "%g", a);
    strcpy(strend(t1_buf_array), r);
    strcpy(t1_line_array, t1_buf_array);
    t1_line_ptr = eol(t1_line_array);
    fd_cur->font_dim[ITALIC_ANGLE_CODE].val = round(a);
    fd_cur->font_dim[ITALIC_ANGLE_CODE].set = true;
}

static void t1_scan_keys(void)
{
    int i, k;
    char *p, *q, *r;
    const key_entry *key;
    if (fm_extend(fd_cur->fm) != 0 || fm_slant(fd_cur->fm) != 0) {
        if (t1_prefix("/FontMatrix")) {
            t1_modify_fm();
            return;
        }
        if (t1_prefix("/ItalicAngle")) {
            t1_modify_italic();
            return;
        }
    }
    if (t1_prefix("/FontType")) {
        p = t1_line_array + strlen("FontType") + 1;
        if ((i = t1_scan_num(p, 0)) != 1)
            pdftex_fail("Type%d fonts unsupported by pdfTeX", i);
        return;
    }
    for (key = font_key; key - font_key < FONT_KEYS_NUM; key++) {
        if (key->t1name[0] != '\0' &&
            str_prefix(t1_line_array + 1, key->t1name))
            break;
    }
    if (key - font_key == FONT_KEYS_NUM)
        return;
    p = t1_line_array + strlen(key->t1name) + 1;
    skip(p, ' ');
    if ((k = key - font_key) == FONTNAME_CODE) {
        if (*p != '/') {
            remove_eol(p, t1_line_array);
            pdftex_fail("a name expected: `%s'", t1_line_array);
        }
        r = ++p;                /* skip the slash */
        for (q = t1_buf_array; *p != ' ' && *p != 10; *q++ = *p++);
        *q = 0;
        if (fm_slant(fd_cur->fm) != 0) {
            sprintf(q, "-Slant_%i", (int) fm_slant(fd_cur->fm));
            q = strend(q);
        }
        if (fm_extend(fd_cur->fm) != 0) {
            sprintf(q, "-Extend_%i", (int) fm_extend(fd_cur->fm));
        }
        xfree(fd_cur->fontname);
        fd_cur->fontname = xstrdup(t1_buf_array);
        /* at this moment we cannot call make_subset_tag() yet, as the encoding
         * is not read; thus we mark the offset of the subset tag and write it
         * later */
        if (is_subsetted(fd_cur->fm)) {
            assert(is_included(fd_cur->fm));
            t1_fontname_offset = t1_offset() + (r - t1_line_array);
            strcpy(t1_buf_array, p);
            sprintf(r, "ABCDEF+%s%s", fd_cur->fontname, t1_buf_array);
            t1_line_ptr = eol(r);
        }
        return;
    }
    if ((k == STEMV_CODE || k == FONTBBOX1_CODE)
        && (*p == '[' || *p == '{'))
        p++;
    if (k == FONTBBOX1_CODE) {
        for (i = 0; i < 4; i++, k++) {
            fd_cur->font_dim[k].val = t1_scan_num(p, &r);
            fd_cur->font_dim[k].set = true;
            p = r;
        }
        return;
    }
    fd_cur->font_dim[k].val = t1_scan_num(p, 0);
    fd_cur->font_dim[k].set = true;
}

static void t1_scan_param(void)
{
    static const char *lenIV = "/lenIV";
    if (!t1_scan || *t1_line_array != '/')
        return;
    if (t1_prefix(lenIV)) {
        t1_lenIV = t1_scan_num(t1_line_array + strlen(lenIV), 0);
        if (t1_lenIV < 0)
            pdftex_fail("negative value of lenIV is not supported");
        return;
    }
    t1_scan_keys();
}

static void copy_glyph_names(char **glyph_names, int a, int b)
{
    if (glyph_names[b] != notdef) {
        xfree(glyph_names[b]);
        glyph_names[b] = notdef;
    }
    if (glyph_names[a] != notdef) {
        glyph_names[b] = xstrdup(glyph_names[a]);
    }
}

/* read encoding from Type1 font file, return glyph_names array, or pdffail() */

static char **t1_builtin_enc(void)
{
    int i, a, b, c, counter = 0;
    char *r, *p, **glyph_names;
    /* At this moment "/Encoding" is the prefix of t1_line_array */
    glyph_names = xtalloc(256, char *);
    for (i = 0; i < 256; i++)
        glyph_names[i] = notdef;
    if (t1_suffix("def")) {     /* predefined encoding */
        if (sscanf(t1_line_array + strlen("/Encoding"), "%255s", t1_buf_array) == 1
            && strcmp(t1_buf_array, "StandardEncoding") == 0) {
            t1_encoding = ENC_STANDARD;
            for (i = 0; i < 256; i++) {
                if (standard_glyph_names[i] != notdef)
                    glyph_names[i] = xstrdup(standard_glyph_names[i]);
            }
            return glyph_names;
        }
        pdftex_fail("cannot subset font (unknown predefined encoding `%s')",
                    t1_buf_array);
    }
    /* At this moment "/Encoding" is the prefix of t1_line_array, and the encoding is
     * not a predefined encoding.
     *
     * We have two possible forms of Encoding vector. The first case is
     *
     *     /Encoding [/a /b /c...] readonly def
     *
     * and the second case can look like
     *
     *     /Encoding 256 array 0 1 255 {1 index exch /.notdef put} for
     *     dup 0 /x put
     *     dup 1 /y put
     *     ...
     *     readonly def
     */
    t1_encoding = ENC_BUILTIN;
    if (t1_prefix("/Encoding [") || t1_prefix("/Encoding[")) {  /* the first case */
        r = strchr(t1_line_array, '[') + 1;
        skip(r, ' ');
        for (;;) {
            while (*r == '/') {
                for (p = t1_buf_array, r++;
                     *r != 32 && *r != 10 && *r != ']' && *r != '/';
                     *p++ = *r++);
                *p = 0;
                skip(r, ' ');
                if (counter > 255)
                    pdftex_fail("encoding vector contains more than 256 names");
                if (strcmp(t1_buf_array, notdef) != 0)
                    glyph_names[counter] = xstrdup(t1_buf_array);
                counter++;
            }
            if (*r != 10 && *r != '%') {
                if (str_prefix(r, "] def") || str_prefix(r, "] readonly def"))
                    break;
                else {
                    remove_eol(r, t1_line_array);
                    pdftex_fail
       ("a name or `] def' or `] readonly def' expected: `%s'", t1_line_array);
                }
            }
            t1_getline();
            r = t1_line_array;
        }
    } else {                    /* the second case */
        p = strchr(t1_line_array, 10);
        for (;;) {
            if (*p == 10) {
                t1_getline();
                p = t1_line_array;
            }
            /*
               check for `dup <index> <glyph> put'
             */
            if (sscanf(p, "dup %i%255s put", &i, t1_buf_array) == 2 &&
                *t1_buf_array == '/' && valid_code(i)) {
                if (strcmp(t1_buf_array + 1, notdef) != 0)
                    glyph_names[i] = xstrdup(t1_buf_array + 1);
                p = strstr(p, " put") + strlen(" put");
                skip(p, ' ');
            }
            /*
               check for `dup dup <to> exch <from> get put'
             */
            else if (sscanf(p, "dup dup %i exch %i get put", &b, &a) == 2
                     && valid_code(a) && valid_code(b)) {
                copy_glyph_names(glyph_names, a, b);
                p = strstr(p, " get put") + strlen(" get put");
                skip(p, ' ');
            }
            /*
               check for `dup dup <from> <size> getinterval <to> exch putinterval'
             */
            else if (sscanf(p, "dup dup %i %i getinterval %i exch putinterval",
                            &a, &c, &b) == 3
                     && valid_code(a) && valid_code(b) && valid_code(c)) {
                for (i = 0; i < c; i++)
                    copy_glyph_names(glyph_names, a + i, b + i);
                p = strstr(p, " putinterval") + strlen(" putinterval");
                skip(p, ' ');
            }
            /*
               check for `def' or `readonly def'
             */
            else if ((p == t1_line_array || (p > t1_line_array && p[-1] == ' '))
                     && strcmp(p, "def\n") == 0)
                return glyph_names;
            /*
               skip an unrecognizable word
             */
            else {
                while (*p != ' ' && *p != 10)
                    p++;
                skip(p, ' ');
            }
        }
    }
    return glyph_names;
}

static void t1_check_end(void)
{
    if (t1_eof())
        return;
    t1_getline();
    if (t1_prefix("{restore}"))
        t1_putline();
}

static boolean t1_open_fontfile(const char *open_name_prefix)
{
    ff_entry *ff;
    ff = check_ff_exist(fd_cur->fm->ff_name, is_truetype(fd_cur->fm));
    if (ff->ff_path != NULL) {
        t1_file = xfopen(cur_file_name = ff->ff_path, FOPEN_RBIN_MODE);
        recorder_record_input(ff->ff_path);
    } else {
        set_cur_file_name(fd_cur->fm->ff_name);
        pdftex_fail("cannot open Type 1 font file for reading");
    }
    t1_init_params(open_name_prefix);
    return true;                /* font file found */
}

static void t1_include(void)
{
    do {
        t1_getline();
        t1_scan_param();
        t1_putline();
    } while (t1_in_eexec == 0);
    t1_start_eexec();
    do {
        t1_getline();
        t1_scan_param();
        t1_putline();
    } while (!(t1_charstrings() || t1_subrs()));
    t1_cs = true;
    do {
        t1_getline();
        t1_putline();
    } while (!t1_end_eexec());
    t1_stop_eexec();
    if (fixedcontent) {         /* copy 512 zeros (not needed for PDF) */
        do {
            t1_getline();
            t1_putline();
        } while (!t1_cleartomark());
        t1_check_end();         /* write "{restore}if" if found */
    }
    get_length3();
}

#define check_subr(subr) \
    if (subr >= subr_size || subr < 0) \
        pdftex_fail("Subrs array: entry index out of range (%i)",  subr);

static const char **check_cs_token_pair(void)
{
    const char **p = (const char **) cs_token_pairs_list;
    for (; p[0] != NULL; ++p)
        if (t1_buf_prefix(p[0]) && t1_buf_suffix(p[1]))
            return p;
    return NULL;
}

static void cs_store(boolean is_subr)
{
    char *p;
    cs_entry *ptr;
    int subr;
    for (p = t1_line_array, t1_buf_ptr = t1_buf_array; *p != ' ';
         *t1_buf_ptr++ = *p++);
    *t1_buf_ptr = 0;
    if (is_subr) {
        subr = t1_scan_num(p + 1, 0);
        check_subr(subr);
        ptr = subr_tab + subr;
    } else {
        ptr = cs_ptr++;
        if (cs_ptr - cs_tab > cs_size)
            pdftex_fail
                ("CharStrings dict: more entries than dict size (%i)", cs_size);
        if (strcmp(t1_buf_array + 1, notdef) == 0)      /* skip the slash */
            ptr->name = notdef;
        else
            ptr->name = xstrdup(t1_buf_array + 1);
    }
    /* copy " RD " + cs data to t1_buf_array */
    memcpy(t1_buf_array, t1_line_array + cs_start - 4,
           (unsigned) (t1_cslen + 4));
    /* copy the end of cs data to t1_buf_array */
    for (p = t1_line_array + cs_start + t1_cslen,
           t1_buf_ptr = t1_buf_array + t1_cslen + 4;
         *p != 10; *t1_buf_ptr++ = *p++);
    *t1_buf_ptr++ = 10;
    if (is_subr && cs_token_pair == NULL)
        cs_token_pair = check_cs_token_pair();
    ptr->len = t1_buf_ptr - t1_buf_array;
    ptr->cslen = t1_cslen;
    ptr->data = xtalloc(ptr->len, byte);
    memcpy(ptr->data, t1_buf_array, ptr->len);
    ptr->valid = true;
}

#define store_subr()     cs_store(true)
#define store_cs()       cs_store(false)

#define CC_STACK_SIZE    24

static integer cc_stack[CC_STACK_SIZE], *stack_ptr = cc_stack;
static cc_entry cc_tab[CS_MAX];
static boolean is_cc_init = false;

#define cc_pop(N)                   \
    if (stack_ptr - cc_stack < (N)) \
        stack_error(N);             \
    stack_ptr -= N

#define stack_error(N) {            \
    pdftex_fail("CharString: invalid access (%i) to stack (%i entries)", \
                (int) N, (int)(stack_ptr - cc_stack));                  \
    goto cs_error;                  \
}

/*
static integer cc_get(integer index)
{
    if (index <  0) {
        if (stack_ptr + index < cc_stack )
            stack_error(stack_ptr - cc_stack + index);
        return *(stack_ptr + index);
    }
    else {
        if (cc_stack  + index >= stack_ptr)
            stack_error(index);
        return cc_stack[index];
    }
}
*/

#define cc_get(N)   ((N) < 0 ? *(stack_ptr + (N)) : *(cc_stack + (N)))
#define cc_push(V)  *stack_ptr++ = V
#define cc_clear()  stack_ptr = cc_stack
#define set_cc(N, B, A, C) \
    cc_tab[N].nargs = A;   \
    cc_tab[N].bottom = B;  \
    cc_tab[N].clear = C;   \
    cc_tab[N].valid = true

static void cc_init(void)
{
    int i;
    if (is_cc_init)
        return;
    for (i = 0; i < CS_MAX; i++)
        cc_tab[i].valid = false;
    set_cc(CS_HSTEM, true, 2, true);
    set_cc(CS_VSTEM, true, 2, true);
    set_cc(CS_VMOVETO, true, 1, true);
    set_cc(CS_RLINETO, true, 2, true);
    set_cc(CS_HLINETO, true, 1, true);
    set_cc(CS_VLINETO, true, 1, true);
    set_cc(CS_RRCURVETO, true, 6, true);
    set_cc(CS_CLOSEPATH, false, 0, true);
    set_cc(CS_CALLSUBR, false, 1, false);
    set_cc(CS_RETURN, false, 0, false);
    /*
       set_cc(CS_ESCAPE, false, 0, false);
     */
    set_cc(CS_HSBW, true, 2, true);
    set_cc(CS_ENDCHAR, false, 0, true);
    set_cc(CS_RMOVETO, true, 2, true);
    set_cc(CS_HMOVETO, true, 1, true);
    set_cc(CS_VHCURVETO, true, 4, true);
    set_cc(CS_HVCURVETO, true, 4, true);
    set_cc(CS_DOTSECTION, false, 0, true);
    set_cc(CS_VSTEM3, true, 6, true);
    set_cc(CS_HSTEM3, true, 6, true);
    set_cc(CS_SEAC, true, 5, true);
    set_cc(CS_SBW, true, 4, true);
    set_cc(CS_DIV, false, 2, false);
    set_cc(CS_CALLOTHERSUBR, false, 0, false);
    set_cc(CS_POP, false, 0, false);
    set_cc(CS_SETCURRENTPOINT, true, 2, true);
    is_cc_init = true;
}

#define cs_getchar()     cdecrypt(*data++, &cr)

#define mark_subr(n)     cs_mark(0, n)
#define mark_cs(s)       cs_mark(s, 0)

__attribute__ ((format(printf, 3, 4)))
static void cs_fail(const char *cs_name, int subr, const char *fmt, ...)
{
    char buf[SMALL_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (cs_name == NULL)
        pdftex_fail("Subr (%i): %s", (int) subr, buf);
    else
        pdftex_fail("CharString (/%s): %s", cs_name, buf);
}

/* fix a return-less subr by appending CS_RETURN */
static void append_cs_return(cs_entry *ptr)
{
    unsigned short cr;
    int i;
    byte *p, *q, *data, *new_data;
    assert(ptr != NULL && ptr->valid && ptr->used);

    /* decrypt the cs data to t1_buf_array, append CS_RETURN */
    p = (byte *) t1_buf_array;
    data = ptr->data + 4;
    cr = 4330;
    for (i = 0; i < ptr->cslen; i++)
        *p++ = cs_getchar();
    *p = CS_RETURN;

    /* encrypt the new cs data to new_data */
    new_data = xtalloc(ptr->len + 1, byte);
    memcpy(new_data, ptr->data, 4);
    p = new_data + 4;
    q = (byte *) t1_buf_array;
    cr = 4330;
    for (i = 0; i < ptr->cslen + 1; i++)
        *p++ = cencrypt(*q++, &cr);
    memcpy(p, ptr->data + 4 + ptr->cslen, ptr->len - ptr->cslen - 4);

    /* update *ptr */
    xfree(ptr->data);
    ptr->data = new_data;
    ptr->len++;
    ptr->cslen++;
}

static void cs_mark(const char *cs_name, int subr)
{
    byte *data;
    int i, b, cs_len;
    int last_cmd = 0;
    integer a, a1, a2;
    unsigned short cr;
    static integer lastargOtherSubr3 = 3;       /* the argument of last call to
                                                   OtherSubrs[3] */
    cs_entry *ptr;
    cc_entry *cc;
    if (cs_name == NULL) {
        check_subr(subr);
        ptr = subr_tab + subr;
        if (!ptr->valid)
            return;
    } else {
        if (cs_notdef != NULL &&
            (cs_name == notdef || strcmp(cs_name, notdef) == 0))
            ptr = cs_notdef;
        else {
            for (ptr = cs_tab; ptr < cs_ptr; ptr++)
                if (strcmp(ptr->name, cs_name) == 0)
                    break;
            if (ptr == cs_ptr) {
                pdftex_warn("glyph `%s' undefined", cs_name);
                return;
            }
            if (ptr->name == notdef)
                cs_notdef = ptr;
        }
    }
    /* only marked CharString entries and invalid entries can be skipped;
       valid marked subrs must be parsed to keep the stack in sync */
    if (!ptr->valid || (ptr->used && cs_name != NULL))
        return;
    ptr->used = true;
    cr = 4330;
    cs_len = ptr->cslen;
    data = ptr->data + 4;
    for (i = 0; i < t1_lenIV; i++, cs_len--)
        cs_getchar();
    while (cs_len > 0) {
        --cs_len;
        b = cs_getchar();
        if (b >= 32) {
            if (b <= 246)
                a = b - 139;
            else if (b <= 250) {
                --cs_len;
                a = ((b - 247) << 8) + 108 + cs_getchar();
            } else if (b <= 254) {
                --cs_len;
                a = -((b - 251) << 8) - 108 - cs_getchar();
            } else {
                cs_len -= 4;
                a = (cs_getchar() & 0xff) << 24;
                a |= (cs_getchar() & 0xff) << 16;
                a |= (cs_getchar() & 0xff) << 8;
                a |= (cs_getchar() & 0xff) << 0;
                if (sizeof(integer) > 4 && (a & 0x80000000))
                    a |= ~0x7FFFFFFF;
            }
            cc_push(a);
        } else {
            if (b == CS_ESCAPE) {
                b = cs_getchar() + CS_1BYTE_MAX;
                cs_len--;
            }
            if (b >= CS_MAX) {
                cs_fail(cs_name, subr, "command value out of range: %i",
                        (int) b);
                goto cs_error;
            }
            cc = cc_tab + b;
            if (!cc->valid) {
                cs_fail(cs_name, subr, "command not valid: %i", (int) b);
                goto cs_error;
            }
            if (cc->bottom) {
                if (stack_ptr - cc_stack < cc->nargs)
                    cs_fail(cs_name, subr,
                            "less arguments on stack (%i) than required (%i)",
                            (int) (stack_ptr - cc_stack), (int) cc->nargs);
                else if (stack_ptr - cc_stack > cc->nargs)
                    cs_fail(cs_name, subr,
                            "more arguments on stack (%i) than required (%i)",
                            (int) (stack_ptr - cc_stack), (int) cc->nargs);
            }
            last_cmd = b;
            switch (cc - cc_tab) {
            case CS_CALLSUBR:
                a1 = cc_get(-1);
                cc_pop(1);
                mark_subr(a1);
                if (!subr_tab[a1].valid) {
                    cs_fail(cs_name, subr, "cannot call subr (%i)", (int) a1);
                    goto cs_error;
                }
                break;
            case CS_DIV:
                cc_pop(2);
                cc_push(0);
                break;
            case CS_CALLOTHERSUBR:
                if (cc_get(-1) == 3)
                    lastargOtherSubr3 = cc_get(-3);
                a1 = cc_get(-2) + 2;
                cc_pop(a1);
                break;
            case CS_POP:
                cc_push(lastargOtherSubr3);
                /* the only case when we care about the value being pushed onto
                   stack is when POP follows CALLOTHERSUBR (changing hints by
                   OtherSubrs[3])
                 */
                break;
            case CS_SEAC:
                a1 = cc_get(3);
                a2 = cc_get(4);
                cc_clear();
                mark_cs(standard_glyph_names[a1]);
                mark_cs(standard_glyph_names[a2]);
                break;
            default:
                if (cc->clear)
                    cc_clear();
            }
        }
    }
    if (cs_name == NULL && last_cmd != CS_RETURN) {
        pdftex_warn("last command in subr `%i' is not a RETURN; "
                    "I will add it now but please consider fixing the font",
                    (int) subr);
        append_cs_return(ptr);
    }
    return;
  cs_error:                    /* an error occured during parsing */
    cc_clear();
    ptr->valid = false;
    ptr->used = false;
}

/**********************************************************************/
/* AVL search tree for glyph code by glyph name */

static int comp_t1_glyphs(const void *pa, const void *pb, void *p)
{
    return strcmp(*((const char * const *) pa), *((const char * const *) pb));
}

static struct avl_table *create_t1_glyph_tree(char **glyph_names)
{
    int i;
    void **aa;
    static struct avl_table *gl_tree;
    gl_tree = avl_create(comp_t1_glyphs, NULL, &avl_xallocator);
    assert(gl_tree != NULL);
    for (i = 0; i < 256; i++) {
        if (glyph_names[i] != notdef &&
            (char **) avl_find(gl_tree, &glyph_names[i]) == NULL) {
            /* no strdup here, just point to the glyph_names array members */
            aa = avl_probe(gl_tree, &glyph_names[i]);
            assert(aa != NULL);
        }
    }
    return gl_tree;
}

static void destroy_t1_glyph_tree(struct avl_table *gl_tree)
{
    assert(gl_tree != NULL);
    avl_destroy(gl_tree, NULL);
}

/**********************************************************************/

static void t1_subset_ascii_part(void)
{
    int j, *p;
    char *glyph, **gg, **glyph_names;
    struct avl_table *gl_tree;
    struct avl_traverser t;
    void **aa;
    assert(fd_cur != NULL);
    assert(fd_cur->gl_tree != NULL);
    t1_getline();
    while (!t1_prefix("/Encoding")) {
        t1_scan_param();
        if (!(t1_prefix("/UniqueID")
              && !strncmp(t1_line_array + strlen(t1_line_array) -4, "def", 3)))
            t1_putline();
        t1_getline();
    }
    glyph_names = t1_builtin_enc();
    fd_cur->builtin_glyph_names = glyph_names;
    if (is_subsetted(fd_cur->fm)) {
        assert(is_included(fd_cur->fm));
        if (fd_cur->tx_tree != NULL) {
            /* take over collected non-reencoded characters from TeX */
            avl_t_init(&t, fd_cur->tx_tree);
            for (p = (int *) avl_t_first(&t, fd_cur->tx_tree); p != NULL;
                 p = (int *) avl_t_next(&t)) {
                if ((char *) avl_find(fd_cur->gl_tree, glyph_names[*p]) == NULL) {
                    glyph = xstrdup(glyph_names[*p]);
                    aa = avl_probe(fd_cur->gl_tree, glyph);
                    assert(aa != NULL);
                }
            }
        }
        make_subset_tag(fd_cur);
        assert(t1_fontname_offset != 0);
        strncpy(fb_array + t1_fontname_offset, fd_cur->subset_tag, 6);
    }
    /* now really all glyphs needed from this font are in the fd_cur->gl_tree */
    if (t1_encoding == ENC_STANDARD)
        t1_puts("/Encoding StandardEncoding def\n");
    else {
        t1_puts
            ("/Encoding 256 array\n0 1 255 {1 index exch /.notdef put} for\n");
        gl_tree = create_t1_glyph_tree(glyph_names);
        avl_t_init(&t, fd_cur->gl_tree);
        j = 0;
        for (glyph = (char *) avl_t_first(&t, fd_cur->gl_tree); glyph != NULL;
             glyph = (char *) avl_t_next(&t)) {
            if ((gg = (char **) avl_find(gl_tree, &glyph)) != NULL) {
                t1_printf("dup %i /%s put\n", (int) (gg - glyph_names), *gg);
                j++;
            }
        }
        destroy_t1_glyph_tree(gl_tree);
        if (j == 0)
            /* We didn't mark anything for the Encoding array. */
            /* We add "dup 0 /.notdef put" for compatibility   */
            /* with Acrobat 5.0.                               */
            t1_puts("dup 0 /.notdef put\n");
        t1_puts("readonly def\n");
    }
    do {
        t1_getline();
        t1_scan_param();
        if (!t1_prefix("/UniqueID"))    /* ignore UniqueID for subsetted fonts */
            t1_putline();
    } while (t1_in_eexec == 0);
}

static void cs_init(void)
{
    cs_ptr = cs_tab = NULL;
    cs_dict_start = cs_dict_end = NULL;
    cs_count = cs_size = cs_size_pos = 0;
    cs_token_pair = NULL;
    subr_tab = NULL;
    subr_array_start = subr_array_end = NULL;
    subr_max = subr_size = subr_size_pos = 0;
}

static void init_cs_entry(cs_entry *cs)
{
    cs->data = NULL;
    cs->name = NULL;
    cs->len = 0;
    cs->cslen = 0;
    cs->used = false;
    cs->valid = false;
}

static void t1_read_subrs(void)
{
    int i, s;
    cs_entry *ptr;
    t1_getline();
    while (!(t1_charstrings() || t1_subrs())) {
        t1_scan_param();
        if (!t1_prefix("/UniqueID"))    /* ignore UniqueID for subsetted fonts */
            t1_putline();
        t1_getline();
    }
  found:
    t1_cs = true;
    t1_scan = false;
    if (!t1_subrs())
        return;
    subr_size_pos = strlen("/Subrs") + 1;
    /* subr_size_pos points to the number indicating dict size after "/Subrs" */
    subr_size = t1_scan_num(t1_line_array + subr_size_pos, 0);
    if (subr_size == 0) {
        while (!t1_charstrings())
            t1_getline();
        return;
    }
    subr_tab = xtalloc(subr_size, cs_entry);
    for (ptr = subr_tab; ptr - subr_tab < subr_size; ptr++)
        init_cs_entry(ptr);
    subr_array_start = xstrdup(t1_line_array);
    t1_getline();
    while (t1_cslen) {
        store_subr();
        t1_getline();
    }
    /* mark the first four entries without parsing */
    for (i = 0; i < subr_size && i < 4; i++)
        subr_tab[i].used = true;
    /* the end of the Subrs array might have more than one line so we need to
       concatnate them to subr_array_end. Unfortunately some fonts don't have
       the Subrs array followed by the CharStrings dict immediately (synthetic
       fonts). If we cannot find CharStrings in next POST_SUBRS_SCAN lines then
       we will treat the font as synthetic and ignore everything until next
       Subrs is found
     */

#define POST_SUBRS_SCAN  5

    s = 0;
    *t1_buf_array = 0;
    for (i = 0; i < POST_SUBRS_SCAN; i++) {
        if (t1_charstrings())
            break;
        s += t1_line_ptr - t1_line_array;
        alloc_array(t1_buf, s, T1_BUF_SIZE);
        strcat(t1_buf_array, t1_line_array);
        t1_getline();
    }
    subr_array_end = xstrdup(t1_buf_array);
    if (i == POST_SUBRS_SCAN) { /* CharStrings not found;
                                   suppose synthetic font */
        for (ptr = subr_tab; ptr - subr_tab < subr_size; ptr++)
            if (ptr->valid)
                xfree(ptr->data);
        xfree(subr_tab);
        xfree(subr_array_start);
        xfree(subr_array_end);
        cs_init();
        t1_cs = false;
        t1_synthetic = true;
        while (!(t1_charstrings() || t1_subrs()))
            t1_getline();
        goto found;
    }
}

#define t1_subr_flush()  t1_flush_cs(true)
#define t1_cs_flush()    t1_flush_cs(false)

static void t1_flush_cs(boolean is_subr)
{
    char *p;
    byte *r, *return_cs = NULL;
    cs_entry *tab, *end_tab, *ptr;
    char *start_line, *line_end;
    int count, size_pos;
    unsigned short cr, cs_len = 0;      /* to avoid warning about uninitialized use of cs_len */
    if (is_subr) {
        start_line = subr_array_start;
        line_end = subr_array_end;
        size_pos = subr_size_pos;
        tab = subr_tab;
        count = subr_max + 1;
        end_tab = subr_tab + count;
    } else {
        start_line = cs_dict_start;
        line_end = cs_dict_end;
        size_pos = cs_size_pos;
        tab = cs_tab;
        end_tab = cs_ptr;
        count = cs_count;
    }
    t1_line_ptr = t1_line_array;
    for (p = start_line; p - start_line < size_pos;)
        *t1_line_ptr++ = *p++;
    while (isdigit((unsigned char)*p))
        p++;
    sprintf(t1_line_ptr, "%u", count);
    strcat(t1_line_ptr, p);
    t1_line_ptr = eol(t1_line_array);
    t1_putline();

    /* create return_cs to replace unused subr's */
    if (is_subr) {
        cr = 4330;
        cs_len = 0;
        /* at this point we have t1_lenIV >= 0;
         * a negative value would be caught in t1_scan_param() */
        return_cs = xtalloc(t1_lenIV + 1, byte);
        for (cs_len = 0, r = return_cs; cs_len < t1_lenIV; cs_len++, r++)
            *r = cencrypt(0x00, &cr);
        *r = cencrypt(CS_RETURN, &cr);
        cs_len++;
    }

    for (ptr = tab; ptr < end_tab; ptr++) {
        if (ptr->used) {
            if (is_subr)
                sprintf(t1_line_array, "dup %lu %u",
                        (unsigned long) (ptr - tab), ptr->cslen);
            else
                sprintf(t1_line_array, "/%s %u", ptr->name, ptr->cslen);
            p = strend(t1_line_array);
            memcpy(p, ptr->data, ptr->len);
            t1_line_ptr = p + ptr->len;
            t1_putline();
        } else {
            /* replace unsused subr's by return_cs */
            if (is_subr) {
                sprintf(t1_line_array, "dup %lu %u%s ",
                        (unsigned long) (ptr - tab), cs_len, cs_token_pair[0]);
                p = strend(t1_line_array);
                memcpy(p, return_cs, cs_len);
                t1_line_ptr = p + cs_len;
                t1_putline();
                sprintf(t1_line_array, " %s", cs_token_pair[1]);
                t1_line_ptr = eol(t1_line_array);
                t1_putline();
            }
        }
        xfree(ptr->data);
        if (ptr->name != notdef)
            xfree(ptr->name);
    }
    sprintf(t1_line_array, "%s", line_end);
    t1_line_ptr = eol(t1_line_array);
    t1_putline();
    if (is_subr)
        xfree(return_cs);
    xfree(tab);
    xfree(start_line);
    xfree(line_end);
}

static void t1_mark_glyphs(void)
{
    char *glyph;
    struct avl_traverser t;
    cs_entry *ptr;
    if (t1_synthetic || fd_cur->all_glyphs) {   /* mark everything */
        if (cs_tab != NULL)
            for (ptr = cs_tab; ptr < cs_ptr; ptr++)
                if (ptr->valid)
                    ptr->used = true;
        if (subr_tab != NULL) {
            for (ptr = subr_tab; ptr - subr_tab < subr_size; ptr++)
                if (ptr->valid)
                    ptr->used = true;
            subr_max = subr_size - 1;
        }
        return;
    }
    mark_cs(notdef);
    avl_t_init(&t, fd_cur->gl_tree);
    for (glyph = (char *) avl_t_first(&t, fd_cur->gl_tree); glyph != NULL;
         glyph = (char *) avl_t_next(&t)) {
        mark_cs(glyph);
    }
    if (subr_tab != NULL)
        for (subr_max = -1, ptr = subr_tab; ptr - subr_tab < subr_size; ptr++)
            if (ptr->used && ptr - subr_tab > subr_max)
                subr_max = ptr - subr_tab;
}

static void t1_check_unusual_charstring(void)
{
    char *p = strstr(t1_line_array, charstringname) + strlen(charstringname);
    int i;
    /* if no number follows "/CharStrings", let's read the next line */
    if (sscanf(p, "%i", &i) != 1) {
        /* pdftex_warn("no number found after `%s', I assume it's on the next line",
                    charstringname); */
        strcpy(t1_buf_array, t1_line_array);

        /* t1_getline always appends EOL to t1_line_array; let's change it to
         * space before appending the next line
         */
        *(strend(t1_buf_array) - 1) = ' ';

        t1_getline();
        alloc_array(t1_buf, strlen(t1_line_array)
                            + (t1_buf_array ? strlen(t1_buf_array) + 1 : 0),
                            T1_BUF_SIZE);
        strcat(t1_buf_array, t1_line_array);
        alloc_array(t1_line, strlen(t1_buf_array) + 1, T1_BUF_SIZE);
        strcpy(t1_line_array, t1_buf_array);
        t1_line_ptr = eol(t1_line_array);
    }
}

static void t1_subset_charstrings(void)
{
    cs_entry *ptr;

    /* at this point t1_line_array contains "/CharStrings".
       when we hit a case like this:
         dup/CharStrings
         229 dict dup begin
       we read the next line and concatenate to t1_line_array before moving on
    */
    t1_check_unusual_charstring();

    cs_size_pos = strstr(t1_line_array, charstringname)
                  + strlen(charstringname) - t1_line_array + 1;
    /* cs_size_pos points to the number indicating
       dict size after "/CharStrings" */
    cs_size = t1_scan_num(t1_line_array + cs_size_pos, 0);
    cs_ptr = cs_tab = xtalloc(cs_size, cs_entry);
    for (ptr = cs_tab; ptr - cs_tab < cs_size; ptr++)
        init_cs_entry(ptr);
    cs_notdef = NULL;
    cs_dict_start = xstrdup(t1_line_array);
    t1_getline();
    while (t1_cslen) {
        store_cs();
        t1_getline();
    }
    cs_dict_end = xstrdup(t1_line_array);
    t1_mark_glyphs();
    if (subr_tab != NULL) {
        if (cs_token_pair == NULL)
            pdftex_fail
                ("This Type 1 font uses mismatched subroutine begin/end token pairs.");
        t1_subr_flush();
    }
    for (cs_count = 0, ptr = cs_tab; ptr < cs_ptr; ptr++)
        if (ptr->used)
            cs_count++;
    t1_cs_flush();
}

static void t1_subset_end(void)
{
    if (t1_synthetic) {         /* copy to "dup /FontName get exch definefont pop" */
        while (!strstr(t1_line_array, "definefont")) {
            t1_getline();
            t1_putline();
        }
        while (!t1_end_eexec())
            t1_getline();       /* ignore the rest */
        t1_putline();           /* write "mark currentfile closefile" */
    } else
        while (!t1_end_eexec()) {       /* copy to "mark currentfile closefile" */
            t1_getline();
            t1_putline();
        }
    t1_stop_eexec();
    if (fixedcontent) {         /* copy 512 zeros (not needed for PDF) */
        while (!t1_cleartomark()) {
            t1_getline();
            t1_putline();
        }
        if (!t1_synthetic)      /* don't check "{restore}if" for synthetic fonts */
            t1_check_end();     /* write "{restore}if" if found */
    }
    get_length3();
}

void writet1(fd_entry *fd)
{
    fd_cur = fd;                /* fd_cur is global inside writet1.c */
    assert(fd_cur->fm != NULL);
    assert(is_type1(fd->fm));
    assert(is_included(fd->fm));

    t1_save_offset = 0;
    if (!is_subsetted(fd_cur->fm)) {    /* include entire font */
        if (!(fd->ff_found = t1_open_fontfile("<<")))
            return;
        t1_include();
        t1_close_font_file(">>");
        return;
    }
    /* partial downloading */
    if (!(fd->ff_found = t1_open_fontfile("<")))
        return;
    t1_subset_ascii_part();
    t1_start_eexec();
    cc_init();
    cs_init();
    t1_read_subrs();
    t1_subset_charstrings();
    t1_subset_end();
    t1_close_font_file(">");
}

void t1_free(void)
{
    xfree(t1_line_array);
    xfree(t1_buf_array);
}
