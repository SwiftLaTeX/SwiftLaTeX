//========================================================================
//
// TextString.h
//
// Copyright 2011-2013 Glyph & Cog, LLC
//
// Represents a PDF "text string", which can either be a UTF-16BE
// string (with a leading byte order marker), or an 8-bit string in
// PDFDocEncoding.
//
//========================================================================

#ifndef TEXTSTRING_H
#define TEXTSTRING_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "CharTypes.h"

class GString;

//------------------------------------------------------------------------

class TextString {
public:

  // Create an empty TextString.
  TextString();

  // Create a TextString from a PDF text string.
  TextString(GString *s);

  // Copy a TextString.
  TextString(TextString *s);

  ~TextString();

  // Append a Unicode character or PDF text string to this TextString.
  TextString *append(Unicode c);
  TextString *append(GString *s);

  // Insert a Unicode character, sequence of Unicode characters, or
  // PDF text string in this TextString.
  TextString *insert(int idx, Unicode c);
  TextString *insert(int idx, Unicode *u2, int n);
  TextString *insert(int idx, GString *s);

  // Get the Unicode characters in the TextString.
  int getLength() { return len; }
  Unicode *getUnicode() { return u; }

  // Create a PDF text string from a TextString.
  GString *toPDFTextString();

private:

  void expand(int delta);

  Unicode *u;			// NB: not null-terminated
  int len;
  int size;
};

#endif
