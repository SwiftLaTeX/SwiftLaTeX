// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_MONTHCALENDARTP_H_
#define XFA_FWL_THEME_CFWL_MONTHCALENDARTP_H_

#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_MonthCalendarTP : public CFWL_WidgetTP {
 public:
  CFWL_MonthCalendarTP();
  ~CFWL_MonthCalendarTP() override;

  // CFWL_WidgetTP
  bool IsValidWidget(IFWL_Widget* pWidget) override;
  uint32_t SetThemeID(IFWL_Widget* pWidget,
                      uint32_t dwThemeID,
                      FX_BOOL bChildren = TRUE) override;
  FX_BOOL DrawBackground(CFWL_ThemeBackground* pParams) override;
  FX_BOOL DrawText(CFWL_ThemeText* pParams) override;
  void* GetCapacity(CFWL_ThemePart* pThemePart,
                    CFWL_WidgetCapacity dwCapacity) override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;

 protected:
  FX_BOOL DrawTotalBK(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawHeadBk(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawLButton(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawRButton(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawDatesInBK(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawDatesInCircle(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawTodayCircle(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawHSeperator(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FX_BOOL DrawWeekNumSep(CFWL_ThemeBackground* pParams, CFX_Matrix* pMatrix);
  FWLTHEME_STATE GetState(uint32_t dwFWLStates);
  void SetThemeData(uint32_t dwThemeID);

  class MCThemeData {
   public:
    FX_ARGB clrCaption;
    FX_ARGB clrSeperator;
    FX_ARGB clrDatesHoverBK;
    FX_ARGB clrDatesSelectedBK;
    FX_ARGB clrDatesCircle;
    FX_ARGB clrToday;
    FX_ARGB clrBK;
  } * m_pThemeData;
  CFX_WideString wsResource;
};

#endif  // XFA_FWL_THEME_CFWL_MONTHCALENDARTP_H_
