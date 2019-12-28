// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_THEMEPART_H_
#define XFA_FWL_CORE_CFWL_THEMEPART_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

enum class CFWL_Part {
  None = 0,

  BackArrow,
  Background,
  Border,
  Caption,
  Check,
  CheckBox,
  CloseBox,
  CombTextLine,
  DateInBK,
  DateInCircle,
  DatesIn,
  DatesOut,
  DownButton,
  DropDownButton,
  Edge,
  ForeArrow,
  HSeparator,
  HeadText,
  Header,
  Icon,
  Image,
  LBtn,
  ListItem,
  LowerTrack,
  MinimizeBox,
  MaximizeBox,
  NarrowCaption,
  RBtn,
  StretchHandler,
  Thumb,
  ThumbBackArrow,
  ThumbForeArrow,
  ThumbLowerTrack,
  ThumbThumb,
  ThumbUpperTrack,
  Today,
  TodayCircle,
  UpButton,
  UpperTrack,
  VSeparator,
  Week,
  WeekNum,
  WeekNumSep
};

enum CFWL_PartState {
  CFWL_PartState_Normal = 0,

  CFWL_PartState_Checked = 1 << 1,
  CFWL_PartState_Default = 1 << 2,
  CFWL_PartState_Disabled = 1 << 3,
  CFWL_PartState_Flagged = 1 << 4,
  CFWL_PartState_Focused = 1 << 5,
  CFWL_PartState_HightLight = 1 << 6,
  CFWL_PartState_Hovered = 1 << 7,
  CFWL_PartState_Inactive = 1 << 8,
  CFWL_PartState_Neutral = 1 << 9,
  CFWL_PartState_Pressed = 1 << 10,
  CFWL_PartState_ReadOnly = 1 << 11,
  CFWL_PartState_LSelected = 1 << 12,
  CFWL_PartState_RSelected = 1 << 13,
  CFWL_PartState_Selected = 1 << 14
};

class IFWL_Widget;

class CFWL_ThemePart {
 public:
  CFWL_ThemePart();

  CFX_Matrix m_matrix;
  CFX_RectF m_rtPart;
  IFWL_Widget* m_pWidget;
  CFWL_Part m_iPart;
  uint32_t m_dwStates;
  bool m_bMaximize;
  bool m_bStaticBackground;
  void* m_pData;
};

inline CFWL_ThemePart::CFWL_ThemePart()
    : m_pWidget(nullptr),
      m_iPart(CFWL_Part::None),
      m_dwStates(CFWL_PartState_Normal),
      m_bMaximize(false),
      m_bStaticBackground(false),
      m_pData(nullptr) {
  m_rtPart.Reset();
  m_matrix.SetIdentity();
}

#endif  // XFA_FWL_CORE_CFWL_THEMEPART_H_
