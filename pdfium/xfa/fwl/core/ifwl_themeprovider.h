// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_THEMEPROVIDER_H_
#define XFA_FWL_CORE_IFWL_THEMEPROVIDER_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_ThemeBackground;
class CFWL_ThemePart;
class CFWL_ThemeText;
class IFWL_Widget;

class IFWL_ThemeProvider {
 public:
  virtual ~IFWL_ThemeProvider() {}
  virtual bool IsValidWidget(IFWL_Widget* pWidget) = 0;
  virtual uint32_t GetThemeID(IFWL_Widget* pWidget) = 0;
  virtual uint32_t SetThemeID(IFWL_Widget* pWidget,
                              uint32_t dwThemeID,
                              FX_BOOL bChildren = TRUE) = 0;
  virtual FWL_Error GetThemeMatrix(IFWL_Widget* pWidget,
                                   CFX_Matrix& matrix) = 0;
  virtual FWL_Error SetThemeMatrix(IFWL_Widget* pWidget,
                                   const CFX_Matrix& matrix) = 0;
  virtual FX_BOOL DrawBackground(CFWL_ThemeBackground* pParams) = 0;
  virtual FX_BOOL DrawText(CFWL_ThemeText* pParams) = 0;
  virtual void* GetCapacity(CFWL_ThemePart* pThemePart,
                            CFWL_WidgetCapacity dwCapacity) = 0;
  virtual FX_BOOL IsCustomizedLayout(IFWL_Widget* pWidget) = 0;
  virtual FWL_Error GetPartRect(CFWL_ThemePart* pThemePart,
                                CFX_RectF& rtPart) = 0;
  virtual FX_BOOL IsInPart(CFWL_ThemePart* pThemePart,
                           FX_FLOAT fx,
                           FX_FLOAT fy) = 0;
  virtual FX_BOOL CalcTextRect(CFWL_ThemeText* pParams, CFX_RectF& rect) = 0;
};

#endif  // XFA_FWL_CORE_IFWL_THEMEPROVIDER_H_
