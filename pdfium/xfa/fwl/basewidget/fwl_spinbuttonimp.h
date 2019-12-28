// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_SPINBUTTONIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_SPINBUTTONIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_timer.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_MsgMouse;
class CFWL_SpinButtonImpDelegate;
class CFWL_WidgetImpProperties;

class CFWL_SpinButtonImp : public CFWL_WidgetImp, public IFWL_Timer {
 public:
  CFWL_SpinButtonImp(const CFWL_WidgetImpProperties& properties,
                     IFWL_Widget* pOuter);
  ~CFWL_SpinButtonImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy) override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

  // IFWL_Timer
  void Run(IFWL_TimerInfo* pTimerInfo) override;

  FWL_Error EnableButton(FX_BOOL bEnable, FX_BOOL bUp = TRUE);
  FX_BOOL IsButtonEnable(FX_BOOL bUp = TRUE);

 protected:
  friend class CFWL_SpinButtonImpDelegate;

  void DrawUpButton(CFX_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix);
  void DrawDownButton(CFX_Graphics* pGraphics,
                      IFWL_ThemeProvider* pTheme,
                      const CFX_Matrix* pMatrix);

  CFX_RectF m_rtClient;
  CFX_RectF m_rtUpButton;
  CFX_RectF m_rtDnButton;
  uint32_t m_dwUpState;
  uint32_t m_dwDnState;
  int32_t m_iButtonIndex;
  FX_BOOL m_bLButtonDwn;
  IFWL_TimerInfo* m_pTimerInfo;
};

class CFWL_SpinButtonImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_SpinButtonImpDelegate(CFWL_SpinButtonImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);
  void OnKeyDown(CFWL_MsgKey* pMsg);
  CFWL_SpinButtonImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_SPINBUTTONIMP_H_
