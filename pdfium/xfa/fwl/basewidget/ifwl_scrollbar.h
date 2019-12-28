// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_SCROLLBAR_H_
#define XFA_FWL_BASEWIDGET_IFWL_SCROLLBAR_H_

#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"

#define FWL_CLASS_ScrollBar L"FWL_SCROLLBAR"
#define FWL_STYLEEXT_SCB_Horz (0L << 0)
#define FWL_STYLEEXT_SCB_Vert (1L << 0)

enum FWL_SCBCODE {
  FWL_SCBCODE_None = 1,
  FWL_SCBCODE_Min,
  FWL_SCBCODE_Max,
  FWL_SCBCODE_PageBackward,
  FWL_SCBCODE_PageForward,
  FWL_SCBCODE_StepBackward,
  FWL_SCBCODE_StepForward,
  FWL_SCBCODE_Pos,
  FWL_SCBCODE_TrackPos,
  FWL_SCBCODE_EndScroll,
};

class IFWL_ScrollBarDP : public IFWL_DataProvider {};

class IFWL_ScrollBar : public IFWL_Widget {
 public:
  static IFWL_ScrollBar* Create(const CFWL_WidgetImpProperties& properties,
                                IFWL_Widget* pOuter);

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

 protected:
  IFWL_ScrollBar();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_SCROLLBAR_H_
