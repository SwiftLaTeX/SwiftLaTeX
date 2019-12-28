// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_fffield.h"

#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/lightwidget/cfwl_edit.h"
#include "xfa/fwl/lightwidget/cfwl_picturebox.h"
#include "xfa/fxfa/app/xfa_fwltheme.h"
#include "xfa/fxfa/app/xfa_textlayout.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

CXFA_FFField::CXFA_FFField(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFWidget(pPageView, pDataAcc), m_pNormalWidget(nullptr) {
  m_rtUI.Set(0, 0, 0, 0);
  m_rtCaption.Set(0, 0, 0, 0);
}
CXFA_FFField::~CXFA_FFField() {
  CXFA_FFField::UnloadWidget();
}

FX_BOOL CXFA_FFField::GetBBox(CFX_RectF& rtBox,
                              uint32_t dwStatus,
                              FX_BOOL bDrawFocus) {
  if (!bDrawFocus)
    return CXFA_FFWidget::GetBBox(rtBox, dwStatus);

  XFA_Element type = m_pDataAcc->GetUIType();
  if (type == XFA_Element::Button || type == XFA_Element::CheckButton ||
      type == XFA_Element::ImageEdit || type == XFA_Element::Signature ||
      type == XFA_Element::ChoiceList) {
    rtBox = m_rtUI;
    CFX_Matrix mt;
    GetRotateMatrix(mt);
    mt.TransformRect(rtBox);
    return TRUE;
  }
  return FALSE;
}

void CXFA_FFField::RenderWidget(CFX_Graphics* pGS,
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
  CFX_RectF rtWidget;
  m_pNormalWidget->GetWidgetRect(rtWidget);
  CFX_Matrix mt;
  mt.Set(1, 0, 0, 1, rtWidget.left, rtWidget.top);
  mt.Concat(mtRotate);
  GetApp()->GetWidgetMgrDelegate()->OnDrawWidget(m_pNormalWidget->GetWidget(),
                                                 pGS, &mt);
}
void CXFA_FFField::DrawHighlight(CFX_Graphics* pGS,
                                 CFX_Matrix* pMatrix,
                                 uint32_t dwStatus,
                                 FX_BOOL bEllipse) {
  if (m_rtUI.IsEmpty() || !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return;
  }
  if ((dwStatus & XFA_WidgetStatus_Highlight) &&
      m_pDataAcc->GetAccess() == XFA_ATTRIBUTEENUM_Open) {
    CXFA_FFDoc* pDoc = GetDoc();
    CFX_Color crHighlight(pDoc->GetDocProvider()->GetHighlightColor(pDoc));
    pGS->SetFillColor(&crHighlight);
    CFX_Path path;
    path.Create();
    if (bEllipse) {
      path.AddEllipse(m_rtUI);
    } else {
      path.AddRectangle(m_rtUI.left, m_rtUI.top, m_rtUI.width, m_rtUI.height);
    }
    pGS->FillPath(&path, FXFILL_WINDING, pMatrix);
  }
}
void CXFA_FFField::DrawFocus(CFX_Graphics* pGS, CFX_Matrix* pMatrix) {
  if (m_dwStatus & XFA_WidgetStatus_Focused) {
    CFX_Color cr(0xFF000000);
    pGS->SetStrokeColor(&cr);
    FX_FLOAT DashPattern[2] = {1, 1};
    pGS->SetLineDash(0.0f, DashPattern, 2);
    pGS->SetLineWidth(0, FALSE);
    CFX_Path path;
    path.Create();
    path.AddRectangle(m_rtUI.left, m_rtUI.top, m_rtUI.width, m_rtUI.height);
    pGS->StrokePath(&path, pMatrix);
  }
}
void CXFA_FFField::SetFWLThemeProvider() {
  if (m_pNormalWidget) {
    m_pNormalWidget->m_pIface->SetThemeProvider(GetApp()->GetFWLTheme());
  }
}
FX_BOOL CXFA_FFField::IsLoaded() {
  return m_pNormalWidget && CXFA_FFWidget::IsLoaded();
}
FX_BOOL CXFA_FFField::LoadWidget() {
  SetFWLThemeProvider();
  m_pDataAcc->LoadCaption();
  PerformLayout();
  return TRUE;
}
void CXFA_FFField::UnloadWidget() {
  delete m_pNormalWidget;
  m_pNormalWidget = nullptr;
}
void CXFA_FFField::SetEditScrollOffset() {
  XFA_Element eType = m_pDataAcc->GetUIType();
  if (eType == XFA_Element::TextEdit || eType == XFA_Element::NumericEdit ||
      eType == XFA_Element::PasswordEdit) {
    FX_FLOAT fScrollOffset = 0;
    CXFA_FFField* pPrev = static_cast<CXFA_FFField*>(GetPrev());
    if (pPrev) {
      CFX_RectF rtMargin;
      m_pDataAcc->GetUIMargin(rtMargin);
      fScrollOffset = -rtMargin.top;
    }
    while (pPrev) {
      fScrollOffset += pPrev->m_rtUI.height;
      pPrev = static_cast<CXFA_FFField*>(pPrev->GetPrev());
    }
    ((CFWL_Edit*)m_pNormalWidget)->SetScrollOffset(fScrollOffset);
  }
}
FX_BOOL CXFA_FFField::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  CapPlacement();
  LayoutCaption();
  SetFWLRect();
  SetEditScrollOffset();
  if (m_pNormalWidget) {
    m_pNormalWidget->Update();
  }
  return TRUE;
}
void CXFA_FFField::CapPlacement() {
  CFX_RectF rtWidget;
  GetRectWithoutRotate(rtWidget);
  CXFA_Margin mgWidget = m_pDataAcc->GetMargin();
  if (mgWidget) {
    CXFA_LayoutItem* pItem = this;
    FX_FLOAT fLeftInset = 0, fRightInset = 0, fTopInset = 0, fBottomInset = 0;
    mgWidget.GetLeftInset(fLeftInset);
    mgWidget.GetRightInset(fRightInset);
    mgWidget.GetTopInset(fTopInset);
    mgWidget.GetBottomInset(fBottomInset);
    if (!pItem->GetPrev() && !pItem->GetNext()) {
      rtWidget.Deflate(fLeftInset, fTopInset, fRightInset, fBottomInset);
    } else {
      if (!pItem->GetPrev()) {
        rtWidget.Deflate(fLeftInset, fTopInset, fRightInset, 0);
      } else if (!pItem->GetNext()) {
        rtWidget.Deflate(fLeftInset, 0, fRightInset, fBottomInset);
      } else {
        rtWidget.Deflate(fLeftInset, 0, fRightInset, 0);
      }
    }
  }
  XFA_ATTRIBUTEENUM iCapPlacement = XFA_ATTRIBUTEENUM_Unknown;
  FX_FLOAT fCapReserve = 0;
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  if (caption && caption.GetPresence() != XFA_ATTRIBUTEENUM_Hidden) {
    iCapPlacement = (XFA_ATTRIBUTEENUM)caption.GetPlacementType();
    if (iCapPlacement == XFA_ATTRIBUTEENUM_Top && GetPrev()) {
      m_rtCaption.Set(0, 0, 0, 0);
    } else if (iCapPlacement == XFA_ATTRIBUTEENUM_Bottom && GetNext()) {
      m_rtCaption.Set(0, 0, 0, 0);
    } else {
      fCapReserve = caption.GetReserve();
      CXFA_LayoutItem* pItem = this;
      if (!pItem->GetPrev() && !pItem->GetNext()) {
        m_rtCaption.Set(rtWidget.left, rtWidget.top, rtWidget.width,
                        rtWidget.height);
      } else {
        pItem = pItem->GetFirst();
        pItem->GetRect(m_rtCaption);
        pItem = pItem->GetNext();
        while (pItem) {
          CFX_RectF rtRect;
          pItem->GetRect(rtRect);
          m_rtCaption.height += rtRect.Height();
          pItem = pItem->GetNext();
        }
        XFA_RectWidthoutMargin(m_rtCaption, mgWidget);
      }
      CXFA_TextLayout* pCapTextLayout = m_pDataAcc->GetCaptionTextLayout();
      if (fCapReserve <= 0 && pCapTextLayout) {
        CFX_SizeF size;
        CFX_SizeF minSize;
        CFX_SizeF maxSize;
        pCapTextLayout->CalcSize(minSize, maxSize, size);
        if (iCapPlacement == XFA_ATTRIBUTEENUM_Top ||
            iCapPlacement == XFA_ATTRIBUTEENUM_Bottom) {
          fCapReserve = size.y;
        } else {
          fCapReserve = size.x;
        }
      }
    }
  }
  m_rtUI = rtWidget;
  switch (iCapPlacement) {
    case XFA_ATTRIBUTEENUM_Left: {
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(caption, rtWidget, iCapPlacement);
      m_rtUI.width -= fCapReserve;
      m_rtUI.left += fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Top: {
      m_rtCaption.height = fCapReserve;
      CapTopBottomPlacement(caption, rtWidget, iCapPlacement);
      m_rtUI.top += fCapReserve;
      m_rtUI.height -= fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Right: {
      m_rtCaption.left = m_rtCaption.right() - fCapReserve;
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(caption, rtWidget, iCapPlacement);
      m_rtUI.width -= fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Bottom: {
      m_rtCaption.top = m_rtCaption.bottom() - fCapReserve;
      m_rtCaption.height = fCapReserve;
      CapTopBottomPlacement(caption, rtWidget, iCapPlacement);
      m_rtUI.height -= fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Inline:
      break;
    default:
      break;
  }
  CXFA_Border borderUI = m_pDataAcc->GetUIBorder();
  if (borderUI) {
    CXFA_Margin margin = borderUI.GetMargin();
    if (margin) {
      XFA_RectWidthoutMargin(m_rtUI, margin);
    }
  }
  m_rtUI.Normalize();
}
void CXFA_FFField::CapTopBottomPlacement(CXFA_Caption caption,
                                         const CFX_RectF& rtWidget,
                                         int32_t iCapPlacement) {
  CFX_RectF rtUIMargin;
  m_pDataAcc->GetUIMargin(rtUIMargin);
  m_rtCaption.left += rtUIMargin.left;
  if (CXFA_Margin mgCap = caption.GetMargin()) {
    XFA_RectWidthoutMargin(m_rtCaption, mgCap);
    if (m_rtCaption.height < 0) {
      m_rtCaption.top += m_rtCaption.height;
    }
  }
  FX_FLOAT fWidth = rtUIMargin.left + rtUIMargin.width;
  FX_FLOAT fHeight = m_rtCaption.height + rtUIMargin.top + rtUIMargin.height;
  if (fWidth > rtWidget.width) {
    m_rtUI.width += fWidth - rtWidget.width;
  }
  if (fHeight == XFA_DEFAULTUI_HEIGHT && m_rtUI.height < XFA_MINUI_HEIGHT) {
    m_rtUI.height = XFA_MINUI_HEIGHT;
    m_rtCaption.top += rtUIMargin.top + rtUIMargin.height;
  } else if (fHeight > rtWidget.height) {
    m_rtUI.height += fHeight - rtWidget.height;
    if (iCapPlacement == XFA_ATTRIBUTEENUM_Bottom) {
      m_rtCaption.top += fHeight - rtWidget.height;
    }
  }
}
void CXFA_FFField::CapLeftRightPlacement(CXFA_Caption caption,
                                         const CFX_RectF& rtWidget,
                                         int32_t iCapPlacement) {
  CFX_RectF rtUIMargin;
  m_pDataAcc->GetUIMargin(rtUIMargin);
  m_rtCaption.top += rtUIMargin.top;
  m_rtCaption.height -= rtUIMargin.top;
  if (CXFA_Margin mgCap = caption.GetMargin()) {
    XFA_RectWidthoutMargin(m_rtCaption, mgCap);
    if (m_rtCaption.height < 0) {
      m_rtCaption.top += m_rtCaption.height;
    }
  }
  FX_FLOAT fWidth = m_rtCaption.width + rtUIMargin.left + rtUIMargin.width;
  FX_FLOAT fHeight = rtUIMargin.top + rtUIMargin.height;
  if (fWidth > rtWidget.width) {
    m_rtUI.width += fWidth - rtWidget.width;
    if (iCapPlacement == XFA_ATTRIBUTEENUM_Right) {
      m_rtCaption.left += fWidth - rtWidget.width;
    }
  }
  if (fHeight == XFA_DEFAULTUI_HEIGHT && m_rtUI.height < XFA_MINUI_HEIGHT) {
    m_rtUI.height = XFA_MINUI_HEIGHT;
    m_rtCaption.top += rtUIMargin.top + rtUIMargin.height;
  } else if (fHeight > rtWidget.height) {
    m_rtUI.height += fHeight - rtWidget.height;
  }
}
void CXFA_FFField::UpdateFWL() {
  if (m_pNormalWidget) {
    m_pNormalWidget->Update();
  }
}
uint32_t CXFA_FFField::UpdateUIProperty() {
  CXFA_Node* pUiNode = m_pDataAcc->GetUIChild();
  uint32_t dwStyle = 0;
  if (pUiNode && pUiNode->GetElementType() == XFA_Element::DefaultUi) {
    dwStyle = FWL_STYLEEXT_EDT_ReadOnly;
  }
  return dwStyle;
}
void CXFA_FFField::SetFWLRect() {
  if (!m_pNormalWidget) {
    return;
  }
  CFX_RectF rtUi = m_rtUI;
  if (rtUi.width < 1.0)
    rtUi.width = 1.0;
  if (!m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    FX_FLOAT fFontSize = m_pDataAcc->GetFontSize();
    if (rtUi.height < fFontSize) {
      rtUi.height = fFontSize;
    }
  }
  m_pNormalWidget->SetWidgetRect(rtUi);
}
FX_BOOL CXFA_FFField::OnMouseEnter() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::Enter;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnMouseExit() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::Leave;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
void CXFA_FFField::FWLToClient(FX_FLOAT& fx, FX_FLOAT& fy) {
  if (!m_pNormalWidget) {
    return;
  }
  CFX_RectF rtWidget;
  m_pNormalWidget->GetWidgetRect(rtWidget);
  fx -= rtWidget.left;
  fy -= rtWidget.top;
}
FX_BOOL CXFA_FFField::OnLButtonDown(uint32_t dwFlags,
                                    FX_FLOAT fx,
                                    FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return FALSE;
  }
  if (!PtInActiveRect(fx, fy)) {
    return FALSE;
  }
  SetButtonDown(TRUE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnLButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  if (!IsButtonDown()) {
    return FALSE;
  }
  SetButtonDown(FALSE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonUp;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnLButtonDblClk(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDblClk;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnMouseMove(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::Move;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnMouseWheel(uint32_t dwFlags,
                                   int16_t zDelta,
                                   FX_FLOAT fx,
                                   FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouseWheel ms;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_fDeltaX = zDelta;
  ms.m_fDeltaY = 0;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnRButtonDown(uint32_t dwFlags,
                                    FX_FLOAT fx,
                                    FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open ||
      !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return FALSE;
  }
  if (!PtInActiveRect(fx, fy)) {
    return FALSE;
  }
  SetButtonDown(TRUE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::RightButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnRButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  if (!IsButtonDown()) {
    return FALSE;
  }
  SetButtonDown(FALSE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::RightButtonUp;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnRButtonDblClk(uint32_t dwFlags,
                                      FX_FLOAT fx,
                                      FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::RightButtonDblClk;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}

FX_BOOL CXFA_FFField::OnSetFocus(CXFA_FFWidget* pOldWidget) {
  CXFA_FFWidget::OnSetFocus(pOldWidget);
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFWL_MsgSetFocus ms;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  m_dwStatus |= XFA_WidgetStatus_Focused;
  AddInvalidateRect();
  return TRUE;
}
FX_BOOL CXFA_FFField::OnKillFocus(CXFA_FFWidget* pNewWidget) {
  if (!m_pNormalWidget) {
    return CXFA_FFWidget::OnKillFocus(pNewWidget);
  }
  CFWL_MsgKillFocus ms;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  m_dwStatus &= ~XFA_WidgetStatus_Focused;
  AddInvalidateRect();
  CXFA_FFWidget::OnKillFocus(pNewWidget);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) {
  if (!m_pNormalWidget || !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return FALSE;
  }
  CFWL_MsgKey ms;
  ms.m_dwCmd = FWL_KeyCommand::KeyDown;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwKeyCode;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) {
  if (!m_pNormalWidget || !m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return FALSE;
  }
  CFWL_MsgKey ms;
  ms.m_dwCmd = FWL_KeyCommand::KeyUp;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwKeyCode;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FX_BOOL CXFA_FFField::OnChar(uint32_t dwChar, uint32_t dwFlags) {
  if (!m_pDataAcc->GetDoc()->GetXFADoc()->IsInteractive()) {
    return FALSE;
  }
  if (dwChar == FWL_VKEY_Tab) {
    return TRUE;
  }
  if (!m_pNormalWidget) {
    return FALSE;
  }
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  CFWL_MsgKey ms;
  ms.m_dwCmd = FWL_KeyCommand::Char;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwChar;
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  ms.m_pSrcTarget = nullptr;
  TranslateFWLMessage(&ms);
  return TRUE;
}
FWL_WidgetHit CXFA_FFField::OnHitTest(FX_FLOAT fx, FX_FLOAT fy) {
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
  return FWL_WidgetHit::Border;
}
FX_BOOL CXFA_FFField::OnSetCursor(FX_FLOAT fx, FX_FLOAT fy) {
  return TRUE;
}
FX_BOOL CXFA_FFField::PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy) {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  CFX_RectF rtWidget;
  m_pNormalWidget->GetWidgetRect(rtWidget);
  if (rtWidget.Contains(fx, fy)) {
    return TRUE;
  }
  return FALSE;
}
void CXFA_FFField::LayoutCaption() {
  CXFA_TextLayout* pCapTextLayout = m_pDataAcc->GetCaptionTextLayout();
  if (!pCapTextLayout)
    return;

  FX_FLOAT fHeight = 0;
  pCapTextLayout->Layout(CFX_SizeF(m_rtCaption.width, m_rtCaption.height),
                         &fHeight);
  if (m_rtCaption.height < fHeight)
    m_rtCaption.height = fHeight;
}
void CXFA_FFField::RenderCaption(CFX_Graphics* pGS, CFX_Matrix* pMatrix) {
  CXFA_TextLayout* pCapTextLayout = m_pDataAcc->GetCaptionTextLayout();
  if (!pCapTextLayout) {
    return;
  }
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  if (caption && caption.GetPresence() == XFA_ATTRIBUTEENUM_Visible) {
    if (!pCapTextLayout->IsLoaded()) {
      pCapTextLayout->Layout(CFX_SizeF(m_rtCaption.width, m_rtCaption.height));
    }
    CFX_RectF rtWidget;
    GetRectWithoutRotate(rtWidget);
    CFX_RectF rtClip = m_rtCaption;
    rtClip.Intersect(rtWidget);
    CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
    CFX_Matrix mt;
    mt.Set(1, 0, 0, 1, m_rtCaption.left, m_rtCaption.top);
    if (pMatrix) {
      pMatrix->TransformRect(rtClip);
      mt.Concat(*pMatrix);
    }
    pCapTextLayout->DrawString(pRenderDevice, mt, rtClip);
  }
}
FX_BOOL CXFA_FFField::ProcessCommittedData() {
  if (m_pDataAcc->GetAccess() != XFA_ATTRIBUTEENUM_Open) {
    return FALSE;
  }
  if (!IsDataChanged()) {
    return FALSE;
  }
  if (CalculateOverride() != 1) {
    return FALSE;
  }
  if (!CommitData()) {
    return FALSE;
  }
  m_pDocView->SetChangeMark();
  m_pDocView->AddValidateWidget(m_pDataAcc);
  return TRUE;
}
int32_t CXFA_FFField::CalculateOverride() {
  CXFA_WidgetAcc* pAcc = m_pDataAcc->GetExclGroup();
  if (!pAcc) {
    return CalculateWidgetAcc(m_pDataAcc);
  }
  if (CalculateWidgetAcc(pAcc) == 0) {
    return 0;
  }
  CXFA_Node* pNode = pAcc->GetExclGroupFirstMember();
  if (!pNode) {
    return 1;
  }
  CXFA_WidgetAcc* pWidgetAcc = nullptr;
  while (pNode) {
    pWidgetAcc = static_cast<CXFA_WidgetAcc*>(pNode->GetWidgetData());
    if (!pWidgetAcc) {
      return 1;
    }
    if (CalculateWidgetAcc(pWidgetAcc) == 0) {
      return 0;
    }
    pNode = pWidgetAcc->GetExclGroupNextMember(pNode);
  }
  return 1;
}
int32_t CXFA_FFField::CalculateWidgetAcc(CXFA_WidgetAcc* pAcc) {
  CXFA_Calculate calc = pAcc->GetCalculate();
  if (!calc) {
    return 1;
  }
  XFA_VERSION version = pAcc->GetDoc()->GetXFADoc()->GetCurVersionMode();
  if (calc) {
    int32_t iOverride = calc.GetOverride();
    switch (iOverride) {
      case XFA_ATTRIBUTEENUM_Error: {
        if (version <= XFA_VERSION_204) {
          return 1;
        }
        IXFA_AppProvider* pAppProvider = GetApp()->GetAppProvider();
        if (pAppProvider) {
          CFX_WideString wsMessage;
          CFX_WideString wsWarning;
          pAppProvider->LoadString(XFA_IDS_NotModifyField, wsWarning);
          wsMessage += wsWarning;
          CFX_WideString wsTitle;
          pAppProvider->LoadString(XFA_IDS_CalcOverride, wsTitle);
          pAppProvider->MsgBox(wsMessage, wsTitle, XFA_MBICON_Warning,
                               XFA_MB_OK);
        }
      }
        return 0;
      case XFA_ATTRIBUTEENUM_Warning: {
        if (version <= XFA_VERSION_204) {
          CXFA_Script script = calc.GetScript();
          if (!script) {
            return 1;
          }
          CFX_WideString wsExpression;
          script.GetExpression(wsExpression);
          if (wsExpression.IsEmpty()) {
            return 1;
          }
        }
        if (pAcc->GetNode()->IsUserInteractive())
          return 1;

        IXFA_AppProvider* pAppProvider = GetApp()->GetAppProvider();
        if (pAppProvider) {
          CFX_WideString wsMessage;
          calc.GetMessageText(wsMessage);
          if (!wsMessage.IsEmpty()) {
            wsMessage += L"\r\n";
          }
          CFX_WideString wsWarning;
          pAppProvider->LoadString(XFA_IDS_ModifyField, wsWarning);
          wsMessage += wsWarning;
          CFX_WideString wsTitle;
          pAppProvider->LoadString(XFA_IDS_CalcOverride, wsTitle);
          if (pAppProvider->MsgBox(wsMessage, wsTitle, XFA_MBICON_Warning,
                                   XFA_MB_YesNo) == XFA_IDYes) {
            pAcc->GetNode()->SetFlag(XFA_NodeFlag_UserInteractive, false);
            return 1;
          }
        }
        return 0;
      }
      case XFA_ATTRIBUTEENUM_Ignore:
        return 0;
      case XFA_ATTRIBUTEENUM_Disabled:
        pAcc->GetNode()->SetFlag(XFA_NodeFlag_UserInteractive, false);
      default:
        return 1;
    }
  }
  return 1;
}
FX_BOOL CXFA_FFField::CommitData() {
  return FALSE;
}
FX_BOOL CXFA_FFField::IsDataChanged() {
  return FALSE;
}
void CXFA_FFField::TranslateFWLMessage(CFWL_Message* pMessage) {
  GetApp()->GetWidgetMgrDelegate()->OnProcessMessageToForm(pMessage);
}
void CXFA_FFField::OnProcessMessage(CFWL_Message* pMessage) {}

void CXFA_FFField::OnProcessEvent(CFWL_Event* pEvent) {
  switch (pEvent->GetClassID()) {
    case CFWL_EventType::Mouse: {
      CFWL_EvtMouse* event = (CFWL_EvtMouse*)pEvent;
      if (event->m_dwCmd == FWL_MouseCommand::Enter) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseEnter;
        eParam.m_pTarget = m_pDataAcc;
        m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_MouseEnter, &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::Leave) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseExit;
        eParam.m_pTarget = m_pDataAcc;
        m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_MouseExit, &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::LeftButtonDown) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseDown;
        eParam.m_pTarget = m_pDataAcc;
        m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_MouseDown, &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::LeftButtonUp) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseUp;
        eParam.m_pTarget = m_pDataAcc;
        m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_MouseUp, &eParam);
      }
      break;
    }
    case CFWL_EventType::Click: {
      CXFA_EventParam eParam;
      eParam.m_eType = XFA_EVENT_Click;
      eParam.m_pTarget = m_pDataAcc;
      m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Click, &eParam);
      break;
    }
    default:
      break;
  }
}

void CXFA_FFField::OnDrawWidget(CFX_Graphics* pGraphics,
                                const CFX_Matrix* pMatrix) {}
