// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffpath.h"

#include "xfa/fxfa/app/xfa_ffdraw.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

CXFA_FFLine::CXFA_FFLine(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFDraw(pPageView, pDataAcc) {}
CXFA_FFLine::~CXFA_FFLine() {}
void CXFA_FFLine::GetRectFromHand(CFX_RectF& rect,
                                  int32_t iHand,
                                  FX_FLOAT fLineWidth) {
  FX_FLOAT fHalfWidth = fLineWidth / 2.0f;
  if (rect.height < 1.0f) {
    switch (iHand) {
      case XFA_ATTRIBUTEENUM_Left:
        rect.top -= fHalfWidth;
        break;
      case XFA_ATTRIBUTEENUM_Right:
        rect.top += fHalfWidth;
    }
  } else if (rect.width < 1.0f) {
    switch (iHand) {
      case XFA_ATTRIBUTEENUM_Left:
        rect.left += fHalfWidth;
        break;
      case XFA_ATTRIBUTEENUM_Right:
        rect.left += fHalfWidth;
        break;
    }
  } else {
    switch (iHand) {
      case XFA_ATTRIBUTEENUM_Left:
        rect.Inflate(fHalfWidth, fHalfWidth);
        break;
      case XFA_ATTRIBUTEENUM_Right:
        rect.Deflate(fHalfWidth, fHalfWidth);
        break;
    }
  }
}
void CXFA_FFLine::RenderWidget(CFX_Graphics* pGS,
                               CFX_Matrix* pMatrix,
                               uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CXFA_Value value = m_pDataAcc->GetFormValue();
  if (!value) {
    return;
  }
  CXFA_Line lineObj = value.GetLine();
  FX_ARGB lineColor = 0xFF000000;
  int32_t iStrokeType = 0;
  FX_FLOAT fLineWidth = 1.0f;
  FX_BOOL bSlope = lineObj.GetSlop();
  int32_t iCap = 0;
  CXFA_Edge edge = lineObj.GetEdge();
  if (edge) {
    if (edge.GetPresence() != XFA_ATTRIBUTEENUM_Visible) {
      return;
    }
    lineColor = edge.GetColor();
    iStrokeType = edge.GetStrokeType();
    fLineWidth = edge.GetThickness();
    iCap = edge.GetCapType();
  }
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  CFX_RectF rtLine;
  GetRectWithoutRotate(rtLine);
  if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
    XFA_RectWidthoutMargin(rtLine, mgWidget);
  }
  GetRectFromHand(rtLine, lineObj.GetHand(), fLineWidth);
  CFX_Path linePath;
  linePath.Create();
  if (bSlope && rtLine.right() > 0.0f && rtLine.bottom() > 0.0f) {
    linePath.AddLine(rtLine.right(), rtLine.top, rtLine.left, rtLine.bottom());
  } else {
    linePath.AddLine(rtLine.left, rtLine.top, rtLine.right(), rtLine.bottom());
  }
  CFX_Color color(lineColor);
  pGS->SaveGraphState();
  pGS->SetLineWidth(fLineWidth, TRUE);
  XFA_StrokeTypeSetLineDash(pGS, iStrokeType, iCap);
  pGS->SetStrokeColor(&color);
  pGS->SetLineCap(XFA_LineCapToFXGE(iCap));
  pGS->StrokePath(&linePath, &mtRotate);
  pGS->RestoreGraphState();
}
CXFA_FFArc::CXFA_FFArc(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFDraw(pPageView, pDataAcc) {}
CXFA_FFArc::~CXFA_FFArc() {}
void CXFA_FFArc::RenderWidget(CFX_Graphics* pGS,
                              CFX_Matrix* pMatrix,
                              uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CXFA_Value value = m_pDataAcc->GetFormValue();
  if (!value) {
    return;
  }
  CXFA_Arc arcObj = value.GetArc();
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  CFX_RectF rtArc;
  GetRectWithoutRotate(rtArc);
  if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
    XFA_RectWidthoutMargin(rtArc, mgWidget);
  }
  DrawBorder(pGS, arcObj, rtArc, &mtRotate);
}
CXFA_FFRectangle::CXFA_FFRectangle(CXFA_FFPageView* pPageView,
                                   CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFDraw(pPageView, pDataAcc) {}
CXFA_FFRectangle::~CXFA_FFRectangle() {}
void CXFA_FFRectangle::RenderWidget(CFX_Graphics* pGS,
                                    CFX_Matrix* pMatrix,
                                    uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CXFA_Value value = m_pDataAcc->GetFormValue();
  if (!value) {
    return;
  }
  CXFA_Rectangle rtObj = value.GetRectangle();
  CFX_RectF rect;
  GetRectWithoutRotate(rect);
  if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
    XFA_RectWidthoutMargin(rect, mgWidget);
  }
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  DrawBorder(pGS, rtObj, rect, &mtRotate);
}
