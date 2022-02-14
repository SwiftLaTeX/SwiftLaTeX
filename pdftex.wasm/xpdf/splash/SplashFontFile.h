//========================================================================
//
// SplashFontFile.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHFONTFILE_H
#define SPLASHFONTFILE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "SplashTypes.h"

#if MULTITHREADED
#include "GMutex.h"
#endif

class GString;
class SplashFontEngine;
class SplashFont;
class SplashFontFileID;

//------------------------------------------------------------------------
// SplashFontType
//------------------------------------------------------------------------

enum SplashFontType {
  splashFontType1,		// GfxFontType.fontType1
  splashFontType1C,		// GfxFontType.fontType1C
  splashFontOpenTypeT1C,	// GfxFontType.fontType1COT
  splashFontCID,		// GfxFontType.fontCIDType0/fontCIDType0C
  splashFontOpenTypeCFF,	// GfxFontType.fontCIDType0COT
  splashFontTrueType		// GfxFontType.fontTrueType/fontTrueTypeOT/
				//             fontCIDType2/fontCIDType2OT
};

//------------------------------------------------------------------------
// SplashFontFile
//------------------------------------------------------------------------

class SplashFontFile {
public:

  virtual ~SplashFontFile();

  // Create a new SplashFont, i.e., a scaled instance of this font
  // file.
  virtual SplashFont *makeFont(SplashCoord *mat, SplashCoord *textMat) = 0;

  // Get the font file ID.
  SplashFontFileID *getID() { return id; }

  // Increment the reference count.
  void incRefCnt();

  // Decrement the reference count.  If the new value is zero, delete
  // the SplashFontFile object.
  void decRefCnt();

protected:

  SplashFontFile(SplashFontFileID *idA,
		 SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
		 GString *fontBufA
#else
		 char *fileNameA, GBool deleteFileA
#endif
		 );

  SplashFontFileID *id;
  SplashFontType fontType;
#if LOAD_FONTS_FROM_MEM
  GString *fontBuf;
#else
  GString *fileName;
  GBool deleteFile;
#endif
#if MULTITHREADED
  GAtomicCounter refCnt;
#else
  int refCnt;
#endif

  friend class SplashFontEngine;
};

#endif
