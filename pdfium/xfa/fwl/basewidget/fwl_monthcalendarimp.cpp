// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_monthcalendarimp.h"

#include <algorithm>

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_monthcalendar.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"

#define MONTHCAL_HSEP_HEIGHT 1
#define MONTHCAL_VSEP_WIDTH 1
#define MONTHCAL_HMARGIN 3
#define MONTHCAL_VMARGIN 2
#define MONTHCAL_ROWS 9
#define MONTHCAL_COLUMNS 7
#define MONTHCAL_HEADER_BTN_VMARGIN 7
#define MONTHCAL_HEADER_BTN_HMARGIN 5

namespace {

CFX_WideString* GetCapacityForDay(IFWL_ThemeProvider* pTheme,
                                  CFWL_ThemePart& params,
                                  uint32_t day) {
  ASSERT(day < 7);

  if (day == 0) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Sun));
  }
  if (day == 1) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Mon));
  }
  if (day == 2) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Tue));
  }
  if (day == 3) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Wed));
  }
  if (day == 4) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Thu));
  }
  if (day == 5) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Fri));
  }

  return static_cast<CFX_WideString*>(
      pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Sat));
}

CFX_WideString* GetCapacityForMonth(IFWL_ThemeProvider* pTheme,
                                    CFWL_ThemePart& params,
                                    uint32_t month) {
  ASSERT(month < 12);

  if (month == 0) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::January));
  }
  if (month == 1) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::February));
  }
  if (month == 2) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::March));
  }
  if (month == 3) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::April));
  }
  if (month == 4) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::May));
  }
  if (month == 5) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::June));
  }
  if (month == 6) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::July));
  }
  if (month == 7) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::August));
  }
  if (month == 8) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::September));
  }
  if (month == 9) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::October));
  }
  if (month == 10) {
    return static_cast<CFX_WideString*>(
        pTheme->GetCapacity(&params, CFWL_WidgetCapacity::November));
  }

  return static_cast<CFX_WideString*>(
      pTheme->GetCapacity(&params, CFWL_WidgetCapacity::December));
}

}  // namespace

// static
IFWL_MonthCalendar* IFWL_MonthCalendar::Create(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter) {
  IFWL_MonthCalendar* pMonthCalendar = new IFWL_MonthCalendar;
  CFWL_MonthCalendarImp* pMonthCalendarImpl =
      new CFWL_MonthCalendarImp(properties, pOuter);
  pMonthCalendar->SetImpl(pMonthCalendarImpl);
  pMonthCalendarImpl->SetInterface(pMonthCalendar);
  return pMonthCalendar;
}
IFWL_MonthCalendar::IFWL_MonthCalendar() {}
int32_t IFWL_MonthCalendar::CountSelect() {
  return static_cast<CFWL_MonthCalendarImp*>(GetImpl())->CountSelect();
}
FX_BOOL IFWL_MonthCalendar::GetSelect(int32_t& iYear,
                                      int32_t& iMonth,
                                      int32_t& iDay,
                                      int32_t nIndex) {
  return static_cast<CFWL_MonthCalendarImp*>(GetImpl())
      ->GetSelect(iYear, iMonth, iDay, nIndex);
}
FX_BOOL IFWL_MonthCalendar::SetSelect(int32_t iYear,
                                      int32_t iMonth,
                                      int32_t iDay) {
  return static_cast<CFWL_MonthCalendarImp*>(GetImpl())
      ->SetSelect(iYear, iMonth, iDay);
}

CFWL_MonthCalendarImp::CFWL_MonthCalendarImp(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_iCurYear(2011),
      m_iCurMonth(1),
      m_iYear(2011),
      m_iMonth(1),
      m_iDay(1),
      m_iHovered(-1),
      m_iLBtnPartStates(CFWL_PartState_Normal),
      m_iRBtnPartStates(CFWL_PartState_Normal) {
  m_rtHead.Reset();
  m_rtWeek.Reset();
  m_rtLBtn.Reset();
  m_rtRBtn.Reset();
  m_rtDates.Reset();
  m_rtHSep.Reset();
  m_rtHeadText.Reset();
  m_rtToday.Reset();
  m_rtTodayFlag.Reset();
  m_rtClient.Reset();
  m_rtWeekNum.Reset();
  m_rtWeekNumSep.Reset();
  m_pDateTime = new CFX_DateTime;
  m_bInit = FALSE;
  m_iMaxSel = 1;
}

CFWL_MonthCalendarImp::~CFWL_MonthCalendarImp() {
  ClearDateItem();
  delete m_pDateTime;
  m_arrSelDays.RemoveAll();
}

FWL_Error CFWL_MonthCalendarImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_MonthCalendar;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_MonthCalendarImp::GetClassID() const {
  return FWL_Type::MonthCalendar;
}

FWL_Error CFWL_MonthCalendarImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;

  m_pDelegate = new CFWL_MonthCalendarImpDelegate(this);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_MonthCalendarImp::Finalize() {
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_MonthCalendarImp::GetWidgetRect(CFX_RectF& rect,
                                               FX_BOOL bAutoSize) {
  if (bAutoSize) {
    CFX_SizeF fs = CalcSize(TRUE);
    rect.Set(0, 0, fs.x, fs.y);
    CFWL_WidgetImp::GetWidgetRect(rect, TRUE);
  } else {
    rect = m_pProperties->m_rtWidget;
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_MonthCalendarImp::Update() {
  if (IsLocked()) {
    return FWL_Error::Indefinite;
  }
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  GetCapValue();
  if (!m_bInit) {
    m_bInit = InitDate();
  }
  ClearDateItem();
  ReSetDateItem();
  LayOut();
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_MonthCalendarImp::DrawWidget(CFX_Graphics* pGraphics,
                                            const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  if (HasBorder()) {
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, pMatrix);
  }
  if (HasEdge()) {
    DrawEdge(pGraphics, CFWL_Part::Edge, pTheme, pMatrix);
  }
  DrawBkground(pGraphics, pTheme, pMatrix);
  DrawHeadBK(pGraphics, pTheme, pMatrix);
  DrawLButton(pGraphics, pTheme, pMatrix);
  DrawRButton(pGraphics, pTheme, pMatrix);
  DrawSeperator(pGraphics, pTheme, pMatrix);
  DrawDatesInBK(pGraphics, pTheme, pMatrix);
  DrawDatesInCircle(pGraphics, pTheme, pMatrix);
  DrawCaption(pGraphics, pTheme, pMatrix);
  DrawWeek(pGraphics, pTheme, pMatrix);
  DrawDatesIn(pGraphics, pTheme, pMatrix);
  DrawDatesOut(pGraphics, pTheme, pMatrix);
  DrawToday(pGraphics, pTheme, pMatrix);
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_WeekNumbers) {
    DrawWeekNumberSep(pGraphics, pTheme, pMatrix);
    DrawWeekNumber(pGraphics, pTheme, pMatrix);
  }
  return FWL_Error::Succeeded;
}
int32_t CFWL_MonthCalendarImp::CountSelect() {
  return m_arrSelDays.GetSize();
}
FX_BOOL CFWL_MonthCalendarImp::GetSelect(int32_t& iYear,
                                         int32_t& iMonth,
                                         int32_t& iDay,
                                         int32_t nIndex) {
  if (nIndex >= m_arrSelDays.GetSize()) {
    return FALSE;
  }
  iYear = m_iCurYear;
  iMonth = m_iCurMonth;
  iDay = m_arrSelDays[nIndex];
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::SetSelect(int32_t iYear,
                                         int32_t iMonth,
                                         int32_t iDay) {
  ChangeToMonth(iYear, iMonth);
  return AddSelDay(iDay);
}
void CFWL_MonthCalendarImp::DrawBkground(CFX_Graphics* pGraphics,
                                         IFWL_ThemeProvider* pTheme,
                                         const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::Background;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtClient;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawHeadBK(CFX_Graphics* pGraphics,
                                       IFWL_ThemeProvider* pTheme,
                                       const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::Header;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtHead;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawLButton(CFX_Graphics* pGraphics,
                                        IFWL_ThemeProvider* pTheme,
                                        const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::LBtn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = m_iLBtnPartStates;
  params.m_rtPart = m_rtLBtn;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawRButton(CFX_Graphics* pGraphics,
                                        IFWL_ThemeProvider* pTheme,
                                        const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::RBtn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = m_iRBtnPartStates;
  params.m_rtPart = m_rtRBtn;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawCaption(CFX_Graphics* pGraphics,
                                        IFWL_ThemeProvider* pTheme,
                                        const CFX_Matrix* pMatrix) {
  CFWL_ThemeText textParam;
  textParam.m_pWidget = m_pInterface;
  textParam.m_iPart = CFWL_Part::Caption;
  textParam.m_dwStates = CFWL_PartState_Normal;
  textParam.m_pGraphics = pGraphics;
  int32_t iYear;
  int32_t iMonth;
  iYear = m_iCurYear;
  iMonth = m_iCurMonth;
  CFX_WideString wsCation;
  GetHeadText(iYear, iMonth, wsCation);
  textParam.m_wsText = wsCation;
  m_szHead = CalcTextSize(textParam.m_wsText, m_pProperties->m_pThemeProvider);
  CalcHeadSize();
  textParam.m_rtPart = m_rtHeadText;
  textParam.m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
  textParam.m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  if (pMatrix) {
    textParam.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawText(&textParam);
}
void CFWL_MonthCalendarImp::DrawSeperator(CFX_Graphics* pGraphics,
                                          IFWL_ThemeProvider* pTheme,
                                          const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::HSeparator;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtHSep;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawDatesInBK(CFX_Graphics* pGraphics,
                                          IFWL_ThemeProvider* pTheme,
                                          const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::DateInBK;
  params.m_pGraphics = pGraphics;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  int32_t iCount = m_arrDates.GetSize();
  for (int32_t j = 0; j < iCount; j++) {
    FWL_DATEINFO* pDataInfo = m_arrDates.GetAt(j);
    if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Selected) {
      params.m_dwStates |= CFWL_PartState_Selected;
      if (((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoTodayCircle) ==
           0) &&
          pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Flag) {
        params.m_dwStates |= CFWL_PartState_Flagged;
      }
      if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Focused) {
        params.m_dwStates |= CFWL_PartState_Focused;
      }
    } else if (j == m_iHovered - 1) {
      params.m_dwStates |= CFWL_PartState_Hovered;
    } else if (pDataInfo->dwStates & FWL_ITEMSTATE_MCD_Flag) {
      params.m_dwStates = CFWL_PartState_Flagged;
      pTheme->DrawBackground(&params);
    }
    params.m_rtPart = pDataInfo->rect;
    pTheme->DrawBackground(&params);
    params.m_dwStates = 0;
  }
}
void CFWL_MonthCalendarImp::DrawWeek(CFX_Graphics* pGraphics,
                                     IFWL_ThemeProvider* pTheme,
                                     const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::Week;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  CFX_RectF rtDayOfWeek;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  for (int32_t i = 0; i < 7; i++) {
    rtDayOfWeek.Set(m_rtWeek.left + i * (m_szCell.x + MONTHCAL_HMARGIN * 2),
                    m_rtWeek.top, m_szCell.x, m_szCell.y);
    params.m_rtPart = rtDayOfWeek;
    params.m_wsText = *GetCapacityForDay(pTheme, params, i);
    params.m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
    pTheme->DrawText(&params);
  }
}
void CFWL_MonthCalendarImp::DrawWeekNumber(CFX_Graphics* pGraphics,
                                           IFWL_ThemeProvider* pTheme,
                                           const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::WeekNum;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_CenterLeft;
  CFX_WideString wsWeekNum;
  params.m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  int32_t iWeekNum = 0;
  int32_t iMonthNum = m_pDateTime->GetMonth();
  int32_t iDayNum = FX_DaysInMonth(m_iCurYear, iMonthNum);
  int32_t iTemp = 0;
  FX_FLOAT fVStartPos = m_rtClient.top + m_fHeadHei + m_fHSepHei;
  FX_FLOAT fHStartPos = m_rtClient.left;
  for (int32_t i = 1; i <= iDayNum; i += 7) {
    iTemp++;
    iWeekNum = CalWeekNumber(m_iCurYear, iMonthNum, i);
    m_rtWeekNum.Set(fHStartPos, fVStartPos + m_fDateCellHei * iTemp,
                    m_fWeekNumWid, m_fDateCellHei);
    wsWeekNum.Format(L"%d", iWeekNum);
    params.m_wsText = wsWeekNum;
    params.m_rtPart = m_rtWeekNum;
    pTheme->DrawText(&params);
  }
}
void CFWL_MonthCalendarImp::DrawWeekNumberSep(CFX_Graphics* pGraphics,
                                              IFWL_ThemeProvider* pTheme,
                                              const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::WeekNumSep;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtWeekNumSep;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawToday(CFX_Graphics* pGraphics,
                                      IFWL_ThemeProvider* pTheme,
                                      const CFX_Matrix* pMatrix) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoToday) {
    return;
  }
  CFWL_ThemeText params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::Today;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_CenterLeft;
  CFX_WideString* wsDay = static_cast<CFX_WideString*>(
      pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Today));
  CFX_WideString wsText;
  GetTodayText(m_iYear, m_iMonth, m_iDay, wsText);
  params.m_wsText = *wsDay + wsText;
  m_szToday = CalcTextSize(params.m_wsText, m_pProperties->m_pThemeProvider);
  CalcTodaySize();
  params.m_rtPart = m_rtToday;
  params.m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawText(&params);
}
void CFWL_MonthCalendarImp::DrawDatesIn(CFX_Graphics* pGraphics,
                                        IFWL_ThemeProvider* pTheme,
                                        const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::DatesIn;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  int32_t iCount = m_arrDates.GetSize();
  for (int32_t j = 0; j < iCount; j++) {
    FWL_DATEINFO* pDataInfo = m_arrDates.GetAt(j);
    params.m_wsText = pDataInfo->wsDay;
    params.m_rtPart = pDataInfo->rect;
    params.m_dwStates = pDataInfo->dwStates;
    if (j + 1 == m_iHovered) {
      params.m_dwStates |= CFWL_PartState_Hovered;
    }
    params.m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
    pTheme->DrawText(&params);
  }
}
void CFWL_MonthCalendarImp::DrawDatesOut(CFX_Graphics* pGraphics,
                                         IFWL_ThemeProvider* pTheme,
                                         const CFX_Matrix* pMatrix) {
  CFWL_ThemeText params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::DatesOut;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawText(&params);
}
void CFWL_MonthCalendarImp::DrawDatesInCircle(CFX_Graphics* pGraphics,
                                              IFWL_ThemeProvider* pTheme,
                                              const CFX_Matrix* pMatrix) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoTodayCircle) {
    return;
  }
  if (m_iMonth != m_iCurMonth || m_iYear != m_iCurYear) {
    return;
  }
  if (m_iDay < 1 || m_iDay > m_arrDates.GetSize()) {
    return;
  }
  FWL_DATEINFO* pDate = m_arrDates[m_iDay - 1];
  if (!pDate)
    return;
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::DateInCircle;
  params.m_pGraphics = pGraphics;
  params.m_rtPart = pDate->rect;
  params.m_dwStates = CFWL_PartState_Normal;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
void CFWL_MonthCalendarImp::DrawTodayCircle(CFX_Graphics* pGraphics,
                                            IFWL_ThemeProvider* pTheme,
                                            const CFX_Matrix* pMatrix) {
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoToday) {
    return;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoTodayCircle) {
    return;
  }
  CFWL_ThemeBackground params;
  params.m_pWidget = m_pInterface;
  params.m_iPart = CFWL_Part::TodayCircle;
  params.m_pGraphics = pGraphics;
  params.m_dwStates = CFWL_PartState_Normal;
  params.m_rtPart = m_rtTodayFlag;
  if (pMatrix) {
    params.m_matrix.Concat(*pMatrix);
  }
  pTheme->DrawBackground(&params);
}
CFX_SizeF CFWL_MonthCalendarImp::CalcSize(FX_BOOL bAutoSize) {
  if (!m_pProperties->m_pThemeProvider)
    return CFX_SizeF();

  if (!bAutoSize) {
    GetClientRect(m_rtClient);
    return CFX_SizeF(m_rtClient.width, m_rtClient.height);
  }

  CFX_SizeF fs;
  CFWL_ThemePart params;
  params.m_pWidget = m_pInterface;
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  FX_FLOAT fMaxWeekW = 0.0f;
  FX_FLOAT fMaxWeekH = 0.0f;

  for (uint32_t i = 0; i < 7; ++i) {
    CFX_SizeF sz = CalcTextSize(*GetCapacityForDay(pTheme, params, i),
                                m_pProperties->m_pThemeProvider);
    fMaxWeekW = (fMaxWeekW >= sz.x) ? fMaxWeekW : sz.x;
    fMaxWeekH = (fMaxWeekH >= sz.y) ? fMaxWeekH : sz.y;
  }

  FX_FLOAT fDayMaxW = 0.0f;
  FX_FLOAT fDayMaxH = 0.0f;
  for (int day = 10; day <= 31; day++) {
    CFX_WideString wsDay;
    wsDay.Format(L"%d", day);
    CFX_SizeF sz = CalcTextSize(wsDay, m_pProperties->m_pThemeProvider);
    fDayMaxW = (fDayMaxW >= sz.x) ? fDayMaxW : sz.x;
    fDayMaxH = (fDayMaxH >= sz.y) ? fDayMaxH : sz.y;
  }
  m_szCell.x = FX_FLOAT((fMaxWeekW >= fDayMaxW) ? (int)(fMaxWeekW + 0.5)
                                                : (int)(fDayMaxW + 0.5));
  m_szCell.y = (fMaxWeekH >= fDayMaxH) ? fMaxWeekH : fDayMaxH;
  fs.x = m_szCell.x * MONTHCAL_COLUMNS +
         MONTHCAL_HMARGIN * MONTHCAL_COLUMNS * 2 +
         MONTHCAL_HEADER_BTN_HMARGIN * 2;
  FX_FLOAT fMonthMaxW = 0.0f;
  FX_FLOAT fMonthMaxH = 0.0f;

  for (uint32_t i = 0; i < 12; ++i) {
    CFX_SizeF sz = CalcTextSize(*GetCapacityForMonth(pTheme, params, i),
                                m_pProperties->m_pThemeProvider);
    fMonthMaxW = (fMonthMaxW >= sz.x) ? fMonthMaxW : sz.x;
    fMonthMaxH = (fMonthMaxH >= sz.y) ? fMonthMaxH : sz.y;
  }
  CFX_WideString wsYear;
  GetHeadText(m_iYear, m_iMonth, wsYear);
  CFX_SizeF szYear = CalcTextSize(wsYear, m_pProperties->m_pThemeProvider);
  fMonthMaxH = std::max(fMonthMaxH, szYear.y);
  m_szHead = CFX_SizeF(fMonthMaxW + szYear.x, fMonthMaxH);
  fMonthMaxW = m_szHead.x + MONTHCAL_HEADER_BTN_HMARGIN * 2 + m_szCell.x * 2;
  fs.x = std::max(fs.x, fMonthMaxW);
  CFX_WideString wsToday;
  GetTodayText(m_iYear, m_iMonth, m_iDay, wsToday);
  CFX_WideString* wsText = static_cast<CFX_WideString*>(
      pTheme->GetCapacity(&params, CFWL_WidgetCapacity::Today));
  m_wsToday = *wsText + wsToday;
  m_szToday = CalcTextSize(wsToday, m_pProperties->m_pThemeProvider);
  m_szToday.y = (m_szToday.y >= m_szCell.y) ? m_szToday.y : m_szCell.y;
  fs.y = m_szCell.x + m_szCell.y * (MONTHCAL_ROWS - 2) + m_szToday.y +
         MONTHCAL_VMARGIN * MONTHCAL_ROWS * 2 + MONTHCAL_HEADER_BTN_VMARGIN * 4;
  return fs;
}

void CFWL_MonthCalendarImp::CalcHeadSize() {
  FX_FLOAT fHeadHMargin = (m_rtClient.width - m_szHead.x) / 2;
  FX_FLOAT fHeadVMargin = (m_szCell.x - m_szHead.y) / 2;
  m_rtHeadText.Set(m_rtClient.left + fHeadHMargin,
                   m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN +
                       MONTHCAL_VMARGIN + fHeadVMargin,
                   m_szHead.x, m_szHead.y);
}
void CFWL_MonthCalendarImp::CalcTodaySize() {
  m_rtTodayFlag.Set(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN,
      m_rtDates.bottom() + MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN,
      m_szCell.x, m_szToday.y);
  m_rtToday.Set(
      m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + m_szCell.x +
          MONTHCAL_HMARGIN * 2,
      m_rtDates.bottom() + MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN,
      m_szToday.x, m_szToday.y);
}
void CFWL_MonthCalendarImp::LayOut() {
  GetClientRect(m_rtClient);
  {
    m_rtHead.Set(
        m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN, m_rtClient.top,
        m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
        m_szCell.x + (MONTHCAL_HEADER_BTN_VMARGIN + MONTHCAL_VMARGIN) * 2);
    m_rtWeek.Set(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                 m_rtHead.bottom(),
                 m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
                 m_szCell.y + MONTHCAL_VMARGIN * 2);
    m_rtLBtn.Set(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                 m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN, m_szCell.x,
                 m_szCell.x);
    m_rtRBtn.Set(m_rtClient.left + m_rtClient.width -
                     MONTHCAL_HEADER_BTN_HMARGIN - m_szCell.x,
                 m_rtClient.top + MONTHCAL_HEADER_BTN_VMARGIN, m_szCell.x,
                 m_szCell.x);
    m_rtHSep.Set(
        m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN,
        m_rtWeek.bottom() - MONTHCAL_VMARGIN,
        m_rtClient.width - (MONTHCAL_HEADER_BTN_HMARGIN + MONTHCAL_HMARGIN) * 2,
        MONTHCAL_HSEP_HEIGHT);
    m_rtDates.Set(m_rtClient.left + MONTHCAL_HEADER_BTN_HMARGIN,
                  m_rtWeek.bottom(),
                  m_rtClient.width - MONTHCAL_HEADER_BTN_HMARGIN * 2,
                  m_szCell.y * (MONTHCAL_ROWS - 3) +
                      MONTHCAL_VMARGIN * (MONTHCAL_ROWS - 3) * 2);
  }
  CalDateItem();
}
void CFWL_MonthCalendarImp::CalDateItem() {
  FX_BOOL bNewWeek = FALSE;
  int32_t iWeekOfMonth = 0;
  FX_FLOAT fLeft = m_rtDates.left;
  FX_FLOAT fTop = m_rtDates.top;
  int32_t iCount = m_arrDates.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    FWL_DATEINFO* pDateInfo = m_arrDates.GetAt(i);
    if (bNewWeek) {
      iWeekOfMonth++;
      bNewWeek = FALSE;
    }
    pDateInfo->rect.Set(
        fLeft + pDateInfo->iDayOfWeek * (m_szCell.x + (MONTHCAL_HMARGIN * 2)),
        fTop + iWeekOfMonth * (m_szCell.y + (MONTHCAL_VMARGIN * 2)),
        m_szCell.x + (MONTHCAL_HMARGIN * 2),
        m_szCell.y + (MONTHCAL_VMARGIN * 2));
    if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_WeekNumbers) {
      pDateInfo->rect.Offset(m_fWeekNumWid, 0);
    }
    if (pDateInfo->iDayOfWeek >= 6) {
      bNewWeek = TRUE;
    }
  }
}
void CFWL_MonthCalendarImp::GetCapValue() {
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  CFWL_ThemePart part;
  part.m_pWidget = m_pInterface;
  m_fHeadWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderWidth));
  m_fHeadHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderHeight));
  m_fHeadBtnWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderBtnWidth));
  m_fHeadBtnHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderBtnHeight));
  m_fHeadBtnHMargin = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderBtnHMargin));
  m_fHeadBtnVMargin = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderBtnVMargin));
  m_fHeadTextWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderTextWidth));
  m_fHeadTextHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderTextHeight));
  m_fHeadTextHMargin = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderTextHMargin));
  m_fHeadTextVMargin = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HeaderTextVMargin));
  m_fHSepWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HSepWidth));
  m_fHSepHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::HSepHeight));
  m_fWeekNumWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::WeekNumWidth));
  m_fSepDOffset = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::SepDOffset));
  m_fSepX = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::SepX));
  m_fSepY = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::SepY));
  m_fWeekNumHeigh = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::WeekNumHeight));
  m_fWeekWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::WeekWidth));
  m_fWeekHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::WeekHeight));
  m_fDateCellWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::DatesCellWidth));
  m_fDateCellHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::DatesCellHeight));
  m_fTodayWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::TodayWidth));
  m_fTodayHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::TodayHeight));
  m_fTodayFlagWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::TodayFlagWidth));
  m_fMCWid = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::Width));
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_WeekNumbers) {
    m_fMCWid += m_fWeekNumWid;
  }
  m_fMCHei = *static_cast<FX_FLOAT*>(
      pTheme->GetCapacity(&part, CFWL_WidgetCapacity::Height));
}
int32_t CFWL_MonthCalendarImp::CalWeekNumber(int32_t iYear,
                                             int32_t iMonth,
                                             int32_t iDay) {
  return 0;
}
FX_BOOL CFWL_MonthCalendarImp::GetMinDate(int32_t& iYear,
                                          int32_t& iMonth,
                                          int32_t& iDay) {
  iYear = m_dtMin.iYear;
  iMonth = m_dtMin.iMonth;
  iDay = m_dtMin.iDay;
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::SetMinDate(int32_t iYear,
                                          int32_t iMonth,
                                          int32_t iDay) {
  m_dtMin = DATE(iYear, iMonth, iDay);
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::GetMaxDate(int32_t& iYear,
                                          int32_t& iMonth,
                                          int32_t& iDay) {
  iYear = m_dtMax.iYear;
  iMonth = m_dtMax.iMonth;
  iDay = m_dtMax.iDay;
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::SetMaxDate(int32_t iYear,
                                          int32_t iMonth,
                                          int32_t iDay) {
  m_dtMax = DATE(iYear, iMonth, iDay);
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::InitDate() {
  if (m_pProperties->m_pDataProvider) {
    IFWL_MonthCalendarDP* pDateProv =
        static_cast<IFWL_MonthCalendarDP*>(m_pProperties->m_pDataProvider);
    m_iYear = pDateProv->GetCurYear(m_pInterface);
    m_iMonth = pDateProv->GetCurMonth(m_pInterface);
    m_iDay = pDateProv->GetCurDay(m_pInterface);
    m_iCurYear = m_iYear;
    m_iCurMonth = m_iMonth;
  } else {
    m_iDay = 1;
    m_iMonth = 1;
    m_iYear = 1;
    m_iCurYear = m_iYear;
    m_iCurMonth = m_iMonth;
  }
  GetTodayText(m_iYear, m_iMonth, m_iDay, m_wsToday);
  GetHeadText(m_iCurYear, m_iCurMonth, m_wsHead);
  m_dtMin = DATE(1500, 12, 1);
  m_dtMax = DATE(2200, 1, 1);
  return TRUE;
}

void CFWL_MonthCalendarImp::ClearDateItem() {
  for (int32_t i = 0; i < m_arrDates.GetSize(); i++)
    delete m_arrDates.GetAt(i);

  m_arrDates.RemoveAll();
}

void CFWL_MonthCalendarImp::ReSetDateItem() {
  m_pDateTime->Set(m_iCurYear, m_iCurMonth, 1);
  int32_t iDays = FX_DaysInMonth(m_iCurYear, m_iCurMonth);
  int32_t iDayOfWeek = m_pDateTime->GetDayOfWeek();
  for (int32_t i = 0; i < iDays; i++) {
    if (iDayOfWeek >= 7) {
      iDayOfWeek = 0;
    }
    CFX_WideString wsDay;
    wsDay.Format(L"%d", i + 1);
    uint32_t dwStates = 0;
    if (m_iYear == m_iCurYear && m_iMonth == m_iCurMonth && m_iDay == (i + 1)) {
      dwStates |= FWL_ITEMSTATE_MCD_Flag;
    }
    if (m_arrSelDays.Find(i + 1) != -1) {
      dwStates |= FWL_ITEMSTATE_MCD_Selected;
    }
    CFX_RectF rtDate;
    rtDate.Set(0, 0, 0, 0);
    m_arrDates.Add(
        new FWL_DATEINFO(i + 1, iDayOfWeek, dwStates, rtDate, wsDay));
    iDayOfWeek++;
  }
}
FX_BOOL CFWL_MonthCalendarImp::NextMonth() {
  int32_t iYear = m_iCurYear, iMonth = m_iCurMonth;
  if (iMonth >= 12) {
    iMonth = 1;
    iYear++;
  } else {
    iMonth++;
  }
  DATE dt(m_iCurYear, m_iCurMonth, 1);
  if (!(dt < m_dtMax)) {
    return FALSE;
  }
  m_iCurYear = iYear, m_iCurMonth = iMonth;
  ChangeToMonth(m_iCurYear, m_iCurMonth);
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::PrevMonth() {
  int32_t iYear = m_iCurYear, iMonth = m_iCurMonth;
  if (iMonth <= 1) {
    iMonth = 12;
    iYear--;
  } else {
    iMonth--;
  }
  DATE dt(m_iCurYear, m_iCurMonth, 1);
  if (!(dt > m_dtMin)) {
    return FALSE;
  }
  m_iCurYear = iYear, m_iCurMonth = iMonth;
  ChangeToMonth(m_iCurYear, m_iCurMonth);
  return TRUE;
}
void CFWL_MonthCalendarImp::ChangeToMonth(int32_t iYear, int32_t iMonth) {
  m_iCurYear = iYear;
  m_iCurMonth = iMonth;
  m_iHovered = -1;
  ClearDateItem();
  ReSetDateItem();
  CalDateItem();
  GetHeadText(m_iCurYear, m_iCurMonth, m_wsHead);
}
FX_BOOL CFWL_MonthCalendarImp::RemoveSelDay(int32_t iDay, FX_BOOL bAll) {
  if (iDay == -1 && !bAll) {
    return FALSE;
  }
  if (bAll) {
    int32_t iCount = m_arrSelDays.GetSize();
    int32_t iDatesCount = m_arrDates.GetSize();
    for (int32_t i = 0; i < iCount; i++) {
      int32_t iSelDay = m_arrSelDays.GetAt(i);
      if (iSelDay <= iDatesCount) {
        FWL_DATEINFO* pDateInfo = m_arrDates.GetAt(iSelDay - 1);
        pDateInfo->dwStates &= ~FWL_ITEMSTATE_MCD_Selected;
      }
    }
    m_arrSelDays.RemoveAll();
  } else {
    int32_t index = m_arrSelDays.Find(iDay);
    if (index == -1) {
      return FALSE;
    }
    int32_t iSelDay = m_arrSelDays.GetAt(iDay);
    int32_t iDatesCount = m_arrDates.GetSize();
    if (iSelDay <= iDatesCount) {
      FWL_DATEINFO* pDateInfo = m_arrDates.GetAt(iSelDay - 1);
      pDateInfo->dwStates &= ~FWL_ITEMSTATE_MCD_Selected;
    }
    m_arrSelDays.RemoveAt(index);
  }
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::AddSelDay(int32_t iDay) {
  ASSERT(iDay > 0);
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_MultiSelect) {
  } else {
    if (m_arrSelDays.Find(iDay) == -1) {
      RemoveSelDay(-1, TRUE);
      if (iDay <= m_arrDates.GetSize()) {
        FWL_DATEINFO* pDateInfo = m_arrDates.GetAt(iDay - 1);
        pDateInfo->dwStates |= FWL_ITEMSTATE_MCD_Selected;
      }
      m_arrSelDays.Add(iDay);
    }
  }
  return TRUE;
}
FX_BOOL CFWL_MonthCalendarImp::JumpToToday() {
  if (m_iYear != m_iCurYear || m_iMonth != m_iCurMonth) {
    m_iCurYear = m_iYear;
    m_iCurMonth = m_iMonth;
    ChangeToMonth(m_iYear, m_iMonth);
    AddSelDay(m_iDay);
  } else {
    if (m_arrSelDays.Find(m_iDay) == -1) {
      AddSelDay(m_iDay);
    }
  }
  return TRUE;
}
void CFWL_MonthCalendarImp::GetHeadText(int32_t iYear,
                                        int32_t iMonth,
                                        CFX_WideString& wsHead) {
  ASSERT(iMonth > 0 && iMonth < 13);
  static const FX_WCHAR* const pMonth[] = {
      L"January",   L"February", L"March",    L"April",
      L"May",       L"June",     L"July",     L"August",
      L"September", L"October",  L"November", L"December"};
  wsHead.Format(L"%s, %d", pMonth[iMonth - 1], iYear);
}
void CFWL_MonthCalendarImp::GetTodayText(int32_t iYear,
                                         int32_t iMonth,
                                         int32_t iDay,
                                         CFX_WideString& wsToday) {
  wsToday.Format(L", %d/%d/%d", iDay, iMonth, iYear);
}
int32_t CFWL_MonthCalendarImp::GetDayAtPoint(FX_FLOAT x, FX_FLOAT y) {
  int32_t iCount = m_arrDates.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    FWL_DATEINFO* pDateInfo = m_arrDates.GetAt(i);
    if (pDateInfo->rect.Contains(x, y)) {
      return ++i;
    }
  }
  return -1;
}
FX_BOOL CFWL_MonthCalendarImp::GetDayRect(int32_t iDay, CFX_RectF& rtDay) {
  if (iDay <= 0 || iDay > m_arrDates.GetSize()) {
    return FALSE;
  }
  FWL_DATEINFO* pDateInfo = m_arrDates[iDay - 1];
  if (!pDateInfo)
    return FALSE;
  rtDay = pDateInfo->rect;
  return TRUE;
}

CFWL_MonthCalendarImpDelegate::CFWL_MonthCalendarImpDelegate(
    CFWL_MonthCalendarImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_MonthCalendarImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  CFWL_MessageType dwMsgCode = pMessage->GetClassID();
  switch (dwMsgCode) {
    case CFWL_MessageType::SetFocus: {
      OnFocusChanged(pMessage, TRUE);
      break;
    }
    case CFWL_MessageType::KillFocus: {
      OnFocusChanged(pMessage, FALSE);
      break;
    }
    case CFWL_MessageType::Key: {
      break;
    }
    case CFWL_MessageType::Mouse: {
      CFWL_MsgMouse* pMouse = static_cast<CFWL_MsgMouse*>(pMessage);
      switch (pMouse->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown: {
          OnLButtonDown(pMouse);
          break;
        }
        case FWL_MouseCommand::LeftButtonUp: {
          OnLButtonUp(pMouse);
          break;
        }
        case FWL_MouseCommand::Move: {
          OnMouseMove(pMouse);
          break;
        }
        case FWL_MouseCommand::Leave: {
          OnMouseLeave(pMouse);
          break;
        }
        default:
          break;
      }
      break;
    }
    default: {
      break;
    }
  }
  CFWL_WidgetImpDelegate::OnProcessMessage(pMessage);
}

void CFWL_MonthCalendarImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                                 const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}

void CFWL_MonthCalendarImpDelegate::OnActivate(CFWL_Message* pMsg) {}

void CFWL_MonthCalendarImpDelegate::OnFocusChanged(CFWL_Message* pMsg,
                                                   FX_BOOL bSet) {
  if (bSet) {
    m_pOwner->m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
  }
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_MonthCalendarImpDelegate::OnLButtonDown(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_rtLBtn.Contains(pMsg->m_fx, pMsg->m_fy)) {
    m_pOwner->m_iLBtnPartStates = CFWL_PartState_Pressed;
    m_pOwner->PrevMonth();
    m_pOwner->Repaint(&m_pOwner->m_rtClient);
  } else if (m_pOwner->m_rtRBtn.Contains(pMsg->m_fx, pMsg->m_fy)) {
    m_pOwner->m_iRBtnPartStates |= CFWL_PartState_Pressed;
    m_pOwner->NextMonth();
    m_pOwner->Repaint(&m_pOwner->m_rtClient);
  } else if (m_pOwner->m_rtToday.Contains(pMsg->m_fx, pMsg->m_fy)) {
    if ((m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_NoToday) ==
        0) {
      m_pOwner->JumpToToday();
      m_pOwner->Repaint(&m_pOwner->m_rtClient);
    }
  } else {
    if (m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_MultiSelect) {
    } else {
      int32_t iOldSel = 0;
      if (m_pOwner->m_arrSelDays.GetSize() > 0) {
        iOldSel = m_pOwner->m_arrSelDays[0];
      } else {
        return;
      }
      int32_t iCurSel = m_pOwner->GetDayAtPoint(pMsg->m_fx, pMsg->m_fy);
      FX_BOOL bSelChanged = iCurSel > 0 && iCurSel != iOldSel;
      if (bSelChanged) {
        FWL_DATEINFO* lpDatesInfo = m_pOwner->m_arrDates.GetAt(iCurSel - 1);
        CFX_RectF rtInvalidate(lpDatesInfo->rect);
        if (iOldSel > 0) {
          lpDatesInfo = m_pOwner->m_arrDates.GetAt(iOldSel - 1);
          rtInvalidate.Union(lpDatesInfo->rect);
        }
        m_pOwner->AddSelDay(iCurSel);
        CFWL_EvtClick wmClick;
        wmClick.m_pSrcTarget = m_pOwner->m_pInterface;
        m_pOwner->DispatchEvent(&wmClick);
        CFWL_EventMcdDateChanged wmDateSelected;
        wmDateSelected.m_iStartDay = iCurSel;
        wmDateSelected.m_iEndDay = iCurSel;
        wmDateSelected.m_iOldMonth = m_pOwner->m_iCurMonth;
        wmDateSelected.m_iOldYear = m_pOwner->m_iCurYear;
        wmDateSelected.m_pSrcTarget = m_pOwner->m_pInterface;
        m_pOwner->DispatchEvent(&wmDateSelected);
        m_pOwner->Repaint(&rtInvalidate);
      }
    }
  }
}
void CFWL_MonthCalendarImpDelegate::OnLButtonUp(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_rtLBtn.Contains(pMsg->m_fx, pMsg->m_fy)) {
    m_pOwner->m_iLBtnPartStates = 0;
    m_pOwner->Repaint(&m_pOwner->m_rtLBtn);
  } else if (m_pOwner->m_rtRBtn.Contains(pMsg->m_fx, pMsg->m_fy)) {
    m_pOwner->m_iRBtnPartStates = 0;
    m_pOwner->Repaint(&m_pOwner->m_rtRBtn);
  } else if (m_pOwner->m_rtDates.Contains(pMsg->m_fx, pMsg->m_fy)) {
    int32_t iDay = m_pOwner->GetDayAtPoint(pMsg->m_fx, pMsg->m_fy);
    if (iDay != -1) {
      m_pOwner->AddSelDay(iDay);
    }
  }
}
void CFWL_MonthCalendarImpDelegate::OnMouseMove(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_pProperties->m_dwStyleExes & FWL_STYLEEXT_MCD_MultiSelect) {
    return;
  }
  FX_BOOL bRepaint = FALSE;
  CFX_RectF rtInvalidate;
  rtInvalidate.Set(0, 0, 0, 0);
  if (m_pOwner->m_rtDates.Contains(pMsg->m_fx, pMsg->m_fy)) {
    int32_t iHover = m_pOwner->GetDayAtPoint(pMsg->m_fx, pMsg->m_fy);
    bRepaint = m_pOwner->m_iHovered != iHover;
    if (bRepaint) {
      if (m_pOwner->m_iHovered > 0) {
        m_pOwner->GetDayRect(m_pOwner->m_iHovered, rtInvalidate);
      }
      if (iHover > 0) {
        CFX_RectF rtDay;
        m_pOwner->GetDayRect(iHover, rtDay);
        if (rtInvalidate.IsEmpty()) {
          rtInvalidate = rtDay;
        } else {
          rtInvalidate.Union(rtDay);
        }
      }
    }
    m_pOwner->m_iHovered = iHover;
  } else {
    bRepaint = m_pOwner->m_iHovered > 0;
    if (bRepaint) {
      m_pOwner->GetDayRect(m_pOwner->m_iHovered, rtInvalidate);
    }
    m_pOwner->m_iHovered = -1;
  }
  if (bRepaint && !rtInvalidate.IsEmpty()) {
    m_pOwner->Repaint(&rtInvalidate);
  }
}
void CFWL_MonthCalendarImpDelegate::OnMouseLeave(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_iHovered > 0) {
    CFX_RectF rtInvalidate;
    rtInvalidate.Set(0, 0, 0, 0);
    m_pOwner->GetDayRect(m_pOwner->m_iHovered, rtInvalidate);
    m_pOwner->m_iHovered = -1;
    if (!rtInvalidate.IsEmpty()) {
      m_pOwner->Repaint(&rtInvalidate);
    }
  }
}

FWL_DATEINFO::FWL_DATEINFO(int32_t day,
                           int32_t dayofweek,
                           uint32_t dwSt,
                           CFX_RectF rc,
                           CFX_WideString& wsday)
    : iDay(day),
      iDayOfWeek(dayofweek),
      dwStates(dwSt),
      rect(rc),
      wsDay(wsday) {}

FWL_DATEINFO::~FWL_DATEINFO() {}
