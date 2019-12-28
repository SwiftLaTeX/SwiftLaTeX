# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This gypi file handles the removal of platform-specific files from the
# Skia build.
{
  'include_dirs': [
    '..',
    'config',
  ],

  'conditions': [
    [ 'OS != "android"', {
      'sources/': [
         ['exclude', '_android\\.(cc|cpp)$'],
      ],
    }],
    [ 'OS != "ios"', {
      'sources/': [
         ['exclude', '_ios\\.(cc|cpp|mm?)$'],
      ],
    }],
    [ 'OS == "ios"', {
      'defines': [
        'SK_BUILD_FOR_IOS',
      ],
    }],
    [ 'OS == "win"', {
      'defines': [
        'SK_BUILD_FOR_WIN32',
      ],
    }],
    [ 'OS != "mac"', {
      'sources/': [
        ['exclude', '_mac\\.(cc|cpp|mm?)$'],
      ],
    }],
    [ 'OS == "mac"', {
      'defines': [
        'SK_BUILD_FOR_MAC',
      ],
    }],
    [ 'OS != "win"', {
      'sources/': [ ['exclude', '_win\\.(cc|cpp)$'] ],
    }],
    [ 'OS == "win"', {
      'defines': [
        # On windows, GDI handles are a scarse system-wide resource so we have to keep
        # the glyph cache, which holds up to 4 GDI handles per entry, to a fairly small
        # size.
        # http://crbug.com/314387
        'SK_DEFAULT_FONT_CACHE_COUNT_LIMIT=256',
        'GR_GL_FUNCTION_TYPE=__stdcall',
      ],
    }],
    [ 'OS!="linux"', {
      'sources/': [ ['exclude', '_linux\\.(cc|cpp)$'] ],
    }],

    #Settings for text blitting, chosen to approximate the system browser.
    [ 'OS == "linux"', {
      'defines': [
        'SK_GAMMA_EXPONENT=1.2',
        'SK_GAMMA_CONTRAST=0.2',
      ],
    }],
    ['OS == "android"', {
      'defines': [
        'SK_GAMMA_APPLY_TO_A8',
        'SK_GAMMA_EXPONENT=1.4',
        'SK_GAMMA_CONTRAST=0.0',
      ],
    }],
    ['OS == "win"', {
      'defines': [
        'SK_GAMMA_SRGB',
        'SK_GAMMA_CONTRAST=0.5',
      ],
    }],
    ['OS == "mac"', {
      'defines': [
        'SK_GAMMA_SRGB',
        'SK_GAMMA_CONTRAST=0.0',
      ],
    }],

    # Neon support.
    [ 'target_arch == "arm" and arm_version >= 7 and arm_neon == 1', {
      'defines': [
        'SK_ARM_HAS_NEON',
      ],
    }],
    [ 'target_arch == "arm" and arm_version >= 7 and arm_neon == 0 and arm_neon_optional == 1', {
      'defines': [
        'SK_ARM_HAS_OPTIONAL_NEON',
      ],
    }],
  ],

  'variables': {
    'variables': {
      'conditions': [
        ['OS== "ios"', {
          'skia_support_gpu': 0,
        }, {
          'skia_support_gpu': 1,
        }],
      ],
    },
    'skia_support_gpu': '<(skia_support_gpu)',
    'skia_support_pdf': '<(skia_support_pdf)',

    # These two set the paths so we can include skia/gyp/core.gypi
    'skia_src_path': '../third_party/skia/src',
    'skia_include_path': '../third_party/skia/include',

    # This list will contain all defines that also need to be exported to
    # dependent components.
    'skia_export_defines': [
      'SK_SUPPORT_GPU=<(skia_support_gpu)',
    ],

    'default_font_cache_limit%': '(20*1024*1024)',

    'conditions': [
      ['OS== "android"', {
        # Android devices are typically more memory constrained, so
        # default to a smaller glyph cache (it may be overriden at runtime
        # when the renderer starts up, depending on the actual device memory).
        'default_font_cache_limit': '(1*1024*1024)',
        'skia_export_defines': [
          'SK_BUILD_FOR_ANDROID',
        ],
      }],
    ],
  },

  'defines': [
    '<@(skia_export_defines)',

    'SK_DEFAULT_FONT_CACHE_LIMIT=<(default_font_cache_limit)',
  ],

  'direct_dependent_settings': {
    'defines': [
      '<@(skia_export_defines)',
    ],
  },

  # We would prefer this to be direct_dependent_settings,
  # however we currently have no means to enforce that direct dependents
  # re-export if they include Skia headers in their public headers.
  'all_dependent_settings': {
    'include_dirs': [
      '..',
      'config',
    ],
  },

  'msvs_disabled_warnings': [4244, 4267, 4341, 4345, 4390, 4554, 4748, 4800],
}
