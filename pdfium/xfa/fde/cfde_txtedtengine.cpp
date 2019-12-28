// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedtengine.h"

#include "xfa/fde/cfde_txtedtbuf.h"
#include "xfa/fde/cfde_txtedtbufiter.h"
#include "xfa/fde/ifx_chariter.h"
#include "xfa/fgas/layout/fgas_textbreak.h"
#include "xfa/fwl/basewidget/fwl_editimp.h"
#include "xfa/fde/cfde_txtedtdorecord_deleterange.h"
#include "xfa/fde/cfde_txtedtdorecord_insert.h"
#include "xfa/fde/cfde_txtedtparag.h"
#include "xfa/fde/cfde_txtedtpage.h"
#include "xfa/fde/tto/fde_textout.h"

namespace {

const uint32_t kPageWidthMax = 0xffff;
const uint32_t kUnicodeParagraphSeparator = 0x2029;

}  // namespace

FDE_TXTEDTPARAMS::FDE_TXTEDTPARAMS()
    : fPlateWidth(0),
      fPlateHeight(0),
      nLineCount(0),
      dwLayoutStyles(0),
      dwAlignment(0),
      dwMode(0),
      pFont(nullptr),
      fFontSize(10.0f),
      dwFontColor(0xff000000),
      fLineSpace(10.0f),
      fTabWidth(36),
      bTabEquidistant(FALSE),
      wDefChar(0xFEFF),
      wLineBreakChar('\n'),
      nCharRotation(0),
      nLineEnd(0),
      nHorzScale(100),
      fCharSpace(0),
      pEventSink(nullptr) {}

FDE_TXTEDT_TEXTCHANGE_INFO::FDE_TXTEDT_TEXTCHANGE_INFO() {}

FDE_TXTEDT_TEXTCHANGE_INFO::~FDE_TXTEDT_TEXTCHANGE_INFO() {}

CFDE_TxtEdtEngine::CFDE_TxtEdtEngine()
    : m_pTextBreak(nullptr),
      m_nPageLineCount(20),
      m_nLineCount(0),
      m_nAnchorPos(-1),
      m_nLayoutPos(0),
      m_fCaretPosReserve(0.0),
      m_nCaret(0),
      m_bBefore(TRUE),
      m_nCaretPage(0),
      m_dwFindFlags(0),
      m_bLock(FALSE),
      m_nLimit(0),
      m_wcAliasChar(L'*'),
      m_nFirstLineEnd(FDE_TXTEDIT_LINEEND_Auto),
      m_bAutoLineEnd(TRUE),
      m_wLineEnd(kUnicodeParagraphSeparator) {
  FXSYS_memset(&m_rtCaret, 0, sizeof(CFX_RectF));
  m_pTxtBuf = new CFDE_TxtEdtBuf();
  m_bAutoLineEnd = (m_Param.nLineEnd == FDE_TXTEDIT_LINEEND_Auto);
}

CFDE_TxtEdtEngine::~CFDE_TxtEdtEngine() {
  delete m_pTxtBuf;
  delete m_pTextBreak;
  RemoveAllParags();
  RemoveAllPages();
  m_Param.pEventSink = nullptr;
  ClearSelection();
}

void CFDE_TxtEdtEngine::SetEditParams(const FDE_TXTEDTPARAMS& params) {
  if (!m_pTextBreak)
    m_pTextBreak = new CFX_TxtBreak(FX_TXTBREAKPOLICY_None);

  FXSYS_memcpy(&m_Param, &params, sizeof(FDE_TXTEDTPARAMS));
  m_wLineEnd = params.wLineBreakChar;
  m_bAutoLineEnd = (m_Param.nLineEnd == FDE_TXTEDIT_LINEEND_Auto);
  UpdateTxtBreak();
}

FDE_TXTEDTPARAMS* CFDE_TxtEdtEngine::GetEditParams() {
  return &m_Param;
}

int32_t CFDE_TxtEdtEngine::CountPages() const {
  if (m_nLineCount == 0) {
    return 0;
  }
  return ((m_nLineCount - 1) / m_nPageLineCount) + 1;
}

IFDE_TxtEdtPage* CFDE_TxtEdtEngine::GetPage(int32_t nIndex) {
  if (m_PagePtrArray.GetSize() <= nIndex) {
    return nullptr;
  }
  return m_PagePtrArray[nIndex];
}

void CFDE_TxtEdtEngine::SetTextByStream(IFX_Stream* pStream) {
  ResetEngine();
  int32_t nIndex = 0;
  if (pStream && pStream->GetLength()) {
    int32_t nStreamLength = pStream->GetLength();
    FX_BOOL bValid = TRUE;
    if (m_nLimit > 0 && nStreamLength > m_nLimit) {
      bValid = FALSE;
    }
    FX_BOOL bPreIsCR = FALSE;
    if (bValid) {
      uint8_t bom[4];
      int32_t nPos = pStream->GetBOM(bom);
      pStream->Seek(FX_STREAMSEEK_Begin, nPos);
      int32_t nPlateSize = std::min(nStreamLength, m_pTxtBuf->GetChunkSize());
      FX_WCHAR* lpwstr = FX_Alloc(FX_WCHAR, nPlateSize);
      FX_BOOL bEos = false;
      while (!bEos) {
        int32_t nRead = pStream->ReadString(lpwstr, nPlateSize, bEos);
        bPreIsCR = ReplaceParagEnd(lpwstr, nRead, bPreIsCR);
        m_pTxtBuf->Insert(nIndex, lpwstr, nRead);
        nIndex += nRead;
      }
      FX_Free(lpwstr);
    }
  }
  m_pTxtBuf->Insert(nIndex, &m_wLineEnd, 1);
  RebuildParagraphs();
}

void CFDE_TxtEdtEngine::SetText(const CFX_WideString& wsText) {
  ResetEngine();
  int32_t nLength = wsText.GetLength();
  if (nLength > 0) {
    CFX_WideString wsTemp;
    FX_WCHAR* lpBuffer = wsTemp.GetBuffer(nLength);
    FXSYS_memcpy(lpBuffer, wsText.c_str(), nLength * sizeof(FX_WCHAR));
    ReplaceParagEnd(lpBuffer, nLength, FALSE);
    wsTemp.ReleaseBuffer(nLength);
    if (m_nLimit > 0 && nLength > m_nLimit) {
      wsTemp.Delete(m_nLimit, nLength - m_nLimit);
      nLength = m_nLimit;
    }
    m_pTxtBuf->SetText(wsTemp);
  }
  m_pTxtBuf->Insert(nLength, &m_wLineEnd, 1);
  RebuildParagraphs();
}

int32_t CFDE_TxtEdtEngine::GetTextLength() const {
  return GetTextBufLength();
}

void CFDE_TxtEdtEngine::GetText(CFX_WideString& wsText,
                                int32_t nStart,
                                int32_t nCount) {
  int32_t nTextBufLength = GetTextBufLength();
  if (nCount == -1) {
    nCount = nTextBufLength - nStart;
  }
  m_pTxtBuf->GetRange(wsText, nStart, nCount);
  RecoverParagEnd(wsText);
}

void CFDE_TxtEdtEngine::ClearText() {
  DeleteRange(0, -1);
}

int32_t CFDE_TxtEdtEngine::GetCaretRect(CFX_RectF& rtCaret) const {
  rtCaret = m_rtCaret;
  return m_nCaret;
}

int32_t CFDE_TxtEdtEngine::GetCaretPos() const {
  if (IsLocked()) {
    return 0;
  }
  return m_nCaret + (m_bBefore ? 0 : 1);
}

int32_t CFDE_TxtEdtEngine::SetCaretPos(int32_t nIndex, FX_BOOL bBefore) {
  if (IsLocked()) {
    return 0;
  }
  ASSERT(nIndex >= 0 && nIndex <= GetTextBufLength());
  if (m_PagePtrArray.GetSize() <= m_nCaretPage) {
    return 0;
  }
  m_bBefore = bBefore;
  m_nCaret = nIndex;
  MovePage2Char(m_nCaret);
  GetCaretRect(m_rtCaret, m_nCaretPage, m_nCaret, m_bBefore);
  if (!m_bBefore) {
    m_nCaret++;
    m_bBefore = TRUE;
  }
  m_fCaretPosReserve = (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical)
                           ? m_rtCaret.top
                           : m_rtCaret.left;
  m_Param.pEventSink->On_CaretChanged(this, m_nCaretPage, 0);
  m_nAnchorPos = -1;
  return m_nCaret;
}

int32_t CFDE_TxtEdtEngine::MoveCaretPos(FDE_TXTEDTMOVECARET eMoveCaret,
                                        FX_BOOL bShift,
                                        FX_BOOL bCtrl) {
  if (IsLocked()) {
    return 0;
  }
  if (m_PagePtrArray.GetSize() <= m_nCaretPage) {
    return 0;
  }
  FX_BOOL bSelChange = FALSE;
  if (IsSelect()) {
    ClearSelection();
    bSelChange = TRUE;
  }
  if (bShift) {
    if (m_nAnchorPos == -1) {
      m_nAnchorPos = m_nCaret;
    }
  } else {
    m_nAnchorPos = -1;
  }
  FX_BOOL bVertical = m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical;
  switch (eMoveCaret) {
    case MC_Left: {
      if (bVertical) {
        CFX_PointF ptCaret;
        if (MoveUp(ptCaret)) {
          UpdateCaretIndex(ptCaret);
        }
      } else {
        FX_BOOL bBefore = TRUE;
        int32_t nIndex = MoveBackward(bBefore);
        if (nIndex >= 0) {
          UpdateCaretRect(nIndex, bBefore);
        }
      }
    } break;
    case MC_Right: {
      if (bVertical) {
        CFX_PointF ptCaret;
        if (MoveDown(ptCaret)) {
          UpdateCaretIndex(ptCaret);
        }
      } else {
        FX_BOOL bBefore = TRUE;
        int32_t nIndex = MoveForward(bBefore);
        if (nIndex >= 0) {
          UpdateCaretRect(nIndex, bBefore);
        }
      }
    } break;
    case MC_Up: {
      if (bVertical) {
        FX_BOOL bBefore = TRUE;
        int32_t nIndex = MoveBackward(bBefore);
        if (nIndex >= 0) {
          UpdateCaretRect(nIndex, bBefore);
        }
      } else {
        CFX_PointF ptCaret;
        if (MoveUp(ptCaret)) {
          UpdateCaretIndex(ptCaret);
        }
      }
    } break;
    case MC_Down: {
      if (bVertical) {
        FX_BOOL bBefore = TRUE;
        int32_t nIndex = MoveForward(bBefore);
        if (nIndex >= 0) {
          UpdateCaretRect(nIndex, bBefore);
        }
      } else {
        CFX_PointF ptCaret;
        if (MoveDown(ptCaret)) {
          UpdateCaretIndex(ptCaret);
        }
      }
    } break;
    case MC_WordBackward:
      break;
    case MC_WordForward:
      break;
    case MC_LineStart:
      MoveLineStart();
      break;
    case MC_LineEnd:
      MoveLineEnd();
      break;
    case MC_ParagStart:
      MoveParagStart();
      break;
    case MC_ParagEnd:
      MoveParagEnd();
      break;
    case MC_PageDown:
      break;
    case MC_PageUp:
      break;
    case MC_Home:
      MoveHome();
      break;
    case MC_End:
      MoveEnd();
      break;
    default:
      break;
  }
  if (bShift && m_nAnchorPos != -1 && (m_nAnchorPos != m_nCaret)) {
    AddSelRange(std::min(m_nAnchorPos, m_nCaret),
                FXSYS_abs(m_nAnchorPos - m_nCaret));
    m_Param.pEventSink->On_SelChanged(this);
  }
  if (bSelChange) {
    m_Param.pEventSink->On_SelChanged(this);
  }
  return m_nCaret;
}

void CFDE_TxtEdtEngine::Lock() {
  m_bLock = TRUE;
}

void CFDE_TxtEdtEngine::Unlock() {
  m_bLock = FALSE;
}

FX_BOOL CFDE_TxtEdtEngine::IsLocked() const {
  return m_bLock;
}

int32_t CFDE_TxtEdtEngine::Insert(int32_t nStart,
                                  const FX_WCHAR* lpText,
                                  int32_t nLength) {
  if (IsLocked()) {
    return FDE_TXTEDT_MODIFY_RET_F_Locked;
  }
  CFX_WideString wsTemp;
  FX_WCHAR* lpBuffer = wsTemp.GetBuffer(nLength);
  FXSYS_memcpy(lpBuffer, lpText, nLength * sizeof(FX_WCHAR));
  ReplaceParagEnd(lpBuffer, nLength, FALSE);
  wsTemp.ReleaseBuffer(nLength);
  FX_BOOL bPart = FALSE;
  if (m_nLimit > 0) {
    int32_t nTotalLength = GetTextBufLength();
    int32_t nCount = m_SelRangePtrArr.GetSize();
    for (int32_t i = 0; i < nCount; i++) {
      FDE_TXTEDTSELRANGE* lpSelRange = m_SelRangePtrArr.GetAt(i);
      nTotalLength -= lpSelRange->nCount;
    }
    int32_t nExpectLength = nTotalLength + nLength;
    if (nTotalLength == m_nLimit) {
      return FDE_TXTEDT_MODIFY_RET_F_Full;
    }
    if (nExpectLength > m_nLimit) {
      nLength -= (nExpectLength - m_nLimit);
      bPart = TRUE;
    }
  }
  if ((m_Param.dwMode & FDE_TEXTEDITMODE_LimitArea_Vert) ||
      (m_Param.dwMode & FDE_TEXTEDITMODE_LimitArea_Horz)) {
    int32_t nTemp = nLength;
    if (m_Param.dwMode & FDE_TEXTEDITMODE_Password) {
      CFX_WideString wsText;
      while (nLength > 0) {
        GetPreInsertText(wsText, m_nCaret, lpBuffer, nLength);
        int32_t nTotal = wsText.GetLength();
        FX_WCHAR* lpBuf = wsText.GetBuffer(nTotal);
        for (int32_t i = 0; i < nTotal; i++) {
          lpBuf[i] = m_wcAliasChar;
        }
        wsText.ReleaseBuffer(nTotal);
        if (IsFitArea(wsText)) {
          break;
        }
        nLength--;
      }
    } else {
      CFX_WideString wsText;
      while (nLength > 0) {
        GetPreInsertText(wsText, m_nCaret, lpBuffer, nLength);
        if (IsFitArea(wsText)) {
          break;
        }
        nLength--;
      }
    }
    if (nLength == 0) {
      return FDE_TXTEDT_MODIFY_RET_F_Full;
    }
    if (nLength < nTemp) {
      bPart = TRUE;
    }
  }
  if (m_Param.dwMode & FDE_TEXTEDITMODE_Validate) {
    CFX_WideString wsText;
    GetPreInsertText(wsText, m_nCaret, lpBuffer, nLength);
    if (!m_Param.pEventSink->On_Validate(this, wsText)) {
      return FDE_TXTEDT_MODIFY_RET_F_Invalidate;
    }
  }
  if (IsSelect()) {
    DeleteSelect();
  }
  if (!(m_Param.dwMode & FDE_TEXTEDITMODE_NoRedoUndo))
    m_Param.pEventSink->On_AddDoRecord(
        this,
        new CFDE_TxtEdtDoRecord_Insert(this, m_nCaret, lpBuffer, nLength));

  GetText(m_ChangeInfo.wsPrevText, 0);
  Inner_Insert(m_nCaret, lpBuffer, nLength);
  m_ChangeInfo.nChangeType = FDE_TXTEDT_TEXTCHANGE_TYPE_Insert;
  m_ChangeInfo.wsInsert = CFX_WideString(lpBuffer, nLength);
  nStart = m_nCaret;
  nStart += nLength;
  FX_WCHAR wChar = m_pTxtBuf->GetCharByIndex(nStart - 1);
  FX_BOOL bBefore = TRUE;
  if (wChar != L'\n' && wChar != L'\r') {
    nStart--;
    bBefore = FALSE;
  }
  SetCaretPos(nStart, bBefore);
  m_Param.pEventSink->On_TextChanged(this, m_ChangeInfo);
  return bPart ? FDE_TXTEDT_MODIFY_RET_S_Part : FDE_TXTEDT_MODIFY_RET_S_Normal;
}

int32_t CFDE_TxtEdtEngine::Delete(int32_t nStart, FX_BOOL bBackspace) {
  if (IsLocked()) {
    return FDE_TXTEDT_MODIFY_RET_F_Locked;
  }
  if (IsSelect()) {
    DeleteSelect();
    return FDE_TXTEDT_MODIFY_RET_S_Normal;
  }

  int32_t nCount = 1;
  if (bBackspace) {
    if (nStart == 0) {
      return FDE_TXTEDT_MODIFY_RET_F_Boundary;
    }
    if (nStart > 2 && m_pTxtBuf->GetCharByIndex(nStart - 1) == L'\n' &&
        m_pTxtBuf->GetCharByIndex(nStart - 2) == L'\r') {
      nStart--;
      nCount++;
    }
    nStart--;
  } else {
    if (nStart == GetTextBufLength()) {
      return FDE_TXTEDT_MODIFY_RET_F_Full;
    }
    if ((nStart + 1 < GetTextBufLength()) &&
        (m_pTxtBuf->GetCharByIndex(nStart) == L'\r') &&
        (m_pTxtBuf->GetCharByIndex(nStart + 1) == L'\n')) {
      nCount++;
    }
  }
  if (m_Param.dwMode & FDE_TEXTEDITMODE_Validate) {
    CFX_WideString wsText;
    GetPreDeleteText(wsText, nStart, nCount);
    if (!m_Param.pEventSink->On_Validate(this, wsText)) {
      return FDE_TXTEDT_MODIFY_RET_F_Invalidate;
    }
  }
  if (!(m_Param.dwMode & FDE_TEXTEDITMODE_NoRedoUndo)) {
    CFX_WideString wsRange;
    m_pTxtBuf->GetRange(wsRange, nStart, nCount);
    m_Param.pEventSink->On_AddDoRecord(
        this,
        new CFDE_TxtEdtDoRecord_DeleteRange(this, nStart, m_nCaret, wsRange));
  }
  m_ChangeInfo.nChangeType = FDE_TXTEDT_TEXTCHANGE_TYPE_Delete;
  GetText(m_ChangeInfo.wsDelete, nStart, nCount);
  Inner_DeleteRange(nStart, nCount);
  SetCaretPos(nStart + ((!bBackspace && nStart > 0) ? -1 : 0),
              (bBackspace || nStart == 0));
  m_Param.pEventSink->On_TextChanged(this, m_ChangeInfo);
  return FDE_TXTEDT_MODIFY_RET_S_Normal;
}

int32_t CFDE_TxtEdtEngine::DeleteRange(int32_t nStart, int32_t nCount) {
  if (IsLocked()) {
    return FDE_TXTEDT_MODIFY_RET_F_Locked;
  }
  if (nCount == -1) {
    nCount = GetTextBufLength();
  }
  if (nCount == 0) {
    return FDE_TXTEDT_MODIFY_RET_S_Normal;
  }
  if (m_Param.dwMode & FDE_TEXTEDITMODE_Validate) {
    CFX_WideString wsText;
    GetPreDeleteText(wsText, nStart, nCount);
    if (!m_Param.pEventSink->On_Validate(this, wsText)) {
      return FDE_TXTEDT_MODIFY_RET_F_Invalidate;
    }
  }
  DeleteRange_DoRecord(nStart, nCount);
  m_Param.pEventSink->On_TextChanged(this, m_ChangeInfo);
  SetCaretPos(nStart, TRUE);
  return FDE_TXTEDT_MODIFY_RET_S_Normal;
}

int32_t CFDE_TxtEdtEngine::Replace(int32_t nStart,
                                   int32_t nLength,
                                   const CFX_WideString& wsReplace) {
  if (IsLocked()) {
    return FDE_TXTEDT_MODIFY_RET_F_Locked;
  }
  if (nStart < 0 || (nStart + nLength > GetTextBufLength())) {
    return FDE_TXTEDT_MODIFY_RET_F_Boundary;
  }
  if (m_Param.dwMode & FDE_TEXTEDITMODE_Validate) {
    CFX_WideString wsText;
    GetPreReplaceText(wsText, nStart, nLength, wsReplace.c_str(),
                      wsReplace.GetLength());
    if (!m_Param.pEventSink->On_Validate(this, wsText)) {
      return FDE_TXTEDT_MODIFY_RET_F_Invalidate;
    }
  }
  if (IsSelect()) {
    ClearSelection();
  }
  m_ChangeInfo.nChangeType = FDE_TXTEDT_TEXTCHANGE_TYPE_Replace;
  GetText(m_ChangeInfo.wsDelete, nStart, nLength);
  if (nLength > 0) {
    Inner_DeleteRange(nStart, nLength);
  }
  int32_t nTextLength = wsReplace.GetLength();
  if (nTextLength > 0) {
    Inner_Insert(nStart, wsReplace.c_str(), nTextLength);
  }
  m_ChangeInfo.wsInsert = CFX_WideString(wsReplace.c_str(), nTextLength);
  nStart += nTextLength;
  FX_WCHAR wChar = m_pTxtBuf->GetCharByIndex(nStart - 1);
  FX_BOOL bBefore = TRUE;
  if (wChar != L'\n' && wChar != L'\r') {
    nStart--;
    bBefore = FALSE;
  }
  SetCaretPos(nStart, bBefore);
  m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
  m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
  m_Param.pEventSink->On_TextChanged(this, m_ChangeInfo);
  return FDE_TXTEDT_MODIFY_RET_S_Normal;
}

void CFDE_TxtEdtEngine::SetLimit(int32_t nLimit) {
  m_nLimit = nLimit;
}

void CFDE_TxtEdtEngine::SetAliasChar(FX_WCHAR wcAlias) {
  m_wcAliasChar = wcAlias;
}

void CFDE_TxtEdtEngine::RemoveSelRange(int32_t nStart, int32_t nCount) {
  FDE_TXTEDTSELRANGE* lpTemp = nullptr;
  int32_t nRangeCount = m_SelRangePtrArr.GetSize();
  int32_t i = 0;
  for (i = 0; i < nRangeCount; i++) {
    lpTemp = m_SelRangePtrArr[i];
    if (lpTemp->nStart == nStart && lpTemp->nCount == nCount) {
      delete lpTemp;
      m_SelRangePtrArr.RemoveAt(i);
      return;
    }
  }
}

void CFDE_TxtEdtEngine::AddSelRange(int32_t nStart, int32_t nCount) {
  if (nCount == -1) {
    nCount = GetTextLength() - nStart;
  }
  int32_t nSize = m_SelRangePtrArr.GetSize();
  if (nSize <= 0) {
    FDE_TXTEDTSELRANGE* lpSelRange = new FDE_TXTEDTSELRANGE;
    lpSelRange->nStart = nStart;
    lpSelRange->nCount = nCount;
    m_SelRangePtrArr.Add(lpSelRange);
    m_Param.pEventSink->On_SelChanged(this);
    return;
  }
  FDE_TXTEDTSELRANGE* lpTemp = nullptr;
  lpTemp = m_SelRangePtrArr[nSize - 1];
  if (nStart >= lpTemp->nStart + lpTemp->nCount) {
    FDE_TXTEDTSELRANGE* lpSelRange = new FDE_TXTEDTSELRANGE;
    lpSelRange->nStart = nStart;
    lpSelRange->nCount = nCount;
    m_SelRangePtrArr.Add(lpSelRange);
    m_Param.pEventSink->On_SelChanged(this);
    return;
  }
  int32_t nEnd = nStart + nCount - 1;
  FX_BOOL bBegin = FALSE;
  int32_t nRangeBgn = 0;
  int32_t nRangeCnt = 0;
  for (int32_t i = 0; i < nSize; i++) {
    lpTemp = m_SelRangePtrArr[i];
    int32_t nTempBgn = lpTemp->nStart;
    int32_t nTempEnd = nTempBgn + lpTemp->nCount - 1;
    if (bBegin) {
      if (nEnd < nTempBgn) {
        break;
      } else if (nStart >= nTempBgn && nStart <= nTempEnd) {
        nRangeCnt++;
        break;
      }
      nRangeCnt++;
    } else {
      if (nStart <= nTempEnd) {
        nRangeBgn = i;
        if (nEnd < nTempBgn) {
          break;
        }
        nRangeCnt = 1;
        bBegin = TRUE;
      }
    }
  }
  if (nRangeCnt == 0) {
    FDE_TXTEDTSELRANGE* lpSelRange = new FDE_TXTEDTSELRANGE;
    lpSelRange->nStart = nStart;
    lpSelRange->nCount = nCount;
    m_SelRangePtrArr.InsertAt(nRangeBgn, lpSelRange);
  } else {
    lpTemp = m_SelRangePtrArr[nRangeBgn];
    lpTemp->nStart = nStart;
    lpTemp->nCount = nCount;
    nRangeCnt--;
    nRangeBgn++;
    while (nRangeCnt--) {
      delete m_SelRangePtrArr[nRangeBgn];
      m_SelRangePtrArr.RemoveAt(nRangeBgn);
    }
  }
  m_Param.pEventSink->On_SelChanged(this);
}

int32_t CFDE_TxtEdtEngine::CountSelRanges() {
  return m_SelRangePtrArr.GetSize();
}

int32_t CFDE_TxtEdtEngine::GetSelRange(int32_t nIndex, int32_t& nStart) {
  nStart = m_SelRangePtrArr[nIndex]->nStart;
  return m_SelRangePtrArr[nIndex]->nCount;
}

void CFDE_TxtEdtEngine::ClearSelection() {
  int32_t nCount = m_SelRangePtrArr.GetSize();
  for (int i = 0; i < nCount; ++i)
    delete m_SelRangePtrArr[i];
  m_SelRangePtrArr.RemoveAll();
  if (nCount && m_Param.pEventSink)
    m_Param.pEventSink->On_SelChanged(this);
}

FX_BOOL CFDE_TxtEdtEngine::Redo(const IFDE_TxtEdtDoRecord* pDoRecord) {
  if (IsLocked())
    return FALSE;
  if (m_Param.dwMode & FDE_TEXTEDITMODE_NoRedoUndo)
    return FALSE;
  return pDoRecord->Redo();
}

FX_BOOL CFDE_TxtEdtEngine::Undo(const IFDE_TxtEdtDoRecord* pDoRecord) {
  if (IsLocked())
    return FALSE;
  if (m_Param.dwMode & FDE_TEXTEDITMODE_NoRedoUndo)
    return FALSE;
  return pDoRecord->Undo();
}

int32_t CFDE_TxtEdtEngine::StartLayout() {
  Lock();
  RemoveAllPages();
  m_nLayoutPos = 0;
  m_nLineCount = 0;
  return 0;
}

int32_t CFDE_TxtEdtEngine::DoLayout(IFX_Pause* pPause) {
  int32_t nCount = m_ParagPtrArray.GetSize();
  CFDE_TxtEdtParag* pParag = nullptr;
  int32_t nLineCount = 0;
  for (; m_nLayoutPos < nCount; m_nLayoutPos++) {
    pParag = m_ParagPtrArray[m_nLayoutPos];
    pParag->CalcLines();
    nLineCount += pParag->GetLineCount();
    if (nLineCount > m_nPageLineCount && pPause && pPause->NeedToPauseNow()) {
      m_nLineCount += nLineCount;
      return (++m_nLayoutPos * 100) / nCount;
    }
  }
  m_nLineCount += nLineCount;
  return 100;
}

void CFDE_TxtEdtEngine::EndLayout() {
  UpdatePages();
  int32_t nLength = GetTextLength();
  if (m_nCaret > nLength) {
    m_nCaret = nLength;
  }
  int32_t nIndex = m_nCaret;
  if (!m_bBefore) {
    nIndex--;
  }
  m_rtCaret.Set(0, 0, 1, m_Param.fFontSize);
  Unlock();
}

FX_BOOL CFDE_TxtEdtEngine::Optimize(IFX_Pause* pPause) {
  return m_pTxtBuf->Optimize(pPause);
}

CFDE_TxtEdtBuf* CFDE_TxtEdtEngine::GetTextBuf() const {
  return m_pTxtBuf;
}

int32_t CFDE_TxtEdtEngine::GetTextBufLength() const {
  return m_pTxtBuf->GetTextLength() - 1;
}

CFX_TxtBreak* CFDE_TxtEdtEngine::GetTextBreak() const {
  return m_pTextBreak;
}

int32_t CFDE_TxtEdtEngine::GetLineCount() const {
  return m_nLineCount;
}

int32_t CFDE_TxtEdtEngine::GetPageLineCount() const {
  return m_nPageLineCount;
}

int32_t CFDE_TxtEdtEngine::CountParags() const {
  return m_ParagPtrArray.GetSize();
}

CFDE_TxtEdtParag* CFDE_TxtEdtEngine::GetParag(int32_t nParagIndex) const {
  return m_ParagPtrArray[nParagIndex];
}

IFX_CharIter* CFDE_TxtEdtEngine::CreateCharIter() {
  if (!m_pTxtBuf) {
    return nullptr;
  }
  return new CFDE_TxtEdtBufIter(static_cast<CFDE_TxtEdtBuf*>(m_pTxtBuf));
}

int32_t CFDE_TxtEdtEngine::Line2Parag(int32_t nStartParag,
                                      int32_t nStartLineofParag,
                                      int32_t nLineIndex,
                                      int32_t& nStartLine) const {
  int32_t nLineTotal = nStartLineofParag;
  int32_t nCount = m_ParagPtrArray.GetSize();
  CFDE_TxtEdtParag* pParag = nullptr;
  int32_t i = nStartParag;
  for (; i < nCount; i++) {
    pParag = m_ParagPtrArray[i];
    nLineTotal += pParag->GetLineCount();
    if (nLineTotal > nLineIndex) {
      break;
    }
  }
  nStartLine = nLineTotal - pParag->GetLineCount();
  return i;
}

void CFDE_TxtEdtEngine::GetPreDeleteText(CFX_WideString& wsText,
                                         int32_t nIndex,
                                         int32_t nLength) {
  GetText(wsText, 0, GetTextBufLength());
  wsText.Delete(nIndex, nLength);
}

void CFDE_TxtEdtEngine::GetPreInsertText(CFX_WideString& wsText,
                                         int32_t nIndex,
                                         const FX_WCHAR* lpText,
                                         int32_t nLength) {
  GetText(wsText, 0, GetTextBufLength());
  int32_t nSelIndex = 0;
  int32_t nSelLength = 0;
  int32_t nSelCount = CountSelRanges();
  while (nSelCount--) {
    nSelLength = GetSelRange(nSelCount, nSelIndex);
    wsText.Delete(nSelIndex, nSelLength);
    nIndex = nSelIndex;
  }
  CFX_WideString wsTemp;
  int32_t nOldLength = wsText.GetLength();
  const FX_WCHAR* pOldBuffer = wsText.c_str();
  FX_WCHAR* lpBuffer = wsTemp.GetBuffer(nOldLength + nLength);
  FXSYS_memcpy(lpBuffer, pOldBuffer, (nIndex) * sizeof(FX_WCHAR));
  FXSYS_memcpy(lpBuffer + nIndex, lpText, nLength * sizeof(FX_WCHAR));
  FXSYS_memcpy(lpBuffer + nIndex + nLength, pOldBuffer + nIndex,
               (nOldLength - nIndex) * sizeof(FX_WCHAR));
  wsTemp.ReleaseBuffer(nOldLength + nLength);
  wsText = wsTemp;
}

void CFDE_TxtEdtEngine::GetPreReplaceText(CFX_WideString& wsText,
                                          int32_t nIndex,
                                          int32_t nOriginLength,
                                          const FX_WCHAR* lpText,
                                          int32_t nLength) {
  GetText(wsText, 0, GetTextBufLength());
  int32_t nSelIndex = 0;
  int32_t nSelLength = 0;
  int32_t nSelCount = CountSelRanges();
  while (nSelCount--) {
    nSelLength = GetSelRange(nSelCount, nSelIndex);
    wsText.Delete(nSelIndex, nSelLength);
  }
  wsText.Delete(nIndex, nOriginLength);
  int32_t i = 0;
  for (i = 0; i < nLength; i++) {
    wsText.Insert(nIndex++, lpText[i]);
  }
}

void CFDE_TxtEdtEngine::Inner_Insert(int32_t nStart,
                                     const FX_WCHAR* lpText,
                                     int32_t nLength) {
  ASSERT(nLength > 0);
  FDE_TXTEDTPARAGPOS ParagPos;
  TextPos2ParagPos(nStart, ParagPos);
  m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
  int32_t nParagCount = m_ParagPtrArray.GetSize();
  int32_t i = 0;
  for (i = ParagPos.nParagIndex + 1; i < nParagCount; i++)
    m_ParagPtrArray[i]->IncrementStartIndex(nLength);

  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPos.nParagIndex];
  int32_t nReserveLineCount = pParag->GetLineCount();
  int32_t nReserveCharStart = pParag->GetStartIndex();
  int32_t nLeavePart = ParagPos.nCharIndex;
  int32_t nCutPart = pParag->GetTextLength() - ParagPos.nCharIndex;
  int32_t nTextStart = 0;
  FX_WCHAR wCurChar = L' ';
  const FX_WCHAR* lpPos = lpText;
  FX_BOOL bFirst = TRUE;
  int32_t nParagIndex = ParagPos.nParagIndex;
  for (i = 0; i < nLength; i++, lpPos++) {
    wCurChar = *lpPos;
    if (wCurChar == m_wLineEnd) {
      if (bFirst) {
        pParag->SetTextLength(nLeavePart + (i - nTextStart + 1));
        pParag->SetLineCount(-1);
        nReserveCharStart += pParag->GetTextLength();
        bFirst = FALSE;
      } else {
        pParag = new CFDE_TxtEdtParag(this);
        pParag->SetLineCount(-1);
        pParag->SetTextLength(i - nTextStart + 1);
        pParag->SetStartIndex(nReserveCharStart);
        m_ParagPtrArray.InsertAt(++nParagIndex, pParag);
        nReserveCharStart += pParag->GetTextLength();
      }
      nTextStart = i + 1;
    }
  }
  if (bFirst) {
    pParag->IncrementTextLength(nLength);
    pParag->SetLineCount(-1);
    bFirst = FALSE;
  } else {
    pParag = new CFDE_TxtEdtParag(this);
    pParag->SetLineCount(-1);
    pParag->SetTextLength(nLength - nTextStart + nCutPart);
    pParag->SetStartIndex(nReserveCharStart);
    m_ParagPtrArray.InsertAt(++nParagIndex, pParag);
  }
  m_pTxtBuf->Insert(nStart, lpText, nLength);
  int32_t nTotalLineCount = 0;
  for (i = ParagPos.nParagIndex; i <= nParagIndex; i++) {
    pParag = m_ParagPtrArray[i];
    pParag->CalcLines();
    nTotalLineCount += pParag->GetLineCount();
  }
  m_nLineCount += nTotalLineCount - nReserveLineCount;
  m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
  UpdatePages();
}

void CFDE_TxtEdtEngine::Inner_DeleteRange(int32_t nStart, int32_t nCount) {
  if (nCount == -1) {
    nCount = m_pTxtBuf->GetTextLength() - nStart;
  }
  int32_t nEnd = nStart + nCount - 1;
  ASSERT(nStart >= 0 && nEnd < m_pTxtBuf->GetTextLength());
  m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
  FDE_TXTEDTPARAGPOS ParagPosBgn, ParagPosEnd;
  TextPos2ParagPos(nStart, ParagPosBgn);
  TextPos2ParagPos(nEnd, ParagPosEnd);
  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPosEnd.nParagIndex];
  FX_BOOL bLastParag = FALSE;
  if (ParagPosEnd.nCharIndex == pParag->GetTextLength() - 1) {
    if (ParagPosEnd.nParagIndex < m_ParagPtrArray.GetSize() - 1) {
      ParagPosEnd.nParagIndex++;
    } else {
      bLastParag = TRUE;
    }
  }
  int32_t nTotalLineCount = 0;
  int32_t nTotalCharCount = 0;
  int32_t i = 0;
  for (i = ParagPosBgn.nParagIndex; i <= ParagPosEnd.nParagIndex; i++) {
    CFDE_TxtEdtParag* pTextParag = m_ParagPtrArray[i];
    pTextParag->CalcLines();
    nTotalLineCount += pTextParag->GetLineCount();
    nTotalCharCount += pTextParag->GetTextLength();
  }
  m_pTxtBuf->Delete(nStart, nCount);
  int32_t nNextParagIndex = (ParagPosBgn.nCharIndex == 0 && bLastParag)
                                ? ParagPosBgn.nParagIndex
                                : (ParagPosBgn.nParagIndex + 1);
  for (i = nNextParagIndex; i <= ParagPosEnd.nParagIndex; i++) {
    delete m_ParagPtrArray[nNextParagIndex];
    m_ParagPtrArray.RemoveAt(nNextParagIndex);
  }
  if (!(bLastParag && ParagPosBgn.nCharIndex == 0)) {
    pParag = m_ParagPtrArray[ParagPosBgn.nParagIndex];
    pParag->SetTextLength(nTotalCharCount - nCount);
    pParag->CalcLines();
    nTotalLineCount -= pParag->GetTextLength();
  }
  int32_t nParagCount = m_ParagPtrArray.GetSize();
  for (i = nNextParagIndex; i < nParagCount; i++)
    m_ParagPtrArray[i]->DecrementStartIndex(nCount);

  m_nLineCount -= nTotalLineCount;
  UpdatePages();
  int32_t nPageCount = CountPages();
  if (m_nCaretPage >= nPageCount) {
    m_nCaretPage = nPageCount - 1;
  }
  m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
}

void CFDE_TxtEdtEngine::DeleteRange_DoRecord(int32_t nStart,
                                             int32_t nCount,
                                             FX_BOOL bSel) {
  ASSERT(nStart >= 0);
  if (nCount == -1) {
    nCount = GetTextLength() - nStart;
  }
  ASSERT((nStart + nCount) <= m_pTxtBuf->GetTextLength());

  if (!(m_Param.dwMode & FDE_TEXTEDITMODE_NoRedoUndo)) {
    CFX_WideString wsRange;
    m_pTxtBuf->GetRange(wsRange, nStart, nCount);
    m_Param.pEventSink->On_AddDoRecord(
        this, new CFDE_TxtEdtDoRecord_DeleteRange(this, nStart, m_nCaret,
                                                  wsRange, bSel));
  }
  m_ChangeInfo.nChangeType = FDE_TXTEDT_TEXTCHANGE_TYPE_Delete;
  GetText(m_ChangeInfo.wsDelete, nStart, nCount);
  Inner_DeleteRange(nStart, nCount);
}

void CFDE_TxtEdtEngine::ResetEngine() {
  RemoveAllPages();
  RemoveAllParags();
  ClearSelection();
  m_nCaret = 0;
  m_pTxtBuf->Clear(FALSE);
  m_nCaret = 0;
}

void CFDE_TxtEdtEngine::RebuildParagraphs() {
  RemoveAllParags();
  FX_WCHAR wChar = L' ';
  int32_t nParagStart = 0;
  int32_t nIndex = 0;
  std::unique_ptr<IFX_CharIter> pIter(
      new CFDE_TxtEdtBufIter(static_cast<CFDE_TxtEdtBuf*>(m_pTxtBuf)));
  pIter->SetAt(0);
  do {
    wChar = pIter->GetChar();
    nIndex = pIter->GetAt();
    if (wChar == m_wLineEnd) {
      CFDE_TxtEdtParag* pParag = new CFDE_TxtEdtParag(this);
      pParag->SetStartIndex(nParagStart);
      pParag->SetTextLength(nIndex - nParagStart + 1);
      pParag->SetLineCount(-1);
      m_ParagPtrArray.Add(pParag);
      nParagStart = nIndex + 1;
    }
  } while (pIter->Next());
}

void CFDE_TxtEdtEngine::RemoveAllParags() {
  for (int32_t i = 0; i < m_ParagPtrArray.GetSize(); ++i)
    delete m_ParagPtrArray[i];
  m_ParagPtrArray.RemoveAll();
}

void CFDE_TxtEdtEngine::RemoveAllPages() {
  for (int32_t i = 0; i < m_PagePtrArray.GetSize(); i++)
    delete m_PagePtrArray[i];
  m_PagePtrArray.RemoveAll();
}

void CFDE_TxtEdtEngine::UpdateParags() {
  int32_t nCount = m_ParagPtrArray.GetSize();
  if (nCount == 0) {
    return;
  }
  CFDE_TxtEdtParag* pParag = nullptr;
  int32_t nLineCount = 0;
  int32_t i = 0;
  for (i = 0; i < nCount; i++) {
    pParag = m_ParagPtrArray[i];
    if (pParag->GetLineCount() == -1)
      pParag->CalcLines();

    nLineCount += pParag->GetLineCount();
  }
  m_nLineCount = nLineCount;
}

void CFDE_TxtEdtEngine::UpdatePages() {
  if (m_nLineCount == 0)
    return;

  int32_t nPageCount = (m_nLineCount - 1) / (m_nPageLineCount) + 1;
  int32_t nSize = m_PagePtrArray.GetSize();
  if (nSize == nPageCount)
    return;

  if (nSize > nPageCount) {
    for (int32_t i = nSize - 1; i >= nPageCount; i--) {
      delete m_PagePtrArray[i];
      m_PagePtrArray.RemoveAt(i);
    }
    return;
  }
  if (nSize < nPageCount) {
    for (int32_t i = nSize; i < nPageCount; i++)
      m_PagePtrArray.Add(IFDE_TxtEdtPage::Create(this, i));
    return;
  }
}

void CFDE_TxtEdtEngine::UpdateTxtBreak() {
  uint32_t dwStyle = m_pTextBreak->GetLayoutStyles();
  if (m_Param.dwMode & FDE_TEXTEDITMODE_MultiLines) {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_SingleLine;
  } else {
    dwStyle |= FX_TXTLAYOUTSTYLE_SingleLine;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
    dwStyle |= FX_TXTLAYOUTSTYLE_VerticalLayout;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_VerticalLayout;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_LineReserve) {
    dwStyle |= FX_TXTLAYOUTSTYLE_ReverseLine;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_ReverseLine;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_RTL) {
    dwStyle |= FX_TXTLAYOUTSTYLE_RTLReadingOrder;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_RTLReadingOrder;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_CombText) {
    dwStyle |= FX_TXTLAYOUTSTYLE_CombText;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_CombText;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_CharVertial) {
    dwStyle |= FX_TXTLAYOUTSTYLE_VerticalChars;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_VerticalChars;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_ExpandTab) {
    dwStyle |= FX_TXTLAYOUTSTYLE_ExpandTab;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_ExpandTab;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_ArabicContext) {
    dwStyle |= FX_TXTLAYOUTSTYLE_ArabicContext;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_ArabicContext;
  }
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_ArabicShapes) {
    dwStyle |= FX_TXTLAYOUTSTYLE_ArabicShapes;
  } else {
    dwStyle &= ~FX_TXTLAYOUTSTYLE_ArabicShapes;
  }
  m_pTextBreak->SetLayoutStyles(dwStyle);
  uint32_t dwAligment = 0;
  if (m_Param.dwAlignment & FDE_TEXTEDITALIGN_Justified) {
    dwAligment |= FX_TXTLINEALIGNMENT_Justified;
  } else if (m_Param.dwAlignment & FDE_TEXTEDITALIGN_Distributed) {
    dwAligment |= FX_TXTLINEALIGNMENT_Distributed;
  }
  if (m_Param.dwAlignment & FDE_TEXTEDITALIGN_Center) {
    dwAligment |= FX_TXTLINEALIGNMENT_Center;
  } else if (m_Param.dwAlignment & FDE_TEXTEDITALIGN_Right) {
    dwAligment |= FX_TXTLINEALIGNMENT_Right;
  }
  m_pTextBreak->SetAlignment(dwAligment);
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
    if (m_Param.dwMode & FDE_TEXTEDITMODE_AutoLineWrap) {
      m_pTextBreak->SetLineWidth(m_Param.fPlateHeight);
    } else {
      m_pTextBreak->SetLineWidth(kPageWidthMax);
    }
  } else {
    if (m_Param.dwMode & FDE_TEXTEDITMODE_AutoLineWrap) {
      m_pTextBreak->SetLineWidth(m_Param.fPlateWidth);
    } else {
      m_pTextBreak->SetLineWidth(kPageWidthMax);
    }
  }
  m_nPageLineCount = m_Param.nLineCount;
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_CombText) {
    FX_FLOAT fCombWidth =
        m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical
            ? m_Param.fPlateHeight
            : m_Param.fPlateWidth;
    if (m_nLimit > 0) {
      fCombWidth /= m_nLimit;
    }
    m_pTextBreak->SetCombWidth(fCombWidth);
  }
  m_pTextBreak->SetFont(m_Param.pFont);
  m_pTextBreak->SetFontSize(m_Param.fFontSize);
  m_pTextBreak->SetTabWidth(m_Param.fTabWidth, m_Param.bTabEquidistant);
  m_pTextBreak->SetDefaultChar(m_Param.wDefChar);
  m_pTextBreak->SetParagraphBreakChar(m_Param.wLineBreakChar);
  m_pTextBreak->SetCharRotation(m_Param.nCharRotation);
  m_pTextBreak->SetLineBreakTolerance(m_Param.fFontSize * 0.2f);
  m_pTextBreak->SetHorizontalScale(m_Param.nHorzScale);
  m_pTextBreak->SetCharSpace(m_Param.fCharSpace);
}

FX_BOOL CFDE_TxtEdtEngine::ReplaceParagEnd(FX_WCHAR*& lpText,
                                           int32_t& nLength,
                                           FX_BOOL bPreIsCR) {
  for (int32_t i = 0; i < nLength; i++) {
    FX_WCHAR wc = lpText[i];
    switch (wc) {
      case L'\r': {
        lpText[i] = m_wLineEnd;
        bPreIsCR = TRUE;
      } break;
      case L'\n': {
        if (bPreIsCR == TRUE) {
          int32_t nNext = i + 1;
          if (nNext < nLength) {
            FXSYS_memmove(lpText + i, lpText + nNext,
                          (nLength - nNext) * sizeof(FX_WCHAR));
          }
          i--;
          nLength--;
          bPreIsCR = FALSE;
          if (m_bAutoLineEnd) {
            m_nFirstLineEnd = FDE_TXTEDIT_LINEEND_CRLF;
            m_bAutoLineEnd = FALSE;
          }
        } else {
          lpText[i] = m_wLineEnd;
          if (m_bAutoLineEnd) {
            m_nFirstLineEnd = FDE_TXTEDIT_LINEEND_LF;
            m_bAutoLineEnd = FALSE;
          }
        }
      } break;
      default: {
        if (bPreIsCR && m_bAutoLineEnd) {
          m_nFirstLineEnd = FDE_TXTEDIT_LINEEND_CR;
          m_bAutoLineEnd = FALSE;
        }
        bPreIsCR = FALSE;
      } break;
    }
  }
  return bPreIsCR;
}

void CFDE_TxtEdtEngine::RecoverParagEnd(CFX_WideString& wsText) {
  FX_WCHAR wc = (m_nFirstLineEnd == FDE_TXTEDIT_LINEEND_CR) ? L'\n' : L'\r';
  if (m_nFirstLineEnd == FDE_TXTEDIT_LINEEND_CRLF) {
    CFX_ArrayTemplate<int32_t> PosArr;
    int32_t nLength = wsText.GetLength();
    int32_t i = 0;
    FX_WCHAR* lpPos = const_cast<FX_WCHAR*>(wsText.c_str());
    for (i = 0; i < nLength; i++, lpPos++) {
      if (*lpPos == m_wLineEnd) {
        *lpPos = wc;
        PosArr.Add(i);
      }
    }
    const FX_WCHAR* lpSrcBuf = wsText.c_str();
    CFX_WideString wsTemp;
    int32_t nCount = PosArr.GetSize();
    FX_WCHAR* lpDstBuf = wsTemp.GetBuffer(nLength + nCount);
    int32_t nDstPos = 0;
    int32_t nSrcPos = 0;
    for (i = 0; i < nCount; i++) {
      int32_t nPos = PosArr[i];
      int32_t nCopyLen = nPos - nSrcPos + 1;
      FXSYS_memcpy(lpDstBuf + nDstPos, lpSrcBuf + nSrcPos,
                   nCopyLen * sizeof(FX_WCHAR));
      nDstPos += nCopyLen;
      nSrcPos += nCopyLen;
      lpDstBuf[nDstPos] = L'\n';
      nDstPos++;
    }
    if (nSrcPos < nLength) {
      FXSYS_memcpy(lpDstBuf + nDstPos, lpSrcBuf + nSrcPos,
                   (nLength - nSrcPos) * sizeof(FX_WCHAR));
    }
    wsTemp.ReleaseBuffer(nLength + nCount);
    wsText = wsTemp;
  } else {
    int32_t nLength = wsText.GetLength();
    FX_WCHAR* lpBuf = const_cast<FX_WCHAR*>(wsText.c_str());
    for (int32_t i = 0; i < nLength; i++, lpBuf++) {
      if (*lpBuf == m_wLineEnd)
        *lpBuf = wc;
    }
  }
}

int32_t CFDE_TxtEdtEngine::MovePage2Char(int32_t nIndex) {
  ASSERT(nIndex >= 0);
  ASSERT(nIndex <= m_pTxtBuf->GetTextLength());
  if (m_nCaretPage >= 0) {
    IFDE_TxtEdtPage* pPage = m_PagePtrArray[m_nCaretPage];
    m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
    int32_t nPageCharStart = pPage->GetCharStart();
    int32_t nPageCharCount = pPage->GetCharCount();
    if (nIndex >= nPageCharStart && nIndex < nPageCharStart + nPageCharCount) {
      m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
      return m_nCaretPage;
    }
    m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
  }
  CFDE_TxtEdtParag* pParag = nullptr;
  int32_t nLineCount = 0;
  int32_t nParagCount = m_ParagPtrArray.GetSize();
  int32_t i = 0;
  for (i = 0; i < nParagCount; i++) {
    pParag = m_ParagPtrArray[i];
    if (pParag->GetStartIndex() <= nIndex &&
        nIndex < (pParag->GetStartIndex() + pParag->GetTextLength())) {
      break;
    }
    nLineCount += pParag->GetLineCount();
  }
  pParag->LoadParag();
  int32_t nLineStart = -1;
  int32_t nLineCharCount = -1;
  for (i = 0; i < pParag->GetLineCount(); i++) {
    pParag->GetLineRange(i, nLineStart, nLineCharCount);
    if (nLineStart <= nIndex && nIndex < (nLineStart + nLineCharCount))
      break;
  }
  ASSERT(i < pParag->GetLineCount());
  nLineCount += (i + 1);
  m_nCaretPage = (nLineCount - 1) / m_nPageLineCount + 1 - 1;
  pParag->UnloadParag();
  return m_nCaretPage;
}

void CFDE_TxtEdtEngine::TextPos2ParagPos(int32_t nIndex,
                                         FDE_TXTEDTPARAGPOS& ParagPos) const {
  ASSERT(nIndex >= 0 && nIndex < m_pTxtBuf->GetTextLength());
  int32_t nCount = m_ParagPtrArray.GetSize();
  int32_t nBgn = 0;
  int32_t nMid = 0;
  int32_t nEnd = nCount - 1;
  while (nEnd > nBgn) {
    nMid = (nBgn + nEnd) / 2;
    CFDE_TxtEdtParag* pParag = m_ParagPtrArray[nMid];
    if (nIndex < pParag->GetStartIndex())
      nEnd = nMid - 1;
    else if (nIndex >= (pParag->GetStartIndex() + pParag->GetTextLength()))
      nBgn = nMid + 1;
    else
      break;
  }
  if (nBgn == nEnd)
    nMid = nBgn;

  ASSERT(nIndex >= m_ParagPtrArray[nMid]->GetStartIndex() &&
         (nIndex < m_ParagPtrArray[nMid]->GetStartIndex() +
                       m_ParagPtrArray[nMid]->GetTextLength()));
  ParagPos.nParagIndex = nMid;
  ParagPos.nCharIndex = nIndex - m_ParagPtrArray[nMid]->GetStartIndex();
}

int32_t CFDE_TxtEdtEngine::MoveForward(FX_BOOL& bBefore) {
  if (m_nCaret == m_pTxtBuf->GetTextLength() - 1)
    return -1;

  int32_t nCaret = m_nCaret;
  if ((nCaret + 1 < m_pTxtBuf->GetTextLength()) &&
      (m_pTxtBuf->GetCharByIndex(nCaret) == L'\r') &&
      (m_pTxtBuf->GetCharByIndex(nCaret + 1) == L'\n')) {
    nCaret++;
  }
  nCaret++;
  bBefore = TRUE;
  return nCaret;
}

int32_t CFDE_TxtEdtEngine::MoveBackward(FX_BOOL& bBefore) {
  if (m_nCaret == 0)
    return FALSE;

  int32_t nCaret = m_nCaret;
  if (nCaret > 2 && m_pTxtBuf->GetCharByIndex(nCaret - 1) == L'\n' &&
      m_pTxtBuf->GetCharByIndex(nCaret - 2) == L'\r') {
    nCaret--;
  }
  nCaret--;
  bBefore = TRUE;
  return nCaret;
}

FX_BOOL CFDE_TxtEdtEngine::MoveUp(CFX_PointF& ptCaret) {
  IFDE_TxtEdtPage* pPage = GetPage(m_nCaretPage);
  const CFX_RectF& rtContent = pPage->GetContentsBox();
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
    ptCaret.x = m_rtCaret.left + m_rtCaret.width / 2 - m_Param.fLineSpace;
    ptCaret.y = m_fCaretPosReserve;
    FX_BOOL bLineReserve =
        m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_LineReserve;
    if (ptCaret.x < rtContent.left) {
      if (bLineReserve) {
        if (m_nCaretPage == CountPages() - 1) {
          return FALSE;
        }
      } else {
        if (m_nCaretPage == 0) {
          return FALSE;
        }
      }
      if (bLineReserve) {
        m_nCaretPage++;
      } else {
        m_nCaretPage--;
      }
      ptCaret.x -= rtContent.left;
      IFDE_TxtEdtPage* pCurPage = GetPage(m_nCaretPage);
      ptCaret.x += pCurPage->GetContentsBox().right();
    }
  } else {
    ptCaret.x = m_fCaretPosReserve;
    ptCaret.y = m_rtCaret.top + m_rtCaret.height / 2 - m_Param.fLineSpace;
    if (ptCaret.y < rtContent.top) {
      if (m_nCaretPage == 0) {
        return FALSE;
      }
      ptCaret.y -= rtContent.top;
      m_nCaretPage--;
      IFDE_TxtEdtPage* pCurPage = GetPage(m_nCaretPage);
      ptCaret.y += pCurPage->GetContentsBox().bottom();
    }
  }
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveDown(CFX_PointF& ptCaret) {
  IFDE_TxtEdtPage* pPage = GetPage(m_nCaretPage);
  const CFX_RectF& rtContent = pPage->GetContentsBox();
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
    ptCaret.x = m_rtCaret.left + m_rtCaret.width / 2 + m_Param.fLineSpace;
    ptCaret.y = m_fCaretPosReserve;
    if (ptCaret.x >= rtContent.right()) {
      FX_BOOL bLineReserve =
          m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_LineReserve;
      if (bLineReserve) {
        if (m_nCaretPage == 0) {
          return FALSE;
        }
      } else {
        if (m_nCaretPage == CountPages() - 1) {
          return FALSE;
        }
      }
      if (bLineReserve) {
        m_nCaretPage--;
      } else {
        m_nCaretPage++;
      }
      ptCaret.x -= rtContent.right();
      IFDE_TxtEdtPage* pCurPage = GetPage(m_nCaretPage);
      ptCaret.x += pCurPage->GetContentsBox().left;
    }
  } else {
    ptCaret.x = m_fCaretPosReserve;
    ptCaret.y = m_rtCaret.top + m_rtCaret.height / 2 + m_Param.fLineSpace;
    if (ptCaret.y >= rtContent.bottom()) {
      if (m_nCaretPage == CountPages() - 1) {
        return FALSE;
      }
      ptCaret.y -= rtContent.bottom();
      m_nCaretPage++;
      IFDE_TxtEdtPage* pCurPage = GetPage(m_nCaretPage);
      ptCaret.y += pCurPage->GetContentsBox().top;
    }
  }
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveLineStart() {
  int32_t nIndex = m_bBefore ? m_nCaret : m_nCaret - 1;
  FDE_TXTEDTPARAGPOS ParagPos;
  TextPos2ParagPos(nIndex, ParagPos);
  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPos.nParagIndex];
  pParag->LoadParag();
  int32_t nLineCount = pParag->GetLineCount();
  int32_t i = 0;
  int32_t nStart = 0;
  int32_t nCount = 0;
  for (; i < nLineCount; i++) {
    pParag->GetLineRange(i, nStart, nCount);
    if (nIndex >= nStart && nIndex < nStart + nCount) {
      break;
    }
  }
  UpdateCaretRect(nStart, TRUE);
  pParag->UnloadParag();
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveLineEnd() {
  int32_t nIndex = m_bBefore ? m_nCaret : m_nCaret - 1;
  FDE_TXTEDTPARAGPOS ParagPos;
  TextPos2ParagPos(nIndex, ParagPos);
  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPos.nParagIndex];
  pParag->LoadParag();
  int32_t nLineCount = pParag->GetLineCount();
  int32_t i = 0;
  int32_t nStart = 0;
  int32_t nCount = 0;
  for (; i < nLineCount; i++) {
    pParag->GetLineRange(i, nStart, nCount);
    if (nIndex >= nStart && nIndex < nStart + nCount) {
      break;
    }
  }
  nIndex = nStart + nCount - 1;
  ASSERT(nIndex <= GetTextBufLength());
  FX_WCHAR wChar = m_pTxtBuf->GetCharByIndex(nIndex);
  FX_BOOL bBefore = FALSE;
  if (nIndex <= GetTextBufLength()) {
    if (wChar == L'\r') {
      bBefore = TRUE;
    } else if (wChar == L'\n' && nIndex > nStart) {
      bBefore = TRUE;
      nIndex--;
      wChar = m_pTxtBuf->GetCharByIndex(nIndex);
      if (wChar != L'\r') {
        nIndex++;
      }
    }
  }
  UpdateCaretRect(nIndex, bBefore);
  pParag->UnloadParag();
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveParagStart() {
  int32_t nIndex = m_bBefore ? m_nCaret : m_nCaret - 1;
  FDE_TXTEDTPARAGPOS ParagPos;
  TextPos2ParagPos(nIndex, ParagPos);
  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPos.nParagIndex];
  UpdateCaretRect(pParag->GetStartIndex(), TRUE);
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveParagEnd() {
  int32_t nIndex = m_bBefore ? m_nCaret : m_nCaret - 1;
  FDE_TXTEDTPARAGPOS ParagPos;
  TextPos2ParagPos(nIndex, ParagPos);
  CFDE_TxtEdtParag* pParag = m_ParagPtrArray[ParagPos.nParagIndex];
  nIndex = pParag->GetStartIndex() + pParag->GetTextLength() - 1;
  FX_WCHAR wChar = m_pTxtBuf->GetCharByIndex(nIndex);
  if (wChar == L'\n' && nIndex > 0) {
    nIndex--;
    wChar = m_pTxtBuf->GetCharByIndex(nIndex);
    if (wChar != L'\r') {
      nIndex++;
    }
  }
  UpdateCaretRect(nIndex, TRUE);
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveHome() {
  UpdateCaretRect(0, TRUE);
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::MoveEnd() {
  UpdateCaretRect(GetTextBufLength(), TRUE);
  return TRUE;
}

FX_BOOL CFDE_TxtEdtEngine::IsFitArea(CFX_WideString& wsText) {
  std::unique_ptr<CFDE_TextOut> pTextOut(new CFDE_TextOut);
  pTextOut->SetLineSpace(m_Param.fLineSpace);
  pTextOut->SetFont(m_Param.pFont);
  pTextOut->SetFontSize(m_Param.fFontSize);
  CFX_RectF rcText;
  FXSYS_memset(&rcText, 0, sizeof(rcText));
  uint32_t dwStyle = 0;
  if (!(m_Param.dwMode & FDE_TEXTEDITMODE_MultiLines))
    dwStyle |= FDE_TTOSTYLE_SingleLine;

  if (m_Param.dwMode & FDE_TEXTEDITMODE_AutoLineWrap) {
    dwStyle |= FDE_TTOSTYLE_LineWrap;
    rcText.width = m_Param.fPlateWidth;
  } else {
    rcText.width = 65535;
  }
  pTextOut->SetStyles(dwStyle);
  wsText += L"\n";
  pTextOut->CalcLogicSize(wsText.c_str(), wsText.GetLength(), rcText);
  wsText.Delete(wsText.GetLength() - 1);
  if ((m_Param.dwMode & FDE_TEXTEDITMODE_LimitArea_Horz) &&
      (rcText.width > m_Param.fPlateWidth)) {
    return FALSE;
  }
  if ((m_Param.dwMode & FDE_TEXTEDITMODE_LimitArea_Vert) &&
      (rcText.height > m_Param.fLineSpace * m_Param.nLineCount)) {
    return FALSE;
  }
  return TRUE;
}

void CFDE_TxtEdtEngine::UpdateCaretRect(int32_t nIndex, FX_BOOL bBefore) {
  MovePage2Char(nIndex);
  GetCaretRect(m_rtCaret, m_nCaretPage, nIndex, bBefore);
  m_nCaret = nIndex;
  m_bBefore = bBefore;
  if (!m_bBefore) {
    m_nCaret++;
    m_bBefore = TRUE;
  }
  m_fCaretPosReserve = (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical)
                           ? m_rtCaret.top
                           : m_rtCaret.left;
  m_Param.pEventSink->On_CaretChanged(this, m_nCaretPage, 0);
}

void CFDE_TxtEdtEngine::GetCaretRect(CFX_RectF& rtCaret,
                                     int32_t nPageIndex,
                                     int32_t nCaret,
                                     FX_BOOL bBefore) {
  IFDE_TxtEdtPage* pPage = m_PagePtrArray[m_nCaretPage];
  m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
  FX_BOOL bCombText = m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_CombText;
  int32_t nIndexInpage = nCaret - pPage->GetCharStart();
  if (bBefore && bCombText && nIndexInpage > 0) {
    nIndexInpage--;
    bBefore = FALSE;
  }
  int32_t nBIDILevel = pPage->GetCharRect(nIndexInpage, rtCaret, bCombText);
  if (m_Param.dwLayoutStyles & FDE_TEXTEDITLAYOUT_DocVertical) {
    if ((!FX_IsOdd(nBIDILevel) && !bBefore) ||
        (FX_IsOdd(nBIDILevel) && bBefore)) {
      rtCaret.Offset(0, rtCaret.height - 1.0f);
    }
    if (rtCaret.height == 0 && rtCaret.top > 1.0f)
      rtCaret.top -= 1.0f;

    rtCaret.height = 1.0f;
  } else {
    if ((!FX_IsOdd(nBIDILevel) && !bBefore) ||
        (FX_IsOdd(nBIDILevel) && bBefore)) {
      rtCaret.Offset(rtCaret.width - 1.0f, 0);
    }
    if (rtCaret.width == 0 && rtCaret.left > 1.0f)
      rtCaret.left -= 1.0f;

    rtCaret.width = 1.0f;
  }
  m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
}

void CFDE_TxtEdtEngine::UpdateCaretIndex(const CFX_PointF& ptCaret) {
  IFDE_TxtEdtPage* pPage = m_PagePtrArray[m_nCaretPage];
  m_Param.pEventSink->On_PageLoad(this, m_nCaretPage, 0);
  m_nCaret = pPage->GetCharIndex(ptCaret, m_bBefore);
  GetCaretRect(m_rtCaret, m_nCaretPage, m_nCaret, m_bBefore);
  if (!m_bBefore) {
    m_nCaret++;
    m_bBefore = TRUE;
  }
  m_Param.pEventSink->On_CaretChanged(this, m_nCaretPage);
  m_Param.pEventSink->On_PageUnload(this, m_nCaretPage, 0);
}

FX_BOOL CFDE_TxtEdtEngine::IsSelect() {
  return m_SelRangePtrArr.GetSize() > 0;
}

void CFDE_TxtEdtEngine::DeleteSelect() {
  int32_t nCountRange = CountSelRanges();
  if (nCountRange > 0) {
    int32_t nSelStart = 0;
    while (nCountRange > 0) {
      int32_t nSelCount = GetSelRange(--nCountRange, nSelStart);
      delete m_SelRangePtrArr[nCountRange];
      m_SelRangePtrArr.RemoveAt(nCountRange);
      DeleteRange_DoRecord(nSelStart, nSelCount, TRUE);
    }
    ClearSelection();
    m_Param.pEventSink->On_TextChanged(this, m_ChangeInfo);
    m_Param.pEventSink->On_SelChanged(this);
    SetCaretPos(nSelStart, TRUE);
    return;
  }
}
