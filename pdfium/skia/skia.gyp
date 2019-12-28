# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'conditions': [
    # In component mode (shared_lib), we build all of skia as a single DLL.
    # However, in the static mode, we need to build skia as multiple targets
    # in order to support the use case where a platform (e.g. Android) may
    # already have a copy of skia as a system library.
    ['component=="static_library"', {
      'targets': [
        {
          'target_name': 'skia_library',
          'type': 'static_library',
          # The optimize: 'max' scattered throughout are particularly
          # important when compiled by MSVC 2013, which seems
          # to mis-link-time-compile code that's built with
          # different optimization levels. http://crbug.com/543583
          'variables': {
            'optimize': 'max',
          },
          'includes': [
            'skia_common.gypi',
            'skia_library.gypi',
          ],
        },
      ],
    }],
    ['component=="static_library"', {
      'targets': [
        {
          'target_name': 'skia',
          # The optimize: 'max' scattered throughout are particularly
          # important when compiled by MSVC 2013, which seems
          # to mis-link-time-compile code that's built with
          # different optimization levels. http://crbug.com/543583
          'variables': {
            'optimize': 'max',
          },
          'type': 'none',
          'dependencies': [
            'skia_library',
            'skia_pdfium',
          ],
          'export_dependent_settings': [
            'skia_library',
            'skia_pdfium',
          ],
          'direct_dependent_settings': {
            'conditions': [
              [ 'OS == "win"', {
                'defines': [
                  'GR_GL_FUNCTION_TYPE=__stdcall',
                ],
              }],
            ],
          },
        },
        {
          'target_name': 'skia_pdfium',
          'type': 'static_library',
          'includes': [
            'skia_pdfium.gypi',
            'skia_common.gypi',
          ],
        },
      ],
    }],
  ],
}
