// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedtpage.h"

#include <algorithm>

#include "xfa/fde/cfde_txtedtbuf.h"
#include "xfa/fde/cfde_txtedtbufiter.h"
#include "xfa/fde/cfde_txtedtengine.h"
#include "xfa/fde/cfde_txtedtparag.h"
#include "xfa/fde/ifde_txtedtengine.h"
#include "xfa/fde/ifde_txtedtpage.h"
#include "xfa/fde/cfde_txtedttextset.h"
#include "xfa/fde/cfx_wordbreak.h"

namespace {

const double kTolerance = 0.1f;

}  // namespace

IFDE_TxtEdtPage* IFDE_TxtEdtPage::Create(CFDE_TxtEdtEngine* pEngine,
                                         int32_t nIndex) {
  return new CFDE_TxtEdtPage(pEngine, nIndex);
}

CFDE_TxtEdtPage::CFDE_TxtEdtPage(CFDE_TxtEdtEngine* pEngine, int32_t nPageIndex)
    : m_pTextSet(nullptr),
      m_pBgnParag(nullptr),
      m_pEndParag(nullptr),
      m_nRefCount(0),
      m_nPageStart(-1),
      m_nCharCount(0),
      m_nPageIndex(nPageIndex),
      m_bLoaded(FALSE),
      m_pCharWidth(nullptr) {
  FXSYS_memset(&m_rtPage, 0, sizeof(CFX_RectF));
  FXSYS_memset(&m_rtPageMargin, 0, sizeof(CFX_RectF));
  FXSYS_memset(&m_rtPageContents, 0, sizeof(CFX_RectF));
  FXSYS_memset(&m_rtPageCanvas, 0, sizeof(CFX_RectF));
  m_pEditEngine = static_cast<CFDE_TxtEdtEngine*>(pEngine);
}

CFDE_TxtEdtPage::~CFDE_TxtEdtPage() {
  m_PieceMassArr.RemoveAll(TRUE);
  delete m_pTextSet;
  delete[] m_pCharWidth;
}

CFDE_TxtEdtEngine* CFDE_TxtEdtPage::GetEngine() const {
  return m_pEditEngine;
}

FDE_VISUALOBJTYPE CFDE_TxtEdtPage::GetType() {
  return FDE_VISUALOBJ_Text;
}

void CFDE_TxtEdtPage::GetRect(FDE_TEXTEDITPIECE* hVisualObj, CFX_RectF& rt) {}

int32_t CFDE_TxtEdtPage::GetCharRect(int32_t nIndex,
                                     CFX_RectF& rect,
                                     FX_BOOL bBBox) const {
  ASSERT(m_nRefCount > 0);
  ASSERT(nIndex >= 0 && nIndex < m_nCharCount);
  if (m_nRefCount < 1) {
    return 0;
  }
  int32_t nCount = m_PieceMassArr.GetSize();
  for (int32_t i = 0; i < nCount; i++) {
    const FDE_TEXTEDITPIECE* pPiece = m_PieceMassArr.GetPtrAt(i);
    if (nIndex >= pPiece->nStart &&
        nIndex < (pPiece->nStart + pPiece->nCount)) {
      CFX_RectFArray rectArr;
      m_pTextSet->GetCharRects(pPiece, rectArr, bBBox);
      rect = rectArr[nIndex - pPiece->nStart];
      return pPiece->nBidiLevel;
    }
  }
  ASSERT(0);
  return 0;
}

int32_t CFDE_TxtEdtPage::GetCharIndex(const CFX_PointF& fPoint,
                                      FX_BOOL& bBefore) {
  FX_BOOL bVertical = m_pEditEngine->GetEditParams()->dwLayoutStyles &
                      FDE_TEXTEDITLAYOUT_DocVertical;
  CFX_PointF ptF = fPoint;
  NormalizePt2Rect(ptF, m_rtPageContents, kTolerance);
  int32_t nCount = m_PieceMassArr.GetSize();
  CFX_RectF rtLine;
  int32_t nBgn = 0;
  int32_t nEnd = 0;
  FX_BOOL bInLine = FALSE;
  int32_t i = 0;
  for (i = 0; i < nCount; i++) {
    const FDE_TEXTEDITPIECE* pPiece = m_PieceMassArr.GetPtrAt(i);
    if (!bInLine && (bVertical ? (pPiece->rtPiece.left <= ptF.x &&
                                  pPiece->rtPiece.right() > ptF.x)
                               : (pPiece->rtPiece.top <= ptF.y &&
                                  pPiece->rtPiece.bottom() > ptF.y))) {
      nBgn = nEnd = i;
      rtLine = pPiece->rtPiece;
      bInLine = TRUE;
    } else if (bInLine) {
      if (bVertical ? (!(pPiece->rtPiece.left <= ptF.x &&
                         pPiece->rtPiece.right() > ptF.x))
                    : (pPiece->rtPiece.bottom() <= ptF.y ||
                       pPiece->rtPiece.top > ptF.y)) {
        nEnd = i - 1;
        break;
      } else {
        rtLine.Union(pPiece->rtPiece);
      }
    }
  }
  NormalizePt2Rect(ptF, rtLine, kTolerance);
  int32_t nCaret = 0;
  FDE_TEXTEDITPIECE* pPiece = nullptr;
  for (i = nBgn; i <= nEnd; i++) {
    pPiece = m_PieceMassArr.GetPtrAt(i);
    nCaret = m_nPageStart + pPiece->nStart;
    if (pPiece->rtPiece.Contains(ptF)) {
      CFX_RectFArray rectArr;
      m_pTextSet->GetCharRects(pPiece, rectArr, FALSE);
      int32_t nRtCount = rectArr.GetSize();
      for (int32_t j = 0; j < nRtCount; j++) {
        if (rectArr[j].Contains(ptF)) {
          nCaret = m_nPageStart + pPiece->nStart + j;
          if (nCaret >= m_pEditEngine->GetTextBufLength()) {
            bBefore = TRUE;
            return m_pEditEngine->GetTextBufLength();
          }
          FX_WCHAR wChar = m_pEditEngine->GetTextBuf()->GetCharByIndex(nCaret);
          if (wChar == L'\n' || wChar == L'\r') {
            if (wChar == L'\n') {
              if (m_pEditEngine->GetTextBuf()->GetCharByIndex(nCaret - 1) ==
                  L'\r') {
                nCaret--;
              }
            }
            bBefore = TRUE;
            return nCaret;
          }
          if (bVertical
                  ? (ptF.y > ((rectArr[j].top + rectArr[j].bottom()) / 2))
                  : (ptF.x > ((rectArr[j].left + rectArr[j].right()) / 2))) {
            bBefore = FX_IsOdd(pPiece->nBidiLevel);
          } else {
            bBefore = !FX_IsOdd(pPiece->nBidiLevel);
          }
          return nCaret;
        }
      }
    }
  }
  bBefore = TRUE;
  return nCaret;
}

int32_t CFDE_TxtEdtPage::GetCharStart() const {
  return m_nPageStart;
}

int32_t CFDE_TxtEdtPage::GetCharCount() const {
  return m_nCharCount;
}

int32_t CFDE_TxtEdtPage::GetDisplayPos(const CFX_RectF& rtClip,
                                       FXTEXT_CHARPOS*& pCharPos,
                                       CFX_RectF* pBBox) const {
  pCharPos = FX_Alloc(FXTEXT_CHARPOS, m_nCharCount);
  int32_t nCharPosCount = 0;
  FDE_TEXTEDITPIECE* pPiece = nullptr;
  int32_t nVisualObjCount = m_PieceMassArr.GetSize();
  FXTEXT_CHARPOS* pos = pCharPos;
  CFX_RectF rtObj;
  for (int32_t i = 0; i < nVisualObjCount; i++) {
    pPiece = m_PieceMassArr.GetPtrAt(i);
    m_pTextSet->GetRect(pPiece, rtObj);
    if (!rtClip.IntersectWith(rtObj)) {
      continue;
    }
    int32_t nCount = m_pTextSet->GetDisplayPos(pPiece, pos, FALSE);
    nCharPosCount += nCount;
    pos += nCount;
  }
  if ((nCharPosCount * 5) < (m_nCharCount << 2)) {
    FXTEXT_CHARPOS* pTemp = FX_Alloc(FXTEXT_CHARPOS, nCharPosCount);
    FXSYS_memcpy(pTemp, pCharPos, sizeof(FXTEXT_CHARPOS) * nCharPosCount);
    FX_Free(pCharPos);
    pCharPos = pTemp;
  }
  return nCharPosCount;
}

void CFDE_TxtEdtPage::CalcRangeRectArray(int32_t nStart,
                                         int32_t nCount,
                                         CFX_RectFArray& RectFArr) const {
  int32_t nPieceCount = m_PieceMassArr.GetSize();
  int32_t nEnd = nStart + nCount - 1;
  FX_BOOL bInRange = FALSE;
  for (int32_t i = 0; i < nPieceCount; i++) {
    FDE_TEXTEDITPIECE* piece = m_PieceMassArr.GetPtrAt(i);
    if (!bInRange) {
      if (nStart >= piece->nStart && nStart < (piece->nStart + piece->nCount)) {
        int32_t nRangeEnd = piece->nCount - 1;
        FX_BOOL bEnd = FALSE;
        if (nEnd >= piece->nStart && nEnd < (piece->nStart + piece->nCount)) {
          nRangeEnd = nEnd - piece->nStart;
          bEnd = TRUE;
        }
        CFX_RectFArray rcArr;
        m_pTextSet->GetCharRects(piece, rcArr, FALSE);
        CFX_RectF rectPiece = rcArr[nStart - piece->nStart];
        rectPiece.Union(rcArr[nRangeEnd]);
        RectFArr.Add(rectPiece);
        if (bEnd) {
          return;
        }
        bInRange = TRUE;
      }
    } else {
      if (nEnd >= piece->nStart && nEnd < (piece->nStart + piece->nCount)) {
        CFX_RectFArray rcArr;
        m_pTextSet->GetCharRects(piece, rcArr, FALSE);
        CFX_RectF rectPiece = rcArr[0];
        rectPiece.Union(rcArr[nEnd - piece->nStart]);
        RectFArr.Add(rectPiece);
        return;
      }
      RectFArr.Add(piece->rtPiece);
    }
  }
}

int32_t CFDE_TxtEdtPage::SelectWord(const CFX_PointF& fPoint, int32_t& nCount) {
  if (m_nRefCount < 0) {
    return -1;
  }
  CFDE_TxtEdtBuf* pBuf = m_pEditEngine->GetTextBuf();
  FX_BOOL bBefore;
  int32_t nIndex = GetCharIndex(fPoint, bBefore);
  if (nIndex == m_pEditEngine->GetTextBufLength()) {
    nIndex = m_pEditEngine->GetTextBufLength() - 1;
  }
  if (nIndex < 0) {
    return -1;
  }
  std::unique_ptr<CFX_WordBreak> pIter(new CFX_WordBreak);
  pIter->Attach(new CFDE_TxtEdtBufIter(pBuf));
  pIter->SetAt(nIndex);
  nCount = pIter->GetWordLength();
  return pIter->GetWordPos();
}

FX_BOOL CFDE_TxtEdtPage::IsLoaded(const CFX_RectF* pClipBox) {
  return m_bLoaded;
}

int32_t CFDE_TxtEdtPage::LoadPage(const CFX_RectF* pClipBox,
                                  IFX_Pause* pPause) {
  if (m_nRefCount > 0) {
    m_nRefCount++;
    return m_nRefCount;
  }
  CFDE_TxtEdtBuf* pBuf = m_pEditEngine->GetTextBuf();
  const FDE_TXTEDTPARAMS* pParams = m_pEditEngine->GetEditParams();
  FX_WCHAR wcAlias = 0;
  if (pParams->dwMode & FDE_TEXTEDITMODE_Password) {
    wcAlias = m_pEditEngine->GetAliasChar();
  }
  m_pIter.reset(
      new CFDE_TxtEdtBufIter(static_cast<CFDE_TxtEdtBuf*>(pBuf), wcAlias));
  CFX_TxtBreak* pBreak = m_pEditEngine->GetTextBreak();
  pBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  pBreak->ClearBreakPieces();
  int32_t nPageLineCount = m_pEditEngine->GetPageLineCount();
  int32_t nStartLine = nPageLineCount * m_nPageIndex;
  int32_t nEndLine = std::min((nStartLine + nPageLineCount - 1),
                              (m_pEditEngine->GetLineCount() - 1));
  int32_t nPageStart, nPageEnd, nTemp, nBgnParag, nStartLineInParag, nEndParag,
      nEndLineInParag;
  nBgnParag = m_pEditEngine->Line2Parag(0, 0, nStartLine, nStartLineInParag);
  m_pBgnParag =
      static_cast<CFDE_TxtEdtParag*>(m_pEditEngine->GetParag(nBgnParag));
  m_pBgnParag->LoadParag();
  m_pBgnParag->GetLineRange(nStartLine - nStartLineInParag, nPageStart, nTemp);
  nEndParag = m_pEditEngine->Line2Parag(nBgnParag, nStartLineInParag, nEndLine,
                                        nEndLineInParag);
  m_pEndParag =
      static_cast<CFDE_TxtEdtParag*>(m_pEditEngine->GetParag(nEndParag));
  m_pEndParag->LoadParag();
  m_pEndParag->GetLineRange(nEndLine - nEndLineInParag, nPageEnd, nTemp);
  nPageEnd += (nTemp - 1);
  FX_BOOL bVertial = pParams->dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical;
  FX_BOOL bLineReserve =
      pParams->dwLayoutStyles & FDE_TEXTEDITLAYOUT_LineReserve;
  FX_FLOAT fLineStart =
      bVertial
          ? (bLineReserve ? (pParams->fPlateWidth - pParams->fLineSpace) : 0.0f)
          : 0.0f;
  FX_FLOAT fLineStep =
      (bVertial && bLineReserve) ? (-pParams->fLineSpace) : pParams->fLineSpace;
  FX_FLOAT fLinePos = fLineStart;
  if (!m_pTextSet)
    m_pTextSet = new CFDE_TxtEdtTextSet(this);

  m_PieceMassArr.RemoveAll(TRUE);
  uint32_t dwBreakStatus = FX_TXTBREAK_None;
  int32_t nPieceStart = 0;
  delete[] m_pCharWidth;

  m_pCharWidth = new int32_t[nPageEnd - nPageStart + 1];
  pBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  pBreak->ClearBreakPieces();
  m_nPageStart = nPageStart;
  m_nCharCount = nPageEnd - nPageStart + 1;
  FX_BOOL bReload = FALSE;
  FX_FLOAT fDefCharWidth = 0;
  std::unique_ptr<IFX_CharIter> pIter(m_pIter->Clone());
  pIter->SetAt(nPageStart);
  m_pIter->SetAt(nPageStart);
  FX_BOOL bFirstPiece = TRUE;
  do {
    if (bReload) {
      dwBreakStatus = pBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    } else {
      FX_WCHAR wAppend = pIter->GetChar();
      dwBreakStatus = pBreak->AppendChar(wAppend);
    }
    if (pIter->GetAt() == nPageEnd && dwBreakStatus < FX_TXTBREAK_LineBreak) {
      dwBreakStatus = pBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
    }
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      int32_t nPieceCount = pBreak->CountBreakPieces();
      for (int32_t j = 0; j < nPieceCount; j++) {
        const CFX_TxtPiece* pPiece = pBreak->GetBreakPiece(j);
        FDE_TEXTEDITPIECE TxtEdtPiece;
        FXSYS_memset(&TxtEdtPiece, 0, sizeof(FDE_TEXTEDITPIECE));
        TxtEdtPiece.nBidiLevel = pPiece->m_iBidiLevel;
        TxtEdtPiece.nCount = pPiece->GetLength();
        TxtEdtPiece.nStart = nPieceStart;
        TxtEdtPiece.dwCharStyles = pPiece->m_dwCharStyles;
        if (FX_IsOdd(pPiece->m_iBidiLevel)) {
          TxtEdtPiece.dwCharStyles |= FX_TXTCHARSTYLE_OddBidiLevel;
        }
        FX_FLOAT fParaBreakWidth = 0.0f;
        if (pPiece->m_dwStatus > FX_TXTBREAK_PieceBreak) {
          FX_WCHAR wRtChar = pParams->wLineBreakChar;
          if (TxtEdtPiece.nCount >= 2) {
            FX_WCHAR wChar = pBuf->GetCharByIndex(
                m_nPageStart + TxtEdtPiece.nStart + TxtEdtPiece.nCount - 1);
            FX_WCHAR wCharPre = pBuf->GetCharByIndex(
                m_nPageStart + TxtEdtPiece.nStart + TxtEdtPiece.nCount - 2);
            if (wChar == wRtChar) {
              fParaBreakWidth += fDefCharWidth;
            }
            if (wCharPre == wRtChar) {
              fParaBreakWidth += fDefCharWidth;
            }
          } else if (TxtEdtPiece.nCount >= 1) {
            FX_WCHAR wChar = pBuf->GetCharByIndex(
                m_nPageStart + TxtEdtPiece.nStart + TxtEdtPiece.nCount - 1);
            if (wChar == wRtChar) {
              fParaBreakWidth += fDefCharWidth;
            }
          }
        }
        if (pParams->dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
          TxtEdtPiece.rtPiece.left = fLinePos;
          TxtEdtPiece.rtPiece.top = (FX_FLOAT)pPiece->m_iStartPos / 20000.0f;
          TxtEdtPiece.rtPiece.width = pParams->fLineSpace;
          TxtEdtPiece.rtPiece.height =
              (FX_FLOAT)pPiece->m_iWidth / 20000.0f + fParaBreakWidth;
        } else {
          TxtEdtPiece.rtPiece.left = (FX_FLOAT)pPiece->m_iStartPos / 20000.0f;
          TxtEdtPiece.rtPiece.top = fLinePos;
          TxtEdtPiece.rtPiece.width =
              (FX_FLOAT)pPiece->m_iWidth / 20000.0f + fParaBreakWidth;
          TxtEdtPiece.rtPiece.height = pParams->fLineSpace;
        }
        if (bFirstPiece) {
          m_rtPageContents = TxtEdtPiece.rtPiece;
          bFirstPiece = FALSE;
        } else {
          m_rtPageContents.Union(TxtEdtPiece.rtPiece);
        }
        nPieceStart += TxtEdtPiece.nCount;
        m_PieceMassArr.Add(TxtEdtPiece);
        for (int32_t k = 0; k < TxtEdtPiece.nCount; k++) {
          CFX_Char* ptc = pPiece->GetCharPtr(k);
          m_pCharWidth[TxtEdtPiece.nStart + k] = ptc->m_iCharWidth;
        }
      }
      fLinePos += fLineStep;
      pBreak->ClearBreakPieces();
    }
    if (pIter->GetAt() == nPageEnd && dwBreakStatus == FX_TXTBREAK_LineBreak) {
      bReload = TRUE;
      pIter->Next(TRUE);
    }
  } while (pIter->Next(FALSE) && (pIter->GetAt() <= nPageEnd));
  if (m_rtPageContents.left != 0) {
    FX_FLOAT fDelta = 0.0f;
    if (m_rtPageContents.width < pParams->fPlateWidth) {
      if (pParams->dwAlignment & FDE_TEXTEDITALIGN_Right) {
        fDelta = pParams->fPlateWidth - m_rtPageContents.width;
      } else if (pParams->dwAlignment & FDE_TEXTEDITALIGN_Center) {
        if ((pParams->dwLayoutStyles & FDE_TEXTEDITLAYOUT_CombText) &&
            m_nCharCount > 1) {
          int32_t nCount = m_nCharCount - 1;
          int32_t n = (m_pEditEngine->m_nLimit - nCount) / 2;
          fDelta = (m_rtPageContents.width / nCount) * n;
        } else {
          fDelta = (pParams->fPlateWidth - m_rtPageContents.width) / 2;
        }
      }
    }
    FX_FLOAT fOffset = m_rtPageContents.left - fDelta;
    int32_t nCount = m_PieceMassArr.GetSize();
    for (int32_t i = 0; i < nCount; i++) {
      FDE_TEXTEDITPIECE* pPiece = m_PieceMassArr.GetPtrAt(i);
      pPiece->rtPiece.Offset(-fOffset, 0.0f);
    }
    m_rtPageContents.Offset(-fOffset, 0.0f);
  }
  if (m_pEditEngine->GetEditParams()->dwLayoutStyles &
      FDE_TEXTEDITLAYOUT_LastLineHeight) {
    m_rtPageContents.height -= pParams->fLineSpace - pParams->fFontSize;
    int32_t nCount = m_PieceMassArr.GetSize();
    FDE_TEXTEDITPIECE* pPiece = m_PieceMassArr.GetPtrAt(nCount - 1);
    pPiece->rtPiece.height = pParams->fFontSize;
  }
  m_nRefCount = 1;
  m_bLoaded = TRUE;
  return 0;
}

void CFDE_TxtEdtPage::UnloadPage(const CFX_RectF* pClipBox) {
  ASSERT(m_nRefCount > 0);
  m_nRefCount--;
  if (m_nRefCount != 0)
    return;

  m_PieceMassArr.RemoveAll();
  delete m_pTextSet;
  m_pTextSet = nullptr;
  delete[] m_pCharWidth;
  m_pCharWidth = nullptr;
  if (m_pBgnParag) {
    m_pBgnParag->UnloadParag();
    m_pBgnParag = nullptr;
  }
  if (m_pEndParag) {
    m_pEndParag->UnloadParag();
    m_pEndParag = nullptr;
  }
  m_pIter.reset();
}

const CFX_RectF& CFDE_TxtEdtPage::GetContentsBox() {
  return m_rtPageContents;
}

FX_POSITION CFDE_TxtEdtPage::GetFirstPosition() {
  if (m_PieceMassArr.GetSize() < 1)
    return nullptr;
  return (FX_POSITION)1;
}

FDE_TEXTEDITPIECE* CFDE_TxtEdtPage::GetNext(FX_POSITION& pos,
                                            IFDE_VisualSet*& pVisualSet) {
  if (!m_pTextSet) {
    pos = nullptr;
    return nullptr;
  }
  int32_t nPos = (int32_t)(uintptr_t)pos;
  pVisualSet = m_pTextSet;
  if (nPos + 1 > m_PieceMassArr.GetSize()) {
    pos = nullptr;
  } else {
    pos = (FX_POSITION)(uintptr_t)(nPos + 1);
  }
  return m_PieceMassArr.GetPtrAt(nPos - 1);
}

FX_WCHAR CFDE_TxtEdtPage::GetChar(const FDE_TEXTEDITPIECE* pIdentity,
                                  int32_t index) const {
  int32_t nIndex = m_nPageStart + pIdentity->nStart + index;
  if (nIndex != m_pIter->GetAt()) {
    m_pIter->SetAt(nIndex);
  }
  FX_WCHAR wChar = m_pIter->GetChar();
  m_pIter->Next();
  return wChar;
}

int32_t CFDE_TxtEdtPage::GetWidth(const FDE_TEXTEDITPIECE* pIdentity,
                                  int32_t index) const {
  int32_t nWidth = m_pCharWidth[pIdentity->nStart + index];
  return nWidth;
}

void CFDE_TxtEdtPage::NormalizePt2Rect(CFX_PointF& ptF,
                                       const CFX_RectF& rtF,
                                       FX_FLOAT fTolerance) const {
  if (rtF.Contains(ptF.x, ptF.y)) {
    return;
  }
  if (ptF.x < rtF.left) {
    ptF.x = rtF.left;
  } else if (ptF.x >= rtF.right()) {
    ptF.x = rtF.right() - fTolerance;
  }
  if (ptF.y < rtF.top) {
    ptF.y = rtF.top;
  } else if (ptF.y >= rtF.bottom()) {
    ptF.y = rtF.bottom() - fTolerance;
  }
}
