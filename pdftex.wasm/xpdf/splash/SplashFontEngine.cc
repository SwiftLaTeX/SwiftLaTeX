//========================================================================
//
// SplashFontEngine.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
#  include <unistd.h>
#endif
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "SplashMath.h"
#include "SplashFTFontEngine.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"
#include "SplashFont.h"
#include "SplashFontEngine.h"

#ifdef VMS
#if (__VMS_VER < 70000000)
extern "C" int unlink(char *filename);
#endif
#endif

//------------------------------------------------------------------------
// SplashFontEngine
//------------------------------------------------------------------------

SplashFontEngine::SplashFontEngine(
#if HAVE_FREETYPE_H
				   GBool enableFreeType,
				   Guint freeTypeFlags,
#endif
				   GBool aa) {
  int i;

  for (i = 0; i < splashFontCacheSize; ++i) {
    fontCache[i] = NULL;
  }

#if HAVE_FREETYPE_H
  if (enableFreeType) {
    ftEngine = SplashFTFontEngine::init(aa, freeTypeFlags);
  } else {
    ftEngine = NULL;
  }
#endif
}

SplashFontEngine::~SplashFontEngine() {
  int i;

  for (i = 0; i < splashFontCacheSize; ++i) {
    if (fontCache[i]) {
      delete fontCache[i];
    }
  }

#if HAVE_FREETYPE_H
  if (ftEngine) {
    delete ftEngine;
  }
#endif
}

SplashFontFile *SplashFontEngine::getFontFile(SplashFontFileID *id) {
  SplashFontFile *fontFile;
  int i;

  for (i = 0; i < splashFontCacheSize; ++i) {
    if (fontCache[i]) {
      fontFile = fontCache[i]->getFontFile();
      if (fontFile && fontFile->getID()->matches(id)) {
	return fontFile;
      }
    }
  }
  return NULL;
}

SplashFontFile *SplashFontEngine::loadType1Font(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
						GString *fontBuf,
#else
						char *fileName,
						GBool deleteFile,
#endif
						const char **enc) {
  SplashFontFile *fontFile;

  fontFile = NULL;
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadType1Font(idA,
#if LOAD_FONTS_FROM_MEM
				       fontBuf,
#else
				       fileName, deleteFile,
#endif
				       enc);
  }
#endif

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFontFile *SplashFontEngine::loadType1CFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
						 GString *fontBuf,
#else
						 char *fileName,
						 GBool deleteFile,
#endif
						 int *codeToGID,
						 const char **enc) {
  SplashFontFile *fontFile;

  fontFile = NULL;
  if (!fontFile) {
    gfree(codeToGID);
  }
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadType1CFont(idA,
#if LOAD_FONTS_FROM_MEM
					fontBuf,
#else
					fileName, deleteFile,
#endif
					enc);
  }
#endif

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFontFile *SplashFontEngine::loadOpenTypeT1CFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
						      GString *fontBuf,
#else
						      char *fileName,
						      GBool deleteFile,
#endif
						      int *codeToGID,
						      const char **enc) {
  SplashFontFile *fontFile;

  fontFile = NULL;
  if (!fontFile) {
    gfree(codeToGID);
  }
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadOpenTypeT1CFont(idA,
#if LOAD_FONTS_FROM_MEM
					     fontBuf,
#else
					     fileName, deleteFile,
#endif
					     enc);
  }
#endif

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFontFile *SplashFontEngine::loadCIDFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
					      GString *fontBuf,
#else
					      char *fileName,
					      GBool deleteFile,
#endif
					      int *codeToGID,
					      int codeToGIDLen) {
  SplashFontFile *fontFile;

  fontFile = NULL;
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadCIDFont(idA,
#if LOAD_FONTS_FROM_MEM
				     fontBuf,
#else
				     fileName, deleteFile,
#endif
				     codeToGID, codeToGIDLen);
  }
#endif

  if (!fontFile) {
    gfree(codeToGID);
  }

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFontFile *SplashFontEngine::loadOpenTypeCFFFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
						      GString *fontBuf,
#else
						      char *fileName,
						      GBool deleteFile,
#endif
						      int *codeToGID,
						      int codeToGIDLen) {
  SplashFontFile *fontFile;

  fontFile = NULL;
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadOpenTypeCFFFont(idA,
#if LOAD_FONTS_FROM_MEM
					     fontBuf,
#else
					     fileName, deleteFile,
#endif
					     codeToGID, codeToGIDLen);
  }
#endif

  if (!fontFile) {
    gfree(codeToGID);
  }

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFontFile *SplashFontEngine::loadTrueTypeFont(SplashFontFileID *idA,
#if LOAD_FONTS_FROM_MEM
						   GString *fontBuf,
#else
						   char *fileName,
						   GBool deleteFile,
#endif
						   int fontNum,
						   int *codeToGID,
						   int codeToGIDLen,
						   char *fontName) {
  SplashFontFile *fontFile;

  fontFile = NULL;
#if HAVE_FREETYPE_H
  if (!fontFile && ftEngine) {
    fontFile = ftEngine->loadTrueTypeFont(idA,
#if LOAD_FONTS_FROM_MEM
					  fontBuf,
#else
					  fileName, deleteFile,
#endif
					  fontNum, codeToGID, codeToGIDLen);
  }
#endif

  if (!fontFile) {
    gfree(codeToGID);
  }

#if !LOAD_FONTS_FROM_MEM && !defined(_WIN32) && !defined(__ANDROID__)
  // delete the (temporary) font file -- with Unix hard link
  // semantics, this will remove the last link; otherwise it will
  // return an error, leaving the file to be deleted later (if
  // loadXYZFont failed, the file will always be deleted)
  if (deleteFile) {
    unlink(fontFile ? fontFile->fileName->getCString() : fileName);
  }
#endif

  return fontFile;
}

SplashFont *SplashFontEngine::getFont(SplashFontFile *fontFile,
				      SplashCoord *textMat,
				      SplashCoord *ctm) {
  SplashCoord mat[4];
  SplashFont *font;
  int i, j;

  mat[0] = textMat[0] * ctm[0] + textMat[1] * ctm[2];
  mat[1] = -(textMat[0] * ctm[1] + textMat[1] * ctm[3]);
  mat[2] = textMat[2] * ctm[0] + textMat[3] * ctm[2];
  mat[3] = -(textMat[2] * ctm[1] + textMat[3] * ctm[3]);
  if (!splashCheckDet(mat[0], mat[1], mat[2], mat[3], 0.01)) {
    // avoid a singular (or close-to-singular) matrix
    mat[0] = 0.01;  mat[1] = 0;
    mat[2] = 0;     mat[3] = 0.01;
  }

  font = fontCache[0];
  if (font && font->matches(fontFile, mat, textMat)) {
    return font;
  }
  for (i = 1; i < splashFontCacheSize; ++i) {
    font = fontCache[i];
    if (font && font->matches(fontFile, mat, textMat)) {
      for (j = i; j > 0; --j) {
	fontCache[j] = fontCache[j-1];
      }
      fontCache[0] = font;
      return font;
    }
  }
  font = fontFile->makeFont(mat, textMat);
  if (fontCache[splashFontCacheSize - 1]) {
    delete fontCache[splashFontCacheSize - 1];
  }
  for (j = splashFontCacheSize - 1; j > 0; --j) {
    fontCache[j] = fontCache[j-1];
  }
  fontCache[0] = font;
  return font;
}
