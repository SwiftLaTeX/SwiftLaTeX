// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_MONTHCALENDARIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_MONTHCALENDARIMP_H_

#include "xfa/fgas/localization/fgas_datetime.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_widget.h"

class CFWL_MonthCalendarImpDelegate;
class CFWL_MsgMouse;
class CFWL_WidgetImpProperties;
class IFWL_Widget;

struct FWL_DATEINFO;

extern uint8_t FX_DaysInMonth(int32_t iYear, uint8_t iMonth);

class CFWL_MonthCalendarImp : public CFWL_WidgetImp {
 public:
  CFWL_MonthCalendarImp(const CFWL_WidgetImpProperties& properties,
                        IFWL_Widget* pOuter);
  ~CFWL_MonthCalendarImp() override;

  // FWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  int32_t CountSelect();
  FX_BOOL GetSelect(int32_t& iYear,
                    int32_t& iMonth,
                    int32_t& iDay,
                    int32_t nIndex = 0);
  FX_BOOL SetSelect(int32_t iYear, int32_t iMonth, int32_t iDay);

 protected:
  struct DATE {
    DATE() : iYear(0), iMonth(0), iDay(0) {}
    DATE(int32_t year, int32_t month, int32_t day)
        : iYear(year), iMonth(month), iDay(day) {}
    FX_BOOL operator<(const DATE& right) {
      if (iYear < right.iYear) {
        return TRUE;
      } else if (iYear == right.iYear) {
        if (iMonth < right.iMonth) {
          return TRUE;
        } else if (iMonth == right.iMonth) {
          return iDay < right.iDay;
        }
      }
      return FALSE;
    }
    FX_BOOL operator>(const DATE& right) {
      if (iYear > right.iYear) {
        return TRUE;
      } else if (iYear == right.iYear) {
        if (iMonth > right.iMonth) {
          return TRUE;
        } else if (iMonth == right.iMonth) {
          return iDay > right.iDay;
        }
      }
      return FALSE;
    }
    int32_t iYear;
    int32_t iMonth;
    int32_t iDay;
  };

  void DrawBkground(CFX_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix);
  void DrawHeadBK(CFX_Graphics* pGraphics,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix* pMatrix);
  void DrawLButton(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawRButton(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawCaption(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawSeperator(CFX_Graphics* pGraphics,
                     IFWL_ThemeProvider* pTheme,
                     const CFX_Matrix* pMatrix);
  void DrawDatesInBK(CFX_Graphics* pGraphics,
                     IFWL_ThemeProvider* pTheme,
                     const CFX_Matrix* pMatrix);
  void DrawWeek(CFX_Graphics* pGraphics,
                IFWL_ThemeProvider* pTheme,
                const CFX_Matrix* pMatrix);
  void DrawWeekNumber(CFX_Graphics* pGraphics,
                      IFWL_ThemeProvider* pTheme,
                      const CFX_Matrix* pMatrix);
  void DrawWeekNumberSep(CFX_Graphics* pGraphics,
                         IFWL_ThemeProvider* pTheme,
                         const CFX_Matrix* pMatrix);
  void DrawToday(CFX_Graphics* pGraphics,
                 IFWL_ThemeProvider* pTheme,
                 const CFX_Matrix* pMatrix);
  void DrawDatesIn(CFX_Graphics* pGraphics,
                   IFWL_ThemeProvider* pTheme,
                   const CFX_Matrix* pMatrix);
  void DrawDatesOut(CFX_Graphics* pGraphics,
                    IFWL_ThemeProvider* pTheme,
                    const CFX_Matrix* pMatrix);
  void DrawDatesInCircle(CFX_Graphics* pGraphics,
                         IFWL_ThemeProvider* pTheme,
                         const CFX_Matrix* pMatrix);
  void DrawTodayCircle(CFX_Graphics* pGraphics,
                       IFWL_ThemeProvider* pTheme,
                       const CFX_Matrix* pMatrix);
  CFX_SizeF CalcSize(FX_BOOL bAutoSize = FALSE);
  void LayOut();
  void CalcHeadSize();
  void CalcTodaySize();
  void CalDateItem();
  void GetCapValue();
  int32_t CalWeekNumber(int32_t iYear, int32_t iMonth, int32_t iDay);
  FX_BOOL GetMinDate(int32_t& iYear, int32_t& iMonth, int32_t& iDay);
  FX_BOOL SetMinDate(int32_t iYear, int32_t iMonth, int32_t iDay);
  FX_BOOL GetMaxDate(int32_t& iYear, int32_t& iMonth, int32_t& iDay);
  FX_BOOL SetMaxDate(int32_t iYear, int32_t iMonth, int32_t iDay);
  FX_BOOL InitDate();
  void ClearDateItem();
  void ReSetDateItem();
  FX_BOOL NextMonth();
  FX_BOOL PrevMonth();
  void ChangeToMonth(int32_t iYear, int32_t iMonth);
  FX_BOOL RemoveSelDay(int32_t iDay, FX_BOOL bAll = FALSE);
  FX_BOOL AddSelDay(int32_t iDay);
  FX_BOOL JumpToToday();
  void GetHeadText(int32_t iYear, int32_t iMonth, CFX_WideString& wsHead);
  void GetTodayText(int32_t iYear,
                    int32_t iMonth,
                    int32_t iDay,
                    CFX_WideString& wsToday);
  int32_t GetDayAtPoint(FX_FLOAT x, FX_FLOAT y);
  FX_BOOL GetDayRect(int32_t iDay, CFX_RectF& rtDay);

  FX_BOOL m_bInit;
  CFX_RectF m_rtHead;
  CFX_RectF m_rtWeek;
  CFX_RectF m_rtLBtn;
  CFX_RectF m_rtRBtn;
  CFX_RectF m_rtDates;
  CFX_RectF m_rtHSep;
  CFX_RectF m_rtHeadText;
  CFX_RectF m_rtToday;
  CFX_RectF m_rtTodayFlag;
  CFX_RectF m_rtWeekNum;
  CFX_RectF m_rtWeekNumSep;
  CFX_RectF m_rtTemp;
  CFX_WideString m_wsHead;
  CFX_WideString m_wsToday;
  CFX_DateTime* m_pDateTime;
  CFX_ArrayTemplate<FWL_DATEINFO*> m_arrDates;
  int32_t m_iCurYear;
  int32_t m_iCurMonth;
  int32_t m_iYear;
  int32_t m_iMonth;
  int32_t m_iDay;
  int32_t m_iHovered;
  int32_t m_iLBtnPartStates;
  int32_t m_iRBtnPartStates;
  DATE m_dtMin;
  DATE m_dtMax;
  CFX_SizeF m_szHead;
  CFX_SizeF m_szCell;
  CFX_SizeF m_szToday;
  CFX_ArrayTemplate<int32_t> m_arrSelDays;
  int32_t m_iMaxSel;
  CFX_RectF m_rtClient;
  FX_FLOAT m_fHeadWid;
  FX_FLOAT m_fHeadHei;
  FX_FLOAT m_fHeadBtnWid;
  FX_FLOAT m_fHeadBtnHei;
  FX_FLOAT m_fHeadBtnHMargin;
  FX_FLOAT m_fHeadBtnVMargin;
  FX_FLOAT m_fHeadTextWid;
  FX_FLOAT m_fHeadTextHei;
  FX_FLOAT m_fHeadTextHMargin;
  FX_FLOAT m_fHeadTextVMargin;
  FX_FLOAT m_fHSepWid;
  FX_FLOAT m_fHSepHei;
  FX_FLOAT m_fWeekNumWid;
  FX_FLOAT m_fSepDOffset;
  FX_FLOAT m_fSepX;
  FX_FLOAT m_fSepY;
  FX_FLOAT m_fWeekNumHeigh;
  FX_FLOAT m_fWeekWid;
  FX_FLOAT m_fWeekHei;
  FX_FLOAT m_fDateCellWid;
  FX_FLOAT m_fDateCellHei;
  FX_FLOAT m_fTodayWid;
  FX_FLOAT m_fTodayHei;
  FX_FLOAT m_fTodayFlagWid;
  FX_FLOAT m_fMCWid;
  FX_FLOAT m_fMCHei;
  friend class CFWL_MonthCalendarImpDelegate;
};

struct FWL_DATEINFO {
  FWL_DATEINFO(int32_t day,
               int32_t dayofweek,
               uint32_t dwSt,
               CFX_RectF rc,
               CFX_WideString& wsday);
  ~FWL_DATEINFO();

  int32_t iDay;
  int32_t iDayOfWeek;
  uint32_t dwStates;
  CFX_RectF rect;
  CFX_WideString wsDay;
};

class CFWL_MonthCalendarImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_MonthCalendarImpDelegate(CFWL_MonthCalendarImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnActivate(CFWL_Message* pMsg);
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);
  CFWL_MonthCalendarImp* m_pOwner;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_MONTHCALENDARIMP_H_
