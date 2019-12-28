// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_PUSHBUTTON_H_
#define XFA_FWL_BASEWIDGET_IFWL_PUSHBUTTON_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"
#include "xfa/fwl/core/ifwl_widget.h"

#define FWL_CLASS_PushButton L"FWL_PUSHBUTTON"
#define FWL_STYLEEXT_PSB_Left (0L << 0)
#define FWL_STYLEEXT_PSB_Center (1L << 0)
#define FWL_STYLEEXT_PSB_Right (2L << 0)
#define FWL_STYLEEXT_PSB_Top (0L << 2)
#define FWL_STYLEEXT_PSB_VCenter (1L << 2)
#define FWL_STYLEEXT_PSB_Bottom (2L << 2)
#define FWL_STYLEEXT_PSB_TextOnly (0L << 4)
#define FWL_STYLEEXT_PSB_IconOnly (1L << 4)
#define FWL_STYLEEXT_PSB_TextIcon (2L << 4)
#define FWL_STYLEEXT_PSB_HLayoutMask (3L << 0)
#define FWL_STYLEEXT_PSB_VLayoutMask (3L << 2)
#define FWL_STYLEEXT_PSB_ModeMask (3L << 4)
#define FWL_STATE_PSB_Hovered (1 << FWL_WGTSTATE_MAX)
#define FWL_STATE_PSB_Pressed (1 << (FWL_WGTSTATE_MAX + 1))
#define FWL_STATE_PSB_Default (1 << (FWL_WGTSTATE_MAX + 2))

class CFX_DIBitmap;

class IFWL_PushButtonDP : public IFWL_DataProvider {
 public:
  virtual CFX_DIBitmap* GetPicture(IFWL_Widget* pWidget) = 0;
};

class IFWL_PushButton : public IFWL_Widget {
 public:
  static IFWL_PushButton* Create(const CFWL_WidgetImpProperties& properties,
                                 IFWL_Widget* pOuter);

 protected:
  IFWL_PushButton();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_PUSHBUTTON_H_
