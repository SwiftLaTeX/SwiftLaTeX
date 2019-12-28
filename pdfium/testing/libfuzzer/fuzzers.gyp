# Copyright 2016 PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'pdf_enable_v8%': 1,
    'pdf_enable_xfa%': 0,  # Set to 1 in standalone builds by standalone.gypi.
  },
  'target_defaults': {
    'defines' : [
      'PNG_PREFIX',
      'PNG_USE_READ_MACROS',
    ],
    'include_dirs': [
      # This is implicit in GN.
      '<(DEPTH)',
      '../../third_party/freetype/include',
      '../../third_party/freetype/include/freetype',
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
  'conditions': [
    # FIXME(ochang): Make this work on Mac/Windows.
    ['pdf_enable_xfa==1 and OS=="linux"', {
      'targets': [
        {
          'target_name': 'pdf_fm2js_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_fm2js_fuzzer.cc',
            'unittest_main.cc',
          ],
        },
        {
          'target_name': 'pdf_xml_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_xml_fuzzer.cc',
            'unittest_main.cc',
          ],
        },
        {
          'target_name': 'pdf_cfx_saxreader_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_cfx_saxreader_fuzzer.cc',
            'unittest_main.cc',
          ],
        },
        {
          'target_name': 'pdf_codec_png_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_codec_png_fuzzer.cc',
            'unittest_main.cc',
            'xfa_codec_fuzzer.h',
          ],
        },
        {
          'target_name': 'pdf_codec_jpeg_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_codec_jpeg_fuzzer.cc',
            'unittest_main.cc',
            'xfa_codec_fuzzer.h',
          ],
        },
        {
          'target_name': 'pdf_codec_gif_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_codec_gif_fuzzer.cc',
            'unittest_main.cc',
            'xfa_codec_fuzzer.h',
          ],
        },
        {
          'target_name': 'pdf_codec_bmp_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_codec_bmp_fuzzer.cc',
            'unittest_main.cc',
            'xfa_codec_fuzzer.h',
          ],
        },
        {
          'target_name': 'pdf_codec_tiff_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_codec_tiff_fuzzer.cc',
            'unittest_main.cc',
            'xfa_codec_fuzzer.h',
          ],
        },
        {
          'target_name': 'pdf_css_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_css_fuzzer.cc',
            'unittest_main.cc',
          ],
        },
      ],
    }],
    ['OS=="linux"', {
      'targets': [
        {
          'target_name': 'pdf_jpx_fuzzer',
          'type': 'executable',
          'dependencies': [
            '../../pdfium.gyp:pdfium',
          ],
          'sources': [
            'pdf_jpx_fuzzer.cc',
            'unittest_main.cc',
          ],
        },
      ],
    }],
  ],
  # Empty target so that nonxfa builds work.
  'targets': [
    {
      'target_name': 'empty_target',
      'type': 'none',
    }
  ],
}
