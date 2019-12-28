// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_CHECKBOX_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_CHECKBOX_H_

#include "xfa/fwl/basewidget/ifwl_checkbox.h"
#include "xfa/fwl/lightwidget/cfwl_widget.h"

class CFWL_CheckBox : public CFWL_Widget {
 public:
  CFWL_CheckBox();
  ~CFWL_CheckBox() override;

  static CFWL_CheckBox* Create();

  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
  FWL_Error SetCaption(const CFX_WideStringC& wsCaption);
  FWL_Error SetBoxSize(FX_FLOAT fHeight);
  int32_t GetCheckState();
  FWL_Error SetCheckState(int32_t iCheck);

 protected:
  class CFWL_CheckBoxDP : public IFWL_CheckBoxDP {
   public:
    CFWL_CheckBoxDP();

    // IFWL_DataProvider
    FWL_Error GetCaption(IFWL_Widget* pWidget,
                         CFX_WideString& wsCaption) override;

    // IFWL_CheckBoxDP
    FX_FLOAT GetBoxSize(IFWL_Widget* pWidget) override;

    FX_FLOAT m_fBoxHeight;
    CFX_WideString m_wsCaption;
  };

  CFWL_CheckBoxDP m_checkboxData;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_CHECKBOX_H_
