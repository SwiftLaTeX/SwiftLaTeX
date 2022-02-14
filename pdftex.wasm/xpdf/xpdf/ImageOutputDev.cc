//========================================================================
//
// ImageOutputDev.cc
//
// Copyright 1998-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include "gmem.h"
#include "gmempp.h"
#include "config.h"
#include "Error.h"
#include "GfxState.h"
#include "Object.h"
#include "Stream.h"
#include "ImageOutputDev.h"

ImageOutputDev::ImageOutputDev(char *fileRootA, GBool dumpJPEGA,
			       GBool dumpRawA, GBool listA) {
  fileRoot = copyString(fileRootA);
  fileName = (char *)gmalloc((int)strlen(fileRoot) + 30);
  dumpJPEG = dumpJPEGA;
  dumpRaw = dumpRawA;
  list = listA;
  imgNum = 0;
  curPageNum = 0;
  ok = gTrue;
}

ImageOutputDev::~ImageOutputDev() {
  gfree(fileName);
  gfree(fileRoot);
}

void ImageOutputDev::startPage(int pageNum, GfxState *state) {
  curPageNum = pageNum;
}

void ImageOutputDev::tilingPatternFill(GfxState *state, Gfx *gfx,
				       Object *strRef,
				       int paintType, int tilingType,
				       Dict *resDict,
				       double *mat, double *bbox,
				       int x0, int y0, int x1, int y1,
				       double xStep, double yStep) {
  // do nothing -- this avoids the potentially slow loop in Gfx.cc
}

void ImageOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				   int width, int height, GBool invert,
				   GBool inlineImg, GBool interpolate) {
  FILE *f;
  char buf[4096];
  int size, n, i;

  // dump raw file
  if (dumpRaw && !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.%s",
	    fileRoot, imgNum, getRawFileExtension(str));
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = getRawStream(str);
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump JPEG file
  } else if (dumpJPEG && str->getKind() == strDCT && !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.jpg", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = ((DCTStream *)str)->getRawStream();
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump PBM file
  } else {

    // open the image file and write the PBM header
    sprintf(fileName, "%s-%04d.pbm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P4\n");
    fprintf(f, "%d %d\n", width, height);

    // initialize stream
    str->reset();

    // copy the stream
    size = height * ((width + 7) / 8);
    while (size > 0) {
      i = size < (int)sizeof(buf) ? size : (int)sizeof(buf);
      n = str->getBlock(buf, i);
      fwrite(buf, 1, n, f);
      if (n < i) {
	break;
      }
      size -= n;
    }

    str->close();
    fclose(f);
  }

  if (list) {
    writeImageInfo(width, height, state, NULL);
  }
}

void ImageOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       int *maskColors, GBool inlineImg,
			       GBool interpolate) {
  GfxColorSpaceMode csMode;
  FILE *f;
  ImageStream *imgStr;
  Guchar *p;
  GfxRGB rgb;
  GfxGray gray;
  int x, y;
  char buf[4096];
  int size, n, i, j;

  csMode = colorMap->getColorSpace()->getMode();
  if (csMode == csIndexed) {
    csMode = ((GfxIndexedColorSpace *)colorMap->getColorSpace())
             ->getBase()->getMode();
  }

  // dump raw file
  if (dumpRaw && !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.%s",
	    fileRoot, imgNum, getRawFileExtension(str));
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = getRawStream(str);
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump JPEG file
  } else if (dumpJPEG && str->getKind() == strDCT &&
	     (colorMap->getNumPixelComps() == 1 ||
	      colorMap->getNumPixelComps() == 3) &&
	     !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.jpg", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = ((DCTStream *)str)->getRawStream();
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump PBM file
  } else if (colorMap->getNumPixelComps() == 1 &&
	     colorMap->getBits() == 1) {

    // open the image file and write the PBM header
    sprintf(fileName, "%s-%04d.pbm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P4\n");
    fprintf(f, "%d %d\n", width, height);

    // initialize stream
    str->reset();

    // copy the stream
    size = height * ((width + 7) / 8);
    while (size > 0) {
      i = size < (int)sizeof(buf) ? size : (int)sizeof(buf);
      n = str->getBlock(buf, i);
      for (j = 0; j < n; ++j) {
	buf[j] = (char)(buf[j] ^ 0xff);
      }
      fwrite(buf, 1, n, f);
      if (n < i) {
	break;
      }
      size -= n;
    }

    str->close();
    fclose(f);

  // dump PGM file
  } else if (colorMap->getNumPixelComps() == 1 &&
	     (csMode == csDeviceGray || csMode == csCalGray)) {

    // open the image file and write the PGM header
    sprintf(fileName, "%s-%04d.pgm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P5\n");
    fprintf(f, "%d %d\n", width, height);
    fprintf(f, "255\n");

    // initialize stream
    imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			     colorMap->getBits());
    imgStr->reset();

    // for each line...
    for (y = 0; y < height; ++y) {

      // write the line
      if ((p = imgStr->getLine())) {
	for (x = 0; x < width; ++x) {
	  colorMap->getGray(p, &gray, state->getRenderingIntent());
	  fputc(colToByte(gray), f);
	  ++p;
	}
      } else {
	for (x = 0; x < width; ++x) {
	  fputc(0, f);
	}
      }
    }

    imgStr->close();
    delete imgStr;

    fclose(f);

  // dump PPM file
  } else {

    // open the image file and write the PPM header
    sprintf(fileName, "%s-%04d.ppm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P6\n");
    fprintf(f, "%d %d\n", width, height);
    fprintf(f, "255\n");

    // initialize stream
    imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			     colorMap->getBits());
    imgStr->reset();

    // for each line...
    for (y = 0; y < height; ++y) {

      // write the line
      if ((p = imgStr->getLine())) {
	for (x = 0; x < width; ++x) {
	  colorMap->getRGB(p, &rgb, state->getRenderingIntent());
	  fputc(colToByte(rgb.r), f);
	  fputc(colToByte(rgb.g), f);
	  fputc(colToByte(rgb.b), f);
	  p += colorMap->getNumPixelComps();
	}
      } else {
	for (x = 0; x < width; ++x) {
	  fputc(0, f);
	  fputc(0, f);
	  fputc(0, f);
	}
      }
    }

    imgStr->close();
    delete imgStr;

    fclose(f);
  }

  if (list) {
    writeImageInfo(width, height, state, colorMap);
  }
}

void ImageOutputDev::drawMaskedImage(GfxState *state, Object *ref, Stream *str,
				     int width, int height,
				     GfxImageColorMap *colorMap,
				     Stream *maskStr,
				     int maskWidth, int maskHeight,
				     GBool maskInvert, GBool interpolate) {
  drawImage(state, ref, str, width, height, colorMap,
	    NULL, gFalse, interpolate);
  drawImageMask(state, ref, maskStr, maskWidth, maskHeight, maskInvert,
		gFalse, interpolate);
}

void ImageOutputDev::drawSoftMaskedImage(GfxState *state, Object *ref,
					 Stream *str,
					 int width, int height,
					 GfxImageColorMap *colorMap,
					 Stream *maskStr,
					 int maskWidth, int maskHeight,
					 GfxImageColorMap *maskColorMap,
					 double *matte, GBool interpolate) {
  drawImage(state, ref, str, width, height, colorMap,
	    NULL, gFalse, interpolate);
  drawImage(state, ref, maskStr, maskWidth, maskHeight, maskColorMap,
	    NULL, gFalse, interpolate);
}

Stream *ImageOutputDev::getRawStream(Stream *str) {
  switch (str->getKind()) {
  case strLZW:
  case strRunLength:
  case strCCITTFax:
  case strDCT:
  case strFlate:
  case strJBIG2:
  case strJPX:
    return ((FilterStream *)str)->getNextStream();
  default:
    return str;
  }
}

const char *ImageOutputDev::getRawFileExtension(Stream *str) {
  switch (str->getKind()) {
  case strLZW:       return "lzw";
  case strRunLength: return "rle";
  case strCCITTFax:  return "fax";
  case strDCT:       return "jpg";
  case strFlate:     return "flate";
  case strJBIG2:     return "jb2";
  case strJPX:       return "jpx";
  default:           return "unknown";
  }
}

void ImageOutputDev::writeImageInfo(int width, int height, GfxState *state,
				    GfxImageColorMap *colorMap) {
  const char *mode;
  double hdpi, vdpi, x0, y0, x1, y1;
  int bpc;

  // this works for 0/90/180/270-degree rotations, along with
  // horizontal/vertical flips
  state->transformDelta(1, 0, &x0, &y0);
  state->transformDelta(0, 1, &x1, &y1);
  x0 = fabs(x0);
  y0 = fabs(y0);
  x1 = fabs(x1);
  y1 = fabs(y1);
  if (x0 > y0) {
    hdpi = (72 * width) / x0;
    vdpi = (72 * height) / y1;
  } else {
    hdpi = (72 * height) / x1;
    vdpi = (72 * width) / y0;
  }

  if (colorMap) {
    mode = GfxColorSpace::getColorSpaceModeName(
			      colorMap->getColorSpace()->getMode());
    bpc = colorMap->getBits();
  } else {
    mode = NULL;
    bpc = 1;
  }

  printf("%s: page=%d width=%d height=%d hdpi=%.2f vdpi=%.2f %s%s bpc=%d\n",
	 fileName, curPageNum, width, height, hdpi, vdpi,
	 mode ? "colorspace=" : "mask",
	 mode ? mode : "",
	 bpc);
}
