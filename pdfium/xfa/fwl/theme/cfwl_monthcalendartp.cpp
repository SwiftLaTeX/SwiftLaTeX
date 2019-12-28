// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_monthcalendartp.h"

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_monthcalendar.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxgraphics/cfx_color.h"
#include "xfa/fxgraphics/cfx_path.h"

namespace {

const int kWidth = 200;
const int kHeight = 160;

const int kHeaderWidth = 200;
const int kHeaderHeight = 30;

const int kButtonWidth = 18;
const int kButtonHeight = 16;
const int kButtonHorizontalMargin = 5;
const int kButtonVerticalMargin = (kHeaderHeight - kButtonHeight) / 2;

const int kHeaderTextWidth = 100;
const int kHeaderTextHeight = 20;
const int kHeaderTextHorizontalMargin = (kHeaderWidth - kHeaderTextWidth) / 2;
const int kHeaderTextVerticalMargin = (kHeaderHeight - kHeaderTextHeight) / 2;

const int kDatesCellWidth = (kHeaderWidth / 7);
const int kDatesCellHeight = 16;
const int kWeekWidth = kDatesCellWidth * 7;
const int kWeekNumWidth = 26;
const int kWeekNumHeight = (6 * kDatesCellHeight);

const int kHorizontalSeparatorWidth = kWeekWidth - 10;
const int kHorizontalSeparatorHeight = 1;
const int kVerticalSeparatorWidth = 1;
const int kVerticalSeparatorHeight = kWeekNumHeight;
const int kSeparatorDOffset = -4;
const int kSeparatorX = 3;
const int kSeparatorY = kHeaderHeight + kDatesCellHeight + kSeparatorDOffset;

}  // namespace

CFWL_MonthCalendarTP::CFWL_MonthCalendarTP() {
  m_pThemeData = new MCThemeData;
  SetThemeData(0);
}

CFWL_MonthCalendarTP::~CFWL_MonthCalendarTP() {
  delete m_pThemeData;
}

bool CFWL_MonthCalendarTP::IsValidWidget(IFWL_Widget* pWidget) {
  return pWidget && pWidget->GetClassID() == FWL_Type::MonthCalendar;
}
uint32_t CFWL_MonthCalendarTP::SetThemeID(IFWL_Widget* pWidget,
                                          uint32_t dwThemeID,
                                          FX_BOOL bChildren) {
  if (m_pThemeData) {
    SetThemeData(FWL_GetThemeColor(dwThemeID));
  }
  return CFWL_WidgetTP::SetThemeID(pWidget, dwThemeID, bChildren);
}
FX_BOOL CFWL_MonthCalendarTP::DrawBackground(CFWL_ThemeBackground* pParams) {
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
      DrawTotalBK(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::Header: {
      DrawHeadBk(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::LBtn: {
      FWLTHEME_STATE eState = GetState(pParams->m_dwStates);
      DrawArrowBtn(pParams->m_pGraphics, &pParams->m_rtPart,
                   FWLTHEME_DIRECTION_Left, eState, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::RBtn: {
      FWLTHEME_STATE eState = GetState(pParams->m_dwStates);
      DrawArrowBtn(pParams->m_pGraphics, &pParams->m_rtPart,
                   FWLTHEME_DIRECTION_Right, eState, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::HSeparator: {
      DrawHSeperator(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::DatesIn: {
      DrawDatesInBK(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::TodayCircle: {
      DrawTodayCircle(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::DateInCircle: {
      DrawDatesInCircle(pParams, &pParams->m_matrix);
      break;
    }
    case CFWL_Part::WeekNumSep: {
      DrawWeekNumSep(pParams, &pParams->m_matrix);
      break;
    }
    default: {}
  }
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawText(CFWL_ThemeText* pParams) {
  if (!m_pTextOut)
    return FALSE;
  if ((pParams->m_iPart == CFWL_Part::DatesIn) &&
      !(pParams->m_dwStates & FWL_ITEMSTATE_MCD_Flag) &&
      (pParams->m_dwStates &
       (CFWL_PartState_Hovered | CFWL_PartState_Selected))) {
    m_pTextOut->SetTextColor(0xFFFFFFFF);
  } else if (pParams->m_iPart == CFWL_Part::Caption) {
    m_pTextOut->SetTextColor(m_pThemeData->clrCaption);
  } else {
    m_pTextOut->SetTextColor(0xFF000000);
  }
  return CFWL_WidgetTP::DrawText(pParams);
}
void* CFWL_MonthCalendarTP::GetCapacity(CFWL_ThemePart* pThemePart,
                                        CFWL_WidgetCapacity dwCapacity) {
  FX_BOOL bDefPro = FALSE;
  FX_BOOL bDwordVal = FALSE;
  switch (dwCapacity) {
    case CFWL_WidgetCapacity::HeaderWidth: {
      m_fValue = kHeaderWidth;
      break;
    }
    case CFWL_WidgetCapacity::HeaderHeight: {
      m_fValue = kHeaderHeight;
      break;
    }
    case CFWL_WidgetCapacity::HeaderBtnWidth: {
      m_fValue = kButtonWidth;
      break;
    }
    case CFWL_WidgetCapacity::HeaderBtnHeight: {
      m_fValue = kButtonHeight;
      break;
    }
    case CFWL_WidgetCapacity::HeaderBtnHMargin: {
      bDwordVal = TRUE;
      m_dwValue = kButtonHorizontalMargin;
      break;
    }
    case CFWL_WidgetCapacity::HeaderBtnVMargin: {
      m_fValue = kButtonVerticalMargin;
      break;
    }
    case CFWL_WidgetCapacity::HeaderTextWidth: {
      m_fValue = kHeaderTextWidth;
      break;
    }
    case CFWL_WidgetCapacity::HeaderTextHeight: {
      m_fValue = kHeaderTextHeight;
      break;
    }
    case CFWL_WidgetCapacity::HeaderTextHMargin: {
      m_fValue = kHeaderTextHorizontalMargin;
      break;
    }
    case CFWL_WidgetCapacity::HeaderTextVMargin: {
      m_fValue = kHeaderTextVerticalMargin;
      break;
    }
    case CFWL_WidgetCapacity::HSepWidth: {
      m_fValue = kHorizontalSeparatorWidth;
      break;
    }
    case CFWL_WidgetCapacity::HSepHeight: {
      m_fValue = kHorizontalSeparatorHeight;
      break;
    }
    case CFWL_WidgetCapacity::VSepWidth: {
      m_fValue = kVerticalSeparatorWidth;
      break;
    }
    case CFWL_WidgetCapacity::VSepHeight: {
      m_fValue = kVerticalSeparatorHeight;
      break;
    }
    case CFWL_WidgetCapacity::WeekNumWidth: {
      m_fValue = kWeekNumWidth;
      break;
    }
    case CFWL_WidgetCapacity::WeekNumHeight: {
      m_fValue = kWeekNumHeight;
      break;
    }
    case CFWL_WidgetCapacity::WeekWidth: {
      m_fValue = kWeekWidth;
      break;
    }
    case CFWL_WidgetCapacity::WeekHeight: {
      m_fValue = kDatesCellHeight;
      break;
    }
    case CFWL_WidgetCapacity::SepDOffset: {
      m_fValue = kSeparatorDOffset;
      break;
    }
    case CFWL_WidgetCapacity::SepX: {
      m_fValue = kSeparatorX;
      break;
    }
    case CFWL_WidgetCapacity::SepY: {
      m_fValue = kSeparatorY;
      break;
    }
    case CFWL_WidgetCapacity::DatesCellWidth: {
      m_fValue = kDatesCellWidth;
      break;
    }
    case CFWL_WidgetCapacity::DatesCellHeight: {
      m_fValue = kDatesCellHeight;
      break;
    }
    case CFWL_WidgetCapacity::TodayWidth: {
      m_fValue = kHeaderWidth;
      break;
    }
    case CFWL_WidgetCapacity::TodayHeight: {
      m_fValue = kDatesCellHeight;
      break;
    }
    case CFWL_WidgetCapacity::TodayFlagWidth: {
      m_fValue = kDatesCellWidth;
      break;
    }
    case CFWL_WidgetCapacity::Width: {
      m_fValue = kWidth;
      break;
    }
    case CFWL_WidgetCapacity::Height: {
      m_fValue = kHeight;
      break;
    }
    case CFWL_WidgetCapacity::Sun: {
      wsResource = L"Sun";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Mon: {
      wsResource = L"Mon";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Tue: {
      wsResource = L"Tue";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Wed: {
      wsResource = L"Wed";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Thu: {
      wsResource = L"Thu";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Fri: {
      wsResource = L"Fri";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Sat: {
      wsResource = L"Sat";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::January: {
      wsResource = L"January";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::February: {
      wsResource = L"February";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::March: {
      wsResource = L"March";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::April: {
      wsResource = L"April";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::May: {
      wsResource = L"May";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::June: {
      wsResource = L"June";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::July: {
      wsResource = L"July";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::August: {
      wsResource = L"August";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::September: {
      wsResource = L"September";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::October: {
      wsResource = L"October";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::November: {
      wsResource = L"November";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::December: {
      wsResource = L"December";
      return &wsResource;
    }
    case CFWL_WidgetCapacity::Today: {
      wsResource = L"Today";
      return &wsResource;
    }
    default: { bDefPro = TRUE; }
  }
  if (!bDefPro) {
    if (bDwordVal) {
      return &m_dwValue;
    }
    return &m_fValue;
  }
  return CFWL_WidgetTP::GetCapacity(pThemePart, dwCapacity);
}
FWL_Error CFWL_MonthCalendarTP::Initialize() {
  InitTTO();
  return CFWL_WidgetTP::Initialize();
}
FWL_Error CFWL_MonthCalendarTP::Finalize() {
  FinalizeTTO();
  return CFWL_WidgetTP::Finalize();
}
FX_BOOL CFWL_MonthCalendarTP::DrawTotalBK(CFWL_ThemeBackground* pParams,
                                          CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtTotal(pParams->m_rtPart);
  path.AddRectangle(rtTotal.left, rtTotal.top, rtTotal.width, rtTotal.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrBK(m_pThemeData->clrBK);
  pParams->m_pGraphics->SetFillColor(&clrBK);
  pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawHeadBk(CFWL_ThemeBackground* pParams,
                                         CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtHead = pParams->m_rtPart;
  path.AddRectangle(rtHead.left, rtHead.top, rtHead.width, rtHead.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrHeadBK(m_pThemeData->clrBK);
  pParams->m_pGraphics->SetFillColor(&clrHeadBK);
  pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawLButton(CFWL_ThemeBackground* pParams,
                                          CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtLBtn;
  rtLBtn = pParams->m_rtPart;
  path.AddRectangle(rtLBtn.left, rtLBtn.top, rtLBtn.width, rtLBtn.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrLBtnEdge(ArgbEncode(0xff, 205, 219, 243));
  pParams->m_pGraphics->SetStrokeColor(&clrLBtnEdge);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  if (pParams->m_dwStates & CFWL_PartState_Pressed) {
    CFX_Color clrLBtnFill(ArgbEncode(0xff, 174, 198, 242));
    pParams->m_pGraphics->SetFillColor(&clrLBtnFill);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  } else {
    CFX_Color clrLBtnFill(ArgbEncode(0xff, 227, 235, 249));
    pParams->m_pGraphics->SetFillColor(&clrLBtnFill);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  }
  path.Clear();
  path.MoveTo(rtLBtn.left + rtLBtn.Width() / 3 * 2,
              rtLBtn.top + rtLBtn.height / 4);
  path.LineTo(rtLBtn.left + rtLBtn.Width() / 3, rtLBtn.top + rtLBtn.height / 2);
  path.LineTo(rtLBtn.left + rtLBtn.Width() / 3 * 2,
              rtLBtn.bottom() - rtLBtn.height / 4);
  CFX_Color clrFlag(ArgbEncode(0xff, 50, 104, 205));
  pParams->m_pGraphics->SetStrokeColor(&clrFlag);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawRButton(CFWL_ThemeBackground* pParams,
                                          CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtRBtn;
  rtRBtn = pParams->m_rtPart;
  path.AddRectangle(rtRBtn.left, rtRBtn.top, rtRBtn.width, rtRBtn.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrRBtnEdge(ArgbEncode(0xff, 205, 219, 243));
  pParams->m_pGraphics->SetStrokeColor(&clrRBtnEdge);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  if (pParams->m_dwStates & CFWL_PartState_Pressed) {
    CFX_Color clrRBtnFill(ArgbEncode(0xff, 174, 198, 242));
    pParams->m_pGraphics->SetFillColor(&clrRBtnFill);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  } else {
    CFX_Color clrRBtnFill(ArgbEncode(0xff, 227, 235, 249));
    pParams->m_pGraphics->SetFillColor(&clrRBtnFill);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  }
  path.Clear();
  path.MoveTo(rtRBtn.left + rtRBtn.Width() / 3, rtRBtn.top + rtRBtn.height / 4);
  path.LineTo(rtRBtn.left + rtRBtn.Width() / 3 * 2,
              rtRBtn.top + rtRBtn.height / 2);
  path.LineTo(rtRBtn.left + rtRBtn.Width() / 3,
              rtRBtn.bottom() - rtRBtn.height / 4);
  CFX_Color clrFlag(ArgbEncode(0xff, 50, 104, 205));
  pParams->m_pGraphics->SetStrokeColor(&clrFlag);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawHSeperator(CFWL_ThemeBackground* pParams,
                                             CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtHSep;
  rtHSep = pParams->m_rtPart;
  path.MoveTo(rtHSep.left, rtHSep.top + rtHSep.height / 2);
  path.LineTo(rtHSep.right(), rtHSep.top + rtHSep.height / 2);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrHSep(m_pThemeData->clrSeperator);
  pParams->m_pGraphics->SetStrokeColor(&clrHSep);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawWeekNumSep(CFWL_ThemeBackground* pParams,
                                             CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtWeekSep;
  rtWeekSep = pParams->m_rtPart;
  path.MoveTo(rtWeekSep.left, rtWeekSep.top);
  path.LineTo(rtWeekSep.left, rtWeekSep.bottom());
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrHSep(m_pThemeData->clrSeperator);
  pParams->m_pGraphics->SetStrokeColor(&clrHSep);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawDatesInBK(CFWL_ThemeBackground* pParams,
                                            CFX_Matrix* pMatrix) {
  pParams->m_pGraphics->SaveGraphState();
  if (pParams->m_dwStates & CFWL_PartState_Selected) {
    CFX_Path path;
    path.Create();
    CFX_RectF rtSelDay;
    rtSelDay = pParams->m_rtPart;
    path.AddRectangle(rtSelDay.left, rtSelDay.top, rtSelDay.width,
                      rtSelDay.height);
    CFX_Color clrSelDayBK;
    clrSelDayBK = m_pThemeData->clrDatesSelectedBK;
    pParams->m_pGraphics->SetFillColor(&clrSelDayBK);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  } else if (pParams->m_dwStates & CFWL_PartState_Hovered) {
    CFX_Path path;
    path.Create();
    CFX_RectF rtSelDay;
    rtSelDay = pParams->m_rtPart;
    path.AddRectangle(rtSelDay.left, rtSelDay.top, rtSelDay.width,
                      rtSelDay.height);
    CFX_Color clrSelDayBK;
    clrSelDayBK = m_pThemeData->clrDatesHoverBK;
    pParams->m_pGraphics->SetFillColor(&clrSelDayBK);
    pParams->m_pGraphics->FillPath(&path, FXFILL_WINDING, pMatrix);
  }
  pParams->m_pGraphics->RestoreGraphState();
  return FALSE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawDatesInCircle(CFWL_ThemeBackground* pParams,
                                                CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtSelDay;
  rtSelDay = pParams->m_rtPart;
  path.AddRectangle(rtSelDay.left, rtSelDay.top, rtSelDay.width,
                    rtSelDay.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrSelDayBK;
  clrSelDayBK = m_pThemeData->clrDatesCircle;
  pParams->m_pGraphics->SetStrokeColor(&clrSelDayBK);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarTP::DrawTodayCircle(CFWL_ThemeBackground* pParams,
                                              CFX_Matrix* pMatrix) {
  CFX_Path path;
  path.Create();
  CFX_RectF rtTodayCircle;
  rtTodayCircle = pParams->m_rtPart;
  path.AddRectangle(rtTodayCircle.left, rtTodayCircle.top, rtTodayCircle.width,
                    rtTodayCircle.height);
  pParams->m_pGraphics->SaveGraphState();
  CFX_Color clrTodayCircle;
  clrTodayCircle = m_pThemeData->clrDatesCircle;
  pParams->m_pGraphics->SetStrokeColor(&clrTodayCircle);
  pParams->m_pGraphics->StrokePath(&path, pMatrix);
  pParams->m_pGraphics->RestoreGraphState();
  return TRUE;
}

FWLTHEME_STATE CFWL_MonthCalendarTP::GetState(uint32_t dwFWLStates) {
  if (dwFWLStates & CFWL_PartState_Hovered)
    return FWLTHEME_STATE_Hover;
  if (dwFWLStates & CFWL_PartState_Pressed)
    return FWLTHEME_STATE_Pressed;
  return FWLTHEME_STATE_Normal;
}

void CFWL_MonthCalendarTP::SetThemeData(uint32_t dwThemeID) {
  if (dwThemeID == 0) {
    m_pThemeData->clrCaption = ArgbEncode(0xff, 0, 153, 255);
    m_pThemeData->clrSeperator = ArgbEncode(0xff, 141, 161, 239);
    m_pThemeData->clrDatesHoverBK = ArgbEncode(0xff, 193, 211, 251);
    m_pThemeData->clrDatesSelectedBK = ArgbEncode(0xff, 173, 188, 239);
    m_pThemeData->clrDatesCircle = ArgbEncode(0xff, 103, 144, 209);
    m_pThemeData->clrToday = ArgbEncode(0xff, 0, 0, 0);
    m_pThemeData->clrBK = ArgbEncode(0xff, 255, 255, 255);
  } else {
    m_pThemeData->clrCaption = ArgbEncode(0xff, 128, 128, 0);
    m_pThemeData->clrSeperator = ArgbEncode(0xff, 128, 128, 64);
    m_pThemeData->clrDatesHoverBK = ArgbEncode(0xff, 217, 220, 191);
    m_pThemeData->clrDatesSelectedBK = ArgbEncode(0xff, 204, 208, 183);
    m_pThemeData->clrDatesCircle = ArgbEncode(0xff, 128, 128, 0);
    m_pThemeData->clrToday = ArgbEncode(0xff, 0, 0, 0);
    m_pThemeData->clrBK = ArgbEncode(0xff, 255, 255, 255);
  }
}
