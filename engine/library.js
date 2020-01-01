mergeInto(LibraryManager.library, {
  kpse_fetch_from_network: function(nameptr) {
    return kpse_fetch_from_network_impl(nameptr);
  }
});