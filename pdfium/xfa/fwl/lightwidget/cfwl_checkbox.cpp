// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_checkbox.h"

#include <memory>

#include "xfa/fwl/core/fwl_error.h"

CFWL_CheckBox* CFWL_CheckBox::Create() {
  return new CFWL_CheckBox;
}

FWL_Error CFWL_CheckBox::Initialize(const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_CheckBox> pCheckBox(IFWL_CheckBox::Create(
      m_pProperties->MakeWidgetImpProperties(&m_checkboxData), nullptr));
  FWL_Error ret = pCheckBox->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pCheckBox.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_CheckBox::SetCaption(const CFX_WideStringC& wsCaption) {
  m_checkboxData.m_wsCaption = wsCaption;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_CheckBox::SetBoxSize(FX_FLOAT fHeight) {
  m_checkboxData.m_fBoxHeight = fHeight;
  return FWL_Error::Succeeded;
}

int32_t CFWL_CheckBox::GetCheckState() {
  return static_cast<IFWL_CheckBox*>(m_pIface)->GetCheckState();
}

FWL_Error CFWL_CheckBox::SetCheckState(int32_t iCheck) {
  return static_cast<IFWL_CheckBox*>(m_pIface)->SetCheckState(iCheck);
}

CFWL_CheckBox::CFWL_CheckBox() {}

CFWL_CheckBox::~CFWL_CheckBox() {}

CFWL_CheckBox::CFWL_CheckBoxDP::CFWL_CheckBoxDP()
    : m_fBoxHeight(16.0f), m_wsCaption(L"Check box") {}

FWL_Error CFWL_CheckBox::CFWL_CheckBoxDP::GetCaption(
    IFWL_Widget* pWidget,
    CFX_WideString& wsCaption) {
  wsCaption = m_wsCaption;
  return FWL_Error::Succeeded;
}

FX_FLOAT CFWL_CheckBox::CFWL_CheckBoxDP::GetBoxSize(IFWL_Widget* pWidget) {
  return m_fBoxHeight;
}
