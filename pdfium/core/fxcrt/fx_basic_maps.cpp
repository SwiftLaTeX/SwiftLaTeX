// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/plex.h"

CFX_MapPtrToPtr::CFX_MapPtrToPtr(int nBlockSize)
    : m_pHashTable(nullptr),
      m_nHashTableSize(17),
      m_nCount(0),
      m_pFreeList(nullptr),
      m_pBlocks(nullptr),
      m_nBlockSize(nBlockSize) {
  ASSERT(m_nBlockSize > 0);
}

void CFX_MapPtrToPtr::RemoveAll() {
  FX_Free(m_pHashTable);
  m_pHashTable = nullptr;
  m_nCount = 0;
  m_pFreeList = nullptr;
  if (m_pBlocks) {
    m_pBlocks->FreeDataChain();
    m_pBlocks = nullptr;
  }
}

CFX_MapPtrToPtr::~CFX_MapPtrToPtr() {
  RemoveAll();
  ASSERT(m_nCount == 0);
}
uint32_t CFX_MapPtrToPtr::HashKey(void* key) const {
  return ((uint32_t)(uintptr_t)key) >> 4;
}
void CFX_MapPtrToPtr::GetNextAssoc(FX_POSITION& rNextPosition,
                                   void*& rKey,
                                   void*& rValue) const {
  ASSERT(m_pHashTable);
  CAssoc* pAssocRet = (CAssoc*)rNextPosition;
  ASSERT(pAssocRet);
  if (pAssocRet == (CAssoc*)-1) {
    for (uint32_t nBucket = 0; nBucket < m_nHashTableSize; nBucket++) {
      pAssocRet = m_pHashTable[nBucket];
      if (pAssocRet)
        break;
    }
    ASSERT(pAssocRet);
  }
  CAssoc* pAssocNext = pAssocRet->pNext;
  for (uint32_t nBucket = (HashKey(pAssocRet->key) % m_nHashTableSize) + 1;
       nBucket < m_nHashTableSize && !pAssocNext; nBucket++) {
    pAssocNext = m_pHashTable[nBucket];
  }
  rNextPosition = (FX_POSITION)pAssocNext;
  rKey = pAssocRet->key;
  rValue = pAssocRet->value;
}
FX_BOOL CFX_MapPtrToPtr::Lookup(void* key, void*& rValue) const {
  uint32_t nHash;
  CAssoc* pAssoc = GetAssocAt(key, nHash);
  if (!pAssoc) {
    return FALSE;
  }
  rValue = pAssoc->value;
  return TRUE;
}

void* CFX_MapPtrToPtr::GetValueAt(void* key) const {
  uint32_t nHash;
  CAssoc* pAssoc = GetAssocAt(key, nHash);
  return pAssoc ? pAssoc->value : nullptr;
}

void*& CFX_MapPtrToPtr::operator[](void* key) {
  uint32_t nHash;
  CAssoc* pAssoc = GetAssocAt(key, nHash);
  if (!pAssoc) {
    if (!m_pHashTable)
      InitHashTable(m_nHashTableSize);
    pAssoc = NewAssoc();
    pAssoc->key = key;
    pAssoc->pNext = m_pHashTable[nHash];
    m_pHashTable[nHash] = pAssoc;
  }
  return pAssoc->value;
}
CFX_MapPtrToPtr::CAssoc* CFX_MapPtrToPtr::GetAssocAt(void* key,
                                                     uint32_t& nHash) const {
  nHash = HashKey(key) % m_nHashTableSize;
  if (!m_pHashTable) {
    return nullptr;
  }
  CAssoc* pAssoc;
  for (pAssoc = m_pHashTable[nHash]; pAssoc; pAssoc = pAssoc->pNext) {
    if (pAssoc->key == key)
      return pAssoc;
  }
  return nullptr;
}
CFX_MapPtrToPtr::CAssoc* CFX_MapPtrToPtr::NewAssoc() {
  if (!m_pFreeList) {
    CFX_Plex* newBlock = CFX_Plex::Create(m_pBlocks, m_nBlockSize,
                                          sizeof(CFX_MapPtrToPtr::CAssoc));
    CFX_MapPtrToPtr::CAssoc* pAssoc =
        (CFX_MapPtrToPtr::CAssoc*)newBlock->data();
    pAssoc += m_nBlockSize - 1;
    for (int i = m_nBlockSize - 1; i >= 0; i--, pAssoc--) {
      pAssoc->pNext = m_pFreeList;
      m_pFreeList = pAssoc;
    }
  }
  CFX_MapPtrToPtr::CAssoc* pAssoc = m_pFreeList;
  m_pFreeList = m_pFreeList->pNext;
  m_nCount++;
  ASSERT(m_nCount > 0);
  pAssoc->key = 0;
  pAssoc->value = 0;
  return pAssoc;
}
void CFX_MapPtrToPtr::InitHashTable(uint32_t nHashSize, FX_BOOL bAllocNow) {
  ASSERT(m_nCount == 0);
  ASSERT(nHashSize > 0);
  FX_Free(m_pHashTable);
  m_pHashTable = nullptr;
  if (bAllocNow) {
    m_pHashTable = FX_Alloc(CAssoc*, nHashSize);
  }
  m_nHashTableSize = nHashSize;
}
FX_BOOL CFX_MapPtrToPtr::RemoveKey(void* key) {
  if (!m_pHashTable) {
    return FALSE;
  }
  CAssoc** ppAssocPrev;
  ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];
  CAssoc* pAssoc;
  for (pAssoc = *ppAssocPrev; pAssoc; pAssoc = pAssoc->pNext) {
    if (pAssoc->key == key) {
      *ppAssocPrev = pAssoc->pNext;
      FreeAssoc(pAssoc);
      return TRUE;
    }
    ppAssocPrev = &pAssoc->pNext;
  }
  return FALSE;
}
void CFX_MapPtrToPtr::FreeAssoc(CFX_MapPtrToPtr::CAssoc* pAssoc) {
  pAssoc->pNext = m_pFreeList;
  m_pFreeList = pAssoc;
  m_nCount--;
  ASSERT(m_nCount >= 0);
  if (m_nCount == 0) {
    RemoveAll();
  }
}
