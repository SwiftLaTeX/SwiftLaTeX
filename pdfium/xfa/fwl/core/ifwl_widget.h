// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_WIDGET_H_
#define XFA_FWL_CORE_IFWL_WIDGET_H_

#include <memory>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/core/fwl_widgetimp.h"

// FWL contains three parallel inheritance hierarchies, which reference each
// other via pointers as follows:
//
//                              m_pAssociate
//                  <----------------------------------
//      CFWL_Widget ----------> IFWL_Widget ----------> CFWL_WidgetImp
//           |       m_pIface        |       m_pImpl         |
//           A                       A                       A
//           |                       |                       |
//      CFWL_...                IFWL_...                CFWL_...Imp
//
// TODO(tsepez): Collapse these into a single hierarchy.
//

enum class FWL_Type {
  Unknown = 0,

  Barcode,
  Caret,
  CheckBox,
  ComboBox,
  DateTimePicker,
  Edit,
  Form,
  FormProxy,
  ListBox,
  MonthCalendar,
  PictureBox,
  PushButton,
  ScrollBar,
  SpinButton,
  ToolTip
};

class CFWL_WidgetImp;
class CFX_Graphics;
class IFWL_App;
class IFWL_DataProvider;
class IFWL_Form;
class IFWL_ThemeProvider;
class IFWL_WidgetDelegate;

class IFWL_Widget {
 public:
  IFWL_Widget();
  virtual ~IFWL_Widget();

  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE);
  FWL_Error GetGlobalRect(CFX_RectF& rect);
  FWL_Error SetWidgetRect(const CFX_RectF& rect);
  FWL_Error GetClientRect(CFX_RectF& rect);
  IFWL_Widget* GetParent();
  FWL_Error SetParent(IFWL_Widget* pParent);
  IFWL_Widget* GetOwner();
  FWL_Error SetOwner(IFWL_Widget* pOwner);
  IFWL_Widget* GetOuter();
  uint32_t GetStyles();
  FWL_Error ModifyStyles(uint32_t dwStylesAdded, uint32_t dwStylesRemoved);
  uint32_t GetStylesEx();
  FWL_Error ModifyStylesEx(uint32_t dwStylesExAdded,
                           uint32_t dwStylesExRemoved);
  uint32_t GetStates();
  void SetStates(uint32_t dwStates, FX_BOOL bSet = TRUE);
  uint32_t GetEventKey() const;
  void SetEventKey(uint32_t key);
  void* GetLayoutItem() const;
  void SetLayoutItem(void* pItem);
  void* GetAssociateWidget() const;
  void SetAssociateWidget(void* pAssociate);
  FWL_Error Update();
  FWL_Error LockUpdate();
  FWL_Error UnlockUpdate();
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy);
  FWL_Error TransformTo(IFWL_Widget* pWidget, FX_FLOAT& fx, FX_FLOAT& fy);
  FWL_Error TransformTo(IFWL_Widget* pWidget, CFX_RectF& rt);
  FWL_Error GetMatrix(CFX_Matrix& matrix, FX_BOOL bGlobal = FALSE);
  FWL_Error SetMatrix(const CFX_Matrix& matrix);
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr);
  IFWL_ThemeProvider* GetThemeProvider();
  FWL_Error SetThemeProvider(IFWL_ThemeProvider* pThemeProvider);
  FWL_Error SetDataProvider(IFWL_DataProvider* pDataProvider);
  IFWL_WidgetDelegate* SetDelegate(IFWL_WidgetDelegate* pDelegate);
  IFWL_App* GetOwnerApp() const;
  CFX_SizeF GetOffsetFromParent(IFWL_Widget* pParent);

  // These call into equivalent polymorphic methods of m_pImpl. There
  // should be no need to override these in subclasses.
  FWL_Error GetClassName(CFX_WideString& wsClass) const;
  FWL_Type GetClassID() const;
  FX_BOOL IsInstance(const CFX_WideStringC& wsClass) const;
  FWL_Error Initialize();
  FWL_Error Finalize();

  CFWL_WidgetImp* GetImpl() const { return m_pImpl.get(); }

 protected:
  // Takes ownership of |pImpl|.
  void SetImpl(CFWL_WidgetImp* pImpl) { m_pImpl.reset(pImpl); }

 private:
  std::unique_ptr<CFWL_WidgetImp> m_pImpl;
};

#endif  // XFA_FWL_CORE_IFWL_WIDGET_H_
