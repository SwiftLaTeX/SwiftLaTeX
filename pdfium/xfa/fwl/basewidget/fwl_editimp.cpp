// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_editimp.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "third_party/base/stl_util.h"
#include "xfa/fde/cfde_txtedtengine.h"
#include "xfa/fde/fde_gedevice.h"
#include "xfa/fde/fde_render.h"
#include "xfa/fde/ifde_txtedtpage.h"
#include "xfa/fgas/font/fgas_gefont.h"
#include "xfa/fwl/basewidget/fwl_caretimp.h"
#include "xfa/fwl/basewidget/fwl_comboboximp.h"
#include "xfa/fwl/basewidget/fwl_scrollbarimp.h"
#include "xfa/fwl/basewidget/ifwl_caret.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themepart.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/cfx_path.h"

namespace {

const int kEditMargin = 3;

bool FX_EDIT_ISLATINWORD(FX_WCHAR c) {
  return c == 0x2D || (c <= 0x005A && c >= 0x0041) ||
         (c <= 0x007A && c >= 0x0061) || (c <= 0x02AF && c >= 0x00C0) ||
         c == 0x0027;
}

void AddSquigglyPath(CFX_Path* pPathData,
                     FX_FLOAT fStartX,
                     FX_FLOAT fEndX,
                     FX_FLOAT fY,
                     FX_FLOAT fStep) {
  pPathData->MoveTo(fStartX, fY);
  int i = 1;
  for (FX_FLOAT fx = fStartX + fStep; fx < fEndX; fx += fStep, ++i) {
    pPathData->LineTo(fx, fY + (i & 1) * fStep);
  }
}

}  // namespace

// static
IFWL_Edit* IFWL_Edit::Create(const CFWL_WidgetImpProperties& properties,
                             IFWL_Widget* pOuter) {
  IFWL_Edit* pEdit = new IFWL_Edit;
  CFWL_EditImp* pEditImpl = new CFWL_EditImp(properties, pOuter);
  pEdit->SetImpl(pEditImpl);
  pEditImpl->SetInterface(pEdit);
  return pEdit;
}
// static
IFWL_Edit* IFWL_Edit::CreateComboEdit(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter) {
  IFWL_Edit* pEdit = new IFWL_Edit;
  CFWL_EditImp* pComboEditImpl = new CFWL_ComboEditImp(properties, pOuter);
  pEdit->SetImpl(pComboEditImpl);
  pComboEditImpl->SetInterface(pEdit);
  return pEdit;
}
IFWL_Edit::IFWL_Edit() {}
FWL_Error IFWL_Edit::SetText(const CFX_WideString& wsText) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetText(wsText);
}
int32_t IFWL_Edit::GetTextLength() const {
  return static_cast<CFWL_EditImp*>(GetImpl())->GetTextLength();
}
FWL_Error IFWL_Edit::GetText(CFX_WideString& wsText,
                             int32_t nStart,
                             int32_t nCount) const {
  return static_cast<CFWL_EditImp*>(GetImpl())->GetText(wsText, nStart, nCount);
}
FWL_Error IFWL_Edit::ClearText() {
  return static_cast<CFWL_EditImp*>(GetImpl())->ClearText();
}
int32_t IFWL_Edit::GetCaretPos() const {
  return static_cast<CFWL_EditImp*>(GetImpl())->GetCaretPos();
}
int32_t IFWL_Edit::SetCaretPos(int32_t nIndex, FX_BOOL bBefore) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetCaretPos(nIndex, bBefore);
}
FWL_Error IFWL_Edit::AddSelRange(int32_t nStart, int32_t nCount) {
  return static_cast<CFWL_EditImp*>(GetImpl())->AddSelRange(nStart, nCount);
}
int32_t IFWL_Edit::CountSelRanges() {
  return static_cast<CFWL_EditImp*>(GetImpl())->CountSelRanges();
}
int32_t IFWL_Edit::GetSelRange(int32_t nIndex, int32_t& nStart) {
  return static_cast<CFWL_EditImp*>(GetImpl())->GetSelRange(nIndex, nStart);
}
FWL_Error IFWL_Edit::ClearSelections() {
  return static_cast<CFWL_EditImp*>(GetImpl())->ClearSelections();
}
int32_t IFWL_Edit::GetLimit() {
  return static_cast<CFWL_EditImp*>(GetImpl())->GetLimit();
}
FWL_Error IFWL_Edit::SetLimit(int32_t nLimit) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetLimit(nLimit);
}
FWL_Error IFWL_Edit::SetAliasChar(FX_WCHAR wAlias) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetAliasChar(wAlias);
}
FWL_Error IFWL_Edit::Insert(int32_t nStart,
                            const FX_WCHAR* lpText,
                            int32_t nLen) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Insert(nStart, lpText, nLen);
}
FWL_Error IFWL_Edit::DeleteSelections() {
  return static_cast<CFWL_EditImp*>(GetImpl())->DeleteSelections();
}
FWL_Error IFWL_Edit::DeleteRange(int32_t nStart, int32_t nCount) {
  return static_cast<CFWL_EditImp*>(GetImpl())->DeleteRange(nStart, nCount);
}
FWL_Error IFWL_Edit::Replace(int32_t nStart,
                             int32_t nLen,
                             const CFX_WideStringC& wsReplace) {
  return static_cast<CFWL_EditImp*>(GetImpl())
      ->Replace(nStart, nLen, wsReplace);
}
FWL_Error IFWL_Edit::DoClipboard(int32_t iCmd) {
  return static_cast<CFWL_EditImp*>(GetImpl())->DoClipboard(iCmd);
}
FX_BOOL IFWL_Edit::Copy(CFX_WideString& wsCopy) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Copy(wsCopy);
}
FX_BOOL IFWL_Edit::Cut(CFX_WideString& wsCut) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Cut(wsCut);
}
FX_BOOL IFWL_Edit::Paste(const CFX_WideString& wsPaste) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Paste(wsPaste);
}
FX_BOOL IFWL_Edit::Delete() {
  return static_cast<CFWL_EditImp*>(GetImpl())->Delete();
}
FX_BOOL IFWL_Edit::Redo(const IFDE_TxtEdtDoRecord* pRecord) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Redo(pRecord);
}
FX_BOOL IFWL_Edit::Undo(const IFDE_TxtEdtDoRecord* pRecord) {
  return static_cast<CFWL_EditImp*>(GetImpl())->Undo(pRecord);
}
FX_BOOL IFWL_Edit::Undo() {
  return static_cast<CFWL_EditImp*>(GetImpl())->Undo();
}
FX_BOOL IFWL_Edit::Redo() {
  return static_cast<CFWL_EditImp*>(GetImpl())->Redo();
}
FX_BOOL IFWL_Edit::CanUndo() {
  return static_cast<CFWL_EditImp*>(GetImpl())->CanUndo();
}
FX_BOOL IFWL_Edit::CanRedo() {
  return static_cast<CFWL_EditImp*>(GetImpl())->CanRedo();
}
FWL_Error IFWL_Edit::SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant) {
  return static_cast<CFWL_EditImp*>(GetImpl())
      ->SetTabWidth(fTabWidth, bEquidistant);
}
FWL_Error IFWL_Edit::SetOuter(IFWL_Widget* pOuter) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetOuter(pOuter);
}
FWL_Error IFWL_Edit::SetNumberRange(int32_t iMin, int32_t iMax) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetNumberRange(iMin, iMax);
}
FWL_Error IFWL_Edit::SetBackColor(uint32_t dwColor) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetBackgroundColor(dwColor);
}
FWL_Error IFWL_Edit::SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetFont(wsFont, fSize);
}
void IFWL_Edit::SetScrollOffset(FX_FLOAT fScrollOffset) {
  return static_cast<CFWL_EditImp*>(GetImpl())->SetScrollOffset(fScrollOffset);
}
FX_BOOL IFWL_Edit::GetSuggestWords(CFX_PointF pointf,
                                   std::vector<CFX_ByteString>& sSuggest) {
  return static_cast<CFWL_EditImp*>(GetImpl())
      ->GetSuggestWords(pointf, sSuggest);
}
FX_BOOL IFWL_Edit::ReplaceSpellCheckWord(CFX_PointF pointf,
                                         const CFX_ByteStringC& bsReplace) {
  return static_cast<CFWL_EditImp*>(GetImpl())
      ->ReplaceSpellCheckWord(pointf, bsReplace);
}

CFWL_EditImp::CFWL_EditImp(const CFWL_WidgetImpProperties& properties,
                           IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_fVAlignOffset(0.0f),
      m_fScrollOffsetX(0.0f),
      m_fScrollOffsetY(0.0f),
      m_pEdtEngine(nullptr),
      m_bLButtonDown(FALSE),
      m_nSelStart(0),
      m_nLimit(-1),
      m_fSpaceAbove(0),
      m_fSpaceBelow(0),
      m_fFontSize(0),
      m_bSetRange(FALSE),
      m_iMin(-1),
      m_iMax(0xFFFFFFF),
      m_backColor(0),
      m_updateBackColor(FALSE),
      m_iCurRecord(-1),
      m_iMaxRecord(128) {
  m_rtClient.Reset();
  m_rtEngine.Reset();
  m_rtStatic.Reset();
}

CFWL_EditImp::~CFWL_EditImp() {
  delete m_pEdtEngine;
  ClearRecord();
}

FWL_Error CFWL_EditImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_Edit;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_EditImp::GetClassID() const {
  return FWL_Type::Edit;
}

FWL_Error CFWL_EditImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;
  if (!m_pDelegate)
    m_pDelegate = new CFWL_EditImpDelegate(this);

  InitCaret();
  if (!m_pEdtEngine)
    InitEngine();

  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::Finalize() {
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    ShowCaret(FALSE);
  }
  if (m_pHorzScrollBar) {
    m_pHorzScrollBar->Finalize();
  }
  if (m_pVertScrollBar) {
    m_pVertScrollBar->Finalize();
  }
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_EditImp::GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize) {
  if (bAutoSize) {
    rect.Set(0, 0, 0, 0);
    if (m_pEdtEngine) {
      int32_t iTextLen = m_pEdtEngine->GetTextLength();
      if (iTextLen > 0) {
        CFX_WideString wsText;
        m_pEdtEngine->GetText(wsText, 0);
        CFX_SizeF sz = CalcTextSize(
            wsText, m_pProperties->m_pThemeProvider,
            m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine);
        rect.Set(0, 0, sz.x, sz.y);
      }
    }
    CFWL_WidgetImp::GetWidgetRect(rect, TRUE);
  } else {
    rect = m_pProperties->m_rtWidget;
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
      if (IsShowScrollBar(TRUE)) {
        FX_FLOAT* pfWidth = static_cast<FX_FLOAT*>(
            GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
        rect.width += *pfWidth;
        rect.width += kEditMargin;
      }
      if (IsShowScrollBar(FALSE)) {
        FX_FLOAT* pfWidth = static_cast<FX_FLOAT*>(
            GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
        rect.height += *pfWidth;
        rect.height += kEditMargin;
      }
    }
  }
  return FWL_Error::Succeeded;
}

void CFWL_EditImp::SetStates(uint32_t dwStates, FX_BOOL bSet) {
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Invisible) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    ShowCaret(FALSE);
  }
  CFWL_WidgetImp::SetStates(dwStates, bSet);
}

FWL_Error CFWL_EditImp::SetWidgetRect(const CFX_RectF& rect) {
  return CFWL_WidgetImp::SetWidgetRect(rect);
}
FWL_Error CFWL_EditImp::Update() {
  if (IsLocked()) {
    return FWL_Error::Indefinite;
  }
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  Layout();
  if (m_rtClient.IsEmpty()) {
    return FWL_Error::Indefinite;
  }
  UpdateEditEngine();
  UpdateVAlignment();
  UpdateScroll();
  InitCaret();
  return FWL_Error::Succeeded;
}

FWL_WidgetHit CFWL_EditImp::HitTest(FX_FLOAT fx, FX_FLOAT fy) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
    if (IsShowScrollBar(TRUE)) {
      CFX_RectF rect;
      m_pVertScrollBar->GetWidgetRect(rect);
      if (rect.Contains(fx, fy))
        return FWL_WidgetHit::VScrollBar;
    }
    if (IsShowScrollBar(FALSE)) {
      CFX_RectF rect;
      m_pHorzScrollBar->GetWidgetRect(rect);
      if (rect.Contains(fx, fy))
        return FWL_WidgetHit::HScrollBar;
    }
  }
  if (m_rtClient.Contains(fx, fy))
    return FWL_WidgetHit::Edit;
  return FWL_WidgetHit::Unknown;
}

void CFWL_EditImp::AddSpellCheckObj(CFX_Path& PathData,
                                    int32_t nStart,
                                    int32_t nCount,
                                    FX_FLOAT fOffSetX,
                                    FX_FLOAT fOffSetY) {
  FX_FLOAT fStartX = 0.0f;
  FX_FLOAT fEndX = 0.0f;
  FX_FLOAT fY = 0.0f;
  FX_FLOAT fStep = 0.0f;
  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  CFX_RectFArray rectArray;
  CFX_RectF rectText;
  const FDE_TXTEDTPARAMS* txtEdtParams = m_pEdtEngine->GetEditParams();
  FX_FLOAT fAsent = (FX_FLOAT)txtEdtParams->pFont->GetAscent() *
                    txtEdtParams->fFontSize / 1000;
  pPage->CalcRangeRectArray(nStart, nCount, rectArray);
  for (int i = 0; i < rectArray.GetSize(); i++) {
    rectText = rectArray.GetAt(i);
    fY = rectText.top + fAsent + fOffSetY;
    fStep = txtEdtParams->fFontSize / 16.0f;
    fStartX = rectText.left + fOffSetX;
    fEndX = fStartX + rectText.Width();
    AddSquigglyPath(&PathData, fStartX, fEndX, fY, fStep);
  }
}
int32_t CFWL_EditImp::GetWordAtPoint(CFX_PointF pointf, int32_t& nCount) {
  return 0;
}
FX_BOOL CFWL_EditImp::GetSuggestWords(CFX_PointF pointf,
                                      std::vector<CFX_ByteString>& sSuggest) {
  int32_t nWordCount = 0;
  int32_t nWordStart = GetWordAtPoint(pointf, nWordCount);
  if (nWordCount < 1) {
    return FALSE;
  }
  CFX_WideString wsSpell;
  GetText(wsSpell, nWordStart, nWordCount);
  CFX_ByteString sLatinWord;
  for (int i = 0; i < nWordCount; i++) {
    if (!FX_EDIT_ISLATINWORD(wsSpell[i])) {
      break;
    }
    sLatinWord += (FX_CHAR)wsSpell[i];
  }
  if (sLatinWord.IsEmpty()) {
    return FALSE;
  }
  CFWL_EvtEdtCheckWord checkWordEvent;
  checkWordEvent.m_pSrcTarget = m_pInterface;
  checkWordEvent.bsWord = sLatinWord;
  checkWordEvent.bCheckWord = TRUE;
  DispatchEvent(&checkWordEvent);
  if (checkWordEvent.bCheckWord) {
    return FALSE;
  }
  CFWL_EvtEdtGetSuggestWords suggestWordsEvent;
  suggestWordsEvent.m_pSrcTarget = m_pInterface;
  suggestWordsEvent.bsWord = sLatinWord;
  suggestWordsEvent.bsArraySuggestWords = sSuggest;
  suggestWordsEvent.bSuggestWords = FALSE;
  DispatchEvent(&checkWordEvent);
  return suggestWordsEvent.bSuggestWords;
}
FX_BOOL CFWL_EditImp::ReplaceSpellCheckWord(CFX_PointF pointf,
                                            const CFX_ByteStringC& bsReplace) {
  int32_t nWordCount = 0;
  int32_t nWordStart = GetWordAtPoint(pointf, nWordCount);
  if (nWordCount < 1) {
    return FALSE;
  }
  CFX_WideString wsSpell;
  GetText(wsSpell, nWordStart, nWordCount);
  for (int i = 0; i < nWordCount; i++) {
    if (!FX_EDIT_ISLATINWORD(wsSpell[i])) {
      nWordCount = i;
      break;
    }
  }
  int32_t nDestLen = bsReplace.GetLength();
  CFX_WideString wsDest;
  FX_WCHAR* pBuffer = wsDest.GetBuffer(nDestLen);
  for (int32_t i = 0; i < nDestLen; i++) {
    pBuffer[i] = bsReplace[i];
  }
  wsDest.ReleaseBuffer(nDestLen);
  Replace(nWordStart, nWordCount, wsDest.AsStringC());
  return TRUE;
}
void CFWL_EditImp::DrawSpellCheck(CFX_Graphics* pGraphics,
                                  const CFX_Matrix* pMatrix) {
  pGraphics->SaveGraphState();
  if (pMatrix) {
    pGraphics->ConcatMatrix(const_cast<CFX_Matrix*>(pMatrix));
  }
  FX_ARGB cr = 0xFFFF0000;
  CFX_Color crLine(cr);
  CFWL_EvtEdtCheckWord checkWordEvent;
  checkWordEvent.m_pSrcTarget = m_pInterface;
  CFX_ByteString sLatinWord;
  CFX_Path pathSpell;
  pathSpell.Create();
  int32_t nStart = 0;
  FX_FLOAT fOffSetX = m_rtEngine.left - m_fScrollOffsetX;
  FX_FLOAT fOffSetY = m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset;
  CFX_WideString wsSpell;
  GetText(wsSpell);
  int32_t nContentLen = wsSpell.GetLength();
  for (int i = 0; i < nContentLen; i++) {
    if (FX_EDIT_ISLATINWORD(wsSpell[i])) {
      if (sLatinWord.IsEmpty()) {
        nStart = i;
      }
      sLatinWord += (FX_CHAR)wsSpell[i];
    } else {
      checkWordEvent.bsWord = sLatinWord;
      checkWordEvent.bCheckWord = TRUE;
      DispatchEvent(&checkWordEvent);
      if (!sLatinWord.IsEmpty() && !checkWordEvent.bCheckWord) {
        AddSpellCheckObj(pathSpell, nStart, sLatinWord.GetLength(), fOffSetX,
                         fOffSetY);
      }
      sLatinWord.clear();
    }
  }
  checkWordEvent.bsWord = sLatinWord;
  checkWordEvent.bCheckWord = TRUE;
  DispatchEvent(&checkWordEvent);
  if (!sLatinWord.IsEmpty() && !checkWordEvent.bCheckWord) {
    AddSpellCheckObj(pathSpell, nStart, sLatinWord.GetLength(), fOffSetX,
                     fOffSetY);
  }
  if (!pathSpell.IsEmpty()) {
    CFX_RectF rtClip = m_rtEngine;
    CFX_Matrix mt;
    mt.Set(1, 0, 0, 1, fOffSetX, fOffSetY);
    if (pMatrix) {
      pMatrix->TransformRect(rtClip);
      mt.Concat(*pMatrix);
    }
    pGraphics->SetClipRect(rtClip);
    pGraphics->SetStrokeColor(&crLine);
    pGraphics->SetLineWidth(0);
    pGraphics->StrokePath(&pathSpell, nullptr);
  }
  pGraphics->RestoreGraphState();
}
FWL_Error CFWL_EditImp::DrawWidget(CFX_Graphics* pGraphics,
                                   const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider)
    return FWL_Error::Indefinite;
  if (m_rtClient.IsEmpty()) {
    return FWL_Error::Indefinite;
  }
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  if (!m_pWidgetMgr->IsFormDisabled()) {
    DrawTextBk(pGraphics, pTheme, pMatrix);
  }
  if (m_pEdtEngine) {
    DrawContent(pGraphics, pTheme, pMatrix);
  }
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) &&
      !(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly)) {
    DrawSpellCheck(pGraphics, pMatrix);
  }
  if (HasBorder()) {
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, pMatrix);
  }
  if (HasEdge()) {
    DrawEdge(pGraphics, CFWL_Part::Edge, pTheme, pMatrix);
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_EditImp::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  if (!pThemeProvider)
    return FWL_Error::Indefinite;
  if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetThemeProvider(pThemeProvider);
  }
  if (m_pVertScrollBar) {
    m_pVertScrollBar->SetThemeProvider(pThemeProvider);
  }
  if (m_pCaret) {
    m_pCaret->SetThemeProvider(pThemeProvider);
  }
  m_pProperties->m_pThemeProvider = pThemeProvider;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::SetText(const CFX_WideString& wsText) {
  m_pEdtEngine->SetText(wsText);
  return FWL_Error::Succeeded;
}

int32_t CFWL_EditImp::GetTextLength() const {
  if (!m_pEdtEngine)
    return -1;
  return m_pEdtEngine->GetTextLength();
}

FWL_Error CFWL_EditImp::GetText(CFX_WideString& wsText,
                                int32_t nStart,
                                int32_t nCount) const {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->GetText(wsText, nStart, nCount);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::ClearText() {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->ClearText();
  return FWL_Error::Succeeded;
}

int32_t CFWL_EditImp::GetCaretPos() const {
  if (!m_pEdtEngine)
    return -1;
  return m_pEdtEngine->GetCaretPos();
}

int32_t CFWL_EditImp::SetCaretPos(int32_t nIndex, FX_BOOL bBefore) {
  if (!m_pEdtEngine)
    return -1;
  return m_pEdtEngine->SetCaretPos(nIndex, bBefore);
}

FWL_Error CFWL_EditImp::AddSelRange(int32_t nStart, int32_t nCount) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->AddSelRange(nStart, nCount);
  return FWL_Error::Succeeded;
}

int32_t CFWL_EditImp::CountSelRanges() {
  if (!m_pEdtEngine)
    return 0;
  return m_pEdtEngine->CountSelRanges();
}

int32_t CFWL_EditImp::GetSelRange(int32_t nIndex, int32_t& nStart) {
  if (!m_pEdtEngine)
    return -1;
  return m_pEdtEngine->GetSelRange(nIndex, nStart);
}

FWL_Error CFWL_EditImp::ClearSelections() {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->ClearSelection();
  return FWL_Error::Succeeded;
}

int32_t CFWL_EditImp::GetLimit() {
  return m_nLimit;
}

FWL_Error CFWL_EditImp::SetLimit(int32_t nLimit) {
  m_nLimit = nLimit;
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->SetLimit(nLimit);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::SetAliasChar(FX_WCHAR wAlias) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->SetAliasChar(wAlias);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::Insert(int32_t nStart,
                               const FX_WCHAR* lpText,
                               int32_t nLen) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    return FWL_Error::Indefinite;
  }
  m_pEdtEngine->Insert(nStart, lpText, nLen);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::DeleteSelections() {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  int32_t iCount = m_pEdtEngine->CountSelRanges();
  if (iCount > 0)
    m_pEdtEngine->Delete(-1);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::DeleteRange(int32_t nStart, int32_t nCount) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->DeleteRange(nStart, nCount);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::Replace(int32_t nStart,
                                int32_t nLen,
                                const CFX_WideStringC& wsReplace) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  m_pEdtEngine->Replace(nStart, nLen, CFX_WideString(wsReplace));
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::DoClipboard(int32_t iCmd) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    return FWL_Error::Succeeded;
  }
  return FWL_Error::Indefinite;
}

FX_BOOL CFWL_EditImp::Copy(CFX_WideString& wsCopy) {
  if (!m_pEdtEngine)
    return FALSE;

  int32_t nCount = m_pEdtEngine->CountSelRanges();
  if (nCount == 0)
    return FALSE;

  wsCopy.clear();
  CFX_WideString wsTemp;
  int32_t nStart, nLength;
  for (int32_t i = 0; i < nCount; i++) {
    nLength = m_pEdtEngine->GetSelRange(i, nStart);
    m_pEdtEngine->GetText(wsTemp, nStart, nLength);
    wsCopy += wsTemp;
    wsTemp.clear();
  }
  return TRUE;
}

FX_BOOL CFWL_EditImp::Cut(CFX_WideString& wsCut) {
  if (!m_pEdtEngine)
    return FALSE;

  int32_t nCount = m_pEdtEngine->CountSelRanges();
  if (nCount == 0)
    return FALSE;

  wsCut.clear();
  CFX_WideString wsTemp;
  int32_t nStart, nLength;
  for (int32_t i = 0; i < nCount; i++) {
    nLength = m_pEdtEngine->GetSelRange(i, nStart);
    m_pEdtEngine->GetText(wsTemp, nStart, nLength);
    wsCut += wsTemp;
    wsTemp.clear();
  }
  m_pEdtEngine->Delete(0);
  return TRUE;
}

FX_BOOL CFWL_EditImp::Paste(const CFX_WideString& wsPaste) {
  if (!m_pEdtEngine)
    return FALSE;

  int32_t nCaret = m_pEdtEngine->GetCaretPos();
  int32_t iError =
      m_pEdtEngine->Insert(nCaret, wsPaste.c_str(), wsPaste.GetLength());
  if (iError < 0) {
    ProcessInsertError(iError);
    return FALSE;
  }
  return TRUE;
}

FX_BOOL CFWL_EditImp::Delete() {
  if (!m_pEdtEngine)
    return FALSE;

  int32_t nCount = m_pEdtEngine->CountSelRanges();
  if (nCount < 1)
    return FALSE;

  m_pEdtEngine->Delete(0);
  return TRUE;
}

FX_BOOL CFWL_EditImp::Redo(const IFDE_TxtEdtDoRecord* pRecord) {
  if (!m_pEdtEngine)
    return FALSE;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_NoRedoUndo)
    return TRUE;
  return m_pEdtEngine->Redo(pRecord);
}

FX_BOOL CFWL_EditImp::Undo(const IFDE_TxtEdtDoRecord* pRecord) {
  if (!m_pEdtEngine)
    return FALSE;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_NoRedoUndo)
    return TRUE;
  return m_pEdtEngine->Undo(pRecord);
}

FX_BOOL CFWL_EditImp::Undo() {
  if (!CanUndo())
    return FALSE;
  return Undo(m_DoRecords[m_iCurRecord--].get());
}

FX_BOOL CFWL_EditImp::Redo() {
  if (!CanRedo())
    return FALSE;
  return Redo(m_DoRecords[++m_iCurRecord].get());
}

FX_BOOL CFWL_EditImp::CanUndo() {
  return m_iCurRecord >= 0;
}

FX_BOOL CFWL_EditImp::CanRedo() {
  return m_iCurRecord < pdfium::CollectionSize<int32_t>(m_DoRecords) - 1;
}

FWL_Error CFWL_EditImp::SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant) {
  if (!m_pEdtEngine)
    return FWL_Error::Indefinite;

  FDE_TXTEDTPARAMS* pParams = m_pEdtEngine->GetEditParams();
  pParams->fTabWidth = fTabWidth;
  pParams->bTabEquidistant = bEquidistant;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::SetOuter(IFWL_Widget* pOuter) {
  m_pOuter = pOuter;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_EditImp::SetNumberRange(int32_t iMin, int32_t iMax) {
  m_iMin = iMin;
  m_iMax = iMax;
  m_bSetRange = TRUE;
  return FWL_Error::Succeeded;
}

void CFWL_EditImp::On_CaretChanged(CFDE_TxtEdtEngine* pEdit,
                                   int32_t nPage,
                                   FX_BOOL bVisible) {
  if (m_rtEngine.IsEmpty())
    return;
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0)
    return;

  FX_BOOL bRepaintContent = UpdateOffset();
  UpdateCaret();
  CFX_RectF rtInvalid;
  rtInvalid.Set(0, 0, 0, 0);
  FX_BOOL bRepaintScroll = FALSE;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) {
    IFWL_ScrollBar* pScroll = UpdateScroll();
    if (pScroll) {
      pScroll->GetWidgetRect(rtInvalid);
      bRepaintScroll = TRUE;
    }
  }
  if (bRepaintContent || bRepaintScroll) {
    if (bRepaintContent) {
      rtInvalid.Union(m_rtEngine);
    }
    Repaint(&rtInvalid);
  }
}

void CFWL_EditImp::On_TextChanged(CFDE_TxtEdtEngine* pEdit,
                                  FDE_TXTEDT_TEXTCHANGE_INFO& ChangeInfo) {
  uint32_t dwStyleEx = m_pProperties->m_dwStyleExes;
  if (dwStyleEx & FWL_STYLEEXT_EDT_VAlignMask)
    UpdateVAlignment();

  IFDE_TxtEdtPage* page = m_pEdtEngine->GetPage(0);
  FX_FLOAT fContentWidth = page->GetContentsBox().width;
  FX_FLOAT fContentHeight = page->GetContentsBox().height;
  CFX_RectF rtTemp;
  GetClientRect(rtTemp);
  FX_BOOL bHSelfAdaption =
      m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HSelfAdaption;
  FX_BOOL bVSelfAdaption =
      m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VSelfAdaption;
  FX_BOOL bNeedUpdate = FALSE;
  if (bHSelfAdaption || bVSelfAdaption) {
    CFWL_EvtEdtPreSelfAdaption evt;
    evt.m_pSrcTarget = m_pInterface;
    evt.bHSelfAdaption = TRUE;
    evt.bVSelfAdaption = TRUE;
    FX_FLOAT fWidth;
    FX_FLOAT fHight;
    fWidth = bHSelfAdaption ? fContentWidth : m_pProperties->m_rtWidget.width;
    fHight = bVSelfAdaption ? fContentHeight : m_pProperties->m_rtWidget.height;
    evt.rtAfterChange.Set(0, 0, fWidth, fHight);
    DispatchEvent(&evt);
    if (!evt.bHSelfAdaption) {
      ModifyStylesEx(
          0, FWL_STYLEEXT_EDT_HSelfAdaption | FWL_STYLEEXT_EDT_AutoHScroll);
    }
    if (!evt.bVSelfAdaption) {
      ModifyStylesEx(
          0, FWL_STYLEEXT_EDT_VSelfAdaption | FWL_STYLEEXT_EDT_AutoVScroll);
    }
    bNeedUpdate = (bHSelfAdaption && !evt.bHSelfAdaption) ||
                  (bVSelfAdaption && !evt.bVSelfAdaption);
  }
  FX_FLOAT fContentWidth1 = fContentWidth;
  FX_FLOAT fContentHeight1 = fContentHeight;
  if (bNeedUpdate) {
    UpdateEditParams();
    UpdateEditLayout();
    IFDE_TxtEdtPage* page1 = m_pEdtEngine->GetPage(0);
    fContentWidth1 = page1->GetContentsBox().width;
    fContentHeight1 = page1->GetContentsBox().height;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HSelfAdaption) {
    rtTemp.width = std::max(m_pProperties->m_rtWidget.width, fContentWidth1);
    m_pProperties->m_rtWidget.width = fContentWidth1;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VSelfAdaption) {
    rtTemp.height = std::max(m_pProperties->m_rtWidget.height, fContentHeight1);
    m_pProperties->m_rtWidget.height = fContentHeight1;
  }
  CFWL_EvtEdtTextChanged event;
  event.m_pSrcTarget = m_pInterface;
  event.nChangeType = ChangeInfo.nChangeType;
  event.wsInsert = ChangeInfo.wsInsert;
  event.wsDelete = ChangeInfo.wsDelete;
  event.wsPrevText = ChangeInfo.wsPrevText;
  DispatchEvent(&event);
  LayoutScrollBar();
  Repaint(&rtTemp);
}
void CFWL_EditImp::On_SelChanged(CFDE_TxtEdtEngine* pEdit) {
  CFX_RectF rtTemp;
  GetClientRect(rtTemp);
  Repaint(&rtTemp);
}
FX_BOOL CFWL_EditImp::On_PageLoad(CFDE_TxtEdtEngine* pEdit,
                                  int32_t nPageIndex,
                                  int32_t nPurpose) {
  CFDE_TxtEdtEngine* pEdtEngine = m_pEdtEngine;
  IFDE_TxtEdtPage* pPage = pEdtEngine->GetPage(nPageIndex);
  if (!pPage)
    return FALSE;
  pPage->LoadPage(nullptr, nullptr);
  return TRUE;
}
FX_BOOL CFWL_EditImp::On_PageUnload(CFDE_TxtEdtEngine* pEdit,
                                    int32_t nPageIndex,
                                    int32_t nPurpose) {
  CFDE_TxtEdtEngine* pEdtEngine = m_pEdtEngine;
  IFDE_TxtEdtPage* pPage = pEdtEngine->GetPage(nPageIndex);
  if (!pPage)
    return FALSE;
  pPage->UnloadPage(nullptr);
  return TRUE;
}

void CFWL_EditImp::On_AddDoRecord(CFDE_TxtEdtEngine* pEdit,
                                  IFDE_TxtEdtDoRecord* pRecord) {
  AddDoRecord(pRecord);
}

FX_BOOL CFWL_EditImp::On_Validate(CFDE_TxtEdtEngine* pEdit,
                                  CFX_WideString& wsText) {
  IFWL_Widget* pDst = GetOuter();
  if (!pDst) {
    pDst = m_pInterface;
  }
  CFWL_EvtEdtValidate event;
  event.pDstWidget = pDst;
  event.m_pSrcTarget = m_pInterface;
  event.wsInsert = wsText;
  event.bValidate = TRUE;
  DispatchEvent(&event);
  return event.bValidate;
}
FWL_Error CFWL_EditImp::SetBackgroundColor(uint32_t color) {
  m_backColor = color;
  m_updateBackColor = TRUE;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_EditImp::SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize) {
  m_wsFont = wsFont;
  m_fFontSize = fSize;
  return FWL_Error::Succeeded;
}
void CFWL_EditImp::SetScrollOffset(FX_FLOAT fScrollOffset) {
  m_fScrollOffsetY = fScrollOffset;
}
void CFWL_EditImp::DrawTextBk(CFX_Graphics* pGraphics,
                              IFWL_ThemeProvider* pTheme,
                              const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = CFWL_Part::Background;
  param.m_bStaticBackground = false;
  param.m_dwStates = m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly
                         ? CFWL_PartState_ReadOnly
                         : CFWL_PartState_Normal;
  uint32_t dwStates = (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled);
  if (dwStates) {
    param.m_dwStates = CFWL_PartState_Disabled;
  }
  param.m_pGraphics = pGraphics;
  param.m_matrix = *pMatrix;
  param.m_rtPart = m_rtClient;
  pTheme->DrawBackground(&param);
  if (!IsShowScrollBar(TRUE) || !IsShowScrollBar(FALSE)) {
    return;
  }
  CFX_RectF rtScorll;
  m_pHorzScrollBar->GetWidgetRect(rtScorll);
  CFX_RectF rtStatic;
  rtStatic.Set(m_rtClient.right() - rtScorll.height,
               m_rtClient.bottom() - rtScorll.height, rtScorll.height,
               rtScorll.height);
  param.m_bStaticBackground = true;
  param.m_bMaximize = true;
  param.m_rtPart = rtStatic;
  pTheme->DrawBackground(&param);
}
void CFWL_EditImp::DrawContent(CFX_Graphics* pGraphics,
                               IFWL_ThemeProvider* pTheme,
                               const CFX_Matrix* pMatrix) {
  if (!m_pEdtEngine)
    return;
  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  if (!pPage)
    return;
  pGraphics->SaveGraphState();
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText) {
    pGraphics->SaveGraphState();
  }
  CFX_RectF rtClip = m_rtEngine;
  FX_FLOAT fOffSetX = m_rtEngine.left - m_fScrollOffsetX;
  FX_FLOAT fOffSetY = m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset;
  CFX_Matrix mt;
  mt.Set(1, 0, 0, 1, fOffSetX, fOffSetY);
  if (pMatrix) {
    pMatrix->TransformRect(rtClip);
    mt.Concat(*pMatrix);
  }
  FX_BOOL bShowSel =
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_NoHideSel) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused);
  if (bShowSel) {
    IFWL_Widget* pForm = m_pWidgetMgr->GetSystemFormWidget(m_pInterface);
    if (pForm) {
      bShowSel = (pForm->GetStates() & FWL_WGTSTATE_Deactivated) !=
                 FWL_WGTSTATE_Deactivated;
    }
  }
  int32_t nSelCount = m_pEdtEngine->CountSelRanges();
  if (bShowSel && nSelCount > 0) {
    int32_t nPageCharStart = pPage->GetCharStart();
    int32_t nPageCharCount = pPage->GetCharCount();
    int32_t nPageCharEnd = nPageCharStart + nPageCharCount - 1;
    int32_t nCharCount;
    int32_t nCharStart;
    CFX_RectFArray rectArr;
    int32_t i = 0;
    for (i = 0; i < nSelCount; i++) {
      nCharCount = m_pEdtEngine->GetSelRange(i, nCharStart);
      int32_t nCharEnd = nCharStart + nCharCount - 1;
      if (nCharEnd < nPageCharStart || nCharStart > nPageCharEnd) {
        continue;
      }
      int32_t nBgn = std::max(nCharStart, nPageCharStart);
      int32_t nEnd = std::min(nCharEnd, nPageCharEnd);
      pPage->CalcRangeRectArray(nBgn - nPageCharStart, nEnd - nBgn + 1,
                                rectArr);
    }
    int32_t nCount = rectArr.GetSize();
    CFX_Path path;
    path.Create();
    for (i = 0; i < nCount; i++) {
      rectArr[i].left += fOffSetX;
      rectArr[i].top += fOffSetY;
      path.AddRectangle(rectArr[i].left, rectArr[i].top, rectArr[i].width,
                        rectArr[i].height);
    }
    pGraphics->SetClipRect(rtClip);
    CFWL_ThemeBackground param;
    param.m_pGraphics = pGraphics;
    param.m_matrix = *pMatrix;
    param.m_pWidget = m_pInterface;
    param.m_iPart = CFWL_Part::Background;
    param.m_pPath = &path;
    pTheme->DrawBackground(&param);
  }
  CFX_RenderDevice* pRenderDev = pGraphics->GetRenderDevice();
  if (!pRenderDev)
    return;

  std::unique_ptr<CFDE_RenderDevice> pRenderDevice(
      new CFDE_RenderDevice(pRenderDev, FALSE));
  std::unique_ptr<CFDE_RenderContext> pRenderContext(new CFDE_RenderContext);
  pRenderDevice->SetClipRect(rtClip);
  pRenderContext->StartRender(pRenderDevice.get(), pPage, mt);
  pRenderContext->DoRender(nullptr);
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText) {
    pGraphics->RestoreGraphState();
    CFX_Path path;
    path.Create();
    int32_t iLimit = m_nLimit > 0 ? m_nLimit : 1;
    FX_FLOAT fStep = m_rtEngine.width / iLimit;
    FX_FLOAT fLeft = m_rtEngine.left + 1;
    for (int32_t i = 1; i < iLimit; i++) {
      fLeft += fStep;
      path.AddLine(fLeft, m_rtClient.top, fLeft, m_rtClient.bottom());
    }
    CFWL_ThemeBackground param;
    param.m_pGraphics = pGraphics;
    param.m_matrix = *pMatrix;
    param.m_pWidget = m_pInterface;
    param.m_iPart = CFWL_Part::CombTextLine;
    param.m_pPath = &path;
    pTheme->DrawBackground(&param);
  }
  pGraphics->RestoreGraphState();
}

void CFWL_EditImp::UpdateEditEngine() {
  UpdateEditParams();
  UpdateEditLayout();
  if (m_nLimit > -1) {
    m_pEdtEngine->SetLimit(m_nLimit);
  }
}
void CFWL_EditImp::UpdateEditParams() {
  FDE_TXTEDTPARAMS params;
  params.nHorzScale = 100;
  params.fPlateWidth = m_rtEngine.width;
  params.fPlateHeight = m_rtEngine.height;
  if (m_pProperties->m_dwStyles & FWL_WGTSTYLE_RTLLayout) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_RTL;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VerticalLayout) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_DocVertical;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VerticalChars) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_CharVertial;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReverseLine) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_LineReserve;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ArabicShapes) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_ArabicShapes;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ExpandTab) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_ExpandTab;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_CombText) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_CombText;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_LastLineHeight) {
    params.dwLayoutStyles |= FDE_TEXTEDITLAYOUT_LastLineHeight;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_Validate) {
    params.dwMode |= FDE_TEXTEDITMODE_Validate;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_Password) {
    params.dwMode |= FDE_TEXTEDITMODE_Password;
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HAlignMask) {
    case FWL_STYLEEXT_EDT_HNear: {
      params.dwAlignment |= FDE_TEXTEDITALIGN_Left;
      break;
    }
    case FWL_STYLEEXT_EDT_HCenter: {
      params.dwAlignment |= FDE_TEXTEDITALIGN_Center;
      break;
    }
    case FWL_STYLEEXT_EDT_HFar: {
      params.dwAlignment |= FDE_TEXTEDITALIGN_Right;
      break;
    }
    default: {}
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HAlignModeMask) {
    case FWL_STYLEEXT_EDT_Justified: {
      params.dwAlignment |= FDE_TEXTEDITALIGN_Justified;
      break;
    }
    case FWL_STYLEEXT_EDT_Distributed: {
      params.dwAlignment |= FDE_TEXTEDITALIGN_Distributed;
      break;
    }
    default: { params.dwAlignment |= FDE_TEXTEDITALIGN_Normal; }
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) {
    params.dwMode |= FDE_TEXTEDITMODE_MultiLines;
    if ((m_pProperties->m_dwStyles & FWL_WGTSTYLE_HScroll) == 0 &&
        (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_AutoHScroll) == 0) {
      params.dwMode |=
          FDE_TEXTEDITMODE_AutoLineWrap | FDE_TEXTEDITMODE_LimitArea_Horz;
    }
    if ((m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll) == 0 &&
        (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_AutoVScroll) == 0) {
      params.dwMode |= FDE_TEXTEDITMODE_LimitArea_Vert;
    } else {
      params.fPlateHeight = 0x00FFFFFF;
    }
  } else {
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_AutoHScroll) == 0) {
      params.dwMode |= FDE_TEXTEDITMODE_LimitArea_Horz;
    }
  }
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
      (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    params.dwMode |= FDE_TEXTEDITMODE_ReadOnly;
  }
  FX_FLOAT* pFontSize =
      static_cast<FX_FLOAT*>(GetThemeCapacity(CFWL_WidgetCapacity::FontSize));
  if (!pFontSize)
    return;
  m_fFontSize = *pFontSize;
  uint32_t* pFontColor =
      static_cast<uint32_t*>(GetThemeCapacity(CFWL_WidgetCapacity::TextColor));
  if (!pFontColor)
    return;
  params.dwFontColor = *pFontColor;
  FX_FLOAT* pLineHeight =
      static_cast<FX_FLOAT*>(GetThemeCapacity(CFWL_WidgetCapacity::LineHeight));
  if (!pLineHeight)
    return;
  params.fLineSpace = *pLineHeight;
  CFGAS_GEFont* pFont =
      static_cast<CFGAS_GEFont*>(GetThemeCapacity(CFWL_WidgetCapacity::Font));
  if (!pFont)
    return;
  params.pFont = pFont;
  params.fFontSize = m_fFontSize;
  params.nLineCount = (int32_t)(params.fPlateHeight / params.fLineSpace);
  if (params.nLineCount <= 0) {
    params.nLineCount = 1;
  }
  params.fTabWidth = params.fFontSize * 1;
  params.bTabEquidistant = TRUE;
  params.wLineBreakChar = L'\n';
  params.nCharRotation = 0;
  params.pEventSink = this;
  m_pEdtEngine->SetEditParams(params);
}

void CFWL_EditImp::UpdateEditLayout() {
  if (m_pEdtEngine->GetTextLength() <= 0)
    m_pEdtEngine->SetTextByStream(nullptr);

  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  if (pPage)
    pPage->UnloadPage(nullptr);

  m_pEdtEngine->StartLayout();
  m_pEdtEngine->DoLayout(nullptr);
  m_pEdtEngine->EndLayout();
  pPage = m_pEdtEngine->GetPage(0);
  if (pPage)
    pPage->LoadPage(nullptr, nullptr);
}

FX_BOOL CFWL_EditImp::UpdateOffset() {
  CFX_RectF rtCaret;
  m_pEdtEngine->GetCaretRect(rtCaret);
  FX_FLOAT fOffSetX = m_rtEngine.left - m_fScrollOffsetX;
  FX_FLOAT fOffSetY = m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset;
  rtCaret.Offset(fOffSetX, fOffSetY);
  const CFX_RectF& rtEidt = m_rtEngine;
  if (rtEidt.Contains(rtCaret)) {
    IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
    if (!pPage)
      return FALSE;

    CFX_RectF rtFDE = pPage->GetContentsBox();
    rtFDE.Offset(fOffSetX, fOffSetY);
    if (rtFDE.right() < rtEidt.right() && m_fScrollOffsetX > 0) {
      m_fScrollOffsetX += rtFDE.right() - rtEidt.right();
      m_fScrollOffsetX = std::max(m_fScrollOffsetX, 0.0f);
    }
    if (rtFDE.bottom() < rtEidt.bottom() && m_fScrollOffsetY > 0) {
      m_fScrollOffsetY += rtFDE.bottom() - rtEidt.bottom();
      m_fScrollOffsetY = std::max(m_fScrollOffsetY, 0.0f);
    }
    return FALSE;
  }

  FX_FLOAT offsetX = 0.0;
  FX_FLOAT offsetY = 0.0;
  if (rtCaret.left < rtEidt.left)
    offsetX = rtCaret.left - rtEidt.left;
  if (rtCaret.right() > rtEidt.right())
    offsetX = rtCaret.right() - rtEidt.right();
  if (rtCaret.top < rtEidt.top)
    offsetY = rtCaret.top - rtEidt.top;
  if (rtCaret.bottom() > rtEidt.bottom())
    offsetY = rtCaret.bottom() - rtEidt.bottom();
  if (!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HSelfAdaption))
    m_fScrollOffsetX += offsetX;
  if (!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VSelfAdaption))
    m_fScrollOffsetY += offsetY;
  if (m_fFontSize > m_rtEngine.height)
    m_fScrollOffsetY = 0;
  return TRUE;
}

FX_BOOL CFWL_EditImp::UpdateOffset(IFWL_ScrollBar* pScrollBar,
                                   FX_FLOAT fPosChanged) {
  if (pScrollBar == m_pHorzScrollBar.get())
    m_fScrollOffsetX += fPosChanged;
  else
    m_fScrollOffsetY += fPosChanged;
  return TRUE;
}

void CFWL_EditImp::UpdateVAlignment() {
  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  if (!pPage)
    return;
  const CFX_RectF& rtFDE = pPage->GetContentsBox();
  FX_FLOAT fOffsetY = 0.0f;
  FX_FLOAT fSpaceAbove = 0.0f;
  FX_FLOAT fSpaceBelow = 0.0f;
  CFX_SizeF* pSpace = static_cast<CFX_SizeF*>(
      GetThemeCapacity(CFWL_WidgetCapacity::SpaceAboveBelow));
  if (pSpace) {
    fSpaceAbove = pSpace->x;
    fSpaceBelow = pSpace->y;
  }
  if (fSpaceAbove < 0.1f) {
    fSpaceAbove = 0;
  }
  if (fSpaceBelow < 0.1f) {
    fSpaceBelow = 0;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VCenter) {
    fOffsetY = (m_rtEngine.height - rtFDE.height) / 2;
    if (fOffsetY < (fSpaceAbove + fSpaceBelow) / 2 &&
        fSpaceAbove < fSpaceBelow) {
      return;
    }
    fOffsetY += (fSpaceAbove - fSpaceBelow) / 2;
  } else if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VFar) {
    fOffsetY = (m_rtEngine.height - rtFDE.height);
    fOffsetY -= fSpaceBelow;
  } else {
    fOffsetY += fSpaceAbove;
  }
  m_fVAlignOffset = fOffsetY;
  if (m_fVAlignOffset < 0) {
    m_fVAlignOffset = 0;
  }
}
void CFWL_EditImp::UpdateCaret() {
  CFX_RectF rtFDE;
  m_pEdtEngine->GetCaretRect(rtFDE);
  rtFDE.Offset(m_rtEngine.left - m_fScrollOffsetX,
               m_rtEngine.top - m_fScrollOffsetY + m_fVAlignOffset);
  CFX_RectF rtCaret;
  rtCaret.Set(rtFDE.left, rtFDE.top, rtFDE.width, rtFDE.height);
  CFX_RectF temp = rtCaret;
  CFX_RectF rtClient;
  GetClientRect(rtClient);
  rtCaret.Intersect(rtClient);
  if (rtCaret.left > rtClient.right()) {
    FX_FLOAT right = rtCaret.right();
    rtCaret.left = rtClient.right() - 1;
    rtCaret.width = right - rtCaret.left;
  }
  FX_BOOL bIntersect = !rtCaret.IsEmpty();
  FX_BOOL bShow = TRUE;
  FX_BOOL bShowWhole = FALSE;
  if (!(m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) || !bIntersect) {
    bShow = FALSE;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_HSelfAdaption &&
      temp.right() > m_rtEngine.right()) {
    bShowWhole = TRUE;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_VSelfAdaption &&
      temp.bottom() > m_rtEngine.bottom()) {
    bShowWhole = TRUE;
  } else {
    bShow = (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused && bIntersect);
  }
  if (bShowWhole) {
    rtCaret = temp;
  }
  ShowCaret(bShow, &rtCaret);
}
IFWL_ScrollBar* CFWL_EditImp::UpdateScroll() {
  FX_BOOL bShowHorz =
      m_pHorzScrollBar &&
      ((m_pHorzScrollBar->GetStates() & FWL_WGTSTATE_Invisible) == 0);
  FX_BOOL bShowVert =
      m_pVertScrollBar &&
      ((m_pVertScrollBar->GetStates() & FWL_WGTSTATE_Invisible) == 0);
  if (!bShowHorz && !bShowVert) {
    return nullptr;
  }
  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  if (!pPage)
    return nullptr;
  const CFX_RectF& rtFDE = pPage->GetContentsBox();
  IFWL_ScrollBar* pRepaint = nullptr;
  if (bShowHorz) {
    CFX_RectF rtScroll;
    m_pHorzScrollBar->GetWidgetRect(rtScroll);
    if (rtScroll.width < rtFDE.width) {
      m_pHorzScrollBar->LockUpdate();
      FX_FLOAT fRange = rtFDE.width - rtScroll.width;
      m_pHorzScrollBar->SetRange(0.0f, fRange);
      FX_FLOAT fPos = m_fScrollOffsetX;
      if (fPos < 0.0f) {
        fPos = 0.0f;
      }
      if (fPos > fRange) {
        fPos = fRange;
      }
      m_pHorzScrollBar->SetPos(fPos);
      m_pHorzScrollBar->SetTrackPos(fPos);
      m_pHorzScrollBar->SetPageSize(rtScroll.width);
      m_pHorzScrollBar->SetStepSize(rtScroll.width / 10);
      m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Disabled, FALSE);
      m_pHorzScrollBar->UnlockUpdate();
      m_pHorzScrollBar->Update();
      pRepaint = m_pHorzScrollBar.get();
    } else if ((m_pHorzScrollBar->GetStates() & FWL_WGTSTATE_Disabled) == 0) {
      m_pHorzScrollBar->LockUpdate();
      m_pHorzScrollBar->SetRange(0, -1);
      m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Disabled, TRUE);
      m_pHorzScrollBar->UnlockUpdate();
      m_pHorzScrollBar->Update();
      pRepaint = m_pHorzScrollBar.get();
    }
  }
  if (bShowVert) {
    CFX_RectF rtScroll;
    m_pVertScrollBar->GetWidgetRect(rtScroll);
    if (rtScroll.height < rtFDE.height) {
      m_pVertScrollBar->LockUpdate();
      FX_FLOAT fStep = m_pEdtEngine->GetEditParams()->fLineSpace;
      FX_FLOAT fRange = rtFDE.height - m_rtEngine.height;
      if (fRange < fStep) {
        fRange = fStep;
      }
      m_pVertScrollBar->SetRange(0.0f, fRange);
      FX_FLOAT fPos = m_fScrollOffsetY;
      if (fPos < 0.0f) {
        fPos = 0.0f;
      }
      if (fPos > fRange) {
        fPos = fRange;
      }
      m_pVertScrollBar->SetPos(fPos);
      m_pVertScrollBar->SetTrackPos(fPos);
      m_pVertScrollBar->SetPageSize(rtScroll.height);
      m_pVertScrollBar->SetStepSize(fStep);
      m_pVertScrollBar->SetStates(FWL_WGTSTATE_Disabled, FALSE);
      m_pVertScrollBar->UnlockUpdate();
      m_pVertScrollBar->Update();
      pRepaint = m_pVertScrollBar.get();
    } else if ((m_pVertScrollBar->GetStates() & FWL_WGTSTATE_Disabled) == 0) {
      m_pVertScrollBar->LockUpdate();
      m_pVertScrollBar->SetRange(0, -1);
      m_pVertScrollBar->SetStates(FWL_WGTSTATE_Disabled, TRUE);
      m_pVertScrollBar->UnlockUpdate();
      m_pVertScrollBar->Update();
      pRepaint = m_pVertScrollBar.get();
    }
  }
  return pRepaint;
}
FX_BOOL CFWL_EditImp::IsShowScrollBar(FX_BOOL bVert) {
  FX_BOOL bShow =
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ShowScrollbarFocus)
          ? (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) ==
                FWL_WGTSTATE_Focused
          : TRUE;
  if (bVert) {
    return bShow && (m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll) &&
           (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine) &&
           IsContentHeightOverflow();
  }
  return bShow && (m_pProperties->m_dwStyles & FWL_WGTSTYLE_HScroll) &&
         (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_MultiLine);
}
FX_BOOL CFWL_EditImp::IsContentHeightOverflow() {
  if (!m_pEdtEngine)
    return FALSE;
  IFDE_TxtEdtPage* pPage = m_pEdtEngine->GetPage(0);
  if (!pPage)
    return FALSE;
  return pPage->GetContentsBox().height > m_rtEngine.height + 1.0f;
}
int32_t CFWL_EditImp::AddDoRecord(IFDE_TxtEdtDoRecord* pRecord) {
  int32_t nCount = pdfium::CollectionSize<int32_t>(m_DoRecords);
  if (m_iCurRecord == nCount - 1) {
    if (nCount == m_iMaxRecord) {
      m_DoRecords.pop_front();
      m_iCurRecord--;
    }
  } else {
    m_DoRecords.erase(m_DoRecords.begin() + m_iCurRecord + 1,
                      m_DoRecords.end());
  }

  m_DoRecords.push_back(std::unique_ptr<IFDE_TxtEdtDoRecord>(pRecord));
  m_iCurRecord = pdfium::CollectionSize<int32_t>(m_DoRecords) - 1;
  return m_iCurRecord;
}
void CFWL_EditImp::Layout() {
  GetClientRect(m_rtClient);
  m_rtEngine = m_rtClient;
  FX_FLOAT* pfWidth = static_cast<FX_FLOAT*>(
      GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
  if (!pfWidth)
    return;
  FX_FLOAT fWidth = *pfWidth;
  if (!m_pOuter) {
    CFX_RectF* pUIMargin = static_cast<CFX_RectF*>(
        GetThemeCapacity(CFWL_WidgetCapacity::UIMargin));
    if (pUIMargin) {
      m_rtEngine.Deflate(pUIMargin->left, pUIMargin->top, pUIMargin->width,
                         pUIMargin->height);
    }
  } else if (m_pOuter->GetClassID() == FWL_Type::DateTimePicker) {
    CFWL_ThemePart part;
    part.m_pWidget = m_pOuter;
    CFX_RectF* pUIMargin =
        static_cast<CFX_RectF*>(m_pOuter->GetThemeProvider()->GetCapacity(
            &part, CFWL_WidgetCapacity::UIMargin));
    if (pUIMargin) {
      m_rtEngine.Deflate(pUIMargin->left, pUIMargin->top, pUIMargin->width,
                         pUIMargin->height);
    }
  }
  FX_BOOL bShowVertScrollbar = IsShowScrollBar(TRUE);
  FX_BOOL bShowHorzScrollbar = IsShowScrollBar(FALSE);
  if (bShowVertScrollbar) {
    InitScrollBar();
    CFX_RectF rtVertScr;
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
      rtVertScr.Set(m_rtClient.right() + kEditMargin, m_rtClient.top, fWidth,
                    m_rtClient.height);
    } else {
      rtVertScr.Set(m_rtClient.right() - fWidth, m_rtClient.top, fWidth,
                    m_rtClient.height);
      if (bShowHorzScrollbar) {
        rtVertScr.height -= fWidth;
      }
      m_rtEngine.width -= fWidth;
    }
    m_pVertScrollBar->SetWidgetRect(rtVertScr);
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
    m_pVertScrollBar->Update();
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
  if (bShowHorzScrollbar) {
    InitScrollBar(FALSE);
    CFX_RectF rtHoriScr;
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
      rtHoriScr.Set(m_rtClient.left, m_rtClient.bottom() + kEditMargin,
                    m_rtClient.width, fWidth);
    } else {
      rtHoriScr.Set(m_rtClient.left, m_rtClient.bottom() - fWidth,
                    m_rtClient.width, fWidth);
      if (bShowVertScrollbar) {
        rtHoriScr.width -= fWidth;
      }
      m_rtEngine.height -= fWidth;
    }
    m_pHorzScrollBar->SetWidgetRect(rtHoriScr);
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
    m_pHorzScrollBar->Update();
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
}
void CFWL_EditImp::LayoutScrollBar() {
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ShowScrollbarFocus) ==
      0) {
    return;
  }
  FX_FLOAT* pfWidth = nullptr;
  FX_BOOL bShowVertScrollbar = IsShowScrollBar(TRUE);
  FX_BOOL bShowHorzScrollbar = IsShowScrollBar(FALSE);
  if (bShowVertScrollbar) {
    if (!m_pVertScrollBar) {
      pfWidth = static_cast<FX_FLOAT*>(
          GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
      FX_FLOAT fWidth = pfWidth ? *pfWidth : 0;
      InitScrollBar();
      CFX_RectF rtVertScr;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
        rtVertScr.Set(m_rtClient.right() + kEditMargin, m_rtClient.top, fWidth,
                      m_rtClient.height);
      } else {
        rtVertScr.Set(m_rtClient.right() - fWidth, m_rtClient.top, fWidth,
                      m_rtClient.height);
        if (bShowHorzScrollbar) {
          rtVertScr.height -= fWidth;
        }
      }
      m_pVertScrollBar->SetWidgetRect(rtVertScr);
      m_pVertScrollBar->Update();
    }
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
  if (bShowHorzScrollbar) {
    if (!m_pHorzScrollBar) {
      if (!pfWidth) {
        pfWidth = static_cast<FX_FLOAT*>(
            GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
      }
      FX_FLOAT fWidth = pfWidth ? *pfWidth : 0;
      InitScrollBar(FALSE);
      CFX_RectF rtHoriScr;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_OuterScrollbar) {
        rtHoriScr.Set(m_rtClient.left, m_rtClient.bottom() + kEditMargin,
                      m_rtClient.width, fWidth);
      } else {
        rtHoriScr.Set(m_rtClient.left, m_rtClient.bottom() - fWidth,
                      m_rtClient.width, fWidth);
        if (bShowVertScrollbar) {
          rtHoriScr.width -= (fWidth);
        }
      }
      m_pHorzScrollBar->SetWidgetRect(rtHoriScr);
      m_pHorzScrollBar->Update();
    }
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
  if (bShowVertScrollbar || bShowHorzScrollbar) {
    UpdateScroll();
  }
}

void CFWL_EditImp::DeviceToEngine(CFX_PointF& pt) {
  pt.x += m_fScrollOffsetX - m_rtEngine.left;
  pt.y += m_fScrollOffsetY - m_rtEngine.top - m_fVAlignOffset;
}

void CFWL_EditImp::InitScrollBar(FX_BOOL bVert) {
  if ((bVert && m_pVertScrollBar) || (!bVert && m_pHorzScrollBar)) {
    return;
  }
  CFWL_WidgetImpProperties prop;
  prop.m_dwStyleExes = bVert ? FWL_STYLEEXT_SCB_Vert : FWL_STYLEEXT_SCB_Horz;
  prop.m_dwStates = FWL_WGTSTATE_Disabled | FWL_WGTSTATE_Invisible;
  prop.m_pParent = m_pInterface;
  prop.m_pThemeProvider = m_pProperties->m_pThemeProvider;
  IFWL_ScrollBar* pScrollBar = IFWL_ScrollBar::Create(prop, m_pInterface);
  pScrollBar->Initialize();
  (bVert ? &m_pVertScrollBar : &m_pHorzScrollBar)->reset(pScrollBar);
}

void CFWL_EditImp::InitEngine() {
  if (!m_pEdtEngine)
    m_pEdtEngine = new CFDE_TxtEdtEngine;
}

FX_BOOL FWL_ShowCaret(IFWL_Widget* pWidget,
                      FX_BOOL bVisible,
                      const CFX_RectF* pRtAnchor) {
  CXFA_FFWidget* pXFAWidget =
      static_cast<CXFA_FFWidget*>(pWidget->GetLayoutItem());
  if (!pXFAWidget)
    return FALSE;

  IXFA_DocProvider* pDocProvider = pXFAWidget->GetDoc()->GetDocProvider();
  if (!pDocProvider)
    return FALSE;

  if (bVisible) {
    CFX_Matrix mt;
    pXFAWidget->GetRotateMatrix(mt);
    CFX_RectF rt(*pRtAnchor);
    mt.TransformRect(rt);
    pDocProvider->DisplayCaret(pXFAWidget, bVisible, &rt);
    return TRUE;
  }
  pDocProvider->DisplayCaret(pXFAWidget, bVisible, pRtAnchor);
  return TRUE;
}

void CFWL_EditImp::ShowCaret(FX_BOOL bVisible, CFX_RectF* pRect) {
  if (m_pCaret) {
    m_pCaret->ShowCaret(bVisible);
    if (bVisible && !pRect->IsEmpty()) {
      m_pCaret->SetWidgetRect(*pRect);
    }
    Repaint(&m_rtEngine);
  } else {
    IFWL_Widget* pOuter = m_pInterface;
    if (bVisible) {
      pRect->Offset(m_pProperties->m_rtWidget.left,
                    m_pProperties->m_rtWidget.top);
    }
    while (pOuter->GetOuter()) {
      pOuter = pOuter->GetOuter();
      if (bVisible) {
        CFX_RectF rtOuter;
        pOuter->GetWidgetRect(rtOuter);
        pRect->Offset(rtOuter.left, rtOuter.top);
      }
    }
    FWL_ShowCaret(pOuter, bVisible, pRect);
  }
}
FX_BOOL CFWL_EditImp::ValidateNumberChar(FX_WCHAR cNum) {
  if (!m_pEdtEngine) {
    return FALSE;
  }
  if (!m_bSetRange) {
    return TRUE;
  }
  CFX_WideString wsOld, wsText;
  m_pEdtEngine->GetText(wsText, 0);
  if (wsText.IsEmpty()) {
    if (cNum == L'0') {
      return FALSE;
    }
    return TRUE;
  }
  int32_t caretPos = m_pEdtEngine->GetCaretPos();
  int32_t iSel = CountSelRanges();
  if (iSel == 0) {
    if (cNum == L'0' && caretPos == 0) {
      return FALSE;
    }
    int32_t nLen = wsText.GetLength();
    CFX_WideString l = wsText.Mid(0, caretPos);
    CFX_WideString r = wsText.Mid(caretPos, nLen - caretPos);
    CFX_WideString wsNew = l + cNum + r;
    if (wsNew.GetInteger() <= m_iMax) {
      return TRUE;
    }
  } else {
    if (wsText.GetInteger() <= m_iMax) {
      return TRUE;
    }
  }
  return FALSE;
}
void CFWL_EditImp::InitCaret() {
  if (!m_pCaret) {
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_InnerCaret)) {
      CFWL_WidgetImpProperties prop;
      m_pCaret.reset(IFWL_Caret::Create(prop, m_pInterface));
      m_pCaret->Initialize();
      m_pCaret->SetParent(m_pInterface);
      m_pCaret->SetStates(m_pProperties->m_dwStates);
    }
  } else if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_InnerCaret) ==
             0) {
    m_pCaret.reset();
  }
}

void CFWL_EditImp::ClearRecord() {
  m_iCurRecord = -1;
  m_DoRecords.clear();
}

void CFWL_EditImp::ProcessInsertError(int32_t iError) {
  switch (iError) {
    case -2: {
      CFWL_EvtEdtTextFull textFullEvent;
      textFullEvent.m_pSrcTarget = m_pInterface;
      DispatchEvent(&textFullEvent);
      break;
    }
    default: {}
  }
}

CFWL_EditImpDelegate::CFWL_EditImpDelegate(CFWL_EditImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_EditImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  CFWL_MessageType dwMsgCode = pMessage->GetClassID();
  switch (dwMsgCode) {
    case CFWL_MessageType::Activate: {
      DoActivate(static_cast<CFWL_MsgActivate*>(pMessage));
      break;
    }
    case CFWL_MessageType::Deactivate: {
      DoDeactivate(static_cast<CFWL_MsgDeactivate*>(pMessage));
      break;
    }
    case CFWL_MessageType::SetFocus:
    case CFWL_MessageType::KillFocus: {
      OnFocusChanged(pMessage, dwMsgCode == CFWL_MessageType::SetFocus);
      break;
    }
    case CFWL_MessageType::Mouse: {
      CFWL_MsgMouse* pMsg = static_cast<CFWL_MsgMouse*>(pMessage);
      switch (pMsg->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown: {
          OnLButtonDown(pMsg);
          break;
        }
        case FWL_MouseCommand::LeftButtonUp: {
          OnLButtonUp(pMsg);
          break;
        }
        case FWL_MouseCommand::LeftButtonDblClk: {
          OnButtonDblClk(pMsg);
          break;
        }
        case FWL_MouseCommand::Move: {
          OnMouseMove(pMsg);
          break;
        }
        case FWL_MouseCommand::RightButtonDown: {
          DoButtonDown(pMsg);
          break;
        }
        default:
          break;
      }
      break;
    }
    case CFWL_MessageType::Key: {
      CFWL_MsgKey* pKey = static_cast<CFWL_MsgKey*>(pMessage);
      if (pKey->m_dwCmd == FWL_KeyCommand::KeyDown)
        OnKeyDown(pKey);
      else if (pKey->m_dwCmd == FWL_KeyCommand::Char)
        OnChar(pKey);
      break;
    }
    default: {
      break;
    }
  }
  CFWL_WidgetImpDelegate::OnProcessMessage(pMessage);
}

void CFWL_EditImpDelegate::OnProcessEvent(CFWL_Event* pEvent) {
  if (!pEvent)
    return;
  if (pEvent->GetClassID() != CFWL_EventType::Scroll)
    return;

  IFWL_Widget* pSrcTarget = pEvent->m_pSrcTarget;
  if ((pSrcTarget == m_pOwner->m_pVertScrollBar.get() &&
       m_pOwner->m_pVertScrollBar) ||
      (pSrcTarget == m_pOwner->m_pHorzScrollBar.get() &&
       m_pOwner->m_pHorzScrollBar)) {
    CFWL_EvtScroll* pScrollEvent = static_cast<CFWL_EvtScroll*>(pEvent);
    OnScroll(static_cast<IFWL_ScrollBar*>(pSrcTarget),
             pScrollEvent->m_iScrollCode, pScrollEvent->m_fPos);
  }
}

void CFWL_EditImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                        const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}

void CFWL_EditImpDelegate::DoActivate(CFWL_MsgActivate* pMsg) {
  m_pOwner->m_pProperties->m_dwStates |= ~FWL_WGTSTATE_Deactivated;
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_EditImpDelegate::DoDeactivate(CFWL_MsgDeactivate* pMsg) {
  m_pOwner->m_pProperties->m_dwStates &= FWL_WGTSTATE_Deactivated;
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_EditImpDelegate::DoButtonDown(CFWL_MsgMouse* pMsg) {
  if ((m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0) {
    m_pOwner->SetFocus(TRUE);
  }
  if (!m_pOwner->m_pEdtEngine) {
    m_pOwner->UpdateEditEngine();
  }
  IFDE_TxtEdtPage* pPage = m_pOwner->m_pEdtEngine->GetPage(0);
  if (!pPage)
    return;
  CFX_PointF pt(pMsg->m_fx, pMsg->m_fy);
  m_pOwner->DeviceToEngine(pt);
  FX_BOOL bBefore = TRUE;
  int32_t nIndex = pPage->GetCharIndex(pt, bBefore);
  if (nIndex < 0) {
    nIndex = 0;
  }
  m_pOwner->m_pEdtEngine->SetCaretPos(nIndex, bBefore);
}
void CFWL_EditImpDelegate::OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet) {
  uint32_t dwStyleEx = m_pOwner->GetStylesEx();
  bool bRepaint = !!(dwStyleEx & FWL_STYLEEXT_EDT_InnerCaret);
  if (bSet) {
    m_pOwner->m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
    if (!m_pOwner->m_pEdtEngine) {
      m_pOwner->UpdateEditEngine();
    }
    m_pOwner->UpdateVAlignment();
    m_pOwner->UpdateOffset();
    m_pOwner->UpdateCaret();
  } else if (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
    m_pOwner->ShowCaret(FALSE);
    if (m_pOwner->m_pEdtEngine &&
        (dwStyleEx & FWL_STYLEEXT_EDT_NoHideSel) == 0) {
      int32_t nSel = m_pOwner->CountSelRanges();
      if (nSel > 0) {
        m_pOwner->ClearSelections();
        bRepaint = TRUE;
      }
      m_pOwner->SetCaretPos(0);
      m_pOwner->UpdateOffset();
    }
    m_pOwner->ClearRecord();
  }
  m_pOwner->LayoutScrollBar();
  if (bRepaint) {
    CFX_RectF rtInvalidate;
    rtInvalidate.Set(0, 0, m_pOwner->m_pProperties->m_rtWidget.width,
                     m_pOwner->m_pProperties->m_rtWidget.height);
    m_pOwner->Repaint(&rtInvalidate);
  }
}
void CFWL_EditImpDelegate::OnLButtonDown(CFWL_MsgMouse* pMsg) {
  DoCursor(pMsg);
  if (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    return;
  }
  m_pOwner->m_bLButtonDown = TRUE;
  m_pOwner->SetGrab(TRUE);
  DoButtonDown(pMsg);
  int32_t nIndex = m_pOwner->m_pEdtEngine->GetCaretPos();
  FX_BOOL bRepaint = FALSE;
  int32_t iCount = m_pOwner->m_pEdtEngine->CountSelRanges();
  if (iCount > 0) {
    m_pOwner->m_pEdtEngine->ClearSelection();
    bRepaint = TRUE;
  }
  FX_BOOL bShift = pMsg->m_dwFlags & FWL_KEYFLAG_Shift;
  if (bShift && m_pOwner->m_nSelStart != nIndex) {
    int32_t iStart = std::min(m_pOwner->m_nSelStart, nIndex);
    int32_t iEnd = std::max(m_pOwner->m_nSelStart, nIndex);
    m_pOwner->m_pEdtEngine->AddSelRange(iStart, iEnd - iStart);
    bRepaint = TRUE;
  } else {
    m_pOwner->m_nSelStart = nIndex;
  }
  if (bRepaint) {
    m_pOwner->Repaint(&m_pOwner->m_rtEngine);
  }
}
void CFWL_EditImpDelegate::OnLButtonUp(CFWL_MsgMouse* pMsg) {
  DoCursor(pMsg);
  m_pOwner->m_bLButtonDown = FALSE;
  m_pOwner->SetGrab(FALSE);
}
void CFWL_EditImpDelegate::OnButtonDblClk(CFWL_MsgMouse* pMsg) {
  if (!m_pOwner->m_pEdtEngine)
    return;
  DoCursor(pMsg);
  IFDE_TxtEdtPage* pPage = m_pOwner->m_pEdtEngine->GetPage(0);
  if (!pPage)
    return;
  CFX_PointF pt(pMsg->m_fx, pMsg->m_fy);
  m_pOwner->DeviceToEngine(pt);
  int32_t nCount = 0;
  int32_t nIndex = pPage->SelectWord(pt, nCount);
  if (nIndex < 0) {
    return;
  }
  m_pOwner->m_pEdtEngine->AddSelRange(nIndex, nCount);
  m_pOwner->m_pEdtEngine->SetCaretPos(nIndex + nCount - 1, FALSE);
  m_pOwner->Repaint(&m_pOwner->m_rtEngine);
}
void CFWL_EditImpDelegate::OnMouseMove(CFWL_MsgMouse* pMsg) {
  if (!m_pOwner->m_pEdtEngine)
    return;
  DoCursor(pMsg);
  if (m_pOwner->m_nSelStart == -1 || !m_pOwner->m_bLButtonDown) {
    return;
  }
  IFDE_TxtEdtPage* pPage = m_pOwner->m_pEdtEngine->GetPage(0);
  if (!pPage)
    return;
  CFX_PointF pt(pMsg->m_fx, pMsg->m_fy);
  m_pOwner->DeviceToEngine(pt);
  FX_BOOL bBefore = TRUE;
  int32_t nIndex = pPage->GetCharIndex(pt, bBefore);
  m_pOwner->m_pEdtEngine->SetCaretPos(nIndex, bBefore);
  nIndex = m_pOwner->m_pEdtEngine->GetCaretPos();
  m_pOwner->m_pEdtEngine->ClearSelection();
  if (nIndex != m_pOwner->m_nSelStart) {
    int32_t nLen = m_pOwner->m_pEdtEngine->GetTextLength();
    if (m_pOwner->m_nSelStart >= nLen) {
      m_pOwner->m_nSelStart = nLen;
    }
    m_pOwner->m_pEdtEngine->AddSelRange(
        std::min(m_pOwner->m_nSelStart, nIndex),
        FXSYS_abs(nIndex - m_pOwner->m_nSelStart));
  }
}
void CFWL_EditImpDelegate::OnKeyDown(CFWL_MsgKey* pMsg) {
  if (!m_pOwner->m_pEdtEngine)
    return;
  FDE_TXTEDTMOVECARET MoveCaret = MC_MoveNone;
  FX_BOOL bShift = pMsg->m_dwFlags & FWL_KEYFLAG_Shift;
  FX_BOOL bCtrl = pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl;
  uint32_t dwKeyCode = pMsg->m_dwKeyCode;
  switch (dwKeyCode) {
    case FWL_VKEY_Left: {
      MoveCaret = MC_Left;
      break;
    }
    case FWL_VKEY_Right: {
      MoveCaret = MC_Right;
      break;
    }
    case FWL_VKEY_Up: {
      MoveCaret = MC_Up;
      break;
    }
    case FWL_VKEY_Down: {
      MoveCaret = MC_Down;
      break;
    }
    case FWL_VKEY_Home: {
      if (bCtrl) {
        MoveCaret = MC_Home;
      } else {
        MoveCaret = MC_LineStart;
      }
      break;
    }
    case FWL_VKEY_End: {
      if (bCtrl) {
        MoveCaret = MC_End;
      } else {
        MoveCaret = MC_LineEnd;
      }
      break;
    }
    case FWL_VKEY_Insert: {
      break;
    }
    case FWL_VKEY_Delete: {
      if ((m_pOwner->m_pProperties->m_dwStyleExes &
           FWL_STYLEEXT_EDT_ReadOnly) ||
          (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
        break;
      }
      int32_t nCaret = m_pOwner->m_pEdtEngine->GetCaretPos();
#if (_FX_OS_ == _FX_MACOSX_)
      m_pOwner->m_pEdtEngine->Delete(nCaret, TRUE);
#else
      m_pOwner->m_pEdtEngine->Delete(nCaret);
#endif
      break;
    }
    case FWL_VKEY_F2: {
      break;
    }
    case FWL_VKEY_Tab: {
      m_pOwner->DispatchKeyEvent(pMsg);
      break;
    }
    default: {
#if (_FX_OS_ == _FX_MACOSX_)
      if (pMsg->m_dwFlags & FWL_KEYFLAG_Command) {
#else
      if (pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl) {
#endif
        if (dwKeyCode == 0x43 || dwKeyCode == 0x63) {
          m_pOwner->DoClipboard(1);
          return;
        }
        if (dwKeyCode == 0x58 || dwKeyCode == 0x78) {
          m_pOwner->DoClipboard(2);
          return;
        }
        if (dwKeyCode == 0x56 || dwKeyCode == 0x76) {
          m_pOwner->DoClipboard(3);
          return;
        }
      }
    }
  }
  if (MoveCaret != MC_MoveNone) {
    m_pOwner->m_pEdtEngine->MoveCaretPos(MoveCaret, bShift, bCtrl);
  }
}
void CFWL_EditImpDelegate::OnChar(CFWL_MsgKey* pMsg) {
  if ((m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_ReadOnly) ||
      (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled)) {
    return;
  }
  if (!m_pOwner->m_pEdtEngine)
    return;
  int32_t iError = 0;
  FX_WCHAR c = (FX_WCHAR)pMsg->m_dwKeyCode;
  int32_t nCaret = m_pOwner->m_pEdtEngine->GetCaretPos();
  switch (c) {
    case FWL_VKEY_Back: {
      m_pOwner->m_pEdtEngine->Delete(nCaret, TRUE);
      break;
    }
    case 0x0A: {
      break;
    }
    case FWL_VKEY_Escape: {
      break;
    }
    case FWL_VKEY_Tab: {
      iError = m_pOwner->m_pEdtEngine->Insert(nCaret, L"\t", 1);
      break;
    }
    case FWL_VKEY_Return: {
      if (m_pOwner->m_pProperties->m_dwStyleExes &
          FWL_STYLEEXT_EDT_WantReturn) {
        iError = m_pOwner->m_pEdtEngine->Insert(nCaret, L"\n", 1);
      }
      break;
    }
    default: {
      if (!m_pOwner->m_pWidgetMgr->IsFormDisabled()) {
        if (m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_EDT_Number) {
          if (((pMsg->m_dwKeyCode < FWL_VKEY_0) &&
               (pMsg->m_dwKeyCode != 0x2E && pMsg->m_dwKeyCode != 0x2D)) ||
              pMsg->m_dwKeyCode > FWL_VKEY_9) {
            break;
          }
          if (!m_pOwner->ValidateNumberChar(c)) {
            break;
          }
        }
      }
#if (_FX_OS_ == _FX_MACOSX_)
      if (pMsg->m_dwFlags & FWL_KEYFLAG_Command)
#else
      if (pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl)
#endif
      {
        break;
      }
      iError = m_pOwner->m_pEdtEngine->Insert(nCaret, &c, 1);
      break;
    }
  }
  if (iError < 0) {
    m_pOwner->ProcessInsertError(iError);
  }
}
FX_BOOL CFWL_EditImpDelegate::OnScroll(IFWL_ScrollBar* pScrollBar,
                                       uint32_t dwCode,
                                       FX_FLOAT fPos) {
  CFX_SizeF fs;
  pScrollBar->GetRange(fs.x, fs.y);
  FX_FLOAT iCurPos = pScrollBar->GetPos();
  FX_FLOAT fStep = pScrollBar->GetStepSize();
  switch (dwCode) {
    case FWL_SCBCODE_Min: {
      fPos = fs.x;
      break;
    }
    case FWL_SCBCODE_Max: {
      fPos = fs.y;
      break;
    }
    case FWL_SCBCODE_StepBackward: {
      fPos -= fStep;
      if (fPos < fs.x + fStep / 2) {
        fPos = fs.x;
      }
      break;
    }
    case FWL_SCBCODE_StepForward: {
      fPos += fStep;
      if (fPos > fs.y - fStep / 2) {
        fPos = fs.y;
      }
      break;
    }
    case FWL_SCBCODE_PageBackward: {
      fPos -= pScrollBar->GetPageSize();
      if (fPos < fs.x) {
        fPos = fs.x;
      }
      break;
    }
    case FWL_SCBCODE_PageForward: {
      fPos += pScrollBar->GetPageSize();
      if (fPos > fs.y) {
        fPos = fs.y;
      }
      break;
    }
    case FWL_SCBCODE_Pos:
    case FWL_SCBCODE_TrackPos: {
      break;
    }
    case FWL_SCBCODE_EndScroll: {
      return FALSE;
    }
    default: {}
  }
  if (iCurPos != fPos) {
    pScrollBar->SetPos(fPos);
    pScrollBar->SetTrackPos(fPos);
    m_pOwner->UpdateOffset(pScrollBar, fPos - iCurPos);
    if (m_pOwner->m_pEdtEngine) {
      m_pOwner->UpdateCaret();
    }
    CFX_RectF rect;
    m_pOwner->GetWidgetRect(rect);
    CFX_RectF rtInvalidate;
    rtInvalidate.Set(0, 0, rect.width + 2, rect.height + 2);
    m_pOwner->Repaint(&rtInvalidate);
  }
  return TRUE;
}
void CFWL_EditImpDelegate::DoCursor(CFWL_MsgMouse* pMsg) {}
