//========================================================================
//
// SplashBitmap.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <limits.h>
#include "gmem.h"
#include "gmempp.h"
#include "SplashErrorCodes.h"
#include "SplashBitmap.h"

//------------------------------------------------------------------------
// SplashBitmap
//------------------------------------------------------------------------

SplashBitmap::SplashBitmap(int widthA, int heightA, int rowPad,
			   SplashColorMode modeA, GBool alphaA,
			   GBool topDown, SplashBitmap *parentA) {
  // NB: this code checks that rowSize fits in a signed 32-bit
  // integer, because some code (outside this class) makes that
  // assumption
  width = widthA;
  height = heightA;
  mode = modeA;
  switch (mode) {
  case splashModeMono1:
    if (width <= 0) {
      gMemError("invalid bitmap width");
    }
    rowSize = (width + 7) >> 3;
    break;
  case splashModeMono8:
    if (width <= 0) {
      gMemError("invalid bitmap width");
    }
    rowSize = width;
    break;
  case splashModeRGB8:
  case splashModeBGR8:
    if (width <= 0 || width > INT_MAX / 3) {
      gMemError("invalid bitmap width");
    }
    rowSize = (SplashBitmapRowSize)width * 3;
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    if (width <= 0 || width > INT_MAX / 4) {
      gMemError("invalid bitmap width");
    }
    rowSize = (SplashBitmapRowSize)width * 4;
    break;
#endif
  }
  rowSize += rowPad - 1;
  rowSize -= rowSize % rowPad;

  parent = parentA;
  oldData = NULL;
  oldAlpha = NULL;
  oldRowSize = 0;
  oldAlphaRowSize = 0;
  oldHeight = 0;
  if (parent && parent->oldData &&
      parent->oldRowSize == rowSize &&
      parent->oldHeight == height) {
    data = parent->oldData;
    parent->oldData = NULL;
  } else {
    data = (SplashColorPtr)gmallocn64(height, rowSize);
  }
  if (!topDown) {
    data += (height - 1) * rowSize;
    rowSize = -rowSize;
  }
  if (alphaA) {
    alphaRowSize = width;
    if (parent && parent->oldAlpha &&
	parent->oldAlphaRowSize == alphaRowSize &&
	parent->oldHeight == height) {
      alpha = parent->oldAlpha;
      parent->oldAlpha = NULL;
    } else {
      alpha = (Guchar *)gmallocn64(height, alphaRowSize);
    }
  } else {
    alphaRowSize = 0;
    alpha = NULL;
  }
}

SplashBitmap::~SplashBitmap() {
  if (data && rowSize < 0) {
    rowSize = -rowSize;
    data -= (height - 1) * rowSize;
  }
  if (parent && rowSize > 10000000 / height) {
    gfree(parent->oldData);
    gfree(parent->oldAlpha);
    parent->oldData = data;
    parent->oldAlpha = alpha;
    parent->oldRowSize = rowSize;
    parent->oldAlphaRowSize = alphaRowSize;
    parent->oldHeight = height;
  } else {
    gfree(data);
    gfree(alpha);
  }
  gfree(oldData);
  gfree(oldAlpha);
}

SplashError SplashBitmap::writePNMFile(char *fileName) {
  FILE *f;
  SplashError err;

  if (!(f = fopen(fileName, "wb"))) {
    return splashErrOpenFile;
  }
  err = writePNMFile(f);
  fclose(f);
  return err;
}

SplashError SplashBitmap::writePNMFile(FILE *f) {
  SplashColorPtr row, p;
  int x, y;

  switch (mode) {

  case splashModeMono1:
    fprintf(f, "P4\n%d %d\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      p = row;
      for (x = 0; x < width; x += 8) {
	fputc(*p ^ 0xff, f);
	++p;
      }
      row += rowSize;
    }
    break;

  case splashModeMono8:
    fprintf(f, "P5\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, width, f);
      row += rowSize;
    }
    break;

  case splashModeRGB8:
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, 3 * width, f);
      row += rowSize;
    }
    break;

  case splashModeBGR8:
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    row = data;
    for (y = 0; y < height; ++y) {
      p = row;
      for (x = 0; x < width; ++x) {
	fputc(splashBGR8R(p), f);
	fputc(splashBGR8G(p), f);
	fputc(splashBGR8B(p), f);
	p += 3;
      }
      row += rowSize;
    }
    break;

#if SPLASH_CMYK
  case splashModeCMYK8:
    fprintf(f, "P7\n");
    fprintf(f, "WIDTH %d\n", width);
    fprintf(f, "HEIGHT %d\n", height);
    fprintf(f, "DEPTH 4\n");
    fprintf(f, "MAXVAL 255\n");
    fprintf(f, "TUPLTYPE CMYK\n");
    fprintf(f, "ENDHDR\n");
    row = data;
    for (y = 0; y < height; ++y) {
      fwrite(row, 1, 4 * width, f);
      row += rowSize;
    }
    break;
#endif

  }

  return splashOk;
}

SplashError SplashBitmap::writeAlphaPGMFile(char *fileName) {
  FILE *f;

  if (!alpha) {
    return splashErrModeMismatch;
  }
  if (!(f = fopen(fileName, "wb"))) {
    return splashErrOpenFile;
  }
  fprintf(f, "P5\n%d %d\n255\n", width, height);
  fwrite(alpha, 1, width * height, f);
  fclose(f);
  return splashOk;
}


void SplashBitmap::getPixel(int x, int y, SplashColorPtr pixel) {
  SplashColorPtr p;

  if (y < 0 || y >= height || x < 0 || x >= width) {
    return;
  }
  switch (mode) {
  case splashModeMono1:
    p = &data[y * rowSize + (x >> 3)];
    pixel[0] = (p[0] & (0x80 >> (x & 7))) ? 0xff : 0x00;
    break;
  case splashModeMono8:
    p = &data[y * rowSize + x];
    pixel[0] = p[0];
    break;
  case splashModeRGB8:
    p = &data[y * rowSize + 3 * x];
    pixel[0] = p[0];
    pixel[1] = p[1];
    pixel[2] = p[2];
    break;
  case splashModeBGR8:
    p = &data[y * rowSize + 3 * x];
    pixel[0] = p[2];
    pixel[1] = p[1];
    pixel[2] = p[0];
    break;
#if SPLASH_CMYK
  case splashModeCMYK8:
    p = &data[y * rowSize + 4 * x];
    pixel[0] = p[0];
    pixel[1] = p[1];
    pixel[2] = p[2];
    pixel[3] = p[3];
    break;
#endif
  }
}

Guchar SplashBitmap::getAlpha(int x, int y) {
  return alpha[y * (size_t)width + x];
}

SplashColorPtr SplashBitmap::takeData() {
  SplashColorPtr data2;

  data2 = data;
  data = NULL;
  return data2;
}

