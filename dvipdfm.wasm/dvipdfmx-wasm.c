// First of all, remove pdf_files_init/close from xetex-ini.c
// And remove picture handling functions from xetex-pic.c
#include "core-bridge.h"
#include <md5.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <libgen.h>
#include "core-memory.h"
void issue_warning(void *context, char const *text) {
    printf("%s\n", text);
}

void issue_error(void *context, char const *text) {
    printf("%s\n", text);
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

int output_close(void *context, void *handle) {
    return fclose(handle);
}

#define MAXFILENAMESIZE 512


#define TEXCACHEROOT "/tex/"
int kpse_fetch_from_network(const char *name_ret, int format);



typedef enum {
  kpse_gf_format,
  kpse_pk_format,
  kpse_any_glyph_format, /* ``any'' meaning gf or pk */
  kpse_tfm_format,
  kpse_afm_format,
  kpse_base_format,
  kpse_bib_format,
  kpse_bst_format,
  kpse_cnf_format,
  kpse_db_format,
  kpse_fmt_format,
  kpse_fontmap_format,
  kpse_mem_format,
  kpse_mf_format,
  kpse_mfpool_format,
  kpse_mft_format,
  kpse_mp_format,
  kpse_mppool_format,
  kpse_mpsupport_format,
  kpse_ocp_format,
  kpse_ofm_format,
  kpse_opl_format,
  kpse_otp_format,
  kpse_ovf_format,
  kpse_ovp_format,
  kpse_pict_format,
  kpse_tex_format,
  kpse_texdoc_format,
  kpse_texpool_format,
  kpse_texsource_format,
  kpse_tex_ps_header_format,
  kpse_troff_font_format,
  kpse_type1_format,
  kpse_vf_format,
  kpse_dvips_config_format,
  kpse_ist_format,
  kpse_truetype_format,
  kpse_type42_format,
  kpse_web2c_format,
  kpse_program_text_format,
  kpse_program_binary_format,
  kpse_miscfonts_format,
  kpse_web_format,
  kpse_cweb_format,
  kpse_enc_format,
  kpse_cmap_format,
  kpse_sfd_format,
  kpse_opentype_format,
  kpse_pdftex_config_format,
  kpse_lig_format,
  kpse_texmfscripts_format,
  kpse_lua_format,
  kpse_fea_format,
  kpse_cid_format,
  kpse_mlbib_format,
  kpse_mlbst_format,
  kpse_clua_format,
  kpse_ris_format,
  kpse_bltxml_format,
  kpse_last_format /* one past last index */
} kpse_file_format_type;


int _formatConvert(int format) {
    switch (format) {
    case TTIF_TFM:
        return kpse_tfm_format;
    case TTIF_AFM:
        return kpse_afm_format;
    case TTIF_BIB:
        return kpse_bib_format;
    case TTIF_BST:
        return kpse_bst_format;
    case TTIF_CNF:
        return kpse_cnf_format;
    case TTIF_FORMAT:
        return kpse_fmt_format;
    case TTIF_FONTMAP:
        return kpse_fontmap_format;
    case TTIF_OFM:
        return kpse_ofm_format;
    case TTIF_OVF:
        return kpse_ovf_format;
    case TTIF_TEX:
        return kpse_tex_format;
    case TTIF_TYPE1:
        return kpse_type1_format;
    case TTIF_VF:
        return kpse_vf_format;
    case TTIF_TRUETYPE:
        return kpse_truetype_format;
    case TTIF_ENC:
        return kpse_enc_format;
    case TTIF_CMAP:
        return kpse_cmap_format;
    case TTIF_SFD:
        return kpse_sfd_format;
    case TTIF_OPENTYPE:
        return kpse_opentype_format;
    default:
        // fprintf(stderr, "Unknown format %d\n", format);
        return kpse_tex_format;
    }
}

static char* concat3(const char* s1, const char* s2, const char* s3) {
  int s2l = s2 ? strlen(s2) : 0;
  int s3l = s3 ? strlen(s3) : 0;
  char* answer = (char*)xmalloc(strlen(s1) + s2l + s3l + 1);
  strcpy(answer, s1);
  if (s2)
    strcat(answer, s2);
  if (s3)
    strcat(answer, s3);

  return answer;
}


extern char* kpse_find_file_js(const char* name, kpse_file_format_type format,
                     int must_exist);

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

char *kpse_find_file(const char *name, tt_input_format_type tt_format) {

  
  int format = _formatConvert(tt_format);
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
  return kpse_find_file_js(name, format, 0);

}

void *input_open(void *context, char const *path, tt_input_format_type format,
                 int is_gz) {
    
    // fprintf(stderr, "Opening %s format %d\n", path, format);
    char *normalized_path = kpse_find_file(path, format);
    if (normalized_path != NULL) {
        FILE *res = fopen(normalized_path, "rb");
        free(normalized_path);
        // fprintf(stderr, "Opening %s format %d pointer: %p\n", path, format, res);
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

    int seek_res = fseek(handle, offset, whence);
    if (seek_res != 0) {
        fprintf(stderr, "seek failed  %d %p\n", seek_res, handle);
        *internal_error = 1;
        return -1;
    } else {
        if (whence == SEEK_SET) {
            return offset;
        }
        /* Return current file pointer */
        return ftell(handle);
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
char MAIN_ENTRY_FILE[512];


int compilePDF(){
    char main_xdv_file[512];
    char main_pdf_file[512];
    strncpy(main_xdv_file, MAIN_ENTRY_FILE, 512);
    strncpy(main_pdf_file, MAIN_ENTRY_FILE, 512);
    int len = strlen(main_xdv_file);
    if(len < 3) return -1;
    main_xdv_file[len - 1] = 'v';
    main_xdv_file[len - 2] = 'd';
    main_xdv_file[len - 3] = 'x';
    main_pdf_file[len - 1] = 'f';
    main_pdf_file[len - 2] = 'd';
    main_pdf_file[len - 3] = 'p';
    return dvipdfmx_simple_main(&ourapi, main_xdv_file, main_pdf_file, false, false, time(0));
  // return tex_simple_main(&ourapi, "xelatex.fmt", "xelatex.ini", time(0), 1); //xelatex.fmt does not matter
}

int setMainEntry(const char *p) {
    strncpy(MAIN_ENTRY_FILE, p, 512);
    MAIN_ENTRY_FILE[511] = 0;
    return 0;
}

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
    strncpy(MAIN_ENTRY_FILE, "main.tex", 512);

    return 0;
}



