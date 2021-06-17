//========================================================================
//
// gmempp.cc
//
// Use gmalloc/gfree for C++ new/delete operators.
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include "gmem.h"
// NB: do not include gmempp.h here.

#ifdef DEBUG_MEM

void *operator new(size_t size) {
  return gmalloc((int)size, 1);
}

void *operator new(size_t size, int dummy) {
  return gmalloc((int)size, 0);
}

void *operator new[](size_t size) {
  return gmalloc((int)size, 1);
}

void *operator new[](size_t size, int dummy) {
  return gmalloc((int)size, 0);
}

void operator delete(void *p) {
  gfree(p);
}

// This is only called if a constructor throws an exception.
void operator delete(void *p, int dummy) {
  gfree(p);
}

void operator delete[](void *p) {
  gfree(p);
}

// This is only called if a constructor throws an exception.
void operator delete[](void *p, int dummy) {
  gfree(p);
}

#endif
