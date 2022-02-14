//========================================================================
//
// SplashFTFontFile.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#if HAVE_FREETYPE_H

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "SplashFTFontEngine.h"
#include "SplashFTFont.h"
#include "SplashFTFontFile.h"

//------------------------------------------------------------------------
// SplashFTFontFile
//------------------------------------------------------------------------

SplashFontFile *SplashFTFontFile::loadType1Font(SplashFTFontEngine *engineA,
						SplashFontFileID *idA,
						SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
						GString *fontBufA,
#else
						char *fileNameA,
						GBool deleteFileA,
#endif
						const char **encA) {
  FT_Face faceA;
  int *codeToGIDA;
  const char *name;
  int i;

#if LOAD_FONTS_FROM_MEM
  if (FT_New_Memory_Face(engineA->lib, (FT_Byte *)fontBufA->getCString(),
			 fontBufA->getLength(), 0, &faceA)) {
#else
  if (FT_New_Face(engineA->lib, fileNameA, 0, &faceA)) {
#endif
    return NULL;
  }
  codeToGIDA = (int *)gmallocn(256, sizeof(int));
  for (i = 0; i < 256; ++i) {
    codeToGIDA[i] = 0;
    if ((name = encA[i])) {
      codeToGIDA[i] = (int)FT_Get_Name_Index(faceA, (char *)name);
    }
  }

  return new SplashFTFontFile(engineA, idA, fontTypeA,
#if LOAD_FONTS_FROM_MEM
			      fontBufA,
#else
			      fileNameA, deleteFileA,
#endif
			      faceA, codeToGIDA, 256);
}

SplashFontFile *SplashFTFontFile::loadCIDFont(SplashFTFontEngine *engineA,
					      SplashFontFileID *idA,
					      SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
					      GString *fontBufA,
#else
					      char *fileNameA,
					      GBool deleteFileA,
#endif
					      int *codeToGIDA,
					      int codeToGIDLenA) {
  FT_Face faceA;

#if LOAD_FONTS_FROM_MEM
  if (FT_New_Memory_Face(engineA->lib, (FT_Byte *)fontBufA->getCString(),
			 fontBufA->getLength(), 0, &faceA)) {
#else
  if (FT_New_Face(engineA->lib, fileNameA, 0, &faceA)) {
#endif
    return NULL;
  }

  return new SplashFTFontFile(engineA, idA, fontTypeA,
#if LOAD_FONTS_FROM_MEM
			      fontBufA,
#else
			      fileNameA, deleteFileA,
#endif
			      faceA, codeToGIDA, codeToGIDLenA);
}

SplashFontFile *SplashFTFontFile::loadTrueTypeFont(SplashFTFontEngine *engineA,
						   SplashFontFileID *idA,
						   SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
						   GString *fontBufA,
#else
						   char *fileNameA,
						   GBool deleteFileA,
#endif
						   int fontNum,
						   int *codeToGIDA,
						   int codeToGIDLenA) {
  FT_Face faceA;

#if LOAD_FONTS_FROM_MEM
  if (FT_New_Memory_Face(engineA->lib, (FT_Byte *)fontBufA->getCString(),
			 fontBufA->getLength(), fontNum, &faceA)) {
#else
  if (FT_New_Face(engineA->lib, fileNameA, fontNum, &faceA)) {
#endif
    return NULL;
  }

  return new SplashFTFontFile(engineA, idA, fontTypeA,
#if LOAD_FONTS_FROM_MEM
			      fontBufA,
#else
			      fileNameA, deleteFileA,
#endif
			      faceA, codeToGIDA, codeToGIDLenA);
}

SplashFTFontFile::SplashFTFontFile(SplashFTFontEngine *engineA,
				   SplashFontFileID *idA,
				   SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
				   GString *fontBufA,
#else
				   char *fileNameA, GBool deleteFileA,
#endif
				   FT_Face faceA,
				   int *codeToGIDA, int codeToGIDLenA):
#if LOAD_FONTS_FROM_MEM
  SplashFontFile(idA, fontTypeA, fontBufA)
#else
  SplashFontFile(idA, fontTypeA, fileNameA, deleteFileA)
#endif
{
  engine = engineA;
  face = faceA;
  codeToGID = codeToGIDA;
  codeToGIDLen = codeToGIDLenA;
}

SplashFTFontFile::~SplashFTFontFile() {
  if (face) {
    FT_Done_Face(face);
  }
  if (codeToGID) {
    gfree(codeToGID);
  }
}

SplashFont *SplashFTFontFile::makeFont(SplashCoord *mat,
				       SplashCoord *textMat) {
  SplashFont *font;

  font = new SplashFTFont(this, mat, textMat);
  font->initCache();
  return font;
}

#endif // HAVE_FREETYPE_H
