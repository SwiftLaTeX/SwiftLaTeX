// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_scrollbartp.h"

#include "xfa/fwl/basewidget/ifwl_scrollbar.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

namespace {

const float kPawLength = 12.5f;

}  // namespace

CFWL_ScrollBarTP::CFWL_ScrollBarTP() : m_pThemeData(new SBThemeData) {
  SetThemeData(0);
}

CFWL_ScrollBarTP::~CFWL_ScrollBarTP() {
  delete m_pThemeData;
}

bool CFWL_ScrollBarTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::ScrollBar;
}
void* CFWL_ScrollBarTP::GetCapacity(CFWL_ThemePart* pThemePart,
                                    CFWL_WidgetCapacity dwCapacity) {
  if (dwCapacity == CFWL_WidgetCapacity::Size) {
    m_fValue = 5;
    return &m_fValue;
  }
  return CFWL_WidgetTP::GetCapacity(pThemePart, dwCapacity);
}
uint32_t CFWL_ScrollBarTP::SetThemeID(IFWL_Widget* pWidget,
                                      uint32_t dwThemeID,
                                      FX_BOOL bChildren) {
  if (m_pThemeData) {
    SetThemeData(FWL_GetThemeColor(dwThemeID));
  }
  return CFWL_WidgetTP::SetThemeID(pWidget, dwThemeID, bChildren);
}
FX_BOOL CFWL_ScrollBarTP::DrawBackground(CFWL_ThemeBackground* pParams) {
  if (!pParams)
    return FALSE;
  IFWL_Widget* pWidget = pParams->m_pWidget;
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  if (pParams->m_dwStates & CFWL_PartState_Hovered)
    eState = FWLTHEME_STATE_Hover;
  else if (pParams->m_dwStates & CFWL_PartState_Pressed)
    eState = FWLTHEME_STATE_Pressed;
  else if (pParams->m_dwStates & CFWL_PartState_Disabled)
    eState = FWLTHEME_STATE_Disabale;
  CFX_Graphics* pGraphics = pParams->m_pGraphics;
  CFX_RectF* pRect = &pParams->m_rtPart;
  FX_BOOL bVert = pWidget->GetStylesEx();
  switch (pParams->m_iPart) {
    case CFWL_Part::ForeArrow: {
      DrawMaxMinBtn(pGraphics, pRect,
                    bVert ? FWLTHEME_DIRECTION_Up : FWLTHEME_DIRECTION_Left,
                    eState, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::BackArrow: {
      DrawMaxMinBtn(pGraphics, pRect,
                    bVert ? FWLTHEME_DIRECTION_Down : FWLTHEME_DIRECTION_Right,
                    eState, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::Thumb: {
      DrawThumbBtn(pGraphics, pRect, bVert, eState, TRUE, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::LowerTrack: {
      DrawTrack(pGraphics, pRect, bVert, eState, TRUE, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::UpperTrack: {
      DrawTrack(pGraphics, pRect, bVert, eState, FALSE, &pParams->m_matrix);
      break;
    }
    default: {}
  }
  return TRUE;
}

void CFWL_ScrollBarTP::DrawThumbBtn(CFX_Graphics* pGraphics,
                                    const CFX_RectF* pRect,
                                    FX_BOOL bVert,
                                    FWLTHEME_STATE eState,
                                    FX_BOOL bPawButton,
                                    CFX_Matrix* pMatrix) {
  if (eState < FWLTHEME_STATE_Normal || eState > FWLTHEME_STATE_Disabale) {
    return;
  }
  CFX_Path path;
  path.Create();
  CFX_RectF rect(*pRect);
  if (bVert) {
    rect.Deflate(1, 0);
    if (rect.IsEmpty(0.1f)) {
      return;
    }
    path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
    DrawAxialShading(pGraphics, rect.left, rect.top, rect.right(), rect.top,
                     m_pThemeData->clrBtnBK[eState - 1][0],
                     m_pThemeData->clrBtnBK[eState - 1][1], &path,
                     FXFILL_WINDING, pMatrix);
    CFX_Color rcStroke;
    rcStroke.Set(m_pThemeData->clrBtnBorder[eState - 1]);
    pGraphics->SaveGraphState();
    pGraphics->SetStrokeColor(&rcStroke);
    pGraphics->StrokePath(&path, pMatrix);
    pGraphics->RestoreGraphState();
  } else {
    rect.Deflate(0, 1);
    if (rect.IsEmpty(0.1f)) {
      return;
    }
    path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
    DrawAxialShading(pGraphics, rect.left, rect.top, rect.left, rect.bottom(),
                     m_pThemeData->clrBtnBK[eState - 1][0],
                     m_pThemeData->clrBtnBK[eState - 1][1], &path,
                     FXFILL_WINDING, pMatrix);
    CFX_Color rcStroke;
    rcStroke.Set(m_pThemeData->clrBtnBorder[eState - 1]);
    pGraphics->SaveGraphState();
    pGraphics->SetStrokeColor(&rcStroke);
    pGraphics->StrokePath(&path, pMatrix);
    pGraphics->RestoreGraphState();
  }
}
void CFWL_ScrollBarTP::DrawPaw(CFX_Graphics* pGraphics,
                               const CFX_RectF* pRect,
                               FX_BOOL bVert,
                               FWLTHEME_STATE eState,
                               CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  if (bVert) {
    FX_FLOAT fPawLen = kPawLength;
    if (pRect->width / 2 <= fPawLen) {
      fPawLen = (pRect->width - 6) / 2;
    }
    FX_FLOAT fX = pRect->left + pRect->width / 4;
    FX_FLOAT fY = pRect->top + pRect->height / 2;
    path.MoveTo(fX, fY - 4);
    path.LineTo(fX + fPawLen, fY - 4);
    path.MoveTo(fX, fY - 2);
    path.LineTo(fX + fPawLen, fY - 2);
    path.MoveTo(fX, fY);
    path.LineTo(fX + fPawLen, fY);
    path.MoveTo(fX, fY + 2);
    path.LineTo(fX + fPawLen, fY + 2);
    CFX_Color clrLight(m_pThemeData->clrPawColorLight[eState - 1]);
    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(&clrLight);
    pGraphics->StrokePath(&path);
    fX++;
    path.Clear();
    path.MoveTo(fX, fY - 3);
    path.LineTo(fX + fPawLen, fY - 3);
    path.MoveTo(fX, fY - 1);
    path.LineTo(fX + fPawLen, fY - 1);
    path.MoveTo(fX, fY + 1);
    path.LineTo(fX + fPawLen, fY + 1);
    path.MoveTo(fX, fY + 3);
    path.LineTo(fX + fPawLen, fY + 3);
    CFX_Color clrDark(m_pThemeData->clrPawColorDark[eState - 1]);
    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(&clrDark);
    pGraphics->StrokePath(&path, pMatrix);
  } else {
    FX_FLOAT fPawLen = kPawLength;
    if (pRect->height / 2 <= fPawLen) {
      fPawLen = (pRect->height - 6) / 2;
    }
    FX_FLOAT fX = pRect->left + pRect->width / 2;
    FX_FLOAT fY = pRect->top + pRect->height / 4;
    path.MoveTo(fX - 4, fY);
    path.LineTo(fX - 4, fY + fPawLen);
    path.MoveTo(fX - 2, fY);
    path.LineTo(fX - 2, fY + fPawLen);
    path.MoveTo(fX, fY);
    path.LineTo(fX, fY + fPawLen);
    path.MoveTo(fX + 2, fY);
    path.LineTo(fX + 2, fY + fPawLen);
    CFX_Color clrLight(m_pThemeData->clrPawColorLight[eState - 1]);
    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(&clrLight);
    pGraphics->StrokePath(&path, pMatrix);
    fY++;
    path.Clear();
    path.MoveTo(fX - 3, fY);
    path.LineTo(fX - 3, fY + fPawLen);
    path.MoveTo(fX - 1, fY);
    path.LineTo(fX - 1, fY + fPawLen);
    path.MoveTo(fX + 1, fY);
    path.LineTo(fX + 1, fY + fPawLen);
    path.MoveTo(fX + 3, fY);
    path.LineTo(fX + 3, fY + fPawLen);
    CFX_Color clrDark(m_pThemeData->clrPawColorDark[eState - 1]);
    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(&clrDark);
    pGraphics->StrokePath(&path, pMatrix);
  }
}
void CFWL_ScrollBarTP::DrawTrack(CFX_Graphics* pGraphics,
                                 const CFX_RectF* pRect,
                                 FX_BOOL bVert,
                                 FWLTHEME_STATE eState,
                                 FX_BOOL bLowerTrack,
                                 CFX_Matrix* pMatrix) {
  if (eState < FWLTHEME_STATE_Normal || eState > FWLTHEME_STATE_Disabale) {
    return;
  }
  pGraphics->SaveGraphState();
  CFX_Color colorLine(ArgbEncode(255, 238, 237, 229));
  CFX_Path path;
  path.Create();
  FX_FLOAT fRight = pRect->right();
  FX_FLOAT fBottom = pRect->bottom();
  if (bVert) {
    path.AddRectangle(pRect->left, pRect->top, 1, pRect->height);
    path.AddRectangle(fRight - 1, pRect->top, 1, pRect->height);
  } else {
    path.AddRectangle(pRect->left, pRect->top, pRect->width, 1);
    path.AddRectangle(pRect->left, fBottom - 1, pRect->width, 1);
  }
  pGraphics->SetFillColor(&colorLine);
  pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  path.Clear();
  path.AddRectangle(pRect->left + 1, pRect->top, pRect->width - 2,
                    pRect->height);
  FX_FLOAT x1 = bVert ? pRect->left + 1 : pRect->left;
  FX_FLOAT y1 = bVert ? pRect->top : pRect->top + 1;
  FX_FLOAT x2 = bVert ? fRight - 1 : pRect->left;
  FX_FLOAT y2 = bVert ? pRect->top : fBottom - 1;
  pGraphics->RestoreGraphState();
  DrawAxialShading(pGraphics, x1, y1, x2, y2, m_pThemeData->clrTrackBKStart,
                   m_pThemeData->clrTrackBKEnd, &path, FXFILL_WINDING, pMatrix);
}
void CFWL_ScrollBarTP::DrawMaxMinBtn(CFX_Graphics* pGraphics,
                                     const CFX_RectF* pRect,
                                     FWLTHEME_DIRECTION eDict,
                                     FWLTHEME_STATE eState,
                                     CFX_Matrix* pMatrix) {
  DrawTrack(pGraphics, pRect,
            eDict == FWLTHEME_DIRECTION_Up || eDict == FWLTHEME_DIRECTION_Down,
            eState, TRUE, pMatrix);
  CFX_RectF rtArrowBtn(*pRect);
  rtArrowBtn.Deflate(1, 1, 1, 1);
  DrawArrowBtn(pGraphics, &rtArrowBtn, eDict, eState, pMatrix);
}

void CFWL_ScrollBarTP::SetThemeData(uint32_t dwID) {
  m_pThemeData->clrPawColorLight[3] = ArgbEncode(0xff, 208, 223, 172);
  m_pThemeData->clrPawColorDark[3] = ArgbEncode(0xff, 140, 157, 115);
  m_pThemeData->clrBtnBK[3][0] = ArgbEncode(0xff, 164, 180, 139);
  m_pThemeData->clrBtnBK[3][1] = ArgbEncode(0xff, 141, 157, 115);
  m_pThemeData->clrBtnBorder[3] = ArgbEncode(0xff, 236, 233, 216);
  if (dwID) {
    m_pThemeData->clrPawColorLight[0] = ArgbEncode(0xff, 208, 223, 172);
    m_pThemeData->clrPawColorDark[0] = ArgbEncode(0xff, 140, 157, 115);
    m_pThemeData->clrBtnBK[0][0] = ArgbEncode(0xff, 162, 179, 141);
    m_pThemeData->clrBtnBK[0][1] = ArgbEncode(0xff, 149, 167, 117);
    m_pThemeData->clrBtnBorder[0] = ArgbEncode(0xff, 142, 153, 125);
    m_pThemeData->clrPawColorLight[1] = ArgbEncode(0xff, 235, 245, 212);
    m_pThemeData->clrPawColorDark[1] = ArgbEncode(0xff, 182, 198, 142);
    m_pThemeData->clrBtnBK[1][0] = ArgbEncode(0xff, 200, 213, 170);
    m_pThemeData->clrBtnBK[1][1] = ArgbEncode(0xff, 195, 208, 150);
    m_pThemeData->clrBtnBorder[1] = ArgbEncode(0xff, 189, 203, 150);
    m_pThemeData->clrPawColorLight[2] = ArgbEncode(0xff, 208, 223, 172);
    m_pThemeData->clrPawColorDark[2] = ArgbEncode(0xff, 140, 157, 115);
    m_pThemeData->clrBtnBK[2][0] = ArgbEncode(0xff, 164, 180, 139);
    m_pThemeData->clrBtnBK[2][1] = ArgbEncode(0xff, 141, 157, 115);
    m_pThemeData->clrBtnBorder[2] = ArgbEncode(0xff, 128, 146, 102);
    m_pThemeData->clrTrackBKStart = ArgbEncode(0xff, 243, 241, 236);
    m_pThemeData->clrTrackBKEnd = ArgbEncode(0xff, 254, 254, 251);
  } else {
    m_pThemeData->clrPawColorLight[0] = ArgbEncode(0xff, 238, 244, 254);
    m_pThemeData->clrPawColorDark[0] = ArgbEncode(0xff, 140, 176, 248);
    m_pThemeData->clrBtnBK[0][0] = ArgbEncode(0xff, 197, 213, 252);
    m_pThemeData->clrBtnBK[0][1] = ArgbEncode(0xff, 182, 205, 251);
    m_pThemeData->clrBtnBorder[0] = ArgbEncode(0xff, 148, 176, 221);
    m_pThemeData->clrPawColorLight[1] = ArgbEncode(0xff, 252, 253, 255);
    m_pThemeData->clrPawColorDark[1] = ArgbEncode(0xff, 156, 197, 255);
    m_pThemeData->clrBtnBK[1][0] = ArgbEncode(0xff, 216, 232, 255);
    m_pThemeData->clrBtnBK[1][1] = ArgbEncode(0xff, 204, 225, 255);
    m_pThemeData->clrBtnBorder[1] = ArgbEncode(0xff, 218, 230, 254);
    m_pThemeData->clrPawColorLight[2] = ArgbEncode(0xff, 207, 221, 253);
    m_pThemeData->clrPawColorDark[2] = ArgbEncode(0xff, 131, 158, 216);
    m_pThemeData->clrBtnBK[2][0] = ArgbEncode(0xff, 167, 190, 245);
    m_pThemeData->clrBtnBK[2][1] = ArgbEncode(0xff, 146, 179, 249);
    m_pThemeData->clrBtnBorder[2] = ArgbEncode(0xff, 124, 159, 211);
    m_pThemeData->clrTrackBKStart = ArgbEncode(0xff, 243, 241, 236);
    m_pThemeData->clrTrackBKEnd = ArgbEncode(0xff, 254, 254, 251);
  }
}
