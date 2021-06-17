//========================================================================
//
// TileMap.h
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef TILEMAP_H
#define TILEMAP_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"

class GList;
class DisplayState;

//------------------------------------------------------------------------

// Tile descriptor: this is all of the information needed to specify a
// tile.
class TileDesc {
public:

  TileDesc(int pageA, int rotateA, double dpiA,
	   int txA, int tyA, int twA, int thA):
    page(pageA), rotate(rotateA), dpi(dpiA),
    tx(txA), ty(tyA), tw(twA), th(thA) {}

  GBool matches(TileDesc *tile) {
    return page == tile->page && rotate == tile->rotate &&
           dpi == tile->dpi &&
           tx == tile->tx && ty == tile->ty &&
           tw == tile->tw && th == tile->th;
  }

  int page;			// page number
  int rotate;			// rotation
  double dpi;			// tile resolution
  int tx, ty;			// origin of tile, relative to top left
				//   corner of page
  int tw, th;			// tile size
};

//------------------------------------------------------------------------

// A TileDesc plus the tile's location in the display.
class PlacedTileDesc: public TileDesc {
public:

  PlacedTileDesc(int pageA, int rotateA, double dpiA,
		 int txA, int tyA, int twA, int thA,
		 int pxA, int pyA):
    TileDesc(pageA, rotateA, dpiA, txA, tyA, twA, thA), px(pxA), py(pyA) {}

  int px, py;			// position, in window coordinates, of
				//   tile origin
};

//------------------------------------------------------------------------

// Tile map: this maps window position to tile, and vice versa; and
// computes the list of tiles needed for the current display.
class TileMap {
public:

  TileMap(DisplayState *stateA);
  ~TileMap();

  // Returns a list of PlacedTileDesc objects describing the tiles
  // needed for the current display.  The returned list is owned by
  // the TileMap object (and may be reused) -- the caller should not
  // modify or free it.
  GList *getTileList();

  // Return the max values for the horizontal and vertical scrollbars.
  // Scroll thumbs should be winW and winH.
  void getScrollLimits(int *horizMax, int *vertMax);

  // Coordinate conversion functions.
  // - user space: per-page, as defined by PDF file; unit = point
  // - device space: (0,0) is upper-left corner of a page; unit = pixel
  // - window space: (0,0) is upper-left corner of drawing area; unit = pixel
  // Conversions to window space return false if the specified page
  // isn't visible (in non-continuous modes).  Conversions from window
  // space return false if the point isn't on a page.  Otherwise,
  // these functions do not do bounds-checking or any clipping to
  // window or page coordinates.
  GBool cvtWindowToUser(int xw, int yw,
			int *pg, double *xu, double *yu);
  GBool cvtWindowToDev(int xw, int yw,
		       int *pg, int *xd, int *yd);
  GBool cvtUserToWindow(int pg, double xu, double yu,
			int *xw, int *yw);
  GBool cvtDevToWindow(int pg, int xd, int yd,
		       int *xw, int *yw);
  void cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd);
  void cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu);

  // Return the range of pages intersected by a rectangle in window
  // coordinates.  E.g., these are the pages that will need to be
  // redrawn for a redraw event covering that rectangle.
  void getWindowPageRange(int x, int y, int w, int h,
			  int *firstPage, int *lastPage);

  // Return the scrollY setting that would place the top of <page> at
  // the top of the window.  (In non-continuous modes, this just
  // returns zero.)
  int getPageTopY(int page);

  // Return the scrollY setting that would place the bottom of <page>
  // at the bottom of the window.
  int getPageBottomY(int page);

  // Return the scrollX setting that would place the left side of
  // <page> at the left side of the window.  (In non-continuous modes,
  // this just returns zero.)
  int getPageLeftX(int page);

  // Return the scrollX setting that would place the right side of
  // <page> at the right side of the window.
  int getPageRightX(int page);

  // In continuous modes: return the lowest page number visible
  // anywhere in the window; in non-continuous modes: return the
  // current page.
  int getFirstPage();

  // In continuous modes: return the lowest page number whose top (or
  // left) edge is visible in the window (or if no top/left edge is
  // visilble, the page after the single visible page); in
  // non-continuous modes: return the current page.
  int getFirstPageTop();

  // In continuous modes: return the page number at the center of the
  // window; in non-continuous modes: return the current page.
  int getMidPage();

  // In continuous modes: return the highest page number visible
  // anywhere in the window; in non-continuous modes: return the
  // current page.
  int getLastPage();

  // Get the resolution at which the specified will be rasterized.
  double getDPI(int page);

  // Get the width and height of a page, in points.
  double getPageBoxWidth(int page);
  double getPageBoxHeight(int page);

  // Get the number of pixels (vertically) between adjacent pages in
  // continuous / side-by-side continuous mode.
  int getContinuousPageSpacing();

  // Get the number of pixels (horizontally) between facing pages in
  // the side-by-side modes.
  int getSideBySidePageSpacing();

  // Get the number of pixels (horizontally) between adjacent pages in
  // horizontal continuous mode.
  int getHorizContinuousPageSpacing();

  void docChanged();
  void windowSizeChanged();
  void displayModeChanged();
  void zoomChanged();
  void rotateChanged();
  void scrollPositionChanged();
  void forceRedraw();

private:

  // Clear the pageDPI, pageW, pageH, tileW, and tileH arrays.
  void clearPageParams();

  // Update pageDPI, pageW, pageH, tileW, tileH (if needed).
  void updatePageParams();

  // Clear the pageX/pageY array.
  void clearContinuousModeParams();

  // Update pageX, pageY, maxW, maxW2, maxH, totalW, totalH.
  void updateContinuousModeParams();

  // Compute the user-to-device transform matrix for the specified
  // page.
  void computePageMatrix(int page, double *m);

  // Invert a matrix.
  void invertMatrix(double *m, double *im);

  // Return the page number at y.  If y falls in a gap between pages,
  // return the next page.  If y falls after the last page, return
  // numPages + 1.
  int findContinuousPage(int y);

  // Return the left-side page number at y.  If y falls in a gap between
  // page pairs, return the next page.  If y falls after the last page,
  // return numPages + 1 (or numPages + 2 if numPages is odd).
  int findSideBySideContinuousPage(int y);

  // Return the page number at x.  If x falls in a gap between pages,
  // return the next page.  If x falls after the last page, return
  // numPages + 1.
  int findHorizContinuousPage(int x);

  DisplayState *state;

  double *pageBoxW, *pageBoxH;	// page sizes, in points

  // Page parameters:
  //   pageDPI[pg-1] = rasterization resolution
  //   pageW[pg-1], pageH[pg-1] = size of rasterized page
  //   tileW[pg-1], tileH[pg-1] = size of tiles
  double *pageDPI;
  int *pageW, *pageH;
  int *tileW, *tileH;

  // In displayContinuous mode:
  //   pageY[pg-1] = top edge of page
  //   maxW = max page width
  //   totalH = total page height (including gaps)
  // In displaySideBySideContinuous mode:
  //   pageY[pg-1] = top edge of page-pair
  //   pageY[pg] = top edge of page-pair (if pg+1 <= nPages)
  //   maxW = max left-side page width
  //   maxW2 = max right-side page width
  //   totalH = total page-pair height (including gaps)
  // In displayHorizontalContinuous mode:
  //   pageX[pg-1] = left edge of page
  //   maxH = max page height
  //   totalW = total page width (including gaps)
  // Only one of pageX or pageY is used at a time, so a single array
  // is allocated, i.e., pageX = pageY.
  int *pageX, *pageY;
  int maxW, maxW2, maxH;
  int totalW, totalH;

  GList *tiles;
};

#endif
