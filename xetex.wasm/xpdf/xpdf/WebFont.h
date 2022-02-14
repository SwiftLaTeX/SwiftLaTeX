//========================================================================
//
// WebFont.h
//
// Modify/convert embedded PDF fonts to a form usable by web browsers.
//
// Copyright 2019 Glyph & Cog, LLC
//
//========================================================================

#ifndef WEBFONT_H
#define WEBFONT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "GfxFont.h"

class FoFiTrueType;
class FoFiType1C;
class XRef;

//------------------------------------------------------------------------

class WebFont {
public:

  WebFont(GfxFont *gfxFontA, XRef *xref);

  ~WebFont();

  // Returns true if the font is, or can be converted to, a TrueType
  // font.
  GBool canWriteTTF();

  // Returns true if the font is, or can be converted to, an OpenType
  // font.
  GBool canWriteOTF();

  // Write a TrueType (.ttf) file to [fontFilePath].  This can only be
  // called if canWriteTTF() returns true.  Returns true on success.
  GBool writeTTF(const char *fontFilePath);

  // Write an OpenType (.otf) file to [fontFilePath].  This can only
  // be called if canWriteOTF() returns true.  Returns true on
  // success.
  GBool writeOTF(const char *fontFilePath);

private:

  Gushort *makeType1CWidths(int *codeToGID, int nCodes, int *nWidths);
  Gushort *makeCIDType0CWidths(int *codeToGID, int nCodes, int *nWidths);
  Guchar *makeUnicodeCmapTable(int *codeToGID, int nCodes,
			       int *unicodeCmapLength);
  int *makeUnicodeToGID(int *codeToGID, int nCodes, int *unicodeToGIDLength);

  GfxFont *gfxFont;
  char *fontBuf;
  int fontLength;
  FoFiTrueType *ffTrueType;
  FoFiType1C *ffType1C;
  GBool isOpenType;
};

#endif
