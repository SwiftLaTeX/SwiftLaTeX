// First of all, remove pdf_files_init/close from xetex-ini.c
// And remove picture handling functions from xetex-pic.c
#include "core-bridge.h"
#ifdef NATIVE_BUILD
#include <curl/curl.h>
#endif
#include <md5.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "core-memory.h"
void issue_warning(void *context, char const *text) {
  fprintf(stderr, "%s\n", text);
}

void issue_error(void *context, char const *text) {
  fprintf(stderr, "%s\n", text);
}

int get_file_md5(void *context, char const *path, char *digest) {

  md5_state_t state;
#define FILE_BUF_SIZE 128
  char file_buf[FILE_BUF_SIZE];
  size_t read;
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    fprintf(stderr, "This file does not exists %s\n, so returning an empty md5",
            path);
    return -1;
  }

  md5_init(&state);
  while ((read = fread(&file_buf, sizeof(char), FILE_BUF_SIZE, f)) > 0) {
    md5_append(&state, (const md5_byte_t *)file_buf, read);
  }
  md5_finish(&state, digest);
  fclose(f);
#undef FILE_BUF_SIZE
  return 0;
}
int get_data_md5(void *context, char const *data, size_t len, char *digest) {

  md5_state_t state;
  md5_init(&state);
  md5_append(&state, data, len);
  md5_finish(&state, digest);
  return 0;
}
void *output_open(void *context, char const *path, int is_gz) {
  return fopen(path, "w");
}

void *output_open_stdout(void *context) { return stdout; }

int output_putc(void *context, void *handle, int c) {
  // printf("output_putc\n");
  return fputc(c, handle);
}

size_t output_write(void *context, void *handle, const char *data, size_t len) {
  // printf("output_write\n");
  return fwrite(data, 1, len, handle);
}

int output_flush(void *context, void *handle) {
  // printf("output_fflush\n");
  return fflush(handle);
}

int output_close(void *context, void *handle) { return fclose(handle); }

#define MAXAVAILABLENAMESIZE 512

#ifndef NATIVE_BUILD
#define TEXCACHEROOT "/tex/"
int kpse_fetch_from_network(char *name_ret);
static int existsInCacheDir(char *name)
{
    char kpseurl[MAXAVAILABLENAMESIZE] = {0};
    sprintf(kpseurl, "%s%s", TEXCACHEROOT, name);
    if( access( kpseurl, F_OK ) != -1 ) {
        //printf("file available there %s\n", kpseurl);
        strcpy(name, kpseurl);
        return 1;
    }
    //printf("file no there %s\n", kpseurl);
    return -1;
}
#else

#define KPSE_BASE_URL "http://127.0.0.1:5000/tex"
// #include <emscripten/fetch.h>
static size_t curl_write_data(void *ptr, size_t size, size_t nmemb,
                              void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

static int kpse_fetch_from_network(char *name_ret) {

  // If "/" or " " in the name_ret, failed instantly

  if (strrchr(name_ret, '/') != NULL) {
    return -1;
  }

  CURL *curl;
  CURLcode curl_res;
  FILE *destfile;

  int res = 0;
  char name_copy[MAXAVAILABLENAMESIZE] = {0};
  char name_url[MAXAVAILABLENAMESIZE] = {0};

  char const *tmpfolder = getenv("TEMP");
  if (tmpfolder == 0)
    tmpfolder = "/tmp";

  // Check 404 Cache
  strcpy(name_copy, name_ret);
  snprintf(name_ret, MAXAVAILABLENAMESIZE, "%s/%s.404", tmpfolder, name_copy);
  if (access(name_ret, F_OK) != -1) {
    return -1;
  }

  // Check 200 Cache
  snprintf(name_ret, MAXAVAILABLENAMESIZE, "%s/%s", tmpfolder, name_copy);
  if (access(name_ret, F_OK) != -1) {
    return 0;
  }

  destfile = fopen(name_ret, "wb");
  if (destfile) {
    curl = curl_easy_init();
    if (curl) {
      snprintf(name_url, MAXAVAILABLENAMESIZE, "%s/%s", KPSE_BASE_URL,
               name_copy);
      curl_easy_setopt(curl, CURLOPT_URL, name_url);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, destfile);
      curl_res = curl_easy_perform(curl);
      if (curl_res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200) {
          res = 0;
        } else if (response_code == 301) {
          char name_404[MAXAVAILABLENAMESIZE] = {0};
          snprintf(name_404, MAXAVAILABLENAMESIZE, "%s/%s.404", tmpfolder,
                   name_copy);
          FILE *error404File = fopen(name_404, "wb");
          if (error404File) {
            fclose(error404File);
          }
          printf("File not exists in remote server.\n");
          res = -1;
        } else {
          printf("Server error %ld.\n", response_code);
          res = -2;
        }
      } else {
        printf("Curl perform failed\n");
        res = -3;
      }
      curl_easy_cleanup(curl);
    } else {
      printf("No curl support\n");
      res = -4;
    }
    fclose(destfile);
    if (res != 0) {
      unlink(name_ret);
    }
    return res;
  } else {
    printf("Unable to write file %s\n", name_ret);
    return -1;
  }
}
#endif

static void fix_extension(char *patched_name, tt_input_format_type format)
{
#define SUFFIX(suf) strcat(patched_name, suf);
  
    switch (format) {
    case TTIF_TFM:
      SUFFIX(".tfm");
      break;
    case TTIF_AFM:
      SUFFIX(".afm");
      break;
    case TTIF_BIB:
      SUFFIX(".bib");
      break;
    case TTIF_BST:
      SUFFIX(".bst");
      break;
    case TTIF_CNF:
      SUFFIX(".cnf");
      break;
    case TTIF_FORMAT:
      SUFFIX(".fmt");
      break;
    case TTIF_FONTMAP:
      SUFFIX(".fontmap");
      break;
    case TTIF_OFM:
      SUFFIX(".ofm");
      break;
    case TTIF_OVF:
      SUFFIX(".ovf");
      break;
    case TTIF_TEX:
      SUFFIX(".tex");
      break;
    case TTIF_TYPE1:
      SUFFIX(".pfb");
      break;
    case TTIF_VF:
      SUFFIX(".vf");
      break;
    case TTIF_TRUETYPE:
      SUFFIX(".ttf");
      break;
    case TTIF_ENC:
      SUFFIX(".enc");
      break;
    case TTIF_CMAP:
      SUFFIX(".cmap");
      break;
    case TTIF_SFD:
      SUFFIX(".sfd");
      break;
    case TTIF_OPENTYPE:
      SUFFIX(".otf");
      break;
    default:
      fprintf(stderr, "Unknown format\n");
      break;
    }
#undef SUFFIX

}


char *kpse_find_file(const char *name, tt_input_format_type format) {
 //printf("Looking for %s\n", name);
  if (name == NULL) {
    return NULL;
  }

  if (strlen(name) > MAXAVAILABLENAMESIZE / 2) {
    return NULL;
  }

  char* patched_name = xmalloc(MAXAVAILABLENAMESIZE);

  strcat(patched_name, name);

  if (access(patched_name, F_OK) != -1) {
    return patched_name;
  }

  if (strrchr(patched_name, '.') == NULL) {
    fix_extension(patched_name, format);
  }

  //Try again2
  if (access(patched_name, F_OK) != -1) {
    return patched_name;
  }

  #ifndef NATIVE_BUILD
  if(existsInCacheDir(patched_name) == 1)
  {
      return patched_name;
  }
  #endif
  
  if (kpse_fetch_from_network(patched_name) ==
      0) // Nope Try download it //Now we can ask for help to download the file
  {
    //printf("kpse_returning %s\n", patched_name);
    return patched_name;
  }

  free(patched_name); // We try out bese, just leave it
  return NULL;
}

void *input_open(void *context, char const *path, tt_input_format_type format,
                 int is_gz) {
  //fprintf(stderr, "Opening %s format %d\n", path, format);
  char *normalized_path = kpse_find_file(path, format);
  if (normalized_path != NULL) {
    FILE *res = fopen(normalized_path, "rb");
    free(normalized_path);
    return res;
  } else {
    return NULL;
  }
}

size_t input_get_size(void *context, void *handle) {
  int fpno = fileno(handle);
  struct stat st;
  fstat(fpno, &st);
  return st.st_size;
}

size_t input_seek(void *context, void *handle, ssize_t offset, int whence,
                  int *internal_error) {

  if (whence != SEEK_SET) {
    fprintf(stderr, "Only allowed SEEK_SET\n");
    *internal_error = 1;
    return -1;
  }

  int seek_res = fseek(handle, offset, whence);
  if (seek_res != 0) {
    fprintf(stderr, "seek failed  %d\n", seek_res);
    *internal_error = 1;
    return -1;
  } else {
    return offset;
  }
}

ssize_t input_read(void *context, void *handle, char *data, size_t len) {
  // printf("input_read\n");
  return fread(data, 1, len, handle);
}

int input_getc(void *context, void *handle) {
  // printf("input_getc\n");
  return fgetc(handle);
}

int input_ungetc(void *context, void *handle, int ch) {

  return ungetc(ch, handle);
}

int input_close(void *context, void *handle) { return fclose(handle); }


tt_bridge_api_t ourapi;
int main() {
  ourapi.issue_warning = issue_warning;
  ourapi.issue_error = issue_error;
  ourapi.get_file_md5 = get_file_md5;
  ourapi.get_data_md5 = get_data_md5;
  ourapi.output_open = output_open;
  ourapi.output_open_stdout = output_open_stdout;
  ourapi.output_putc = output_putc;
  ourapi.output_write = output_write;
  ourapi.output_close = output_close;
  ourapi.output_flush = output_flush;
  ourapi.input_open = input_open;
  ourapi.input_get_size = input_get_size;
  ourapi.input_seek = input_seek;
  ourapi.input_read = input_read;
  ourapi.input_getc = input_getc;
  ourapi.input_ungetc = input_ungetc;
  ourapi.input_close = input_close;
  return 0;
}

int compileLaTeX() {  
  return tex_simple_main(&ourapi, "swiftlatex.fmt", "_input_.tex", 0);
}

int comileBibtex() {
  return bibtex_simple_main(&ourapi, "_input_.aux");
}

