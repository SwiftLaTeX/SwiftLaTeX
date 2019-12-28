// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFIMAGEEDIT_H_
#define XFA_FXFA_APP_XFA_FFIMAGEEDIT_H_

#include "xfa/fxfa/app/xfa_fffield.h"

class CXFA_FFImageEdit : public CXFA_FFField {
 public:
  CXFA_FFImageEdit(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFImageEdit() override;

  // CXFA_FFField
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
  FX_BOOL LoadWidget() override;
  void UnloadWidget() override;
  FX_BOOL OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void SetFWLRect() override;
  FX_BOOL UpdateFWLData() override;
  FX_BOOL CommitData() override;

  IFWL_WidgetDelegate* m_pOldDelegate;
};

#endif  // XFA_FXFA_APP_XFA_FFIMAGEEDIT_H_
