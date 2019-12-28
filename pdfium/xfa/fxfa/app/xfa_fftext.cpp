// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_fftext.h"

#include "xfa/fwl/core/fwl_widgetdef.h"
#include "xfa/fxfa/app/xfa_ffdraw.h"
#include "xfa/fxfa/app/xfa_textlayout.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

CXFA_FFText::CXFA_FFText(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFDraw(pPageView, pDataAcc) {}
CXFA_FFText::~CXFA_FFText() {}
void CXFA_FFText::RenderWidget(CFX_Graphics* pGS,
                               CFX_Matrix* pMatrix,
                               uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  {
    CFX_Matrix mtRotate;
    GetRotateMatrix(mtRotate);
    if (pMatrix) {
      mtRotate.Concat(*pMatrix);
    }
    CXFA_FFWidget::RenderWidget(pGS, &mtRotate, dwStatus);
    CXFA_TextLayout* pTextLayout = m_pDataAcc->GetTextLayout();
    if (pTextLayout) {
      CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
      CFX_RectF rtText;
      GetRectWithoutRotate(rtText);
      if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
        CXFA_LayoutItem* pItem = this;
        if (!pItem->GetPrev() && !pItem->GetNext()) {
          XFA_RectWidthoutMargin(rtText, mgWidget);
        } else {
          FX_FLOAT fLeftInset, fRightInset, fTopInset = 0, fBottomInset = 0;
          mgWidget.GetLeftInset(fLeftInset);
          mgWidget.GetRightInset(fRightInset);
          if (!pItem->GetPrev()) {
            mgWidget.GetTopInset(fTopInset);
          } else if (!pItem->GetNext()) {
            mgWidget.GetBottomInset(fBottomInset);
          }
          rtText.Deflate(fLeftInset, fTopInset, fRightInset, fBottomInset);
        }
      }
      CFX_Matrix mt;
      mt.Set(1, 0, 0, 1, rtText.left, rtText.top);
      CFX_RectF rtClip = rtText;
      mtRotate.TransformRect(rtClip);
      mt.Concat(mtRotate);
      pTextLayout->DrawString(pRenderDevice, mt, rtClip, GetIndex());
    }
  }
}
FX_BOOL CXFA_FFText::IsLoaded() {
  CXFA_TextLayout* pTextLayout = m_pDataAcc->GetTextLayout();
  return pTextLayout && !pTextLayout->m_bHasBlock;
}
FX_BOOL CXFA_FFText::PerformLayout() {
  CXFA_FFDraw::PerformLayout();
  CXFA_TextLayout* pTextLayout = m_pDataAcc->GetTextLayout();
  if (!pTextLayout) {
    return FALSE;
  }
  if (!pTextLayout->m_bHasBlock) {
    return TRUE;
  }
  pTextLayout->m_Blocks.RemoveAll();
  CXFA_LayoutItem* pItem = this;
  if (!pItem->GetPrev() && !pItem->GetNext()) {
    return TRUE;
  }
  pItem = pItem->GetFirst();
  while (pItem) {
    CFX_RectF rtText;
    pItem->GetRect(rtText);
    if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
      if (!pItem->GetPrev()) {
        FX_FLOAT fTopInset;
        mgWidget.GetTopInset(fTopInset);
        rtText.height -= fTopInset;
      } else if (!pItem->GetNext()) {
        FX_FLOAT fBottomInset;
        mgWidget.GetBottomInset(fBottomInset);
        rtText.height -= fBottomInset;
      }
    }
    pTextLayout->ItemBlocks(rtText, pItem->GetIndex());
    pItem = pItem->GetNext();
  }
  pTextLayout->m_bHasBlock = FALSE;
  return TRUE;
}
FX_BOOL CXFA_FFText::OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  CFX_RectF rtBox;
  GetRectWithoutRotate(rtBox);
  if (!rtBox.Contains(fx, fy)) {
    return FALSE;
  }
  const FX_WCHAR* wsURLContent = GetLinkURLAtPoint(fx, fy);
  if (!wsURLContent) {
    return FALSE;
  }
  SetButtonDown(TRUE);
  return TRUE;
}
FX_BOOL CXFA_FFText::OnMouseMove(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  CFX_RectF rtBox;
  GetRectWithoutRotate(rtBox);
  if (!rtBox.Contains(fx, fy)) {
    return FALSE;
  }
  const FX_WCHAR* wsURLContent = GetLinkURLAtPoint(fx, fy);
  if (!wsURLContent) {
    return FALSE;
  }
  return TRUE;
}
FX_BOOL CXFA_FFText::OnLButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) {
  if (!IsButtonDown()) {
    return FALSE;
  }
  SetButtonDown(FALSE);
  const FX_WCHAR* wsURLContent = GetLinkURLAtPoint(fx, fy);
  if (!wsURLContent) {
    return FALSE;
  }
  CXFA_FFDoc* pDoc = GetDoc();
  pDoc->GetDocProvider()->GotoURL(pDoc, wsURLContent, FALSE);
  return TRUE;
}
FWL_WidgetHit CXFA_FFText::OnHitTest(FX_FLOAT fx, FX_FLOAT fy) {
  CFX_RectF rtBox;
  GetRectWithoutRotate(rtBox);
  if (!rtBox.Contains(fx, fy))
    return FWL_WidgetHit::Unknown;
  if (!GetLinkURLAtPoint(fx, fy))
    return FWL_WidgetHit::Unknown;
  return FWL_WidgetHit::HyperLink;
}
const FX_WCHAR* CXFA_FFText::GetLinkURLAtPoint(FX_FLOAT fx, FX_FLOAT fy) {
  CXFA_TextLayout* pTextLayout = m_pDataAcc->GetTextLayout();
  if (!pTextLayout) {
    return nullptr;
  }
  FX_FLOAT x(fx), y(fy);
  FWLToClient(x, y);
  const CXFA_PieceLineArray* pPieceLines = pTextLayout->GetPieceLines();
  int32_t iCount = pPieceLines->GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CXFA_PieceLine* pPieceLine = pPieceLines->GetAt(i);
    int32_t iPieces = pPieceLine->m_textPieces.GetSize();
    for (int32_t j = 0; j < iPieces; j++) {
      XFA_TextPiece* pPiece = pPieceLine->m_textPieces.GetAt(j);
      if (pPiece->pLinkData && pPiece->rtPiece.Contains(x, y)) {
        return pPiece->pLinkData->GetLinkURL();
      }
    }
  }
  return nullptr;
}
void CXFA_FFText::FWLToClient(FX_FLOAT& fx, FX_FLOAT& fy) {
  CFX_RectF rtWidget;
  GetRectWithoutRotate(rtWidget);
  fx -= rtWidget.left;
  fy -= rtWidget.top;
}
