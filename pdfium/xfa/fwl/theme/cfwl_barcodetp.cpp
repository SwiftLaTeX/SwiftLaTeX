// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_barcodetp.h"

#include "xfa/fwl/basewidget/ifwl_barcode.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/ifwl_widget.h"

CFWL_BarcodeTP::CFWL_BarcodeTP() {}

CFWL_BarcodeTP::~CFWL_BarcodeTP() {}

bool CFWL_BarcodeTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::Barcode;
}

FX_BOOL CFWL_BarcodeTP::DrawBackground(CFWL_ThemeBackground* pParams) {
  if (!pParams)
    return FALSE;
  switch (pParams->m_iPart) {
    case CFWL_Part::Border: {
      DrawBorder(pParams->m_pGraphics, &pParams->m_rtPart, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::Edge: {
      DrawEdge(pParams->m_pGraphics, pParams->m_pWidget->GetStyles(),
               &pParams->m_rtPart, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::Background: {
      FillBackground(pParams->m_pGraphics, &pParams->m_rtPart,
                     &pParams->m_matrix);
      break;
    }
    default: {}
  }
  return TRUE;
}
