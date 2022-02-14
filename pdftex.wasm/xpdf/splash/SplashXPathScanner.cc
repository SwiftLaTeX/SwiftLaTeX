//========================================================================
//
// SplashXPathScanner.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <string.h>
#if HAVE_STD_SORT
#include <algorithm>
#endif
#include "gmem.h"
#include "gmempp.h"
#include "GList.h"
#include "SplashMath.h"
#include "SplashXPath.h"
#include "SplashXPathScanner.h"

//------------------------------------------------------------------------

#if ANTIALIAS_256

#define aaVert  15
#define aaHoriz 17

#else

#define aaVert  4
#define aaHoriz 4

static Guchar map16to255[17] = {
  0,
  16,
  32,
  48,
  64,
  80,
  96,
  112,
  128,
  143,
  159,
  175,
  191,
  207,
  223,
  239,
  255
};

#endif

//------------------------------------------------------------------------

SplashXPathScanner::SplashXPathScanner(SplashXPath *xPathA, GBool eo,
				       int yMinA, int yMaxA) {
  xPath = xPathA;
  eoMask = eo ? 1 : 0xffffffff;
  yMin = yMinA;
  yMax = yMaxA;
  if (xPath->isRect) {
    rectX0I = splashFloor(xPath->rectX0);
    rectY0I = splashFloor(xPath->rectY0);
    rectX1I = splashFloor(xPath->rectX1);
    rectY1I = splashFloor(xPath->rectY1);
  }

  pre = &preSeg;
  post = &postSeg;
  pre->mx = xPath->xMin - 1;
  post->mx = xPath->xMax + 1;

  resetDone = gFalse;
  resetAA = gFalse;
}

SplashXPathScanner::~SplashXPathScanner() {
}

void SplashXPathScanner::insertSegmentBefore(SplashXPathSeg *s,
					     SplashXPathSeg *sNext) {
  SplashXPathSeg *sPrev;

  sPrev = sNext->prev;
  sPrev->next = s;
  s->prev = sPrev;
  s->next = sNext;
  sNext->prev = s;
}

void SplashXPathScanner::removeSegment(SplashXPathSeg *s) {
  SplashXPathSeg *sPrev, *sNext;

  sPrev = s->prev;
  sNext = s->next;
  sPrev->next = sNext;
  sNext->prev = sPrev;
  s->prev = s->next = NULL;
}

void SplashXPathScanner::moveSegmentAfter(SplashXPathSeg *s,
					  SplashXPathSeg *sPrev) {
  SplashXPathSeg *sNext;

  s->prev->next = s->next;
  s->next->prev = s->prev;

  sNext = sPrev->next;
  sPrev->next = s;
  s->prev = sPrev;
  s->next = sNext;
  sNext->prev = s;
}

void SplashXPathScanner::reset(GBool aa, GBool aaChanged) {
  SplashXPathSeg *seg;
  SplashCoord y;
  int i;

  //--- initialize segment parameters
  for (i = 0; i < xPath->length; ++i) {
    seg = &xPath->segs[i];
    if (aa) {
      if (aaChanged) {
	seg->iy = splashFloor(seg->y0 * aaVert);
      }
      y = (SplashCoord)(seg->iy + 1) / (SplashCoord)aaVert;
    } else {
      if (aaChanged) {
	seg->iy = splashFloor(seg->y0);
      }
      y = (SplashCoord)(seg->iy + 1);
    }
    seg->sx0 = seg->x0;
    if (seg->y1 <= y) {
      seg->sx1 = seg->x1;
    } else {
      seg->sx1 = seg->x0 + (y - seg->y0) * seg->dxdy;
    }
    seg->mx = (seg->sx0 <= seg->sx1) ? seg->sx0 : seg->sx1;
    seg->prev = seg->next = NULL;
  }

  //--- sort the inactive segments by iy, mx
  if (aaChanged) {
#if HAVE_STD_SORT
    std::sort(xPath->segs, xPath->segs + xPath->length, &SplashXPathSeg::cmpMX);
#else
    qsort(xPath->segs, xPath->length, sizeof(SplashXPathSeg),
	  &SplashXPathSeg::cmpMX);
#endif
  }

  //--- initialize the active list
  pre->prev = NULL;
  pre->next = post;
  post->prev = pre;
  post->next = NULL;

  //--- initialize the scan state
  nextSeg = 0;
  if (xPath->length) {
    yBottomI = xPath->segs[0].iy;
    if (aa) {
      yBottomI -= yBottomI % aaVert;
    }
  } else {
    yBottomI = 0;
  }
  yTopI = yBottomI - 1;
  if (aa) {
    yTop = (SplashCoord)yTopI / (SplashCoord)aaVert;
    yBottom = (SplashCoord)yBottomI / (SplashCoord)aaVert;
  } else {
    yTop = (SplashCoord)yTopI;
    yBottom = (SplashCoord)yBottomI;
  }

  resetDone = gTrue;
  resetAA = aa;
}

void SplashXPathScanner::skip(int newYBottomI, GBool aa) {
  SplashXPathSeg *s0, *s1,*s2;
  int iy;

  yTopI = newYBottomI - 1;
  yBottomI = newYBottomI;
  if (aa) {
    yTop = (SplashCoord)yTopI / (SplashCoord)aaVert;
    yBottom = (SplashCoord)yBottomI / (SplashCoord)aaVert;
  } else {
    yTop = (SplashCoord)yTopI;
    yBottom = (SplashCoord)yBottomI;
  }

  //--- remove finished segments; update sx0, sx1, mx for active segments
  s0 = pre->next;
  while (s0 != post) {
    s1 = s0->next;

    // check for a finished segment
    if (s0->y1 < yTop) {
      removeSegment(s0);

    // compute sx0, sx1, mx
    } else {
      if (s0->y0 >= yTop) {
	s0->sx0 = s0->x0;
      } else {
	s0->sx0 = s0->x0 + (yTop - s0->y0) * s0->dxdy;
      }
      if (s0->y1 <= yBottom) {
	s0->sx1 = s0->x1;
      } else {
	s0->sx1 = s0->x0 + (yBottom - s0->y0) * s0->dxdy;
      }
      s0->mx = (s0->sx0 <= s0->sx1) ? s0->sx0 : s0->sx1;
    }

    s0 = s1;
  }

  //--- check for intersections
  s0 = pre->next;
  if (s0 != post) {
    s1 = s0->next;
    while (s1 != post) {
      if (s1->mx < s0->mx) {
	for (s2 = s0->prev; s1->mx < s2->mx; s2 = s2->prev) ;
	moveSegmentAfter(s1, s2);
      } else {
	s0 = s1;
      }
      s1 = s0->next;
    }
  }

  //--- insert new segments with a merge sort
  // - this has to be done one (sub)scanline at a time, because the
  //   inactive list is bin-sorted by (sub)scanline
  while (nextSeg < xPath->length && xPath->segs[nextSeg].iy <= yTopI) {
    // the first inactive segment determines the next scanline to process
    iy = xPath->segs[nextSeg].iy;
    s0 = pre->next;
    do {
      s1 = &xPath->segs[nextSeg];
      ++nextSeg;
      if (s1->y1 < yTop) {
	continue;
      }
      if (s1->y0 >= yTop) {
	s1->sx0 = s1->x0;
      } else {
	s1->sx0 = s1->x0 + (yTop - s1->y0) * s1->dxdy;
      }
      if (s1->y1 <= yBottom) {
	s1->sx1 = s1->x1;
      } else {
	s1->sx1 = s1->x0 + (yBottom - s1->y0) * s1->dxdy;
      }
      s1->mx = (s1->sx0 <= s1->sx1) ? s1->sx0 : s1->sx1;
      insertSegmentBefore(s1, s0);
    } while (nextSeg < xPath->length && xPath->segs[nextSeg].iy <= iy);
  }
}

void SplashXPathScanner::advance(GBool aa) {
  SplashXPathSeg *s, *sNext, *s1;

  yTopI = yBottomI;
  yTop = yBottom;
  yBottomI = yTopI + 1;
  if (aa) {
    yBottom = (SplashCoord)yBottomI / (SplashCoord)aaVert;
  } else {
    yBottom = (SplashCoord)yBottomI;
  }

  s = pre->next;
  while (s != post) {
    sNext = s->next;

    // check for a finished segment
    if (s->y1 < yTop) {
      removeSegment(s);

    } else {

      // compute sx0, sx1, mx
      s->sx0 = s->sx1;
      if (s->y1 <= yBottom) {
	s->sx1 = s->x1;
      } else {
	s->sx1 = s->x0 + (yBottom - s->y0) * s->dxdy;
      }
      s->mx = (s->sx0 <= s->sx1) ? s->sx0 : s->sx1;

      // check for intersection
      if (s->mx < s->prev->mx) {
	for (s1 = s->prev->prev; s->mx < s1->mx; s1 = s1->prev) ;
	moveSegmentAfter(s, s1);
      }
    }

    s = sNext;
  }

  // insert new segments
  s = pre->next;
  while (nextSeg < xPath->length && xPath->segs[nextSeg].iy <= yTopI) {
    s1 = &xPath->segs[nextSeg];
    ++nextSeg;
    while (s1->mx > s->mx) {
      s = s->next;
    }
    insertSegmentBefore(s1, s);
  }
}

void SplashXPathScanner::generatePixels(int x0, int x1, Guchar *line,
					int *xMin, int *xMax) {
  SplashXPathSeg *s;
  int fillCount, x, xEnd, ix0, ix1, t;

  fillCount = 0;
  x = x0 * aaHoriz;
  xEnd = (x1 + 1) * aaHoriz;
  for (s = pre->next; s != post && x < xEnd; s = s->next) {
    ix0 = splashFloor(s->sx0 * aaHoriz);
    ix1 = splashFloor(s->sx1 * aaHoriz);
    if (ix0 > ix1) {
      t = ix0;  ix0 = ix1;  ix1 = t;
    }
    if (!(fillCount & eoMask)) {
      if (ix0 > x) {
	x = ix0;
      }
    }
    if (ix1 >= xEnd) {
      ix1 = xEnd - 1;
    }
    if (x / aaHoriz < *xMin) {
      *xMin = x / aaHoriz;
    }
    if (ix1 / aaHoriz > *xMax) {
      *xMax = ix1 / aaHoriz;
    }
    for (; x <= ix1; ++x) {
      ++line[x / aaHoriz];
    }
    if (s->y0 <= yTop && s->y1 > yTop) {
      fillCount += s->count;
    }
  }
}

void SplashXPathScanner::generatePixelsBinary(int x0, int x1, Guchar *line,
					      int *xMin, int *xMax) {
  SplashXPathSeg *s;
  int fillCount, x, xEnd, ix0, ix1, t;

  fillCount = 0;
  x = x0;
  xEnd = x1 + 1;
  for (s = pre->next; s != post && x < xEnd; s = s->next) {
    ix0 = splashFloor(s->sx0);
    ix1 = splashFloor(s->sx1);
    if (ix0 > ix1) {
      t = ix0;  ix0 = ix1;  ix1 = t;
    }
    if (!(fillCount & eoMask)) {
      if (ix0 > x) {
	x = ix0;
      }
    }
    if (ix1 >= xEnd) {
      ix1 = xEnd - 1;
    }
    if (x < *xMin) {
      *xMin = x;
    }
    if (ix1 > *xMax) {
      *xMax = ix1;
    }
    for (; x <= ix1; ++x) {
      line[x] = 255;
    }
    if (s->y0 <= yTop && s->y1 > yTop) {
      fillCount += s->count;
    }
  }
}

void SplashXPathScanner::drawRectangleSpan(Guchar *line, int y,
					   int x0, int x1,
					   int *xMin, int *xMax) {
  SplashCoord edge;
  Guchar pix;
  int xe, x;

  if (rectX0I > x1 || rectX1I < x0) {
    return;
  }

  *xMin = x0 <= rectX0I ? rectX0I : x0;
  *xMax = rectX1I <= x1 ? rectX1I : x1;

  //--- upper edge
  if (y == rectY0I) {

    // handle a rectangle less than one pixel high
    if (rectY0I == rectY1I) {
      edge = xPath->rectY1 - xPath->rectY0;
    } else {
      edge = (SplashCoord)1 - (xPath->rectY0 - rectY0I);
    }

    // upper left corner
    if (x0 <= rectX0I) {
      pix = (Guchar)splashCeil(edge
			       * ((SplashCoord)1 - (xPath->rectX0 - rectX0I))
			       * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX0I] = pix;
      x = rectX0I + 1;
    } else {
      x = x0;
    }

    // upper right corner
    if (rectX1I <= x1) {
      pix = (Guchar)splashCeil(edge * (xPath->rectX1 - rectX1I) * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX1I] = pix;
      xe = rectX1I - 1;
    } else {
      xe = x1;
    }

    // upper edge
    pix = (Guchar)splashCeil(edge * 255);
    if (pix < 16) {
      pix = 16;
    }
    for (; x <= xe; ++x) {
      line[x] = pix;
    }

  //--- lower edge
  } else if (y == rectY1I) {
    edge = xPath->rectY1 - rectY1I;

    // lower left corner
    if (x0 <= rectX0I) {
      pix = (Guchar)splashCeil(edge
			       * ((SplashCoord)1 - (xPath->rectX0 - rectX0I))
			       * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX0I] = pix;
      x = rectX0I + 1;
    } else {
      x = x0;
    }

    // lower right corner
    if (rectX1I <= x1) {
      pix = (Guchar)splashCeil(edge * (xPath->rectX1 - rectX1I) * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX1I] = pix;
      xe = rectX1I - 1;
    } else {
      xe = x1;
    }

    // lower edge
    pix = (Guchar)splashCeil(edge * 255);
    if (pix < 16) {
      pix = 16;
    }
    for (; x <= xe; ++x) {
      line[x] = pix;
    }

  //--- between the upper and lower edges
  } else if (y > rectY0I && y < rectY1I) {

    // left edge
    if (x0 <= rectX0I) {
      pix = (Guchar)splashCeil(((SplashCoord)1 - (xPath->rectX0 - rectX0I))
			       * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX0I] = pix;
      x = rectX0I + 1;
    } else {
      x = x0;
    }

    // right edge
    if (rectX1I <= x1) {
      pix = (Guchar)splashCeil((xPath->rectX1 - rectX1I) * 255);
      if (pix < 16) {
	pix = 16;
      }
      line[rectX1I] = pix;
      xe = rectX1I - 1;
    } else {
      xe = x1;
    }

    // middle
    for (; x <= xe; ++x) {
      line[x] = 255;
    }
  }
}

void SplashXPathScanner::drawRectangleSpanBinary(Guchar *line, int y,
						 int x0, int x1,
						 int *xMin, int *xMax) {
  int xe, x;

  if (y >= rectY0I && y <= rectY1I) {
    if (x0 <= rectX0I) {
      x = rectX0I;
    } else {
      x = x0;
    }
    *xMin = x;
    if (rectX1I <= x1) {
      xe = rectX1I;
    } else {
      xe = x1;
    }
    *xMax = xe;
    for (; x <= xe; ++x) {
      line[x] = 255;
    }
  }
}

void SplashXPathScanner::getSpan(Guchar *line, int y, int x0, int x1,
				 int *xMin, int *xMax) {
  int iy, x, k;

  iy = y * aaVert;
  if (!resetDone || !resetAA) {
    reset(gTrue, gTrue);
  } else if (yBottomI > iy) {
    reset(gTrue, gFalse);
  }
  memset(line + x0, 0, x1 - x0 + 1);

  *xMin = x1 + 1;
  *xMax = x0 - 1;

  if (xPath->isRect) {
    drawRectangleSpan(line, y, x0, x1, xMin, xMax);
    return;
  }

  if (yBottomI < iy) {
    skip(iy, gTrue);
  }
  for (k = 0; k < aaVert; ++k, ++iy) {
    advance(gTrue);
    generatePixels(x0, x1, line, xMin, xMax);
  }

#if !ANTIALIAS_256
  for (x = *xMin; x <= *xMax; ++x) {
    line[x] = map16to255[line[x]];
  }
#endif
}

void SplashXPathScanner::getSpanBinary(Guchar *line, int y, int x0, int x1,
				       int *xMin, int *xMax) {
  int iy;

  iy = y;
  if (!resetDone || resetAA) {
    reset(gFalse, gTrue);
  } else if (yBottomI > iy) {
    reset(gFalse, gFalse);
  }
  memset(line + x0, 0, x1 - x0 + 1);

  *xMin = x1 + 1;
  *xMax = x0 - 1;

  if (xPath->isRect) {
    drawRectangleSpanBinary(line, y, x0, x1, xMin, xMax);
    return;
  }

  if (yBottomI < iy) {
    skip(iy, gFalse);
  }
  advance(gFalse);
  generatePixelsBinary(x0, x1, line, xMin, xMax);
}
