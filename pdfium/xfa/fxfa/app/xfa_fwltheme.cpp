// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_fwltheme.h"

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/font/fgas_gefont.h"
#include "xfa/fwl/basewidget/ifwl_barcode.h"
#include "xfa/fwl/basewidget/ifwl_caret.h"
#include "xfa/fwl/basewidget/ifwl_checkbox.h"
#include "xfa/fwl/basewidget/ifwl_combobox.h"
#include "xfa/fwl/basewidget/ifwl_datetimepicker.h"
#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/basewidget/ifwl_listbox.h"
#include "xfa/fwl/basewidget/ifwl_monthcalendar.h"
#include "xfa/fwl/basewidget/ifwl_picturebox.h"
#include "xfa/fwl/basewidget/ifwl_pushbutton.h"
#include "xfa/fwl/basewidget/ifwl_scrollbar.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxgraphics/cfx_color.h"

namespace {

const FX_WCHAR* const g_FWLTheme_CalFonts[] = {
    L"Arial", L"Courier New", L"DejaVu Sans",
};

}  // namespace

CXFA_FFWidget* XFA_ThemeGetOuterWidget(IFWL_Widget* pWidget) {
  IFWL_Widget* pOuter = pWidget;
  while (pOuter && pOuter->GetOuter())
    pOuter = pOuter->GetOuter();

  return pOuter ? static_cast<CXFA_FFWidget*>(pOuter->GetLayoutItem())
                : nullptr;
}

CXFA_FWLTheme::CXFA_FWLTheme(CXFA_FFApp* pApp) : m_pApp(pApp) {
  m_dwCapacity = 0;
  m_fCapacity = 0;
  m_pCalendarFont = nullptr;
  m_Rect.Set(0, 0, 0, 0);
  m_pCheckBoxTP = new CXFA_FWLCheckBoxTP;
  m_pListBoxTP = new CFWL_ListBoxTP;
  m_pPictureBoxTP = new CFWL_PictureBoxTP;
  m_pSrollBarTP = new CFWL_ScrollBarTP;
  m_pEditTP = new CXFA_FWLEditTP;
  m_pComboBoxTP = new CFWL_ComboBoxTP;
  m_pMonthCalendarTP = new CFWL_MonthCalendarTP;
  m_pDateTimePickerTP = new CFWL_DateTimePickerTP;
  m_pPushButtonTP = new CFWL_PushButtonTP;
  m_pCaretTP = new CFWL_CaretTP;
  m_pBarcodeTP = new CFWL_BarcodeTP;
  Initialize();
}
CXFA_FWLTheme::~CXFA_FWLTheme() {
  Finalize();
  delete m_pCheckBoxTP;
  delete m_pListBoxTP;
  delete m_pPictureBoxTP;
  delete m_pSrollBarTP;
  delete m_pEditTP;
  delete m_pComboBoxTP;
  delete m_pMonthCalendarTP;
  delete m_pDateTimePickerTP;
  delete m_pPushButtonTP;
  delete m_pCaretTP;
  delete m_pBarcodeTP;
}
FWL_Error CXFA_FWLTheme::Initialize() {
  m_pTextOut.reset(new CFDE_TextOut);
  for (size_t i = 0; !m_pCalendarFont && i < FX_ArraySize(g_FWLTheme_CalFonts);
       ++i) {
    m_pCalendarFont = CFGAS_GEFont::LoadFont(g_FWLTheme_CalFonts[i], 0, 0,
                                             m_pApp->GetFDEFontMgr());
  }
  if (!m_pCalendarFont) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    m_pCalendarFont = m_pApp->GetFDEFontMgr()->GetDefFontByCodePage(
        FX_CODEPAGE_MSWin_WesternEuropean, 0, nullptr);
#else
    m_pCalendarFont = m_pApp->GetFDEFontMgr()->GetFontByCodePage(
        FX_CODEPAGE_MSWin_WesternEuropean, 0, nullptr);
#endif
  }

  ASSERT(m_pCalendarFont);
  FWLTHEME_Init();
  return FWL_Error::Succeeded;
}
FWL_Error CXFA_FWLTheme::Finalize() {
  m_pTextOut.reset();
  if (m_pCalendarFont) {
    m_pCalendarFont->Release();
    m_pCalendarFont = nullptr;
  }
  FWLTHEME_Release();
  return FWL_Error::Succeeded;
}

bool CXFA_FWLTheme::IsValidWidget(IFWL_Widget* pWidget) {
  return TRUE;
}

uint32_t CXFA_FWLTheme::GetThemeID(IFWL_Widget* pWidget) {
  return 0;
}

uint32_t CXFA_FWLTheme::SetThemeID(IFWL_Widget* pWidget,
                                   uint32_t dwThemeID,
                                   FX_BOOL bChildren) {
  return 0;
}

FWL_Error CXFA_FWLTheme::GetThemeMatrix(IFWL_Widget* pWidget,
                                        CFX_Matrix& matrix) {
  return FWL_Error::Succeeded;
}

FWL_Error CXFA_FWLTheme::SetThemeMatrix(IFWL_Widget* pWidget,
                                        const CFX_Matrix& matrix) {
  return FWL_Error::Succeeded;
}

FX_BOOL CXFA_FWLTheme::DrawBackground(CFWL_ThemeBackground* pParams) {
  return GetTheme(pParams->m_pWidget)->DrawBackground(pParams);
}
FX_BOOL CXFA_FWLTheme::DrawText(CFWL_ThemeText* pParams) {
  if (pParams->m_wsText.IsEmpty())
    return FALSE;

  if (pParams->m_pWidget->GetClassID() == FWL_Type::MonthCalendar) {
    CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams->m_pWidget);
    if (!pWidget)
      return FALSE;

    m_pTextOut->SetStyles(pParams->m_dwTTOStyles);
    m_pTextOut->SetAlignment(pParams->m_iTTOAlign);
    m_pTextOut->SetFont(m_pCalendarFont);
    m_pTextOut->SetFontSize(FWLTHEME_CAPACITY_FontSize);
    m_pTextOut->SetTextColor(FWLTHEME_CAPACITY_TextColor);
    if ((pParams->m_iPart == CFWL_Part::DatesIn) &&
        !(pParams->m_dwStates & FWL_ITEMSTATE_MCD_Flag) &&
        (pParams->m_dwStates &
         (CFWL_PartState_Hovered | CFWL_PartState_Selected))) {
      m_pTextOut->SetTextColor(0xFFFFFFFF);
    }
    if (pParams->m_iPart == CFWL_Part::Caption) {
      if (m_pMonthCalendarTP->GetThemeID(pParams->m_pWidget) == 0) {
        m_pTextOut->SetTextColor(ArgbEncode(0xff, 0, 153, 255));
      } else {
        m_pTextOut->SetTextColor(ArgbEncode(0xff, 128, 128, 0));
      }
    }
    CFX_Graphics* pGraphics = pParams->m_pGraphics;
    CFX_RenderDevice* pRenderDevice = pGraphics->GetRenderDevice();
    if (!pRenderDevice)
      return FALSE;
    m_pTextOut->SetRenderDevice(pRenderDevice);
    CFX_Matrix mtPart = pParams->m_matrix;
    CFX_Matrix* pMatrix = pGraphics->GetMatrix();
    if (pMatrix) {
      mtPart.Concat(*pMatrix);
    }
    m_pTextOut->SetMatrix(mtPart);
    m_pTextOut->DrawLogicText(pParams->m_wsText.c_str(),
                              pParams->m_wsText.GetLength(), pParams->m_rtPart);
    return TRUE;
  }
  CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams->m_pWidget);
  if (!pWidget)
    return FALSE;

  CXFA_WidgetAcc* pAcc = pWidget->GetDataAcc();
  CFX_Graphics* pGraphics = pParams->m_pGraphics;
  CFX_RenderDevice* pRenderDevice = pGraphics->GetRenderDevice();
  if (!pRenderDevice)
    return FALSE;
  m_pTextOut->SetRenderDevice(pRenderDevice);
  m_pTextOut->SetStyles(pParams->m_dwTTOStyles);
  m_pTextOut->SetAlignment(pParams->m_iTTOAlign);
  m_pTextOut->SetFont(pAcc->GetFDEFont());
  m_pTextOut->SetFontSize(pAcc->GetFontSize());
  m_pTextOut->SetTextColor(pAcc->GetTextColor());
  CFX_Matrix mtPart = pParams->m_matrix;
  CFX_Matrix* pMatrix = pGraphics->GetMatrix();
  if (pMatrix) {
    mtPart.Concat(*pMatrix);
  }
  m_pTextOut->SetMatrix(mtPart);
  m_pTextOut->DrawLogicText(pParams->m_wsText.c_str(),
                            pParams->m_wsText.GetLength(), pParams->m_rtPart);
  return TRUE;
}
void* CXFA_FWLTheme::GetCapacity(CFWL_ThemePart* pThemePart,
                                 CFWL_WidgetCapacity dwCapacity) {
  switch (dwCapacity) {
    case CFWL_WidgetCapacity::Font: {
      if (CXFA_FFWidget* pWidget =
              XFA_ThemeGetOuterWidget(pThemePart->m_pWidget)) {
        return pWidget->GetDataAcc()->GetFDEFont();
      }
    } break;
    case CFWL_WidgetCapacity::FontSize: {
      if (CXFA_FFWidget* pWidget =
              XFA_ThemeGetOuterWidget(pThemePart->m_pWidget)) {
        m_fCapacity = pWidget->GetDataAcc()->GetFontSize();
        return &m_fCapacity;
      }
    } break;
    case CFWL_WidgetCapacity::TextColor: {
      if (CXFA_FFWidget* pWidget =
              XFA_ThemeGetOuterWidget(pThemePart->m_pWidget)) {
        m_dwCapacity = pWidget->GetDataAcc()->GetTextColor();
        return &m_dwCapacity;
      }
    } break;
    case CFWL_WidgetCapacity::LineHeight: {
      if (CXFA_FFWidget* pWidget =
              XFA_ThemeGetOuterWidget(pThemePart->m_pWidget)) {
        m_fCapacity = pWidget->GetDataAcc()->GetLineHeight();
        return &m_fCapacity;
      }
    } break;
    case CFWL_WidgetCapacity::ScrollBarWidth: {
      m_fCapacity = 9;
      return &m_fCapacity;
    } break;
    case CFWL_WidgetCapacity::UIMargin: {
      CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart->m_pWidget);
      if (pWidget) {
        CXFA_LayoutItem* pItem = pWidget;
        CXFA_WidgetAcc* pWidgetAcc = pWidget->GetDataAcc();
        pWidgetAcc->GetUIMargin(m_Rect);
        if (CXFA_Para para = pWidgetAcc->GetPara()) {
          m_Rect.left += para.GetMarginLeft();
          if (pWidgetAcc->IsMultiLine()) {
            m_Rect.width += para.GetMarginRight();
          }
        }
        if (!pItem->GetPrev()) {
          if (pItem->GetNext()) {
            m_Rect.height = 0;
          }
        } else if (!pItem->GetNext()) {
          m_Rect.top = 0;
        } else {
          m_Rect.top = 0;
          m_Rect.height = 0;
        }
      }
      return &m_Rect;
    } break;
    case CFWL_WidgetCapacity::SpaceAboveBelow: {
      CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pThemePart->m_pWidget);
      if (pWidget) {
        CXFA_WidgetAcc* pWidgetAcc = pWidget->GetDataAcc();
        if (CXFA_Para para = pWidgetAcc->GetPara()) {
          m_SizeAboveBelow.x = para.GetSpaceAbove();
          m_SizeAboveBelow.y = para.GetSpaceBelow();
        }
      }
      return &m_SizeAboveBelow;
    } break;
    default:
      break;
  }

  int dwCapValue = static_cast<int>(dwCapacity);
  if (pThemePart->m_pWidget->GetClassID() == FWL_Type::MonthCalendar &&
      dwCapValue >= static_cast<int>(CFWL_WidgetCapacity::Today) &&
      dwCapValue <= static_cast<int>(CFWL_WidgetCapacity::December)) {
    if (CXFA_FFWidget* pWidget =
            XFA_ThemeGetOuterWidget(pThemePart->m_pWidget)) {
      IXFA_AppProvider* pAppProvider = pWidget->GetAppProvider();
      m_wsResource.clear();
      switch (dwCapacity) {
        case CFWL_WidgetCapacity::Sun:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Sun, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Mon:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Mon, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Tue:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Tue, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Wed:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Wed, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Thu:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Thu, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Fri:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Fri, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Sat:
          pAppProvider->LoadString(XFA_IDS_StringWeekDay_Sat, m_wsResource);
          break;
        case CFWL_WidgetCapacity::January:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Jan, m_wsResource);
          break;
        case CFWL_WidgetCapacity::February:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Feb, m_wsResource);
          break;
        case CFWL_WidgetCapacity::March:
          pAppProvider->LoadString(XFA_IDS_StringMonth_March, m_wsResource);
          break;
        case CFWL_WidgetCapacity::April:
          pAppProvider->LoadString(XFA_IDS_StringMonth_April, m_wsResource);
          break;
        case CFWL_WidgetCapacity::May:
          pAppProvider->LoadString(XFA_IDS_StringMonth_May, m_wsResource);
          break;
        case CFWL_WidgetCapacity::June:
          pAppProvider->LoadString(XFA_IDS_StringMonth_June, m_wsResource);
          break;
        case CFWL_WidgetCapacity::July:
          pAppProvider->LoadString(XFA_IDS_StringMonth_July, m_wsResource);
          break;
        case CFWL_WidgetCapacity::August:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Aug, m_wsResource);
          break;
        case CFWL_WidgetCapacity::September:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Sept, m_wsResource);
          break;
        case CFWL_WidgetCapacity::October:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Oct, m_wsResource);
          break;
        case CFWL_WidgetCapacity::November:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Nov, m_wsResource);
          break;
        case CFWL_WidgetCapacity::December:
          pAppProvider->LoadString(XFA_IDS_StringMonth_Dec, m_wsResource);
          break;
        case CFWL_WidgetCapacity::Today:
          pAppProvider->LoadString(XFA_IDS_String_Today, m_wsResource);
          break;
        default:
          break;
      }
      if (!m_wsResource.IsEmpty())
        return &m_wsResource;
    }
  }
  return GetTheme(pThemePart->m_pWidget)->GetCapacity(pThemePart, dwCapacity);
}
FX_BOOL CXFA_FWLTheme::IsCustomizedLayout(IFWL_Widget* pWidget) {
  return GetTheme(pWidget)->IsCustomizedLayout(pWidget);
}

FWL_Error CXFA_FWLTheme::GetPartRect(CFWL_ThemePart* pThemePart,
                                     CFX_RectF& rtPart) {
  return FWL_Error::Succeeded;
}
FWL_Error CXFA_FWLTheme::GetPartRect(CFWL_ThemePart* pThemePart) {
  CFX_RectF rect;
  return GetTheme(pThemePart->m_pWidget)->GetPartRect(pThemePart, rect);
}
FX_BOOL CXFA_FWLTheme::IsInPart(CFWL_ThemePart* pThemePart,
                                FX_FLOAT fx,
                                FX_FLOAT fy) {
  return GetTheme(pThemePart->m_pWidget)->IsInPart(pThemePart, fx, fy);
}
FX_BOOL CXFA_FWLTheme::CalcTextRect(CFWL_ThemeText* pParams, CFX_RectF& rect) {
  if (pParams->m_pWidget->GetClassID() == FWL_Type::MonthCalendar) {
    CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams->m_pWidget);
    if (!pWidget)
      return FALSE;
    if (!pParams)
      return FALSE;
    if (!m_pTextOut)
      return FALSE;
    m_pTextOut->SetFont(m_pCalendarFont);
    m_pTextOut->SetFontSize(FWLTHEME_CAPACITY_FontSize);
    m_pTextOut->SetTextColor(FWLTHEME_CAPACITY_TextColor);
    m_pTextOut->SetAlignment(pParams->m_iTTOAlign);
    m_pTextOut->SetStyles(pParams->m_dwTTOStyles);
    m_pTextOut->CalcLogicSize(pParams->m_wsText.c_str(),
                              pParams->m_wsText.GetLength(), rect);
    return TRUE;
  }
  CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams->m_pWidget);
  if (!pWidget)
    return FALSE;

  CXFA_WidgetAcc* pAcc = pWidget->GetDataAcc();
  m_pTextOut->SetFont(pAcc->GetFDEFont());
  m_pTextOut->SetFontSize(pAcc->GetFontSize());
  m_pTextOut->SetTextColor(pAcc->GetTextColor());
  if (!pParams)
    return FALSE;
  m_pTextOut->SetAlignment(pParams->m_iTTOAlign);
  m_pTextOut->SetStyles(pParams->m_dwTTOStyles);
  m_pTextOut->CalcLogicSize(pParams->m_wsText.c_str(),
                            pParams->m_wsText.GetLength(), rect);
  return TRUE;
}
CFWL_WidgetTP* CXFA_FWLTheme::GetTheme(IFWL_Widget* pWidget) {
  switch (pWidget->GetClassID()) {
    case FWL_Type::CheckBox:
      return m_pCheckBoxTP;
    case FWL_Type::ListBox:
      return m_pListBoxTP;
    case FWL_Type::PictureBox:
      return m_pPictureBoxTP;
    case FWL_Type::ScrollBar:
      return m_pSrollBarTP;
    case FWL_Type::Edit:
      return m_pEditTP;
    case FWL_Type::ComboBox:
      return m_pComboBoxTP;
    case FWL_Type::MonthCalendar:
      return m_pMonthCalendarTP;
    case FWL_Type::DateTimePicker:
      return m_pDateTimePickerTP;
    case FWL_Type::PushButton:
      return m_pPushButtonTP;
    case FWL_Type::Caret:
      return m_pCaretTP;
    case FWL_Type::Barcode:
      return m_pBarcodeTP;
    default:
      return nullptr;
  }
}
CXFA_FWLCheckBoxTP::CXFA_FWLCheckBoxTP() {}
FX_BOOL CXFA_FWLCheckBoxTP::DrawBackground(CFWL_ThemeBackground* pParams) {
  if (pParams->m_iPart != CFWL_Part::CheckBox) {
    return TRUE;
  }
  if ((pParams->m_dwStates & CFWL_PartState_Checked) ||
      (pParams->m_dwStates & CFWL_PartState_Neutral)) {
    DrawCheckSign(pParams->m_pWidget, pParams->m_pGraphics, &pParams->m_rtPart,
                  pParams->m_dwStates, &pParams->m_matrix);
  }
  return TRUE;
}
void CXFA_FWLCheckBoxTP::DrawCheckSign(IFWL_Widget* pWidget,
                                       CFX_Graphics* pGraphics,
                                       const CFX_RectF* pRtBox,
                                       int32_t iState,
                                       CFX_Matrix* pMatrix) {
  CFX_RectF rtSign(*pRtBox);
  uint32_t dwColor = 0xFF000000;
  if (iState & CFWL_PartState_Neutral)
    dwColor = 0xFFA9A9A9;

  {
    uint32_t dwStyle = pWidget->GetStylesEx();
    rtSign.Deflate(rtSign.width / 4, rtSign.height / 4);
    switch (dwStyle & FWL_STYLEEXT_CKB_SignShapeMask) {
      case FWL_STYLEEXT_CKB_SignShapeCheck:
        DrawSignCheck(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      case FWL_STYLEEXT_CKB_SignShapeCircle:
        DrawSignCircle(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      case FWL_STYLEEXT_CKB_SignShapeCross:
        DrawSignCross(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      case FWL_STYLEEXT_CKB_SignShapeDiamond:
        DrawSignDiamond(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      case FWL_STYLEEXT_CKB_SignShapeSquare:
        DrawSignSquare(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      case FWL_STYLEEXT_CKB_SignShapeStar:
        DrawSignStar(pGraphics, &rtSign, dwColor, pMatrix);
        break;
      default:
        break;
    }
  }
}
CXFA_FWLEditTP::CXFA_FWLEditTP() {}
CXFA_FWLEditTP::~CXFA_FWLEditTP() {}
FX_BOOL CXFA_FWLEditTP::DrawBackground(CFWL_ThemeBackground* pParams) {
  if (CFWL_Part::CombTextLine == pParams->m_iPart) {
    CXFA_FFWidget* pWidget = XFA_ThemeGetOuterWidget(pParams->m_pWidget);
    FX_ARGB cr = 0xFF000000;
    FX_FLOAT fWidth = 1.0f;
    if (CXFA_Border borderUI = pWidget->GetDataAcc()->GetUIBorder()) {
      CXFA_Edge edge = borderUI.GetEdge(0);
      if (edge) {
        cr = edge.GetColor();
        fWidth = edge.GetThickness();
      }
    }
    CFX_Color crLine(cr);
    pParams->m_pGraphics->SetStrokeColor(&crLine);
    pParams->m_pGraphics->SetLineWidth(fWidth);
    pParams->m_pGraphics->StrokePath(pParams->m_pPath, &pParams->m_matrix);
    return TRUE;
  }
  return CFWL_EditTP::DrawBackground(pParams);
}
