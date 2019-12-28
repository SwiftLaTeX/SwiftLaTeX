# Copyright 2014 PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'pdf_enable_v8%': 1,
    'pdf_enable_xfa%': 0,  # Set to 1 in standalone builds by standalone.gypi.
    'pdf_use_skia%': 0,
  },
  'target_defaults': {
    'defines' : [
      'PNG_PREFIX',
      'PNG_USE_READ_MACROS',
    ],
    'include_dirs': [
      # This is implicit in GN.
      '<(DEPTH)',
      '..',
    ],
    'conditions': [
      ['pdf_enable_v8==1', {
        'defines': [
          'PDF_ENABLE_V8',
        ],
        'include_dirs': [
          '<(DEPTH)/v8',
          '<(DEPTH)/v8/include',
        ],
      }],
      ['pdf_enable_xfa==1', {
        'defines': [
          'PDF_ENABLE_XFA',
        ],
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'pdfium_test',
      'type': 'executable',
      'dependencies': [
        '../pdfium.gyp:pdfium',
        '../pdfium.gyp:test_support',
        # Regardless of whether the library ships against system freetype,
        # always link this binary against the bundled one for consistency
        # of results across platforms.
        '../third_party/third_party.gyp:fx_freetype',
      ],
      'sources': [
        'pdfium_test.cc',
        'image_diff_png.cc',
      ],
      'link_settings': {
        'libraries!': [
          '-lfreetype',
        ],
      },
      'conditions': [
        ['pdf_enable_xfa==0', {
          'dependencies': [
            '../third_party/third_party.gyp:fx_lpng',
          ],
        }],
        ['pdf_enable_v8==1', {
          'dependencies': [
            '<(DEPTH)/v8/src/v8.gyp:v8_libplatform',
          ],
        }],
        ['pdf_use_skia==1', {
          'defines': ['PDF_ENABLE_SKIA'],
          'dependencies': [
            '<(DEPTH)/skia/skia.gyp:skia',
          ],
        }],
      ],
    },
    {
      'target_name': 'pdfium_diff',
      'type': 'executable',
      'variables': { 'enable_wexit_time_destructors': 1, },
      'dependencies': [
        '../pdfium.gyp:pdfium',
        '../third_party/third_party.gyp:pdfium_base',
      ],
      'sources': [
        'image_diff.cc',
        'image_diff_png.h',
        'image_diff_png.cc',
      ],
      'conditions': [
        ['pdf_enable_xfa==0', {
          'dependencies': [
            '../third_party/third_party.gyp:fx_lpng',
          ],
        }],
      ],
    },
  ],
}
