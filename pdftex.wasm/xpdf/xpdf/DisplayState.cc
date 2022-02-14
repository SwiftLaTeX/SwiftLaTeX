//========================================================================
//
// DisplayState.cc
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "TileMap.h"
#include "TileCache.h"
#include "TileCompositor.h"
#include "DisplayState.h"


//------------------------------------------------------------------------
// DisplayState
//------------------------------------------------------------------------

DisplayState::DisplayState(int maxTileWidthA, int maxTileHeightA,
			   int tileCacheSizeA, int nWorkerThreadsA,
			   SplashColorMode colorModeA, int bitmapRowPadA) {
  int i;

  maxTileWidth = maxTileWidthA;
  maxTileHeight = maxTileHeightA;
  tileCacheSize = tileCacheSizeA;
  nWorkerThreads = nWorkerThreadsA;
  colorMode = colorModeA;
  bitmapRowPad = bitmapRowPadA;

  tileMap = NULL;
  tileCache = NULL;
  tileCompositor = NULL;

  for (i = 0; i < splashColorModeNComps[colorMode]; ++i) {
    paperColor[i] = 0xff;
    matteColor[i] = 0x80;
  }
  if (colorMode == splashModeRGB8 || colorMode == splashModeBGR8) {
    selectColor[0] = 0x80;
    selectColor[1] = 0x80;
    selectColor[2] = 0xff;
  } else {
    for (i = 0; i < splashColorModeNComps[colorMode]; ++i) {
      selectColor[i] = 0xa0;
    }
  }
  reverseVideo = gFalse;

  doc = NULL;

  winW = winH = 100;
  displayMode = displayContinuous;
  zoom = 100;
  rotate = 0;
  scrollPage = 0;
  scrollX = scrollY = 0;

  selectRects = NULL;


}

DisplayState::~DisplayState() {
  if (selectRects) {
    deleteGList(selectRects, SelectRect);
  }
}

void DisplayState::setPaperColor(SplashColorPtr paperColorA) {
  splashColorCopy(paperColor, paperColorA);
  tileCache->paperColorChanged();
  tileCompositor->paperColorChanged();
}

void DisplayState::setMatteColor(SplashColorPtr matteColorA) {
  splashColorCopy(matteColor, matteColorA);
  tileCompositor->matteColorChanged();
}

void DisplayState::setSelectColor(SplashColorPtr selectColorA) {
  splashColorCopy(selectColor, selectColorA);
  tileCompositor->selectColorChanged();
}

void DisplayState::setReverseVideo(GBool reverseVideoA) {
  if (reverseVideo != reverseVideoA) {
    reverseVideo = reverseVideoA;
    tileCache->reverseVideoChanged();
    tileCompositor->reverseVideoChanged();
  }
}

void DisplayState::setDoc(PDFDoc *docA) {
  doc = docA;
  tileMap->docChanged();
  tileCache->docChanged();
  tileCompositor->docChanged();
}

void DisplayState::setWindowSize(int winWA, int winHA) {
  if (winW != winWA || winH != winHA) {
    winW = winWA;
    winH = winHA;
    tileMap->windowSizeChanged();
    tileCompositor->windowSizeChanged();
  }
}

void DisplayState::setDisplayMode(DisplayMode displayModeA) {
  if (displayMode != displayModeA) {
    displayMode = displayModeA;
    tileMap->displayModeChanged();
    tileCompositor->displayModeChanged();
  }
}

void DisplayState::setZoom(double zoomA) {
  if (zoom != zoomA) {
    zoom = zoomA;
    tileMap->zoomChanged();
    tileCompositor->zoomChanged();
  }
}

void DisplayState::setRotate(int rotateA) {
  if (rotate != rotateA) {
    rotate = rotateA;
    tileMap->rotateChanged();
    tileCompositor->rotateChanged();
  }
}

void DisplayState::setScrollPosition(int scrollPageA,
				     int scrollXA, int scrollYA) {
  if (scrollPage != scrollPageA ||
      scrollX != scrollXA ||
      scrollY != scrollYA) {
    scrollPage = scrollPageA;
    scrollX = scrollXA;
    scrollY = scrollYA;
    tileMap->scrollPositionChanged();
    tileCompositor->scrollPositionChanged();
  }
}

void DisplayState::setSelection(int selectPage,
				double selectX0, double selectY0,
				double selectX1, double selectY1) {
  GList *rects;
  SelectRect *rect;

  rect = new SelectRect(selectPage, selectX0, selectY0, selectX1, selectY1);
  rects = new GList();
  rects->append(rect);
  setSelection(rects);
}

void DisplayState::setSelection(GList *selectRectsA) {
  SelectRect *rect, *rectA;
  int i;

  if (!selectRects && !selectRectsA) {
    return;
  }
  if (selectRects && selectRectsA &&
      selectRects->getLength() == selectRectsA->getLength()) {
    for (i = 0; i < selectRects->getLength(); ++i) {
      rect = (SelectRect *)selectRects->get(i);
      rectA = (SelectRect *)selectRectsA->get(i);
      if (*rect != *rectA) {
	break;
      }
    }
    if (i == selectRects->getLength()) {
      deleteGList(selectRectsA, SelectRect);
      return;
    }
  }
  if (selectRects) {
    deleteGList(selectRects, SelectRect);
  }
  selectRects = selectRectsA;
  tileCompositor->selectionChanged();
}

void DisplayState::clearSelection() {
  setSelection(NULL);
}

int DisplayState::getNumSelectRects() {
  if (!selectRects) {
    return 0;
  }
  return selectRects->getLength();
}

SelectRect *DisplayState::getSelectRect(int idx) {
  return (SelectRect *)selectRects->get(idx);
}

void DisplayState::optionalContentChanged() {
  tileCache->optionalContentChanged();
  tileCompositor->optionalContentChanged();
}



void DisplayState::forceRedraw() {
  tileMap->forceRedraw();
  tileCache->forceRedraw();
  tileCompositor->forceRedraw();
}
