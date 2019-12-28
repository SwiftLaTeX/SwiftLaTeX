// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/fgas_utils.h"

#include <algorithm>

#include "core/fxcrt/include/fx_basic.h"

class FX_BASEARRAYDATA : public CFX_Target {
 public:
  FX_BASEARRAYDATA(int32_t growsize, int32_t blocksize);
  ~FX_BASEARRAYDATA() override;

  int32_t iGrowSize;
  int32_t iBlockSize;
  int32_t iTotalCount;
  int32_t iBlockCount;
  uint8_t* pBuffer;
};

FX_BASEARRAYDATA::FX_BASEARRAYDATA(int32_t growsize, int32_t blocksize)
    : iGrowSize(growsize),
      iBlockSize(blocksize),
      iTotalCount(0),
      iBlockCount(0),
      pBuffer(nullptr) {}

FX_BASEARRAYDATA::~FX_BASEARRAYDATA() {
  FX_Free(pBuffer);
}

CFX_BaseArray::CFX_BaseArray(int32_t iGrowSize, int32_t iBlockSize) {
  ASSERT(iGrowSize > 0 && iBlockSize > 0);
  m_pData = new FX_BASEARRAYDATA(iGrowSize, iBlockSize);
}
CFX_BaseArray::~CFX_BaseArray() {
  RemoveAll();
  delete m_pData;
}
int32_t CFX_BaseArray::GetSize() const {
  return m_pData->iBlockCount;
}
int32_t CFX_BaseArray::GetBlockSize() const {
  return m_pData->iBlockSize;
}
uint8_t* CFX_BaseArray::AddSpaceTo(int32_t index) {
  ASSERT(index > -1);
  uint8_t*& pBuffer = m_pData->pBuffer;
  int32_t& iTotalCount = m_pData->iTotalCount;
  int32_t iBlockSize = m_pData->iBlockSize;
  if (index >= iTotalCount) {
    int32_t iGrowSize = m_pData->iGrowSize;
    iTotalCount = (index / iGrowSize + 1) * iGrowSize;
    int32_t iNewSize = iTotalCount * iBlockSize;
    if (!pBuffer) {
      pBuffer = FX_Alloc(uint8_t, iNewSize);
    } else {
      pBuffer = FX_Realloc(uint8_t, pBuffer, iNewSize);
    }
  }
  int32_t& iBlockCount = m_pData->iBlockCount;
  if (index >= iBlockCount) {
    iBlockCount = index + 1;
  }
  return pBuffer + index * iBlockSize;
}
uint8_t* CFX_BaseArray::GetAt(int32_t index) const {
  ASSERT(index > -1 && index < m_pData->iBlockCount);
  return m_pData->pBuffer + index * m_pData->iBlockSize;
}
uint8_t* CFX_BaseArray::GetBuffer() const {
  return m_pData->pBuffer;
}
int32_t CFX_BaseArray::Append(const CFX_BaseArray& src,
                              int32_t iStart,
                              int32_t iCount) {
  int32_t iBlockSize = m_pData->iBlockSize;
  ASSERT(iBlockSize == src.m_pData->iBlockSize);
  int32_t& iBlockCount = m_pData->iBlockCount;
  int32_t iAdded = src.GetSize();
  ASSERT(iStart > -1 && iStart < iAdded);
  if (iCount < 0) {
    iCount = iAdded;
  }
  if (iStart + iCount > iAdded) {
    iCount = iAdded - iStart;
  }
  if (iCount < 1) {
    return 0;
  }
  uint8_t* pDst = m_pData->pBuffer + iBlockCount * iBlockSize;
  AddSpaceTo(iBlockCount + iCount - 1);
  FXSYS_memcpy(pDst, src.m_pData->pBuffer + iStart * iBlockSize,
               iCount * iBlockSize);
  return iCount;
}
int32_t CFX_BaseArray::Copy(const CFX_BaseArray& src,
                            int32_t iStart,
                            int32_t iCount) {
  int32_t iBlockSize = m_pData->iBlockSize;
  ASSERT(iBlockSize == src.m_pData->iBlockSize);
  int32_t iCopied = src.GetSize();
  ASSERT(iStart > -1 && iStart < iCopied);
  if (iCount < 0) {
    iCount = iCopied;
  }
  if (iStart + iCount > iCopied) {
    iCount = iCopied - iStart;
  }
  if (iCount < 1) {
    return 0;
  }
  RemoveAll(TRUE);
  AddSpaceTo(iCount - 1);
  FXSYS_memcpy(m_pData->pBuffer, src.m_pData->pBuffer + iStart * iBlockSize,
               iCount * iBlockSize);
  return iCount;
}
int32_t CFX_BaseArray::RemoveLast(int32_t iCount) {
  int32_t& iBlockCount = m_pData->iBlockCount;
  if (iCount < 0 || iCount > iBlockCount) {
    iCount = iBlockCount;
    iBlockCount = 0;
  } else {
    iBlockCount -= iCount;
  }
  return iCount;
}
void CFX_BaseArray::RemoveAll(FX_BOOL bLeaveMemory) {
  if (!bLeaveMemory) {
    uint8_t*& pBuffer = m_pData->pBuffer;
    if (pBuffer) {
      FX_Free(pBuffer);
      pBuffer = nullptr;
    }
    m_pData->iTotalCount = 0;
  }
  m_pData->iBlockCount = 0;
}

CFX_BaseMassArrayImp::CFX_BaseMassArrayImp(int32_t iChunkSize,
                                           int32_t iBlockSize)
    : m_iChunkSize(iChunkSize),
      m_iBlockSize(iBlockSize),
      m_iChunkCount(0),
      m_iBlockCount(0),
      m_pData(new CFX_ArrayTemplate<void*>()) {
  ASSERT(m_iChunkSize > 0 && m_iBlockSize > 0);
  m_pData->SetSize(16);
}
CFX_BaseMassArrayImp::~CFX_BaseMassArrayImp() {
  RemoveAll();
  delete m_pData;
}
uint8_t* CFX_BaseMassArrayImp::AddSpaceTo(int32_t index) {
  ASSERT(index > -1);
  uint8_t* pChunk;
  if (index < m_iBlockCount) {
    pChunk = (uint8_t*)m_pData->GetAt(index / m_iChunkSize);
  } else {
    int32_t iMemSize = m_iChunkSize * m_iBlockSize;
    while (TRUE) {
      if (index < m_iChunkCount * m_iChunkSize) {
        pChunk = (uint8_t*)m_pData->GetAt(index / m_iChunkSize);
        break;
      } else {
        pChunk = FX_Alloc(uint8_t, iMemSize);
        if (m_iChunkCount < m_pData->GetSize()) {
          m_pData->SetAt(m_iChunkCount, pChunk);
        } else {
          m_pData->Add(pChunk);
        }
        m_iChunkCount++;
      }
    }
  }
  ASSERT(pChunk);
  m_iBlockCount = index + 1;
  return pChunk + (index % m_iChunkSize) * m_iBlockSize;
}
uint8_t* CFX_BaseMassArrayImp::GetAt(int32_t index) const {
  ASSERT(index > -1 && index < m_iBlockCount);
  uint8_t* pChunk = (uint8_t*)m_pData->GetAt(index / m_iChunkSize);
  ASSERT(pChunk);
  return pChunk + (index % m_iChunkSize) * m_iBlockSize;
}
int32_t CFX_BaseMassArrayImp::Append(const CFX_BaseMassArrayImp& src,
                                     int32_t iStart,
                                     int32_t iCount) {
  ASSERT(m_iBlockSize == src.m_iBlockSize);
  int32_t iAdded = src.m_iBlockCount;
  ASSERT(iStart > -1 && iStart < iAdded);
  if (iCount < 0) {
    iCount = iAdded;
  }
  if (iStart + iCount > iAdded) {
    iCount = iAdded - iStart;
  }
  if (iCount < 1) {
    return m_iBlockCount;
  }
  int32_t iBlockCount = m_iBlockCount;
  int32_t iTotal = m_iBlockCount + iCount;
  AddSpaceTo(iTotal - 1);
  Append(iBlockCount, src, iStart, iCount);
  return m_iBlockCount;
}
int32_t CFX_BaseMassArrayImp::Copy(const CFX_BaseMassArrayImp& src,
                                   int32_t iStart,
                                   int32_t iCount) {
  ASSERT(m_iBlockSize == src.m_iBlockSize);
  int32_t iCopied = src.m_iBlockCount;
  ASSERT(iStart > -1);
  if (iStart >= iCopied) {
    return 0;
  }
  RemoveAll(TRUE);
  if (iCount < 0) {
    iCount = iCopied;
  }
  if (iStart + iCount > iCopied) {
    iCount = iCopied - iStart;
  }
  if (iCount < 1) {
    return 0;
  }
  if (m_iBlockCount < iCount) {
    AddSpaceTo(iCount - 1);
  }
  Append(0, src, iStart, iCount);
  return m_iBlockCount;
}

void CFX_BaseMassArrayImp::Append(int32_t iDstStart,
                                  const CFX_BaseMassArrayImp& src,
                                  int32_t iSrcStart,
                                  int32_t iSrcCount) {
  ASSERT(iDstStart > -1);
  ASSERT(m_iBlockSize == src.m_iBlockSize);
  ASSERT(src.m_iBlockCount > 0);
  ASSERT(m_iBlockCount >= iDstStart + iSrcCount);
  ASSERT(iSrcStart > -1);
  ASSERT(iSrcStart < src.m_iBlockCount);
  ASSERT(iSrcCount > 0);
  ASSERT(iSrcStart + iSrcCount <= src.m_iBlockCount);

  int32_t iDstChunkIndex = iDstStart / m_iChunkSize;
  int32_t iSrcChunkIndex = iSrcStart / src.m_iChunkSize;
  uint8_t* pDstChunk = (uint8_t*)GetAt(iDstStart);
  uint8_t* pSrcChunk = (uint8_t*)src.GetAt(iSrcStart);
  int32_t iDstChunkSize = m_iChunkSize - (iDstStart % m_iChunkSize);
  int32_t iSrcChunkSize = src.m_iChunkSize - (iSrcStart % src.m_iChunkSize);
  int32_t iCopySize =
      std::min(iSrcCount, std::min(iSrcChunkSize, iDstChunkSize));
  int32_t iCopyBytes = iCopySize * m_iBlockSize;
  while (iSrcCount > 0) {
    ASSERT(pDstChunk && pSrcChunk);
    FXSYS_memcpy(pDstChunk, pSrcChunk, iCopyBytes);
    iSrcCount -= iCopySize;
    iSrcChunkSize -= iCopySize;
    if (iSrcChunkSize < 1) {
      iSrcChunkSize = src.m_iChunkSize;
      iSrcChunkIndex++;
      pSrcChunk = (uint8_t*)src.m_pData->GetAt(iSrcChunkIndex);
    } else {
      pSrcChunk += iCopyBytes;
    }
    iDstChunkSize -= iCopySize;
    if (iDstChunkSize < 1) {
      iDstChunkSize = m_iChunkSize;
      iDstChunkIndex++;
      pDstChunk = (uint8_t*)m_pData->GetAt(iDstChunkIndex);
    } else {
      pDstChunk += iCopyBytes;
    }
    iCopySize = std::min(iSrcCount, std::min(iSrcChunkSize, iDstChunkSize));
    iCopyBytes = iCopySize * m_iBlockSize;
  }
}
int32_t CFX_BaseMassArrayImp::RemoveLast(int32_t iCount) {
  if (iCount < 0 || iCount >= m_iBlockCount) {
    m_iBlockCount = 0;
  } else {
    m_iBlockCount -= iCount;
  }
  return m_iBlockCount;
}
void CFX_BaseMassArrayImp::RemoveAll(FX_BOOL bLeaveMemory) {
  if (bLeaveMemory) {
    m_iBlockCount = 0;
    return;
  }
  for (int32_t i = 0; i < m_iChunkCount; i++)
    FX_Free(m_pData->GetAt(i));

  m_pData->RemoveAll();
  m_iChunkCount = 0;
  m_iBlockCount = 0;
}
CFX_BaseMassArray::CFX_BaseMassArray(int32_t iChunkSize, int32_t iBlockSize) {
  m_pData = new CFX_BaseMassArrayImp(iChunkSize, iBlockSize);
}
CFX_BaseMassArray::~CFX_BaseMassArray() {
  delete m_pData;
}
int32_t CFX_BaseMassArray::GetSize() const {
  return m_pData->m_iBlockCount;
}
uint8_t* CFX_BaseMassArray::AddSpaceTo(int32_t index) {
  return m_pData->AddSpaceTo(index);
}
uint8_t* CFX_BaseMassArray::GetAt(int32_t index) const {
  return m_pData->GetAt(index);
}
int32_t CFX_BaseMassArray::Append(const CFX_BaseMassArray& src,
                                  int32_t iStart,
                                  int32_t iCount) {
  return m_pData->Append(*(CFX_BaseMassArrayImp*)src.m_pData, iStart, iCount);
}
int32_t CFX_BaseMassArray::Copy(const CFX_BaseMassArray& src,
                                int32_t iStart,
                                int32_t iCount) {
  return m_pData->Copy(*(CFX_BaseMassArrayImp*)src.m_pData, iStart, iCount);
}
int32_t CFX_BaseMassArray::RemoveLast(int32_t iCount) {
  return m_pData->RemoveLast(iCount);
}
void CFX_BaseMassArray::RemoveAll(FX_BOOL bLeaveMemory) {
  m_pData->RemoveAll(bLeaveMemory);
}

struct FX_BASEDISCRETEARRAYDATA {
  int32_t iBlockSize;
  int32_t iChunkSize;
  int32_t iChunkCount;
  CFX_ArrayTemplate<uint8_t*> ChunkBuffer;
};

CFX_BaseDiscreteArray::CFX_BaseDiscreteArray(int32_t iChunkSize,
                                             int32_t iBlockSize) {
  ASSERT(iChunkSize > 0 && iBlockSize > 0);
  FX_BASEDISCRETEARRAYDATA* pData = new FX_BASEDISCRETEARRAYDATA;
  m_pData = pData;
  pData->ChunkBuffer.SetSize(16);
  pData->iChunkCount = 0;
  pData->iChunkSize = iChunkSize;
  pData->iBlockSize = iBlockSize;
}
CFX_BaseDiscreteArray::~CFX_BaseDiscreteArray() {
  RemoveAll();
  delete static_cast<FX_BASEDISCRETEARRAYDATA*>(m_pData);
}
uint8_t* CFX_BaseDiscreteArray::AddSpaceTo(int32_t index) {
  ASSERT(index > -1);
  FX_BASEDISCRETEARRAYDATA* pData = (FX_BASEDISCRETEARRAYDATA*)m_pData;
  int32_t& iChunkCount = pData->iChunkCount;
  int32_t iChunkSize = pData->iChunkSize;
  uint8_t* pChunk = nullptr;
  int32_t iChunk = index / iChunkSize;
  if (iChunk < iChunkCount) {
    pChunk = pData->ChunkBuffer.GetAt(iChunk);
  }
  if (!pChunk) {
    pChunk = FX_Alloc2D(uint8_t, iChunkSize, pData->iBlockSize);
    FXSYS_memset(pChunk, 0, iChunkSize * pData->iBlockSize);
    pData->ChunkBuffer.SetAtGrow(iChunk, pChunk);
    if (iChunkCount <= iChunk) {
      iChunkCount = iChunk + 1;
    }
  }
  return pChunk + (index % iChunkSize) * pData->iBlockSize;
}
uint8_t* CFX_BaseDiscreteArray::GetAt(int32_t index) const {
  ASSERT(index >= 0);
  FX_BASEDISCRETEARRAYDATA* pData = (FX_BASEDISCRETEARRAYDATA*)m_pData;
  int32_t iChunkSize = pData->iChunkSize;
  int32_t iChunk = index / iChunkSize;
  if (iChunk >= pData->iChunkCount)
    return nullptr;

  uint8_t* pChunk = pData->ChunkBuffer.GetAt(iChunk);
  if (!pChunk)
    return nullptr;

  return pChunk + (index % iChunkSize) * pData->iBlockSize;
}
void CFX_BaseDiscreteArray::RemoveAll() {
  FX_BASEDISCRETEARRAYDATA* pData = (FX_BASEDISCRETEARRAYDATA*)m_pData;
  CFX_ArrayTemplate<uint8_t*>& ChunkBuffer = pData->ChunkBuffer;
  int32_t& iChunkCount = pData->iChunkCount;
  for (int32_t i = 0; i < iChunkCount; i++)
    FX_Free(ChunkBuffer.GetAt(i));

  ChunkBuffer.RemoveAll();
  iChunkCount = 0;
}
CFX_BaseStack::CFX_BaseStack(int32_t iChunkSize, int32_t iBlockSize) {
  m_pData = new CFX_BaseMassArrayImp(iChunkSize, iBlockSize);
}
CFX_BaseStack::~CFX_BaseStack() {
  delete (CFX_BaseMassArrayImp*)m_pData;
}
uint8_t* CFX_BaseStack::Push() {
  return m_pData->AddSpace();
}
void CFX_BaseStack::Pop() {
  int32_t& iBlockCount = m_pData->m_iBlockCount;
  if (iBlockCount < 1) {
    return;
  }
  iBlockCount--;
}
uint8_t* CFX_BaseStack::GetTopElement() const {
  int32_t iSize = m_pData->m_iBlockCount;
  if (iSize < 1) {
    return nullptr;
  }
  return m_pData->GetAt(iSize - 1);
}
int32_t CFX_BaseStack::GetSize() const {
  return m_pData->m_iBlockCount;
}
uint8_t* CFX_BaseStack::GetAt(int32_t index) const {
  return m_pData->GetAt(index);
}
void CFX_BaseStack::RemoveAll(FX_BOOL bLeaveMemory) {
  m_pData->RemoveAll(bLeaveMemory);
}
