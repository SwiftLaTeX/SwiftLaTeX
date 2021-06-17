//========================================================================
//
// TileCache.cc
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include "gmem.h"
#include "gmempp.h"
#include "GList.h"
#include "GMutex.h"
#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#  include <unistd.h>
#endif
#include "Object.h"
#include "PDFDoc.h"
#include "SplashBitmap.h"
#include "SplashOutputDev.h"
#include "DisplayState.h"
#include "GfxState.h"
#include "TileMap.h"
#include "TileCache.h"

//------------------------------------------------------------------------
// CachedTileDesc
//------------------------------------------------------------------------

enum CachedTileState {
  cachedTileUnstarted,		// worker thread hasn't started
				//   rasterization yet
  cachedTileStarted,		// worker thread is rasterizing the tile
  cachedTileFinished,		// rasterization is done
  cachedTileCanceled		// worker thread should stop rasterizing
				//   and remove this tile from the cache
};

class CachedTileDesc: public TileDesc {
public:

  CachedTileDesc(TileDesc *tile):
    TileDesc(tile->page, tile->rotate, tile->dpi,
	     tile->tx, tile->ty, tile->tw, tile->th),
    state(cachedTileUnstarted), active(gTrue),
    bitmap(NULL), freeBitmap(gFalse) {}
  ~CachedTileDesc();

  CachedTileState state;
  GBool active;
  SplashBitmap *bitmap;
  GBool freeBitmap;
};

CachedTileDesc::~CachedTileDesc() {
  if (freeBitmap) {
    delete bitmap;
  }
}

//------------------------------------------------------------------------
// OS-dependent threading support code
//
// NB: This wrapper code is not meant to be general purpose.  Pthreads
// condition objects are not equivalent to Windows event objects, in
// general.
//------------------------------------------------------------------------

//-------------------- Windows --------------------
#ifdef _WIN32

typedef HANDLE GThreadID;
typedef DWORD (WINAPI *GThreadFunc)(void *);
#define GThreadReturn DWORD WINAPI

static void gCreateThread(GThreadID *thr, GThreadFunc threadFunc,
			  void *data) {
  *thr = CreateThread(NULL, 0, threadFunc, data, 0, NULL);
}
   
static void gJoinThread(GThreadID thr) {
  WaitForSingleObject(thr, INFINITE);
  CloseHandle(thr);
}

typedef HANDLE GCondition;

static void gInitCondition(GCondition *c) {
  *c = CreateEvent(NULL, TRUE, FALSE, NULL);
}

static void gDestroyCondition(GCondition *c) {
  CloseHandle(*c);
}

static void gSignalCondition(GCondition *c) {
  SetEvent(*c);
}

static void gClearCondition(GCondition *c) {
  ResetEvent(*c);
}

static void gWaitCondition(GCondition *c, GMutex *m) {
  LeaveCriticalSection(m);
  WaitForSingleObject(*c, INFINITE);
  EnterCriticalSection(m);
}

//-------------------- pthreads --------------------
#else

typedef pthread_t GThreadID;
typedef void *(*GThreadFunc)(void *);
#define GThreadReturn void*

static void gCreateThread(GThreadID *thr, GThreadFunc threadFunc,
			  void *data) {
  pthread_create(thr, NULL, threadFunc, data);
}

static void gJoinThread(GThreadID thr) {
  pthread_join(thr, NULL);
}

typedef pthread_cond_t GCondition;

static void gInitCondition(GCondition *c) {
  pthread_cond_init(c, NULL);
}

static void gDestroyCondition(GCondition *c) {
  pthread_cond_destroy(c);
}

static void gSignalCondition(GCondition *c) {
  pthread_cond_broadcast(c);
}

static void gClearCondition(GCondition *c) {
}

static void gWaitCondition(GCondition *c, GMutex *m) {
  pthread_cond_wait(c, m);
}

#endif

//------------------------------------------------------------------------
// TileCacheThreadPool
//------------------------------------------------------------------------

class TileCacheThreadPool {
public:

  TileCacheThreadPool(TileCache *tileCacheA, int nThreadsA);
  ~TileCacheThreadPool();

  // Called by the client when one or more jobs have been added to the
  // queue.  This must be called with the mutex unlocked.
  void jobAdded();

  // Wait for at least one job to be finished.  This must be called
  // with the mutex locked.
  void waitForFinishedJob();

  // The client should use the mutex to protect the queue data
  // structure.
  void lockMutex() { gLockMutex(&mutex); }
  void unlockMutex() { gUnlockMutex(&mutex); }

private:

  static GThreadReturn threadFunc(void *arg);
  void worker();

  TileCache *tileCache;
  int nThreads;
  GThreadID *threads;
  GBool quit;
  GMutex mutex;
  GCondition cond;		// signalled when a job is added to the
				//   queue and when the quit flag is set
  GCondition finishCond;	// signalled whenever a worker thread
				//   finishes rasterizing a tile
};

TileCacheThreadPool::TileCacheThreadPool(TileCache *tileCacheA,
					 int nThreadsA) {
  int i;

  tileCache = tileCacheA;
  nThreads = nThreadsA;
  quit = gFalse;
  gInitMutex(&mutex);
  gInitCondition(&cond);
  gInitCondition(&finishCond);
  threads = (GThreadID *)gmallocn(nThreads, sizeof(GThreadID));
  for (i = 0; i < nThreads; ++i) {
    gCreateThread(&threads[i], &threadFunc, this);
  }
}

TileCacheThreadPool::~TileCacheThreadPool() {
  int i;

  gLockMutex(&mutex);
  quit = true;
  gSignalCondition(&cond);
  gUnlockMutex(&mutex);
  for (i = 0; i < nThreads; ++i) {
    gJoinThread(threads[i]);
  }
  gDestroyCondition(&cond);
  gDestroyCondition(&finishCond);
  gDestroyMutex(&mutex);
  gfree(threads);
}

void TileCacheThreadPool::jobAdded() {
  gLockMutex(&mutex);
  gSignalCondition(&cond);
  gUnlockMutex(&mutex);
}

GThreadReturn TileCacheThreadPool::threadFunc(void *arg) {
  ((TileCacheThreadPool *)arg)->worker();
  return 0;
}

void TileCacheThreadPool::worker() {
  CachedTileDesc *ct;

  while (1) {
    gLockMutex(&mutex);
    while (!quit && !(ct = tileCache->getUnstartedTile())) {
      gWaitCondition(&cond, &mutex);
    }
    if (quit) {
      gUnlockMutex(&mutex);
      break;
    }
    if (!tileCache->hasUnstartedTiles()) {
      gClearCondition(&cond);
    }
    gUnlockMutex(&mutex);
    tileCache->rasterizeTile(ct);
    gSignalCondition(&finishCond);
  }
}

void TileCacheThreadPool::waitForFinishedJob() {
  gWaitCondition(&finishCond, &mutex);
}

//------------------------------------------------------------------------
// TileCache
//------------------------------------------------------------------------

TileCache::TileCache(DisplayState *stateA) {
  state = stateA;
  state->setTileCache(this);
  cache = new GList();
  threadPool = new TileCacheThreadPool(this, state->getNWorkerThreads());
  tileDoneCbk = NULL;
  tileDoneCbkData = NULL;
}

TileCache::~TileCache() {
  flushCache(gFalse);
  delete threadPool;
  delete cache;
}

void TileCache::setActiveTileList(GList *tiles) {
  TileDesc *tile;
  CachedTileDesc *ct;
  int tileIdx, cacheIdx;
  GBool newTiles;

  threadPool->lockMutex();

  // remove any unstarted tiles not on the new active list;
  // cancel any started tiles not on the new active list;
  // mark all other tiles as inactive (active tiles will be marked later)
  cacheIdx = 0;
  while (cacheIdx < cache->getLength()) {
    ct = (CachedTileDesc *)cache->get(cacheIdx);
    if (ct->state == cachedTileUnstarted && findTile(ct, tiles) < 0) {
      delete (CachedTileDesc *)cache->del(cacheIdx);
    } else if (ct->state == cachedTileStarted && findTile(ct, tiles) < 0) {
      ct->state = cachedTileCanceled;
      ++cacheIdx;
    } else {
      ct->active = gFalse;
      ++cacheIdx;
    }
  }

  // mark cached tiles as active; add any new tiles to the cache
  newTiles = gFalse;
  for (tileIdx = 0; tileIdx < tiles->getLength(); ++tileIdx) {
    tile = (TileDesc *)tiles->get(tileIdx);
    cacheIdx = findTile(tile, cache);
    if (cacheIdx >= 0) {
      ct = (CachedTileDesc *)cache->del(cacheIdx);
    } else {
      ct = new CachedTileDesc(tile);
      newTiles = gTrue;
    }
    ct->active = gTrue;
    cache->insert(0, ct);
  }

  cleanCache();

  threadPool->unlockMutex();

  if (newTiles) {
    threadPool->jobAdded();
  }
}

SplashBitmap *TileCache::getTileBitmap(TileDesc *tile, GBool *finished) {
  CachedTileDesc *ct;
  SplashBitmap *bitmap;
  int cacheIdx;

  threadPool->lockMutex();
  cacheIdx = findTile(tile, cache);
  if (cacheIdx < 0) {
    threadPool->unlockMutex();
    return NULL;
  }
  ct = (CachedTileDesc *)cache->get(cacheIdx);
  if (ct->state != cachedTileCanceled) {
    bitmap = ct->bitmap;
  } else {
    bitmap = NULL;
  }
  if (finished) {
    *finished = ct->state == cachedTileFinished;
  }
  threadPool->unlockMutex();
  return bitmap;
}

void TileCache::paperColorChanged() {
  flushCache(gFalse);
}

void TileCache::reverseVideoChanged() {
  flushCache(gFalse);
}

void TileCache::optionalContentChanged() {
  flushCache(gFalse);
}

void TileCache::docChanged() {
  flushCache(gTrue);
}


void TileCache::forceRedraw() {
  flushCache(gFalse);
}

// Search for <tile> on <tileList>, and return its index if found, or
// -1 if not found.  If <tile> is part of the cache, or if <tileList>
// is the cache, the caller must have locked the ThreadPool mutex.
int TileCache::findTile(TileDesc *tile, GList *tileList) {
  TileDesc *t;
  int i;

  for (i = 0; i < tileList->getLength(); ++i) {
    t = (TileDesc *)tileList->get(i);
    if (t->matches(tile)) {
      return i;
    }
  }
  return -1;
}

// If there are too many tiles in the cache, remove the least recently
// used tiles.  Never removes active tiles.  The caller must have
// locked the ThreadPool mutex.
void TileCache::cleanCache() {
  CachedTileDesc *ct;
  int n, i;

  // count the number of non-canceled tiles
  n = 0;
  for (i = 0; i < cache->getLength(); ++i) {
    ct = (CachedTileDesc *)cache->get(i);
    if (ct->state != cachedTileCanceled) {
      ++n;
    }
  }

  // if there are too many non-canceled tiles, remove tiles
  i = cache->getLength() - 1;
  while (n > state->getTileCacheSize() && i >= 0) {
    ct = (CachedTileDesc *)cache->get(i);
    if (ct->active) {
      break;
    }
    // any non-active tiles with state == cachedTileUnstarted should
    // already have been removed by setActiveTileList()
    if (ct->state == cachedTileFinished) {
      delete (CachedTileDesc *)cache->del(i);
      --n;
      --i;
    } else {
      --i;
    }
  }
}

// Remove all cached tiles.  For tiles that are being rasterized, sets
// state to canceled.  If <wait> is true, this function won't return
// until the cache is empty, i.e., until all possible users of the
// PDFDoc are done.
void TileCache::flushCache(GBool wait) {
  CachedTileDesc *ct;
  int i;

  threadPool->lockMutex();
  i = 0;
  while (i < cache->getLength()) {
    ct = (CachedTileDesc *)cache->get(i);
    switch (ct->state) {
    case cachedTileUnstarted:
    case cachedTileFinished:
      delete (CachedTileDesc *)cache->del(i);
      break;
    case cachedTileStarted:
      ct->state = cachedTileCanceled;
      ++i;
      break;
    case cachedTileCanceled:
    default:
      ++i;
      break;
    }
  }
  if (wait) {
    while (cache->getLength() > 0) {
      threadPool->waitForFinishedJob();
    }
  }
  threadPool->unlockMutex();
}

// Remove a tile from the cache and delete it.  This will be called
// with the TileCacheThreadPool mutex locked.
void TileCache::removeTile(CachedTileDesc *ct) {
  int i;

  for (i = 0; i < cache->getLength(); ++i) {
    if (cache->get(i) == ct) {
      delete (CachedTileDesc *)cache->del(i);
      break;
    }
  }
}

// Return true if there are one or more unstarted tiles.  This will be
// called with the TileCacheThreadPool mutex locked.
GBool TileCache::hasUnstartedTiles() {
  CachedTileDesc *ct;
  int i;

  for (i = 0; i < cache->getLength(); ++i) {
    ct = (CachedTileDesc *)cache->get(i);
    if (ct->state == cachedTileUnstarted) {
      return gTrue;
    }
  }
  return gFalse;
}

// Return the next unstarted tile, changing its state to
// cachedTileStarted.  If there are no unstarted tiles, return NULL.
// This will be called with the TileCacheThreadPool mutex locked.
CachedTileDesc *TileCache::getUnstartedTile() {
  CachedTileDesc *ct;
  int i;

  for (i = 0; i < cache->getLength(); ++i) {
    ct = (CachedTileDesc *)cache->get(i);
    if (ct->state == cachedTileUnstarted) {
      ct->state = cachedTileStarted;
      return ct;
    }
  }
  return NULL;
}

struct TileCacheStartPageInfo {
  TileCache *tileCache;
  CachedTileDesc *ct;
  SplashOutputDev *out;
};

void TileCache::startPageCbk(void *data) {
  TileCacheStartPageInfo *info = (TileCacheStartPageInfo *)data;

  info->tileCache->threadPool->lockMutex();
  info->ct->bitmap = info->out->getBitmap();
  info->ct->freeBitmap = gFalse;
  info->tileCache->threadPool->unlockMutex();
}

// Rasterize <tile>.  The state has already been set to
// cachedTileStarted.
void TileCache::rasterizeTile(CachedTileDesc *ct) {
  SplashOutputDev *out;
  TileCacheStartPageInfo info;


  out = new SplashOutputDev(state->getColorMode(), 1, state->getReverseVideo(),
			    state->getPaperColor());
  info.tileCache = this;
  info.ct = ct;
  info.out = out;
  out->setStartPageCallback(&TileCache::startPageCbk, &info);
  out->startDoc(state->getDoc()->getXRef());
  state->getDoc()->displayPageSlice(out, ct->page, ct->dpi, ct->dpi, ct->rotate,
				    gFalse, gTrue, gFalse,
				    ct->tx, ct->ty, ct->tw, ct->th,
				    &abortCheckCbk, ct);
  if (ct->state == cachedTileCanceled) {
    threadPool->lockMutex();
    removeTile(ct);
    threadPool->unlockMutex();
  } else {
    threadPool->lockMutex();
    ct->bitmap = out->takeBitmap();
    ct->freeBitmap = gTrue;
    ct->state = cachedTileFinished;
    threadPool->unlockMutex();
    if (tileDoneCbk) {
      (*tileDoneCbk)(tileDoneCbkData);
    }
  }
  delete out;
}


GBool TileCache::abortCheckCbk(void *data) {
  CachedTileDesc *ct = (CachedTileDesc *)data;
  return ct->state == cachedTileCanceled;
}
