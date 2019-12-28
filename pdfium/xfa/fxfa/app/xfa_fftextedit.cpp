// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_fftextedit.h"

#include <vector>

#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/lightwidget/cfwl_datetimepicker.h"
#include "xfa/fwl/lightwidget/cfwl_edit.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/app/xfa_textlayout.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/parser/xfa_localevalue.h"

CXFA_FFTextEdit::CXFA_FFTextEdit(CXFA_FFPageView* pPageView,
                                 CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc), m_pOldDelegate(nullptr) {}
CXFA_FFTextEdit::~CXFA_FFTextEdit() {
  if (m_pNormalWidget) {
    IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
    CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
    pNoteDriver->UnregisterEventTarget(pWidget);
  }
}
FX_BOOL CXFA_FFTextEdit::LoadWidget() {
  CFWL_Edit* pFWLEdit = CFWL_Edit::Create();
  pFWLEdit->Initialize();
  m_pNormalWidget = pFWLEdit;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  UpdateWidgetProperty();
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Display);
  pFWLEdit->SetText(wsText);
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFTextEdit::UpdateWidgetProperty() {
  CFWL_Edit* pWidget = (CFWL_Edit*)m_pNormalWidget;
  if (!pWidget) {
    return;
  }
  uint32_t dwStyle = 0;
  uint32_t dwExtendedStyle = FWL_STYLEEXT_EDT_ShowScrollbarFocus |
                             FWL_STYLEEXT_EDT_OuterScrollbar |
                             FWL_STYLEEXT_EDT_LastLineHeight;
  dwExtendedStyle |= UpdateUIProperty();
  if (m_pDataAcc->IsMultiLine()) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_MultiLine | FWL_STYLEEXT_EDT_WantReturn;
    if (m_pDataAcc->GetVerticalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
      dwStyle |= FWL_WGTSTYLE_VScroll;
      dwExtendedStyle |= FWL_STYLEEXT_EDT_AutoVScroll;
    }
  } else if (m_pDataAcc->GetHorizontalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_AutoHScroll;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_ReadOnly;
    dwExtendedStyle |= FWL_STYLEEXT_EDT_MultiLine;
  }
  XFA_Element eType = XFA_Element::Unknown;
  int32_t iMaxChars = m_pDataAcc->GetMaxChars(eType);
  if (eType == XFA_Element::ExData) {
    iMaxChars = 0;
  }
  int32_t iNumCells = m_pDataAcc->GetNumberOfCells();
  if (iNumCells == 0) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_CombText;
    pWidget->SetLimit(iMaxChars > 0 ? iMaxChars : 1);
  } else if (iNumCells > 0) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_CombText;
    pWidget->SetLimit(iNumCells);
  } else {
    pWidget->SetLimit(iMaxChars);
  }
  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStyles(dwStyle, 0xFFFFFFFF);
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
}
FX_BOOL CXFA_FFTextEdit::OnLButtonDown(uint32_t dwFlags,
                                       FX_FLOAT fx,
                                       FX_FLOAT fy) {
  if (!PtInActiveRect(fx, fy)) {
    return FALSE;
  }
  if (!IsFocused()) {
    m_dwStatus |= XFA_WidgetStatus_Focused;
    UpdateFWLData();
    AddInvalidateRect();
  }
  SetButtonDown(TRUE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  FWLToClient(ms.m_fx, ms.m_fy);
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::OnRButtonDown(uint32_t dwFlags,
                                       FX_FLOAT fx,
                                       FX_FLOAT fy) {
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  if (!PtInActiveRect(fx, fy)) {
    return FALSE;
  }
  if (!IsFocused()) {
    m_dwStatus |= XFA_WidgetStatus_Focused;
    UpdateFWLData();
    AddInvalidateRect();
  }
  SetButtonDown(TRUE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::RightButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::OnRButtonUp(uint32_t dwFlags,
                                     FX_FLOAT fx,
                                     FX_FLOAT fy) {
  if (!CXFA_FFField::OnRButtonUp(dwFlags, fx, fy))
    return FALSE;

  GetDoc()->GetDocProvider()->PopupMenu(this, CFX_PointF(fx, fy), nullptr);
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::OnSetFocus(CXFA_FFWidget* pOldWidget) {
  m_dwStatus &= ~XFA_WidgetStatus_TextEditValueChanged;
  if (!IsFocused()) {
    m_dwStatus |= XFA_WidgetStatus_Focused;
    UpdateFWLData();
    AddInvalidateRect();
  }
  CXFA_FFWidget::OnSetFocus(pOldWidget);
  CFWL_MsgSetFocus ms;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::OnKillFocus(CXFA_FFWidget* pNewWidget) {
  CFWL_MsgKillFocus ms;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  m_dwStatus &= ~XFA_WidgetStatus_Focused;
  SetEditScrollOffset();
  ProcessCommittedData();
  UpdateFWLData();
  AddInvalidateRect();
  CXFA_FFWidget::OnKillFocus(pNewWidget);
  m_dwStatus &= ~XFA_WidgetStatus_TextEditValueChanged;
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::CommitData() {
  CFX_WideString wsText;
  ((CFWL_Edit*)m_pNormalWidget)->GetText(wsText);
  if (m_pDataAcc->SetValue(wsText, XFA_VALUEPICTURE_Edit)) {
    m_pDataAcc->UpdateUIDisplay(this);
    return TRUE;
  }
  ValidateNumberField(wsText);
  return FALSE;
}
void CXFA_FFTextEdit::ValidateNumberField(const CFX_WideString& wsText) {
  CXFA_WidgetAcc* pAcc = GetDataAcc();
  if (pAcc && pAcc->GetUIType() == XFA_Element::NumericEdit) {
    IXFA_AppProvider* pAppProvider = GetApp()->GetAppProvider();
    if (pAppProvider) {
      CFX_WideString wsTitle;
      pAppProvider->LoadString(XFA_IDS_AppName, wsTitle);
      CFX_WideString wsError;
      pAppProvider->LoadString(XFA_IDS_ValidateNumberError, wsError);
      CFX_WideString wsSomField;
      pAcc->GetNode()->GetSOMExpression(wsSomField);
      CFX_WideString wsMessage;
      wsMessage.Format(wsError.c_str(), wsText.c_str(), wsSomField.c_str());
      pAppProvider->MsgBox(wsMessage, wsTitle, XFA_MBICON_Error, XFA_MB_OK);
    }
  }
}
FX_BOOL CXFA_FFTextEdit::IsDataChanged() {
  return (m_dwStatus & XFA_WidgetStatus_TextEditValueChanged) != 0;
}
uint32_t CXFA_FFTextEdit::GetAlignment() {
  uint32_t dwExtendedStyle = 0;
  if (CXFA_Para para = m_pDataAcc->GetPara()) {
    int32_t iHorz = para.GetHorizontalAlign();
    switch (iHorz) {
      case XFA_ATTRIBUTEENUM_Center:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_HCenter;
        break;
      case XFA_ATTRIBUTEENUM_Justify:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_Justified;
        break;
      case XFA_ATTRIBUTEENUM_JustifyAll:
        break;
      case XFA_ATTRIBUTEENUM_Radix:
        break;
      case XFA_ATTRIBUTEENUM_Right:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_HFar;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_HNear;
        break;
    }
    int32_t iVert = para.GetVerticalAlign();
    switch (iVert) {
      case XFA_ATTRIBUTEENUM_Middle:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_VCenter;
        break;
      case XFA_ATTRIBUTEENUM_Bottom:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_VFar;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_EDT_VNear;
        break;
    }
  }
  return dwExtendedStyle;
}
FX_BOOL CXFA_FFTextEdit::UpdateFWLData() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  XFA_VALUEPICTURE eType = XFA_VALUEPICTURE_Display;
  if (IsFocused()) {
    eType = XFA_VALUEPICTURE_Edit;
  }
  FX_BOOL bUpdate = FALSE;
  if (m_pDataAcc->GetUIType() == XFA_Element::TextEdit &&
      m_pDataAcc->GetNumberOfCells() < 0) {
    XFA_Element elementType = XFA_Element::Unknown;
    int32_t iMaxChars = m_pDataAcc->GetMaxChars(elementType);
    if (elementType == XFA_Element::ExData) {
      iMaxChars = eType == XFA_VALUEPICTURE_Edit ? iMaxChars : 0;
    }
    if (((CFWL_Edit*)m_pNormalWidget)->GetLimit() != iMaxChars) {
      ((CFWL_Edit*)m_pNormalWidget)->SetLimit(iMaxChars);
      bUpdate = TRUE;
    }
  }
  if (m_pDataAcc->GetUIType() == XFA_Element::Barcode) {
    int32_t nDataLen = 0;
    if (eType == XFA_VALUEPICTURE_Edit)
      m_pDataAcc->GetBarcodeAttribute_DataLength(nDataLen);
    static_cast<CFWL_Edit*>(m_pNormalWidget)->SetLimit(nDataLen);
    bUpdate = TRUE;
  }
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, eType);
  CFX_WideString wsOldText;
  ((CFWL_Edit*)m_pNormalWidget)->GetText(wsOldText);
  if (wsText != wsOldText || (eType == XFA_VALUEPICTURE_Edit && bUpdate)) {
    ((CFWL_Edit*)m_pNormalWidget)->SetText(wsText);
    bUpdate = TRUE;
  }
  if (bUpdate) {
    m_pNormalWidget->Update();
  }
  return TRUE;
}
FX_BOOL CXFA_FFTextEdit::CanUndo() {
  return ((CFWL_Edit*)m_pNormalWidget)->CanUndo();
}
FX_BOOL CXFA_FFTextEdit::CanRedo() {
  return ((CFWL_Edit*)m_pNormalWidget)->CanRedo();
}
FX_BOOL CXFA_FFTextEdit::Undo() {
  return ((CFWL_Edit*)m_pNormalWidget)->Undo();
}
FX_BOOL CXFA_FFTextEdit::Redo() {
  return ((CFWL_Edit*)m_pNormalWidget)->Redo();
}
FX_BOOL CXFA_FFTextEdit::CanCopy() {
  int32_t nCount = ((CFWL_Edit*)m_pNormalWidget)->CountSelRanges();
  return nCount > 0;
}
FX_BOOL CXFA_FFTextEdit::CanCut() {
  if (m_pNormalWidget->GetStylesEx() & FWL_STYLEEXT_EDT_ReadOnly) {
    return FALSE;
  }
  int32_t nCount = ((CFWL_Edit*)m_pNormalWidget)->CountSelRanges();
  return nCount > 0;
}
FX_BOOL CXFA_FFTextEdit::CanPaste() {
  return m_pDataAcc->GetAccess() == XFA_ATTRIBUTEENUM_Open;
}
FX_BOOL CXFA_FFTextEdit::CanSelectAll() {
  return ((CFWL_Edit*)m_pNormalWidget)->GetTextLength() > 0;
}
FX_BOOL CXFA_FFTextEdit::Copy(CFX_WideString& wsCopy) {
  return ((CFWL_Edit*)m_pNormalWidget)->Copy(wsCopy);
}
FX_BOOL CXFA_FFTextEdit::Cut(CFX_WideString& wsCut) {
  return ((CFWL_Edit*)m_pNormalWidget)->Cut(wsCut);
}
FX_BOOL CXFA_FFTextEdit::Paste(const CFX_WideString& wsPaste) {
  return ((CFWL_Edit*)m_pNormalWidget)->Paste(wsPaste);
}
FX_BOOL CXFA_FFTextEdit::SelectAll() {
  int32_t nCount = ((CFWL_Edit*)m_pNormalWidget)->GetTextLength();
  return ((CFWL_Edit*)m_pNormalWidget)->AddSelRange(0, nCount);
}
FX_BOOL CXFA_FFTextEdit::Delete() {
  return ((CFWL_Edit*)m_pNormalWidget)->Delete();
}
FX_BOOL CXFA_FFTextEdit::DeSelect() {
  return ((CFWL_Edit*)m_pNormalWidget)->ClearSelections() ==
         FWL_Error::Succeeded;
}
FX_BOOL CXFA_FFTextEdit::GetSuggestWords(
    CFX_PointF pointf,
    std::vector<CFX_ByteString>& sSuggest) {
  if (m_pDataAcc->GetUIType() != XFA_Element::TextEdit) {
    return FALSE;
  }
  FWLToClient(pointf.x, pointf.y);
  return ((CFWL_Edit*)m_pNormalWidget)->GetSuggestWords(pointf, sSuggest);
}
FX_BOOL CXFA_FFTextEdit::ReplaceSpellCheckWord(
    CFX_PointF pointf,
    const CFX_ByteStringC& bsReplace) {
  if (m_pDataAcc->GetUIType() != XFA_Element::TextEdit) {
    return FALSE;
  }
  FWLToClient(pointf.x, pointf.y);
  return ((CFWL_Edit*)m_pNormalWidget)
      ->ReplaceSpellCheckWord(pointf, bsReplace);
}
void CXFA_FFTextEdit::OnTextChanged(IFWL_Widget* pWidget,
                                    const CFX_WideString& wsChanged,
                                    const CFX_WideString& wsPrevText) {
  m_dwStatus |= XFA_WidgetStatus_TextEditValueChanged;
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Change;
  eParam.m_wsChange = wsChanged;
  eParam.m_pTarget = m_pDataAcc;
  eParam.m_wsPrevText = wsPrevText;
  CFWL_Edit* pEdit = ((CFWL_Edit*)m_pNormalWidget);
  if (m_pDataAcc->GetUIType() == XFA_Element::DateTimeEdit) {
    CFWL_DateTimePicker* pDateTime = (CFWL_DateTimePicker*)pEdit;
    pDateTime->GetEditText(eParam.m_wsNewText);
    int32_t iSels = pDateTime->CountSelRanges();
    if (iSels) {
      eParam.m_iSelEnd = pDateTime->GetSelRange(0, eParam.m_iSelStart);
    }
  } else {
    pEdit->GetText(eParam.m_wsNewText);
    int32_t iSels = pEdit->CountSelRanges();
    if (iSels) {
      eParam.m_iSelEnd = pEdit->GetSelRange(0, eParam.m_iSelStart);
    }
  }
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Change, &eParam);
}
void CXFA_FFTextEdit::OnTextFull(IFWL_Widget* pWidget) {
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Full;
  eParam.m_pTarget = m_pDataAcc;
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Full, &eParam);
}

FX_BOOL CXFA_FFTextEdit::CheckWord(const CFX_ByteStringC& sWord) {
  if (sWord.IsEmpty() || m_pDataAcc->GetUIType() != XFA_Element::TextEdit) {
    return TRUE;
  }
  return GetDoc()->GetDocProvider()->CheckWord(GetDoc(), sWord);
}
FX_BOOL CXFA_FFTextEdit::GetSuggestWords(
    const CFX_ByteStringC& sWord,
    std::vector<CFX_ByteString>& sSuggest) {
  if (m_pDataAcc->GetUIType() != XFA_Element::TextEdit) {
    return FALSE;
  }
  return GetDoc()->GetDocProvider()->GetSuggestWords(GetDoc(), sWord, sSuggest);
}

void CXFA_FFTextEdit::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFTextEdit::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  switch (pEvent->GetClassID()) {
    case CFWL_EventType::TextChanged: {
      CFWL_EvtEdtTextChanged* event = (CFWL_EvtEdtTextChanged*)pEvent;
      CFX_WideString wsChange;
      OnTextChanged(m_pNormalWidget->GetWidget(), wsChange, event->wsPrevText);
      break;
    }
    case CFWL_EventType::TextFull: {
      OnTextFull(m_pNormalWidget->GetWidget());
      break;
    }
    case CFWL_EventType::CheckWord: {
      CFX_WideString wstr(L"FWL_EVENT_DTP_SelectChanged");
      CFWL_EvtEdtCheckWord* event = (CFWL_EvtEdtCheckWord*)pEvent;
      event->bCheckWord = CheckWord(event->bsWord.AsStringC());
      break;
    }
    case CFWL_EventType::GetSuggestedWords: {
      CFWL_EvtEdtGetSuggestWords* event = (CFWL_EvtEdtGetSuggestWords*)pEvent;
      event->bSuggestWords = GetSuggestWords(event->bsWord.AsStringC(),
                                             event->bsArraySuggestWords);
      break;
    }
    default:
      break;
  }
  m_pOldDelegate->OnProcessEvent(pEvent);
}

void CXFA_FFTextEdit::OnDrawWidget(CFX_Graphics* pGraphics,
                                   const CFX_Matrix* pMatrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, pMatrix);
}

CXFA_FFNumericEdit::CXFA_FFNumericEdit(CXFA_FFPageView* pPageView,
                                       CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFTextEdit(pPageView, pDataAcc) {}
CXFA_FFNumericEdit::~CXFA_FFNumericEdit() {}
FX_BOOL CXFA_FFNumericEdit::LoadWidget() {
  CFWL_Edit* pWidget = CFWL_Edit::Create();
  pWidget->Initialize();
  m_pNormalWidget = pWidget;
  IFWL_Widget* pIWidget = m_pNormalWidget->GetWidget();
  m_pNormalWidget->SetLayoutItem(this);
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pIWidget, pIWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Display);
  pWidget->SetText(wsText);
  UpdateWidgetProperty();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFNumericEdit::UpdateWidgetProperty() {
  CFWL_Edit* pWidget = (CFWL_Edit*)m_pNormalWidget;
  if (!pWidget) {
    return;
  }
  uint32_t dwExtendedStyle =
      FWL_STYLEEXT_EDT_ShowScrollbarFocus | FWL_STYLEEXT_EDT_OuterScrollbar |
      FWL_STYLEEXT_EDT_Validate | FWL_STYLEEXT_EDT_Number |
      FWL_STYLEEXT_EDT_LastLineHeight;
  dwExtendedStyle |= UpdateUIProperty();
  if (m_pDataAcc->GetHorizontalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_AutoHScroll;
  }
  int32_t iNumCells = m_pDataAcc->GetNumberOfCells();
  if (iNumCells > 0) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_CombText;
    pWidget->SetLimit(iNumCells);
  }
  dwExtendedStyle |= GetAlignment();
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_ReadOnly;
  }
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
}

void CXFA_FFNumericEdit::OnProcessEvent(CFWL_Event* pEvent) {
  if (pEvent->GetClassID() == CFWL_EventType::Validate) {
    CFWL_EvtEdtValidate* event = (CFWL_EvtEdtValidate*)pEvent;
    CFX_WideString wsChange = event->wsInsert;
    event->bValidate = OnValidate(m_pNormalWidget->GetWidget(), wsChange);
    return;
  }
  CXFA_FFTextEdit::OnProcessEvent(pEvent);
}

FX_BOOL CXFA_FFNumericEdit::OnValidate(IFWL_Widget* pWidget,
                                       CFX_WideString& wsText) {
  CFX_WideString wsPattern;
  m_pDataAcc->GetPictureContent(wsPattern, XFA_VALUEPICTURE_Edit);
  if (!wsPattern.IsEmpty()) {
    return TRUE;
  }
  int32_t iLeads = 0;
  m_pDataAcc->GetLeadDigits(iLeads);
  int32_t iFracs = 0;
  m_pDataAcc->GetFracDigits(iFracs);
  CFX_WideString wsFormat;
  CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(m_pDataAcc);
  widgetValue.GetNumbericFormat(wsFormat, iLeads, iFracs);
  return widgetValue.ValidateNumericTemp(wsText, wsFormat,
                                         m_pDataAcc->GetLocal());
}
CXFA_FFPasswordEdit::CXFA_FFPasswordEdit(CXFA_FFPageView* pPageView,
                                         CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFTextEdit(pPageView, pDataAcc) {}
CXFA_FFPasswordEdit::~CXFA_FFPasswordEdit() {}
FX_BOOL CXFA_FFPasswordEdit::LoadWidget() {
  CFWL_Edit* pWidget = CFWL_Edit::Create();
  pWidget->Initialize();
  m_pNormalWidget = pWidget;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pIWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pIWidget, pIWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Display);
  pWidget->SetText(wsText);
  UpdateWidgetProperty();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFPasswordEdit::UpdateWidgetProperty() {
  CFWL_Edit* pWidget = (CFWL_Edit*)m_pNormalWidget;
  if (!pWidget) {
    return;
  }
  uint32_t dwExtendedStyle =
      FWL_STYLEEXT_EDT_ShowScrollbarFocus | FWL_STYLEEXT_EDT_OuterScrollbar |
      FWL_STYLEEXT_EDT_Password | FWL_STYLEEXT_EDT_LastLineHeight;
  dwExtendedStyle |= UpdateUIProperty();
  CFX_WideString wsPassWord;
  m_pDataAcc->GetPasswordChar(wsPassWord);
  if (!wsPassWord.IsEmpty()) {
    pWidget->SetAliasChar(wsPassWord.GetAt(0));
  }
  if (m_pDataAcc->GetHorizontalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_AutoHScroll;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    dwExtendedStyle |= FWL_STYLEEXT_EDT_ReadOnly;
  }
  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
}
CXFA_FFDateTimeEdit::CXFA_FFDateTimeEdit(CXFA_FFPageView* pPageView,
                                         CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFTextEdit(pPageView, pDataAcc) {}

CXFA_FFDateTimeEdit::~CXFA_FFDateTimeEdit() {}

FX_BOOL CXFA_FFDateTimeEdit::GetBBox(CFX_RectF& rtBox,
                                     uint32_t dwStatus,
                                     FX_BOOL bDrawFocus) {
  if (bDrawFocus)
    return FALSE;
  return CXFA_FFWidget::GetBBox(rtBox, dwStatus);
}

FX_BOOL CXFA_FFDateTimeEdit::PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFX_RectF rtWidget;
  ((CFWL_DateTimePicker*)m_pNormalWidget)->GetBBox(rtWidget);
  if (rtWidget.Contains(fx, fy)) {
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_FFDateTimeEdit::LoadWidget() {
  CFWL_DateTimePicker* pWidget = CFWL_DateTimePicker::Create();
  pWidget->Initialize();
  m_pNormalWidget = pWidget;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pIWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pIWidget, pIWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  m_pNormalWidget->LockUpdate();
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, XFA_VALUEPICTURE_Display);
  pWidget->SetEditText(wsText);
  if (CXFA_Value value = m_pDataAcc->GetFormValue()) {
    switch (value.GetChildValueClassID()) {
      case XFA_Element::Date: {
        if (!wsText.IsEmpty()) {
          CXFA_LocaleValue lcValue = XFA_GetLocaleValue(m_pDataAcc);
          CFX_Unitime date = lcValue.GetDate();
          if ((FX_UNITIME)date != 0) {
            pWidget->SetCurSel(date.GetYear(), date.GetMonth(), date.GetDay());
          }
        }
      } break;
      default:
        break;
    }
  }
  UpdateWidgetProperty();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFDateTimeEdit::UpdateWidgetProperty() {
  CFWL_DateTimePicker* pWidget = (CFWL_DateTimePicker*)m_pNormalWidget;
  if (!pWidget) {
    return;
  }
  uint32_t dwExtendedStyle = FWL_STYLEEXT_DTP_ShortDateFormat;
  dwExtendedStyle |= UpdateUIProperty();
  dwExtendedStyle |= GetAlignment();
  m_pNormalWidget->ModifyStylesEx(dwExtendedStyle, 0xFFFFFFFF);
  uint32_t dwEditStyles = FWL_STYLEEXT_EDT_LastLineHeight;
  int32_t iNumCells = m_pDataAcc->GetNumberOfCells();
  if (iNumCells > 0) {
    dwEditStyles |= FWL_STYLEEXT_EDT_CombText;
    pWidget->SetEditLimit(iNumCells);
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    dwEditStyles |= FWL_STYLEEXT_EDT_ReadOnly;
  }
  if (m_pDataAcc->GetHorizontalScrollPolicy() != XFA_ATTRIBUTEENUM_Off) {
    dwEditStyles |= FWL_STYLEEXT_EDT_AutoHScroll;
  }
  pWidget->ModifyEditStylesEx(dwEditStyles, 0xFFFFFFFF);
}
uint32_t CXFA_FFDateTimeEdit::GetAlignment() {
  uint32_t dwExtendedStyle = 0;
  if (CXFA_Para para = m_pDataAcc->GetPara()) {
    int32_t iHorz = para.GetHorizontalAlign();
    switch (iHorz) {
      case XFA_ATTRIBUTEENUM_Center:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditHCenter;
        break;
      case XFA_ATTRIBUTEENUM_Justify:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditJustified;
        break;
      case XFA_ATTRIBUTEENUM_JustifyAll:
        break;
      case XFA_ATTRIBUTEENUM_Radix:
        break;
      case XFA_ATTRIBUTEENUM_Right:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditHFar;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditHNear;
        break;
    }
    int32_t iVert = para.GetVerticalAlign();
    switch (iVert) {
      case XFA_ATTRIBUTEENUM_Middle:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditVCenter;
        break;
      case XFA_ATTRIBUTEENUM_Bottom:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditVFar;
        break;
      default:
        dwExtendedStyle |= FWL_STYLEEXT_DTP_EditVNear;
        break;
    }
  }
  return dwExtendedStyle;
}
FX_BOOL CXFA_FFDateTimeEdit::CommitData() {
  CFX_WideString wsText;
  ((CFWL_DateTimePicker*)m_pNormalWidget)->GetEditText(wsText);
  if (m_pDataAcc->SetValue(wsText, XFA_VALUEPICTURE_Edit)) {
    m_pDataAcc->UpdateUIDisplay(this);
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_FFDateTimeEdit::UpdateFWLData() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  XFA_VALUEPICTURE eType = XFA_VALUEPICTURE_Display;
  if (IsFocused()) {
    eType = XFA_VALUEPICTURE_Edit;
  }
  CFX_WideString wsText;
  m_pDataAcc->GetValue(wsText, eType);
  ((CFWL_DateTimePicker*)m_pNormalWidget)->SetEditText(wsText);
  if (IsFocused() && !wsText.IsEmpty()) {
    CXFA_LocaleValue lcValue = XFA_GetLocaleValue(m_pDataAcc);
    CFX_Unitime date = lcValue.GetDate();
    if (lcValue.IsValid()) {
      if ((FX_UNITIME)date != 0) {
        ((CFWL_DateTimePicker*)m_pNormalWidget)
            ->SetCurSel(date.GetYear(), date.GetMonth(), date.GetDay());
      }
    }
  }
  m_pNormalWidget->Update();
  return TRUE;
}
FX_BOOL CXFA_FFDateTimeEdit::IsDataChanged() {
  if (m_dwStatus & XFA_WidgetStatus_TextEditValueChanged) {
    return TRUE;
  }
  CFX_WideString wsText;
  ((CFWL_DateTimePicker*)m_pNormalWidget)->GetEditText(wsText);
  CFX_WideString wsOldValue;
  m_pDataAcc->GetValue(wsOldValue, XFA_VALUEPICTURE_Edit);
  return wsOldValue != wsText;
}
FX_BOOL CXFA_FFDateTimeEdit::CanUndo() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->CanUndo();
}
FX_BOOL CXFA_FFDateTimeEdit::CanRedo() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->CanRedo();
}
FX_BOOL CXFA_FFDateTimeEdit::Undo() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Undo();
}
FX_BOOL CXFA_FFDateTimeEdit::Redo() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Redo();
}
FX_BOOL CXFA_FFDateTimeEdit::CanCopy() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->CanCopy();
}
FX_BOOL CXFA_FFDateTimeEdit::CanCut() {
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->CanCut();
}
FX_BOOL CXFA_FFDateTimeEdit::CanPaste() {
  return m_pDataAcc->GetAccess() == XFA_ATTRIBUTEENUM_Open;
}
FX_BOOL CXFA_FFDateTimeEdit::CanSelectAll() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->CanSelectAll();
}
FX_BOOL CXFA_FFDateTimeEdit::Copy(CFX_WideString& wsCopy) {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Copy(wsCopy);
}
FX_BOOL CXFA_FFDateTimeEdit::Cut(CFX_WideString& wsCut) {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Cut(wsCut);
}
FX_BOOL CXFA_FFDateTimeEdit::Paste(const CFX_WideString& wsPaste) {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Paste(wsPaste);
}
FX_BOOL CXFA_FFDateTimeEdit::SelectAll() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->SelectAll();
}
FX_BOOL CXFA_FFDateTimeEdit::Delete() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->Delete();
}
FX_BOOL CXFA_FFDateTimeEdit::DeSelect() {
  return ((CFWL_DateTimePicker*)m_pNormalWidget)->DeSelect();
}
void CXFA_FFDateTimeEdit::OnSelectChanged(IFWL_Widget* pWidget,
                                          int32_t iYear,
                                          int32_t iMonth,
                                          int32_t iDay) {
  CFX_WideString wsPicture;
  m_pDataAcc->GetPictureContent(wsPicture, XFA_VALUEPICTURE_Edit);
  CXFA_LocaleValue date(XFA_VT_DATE, GetDoc()->GetXFADoc()->GetLocalMgr());
  CFX_Unitime dt;
  dt.Set(iYear, iMonth, iDay);
  date.SetDate(dt);
  CFX_WideString wsDate;
  date.FormatPatterns(wsDate, wsPicture, m_pDataAcc->GetLocal(),
                      XFA_VALUEPICTURE_Edit);
  CFWL_DateTimePicker* pDateTime = (CFWL_DateTimePicker*)m_pNormalWidget;
  pDateTime->SetEditText(wsDate);
  pDateTime->Update();
  GetDoc()->GetDocProvider()->SetFocusWidget(GetDoc(), nullptr);
  CXFA_EventParam eParam;
  eParam.m_eType = XFA_EVENT_Change;
  eParam.m_pTarget = m_pDataAcc;
  m_pDataAcc->GetValue(eParam.m_wsNewText, XFA_VALUEPICTURE_Raw);
  m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Change, &eParam);
}

void CXFA_FFDateTimeEdit::OnProcessEvent(CFWL_Event* pEvent) {
  if (pEvent->GetClassID() == CFWL_EventType::SelectChanged) {
    CFWL_Event_DtpSelectChanged* event = (CFWL_Event_DtpSelectChanged*)pEvent;
    OnSelectChanged(m_pNormalWidget->GetWidget(), event->iYear, event->iMonth,
                    event->iDay);
    return;
  }
  CXFA_FFTextEdit::OnProcessEvent(pEvent);
}
