// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/fgas_memory.h"

#ifndef MEMORY_TOOL_REPLACES_ALLOCATOR
#define MEMORY_TOOL_REPLACES_ALLOCATOR  // Temporary, for CF testing.
#endif

#include <algorithm>

#ifdef MEMORY_TOOL_REPLACES_ALLOCATOR

namespace {

class CFX_DefStore : public IFX_MemoryAllocator, public CFX_Target {
 public:
  CFX_DefStore() {}
  ~CFX_DefStore() override {}

  void* Alloc(size_t size) override { return FX_Alloc(uint8_t, size); }
  void Free(void* pBlock) override { FX_Free(pBlock); }
};

}  // namespace

IFX_MemoryAllocator* IFX_MemoryAllocator::Create(FX_ALLOCTYPE eType,
                                                 size_t chunkSize,
                                                 size_t blockSize) {
  return new CFX_DefStore();
}

#else  // MEMORY_TOOL_REPLACES_ALLOCATOR

namespace {

struct FX_STATICSTORECHUNK {
  FX_STATICSTORECHUNK* pNextChunk;
  size_t iChunkSize;
  size_t iFreeSize;
};

class CFX_StaticStore : public IFX_MemoryAllocator, public CFX_Target {
 public:
  CFX_StaticStore(size_t iDefChunkSize);
  ~CFX_StaticStore() override;

  void* Alloc(size_t size) override;
  void Free(void* pBlock) override {}

 private:
  size_t m_iAllocatedSize;
  size_t m_iDefChunkSize;
  FX_STATICSTORECHUNK* m_pChunk;
  FX_STATICSTORECHUNK* m_pLastChunk;
  FX_STATICSTORECHUNK* AllocChunk(size_t size);
  FX_STATICSTORECHUNK* FindChunk(size_t size);
};

struct FX_FIXEDSTORECHUNK {
  uint8_t* FirstFlag() { return reinterpret_cast<uint8_t*>(this + 1); }
  uint8_t* FirstBlock() { return FirstFlag() + iChunkSize; }

  FX_FIXEDSTORECHUNK* pNextChunk;
  size_t iChunkSize;
  size_t iFreeNum;
};

class CFX_FixedStore : public IFX_MemoryAllocator, public CFX_Target {
 public:
  CFX_FixedStore(size_t iBlockSize, size_t iBlockNumsInChunk);
  ~CFX_FixedStore() override;
  void* Alloc(size_t size) override;
  void Free(void* pBlock) override;

 private:
  FX_FIXEDSTORECHUNK* AllocChunk();

  size_t m_iBlockSize;
  size_t m_iDefChunkSize;
  FX_FIXEDSTORECHUNK* m_pChunk;
};

}  // namespace

#define FX_4BYTEALIGN(size) (((size) + 3) & ~3)

IFX_MemoryAllocator* IFX_MemoryAllocator::Create(FX_ALLOCTYPE eType,
                                                 size_t chunkSize,
                                                 size_t blockSize) {
  switch (eType) {
    case FX_ALLOCTYPE_Static:
      return new CFX_StaticStore(chunkSize);
    case FX_ALLOCTYPE_Fixed:
      return new CFX_FixedStore(blockSize, chunkSize);
    default:
      ASSERT(0);
      return nullptr;
  }
}

CFX_StaticStore::CFX_StaticStore(size_t iDefChunkSize)
    : m_iAllocatedSize(0),
      m_iDefChunkSize(iDefChunkSize),
      m_pChunk(nullptr),
      m_pLastChunk(nullptr) {
  ASSERT(m_iDefChunkSize != 0);
}
CFX_StaticStore::~CFX_StaticStore() {
  FX_STATICSTORECHUNK* pChunk = m_pChunk;
  while (pChunk) {
    FX_STATICSTORECHUNK* pNext = pChunk->pNextChunk;
    FX_Free(pChunk);
    pChunk = pNext;
  }
}
FX_STATICSTORECHUNK* CFX_StaticStore::AllocChunk(size_t size) {
  ASSERT(size != 0);
  FX_STATICSTORECHUNK* pChunk = (FX_STATICSTORECHUNK*)FX_Alloc(
      uint8_t, sizeof(FX_STATICSTORECHUNK) + size);
  pChunk->iChunkSize = size;
  pChunk->iFreeSize = size;
  pChunk->pNextChunk = nullptr;
  if (!m_pLastChunk) {
    m_pChunk = pChunk;
  } else {
    m_pLastChunk->pNextChunk = pChunk;
  }
  m_pLastChunk = pChunk;
  return pChunk;
}
FX_STATICSTORECHUNK* CFX_StaticStore::FindChunk(size_t size) {
  ASSERT(size != 0);
  if (!m_pLastChunk || m_pLastChunk->iFreeSize < size) {
    return AllocChunk(std::max(m_iDefChunkSize, size));
  }
  return m_pLastChunk;
}
void* CFX_StaticStore::Alloc(size_t size) {
  size = FX_4BYTEALIGN(size);
  ASSERT(size != 0);
  FX_STATICSTORECHUNK* pChunk = FindChunk(size);
  ASSERT(pChunk->iFreeSize >= size);
  uint8_t* p = (uint8_t*)pChunk;
  p += sizeof(FX_STATICSTORECHUNK) + pChunk->iChunkSize - pChunk->iFreeSize;
  pChunk->iFreeSize -= size;
  m_iAllocatedSize += size;
  return p;
}
size_t CFX_StaticStore::SetDefChunkSize(size_t size) {
  ASSERT(size != 0);
  size_t v = m_iDefChunkSize;
  m_iDefChunkSize = size;
  return v;
}
CFX_FixedStore::CFX_FixedStore(size_t iBlockSize, size_t iBlockNumsInChunk)
    : m_iBlockSize(FX_4BYTEALIGN(iBlockSize)),
      m_iDefChunkSize(FX_4BYTEALIGN(iBlockNumsInChunk)),
      m_pChunk(nullptr) {
  ASSERT(m_iBlockSize != 0 && m_iDefChunkSize != 0);
}
CFX_FixedStore::~CFX_FixedStore() {
  FX_FIXEDSTORECHUNK* pChunk = m_pChunk;
  while (pChunk) {
    FX_FIXEDSTORECHUNK* pNext = pChunk->pNextChunk;
    FX_Free(pChunk);
    pChunk = pNext;
  }
}
FX_FIXEDSTORECHUNK* CFX_FixedStore::AllocChunk() {
  int32_t iTotalSize = sizeof(FX_FIXEDSTORECHUNK) + m_iDefChunkSize +
                       m_iBlockSize * m_iDefChunkSize;
  FX_FIXEDSTORECHUNK* pChunk =
      (FX_FIXEDSTORECHUNK*)FX_Alloc(uint8_t, iTotalSize);
  if (!pChunk)
    return nullptr;

  FXSYS_memset(pChunk->FirstFlag(), 0, m_iDefChunkSize);
  pChunk->pNextChunk = m_pChunk;
  pChunk->iChunkSize = m_iDefChunkSize;
  pChunk->iFreeNum = m_iDefChunkSize;
  m_pChunk = pChunk;
  return pChunk;
}
void* CFX_FixedStore::Alloc(size_t size) {
  if (size > m_iBlockSize) {
    return nullptr;
  }
  FX_FIXEDSTORECHUNK* pChunk = m_pChunk;
  while (pChunk) {
    if (pChunk->iFreeNum > 0) {
      break;
    }
    pChunk = pChunk->pNextChunk;
  }
  if (!pChunk) {
    pChunk = AllocChunk();
  }
  uint8_t* pFlags = pChunk->FirstFlag();
  size_t i = 0;
  for (; i < pChunk->iChunkSize; i++)
    if (pFlags[i] == 0) {
      break;
    }
  ASSERT(i < pChunk->iChunkSize);
  pFlags[i] = 1;
  pChunk->iFreeNum--;
  return pChunk->FirstBlock() + i * m_iBlockSize;
}
void CFX_FixedStore::Free(void* pBlock) {
  FX_FIXEDSTORECHUNK* pPrior = nullptr;
  FX_FIXEDSTORECHUNK* pChunk = m_pChunk;
  uint8_t* pStart = nullptr;
  uint8_t* pEnd;
  while (pChunk) {
    pStart = pChunk->FirstBlock();
    if (pBlock >= pStart) {
      pEnd = pStart + m_iBlockSize * pChunk->iChunkSize;
      if (pBlock < pEnd) {
        break;
      }
    }
    pPrior = pChunk, pChunk = pChunk->pNextChunk;
  }
  ASSERT(pChunk);
  size_t iPos = ((uint8_t*)pBlock - pStart) / m_iBlockSize;
  ASSERT(iPos < pChunk->iChunkSize);
  uint8_t* pFlags = pChunk->FirstFlag();
  if (pFlags[iPos] == 0) {
    return;
  }
  pFlags[iPos] = 0;
  pChunk->iFreeNum++;
  if (pChunk->iFreeNum == pChunk->iChunkSize) {
    if (!pPrior) {
      m_pChunk = pChunk->pNextChunk;
    } else {
      pPrior->pNextChunk = pChunk->pNextChunk;
    }
    FX_Free(pChunk);
  }
}
size_t CFX_FixedStore::SetDefChunkSize(size_t iChunkSize) {
  ASSERT(iChunkSize != 0);
  size_t v = m_iDefChunkSize;
  m_iDefChunkSize = FX_4BYTEALIGN(iChunkSize);
  return v;
}

#endif  // MEMORY_TOOL_REPLACES_ALLOCATOR
