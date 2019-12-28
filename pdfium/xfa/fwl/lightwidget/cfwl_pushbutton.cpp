// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_pushbutton.h"

#include <memory>

CFWL_PushButton* CFWL_PushButton::Create() {
  return new CFWL_PushButton;
}

FWL_Error CFWL_PushButton::Initialize(
    const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_PushButton> pPushButton(IFWL_PushButton::Create(
      m_pProperties->MakeWidgetImpProperties(&m_buttonData), nullptr));
  FWL_Error ret = pPushButton->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pPushButton.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_PushButton::GetCaption(CFX_WideString& wsCaption) {
  wsCaption = m_buttonData.m_wsCaption;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_PushButton::SetCaption(const CFX_WideStringC& wsCaption) {
  m_buttonData.m_wsCaption = wsCaption;
  return FWL_Error::Succeeded;
}

CFX_DIBitmap* CFWL_PushButton::GetPicture() {
  return m_buttonData.m_pBitmap;
}

FWL_Error CFWL_PushButton::SetPicture(CFX_DIBitmap* pBitmap) {
  m_buttonData.m_pBitmap = pBitmap;
  return FWL_Error::Succeeded;
}

CFWL_PushButton::CFWL_PushButton() {}

CFWL_PushButton::~CFWL_PushButton() {}

FWL_Error CFWL_PushButton::CFWL_PushButtonDP::GetCaption(
    IFWL_Widget* pWidget,
    CFX_WideString& wsCaption) {
  wsCaption = m_wsCaption;
  return FWL_Error::Succeeded;
}

CFX_DIBitmap* CFWL_PushButton::CFWL_PushButtonDP::GetPicture(
    IFWL_Widget* pWidget) {
  return m_pBitmap;
}
