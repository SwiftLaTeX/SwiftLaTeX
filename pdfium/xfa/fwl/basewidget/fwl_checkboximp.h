// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_CHECKBOXIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_CHECKBOXIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_CheckBoxImpDelegate;
class CFWL_MsgMouse;
class CFWL_WidgetImpProperties;
class IFWL_Widget;

class CFWL_CheckBoxImp : public CFWL_WidgetImp {
 public:
  CFWL_CheckBoxImp(const CFWL_WidgetImpProperties& properties,
                   IFWL_Widget* pOuter);
  ~CFWL_CheckBoxImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  int32_t GetCheckState();
  FWL_Error SetCheckState(int32_t iCheck);

 protected:
  friend class CFWL_CheckBoxImpDelegate;

  void Layout();
  uint32_t GetPartStates();
  void UpdateTextOutStyles();
  void NextStates();

  CFX_RectF m_rtClient;
  CFX_RectF m_rtBox;
  CFX_RectF m_rtCaption;
  CFX_RectF m_rtFocus;
  uint32_t m_dwTTOStyles;
  int32_t m_iTTOAlign;
  FX_BOOL m_bBtnDown;
};

class CFWL_CheckBoxImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_CheckBoxImpDelegate(CFWL_CheckBoxImp* pOwner);

  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnActivate(CFWL_Message* pMsg);
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);
  void OnKeyDown(CFWL_MsgKey* pMsg);

  CFWL_CheckBoxImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_CHECKBOXIMP_H_
