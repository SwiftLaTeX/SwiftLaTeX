// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_listboximp.h"

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/fwl_comboboximp.h"
#include "xfa/fwl/basewidget/fwl_scrollbarimp.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themepart.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"

namespace {

const int kItemTextMargin = 2;

}  // namespace

// static
IFWL_ListBox* IFWL_ListBox::Create(const CFWL_WidgetImpProperties& properties,
                                   IFWL_Widget* pOuter) {
  IFWL_ListBox* pListBox = new IFWL_ListBox;
  CFWL_ListBoxImp* pListBoxImpl = new CFWL_ListBoxImp(properties, pOuter);
  pListBox->SetImpl(pListBoxImpl);
  pListBoxImpl->SetInterface(pListBox);
  return pListBox;
}
// static
IFWL_ListBox* IFWL_ListBox::CreateComboList(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter) {
  IFWL_ListBox* pListBox = new IFWL_ListBox;
  CFWL_ListBoxImp* pComboListImpl = new CFWL_ComboListImp(properties, pOuter);
  pListBox->SetImpl(pComboListImpl);
  pComboListImpl->SetInterface(pListBox);
  return pListBox;
}
IFWL_ListBox::IFWL_ListBox() {}
int32_t IFWL_ListBox::CountSelItems() {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->CountSelItems();
}
IFWL_ListItem* IFWL_ListBox::GetSelItem(int32_t nIndexSel) {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->GetSelItem(nIndexSel);
}
int32_t IFWL_ListBox::GetSelIndex(int32_t nIndex) {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->GetSelIndex(nIndex);
}
FWL_Error IFWL_ListBox::SetSelItem(IFWL_ListItem* pItem, FX_BOOL bSelect) {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->SetSelItem(pItem, bSelect);
}
FWL_Error IFWL_ListBox::GetItemText(IFWL_ListItem* pItem,
                                    CFX_WideString& wsText) {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->GetItemText(pItem, wsText);
}
FWL_Error IFWL_ListBox::GetScrollPos(FX_FLOAT& fPos, FX_BOOL bVert) {
  return static_cast<CFWL_ListBoxImp*>(GetImpl())->GetScrollPos(fPos, bVert);
}

CFWL_ListBoxImp::CFWL_ListBoxImp(const CFWL_WidgetImpProperties& properties,
                                 IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_dwTTOStyles(0),
      m_iTTOAligns(0),
      m_hAnchor(nullptr),
      m_fScorllBarWidth(0),
      m_bLButtonDown(FALSE),
      m_pScrollBarTP(nullptr) {
  m_rtClient.Reset();
  m_rtConent.Reset();
  m_rtStatic.Reset();
}

CFWL_ListBoxImp::~CFWL_ListBoxImp() {}

FWL_Error CFWL_ListBoxImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_ListBox;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_ListBoxImp::GetClassID() const {
  return FWL_Type::ListBox;
}

FWL_Error CFWL_ListBoxImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;

  m_pDelegate = new CFWL_ListBoxImpDelegate(this);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBoxImp::Finalize() {
  if (m_pVertScrollBar) {
    m_pVertScrollBar->Finalize();
  }
  if (m_pHorzScrollBar) {
    m_pHorzScrollBar->Finalize();
  }
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_ListBoxImp::GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize) {
  if (bAutoSize) {
    rect.Set(0, 0, 0, 0);
    if (!m_pProperties->m_pThemeProvider) {
      m_pProperties->m_pThemeProvider = GetAvailableTheme();
    }
    CFX_SizeF fs = CalcSize(TRUE);
    rect.Set(0, 0, fs.x, fs.y);
    CFWL_WidgetImp::GetWidgetRect(rect, TRUE);
  } else {
    rect = m_pProperties->m_rtWidget;
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_ListBoxImp::Update() {
  if (IsLocked()) {
    return FWL_Error::Indefinite;
  }
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  m_iTTOAligns = FDE_TTOALIGNMENT_Center;
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_AlignMask) {
    case FWL_STYLEEXT_LTB_LeftAlign: {
      m_iTTOAligns = FDE_TTOALIGNMENT_CenterLeft;
      break;
    }
    case FWL_STYLEEXT_LTB_RightAlign: {
      m_iTTOAligns = FDE_TTOALIGNMENT_CenterRight;
      break;
    }
    case FWL_STYLEEXT_LTB_CenterAlign:
    default: { m_iTTOAligns = FDE_TTOALIGNMENT_Center; }
  }
  if (m_pProperties->m_dwStyleExes & FWL_WGTSTYLE_RTLReading) {
    m_dwTTOStyles |= FDE_TTOSTYLE_RTL;
  }
  m_dwTTOStyles |= FDE_TTOSTYLE_SingleLine;
  m_fScorllBarWidth = GetScrollWidth();
  CalcSize();
  return FWL_Error::Succeeded;
}
FWL_WidgetHit CFWL_ListBoxImp::HitTest(FX_FLOAT fx, FX_FLOAT fy) {
  if (IsShowScrollBar(FALSE)) {
    CFX_RectF rect;
    m_pHorzScrollBar->GetWidgetRect(rect);
    if (rect.Contains(fx, fy))
      return FWL_WidgetHit::HScrollBar;
  }
  if (IsShowScrollBar(TRUE)) {
    CFX_RectF rect;
    m_pVertScrollBar->GetWidgetRect(rect);
    if (rect.Contains(fx, fy))
      return FWL_WidgetHit::VScrollBar;
  }
  if (m_rtClient.Contains(fx, fy))
    return FWL_WidgetHit::Client;
  return FWL_WidgetHit::Unknown;
}
FWL_Error CFWL_ListBoxImp::DrawWidget(CFX_Graphics* pGraphics,
                                      const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider)
    return FWL_Error::Indefinite;
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  pGraphics->SaveGraphState();
  if (HasBorder()) {
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, pMatrix);
  }
  if (HasEdge()) {
    DrawEdge(pGraphics, CFWL_Part::Edge, pTheme, pMatrix);
  }
  CFX_RectF rtClip(m_rtConent);
  if (IsShowScrollBar(FALSE)) {
    rtClip.height -= m_fScorllBarWidth;
  }
  if (IsShowScrollBar(TRUE)) {
    rtClip.width -= m_fScorllBarWidth;
  }
  if (pMatrix) {
    pMatrix->TransformRect(rtClip);
  }
  pGraphics->SetClipRect(rtClip);
  if ((m_pProperties->m_dwStyles & FWL_WGTSTYLE_NoBackground) == 0) {
    DrawBkground(pGraphics, pTheme, pMatrix);
  }
  DrawItems(pGraphics, pTheme, pMatrix);
  pGraphics->RestoreGraphState();
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_ListBoxImp::SetThemeProvider(
    IFWL_ThemeProvider* pThemeProvider) {
  if (!pThemeProvider)
    return FWL_Error::Indefinite;
  if (!pThemeProvider->IsValidWidget(m_pInterface)) {
    m_pScrollBarTP = pThemeProvider;
    return FWL_Error::Succeeded;
  }
  m_pProperties->m_pThemeProvider = pThemeProvider;
  return FWL_Error::Succeeded;
}
int32_t CFWL_ListBoxImp::CountSelItems() {
  if (!m_pProperties->m_pDataProvider)
    return 0;
  int32_t iRet = 0;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      continue;
    }
    uint32_t dwStyle = pData->GetItemStyles(m_pInterface, pItem);
    if (dwStyle & FWL_ITEMSTATE_LTB_Selected) {
      iRet++;
    }
  }
  return iRet;
}
IFWL_ListItem* CFWL_ListBoxImp::GetSelItem(int32_t nIndexSel) {
  if (!m_pProperties->m_pDataProvider)
    return nullptr;
  int32_t index = 0;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      return nullptr;
    }
    uint32_t dwStyle = pData->GetItemStyles(m_pInterface, pItem);
    if (dwStyle & FWL_ITEMSTATE_LTB_Selected) {
      if (index == nIndexSel) {
        return pItem;
      } else {
        index++;
      }
    }
  }
  return nullptr;
}
int32_t CFWL_ListBoxImp::GetSelIndex(int32_t nIndex) {
  if (!m_pProperties->m_pDataProvider)
    return -1;
  int32_t index = 0;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      return -1;
    }
    uint32_t dwStyle = pData->GetItemStyles(m_pInterface, pItem);
    if (dwStyle & FWL_ITEMSTATE_LTB_Selected) {
      if (index == nIndex) {
        return i;
      } else {
        index++;
      }
    }
  }
  return -1;
}
FWL_Error CFWL_ListBoxImp::SetSelItem(IFWL_ListItem* pItem, FX_BOOL bSelect) {
  if (!m_pProperties->m_pDataProvider)
    return FWL_Error::Indefinite;
  if (!pItem) {
    if (bSelect) {
      SelectAll();
    } else {
      ClearSelection();
      SetFocusItem(nullptr);
    }
    return FWL_Error::Indefinite;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiSelection) {
    SetSelectionDirect(pItem, bSelect);
  } else {
    SetSelection(pItem, pItem, bSelect);
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_ListBoxImp::GetItemText(IFWL_ListItem* pItem,
                                       CFX_WideString& wsText) {
  if (!m_pProperties->m_pDataProvider)
    return FWL_Error::Indefinite;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  if (!pItem)
    return FWL_Error::Indefinite;
  pData->GetItemText(m_pInterface, pItem, wsText);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_ListBoxImp::GetScrollPos(FX_FLOAT& fPos, FX_BOOL bVert) {
  if ((bVert && IsShowScrollBar(TRUE)) || (!bVert && IsShowScrollBar(FALSE))) {
    IFWL_ScrollBar* pScrollBar =
        bVert ? m_pVertScrollBar.get() : m_pHorzScrollBar.get();
    fPos = pScrollBar->GetPos();
    return FWL_Error::Succeeded;
  }
  return FWL_Error::Indefinite;
}

IFWL_ListItem* CFWL_ListBoxImp::GetItem(IFWL_ListItem* pItem,
                                        uint32_t dwKeyCode) {
  IFWL_ListItem* hRet = nullptr;
  switch (dwKeyCode) {
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Home:
    case FWL_VKEY_End: {
      const bool bUp = dwKeyCode == FWL_VKEY_Up;
      const bool bDown = dwKeyCode == FWL_VKEY_Down;
      const bool bHome = dwKeyCode == FWL_VKEY_Home;
      IFWL_ListBoxDP* pData =
          static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
      int32_t iDstItem = -1;
      if (bUp || bDown) {
        int32_t index = pData->GetItemIndex(m_pInterface, pItem);
        iDstItem = dwKeyCode == FWL_VKEY_Up ? index - 1 : index + 1;
      } else if (bHome) {
        iDstItem = 0;
      } else {
        int32_t iCount = pData->CountItems(m_pInterface);
        iDstItem = iCount - 1;
      }
      hRet = pData->GetItem(m_pInterface, iDstItem);
      break;
    }
    default: {}
  }
  return hRet;
}
void CFWL_ListBoxImp::SetSelection(IFWL_ListItem* hStart,
                                   IFWL_ListItem* hEnd,
                                   FX_BOOL bSelected) {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iStart = pData->GetItemIndex(m_pInterface, hStart);
  int32_t iEnd = pData->GetItemIndex(m_pInterface, hEnd);
  if (iStart > iEnd) {
    int32_t iTemp = iStart;
    iStart = iEnd;
    iEnd = iTemp;
  }
  if (bSelected) {
    int32_t iCount = pData->CountItems(m_pInterface);
    for (int32_t i = 0; i < iCount; i++) {
      IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
      SetSelectionDirect(pItem, FALSE);
    }
  }
  for (; iStart <= iEnd; iStart++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, iStart);
    SetSelectionDirect(pItem, bSelected);
  }
}
void CFWL_ListBoxImp::SetSelectionDirect(IFWL_ListItem* pItem,
                                         FX_BOOL bSelect) {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  uint32_t dwOldStyle = pData->GetItemStyles(m_pInterface, pItem);
  bSelect ? dwOldStyle |= FWL_ITEMSTATE_LTB_Selected
          : dwOldStyle &= ~FWL_ITEMSTATE_LTB_Selected;
  pData->SetItemStyles(m_pInterface, pItem, dwOldStyle);
}
FX_BOOL CFWL_ListBoxImp::IsItemSelected(IFWL_ListItem* pItem) {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  uint32_t dwState = pData->GetItemStyles(m_pInterface, pItem);
  return (dwState & FWL_ITEMSTATE_LTB_Selected) != 0;
}
void CFWL_ListBoxImp::ClearSelection() {
  FX_BOOL bMulti =
      m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiSelection;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    uint32_t dwState = pData->GetItemStyles(m_pInterface, pItem);
    if (!(dwState & FWL_ITEMSTATE_LTB_Selected))
      continue;
    SetSelectionDirect(pItem, FALSE);
    if (!bMulti)
      return;
  }
}
void CFWL_ListBoxImp::SelectAll() {
  FX_BOOL bMulti =
      m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiSelection;
  if (!bMulti) {
    return;
  }
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  if (iCount > 0) {
    IFWL_ListItem* pItemStart = pData->GetItem(m_pInterface, 0);
    IFWL_ListItem* pItemEnd = pData->GetItem(m_pInterface, iCount - 1);
    SetSelection(pItemStart, pItemEnd, FALSE);
  }
}
IFWL_ListItem* CFWL_ListBoxImp::GetFocusedItem() {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem)
      return nullptr;
    if (pData->GetItemStyles(m_pInterface, pItem) & FWL_ITEMSTATE_LTB_Focused) {
      return pItem;
    }
  }
  return nullptr;
}
void CFWL_ListBoxImp::SetFocusItem(IFWL_ListItem* pItem) {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  IFWL_ListItem* hFocus = GetFocusedItem();
  if (pItem != hFocus) {
    if (hFocus) {
      uint32_t dwStyle = pData->GetItemStyles(m_pInterface, hFocus);
      dwStyle &= ~FWL_ITEMSTATE_LTB_Focused;
      pData->SetItemStyles(m_pInterface, hFocus, dwStyle);
    }
    if (pItem) {
      uint32_t dwStyle = pData->GetItemStyles(m_pInterface, pItem);
      dwStyle |= FWL_ITEMSTATE_LTB_Focused;
      pData->SetItemStyles(m_pInterface, pItem, dwStyle);
    }
  }
}
IFWL_ListItem* CFWL_ListBoxImp::GetItemAtPoint(FX_FLOAT fx, FX_FLOAT fy) {
  fx -= m_rtConent.left, fy -= m_rtConent.top;
  FX_FLOAT fPosX = 0.0f;
  if (m_pHorzScrollBar) {
    fPosX = m_pHorzScrollBar->GetPos();
  }
  FX_FLOAT fPosY = 0.0;
  if (m_pVertScrollBar) {
    fPosY = m_pVertScrollBar->GetPos();
  }
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t nCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < nCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      continue;
    }
    CFX_RectF rtItem;
    pData->GetItemRect(m_pInterface, pItem, rtItem);
    rtItem.Offset(-fPosX, -fPosY);
    if (rtItem.Contains(fx, fy)) {
      return pItem;
    }
  }
  return nullptr;
}
FX_BOOL CFWL_ListBoxImp::GetItemCheckRect(IFWL_ListItem* pItem,
                                          CFX_RectF& rtCheck) {
  if (!m_pProperties->m_pDataProvider)
    return FALSE;
  if (!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_Check)) {
    return FALSE;
  }
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  pData->GetItemCheckRect(m_pInterface, pItem, rtCheck);
  return TRUE;
}
FX_BOOL CFWL_ListBoxImp::GetItemChecked(IFWL_ListItem* pItem) {
  if (!m_pProperties->m_pDataProvider)
    return FALSE;
  if (!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_Check)) {
    return FALSE;
  }
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  return (pData->GetItemCheckState(m_pInterface, pItem) &
          FWL_ITEMSTATE_LTB_Checked);
}
FX_BOOL CFWL_ListBoxImp::SetItemChecked(IFWL_ListItem* pItem,
                                        FX_BOOL bChecked) {
  if (!m_pProperties->m_pDataProvider)
    return FALSE;
  if (!(m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_Check)) {
    return FALSE;
  }
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  pData->SetItemCheckState(m_pInterface, pItem,
                           bChecked ? FWL_ITEMSTATE_LTB_Checked : 0);
  return TRUE;
}
FX_BOOL CFWL_ListBoxImp::ScrollToVisible(IFWL_ListItem* pItem) {
  if (!m_pVertScrollBar)
    return FALSE;
  CFX_RectF rtItem;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  pData->GetItemRect(m_pInterface, pItem, rtItem);
  FX_BOOL bScroll = FALSE;
  FX_FLOAT fPosY = m_pVertScrollBar->GetPos();
  rtItem.Offset(0, -fPosY + m_rtConent.top);
  if (rtItem.top < m_rtConent.top) {
    fPosY += rtItem.top - m_rtConent.top;
    bScroll = TRUE;
  } else if (rtItem.bottom() > m_rtConent.bottom()) {
    fPosY += rtItem.bottom() - m_rtConent.bottom();
    bScroll = TRUE;
  }
  if (!bScroll) {
    return FALSE;
  }
  m_pVertScrollBar->SetPos(fPosY);
  m_pVertScrollBar->SetTrackPos(fPosY);
  Repaint(&m_rtClient);
  return TRUE;
}
void CFWL_ListBoxImp::DrawBkground(CFX_Graphics* pGraphics,
                                   IFWL_ThemeProvider* pTheme,
                                   const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return;
  if (!pTheme)
    return;
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = 0;
  param.m_pGraphics = pGraphics;
  param.m_matrix.Concat(*pMatrix);
  param.m_rtPart = m_rtClient;
  if (IsShowScrollBar(FALSE) && IsShowScrollBar(TRUE)) {
    param.m_pData = &m_rtStatic;
  }
  if (!IsEnabled()) {
    param.m_dwStates = CFWL_PartState_Disabled;
  }
  pTheme->DrawBackground(&param);
}
void CFWL_ListBoxImp::DrawItems(CFX_Graphics* pGraphics,
                                IFWL_ThemeProvider* pTheme,
                                const CFX_Matrix* pMatrix) {
  FX_FLOAT fPosX = 0.0f;
  if (m_pHorzScrollBar) {
    fPosX = m_pHorzScrollBar->GetPos();
  }
  FX_FLOAT fPosY = 0.0f;
  if (m_pVertScrollBar) {
    fPosY = m_pVertScrollBar->GetPos();
  }
  CFX_RectF rtView(m_rtConent);
  if (m_pHorzScrollBar) {
    rtView.height -= m_fScorllBarWidth;
  }
  if (m_pVertScrollBar) {
    rtView.width -= m_fScorllBarWidth;
  }
  FX_BOOL bMultiCol =
      m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiColumn;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      continue;
    }
    CFX_RectF rtItem;
    pData->GetItemRect(m_pInterface, pItem, rtItem);
    rtItem.Offset(m_rtConent.left - fPosX, m_rtConent.top - fPosY);
    if (rtItem.bottom() < m_rtConent.top) {
      continue;
    }
    if (rtItem.top >= m_rtConent.bottom()) {
      break;
    }
    if (bMultiCol && rtItem.left > m_rtConent.right()) {
      break;
    }
    if (GetStylesEx() & FWL_STYLEEXT_LTB_OwnerDraw) {
      CFWL_EvtLtbDrawItem ev;
      ev.m_pSrcTarget = m_pInterface;
      ev.m_pGraphics = pGraphics;
      ev.m_matrix = *pMatrix;
      ev.m_index = i;
      ev.m_rect = rtItem;
      DispatchEvent(&ev);
    } else {
      DrawItem(pGraphics, pTheme, pItem, i, rtItem, pMatrix);
    }
  }
}
void CFWL_ListBoxImp::DrawItem(CFX_Graphics* pGraphics,
                               IFWL_ThemeProvider* pTheme,
                               IFWL_ListItem* pItem,
                               int32_t Index,
                               const CFX_RectF& rtItem,
                               const CFX_Matrix* pMatrix) {
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  uint32_t dwItemStyles = pData->GetItemStyles(m_pInterface, pItem);
  uint32_t dwPartStates = CFWL_PartState_Normal;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    dwPartStates = CFWL_PartState_Disabled;
  } else if (dwItemStyles & FWL_ITEMSTATE_LTB_Selected) {
    dwPartStates = CFWL_PartState_Selected;
  }
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused &&
      dwItemStyles & FWL_ITEMSTATE_LTB_Focused) {
    dwPartStates |= CFWL_PartState_Focused;
  }
  {
    CFWL_ThemeBackground param;
    param.m_pWidget = m_pInterface;
    param.m_iPart = CFWL_Part::ListItem;
    param.m_dwStates = dwPartStates;
    param.m_pGraphics = pGraphics;
    param.m_matrix.Concat(*pMatrix);
    param.m_rtPart = rtItem;
    param.m_bMaximize = true;
    CFX_RectF rtFocus(rtItem);
    param.m_pData = &rtFocus;
    if (m_pVertScrollBar && !m_pHorzScrollBar &&
        (dwPartStates & CFWL_PartState_Focused)) {
      param.m_rtPart.left += 1;
      param.m_rtPart.width -= (m_fScorllBarWidth + 1);
      rtFocus.Deflate(0.5, 0.5, 1 + m_fScorllBarWidth, 1);
    }
    pTheme->DrawBackground(&param);
  }
  {
    FX_BOOL bHasIcon = GetStylesEx() & FWL_STYLEEXT_LTB_Icon;
    if (bHasIcon) {
      CFX_RectF rtDIB;
      CFX_DIBitmap* pDib = pData->GetItemIcon(m_pInterface, pItem);
      rtDIB.Set(rtItem.left, rtItem.top, rtItem.height, rtItem.height);
      if (pDib) {
        CFWL_ThemeBackground param;
        param.m_pWidget = m_pInterface;
        param.m_iPart = CFWL_Part::Icon;
        param.m_pGraphics = pGraphics;
        param.m_matrix.Concat(*pMatrix);
        param.m_rtPart = rtDIB;
        param.m_bMaximize = true;
        param.m_pImage = pDib;
        pTheme->DrawBackground(&param);
      }
    }
    FX_BOOL bHasCheck = GetStylesEx() & FWL_STYLEEXT_LTB_Check;
    if (bHasCheck) {
      CFX_RectF rtCheck;
      rtCheck.Set(rtItem.left, rtItem.top, rtItem.height, rtItem.height);
      rtCheck.Deflate(2, 2, 2, 2);
      pData->SetItemCheckRect(m_pInterface, pItem, rtCheck);
      CFWL_ThemeBackground param;
      param.m_pWidget = m_pInterface;
      param.m_iPart = CFWL_Part::Check;
      param.m_pGraphics = pGraphics;
      if (GetItemChecked(pItem)) {
        param.m_dwStates = CFWL_PartState_Checked;
      } else {
        param.m_dwStates = CFWL_PartState_Normal;
      }
      param.m_matrix.Concat(*pMatrix);
      param.m_rtPart = rtCheck;
      param.m_bMaximize = true;
      pTheme->DrawBackground(&param);
    }
    CFX_WideString wsText;
    pData->GetItemText(m_pInterface, pItem, wsText);
    if (wsText.GetLength() <= 0) {
      return;
    }
    CFX_RectF rtText(rtItem);
    rtText.Deflate(kItemTextMargin, kItemTextMargin);
    if (bHasIcon || bHasCheck) {
      rtText.Deflate(rtItem.height, 0, 0, 0);
    }
    CFWL_ThemeText textParam;
    textParam.m_pWidget = m_pInterface;
    textParam.m_iPart = CFWL_Part::ListItem;
    textParam.m_dwStates = dwPartStates;
    textParam.m_pGraphics = pGraphics;
    textParam.m_matrix.Concat(*pMatrix);
    textParam.m_rtPart = rtText;
    textParam.m_wsText = wsText;
    textParam.m_dwTTOStyles = m_dwTTOStyles;
    textParam.m_iTTOAlign = m_iTTOAligns;
    textParam.m_bMaximize = true;
    pTheme->DrawText(&textParam);
  }
}
CFX_SizeF CFWL_ListBoxImp::CalcSize(FX_BOOL bAutoSize) {
  CFX_SizeF fs;
  if (!m_pProperties->m_pThemeProvider)
    return fs;

  GetClientRect(m_rtClient);
  m_rtConent = m_rtClient;
  CFX_RectF rtUIMargin;
  rtUIMargin.Set(0, 0, 0, 0);
  if (!m_pOuter) {
    CFX_RectF* pUIMargin = static_cast<CFX_RectF*>(
        GetThemeCapacity(CFWL_WidgetCapacity::UIMargin));
    if (pUIMargin) {
      m_rtConent.Deflate(pUIMargin->left, pUIMargin->top, pUIMargin->width,
                         pUIMargin->height);
    }
  }
  FX_FLOAT fWidth = 0;
  if (m_pProperties->m_pThemeProvider->IsCustomizedLayout(m_pInterface)) {
    IFWL_ListBoxDP* pData =
        static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
    int32_t iCount = pData->CountItems(m_pInterface);
    for (int32_t i = 0; i < iCount; i++) {
      IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
      CFWL_ThemePart itemPart;
      itemPart.m_pWidget = m_pInterface;
      itemPart.m_iPart = CFWL_Part::ListItem;
      itemPart.m_pData = m_pProperties->m_pDataProvider;
      itemPart.m_bMaximize = i > 0;
      CFX_RectF r;
      m_pProperties->m_pThemeProvider->GetPartRect(&itemPart, r);
      if (!bAutoSize) {
        CFX_RectF rtItem;
        rtItem.Set(m_rtClient.left, m_rtClient.top + fs.y, r.width, r.height);
        IFWL_ListBoxDP* pBox =
            static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
        pBox->SetItemRect(m_pInterface, pItem, rtItem);
      }
      fs.y += r.height;
      if (fs.x < r.width) {
        fs.x = r.width;
        fWidth = r.width;
      }
    }
  } else {
    fWidth = GetMaxTextWidth();
    fWidth += 2 * kItemTextMargin;
    if (!bAutoSize) {
      FX_FLOAT fActualWidth =
          m_rtClient.width - rtUIMargin.left - rtUIMargin.width;
      if (fWidth < fActualWidth) {
        fWidth = fActualWidth;
      }
    }
    IFWL_ListBoxDP* pData =
        static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
    m_fItemHeight = GetItemHeigt();
    FX_BOOL bHasIcon;
    bHasIcon = GetStylesEx() & FWL_STYLEEXT_LTB_Icon;
    if (bHasIcon) {
      fWidth += m_fItemHeight;
    }
    int32_t iCount = pData->CountItems(m_pInterface);
    for (int32_t i = 0; i < iCount; i++) {
      IFWL_ListItem* htem = pData->GetItem(m_pInterface, i);
      GetItemSize(fs, htem, fWidth, m_fItemHeight, bAutoSize);
    }
  }
  if (bAutoSize) {
    return fs;
  }
  FX_FLOAT iWidth = m_rtClient.width - rtUIMargin.left - rtUIMargin.width;
  FX_FLOAT iHeight = m_rtClient.height;
  FX_BOOL bShowVertScr =
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarAlaways) &&
      (m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll);
  FX_BOOL bShowHorzScr =
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarAlaways) &&
      (m_pProperties->m_dwStyles & FWL_WGTSTYLE_HScroll);
  if (!bShowVertScr && m_pProperties->m_dwStyles & FWL_WGTSTYLE_VScroll &&
      (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiColumn) == 0) {
    bShowVertScr = (fs.y > iHeight);
  }
  if (!bShowHorzScr && m_pProperties->m_dwStyles & FWL_WGTSTYLE_HScroll) {
    bShowHorzScr = (fs.x > iWidth);
  }
  CFX_SizeF szRange;
  if (bShowVertScr) {
    if (!m_pVertScrollBar) {
      InitScrollBar();
    }
    CFX_RectF rtScrollBar;
    rtScrollBar.Set(m_rtClient.right() - m_fScorllBarWidth, m_rtClient.top,
                    m_fScorllBarWidth, m_rtClient.height - 1);
    if (bShowHorzScr) {
      rtScrollBar.height -= m_fScorllBarWidth;
    }
    m_pVertScrollBar->SetWidgetRect(rtScrollBar);
    szRange.x = 0, szRange.y = fs.y - m_rtConent.height;
    if (szRange.y < m_fItemHeight) {
      szRange.y = m_fItemHeight;
    }
    m_pVertScrollBar->SetRange(szRange.x, szRange.y);
    m_pVertScrollBar->SetPageSize(rtScrollBar.height * 9 / 10);
    m_pVertScrollBar->SetStepSize(m_fItemHeight);
    FX_FLOAT fPos = m_pVertScrollBar->GetPos();
    if (fPos < 0) {
      fPos = 0;
    }
    if (fPos > szRange.y) {
      fPos = szRange.y;
    }
    m_pVertScrollBar->SetPos(fPos);
    m_pVertScrollBar->SetTrackPos(fPos);
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarFocus) ==
            0 ||
        (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)) {
      m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
    }
    m_pVertScrollBar->Update();
  } else if (m_pVertScrollBar) {
    m_pVertScrollBar->SetPos(0);
    m_pVertScrollBar->SetTrackPos(0);
    m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
  if (bShowHorzScr) {
    if (!m_pHorzScrollBar) {
      InitScrollBar(FALSE);
    }
    CFX_RectF rtScrollBar;
    rtScrollBar.Set(m_rtClient.left, m_rtClient.bottom() - m_fScorllBarWidth,
                    m_rtClient.width, m_fScorllBarWidth);
    if (bShowVertScr) {
      rtScrollBar.width -= m_fScorllBarWidth;
    }
    m_pHorzScrollBar->SetWidgetRect(rtScrollBar);
    szRange.x = 0, szRange.y = fs.x - rtScrollBar.width;
    m_pHorzScrollBar->SetRange(szRange.x, szRange.y);
    m_pHorzScrollBar->SetPageSize(fWidth * 9 / 10);
    m_pHorzScrollBar->SetStepSize(fWidth / 10);
    FX_FLOAT fPos = m_pHorzScrollBar->GetPos();
    if (fPos < 0) {
      fPos = 0;
    }
    if (fPos > szRange.y) {
      fPos = szRange.y;
    }
    m_pHorzScrollBar->SetPos(fPos);
    m_pHorzScrollBar->SetTrackPos(fPos);
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_ShowScrollBarFocus) ==
            0 ||
        (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused)) {
      m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, FALSE);
    }
    m_pHorzScrollBar->Update();
  } else if (m_pHorzScrollBar) {
    m_pHorzScrollBar->SetPos(0);
    m_pHorzScrollBar->SetTrackPos(0);
    m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, TRUE);
  }
  if (bShowVertScr && bShowHorzScr) {
    m_rtStatic.Set(m_rtClient.right() - m_fScorllBarWidth,
                   m_rtClient.bottom() - m_fScorllBarWidth, m_fScorllBarWidth,
                   m_fScorllBarWidth);
  }
  return fs;
}
void CFWL_ListBoxImp::GetItemSize(CFX_SizeF& size,
                                  IFWL_ListItem* pItem,
                                  FX_FLOAT fWidth,
                                  FX_FLOAT fItemHeight,
                                  FX_BOOL bAutoSize) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_MultiColumn) {
  } else {
    if (!bAutoSize) {
      CFX_RectF rtItem;
      rtItem.Set(0, size.y, fWidth, fItemHeight);
      IFWL_ListBoxDP* pData =
          static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
      pData->SetItemRect(m_pInterface, pItem, rtItem);
    }
    size.x = fWidth;
    size.y += fItemHeight;
  }
}
FX_FLOAT CFWL_ListBoxImp::GetMaxTextWidth() {
  FX_FLOAT fRet = 0.0f;
  IFWL_ListBoxDP* pData =
      static_cast<IFWL_ListBoxDP*>(m_pProperties->m_pDataProvider);
  int32_t iCount = pData->CountItems(m_pInterface);
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* pItem = pData->GetItem(m_pInterface, i);
    if (!pItem) {
      continue;
    }
    CFX_WideString wsText;
    pData->GetItemText(m_pInterface, pItem, wsText);
    CFX_SizeF sz = CalcTextSize(wsText, m_pProperties->m_pThemeProvider);
    if (sz.x > fRet) {
      fRet = sz.x;
    }
  }
  return fRet;
}
FX_FLOAT CFWL_ListBoxImp::GetScrollWidth() {
  FX_FLOAT* pfWidth = static_cast<FX_FLOAT*>(
      GetThemeCapacity(CFWL_WidgetCapacity::ScrollBarWidth));
  if (!pfWidth)
    return 0;
  return *pfWidth;
}
FX_FLOAT CFWL_ListBoxImp::GetItemHeigt() {
  FX_FLOAT* pfFont =
      static_cast<FX_FLOAT*>(GetThemeCapacity(CFWL_WidgetCapacity::FontSize));
  if (!pfFont)
    return 20;
  return *pfFont + 2 * kItemTextMargin;
}
void CFWL_ListBoxImp::InitScrollBar(FX_BOOL bVert) {
  if ((bVert && m_pVertScrollBar) || (!bVert && m_pHorzScrollBar)) {
    return;
  }
  CFWL_WidgetImpProperties prop;
  prop.m_dwStyleExes = bVert ? FWL_STYLEEXT_SCB_Vert : FWL_STYLEEXT_SCB_Horz;
  prop.m_dwStates = FWL_WGTSTATE_Invisible;
  prop.m_pParent = m_pInterface;
  prop.m_pThemeProvider = m_pScrollBarTP;
  IFWL_ScrollBar* pScrollBar = IFWL_ScrollBar::Create(prop, m_pInterface);
  pScrollBar->Initialize();
  (bVert ? &m_pVertScrollBar : &m_pHorzScrollBar)->reset(pScrollBar);
}

FX_BOOL CFWL_ListBoxImp::IsShowScrollBar(FX_BOOL bVert) {
  IFWL_ScrollBar* pScrollbar =
      bVert ? m_pVertScrollBar.get() : m_pHorzScrollBar.get();
  if (!pScrollbar || (pScrollbar->GetStates() & FWL_WGTSTATE_Invisible)) {
    return FALSE;
  }
  return !(m_pProperties->m_dwStyleExes &
           FWL_STYLEEXT_LTB_ShowScrollBarFocus) ||
         (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused);
}
void CFWL_ListBoxImp::ProcessSelChanged() {
  CFWL_EvtLtbSelChanged selEvent;
  selEvent.m_pSrcTarget = m_pInterface;
  CFX_Int32Array arrSels;
  int32_t iCount = CountSelItems();
  for (int32_t i = 0; i < iCount; i++) {
    IFWL_ListItem* item = GetSelItem(i);
    if (!item) {
      continue;
    }
    selEvent.iarraySels.Add(i);
  }
  DispatchEvent(&selEvent);
}

CFWL_ListBoxImpDelegate::CFWL_ListBoxImpDelegate(CFWL_ListBoxImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_ListBoxImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;
  if (!m_pOwner->IsEnabled())
    return;

  CFWL_MessageType dwMsgCode = pMessage->GetClassID();
  switch (dwMsgCode) {
    case CFWL_MessageType::SetFocus: {
      OnFocusChanged(pMessage, TRUE);
      break;
    }
    case CFWL_MessageType::KillFocus: {
      OnFocusChanged(pMessage, FALSE);
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
        default:
          break;
      }
      break;
    }
    case CFWL_MessageType::MouseWheel: {
      OnMouseWheel(static_cast<CFWL_MsgMouseWheel*>(pMessage));
      break;
    }
    case CFWL_MessageType::Key: {
      CFWL_MsgKey* pMsg = static_cast<CFWL_MsgKey*>(pMessage);
      if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown)
        OnKeyDown(pMsg);
      break;
    }
    default: {
      break;
    }
  }
  CFWL_WidgetImpDelegate::OnProcessMessage(pMessage);
}

void CFWL_ListBoxImpDelegate::OnProcessEvent(CFWL_Event* pEvent) {
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

void CFWL_ListBoxImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                           const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}

void CFWL_ListBoxImpDelegate::OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet) {
  if (m_pOwner->GetStylesEx() & FWL_STYLEEXT_LTB_ShowScrollBarFocus) {
    if (m_pOwner->m_pVertScrollBar) {
      m_pOwner->m_pVertScrollBar->SetStates(FWL_WGTSTATE_Invisible, !bSet);
    }
    if (m_pOwner->m_pHorzScrollBar) {
      m_pOwner->m_pHorzScrollBar->SetStates(FWL_WGTSTATE_Invisible, !bSet);
    }
  }
  if (bSet) {
    m_pOwner->m_pProperties->m_dwStates |= (FWL_WGTSTATE_Focused);
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~(FWL_WGTSTATE_Focused);
  }
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_ListBoxImpDelegate::OnLButtonDown(CFWL_MsgMouse* pMsg) {
  m_pOwner->m_bLButtonDown = TRUE;
  if ((m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0) {
    m_pOwner->SetFocus(TRUE);
  }
  IFWL_ListItem* pItem = m_pOwner->GetItemAtPoint(pMsg->m_fx, pMsg->m_fy);
  if (!pItem) {
    return;
  }
  if (m_pOwner->m_pProperties->m_dwStyleExes &
      FWL_STYLEEXT_LTB_MultiSelection) {
    if (pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl) {
      FX_BOOL bSelected = m_pOwner->IsItemSelected(pItem);
      m_pOwner->SetSelectionDirect(pItem, !bSelected);
      m_pOwner->m_hAnchor = pItem;
    } else if (pMsg->m_dwFlags & FWL_KEYFLAG_Shift) {
      if (m_pOwner->m_hAnchor) {
        m_pOwner->SetSelection(m_pOwner->m_hAnchor, pItem, TRUE);
      } else {
        m_pOwner->SetSelectionDirect(pItem, TRUE);
      }
    } else {
      m_pOwner->SetSelection(pItem, pItem, TRUE);
      m_pOwner->m_hAnchor = pItem;
    }
  } else {
    m_pOwner->SetSelection(pItem, pItem, TRUE);
  }
  if (m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_LTB_Check) {
    IFWL_ListItem* hSelectedItem =
        m_pOwner->GetItemAtPoint(pMsg->m_fx, pMsg->m_fy);
    CFX_RectF rtCheck;
    m_pOwner->GetItemCheckRect(hSelectedItem, rtCheck);
    FX_BOOL bChecked = m_pOwner->GetItemChecked(pItem);
    if (rtCheck.Contains(pMsg->m_fx, pMsg->m_fy)) {
      if (bChecked) {
        m_pOwner->SetItemChecked(pItem, FALSE);
      } else {
        m_pOwner->SetItemChecked(pItem, TRUE);
      }
      m_pOwner->Update();
    }
  }
  m_pOwner->SetFocusItem(pItem);
  m_pOwner->ScrollToVisible(pItem);
  m_pOwner->SetGrab(TRUE);
  m_pOwner->ProcessSelChanged();
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_ListBoxImpDelegate::OnLButtonUp(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_bLButtonDown) {
    m_pOwner->m_bLButtonDown = FALSE;
    m_pOwner->SetGrab(FALSE);
    DispatchSelChangedEv();
  }
}
void CFWL_ListBoxImpDelegate::OnMouseWheel(CFWL_MsgMouseWheel* pMsg) {
  if (!m_pOwner->IsShowScrollBar(TRUE)) {
    return;
  }
  IFWL_WidgetDelegate* pDelegate =
      m_pOwner->m_pVertScrollBar->SetDelegate(nullptr);
  pDelegate->OnProcessMessage(pMsg);
}
void CFWL_ListBoxImpDelegate::OnKeyDown(CFWL_MsgKey* pMsg) {
  uint32_t dwKeyCode = pMsg->m_dwKeyCode;
  switch (dwKeyCode) {
    case FWL_VKEY_Tab:
    case FWL_VKEY_Up:
    case FWL_VKEY_Down:
    case FWL_VKEY_Home:
    case FWL_VKEY_End: {
      IFWL_ListItem* pItem = m_pOwner->GetFocusedItem();
      pItem = m_pOwner->GetItem(pItem, dwKeyCode);
      FX_BOOL bShift = pMsg->m_dwFlags & FWL_KEYFLAG_Shift;
      FX_BOOL bCtrl = pMsg->m_dwFlags & FWL_KEYFLAG_Ctrl;
      OnVK(pItem, bShift, bCtrl);
      DispatchSelChangedEv();
      m_pOwner->ProcessSelChanged();
      break;
    }
    default: {}
  }
}
void CFWL_ListBoxImpDelegate::OnVK(IFWL_ListItem* pItem,
                                   FX_BOOL bShift,
                                   FX_BOOL bCtrl) {
  if (!pItem) {
    return;
  }
  if (m_pOwner->m_pProperties->m_dwStyleExes &
      FWL_STYLEEXT_LTB_MultiSelection) {
    if (bCtrl) {
    } else if (bShift) {
      if (m_pOwner->m_hAnchor) {
        m_pOwner->SetSelection(m_pOwner->m_hAnchor, pItem, TRUE);
      } else {
        m_pOwner->SetSelectionDirect(pItem, TRUE);
      }
    } else {
      m_pOwner->SetSelection(pItem, pItem, TRUE);
      m_pOwner->m_hAnchor = pItem;
    }
  } else {
    m_pOwner->SetSelection(pItem, pItem, TRUE);
  }
  m_pOwner->SetFocusItem(pItem);
  m_pOwner->ScrollToVisible(pItem);
  {
    CFX_RectF rtInvalidate;
    rtInvalidate.Set(0, 0, m_pOwner->m_pProperties->m_rtWidget.width,
                     m_pOwner->m_pProperties->m_rtWidget.height);
    m_pOwner->Repaint(&rtInvalidate);
  }
}
FX_BOOL CFWL_ListBoxImpDelegate::OnScroll(IFWL_ScrollBar* pScrollBar,
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
    case FWL_SCBCODE_TrackPos:
      break;
    case FWL_SCBCODE_EndScroll:
      return FALSE;
  }
  if (iCurPos != fPos) {
    pScrollBar->SetPos(fPos);
    pScrollBar->SetTrackPos(fPos);
    m_pOwner->Repaint(&m_pOwner->m_rtClient);
  }
  return TRUE;
}
void CFWL_ListBoxImpDelegate::DispatchSelChangedEv() {
  CFWL_EvtLtbSelChanged ev;
  ev.m_pSrcTarget = m_pOwner->m_pInterface;
  m_pOwner->DispatchEvent(&ev);
}
