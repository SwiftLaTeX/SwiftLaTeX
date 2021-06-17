//========================================================================
//
// XRef.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "gmem.h"
#include "gmempp.h"
#include "gfile.h"
#include "Object.h"
#include "Stream.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dict.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "XRef.h"

//------------------------------------------------------------------------

#define xrefSearchSize 1024	// read this many bytes at end of file
				//   to look for 'startxref'

//------------------------------------------------------------------------
// Permission bits
//------------------------------------------------------------------------

#define permPrint    (1<<2)
#define permChange   (1<<3)
#define permCopy     (1<<4)
#define permNotes    (1<<5)
#define defPermFlags 0xfffc

//------------------------------------------------------------------------
// XRefPosSet
//------------------------------------------------------------------------

class XRefPosSet {
public:

  XRefPosSet();
  ~XRefPosSet();
  void add(GFileOffset pos);
  GBool check(GFileOffset pos);
  int getLength() { return len; }
  GFileOffset get(int idx) { return tab[idx]; }

private:

  int find(GFileOffset pos);

  GFileOffset *tab;
  int size;
  int len;
};

XRefPosSet::XRefPosSet() {
  size = 16;
  len = 0;
  tab = (GFileOffset *)gmallocn(size, sizeof(GFileOffset));
}

XRefPosSet::~XRefPosSet() {
  gfree(tab);
}

void XRefPosSet::add(GFileOffset pos) {
  int i;

  i = find(pos);
  if (i < len && tab[i] == pos) {
    return;
  }
  if (len == size) {
    if (size > INT_MAX / 2) {
      gMemError("Integer overflow in XRefPosSet::add()");
    }
    size *= 2;
    tab = (GFileOffset *)greallocn(tab, size, sizeof(GFileOffset));
  }
  if (i < len) {
    memmove(&tab[i + 1], &tab[i], (len - i) * sizeof(GFileOffset));
  }
  tab[i] = pos;
  ++len;
}

GBool XRefPosSet::check(GFileOffset pos) {
  int i;

  i = find(pos);
  return i < len && tab[i] == pos;
}

int XRefPosSet::find(GFileOffset pos) {
  int a, b, m;

  a = - 1;
  b = len;
  // invariant: tab[a] < pos < tab[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (tab[m] < pos) {
      a = m;
    } else if (tab[m] > pos) {
      b = m;
    } else {
      return m;
    }
  }
  return b;
}

//------------------------------------------------------------------------
// ObjectStream
//------------------------------------------------------------------------

class ObjectStream {
public:

  // Create an object stream, using object number <objStrNum>,
  // generation 0.
  ObjectStream(XRef *xref, int objStrNumA);

  GBool isOk() { return ok; }

  ~ObjectStream();

  // Return the object number of this object stream.
  int getObjStrNum() { return objStrNum; }

  // Get the <objIdx>th object from this stream, which should be
  // object number <objNum>, generation 0.
  Object *getObject(int objIdx, int objNum, Object *obj);

private:

  int objStrNum;		// object number of the object stream
  int nObjects;			// number of objects in the stream
  Object *objs;			// the objects (length = nObjects)
  int *objNums;			// the object numbers (length = nObjects)
  GBool ok;
};

ObjectStream::ObjectStream(XRef *xref, int objStrNumA) {
  Stream *str;
  Lexer *lexer;
  Parser *parser;
  int *offsets;
  Object objStr, obj1, obj2;
  int first, i;

  objStrNum = objStrNumA;
  nObjects = 0;
  objs = NULL;
  objNums = NULL;
  ok = gFalse;

  if (!xref->fetch(objStrNum, 0, &objStr)->isStream()) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("N", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  nObjects = obj1.getInt();
  obj1.free();
  if (nObjects <= 0) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("First", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  first = obj1.getInt();
  obj1.free();
  if (first < 0) {
    goto err1;
  }

  // this is an arbitrary limit to avoid integer overflow problems
  // in the 'new Object[nObjects]' call (Acrobat apparently limits
  // object streams to 100-200 objects)
  if (nObjects > 1000000) {
    error(errSyntaxError, -1, "Too many objects in an object stream");
    goto err1;
  }
  objs = new Object[nObjects];
  objNums = (int *)gmallocn(nObjects, sizeof(int));
  offsets = (int *)gmallocn(nObjects, sizeof(int));

  // parse the header: object numbers and offsets
  objStr.streamReset();
  obj1.initNull();
  str = new EmbedStream(objStr.getStream(), &obj1, gTrue, first);
  lexer = new Lexer(xref, str);
  parser = new Parser(xref, lexer, gFalse);
  for (i = 0; i < nObjects; ++i) {
    parser->getObj(&obj1, gTrue);
    parser->getObj(&obj2, gTrue);
    if (!obj1.isInt() || !obj2.isInt()) {
      obj1.free();
      obj2.free();
      delete parser;
      gfree(offsets);
      goto err2;
    }
    objNums[i] = obj1.getInt();
    offsets[i] = obj2.getInt();
    obj1.free();
    obj2.free();
    if (objNums[i] < 0 || offsets[i] < 0 ||
	(i > 0 && offsets[i] < offsets[i-1])) {
      delete parser;
      gfree(offsets);
      goto err2;
    }
  }
  lexer->skipToEOF();
  delete parser;

  // skip to the first object - this shouldn't be necessary because
  // the First key is supposed to be equal to offsets[0], but just in
  // case...
  if (first < offsets[0]) {
    objStr.getStream()->discardChars(offsets[0] - first);
  }

  // parse the objects
  for (i = 0; i < nObjects; ++i) {
    obj1.initNull();
    if (i == nObjects - 1) {
      str = new EmbedStream(objStr.getStream(), &obj1, gFalse, 0);
    } else {
      str = new EmbedStream(objStr.getStream(), &obj1, gTrue,
			    offsets[i+1] - offsets[i]);
    }
    lexer = new Lexer(xref, str);
    parser = new Parser(xref, lexer, gFalse);
    parser->getObj(&objs[i]);
    lexer->skipToEOF();
    delete parser;
  }

  gfree(offsets);
  ok = gTrue;

 err2:
  objStr.streamClose();
 err1:
  objStr.free();
}

ObjectStream::~ObjectStream() {
  int i;

  if (objs) {
    for (i = 0; i < nObjects; ++i) {
      objs[i].free();
    }
    delete[] objs;
  }
  gfree(objNums);
}

Object *ObjectStream::getObject(int objIdx, int objNum, Object *obj) {
  if (objIdx < 0 || objIdx >= nObjects || objNum != objNums[objIdx]) {
    obj->initNull();
  } else {
    objs[objIdx].copy(obj);
  }
  return obj;
}

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

XRef::XRef(BaseStream *strA, GBool repair) {
  GFileOffset pos;
  Object obj;
  XRefPosSet *posSet;
  int i;

  ok = gTrue;
  errCode = errNone;
  size = 0;
  last = -1;
  entries = NULL;
  lastStartxrefPos = 0;
  xrefTablePos = NULL;
  xrefTablePosLen = 0;
  streamEnds = NULL;
  streamEndsLen = 0;
  for (i = 0; i < objStrCacheSize; ++i) {
    objStrs[i] = NULL;
    objStrLastUse[i] = 0;
  }
  objStrCacheLength = 0;
  objStrTime = 0;

  encrypted = gFalse;
  permFlags = defPermFlags;
  ownerPasswordOk = gFalse;

  for (i = 0; i < xrefCacheSize; ++i) {
    cache[i].num = -1;
  }

#if MULTITHREADED
  gInitMutex(&objStrsMutex);
  gInitMutex(&cacheMutex);
#endif

  str = strA;
  start = str->getStart();

  // if the 'repair' flag is set, try to reconstruct the xref table
  if (repair) {
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }

  // if the 'repair' flag is not set, read the xref table
  } else {

    // read the trailer
    pos = getStartXref();
    if (pos == 0) {
      errCode = errDamaged;
      ok = gFalse;
      return;
    }

    // read the xref table
    posSet = new XRefPosSet();
    while (readXRef(&pos, posSet)) ;
    xrefTablePosLen = posSet->getLength();
    xrefTablePos = (GFileOffset *)gmallocn(xrefTablePosLen,
					   sizeof(GFileOffset));
    for (i = 0; i < xrefTablePosLen; ++i)  {
      xrefTablePos[i] = posSet->get(i);
    }
    delete posSet;
    if (!ok) {
      errCode = errDamaged;
      return;
    }
  }

  // get the root dictionary (catalog) object
  trailerDict.dictLookupNF("Root", &obj);
  if (obj.isRef()) {
    rootNum = obj.getRefNum();
    rootGen = obj.getRefGen();
    obj.free();
  } else {
    obj.free();
    if (!(ok = constructXRef())) {
      errCode = errDamaged;
      return;
    }
  }

  // now set the trailer dictionary's xref pointer so we can fetch
  // indirect objects from it
  trailerDict.getDict()->setXRef(this);
}

XRef::~XRef() {
  int i;

  for (i = 0; i < xrefCacheSize; ++i) {
    if (cache[i].num >= 0) {
      cache[i].obj.free();
    }
  }
  gfree(entries);
  trailerDict.free();
  if (xrefTablePos) {
    gfree(xrefTablePos);
  }
  if (streamEnds) {
    gfree(streamEnds);
  }
  for (i = 0; i < objStrCacheSize; ++i) {
    if (objStrs[i]) {
      delete objStrs[i];
    }
  }
#if MULTITHREADED
  gDestroyMutex(&objStrsMutex);
  gDestroyMutex(&cacheMutex);
#endif
}

// Read the 'startxref' position.
GFileOffset XRef::getStartXref() {
  char buf[xrefSearchSize+1];
  char *p;
  int n, i;

  // read last xrefSearchSize bytes
  str->setPos(xrefSearchSize, -1);
  n = str->getBlock(buf, xrefSearchSize);
  buf[n] = '\0';

  // find startxref
  for (i = n - 9; i >= 0; --i) {
    if (!strncmp(&buf[i], "startxref", 9)) {
      break;
    }
  }
  if (i < 0) {
    return 0;
  }
  for (p = &buf[i+9]; isspace(*p & 0xff); ++p) ;
  lastXRefPos = strToFileOffset(p);
  lastStartxrefPos = str->getPos() - n + i;

  return lastXRefPos;
}

// Read one xref table section.  Also reads the associated trailer
// dictionary, and returns the prev pointer (if any).
GBool XRef::readXRef(GFileOffset *pos, XRefPosSet *posSet) {
  Parser *parser;
  Object obj;
  GBool more;
  char buf[100];
  int n, i;

  // check for a loop in the xref tables
  if (posSet->check(*pos)) {
    error(errSyntaxWarning, -1, "Infinite loop in xref table");
    return gFalse;
  }
  posSet->add(*pos);

  // the xref data should either be "xref ..." (for an xref table) or
  // "nn gg obj << ... >> stream ..." (for an xref stream); possibly
  // preceded by whitespace
  str->setPos(start + *pos);
  n = str->getBlock(buf, 100);
  for (i = 0; i < n && Lexer::isSpace(buf[i]); ++i) ;

  // parse an old-style xref table
  if (i + 4 < n &&
      buf[i] == 'x' && buf[i+1] == 'r' && buf[i+2] == 'e' && buf[i+3] == 'f' &&
      Lexer::isSpace(buf[i+4])) {
    more = readXRefTable(pos, i + 5, posSet);

  // parse an xref stream
  } else if (i < n && buf[i] >= '0' && buf[i] <= '9') {
    obj.initNull();
    parser = new Parser(NULL,
	       new Lexer(NULL,
		 str->makeSubStream(start + *pos, gFalse, 0, &obj)),
	       gTrue);
    if (!parser->getObj(&obj, gTrue)->isInt()) {
      goto err2;
    }
    obj.free();
    if (!parser->getObj(&obj, gTrue)->isInt()) {
      goto err2;
    }
    obj.free();
    if (!parser->getObj(&obj, gTrue)->isCmd("obj")) {
      goto err2;
    }
    obj.free();
    if (!parser->getObj(&obj)->isStream()) {
      goto err2;
    }
    more = readXRefStream(obj.getStream(), pos);
    obj.free();
    delete parser;

  } else {
    goto err1;
  }

  return more;

 err2:
  obj.free();
  delete parser;
 err1:
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefTable(GFileOffset *pos, int offset, XRefPosSet *posSet) {
  XRefEntry entry;
  Parser *parser;
  Object obj, obj2;
  char buf[6];
  GFileOffset off, pos2;
  GBool more;
  int first, n, newSize, gen, i, c;

  str->setPos(start + *pos + offset);

  while (1) {
    do {
      c = str->getChar();
    } while (Lexer::isSpace(c));
    if (c == 't') {
      if (str->getBlock(buf, 6) != 6 || memcmp(buf, "railer", 6)) {
	goto err1;
      }
      break;
    }
    if (c < '0' || c > '9') {
      goto err1;
    }
    first = 0;
    do {
      first = (first * 10) + (c - '0');
      c = str->getChar();
    } while (c >= '0' && c <= '9');
    if (!Lexer::isSpace(c)) {
      goto err1;
    }
    do {
      c = str->getChar();
    } while (Lexer::isSpace(c));
    n = 0;
    do {
      n = (n * 10) + (c - '0');
      c = str->getChar();
    } while (c >= '0' && c <= '9');
    if (!Lexer::isSpace(c)) {
      goto err1;
    }
    if (first < 0 || n < 0 || first > INT_MAX - n) {
      goto err1;
    }
    if (first + n > size) {
      for (newSize = size ? 2 * size : 1024;
	   first + n > newSize && newSize > 0;
	   newSize <<= 1) ;
      if (newSize < 0) {
	goto err1;
      }
      entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
      for (i = size; i < newSize; ++i) {
	entries[i].offset = (GFileOffset)-1;
	entries[i].type = xrefEntryFree;
      }
      size = newSize;
    }
    for (i = first; i < first + n; ++i) {
      do {
	c = str->getChar();
      } while (Lexer::isSpace(c));
      off = 0;
      do {
	off = (off * 10) + (c - '0');
	c = str->getChar();
      } while (c >= '0' && c <= '9');
      if (!Lexer::isSpace(c)) {
	goto err1;
      }
      entry.offset = off;
      do {
	c = str->getChar();
      } while (Lexer::isSpace(c));
      gen = 0;
      do {
	gen = (gen * 10) + (c - '0');
	c = str->getChar();
      } while (c >= '0' && c <= '9');
      if (!Lexer::isSpace(c)) {
	goto err1;
      }
      entry.gen = gen;
      do {
	c = str->getChar();
      } while (Lexer::isSpace(c));
      if (c == 'n') {
	entry.type = xrefEntryUncompressed;
      } else if (c == 'f') {
	entry.type = xrefEntryFree;
      } else {
	goto err1;
      }
      c = str->getChar();
      if (!Lexer::isSpace(c)) {
	goto err1;
      }
      if (entries[i].offset == (GFileOffset)-1) {
	entries[i] = entry;
	// PDF files of patents from the IBM Intellectual Property
	// Network have a bug: the xref table claims to start at 1
	// instead of 0.
	if (i == 1 && first == 1 &&
	    entries[1].offset == 0 && entries[1].gen == 65535 &&
	    entries[1].type == xrefEntryFree) {
	  i = first = 0;
	  entries[0] = entries[1];
	  entries[1].offset = (GFileOffset)-1;
	}
	if (i > last) {
	  last = i;
	}
      }
    }
  }

  // read the trailer dictionary
  obj.initNull();
  parser = new Parser(NULL,
	     new Lexer(NULL,
	       str->makeSubStream(str->getPos(), gFalse, 0, &obj)),
	     gTrue);
  parser->getObj(&obj);
  delete parser;
  if (!obj.isDict()) {
    obj.free();
    goto err1;
  }

  // get the 'Prev' pointer
  //~ this can be a 64-bit int (?)
  obj.getDict()->lookupNF("Prev", &obj2);
  if (obj2.isInt()) {
    *pos = (GFileOffset)(Guint)obj2.getInt();
    more = gTrue;
  } else if (obj2.isRef()) {
    // certain buggy PDF generators generate "/Prev NNN 0 R" instead
    // of "/Prev NNN"
    *pos = (GFileOffset)(Guint)obj2.getRefNum();
    more = gTrue;
  } else {
    more = gFalse;
  }
  obj2.free();

  // save the first trailer dictionary
  if (trailerDict.isNone()) {
    obj.copy(&trailerDict);
  }

  // check for an 'XRefStm' key
  //~ this can be a 64-bit int (?)
  if (obj.getDict()->lookup("XRefStm", &obj2)->isInt()) {
    pos2 = (GFileOffset)(Guint)obj2.getInt();
    readXRef(&pos2, posSet);
    if (!ok) {
      obj2.free();
      obj.free();
      goto err1;
    }
  }
  obj2.free();

  obj.free();
  return more;

 err1:
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefStream(Stream *xrefStr, GFileOffset *pos) {
  Dict *dict;
  int w[3];
  GBool more;
  Object obj, obj2, idx;
  int newSize, first, n, i;

  dict = xrefStr->getDict();

  if (!dict->lookupNF("Size", &obj)->isInt()) {
    goto err1;
  }
  newSize = obj.getInt();
  obj.free();
  if (newSize < 0) {
    goto err1;
  }
  if (newSize > size) {
    entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = (GFileOffset)-1;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
  }

  if (!dict->lookupNF("W", &obj)->isArray() ||
      obj.arrayGetLength() < 3) {
    goto err1;
  }
  for (i = 0; i < 3; ++i) {
    if (!obj.arrayGet(i, &obj2)->isInt()) {
      obj2.free();
      goto err1;
    }
    w[i] = obj2.getInt();
    obj2.free();
  }
  obj.free();
  if (w[0] < 0 || w[0] > 8 ||
      w[1] < 0 || w[1] > 8 ||
      w[2] < 0 || w[2] > 8) {
    goto err0;
  }

  xrefStr->reset();
  dict->lookupNF("Index", &idx);
  if (idx.isArray()) {
    for (i = 0; i+1 < idx.arrayGetLength(); i += 2) {
      if (!idx.arrayGet(i, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      first = obj.getInt();
      obj.free();
      if (!idx.arrayGet(i+1, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      n = obj.getInt();
      obj.free();
      if (first < 0 || n < 0 ||
	  !readXRefStreamSection(xrefStr, w, first, n)) {
	idx.free();
	goto err0;
      }
    }
  } else {
    if (!readXRefStreamSection(xrefStr, w, 0, newSize)) {
      idx.free();
      goto err0;
    }
  }
  idx.free();

  //~ this can be a 64-bit int (?)
  dict->lookupNF("Prev", &obj);
  if (obj.isInt()) {
    *pos = (GFileOffset)(Guint)obj.getInt();
    more = gTrue;
  } else {
    more = gFalse;
  }
  obj.free();
  if (trailerDict.isNone()) {
    trailerDict.initDict(dict);
  }

  return more;

 err1:
  obj.free();
 err0:
  ok = gFalse;
  return gFalse;
}

GBool XRef::readXRefStreamSection(Stream *xrefStr, int *w, int first, int n) {
  long long type, gen, offset;
  int c, newSize, i, j;

  if (first + n < 0) {
    return gFalse;
  }
  if (first + n > size) {
    for (newSize = size ? 2 * size : 1024;
	 first + n > newSize && newSize > 0;
	 newSize <<= 1) ;
    if (newSize < 0) {
      return gFalse;
    }
    entries = (XRefEntry *)greallocn(entries, newSize, sizeof(XRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = (GFileOffset)-1;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
  }
  for (i = first; i < first + n; ++i) {
    if (w[0] == 0) {
      type = 1;
    } else {
      for (type = 0, j = 0; j < w[0]; ++j) {
	if ((c = xrefStr->getChar()) == EOF) {
	  return gFalse;
	}
	type = (type << 8) + c;
      }
    }
    for (offset = 0, j = 0; j < w[1]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return gFalse;
      }
      offset = (offset << 8) + c;
    }
    if (offset < 0 || offset > GFILEOFFSET_MAX) {
      return gFalse;
    }
    for (gen = 0, j = 0; j < w[2]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return gFalse;
      }
      gen = (gen << 8) + c;
    }
    if (gen < 0 || gen > INT_MAX) {
      return gFalse;
    }
    if (entries[i].offset == (GFileOffset)-1) {
      switch (type) {
      case 0:
	entries[i].offset = (GFileOffset)offset;
	entries[i].gen = (int)gen;
	entries[i].type = xrefEntryFree;
	break;
      case 1:
	entries[i].offset = (GFileOffset)offset;
	entries[i].gen = (int)gen;
	entries[i].type = xrefEntryUncompressed;
	break;
      case 2:
	entries[i].offset = (GFileOffset)offset;
	entries[i].gen = (int)gen;
	entries[i].type = xrefEntryCompressed;
	break;
      default:
	return gFalse;
      }
      if (i > last) {
	last = i;
      }
    }
  }

  return gTrue;
}

// Attempt to construct an xref table for a damaged file.
GBool XRef::constructXRef() {
  Parser *parser;
  Object newTrailerDict, obj;
  char buf[256];
  GFileOffset pos;
  int num, gen;
  int newSize;
  int streamEndsSize;
  char *p;
  int i;
  GBool gotRoot;

  gfree(entries);
  size = 0;
  entries = NULL;

  gotRoot = gFalse;
  streamEndsLen = streamEndsSize = 0;

  str->reset();
  while (1) {
    pos = str->getPos();
    if (!str->getLine(buf, 256)) {
      break;
    }
    p = buf;

    // skip whitespace
    while (*p && Lexer::isSpace(*p & 0xff)) ++p;

    // got trailer dictionary
    if (!strncmp(p, "trailer", 7)) {
      obj.initNull();
      parser = new Parser(NULL,
		 new Lexer(NULL,
		   str->makeSubStream(pos + 7, gFalse, 0, &obj)),
		 gFalse);
      parser->getObj(&newTrailerDict);
      if (newTrailerDict.isDict()) {
	newTrailerDict.dictLookupNF("Root", &obj);
	if (obj.isRef()) {
	  rootNum = obj.getRefNum();
	  rootGen = obj.getRefGen();
	  if (!trailerDict.isNone()) {
	    trailerDict.free();
	  }
	  newTrailerDict.copy(&trailerDict);
	  gotRoot = gTrue;
	}
	obj.free();
      }
      newTrailerDict.free();
      delete parser;

    // look for object
    } else if (isdigit(*p & 0xff)) {
      num = atoi(p);
      if (num > 0) {
	do {
	  ++p;
	} while (*p && isdigit(*p & 0xff));
	if (isspace(*p & 0xff)) {
	  do {
	    ++p;
	  } while (*p && isspace(*p & 0xff));
	  if (isdigit(*p & 0xff)) {
	    gen = atoi(p);
	    do {
	      ++p;
	    } while (*p && isdigit(*p & 0xff));
	    if (isspace(*p & 0xff)) {
	      do {
		++p;
	      } while (*p && isspace(*p & 0xff));
	      if (!strncmp(p, "obj", 3)) {
		if (num >= size) {
		  newSize = (num + 1 + 255) & ~255;
		  if (newSize < 0) {
		    error(errSyntaxError, -1, "Bad object number");
		    return gFalse;
		  }
		  entries = (XRefEntry *)
		      greallocn(entries, newSize, sizeof(XRefEntry));
		  for (i = size; i < newSize; ++i) {
		    entries[i].offset = (GFileOffset)-1;
		    entries[i].type = xrefEntryFree;
		  }
		  size = newSize;
		}
		if (entries[num].type == xrefEntryFree ||
		    gen >= entries[num].gen) {
		  entries[num].offset = pos - start;
		  entries[num].gen = gen;
		  entries[num].type = xrefEntryUncompressed;
		  if (num > last) {
		    last = num;
		  }
		}
	      }
	    }
	  }
	}
      }

    } else if (!strncmp(p, "endstream", 9)) {
      if (streamEndsLen == streamEndsSize) {
	streamEndsSize += 64;
	streamEnds = (GFileOffset *)greallocn(streamEnds, streamEndsSize,
					      sizeof(GFileOffset));
      }
      streamEnds[streamEndsLen++] = pos;
    }
  }

  if (gotRoot) {
    return gTrue;
  }

  error(errSyntaxError, -1, "Couldn't find trailer dictionary");
  return gFalse;
}

void XRef::setEncryption(int permFlagsA, GBool ownerPasswordOkA,
			 Guchar *fileKeyA, int keyLengthA, int encVersionA,
			 CryptAlgorithm encAlgorithmA) {
  int i;

  encrypted = gTrue;
  permFlags = permFlagsA;
  ownerPasswordOk = ownerPasswordOkA;
  if (keyLengthA <= 32) {
    keyLength = keyLengthA;
  } else {
    keyLength = 32;
  }
  for (i = 0; i < keyLength; ++i) {
    fileKey[i] = fileKeyA[i];
  }
  encVersion = encVersionA;
  encAlgorithm = encAlgorithmA;
}

GBool XRef::getEncryption(int *permFlagsA, GBool *ownerPasswordOkA,
			  int *keyLengthA, int *encVersionA,
			  CryptAlgorithm *encAlgorithmA) {
  if (!encrypted) {
    return gFalse;
  }
  *permFlagsA = permFlags;
  *ownerPasswordOkA = ownerPasswordOk;
  *keyLengthA = keyLength;
  *encVersionA = encVersion;
  *encAlgorithmA = encAlgorithm;
  return gTrue;
}

GBool XRef::okToPrint(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permPrint);
}

GBool XRef::okToChange(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permChange);
}

GBool XRef::okToCopy(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permCopy);
}

GBool XRef::okToAddNotes(GBool ignoreOwnerPW) {
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permNotes);
}

Object *XRef::fetch(int num, int gen, Object *obj, int recursion) {
  XRefEntry *e;
  Parser *parser;
  Object obj1, obj2, obj3;
  XRefCacheEntry tmp;
  int i, j;

  // check for bogus ref - this can happen in corrupted PDF files
  if (num < 0 || num >= size) {
    goto err;
  }

  // check the cache
#if MULTITHREADED
  gLockMutex(&cacheMutex);
#endif
  if (cache[0].num == num && cache[0].gen == gen) {
    cache[0].obj.copy(obj);
#if MULTITHREADED
    gUnlockMutex(&cacheMutex);
#endif
    return obj;
  }
  for (i = 1; i < xrefCacheSize; ++i) {
    if (cache[i].num == num && cache[i].gen == gen) {
      tmp = cache[i];
      for (j = i; j > 0; --j) {
	cache[j] = cache[j - 1];
      }
      cache[0] = tmp;
      cache[0].obj.copy(obj);
#if MULTITHREADED
      gUnlockMutex(&cacheMutex);
#endif
      return obj;
    }
  }
#if MULTITHREADED
  gUnlockMutex(&cacheMutex);
#endif

  e = &entries[num];
  switch (e->type) {

  case xrefEntryUncompressed:
    if (e->gen != gen) {
      goto err;
    }
    obj1.initNull();
    parser = new Parser(this,
	       new Lexer(this,
		 str->makeSubStream(start + e->offset, gFalse, 0, &obj1)),
	       gTrue);
    parser->getObj(&obj1, gTrue);
    parser->getObj(&obj2, gTrue);
    parser->getObj(&obj3, gTrue);
    if (!obj1.isInt() || obj1.getInt() != num ||
	!obj2.isInt() || obj2.getInt() != gen ||
	!obj3.isCmd("obj")) {
      obj1.free();
      obj2.free();
      obj3.free();
      delete parser;
      goto err;
    }
    parser->getObj(obj, gFalse, encrypted ? fileKey : (Guchar *)NULL,
		   encAlgorithm, keyLength, num, gen, recursion);
    obj1.free();
    obj2.free();
    obj3.free();
    delete parser;
    break;

  case xrefEntryCompressed:
#if 0 // Adobe apparently ignores the generation number on compressed objects
    if (gen != 0) {
      goto err;
    }
#endif
    if (e->offset >= (GFileOffset)size ||
	entries[e->offset].type != xrefEntryUncompressed) {
      error(errSyntaxError, -1, "Invalid object stream");
      goto err;
    }
    if (!getObjectStreamObject((int)e->offset, e->gen, num, obj)) {
      goto err;
    }
    break;

  default:
    goto err;
  }

  // put the new object in the cache, throwing away the oldest object
  // currently in the cache
#if MULTITHREADED
  gLockMutex(&cacheMutex);
#endif
  if (cache[xrefCacheSize - 1].num >= 0) {
    cache[xrefCacheSize - 1].obj.free();
  }
  for (i = xrefCacheSize - 1; i > 0; --i) {
    cache[i] = cache[i - 1];
  }
  cache[0].num = num;
  cache[0].gen = gen;
  obj->copy(&cache[0].obj);
#if MULTITHREADED
  gUnlockMutex(&cacheMutex);
#endif

  return obj;

 err:
  return obj->initNull();
}

GBool XRef::getObjectStreamObject(int objStrNum, int objIdx,
				  int objNum, Object *obj) {
  ObjectStream *objStr;

#if MULTITHREADED
  gLockMutex(&objStrsMutex);
#endif
  if (!(objStr = getObjectStream(objStrNum))) {
    return gFalse;
  }
  cleanObjectStreamCache();
  objStr->getObject(objIdx, objNum, obj);
#if MULTITHREADED
  gUnlockMutex(&objStrsMutex);
#endif
  return gTrue;
}

// NB: objStrsMutex must be locked when calling this function.
ObjectStream *XRef::getObjectStream(int objStrNum) {
  ObjectStream *objStr;
  int i, j;

  // check the MRU entry in the cache
  if (objStrs[0] && objStrs[0]->getObjStrNum() == objStrNum) {
    objStr = objStrs[0];
    objStrLastUse[0] = objStrTime++;
    return objStr;
  }

  // check the rest of the cache
  for (i = 1; i < objStrCacheLength; ++i) {
    if (objStrs[i] && objStrs[i]->getObjStrNum() == objStrNum) {
      objStr = objStrs[i];
      for (j = i; j > 0; --j) {
	objStrs[j] = objStrs[j - 1];
	objStrLastUse[j] = objStrLastUse[j - 1];
      }
      objStrs[0] = objStr;
      objStrLastUse[0] = objStrTime++;
      return objStr;
    }
  }

  // load a new ObjectStream
  objStr = new ObjectStream(this, objStrNum);
  if (!objStr->isOk()) {
    delete objStr;
    return NULL;
  }

  // add to the cache
  if (objStrCacheLength == objStrCacheSize) {
    delete objStrs[objStrCacheSize - 1];
    --objStrCacheLength;
  }
  for (j = objStrCacheLength; j > 0; --j) {
    objStrs[j] = objStrs[j - 1];
    objStrLastUse[j] = objStrLastUse[j - 1];
  }
  ++objStrCacheLength;
  objStrs[0] = objStr;
  objStrLastUse[0] = objStrTime++;

  return objStr;
}

// If the oldest (least recently used) entry in the object stream
// cache is more than objStrCacheTimeout accesses old (hasn't been
// used in the last objStrCacheTimeout accesses), eject it from the
// cache.
void XRef::cleanObjectStreamCache() {
  // NB: objStrTime and objStrLastUse[] are unsigned ints, so the
  // mod-2^32 arithmetic makes the subtraction work out, even if the
  // time wraps around.
  if (objStrCacheLength > 1 &&
      objStrTime - objStrLastUse[objStrCacheLength - 1]
        > objStrCacheTimeout) {
    delete objStrs[objStrCacheLength - 1];
    objStrs[objStrCacheLength - 1] = NULL;
    --objStrCacheLength;
  }
}

Object *XRef::getDocInfo(Object *obj) {
  return trailerDict.dictLookup("Info", obj);
}

// Added for the pdftex project.
Object *XRef::getDocInfoNF(Object *obj) {
  return trailerDict.dictLookupNF("Info", obj);
}

GBool XRef::getStreamEnd(GFileOffset streamStart, GFileOffset *streamEnd) {
  int a, b, m;

  if (streamEndsLen == 0 ||
      streamStart > streamEnds[streamEndsLen - 1]) {
    return gFalse;
  }

  a = -1;
  b = streamEndsLen - 1;
  // invariant: streamEnds[a] < streamStart <= streamEnds[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (streamStart <= streamEnds[m]) {
      b = m;
    } else {
      a = m;
    }
  }
  *streamEnd = streamEnds[b];
  return gTrue;
}

GFileOffset XRef::strToFileOffset(char *s) {
  GFileOffset x, d;
  char *p;

  x = 0;
  for (p = s; *p && isdigit(*p & 0xff); ++p) {
    d = *p - '0';
    if (x > (GFILEOFFSET_MAX - d) / 10) {
      break;
    }
    x = 10 * x + d;
  }
  return x;
}
