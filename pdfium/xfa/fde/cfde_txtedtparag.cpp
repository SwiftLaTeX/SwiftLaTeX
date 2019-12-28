// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedtparag.h"

#include "xfa/fde/cfde_txtedtbuf.h"
#include "xfa/fde/cfde_txtedtbufiter.h"
#include "xfa/fde/cfde_txtedtengine.h"
#include "xfa/fde/ifde_txtedtengine.h"
#include "xfa/fgas/layout/fgas_textbreak.h"
#include "xfa/fde/ifx_chariter.h"

CFDE_TxtEdtParag::CFDE_TxtEdtParag(CFDE_TxtEdtEngine* pEngine)
    : m_nCharStart(0),
      m_nCharCount(0),
      m_nLineCount(0),
      m_lpData(nullptr),
      m_pEngine(pEngine) {
  ASSERT(m_pEngine);
}

CFDE_TxtEdtParag::~CFDE_TxtEdtParag() {
  if (m_lpData)
    FX_Free(m_lpData);
}

void CFDE_TxtEdtParag::LoadParag() {
  if (m_lpData) {
    m_lpData[0]++;
    return;
  }
  CFX_TxtBreak* pTxtBreak = m_pEngine->GetTextBreak();
  CFDE_TxtEdtBuf* pTxtBuf = m_pEngine->GetTextBuf();
  const FDE_TXTEDTPARAMS* pParam = m_pEngine->GetEditParams();
  FX_WCHAR wcAlias = 0;
  if (pParam->dwMode & FDE_TEXTEDITMODE_Password) {
    wcAlias = m_pEngine->GetAliasChar();
  }
  std::unique_ptr<IFX_CharIter> pIter(
      new CFDE_TxtEdtBufIter(static_cast<CFDE_TxtEdtBuf*>(pTxtBuf), wcAlias));
  pIter->SetAt(m_nCharStart);
  int32_t nEndIndex = m_nCharStart + m_nCharCount;
  CFX_ArrayTemplate<int32_t> LineBaseArr;
  FX_BOOL bReload = FALSE;
  uint32_t dwBreakStatus = FX_TXTBREAK_None;
  do {
    if (bReload) {
      dwBreakStatus = pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    } else {
      FX_WCHAR wAppend = pIter->GetChar();
      dwBreakStatus = pTxtBreak->AppendChar(wAppend);
    }
    if (pIter->GetAt() + 1 == nEndIndex &&
        dwBreakStatus < FX_TXTBREAK_LineBreak) {
      dwBreakStatus = pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    }
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      int32_t nCount = pTxtBreak->CountBreakPieces();
      int32_t nTotal = 0;
      for (int32_t j = 0; j < nCount; j++) {
        const CFX_TxtPiece* Piece = pTxtBreak->GetBreakPiece(j);
        nTotal += Piece->GetLength();
      }
      LineBaseArr.Add(nTotal);
      pTxtBreak->ClearBreakPieces();
    }
    if ((pIter->GetAt() + 1 == nEndIndex) &&
        (dwBreakStatus == FX_TXTBREAK_LineBreak)) {
      bReload = TRUE;
      pIter->Next(TRUE);
    }
  } while (pIter->Next(FALSE) && (pIter->GetAt() < nEndIndex));
  pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  pTxtBreak->ClearBreakPieces();
  int32_t nLineCount = LineBaseArr.GetSize();
  m_nLineCount = nLineCount;
  if (m_lpData) {
    m_lpData = FX_Realloc(int32_t, m_lpData, nLineCount + 1);
  } else {
    m_lpData = FX_Alloc(int32_t, nLineCount + 1);
  }
  int32_t* pIntArr = m_lpData;
  pIntArr[0] = 1;
  m_nLineCount = nLineCount;
  pIntArr++;
  for (int32_t j = 0; j < nLineCount; j++, pIntArr++) {
    *pIntArr = LineBaseArr[j];
  }
  LineBaseArr.RemoveAll();
}

void CFDE_TxtEdtParag::UnloadParag() {
  m_lpData[0]--;
  ASSERT(m_lpData[0] >= 0);
  if (m_lpData[0] == 0) {
    FX_Free(m_lpData);
    m_lpData = nullptr;
  }
}

void CFDE_TxtEdtParag::CalcLines() {
  CFX_TxtBreak* pTxtBreak = m_pEngine->GetTextBreak();
  CFDE_TxtEdtBuf* pTxtBuf = m_pEngine->GetTextBuf();
  int32_t nCount = 0;
  uint32_t dwBreakStatus = FX_TXTBREAK_None;
  int32_t nEndIndex = m_nCharStart + m_nCharCount;
  std::unique_ptr<IFX_CharIter> pIter(
      new CFDE_TxtEdtBufIter(static_cast<CFDE_TxtEdtBuf*>(pTxtBuf)));
  pIter->SetAt(m_nCharStart);
  FX_BOOL bReload = FALSE;
  do {
    if (bReload) {
      dwBreakStatus = pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    } else {
      FX_WCHAR wAppend = pIter->GetChar();
      dwBreakStatus = pTxtBreak->AppendChar(wAppend);
    }
    if (pIter->GetAt() + 1 == nEndIndex &&
        dwBreakStatus < FX_TXTBREAK_LineBreak) {
      dwBreakStatus = pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    }
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      nCount++;
      pTxtBreak->ClearBreakPieces();
    }
    if ((pIter->GetAt() + 1 == nEndIndex) &&
        (dwBreakStatus == FX_TXTBREAK_LineBreak)) {
      bReload = TRUE;
      pIter->Next(TRUE);
    }
  } while (pIter->Next(FALSE) && (pIter->GetAt() < nEndIndex));
  pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  pTxtBreak->ClearBreakPieces();
  m_nLineCount = nCount;
}

void CFDE_TxtEdtParag::GetLineRange(int32_t nLineIndex,
                                    int32_t& nStart,
                                    int32_t& nCount) const {
  int32_t* pLineBaseArr = m_lpData;
  ASSERT(nLineIndex < m_nLineCount);
  nStart = m_nCharStart;
  pLineBaseArr++;
  for (int32_t i = 0; i < nLineIndex; i++) {
    nStart += *pLineBaseArr;
    pLineBaseArr++;
  }
  nCount = *pLineBaseArr;
}
