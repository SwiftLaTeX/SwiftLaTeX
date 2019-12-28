// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_MONTHCALENDAR_H_
#define XFA_FWL_BASEWIDGET_IFWL_MONTHCALENDAR_H_

#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_MonthCalendar L"FWL_MONTHCALENDAR"
#define FWL_STYLEEXT_MCD_MultiSelect (1L << 0)
#define FWL_STYLEEXT_MCD_NoToday (1L << 1)
#define FWL_STYLEEXT_MCD_NoTodayCircle (1L << 2)
#define FWL_STYLEEXT_MCD_WeekNumbers (1L << 3)
#define FWL_ITEMSTATE_MCD_Nomal (0L << 0)
#define FWL_ITEMSTATE_MCD_Flag (1L << 0)
#define FWL_ITEMSTATE_MCD_Selected (1L << 1)
#define FWL_ITEMSTATE_MCD_Focused (1L << 2)

FWL_EVENT_DEF(CFWL_Event_McdDateSelected,
              CFWL_EventType::DataSelected,
              int32_t m_iStartDay;
              int32_t m_iEndDay;)

FWL_EVENT_DEF(CFWL_EventMcdDateChanged,
              CFWL_EventType::DateChanged,
              int32_t m_iOldYear;
              int32_t m_iOldMonth;
              int32_t m_iStartDay;
              int32_t m_iEndDay;)

class IFWL_MonthCalendarDP : public IFWL_DataProvider {
 public:
  virtual int32_t GetCurDay(IFWL_Widget* pWidget) = 0;
  virtual int32_t GetCurMonth(IFWL_Widget* pWidget) = 0;
  virtual int32_t GetCurYear(IFWL_Widget* pWidget) = 0;
};

class IFWL_MonthCalendar : public IFWL_Widget {
 public:
  static IFWL_MonthCalendar* Create(const CFWL_WidgetImpProperties& properties,
                                    IFWL_Widget* pOuter);

  int32_t CountSelect();
  FX_BOOL GetSelect(int32_t& iYear,
                    int32_t& iMonth,
                    int32_t& iDay,
                    int32_t nIndex = 0);
  FX_BOOL SetSelect(int32_t iYear, int32_t iMonth, int32_t iDay);

 protected:
  IFWL_MonthCalendar();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_MONTHCALENDAR_H_
