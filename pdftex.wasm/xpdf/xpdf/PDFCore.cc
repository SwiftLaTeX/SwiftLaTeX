//========================================================================
//
// PDFCore.cc
//
// Copyright 2004-2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <math.h>
#include "gmempp.h"
#include "GString.h"
#include "GList.h"
#include "GlobalParams.h"
#include "Splash.h"
#include "SplashBitmap.h"
#include "SplashPattern.h"
#include "SplashPath.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "PDFDoc.h"
#include "Link.h"
#include "Annot.h"
#include "Form.h"
#include "OptionalContent.h"
#include "TileMap.h"
#include "TileCache.h"
#include "TileCompositor.h"
#include "PDFCore.h"

//------------------------------------------------------------------------
// PDFCore
//------------------------------------------------------------------------

PDFCore::PDFCore(SplashColorMode colorMode, int bitmapRowPad,
		 GBool reverseVideo, SplashColorPtr paperColor) {
  GString *initialZoom, *initialDisplayMode;
  int z, i;

  doc = NULL;

  linksPage = 0;
  links = NULL;

  annotsPage = 0;
  annots = NULL;

  textPage = 0;
  textDPI = 0;
  textRotate = 0;
  textOutCtrl.mode = textOutPhysLayout;
  text = NULL;

  state = new DisplayState(globalParams->getMaxTileWidth(),
			   globalParams->getMaxTileHeight(),
			   globalParams->getTileCacheSize(),
			   globalParams->getWorkerThreads(),
			   colorMode, bitmapRowPad);
  tileMap = new TileMap(state);
  tileCache = new TileCache(state);
  tileCompositor = new TileCompositor(state, tileMap, tileCache);
  bitmapFinished = gTrue;

  state->setReverseVideo(reverseVideo);
  state->setPaperColor(paperColor);
  initialZoom = globalParams->getInitialZoom();
  if (!initialZoom->cmp("page")) {
    state->setZoom(zoomPage);
  } else if (!initialZoom->cmp("width")) {
    state->setZoom(zoomWidth);
  } else {
    z = atoi(initialZoom->getCString());
    if (z <= 0) {
      z = zoomWidth;
    }
    state->setZoom(z);
  }
  delete initialZoom;
  initialDisplayMode = globalParams->getInitialDisplayMode();
  if (!initialDisplayMode->cmp("single")) {
    state->setDisplayMode(displaySingle);
  } else if (!initialDisplayMode->cmp("sideBySideSingle")) {
    state->setDisplayMode(displaySideBySideSingle);
  } else if (!initialDisplayMode->cmp("sideBySideContinuous")) {
    state->setDisplayMode(displaySideBySideContinuous);
  } else if (!initialDisplayMode->cmp("horizontalContinuous")) {
    state->setDisplayMode(displayHorizontalContinuous);
  } else {
    state->setDisplayMode(displayContinuous);
  }
  delete initialDisplayMode;

  selectMode = selectModeBlock;
  selectPage = 0;
  selectStartX = selectStartY = 0;

  historyCur = pdfHistorySize - 1;
  historyBLen = historyFLen = 0;
  for (i = 0; i < pdfHistorySize; ++i) {
    history[i].fileName = NULL;
    history[i].page = 0;
  }
}

PDFCore::~PDFCore() {
  int i;

  delete tileCompositor;
  delete tileCache;
  delete tileMap;
  delete state;
  clearPage();
  if (doc) {
    delete doc;
  }
  for (i = 0; i < pdfHistorySize; ++i) {
    if (history[i].fileName) {
#ifdef _WIN32
      delete[] history[i].fileName;
#else
      delete history[i].fileName;
#endif
    }
  }
}

int PDFCore::loadFile(GString *fileName, GString *ownerPassword,
		      GString *userPassword) {
  int err;

  setBusyCursor(gTrue);
  err = loadFile2(new PDFDoc(fileName->copy(), ownerPassword, userPassword,
			     this));
  setBusyCursor(gFalse);
  return err;
}

#ifdef _WIN32
int PDFCore::loadFile(wchar_t *fileName, int fileNameLen,
		      GString *ownerPassword, GString *userPassword) {
  int err;

  setBusyCursor(gTrue);
  err = loadFile2(new PDFDoc(fileName, fileNameLen,
			     ownerPassword, userPassword, this));
  setBusyCursor(gFalse);
  return err;
}
#endif

int PDFCore::loadFile(BaseStream *stream, GString *ownerPassword,
		      GString *userPassword) {
  int err;

  setBusyCursor(gTrue);
  err = loadFile2(new PDFDoc(stream, ownerPassword, userPassword, this));
  setBusyCursor(gFalse);
  return err;
}

void PDFCore::loadDoc(PDFDoc *docA) {
  setBusyCursor(gTrue);
  loadFile2(docA);
  setBusyCursor(gFalse);
}

int PDFCore::reload() {
  int err;

  if (!doc->getFileName()) {
    return errOpenFile;
  }
  setBusyCursor(gTrue);
  err = loadFile2(new PDFDoc(doc->getFileName()->copy(), NULL, NULL, this));
  setBusyCursor(gFalse);
  startUpdate();
  finishUpdate(gTrue, gFalse);
  return err;
}

int PDFCore::loadFile2(PDFDoc *newDoc) {
  int err;

  clearSelection();

  // open the PDF file
  if (!newDoc->isOk()) {
    err = newDoc->getErrorCode();
    delete newDoc;
    return err;
  }

  preLoad();

  // replace old document
  // NB: do not delete doc until after DisplayState::setDoc() returns
  state->setDoc(newDoc);
  if (doc) {
    delete doc;
  }
  doc = newDoc;
  clearPage();

  postLoad();

  return errNone;
}

void PDFCore::clear() {
  if (!doc) {
    return;
  }

  // no document
  // NB: do not delete doc until after DisplayState::setDoc() returns
  state->setDoc(NULL);
  delete doc;
  doc = NULL;
  clearPage();

  // redraw
  state->setScrollPosition(1, 0, 0);
  invalidateWholeWindow();
  updateScrollbars();
}

PDFDoc *PDFCore::takeDoc(GBool redraw) {
  PDFDoc *docA;

  if (!doc) {
    return NULL;
  }

  // no document
  // NB: do not delete doc until after DisplayState::setDoc() returns
  state->setDoc(NULL);
  docA = doc;
  doc = NULL;
  clearPage();

  // redraw
  state->setScrollPosition(1, 0, 0);
  if (redraw) {
    invalidateWholeWindow();
    updateScrollbars();
  }

  return docA;
}

void PDFCore::displayPage(int page, GBool scrollToTop,
			  GBool scrollToBottom, GBool addToHist) {
  int scrollX, scrollY;

  if (page <= 0 || page > doc->getNumPages()) {
    return;
  }
  if (scrollToTop ||
      state->getDisplayMode() == displayContinuous ||
      state->getDisplayMode() == displaySideBySideContinuous) {
    scrollY = tileMap->getPageTopY(page);
  } else if (scrollToBottom) {
    scrollY = tileMap->getPageBottomY(page);
  } else {
    scrollY = state->getScrollY();
  }
  if (state->getDisplayMode() == displayHorizontalContinuous) {
    scrollX = tileMap->getPageLeftX(page);
  } else {
    scrollX = state->getScrollX();
  }
  startUpdate();
  state->setScrollPosition(page, scrollX, scrollY);
  finishUpdate(addToHist, gTrue);
}

void PDFCore::displayDest(LinkDest *dest) {
  Ref pageRef;
  int page;
  int dx, dy, scrollX, scrollY;

  if (dest->isPageRef()) {
    pageRef = dest->getPageRef();
    page = doc->findPage(pageRef.num, pageRef.gen);
  } else {
    page = dest->getPageNum();
  }
  if (page <= 0 || page > doc->getNumPages()) {
    page = 1;
  }

  switch (dest->getKind()) {
  case destXYZ:
    cvtUserToDev(page, dest->getLeft(), dest->getTop(), &dx, &dy);
    scrollX = tileMap->getPageLeftX(page);
    if (dest->getChangeLeft()) {
      scrollX += dx;
    }
    scrollY = tileMap->getPageTopY(page);
    if (dest->getChangeTop()) {
      scrollY += dy;
    }
    startUpdate();
    state->setScrollPosition(page, scrollX, scrollY);
    finishUpdate(gTrue, gTrue);
    break;
  case destFit:
  case destFitB:
    state->setZoom(zoomPage);
    scrollX = tileMap->getPageLeftX(page);
    scrollY = tileMap->getPageTopY(page);
    startUpdate();
    state->setScrollPosition(page, scrollX, scrollY);
    finishUpdate(gTrue, gTrue);
    break;
  case destFitH:
  case destFitBH:
    state->setZoom(zoomWidth);
    scrollX = tileMap->getPageLeftX(page);
    scrollY = tileMap->getPageTopY(page);
    if (dest->getChangeTop()) {
      cvtUserToDev(page, 0, dest->getTop(), &dx, &dy);
      scrollY += dy;
    }
    startUpdate();
    state->setScrollPosition(page, scrollX, scrollY);
    finishUpdate(gTrue, gTrue);
    break;
  case destFitV:
  case destFitBV:
    state->setZoom(zoomHeight);
    scrollX = tileMap->getPageLeftX(page);
    scrollY = tileMap->getPageTopY(page);
    if (dest->getChangeTop()) {
      cvtUserToDev(page, dest->getLeft(), 0, &dx, &dy);
      scrollX += dx;
    }
    startUpdate();
    state->setScrollPosition(page, scrollX, scrollY);
    finishUpdate(gTrue, gTrue);
    break;
  case destFitR:
    zoomToRect(page, dest->getLeft(), dest->getTop(),
	       dest->getRight(), dest->getBottom());
    break;
  }
}

void PDFCore::startUpdate() {
}

void PDFCore::finishUpdate(GBool addToHist, GBool checkForChangedFile) {
  int scrollPage, scrollX, scrollY, maxScrollX, maxScrollY;

  if (!doc) {
    invalidateWholeWindow();
    updateScrollbars();
    return;
  }

  // check for changes to the PDF file
  if (checkForChangedFile &&
      doc->getFileName() &&
      checkForNewFile()) {
    loadFile(doc->getFileName());
  }

  // zero-page documents are a special case
  // (check for this *after* checking for changes to the file)
  if (!doc->getNumPages()) {
    invalidateWholeWindow();
    updateScrollbars();
    return;
  }

  // check the scroll position
  scrollPage = state->getScrollPage();
  if (state->getDisplayMode() == displaySideBySideSingle &&
      !(scrollPage & 1)) {
    --scrollPage;
  }
  if (state->displayModeIsContinuous()) {
    scrollPage = 0;
  } else if (scrollPage <= 0 || scrollPage > doc->getNumPages()) {
    scrollPage = 1;
  }
  scrollX = state->getScrollX();
  scrollY = state->getScrollY();
  // we need to set scrollPage before calling getScrollLimits()
  state->setScrollPosition(scrollPage, scrollX, scrollY);
  tileMap->getScrollLimits(&maxScrollX, &maxScrollY);
  maxScrollX -= state->getWinW();
  maxScrollY -= state->getWinH();
  if (scrollX > maxScrollX) {
    scrollX = maxScrollX;
  }
  if (scrollX < 0) {
    scrollX = 0;
  }
  if (scrollY > maxScrollY) {
    scrollY = maxScrollY;
  }
  if (scrollY < 0) {
    scrollY = 0;
  }
  if (scrollPage != state->getScrollPage() ||
      scrollX != state->getScrollX() ||
      scrollY != state->getScrollY()) {
    state->setScrollPosition(scrollPage, scrollX, scrollY);
  }

  // redraw
  invalidateWholeWindow();
  updateScrollbars();

  // add to history
  if (addToHist) {
    addToHistory();
  }
}

void PDFCore::addToHistory() {
  PDFHistory h;
  PDFHistory *cur;

  cur = &history[historyCur];
  h.page = tileMap->getMidPage();
#ifdef _WIN32
  if (doc->getFileNameU()) {
    h.fileName = (wchar_t *)gmallocn(MAX_PATH + 1, sizeof(wchar_t));
    if (GetFullPathNameW(doc->getFileNameU(), MAX_PATH + 1,
			 h.fileName, NULL) == 0) {
      h.fileName = NULL;
    }
  } else {
    h.fileName = NULL;
  }
#else
  if (doc->getFileName()) {
    h.fileName = doc->getFileName()->copy();
  } else {
    h.fileName = NULL;
  }
#endif
  if (historyBLen > 0 && h.page == cur->page) {
    if (!h.fileName && !cur->fileName) {
      return;
    }
#ifdef _WIN32
    if (h.fileName && cur->fileName && !wcscmp(h.fileName, cur->fileName)) {
      gfree(h.fileName);
      return;
    }
#else
    if (h.fileName && cur->fileName && !h.fileName->cmp(cur->fileName)) {
      delete h.fileName;
      return;
    }
#endif
  }
  if (++historyCur == pdfHistorySize) {
    historyCur = 0;
  }
  if (history[historyCur].fileName) {
#ifdef _WIN32
    gfree(history[historyCur].fileName);
#else
    delete history[historyCur].fileName;
#endif
  }
  history[historyCur] = h;
  if (historyBLen < pdfHistorySize) {
    ++historyBLen;
  }
  historyFLen = 0;
}

GBool PDFCore::gotoNextPage(int inc, GBool top) {
  GBool sideBySide;
  int pg;

  if (!doc || !doc->getNumPages()) {
    return gFalse;
  }
  pg = tileMap->getFirstPage();
  sideBySide = state->displayModeIsSideBySide();
  if (pg + (sideBySide ? 2 : 1) > doc->getNumPages()) {
    return gFalse;
  }
  if (sideBySide && inc < 2) {
    inc = 2;
  }
  if ((pg += inc) > doc->getNumPages()) {
    pg = doc->getNumPages();
  }
  displayPage(pg, top, gFalse);
  return gTrue;
}

GBool PDFCore::gotoPrevPage(int dec, GBool top, GBool bottom) {
  int pg;

  if (!doc || !doc->getNumPages()) {
    return gFalse;
  }
  pg = tileMap->getFirstPage();
  if (state->getDisplayMode() == displayContinuous &&
      state->getScrollY() > tileMap->getPageTopY(pg)) {
    ++pg;
  } else if (state->getDisplayMode() == displaySideBySideContinuous &&
	     state->getScrollY() > tileMap->getPageTopY(pg)) {
    pg += 2;
  } else if (state->getDisplayMode() == displayHorizontalContinuous &&
	     state->getScrollX() > tileMap->getPageLeftX(pg)) {
    ++pg;
  }
  if (pg <= 1) {
    return gFalse;
  }
  if (state->displayModeIsSideBySide() && dec < 2) {
    dec = 2;
  }
  if ((pg -= dec) < 1) {
    pg = 1;
  }
  displayPage(pg, top, bottom);
  return gTrue;
}

GBool PDFCore::gotoNamedDestination(GString *dest) {
  LinkDest *d;

  if (!doc) {
    return gFalse;
  }
  if (!(d = doc->findDest(dest))) {
    return gFalse;
  }
  displayDest(d);
  delete d;
  return gTrue;
}

GBool PDFCore::goForward() {
  int pg;

  if (historyFLen == 0) {
    return gFalse;
  }
  if (++historyCur == pdfHistorySize) {
    historyCur = 0;
  }
  --historyFLen;
  ++historyBLen;
  if (!history[historyCur].fileName) {
    return gFalse;
  }
#ifdef _WIN32
  if (!doc ||
      !doc->getFileNameU() ||
      wcscmp(history[historyCur].fileName, doc->getFileNameU()) != 0) {
    if (loadFile(history[historyCur].fileName,
		 (int)wcslen(history[historyCur].fileName)) != errNone) {
      return gFalse;
    }
  }
#else
  if (!doc ||
      !doc->getFileName() ||
      history[historyCur].fileName->cmp(doc->getFileName()) != 0) {
    if (loadFile(history[historyCur].fileName) != errNone) {
      return gFalse;
    }
  }
#endif
  pg = history[historyCur].page;
  displayPage(pg, gFalse, gFalse, gFalse);
  return gTrue;
}

GBool PDFCore::goBackward() {
  int pg;

  if (historyBLen <= 1) {
    return gFalse;
  }
  if (--historyCur < 0) {
    historyCur = pdfHistorySize - 1;
  }
  --historyBLen;
  ++historyFLen;
  if (!history[historyCur].fileName) {
    return gFalse;
  }
#ifdef _WIN32
  if (!doc ||
      !doc->getFileNameU() ||
      wcscmp(history[historyCur].fileName, doc->getFileNameU()) != 0) {
    if (loadFile(history[historyCur].fileName,
		 (int)wcslen(history[historyCur].fileName)) != errNone) {
      return gFalse;
    }
  }
#else
  if (!doc ||
      !doc->getFileName() ||
      history[historyCur].fileName->cmp(doc->getFileName()) != 0) {
    if (loadFile(history[historyCur].fileName) != errNone) {
      return gFalse;
    }
  }
#endif
  pg = history[historyCur].page;
  displayPage(pg, gFalse, gFalse, gFalse);
  return gTrue;
}

void PDFCore::scrollLeft(int nCols) {
  scrollTo(state->getScrollX() - nCols, state->getScrollY());
}

void PDFCore::scrollRight(int nCols) {
  scrollTo(state->getScrollX() + nCols, state->getScrollY());
}

void PDFCore::scrollUp(int nLines, GBool snapToPage) {
  scrollTo(state->getScrollX(), state->getScrollY() - nLines, snapToPage);
}

void PDFCore::scrollUpPrevPage(int nLines) {
  if (!state->displayModeIsContinuous() &&
      state->getScrollY() == 0) {
    gotoPrevPage(1, gFalse, gTrue);
  } else {
    scrollUp(nLines, gTrue);
  }
}

void PDFCore::scrollDown(int nLines, GBool snapToPage) {
  scrollTo(state->getScrollX(), state->getScrollY() + nLines, snapToPage);
}

void PDFCore::scrollDownNextPage(int nLines) {
  int horizMax, vertMax;

  if (!state->displayModeIsContinuous()) {
    tileMap->getScrollLimits(&horizMax, &vertMax);
    if (state->getScrollY() >= vertMax - state->getWinH()) {
      gotoNextPage(1, gTrue);
    } else {
      scrollDown(nLines);
    }
  } else {
    scrollDown(nLines, gTrue);
  }
}

void PDFCore::scrollPageUp() {
  scrollUpPrevPage(state->getWinH());
}

void PDFCore::scrollPageDown() {
  scrollDownNextPage(state->getWinH());
}

void PDFCore::scrollTo(int x, int y, GBool snapToPage) {
  int next, topPage, topPageY, sy, dy;

  startUpdate();
  state->setScrollPosition(state->getScrollPage(), x, y);

  if (snapToPage) {
    if (state->getDisplayMode() == displayContinuous ||
	state->getDisplayMode() == displaySideBySideContinuous) {
      next = state->getDisplayMode() == displaySideBySideContinuous ? 2 : 1;
      topPage = tileMap->getFirstPage();
      topPageY = tileMap->getPageTopY(topPage);
      sy = state->getScrollY();
      dy = sy - topPageY;
      // note: dy can be negative here if the inter-page gap is at the
      // top of the window
      if (-16 < dy && dy < 16) {
	state->setScrollPosition(state->getScrollPage(), x, topPageY);
      } else if (topPage + next <= doc->getNumPages()) {
	topPage += next;
	topPageY = tileMap->getPageTopY(topPage);
	dy = sy - topPageY;
	if (-16 < dy && dy < 0) {
	  state->setScrollPosition(state->getScrollPage(), x, topPageY);
	}
      }
    }
  }

  finishUpdate(gTrue, gTrue);
}

void PDFCore::scrollToLeftEdge() {
  scrollTo(0, state->getScrollY());
}

void PDFCore::scrollToRightEdge() {
  int horizMax, vertMax;

  tileMap->getScrollLimits(&horizMax, &vertMax);
  scrollTo(horizMax - state->getWinW(), state->getScrollY());
}

void PDFCore::scrollToTopEdge() {
  scrollTo(state->getScrollX(),
	   tileMap->getPageTopY(tileMap->getFirstPage()));
}

void PDFCore::scrollToBottomEdge() {
  scrollTo(state->getScrollX(),
	   tileMap->getPageBottomY(tileMap->getLastPage()));
}

void PDFCore::scrollToTopLeft() {
  scrollTo(tileMap->getPageLeftX(tileMap->getFirstPage()),
	   tileMap->getPageTopY(tileMap->getFirstPage()));
}

void PDFCore::scrollToBottomRight() {
  scrollTo(tileMap->getPageRightX(tileMap->getLastPage()),
	   tileMap->getPageBottomY(tileMap->getLastPage()));
}

void PDFCore::scrollToCentered(int page, double x, double y) {
  int wx, wy, sx, sy;

  startUpdate();

  // scroll to the requested page
  state->setScrollPosition(page, tileMap->getPageLeftX(page),
			   tileMap->getPageTopY(page));

  // scroll the requested point to the center of the window
  cvtUserToWindow(page, x, y, &wx, &wy);
  sx = state->getScrollX() + wx - state->getWinW() / 2;
  sy = state->getScrollY() + wy - state->getWinH() / 2;
  state->setScrollPosition(page, sx, sy);

  finishUpdate(gTrue, gFalse);
}

void PDFCore::setZoom(double zoom) {
  int page;

  if (state->getZoom() == zoom) {
    return;
  }
  if (!doc || !doc->getNumPages()) {
    state->setZoom(zoom);
    return;
  }
  startUpdate();
  page = tileMap->getFirstPage();
  state->setZoom(zoom);
  state->setScrollPosition(page, tileMap->getPageLeftX(page),
			   tileMap->getPageTopY(page));
  finishUpdate(gTrue, gTrue);
}

void PDFCore::zoomToRect(int page, double ulx, double uly,
			 double lrx, double lry) {
  int x0, y0, x1, y1, sx, sy, t;
  double dpi, rx, ry, zoom;

  startUpdate();

  // set the new zoom level
  cvtUserToDev(page, ulx, uly, &x0, &y0);
  cvtUserToDev(page, lrx, lry, &x1, &y1);
  if (x0 > x1) {
    t = x0;  x0 = x1;  x1 = t;
  }
  if (y0 > y1) {
    t = y0;  y0 = y1;  y1 = t;
  }
  rx = (double)state->getWinW() / (double)(x1 - x0);
  ry = (double)state->getWinH() / (double)(y1 - y0);
  dpi = tileMap->getDPI(page);
  if (rx < ry) {
    zoom = rx * (dpi / (0.01 * 72));
  } else {
    zoom = ry * (dpi / (0.01 * 72));
  }
  state->setZoom(zoom);

  // scroll to the requested page
  state->setScrollPosition(page, tileMap->getPageLeftX(page),
			   tileMap->getPageTopY(page));

  // scroll the requested rectangle to the center of the window
  cvtUserToWindow(page, 0.5 * (ulx + lrx), 0.5 * (uly + lry), &x0, &y0);
  sx = state->getScrollX() + x0 - state->getWinW() / 2;
  sy = state->getScrollY() + y0 - state->getWinH() / 2;
  state->setScrollPosition(page, sx, sy);

  finishUpdate(gTrue, gFalse);
}

void PDFCore::zoomCentered(double zoom) {
  int page, wx, wy, sx, sy;
  double cx, cy;

  if (state->getZoom() == zoom) {
    return;
  }

  startUpdate();

  // get the center of the window, in user coords
  cvtWindowToUser(state->getWinW() / 2, state->getWinH() / 2,
		  &page, &cx, &cy);

  // set the new zoom level
  state->setZoom(zoom);

  // scroll to re-center
  cvtUserToWindow(page, cx, cy, &wx, &wy);
  sx = state->getScrollX() + wx - state->getWinW() / 2;
  sy = state->getScrollY() + wy - state->getWinH() / 2;
  state->setScrollPosition(page, sx, sy);

  finishUpdate(gTrue, gFalse);
}

// Zoom so that the current page(s) fill the window width.  Maintain
// the vertical center.
void PDFCore::zoomToCurrentWidth() {
  int page0, page1, page, gap;
  double w, w1, zoom;

  startUpdate();

  // get first and last pages
  page0 = tileMap->getFirstPage();
  page1 = tileMap->getLastPage();

  // compute the desired width (in points)
  gap = 0;
  switch (state->getDisplayMode()) {
  case displaySingle:
  default:
    w = tileMap->getPageBoxWidth(page0);
    break;
  case displayContinuous:
    w = 0;
    for (page = page0; page <= page1; ++page) {
      w1 = tileMap->getPageBoxWidth(page);
      if (w1 > w) {
	w = w1;
      }
    }
    break;
  case displaySideBySideSingle:
    w = tileMap->getPageBoxWidth(page0);
    if (page1 != page0) {
      w += tileMap->getPageBoxWidth(page1);
      gap = tileMap->getSideBySidePageSpacing();
    }
    break;
  case displaySideBySideContinuous:
    w = 0;
    for (page = page0; w <= page1; w += 2) {
      w1 = tileMap->getPageBoxWidth(page);
      if (page + 1 <= doc->getNumPages()) {
	w1 += tileMap->getPageBoxWidth(page + 1);
      }
      if (w1 > w) {
	w = w1;
      }
    }
    gap = tileMap->getSideBySidePageSpacing();
    break;
  case displayHorizontalContinuous:
    w = 0;
    gap = 0;
    for (page = page0; page <= page1; ++page) {
      w += tileMap->getPageBoxWidth(page);
      if (page != page0) {
	gap += tileMap->getHorizContinuousPageSpacing();
      }
    }
    break;
  }

  // set the new zoom level
  zoom = 100.0 * (state->getWinW() - gap) / w;
  state->setZoom(zoom);

  // scroll so that the first page is at the left edge of the window
  state->setScrollPosition(page0, tileMap->getPageLeftX(page0),
			   tileMap->getPageTopY(page0));

  finishUpdate(gTrue, gFalse);
}

void PDFCore::setRotate(int rotate) {
  int page;

  if (state->getRotate() == rotate) {
    return;
  }
  if (!doc || !doc->getNumPages()) {
    state->setRotate(rotate);
    return;
  }
  startUpdate();
  page = tileMap->getFirstPage();
  state->setRotate(rotate);
  state->setScrollPosition(page, tileMap->getPageLeftX(page),
			   tileMap->getPageTopY(page));
  finishUpdate(gTrue, gTrue);
}

void PDFCore::setDisplayMode(DisplayMode mode) {
  int page;

  if (state->getDisplayMode() == mode) {
    return;
  }
  if (!doc || !doc->getNumPages()) {
    state->setDisplayMode(mode);
    return;
  }
  startUpdate();
  page = tileMap->getFirstPage();
  state->setDisplayMode(mode);
  state->setScrollPosition(page, tileMap->getPageLeftX(page),
			   tileMap->getPageTopY(page));
  finishUpdate(gTrue, gTrue);
}

void PDFCore::setOCGState(OptionalContentGroup *ocg, GBool ocgState) {
  if (ocgState != ocg->getState()) {
    ocg->setState(ocgState);
    state->optionalContentChanged();
    invalidateWholeWindow();
  }
}

void PDFCore::setSelectMode(SelectMode mode) {
  if (mode != selectMode) {
    selectMode = mode;
    clearSelection();
  }
}

SplashColorPtr PDFCore::getSelectionColor() {
  return state->getSelectColor();
}

void PDFCore::setSelectionColor(SplashColor color) {
  int wx0, wy0, wx1, wy1;

  state->setSelectColor(color);
  if (state->hasSelection()) {
    getSelectionBBox(&wx0, &wy0, &wx1, &wy1);
    checkInvalidate(wx0, wy0, wx1 - wx0, wy1 - wy0);
  }
}

void PDFCore::setSelection(int page, int x0, int y0, int x1, int y1) {
  SelectRect *rect;
  GBool moveLeft, moveTop, moveRight, moveBottom, needScroll;
  double selectX0, selectY0, selectX1, selectY1;
  int oldWx0, oldWy0, oldWx1, oldWy1, ix0, iy0, ix1, iy1;
  int wx0, wy0, wx1, wy1, sx, sy, t;


  // if selection rectangle is empty, clear the selection
  if (x0 == x1 || y0 == y1) {
    clearSelection();
    return;
  }

  // x0 = left, x1 = right
  // y0 = top, y1 = bottom
  if (x0 > x1) {
    t = x0;  x0 = x1;  x1 = t;
  }
  if (y0 > y1) {
    t = y0;  y0 = y1;  y1 = t;
  }

  // convert new selection coords to user space and window space
  tileMap->cvtDevToUser(page, x0, y0, &selectX0, &selectY0);
  tileMap->cvtDevToUser(page, x1, y1, &selectX1, &selectY1);
  cvtUserToWindow(page, selectX0, selectY0, &wx0, &wy0);
  cvtUserToWindow(page, selectX1, selectY1, &wx1, &wy1);
  if (wx0 > wx1) {
    t = wx0;  wx0 = wx1;  wx1 = t;
  }
  if (wy0 > wy1) {
    t = wy0;  wy0 = wy1;  wy1 = t;
  }

  // convert current selection coords to window space;
  // check which edges moved
  if (state->hasSelection()) {
    rect = state->getSelectRect(0);
    tileMap->cvtUserToWindow(rect->page, rect->x0, rect->y0, &oldWx0, &oldWy0);
    tileMap->cvtUserToWindow(rect->page, rect->x1, rect->y1, &oldWx1, &oldWy1);
    if (oldWx0 > oldWx1) {
      t = oldWx0;  oldWx0 = oldWx1;  oldWx1 = t;
    }
    if (oldWy0 > oldWy1) {
      t = oldWy0;  oldWy0 = oldWy1;  oldWy1 = t;
    }
    moveLeft = wx0 != oldWx0;
    moveTop = wy0 != oldWy0;
    moveRight = wx1 != oldWx1;
    moveBottom = wy1 != oldWy1;
  } else {
    oldWx0 = wx0;
    oldWy0 = wy0;
    oldWx1 = wx1;
    oldWy1 = wy1;
    moveLeft = moveTop = moveRight = moveBottom = gTrue;
  }

  // set the new selection
  state->setSelection(page, selectX0, selectY0, selectX1, selectY1);

  // scroll if necessary
  needScroll = gFalse;
  sx = state->getScrollX();
  sy = state->getScrollY();
  if (moveLeft && wx0 < 0) {
    sx += wx0;
    needScroll = gTrue;
  } else if (moveRight && wx1 >= state->getWinW()) {
    sx += wx1 - state->getWinW();
    needScroll = gTrue;
  } else if (moveLeft && wx0 >= state->getWinW()) {
    sx += wx0 - state->getWinW();
    needScroll = gTrue;
  } else if (moveRight && wx1 < 0) {
    sx += wx1;
    needScroll = gTrue;
  }
  if (moveTop && wy0 < 0) {
    sy += wy0;
    needScroll = gTrue;
  } else if (moveBottom && wy1 >= state->getWinH()) {
    sy += wy1 - state->getWinH();
    needScroll = gTrue;
  } else if (moveTop && wy0 >= state->getWinH()) {
    sy += wy0 - state->getWinH();
    needScroll = gTrue;
  } else if (moveBottom && wy1 < 0) {
    sy += wy1;
    needScroll = gTrue;
  }
  if (needScroll) {
    scrollTo(sx, sy);
  } else {
    ix0 = (wx0 < oldWx0) ? wx0 : oldWx0;
    iy0 = (wy0 < oldWy0) ? wy0 : oldWy0;
    ix1 = (wx1 > oldWx1) ? wx1 : oldWx1;
    iy1 = (wy1 > oldWy1) ? wy1 : oldWy1;
    checkInvalidate(ix0, iy0, ix1 - ix0, iy1 - iy0);
  }
}

void PDFCore::setLinearSelection(int page, TextPosition *pos0,
				 TextPosition *pos1) {
  TextPosition begin, end;
  GList *rects;
  GBool moveLeft, moveTop, moveRight, moveBottom, needScroll;
  double x0, y0, x1, y1, x2, y2, x3, y3;
  double ux0, uy0, ux1, uy1;
  int oldWx0, oldWy0, oldWx1, oldWy1, ix0, iy0, ix1, iy1;
  int wx0, wy0, wx1, wy1;
  int sx, sy, colIdx;


  // if selection rectangle is empty, clear the selection
  if (*pos0 == *pos1) {
    clearSelection();
    return;
  }

  // swap into correct order
  if (*pos0 < *pos1) {
    begin = *pos0;
    end = *pos1;
  } else {
    begin = *pos1;
    end = *pos0;
  }

  // build the list of rectangles
  //~ this doesn't handle RtL, vertical, or rotated text
  loadText(selectPage);
  rects = new GList();
  if (begin.colIdx == end.colIdx &&
      begin.parIdx == end.parIdx &&
      begin.lineIdx == end.lineIdx) {
    // same line
    text->convertPosToPointUpper(&begin, &x0, &y0);
    text->convertPosToPointLower(&end, &x1, &y1);
    cvtDevToUser(selectPage, (int)(x0 + 0.5), (int)(y0 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y1 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
  } else if (begin.colIdx == end.colIdx) {
    // same column
    text->convertPosToPointUpper(&begin, &x0, &y0);
    text->convertPosToPointRightEdge(&begin, &x1, &y1);
    text->convertPosToPointLeftEdge(&end, &x2, &y2);
    text->convertPosToPointLower(&end, &x3, &y3);
    cvtDevToUser(selectPage, (int)(x0 + 0.5), (int)(y0 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y1 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    cvtDevToUser(selectPage, (int)(x2 + 0.5), (int)(y1 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y2 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    cvtDevToUser(selectPage, (int)(x2 + 0.5), (int)(y2 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x3 + 0.5), (int)(y3 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
  } else {
    // different columns
    text->convertPosToPointUpper(&begin, &x0, &y0);
    text->convertPosToPointRightEdge(&begin, &x1, &y1);
    text->getColumnLowerLeft(begin.colIdx, &x2, &y2);
    cvtDevToUser(selectPage, (int)(x0 + 0.5), (int)(y0 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y1 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    cvtDevToUser(selectPage, (int)(x2 + 0.5), (int)(y1 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y2 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    for (colIdx = begin.colIdx + 1; colIdx < end.colIdx; ++colIdx) {
      text->getColumnLowerLeft(colIdx, &x0, &y0);
      text->getColumnUpperRight(colIdx, &x1, &y1);
      cvtDevToUser(selectPage, (int)(x0 + 0.5), (int)(y1 + 0.5), &ux0, &uy0);
      cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y0 + 0.5), &ux1, &uy1);
      rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    }
    text->getColumnUpperRight(end.colIdx, &x0, &y0);
    text->convertPosToPointLeftEdge(&end, &x1, &y1);
    text->convertPosToPointLower(&end, &x2, &y2);
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y0 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x0 + 0.5), (int)(y1 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
    cvtDevToUser(selectPage, (int)(x1 + 0.5), (int)(y1 + 0.5), &ux0, &uy0);
    cvtDevToUser(selectPage, (int)(x2 + 0.5), (int)(y2 + 0.5), &ux1, &uy1);
    rects->append(new SelectRect(selectPage, ux0, uy0, ux1, uy1));
  }

  // get window coord bboxes for old selection and new selection;
  // check which edges moved
  if (state->hasSelection()) {
    getSelectionBBox(&oldWx0, &oldWy0, &oldWx1, &oldWy1);
    getSelectRectListBBox(rects, &wx0, &wy0, &wx1, &wy1);
    moveLeft = wx0 != oldWx0;
    moveTop = wy0 != oldWy0;
    moveRight = wx1 != oldWx1;
    moveBottom = wy1 != oldWy1;
  } else {
    getSelectRectListBBox(rects, &wx0, &wy0, &wx1, &wy1);
    oldWx0 = wx0;
    oldWy0 = wy0;
    oldWx1 = wx1;
    oldWy1 = wy1;
    moveLeft = moveTop = moveRight = moveBottom = gTrue;
  }

  // set the new selection
  state->setSelection(rects);

  // scroll if necessary
  needScroll = gFalse;
  sx = state->getScrollX();
  sy = state->getScrollY();
  if (moveLeft && wx0 < 0) {
    sx += wx0;
    needScroll = gTrue;
  } else if (moveRight && wx1 >= state->getWinW()) {
    sx += wx1 - state->getWinW();
    needScroll = gTrue;
  } else if (moveLeft && wx0 >= state->getWinW()) {
    sx += wx0 - state->getWinW();
    needScroll = gTrue;
  } else if (moveRight && wx1 < 0) {
    sx += wx1;
    needScroll = gTrue;
  }
  if (moveTop && wy0 < 0) {
    sy += wy0;
    needScroll = gTrue;
  } else if (moveBottom && wy1 >= state->getWinH()) {
    sy += wy1 - state->getWinH();
    needScroll = gTrue;
  } else if (moveTop && wy0 >= state->getWinH()) {
    sy += wy0 - state->getWinH();
    needScroll = gTrue;
  } else if (moveBottom && wy1 < 0) {
    sy += wy1;
    needScroll = gTrue;
  }
  if (needScroll) {
    scrollTo(sx, sy);
  } else {
    ix0 = (wx0 < oldWx0) ? wx0 : oldWx0;
    iy0 = (wy0 < oldWy0) ? wy0 : oldWy0;
    ix1 = (wx1 > oldWx1) ? wx1 : oldWx1;
    iy1 = (wy1 > oldWy1) ? wy1 : oldWy1;
    checkInvalidate(ix0, iy0, ix1 - ix0, iy1 - iy0);
  }
}

void PDFCore::clearSelection() {
  int wx0, wy0, wx1, wy1;

  if (state->hasSelection()) {
    getSelectionBBox(&wx0, &wy0, &wx1, &wy1);
    state->clearSelection();
    checkInvalidate(wx0, wy0, wx1 - wx0, wy1 - wy0);
  }
}

void PDFCore::startSelectionDrag(int pg, int x, int y) {
  clearSelection();
  if (selectMode == selectModeBlock) {
    selectPage = pg;
    selectStartX = x;
    selectStartY = y;
  } else { // selectModeLinear
    loadText(pg);
    if (text->findPointInside(x, y, &selectStartPos)) {
      selectPage = pg;
    } else {
      selectPage = 0;
    }
  }
}

void PDFCore::moveSelectionDrag(int pg, int x, int y) {
  TextPosition pos;

  // don't allow selections to span multiple pages
  // -- this also handles the case where a linear selection was started
  //    outside any text column, in which case selectPage = 0
  if (pg != selectPage) {
    return;
  }

  if (selectMode == selectModeBlock) {
    setSelection(pg, selectStartX, selectStartY, x, y);
  } else { // selectModeLinear
    loadText(pg);
    if (text->findPointNear(x, y, &pos)) {
      setLinearSelection(pg, &selectStartPos, &pos);
    }
  }
}

void PDFCore::finishSelectionDrag() {
  selectPage = 0;
  selectStartX = selectStartY = 0;
}

GBool PDFCore::getSelection(int *pg, double *ulx, double *uly,
			    double *lrx, double *lry) {
  SelectRect *rect;
  double xMin, yMin, xMax, yMax;
  int page, i;

  if (!state->hasSelection()) {
    return gFalse;
  }
  page = state->getSelectRect(0)->page;
  xMin = yMin = xMax = yMax = 0;
  for (i = 0; i < state->getNumSelectRects(); ++i) {
    rect = state->getSelectRect(i);
    if (rect->page != page) {
      continue;
    }
    if (i == 0) {
      xMin = xMax = rect->x0;
      yMin = yMax = rect->y0;
    } else {
      if (rect->x0 < xMin) {
	xMin = rect->x0;
      } else if (rect->x0 > xMax) {
	xMax = rect->x0;
      }
      if (rect->y0 < yMin) {
	yMin = rect->y0;
      } else if (rect->y0 > yMax) {
	yMax = rect->y0;
      }
    }
    if (rect->x1 < xMin) {
      xMin = rect->x1;
    } else if (rect->x1 > xMax) {
      xMax = rect->x1;
    }
    if (rect->y1 < yMin) {
      yMin = rect->y1;
    } else if (rect->y1 > yMax) {
      yMax = rect->y1;
    }
  }
  *pg = page;
  *ulx = xMin;
  *uly = yMax;
  *lrx = xMax;
  *lry = yMin;
  return gTrue;
}

GBool PDFCore::hasSelection() {
  return state->hasSelection();
}

void PDFCore::setTextExtractionMode(TextOutputMode mode) {
  if (textOutCtrl.mode != mode) {
    textOutCtrl.mode = mode;
    if (text) {
      delete text;
      text = NULL;
    }
    textPage = 0;
    textDPI = 0;
    textRotate = 0;
  }
}

GBool PDFCore::getDiscardDiagonalText() {
  return textOutCtrl.discardDiagonalText;
}

void PDFCore::setDiscardDiagonalText(GBool discard) {
  if (textOutCtrl.discardDiagonalText != discard) {
    textOutCtrl.discardDiagonalText = discard;
    if (text) {
      delete text;
      text = NULL;
    }
    textPage = 0;
    textDPI = 0;
    textRotate = 0;
  }
}

GString *PDFCore::extractText(int pg, double xMin, double yMin,
			      double xMax, double yMax) {
  int x0, y0, x1, y1, t;

  loadText(pg);
  cvtUserToDev(pg, xMin, yMin, &x0, &y0);
  cvtUserToDev(pg, xMax, yMax, &x1, &y1);
  if (x0 > x1) {
    t = x0; x0 = x1; x1 = t;
  }
  if (y0 > y1) {
    t = y0; y0 = y1; y1 = t;
  }
  return text->getText(x0, y0, x1, y1);
}

GString *PDFCore::getSelectedText() {
  SelectRect *rect;
  GString *ret, *s;
  int x0, y0, x1, y1, t, i;

  if (!state->hasSelection()) {
    return NULL;
  }
  ret = new GString();
  for (i = 0; i < state->getNumSelectRects(); ++i) {
    rect = state->getSelectRect(i);
    loadText(rect->page);
    cvtUserToDev(rect->page, rect->x0, rect->y0, &x0, &y0);
    cvtUserToDev(rect->page, rect->x1, rect->y1, &x1, &y1);
    if (x0 > x1) {
      t = x0; x0 = x1; x1 = t;
    }
    if (y0 > y1) {
      t = y0; y0 = y1; y1 = t;
    }
    s = text->getText(x0, y0, x1, y1, state->getNumSelectRects() > 1);
    ret->append(s);
    delete s;
  }
  return ret;
}

GBool PDFCore::find(char *s, GBool caseSensitive, GBool next, GBool backward,
		    GBool wholeWord, GBool onePageOnly) {
  Unicode *u;
  int len, i;
  GBool ret;

  // convert to Unicode
  len = (int)strlen(s);
  u = (Unicode *)gmallocn(len, sizeof(Unicode));
  for (i = 0; i < len; ++i) {
    u[i] = (Unicode)(s[i] & 0xff);
  }

  ret = findU(u, len, caseSensitive, next, backward, wholeWord, onePageOnly);

  gfree(u);
  return ret;
}

GBool PDFCore::findU(Unicode *u, int len, GBool caseSensitive,
		     GBool next, GBool backward, GBool wholeWord,
		     GBool onePageOnly) {
  TextOutputDev *textOut;
  SelectRect *rect;
  double xMin, yMin, xMax, yMax;
  int topPage, pg, x, y, x2, y2;
  GBool startAtTop, startAtLast, stopAtLast;

  // check for zero-length string
  if (len == 0) {
    return gFalse;
  }

  setBusyCursor(gTrue);

  // search current page starting at previous result, current
  // selection, or top/bottom of page
  startAtTop = startAtLast = gFalse;
  rect = NULL;
  xMin = yMin = xMax = yMax = 0;
  topPage = tileMap->getFirstPage();
  pg = topPage;
  if (next) {
    if (textPage >= 1 && textPage <= doc->getNumPages()) {
      startAtLast = gTrue;
      pg = textPage;
    }
  } else if (state->hasSelection()) {
    rect = state->getSelectRect(0);
    pg = rect->page;
    cvtUserToDev(pg, rect->x0, rect->y0, &x, &y);
    cvtUserToDev(pg, rect->x1, rect->y1, &x2, &y2);
    if (x2 < x) {
      x = x2;
    }
    if (y2 < y) {
      y = y2;
    }
    if (backward) {
      xMin = x - 1;
      yMin = y - 1;
    } else {
      xMin = x + 1;
      yMin = y + 1;
    }
  } else {
    startAtTop = gTrue;
  }
  loadText(pg);
  if (text->findText(u, len, startAtTop, gTrue, startAtLast, gFalse,
		     caseSensitive, backward, wholeWord,
		     &xMin, &yMin, &xMax, &yMax)) {
    goto found;
  }

  if (!onePageOnly) {

    // search following/previous pages
    textOut = new TextOutputDev(NULL, &textOutCtrl, gFalse);
    if (!textOut->isOk()) {
      delete textOut;
      goto notFound;
    }
    for (pg = backward ? pg - 1 : pg + 1;
	 backward ? pg >= 1 : pg <= doc->getNumPages();
	 pg += backward ? -1 : 1) {
      doc->displayPage(textOut, pg, 72, 72, 0, gFalse, gTrue, gFalse);
      if (textOut->findText(u, len, gTrue, gTrue, gFalse, gFalse,
			    caseSensitive, backward, wholeWord,
			    &xMin, &yMin, &xMax, &yMax)) {
	delete textOut;
	goto foundPage;
      }
    }

    // search previous/following pages
    for (pg = backward ? doc->getNumPages() : 1;
	 backward ? pg > topPage : pg < topPage;
	 pg += backward ? -1 : 1) {
      doc->displayPage(textOut, pg, 72, 72, 0, gFalse, gTrue, gFalse);
      if (textOut->findText(u, len, gTrue, gTrue, gFalse, gFalse,
			    caseSensitive, backward, wholeWord,
			    &xMin, &yMin, &xMax, &yMax)) {
	delete textOut;
	goto foundPage;
      }
    }
    delete textOut;

  }

  // search current page ending at previous result, current selection,
  // or bottom/top of page
  if (!startAtTop) {
    xMin = yMin = xMax = yMax = 0;
    if (next) {
      stopAtLast = gTrue;
    } else {
      stopAtLast = gFalse;
      cvtUserToDev(pg, rect->x1, rect->y1, &x, &y);
      xMax = x;
      yMax = y;
    }
    if (text->findText(u, len, gTrue, gFalse, gFalse, stopAtLast,
		       caseSensitive, backward, wholeWord,
		       &xMin, &yMin, &xMax, &yMax)) {
      goto found;
    }
  }

  // not found
 notFound:
  setBusyCursor(gFalse);
  return gFalse;

  // found on a different page
 foundPage:
  displayPage(pg, gTrue, gFalse);
  loadText(pg);
  if (!text->findText(u, len, gTrue, gTrue, gFalse, gFalse,
		      caseSensitive, backward, wholeWord,
		      &xMin, &yMin, &xMax, &yMax)) {
    // this can happen if coalescing is bad
    goto notFound;
  }

  // found: change the selection
 found:
  setSelection(pg, (int)floor(xMin), (int)floor(yMin),
	       (int)ceil(xMax), (int)ceil(yMax));

  setBusyCursor(gFalse);
  return gTrue;
}


GBool PDFCore::cvtWindowToUser(int xw, int yw,
			       int *pg, double *xu, double *yu) {
  return tileMap->cvtWindowToUser(xw, yw, pg, xu, yu);
}

GBool PDFCore::cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd) {
  return tileMap->cvtWindowToDev(xw, yw, pg, xd, yd);
}

GBool PDFCore::cvtUserToWindow(int pg, double xu, double yu, int *xw, int *yw) {
  return tileMap->cvtUserToWindow(pg, xu, yu, xw, yw);
}

void PDFCore::cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd) {
  tileMap->cvtUserToDev(pg, xu, yu, xd, yd);
}

GBool PDFCore::cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw) {
  return tileMap->cvtDevToWindow(pg, xd, yd, xw, yw);
}

void PDFCore::cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu) {
  tileMap->cvtDevToUser(pg, xd, yd, xu, yu);
}

void PDFCore::getWindowPageRange(int x, int y, int w, int h,
				 int *firstPage, int *lastPage) {
  tileMap->getWindowPageRange(x, y, w, h, firstPage, lastPage);
}

int PDFCore::getPageNum() {
  if (!doc || !doc->getNumPages()) {
    return 0;
  }
  return tileMap->getFirstPage();
}

int PDFCore::getMidPageNum() {
  if (!doc || !doc->getNumPages()) {
    return 0;
  }
  return tileMap->getMidPage();
}

double PDFCore::getZoom() {
  return state->getZoom();
}

double PDFCore::getZoomDPI(int page) {
  if (!doc) {
    return 0;
  }
  return tileMap->getDPI(page);
}

int PDFCore::getRotate() {
  return state->getRotate();
}

DisplayMode PDFCore::getDisplayMode() {
  return state->getDisplayMode();
}

int PDFCore::getScrollX() {
  return state->getScrollX();
}

int PDFCore::getScrollY() {
  return state->getScrollY();
}

int PDFCore::getWindowWidth() {
  return state->getWinW();
}

int PDFCore::getWindowHeight() {
  return state->getWinH();
}

void PDFCore::setPaperColor(SplashColorPtr paperColor) {
  state->setPaperColor(paperColor);
  invalidateWholeWindow();
}

void PDFCore::setMatteColor(SplashColorPtr matteColor) {
  state->setMatteColor(matteColor);
  invalidateWholeWindow();
}

void PDFCore::setReverseVideo(GBool reverseVideo) {
  SplashColorPtr oldPaperColor;
  SplashColor newPaperColor;
  int i;

  if (reverseVideo != state->getReverseVideo()) {
    state->setReverseVideo(reverseVideo);
    oldPaperColor = state->getPaperColor();
    for (i = 0; i < splashColorModeNComps[state->getColorMode()]; ++i) {
      newPaperColor[i] = oldPaperColor[i] ^ 0xff;
    }
    state->setPaperColor(newPaperColor);
    invalidateWholeWindow();
  }
}



LinkAction *PDFCore::findLink(int pg, double x, double y) {
  loadLinks(pg);
  return links->find(x, y);
}

Annot *PDFCore::findAnnot(int pg, double x, double y) {
  loadAnnots(pg);
  return annots->find(x, y);
}

int PDFCore::findAnnotIdx(int pg, double x, double y) {
  loadAnnots(pg);
  return annots->findIdx(x, y);
}

Annot *PDFCore::getAnnot(int idx) {
  if (!annots) {
    return NULL;
  }
  if (idx < 0 || idx >= annots->getNumAnnots()) {
    return NULL;
  }
  return annots->getAnnot(idx);
}

FormField *PDFCore::findFormField(int pg, double x, double y) {
  if (!doc->getCatalog()->getForm()) {
    return NULL;
  }
  return doc->getCatalog()->getForm()->findField(pg, x, y);
}

int PDFCore::findFormFieldIdx(int pg, double x, double y) {
  if (!doc->getCatalog()->getForm()) {
    return -1;
  }
  return doc->getCatalog()->getForm()->findFieldIdx(pg, x, y);
}

FormField *PDFCore::getFormField(int idx) {
  if (!doc->getCatalog()->getForm()) {
    return NULL;
  }
  if (idx < 0 || idx >= doc->getCatalog()->getForm()->getNumFields()) {
    return NULL;
  }
  return doc->getCatalog()->getForm()->getField(idx);
}

GBool PDFCore::overText(int pg, double x, double y) {
  loadText(pg);
  return text->checkPointInside(x, y);
}

void PDFCore::forceRedraw() {
  startUpdate();
  state->forceRedraw();
  finishUpdate(gFalse, gFalse);
}

void PDFCore::setTileDoneCbk(void (*cbk)(void *data), void *data) {
  tileCache->setTileDoneCbk(cbk, data);
}

void PDFCore::setWindowSize(int winWidth, int winHeight) {
  GBool doScroll;
  int page, wx0, wy0, wx, wy, sx, sy;
  double ux, uy;

  startUpdate();

  wx0 = wy0 = 0; // make gcc happy
  doScroll = gFalse;
  if (state->getZoom() < 0 && state->displayModeIsContinuous()) {
    // save the user coordinates of the appropriate edge of the window
    if (state->getDisplayMode() == displayHorizontalContinuous) {
      wx0 = 0;
      wy0 = state->getWinH() / 2;
    } else {
      wx0 = state->getWinW() / 2;
      wy0 = 0;
    }
    if (!(doScroll = cvtWindowToUser(wx0, wy0, &page, &ux, &uy))) {
      // tweak the save position if it happens to fall in a gutter
      if (state->getDisplayMode() == displayContinuous) {
	wy0 += tileMap->getContinuousPageSpacing();
      } else if (state->getDisplayMode() == displaySideBySideContinuous) {
	wx0 += tileMap->getSideBySidePageSpacing();
	wy0 += tileMap->getContinuousPageSpacing();
      } else { // state->getDisplayMode() == displayHorizontalContinuous
	wx0 += tileMap->getHorizContinuousPageSpacing();
      }
      doScroll = cvtWindowToUser(wx0, wy0, &page, &ux, &uy);
    }
  }

  state->setWindowSize(winWidth, winHeight);

  if (doScroll) {
    // restore the saved scroll position
    cvtUserToWindow(page, ux, uy, &wx, &wy);
    sx = state->getScrollX();
    sy = state->getScrollY();
    if (state->getDisplayMode() == displayHorizontalContinuous) {
      sx += wx - wx0;
    } else {
      sy += wy - wy0;
    }
    state->setScrollPosition(page, sx, sy);
  }

  finishUpdate(gTrue, gFalse);
}

SplashBitmap *PDFCore::getWindowBitmap(GBool wholeWindow) {
  GBool dummy;

  return tileCompositor->getBitmap(wholeWindow ? &bitmapFinished : &dummy);
}

void PDFCore::tick() {
  if (!bitmapFinished) {
    invalidateWholeWindow();
  }
}

// Clear cached info (links, text) that's tied to a PDFDoc.
void PDFCore::clearPage() {
  if (links) {
    delete links;
  }
  links = NULL;
  linksPage = 0;

  if (annots) {
    delete annots;
  }
  annots = NULL;
  annotsPage = 0;

  if (text) {
    delete text;
  }
  text = NULL;
  textPage = 0;
  textDPI = 0;
  textRotate = 0;
}

// Load the links for <pg>.
void PDFCore::loadLinks(int pg) {
  if (links && linksPage == pg) {
    return;
  }
  if (links) {
    delete links;
  }
  links = doc->getLinks(pg);
  linksPage = pg;
}

// Load the annotations for <pg>.
void PDFCore::loadAnnots(int pg) {
  Object annotsObj;

  if (annots && annotsPage == pg) {
    return;
  }
  if (annots) {
    delete annots;
  }
  doc->getCatalog()->getPage(pg)->getAnnots(&annotsObj);
  annots = new Annots(doc, &annotsObj);
  annotsObj.free();
  annotsPage = pg;
}

// Extract text from <pg>.
void PDFCore::loadText(int pg) {
  TextOutputDev *textOut;
  double dpi;
  int rotate;

  dpi = tileMap->getDPI(pg);
  rotate = state->getRotate();
  if (text && textPage == pg && textDPI == dpi && textRotate == rotate) {
    return;
  }
  if (text) {
    delete text;
  }
  textOut = new TextOutputDev(NULL, &textOutCtrl, gFalse);
  if (!textOut->isOk()) {
    text = new TextPage(&textOutCtrl);
  } else {
    doc->displayPage(textOut, pg, dpi, dpi, rotate, gFalse, gTrue, gFalse);
    text = textOut->takeText();
  }
  delete textOut;
  textPage = pg;
  textDPI = dpi;
  textRotate = rotate;
}

void PDFCore::getSelectionBBox(int *wxMin, int *wyMin, int *wxMax, int *wyMax) {
  *wxMin = *wyMin = *wxMax = *wyMax = 0;
  if (!state->hasSelection()) {
    return;
  }
  getSelectRectListBBox(state->getSelectRects(), wxMin, wyMin, wxMax, wyMax);
}

void PDFCore::getSelectRectListBBox(GList *rects, int *wxMin, int *wyMin,
				    int *wxMax, int *wyMax) {
  SelectRect *rect;
  int x, y, i;

  *wxMin = *wyMin = *wxMax = *wyMax = 0;
  for (i = 0; i < rects->getLength(); ++i) {
    rect = (SelectRect *)rects->get(i);
    tileMap->cvtUserToWindow(rect->page, rect->x0, rect->y0, &x, &y);
    if (i == 0) {
      *wxMin = *wxMax = x;
      *wyMin = *wyMax = y;
    } else {
      if (x < *wxMin) {
	*wxMin = x;
      } else if (x > *wxMax) {
	*wxMax = x;
      }
      if (y < *wyMin) {
	*wyMin = y;
      } else if (y > *wyMax) {
	*wyMax = y;
      }
    }
    tileMap->cvtUserToWindow(rect->page, rect->x1, rect->y1, &x, &y);
    if (x < *wxMin) {
      *wxMin = x;
    } else if (x > *wxMax) {
      *wxMax = x;
    }
    if (y < *wyMin) {
      *wyMin = y;
    } else if (y > *wyMax) {
      *wyMax = y;
    }
  }
}

void PDFCore::checkInvalidate(int x, int y, int w, int h) {
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (x + w > state->getWinW()) {
    w = state->getWinW() - x;
  }
  if (w <= 0) {
    return;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > state->getWinH()) {
    h = state->getWinH() - y;
  }
  if (h <= 0) {
    return;
  }
  invalidate(x, y, w, h);
}

void PDFCore::invalidateWholeWindow() {
  invalidate(0, 0, state->getWinW(), state->getWinH());
}
