//========================================================================
//
// SplashMath.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHMATH_H
#define SPLASHMATH_H

#include <aconf.h>

#if USE_FIXEDPONT
#  include "FixedPoint.h"
#else
#  include <math.h>
#  if (defined(__GNUC__) && defined(__SSE2__)) || \
      (defined(_WIN32) && (_M_IX86_FP == 2 || defined(_M_X64)))
#    include <emmintrin.h>
#  endif
#endif
#include "SplashTypes.h"

static inline SplashCoord splashAbs(SplashCoord x) {
#if USE_FIXEDPOINT
  return FixedPoint::abs(x);
#else
  return fabs(x);
#endif
}

// floor() and (int)() are implemented separately, which results
// in changing the FPCW multiple times - so we optimize it with
// some inline assembly or SSE intrinsics.
static inline int splashFloor(SplashCoord x) {
#if USE_FIXEDPOINT

  //--- fixed point

  return FixedPoint::floor(x);

#elif (defined(__GNUC__) && defined(__SSE2__)) || \
      (defined(_WIN32) && (_M_IX86_FP == 2 || defined(_M_X64)))

  //--- SSE2 intrinsics
  // NB: 64-bit x86 guarantees availability of SSE2.

  __m128d m1, m2;
  int i, s;

  m1 = _mm_set_sd(x);
  i = _mm_cvttsd_si32(m1);
  m2 = _mm_cvtsi32_sd(m1, i);
  s = _mm_ucomigt_sd(m2, m1);
  return i - s;

#elif defined(__GNUC__) && defined(__i386__) && !defined(__APPLE__)

  //--- x87 inline assembly (gcc/clang)
  // (this code fails on OSX for reasons I don't understand)

  Gushort oldCW, newCW, t;
  int result;

  __asm__ volatile("fnstcw %0\n"
		   "movw   %0, %3\n"
		   "andw   $0xf3ff, %3\n"
		   "orw    $0x0400, %3\n"
		   "movw   %3, %1\n"       // round down
		   "fldcw  %1\n"
		   "fistl %2\n"
		   "fldcw  %0\n"
		   : "=m" (oldCW), "=m" (newCW), "=m" (result), "=r" (t)
		   : "t" (x));
  return result;

#elif defined(_WIN32) && defined(_M_IX86)

  //--- x87 inline assembly (VC)

  Gushort oldCW, newCW;
  int result;

  __asm fld QWORD PTR x
  __asm fnstcw WORD PTR oldCW
  __asm mov ax, WORD PTR oldCW
  __asm and ax, 0xf3ff
  __asm or ax, 0x0400
  __asm mov WORD PTR newCW, ax     // round down
  __asm fldcw WORD PTR newCW
  __asm fistp DWORD PTR result
  __asm fldcw WORD PTR oldCW
  return result;

#else

  //--- all others

  return (int)floor(x);

#endif
}

// ceil() and (int)() are implemented separately, which results
// in changing the FPCW multiple times - so we optimize it with
// some inline assembly or SSE intrinsics.
static inline int splashCeil(SplashCoord x) {
#if USE_FIXEDPOINT

  //--- fixed point

  return FixedPoint::ceil(x);

#elif (defined(__GNUC__) && defined(__SSE2__)) || \
      (defined(_WIN32) && (_M_IX86_FP == 2 || defined(_M_X64)))

  //--- SSE2 intrinsics
  // NB: 64-bit x86 guarantees availability of SSE2.

  __m128d m1, m2;
  int i, s;

  m1 = _mm_set_sd(x);
  i = _mm_cvttsd_si32(m1);
  m2 = _mm_cvtsi32_sd(m1, i);
  s = _mm_ucomilt_sd(m2, m1);
  return i + s;

#elif defined(__GNUC__) && defined(__i386__) && !defined(__APPLE__)

  //--- x87 inline assembly (gcc/clang)
  // (this code fails on OSX for reasons I don't understand)

  Gushort oldCW, newCW, t;
  int result;

  __asm__ volatile("fnstcw %0\n"
		   "movw   %0, %3\n"
		   "andw   $0xf3ff, %3\n"
		   "orw    $0x0800, %3\n"
		   "movw   %3, %1\n"       // round up
		   "fldcw  %1\n"
		   "fistl %2\n"
		   "fldcw  %0\n"
		   : "=m" (oldCW), "=m" (newCW), "=m" (result), "=r" (t)
		   : "t" (x));
  return result;

#elif defined(_WIN32) && defined(_M_IX86)

  //--- x87 inline assembly (VC)

  // ceil() and (int)() are implemented separately, which results
  // in changing the FPCW multiple times - so we optimize it with
  // some inline assembly
  Gushort oldCW, newCW;
  int result;

  __asm fld QWORD PTR x
  __asm fnstcw WORD PTR oldCW
  __asm mov ax, WORD PTR oldCW
  __asm and ax, 0xf3ff
  __asm or ax, 0x0800
  __asm mov WORD PTR newCW, ax     // round up
  __asm fldcw WORD PTR newCW
  __asm fistp DWORD PTR result
  __asm fldcw WORD PTR oldCW
  return result;

#else

  //--- all others

  return (int)ceil(x);

#endif
}

static inline int splashRound(SplashCoord x) {
#if USE_FIXEDPOINT

  //--- fixed point

  return FixedPoint::round(x);

#else

  //--- all others

  return splashFloor(x + 0.5);

#endif
}

static inline SplashCoord splashAvg(SplashCoord x, SplashCoord y) {
#if USE_FIXEDPOINT
  return FixedPoint::avg(x, y);
#else
  return 0.5 * (x + y);
#endif
}

static inline SplashCoord splashSqrt(SplashCoord x) {
#if USE_FIXEDPOINT
  return FixedPoint::sqrt(x);
#else
  return sqrt(x);
#endif
}

static inline SplashCoord splashPow(SplashCoord x, SplashCoord y) {
#if USE_FIXEDPOINT
  return FixedPoint::pow(x, y);
#else
  return pow(x, y);
#endif
}

static inline SplashCoord splashDist(SplashCoord x0, SplashCoord y0,
				     SplashCoord x1, SplashCoord y1) {
  SplashCoord dx, dy;
  dx = x1 - x0;
  dy = y1 - y0;
#if USE_FIXEDPOINT
  // this handles the situation where dx*dx or dy*dy is too large to
  // fit in the 16.16 fixed point format
  SplashCoord dxa, dya, d;
  dxa = splashAbs(dx);
  dya = splashAbs(dy);
  if (dxa == 0 && dya == 0) {
    return 0;
  } else if (dxa > dya) {
    d = dya / dxa;
    return dxa * FixedPoint::sqrt(d*d + 1);
  } else {
    d = dxa / dya;
    return dya * FixedPoint::sqrt(d*d + 1);
  }
#else
  return sqrt(dx * dx + dy * dy);
#endif
}

static inline GBool splashCheckDet(SplashCoord m11, SplashCoord m12,
				   SplashCoord m21, SplashCoord m22,
				   SplashCoord epsilon) {
#if USE_FIXEDPOINT
  return FixedPoint::checkDet(m11, m12, m21, m22, epsilon);
#else
  return fabs(m11 * m22 - m12 * m21) >= epsilon;
#endif
}

// Perform stroke adjustment on a SplashCoord range [xMin, xMax),
// resulting in an int range [*xMinI, *xMaxI).
//
// There are several options:
//
// 1. Round both edge coordinates.
//    Pro: adjacent strokes/fills line up without any gaps or
//         overlaps
//    Con: lines with the same original floating point width can
//         end up with different integer widths, e.g.:
//               xMin  = 10.1   xMax  = 11.3   (width = 1.2)
//           --> xMinI = 10     xMaxI = 11     (width = 1)
//         but
//               xMin  = 10.4   xMax  = 11.6   (width = 1.2)
//           --> xMinI = 10     xMaxI = 12     (width = 2)
//
// 2. Round the min coordinate; add the ceiling of the width.
//    Pro: lines with the same original floating point width will
//         always end up with the same integer width
//    Con: adjacent strokes/fills can have overlaps (which is
//         problematic with transparency)
//    (This could use floor on the min coordinate, instead of
//    rounding, with similar results.)
//    (If the width is rounded instead of using ceiling, the results
//    Are similar, except that adjacent strokes/fills can have gaps
//    as well as overlaps.)
//
// 3. Use floor on the min coordinate and ceiling on the max
//    coordinate.
//    Pro: lines always end up at least as wide as the original
//         floating point width
//    Con: adjacent strokes/fills can have overlaps, and lines with
//         the same original floating point width can end up with
//         different integer widths; the integer width can be more
//         than one pixel wider than the original width, e.g.:
//               xMin  = 10.9   xMax  = 12.1   (width = 1.2)
//           --> xMinI = 10     xMaxI = 13     (width = 3)
//         but
//               xMin  = 10.1   xMax  = 11.3   (width = 1.2)
//           --> xMinI = 10     xMaxI = 12     (width = 2)
//
// 4. Use a hybrid approach, choosing between two of the above
//    options, based on width.  E.g., use #2 if width <= 4, and use #1
//    if width > 4.
//
// If w >= 0 and strokeAdjMode is splashStrokeAdjustCAD then a special
// mode for projecting line caps is enabled, with w being the
// transformed line width.

static inline void splashStrokeAdjust(SplashCoord xMin, SplashCoord xMax,
				      int *xMinI, int *xMaxI,
				      SplashStrokeAdjustMode strokeAdjMode,
				      SplashCoord w = -1) {
  int x0, x1;

  // make sure the coords fit in 32-bit ints
#if USE_FIXEDPOINT
  if (xMin < -32767) {
    xMin = -32767;
  } else if (xMin > 32767) {
    xMin = 32767;
  }
  if (xMax < -32767) {
    xMax = -32767;
  } else if (xMax > 32767) {
    xMax = 32767;
  }
#else
  if (xMin < -1e9) {
    xMin = -1e9;
  } else if (xMin > 1e9) {
    xMin = 1e9;
  }
  if (xMax < -1e9) {
    xMax = -1e9;
  } else if (xMax > 1e9) {
    xMax = 1e9;
  }
#endif

  // this will never be called with strokeAdjMode == splashStrokeAdjustOff
  if (strokeAdjMode == splashStrokeAdjustCAD) {
    x0 = splashRound(xMin);
    if (w >= 0) {
      x1 = splashRound(xMax - w) + splashRound(w);
    } else {
      x1 = x0 + splashRound(xMax - xMin);
    }
  } else {
    // NB: enable exactly one of these.
#if 1 // 1. Round both edge coordinates.
    x0 = splashRound(xMin);
    x1 = splashRound(xMax);
#endif
#if 0 // 2. Round the min coordinate; add the ceiling of the width.
    x0 = splashRound(xMin);
    x1 = x0 + splashCeil(xMax - xMin);
#endif
#if 0 // 3. Use floor on the min coord and ceiling on the max coord.
    x0 = splashFloor(xMin);
    x1 = splashCeil(xMax);
#endif
#if 0 // 4. Hybrid.
    SplashCoord w = xMax - xMin;
    x0 = splashRound(xMin);
    if (w > 4) {
      x1 = splashRound(xMax);
    } else {
      x1 = x0 + splashRound(w);
    }
#endif
  }
  if (x0 == x1) {
    if (xMin + xMax < 2 * x0) {
      --x0;
    } else {
      ++x1;
    }
  }
  *xMinI = x0;
  *xMaxI = x1;
}

#endif
