# Copyright 2014 PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'pdf_enable_xfa%': 0,  # Set to 1 by standalone.gypi in standalone builds.
    'pdf_use_skia%': 0,
  },
  'target_defaults': {
    'defines': [
      'OPJ_STATIC',
      'PNG_PREFIX',
      'PNG_USE_READ_MACROS',
    ],
    'include_dirs': [
      # This is implicit in GN.
      '<(DEPTH)',
      '..',
    ],
  },
  'targets': [
    {
      'target_name': 'bigint',
      'type': 'static_library',
      'sources': [
        'bigint/BigInteger.hh',
        'bigint/BigIntegerLibrary.hh',
        'bigint/BigIntegerUtils.hh',
        'bigint/BigUnsigned.hh',
        'bigint/NumberlikeArray.hh',
        'bigint/BigUnsignedInABase.hh',
        'bigint/BigInteger.cc',
        'bigint/BigIntegerUtils.cc',
        'bigint/BigUnsigned.cc',
        'bigint/BigUnsignedInABase.cc',
      ],
    },
    {
      'target_name': 'fx_freetype',
      'type': 'static_library',
      'defines': [
        'FT2_BUILD_LIBRARY',
      ],
      'include_dirs': [
         'freetype/include',
         'freetype/include/freetype',
      ],
      'sources': [
        'freetype/include/freetype/freetype.h',
        'freetype/include/freetype/ftmm.h',
        'freetype/include/freetype/ftotval.h',
        'freetype/include/freetype/ftoutln.h',
        'freetype/include/freetype/internal/ftobjs.h',
        'freetype/include/freetype/internal/ftstream.h',
        'freetype/include/freetype/internal/tttypes.h',
        'freetype/include/freetype/tttables.h',
        'freetype/include/ft2build.h',
        'freetype/src/base/ftbase.c',
        'freetype/src/base/ftbase.h',
        'freetype/src/base/ftbitmap.c',
        'freetype/src/base/ftglyph.c',
        'freetype/src/base/ftinit.c',
        'freetype/src/base/ftlcdfil.c',
        'freetype/src/base/ftmm.c',
        'freetype/src/base/ftsystem.c',
        'freetype/src/cff/cff.c',
        'freetype/src/cff/cffobjs.h',
        'freetype/src/cff/cfftypes.h',
        'freetype/src/cid/type1cid.c',
        'freetype/src/psaux/psaux.c',
        'freetype/src/pshinter/pshinter.c',
        'freetype/src/psnames/psmodule.c',
        'freetype/src/raster/raster.c',
        'freetype/src/sfnt/sfnt.c',
        'freetype/src/smooth/smooth.c',
        'freetype/src/truetype/truetype.c',
        'freetype/src/type1/type1.c',
      ],
      'conditions': [
        ['pdf_use_skia==1', {
          'sources': [
           'freetype/src/base/ftfntfmt.c',
           'freetype/src/base/ftfstype.c',
           'freetype/src/base/fttype1.c',
          ],
        }],
        ['os_posix==1 and clang==0', {
          'cflags': [
            # open_face_PS_from_sfnt_stream() and open_face_from_buffer() in
            # ftbase.h are unused. GCC needs this flag too.
            '-Wno-unused-function',
          ],
        }],
      ],
      'variables': {
        'clang_warning_flags': [
          # open_face_PS_from_sfnt_stream() and open_face_from_buffer() in
          # ftbase.h are unused.
          '-Wno-unused-function',
        ],
      },
      'msvs_disabled_warnings': [
        # Warnings about conversion from 'size_t' to 'long', possible loss of
        # data.
        4267,
      ],
    },
    {
      'target_name': 'fx_lcms2',
      'type': 'static_library',
      'sources': [
        'lcms2-2.6/include/lcms2.h',
        'lcms2-2.6/include/lcms2_plugin.h',
        'lcms2-2.6/src/cmscam02.c',
        'lcms2-2.6/src/cmscgats.c',
        'lcms2-2.6/src/cmscnvrt.c',
        'lcms2-2.6/src/cmserr.c',
        'lcms2-2.6/src/cmsgamma.c',
        'lcms2-2.6/src/cmsgmt.c',
        'lcms2-2.6/src/cmshalf.c',
        'lcms2-2.6/src/cmsintrp.c',
        'lcms2-2.6/src/cmsio0.c',
        'lcms2-2.6/src/cmsio1.c',
        'lcms2-2.6/src/cmslut.c',
        'lcms2-2.6/src/cmsmd5.c',
        'lcms2-2.6/src/cmsmtrx.c',
        'lcms2-2.6/src/cmsnamed.c',
        'lcms2-2.6/src/cmsopt.c',
        'lcms2-2.6/src/cmspack.c',
        'lcms2-2.6/src/cmspcs.c',
        'lcms2-2.6/src/cmsplugin.c',
        'lcms2-2.6/src/cmsps2.c',
        'lcms2-2.6/src/cmssamp.c',
        'lcms2-2.6/src/cmssm.c',
        'lcms2-2.6/src/cmstypes.c',
        'lcms2-2.6/src/cmsvirt.c',
        'lcms2-2.6/src/cmswtpnt.c',
        'lcms2-2.6/src/cmsxform.c',
      ],
      'conditions': [
        ['os_posix==1', {
          'cflags': [
            '-Wno-main',
            '-Wno-missing-braces',
            '-Wno-unused',
          ],
        }],
      ],
      'variables': {
        'clang_warning_flags': [
          '-Wno-missing-braces',
          # FindPrev() in cmsplugin.c is unused.
          '-Wno-unused-function',
        ],
      },
    },
    {
      'target_name': 'libjpeg',
      'type': 'static_library',
      'sources': [
        'libjpeg/cderror.h',
        'libjpeg/cdjpeg.h',
        'libjpeg/fpdfapi_jcapimin.c',
        'libjpeg/fpdfapi_jcapistd.c',
        'libjpeg/fpdfapi_jccoefct.c',
        'libjpeg/fpdfapi_jccolor.c',
        'libjpeg/fpdfapi_jcdctmgr.c',
        'libjpeg/fpdfapi_jchuff.c',
        'libjpeg/fpdfapi_jcinit.c',
        'libjpeg/fpdfapi_jcmainct.c',
        'libjpeg/fpdfapi_jcmarker.c',
        'libjpeg/fpdfapi_jcmaster.c',
        'libjpeg/fpdfapi_jcomapi.c',
        'libjpeg/fpdfapi_jcparam.c',
        'libjpeg/fpdfapi_jcphuff.c',
        'libjpeg/fpdfapi_jcprepct.c',
        'libjpeg/fpdfapi_jcsample.c',
        'libjpeg/fpdfapi_jctrans.c',
        'libjpeg/fpdfapi_jdapimin.c',
        'libjpeg/fpdfapi_jdapistd.c',
        'libjpeg/fpdfapi_jdcoefct.c',
        'libjpeg/fpdfapi_jdcolor.c',
        'libjpeg/fpdfapi_jddctmgr.c',
        'libjpeg/fpdfapi_jdhuff.c',
        'libjpeg/fpdfapi_jdinput.c',
        'libjpeg/fpdfapi_jdmainct.c',
        'libjpeg/fpdfapi_jdmarker.c',
        'libjpeg/fpdfapi_jdmaster.c',
        'libjpeg/fpdfapi_jdmerge.c',
        'libjpeg/fpdfapi_jdphuff.c',
        'libjpeg/fpdfapi_jdpostct.c',
        'libjpeg/fpdfapi_jdsample.c',
        'libjpeg/fpdfapi_jdtrans.c',
        'libjpeg/fpdfapi_jerror.c',
        'libjpeg/fpdfapi_jfdctfst.c',
        'libjpeg/fpdfapi_jfdctint.c',
        'libjpeg/fpdfapi_jidctfst.c',
        'libjpeg/fpdfapi_jidctint.c',
        'libjpeg/fpdfapi_jidctred.c',
        'libjpeg/fpdfapi_jmemmgr.c',
        'libjpeg/fpdfapi_jmemnobs.c',
        'libjpeg/fpdfapi_jutils.c',
        'libjpeg/jchuff.h',
        'libjpeg/jconfig.h',
        'libjpeg/jdct.h',
        'libjpeg/jdhuff.h',
        'libjpeg/jerror.h',
        'libjpeg/jinclude.h',
        'libjpeg/jmemsys.h',
        'libjpeg/jmorecfg.h',
        'libjpeg/jpegint.h',
        'libjpeg/jpeglib.h',
        'libjpeg/jversion.h',
        'libjpeg/transupp.h',
      ],
      'conditions': [
        ['os_posix==1', {
          'cflags': [
            '-Wno-main',
            '-Wno-missing-braces',
            '-Wno-shift-negative-value',
            '-Wno-unused',
          ],
        }],
      ],
      'variables': {
        'clang_warning_flags': [
          # Avoid warning for undefined behaviour.
          '-Wno-shift-negative-value',
        ],
      },
      'msvs_disabled_warnings': [
        # Warnings about conversion from 'size_t' to 'long', possible loss of
        # data.
        4267,
      ],
    },
    {
      'target_name': 'fx_libopenjpeg',
      'type': 'static_library',
      'sources': [
        'libopenjpeg20/bio.c',
        'libopenjpeg20/cio.c',
        'libopenjpeg20/dwt.c',
        'libopenjpeg20/event.c',
        'libopenjpeg20/function_list.c',
        'libopenjpeg20/image.c',
        'libopenjpeg20/invert.c',
        'libopenjpeg20/j2k.c',
        'libopenjpeg20/jp2.c',
        'libopenjpeg20/mct.c',
        'libopenjpeg20/mqc.c',
        'libopenjpeg20/openjpeg.c',
        'libopenjpeg20/opj_clock.c',
        'libopenjpeg20/pi.c',
        'libopenjpeg20/raw.c',
        'libopenjpeg20/t1.c',
        'libopenjpeg20/t2.c',
        'libopenjpeg20/tcd.c',
        'libopenjpeg20/tgt.c',
      ],
      'msvs_disabled_warnings': [
        4018,
      ],
    },
    {
      'target_name': 'fx_lpng',
      'type': 'static_library',
      'sources': [
        'libpng16/png.c',
        'libpng16/png.h',
        'libpng16/pngconf.h',
        'libpng16/pngdebug.h',
        'libpng16/pngerror.c',
        'libpng16/pngget.c',
        'libpng16/pnginfo.h',
        'libpng16/pnglibconf.h',
        'libpng16/pngmem.c',
        'libpng16/pngpread.c',
        'libpng16/pngprefix.h',
        'libpng16/pngpriv.h',
        'libpng16/pngread.c',
        'libpng16/pngrio.c',
        'libpng16/pngrtran.c',
        'libpng16/pngrutil.c',
        'libpng16/pngset.c',
        'libpng16/pngstruct.h',
        'libpng16/pngtrans.c',
        'libpng16/pngwio.c',
        'libpng16/pngwrite.c',
        'libpng16/pngwtran.c',
        'libpng16/pngwutil.c',
      ],
    },
    {
      'target_name': 'fx_zlib',
      'type': 'static_library',
      'sources': [
        'zlib_v128/adler32.c',
        'zlib_v128/compress.c',
        'zlib_v128/crc32.c',
        'zlib_v128/deflate.c',
        'zlib_v128/gzclose.c',
        'zlib_v128/gzlib.c',
        'zlib_v128/gzread.c',
        'zlib_v128/gzwrite.c',
        'zlib_v128/infback.c',
        'zlib_v128/inffast.c',
        'zlib_v128/inflate.c',
        'zlib_v128/inftrees.c',
        'zlib_v128/trees.c',
        'zlib_v128/uncompr.c',
        'zlib_v128/zutil.c',
      ],
      'conditions': [
        ['os_posix==1', {
          'cflags': [
            # TODO(dsinclair): Remove if fixed upstream. https://crbug.com/507712
            '-Wno-shift-negative-value',
          ],
        }],
        ['OS == "win"', {
          'direct_dependent_settings': {
            'include_dirs': [
              'zlib_v128',
            ],
          }
        }],
      ],
      'variables': {
        'clang_warning_flags': [
          # Avoid warning for undefined behaviour. https://crbug.com/507712
          '-Wno-shift-negative-value',
        ]
      },
    },
    {
      'target_name': 'pdfium_base',
      'type': 'none',
      'sources': [
        'base/logging.h',
        'base/macros.h',
        'base/numerics/safe_conversions.h',
        'base/numerics/safe_conversions_impl.h',
        'base/numerics/safe_math.h',
        'base/numerics/safe_math_impl.h',
        'base/stl_util.h',
      ],
    },
  ],
  'conditions': [
    ['pdf_enable_xfa==1', {
      'targets': [
        {
          'target_name': 'fx_tiff',
          'type': 'static_library',
          'sources': [
            'libtiff/tiffiop.h',
            'libtiff/tiffvers.h',
            'libtiff/tif_aux.c',
            'libtiff/tif_close.c',
            'libtiff/tif_codec.c',
            'libtiff/tif_color.c',
            'libtiff/tif_compress.c',
            'libtiff/tif_dir.c',
            'libtiff/tif_dirinfo.c',
            'libtiff/tif_dirread.c',
            'libtiff/tif_dirwrite.c',
            'libtiff/tif_dumpmode.c',
            'libtiff/tif_error.c',
            'libtiff/tif_extension.c',
            'libtiff/tif_fax3.c',
            'libtiff/tif_fax3sm.c',
            'libtiff/tif_flush.c',
            'libtiff/tif_getimage.c',
            'libtiff/tif_jpeg.c',
            'libtiff/tif_luv.c',
            'libtiff/tif_lzw.c',
            'libtiff/tif_next.c',
            'libtiff/tif_ojpeg.c',
            'libtiff/tif_open.c',
            'libtiff/tif_packbits.c',
            'libtiff/tif_pixarlog.c',
            'libtiff/tif_predict.c',
            'libtiff/tif_print.c',
            'libtiff/tif_read.c',
            'libtiff/tif_strip.c',
            'libtiff/tif_swab.c',
            'libtiff/tif_thunder.c',
            'libtiff/tif_tile.c',
            'libtiff/tif_version.c',
            'libtiff/tif_warning.c',
            'libtiff/tif_write.c',
            'libtiff/tif_zip.c',
          ],
          'conditions': [
            ['OS=="win"', {
              'defines!': [
                # Need to undefine the macro since it is redefined in
                # tif_ojpeg.c and tif_jpeg.c.
                'WIN32_LEAN_AND_MEAN',
              ],
            }],
          ],
        },
      ],
    }],
    ['pdf_use_skia!=1', {
      'targets': [
        {
          'target_name': 'fx_agg',
          'type': 'static_library',
          'sources': [
            'agg23/agg_basics.h',
            'agg23/agg_clip_liang_barsky.h',
            'agg23/agg_conv_dash.h',
            'agg23/agg_conv_stroke.h',
            'agg23/agg_curves.cpp',
            'agg23/agg_curves.h',
            'agg23/agg_path_storage.cpp',
            'agg23/agg_path_storage.h',
            'agg23/agg_rasterizer_scanline_aa.cpp',
            'agg23/agg_rasterizer_scanline_aa.h',
            'agg23/agg_renderer_scanline.h',
            'agg23/agg_rendering_buffer.h',
            'agg23/agg_scanline_u.h',
            'agg23/agg_vcgen_dash.cpp',
            'agg23/agg_vcgen_stroke.cpp',
          ],
          'conditions': [
            ['os_posix==1', {
              # library contains several enum vs non-enum conditionals.
              'cflags': [ '-Wno-extra', ],
            }],
          ],
          'variables': {
            'clang_warning_flags': [
              # calc_butt_cap() in agg_vcgen_stroke.cpp is unused.
              '-Wno-unused-function',
            ],
          },
        },
      ],
    }],
  ],
}
