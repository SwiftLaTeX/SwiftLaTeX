//========================================================================
//
// UnicodeRemapping.cc
//
// Copyright 2018 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "gfile.h"
#include "GString.h"
#include "Error.h"
#include "UnicodeRemapping.h"

//------------------------------------------------------------------------

#define maxUnicodeString 8

struct UnicodeRemappingString {
  Unicode in;
  Unicode out[maxUnicodeString];
  int len;
};

//------------------------------------------------------------------------

static int hexCharVals[256] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 1x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 2x
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // 3x
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 4x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 5x
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 6x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 7x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 8x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 9x
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Ax
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Bx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Cx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Dx
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // Ex
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // Fx
};

// Parse a <len>-byte hex string <s> into *<val>.  Returns false on
// error.
static GBool parseHex(char *s, int len, Guint *val) {
  int i, x;

  *val = 0;
  for (i = 0; i < len; ++i) {
    x = hexCharVals[s[i] & 0xff];
    if (x < 0) {
      return gFalse;
    }
    *val = (*val << 4) + x;
  }
  return gTrue;
}

//------------------------------------------------------------------------

UnicodeRemapping::UnicodeRemapping() {
  for (int i = 0; i < 256; ++i) {
    page0[i] = (Unicode)i;
  }
  sMap = NULL;
  sMapLen = sMapSize = 0;
}

UnicodeRemapping::~UnicodeRemapping() {
  gfree(sMap);
}

void UnicodeRemapping::addRemapping(Unicode in, Unicode *out, int len) {
  int i;

  if (in < 256 && len == 1) {
    page0[in] = out[0];
  } else {
    if (in < 256) {
      page0[in] = 0xffffffff;
    }
    if (sMapLen == sMapSize) {
      sMapSize += 16;
      sMap = (UnicodeRemappingString *)
	         greallocn(sMap, sMapSize, sizeof(UnicodeRemappingString));
    }
    sMap[sMapLen].in = in;
    for (i = 0; i < len && i < maxUnicodeString; ++i) {
      sMap[sMapLen].out[i] = out[i];
    }
    sMap[sMapLen].len = i;
    ++sMapLen;
  }
}

void UnicodeRemapping::parseFile(GString *fileName) {
  FILE *f;
  char buf[256];
  Unicode in;
  Unicode out[maxUnicodeString];
  char *tok;
  int line, n;

  if (!(f = openFile(fileName->getCString(), "r"))) {
    error(errSyntaxError, -1, "Couldn't open unicodeRemapping file '{0:t}'",
	  fileName);
    return;
  }

  line = 0;
  while (getLine(buf, sizeof(buf), f)) {
    ++line;
    if (!(tok = strtok(buf, " \t\r\n")) ||
	!parseHex(tok, (int)strlen(tok), &in)) {
      error(errSyntaxWarning, -1,
	    "Bad line ({0:d}) in unicodeRemapping file '{1:t}'",
	    line, fileName);
      continue;
    }
    n = 0;
    while (n < maxUnicodeString) {
      if (!(tok = strtok(NULL, " \t\r\n"))) {
	break;
      }
      if (!parseHex(tok, (int)strlen(tok), &out[n])) {
	error(errSyntaxWarning, -1,
	      "Bad line ({0:d}) in unicodeRemapping file '{1:t}'",
	      line, fileName);
	break;
      }
      ++n;
    }
    addRemapping(in, out, n);
  }

  fclose(f);
}

int UnicodeRemapping::map(Unicode in, Unicode *out, int size) {
  int a, b, m, i;

  if (in < 256 && page0[in] != 0xffffffff) {
    out[0] = page0[in];
    return 1;
  }

  a = -1;
  b = sMapLen;
  // invariant: sMap[a].in < in < sMap[b].in
  while (b - a > 1) {
    m = (a + b) / 2;
    if (sMap[m].in < in) {
      a = m;
    } else if (in < sMap[m].in) {
      b = m;
    } else {
      for (i = 0; i < sMap[m].len && i < size; ++i) {
	out[i] = sMap[m].out[i];
      }
      return i;
    }
  }

  out[0] = in;
  return 1;
}
