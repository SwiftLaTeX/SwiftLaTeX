//========================================================================
//
// SplashFontEngine.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHFONTENGINE_H
#define SPLASHFONTENGINE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
class GString;

class SplashFTFontEngine;
class SplashDTFontEngine;
class SplashDT4FontEngine;
class SplashFontFile;
class SplashFontFileID;
class SplashFont;

//------------------------------------------------------------------------

#define splashFontCacheSize 16

#if HAVE_FREETYPE_H
#define splashFTNoHinting (1 << 0)
#endif

//------------------------------------------------------------------------
// SplashFontEngine
//------------------------------------------------------------------------

class SplashFontEngine {
public:

  // Create a font engine.
  SplashFontEngine(
#if HAVE_FREETYPE_H
		   GBool enableFreeType,
		   Guint freeTypeFlags,
#endif
		   GBool aa);

  ~SplashFontEngine();

  // Get a font file from the cache.  Returns NULL if there is no
  // matching entry in the cache.
  SplashFontFile *getFontFile(SplashFontFileID *id);

  // Load fonts - these create new SplashFontFile objects.
  SplashFontFile *loadType1Font(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				GString *fontBuf,
#else
				char *fileName, GBool deleteFile,
#endif
				const char **enc);
  SplashFontFile *loadType1CFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				 GString *fontBuf,
#else
				 char *fileName, GBool deleteFile,
#endif
				 int *codeToGID, const char **enc);
  SplashFontFile *loadOpenTypeT1CFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				      GString *fontBuf,
#else
				      char *fileName, GBool deleteFile,
#endif
				      int *codeToGID, const char **enc);
  SplashFontFile *loadCIDFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
			      GString *fontBuf,
#else
			      char *fileName, GBool deleteFile,
#endif
			      int *codeToGID, int codeToGIDLen);
  SplashFontFile *loadOpenTypeCFFFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				      GString *fontBuf,
#else
				      char *fileName, GBool deleteFile,
#endif
				      int *codeToGID, int codeToGIDLen);
  SplashFontFile *loadTrueTypeFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				   GString *fontBuf,
#else
				   char *fileName, GBool deleteFile,
#endif
				   int fontNum,
				   int *codeToGID, int codeToGIDLen,
				   char *fontName);

  // Get a font - this does a cache lookup first, and if not found,
  // creates a new SplashFont object and adds it to the cache.  The
  // matrix, mat = textMat * ctm:
  //    [ mat[0] mat[1] ]
  //    [ mat[2] mat[3] ]
  // specifies the font transform in PostScript style:
  //    [x' y'] = [x y] * mat
  // Note that the Splash y axis points downward.
  SplashFont *getFont(SplashFontFile *fontFile,
		      SplashCoord *textMat, SplashCoord *ctm);

private:

  SplashFont *fontCache[splashFontCacheSize];

#if HAVE_FREETYPE_H
  SplashFTFontEngine *ftEngine;
#endif
};

#endif
