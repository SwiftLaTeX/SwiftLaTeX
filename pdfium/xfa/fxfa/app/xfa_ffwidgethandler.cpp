// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/xfa_ffwidgethandler.h"

#include <vector>

#include "xfa/fxfa/app/xfa_ffchoicelist.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"

CXFA_FFWidgetHandler::CXFA_FFWidgetHandler(CXFA_FFDocView* pDocView)
    : m_pDocView(pDocView) {}

CXFA_FFWidgetHandler::~CXFA_FFWidgetHandler() {}

FX_BOOL CXFA_FFWidgetHandler::OnMouseEnter(CXFA_FFWidget* hWidget) {
  m_pDocView->LockUpdate();
  FX_BOOL bRet = hWidget->OnMouseEnter();
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnMouseExit(CXFA_FFWidget* hWidget) {
  m_pDocView->LockUpdate();
  FX_BOOL bRet = hWidget->OnMouseExit();
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnLButtonDown(CXFA_FFWidget* hWidget,
                                            uint32_t dwFlags,
                                            FX_FLOAT fx,
                                            FX_FLOAT fy) {
  m_pDocView->LockUpdate();
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnLButtonDown(dwFlags, fx, fy);
  if (bRet && m_pDocView->SetFocus(hWidget)) {
    m_pDocView->GetDoc()->GetDocProvider()->SetFocusWidget(m_pDocView->GetDoc(),
                                                           hWidget);
  }
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnLButtonUp(CXFA_FFWidget* hWidget,
                                          uint32_t dwFlags,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  m_pDocView->LockUpdate();
  hWidget->Rotate2Normal(fx, fy);
  m_pDocView->m_bLayoutEvent = TRUE;
  FX_BOOL bRet = hWidget->OnLButtonUp(dwFlags, fx, fy);
  m_pDocView->UnlockUpdate();
  m_pDocView->UpdateDocView();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnLButtonDblClk(CXFA_FFWidget* hWidget,
                                              uint32_t dwFlags,
                                              FX_FLOAT fx,
                                              FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnLButtonDblClk(dwFlags, fx, fy);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnMouseMove(CXFA_FFWidget* hWidget,
                                          uint32_t dwFlags,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnMouseMove(dwFlags, fx, fy);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnMouseWheel(CXFA_FFWidget* hWidget,
                                           uint32_t dwFlags,
                                           int16_t zDelta,
                                           FX_FLOAT fx,
                                           FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnMouseWheel(dwFlags, zDelta, fx, fy);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnRButtonDown(CXFA_FFWidget* hWidget,
                                            uint32_t dwFlags,
                                            FX_FLOAT fx,
                                            FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnRButtonDown(dwFlags, fx, fy);
  if (bRet && m_pDocView->SetFocus(hWidget)) {
    m_pDocView->GetDoc()->GetDocProvider()->SetFocusWidget(m_pDocView->GetDoc(),
                                                           hWidget);
  }
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnRButtonUp(CXFA_FFWidget* hWidget,
                                          uint32_t dwFlags,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnRButtonUp(dwFlags, fx, fy);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnRButtonDblClk(CXFA_FFWidget* hWidget,
                                              uint32_t dwFlags,
                                              FX_FLOAT fx,
                                              FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  FX_BOOL bRet = hWidget->OnRButtonDblClk(dwFlags, fx, fy);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnKeyDown(CXFA_FFWidget* hWidget,
                                        uint32_t dwKeyCode,
                                        uint32_t dwFlags) {
  FX_BOOL bRet = hWidget->OnKeyDown(dwKeyCode, dwFlags);
  m_pDocView->RunInvalidate();
  m_pDocView->UpdateDocView();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnKeyUp(CXFA_FFWidget* hWidget,
                                      uint32_t dwKeyCode,
                                      uint32_t dwFlags) {
  FX_BOOL bRet = hWidget->OnKeyUp(dwKeyCode, dwFlags);
  m_pDocView->RunInvalidate();
  return bRet;
}

FX_BOOL CXFA_FFWidgetHandler::OnChar(CXFA_FFWidget* hWidget,
                                     uint32_t dwChar,
                                     uint32_t dwFlags) {
  FX_BOOL bRet = hWidget->OnChar(dwChar, dwFlags);
  m_pDocView->RunInvalidate();
  return bRet;
}

FWL_WidgetHit CXFA_FFWidgetHandler::OnHitTest(CXFA_FFWidget* hWidget,
                                              FX_FLOAT fx,
                                              FX_FLOAT fy) {
  if (!(hWidget->GetStatus() & XFA_WidgetStatus_Visible))
    return FWL_WidgetHit::Unknown;

  hWidget->Rotate2Normal(fx, fy);
  return hWidget->OnHitTest(fx, fy);
}

FX_BOOL CXFA_FFWidgetHandler::OnSetCursor(CXFA_FFWidget* hWidget,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  hWidget->Rotate2Normal(fx, fy);
  return hWidget->OnSetCursor(fx, fy);
}

void CXFA_FFWidgetHandler::RenderWidget(CXFA_FFWidget* hWidget,
                                        CFX_Graphics* pGS,
                                        CFX_Matrix* pMatrix,
                                        FX_BOOL bHighlight) {
  hWidget->RenderWidget(pGS, pMatrix,
                        bHighlight ? XFA_WidgetStatus_Highlight : 0);
}

FX_BOOL CXFA_FFWidgetHandler::HasEvent(CXFA_WidgetAcc* pWidgetAcc,
                                       XFA_EVENTTYPE eEventType) {
  if (!pWidgetAcc || eEventType == XFA_EVENT_Unknown)
    return FALSE;
  if (pWidgetAcc->GetElementType() == XFA_Element::Draw)
    return FALSE;

  switch (eEventType) {
    case XFA_EVENT_Calculate: {
      CXFA_Calculate calc = pWidgetAcc->GetCalculate();
      if (!calc)
        return FALSE;
      if (calc.GetScript())
        return TRUE;
      return FALSE;
    }
    case XFA_EVENT_Validate: {
      CXFA_Validate val = pWidgetAcc->GetValidate();
      if (!val)
        return FALSE;
      if (val.GetScript())
        return TRUE;
      return FALSE;
    }
    default:
      break;
  }
  CXFA_NodeArray eventArray;
  return pWidgetAcc->GetEventByActivity(gs_EventActivity[eEventType],
                                        eventArray);
}

int32_t CXFA_FFWidgetHandler::ProcessEvent(CXFA_WidgetAcc* pWidgetAcc,
                                           CXFA_EventParam* pParam) {
  if (!pParam || pParam->m_eType == XFA_EVENT_Unknown)
    return XFA_EVENTERROR_NotExist;
  if (!pWidgetAcc || pWidgetAcc->GetElementType() == XFA_Element::Draw)
    return XFA_EVENTERROR_NotExist;

  switch (pParam->m_eType) {
    case XFA_EVENT_Calculate:
      return pWidgetAcc->ProcessCalculate();
    case XFA_EVENT_Validate:
      if (m_pDocView->GetDoc()->GetDocProvider()->IsValidationsEnabled(
              m_pDocView->GetDoc())) {
        return pWidgetAcc->ProcessValidate();
      }
      return XFA_EVENTERROR_Disabled;
    case XFA_EVENT_InitCalculate: {
      CXFA_Calculate calc = pWidgetAcc->GetCalculate();
      if (!calc)
        return XFA_EVENTERROR_NotExist;
      if (pWidgetAcc->GetNode()->IsUserInteractive())
        return XFA_EVENTERROR_Disabled;

      CXFA_Script script = calc.GetScript();
      return pWidgetAcc->ExecuteScript(script, pParam);
    }
    default:
      break;
  }
  int32_t iRet =
      pWidgetAcc->ProcessEvent(gs_EventActivity[pParam->m_eType], pParam);
  return iRet;
}

CXFA_FFWidget* CXFA_FFWidgetHandler::CreateWidget(CXFA_FFWidget* hParent,
                                                  XFA_WIDGETTYPE eType,
                                                  CXFA_FFWidget* hBefore) {
  CXFA_Node* pParentFormItem =
      hParent ? hParent->GetDataAcc()->GetNode() : nullptr;
  CXFA_Node* pBeforeFormItem =
      hBefore ? hBefore->GetDataAcc()->GetNode() : nullptr;
  CXFA_Node* pNewFormItem =
      CreateWidgetFormItem(eType, pParentFormItem, pBeforeFormItem);
  if (!pNewFormItem)
    return nullptr;

  pNewFormItem->GetTemplateNode()->SetFlag(XFA_NodeFlag_Initialized, true);
  pNewFormItem->SetFlag(XFA_NodeFlag_Initialized, true);
  m_pDocView->RunLayout();
  CXFA_LayoutItem* pLayout =
      m_pDocView->GetXFALayout()->GetLayoutItem(pNewFormItem);
  return static_cast<CXFA_FFWidget*>(pLayout);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateWidgetFormItem(
    XFA_WIDGETTYPE eType,
    CXFA_Node* pParent,
    CXFA_Node* pBefore) const {
  switch (eType) {
    case XFA_WIDGETTYPE_Barcode:
      return nullptr;
    case XFA_WIDGETTYPE_PushButton:
      return CreatePushButton(pParent, pBefore);
    case XFA_WIDGETTYPE_CheckButton:
      return CreateCheckButton(pParent, pBefore);
    case XFA_WIDGETTYPE_ExcludeGroup:
      return CreateExclGroup(pParent, pBefore);
    case XFA_WIDGETTYPE_RadioButton:
      return CreateRadioButton(pParent, pBefore);
    case XFA_WIDGETTYPE_Arc:
      return CreateArc(pParent, pBefore);
    case XFA_WIDGETTYPE_Rectangle:
      return CreateRectangle(pParent, pBefore);
    case XFA_WIDGETTYPE_Image:
      return CreateImage(pParent, pBefore);
    case XFA_WIDGETTYPE_Line:
      return CreateLine(pParent, pBefore);
    case XFA_WIDGETTYPE_Text:
      return CreateText(pParent, pBefore);
    case XFA_WIDGETTYPE_DatetimeEdit:
      return CreateDatetimeEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_DecimalField:
      return CreateDecimalField(pParent, pBefore);
    case XFA_WIDGETTYPE_NumericField:
      return CreateNumericField(pParent, pBefore);
    case XFA_WIDGETTYPE_Signature:
      return CreateSignature(pParent, pBefore);
    case XFA_WIDGETTYPE_TextEdit:
      return CreateTextEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_DropdownList:
      return CreateDropdownList(pParent, pBefore);
    case XFA_WIDGETTYPE_ListBox:
      return CreateListBox(pParent, pBefore);
    case XFA_WIDGETTYPE_ImageField:
      return CreateImageField(pParent, pBefore);
    case XFA_WIDGETTYPE_PasswordEdit:
      return CreatePasswordEdit(pParent, pBefore);
    case XFA_WIDGETTYPE_Subform:
      return CreateSubform(pParent, pBefore);
    default:
      return nullptr;
  }
}

CXFA_Node* CXFA_FFWidgetHandler::CreatePushButton(CXFA_Node* pParent,
                                                  CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::Button, pParent, pBefore);
  CXFA_Node* pCaption = CreateCopyNode(XFA_Element::Caption, pField);
  CXFA_Node* pValue = CreateCopyNode(XFA_Element::Value, pCaption);
  CXFA_Node* pText = CreateCopyNode(XFA_Element::Text, pValue);
  pText->SetContent(L"Button", L"Button", FALSE);

  CXFA_Node* pPara = CreateCopyNode(XFA_Element::Para, pCaption);
  pPara->SetEnum(XFA_ATTRIBUTE_VAlign, XFA_ATTRIBUTEENUM_Middle, FALSE);
  pPara->SetEnum(XFA_ATTRIBUTE_HAlign, XFA_ATTRIBUTEENUM_Center, FALSE);
  CreateFontNode(pCaption);

  CXFA_Node* pBorder = CreateCopyNode(XFA_Element::Border, pField);
  pBorder->SetEnum(XFA_ATTRIBUTE_Hand, XFA_ATTRIBUTEENUM_Right, FALSE);

  CXFA_Node* pEdge = CreateCopyNode(XFA_Element::Edge, pBorder);
  pEdge->SetEnum(XFA_ATTRIBUTE_Stroke, XFA_ATTRIBUTEENUM_Raised, FALSE);

  CXFA_Node* pFill = CreateCopyNode(XFA_Element::Fill, pBorder);
  CXFA_Node* pColor = CreateCopyNode(XFA_Element::Color, pFill);
  pColor->SetCData(XFA_ATTRIBUTE_Value, L"212, 208, 200", FALSE);

  CXFA_Node* pBind = CreateCopyNode(XFA_Element::Bind, pField);
  pBind->SetEnum(XFA_ATTRIBUTE_Match, XFA_ATTRIBUTEENUM_None);

  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateCheckButton(CXFA_Node* pParent,
                                                   CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::CheckButton, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateExclGroup(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateFormItem(XFA_Element::ExclGroup, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateRadioButton(CXFA_Node* pParent,
                                                   CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::CheckButton, pParent, pBefore);
  CXFA_Node* pUi = pField->GetFirstChildByClass(XFA_Element::Ui);
  CXFA_Node* pWidget = pUi->GetFirstChildByClass(XFA_Element::CheckButton);
  pWidget->SetEnum(XFA_ATTRIBUTE_Shape, XFA_ATTRIBUTEENUM_Round);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDatetimeEdit(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::DateTimeEdit, pParent, pBefore);
  CreateValueNode(XFA_Element::Date, pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDecimalField(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateNumericField(pParent, pBefore);
  CreateValueNode(XFA_Element::Decimal, pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateNumericField(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::NumericEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateSignature(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::Signature, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateTextEdit(CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::TextEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDropdownList(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::ChoiceList, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateListBox(CXFA_Node* pParent,
                                               CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDropdownList(pParent, pBefore);
  CXFA_Node* pUi = pField->GetNodeItem(XFA_NODEITEM_FirstChild);
  CXFA_Node* pListBox = pUi->GetNodeItem(XFA_NODEITEM_FirstChild);
  pListBox->SetEnum(XFA_ATTRIBUTE_Open, XFA_ATTRIBUTEENUM_Always);
  pListBox->SetEnum(XFA_ATTRIBUTE_CommitOn, XFA_ATTRIBUTEENUM_Exit);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateImageField(CXFA_Node* pParent,
                                                  CXFA_Node* pBefore) const {
  return CreateField(XFA_Element::ImageEdit, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreatePasswordEdit(CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateField(XFA_Element::PasswordEdit, pParent, pBefore);
  CXFA_Node* pBind = CreateCopyNode(XFA_Element::Bind, pField);
  pBind->SetEnum(XFA_ATTRIBUTE_Match, XFA_ATTRIBUTEENUM_None, FALSE);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateField(XFA_Element eElement,
                                             CXFA_Node* pParent,
                                             CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateFormItem(XFA_Element::Field, pParent, pBefore);
  CreateCopyNode(eElement, CreateCopyNode(XFA_Element::Ui, pField));
  CreateFontNode(pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateArc(CXFA_Node* pParent,
                                           CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Arc, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateRectangle(CXFA_Node* pParent,
                                                 CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Rectangle, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateImage(CXFA_Node* pParent,
                                             CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDraw(XFA_Element::Image, pParent, pBefore);
  CreateCopyNode(XFA_Element::ImageEdit,
                 CreateCopyNode(XFA_Element::Ui, pField));
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateLine(CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  return CreateDraw(XFA_Element::Line, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateText(CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  CXFA_Node* pField = CreateDraw(XFA_Element::Text, pParent, pBefore);
  CreateCopyNode(XFA_Element::TextEdit,
                 CreateCopyNode(XFA_Element::Ui, pField));
  CreateFontNode(pField);
  return pField;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateDraw(XFA_Element eElement,
                                            CXFA_Node* pParent,
                                            CXFA_Node* pBefore) const {
  CXFA_Node* pDraw = CreateFormItem(XFA_Element::Draw, pParent, pBefore);
  CreateValueNode(eElement, pDraw);
  return pDraw;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateSubform(CXFA_Node* pParent,
                                               CXFA_Node* pBefore) const {
  return CreateFormItem(XFA_Element::Subform, pParent, pBefore);
}

CXFA_Node* CXFA_FFWidgetHandler::CreateFormItem(XFA_Element eElement,
                                                CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  CXFA_Node* pTemplateParent = pParent ? pParent->GetTemplateNode() : nullptr;
  CXFA_Node* pNewFormItem = pTemplateParent->CloneTemplateToForm(FALSE);
  if (pParent)
    pParent->InsertChild(pNewFormItem, pBefore);
  return pNewFormItem;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateCopyNode(XFA_Element eElement,
                                                CXFA_Node* pParent,
                                                CXFA_Node* pBefore) const {
  CXFA_Node* pTemplateParent = pParent ? pParent->GetTemplateNode() : nullptr;
  CXFA_Node* pNewNode =
      CreateTemplateNode(eElement, pTemplateParent,
                         pBefore ? pBefore->GetTemplateNode() : nullptr)
          ->Clone(FALSE);
  if (pParent)
    pParent->InsertChild(pNewNode, pBefore);
  return pNewNode;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateTemplateNode(XFA_Element eElement,
                                                    CXFA_Node* pParent,
                                                    CXFA_Node* pBefore) const {
  CXFA_Document* pXFADoc = GetXFADoc();
  CXFA_Node* pNewTemplateNode =
      pXFADoc->CreateNode(XFA_XDPPACKET_Template, eElement);
  if (pParent)
    pParent->InsertChild(pNewTemplateNode, pBefore);
  return pNewTemplateNode;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateFontNode(CXFA_Node* pParent) const {
  CXFA_Node* pFont = CreateCopyNode(XFA_Element::Font, pParent);
  pFont->SetCData(XFA_ATTRIBUTE_Typeface, L"Myriad Pro", FALSE);
  return pFont;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateMarginNode(CXFA_Node* pParent,
                                                  uint32_t dwFlags,
                                                  FX_FLOAT fInsets[4]) const {
  CXFA_Node* pMargin = CreateCopyNode(XFA_Element::Margin, pParent);
  if (dwFlags & 0x01)
    pMargin->SetMeasure(XFA_ATTRIBUTE_LeftInset,
                        CXFA_Measurement(fInsets[0], XFA_UNIT_Pt), FALSE);
  if (dwFlags & 0x02)
    pMargin->SetMeasure(XFA_ATTRIBUTE_TopInset,
                        CXFA_Measurement(fInsets[1], XFA_UNIT_Pt), FALSE);
  if (dwFlags & 0x04)
    pMargin->SetMeasure(XFA_ATTRIBUTE_RightInset,
                        CXFA_Measurement(fInsets[2], XFA_UNIT_Pt), FALSE);
  if (dwFlags & 0x08)
    pMargin->SetMeasure(XFA_ATTRIBUTE_BottomInset,
                        CXFA_Measurement(fInsets[3], XFA_UNIT_Pt), FALSE);
  return pMargin;
}

CXFA_Node* CXFA_FFWidgetHandler::CreateValueNode(XFA_Element eValue,
                                                 CXFA_Node* pParent) const {
  CXFA_Node* pValue = CreateCopyNode(XFA_Element::Value, pParent);
  CreateCopyNode(eValue, pValue);
  return pValue;
}

CXFA_Document* CXFA_FFWidgetHandler::GetObjFactory() const {
  return GetXFADoc();
}

CXFA_Document* CXFA_FFWidgetHandler::GetXFADoc() const {
  return m_pDocView->GetDoc()->GetXFADoc();
}

