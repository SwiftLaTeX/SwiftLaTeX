//========================================================================
//
// Dict.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef DICT_H
#define DICT_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#if MULTITHREADED
#include "GMutex.h"
#endif
#include "Object.h"

struct DictEntry;

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

class Dict {
public:

  // Constructor.
  Dict(XRef *xrefA);

  // Destructor.
  ~Dict();

  // Reference counting.
#if MULTITHREADED
  long incRef() { return gAtomicIncrement(&ref); }
  long decRef() { return gAtomicDecrement(&ref); }
#else
  long incRef() { return ++ref; }
  long decRef() { return --ref; }
#endif

  // Get number of entries.
  int getLength() { return length; }

  // Add an entry.  NB: does not copy key.
  void add(char *key, Object *val);

  // Check if dictionary is of specified type.
  GBool is(const char *type);

  // Look up an entry and return the value.  Returns a null object
  // if <key> is not in the dictionary.
  Object *lookup(const char *key, Object *obj, int recursion = 0);
  Object *lookupNF(const char *key, Object *obj);

  // Iterative accessors.
  char *getKey(int i);
  Object *getVal(int i, Object *obj);
  Object *getValNF(int i, Object *obj);

  // Set the xref pointer.  This is only used in one special case: the
  // trailer dictionary, which is read before the xref table is
  // parsed.
  void setXRef(XRef *xrefA) { xref = xrefA; }

private:

  XRef *xref;			// the xref table for this PDF file
  DictEntry *entries;		// array of entries
  DictEntry **hashTab;		// hash table pointers
  int size;			// size of <entries> array
  int length;			// number of entries in dictionary
#if MULTITHREADED
  GAtomicCounter ref;		// reference count
#else
  long ref;			// reference count
#endif

  DictEntry *find(const char *key);
  void expand();
  int hash(const char *key);
};

#endif
