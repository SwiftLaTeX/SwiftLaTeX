mergeInto(LibraryManager.library, {
  kpse_find_file_js: function(nameptr, format) {
    return kpse_find_file_impl(nameptr, format);
  }
});

