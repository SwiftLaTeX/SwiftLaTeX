// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/xfa_rendercontext.h"

#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

namespace {

const int32_t kMaxCount = 30;

}  // namsepace

CXFA_RenderContext::CXFA_RenderContext()
    : m_pWidget(nullptr), m_pPageView(nullptr), m_pGS(nullptr), m_dwStatus(0) {
  m_matrix.SetIdentity();
  m_rtClipRect.Reset();
}

CXFA_RenderContext::~CXFA_RenderContext() {}

int32_t CXFA_RenderContext::StartRender(CXFA_FFPageView* pPageView,
                                        CFX_Graphics* pGS,
                                        const CFX_Matrix& matrix,
                                        const CXFA_RenderOptions& options) {
  m_pPageView = pPageView;
  m_pGS = pGS;
  m_matrix = matrix;
  m_options = options;
  CFX_RectF rtPage;
  pGS->GetClipRect(rtPage);
  CFX_Matrix mtRes;
  mtRes.SetReverse(matrix);
  m_rtClipRect.Set(rtPage.left, rtPage.top, rtPage.width, rtPage.height);
  mtRes.TransformRect(m_rtClipRect);
  m_dwStatus = m_options.m_bHighlight ? XFA_WidgetStatus_Highlight : 0;
  uint32_t dwFilterType = XFA_WidgetStatus_Visible |
                          (m_options.m_bPrint ? XFA_WidgetStatus_Printable
                                              : XFA_WidgetStatus_Viewable);
  m_pWidgetIterator.reset(
      m_pPageView->CreateWidgetIterator(XFA_TRAVERSEWAY_Form, dwFilterType));
  m_pWidget = m_pWidgetIterator->MoveToNext();
  return XFA_RENDERSTATUS_Ready;
}

int32_t CXFA_RenderContext::DoRender(IFX_Pause* pPause) {
  int32_t iCount = 0;
  while (m_pWidget) {
    CXFA_FFWidget* pWidget = m_pWidget;
    CFX_RectF rtWidgetBox;
    pWidget->GetBBox(rtWidgetBox, XFA_WidgetStatus_Visible);
    rtWidgetBox.width += 1;
    rtWidgetBox.height += 1;
    if (rtWidgetBox.IntersectWith(m_rtClipRect))
      pWidget->RenderWidget(m_pGS, &m_matrix, m_dwStatus);
    m_pWidget = m_pWidgetIterator->MoveToNext();
    iCount++;
    if (iCount > kMaxCount && pPause && pPause->NeedToPauseNow())
      return XFA_RENDERSTATUS_ToBeContinued;
  }
  return XFA_RENDERSTATUS_Done;
}

void CXFA_RenderContext::StopRender() {
  m_pWidgetIterator.reset();
}
