// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFIMAGE_H_
#define XFA_FXFA_APP_XFA_FFIMAGE_H_

#include "xfa/fxfa/app/xfa_ffdraw.h"

class CXFA_FFImage : public CXFA_FFDraw {
 public:
  CXFA_FFImage(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFImage() override;

  // CXFA_FFWidget
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
  FX_BOOL IsLoaded() override;
  FX_BOOL LoadWidget() override;
  void UnloadWidget() override;
};

#endif  // XFA_FXFA_APP_XFA_FFIMAGE_H_
