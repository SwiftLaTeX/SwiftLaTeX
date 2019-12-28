// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_CARETIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_CARETIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_timer.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"

class CFWL_WidgetImpProperties;
class IFWL_Widget;
class CFWL_CaretImpDelegate;

class CFWL_CaretImp : public CFWL_WidgetImp {
 public:
  CFWL_CaretImp(const CFWL_WidgetImpProperties& properties,
                IFWL_Widget* pOuter);
  ~CFWL_CaretImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

  void ShowCaret(FX_BOOL bFlag = TRUE);
  FWL_Error GetFrequency(uint32_t& elapse);
  FWL_Error SetFrequency(uint32_t elapse);
  FWL_Error SetColor(CFX_Color crFill);

 protected:
  friend class CFWL_CaretImpDelegate;
  friend class CFWL_CaretTimer;

  class CFWL_CaretTimer : public IFWL_Timer {
   public:
    explicit CFWL_CaretTimer(CFWL_CaretImp* pCaret);
    ~CFWL_CaretTimer() override {}
    void Run(IFWL_TimerInfo* hTimer) override;
    CFWL_CaretImp* const m_pCaret;
  };

  void DrawCaretBK(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);

  CFWL_CaretTimer* m_pTimer;
  IFWL_TimerInfo* m_pTimerInfo;
  uint32_t m_dwElapse;
  CFX_Color m_crFill;
  FX_BOOL m_bSetColor;
};

class CFWL_CaretImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_CaretImpDelegate(CFWL_CaretImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  CFWL_CaretImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_CARETIMP_H_
