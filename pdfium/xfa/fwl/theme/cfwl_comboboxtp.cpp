// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_comboboxtp.h"

#include "xfa/fwl/basewidget/ifwl_combobox.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

namespace {

const float kComboFormHandler = 8.0f;

}  // namespace

CFWL_ComboBoxTP::CFWL_ComboBoxTP() {
  m_dwThemeID = 0;
}

CFWL_ComboBoxTP::~CFWL_ComboBoxTP() {}

bool CFWL_ComboBoxTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::ComboBox;
}

FX_BOOL CFWL_ComboBoxTP::DrawBackground(CFWL_ThemeBackground* pParams) {
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
      CFX_Path path;
      path.Create();
      CFX_RectF& rect = pParams->m_rtPart;
      path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
      CFX_Color cr;
      switch (pParams->m_dwStates) {
        case CFWL_PartState_Selected:
          cr = FWLTHEME_COLOR_BKSelected;
          break;
        case CFWL_PartState_Disabled:
          cr = FWLTHEME_COLOR_EDGERB1;
          break;
        default:
          cr = 0xFFFFFFFF;
      }
      pParams->m_pGraphics->SaveGraphState();
      pParams->m_pGraphics->SetFillColor(&cr);
      pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, &pParams->m_matrix);
      pParams->m_pGraphics->RestoreGraphState();
      break;
    }
    case CFWL_Part::DropDownButton: {
      DrawDropDownButton(pParams, pParams->m_dwStates, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::StretchHandler: {
      DrawStrethHandler(pParams, 0, &pParams->m_matrix);
      break;
    }
    default: { return FALSE; }
  }
  return TRUE;
}
void CFWL_ComboBoxTP::DrawStrethHandler(CFWL_ThemeBackground* pParams,
                                        uint32_t dwStates,
                                        CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  path.AddRectangle(pParams->m_rtPart.left, pParams->m_rtPart.top,
                    pParams->m_rtPart.width - 1, pParams->m_rtPart.height);
  CFX_Color cr(ArgbEncode(0xff, 0xff, 0, 0));
  pParams->m_pGraphics->SetFillColor(&cr);
  pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, &pParams->m_matrix);
}
void* CFWL_ComboBoxTP::GetCapacity(CFWL_ThemePart* pThemePart,
                                   CFWL_WidgetCapacity dwCapacity) {
  if (dwCapacity == CFWL_WidgetCapacity::ComboFormHandler) {
    m_fValue = kComboFormHandler;
    return &m_fValue;
  }
  return CFWL_WidgetTP::GetCapacity(pThemePart, dwCapacity);
}

void CFWL_ComboBoxTP::DrawDropDownButton(CFWL_ThemeBackground* pParams,
                                         uint32_t dwStates,
                                         CFX_Matrix* pMatrix) {
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  switch (dwStates) {
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
               FWLTHEME_DIRECTION_Down, eState, &pParams->m_matrix);
}
