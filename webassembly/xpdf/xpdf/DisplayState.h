//========================================================================
//
// DisplayState.h
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef DISPLAYSTATE_H
#define DISPLAYSTATE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "SplashTypes.h"

class GString;
class GList;
class PDFDoc;
class TileMap;
class TileCache;
class TileCompositor;

//------------------------------------------------------------------------
// zoom level
//------------------------------------------------------------------------

// positive zoom levels are percentage of 72 dpi
// (e.g., 50 means 36dpi, 100 means 72dpi, 150 means 108 dpi)

#define zoomPage   -1
#define zoomWidth  -2
#define zoomHeight -3

//------------------------------------------------------------------------
// display mode
//------------------------------------------------------------------------

enum DisplayMode {
  displaySingle,
  displayContinuous,
  displaySideBySideSingle,
  displaySideBySideContinuous,
  displayHorizontalContinuous
};

//------------------------------------------------------------------------
// SelectRect
//------------------------------------------------------------------------

class SelectRect {
public:

  SelectRect(int pageA, double x0A, double y0A, double x1A, double y1A):
    page(pageA), x0(x0A), y0(y0A), x1(x1A), y1(y1A) {}
  int operator==(SelectRect r)
    { return page == r.page && x0 == r.x0 && y0 == r.y0 &&
                               x1 == r.x1 && y1 == r.y1; }
  int operator!=(SelectRect r)
    { return page != r.page || x0 != r.x0 || y0 != r.y0 ||
                               x1 != r.x1 || y1 != r.y1; }

  int page;
  double x0, y0, x1, y1;	// user coords
};


//------------------------------------------------------------------------
// DisplayState
//------------------------------------------------------------------------

class DisplayState {
public:

  DisplayState(int maxTileWidthA, int maxTileHeightA,
	       int tileCacheSizeA, int nWorkerThreadsA,
	       SplashColorMode colorModeA, int bitmapRowPadA);
  ~DisplayState();

  void setTileMap(TileMap *tileMapA)
    { tileMap = tileMapA; }
  void setTileCache(TileCache *tileCacheA)
    { tileCache = tileCacheA; }
  void setTileCompositor(TileCompositor *tileCompositorA)
    { tileCompositor = tileCompositorA; }

  void setPaperColor(SplashColorPtr paperColorA);
  void setMatteColor(SplashColorPtr matteColorA);
  void setSelectColor(SplashColorPtr selectColorA);
  void setReverseVideo(GBool reverseVideoA);
  void setDoc(PDFDoc *docA);
  void setWindowSize(int winWA, int winHA);
  void setDisplayMode(DisplayMode displayModeA);
  void setZoom(double zoomA);
  void setRotate(int rotateA);
  void setScrollPosition(int scrollPageA, int scrollXA, int scrollYA);
  void setSelection(int selectPage, double selectX0, double selectY0,
		    double selectX1, double selectY1);
  void setSelection(GList *selectRectsA);
  void clearSelection();
  void forceRedraw();

  int getMaxTileWidth() { return maxTileWidth; }
  int getMaxTileHeight() { return maxTileHeight; }
  int getTileCacheSize() { return tileCacheSize; }
  int getNWorkerThreads() { return nWorkerThreads; }
  SplashColorMode getColorMode() { return colorMode; }
  int getBitmapRowPad() { return bitmapRowPad; }
  SplashColorPtr getPaperColor() { return paperColor; }
  SplashColorPtr getMatteColor() { return matteColor; }
  SplashColorPtr getSelectColor() { return selectColor; }
  GBool getReverseVideo() { return reverseVideo; }
  PDFDoc *getDoc() { return doc; }
  int getWinW() { return winW; }
  int getWinH() { return winH; }
  DisplayMode getDisplayMode() { return displayMode; }
  GBool displayModeIsContinuous()
    { return displayMode == displayContinuous ||
	     displayMode == displaySideBySideContinuous ||
	     displayMode == displayHorizontalContinuous; }
  GBool displayModeIsSideBySide()
    { return displayMode == displaySideBySideSingle ||
	     displayMode == displaySideBySideContinuous; }
  double getZoom() { return zoom; }
  int getRotate() { return rotate; }
  int getScrollPage() { return scrollPage; }
  int getScrollX() { return scrollX; }
  int getScrollY() { return scrollY; }
  GBool hasSelection() { return selectRects != NULL; }
  GList *getSelectRects() { return selectRects; }
  int getNumSelectRects();
  SelectRect *getSelectRect(int idx);
  void optionalContentChanged();

private:

  int maxTileWidth, maxTileHeight;
  int tileCacheSize;
  int nWorkerThreads;

  SplashColorMode colorMode;
  int bitmapRowPad;

  TileMap *tileMap;
  TileCache *tileCache;
  TileCompositor *tileCompositor;

  SplashColor paperColor;
  SplashColor matteColor;
  SplashColor selectColor;
  GBool reverseVideo;

  PDFDoc *doc;

  int winW, winH;		// window (draw area) size
  DisplayMode displayMode;
  double zoom;			// zoom level (see zoom* defines, above)
  int rotate;			// rotation (0, 90, 180, or 270)
  int scrollPage;		// scroll page - only used in
				//   non-continuous modes
  int scrollX, scrollY;

  GList *selectRects;		// selection rectangles [SelectRect]
				//   (NULL if there is no selection)


};

#endif
