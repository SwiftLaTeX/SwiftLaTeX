//========================================================================
//
// FoFiIdentifier.cc
//
// Copyright 2009 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "gtypes.h"
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "FoFiIdentifier.h"

//------------------------------------------------------------------------

class Reader {
public:

  virtual ~Reader() {}

  // Read one byte.  Returns -1 if past EOF.
  virtual int getByte(int pos) = 0;

  // Read a big-endian unsigned 16-bit integer.  Fills in *val and
  // returns true if successful.
  virtual GBool getU16BE(int pos, int *val) = 0;

  // Read a big-endian unsigned 32-bit integer.  Fills in *val and
  // returns true if successful.
  virtual GBool getU32BE(int pos, Guint *val) = 0;

  // Read a little-endian unsigned 32-bit integer.  Fills in *val and
  // returns true if successful.
  virtual GBool getU32LE(int pos, Guint *val) = 0;

  // Read a big-endian unsigned <size>-byte integer, where 1 <= size
  // <= 4.  Fills in *val and returns true if successful.
  virtual GBool getUVarBE(int pos, int size, Guint *val) = 0;

  // Compare against a string.  Returns true if equal.
  virtual GBool cmp(int pos, const char *s) = 0;

};

//------------------------------------------------------------------------

class MemReader: public Reader {
public:

  static MemReader *make(char *bufA, int lenA);
  virtual ~MemReader();
  virtual int getByte(int pos);
  virtual GBool getU16BE(int pos, int *val);
  virtual GBool getU32BE(int pos, Guint *val);
  virtual GBool getU32LE(int pos, Guint *val);
  virtual GBool getUVarBE(int pos, int size, Guint *val);
  virtual GBool cmp(int pos, const char *s);

private:

  MemReader(char *bufA, int lenA);

  char *buf;
  int len;
};

MemReader *MemReader::make(char *bufA, int lenA) {
  return new MemReader(bufA, lenA);
}

MemReader::MemReader(char *bufA, int lenA) {
  buf = bufA;
  len = lenA;
}

MemReader::~MemReader() {
}

int MemReader::getByte(int pos) {
  if (pos < 0 || pos >= len) {
    return -1;
  }
  return buf[pos] & 0xff;
}

GBool MemReader::getU16BE(int pos, int *val) {
  if (pos < 0 || pos > len - 2) {
    return gFalse;
  }
  *val = ((buf[pos] & 0xff) << 8) +
         (buf[pos+1] & 0xff);
  return gTrue;
}

GBool MemReader::getU32BE(int pos, Guint *val) {
  if (pos < 0 || pos > len - 4) {
    return gFalse;
  }
  *val = ((buf[pos] & 0xff) << 24) +
         ((buf[pos+1] & 0xff) << 16) +
         ((buf[pos+2] & 0xff) << 8) +
         (buf[pos+3] & 0xff);
  return gTrue;
}

GBool MemReader::getU32LE(int pos, Guint *val) {
  if (pos < 0 || pos > len - 4) {
    return gFalse;
  }
  *val = (buf[pos] & 0xff) +
         ((buf[pos+1] & 0xff) << 8) +
         ((buf[pos+2] & 0xff) << 16) +
         ((buf[pos+3] & 0xff) << 24);
  return gTrue;
}

GBool MemReader::getUVarBE(int pos, int size, Guint *val) {
  int i;

  if (size < 1 || size > 4 || pos < 0 || pos > len - size) {
    return gFalse;
  }
  *val = 0;
  for (i = 0; i < size; ++i) {
    *val = (*val << 8) + (buf[pos + i] & 0xff);
  }
  return gTrue;
}

GBool MemReader::cmp(int pos, const char *s) {
  int n;

  n = (int)strlen(s);
  if (pos < 0 || len < n || pos > len - n) {
    return gFalse;
  }
  return !memcmp(buf + pos, s, n);
}

//------------------------------------------------------------------------

class FileReader: public Reader {
public:

  static FileReader *make(char *fileName);
  virtual ~FileReader();
  virtual int getByte(int pos);
  virtual GBool getU16BE(int pos, int *val);
  virtual GBool getU32BE(int pos, Guint *val);
  virtual GBool getU32LE(int pos, Guint *val);
  virtual GBool getUVarBE(int pos, int size, Guint *val);
  virtual GBool cmp(int pos, const char *s);

private:

  FileReader(FILE *fA);
  GBool fillBuf(int pos, int len);

  FILE *f;
  char buf[1024];
  int bufPos, bufLen;
};

FileReader *FileReader::make(char *fileName) {
  FILE *fA;

  if (!(fA = fopen(fileName, "rb"))) {
    return NULL;
  }
  return new FileReader(fA);
}

FileReader::FileReader(FILE *fA) {
  f = fA;
  bufPos = 0;
  bufLen = 0;
}

FileReader::~FileReader() {
  fclose(f);
}

int FileReader::getByte(int pos) {
  if (!fillBuf(pos, 1)) {
    return -1;
  }
  return buf[pos - bufPos] & 0xff;
}

GBool FileReader::getU16BE(int pos, int *val) {
  if (!fillBuf(pos, 2)) {
    return gFalse;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 8) +
         (buf[pos - bufPos + 1] & 0xff);
  return gTrue;
}

GBool FileReader::getU32BE(int pos, Guint *val) {
  if (!fillBuf(pos, 4)) {
    return gFalse;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 24) +
         ((buf[pos - bufPos + 1] & 0xff) << 16) +
         ((buf[pos - bufPos + 2] & 0xff) << 8) +
         (buf[pos - bufPos + 3] & 0xff);
  return gTrue;
}

GBool FileReader::getU32LE(int pos, Guint *val) {
  if (!fillBuf(pos, 4)) {
    return gFalse;
  }
  *val = (buf[pos - bufPos] & 0xff) +
         ((buf[pos - bufPos + 1] & 0xff) << 8) +
         ((buf[pos - bufPos + 2] & 0xff) << 16) +
         ((buf[pos - bufPos + 3] & 0xff) << 24);
  return gTrue;
}

GBool FileReader::getUVarBE(int pos, int size, Guint *val) {
  int i;

  if (size < 1 || size > 4 || !fillBuf(pos, size)) {
    return gFalse;
  }
  *val = 0;
  for (i = 0; i < size; ++i) {
    *val = (*val << 8) + (buf[pos - bufPos + i] & 0xff);
  }
  return gTrue;
}

GBool FileReader::cmp(int pos, const char *s) {
  int n;

  n = (int)strlen(s);
  if (!fillBuf(pos, n)) {
    return gFalse;
  }
  return !memcmp(buf - bufPos + pos, s, n);
}

GBool FileReader::fillBuf(int pos, int len) {
  if (pos < 0 || len < 0 || len > (int)sizeof(buf) ||
      pos > INT_MAX - (int)sizeof(buf)) {
    return gFalse;
  }
  if (pos >= bufPos && pos + len <= bufPos + bufLen) {
    return gTrue;
  }
  if (fseek(f, pos, SEEK_SET)) {
    return gFalse;
  }
  bufPos = pos;
  bufLen = (int)fread(buf, 1, sizeof(buf), f);
  if (bufLen < len) {
    return gFalse;
  }
  return gTrue;
}

//------------------------------------------------------------------------

class StreamReader: public Reader {
public:

  static StreamReader *make(int (*getCharA)(void *data), void *dataA);
  virtual ~StreamReader();
  virtual int getByte(int pos);
  virtual GBool getU16BE(int pos, int *val);
  virtual GBool getU32BE(int pos, Guint *val);
  virtual GBool getU32LE(int pos, Guint *val);
  virtual GBool getUVarBE(int pos, int size, Guint *val);
  virtual GBool cmp(int pos, const char *s);

private:

  StreamReader(int (*getCharA)(void *data), void *dataA);
  GBool fillBuf(int pos, int len);

  int (*getChar)(void *data);
  void *data;
  int streamPos;
  char buf[1024];
  int bufPos, bufLen;
};

StreamReader *StreamReader::make(int (*getCharA)(void *data), void *dataA) {
  return new StreamReader(getCharA, dataA);
}

StreamReader::StreamReader(int (*getCharA)(void *data), void *dataA) {
  getChar = getCharA;
  data = dataA;
  streamPos = 0;
  bufPos = 0;
  bufLen = 0;
}

StreamReader::~StreamReader() {
}

int StreamReader::getByte(int pos) {
  if (!fillBuf(pos, 1)) {
    return -1;
  }
  return buf[pos - bufPos] & 0xff;
}

GBool StreamReader::getU16BE(int pos, int *val) {
  if (!fillBuf(pos, 2)) {
    return gFalse;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 8) +
         (buf[pos - bufPos + 1] & 0xff);
  return gTrue;
}

GBool StreamReader::getU32BE(int pos, Guint *val) {
  if (!fillBuf(pos, 4)) {
    return gFalse;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 24) +
         ((buf[pos - bufPos + 1] & 0xff) << 16) +
         ((buf[pos - bufPos + 2] & 0xff) << 8) +
         (buf[pos - bufPos + 3] & 0xff);
  return gTrue;
}

GBool StreamReader::getU32LE(int pos, Guint *val) {
  if (!fillBuf(pos, 4)) {
    return gFalse;
  }
  *val = (buf[pos - bufPos] & 0xff) +
         ((buf[pos - bufPos + 1] & 0xff) << 8) +
         ((buf[pos - bufPos + 2] & 0xff) << 16) +
         ((buf[pos - bufPos + 3] & 0xff) << 24);
  return gTrue;
}

GBool StreamReader::getUVarBE(int pos, int size, Guint *val) {
  int i;

  if (size < 1 || size > 4 || !fillBuf(pos, size)) {
    return gFalse;
  }
  *val = 0;
  for (i = 0; i < size; ++i) {
    *val = (*val << 8) + (buf[pos - bufPos + i] & 0xff);
  }
  return gTrue;
}

GBool StreamReader::cmp(int pos, const char *s) {
  int n;

  n = (int)strlen(s);
  if (!fillBuf(pos, n)) {
    return gFalse;
  }
  return !memcmp(buf - bufPos + pos, s, n);
}

GBool StreamReader::fillBuf(int pos, int len) {
  int c;

  if (pos < 0 || len < 0 || len > (int)sizeof(buf) ||
      pos > INT_MAX - (int)sizeof(buf)) {
    return gFalse;
  }
  if (pos < bufPos) {
    return gFalse;
  }

  // if requested region will not fit in the current buffer...
  if (pos + len > bufPos + (int)sizeof(buf)) {

    // if the start of the requested data is already in the buffer, move
    // it to the start of the buffer
    if (pos < bufPos + bufLen) {
      bufLen -= pos - bufPos;
      memmove(buf, buf + (pos - bufPos), bufLen);
      bufPos = pos;

    // otherwise discard data from the
    // stream until we get to the requested position
    } else {
      bufPos += bufLen;
      bufLen = 0;
      while (bufPos < pos) {
	if ((c = (*getChar)(data)) < 0) {
	  return gFalse;
	}
	++bufPos;
      }
    }
  }

  // read the rest of the requested data
  while (bufPos + bufLen < pos + len) {
    if ((c = (*getChar)(data)) < 0) {
      return gFalse;
    }
    buf[bufLen++] = (char)c;
  }

  return gTrue;
}

//------------------------------------------------------------------------

static FoFiIdentifierType identify(Reader *reader);
static FoFiIdentifierType identifyOpenType(Reader *reader);
static FoFiIdentifierType identifyCFF(Reader *reader, int start);

FoFiIdentifierType FoFiIdentifier::identifyMem(char *file, int len) {
  MemReader *reader;
  FoFiIdentifierType type;

  if (!(reader = MemReader::make(file, len))) {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;
  return type;
}

FoFiIdentifierType FoFiIdentifier::identifyFile(char *fileName) {
  FileReader *reader;
  FoFiIdentifierType type;
  int n;

  if (!(reader = FileReader::make(fileName))) {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;

  // Mac OS X dfont files don't have any sort of header or magic number,
  // so look at the file name extension
  if (type == fofiIdUnknown) {
    n = (int)strlen(fileName);
    if (n >= 6 && !strcmp(fileName + n - 6, ".dfont")) {
      type = fofiIdDfont;
    }
  }

  return type;
}

FoFiIdentifierType FoFiIdentifier::identifyStream(int (*getChar)(void *data),
						  void *data) {
  StreamReader *reader;
  FoFiIdentifierType type;

  if (!(reader = StreamReader::make(getChar, data))) {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;
  return type;
}

static FoFiIdentifierType identify(Reader *reader) {
  Guint n;

  //----- PFA
  if (reader->cmp(0, "%!PS-AdobeFont-1") ||
      reader->cmp(0, "%!FontType1")) {
    return fofiIdType1PFA;
  }

  //----- PFB
  if (reader->getByte(0) == 0x80 &&
      reader->getByte(1) == 0x01 &&
      reader->getU32LE(2, &n)) {
    if ((n >= 16 && reader->cmp(6, "%!PS-AdobeFont-1")) ||
	(n >= 11 && reader->cmp(6, "%!FontType1"))) {
      return fofiIdType1PFB;
    }
  }

  //----- TrueType
  if ((reader->getByte(0) == 0x00 &&
       reader->getByte(1) == 0x01 &&
       reader->getByte(2) == 0x00 &&
       reader->getByte(3) == 0x00) ||
      (reader->getByte(0) == 0x74 &&	// 'true'
       reader->getByte(1) == 0x72 &&
       reader->getByte(2) == 0x75 &&
       reader->getByte(3) == 0x65)) {
    return fofiIdTrueType;
  }
  if (reader->getByte(0) == 0x74 &&	// 'ttcf'
      reader->getByte(1) == 0x74 &&
      reader->getByte(2) == 0x63 &&
      reader->getByte(3) == 0x66) {
    return fofiIdTrueTypeCollection;
  }

  //----- OpenType
  if (reader->getByte(0) == 0x4f &&	// 'OTTO
      reader->getByte(1) == 0x54 &&
      reader->getByte(2) == 0x54 &&
      reader->getByte(3) == 0x4f) {
    return identifyOpenType(reader);
  }

  //----- CFF
  if (reader->getByte(0) == 0x01 &&
      reader->getByte(1) == 0x00) {
    return identifyCFF(reader, 0);
  }
  // some tools embed CFF fonts with an extra whitespace char at the
  // beginning
  if (reader->getByte(1) == 0x01 &&
      reader->getByte(2) == 0x00) {
    return identifyCFF(reader, 1);
  }

  return fofiIdUnknown;
}

static FoFiIdentifierType identifyOpenType(Reader *reader) {
  FoFiIdentifierType type;
  Guint offset;
  int nTables, i;

  if (!reader->getU16BE(4, &nTables)) {
    return fofiIdUnknown;
  }
  for (i = 0; i < nTables; ++i) {
    if (reader->cmp(12 + i*16, "CFF ")) {
      if (reader->getU32BE(12 + i*16 + 8, &offset) &&
	  offset < (Guint)INT_MAX) {
	type = identifyCFF(reader, (int)offset);
	if (type == fofiIdCFF8Bit) {
	  type = fofiIdOpenTypeCFF8Bit;
	} else if (type == fofiIdCFFCID) {
	  type = fofiIdOpenTypeCFFCID;
	}
	return type;
      }
      return fofiIdUnknown;
    }
  }
  return fofiIdUnknown;
}

static FoFiIdentifierType identifyCFF(Reader *reader, int start) {
  Guint offset0, offset1;
  int hdrSize, offSize0, offSize1, pos, endPos, b0, n, i;

  //----- read the header
  if (reader->getByte(start) != 0x01 ||
      reader->getByte(start + 1) != 0x00) {
    return fofiIdUnknown;
  }
  if ((hdrSize = reader->getByte(start + 2)) < 0) {
    return fofiIdUnknown;
  }
  if ((offSize0 = reader->getByte(start + 3)) < 1 || offSize0 > 4) {
    return fofiIdUnknown;
  }
  pos = start + hdrSize;
  if (pos < 0) {
    return fofiIdUnknown;
  }

  //----- skip the name index
  if (!reader->getU16BE(pos, &n)) {
    return fofiIdUnknown;
  }
  if (n == 0) {
    pos += 2;
  } else {
    if ((offSize1 = reader->getByte(pos + 2)) < 1 || offSize1 > 4) {
      return fofiIdUnknown;
    }
    if (!reader->getUVarBE(pos + 3 + n * offSize1, offSize1, &offset1) ||
	offset1 > (Guint)INT_MAX) {
      return fofiIdUnknown;
    }
    pos += 3 + (n + 1) * offSize1 + (int)offset1 - 1;
  }
  if (pos < 0) {
    return fofiIdUnknown;
  }

  //----- parse the top dict index
  if (!reader->getU16BE(pos, &n) || n < 1) {
    return fofiIdUnknown;
  }
  if ((offSize1 = reader->getByte(pos + 2)) < 1 || offSize1 > 4) {
    return fofiIdUnknown;
  }
  if (!reader->getUVarBE(pos + 3, offSize1, &offset0) ||
      offset0 > (Guint)INT_MAX ||
      !reader->getUVarBE(pos + 3 + offSize1, offSize1, &offset1) ||
      offset1 > (Guint)INT_MAX ||
      offset0 > offset1) {
    return fofiIdUnknown;
  }
  pos = pos + 3 + (n + 1) * offSize1 + (int)offset0 - 1;
  endPos = pos + 3 + (n + 1) * offSize1 + (int)offset1 - 1;
  if (pos < 0 || endPos < 0 || pos > endPos) {
    return fofiIdUnknown;
  }
  
  //----- parse the top dict, look for ROS as first entry
  // for a CID font, the top dict starts with:
  //     <int> <int> <int> ROS
  for (i = 0; i < 3; ++i) {
    b0 = reader->getByte(pos++);
    if (b0 == 0x1c) {
      pos += 2;
    } else if (b0 == 0x1d) {
      pos += 4;
    } else if (b0 >= 0xf7 && b0 <= 0xfe) {
      pos += 1;
    } else if (b0 < 0x20 || b0 > 0xf6) {
      return fofiIdCFF8Bit;
    }
    if (pos >= endPos || pos < 0) {
      return fofiIdCFF8Bit;
    }
  }
  if (pos + 1 < endPos &&
      reader->getByte(pos) == 12 &&
      reader->getByte(pos + 1) == 30) {
    return fofiIdCFFCID;
  } else {
    return fofiIdCFF8Bit;
  }
}

//------------------------------------------------------------------------

static GList *getTTCFontList(FILE *f);
static GList *getDfontFontList(FILE *f);

GList *FoFiIdentifier::getFontList(char *fileName) {
  FILE *f;
  char buf[4];
  GList *ret;

  if (!(f = fopen(fileName, "rb"))) {
    return NULL;
  }
  if (fread(buf, 1, 4, f) == 4 &&
      buf[0] == 0x74 &&    // 'ttcf'
      buf[1] == 0x74 &&
      buf[2] == 0x63 &&
      buf[3] == 0x66) {
    ret = getTTCFontList(f);
  } else {
    ret = getDfontFontList(f);
  }
  fclose(f);
  return ret;
}

static GList *getTTCFontList(FILE *f) {
  Guchar buf[12];
  Guchar *buf2;
  int fileLength, nFonts;
  int tabDirOffset, nTables, nameTabOffset, nNames, stringsOffset;
  int stringPlatform, stringLength, stringOffset;
  GBool stringUnicode;
  int i, j;
  GList *ret;

  fseek(f, 0, SEEK_END);
  fileLength = (int)ftell(f);
  if (fileLength < 0) {
    goto err1;
  }
  fseek(f, 8, SEEK_SET);
  if (fread(buf, 1, 4, f) != 4) {
    goto err1;
  }
  nFonts = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
  if (nFonts < 0 ||
      12 + 4 * nFonts > fileLength) {
    goto err1;
  }
  ret = new GList();
  for (i = 0; i < nFonts; ++i) {
    fseek(f, 12 + 4 * i, SEEK_SET);
    if (fread(buf, 1, 4, f) != 4) {
      goto err2;
    }
    tabDirOffset = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    if (tabDirOffset < 0 ||
	tabDirOffset + 12 < 0 ||
	tabDirOffset + 12 > fileLength) {
      goto err2;
    }
    fseek(f, tabDirOffset, SEEK_SET);
    if (fread(buf, 1, 12, f) != 12) {
      goto err2;
    }
    nTables = (buf[4] << 8) | buf[5];
    if (tabDirOffset + 12 + 16 * nTables < 0 ||
	tabDirOffset + 12 + 16 * nTables > fileLength) {
      goto err2;
    }
    buf2 = (Guchar *)gmallocn(nTables, 16);
    if ((int)fread(buf2, 1, 16 * nTables, f) != 16 * nTables) {
      goto err3;
    }
    nameTabOffset = 0; // make gcc happy
    for (j = 0; j < nTables; ++j) {
      if (buf2[16*j + 0] == 'n' &&
	  buf2[16*j + 1] == 'a' &&
	  buf2[16*j + 2] == 'm' &&
	  buf2[16*j + 3] == 'e') {
	nameTabOffset = (buf2[16*j + 8] << 24) | (buf2[16*j + 9] << 16) |
	                (buf2[16*j + 10] << 8) | buf2[16*j + 11];
	break;
      }
    }
    gfree(buf2);
    if (j >= nTables) {
      goto err2;
    }
    if (nameTabOffset < 0 ||
	nameTabOffset + 6 < 0 ||
	nameTabOffset + 6 > fileLength) {
      goto err2;
    }
    fseek(f, nameTabOffset, SEEK_SET);
    if (fread(buf, 1, 6, f) != 6) {
      goto err2;
    }
    nNames = (buf[2] << 8) | buf[3];
    stringsOffset = (buf[4] << 8) | buf[5];
    if (nameTabOffset + 6 + 12 * nNames < 0 ||
	nameTabOffset + 6 + 12 * nNames > fileLength ||
	nameTabOffset + stringsOffset < 0) {
      goto err2;
    }
    buf2 = (Guchar *)gmallocn(nNames, 12);
    if ((int)fread(buf2, 1, 12 * nNames, f) != 12 * nNames) {
      goto err3;
    }
    for (j = 0; j < nNames; ++j) {
      if (buf2[12*j + 6] == 0 &&   // 0x0004 = full name
	  buf2[12*j + 7] == 4) {
	break;
      }
    }
    if (j >= nNames) {
      goto err3;
    }
    stringPlatform = (buf2[12*j] << 8) | buf2[12*j + 1];
    // stringEncoding = (buf2[12*j + 2] << 8) | buf2[12*j + 3];
    stringUnicode = stringPlatform == 0 || stringPlatform == 3;
    stringLength = (buf2[12*j + 8] << 8) | buf2[12*j + 9];
    stringOffset = nameTabOffset + stringsOffset +
                   ((buf2[12*j + 10] << 8) | buf2[12*j + 11]);
    gfree(buf2);
    if (stringOffset < 0 ||
	stringOffset + stringLength < 0 ||
	stringOffset + stringLength > fileLength) {
      goto err2;
    }
    buf2 = (Guchar *)gmalloc(stringLength);
    fseek(f, stringOffset, SEEK_SET);
    if ((int)fread(buf2, 1, stringLength, f) != stringLength) {
      goto err3;
    }
    if (stringUnicode) {
      stringLength /= 2;
      for (j = 0; j < stringLength; ++j) {
	buf2[j] = buf2[2*j + 1];
      }
    }
    ret->append(new GString((char *)buf2, stringLength));
    gfree(buf2);
  }
  return ret;

 err3:
  gfree(buf2);
 err2:
  deleteGList(ret, GString);
 err1:
  return NULL;
}

static GList *getDfontFontList(FILE *f) {
  Guchar buf[16];
  Guchar *resMap;
  int fileLength, resMapOffset, resMapLength;
  int resTypeListOffset, resNameListOffset, nTypes;
  int refListOffset, nFonts, nameOffset, nameLen;
  int offset, i;
  GList *ret;

  fseek(f, 0, SEEK_END);
  fileLength = (int)ftell(f);
  if (fileLength < 0) {
    goto err1;
  }
  fseek(f, 0, SEEK_SET);
  if (fread((char *)buf, 1, 16, f) != 16) {
    goto err1;
  }
  resMapOffset = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
  resMapLength = (buf[12] << 24) | (buf[13] << 16) | (buf[14] << 8) | buf[15];
  if (resMapOffset < 0 ||
      resMapOffset >= fileLength ||
      resMapLength < 0 ||
      resMapOffset + resMapLength > fileLength ||
      resMapOffset + resMapLength < 0) {
    goto err1;
  }
  if (resMapLength > 32768) {
    // sanity check - this probably isn't a dfont file
    goto err1;
  }
  resMap = (Guchar *)gmalloc(resMapLength);
  fseek(f, resMapOffset, SEEK_SET);
  if ((int)fread((char *)resMap, 1, resMapLength, f) != resMapLength) {
    goto err2;
  }
  resTypeListOffset = (resMap[24] << 8) | resMap[25];
  resNameListOffset = (resMap[26] << 8) | resMap[27];
  nTypes = ((resMap[28] << 8) | resMap[29]) + 1;
  if (resTypeListOffset + 2 + nTypes * 8 > resMapLength ||
      resNameListOffset >= resMapLength) {
    goto err2;
  }
  for (i = 0; i < nTypes; ++i) {
    offset = resTypeListOffset + 2 + 8 * i;
    if (resMap[offset] == 0x73 &&    // 'sfnt'
	resMap[offset+1] == 0x66 &&
	resMap[offset+2] == 0x6e &&
	resMap[offset+3] == 0x74) {
      nFonts = ((resMap[offset+4] << 8) | resMap[offset+5]) + 1;
      refListOffset = (resMap[offset+6] << 8) | resMap[offset+7];
      break;
    }
  }
  if (i >= nTypes) {
    goto err2;
  }
  if (resTypeListOffset + refListOffset >= resMapLength ||
      resTypeListOffset + refListOffset + nFonts * 12 > resMapLength) {
    goto err2;
  }
  ret = new GList();
  for (i = 0; i < nFonts; ++i) {
    offset = resTypeListOffset + refListOffset + 12 * i;
    nameOffset = (resMap[offset+2] << 8) | resMap[offset+3];
    offset = resNameListOffset + nameOffset;
    if (offset >= resMapLength) {
      goto err3;
    }
    nameLen = resMap[offset];
    if (offset + 1 + nameLen > resMapLength) {
      goto err3;
    }
    ret->append(new GString((char *)resMap + offset + 1, nameLen));
  }
  gfree(resMap);
  return ret;

 err3:
  deleteGList(ret, GString);
 err2:
  gfree(resMap);
 err1:
  return NULL;
}
