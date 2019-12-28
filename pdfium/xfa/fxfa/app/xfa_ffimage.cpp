// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffimage.h"

#include "xfa/fxfa/app/xfa_ffdraw.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

CXFA_FFImage::CXFA_FFImage(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFDraw(pPageView, pDataAcc) {}
CXFA_FFImage::~CXFA_FFImage() {
  CXFA_FFImage::UnloadWidget();
}
FX_BOOL CXFA_FFImage::IsLoaded() {
  return !!GetDataAcc()->GetImageImage();
}
FX_BOOL CXFA_FFImage::LoadWidget() {
  if (GetDataAcc()->GetImageImage()) {
    return TRUE;
  }
  GetDataAcc()->LoadImageImage();
  return CXFA_FFDraw::LoadWidget();
}
void CXFA_FFImage::UnloadWidget() {
  GetDataAcc()->SetImageImage(nullptr);
}
void CXFA_FFImage::RenderWidget(CFX_Graphics* pGS,
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
  if (CFX_DIBitmap* pDIBitmap = GetDataAcc()->GetImageImage()) {
    CFX_RectF rtImage;
    GetRectWithoutRotate(rtImage);
    if (CXFA_Margin mgWidget = m_pDataAcc->GetMargin()) {
      XFA_RectWidthoutMargin(rtImage, mgWidget);
    }
    int32_t iHorzAlign = XFA_ATTRIBUTEENUM_Left;
    int32_t iVertAlign = XFA_ATTRIBUTEENUM_Top;
    if (CXFA_Para para = m_pDataAcc->GetPara()) {
      iHorzAlign = para.GetHorizontalAlign();
      iVertAlign = para.GetVerticalAlign();
    }
    CXFA_Value value = m_pDataAcc->GetFormValue();
    CXFA_Image imageObj = value.GetImage();
    int32_t iAspect = imageObj.GetAspect();
    int32_t iImageXDpi = 0;
    int32_t iImageYDpi = 0;
    m_pDataAcc->GetImageDpi(iImageXDpi, iImageYDpi);
    XFA_DrawImage(pGS, rtImage, &mtRotate, pDIBitmap, iAspect, iImageXDpi,
                  iImageYDpi, iHorzAlign, iVertAlign);
  }
}
