// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFPUSHBUTTON_H_
#define XFA_FXFA_APP_XFA_FFPUSHBUTTON_H_

#include "xfa/fxfa/app/xfa_fffield.h"

#define XFA_FWL_PSBSTYLEEXT_HiliteNone (0L << 0)
#define XFA_FWL_PSBSTYLEEXT_HiliteInverted (1L << 0)
#define XFA_FWL_PSBSTYLEEXT_HilitePush (2L << 0)
#define XFA_FWL_PSBSTYLEEXT_HiliteOutLine (4L << 0)

class CXFA_TextProvider;

class CXFA_FFPushButton : public CXFA_FFField {
 public:
  CXFA_FFPushButton(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFPushButton() override;

  // CXFA_FFField
  void RenderWidget(CFX_Graphics* pGS,
                    CFX_Matrix* pMatrix,
                    uint32_t dwStatus) override;
  FX_BOOL LoadWidget() override;
  void UnloadWidget() override;
  FX_BOOL PerformLayout() override;
  void UpdateWidgetProperty() override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void LoadHighlightCaption();
  void LayoutHighlightCaption();
  void RenderHighlightCaption(CFX_Graphics* pGS, CFX_Matrix* pMatrix = nullptr);
  FX_FLOAT GetLineWidth();
  FX_ARGB GetLineColor();
  FX_ARGB GetFillColor();

  CXFA_TextLayout* m_pRolloverTextLayout;
  CXFA_TextLayout* m_pDownTextLayout;
  CXFA_TextProvider* m_pDownProvider;
  CXFA_TextProvider* m_pRollProvider;
  IFWL_WidgetDelegate* m_pOldDelegate;
};

#endif  // XFA_FXFA_APP_XFA_FFPUSHBUTTON_H_
