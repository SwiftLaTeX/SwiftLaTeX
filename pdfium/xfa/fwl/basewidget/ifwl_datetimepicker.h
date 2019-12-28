// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_DATETIMEPICKER_H_
#define XFA_FWL_BASEWIDGET_IFWL_DATETIMEPICKER_H_

#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_DateTimePicker L"FWL_DATETIMEPICKER"
#define FWL_STYLEEXT_DTP_AllowEdit (1L << 0)
#define FWL_STYLEEXT_DTP_LongDateFormat (0L << 1)
#define FWL_STYLEEXT_DTP_ShortDateFormat (1L << 1)
#define FWL_STYLEEXT_DTP_TimeFormat (2L << 1)
#define FWL_STYLEEXT_DTP_Spin (1L << 3)
#define FWL_STYLEEXT_DTP_EditHNear (0L << 4)
#define FWL_STYLEEXT_DTP_EditHCenter (1L << 4)
#define FWL_STYLEEXT_DTP_EditHFar (2L << 4)
#define FWL_STYLEEXT_DTP_EditVNear (0L << 6)
#define FWL_STYLEEXT_DTP_EditVCenter (1L << 6)
#define FWL_STYLEEXT_DTP_EditVFar (2L << 6)
#define FWL_STYLEEXT_DTP_EditJustified (1L << 8)
#define FWL_STYLEEXT_DTP_EditDistributed (2L << 8)
#define FWL_STYLEEXT_DTP_EditHAlignMask (3L << 4)
#define FWL_STYLEEXT_DTP_EditVAlignMask (3L << 6)
#define FWL_STYLEEXT_DTP_EditHAlignModeMask (3L << 8)

FWL_EVENT_DEF(CFWL_Event_DtpDropDown, CFWL_EventType::DropDown)

FWL_EVENT_DEF(CFWL_Event_DtpCloseUp, CFWL_EventType::CloseUp)

FWL_EVENT_DEF(CFWL_Event_DtpEditChanged,
              CFWL_EventType::EditChanged,
              CFX_WideString m_wsText;)

FWL_EVENT_DEF(CFWL_Event_DtpHoverChanged,
              CFWL_EventType::HoverChanged,
              int32_t hoverday;)

FWL_EVENT_DEF(CFWL_Event_DtpSelectChanged,
              CFWL_EventType::SelectChanged,
              int32_t iYear;
              int32_t iMonth;
              int32_t iDay;)

class IFWL_DateTimePickerDP : public IFWL_DataProvider {
 public:
  virtual FWL_Error GetToday(IFWL_Widget* pWidget,
                             int32_t& iYear,
                             int32_t& iMonth,
                             int32_t& iDay) = 0;
};

class IFWL_DateTimePicker : public IFWL_Widget {
 public:
  static IFWL_DateTimePicker* Create(const CFWL_WidgetImpProperties& properties,
                                     IFWL_Widget* pOuter);

  FWL_Error GetCurSel(int32_t& iYear, int32_t& iMonth, int32_t& iDay);
  FWL_Error SetCurSel(int32_t iYear, int32_t iMonth, int32_t iDay);
  FWL_Error SetEditText(const CFX_WideString& wsText);
  FWL_Error GetEditText(CFX_WideString& wsText,
                        int32_t nStart = 0,
                        int32_t nCount = -1) const;
  int32_t CountSelRanges();
  int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  FX_BOOL CanUndo();
  FX_BOOL CanRedo();
  FX_BOOL Undo();
  FX_BOOL Redo();
  FX_BOOL CanCopy();
  FX_BOOL CanCut();
  FX_BOOL CanSelectAll();
  FX_BOOL Copy(CFX_WideString& wsCopy);
  FX_BOOL Cut(CFX_WideString& wsCut);
  FX_BOOL Paste(const CFX_WideString& wsPaste);
  FX_BOOL SelectAll();
  FX_BOOL Delete();
  FX_BOOL DeSelect();
  FWL_Error GetBBox(CFX_RectF& rect);
  FWL_Error SetEditLimit(int32_t nLimit);
  FWL_Error ModifyEditStylesEx(uint32_t dwStylesExAdded,
                               uint32_t dwStylesExRemoved);

 protected:
  IFWL_DateTimePicker();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_DATETIMEPICKER_H_
