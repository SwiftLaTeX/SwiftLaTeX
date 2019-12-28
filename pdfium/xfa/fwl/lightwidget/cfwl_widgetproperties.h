// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_WIDGETPROPERTIES_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_WIDGETPROPERTIES_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

class CFWL_Widget;
class IFWL_DataProvider;

class CFWL_WidgetProperties {
 public:
  CFWL_WidgetProperties();
  ~CFWL_WidgetProperties();
  CFWL_WidgetProperties(const CFWL_WidgetProperties& other);

  CFWL_WidgetImpProperties MakeWidgetImpProperties(
      IFWL_DataProvider* pDataProvider) const;

  CFX_WideString m_wsWindowclass;
  CFX_Matrix m_ctmOnParent;
  CFX_RectF m_rtWidget;
  uint32_t m_dwStyles;
  uint32_t m_dwStyleExes;
  uint32_t m_dwStates;
  CFWL_Widget* m_pParent;
  CFWL_Widget* m_pOwner;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_WIDGETPROPERTIES_H_
