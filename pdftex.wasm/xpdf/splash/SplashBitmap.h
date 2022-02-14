//========================================================================
//
// SplashBitmap.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHBITMAP_H
#define SPLASHBITMAP_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include "SplashTypes.h"

//------------------------------------------------------------------------

// ssize_t isn't well-defined, so define our own
#if SIZE_MAX == UINT_MAX
  typedef int SplashBitmapRowSize;
# define SplashBitmapRowSizeMax INT_MAX
#else
  typedef long long SplashBitmapRowSize;
# define SplashBitmapRowSizeMax LLONG_MAX
#endif

//------------------------------------------------------------------------
// SplashBitmap
//------------------------------------------------------------------------

class SplashBitmap {
public:

  // Create a new bitmap.  It will have <widthA> x <heightA> pixels in
  // color mode <modeA>.  Rows will be padded out to a multiple of
  // <rowPad> bytes.  If <topDown> is false, the bitmap will be stored
  // upside-down, i.e., with the last row first in memory.
  SplashBitmap(int widthA, int heightA, int rowPad,
	       SplashColorMode modeA, GBool alphaA,
	       GBool topDown, SplashBitmap *parentA);

  ~SplashBitmap();

  int getWidth() { return width; }
  int getHeight() { return height; }
  SplashBitmapRowSize getRowSize() { return rowSize; }
  size_t getAlphaRowSize() { return alphaRowSize; }
  SplashColorMode getMode() { return mode; }
  SplashColorPtr getDataPtr() { return data; }
  Guchar *getAlphaPtr() { return alpha; }

  SplashError writePNMFile(char *fileName);
  SplashError writePNMFile(FILE *f);
  SplashError writeAlphaPGMFile(char *fileName);

  void getPixel(int x, int y, SplashColorPtr pixel);
  Guchar getAlpha(int x, int y);

  // Caller takes ownership of the bitmap data.  The SplashBitmap
  // object is no longer valid -- the next call should be to the
  // destructor.
  SplashColorPtr takeData();

private:

  int width, height;		// size of bitmap
  SplashBitmapRowSize rowSize;	// size of one row of data, in bytes
				//   - negative for bottom-up bitmaps
  size_t alphaRowSize;		// size of one row of alpha, in bytes
  SplashColorMode mode;		// color mode
  SplashColorPtr data;		// pointer to row zero of the color data
  Guchar *alpha;		// pointer to row zero of the alpha data
				//   (always top-down)

  // save the last-allocated (large) bitmap data and reuse if possible
  SplashBitmap *parent;
  SplashColorPtr oldData;
  Guchar *oldAlpha;
  SplashBitmapRowSize oldRowSize;
  size_t oldAlphaRowSize;
  int oldHeight;

  friend class Splash;
};


#endif
