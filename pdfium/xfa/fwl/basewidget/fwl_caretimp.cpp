// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_caretimp.h"

#include "xfa/fwl/basewidget/ifwl_caret.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"

// static
IFWL_Caret* IFWL_Caret::Create(const CFWL_WidgetImpProperties& properties,
                               IFWL_Widget* pOuter) {
  IFWL_Caret* pCaret = new IFWL_Caret;
  CFWL_CaretImp* pCaretImpl = new CFWL_CaretImp(properties, pOuter);
  pCaret->SetImpl(pCaretImpl);
  pCaretImpl->SetInterface(pCaret);
  return pCaret;
}
IFWL_Caret::IFWL_Caret() {}
void IFWL_Caret::ShowCaret(FX_BOOL bFlag) {
  static_cast<CFWL_CaretImp*>(GetImpl())->ShowCaret(bFlag);
}
FWL_Error IFWL_Caret::GetFrequency(uint32_t& elapse) {
  return static_cast<CFWL_CaretImp*>(GetImpl())->GetFrequency(elapse);
}
FWL_Error IFWL_Caret::SetFrequency(uint32_t elapse) {
  return static_cast<CFWL_CaretImp*>(GetImpl())->SetFrequency(elapse);
}
FWL_Error IFWL_Caret::SetColor(CFX_Color crFill) {
  return static_cast<CFWL_CaretImp*>(GetImpl())->SetColor(crFill);
}

CFWL_CaretImp::CFWL_CaretImp(const CFWL_WidgetImpProperties& properties,
                             IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_pTimerInfo(nullptr),
      m_dwElapse(400),
      m_bSetColor(FALSE) {
  m_pTimer = new CFWL_CaretTimer(this);
  SetStates(FWL_STATE_CAT_HightLight);
}

CFWL_CaretImp::~CFWL_CaretImp() {
  delete m_pTimer;
}

FWL_Error CFWL_CaretImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_Caret;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_CaretImp::GetClassID() const {
  return FWL_Type::Caret;
}

FWL_Error CFWL_CaretImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;

  m_pDelegate = new CFWL_CaretImpDelegate(this);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_CaretImp::Finalize() {
  if (m_pTimerInfo) {
    m_pTimerInfo->StopTimer();
    m_pTimerInfo = nullptr;
  }
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_CaretImp::DrawWidget(CFX_Graphics* pGraphics,
                                    const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  if (!m_pProperties->m_pThemeProvider)
    return FWL_Error::Indefinite;

  DrawCaretBK(pGraphics, m_pProperties->m_pThemeProvider, pMatrix);
  return FWL_Error::Succeeded;
}

void CFWL_CaretImp::ShowCaret(FX_BOOL bFlag) {
  if (m_pTimerInfo) {
    m_pTimerInfo->StopTimer();
    m_pTimerInfo = nullptr;
  }
  if (bFlag)
    m_pTimerInfo = m_pTimer->StartTimer(m_dwElapse, true);

  SetStates(FWL_WGTSTATE_Invisible, !bFlag);
}
FWL_Error CFWL_CaretImp::GetFrequency(uint32_t& elapse) {
  elapse = m_dwElapse;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_CaretImp::SetFrequency(uint32_t elapse) {
  m_dwElapse = elapse;
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_CaretImp::SetColor(CFX_Color crFill) {
  m_bSetColor = TRUE;
  m_crFill = crFill;
  return FWL_Error::Succeeded;
}

void CFWL_CaretImp::DrawCaretBK(CFX_Graphics* pGraphics,
                                IFWL_ThemeProvider* pTheme,
                                const CFX_Matrix* pMatrix) {
  CFX_RectF rect;
  GetWidgetRect(rect);
  rect.Set(0, 0, rect.width, rect.height);
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_pGraphics = pGraphics;
  param.m_rtPart = rect;
  if (m_bSetColor)
    param.m_pData = &m_crFill;
  if (!(m_pProperties->m_dwStates & FWL_STATE_CAT_HightLight))
    return;

  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = CFWL_PartState_HightLight;
  if (pMatrix)
    param.m_matrix.Concat(*pMatrix);

  pTheme->DrawBackground(&param);
}

CFWL_CaretImp::CFWL_CaretTimer::CFWL_CaretTimer(CFWL_CaretImp* pCaret)
    : m_pCaret(pCaret) {}

void CFWL_CaretImp::CFWL_CaretTimer::Run(IFWL_TimerInfo* pTimerInfo) {
  bool toggle = !(m_pCaret->GetStates() & FWL_STATE_CAT_HightLight);
  m_pCaret->SetStates(FWL_STATE_CAT_HightLight, toggle);

  CFX_RectF rt;
  m_pCaret->GetWidgetRect(rt);
  rt.Set(0, 0, rt.width + 1, rt.height);
  m_pCaret->Repaint(&rt);
}

CFWL_CaretImpDelegate::CFWL_CaretImpDelegate(CFWL_CaretImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_CaretImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {}

void CFWL_CaretImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                         const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}
