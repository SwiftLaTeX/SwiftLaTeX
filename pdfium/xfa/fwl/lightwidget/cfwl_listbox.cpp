// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_listbox.h"

#include <memory>

#include "third_party/base/stl_util.h"

CFWL_ListBox* CFWL_ListBox::Create() {
  return new CFWL_ListBox;
}

FWL_Error CFWL_ListBox::Initialize(const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_ListBox> pListBox(IFWL_ListBox::Create(
      m_pProperties->MakeWidgetImpProperties(&m_ListBoxDP), nullptr));
  FWL_Error ret = pListBox->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pListBox.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::AddDIBitmap(CFX_DIBitmap* pDIB, IFWL_ListItem* pItem) {
  static_cast<CFWL_ListItem*>(pItem)->m_pDIB = pDIB;
  return FWL_Error::Succeeded;
}

IFWL_ListItem* CFWL_ListBox::AddString(const CFX_WideStringC& wsAdd,
                                       FX_BOOL bSelect) {
  std::unique_ptr<CFWL_ListItem> pItem(new CFWL_ListItem);
  pItem->m_dwStates = 0;
  pItem->m_wsText = wsAdd;
  pItem->m_dwStates = bSelect ? FWL_ITEMSTATE_LTB_Selected : 0;
  m_ListBoxDP.m_ItemArray.push_back(std::move(pItem));
  return m_ListBoxDP.m_ItemArray.back().get();
}

FX_BOOL CFWL_ListBox::DeleteString(IFWL_ListItem* pItem) {
  int32_t nIndex = m_ListBoxDP.GetItemIndex(GetWidget(), pItem);
  if (nIndex < 0 ||
      static_cast<size_t>(nIndex) >= m_ListBoxDP.m_ItemArray.size()) {
    return FALSE;
  }
  int32_t iCount = m_ListBoxDP.CountItems(m_pIface);
  int32_t iSel = nIndex + 1;
  if (iSel >= iCount) {
    iSel = nIndex - 1;
    if (iSel < 0) {
      iSel = -1;
    }
  }
  if (iSel >= 0) {
    CFWL_ListItem* pSel =
        static_cast<CFWL_ListItem*>(m_ListBoxDP.GetItem(m_pIface, iSel));
    pSel->m_dwStates |= FWL_ITEMSTATE_LTB_Selected;
  }
  m_ListBoxDP.m_ItemArray.erase(m_ListBoxDP.m_ItemArray.begin() + nIndex);
  return TRUE;
}

void CFWL_ListBox::DeleteAll() {
  m_ListBoxDP.m_ItemArray.clear();
}

int32_t CFWL_ListBox::CountSelItems() {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_ListBox*>(m_pIface)->CountSelItems();
}

IFWL_ListItem* CFWL_ListBox::GetSelItem(int32_t nIndexSel) {
  if (!m_pIface)
    return nullptr;
  return static_cast<IFWL_ListBox*>(m_pIface)->GetSelItem(nIndexSel);
}

int32_t CFWL_ListBox::GetSelIndex(int32_t nIndex) {
  if (!m_pIface)
    return 0;
  return static_cast<IFWL_ListBox*>(m_pIface)->GetSelIndex(nIndex);
}

FWL_Error CFWL_ListBox::SetSelItem(IFWL_ListItem* pItem, FX_BOOL bSelect) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ListBox*>(m_pIface)->SetSelItem(pItem, bSelect);
}

FWL_Error CFWL_ListBox::GetItemText(IFWL_ListItem* pItem,
                                    CFX_WideString& wsText) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ListBox*>(m_pIface)->GetItemText(pItem, wsText);
}

FWL_Error CFWL_ListBox::GetScrollPos(FX_FLOAT& fPos, FX_BOOL bVert) {
  if (!m_pIface)
    return FWL_Error::Indefinite;
  return static_cast<IFWL_ListBox*>(m_pIface)->GetScrollPos(fPos, bVert);
}

FWL_Error CFWL_ListBox::SetItemHeight(FX_FLOAT fItemHeight) {
  m_ListBoxDP.m_fItemHeight = fItemHeight;
  return FWL_Error::Succeeded;
}

IFWL_ListItem* CFWL_ListBox::GetFocusItem() {
  for (const auto& pItem : m_ListBoxDP.m_ItemArray) {
    if (pItem->m_dwStates & FWL_ITEMSTATE_LTB_Focused)
      return pItem.get();
  }
  return nullptr;
}

FWL_Error CFWL_ListBox::SetFocusItem(IFWL_ListItem* pItem) {
  int32_t nIndex = m_ListBoxDP.GetItemIndex(GetWidget(), pItem);
  m_ListBoxDP.m_ItemArray[nIndex]->m_dwStates |= FWL_ITEMSTATE_LTB_Focused;
  return FWL_Error::Succeeded;
}

int32_t CFWL_ListBox::CountItems() {
  return pdfium::CollectionSize<int32_t>(m_ListBoxDP.m_ItemArray);
}

IFWL_ListItem* CFWL_ListBox::GetItem(int32_t nIndex) {
  if (nIndex < 0 || nIndex >= CountItems())
    return nullptr;

  return m_ListBoxDP.m_ItemArray[nIndex].get();
}

FWL_Error CFWL_ListBox::SetItemString(IFWL_ListItem* pItem,
                                      const CFX_WideStringC& wsText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ListItem*>(pItem)->m_wsText = wsText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::GetItemString(IFWL_ListItem* pItem,
                                      CFX_WideString& wsText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  wsText = static_cast<CFWL_ListItem*>(pItem)->m_wsText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::SetItemData(IFWL_ListItem* pItem, void* pData) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ListItem*>(pItem)->m_pData = pData;
  return FWL_Error::Succeeded;
}

void* CFWL_ListBox::GetItemData(IFWL_ListItem* pItem) {
  return pItem ? static_cast<CFWL_ListItem*>(pItem)->m_pData : nullptr;
}

IFWL_ListItem* CFWL_ListBox::GetItemAtPoint(FX_FLOAT fx, FX_FLOAT fy) {
  CFX_RectF rtClient;
  m_pIface->GetClientRect(rtClient);
  fx -= rtClient.left;
  fy -= rtClient.top;
  FX_FLOAT fPosX = 0;
  FX_FLOAT fPosY = 0;
  static_cast<IFWL_ListBox*>(m_pIface)->GetScrollPos(fx);
  static_cast<IFWL_ListBox*>(m_pIface)->GetScrollPos(fy, FALSE);
  int32_t nCount = m_ListBoxDP.CountItems(nullptr);
  for (int32_t i = 0; i < nCount; i++) {
    IFWL_ListItem* pItem = m_ListBoxDP.GetItem(nullptr, i);
    if (!pItem) {
      continue;
    }
    CFX_RectF rtItem;
    m_ListBoxDP.GetItemRect(nullptr, pItem, rtItem);
    rtItem.Offset(-fPosX, -fPosY);
    if (rtItem.Contains(fx, fy)) {
      return pItem;
    }
  }
  return nullptr;
}

uint32_t CFWL_ListBox::GetItemStates(IFWL_ListItem* pItem) {
  if (!pItem)
    return 0;
  CFWL_ListItem* pListItem = static_cast<CFWL_ListItem*>(pItem);
  return pListItem->m_dwStates | pListItem->m_dwCheckState;
}

CFWL_ListBox::CFWL_ListBox() {}

CFWL_ListBox::~CFWL_ListBox() {}

CFWL_ListBox::CFWL_ListBoxDP::CFWL_ListBoxDP() {}

CFWL_ListBox::CFWL_ListBoxDP::~CFWL_ListBoxDP() {}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::GetCaption(IFWL_Widget* pWidget,
                                                   CFX_WideString& wsCaption) {
  wsCaption = m_wsData;
  return FWL_Error::Succeeded;
}

int32_t CFWL_ListBox::CFWL_ListBoxDP::CountItems(IFWL_Widget* pWidget) {
  return pdfium::CollectionSize<int32_t>(m_ItemArray);
}

IFWL_ListItem* CFWL_ListBox::CFWL_ListBoxDP::GetItem(IFWL_Widget* pWidget,
                                                     int32_t nIndex) {
  if (nIndex < 0 || nIndex >= CountItems(pWidget))
    return nullptr;

  return m_ItemArray[nIndex].get();
}

int32_t CFWL_ListBox::CFWL_ListBoxDP::GetItemIndex(IFWL_Widget* pWidget,
                                                   IFWL_ListItem* pItem) {
  auto it = std::find_if(
      m_ItemArray.begin(), m_ItemArray.end(),
      [pItem](const std::unique_ptr<CFWL_ListItem>& candidate) {
        return candidate.get() == static_cast<CFWL_ListItem*>(pItem);
      });
  return it != m_ItemArray.end() ? it - m_ItemArray.begin() : -1;
}

FX_BOOL CFWL_ListBox::CFWL_ListBoxDP::SetItemIndex(IFWL_Widget* pWidget,
                                                   IFWL_ListItem* pItem,
                                                   int32_t nIndex) {
  if (nIndex < 0 || nIndex >= CountItems(pWidget))
    return FALSE;
  m_ItemArray[nIndex].reset(static_cast<CFWL_ListItem*>(pItem));
  return TRUE;
}

uint32_t CFWL_ListBox::CFWL_ListBoxDP::GetItemStyles(IFWL_Widget* pWidget,
                                                     IFWL_ListItem* pItem) {
  if (!pItem)
    return 0;
  return static_cast<CFWL_ListItem*>(pItem)->m_dwStates;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::GetItemText(IFWL_Widget* pWidget,
                                                    IFWL_ListItem* pItem,
                                                    CFX_WideString& wsText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  wsText = static_cast<CFWL_ListItem*>(pItem)->m_wsText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::GetItemRect(IFWL_Widget* pWidget,
                                                    IFWL_ListItem* pItem,
                                                    CFX_RectF& rtItem) {
  if (!pItem)
    return FWL_Error::Indefinite;
  rtItem = static_cast<CFWL_ListItem*>(pItem)->m_rtItem;
  return FWL_Error::Succeeded;
}

void* CFWL_ListBox::CFWL_ListBoxDP::GetItemData(IFWL_Widget* pWidget,
                                                IFWL_ListItem* pItem) {
  return pItem ? static_cast<CFWL_ListItem*>(pItem)->m_pData : nullptr;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::SetItemStyles(IFWL_Widget* pWidget,
                                                      IFWL_ListItem* pItem,
                                                      uint32_t dwStyle) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ListItem*>(pItem)->m_dwStates = dwStyle;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::SetItemText(IFWL_Widget* pWidget,
                                                    IFWL_ListItem* pItem,
                                                    const FX_WCHAR* pszText) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ListItem*>(pItem)->m_wsText = pszText;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::SetItemRect(IFWL_Widget* pWidget,
                                                    IFWL_ListItem* pItem,
                                                    const CFX_RectF& rtItem) {
  if (!pItem)
    return FWL_Error::Indefinite;
  static_cast<CFWL_ListItem*>(pItem)->m_rtItem = rtItem;
  return FWL_Error::Succeeded;
}

FX_FLOAT CFWL_ListBox::CFWL_ListBoxDP::GetItemHeight(IFWL_Widget* pWidget) {
  return m_fItemHeight;
}

CFX_DIBitmap* CFWL_ListBox::CFWL_ListBoxDP::GetItemIcon(IFWL_Widget* pWidget,
                                                        IFWL_ListItem* pItem) {
  return static_cast<CFWL_ListItem*>(pItem)->m_pDIB;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::GetItemCheckRect(IFWL_Widget* pWidget,
                                                         IFWL_ListItem* pItem,
                                                         CFX_RectF& rtCheck) {
  rtCheck = static_cast<CFWL_ListItem*>(pItem)->m_rtCheckBox;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::SetItemCheckRect(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem,
    const CFX_RectF& rtCheck) {
  static_cast<CFWL_ListItem*>(pItem)->m_rtCheckBox = rtCheck;
  return FWL_Error::Succeeded;
}

uint32_t CFWL_ListBox::CFWL_ListBoxDP::GetItemCheckState(IFWL_Widget* pWidget,
                                                         IFWL_ListItem* pItem) {
  return static_cast<CFWL_ListItem*>(pItem)->m_dwCheckState;
}

FWL_Error CFWL_ListBox::CFWL_ListBoxDP::SetItemCheckState(
    IFWL_Widget* pWidget,
    IFWL_ListItem* pItem,
    uint32_t dwCheckState) {
  static_cast<CFWL_ListItem*>(pItem)->m_dwCheckState = dwCheckState;
  return FWL_Error::Succeeded;
}

CFWL_ListItem::CFWL_ListItem() {
  m_rtItem.Reset();
  m_dwStates = 0;
  m_wsText = L"";
  m_pDIB = nullptr;
  m_pData = nullptr;
  m_dwCheckState = 0;
  m_rtCheckBox.Reset();
}

CFWL_ListItem::~CFWL_ListItem() {}
