# Copyright 2014 PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Definitions to be used when building stand-alone PDFium binaries.

{
  'variables': {
    'component%': 'static_library',
    'chromium_code%': 0,
    'clang%': 0,
    'asan%': 0,
    'sanitizer_coverage%': 0,
    'use_goma%': 0,
    'gomadir%': '',
    'msvs_multi_core_compile%': '1',
    'pdf_enable_xfa%': 1,
    'variables': {
      'variables': {
        'variables': {
          'conditions': [
            ['OS=="linux" or OS=="mac"', {
              # This handles the Unix platforms we generally deal with.
              # Anything else gets passed through, which probably won't work
              # very well; such hosts should pass an explicit target_arch
              # to gyp.
              'host_arch%':
                '<!(uname -m | sed -e "s/i.86/ia32/;\
                                       s/x86_64/x64/;\
                                       s/amd64/x64/;\
                                       s/arm.*/arm/;\
                                       s/aarch64/arm64/;\
                                       s/mips.*/mipsel/")',
            }, {
              # OS!="linux" and OS!="mac"
              'host_arch%': 'ia32',
            }],
          ],
        },
        'host_arch%': '<(host_arch)',
        'target_arch%': '<(host_arch)',
      },
      'host_arch%': '<(host_arch)',
      'target_arch%': '<(target_arch)',
      'clang_use_chrome_plugins%': 0,
    },
    'clang_dir%': 'third_party/llvm-build/Release+Asserts',
    'clang_use_chrome_plugins%': '<(clang_use_chrome_plugins)',
    # These two are needed by V8.
    'host_arch%': '<(host_arch)',
    'target_arch%': '<(target_arch)',
    'werror%': '-Werror',
    'v8_optimized_debug%': 0,
    'v8_use_external_startup_data%': 0,
    'icu_use_data_file_flag%': 1,
    'icu_gyp_path': '../third_party/icu/icu.gyp',
    'libjpeg_gyp_path': '../third_party/third_party.gyp',
    'conditions': [
      ['OS == "win"', {
        'os_posix%': 0,
      }, {
        'os_posix%': 1,
      }],
      ['OS=="linux" or OS=="mac"', {
        'clang%': 1,
        'host_clang%': 1,
      }, {
        'clang%': 0,
        'host_clang%': 0,
      }],
      # Set default gomadir.
      ['OS=="win"', {
        'gomadir%': 'c:\\goma\\goma-win64',
      }, {
        'gomadir%': '<!(/bin/echo -n ${HOME}/goma)',
      }],
      ['clang_use_chrome_plugins==1', {
        # Share the same settings as chromium in build/common.gypi.
        'variables': {
          'conditions': [
            ['OS!="win"', {
              'variables': {
                'conditions': [
                  ['OS=="mac" or OS=="ios"', {
                    'clang_lib_path%': '<!(cd <(DEPTH) && pwd -P)/third_party/llvm-build/Release+Asserts/lib/libFindBadConstructs.dylib',
                  }, { # OS != "mac" and OS != "ios"
                    'clang_lib_path%': '<!(cd <(DEPTH) && pwd -P)/third_party/llvm-build/Release+Asserts/lib/libFindBadConstructs.so',
                  }],
                ],
              },
              'clang_dynlib_flags%': '-Xclang -load -Xclang <(clang_lib_path) ',
            }, { # OS == "win"
              # On Windows, the plugin is built directly into clang, so there's
              # no need to load it dynamically.
              'clang_dynlib_flags%': '',
            }],
            ['OS=="android" or OS=="linux"', {
              'clang_plugin_check_ipc_arg': '-Xclang -plugin-arg-find-bad-constructs -Xclang check-ipc',
            }, {
              'clang_plugin_check_ipc_arg': '',
            }],
          ],
          'clang_plugin_args%': '-Xclang -plugin-arg-find-bad-constructs -Xclang check-templates '
          '-Xclang -plugin-arg-find-bad-constructs -Xclang follow-macro-expansion '
          '-Xclang -plugin-arg-find-bad-constructs -Xclang check-implicit-copy-ctors ',
        },
        'clang_chrome_plugins_flags%':
          '<(clang_dynlib_flags)'
          '-Xclang -add-plugin -Xclang find-bad-constructs <(clang_plugin_args) <(clang_plugin_check_ipc_arg)',
      }],
    ],
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'cflags': [
          '-g',
          '-O0',
          '-fdata-sections',
          '-ffunction-sections',
        ],
        'defines': ['_DEBUG'],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'Optimization': '0',
            'conditions': [
              ['component=="shared_library"', {
                'RuntimeLibrary': '3',  # /MDd
              }, {
                'RuntimeLibrary': '1',  # /MTd
              }],
            ],
          },
          'VCLinkerTool': {
            'LinkIncremental': '2',
          },
        },
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '0',  # -O0
        },
        'conditions': [
          ['OS=="linux"', {
            # Enable libstdc++ debugging to help catch problems early.
            'defines': ['_GLIBCXX_DEBUG=1',],
          }],
        ],
      },
      'Release': {
        'cflags': [
          '-fno-strict-aliasing',
        ],
        'defines': ['NDEBUG'],
        'xcode_settings': {
          'GCC_OPTIMIZATION_LEVEL': '3',  # -O3
          'GCC_STRICT_ALIASING': 'NO',
        },
        'msvs_settings': {
          'VCCLCompilerTool': {
            'Optimization': '2',
            'InlineFunctionExpansion': '2',
            'EnableIntrinsicFunctions': 'true',
            'FavorSizeOrSpeed': '0',
            'StringPooling': 'true',
            'conditions': [
              ['component=="shared_library"', {
                'RuntimeLibrary': '2',  #/MD
              }, {
                'RuntimeLibrary': '0',  #/MT
              }],
            ],
          },
          'VCLinkerTool': {
            'LinkIncremental': '1',
            'OptimizeReferences': '2',
            'EnableCOMDATFolding': '2',
          },
        },
        'conditions': [
          ['OS=="linux"', {
            'cflags': [
              '-fdata-sections',
              '-ffunction-sections',
              '-O3',
              '-O2',
            ],
          }],
          ['OS=="android"', {
            'cflags!': [
              '-O3',
              '-Os',
            ],
            'cflags': [
              '-fdata-sections',
              '-ffunction-sections',
              '-O2',
            ],
          }],
        ],  # conditions
      },
      'Debug_x64': {
        'inherit_from': ['Debug'],
        'msvs_configuration_platform': 'x64',
      },
      'Release_x64': {
        'inherit_from': ['Release'],
        'msvs_configuration_platform': 'x64',
      },
    },
    'cflags': [
      '-Wall',
      '-Werror',
      '-Wextra',
      # Two common warning flags for Clang and GCC.
      '-Wno-missing-field-initializers',
      '-Wno-unused-parameter',
      '-pthread',
      '-fno-exceptions',
      '-fvisibility=hidden',
    ],
    'cflags_cc': [
      '-std=c++11',
      # Add back when ICU is clean
      # '-Wnon-virtual-dtor',
      '-fno-rtti',
    ],
    'ldflags': [
      '-pthread',
    ],
    'defines': [
      # Don't use deprecated V8 APIs anywhere.
      'V8_DEPRECATION_WARNINGS',
    ],
    'msvs_cygwin_dirs': ['<(DEPTH)/v8/third_party/cygwin'],
    'msvs_configuration_attributes': {
      'OutputDirectory': '<(DEPTH)\\build\\$(ConfigurationName)',
      'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)',
      'CharacterSet': '1',
    },
    'msvs_disabled_warnings': [
      # ####
      # This section is PDFium specific.
      # ####

      # Now there is no PDFium specific flag. Add here if needed.

      # ####
      # This section should match Chromium's build/common.gypi.
      # ####

      # C4091: 'typedef ': ignored on left of 'X' when no variable is
      #                    declared.
      # This happens in a number of Windows headers. Dumb.
      4091,

      # C4127: conditional expression is constant
      # This warning can in theory catch dead code and other problems, but
      # triggers in far too many desirable cases where the conditional
      # expression is either set by macros or corresponds some legitimate
      # compile-time constant expression (due to constant template args,
      # conditionals comparing the sizes of different types, etc.).  Some of
      # these can be worked around, but it's not worth it.
      4127,

      # C4351: new behavior: elements of array 'array' will be default
      #        initialized
      # This is a silly "warning" that basically just alerts you that the
      # compiler is going to actually follow the language spec like it's
      # supposed to, instead of not following it like old buggy versions
      # did.  There's absolutely no reason to turn this on.
      4351,

      # C4355: 'this': used in base member initializer list
      # It's commonly useful to pass |this| to objects in a class'
      # initializer list.  While this warning can catch real bugs, most of
      # the time the constructors in question don't attempt to call methods
      # on the passed-in pointer (until later), and annotating every legit
      # usage of this is simply more hassle than the warning is worth.
      4355,

      # C4503: 'identifier': decorated name length exceeded, name was
      #        truncated
      # This only means that some long error messages might have truncated
      # identifiers in the presence of lots of templates.  It has no effect
      # on program correctness and there's no real reason to waste time
      # trying to prevent it.
      4503,

      # Warning C4589 says: "Constructor of abstract class ignores
      # initializer for virtual base class." Disable this warning because it
      # is flaky in VS 2015 RTM. It triggers on compiler generated
      # copy-constructors in some cases.
      4589,

      # C4611: interaction between 'function' and C++ object destruction is
      #        non-portable
      # This warning is unavoidable when using e.g. setjmp/longjmp.  MSDN
      # suggests using exceptions instead of setjmp/longjmp for C++, but
      # Chromium code compiles without exception support.  We therefore have
      # to use setjmp/longjmp for e.g. JPEG decode error handling, which
      # means we have to turn off this warning (and be careful about how
      # object destruction happens in such cases).
      4611,

      # TODO(thestig): These warnings are level 4. They will be slowly
      # removed as code is fixed.
      4100, # Unreferenced formal parameter
      4121, # Alignment of a member was sensitive to packing
      4244, # Conversion from 'type1' to 'type2', possible loss of data
      4505, # Unreferenced local function has been removed
      4510, # Default constructor could not be generated
      4512, # Assignment operator could not be generated
      4610, # Object can never be instantiated
      4838, # Narrowing conversion. Doesn't seem to be very useful.
      4995, # 'X': name was marked as #pragma deprecated
      4996, # 'X': was declared deprecated (for GetVersionEx).

      # These are variable shadowing warnings that are new in VS2015. We
      # should work through these at some point -- they may be removed from
      # the RTM release in the /W4 set.
      4456, 4457, 4458, 4459,

      # TODO(brucedawson): http://crbug.com/554200 4312 is a VS
      # 2015 64-bit warning for integer to larger pointer
      4312,

      # ####
      # Do not add PDFium specific entries here. Add them to the top.
      # ####
    ],
    'msvs_settings': {
      'VCCLCompilerTool': {
        'MinimalRebuild': 'false',
        'BufferSecurityCheck': 'true',
        'EnableFunctionLevelLinking': 'true',
        'RuntimeTypeInfo': 'false',
        'WarningLevel': '3',
        'WarnAsError': 'true',
        'DebugInformationFormat': '3',
        'Detect64BitPortabilityProblems': 'false',
        'conditions': [
          [ 'msvs_multi_core_compile', {
            'AdditionalOptions': ['/MP'],
          }],
          ['component=="shared_library"', {
            'ExceptionHandling': '1',  # /EHsc
          }, {
            'ExceptionHandling': '0',
          }],
          ['clang==1', {
            'AdditionalOptions': [
              # Don't warn about unused function parameters.
              # (This is also used on other platforms.)
              '-Wno-unused-parameter',
              # Don't warn about the "struct foo f = {0};" initialization
              # pattern.
              '-Wno-missing-field-initializers',

              # TODO(hans): Make this list shorter eventually, http://crbug.com/504657
              '-Qunused-arguments',  # http://crbug.com/504658
              '-Wno-microsoft-enum-value',  # http://crbug.com/505296
              '-Wno-unknown-pragmas',  # http://crbug.com/505314
              '-Wno-microsoft-cast',  # http://crbug.com/550065
            ],
          }],
          ['clang==1 and clang_use_chrome_plugins==1', {
            'AdditionalOptions': [
              '<@(clang_chrome_plugins_flags)',
            ],
          }],
          ['OS=="win" and clang==1 and MSVS_VERSION == "2013"', {
            'AdditionalOptions': [
              '-fmsc-version=1800',
            ],
          }],
          ['OS=="win" and clang==1 and MSVS_VERSION == "2015"', {
            'AdditionalOptions': [
              '-fmsc-version=1900',
            ],
          }],
        ],
      },
      'VCLibrarianTool': {
        'AdditionalOptions': ['/ignore:4221'],
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
        'LinkIncremental': '1',
        # SubSystem values:
        #   0 == not set
        #   1 == /SUBSYSTEM:CONSOLE
        #   2 == /SUBSYSTEM:WINDOWS
        'SubSystem': '1',
      },
    },
    'xcode_settings': {
      'ALWAYS_SEARCH_USER_PATHS': 'NO',
      'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
      'CLANG_CXX_LIBRARY': 'libc++',            # -stdlib=libc++
      'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
      'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                # (Equivalent to -fPIC)
      'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',        # -fno-exceptions
      'GCC_ENABLE_CPP_RTTI': 'NO',              # -fno-rtti
      'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
      # GCC_INLINES_ARE_PRIVATE_EXTERN maps to -fvisibility-inlines-hidden
      'GCC_INLINES_ARE_PRIVATE_EXTERN': 'YES',
      'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',      # -fvisibility=hidden
      'GCC_TREAT_WARNINGS_AS_ERRORS': 'NO',     # -Werror
      'GCC_WARN_NON_VIRTUAL_DESTRUCTOR': 'YES', # -Wnon-virtual-dtor
      'MACOSX_DEPLOYMENT_TARGET': '10.7',       # -mmacosx-version-min
      'SYMROOT': '<(DEPTH)/xcodebuild',
      'USE_HEADERMAP': 'NO',
      'OTHER_CFLAGS': [
        '-fno-strict-aliasing',
      ],
      'WARNING_CFLAGS': [
        '-Wall',
        '-Werror',
        '-Wextra',
        '-Wno-unused-parameter',
      ],
      'conditions': [
        ['clang==1 and clang_use_chrome_plugins==1', {
          'OTHER_CFLAGS': [
            '<@(clang_chrome_plugins_flags)',
          ],
        }],
      ],
    },
    'variables': {
      'chromium_code%': '<(chromium_code)',
      'clang_warning_flags': [
        # TODO(thakis): https://crbug.com/604888
        '-Wno-undefined-var-template',
      ],
    },
    'includes': [ 'set_clang_warning_flags.gypi', ],
    'conditions': [
      ['clang==1 and clang_use_chrome_plugins==1', {
        'cflags': [
          '<@(clang_chrome_plugins_flags)',
        ],
      }],
      ['component=="shared_library"', {
        'cflags': [
          '-fPIC',
        ],
      }],
      ['asan==1', {
        'defines': [
          'ADDRESS_SANITIZER',
          'LEAK_SANITIZER',
        ],
        'cflags': [
          '-fsanitize=address',
          '-gline-tables-only',
        ],
        'ldflags': [
          '-fsanitize=address',
        ],
      }],
      ['sanitizer_coverage!=0', {
        'cflags': [
          '-fsanitize-coverage=<(sanitizer_coverage)',
        ],
      }],
      ['OS=="win"', {
        'defines': [
          'NOMINMAX',
          '_CRT_SECURE_NO_DEPRECATE',
          '_CRT_NONSTDC_NO_DEPRECATE',
        ],
        'conditions': [
          ['component=="static_library"', {
            'defines': [
              '_HAS_EXCEPTIONS=0',
            ],
          }],
          ['use_goma==1', {
            # goma doesn't support PDB yet.
            'msvs_settings': {
              'VCLinkerTool': {
                'GenerateDebugInformation': 'true',
              },
              'VCCLCompilerTool': {
                'DebugInformationFormat': '1',
              },
            },
          }],
        ],
      }],  # OS=="win"
      ['OS=="mac"', {
        'target_conditions': [
          ['_type!="static_library"', {
            'xcode_settings': {'OTHER_LDFLAGS': ['-Wl,-search_paths_first']},
          }],
        ],  # target_conditions
        'variables': {
          'mac_sdk_min': '10.10',
          'mac_sdk%': '<!(python <(DEPTH)/build/mac/find_sdk.py <(mac_sdk_min))',
        },
        'xcode_settings': {
          'SDKROOT': 'macosx<(mac_sdk)',  # -isysroot
          # See comment in Chromium's common.gypi for why this is needed.
          'SYMROOT': '<(DEPTH)/xcodebuild',
        },
      }],  # OS=="mac"
      ['v8_use_external_startup_data==1', {
        'defines': [
          'V8_USE_EXTERNAL_STARTUP_DATA',
        ],
      }],  # v8_use_external_startup_data==1
      ['clang==1 or host_clang==1', {
        # This is here so that all files get recompiled after a clang roll and
        # when turning clang on or off.
        # (defines are passed via the command line, and build systems rebuild
        # things when their commandline changes). Nothing should ever read this
        # define.
        'defines': ['CR_CLANG_REVISION=<!(python <(DEPTH)/tools/clang/scripts/update.py --print-revision)'],
      }],
    ],
    'target_conditions': [
      ['chromium_code==0', {
        'variables': {
          'clang_warning_flags': [
            # Suppress unused variable warnings from third-party libraries.
            '-Wno-unused-variable',
          ],
        },
        'msvs_disabled_warnings': [
          # forcing value to bool 'true' or 'false' (performance warning)
          4800,
        ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'WarningLevel': '3',
          },
        },
        'conditions': [
          ['OS!="win"', {
            # Remove -Wextra for third-party code.
            'cflags!': [ '-Wextra' ],
          }],
          ['OS!="win" and clang==0', {
            'cflags!': [
              '-Wall',
            ],
          }],
          ['OS=="mac"', {
            'xcode_settings': {
              'WARNING_CFLAGS!': ['-Wextra'],
            },
          }],
        ],
      }, {  # chromium_code==1
        'msvs_settings': {
          'VCCLCompilerTool': {
            'WarningLevel': '4',
          },
        },
      }],
    ],  # target_conditions for 'target_defaults'
  },
  'conditions': [
    ['OS=="linux" or OS=="mac"', {
      'conditions': [
        ['clang==1', {
          'make_global_settings': [
            ['CC', '<(clang_dir)/bin/clang'],
            ['CXX', '<(clang_dir)/bin/clang++'],
          ],
        }],
      ],
    }],  # OS=="linux" or OS=="mac"
    ['OS=="win"', {
      'conditions': [
        ['clang==1', {
          'make_global_settings': [
            ['CC', '<(clang_dir)/bin/clang-cl'],
          ],
        }],
      ],
    }],  # OS=="win"
    ["use_goma==1", {
      'make_global_settings': [
        ['CC_wrapper', '<(gomadir)/gomacc'],
        ['CXX_wrapper', '<(gomadir)/gomacc'],
      ],
    }],  # use_goma==1
  ],
}
