//========================================================================
//
// pdfinfo.cc
//
// Copyright 1998-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "gfile.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "TextString.h"
#include "UTF8.h"
#include "Zoox.h"
#include "Error.h"
#include "config.h"

static void printInfoString(Object *infoDict, const char *infoKey,
			    ZxDoc *xmp, const char *xmpKey1,
			    const char *xmpKey2,
			    const char *text, GBool parseDate,
			    UnicodeMap *uMap);
static GString *parseInfoDate(GString *s);
static GString *parseXMPDate(GString *s);
static void printBox(const char *text, PDFRectangle *box);

static int firstPage = 1;
static int lastPage = 0;
static GBool printBoxes = gFalse;
static GBool printMetadata = gFalse;
static GBool rawDates = gFalse;
static char textEncName[128] = "";
static char ownerPassword[33] = "\001";
static char userPassword[33] = "\001";
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,        0,
   "first page to convert"},
  {"-l",      argInt,      &lastPage,         0,
   "last page to convert"},
  {"-box",    argFlag,     &printBoxes,       0,
   "print the page bounding boxes"},
  {"-meta",   argFlag,     &printMetadata,    0,
   "print the document metadata (XML)"},
  {"-rawdates", argFlag,   &rawDates,         0,
   "print the undecoded date strings directly from the PDF file"},
  {"-enc",    argString,   textEncName,    sizeof(textEncName),
   "output text encoding name"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-cfg",        argString,      cfgFileName,    sizeof(cfgFileName),
   "configuration file to use in place of .xpdfrc"},
  {"-v",      argFlag,     &printVersion,  0,
   "print copyright and version info"},
  {"-h",      argFlag,     &printHelp,     0,
   "print usage information"},
  {"-help",   argFlag,     &printHelp,     0,
   "print usage information"},
  {"--help",  argFlag,     &printHelp,     0,
   "print usage information"},
  {"-?",      argFlag,     &printHelp,     0,
   "print usage information"},
  {NULL}
};

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  char *fileName;
  GString *ownerPW, *userPW;
  UnicodeMap *uMap;
  Page *page;
  Object info, xfa;
  Object *acroForm;
  char buf[256];
  double w, h, wISO, hISO;
  FILE *f;
  GString *metadata;
  ZxDoc *xmp;
  GBool ok;
  int exitCode;
  int pg, i;
  GBool multiPage;

  exitCode = 99;

  // parse args
  fixCommandLine(&argc, &argv);
  ok = parseArgs(argDesc, &argc, argv);
  if (!ok || argc != 2 || printVersion || printHelp) {
    fprintf(stderr, "pdfinfo version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdfinfo", "<PDF-file>", argDesc);
    }
    goto err0;
  }
  fileName = argv[1];

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
  }

  // get mapping to output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    error(errConfig, -1, "Couldn't get text encoding");
    goto err1;
  }

  // open PDF file
  if (ownerPassword[0] != '\001') {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0] != '\001') {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err2;
  }

  // get page range
  if (firstPage < 1) {
    firstPage = 1;
  }
  if (lastPage == 0) {
    multiPage = gFalse;
    lastPage = 1;
  } else {
    multiPage = gTrue;
  }
  if (lastPage < 1 || lastPage > doc->getNumPages()) {
    lastPage = doc->getNumPages();
  }

  // print doc info
  doc->getDocInfo(&info);
  if ((metadata = doc->readMetadata())) {
    xmp = ZxDoc::loadMem(metadata->getCString(), metadata->getLength());
  } else {
    xmp = NULL;
  }
  printInfoString(&info, "Title",        xmp, "dc:title",        NULL,              "Title:          ", gFalse,    uMap);
  printInfoString(&info, "Subject",      xmp, "dc:description",  NULL,              "Subject:        ", gFalse,    uMap);
  printInfoString(&info, "Keywords",     xmp, "pdf:Keywords",    NULL,              "Keywords:       ", gFalse,    uMap);
  printInfoString(&info, "Author",       xmp, "dc:creator",      NULL,              "Author:         ", gFalse,    uMap);
  printInfoString(&info, "Creator",      xmp, "xmp:CreatorTool", NULL,              "Creator:        ", gFalse,    uMap);
  printInfoString(&info, "Producer",     xmp, "pdf:Producer",    NULL,              "Producer:       ", gFalse,    uMap);
  printInfoString(&info, "CreationDate", xmp, "xap:CreateDate",  "xmp:CreateDate",  "CreationDate:   ", !rawDates, uMap);
  printInfoString(&info, "ModDate",      xmp, "xap:ModifyDate",  "xmp:ModifyDate",  "ModDate:        ", !rawDates, uMap);
  info.free();
  if (xmp) {
    delete xmp;
  }

  // print tagging info
  printf("Tagged:         %s\n",
	 doc->getStructTreeRoot()->isDict() ? "yes" : "no");

  // print form info
  if ((acroForm = doc->getCatalog()->getAcroForm())->isDict()) {
    acroForm->dictLookup("XFA", &xfa);
    if (xfa.isStream() || xfa.isArray()) {
      if (doc->getCatalog()->getNeedsRendering()) {
	printf("Form:           dynamic XFA\n");
      } else {
	printf("Form:           static XFA\n");
      }
    } else {
      printf("Form:           AcroForm\n");
    }
    xfa.free();
  } else {
    printf("Form:           none\n");
  }

  // print page count
  printf("Pages:          %d\n", doc->getNumPages());

  // print encryption info
  printf("Encrypted:      ");
  if (doc->isEncrypted()) {
    printf("yes (print:%s copy:%s change:%s addNotes:%s)\n",
	   doc->okToPrint(gTrue) ? "yes" : "no",
	   doc->okToCopy(gTrue) ? "yes" : "no",
	   doc->okToChange(gTrue) ? "yes" : "no",
	   doc->okToAddNotes(gTrue) ? "yes" : "no");
  } else {
    printf("no\n");
  }

  // print page size
  for (pg = firstPage; pg <= lastPage; ++pg) {
    w = doc->getPageCropWidth(pg);
    h = doc->getPageCropHeight(pg);
    if (multiPage) {
      printf("Page %4d size: %g x %g pts", pg, w, h);
    } else {
      printf("Page size:      %g x %g pts", w, h);
    }
    if ((fabs(w - 612) < 0.1 && fabs(h - 792) < 0.1) ||
	(fabs(w - 792) < 0.1 && fabs(h - 612) < 0.1)) {
      printf(" (letter)");
    } else {
      hISO = sqrt(sqrt(2.0)) * 7200 / 2.54;
      wISO = hISO / sqrt(2.0);
      for (i = 0; i <= 6; ++i) {
	if ((fabs(w - wISO) < 1 && fabs(h - hISO) < 1) ||
	    (fabs(w - hISO) < 1 && fabs(h - wISO) < 1)) {
	  printf(" (A%d)", i);
	  break;
	}
	hISO = wISO;
	wISO /= sqrt(2.0);
      }
    }
    printf(" (rotated %d degrees)", doc->getPageRotate(pg));
    printf("\n");
  } 

  // print the boxes
  if (printBoxes) {
    if (multiPage) {
      for (pg = firstPage; pg <= lastPage; ++pg) {
	page = doc->getCatalog()->getPage(pg);
	sprintf(buf, "Page %4d MediaBox: ", pg);
	printBox(buf, page->getMediaBox());
	sprintf(buf, "Page %4d CropBox:  ", pg);
	printBox(buf, page->getCropBox());
	sprintf(buf, "Page %4d BleedBox: ", pg);
	printBox(buf, page->getBleedBox());
	sprintf(buf, "Page %4d TrimBox:  ", pg);
	printBox(buf, page->getTrimBox());
	sprintf(buf, "Page %4d ArtBox:   ", pg);
	printBox(buf, page->getArtBox());
      }
    } else {
      page = doc->getCatalog()->getPage(firstPage);
      printBox("MediaBox:       ", page->getMediaBox());
      printBox("CropBox:        ", page->getCropBox());
      printBox("BleedBox:       ", page->getBleedBox());
      printBox("TrimBox:        ", page->getTrimBox());
      printBox("ArtBox:         ", page->getArtBox());
    }
  }

  // print file size
  f = openFile(fileName, "rb");
  if (f) {
    gfseek(f, 0, SEEK_END);
    printf("File size:      %u bytes\n", (Guint)gftell(f));
    fclose(f);
  }

  // print linearization info
  printf("Optimized:      %s\n", doc->isLinearized() ? "yes" : "no");

  // print PDF version
  printf("PDF version:    %.1f\n", doc->getPDFVersion());

  // print the metadata
  if (printMetadata && metadata) {
    fputs("Metadata:\n", stdout);
    fputs(metadata->getCString(), stdout);
    fputc('\n', stdout);
  }

  if (metadata) {
    delete metadata;
  }

  exitCode = 0;

  // clean up
 err2:
  uMap->decRefCnt();
  delete doc;
 err1:
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}

static void printInfoString(Object *infoDict, const char *infoKey,
			    ZxDoc *xmp, const char *xmpKey1,
			    const char *xmpKey2,
			    const char *text, GBool parseDate,
			    UnicodeMap *uMap) {
  Object obj;
  TextString *s;
  Unicode *u;
  Unicode uu;
  char buf[8];
  GString *value, *tmp;
  ZxElement *rdf, *elem, *child;
  ZxNode *node, *node2;
  int i, n;

  value = NULL;

  //-- check the info dictionary
  if (infoDict->isDict()) {
    if (infoDict->dictLookup(infoKey, &obj)->isString()) {
      if (!parseDate || !(value = parseInfoDate(obj.getString()))) {
	s = new TextString(obj.getString());
	u = s->getUnicode();
	value = new GString();
	for (i = 0; i < s->getLength(); ++i) {
	  n = uMap->mapUnicode(u[i], buf, sizeof(buf));
	  value->append(buf, n);
	}
	delete s;
      }
    }
    obj.free();
  }

  //-- check the XMP metadata
  if (xmp) {
    rdf = xmp->getRoot();
    if (rdf->isElement("x:xmpmeta")) {
      rdf = rdf->findFirstChildElement("rdf:RDF");
    }
    if (rdf && rdf->isElement("rdf:RDF")) {
      for (node = rdf->getFirstChild(); node; node = node->getNextChild()) {
	if (node->isElement("rdf:Description")) {
	  if (!(elem = node->findFirstChildElement(xmpKey1)) && xmpKey2) {
	    elem = node->findFirstChildElement(xmpKey2);
	  }
	  if (elem) {
	    if ((child = elem->findFirstChildElement("rdf:Alt")) ||
		(child = elem->findFirstChildElement("rdf:Seq"))) {
	      if ((node2 = child->findFirstChildElement("rdf:li"))) {
		node2 = node2->getFirstChild();
	      }
	    } else {
	      node2 = elem->getFirstChild();
	    }
	    if (node2 && node2->isCharData()) {
	      if (value) {
		delete value;
	      }
	      if (!parseDate ||
		  !(value = parseXMPDate(((ZxCharData *)node2)->getData()))) {
		tmp = ((ZxCharData *)node2)->getData();
		int i = 0;
		value = new GString();
		while (getUTF8(tmp, &i, &uu)) {
		  n = uMap->mapUnicode(uu, buf, sizeof(buf));
		  value->append(buf, n);
		}
	      }
	    }
	    break;
	  }
	}
      }
    }
  }

  if (value) {
    fputs(text, stdout);
    fwrite(value->getCString(), 1, value->getLength(), stdout);
    fputc('\n', stdout);
    delete value;
  }
}

static GString *parseInfoDate(GString *s) {
  int year, mon, day, hour, min, sec, n;
  struct tm tmStruct;
  char buf[256];
  char *p;

  p = s->getCString();
  if (p[0] == 'D' && p[1] == ':') {
    p += 2;
  }
  if ((n = sscanf(p, "%4d%2d%2d%2d%2d%2d",
		  &year, &mon, &day, &hour, &min, &sec)) < 1) {
    return NULL;
  }
  switch (n) {
  case 1: mon = 1;
  case 2: day = 1;
  case 3: hour = 0;
  case 4: min = 0;
  case 5: sec = 0;
  }
  tmStruct.tm_year = year - 1900;
  tmStruct.tm_mon = mon - 1;
  tmStruct.tm_mday = day;
  tmStruct.tm_hour = hour;
  tmStruct.tm_min = min;
  tmStruct.tm_sec = sec;
  tmStruct.tm_wday = -1;
  tmStruct.tm_yday = -1;
  tmStruct.tm_isdst = -1;
  // compute the tm_wday and tm_yday fields
  if (!(mktime(&tmStruct) != (time_t)-1 &&
	strftime(buf, sizeof(buf), "%c", &tmStruct))) {
    return NULL;
  }
  return new GString(buf);
}

static GString *parseXMPDate(GString *s) {
  int year, mon, day, hour, min, sec, tz;
  struct tm tmStruct;
  char buf[256];
  char *p;

  p = s->getCString();
  if (isdigit(p[0]) && isdigit(p[1]) && isdigit(p[2]) && isdigit(p[3])) {
    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = p[2];
    buf[3] = p[3];
    buf[4] = '\0';
    year = atoi(buf);
    p += 4;
  } else {
    return NULL;
  }
  mon = day = 1;
  hour = min = sec = 0;
  tz = 2000;
  if (p[0] == '-' && isdigit(p[1]) && isdigit(p[2])) {
    buf[0] = p[1];
    buf[1] = p[2];
    buf[2] = '\0';
    mon = atoi(buf);
    p += 3;
    if (p[0] == '-' && isdigit(p[1]) && isdigit(p[2])) {
      buf[0] = p[1];
      buf[1] = p[2];
      buf[2] = '\0';
      day = atoi(buf);
      p += 3;
      if (p[0] == 'T' && isdigit(p[1]) && isdigit(p[2]) &&
	  p[3] == ':' && isdigit(p[4]) && isdigit(p[5])) {
	buf[0] = p[1];
	buf[1] = p[2];
	buf[2] = '\0';
	hour = atoi(buf);
	buf[0] = p[4];
	buf[1] = p[5];
	buf[2] = '\0';
	min = atoi(buf);
	p += 6;
	if (p[0] == ':' && isdigit(p[1]) && isdigit(p[2])) {
	  buf[0] = p[1];
	  buf[1] = p[2];
	  buf[2] = '\0';
	  sec = atoi(buf);
	  if (p[0] == '.' && isdigit(p[1])) {
	    p += 2;
	  }
	}
	if ((p[0] == '+' || p[0] == '-') &&
	    isdigit(p[1]) && isdigit(p[2]) && p[3] == ':' &&
	    isdigit(p[4]) && isdigit(p[5])) {
	  buf[0] = p[1];
	  buf[1] = p[2];
	  buf[2] = '\0';
	  tz = atoi(buf);
	  buf[0] = p[4];
	  buf[1] = p[5];
	  buf[2] = '\0';
	  tz = tz * 60 + atoi(buf);
	  tz = tz * 60;
	  if (p[0] == '-') {
	    tz = -tz;
	  }
	}
      }
    }
  }

  tmStruct.tm_year = year - 1900;
  tmStruct.tm_mon = mon - 1;
  tmStruct.tm_mday = day;
  tmStruct.tm_hour = hour;
  tmStruct.tm_min = min;
  tmStruct.tm_sec = sec;
  tmStruct.tm_wday = -1;
  tmStruct.tm_yday = -1;
  tmStruct.tm_isdst = -1;
  // compute the tm_wday and tm_yday fields
  //~ this ignores the timezone
  if (!(mktime(&tmStruct) != (time_t)-1 &&
	strftime(buf, sizeof(buf), "%c", &tmStruct))) {
    return NULL;
  }
  return new GString(buf);
}

static void printBox(const char *text, PDFRectangle *box) {
  printf("%s%8.2f %8.2f %8.2f %8.2f\n",
	 text, box->x1, box->y1, box->x2, box->y2);
}
