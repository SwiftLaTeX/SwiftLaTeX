// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_SCROLLBARIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_SCROLLBARIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_timer.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_WidgetImpProperties;
class IFWL_Widget;
class CFWL_ScrollBarImpDelegate;

class CFWL_ScrollBarImp : public CFWL_WidgetImp, public IFWL_Timer {
 public:
  CFWL_ScrollBarImp(const CFWL_WidgetImpProperties& properties,
                    IFWL_Widget* pOuter);
  ~CFWL_ScrollBarImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

  // IFWL_Timer
  void Run(IFWL_TimerInfo* pTimerInfo) override;

  FX_BOOL IsVertical();
  FWL_Error GetRange(FX_FLOAT& fMin, FX_FLOAT& fMax);
  FWL_Error SetRange(FX_FLOAT fMin, FX_FLOAT fMax);
  FX_FLOAT GetPageSize();
  FWL_Error SetPageSize(FX_FLOAT fPageSize);
  FX_FLOAT GetStepSize();
  FWL_Error SetStepSize(FX_FLOAT fStepSize);
  FX_FLOAT GetPos();
  FWL_Error SetPos(FX_FLOAT fPos);
  FX_FLOAT GetTrackPos();
  FWL_Error SetTrackPos(FX_FLOAT fTrackPos);
  FX_BOOL DoScroll(uint32_t dwCode, FX_FLOAT fPos = 0.0f);
  FWL_Error SetOuter(IFWL_Widget* pOuter);

 protected:
  friend class CFWL_ScrollBarImpDelegate;

  void DrawTrack(CFX_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 FX_BOOL bLower = TRUE,
                 const CFX_Matrix* pMatrix = nullptr);
  void DrawArrowBtn(CFX_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    FX_BOOL bMinBtn = TRUE,
                    const CFX_Matrix* pMatrix = nullptr);
  void DrawThumb(CFX_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 const CFX_Matrix* pMatrix = nullptr);
  void Layout();
  void CalcButtonLen();
  void CalcMinButtonRect(CFX_RectF& rect);
  void CalcMaxButtonRect(CFX_RectF& rect);
  void CalcThumbButtonRect(CFX_RectF& rect);
  void CalcMinTrackRect(CFX_RectF& rect);
  void CalcMaxTrackRect(CFX_RectF& rect);
  FX_FLOAT GetTrackPointPos(FX_FLOAT fx, FX_FLOAT fy);
  void GetTrackRect(CFX_RectF& rect, FX_BOOL bLower = TRUE);
  FX_BOOL SendEvent();
  FX_BOOL OnScroll(uint32_t dwCode, FX_FLOAT fPos);

  IFWL_TimerInfo* m_pTimerInfo;
  FX_FLOAT m_fRangeMin;
  FX_FLOAT m_fRangeMax;
  FX_FLOAT m_fPageSize;
  FX_FLOAT m_fStepSize;
  FX_FLOAT m_fPos;
  FX_FLOAT m_fTrackPos;
  int32_t m_iMinButtonState;
  int32_t m_iMaxButtonState;
  int32_t m_iThumbButtonState;
  int32_t m_iMinTrackState;
  int32_t m_iMaxTrackState;
  FX_FLOAT m_fLastTrackPos;
  FX_FLOAT m_cpTrackPointX;
  FX_FLOAT m_cpTrackPointY;
  int32_t m_iMouseWheel;
  FX_BOOL m_bTrackMouseLeave;
  FX_BOOL m_bMouseHover;
  FX_BOOL m_bMouseDown;
  FX_BOOL m_bRepaintThumb;
  FX_FLOAT m_fButtonLen;
  FX_BOOL m_bMinSize;
  CFX_RectF m_rtClient;
  CFX_RectF m_rtThumb;
  CFX_RectF m_rtMinBtn;
  CFX_RectF m_rtMaxBtn;
  CFX_RectF m_rtMinTrack;
  CFX_RectF m_rtMaxTrack;
  FX_BOOL m_bCustomLayout;
  FX_FLOAT m_fMinThumb;
};

class CFWL_ScrollBarImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_ScrollBarImpDelegate(CFWL_ScrollBarImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  void OnLButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  void OnMouseMove(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy);
  void OnMouseLeave();
  void OnMouseWheel(FX_FLOAT fx,
                    FX_FLOAT fy,
                    uint32_t dwFlags,
                    FX_FLOAT fDeltaX,
                    FX_FLOAT fDeltaY);
  void DoMouseDown(int32_t iItem,
                   const CFX_RectF& rtItem,
                   int32_t& iState,
                   FX_FLOAT fx,
                   FX_FLOAT fy);
  void DoMouseUp(int32_t iItem,
                 const CFX_RectF& rtItem,
                 int32_t& iState,
                 FX_FLOAT fx,
                 FX_FLOAT fy);
  void DoMouseMove(int32_t iItem,
                   const CFX_RectF& rtItem,
                   int32_t& iState,
                   FX_FLOAT fx,
                   FX_FLOAT fy);
  void DoMouseLeave(int32_t iItem, const CFX_RectF& rtItem, int32_t& iState);
  void DoMouseHover(int32_t iItem, const CFX_RectF& rtItem, int32_t& iState);

  CFWL_ScrollBarImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_SCROLLBARIMP_H_
