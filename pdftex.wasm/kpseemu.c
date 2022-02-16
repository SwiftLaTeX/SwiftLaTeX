#define EXTERN extern
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pdftexd.h>
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

extern char* kpse_find_file_js(const char* name, kpse_file_format_type format,
                     boolean must_exist);


extern char* kpse_find_pk_js(const char* passed_fontname,  unsigned int dpi);


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
    SUFFIX(".cfg");
    break;
  case kpse_lig_format:
    SUFFIX(".lig");
    break;
  case kpse_texmfscripts_format:
    // Todo
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

#define MAX_PATH_LEN 256
char* kpse_find_file(const char* name, kpse_file_format_type format,
                     boolean must_exist) {
  if (name == NULL) {
    return NULL;
  }

  if (strlen(name) > MAX_PATH_LEN) {
    return NULL;
  }

  char* local_name = xmalloc(MAX_PATH_LEN + 32);
  strcpy(local_name, name);
  
  // Search local directory
  if (access(local_name, F_OK) != -1) {
    return local_name;
  }

  // Append extension and search again
  const char *basePath = basename(local_name);
  if (strstr(basePath, ".") == NULL) {
    strcpy(local_name, name); // Basename may modify the argument, recopy
    fix_extension(local_name, format);
    if (access(local_name, F_OK) != -1) {
      return local_name;
    }
  }

  // End local Search
  free(local_name);

  // Head to network search
  return kpse_find_file_js(name, format, must_exist);

}

char* kpse_find_pk(const char* fontname,  unsigned int dpi) {
  if (fontname == NULL) {
    return NULL;
  }

  if (strlen(fontname) > MAX_PATH_LEN) {
    return NULL;
  }

  // Search local directory
  char* local_name = xmalloc(MAX_PATH_LEN + 32);
  if (access(local_name, F_OK) != -1) {
    return local_name;
  }

  sprintf(local_name, "%s.%dpk", fontname, dpi);

  if (access(local_name, F_OK) != -1) {
    return local_name;
  }

  // End local Search
  free(local_name);
  
  // Head to network search
  return kpse_find_pk_js(fontname, dpi);
}