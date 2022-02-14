//========================================================================
//
// GlobalParams.h
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================
//  Modified for TeX Live by Peter Breitenlohner <tex-live@tug.org>
//  See top-level ChangeLog for a list of all modifications
//========================================================================

#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#ifdef _WIN32
#  include <windows.h>
#endif
#include "gtypes.h"
#include "CharTypes.h"

#if MULTITHREADED
#include "GMutex.h"
#endif

class GString;
class GList;
class GHash;
class NameToCharCode;
class CharCodeToUnicode;
class CharCodeToUnicodeCache;
class UnicodeMap;
class UnicodeMapCache;
class UnicodeRemapping;
class CMap;
class CMapCache;
struct XpdfSecurityHandler;
class GlobalParams;
class SysFontList;

//------------------------------------------------------------------------

// The global parameters object.
extern GlobalParams *globalParams;

//------------------------------------------------------------------------

enum SysFontType {
  sysFontPFA,
  sysFontPFB,
  sysFontTTF,
  sysFontTTC,
  sysFontOTF
};

//------------------------------------------------------------------------

class PSFontParam16 {
public:

  GString *name;		// PDF font name for psResidentFont16;
				//   char collection name for psResidentFontCC
  int wMode;			// writing mode (0=horiz, 1=vert)
  GString *psFontName;		// PostScript font name
  GString *encoding;		// encoding

  PSFontParam16(GString *nameA, int wModeA,
		GString *psFontNameA, GString *encodingA);
  ~PSFontParam16();
};

//------------------------------------------------------------------------

enum PSLevel {
  psLevel1,
  psLevel1Sep,
  psLevel2,
  psLevel2Gray,
  psLevel2Sep,
  psLevel3,
  psLevel3Gray,
  psLevel3Sep,
};

//------------------------------------------------------------------------

enum EndOfLineKind {
  eolUnix,			// LF
  eolDOS,			// CR+LF
  eolMac			// CR
};

//------------------------------------------------------------------------

enum StrokeAdjustMode {
  strokeAdjustOff,
  strokeAdjustNormal,
  strokeAdjustCAD
};

//------------------------------------------------------------------------

enum ScreenType {
  screenUnset,
  screenDispersed,
  screenClustered,
  screenStochasticClustered
};

//------------------------------------------------------------------------

class KeyBinding {
public:

  int code;			// 0x20 .. 0xfe = ASCII,
				//   >=0x10000 = special keys, mouse buttons,
				//   etc. (xpdfKeyCode* symbols)
  int mods;			// modifiers (xpdfKeyMod* symbols, or-ed
				//   together)
  int context;			// context (xpdfKeyContext* symbols, or-ed
				//   together)
  GList *cmds;			// list of commands [GString]

  KeyBinding(int codeA, int modsA, int contextA, const char *cmd0);
  KeyBinding(int codeA, int modsA, int contextA,
	     const char *cmd0, const char *cmd1);
  KeyBinding(int codeA, int modsA, int contextA, GList *cmdsA);
  ~KeyBinding();
};

#define xpdfKeyCodeTab            0x1000
#define xpdfKeyCodeReturn         0x1001
#define xpdfKeyCodeEnter          0x1002
#define xpdfKeyCodeBackspace      0x1003
#define xpdfKeyCodeEsc            0x1004
#define xpdfKeyCodeInsert         0x1005
#define xpdfKeyCodeDelete         0x1006
#define xpdfKeyCodeHome           0x1007
#define xpdfKeyCodeEnd            0x1008
#define xpdfKeyCodePgUp           0x1009
#define xpdfKeyCodePgDn           0x100a
#define xpdfKeyCodeLeft           0x100b
#define xpdfKeyCodeRight          0x100c
#define xpdfKeyCodeUp             0x100d
#define xpdfKeyCodeDown           0x100e
#define xpdfKeyCodeF1             0x1100
#define xpdfKeyCodeF35            0x1122
#define xpdfKeyCodeMousePress1    0x2001
#define xpdfKeyCodeMousePress2    0x2002
#define xpdfKeyCodeMousePress3    0x2003
#define xpdfKeyCodeMousePress4    0x2004
#define xpdfKeyCodeMousePress5    0x2005
#define xpdfKeyCodeMousePress6    0x2006
#define xpdfKeyCodeMousePress7    0x2007
// ...
#define xpdfKeyCodeMousePress32   0x2020
#define xpdfKeyCodeMouseRelease1  0x2101
#define xpdfKeyCodeMouseRelease2  0x2102
#define xpdfKeyCodeMouseRelease3  0x2103
#define xpdfKeyCodeMouseRelease4  0x2104
#define xpdfKeyCodeMouseRelease5  0x2105
#define xpdfKeyCodeMouseRelease6  0x2106
#define xpdfKeyCodeMouseRelease7  0x2107
// ...
#define xpdfKeyCodeMouseRelease32 0x2120
#define xpdfKeyCodeMouseClick1    0x2201
#define xpdfKeyCodeMouseClick2    0x2202
#define xpdfKeyCodeMouseClick3    0x2203
#define xpdfKeyCodeMouseClick4    0x2204
#define xpdfKeyCodeMouseClick5    0x2205
#define xpdfKeyCodeMouseClick6    0x2206
#define xpdfKeyCodeMouseClick7    0x2207
// ...
#define xpdfKeyCodeMouseClick32   0x2220
#define xpdfKeyModNone            0
#define xpdfKeyModShift           (1 << 0)
#define xpdfKeyModCtrl            (1 << 1)
#define xpdfKeyModAlt             (1 << 2)
#define xpdfKeyContextAny         0
#define xpdfKeyContextFullScreen  (1 << 0)
#define xpdfKeyContextWindow      (2 << 0)
#define xpdfKeyContextContinuous  (1 << 2)
#define xpdfKeyContextSinglePage  (2 << 2)
#define xpdfKeyContextOverLink    (1 << 4)
#define xpdfKeyContextOffLink     (2 << 4)
#define xpdfKeyContextOutline     (1 << 6)
#define xpdfKeyContextMainWin     (2 << 6)
#define xpdfKeyContextScrLockOn   (1 << 8)
#define xpdfKeyContextScrLockOff  (2 << 8)

//------------------------------------------------------------------------

class PopupMenuCmd {
public:

  GString *label;		// label for display in the menu
  GList *cmds;			// list of commands [GString]

  PopupMenuCmd(GString *labelA, GList *cmdsA);
  ~PopupMenuCmd();
};

//------------------------------------------------------------------------

#ifdef _WIN32
struct XpdfWin32ErrorInfo {
  const char *func;		// last Win32 API function call to fail
  DWORD code;			// error code returned by that function
};
#endif

//------------------------------------------------------------------------

class GlobalParams {
public:

  // Initialize the global parameters by attempting to read a config
  // file.
  GlobalParams(const char *cfgFileName = NULL);

  ~GlobalParams();

  void setBaseDir(const char *dir);
  void setupBaseFonts(const char *dir);

  void parseLine(char *buf, GString *fileName, int line);

  //----- accessors

  CharCode getMacRomanCharCode(char *charName);

  GString *getBaseDir();
  Unicode mapNameToUnicode(const char *charName);
  UnicodeMap *getResidentUnicodeMap(GString *encodingName);
  FILE *getUnicodeMapFile(GString *encodingName);
  FILE *findCMapFile(GString *collection, GString *cMapName);
  FILE *findToUnicodeFile(GString *name);
  UnicodeRemapping *getUnicodeRemapping();
  GString *findFontFile(GString *fontName);
  GString *findBase14FontFile(GString *fontName, int *fontNum,
			      double *oblique);
  GString *findSystemFontFile(GString *fontName, SysFontType *type,
			      int *fontNum);
  GString *findCCFontFile(GString *collection);
  int getPSPaperWidth();
  int getPSPaperHeight();
  void getPSImageableArea(int *llx, int *lly, int *urx, int *ury);
  GBool getPSDuplex();
  GBool getPSCrop();
  GBool getPSUseCropBoxAsPage();
  GBool getPSExpandSmaller();
  GBool getPSShrinkLarger();
  GBool getPSCenter();
  PSLevel getPSLevel();
  GString *getPSResidentFont(GString *fontName);
  GList *getPSResidentFonts();
  PSFontParam16 *getPSResidentFont16(GString *fontName, int wMode);
  PSFontParam16 *getPSResidentFontCC(GString *collection, int wMode);
  GBool getPSEmbedType1();
  GBool getPSEmbedTrueType();
  GBool getPSEmbedCIDPostScript();
  GBool getPSEmbedCIDTrueType();
  GBool getPSFontPassthrough();
  GBool getPSPreload();
  GBool getPSOPI();
  GBool getPSASCIIHex();
  GBool getPSLZW();
  GBool getPSUncompressPreloadedImages();
  double getPSMinLineWidth();
  double getPSRasterResolution();
  GBool getPSRasterMono();
  int getPSRasterSliceSize();
  GBool getPSAlwaysRasterize();
  GBool getPSNeverRasterize();
  GString *getTextEncodingName();
  EndOfLineKind getTextEOL();
  GBool getTextPageBreaks();
  GBool getTextKeepTinyChars();
  GString *getInitialZoom();
  int getDefaultFitZoom();
  GString *getInitialDisplayMode();
  GBool getInitialToolbarState();
  GBool getInitialSidebarState();
  GString *getInitialSelectMode();
  int getMaxTileWidth();
  int getMaxTileHeight();
  int getTileCacheSize();
  int getWorkerThreads();
  GBool getEnableFreeType();
  GBool getDisableFreeTypeHinting();
  GBool getAntialias();
  GBool getVectorAntialias();
  GBool getAntialiasPrinting();
  StrokeAdjustMode getStrokeAdjust();
  ScreenType getScreenType();
  int getScreenSize();
  int getScreenDotRadius();
  double getScreenGamma();
  double getScreenBlackThreshold();
  double getScreenWhiteThreshold();
  double getMinLineWidth();
  GBool getEnablePathSimplification();
  GBool getDrawAnnotations();
  GBool getDrawFormFields();
  GBool getOverprintPreview() { return overprintPreview; }
  GString *getPaperColor();
  GString *getMatteColor();
  GString *getFullScreenMatteColor();
  GBool getReverseVideoInvertImages();
  GString *getLaunchCommand() { return launchCommand; }
  GString *getMovieCommand() { return movieCommand; }
  GString *getDefaultPrinter();
  GBool getMapNumericCharNames();
  GBool getMapUnknownCharNames();
  GBool getMapExtTrueTypeFontsViaUnicode();
  GBool isDroppedFont(const char *fontName);
  GBool getEnableXFA();
  GList *getKeyBinding(int code, int mods, int context);
  int getNumPopupMenuCmds();
  PopupMenuCmd *getPopupMenuCmd(int idx);
  GString *getTabStateFile();
  GBool getPrintCommands();
  GBool getErrQuiet();
  GString *getDebugLogFile();
  void debugLogPrintf(char *fmt, ...);

  CharCodeToUnicode *getCIDToUnicode(GString *collection);
  CharCodeToUnicode *getUnicodeToUnicode(GString *fontName);
  UnicodeMap *getUnicodeMap(GString *encodingName);
  CMap *getCMap(GString *collection, GString *cMapName);
  UnicodeMap *getTextEncoding();

  //----- functions to set parameters

  void addUnicodeRemapping(Unicode in, Unicode *out, int len);
  void addFontFile(GString *fontName, GString *path);
  GBool setPSPaperSize(char *size);
  void setPSPaperWidth(int width);
  void setPSPaperHeight(int height);
  void setPSImageableArea(int llx, int lly, int urx, int ury);
  void setPSDuplex(GBool duplex);
  void setPSCrop(GBool crop);
  void setPSUseCropBoxAsPage(GBool crop);
  void setPSExpandSmaller(GBool expand);
  void setPSShrinkLarger(GBool shrink);
  void setPSCenter(GBool center);
  void setPSLevel(PSLevel level);
  void setPSEmbedType1(GBool embed);
  void setPSEmbedTrueType(GBool embed);
  void setPSEmbedCIDPostScript(GBool embed);
  void setPSEmbedCIDTrueType(GBool embed);
  void setPSFontPassthrough(GBool passthrough);
  void setPSPreload(GBool preload);
  void setPSOPI(GBool opi);
  void setPSASCIIHex(GBool hex);
  void setTextEncoding(const char *encodingName);
  GBool setTextEOL(char *s);
  void setTextPageBreaks(GBool pageBreaks);
  void setTextKeepTinyChars(GBool keep);
  void setInitialZoom(char *s);
  GBool setEnableFreeType(char *s);
  GBool setAntialias(char *s);
  GBool setVectorAntialias(char *s);
  void setScreenType(ScreenType t);
  void setScreenSize(int size);
  void setScreenDotRadius(int r);
  void setScreenGamma(double gamma);
  void setScreenBlackThreshold(double thresh);
  void setScreenWhiteThreshold(double thresh);
  void setDrawFormFields(GBool draw);
  void setOverprintPreview(GBool preview);
  void setMapNumericCharNames(GBool map);
  void setMapUnknownCharNames(GBool map);
  void setMapExtTrueTypeFontsViaUnicode(GBool map);
  void setEnableXFA(GBool enable);
  void setTabStateFile(char *tabStateFileA);
  void setPrintCommands(GBool printCommandsA);
  void setErrQuiet(GBool errQuietA);

#ifdef _WIN32
  void setWin32ErrorInfo(const char *func, DWORD code);
  XpdfWin32ErrorInfo *getWin32ErrorInfo();
#endif

  static const char *defaultTextEncoding;

private:

  void createDefaultKeyBindings();
  void parseFile(GString *fileName, FILE *f);
  void parseNameToUnicode(GList *tokens, GString *fileName, int line);
  void parseCIDToUnicode(GList *tokens, GString *fileName, int line);
  void parseUnicodeToUnicode(GList *tokens, GString *fileName, int line);
  void parseUnicodeMap(GList *tokens, GString *fileName, int line);
  void parseCMapDir(GList *tokens, GString *fileName, int line);
  void parseToUnicodeDir(GList *tokens, GString *fileName, int line);
  void parseUnicodeRemapping(GList *tokens, GString *fileName, int line);
  void parseFontFile(GList *tokens, GString *fileName, int line);
  void parseFontDir(GList *tokens, GString *fileName, int line);
  void parseFontFileCC(GList *tokens, GString *fileName,
		       int line);
  void parsePSPaperSize(GList *tokens, GString *fileName, int line);
  void parsePSImageableArea(GList *tokens, GString *fileName, int line);
  void parsePSLevel(GList *tokens, GString *fileName, int line);
  void parsePSResidentFont(GList *tokens, GString *fileName, int line);
  void parsePSResidentFont16(GList *tokens, GString *fileName, int line);
  void parsePSResidentFontCC(GList *tokens, GString *fileName, int line);
  void parseTextEOL(GList *tokens, GString *fileName, int line);
  void parseStrokeAdjust(GList *tokens, GString *fileName, int line);
  void parseScreenType(GList *tokens, GString *fileName, int line);
  void parseDropFont(GList *tokens, GString *fileName, int line);
  void parseBind(GList *tokens, GString *fileName, int line);
  void parseUnbind(GList *tokens, GString *fileName, int line);
  GBool parseKey(GString *modKeyStr, GString *contextStr,
		 int *code, int *mods, int *context,
		 const char *cmdName,
		 GList *tokens, GString *fileName, int line);
  void parsePopupMenuCmd(GList *tokens, GString *fileName, int line);
  void parseYesNo(const char *cmdName, GBool *flag,
		  GList *tokens, GString *fileName, int line);
  GBool parseYesNo2(char *token, GBool *flag);
  void parseString(const char *cmdName, GString **s,
		   GList *tokens, GString *fileName, int line);
  void parseInteger(const char *cmdName, int *val,
		    GList *tokens, GString *fileName, int line);
  void parseFloat(const char *cmdName, double *val,
		  GList *tokens, GString *fileName, int line);
  UnicodeMap *getUnicodeMap2(GString *encodingName);

  //----- static tables

  NameToCharCode *		// mapping from char name to
    macRomanReverseMap;		//   MacRomanEncoding index

  //----- user-modifiable settings

  GString *baseDir;		// base directory - for plugins, etc.
  NameToCharCode *		// mapping from char name to Unicode
    nameToUnicode;
  GHash *cidToUnicodes;		// files for mappings from char collections
				//   to Unicode, indexed by collection name
				//   [GString]
  GHash *unicodeToUnicodes;	// files for Unicode-to-Unicode mappings,
				//   indexed by font name pattern [GString]
  GHash *residentUnicodeMaps;	// mappings from Unicode to char codes,
				//   indexed by encoding name [UnicodeMap]
  GHash *unicodeMaps;		// files for mappings from Unicode to char
				//   codes, indexed by encoding name [GString]
  GHash *cMapDirs;		// list of CMap dirs, indexed by collection
				//   name [GList[GString]]
  GList *toUnicodeDirs;		// list of ToUnicode CMap dirs [GString]
  UnicodeRemapping *		// Unicode remapping for text output
    unicodeRemapping;
  GHash *fontFiles;		// font files: font name mapped to path
				//   [GString]
  GList *fontDirs;		// list of font dirs [GString]
  GHash *ccFontFiles;		// character collection font files:
				//   collection name  mapped to path [GString]
  GHash *base14SysFonts;	// Base-14 system font files: font name
				//   mapped to path [Base14FontInfo]
  SysFontList *sysFonts;	// system fonts
  int psPaperWidth;		// paper size, in PostScript points, for
  int psPaperHeight;		//   PostScript output
  int psImageableLLX,		// imageable area, in PostScript points,
      psImageableLLY,		//   for PostScript output
      psImageableURX,
      psImageableURY;
  GBool psCrop;			// crop PS output to CropBox
  GBool psUseCropBoxAsPage;	// use CropBox as page size
  GBool psExpandSmaller;	// expand smaller pages to fill paper
  GBool psShrinkLarger;		// shrink larger pages to fit paper
  GBool psCenter;		// center pages on the paper
  GBool psDuplex;		// enable duplexing in PostScript?
  PSLevel psLevel;		// PostScript level to generate
  GHash *psResidentFonts;	// 8-bit fonts resident in printer:
				//   PDF font name mapped to PS font name
				//   [GString]
  GList *psResidentFonts16;	// 16-bit fonts resident in printer:
				//   PDF font name mapped to font info
				//   [PSFontParam16]
  GList *psResidentFontsCC;	// 16-bit character collection fonts
				//   resident in printer: collection name
				//   mapped to font info [PSFontParam16]
  GBool psEmbedType1;		// embed Type 1 fonts?
  GBool psEmbedTrueType;	// embed TrueType fonts?
  GBool psEmbedCIDPostScript;	// embed CID PostScript fonts?
  GBool psEmbedCIDTrueType;	// embed CID TrueType fonts?
  GBool psFontPassthrough;	// pass all fonts through as-is?
  GBool psPreload;		// preload PostScript images and forms into
				//   memory
  GBool psOPI;			// generate PostScript OPI comments?
  GBool psASCIIHex;		// use ASCIIHex instead of ASCII85?
  GBool psLZW;			// false to use RLE instead of LZW
  GBool psUncompressPreloadedImages;  // uncompress all preloaded images
  double psMinLineWidth;	// minimum line width for PostScript output
  double psRasterResolution;	// PostScript rasterization resolution (dpi)
  GBool psRasterMono;		// true to do PostScript rasterization
				//   in monochrome (gray); false to do it
				//   in color (RGB/CMYK)
  int psRasterSliceSize;	// maximum size (pixels) of PostScript
				//   rasterization slice
  GBool psAlwaysRasterize;	// force PostScript rasterization
  GBool psNeverRasterize;	// prevent PostScript rasterization
  GString *textEncoding;	// encoding (unicodeMap) to use for text
				//   output
  EndOfLineKind textEOL;	// type of EOL marker to use for text
				//   output
  GBool textPageBreaks;		// insert end-of-page markers?
  GBool textKeepTinyChars;	// keep all characters in text output
  GString *initialZoom;		// initial zoom level
  int defaultFitZoom;		// default zoom factor if initialZoom is
				//   'page' or 'width'.
  GString *initialDisplayMode;	// initial display mode (single,
				//   continuous, etc.)
  GBool initialToolbarState;	// initial toolbar state - open (true)
				//   or closed (false)
  GBool initialSidebarState;	// initial sidebar state - open (true)
				//   or closed (false)
  GString *initialSelectMode;	// initial selection mode (block or linear)
  int maxTileWidth;		// maximum rasterization tile width
  int maxTileHeight;		// maximum rasterization tile height
  int tileCacheSize;		// number of rasterization tiles in cache
  int workerThreads;		// number of rasterization worker threads
  GBool enableFreeType;		// FreeType enable flag
  GBool disableFreeTypeHinting;	// FreeType hinting disable flag
  GBool antialias;		// font anti-aliasing enable flag
  GBool vectorAntialias;	// vector anti-aliasing enable flag
  GBool antialiasPrinting;	// allow anti-aliasing when printing
  StrokeAdjustMode strokeAdjust; // stroke adjustment mode
  ScreenType screenType;	// halftone screen type
  int screenSize;		// screen matrix size
  int screenDotRadius;		// screen dot radius
  double screenGamma;		// screen gamma correction
  double screenBlackThreshold;	// screen black clamping threshold
  double screenWhiteThreshold;	// screen white clamping threshold
  double minLineWidth;		// minimum line width
  GBool				// enable path simplification
    enablePathSimplification;
  GBool drawAnnotations;	// draw annotations or not
  GBool drawFormFields;		// draw form fields or not
  GBool overprintPreview;	// enable overprint preview
  GString *paperColor;		// paper (page background) color
  GString *matteColor;		// matte (background outside of page) color
  GString *fullScreenMatteColor; // matte color in full-screen mode
  GBool reverseVideoInvertImages; // invert images in reverse video mode
  GString *launchCommand;	// command executed for 'launch' links
  GString *movieCommand;	// command executed for movie annotations
  GString *defaultPrinter;	// default printer (for interactive printing
				//   from the viewer)
  GBool mapNumericCharNames;	// map numeric char names (from font subsets)?
  GBool mapUnknownCharNames;	// map unknown char names?
  GBool mapExtTrueTypeFontsViaUnicode;  // map char codes to GID via Unicode
				        //   for external TrueType fonts?
  GHash *droppedFonts;		// dropped fonts [int]
  GBool enableXFA;		// enable XFA form rendering
  GList *keyBindings;		// key & mouse button bindings [KeyBinding]
  GList *popupMenuCmds;		// popup menu commands [PopupMenuCmd]
  GString *tabStateFile;	// path for the tab state save file
  GBool printCommands;		// print the drawing commands
  GBool errQuiet;		// suppress error messages?
  GString *debugLogFile;	// path for debug log file

  CharCodeToUnicodeCache *cidToUnicodeCache;
  CharCodeToUnicodeCache *unicodeToUnicodeCache;
  UnicodeMapCache *unicodeMapCache;
  CMapCache *cMapCache;

#if MULTITHREADED
  GMutex mutex;
  GMutex unicodeMapCacheMutex;
  GMutex cMapCacheMutex;
#endif
#ifdef _WIN32
  DWORD tlsWin32ErrorInfo;	// TLS index for error info
#endif
};

#endif
