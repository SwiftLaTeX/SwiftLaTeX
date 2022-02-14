//========================================================================
//
// GMutex.h
//
// Portable mutex macros.
// Portable atomic increment/decrement.
//
// Copyright 2002-2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef GMUTEX_H
#define GMUTEX_H

#include <aconf.h>
#ifdef _WIN32
#  include <windows.h>
#  include <intrin.h>
#else
#  include <pthread.h>
#endif

//------------------------------------------------------------------------
// GMutex
//------------------------------------------------------------------------

// Usage:
//
// GMutex m;
// gInitMutex(&m);
// ...
// gLockMutex(&m);
//   ... critical section ...
// gUnlockMutex(&m);
// ...
// gDestroyMutex(&m);

#ifdef _WIN32

typedef CRITICAL_SECTION GMutex;

#define gInitMutex(m) InitializeCriticalSection(m)
#define gDestroyMutex(m) DeleteCriticalSection(m)
#define gLockMutex(m) EnterCriticalSection(m)
#define gUnlockMutex(m) LeaveCriticalSection(m)

#else // assume pthreads

typedef pthread_mutex_t GMutex;

#define gInitMutex(m) pthread_mutex_init(m, NULL)
#define gDestroyMutex(m) pthread_mutex_destroy(m)
#define gLockMutex(m) pthread_mutex_lock(m)
#define gUnlockMutex(m) pthread_mutex_unlock(m)

#endif

//------------------------------------------------------------------------
// atomic increment/decrement
//------------------------------------------------------------------------

// NB: this must be "long" to work on Windows
typedef long GAtomicCounter;

// Increment *counter by one and return the final value (after the
// increment).
static inline GAtomicCounter gAtomicIncrement(GAtomicCounter *counter) {
  GAtomicCounter newVal;

#if defined(_WIN32)
  newVal = _InterlockedIncrement(counter);
#elif defined(__GNUC__) || defined(__xlC__)
  // __GNUC__ also covers LLVM/clang
  newVal = __sync_add_and_fetch(counter, 1);
#elif defined(__SUNPRO_CC)
  newVal = atomic_inc_ulong_nv((ulong_t *)counter);
#else
#  error "gAtomicIncrement is not defined for this compiler/platform"
#endif
  return newVal;
}

// Decrement *counter by one and return the final value (after the
// decrement).
static inline GAtomicCounter gAtomicDecrement(GAtomicCounter *counter) {
  GAtomicCounter newVal;

#if defined(_WIN32)
  newVal = _InterlockedDecrement(counter);
#elif defined(__GNUC__) || defined(__xlC__)
  // __GNUC__ also covers LLVM/clang
  newVal = __sync_sub_and_fetch(counter, 1);
#elif defined(__SUNPRO_CC)
  newVal = atomic_dec_ulong_nv((ulong_t *)counter);
#else
#  error "gAtomicDecrement is not defined for this compiler/platform"
#endif
  return newVal;
}

#endif // GMUTEX_H
