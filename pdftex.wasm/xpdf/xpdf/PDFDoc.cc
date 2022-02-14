//========================================================================
//
// PDFDoc.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#ifdef _WIN32
#  include <windows.h>
#endif
#include "gmempp.h"
#include "GString.h"
#include "config.h"
#include "GlobalParams.h"
#include "Page.h"
#include "Catalog.h"
#include "Stream.h"
#include "XRef.h"
#include "Link.h"
#include "OutputDev.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "Lexer.h"
#include "Parser.h"
#include "SecurityHandler.h"
#include "UTF8.h"
#ifndef DISABLE_OUTLINE
#include "Outline.h"
#endif
#include "OptionalContent.h"
#include "PDFDoc.h"

//------------------------------------------------------------------------

#define headerSearchSize 1024	// read this many bytes at beginning of
				//   file to look for '%PDF'

// Avoid sharing files with child processes on Windows, where sharing
// can cause problems.
#ifdef _WIN32
#  define fopenReadMode "rbN"
#  define wfopenReadMode L"rbN"
#else
#  define fopenReadMode "rb"
#endif

//------------------------------------------------------------------------
// PDFDoc
//------------------------------------------------------------------------

PDFDoc::PDFDoc(GString *fileNameA, GString *ownerPassword,
	       GString *userPassword, PDFCore *coreA) {
  Object obj;
  GString *fileName1, *fileName2;
#ifdef _WIN32
  int n, i;
#endif

  init(coreA);

  fileName = fileNameA;
#ifdef _WIN32
  n = fileName->getLength();
  fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
  for (i = 0; i < n; ++i) {
    fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
  }
  fileNameU[n] = L'\0';
#endif

  fileName1 = fileName;

  // try to open file
  fileName2 = NULL;
#ifdef VMS
  if (!(file = fopen(fileName1->getCString(), fopenReadMode, "ctx=stm"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", fileName1);
    errCode = errOpenFile;
    return;
  }
#else
  if (!(file = fopen(fileName1->getCString(), fopenReadMode))) {
    fileName2 = fileName->copy();
    fileName2->lowerCase();
    if (!(file = fopen(fileName2->getCString(), fopenReadMode))) {
      fileName2->upperCase();
      if (!(file = fopen(fileName2->getCString(), fopenReadMode))) {
	error(errIO, -1, "Couldn't open file '{0:t}'", fileName);
	delete fileName2;
	errCode = errOpenFile;
	return;
      }
    }
    delete fileName2;
  }
#endif

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  ok = setup(ownerPassword, userPassword);
}

#ifdef _WIN32
PDFDoc::PDFDoc(wchar_t *fileNameA, int fileNameLen, GString *ownerPassword,
	       GString *userPassword, PDFCore *coreA) {
  OSVERSIONINFO version;
  Object obj;
  int i;

  init(coreA);

  // save both Unicode and 8-bit copies of the file name
  fileName = new GString();
  fileNameU = (wchar_t *)gmallocn(fileNameLen + 1, sizeof(wchar_t));
  for (i = 0; i < fileNameLen; ++i) {
    fileName->append((char)fileNameA[i]);
    fileNameU[i] = fileNameA[i];
  }
  fileNameU[fileNameLen] = L'\0';

  // try to open file
  // NB: _wfopen is only available in NT
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    file = _wfopen(fileNameU, wfopenReadMode);
  } else {
    file = fopen(fileName->getCString(), fopenReadMode);
  }
  if (!file) {
    error(errIO, -1, "Couldn't open file '{0:t}'", fileName);
    errCode = errOpenFile;
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  ok = setup(ownerPassword, userPassword);
}
#endif

PDFDoc::PDFDoc(char *fileNameA, GString *ownerPassword,
	       GString *userPassword, PDFCore *coreA) {
/*
#ifdef _WIN32
  OSVERSIONINFO version;
#endif
*/
  Object obj;
/*
#ifdef _WIN32
  Unicode u;
  int n, i, j;
#endif
*/

  init(coreA);

  fileName = new GString(fileNameA);

#if defined(_WIN32)
#if 0
  n = 0;
  i = 0;
  while (getUTF8(fileName, &i, &u)) {
    ++n;
  }
  fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
  i = j = 0;
  while (j < n && getUTF8(fileName, &i, &u)) {
    fileNameU[j++] = (wchar_t)u;
  }
  fileNameU[n] = L'\0';
  // NB: _wfopen is only available in NT
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    file = _wfopen(fileNameU, wfopenReadMode);
  } else {
#endif /* 0 */
    file = fopen(fileName->getCString(), fopenReadMode);
#if 0
  }
#endif /* 0 */
#elif defined(VMS)
  file = fopen(fileName->getCString(), fopenReadMode, "ctx=stm");
#else
  file = fopen(fileName->getCString(), fopenReadMode);
#endif

  if (!file) {
    error(errIO, -1, "Couldn't open file '{0:t}'", fileName);
    errCode = errOpenFile;
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, 0, &obj);

  ok = setup(ownerPassword, userPassword);
}

PDFDoc::PDFDoc(BaseStream *strA, GString *ownerPassword,
	       GString *userPassword, PDFCore *coreA) {
#ifdef _WIN32
  int n, i;
#endif

  init(coreA);

  if (strA->getFileName()) {
    fileName = strA->getFileName()->copy();
#ifdef _WIN32
    n = fileName->getLength();
    fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
    for (i = 0; i < n; ++i) {
      fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
    }
    fileNameU[n] = L'\0';
#endif
  } else {
    fileName = NULL;
#ifdef _WIN32
    fileNameU = NULL;
#endif
  }
  str = strA;
  ok = setup(ownerPassword, userPassword);
}

void PDFDoc::init(PDFCore *coreA) {
  ok = gFalse;
  errCode = errNone;
  core = coreA;
  file = NULL;
  str = NULL;
  xref = NULL;
  catalog = NULL;
#ifndef DISABLE_OUTLINE
  outline = NULL;
#endif
  optContent = NULL;
}

GBool PDFDoc::setup(GString *ownerPassword, GString *userPassword) {

  str->reset();

  // check header
  checkHeader();

  // read the xref and catalog
  if (!PDFDoc::setup2(ownerPassword, userPassword, gFalse)) {
    if (errCode == errDamaged || errCode == errBadCatalog) {
      // try repairing the xref table
      error(errSyntaxWarning, -1,
	    "PDF file is damaged - attempting to reconstruct xref table...");
      if (!PDFDoc::setup2(ownerPassword, userPassword, gTrue)) {
	return gFalse;
      }
    } else {
      return gFalse;
    }
  }

#ifndef DISABLE_OUTLINE
  // read outline
  outline = new Outline(catalog->getOutline(), xref);
#endif

  // read the optional content info
  optContent = new OptionalContent(this);


  // done
  return gTrue;
}

GBool PDFDoc::setup2(GString *ownerPassword, GString *userPassword,
		     GBool repairXRef) {
  // read xref table
  xref = new XRef(str, repairXRef);
  if (!xref->isOk()) {
    error(errSyntaxError, -1, "Couldn't read xref table");
    errCode = xref->getErrorCode();
    delete xref;
    xref = NULL;
    return gFalse;
  }

  // check for encryption
  if (!checkEncryption(ownerPassword, userPassword)) {
    errCode = errEncrypted;
    delete xref;
    xref = NULL;
    return gFalse;
  }

  // read catalog
  catalog = new Catalog(this);
  if (!catalog->isOk()) {
    error(errSyntaxError, -1, "Couldn't read page catalog");
    errCode = errBadCatalog;
    delete catalog;
    catalog = NULL;
    delete xref;
    xref = NULL;
    return gFalse;
  }

  return gTrue;
}

PDFDoc::~PDFDoc() {
  if (optContent) {
    delete optContent;
  }
#ifndef DISABLE_OUTLINE
  if (outline) {
    delete outline;
  }
#endif
  if (catalog) {
    delete catalog;
  }
  if (xref) {
    delete xref;
  }
  if (str) {
    delete str;
  }
  if (file) {
    fclose(file);
  }
  if (fileName) {
    delete fileName;
  }
#ifdef _WIN32
  if (fileNameU) {
    gfree(fileNameU);
  }
#endif
}

// Check for a PDF header on this stream.  Skip past some garbage
// if necessary.
void PDFDoc::checkHeader() {
  char hdrBuf[headerSearchSize+1];
  char *p;
  int i;

  pdfVersion = 0;
  memset(hdrBuf, 0, headerSearchSize + 1);
  str->getBlock(hdrBuf, headerSearchSize);
  for (i = 0; i < headerSearchSize - 5; ++i) {
    if (!strncmp(&hdrBuf[i], "%PDF-", 5)) {
      break;
    }
  }
  if (i >= headerSearchSize - 5) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  str->moveStart(i);
  if (!(p = strtok(&hdrBuf[i+5], " \t\n\r"))) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  pdfVersion = atof(p);
  if (!(hdrBuf[i+5] >= '0' && hdrBuf[i+5] <= '9') ||
      pdfVersion > supportedPDFVersionNum + 0.0001) {
    error(errSyntaxWarning, -1,
	  "PDF version {0:s} -- xpdf supports version {1:s} (continuing anyway)",
	  p, supportedPDFVersionStr);
  }
}

GBool PDFDoc::checkEncryption(GString *ownerPassword, GString *userPassword) {
  Object encrypt;
  GBool encrypted;
  SecurityHandler *secHdlr;
  GBool ret;

  xref->getTrailerDict()->dictLookup("Encrypt", &encrypt);
  if ((encrypted = encrypt.isDict())) {
    if ((secHdlr = SecurityHandler::make(this, &encrypt))) {
      if (secHdlr->isUnencrypted()) {
	// no encryption
	ret = gTrue;
      } else if (secHdlr->checkEncryption(ownerPassword, userPassword)) {
	// authorization succeeded
       	xref->setEncryption(secHdlr->getPermissionFlags(),
			    secHdlr->getOwnerPasswordOk(),
			    secHdlr->getFileKey(),
			    secHdlr->getFileKeyLength(),
			    secHdlr->getEncVersion(),
			    secHdlr->getEncAlgorithm());
	ret = gTrue;
      } else {
	// authorization failed
	ret = gFalse;
      }
      delete secHdlr;
    } else {
      // couldn't find the matching security handler
      ret = gFalse;
    }
  } else {
    // document is not encrypted
    ret = gTrue;
  }
  encrypt.free();
  return ret;
}

void PDFDoc::displayPage(OutputDev *out, int page,
			 double hDPI, double vDPI, int rotate,
			 GBool useMediaBox, GBool crop, GBool printing,
			 GBool (*abortCheckCbk)(void *data),
			 void *abortCheckCbkData) {
  if (globalParams->getPrintCommands()) {
    printf("***** page %d *****\n", page);
  }
  catalog->getPage(page)->display(out, hDPI, vDPI,
				  rotate, useMediaBox, crop, printing,
				  abortCheckCbk, abortCheckCbkData);
}

void PDFDoc::displayPages(OutputDev *out, int firstPage, int lastPage,
			  double hDPI, double vDPI, int rotate,
			  GBool useMediaBox, GBool crop, GBool printing,
			  GBool (*abortCheckCbk)(void *data),
			  void *abortCheckCbkData) {
  int page;

  for (page = firstPage; page <= lastPage; ++page) {
    displayPage(out, page, hDPI, vDPI, rotate, useMediaBox, crop, printing,
		abortCheckCbk, abortCheckCbkData);
    catalog->doneWithPage(page);
  }
}

void PDFDoc::displayPageSlice(OutputDev *out, int page,
			      double hDPI, double vDPI, int rotate,
			      GBool useMediaBox, GBool crop, GBool printing,
			      int sliceX, int sliceY, int sliceW, int sliceH,
			      GBool (*abortCheckCbk)(void *data),
			      void *abortCheckCbkData) {
  catalog->getPage(page)->displaySlice(out, hDPI, vDPI,
				       rotate, useMediaBox, crop,
				       sliceX, sliceY, sliceW, sliceH,
				       printing,
				       abortCheckCbk, abortCheckCbkData);
}

Links *PDFDoc::getLinks(int page) {
  return catalog->getPage(page)->getLinks();
}

void PDFDoc::processLinks(OutputDev *out, int page) {
  catalog->getPage(page)->processLinks(out);
}

#ifndef DISABLE_OUTLINE
int PDFDoc::getOutlineTargetPage(OutlineItem *outlineItem) {
  LinkAction *action;
  LinkActionKind kind;
  LinkDest *dest;
  GString *namedDest;
  Ref pageRef;
  int pg;

  if (outlineItem->pageNum >= 0) {
    return outlineItem->pageNum;
  }
  if (!(action = outlineItem->getAction())) {
    outlineItem->pageNum = 0;
    return 0;
  }
  kind = action->getKind();
  if (kind != actionGoTo) {
    outlineItem->pageNum = 0;
    return 0;
  }
  if ((dest = ((LinkGoTo *)action)->getDest())) {
    dest = dest->copy();
  } else if ((namedDest = ((LinkGoTo *)action)->getNamedDest())) {
    dest = findDest(namedDest);
  }
  pg = 0;
  if (dest) {
    if (dest->isPageRef()) {
      pageRef = dest->getPageRef();
      pg = findPage(pageRef.num, pageRef.gen);
    } else {
      pg = dest->getPageNum();
    }
    delete dest;
  }
  outlineItem->pageNum = pg;
  return pg;
}
#endif

GBool PDFDoc::isLinearized() {
  Parser *parser;
  Object obj1, obj2, obj3, obj4, obj5;
  GBool lin;

  lin = gFalse;
  obj1.initNull();
  parser = new Parser(xref,
	     new Lexer(xref,
	       str->makeSubStream(str->getStart(), gFalse, 0, &obj1)),
	     gTrue);
  parser->getObj(&obj1);
  parser->getObj(&obj2);
  parser->getObj(&obj3);
  parser->getObj(&obj4);
  if (obj1.isInt() && obj2.isInt() && obj3.isCmd("obj") &&
      obj4.isDict()) {
    obj4.dictLookup("Linearized", &obj5);
    if (obj5.isNum() && obj5.getNum() > 0) {
      lin = gTrue;
    }
    obj5.free();
  }
  obj4.free();
  obj3.free();
  obj2.free();
  obj1.free();
  delete parser;
  return lin;
}

GBool PDFDoc::saveAs(GString *name) {
  FILE *f;
  char buf[4096];
  int n;

  if (!(f = fopen(name->getCString(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return gFalse;
  }
  str->reset();
  while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
    fwrite(buf, 1, n, f);
  }
  str->close();
  fclose(f);
  return gTrue;
}

GBool PDFDoc::saveEmbeddedFile(int idx, const char *path) {
  FILE *f;
  GBool ret;

  if (!(f = fopen(path, "wb"))) {
    return gFalse;
  }
  ret = saveEmbeddedFile2(idx, f);
  fclose(f);
  return ret;
}

#ifdef _WIN32
GBool PDFDoc::saveEmbeddedFile(int idx, const wchar_t *path, int pathLen) {
  FILE *f;
  OSVERSIONINFO version;
  wchar_t path2w[_MAX_PATH + 1];
  char path2c[_MAX_PATH + 1];
  int i;
  GBool ret;

  // NB: _wfopen is only available in NT
/*
  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    for (i = 0; i < pathLen && i < _MAX_PATH; ++i) {
      path2w[i] = path[i];
    }
    path2w[i] = 0;
    f = _wfopen(path2w, L"wb");
  } else {
*/
    for (i = 0; i < pathLen && i < _MAX_PATH; ++i) {
      path2c[i] = (char)path[i];
    }
    path2c[i] = 0;
    f = fopen(path2c, "wb");
/*
  }
*/
  if (!f) {
    return gFalse;
  }
  ret = saveEmbeddedFile2(idx, f);
  fclose(f);
  return ret;
}
#endif

GBool PDFDoc::saveEmbeddedFile2(int idx, FILE *f) {
  Object strObj;
  char buf[4096];
  int n;

  if (!catalog->getEmbeddedFileStreamObj(idx, &strObj)) {
    return gFalse;
  }
  strObj.streamReset();
  while ((n = strObj.streamGetBlock(buf, sizeof(buf))) > 0) {
    fwrite(buf, 1, n, f);
  }
  strObj.streamClose();
  strObj.free();
  return gTrue;
}

char *PDFDoc::getEmbeddedFileMem(int idx, int *size) {
  Object strObj;
  char *buf;
  int bufSize, sizeInc, n;

  if (!catalog->getEmbeddedFileStreamObj(idx, &strObj)) {
    return NULL;
  }
  strObj.streamReset();
  bufSize = 0;
  buf = NULL;
  do {
    sizeInc = bufSize ? bufSize : 1024;
    if (bufSize > INT_MAX - sizeInc) {
      error(errIO, -1, "embedded file is too large");
      *size = 0;
      return NULL;
    }
    buf = (char *)grealloc(buf, bufSize + sizeInc);
    n = strObj.streamGetBlock(buf + bufSize, sizeInc);
    bufSize += n;
  } while (n == sizeInc);
  strObj.streamClose();
  strObj.free();
  *size = bufSize;
  return buf;
}

