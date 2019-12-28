// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/xfa_ffdocview.h"

#include "core/fxcrt/include/fx_ext.h"
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
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"
#include "xfa/fxfa/parser/cxfa_binditems.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"

extern const XFA_ATTRIBUTEENUM gs_EventActivity[] = {
    XFA_ATTRIBUTEENUM_Click,      XFA_ATTRIBUTEENUM_Change,
    XFA_ATTRIBUTEENUM_DocClose,   XFA_ATTRIBUTEENUM_DocReady,
    XFA_ATTRIBUTEENUM_Enter,      XFA_ATTRIBUTEENUM_Exit,
    XFA_ATTRIBUTEENUM_Full,       XFA_ATTRIBUTEENUM_IndexChange,
    XFA_ATTRIBUTEENUM_Initialize, XFA_ATTRIBUTEENUM_MouseDown,
    XFA_ATTRIBUTEENUM_MouseEnter, XFA_ATTRIBUTEENUM_MouseExit,
    XFA_ATTRIBUTEENUM_MouseUp,    XFA_ATTRIBUTEENUM_PostExecute,
    XFA_ATTRIBUTEENUM_PostOpen,   XFA_ATTRIBUTEENUM_PostPrint,
    XFA_ATTRIBUTEENUM_PostSave,   XFA_ATTRIBUTEENUM_PostSign,
    XFA_ATTRIBUTEENUM_PostSubmit, XFA_ATTRIBUTEENUM_PreExecute,
    XFA_ATTRIBUTEENUM_PreOpen,    XFA_ATTRIBUTEENUM_PrePrint,
    XFA_ATTRIBUTEENUM_PreSave,    XFA_ATTRIBUTEENUM_PreSign,
    XFA_ATTRIBUTEENUM_PreSubmit,  XFA_ATTRIBUTEENUM_Ready,
    XFA_ATTRIBUTEENUM_Unknown,
};
CXFA_FFDocView::CXFA_FFDocView(CXFA_FFDoc* pDoc)
    : m_bLayoutEvent(FALSE),
      m_pListFocusWidget(nullptr),
      m_bInLayoutStatus(FALSE),
      m_pDoc(pDoc),
      m_pWidgetHandler(nullptr),
      m_pXFADocLayout(nullptr),
      m_pFocusAcc(nullptr),
      m_pFocusWidget(nullptr),
      m_pOldFocusWidget(nullptr),
      m_iStatus(XFA_DOCVIEW_LAYOUTSTATUS_None),
      m_iLock(0) {}

CXFA_FFDocView::~CXFA_FFDocView() {
  DestroyDocView();
  delete m_pWidgetHandler;
}

void CXFA_FFDocView::InitLayout(CXFA_Node* pNode) {
  RunBindItems();
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Initialize);
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_IndexChange);
}
int32_t CXFA_FFDocView::StartLayout(int32_t iStartPage) {
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Start;
  m_pDoc->GetXFADoc()->DoProtoMerge();
  m_pDoc->GetXFADoc()->DoDataMerge();
  m_pXFADocLayout = GetXFALayout();
  int32_t iStatus = m_pXFADocLayout->StartLayout();
  if (iStatus < 0) {
    return iStatus;
  }
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem) {
    return iStatus;
  }
  InitLayout(pRootItem);
  InitCalculate(pRootItem);
  InitValidate(pRootItem);
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready, TRUE);
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Start;
  return iStatus;
}
int32_t CXFA_FFDocView::DoLayout(IFX_Pause* pPause) {
  int32_t iStatus = 100;
  iStatus = m_pXFADocLayout->DoLayout(pPause);
  if (iStatus != 100) {
    return iStatus;
  }
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_Doing;
  return iStatus;
}
void CXFA_FFDocView::StopLayout() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem) {
    return;
  }
  CXFA_Node* pSubformNode = pRootItem->GetChild(0, XFA_Element::Subform);
  if (!pSubformNode) {
    return;
  }
  CXFA_Node* pPageSetNode =
      pSubformNode->GetFirstChildByClass(XFA_Element::PageSet);
  if (!pPageSetNode) {
    return;
  }
  RunCalculateWidgets();
  RunValidate();
  InitLayout(pPageSetNode);
  InitCalculate(pPageSetNode);
  InitValidate(pPageSetNode);
  ExecEventActivityByDeepFirst(pPageSetNode, XFA_EVENT_Ready, TRUE);
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready);
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_DocReady);
  RunCalculateWidgets();
  RunValidate();
  if (RunLayout()) {
    ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready);
  }
  m_CalculateAccs.RemoveAll();
  if (m_pFocusAcc && !m_pFocusWidget) {
    SetFocusWidgetAcc(m_pFocusAcc);
  }
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_End;
}
int32_t CXFA_FFDocView::GetLayoutStatus() {
  return m_iStatus;
}
void CXFA_FFDocView::ShowNullTestMsg() {
  int32_t iCount = m_arrNullTestMsg.GetSize();
  CXFA_FFApp* pApp = m_pDoc->GetApp();
  IXFA_AppProvider* pAppProvider = pApp->GetAppProvider();
  if (pAppProvider && iCount) {
    int32_t iRemain = iCount > 7 ? iCount - 7 : 0;
    iCount -= iRemain;
    CFX_WideString wsMsg;
    for (int32_t i = 0; i < iCount; i++) {
      wsMsg += m_arrNullTestMsg[i] + FX_WSTRC(L"\n");
    }
    if (iRemain > 0) {
      CFX_WideString wsLimit;
      pAppProvider->LoadString(XFA_IDS_ValidateLimit, wsLimit);
      if (!wsLimit.IsEmpty()) {
        CFX_WideString wsTemp;
        wsTemp.Format(wsLimit.c_str(), iRemain);
        wsMsg += FX_WSTRC(L"\n") + wsTemp;
      }
    }
    CFX_WideString wsTitle;
    pAppProvider->LoadString(XFA_IDS_AppName, wsTitle);
    pAppProvider->MsgBox(wsMsg, wsTitle, XFA_MBICON_Status, XFA_MB_OK);
  }
  m_arrNullTestMsg.RemoveAll();
}
void CXFA_FFDocView::UpdateDocView() {
  if (IsUpdateLocked()) {
    return;
  }
  LockUpdate();
  int32_t iNewAdds = m_NewAddedNodes.GetSize();
  for (int32_t i = 0; i < iNewAdds; i++) {
    CXFA_Node* pNode = m_NewAddedNodes[i];
    InitCalculate(pNode);
    InitValidate(pNode);
    ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Ready, TRUE);
  }
  m_NewAddedNodes.RemoveAll();
  RunSubformIndexChange();
  RunCalculateWidgets();
  RunValidate();
  ShowNullTestMsg();
  if (RunLayout() && m_bLayoutEvent) {
    RunEventLayoutReady();
  }
  m_bLayoutEvent = FALSE;
  m_CalculateAccs.RemoveAll();
  RunInvalidate();
  UnlockUpdate();
}
int32_t CXFA_FFDocView::CountPageViews() {
  if (!m_pXFADocLayout) {
    return 0;
  }
  return m_pXFADocLayout->CountPages();
}
CXFA_FFPageView* CXFA_FFDocView::GetPageView(int32_t nIndex) {
  if (!m_pXFADocLayout) {
    return nullptr;
  }
  return static_cast<CXFA_FFPageView*>(m_pXFADocLayout->GetPage(nIndex));
}

CXFA_LayoutProcessor* CXFA_FFDocView::GetXFALayout() const {
  return m_pDoc->GetXFADoc()->GetDocLayout();
}
FX_BOOL CXFA_FFDocView::ResetSingleWidgetAccData(CXFA_WidgetAcc* pWidgetAcc) {
  CXFA_Node* pNode = pWidgetAcc->GetNode();
  XFA_Element eType = pNode->GetElementType();
  if (eType != XFA_Element::Field && eType != XFA_Element::ExclGroup) {
    return FALSE;
  }
  pWidgetAcc->ResetData();
  pWidgetAcc->UpdateUIDisplay();
  if (CXFA_Validate validate = pWidgetAcc->GetValidate()) {
    AddValidateWidget(pWidgetAcc);
    validate.GetNode()->SetFlag(XFA_NodeFlag_NeedsInitApp, false);
  }
  return TRUE;
}
void CXFA_FFDocView::ResetWidgetData(CXFA_WidgetAcc* pWidgetAcc) {
  m_bLayoutEvent = TRUE;
  FX_BOOL bChanged = FALSE;
  CXFA_Node* pFormNode = nullptr;
  if (pWidgetAcc) {
    bChanged = ResetSingleWidgetAccData(pWidgetAcc);
    pFormNode = pWidgetAcc->GetNode();
  } else {
    pFormNode = GetRootSubform();
  }
  if (!pFormNode) {
    return;
  }
  if (pFormNode->GetElementType() != XFA_Element::Field &&
      pFormNode->GetElementType() != XFA_Element::ExclGroup) {
    CXFA_WidgetAccIterator Iterator(this, pFormNode);
    while (CXFA_WidgetAcc* pAcc = Iterator.MoveToNext()) {
      bChanged |= ResetSingleWidgetAccData(pAcc);
      if (pAcc->GetNode()->GetElementType() == XFA_Element::ExclGroup) {
        Iterator.SkipTree();
      }
    }
  }
  if (bChanged) {
    m_pDoc->GetDocProvider()->SetChangeMark(m_pDoc);
  }
}
int32_t CXFA_FFDocView::ProcessWidgetEvent(CXFA_EventParam* pParam,
                                           CXFA_WidgetAcc* pWidgetAcc) {
  if (!pParam)
    return XFA_EVENTERROR_Error;

  if (pParam->m_eType == XFA_EVENT_Validate) {
    CFX_WideString wsValidateStr(L"preSubmit");
    CXFA_Node* pConfigItem =
        ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Config));
    if (pConfigItem) {
      CXFA_Node* pValidateNode = nullptr;
      CXFA_Node* pAcrobatNode = pConfigItem->GetChild(0, XFA_Element::Acrobat);
      pValidateNode = pAcrobatNode
                          ? pAcrobatNode->GetChild(0, XFA_Element::Validate)
                          : nullptr;
      if (!pValidateNode) {
        CXFA_Node* pPresentNode =
            pConfigItem->GetChild(0, XFA_Element::Present);
        pValidateNode = pPresentNode
                            ? pPresentNode->GetChild(0, XFA_Element::Validate)
                            : nullptr;
      }
      if (pValidateNode) {
        wsValidateStr = pValidateNode->GetContent();
      }
    }
    FX_BOOL bValidate = FALSE;
    switch (pParam->m_iValidateActivities) {
      case XFA_VALIDATE_preSubmit:
        bValidate = wsValidateStr.Find(L"preSubmit") != -1;
        break;
      case XFA_VALIDATE_prePrint:
        bValidate = wsValidateStr.Find(L"prePrint") != -1;
        break;
      case XFA_VALIDATE_preExecute:
        bValidate = wsValidateStr.Find(L"preExecute") != -1;
        break;
      case XFA_VALIDATE_preSave:
        bValidate = wsValidateStr.Find(L"preSave") != -1;
        break;
    }
    if (!bValidate) {
      return XFA_EVENTERROR_Success;
    }
  }
  CXFA_Node* pNode = pWidgetAcc ? pWidgetAcc->GetNode() : nullptr;
  if (!pNode) {
    CXFA_Node* pRootItem =
        ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
    if (!pRootItem) {
      return XFA_EVENTERROR_Error;
    }
    pNode = pRootItem->GetChild(0, XFA_Element::Subform);
  }
  ExecEventActivityByDeepFirst(pNode, pParam->m_eType, pParam->m_bIsFormReady);
  return XFA_EVENTERROR_Success;
}
CXFA_FFWidgetHandler* CXFA_FFDocView::GetWidgetHandler() {
  if (!m_pWidgetHandler) {
    m_pWidgetHandler = new CXFA_FFWidgetHandler(this);
  }
  return m_pWidgetHandler;
}

CXFA_WidgetAccIterator* CXFA_FFDocView::CreateWidgetAccIterator(
    XFA_WIDGETORDER eOrder) {
  CXFA_Node* pFormRoot = GetRootSubform();
  return pFormRoot ? new CXFA_WidgetAccIterator(this, pFormRoot) : nullptr;
}
CXFA_FFWidget* CXFA_FFDocView::GetFocusWidget() {
  return m_pFocusWidget;
}
void CXFA_FFDocView::KillFocus() {
  if (m_pFocusWidget &&
      (m_pFocusWidget->GetStatus() & XFA_WidgetStatus_Focused)) {
    (m_pFocusWidget)->OnKillFocus(nullptr);
  }
  m_pFocusAcc = nullptr;
  m_pFocusWidget = nullptr;
  m_pOldFocusWidget = nullptr;
}
FX_BOOL CXFA_FFDocView::SetFocus(CXFA_FFWidget* hWidget) {
  CXFA_FFWidget* pNewFocus = hWidget;
  if (m_pOldFocusWidget == pNewFocus) {
    return FALSE;
  }
  CXFA_FFWidget* pOldFocus = m_pOldFocusWidget;
  m_pOldFocusWidget = pNewFocus;
  if (pOldFocus) {
    if (m_pFocusWidget != m_pOldFocusWidget &&
        (pOldFocus->GetStatus() & XFA_WidgetStatus_Focused)) {
      m_pFocusWidget = pOldFocus;
      pOldFocus->OnKillFocus(pNewFocus);
    } else if ((pOldFocus->GetStatus() & XFA_WidgetStatus_Visible)) {
      if (!pOldFocus->IsLoaded()) {
        pOldFocus->LoadWidget();
      }
      pOldFocus->OnSetFocus(m_pFocusWidget);
      m_pFocusWidget = pOldFocus;
      pOldFocus->OnKillFocus(pNewFocus);
    }
  }
  if (m_pFocusWidget == m_pOldFocusWidget) {
    return FALSE;
  }
  pNewFocus = m_pOldFocusWidget;
  if (m_pListFocusWidget && pNewFocus == m_pListFocusWidget) {
    m_pFocusAcc = nullptr;
    m_pFocusWidget = nullptr;
    m_pListFocusWidget = nullptr;
    m_pOldFocusWidget = nullptr;
    return FALSE;
  }
  if (pNewFocus && (pNewFocus->GetStatus() & XFA_WidgetStatus_Visible)) {
    if (!pNewFocus->IsLoaded()) {
      pNewFocus->LoadWidget();
    }
    pNewFocus->OnSetFocus(m_pFocusWidget);
  }
  m_pFocusAcc = pNewFocus ? pNewFocus->GetDataAcc() : nullptr;
  m_pFocusWidget = pNewFocus;
  m_pOldFocusWidget = m_pFocusWidget;
  return TRUE;
}
CXFA_WidgetAcc* CXFA_FFDocView::GetFocusWidgetAcc() {
  return m_pFocusAcc;
}
void CXFA_FFDocView::SetFocusWidgetAcc(CXFA_WidgetAcc* pWidgetAcc) {
  CXFA_FFWidget* pNewFocus =
      pWidgetAcc ? pWidgetAcc->GetNextWidget(nullptr) : nullptr;
  if (SetFocus(pNewFocus)) {
    m_pFocusAcc = pWidgetAcc;
    if (m_iStatus == XFA_DOCVIEW_LAYOUTSTATUS_End) {
      m_pDoc->GetDocProvider()->SetFocusWidget(m_pDoc, m_pFocusWidget);
    }
  }
}
void CXFA_FFDocView::DeleteLayoutItem(CXFA_FFWidget* pWidget) {
  if (m_pFocusAcc == pWidget->GetDataAcc()) {
    m_pFocusAcc = nullptr;
    m_pFocusWidget = nullptr;
    m_pOldFocusWidget = nullptr;
  }
}
static int32_t XFA_ProcessEvent(CXFA_FFDocView* pDocView,
                                CXFA_WidgetAcc* pWidgetAcc,
                                CXFA_EventParam* pParam) {
  if (!pParam || pParam->m_eType == XFA_EVENT_Unknown) {
    return XFA_EVENTERROR_NotExist;
  }
  if (!pWidgetAcc || pWidgetAcc->GetElementType() == XFA_Element::Draw) {
    return XFA_EVENTERROR_NotExist;
  }
  switch (pParam->m_eType) {
    case XFA_EVENT_Calculate:
      return pWidgetAcc->ProcessCalculate();
    case XFA_EVENT_Validate:
      if (((CXFA_FFDoc*)pDocView->GetDoc())
              ->GetDocProvider()
              ->IsValidationsEnabled(pDocView->GetDoc())) {
        return pWidgetAcc->ProcessValidate(0x01);
      }
      return XFA_EVENTERROR_Disabled;
    case XFA_EVENT_InitCalculate: {
      CXFA_Calculate calc = pWidgetAcc->GetCalculate();
      if (!calc) {
        return XFA_EVENTERROR_NotExist;
      }
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
int32_t CXFA_FFDocView::ExecEventActivityByDeepFirst(CXFA_Node* pFormNode,
                                                     XFA_EVENTTYPE eEventType,
                                                     FX_BOOL bIsFormReady,
                                                     FX_BOOL bRecursive,
                                                     CXFA_Node* pExclude) {
  int32_t iRet = XFA_EVENTERROR_NotExist;
  if (pFormNode == pExclude) {
    return iRet;
  }
  XFA_Element elementType = pFormNode->GetElementType();
  if (elementType == XFA_Element::Field) {
    if (eEventType == XFA_EVENT_IndexChange) {
      return iRet;
    }
    CXFA_WidgetAcc* pWidgetAcc = (CXFA_WidgetAcc*)pFormNode->GetWidgetData();
    if (!pWidgetAcc) {
      return iRet;
    }
    CXFA_EventParam eParam;
    eParam.m_eType = eEventType;
    eParam.m_pTarget = pWidgetAcc;
    eParam.m_bIsFormReady = bIsFormReady;
    return XFA_ProcessEvent(this, pWidgetAcc, &eParam);
  }
  if (bRecursive) {
    for (CXFA_Node* pNode = pFormNode->GetNodeItem(
             XFA_NODEITEM_FirstChild, XFA_ObjectType::ContainerNode);
         pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling,
                                           XFA_ObjectType::ContainerNode)) {
      elementType = pNode->GetElementType();
      if (elementType != XFA_Element::Variables &&
          elementType != XFA_Element::Draw) {
        iRet |= ExecEventActivityByDeepFirst(pNode, eEventType, bIsFormReady,
                                             bRecursive, pExclude);
      }
    }
  }
  CXFA_WidgetAcc* pWidgetAcc = (CXFA_WidgetAcc*)pFormNode->GetWidgetData();
  if (!pWidgetAcc) {
    return iRet;
  }
  CXFA_EventParam eParam;
  eParam.m_eType = eEventType;
  eParam.m_pTarget = pWidgetAcc;
  eParam.m_bIsFormReady = bIsFormReady;
  iRet |= XFA_ProcessEvent(this, pWidgetAcc, &eParam);
  return iRet;
}

CXFA_FFWidget* CXFA_FFDocView::GetWidgetByName(const CFX_WideString& wsName,
                                               CXFA_FFWidget* pRefWidget) {
  CXFA_WidgetAcc* pRefAcc = pRefWidget ? pRefWidget->GetDataAcc() : nullptr;
  CXFA_WidgetAcc* pAcc = GetWidgetAccByName(wsName, pRefAcc);
  return pAcc ? pAcc->GetNextWidget(nullptr) : nullptr;
}

CXFA_WidgetAcc* CXFA_FFDocView::GetWidgetAccByName(
    const CFX_WideString& wsName,
    CXFA_WidgetAcc* pRefWidgetAcc) {
  CFX_WideString wsExpression;
  uint32_t dwStyle = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
                     XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent;
  CXFA_ScriptContext* pScriptContext = m_pDoc->GetXFADoc()->GetScriptContext();
  if (!pScriptContext) {
    return nullptr;
  }
  CXFA_Node* refNode = nullptr;
  if (pRefWidgetAcc) {
    refNode = pRefWidgetAcc->GetNode();
    wsExpression = wsName;
  } else {
    wsExpression = L"$form." + wsName;
  }
  XFA_RESOLVENODE_RS resoveNodeRS;
  int32_t iRet = pScriptContext->ResolveObjects(
      refNode, wsExpression.AsStringC(), resoveNodeRS, dwStyle);
  if (iRet < 1) {
    return nullptr;
  }
  if (resoveNodeRS.dwFlags == XFA_RESOVENODE_RSTYPE_Nodes) {
    CXFA_Node* pNode = resoveNodeRS.nodes[0]->AsNode();
    if (pNode) {
      return (CXFA_WidgetAcc*)pNode->GetWidgetData();
    }
  }
  return nullptr;
}

void CXFA_FFDocView::OnPageEvent(CXFA_ContainerLayoutItem* pSender,
                                 uint32_t dwEvent) {
  CXFA_FFPageView* pFFPageView = static_cast<CXFA_FFPageView*>(pSender);
  m_pDoc->GetDocProvider()->PageViewEvent(pFFPageView, dwEvent);
}

void CXFA_FFDocView::LockUpdate() {
  m_iLock++;
}
void CXFA_FFDocView::UnlockUpdate() {
  m_iLock--;
}
FX_BOOL CXFA_FFDocView::IsUpdateLocked() {
  return m_iLock;
}
void CXFA_FFDocView::ClearInvalidateList() {
  m_mapPageInvalidate.clear();
}
void CXFA_FFDocView::AddInvalidateRect(CXFA_FFWidget* pWidget,
                                       const CFX_RectF& rtInvalidate) {
  AddInvalidateRect(pWidget->GetPageView(), rtInvalidate);
}

void CXFA_FFDocView::AddInvalidateRect(CXFA_FFPageView* pPageView,
                                       const CFX_RectF& rtInvalidate) {
  if (m_mapPageInvalidate[pPageView]) {
    m_mapPageInvalidate[pPageView]->Union(rtInvalidate);
    return;
  }
  CFX_RectF* pRect = new CFX_RectF;
  pRect->Set(rtInvalidate.left, rtInvalidate.top, rtInvalidate.width,
             rtInvalidate.height);
  m_mapPageInvalidate[pPageView].reset(pRect);
}

void CXFA_FFDocView::RunInvalidate() {
  for (const auto& pair : m_mapPageInvalidate)
    m_pDoc->GetDocProvider()->InvalidateRect(pair.first, *pair.second);
  m_mapPageInvalidate.clear();
}

FX_BOOL CXFA_FFDocView::RunLayout() {
  LockUpdate();
  m_bInLayoutStatus = TRUE;
  if (!m_pXFADocLayout->IncrementLayout() &&
      m_pXFADocLayout->StartLayout() < 100) {
    m_pXFADocLayout->DoLayout();
    UnlockUpdate();
    m_bInLayoutStatus = FALSE;
    m_pDoc->GetDocProvider()->PageViewEvent(nullptr,
                                            XFA_PAGEVIEWEVENT_StopLayout);
    return TRUE;
  }
  m_bInLayoutStatus = FALSE;
  m_pDoc->GetDocProvider()->PageViewEvent(nullptr,
                                          XFA_PAGEVIEWEVENT_StopLayout);
  UnlockUpdate();
  return FALSE;
}
void CXFA_FFDocView::RunSubformIndexChange() {
  int32_t iSubforms = m_IndexChangedSubforms.GetSize();
  for (int32_t i = 0; i < iSubforms; i++) {
    CXFA_Node* pSubformNode = m_IndexChangedSubforms[i];
    CXFA_WidgetAcc* pWidgetAcc =
        static_cast<CXFA_WidgetAcc*>(pSubformNode->GetWidgetData());
    if (!pWidgetAcc) {
      continue;
    }
    CXFA_EventParam eParam;
    eParam.m_eType = XFA_EVENT_IndexChange;
    eParam.m_pTarget = pWidgetAcc;
    pWidgetAcc->ProcessEvent(XFA_ATTRIBUTEENUM_IndexChange, &eParam);
  }
  m_IndexChangedSubforms.RemoveAll();
}
void CXFA_FFDocView::AddNewFormNode(CXFA_Node* pNode) {
  m_NewAddedNodes.Add(pNode);
  InitLayout(pNode);
}
void CXFA_FFDocView::AddIndexChangedSubform(CXFA_Node* pNode) {
  ASSERT(pNode->GetElementType() == XFA_Element::Subform);
  m_IndexChangedSubforms.Add(pNode);
}
void CXFA_FFDocView::RunDocClose() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem) {
    return;
  }
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_DocClose);
}
void CXFA_FFDocView::DestroyDocView() {
  ClearInvalidateList();
  m_iStatus = XFA_DOCVIEW_LAYOUTSTATUS_None;
  m_iLock = 0;
  m_ValidateAccs.RemoveAll();
  m_BindItems.RemoveAll();
  m_CalculateAccs.RemoveAll();
}
FX_BOOL CXFA_FFDocView::IsStaticNotify() {
  return m_pDoc->GetDocType() == XFA_DOCTYPE_Static;
}
void CXFA_FFDocView::AddCalculateWidgetAcc(CXFA_WidgetAcc* pWidgetAcc) {
  int32_t iAccs = m_CalculateAccs.GetSize();
  CXFA_WidgetAcc* pCurrentAcc =
      iAccs > 0 ? m_CalculateAccs[iAccs - 1] : nullptr;
  if (pCurrentAcc != pWidgetAcc) {
    m_CalculateAccs.Add(pWidgetAcc);
  }
}
void CXFA_FFDocView::AddCalculateNodeNotify(CXFA_Node* pNodeChange) {
  CXFA_CalcData* pGlobalData =
      (CXFA_CalcData*)pNodeChange->GetUserData(XFA_CalcData);
  int32_t iCount = pGlobalData ? pGlobalData->m_Globals.GetSize() : 0;
  for (int32_t i = 0; i < iCount; i++) {
    CXFA_WidgetAcc* pResultAcc = pGlobalData->m_Globals[i];
    if (pResultAcc->GetNode()->HasRemovedChildren())
      continue;

    int32_t iAccs = m_CalculateAccs.GetSize();
    CXFA_WidgetAcc* pCurrentAcc =
        iAccs > 0 ? m_CalculateAccs[iAccs - 1] : nullptr;
    if (pCurrentAcc != pResultAcc) {
      m_CalculateAccs.Add(pResultAcc);
    }
  }
}
void CXFA_FFDocView::RunCalculateRecursive(int32_t& iIndex) {
  while (iIndex < m_CalculateAccs.GetSize()) {
    CXFA_WidgetAcc* pCurAcc = m_CalculateAccs[iIndex];
    AddCalculateNodeNotify(pCurAcc->GetNode());
    int32_t iRefCount =
        (int32_t)(uintptr_t)pCurAcc->GetNode()->GetUserData(XFA_CalcRefCount);
    iRefCount++;
    pCurAcc->GetNode()->SetUserData(XFA_CalcRefCount,
                                    (void*)(uintptr_t)iRefCount);
    if (iRefCount > 11) {
      break;
    }
    if ((pCurAcc->ProcessCalculate()) == XFA_EVENTERROR_Success) {
      AddValidateWidget(pCurAcc);
    }
    iIndex++;
    RunCalculateRecursive(iIndex);
  }
}
int32_t CXFA_FFDocView::RunCalculateWidgets() {
  if (!m_pDoc->GetDocProvider()->IsCalculationsEnabled(m_pDoc)) {
    return XFA_EVENTERROR_Disabled;
  }
  int32_t iCounts = m_CalculateAccs.GetSize();
  int32_t iIndex = 0;
  if (iCounts > 0) {
    RunCalculateRecursive(iIndex);
  }
  for (int32_t i = 0; i < m_CalculateAccs.GetSize(); i++) {
    CXFA_WidgetAcc* pCurAcc = m_CalculateAccs[i];
    pCurAcc->GetNode()->SetUserData(XFA_CalcRefCount, (void*)(uintptr_t)0);
  }
  m_CalculateAccs.RemoveAll();
  return XFA_EVENTERROR_Success;
}
void CXFA_FFDocView::AddValidateWidget(CXFA_WidgetAcc* pWidget) {
  if (m_ValidateAccs.Find(pWidget) < 0)
    m_ValidateAccs.Add(pWidget);
}
FX_BOOL CXFA_FFDocView::InitCalculate(CXFA_Node* pNode) {
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_InitCalculate);
  return TRUE;
}
FX_BOOL CXFA_FFDocView::InitValidate(CXFA_Node* pNode) {
  if (!m_pDoc->GetDocProvider()->IsValidationsEnabled(m_pDoc)) {
    return FALSE;
  }
  ExecEventActivityByDeepFirst(pNode, XFA_EVENT_Validate);
  m_ValidateAccs.RemoveAll();
  return TRUE;
}
FX_BOOL CXFA_FFDocView::RunValidate() {
  if (!m_pDoc->GetDocProvider()->IsValidationsEnabled(m_pDoc)) {
    return FALSE;
  }
  int32_t iCounts = m_ValidateAccs.GetSize();
  for (int32_t i = 0; i < iCounts; i++) {
    CXFA_WidgetAcc* pAcc = m_ValidateAccs[i];
    if (pAcc->GetNode()->HasRemovedChildren())
      continue;

    pAcc->ProcessValidate();
  }
  m_ValidateAccs.RemoveAll();
  return TRUE;
}
FX_BOOL CXFA_FFDocView::RunEventLayoutReady() {
  CXFA_Node* pRootItem =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pRootItem) {
    return FALSE;
  }
  ExecEventActivityByDeepFirst(pRootItem, XFA_EVENT_Ready);
  RunLayout();
  return TRUE;
}
void CXFA_FFDocView::RunBindItems() {
  for (int32_t i = 0; i < m_BindItems.GetSize(); i++) {
    if (m_BindItems[i]->HasRemovedChildren())
      continue;

    CXFA_Node* pWidgetNode = m_BindItems[i]->GetNodeItem(XFA_NODEITEM_Parent);
    CXFA_WidgetAcc* pAcc =
        static_cast<CXFA_WidgetAcc*>(pWidgetNode->GetWidgetData());
    if (!pAcc)
      continue;

    CXFA_BindItems binditems(m_BindItems[i]);
    CXFA_ScriptContext* pScriptContext =
        pWidgetNode->GetDocument()->GetScriptContext();
    CFX_WideStringC wsRef;
    binditems.GetRef(wsRef);
    uint32_t dwStyle = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties |
                       XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent |
                       XFA_RESOLVENODE_ALL;
    XFA_RESOLVENODE_RS rs;
    pScriptContext->ResolveObjects(pWidgetNode, wsRef, rs, dwStyle);
    int32_t iCount = rs.nodes.GetSize();
    pAcc->DeleteItem(-1);
    if (rs.dwFlags != XFA_RESOVENODE_RSTYPE_Nodes || iCount < 1)
      continue;

    CFX_WideStringC wsValueRef, wsLabelRef;
    binditems.GetValueRef(wsValueRef);
    binditems.GetLabelRef(wsLabelRef);
    const bool bUseValue = wsLabelRef.IsEmpty() || wsLabelRef == wsValueRef;
    const bool bLabelUseContent =
        wsLabelRef.IsEmpty() || wsLabelRef == FX_WSTRC(L"$");
    const bool bValueUseContent =
        wsValueRef.IsEmpty() || wsValueRef == FX_WSTRC(L"$");
    CFX_WideString wsValue;
    CFX_WideString wsLabel;
    uint32_t uValueHash = FX_HashCode_GetW(wsValueRef, false);
    for (int32_t j = 0; j < iCount; j++) {
      CXFA_Object* refObj = rs.nodes[j];
      if (!refObj->IsNode()) {
        continue;
      }
      CXFA_Node* refNode = refObj->AsNode();
      if (bValueUseContent) {
        wsValue = refNode->GetContent();
      } else {
        CXFA_Node* nodeValue = refNode->GetFirstChildByName(uValueHash);
        wsValue = nodeValue ? nodeValue->GetContent() : refNode->GetContent();
      }
      if (!bUseValue) {
        if (bLabelUseContent) {
          wsLabel = refNode->GetContent();
        } else {
          CXFA_Node* nodeLabel = refNode->GetFirstChildByName(wsLabelRef);
          if (nodeLabel)
            wsLabel = nodeLabel->GetContent();
        }
      } else {
        wsLabel = wsValue;
      }
      pAcc->InsertItem(wsLabel, wsValue);
    }
  }
  m_BindItems.RemoveAll();
}
void CXFA_FFDocView::SetChangeMark() {
  if (m_iStatus < XFA_DOCVIEW_LAYOUTSTATUS_End) {
    return;
  }
  m_pDoc->GetDocProvider()->SetChangeMark(m_pDoc);
}
CXFA_Node* CXFA_FFDocView::GetRootSubform() {
  CXFA_Node* pFormPacketNode =
      ToNode(m_pDoc->GetXFADoc()->GetXFAObject(XFA_HASHCODE_Form));
  if (!pFormPacketNode) {
    return nullptr;
  }
  return pFormPacketNode->GetFirstChildByClass(XFA_Element::Subform);
}

CXFA_WidgetAccIterator::CXFA_WidgetAccIterator(CXFA_FFDocView* pDocView,
                                               CXFA_Node* pTravelRoot)
    : m_ContentIterator(pTravelRoot) {
  m_pDocView = pDocView;
  m_pCurWidgetAcc = nullptr;
}
CXFA_WidgetAccIterator::~CXFA_WidgetAccIterator() {}
void CXFA_WidgetAccIterator::Reset() {
  m_pCurWidgetAcc = nullptr;
  m_ContentIterator.Reset();
}
CXFA_WidgetAcc* CXFA_WidgetAccIterator::MoveToFirst() {
  return nullptr;
}
CXFA_WidgetAcc* CXFA_WidgetAccIterator::MoveToLast() {
  return nullptr;
}
CXFA_WidgetAcc* CXFA_WidgetAccIterator::MoveToNext() {
  CXFA_Node* pItem = m_pCurWidgetAcc ? m_ContentIterator.MoveToNext()
                                     : m_ContentIterator.GetCurrent();
  while (pItem) {
    m_pCurWidgetAcc = static_cast<CXFA_WidgetAcc*>(pItem->GetWidgetData());
    if (m_pCurWidgetAcc)
      return m_pCurWidgetAcc;
    pItem = m_ContentIterator.MoveToNext();
  }
  return nullptr;
}
CXFA_WidgetAcc* CXFA_WidgetAccIterator::MoveToPrevious() {
  return nullptr;
}
CXFA_WidgetAcc* CXFA_WidgetAccIterator::GetCurrentWidgetAcc() {
  return nullptr;
}
FX_BOOL CXFA_WidgetAccIterator::SetCurrentWidgetAcc(CXFA_WidgetAcc* hWidget) {
  return FALSE;
}
void CXFA_WidgetAccIterator::SkipTree() {
  m_ContentIterator.SkipChildrenAndMoveToNext();
  m_pCurWidgetAcc = nullptr;
}
