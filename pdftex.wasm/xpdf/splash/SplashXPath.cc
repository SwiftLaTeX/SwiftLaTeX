//========================================================================
//
// SplashXPath.cc
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
#include "SplashMath.h"
#include "SplashPath.h"
#include "SplashXPath.h"

//------------------------------------------------------------------------

#define minCosSquaredJoinAngle 0.75
#define maxPointToLineDistanceSquared 0.04

//------------------------------------------------------------------------

struct SplashXPathPoint {
  SplashCoord x, y;
};

struct SplashXPathAdjust {
  int firstPt, lastPt;		// range of points
  GBool vert;			// vertical or horizontal hint
  SplashCoord x0a, x0b,		// hint boundaries
              xma, xmb,
              x1a, x1b;
  SplashCoord x0, x1, xm;	// adjusted coordinates
};

//------------------------------------------------------------------------

// Transform a point from user space to device space.
inline void SplashXPath::transform(SplashCoord *matrix,
				   SplashCoord xi, SplashCoord yi,
				   SplashCoord *xo, SplashCoord *yo) {
  //                          [ m[0] m[1] 0 ]
  // [xo yo 1] = [xi yi 1] *  [ m[2] m[3] 0 ]
  //                          [ m[4] m[5] 1 ]
  *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
  *yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
}

//------------------------------------------------------------------------
// SplashXPath
//------------------------------------------------------------------------

// SplashXPath segment coords are clipped to +/-maxCoord to avoid
// problems.  The xMin/yMin/xMax/yMax fields are 32-bit integers, so
// coords need to be < 2^31 / aa{Horiz,Vert}.
#define maxCoord 100000000.0

void SplashXPath::clampCoords(SplashCoord *x, SplashCoord *y) {
#if !USE_FIXEDPOINT
  if (*x > maxCoord) {
    *x = maxCoord;
  } else if (*x < -maxCoord) {
    *x = -maxCoord;
  }
  if (*y > maxCoord) {
    *y = maxCoord;
  } else if (*y < -maxCoord) {
    *y = -maxCoord;
  }
#endif
}

SplashXPath::SplashXPath(SplashPath *path, SplashCoord *matrix,
			 SplashCoord flatness, GBool closeSubpaths,
			 GBool simplify,
			 SplashStrokeAdjustMode strokeAdjMode) {
  SplashXPathPoint *pts;
  SplashCoord x0, y0, x1, y1, x2, y2, x3, y3, xsp, ysp, t;
  int curSubpath, firstSegInSubpath, i;
  GBool adjusted;

  //--- transform the points
  pts = (SplashXPathPoint *)gmallocn(path->length, sizeof(SplashXPathPoint));
  for (i = 0; i < path->length; ++i) {
    transform(matrix, path->pts[i].x, path->pts[i].y, &pts[i].x, &pts[i].y);
    clampCoords(&pts[i].x, &pts[i].y);
  }

  //--- do stroke adjustment
  if (path->hints) {
    adjusted = strokeAdjust(pts, path->hints, path->hintsLength,
			    strokeAdjMode);
  } else {
    adjusted = gFalse;
  }

  //--- construct the segments

  segs = NULL;
  length = size = 0;

  x0 = y0 = xsp = ysp = 0; // make gcc happy
  curSubpath = 0;
  firstSegInSubpath = 0;
  i = 0;
  while (i < path->length) {

    // first point in subpath - skip it
    if (path->flags[i] & splashPathFirst) {
      x0 = pts[i].x;
      y0 = pts[i].y;
      xsp = x0;
      ysp = y0;
      curSubpath = i;
      ++i;

    } else {

      // curve segment
      if (path->flags[i] & splashPathCurve) {
	x1 = pts[i].x;
	y1 = pts[i].y;
	x2 = pts[i+1].x;
	y2 = pts[i+1].y;
	x3 = pts[i+2].x;
	y3 = pts[i+2].y;
	addCurve(x0, y0, x1, y1, x2, y2, x3, y3,
		 flatness,
		 (path->flags[i-1] & splashPathFirst),
		 (path->flags[i+2] & splashPathLast),
		 !closeSubpaths &&
		   (path->flags[i-1] & splashPathFirst) &&
		   !(path->flags[i-1] & splashPathClosed),
		 !closeSubpaths &&
		   (path->flags[i+2] & splashPathLast) &&
		   !(path->flags[i+2] & splashPathClosed));
	x0 = x3;
	y0 = y3;
	i += 3;

      // line segment
      } else {
	x1 = pts[i].x;
	y1 = pts[i].y;
	addSegment(x0, y0, x1, y1);
	x0 = x1;
	y0 = y1;
	++i;
      }

      // end a subpath
      if (path->flags[i-1] & splashPathLast) {
	if (closeSubpaths &&
	    (pts[i-1].x != pts[curSubpath].x ||
	     pts[i-1].y != pts[curSubpath].y)) {
	  addSegment(x0, y0, xsp, ysp);
	}
	if (simplify && !adjusted) {
	  mergeSegments(firstSegInSubpath);
	}
	firstSegInSubpath = length;
      }
    }
  }

  gfree(pts);

  finishSegments();

  //--- check for a rectangle
  isRect = gFalse;
  rectX0 = rectY0 = rectX1 = rectY1 = 0;
  if (length == 4) {
#if HAVE_STD_SORT
    std::sort(segs, segs + length, SplashXPathSeg::cmpY);
#else
    qsort(segs, length, sizeof(SplashXPathSeg), &SplashXPathSeg::cmpY);
#endif
    if (segs[0].y0 == segs[0].y1 &&
	segs[1].x0 == segs[1].x1 &&
	segs[2].x0 == segs[2].x1 &&
	segs[3].y0 == segs[3].y1) {
      isRect = gTrue;
      rectX0 = segs[1].x0;
      rectX1 = segs[2].x0;
      rectY0 = segs[0].y0;
      rectY1 = segs[3].y0;
    } else if (segs[0].x0 == segs[0].x1 &&
	       segs[1].y0 == segs[1].y1 &&
	       segs[2].x0 == segs[2].x1 &&
	       segs[3].y0 == segs[3].y1) {
      isRect = gTrue;
      rectX0 = segs[0].x0;
      rectX1 = segs[2].x0;
      rectY0 = segs[1].y0;
      rectY1 = segs[3].y0;
    } else if (segs[0].x0 == segs[0].x1 &&
	       segs[1].x0 == segs[1].x1 &&
	       segs[2].y0 == segs[2].y1 &&
	       segs[3].y0 == segs[3].y1) {
      isRect = gTrue;
      rectX0 = segs[0].x0;
      rectX1 = segs[1].x0;
      rectY0 = segs[2].y0;
      rectY1 = segs[3].y0;
    }
    if (isRect) {
      if (rectX0 > rectX1) {
	t = rectX0;  rectX0 = rectX1;  rectX1 = t;
      }
      if (rectY0 > rectY1) {
	t = rectY0;  rectY0 = rectY1;  rectY1 = t;
      }
    }
  }
}

GBool SplashXPath::strokeAdjust(SplashXPathPoint *pts,
				SplashPathHint *hints, int nHints,
				SplashStrokeAdjustMode strokeAdjMode) {
  SplashXPathAdjust *adjusts, *adjust;
  SplashPathHint *hint;
  SplashCoord x0, y0, x1, y1, x2, y2, x3, y3;
  SplashCoord adj0, adj1, w, d;
  int xi0, xi1;
  int i, j;
  GBool adjusted;

  adjusted = gFalse;

  // set up the stroke adjustment hints
  adjusts = (SplashXPathAdjust *)gmallocn(nHints, sizeof(SplashXPathAdjust));
  for (i = 0; i < nHints; ++i) {
    hint = &hints[i];
    x0 = pts[hint->ctrl0    ].x;    y0 = pts[hint->ctrl0    ].y;
    x1 = pts[hint->ctrl0 + 1].x;    y1 = pts[hint->ctrl0 + 1].y;
    x2 = pts[hint->ctrl1    ].x;    y2 = pts[hint->ctrl1    ].y;
    x3 = pts[hint->ctrl1 + 1].x;    y3 = pts[hint->ctrl1 + 1].y;
    w = -1;
    if (splashAbs(x0 - x1) < 0.01 && splashAbs(x2 - x3) < 0.01) {
      adjusts[i].vert = gTrue;
      adj0 = x0;
      adj1 = x2;
      if (hint->projectingCap) {
	w = splashAbs(y1 - y0);
      }
    } else if (splashAbs(y0 - y1) < 0.01 && splashAbs(y2 - y3) < 0.01) {
      adjusts[i].vert = gFalse;
      adj0 = y0;
      adj1 = y2;
      if (hint->projectingCap) {
	w = splashAbs(x1 - x0);
      }
    } else {
      goto done;
    }
    if (adj0 > adj1) {
      x0 = adj0;
      adj0 = adj1;
      adj1 = x0;
    }
    d = adj1 - adj0;
    if (d > 0.04) {
      d = 0.01;
    } else {
      d *= 0.25;
    }
    adjusts[i].x0a = adj0 - d;
    adjusts[i].x0b = adj0 + d;
    adjusts[i].xma = (SplashCoord)0.5 * (adj0 + adj1) - d;
    adjusts[i].xmb = (SplashCoord)0.5 * (adj0 + adj1) + d;
    adjusts[i].x1a = adj1 - d;
    adjusts[i].x1b = adj1 + d;
    splashStrokeAdjust(adj0, adj1, &xi0, &xi1, strokeAdjMode, w);
    adjusts[i].x0 = (SplashCoord)xi0;
    // the "minus epsilon" thing here is needed when vector
    // antialiasing is turned off -- otherwise stroke adjusted lines
    // will touch an extra pixel on one edge
    adjusts[i].x1 = (SplashCoord)xi1 - 0.001;
    adjusts[i].xm = (SplashCoord)0.5 * (adjusts[i].x0 + adjusts[i].x1);
    adjusts[i].firstPt = hint->firstPt;
    adjusts[i].lastPt = hint->lastPt;
  }

  // perform stroke adjustment
  for (i = 0, adjust = adjusts; i < nHints; ++i, ++adjust) {
    for (j = adjust->firstPt; j <= adjust->lastPt; ++j) {
      if (adjust->vert) {
	x0 = pts[j].x;
	if (x0 > adjust->x0a && x0 < adjust->x0b) {
	  pts[j].x = adjust->x0;
	} else if (x0 > adjust->xma && x0 < adjust->xmb) {
	  pts[j].x = adjust->xm;
	} else if (x0 > adjust->x1a && x0 < adjust->x1b) {
	  pts[j].x = adjust->x1;
	}
      } else {
	y0 = pts[j].y;
	if (y0 > adjust->x0a && y0 < adjust->x0b) {
	  pts[j].y = adjust->x0;
	} else if (y0 > adjust->xma && y0 < adjust->xmb) {
	  pts[j].y = adjust->xm;
	} else if (y0 > adjust->x1a && y0 < adjust->x1b) {
	  pts[j].y = adjust->x1;
	}
      }
    }
  }
  adjusted = gTrue;

 done:
  gfree(adjusts);
  return adjusted;
}

SplashXPath::SplashXPath(SplashXPath *xPath) {
  length = xPath->length;
  size = xPath->size;
  segs = (SplashXPathSeg *)gmallocn(size, sizeof(SplashXPathSeg));
  memcpy(segs, xPath->segs, length * sizeof(SplashXPathSeg));
  xMin = xPath->xMin;
  yMin = xPath->yMin;
  xMax = xPath->xMax;
  yMax = xPath->yMax;
}

SplashXPath::~SplashXPath() {
  gfree(segs);
}

// Add space for <nSegs> more segments
void SplashXPath::grow(int nSegs) {
  if (length + nSegs > size) {
    if (size == 0) {
      size = 32;
    }
    while (size < length + nSegs) {
      size *= 2;
    }
    segs = (SplashXPathSeg *)greallocn(segs, size, sizeof(SplashXPathSeg));
  }
}

void SplashXPath::addCurve(SplashCoord x0, SplashCoord y0,
			   SplashCoord x1, SplashCoord y1,
			   SplashCoord x2, SplashCoord y2,
			   SplashCoord x3, SplashCoord y3,
			   SplashCoord flatness,
			   GBool first, GBool last, GBool end0, GBool end1) {
  SplashCoord cx[splashMaxCurveSplits + 1][3];
  SplashCoord cy[splashMaxCurveSplits + 1][3];
  int cNext[splashMaxCurveSplits + 1];
  SplashCoord xl0, xl1, xl2, xr0, xr1, xr2, xr3, xx1, xx2, xh;
  SplashCoord yl0, yl1, yl2, yr0, yr1, yr2, yr3, yy1, yy2, yh;
  SplashCoord dx, dy, mx, my, d1, d2, flatness2;
  int p1, p2, p3;

#if USE_FIXEDPOINT
  flatness2 = flatness;
#else
  flatness2 = flatness * flatness;
#endif

  // initial segment
  p1 = 0;
  p2 = splashMaxCurveSplits;
  cx[p1][0] = x0;  cy[p1][0] = y0;
  cx[p1][1] = x1;  cy[p1][1] = y1;
  cx[p1][2] = x2;  cy[p1][2] = y2;
  cx[p2][0] = x3;  cy[p2][0] = y3;
  cNext[p1] = p2;

  while (p1 < splashMaxCurveSplits) {

    // get the next segment
    xl0 = cx[p1][0];  yl0 = cy[p1][0];
    xx1 = cx[p1][1];  yy1 = cy[p1][1];
    xx2 = cx[p1][2];  yy2 = cy[p1][2];
    p2 = cNext[p1];
    xr3 = cx[p2][0];  yr3 = cy[p2][0];

    // compute the distances from the control points to the
    // midpoint of the straight line (this is a bit of a hack, but
    // it's much faster than computing the actual distances to the
    // line)
    mx = (xl0 + xr3) * 0.5;
    my = (yl0 + yr3) * 0.5;
#if USE_FIXEDPOINT
    d1 = splashDist(xx1, yy1, mx, my);
    d2 = splashDist(xx2, yy2, mx, my);
#else
    dx = xx1 - mx;
    dy = yy1 - my;
    d1 = dx*dx + dy*dy;
    dx = xx2 - mx;
    dy = yy2 - my;
    d2 = dx*dx + dy*dy;
#endif

    // if the curve is flat enough, or no more subdivisions are
    // allowed, add the straight line segment
    if (p2 - p1 == 1 || (d1 <= flatness2 && d2 <= flatness2)) {
      addSegment(xl0, yl0, xr3, yr3);
      p1 = p2;

    // otherwise, subdivide the curve
    } else {
      xl1 = (xl0 + xx1) * 0.5;
      yl1 = (yl0 + yy1) * 0.5;
      xh = (xx1 + xx2) * 0.5;
      yh = (yy1 + yy2) * 0.5;
      xl2 = (xl1 + xh) * 0.5;
      yl2 = (yl1 + yh) * 0.5;
      xr2 = (xx2 + xr3) * 0.5;
      yr2 = (yy2 + yr3) * 0.5;
      xr1 = (xh + xr2) * 0.5;
      yr1 = (yh + yr2) * 0.5;
      xr0 = (xl2 + xr1) * 0.5;
      yr0 = (yl2 + yr1) * 0.5;
      // add the new subdivision points
      p3 = (p1 + p2) / 2;
      cx[p1][1] = xl1;  cy[p1][1] = yl1;
      cx[p1][2] = xl2;  cy[p1][2] = yl2;
      cNext[p1] = p3;
      cx[p3][0] = xr0;  cy[p3][0] = yr0;
      cx[p3][1] = xr1;  cy[p3][1] = yr1;
      cx[p3][2] = xr2;  cy[p3][2] = yr2;
      cNext[p3] = p2;
    }
  }
}

void SplashXPath::addSegment(SplashCoord x0, SplashCoord y0,
			     SplashCoord x1, SplashCoord y1) {
  grow(1);
  segs[length].x0 = x0;
  segs[length].y0 = y0;
  segs[length].x1 = x1;
  segs[length].y1 = y1;
  ++length;
}

// Returns true if the angle between (x0,y0)-(x1,y1) and
// (x1,y1)-(x2,y2) is close to 180 degrees.
static GBool joinAngleIsFlat(SplashCoord x0, SplashCoord y0,
			     SplashCoord x1, SplashCoord y1,
			     SplashCoord x2, SplashCoord y2) {
  SplashCoord dx1, dy1, dx2, dy2, d, len1, len2;

  dx1 = x1 - x0;
  dy1 = y1 - y0;
  dx2 = x2 - x1;
  dy2 = y2 - y1;
  d = dx1 * dx2 + dy1 * dy2;
  len1 = dx1 * dx1 + dy1 * dy1;
  len2 = dx2 * dx2 + dy2 * dy2;
  return d > 0 && d * d > len1 * len2 * minCosSquaredJoinAngle;
}

// Returns true if (x1,y1) is sufficiently close to the segment
// (x0,y0)-(x2,y2), looking at the perpendicular point-to-line
// distance.
static GBool pointCloseToSegment(SplashCoord x0, SplashCoord y0,
				 SplashCoord x1, SplashCoord y1,
				 SplashCoord x2, SplashCoord y2) {
  SplashCoord t1, t2, dx, dy;

  // compute the perpendicular distance from the point to the segment,
  // i.e., the projection of (x0,y0)-(x1,y1) onto a unit normal to the
  // segment (this actually computes the square of the distance)
  dx = x2 - x0;
  dy = y2 - y0;
  t1 = dx*dx + dy*dy;
  if (t1 < 0.0001) {
    // degenerate case: (x0,y0) and (x2,y2) are (nearly) identical --
    // just compute the distance to (x1,y1)
    dx = x0 - x1;
    dy = y0 - y1;
    t2 = dx*dx + dy*dy;
    return t2 < maxPointToLineDistanceSquared;
  }
  t2 = x1 * dy - dx * y1 - x0 * y2 + x2 * y0;
  // actual distance = t2 / sqrt(t1)
  return t2 * t2 < t1 * maxPointToLineDistanceSquared;
}

// Attempt to simplify the path by merging sequences of consecutive
// segments in [first] .. [length]-1.
void SplashXPath::mergeSegments(int first) {
  GBool horiz, vert;
  int in, out, prev, i, j;

  in = out = first;
  while (in < length) {

    // skip zero-length segments
    if (segs[in].x0 == segs[in].x1 && segs[in].y0 == segs[in].y1) {
      ++in;
      continue;
    }

    horiz = segs[in].y0 == segs[in].y1;
    vert = segs[in].x0 == segs[in].x1;

    // check for a sequence of mergeable segments: in .. i
    prev = in;
    for (i = in + 1; i < length; ++i) {

      // skip zero-length segments
      if (segs[i].x0 == segs[i].x1 && segs[i].y0 == segs[i].y1) {
	continue;
      }

      // check for a horizontal or vertical segment
      if ((horiz && segs[in].y0 != segs[in].y1) ||
	  (vert && segs[in].x0 != segs[in].x1)) {
	break;
      }

      // check the angle between segs i-1 and i
      // (actually, we compare seg i to the previous non-zero-length
      // segment, which may not be i-1)
      if (!joinAngleIsFlat(segs[prev].x0, segs[prev].y0,
			   segs[i].x0, segs[i].y0,
			   segs[i].x1, segs[i].y1)) {
	break;
      }

      // check the distances from the ends of segs in .. i-1 to the
      // proposed new segment
      for (j = in; j < i; ++j) {
	if (!pointCloseToSegment(segs[in].x0, segs[in].y0,
				 segs[j].x1, segs[j].y1,
				 segs[i].x1, segs[i].y1)) {
	  break;
	}
      }
      if (j < i) {
	break;
      }

      prev = i;
    }

    // we can merge segs: in .. i-1
    // (this may be the single segment: in)
    segs[out].x0 = segs[in].x0;
    segs[out].y0 = segs[in].y0;
    segs[out].x1 = segs[i-1].x1;
    segs[out].y1 = segs[i-1].y1;
    in = i;
    ++out;
  }

  length = out;
}

void SplashXPath::finishSegments() {
  SplashXPathSeg *seg;
  SplashCoord xMinFP, xMaxFP, yMinFP, yMaxFP, t;
  int i;

  xMinFP = yMinFP = xMaxFP = yMaxFP = 0;

  for (i = 0; i < length; ++i) {
    seg = &segs[i];

    //--- compute the slopes
    if (seg->y0 <= seg->y1) {
      seg->count = 1;
    } else {
      t = seg->x0;  seg->x0 = seg->x1;  seg->x1 = t;
      t = seg->y0;  seg->y0 = seg->y1;  seg->y1 = t;
      seg->count = -1;
    }
#if USE_FIXEDPOINT
    if (seg->y0 == seg->y1 || seg->x0 == seg->x1 ||
	!FixedPoint::divCheck(seg->x1 - seg->x0, seg->y1 - seg->y0,
			      &seg->dxdy) ||
	!FixedPoint::divCheck(seg->y1 - seg->y0, seg->x1 - seg->x0,
			      &seg->dydx)) {
      seg->dxdy = 0;
      seg->dydx = 0;
    }
#else
    if (seg->y0 == seg->y1 || seg->x0 == seg->x1) {
      seg->dxdy = 0;
      seg->dydx = 0;
    } else {
      seg->dxdy = (seg->x1 - seg->x0) / (seg->y1 - seg->y0);
      if (seg->dxdy == 0) {
	seg->dydx = 0;
      } else {
	seg->dydx = 1 / seg->dxdy;
      }
    }
#endif

    //--- update bbox
    if (i == 0) {
      if (seg->x0 <= seg->x1) {
	xMinFP = seg->x0;
	xMaxFP = seg->x1;
      } else {
	xMinFP = seg->x1;
	xMaxFP = seg->x0;
      }
      yMinFP = seg->y0;
      yMaxFP = seg->y1;
    } else {
      if (seg->x0 < xMinFP) {
	xMinFP = seg->x0;
      } else if (seg->x0 > xMaxFP) {
	xMaxFP = seg->x0;
      }
      if (seg->x1 < xMinFP) {
	xMinFP = seg->x1;
      } else if (seg->x1 > xMaxFP) {
	xMaxFP = seg->x1;
      }
      if (seg->y0 < yMinFP) {
	yMinFP = seg->y0;
      }
      if (seg->y1 > yMaxFP) {
	yMaxFP = seg->y1;
      }
    }
  }

  xMin = splashFloor(xMinFP);
  yMin = splashFloor(yMinFP);
  xMax = splashFloor(xMaxFP);
  yMax = splashFloor(yMaxFP);
}
