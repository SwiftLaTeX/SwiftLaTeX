// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_CHECKBOX_H_
#define XFA_FWL_BASEWIDGET_IFWL_CHECKBOX_H_

#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_CheckBox L"FWL_CHECKBOX"
#define FWL_STYLEEXT_CKB_Left (0L << 0)
#define FWL_STYLEEXT_CKB_Center (1L << 0)
#define FWL_STYLEEXT_CKB_Right (2L << 0)
#define FWL_STYLEEXT_CKB_Top (0L << 2)
#define FWL_STYLEEXT_CKB_VCenter (1L << 2)
#define FWL_STYLEEXT_CKB_Bottom (2L << 2)
#define FWL_STYLEEXT_CKB_LeftText (1L << 4)
#define FWL_STYLEEXT_CKB_MultiLine (1L << 5)
#define FWL_STYLEEXT_CKB_3State (1L << 6)
#define FWL_STYLEEXT_CKB_RadioButton (1L << 7)
#define FWL_STYLEEXT_CKB_ShapeSolidSquare (0L << 8)
#define FWL_STYLEEXT_CKB_ShapeSunkenSquare (1L << 8)
#define FWL_STYLEEXT_CKB_ShapeSolidCircle (2L << 8)
#define FWL_STYLEEXT_CKB_ShapeSunkenCircle (3L << 8)
#define FWL_STYLEEXT_CKB_SignShapeCheck (0L << 10)
#define FWL_STYLEEXT_CKB_SignShapeCircle (1L << 10)
#define FWL_STYLEEXT_CKB_SignShapeCross (2L << 10)
#define FWL_STYLEEXT_CKB_SignShapeDiamond (3L << 10)
#define FWL_STYLEEXT_CKB_SignShapeSquare (4L << 10)
#define FWL_STYLEEXT_CKB_SignShapeStar (5L << 10)
#define FWL_STYLEEXT_CKB_HLayoutMask (3L << 0)
#define FWL_STYLEEXT_CKB_VLayoutMask (3L << 2)
#define FWL_STYLEEXT_CKB_ShapeMask (3L << 8)
#define FWL_STYLEEXT_CKB_SignShapeMask (7L << 10)
#define FWL_STATE_CKB_Hovered (1 << FWL_WGTSTATE_MAX)
#define FWL_STATE_CKB_Pressed (1 << (FWL_WGTSTATE_MAX + 1))
#define FWL_STATE_CKB_Unchecked (0 << (FWL_WGTSTATE_MAX + 2))
#define FWL_STATE_CKB_Checked (1 << (FWL_WGTSTATE_MAX + 2))
#define FWL_STATE_CKB_Neutral (2 << (FWL_WGTSTATE_MAX + 2))
#define FWL_STATE_CKB_CheckMask (3L << (FWL_WGTSTATE_MAX + 2))

FWL_EVENT_DEF(CFWL_EvtCkbCheckStateChanged, CFWL_EventType::CheckStateChanged)

class IFWL_CheckBoxDP : public IFWL_DataProvider {
 public:
  virtual FX_FLOAT GetBoxSize(IFWL_Widget* pWidget) = 0;
};

class IFWL_CheckBox : public IFWL_Widget {
 public:
  static IFWL_CheckBox* Create(const CFWL_WidgetImpProperties& properties,
                               IFWL_Widget* pOuter);
  int32_t GetCheckState();
  FWL_Error SetCheckState(int32_t iCheck);

 protected:
  IFWL_CheckBox();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_CHECKBOX_H_
