#ifndef MIN_PDFTEX_TEXMFMP
#define MIN_PDFTEX_TEXMFMP
#include "w2c/config.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// XeTeX Define

#define TEXMFPOOLNAME "xetex.pool"
#define TEXMFENGINENAME "xetex"
typedef FILE *gzFile;
typedef struct UFILE {
  FILE *f;
  long savedChar;
  short skipNextLF;
  short encodingMode;
  void *conversionData;
} UFILE;
typedef struct UFILE *unicodefile;
typedef void *voidpointer;

// File
#define FOPEN_W_MODE "wb"
#define FOPEN_R_MODE "rb"
#define FOPEN_RBIN_MODE "rb"
#define FOPEN_WBIN_MODE "wb"

#define recorderchangefilename recorder_change_filename
#define ISDIRSEP IS_DIR_SEP
#define IS_DIR_SEP(ch) ((ch) == '/')

#define writedvi(a,b)  {if(nopdfoutput){WRITE_OUT(a, b)}}
#define WRITE_OUT(a, b)             \
  if ((size_t) fwrite ((char *) &OUT_BUF[a], sizeof (OUT_BUF[a]),       \
                    (size_t) ((size_t)(b) - (size_t)(a) + 1), OUT_FILE) \
      != (size_t) ((size_t) (b) - (size_t) (a) + 1))                    \
    fprintf (stderr, "fwrite failed");   \

#define aopenin(f, p) open_input(&(f), p, FOPEN_RBIN_MODE)
#define aopenout(f) open_output(&(f), FOPEN_WBIN_MODE)
#define aclose close_file
#define bopenin(f) open_input(&(f), kpse_tfm_format, FOPEN_RBIN_MODE)
#define bopenout(f) open_output(&(f), FOPEN_WBIN_MODE)
#define bclose aclose
#define wopenin(f) open_input(&(f), DUMP_FORMAT, FOPEN_RBIN_MODE)
#define wopenout bopenout
#define wclose aclose
#define uopenin(f, p, m, d) u_open_in(&(f), p, FOPEN_RBIN_MODE, m, d)
#define uclose(f) u_close_inout(&(f))
#define Fputs(f, s) (void)fputs(s, f)
#define inputln(stream, flag) input_line(stream)

#define dumpint(x)                                                             \
  do {                                                                         \
    integer x_val = (x);                                                       \
    generic_dump(x_val);                                                       \
  } while (0)

#define undumpint generic_undump
#define generic_dump(x) dumpthings(x, 1)
#define generic_undump(x) undumpthings(x, 1)
#define DUMP_FILE fmtfile
#define dumpthings(base, len)                                                  \
  do_dump((char *)&(base), sizeof(base), (int)(len), DUMP_FILE)
#define undumpthings(base, len)                                                \
  do_undump((char *)&(base), sizeof(base), (int)(len), DUMP_FILE)
#define dumpwd generic_dump
#define dumphh generic_dump
#define dumpqqqq generic_dump
#define undumpwd generic_undump
#define undumphh generic_undump
#define undumpqqqq generic_undump

#define undumpcheckedthings(low, high, base, len)                              \
  do {                                                                         \
    unsigned i;                                                                \
    undumpthings(base, len);                                                   \
    for (i = 0; i < (len); i++) {                                              \
      if ((&(base))[i] < (low) || (&(base))[i] > (high)) {                     \
        fprintf(stderr,                                                        \
                "Item %u (=%" PRIdPTR ") of .fmt array at %" PRIxPTR           \
                " <%" PRIdPTR " or >%" PRIdPTR,                                \
                i, (uintptr_t)(&(base))[i], (uintptr_t) & (base),              \
                (uintptr_t)low, (uintptr_t)high);                              \
      }                                                                        \
    }                                                                          \
  } while (0)

/* Like undump_checked_things, but only check the upper value. We use
   this when the base type is unsigned, and thus all the values will be
   greater than zero by definition.  */
#define undumpuppercheckthings(high, base, len)                                \
  do {                                                                         \
    unsigned i;                                                                \
    undumpthings(base, len);                                                   \
    for (i = 0; i < (len); i++) {                                              \
      if ((&(base))[i] > (high)) {                                             \
        fprintf(stderr,                                                        \
                "Item %u (=%" PRIdPTR ") of .fmt array at %" PRIxPTR           \
                " >%" PRIdPTR,                                                 \
                i, (uintptr_t)(&(base))[i], (uintptr_t) & (base),              \
                (uintptr_t)high);                                              \
      }                                                                        \
    }                                                                          \
  } while (0)

// Misc
#define translatefilename translate_filename
#define promptfilenamehelpmsg "(Press Enter to retry, or Control-D to exit"
#define stringcast(x) ((string)(x))
#define maxint 0x3FFFFFFF
#define ucharcast(x) ((unsigned char)(x))
#define conststringcast(x) ((const_string)(x))
#define printcstring(STR)                                                      \
  do {                                                                         \
    const_string ch_ptr = (STR);                                               \
    while (*ch_ptr)                                                            \
      printchar(*(ch_ptr++));                                                  \
  } while (0)
#define OUT_FILE dvifile
#define OUT_BUF dvibuf


#define secondsandmicros(i, j) get_seconds_and_micros(&(i), &(j))
#define WEB2C_NORETURN
#define Xchr(x) xchr[x]
#define incr(x) ++(x)
#define decr(x) --(x)
#define odd(x) ((x)&1)
#define ord(x) (x)
#define chr(x) (x)

#define putbyte(x, f)                                                          \
  do {                                                                         \
    if (putc((char)(x)&255, f) == EOF)                                         \
      fprintf(stderr, "putbyte(%ld) failed", (long)x);                         \
  } while (0)
#define addressof(x) (&(x))
#define nil NULL

#define dateandtime(i, j, k, l) get_date_and_time(&(i), &(j), &(k), &(l))

// Function Define
// File Function
// extern int input_line (UFILE *);
extern void recorder_change_filename(string new_name);
extern void recorder_record_input(const_string name);
extern void recorder_record_output(const_string name);
extern boolean dir_p(string fn);
extern boolean open_output(FILE **, const_string fopen_mode);
extern boolean open_input(FILE **, int, const_string fopen_mode);
extern boolean bibopenout(FILE **, const_string name);
extern boolean bibopenin(FILE **, int, const_string name);
extern void close_file(FILE *);
extern void topenin(void);
extern void do_dump(char *, int, int, FILE *);
extern void do_undump(char *, int, int, FILE *);

// Memory Function

// Misc Function
extern void get_seconds_and_micros(integer *, integer *);
extern void getcreationdate(void);
extern void getfilemoddate(integer s);
extern void getfilesize(integer s);
extern void getfiledump(integer s, int offset, int length);
extern void uexit(int code);
extern int runsystem(const char *);

extern char *makecstring(integer s);
extern char *makecfilename(integer s);

extern void readtcxfile(void);
extern void initstarttime(void);
extern int loadpoolstrings(integer);
extern void setupboundvariable(integer *, const_string, integer);
extern void get_date_and_time(integer *, integer *, integer *, integer *);

extern const char *ptexbanner;
extern char start_time_str[];
extern string translate_filename;
extern string versionstring;
extern int tfmtemp, texinputtype, kpse_make_tex_discard_errors;
extern string fullnameoffile, output_directory;
extern void getmd5sum(integer s, boolean file);

#endif
