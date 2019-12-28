// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_combobox.h"

#include <utility>

#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/core/ifwl_widget.h"

CFWL_ComboBox* CFWL_ComboBox::Create() {
  return new CFWL_ComboBox;
}

FWL_Error CFWL_ComboBox::Initialize(const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_ComboBox> pComboBox(IFWL_ComboBox::Create(
      m_pProperties->MakeWidgetImpProperties(&m_comboBoxData)));
  FWL_Error ret = pComboBox->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pComboBox.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

int32_t CFWL_ComboBox::AddString(const CFX_WideStringC& wsText) {
  std::unique_ptr<CFWL_ComboBoxItem> pItem(new CFWL_ComboBoxItem);
  pItem->m_wsText = wsText;
  pItem->m_dwStyles = 0;
  m_comboBoxData.m_ItemArray.push_back(std::move(pItem));
  return m_comboBoxData.m_ItemArray.size() - 1;
}

int32_t CFWL_ComboBox::AddString(const CFX_WideStringC& wsText,
                                 CFX_DIBitmap* pIcon) {
  std::unique_ptr<CFWL_ComboBoxItem> pItem(new CFWL_ComboBoxItem);
  pItem->m_wsText = wsText;
  pItem->m_dwStyles = 0;
  pItem->m_pDIB = pIcon;
  m_comboBoxData.m_ItemArray.push_back(std::move(pItem));
  return m_comboBoxData.m_ItemArray.size() - 1;
}

bool CFWL_ComboBox::RemoveAt(int32_t iIndex) {
  if (iIndex < 0 ||
      static_cast<size_t>(iIndex) >= m_comboBoxData.m_ItemArray.size()) {
    return false;
  }
  m_comboBoxData.m_ItemArray.erase(m_comboBoxData.m_ItemArray.begin() + iIndex);
  return true;
}

void CFWL_ComboBox::RemoveAll() {
  m_comboBoxData.m_ItemArray.clear();
}

int32_t CFWL_ComboBox::CountItems() {
  return m_comboBoxData.CountItems(GetWidget());
}

FWL_Error CFWL_ComboBox::GetTextByIndex(int32_t iIndex,
                                        CFX_WideString& wsText) {
  CFWL_ComboBoxItem* pItem =
      static_cast<CFWL_ComboBoxItem*>(m_comboBoxData.GetItem(m_pIface, iIndex));
  if (!pItem)
    return FWL_Error::Indefinite;
  wsText = pItem->m_wsText;
  return FWL_Error::Succeeded;
}

int32_t CFWL_ComboBox::GetCurSel() {
  if (!m_pIface)
    return -1;
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetCurSel();
}

FWL_Error CFWL_ComboBox::SetCurSel(int32_t iSel) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->SetCurSel(iSel);
}

FWL_Error CFWL_ComboBox::SetEditText(const CFX_WideString& wsText) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->SetEditText(wsText);
}

int32_t CFWL_ComboBox::GetEditTextLength() const {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetEditTextLength();
}

FWL_Error CFWL_ComboBox::GetEditText(CFX_WideString& wsText,
                                     int32_t nStart,
                                     int32_t nCount) const {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)
      ->GetEditText(wsText, nStart, nCount);
}

FWL_Error CFWL_ComboBox::SetEditSelRange(int32_t nStart, int32_t nCount) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->SetEditSelRange(nStart, nCount);
}

int32_t CFWL_ComboBox::GetEditSelRange(int32_t nIndex, int32_t& nStart) {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetEditSelRange(nIndex, nStart);
}

int32_t CFWL_ComboBox::GetEditLimit() {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetEditLimit();
}

FWL_Error CFWL_ComboBox::SetEditLimit(int32_t nLimit) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->SetEditLimit(nLimit);
}

FWL_Error CFWL_ComboBox::EditDoClipboard(int32_t iCmd) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditDoClipboard(iCmd);
}

FX_BOOL CFWL_ComboBox::EditRedo(const IFDE_TxtEdtDoRecord* pRecord) {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditRedo(pRecord);
}

FX_BOOL CFWL_ComboBox::EditUndo(const IFDE_TxtEdtDoRecord* pRecord) {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditUndo(pRecord);
}

FWL_Error CFWL_ComboBox::SetMaxListHeight(FX_FLOAT fMaxHeight) {
  m_comboBoxData.m_fMaxListHeight = fMaxHeight;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ComboBox::SetItemData(int32_t iIndex, void* pData) {
  CFWL_ComboBoxItem* pItem =
      static_cast<CFWL_ComboBoxItem*>(m_comboBoxData.GetItem(m_pIface, iIndex));
  if (!pItem)
    return FWL_Error::Indefinite;
  pItem->m_pData = pData;
  return FWL_Error::Succeeded;
}

void* CFWL_ComboBox::GetItemData(int32_t iIndex) {
  CFWL_ComboBoxItem* pItem =
      static_cast<CFWL_ComboBoxItem*>(m_comboBoxData.GetItem(m_pIface, iIndex));
  return pItem ? pItem->m_pData : nullptr;
}

FWL_Error CFWL_ComboBox::SetListTheme(IFWL_ThemeProvider* pTheme) {
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetListBoxt()->SetThemeProvider(
      pTheme);
}

FX_BOOL CFWL_ComboBox::AfterFocusShowDropList() {
  return static_cast<IFWL_ComboBox*>(m_pIface)->AfterFocusShowDropList();
}

FWL_Error CFWL_ComboBox::OpenDropDownList(FX_BOOL bActivate) {
  return static_cast<IFWL_ComboBox*>(m_pIface)->OpenDropDownList(bActivate);
}

FX_BOOL CFWL_ComboBox::EditCanUndo() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCanUndo();
}

FX_BOOL CFWL_ComboBox::EditCanRedo() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCanRedo();
}

FX_BOOL CFWL_ComboBox::EditUndo() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditUndo();
}

FX_BOOL CFWL_ComboBox::EditRedo() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditRedo();
}

FX_BOOL CFWL_ComboBox::EditCanCopy() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCanCopy();
}

FX_BOOL CFWL_ComboBox::EditCanCut() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCanCut();
}

FX_BOOL CFWL_ComboBox::EditCanSelectAll() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCanSelectAll();
}

FX_BOOL CFWL_ComboBox::EditCopy(CFX_WideString& wsCopy) {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCopy(wsCopy);
}

FX_BOOL CFWL_ComboBox::EditCut(CFX_WideString& wsCut) {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditCut(wsCut);
}

FX_BOOL CFWL_ComboBox::EditPaste(const CFX_WideString& wsPaste) {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditPaste(wsPaste);
}

FX_BOOL CFWL_ComboBox::EditSelectAll() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditSelectAll();
}

FX_BOOL CFWL_ComboBox::EditDelete() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditDelete();
}

FX_BOOL CFWL_ComboBox::EditDeSelect() {
  if (!m_pIface)
    return FALSE;
  return static_cast<IFWL_ComboBox*>(m_pIface)->EditDeSelect();
}

FWL_Error CFWL_ComboBox::GetBBox(CFX_RectF& rect) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)->GetBBox(rect);
}

FWL_Error CFWL_ComboBox::EditModifyStylesEx(uint32_t dwStylesExAdded,
                                            uint32_t dwStylesExRemoved) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ComboBox*>(m_pIface)
      ->EditModifyStylesEx(dwStylesExAdded, dwStylesExRemoved);
}

CFWL_ComboBox::CFWL_ComboBox() {}

CFWL_ComboBox::~CFWL_ComboBox() {}

CFWL_ComboBox::CFWL_ComboBoxDP::CFWL_ComboBoxDP() {
  m_fItemHeight = 0;
  m_fMaxListHeight = 0;
}

CFWL_ComboBox::CFWL_ComboBoxDP::~CFWL_ComboBoxDP() {}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::GetCaption(
    IFWL_Widget* pWidget,
    CFX_WideString& wsCaption) {
  return FWL_Error::Succeeded;
}

int32_t CFWL_ComboBox::CFWL_ComboBoxDP::CountItems(IFWL_Widget* pWidget) {
  return m_ItemArray.size();
}

IFWL_ListItem* CFWL_ComboBox::CFWL_ComboBoxDP::GetItem(IFWL_Widget* pWidget,
                                                       int32_t nIndex) {
  if (nIndex < 0 || static_cast<size_t>(nIndex) >= m_ItemArray.size())
    return nullptr;

  return m_ItemArray[nIndex].get();
}

int32_t CFWL_ComboBox::CFWL_ComboBoxDP::GetItemIndex(IFWL_Widget* pWidget,
                                                     IFWL_ListItem* pItem) {
  auto it = std::find_if(
      m_ItemArray.begin(), m_ItemArray.end(),
      [pItem](const std::unique_ptr<CFWL_ComboBoxItem>& candidate) {
        return candidate.get() == static_cast<CFWL_ComboBoxItem*>(pItem);
      });
  return it != m_ItemArray.end() ? it - m_ItemArray.begin() : -1;
}

FX_BOOL CFWL_ComboBox::CFWL_ComboBoxDP::SetItemIndex(IFWL_Widget* pWidget,
                                                     IFWL_ListItem* pItem,
                                                     int32_t nIndex) {
  if (nIndex < 0 || static_cast<size_t>(nIndex) >= m_ItemArray.size())
    return FALSE;

  m_ItemArray[nIndex].reset(static_cast<CFWL_ComboBoxItem*>(pItem));
  return TRUE;
}

uint32_t CFWL_ComboBox::CFWL_ComboBoxDP::GetItemStyles(IFWL_Widget* pWidget,
                                                       IFWL_ListItem* pItem) {
  if (!pItem)
    return 0;
  return static_cast<CFWL_ComboBoxItem*>(pItem)->m_dwStyles;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::GetItemText(IFWL_Widget* pWidget,
                                                      IFWL_ListItem* pItem,
                                                      CFX_WideString& wsText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  wsText = static_cast<CFWL_ComboBoxItem*>(pItem)->m_wsText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::GetItemRect(IFWL_Widget* pWidget,
                                                      IFWL_ListItem* pItem,
                                                      CFX_RectF& rtItem) {
  if (!pItem)
    return FWL_Error::Indefinite;
  CFWL_ComboBoxItem* pComboItem = static_cast<CFWL_ComboBoxItem*>(pItem);
  rtItem.Set(pComboItem->m_rtItem.left, pComboItem->m_rtItem.top,
             pComboItem->m_rtItem.width, pComboItem->m_rtItem.height);
  return FWL_Error::Succeeded;
}

void* CFWL_ComboBox::CFWL_ComboBoxDP::GetItemData(IFWL_Widget* pWidget,
                                                  IFWL_ListItem* pItem) {
  return pItem ? static_cast<CFWL_ComboBoxItem*>(pItem)->m_pData : nullptr;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::SetItemStyles(IFWL_Widget* pWidget,
                                                        IFWL_ListItem* pItem,
                                                        uint32_t dwStyle) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ComboBoxItem*>(pItem)->m_dwStyles = dwStyle;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::SetItemText(IFWL_Widget* pWidget,
                                                      IFWL_ListItem* pItem,
                                                      const FX_WCHAR* pszText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ComboBoxItem*>(pItem)->m_wsText = pszText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::SetItemRect(IFWL_Widget* pWidget,
                                                      IFWL_ListItem* pItem,
                                                      const CFX_RectF& rtItem) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ComboBoxItem*>(pItem)->m_rtItem = rtItem;
  return FWL_Error::Succeeded;
}

FX_FLOAT CFWL_ComboBox::CFWL_ComboBoxDP::GetItemHeight(IFWL_Widget* pWidget) {
  return m_fItemHeight;
}

CFX_DIBitmap* CFWL_ComboBox::CFWL_ComboBoxDP::GetItemIcon(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem) {
  return pItem ? static_cast<CFWL_ComboBoxItem*>(pItem)->m_pDIB : nullptr;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::GetItemCheckRect(IFWL_Widget* pWidget,
                                                           IFWL_ListItem* pItem,
                                                           CFX_RectF& rtCheck) {
  rtCheck = static_cast<CFWL_ComboBoxItem*>(pItem)->m_rtCheckBox;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::SetItemCheckRect(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem,
    const CFX_RectF& rtCheck) {
  static_cast<CFWL_ComboBoxItem*>(pItem)->m_rtCheckBox = rtCheck;
  return FWL_Error::Succeeded;
}

uint32_t CFWL_ComboBox::CFWL_ComboBoxDP::GetItemCheckState(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem) {
  return static_cast<CFWL_ComboBoxItem*>(pItem)->m_dwCheckState;
}

FWL_Error CFWL_ComboBox::CFWL_ComboBoxDP::SetItemCheckState(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem,
    uint32_t dwCheckState) {
  static_cast<CFWL_ComboBoxItem*>(pItem)->m_dwCheckState = dwCheckState;
  return FWL_Error::Succeeded;
}

FX_FLOAT CFWL_ComboBox::CFWL_ComboBoxDP::GetListHeight(IFWL_Widget* pWidget) {
  return m_fMaxListHeight;
}

CFWL_ComboBoxItem::CFWL_ComboBoxItem() : m_pDIB(nullptr), m_pData(nullptr) {}

CFWL_ComboBoxItem::~CFWL_ComboBoxItem() {}
