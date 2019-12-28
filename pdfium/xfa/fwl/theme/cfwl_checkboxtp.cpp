// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_checkboxtp.h"

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_checkbox.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

namespace {

const int kSignMargin = 3;
const int kSignBorder = 2;
const int kSignPath = 100;

}  // namespace

#define CHECKBOX_COLOR_BOXLT1 (ArgbEncode(255, 172, 168, 153))
#define CHECKBOX_COLOR_BOXLT2 (ArgbEncode(255, 113, 111, 100))
#define CHECKBOX_COLOR_BOXRB1 (ArgbEncode(255, 241, 239, 226))
#define CHECKBOX_COLOR_BOXRB2 (ArgbEncode(255, 255, 255, 255))

CFWL_CheckBoxTP::CFWL_CheckBoxTP()
    : m_pThemeData(new CKBThemeData), m_pCheckPath(nullptr) {
  SetThemeData(0);
}

CFWL_CheckBoxTP::~CFWL_CheckBoxTP() {
  delete m_pThemeData;
  if (m_pCheckPath) {
    m_pCheckPath->Clear();
    delete m_pCheckPath;
  }
}

bool CFWL_CheckBoxTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::CheckBox;
}

uint32_t CFWL_CheckBoxTP::SetThemeID(IFWL_Widget* pWidget,
                                     uint32_t dwThemeID,
                                     FX_BOOL bChildren) {
  if (m_pThemeData) {
    SetThemeData(FWL_GetThemeColor(dwThemeID));
  }
  return CFWL_WidgetTP::SetThemeID(pWidget, dwThemeID, bChildren);
}
FX_BOOL CFWL_CheckBoxTP::DrawText(CFWL_ThemeText* pParams) {
  if (!m_pTextOut)
    return FALSE;
  m_pTextOut->SetTextColor(pParams->m_dwStates & CFWL_PartState_Disabled
                               ? FWLTHEME_CAPACITY_TextDisColor
                               : FWLTHEME_CAPACITY_TextColor);
  return CFWL_WidgetTP::DrawText(pParams);
}
FX_BOOL CFWL_CheckBoxTP::DrawBackground(CFWL_ThemeBackground* pParams) {
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
      if (pParams->m_dwStates & CFWL_PartState_Focused) {
        pParams->m_rtPart = *(CFX_RectF*)pParams->m_pData;
        DrawFocus(pParams->m_pGraphics, &pParams->m_rtPart, &pParams->m_matrix);
      }
      break;
    }
    case CFWL_Part::CheckBox: {
      DrawBoxBk(pParams->m_pWidget, pParams->m_pGraphics, &pParams->m_rtPart,
                pParams->m_dwStates, &pParams->m_matrix);
      if ((pParams->m_dwStates & CFWL_PartState_Checked) |
          (pParams->m_dwStates & CFWL_PartState_Neutral)) {
        DrawSign(pParams->m_pWidget, pParams->m_pGraphics, &pParams->m_rtPart,
                 pParams->m_dwStates, &pParams->m_matrix);
      }
      DrawSignBorder(
          pParams->m_pWidget, pParams->m_pGraphics, &pParams->m_rtPart,
          pParams->m_dwStates & CFWL_PartState_Disabled, &pParams->m_matrix);
      break;
    }
    default: { return FALSE; }
  }
  return TRUE;
}
FWL_Error CFWL_CheckBoxTP::Initialize() {
  InitTTO();
  return CFWL_WidgetTP::Initialize();
}
FWL_Error CFWL_CheckBoxTP::Finalize() {
  FinalizeTTO();
  return CFWL_WidgetTP::Finalize();
}
void CFWL_CheckBoxTP::DrawBoxBk(IFWL_Widget* pWidget,
                                CFX_Graphics* pGraphics,
                                const CFX_RectF* pRect,
                                uint32_t dwStates,
                                CFX_Matrix* pMatrix) {
  dwStates &= 0x03;
  int32_t fillMode = FXFILL_WINDING;
  uint32_t dwStyleEx = pWidget->GetStylesEx();
  dwStyleEx &= FWL_STYLEEXT_CKB_ShapeMask;
  CFX_Path path;
  path.Create();
  FX_FLOAT fRight = pRect->right();
  FX_FLOAT fBottom = pRect->bottom();
  bool bClipSign = !!(dwStates & CFWL_PartState_Hovered);
  if ((dwStyleEx == FWL_STYLEEXT_CKB_ShapeSolidSquare) ||
      (dwStyleEx == FWL_STYLEEXT_CKB_ShapeSunkenSquare)) {
    path.AddRectangle(pRect->left, pRect->top, pRect->width, pRect->height);
    if (bClipSign) {
      fillMode = FXFILL_ALTERNATE;
      path.AddRectangle(pRect->left + kSignMargin, pRect->top + kSignMargin,
                        pRect->width - kSignMargin * 2,
                        pRect->height - kSignMargin * 2);
    }
  } else {
    CFX_RectF rect(*pRect);
    rect.Deflate(0, 0, 1, 1);
    path.AddEllipse(rect);
    if (bClipSign) {
      fillMode = FXFILL_ALTERNATE;
      CFX_RectF rtClip(rect);
      rtClip.Deflate(kSignMargin - 1, kSignMargin - 1);
      path.AddEllipse(rtClip);
    }
  }
  int32_t iTheme = 1;
  if (dwStates & CFWL_PartState_Hovered) {
    iTheme = 2;
  } else if (dwStates & CFWL_PartState_Pressed) {
    iTheme = 3;
  } else if (dwStates & CFWL_PartState_Disabled) {
    iTheme = 4;
  }
  if (dwStates & CFWL_PartState_Checked) {
    iTheme += 4;
  } else if (dwStates & CFWL_PartState_Neutral) {
    iTheme += 8;
  }
  DrawAxialShading(pGraphics, pRect->left - 1, pRect->top - 1, fRight, fBottom,
                   m_pThemeData->clrBoxBk[iTheme][0],
                   m_pThemeData->clrBoxBk[iTheme][1], &path, fillMode, pMatrix);
}
void CFWL_CheckBoxTP::DrawSign(IFWL_Widget* pWidget,
                               CFX_Graphics* pGraphics,
                               const CFX_RectF* pRtBox,
                               uint32_t dwStates,
                               CFX_Matrix* pMatrix) {
  CFX_RectF rtSign(*pRtBox);
  rtSign.Deflate(kSignMargin, kSignMargin);
  uint32_t dwColor = m_pThemeData->clrSignCheck;
  bool bCheck = true;
  if ((dwStates & CFWL_PartState_Disabled) &&
      (dwStates & CFWL_PartState_Checked)) {
    dwColor = m_pThemeData->clrSignBorderDisable;
  } else if (dwStates & CFWL_PartState_Neutral) {
    if (dwStates & CFWL_PartState_Hovered) {
      dwColor = m_pThemeData->clrSignNeutralHover;
    } else if (dwStates & CFWL_PartState_Pressed) {
      dwColor = m_pThemeData->clrSignNeutralPressed;
    } else if (dwStates & CFWL_PartState_Disabled) {
      dwColor = m_pThemeData->clrSignBorderDisable;
    } else {
      dwColor = m_pThemeData->clrSignNeutralNormal;
    }
    bCheck = false;
  }
  if (bCheck) {
    uint32_t dwStyle = pWidget->GetStylesEx();
    switch (dwStyle & FWL_STYLEEXT_CKB_SignShapeMask) {
      case FWL_STYLEEXT_CKB_SignShapeCheck: {
        DrawSignCheck(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
      case FWL_STYLEEXT_CKB_SignShapeCircle: {
        rtSign.Deflate(1, 1);
        DrawSignCircle(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
      case FWL_STYLEEXT_CKB_SignShapeCross: {
        DrawSignCross(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
      case FWL_STYLEEXT_CKB_SignShapeDiamond: {
        DrawSignDiamond(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
      case FWL_STYLEEXT_CKB_SignShapeSquare: {
        DrawSignSquare(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
      case FWL_STYLEEXT_CKB_SignShapeStar: {
        DrawSignStar(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      }
    }
  } else {
    FillSoildRect(pGraphics, ArgbEncode(255, 33, 161, 33), &rtSign, pMatrix);
  }
}
void CFWL_CheckBoxTP::DrawSignNeutral(CFX_Graphics* pGraphics,
                                      const CFX_RectF* pRtSign,
                                      CFX_Matrix* pMatrix) {
  ((CFX_RectF*)pRtSign)->Inflate(-3, -3);
  FillSoildRect(pGraphics, m_pThemeData->clrSignNeutral, pRtSign, pMatrix);
}
void CFWL_CheckBoxTP::DrawSignCheck(CFX_Graphics* pGraphics,
                                    const CFX_RectF* pRtSign,
                                    FX_ARGB argbFill,
                                    CFX_Matrix* pMatrix) {
  if (!m_pCheckPath) {
    initCheckPath(pRtSign->width);
  }
  CFX_Matrix mt;
  mt.SetIdentity();
  mt.Translate(pRtSign->left, pRtSign->top);
  mt.Concat(*pMatrix);
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(&crFill);
  pGraphics->FillPath(m_pCheckPath, FXFILL_WINDING, &mt);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignCircle(CFX_Graphics* pGraphics,
                                     const CFX_RectF* pRtSign,
                                     FX_ARGB argbFill,
                                     CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  path.AddEllipse(*pRtSign);
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(&crFill);
  pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignCross(CFX_Graphics* pGraphics,
                                    const CFX_RectF* pRtSign,
                                    FX_ARGB argbFill,
                                    CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  FX_FLOAT fRight = pRtSign->right();
  FX_FLOAT fBottom = pRtSign->bottom();
  path.AddLine(pRtSign->left, pRtSign->top, fRight, fBottom);
  path.AddLine(pRtSign->left, fBottom, fRight, pRtSign->top);
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetStrokeColor(&crFill);
  pGraphics->SetLineWidth(1.0f);
  pGraphics->StrokePath(&path, pMatrix);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignDiamond(CFX_Graphics* pGraphics,
                                      const CFX_RectF* pRtSign,
                                      FX_ARGB argbFill,
                                      CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  FX_FLOAT fWidth = pRtSign->width;
  FX_FLOAT fHeight = pRtSign->height;
  FX_FLOAT fBottom = pRtSign->bottom();
  path.MoveTo(pRtSign->left + fWidth / 2, pRtSign->top);
  path.LineTo(pRtSign->left, pRtSign->top + fHeight / 2);
  path.LineTo(pRtSign->left + fWidth / 2, fBottom);
  path.LineTo(pRtSign->right(), pRtSign->top + fHeight / 2);
  path.LineTo(pRtSign->left + fWidth / 2, pRtSign->top);
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(&crFill);
  pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignSquare(CFX_Graphics* pGraphics,
                                     const CFX_RectF* pRtSign,
                                     FX_ARGB argbFill,
                                     CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  path.AddRectangle(pRtSign->left, pRtSign->top, pRtSign->width,
                    pRtSign->height);
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(&crFill);
  pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignStar(CFX_Graphics* pGraphics,
                                   const CFX_RectF* pRtSign,
                                   FX_ARGB argbFill,
                                   CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  FX_FLOAT fBottom = pRtSign->bottom();
  FX_FLOAT fRadius =
      (pRtSign->top - fBottom) / (1 + (FX_FLOAT)cos(FX_PI / 5.0f));
  CFX_PointF ptCenter((pRtSign->left + pRtSign->right()) / 2.0f,
                      (pRtSign->top + fBottom) / 2.0f);
  FX_FLOAT px[5], py[5];
  FX_FLOAT fAngel = FX_PI / 10.0f;
  for (int32_t i = 0; i < 5; i++) {
    px[i] = ptCenter.x + fRadius * (FX_FLOAT)cos(fAngel);
    py[i] = ptCenter.y + fRadius * (FX_FLOAT)sin(fAngel);
    fAngel += FX_PI * 2 / 5.0f;
  }
  path.MoveTo(px[0], py[0]);
  int32_t nNext = 0;
  for (int32_t j = 0; j < 5; j++) {
    nNext += 2;
    if (nNext >= 5) {
      nNext -= 5;
    }
    path.LineTo(px[nNext], py[nNext]);
  }
  CFX_Color crFill(argbFill);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(&crFill);
  pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pGraphics->RestoreGraphState();
}
void CFWL_CheckBoxTP::DrawSignBorder(IFWL_Widget* pWidget,
                                     CFX_Graphics* pGraphics,
                                     const CFX_RectF* pRtBox,
                                     FX_BOOL bDisable,
                                     CFX_Matrix* pMatrix) {
  switch (pWidget->GetStylesEx() & FWL_STYLEEXT_CKB_ShapeMask) {
    case FWL_STYLEEXT_CKB_ShapeSolidSquare: {
      DrawAnnulusRect(pGraphics, bDisable ? m_pThemeData->clrSignBorderDisable
                                          : m_pThemeData->clrSignBorderNormal,
                      pRtBox, 1, pMatrix);
      break;
    }
    case FWL_STYLEEXT_CKB_ShapeSunkenSquare: {
      Draw3DRect(pGraphics, FWLTHEME_EDGE_Sunken, kSignBorder, pRtBox,
                 CHECKBOX_COLOR_BOXLT1, CHECKBOX_COLOR_BOXLT2,
                 CHECKBOX_COLOR_BOXRB1, CHECKBOX_COLOR_BOXRB2, pMatrix);
      break;
    }
    case FWL_STYLEEXT_CKB_ShapeSolidCircle: {
      DrawAnnulusCircle(pGraphics, bDisable ? m_pThemeData->clrSignBorderDisable
                                            : m_pThemeData->clrSignBorderNormal,
                        pRtBox, 1, pMatrix);
      break;
    }
    case FWL_STYLEEXT_CKB_ShapeSunkenCircle: {
      Draw3DCircle(pGraphics, FWLTHEME_EDGE_Sunken, kSignBorder, pRtBox,
                   CHECKBOX_COLOR_BOXLT1, CHECKBOX_COLOR_BOXLT2,
                   CHECKBOX_COLOR_BOXRB1, CHECKBOX_COLOR_BOXRB2, pMatrix);
      break;
    }
  }
}
void CFWL_CheckBoxTP::SetThemeData(uint32_t dwID) {
  uint32_t* pData = (uint32_t*)&m_pThemeData->clrBoxBk;
  if (dwID) {
    *pData++ = 0, *pData++ = 0, *pData++ = ArgbEncode(255, 220, 220, 215),
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
    m_pThemeData->clrSignBorderNormal = ArgbEncode(255, 154, 167, 114);
    m_pThemeData->clrSignBorderDisable = ArgbEncode(255, 202, 200, 187);
    m_pThemeData->clrSignCheck = ArgbEncode(255, 164, 180, 138);
    m_pThemeData->clrSignNeutral = ArgbEncode(2255, 28, 134, 26);
    m_pThemeData->clrSignNeutralNormal = ArgbEncode(255, 114, 192, 113);
    m_pThemeData->clrSignNeutralHover = ArgbEncode(255, 33, 161, 33);
    m_pThemeData->clrSignNeutralPressed = ArgbEncode(255, 28, 134, 26);
  } else {
    *pData++ = 0, *pData++ = 0, *pData++ = ArgbEncode(255, 220, 220, 215),
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
    m_pThemeData->clrSignBorderNormal = ArgbEncode(255, 28, 81, 128);
    m_pThemeData->clrSignBorderDisable = ArgbEncode(255, 202, 200, 187);
    m_pThemeData->clrSignCheck = ArgbEncode(255, 28, 81, 128);
    m_pThemeData->clrSignNeutral = ArgbEncode(255, 28, 134, 26);
    m_pThemeData->clrSignNeutralNormal = ArgbEncode(255, 114, 192, 113);
    m_pThemeData->clrSignNeutralHover = ArgbEncode(255, 33, 161, 33);
    m_pThemeData->clrSignNeutralPressed = ArgbEncode(255, 28, 134, 26);
  }
}
void CFWL_CheckBoxTP::initCheckPath(FX_FLOAT fCheckLen) {
  if (!m_pCheckPath) {
    m_pCheckPath = new CFX_Path;
    m_pCheckPath->Create();
    FX_FLOAT fWidth = kSignPath;
    FX_FLOAT fHeight = -kSignPath;
    FX_FLOAT fBottom = kSignPath;
    CFX_PointF pt1(fWidth / 15.0f, fBottom + fHeight * 2 / 5.0f);
    CFX_PointF pt2(fWidth / 4.5f, fBottom + fHeight / 16.0f);
    CFX_PointF pt3(fWidth / 3.0f, fBottom);
    CFX_PointF pt4(fWidth * 14 / 15.0f, fBottom + fHeight * 15 / 16.0f);
    CFX_PointF pt5(fWidth / 3.6f, fBottom + fHeight / 3.5f);
    CFX_PointF pt12(fWidth / 7.0f, fBottom + fHeight * 2 / 7.0f);
    CFX_PointF pt21(fWidth / 5.0f, fBottom + fHeight / 5.0f);
    CFX_PointF pt23(fWidth / 4.4f, fBottom + fHeight * 0 / 16.0f);
    CFX_PointF pt32(fWidth / 4.0f, fBottom);
    CFX_PointF pt34(fWidth * (1 / 7.0f + 7 / 15.0f),
                    fBottom + fHeight * 4 / 5.0f);
    CFX_PointF pt43(fWidth * (1 / 7.0f + 7 / 15.0f),
                    fBottom + fHeight * 4 / 5.0f);
    CFX_PointF pt45(fWidth * 7 / 15.0f, fBottom + fHeight * 8 / 7.0f);
    CFX_PointF pt54(fWidth / 3.4f, fBottom + fHeight / 3.5f);
    CFX_PointF pt51(fWidth / 3.6f, fBottom + fHeight / 4.0f);
    CFX_PointF pt15(fWidth / 3.5f, fBottom + fHeight * 3.5f / 5.0f);
    m_pCheckPath->MoveTo(pt1.x, pt1.y);
    FX_FLOAT px1 = pt12.x - pt1.x;
    FX_FLOAT py1 = pt12.y - pt1.y;
    FX_FLOAT px2 = pt21.x - pt2.x;
    FX_FLOAT py2 = pt21.y - pt2.y;
    m_pCheckPath->BezierTo(pt1.x + px1 * FX_BEZIER, pt1.y + py1 * FX_BEZIER,
                           pt2.x + px2 * FX_BEZIER, pt2.y + py2 * FX_BEZIER,
                           pt2.x, pt2.y);
    px1 = pt23.x - pt2.x;
    py1 = pt23.y - pt2.y;
    px2 = pt32.x - pt3.x;
    py2 = pt32.y - pt3.y;
    m_pCheckPath->BezierTo(pt2.x + px1 * FX_BEZIER, pt2.y + py1 * FX_BEZIER,
                           pt3.x + px2 * FX_BEZIER, pt3.y + py2 * FX_BEZIER,
                           pt3.x, pt3.y);
    px1 = pt34.x - pt3.x;
    py1 = pt34.y - pt3.y;
    px2 = pt43.x - pt4.x;
    py2 = pt43.y - pt4.y;
    m_pCheckPath->BezierTo(pt3.x + px1 * FX_BEZIER, pt3.y + py1 * FX_BEZIER,
                           pt4.x + px2 * FX_BEZIER, pt4.y + py2 * FX_BEZIER,
                           pt4.x, pt4.y);
    px1 = pt45.x - pt4.x;
    py1 = pt45.y - pt4.y;
    px2 = pt54.x - pt5.x;
    py2 = pt54.y - pt5.y;
    m_pCheckPath->BezierTo(pt4.x + px1 * FX_BEZIER, pt4.y + py1 * FX_BEZIER,
                           pt5.x + px2 * FX_BEZIER, pt5.y + py2 * FX_BEZIER,
                           pt5.x, pt5.y);
    px1 = pt51.x - pt5.x;
    py1 = pt51.y - pt5.y;
    px2 = pt15.x - pt1.x;
    py2 = pt15.y - pt1.y;
    m_pCheckPath->BezierTo(pt5.x + px1 * FX_BEZIER, pt5.y + py1 * FX_BEZIER,
                           pt1.x + px2 * FX_BEZIER, pt1.y + py2 * FX_BEZIER,
                           pt1.x, pt1.y);
    FX_FLOAT fScale = fCheckLen / kSignPath;
    CFX_Matrix mt;
    mt.Set(1, 0, 0, 1, 0, 0);
    mt.Scale(fScale, fScale);
    CFX_PathData* pData = m_pCheckPath->GetPathData();
    pData->Transform(&mt);
  }
}
