//========================================================================
//
// SplashXPathScanner.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHXPATHSCANNER_H
#define SPLASHXPATHSCANNER_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "SplashTypes.h"
#include "SplashXPath.h"

class GList;

//------------------------------------------------------------------------

// Set this to 0 for antialiasing with 16 levels of gray.
// Set it to 1 for (much slower) antialiasing with 256 levels of gray.
#define ANTIALIAS_256 0

//------------------------------------------------------------------------
// SplashXPathScanner
//------------------------------------------------------------------------

class SplashXPathScanner {
public:

  // Create a new SplashXPathScanner object.  <xPathA> must be sorted.
  SplashXPathScanner(SplashXPath *xPathA, GBool eo,
		     int yMinA, int yMaxA);

  ~SplashXPathScanner();

  // Compute shape values for a scan line.  Fills in line[] with shape
  // values for one scan line: ([x0, x1], y).  The values are in [0,
  // 255].  Also returns the min/max x positions with non-zero shape
  // values.
  void getSpan(Guchar *line, int y, int x0, int x1, int *xMin, int *xMax);

  // Like getSpan(), but uses the values 0 and 255 only.  Writes 255
  // for all pixels which include non-zero area inside the path.
  void getSpanBinary(Guchar *line, int y, int x0, int x1,
		     int *xMin, int *xMax);

private:

  void insertSegmentBefore(SplashXPathSeg *s, SplashXPathSeg *sNext);
  void removeSegment(SplashXPathSeg *s);
  void moveSegmentAfter(SplashXPathSeg *s, SplashXPathSeg *sPrev);
  void reset(GBool aa, GBool aaChanged);
  void skip(int newYBottomI, GBool aa);
  void advance(GBool aa);
  void generatePixels(int x0, int x1, Guchar *line, int *xMin, int *xMax);
  void generatePixelsBinary(int x0, int x1, Guchar *line,
			    int *xMin, int *xMax);
  void drawRectangleSpan(Guchar *line, int y, int x0, int x1,
			 int *xMin, int *xMax);
  void drawRectangleSpanBinary(Guchar *line, int y, int x0, int x1,
			       int *xMin, int *xMax);

  SplashXPath *xPath;
  int eoMask;
  int yMin, yMax;
  int rectX0I, rectY0I, rectX1I, rectY1I;

  SplashXPathSeg preSeg, postSeg;
  SplashXPathSeg *pre, *post;

  GBool resetDone;
  GBool resetAA;
  int nextSeg;
  int yTopI, yBottomI;
  SplashCoord yTop, yBottom;
};

#endif
