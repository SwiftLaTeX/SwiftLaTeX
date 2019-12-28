// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/lightwidget/cfwl_datetimepicker.h"

#include <memory>

#include "xfa/fwl/basewidget/ifwl_datetimepicker.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/core/ifwl_widget.h"

CFWL_DateTimePicker* CFWL_DateTimePicker::Create() {
  return new CFWL_DateTimePicker;
}

FWL_Error CFWL_DateTimePicker::Initialize(
    const CFWL_WidgetProperties* pProperties) {
  if (m_pIface)
    return FWL_Error::Indefinite;
  if (pProperties) {
    *m_pProperties = *pProperties;
  }
  std::unique_ptr<IFWL_DateTimePicker> pDateTimePicker(
      IFWL_DateTimePicker::Create(
          m_pProperties->MakeWidgetImpProperties(&m_DateTimePickerDP),
          nullptr));
  FWL_Error ret = pDateTimePicker->Initialize();
  if (ret != FWL_Error::Succeeded) {
    return ret;
  }
  m_pIface = pDateTimePicker.release();
  CFWL_Widget::Initialize();
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_DateTimePicker::SetToday(int32_t iYear,
                                        int32_t iMonth,
                                        int32_t iDay) {
  m_DateTimePickerDP.m_iYear = iYear;
  m_DateTimePickerDP.m_iMonth = iMonth;
  m_DateTimePickerDP.m_iDay = iDay;
  return FWL_Error::Succeeded;
}

int32_t CFWL_DateTimePicker::CountSelRanges() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CountSelRanges();
}

int32_t CFWL_DateTimePicker::GetSelRange(int32_t nIndex, int32_t& nStart) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)
      ->GetSelRange(nIndex, nStart);
}

FWL_Error CFWL_DateTimePicker::GetEditText(CFX_WideString& wsText) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->GetEditText(wsText);
}

FWL_Error CFWL_DateTimePicker::SetEditText(const CFX_WideString& wsText) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->SetEditText(wsText);
}

FWL_Error CFWL_DateTimePicker::GetCurSel(int32_t& iYear,
                                         int32_t& iMonth,
                                         int32_t& iDay) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)
      ->GetCurSel(iYear, iMonth, iDay);
}

FWL_Error CFWL_DateTimePicker::SetCurSel(int32_t iYear,
                                         int32_t iMonth,
                                         int32_t iDay) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)
      ->SetCurSel(iYear, iMonth, iDay);
}

CFWL_DateTimePicker::CFWL_DateTimePicker() {}

CFWL_DateTimePicker::~CFWL_DateTimePicker() {}

CFWL_DateTimePicker::CFWL_DateTimePickerDP::CFWL_DateTimePickerDP() {
  m_iYear = 2011;
  m_iMonth = 1;
  m_iDay = 1;
}

FWL_Error CFWL_DateTimePicker::CFWL_DateTimePickerDP::GetCaption(
    IFWL_Widget* pWidget,
    CFX_WideString& wsCaption) {
  wsCaption = m_wsData;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_DateTimePicker::CFWL_DateTimePickerDP::GetToday(
    IFWL_Widget* pWidget,
    int32_t& iYear,
    int32_t& iMonth,
    int32_t& iDay) {
  iYear = m_iYear;
  iMonth = m_iMonth;
  iDay = m_iDay;
  return FWL_Error::Succeeded;
}

FX_BOOL CFWL_DateTimePicker::CanUndo() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CanUndo();
}

FX_BOOL CFWL_DateTimePicker::CanRedo() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CanRedo();
}

FX_BOOL CFWL_DateTimePicker::Undo() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Undo();
}

FX_BOOL CFWL_DateTimePicker::Redo() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Redo();
}

FX_BOOL CFWL_DateTimePicker::CanCopy() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CanCopy();
}

FX_BOOL CFWL_DateTimePicker::CanCut() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CanCut();
}

FX_BOOL CFWL_DateTimePicker::CanSelectAll() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->CanSelectAll();
}

FX_BOOL CFWL_DateTimePicker::Copy(CFX_WideString& wsCopy) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Copy(wsCopy);
}

FX_BOOL CFWL_DateTimePicker::Cut(CFX_WideString& wsCut) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Copy(wsCut);
}

FX_BOOL CFWL_DateTimePicker::Paste(const CFX_WideString& wsPaste) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Paste(wsPaste);
}

FX_BOOL CFWL_DateTimePicker::SelectAll() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->SelectAll();
}

FX_BOOL CFWL_DateTimePicker::Delete() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->Delete();
}

FX_BOOL CFWL_DateTimePicker::DeSelect() {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->DeSelect();
}

FWL_Error CFWL_DateTimePicker::GetBBox(CFX_RectF& rect) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->GetBBox(rect);
}

FWL_Error CFWL_DateTimePicker::SetEditLimit(int32_t nLimit) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)->SetEditLimit(nLimit);
}

FWL_Error CFWL_DateTimePicker::ModifyEditStylesEx(uint32_t dwStylesExAdded,
                                                  uint32_t dwStylesExRemoved) {
  return static_cast<IFWL_DateTimePicker*>(m_pIface)
      ->ModifyEditStylesEx(dwStylesExAdded, dwStylesExRemoved);
}
