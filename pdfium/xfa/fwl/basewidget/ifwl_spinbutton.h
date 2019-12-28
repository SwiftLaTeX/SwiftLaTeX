// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_SPINBUTTON_H_
#define XFA_FWL_BASEWIDGET_IFWL_SPINBUTTON_H_

#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"

#define FWL_CLASS_SpinButton L"FWL_SPINBUTTON"
#define FWL_STYLEEXE_SPB_Vert (1L << 0)

FWL_EVENT_DEF(CFWL_EvtSpbClick, CFWL_EventType::Click, FX_BOOL m_bUp;)

class IFWL_SpinButton : public IFWL_Widget {
 public:
  static IFWL_SpinButton* Create(const CFWL_WidgetImpProperties& properties,
                                 IFWL_Widget* pOuter);

  FWL_Error EnableButton(FX_BOOL bEnable, FX_BOOL bUp = TRUE);
  FX_BOOL IsButtonEnable(FX_BOOL bUp = TRUE);

 protected:
  IFWL_SpinButton();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_SPINBUTTON_H_
