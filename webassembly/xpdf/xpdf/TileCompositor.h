//========================================================================
//
// TileCompositor.h
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef TILECOMPOSITOR_H
#define TILECOMPOSITOR_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "SplashTypes.h"

class GList;
class SplashBitmap;
class DisplayState;
class TileCache;

//------------------------------------------------------------------------

class TileCompositor {
public:

  TileCompositor(DisplayState *stateA,
		 TileMap *tileMapA, TileCache *tileCacheA);
  ~TileCompositor();

  // Returns the window bitmap.  The returned bitmap is owned by the
  // TileCompositor object (and may be reused) -- the caller should
  // not modify or free it.  If <finished> is is non-NULL, *<finished>
  // will be set to true if all of the needed tiles are finished,
  // i.e., if the returned bitmap is complete.
  SplashBitmap *getBitmap(GBool *finished);

  void paperColorChanged();
  void matteColorChanged();
  void selectColorChanged();
  void reverseVideoChanged();
  void docChanged();
  void windowSizeChanged();
  void displayModeChanged();
  void zoomChanged();
  void rotateChanged();
  void scrollPositionChanged();
  void selectionChanged();
  void regionsChanged();
  void optionalContentChanged();
  void forceRedraw();

private:

  void clearBitmap();
  void blit(SplashBitmap *srcBitmap, int xSrc, int ySrc,
	    SplashBitmap *destBitmap, int xDest, int yDest,
	    int w, int h, GBool compositeWithPaper);
  void fill(int xDest, int yDest, int w, int h,
	    SplashColorPtr color);
  void drawSelection();
  void applySelection(int xDest, int yDest, int w, int h,
		      SplashColorPtr color);

  DisplayState *state;
  TileMap *tileMap;
  TileCache *tileCache;

  SplashBitmap *bitmap;
  GBool bitmapValid;

};

#endif
