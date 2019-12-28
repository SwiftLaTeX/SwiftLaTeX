// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_FGAS_MEMORY_H_
#define XFA_FGAS_CRT_FGAS_MEMORY_H_

#include "core/fxcrt/include/fx_memory.h"
#include "core/fxcrt/include/fx_system.h"

enum FX_ALLOCTYPE {
  FX_ALLOCTYPE_Static,
  FX_ALLOCTYPE_Fixed,
};

class IFX_MemoryAllocator {
 public:
  virtual ~IFX_MemoryAllocator() {}
  virtual void* Alloc(size_t size) = 0;
  virtual void Free(void* pBlock) = 0;

  static IFX_MemoryAllocator* Create(FX_ALLOCTYPE eType,
                                     size_t chunkSize,
                                     size_t blockSize);
};

class CFX_Target {
 public:
  virtual ~CFX_Target() {}
  void* operator new(size_t size) { return FX_Alloc(uint8_t, size); }
  void operator delete(void* p) { FX_Free(p); }
  void* operator new(size_t size, IFX_MemoryAllocator* pAllocator) {
    return pAllocator->Alloc(size);
  }
  void operator delete(void* p, IFX_MemoryAllocator* pAllocator) {
    pAllocator->Free(p);
  }
  void* operator new(size_t size, void* place) { return place; }
  void operator delete(void* p, void* place) {}
};

#define FXTARGET_NewWith(__allocator__) new (__allocator__)
#define FXTARGET_DeleteWith(__class__, __allocator__, pointer) \
  {                                                            \
    (pointer)->~__class__();                                   \
    (pointer)->operator delete((pointer), (__allocator__));    \
  }

#endif  // XFA_FGAS_CRT_FGAS_MEMORY_H_
