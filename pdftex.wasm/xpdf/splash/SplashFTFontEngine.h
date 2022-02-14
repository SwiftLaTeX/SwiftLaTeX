//========================================================================
//
// SplashFTFontEngine.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHFTFONTENGINE_H
#define SPLASHFTFONTENGINE_H

#include <aconf.h>

#if HAVE_FREETYPE_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include "gtypes.h"
class GString;

class SplashFontFile;
class SplashFontFileID;

//------------------------------------------------------------------------
// SplashFTFontEngine
//------------------------------------------------------------------------

class SplashFTFontEngine {
public:

  static SplashFTFontEngine *init(GBool aaA, Guint flagsA);

  ~SplashFTFontEngine();

  // Load fonts.
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
				 const char **enc);
  SplashFontFile *loadOpenTypeT1CFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
				      GString *fontBuf,
#else
				      char *fileName, GBool deleteFile,
#endif
				      const char **enc);
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
				   int *codeToGID, int codeToGIDLen);

private:

  SplashFTFontEngine(GBool aaA, Guint flagsA, FT_Library libA);

  GBool aa;
  Guint flags;
  FT_Library lib;
  GBool useCIDs;

  friend class SplashFTFontFile;
  friend class SplashFTFont;
};

#endif // HAVE_FREETYPE_H

#endif
