// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_FWL_FORMIMP_H_
#define XFA_FWL_CORE_FWL_FORMIMP_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_form.h"

class CFWL_MsgMouse;
class CFWL_MsgClose;
class CFWL_MsgWindowMove;
class CFWL_NoteLoop;
class CFWL_WidgetImpProperties;
class IFWL_Widget;
class IFWL_ThemeProvider;
class CFWL_SysBtn;
class CFWL_FormImp;
class CFWL_FormImpDelegate;

#define FWL_SYSBUTTONSTATE_Hover 0x0001
#define FWL_SYSBUTTONSTATE_Pressed 0x0002
#define FWL_SYSBUTTONSTATE_Disabled 0x0010

class CFWL_SysBtn {
 public:
  CFWL_SysBtn();

  bool IsDisabled() const;
  uint32_t GetPartState() const;

  void SetNormal();
  void SetPressed();
  void SetHover();
  void SetDisabled(FX_BOOL bDisabled);

  CFX_RectF m_rtBtn;
  uint32_t m_dwState;
};

enum FORM_RESIZETYPE {
  FORM_RESIZETYPE_None = 0,
  FORM_RESIZETYPE_Cap,
};

typedef struct RestoreResizeInfo {
  RestoreResizeInfo();
  ~RestoreResizeInfo();

  CFX_PointF m_ptStart;
  CFX_SizeF m_szStart;
} RestoreInfo;

class CFWL_FormImp : public CFWL_WidgetImp {
 public:
  CFWL_FormImp(const CFWL_WidgetImpProperties& properties, IFWL_Widget* pOuter);
  ~CFWL_FormImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FX_BOOL IsInstance(const CFX_WideStringC& wsClass) const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;

  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error GetClientRect(CFX_RectF& rect) override;
  FWL_Error Update() override;
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy) override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;

  FWL_FORMSIZE GetFormSize();
  FWL_Error SetFormSize(FWL_FORMSIZE eFormSize);
  IFWL_Widget* DoModal();
  IFWL_Widget* DoModal(uint32_t& dwCommandID);
  FWL_Error EndDoModal();
  FWL_Error SetBorderRegion(CFX_Path* pPath);
  void DrawBackground(CFX_Graphics* pGraphics, IFWL_ThemeProvider* pTheme);
  CFWL_WidgetImp* GetSubFocus();
  void SetSubFocus(CFWL_WidgetImp* pWidget);

 protected:
  void ShowChildWidget(IFWL_Widget* pParent);
  void RemoveSysButtons();
  void CalcContentRect(CFX_RectF& rtContent);
  CFWL_SysBtn* GetSysBtnAtPoint(FX_FLOAT fx, FX_FLOAT fy);
  CFWL_SysBtn* GetSysBtnByState(uint32_t dwState);
  CFWL_SysBtn* GetSysBtnByIndex(int32_t nIndex);
  int32_t GetSysBtnIndex(CFWL_SysBtn* pBtn);
  FX_FLOAT GetCaptionHeight();
  void DrawCaptionText(CFX_Graphics* pGs,
                       IFWL_ThemeProvider* pTheme,
                       const CFX_Matrix* pMatrix = nullptr);
  void DrawIconImage(CFX_Graphics* pGs,
                     IFWL_ThemeProvider* pTheme,
                     const CFX_Matrix* pMatrix = nullptr);
  void GetEdgeRect(CFX_RectF& rtEdge);
  void SetWorkAreaRect();
  void SetCursor(FX_FLOAT fx, FX_FLOAT fy);
  void Layout();
  void ReSetSysBtn();
  void RegisterForm();
  void UnRegisterForm();
  FX_BOOL IsDoModal();
  void SetThemeData();
  FX_BOOL HasIcon();
  void UpdateIcon();
  void UpdateCaption();
  void DoWidthLimit(FX_FLOAT& fLeft,
                    FX_FLOAT& fWidth,
                    FX_FLOAT fCurX,
                    FX_FLOAT fSpace,
                    FX_FLOAT fLimitMin,
                    FX_FLOAT fLimitMax,
                    FX_BOOL bLeft);
  void DoHeightLimit(FX_FLOAT& fTop,
                     FX_FLOAT& fHeight,
                     FX_FLOAT fCurY,
                     FX_FLOAT fSpace,
                     FX_FLOAT fLimitMin,
                     FX_FLOAT fLimitMax,
                     FX_BOOL bTop);

  CFX_RectF m_rtRestore;
  CFX_RectF m_rtCaptionText;
  CFX_RectF m_rtRelative;
  CFX_RectF m_rtCaption;
  CFX_RectF m_rtIcon;
  CFWL_SysBtn* m_pCloseBox;
  CFWL_SysBtn* m_pMinBox;
  CFWL_SysBtn* m_pMaxBox;
  CFWL_SysBtn* m_pCaptionBox;
  CFWL_NoteLoop* m_pNoteLoop;
  CFWL_WidgetImp* m_pSubFocus;
  RestoreInfo m_InfoStart;
  FX_FLOAT m_fCXBorder;
  FX_FLOAT m_fCYBorder;
  int32_t m_iCaptureBtn;
  int32_t m_iSysBox;
  int32_t m_eResizeType;
  FX_BOOL m_bLButtonDown;
  bool m_bMaximized;
  FX_BOOL m_bSetMaximize;
  FX_BOOL m_bCustomizeLayout;
  FWL_FORMSIZE m_eFormSize;
  FX_BOOL m_bDoModalFlag;
  FX_FLOAT m_fSmallIconSz;
  FX_FLOAT m_fBigIconSz;
  CFX_DIBitmap* m_pBigIcon;
  CFX_DIBitmap* m_pSmallIcon;
  FX_BOOL m_bMouseIn;
  friend class CFWL_FormImpDelegate;
};

class CFWL_FormImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_FormImpDelegate(CFWL_FormImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseHover(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);
  void OnLButtonDblClk(CFWL_MsgMouse* pMsg);
  void OnWindowMove(CFWL_MsgWindowMove* pMsg);
  void OnClose(CFWL_MsgClose* pMsg);
  CFWL_FormImp* m_pOwner;
};

#endif  // XFA_FWL_CORE_FWL_FORMIMP_H_
