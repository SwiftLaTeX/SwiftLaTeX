//========================================================================
//
// SplashClip.cc
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
#include "gmem.h"
#include "gmempp.h"
#include "SplashErrorCodes.h"
#include "SplashPath.h"
#include "SplashXPath.h"
#include "SplashXPathScanner.h"
#include "SplashClip.h"

//------------------------------------------------------------------------

// Compute x * y / 255, where x and y are in [0, 255].
static inline Guchar mul255(Guchar x, Guchar y) {
  int z;

  z = (int)x * (int)y;
  return (Guchar)((z + (z >> 8) + 0x80) >> 8);
}

//------------------------------------------------------------------------
// SplashClip
//------------------------------------------------------------------------

SplashClip::SplashClip(int hardXMinA, int hardYMinA,
		       int hardXMaxA, int hardYMaxA) {
  int w;

  hardXMin = hardXMinA;
  hardYMin = hardYMinA;
  hardXMax = hardXMaxA;
  hardYMax = hardYMaxA;
  xMin = hardXMin;
  yMin = hardYMin;
  xMax = hardXMax;
  yMax = hardYMax;
  intBoundsValid = gFalse;
  paths = NULL;
  eo = NULL;
  scanners = NULL;
  length = size = 0;
  isSimple = gTrue;
  prev = NULL;
  if ((w = hardXMax + 1) <= 0) {
    w = 1;
  }
  buf = (Guchar *)gmalloc(w);
}

SplashClip::SplashClip(SplashClip *clip) {
  int w;

  hardXMin = clip->hardXMin;
  hardYMin = clip->hardYMin;
  hardXMax = clip->hardXMax;
  hardYMax = clip->hardYMax;
  xMin = clip->xMin;
  yMin = clip->yMin;
  xMax = clip->xMax;
  yMax = clip->yMax;
  xMinI = clip->xMinI;
  yMinI = clip->yMinI;
  xMaxI = clip->xMaxI;
  yMaxI = clip->yMaxI;
  intBoundsValid = clip->intBoundsValid;
  intBoundsStrokeAdjust = clip->intBoundsStrokeAdjust;
  paths = NULL;
  eo = NULL;
  scanners = NULL;
  length = size = 0;
  isSimple = clip->isSimple;
  prev = clip;
  if ((w = splashCeil(xMax)) <= 0) {
    w = 1;
  }
  buf = (Guchar *)gmalloc(w);
}

SplashClip::~SplashClip() {
  int i;

  for (i = 0; i < length; ++i) {
    delete scanners[i];
    delete paths[i];
  }
  gfree(paths);
  gfree(eo);
  gfree(scanners);
  gfree(buf);
}

void SplashClip::grow(int nPaths) {
  if (length + nPaths > size) {
    if (size == 0) {
      size = 32;
    }
    while (size < length + nPaths) {
      size *= 2;
    }
    paths = (SplashXPath **)greallocn(paths, size, sizeof(SplashXPath *));
    eo = (Guchar *)greallocn(eo, size, sizeof(Guchar));
    scanners = (SplashXPathScanner **)
                   greallocn(scanners, size, sizeof(SplashXPathScanner *));
  }
}

void SplashClip::resetToRect(SplashCoord x0, SplashCoord y0,
			     SplashCoord x1, SplashCoord y1) {
  int w, i;

  for (i = 0; i < length; ++i) {
    delete paths[i];
    delete scanners[i];
  }
  gfree(paths);
  gfree(eo);
  gfree(scanners);
  gfree(buf);
  paths = NULL;
  eo = NULL;
  scanners = NULL;
  length = size = 0;
  isSimple = gTrue;
  prev = NULL;

  if (x0 < x1) {
    xMin = x0;
    xMax = x1;
  } else {
    xMin = x1;
    xMax = x0;
  }
  if (y0 < y1) {
    yMin = y0;
    yMax = y1;
  } else {
    yMin = y1;
    yMax = y0;
  }
  intBoundsValid = gFalse;
  if ((w = splashCeil(xMax)) <= 0) {
    w = 1;
  }
  buf = (Guchar *)gmalloc(w);
}

SplashError SplashClip::clipToRect(SplashCoord x0, SplashCoord y0,
				   SplashCoord x1, SplashCoord y1) {
  if (x0 < x1) {
    if (x0 > xMin) {
      xMin = x0;
      intBoundsValid = gFalse;
    }
    if (x1 < xMax) {
      xMax = x1;
      intBoundsValid = gFalse;
    }
  } else {
    if (x1 > xMin) {
      xMin = x1;
      intBoundsValid = gFalse;
    }
    if (x0 < xMax) {
      xMax = x0;
      intBoundsValid = gFalse;
    }
  }
  if (y0 < y1) {
    if (y0 > yMin) {
      yMin = y0;
      intBoundsValid = gFalse;
    }
    if (y1 < yMax) {
      yMax = y1;
      intBoundsValid = gFalse;
    }
  } else {
    if (y1 > yMin) {
      yMin = y1;
      intBoundsValid = gFalse;
    }
    if (y0 < yMax) {
      yMax = y0;
      intBoundsValid = gFalse;
    }
  }
  return splashOk;
}

SplashError SplashClip::clipToPath(SplashPath *path, SplashCoord *matrix,
				   SplashCoord flatness, GBool eoA,
				   GBool enablePathSimplification,
				   SplashStrokeAdjustMode strokeAdjust) {
  SplashXPath *xPath;
  SplashCoord t;

  xPath = new SplashXPath(path, matrix, flatness, gTrue,
			  enablePathSimplification,
			  strokeAdjust);

  // check for an empty path
  if (xPath->length == 0) {
    xMin = yMin = 1;
    xMax = yMax = 0;
    intBoundsValid = gFalse;
    delete xPath;
    return splashOk;
  }

  // check for a rectangle
  if (xPath->isRect) {
    clipToRect(xPath->rectX0, xPath->rectY0, xPath->rectX1, xPath->rectY1);
    delete xPath;
    return splashOk;
  }

  grow(1);
  paths[length] = xPath;
  eo[length] = (Guchar)eoA;
  if ((t = xPath->getXMin()) > xMin) {
    xMin = t;
  }
  if ((t = xPath->getYMin()) > yMin) {
    yMin = t;
  }
  if ((t = xPath->getXMax() + 1) < xMax) {
    xMax = t;
  }
  if ((t = xPath->getYMax() + 1) < yMax) {
    yMax = t;
  }
  intBoundsValid = gFalse;
  scanners[length] = new SplashXPathScanner(xPath, eoA, splashFloor(yMin),
					    splashCeil(yMax) - 1);
  ++length;
  isSimple = gFalse;

  return splashOk;
}

SplashClipResult SplashClip::testRect(int rectXMin, int rectYMin,
				      int rectXMax, int rectYMax,
				      SplashStrokeAdjustMode strokeAdjust) {
  // In general, this function tests the rectangle:
  //     x = [rectXMin, rectXMax + 1)    (note: coords are ints)
  //     y = [rectYMin, rectYMax + 1)
  // against the clipping region:
  //     x = [xMin, xMax)                (note: coords are fp)
  //     y = [yMin, yMax)

  if (strokeAdjust != splashStrokeAdjustOff && isSimple) {
    // special case for stroke adjustment with a simple clipping
    // rectangle -- the clipping region is:
    //     x = [xMinI, xMaxI + 1)
    //     y = [yMinI, yMaxI + 1)
    updateIntBounds(strokeAdjust);
    if (xMinI > xMaxI || yMinI > yMaxI) {
      return splashClipAllOutside;
    }
    if (rectXMax + 1 <= xMinI ||
	rectXMin >= xMaxI + 1 ||
	rectYMax + 1 <= yMinI ||
	rectYMin >= yMaxI + 1) {
      return splashClipAllOutside;
    }
    if (rectXMin >= xMinI &&
	rectXMax <= xMaxI &&
	rectYMin >= yMinI &&
	rectYMax <= yMaxI) {
      return splashClipAllInside;
    }
  } else {
    if (xMin >= xMax || yMin >= yMax) {
      return splashClipAllOutside;
    }
    if ((SplashCoord)(rectXMax + 1) <= xMin ||
	(SplashCoord)rectXMin >= xMax ||
	(SplashCoord)(rectYMax + 1) <= yMin ||
	(SplashCoord)rectYMin >= yMax) {
      return splashClipAllOutside;
    }
    if (isSimple &&
	(SplashCoord)rectXMin >= xMin &&
	(SplashCoord)(rectXMax + 1) <= xMax &&
	(SplashCoord)rectYMin >= yMin &&
	(SplashCoord)(rectYMax + 1) <= yMax) {
      return splashClipAllInside;
    }
  }
  return splashClipPartial;
}

void SplashClip::clipSpan(Guchar *line, int y, int x0, int x1,
			  SplashStrokeAdjustMode strokeAdjust) {
  SplashClip *clip;
  SplashCoord d;
  int x0a, x1a, x0b, x1b, x, i;

  updateIntBounds(strokeAdjust);

  //--- clip to the integer rectangle

  if (y < yMinI || y > yMaxI ||
      x1 < xMinI || x0 > xMaxI) {
    memset(line + x0, 0, x1 - x0 + 1);
    return;
  }

  if (x0 > xMinI) {
    x0a = x0;
  } else {
    x0a = xMinI;
    memset(line + x0, 0, x0a - x0);
  }

  if (x1 < xMaxI) {
    x1a = x1;
  } else {
    x1a = xMaxI;
    memset(line + x1a + 1, 0, x1 - x1a);
  }

  if (x0a > x1a) {
    return;
  }

  //--- clip to the floating point rectangle
  //    (if stroke adjustment is disabled)

  if (strokeAdjust == splashStrokeAdjustOff) {

    // clip left edge (xMin)
    if (x0a == xMinI) {
      d = (SplashCoord)(xMinI + 1) - xMin;
      line[x0a] = (Guchar)(int)((SplashCoord)line[x0a] * d);
    }

    // clip right edge (xMax)
    if (x1a == xMaxI) {
      d = xMax - (SplashCoord)xMaxI;
      line[x1a] = (Guchar)(int)((SplashCoord)line[x1a] * d);
    }

    // clip top edge (yMin)
    if (y == yMinI) {
      d = (SplashCoord)(yMinI + 1) - yMin;
      for (x = x0a; x <= x1a; ++x) {
	line[x] = (Guchar)(int)((SplashCoord)line[x] * d);
      }
    }

    // clip bottom edge (yMax)
    if (y == yMaxI) {
      d = yMax - (SplashCoord)yMaxI;
      for (x = x0a; x <= x1a; ++x) {
	line[x] = (Guchar)(int)((SplashCoord)line[x] * d);
      }
    }
  }

  if (isSimple) {
    return;
  }

  //--- clip to the paths

  for (clip = this; clip; clip = clip->prev) {
    for (i = 0; i < clip->length; ++i) {
      clip->scanners[i]->getSpan(buf, y, x0a, x1a, &x0b, &x1b);
      if (x0a < x0b) {
	memset(line + x0a, 0, x0b - x0a);
      }
      for (x = x0b; x <= x1b; ++x) {
	line[x] = mul255(line[x], buf[x]);
      }
      if (x1b < x1a) {
	memset(line + x1b + 1, 0, x1a - x1b);
      }
    }
  }
}

GBool SplashClip::clipSpanBinary(Guchar *line, int y, int x0, int x1,
				 SplashStrokeAdjustMode strokeAdjust) {
  SplashClip *clip;
  int x0a, x1a, x0b, x1b, x, i;
  Guchar any;

  updateIntBounds(strokeAdjust);

  if (y < yMinI || y > yMaxI ||
      x1 < xMinI || x0 > xMaxI) {
    if (x0 <= x1) {
      memset(line + x0, 0, x1 - x0 + 1);
    }
    return gFalse;
  }

  if (x0 > xMinI) {
    x0a = x0;
  } else {
    x0a = xMinI;
    memset(line + x0, 0, x0a - x0);
  }

  if (x1 < xMaxI) {
    x1a = x1;
  } else {
    x1a = xMaxI;
    memset(line + x1a + 1, 0, x1 - x1a);
  }

  if (x0a > x1a) {
    return gFalse;
  }

  if (isSimple) {
    for (x = x0a; x <= x1a; ++x) {
      if (line[x]) {
	return gTrue;
      }
    }
    return gFalse;
  }

  any = 0;
  for (clip = this; clip; clip = clip->prev) {
    for (i = 0; i < clip->length; ++i) {
      clip->scanners[i]->getSpanBinary(buf, y, x0a, x1a, &x0b, &x1b);
      if (x0a < x0b) {
	memset(line + x0a, 0, x0b - x0a);
      }
      for (x = x0b; x <= x1b; ++x) {
	line[x] &= buf[x];
	any |= line[x];
      }
      if (x1b < x1a) {
	memset(line + x1b + 1, 0, x1a - x1b);
      }
    }
  }

  return any != 0;
}

int SplashClip::getXMinI(SplashStrokeAdjustMode strokeAdjust) {
  updateIntBounds(strokeAdjust);
  return xMinI;
}

int SplashClip::getXMaxI(SplashStrokeAdjustMode strokeAdjust) {
  updateIntBounds(strokeAdjust);
  return xMaxI;
}

int SplashClip::getYMinI(SplashStrokeAdjustMode strokeAdjust) {
  updateIntBounds(strokeAdjust);
  return yMinI;
}

int SplashClip::getYMaxI(SplashStrokeAdjustMode strokeAdjust) {
  updateIntBounds(strokeAdjust);
  return yMaxI;
}

int SplashClip::getNumPaths() {
  SplashClip *clip;
  int n;

  n = 0;
  for (clip = this; clip; clip = clip->prev) {
    n += clip->length;
  }
  return n;
}

void SplashClip::updateIntBounds(SplashStrokeAdjustMode strokeAdjust) {
  if (intBoundsValid && strokeAdjust == intBoundsStrokeAdjust) {
    return;
  }
  if (strokeAdjust != splashStrokeAdjustOff && isSimple) {
    splashStrokeAdjust(xMin, xMax, &xMinI, &xMaxI, strokeAdjust);
    splashStrokeAdjust(yMin, yMax, &yMinI, &yMaxI, strokeAdjust);
  } else {
    xMinI = splashFloor(xMin);
    yMinI = splashFloor(yMin);
    xMaxI = splashCeil(xMax);
    yMaxI = splashCeil(yMax);
  }
  if (xMinI < hardXMin) {
    xMinI = hardXMin;
  }
  if (yMinI < hardYMin) {
    yMinI = hardYMin;
  }
  if (xMaxI > hardXMax) {
    xMaxI = hardXMax;
  }
  if (yMaxI > hardYMax) {
    yMaxI = hardYMax;
  }
  // the clipping code uses [xMinI, xMaxI] instead of [xMinI, xMaxI)
  --xMaxI;
  --yMaxI;
  intBoundsValid = gTrue;
  intBoundsStrokeAdjust = strokeAdjust;
}
