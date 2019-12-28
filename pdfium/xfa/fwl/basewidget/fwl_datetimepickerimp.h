// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_DATETIMEPICKERIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_DATETIMEPICKERIMP_H_

#include <memory>

#include "xfa/fwl/basewidget/fwl_editimp.h"
#include "xfa/fwl/basewidget/fwl_monthcalendarimp.h"
#include "xfa/fwl/basewidget/ifwl_datetimepicker.h"
#include "xfa/fwl/basewidget/ifwl_edit.h"
#include "xfa/fwl/basewidget/ifwl_monthcalendar.h"
#include "xfa/fwl/core/ifwl_form.h"

class CFWL_WidgetImp;
class CFWL_WidgetImpProperties;
class CFWL_WidgetImpDelegate;
class CFWL_DateTimeEdit;
class CFWL_DateTimeEditImpDelegate;
class CFWL_DateTimeCalendar;
class CFWL_DateTimeCalendarImpDelegate;
class CFWL_DateTimePickerImp;
class CFWL_DateTimePickerImpDelegate;

class IFWL_DateTimeForm : public IFWL_Form {
 public:
  static IFWL_DateTimeForm* Create(const CFWL_WidgetImpProperties& properties,
                                   IFWL_Widget* pOuter);

 protected:
  IFWL_DateTimeForm() {}
};

class IFWL_DateTimeCalender : public IFWL_MonthCalendar {
 public:
  static IFWL_DateTimeCalender* Create(
      const CFWL_WidgetImpProperties& properties,
      IFWL_Widget* pOuter);

 protected:
  IFWL_DateTimeCalender() {}
};

class IFWL_DateTimeEdit : public IFWL_Edit {
 public:
  static IFWL_DateTimeEdit* Create(const CFWL_WidgetImpProperties& properties,
                                   IFWL_Widget* pOuter);

 protected:
  IFWL_DateTimeEdit() {}
};

class CFWL_DateTimeEdit : public CFWL_EditImp {
 public:
  CFWL_DateTimeEdit(const CFWL_WidgetImpProperties& properties,
                    IFWL_Widget* pOuter);

  // CFWL_EditImp
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;

 protected:
  friend class CFWL_DateTimeEditImpDelegate;
};

class CFWL_DateTimeEditImpDelegate : public CFWL_EditImpDelegate {
 public:
  CFWL_DateTimeEditImpDelegate(CFWL_DateTimeEdit* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;

 protected:
  CFWL_DateTimeEdit* m_pOwner;

 private:
  void DisForm_OnProcessMessage(CFWL_Message* pMessage);
};

class CFWL_DateTimeCalendar : public CFWL_MonthCalendarImp {
 public:
  CFWL_DateTimeCalendar(const CFWL_WidgetImpProperties& properties,
                        IFWL_Widget* pOuter);

  // CFWL_MonthCalendarImp
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;

 protected:
  friend class CFWL_DateTimeCalendarImpDelegate;
};

class CFWL_DateTimeCalendarImpDelegate : public CFWL_MonthCalendarImpDelegate {
 public:
  CFWL_DateTimeCalendarImpDelegate(CFWL_DateTimeCalendar* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;

  void OnLButtonDownEx(CFWL_MsgMouse* pMsg);
  void OnLButtonUpEx(CFWL_MsgMouse* pMsg);
  void OnMouseMoveEx(CFWL_MsgMouse* pMsg);

 protected:
  CFWL_DateTimeCalendar* m_pOwner;
  FX_BOOL m_bFlag;

 private:
  void DisForm_OnProcessMessage(CFWL_Message* pMessage);
  void DisForm_OnLButtonUpEx(CFWL_MsgMouse* pMsg);
};

class CFWL_DateTimePickerImp : public CFWL_WidgetImp {
 public:
  CFWL_DateTimePickerImp(const CFWL_WidgetImpProperties& properties,
                         IFWL_Widget* pOuter);
  ~CFWL_DateTimePickerImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error Update() override;
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy) override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  FWL_Error SetThemeProvider(IFWL_ThemeProvider* pTP) override;

  FWL_Error GetCurSel(int32_t& iYear, int32_t& iMonth, int32_t& iDay);
  FWL_Error SetCurSel(int32_t iYear, int32_t iMonth, int32_t iDay);
  FWL_Error SetEditText(const CFX_WideString& wsText);
  FWL_Error GetEditText(CFX_WideString& wsText,
                        int32_t nStart = 0,
                        int32_t nCount = -1) const;

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
  IFWL_DateTimeEdit* GetDataTimeEdit();

 protected:
  friend class CFWL_DateTimeEditImpDelegate;
  friend class CFWL_DateTimeCalendar;
  friend class CFWL_DateTimeCalendarImpDelegate;
  friend class CFWL_DateTimePickerImpDelegate;

  class CFWL_MonthCalendarImpDP : public IFWL_MonthCalendarDP {
   public:
    CFWL_MonthCalendarImpDP();

    // IFWL_DataProvider
    FWL_Error GetCaption(IFWL_Widget* pWidget,
                         CFX_WideString& wsCaption) override;

    // IFWL_MonthCalendarDP
    int32_t GetCurDay(IFWL_Widget* pWidget) override;
    int32_t GetCurMonth(IFWL_Widget* pWidget) override;
    int32_t GetCurYear(IFWL_Widget* pWidget) override;

    int32_t m_iCurDay;
    int32_t m_iCurYear;
    int32_t m_iCurMonth;
  };

  void DrawDropDownButton(CFX_Graphics* pGraphics,
                          IFWL_ThemeProvider* pTheme,
                          const CFX_Matrix* pMatrix);
  void FormatDateString(int32_t iYear,
                        int32_t iMonth,
                        int32_t iDay,
                        CFX_WideString& wsText);
  void ShowMonthCalendar(FX_BOOL bActivate);
  FX_BOOL IsMonthCalendarShowed();
  void ReSetEditAlignment();
  void InitProxyForm();
  void ProcessSelChanged(int32_t iYear, int32_t iMonth, int32_t iDay);

  CFX_RectF m_rtBtn;
  CFX_RectF m_rtClient;
  int32_t m_iBtnState;
  int32_t m_iYear;
  int32_t m_iMonth;
  int32_t m_iDay;
  FX_BOOL m_bLBtnDown;
  std::unique_ptr<IFWL_DateTimeEdit> m_pEdit;
  std::unique_ptr<IFWL_DateTimeCalender> m_pMonthCal;
  std::unique_ptr<IFWL_DateTimeForm> m_pForm;
  FX_FLOAT m_fBtn;
  CFWL_MonthCalendarImpDP m_MonthCalendarDP;

 private:
  FWL_Error DisForm_Initialize();
  void DisForm_InitDateTimeCalendar();
  void DisForm_InitDateTimeEdit();
  FX_BOOL DisForm_IsMonthCalendarShowed();
  void DisForm_ShowMonthCalendar(FX_BOOL bActivate);
  FWL_WidgetHit DisForm_HitTest(FX_FLOAT fx, FX_FLOAT fy);
  FX_BOOL DisForm_IsNeedShowButton();
  FWL_Error DisForm_Update();
  FWL_Error DisForm_GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE);
  FWL_Error DisForm_GetBBox(CFX_RectF& rect);
  FWL_Error DisForm_DrawWidget(CFX_Graphics* pGraphics,
                               const CFX_Matrix* pMatrix = nullptr);
};

class CFWL_DateTimePickerImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_DateTimePickerImpDelegate(CFWL_DateTimePickerImp* pOwner);

  // CFWL_WidgetImpDelegate
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);

  CFWL_DateTimePickerImp* m_pOwner;

 private:
  void DisForm_OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
};

#endif  // XFA_FWL_BASEWIDGET_FWL_DATETIMEPICKERIMP_H_
