#define EXTERN extern
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <xetexd.h>
#include <stdlib.h>
#include <libgen.h>
void setupboundvariable(integer *var, const_string var_name, integer dflt) {

  *var = dflt;

  if (strcmp(var_name, "main_memory") == 0) {
    *var = 5000000;
  } else if (strcmp(var_name, "extra_mem_top") == 0) {
    *var = 0;
  } else if (strcmp(var_name, "extra_mem_bot") == 0) {
    *var = 0;
  } else if (strcmp(var_name, "pool_size") == 0) {
    *var = 6250000;
  } else if (strcmp(var_name, "string_vacancies") == 0) {
    *var = 90000;
  } else if (strcmp(var_name, "pool_free") == 0) {
    *var = 47500;
  } else if (strcmp(var_name, "max_strings") == 0) {
    *var = 500000;
  } else if (strcmp(var_name, "font_mem_size") == 0) {
    *var = 8000000;
  } else if (strcmp(var_name, "font_max") == 0) {
    *var = 9000;
  } else if (strcmp(var_name, "trie_size") == 0) {
    *var = 1000000;
  } else if (strcmp(var_name, "hyph_size") == 0) {
    *var = 8191;
  } else if (strcmp(var_name, "buf_size") == 0) {
    *var = 200000;
  } else if (strcmp(var_name, "nest_size") == 0) {
    *var = 500;
  } else if (strcmp(var_name, "max_in_open") == 0) {
    *var = 15;
  } else if (strcmp(var_name, "param_size") == 0) {
    *var = 10000;
  } else if (strcmp(var_name, "save_size") == 0) {
    *var = 100000;
  } else if (strcmp(var_name, "stack_size") == 0) {
    *var = 5000;
  } else if (strcmp(var_name, "dvi_buf_size") == 0) {
    *var = 16384;
  } else if (strcmp(var_name, "error_line") == 0) {
    *var = 79;
  } else if (strcmp(var_name, "half_error_line") == 0) {
    *var = 50;
  } else if (strcmp(var_name, "max_print_line") == 0) {
    *var = 79;
  } else if (strcmp(var_name, "hash_extra") == 0) {
    *var = 600000;
  }
}

string concat3(const_string s1, const_string s2, const_string s3) {
  int s2l = s2 ? strlen(s2) : 0;
  int s3l = s3 ? strlen(s3) : 0;
  string answer = (string)xmalloc(strlen(s1) + s2l + s3l + 1);
  strcpy(answer, s1);
  if (s2)
    strcat(answer, s2);
  if (s3)
    strcat(answer, s3);

  return answer;
}

integer zround(double r) {
  integer i;
  if (r > 2147483647.0)
    i = 2147483647;
  else if (r < -2147483647.0)
    i = -2147483647;
  else if (r >= 0.0)
    i = (integer)(r + 0.5);
  else
    i = (integer)(r - 0.5);

  return i;
}

const_string xbasename(const_string name) {
#define IS_DEVICE_SEP(ch) ((ch) == ':')
#define NAME_BEGINS_WITH_DEVICE(name) (*(name) && IS_DEVICE_SEP((name)[1]))
  const_string base = name;
  const_string p;
  if (NAME_BEGINS_WITH_DEVICE(name))
    base += 2;

  for (p = base; *p; p++) {
    if (IS_DIR_SEP(*p))
      base = p + 1;
  }

  return base;
}

boolean dir_p(string fn) {
  struct stat stats;
  return stat(fn, &stats) == 0 && S_ISDIR(stats.st_mode);
}

boolean kpse_absolute_p(const_string filename, boolean relative_ok) {
  boolean absolute;
  boolean explicit_relative;

  absolute = IS_DIR_SEP(*filename);
  explicit_relative = relative_ok;
  return absolute || explicit_relative;
}

void kpse_init_prog(const_string prefix, unsigned dpi, const_string mode,
                    const_string fallback) {}

boolean kpse_in_name_ok(const_string fname) { return true; }
boolean kpse_out_name_ok(const_string fname) { return true; }

void kpse_set_program_enabled(kpse_file_format_type fmt, boolean value,
                              kpse_src_type level) {}

void kpse_set_program_name(const_string argv0, const_string progname) {}

void kpse_reset_program_name(const_string progname) {}

void xfseek(FILE *fp, long offset, int wherefrom, const_string filename) {
  int ret = fseek(fp, offset, wherefrom);
  if (ret != 0) {
    fprintf(stderr, "File Seek Failed %s", filename);
    abort();
  }
}

long xftell(FILE *f, const_string filename) {
  long where = ftell(f);

  if (where < 0) {
    fprintf(stderr, "File Tell Failed %s", filename);
    abort();
  }

  return where;
}

long xftello(FILE *f, const_string filename) {
  long where = ftello(f);

  if (where < 0) {
    fprintf(stderr, "File Tello Failed %s", filename);
    abort();
  }

  return where;
}

void xfseeko(FILE *f, off_t offset, int wherefrom, const_string filename) {
  if (fseeko(f, offset, wherefrom) != 0) {
    fprintf(stderr, "File fseeko Failed (%s %d %d)", filename, (int)(offset),
            wherefrom);
    abort();
  }
}

FILE *xfopen(const_string filename, const_string mode) {
  FILE *f;

  // assert(filename && mode);

  f = fopen(filename, mode);
  if (f == NULL) {
    fprintf(stderr, "File Open Failed (%s)\n", filename);
    abort();
  }

  return f;
}

int xfclose(FILE *stream, const_string filename) {
  int ret = fclose(stream);
  if (ret != 0) {
    fprintf(stderr, "File Close Failed %s", filename);
    abort();
  }
  return 0;
}

static void fix_extension(char *local_name, int format) {
#define SUFFIX(suf) strcat(local_name, suf);

  switch (format) {
  case kpse_gf_format:
    SUFFIX(".gf");
    break;
  case kpse_pk_format:
    SUFFIX(".pk");
    break;
  case kpse_tfm_format:
    SUFFIX(".tfm");
    break;
  case kpse_afm_format:
    SUFFIX(".afm");
    break;
  case kpse_base_format:
    SUFFIX(".base");
    break;
  case kpse_bib_format:
    SUFFIX(".bib");
    break;
  case kpse_bst_format:
    SUFFIX(".bst");
    break;
  case kpse_fontmap_format:
    SUFFIX(".map");
    break;
  case kpse_mem_format:
    SUFFIX(".mem");
    break;
  case kpse_mf_format:
    SUFFIX(".mf");
    break;
  case kpse_mft_format:
    SUFFIX(".mft");
    break;
  case kpse_mfpool_format:
    SUFFIX(".pool");
    break;
  case kpse_mp_format:
    SUFFIX(".mp");
    break;
  case kpse_mppool_format:
    SUFFIX(".pool");
    break;
  case kpse_ocp_format:
    SUFFIX(".ocp");
    break;
  case kpse_ofm_format:
    SUFFIX(".ofm");
    break;
  case kpse_opl_format:
    SUFFIX(".opl");
    break;
  case kpse_otp_format:
    SUFFIX(".otp");
    break;
  case kpse_ovf_format:
    SUFFIX(".ovf");
    break;
  case kpse_ovp_format:
    SUFFIX(".ovp");
    break;
  case kpse_pict_format:
    SUFFIX(".esp");
    break;
  case kpse_tex_format:
    SUFFIX(".tex");
    break;
  case kpse_texpool_format:
    SUFFIX(".pool");
    break;
  case kpse_texsource_format:
    SUFFIX(".dtx");
    break;
  case kpse_type1_format:
    SUFFIX(".pfa");
    break;
  case kpse_vf_format:
    SUFFIX(".vf");
    break;
  case kpse_ist_format:
    SUFFIX(".ist");
    break;
  case kpse_truetype_format:
    SUFFIX(".ttf");
    break;
  case kpse_type42_format:
    SUFFIX(".t42");
    break;
  case kpse_miscfonts_format:
    break;
  case kpse_enc_format:
    SUFFIX(".enc");
    break;
  case kpse_cmap_format:
    SUFFIX("cmap");
    break;
  case kpse_sfd_format:
    SUFFIX(".sfd");
    break;
  case kpse_opentype_format:
    SUFFIX(".otf");
    break;
  case kpse_pdftex_config_format:
    break;
  case kpse_lig_format:
    SUFFIX(".lig");
    break;
  case kpse_texmfscripts_format:
    break;
  case kpse_fea_format:
    SUFFIX(".fea");
    break;
  case kpse_cid_format:
    SUFFIX(".cid");
    break;
  case kpse_mlbib_format:
    SUFFIX(".mlbib");
    break;
  case kpse_mlbst_format:
    SUFFIX(".mlbst");
    break;
  case kpse_ris_format:
    SUFFIX(".ris");
    break;
  case kpse_bltxml_format:
    SUFFIX(".bltxml");
    break;
  case kpse_fmt_format:
    SUFFIX(".fmt");
    break;
  default:
    printf("Unknown Kpse Format %d fixme\n", format);
    break;
  }
#undef SUFFIX
}

#define MAXFILENAMESIZE 512
#ifdef WEBASSEMBLY_BUILD_LOCAL
#define TEXCACHEROOT ""
#include <curl/curl.h>
#define KPSE_BASE_URL "https://texlive.swiftlatex.com/xetex"
static size_t curl_write_data(void *ptr, size_t size, size_t nmemb,
                              void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

char* fontconfig_search_font(const char *nameString, const char *varString) {
  /* Todo: implement in local build */
  return NULL;
}

static int kpse_fetch_from_network(char *name_ret, int format) {

  // If "/" or " " in the name_ret, failed instantly

  if (strrchr(name_ret, '/') != NULL) {
    return -1;
  }

  CURL *curl;
  CURLcode curl_res;
  FILE *destfile;

  int res = 0;
  char name_copy[MAXFILENAMESIZE] = {0};
  char name_url[MAXFILENAMESIZE] = {0};

  char const *tmpfolder = getenv("TEMP");
  if (tmpfolder == 0)
    tmpfolder = "/tmp";

  // Check 404 Cache
  strcpy(name_copy, name_ret);
  snprintf(name_ret, MAXFILENAMESIZE, "%s/%s.404", tmpfolder, name_copy);
  if (access(name_ret, F_OK) != -1) {
    return -1;
  }

  // Check 200 Cache
  snprintf(name_ret, MAXFILENAMESIZE, "%s/%s", tmpfolder, name_copy);
  if (access(name_ret, F_OK) != -1) {
    return 0;
  }

  destfile = fopen(name_ret, "wb");
  if (destfile) {
    curl = curl_easy_init();
    if (curl) {
      snprintf(name_url, MAXFILENAMESIZE, "%s/%s", KPSE_BASE_URL,
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
          printf("File not exists in remote server.\n");
          res = -1;
        } else {
          printf("Server error %ld. %s\n", response_code, name_url);
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

#else
#define TEXCACHEROOT "/tex/"
extern int kpse_fetch_from_network(const char *name_ret, int format);
#endif

static void _prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

char *kpse_find_file(const_string name, kpse_file_format_type format,
                     boolean must_exist) {

  if (name == NULL || strlen(name) > MAXFILENAMESIZE / 2) {
    return NULL;
  }

  if (strncmp(name, "/dev", 4) == 0) {
  	return NULL;
  }

  // printf("looking for %s, format :%d\n ", name, format);
  char local_name[MAXFILENAMESIZE] = {0};
  strncpy(local_name, name, MAXFILENAMESIZE / 2);
  const char *basePath = basename(local_name);
  if (strstr(basePath, ".") == NULL) {
    strncpy(local_name, name, MAXFILENAMESIZE / 2); /* Basename may kill last copy */ 
    fix_extension(local_name, format);
    // printf("fixing extension %s\n", local_name);
  } else {
    strncpy(local_name, name, MAXFILENAMESIZE / 2); /* Basename may kill last copy */ 
  }
  
  if (access(local_name, F_OK) != -1) {
      if (local_name[0] == '/' || (local_name[0] == '.' && local_name[1] == '/')) {
        return concat3("", local_name, NULL);
      } else {
        return concat3("./", local_name, NULL);
      }
  } 
  // printf("failed local access %s\n", local_name);
  // It is an absolute path, no need to go forward
  if (local_name[0] == '/') {
  	return NULL;
  }

  // Download from network, and put it in the cache dir
  if (kpse_fetch_from_network(local_name, format) == 0) {
    return concat3(TEXCACHEROOT, local_name, NULL);
  }
 
  return NULL;
}
