// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_SCROLLBARTP_H_
#define XFA_FWL_THEME_CFWL_SCROLLBARTP_H_

#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_ScrollBarTP : public CFWL_WidgetTP {
 public:
  CFWL_ScrollBarTP();
  ~CFWL_ScrollBarTP() override;

  // CFWL_WidgetTP
  bool IsValidWidget(IFWL_Widget* pWidget) override;
  uint32_t SetThemeID(IFWL_Widget* pWidget,
                      uint32_t dwThemeID,
                      FX_BOOL bChildren = TRUE) override;
  FX_BOOL DrawBackground(CFWL_ThemeBackground* pParams) override;
  void* GetCapacity(CFWL_ThemePart* pThemePart,
                    CFWL_WidgetCapacity dwCapacity) override;

 protected:
  struct SBThemeData {
    FX_ARGB clrPawColorLight[4];
    FX_ARGB clrPawColorDark[4];
    FX_ARGB clrBtnBK[4][2];
    FX_ARGB clrBtnBorder[4];
    FX_ARGB clrTrackBKStart;
    FX_ARGB clrTrackBKEnd;
  };

  void DrawThumbBtn(CFX_Graphics* pGraphics,
                    const CFX_RectF* pRect,
                    FX_BOOL bVert,
                    FWLTHEME_STATE eState,
                    FX_BOOL bPawButton = TRUE,
                    CFX_Matrix* pMatrix = nullptr);
  void DrawTrack(CFX_Graphics* pGraphics,
                 const CFX_RectF* pRect,
                 FX_BOOL bVert,
                 FWLTHEME_STATE eState,
                 FX_BOOL bLowerTrack,
                 CFX_Matrix* pMatrix = nullptr);
  void DrawMaxMinBtn(CFX_Graphics* pGraphics,
                     const CFX_RectF* pRect,
                     FWLTHEME_DIRECTION eDict,
                     FWLTHEME_STATE eState,
                     CFX_Matrix* pMatrix = nullptr);
  void DrawPaw(CFX_Graphics* pGraphics,
               const CFX_RectF* pRect,
               FX_BOOL bVert,
               FWLTHEME_STATE eState,
               CFX_Matrix* pMatrix = nullptr);
  void SetThemeData(uint32_t dwID);

  struct SBThemeData* m_pThemeData;
};

#endif  // XFA_FWL_THEME_CFWL_SCROLLBARTP_H_
