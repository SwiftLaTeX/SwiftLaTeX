//========================================================================
//
// TextString.cc
//
// Copyright 2011-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "GString.h"
#include "PDFDocEncoding.h"
#include "UTF8.h"
#include "TextString.h"

//------------------------------------------------------------------------

TextString::TextString() {
  u = NULL;
  len = size = 0;
}

TextString::TextString(GString *s) {
  u = NULL;
  len = size = 0;
  append(s);
}

TextString::TextString(TextString *s) {
  len = size = s->len;
  if (len) {
    u = (Unicode *)gmallocn(size, sizeof(Unicode));
    memcpy(u, s->u, len * sizeof(Unicode));
  } else {
    u = NULL;
  }
}

TextString::~TextString() {
  gfree(u);
}

TextString *TextString::append(Unicode c) {
  expand(1);
  u[len] = c;
  ++len;
  return this;
}

TextString *TextString::append(GString *s) {
  return insert(len, s);
}

TextString *TextString::insert(int idx, Unicode c) {
  if (idx >= 0 && idx <= len) {
    expand(1);
    if (idx < len) {
      memmove(u + idx + 1, u + idx, (len - idx) * sizeof(Unicode));
    }
    u[idx] = c;
    ++len;
  }
  return this;
}

TextString *TextString::insert(int idx, Unicode *u2, int n) {
  if (idx >= 0 && idx <= len) {
    expand(n);
    if (idx < len) {
      memmove(u + idx + n, u + idx, (len - idx) * sizeof(Unicode));
    }
    memcpy(u + idx, u2, n * sizeof(Unicode));
    len += n;
  }
  return this;
}

TextString *TextString::insert(int idx, GString *s) {
  Unicode uBuf[100];
  int n, i;

  if (idx >= 0 && idx <= len) {
    // look for a UTF-16BE BOM
    if ((s->getChar(0) & 0xff) == 0xfe &&
	(s->getChar(1) & 0xff) == 0xff) {
      i = 2;
      n = 0;
      while (getUTF16BE(s, &i, uBuf + n)) {
	++n;
	if (n == sizeof(uBuf) / sizeof(Unicode)) {
	  insert(idx, uBuf, n);
	  idx += n;
	  n = 0;
	}
      }
      if (n > 0) {
	insert(idx, uBuf, n);
      }

    // look for a UTF-16LE BOM
    // (technically, this isn't allowed by the PDF spec, but some
    // PDF files use it)
    } else if ((s->getChar(0) & 0xff) == 0xff &&
	       (s->getChar(1) & 0xff) == 0xfe) {
      i = 2;
      n = 0;
      while (getUTF16LE(s, &i, uBuf + n)) {
	++n;
	if (n == sizeof(uBuf) / sizeof(Unicode)) {
	  insert(idx, uBuf, n);
	  idx += n;
	  n = 0;
	}
      }
      if (n > 0) {
	insert(idx, uBuf, n);
      }

    // look for a UTF-8 BOM
    } else if ((s->getChar(0) & 0xff) == 0xef &&
	       (s->getChar(1) & 0xff) == 0xbb &&
	       (s->getChar(2) & 0xff) == 0xbf) {
      i = 3;
      n = 0;
      while (getUTF8(s, &i, uBuf + n)) {
	++n;
	if (n == sizeof(uBuf) / sizeof(Unicode)) {
	  insert(idx, uBuf, n);
	  idx += n;
	  n = 0;
	}
      }
      if (n > 0) {
	insert(idx, uBuf, n);
      }

    // otherwise, use PDFDocEncoding
    } else {
      n = s->getLength();
      expand(n);
      if (idx < len) {
	memmove(u + idx + n, u + idx, (len - idx) * sizeof(Unicode));
      }
      for (i = 0; i < n; ++i) {
	u[idx + i] = pdfDocEncoding[s->getChar(i) & 0xff];
      }
      len += n;
    }
  }
  return this;
}

void TextString::expand(int delta) {
  int newLen;

  newLen = len + delta;
  if (delta > INT_MAX - len) {
    // trigger an out-of-memory error
    size = -1;
  } else if (newLen <= size) {
    return;
  } else if (size > 0 && size <= INT_MAX / 2 && size*2 >= newLen) {
    size *= 2;
  } else {
    size = newLen;
  }
  u = (Unicode *)greallocn(u, size, sizeof(Unicode));
}

GString *TextString::toPDFTextString() {
  GString *s;
  GBool useUnicode;
  int i;

  useUnicode = gFalse;
  for (i = 0; i < len; ++i) {
    if (u[i] >= 0x80) {
      useUnicode = gTrue;
      break;
    }
  }
  s = new GString();
  if (useUnicode) {
    s->append((char)0xfe);
    s->append((char)0xff);
    for (i = 0; i < len; ++i) {
      s->append((char)(u[i] >> 8));
      s->append((char)u[i]);
    }
  } else {
    for (i = 0; i < len; ++i) {
      s->append((char)u[i]);
    }
  }
  return s;
}
