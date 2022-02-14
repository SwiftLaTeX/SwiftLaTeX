//========================================================================
//
// FoFiType1C.cc
//
// Copyright 1999-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GHash.h"
#include "FoFiEncodings.h"
#include "FoFiType1C.h"

//------------------------------------------------------------------------

static char hexChars[17] = "0123456789ABCDEF";

//------------------------------------------------------------------------

GBool Type1COp::isZero() {
  switch (kind) {
  case type1COpInteger:  return intgr == 0;
  case type1COpFloat:    return flt == 0;
  case type1COpRational: return rat.num == 0;
  default:               return gFalse;   // shouldn't happen
  }
}

GBool Type1COp::isNegative() {
  switch (kind) {
  case type1COpInteger:  return intgr < 0;
  case type1COpFloat:    return flt < 0;
  case type1COpRational: return (rat.num < 0) != (rat.den < 0);
  default:               return gFalse;   // shouldn't happen
  }
}

int Type1COp::toInt() {
  switch (kind) {
  case type1COpInteger:  return intgr;
  case type1COpFloat:    return (int)flt;
  case type1COpRational: return rat.num / rat.den;
  default:               return 0;   // shouldn't happen
  }
}

double Type1COp::toFloat() {
  switch (kind) {
  case type1COpInteger:  return (double)intgr;
  case type1COpFloat:    return flt;
  case type1COpRational: return (double)rat.num / (double)rat.den;
  default:               return 0.0;   // shouldn't happen
  }
}

//------------------------------------------------------------------------
// FoFiType1C
//------------------------------------------------------------------------

FoFiType1C *FoFiType1C::make(char *fileA, int lenA) {
  FoFiType1C *ff;

  ff = new FoFiType1C(fileA, lenA, gFalse);
  if (!ff->parse()) {
    delete ff;
    return NULL;
  }
  return ff;
}

FoFiType1C *FoFiType1C::load(char *fileName) {
  FoFiType1C *ff;
  char *fileA;
  int lenA;

  if (!(fileA = FoFiBase::readFile(fileName, &lenA))) {
    return NULL;
  }
  ff = new FoFiType1C(fileA, lenA, gTrue);
  if (!ff->parse()) {
    delete ff;
    return NULL;
  }
  return ff;
}

FoFiType1C::FoFiType1C(char *fileA, int lenA, GBool freeFileDataA):
  FoFiBase(fileA, lenA, freeFileDataA)
{
  name = NULL;
  encoding = NULL;
  privateDicts = NULL;
  fdSelect = NULL;
  charset = NULL;
}

FoFiType1C::~FoFiType1C() {
  int i;

  if (name) {
    delete name;
  }
  if (encoding &&
      encoding != (char **)fofiType1StandardEncoding &&
      encoding != (char **)fofiType1ExpertEncoding) {
    for (i = 0; i < 256; ++i) {
      gfree(encoding[i]);
    }
    gfree(encoding);
  }
  if (privateDicts) {
    gfree(privateDicts);
  }
  if (fdSelect) {
    gfree(fdSelect);
  }
  if (charset &&
      charset != fofiType1CISOAdobeCharset &&
      charset != fofiType1CExpertCharset &&
      charset != fofiType1CExpertSubsetCharset) {
    gfree(charset);
  }
}

char *FoFiType1C::getName() {
  return name ? name->getCString() : (char *)NULL;
}

char **FoFiType1C::getEncoding() {
  return encoding;
}

GString *FoFiType1C::getGlyphName(int gid) {
  char buf[256];
  GBool ok;

  ok = gTrue;
  getString(charset[gid], buf, &ok);
  if (!ok) {
    return NULL;
  }
  return new GString(buf);
}

GHash *FoFiType1C::getNameToGIDMap() {
  GHash *map;
  char name[256];
  GBool ok;
  int gid;

  map = new GHash(gTrue);
  for (gid = 0; gid < nGlyphs; ++gid) {
    ok = gTrue;
    getString(charset[gid], name, &ok);
    if (ok) {
      map->add(new GString(name), gid);
    }
  }
  return map;
}

int *FoFiType1C::getCIDToGIDMap(int *nCIDs) {
  int *map;
  int n, i;

  // a CID font's top dict has ROS as the first operator
  if (topDict.firstOp != 0x0c1e) {
    *nCIDs = 0;
    return NULL;
  }

  // in a CID font, the charset data is the GID-to-CID mapping, so all
  // we have to do is reverse it
  n = 0;
  for (i = 0; i < nGlyphs; ++i) {
    if (charset[i] > n) {
      n = charset[i];
    }
  }
  ++n;
  map = (int *)gmallocn(n, sizeof(int));
  memset(map, 0, n * sizeof(int));
  for (i = 0; i < nGlyphs; ++i) {
    map[charset[i]] = i;
  }
  *nCIDs = n;
  return map;
}

void FoFiType1C::getFontMatrix(double *mat) {
  int i;

  if (topDict.firstOp == 0x0c1e && privateDicts[0].hasFontMatrix) {
    if (topDict.hasFontMatrix) {
      mat[0] = topDict.fontMatrix[0] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[1] * privateDicts[0].fontMatrix[2];
      mat[1] = topDict.fontMatrix[0] * privateDicts[0].fontMatrix[1] +
               topDict.fontMatrix[1] * privateDicts[0].fontMatrix[3];
      mat[2] = topDict.fontMatrix[2] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[3] * privateDicts[0].fontMatrix[2];
      mat[3] = topDict.fontMatrix[2] * privateDicts[0].fontMatrix[1] +
	       topDict.fontMatrix[3] * privateDicts[0].fontMatrix[3];
      mat[4] = topDict.fontMatrix[4] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[5] * privateDicts[0].fontMatrix[2];
      mat[5] = topDict.fontMatrix[4] * privateDicts[0].fontMatrix[1] +
	       topDict.fontMatrix[5] * privateDicts[0].fontMatrix[3];
    } else {
      for (i = 0; i < 6; ++i) {
	mat[i] = privateDicts[0].fontMatrix[i];
      }
    }
  } else {
    for (i = 0; i < 6; ++i) {
      mat[i] = topDict.fontMatrix[i];
    }
  }
}

void FoFiType1C::convertToType1(char *psName, const char **newEncoding,
				GBool ascii, FoFiOutputFunc outputFunc,
				void *outputStream) {
  int psNameLen;
  Type1CEexecBuf eb;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  GString *buf;
  char buf2[256];
  const char **enc;
  GBool ok;
  int i;

  if (psName) {
    psNameLen = (int)strlen(psName);
  } else {
    psName = name->getCString();
    psNameLen = name->getLength();
  }

  // write header and font dictionary, up to encoding
  ok = gTrue;
  (*outputFunc)(outputStream, "%!FontType1-1.0: ", 17);
  (*outputFunc)(outputStream, psName, psNameLen);
  if (topDict.versionSID != 0) {
    getString(topDict.versionSID, buf2, &ok);
    (*outputFunc)(outputStream, buf2, (int)strlen(buf2));
  }
  (*outputFunc)(outputStream, "\n", 1);
  // the dictionary needs room for 12 entries: the following 9, plus
  // Private and CharStrings (in the eexec section) and FID (which is
  // added by definefont)
  (*outputFunc)(outputStream, "12 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontInfo 10 dict dup begin\n", 28);
  if (topDict.versionSID != 0) {
    (*outputFunc)(outputStream, "/version ", 9);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.noticeSID != 0) {
    getString(topDict.noticeSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Notice ", 8);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.copyrightSID != 0) {
    getString(topDict.copyrightSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Copyright ", 11);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.fullNameSID != 0) {
    getString(topDict.fullNameSID, buf2, &ok);
    (*outputFunc)(outputStream, "/FullName ", 10);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.familyNameSID != 0) {
    getString(topDict.familyNameSID, buf2, &ok);
    (*outputFunc)(outputStream, "/FamilyName ", 12);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.weightSID != 0) {
    getString(topDict.weightSID, buf2, &ok);
    (*outputFunc)(outputStream, "/Weight ", 8);
    writePSString(buf2, outputFunc, outputStream);
    (*outputFunc)(outputStream, " readonly def\n", 14);
  }
  if (topDict.isFixedPitch) {
    (*outputFunc)(outputStream, "/isFixedPitch true def\n", 23);
  } else {
    (*outputFunc)(outputStream, "/isFixedPitch false def\n", 24);
  }
  buf = GString::format("/ItalicAngle {0:.4g} def\n", topDict.italicAngle);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GString::format("/UnderlinePosition {0:.4g} def\n",
			topDict.underlinePosition);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GString::format("/UnderlineThickness {0:.4g} def\n",
			topDict.underlineThickness);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "end readonly def\n", 17);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, psNameLen);
  (*outputFunc)(outputStream, " def\n", 5);
  buf = GString::format("/PaintType {0:d} def\n", topDict.paintType);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
  buf = GString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] readonly def\n",
			topDict.fontMatrix[0], topDict.fontMatrix[1],
			topDict.fontMatrix[2], topDict.fontMatrix[3],
			topDict.fontMatrix[4], topDict.fontMatrix[5]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] readonly def\n",
			topDict.fontBBox[0], topDict.fontBBox[1],
			topDict.fontBBox[2], topDict.fontBBox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  buf = GString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  if (topDict.uniqueID != 0) {
    buf = GString::format("/UniqueID {0:d} def\n", topDict.uniqueID);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }

  // write the encoding
  (*outputFunc)(outputStream, "/Encoding ", 10);
  if (!newEncoding && encoding == (char **)fofiType1StandardEncoding) {
    (*outputFunc)(outputStream, "StandardEncoding def\n", 21);
  } else {
    (*outputFunc)(outputStream, "256 array\n", 10);
    (*outputFunc)(outputStream,
		  "0 1 255 {1 index exch /.notdef put} for\n", 40);
    enc = newEncoding ? newEncoding : (const char **)encoding;
    for (i = 0; i < 256; ++i) {
      if (enc[i]) {
	buf = GString::format("dup {0:d} /{1:s} put\n", i, enc[i]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
    }
    (*outputFunc)(outputStream, "readonly def\n", 13);
  }
  (*outputFunc)(outputStream, "currentdict end\n", 16);

  // start the binary section
  (*outputFunc)(outputStream, "currentfile eexec\n", 18);
  eb.outputFunc = outputFunc;
  eb.outputStream = outputStream;
  eb.ascii = ascii;
  eb.r1 = 55665;
  eb.line = 0;

  // write the private dictionary
  eexecWrite(&eb, "\x83\xca\x73\xd5");
  eexecWrite(&eb, "dup /Private 32 dict dup begin\n");
  eexecWrite(&eb, "/RD {string currentfile exch readstring pop}"
	     " executeonly def\n");
  eexecWrite(&eb, "/ND {noaccess def} executeonly def\n");
  eexecWrite(&eb, "/NP {noaccess put} executeonly def\n");
  eexecWrite(&eb, "/MinFeature {16 16} def\n");
  eexecWrite(&eb, "/password 5839 def\n");
  if (privateDicts[0].nBlueValues) {
    eexecWrite(&eb, "/BlueValues [");
    for (i = 0; i < privateDicts[0].nBlueValues; ++i) {
      buf = GString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].blueValues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nOtherBlues) {
    eexecWrite(&eb, "/OtherBlues [");
    for (i = 0; i < privateDicts[0].nOtherBlues; ++i) {
      buf = GString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].otherBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nFamilyBlues) {
    eexecWrite(&eb, "/FamilyBlues [");
    for (i = 0; i < privateDicts[0].nFamilyBlues; ++i) {
      buf = GString::format("{0:s}{1:d}",
			    i > 0 ? " " : "", privateDicts[0].familyBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].nFamilyOtherBlues) {
    eexecWrite(&eb, "/FamilyOtherBlues [");
    for (i = 0; i < privateDicts[0].nFamilyOtherBlues; ++i) {
      buf = GString::format("{0:s}{1:d}", i > 0 ? " " : "",
			    privateDicts[0].familyOtherBlues[i]);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    eexecWrite(&eb, "] def\n");
  }
  if (privateDicts[0].blueScale != 0.039625) {
    buf = GString::format("/BlueScale {0:.4g} def\n",
			  privateDicts[0].blueScale);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].blueShift != 7) {
    buf = GString::format("/BlueShift {0:d} def\n", privateDicts[0].blueShift);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].blueFuzz != 1) {
    buf = GString::format("/BlueFuzz {0:d} def\n", privateDicts[0].blueFuzz);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].hasStdHW) {
    buf = GString::format("/StdHW [{0:.4g}] def\n", privateDicts[0].stdHW);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].hasStdVW) {
    buf = GString::format("/StdVW [{0:.4g}] def\n", privateDicts[0].stdVW);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].nStemSnapH) {
    // the StemSnapH array should be unique values in ascending order --
    // if not, just skip it
    for (i = 1; i < privateDicts[0].nStemSnapH; ++i) {
      if (privateDicts[0].stemSnapH[i-1] >= privateDicts[0].stemSnapH[i]) {
	break;
      }
    }
    if (i == privateDicts[0].nStemSnapH) {
      eexecWrite(&eb, "/StemSnapH [");
      for (i = 0; i < privateDicts[0].nStemSnapH; ++i) {
	buf = GString::format("{0:s}{1:.4g}",
			      i > 0 ? " " : "", privateDicts[0].stemSnapH[i]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
  }
  if (privateDicts[0].nStemSnapV) {
    // the StemSnapV array should be unique values in ascending order --
    // if not, just skip it
    for (i = 1; i < privateDicts[0].nStemSnapV; ++i) {
      if (privateDicts[0].stemSnapV[i-1] >= privateDicts[0].stemSnapV[i]) {
	break;
      }
    }
    if (i == privateDicts[0].nStemSnapV) {
      eexecWrite(&eb, "/StemSnapV [");
      for (i = 0; i < privateDicts[0].nStemSnapV; ++i) {
	buf = GString::format("{0:s}{1:.4g}",
			      i > 0 ? " " : "", privateDicts[0].stemSnapV[i]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
  }
  if (privateDicts[0].hasForceBold) {
    buf = GString::format("/ForceBold {0:s} def\n",
			  privateDicts[0].forceBold ? "true" : "false");
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].forceBoldThreshold != 0) {
    buf = GString::format("/ForceBoldThreshold {0:.4g} def\n",
			  privateDicts[0].forceBoldThreshold);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].languageGroup != 0) {
    buf = GString::format("/LanguageGroup {0:d} def\n",
			  privateDicts[0].languageGroup);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }
  if (privateDicts[0].expansionFactor != 0.06) {
    buf = GString::format("/ExpansionFactor {0:.4g} def\n",
			  privateDicts[0].expansionFactor);
    eexecWrite(&eb, buf->getCString());
    delete buf;
  }

  // set up subroutines
  ok = gTrue;
  getIndex(privateDicts[0].subrsOffset, &subrIdx, &ok);
  if (!ok) {
    subrIdx.pos = -1;
  }

  // write the CharStrings
  buf = GString::format("2 index /CharStrings {0:d} dict dup begin\n",
			nGlyphs);
  eexecWrite(&eb, buf->getCString());
  delete buf;
  for (i = 0; i < nGlyphs; ++i) {
    ok = gTrue;
    getIndexVal(&charStringsIdx, i, &val, &ok);
    if (ok) {
      getString(charset[i], buf2, &ok);
      if (ok) {
	eexecCvtGlyph(&eb, buf2, val.pos, val.len, &subrIdx, &privateDicts[0]);
      }
    }
  }
  eexecWrite(&eb, "end\n");
  eexecWrite(&eb, "end\n");
  eexecWrite(&eb, "readonly put\n");
  eexecWrite(&eb, "noaccess put\n");
  eexecWrite(&eb, "dup /FontName get exch definefont pop\n");
  eexecWrite(&eb, "mark currentfile closefile\n");

  // trailer
  if (ascii && eb.line > 0) {
    (*outputFunc)(outputStream, "\n", 1);
  }
  for (i = 0; i < 8; ++i) {
    (*outputFunc)(outputStream, "0000000000000000000000000000000000000000000000000000000000000000\n", 65);
  }
  (*outputFunc)(outputStream, "cleartomark\n", 12);
}

void FoFiType1C::convertToCIDType0(char *psName, int *codeMap, int nCodes,
				   FoFiOutputFunc outputFunc,
				   void *outputStream) {
  int *cidMap;
  GString *charStrings;
  int *charStringOffsets;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  int nCIDs, gdBytes;
  GString *buf;
  char buf2[256];
  GBool ok;
  int gid, offset, n, i, j, k;

  // compute the CID count and build the CID-to-GID mapping
  if (codeMap) {
    nCIDs = nCodes;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCodes; ++i) {
      if (codeMap[i] >= 0 && codeMap[i] < nGlyphs) {
	cidMap[i] = codeMap[i];
      } else {
	cidMap[i] = -1;
      }
    }
  } else if (topDict.firstOp == 0x0c1e) {
    nCIDs = 0;
    for (i = 0; i < nGlyphs; ++i) {
      if (charset[i] >= nCIDs) {
	nCIDs = charset[i] + 1;
      }
    }
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = -1;
    }
    for (i = 0; i < nGlyphs; ++i) {
      cidMap[charset[i]] = i;
    }
  } else {
    nCIDs = nGlyphs;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = i;
    }
  }

  // build the charstrings
  charStrings = new GString();
  charStringOffsets = (int *)gmallocn(nCIDs + 1, sizeof(int));
  for (i = 0; i < nCIDs; ++i) {
    charStringOffsets[i] = charStrings->getLength();
    if ((gid = cidMap[i]) >= 0) {
      ok = gTrue;
      getIndexVal(&charStringsIdx, gid, &val, &ok);
      if (ok) {
	getIndex(privateDicts[fdSelect ? fdSelect[gid] : 0].subrsOffset,
		 &subrIdx, &ok);
	if (!ok) {
	  subrIdx.pos = -1;
	}
	cvtGlyph(val.pos, val.len, charStrings,
		 &subrIdx, &privateDicts[fdSelect ? fdSelect[gid] : 0], gTrue);
      }
    }
  }
  charStringOffsets[nCIDs] = charStrings->getLength();

  // compute gdBytes = number of bytes needed for charstring offsets
  // (offset size needs to account for the charstring offset table,
  // with a worst case of five bytes per entry, plus the charstrings
  // themselves)
  i = (nCIDs + 1) * 5 + charStrings->getLength();
  if (i < 0x100) {
    gdBytes = 1;
  } else if (i < 0x10000) {
    gdBytes = 2;
  } else if (i < 0x1000000) {
    gdBytes = 3;
  } else {
    gdBytes = 4;
  }

  // begin the font dictionary
  (*outputFunc)(outputStream, "/CIDInit /ProcSet findresource begin\n", 37);
  (*outputFunc)(outputStream, "20 dict begin\n", 14);
  (*outputFunc)(outputStream, "/CIDFontName /", 14);
  (*outputFunc)(outputStream, psName, (int)strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/CIDFontType 0 def\n", 19);
  (*outputFunc)(outputStream, "/CIDSystemInfo 3 dict dup begin\n", 32);
  if (topDict.registrySID > 0 && topDict.orderingSID > 0) {
    ok = gTrue;
    getString(topDict.registrySID, buf2, &ok);
    if (ok) {
      (*outputFunc)(outputStream, "  /Registry (", 13);
      (*outputFunc)(outputStream, buf2, (int)strlen(buf2));
      (*outputFunc)(outputStream, ") def\n", 6);
    }
    ok = gTrue;
    getString(topDict.orderingSID, buf2, &ok);
    if (ok) {
      (*outputFunc)(outputStream, "  /Ordering (", 13);
      (*outputFunc)(outputStream, buf2, (int)strlen(buf2));
      (*outputFunc)(outputStream, ") def\n", 6);
    }
  } else {
    (*outputFunc)(outputStream, "  /Registry (Adobe) def\n", 24);
    (*outputFunc)(outputStream, "  /Ordering (Identity) def\n", 27);
  }
  buf = GString::format("  /Supplement {0:d} def\n", topDict.supplement);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "end def\n", 8);
  if (topDict.hasFontMatrix) {
    buf = GString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			  topDict.fontMatrix[0], topDict.fontMatrix[1],
			  topDict.fontMatrix[2], topDict.fontMatrix[3],
			  topDict.fontMatrix[4], topDict.fontMatrix[5]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  } else if (privateDicts[0].hasFontMatrix) {
    (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  } else {
    (*outputFunc)(outputStream,
		  "/FontMatrix [0.001 0 0 0.001 0 0] def\n", 38);
  }
  buf = GString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] def\n",
			topDict.fontBBox[0], topDict.fontBBox[1],
			topDict.fontBBox[2], topDict.fontBBox[3]);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FontInfo 1 dict dup begin\n", 27);
  (*outputFunc)(outputStream, "  /FSType 8 def\n", 16);
  (*outputFunc)(outputStream, "end def\n", 8);

  // CIDFont-specific entries
  buf = GString::format("/CIDCount {0:d} def\n", nCIDs);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/FDBytes 1 def\n", 15);
  buf = GString::format("/GDBytes {0:d} def\n", gdBytes);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  (*outputFunc)(outputStream, "/CIDMapOffset 0 def\n", 20);
  if (topDict.paintType != 0) {
    buf = GString::format("/PaintType {0:d} def\n", topDict.paintType);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    buf = GString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }

  // FDArray entry
  buf = GString::format("/FDArray {0:d} array\n", nFDs);
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;
  for (i = 0; i < nFDs; ++i) {
    buf = GString::format("dup {0:d} 10 dict begin\n", i);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
    if (privateDicts[i].hasFontMatrix) {
      buf = GString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			    privateDicts[i].fontMatrix[0],
			    privateDicts[i].fontMatrix[1],
			    privateDicts[i].fontMatrix[2],
			    privateDicts[i].fontMatrix[3],
			    privateDicts[i].fontMatrix[4],
			    privateDicts[i].fontMatrix[5]);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    } else {
      (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
    }
    buf = GString::format("/PaintType {0:d} def\n", topDict.paintType);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/Private 32 dict begin\n", 23);
    if (privateDicts[i].nBlueValues) {
      (*outputFunc)(outputStream, "/BlueValues [", 13);
      for (j = 0; j < privateDicts[i].nBlueValues; ++j) {
	buf = GString::format("{0:s}{1:d}",
			      j > 0 ? " " : "", privateDicts[i].blueValues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nOtherBlues) {
      (*outputFunc)(outputStream, "/OtherBlues [", 13);
      for (j = 0; j < privateDicts[i].nOtherBlues; ++j) {
	buf = GString::format("{0:s}{1:d}",
			      j > 0 ? " " : "", privateDicts[i].otherBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nFamilyBlues) {
      (*outputFunc)(outputStream, "/FamilyBlues [", 14);
      for (j = 0; j < privateDicts[i].nFamilyBlues; ++j) {
	buf = GString::format("{0:s}{1:d}",
			      j > 0 ? " " : "",
			      privateDicts[i].familyBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].nFamilyOtherBlues) {
      (*outputFunc)(outputStream, "/FamilyOtherBlues [", 19);
      for (j = 0; j < privateDicts[i].nFamilyOtherBlues; ++j) {
	buf = GString::format("{0:s}{1:d}", j > 0 ? " " : "",
			      privateDicts[i].familyOtherBlues[j]);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
      (*outputFunc)(outputStream, "] def\n", 6);
    }
    if (privateDicts[i].blueScale != 0.039625) {
      buf = GString::format("/BlueScale {0:.4g} def\n",
			    privateDicts[i].blueScale);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].blueShift != 7) {
      buf = GString::format("/BlueShift {0:d} def\n",
			    privateDicts[i].blueShift);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].blueFuzz != 1) {
      buf = GString::format("/BlueFuzz {0:d} def\n", privateDicts[i].blueFuzz);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].hasStdHW) {
      buf = GString::format("/StdHW [{0:.4g}] def\n", privateDicts[i].stdHW);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].hasStdVW) {
      buf = GString::format("/StdVW [{0:.4g}] def\n", privateDicts[i].stdVW);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].nStemSnapH) {
      // the StemSnapH array should be unique values in ascending order --
      // if not, just skip it
      for (j = 1; j < privateDicts[i].nStemSnapH; ++j) {
	if (privateDicts[i].stemSnapH[j-1] >= privateDicts[i].stemSnapH[j]) {
	  break;
	}
      }
      if (j == privateDicts[i].nStemSnapH) {
	(*outputFunc)(outputStream, "/StemSnapH [", 12);
	for (j = 0; j < privateDicts[i].nStemSnapH; ++j) {
	  buf = GString::format("{0:s}{1:.4g}",
				j > 0 ? " " : "", privateDicts[i].stemSnapH[j]);
	  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	  delete buf;
	}
	(*outputFunc)(outputStream, "] def\n", 6);
      }
    }
    if (privateDicts[i].nStemSnapV) {
      // the StemSnapV array should be unique values in ascending order --
      // if not, just skip it
      for (j = 1; j < privateDicts[i].nStemSnapV; ++j) {
	if (privateDicts[i].stemSnapV[j-1] >= privateDicts[i].stemSnapV[j]) {
	  break;
	}
      }
      if (j == privateDicts[i].nStemSnapV) {
	(*outputFunc)(outputStream, "/StemSnapV [", 12);
	for (j = 0; j < privateDicts[i].nStemSnapV; ++j) {
	  buf = GString::format("{0:s}{1:.4g}",
				j > 0 ? " " : "", privateDicts[i].stemSnapV[j]);
	  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	  delete buf;
	}
	(*outputFunc)(outputStream, "] def\n", 6);
      }
    }
    if (privateDicts[i].hasForceBold) {
      buf = GString::format("/ForceBold {0:s} def\n",
			    privateDicts[i].forceBold ? "true" : "false");
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].forceBoldThreshold != 0) {
      buf = GString::format("/ForceBoldThreshold {0:.4g} def\n",
			    privateDicts[i].forceBoldThreshold);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].languageGroup != 0) {
      buf = GString::format("/LanguageGroup {0:d} def\n",
			    privateDicts[i].languageGroup);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (privateDicts[i].expansionFactor != 0.06) {
      buf = GString::format("/ExpansionFactor {0:.4g} def\n",
			    privateDicts[i].expansionFactor);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "currentdict end def\n", 20);
    (*outputFunc)(outputStream, "currentdict end put\n", 20);
  }
  (*outputFunc)(outputStream, "def\n", 4);

  // start the binary section
  offset = (nCIDs + 1) * (1 + gdBytes);
  buf = GString::format("(Hex) {0:d} StartData\n",
			offset + charStrings->getLength());
  (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
  delete buf;

  // write the charstring offset (CIDMap) table
  for (i = 0; i <= nCIDs; i += 6) {
    for (j = 0; j < 6 && i+j <= nCIDs; ++j) {
      if (i+j < nCIDs && cidMap[i+j] >= 0 && fdSelect) {
	buf2[0] = (char)fdSelect[cidMap[i+j]];
      } else {
	buf2[0] = (char)0;
      }
      n = offset + charStringOffsets[i+j];
      for (k = gdBytes; k >= 1; --k) {
	buf2[k] = (char)(n & 0xff);
	n >>= 8;
      }
      for (k = 0; k <= gdBytes; ++k) {
	buf = GString::format("{0:02x}", buf2[k] & 0xff);
	(*outputFunc)(outputStream, buf->getCString(), buf->getLength());
	delete buf;
      }
    }
    (*outputFunc)(outputStream, "\n", 1);
  }

  // write the charstring data
  n = charStrings->getLength();
  for (i = 0; i < n; i += 32) {
    for (j = 0; j < 32 && i+j < n; ++j) {
      buf = GString::format("{0:02x}", charStrings->getChar(i+j) & 0xff);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (i + 32 >= n) {
      (*outputFunc)(outputStream, ">", 1);
    }
    (*outputFunc)(outputStream, "\n", 1);
  }

  gfree(charStringOffsets);
  delete charStrings;
  gfree(cidMap);
}

void FoFiType1C::convertToType0(char *psName, int *codeMap, int nCodes,
				FoFiOutputFunc outputFunc,
				void *outputStream) {
  int *cidMap;
  Type1CIndex subrIdx;
  Type1CIndexVal val;
  int nCIDs;
  GString *buf;
  Type1CEexecBuf eb;
  GBool ok;
  int fd, i, j, k;

  // compute the CID count and build the CID-to-GID mapping
  if (codeMap) {
    nCIDs = nCodes;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCodes; ++i) {
      if (codeMap[i] >= 0 && codeMap[i] < nGlyphs) {
	cidMap[i] = codeMap[i];
      } else {
	cidMap[i] = -1;
      }
    }
  } else if (topDict.firstOp == 0x0c1e) {
    nCIDs = 0;
    for (i = 0; i < nGlyphs; ++i) {
      if (charset[i] >= nCIDs) {
	nCIDs = charset[i] + 1;
      }
    }
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = -1;
    }
    for (i = 0; i < nGlyphs; ++i) {
      cidMap[charset[i]] = i;
    }
  } else {
    nCIDs = nGlyphs;
    cidMap = (int *)gmallocn(nCIDs, sizeof(int));
    for (i = 0; i < nCIDs; ++i) {
      cidMap[i] = i;
    }
  }

  // write the descendant Type 1 fonts
  for (i = 0; i < nCIDs; i += 256) {

    //~ this assumes that all CIDs in this block have the same FD --
    //~ to handle multiple FDs correctly, need to somehow divide the
    //~ font up by FD; as a kludge we ignore CID 0, which is .notdef
    fd = 0;
    // if fdSelect is NULL, we have an 8-bit font, so just leave fd=0
    if (fdSelect) {
      for (j = i==0 ? 1 : 0; j < 256 && i+j < nCIDs; ++j) {
	if (cidMap[i+j] >= 0) {
	  fd = fdSelect[cidMap[i+j]];
	  break;
	}
      }
    }

    // font dictionary (unencrypted section)
    (*outputFunc)(outputStream, "16 dict begin\n", 14);
    (*outputFunc)(outputStream, "/FontName /", 11);
    (*outputFunc)(outputStream, psName, (int)strlen(psName));
    buf = GString::format("_{0:02x} def\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    (*outputFunc)(outputStream, "/FontType 1 def\n", 16);
    if (privateDicts[fd].hasFontMatrix) {
      buf = GString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			    privateDicts[fd].fontMatrix[0],
			    privateDicts[fd].fontMatrix[1],
			    privateDicts[fd].fontMatrix[2],
			    privateDicts[fd].fontMatrix[3],
			    privateDicts[fd].fontMatrix[4],
			    privateDicts[fd].fontMatrix[5]);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    } else if (topDict.hasFontMatrix) {
      (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
    } else {
      (*outputFunc)(outputStream,
		    "/FontMatrix [0.001 0 0 0.001 0 0] def\n", 38);
    }
    buf = GString::format("/FontBBox [{0:.4g} {1:.4g} {2:.4g} {3:.4g}] def\n",
			  topDict.fontBBox[0], topDict.fontBBox[1],
			  topDict.fontBBox[2], topDict.fontBBox[3]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    buf = GString::format("/PaintType {0:d} def\n", topDict.paintType);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
    if (topDict.paintType != 0) {
      buf = GString::format("/StrokeWidth {0:.4g} def\n", topDict.strokeWidth);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "/Encoding 256 array\n", 20);
    for (j = 0; j < 256 && i+j < nCIDs; ++j) {
      buf = GString::format("dup {0:d} /c{1:02x} put\n", j, j);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    if (j < 256) {
      buf = GString::format("{0:d} 1 255 {{ 1 index exch /.notdef put }} for\n",
			    j);
      (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
      delete buf;
    }
    (*outputFunc)(outputStream, "readonly def\n", 13);
    (*outputFunc)(outputStream, "currentdict end\n", 16);

    // start the binary section
    (*outputFunc)(outputStream, "currentfile eexec\n", 18);
    eb.outputFunc = outputFunc;
    eb.outputStream = outputStream;
    eb.ascii = gTrue;
    eb.r1 = 55665;
    eb.line = 0;

    // start the private dictionary
    eexecWrite(&eb, "\x83\xca\x73\xd5");
    eexecWrite(&eb, "dup /Private 32 dict dup begin\n");
    eexecWrite(&eb, "/RD {string currentfile exch readstring pop}"
	       " executeonly def\n");
    eexecWrite(&eb, "/ND {noaccess def} executeonly def\n");
    eexecWrite(&eb, "/NP {noaccess put} executeonly def\n");
    eexecWrite(&eb, "/MinFeature {16 16} def\n");
    eexecWrite(&eb, "/password 5839 def\n");
    if (privateDicts[fd].nBlueValues) {
      eexecWrite(&eb, "/BlueValues [");
      for (k = 0; k < privateDicts[fd].nBlueValues; ++k) {
	buf = GString::format("{0:s}{1:d}",
			      k > 0 ? " " : "",
			      privateDicts[fd].blueValues[k]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
    if (privateDicts[fd].nOtherBlues) {
      eexecWrite(&eb, "/OtherBlues [");
      for (k = 0; k < privateDicts[fd].nOtherBlues; ++k) {
	buf = GString::format("{0:s}{1:d}",
			      k > 0 ? " " : "",
			      privateDicts[fd].otherBlues[k]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
    if (privateDicts[fd].nFamilyBlues) {
      eexecWrite(&eb, "/FamilyBlues [");
      for (k = 0; k < privateDicts[fd].nFamilyBlues; ++k) {
	buf = GString::format("{0:s}{1:d}", k > 0 ? " " : "",
			      privateDicts[fd].familyBlues[k]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
    if (privateDicts[fd].nFamilyOtherBlues) {
      eexecWrite(&eb, "/FamilyOtherBlues [");
      for (k = 0; k < privateDicts[fd].nFamilyOtherBlues; ++k) {
	buf = GString::format("{0:s}{1:d}", k > 0 ? " " : "",
			      privateDicts[fd].familyOtherBlues[k]);
	eexecWrite(&eb, buf->getCString());
	delete buf;
      }
      eexecWrite(&eb, "] def\n");
    }
    if (privateDicts[fd].blueScale != 0.039625) {
      buf = GString::format("/BlueScale {0:.4g} def\n",
			    privateDicts[fd].blueScale);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].blueShift != 7) {
      buf = GString::format("/BlueShift {0:d} def\n",
			    privateDicts[fd].blueShift);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].blueFuzz != 1) {
      buf = GString::format("/BlueFuzz {0:d} def\n",
			    privateDicts[fd].blueFuzz);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].hasStdHW) {
      buf = GString::format("/StdHW [{0:.4g}] def\n", privateDicts[fd].stdHW);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].hasStdVW) {
      buf = GString::format("/StdVW [{0:.4g}] def\n", privateDicts[fd].stdVW);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].nStemSnapH) {
      // the StemSnapH array should be unique values in ascending order --
      // if not, just skip it
      for (k = 1; k < privateDicts[fd].nStemSnapH; ++k) {
	if (privateDicts[fd].stemSnapH[k-1] >= privateDicts[fd].stemSnapH[k]) {
	  break;
	}
      }
      if (k == privateDicts[fd].nStemSnapH) {
	eexecWrite(&eb, "/StemSnapH [");
	for (k = 0; k < privateDicts[fd].nStemSnapH; ++k) {
	  buf = GString::format("{0:s}{1:.4g}",
				k > 0 ? " " : "",
				privateDicts[fd].stemSnapH[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
    }
    if (privateDicts[fd].nStemSnapV) {
      // the StemSnapV array should be unique values in ascending order --
      // if not, just skip it
      for (k = 1; k < privateDicts[fd].nStemSnapV; ++k) {
	if (privateDicts[fd].stemSnapV[k-1] >= privateDicts[fd].stemSnapV[k]) {
	  break;
	}
      }
      if (k == privateDicts[fd].nStemSnapV) {
	eexecWrite(&eb, "/StemSnapV [");
	for (k = 0; k < privateDicts[fd].nStemSnapV; ++k) {
	  buf = GString::format("{0:s}{1:.4g}",
				k > 0 ? " " : "",
				privateDicts[fd].stemSnapV[k]);
	  eexecWrite(&eb, buf->getCString());
	  delete buf;
	}
	eexecWrite(&eb, "] def\n");
      }
    }
    if (privateDicts[fd].hasForceBold) {
      buf = GString::format("/ForceBold {0:s} def\n",
			    privateDicts[fd].forceBold ? "true" : "false");
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].forceBoldThreshold != 0) {
      buf = GString::format("/ForceBoldThreshold {0:.4g} def\n",
			    privateDicts[fd].forceBoldThreshold);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].languageGroup != 0) {
      buf = GString::format("/LanguageGroup {0:d} def\n",
			    privateDicts[fd].languageGroup);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }
    if (privateDicts[fd].expansionFactor != 0.06) {
      buf = GString::format("/ExpansionFactor {0:.4g} def\n",
			    privateDicts[fd].expansionFactor);
      eexecWrite(&eb, buf->getCString());
      delete buf;
    }

    // set up the subroutines
    ok = gTrue;
    getIndex(privateDicts[fd].subrsOffset, &subrIdx, &ok);
    if (!ok) {
      subrIdx.pos = -1;
    }

    // start the CharStrings
    eexecWrite(&eb, "2 index /CharStrings 256 dict dup begin\n");

    // write the .notdef CharString
    ok = gTrue;
    getIndexVal(&charStringsIdx, 0, &val, &ok);
    if (ok) {
      eexecCvtGlyph(&eb, ".notdef", val.pos, val.len,
		    &subrIdx, &privateDicts[fd]);
    }

    // write the CharStrings
    for (j = 0; j < 256 && i+j < nCIDs; ++j) {
      if (cidMap[i+j] >= 0) {
	ok = gTrue;
	getIndexVal(&charStringsIdx, cidMap[i+j], &val, &ok);
	if (ok) {
	  buf = GString::format("c{0:02x}", j);
	  eexecCvtGlyph(&eb, buf->getCString(), val.pos, val.len,
			&subrIdx, &privateDicts[fd]);
	  delete buf;
	}
      }
    }
    eexecWrite(&eb, "end\n");
    eexecWrite(&eb, "end\n");
    eexecWrite(&eb, "readonly put\n");
    eexecWrite(&eb, "noaccess put\n");
    eexecWrite(&eb, "dup /FontName get exch definefont pop\n");
    eexecWrite(&eb, "mark currentfile closefile\n");

    // trailer
    if (eb.line > 0) {
      (*outputFunc)(outputStream, "\n", 1);
    }
    for (j = 0; j < 8; ++j) {
      (*outputFunc)(outputStream, "0000000000000000000000000000000000000000000000000000000000000000\n", 65);
    }
    (*outputFunc)(outputStream, "cleartomark\n", 12);
  }

  // write the Type 0 parent font
  (*outputFunc)(outputStream, "16 dict begin\n", 14);
  (*outputFunc)(outputStream, "/FontName /", 11);
  (*outputFunc)(outputStream, psName, (int)strlen(psName));
  (*outputFunc)(outputStream, " def\n", 5);
  (*outputFunc)(outputStream, "/FontType 0 def\n", 16);
  if (topDict.hasFontMatrix) {
    buf = GString::format("/FontMatrix [{0:.8g} {1:.8g} {2:.8g} {3:.8g} {4:.8g} {5:.8g}] def\n",
			  topDict.fontMatrix[0], topDict.fontMatrix[1],
			  topDict.fontMatrix[2], topDict.fontMatrix[3],
			  topDict.fontMatrix[4], topDict.fontMatrix[5]);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  } else {
    (*outputFunc)(outputStream, "/FontMatrix [1 0 0 1 0 0] def\n", 30);
  }
  (*outputFunc)(outputStream, "/FMapType 2 def\n", 16);
  (*outputFunc)(outputStream, "/Encoding [\n", 12);
  for (i = 0; i < nCIDs; i += 256) {
    buf = GString::format("{0:d}\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "/FDepVector [\n", 14);
  for (i = 0; i < nCIDs; i += 256) {
    (*outputFunc)(outputStream, "/", 1);
    (*outputFunc)(outputStream, psName, (int)strlen(psName));
    buf = GString::format("_{0:02x} findfont\n", i >> 8);
    (*outputFunc)(outputStream, buf->getCString(), buf->getLength());
    delete buf;
  }
  (*outputFunc)(outputStream, "] def\n", 6);
  (*outputFunc)(outputStream, "FontName currentdict end definefont pop\n", 40);

  gfree(cidMap);
}

void FoFiType1C::eexecCvtGlyph(Type1CEexecBuf *eb, const char *glyphName,
			       int offset, int nBytes,
			       Type1CIndex *subrIdx,
			       Type1CPrivateDict *pDict) {
  GString *buf;
  GString *charBuf;

  // generate the charstring
  charBuf = new GString();
  cvtGlyph(offset, nBytes, charBuf, subrIdx, pDict, gTrue);

  buf = GString::format("/{0:s} {1:d} RD ", glyphName, charBuf->getLength());
  eexecWrite(eb, buf->getCString());
  delete buf;
  eexecWriteCharstring(eb, (Guchar *)charBuf->getCString(),
		       charBuf->getLength());
  eexecWrite(eb, " ND\n");

  delete charBuf;
}

void FoFiType1C::cvtGlyph(int offset, int nBytes, GString *charBuf,
			  Type1CIndex *subrIdx, Type1CPrivateDict *pDict,
			  GBool top) {
  Type1CIndexVal val;
  Type1COp zero, tmp;
  GBool ok, dInt;
  double d, dx, dy;
  Gushort r2;
  Guchar byte;
  int pos, subrBias, start, num, den, i, k;

  start = charBuf->getLength();
  if (top) {
    charBuf->append((char)73);
    charBuf->append((char)58);
    charBuf->append((char)147);
    charBuf->append((char)134);
    nOps = 0;
    nHints = 0;
    firstOp = gTrue;
    openPath = gFalse;
  }

  zero.kind = type1COpInteger;
  zero.intgr = 0;

  pos = offset;
  while (pos < offset + nBytes) {
    ok = gTrue;
    pos = getOp(pos, gTrue, &ok);
    if (!ok) {
      break;
    }
    if (ops[nOps - 1].kind == type1COpOperator) {
      --nOps; // drop the operator
      switch (ops[nOps].op) {
      case 0x0001:		// hstem
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hstem", nOps);
	}
	d = 0;
	dInt = gTrue;
	for (k = 0; k+1 < nOps; k += 2) {
	  // convert Type 2 edge hints (-20 or -21) to Type 1 ghost hints
	  if (ops[k+1].isNegative()) {
	    d += ops[k].toFloat() + ops[k+1].toFloat();
	    dInt &= ops[k].kind == type1COpInteger;
	    dInt &= ops[k+1].kind == type1COpInteger;
	    if (dInt) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = (int)(d + 0.5);
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = d;
	    }
	    cvtNum(tmp, charBuf);
	    if (ops[k+1].kind == type1COpInteger) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = -ops[k+1].intgr;
	    } else if (ops[k+1].kind == type1COpRational) {
	      tmp.kind = type1COpRational;
	      tmp.rat.num = -ops[k+1].rat.num;
	      tmp.rat.den = ops[k+1].rat.den;
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = -ops[k+1].toFloat();
	    }
	    cvtNum(tmp, charBuf);
	  } else {
	    d += ops[k].toFloat();
	    dInt &= ops[k].kind == type1COpInteger;
	    if (dInt) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = (int)(d + 0.5);
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = d;
	    }
	    cvtNum(tmp, charBuf);
	    cvtNum(ops[k+1], charBuf);
	    d += ops[k+1].toFloat();
	    dInt &= ops[k+1].kind == type1COpInteger;
	  }
	  charBuf->append((char)1);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0003:		// vstem
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vstem", nOps);
	}
	d = 0;
	dInt = gTrue;
	for (k = 0; k+1 < nOps; k += 2) {
	  // convert Type 2 edge hints (-20 or -21) to Type 1 ghost hints
	  if (ops[k+1].isNegative()) {
	    d += ops[k].toFloat() + ops[k+1].toFloat();
	    dInt &= ops[k].kind == type1COpInteger;
	    dInt &= ops[k+1].kind == type1COpInteger;
	    if (dInt) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = (int)(d + 0.5);
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = d;
	    }
	    cvtNum(tmp, charBuf);
	    if (ops[k+1].kind == type1COpInteger) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = -ops[k+1].intgr;
	    } else if (ops[k+1].kind == type1COpRational) {
	      tmp.kind = type1COpRational;
	      tmp.rat.num = -ops[k+1].rat.num;
	      tmp.rat.den = ops[k+1].rat.den;
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = -ops[k+1].toFloat();
	    }
	    cvtNum(tmp, charBuf);
	  } else {
	    d += ops[k].toFloat();
	    dInt &= ops[k].kind == type1COpInteger;
	    if (dInt) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = (int)(d + 0.5);
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = d;
	    }
	    cvtNum(tmp, charBuf);
	    cvtNum(ops[k+1], charBuf);
	    d += ops[k+1].toFloat();
	    dInt &= ops[k+1].kind == type1COpInteger;
	  }
	  charBuf->append((char)3);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0004:		// vmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 2, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vmoveto", nOps);
	}
	cvtNum(ops[0], charBuf);
	charBuf->append((char)4);
	nOps = 0;
	break;
      case 0x0005:		// rlineto
	if (nOps < 2 || nOps % 2 != 0) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 rlineto", nOps);
	}
	for (k = 0; k+1 < nOps; k += 2) {
	  cvtNum(ops[k], charBuf);
	  cvtNum(ops[k+1], charBuf);
	  charBuf->append((char)5);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0006:		// hlineto
	if (nOps < 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hlineto", nOps);
	}
	for (k = 0; k < nOps; ++k) {
	  cvtNum(ops[k], charBuf);
	  charBuf->append((char)((k & 1) ? 7 : 6));
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0007:		// vlineto
	if (nOps < 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vlineto", nOps);
	}
	for (k = 0; k < nOps; ++k) {
	  cvtNum(ops[k], charBuf);
	  charBuf->append((char)((k & 1) ? 6 : 7));
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0008:		// rrcurveto
	if (nOps < 6 || nOps % 6 != 0) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 rrcurveto", nOps);
	}
	for (k = 0; k < nOps; k += 6) {
	  cvtNum(ops[k], charBuf);
	  cvtNum(ops[k+1], charBuf);
	  cvtNum(ops[k+2], charBuf);
	  cvtNum(ops[k+3], charBuf);
	  cvtNum(ops[k+4], charBuf);
	  cvtNum(ops[k+5], charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x000a:		// callsubr
	if (nOps >= 1) {
	  subrBias = (subrIdx->len < 1240)
	               ? 107 : (subrIdx->len < 33900) ? 1131 : 32768;
	  k = subrBias + ops[nOps - 1].toInt();
	  --nOps;
	  ok = gTrue;
	  getIndexVal(subrIdx, k, &val, &ok);
	  if (ok) {
	    cvtGlyph(val.pos, val.len, charBuf, subrIdx, pDict, gFalse);
	  }
	} else {
	  //~ error(-1, "Too few args to Type 2 callsubr");
	}
	// don't clear the stack
	break;
      case 0x000b:		// return
	// don't clear the stack
	break;
      case 0x000e:		// endchar / seac
	if (firstOp) {
	  cvtGlyphWidth(nOps == 1 || nOps == 5, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps == 4) {
	  cvtNum(zero, charBuf);
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  charBuf->append((char)12)->append((char)6);
	} else if (nOps == 0) {
	  charBuf->append((char)14);
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 endchar", nOps);
	}
	nOps = 0;
	break;
      case 0x000f:		// (obsolete)
	// this op is ignored, but we need the glyph width
	if (firstOp) {
	  cvtGlyphWidth(nOps > 0, charBuf, pDict);
	  firstOp = gFalse;
	}
	nOps = 0;
	break;
      case 0x0010:		// blend
	//~ error(-1, "Unimplemented Type 2 charstring op: %d", file[i]);
	nOps = 0;
	break;
      case 0x0012:		// hstemhm
	// ignored
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hstemhm", nOps);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0013:		// hintmask
	// ignored
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps > 0) {
	  if (nOps & 1) {
	    //~ error(-1, "Wrong number of args (%d) to Type 2 hintmask/vstemhm",
	    //~       nOps);
	  }
	  nHints += nOps / 2;
	}
	pos += (nHints + 7) >> 3;
	nOps = 0;
	break;
      case 0x0014:		// cntrmask
	// ignored
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps > 0) {
	  if (nOps & 1) {
	    //~ error(-1, "Wrong number of args (%d) to Type 2 cntrmask/vstemhm",
	    //~       nOps);
	  }
	  nHints += nOps / 2;
	}
	pos += (nHints + 7) >> 3;
	nOps = 0;
	break;
      case 0x0015:		// rmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 3, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 2) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 rmoveto", nOps);
	}
	cvtNum(ops[0], charBuf);
	cvtNum(ops[1], charBuf);
	charBuf->append((char)21);
	nOps = 0;
	break;
      case 0x0016:		// hmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 2, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hmoveto", nOps);
	}
	cvtNum(ops[0], charBuf);
	charBuf->append((char)22);
	nOps = 0;
	break;
      case 0x0017:		// vstemhm
	// ignored
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vstemhm", nOps);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0018:		// rcurveline
	if (nOps < 8 || (nOps - 2) % 6 != 0) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 rcurveline", nOps);
	}
	for (k = 0; k <= nOps - 8; k += 6) {
	  cvtNum(ops[k], charBuf);
	  cvtNum(ops[k+1], charBuf);
	  cvtNum(ops[k+2], charBuf);
	  cvtNum(ops[k+3], charBuf);
	  cvtNum(ops[k+4], charBuf);
	  cvtNum(ops[k+5], charBuf);
	  charBuf->append((char)8);
	}
	cvtNum(ops[k], charBuf);
	cvtNum(ops[k+1], charBuf);
	charBuf->append((char)5);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0019:		// rlinecurve
	if (nOps < 8 || (nOps - 6) % 2 != 0) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 rlinecurve", nOps);
	}
	for (k = 0; k <= nOps - 8; k += 2) {
	  cvtNum(ops[k], charBuf);
	  cvtNum(ops[k+1], charBuf);
	  charBuf->append((char)5);
	}
	cvtNum(ops[k], charBuf);
	cvtNum(ops[k+1], charBuf);
	cvtNum(ops[k+2], charBuf);
	cvtNum(ops[k+3], charBuf);
	cvtNum(ops[k+4], charBuf);
	cvtNum(ops[k+5], charBuf);
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001a:		// vvcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vvcurveto", nOps);
	}
	if (nOps % 2 == 1 && nOps >= 5) {
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[4], charBuf);
	  charBuf->append((char)8);
	  k = 5;
	} else {
	  k = 0;
	}
	for (; k+3 < nOps; k += 4) {
	  cvtNum(zero, charBuf);
	  cvtNum(ops[k], charBuf);
	  cvtNum(ops[k+1], charBuf);
	  cvtNum(ops[k+2], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[k+3], charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001b:		// hhcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hhcurveto", nOps);
	}
	if (nOps % 2 == 1 && nOps >= 5) {
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(ops[4], charBuf);
	  cvtNum(zero, charBuf);
	  charBuf->append((char)8);
	  k = 5;
	} else {
	  k = 0;
	}
	for (; k+3 < nOps; k += 4) {
	  cvtNum(ops[k], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[k+1], charBuf);
	  cvtNum(ops[k+2], charBuf);
	  cvtNum(ops[k+3], charBuf);
	  cvtNum(zero, charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001d:		// callgsubr
	if (nOps >= 1) {
	  k = gsubrBias + ops[nOps - 1].toInt();
	  --nOps;
	  ok = gTrue;
	  getIndexVal(&gsubrIdx, k, &val, &ok);
	  if (ok) {
	    cvtGlyph(val.pos, val.len, charBuf, subrIdx, pDict, gFalse);
	  }
	} else {
	  //~ error(-1, "Too few args to Type 2 callgsubr");
	}
	// don't clear the stack
	break;
      case 0x001e:		// vhcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 vhcurveto", nOps);
	}
	for (k = 0; k+3 < nOps && k != nOps-5; k += 4) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    charBuf->append((char)30);
	  } else {
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    charBuf->append((char)31);
	  }
	}
	if (k == nOps-5) {
	  if (k % 8 == 0) {
	    cvtNum(zero, charBuf);
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    cvtNum(ops[k+4], charBuf);
	  } else {
	    cvtNum(ops[k], charBuf);
	    cvtNum(zero, charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+4], charBuf);
	    cvtNum(ops[k+3], charBuf);
	  }
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001f:		// hvcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hvcurveto", nOps);
	}
	for (k = 0; k+3 < nOps && k != nOps-5; k += 4) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    charBuf->append((char)31);
	  } else {
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    charBuf->append((char)30);
	  }
	}
	if (k == nOps-5) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k], charBuf);
	    cvtNum(zero, charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+4], charBuf);
	    cvtNum(ops[k+3], charBuf);
	  } else {
	    cvtNum(zero, charBuf);
	    cvtNum(ops[k], charBuf);
	    cvtNum(ops[k+1], charBuf);
	    cvtNum(ops[k+2], charBuf);
	    cvtNum(ops[k+3], charBuf);
	    cvtNum(ops[k+4], charBuf);
	  }
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c00:		// dotsection (should be Type 1 only?)
	// ignored
	nOps = 0;
	break;
      case 0x0c0c:		// div
	if (nOps >= 2) {
	  num = ops[nOps-2].toInt();
	  den = ops[nOps-1].toInt();
	  --nOps;
	  ops[nOps-1].kind = type1COpRational;
	  ops[nOps-1].rat.num = num;
	  ops[nOps-1].rat.den = den;
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 div", nOps);
	}
	break;
      case 0x0c03:		// and
      case 0x0c04:		// or
      case 0x0c05:		// not
      case 0x0c08:		// store
      case 0x0c09:		// abs
      case 0x0c0a:		// add
      case 0x0c0b:		// sub
      case 0x0c0d:		// load
      case 0x0c0e:		// neg
      case 0x0c0f:		// eq
      case 0x0c12:		// drop
      case 0x0c14:		// put
      case 0x0c15:		// get
      case 0x0c16:		// ifelse
      case 0x0c17:		// random
      case 0x0c18:		// mul
      case 0x0c1a:		// sqrt
      case 0x0c1b:		// dup
      case 0x0c1c:		// exch
      case 0x0c1d:		// index
      case 0x0c1e:		// roll
	//~ error(-1, "Unimplemented Type 2 charstring op: 12.%d", file[i+1]);
	nOps = 0;
	break;
      case 0x0c22:		// hflex
	if (nOps == 7) {
	  cvtNum(ops[0], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(zero, charBuf);
	  charBuf->append((char)8);
	  cvtNum(ops[4], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[5], charBuf);
	  if (ops[2].kind == type1COpInteger) {
	    tmp.kind = type1COpInteger;
	    tmp.intgr = -ops[2].intgr;
	  } else if (ops[2].kind == type1COpRational) {
	    tmp.kind = type1COpRational;
	    tmp.rat.num = -ops[2].rat.num;
	    tmp.rat.den = ops[2].rat.den;
	  } else {
	    tmp.kind = type1COpFloat;
	    tmp.flt = -ops[2].toFloat();
	  }
	  cvtNum(tmp, charBuf);
	  cvtNum(ops[6], charBuf);
	  cvtNum(zero, charBuf);
	  charBuf->append((char)8);
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hflex", nOps);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c23:		// flex
	if (nOps == 13) {
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(ops[4], charBuf);
	  cvtNum(ops[5], charBuf);
	  charBuf->append((char)8);
	  cvtNum(ops[6], charBuf);
	  cvtNum(ops[7], charBuf);
	  cvtNum(ops[8], charBuf);
	  cvtNum(ops[9], charBuf);
	  cvtNum(ops[10], charBuf);
	  cvtNum(ops[11], charBuf);
	  charBuf->append((char)8);
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 flex", nOps);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c24:		// hflex1
	if (nOps == 9) {
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(ops[4], charBuf);
	  cvtNum(zero, charBuf);
	  charBuf->append((char)8);
	  cvtNum(ops[5], charBuf);
	  cvtNum(zero, charBuf);
	  cvtNum(ops[6], charBuf);
	  cvtNum(ops[7], charBuf);
	  cvtNum(ops[8], charBuf);
	  if (ops[1].kind == type1COpInteger &&
	      ops[3].kind == type1COpInteger &&
	      ops[7].kind == type1COpInteger) {
	    tmp.kind = type1COpInteger;
	    tmp.intgr = -(ops[1].intgr + ops[3].intgr + ops[7].intgr);
	  } else {
	    tmp.kind = type1COpFloat;
	    tmp.flt = -(ops[1].toFloat() + ops[3].toFloat() + ops[7].toFloat());
	  }
	  cvtNum(tmp, charBuf);
	  charBuf->append((char)8);
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 hflex1", nOps);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c25:		// flex1
	if (nOps == 11) {
	  cvtNum(ops[0], charBuf);
	  cvtNum(ops[1], charBuf);
	  cvtNum(ops[2], charBuf);
	  cvtNum(ops[3], charBuf);
	  cvtNum(ops[4], charBuf);
	  cvtNum(ops[5], charBuf);
	  charBuf->append((char)8);
	  cvtNum(ops[6], charBuf);
	  cvtNum(ops[7], charBuf);
	  cvtNum(ops[8], charBuf);
	  cvtNum(ops[9], charBuf);
	  dx = ops[0].toFloat() + ops[2].toFloat() + ops[4].toFloat()
	       + ops[6].toFloat() + ops[8].toFloat();
	  dy = ops[1].toFloat() + ops[3].toFloat() + ops[5].toFloat()
	       + ops[7].toFloat() + ops[9].toFloat();
	  if (fabs(dx) > fabs(dy)) {
	    cvtNum(ops[10], charBuf);
	    if (ops[1].kind == type1COpInteger &&
		ops[3].kind == type1COpInteger &&
		ops[5].kind == type1COpInteger &&
		ops[7].kind == type1COpInteger &&
		ops[9].kind == type1COpInteger) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = -(int)dy;
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = -dy;
	    }
	    cvtNum(tmp, charBuf);
	  } else {
	    if (ops[0].kind == type1COpInteger &&
		ops[2].kind == type1COpInteger &&
		ops[4].kind == type1COpInteger &&
		ops[6].kind == type1COpInteger &&
		ops[8].kind == type1COpInteger) {
	      tmp.kind = type1COpInteger;
	      tmp.intgr = -(int)dx;
	    } else {
	      tmp.kind = type1COpFloat;
	      tmp.flt = -dx;
	    }
	    cvtNum(tmp, charBuf);
	    cvtNum(ops[10], charBuf);
	  }
	  charBuf->append((char)8);
	} else {
	  //~ error(-1, "Wrong number of args (%d) to Type 2 flex1", nOps);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      default:
	//~ error(-1, "Illegal Type 2 charstring op: %04x",
	//~       ops[nOps].op);
	nOps = 0;
	break;
      }
    }
  }

  // charstring encryption
  if (top) {
    r2 = 4330;
    for (i = start; i < charBuf->getLength(); ++i) {
      byte = (Guchar)(charBuf->getChar(i) ^ (r2 >> 8));
      charBuf->setChar(i, byte);
      r2 = (Gushort)((byte + r2) * 52845 + 22719);
    }
  }
}

void FoFiType1C::cvtGlyphWidth(GBool useOp, GString *charBuf,
			       Type1CPrivateDict *pDict) {
  Type1COp zero, w;
  int i;

  if (useOp) {
    if (pDict->nominalWidthXInt & (ops[0].kind == type1COpInteger)) {
      w.kind = type1COpInteger;
      w.intgr = (int)(pDict->nominalWidthX + ops[0].intgr);
    } else {
      w.kind = type1COpFloat;
      w.flt = pDict->nominalWidthX + ops[0].toFloat();
    }
    for (i = 1; i < nOps; ++i) {
      ops[i-1] = ops[i];
    }
    --nOps;
  } else {
    if (pDict->defaultWidthXInt) {
      w.kind = type1COpInteger;
      w.intgr = (int)pDict->defaultWidthX;
    } else {
      w.kind = type1COpFloat;
      w.flt = pDict->defaultWidthX;
    }
  }
  zero.kind = type1COpInteger;
  zero.intgr = 0;
  cvtNum(zero, charBuf);
  cvtNum(w, charBuf);
  charBuf->append((char)13);
}

void FoFiType1C::cvtNum(Type1COp op, GString *charBuf) {
  Type1COp tmp;
  Guchar buf[12];
  int y, n;

  n = 0;
  switch (op.kind) {
  case type1COpInteger:
    y = op.intgr;
    if (y >= -107 && y <= 107) {
      buf[0] = (Guchar)(y + 139);
      n = 1;
    } else if (y > 107 && y <= 1131) {
      y -= 108;
      buf[0] = (Guchar)((y >> 8) + 247);
      buf[1] = (Guchar)(y & 0xff);
      n = 2;
    } else if (y < -107 && y >= -1131) {
      y = -y - 108;
      buf[0] = (Guchar)((y >> 8) + 251);
      buf[1] = (Guchar)(y & 0xff);
      n = 2;
    } else {
      buf[0] = 255;
      buf[1] = (Guchar)(y >> 24);
      buf[2] = (Guchar)(y >> 16);
      buf[3] = (Guchar)(y >> 8);
      buf[4] = (Guchar)y;
      n = 5;
    }
    break;
  case type1COpFloat:
    if (op.flt >= -32768 && op.flt < 32768) {
      y = (int)(op.flt * 256.0);
      buf[0] = 255;
      buf[1] = (Guchar)(y >> 24);
      buf[2] = (Guchar)(y >> 16);
      buf[3] = (Guchar)(y >> 8);
      buf[4] = (Guchar)y;
      buf[5] = 255;
      buf[6] = 0;
      buf[7] = 0;
      buf[8] = 1;
      buf[9] = 0;
      buf[10] = 12;
      buf[11] = 12;
      n = 12;
    } else {
      //~ error(-1, "Type 2 fixed point constant out of range");
    }
    break;
  case type1COpRational:
    tmp.kind = type1COpInteger;
    tmp.intgr = op.rat.num;
    cvtNum(tmp, charBuf);
    tmp.intgr = op.rat.den;
    cvtNum(tmp, charBuf);
    buf[0] = 0x0c;
    buf[1] = 0x0c;
    n = 2;
    break;
  default: // shouldn't happen
    break;
  }
  charBuf->append((char *)buf, n);
}

void FoFiType1C::eexecWrite(Type1CEexecBuf *eb, const char *s) {
  Guchar *p;
  Guchar x;

  for (p = (Guchar *)s; *p; ++p) {
    x = *p ^ (Guchar)(eb->r1 >> 8);
    eb->r1 = (Gushort)((x + eb->r1) * 52845 + 22719);
    if (eb->ascii) {
      (*eb->outputFunc)(eb->outputStream, &hexChars[x >> 4], 1);
      (*eb->outputFunc)(eb->outputStream, &hexChars[x & 0x0f], 1);
      eb->line += 2;
      if (eb->line == 64) {
	(*eb->outputFunc)(eb->outputStream, "\n", 1);
	eb->line = 0;
      }
    } else {
      (*eb->outputFunc)(eb->outputStream, (char *)&x, 1);
    }
  }
}

void FoFiType1C::eexecWriteCharstring(Type1CEexecBuf *eb,
				      Guchar *s, int n) {
  Guchar x;
  int i;

  // eexec encryption
  for (i = 0; i < n; ++i) {
    x = s[i] ^ (Guchar)(eb->r1 >> 8);
    eb->r1 = (Gushort)((x + eb->r1) * 52845 + 22719);
    if (eb->ascii) {
      (*eb->outputFunc)(eb->outputStream, &hexChars[x >> 4], 1);
      (*eb->outputFunc)(eb->outputStream, &hexChars[x & 0x0f], 1);
      eb->line += 2;
      if (eb->line == 64) {
	(*eb->outputFunc)(eb->outputStream, "\n", 1);
	eb->line = 0;
      }
    } else {
      (*eb->outputFunc)(eb->outputStream, (char *)&x, 1);
    }
  }
}

void FoFiType1C::writePSString(char *s, FoFiOutputFunc outputFunc,
			       void *outputStream) {
  char buf[80];
  char *p;
  int i, c;

  i = 0;
  buf[i++] = '(';
  for (p = s; *p; ++p) {
    c = *p & 0xff;
    if (c == '(' || c == ')' || c == '\\') {
      buf[i++] = '\\';
      buf[i++] = (char)c;
    } else if (c < 0x20 || c >= 0x80) {
      buf[i++] = '\\';
      buf[i++] = (char)('0' + ((c >> 6) & 7));
      buf[i++] = (char)('0' + ((c >> 3) & 7));
      buf[i++] = (char)('0' + (c & 7));
    } else {
      buf[i++] = (char)c;
    }
    if (i >= 64) {
      buf[i++] = '\\';
      buf[i++] = '\n';
      (*outputFunc)(outputStream, buf, i);
      i = 0;
    }
  }
  buf[i++] = ')';
  (*outputFunc)(outputStream, buf, i);
}

void FoFiType1C::convertToOpenType(FoFiOutputFunc outputFunc,
				   void *outputStream,
				   int nWidths, Gushort *widths,
				   Guchar *cmapTable, int cmapTableLen) {
  // dummy OS/2 table (taken from FoFiTrueType::writeTTF)
  static Guchar os2Tab[86] = {
    0, 1,			// version
    0, 1,			// xAvgCharWidth
    0x01, 0x90,			// usWeightClass
    0, 5,			// usWidthClass
    0, 0,			// fsType
    0, 0,			// ySubscriptXSize
    0, 0,			// ySubscriptYSize
    0, 0,			// ySubscriptXOffset
    0, 0,			// ySubscriptYOffset
    0, 0,			// ySuperscriptXSize
    0, 0,			// ySuperscriptYSize
    0, 0,			// ySuperscriptXOffset
    0, 0,			// ySuperscriptYOffset
    0, 0,			// yStrikeoutSize
    0, 0,			// yStrikeoutPosition
    0, 0,			// sFamilyClass
    0, 0, 0, 0, 0,		// panose
    0, 0, 0, 0, 0,
    0, 0, 0, 0,			// ulUnicodeRange1
    0, 0, 0, 0,			// ulUnicodeRange2
    0, 0, 0, 0,			// ulUnicodeRange3
    0, 0, 0, 0,			// ulUnicodeRange4
    0, 0, 0, 0,			// achVendID
    0, 0,			// fsSelection
    0, 0,			// usFirstCharIndex
    0, 0,			// usLastCharIndex
    0, 0,			// sTypoAscender
    0, 0,			// sTypoDescender
    0, 0,			// sTypoLineGap
    0x20, 0x00,			// usWinAscent
    0x20, 0x00,			// usWinDescent
    0, 0, 0, 1,			// ulCodePageRange1
    0, 0, 0, 0			// ulCodePageRange2
  };
  Guchar headTable[54], hheaTable[36], maxpTable[6];
  Guchar nameTable[26], postTable[32];
  Guchar *hmtxTable;
  static const char *tableTag[9] = {
    "CFF ",
    "OS/2",
    "cmap",
    "head",
    "hhea",
    "hmtx",
    "maxp",
    "name",
    "post"
  };
  Guchar *tableData[9];
  int tableLength[9];
  Guchar header[12 + 9*16];
  double mat[6];
  Gushort maxWidth;
  Guint checksum, fileChecksum;
  int unitsPerEm, xMin, yMin, xMax, yMax, offset, i;

  //--- CFF_ table
  tableData[0] = file;
  tableLength[0] = len;

  //--- OS/2 table
  tableData[1] = os2Tab;
  tableLength[1] = 86;

  //--- cmap table
  tableData[2] = cmapTable;
  tableLength[2] = cmapTableLen;

  //--- head table
  getFontMatrix(mat);
  if (mat[0] == 0) {
    unitsPerEm = 1000;
  } else {
    unitsPerEm = (int)(1 / mat[0] + 0.5);
  }
  xMin = (int)(topDict.fontBBox[0] + 0.5);
  yMin = (int)(topDict.fontBBox[1] + 0.5);
  xMax = (int)(topDict.fontBBox[2] + 0.5);
  yMax = (int)(topDict.fontBBox[3] + 0.5);
  headTable[ 0] = 0x00;				// version
  headTable[ 1] = 0x01;
  headTable[ 2] = 0x00;
  headTable[ 3] = 0x00;
  headTable[ 4] = 0x00;				// revision
  headTable[ 5] = 0x00;
  headTable[ 6] = 0x00;
  headTable[ 7] = 0x00;
  headTable[ 8] = 0x00;				// checksumAdjustment
  headTable[ 9] = 0x00;				//   (set later)
  headTable[10] = 0x00;
  headTable[11] = 0x00;
  headTable[12] = 0x5f;				// magicNumber
  headTable[13] = 0x0f;
  headTable[14] = 0x3c;
  headTable[15] = 0xf5;
  headTable[16] = 0x00;				// flags
  headTable[17] = 0x03;
  headTable[18] = (Guchar)(unitsPerEm >> 8);	// unitsPerEm
  headTable[19] = (Guchar)unitsPerEm;
  headTable[20] = 0x00;				// created
  headTable[21] = 0x00;
  headTable[22] = 0x00;
  headTable[23] = 0x00;
  headTable[24] = 0x00;
  headTable[25] = 0x00;
  headTable[26] = 0x00;
  headTable[27] = 0x00;
  headTable[28] = 0x00;				// modified
  headTable[29] = 0x00;
  headTable[30] = 0x00;
  headTable[31] = 0x00;
  headTable[32] = 0x00;
  headTable[33] = 0x00;
  headTable[34] = 0x00;
  headTable[35] = 0x00;
  headTable[36] = (Guchar)(xMin >> 8);		// xMin
  headTable[37] = (Guchar)xMin;
  headTable[38] = (Guchar)(yMin >> 8);		// yMin
  headTable[39] = (Guchar)yMin;
  headTable[40] = (Guchar)(xMax >> 8);		// xMax
  headTable[41] = (Guchar)xMax;
  headTable[42] = (Guchar)(yMax >> 8);		// yMax
  headTable[43] = (Guchar)yMax;
  headTable[44] = 0;				// macStyle
  headTable[45] = 0;
  headTable[46] = 0;				// lowestRecPPEM
  headTable[47] = 3;
  headTable[48] = 0;				// fontDirectionHint
  headTable[49] = 2;				//   (deprecated)
  headTable[50] = 0;				// indexToLocFormat
  headTable[51] = 0;				//   (n/a to CFF fonts)
  headTable[52] = 0;				// glyphDataFormat
  headTable[53] = 0;				//   (n/a to CFF fonts)
  tableData[3] = headTable;
  tableLength[3] = 54;

  //--- hhea table
  maxWidth = widths[0];
  for (i = 1; i < nWidths; ++i) {
    if (widths[i] > maxWidth) {
      maxWidth = widths[i];
    }
  }
  hheaTable[ 0] = 0x00;				// version
  hheaTable[ 1] = 0x01;
  hheaTable[ 2] = 0x00;
  hheaTable[ 3] = 0x00;
  hheaTable[ 4] = (Guchar)(yMax >> 8);		// ascender
  hheaTable[ 5] = (Guchar)yMax;
  hheaTable[ 6] = (Guchar)(yMin >> 8);		// descender
  hheaTable[ 7] = (Guchar)yMin;
  hheaTable[ 8] = 0;				// lineGap
  hheaTable[ 9] = 0;
  hheaTable[10] = (Guchar)(maxWidth >> 8);	// advanceWidthMax
  hheaTable[11] = (Guchar)maxWidth;
  hheaTable[12] = 0;				// minLeftSideBearing
  hheaTable[13] = 0;
  hheaTable[14] = 0;				// minRightSideBearing
  hheaTable[15] = 0;
  hheaTable[16] = (Guchar)(maxWidth >> 8);	// xMaxExtent
  hheaTable[17] = (Guchar)maxWidth;
  hheaTable[18] = 0;				// caretSlopeRise
  hheaTable[19] = 1;
  hheaTable[20] = 0;				// caretSlopeRun
  hheaTable[21] = 0;
  hheaTable[22] = 0;				// caretOffset
  hheaTable[23] = 0;
  hheaTable[24] = 0;				// reserved
  hheaTable[25] = 0;
  hheaTable[26] = 0;				// reserved
  hheaTable[27] = 0;
  hheaTable[28] = 0;				// reserved
  hheaTable[29] = 0;
  hheaTable[30] = 0;				// reserved
  hheaTable[31] = 0;
  hheaTable[32] = 0;				// metricDataFormat
  hheaTable[33] = 0;
  hheaTable[34] = (Guchar)(nWidths >> 8);	// numberOfHMetrics
  hheaTable[35] = (Guchar)nWidths;
  tableData[4] = hheaTable;
  tableLength[4] = 36;

  //--- hmtx table
  //~ this currently sets LSB to 0 for all glyphs
  hmtxTable = (Guchar *)gmallocn(nWidths, 4);
  for (i = 0; i < nWidths; ++i) {
    hmtxTable[4*i  ] = (Guchar)(widths[i] >> 8);
    hmtxTable[4*i+1] = (Guchar)widths[i];
    hmtxTable[4*i+2] = 0;
    hmtxTable[4*i+3] = 0;
  }
  tableData[5] = hmtxTable;
  tableLength[5] = 4 * nWidths;

  //--- maxp table
  maxpTable[0] = 0x00;				// version = 0.5
  maxpTable[1] = 0x00;
  maxpTable[2] = 0x50;
  maxpTable[3] = 0x00;
  maxpTable[4] = (Guchar)(nGlyphs >> 8);	// numGlyphs
  maxpTable[5] = (Guchar)nGlyphs;
  tableData[6] = maxpTable;
  tableLength[6] = 6;

  //--- name table
  nameTable[ 0] = 0x00;				// format
  nameTable[ 1] = 0x00;
  nameTable[ 2] = 0x00;				// count
  nameTable[ 3] = 0x01;
  nameTable[ 4] = 0x00;				// stringOffset
  nameTable[ 5] = 0x12;
  nameTable[ 6] = 0x00;				// platformID
  nameTable[ 7] = 0x00;
  nameTable[ 8] = 0x00;				// encodingID
  nameTable[ 9] = 0x03;
  nameTable[10] = 0x00;				// languageID
  nameTable[11] = 0x00;
  nameTable[12] = 0x00;				// nameID
  nameTable[13] = 0x00;
  nameTable[14] = 0x00;				// length
  nameTable[15] = 0x08;
  nameTable[16] = 0x00;				// offset
  nameTable[17] = 0x00;
  nameTable[18] = 0x00;				// string data
  nameTable[19] = (Guchar)'n';
  nameTable[20] = 0x00;
  nameTable[21] = (Guchar)'o';
  nameTable[22] = 0x00;
  nameTable[23] = (Guchar)'n';
  nameTable[24] = 0x00;
  nameTable[25] = (Guchar)'e';
  tableData[7] = nameTable;
  tableLength[7] = 26;

  //--- post table
  postTable[ 0] = 0x00;				// version = 3.0
  postTable[ 1] = 0x03;
  postTable[ 2] = 0x00;
  postTable[ 3] = 0x00;
  postTable[ 4] = 0x00;				// italicAngle
  postTable[ 5] = 0x00;
  postTable[ 6] = 0x00;
  postTable[ 7] = 0x00;
  postTable[ 8] = 0x00;				// underlinePosition
  postTable[ 9] = 0x00;
  postTable[10] = 0x00;				// underlineThickness
  postTable[11] = 0x00;
  postTable[12] = 0x00;				// isFixedPitch
  postTable[13] = 0x00;
  postTable[14] = 0x00;
  postTable[15] = 0x00;
  postTable[16] = 0x00;				// minMemType42
  postTable[17] = 0x00;
  postTable[18] = 0x00;
  postTable[19] = 0x00;
  postTable[20] = 0x00;				// maxMemType42
  postTable[21] = 0x00;
  postTable[22] = 0x00;
  postTable[23] = 0x00;
  postTable[24] = 0x00;				// minMemType1
  postTable[25] = 0x00;
  postTable[26] = 0x00;
  postTable[27] = 0x00;
  postTable[28] = 0x00;				// maxMemType1
  postTable[29] = 0x00;
  postTable[30] = 0x00;
  postTable[31] = 0x00;
  tableData[8] = postTable;
  tableLength[8] = 32;

  //--- header and table directory
  header[ 0] = 'O';				// sfnt version
  header[ 1] = 'T';
  header[ 2] = 'T';
  header[ 3] = 'O';
  header[ 4] = 0x00;				// numTables
  header[ 5] = 0x09;
  header[ 6] = 0x00;				// searchRange
  header[ 7] = 0x80;
  header[ 8] = 0x00;				// entrySelector
  header[ 9] = 0x03;
  header[10] = 0x00;				// rangeShift
  header[11] = 0x10;
  offset = 12 + 9*16;
  fileChecksum = 0;
  for (i = 0; i < 9; ++i) {
    header[12 + i*16 +  0] = tableTag[i][0];
    header[12 + i*16 +  1] = tableTag[i][1];
    header[12 + i*16 +  2] = tableTag[i][2];
    header[12 + i*16 +  3] = tableTag[i][3];
    checksum = computeOpenTypeTableChecksum(tableData[i], tableLength[i]);
    fileChecksum += checksum;
    header[12 + i*16 +  4] = (Guchar)(checksum >> 24);
    header[12 + i*16 +  5] = (Guchar)(checksum >> 16);
    header[12 + i*16 +  6] = (Guchar)(checksum >> 8);
    header[12 + i*16 +  7] = (Guchar)checksum;
    header[12 + i*16 +  8] = (Guchar)(offset >> 24);
    header[12 + i*16 +  9] = (Guchar)(offset >> 16);
    header[12 + i*16 + 10] = (Guchar)(offset >> 8);
    header[12 + i*16 + 11] = (Guchar)offset;
    header[12 + i*16 + 12] = (Guchar)(tableLength[i] >> 24);
    header[12 + i*16 + 13] = (Guchar)(tableLength[i] >> 16);
    header[12 + i*16 + 14] = (Guchar)(tableLength[i] >> 8);
    header[12 + i*16 + 15] = (Guchar)tableLength[i];
    offset += tableLength[i];
    if (tableLength[i] & 3) {
      offset += 4 - (tableLength[i] & 3);
    }
  }

  //--- file checksum
  fileChecksum += computeOpenTypeTableChecksum(header, 12 + 9*16);
  fileChecksum = 0xb1b0afba - fileChecksum;
  headTable[ 8] = (Guchar)(fileChecksum >> 24);
  headTable[ 9] = (Guchar)(fileChecksum >> 16);
  headTable[10] = (Guchar)(fileChecksum >>  8);
  headTable[11] = (Guchar)fileChecksum;

  //--- write the OpenType font
  (*outputFunc)(outputStream, (char *)header, 12 + 9*16);
  for (i = 0; i < 9; ++i) {
    (*outputFunc)(outputStream, (char *)tableData[i], tableLength[i]);
    if (tableLength[i] & 3) {
      (*outputFunc)(outputStream, "\0\0\0", 4 - (tableLength[i] & 3));
    }
  }

  gfree(hmtxTable);
}

Guint FoFiType1C::computeOpenTypeTableChecksum(Guchar *data, int length) {
  Guint checksum, word;
  int i;

  checksum = 0;
  for (i = 0; i+3 < length; i += 4) {
    word = ((data[i  ] & 0xff) << 24) +
           ((data[i+1] & 0xff) << 16) +
           ((data[i+2] & 0xff) <<  8) +
            (data[i+3] & 0xff);
    checksum += word;
  }
  if (length & 3) {
    word = 0;
    i = length & ~3;
    switch (length & 3) {
    case 3:
      word |= (data[i+2] & 0xff) <<  8;
    case 2:
      word |= (data[i+1] & 0xff) << 16;
    case 1:
      word |= (data[i  ] & 0xff) << 24;
      break;
    }
    checksum += word;
  }
  return checksum;
}

GBool FoFiType1C::parse() {
  Type1CIndex fdIdx;
  Type1CIndexVal val;
  int i;

  parsedOk = gTrue;

  // some tools embed Type 1C fonts with an extra whitespace char at
  // the beginning
  if (len > 0 && file[0] != '\x01') {
    ++file;
    --len;
  }

  // find the indexes
  getIndex(getU8(2, &parsedOk), &nameIdx, &parsedOk);
  getIndex(nameIdx.endPos, &topDictIdx, &parsedOk);
  getIndex(topDictIdx.endPos, &stringIdx, &parsedOk);
  getIndex(stringIdx.endPos, &gsubrIdx, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  gsubrBias = (gsubrIdx.len < 1240) ? 107
                                    : (gsubrIdx.len < 33900) ? 1131 : 32768;

  // read the first font name
  getIndexVal(&nameIdx, 0, &val, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  name = new GString((char *)&file[val.pos], val.len);

  // read the top dict for the first font
  readTopDict();

  // for CID fonts: read the FDArray dicts and private dicts
  if (topDict.firstOp == 0x0c1e) {
    if (topDict.fdArrayOffset == 0) {
      nFDs = 1;
      privateDicts = (Type1CPrivateDict *)gmalloc(sizeof(Type1CPrivateDict));
      readPrivateDict(0, 0, &privateDicts[0]);
    } else {
      getIndex(topDict.fdArrayOffset, &fdIdx, &parsedOk);
      if (!parsedOk) {
	return gFalse;
      }
      nFDs = fdIdx.len;
      privateDicts = (Type1CPrivateDict *)
	                 gmallocn(nFDs, sizeof(Type1CPrivateDict));
      for (i = 0; i < nFDs; ++i) {
	getIndexVal(&fdIdx, i, &val, &parsedOk);
	if (!parsedOk) {
	  return gFalse;
	}
	readFD(val.pos, val.len, &privateDicts[i]);
      }
    }

  // for 8-bit fonts: read the private dict
  } else {
    nFDs = 1;
    privateDicts = (Type1CPrivateDict *)gmalloc(sizeof(Type1CPrivateDict));
    readPrivateDict(topDict.privateOffset, topDict.privateSize,
		    &privateDicts[0]);
  }

  // check for parse errors in the private dict(s)
  if (!parsedOk) {
    return gFalse;
  }

  // get the charstrings index
  if (topDict.charStringsOffset <= 0) {
    parsedOk = gFalse;
    return gFalse;
  }
  getIndex(topDict.charStringsOffset, &charStringsIdx, &parsedOk);
  if (!parsedOk) {
    return gFalse;
  }
  nGlyphs = charStringsIdx.len;

  // for CID fonts: read the FDSelect table
  if (topDict.firstOp == 0x0c1e) {
    readFDSelect();
    if (!parsedOk) {
      return gFalse;
    }
  }

  // read the charset
  if (!readCharset()) {
    parsedOk = gFalse;
    return gFalse;
  }

  // for 8-bit fonts: build the encoding
  if (topDict.firstOp != 0x0c14 && topDict.firstOp != 0x0c1e) {
    buildEncoding();
    if (!parsedOk) {
      return gFalse;
    }
  }

  return parsedOk;
}

void FoFiType1C::readTopDict() {
  Type1CIndexVal topDictPtr;
  int pos;

  topDict.firstOp = -1;
  topDict.versionSID = 0;
  topDict.noticeSID = 0;
  topDict.copyrightSID = 0;
  topDict.fullNameSID = 0;
  topDict.familyNameSID = 0;
  topDict.weightSID = 0;
  topDict.isFixedPitch = 0;
  topDict.italicAngle = 0;
  topDict.underlinePosition = -100;
  topDict.underlineThickness = 50;
  topDict.paintType = 0;
  topDict.charstringType = 2;
  topDict.fontMatrix[0] = 0.001;
  topDict.fontMatrix[1] = 0;
  topDict.fontMatrix[2] = 0;
  topDict.fontMatrix[3] = 0.001;
  topDict.fontMatrix[4] = 0;
  topDict.fontMatrix[5] = 0;
  topDict.hasFontMatrix = gFalse;
  topDict.uniqueID = 0;
  topDict.fontBBox[0] = 0;
  topDict.fontBBox[1] = 0;
  topDict.fontBBox[2] = 0;
  topDict.fontBBox[3] = 0;
  topDict.strokeWidth = 0;
  topDict.charsetOffset = 0;
  topDict.encodingOffset = 0;
  topDict.charStringsOffset = 0;
  topDict.privateSize = 0;
  topDict.privateOffset = 0;
  topDict.registrySID = 0;
  topDict.orderingSID = 0;
  topDict.supplement = 0;
  topDict.fdArrayOffset = 0;
  topDict.fdSelectOffset = 0;

  getIndexVal(&topDictIdx, 0, &topDictPtr, &parsedOk);
  pos = topDictPtr.pos;
  nOps = 0;
  while (pos < topDictPtr.pos + topDictPtr.len) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      break;
    }
    if (ops[nOps - 1].kind == type1COpOperator) {
      --nOps; // drop the operator
      if (topDict.firstOp < 0) {
	topDict.firstOp = ops[nOps].op;
      }
      switch (ops[nOps].op) {
      case 0x0000: topDict.versionSID = ops[0].toInt(); break;
      case 0x0001: topDict.noticeSID = ops[0].toInt(); break;
      case 0x0c00: topDict.copyrightSID = ops[0].toInt(); break;
      case 0x0002: topDict.fullNameSID = ops[0].toInt(); break;
      case 0x0003: topDict.familyNameSID = ops[0].toInt(); break;
      case 0x0004: topDict.weightSID = ops[0].toInt(); break;
      case 0x0c01: topDict.isFixedPitch = ops[0].toInt(); break;
      case 0x0c02: topDict.italicAngle = ops[0].toFloat(); break;
      case 0x0c03: topDict.underlinePosition = ops[0].toFloat(); break;
      case 0x0c04: topDict.underlineThickness = ops[0].toFloat(); break;
      case 0x0c05: topDict.paintType = ops[0].toInt(); break;
      case 0x0c06: topDict.charstringType = ops[0].toInt(); break;
      case 0x0c07: topDict.fontMatrix[0] = ops[0].toFloat();
	           topDict.fontMatrix[1] = ops[1].toFloat();
	           topDict.fontMatrix[2] = ops[2].toFloat();
	           topDict.fontMatrix[3] = ops[3].toFloat();
	           topDict.fontMatrix[4] = ops[4].toFloat();
	           topDict.fontMatrix[5] = ops[5].toFloat();
		   topDict.hasFontMatrix = gTrue;
		   break;
      case 0x000d: topDict.uniqueID = ops[0].toInt(); break;
      case 0x0005: topDict.fontBBox[0] = ops[0].toFloat();
	           topDict.fontBBox[1] = ops[1].toFloat();
	           topDict.fontBBox[2] = ops[2].toFloat();
	           topDict.fontBBox[3] = ops[3].toFloat();
		   break;
      case 0x0c08: topDict.strokeWidth = ops[0].toFloat(); break;
      case 0x000f: topDict.charsetOffset = ops[0].toInt(); break;
      case 0x0010: topDict.encodingOffset = ops[0].toInt(); break;
      case 0x0011: topDict.charStringsOffset = ops[0].toInt(); break;
      case 0x0012: topDict.privateSize = ops[0].toInt();
	           topDict.privateOffset = ops[1].toInt();
		   break;
      case 0x0c1e: topDict.registrySID = ops[0].toInt();
	           topDict.orderingSID = ops[1].toInt();
		   topDict.supplement = ops[2].toInt();
		   break;
      case 0x0c24: topDict.fdArrayOffset = ops[0].toInt(); break;
      case 0x0c25: topDict.fdSelectOffset = ops[0].toInt(); break;
      }
      nOps = 0;
    }
  }
}

// Read a CID font dict (FD) - this pulls out the private dict
// pointer, and reads the private dict.  It also pulls the FontMatrix
// (if any) out of the FD.
void FoFiType1C::readFD(int offset, int length, Type1CPrivateDict *pDict) {
  int pos, pSize, pOffset;
  double fontMatrix[6];
  GBool hasFontMatrix;

  hasFontMatrix = gFalse;
  fontMatrix[0] = fontMatrix[1] = fontMatrix[2] = 0; // make gcc happy
  fontMatrix[3] = fontMatrix[4] = fontMatrix[5] = 0;
  pSize = pOffset = 0;
  pos = offset;
  nOps = 0;
  while (pos < offset + length) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if (ops[nOps - 1].kind == type1COpOperator) {
      if (ops[nOps - 1].op == 0x0012) {
	if (nOps < 3) {
	  parsedOk = gFalse;
	  return;
	}
	pSize = ops[0].toInt();
	pOffset = ops[1].toInt();
	break;
      } else if (ops[nOps - 1].op == 0x0c07) {
	fontMatrix[0] = ops[0].toFloat();
	fontMatrix[1] = ops[1].toFloat();
	fontMatrix[2] = ops[2].toFloat();
	fontMatrix[3] = ops[3].toFloat();
	fontMatrix[4] = ops[4].toFloat();
	fontMatrix[5] = ops[5].toFloat();
	hasFontMatrix = gTrue;
      }
      nOps = 0;
    }
  }
  readPrivateDict(pOffset, pSize, pDict);
  if (hasFontMatrix) {
    pDict->fontMatrix[0] = fontMatrix[0];
    pDict->fontMatrix[1] = fontMatrix[1];
    pDict->fontMatrix[2] = fontMatrix[2];
    pDict->fontMatrix[3] = fontMatrix[3];
    pDict->fontMatrix[4] = fontMatrix[4];
    pDict->fontMatrix[5] = fontMatrix[5];
    pDict->hasFontMatrix = gTrue;
  }
}

void FoFiType1C::readPrivateDict(int offset, int length,
				 Type1CPrivateDict *pDict) {
  int pos;

  pDict->hasFontMatrix = gFalse;
  pDict->nBlueValues = 0;
  pDict->nOtherBlues = 0;
  pDict->nFamilyBlues = 0;
  pDict->nFamilyOtherBlues = 0;
  pDict->blueScale = 0.039625;
  pDict->blueShift = 7;
  pDict->blueFuzz = 1;
  pDict->hasStdHW = gFalse;
  pDict->hasStdVW = gFalse;
  pDict->nStemSnapH = 0;
  pDict->nStemSnapV = 0;
  pDict->hasForceBold = gFalse;
  pDict->forceBoldThreshold = 0;
  pDict->languageGroup = 0;
  pDict->expansionFactor = 0.06;
  pDict->initialRandomSeed = 0;
  pDict->subrsOffset = 0;
  pDict->defaultWidthX = 0;
  pDict->defaultWidthXInt = gTrue;
  pDict->nominalWidthX = 0;
  pDict->nominalWidthXInt = gTrue;

  // no dictionary
  if (offset == 0 || length == 0) {
    return;
  }

  pos = offset;
  nOps = 0;
  while (pos < offset + length) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      break;
    }
    if (ops[nOps - 1].kind == type1COpOperator) {
      --nOps; // drop the operator
      switch (ops[nOps].op) {
      case 0x0006:
	pDict->nBlueValues = getDeltaIntArray(pDict->blueValues,
					      type1CMaxBlueValues);
	break;
      case 0x0007:
	pDict->nOtherBlues = getDeltaIntArray(pDict->otherBlues,
					      type1CMaxOtherBlues);
	break;
      case 0x0008:
	pDict->nFamilyBlues = getDeltaIntArray(pDict->familyBlues,
					       type1CMaxBlueValues);
	break;
      case 0x0009:
	pDict->nFamilyOtherBlues = getDeltaIntArray(pDict->familyOtherBlues,
						    type1CMaxOtherBlues);
	break;
      case 0x0c09:
	pDict->blueScale = ops[0].toFloat();
	break;
      case 0x0c0a:
	pDict->blueShift = ops[0].toInt();
	break;
      case 0x0c0b:
	pDict->blueFuzz = ops[0].toInt();
	break;
      case 0x000a:
	pDict->stdHW = ops[0].toFloat();
	pDict->hasStdHW = gTrue;
	break;
      case 0x000b:
	pDict->stdVW = ops[0].toFloat();
	pDict->hasStdVW = gTrue;
	break;
      case 0x0c0c:
	pDict->nStemSnapH = getDeltaFPArray(pDict->stemSnapH,
					    type1CMaxStemSnap);
	break;
      case 0x0c0d:
	pDict->nStemSnapV = getDeltaFPArray(pDict->stemSnapV,
					    type1CMaxStemSnap);
	break;
      case 0x0c0e:
	pDict->forceBold = !ops[0].isZero();
	pDict->hasForceBold = gTrue;
	break;
      case 0x0c0f:
	pDict->forceBoldThreshold = ops[0].toFloat();
	break;
      case 0x0c11:
	pDict->languageGroup = ops[0].toInt();
	break;
      case 0x0c12:
	pDict->expansionFactor = ops[0].toFloat();
	break;
      case 0x0c13:
	pDict->initialRandomSeed = ops[0].toInt();
	break;
      case 0x0013:
	pDict->subrsOffset = offset + ops[0].toInt();
	break;
      case 0x0014:
	pDict->defaultWidthX = ops[0].toFloat();
	pDict->defaultWidthXInt = ops[0].kind == type1COpInteger;
	break;
      case 0x0015:
	pDict->nominalWidthX = ops[0].toFloat();
	pDict->nominalWidthXInt = ops[0].kind == type1COpInteger;
	break;
      }
      nOps = 0;
    }
  }
}

void FoFiType1C::readFDSelect() {
  int fdSelectFmt, pos, nRanges, gid0, gid1, fd, i, j;

  fdSelect = (Guchar *)gmalloc(nGlyphs);
  if (topDict.fdSelectOffset == 0) {
    for (i = 0; i < nGlyphs; ++i) {
      fdSelect[i] = 0;
    }
  } else {
    pos = topDict.fdSelectOffset;
    fdSelectFmt = getU8(pos++, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if (fdSelectFmt == 0) {
      if (!checkRegion(pos, nGlyphs)) {
	parsedOk = gFalse;
	return;
      }
      for (gid0 = 0; gid0 < nGlyphs; ++gid0) {
	if (file[pos + gid0] >= nFDs) {
	  //~ error(-1, "Bad FDSelect table in CID font");
	  parsedOk = gFalse;
	  return;
	}
	fdSelect[gid0] = file[pos + gid0];
      }
    } else if (fdSelectFmt == 3) {
      nRanges = getU16BE(pos, &parsedOk);
      pos += 2;
      gid0 = getU16BE(pos, &parsedOk);
      pos += 2;
      for (i = 1; i <= nRanges; ++i) {
	fd = getU8(pos++, &parsedOk);
	gid1 = getU16BE(pos, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	pos += 2;
	if (gid0 > gid1 || gid1 > nGlyphs || fd >= nFDs) {
	  //~ error(-1, "Bad FDSelect table in CID font");
	  parsedOk = gFalse;
	  return;
	}
	for (j = gid0; j < gid1; ++j) {
	  fdSelect[j] = (Guchar)fd;
	}
	gid0 = gid1;
      }
    } else {
      //~ error(-1, "Unknown FDSelect table format in CID font");
      for (i = 0; i < nGlyphs; ++i) {
	fdSelect[i] = 0;
      }
    }
  }
}

void FoFiType1C::buildEncoding() {
  char buf[256];
  int nCodes, nRanges, encFormat;
  int pos, c, sid, nLeft, nSups, i, j;

  if (topDict.encodingOffset == 0) {
    encoding = (char **)fofiType1StandardEncoding;

  } else if (topDict.encodingOffset == 1) {
    encoding = (char **)fofiType1ExpertEncoding;

  } else {
    encoding = (char **)gmallocn(256, sizeof(char *));
    for (i = 0; i < 256; ++i) {
      encoding[i] = NULL;
    }
    pos = topDict.encodingOffset;
    encFormat = getU8(pos++, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if ((encFormat & 0x7f) == 0) {
      nCodes = 1 + getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      if (nCodes > nGlyphs) {
	nCodes = nGlyphs;
      }
      for (i = 1; i < nCodes; ++i) {
	c = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	if (encoding[c]) {
	  gfree(encoding[c]);
	}
	encoding[c] = copyString(getString(charset[i], buf, &parsedOk));
      }
    } else if ((encFormat & 0x7f) == 1) {
      nRanges = getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      nCodes = 1;
      for (i = 0; i < nRanges; ++i) {
	c = getU8(pos++, &parsedOk);
	nLeft = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  return;
	}
	for (j = 0; j <= nLeft && nCodes < nGlyphs; ++j) {
	  if (c < 256) {
	    if (encoding[c]) {
	      gfree(encoding[c]);
	    }
	    encoding[c] = copyString(getString(charset[nCodes], buf,
					       &parsedOk));
	  }
	  ++nCodes;
	  ++c;
	}
      }
    }
    if (encFormat & 0x80) {
      nSups = getU8(pos++, &parsedOk);
      if (!parsedOk) {
	return;
      }
      for (i = 0; i < nSups; ++i) {
	c = getU8(pos++, &parsedOk);;
	if (!parsedOk) {
	  return;;
	}
	sid = getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  return;
	}
	if (encoding[c]) {
	  gfree(encoding[c]);
	}
	encoding[c] = copyString(getString(sid, buf, &parsedOk));
      }
    }
  }
}

GBool FoFiType1C::readCharset() {
  int charsetFormat, c, pos;
  int nLeft, i, j;

  if (topDict.charsetOffset == 0) {
    charset = fofiType1CISOAdobeCharset;
    if (nGlyphs > 229) {
      nGlyphs = 229;
    }
  } else if (topDict.charsetOffset == 1) {
    charset = fofiType1CExpertCharset;
    if (nGlyphs > 166) {
      nGlyphs = 166;
    }
  } else if (topDict.charsetOffset == 2) {
    charset = fofiType1CExpertSubsetCharset;
    if (nGlyphs > 87) {
      nGlyphs = 87;
    }
  } else {
    charset = (Gushort *)gmallocn(nGlyphs, sizeof(Gushort));
    for (i = 0; i < nGlyphs; ++i) {
      charset[i] = 0;
    }
    pos = topDict.charsetOffset;
    charsetFormat = getU8(pos++, &parsedOk);
    if (charsetFormat == 0) {
      for (i = 1; i < nGlyphs; ++i) {
	charset[i] = (Gushort)getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  break;
	}
      }
    } else if (charsetFormat == 1) {
      i = 1;
      while (i < nGlyphs) {
	c = getU16BE(pos, &parsedOk);
	pos += 2;
	nLeft = getU8(pos++, &parsedOk);
	if (!parsedOk) {
	  break;
	}
	for (j = 0; j <= nLeft && i < nGlyphs; ++j) {
	  charset[i++] = (Gushort)c++;
	}
      }
    } else if (charsetFormat == 2) {
      i = 1;
      while (i < nGlyphs) {
	c = getU16BE(pos, &parsedOk);
	pos += 2;
	nLeft = getU16BE(pos, &parsedOk);
	pos += 2;
	if (!parsedOk) {
	  break;
	}
	for (j = 0; j <= nLeft && i < nGlyphs; ++j) {
	  charset[i++] = (Gushort)c++;
	}
      }
    }
    if (!parsedOk) {
      gfree(charset);
      charset = NULL;
      return gFalse;
    }
  }
  return gTrue;
}

int FoFiType1C::getOp(int pos, GBool charstring, GBool *ok) {
  static char nybChars[16] = "0123456789.ee -";
  Type1COp op;
  char buf[65];
  int b0, b1, nyb0, nyb1, x, i;

  b0 = getU8(pos++, ok);

  if (b0 == 28) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x8000) {
      x |= ~0xffff;
    }
    op.kind = type1COpInteger;
    op.intgr = x;

  } else if (!charstring && b0 == 29) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x80000000) {
      x |= ~0xffffffff;
    }
    op.kind = type1COpInteger;
    op.intgr = x;

  } else if (!charstring && b0 == 30) {
    i = 0;
    do {
      b1 = getU8(pos++, ok);
      nyb0 = b1 >> 4;
      nyb1 = b1 & 0x0f;
      if (nyb0 == 0xf) {
	break;
      }
      buf[i++] = nybChars[nyb0];
      if (i == 64) {
	break;
      }
      if (nyb0 == 0xc) {
	buf[i++] = '-';
      }
      if (i == 64) {
	break;
      }
      if (nyb1 == 0xf) {
	break;
      }
      buf[i++] = nybChars[nyb1];
      if (i == 64) {
	break;
      }
      if (nyb1 == 0xc) {
	buf[i++] = '-';
      }
    } while (i < 64);
    buf[i] = '\0';
    op.kind = type1COpFloat;
    op.flt = atof(buf);

  } else if (b0 >= 32 && b0 <= 246) {
    op.kind = type1COpInteger;
    op.intgr = b0 - 139;

  } else if (b0 >= 247 && b0 <= 250) {
    op.kind = type1COpInteger;
    op.intgr = ((b0 - 247) << 8) + getU8(pos++, ok) + 108;

  } else if (b0 >= 251 && b0 <= 254) {
    op.kind = type1COpInteger;
    op.intgr = -((b0 - 251) << 8) - getU8(pos++, ok) - 108;

  } else if (charstring && b0 == 255) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x80000000) {
      x |= ~0xffffffff;
    }
    op.kind = type1COpFloat;
    op.flt = (double)x / 65536.0;

  } else if (b0 == 12) {
    op.kind = type1COpOperator;
    op.op = 0x0c00 + getU8(pos++, ok);

  } else {
    op.kind = type1COpOperator;
    op.op = b0;
  }

  if (nOps < 49) {
    ops[nOps++] = op;
  }

  return pos;
}

// Convert the delta-encoded ops array to an array of ints.
int FoFiType1C::getDeltaIntArray(int *arr, int maxLen) {
  int x;
  int n, i;

  if ((n = nOps) > maxLen) {
    n = maxLen;
  }
  x = 0;
  for (i = 0; i < n; ++i) {
    x += ops[i].toInt();
    arr[i] = x;
  }
  return n;
}

// Convert the delta-encoded ops array to an array of doubles.
int FoFiType1C::getDeltaFPArray(double *arr, int maxLen) {
  double x;
  int n, i;

  if ((n = nOps) > maxLen) {
    n = maxLen;
  }
  x = 0;
  for (i = 0; i < n; ++i) {
    x += ops[i].toFloat();
    arr[i] = x;
  }
  return n;
}

void FoFiType1C::getIndex(int pos, Type1CIndex *idx, GBool *ok) {
  idx->pos = pos;
  idx->len = getU16BE(pos, ok);
  if (idx->len == 0) {
    // empty indexes are legal and contain just the length field
    idx->offSize = 0;
    idx->startPos = idx->endPos = pos + 2;
  } else {
    idx->offSize = getU8(pos + 2, ok);
    if (idx->offSize < 1 || idx->offSize > 4) {
      *ok = gFalse;
    }
    idx->startPos = pos + 3 + (idx->len + 1) * idx->offSize - 1;
    if (idx->startPos < 0 || idx->startPos >= len) {
      *ok = gFalse;
    }
    idx->endPos = idx->startPos + getUVarBE(pos + 3 + idx->len * idx->offSize,
					    idx->offSize, ok);
    if (idx->endPos < idx->startPos || idx->endPos > len) {
      *ok = gFalse;
    }
  }
}

void FoFiType1C::getIndexVal(Type1CIndex *idx, int i,
			     Type1CIndexVal *val, GBool *ok) {
  int pos0, pos1;

  if (i < 0 || i >= idx->len) {
    val->pos = 0;
    val->len = 0;
    *ok = gFalse;
    return;
  }
  pos0 = idx->startPos + getUVarBE(idx->pos + 3 + i * idx->offSize,
				   idx->offSize, ok);
  pos1 = idx->startPos + getUVarBE(idx->pos + 3 + (i + 1) * idx->offSize,
				   idx->offSize, ok);
  if (pos0 < idx->startPos || pos0 > idx->endPos ||
      pos1 <= idx->startPos || pos1 > idx->endPos ||
      pos1 < pos0) {
    *ok = gFalse;
  }
  val->pos = pos0;
  val->len = pos1 - pos0;
}

char *FoFiType1C::getString(int sid, char *buf, GBool *ok) {
  Type1CIndexVal val;
  int n;

  if (sid < 0) {
    buf[0] = '\0';
  } else if (sid < 391) {
    strcpy(buf, fofiType1CStdStrings[sid]);
  } else {
    sid -= 391;
    getIndexVal(&stringIdx, sid, &val, ok);
    if (*ok) {
      if ((n = val.len) > 255) {
	n = 255;
      }
      strncpy(buf, (char *)&file[val.pos], n);
      buf[n] = '\0';
    } else {
      buf[0] = '\0';
    }
  }
  return buf;
}
