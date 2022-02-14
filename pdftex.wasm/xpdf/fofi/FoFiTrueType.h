//========================================================================
//
// FoFiTrueType.h
//
// Copyright 1999-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef FOFITRUETYPE_H
#define FOFITRUETYPE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "FoFiBase.h"

class GString;
class GHash;
struct TrueTypeTable;
struct TrueTypeCmap;

//------------------------------------------------------------------------
// FoFiTrueType
//------------------------------------------------------------------------

class FoFiTrueType: public FoFiBase {
public:

  // Create a FoFiTrueType object from a memory buffer.  If
  // <allowHeadlessCFF> is true, OpenType CFF fonts without the 'head'
  // table are permitted -- this is useful when calling the convert*
  // functions.
  static FoFiTrueType *make(char *fileA, int lenA, int fontNum,
			    GBool allowHeadlessCFF = gFalse);

  // Create a FoFiTrueType object from a file on disk.  If
  // <allowHeadlessCFF> is true, OpenType CFF fonts without the 'head'
  // table are permitted -- this is useful when calling the convert*
  // functions.
  static FoFiTrueType *load(char *fileName, int fontNum,
			    GBool allowHeadlessCFF = gFalse);

  virtual ~FoFiTrueType();

  // Returns true if this an OpenType font containing CFF data, false
  // if it's a TrueType font (or OpenType font with TrueType data).
  GBool isOpenTypeCFF() { return openTypeCFF; }

  // Returns true if this is an OpenType CFF font that is missing the
  // 'head' table.  This is a violation of the OpenType spec, but the
  // embedded CFF font can be usable for some purposes (e.g., the
  // convert* functions).
  GBool isHeadlessCFF() { return headlessCFF; }

  // Return the number of cmaps defined by this font.
  int getNumCmaps();

  // Return the platform ID of the <i>th cmap.
  int getCmapPlatform(int i);

  // Return the encoding ID of the <i>th cmap.
  int getCmapEncoding(int i);

  // Return the index of the cmap for <platform>, <encoding>.  Returns
  // -1 if there is no corresponding cmap.
  int findCmap(int platform, int encoding);

  // Return the GID corresponding to <c> according to the <i>th cmap.
  int mapCodeToGID(int i, int c);

  // Returns the GID corresponding to <name> according to the post
  // table.  Returns 0 if there is no mapping for <name> or if the
  // font does not have a post table.
  int mapNameToGID(char *name);

  // Return the mapping from CIDs to GIDs, and return the number of
  // CIDs in *<nCIDs>.  This is only useful for CID fonts.  (Only
  // useful for OpenType CFF fonts.)
  int *getCIDToGIDMap(int *nCIDs);

  // Returns the least restrictive embedding licensing right (as
  // defined by the TrueType spec):
  // * 4: OS/2 table is missing or invalid
  // * 3: installable embedding
  // * 2: editable embedding
  // * 1: preview & print embedding
  // * 0: restricted license embedding
  int getEmbeddingRights();

  // Return the font matrix as an array of six numbers.  (Only useful
  // for OpenType CFF fonts.)
  void getFontMatrix(double *mat);

  // Return the number of glyphs in the font.
  int getNumGlyphs() { return nGlyphs; }

  // Returns true if this looks like a CJK font that uses bytecode
  // instructions to assemble glyphs.
  GBool checkForTrickyCJK();

  // Convert to a Type 42 font, suitable for embedding in a PostScript
  // file.  <psName> will be used as the PostScript font name (so we
  // don't need to depend on the 'name' table in the font).  The
  // <encoding> array specifies the mapping from char codes to names.
  // If <encoding> is NULL, the encoding is unknown or undefined.  The
  // <codeToGID> array specifies the mapping from char codes to GIDs.
  // (Not useful for OpenType CFF fonts.)
  void convertToType42(char *psName, char **encoding,
		       int *codeToGID,
		       FoFiOutputFunc outputFunc, void *outputStream);

  // Convert to a Type 1 font, suitable for embedding in a PostScript
  // file.  This is only useful with 8-bit fonts.  If <newEncoding> is
  // not NULL, it will be used in place of the encoding in the Type 1C
  // font.  If <ascii> is true the eexec section will be hex-encoded,
  // otherwise it will be left as binary data.  If <psName> is
  // non-NULL, it will be used as the PostScript font name.  (Only
  // useful for OpenType CFF fonts.)
  void convertToType1(char *psName, const char **newEncoding, GBool ascii,
		      FoFiOutputFunc outputFunc, void *outputStream);

  // Convert to a Type 2 CIDFont, suitable for embedding in a
  // PostScript file.  <psName> will be used as the PostScript font
  // name (so we don't need to depend on the 'name' table in the
  // font).  The <cidMap> array maps CIDs to GIDs; it has <nCIDs>
  // entries.  (Not useful for OpenType CFF fonts.)
  void convertToCIDType2(char *psName, int *cidMap, int nCIDs,
			 GBool needVerticalMetrics,
			 FoFiOutputFunc outputFunc, void *outputStream);

  // Convert to a Type 0 CIDFont, suitable for embedding in a
  // PostScript file.  <psName> will be used as the PostScript font
  // name.  (Only useful for OpenType CFF fonts.)
  void convertToCIDType0(char *psName, int *cidMap, int nCIDs,
			 FoFiOutputFunc outputFunc, void *outputStream);

  // Convert to a Type 0 (but non-CID) composite font, suitable for
  // embedding in a PostScript file.  <psName> will be used as the
  // PostScript font name (so we don't need to depend on the 'name'
  // table in the font).  The <cidMap> array maps CIDs to GIDs; it has
  // <nCIDs> entries.  (Not useful for OpenType CFF fonts.)
  void convertToType0(char *psName, int *cidMap, int nCIDs,
		      GBool needVerticalMetrics,
		      FoFiOutputFunc outputFunc, void *outputStream);

  // Convert to a Type 0 (but non-CID) composite font, suitable for
  // embedding in a PostScript file.  <psName> will be used as the
  // PostScript font name.  (Only useful for OpenType CFF fonts.)
  void convertToType0(char *psName, int *cidMap, int nCIDs,
		      FoFiOutputFunc outputFunc, void *outputStream);

  // Write a clean TTF file, filling in missing tables and correcting
  // various other errors.  If <name> is non-NULL, the font is renamed
  // to <name>.  If <codeToGID> is non-NULL, the font is re-encoded,
  // using a Windows Unicode cmap.  If <name> is NULL and the font is
  // complete and correct, it will be written unmodified.  If
  // <replacementCmapTable> is non-NULL it will be used as the cmap
  // table in the written font (overriding any existing cmap table
  // and/or the codeToGID arg).  (Not useful for OpenType CFF fonts.)
  // Returns true if the font was modified.
  GBool writeTTF(FoFiOutputFunc outputFunc, void *outputStream,
		 char *name = NULL, int *codeToGID = NULL,
		 Guchar *replacementCmapTable = NULL,
		 int replacementCmapTableLen = 0);

  // Returns a pointer to the CFF font embedded in this OpenType font.
  // If successful, sets *<start> and *<length>, and returns true.
  // Otherwise returns false.  (Only useful for OpenType CFF fonts).
  GBool getCFFBlock(char **start, int *length);

private:

  FoFiTrueType(char *fileA, int lenA, GBool freeFileDataA,
	       int fontNum, GBool isDfont, GBool allowHeadlessCFF);
  void cvtEncoding(char **encoding,
		   FoFiOutputFunc outputFunc,
		   void *outputStream);
  void cvtCharStrings(char **encoding,
		      int *codeToGID,
		      FoFiOutputFunc outputFunc,
		      void *outputStream);
  void cvtSfnts(FoFiOutputFunc outputFunc,
		void *outputStream, GString *name,
		GBool needVerticalMetrics,
		int *maxUsedGlyph);
  void dumpString(Guchar *s, int length,
		  FoFiOutputFunc outputFunc,
		  void *outputStream);
  Guint computeTableChecksum(Guchar *data, int length);
  void parse(int fontNum, GBool allowHeadlessCFF);
  void parseTTC(int fontNum, int *pos);
  void parseDfont(int fontNum, int *offset, int *pos);
  void readPostTable();
  int seekTable(const char *tag);

  TrueTypeTable *tables;
  int nTables;
  TrueTypeCmap *cmaps;
  int nCmaps;
  int nGlyphs;
  int locaFmt;
  int bbox[4];
  GHash *nameToGID;
  GBool openTypeCFF;
  GBool headlessCFF;
  GBool isDfont;
  GBool isTTC;

  GBool parsedOk;
};

#endif
