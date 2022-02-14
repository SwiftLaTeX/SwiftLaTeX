//========================================================================
//
// SplashFontFile.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#ifndef _WIN32
#  include <unistd.h>
#endif
#include "gmempp.h"
#include "GString.h"
#include "SplashFontFile.h"
#include "SplashFontFileID.h"

#ifdef VMS
#if (__VMS_VER < 70000000)
extern "C" int unlink(char *filename);
#endif
#endif

//------------------------------------------------------------------------
// SplashFontFile
//------------------------------------------------------------------------

SplashFontFile::SplashFontFile(SplashFontFileID *idA,
			       SplashFontType fontTypeA,
#if LOAD_FONTS_FROM_MEM
			       GString *fontBufA
#else
			       char *fileNameA, GBool deleteFileA
#endif
			       ) {
  id = idA;
  fontType = fontTypeA;
#if LOAD_FONTS_FROM_MEM
  fontBuf = fontBufA;
#else
  fileName = new GString(fileNameA);
  deleteFile = deleteFileA;
#endif
  refCnt = 0;
}

SplashFontFile::~SplashFontFile() {
#if LOAD_FONTS_FROM_MEM
  delete fontBuf;
#else
  if (deleteFile) {
    unlink(fileName->getCString());
  }
  delete fileName;
#endif
  delete id;
}

void SplashFontFile::incRefCnt() {
#if MULTITHREADED
  gAtomicIncrement(&refCnt);
#else
  ++refCnt;
#endif
}

void SplashFontFile::decRefCnt() {
  GBool done;

#if MULTITHREADED
  done = gAtomicDecrement(&refCnt) == 0;
#else
  done = --refCnt == 0;
#endif
  if (done) {
    delete this;
  }
}
