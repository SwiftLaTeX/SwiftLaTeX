// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/core/fwl_widgetimp.h"

#include <algorithm>

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_combobox.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themepart.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/core/ifwl_form.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxfa/include/xfa_ffapp.h"

#define FWL_STYLEEXT_MNU_Vert (1L << 0)

IFWL_Widget::IFWL_Widget() {}

IFWL_Widget::~IFWL_Widget() {}

FWL_Error IFWL_Widget::GetClassName(CFX_WideString& wsClass) const {
  return m_pImpl->GetClassName(wsClass);
}

FWL_Type IFWL_Widget::GetClassID() const {
  return m_pImpl->GetClassID();
}

FX_BOOL IFWL_Widget::IsInstance(const CFX_WideStringC& wsClass) const {
  return m_pImpl->IsInstance(wsClass);
}

FWL_Error IFWL_Widget::Initialize() {
  return m_pImpl->Initialize();
}

FWL_Error IFWL_Widget::Finalize() {
  return m_pImpl->Finalize();
}

FWL_Error IFWL_Widget::GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize) {
  return GetImpl()->GetWidgetRect(rect, bAutoSize);
}
FWL_Error IFWL_Widget::GetGlobalRect(CFX_RectF& rect) {
  return GetImpl()->GetGlobalRect(rect);
}
FWL_Error IFWL_Widget::SetWidgetRect(const CFX_RectF& rect) {
  return GetImpl()->SetWidgetRect(rect);
}
FWL_Error IFWL_Widget::GetClientRect(CFX_RectF& rect) {
  return GetImpl()->GetClientRect(rect);
}
IFWL_Widget* IFWL_Widget::GetParent() {
  return GetImpl()->GetParent();
}
FWL_Error IFWL_Widget::SetParent(IFWL_Widget* pParent) {
  return GetImpl()->SetParent(pParent);
}
IFWL_Widget* IFWL_Widget::GetOwner() {
  return GetImpl()->GetOwner();
}
FWL_Error IFWL_Widget::SetOwner(IFWL_Widget* pOwner) {
  return GetImpl()->SetOwner(pOwner);
}
IFWL_Widget* IFWL_Widget::GetOuter() {
  return GetImpl()->GetOuter();
}
uint32_t IFWL_Widget::GetStyles() {
  return GetImpl()->GetStyles();
}
FWL_Error IFWL_Widget::ModifyStyles(uint32_t dwStylesAdded,
                                    uint32_t dwStylesRemoved) {
  return GetImpl()->ModifyStyles(dwStylesAdded, dwStylesRemoved);
}
uint32_t IFWL_Widget::GetStylesEx() {
  return GetImpl()->GetStylesEx();
}
FWL_Error IFWL_Widget::ModifyStylesEx(uint32_t dwStylesExAdded,
                                      uint32_t dwStylesExRemoved) {
  return GetImpl()->ModifyStylesEx(dwStylesExAdded, dwStylesExRemoved);
}
uint32_t IFWL_Widget::GetStates() {
  return GetImpl()->GetStates();
}
void IFWL_Widget::SetStates(uint32_t dwStates, FX_BOOL bSet) {
  GetImpl()->SetStates(dwStates, bSet);
}

uint32_t IFWL_Widget::GetEventKey() const {
  return GetImpl()->GetEventKey();
}

void IFWL_Widget::SetEventKey(uint32_t key) {
  GetImpl()->SetEventKey(key);
}

void* IFWL_Widget::GetLayoutItem() const {
  return GetImpl()->GetLayoutItem();
}

void IFWL_Widget::SetLayoutItem(void* pItem) {
  GetImpl()->SetLayoutItem(pItem);
}

void* IFWL_Widget::GetAssociateWidget() const {
  return GetImpl()->GetAssociateWidget();
}

void IFWL_Widget::SetAssociateWidget(void* pAssociate) {
  GetImpl()->SetAssociateWidget(pAssociate);
}

FWL_Error IFWL_Widget::Update() {
  return GetImpl()->Update();
}
FWL_Error IFWL_Widget::LockUpdate() {
  return GetImpl()->LockUpdate();
}
FWL_Error IFWL_Widget::UnlockUpdate() {
  return GetImpl()->UnlockUpdate();
}
FWL_WidgetHit IFWL_Widget::HitTest(FX_FLOAT fx, FX_FLOAT fy) {
  return GetImpl()->HitTest(fx, fy);
}
FWL_Error IFWL_Widget::TransformTo(IFWL_Widget* pWidget,
                                   FX_FLOAT& fx,
                                   FX_FLOAT& fy) {
  return GetImpl()->TransformTo(pWidget, fx, fy);
}
FWL_Error IFWL_Widget::TransformTo(IFWL_Widget* pWidget, CFX_RectF& rt) {
  return GetImpl()->TransformTo(pWidget, rt);
}
FWL_Error IFWL_Widget::GetMatrix(CFX_Matrix& matrix, FX_BOOL bGlobal) {
  return GetImpl()->GetMatrix(matrix, bGlobal);
}
FWL_Error IFWL_Widget::SetMatrix(const CFX_Matrix& matrix) {
  return GetImpl()->SetMatrix(matrix);
}
FWL_Error IFWL_Widget::DrawWidget(CFX_Graphics* pGraphics,
                                  const CFX_Matrix* pMatrix) {
  return GetImpl()->DrawWidget(pGraphics, pMatrix);
}
IFWL_ThemeProvider* IFWL_Widget::GetThemeProvider() {
  return GetImpl()->GetThemeProvider();
}
FWL_Error IFWL_Widget::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  return GetImpl()->SetThemeProvider(pThemeProvider);
}
FWL_Error IFWL_Widget::SetDataProvider(IFWL_DataProvider* pDataProvider) {
  return GetImpl()->SetDataProvider(pDataProvider);
}
IFWL_WidgetDelegate* IFWL_Widget::SetDelegate(IFWL_WidgetDelegate* pDelegate) {
  return GetImpl()->SetDelegate(pDelegate);
}
IFWL_App* IFWL_Widget::GetOwnerApp() const {
  return GetImpl()->GetOwnerApp();
}
CFX_SizeF IFWL_Widget::GetOffsetFromParent(IFWL_Widget* pParent) {
  return GetImpl()->GetOffsetFromParent(pParent);
}

FWL_Error CFWL_WidgetImp::Initialize() {
  IFWL_App* pApp = FWL_GetApp();
  if (!pApp)
    return FWL_Error::Indefinite;

  CXFA_FFApp* pAdapter = pApp->GetAdapterNative();
  if (!pAdapter)
    return FWL_Error::Indefinite;

  SetOwnerApp(static_cast<CFWL_AppImp*>(FWL_GetApp()->GetImpl()));

  IFWL_Widget* pParent = m_pProperties->m_pParent;
  m_pWidgetMgr->InsertWidget(pParent, m_pInterface);
  if (!IsChild()) {
    IFWL_Widget* pOwner = m_pProperties->m_pOwner;
    if (pOwner)
      m_pWidgetMgr->SetOwner(pOwner, m_pInterface);
  }
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_WidgetImp::Finalize() {
  NotifyDriver();
  m_pWidgetMgr->RemoveWidget(m_pInterface);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_WidgetImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass.clear();
  return FWL_Error::Succeeded;
}

FX_BOOL CFWL_WidgetImp::IsInstance(const CFX_WideStringC& wsClass) const {
  return FALSE;
}

FWL_Error CFWL_WidgetImp::GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize) {
  if (bAutoSize) {
    if (HasEdge()) {
      FX_FLOAT fEdge = GetEdgeWidth();
      rect.Inflate(fEdge, fEdge);
    }
    if (HasBorder()) {
      FX_FLOAT fBorder = GetBorderSize();
      rect.Inflate(fBorder, fBorder);
    }
  } else {
    rect = m_pProperties->m_rtWidget;
  }
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_WidgetImp::GetGlobalRect(CFX_RectF& rect) {
  IFWL_Widget* pForm = m_pWidgetMgr->GetSystemFormWidget(m_pInterface);
  if (!pForm)
    return FWL_Error::Indefinite;

  rect.Set(0, 0, m_pProperties->m_rtWidget.width,
           m_pProperties->m_rtWidget.height);
  if (pForm == m_pInterface)
    return FWL_Error::Succeeded;

  return TransformTo(pForm, rect);
}

FWL_Error CFWL_WidgetImp::SetWidgetRect(const CFX_RectF& rect) {
  CFX_RectF rtOld = m_pProperties->m_rtWidget;
  m_pProperties->m_rtWidget = rect;
  if (IsChild()) {
    if (FXSYS_fabs(rtOld.width - rect.width) > 0.5f ||
        FXSYS_fabs(rtOld.height - rect.height) > 0.5f) {
      CFWL_EvtSizeChanged ev;
      ev.m_pSrcTarget = m_pInterface;
      ev.m_rtOld = rtOld;
      ev.m_rtNew = rect;
      IFWL_WidgetDelegate* pDelegate = SetDelegate(nullptr);
      if (pDelegate) {
        pDelegate->OnProcessEvent(&ev);
      }
    }
    return FWL_Error::Succeeded;
  }
  m_pWidgetMgr->SetWidgetRect_Native(m_pInterface, rect);
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::GetClientRect(CFX_RectF& rect) {
  GetEdgeRect(rect);
  if (HasEdge()) {
    FX_FLOAT fEdge = GetEdgeWidth();
    rect.Deflate(fEdge, fEdge);
  }
  return FWL_Error::Succeeded;
}
IFWL_Widget* CFWL_WidgetImp::GetParent() {
  return m_pWidgetMgr->GetParentWidget(m_pInterface);
}
FWL_Error CFWL_WidgetImp::SetParent(IFWL_Widget* pParent) {
  m_pProperties->m_pParent = pParent;
  m_pWidgetMgr->SetParent(pParent, m_pInterface);
  return FWL_Error::Succeeded;
}
IFWL_Widget* CFWL_WidgetImp::GetOwner() {
  return m_pWidgetMgr->GetOwnerWidget(m_pInterface);
}
FWL_Error CFWL_WidgetImp::SetOwner(IFWL_Widget* pOwner) {
  m_pProperties->m_pOwner = pOwner;
  m_pWidgetMgr->SetOwner(pOwner, m_pInterface);
  return FWL_Error::Succeeded;
}
IFWL_Widget* CFWL_WidgetImp::GetOuter() {
  return m_pOuter;
}
uint32_t CFWL_WidgetImp::GetStyles() {
  return m_pProperties->m_dwStyles;
}
FWL_Error CFWL_WidgetImp::ModifyStyles(uint32_t dwStylesAdded,
                                       uint32_t dwStylesRemoved) {
  m_pProperties->m_dwStyles =
      (m_pProperties->m_dwStyles & ~dwStylesRemoved) | dwStylesAdded;
  return FWL_Error::Succeeded;
}
uint32_t CFWL_WidgetImp::GetStylesEx() {
  return m_pProperties->m_dwStyleExes;
}
FWL_Error CFWL_WidgetImp::ModifyStylesEx(uint32_t dwStylesExAdded,
                                         uint32_t dwStylesExRemoved) {
  m_pProperties->m_dwStyleExes =
      (m_pProperties->m_dwStyleExes & ~dwStylesExRemoved) | dwStylesExAdded;
  return FWL_Error::Succeeded;
}
uint32_t CFWL_WidgetImp::GetStates() {
  return m_pProperties->m_dwStates;
}
static void NotifyHideChildWidget(CFWL_WidgetMgr* widgetMgr,
                                  IFWL_Widget* widget,
                                  CFWL_NoteDriver* noteDriver) {
  IFWL_Widget* child = widgetMgr->GetFirstChildWidget(widget);
  while (child) {
    noteDriver->NotifyTargetHide(child);
    NotifyHideChildWidget(widgetMgr, child, noteDriver);
    child = widgetMgr->GetNextSiblingWidget(child);
  }
}

void CFWL_WidgetImp::SetStates(uint32_t dwStates, FX_BOOL bSet) {
  bSet ? (m_pProperties->m_dwStates |= dwStates)
       : (m_pProperties->m_dwStates &= ~dwStates);
  if (!(dwStates & FWL_WGTSTATE_Invisible) || !bSet)
    return;

  CFWL_NoteDriver* noteDriver =
      static_cast<CFWL_NoteDriver*>(GetOwnerApp()->GetNoteDriver());
  CFWL_WidgetMgr* widgetMgr = CFWL_WidgetMgr::GetInstance();
  noteDriver->NotifyTargetHide(m_pInterface);
  IFWL_Widget* child = widgetMgr->GetFirstChildWidget(m_pInterface);
  while (child) {
    noteDriver->NotifyTargetHide(child);
    NotifyHideChildWidget(widgetMgr, child, noteDriver);
    child = widgetMgr->GetNextSiblingWidget(child);
  }
  return;
}
FWL_Error CFWL_WidgetImp::Update() {
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::LockUpdate() {
  m_iLock++;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::UnlockUpdate() {
  if (IsLocked()) {
    m_iLock--;
  }
  return FWL_Error::Succeeded;
}
FWL_WidgetHit CFWL_WidgetImp::HitTest(FX_FLOAT fx, FX_FLOAT fy) {
  CFX_RectF rtClient;
  GetClientRect(rtClient);
  if (rtClient.Contains(fx, fy))
    return FWL_WidgetHit::Client;
  if (HasEdge()) {
    CFX_RectF rtEdge;
    GetEdgeRect(rtEdge);
    if (rtEdge.Contains(fx, fy))
      return FWL_WidgetHit::Edge;
  }
  if (HasBorder()) {
    CFX_RectF rtRelative;
    GetRelativeRect(rtRelative);
    if (rtRelative.Contains(fx, fy))
      return FWL_WidgetHit::Border;
  }
  return FWL_WidgetHit::Unknown;
}
FWL_Error CFWL_WidgetImp::TransformTo(IFWL_Widget* pWidget,
                                      FX_FLOAT& fx,
                                      FX_FLOAT& fy) {
  if (m_pWidgetMgr->IsFormDisabled()) {
    CFX_SizeF szOffset;
    if (IsParent(pWidget)) {
      szOffset = GetOffsetFromParent(pWidget);
    } else {
      szOffset = pWidget->GetOffsetFromParent(m_pInterface);
      szOffset.x = -szOffset.x;
      szOffset.y = -szOffset.y;
    }
    fx += szOffset.x;
    fy += szOffset.y;
    return FWL_Error::Succeeded;
  }
  CFX_RectF r;
  CFX_Matrix m;
  IFWL_Widget* parent = GetParent();
  if (parent) {
    GetWidgetRect(r);
    fx += r.left;
    fy += r.top;
    GetMatrix(m, TRUE);
    m.TransformPoint(fx, fy);
  }
  IFWL_Widget* form1 = m_pWidgetMgr->GetSystemFormWidget(m_pInterface);
  if (!form1)
    return FWL_Error::Indefinite;
  if (!pWidget) {
    form1->GetWidgetRect(r);
    fx += r.left;
    fy += r.top;
    return FWL_Error::Succeeded;
  }
  IFWL_Widget* form2 = m_pWidgetMgr->GetSystemFormWidget(pWidget);
  if (!form2)
    return FWL_Error::Indefinite;
  if (form1 != form2) {
    form1->GetWidgetRect(r);
    fx += r.left;
    fy += r.top;
    form2->GetWidgetRect(r);
    fx -= r.left;
    fy -= r.top;
  }
  parent = pWidget->GetParent();
  if (parent) {
    pWidget->GetMatrix(m, TRUE);
    CFX_Matrix m1;
    m1.SetIdentity();
    m1.SetReverse(m);
    m1.TransformPoint(fx, fy);
    pWidget->GetWidgetRect(r);
    fx -= r.left;
    fy -= r.top;
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::TransformTo(IFWL_Widget* pWidget, CFX_RectF& rt) {
  return TransformTo(pWidget, rt.left, rt.top);
}
FWL_Error CFWL_WidgetImp::GetMatrix(CFX_Matrix& matrix, FX_BOOL bGlobal) {
  if (!m_pProperties)
    return FWL_Error::Indefinite;
  if (bGlobal) {
    IFWL_Widget* parent = GetParent();
    CFX_ArrayTemplate<IFWL_Widget*> parents;
    while (parent) {
      parents.Add(parent);
      parent = parent->GetParent();
    }
    matrix.SetIdentity();
    CFX_Matrix ctmOnParent;
    CFX_RectF rect;
    int32_t count = parents.GetSize();
    for (int32_t i = count - 2; i >= 0; i--) {
      parent = parents.GetAt(i);
      parent->GetMatrix(ctmOnParent, FALSE);
      parent->GetWidgetRect(rect);
      matrix.Concat(ctmOnParent, TRUE);
      matrix.Translate(rect.left, rect.top, TRUE);
    }
    matrix.Concat(m_pProperties->m_ctmOnParent, TRUE);
    parents.RemoveAll();
  } else {
    matrix = m_pProperties->m_ctmOnParent;
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::SetMatrix(const CFX_Matrix& matrix) {
  if (!m_pProperties)
    return FWL_Error::Indefinite;
  IFWL_Widget* parent = GetParent();
  if (!parent) {
    return FWL_Error::Indefinite;
  }
  m_pProperties->m_ctmOnParent = matrix;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::DrawWidget(CFX_Graphics* pGraphics,
                                     const CFX_Matrix* pMatrix) {
  return FWL_Error::Indefinite;
}
IFWL_ThemeProvider* CFWL_WidgetImp::GetThemeProvider() {
  return m_pProperties->m_pThemeProvider;
}
FWL_Error CFWL_WidgetImp::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  m_pProperties->m_pThemeProvider = pThemeProvider;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_WidgetImp::SetDataProvider(IFWL_DataProvider* pDataProvider) {
  m_pProperties->m_pDataProvider = pDataProvider;
  return FWL_Error::Succeeded;
}
IFWL_WidgetDelegate* CFWL_WidgetImp::SetDelegate(
    IFWL_WidgetDelegate* pDelegate) {
  if (!m_pCurDelegate) {
    m_pCurDelegate = m_pDelegate;
  }
  if (!pDelegate) {
    return m_pCurDelegate;
  }
  IFWL_WidgetDelegate* pOldDelegate = m_pCurDelegate;
  m_pCurDelegate = pDelegate;
  return pOldDelegate;
}
IFWL_App* CFWL_WidgetImp::GetOwnerApp() const {
  return static_cast<IFWL_App*>(m_pOwnerApp->GetInterface());
}
FWL_Error CFWL_WidgetImp::SetOwnerApp(CFWL_AppImp* pOwnerApp) {
  m_pOwnerApp = pOwnerApp;
  return FWL_Error::Succeeded;
}
IFWL_Widget* CFWL_WidgetImp::GetInterface() const {
  return m_pInterface;
}
void CFWL_WidgetImp::SetInterface(IFWL_Widget* pInterface) {
  m_pInterface = pInterface;
}

uint32_t CFWL_WidgetImp::GetEventKey() const {
  return m_nEventKey;
}

void CFWL_WidgetImp::SetEventKey(uint32_t key) {
  m_nEventKey = key;
}

void* CFWL_WidgetImp::GetLayoutItem() const {
  return m_pLayoutItem;
}

void CFWL_WidgetImp::SetLayoutItem(void* pItem) {
  m_pLayoutItem = pItem;
}

void* CFWL_WidgetImp::GetAssociateWidget() const {
  return m_pAssociate;
}

void CFWL_WidgetImp::SetAssociateWidget(void* pAssociate) {
  m_pAssociate = pAssociate;
}

CFWL_WidgetImp::CFWL_WidgetImp(const CFWL_WidgetImpProperties& properties,
                               IFWL_Widget* pOuter)
    : m_pProperties(new CFWL_WidgetImpProperties),
      m_pDelegate(nullptr),
      m_pCurDelegate(nullptr),
      m_pOuter(pOuter),
      m_pInterface(nullptr),
      m_pLayoutItem(nullptr),
      m_pAssociate(nullptr),
      m_iLock(0),
      m_nEventKey(0) {
  *m_pProperties = properties;
  m_pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  ASSERT(m_pWidgetMgr);
}

CFWL_WidgetImp::~CFWL_WidgetImp() {
  delete m_pProperties;
}

FX_BOOL CFWL_WidgetImp::IsEnabled() const {
  return (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) == 0;
}
FX_BOOL CFWL_WidgetImp::IsVisible() const {
  return (m_pProperties->m_dwStates & FWL_WGTSTATE_Invisible) == 0;
}
FX_BOOL CFWL_WidgetImp::IsActive() const {
  return (m_pProperties->m_dwStates & FWL_WGTSTATE_Deactivated) == 0;
}
FX_BOOL CFWL_WidgetImp::IsOverLapper() const {
  return (m_pProperties->m_dwStyles & FWL_WGTSTYLE_WindowTypeMask) ==
         FWL_WGTSTYLE_OverLapper;
}
FX_BOOL CFWL_WidgetImp::IsPopup() const {
  return !!(m_pProperties->m_dwStyles & FWL_WGTSTYLE_Popup);
}
FX_BOOL CFWL_WidgetImp::IsChild() const {
  return !!(m_pProperties->m_dwStyles & FWL_WGTSTYLE_Child);
}
FX_BOOL CFWL_WidgetImp::IsLocked() const {
  return m_iLock > 0;
}
FX_BOOL CFWL_WidgetImp::IsOffscreen() const {
  return !!(m_pProperties->m_dwStyles & FWL_WGTSTYLE_Offscreen);
}
FX_BOOL CFWL_WidgetImp::HasBorder() const {
  return !!(m_pProperties->m_dwStyles & FWL_WGTSTYLE_Border);
}
FX_BOOL CFWL_WidgetImp::HasEdge() const {
  return !!(m_pProperties->m_dwStyles & FWL_WGTSTYLE_EdgeMask);
}
void CFWL_WidgetImp::GetEdgeRect(CFX_RectF& rtEdge) {
  rtEdge = m_pProperties->m_rtWidget;
  rtEdge.left = rtEdge.top = 0;
  if (HasBorder()) {
    FX_FLOAT fCX = GetBorderSize();
    FX_FLOAT fCY = GetBorderSize(FALSE);
    rtEdge.Deflate(fCX, fCY);
  }
}
FX_FLOAT CFWL_WidgetImp::GetBorderSize(FX_BOOL bCX) {
  FX_FLOAT* pfBorder = static_cast<FX_FLOAT*>(GetThemeCapacity(
      bCX ? CFWL_WidgetCapacity::CXBorder : CFWL_WidgetCapacity::CYBorder));
  if (!pfBorder)
    return 0;
  return *pfBorder;
}
FX_FLOAT CFWL_WidgetImp::GetEdgeWidth() {
  CFWL_WidgetCapacity dwCapacity = CFWL_WidgetCapacity::None;
  switch (m_pProperties->m_dwStyles & FWL_WGTSTYLE_EdgeMask) {
    case FWL_WGTSTYLE_EdgeFlat: {
      dwCapacity = CFWL_WidgetCapacity::EdgeFlat;
      break;
    }
    case FWL_WGTSTYLE_EdgeRaised: {
      dwCapacity = CFWL_WidgetCapacity::EdgeRaised;
      break;
    }
    case FWL_WGTSTYLE_EdgeSunken: {
      dwCapacity = CFWL_WidgetCapacity::EdgeSunken;
      break;
    }
  }
  if (dwCapacity != CFWL_WidgetCapacity::None) {
    FX_FLOAT* fRet = static_cast<FX_FLOAT*>(GetThemeCapacity(dwCapacity));
    return fRet ? *fRet : 0;
  }
  return 0;
}
void CFWL_WidgetImp::GetRelativeRect(CFX_RectF& rect) {
  rect = m_pProperties->m_rtWidget;
  rect.left = rect.top = 0;
}
void* CFWL_WidgetImp::GetThemeCapacity(CFWL_WidgetCapacity dwCapacity) {
  IFWL_ThemeProvider* pTheme = GetAvailableTheme();
  if (!pTheme)
    return nullptr;
  CFWL_ThemePart part;
  part.m_pWidget = m_pInterface;
  return pTheme->GetCapacity(&part, dwCapacity);
}
IFWL_ThemeProvider* CFWL_WidgetImp::GetAvailableTheme() {
  if (m_pProperties->m_pThemeProvider) {
    return m_pProperties->m_pThemeProvider;
  }
  IFWL_Widget* pUp = m_pInterface;
  do {
    pUp = (pUp->GetStyles() & FWL_WGTSTYLE_Popup)
              ? m_pWidgetMgr->GetOwnerWidget(pUp)
              : m_pWidgetMgr->GetParentWidget(pUp);
    if (pUp) {
      IFWL_ThemeProvider* pRet = pUp->GetThemeProvider();
      if (pRet && pRet->IsValidWidget(m_pInterface))
        return pRet;
    }
  } while (pUp);
  return FWL_GetApp()->GetThemeProvider();
}
CFWL_WidgetImp* CFWL_WidgetImp::GetRootOuter() {
  IFWL_Widget* pRet = m_pOuter;
  if (!pRet)
    return nullptr;
  while (IFWL_Widget* pOuter = pRet->GetOuter()) {
    pRet = pOuter;
  }
  return static_cast<CFWL_WidgetImp*>(pRet->GetImpl());
}
#define FWL_WGT_CalcHeight 2048
#define FWL_WGT_CalcWidth 2048
#define FWL_WGT_CalcMultiLineDefWidth 120.0f
CFX_SizeF CFWL_WidgetImp::CalcTextSize(const CFX_WideString& wsText,
                                       IFWL_ThemeProvider* pTheme,
                                       FX_BOOL bMultiLine,
                                       int32_t iLineWidth) {
  if (!pTheme)
    return CFX_SizeF();

  CFWL_ThemeText calPart;
  calPart.m_pWidget = m_pInterface;
  calPart.m_wsText = wsText;
  calPart.m_dwTTOStyles =
      bMultiLine ? FDE_TTOSTYLE_LineWrap : FDE_TTOSTYLE_SingleLine;
  calPart.m_iTTOAlign = FDE_TTOALIGNMENT_TopLeft;
  CFX_RectF rect;
  FX_FLOAT fWidth = bMultiLine
                        ? (iLineWidth > 0 ? (FX_FLOAT)iLineWidth
                                          : FWL_WGT_CalcMultiLineDefWidth)
                        : FWL_WGT_CalcWidth;
  rect.Set(0, 0, fWidth, FWL_WGT_CalcHeight);
  pTheme->CalcTextRect(&calPart, rect);
  return CFX_SizeF(rect.width, rect.height);
}
void CFWL_WidgetImp::CalcTextRect(const CFX_WideString& wsText,
                                  IFWL_ThemeProvider* pTheme,
                                  uint32_t dwTTOStyles,
                                  int32_t iTTOAlign,
                                  CFX_RectF& rect) {
  CFWL_ThemeText calPart;
  calPart.m_pWidget = m_pInterface;
  calPart.m_wsText = wsText;
  calPart.m_dwTTOStyles = dwTTOStyles;
  calPart.m_iTTOAlign = iTTOAlign;
  pTheme->CalcTextRect(&calPart, rect);
}
void CFWL_WidgetImp::SetFocus(FX_BOOL bFocus) {
  if (m_pWidgetMgr->IsFormDisabled())
    return;

  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pDriver =
      static_cast<CFWL_NoteDriver*>(pApp->GetNoteDriver());
  if (!pDriver)
    return;

  IFWL_Widget* curFocus = pDriver->GetFocus();
  if (bFocus && curFocus != m_pInterface) {
    pDriver->SetFocus(m_pInterface);
  } else if (!bFocus && curFocus == m_pInterface) {
    pDriver->SetFocus(nullptr);
  }
}
void CFWL_WidgetImp::SetGrab(FX_BOOL bSet) {
  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;
  CFWL_NoteDriver* pDriver =
      static_cast<CFWL_NoteDriver*>(pApp->GetNoteDriver());
  pDriver->SetGrab(m_pInterface, bSet);
}

FX_BOOL CFWL_WidgetImp::GetPopupPos(FX_FLOAT fMinHeight,
                                    FX_FLOAT fMaxHeight,
                                    const CFX_RectF& rtAnchor,
                                    CFX_RectF& rtPopup) {
  if (GetClassID() == FWL_Type::ComboBox) {
    if (m_pWidgetMgr->IsFormDisabled()) {
      return m_pWidgetMgr->GetAdapterPopupPos(m_pInterface, fMinHeight,
                                              fMaxHeight, rtAnchor, rtPopup);
    }
    return GetPopupPosComboBox(fMinHeight, fMaxHeight, rtAnchor, rtPopup);
  }
  if (GetClassID() == FWL_Type::DateTimePicker &&
      m_pWidgetMgr->IsFormDisabled()) {
    return m_pWidgetMgr->GetAdapterPopupPos(m_pInterface, fMinHeight,
                                            fMaxHeight, rtAnchor, rtPopup);
  }
  return GetPopupPosGeneral(fMinHeight, fMaxHeight, rtAnchor, rtPopup);
}

FX_BOOL CFWL_WidgetImp::GetPopupPosMenu(FX_FLOAT fMinHeight,
                                        FX_FLOAT fMaxHeight,
                                        const CFX_RectF& rtAnchor,
                                        CFX_RectF& rtPopup) {
  FX_FLOAT fx = 0;
  FX_FLOAT fy = 0;
  FX_FLOAT fScreenWidth = 0;
  FX_FLOAT fScreenHeight = 0;
  GetScreenSize(fScreenWidth, fScreenHeight);
  if (GetStylesEx() & FWL_STYLEEXT_MNU_Vert) {
    FX_BOOL bLeft = m_pProperties->m_rtWidget.left < 0;
    FX_FLOAT fRight = rtAnchor.right() + rtPopup.width;
    TransformTo(nullptr, fx, fy);
    if (fRight + fx > fScreenWidth || bLeft) {
      rtPopup.Set(rtAnchor.left - rtPopup.width, rtAnchor.top, rtPopup.width,
                  rtPopup.height);
    } else {
      rtPopup.Set(rtAnchor.right(), rtAnchor.top, rtPopup.width,
                  rtPopup.height);
    }
  } else {
    FX_FLOAT fBottom = rtAnchor.bottom() + rtPopup.height;
    TransformTo(nullptr, fx, fy);
    if (fBottom + fy > fScreenHeight) {
      rtPopup.Set(rtAnchor.left, rtAnchor.top - rtPopup.height, rtPopup.width,
                  rtPopup.height);
    } else {
      rtPopup.Set(rtAnchor.left, rtAnchor.bottom(), rtPopup.width,
                  rtPopup.height);
    }
  }
  rtPopup.Offset(fx, fy);
  return TRUE;
}
FX_BOOL CFWL_WidgetImp::GetPopupPosComboBox(FX_FLOAT fMinHeight,
                                            FX_FLOAT fMaxHeight,
                                            const CFX_RectF& rtAnchor,
                                            CFX_RectF& rtPopup) {
  FX_FLOAT fx = 0;
  FX_FLOAT fy = 0;
  FX_FLOAT fScreenWidth = 0;
  FX_FLOAT fScreenHeight = 0;
  GetScreenSize(fScreenWidth, fScreenHeight);
  FX_FLOAT fPopHeight = rtPopup.height;
  if (rtPopup.height > fMaxHeight) {
    fPopHeight = fMaxHeight;
  } else if (rtPopup.height < fMinHeight) {
    fPopHeight = fMinHeight;
  }
  FX_FLOAT fWidth = std::max(rtAnchor.width, rtPopup.width);
  FX_FLOAT fBottom = rtAnchor.bottom() + fPopHeight;
  TransformTo(nullptr, fx, fy);
  if (fBottom + fy > fScreenHeight) {
    rtPopup.Set(rtAnchor.left, rtAnchor.top - fPopHeight, fWidth, fPopHeight);
  } else {
    rtPopup.Set(rtAnchor.left, rtAnchor.bottom(), fWidth, fPopHeight);
  }
  rtPopup.Offset(fx, fy);
  return TRUE;
}
FX_BOOL CFWL_WidgetImp::GetPopupPosGeneral(FX_FLOAT fMinHeight,
                                           FX_FLOAT fMaxHeight,
                                           const CFX_RectF& rtAnchor,
                                           CFX_RectF& rtPopup) {
  FX_FLOAT fx = 0;
  FX_FLOAT fy = 0;
  FX_FLOAT fScreenWidth = 0;
  FX_FLOAT fScreenHeight = 0;
  GetScreenSize(fScreenWidth, fScreenHeight);
  TransformTo(nullptr, fx, fy);
  if (rtAnchor.bottom() + fy > fScreenHeight) {
    rtPopup.Set(rtAnchor.left, rtAnchor.top - rtPopup.height, rtPopup.width,
                rtPopup.height);
  } else {
    rtPopup.Set(rtAnchor.left, rtAnchor.bottom(), rtPopup.width,
                rtPopup.height);
  }
  rtPopup.Offset(fx, fy);
  return TRUE;
}
FX_BOOL CFWL_WidgetImp::GetScreenSize(FX_FLOAT& fx, FX_FLOAT& fy) {
  return FALSE;
}
void CFWL_WidgetImp::RegisterEventTarget(IFWL_Widget* pEventSource,
                                         uint32_t dwFilter) {
  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pNoteDriver = pApp->GetNoteDriver();
  if (!pNoteDriver)
    return;

  pNoteDriver->RegisterEventTarget(m_pInterface, pEventSource, dwFilter);
}
void CFWL_WidgetImp::UnregisterEventTarget() {
  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pNoteDriver = pApp->GetNoteDriver();
  if (!pNoteDriver)
    return;

  pNoteDriver->UnregisterEventTarget(m_pInterface);
}
void CFWL_WidgetImp::DispatchKeyEvent(CFWL_MsgKey* pNote) {
  if (!pNote)
    return;
  CFWL_EvtKey* pEvent = new CFWL_EvtKey;
  pEvent->m_pSrcTarget = m_pInterface;
  pEvent->m_dwCmd = pNote->m_dwCmd;
  pEvent->m_dwKeyCode = pNote->m_dwKeyCode;
  pEvent->m_dwFlags = pNote->m_dwFlags;
  DispatchEvent(pEvent);
  pEvent->Release();
}
void CFWL_WidgetImp::DispatchEvent(CFWL_Event* pEvent) {
  if (m_pOuter) {
    IFWL_WidgetDelegate* pDelegate = m_pOuter->SetDelegate(nullptr);
    pDelegate->OnProcessEvent(pEvent);
    return;
  }
  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;
  CFWL_NoteDriver* pNoteDriver = pApp->GetNoteDriver();
  if (!pNoteDriver)
    return;
  pNoteDriver->SendEvent(pEvent);
}
void CFWL_WidgetImp::Repaint(const CFX_RectF* pRect) {
  if (pRect) {
    m_pWidgetMgr->RepaintWidget(m_pInterface, pRect);
    return;
  }
  CFX_RectF rect;
  rect = m_pProperties->m_rtWidget;
  rect.left = rect.top = 0;
  m_pWidgetMgr->RepaintWidget(m_pInterface, &rect);
}
void CFWL_WidgetImp::DrawBackground(CFX_Graphics* pGraphics,
                                    CFWL_Part iPartBk,
                                    IFWL_ThemeProvider* pTheme,
                                    const CFX_Matrix* pMatrix) {
  CFX_RectF rtRelative;
  GetRelativeRect(rtRelative);
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = iPartBk;
  param.m_pGraphics = pGraphics;
  if (pMatrix) {
    param.m_matrix.Concat(*pMatrix, TRUE);
  }
  param.m_rtPart = rtRelative;
  pTheme->DrawBackground(&param);
}
void CFWL_WidgetImp::DrawBorder(CFX_Graphics* pGraphics,
                                CFWL_Part iPartBorder,
                                IFWL_ThemeProvider* pTheme,
                                const CFX_Matrix* pMatrix) {
  CFX_RectF rtRelative;
  GetRelativeRect(rtRelative);
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = iPartBorder;
  param.m_pGraphics = pGraphics;
  if (pMatrix) {
    param.m_matrix.Concat(*pMatrix, TRUE);
  }
  param.m_rtPart = rtRelative;
  pTheme->DrawBackground(&param);
}
void CFWL_WidgetImp::DrawEdge(CFX_Graphics* pGraphics,
                              CFWL_Part iPartEdge,
                              IFWL_ThemeProvider* pTheme,
                              const CFX_Matrix* pMatrix) {
  CFX_RectF rtEdge;
  GetEdgeRect(rtEdge);
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = iPartEdge;
  param.m_pGraphics = pGraphics;
  if (pMatrix) {
    param.m_matrix.Concat(*pMatrix, TRUE);
  }
  param.m_rtPart = rtEdge;
  pTheme->DrawBackground(&param);
}
void CFWL_WidgetImp::NotifyDriver() {
  IFWL_App* pApp = GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pDriver =
      static_cast<CFWL_NoteDriver*>(pApp->GetNoteDriver());
  if (!pDriver)
    return;

  pDriver->NotifyTargetDestroy(m_pInterface);
}
CFX_SizeF CFWL_WidgetImp::GetOffsetFromParent(IFWL_Widget* pParent) {
  if (pParent == GetInterface())
    return CFX_SizeF();

  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (!pWidgetMgr)
    return CFX_SizeF();

  CFX_SizeF szRet(m_pProperties->m_rtWidget.left,
                  m_pProperties->m_rtWidget.top);

  IFWL_Widget* pDstWidget = GetParent();
  while (pDstWidget && pDstWidget != pParent) {
    CFX_RectF rtDst;
    pDstWidget->GetWidgetRect(rtDst);
    szRet += CFX_SizeF(rtDst.left, rtDst.top);
    pDstWidget = pWidgetMgr->GetParentWidget(pDstWidget);
  }
  return szRet;
}
FX_BOOL CFWL_WidgetImp::IsParent(IFWL_Widget* pParent) {
  IFWL_Widget* pUpWidget = GetParent();
  while (pUpWidget) {
    if (pUpWidget == pParent)
      return TRUE;
    pUpWidget = pUpWidget->GetParent();
  }
  return FALSE;
}

CFWL_WidgetImpDelegate::CFWL_WidgetImpDelegate() {}

void CFWL_WidgetImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage->m_pDstTarget)
    return;

  CFWL_WidgetImp* pWidget =
      static_cast<CFWL_WidgetImp*>(pMessage->m_pDstTarget->GetImpl());
  CFWL_MessageType dwMsgCode = pMessage->GetClassID();
  switch (dwMsgCode) {
    case CFWL_MessageType::Mouse: {
      CFWL_MsgMouse* pMsgMouse = static_cast<CFWL_MsgMouse*>(pMessage);
      CFWL_EvtMouse evt;
      evt.m_pSrcTarget = pWidget->m_pInterface;
      evt.m_pDstTarget = pWidget->m_pInterface;
      evt.m_dwCmd = pMsgMouse->m_dwCmd;
      evt.m_dwFlags = pMsgMouse->m_dwFlags;
      evt.m_fx = pMsgMouse->m_fx;
      evt.m_fy = pMsgMouse->m_fy;
      pWidget->DispatchEvent(&evt);
      break;
    }
    case CFWL_MessageType::MouseWheel: {
      CFWL_MsgMouseWheel* pMsgMouseWheel =
          static_cast<CFWL_MsgMouseWheel*>(pMessage);
      CFWL_EvtMouseWheel evt;
      evt.m_pSrcTarget = pWidget->m_pInterface;
      evt.m_pDstTarget = pWidget->m_pInterface;
      evt.m_dwFlags = pMsgMouseWheel->m_dwFlags;
      evt.m_fDeltaX = pMsgMouseWheel->m_fDeltaX;
      evt.m_fDeltaY = pMsgMouseWheel->m_fDeltaY;
      evt.m_fx = pMsgMouseWheel->m_fx;
      evt.m_fy = pMsgMouseWheel->m_fy;
      pWidget->DispatchEvent(&evt);
      break;
    }
    case CFWL_MessageType::Key: {
      CFWL_MsgKey* pMsgKey = static_cast<CFWL_MsgKey*>(pMessage);
      CFWL_EvtKey evt;
      evt.m_pSrcTarget = pWidget->m_pInterface;
      evt.m_pDstTarget = pWidget->m_pInterface;
      evt.m_dwKeyCode = pMsgKey->m_dwKeyCode;
      evt.m_dwFlags = pMsgKey->m_dwFlags;
      evt.m_dwCmd = pMsgKey->m_dwCmd;
      pWidget->DispatchEvent(&evt);
      break;
    }
    case CFWL_MessageType::SetFocus: {
      CFWL_MsgSetFocus* pMsgSetFocus = static_cast<CFWL_MsgSetFocus*>(pMessage);
      CFWL_EvtSetFocus evt;
      evt.m_pSrcTarget = pMsgSetFocus->m_pDstTarget;
      evt.m_pDstTarget = pMsgSetFocus->m_pDstTarget;
      evt.m_pSetFocus = pWidget->m_pInterface;
      pWidget->DispatchEvent(&evt);
      break;
    }
    case CFWL_MessageType::KillFocus: {
      CFWL_MsgKillFocus* pMsgKillFocus =
          static_cast<CFWL_MsgKillFocus*>(pMessage);
      CFWL_EvtKillFocus evt;
      evt.m_pSrcTarget = pMsgKillFocus->m_pDstTarget;
      evt.m_pDstTarget = pMsgKillFocus->m_pDstTarget;
      evt.m_pKillFocus = pWidget->m_pInterface;
      pWidget->DispatchEvent(&evt);
      break;
    }
    default:
      break;
  }
}

void CFWL_WidgetImpDelegate::OnProcessEvent(CFWL_Event* pEvent) {}

void CFWL_WidgetImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                          const CFX_Matrix* pMatrix) {
  CFWL_EvtDraw evt;
  evt.m_pGraphics = pGraphics;
}
