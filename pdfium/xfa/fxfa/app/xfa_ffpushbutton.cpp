// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffpushbutton.h"

#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/lightwidget/cfwl_pushbutton.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_ffwidgetacc.h"
#include "xfa/fxfa/app/xfa_textlayout.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

CXFA_FFPushButton::CXFA_FFPushButton(CXFA_FFPageView* pPageView,
                                     CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc),
      m_pRolloverTextLayout(nullptr),
      m_pDownTextLayout(nullptr),
      m_pDownProvider(nullptr),
      m_pRollProvider(nullptr),
      m_pOldDelegate(nullptr) {}
CXFA_FFPushButton::~CXFA_FFPushButton() {
  CXFA_FFPushButton::UnloadWidget();
}
void CXFA_FFPushButton::RenderWidget(CFX_Graphics* pGS,
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
  RenderHighlightCaption(pGS, &mtRotate);
  CFX_RectF rtWidget;
  GetRectWithoutRotate(rtWidget);
  CFX_Matrix mt;
  mt.Set(1, 0, 0, 1, rtWidget.left, rtWidget.top);
  mt.Concat(mtRotate);
  GetApp()->GetWidgetMgrDelegate()->OnDrawWidget(m_pNormalWidget->GetWidget(),
                                                 pGS, &mt);
}
FX_BOOL CXFA_FFPushButton::LoadWidget() {
  ASSERT(!m_pNormalWidget);
  CFWL_PushButton* pPushButton = CFWL_PushButton::Create();
  if (pPushButton) {
    pPushButton->Initialize();
  }
  m_pOldDelegate = pPushButton->SetDelegate(this);
  m_pNormalWidget = pPushButton;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pNormalWidget->LockUpdate();
  UpdateWidgetProperty();
  LoadHighlightCaption();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFPushButton::UpdateWidgetProperty() {
  uint32_t dwStyleEx = 0;
  switch (m_pDataAcc->GetButtonHighlight()) {
    case XFA_ATTRIBUTEENUM_Inverted:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HiliteInverted;
      break;
    case XFA_ATTRIBUTEENUM_Outline:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HiliteOutLine;
      break;
    case XFA_ATTRIBUTEENUM_Push:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HilitePush;
      break;
    default:
      break;
  }
  m_pNormalWidget->ModifyStylesEx(dwStyleEx, 0xFFFFFFFF);
}

void CXFA_FFPushButton::UnloadWidget() {
  delete m_pRolloverTextLayout;
  m_pRolloverTextLayout = nullptr;
  delete m_pDownTextLayout;
  m_pDownTextLayout = nullptr;
  delete m_pDownProvider;
  m_pDownProvider = nullptr;
  delete m_pRollProvider;
  m_pRollProvider = nullptr;
  CXFA_FFField::UnloadWidget();
}

FX_BOOL CXFA_FFPushButton::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  CFX_RectF rtWidget;
  GetRectWithoutRotate(rtWidget);
  m_rtUI = rtWidget;
  if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
    XFA_RectWidthoutMargin(rtWidget, mgWidget);
  }
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  m_rtCaption.Set(rtWidget.left, rtWidget.top, rtWidget.width, rtWidget.height);
  if (CXFA_Margin mgCap = caption.GetMargin()) {
    XFA_RectWidthoutMargin(m_rtCaption, mgCap);
  }
  LayoutHighlightCaption();
  SetFWLRect();
  if (m_pNormalWidget) {
    m_pNormalWidget->Update();
  }
  return TRUE;
}
FX_FLOAT CXFA_FFPushButton::GetLineWidth() {
  CXFA_Border border = m_pDataAcc->GetBorder();
  if (border && border.GetPresence() == XFA_ATTRIBUTEENUM_Visible) {
    CXFA_Edge edge = border.GetEdge(0);
    return edge.GetThickness();
  }
  return 0;
}
FX_ARGB CXFA_FFPushButton::GetLineColor() {
  return 0xFF000000;
}
FX_ARGB CXFA_FFPushButton::GetFillColor() {
  return 0xFFFFFFFF;
}
void CXFA_FFPushButton::LoadHighlightCaption() {
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  if (caption && caption.GetPresence() != XFA_ATTRIBUTEENUM_Hidden) {
    {
      CFX_WideString wsRollover;
      FX_BOOL bRichText;
      if (m_pDataAcc->GetButtonRollover(wsRollover, bRichText)) {
        if (!m_pRollProvider) {
          m_pRollProvider =
              new CXFA_TextProvider(m_pDataAcc, XFA_TEXTPROVIDERTYPE_Rollover);
        }
        m_pRolloverTextLayout = new CXFA_TextLayout(m_pRollProvider);
      }
      CFX_WideString wsDown;
      if (m_pDataAcc->GetButtonDown(wsDown, bRichText)) {
        if (!m_pDownProvider) {
          m_pDownProvider =
              new CXFA_TextProvider(m_pDataAcc, XFA_TEXTPROVIDERTYPE_Down);
        }
        m_pDownTextLayout = new CXFA_TextLayout(m_pDownProvider);
      }
    }
  }
}
void CXFA_FFPushButton::LayoutHighlightCaption() {
  CFX_SizeF sz(m_rtCaption.width, m_rtCaption.height);
  LayoutCaption();
  if (m_pRolloverTextLayout) {
    m_pRolloverTextLayout->Layout(sz);
  }
  if (m_pDownTextLayout) {
    m_pDownTextLayout->Layout(sz);
  }
}
void CXFA_FFPushButton::RenderHighlightCaption(CFX_Graphics* pGS,
                                               CFX_Matrix* pMatrix) {
  CXFA_TextLayout* pCapTextLayout = m_pDataAcc->GetCaptionTextLayout();
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  if (caption && caption.GetPresence() == XFA_ATTRIBUTEENUM_Visible) {
    CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
    CFX_RectF rtWidget;
    GetRectWithoutRotate(rtWidget);
    CFX_RectF rtClip = m_rtCaption;
    rtClip.Intersect(rtWidget);
    CFX_Matrix mt;
    mt.Set(1, 0, 0, 1, m_rtCaption.left, m_rtCaption.top);
    if (pMatrix) {
      pMatrix->TransformRect(rtClip);
      mt.Concat(*pMatrix);
    }
    {
      uint32_t dwState = m_pNormalWidget->GetStates();
      if (m_pDownTextLayout && (dwState & FWL_STATE_PSB_Pressed) &&
          (dwState & FWL_STATE_PSB_Hovered)) {
        if (m_pDownTextLayout->DrawString(pRenderDevice, mt, rtClip)) {
          return;
        }
      } else if (m_pRolloverTextLayout && (dwState & FWL_STATE_PSB_Hovered)) {
        if (m_pRolloverTextLayout->DrawString(pRenderDevice, mt, rtClip)) {
          return;
        }
      }
    }
    if (pCapTextLayout) {
      pCapTextLayout->DrawString(pRenderDevice, mt, rtClip);
    }
  }
}

void CXFA_FFPushButton::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFPushButton::OnProcessEvent(CFWL_Event* pEvent) {
  m_pOldDelegate->OnProcessEvent(pEvent);
  CXFA_FFField::OnProcessEvent(pEvent);
}

void CXFA_FFPushButton::OnDrawWidget(CFX_Graphics* pGraphics,
                                     const CFX_Matrix* pMatrix) {
  if (m_pNormalWidget->GetStylesEx() & XFA_FWL_PSBSTYLEEXT_HiliteInverted) {
    if ((m_pNormalWidget->GetStates() & FWL_STATE_PSB_Pressed) &&
        (m_pNormalWidget->GetStates() & FWL_STATE_PSB_Hovered)) {
      CFX_RectF rtFill;
      m_pNormalWidget->GetWidgetRect(rtFill);
      rtFill.left = rtFill.top = 0;
      FX_FLOAT fLineWith = GetLineWidth();
      rtFill.Deflate(fLineWith, fLineWith);
      CFX_Color cr(FXARGB_MAKE(128, 128, 255, 255));
      pGraphics->SetFillColor(&cr);
      CFX_Path path;
      path.Create();
      path.AddRectangle(rtFill.left, rtFill.top, rtFill.width, rtFill.height);
      pGraphics->FillPath(&path, FXFILL_WINDING, (CFX_Matrix*)pMatrix);
    }
  } else if (m_pNormalWidget->GetStylesEx() &
             XFA_FWL_PSBSTYLEEXT_HiliteOutLine) {
    if ((m_pNormalWidget->GetStates() & FWL_STATE_PSB_Pressed) &&
        (m_pNormalWidget->GetStates() & FWL_STATE_PSB_Hovered)) {
      FX_FLOAT fLineWidth = GetLineWidth();
      CFX_Color cr(FXARGB_MAKE(255, 128, 255, 255));
      pGraphics->SetStrokeColor(&cr);
      pGraphics->SetLineWidth(fLineWidth);
      CFX_Path path;
      path.Create();
      CFX_RectF rect;
      m_pNormalWidget->GetWidgetRect(rect);
      path.AddRectangle(0, 0, rect.width, rect.height);
      pGraphics->StrokePath(&path, (CFX_Matrix*)pMatrix);
    }
  }
}
