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

#include <w2c/config.h>		/* for large file support */
#include <sys/types.h>
#include <regex.h>
#ifndef WEBASSEMBLY_BUILD
#include <kpathsea/config.h>
#include <kpathsea/c-proto.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/version.h>
#endif
#include <string.h>
#include <time.h>
#include <float.h>              /* for DBL_EPSILON */
#include "md5.h"
#include <zlib.h>
#include "ptexlib.h"
#include <png.h>
#ifdef POPPLER_VERSION
/* POPPLER_VERSION should be a proper version string */
#define xpdfVersion POPPLER_VERSION
#define xpdfString "poppler"
#else
#include <xpdf/config.h>        /* just to get the xpdf version */
#define xpdfString "xpdf"
#endif

#define check_nprintf(size_get, size_want) \
    if ((unsigned)(size_get) >= (unsigned)(size_want)) \
        pdftex_fail ("snprintf failed: file %s, line %d", __FILE__, __LINE__);

char *cur_file_name = NULL;
strnumber last_tex_string;
static char print_buf[PRINTF_BUF_SIZE];
static char *jobname_cstr = NULL;
static char *job_id_string = NULL;

size_t last_ptr_index;          /* for use with alloc_array */

/* define fb_ptr, fb_array & fb_limit */
typedef char fb_entry;
define_array(fb);

/* define char_ptr, char_array & char_limit */
typedef char char_entry;
define_array(char);

/* define vf_e_fnts_ptr, vf_e_fnts_array & vf_e_fnts_limit */
typedef integer vf_e_fnts_entry;
define_array(vf_e_fnts);

/* define vf_i_fnts_ptr, vf_i_fnts_array & vf_i_fnts_limit */
typedef internalfontnumber vf_i_fnts_entry;
define_array(vf_i_fnts);

integer fb_offset(void)
{
    return fb_ptr - fb_array;
}

void fb_seek(integer offset)
{
    fb_ptr = fb_array + offset;
}

void fb_putchar(eightbits b)
{
    alloc_array(fb, 1, SMALL_ARRAY_SIZE);
    *fb_ptr++ = b;
}

void fb_flush(void)
{
    fb_entry *p;
    integer n;
    for (p = fb_array; p < fb_ptr;) {
        n = pdfbufsize - pdfptr;
        if (fb_ptr - p < n)
            n = fb_ptr - p;
        memcpy(pdfbuf + pdfptr, p, (unsigned) n);
        pdfptr += n;
        if (pdfptr == pdfbufsize)
            pdfflush();
        p += n;
    }
    fb_ptr = fb_array;
}

#define SUBSET_TAG_LENGTH 6

void make_subset_tag(fd_entry * fd)
{
    int i, j = 0, a[SUBSET_TAG_LENGTH];
    md5_state_t pms;
    char *glyph;
    struct avl_traverser t;
    md5_byte_t digest[16];
    void **aa;
    static struct avl_table *st_tree = NULL;
    if (st_tree == NULL)
        st_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(fd != NULL);
    assert(fd->gl_tree != NULL);
    assert(fd->fontname != NULL);
    assert(fd->subset_tag == NULL);
    fd->subset_tag = xtalloc(SUBSET_TAG_LENGTH + 1, char);
    do {
        md5_init(&pms);
        avl_t_init(&t, fd->gl_tree);
        for (glyph = (char *) avl_t_first(&t, fd->gl_tree); glyph != NULL;
             glyph = (char *) avl_t_next(&t)) {
            md5_append(&pms, (md5_byte_t *) glyph, strlen(glyph));
            md5_append(&pms, (const md5_byte_t *) " ", 1);
        }
        md5_append(&pms, (md5_byte_t *) fd->fontname, strlen(fd->fontname));
        md5_append(&pms, (md5_byte_t *) & j, sizeof(int));      /* to resolve collision */
        md5_finish(&pms, digest);
        for (a[0] = 0, i = 0; i < 13; i++)
            a[0] += digest[i];
        for (i = 1; i < SUBSET_TAG_LENGTH; i++)
            a[i] = a[i - 1] - digest[i - 1] + digest[(i + 12) % 16];
        for (i = 0; i < SUBSET_TAG_LENGTH; i++)
            fd->subset_tag[i] = a[i] % 26 + 'A';
        fd->subset_tag[SUBSET_TAG_LENGTH] = '\0';
        j++;
        assert(j < 100);
    }
    while ((char *) avl_find(st_tree, fd->subset_tag) != NULL);
    aa = avl_probe(st_tree, fd->subset_tag);
    assert(aa != NULL);
    if (j > 2)
        pdftex_warn
            ("\nmake_subset_tag(): subset-tag collision, resolved in round %d.\n",
             j);
}

void pdf_puts(const char *s)
{
    pdfroom(strlen(s) + 1);
    while (*s)
        pdfbuf[pdfptr++] = *s++;
    pdflastbyte = s[-1];
}

void pdf_newline(void)
{
    if (pdflastbyte != '\n')
        pdf_puts("\n");
}

__attribute__ ((format(printf, 1, 2)))
void pdf_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    pdf_puts(print_buf);
    va_end(args);
}

strnumber maketexstring(const char *s)
{
    size_t l;
    if (s == NULL || *s == 0)
        return getnullstr();
    l = strlen(s);
    check_buf(poolptr + l, poolsize);
    while (l-- > 0)
        strpool[poolptr++] = *s++;
    last_tex_string = makestring();
    return last_tex_string;
}

__attribute__ ((format(printf, 1, 2)))
void tex_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    print(maketexstring(print_buf));
    flushstr(last_tex_string);
    xfflush(stdout);
    va_end(args);
}

/* Helper for pdftex_fail. */
static void safe_print(const char *str)
{
    const char *c;
    for (c = str; *c; ++c)
        print(*c);
}

void removepdffile(void)
{
    
    if (outputfilename && !fixedpdfdraftmode) {
        xfclose(pdffile, makecstring(outputfilename));
        remove(makecstring(outputfilename));
    }

}

/* pdftex_fail may be called when a buffer overflow has happened/is
   happening, therefore may not call mktexstring.  However, with the
   current implementation it appears that error messages are misleading,
   possibly because pool overflows are detected too late.

   The output format of this fuction must be the same as pdf_error in
   pdftex.web! */
__attribute__ ((noreturn, format(printf, 1, 2)))
void pdftex_fail(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    println();
    safe_print("!pdfTeX error: ");
    if (cur_file_name) {
        safe_print(" (file ");
        safe_print(cur_file_name);
        safe_print(")");
    }
    safe_print(": ");
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    safe_print(print_buf);
    va_end(args);
    println();
    removepdffile();
    safe_print(" ==> Fatal error occurred, no output PDF file produced!");
    println();
    abort();
}

/* The output format of this fuction must be the same as pdf_warn in
   pdftex.web! */
__attribute__ ((format(printf, 1, 2)))
void pdftex_warn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    println();
    println();
    tex_printf("pdfTeX warning: ");
    if (cur_file_name)
        tex_printf(" (file %s)", cur_file_name);
    tex_printf(": ");
    vsnprintf(print_buf, PRINTF_BUF_SIZE, fmt, args);
    print(maketexstring(print_buf));
    flushstr(last_tex_string);
    va_end(args);
    println();
}

void garbagewarning(void)
{
    pdftex_warn("dangling objects discarded, no output file produced.");
    removepdffile();
}

void setjobid(int year, int month, int day, int time)
{
    char *name_string, *format_string, *s;
    size_t slen;
    int i;

    if (job_id_string != NULL)
        return;

    name_string = xstrdup(makecstring(jobname));
    format_string = xstrdup(makecstring(formatident));
    slen = SMALL_BUF_SIZE +
        strlen(name_string) +
        strlen(format_string) +
        strlen(ptexbanner) +
        strlen(versionstring);
    s = xtalloc(slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf(s, slen,
                 "%.4d/%.2d/%.2d %.2d:%.2d %s %s %s%s",
                 year, month, day, time / 60, time % 60,
                 name_string, format_string, ptexbanner,
                 versionstring);
    check_nprintf(i, slen);
    job_id_string = xstrdup(s);
    xfree(s);
    xfree(name_string);
    xfree(format_string);
}

void makepdftexbanner(void)
{
    static boolean pdftexbanner_init = false;
    char *s;
    size_t slen;
    int i;

    if (pdftexbanner_init)
        return;

    slen = SMALL_BUF_SIZE +
        strlen(ptexbanner) +
        strlen(versionstring);
    s = xtalloc(slen, char);
    /* The Web2c version string starts with a space.  */
    i = snprintf(s, slen,
                 "%s%s", ptexbanner, versionstring);
    check_nprintf(i, slen);
    pdftexbanner = maketexstring(s);
    xfree(s);
    pdftexbanner_init = true;
}

strnumber getresnameprefix(void)
{
/*     static char name_str[] = */
/* "!\"$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\" */
/* "^_`abcdefghijklmnopqrstuvwxyz|~"; */
    static char name_str[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char prefix[7];             /* make a tag of 6 chars long */
    unsigned long crc;
    int i;
    size_t base = strlen(name_str);
    crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (Bytef *) job_id_string, strlen(job_id_string));
    for (i = 0; i < 6; i++) {
        prefix[i] = name_str[crc % base];
        crc /= base;
    }
    prefix[6] = 0;
    return maketexstring(prefix);
}

size_t xfwrite(void *ptr, size_t size, size_t nmemb, FILE * stream)
{
    if (fwrite(ptr, size, nmemb, stream) != nmemb)
        pdftex_fail("fwrite() failed");
    return nmemb;
}

int xfflush(FILE * stream)
{
    if (fflush(stream) != 0)
        pdftex_fail("fflush() failed (%s)", strerror(errno));
    return 0;
}

int xgetc(FILE * stream)
{
    int c = getc(stream);
    if (c < 0 && c != EOF)
        pdftex_fail("getc() failed (%s)", strerror(errno));
    return c;
}

int xputc(int c, FILE * stream)
{
    int i = putc(c, stream);
    if (i < 0)
        pdftex_fail("putc() failed (%s)", strerror(errno));
    return i;
}

void writestreamlength(longinteger length, longinteger offset)
{
    if (jobname_cstr == NULL)
        jobname_cstr = xstrdup(makecstring(jobname));
    if (fixedpdfdraftmode == 0) {
        xfseeko(pdffile, (off_t) offset, SEEK_SET, jobname_cstr);
        fprintf(pdffile, "%" LONGINTEGER_PRI "i", (LONGINTEGER_TYPE) length);
        xfseeko(pdffile, (off_t) pdfoffset(), SEEK_SET, jobname_cstr);
    }
}

scaled extxnoverd(scaled x, scaled n, scaled d)
{
    double r = (((double) x) * ((double) n)) / ((double) d);
    if (r > DBL_EPSILON)
        r += 0.5;
    else
        r -= 0.5;
    if (r >= (double) maxinteger || r <= -(double) maxinteger)
        pdftex_warn("arithmetic: number too big");
    return (scaled) r;
}

void libpdffinish(void)
{
    xfree(fb_array);
    xfree(char_array);
    xfree(job_id_string);
    fm_free();
    t1_free();
    enc_free();
    img_free();
    vf_free();
    epdf_free();
    ttf_free();
    sfd_free();
    glyph_unicode_free();
    zip_free();
}

/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 */
char *convertStringToPDFString(const char *in, int len)
{
    static char pstrbuf[MAX_PSTRING_LEN];
    char *out = pstrbuf;
    int i, j = 0, k;
    char buf[5];
    for (i = 0; i < len; i++) {
        check_buf(j + sizeof(buf), MAX_PSTRING_LEN);
        if (((unsigned char) in[i] < '!') || ((unsigned char) in[i] > '~')) {
            /* convert control characters into oct */
            k = snprintf(buf, sizeof(buf),
                         "\\%03o", (unsigned int) (unsigned char) in[i]);
            check_nprintf(k, sizeof(buf));
            out[j++] = buf[0];
            out[j++] = buf[1];
            out[j++] = buf[2];
            out[j++] = buf[3];
        } else if ((in[i] == '(') || (in[i] == ')')) {
            /* escape paranthesis */
            out[j++] = '\\';
            out[j++] = in[i];
        } else if (in[i] == '\\') {
            /* escape backslash */
            out[j++] = '\\';
            out[j++] = '\\';
        } else {
            /* copy char :-) */
            out[j++] = in[i];
        }
    }
    out[j] = '\0';
    return pstrbuf;
}


/* Converts any string given in in in an allowed PDF string which can be
 * handled by printf et.al.: \ is escaped to \\, parenthesis are escaped and
 * control characters are octal encoded.
 * This assumes that the string does not contain any already escaped
 * characters!
 *
 * See escapename for parameter description.
 */
void escapestring(poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    while (in < out) {
        if (poolptr + 4 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch < '!') || (ch > '~')) {
            /* convert control characters into oct */
            int i = snprintf((char *) &strpool[poolptr], 5,
                             "\\%.3o", (unsigned int) ch);
            check_nprintf(i, 5);
            poolptr += i;
            continue;
        }
        if ((ch == '(') || (ch == ')') || (ch == '\\')) {
            /* escape parenthesis and backslash */
            strpool[poolptr++] = '\\';
        }
        /* copy char :-) */
        strpool[poolptr++] = ch;
    }
}

/* Convert any given string in a PDF name using escaping mechanism
   of PDF 1.2. The result does not include the leading slash.

   PDF specification 1.6, section 3.2.6 "Name Objects" explains:
   <blockquote>
    Beginning with PDF 1.2, any character except null (character code 0) may
    be included in a name by writing its 2-digit hexadecimal code, preceded
    by the number sign character (#); see implementation notes 3 and 4 in
    Appendix H. This syntax is required to represent any of the delimiter or
    white-space characters or the number sign character itself; it is
    recommended but not required for characters whose codes are outside the
    range 33 (!) to 126 (~).
   </blockquote>
   The following table shows the conversion that are done by this
   function:
     code      result   reason
     -----------------------------------
     0         ignored  not allowed
     1..32     escaped  must for white-space:
                          9 (tab), 10 (lf), 12 (ff), 13 (cr), 32 (space)
                        recommended for the other control characters
     35        escaped  escape char "#"
     37        escaped  delimiter "%"
     40..41    escaped  delimiters "(" and ")"
     47        escaped  delimiter "/"
     60        escaped  delimiter "<"
     62        escaped  delimiter ">"
     91        escaped  delimiter "["
     93        escaped  delimiter "]"
     123       escaped  delimiter "{"
     125       escaped  delimiter "}"
     127..255  escaped  recommended
     else      copy     regular characters

   Parameter "in" is a pointer into the string pool where
   the input string is located. The output string is written
   as temporary string right after the input string.
   Thus at the begin of the procedure the global variable
   "poolptr" points to the start of the output string and
   after the end when the procedure returns.
*/
void escapename(poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (poolptr + 3 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch >= 1 && ch <= 32) || ch >= 127) {
            /* escape */
            i = snprintf((char *) &strpool[poolptr], 4,
                         "#%.2X", (unsigned int) ch);
            check_nprintf(i, 4);
            poolptr += i;
            continue;
        }
        switch (ch) {
        case 0:
            /* ignore */
            break;
        case 35:
        case 37:
        case 40:
        case 41:
        case 47:
        case 60:
        case 62:
        case 91:
        case 93:
        case 123:
        case 125:
            /* escape */
            i = snprintf((char *) &strpool[poolptr], 4,
                         "#%.2X", (unsigned int) ch);
            check_nprintf(i, 4);
            poolptr += i;
            break;
        default:
            /* copy */
            strpool[poolptr++] = ch;
        }
    }
}

/* Convert any given string in a PDF hexadecimal string. The
   result does not include the angle brackets.

   This procedure uses uppercase hexadecimal letters.

   See escapename for description of parameters.
*/
void escapehex(poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    int i;

    while (in < out) {
        if (poolptr + 2 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        i = snprintf((char *) &strpool[poolptr], 3, "%.2X", (unsigned int) ch);
        check_nprintf(i, 3);
        poolptr += 2;
    }
}

/* Unescape any given hexadecimal string.

   Last hex digit can be omitted, it is replaced by zero, see
   PDF specification.

   Invalid digits are silently ignored.

   See escapename for description of parameters.
*/
void unescapehex(poolpointer in)
{
    const poolpointer out = poolptr;
    unsigned char ch;
    unsigned char a = 0;        /* to avoid warning about uninitialized use of a */
    boolean first = true;

    while (in < out) {
        if (poolptr + 1 >= poolsize) {
            poolptr = poolsize;
            /* error by str_toks that calls str_room(1) */
            return;
        }

        ch = (unsigned char) strpool[in++];

        if ((ch >= '0') && (ch <= '9')) {
            ch -= '0';
        } else if ((ch >= 'A') && (ch <= 'F')) {
            ch -= 'A' - 10;
        } else if ((ch >= 'a') && (ch <= 'f')) {
            ch -= 'a' - 10;
        } else {
            continue;           /* ignore wrong character */
        }

        if (first) {
            a = ch << 4;
            first = false;
            continue;
        }

        strpool[poolptr++] = a + ch;
        first = true;
    }
    if (!first) {               /* last hex digit is omitted */
        strpool[poolptr++] = a;
    }
}

/* Compute the ID string as per PDF1.4 9.3:
  <blockquote>
    File identifers are defined by the optional ID entry in a PDF file's
    trailer dictionary (see Section 3.4.4, "File Trailer"; see also
    implementation note 105 in Appendix H). The value of this entry is an
    array of two strings. The first string is a permanent identifier based
    on the contents of the file at the time it was originally created, and
    does not change when the file is incrementally updated. The second
    string is a changing identifier based on the file's contents at the
    time it was last updated. When a file is first written, both
    identifiers are set to the same value. If both identifiers match when a
    file reference is resolved, it is very likely that the correct file has
    been found; if only the first identifier matches, then a different
    version of the correct file has been found.
        To help ensure the uniqueness of file identifiers, it is recommend
    that they be computed using a message digest algorithm such as MD5
    (described in Internet RFC 1321, The MD5 Message-Digest Algorithm; see
    the Bibliography), using the following information (see implementation
    note 106 in Appendix H):
    - The current time
    - A string representation of the file's location, usually a pathname
    - The size of the file in bytes
    - The values of all entries in the file's document information
      dictionary (see Section 9.2.1,  Document Information Dictionary )
  </blockquote>
  This stipulates only that the two IDs must be identical when the file is
  created and that they should be reasonably unique. Since it's difficult
  to get the file size at this point in the execution of pdfTeX, scanning
  the info dict is also difficult, and any variability in the current
  directory name leads to non-reproducible builds, we start with a
  simpler implementation using just the current time and the file name.
 */
void printID(strnumber filename)
{
    md5_state_t state;
    md5_byte_t digest[16];
    char id[64];
    char *file_name;
    /* start md5 */
    md5_init(&state);
    /* get the time */
    initstarttime();
    md5_append(&state, (const md5_byte_t *) start_time_str, strlen(start_time_str));
    /* get the file name */
    file_name = makecstring(filename);
    md5_append(&state, (const md5_byte_t *) file_name, strlen(file_name));
    /* finish md5 */
    md5_finish(&state, digest);
    /* write the IDs */
    convertStringToHexString((char *) digest, id, 16);
    pdf_printf("/ID [<%s> <%s>]", id, id);
}

void printIDalt(integer toks)
{
    md5_state_t state;
    md5_byte_t digest[16];
    char id[64];
    char *s = makecstring(tokenstostring(toks));
    flushstr(lasttokensstring);
    if (strlen(s) == 0)
        return;
    md5_init(&state);
    md5_append(&state, (const md5_byte_t *) s, strlen(s));
    md5_finish(&state, digest);
    convertStringToHexString((char *) digest, id, 16);
    pdf_printf("/ID [<%s> <%s>]", id, id);
}


/* Print the /CreationDate entry.

  PDF Reference, third edition says about the expected date format:
  <blockquote>
    3.8.2 Dates

      PDF defines a standard date format, which closely follows that of
      the international standard ASN.1 (Abstract Syntax Notation One),
      defined in ISO/IEC 8824 (see the Bibliography). A date is a string
      of the form

        (D:YYYYMMDDHHmmSSOHH'mm')

      where

        YYYY is the year
        MM is the month
        DD is the day (01-31)
        HH is the hour (00-23)
        mm is the minute (00-59)
        SS is the second (00-59)
        O is the relationship of local time to Universal Time (UT),
          denoted by one of the characters +, -, or Z (see below)
        HH followed by ' is the absolute value of the offset from UT
          in hours (00-23)
        mm followed by ' is the absolute value of the offset from UT
          in minutes (00-59)

      The apostrophe character (') after HH and mm is part of the syntax.
      All fields after the year are optional. (The prefix D:, although also
      optional, is strongly recommended.) The default values for MM and DD
      are both 01; all other numerical fields default to zero values.  A plus
      sign (+) as the value of the O field signifies that local time is
      later than UT, a minus sign (-) that local time is earlier than UT,
      and the letter Z that local time is equal to UT. If no UT information
      is specified, the relationship of the specified time to UT is
      considered to be unknown. Whether or not the time zone is known, the
      rest of the date should be specified in local time.

      For example, December 23, 1998, at 7:52 PM, U.S. Pacific Standard
      Time, is represented by the string

        D:199812231952-08'00'
  </blockquote>

  The main difficulty is get the time zone offset. strftime() does this in ISO
  C99 (e.g. newer glibc) with %z, but we have to work with other systems (e.g.
  Solaris 2.5).
*/

void printcreationdate(void)
{
    initstarttime();
    pdf_printf("/CreationDate (%s)\n", start_time_str);
}

void printmoddate(void)
{
    initstarttime();
    pdf_printf("/ModDate (%s)\n", start_time_str);
}


#define DEFAULT_SUB_MATCH_COUNT 10
static int sub_match_count = DEFAULT_SUB_MATCH_COUNT;
static regmatch_t *pmatch = NULL;
static char *match_string = NULL;
static int last_match_succeeded = 0;

/* Implements \pdfmatch */
void
matchstrings(strnumber s, strnumber t, int subcount, boolean icase)
{
    regex_t preg;
    int cflags = REG_EXTENDED;
    int eflags = 0;
    int ret;
    char *str;

    if (icase) {
        cflags |= REG_ICASE;
    }

    if (poolptr + 10 >= poolsize) {
        poolptr = poolsize;
        return;
    }

    str = makecstring(s);
    ret = regcomp(&preg, str, cflags);
    if (ret != 0) {
        size_t size = regerror(ret, &preg, NULL, 0);
        str = xtalloc(size, char);
        regerror(ret, &preg, str, size);
        pdftex_warn("%s%s", "\\pdfmatch: ", str);
        xfree(str);
        strpool[poolptr++] = '-';
        strpool[poolptr++] = '1';
    } else {
        str = makecstring(t);
        sub_match_count = ((subcount < 0) ? DEFAULT_SUB_MATCH_COUNT : subcount);
        xfree(pmatch);
        if (sub_match_count > 0) {
            pmatch = xtalloc(sub_match_count, regmatch_t);
        }
        ret = regexec(&preg, str, sub_match_count, pmatch, eflags);
        
        xfree(match_string);
        match_string = xstrdup(str);  /* save searched-in string, used below */
        last_match_succeeded = ret == 0;     /* save whether match succeeded */
        strpool[poolptr++] = ((ret == 0) ? '1' : '0'); /* in string pool too */
    }

    regfree(&preg);
}

/* Implements \pdflastmatch */

void
getmatch(int i)
{
    int size;
    int len = 0;                /* avoid spurious uninitialized warning */

    boolean found
      = i >= 0                  /* should always be so due to pdftex.web */
        && i < sub_match_count  /* if >subcount, not found by definition */
        && match_string != NULL /* first call, and just in case */
        && last_match_succeeded /* if no match, not found */
        && pmatch[i].rm_so >= 0 /* if no starting position, not found */
        && pmatch[i].rm_eo >= pmatch[i].rm_so; /* just in case */

    if (found) {
        len = pmatch[i].rm_eo - pmatch[i].rm_so;
        size = 20 + len;
        /* 20: place for integer number and '->' */
    } else {
        size = 4;
    }

    if (poolptr + size >= poolsize) {
        poolptr = poolsize;
        return;
    }

    if (found) {
        int j = snprintf((char *) &strpool[poolptr], 20, "%d",
                         (int) pmatch[i].rm_so);
        check_nprintf(j, 20);
        poolptr += j;
        strpool[poolptr++] = '-';
        strpool[poolptr++] = '>';
        memcpy(&strpool[poolptr], &match_string[pmatch[i].rm_so], len);
        poolptr += len;
        return;
    }

    strpool[poolptr++] = '-';
    strpool[poolptr++] = '1';
    strpool[poolptr++] = '-';
    strpool[poolptr++] = '>';
}


/* function strips trailing zeros in string with numbers; */
/* leading zeros are not stripped (as in real life) */
char *stripzeros(char *a)
{
    enum { NONUM, DOTNONUM, INT, DOT, LEADDOT, FRAC } s = NONUM, t = NONUM;
    char *p, *q, *r;
    for (p = q = r = a; *p != '\0';) {
        switch (s) {
        case NONUM:
            if (*p >= '0' && *p <= '9')
                s = INT;
            else if (*p == '.')
                s = LEADDOT;
            break;
        case DOTNONUM:
            if (*p != '.' && (*p < '0' || *p > '9'))
                s = NONUM;
            break;
        case INT:
            if (*p == '.')
                s = DOT;
            else if (*p < '0' || *p > '9')
                s = NONUM;
            break;
        case DOT:
        case LEADDOT:
            if (*p >= '0' && *p <= '9')
                s = FRAC;
            else if (*p == '.')
                s = DOTNONUM;
            else
                s = NONUM;
            break;
        case FRAC:
            if (*p == '.')
                s = DOTNONUM;
            else if (*p < '0' || *p > '9')
                s = NONUM;
            break;
        default:;
        }
        switch (s) {
        case DOT:
            r = q;
            break;
        case LEADDOT:
            r = q + 1;
            break;
        case FRAC:
            if (*p > '0')
                r = q + 1;
            break;
        case NONUM:
            if ((t == FRAC || t == DOT) && r != a) {
                q = r--;
                if (*r == '.')  /* was a LEADDOT */
                    *r = '0';
                r = a;
            }
            break;
        default:;
        }
        *q++ = *p++;
        t = s;
    }
    *q = '\0';
    return a;
}

void initversionstring(char **versions)
{
    const_string fmt =
                    "Compiled with libpng %s; using libpng %s\n"
                    "Compiled with zlib %s; using zlib %s\n"
                    "Compiled with %s version %s\n";
    size_t len = strlen(fmt)
                    + strlen(PNG_LIBPNG_VER_STRING) + strlen(png_libpng_ver)
                    + strlen(ZLIB_VERSION) + strlen(zlib_version)
                    + strlen(xpdfString) + strlen(xpdfVersion)
                    + 1;

    /* len will be more than enough, because of the placeholder chars in fmt
       that get replaced by the arguments.  */
    *versions = xmalloc(len);
    sprintf(*versions, fmt,
                    PNG_LIBPNG_VER_STRING, png_libpng_ver,
                    ZLIB_VERSION, zlib_version, xpdfString, xpdfVersion);
}


/*************************************************/
/* Color Stack and Matrix Transformation Support */
/*************************************************/

/*
    In the following array and especially stack data structures are used.
    They have the following properties:
    - They automatically grow dynamically.
    - The size never decreases.
    - The variable with name ending in "size" contains the number how many
      entries the data structure can hold.
    - The variable with name ending in "used" contains the number of
      actually used entries.
    - Memory of strings in stack entries must be allocated and
      freed if the stack is cleared.
*/

/* Color Stack */

#define STACK_INCREMENT 8
#define MAX_COLORSTACKS 32768
/* The colorstack number is stored in two bytes (info field of the node) */
/* Condition (newcolorstack): MAX_COLORSTACKS mod STACK_INCREMENT = 0 */

#define COLOR_DEFAULT "0 g 0 G"
/* literal_modes, see pdftex.web */
#define SET_ORIGIN 0
#define DIRECT_PAGE 1
#define DIRECT_ALWAYS 2

/* remember shipout mode: page/form */
static boolean page_mode;

typedef struct {
    char **page_stack;
    char **form_stack;
    char *page_current;
    char *form_current;
    char *form_init;
    int page_size;
    int form_size;
    int page_used;
    int form_used;
    int literal_mode;
    boolean page_start;
} colstack_type;

static colstack_type *colstacks = NULL;
static int colstacks_size = 0;
static int colstacks_used = 0;

/*
    Initialization is done, if the color stacks are used,
    init_colorstacks() is defined as macro to avoid unnecessary
    procedure calls.
*/
#define init_colorstacks() if (colstacks_size == 0) colstacks_first_init();
static void colstacks_first_init(void)
{
    colstacks_size = STACK_INCREMENT;
    colstacks = xtalloc(colstacks_size, colstack_type);
    colstacks_used = 1;
    colstacks[0].page_stack = NULL;
    colstacks[0].form_stack = NULL;
    colstacks[0].page_size = 0;
    colstacks[0].form_size = 0;
    colstacks[0].page_used = 0;
    colstacks[0].form_used = 0;
    colstacks[0].page_current = xstrdup(COLOR_DEFAULT);
    colstacks[0].form_current = xstrdup(COLOR_DEFAULT);
    colstacks[0].form_init = xstrdup(COLOR_DEFAULT);
    colstacks[0].literal_mode = DIRECT_ALWAYS;
    colstacks[0].page_start = true;
}

int colorstackused(void)
{
    init_colorstacks();
    return colstacks_used;
}

/*
    newcolorstack()
    A new color stack is setup with the given parameters.
    The stack number is returned or -1 in case of error (no room).
*/
int newcolorstack(integer s, integer literal_mode, boolean page_start)
{
    colstack_type *colstack;
    int colstack_num;
    char *str;

    init_colorstacks();

    /* make room */
    if (colstacks_used == MAX_COLORSTACKS) {
        return -1;
    }
    if (colstacks_used == colstacks_size) {
        colstacks_size += STACK_INCREMENT;
        /* If (MAX_COLORSTACKS mod STACK_INCREMENT = 0) then we don't
           need to check the case that size overruns MAX_COLORSTACKS. */
        xretalloc(colstacks, colstacks_size, colstack_type);
    }
    /* claim new color stack */
    colstack_num = colstacks_used++;
    colstack = &colstacks[colstack_num];
    /* configure the new color stack */
    colstack->page_stack = NULL;
    colstack->form_stack = NULL;
    colstack->page_size = 0;
    colstack->page_used = 0;
    colstack->form_size = 0;
    colstack->form_used = 0;
    colstack->literal_mode = literal_mode;
    colstack->page_start = page_start;
    str = makecstring(s);
    if (*str == 0) {
        colstack->page_current = NULL;
        colstack->form_current = NULL;
        colstack->form_init = NULL;
    } else {
        colstack->page_current = xstrdup(str);
        colstack->form_current = xstrdup(str);
        colstack->form_init = xstrdup(str);
    }
    return colstack_num;
}

#define get_colstack(n) (&colstacks[n])

/*
    Puts a string on top of the string pool and updates poolptr.
*/
static void put_cstring_on_strpool(poolpointer start, char *str)
{
    size_t len;

    if (str == NULL || *str == 0) {
        return;
    }

    len = strlen(str);
    poolptr = start + len;
    if (poolptr >= poolsize) {
        poolptr = poolsize;
        /* error by str_toks that calls str_room(1) */
        return;
    }
    memcpy(&strpool[start], str, len);
}

integer colorstackset(int colstack_no, integer s)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        xfree(colstack->page_current);
        colstack->page_current = xstrdup(makecstring(s));
    } else {
        xfree(colstack->form_current);
        colstack->form_current = xstrdup(makecstring(s));
    }
    return colstack->literal_mode;
}

integer colorstackcurrent(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        put_cstring_on_strpool(poolptr, colstack->page_current);
    } else {
        put_cstring_on_strpool(poolptr, colstack->form_current);
    }
    return colstack->literal_mode;
}

integer colorstackpush(int colstack_no, integer s)
{
    colstack_type *colstack = get_colstack(colstack_no);
    char *str;

    if (page_mode) {
        if (colstack->page_used == colstack->page_size) {
            colstack->page_size += STACK_INCREMENT;
            xretalloc(colstack->page_stack, colstack->page_size, char *);
        }
        colstack->page_stack[colstack->page_used++] = colstack->page_current;
        str = makecstring(s);
        if (*str == 0) {
            colstack->page_current = NULL;
        } else {
            colstack->page_current = xstrdup(str);
        }
    } else {
        if (colstack->form_used == colstack->form_size) {
            colstack->form_size += STACK_INCREMENT;
            xretalloc(colstack->form_stack, colstack->form_size, char *);
        }
        colstack->form_stack[colstack->form_used++] = colstack->form_current;
        str = makecstring(s);
        if (*str == 0) {
            colstack->form_current = NULL;
        } else {
            colstack->form_current = xstrdup(str);
        }
    }
    return colstack->literal_mode;
}

integer colorstackpop(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (page_mode) {
        if (colstack->page_used == 0) {
            pdftex_warn("pop empty color page stack %u",
                        (unsigned int) colstack_no);
            return colstack->literal_mode;
        }
        xfree(colstack->page_current);
        colstack->page_current = colstack->page_stack[--colstack->page_used];
        put_cstring_on_strpool(poolptr, colstack->page_current);
    } else {
        if (colstack->form_used == 0) {
            pdftex_warn("pop empty color form stack %u",
                        (unsigned int) colstack_no);
            return colstack->literal_mode;
        }
        xfree(colstack->form_current);
        colstack->form_current = colstack->form_stack[--colstack->form_used];
        put_cstring_on_strpool(poolptr, colstack->form_current);
    }
    return colstack->literal_mode;
}

static void colorstackpagestart(void)
{
    int i, j;
    colstack_type *colstack;

    if (page_mode) {
        /* see procedure pdf_out_colorstack_startpage */
        return;
    }

    for (i = 0; i < colstacks_used; i++) {
        colstack = &colstacks[i];
        for (j = 0; j < colstack->form_used; j++) {
            xfree(colstack->form_stack[j]);
        }
        colstack->form_used = 0;
        xfree(colstack->form_current);
        if (colstack->form_init == NULL) {
            colstack->form_current = NULL;
        } else {
            colstack->form_current = xstrdup(colstack->form_init);
        }
    }
}

integer colorstackskippagestart(int colstack_no)
{
    colstack_type *colstack = get_colstack(colstack_no);

    if (!colstack->page_start) {
        return 1;
    }
    if (colstack->page_current == NULL) {
        return 0;
    }
    if (strcmp(COLOR_DEFAULT, colstack->page_current) == 0) {
        return 2;
    }
    return 0;
}


/* stack for \pdfsetmatrix */

typedef struct {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
} matrix_entry;
static matrix_entry *matrix_stack = 0;
static int matrix_stack_size = 0;
static int matrix_stack_used = 0;

boolean matrixused(void)
{
    return matrix_stack_used > 0;
}

/* stack for positions of \pdfsave */
typedef struct {
    int pos_h;
    int pos_v;
    int matrix_stack;
} pos_entry;
static pos_entry *pos_stack = 0;        /* the stack */
static int pos_stack_size = 0;  /* initially empty */
static int pos_stack_used = 0;  /* used entries */

static void matrix_stack_room(void)
{
    matrix_entry *new_stack;

    if (matrix_stack_used >= matrix_stack_size) {
        matrix_stack_size += STACK_INCREMENT;
        new_stack = xtalloc(matrix_stack_size, matrix_entry);
        memcpy((void *) new_stack, (void *) matrix_stack,
               matrix_stack_used * sizeof(matrix_entry));
        xfree(matrix_stack);
        matrix_stack = new_stack;
    }
}

void checkpdfsave(int cur_h, int cur_v)
{
    pos_entry *new_stack;

    if (pos_stack_used >= pos_stack_size) {
        pos_stack_size += STACK_INCREMENT;
        new_stack = xtalloc(pos_stack_size, pos_entry);
        memcpy((void *) new_stack, (void *) pos_stack,
               pos_stack_used * sizeof(pos_entry));
        xfree(pos_stack);
        pos_stack = new_stack;
    }
    pos_stack[pos_stack_used].pos_h = cur_h;
    pos_stack[pos_stack_used].pos_v = cur_v;
    if (page_mode) {
        pos_stack[pos_stack_used].matrix_stack = matrix_stack_used;
    }
    pos_stack_used++;
}

void checkpdfrestore(int cur_h, int cur_v)
{
    int diff_h, diff_v;
    if (pos_stack_used == 0) {
        pdftex_warn("%s", "\\pdfrestore: missing \\pdfsave");
        return;
    }
    pos_stack_used--;
    diff_h = cur_h - pos_stack[pos_stack_used].pos_h;
    diff_v = cur_v - pos_stack[pos_stack_used].pos_v;
    if (diff_h != 0 || diff_v != 0) {
        pdftex_warn("Misplaced \\pdfrestore by (%usp, %usp)", diff_h, diff_v);
    }
    if (page_mode) {
        matrix_stack_used = pos_stack[pos_stack_used].matrix_stack;
    }
}

void pdfshipoutbegin(boolean shipping_page)
{
    pos_stack_used = 0;         /* start with empty stack */

    page_mode = shipping_page;
    if (shipping_page) {
        colorstackpagestart();
    }
}

void pdfshipoutend(boolean shipping_page)
{
    if (pos_stack_used > 0) {
        pdftex_fail("%u unmatched \\pdfsave after %s shipout",
                    (unsigned int) pos_stack_used,
                    ((shipping_page) ? "page" : "form"));
    }
}

/*
    \pdfsetmatrix{a b c d}
    e := cur_h
    f := cur_v
    M_top: current active matrix at the top of
           the matrix stack

    The origin of \pdfsetmatrix is the current point.
    The annotation coordinate system is the original
    page coordinate system. When pdfTeX calculates
    annotation rectangles it does not take into
    account this transformations, it uses the original
    coordinate system. To get the corrected values,
    first we go back to the origin, perform the
    transformation and go back:

    (  1   0  0 )   ( a b 0 )   ( 1 0 0 )
    (  0   1  0 ) x ( c d 0 ) x ( 0 1 0 ) x M_top
    ( -e  -f  1 )   ( 0 0 1 )   ( e f 1 )

    ( 1  0  0 )   (  a  b 0 )
  = ( 0  1  0 ) x (  c  d 0 ) x M_top
    ( e  f  1 )   ( -e -f 1 )

    ( a         b         0 )
  = ( c         d         0 ) x M_top
    ( e(1-a)-fc f(1-d)-eb 1 )

*/

integer pdfsetmatrix(poolpointer in, scaled cur_h, scaled cur_v)
{
    /* Argument of \pdfsetmatrix starts with strpool[in] and ends
       before strpool[poolptr]. */

    matrix_entry x, *y, *z;
    char dummy;

    if (page_mode) {
        if (sscanf((const char *) &strpool[in], " %lf %lf %lf %lf %c",
                   &x.a, &x.b, &x.c, &x.d, &dummy) != 4) {
            return 0; /* failure */
        }
        /* calculate this transformation matrix */
        x.e = (double) cur_h *(1.0 - x.a) - (double) cur_v *x.c;
        x.f = (double) cur_v *(1.0 - x.d) - (double) cur_h *x.b;
        matrix_stack_room();
        z = &matrix_stack[matrix_stack_used];
        if (matrix_stack_used > 0) {
            y = &matrix_stack[matrix_stack_used - 1];
            z->a = x.a * y->a + x.b * y->c;
            z->b = x.a * y->b + x.b * y->d;
            z->c = x.c * y->a + x.d * y->c;
            z->d = x.c * y->b + x.d * y->d;
            z->e = x.e * y->a + x.f * y->c + y->e;
            z->f = x.e * y->b + x.f * y->d + y->f;
        } else {
            z->a = x.a;
            z->b = x.b;
            z->c = x.c;
            z->d = x.d;
            z->e = x.e;
            z->f = x.f;
        }
        matrix_stack_used++;
    }
    return 1; /* success */
}

/* Apply matrix to point (x,y)

               ( a b 0 )
   ( x y 1 ) x ( c d 0 ) = ( xa+yc+e xb+yd+f 1 )
               ( e f 1 )

   If \pdfsetmatrix wasn't used, then return the value unchanged.
*/

/* Return valeus for matrix tranform functions */
static scaled ret_llx;
static scaled ret_lly;
static scaled ret_urx;
static scaled ret_ury;

scaled getllx(void)
{
    return ret_llx;
}

scaled getlly(void)
{
    return ret_lly;
}

scaled geturx(void)
{
    return ret_urx;
}

scaled getury(void)
{
    return ret_ury;
}

static int last_llx;
static int last_lly;
static int last_urx;
static int last_ury;

#define DO_ROUND(x)  ((x > 0) ? (x + .5) : (x - .5))
#define DO_MIN(a, b) ((a < b) ? a : b)
#define DO_MAX(a, b) ((a > b) ? a : b)

static void do_matrixtransform(scaled x, scaled y, scaled * retx, scaled * rety)
{
    matrix_entry *m = &matrix_stack[matrix_stack_used - 1];
    double x_old = x;
    double y_old = y;
    double x_new = x_old * m->a + y_old * m->c + m->e;
    double y_new = x_old * m->b + y_old * m->d + m->f;
    *retx = (scaled) DO_ROUND(x_new);
    *rety = (scaled) DO_ROUND(y_new);
}

void matrixtransformrect(scaled llx, scaled lly, scaled urx, scaled ury)
{
    scaled x1, x2, x3, x4, y1, y2, y3, y4;

    if (page_mode && matrix_stack_used > 0) {
        last_llx = llx;
        last_lly = lly;
        last_urx = urx;
        last_ury = ury;
        do_matrixtransform(llx, lly, &x1, &y1);
        do_matrixtransform(llx, ury, &x2, &y2);
        do_matrixtransform(urx, lly, &x3, &y3);
        do_matrixtransform(urx, ury, &x4, &y4);
        ret_llx = DO_MIN(DO_MIN(x1, x2), DO_MIN(x3, x4));
        ret_lly = DO_MIN(DO_MIN(y1, y2), DO_MIN(y3, y4));
        ret_urx = DO_MAX(DO_MAX(x1, x2), DO_MAX(x3, x4));
        ret_ury = DO_MAX(DO_MAX(y1, y2), DO_MAX(y3, y4));
    } else {
        ret_llx = llx;
        ret_lly = lly;
        ret_urx = urx;
        ret_ury = ury;
    }
}

void matrixtransformpoint(scaled x, scaled y)
{
    if (page_mode && matrix_stack_used > 0) {
        do_matrixtransform(x, y, &ret_llx, &ret_lly);
    } else {
        ret_llx = x;
        ret_lly = y;
    }
}

void matrixrecalculate(scaled urx)
{
    matrixtransformrect(last_llx, last_lly, urx, last_ury);
}

void allocvffnts(void)
{
    if (vf_e_fnts_array == NULL) {
        vf_e_fnts_array = vfefnts;
        vf_e_fnts_limit = fontmax;
        vf_e_fnts_ptr = vf_e_fnts_array;
        vf_i_fnts_array = vfifnts;
        vf_i_fnts_limit = fontmax;
        vf_i_fnts_ptr = vf_i_fnts_array;
    }
    alloc_array(vf_e_fnts, 1, fontmax);
    vf_e_fnts_ptr++;
    alloc_array(vf_i_fnts, 1, fontmax);
    vf_i_fnts_ptr++;
    if (vf_e_fnts_array != vfefnts) {
        vfefnts = vf_e_fnts_array;
        vfifnts = vf_i_fnts_array;
    }
}


