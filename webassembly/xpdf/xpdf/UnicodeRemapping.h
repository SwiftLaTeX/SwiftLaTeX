//========================================================================
//
// UnicodeRemapping.h
//
// Sparse remapping of Unicode characters.
//
// Copyright 2018 Glyph & Cog, LLC
//
//========================================================================

#ifndef UNICODEREMAPPING_H
#define UNICODEREMAPPING_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "CharTypes.h"

struct UnicodeRemappingString;

//------------------------------------------------------------------------

class UnicodeRemapping {
public:

  // Create an empty (i.e., identity) remapping.
  UnicodeRemapping();

  ~UnicodeRemapping();

  // Add a remapping for <in>.
  void addRemapping(Unicode in, Unicode *out, int len);

  // Add entries from the specified file to this UnicodeRemapping.
  void parseFile(GString *fileName);

  // Map <in> to zero or more (up to <size>) output characters in
  // <out>.  Returns the number of output characters.
  int map(Unicode in, Unicode *out, int size);

private:

  Unicode page0[256];
  UnicodeRemappingString *sMap;
  int sMapLen, sMapSize;
};

#endif
