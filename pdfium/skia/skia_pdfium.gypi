# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.


# This gypi file contains all the PDFium-specific enhancements to Skia.
# In component mode (shared_lib) it is folded into a single shared library with
# the Skia files but in all other cases it is a separate library.
{
  'dependencies': [
    'skia_library',
  ],

  'direct_dependent_settings': {
    'include_dirs': [
      'ext',
    ],
  },
  'variables': {
    # TODO(scottmg): http://crbug.com/177306
    'clang_warning_flags_unset': [
      # Don't warn about string->bool used in asserts.
      '-Wstring-conversion',
    ],
  },
  'sources': [
    'config/SkUserConfig.h',
    'ext/google_logging.cc',
  ],
}
