//========================================================================
//
// TileCache.h
//
// Copyright 2014 Glyph & Cog, LLC
//
//========================================================================

#ifndef TILECACHE_H
#define TILECACHE_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "SplashTypes.h"

class SplashBitmap;
class SplashOutputDev;
class DisplayState;
class CachedTileDesc;
class TileCacheThreadPool;
class TileDesc;

//------------------------------------------------------------------------

class TileCache {
public:

  TileCache(DisplayState *stateA);
  ~TileCache();

  // Set the list of currently displayed tiles (TileDesc objects).
  void setActiveTileList(GList *tiles);

  // Return the bitmap for a tile.  The tile must be on the current
  // active list.  This can return NULL if tile rasterization hasn't
  // started yet.  If <finished> is non-NULL, *<finished> will be set
  // to true if rasterization of this bitmap is finished, false
  // otherwise.
  SplashBitmap *getTileBitmap(TileDesc *tile, GBool *finished);

  // Set a callback to be called whenever a tile rasterization is
  // finished.  NB: this callback will be called from a worker thread.
  void setTileDoneCbk(void (*cbk)(void *data), void *data)
    { tileDoneCbk = cbk; tileDoneCbkData = data; }

  void paperColorChanged();
  void reverseVideoChanged();
  void optionalContentChanged();
  void docChanged();
  void forceRedraw();

private:

  int findTile(TileDesc *tile, GList *tileList);
  void cleanCache();
  void flushCache(GBool wait);
  void removeTile(CachedTileDesc *ct);
  GBool hasUnstartedTiles();
  CachedTileDesc *getUnstartedTile();
  static void startPageCbk(void *data);
  void rasterizeTile(CachedTileDesc *tile);
  static GBool abortCheckCbk(void *data);

  DisplayState *state;
  GList *cache;			// [CachedTileDesc]
  TileCacheThreadPool *threadPool;
  void (*tileDoneCbk)(void *data);
  void *tileDoneCbkData;

  friend class TileCacheThreadPool;
};

#endif
