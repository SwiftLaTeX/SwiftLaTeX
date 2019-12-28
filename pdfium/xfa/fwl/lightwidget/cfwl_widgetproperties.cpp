// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_widgetproperties.h"

#include "xfa/fwl/lightwidget/cfwl_widget.h"

CFWL_WidgetProperties::CFWL_WidgetProperties()
    : m_dwStyles(FWL_WGTSTYLE_Child),
      m_dwStyleExes(0),
      m_dwStates(0),
      m_pParent(nullptr),
      m_pOwner(nullptr) {
  m_rtWidget.Set(0, 0, 0, 0);
}

CFWL_WidgetProperties::~CFWL_WidgetProperties() {}

CFWL_WidgetProperties::CFWL_WidgetProperties(
    const CFWL_WidgetProperties& other) = default;

CFWL_WidgetImpProperties CFWL_WidgetProperties::MakeWidgetImpProperties(
    IFWL_DataProvider* pDataProvider) const {
  CFWL_WidgetImpProperties result;
  result.m_ctmOnParent = m_ctmOnParent;
  result.m_rtWidget = m_rtWidget;
  result.m_dwStyles = m_dwStyles;
  result.m_dwStyleExes = m_dwStyleExes;
  result.m_dwStates = m_dwStates;
  if (m_pParent)
    result.m_pParent = m_pParent->GetWidget();
  if (m_pOwner)
    result.m_pOwner = m_pOwner->GetWidget();
  result.m_pDataProvider = pDataProvider;
  return result;
}
