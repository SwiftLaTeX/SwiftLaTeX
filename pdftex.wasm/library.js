mergeInto(LibraryManager.library, {
  kpse_find_file_js: function(nameptr, format, mustexist) {
    return kpse_find_file_impl(nameptr, format, mustexist);
  },
  kpse_find_pk_js: function(nameptr, dpi) {
    return kpse_find_pk_impl(nameptr, dpi);
  }
});

