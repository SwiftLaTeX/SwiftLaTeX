// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffchoicelist.h"

#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/lightwidget/cfwl_combobox.h"
#include "xfa/fwl/lightwidget/cfwl_listbox.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

CXFA_FFListBox::CXFA_FFListBox(CXFA_FFPageView* pPageView,
                               CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc), m_pOldDelegate(nullptr) {}
CXFA_FFListBox::~CXFA_FFListBox() {
  if (m_pNormalWidget) {
    IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
    CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
    pNoteDriver->UnregisterEventTarget(pWidget);
  }
}
FX_BOOL CXFA_FFListBox::LoadWidget() {
  CFWL_ListBox* pListBox = CFWL_ListBox::Create();
  pListBox->Initialize();
  pListBox->ModifyStyles(FWL_WGTSTYLE_VScroll | FWL_WGTSTYLE_NoBackground,
                         0xFFFFFFFF);
  m_pNormalWidget = (CFWL_Widget*)pListBox;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideStringArray wsLabelArray;
  m_pDataAcc->GetChoiceListItems(wsLabelArray, FALSE);
  int32_t iItems = wsLabelArray.GetSize();
  for (int32_t i = 0; i < iItems; i++) {
    pListBox->AddString(wsLabelArray[i].AsStringC());
  }
  uint32_t dwExtendedStyle = FWL_STYLEEXT_LTB_ShowScrollBarFocus;
  if (m_pDataAcc->GetChoiceListOpen() == XFA_ATTRIBUTEENUM_MultiSelect) {
    dwExtendedStyle |= FWL_STYLEEXT_LTB_MultiSelection;
  }
  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
  CFX_Int32Array iSelArray;
  m_pDataAcc->GetSelectedItems(iSelArray);
  int32_t iSelCount = iSelArray.GetSize();
  for (int32_t j = 0; j < iSelCount; j++) {
    IFWL_ListItem* item = pListBox->GetItem(iSelArray[j]);
    pListBox->SetSelItem(item, TRUE);
  }
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}

FX_BOOL CXFA_FFListBox::OnKillFocus(CXFA_FFWidget* pNewFocus) {
  if (!ProcessCommittedData())
    UpdateFWLData();
  CXFA_FFField::OnKillFocus(pNewFocus);
  return TRUE;
}

FX_BOOL CXFA_FFListBox::CommitData() {
  CFWL_ListBox* pListBox = static_cast<CFWL_ListBox*>(m_pNormalWidget);
  int32_t iSels = pListBox->CountSelItems();
  CFX_Int32Array iSelArray;
  for (int32_t i = 0; i < iSels; ++i)
    iSelArray.Add(pListBox->GetSelIndex(i));
  m_pDataAcc->SetSelectedItems(iSelArray, true, FALSE, TRUE);
  return TRUE;
}

FX_BOOL CXFA_FFListBox::IsDataChanged() {
  CFX_Int32Array iSelArray;
  m_pDataAcc->GetSelectedItems(iSelArray);
  int32_t iOldSels = iSelArray.GetSize();
  CFWL_ListBox* pListBox = (CFWL_ListBox*)m_pNormalWidget;
  int32_t iSels = pListBox->CountSelItems();
  if (iOldSels != iSels)
    return TRUE;

  for (int32_t i = 0; i < iSels; ++i) {
    IFWL_ListItem* hlistItem = pListBox->GetItem(iSelArray[i]);
    if (!(pListBox->GetItemStates(hlistItem) & FWL_ITEMSTATE_LTB_Selected))
      return TRUE;
  }
  return FALSE;
}

uint32_t CXFA_FFListBox::GetAlignment() {
  uint32_t dwExtendedStyle = 0;
  if (CXFA_Para para = m_pDataAcc->GetPara()) {
    int32_t iHorz = para.GetHorizontalAlign();
    switch (iHorz) {
      case XFA_ATTRIBUTEENUM_Center:
        dwExtendedStyle |= FWL_STYLEEXT_LTB_CenterAlign;
        break;
      case XFA_ATTRIBUTEENUM_Justify:
        break;
      case XFA_ATTRIBUTEENUM_JustifyAll:
        break;
      case XFA_ATTRIBUTEENUM_Radix:
        break;
      case XFA_ATTRIBUTEENUM_Right:
        dwExtendedStyle |= FWL_STYLEEXT_LTB_RightAlign;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_LTB_LeftAlign;
        break;
    }
  }
  return dwExtendedStyle;
}
FX_BOOL CXFA_FFListBox::UpdateFWLData() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_ListBox* pListBox = ((CFWL_ListBox*)m_pNormalWidget);
  CFX_ArrayTemplate<IFWL_ListItem*> selItemArray;
  CFX_Int32Array iSelArray;
  m_pDataAcc->GetSelectedItems(iSelArray);
  int32_t iSelCount = iSelArray.GetSize();
  for (int32_t j = 0; j < iSelCount; j++) {
    IFWL_ListItem* lpItemSel = pListBox->GetSelItem(iSelArray[j]);
    selItemArray.Add(lpItemSel);
  }
  pListBox->SetSelItem(pListBox->GetSelItem(-1), FALSE);
  for (int32_t i = 0; i < iSelCount; i++) {
    ((CFWL_ListBox*)m_pNormalWidget)->SetSelItem(selItemArray[i], TRUE);
  }
  m_pNormalWidget->Update();
  return TRUE;
}
void CXFA_FFListBox::OnSelectChanged(IFWL_Widget* pWidget,
                                     const CFX_Int32Array& arrSels) {
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Change;
  eParam.m_pTarget = m_pDataAcc;
  m_pDataAcc->GetValue(eParam.m_wsPrevText, XFA_VALUEPICTURE_Raw);
  CFWL_ListBox* pListBox = (CFWL_ListBox*)m_pNormalWidget;
  int32_t iSels = pListBox->CountSelItems();
  if (iSels > 0) {
    pListBox->GetItemText(pListBox->GetSelItem(0), eParam.m_wsNewText);
  }
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Change, &eParam);
}
void CXFA_FFListBox::SetItemState(int32_t nIndex, FX_BOOL bSelected) {
  IFWL_ListItem* item = ((CFWL_ListBox*)m_pNormalWidget)->GetSelItem(nIndex);
  ((CFWL_ListBox*)m_pNormalWidget)->SetSelItem(item, bSelected);
  m_pNormalWidget->Update();
  AddInvalidateRect();
}
void CXFA_FFListBox::InsertItem(const CFX_WideStringC& wsLabel,
                                int32_t nIndex) {
  CFX_WideString wsTemp(wsLabel);
  ((CFWL_ListBox*)m_pNormalWidget)->AddString(wsTemp.AsStringC());
  m_pNormalWidget->Update();
  AddInvalidateRect();
}
void CXFA_FFListBox::DeleteItem(int32_t nIndex) {
  if (nIndex < 0) {
    ((CFWL_ListBox*)m_pNormalWidget)->DeleteAll();
  } else {
    ((CFWL_ListBox*)m_pNormalWidget)
        ->DeleteString(((CFWL_ListBox*)m_pNormalWidget)->GetItem(nIndex));
  }
  m_pNormalWidget->Update();
  AddInvalidateRect();
}

void CXFA_FFListBox::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFListBox::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  switch (pEvent->GetClassID()) {
    case CFWL_EventType::SelectChanged: {
      CFX_Int32Array arrSels;
      OnSelectChanged(m_pNormalWidget->GetWidget(), arrSels);
      break;
    }
    default:
      break;
  }
  m_pOldDelegate->OnProcessEvent(pEvent);
}
void CXFA_FFListBox::OnDrawWidget(CFX_Graphics* pGraphics,
                                  const CFX_Matrix* pMatrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, pMatrix);
}

CXFA_FFComboBox::CXFA_FFComboBox(CXFA_FFPageView* pPageView,
                                 CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc), m_pOldDelegate(nullptr) {}

CXFA_FFComboBox::~CXFA_FFComboBox() {}

FX_BOOL CXFA_FFComboBox::GetBBox(CFX_RectF& rtBox,
                                 uint32_t dwStatus,
                                 FX_BOOL bDrawFocus) {
  if (bDrawFocus)
    return FALSE;
  return CXFA_FFWidget::GetBBox(rtBox, dwStatus);
}

FX_BOOL CXFA_FFComboBox::PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFX_RectF rtWidget;
  ((CFWL_ComboBox*)m_pNormalWidget)->GetBBox(rtWidget);
  if (rtWidget.Contains(fx, fy)) {
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_FFComboBox::LoadWidget() {
  CFWL_ComboBox* pComboBox = CFWL_ComboBox::Create();
  pComboBox->Initialize();
  m_pNormalWidget = (CFWL_Widget*)pComboBox;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideStringArray wsLabelArray;
  m_pDataAcc->GetChoiceListItems(wsLabelArray, FALSE);
  int32_t iItems = wsLabelArray.GetSize();
  for (int32_t i = 0; i < iItems; i++) {
    pComboBox->AddString(wsLabelArray[i].AsStringC());
  }
  CFX_Int32Array iSelArray;
  m_pDataAcc->GetSelectedItems(iSelArray);
  int32_t iSelCount = iSelArray.GetSize();
  if (iSelCount > 0) {
    pComboBox->SetCurSel(iSelArray[0]);
  } else {
    CFX_WideString wsText;
    m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Raw);
    pComboBox->SetEditText(wsText);
  }
  UpdateWidgetProperty();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFComboBox::UpdateWidgetProperty() {
  CFWL_ComboBox* pComboBox = (CFWL_ComboBox*)m_pNormalWidget;
  if (!pComboBox) {
    return;
  }
  uint32_t dwExtendedStyle = 0;
  uint32_t dwEditStyles =
      FWL_STYLEEXT_EDT_ReadOnly | FWL_STYLEEXT_EDT_LastLineHeight;
  dwExtendedStyle |= UpdateUIProperty();
  if (m_pDataAcc->IsChoiceListAllowTextEntry()) {
    dwEditStyles &= ~FWL_STYLEEXT_EDT_ReadOnly;
    dwExtendedStyle |= FWL_STYLEEXT_CMB_DropDown;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    dwEditStyles |= FWL_STYLEEXT_EDT_ReadOnly;
    dwExtendedStyle |= FWL_STYLEEXT_CMB_ReadOnly;
  }
  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
  if (m_pDataAcc->GetHorizontalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
    dwEditStyles |= FWL_STYLEEXT_EDT_AutoHScroll;
  }
  pComboBox->EditModifyStylesEx(dwEditStyles, 0xFFFFFFFF);
}
FX_BOOL CXFA_FFComboBox::OnRButtonUp(uint32_t dwFlags,
                                     FX_FLOAT fx,
                                     FX_FLOAT fy) {
  if (!CXFA_FFField::OnRButtonUp(dwFlags, fx, fy))
    return FALSE;

  GetDoc()->GetDocProvider()->PopupMenu(this, CFX_PointF(fx, fy), nullptr);
  return TRUE;
}
FX_BOOL CXFA_FFComboBox::OnKillFocus(CXFA_FFWidget* pNewWidget) {
  FX_BOOL flag = ProcessCommittedData();
  if (!flag) {
    UpdateFWLData();
  }
  CXFA_FFField::OnKillFocus(pNewWidget);
  return TRUE;
}
void CXFA_FFComboBox::OpenDropDownList() {
  ((CFWL_ComboBox*)m_pNormalWidget)->OpenDropDownList(TRUE);
}
FX_BOOL CXFA_FFComboBox::CommitData() {
  return m_pDataAcc->SetValue(m_wsNewValue, XFA_VALUEPICTURE_Raw);
}
FX_BOOL CXFA_FFComboBox::IsDataChanged() {
  CFWL_ComboBox* pFWLcombobox = ((CFWL_ComboBox*)m_pNormalWidget);
  CFX_WideString wsText;
  pFWLcombobox->GetEditText(wsText);
  int32_t iCursel = pFWLcombobox->GetCurSel();
  if (iCursel >= 0) {
    CFX_WideString wsSel;
    pFWLcombobox->GetTextByIndex(iCursel, wsSel);
    if (wsSel == wsText) {
      m_pDataAcc->GetChoiceListItem(wsText, iCursel, TRUE);
    }
  }
  CFX_WideString wsOldValue;
  m_pDataAcc->GetValue(wsOldValue, XFA_VALUEPICTURE_Raw);
  if (wsOldValue != wsText) {
    m_wsNewValue = wsText;
    return TRUE;
  }
  return FALSE;
}
void CXFA_FFComboBox::FWLEventSelChange(CXFA_EventParam* pParam) {
  pParam->m_eType = XFA_EVENT_Change;
  pParam->m_pTarget = m_pDataAcc;
  CFWL_ComboBox* pFWLcombobox = ((CFWL_ComboBox*)m_pNormalWidget);
  pFWLcombobox->GetEditText(pParam->m_wsNewText);
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Change, pParam);
}
uint32_t CXFA_FFComboBox::GetAlignment() {
  uint32_t dwExtendedStyle = 0;
  if (CXFA_Para para = m_pDataAcc->GetPara()) {
    int32_t iHorz = para.GetHorizontalAlign();
    switch (iHorz) {
      case XFA_ATTRIBUTEENUM_Center:
        dwExtendedStyle |=
            FWL_STYLEEXT_CMB_EditHCenter | FWL_STYLEEXT_CMB_ListItemCenterAlign;
        break;
      case XFA_ATTRIBUTEENUM_Justify:
        dwExtendedStyle |= FWL_STYLEEXT_CMB_EditJustified;
        break;
      case XFA_ATTRIBUTEENUM_JustifyAll:
        break;
      case XFA_ATTRIBUTEENUM_Radix:
        break;
      case XFA_ATTRIBUTEENUM_Right:
        break;
      default:
        dwExtendedStyle |=
            FWL_STYLEEXT_CMB_EditHNear | FWL_STYLEEXT_CMB_ListItemLeftAlign;
        break;
    }
    int32_t iVert = para.GetVerticalAlign();
    switch (iVert) {
      case XFA_ATTRIBUTEENUM_Middle:
        dwExtendedStyle |= FWL_STYLEEXT_CMB_EditVCenter;
        break;
      case XFA_ATTRIBUTEENUM_Bottom:
        dwExtendedStyle |= FWL_STYLEEXT_CMB_EditVFar;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_CMB_EditVNear;
        break;
    }
  }
  return dwExtendedStyle;
}
FX_BOOL CXFA_FFComboBox::UpdateFWLData() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFX_Int32Array iSelArray;
  m_pDataAcc->GetSelectedItems(iSelArray);
  int32_t iSelCount = iSelArray.GetSize();
  if (iSelCount > 0) {
    ((CFWL_ComboBox*)m_pNormalWidget)->SetCurSel(iSelArray[0]);
  } else {
    CFX_WideString wsText;
    ((CFWL_ComboBox*)m_pNormalWidget)->SetCurSel(-1);
    m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Raw);
    ((CFWL_ComboBox*)m_pNormalWidget)->SetEditText(wsText);
  }
  m_pNormalWidget->Update();
  return TRUE;
}
FX_BOOL CXFA_FFComboBox::CanUndo() {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditCanUndo();
}
FX_BOOL CXFA_FFComboBox::CanRedo() {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditCanRedo();
}
FX_BOOL CXFA_FFComboBox::Undo() {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditUndo();
}
FX_BOOL CXFA_FFComboBox::Redo() {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditRedo();
}
FX_BOOL CXFA_FFComboBox::CanCopy() {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditCanCopy();
}
FX_BOOL CXFA_FFComboBox::CanCut() {
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditCanCut();
}
FX_BOOL CXFA_FFComboBox::CanPaste() {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         (m_pDataAcc->GetAccess() == XFA_ATTRIBUTEENUM_Open);
}
FX_BOOL CXFA_FFComboBox::CanSelectAll() {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditCanSelectAll();
}
FX_BOOL CXFA_FFComboBox::Copy(CFX_WideString& wsCopy) {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditCopy(wsCopy);
}
FX_BOOL CXFA_FFComboBox::Cut(CFX_WideString& wsCut) {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditCut(wsCut);
}
FX_BOOL CXFA_FFComboBox::Paste(const CFX_WideString& wsPaste) {
  return m_pDataAcc->IsChoiceListAllowTextEntry() &&
         ((CFWL_ComboBox*)m_pNormalWidget)->EditPaste(wsPaste);
}
FX_BOOL CXFA_FFComboBox::SelectAll() {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditSelectAll();
}
FX_BOOL CXFA_FFComboBox::Delete() {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditDelete();
}
FX_BOOL CXFA_FFComboBox::DeSelect() {
  return ((CFWL_ComboBox*)m_pNormalWidget)->EditDeSelect();
}
void CXFA_FFComboBox::SetItemState(int32_t nIndex, FX_BOOL bSelected) {
  if (bSelected) {
    ((CFWL_ComboBox*)m_pNormalWidget)->SetCurSel(nIndex);
  } else {
    ((CFWL_ComboBox*)m_pNormalWidget)->SetCurSel(-1);
  }
  m_pNormalWidget->Update();
  AddInvalidateRect();
}
void CXFA_FFComboBox::InsertItem(const CFX_WideStringC& wsLabel,
                                 int32_t nIndex) {
  ((CFWL_ComboBox*)m_pNormalWidget)->AddString(wsLabel);
  m_pNormalWidget->Update();
  AddInvalidateRect();
}
void CXFA_FFComboBox::DeleteItem(int32_t nIndex) {
  if (nIndex < 0) {
    ((CFWL_ComboBox*)m_pNormalWidget)->RemoveAll();
  } else {
    ((CFWL_ComboBox*)m_pNormalWidget)->RemoveAt(nIndex);
  }
  m_pNormalWidget->Update();
  AddInvalidateRect();
}
void CXFA_FFComboBox::OnTextChanged(IFWL_Widget* pWidget,
                                    const CFX_WideString& wsChanged) {
  CXFA_EventParam eParam;
  m_pDataAcc->GetValue(eParam.m_wsPrevText, XFA_VALUEPICTURE_Raw);
  eParam.m_wsChange = wsChanged;
  FWLEventSelChange(&eParam);
}
void CXFA_FFComboBox::OnSelectChanged(IFWL_Widget* pWidget,
                                      const CFX_Int32Array& arrSels,
                                      FX_BOOL bLButtonUp) {
  CXFA_EventParam eParam;
  m_pDataAcc->GetValue(eParam.m_wsPrevText, XFA_VALUEPICTURE_Raw);
  FWLEventSelChange(&eParam);
  if (m_pDataAcc->GetChoiceListCommitOn() == XFA_ATTRIBUTEENUM_Select &&
      bLButtonUp) {
    m_pDocView->SetFocusWidgetAcc(nullptr);
  }
}
void CXFA_FFComboBox::OnPreOpen(IFWL_Widget* pWidget) {
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_PreOpen;
  eParam.m_pTarget = m_pDataAcc;
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_PreOpen, &eParam);
}
void CXFA_FFComboBox::OnPostOpen(IFWL_Widget* pWidget) {
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_PostOpen;
  eParam.m_pTarget = m_pDataAcc;
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_PostOpen, &eParam);
}

void CXFA_FFComboBox::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFComboBox::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  switch (pEvent->GetClassID()) {
    case CFWL_EventType::SelectChanged: {
      CFWL_EvtCmbSelChanged* postEvent = (CFWL_EvtCmbSelChanged*)pEvent;
      OnSelectChanged(m_pNormalWidget->GetWidget(), postEvent->iArraySels,
                      postEvent->bLButtonUp);
      break;
    }
    case CFWL_EventType::EditChanged: {
      CFX_WideString wsChanged;
      OnTextChanged(m_pNormalWidget->GetWidget(), wsChanged);
      break;
    }
    case CFWL_EventType::PreDropDown: {
      OnPreOpen(m_pNormalWidget->GetWidget());
      break;
    }
    case CFWL_EventType::PostDropDown: {
      OnPostOpen(m_pNormalWidget->GetWidget());
      break;
    }
    default:
      break;
  }
  m_pOldDelegate->OnProcessEvent(pEvent);
}

void CXFA_FFComboBox::OnDrawWidget(CFX_Graphics* pGraphics,
                                   const CFX_Matrix* pMatrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, pMatrix);
}
