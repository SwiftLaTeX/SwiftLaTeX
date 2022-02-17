mergeInto(LibraryManager.library, {
  kpse_find_file_js: function(nameptr, format, mustexist) {
    return kpse_find_file_impl(nameptr, format, mustexist);
  },
  fontconfig_search_font_js: function(nameptr, varptr) {
    return fontconfig_search_font_impl(nameptr, varptr);
  }
});

