#ifndef KPSEEMU
#define KPSEEMU
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

/* Perhaps we could use this for path values themselves; for now, we use
   it only for the program_enabled_p value.  */
typedef enum {
  kpse_src_implicit,   /* C initialization to zero */
  kpse_src_compile,    /* configure/compile-time default */
  kpse_src_texmf_cnf,  /* texmf.cnf, the kpathsea config file */
  kpse_src_client_cnf, /* application config file, e.g., config.ps */
  kpse_src_env,        /* environment variable */
  kpse_src_x,          /* X Window System resource */
  kpse_src_cmdline     /* command-line option */
} kpse_src_type;

#define kpseinnameok kpse_in_name_ok
#define kpseoutnameok kpse_out_name_ok
#define kpseinitprog kpse_init_prog
#define kpse_find_tex(name) kpse_find_file(name, kpse_tex_format, true)
#define kpsesetprogramenabled kpse_set_program_enabled
#define kpsetexformat kpse_tex_format
#define kpsebibformat kpse_bib_format
#define kpsebstformat kpse_bst_format
#define DUMP_FORMAT kpse_fmt_format
#define kpsepkformat kpse_pk_format
#define kpsesrccompile kpse_src_compile
#define kpsemaketexdiscarderrors kpse_make_tex_discard_errors
#define kpseinnameok kpse_in_name_ok
extern boolean kpse_in_name_ok(const_string fname);
extern boolean kpse_out_name_ok(const_string fname);
extern void kpse_init_prog(const_string prefix, unsigned dpi, const_string mode,
                           const_string fallback);
extern void kpse_set_program_enabled(kpse_file_format_type fmt, boolean value,
                                     kpse_src_type level);
extern string kpse_find_file(const_string name, kpse_file_format_type format,
                             boolean must_exist);
extern string kpse_find_glyph(const_string passed_fontname, unsigned int dpi,
                              kpse_file_format_type format, void *notused);
extern boolean kpse_absolute_p(const_string filename, boolean relative_ok);

extern string concat3(const_string s1, const_string s2, const_string s3);
extern const_string xbasename(const_string name);
extern int xfclose(FILE *stream, const_string filename);
extern FILE *xfopen(const_string filename, const_string mode);
extern void xfseek(FILE *fp, long offset, int wherefrom, const_string filename);
extern long xftell(FILE *f, const_string filename);
extern long xftello(FILE *f, const_string filename);
extern void xfseeko(FILE *f, off_t offset, int wherefrom,
                    const_string filename);
#endif