// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffnotify.h"

#include "fxjs/include/cfxjse_value.h"
#include "xfa/fxfa/app/xfa_ffbarcode.h"
#include "xfa/fxfa/app/xfa_ffcheckbutton.h"
#include "xfa/fxfa/app/xfa_ffchoicelist.h"
#include "xfa/fxfa/app/xfa_ffdraw.h"
#include "xfa/fxfa/app/xfa_ffexclgroup.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_ffimage.h"
#include "xfa/fxfa/app/xfa_ffimageedit.h"
#include "xfa/fxfa/app/xfa_ffpath.h"
#include "xfa/fxfa/app/xfa_ffpushbutton.h"
#include "xfa/fxfa/app/xfa_ffsignature.h"
#include "xfa/fxfa/app/xfa_ffsubform.h"
#include "xfa/fxfa/app/xfa_fftext.h"
#include "xfa/fxfa/app/xfa_fftextedit.h"
#include "xfa/fxfa/app/xfa_ffwidgetacc.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/app/xfa_textlayout.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"

static void XFA_FFDeleteWidgetAcc(void* pData) {
  delete static_cast<CXFA_WidgetAcc*>(pData);
}

static XFA_MAPDATABLOCKCALLBACKINFO gs_XFADeleteWidgetAcc = {
    XFA_FFDeleteWidgetAcc, nullptr};

CXFA_FFNotify::CXFA_FFNotify(CXFA_FFDoc* pDoc) : m_pDoc(pDoc) {}
CXFA_FFNotify::~CXFA_FFNotify() {}

void CXFA_FFNotify::OnPageEvent(CXFA_ContainerLayoutItem* pSender,
                                uint32_t dwEvent) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView(pSender->GetLayout());
  if (pDocView)
    pDocView->OnPageEvent(pSender, dwEvent);
}

void CXFA_FFNotify::OnWidgetListItemAdded(CXFA_WidgetData* pSender,
                                          const FX_WCHAR* pLabel,
                                          const FX_WCHAR* pValue,
                                          int32_t iIndex) {
  CXFA_WidgetAcc* pWidgetAcc = static_cast<CXFA_WidgetAcc*>(pSender);
  if (pWidgetAcc->GetUIType() != XFA_Element::ChoiceList)
    return;

  CXFA_FFWidget* pWidget = nullptr;
  while ((pWidget = pWidgetAcc->GetNextWidget(pWidget)) != nullptr) {
    if (pWidget->IsLoaded()) {
      if (pWidgetAcc->IsListBox()) {
        static_cast<CXFA_FFListBox*>(pWidget)->InsertItem(pLabel, iIndex);
      } else {
        static_cast<CXFA_FFComboBox*>(pWidget)->InsertItem(pLabel, iIndex);
      }
    }
  }
}

void CXFA_FFNotify::OnWidgetListItemRemoved(CXFA_WidgetData* pSender,
                                            int32_t iIndex) {
  CXFA_WidgetAcc* pWidgetAcc = static_cast<CXFA_WidgetAcc*>(pSender);
  if (pWidgetAcc->GetUIType() != XFA_Element::ChoiceList)
    return;

  CXFA_FFWidget* pWidget = nullptr;
  while ((pWidget = pWidgetAcc->GetNextWidget(pWidget)) != nullptr) {
    if (pWidget->IsLoaded()) {
      if (pWidgetAcc->IsListBox()) {
        static_cast<CXFA_FFListBox*>(pWidget)->DeleteItem(iIndex);
      } else {
        static_cast<CXFA_FFComboBox*>(pWidget)->DeleteItem(iIndex);
      }
    }
  }
}

CXFA_LayoutItem* CXFA_FFNotify::OnCreateLayoutItem(CXFA_Node* pNode) {
  CXFA_LayoutProcessor* pLayout = m_pDoc->GetXFADoc()->GetDocLayout();
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView(pLayout);
  XFA_Element eType = pNode->GetElementType();
  if (eType == XFA_Element::PageArea)
    return new CXFA_FFPageView(pDocView, pNode);
  if (eType == XFA_Element::ContentArea)
    return new CXFA_ContainerLayoutItem(pNode);

  CXFA_WidgetAcc* pAcc = static_cast<CXFA_WidgetAcc*>(pNode->GetWidgetData());
  if (!pAcc)
    return new CXFA_ContentLayoutItem(pNode);

  CXFA_FFWidget* pWidget;
  switch (pAcc->GetUIType()) {
    case XFA_Element::Barcode:
      pWidget = new CXFA_FFBarcode(nullptr, pAcc);
      break;
    case XFA_Element::Button:
      pWidget = new CXFA_FFPushButton(nullptr, pAcc);
      break;
    case XFA_Element::CheckButton:
      pWidget = new CXFA_FFCheckButton(nullptr, pAcc);
      break;
    case XFA_Element::ChoiceList: {
      if (pAcc->IsListBox()) {
        pWidget = new CXFA_FFListBox(nullptr, pAcc);
      } else {
        pWidget = new CXFA_FFComboBox(nullptr, pAcc);
      }
    } break;
    case XFA_Element::DateTimeEdit:
      pWidget = new CXFA_FFDateTimeEdit(nullptr, pAcc);
      break;
    case XFA_Element::ImageEdit:
      pWidget = new CXFA_FFImageEdit(nullptr, pAcc);
      break;
    case XFA_Element::NumericEdit:
      pWidget = new CXFA_FFNumericEdit(nullptr, pAcc);
      break;
    case XFA_Element::PasswordEdit:
      pWidget = new CXFA_FFPasswordEdit(nullptr, pAcc);
      break;
    case XFA_Element::Signature:
      pWidget = new CXFA_FFSignature(nullptr, pAcc);
      break;
    case XFA_Element::TextEdit:
      pWidget = new CXFA_FFTextEdit(nullptr, pAcc);
      break;
    case XFA_Element::Arc:
      pWidget = new CXFA_FFArc(nullptr, pAcc);
      break;
    case XFA_Element::Line:
      pWidget = new CXFA_FFLine(nullptr, pAcc);
      break;
    case XFA_Element::Rectangle:
      pWidget = new CXFA_FFRectangle(nullptr, pAcc);
      break;
    case XFA_Element::Text:
      pWidget = new CXFA_FFText(nullptr, pAcc);
      break;
    case XFA_Element::Image:
      pWidget = new CXFA_FFImage(nullptr, pAcc);
      break;
    case XFA_Element::Draw:
      pWidget = new CXFA_FFDraw(nullptr, pAcc);
      break;
    case XFA_Element::Subform:
      pWidget = new CXFA_FFSubForm(nullptr, pAcc);
      break;
    case XFA_Element::ExclGroup:
      pWidget = new CXFA_FFExclGroup(nullptr, pAcc);
      break;
    case XFA_Element::DefaultUi:
    default:
      pWidget = nullptr;
      break;
  }

  if (pWidget)
    pWidget->SetDocView(pDocView);
  return pWidget;
}

void CXFA_FFNotify::StartFieldDrawLayout(CXFA_Node* pItem,
                                         FX_FLOAT& fCalcWidth,
                                         FX_FLOAT& fCalcHeight) {
  CXFA_WidgetAcc* pAcc = static_cast<CXFA_WidgetAcc*>(pItem->GetWidgetData());
  if (!pAcc) {
    return;
  }
  pAcc->StartWidgetLayout(fCalcWidth, fCalcHeight);
}
FX_BOOL CXFA_FFNotify::FindSplitPos(CXFA_Node* pItem,
                                    int32_t iBlockIndex,
                                    FX_FLOAT& fCalcHeightPos) {
  CXFA_WidgetAcc* pAcc = static_cast<CXFA_WidgetAcc*>(pItem->GetWidgetData());
  if (!pAcc) {
    return FALSE;
  }
  return (XFA_LAYOUTRESULT)pAcc->FindSplitPos(iBlockIndex, fCalcHeightPos);
}
FX_BOOL CXFA_FFNotify::RunScript(CXFA_Node* pScript, CXFA_Node* pFormItem) {
  FX_BOOL bRet = FALSE;
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return bRet;
  }
  CXFA_WidgetAcc* pWidgetAcc =
      static_cast<CXFA_WidgetAcc*>(pFormItem->GetWidgetData());
  if (!pWidgetAcc) {
    return bRet;
  }
  CXFA_EventParam EventParam;
  EventParam.m_eType = XFA_EVENT_Unknown;
  CFXJSE_Value* pRetValue = nullptr;
  int32_t iRet =
      pWidgetAcc->ExecuteScript(CXFA_Script(pScript), &EventParam, &pRetValue);
  if (iRet == XFA_EVENTERROR_Success && pRetValue) {
    bRet = pRetValue->ToBoolean();
    delete pRetValue;
  }
  return bRet;
}
int32_t CXFA_FFNotify::ExecEventByDeepFirst(CXFA_Node* pFormNode,
                                            XFA_EVENTTYPE eEventType,
                                            FX_BOOL bIsFormReady,
                                            FX_BOOL bRecursive,
                                            CXFA_WidgetAcc* pExclude) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return XFA_EVENTERROR_NotExist;
  }
  return pDocView->ExecEventActivityByDeepFirst(
      pFormNode, eEventType, bIsFormReady, bRecursive,
      pExclude ? pExclude->GetNode() : nullptr);
}
void CXFA_FFNotify::AddCalcValidate(CXFA_Node* pNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  CXFA_WidgetAcc* pWidgetAcc =
      static_cast<CXFA_WidgetAcc*>(pNode->GetWidgetData());
  if (!pWidgetAcc) {
    return;
  }
  pDocView->AddCalculateWidgetAcc(pWidgetAcc);
  pDocView->AddValidateWidget(pWidgetAcc);
}
CXFA_FFDoc* CXFA_FFNotify::GetHDOC() {
  return m_pDoc;
}
IXFA_DocProvider* CXFA_FFNotify::GetDocProvider() {
  return m_pDoc->GetDocProvider();
}
IXFA_AppProvider* CXFA_FFNotify::GetAppProvider() {
  return m_pDoc->GetApp()->GetAppProvider();
}
CXFA_FFWidgetHandler* CXFA_FFNotify::GetWidgetHandler() {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  return pDocView ? pDocView->GetWidgetHandler() : nullptr;
}
CXFA_FFWidget* CXFA_FFNotify::GetHWidget(CXFA_LayoutItem* pLayoutItem) {
  return XFA_GetWidgetFromLayoutItem(pLayoutItem);
}
void CXFA_FFNotify::OpenDropDownList(CXFA_FFWidget* hWidget) {
  if (hWidget->GetDataAcc()->GetUIType() != XFA_Element::ChoiceList) {
    return;
  }
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  pDocView->LockUpdate();
  static_cast<CXFA_FFComboBox*>(hWidget)->OpenDropDownList();
  pDocView->UnlockUpdate();
  pDocView->UpdateDocView();
}
CFX_WideString CXFA_FFNotify::GetCurrentDateTime() {
  CFX_Unitime dataTime;
  dataTime.Now();
  CFX_WideString wsDateTime;
  wsDateTime.Format(L"%d%02d%02dT%02d%02d%02d", dataTime.GetYear(),
                    dataTime.GetMonth(), dataTime.GetDay(), dataTime.GetHour(),
                    dataTime.GetMinute(), dataTime.GetSecond());
  return wsDateTime;
}
void CXFA_FFNotify::ResetData(CXFA_WidgetData* pWidgetData) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  pDocView->ResetWidgetData(static_cast<CXFA_WidgetAcc*>(pWidgetData));
}
int32_t CXFA_FFNotify::GetLayoutStatus() {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  return pDocView ? pDocView->GetLayoutStatus() : 0;
}
void CXFA_FFNotify::RunNodeInitialize(CXFA_Node* pNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  pDocView->AddNewFormNode(pNode);
}
void CXFA_FFNotify::RunSubformIndexChange(CXFA_Node* pSubformNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  pDocView->AddIndexChangedSubform(pSubformNode);
}
CXFA_Node* CXFA_FFNotify::GetFocusWidgetNode() {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return nullptr;
  }
  CXFA_WidgetAcc* pAcc = pDocView->GetFocusWidgetAcc();
  return pAcc ? pAcc->GetNode() : nullptr;
}
void CXFA_FFNotify::SetFocusWidgetNode(CXFA_Node* pNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  CXFA_WidgetAcc* pAcc =
      pNode ? static_cast<CXFA_WidgetAcc*>(pNode->GetWidgetData()) : nullptr;
  pDocView->SetFocusWidgetAcc(pAcc);
}

void CXFA_FFNotify::OnNodeReady(CXFA_Node* pNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView)
    return;

  XFA_Element eType = pNode->GetElementType();
  if (XFA_IsCreateWidget(eType)) {
    CXFA_WidgetAcc* pAcc = new CXFA_WidgetAcc(pDocView, pNode);
    pNode->SetObject(XFA_ATTRIBUTE_WidgetData, pAcc, &gs_XFADeleteWidgetAcc);
    return;
  }
  switch (eType) {
    case XFA_Element::BindItems:
      pDocView->m_BindItems.Add(pNode);
      break;
    case XFA_Element::Validate: {
      pNode->SetFlag(XFA_NodeFlag_NeedsInitApp, false);
    } break;
    default:
      break;
  }
}

void CXFA_FFNotify::OnValueChanging(CXFA_Node* pSender, XFA_ATTRIBUTE eAttr) {
  if (eAttr != XFA_ATTRIBUTE_Presence)
    return;

  if (pSender->GetPacketID() & XFA_XDPPACKET_Datasets)
    return;

  if (!pSender->IsFormContainer())
    return;

  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView)
    return;

  if (pDocView->GetLayoutStatus() < XFA_DOCVIEW_LAYOUTSTATUS_End)
    return;

  CXFA_WidgetAcc* pWidgetAcc =
      static_cast<CXFA_WidgetAcc*>(pSender->GetWidgetData());
  if (!pWidgetAcc)
    return;

  CXFA_FFWidget* pWidget = nullptr;
  while ((pWidget = pWidgetAcc->GetNextWidget(pWidget)) != nullptr) {
    if (pWidget->IsLoaded())
      pWidget->AddInvalidateRect();
  }
}

void CXFA_FFNotify::OnValueChanged(CXFA_Node* pSender,
                                   XFA_ATTRIBUTE eAttr,
                                   CXFA_Node* pParentNode,
                                   CXFA_Node* pWidgetNode) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView)
    return;

  if (!(pSender->GetPacketID() & XFA_XDPPACKET_Form)) {
    if (eAttr == XFA_ATTRIBUTE_Value)
      pDocView->AddCalculateNodeNotify(pSender);
    return;
  }

  XFA_Element eType = pParentNode->GetElementType();
  FX_BOOL bIsContainerNode = pParentNode->IsContainerNode();
  CXFA_WidgetAcc* pWidgetAcc =
      static_cast<CXFA_WidgetAcc*>(pWidgetNode->GetWidgetData());
  if (!pWidgetAcc)
    return;

  bool bUpdateProperty = false;
  pDocView->SetChangeMark();
  switch (eType) {
    case XFA_Element::Caption: {
      CXFA_TextLayout* pCapOut = pWidgetAcc->GetCaptionTextLayout();
      if (!pCapOut)
        return;

      pCapOut->Unload();
    } break;
    case XFA_Element::Ui:
    case XFA_Element::Para:
      bUpdateProperty = true;
      break;
    default:
      break;
  }
  if (bIsContainerNode && eAttr == XFA_ATTRIBUTE_Access)
    bUpdateProperty = true;

  if (eAttr == XFA_ATTRIBUTE_Value) {
    pDocView->AddCalculateNodeNotify(pSender);
    if (eType == XFA_Element::Value || bIsContainerNode) {
      if (bIsContainerNode) {
        pWidgetAcc->UpdateUIDisplay();
        pDocView->AddCalculateWidgetAcc(pWidgetAcc);
        pDocView->AddValidateWidget(pWidgetAcc);
      } else if (pWidgetNode->GetNodeItem(XFA_NODEITEM_Parent)
                     ->GetElementType() == XFA_Element::ExclGroup) {
        pWidgetAcc->UpdateUIDisplay();
      }
      return;
    }
  }
  CXFA_FFWidget* pWidget = nullptr;
  while ((pWidget = pWidgetAcc->GetNextWidget(pWidget)) != nullptr) {
    if (!pWidget->IsLoaded())
      continue;

    if (bUpdateProperty)
      pWidget->UpdateWidgetProperty();
    pWidget->PerformLayout();
    pWidget->AddInvalidateRect();
  }
}

void CXFA_FFNotify::OnChildAdded(CXFA_Node* pSender) {
  if (!pSender->IsFormContainer()) {
    return;
  }
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView) {
    return;
  }
  FX_BOOL bLayoutReady =
      !(pDocView->m_bInLayoutStatus) &&
      (pDocView->GetLayoutStatus() == XFA_DOCVIEW_LAYOUTSTATUS_End);
  if (bLayoutReady)
    m_pDoc->GetDocProvider()->SetChangeMark(m_pDoc);
}

void CXFA_FFNotify::OnChildRemoved() {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView();
  if (!pDocView)
    return;

  bool bLayoutReady =
      !(pDocView->m_bInLayoutStatus) &&
      (pDocView->GetLayoutStatus() == XFA_DOCVIEW_LAYOUTSTATUS_End);
  if (bLayoutReady)
    m_pDoc->GetDocProvider()->SetChangeMark(m_pDoc);
}

void CXFA_FFNotify::OnLayoutItemAdded(CXFA_LayoutProcessor* pLayout,
                                      CXFA_LayoutItem* pSender,
                                      int32_t iPageIdx,
                                      uint32_t dwStatus) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView(pLayout);
  if (!pDocView)
    return;

  CXFA_FFWidget* pWidget = XFA_GetWidgetFromLayoutItem(pSender);
  if (!pWidget)
    return;

  CXFA_FFPageView* pNewPageView = pDocView->GetPageView(iPageIdx);
  uint32_t dwFilter = XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable |
                      XFA_WidgetStatus_Printable;
  pWidget->ModifyStatus(dwStatus, dwFilter);
  CXFA_FFPageView* pPrePageView = pWidget->GetPageView();
  if (pPrePageView != pNewPageView ||
      (dwStatus & (XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable)) ==
          (XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable)) {
    pWidget->SetPageView(pNewPageView);
    m_pDoc->GetDocProvider()->WidgetPostAdd(pWidget, pWidget->GetDataAcc());
  }
  if (pDocView->GetLayoutStatus() != XFA_DOCVIEW_LAYOUTSTATUS_End ||
      !(dwStatus & XFA_WidgetStatus_Visible)) {
    return;
  }
  if (pWidget->IsLoaded()) {
    CFX_RectF rtOld;
    pWidget->GetWidgetRect(rtOld);
    if (rtOld != pWidget->ReCacheWidgetRect())
      pWidget->PerformLayout();
  } else {
    pWidget->LoadWidget();
  }
  pWidget->AddInvalidateRect(nullptr);
}

void CXFA_FFNotify::OnLayoutItemRemoving(CXFA_LayoutProcessor* pLayout,
                                         CXFA_LayoutItem* pSender) {
  CXFA_FFDocView* pDocView = m_pDoc->GetDocView(pLayout);
  if (!pDocView)
    return;

  CXFA_FFWidget* pWidget = XFA_GetWidgetFromLayoutItem(pSender);
  if (!pWidget)
    return;

  pDocView->DeleteLayoutItem(pWidget);
  m_pDoc->GetDocProvider()->WidgetPreRemove(pWidget, pWidget->GetDataAcc());
  pWidget->AddInvalidateRect(nullptr);
}
