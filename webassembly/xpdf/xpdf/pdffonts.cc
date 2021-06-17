//========================================================================
//
// pdffonts.cc
//
// Copyright 2001-2007 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Error.h"
#include "Object.h"
#include "Dict.h"
#include "GfxFont.h"
#include "Annot.h"
#include "Form.h"
#include "PDFDoc.h"
#include "config.h"

// NB: this must match the definition of GfxFontType in GfxFont.h.
static const char *fontTypeNames[] = {
  "unknown",
  "Type 1",
  "Type 1C",
  "Type 1C (OT)",
  "Type 3",
  "TrueType",
  "TrueType (OT)",
  "CID Type 0",
  "CID Type 0C",
  "CID Type 0C (OT)",
  "CID TrueType",
  "CID TrueType (OT)"
};

static void scanFonts(Object *obj, PDFDoc *doc);
static void scanFonts(Dict *resDict, PDFDoc *doc);
static void scanFont(GfxFont *font, PDFDoc *doc);
static GBool checkObject(Object *in, Object *out);

static int firstPage = 1;
static int lastPage = 0;
static GBool showFontLoc = gFalse;
static GBool showFontLocPS = gFalse;
static char ownerPassword[33] = "\001";
static char userPassword[33] = "\001";
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to examine"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to examine"},
  {"-loc",    argFlag,     &showFontLoc,   0,
   "print extended info on font location"},
  {"-locPS",  argFlag,     &showFontLocPS, 0,
   "print extended info on font location for PostScript conversion"},
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

static PDFDoc *doc;

static Ref *fonts;
static int fontsLen;
static int fontsSize;

static char *seenObjs;
static int numObjects;

int main(int argc, char *argv[]) {
  char *fileName;
  GString *ownerPW, *userPW;
  GBool ok;
  Page *page;
  Dict *resDict;
  Annots *annots;
  Form *form;
  Object obj1, obj2;
  int pg, i, j;
  int exitCode;

  exitCode = 99;

  // parse args
  fixCommandLine(&argc, &argv);
  ok = parseArgs(argDesc, &argc, argv);
  if (!ok || argc != 2 || printVersion || printHelp) {
    fprintf(stderr, "pdffonts version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdffonts", "<PDF-file>", argDesc);
    }
    goto err0;
  }
  fileName = argv[1];

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  globalParams->setupBaseFonts(NULL);

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
    goto err1;
  }

  // get page range
  if (firstPage < 1) {
    firstPage = 1;
  }
  if (lastPage < 1 || lastPage > doc->getNumPages()) {
    lastPage = doc->getNumPages();
  }

  // scan the fonts
  if (showFontLoc || showFontLocPS) {
    printf("name                                           type              emb sub uni prob object ID location\n");
    printf("---------------------------------------------- ----------------- --- --- --- ---- --------- --------\n");
  } else {
    printf("name                                           type              emb sub uni prob object ID\n");
    printf("---------------------------------------------- ----------------- --- --- --- ---- ---------\n");
  }
  fonts = NULL;
  fontsLen = fontsSize = 0;
  numObjects = doc->getXRef()->getNumObjects();
  seenObjs = (char *)gmalloc(numObjects);
  memset(seenObjs, 0, numObjects);
  for (pg = firstPage; pg <= lastPage; ++pg) {
    page = doc->getCatalog()->getPage(pg);
    if ((resDict = page->getResourceDict())) {
      scanFonts(resDict, doc);
    }
    annots = new Annots(doc, page->getAnnots(&obj1));
    obj1.free();
    for (i = 0; i < annots->getNumAnnots(); ++i) {
      if (annots->getAnnot(i)->getAppearance(&obj1)->isStream()) {
	obj1.streamGetDict()->lookupNF("Resources", &obj2);
	scanFonts(&obj2, doc);
	obj2.free();
      }
      obj1.free();
    }
    delete annots;
  }
  if ((form = doc->getCatalog()->getForm())) {
    for (i = 0; i < form->getNumFields(); ++i) {
      form->getField(i)->getResources(&obj1);
      if (obj1.isArray()) {
	for (j = 0; j < obj1.arrayGetLength(); ++j) {
	  obj1.arrayGetNF(j, &obj2);
	  scanFonts(&obj2, doc);
	  obj2.free();
	}
      } else if (obj1.isDict()) {
	scanFonts(obj1.getDict(), doc);
      }
      obj1.free();
    }
  }

  exitCode = 0;

  // clean up
  gfree(fonts);
  gfree(seenObjs);
 err1:
  delete doc;
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}

static void scanFonts(Object *obj, PDFDoc *doc) {
  Object obj2;

  if (checkObject(obj, &obj2) && obj2.isDict()) {
    scanFonts(obj2.getDict(), doc);
  }
  obj2.free();
}

static void scanFonts(Dict *resDict, PDFDoc *doc) {
  Object fontDict1, fontDict2, xObjDict1, xObjDict2, xObj1, xObj2;
  Object patternDict1, patternDict2, pattern1, pattern2;
  Object gsDict1, gsDict2, gs1, gs2, smask1, smask2, smaskGroup1, smaskGroup2;
  Object resObj;
  Ref r;
  GfxFontDict *gfxFontDict;
  GfxFont *font;
  int i;

  // scan the fonts in this resource dictionary
  gfxFontDict = NULL;
  resDict->lookupNF("Font", &fontDict1);
  if (checkObject(&fontDict1, &fontDict2) && fontDict2.isDict()) {
    if (fontDict1.isRef()) {
      r = fontDict1.getRef();
      gfxFontDict = new GfxFontDict(doc->getXRef(), &r, fontDict2.getDict());
    } else {
      gfxFontDict = new GfxFontDict(doc->getXRef(), NULL, fontDict2.getDict());
    }
    if (gfxFontDict) {
      for (i = 0; i < gfxFontDict->getNumFonts(); ++i) {
	if ((font = gfxFontDict->getFont(i))) {
	  scanFont(font, doc);
	}
      }
      delete gfxFontDict;
    }
  }
  fontDict2.free();
  fontDict1.free();

  // recursively scan any resource dictionaries in XObjects in this
  // resource dictionary
  resDict->lookupNF("XObject", &xObjDict1);
  if (checkObject(&xObjDict1, &xObjDict2) && xObjDict2.isDict()) {
    for (i = 0; i < xObjDict2.dictGetLength(); ++i) {
      xObjDict2.dictGetValNF(i, &xObj1);
      if (checkObject(&xObj1, &xObj2) && xObj2.isStream()) {
	xObj2.streamGetDict()->lookupNF("Resources", &resObj);
	scanFonts(&resObj, doc);
	resObj.free();
      }
      xObj2.free();
      xObj1.free();
    }
  }
  xObjDict2.free();
  xObjDict1.free();

  // recursively scan any resource dictionaries in Patterns in this
  // resource dictionary
  resDict->lookupNF("Pattern", &patternDict1);
  if (checkObject(&patternDict1, &patternDict2) && patternDict2.isDict()) {
    for (i = 0; i < patternDict2.dictGetLength(); ++i) {
      patternDict2.dictGetValNF(i, &pattern1);
      if (checkObject(&pattern1, &pattern2) && pattern2.isStream()) {
	pattern2.streamGetDict()->lookupNF("Resources", &resObj);
	scanFonts(&resObj, doc);
	resObj.free();
      }
      pattern2.free();
      pattern1.free();
    }
  }
  patternDict2.free();
  patternDict1.free();

  // recursively scan any resource dictionaries in ExtGStates in this
  // resource dictionary
  resDict->lookupNF("ExtGState", &gsDict1);
  if (checkObject(&gsDict1, &gsDict2) && gsDict2.isDict()) {
    for (i = 0; i < gsDict2.dictGetLength(); ++i) {
      gsDict2.dictGetValNF(i, &gs1);
      if (checkObject(&gs1, &gs2) && gs2.isDict()) {
	gs2.dictLookupNF("SMask", &smask1);
	if (checkObject(&smask1, &smask2) && smask2.isDict()) {
	  smask2.dictLookupNF("G", &smaskGroup1);
	  if (checkObject(&smaskGroup1, &smaskGroup2) &&
	      smaskGroup2.isStream()) {
	    smaskGroup2.streamGetDict()->lookupNF("Resources", &resObj);
	    scanFonts(&resObj, doc);
	    resObj.free();
	  }
	  smaskGroup2.free();
	  smaskGroup1.free();
	}
	smask2.free();
	smask1.free();
      }
      gs2.free();
      gs1.free();
    }
  }
  gsDict2.free();
  gsDict1.free();
}

static void scanFont(GfxFont *font, PDFDoc *doc) {
  Ref fontRef, embRef;
  Object fontObj, toUnicodeObj;
  GString *name;
  GBool emb, subset, hasToUnicode;
  GfxFontLoc *loc;
  int i;

  fontRef = *font->getID();

  // check for an already-seen font
  for (i = 0; i < fontsLen; ++i) {
    if (fontRef.num == fonts[i].num && fontRef.gen == fonts[i].gen) {
      return;
    }
  }

  // font name
  name = font->getName();

  // check for an embedded font
  if (font->getType() == fontType3) {
    emb = gTrue;
  } else {
    emb = font->getEmbeddedFontID(&embRef);
  }

  // look for a ToUnicode map
  hasToUnicode = gFalse;
  if (doc->getXRef()->fetch(fontRef.num, fontRef.gen, &fontObj)->isDict()) {
    hasToUnicode = fontObj.dictLookup("ToUnicode", &toUnicodeObj)->isStream();
    toUnicodeObj.free();
  }
  fontObj.free();

  // check for a font subset name: capital letters followed by a '+'
  // sign
  subset = gFalse;
  if (name) {
    for (i = 0; i < name->getLength(); ++i) {
      if (name->getChar(i) < 'A' || name->getChar(i) > 'Z') {
	break;
      }
    }
    subset = i > 0 && i < name->getLength() && name->getChar(i) == '+';
  }

  // print the font info
  printf("%-46s %-17s %-3s %-3s %-3s %-4s",
	 name ? name->getCString() : "[none]",
	 fontTypeNames[font->getType()],
	 emb ? "yes" : "no",
	 subset ? "yes" : "no",
	 hasToUnicode ? "yes" : "no",
	 font->problematicForUnicode() ? " X" : "");
  if (fontRef.gen >= 100000) {
    printf(" [none]");
  } else {
    printf(" %6d %2d", fontRef.num, fontRef.gen);
  }
  if (showFontLoc || showFontLocPS) {
    if (font->getType() == fontType3) {
      printf(" embedded");
    } else {
      loc = font->locateFont(doc->getXRef(), showFontLocPS);
      if (loc) {
	if (loc->locType == gfxFontLocEmbedded) {
	  printf(" embedded");
	} else if (loc->locType == gfxFontLocExternal) {
	  if (loc->path) {
	    printf(" external: %s", loc->path->getCString());
	  } else {
	    printf(" unavailable");
	  }
	} else if (loc->locType == gfxFontLocResident) {
	  if (loc->path) {
	    printf(" resident: %s", loc->path->getCString());
	  } else {
	    printf(" unavailable");
	  }
	}
      } else {
	printf(" unknown");
      }
      delete loc;
    }
  }
  printf("\n");

  // add this font to the list
  if (fontsLen == fontsSize) {
    if (fontsSize <= INT_MAX - 32) {
      fontsSize += 32;
    } else {
      // let greallocn throw an exception
      fontsSize = -1;
    }
    fonts = (Ref *)greallocn(fonts, fontsSize, sizeof(Ref));
  }
  fonts[fontsLen++] = *font->getID();
}

static GBool checkObject(Object *in, Object *out) {
  int objNum;

  if (!in->isRef()) {
    in->copy(out);
    return gTrue;
  }
  objNum = in->getRefNum();
  if (objNum < 0 || objNum >= numObjects) {
    out->initNull();
    return gTrue;
  }
  if (seenObjs[objNum]) {
    out->initNull();
    return gFalse;
  }
  seenObjs[objNum] = (char)1;
  in->fetch(doc->getXRef(), out);
  return gTrue;
}
