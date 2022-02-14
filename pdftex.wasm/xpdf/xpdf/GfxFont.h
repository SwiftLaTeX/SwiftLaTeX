//========================================================================
//
// GfxFont.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GFXFONT_H
#define GFXFONT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "GString.h"
#include "Object.h"
#include "CharTypes.h"

class GList;
class GHash;
class Dict;
class CMap;
class CharCodeToUnicode;
class FoFiTrueType;
class FoFiType1C;
struct GfxFontCIDWidths;
struct Base14FontMapEntry;
class FNVHash;

//------------------------------------------------------------------------
// GfxFontType
//------------------------------------------------------------------------

enum GfxFontType {
  //----- Gfx8BitFont
  fontUnknownType,
  fontType1,
  fontType1C,
  fontType1COT,
  fontType3,
  fontTrueType,
  fontTrueTypeOT,
  //----- GfxCIDFont
  fontCIDType0,
  fontCIDType0C,
  fontCIDType0COT,
  fontCIDType2,
  fontCIDType2OT
};

//------------------------------------------------------------------------
// GfxFontCIDWidths
//------------------------------------------------------------------------

struct GfxFontCIDWidthExcep {
  CID first;			// this record applies to
  CID last;			//   CIDs <first>..<last>
  double width;			// char width
};

struct GfxFontCIDWidthExcepV {
  CID first;			// this record applies to
  CID last;			//   CIDs <first>..<last>
  double height;		// char height
  double vx, vy;		// origin position
};

struct GfxFontCIDWidths {
  double defWidth;		// default char width
  double defHeight;		// default char height
  double defVY;			// default origin position
  GfxFontCIDWidthExcep *exceps;	// exceptions
  int nExceps;			// number of valid entries in exceps
  GfxFontCIDWidthExcepV *	// exceptions for vertical font
    excepsV;
  int nExcepsV;			// number of valid entries in excepsV
};

//------------------------------------------------------------------------
// GfxFontLoc
//------------------------------------------------------------------------

enum GfxFontLocType {
  gfxFontLocEmbedded,		// font embedded in PDF file
  gfxFontLocExternal,		// external font file
  gfxFontLocResident		// font resident in PS printer
};

class GfxFontLoc {
public:

  GfxFontLoc();
  ~GfxFontLoc();

  GfxFontLocType locType;
  GfxFontType fontType;
  Ref embFontID;		// embedded stream obj ID
				//   (if locType == gfxFontLocEmbedded)
  GString *path;		// font file path
				//   (if locType == gfxFontLocExternal)
				// PS font name
				//   (if locType == gfxFontLocResident)
  int fontNum;			// for TrueType collections and Mac dfonts
				//   (if locType == gfxFontLocExternal)
  double oblique;		// sheer factor to oblique this font
				//   (used when substituting a plain
				//   font for an oblique font)
  GString *encoding;		// PS font encoding, only for 16-bit fonts
				//   (if locType == gfxFontLocResident)
  int wMode;			// writing mode, only for 16-bit fonts
				//   (if locType == gfxFontLocResident)
  int substIdx;			// substitute font index
				//   (if locType == gfxFontLocExternal,
				//   and a Base-14 substitution was made)
};

//------------------------------------------------------------------------
// GfxFont
//------------------------------------------------------------------------

#define fontFixedWidth (1 << 0)
#define fontSerif      (1 << 1)
#define fontSymbolic   (1 << 2)
#define fontItalic     (1 << 6)
#define fontBold       (1 << 18)

class GfxFont {
public:

  // Build a GfxFont object.
  static GfxFont *makeFont(XRef *xref, const char *tagA,
			   Ref idA, Dict *fontDict);

  GfxFont(const char *tagA, Ref idA, GString *nameA,
	  GfxFontType typeA, Ref embFontIDA);

  virtual ~GfxFont();

  GBool isOk() { return ok; }

  // Get font tag.
  GString *getTag() { return tag; }

  // Get font dictionary ID.
  Ref *getID() { return &id; }

  // Does this font match the tag?
  GBool matches(char *tagA) { return !tag->cmp(tagA); }

  // Get the original font name (ignornig any munging that might have
  // been done to map to a canonical Base-14 font name).
  GString *getName() { return name; }

  // Get font type.
  GfxFontType getType() { return type; }
  virtual GBool isCIDFont() { return gFalse; }

  // Get embedded font ID, i.e., a ref for the font file stream.
  // Returns false if there is no embedded font.
  GBool getEmbeddedFontID(Ref *embID)
    { *embID = embFontID; return embFontID.num >= 0; }

  // Get the PostScript font name for the embedded font.  Returns
  // NULL if there is no embedded font.
  GString *getEmbeddedFontName() { return embFontName; }

  // Get font descriptor flags.
  int getFlags() { return flags; }
  GBool isFixedWidth() { return flags & fontFixedWidth; }
  GBool isSerif() { return flags & fontSerif; }
  GBool isSymbolic() { return flags & fontSymbolic; }
  GBool isItalic() { return flags & fontItalic; }
  GBool isBold() { return flags & fontBold; }

  // Return the font matrix.
  double *getFontMatrix() { return fontMat; }

  // Return the font bounding box.
  double *getFontBBox() { return fontBBox; }

  // Return the ascent and descent values.
  double getAscent() { return ascent; }
  double getDescent() { return descent; }

  // Return the writing mode (0=horizontal, 1=vertical).
  virtual int getWMode() { return 0; }

  // Locate the font file for this font.  If <ps> is true, includes PS
  // printer-resident fonts.  Returns NULL on failure.
  GfxFontLoc *locateFont(XRef *xref, GBool ps);

  // Locate a Base-14 font file for a specified font name.
  static GfxFontLoc *locateBase14Font(GString *base14Name);

  // Read an embedded font file into a buffer.
  char *readEmbFontFile(XRef *xref, int *len);

  // Get the next char from a string <s> of <len> bytes, returning the
  // char <code>, its Unicode mapping <u>, its displacement vector
  // (<dx>, <dy>), and its origin offset vector (<ox>, <oy>).  <uSize>
  // is the number of entries available in <u>, and <uLen> is set to
  // the number actually used.  Returns the number of bytes used by
  // the char code.
  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy) = 0;

  // Returns true if this font is likely to be problematic when
  // converting text to Unicode.
  virtual GBool problematicForUnicode() = 0;

protected:

  static GfxFontType getFontType(XRef *xref, Dict *fontDict, Ref *embID);
  void readFontDescriptor(XRef *xref, Dict *fontDict);
  CharCodeToUnicode *readToUnicodeCMap(Dict *fontDict, int nBits,
				       CharCodeToUnicode *ctu);
  static GfxFontLoc *getExternalFont(GString *path, int fontNum,
				     double oblique, GBool cid);

  GString *tag;			// PDF font tag
  Ref id;			// reference (used as unique ID)
  GString *name;		// font name
  GfxFontType type;		// type of font
  int flags;			// font descriptor flags
  GString *embFontName;		// name of embedded font
  Ref embFontID;		// ref to embedded font file stream
  double fontMat[6];		// font matrix
  double fontBBox[4];		// font bounding box
  double missingWidth;		// "default" width
  double ascent;		// max height above baseline
  double descent;		// max depth below baseline
  GBool hasToUnicode;		// true if the font has a ToUnicode map
  GBool ok;
};

//------------------------------------------------------------------------
// Gfx8BitFont
//------------------------------------------------------------------------

class Gfx8BitFont: public GfxFont {
public:

  Gfx8BitFont(XRef *xref, const char *tagA, Ref idA, GString *nameA,
	      GfxFontType typeA, Ref embFontIDA, Dict *fontDict);

  virtual ~Gfx8BitFont();

  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy);

  // Return the encoding.
  char **getEncoding() { return enc; }

  // Return the Unicode map.
  CharCodeToUnicode *getToUnicode();

  // Return the character name associated with <code>.
  char *getCharName(int code) { return enc[code]; }

  // Returns true if the PDF font specified an encoding.
  GBool getHasEncoding() { return hasEncoding; }

  // Returns true if the PDF font specified MacRomanEncoding.
  GBool getUsesMacRomanEnc() { return usesMacRomanEnc; }

  // Get width of a character.
  double getWidth(Guchar c) { return widths[c]; }

  // Return a char code-to-GID mapping for the provided font file.
  // (This is only useful for TrueType fonts.)
  int *getCodeToGIDMap(FoFiTrueType *ff);

  // Return a char code-to-GID mapping for the provided font file.
  // (This is only useful for Type1C fonts.)
  int *getCodeToGIDMap(FoFiType1C *ff);

  // Return the Type 3 CharProc dictionary, or NULL if none.
  Dict *getCharProcs();

  // Return the Type 3 CharProc for the character associated with <code>.
  Object *getCharProc(int code, Object *proc);
  Object *getCharProcNF(int code, Object *proc);

  // Return the Type 3 Resources dictionary, or NULL if none.
  Dict *getResources();

  virtual GBool problematicForUnicode();

private:

  Base14FontMapEntry *base14;	// for Base-14 fonts only; NULL otherwise
  char *enc[256];		// char code --> char name
  char encFree[256];		// boolean for each char name: if set,
				//   the string is malloc'ed
  CharCodeToUnicode *ctu;	// char code --> Unicode
  GBool hasEncoding;
  GBool usesMacRomanEnc;
  GBool baseEncFromFontFile;
  GBool usedNumericHeuristic;
  double widths[256];		// character widths
  Object charProcs;		// Type 3 CharProcs dictionary
  Object resources;		// Type 3 Resources dictionary

  friend class GfxFont;
};

//------------------------------------------------------------------------
// GfxCIDFont
//------------------------------------------------------------------------

class GfxCIDFont: public GfxFont {
public:

  GfxCIDFont(XRef *xref, const char *tagA, Ref idA, GString *nameA,
	     GfxFontType typeA, Ref embFontIDA, Dict *fontDict);

  virtual ~GfxCIDFont();

  virtual GBool isCIDFont() { return gTrue; }

  virtual int getNextChar(char *s, int len, CharCode *code,
			  Unicode *u, int uSize, int *uLen,
			  double *dx, double *dy, double *ox, double *oy);

  // Return the writing mode (0=horizontal, 1=vertical).
  virtual int getWMode();

  // Return the Unicode map.
  CharCodeToUnicode *getToUnicode();

  // Get the collection name (<registry>-<ordering>).
  GString *getCollection();

  // Return the horizontal width for <cid>.
  double getWidth(CID cid);

  // Return the CID-to-GID mapping table.  These should only be called
  // if type is fontCIDType2.
  int *getCIDToGID() { return cidToGID; }
  int getCIDToGIDLen() { return cidToGIDLen; }

  // Returns true if this font uses the Identity-H encoding (cmap),
  // and the Adobe-Identity character collection, and does not have a
  // CIDToGIDMap.  When this is true for a CID TrueType font, Adobe
  // appears to treat char codes as raw GIDs.
  GBool usesIdentityEncoding() { return identityEnc; }

  virtual GBool problematicForUnicode();

private:

  void getHorizontalMetrics(CID cid, double *w);
  void getVerticalMetrics(CID cid, double *h,
			  double *vx, double *vy);

  GString *collection;		// collection name
  CMap *cMap;			// char code --> CID
  CharCodeToUnicode *ctu;	// CID/char code --> Unicode
  GBool ctuUsesCharCode;	// true: ctu maps char code to Unicode;
				//   false: ctu maps CID to Unicode
  GfxFontCIDWidths widths;	// character widths
  int *cidToGID;		// CID --> GID mapping (for embedded
				//   TrueType fonts)
  int cidToGIDLen;
  GBool hasKnownCollection;
  GBool hasIdentityCIDToGID;
  GBool identityEnc;
};

//------------------------------------------------------------------------
// GfxFontDict
//------------------------------------------------------------------------

class GfxFontDict {
public:

  // Build the font dictionary, given the PDF font dictionary.
  GfxFontDict(XRef *xref, Ref *fontDictRef, Dict *fontDict);

  // Destructor.
  ~GfxFontDict();

  // Get the specified font.
  GfxFont *lookup(char *tag);
  GfxFont *lookupByRef(Ref ref);

  // Iterative access.
  int getNumFonts();
  GfxFont *getFont(int i);

private:

  int hashFontObject(Object *obj);
  void hashFontObject1(Object *obj, FNVHash *h);

  GHash *fonts;			// hash table of fonts -- this may
				//   include duplicates, i.e., when
				//   two tags map to the same font
  GList *uniqueFonts;		// list of all unique font objects (no dups)
};

#endif
