// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_COMBOBOX_H_
#define XFA_FWL_BASEWIDGET_IFWL_COMBOBOX_H_

#include "xfa/fwl/basewidget/ifwl_listbox.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

class CFWL_WidgetImpProperties;
class IFDE_TxtEdtDoRecord;
class IFWL_ComboBox;

#define FWL_CLASS_ComboBox L"FWL_COMBOBOX"
#define FWL_STYLEEXT_CMB_DropList (0L << 0)
#define FWL_STYLEEXT_CMB_DropDown (1L << 0)
#define FWL_STYLEEXT_CMB_Sort (1L << 1)
#define FWL_STYLEEXT_CMB_ListDrag (1L << 2)
#define FWL_STYLEEXT_CMB_OwnerDraw (1L << 3)
#define FWL_STYLEEXT_CMB_EditHNear (0L << 4)
#define FWL_STYLEEXT_CMB_EditHCenter (1L << 4)
#define FWL_STYLEEXT_CMB_EditHFar (2L << 4)
#define FWL_STYLEEXT_CMB_EditVNear (0L << 6)
#define FWL_STYLEEXT_CMB_EditVCenter (1L << 6)
#define FWL_STYLEEXT_CMB_EditVFar (2L << 6)
#define FWL_STYLEEXT_CMB_EditJustified (1L << 8)
#define FWL_STYLEEXT_CMB_EditDistributed (2L << 8)
#define FWL_STYLEEXT_CMB_EditHAlignMask (3L << 4)
#define FWL_STYLEEXT_CMB_EditVAlignMask (3L << 6)
#define FWL_STYLEEXT_CMB_EditHAlignModeMask (3L << 8)
#define FWL_STYLEEXT_CMB_ListItemLeftAlign (0L << 10)
#define FWL_STYLEEXT_CMB_ListItemCenterAlign (1L << 10)
#define FWL_STYLEEXT_CMB_ListItemRightAlign (2L << 10)
#define FWL_STYLEEXT_CMB_ListItemAlignMask (3L << 10)
#define FWL_STYLEEXT_CMB_ListItemText (0L << 12)
#define FWL_STYLEEXT_CMB_ListItemIconText (1L << 12)
#define FWL_STYLEEXT_CMB_ReadOnly (1L << 13)

FWL_EVENT_DEF(CFWL_EvtCmbPreDropDown, CFWL_EventType::PreDropDown)

FWL_EVENT_DEF(CFWL_EvtCmbPostDropDown, CFWL_EventType::PostDropDown)

FWL_EVENT_DEF(CFWL_EvtCmbCloseUp, CFWL_EventType::CloseUp)

FWL_EVENT_DEF(CFWL_EvtCmbEditChanged,
              CFWL_EventType::EditChanged,
              int32_t nChangeType;
              CFX_WideString wsInsert;
              CFX_WideString wsDelete;)

FWL_EVENT_DEF(CFWL_EvtCmbSelChanged,
              CFWL_EventType::SelectChanged,
              CFX_Int32Array iArraySels;
              FX_BOOL bLButtonUp;)

FWL_EVENT_DEF(CFWL_EvtCmbHoverChanged,
              CFWL_EventType::HoverChanged,
              int32_t m_iCurHover;)

FWL_EVENT_DEF(CFWL_EvtCmbDrawItem,
              CFWL_EventType::DrawItem,
              CFX_Graphics* m_pGraphics;
              CFX_Matrix m_matrix;
              int32_t m_index;
              CFX_RectF m_rtItem;)

class IFWL_ComboBoxDP : public IFWL_ListBoxDP {
 public:
  virtual FX_FLOAT GetListHeight(IFWL_Widget* pWidget) = 0;
};

class IFWL_ComboBox : public IFWL_Widget {
 public:
  static IFWL_ComboBox* Create(const CFWL_WidgetImpProperties& properties);

  int32_t GetCurSel();
  FWL_Error SetCurSel(int32_t iSel);
  FWL_Error SetEditText(const CFX_WideString& wsText);
  int32_t GetEditTextLength() const;
  FWL_Error GetEditText(CFX_WideString& wsText,
                        int32_t nStart = 0,
                        int32_t nCount = -1) const;
  FWL_Error SetEditSelRange(int32_t nStart, int32_t nCount = -1);
  int32_t GetEditSelRange(int32_t nIndex, int32_t& nStart);
  int32_t GetEditLimit();
  FWL_Error SetEditLimit(int32_t nLimit);
  FWL_Error EditDoClipboard(int32_t iCmd);
  FX_BOOL EditRedo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL EditUndo(const IFDE_TxtEdtDoRecord* pRecord);
  IFWL_ListBox* GetListBoxt();
  FX_BOOL AfterFocusShowDropList();
  FWL_Error OpenDropDownList(FX_BOOL bActivate);
  FX_BOOL EditCanUndo();
  FX_BOOL EditCanRedo();
  FX_BOOL EditUndo();
  FX_BOOL EditRedo();
  FX_BOOL EditCanCopy();
  FX_BOOL EditCanCut();
  FX_BOOL EditCanSelectAll();
  FX_BOOL EditCopy(CFX_WideString& wsCopy);
  FX_BOOL EditCut(CFX_WideString& wsCut);
  FX_BOOL EditPaste(const CFX_WideString& wsPaste);
  FX_BOOL EditSelectAll();
  FX_BOOL EditDelete();
  FX_BOOL EditDeSelect();
  FWL_Error GetBBox(CFX_RectF& rect);
  FWL_Error EditModifyStylesEx(uint32_t dwStylesExAdded,
                               uint32_t dwStylesExRemoved);

 protected:
  IFWL_ComboBox();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_COMBOBOX_H_
