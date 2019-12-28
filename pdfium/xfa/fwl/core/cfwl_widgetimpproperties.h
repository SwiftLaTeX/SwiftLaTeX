// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_WIDGETIMPPROPERTIES_H_
#define XFA_FWL_CORE_CFWL_WIDGETIMPPROPERTIES_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_widgetdef.h"

class IFWL_DataProvider;
class IFWL_ThemeProvider;
class IFWL_Widget;

class CFWL_WidgetImpProperties {
 public:
  CFWL_WidgetImpProperties();
  ~CFWL_WidgetImpProperties();
  CFWL_WidgetImpProperties(const CFWL_WidgetImpProperties& other);

  CFX_Matrix m_ctmOnParent;
  CFX_RectF m_rtWidget;
  uint32_t m_dwStyles;
  uint32_t m_dwStyleExes;
  uint32_t m_dwStates;
  IFWL_ThemeProvider* m_pThemeProvider;
  IFWL_DataProvider* m_pDataProvider;
  IFWL_Widget* m_pParent;
  IFWL_Widget* m_pOwner;
};

inline CFWL_WidgetImpProperties::CFWL_WidgetImpProperties()
    : m_dwStyles(FWL_WGTSTYLE_Child),
      m_dwStyleExes(0),
      m_dwStates(0),
      m_pThemeProvider(nullptr),
      m_pDataProvider(nullptr),
      m_pParent(nullptr),
      m_pOwner(nullptr) {
  m_ctmOnParent.SetIdentity();
  m_rtWidget.Set(0, 0, 0, 0);
}

inline CFWL_WidgetImpProperties::~CFWL_WidgetImpProperties() {}

inline CFWL_WidgetImpProperties::CFWL_WidgetImpProperties(
    const CFWL_WidgetImpProperties& other) = default;

#endif  // XFA_FWL_CORE_CFWL_WIDGETIMPPROPERTIES_H_
