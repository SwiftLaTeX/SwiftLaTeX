//========================================================================
//
// pdftopng.cc
//
// Copyright 2009 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#ifdef _WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include "gmem.h"
#include "gmempp.h"
#include "parseargs.h"
#include "GString.h"
#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#include "SplashBitmap.h"
#include "Splash.h"
#include "SplashOutputDev.h"
#include "config.h"

static int firstPage = 1;
static int lastPage = 0;
static double resolution = 150;
static GBool mono = gFalse;
static GBool gray = gFalse;
static GBool pngAlpha = gFalse;
static char enableFreeTypeStr[16] = "";
static char antialiasStr[16] = "";
static char vectorAntialiasStr[16] = "";
static char ownerPassword[33] = "";
static char userPassword[33] = "";
static GBool quiet = gFalse;
static char cfgFileName[256] = "";
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to print"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to print"},
  {"-r",      argFP,       &resolution,    0,
   "resolution, in DPI (default is 150)"},
  {"-mono",   argFlag,     &mono,          0,
   "generate a monochrome PNG file"},
  {"-gray",   argFlag,     &gray,          0,
   "generate a grayscale PNG file"},
  {"-alpha",  argFlag,     &pngAlpha,      0,
   "include an alpha channel in the PNG file"},
#if HAVE_FREETYPE_H
  {"-freetype",   argString,      enableFreeTypeStr, sizeof(enableFreeTypeStr),
   "enable FreeType font rasterizer: yes, no"},
#endif
  {"-aa",         argString,      antialiasStr,   sizeof(antialiasStr),
   "enable font anti-aliasing: yes, no"},
  {"-aaVector",   argString,      vectorAntialiasStr, sizeof(vectorAntialiasStr),
   "enable vector anti-aliasing: yes, no"},
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  {"-q",      argFlag,     &quiet,         0,
   "don't print any messages or errors"},
  {"-cfg",        argString,      cfgFileName,    sizeof(cfgFileName),
   "configuration file to use in place of .xpdfrc"},
  {"-v",      argFlag,     &printVersion,  0,
   "print copyright and version info"},
  {"-h",      argFlag,     &printHelp,     0,
   "print usage information"},
  {"-help",   argFlag,     &printHelp,     0,
   "print usage information"},
  {"--help",  argFlag,     &printHelp,     0,
   "print usage information"},
  {"-?",      argFlag,     &printHelp,     0,
   "print usage information"},
  {NULL}
};

static void setupPNG(png_structp *png, png_infop *pngInfo, FILE *f,
		     int bitDepth, int colorType, double res,
		     SplashBitmap *bitmap);
static void writePNGData(png_structp png, SplashBitmap *bitmap);
static void finishPNG(png_structp *png, png_infop *pngInfo);

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  char *fileName;
  char *pngRoot;
  GString *pngFile;
  GString *ownerPW, *userPW;
  SplashColor paperColor;
  SplashOutputDev *splashOut;
  GBool ok;
  int exitCode;
  int pg;
  png_structp png;
  png_infop pngInfo;
  FILE *f;

  exitCode = 99;

  // parse args
  fixCommandLine(&argc, &argv);
  ok = parseArgs(argDesc, &argc, argv);
  if (mono && gray) {
    fprintf(stderr, "Specify one of -mono or -gray\n");
    goto err0;
  }
  if (mono && pngAlpha) {
    fprintf(stderr, "The -alpha flag cannot be used with -mono\n");
    goto err0;
  }
  if (!ok || argc != 3 || printVersion || printHelp) {
    fprintf(stderr, "pdftopng version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdftopng", "<PDF-file> <PNG-root>", argDesc);
    }
    goto err0;
  }
  fileName = argv[1];
  pngRoot = argv[2];

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  globalParams->setupBaseFonts(NULL);
  if (enableFreeTypeStr[0]) {
    if (!globalParams->setEnableFreeType(enableFreeTypeStr)) {
      fprintf(stderr, "Bad '-freetype' value on command line\n");
    }
  }
  if (antialiasStr[0]) {
    if (!globalParams->setAntialias(antialiasStr)) {
      fprintf(stderr, "Bad '-aa' value on command line\n");
    }
  }
  if (vectorAntialiasStr[0]) {
    if (!globalParams->setVectorAntialias(vectorAntialiasStr)) {
      fprintf(stderr, "Bad '-aaVector' value on command line\n");
    }
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // open PDF file
  if (ownerPassword[0]) {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0]) {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err1;
  }

  // get page range
  if (firstPage < 1)
    firstPage = 1;
  if (lastPage < 1 || lastPage > doc->getNumPages())
    lastPage = doc->getNumPages();


  // write PNG files
  if (mono) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono1, 1, gFalse, paperColor);
  } else if (gray) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono8, 1, gFalse, paperColor);
  } else {
    paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
    splashOut = new SplashOutputDev(splashModeRGB8, 1, gFalse, paperColor);
  }
  if (pngAlpha) {
    splashOut->setNoComposite(gTrue);
  }
  splashOut->startDoc(doc->getXRef());
  for (pg = firstPage; pg <= lastPage; ++pg) {
    doc->displayPage(splashOut, pg, resolution, resolution, 0,
		     gFalse, gTrue, gFalse);
    if (mono) {
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
#ifdef _WIN32
	_setmode(_fileno(f), _O_BINARY);
#endif
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       1, PNG_COLOR_TYPE_GRAY, resolution, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    } else if (gray) {
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
#ifdef _WIN32
	_setmode(_fileno(f), _O_BINARY);
#endif
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       8, pngAlpha ? PNG_COLOR_TYPE_GRAY_ALPHA : PNG_COLOR_TYPE_GRAY,
	       resolution, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    } else { // RGB
      if (!strcmp(pngRoot, "-")) {
	f = stdout;
#ifdef _WIN32
	_setmode(_fileno(f), _O_BINARY);
#endif
      } else {
	pngFile = GString::format("{0:s}-{1:06d}.png", pngRoot, pg);
	if (!(f = fopen(pngFile->getCString(), "wb"))) {
	  exit(2);
	}
	delete pngFile;
      }
      setupPNG(&png, &pngInfo, f,
	       8, pngAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
	       resolution, splashOut->getBitmap());
      writePNGData(png, splashOut->getBitmap());
      finishPNG(&png, &pngInfo);
      fclose(f);
    }
  }
  delete splashOut;

  exitCode = 0;

  // clean up
 err1:
  delete doc;
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}

static void setupPNG(png_structp *png, png_infop *pngInfo, FILE *f,
		     int bitDepth, int colorType, double res,
		     SplashBitmap *bitmap) {
  png_color_16 background;
  int pixelsPerMeter;

  if (!(*png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				       NULL, NULL, NULL)) ||
      !(*pngInfo = png_create_info_struct(*png))) {
    exit(2);
  }
  if (setjmp(png_jmpbuf(*png))) {
    exit(2);
  }
  png_init_io(*png, f);
  png_set_IHDR(*png, *pngInfo, bitmap->getWidth(), bitmap->getHeight(),
	       bitDepth, colorType, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA ||
      colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
    background.index = 0;
    background.red = 0xff;
    background.green = 0xff;
    background.blue = 0xff;
    background.gray = 0xff;
    png_set_bKGD(*png, *pngInfo, &background);
  }
  pixelsPerMeter = (int)(res * (1000 / 25.4) + 0.5);
  png_set_pHYs(*png, *pngInfo, pixelsPerMeter, pixelsPerMeter,
	       PNG_RESOLUTION_METER);
  png_write_info(*png, *pngInfo);
}

static void writePNGData(png_structp png, SplashBitmap *bitmap) {
  Guchar *p, *alpha, *rowBuf, *rowBufPtr;
  int y, x;

  if (setjmp(png_jmpbuf(png))) {
    exit(2);
  }
  p = bitmap->getDataPtr();
  if (pngAlpha) {
    alpha = bitmap->getAlphaPtr();
    if (bitmap->getMode() == splashModeMono8) {
      rowBuf = (Guchar *)gmallocn(bitmap->getWidth(), 2);
      for (y = 0; y < bitmap->getHeight(); ++y) {
	rowBufPtr = rowBuf;
	for (x = 0; x < bitmap->getWidth(); ++x) {
	  *rowBufPtr++ = *p++;
	  *rowBufPtr++ = *alpha++;
	}
	png_write_row(png, (png_bytep)rowBuf);
      }
      gfree(rowBuf);
    } else { // splashModeRGB8
      rowBuf = (Guchar *)gmallocn(bitmap->getWidth(), 4);
      for (y = 0; y < bitmap->getHeight(); ++y) {
	rowBufPtr = rowBuf;
	for (x = 0; x < bitmap->getWidth(); ++x) {
	  *rowBufPtr++ = *p++;
	  *rowBufPtr++ = *p++;
	  *rowBufPtr++ = *p++;
	  *rowBufPtr++ = *alpha++;
	}
	png_write_row(png, (png_bytep)rowBuf);
      }
      gfree(rowBuf);
    }
  } else {
    for (y = 0; y < bitmap->getHeight(); ++y) {
      png_write_row(png, (png_bytep)p);
      p += bitmap->getRowSize();
    }
  }
}




static void finishPNG(png_structp *png, png_infop *pngInfo) {
  if (setjmp(png_jmpbuf(*png))) {
    exit(2);
  }
  png_write_end(*png, *pngInfo);
  png_destroy_write_struct(png, pngInfo);
}
