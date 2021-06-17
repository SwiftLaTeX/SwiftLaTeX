//========================================================================
//
// WebFont.cc
//
// Copyright 2019 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "gmempp.h"
#include "GHash.h"
#include "FoFiTrueType.h"
#include "FoFiType1C.h"
#include "CharCodeToUnicode.h"
#include "WebFont.h"

WebFont::WebFont(GfxFont *gfxFontA, XRef *xref) {
  GfxFontType type;

  gfxFont = gfxFontA;
  fontBuf = NULL;
  ffTrueType = NULL;
  ffType1C = NULL;
  isOpenType = gFalse;

  type = gfxFont->getType();
  if (type == fontTrueType ||
      type == fontTrueTypeOT ||
      type == fontCIDType2 ||
      type == fontCIDType2OT) {
    if ((fontBuf = gfxFont->readEmbFontFile(xref, &fontLength))) {
      ffTrueType = FoFiTrueType::make(fontBuf, fontLength, 0);
    }
  } else if (type == fontType1C ||
	     type == fontCIDType0C) {
    if ((fontBuf = gfxFont->readEmbFontFile(xref, &fontLength))) {
      ffType1C = FoFiType1C::make(fontBuf, fontLength);
    }
  } else if (type == fontType1COT ||
	     type == fontCIDType0COT) {
    if ((fontBuf = gfxFont->readEmbFontFile(xref, &fontLength))) {
      isOpenType = gTrue;
    }
  }
}

WebFont::~WebFont() {
  delete ffTrueType;
  delete ffType1C;
  gfree(fontBuf);
}

GBool WebFont::canWriteTTF() {
  return ffTrueType != NULL;
}

GBool WebFont::canWriteOTF() {
  return ffType1C || isOpenType;
}

static void writeToFile(void *stream, const char *data, int len) {
  fwrite(data, 1, len, (FILE *)stream);
}

GBool WebFont::writeTTF(const char *fontFilePath) {
  FILE *out;
  int *codeToGID;
  Guchar *cmapTable;
  GBool freeCodeToGID;
  int nCodes, cmapTableLength;

  if (!ffTrueType) {
    return gFalse;
  }
  if (gfxFont->isCIDFont()) {
    codeToGID = ((GfxCIDFont *)gfxFont)->getCIDToGID();
    nCodes = ((GfxCIDFont *)gfxFont)->getCIDToGIDLen();
    if (!codeToGID) {
      nCodes = ffTrueType->getNumGlyphs();
    }
    freeCodeToGID = gFalse;
  } else {
    codeToGID = ((Gfx8BitFont *)gfxFont)->getCodeToGIDMap(ffTrueType);
    nCodes = 256;
    freeCodeToGID = gTrue;
  }
  cmapTable = makeUnicodeCmapTable(codeToGID, nCodes, &cmapTableLength);
  if (freeCodeToGID) {
    gfree(codeToGID);
  }
  if (!cmapTable) {
    return gFalse;
  }
  if (!(out = fopen(fontFilePath, "wb"))) {
    gfree(cmapTable);
    return gFalse;
  }
  ffTrueType->writeTTF(writeToFile, out, NULL, NULL,
		       cmapTable, cmapTableLength);
  fclose(out);
  gfree(cmapTable);
  return gTrue;
}

GBool WebFont::writeOTF(const char *fontFilePath) {
  int *codeToGID;
  Gushort *widths;
  Guchar *cmapTable;
  FILE *out;
  int nCodes, nWidths, cmapTableLength;

  if (ffType1C) {
    if (gfxFont->getType() == fontType1C) {
      codeToGID = ((Gfx8BitFont *)gfxFont)->getCodeToGIDMap(ffType1C);
      if (!(cmapTable = makeUnicodeCmapTable(codeToGID, 256,
					     &cmapTableLength))) {
	gfree(codeToGID);
	return gFalse;
      }
      widths = makeType1CWidths(codeToGID, 256, &nWidths);
      gfree(codeToGID);
    } else { // fontCIDType0C
      codeToGID = ffType1C->getCIDToGIDMap(&nCodes);
      if (!(cmapTable = makeUnicodeCmapTable(codeToGID, nCodes,
					     &cmapTableLength))) {
	gfree(codeToGID);
	return gFalse;
      }
      widths = makeCIDType0CWidths(codeToGID, nCodes, &nWidths);
      gfree(codeToGID);
    }
    if (!(out = fopen(fontFilePath, "wb"))) {
      gfree(cmapTable);
      gfree(widths);
      return gFalse;
    }
    ffType1C->convertToOpenType(writeToFile, out,
				nWidths, widths,
				cmapTable, cmapTableLength);
    fclose(out);
    gfree(cmapTable);
    gfree(widths);

  } else if (isOpenType) {
    if (!(out = fopen(fontFilePath, "wb"))) {
      return gFalse;
    }
    if (fwrite(fontBuf, 1, fontLength, out) != (Guint)fontLength) {
      fclose(out);
      return gFalse;
    }
    fclose(out);

  } else {
    return gFalse;
  }

  return gTrue;
}

Gushort *WebFont::makeType1CWidths(int *codeToGID, int nCodes,
				   int *nWidths) {
  Gushort *widths;
  Gushort width;
  int widthsLen, gid, i;

  widthsLen = ffType1C->getNumGlyphs();
  widths = (Gushort *)gmallocn(widthsLen, sizeof(Gushort));
  for (i = 0; i < widthsLen; ++i) {
    widths[i] = 0;
  }
  for (i = 0; i < nCodes; ++i) {
    gid = codeToGID[i];
    if (gid < 0 || gid >= widthsLen) {
      continue;
    }
    width = (Gushort)(((Gfx8BitFont *)gfxFont)->getWidth((Guchar)i)
		      * 1000 + 0.5);
    if (width == 0) {
      continue;
    }
    widths[gid] = width;
  }
  *nWidths = widthsLen;
  return widths;
}

Gushort *WebFont::makeCIDType0CWidths(int *codeToGID, int nCodes,
				      int *nWidths) {
  Gushort *widths;
  Gushort width;
  int widthsLen, gid, i;

  widthsLen = ffType1C->getNumGlyphs();
  widths = (Gushort *)gmallocn(widthsLen, sizeof(Gushort));
  for (i = 0 ; i < widthsLen; ++i) {
    widths[i] = 0;
  }
  for (i = 0; i < nCodes; ++i) {
    gid = codeToGID[i];
    if (gid < 0 || gid >= widthsLen) {
      continue;
    }
    width = (Gushort)(((GfxCIDFont *)gfxFont)->getWidth((CID)i)
		      * 1000 + 0.5);
    if (width == 0) {
      continue;
    }
    widths[gid] = width;
  }
  *nWidths = widthsLen;
  return widths;
}

Guchar *WebFont::makeUnicodeCmapTable(int *codeToGID, int nCodes,
				      int *unicodeCmapLength) {
  int *unicodeToGID;
  Guchar *cmapTable;
  int unicodeToGIDLength, nMappings, len;
  int nSegs, searchRange, entrySelector, rangeShift;
  int glyphIdOffset, idRangeOffset;
  int start, end, c, i;

  if (!(unicodeToGID = makeUnicodeToGID(codeToGID, nCodes,
					&unicodeToGIDLength))) {
    return NULL;
  }

  // count the valid code-to-glyph mappings, and the sequences of
  // consecutive valid mappings
  // (note: char code 65535 is used to mark the end of table)
  nMappings = 0;
  nSegs = 1; // count the last segment, mapping 65535
  for (c = 0; c < unicodeToGIDLength && c <= 65534; ++c) {
    if (unicodeToGID[c]) {
      ++nMappings;
      if (c == 0 || !unicodeToGID[c-1]) {
	++nSegs;
      }
    }
  }

  i = 1;
  entrySelector = 0;
  while (2 * i <= nSegs) {
    i *= 2;
    ++entrySelector;
  }
  searchRange = 1 << (entrySelector + 1);
  rangeShift = 2 * nSegs - searchRange;

  len = 28 + nSegs * 8 + nMappings * 2;
  cmapTable = (Guchar *)gmalloc(len);

  // header
  cmapTable[ 0] = 0x00;	// table version
  cmapTable[ 1] = 0x00;
  cmapTable[ 2] = 0x00;	// number of cmaps
  cmapTable[ 3] = 0x01;
  cmapTable[ 4] = 0x00;	// platform[0]
  cmapTable[ 5] = 0x03;
  cmapTable[ 6] = 0x00;	// encoding[0]
  cmapTable[ 7] = 0x01;
  cmapTable[ 8] = 0x00;	// offset[0]
  cmapTable[ 9] = 0x00;
  cmapTable[10] = 0x00;
  cmapTable[11] = 0x0c;

  // table info
  cmapTable[12] = 0x00;					// cmap format
  cmapTable[13] = 0x04;
  cmapTable[14] = (Guchar)((len - 12) >> 8);		// cmap length
  cmapTable[15] = (Guchar)(len - 12);
  cmapTable[16] = 0x00;					// cmap version
  cmapTable[17] = 0x00;
  cmapTable[18] = (Guchar)(nSegs >> 7);			// segCountX2
  cmapTable[19] = (Guchar)(nSegs << 1);
  cmapTable[20] = (Guchar)(searchRange >> 8);		// searchRange
  cmapTable[21] = (Guchar)searchRange;
  cmapTable[22] = (Guchar)(entrySelector >> 8);		// entrySelector
  cmapTable[23] = (Guchar)entrySelector;
  cmapTable[24] = (Guchar)(rangeShift >> 8);		// rangeShift
  cmapTable[25] = (Guchar)rangeShift;
  cmapTable[26 + nSegs*2    ] = 0;			// reservedPad
  cmapTable[26 + nSegs*2 + 1] = 0;

  i = 0;
  glyphIdOffset = 28 + nSegs*8;
  for (c = 0; c < unicodeToGIDLength && c <= 65534; ++c) {
    if (unicodeToGID[c]) {
      if (c == 0 || !unicodeToGID[c-1]) {
	start = c;
	cmapTable[28 + nSegs*2 + i*2    ] = (Guchar)(start >> 8);
	cmapTable[28 + nSegs*2 + i*2 + 1] = (Guchar)start;
	cmapTable[28 + nSegs*4 + i*2    ] = (Guchar)0;	// idDelta
	cmapTable[28 + nSegs*4 + i*2 + 1] = (Guchar)0;
	idRangeOffset = glyphIdOffset - (28 + nSegs*6 + i*2);
	cmapTable[28 + nSegs*6 + i*2    ] = (Guchar)(idRangeOffset >> 8);
	cmapTable[28 + nSegs*6 + i*2 + 1] = (Guchar)idRangeOffset;
      }
      if (c == 65534 || !unicodeToGID[c+1]) {
	end = c;
	cmapTable[26 + i*2    ] = (Guchar)(end >> 8);
	cmapTable[26 + i*2 + 1] = (Guchar)end;
	++i;
      }
      cmapTable[glyphIdOffset++] = (Guchar)(unicodeToGID[c] >> 8);
      cmapTable[glyphIdOffset++] = (Guchar)unicodeToGID[c];
    }
  }

  // last segment maps code 65535 to GID 0
  cmapTable[26 + i*2    ] = (Guchar)0xff;		// end
  cmapTable[26 + i*2 + 1] = (Guchar)0xff;
  cmapTable[28 + nSegs*2 + i*2    ] = (Guchar)0xff;	// start
  cmapTable[28 + nSegs*2 + i*2 + 1] = (Guchar)0xff;
  cmapTable[28 + nSegs*4 + i*2    ] = (Guchar)0;	// idDelta
  cmapTable[28 + nSegs*4 + i*2 + 1] = (Guchar)1;
  cmapTable[28 + nSegs*6 + i*2    ] = (Guchar)0;	// idRangeOffset
  cmapTable[28 + nSegs*6 + i*2 + 1] = (Guchar)0;

  gfree(unicodeToGID);

  *unicodeCmapLength = len;
  return cmapTable;
}

int *WebFont::makeUnicodeToGID(int *codeToGID, int nCodes,
			       int *unicodeToGIDLength) {
  int *unicodeToGID;
  CharCodeToUnicode *ctu;
  Unicode u[2];
  int len, size, newSize, uLen, c, gid;

  if (gfxFont->isCIDFont()) {
    if (!(ctu = ((GfxCIDFont *)gfxFont)->getToUnicode())) {
      return NULL;
    }
  } else {
    ctu = ((Gfx8BitFont *)gfxFont)->getToUnicode();
  }

  len = 0;
  size = 256;
  unicodeToGID = (int *)gmallocn(size, sizeof(int));
  memset(unicodeToGID, 0, size * sizeof(int));
  for (c = 0; c < nCodes; ++c) {
    gid = codeToGID ? codeToGID[c] : c;
    if (gid < 0 || gid >= 65536) {
      continue;
    }
    uLen = ctu->mapToUnicode(c, u, 2);
    if (uLen != 1) {
      continue;
    }
    if (u[0] >= 65536) {    // sanity check
      continue;
    }
    if ((int)u[0] >= size) {
      newSize = 2 * size;
      while ((int)u[0] >= newSize) {
	newSize *= 2;
      }
      unicodeToGID = (int *)greallocn(unicodeToGID, newSize, sizeof(int));
      memset(unicodeToGID + size, 0, (newSize - size) * sizeof(int));
      size = newSize;
    }
    unicodeToGID[u[0]] = gid;
    if ((int)u[0] >= len) {
      len = u[0] + 1;
    }
  }

  ctu->decRefCnt();

  *unicodeToGIDLength = len;
  return unicodeToGID;
}
