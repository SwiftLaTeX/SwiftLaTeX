#define EXTERN extern /* Instantiate data from pdftexd.h here.  */

#include <xetexd.h>

#include <errno.h>
#include <md5.h>
#include <setjmp.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
string fullnameoffile;
string output_directory;
int tfmtemp;
int texinputtype;
int kpse_make_tex_discard_errors;
string translate_filename;
const_string c_job_name;
char start_time_str[32];
char *last_source_name;
int last_lineno;

#define check_nprintf(size_get, size_want)                                     \
  if ((unsigned)(size_get) >= (unsigned)(size_want)) {                         \
    fprintf(stderr, "snprintf failed: file %s, line %d", __FILE__, __LINE__);  \
    abort();                                                                   \
  }

static int compare_paths(const_string p1, const_string p2) {
  int ret;
  while ((((ret = (*p1 - *p2)) == 0) && (*p2 != 0))

         || (IS_DIR_SEP(*p1) && IS_DIR_SEP(*p2))) {
    p1++, p2++;
  }
  ret = (ret < 0 ? -1 : (ret > 0 ? 1 : 0));
  return ret;
}

boolean isnewsource(strnumber srcfilename, int lineno) {
  char *name = gettexstring(srcfilename);
  return (compare_paths(name, last_source_name) != 0 || lineno != last_lineno);
}

static void checkpoolpointer(poolpointer poolptr, size_t len) {
  if (poolptr + len >= poolsize) {
    fprintf(stderr, "\nstring pool overflow [%i bytes]\n",
            (int)poolsize); /* fixme */
    exit(1);
  }
}

int maketexstring(const_string s) {
  size_t len;

  UInt32 rval;
  const unsigned char *cp = (const unsigned char *)s;

  if (s == NULL || *s == 0)
    return getnullstr();

  len = strlen(s);
  checkpoolpointer(poolptr,
                   len); /* in the XeTeX case, this may be more than enough */

  while ((rval = *(cp++)) != 0) {
    UInt16 extraBytes = bytesFromUTF8[rval];
    switch (extraBytes) { /* note: code falls through cases! */
    case 5:
      rval <<= 6;
      if (*cp)
        rval += *(cp++);
    case 4:
      rval <<= 6;
      if (*cp)
        rval += *(cp++);
    case 3:
      rval <<= 6;
      if (*cp)
        rval += *(cp++);
    case 2:
      rval <<= 6;
      if (*cp)
        rval += *(cp++);
    case 1:
      rval <<= 6;
      if (*cp)
        rval += *(cp++);
    case 0:;
    };
    rval -= offsetsFromUTF8[extraBytes];
    if (rval > 0xffff) {
      rval -= 0x10000;
      strpool[poolptr++] = 0xd800 + rval / 0x0400;
      strpool[poolptr++] = 0xdc00 + rval % 0x0400;
    } else
      strpool[poolptr++] = rval;
  }

  return makestring();
}

int runsystem(const char *noused) { return 0; }

void get_seconds_and_micros(integer *seconds, integer *micros) {

  struct timeval tv;
  gettimeofday(&tv, NULL);
  *seconds = tv.tv_sec;
  *micros = tv.tv_usec;
}

void maketimestr(char *time_str) {
  time_t start_time = time((time_t *)NULL);
  struct tm lt;
  lt = *localtime(&start_time);
  size_t size = strftime(time_str, 31, "D:%Y%m%d%H%M%S", &lt);

  if (size == 0) {
    time_str[0] = '\0';
    return;
  }

  if (time_str[14] == '6') {
    time_str[14] = '5';
    time_str[15] = '9';
    time_str[16] = '\0'; /* for safety */
  }

  time_str[size++] = 'Z';
  time_str[size] = 0;
}

void initstarttime() {
  if (start_time_str[0] == '\0') {
    maketimestr(start_time_str);
  }
}

void convertStringToHexString(const char *in, char *out, int lin) {
  int i, j, k;
  char buf[3];
  j = 0;
  for (i = 0; i < lin; i++) {
    k = snprintf(buf, sizeof(buf), "%02X", (unsigned int)(unsigned char)in[i]);
    out[j++] = buf[0];
    out[j++] = buf[1];
  }
  out[j] = '\0';
}

void calledit(packedASCIIcode *filename, poolpointer fnstart, integer fnlength,
              integer linenumber) {
  fprintf(stderr, "Never calledit.. aborting\n");
  abort();
}

void get_date_and_time(integer *minutes, integer *day, integer *month,
                       integer *year) {
  struct tm *tmptr;

  /* whether the envvar was not set (usual case) or invalid,
     use current time.  */
  time_t myclock = time((time_t *)0);
  tmptr = localtime(&myclock);

  *minutes = tmptr->tm_hour * 60 + tmptr->tm_min;
  *day = tmptr->tm_mday;
  *month = tmptr->tm_mon + 1;
  *year = tmptr->tm_year + 1900;
}

strnumber getjobname(strnumber name) {
  strnumber ret = name;
  if (c_job_name != NULL)
    ret = maketexstring(c_job_name);
  return ret;
}

strnumber makefullnamestring(void) { return maketexstring(fullnameoffile); }

strnumber makeinputoutputnamestring(void) {
  return maketexstring(nameoffile + 1);
}

void getcreationdate(void) {
  size_t len;

  initstarttime();
  /* put creation date on top of string pool and update poolptr */
  len = strlen(start_time_str);
  if ((unsigned)(poolptr + len) >= (unsigned)(poolsize)) {
    poolptr = poolsize;
    /* error by str_toks that calls str_room(1) */
    return;
  }
  memcpy(&strpool[poolptr], start_time_str, len);
  poolptr += len;
}

void getfilemoddate(integer s) {
  struct stat file_data;
  int i = 0;
  const_string orig_name = gettexstring(s);

  char *file_name = kpse_find_tex(orig_name);
  if (file_name == NULL) {
    return; /* empty string */
  }
  if (!kpse_in_name_ok(file_name)) {
    return; /* no permission */
  }

  recorder_record_input(file_name);
  /* get file status */

  if (stat(file_name, &file_data) == 0) {

    size_t len;
    char time_str[32];
    maketimestr(time_str);
    len = strlen(time_str);
    if ((unsigned)(poolptr + len) >= (unsigned)(poolsize)) {
      poolptr = poolsize;
      /* error by str_toks that calls str_room(1) */
    } else {
      for (i = 0; i < len; i++)
        strpool[poolptr++] = (uint16_t)time_str[i];
    }
  }
  /* else { errno contains error code } */

  free(file_name);
}

void getfilesize(integer s) {
  struct stat file_data;
  int i;

  char *file_name = kpse_find_tex(gettexstring(s));

  if (file_name == NULL) {
    return; /* empty string */
  }
  if (!kpse_in_name_ok(file_name)) {
    return; /* no permission */
  }

  recorder_record_input(file_name);
  /* get file status */

  if (stat(file_name, &file_data) == 0) {

    size_t len;
    char buf[20];
    /* st_size has type off_t */
    i = snprintf(buf, sizeof(buf), "%lu", (long unsigned int)file_data.st_size);
    len = strlen(buf);
    if ((unsigned)(poolptr + len) >= (unsigned)(poolsize)) {
      poolptr = poolsize;
      /* error by str_toks that calls str_room(1) */
    } else {
      for (i = 0; i < len; i++)
        strpool[poolptr++] = (uint16_t)buf[i];
    }
  }
  /* else { errno contains error code } */

  free(file_name);
}

#define DIGEST_SIZE 16
#define FILE_BUF_SIZE 1024

void getmd5sum(strnumber s, boolean file) {
  md5_state_t state;
  md5_byte_t digest[DIGEST_SIZE];
  char outbuf[2 * DIGEST_SIZE + 1];
  int len = 2 * DIGEST_SIZE;
  char *xname;
  int i;

  if (file) {
    char file_buf[FILE_BUF_SIZE];
    int read = 0;
    FILE *f;
    char *file_name;

    xname = gettexstring(s);
    file_name = kpse_find_tex(xname);
    free(xname);

    if (file_name == NULL) {
      return; /* empty string */
    }
    if (!kpse_in_name_ok(file_name)) {
      return; /* no permission */
    }

    /* in case of error the empty string is returned,
       no need for xfopen that aborts on error.
     */
    f = fopen(file_name, FOPEN_RBIN_MODE);
    if (f == NULL) {
      free(file_name);
      return;
    }
    recorder_record_input(file_name);
    md5_init(&state);
    while ((read = fread(&file_buf, sizeof(char), FILE_BUF_SIZE, f)) > 0) {
      md5_append(&state, (const md5_byte_t *)file_buf, read);
    }
    md5_finish(&state, digest);
    fclose(f);

    free(file_name);
  } else {
    /* s contains the data */
    md5_init(&state);

    xname = gettexstring(s);
    md5_append(&state, (md5_byte_t *)xname, strlen(xname));
    free(xname);
    md5_finish(&state, digest);
  }

  if (poolptr + len >= poolsize) {
    /* error by str_toks that calls str_room(1) */
    return;
  }
  convertStringToHexString((char *)digest, outbuf, DIGEST_SIZE);

  for (i = 0; i < 2 * DIGEST_SIZE; i++)
    strpool[poolptr++] = (uint16_t)outbuf[i];
}

void getfiledump(integer s, int offset, int length) {
  FILE *f;
  int read, i;
  char *readbuffer, strbuf[3];
  int j, k;
  char *file_name;

  if (length == 0) {
    /* empty result string */
    return;
  }

  if (poolptr + 2 * length + 1 >= poolsize) {
    /* no place for result */
    poolptr = poolsize;
    /* error by str_toks that calls str_room(1) */
    return;
  }

  file_name = kpse_find_tex(gettexstring(s));
  if (file_name == NULL) {
    return; /* empty string */
  }
  if (!kpse_in_name_ok(file_name)) {
    return; /* no permission */
  }

  /* read file data */
  f = fopen(file_name, FOPEN_RBIN_MODE);
  if (f == NULL) {
    free(file_name);
    return;
  }
  recorder_record_input(file_name);
  if (fseek(f, offset, SEEK_SET) != 0) {
    free(file_name);
    return;
  }

  readbuffer = (char *)xmalloc(length + 1);
  read = fread(readbuffer, sizeof(char), length, f);
  fclose(f);
  for (j = 0; j < read; j++) {
    i = snprintf(strbuf, 3, "%.2X", (unsigned int)readbuffer[j]);
    check_nprintf(i, 3);
    for (k = 0; k < i; k++)
      strpool[poolptr++] = (uint16_t)strbuf[k];
  }
  free(readbuffer);

  free(file_name);
}

string gettexstring(strnumber s) {
  unsigned bytesToWrite = 0;
  poolpointer len, i, j;
  string name;
  if (s >= 65536L)
    len = strstart[s + 1 - 65536L] - strstart[s - 65536L];
  else
    len = 0;
  name = xmalloc(len * 3 + 1); /* max UTF16->UTF8 expansion
                                  (code units, not bytes) */
  for (i = 0, j = 0; i < len; i++) {
    unsigned c = strpool[i + strstart[s - 65536L]];
    if (c >= 0xD800 && c <= 0xDBFF) {
      unsigned lo = strpool[++i + strstart[s - 65536L]];
      if (lo >= 0xDC00 && lo <= 0xDFFF)
        c = (c - 0xD800) * 0x0400 + lo - 0xDC00;
      else
        c = 0xFFFD;
    }
    if (c < 0x80)
      bytesToWrite = 1;
    else if (c < 0x800)
      bytesToWrite = 2;
    else if (c < 0x10000)
      bytesToWrite = 3;
    else if (c < 0x110000)
      bytesToWrite = 4;
    else {
      bytesToWrite = 3;
      c = 0xFFFD;
    }

    j += bytesToWrite;
    switch (bytesToWrite) { /* note: everything falls through. */
    case 4:
      name[--j] = ((c | 0x80) & 0xBF);
      c >>= 6;
    case 3:
      name[--j] = ((c | 0x80) & 0xBF);
      c >>= 6;
    case 2:
      name[--j] = ((c | 0x80) & 0xBF);
      c >>= 6;
    case 1:
      name[--j] = (c | firstByteMark[bytesToWrite]);
    }
    j += bytesToWrite;
  }
  name[j] = 0;
  return name;
}

void remembersourceinfo(strnumber srcfilename, int lineno) {
  if (last_source_name) {
    free(last_source_name);
  }
  last_source_name = gettexstring(srcfilename);
  last_lineno = lineno;
}

poolpointer makesrcspecial(strnumber srcfilename, int lineno) {
  poolpointer oldpoolptr = poolptr;
  char *filename = gettexstring(srcfilename);
  /* FIXME: Magic number. */
  char buf[40];
  char *s = buf;

  /* Always put a space after the number, which makes things easier
   * to parse.
   */
  sprintf(buf, "src:%d ", lineno);

  if (poolptr + strlen(buf) + strlen(filename) >= (size_t)poolsize) {
    fprintf(stderr, "\nstring pool overflow\n"); /* fixme */
    exit(1);
  }
  s = buf;
  while (*s)
    strpool[poolptr++] = *s++;

  s = filename;
  while (*s)
    strpool[poolptr++] = *s++;

  return (oldpoolptr);
}
