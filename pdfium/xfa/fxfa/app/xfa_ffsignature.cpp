// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffsignature.h"

#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

CXFA_FFSignature::CXFA_FFSignature(CXFA_FFPageView* pPageView,
                                   CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc) {}
CXFA_FFSignature::~CXFA_FFSignature() {}
FX_BOOL CXFA_FFSignature::LoadWidget() {
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFSignature::RenderWidget(CFX_Graphics* pGS,
                                    CFX_Matrix* pMatrix,
                                    uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  CXFA_FFWidget::RenderWidget(pGS, &mtRotate, dwStatus);
  CXFA_Border borderUI = m_pDataAcc->GetUIBorder();
  DrawBorder(pGS, borderUI, m_rtUI, &mtRotate);
  RenderCaption(pGS, &mtRotate);
  DrawHighlight(pGS, &mtRotate, dwStatus, FALSE);
  CFX_RectF rtWidget = m_rtUI;
  IXFA_DocProvider* pDocProvider = m_pDataAcc->GetDoc()->GetDocProvider();
  ASSERT(pDocProvider);
  pDocProvider->RenderCustomWidget(this, pGS, &mtRotate, rtWidget);
}
FX_BOOL CXFA_FFSignature::OnMouseEnter() {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnMouseExit() {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnLButtonDown(uint32_t dwFlags,
                                        FX_FLOAT fx,
                                        FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnLButtonUp(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnLButtonDblClk(uint32_t dwFlags,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnMouseMove(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnMouseWheel(uint32_t dwFlags,
                                       int16_t zDelta,
                                       FX_FLOAT fx,
                                       FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnRButtonDown(uint32_t dwFlags,
                                        FX_FLOAT fx,
                                        FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnRButtonUp(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnRButtonDblClk(uint32_t dwFlags,
                                          FX_FLOAT fx,
                                          FX_FLOAT fy) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) {
  return FALSE;
}
FX_BOOL CXFA_FFSignature::OnChar(uint32_t dwChar, uint32_t dwFlags) {
  return FALSE;
}
FWL_WidgetHit CXFA_FFSignature::OnHitTest(FX_FLOAT fx, FX_FLOAT fy) {
  if (m_pNormalWidget) {
    FX_FLOAT ffx = fx, ffy = fy;
    FWLToClient(ffx, ffy);
    if (m_pNormalWidget->HitTest(ffx, ffy) != FWL_WidgetHit::Unknown)
      return FWL_WidgetHit::Client;
  }
  CFX_RectF rtBox;
  GetRectWithoutRotate(rtBox);
  if (!rtBox.Contains(fx, fy))
    return FWL_WidgetHit::Unknown;
  if (m_rtCaption.Contains(fx, fy))
    return FWL_WidgetHit::Titlebar;
  return FWL_WidgetHit::Client;
}
FX_BOOL CXFA_FFSignature::OnSetCursor(FX_FLOAT fx, FX_FLOAT fy) {
  return FALSE;
}
