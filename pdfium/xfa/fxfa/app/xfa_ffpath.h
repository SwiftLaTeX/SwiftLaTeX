// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFPATH_H_
#define XFA_FXFA_APP_XFA_FFPATH_H_

#include "xfa/fxfa/app/xfa_ffdraw.h"

class CXFA_FFLine : public CXFA_FFDraw {
 public:
  CXFA_FFLine(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFLine() override;

  // CXFA_FFWidget
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;

 private:
  void GetRectFromHand(CFX_RectF& rect, int32_t iHand, FX_FLOAT fLineWidth);
};
class CXFA_FFArc : public CXFA_FFDraw {
 public:
  CXFA_FFArc(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFArc() override;

  // CXFA_FFWidget
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
};

class CXFA_FFRectangle : public CXFA_FFDraw {
 public:
  CXFA_FFRectangle(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFRectangle() override;

  // CXFA_FFWidget
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
};

#endif  // XFA_FXFA_APP_XFA_FFPATH_H_
