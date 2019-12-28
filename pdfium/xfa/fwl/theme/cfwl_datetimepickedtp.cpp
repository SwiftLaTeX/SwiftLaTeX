// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_datetimepickertp.h"

#include "xfa/fwl/basewidget/ifwl_datetimepicker.h"
#include "xfa/fwl/core/cfwl_themebackground.h"

CFWL_DateTimePickerTP::CFWL_DateTimePickerTP() {
  m_pThemeData = new DTPThemeData;
  initThemeData();
}

CFWL_DateTimePickerTP::~CFWL_DateTimePickerTP() {
  delete m_pThemeData;
}

bool CFWL_DateTimePickerTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::DateTimePicker;
}

FX_BOOL CFWL_DateTimePickerTP::DrawBackground(CFWL_ThemeBackground* pParams) {
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
    case CFWL_Part::DropDownButton: {
      DrawDropDownButton(pParams, &pParams->m_matrix);
      break;
    }
    default: {}
  }
  return TRUE;
}

void CFWL_DateTimePickerTP::DrawDropDownButton(CFWL_ThemeBackground* pParams,
                                               CFX_Matrix* pMatrix) {
  uint32_t dwStates = pParams->m_dwStates;
  dwStates &= 0x03;
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  switch (eState & dwStates) {
    case CFWL_PartState_Normal: {
      eState = FWLTHEME_STATE_Normal;
      break;
    }
    case CFWL_PartState_Hovered: {
      eState = FWLTHEME_STATE_Hover;
      break;
    }
    case CFWL_PartState_Pressed: {
      eState = FWLTHEME_STATE_Pressed;
      break;
    }
    case CFWL_PartState_Disabled: {
      eState = FWLTHEME_STATE_Disabale;
      break;
    }
    default: {}
  }
  DrawArrowBtn(pParams->m_pGraphics, &pParams->m_rtPart,
               FWLTHEME_DIRECTION_Down, eState, pMatrix);
}

void CFWL_DateTimePickerTP::initThemeData() {
  uint32_t* pData = (uint32_t*)&m_pThemeData->BoxBkColor;
  *pData++ = 0;
  *pData++ = 0;
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255);
}
