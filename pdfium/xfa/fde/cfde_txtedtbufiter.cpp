// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedtbufiter.h"

#include "xfa/fde/cfde_txtedtbuf.h"

CFDE_TxtEdtBufIter::CFDE_TxtEdtBufIter(CFDE_TxtEdtBuf* pBuf, FX_WCHAR wcAlias)
    : m_pBuf(pBuf),
      m_nCurChunk(0),
      m_nCurIndex(0),
      m_nIndex(0),
      m_Alias(wcAlias) {
  ASSERT(m_pBuf);
}

CFDE_TxtEdtBufIter::~CFDE_TxtEdtBufIter() {}

FX_BOOL CFDE_TxtEdtBufIter::Next(FX_BOOL bPrev) {
  if (bPrev) {
    if (m_nIndex == 0) {
      return FALSE;
    }
    ASSERT(m_nCurChunk < m_pBuf->m_Chunks.GetSize());
    CFDE_TxtEdtBuf::FDE_CHUNKHEADER* lpChunk = nullptr;
    if (m_nCurIndex > 0) {
      m_nCurIndex--;
    } else {
      while (m_nCurChunk > 0) {
        --m_nCurChunk;
        lpChunk = m_pBuf->m_Chunks[m_nCurChunk];
        if (lpChunk->nUsed > 0) {
          m_nCurIndex = lpChunk->nUsed - 1;
          break;
        }
      }
    }
    ASSERT(m_nCurChunk >= 0);
    m_nIndex--;
    return TRUE;
  } else {
    if (m_nIndex >= (m_pBuf->m_nTotal - 1)) {
      return FALSE;
    }
    ASSERT(m_nCurChunk < m_pBuf->m_Chunks.GetSize());
    CFDE_TxtEdtBuf::FDE_CHUNKHEADER* lpChunk = m_pBuf->m_Chunks[m_nCurChunk];
    if (lpChunk->nUsed != (m_nCurIndex + 1)) {
      m_nCurIndex++;
    } else {
      int32_t nEnd = m_pBuf->m_Chunks.GetSize() - 1;
      while (m_nCurChunk < nEnd) {
        m_nCurChunk++;
        CFDE_TxtEdtBuf::FDE_CHUNKHEADER* lpChunkTemp =
            m_pBuf->m_Chunks[m_nCurChunk];
        if (lpChunkTemp->nUsed > 0) {
          m_nCurIndex = 0;
          break;
        }
      }
    }
    m_nIndex++;
    return TRUE;
  }
}

void CFDE_TxtEdtBufIter::SetAt(int32_t nIndex) {
  ASSERT(nIndex >= 0 && nIndex < m_pBuf->m_nTotal);
  CFDE_TxtEdtBuf::FDE_CHUNKPLACE cp;
  m_pBuf->Index2CP(nIndex, cp);
  m_nIndex = nIndex;
  m_nCurChunk = cp.nChunkIndex;
  m_nCurIndex = cp.nCharIndex;
}

int32_t CFDE_TxtEdtBufIter::GetAt() const {
  return m_nIndex;
}

FX_WCHAR CFDE_TxtEdtBufIter::GetChar() {
  ASSERT(m_nIndex >= 0 && m_nIndex < m_pBuf->m_nTotal);
  if (m_Alias == 0 || m_nIndex == (m_pBuf->m_nTotal - 1))
    return m_pBuf->m_Chunks[m_nCurChunk]->wChars[m_nCurIndex];
  return m_Alias;
}

FX_BOOL CFDE_TxtEdtBufIter::IsEOF(FX_BOOL bTail) const {
  return bTail ? m_nIndex == (m_pBuf->GetTextLength() - 2) : m_nIndex == 0;
}

IFX_CharIter* CFDE_TxtEdtBufIter::Clone() {
  CFDE_TxtEdtBufIter* pIter = new CFDE_TxtEdtBufIter(m_pBuf);
  pIter->m_nCurChunk = m_nCurChunk;
  pIter->m_nCurIndex = m_nCurIndex;
  pIter->m_nIndex = m_nIndex;
  pIter->m_Alias = m_Alias;
  return pIter;
}
