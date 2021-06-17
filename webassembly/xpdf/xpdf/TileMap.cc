//========================================================================
//
// TileMap.cc
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "gmempp.h"
#include "GList.h"
#include "PDFDoc.h"
#include "DisplayState.h"
#include "TileMap.h"

//------------------------------------------------------------------------

// Number of pixels of matte color between pages (above and below each
// other) in continuous mode.
#define continuousPageSpacing 3

// Number of pixels of matte color between facing pages (left and
// right of each other) in side-by-side mode.
#define sideBySidePageSpacing 3

// Number of pixels of matte color between pages (left and right of
// each other) in horizontal continuous mode.
#define horizContinuousPageSpacing 3

//------------------------------------------------------------------------

TileMap::TileMap(DisplayState *stateA) {
  state = stateA;
  state->setTileMap(this);
  pageDPI = NULL;
  pageW = pageH = NULL;
  tileW = tileH = NULL;
  pageBoxW = pageBoxH = NULL;
  pageX = pageY = NULL;
  tiles = NULL;
}

TileMap::~TileMap() {
  clearPageParams();
  clearContinuousModeParams();
  gfree(pageBoxW);
  gfree(pageBoxH);
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
  }
}

GList *TileMap::getTileList() {
  double pageDPI1, pageDPI2;
  int pageW1, pageH1, tileW1, tileH1, pageW2, pageH2, tileW2, tileH2;
  int offsetX, offsetY, offsetX2;
  int x0, y0, x1, y1, x, y, tx, ty, tw, th, page;

  if (tiles) {
    return tiles;
  }

  tiles = new GList();

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    return tiles;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {

  case displaySingle:
    page = state->getScrollPage();
    pageDPI1 = pageDPI[page - 1];
    pageW1 = pageW[page - 1];
    pageH1 = pageH[page - 1];
    tileW1 = tileW[page - 1];
    tileH1 = tileH[page - 1];
    if (pageW1 < state->getWinW()) {
      offsetX = (state->getWinW() - pageW1) / 2;
    } else {
      offsetX = 0;
    }
    if (pageH1 < state->getWinH()) {
      offsetY = (state->getWinH() - pageH1) / 2;
    } else {
      offsetY = 0;
    }
    if ((x0 = state->getScrollX() - offsetX) < 0) {
      x0 = 0;
    }
    if ((y0 = state->getScrollY() - offsetY) < 0) {
      y0 = 0;
    }
    if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX) >= pageW1) {
      x1 = pageW1 - 1;
    }
    if ((y1 = state->getScrollY() + state->getWinH() - 1 - offsetY) >= pageH1) {
      y1 = pageH1 - 1;
    }
    for (y = y0 / tileH1; y <= y1 / tileH1; ++y) {
      for (x = x0 / tileW1; x <= x1 / tileW1; ++x) {
	tx = x * tileW1;
	ty = y * tileH1;
	tw = tileW1;
	if (tx + tw > pageW1) {
	  tw = pageW1 - tx;
	}
	th = tileH1;
	if (ty + th > pageH1) {
	  th = pageH1 - ty;
	}
	tiles->append(new PlacedTileDesc(page, state->getRotate(), pageDPI1,
					 tx, ty, tw, th,
					 tx - state->getScrollX() + offsetX,
					 ty - state->getScrollY() + offsetY));
      }
    }
    break;

  case displayContinuous:
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    page = findContinuousPage(state->getScrollY());
    while (page <= state->getDoc()->getNumPages() &&
	   pageY[page - 1] < state->getScrollY() + state->getWinH()) {
      pageDPI1 = pageDPI[page - 1];
      pageW1 = pageW[page - 1];
      pageH1 = pageH[page - 1];
      tileW1 = tileW[page - 1];
      tileH1 = tileH[page - 1];
      if (maxW < state->getWinW()) {
	offsetX = (state->getWinW() - maxW) / 2;
      } else {
	offsetX = 0;
      }
      offsetX += (maxW - pageW1) / 2;
      if ((x0 = state->getScrollX() - offsetX) < 0) {
	x0 = 0;
      }
      if ((y0 = state->getScrollY() - pageY[page - 1] - offsetY) < 0) {
	y0 = 0;
      }
      if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX)
	  >= pageW1) {
	x1 = pageW1 - 1;
      }
      if ((y1 = state->getScrollY() - pageY[page - 1]
	        + state->getWinH() - 1 - offsetY)
	  >= pageH1) {
	y1 = pageH1 - 1;
      }
      for (y = y0 / tileH1; y <= y1 / tileH1; ++y) {
	for (x = x0 / tileW1; x <= x1 / tileW1; ++x) {
	  tx = x * tileW1;
	  ty = y * tileH1;
	  tw = tileW1;
	  if (tx + tw > pageW1) {
	    tw = pageW1 - tx;
	  }
	  th = tileH1;
	  if (ty + th > pageH1) {
	    th = pageH1 - ty;
	  }
	  tiles->append(new PlacedTileDesc(
				page, state->getRotate(), pageDPI1,
				tx, ty, tw, th,
				tx - state->getScrollX() + offsetX,
				ty - state->getScrollY() + pageY[page - 1]
				  + offsetY));
	}
      }
      ++page;
    }
    break;

  case displaySideBySideSingle:
    page = state->getScrollPage();
    pageDPI1 = pageDPI[page - 1];
    pageW1 = pageW[page - 1];
    pageH1 = pageH[page - 1];
    tileW1 = tileW[page - 1];
    tileH1 = tileH[page - 1];
    if (page + 1 <= state->getDoc()->getNumPages()) {
      pageDPI2 = pageDPI[page];
      pageW2 = pageW[page];
      pageH2 = pageH[page];
      tileW2 = tileW[page];
      tileH2 = tileH[page];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageDPI2 = pageDPI1;
      pageW2 = pageW1;
      pageH2 = pageH1;
      tileW2 = tileW1;
      tileH2 = tileH1;
    }
    if (pageW1 + sideBySidePageSpacing + pageW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (pageW1 + sideBySidePageSpacing + pageW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    if (pageH1 < state->getWinH() && pageH2 < state->getWinH()) {
      if (pageH1 > pageH2) {
	offsetY = (state->getWinH() - pageH1) / 2;
      } else {
	offsetY = (state->getWinH() - pageH2) / 2;
      }
    } else {
      offsetY = 0;
    }
    // left page
    if ((x0 = state->getScrollX() - offsetX) < 0) {
      x0 = 0;
    }
    if ((y0 = state->getScrollY() - offsetY) < 0) {
      y0 = 0;
    }
    if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX) >= pageW1) {
      x1 = pageW1 - 1;
    } else if (x1 < 0) {
      x1 = -tileW2;
    }
    if ((y1 = state->getScrollY() + state->getWinH() - 1 - offsetY) >= pageH1) {
      y1 = pageH1 - 1;
    } else if (y1 < 0) {
      y1 = -tileH2;
    }
    for (y = y0 / tileH1; y <= y1 / tileH1; ++y) {
      for (x = x0 / tileW1; x <= x1 / tileW1; ++x) {
	tx = x * tileW1;
	ty = y * tileH1;
	tw = tileW1;
	if (tx + tw > pageW1) {
	  tw = pageW1 - tx;
	}
	th = tileH1;
	if (ty + th > pageH1) {
	  th = pageH1 - ty;
	}
	tiles->append(new PlacedTileDesc(page,
					 state->getRotate(), pageDPI1,
					 tx, ty, tw, th,
					 tx - state->getScrollX() + offsetX,
					 ty - state->getScrollY() + offsetY));
      }
    }
    // right page
    if (page + 1 <= state->getDoc()->getNumPages()) {
      if ((x0 = state->getScrollX() - offsetX2) < 0) {
	x0 = 0;
      }
      if ((y0 = state->getScrollY() - offsetY) < 0) {
	y0 = 0;
      }
      if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX2)
	  >= pageW2) {
	x1 = pageW2 - 1;
      } else if (x1 < 0) {
	x1 = -tileW2;
      }
      if ((y1 = state->getScrollY() + state->getWinH() - 1 - offsetY)
	  >= pageH2) {
	y1 = pageH2 - 1;
      } else if (y1 < 0) {
	y1 = -tileH2;
      }
      for (y = y0 / tileH2; y <= y1 / tileH2; ++y) {
	for (x = x0 / tileW2; x <= x1 / tileW2; ++x) {
	  tx = x * tileW2;
	  ty = y * tileH2;
	  tw = tileW2;
	  if (tx + tw > pageW2) {
	    tw = pageW2 - tx;
	  }
	  th = tileH2;
	  if (ty + th > pageH2) {
	    th = pageH2 - ty;
	  }
	  tiles->append(new PlacedTileDesc(page + 1,
					   state->getRotate(), pageDPI2,
					   tx, ty, tw, th,
					   tx - state->getScrollX() + offsetX2,
					   ty - state->getScrollY() + offsetY));
	}
      }
    }
    break;

  case displaySideBySideContinuous:
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    page = findSideBySideContinuousPage(state->getScrollY());
    while (page <= state->getDoc()->getNumPages() &&
	   (pageY[page - 1] < state->getScrollY() + state->getWinH() ||
	    (page + 1 <= state->getDoc()->getNumPages() &&
	     pageY[page] < state->getScrollY() + state->getWinH()))) {
      pageDPI1 = pageDPI[page - 1];
      pageW1 = pageW[page - 1];
      pageH1 = pageH[page - 1];
      tileW1 = tileW[page - 1];
      tileH1 = tileH[page - 1];
      if (page + 1 <= state->getDoc()->getNumPages()) {
	pageDPI2 = pageDPI[page];
	pageW2 = pageW[page];
	pageH2 = pageH[page];
	tileW2 = tileW[page];
	tileH2 = tileH[page];
      } else {
	// display a single page as though there were a blank facing
	// page of the same size
	pageDPI2 = pageDPI1;
	pageW2 = pageW1;
	pageH2 = pageH1;
	tileW2 = tileW1;
	tileH2 = tileH1;
      }
      if (maxW + sideBySidePageSpacing + maxW2 < state->getWinW()) {
	offsetX = (state->getWinW() -
		   (maxW + sideBySidePageSpacing + maxW2)) / 2;
      } else {
	offsetX = 0;
      }
      offsetX += maxW - pageW1;
      offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
      // left page
      if ((x0 = state->getScrollX() - offsetX) < 0) {
	x0 = 0;
      }
      if ((y0 = state->getScrollY() - pageY[page - 1] - offsetY) < 0) {
	y0 = 0;
      }
      if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX)
	  >= pageW1) {
	x1 = pageW1 - 1;
      } else if (x1 < 0) {
	x1 = -tileW2;
      }
      if ((y1 = state->getScrollY() - pageY[page - 1]
	        + state->getWinH() - 1 - offsetY)
	  >= pageH1) {
	y1 = pageH1 - 1;
      } else if (y1 < 0) {
	y1 = -tileH2;
      }
      for (y = y0 / tileH1; y <= y1 / tileH1; ++y) {
	for (x = x0 / tileW1; x <= x1 / tileW1; ++x) {
	  tx = x * tileW1;
	  ty = y * tileH1;
	  tw = tileW1;
	  if (tx + tw > pageW1) {
	    tw = pageW1 - tx;
	  }
	  th = tileH1;
	  if (ty + th > pageH1) {
	    th = pageH1 - ty;
	  }
	  tiles->append(new PlacedTileDesc(
				page, state->getRotate(), pageDPI1,
				tx, ty, tw, th,
				tx - state->getScrollX() + offsetX,
				ty - state->getScrollY() + pageY[page - 1]
				  + offsetY));
	}
      }
      ++page;
      // right page
      if (page <= state->getDoc()->getNumPages()) {
	if ((x0 = state->getScrollX() - offsetX2) < 0) {
	  x0 = 0;
	}
	if ((y0 = state->getScrollY() - pageY[page - 1] - offsetY) < 0) {
	  y0 = 0;
	}
	if ((x1 = state->getScrollX() + state->getWinW() - 1 - offsetX2)
	    >= pageW2) {
	  x1 = pageW2 - 1;
	} else if (x1 < 0) {
	  x1 = -tileW2;
	}
	if ((y1 = state->getScrollY() - pageY[page - 1]
	          + state->getWinH() - 1 - offsetY)
	    >= pageH2) {
	  y1 = pageH2 - 1;
	} else if (y1 < 0) {
	  y1 = -tileH2;
	}
	for (y = y0 / tileH2; y <= y1 / tileH2; ++y) {
	  for (x = x0 / tileW2; x <= x1 / tileW2; ++x) {
	    tx = x * tileW2;
	    ty = y * tileH2;
	    tw = tileW2;
	    if (tx + tw > pageW2) {
	      tw = pageW2 - tx;
	    }
	    th = tileH2;
	    if (ty + th > pageH2) {
	      th = pageH2 - ty;
	    }
	    tiles->append(new PlacedTileDesc(
				  page, state->getRotate(), pageDPI2,
				  tx, ty, tw, th,
				  tx - state->getScrollX() + offsetX2,
				  ty - state->getScrollY() + pageY[page - 1]
				    + offsetY));
	  }
	}
      }
      ++page;
    }
    break;

  case displayHorizontalContinuous:
    if (totalW < state->getWinW()) {
      offsetX = (state->getWinW() - totalW) / 2;
    } else {
      offsetX = 0;
    }
    page = findHorizContinuousPage(state->getScrollX());
    while (page <= state->getDoc()->getNumPages() &&
	   pageX[page - 1] < state->getScrollX() + state->getWinW()) {
      pageDPI1 = pageDPI[page - 1];
      pageW1 = pageW[page - 1];
      pageH1 = pageH[page - 1];
      tileW1 = tileW[page - 1];
      tileH1 = tileH[page - 1];
      if (maxH < state->getWinH()) {
	offsetY = (state->getWinH() - maxH) / 2;
      } else {
	offsetY = 0;
      }
      if ((x0 = state->getScrollX() - pageX[page - 1] - offsetX) < 0) {
	x0 = 0;
      }
      if ((y0 = state->getScrollY() - offsetY) < 0) {
	y0 = 0;
      }
      if ((x1 = state->getScrollX() - pageX[page - 1]
	        + state->getWinW() - 1 - offsetX)
	  >= pageW1) {
	x1 = pageW1 - 1;
      }
      if ((y1 = state->getScrollY() + state->getWinH() - 1 - offsetY)
	  >= pageH1) {
	y1 = pageH1 - 1;
      }
      for (y = y0 / tileH1; y <= y1 / tileH1; ++y) {
	for (x = x0 / tileW1; x <= x1 / tileW1; ++x) {
	  tx = x * tileW1;
	  ty = y * tileH1;
	  tw = tileW1;
	  if (tx + tw > pageW1) {
	    tw = pageW1 - tx;
	  }
	  th = tileH1;
	  if (ty + th > pageH1) {
	    th = pageH1 - ty;
	  }
	  tiles->append(new PlacedTileDesc(
				page, state->getRotate(), pageDPI1,
				tx, ty, tw, th,
				tx - state->getScrollX() + pageX[page - 1]
				  + offsetX,
				ty - state->getScrollY() + offsetY));
	}
      }
      ++page;
    }
    break;
  }

  return tiles;
}

void TileMap::getScrollLimits(int *horizMax, int *vertMax) {
  int pageW1, pageH1, pageW2, pageH2;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    *horizMax = *vertMax = 0;
    return;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {
  case displaySingle:
    *horizMax = pageW[state->getScrollPage() - 1];
    *vertMax = pageH[state->getScrollPage() - 1];
    break;
  case displayContinuous:
    *horizMax = maxW;
    *vertMax = totalH;
    break;
  case displaySideBySideSingle:
    pageW1 = pageW[state->getScrollPage() - 1];
    pageH1 = pageH[state->getScrollPage() - 1];
    if (state->getScrollPage() + 1 <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[state->getScrollPage()];
      pageH2 = pageH[state->getScrollPage()];
    } else {
      pageW2 = pageW1;
      pageH2 = pageH1;
    }
    *horizMax = pageW1 + sideBySidePageSpacing + pageW2;
    *vertMax = pageH1 > pageH2 ? pageH1 : pageH2;
    break;
  case displaySideBySideContinuous:
    *horizMax = maxW + sideBySidePageSpacing + maxW2;
    *vertMax = totalH;
    break;
  case displayHorizontalContinuous:
    *horizMax = totalW;
    *vertMax = maxH;
    break;
  default: // should never happen
    *horizMax = *vertMax = 0;
    break;
  }
}

GBool TileMap::cvtWindowToUser(int xw, int yw,
			       int *pg, double *xu, double *yu) {
  GBool ok;
  int xd, yd;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    *pg = 0;
    *xu = *yu = 0;
    return gFalse;
  }

  ok = cvtWindowToDev(xw, yw, pg, &xd, &yd);
  cvtDevToUser(*pg, xd, yd, xu, yu);
  return ok;
}

GBool TileMap::cvtWindowToDev(int xw, int yw,
			      int *pg, int *xd, int *yd) {
  int pageW1, pageH1, pageW2, pageH2, offsetX, offsetX2, offsetY;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    *pg = 0;
    *xd = *yd = 0;
    return gFalse;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {

  case displaySingle:
    *pg = state->getScrollPage();
    pageW1 = pageW[*pg - 1];
    pageH1 = pageH[*pg - 1];
    if (pageW1 < state->getWinW()) {
      offsetX = (state->getWinW() - pageW1) / 2;
    } else {
      offsetX = 0;
    }
    if (pageH1 < state->getWinH()) {
      offsetY = (state->getWinH() - pageH1) / 2;
    } else {
      offsetY = 0;
    }
    *xd = xw - offsetX + state->getScrollX();
    *yd = yw - offsetY + state->getScrollY();
    return *xd >= 0 && *xd < pageW1 && *yd >= 0 && *yd < pageH1;

  case displayContinuous:
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    *pg = findContinuousPage(yw - offsetY + state->getScrollY());
    if (*pg < 1 || *pg > state->getDoc()->getNumPages()) {
      *pg = 0;
      *xd = *yd = 0;
      return gFalse;
    }
    pageW1 = pageW[*pg - 1];
    pageH1 = pageH[*pg - 1];
    if (maxW < state->getWinW()) {
      offsetX = (state->getWinW() - maxW) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += (maxW - pageW1) / 2;
    *xd = xw - offsetX + state->getScrollX();
    *yd = yw - offsetY - pageY[*pg - 1] + state->getScrollY();
    return *xd >= 0 && *xd < pageW1 && *yd >= 0 && *yd < pageH1;

  case displaySideBySideSingle:
    pageW1 = pageW[state->getScrollPage() - 1];
    pageH1 = pageH[state->getScrollPage() - 1];
    if (state->getScrollPage() + 1 <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[state->getScrollPage()];
      pageH2 = pageH[state->getScrollPage()];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageW2 = pageW1;
      pageH2 = pageH1;
    }
    if (pageW1 + sideBySidePageSpacing + pageW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (pageW1 + sideBySidePageSpacing + pageW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    if (pageH1 < state->getWinH() && pageH2 < state->getWinH()) {
      if (pageH1 > pageH2) {
	offsetY = (state->getWinH() - pageH1) / 2;
      } else {
	offsetY = (state->getWinH() - pageH2) / 2;
      }
    } else {
      offsetY = 0;
    }
    if (xw + state->getScrollX() < offsetX2) {
      *pg = state->getScrollPage();
      *xd = xw - offsetX + state->getScrollX();
      *yd = yw - offsetY + state->getScrollY();
      return *xd >= 0 && *xd < pageW1 && *yd >= 0 && *yd < pageH1;
    } else {
      if (state->getScrollPage() + 1 > state->getDoc()->getNumPages()) {
	*pg = *xd = *yd = 0;
	return gFalse;
      }
      *pg = state->getScrollPage() + 1;
      *xd = xw - offsetX2 + state->getScrollX();
      *yd = yw - offsetY + state->getScrollY();
      return *xd >= 0 && *xd < pageW2 && *yd >= 0 && *yd < pageH2;
    }

  case displaySideBySideContinuous:
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    *pg = findSideBySideContinuousPage(yw - offsetY + state->getScrollY());
    if (*pg < 1 || *pg > state->getDoc()->getNumPages()) {
      *pg = 0;
      *xd = *yd = 0;
      return gFalse;
    }
    pageW1 = pageW[*pg - 1];
    pageH1 = pageH[*pg - 1];
    if (*pg + 1 <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[*pg];
      pageH2 = pageH[*pg];
    } else {
      pageW2 = pageH2 = 0;
    }
    if (maxW + sideBySidePageSpacing + maxW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (maxW + sideBySidePageSpacing + maxW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += maxW - pageW1;
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    if (xw + state->getScrollX() < offsetX2) {
      *xd = xw - offsetX + state->getScrollX();
      *yd = yw - offsetY - pageY[*pg - 1] + state->getScrollY();
      return *xd >= 0 && *xd < pageW1 && *yd >= 0 && *yd < pageH1;
    } else {
      if (*pg + 1 > state->getDoc()->getNumPages()) {
	*pg = *xd = *yd = 0;
	return false;
      }
      ++*pg;
      *xd = xw - offsetX2 + state->getScrollX();
      *yd = yw - offsetY - pageY[*pg - 1] + state->getScrollY();
      return *xd >= 0 && *xd < pageW2 && *yd >= 0 && *yd < pageH2;
    }

  case displayHorizontalContinuous:
    if (totalW < state->getWinW()) {
      offsetX = (state->getWinW() - totalW) / 2;
    } else {
      offsetX = 0;
    }
    *pg = findHorizContinuousPage(xw - offsetX + state->getScrollX());
    if (*pg < 1 || *pg > state->getDoc()->getNumPages()) {
      *pg = 0;
      *xd = *yd = 0;
      return gFalse;
    }
    pageW1 = pageW[*pg - 1];
    pageH1 = pageH[*pg - 1];
    if (maxH < state->getWinH()) {
      offsetY = (state->getWinH() - maxH) / 2;
    } else {
      offsetY = 0;
    }
    *xd = xw - offsetX - pageX[*pg - 1] + state->getScrollX();
    *yd = yw - offsetY + state->getScrollY();
    return *xd >= 0 && *xd < pageW1 && *yd >= 0 && *yd < pageH1;
  }

  return gFalse;
}

GBool TileMap::cvtUserToWindow(int pg, double xu, double yu,
			       int *xw, int *yw) {
  int xd, yd;

  cvtUserToDev(pg, xu, yu, &xd, &yd);
  return cvtDevToWindow(pg, xd, yd, xw, yw);
}

GBool TileMap::cvtDevToWindow(int pg, int xd, int yd,
			      int *xw, int *yw) {
  int leftPg, pageW1, pageH1, pageW2, pageH2, offsetX, offsetX2, offsetY;

  if (!state->getDoc() ||
      pg < 1 || pg > state->getDoc()->getNumPages()) {
    *xw = *yw = 0;
    return gFalse;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {

  case displaySingle:
    if (pg != state->getScrollPage()) {
      *xw = *yw = 0;
      return gFalse;
    }
    pageW1 = pageW[pg - 1];
    pageH1 = pageH[pg - 1];
    if (pageW1 < state->getWinW()) {
      offsetX = (state->getWinW() - pageW1) / 2;
    } else {
      offsetX = 0;
    }
    if (pageH1 < state->getWinH()) {
      offsetY = (state->getWinH() - pageH1) / 2;
    } else {
      offsetY = 0;
    }
    *xw = xd + offsetX - state->getScrollX();
    *yw = yd + offsetY - state->getScrollY();
    break;

  case displayContinuous:
    pageW1 = pageW[pg - 1];
    pageH1 = pageH[pg - 1];
    if (maxW < state->getWinW()) {
      offsetX = (state->getWinW() - maxW) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += (maxW - pageW1) / 2;
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    *xw = xd + offsetX - state->getScrollX();
    *yw = pageY[pg - 1] + yd + offsetY - state->getScrollY();
    break;

  case displaySideBySideSingle:
    if (!(pg == state->getScrollPage() ||
	  (pg == state->getScrollPage() + 1 &&
	   state->getScrollPage() + 1 <= state->getDoc()->getNumPages()))) {
      *xw = *yw = 0;
      return gFalse;
    }
    pageW1 = pageW[state->getScrollPage() - 1];
    pageH1 = pageH[state->getScrollPage() - 1];
    if (state->getScrollPage() + 1 <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[state->getScrollPage()];
      pageH2 = pageH[state->getScrollPage()];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageW2 = pageW1;
      pageH2 = pageH1;
    }
    if (pageW1 + sideBySidePageSpacing + pageW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (pageW1 + sideBySidePageSpacing + pageW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    if (pageH1 < state->getWinH() && pageH2 < state->getWinH()) {
      if (pageH1 > pageH2) {
	offsetY = (state->getWinH() - pageH1) / 2;
      } else {
	offsetY = (state->getWinH() - pageH2) / 2;
      }
    } else {
      offsetY = 0;
    }
    if (pg == state->getScrollPage()) {
      *xw = xd + offsetX - state->getScrollX();
      *yw = yd + offsetY - state->getScrollY();
    } else {
      *xw = xd + offsetX2 - state->getScrollX();
      *yw = yd + offsetY - state->getScrollY();
    }
    break;

  case displaySideBySideContinuous:
    leftPg = (pg - 1) | 1;
    pageW1 = pageW[leftPg - 1];
    pageH1 = pageH[leftPg - 1];
    if (maxW + sideBySidePageSpacing + maxW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (maxW + sideBySidePageSpacing + maxW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += maxW - pageW1;
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    if (totalH < state->getWinH()) {
      offsetY = (state->getWinH() - totalH) / 2;
    } else {
      offsetY = 0;
    }
    if (pg == leftPg) {
      *xw = xd + offsetX - state->getScrollX();
    } else {
      *xw = xd + offsetX2 - state->getScrollX();
    }
    *yw = pageY[pg - 1] + yd + offsetY - state->getScrollY();
    break;

  case displayHorizontalContinuous:
    if (totalW < state->getWinW()) {
      offsetX = (state->getWinW() - totalW) / 2;
    } else {
      offsetX = 0;
    }
    if (maxH < state->getWinH()) {
      offsetY = (state->getWinH() - maxH) / 2;
    } else {
      offsetY = 0;
    }
    *xw = pageX[pg - 1] + xd + offsetX - state->getScrollX();
    *yw = yd + offsetY - state->getScrollY();
    break;
  }

  return gTrue;
}

void TileMap::cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd) {
  double m[6];

  if (!state->getDoc() ||
      pg < 1 || pg > state->getDoc()->getNumPages()) {
    *xd = *yd = 0;
    return;
  }

  computePageMatrix(pg, m);
  *xd = (int)(xu * m[0] + yu * m[2] + m[4] + 0.5);
  *yd = (int)(xu * m[1] + yu * m[3] + m[5] + 0.5);
}

void TileMap::cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu) {
  double m[6], im[6];
  
  if (!state->getDoc() ||
      pg < 1 || pg > state->getDoc()->getNumPages()) {
    *xu = *yu = 0;
    return;
  }

  computePageMatrix(pg, m);
  invertMatrix(m, im);
  *xu = xd * im[0] + yd * im[2] + im[4];
  *yu = xd * im[1] + yd * im[3] + im[5];
}

void TileMap::getWindowPageRange(int x, int y, int w, int h,
				 int *firstPage, int *lastPage) {
  GList *tiles;
  PlacedTileDesc *tile;
  int i;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    *firstPage = *lastPage = 0;
    return;
  }
  *firstPage = state->getDoc()->getNumPages();
  *lastPage = 0;
  tiles = getTileList();
  for (i = 0; i < tiles->getLength(); ++i) {
    tile = (PlacedTileDesc *)tiles->get(i);
    if (tile->px < x + w &&
	tile->px + tile->tw > x &&
	tile->py < y + h &&
	tile->py + tile->th > y) {
      if (tile->page < *firstPage) {
	*firstPage = tile->page;
      }
      if (tile->page > *lastPage) {
	*lastPage = tile->page;
      }
    }
  }
}

int TileMap::getPageTopY(int page) {
  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    return 0;
  }

  updateContinuousModeParams();

  switch (state->getDisplayMode()) {
  case displaySingle:
  case displaySideBySideSingle:
  case displayHorizontalContinuous:
  default:
    return 0;
  case displayContinuous:
  case displaySideBySideContinuous:
    return pageY[page - 1];
  }
}

int TileMap::getPageBottomY(int page) {
  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    return 0;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {
  case displaySingle:
  case displaySideBySideSingle:
  case displayHorizontalContinuous:
  default:
    return pageH[page - 1] - state->getWinH();
  case displayContinuous:
  case displaySideBySideContinuous:
    return pageY[page - 1] + pageH[page - 1] - state->getWinH();
  }
}

int TileMap::getPageLeftX(int page) {
  int leftPage, rightPage, pageW1, pageW2, offsetX, offsetX2;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    return 0;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    return 0;
  case displayContinuous:
    return (maxW - pageW[page - 1]) / 2;
  case displaySideBySideSingle:
    leftPage = ((page - 1) & ~1) + 1;
    rightPage = leftPage + 1;
    pageW1 = pageW[leftPage - 1];
    if (rightPage <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[rightPage - 1];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageW2 = pageW1;
    }
    if (pageW1 + sideBySidePageSpacing + pageW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (pageW1 + sideBySidePageSpacing + pageW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    return (page == leftPage) ? offsetX : offsetX2;
  case displaySideBySideContinuous:
    leftPage = ((page - 1) & ~1) + 1;
    rightPage = leftPage + 1;
    pageW1 = pageW[leftPage - 1];
    if (maxW + sideBySidePageSpacing + maxW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (maxW + sideBySidePageSpacing + maxW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += maxW - pageW1;
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    return (page == leftPage) ? offsetX : offsetX2;
  case displayHorizontalContinuous:
    return pageX[page - 1];
  }
}

int TileMap::getPageRightX(int page) {
  int leftPage, rightPage, pageW1, pageW2, offsetX, offsetX2;

  if (!state->getDoc() || !state->getDoc()->getNumPages()) {
    return 0;
  }

  updatePageParams();
  updateContinuousModeParams();

  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    return pageW[page - 1] - state->getWinW();
  case displayContinuous:
    return (maxW + pageW[page - 1]) / 2 - state->getWinW();
  case displaySideBySideSingle:
    leftPage = ((page - 1) & ~1) + 1;
    rightPage = leftPage + 1;
    pageW1 = pageW[leftPage - 1];
    if (rightPage <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[rightPage - 1];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageW2 = pageW1;
    }
    if (pageW1 + sideBySidePageSpacing + pageW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (pageW1 + sideBySidePageSpacing + pageW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    return (page == leftPage) ? offsetX + pageW1 - state->getWinW()
                              : offsetX2 + pageW2 - state->getWinW();
  case displaySideBySideContinuous:
    leftPage = ((page - 1) & ~1) + 1;
    rightPage = leftPage + 1;
    pageW1 = pageW[leftPage - 1];
    if (rightPage <= state->getDoc()->getNumPages()) {
      pageW2 = pageW[rightPage - 1];
    } else {
      // display a single page as though there were a blank facing
      // page of the same size
      pageW2 = pageW1;
    }
    if (maxW + sideBySidePageSpacing + maxW2 < state->getWinW()) {
      offsetX = (state->getWinW() -
		 (maxW + sideBySidePageSpacing + maxW2)) / 2;
    } else {
      offsetX = 0;
    }
    offsetX += maxW - pageW1;
    offsetX2 = offsetX + pageW1 + sideBySidePageSpacing;
    return (page == leftPage) ? offsetX + pageW1 - state->getWinW()
                              : offsetX2 + pageW2 - state->getWinW();
  case displayHorizontalContinuous:
    return pageX[page - 1] + pageW[page - 1] - state->getWinW();
  }
}

int TileMap::getFirstPage() {
  updateContinuousModeParams();
  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    return state->getScrollPage();
  case displayContinuous:
    return findContinuousPage(state->getScrollY());
  case displaySideBySideSingle:
    return state->getScrollPage();
  case displaySideBySideContinuous:
    return findSideBySideContinuousPage(state->getScrollY());
  case displayHorizontalContinuous:
    return findHorizContinuousPage(state->getScrollX());
  }
}

int TileMap::getFirstPageTop() {
  int page;

  updateContinuousModeParams();
  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    return state->getScrollPage();
  case displayContinuous:
    page = findContinuousPage(state->getScrollY());
    if (page < state->getDoc()->getNumPages() &&
	pageY[page - 1] < state->getScrollY()) {
      return page + 1;
    } else {
      return page;
    }
  case displaySideBySideSingle:
    return state->getScrollPage();
  case displaySideBySideContinuous:
    page = findSideBySideContinuousPage(state->getScrollY());
    if (page < state->getDoc()->getNumPages() &&
	pageY[page - 1] < state->getScrollY()) {
      return page + 1;
    } else {
      return page;
    }
  case displayHorizontalContinuous:
    page = findHorizContinuousPage(state->getScrollX());
    if (page < state->getDoc()->getNumPages() &&
	pageX[page - 1] < state->getScrollX()) {
      return page + 1;
    } else {
      return page;
    }
  }
}

int TileMap::getMidPage() {
  int wx, wy, pg, x, y;

  wx = state->getWinW() / 2;
  wy = state->getWinH() / 2;
  if (!cvtWindowToDev(wx, wy, &pg, &x, &y)) {
    if (state->getDisplayMode() == displayContinuous) {
      wy += continuousPageSpacing;
    } else if (state->getDisplayMode() == displaySideBySideContinuous) {
      wx += sideBySidePageSpacing;
      wy += continuousPageSpacing;
    } else if (state->getDisplayMode() == displayHorizontalContinuous) {
      wx += horizContinuousPageSpacing;
    } else {
      return state->getScrollPage();
    }
    if (!cvtWindowToDev(wx, wy, &pg, &x, &y)) {
      return 1;
    }
  }
  return pg;
}

int TileMap::getLastPage() {
  int pg, x, y, n;

  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    return state->getScrollPage();
  case displayContinuous:
    if (!cvtWindowToDev(state->getWinW() / 2, state->getWinH() - 1,
			&pg, &x, &y)) {
      return state->getDoc()->getNumPages();
    }
    return pg;
  case displaySideBySideSingle:
    pg = state->getScrollPage() + 1;
    n = state->getDoc()->getNumPages();
    if (pg > n) {
      pg = n;
    }
    return pg;
  case displaySideBySideContinuous:
    if (!cvtWindowToDev(state->getWinW() / 2, state->getWinH() - 1,
			&pg, &x, &y)) {
      return state->getScrollPage();
    }
    pg = ((pg - 1) & ~1) + 2;
    n = state->getDoc()->getNumPages();
    if (pg > n) {
      pg = n;
    }
    return pg;
  case displayHorizontalContinuous:
    x = state->getWinW() - 1;
    y = state->getWinH() / 2;
    if (!cvtWindowToDev(state->getWinW() - 1, state->getWinH() / 2,
			&pg, &x, &y)) {
      return state->getDoc()->getNumPages();
    }
    return pg;
  }
}

double TileMap::getDPI(int page) {
  if (page < 1 || page > state->getDoc()->getNumPages()) {
    return 0;
  }
  updatePageParams();
  return pageDPI[page - 1];
}

double TileMap::getPageBoxWidth(int page) {
  return pageBoxW[page - 1];
}

double TileMap::getPageBoxHeight(int page) {
  return pageBoxH[page - 1];
}

int TileMap::getContinuousPageSpacing() {
  return continuousPageSpacing;
}

int TileMap::getSideBySidePageSpacing() {
  return sideBySidePageSpacing;
}

int TileMap::getHorizContinuousPageSpacing() {
  return horizContinuousPageSpacing;
}

void TileMap::docChanged() {
  PDFDoc *doc;
  int nPages, pg, rot;

  doc = state->getDoc();

  if (doc) {
    nPages = doc->getNumPages();
  } else {
    nPages = 0;
  }
  pageBoxW = (double *)greallocn(pageBoxW, nPages, sizeof(double));
  pageBoxH = (double *)greallocn(pageBoxH, nPages, sizeof(double));
  for (pg = 1; pg <= nPages; ++pg) {
    rot = doc->getPageRotate(pg);
    if (rot == 0 || rot == 180) {
      pageBoxW[pg - 1] = doc->getPageCropWidth(pg);
      pageBoxH[pg - 1] = doc->getPageCropHeight(pg);
    } else {
      pageBoxW[pg - 1] = doc->getPageCropHeight(pg);
      pageBoxH[pg - 1] = doc->getPageCropWidth(pg);
    }
  }

  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::windowSizeChanged() {
  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::displayModeChanged() {
  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::zoomChanged() {
  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::rotateChanged() {
  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::scrollPositionChanged() {
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}


void TileMap::forceRedraw() {
  clearPageParams();
  clearContinuousModeParams();
  if (tiles) {
    deleteGList(tiles, PlacedTileDesc);
    tiles = NULL;
  }
}

void TileMap::clearPageParams() {
  gfree(pageDPI);
  gfree(pageW);
  gfree(pageH);
  gfree(tileW);
  gfree(tileH);
  pageDPI = NULL;
  pageW = pageH = NULL;
  tileW = tileH = NULL;
}

void TileMap::updatePageParams() {
  double rotPageBoxW, rotPageBoxW2, rotPageBoxH, rotPageBoxH2, rotPageBoxHMax;
  double hDPI, vDPI, dpi;
  int page, otherPage, nxTiles, nyTiles;

  //--- check to see if the continuous mode params have already been updated
  if (pageDPI) {
    return;
  }

  //--- allocate memory
  pageDPI = (double *)gmallocn(state->getDoc()->getNumPages(), sizeof(double));
  pageW = (int *)gmallocn(state->getDoc()->getNumPages(), sizeof(int));
  pageH = (int *)gmallocn(state->getDoc()->getNumPages(), sizeof(int));
  tileW = (int *)gmallocn(state->getDoc()->getNumPages(), sizeof(int));
  tileH = (int *)gmallocn(state->getDoc()->getNumPages(), sizeof(int));

  for (page = 1; page <= state->getDoc()->getNumPages(); ++page) {

    //--- special handling for side-by-side modes
    if (state->displayModeIsSideBySide()) {

      // rotate the page boxes
      if (page & 1) {
	otherPage = page + 1;
	if (otherPage >= state->getDoc()->getNumPages()) {
	  otherPage = page;
	}
      } else {
	otherPage = page - 1;
	if (otherPage < 1) {
	  otherPage = page;
	}
      }
      if (state->getRotate() == 0 || state->getRotate() == 180) {
	rotPageBoxW = pageBoxW[page - 1];
	rotPageBoxW2 = pageBoxW[otherPage - 1];
	rotPageBoxH = pageBoxH[page - 1];
	rotPageBoxH2 = pageBoxH[otherPage - 1];
      } else {
	rotPageBoxW = pageBoxH[page - 1];
	rotPageBoxW2 = pageBoxH[otherPage - 1];
	rotPageBoxH = pageBoxW[page - 1];
	rotPageBoxH2 = pageBoxW[otherPage - 1];
      }
      rotPageBoxHMax = (rotPageBoxH > rotPageBoxH2) ? rotPageBoxH
	                                            : rotPageBoxH2;

      // compute resolution
      if (state->getZoom() == zoomPage) {
	hDPI = ((state->getWinW() - sideBySidePageSpacing) /
		(rotPageBoxW + rotPageBoxW2)) * 72.0;
	vDPI = (state->getWinH() / rotPageBoxHMax) * 72.0;
	dpi = hDPI < vDPI ? hDPI : vDPI;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else if (state->getZoom() == zoomWidth) {
	dpi = ((state->getWinW() - sideBySidePageSpacing) /
	       (rotPageBoxW + rotPageBoxW2)) * 72.0;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else if (state->getZoom() == zoomHeight) {
	dpi = (state->getWinH() / rotPageBoxHMax) * 72.0;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else {
	dpi = 0.01 * state->getZoom() * 72.0;
      }

    //--- all other (non-side-by-side) modes
    } else {

      // rotate the page boxes
      if (state->getRotate() == 0 || state->getRotate() == 180) {
	rotPageBoxW = pageBoxW[page - 1];
	rotPageBoxH = pageBoxH[page - 1];
      } else {
	rotPageBoxW = pageBoxH[page - 1];
	rotPageBoxH = pageBoxW[page - 1];
      }

      // compute resolution
      if (state->getZoom() == zoomPage) {
	hDPI = (state->getWinW() / rotPageBoxW) * 72.0;
	vDPI = (state->getWinH() / rotPageBoxH) * 72.0;
	dpi = hDPI < vDPI ? hDPI : vDPI;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else if (state->getZoom() == zoomWidth) {
	dpi = (state->getWinW() / rotPageBoxW) * 72.0;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else if (state->getZoom() == zoomHeight) {
	dpi = (state->getWinH() / rotPageBoxH) * 72.0;
	// allow for some floating point jitter
	dpi -= 0.01;
      } else {
	dpi = 0.01 * state->getZoom() * 72.0;
      }

    }
    pageDPI[page - 1] = dpi;

    // compute bitmap size
    pageW[page - 1] = (int)((rotPageBoxW * dpi / 72.0) + 0.5);
    if (pageW[page - 1] < 1) {
      pageW[page - 1] = 1;
    }
    pageH[page - 1] = (int)((rotPageBoxH * dpi / 72.0) + 0.5);
    if (pageH[page - 1] < 1) {
      pageH[page - 1] = 1;
    }

    // compute tile size
    // (tile width and height are rounded up -- the bottom and right
    // tiles may be slightly smaller than the computed size)
    if (pageW[page - 1] <= state->getMaxTileWidth()) {
      nxTiles = 1;
      tileW[page - 1] = pageW[page - 1];
    } else {
      nxTiles = (pageW[page - 1] + state->getMaxTileWidth() - 1)
	        / state->getMaxTileWidth();
      tileW[page - 1] = (pageW[page - 1] + nxTiles - 1) / nxTiles;
    }
    if (pageH[page - 1] <= state->getMaxTileHeight()) {
      nyTiles = 1;
      tileH[page - 1] = pageH[page - 1];
    } else {
      nyTiles = (pageH[page - 1] + state->getMaxTileHeight() - 1)
	        / state->getMaxTileHeight();
      tileH[page - 1] = (pageH[page - 1] + nyTiles - 1) / nyTiles;
    }

  }
}

void TileMap::clearContinuousModeParams() {
  gfree(pageX);
  pageX = pageY = NULL;
}

void TileMap::updateContinuousModeParams() {
  int page, pageW1, pageH1, pageW2, pageH2, x, y;

  // check to see if the continuous mode params have already been updated
  if (pageX) {
    return;
  }

  updatePageParams();

  switch (state->getDisplayMode()) {
  case displayContinuous:
    if (!pageX) {
      pageX = pageY = (int *)gmallocn(state->getDoc()->getNumPages(),
				      sizeof(int));
    }
    y = 0;
    maxW = 0;
    for (page = 1; page <= state->getDoc()->getNumPages(); ++page) {
      pageY[page - 1] = y;
      y += pageH[page - 1] + continuousPageSpacing;
      if (page == 1 || pageW[page - 1] > maxW) {
	maxW = pageW[page - 1];
      }
    }
    totalH = y - continuousPageSpacing;
    break;
  case displaySideBySideContinuous:
    if (!pageX) {
      pageX = pageY = (int *)gmallocn(state->getDoc()->getNumPages(),
				      sizeof(int));
    }
    y = 0;
    maxW = maxW2 = 0;
    for (page = 1; page <= state->getDoc()->getNumPages(); page += 2) {
      pageW1 = pageW[page - 1];
      pageH1 = pageH[page - 1];
      if (page + 1 <= state->getDoc()->getNumPages()) {
	pageW2 = pageW[page];
	pageH2 = pageH[page];
      } else {
	pageW2 = pageW1;
	pageH2 = pageH1;
      }
      pageY[page - 1] = y;
      if (page == 1 || pageW1 > maxW) {
	maxW = pageW1;
      }
      if (page + 1 <= state->getDoc()->getNumPages()) {
	pageY[page] = y;
      }
      if (pageW2 > maxW2) {
	maxW2 = pageW2;
      }
      y += (pageH1 > pageH2) ? pageH1 : pageH2;
      y += continuousPageSpacing;
    }
    totalH = y - continuousPageSpacing;
    break;
  case displayHorizontalContinuous:
    if (!pageX) {
      pageX = pageY = (int *)gmallocn(state->getDoc()->getNumPages(),
				      sizeof(int));
    }
    x = 0;
    maxH = 0;
    for (page = 1; page <= state->getDoc()->getNumPages(); ++page) {
      pageX[page - 1] = x;
      x += pageW[page - 1] + horizContinuousPageSpacing;
      if (page == 1 || pageH[page - 1] > maxH) {
	maxH = pageH[page - 1];
      }
    }
    totalW = x - horizContinuousPageSpacing;
    break;
  default:
    break;
  }
}

void TileMap::computePageMatrix(int page, double *m) {
  PDFRectangle *cropBox;
  double px1, py1, px2, py2, k;
  int rotate;

  updatePageParams();
  cropBox = state->getDoc()->getCatalog()->getPage(page)->getCropBox();
  px1 = cropBox->x1;
  py1 = cropBox->y1;
  px2 = cropBox->x2;
  py2 = cropBox->y2;
  k = pageDPI[page - 1] / 72.0;
  rotate = state->getRotate() +
           state->getDoc()->getCatalog()->getPage(page)->getRotate();
  if (rotate > 360) {
    rotate -= 360;
  }
  switch (rotate) {
  case 0:
  default:
    m[0] = k;
    m[1] = 0;
    m[2] = 0;
    m[3] = -k;
    m[4] = -k * px1;
    m[5] = k * py2;
    break;
  case 90:
    m[0] = 0;
    m[1] = k;
    m[2] = k;
    m[3] = 0;
    m[4] = -k * py1;
    m[5] = -k * px1;
    break;
  case 180:
    m[0] = -k;
    m[1] = 0;
    m[2] = 0;
    m[3] = k;
    m[4] = k * px2;
    m[5] = -k * py1;
    break;
  case 270:
    m[0] = 0;
    m[1] = -k;
    m[2] = -k;
    m[3] = 0;
    m[4] = k * py2;
    m[5] = k * px2;
    break;
  }
}

void TileMap::invertMatrix(double *m, double *im) {
  double det;

  det = 1 / (m[0] * m[3] - m[1] * m[2]);
  im[0] = m[3] * det;
  im[1] = -m[1] * det;
  im[2] = -m[2] * det;
  im[3] = m[0] * det;
  im[4] = (m[2] * m[5] - m[3] * m[4]) * det;
  im[5] = (m[1] * m[4] - m[0] * m[5]) * det;
}

int TileMap::findContinuousPage(int y) {
  int a, b, m;

  if (y < pageY[0]) {
    return 0;
  }
  if (y >= totalH) {
    return state->getDoc()->getNumPages() + 1;
  }
  a = -1;
  b = state->getDoc()->getNumPages();
  // invariant: pageY[a] < y < pageY[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (y > pageY[m] - continuousPageSpacing) {
      a = m;
    } else if (y < pageY[m] - continuousPageSpacing) {
      b = m;
    } else {
      return m + 1;
    }
  }
  return a + 1;
}

int TileMap::findSideBySideContinuousPage(int y) {
  int a, b, m;

  if (y < pageY[0]) {
    return 0;
  }
  if (y >= totalH) {
    return (state->getDoc()->getNumPages() + 2) & ~1;
  }
  a = -2;
  b = (state->getDoc()->getNumPages() + 1) & ~1;
  // invariant: pageY[a] < y < pageY[b]
  while (b - a > 2) {
    m = ((a + b) / 2) & ~1;
    if (y > pageY[m] - continuousPageSpacing) {
      a = m;
    } else if (y < pageY[m] - continuousPageSpacing) {
      b = m;
    } else {
      return m + 1;
    }
  }
  return a + 1;
}

int TileMap::findHorizContinuousPage(int x) {
  int a, b, m;

  if (x < pageX[0]) {
    return 0;
  }
  if (x >= totalW) {
    return state->getDoc()->getNumPages() + 1;
  }
  a = -1;
  b = state->getDoc()->getNumPages();
  // invariant: pageX[a] < x < pageX[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (x > pageX[m] - horizContinuousPageSpacing) {
      a = m;
    } else if (x < pageX[m] - horizContinuousPageSpacing) {
      b = m;
    } else {
      return m + 1;
    }
  }
  return a + 1;
}
