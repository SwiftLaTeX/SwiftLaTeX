// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_WIDGET_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_WIDGET_H_

#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/lightwidget/cfwl_widgetproperties.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_Event;
class CFWL_Message;
class CFWL_Widget;
class CFWL_WidgetDelegate;
class CFWL_WidgetMgr;

class CFWL_Widget {
 public:
  virtual ~CFWL_Widget();

  IFWL_Widget* GetWidget();
  FWL_Error GetClassName(CFX_WideString& wsClass) const;
  FWL_Type GetClassID() const;
  virtual FX_BOOL IsInstance(const CFX_WideStringC& wsClass) const;

  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE);
  FWL_Error GetGlobalRect(CFX_RectF& rect);
  FWL_Error SetWidgetRect(const CFX_RectF& rect);
  FWL_Error GetClientRect(CFX_RectF& rtClient);
  CFWL_Widget* GetParent();
  FWL_Error SetParent(CFWL_Widget* pParent);
  CFWL_Widget* GetOwner();
  FWL_Error SetOwner(CFWL_Widget* pOwner);
  uint32_t GetStyles();
  FWL_Error ModifyStyles(uint32_t dwStylesAdded, uint32_t dwStylesRemoved);
  uint32_t GetStylesEx();
  FWL_Error ModifyStylesEx(uint32_t dwStylesExAdded,
                           uint32_t dwStylesExRemoved);
  uint32_t GetStates();
  void SetStates(uint32_t dwStates, FX_BOOL bSet = TRUE);
  void* GetLayoutItem() const;
  void SetLayoutItem(void* pItem);

  FWL_Error SetPrivateData(void* module_id,
                           void* pData,
                           PD_CALLBACK_FREEDATA callback);
  void* GetPrivateData(void* module_id);
  FWL_Error Update();
  FWL_Error LockUpdate();
  FWL_Error UnlockUpdate();
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy);
  FWL_Error TransformTo(CFWL_Widget* pWidget, FX_FLOAT& fx, FX_FLOAT& fy);
  FWL_Error TransformTo(CFWL_Widget* pWidget, CFX_RectF& rt);
  FWL_Error GetMatrix(CFX_Matrix& matrix, FX_BOOL bGlobal = FALSE);
  FWL_Error SetMatrix(const CFX_Matrix& matrix);
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr);
  IFWL_WidgetDelegate* SetDelegate(IFWL_WidgetDelegate* pDelegate);
  FWL_Error Repaint(const CFX_RectF* pRect = nullptr);
  FWL_Error SetFocus(FX_BOOL bFocus);
  FWL_Error SetGrab(FX_BOOL bSet);
  CFWL_Widget();

  void RegisterEventTarget(CFWL_Widget* pEventSource = nullptr,
                           uint32_t dwFilter = FWL_EVENT_ALL_MASK);
  void DispatchEvent(CFWL_Event* pEvent);
  CFX_SizeF CalcTextSize(const CFX_WideString& wsText,
                         FX_BOOL bMultiLine = FALSE,
                         int32_t iLineWidth = -1);
  IFWL_Widget* m_pIface;
  IFWL_WidgetDelegate* m_pDelegate;
  CFWL_WidgetMgr* m_pWidgetMgr;
  CFWL_WidgetProperties* m_pProperties;

 protected:
  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_WIDGET_H_
