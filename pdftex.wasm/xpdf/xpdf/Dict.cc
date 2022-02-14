//========================================================================
//
// Dict.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include "gmem.h"
#include "gmempp.h"
#include "Object.h"
#include "XRef.h"
#include "Dict.h"

//------------------------------------------------------------------------

struct DictEntry {
  char *key;
  Object val;
  DictEntry *next;
};

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

Dict::Dict(XRef *xrefA) {
  xref = xrefA;
  size = 8;
  length = 0;
  entries = (DictEntry *)gmallocn(size, sizeof(DictEntry));
  hashTab = (DictEntry **)gmallocn(2 * size - 1, sizeof(DictEntry *));
  memset(hashTab, 0, (2 * size - 1) * sizeof(DictEntry *));
  ref = 1;
}

Dict::~Dict() {
  int i;

  for (i = 0; i < length; ++i) {
    gfree(entries[i].key);
    entries[i].val.free();
  }
  gfree(entries);
  gfree(hashTab);
}

void Dict::add(char *key, Object *val) {
  DictEntry *e;
  int h;

  if ((e = find(key))) {
    e->val.free();
    e->val = *val;
    gfree(key);
  } else {
    if (length == size) {
      expand();
    }
    h = hash(key);
    entries[length].key = key;
    entries[length].val = *val;
    entries[length].next = hashTab[h];
    hashTab[h] = &entries[length];
    ++length;
  }
}

void Dict::expand() {
  int h, i;

  size *= 2;
  entries = (DictEntry *)greallocn(entries, size, sizeof(DictEntry));
  hashTab = (DictEntry **)greallocn(hashTab, 2 * size - 1,
				    sizeof(DictEntry *));
  memset(hashTab, 0, (2 * size - 1) * sizeof(DictEntry *));
  for (i = 0; i < length; ++i) {
    h = hash(entries[i].key);
    entries[i].next = hashTab[h];
    hashTab[h] = &entries[i];
  }
}

inline DictEntry *Dict::find(const char *key) {
  DictEntry *e;
  int h;

  h = hash(key);
  for (e = hashTab[h]; e; e = e->next) {
    if (!strcmp(key, e->key)) {
      return e;
    }
  }
  return NULL;
}

int Dict::hash(const char *key) {
  const char *p;
  unsigned int h;

  h = 0;
  for (p = key; *p; ++p) {
    h = 17 * h + (int)(*p & 0xff);
  }
  return (int)(h % (2 * size - 1));
}

GBool Dict::is(const char *type) {
  DictEntry *e;

  return (e = find("Type")) && e->val.isName(type);
}

Object *Dict::lookup(const char *key, Object *obj, int recursion) {
  DictEntry *e;

  return (e = find(key)) ? e->val.fetch(xref, obj, recursion)
                         : obj->initNull();
}

Object *Dict::lookupNF(const char *key, Object *obj) {
  DictEntry *e;

  return (e = find(key)) ? e->val.copy(obj) : obj->initNull();
}

char *Dict::getKey(int i) {
  return entries[i].key;
}

Object *Dict::getVal(int i, Object *obj) {
  return entries[i].val.fetch(xref, obj);
}

Object *Dict::getValNF(int i, Object *obj) {
  return entries[i].val.copy(obj);
}
