// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedtbuf.h"

#include "xfa/fgas/crt/fgas_memory.h"

namespace {

const int kDefaultChunkSize = 1024;
const int kDefaultChunkCount = 2;

}  // namespace

CFDE_TxtEdtBuf::CFDE_TxtEdtBuf()
    : m_nChunkSize(kDefaultChunkSize),
      m_nTotal(0),
      m_bChanged(FALSE),
      m_pAllocator(nullptr) {
  ASSERT(m_nChunkSize);
  ResetChunkBuffer(kDefaultChunkCount, m_nChunkSize);
}

CFDE_TxtEdtBuf::~CFDE_TxtEdtBuf() {
  Clear(TRUE);
  delete m_pAllocator;
  m_Chunks.RemoveAll();
}

int32_t CFDE_TxtEdtBuf::GetChunkSize() const {
  return m_nChunkSize;
}

int32_t CFDE_TxtEdtBuf::GetTextLength() const {
  return m_nTotal;
}

void CFDE_TxtEdtBuf::SetText(const CFX_WideString& wsText) {
  ASSERT(!wsText.IsEmpty());
  Clear(FALSE);
  int32_t nTextLength = wsText.GetLength();
  int32_t nNeedCount =
      ((nTextLength - 1) / m_nChunkSize + 1) - m_Chunks.GetSize();
  int32_t i = 0;
  for (i = 0; i < nNeedCount; i++) {
    FDE_CHUNKHEADER* lpChunk =
        static_cast<FDE_CHUNKHEADER*>(m_pAllocator->Alloc(
            sizeof(FDE_CHUNKHEADER) + (m_nChunkSize - 1) * sizeof(FX_WCHAR)));
    lpChunk->nUsed = 0;
    m_Chunks.Add(lpChunk);
  }
  int32_t nTotalCount = m_Chunks.GetSize();
  const FX_WCHAR* lpSrcBuf = wsText.c_str();
  int32_t nLeave = nTextLength;
  int32_t nCopyedLength = m_nChunkSize;
  for (i = 0; i < nTotalCount && nLeave > 0; i++) {
    if (nLeave < nCopyedLength) {
      nCopyedLength = nLeave;
    }
    FDE_CHUNKHEADER* lpChunk = m_Chunks[i];
    FXSYS_memcpy(lpChunk->wChars, lpSrcBuf, nCopyedLength * sizeof(FX_WCHAR));
    nLeave -= nCopyedLength;
    lpSrcBuf += nCopyedLength;
    lpChunk->nUsed = nCopyedLength;
  }
  m_nTotal = nTextLength;
  m_bChanged = TRUE;
}

void CFDE_TxtEdtBuf::GetText(CFX_WideString& wsText) const {
  GetRange(wsText, 0, m_nTotal);
}

FX_WCHAR CFDE_TxtEdtBuf::GetCharByIndex(int32_t nIndex) const {
  ASSERT(nIndex >= 0 && nIndex < GetTextLength());
  FDE_CHUNKHEADER* pChunkHeader = nullptr;
  int32_t nTotal = 0;
  int32_t nCount = m_Chunks.GetSize();
  int32_t i = 0;
  for (i = 0; i < nCount; i++) {
    pChunkHeader = m_Chunks[i];
    nTotal += pChunkHeader->nUsed;
    if (nTotal > nIndex) {
      break;
    }
  }
  ASSERT(pChunkHeader);
  return pChunkHeader->wChars[pChunkHeader->nUsed - (nTotal - nIndex)];
}

void CFDE_TxtEdtBuf::GetRange(CFX_WideString& wsText,
                              int32_t nBegin,
                              int32_t nLength) const {
  FDE_CHUNKPLACE cp;
  Index2CP(nBegin, cp);
  int32_t nLeave = nLength;
  int32_t nCount = m_Chunks.GetSize();
  FX_WCHAR* lpDstBuf = wsText.GetBuffer(nLength);
  int32_t nChunkIndex = cp.nChunkIndex;
  FDE_CHUNKHEADER* lpChunkHeader = m_Chunks[nChunkIndex];
  int32_t nCopyLength = lpChunkHeader->nUsed - cp.nCharIndex;
  FX_WCHAR* lpSrcBuf = lpChunkHeader->wChars + cp.nCharIndex;
  while (nLeave > 0) {
    if (nLeave <= nCopyLength) {
      nCopyLength = nLeave;
    }
    FXSYS_memcpy(lpDstBuf, lpSrcBuf, nCopyLength * sizeof(FX_WCHAR));
    nChunkIndex++;
    if (nChunkIndex >= nCount) {
      break;
    }
    lpChunkHeader = m_Chunks[nChunkIndex];
    lpSrcBuf = lpChunkHeader->wChars;
    nLeave -= nCopyLength;
    lpDstBuf += nCopyLength;
    nCopyLength = lpChunkHeader->nUsed;
  }
  wsText.ReleaseBuffer();
}

void CFDE_TxtEdtBuf::Insert(int32_t nPos,
                            const FX_WCHAR* lpText,
                            int32_t nLength) {
  ASSERT(nPos >= 0 && nPos <= m_nTotal);
  FDE_CHUNKPLACE cp;
  Index2CP(nPos, cp);
  int32_t nLengthTemp = nLength;
  if (cp.nCharIndex != 0) {
    FDE_CHUNKHEADER* lpNewChunk =
        static_cast<FDE_CHUNKHEADER*>(m_pAllocator->Alloc(
            sizeof(FDE_CHUNKHEADER) + (m_nChunkSize - 1) * sizeof(FX_WCHAR)));
    FDE_CHUNKHEADER* lpChunk = m_Chunks[cp.nChunkIndex];
    int32_t nCopy = lpChunk->nUsed - cp.nCharIndex;
    FXSYS_memcpy(lpNewChunk->wChars, lpChunk->wChars + cp.nCharIndex,
                 nCopy * sizeof(FX_WCHAR));
    lpChunk->nUsed -= nCopy;
    cp.nChunkIndex++;
    m_Chunks.InsertAt(cp.nChunkIndex, lpNewChunk);
    lpNewChunk->nUsed = nCopy;
    cp.nCharIndex = 0;
  }
  if (cp.nChunkIndex != 0) {
    FDE_CHUNKHEADER* lpChunk = m_Chunks[cp.nChunkIndex - 1];
    if (lpChunk->nUsed != m_nChunkSize) {
      cp.nChunkIndex--;
      int32_t nFree = m_nChunkSize - lpChunk->nUsed;
      int32_t nCopy = std::min(nLengthTemp, nFree);
      FXSYS_memcpy(lpChunk->wChars + lpChunk->nUsed, lpText,
                   nCopy * sizeof(FX_WCHAR));
      lpText += nCopy;
      nLengthTemp -= nCopy;
      lpChunk->nUsed += nCopy;
      cp.nChunkIndex++;
    }
  }
  while (nLengthTemp > 0) {
    FDE_CHUNKHEADER* lpChunk =
        static_cast<FDE_CHUNKHEADER*>(m_pAllocator->Alloc(
            sizeof(FDE_CHUNKHEADER) + (m_nChunkSize - 1) * sizeof(FX_WCHAR)));
    ASSERT(lpChunk);
    int32_t nCopy = std::min(nLengthTemp, m_nChunkSize);
    FXSYS_memcpy(lpChunk->wChars, lpText, nCopy * sizeof(FX_WCHAR));
    lpText += nCopy;
    nLengthTemp -= nCopy;
    lpChunk->nUsed = nCopy;
    m_Chunks.InsertAt(cp.nChunkIndex, lpChunk);
    cp.nChunkIndex++;
  }
  m_nTotal += nLength;
  m_bChanged = TRUE;
}

void CFDE_TxtEdtBuf::Delete(int32_t nIndex, int32_t nLength) {
  ASSERT(nLength > 0 && nIndex >= 0 && nIndex + nLength <= m_nTotal);
  FDE_CHUNKPLACE cpEnd;
  Index2CP(nIndex + nLength - 1, cpEnd);
  m_nTotal -= nLength;
  FDE_CHUNKHEADER* lpChunk = m_Chunks[cpEnd.nChunkIndex];
  int32_t nFirstPart = cpEnd.nCharIndex + 1;
  int32_t nMovePart = lpChunk->nUsed - nFirstPart;
  if (nMovePart != 0) {
    int32_t nDelete = std::min(nFirstPart, nLength);
    FXSYS_memmove(lpChunk->wChars + nFirstPart - nDelete,
                  lpChunk->wChars + nFirstPart, nMovePart * sizeof(FX_WCHAR));
    lpChunk->nUsed -= nDelete;
    nLength -= nDelete;
    cpEnd.nChunkIndex--;
  }
  while (nLength > 0) {
    lpChunk = m_Chunks[cpEnd.nChunkIndex];
    int32_t nDeleted = std::min(lpChunk->nUsed, nLength);
    lpChunk->nUsed -= nDeleted;
    if (lpChunk->nUsed == 0) {
      m_pAllocator->Free(lpChunk);
      m_Chunks.RemoveAt(cpEnd.nChunkIndex);
      lpChunk = nullptr;
    }
    nLength -= nDeleted;
    cpEnd.nChunkIndex--;
  }
  m_bChanged = TRUE;
}

void CFDE_TxtEdtBuf::Clear(FX_BOOL bRelease) {
  int32_t i = 0;
  int32_t nCount = m_Chunks.GetSize();
  if (bRelease) {
    while (i < nCount) {
      m_pAllocator->Free(m_Chunks[i++]);
    }
    m_Chunks.RemoveAll();
  } else {
    while (i < nCount) {
      m_Chunks[i++]->nUsed = 0;
    }
  }
  m_nTotal = 0;
  m_bChanged = TRUE;
}

FX_BOOL CFDE_TxtEdtBuf::Optimize(IFX_Pause* pPause) {
  if (m_bChanged == FALSE) {
    return TRUE;
  }
  if (m_nTotal == 0) {
    return TRUE;
  }
  int32_t nCount = m_Chunks.GetSize();
  if (nCount == 0) {
    return TRUE;
  }
  int32_t i = 0;
  for (; i < nCount; i++) {
    FDE_CHUNKHEADER* lpChunk = m_Chunks[i];
    if (lpChunk->nUsed == 0) {
      m_pAllocator->Free(lpChunk);
      m_Chunks.RemoveAt(i);
      --i;
      --nCount;
    }
  }
  if (pPause && pPause->NeedToPauseNow())
    return FALSE;

  FDE_CHUNKHEADER* lpPreChunk = m_Chunks[0];
  FDE_CHUNKHEADER* lpCurChunk = nullptr;
  for (i = 1; i < nCount; i++) {
    lpCurChunk = m_Chunks[i];
    if (lpPreChunk->nUsed + lpCurChunk->nUsed <= m_nChunkSize) {
      FXSYS_memcpy(lpPreChunk->wChars + lpPreChunk->nUsed, lpCurChunk->wChars,
                   lpCurChunk->nUsed * sizeof(FX_WCHAR));
      lpPreChunk->nUsed += lpCurChunk->nUsed;
      m_pAllocator->Free(lpCurChunk);
      m_Chunks.RemoveAt(i);
      --i;
      --nCount;
    } else {
      lpPreChunk = lpCurChunk;
    }
    if (pPause && pPause->NeedToPauseNow())
      return FALSE;
  }
  m_bChanged = FALSE;
  return TRUE;
}

void CFDE_TxtEdtBuf::ResetChunkBuffer(int32_t nDefChunkCount,
                                      int32_t nChunkSize) {
  ASSERT(nChunkSize);
  ASSERT(nDefChunkCount);
  delete m_pAllocator;
  m_pAllocator = nullptr;
  m_Chunks.RemoveAll();
  m_nChunkSize = nChunkSize;
  int32_t nChunkLength =
      sizeof(FDE_CHUNKHEADER) + (m_nChunkSize - 1) * sizeof(FX_WCHAR);
  m_pAllocator = IFX_MemoryAllocator::Create(FX_ALLOCTYPE_Fixed, nDefChunkCount,
                                             nChunkLength);
  FDE_CHUNKHEADER* lpChunkHeader =
      static_cast<FDE_CHUNKHEADER*>(m_pAllocator->Alloc(nChunkLength));
  ASSERT(lpChunkHeader);
  lpChunkHeader->nUsed = 0;
  m_Chunks.Add(lpChunkHeader);
  m_nTotal = 0;
}

int32_t CFDE_TxtEdtBuf::CP2Index(const FDE_CHUNKPLACE& cp) const {
  int32_t nTotal = cp.nCharIndex;
  int32_t i = 0;
  for (i = 0; i < cp.nChunkIndex; i++) {
    nTotal += m_Chunks[i]->nUsed;
  }
  return nTotal;
}

void CFDE_TxtEdtBuf::Index2CP(int32_t nIndex, FDE_CHUNKPLACE& cp) const {
  ASSERT(nIndex <= GetTextLength());
  if (nIndex == m_nTotal) {
    cp.nChunkIndex = m_Chunks.GetSize() - 1;
    cp.nCharIndex = m_Chunks[cp.nChunkIndex]->nUsed;
    return;
  }
  int32_t i = 0;
  int32_t nTotal = 0;
  int32_t nCount = m_Chunks.GetSize();
  for (; i < nCount; i++) {
    nTotal += m_Chunks[i]->nUsed;
    if (nTotal > nIndex) {
      break;
    }
  }
  cp.nChunkIndex = i;
  cp.nCharIndex = m_Chunks[i]->nUsed - (nTotal - nIndex);
}
