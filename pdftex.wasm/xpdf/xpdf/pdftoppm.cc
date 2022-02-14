//========================================================================
//
// pdftoppm.cc
//
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#ifdef _WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#ifdef DEBUG_FP_LINUX
#  include <fenv.h>
#  include <fpu_control.h>
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
#if SPLASH_CMYK
static GBool cmyk = gFalse;
#endif
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
   "generate a monochrome PBM file"},
  {"-gray",   argFlag,     &gray,          0,
   "generate a grayscale PGM file"},
#if SPLASH_CMYK
  {"-cmyk",   argFlag,     &cmyk,          0,
   "generate a CMYK PAM file"},
#endif
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

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  char *fileName;
  char *ppmRoot;
  GString *ppmFile;
  GString *ownerPW, *userPW;
  SplashColor paperColor;
  SplashOutputDev *splashOut;
  GBool ok;
  int exitCode;
  int pg, n;
  const char *ext;

#ifdef DEBUG_FP_LINUX
  // enable exceptions on floating point div-by-zero
  feenableexcept(FE_DIVBYZERO);
  // force 64-bit rounding: this avoids changes in output when minor
  // code changes result in spills of x87 registers; it also avoids
  // differences in output with valgrind's 64-bit floating point
  // emulation (yes, this is a kludge; but it's pretty much
  // unavoidable given the x87 instruction set; see gcc bug 323 for
  // more info)
  fpu_control_t cw;
  _FPU_GETCW(cw);
  cw = (fpu_control_t)((cw & ~_FPU_EXTENDED) | _FPU_DOUBLE);
  _FPU_SETCW(cw);
#endif

  exitCode = 99;

  // parse args
  fixCommandLine(&argc, &argv);
  ok = parseArgs(argDesc, &argc, argv);
  n = 0;
  n += mono ? 1 : 0;
  n += gray ? 1 : 0;
#if SPLASH_CMYK
  n += cmyk ? 1 : 0;
#endif
  if (n > 1) {
    ok = gFalse;
  }
  if (!ok || argc != 3 || printVersion || printHelp) {
    fprintf(stderr, "pdftoppm version %s\n", xpdfVersion);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdftoppm", "<PDF-file> <PPM-root>", argDesc);
    }
    goto err0;
  }
  fileName = argv[1];
  ppmRoot = argv[2];

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

  // file name extension
  if (mono) {
    ext = "pbm";
  } else if (gray) {
    ext = "pgm";
#if SPLASH_CMYK
  } else if (cmyk) {
    ext = "pam";
#endif
  } else {
    ext = "ppm";
  }


  // write PPM files
  if (mono) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono1, 1, gFalse, paperColor);
  } else if (gray) {
    paperColor[0] = 0xff;
    splashOut = new SplashOutputDev(splashModeMono8, 1, gFalse, paperColor);
#if SPLASH_CMYK
  } else if (cmyk) {
    paperColor[0] = paperColor[1] = paperColor[2] = paperColor[3] = 0;
    splashOut = new SplashOutputDev(splashModeCMYK8, 1, gFalse, paperColor);
#endif // SPLASH_CMYK
  } else {
    paperColor[0] = paperColor[1] = paperColor[2] = 0xff;
    splashOut = new SplashOutputDev(splashModeRGB8, 1, gFalse, paperColor);
  }
  splashOut->startDoc(doc->getXRef());
  for (pg = firstPage; pg <= lastPage; ++pg) {
    doc->displayPage(splashOut, pg, resolution, resolution, 0,
		     gFalse, gTrue, gFalse);
    if (!strcmp(ppmRoot, "-")) {
#ifdef _WIN32
      _setmode(_fileno(stdout), _O_BINARY);
#endif
      splashOut->getBitmap()->writePNMFile(stdout);
    } else {
      ppmFile = GString::format("{0:s}-{1:06d}.{2:s}", ppmRoot, pg, ext);
      splashOut->getBitmap()->writePNMFile(ppmFile->getCString());
      delete ppmFile;
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
