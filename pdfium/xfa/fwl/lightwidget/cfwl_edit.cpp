// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_edit.h"

#include <memory>
#include <vector>

#include "xfa/fwl/basewidget/ifwl_edit.h"

CFWL_Edit* CFWL_Edit::Create() {
  return new CFWL_Edit;
}

FWL_Error CFWL_Edit::Initialize(const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_Edit> pEdit(IFWL_Edit::Create(
      m_pProperties->MakeWidgetImpProperties(nullptr), nullptr));
  FWL_Error ret = pEdit->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pEdit.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_Edit::SetText(const CFX_WideString& wsText) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->SetText(wsText);
}

int32_t CFWL_Edit::GetTextLength() const {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_Edit*>(m_pIface)->GetTextLength();
}

FWL_Error CFWL_Edit::GetText(CFX_WideString& wsText,
                             int32_t nStart,
                             int32_t nCount) const {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->GetText(wsText, nStart, nCount);
}

FWL_Error CFWL_Edit::ClearText() {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->ClearText();
}

int32_t CFWL_Edit::GetCaretPos() const {
  if (!m_pIface)
    return -1;
  return static_cast<IFWL_Edit*>(m_pIface)->GetCaretPos();
}

int32_t CFWL_Edit::SetCaretPos(int32_t nIndex, FX_BOOL bBefore) {
  if (!m_pIface)
    return -1;
  return static_cast<IFWL_Edit*>(m_pIface)->SetCaretPos(nIndex, bBefore);
}

int32_t CFWL_Edit::AddSelRange(int32_t nStart, int32_t nCount) {
  if (!m_pIface)
    return -1;
  static_cast<IFWL_Edit*>(m_pIface)->AddSelRange(nStart, nCount);
  int32_t pos = 0;
  int32_t sum = static_cast<IFWL_Edit*>(m_pIface)->GetTextLength();
  if (nCount == -1) {
    pos = sum;
  } else {
    pos = nStart + nCount;
  }
  return static_cast<IFWL_Edit*>(m_pIface)->SetCaretPos(pos);
}

int32_t CFWL_Edit::CountSelRanges() {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_Edit*>(m_pIface)->CountSelRanges();
}

int32_t CFWL_Edit::GetSelRange(int32_t nIndex, int32_t& nStart) {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_Edit*>(m_pIface)->GetSelRange(nIndex, nStart);
}

FWL_Error CFWL_Edit::ClearSelections() {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->ClearSelections();
}

int32_t CFWL_Edit::GetLimit() {
  if (!m_pIface)
    return -1;
  return static_cast<IFWL_Edit*>(m_pIface)->GetLimit();
}

FWL_Error CFWL_Edit::SetLimit(int32_t nLimit) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->SetLimit(nLimit);
}

FWL_Error CFWL_Edit::SetAliasChar(FX_WCHAR wAlias) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->SetAliasChar(wAlias);
}

FWL_Error CFWL_Edit::Insert(int32_t nStart,
                            const FX_WCHAR* lpText,
                            int32_t nLen) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->Insert(nStart, lpText, nLen);
}

FWL_Error CFWL_Edit::DeleteSelections() {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->DeleteSelections();
}

FWL_Error CFWL_Edit::DeleteRange(int32_t nStart, int32_t nCount) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->DeleteRange(nStart, nCount);
}

FWL_Error CFWL_Edit::Replace(int32_t nStart,
                             int32_t nLen,
                             const CFX_WideStringC& wsReplace) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->Replace(nStart, nLen, wsReplace);
}

FWL_Error CFWL_Edit::DoClipboard(int32_t iCmd) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->DoClipboard(iCmd);
}

FX_BOOL CFWL_Edit::Redo(const IFDE_TxtEdtDoRecord* pRecord) {
  return m_pIface && static_cast<IFWL_Edit*>(m_pIface)->Redo(pRecord);
}

FX_BOOL CFWL_Edit::Undo(const IFDE_TxtEdtDoRecord* pRecord) {
  return m_pIface && static_cast<IFWL_Edit*>(m_pIface)->Undo(pRecord);
}

FWL_Error CFWL_Edit::SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)
      ->SetTabWidth(fTabWidth, bEquidistant);
}

FWL_Error CFWL_Edit::SetNumberRange(int32_t iMin, int32_t iMax) {
  if (iMin > iMax) {
    return FWL_Error::ParameterInvalid;
  }
  return static_cast<IFWL_Edit*>(m_pIface)->SetNumberRange(iMin, iMax);
}

FWL_Error CFWL_Edit::SetBackColor(uint32_t dwColor) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->SetBackColor(dwColor);
}

FWL_Error CFWL_Edit::SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_Edit*>(m_pIface)->SetFont(wsFont, fSize);
}

FX_BOOL CFWL_Edit::CanUndo() {
  return static_cast<IFWL_Edit*>(m_pIface)->CanUndo();
}

FX_BOOL CFWL_Edit::CanRedo() {
  return static_cast<IFWL_Edit*>(m_pIface)->CanRedo();
}

FX_BOOL CFWL_Edit::Undo() {
  return static_cast<IFWL_Edit*>(m_pIface)->Undo();
}

FX_BOOL CFWL_Edit::Redo() {
  return static_cast<IFWL_Edit*>(m_pIface)->Undo();
}

FX_BOOL CFWL_Edit::Copy(CFX_WideString& wsCopy) {
  return static_cast<IFWL_Edit*>(m_pIface)->Copy(wsCopy);
}

FX_BOOL CFWL_Edit::Cut(CFX_WideString& wsCut) {
  return static_cast<IFWL_Edit*>(m_pIface)->Cut(wsCut);
}

FX_BOOL CFWL_Edit::Paste(const CFX_WideString& wsPaste) {
  return static_cast<IFWL_Edit*>(m_pIface)->Paste(wsPaste);
}

FX_BOOL CFWL_Edit::Delete() {
  return static_cast<IFWL_Edit*>(m_pIface)->Delete();
}

void CFWL_Edit::SetScrollOffset(FX_FLOAT fScrollOffset) {
  return static_cast<IFWL_Edit*>(m_pIface)->SetScrollOffset(fScrollOffset);
}

FX_BOOL CFWL_Edit::GetSuggestWords(CFX_PointF pointf,
                                   std::vector<CFX_ByteString>& sSuggest) {
  return static_cast<IFWL_Edit*>(m_pIface)->GetSuggestWords(pointf, sSuggest);
}

FX_BOOL CFWL_Edit::ReplaceSpellCheckWord(CFX_PointF pointf,
                                         const CFX_ByteStringC& bsReplace) {
  return static_cast<IFWL_Edit*>(m_pIface)
      ->ReplaceSpellCheckWord(pointf, bsReplace);
}

CFWL_Edit::CFWL_Edit() {}

CFWL_Edit::~CFWL_Edit() {}
