//========================================================================
//
// XRef.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef XREF_H
#define XREF_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "gfile.h"
#include "Object.h"
#if MULTITHREADED
#include "GMutex.h"
#endif

class Dict;
class Stream;
class Parser;
class ObjectStream;
class XRefPosSet;

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

enum XRefEntryType {
  xrefEntryFree,
  xrefEntryUncompressed,
  xrefEntryCompressed
};

struct XRefEntry {
  GFileOffset offset;
  int gen;
  XRefEntryType type;
};

struct XRefCacheEntry {
  int num;
  int gen;
  Object obj;
};

#define xrefCacheSize 16

#define objStrCacheSize 128
#define objStrCacheTimeout 1000

class XRef {
public:

  // Constructor.  Read xref table from stream.
  XRef(BaseStream *strA, GBool repair);

  // Destructor.
  ~XRef();

  // Is xref table valid?
  GBool isOk() { return ok; }

  // Get the error code (if isOk() returns false).
  int getErrorCode() { return errCode; }

  // Set the encryption parameters.
  void setEncryption(int permFlagsA, GBool ownerPasswordOkA,
		     Guchar *fileKeyA, int keyLengthA, int encVersionA,
		     CryptAlgorithm encAlgorithmA);

  // Is the file encrypted?
  GBool isEncrypted() { return encrypted; }
  GBool getEncryption(int *permFlagsA, GBool *ownerPasswordOkA,
		      int *keyLengthA, int *encVersionA,
		      CryptAlgorithm *encAlgorithmA);

  // Check various permissions.
  GBool okToPrint(GBool ignoreOwnerPW = gFalse);
  GBool okToChange(GBool ignoreOwnerPW = gFalse);
  GBool okToCopy(GBool ignoreOwnerPW = gFalse);
  GBool okToAddNotes(GBool ignoreOwnerPW = gFalse);
  int getPermFlags() { return permFlags; }

  // Get catalog object.
  Object *getCatalog(Object *obj) { return fetch(rootNum, rootGen, obj); }

  // Fetch an indirect reference.
  Object *fetch(int num, int gen, Object *obj, int recursion = 0);

  // Return the document's Info dictionary (if any).
  Object *getDocInfo(Object *obj);
  Object *getDocInfoNF(Object *obj);

  // Return the number of objects in the xref table.
  int getNumObjects() { return last + 1; }

  // Return the offset of the last xref table.
  GFileOffset getLastXRefPos() { return lastXRefPos; }

  // Return the offset of the 'startxref' at the end of the file.
  GFileOffset getLastStartxrefPos() { return lastStartxrefPos; }

  // Return the catalog object reference.
  int getRootNum() { return rootNum; }
  int getRootGen() { return rootGen; }

  // Get the xref table positions.
  int getNumXRefTables() { return xrefTablePosLen; }
  GFileOffset getXRefTablePos(int idx) { return xrefTablePos[idx]; }

  // Get end position for a stream in a damaged file.
  // Returns false if unknown or file is not damaged.
  GBool getStreamEnd(GFileOffset streamStart, GFileOffset *streamEnd);

  // Direct access.
  int getSize() { return size; }
  XRefEntry *getEntry(int i) { return &entries[i]; }
  Object *getTrailerDict() { return &trailerDict; }

private:

  BaseStream *str;		// input stream
  GFileOffset start;		// offset in file (to allow for garbage
				//   at beginning of file)
  XRefEntry *entries;		// xref entries
  int size;			// size of <entries> array
  int last;			// last used index in <entries>
  int rootNum, rootGen;		// catalog dict
  GBool ok;			// true if xref table is valid
  int errCode;			// error code (if <ok> is false)
  Object trailerDict;		// trailer dictionary
  GFileOffset lastXRefPos;	// offset of last xref table
  GFileOffset lastStartxrefPos;	// offset of 'startxref' at end of file
  GFileOffset *xrefTablePos;	// positions of all xref tables
  int xrefTablePosLen;		// number of xref table positions
  GFileOffset *streamEnds;	// 'endstream' positions - only used in
				//   damaged files
  int streamEndsLen;		// number of valid entries in streamEnds
  ObjectStream *		// cached object streams
    objStrs[objStrCacheSize];
  int objStrCacheLength;	// number of valid entries in objStrs[]
  Guint				// time of last use for each obj stream
    objStrLastUse[objStrCacheSize];
  Guint objStrTime;		// current time for the obj stream cache
#if MULTITHREADED
  GMutex objStrsMutex;
#endif
  GBool encrypted;		// true if file is encrypted
  int permFlags;		// permission bits
  GBool ownerPasswordOk;	// true if owner password is correct
  Guchar fileKey[32];		// file decryption key
  int keyLength;		// length of key, in bytes
  int encVersion;		// encryption version
  CryptAlgorithm encAlgorithm;	// encryption algorithm
  XRefCacheEntry		// cache of recently accessed objects
    cache[xrefCacheSize];
#if MULTITHREADED
  GMutex cacheMutex;
#endif

  GFileOffset getStartXref();
  GBool readXRef(GFileOffset *pos, XRefPosSet *posSet);
  GBool readXRefTable(GFileOffset *pos, int offset, XRefPosSet *posSet);
  GBool readXRefStreamSection(Stream *xrefStr, int *w, int first, int n);
  GBool readXRefStream(Stream *xrefStr, GFileOffset *pos);
  GBool constructXRef();
  GBool getObjectStreamObject(int objStrNum, int objIdx,
			      int objNum, Object *obj);
  ObjectStream *getObjectStream(int objStrNum);
  void cleanObjectStreamCache();
  GFileOffset strToFileOffset(char *s);
};

#endif
