// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_edittp.h"

#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

CFWL_EditTP::CFWL_EditTP() {}
CFWL_EditTP::~CFWL_EditTP() {}

bool CFWL_EditTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::Edit;
}

FX_BOOL CFWL_EditTP::DrawBackground(CFWL_ThemeBackground* pParams) {
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
      if (pParams->m_pPath) {
        CFX_Graphics* pGraphics = pParams->m_pGraphics;
        pGraphics->SaveGraphState();
        CFX_Color crSelected(FWL_GetThemeColor(m_dwThemeID) == 0
                                 ? FWLTHEME_COLOR_BKSelected
                                 : FWLTHEME_COLOR_Green_BKSelected);
        pGraphics->SetFillColor(&crSelected);
        pGraphics->FillPath(pParams->m_pPath, FXFILL_WINDING,
                            &pParams->m_matrix);
        pGraphics->RestoreGraphState();
      } else {
        CFX_Path path;
        path.Create();
        path.AddRectangle(pParams->m_rtPart.left, pParams->m_rtPart.top,
                          pParams->m_rtPart.width, pParams->m_rtPart.height);
        CFX_Color cr(FWLTHEME_COLOR_Background);
        if (!pParams->m_bStaticBackground) {
          if (pParams->m_dwStates & CFWL_PartState_Disabled)
            cr.Set(FWLTHEME_COLOR_EDGERB1);
          else if (pParams->m_dwStates & CFWL_PartState_ReadOnly)
            cr.Set(ArgbEncode(255, 236, 233, 216));
          else
            cr.Set(0xFFFFFFFF);
        }
        pParams->m_pGraphics->SaveGraphState();
        pParams->m_pGraphics->SetFillColor(&cr);
        pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING,
                                       &pParams->m_matrix);
        pParams->m_pGraphics->RestoreGraphState();
      }
      break;
    }
    case CFWL_Part::CombTextLine: {
      FX_ARGB cr = 0xFF000000;
      FX_FLOAT fWidth = 1.0f;
      CFX_Color crLine(cr);
      pParams->m_pGraphics->SetStrokeColor(&crLine);
      pParams->m_pGraphics->SetLineWidth(fWidth);
      pParams->m_pGraphics->StrokePath(pParams->m_pPath, &pParams->m_matrix);
      break;
    }
    default: { break; }
  }
  return TRUE;
}
FWL_Error CFWL_EditTP::Initialize() {
  InitTTO();
  return CFWL_WidgetTP::Initialize();
}
FWL_Error CFWL_EditTP::Finalize() {
  FinalizeTTO();
  return CFWL_WidgetTP::Finalize();
}
