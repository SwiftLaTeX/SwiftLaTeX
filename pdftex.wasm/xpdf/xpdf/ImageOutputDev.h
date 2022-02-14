//========================================================================
//
// ImageOutputDev.h
//
// Copyright 1998-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef IMAGEOUTPUTDEV_H
#define IMAGEOUTPUTDEV_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#include "gtypes.h"
#include "OutputDev.h"

class GfxImageColorMap;
class GfxState;

//------------------------------------------------------------------------
// ImageOutputDev
//------------------------------------------------------------------------

class ImageOutputDev: public OutputDev {
public:

  // Create an OutputDev which will write images to files named
  // <fileRoot>-NNN.<type>.  Normally, all images are written as PBM
  // (.pbm) or PPM (.ppm) files.  If <dumpJPEG> is set, JPEG images
  // are written as JPEG (.jpg) files.  If <dumpRaw> is set, all
  // images are written in PDF-native formats.  If <list> is set, a
  // one-line summary will be written to stdout for each image.
  ImageOutputDev(char *fileRootA, GBool dumpJPEGA, GBool dumpRawA,
		 GBool listA);

  // Destructor.
  virtual ~ImageOutputDev();

  // Check if file was successfully created.
  virtual GBool isOk() { return ok; }

  // Does this device use tilingPatternFill()?  If this returns false,
  // tiling pattern fills will be reduced to a series of other drawing
  // operations.
  virtual GBool useTilingPatternFill() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gFalse; }

  // Does this device need non-text content?
  virtual GBool needNonText() { return gTrue; }

  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gFalse; }

  //----- initialization and control
  virtual void startPage(int pageNum, GfxState *state);

  //----- path painting
  virtual void tilingPatternFill(GfxState *state, Gfx *gfx, Object *strRef,
				 int paintType, int tilingType, Dict *resDict,
				 double *mat, double *bbox,
				 int x0, int y0, int x1, int y1,
				 double xStep, double yStep);

  //----- image drawing
  virtual void drawImageMask(GfxState *state, Object *ref, Stream *str,
			     int width, int height, GBool invert,
			     GBool inlineImg, GBool interpolate);
  virtual void drawImage(GfxState *state, Object *ref, Stream *str,
			 int width, int height, GfxImageColorMap *colorMap,
			 int *maskColors, GBool inlineImg, GBool interpolate);
  virtual void drawMaskedImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       Stream *maskStr, int maskWidth, int maskHeight,
			       GBool maskInvert, GBool interpolate);
  virtual void drawSoftMaskedImage(GfxState *state, Object *ref, Stream *str,
				   int width, int height,
				   GfxImageColorMap *colorMap,
				   Stream *maskStr,
				   int maskWidth, int maskHeight,
				   GfxImageColorMap *maskColorMap,
				   double *matte, GBool interpolate);

private:

  Stream *getRawStream(Stream *str);
  const char *getRawFileExtension(Stream *str);
  void writeImageInfo(int width, int height, GfxState *state,
		      GfxImageColorMap *colorMap);

  char *fileRoot;		// root of output file names
  char *fileName;		// buffer for output file names
  GBool dumpJPEG;		// set to dump native JPEG files
  GBool dumpRaw;		// set to dump raw PDF-native image files
  GBool list;			// set to write image info to stdout
  int imgNum;			// current image number
  int curPageNum;		// current page number
  GBool ok;			// set up ok?
};

#endif
