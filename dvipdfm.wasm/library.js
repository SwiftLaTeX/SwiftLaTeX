mergeInto(LibraryManager.library, {
  kpse_fetch_from_network: function(nameptr, format) {
    return kpse_fetch_from_network_impl(nameptr, format);
  }
});

