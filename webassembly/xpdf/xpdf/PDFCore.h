//========================================================================
//
// PDFCore.h
//
// Copyright 2004-2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef PDFCORE_H
#define PDFCORE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdlib.h>
#include "SplashTypes.h"
#include "CharTypes.h"
#include "DisplayState.h"
#include "TextOutputDev.h"

class GString;
class GList;
class SplashBitmap;
class SplashPattern;
class BaseStream;
class PDFDoc;
class Links;
class LinkDest;
class LinkAction;
class Annot;
class Annots;
class FormField;
class TextPage;
class HighlightFile;
class OptionalContentGroup;
class TileMap;
class TileCache;
class TileCompositor;
class PDFCore;

//------------------------------------------------------------------------
// PDFHistory
//------------------------------------------------------------------------

struct PDFHistory {
#ifdef _WIN32
  wchar_t *fileName;
#else
  GString *fileName;
#endif
  int page;
};

#define pdfHistorySize 50

//------------------------------------------------------------------------
// SelectMode
//------------------------------------------------------------------------

enum SelectMode {
  selectModeBlock,
  selectModeLinear
};

//------------------------------------------------------------------------
// PDFCore
//------------------------------------------------------------------------

class PDFCore {
public:

  PDFCore(SplashColorMode colorMode, int bitmapRowPad,
	  GBool reverseVideo, SplashColorPtr paperColor);
  virtual ~PDFCore();

  //----- loadFile / displayPage / displayDest

  // Load a new file.  Returns pdfOk or error code.
  virtual int loadFile(GString *fileName, GString *ownerPassword = NULL,
		       GString *userPassword = NULL);

#ifdef _WIN32
  // Load a new file.  Returns pdfOk or error code.
  virtual int loadFile(wchar_t *fileName, int fileNameLen,
		       GString *ownerPassword = NULL,
		       GString *userPassword = NULL);
#endif

  // Load a new file, via a Stream instead of a file name.  Returns
  // pdfOk or error code.
  virtual int loadFile(BaseStream *stream, GString *ownerPassword = NULL,
		       GString *userPassword = NULL);

  // Load an already-created PDFDoc object.
  virtual void loadDoc(PDFDoc *docA);

  // Reload the current file.  This only works if the PDF was loaded
  // via a file.  Returns pdfOk or error code.
  virtual int reload();

  // Clear out the current document, if any.
  virtual void clear();

  // Same as clear(), but returns the PDFDoc object instead of
  // deleting it.
  virtual PDFDoc *takeDoc(GBool redraw);

  // Display (or redisplay) the specified page.  If <scrollToTop> is
  // set, the window is vertically scrolled to the top; if
  // <scrollToBottom> is set, the window is vertically scrolled to the
  // bottom; otherwise, no scrolling is done.  If <addToHist> is set,
  // this page change is added to the history list.
  virtual void displayPage(int page, GBool scrollToTop,
			   GBool scrollToBottom, GBool addToHist = gTrue);

  // Display a link destination.
  virtual void displayDest(LinkDest *dest);

  // Called before any update is started.
  virtual void startUpdate();

  // Called after any update is complete.  Subclasses can check for
  // changes in the display parameters here.
  virtual void finishUpdate(GBool addToHist, GBool checkForChangedFile);

  //----- page/position changes

  virtual GBool gotoNextPage(int inc, GBool top);
  virtual GBool gotoPrevPage(int dec, GBool top, GBool bottom);
  virtual GBool gotoNamedDestination(GString *dest);
  virtual GBool goForward();
  virtual GBool goBackward();
  virtual void scrollLeft(int nCols = 16);
  virtual void scrollRight(int nCols = 16);
  virtual void scrollUp(int nLines = 16, GBool snapToPage = gFalse);
  virtual void scrollUpPrevPage(int nLines = 16);
  virtual void scrollDown(int nLines = 16, GBool snapToPage = gFalse);
  virtual void scrollDownNextPage(int nLines = 16);
  virtual void scrollPageUp();
  virtual void scrollPageDown();
  virtual void scrollTo(int x, int y, GBool snapToPage = gFalse);
  virtual void scrollToLeftEdge();
  virtual void scrollToRightEdge();
  virtual void scrollToTopEdge();
  virtual void scrollToBottomEdge();
  virtual void scrollToTopLeft();
  virtual void scrollToBottomRight();
  // Scroll so that (page, x, y) is centered in the window.
  virtual void scrollToCentered(int page, double x, double y);
  virtual void setZoom(double zoom);
  virtual void zoomToRect(int page, double ulx, double uly,
			  double lrx, double lry);
  virtual void zoomCentered(double zoom);
  virtual void zoomToCurrentWidth();
  virtual void setRotate(int rotate);
  virtual void setDisplayMode(DisplayMode mode);
  virtual void setOCGState(OptionalContentGroup *ocg, GBool ocgState);

  //----- selection

  // Selection mode.
  SelectMode getSelectMode() { return selectMode; }
  void setSelectMode(SelectMode mode);

  // Selection color.
  SplashColorPtr getSelectionColor();
  void setSelectionColor(SplashColor color);

  // Modify the selection.  These functions use device coordinates.
  void setSelection(int page, int x0, int y0, int x1, int y1);
  void setLinearSelection(int page, TextPosition *pos0, TextPosition *pos1);
  void clearSelection();
  void startSelectionDrag(int pg, int x, int y);
  void moveSelectionDrag(int pg, int x, int y);
  void finishSelectionDrag();

  // Retrieve the current selection.  This function uses user
  // coordinates.  Returns false if there is no selection.
  GBool getSelection(int *pg, double *ulx, double *uly,
		     double *lrx, double *lry);
  GBool hasSelection();

  // Text extraction.
  void setTextExtractionMode(TextOutputMode mode);
  GBool getDiscardDiagonalText();
  void setDiscardDiagonalText(GBool discard);
  GString *extractText(int pg, double xMin, double yMin,
		       double xMax, double yMax);
  GString *getSelectedText();

  //----- find

  virtual GBool find(char *s, GBool caseSensitive, GBool next, GBool backward,
		     GBool wholeWord, GBool onePageOnly);
  virtual GBool findU(Unicode *u, int len, GBool caseSensitive,
		      GBool next, GBool backward, GBool wholeWord,
		      GBool onePageOnly);


  //----- coordinate conversion

  // user space: per-page, as defined by PDF file; unit = point
  // device space: (0,0) is upper-left corner of a page; unit = pixel
  // window space: (0,0) is upper-left corner of drawing area; unit = pixel

  GBool cvtWindowToUser(int xw, int yw, int *pg, double *xu, double *yu);
  GBool cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd);
  GBool cvtUserToWindow(int pg, double xy, double yu, int *xw, int *yw);
  void cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd);
  GBool cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw);
  void cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu);
  void getWindowPageRange(int x, int y, int w, int h,
			  int *firstPage, int *lastPage);

  //----- password dialog

  virtual GString *getPassword() { return NULL; }

  //----- misc access

  PDFDoc *getDoc() { return doc; }
  int getPageNum();
  int getMidPageNum();
  double getZoom();
  double getZoomDPI(int page);
  int getRotate();
  DisplayMode getDisplayMode();
  virtual void setPaperColor(SplashColorPtr paperColor);
  virtual void setMatteColor(SplashColorPtr matteColor);
  virtual void setReverseVideo(GBool reverseVideo);
  GBool canGoBack() { return historyBLen > 1; }
  GBool canGoForward() { return historyFLen > 0; }
  int getScrollX();
  int getScrollY();
  int getWindowWidth();
  int getWindowHeight();
  virtual void setBusyCursor(GBool busy) = 0;
  LinkAction *findLink(int pg, double x, double y);
  Annot *findAnnot(int pg, double x, double y);
  int findAnnotIdx(int pg, double x, double y);
  Annot *getAnnot(int idx);
  FormField *findFormField(int pg, double x, double y);
  int findFormFieldIdx(int pg, double x, double y);
  FormField *getFormField(int idx);
  GBool overText(int pg, double x, double y);
  void forceRedraw();
  void setTileDoneCbk(void (*cbk)(void *data), void *data);

protected:

  //--- calls from PDFCore subclass

  // Set the window size (when the window is resized).
  void setWindowSize(int winWidthA, int winHeightA);

  // Get the current window bitmap.  If <wholeWindow> is true, the
  // full window is being redrawn -- this is used to end incremental
  // updates when the rasterization is done.
  SplashBitmap *getWindowBitmap(GBool wholeWindow);

  // Returns true if the last call to getWindowBitmap() returned a
  // finished bitmap; or false if the bitmap was still being
  // rasterized.
  GBool isBitmapFinished() { return bitmapFinished; }

  // This should be called periodically (typically every ~0.1 seconds)
  // to do incremental updates.  If an update is required, it will
  // trigger a call to invalidate().
  virtual void tick();

  //--- callbacks to PDFCore subclass

  // Invalidate the specified rectangle (in window coordinates).
  virtual void invalidate(int x, int y, int w, int h) = 0;

  // Update the scrollbars.
  virtual void updateScrollbars() = 0;

  // This returns true if the PDF file has changed on disk (if it can
  // be checked).
  virtual GBool checkForNewFile() { return gFalse; }

  // This is called just before a PDF file is loaded.
  virtual void preLoad() {}

  // This is called just after a PDF file is loaded.
  virtual void postLoad() {}

  //--- internal

  int loadFile2(PDFDoc *newDoc);
  void addToHistory();
  void clearPage();
  void loadLinks(int pg);
  void loadAnnots(int pg);
  void loadText(int pg);
  void getSelectionBBox(int *wxMin, int *wyMin, int *wxMax, int *wyMax);
  void getSelectRectListBBox(GList *rects, int *wxMin, int *wyMin,
			     int *wxMax, int *wyMax);
  void checkInvalidate(int x, int y, int w, int h);
  void invalidateWholeWindow();

  PDFDoc *doc;

  int linksPage;		// cached links for one page
  Links *links;

  int annotsPage;		// cached annotations for one page
  Annots *annots;

  int textPage;			// cached extracted text for one page
  double textDPI;
  int textRotate;
  TextOutputControl textOutCtrl;
  TextPage *text;

  DisplayState *state;
  TileMap *tileMap;
  TileCache *tileCache;
  TileCompositor *tileCompositor;
  GBool bitmapFinished;

  SelectMode selectMode;
  int selectPage;	 	// page of current selection
  int selectStartX,		// for block mode: start point of current
      selectStartY;		//   selection, in device coords
  TextPosition selectStartPos;	// for linear mode: start position of
				//   current selection

  PDFHistory			// page history queue
    history[pdfHistorySize];
  int historyCur;               // currently displayed page
  int historyBLen;              // number of valid entries backward from
                                //   current entry
  int historyFLen;              // number of valid entries forward from
                                //   current entry
};

#endif
