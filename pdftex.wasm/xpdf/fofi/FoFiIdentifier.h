//========================================================================
//
// FoFiIdentifier.h
//
// Copyright 2009 Glyph & Cog, LLC
//
//========================================================================

#ifndef FOFIIDENTIFIER_H
#define FOFIIDENTIFIER_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class GList;

//------------------------------------------------------------------------
// FoFiIdentifier
//------------------------------------------------------------------------

enum FoFiIdentifierType {
  fofiIdType1PFA,		// Type 1 font in PFA format
  fofiIdType1PFB,		// Type 1 font in PFB format
  fofiIdCFF8Bit,		// 8-bit CFF font
  fofiIdCFFCID,			// CID CFF font
  fofiIdTrueType,		// TrueType font
  fofiIdTrueTypeCollection,	// TrueType collection
  fofiIdOpenTypeCFF8Bit,	// OpenType wrapper with 8-bit CFF font
  fofiIdOpenTypeCFFCID,		// OpenType wrapper with CID CFF font
  fofiIdDfont,			// Mac OS X dfont
  fofiIdUnknown,		// unknown type
  fofiIdError			// error in reading the file
};

class FoFiIdentifier {
public:

  // Identify a font file.
  static FoFiIdentifierType identifyMem(char *file, int len);
  static FoFiIdentifierType identifyFile(char *fileName);
  static FoFiIdentifierType identifyStream(int (*getChar)(void *data),
					   void *data);

  // Return a list of font names (GString *) in a font collection
  // file.  Indexes into the returned list are indexes into the
  // collection.  This function is only useful with TrueType
  // collections and Mac dfont files.  Returns NULL on error
  // (including invalid font type).
  static GList *getFontList(char *fileName);
};

#endif
