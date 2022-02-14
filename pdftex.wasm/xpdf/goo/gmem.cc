/*
 * gmem.c
 *
 * Memory routines with out-of-memory checking.
 *
 * Copyright 1996-2003 Glyph & Cog, LLC
 */

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
// older compilers won't define SIZE_MAX in stdint.h without this
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS 1
#endif
#include <stdint.h>
#include <string.h>
#include <limits.h>
#if MULTITHREADED && defined(_WIN32)
#  include <windows.h>
#endif
#include "gmem.h"

#ifdef DEBUG_MEM

typedef struct _GMemHdr {
  unsigned int magic;
  int index;
  size_t size;
  struct _GMemHdr *next, *prev;
} GMemHdr;

#define gMemHdrSize ((sizeof(GMemHdr) + 7) & ~7)
#define gMemTrlSize (sizeof(long))

#define gMemMagic 0xabcd9999

#if ULONG_MAX > 0xffffffffL
#define gMemDeadVal 0xdeadbeefdeadbeefUL
#else
#define gMemDeadVal 0xdeadbeefUL
#endif

/* round data size so trailer will be aligned */
#define gMemDataSize(size) \
  (int)(((((size) + gMemTrlSize - 1) / gMemTrlSize) * gMemTrlSize))

#define gMemDataSize64(size) \
  (size_t)(((((size) + gMemTrlSize - 1) / gMemTrlSize) * gMemTrlSize))

static GMemHdr *gMemHead = NULL;
static GMemHdr *gMemTail = NULL;

static int gMemIndex = 0;
static int gMemAlloc = 0;
static size_t gMemInUse = 0;
static size_t gMaxMemInUse = 0;

#if MULTITHREADED
#  ifdef _WIN32
     static CRITICAL_SECTION gMemMutex;
     static INIT_ONCE gMemMutexInitStruct = INIT_ONCE_STATIC_INIT;
     static BOOL CALLBACK gMemMutexInitFunc(PINIT_ONCE initOnce, PVOID param,
					    PVOID *context) {
       InitializeCriticalSection(&gMemMutex);
       return TRUE;
     }
#    define gMemInitMutex InitOnceExecuteOnce(&gMemMutexInitStruct, \
                                              &gMemMutexInitFunc, NULL, NULL)
#    define gMemLock EnterCriticalSection(&gMemMutex);
#    define gMemUnlock LeaveCriticalSection(&gMemMutex);
#  else
#    include <pthread.h>
     static pthread_mutex_t gMemMutex = PTHREAD_MUTEX_INITIALIZER;
#    define gMemInitMutex
#    define gMemLock pthread_mutex_lock(&gMemMutex)
#    define gMemUnlock pthread_mutex_unlock(&gMemMutex)
#  endif
#else
#  define gMemInitMutex
#  define gMemLock
#  define gMemUnlock
#endif

#endif /* DEBUG_MEM */

#ifdef DEBUG_MEM
void *gmalloc(int size, int ignore) GMEM_EXCEP {
  int size1;
  char *mem;
  GMemHdr *hdr;
  void *data;
  unsigned long *trl, *p;

  gMemInitMutex;
  if (size < 0) {
    gMemError("Invalid memory allocation size");
  }
  if (size == 0) {
    return NULL;
  }
  size1 = gMemDataSize(size);
  if (!(mem = (char *)malloc(size1 + gMemHdrSize + gMemTrlSize))) {
    gMemError("Out of memory");
  }
  hdr = (GMemHdr *)mem;
  data = (void *)(mem + gMemHdrSize);
  trl = (unsigned long *)(mem + gMemHdrSize + size1);
  hdr->magic = gMemMagic;
  hdr->size = size;
  gMemLock;
  if (ignore) {
    hdr->index = -1;
  } else {
    hdr->index = gMemIndex++;
  }
  if (gMemTail) {
    gMemTail->next = hdr;
    hdr->prev = gMemTail;
    gMemTail = hdr;
  } else {
    hdr->prev = NULL;
    gMemHead = gMemTail = hdr;
  }
  hdr->next = NULL;
  ++gMemAlloc;
  gMemInUse += size;
  if (gMemInUse > gMaxMemInUse) {
    gMaxMemInUse = gMemInUse;
  }
  gMemUnlock;
  for (p = (unsigned long *)data; p <= trl; ++p) {
    *p = gMemDeadVal;
  }
  return data;
}
#else
void *gmalloc(int size) GMEM_EXCEP {
  void *p;

  if (size < 0) {
    gMemError("Invalid memory allocation size");
  }
  if (size == 0) {
    return NULL;
  }
  if (!(p = malloc(size))) {
    gMemError("Out of memory");
  }
  return p;
}
#endif

void *grealloc(void *p, int size) GMEM_EXCEP {
#ifdef DEBUG_MEM
  GMemHdr *hdr;
  void *q;
  int oldSize;

  if (size < 0) {
    gMemError("Invalid memory allocation size");
  }
  if (size == 0) {
    if (p) {
      gfree(p);
    }
    return NULL;
  }
  if (p) {
    hdr = (GMemHdr *)((char *)p - gMemHdrSize);
    oldSize = (int)hdr->size;
    q = gmalloc(size);
    memcpy(q, p, size < oldSize ? size : oldSize);
    gfree(p);
  } else {
    q = gmalloc(size);
  }
  return q;
#else
  void *q;

  if (size < 0) {
    gMemError("Invalid memory allocation size");
  }
  if (size == 0) {
    if (p) {
      free(p);
    }
    return NULL;
  }
  if (p) {
    q = realloc(p, size);
  } else {
    q = malloc(size);
  }
  if (!q) {
    gMemError("Out of memory");
  }
  return q;
#endif
}

void *gmallocn(int nObjs, int objSize) GMEM_EXCEP {
  int n;

  if (nObjs == 0) {
    return NULL;
  }
  n = nObjs * objSize;
  if (objSize <= 0 || nObjs < 0 || nObjs >= INT_MAX / objSize) {
    gMemError("Bogus memory allocation size");
  }
  return gmalloc(n);
}

#ifdef DEBUG_MEM
void *gmalloc64(size_t size, int ignore) GMEM_EXCEP {
  size_t size1;
  char *mem;
  GMemHdr *hdr;
  void *data;
  unsigned long *trl, *p;

  gMemInitMutex;
  if (size == 0) {
    return NULL;
  }
  size1 = gMemDataSize64(size);
  if (!(mem = (char *)malloc(size1 + gMemHdrSize + gMemTrlSize))) {
    gMemError("Out of memory");
  }
  hdr = (GMemHdr *)mem;
  data = (void *)(mem + gMemHdrSize);
  trl = (unsigned long *)(mem + gMemHdrSize + size1);
  hdr->magic = gMemMagic;
  hdr->size = size;
  gMemLock;
  if (ignore) {
    hdr->index = -1;
  } else {
    hdr->index = gMemIndex++;
  }
  if (gMemTail) {
    gMemTail->next = hdr;
    hdr->prev = gMemTail;
    gMemTail = hdr;
  } else {
    hdr->prev = NULL;
    gMemHead = gMemTail = hdr;
  }
  hdr->next = NULL;
  ++gMemAlloc;
  gMemInUse += size;
  if (gMemInUse > gMaxMemInUse) {
    gMaxMemInUse = gMemInUse;
  }
  gMemUnlock;
  for (p = (unsigned long *)data; p <= trl; ++p) {
    *p = gMemDeadVal;
  }
  return data;
}
#else
void *gmalloc64(size_t size) GMEM_EXCEP {
  void *p;

  if (size == 0) {
    return NULL;
  }
  if (!(p = malloc(size))) {
    gMemError("Out of memory");
  }
  return p;
}
#endif

void *gmallocn64(int nObjs, size_t objSize) GMEM_EXCEP {
  size_t n;

  if (nObjs == 0) {
    return NULL;
  }
  n = nObjs * objSize;
  if (nObjs < 0 || (size_t)nObjs >= SIZE_MAX / objSize) {
    gMemError("Bogus memory allocation size");
  }
  return gmalloc64(n);
}

void *greallocn(void *p, int nObjs, int objSize) GMEM_EXCEP {
  int n;

  if (nObjs == 0) {
    if (p) {
      gfree(p);
    }
    return NULL;
  }
  n = nObjs * objSize;
  if (objSize <= 0 || nObjs < 0 || nObjs >= INT_MAX / objSize) {
    gMemError("Bogus memory allocation size");
  }
  return grealloc(p, n);
}

void gfree(void *p) {
#ifdef DEBUG_MEM
  size_t size;
  GMemHdr *hdr;
  unsigned long *trl, *clr;

  if (p) {
    hdr = (GMemHdr *)((char *)p - gMemHdrSize);
    gMemLock;
    if (hdr->magic == gMemMagic &&
	((hdr->prev == NULL) == (hdr == gMemHead)) &&
	((hdr->next == NULL) == (hdr == gMemTail))) {
      if (hdr->prev) {
	hdr->prev->next = hdr->next;
      } else {
	gMemHead = hdr->next;
      }
      if (hdr->next) {
	hdr->next->prev = hdr->prev;
      } else {
	gMemTail = hdr->prev;
      }
      --gMemAlloc;
      gMemInUse -= hdr->size;
      gMemUnlock;
      size = gMemDataSize64(hdr->size);
      trl = (unsigned long *)((char *)hdr + gMemHdrSize + size);
      if (*trl != gMemDeadVal) {
	fprintf(stderr, "Overwrite past end of block %d at address %p\n",
		hdr->index, p);
      }
      for (clr = (unsigned long *)hdr; clr <= trl; ++clr) {
	*clr = gMemDeadVal;
      }
      free(hdr);
    } else {
      gMemUnlock;
      fprintf(stderr, "Attempted to free bad address %p\n", p);
    }
  }
#else
  if (p) {
    free(p);
  }
#endif
}

void gMemError(const char *msg) GMEM_EXCEP {
#if USE_EXCEPTIONS
  throw GMemException();
#else
  fprintf(stderr, "%s\n", msg);
  abort();
#endif
}

#ifdef DEBUG_MEM
void gMemReport(FILE *f) {
  GMemHdr *p;
  int left;

  fprintf(f, "%d memory allocations in all\n", gMemIndex);
  fprintf(f, "maximum memory in use: %zd bytes\n", gMaxMemInUse);
  left = 0;
  if (gMemAlloc > 0) {
    for (p = gMemHead; p; p = p->next) {
      if (p->index >= 0) {
	if (!left) {
	  fprintf(f, "%d memory blocks left allocated:\n", gMemAlloc);
	  fprintf(f, " index     size\n");
	  fprintf(f, "-------- --------\n");
	  left = 1;
	}
	fprintf(f, "%8d %8zd\n", p->index, p->size);
      }
    }
  }
  if (!left) {
    fprintf(f, "No memory blocks left allocated\n");
  }
}
#endif

char *copyString(const char *s) {
  char *s1;

  s1 = (char *)gmalloc((int)strlen(s) + 1);
  strcpy(s1, s);
  return s1;
}
