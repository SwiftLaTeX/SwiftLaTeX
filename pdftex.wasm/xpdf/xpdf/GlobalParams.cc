//========================================================================
//
// GlobalParams.cc
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================
//  Modified for TeX Live by Peter Breitenlohner <tex-live@tug.org>
//  See top-level ChangeLog for a list of all modifications
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#  include <shlobj.h>
#endif
#if HAVE_PAPER_H
#include <paper.h>
#endif
#if HAVE_FONTCONFIG
#  include <fontconfig/fontconfig.h>
#endif
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "gfile.h"
#include "FoFiIdentifier.h"
#include "Error.h"
#include "NameToCharCode.h"
#include "CharCodeToUnicode.h"
#include "UnicodeRemapping.h"
#include "UnicodeMap.h"
#include "CMap.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "GlobalParams.h"

#ifdef _WIN32
#  undef strcasecmp
#  undef strncasecmp
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#else
#  include <strings.h>
#endif

#if MULTITHREADED
#  define lockGlobalParams            gLockMutex(&mutex)
#  define lockUnicodeMapCache         gLockMutex(&unicodeMapCacheMutex)
#  define lockCMapCache               gLockMutex(&cMapCacheMutex)
#  define unlockGlobalParams          gUnlockMutex(&mutex)
#  define unlockUnicodeMapCache       gUnlockMutex(&unicodeMapCacheMutex)
#  define unlockCMapCache             gUnlockMutex(&cMapCacheMutex)
#else
#  define lockGlobalParams
#  define lockUnicodeMapCache
#  define lockCMapCache
#  define unlockGlobalParams
#  define unlockUnicodeMapCache
#  define unlockCMapCache
#endif

#include "NameToUnicodeTable.h"
#include "UnicodeMapTables.h"
#include "UTF8.h"

//------------------------------------------------------------------------

#define cidToUnicodeCacheSize     4
#define unicodeToUnicodeCacheSize 4

//------------------------------------------------------------------------

static struct {
  const char *name;
  const char *t1FileName;
  const char *ttFileName;
  const char *macFileName;	// may be .dfont, .ttf, or .ttc
  const char *macFontName;	// font name inside .dfont or .ttc
  const char *obliqueFont;	// name of font to oblique
  double obliqueFactor;		// oblique sheer factor
} displayFontTab[] = {
  {"Courier",               "n022003l.pfb", "cour.ttf",    "Courier",      "Courier",                NULL,             0},
  {"Courier-Bold",          "n022004l.pfb", "courbd.ttf",  "Courier",      "Courier Bold",           NULL,             0},
  {"Courier-BoldOblique",   "n022024l.pfb", "courbi.ttf",  "Courier",      "Courier Bold Oblique",   "Courier-Bold",   0.212557},
  {"Courier-Oblique",       "n022023l.pfb", "couri.ttf",   "Courier",      "Courier Oblique",        "Courier",        0.212557},
  {"Helvetica",             "n019003l.pfb", "arial.ttf",   "Helvetica",    "Helvetica",              NULL,             0},
  {"Helvetica-Bold",        "n019004l.pfb", "arialbd.ttf", "Helvetica",    "Helvetica Bold",         NULL,             0},
  {"Helvetica-BoldOblique", "n019024l.pfb", "arialbi.ttf", "Helvetica",    "Helvetica Bold Oblique", "Helvetica-Bold", 0.212557},
  {"Helvetica-Oblique",     "n019023l.pfb", "ariali.ttf",  "Helvetica",    "Helvetica Oblique",      "Helvetica",      0.212557},
  {"Symbol",                "s050000l.pfb", NULL,          "Symbol",       "Symbol",                 NULL,             0},
  {"Times-Bold",            "n021004l.pfb", "timesbd.ttf", "Times",        "Times Bold",             NULL,             0},
  {"Times-BoldItalic",      "n021024l.pfb", "timesbi.ttf", "Times",        "Times Bold Italic",       NULL,             0},
  {"Times-Italic",          "n021023l.pfb", "timesi.ttf",  "Times",        "Times Italic",           NULL,             0},
  {"Times-Roman",           "n021003l.pfb", "times.ttf",   "Times",        "Times Roman",            NULL,             0},
  {"ZapfDingbats",          "d050000l.pfb", NULL,          "ZapfDingbats", "Zapf Dingbats",          NULL,             0},
  {NULL}
};

#ifdef _WIN32
static const char *displayFontDirs[] = {
  "c:/windows/fonts",
  "c:/winnt/fonts",
  NULL
};
#else
static const char *displayFontDirs[] = {
  "/usr/share/ghostscript/fonts",
  "/usr/local/share/ghostscript/fonts",
  "/usr/share/fonts/default/Type1",
  "/usr/share/fonts/default/ghostscript",
  "/usr/share/fonts/type1/gsfonts",
#if defined(__sun) && defined(__SVR4)
  "/usr/sfw/share/ghostscript/fonts",
#endif
  NULL
};
#endif

#ifdef __APPLE__
static const char *macSystemFontPath = "/System/Library/Fonts";
#endif

struct Base14FontInfo {
  Base14FontInfo(GString *fileNameA, int fontNumA, double obliqueA) {
    fileName = fileNameA;
    fontNum = fontNumA;
    oblique = obliqueA;
  }
  ~Base14FontInfo() {
    delete fileName;
  }
  GString *fileName;
  int fontNum;
  double oblique;
};

//------------------------------------------------------------------------

GlobalParams *globalParams = NULL;

const char *GlobalParams::defaultTextEncoding = "Latin1";

//------------------------------------------------------------------------
// PSFontParam16
//------------------------------------------------------------------------

PSFontParam16::PSFontParam16(GString *nameA, int wModeA,
			     GString *psFontNameA, GString *encodingA) {
  name = nameA;
  wMode = wModeA;
  psFontName = psFontNameA;
  encoding = encodingA;
}

PSFontParam16::~PSFontParam16() {
  delete name;
  delete psFontName;
  delete encoding;
}

//------------------------------------------------------------------------
// SysFontInfo
//------------------------------------------------------------------------

class SysFontInfo {
public:

  GString *name;
  GBool bold;
  GBool italic;
  GString *path;
  SysFontType type;
  int fontNum;			// for TrueType collections

  SysFontInfo(GString *nameA, GBool boldA, GBool italicA,
	      GString *pathA, SysFontType typeA, int fontNumA);
  ~SysFontInfo();
  GBool match(SysFontInfo *fi);
  GBool match(GString *nameA, GBool boldA, GBool italicA);
};

SysFontInfo::SysFontInfo(GString *nameA, GBool boldA, GBool italicA,
			 GString *pathA, SysFontType typeA, int fontNumA) {
  name = nameA;
  bold = boldA;
  italic = italicA;
  path = pathA;
  type = typeA;
  fontNum = fontNumA;
}

SysFontInfo::~SysFontInfo() {
  delete name;
  delete path;
}

GBool SysFontInfo::match(SysFontInfo *fi) {
  return !strcasecmp(name->getCString(), fi->name->getCString()) &&
         bold == fi->bold && italic == fi->italic;
}

GBool SysFontInfo::match(GString *nameA, GBool boldA, GBool italicA) {
  return !strcasecmp(name->getCString(), nameA->getCString()) &&
         bold == boldA && italic == italicA;
}

//------------------------------------------------------------------------
// SysFontList
//------------------------------------------------------------------------

class SysFontList {
public:

  SysFontList();
  ~SysFontList();
  SysFontInfo *find(GString *name);

#ifdef _WIN32
  void scanWindowsFonts(char *winFontDir);
#endif

#if HAVE_FONTCONFIG
  void scanFontconfigFonts();
#endif

private:

#ifdef _WIN32
  SysFontInfo *makeWindowsFont(char *name, int fontNum,
			       char *path);
#endif

  GList *fonts;			// [SysFontInfo]
};

SysFontList::SysFontList() {
  fonts = new GList();
}

SysFontList::~SysFontList() {
  deleteGList(fonts, SysFontInfo);
}

SysFontInfo *SysFontList::find(GString *name) {
  GString *name2;
  GBool bold, italic;
  SysFontInfo *fi;
  char c;
  int n, i;

  name2 = name->copy();

  // remove space, comma, dash chars
  i = 0;
  while (i < name2->getLength()) {
    c = name2->getChar(i);
    if (c == ' ' || c == ',' || c == '-') {
      name2->del(i);
    } else {
      ++i;
    }
  }
  n = name2->getLength();

  // font names like "Arial-BoldMT,Bold" are occasionally used,
  // so run this loop twice
  bold = italic = gFalse;
  for (i = 0; i < 2; ++i) {

    // remove trailing "MT" (Foo-MT, Foo-BoldMT, etc.)
    if (n > 2 && !strcmp(name2->getCString() + n - 2, "MT")) {
      name2->del(n - 2, 2);
      n -= 2;
    }

    // look for "Regular"
    if (n > 7 && !strcmp(name2->getCString() + n - 7, "Regular")) {
      name2->del(n - 7, 7);
      n -= 7;
    }

    // look for "Italic"
    if (n > 6 && !strcmp(name2->getCString() + n - 6, "Italic")) {
      name2->del(n - 6, 6);
      italic = gTrue;
      n -= 6;
    }

    // look for "Bold"
    if (n > 4 && !strcmp(name2->getCString() + n - 4, "Bold")) {
      name2->del(n - 4, 4);
      bold = gTrue;
      n -= 4;
    }
  }

  // remove trailing "PS"
  if (n > 2 && !strcmp(name2->getCString() + n - 2, "PS")) {
    name2->del(n - 2, 2);
    n -= 2;
  }

  // remove trailing "IdentityH"
  if (n > 9 && !strcmp(name2->getCString() + n - 9, "IdentityH")) {
    name2->del(n - 9, 9);
    n -= 9;
  }

  // search for the font
  fi = NULL;
  for (i = 0; i < fonts->getLength(); ++i) {
    fi = (SysFontInfo *)fonts->get(i);
    if (fi->match(name2, bold, italic)) {
      break;
    }
    fi = NULL;
  }
  if (!fi && bold) {
    // try ignoring the bold flag
    for (i = 0; i < fonts->getLength(); ++i) {
      fi = (SysFontInfo *)fonts->get(i);
      if (fi->match(name2, gFalse, italic)) {
	break;
      }
      fi = NULL;
    }
  }
  if (!fi && (bold || italic)) {
    // try ignoring the bold and italic flags
    for (i = 0; i < fonts->getLength(); ++i) {
      fi = (SysFontInfo *)fonts->get(i);
      if (fi->match(name2, gFalse, gFalse)) {
	break;
      }
      fi = NULL;
    }
  }

  delete name2;
  return fi;
}

#ifdef _WIN32
void SysFontList::scanWindowsFonts(char *winFontDir) {
  OSVERSIONINFO version;
  const char *path;
  DWORD idx, valNameLen, dataLen, type;
  HKEY regKey;
  char valName[1024], data[1024];
  int n, fontNum;
  char *p0, *p1;
  GString *fontPath;

  version.dwOSVersionInfoSize = sizeof(version);
  GetVersionEx(&version);
  if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
    path = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts\\";
  } else {
    path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts\\";
  }
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0,
		    KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,
		    &regKey) == ERROR_SUCCESS) {
    idx = 0;
    while (1) {
      valNameLen = sizeof(valName) - 1;
      dataLen = sizeof(data) - 1;
      if (RegEnumValueA(regKey, idx, valName, &valNameLen, NULL,
			&type, (LPBYTE)data, &dataLen) != ERROR_SUCCESS) {
	break;
      }
      if (type == REG_SZ &&
	  valNameLen > 0 && valNameLen < sizeof(valName) &&
	  dataLen > 0 && dataLen < sizeof(data)) {
	valName[valNameLen] = '\0';
	data[dataLen] = '\0';
	n = (int)strlen(data);
	if (!strcasecmp(data + n - 4, ".ttf") ||
	    !strcasecmp(data + n - 4, ".ttc") ||
	    !strcasecmp(data + n - 4, ".otf")) {
	  fontPath = new GString(data);
	  if (!(dataLen >= 3 && data[1] == ':' && data[2] == '\\')) {
	    fontPath->insert(0, '\\');
	    fontPath->insert(0, winFontDir);
	  }
	  p0 = valName;
	  fontNum = 0;
	  while (*p0) {
	    p1 = strstr(p0, " & ");
	    if (p1) {
	      *p1 = '\0';
	      p1 = p1 + 3;
	    } else {
	      p1 = p0 + strlen(p0);
	    }
	    fonts->append(makeWindowsFont(p0, fontNum,
					  fontPath->getCString()));
	    p0 = p1;
	    ++fontNum;
	  }
	  delete fontPath;
	}
      }
      ++idx;
    }
    RegCloseKey(regKey);
  }
}

SysFontInfo *SysFontList::makeWindowsFont(char *name, int fontNum,
					  char *path) {
  int n;
  GBool bold, italic;
  GString *s;
  char c;
  int i;
  SysFontType type;

  n = (int)strlen(name);
  bold = italic = gFalse;

  // remove trailing ' (TrueType)'
  if (n > 11 && !strncmp(name + n - 11, " (TrueType)", 11)) {
    n -= 11;
  }

  // remove trailing ' Italic'
  if (n > 7 && !strncmp(name + n - 7, " Italic", 7)) {
    n -= 7;
    italic = gTrue;
  }

  // remove trailing ' Bold'
  if (n > 5 && !strncmp(name + n - 5, " Bold", 5)) {
    n -= 5;
    bold = gTrue;
  }

  // remove trailing ' Regular'
  if (n > 8 && !strncmp(name + n - 8, " Regular", 8)) {
    n -= 8;
  }

  //----- normalize the font name
  s = new GString(name, n);
  i = 0;
  while (i < s->getLength()) {
    c = s->getChar(i);
    if (c == ' ' || c == ',' || c == '-') {
      s->del(i);
    } else {
      ++i;
    }
  }

  if (!strcasecmp(path + strlen(path) - 4, ".ttc")) {
    type = sysFontTTC;
  } else if (!strcasecmp(path + strlen(path) - 4, ".otf")) {
    type = sysFontOTF;
  } else {
    type = sysFontTTF;
  }
  return new SysFontInfo(s, bold, italic, new GString(path), type, fontNum);
}
#endif

#if HAVE_FONTCONFIG
void SysFontList::scanFontconfigFonts() {
  FcPattern *pattern;
  FcObjectSet *objSet;
  FcFontSet *fontSet;
  char *family, *file, *styleLang, *style;
  GString *family2;
  SysFontType type;
  GBool bold, italic;
  char c;
  int fontNum, i, j, n;

  FcInit();

  pattern = FcPatternBuild(NULL,
			   FC_OUTLINE, FcTypeBool, FcTrue,
			   FC_SCALABLE, FcTypeBool, FcTrue,
			   NULL);
  objSet = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_STYLELANG,
			    FC_FILE, FC_INDEX, NULL);
  fontSet = FcFontList(NULL, pattern, objSet);
  FcPatternDestroy(pattern);
  FcObjectSetDestroy(objSet);

  if (fontSet) {
    for (i = 0; i < fontSet->nfont; ++i) {

      //--- font file, font type
      if (FcPatternGetString(fontSet->fonts[i], FC_FILE, 0,
			     (FcChar8 **)&file)
	  != FcResultMatch) {
	continue;
      }
      n = (int)strlen(file);
      if (n > 4 && !strcasecmp(file + n - 4, ".pfa")) {
	type = sysFontPFA;
      } else if (n > 4 && !strcasecmp(file + n - 4, ".pfb")) {
	type = sysFontPFB;
      } else if (n > 4 && !strcasecmp(file + n - 4, ".ttf")) {
	type = sysFontTTF;
      } else if (n > 4 && !strcasecmp(file + n - 4, ".otf")) {
	type = sysFontOTF;
      } else {
	continue;
      }

      //--- font number
      if (FcPatternGetInteger(fontSet->fonts[i], FC_INDEX, 0, &fontNum)
	  != FcResultMatch) {
	fontNum = 0;
      }

      //--- font family
      if (FcPatternGetString(fontSet->fonts[i], FC_FAMILY, 0,
			     (FcChar8 **)&family)
	  != FcResultMatch) {
	continue;
      }

      //----- normalize the font name
      family2 = new GString(family);
      j = 0;
      while (j < family2->getLength()) {
	c = family2->getChar(j);
	if (c == ' ' || c == ',' || c == '-') {
	  family2->del(j);
	} else {
	  ++j;
	}
      }

      //--- font style
      style = NULL;
      for (j = 0;
	   FcPatternGetString(fontSet->fonts[i], FC_STYLELANG, j,
			      (FcChar8 **)&styleLang)
	     == FcResultMatch;
	   ++j) {
	if (!strcmp(styleLang, "en")) {
	  if (FcPatternGetString(fontSet->fonts[i], FC_STYLE, j,
				 (FcChar8 **)&style)
	      != FcResultMatch) {
	    style = NULL;
	  }
	  break;
	}
	++j;
      }
      bold = style && strstr(style, "Bold") != NULL;
      italic = style && (strstr(style, "Italic") != NULL ||
			 strstr(style, "Oblique") != NULL);

      fonts->append(new SysFontInfo(family2, bold, italic,
				    new GString(file), type, fontNum));
    }

    FcFontSetDestroy(fontSet);
  }

  FcFini();
}
#endif // HAVE_FONTCONFIG

//------------------------------------------------------------------------
// KeyBinding
//------------------------------------------------------------------------

KeyBinding::KeyBinding(int codeA, int modsA, int contextA, const char *cmd0) {
  code = codeA;
  mods = modsA;
  context = contextA;
  cmds = new GList();
  cmds->append(new GString(cmd0));
}

KeyBinding::KeyBinding(int codeA, int modsA, int contextA,
		       const char *cmd0, const char *cmd1) {
  code = codeA;
  mods = modsA;
  context = contextA;
  cmds = new GList();
  cmds->append(new GString(cmd0));
  cmds->append(new GString(cmd1));
}

KeyBinding::KeyBinding(int codeA, int modsA, int contextA, GList *cmdsA) {
  code = codeA;
  mods = modsA;
  context = contextA;
  cmds = cmdsA;
}

KeyBinding::~KeyBinding() {
  deleteGList(cmds, GString);
}

//------------------------------------------------------------------------
// PopupMenuCmd
//------------------------------------------------------------------------

PopupMenuCmd::PopupMenuCmd(GString *labelA, GList *cmdsA) {
  label = labelA;
  cmds = cmdsA;
}

PopupMenuCmd::~PopupMenuCmd() {
  delete label;
  deleteGList(cmds, GString);
}

//------------------------------------------------------------------------
// parsing
//------------------------------------------------------------------------

GlobalParams::GlobalParams(const char *cfgFileName) {
  UnicodeMap *map;
  GString *fileName;
  FILE *f;
  int i;

#if MULTITHREADED
  gInitMutex(&mutex);
  gInitMutex(&unicodeMapCacheMutex);
  gInitMutex(&cMapCacheMutex);
#endif

#ifdef _WIN32
  tlsWin32ErrorInfo = TlsAlloc();
#endif

  initBuiltinFontTables();

  // scan the encoding in reverse because we want the lowest-numbered
  // index for each char name ('space' is encoded twice)
  macRomanReverseMap = new NameToCharCode();
  for (i = 255; i >= 0; --i) {
    if (macRomanEncoding[i]) {
      macRomanReverseMap->add(macRomanEncoding[i], (CharCode)i);
    }
  }

#ifdef _WIN32
  // baseDir will be set by a call to setBaseDir
  baseDir = new GString();
#else
  baseDir = appendToPath(getHomeDir(), ".xpdf");
#endif
  nameToUnicode = new NameToCharCode();
  cidToUnicodes = new GHash(gTrue);
  unicodeToUnicodes = new GHash(gTrue);
  residentUnicodeMaps = new GHash();
  unicodeMaps = new GHash(gTrue);
  cMapDirs = new GHash(gTrue);
  toUnicodeDirs = new GList();
  unicodeRemapping = new UnicodeRemapping();
  fontFiles = new GHash(gTrue);
  fontDirs = new GList();
  ccFontFiles = new GHash(gTrue);
  base14SysFonts = new GHash(gTrue);
  sysFonts = new SysFontList();
#if HAVE_PAPER_H
  char *paperName;
  const struct paper *paperType;
  paperinit();
  if ((paperName = systempapername())) {
    paperType = paperinfo(paperName);
    psPaperWidth = (int)paperpswidth(paperType);
    psPaperHeight = (int)paperpsheight(paperType);
  } else {
    error(errConfig, -1, "No paper information available - using defaults");
    psPaperWidth = defPaperWidth;
    psPaperHeight = defPaperHeight;
  }
  paperdone();
#else
  psPaperWidth = defPaperWidth;
  psPaperHeight = defPaperHeight;
#endif
  psImageableLLX = psImageableLLY = 0;
  psImageableURX = psPaperWidth;
  psImageableURY = psPaperHeight;
  psCrop = gTrue;
  psUseCropBoxAsPage = gFalse;
  psExpandSmaller = gFalse;
  psShrinkLarger = gTrue;
  psCenter = gTrue;
  psDuplex = gFalse;
  psLevel = psLevel2;
  psResidentFonts = new GHash(gTrue);
  psResidentFonts16 = new GList();
  psResidentFontsCC = new GList();
  psEmbedType1 = gTrue;
  psEmbedTrueType = gTrue;
  psEmbedCIDPostScript = gTrue;
  psEmbedCIDTrueType = gTrue;
  psFontPassthrough = gFalse;
  psPreload = gFalse;
  psOPI = gFalse;
  psASCIIHex = gFalse;
  psLZW = gTrue;
  psUncompressPreloadedImages = gFalse;
  psMinLineWidth = 0;
  psRasterResolution = 300;
  psRasterMono = gFalse;
  psRasterSliceSize = 20000000;
  psAlwaysRasterize = gFalse;
  psNeverRasterize = gFalse;
  textEncoding = new GString(defaultTextEncoding);
#if defined(_WIN32)
  textEOL = eolDOS;
#else
  textEOL = eolUnix;
#endif
  textPageBreaks = gTrue;
  textKeepTinyChars = gTrue;
  initialZoom = new GString("125");
  defaultFitZoom = 0;
  initialDisplayMode = new GString("continuous");
  initialToolbarState = gTrue;
  initialSidebarState = gTrue;
  initialSelectMode = new GString("linear");
  maxTileWidth = 1500;
  maxTileHeight = 1500;
  tileCacheSize = 10;
  workerThreads = 1;
  enableFreeType = gTrue;
  disableFreeTypeHinting = gFalse;
  antialias = gTrue;
  vectorAntialias = gTrue;
  antialiasPrinting = gFalse;
  strokeAdjust = strokeAdjustNormal;
  screenType = screenUnset;
  screenSize = -1;
  screenDotRadius = -1;
  screenGamma = 1.0;
  screenBlackThreshold = 0.0;
  screenWhiteThreshold = 1.0;
  minLineWidth = 0.0;
  enablePathSimplification = gFalse;
  drawAnnotations = gTrue;
  drawFormFields = gTrue;
  overprintPreview = gFalse;
  paperColor = new GString("#ffffff");
  matteColor = new GString("#808080");
  fullScreenMatteColor = new GString("#000000");
  reverseVideoInvertImages = gFalse;
  launchCommand = NULL;
  movieCommand = NULL;
  defaultPrinter = NULL;
  mapNumericCharNames = gTrue;
  mapUnknownCharNames = gFalse;
  mapExtTrueTypeFontsViaUnicode = gTrue;
  droppedFonts = new GHash(gTrue);
  enableXFA = gTrue;
  createDefaultKeyBindings();
  popupMenuCmds = new GList();
  tabStateFile = appendToPath(getHomeDir(), ".xpdf.tab-state");
  printCommands = gFalse;
  errQuiet = gFalse;
  debugLogFile = NULL;

  cidToUnicodeCache = new CharCodeToUnicodeCache(cidToUnicodeCacheSize);
  unicodeToUnicodeCache =
      new CharCodeToUnicodeCache(unicodeToUnicodeCacheSize);
  unicodeMapCache = new UnicodeMapCache();
  cMapCache = new CMapCache();

  // set up the initial nameToUnicode table
  for (i = 0; nameToUnicodeTab[i].name; ++i) {
    nameToUnicode->add(nameToUnicodeTab[i].name, nameToUnicodeTab[i].u);
  }

  // set up the residentUnicodeMaps table
  map = new UnicodeMap("Latin1", gFalse,
		       latin1UnicodeMapRanges, latin1UnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("ASCII7", gFalse,
		       ascii7UnicodeMapRanges, ascii7UnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("Symbol", gFalse,
		       symbolUnicodeMapRanges, symbolUnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("ZapfDingbats", gFalse, zapfDingbatsUnicodeMapRanges,
		       zapfDingbatsUnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("UTF-8", gTrue, &mapUTF8);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("UCS-2", gTrue, &mapUCS2);
  residentUnicodeMaps->add(map->getEncodingName(), map);

  // look for a user config file, then a system-wide config file
  f = NULL;
  fileName = NULL;
  if (cfgFileName && cfgFileName[0]) {
#ifndef PDF_PARSER_ONLY
    fileName = new GString(cfgFileName);
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (!f) {
    fileName = appendToPath(getHomeDir(), xpdfUserConfigFile);
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (!f) {
#ifdef _WIN32
    char buf[512];
    i = GetModuleFileNameA(NULL, buf, sizeof(buf));
    if (i <= 0 || i >= sizeof(buf)) {
      // error or path too long for buffer - just use the current dir
      buf[0] = '\0';
    }
    fileName = grabPath(buf);
    appendToPath(fileName, xpdfSysConfigFile);
#else
    fileName = new GString(xpdfSysConfigFile);
#endif
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (f) {
    parseFile(fileName, f);
    delete fileName;
    fclose(f);
#endif /* !PDF_PARSER_ONLY */
  }
}

void GlobalParams::createDefaultKeyBindings() {
  keyBindings = new GList();

  //----- mouse buttons
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress1, xpdfKeyModNone,
				     xpdfKeyContextAny, "startSelection"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMouseRelease1, xpdfKeyModNone,
				     xpdfKeyContextAny, "endSelection"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMouseClick1, xpdfKeyModNone,
				     xpdfKeyContextAny, "followLinkNoSel"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMouseClick2, xpdfKeyModNone,
				     xpdfKeyContextOverLink,
				     "followLinkInNewTab"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress2, xpdfKeyModNone,
				     xpdfKeyContextAny, "startPan"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMouseRelease2, xpdfKeyModNone,
				     xpdfKeyContextAny, "endPan"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress3, xpdfKeyModNone,
				     xpdfKeyContextAny, "postPopupMenu"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress4, xpdfKeyModNone,
				     xpdfKeyContextAny,
				     "scrollUpPrevPage(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress5, xpdfKeyModNone,
				     xpdfKeyContextAny,
				     "scrollDownNextPage(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress6, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollLeft(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeMousePress7, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollRight(16)"));

  //----- control keys
  keyBindings->append(new KeyBinding('o', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "open"));
  keyBindings->append(new KeyBinding('r', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "reload"));
  keyBindings->append(new KeyBinding('f', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "find"));
  keyBindings->append(new KeyBinding('g', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "findNext"));
  keyBindings->append(new KeyBinding('c', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "copy"));
  keyBindings->append(new KeyBinding('p', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "print"));
  keyBindings->append(new KeyBinding('0', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "zoomPercent(125)"));
  keyBindings->append(new KeyBinding('+', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "zoomIn"));
  keyBindings->append(new KeyBinding('=', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "zoomIn"));
  keyBindings->append(new KeyBinding('-', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "zoomOut"));
  keyBindings->append(new KeyBinding('s', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "saveAs"));
  keyBindings->append(new KeyBinding('t', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "newTab"));
  keyBindings->append(new KeyBinding('n', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "newWindow"));
  keyBindings->append(new KeyBinding('w', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "closeTabOrQuit"));
  keyBindings->append(new KeyBinding('l', xpdfKeyModCtrl,
				     xpdfKeyContextAny,
				     "toggleFullScreenMode"));
  keyBindings->append(new KeyBinding('q', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "quit"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeTab, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "nextTab"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeTab,
				     xpdfKeyModShift | xpdfKeyModCtrl,
				     xpdfKeyContextAny, "prevTab"));
  keyBindings->append(new KeyBinding('?', xpdfKeyModCtrl,
				     xpdfKeyContextAny, "help"));

  //----- alt keys
  keyBindings->append(new KeyBinding(xpdfKeyCodeLeft, xpdfKeyModAlt,
				     xpdfKeyContextAny, "goBackward"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeRight, xpdfKeyModAlt,
				     xpdfKeyContextAny, "goForward"));

  //----- home/end keys
  keyBindings->append(new KeyBinding(xpdfKeyCodeHome, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "gotoPage(1)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeHome, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollToTopLeft"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeEnd, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "gotoLastPage"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeEnd, xpdfKeyModNone,
				     xpdfKeyContextAny,
				     "scrollToBottomRight"));

  //----- pgup/pgdn keys
  keyBindings->append(new KeyBinding(xpdfKeyCodePgUp, xpdfKeyModNone,
				     xpdfKeyContextAny, "pageUp"));
  keyBindings->append(new KeyBinding(xpdfKeyCodePgDn, xpdfKeyModNone,
				     xpdfKeyContextAny, "pageDown"));
  keyBindings->append(new KeyBinding(xpdfKeyCodePgUp, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "prevPage"));
  keyBindings->append(new KeyBinding(xpdfKeyCodePgDn, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "nextPage"));
  keyBindings->append(new KeyBinding(xpdfKeyCodePgUp, xpdfKeyModCtrl,
				     xpdfKeyContextScrLockOn,
				     "prevPageNoScroll"));
  keyBindings->append(new KeyBinding(xpdfKeyCodePgDn, xpdfKeyModCtrl,
				     xpdfKeyContextScrLockOn,
				     "nextPageNoScroll"));

  //----- esc key
  keyBindings->append(new KeyBinding(xpdfKeyCodeEsc, xpdfKeyModNone,
				     xpdfKeyContextFullScreen,
				     "windowMode"));

  //----- arrow keys
  keyBindings->append(new KeyBinding(xpdfKeyCodeLeft, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollLeft(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeRight, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollRight(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeUp, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollUp(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeDown, xpdfKeyModNone,
				     xpdfKeyContextAny, "scrollDown(16)"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeUp, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "prevPage"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeDown, xpdfKeyModCtrl,
				     xpdfKeyContextAny, "nextPage"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeUp, xpdfKeyModCtrl,
				     xpdfKeyContextScrLockOn,
				     "prevPageNoScroll"));
  keyBindings->append(new KeyBinding(xpdfKeyCodeDown, xpdfKeyModCtrl,
				     xpdfKeyContextScrLockOn,
				     "nextPageNoScroll"));

  //----- letter keys
  keyBindings->append(new KeyBinding(' ', xpdfKeyModNone,
				     xpdfKeyContextAny, "pageDown"));
  keyBindings->append(new KeyBinding('g', xpdfKeyModNone,
				     xpdfKeyContextAny, "focusToPageNum"));
  keyBindings->append(new KeyBinding('z', xpdfKeyModNone,
				     xpdfKeyContextAny, "zoomFitPage"));
  keyBindings->append(new KeyBinding('w', xpdfKeyModNone,
				     xpdfKeyContextAny, "zoomFitWidth"));
}

void GlobalParams::parseFile(GString *fileName, FILE *f) {
  int line;
  char buf[512];

  line = 1;
  while (getLine(buf, sizeof(buf) - 1, f)) {
    parseLine(buf, fileName, line);
    ++line;
  }
}

void GlobalParams::parseLine(char *buf, GString *fileName, int line) {
  GList *tokens;
  GString *cmd, *incFile;
  char *p1, *p2;
  FILE *f2;

  // break the line into tokens
  tokens = new GList();
  p1 = buf;
  while (*p1) {
    for (; *p1 && isspace(*p1); ++p1) ;
    if (!*p1) {
      break;
    }
    if (*p1 == '"' || *p1 == '\'') {
      for (p2 = p1 + 1; *p2 && *p2 != *p1; ++p2) ;
      ++p1;
    } else {
      for (p2 = p1 + 1; *p2 && !isspace(*p2); ++p2) ;
    }
    tokens->append(new GString(p1, (int)(p2 - p1)));
    p1 = *p2 ? p2 + 1 : p2;
  }

  // parse the line
  if (tokens->getLength() > 0 &&
      ((GString *)tokens->get(0))->getChar(0) != '#') {
    cmd = (GString *)tokens->get(0);
    if (!cmd->cmp("include")) {
      if (tokens->getLength() == 2) {
	incFile = (GString *)tokens->get(1);
	if ((f2 = openFile(incFile->getCString(), "r"))) {
	  parseFile(incFile, f2);
	  fclose(f2);
	} else {
	  error(errConfig, -1,
		"Couldn't find included config file: '{0:t}' ({1:t}:{2:d})",
		incFile, fileName, line);
	}
      } else {
	error(errConfig, -1, "Bad 'include' config file command ({0:t}:{1:d})",
	      fileName, line);
      }
    } else if (!cmd->cmp("nameToUnicode")) {
      parseNameToUnicode(tokens, fileName, line);
    } else if (!cmd->cmp("cidToUnicode")) {
      parseCIDToUnicode(tokens, fileName, line);
    } else if (!cmd->cmp("unicodeToUnicode")) {
      parseUnicodeToUnicode(tokens, fileName, line);
    } else if (!cmd->cmp("unicodeMap")) {
      parseUnicodeMap(tokens, fileName, line);
    } else if (!cmd->cmp("cMapDir")) {
      parseCMapDir(tokens, fileName, line);
    } else if (!cmd->cmp("toUnicodeDir")) {
      parseToUnicodeDir(tokens, fileName, line);
    } else if (!cmd->cmp("unicodeRemapping")) {
      parseUnicodeRemapping(tokens, fileName, line);
    } else if (!cmd->cmp("fontFile")) {
      parseFontFile(tokens, fileName, line);
    } else if (!cmd->cmp("fontDir")) {
      parseFontDir(tokens, fileName, line);
    } else if (!cmd->cmp("fontFileCC")) {
      parseFontFileCC(tokens, fileName, line);
    } else if (!cmd->cmp("psPaperSize")) {
      parsePSPaperSize(tokens, fileName, line);
    } else if (!cmd->cmp("psImageableArea")) {
      parsePSImageableArea(tokens, fileName, line);
    } else if (!cmd->cmp("psCrop")) {
      parseYesNo("psCrop", &psCrop, tokens, fileName, line);
    } else if (!cmd->cmp("psUseCropBoxAsPage")) {
      parseYesNo("psUseCropBoxAsPage", &psUseCropBoxAsPage,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psExpandSmaller")) {
      parseYesNo("psExpandSmaller", &psExpandSmaller,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psShrinkLarger")) {
      parseYesNo("psShrinkLarger", &psShrinkLarger, tokens, fileName, line);
    } else if (!cmd->cmp("psCenter")) {
      parseYesNo("psCenter", &psCenter, tokens, fileName, line);
    } else if (!cmd->cmp("psDuplex")) {
      parseYesNo("psDuplex", &psDuplex, tokens, fileName, line);
    } else if (!cmd->cmp("psLevel")) {
      parsePSLevel(tokens, fileName, line);
    } else if (!cmd->cmp("psResidentFont")) {
      parsePSResidentFont(tokens, fileName, line);
    } else if (!cmd->cmp("psResidentFont16")) {
      parsePSResidentFont16(tokens, fileName, line);
    } else if (!cmd->cmp("psResidentFontCC")) {
      parsePSResidentFontCC(tokens, fileName, line);
    } else if (!cmd->cmp("psEmbedType1Fonts")) {
      parseYesNo("psEmbedType1", &psEmbedType1, tokens, fileName, line);
    } else if (!cmd->cmp("psEmbedTrueTypeFonts")) {
      parseYesNo("psEmbedTrueType", &psEmbedTrueType,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psEmbedCIDPostScriptFonts")) {
      parseYesNo("psEmbedCIDPostScript", &psEmbedCIDPostScript,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psEmbedCIDTrueTypeFonts")) {
      parseYesNo("psEmbedCIDTrueType", &psEmbedCIDTrueType,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psFontPassthrough")) {
      parseYesNo("psFontPassthrough", &psFontPassthrough,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psPreload")) {
      parseYesNo("psPreload", &psPreload, tokens, fileName, line);
    } else if (!cmd->cmp("psOPI")) {
      parseYesNo("psOPI", &psOPI, tokens, fileName, line);
    } else if (!cmd->cmp("psASCIIHex")) {
      parseYesNo("psASCIIHex", &psASCIIHex, tokens, fileName, line);
    } else if (!cmd->cmp("psLZW")) {
      parseYesNo("psLZW", &psLZW, tokens, fileName, line);
    } else if (!cmd->cmp("psUncompressPreloadedImages")) {
      parseYesNo("psUncompressPreloadedImages", &psUncompressPreloadedImages,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psMinLineWidth")) {
      parseFloat("psMinLineWidth", &psMinLineWidth,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psRasterResolution")) {
      parseFloat("psRasterResolution", &psRasterResolution,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psRasterMono")) {
      parseYesNo("psRasterMono", &psRasterMono, tokens, fileName, line);
    } else if (!cmd->cmp("psRasterSliceSize")) {
      parseInteger("psRasterSliceSize", &psRasterSliceSize,
		   tokens, fileName, line);
    } else if (!cmd->cmp("psAlwaysRasterize")) {
      parseYesNo("psAlwaysRasterize", &psAlwaysRasterize,
		 tokens, fileName, line);
    } else if (!cmd->cmp("psNeverRasterize")) {
      parseYesNo("psNeverRasterize", &psNeverRasterize,
		 tokens, fileName, line);
    } else if (!cmd->cmp("textEncoding")) {
      parseString("textEncoding", &textEncoding, tokens, fileName, line);
    } else if (!cmd->cmp("textEOL")) {
      parseTextEOL(tokens, fileName, line);
    } else if (!cmd->cmp("textPageBreaks")) {
      parseYesNo("textPageBreaks", &textPageBreaks,
		 tokens, fileName, line);
    } else if (!cmd->cmp("textKeepTinyChars")) {
      parseYesNo("textKeepTinyChars", &textKeepTinyChars,
		 tokens, fileName, line);
    } else if (!cmd->cmp("initialZoom")) {
      parseString("initialZoom", &initialZoom, tokens, fileName, line);
    } else if (!cmd->cmp("defaultFitZoom")) {
      parseInteger("defaultFitZoom", &defaultFitZoom, tokens, fileName, line);
    } else if (!cmd->cmp("initialDisplayMode")) {
      parseString("initialDisplayMode", &initialDisplayMode,
		  tokens, fileName, line);
    } else if (!cmd->cmp("initialToolbarState")) {
      parseYesNo("initialToolbarState", &initialToolbarState,
		 tokens, fileName, line);
    } else if (!cmd->cmp("initialSidebarState")) {
      parseYesNo("initialSidebarState", &initialSidebarState,
		 tokens, fileName, line);
    } else if (!cmd->cmp("initialSelectMode")) {
      parseString("initialSelectMode", &initialSelectMode,
		  tokens, fileName, line);
    } else if (!cmd->cmp("maxTileWidth")) {
      parseInteger("maxTileWidth", &maxTileWidth, tokens, fileName, line);
    } else if (!cmd->cmp("maxTileHeight")) {
      parseInteger("maxTileHeight", &maxTileHeight, tokens, fileName, line);
    } else if (!cmd->cmp("tileCacheSize")) {
      parseInteger("tileCacheSize", &tileCacheSize, tokens, fileName, line);
    } else if (!cmd->cmp("workerThreads")) {
      parseInteger("workerThreads", &workerThreads, tokens, fileName, line);
    } else if (!cmd->cmp("enableFreeType")) {
      parseYesNo("enableFreeType", &enableFreeType, tokens, fileName, line);
    } else if (!cmd->cmp("disableFreeTypeHinting")) {
      parseYesNo("disableFreeTypeHinting", &disableFreeTypeHinting,
		 tokens, fileName, line);
    } else if (!cmd->cmp("antialias")) {
      parseYesNo("antialias", &antialias, tokens, fileName, line);
    } else if (!cmd->cmp("vectorAntialias")) {
      parseYesNo("vectorAntialias", &vectorAntialias,
		 tokens, fileName, line);
    } else if (!cmd->cmp("antialiasPrinting")) {
      parseYesNo("antialiasPrinting", &antialiasPrinting,
		 tokens, fileName, line);
    } else if (!cmd->cmp("strokeAdjust")) {
      parseStrokeAdjust(tokens, fileName, line);
    } else if (!cmd->cmp("screenType")) {
      parseScreenType(tokens, fileName, line);
    } else if (!cmd->cmp("screenSize")) {
      parseInteger("screenSize", &screenSize, tokens, fileName, line);
    } else if (!cmd->cmp("screenDotRadius")) {
      parseInteger("screenDotRadius", &screenDotRadius,
		   tokens, fileName, line);
    } else if (!cmd->cmp("screenGamma")) {
      parseFloat("screenGamma", &screenGamma,
		 tokens, fileName, line);
    } else if (!cmd->cmp("screenBlackThreshold")) {
      parseFloat("screenBlackThreshold", &screenBlackThreshold,
		 tokens, fileName, line);
    } else if (!cmd->cmp("screenWhiteThreshold")) {
      parseFloat("screenWhiteThreshold", &screenWhiteThreshold,
		 tokens, fileName, line);
    } else if (!cmd->cmp("minLineWidth")) {
      parseFloat("minLineWidth", &minLineWidth,
		 tokens, fileName, line);
    } else if (!cmd->cmp("enablePathSimplification")) {
      parseYesNo("enablePathSimplification", &enablePathSimplification,
		 tokens, fileName, line);
    } else if (!cmd->cmp("drawAnnotations")) {
      parseYesNo("drawAnnotations", &drawAnnotations,
		 tokens, fileName, line);
    } else if (!cmd->cmp("drawFormFields")) {
      parseYesNo("drawFormFields", &drawFormFields,
		 tokens, fileName, line);
    } else if (!cmd->cmp("overprintPreview")) {
      parseYesNo("overprintPreview", &overprintPreview,
		 tokens, fileName, line);
    } else if (!cmd->cmp("paperColor")) {
      parseString("paperColor", &paperColor, tokens, fileName, line);
    } else if (!cmd->cmp("matteColor")) {
      parseString("matteColor", &matteColor, tokens, fileName, line);
    } else if (!cmd->cmp("fullScreenMatteColor")) {
      parseString("fullScreenMatteColor", &fullScreenMatteColor,
		  tokens, fileName, line);
    } else if (!cmd->cmp("reverseVideoInvertImages")) {
      parseYesNo("reverseVideoInvertImages", &reverseVideoInvertImages,
		 tokens, fileName, line);
    } else if (!cmd->cmp("launchCommand")) {
      parseString("launchCommand", &launchCommand, tokens, fileName, line);
    } else if (!cmd->cmp("movieCommand")) {
      parseString("movieCommand", &movieCommand, tokens, fileName, line);
    } else if (!cmd->cmp("defaultPrinter")) {
      parseString("defaultPrinter", &defaultPrinter, tokens, fileName, line);
    } else if (!cmd->cmp("mapNumericCharNames")) {
      parseYesNo("mapNumericCharNames", &mapNumericCharNames,
		 tokens, fileName, line);
    } else if (!cmd->cmp("mapUnknownCharNames")) {
      parseYesNo("mapUnknownCharNames", &mapUnknownCharNames,
		 tokens, fileName, line);
    } else if (!cmd->cmp("mapExtTrueTypeFontsViaUnicode")) {
      parseYesNo("mapExtTrueTypeFontsViaUnicode",
		 &mapExtTrueTypeFontsViaUnicode,
		 tokens, fileName, line);
    } else if (!cmd->cmp("dropFont")) {
      parseDropFont(tokens, fileName, line);
    } else if (!cmd->cmp("enableXFA")) {
      parseYesNo("enableXFA", &enableXFA, tokens, fileName, line);
    } else if (!cmd->cmp("bind")) {
      parseBind(tokens, fileName, line);
    } else if (!cmd->cmp("unbind")) {
      parseUnbind(tokens, fileName, line);
    } else if (!cmd->cmp("popupMenuCmd")) {
      parsePopupMenuCmd(tokens, fileName, line);
    } else if (!cmd->cmp("tabStateFile")) {
      parseString("tabStateFile", &tabStateFile, tokens, fileName, line);
    } else if (!cmd->cmp("printCommands")) {
      parseYesNo("printCommands", &printCommands, tokens, fileName, line);
    } else if (!cmd->cmp("errQuiet")) {
      parseYesNo("errQuiet", &errQuiet, tokens, fileName, line);
    } else if (!cmd->cmp("debugLogFile")) {
      parseString("debugLogFile", &debugLogFile, tokens, fileName, line);
    } else {
      error(errConfig, -1, "Unknown config file command '{0:t}' ({1:t}:{2:d})",
	    cmd, fileName, line);
      if (!cmd->cmp("displayFontX") ||
	  !cmd->cmp("displayNamedCIDFontX") ||
	  !cmd->cmp("displayCIDFontX")) {
	error(errConfig, -1, "Xpdf no longer supports X fonts");
      } else if (!cmd->cmp("enableT1lib")) {
	error(errConfig, -1, "Xpdf no longer uses t1lib");
      } else if (!cmd->cmp("t1libControl") || !cmd->cmp("freetypeControl")) {
	error(errConfig, -1,
	      "The t1libControl and freetypeControl options have been replaced by the enableT1lib, enableFreeType, and antialias options");
      } else if (!cmd->cmp("fontpath") || !cmd->cmp("fontmap")) {
	error(errConfig, -1,
	      "The config file format has changed since Xpdf 0.9x");
      }
    }
  }

  deleteGList(tokens, GString);
}

void GlobalParams::parseNameToUnicode(GList *tokens, GString *fileName,
				      int line) {
  GString *name;
  char *tok1, *tok2;
  FILE *f;
  char buf[256];
  int line2;
  Unicode u;

  if (tokens->getLength() != 2) {
    error(errConfig, -1,
	  "Bad 'nameToUnicode' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  name = (GString *)tokens->get(1);
  if (!(f = openFile(name->getCString(), "r"))) {
    error(errConfig, -1, "Couldn't open 'nameToUnicode' file '{0:t}'", name);
    return;
  }
  line2 = 1;
  while (getLine(buf, sizeof(buf), f)) {
    tok1 = strtok(buf, " \t\r\n");
    tok2 = strtok(NULL, " \t\r\n");
    if (tok1 && tok2) {
      sscanf(tok1, "%x", &u);
      nameToUnicode->add(tok2, u);
    } else {
      error(errConfig, -1, "Bad line in 'nameToUnicode' file ({0:t}:{1:d})",
	    name, line2);
    }
    ++line2;
  }
  fclose(f);
}

void GlobalParams::parseCIDToUnicode(GList *tokens, GString *fileName,
				     int line) {
  GString *collection, *name, *old;

  if (tokens->getLength() != 3) {
    error(errConfig, -1,
	  "Bad 'cidToUnicode' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  collection = (GString *)tokens->get(1);
  name = (GString *)tokens->get(2);
  if ((old = (GString *)cidToUnicodes->remove(collection))) {
    delete old;
  }
  cidToUnicodes->add(collection->copy(), name->copy());
}

void GlobalParams::parseUnicodeToUnicode(GList *tokens, GString *fileName,
					 int line) {
  GString *font, *file, *old;

  if (tokens->getLength() != 3) {
    error(errConfig, -1,
	  "Bad 'unicodeToUnicode' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  font = (GString *)tokens->get(1);
  file = (GString *)tokens->get(2);
  if ((old = (GString *)unicodeToUnicodes->remove(font))) {
    delete old;
  }
  unicodeToUnicodes->add(font->copy(), file->copy());
}

void GlobalParams::parseUnicodeMap(GList *tokens, GString *fileName,
				   int line) {
  GString *encodingName, *name, *old;

  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'unicodeMap' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  encodingName = (GString *)tokens->get(1);
  name = (GString *)tokens->get(2);
  if ((old = (GString *)unicodeMaps->remove(encodingName))) {
    delete old;
  }
  unicodeMaps->add(encodingName->copy(), name->copy());
}

void GlobalParams::parseCMapDir(GList *tokens, GString *fileName, int line) {
  GString *collection, *dir;
  GList *list;

  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'cMapDir' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  collection = (GString *)tokens->get(1);
  dir = (GString *)tokens->get(2);
  if (!(list = (GList *)cMapDirs->lookup(collection))) {
    list = new GList();
    cMapDirs->add(collection->copy(), list);
  }
  list->append(dir->copy());
}

void GlobalParams::parseToUnicodeDir(GList *tokens, GString *fileName,
				     int line) {
  if (tokens->getLength() != 2) {
    error(errConfig, -1,
	  "Bad 'toUnicodeDir' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  toUnicodeDirs->append(((GString *)tokens->get(1))->copy());
}

void GlobalParams::parseUnicodeRemapping(GList *tokens, GString *fileName,
					 int line) {
  if (tokens->getLength() != 2) {
    error(errConfig, -1,
	  "Bad 'unicodeRemapping' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  unicodeRemapping->parseFile((GString *)tokens->get(1));
}

void GlobalParams::parseFontFile(GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'fontFile' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  fontFiles->add(((GString *)tokens->get(1))->copy(),
		 ((GString *)tokens->get(2))->copy());
}

void GlobalParams::parseFontDir(GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad 'fontDir' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  fontDirs->append(((GString *)tokens->get(1))->copy());
}

void GlobalParams::parseFontFileCC(GList *tokens, GString *fileName,
				   int line) {
  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'fontFileCC' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  ccFontFiles->add(((GString *)tokens->get(1))->copy(),
		   ((GString *)tokens->get(2))->copy());
}

void GlobalParams::parsePSPaperSize(GList *tokens, GString *fileName,
				    int line) {
  GString *tok;

  if (tokens->getLength() == 2) {
    tok = (GString *)tokens->get(1);
    if (!setPSPaperSize(tok->getCString())) {
      error(errConfig, -1,
	    "Bad 'psPaperSize' config file command ({0:s}:{1:d})",
	    fileName, line);
    }
  } else if (tokens->getLength() == 3) {
    tok = (GString *)tokens->get(1);
    psPaperWidth = atoi(tok->getCString());
    tok = (GString *)tokens->get(2);
    psPaperHeight = atoi(tok->getCString());
    psImageableLLX = psImageableLLY = 0;
    psImageableURX = psPaperWidth;
    psImageableURY = psPaperHeight;
  } else {
    error(errConfig, -1, "Bad 'psPaperSize' config file command ({0:t}:{1:d})",
	  fileName, line);
  }
}

void GlobalParams::parsePSImageableArea(GList *tokens, GString *fileName,
					int line) {
  if (tokens->getLength() != 5) {
    error(errConfig, -1,
	  "Bad 'psImageableArea' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  psImageableLLX = atoi(((GString *)tokens->get(1))->getCString());
  psImageableLLY = atoi(((GString *)tokens->get(2))->getCString());
  psImageableURX = atoi(((GString *)tokens->get(3))->getCString());
  psImageableURY = atoi(((GString *)tokens->get(4))->getCString());
}

void GlobalParams::parsePSLevel(GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad 'psLevel' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("level1")) {
    psLevel = psLevel1;
  } else if (!tok->cmp("level1sep")) {
    psLevel = psLevel1Sep;
  } else if (!tok->cmp("level2")) {
    psLevel = psLevel2;
  } else if (!tok->cmp("level2gray")) {
    psLevel = psLevel2Gray;
  } else if (!tok->cmp("level2sep")) {
    psLevel = psLevel2Sep;
  } else if (!tok->cmp("level3")) {
    psLevel = psLevel3;
  } else if (!tok->cmp("level3gray")) {
    psLevel = psLevel3Gray;
  } else if (!tok->cmp("level3Sep")) {
    psLevel = psLevel3Sep;
  } else {
    error(errConfig, -1, "Bad 'psLevel' config file command ({0:t}:{1:d})",
	  fileName, line);
  }
}

void GlobalParams::parsePSResidentFont(GList *tokens, GString *fileName,
				       int line) {
  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'psResidentFont' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  psResidentFonts->add(((GString *)tokens->get(1))->copy(),
		       ((GString *)tokens->get(2))->copy());
}

void GlobalParams::parsePSResidentFont16(GList *tokens, GString *fileName,
					 int line) {
  PSFontParam16 *param;
  int wMode;
  GString *tok;

  if (tokens->getLength() != 5) {
    error(errConfig, -1, "Bad 'psResidentFont16' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(2);
  if (!tok->cmp("H")) {
    wMode = 0;
  } else if (!tok->cmp("V")) {
    wMode = 1;
  } else {
    error(errConfig, -1, "Bad wMode in psResidentFont16 config file command ({1:t}:{2:d})",
	  fileName, line);
    return;
  }
  param = new PSFontParam16(((GString *)tokens->get(1))->copy(),
			    wMode,
			    ((GString *)tokens->get(3))->copy(),
			    ((GString *)tokens->get(4))->copy());
  psResidentFonts16->append(param);
}

void GlobalParams::parsePSResidentFontCC(GList *tokens, GString *fileName,
					 int line) {
  PSFontParam16 *param;
  int wMode;
  GString *tok;

  if (tokens->getLength() != 5) {
    error(errConfig, -1, "Bad 'psResidentFontCC' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(2);
  if (!tok->cmp("H")) {
    wMode = 0;
  } else if (!tok->cmp("V")) {
    wMode = 1;
  } else {
    error(errConfig, -1, "Bad wMode in psResidentFontCC config file command ({1:t}:{2:d})",
	  fileName, line);
    return;
  }
  param = new PSFontParam16(((GString *)tokens->get(1))->copy(),
			    wMode,
			    ((GString *)tokens->get(3))->copy(),
			    ((GString *)tokens->get(4))->copy());
  psResidentFontsCC->append(param);
}

void GlobalParams::parseTextEOL(GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad 'textEOL' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("unix")) {
    textEOL = eolUnix;
  } else if (!tok->cmp("dos")) {
    textEOL = eolDOS;
  } else if (!tok->cmp("mac")) {
    textEOL = eolMac;
  } else {
    error(errConfig, -1, "Bad 'textEOL' config file command ({0:t}:{1:d})",
	  fileName, line);
  }
}

void GlobalParams::parseStrokeAdjust(GList *tokens, GString *fileName,
				     int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(errConfig, -1,
	  "Bad 'strokeAdjust' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("no")) {
    strokeAdjust = strokeAdjustOff;
  } else if (!tok->cmp("yes")) {
    strokeAdjust = strokeAdjustNormal;
  } else if (!tok->cmp("cad")) {
    strokeAdjust = strokeAdjustCAD;
  } else {
    error(errConfig, -1,
	  "Bad 'strokeAdjust' config file command ({0:t}:{1:d})",
	  fileName, line);
  }
}

void GlobalParams::parseScreenType(GList *tokens, GString *fileName,
				   int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad 'screenType' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("dispersed")) {
    screenType = screenDispersed;
  } else if (!tok->cmp("clustered")) {
    screenType = screenClustered;
  } else if (!tok->cmp("stochasticClustered")) {
    screenType = screenStochasticClustered;
  } else {
    error(errConfig, -1, "Bad 'screenType' config file command ({0:t}:{1:d})",
	  fileName, line);
  }
}

void GlobalParams::parseDropFont(GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad 'dropFont' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  droppedFonts->add(((GString *)tokens->get(1))->copy(), 1);
}

void GlobalParams::parseBind(GList *tokens, GString *fileName, int line) {
  KeyBinding *binding;
  GList *cmds;
  int code, mods, context, i;

  if (tokens->getLength() < 4) {
    error(errConfig, -1, "Bad 'bind' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  if (!parseKey((GString *)tokens->get(1), (GString *)tokens->get(2),
		&code, &mods, &context,
		"bind", tokens, fileName, line)) {
    return;
  }
  for (i = 0; i < keyBindings->getLength(); ++i) {
    binding = (KeyBinding *)keyBindings->get(i);
    if (binding->code == code &&
	binding->mods == mods &&
	binding->context == context) {
      delete (KeyBinding *)keyBindings->del(i);
      break;
    }
  }
  cmds = new GList();
  for (i = 3; i < tokens->getLength(); ++i) {
    cmds->append(((GString *)tokens->get(i))->copy());
  }
  keyBindings->append(new KeyBinding(code, mods, context, cmds));
}

void GlobalParams::parseUnbind(GList *tokens, GString *fileName, int line) {
  KeyBinding *binding;
  int code, mods, context, i;

  if (tokens->getLength() != 3) {
    error(errConfig, -1, "Bad 'unbind' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  if (!parseKey((GString *)tokens->get(1), (GString *)tokens->get(2),
		&code, &mods, &context,
		"unbind", tokens, fileName, line)) {
    return;
  }
  for (i = 0; i < keyBindings->getLength(); ++i) {
    binding = (KeyBinding *)keyBindings->get(i);
    if (binding->code == code &&
	binding->mods == mods &&
	binding->context == context) {
      delete (KeyBinding *)keyBindings->del(i);
      break;
    }
  }
}

GBool GlobalParams::parseKey(GString *modKeyStr, GString *contextStr,
			     int *code, int *mods, int *context,
			     const char *cmdName,
			     GList *tokens, GString *fileName, int line) {
  char *p0;
  int btn;

  *mods = xpdfKeyModNone;
  p0 = modKeyStr->getCString();
  while (1) {
    if (!strncmp(p0, "shift-", 6)) {
      *mods |= xpdfKeyModShift;
      p0 += 6;
    } else if (!strncmp(p0, "ctrl-", 5)) {
      *mods |= xpdfKeyModCtrl;
      p0 += 5;
    } else if (!strncmp(p0, "alt-", 4)) {
      *mods |= xpdfKeyModAlt;
      p0 += 4;
    } else {
      break;
    }
  }

  if (!strcmp(p0, "space")) {
    *code = ' ';
  } else if (!strcmp(p0, "tab")) {
    *code = xpdfKeyCodeTab;
  } else if (!strcmp(p0, "return")) {
    *code = xpdfKeyCodeReturn;
  } else if (!strcmp(p0, "enter")) {
    *code = xpdfKeyCodeEnter;
  } else if (!strcmp(p0, "backspace")) {
    *code = xpdfKeyCodeBackspace;
  } else if (!strcmp(p0, "insert")) {
    *code = xpdfKeyCodeInsert;
  } else if (!strcmp(p0, "delete")) {
    *code = xpdfKeyCodeDelete;
  } else if (!strcmp(p0, "home")) {
    *code = xpdfKeyCodeHome;
  } else if (!strcmp(p0, "end")) {
    *code = xpdfKeyCodeEnd;
  } else if (!strcmp(p0, "pgup")) {
    *code = xpdfKeyCodePgUp;
  } else if (!strcmp(p0, "pgdn")) {
    *code = xpdfKeyCodePgDn;
  } else if (!strcmp(p0, "left")) {
    *code = xpdfKeyCodeLeft;
  } else if (!strcmp(p0, "right")) {
    *code = xpdfKeyCodeRight;
  } else if (!strcmp(p0, "up")) {
    *code = xpdfKeyCodeUp;
  } else if (!strcmp(p0, "down")) {
    *code = xpdfKeyCodeDown;
  } else if (!strcmp(p0, "esc")) {
    *code = xpdfKeyCodeEsc;
  } else if (p0[0] == 'f' && p0[1] >= '1' && p0[1] <= '9' && !p0[2]) {
    *code = xpdfKeyCodeF1 + (p0[1] - '1');
  } else if (p0[0] == 'f' &&
	     ((p0[1] >= '1' && p0[1] <= '2' && p0[2] >= '0' && p0[2] <= '9') ||
	      (p0[1] == '3' && p0[2] >= '0' && p0[2] <= '5')) &&
	     !p0[3]) {
    *code = xpdfKeyCodeF1 + 10 * (p0[1] - '0') + (p0[2] - '0') - 1;
  } else if (!strncmp(p0, "mousePress", 10) &&
	     p0[10] >= '0' && p0[10] <= '9' &&
	     (!p0[11] || (p0[11] >= '0' && p0[11] <= '9' && !p0[12])) &&
	     (btn = atoi(p0 + 10)) >= 1 && btn <= 32) {
    *code = xpdfKeyCodeMousePress1 + btn - 1;
  } else if (!strncmp(p0, "mouseRelease", 12) &&
	     p0[12] >= '0' && p0[12] <= '9' &&
	     (!p0[13] || (p0[13] >= '0' && p0[13] <= '9' && !p0[14])) &&
	     (btn = atoi(p0 + 12)) >= 1 && btn <= 32) {
    *code = xpdfKeyCodeMouseRelease1 + btn - 1;
  } else if (!strncmp(p0, "mouseClick", 10) &&
	     p0[10] >= '0' && p0[10] <= '9' &&
	     (!p0[11] || (p0[11] >= '0' && p0[11] <= '9' && !p0[12])) &&
	     (btn = atoi(p0 + 10)) >= 1 && btn <= 32) {
    *code = xpdfKeyCodeMouseClick1 + btn - 1;
  } else if (*p0 >= 0x20 && *p0 <= 0x7e && !p0[1]) {
    *code = (int)*p0;
  } else {
    error(errConfig, -1,
	  "Bad key/modifier in '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return gFalse;
  }

  p0 = contextStr->getCString();
  if (!strcmp(p0, "any")) {
    *context = xpdfKeyContextAny;
  } else {
    *context = xpdfKeyContextAny;
    while (1) {
      if (!strncmp(p0, "fullScreen", 10)) {
	*context |= xpdfKeyContextFullScreen;
	p0 += 10;
      } else if (!strncmp(p0, "window", 6)) {
	*context |= xpdfKeyContextWindow;
	p0 += 6;
      } else if (!strncmp(p0, "continuous", 10)) {
	*context |= xpdfKeyContextContinuous;
	p0 += 10;
      } else if (!strncmp(p0, "singlePage", 10)) {
	*context |= xpdfKeyContextSinglePage;
	p0 += 10;
      } else if (!strncmp(p0, "overLink", 8)) {
	*context |= xpdfKeyContextOverLink;
	p0 += 8;
      } else if (!strncmp(p0, "offLink", 7)) {
	*context |= xpdfKeyContextOffLink;
	p0 += 7;
      } else if (!strncmp(p0, "outline", 7)) {
	*context |= xpdfKeyContextOutline;
	p0 += 7;
      } else if (!strncmp(p0, "mainWin", 7)) {
	*context |= xpdfKeyContextMainWin;
	p0 += 7;
      } else if (!strncmp(p0, "scrLockOn", 9)) {
	*context |= xpdfKeyContextScrLockOn;
	p0 += 9;
      } else if (!strncmp(p0, "scrLockOff", 10)) {
	*context |= xpdfKeyContextScrLockOff;
	p0 += 10;
      } else {
	error(errConfig, -1,
	      "Bad context in '{0:s}' config file command ({1:t}:{2:d})",
	      cmdName, fileName, line);
	return gFalse;
      }
      if (!*p0) {
	break;
      }
      if (*p0 != ',') {
	error(errConfig, -1,
	      "Bad context in '{0:s}' config file command ({1:t}:{2:d})",
	      cmdName, fileName, line);
	return gFalse;
      }
      ++p0;
    }
  }

  return gTrue;
}

void GlobalParams::parsePopupMenuCmd(GList *tokens,
				     GString *fileName, int line) {
  GList *cmds;
  int i;

  if (tokens->getLength() < 3) {
    error(errConfig, -1,
	  "Bad 'popupMenuCmd' config file command ({0:t}:{1:d})",
	  fileName, line);
    return;
  }
  cmds = new GList();
  for (i = 2; i < tokens->getLength(); ++i) {
    cmds->append(((GString *)tokens->get(i))->copy());
  }
  popupMenuCmds->append(new PopupMenuCmd(((GString *)tokens->get(1))->copy(),
					 cmds));
}

void GlobalParams::parseYesNo(const char *cmdName, GBool *flag,
			      GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!parseYesNo2(tok->getCString(), flag)) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
  }
}

GBool GlobalParams::parseYesNo2(char *token, GBool *flag) {
  if (!strcmp(token, "yes")) {
    *flag = gTrue;
  } else if (!strcmp(token, "no")) {
    *flag = gFalse;
  } else {
    return gFalse;
  }
  return gTrue;
}

void GlobalParams::parseString(const char *cmdName, GString **s,
			       GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  if (*s) {
    delete *s;
  }
  *s = ((GString *)tokens->get(1))->copy();
}

void GlobalParams::parseInteger(const char *cmdName, int *val,
				GList *tokens, GString *fileName, int line) {
  GString *tok;
  int i;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (tok->getLength() == 0) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  if (tok->getChar(0) == '-') {
    i = 1;
  } else {
    i = 0;
  }
  for (; i < tok->getLength(); ++i) {
    if (tok->getChar(i) < '0' || tok->getChar(i) > '9') {
      error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	    cmdName, fileName, line);
      return;
    }
  }
  *val = atoi(tok->getCString());
}

void GlobalParams::parseFloat(const char *cmdName, double *val,
			      GList *tokens, GString *fileName, int line) {
  GString *tok;
  int i;

  if (tokens->getLength() != 2) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (tok->getLength() == 0) {
    error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	  cmdName, fileName, line);
    return;
  }
  if (tok->getChar(0) == '-') {
    i = 1;
  } else {
    i = 0;
  }
  for (; i < tok->getLength(); ++i) {
    if (!((tok->getChar(i) >= '0' && tok->getChar(i) <= '9') ||
	  tok->getChar(i) == '.')) {
      error(errConfig, -1, "Bad '{0:s}' config file command ({1:t}:{2:d})",
	    cmdName, fileName, line);
      return;
    }
  }
  *val = atof(tok->getCString());
}

GlobalParams::~GlobalParams() {
  GHashIter *iter;
  GString *key;
  GList *list;

  freeBuiltinFontTables();

  delete macRomanReverseMap;

  delete baseDir;
  delete nameToUnicode;
  deleteGHash(cidToUnicodes, GString);
  deleteGHash(unicodeToUnicodes, GString);
  deleteGHash(residentUnicodeMaps, UnicodeMap);
  deleteGHash(unicodeMaps, GString);
  deleteGList(toUnicodeDirs, GString);
  delete unicodeRemapping;
  deleteGHash(fontFiles, GString);
  deleteGList(fontDirs, GString);
  deleteGHash(ccFontFiles, GString);
  deleteGHash(base14SysFonts, Base14FontInfo);
  delete sysFonts;
  deleteGHash(psResidentFonts, GString);
  deleteGList(psResidentFonts16, PSFontParam16);
  deleteGList(psResidentFontsCC, PSFontParam16);
  delete textEncoding;
  delete initialZoom;
  delete initialDisplayMode;
  delete initialSelectMode;
  if (paperColor) {
    delete paperColor;
  }
  if (matteColor) {
    delete matteColor;
  }
  if (fullScreenMatteColor) {
    delete fullScreenMatteColor;
  }
  if (launchCommand) {
    delete launchCommand;
  }
  if (movieCommand) {
    delete movieCommand;
  }
  if (defaultPrinter) {
    delete defaultPrinter;
  }
  delete droppedFonts;
  deleteGList(keyBindings, KeyBinding);
  deleteGList(popupMenuCmds, PopupMenuCmd);
  delete tabStateFile;

  cMapDirs->startIter(&iter);
  while (cMapDirs->getNext(&iter, &key, (void **)&list)) {
    deleteGList(list, GString);
  }
  delete cMapDirs;

  delete cidToUnicodeCache;
  delete unicodeToUnicodeCache;
  delete unicodeMapCache;
  delete cMapCache;

#if MULTITHREADED
  gDestroyMutex(&mutex);
  gDestroyMutex(&unicodeMapCacheMutex);
  gDestroyMutex(&cMapCacheMutex);
#endif
}

//------------------------------------------------------------------------

void GlobalParams::setBaseDir(const char *dir) {
  delete baseDir;
  baseDir = new GString(dir);
}

#ifdef _WIN32
static void getWinFontDir(char *winFontDir) {
  HMODULE shell32Lib;
  BOOL (__stdcall *SHGetSpecialFolderPathFunc)(HWND hwndOwner,
					       LPSTR lpszPath,
					       int nFolder,
					       BOOL fCreate);
  char *p;
  int i;

  // SHGetSpecialFolderPath isn't available in older versions of
  // shell32.dll (Win95 and WinNT4), so do a dynamic load
  winFontDir[0] = '\0';
  if ((shell32Lib = LoadLibraryA("shell32.dll"))) {
    if ((SHGetSpecialFolderPathFunc = 
	 (BOOL (__stdcall *)(HWND hwndOwner, LPSTR lpszPath,
			     int nFolder, BOOL fCreate))
	 GetProcAddress(shell32Lib, "SHGetSpecialFolderPathA"))) {
      if (!(*SHGetSpecialFolderPathFunc)(NULL, winFontDir,
					 CSIDL_FONTS, FALSE)) {
	winFontDir[0] = '\0';
      }
      // kludge: Terminal Server changes CSIDL_FONTS to something like
      // "C:\Users\whatever\Windows\Fonts", which doesn't actually
      // contain any fonts -- kill that, so we hit the fallback code
      // below.
      for (p = winFontDir; *p; ++p) {
	if (!strncasecmp(p, "\\Users\\", 7)) {
	  winFontDir[0] = '\0';
	  break;
	}
      }
    }
    FreeLibrary(shell32Lib);
  }
  // if something went wrong, or we're on a Terminal Server, try using
  // %SYSTEMROOT%\Fonts
  if (!winFontDir[0]) {
    GetSystemWindowsDirectoryA(winFontDir, MAX_PATH - 6);
    winFontDir[MAX_PATH - 7] = '\0';
    i = (int)strlen(winFontDir);
    if (winFontDir[i-1] != '\\') {
      winFontDir[i++] = '\\';
    }
    strcpy(winFontDir + i, "Fonts");
  }
}
#endif

#ifdef __APPLE__
// Apple dfonts and ttc fonts seem to randomly and interchangeably use
// space and hyphen, and sometimes drop the separator entirely.
static GBool macFontNameMatches(GString *name1, const char *name2) {
  const char *p1, *p2;
  char c1, c2;

  p1 = name1->getCString();
  p2 = name2;
  while (*p1 && *p2) {
    c1 = *p1;
    c2 = *p2;
    if (c2 == ' ') {
      // * space or hyphen matches a space in the pattern
      // * separators can also be dropped, in which case we move to
      //   the next char in the pattern
      if (c1 == ' ' || c1 == '-') {
	++p1;
      }
    } else {
      if (c1 != c2) {
	return gFalse;
      }
      ++p1;
    }
    ++p2;
  }
  if (*p1 || *p2) {
    return gFalse;
  }
  return gTrue;
}
#endif

void GlobalParams::setupBaseFonts(const char *dir) {
  GString *fontName;
  GString *fileName;
  int fontNum;
  const char *s;
  Base14FontInfo *base14;
#ifdef _WIN32
  char winFontDir[MAX_PATH];
#endif
#ifdef __APPLE__
  static const char *macFontExts[3] = { "dfont", "ttc", "ttf" };
  GList *dfontFontNames;
  GBool found;
  int k;
#endif
  FILE *f;
  int i, j;

#ifdef _WIN32
  getWinFontDir(winFontDir);
#endif
#ifdef __APPLE__
  dfontFontNames = NULL;
#endif
  for (i = 0; displayFontTab[i].name; ++i) {
    if (fontFiles->lookup(displayFontTab[i].name)) {
      continue;
    }
    fontName = new GString(displayFontTab[i].name);
    fileName = NULL;
    fontNum = 0;
    if (dir) {
      fileName = appendToPath(new GString(dir), displayFontTab[i].t1FileName);
      if ((f = fopen(fileName->getCString(), "rb"))) {
	fclose(f);
      } else {
	delete fileName;
	fileName = NULL;
      }
    }
#ifdef _WIN32
    if (!fileName && winFontDir[0] && displayFontTab[i].ttFileName) {
      fileName = appendToPath(new GString(winFontDir),
			      displayFontTab[i].ttFileName);
      if ((f = fopen(fileName->getCString(), "rb"))) {
	fclose(f);
      } else {
	delete fileName;
	fileName = NULL;
      }
    }
#endif
#ifdef __APPLE__
    // Check for Mac OS X system fonts.
    s = displayFontTab[i].macFileName;
    if (dfontFontNames && i > 0 &&
	(!s || strcmp(s, displayFontTab[i-1].macFileName))) {
      deleteGList(dfontFontNames, GString);
      dfontFontNames = NULL;
    }
    if (!fileName && s) {
      for (j = 0; j < 3; ++j) {
	fileName = GString::format("{0:s}/{1:s}.{2:s}",
				   macSystemFontPath, s, macFontExts[j]);
	if (!(f = fopen(fileName->getCString(), "rb"))) {
	  delete fileName;
	  fileName = NULL;
	} else {
	  fclose(f);
	  found = gFalse;
	  // for .dfont or .ttc, we need to scan the font list
	  if (j < 2) {
	    if (!dfontFontNames) {
	      dfontFontNames =
	          FoFiIdentifier::getFontList(fileName->getCString());
	    }
	    if (dfontFontNames) {
	      for (k = 0; k < dfontFontNames->getLength(); ++k) {
		if (macFontNameMatches((GString *)dfontFontNames->get(k),
				       displayFontTab[i].macFontName)) {
		  fontNum = k;
		  found = gTrue;
		  break;
		}
	      }
	    }
	  // for .ttf, we just use the font
	  } else {
	    found = gTrue;
	  }
	  if (!found) {
	    delete fileName;
	    fileName = NULL;
	  }
	  break;
	}
      }
    }
#endif // __APPLE__
    // On Linux, this checks the "standard" ghostscript font
    // directories.  On Windows, it checks the "standard" system font
    // directories (because SHGetSpecialFolderPath(CSIDL_FONTS)
    // doesn't work on Win 2k Server or Win2003 Server, or with older
    // versions of shell32.dll).
#ifdef _WIN32
    s = displayFontTab[i].ttFileName;
#else
    s = displayFontTab[i].t1FileName;
#endif
    if (!fileName && s) {
      for (j = 0; !fileName && displayFontDirs[j]; ++j) {
	fileName = appendToPath(new GString(displayFontDirs[j]), s);
	if ((f = fopen(fileName->getCString(), "rb"))) {
	  fclose(f);
	} else {
	  delete fileName;
	  fileName = NULL;
	}
      }
    }
    if (!fileName) {
      delete fontName;
      continue;
    }
    base14SysFonts->add(fontName, new Base14FontInfo(fileName, fontNum, 0));
  }
#ifdef __APPLE__
  if (dfontFontNames) {
    deleteGList(dfontFontNames, GString);
  }
#endif
  for (i = 0; displayFontTab[i].name; ++i) {
    if (!base14SysFonts->lookup(displayFontTab[i].name) &&
	!fontFiles->lookup(displayFontTab[i].name)) {
      if (displayFontTab[i].obliqueFont &&
	  ((base14 = (Base14FontInfo *)base14SysFonts
	                 ->lookup(displayFontTab[i].obliqueFont)))) {
	base14SysFonts->add(
	        new GString(displayFontTab[i].name),
		new Base14FontInfo(base14->fileName->copy(),
				   base14->fontNum,
				   displayFontTab[i].obliqueFactor));
      } else {
// Do not display unnecessary looking message on W32
#ifndef _WIN32
	error(errConfig, -1, "No display font for '{0:s}'",
	      displayFontTab[i].name);
#endif
      }
    }
  }
#ifdef _WIN32
  if (winFontDir[0]) {
    sysFonts->scanWindowsFonts(winFontDir);
  }
#endif
#if HAVE_FONTCONFIG
  sysFonts->scanFontconfigFonts();
#endif
}

//------------------------------------------------------------------------
// accessors
//------------------------------------------------------------------------

CharCode GlobalParams::getMacRomanCharCode(char *charName) {
  // no need to lock - macRomanReverseMap is constant
  return macRomanReverseMap->lookup(charName);
}

GString *GlobalParams::getBaseDir() {
  GString *s;

  lockGlobalParams;
  s = baseDir->copy();
  unlockGlobalParams;
  return s;
}

Unicode GlobalParams::mapNameToUnicode(const char *charName) {
  // no need to lock - nameToUnicode is constant
  return nameToUnicode->lookup(charName);
}

UnicodeMap *GlobalParams::getResidentUnicodeMap(GString *encodingName) {
  UnicodeMap *map;

  lockGlobalParams;
  map = (UnicodeMap *)residentUnicodeMaps->lookup(encodingName);
  unlockGlobalParams;
  if (map) {
    map->incRefCnt();
  }
  return map;
}

FILE *GlobalParams::getUnicodeMapFile(GString *encodingName) {
  GString *fileName;
  FILE *f;

  lockGlobalParams;
  if ((fileName = (GString *)unicodeMaps->lookup(encodingName))) {
    f = openFile(fileName->getCString(), "r");
  } else {
    f = NULL;
  }
  unlockGlobalParams;
  return f;
}

FILE *GlobalParams::findCMapFile(GString *collection, GString *cMapName) {
  GList *list;
  GString *dir;
  GString *fileName;
  FILE *f;
  int i;

  lockGlobalParams;
  if (!(list = (GList *)cMapDirs->lookup(collection))) {
    unlockGlobalParams;
    return NULL;
  }
  for (i = 0; i < list->getLength(); ++i) {
    dir = (GString *)list->get(i);
    fileName = appendToPath(dir->copy(), cMapName->getCString());
    f = openFile(fileName->getCString(), "r");
    delete fileName;
    if (f) {
      unlockGlobalParams;
      return f;
    }
  }
  unlockGlobalParams;
  return NULL;
}

FILE *GlobalParams::findToUnicodeFile(GString *name) {
  GString *dir, *fileName;
  FILE *f;
  int i;

  lockGlobalParams;
  for (i = 0; i < toUnicodeDirs->getLength(); ++i) {
    dir = (GString *)toUnicodeDirs->get(i);
    fileName = appendToPath(dir->copy(), name->getCString());
    f = openFile(fileName->getCString(), "r");
    delete fileName;
    if (f) {
      unlockGlobalParams;
      return f;
    }
  }
  unlockGlobalParams;
  return NULL;
}

UnicodeRemapping *GlobalParams::getUnicodeRemapping() {
  return unicodeRemapping;
}

GString *GlobalParams::findFontFile(GString *fontName) {
  static const char *exts[] = { ".pfa", ".pfb", ".ttf", ".ttc", ".otf" };
  GString *path, *dir;
#ifdef _WIN32
  GString *fontNameU;
#endif
  const char *ext;
  FILE *f;
  int i, j;

  lockGlobalParams;
  if ((path = (GString *)fontFiles->lookup(fontName))) {
    path = path->copy();
    unlockGlobalParams;
    return path;
  }
  for (i = 0; i < fontDirs->getLength(); ++i) {
    dir = (GString *)fontDirs->get(i);
    for (j = 0; j < (int)(sizeof(exts) / sizeof(exts[0])); ++j) {
      ext = exts[j];
#ifdef _WIN32
      fontNameU = fileNameToUTF8(fontName->getCString());
      path = appendToPath(dir->copy(), fontNameU->getCString());
      delete fontNameU;
#else
      path = appendToPath(dir->copy(), fontName->getCString());
#endif
      path->append(ext);
      if ((f = openFile(path->getCString(), "rb"))) {
	fclose(f);
	unlockGlobalParams;
	return path;
      }
      delete path;
    }
  }
  unlockGlobalParams;
  return NULL;
}

GString *GlobalParams::findBase14FontFile(GString *fontName, int *fontNum,
					  double *oblique) {
  Base14FontInfo *fi;
  GString *path;

  lockGlobalParams;
  if ((fi = (Base14FontInfo *)base14SysFonts->lookup(fontName))) {
    path = fi->fileName->copy();
    *fontNum = fi->fontNum;
    *oblique = fi->oblique;
    unlockGlobalParams;
    return path;
  }
  unlockGlobalParams;
  *fontNum = 0;
  *oblique = 0;
  return findFontFile(fontName);
}

GString *GlobalParams::findSystemFontFile(GString *fontName,
					  SysFontType *type,
					  int *fontNum) {
  SysFontInfo *fi;
  GString *path;

  path = NULL;
  lockGlobalParams;
  if ((fi = sysFonts->find(fontName))) {
    path = fi->path->copy();
    *type = fi->type;
    *fontNum = fi->fontNum;
  }
  unlockGlobalParams;
  return path;
}

GString *GlobalParams::findCCFontFile(GString *collection) {
  GString *path;

  lockGlobalParams;
  if ((path = (GString *)ccFontFiles->lookup(collection))) {
    path = path->copy();
  }
  unlockGlobalParams;
  return path;
}

int GlobalParams::getPSPaperWidth() {
  int w;

  lockGlobalParams;
  w = psPaperWidth;
  unlockGlobalParams;
  return w;
}

int GlobalParams::getPSPaperHeight() {
  int h;

  lockGlobalParams;
  h = psPaperHeight;
  unlockGlobalParams;
  return h;
}

void GlobalParams::getPSImageableArea(int *llx, int *lly, int *urx, int *ury) {
  lockGlobalParams;
  *llx = psImageableLLX;
  *lly = psImageableLLY;
  *urx = psImageableURX;
  *ury = psImageableURY;
  unlockGlobalParams;
}

GBool GlobalParams::getPSCrop() {
  GBool f;

  lockGlobalParams;
  f = psCrop;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSUseCropBoxAsPage() {
  GBool f;

  lockGlobalParams;
  f = psUseCropBoxAsPage;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSExpandSmaller() {
  GBool f;

  lockGlobalParams;
  f = psExpandSmaller;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSShrinkLarger() {
  GBool f;

  lockGlobalParams;
  f = psShrinkLarger;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSCenter() {
  GBool f;

  lockGlobalParams;
  f = psCenter;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSDuplex() {
  GBool d;

  lockGlobalParams;
  d = psDuplex;
  unlockGlobalParams;
  return d;
}

PSLevel GlobalParams::getPSLevel() {
  PSLevel level;

  lockGlobalParams;
  level = psLevel;
  unlockGlobalParams;
  return level;
}

GString *GlobalParams::getPSResidentFont(GString *fontName) {
  GString *psName;

  lockGlobalParams;
  if ((psName = (GString *)psResidentFonts->lookup(fontName))) {
    psName = psName->copy();
  }
  unlockGlobalParams;
  return psName;
}

GList *GlobalParams::getPSResidentFonts() {
  GList *names;
  GHashIter *iter;
  GString *name;
  GString *psName;

  names = new GList();
  lockGlobalParams;
  psResidentFonts->startIter(&iter);
  while (psResidentFonts->getNext(&iter, &name, (void **)&psName)) {
    names->append(psName->copy());
  }
  unlockGlobalParams;
  return names;
}

PSFontParam16 *GlobalParams::getPSResidentFont16(GString *fontName,
						 int wMode) {
  PSFontParam16 *p;
  int i;

  lockGlobalParams;
  p = NULL;
  for (i = 0; i < psResidentFonts16->getLength(); ++i) {
    p = (PSFontParam16 *)psResidentFonts16->get(i);
    if (!(p->name->cmp(fontName)) && p->wMode == wMode) {
      break;
    }
    p = NULL;
  }
  unlockGlobalParams;
  return p;
}

PSFontParam16 *GlobalParams::getPSResidentFontCC(GString *collection,
						 int wMode) {
  PSFontParam16 *p;
  int i;

  lockGlobalParams;
  p = NULL;
  for (i = 0; i < psResidentFontsCC->getLength(); ++i) {
    p = (PSFontParam16 *)psResidentFontsCC->get(i);
    if (!(p->name->cmp(collection)) && p->wMode == wMode) {
      break;
    }
    p = NULL;
  }
  unlockGlobalParams;
  return p;
}

GBool GlobalParams::getPSEmbedType1() {
  GBool e;

  lockGlobalParams;
  e = psEmbedType1;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedTrueType() {
  GBool e;

  lockGlobalParams;
  e = psEmbedTrueType;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedCIDPostScript() {
  GBool e;

  lockGlobalParams;
  e = psEmbedCIDPostScript;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedCIDTrueType() {
  GBool e;

  lockGlobalParams;
  e = psEmbedCIDTrueType;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSFontPassthrough() {
  GBool e;

  lockGlobalParams;
  e = psFontPassthrough;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSPreload() {
  GBool preload;

  lockGlobalParams;
  preload = psPreload;
  unlockGlobalParams;
  return preload;
}

GBool GlobalParams::getPSOPI() {
  GBool opi;

  lockGlobalParams;
  opi = psOPI;
  unlockGlobalParams;
  return opi;
}

GBool GlobalParams::getPSASCIIHex() {
  GBool ah;

  lockGlobalParams;
  ah = psASCIIHex;
  unlockGlobalParams;
  return ah;
}

GBool GlobalParams::getPSLZW() {
  GBool ah;

  lockGlobalParams;
  ah = psLZW;
  unlockGlobalParams;
  return ah;
}

GBool GlobalParams::getPSUncompressPreloadedImages() {
  GBool ah;

  lockGlobalParams;
  ah = psUncompressPreloadedImages;
  unlockGlobalParams;
  return ah;
}

double GlobalParams::getPSMinLineWidth() {
  double w;

  lockGlobalParams;
  w = psMinLineWidth;
  unlockGlobalParams;
  return w;
}

double GlobalParams::getPSRasterResolution() {
  double res;

  lockGlobalParams;
  res = psRasterResolution;
  unlockGlobalParams;
  return res;
}

GBool GlobalParams::getPSRasterMono() {
  GBool mono;

  lockGlobalParams;
  mono = psRasterMono;
  unlockGlobalParams;
  return mono;
}

int GlobalParams::getPSRasterSliceSize() {
  int slice;

  lockGlobalParams;
  slice = psRasterSliceSize;
  unlockGlobalParams;
  return slice;
}

GBool GlobalParams::getPSAlwaysRasterize() {
  GBool rast;

  lockGlobalParams;
  rast = psAlwaysRasterize;
  unlockGlobalParams;
  return rast;
}

GBool GlobalParams::getPSNeverRasterize() {
  GBool rast;

  lockGlobalParams;
  rast = psNeverRasterize;
  unlockGlobalParams;
  return rast;
}

GString *GlobalParams::getTextEncodingName() {
  GString *s;

  lockGlobalParams;
  s = textEncoding->copy();
  unlockGlobalParams;
  return s;
}

EndOfLineKind GlobalParams::getTextEOL() {
  EndOfLineKind eol;

  lockGlobalParams;
  eol = textEOL;
  unlockGlobalParams;
  return eol;
}

GBool GlobalParams::getTextPageBreaks() {
  GBool pageBreaks;

  lockGlobalParams;
  pageBreaks = textPageBreaks;
  unlockGlobalParams;
  return pageBreaks;
}

GBool GlobalParams::getTextKeepTinyChars() {
  GBool tiny;

  lockGlobalParams;
  tiny = textKeepTinyChars;
  unlockGlobalParams;
  return tiny;
}

GString *GlobalParams::getInitialZoom() {
  GString *s;

  lockGlobalParams;
  s = initialZoom->copy();
  unlockGlobalParams;
  return s;
}

int GlobalParams::getDefaultFitZoom() {
  int z;

  lockGlobalParams;
  z = defaultFitZoom;
  unlockGlobalParams;
  return z;
}

GString *GlobalParams::getInitialDisplayMode() {
  GString *s;

  lockGlobalParams;
  s = initialDisplayMode->copy();
  unlockGlobalParams;
  return s;
}

GBool GlobalParams::getInitialToolbarState() {
  GBool state;

  lockGlobalParams;
  state = initialToolbarState;
  unlockGlobalParams;
  return state;
}

GBool GlobalParams::getInitialSidebarState() {
  GBool state;

  lockGlobalParams;
  state = initialSidebarState;
  unlockGlobalParams;
  return state;
}

GString *GlobalParams::getInitialSelectMode() {
  GString *s;

  lockGlobalParams;
  s = initialSelectMode->copy();
  unlockGlobalParams;
  return s;
}

int GlobalParams::getMaxTileWidth() {
  int w;

  lockGlobalParams;
  w = maxTileWidth;
  unlockGlobalParams;
  return w;
}

int GlobalParams::getMaxTileHeight() {
  int h;

  lockGlobalParams;
  h = maxTileHeight;
  unlockGlobalParams;
  return h;
}

int GlobalParams::getTileCacheSize() {
  int n;

  lockGlobalParams;
  n = tileCacheSize;
  unlockGlobalParams;
  return n;
}

int GlobalParams::getWorkerThreads() {
  int n;

  lockGlobalParams;
  n = workerThreads;
  unlockGlobalParams;
  return n;
}

GBool GlobalParams::getEnableFreeType() {
  GBool f;

  lockGlobalParams;
  f = enableFreeType;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getDisableFreeTypeHinting() {
  GBool f;

  lockGlobalParams;
  f = disableFreeTypeHinting;
  unlockGlobalParams;
  return f;
}


GBool GlobalParams::getAntialias() {
  GBool f;

  lockGlobalParams;
  f = antialias;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getVectorAntialias() {
  GBool f;

  lockGlobalParams;
  f = vectorAntialias;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getAntialiasPrinting() {
  GBool f;

  lockGlobalParams;
  f = antialiasPrinting;
  unlockGlobalParams;
  return f;
}

StrokeAdjustMode GlobalParams::getStrokeAdjust() {
  StrokeAdjustMode mode;

  lockGlobalParams;
  mode = strokeAdjust;
  unlockGlobalParams;
  return mode;
}

ScreenType GlobalParams::getScreenType() {
  ScreenType t;

  lockGlobalParams;
  t = screenType;
  unlockGlobalParams;
  return t;
}

int GlobalParams::getScreenSize() {
  int size;

  lockGlobalParams;
  size = screenSize;
  unlockGlobalParams;
  return size;
}

int GlobalParams::getScreenDotRadius() {
  int r;

  lockGlobalParams;
  r = screenDotRadius;
  unlockGlobalParams;
  return r;
}

double GlobalParams::getScreenGamma() {
  double gamma;

  lockGlobalParams;
  gamma = screenGamma;
  unlockGlobalParams;
  return gamma;
}

double GlobalParams::getScreenBlackThreshold() {
  double thresh;

  lockGlobalParams;
  thresh = screenBlackThreshold;
  unlockGlobalParams;
  return thresh;
}

double GlobalParams::getScreenWhiteThreshold() {
  double thresh;

  lockGlobalParams;
  thresh = screenWhiteThreshold;
  unlockGlobalParams;
  return thresh;
}

double GlobalParams::getMinLineWidth() {
  double w;

  lockGlobalParams;
  w = minLineWidth;
  unlockGlobalParams;
  return w;
}

GBool GlobalParams::getEnablePathSimplification() {
  GBool en;

  lockGlobalParams;
  en = enablePathSimplification;
  unlockGlobalParams;
  return en;
}

GBool GlobalParams::getDrawAnnotations() {
  GBool draw;

  lockGlobalParams;
  draw = drawAnnotations;
  unlockGlobalParams;
  return draw;
}

GBool GlobalParams::getDrawFormFields() {
  GBool draw;

  lockGlobalParams;
  draw = drawFormFields;
  unlockGlobalParams;
  return draw;
}



GString *GlobalParams::getPaperColor() {
  GString *s;

  lockGlobalParams;
  s = paperColor->copy();
  unlockGlobalParams;
  return s;
}

GString *GlobalParams::getMatteColor() {
  GString *s;

  lockGlobalParams;
  s = matteColor->copy();
  unlockGlobalParams;
  return s;
}

GString *GlobalParams::getFullScreenMatteColor() {
  GString *s;

  lockGlobalParams;
  s = fullScreenMatteColor->copy();
  unlockGlobalParams;
  return s;
}

GBool GlobalParams::getReverseVideoInvertImages() {
  GBool invert;

  lockGlobalParams;
  invert = reverseVideoInvertImages;
  unlockGlobalParams;
  return invert;
}

GString *GlobalParams::getDefaultPrinter() {
  GString *s;

  lockGlobalParams;
  s = defaultPrinter ? defaultPrinter->copy() : (GString *)NULL;
  unlockGlobalParams;
  return s;
}

GBool GlobalParams::getMapNumericCharNames() {
  GBool map;

  lockGlobalParams;
  map = mapNumericCharNames;
  unlockGlobalParams;
  return map;
}

GBool GlobalParams::getMapUnknownCharNames() {
  GBool map;

  lockGlobalParams;
  map = mapUnknownCharNames;
  unlockGlobalParams;
  return map;
}

GBool GlobalParams::getMapExtTrueTypeFontsViaUnicode() {
  GBool map;

  lockGlobalParams;
  map = mapExtTrueTypeFontsViaUnicode;
  unlockGlobalParams;
  return map;
}

GBool GlobalParams::isDroppedFont(const char *fontName) {
  GBool isDropped;

  lockGlobalParams;
  isDropped = droppedFonts->lookupInt(fontName) != 0;
  unlockGlobalParams;
  return isDropped;
}

GBool GlobalParams::getEnableXFA() {
  GBool enable;

  lockGlobalParams;
  enable = enableXFA;
  unlockGlobalParams;
  return enable;
}

GList *GlobalParams::getKeyBinding(int code, int mods, int context) {
  KeyBinding *binding;
  GList *cmds;
  int modMask;
  int i, j;

  lockGlobalParams;
  cmds = NULL;
  // for ASCII chars, ignore the shift modifier
  modMask = (code >= 0x21 && code <= 0xff) ? ~xpdfKeyModShift : ~0;
  for (i = 0; i < keyBindings->getLength(); ++i) {
    binding = (KeyBinding *)keyBindings->get(i);
    if (binding->code == code &&
	(binding->mods & modMask) == (mods & modMask) &&
	(~binding->context | context) == ~0) {
      cmds = new GList();
      for (j = 0; j < binding->cmds->getLength(); ++j) {
	cmds->append(((GString *)binding->cmds->get(j))->copy());
      }
      break;
    }
  }
  unlockGlobalParams;
  return cmds;
}

int GlobalParams::getNumPopupMenuCmds() {
  int n;

  lockGlobalParams;
  n = popupMenuCmds->getLength();
  unlockGlobalParams;
  return n;
}

PopupMenuCmd *GlobalParams::getPopupMenuCmd(int idx) {
  PopupMenuCmd *cmd;

  lockGlobalParams;
  if (idx < popupMenuCmds->getLength()) {
    cmd = (PopupMenuCmd *)popupMenuCmds->get(idx);
  } else {
    cmd = NULL;
  }
  unlockGlobalParams;
  return cmd;
}

GString *GlobalParams::getTabStateFile() {
  GString *s;

  lockGlobalParams;
  s = tabStateFile->copy();
  unlockGlobalParams;
  return s;
}

GBool GlobalParams::getPrintCommands() {
  GBool p;

  lockGlobalParams;
  p = printCommands;
  unlockGlobalParams;
  return p;
}

GBool GlobalParams::getErrQuiet() {
  // no locking -- this function may get called from inside a locked
  // section
  return errQuiet;
}

GString *GlobalParams::getDebugLogFile() {
  return debugLogFile;
}

void GlobalParams::debugLogPrintf(char *fmt, ...) {
  GString *path;
  FILE *f;
  GBool needClose;
  time_t t;
  struct tm tm;
  va_list args;

  if (!(path = getDebugLogFile())) {
    return;
  }
  needClose = gFalse;
  if (!path->cmp("-")) {
    f = stdout;
  } else if (!path->cmp("+")) {
    f = stderr;
  } else {
    f = fopen(path->getCString(), "a");
    needClose = gTrue;
  }
  if (!f) {
    return;
  }
  t = time(NULL);
#ifdef _WIN32
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif
  fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] ",
	  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
	  tm.tm_hour, tm.tm_min, tm.tm_sec);
  va_start(args, fmt);
  vfprintf(f, fmt, args);
  va_end(args);
  fflush(f);
  if (needClose) {
    fclose(f);
  }
}

CharCodeToUnicode *GlobalParams::getCIDToUnicode(GString *collection) {
  GString *fileName;
  CharCodeToUnicode *ctu;

  lockGlobalParams;
  if (!(ctu = cidToUnicodeCache->getCharCodeToUnicode(collection))) {
    if ((fileName = (GString *)cidToUnicodes->lookup(collection)) &&
	(ctu = CharCodeToUnicode::parseCIDToUnicode(fileName, collection))) {
      cidToUnicodeCache->add(ctu);
    }
  }
  unlockGlobalParams;
  return ctu;
}

CharCodeToUnicode *GlobalParams::getUnicodeToUnicode(GString *fontName) {
  CharCodeToUnicode *ctu;
  GHashIter *iter;
  GString *fontPattern, *fileName;

  lockGlobalParams;
  fileName = NULL;
  unicodeToUnicodes->startIter(&iter);
  while (unicodeToUnicodes->getNext(&iter, &fontPattern, (void **)&fileName)) {
    if (strstr(fontName->getCString(), fontPattern->getCString())) {
      unicodeToUnicodes->killIter(&iter);
      break;
    }
    fileName = NULL;
  }
  if (fileName) {
    if (!(ctu = unicodeToUnicodeCache->getCharCodeToUnicode(fileName))) {
      if ((ctu = CharCodeToUnicode::parseUnicodeToUnicode(fileName))) {
	unicodeToUnicodeCache->add(ctu);
      }
    }
  } else {
    ctu = NULL;
  }
  unlockGlobalParams;
  return ctu;
}

UnicodeMap *GlobalParams::getUnicodeMap(GString *encodingName) {
  return getUnicodeMap2(encodingName);
}

UnicodeMap *GlobalParams::getUnicodeMap2(GString *encodingName) {
  UnicodeMap *map;

  if (!(map = getResidentUnicodeMap(encodingName))) {
    lockUnicodeMapCache;
    map = unicodeMapCache->getUnicodeMap(encodingName);
    unlockUnicodeMapCache;
  }
  return map;
}

CMap *GlobalParams::getCMap(GString *collection, GString *cMapName) {
  CMap *cMap;

  lockCMapCache;
  cMap = cMapCache->getCMap(collection, cMapName);
  unlockCMapCache;
  return cMap;
}

UnicodeMap *GlobalParams::getTextEncoding() {
  return getUnicodeMap2(textEncoding);
}

//------------------------------------------------------------------------
// functions to set parameters
//------------------------------------------------------------------------

void GlobalParams::addUnicodeRemapping(Unicode in, Unicode *out, int len) {
  unicodeRemapping->addRemapping(in, out, len);
}

void GlobalParams::addFontFile(GString *fontName, GString *path) {
  lockGlobalParams;
  fontFiles->add(fontName, path);
  unlockGlobalParams;
}

GBool GlobalParams::setPSPaperSize(char *size) {
  lockGlobalParams;
  if (!strcmp(size, "match")) {
    psPaperWidth = psPaperHeight = -1;
  } else if (!strcmp(size, "letter")) {
    psPaperWidth = 612;
    psPaperHeight = 792;
  } else if (!strcmp(size, "legal")) {
    psPaperWidth = 612;
    psPaperHeight = 1008;
  } else if (!strcmp(size, "A4")) {
    psPaperWidth = 595;
    psPaperHeight = 842;
  } else if (!strcmp(size, "A3")) {
    psPaperWidth = 842;
    psPaperHeight = 1190;
  } else {
    unlockGlobalParams;
    return gFalse;
  }
  psImageableLLX = psImageableLLY = 0;
  psImageableURX = psPaperWidth;
  psImageableURY = psPaperHeight;
  unlockGlobalParams;
  return gTrue;
}

void GlobalParams::setPSPaperWidth(int width) {
  lockGlobalParams;
  psPaperWidth = width;
  psImageableLLX = 0;
  psImageableURX = psPaperWidth;
  unlockGlobalParams;
}

void GlobalParams::setPSPaperHeight(int height) {
  lockGlobalParams;
  psPaperHeight = height;
  psImageableLLY = 0;
  psImageableURY = psPaperHeight;
  unlockGlobalParams;
}

void GlobalParams::setPSImageableArea(int llx, int lly, int urx, int ury) {
  lockGlobalParams;
  psImageableLLX = llx;
  psImageableLLY = lly;
  psImageableURX = urx;
  psImageableURY = ury;
  unlockGlobalParams;
}

void GlobalParams::setPSCrop(GBool crop) {
  lockGlobalParams;
  psCrop = crop;
  unlockGlobalParams;
}

void GlobalParams::setPSUseCropBoxAsPage(GBool crop) {
  lockGlobalParams;
  psUseCropBoxAsPage = crop;
  unlockGlobalParams;
}

void GlobalParams::setPSExpandSmaller(GBool expand) {
  lockGlobalParams;
  psExpandSmaller = expand;
  unlockGlobalParams;
}

void GlobalParams::setPSShrinkLarger(GBool shrink) {
  lockGlobalParams;
  psShrinkLarger = shrink;
  unlockGlobalParams;
}

void GlobalParams::setPSCenter(GBool center) {
  lockGlobalParams;
  psCenter = center;
  unlockGlobalParams;
}

void GlobalParams::setPSDuplex(GBool duplex) {
  lockGlobalParams;
  psDuplex = duplex;
  unlockGlobalParams;
}

void GlobalParams::setPSLevel(PSLevel level) {
  lockGlobalParams;
  psLevel = level;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedType1(GBool embed) {
  lockGlobalParams;
  psEmbedType1 = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedTrueType(GBool embed) {
  lockGlobalParams;
  psEmbedTrueType = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedCIDPostScript(GBool embed) {
  lockGlobalParams;
  psEmbedCIDPostScript = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedCIDTrueType(GBool embed) {
  lockGlobalParams;
  psEmbedCIDTrueType = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSFontPassthrough(GBool passthrough) {
  lockGlobalParams;
  psFontPassthrough = passthrough;
  unlockGlobalParams;
}

void GlobalParams::setPSPreload(GBool preload) {
  lockGlobalParams;
  psPreload = preload;
  unlockGlobalParams;
}

void GlobalParams::setPSOPI(GBool opi) {
  lockGlobalParams;
  psOPI = opi;
  unlockGlobalParams;
}

void GlobalParams::setPSASCIIHex(GBool hex) {
  lockGlobalParams;
  psASCIIHex = hex;
  unlockGlobalParams;
}

void GlobalParams::setTextEncoding(const char *encodingName) {
  lockGlobalParams;
  delete textEncoding;
  textEncoding = new GString(encodingName);
  unlockGlobalParams;
}

GBool GlobalParams::setTextEOL(char *s) {
  lockGlobalParams;
  if (!strcmp(s, "unix")) {
    textEOL = eolUnix;
  } else if (!strcmp(s, "dos")) {
    textEOL = eolDOS;
  } else if (!strcmp(s, "mac")) {
    textEOL = eolMac;
  } else {
    unlockGlobalParams;
    return gFalse;
  }
  unlockGlobalParams;
  return gTrue;
}

void GlobalParams::setTextPageBreaks(GBool pageBreaks) {
  lockGlobalParams;
  textPageBreaks = pageBreaks;
  unlockGlobalParams;
}

void GlobalParams::setTextKeepTinyChars(GBool keep) {
  lockGlobalParams;
  textKeepTinyChars = keep;
  unlockGlobalParams;
}

void GlobalParams::setInitialZoom(char *s) {
  lockGlobalParams;
  delete initialZoom;
  initialZoom = new GString(s);
  unlockGlobalParams;
}

GBool GlobalParams::setEnableFreeType(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &enableFreeType);
  unlockGlobalParams;
  return ok;
}


GBool GlobalParams::setAntialias(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &antialias);
  unlockGlobalParams;
  return ok;
}

GBool GlobalParams::setVectorAntialias(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &vectorAntialias);
  unlockGlobalParams;
  return ok;
}

void GlobalParams::setScreenType(ScreenType t) {
  lockGlobalParams;
  screenType = t;
  unlockGlobalParams;
}

void GlobalParams::setScreenSize(int size) {
  lockGlobalParams;
  screenSize = size;
  unlockGlobalParams;
}

void GlobalParams::setScreenDotRadius(int r) {
  lockGlobalParams;
  screenDotRadius = r;
  unlockGlobalParams;
}

void GlobalParams::setScreenGamma(double gamma) {
  lockGlobalParams;
  screenGamma = gamma;
  unlockGlobalParams;
}

void GlobalParams::setScreenBlackThreshold(double thresh) {
  lockGlobalParams;
  screenBlackThreshold = thresh;
  unlockGlobalParams;
}

void GlobalParams::setScreenWhiteThreshold(double thresh) {
  lockGlobalParams;
  screenWhiteThreshold = thresh;
  unlockGlobalParams;
}

void GlobalParams::setDrawFormFields(GBool draw) {
  lockGlobalParams;
  drawFormFields = draw;
  unlockGlobalParams;
}

void GlobalParams::setOverprintPreview(GBool preview) {
  lockGlobalParams;
  overprintPreview = preview;
  unlockGlobalParams;
}



void GlobalParams::setMapNumericCharNames(GBool map) {
  lockGlobalParams;
  mapNumericCharNames = map;
  unlockGlobalParams;
}

void GlobalParams::setMapUnknownCharNames(GBool map) {
  lockGlobalParams;
  mapUnknownCharNames = map;
  unlockGlobalParams;
}

void GlobalParams::setMapExtTrueTypeFontsViaUnicode(GBool map) {
  lockGlobalParams;
  mapExtTrueTypeFontsViaUnicode = map;
  unlockGlobalParams;
}

void GlobalParams::setEnableXFA(GBool enable) {
  lockGlobalParams;
  enableXFA = enable;
  unlockGlobalParams;
}

void GlobalParams::setTabStateFile(char *tabStateFileA) {
  lockGlobalParams;
  delete tabStateFile;
  tabStateFile = new GString(tabStateFileA);
  unlockGlobalParams;
}

void GlobalParams::setPrintCommands(GBool printCommandsA) {
  lockGlobalParams;
  printCommands = printCommandsA;
  unlockGlobalParams;
}

void GlobalParams::setErrQuiet(GBool errQuietA) {
  lockGlobalParams;
  errQuiet = errQuietA;
  unlockGlobalParams;
}

#ifdef _WIN32
void GlobalParams::setWin32ErrorInfo(const char *func, DWORD code) {
  XpdfWin32ErrorInfo *errorInfo;

  if (tlsWin32ErrorInfo == TLS_OUT_OF_INDEXES) {
    return;
  }
  errorInfo = (XpdfWin32ErrorInfo *)TlsGetValue(tlsWin32ErrorInfo);
  if (!errorInfo) {
    errorInfo = new XpdfWin32ErrorInfo();
    TlsSetValue(tlsWin32ErrorInfo, errorInfo);
  }
  errorInfo->func = func;
  errorInfo->code = code;
}

XpdfWin32ErrorInfo *GlobalParams::getWin32ErrorInfo() {
  XpdfWin32ErrorInfo *errorInfo;

  if (tlsWin32ErrorInfo == TLS_OUT_OF_INDEXES) {
    return NULL;
  }
  errorInfo = (XpdfWin32ErrorInfo *)TlsGetValue(tlsWin32ErrorInfo);
  if (!errorInfo) {
    errorInfo = new XpdfWin32ErrorInfo();
    TlsSetValue(tlsWin32ErrorInfo, errorInfo);
    errorInfo->func = NULL;
    errorInfo->code = 0;
  }
  return errorInfo;
}
#endif
