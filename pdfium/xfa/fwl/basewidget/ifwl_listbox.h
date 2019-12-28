// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_LISTBOX_H_
#define XFA_FWL_BASEWIDGET_IFWL_LISTBOX_H_

#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_ListBox L"FWL_LISTBOX"
#define FWL_STYLEEXT_LTB_MultiSelection (1L << 0)
#define FWL_STYLEEXT_LTB_ShowScrollBarAlaways (1L << 2)
#define FWL_STYLEEXT_LTB_MultiColumn (1L << 3)
#define FWL_STYLEEXT_LTB_LeftAlign (0L << 4)
#define FWL_STYLEEXT_LTB_CenterAlign (1L << 4)
#define FWL_STYLEEXT_LTB_RightAlign (2L << 4)
#define FWL_STYLEEXT_LTB_MultiLine (1L << 6)
#define FWL_STYLEEXT_LTB_OwnerDraw (1L << 7)
#define FWL_STYLEEXT_LTB_Icon (1L << 8)
#define FWL_STYLEEXT_LTB_Check (1L << 9)
#define FWL_STYLEEXT_LTB_AlignMask (3L << 4)
#define FWL_STYLEEXT_LTB_ShowScrollBarFocus (1L << 10)
#define FWL_ITEMSTATE_LTB_Selected (1L << 0)
#define FWL_ITEMSTATE_LTB_Focused (1L << 1)
#define FWL_ITEMSTATE_LTB_Checked (1L << 2)

class CFX_DIBitmap;

FWL_EVENT_DEF(CFWL_EvtLtbSelChanged,
              CFWL_EventType::SelectChanged,
              CFX_Int32Array iarraySels;)

FWL_EVENT_DEF(CFWL_EvtLtbDrawItem,
              CFWL_EventType::DrawItem,
              CFX_Graphics* m_pGraphics;
              CFX_Matrix m_matrix;
              int32_t m_index;
              CFX_RectF m_rect;)

class IFWL_ListItem {};

class IFWL_ListBoxDP : public IFWL_DataProvider {
 public:
  virtual int32_t CountItems(IFWL_Widget* pWidget) = 0;
  virtual IFWL_ListItem* GetItem(IFWL_Widget* pWidget, int32_t nIndex) = 0;
  virtual int32_t GetItemIndex(IFWL_Widget* pWidget, IFWL_ListItem* pItem) = 0;
  virtual FX_BOOL SetItemIndex(IFWL_Widget* pWidget,
                               IFWL_ListItem* pItem,
                               int32_t nIndex) = 0;
  virtual uint32_t GetItemStyles(IFWL_Widget* pWidget,
                                 IFWL_ListItem* pItem) = 0;
  virtual FWL_Error GetItemText(IFWL_Widget* pWidget,
                                IFWL_ListItem* pItem,
                                CFX_WideString& wsText) = 0;
  virtual FWL_Error GetItemRect(IFWL_Widget* pWidget,
                                IFWL_ListItem* pItem,
                                CFX_RectF& rtItem) = 0;
  virtual void* GetItemData(IFWL_Widget* pWidget, IFWL_ListItem* pItem) = 0;
  virtual FWL_Error SetItemStyles(IFWL_Widget* pWidget,
                                  IFWL_ListItem* pItem,
                                  uint32_t dwStyle) = 0;
  virtual FWL_Error SetItemText(IFWL_Widget* pWidget,
                                IFWL_ListItem* pItem,
                                const FX_WCHAR* pszText) = 0;
  virtual FWL_Error SetItemRect(IFWL_Widget* pWidget,
                                IFWL_ListItem* pItem,
                                const CFX_RectF& rtItem) = 0;
  virtual FX_FLOAT GetItemHeight(IFWL_Widget* pWidget) = 0;
  virtual CFX_DIBitmap* GetItemIcon(IFWL_Widget* pWidget,
                                    IFWL_ListItem* pItem) = 0;
  virtual FWL_Error GetItemCheckRect(IFWL_Widget* pWidget,
                                     IFWL_ListItem* pItem,
                                     CFX_RectF& rtCheck) = 0;
  virtual FWL_Error SetItemCheckRect(IFWL_Widget* pWidget,
                                     IFWL_ListItem* pItem,
                                     const CFX_RectF& rtCheck) = 0;
  virtual uint32_t GetItemCheckState(IFWL_Widget* pWidget,
                                     IFWL_ListItem* pItem) = 0;
  virtual FWL_Error SetItemCheckState(IFWL_Widget* pWidget,
                                      IFWL_ListItem* pItem,
                                      uint32_t dwCheckState) = 0;
};

class IFWL_ListBoxCompare {
 public:
  virtual ~IFWL_ListBoxCompare() {}
  virtual int32_t Compare(IFWL_ListItem* hLeft, IFWL_ListItem* hRight) = 0;
};

class IFWL_ListBox : public IFWL_Widget {
 public:
  static IFWL_ListBox* Create(const CFWL_WidgetImpProperties& properties,
                              IFWL_Widget* pOuter);
  static IFWL_ListBox* CreateComboList(
      const CFWL_WidgetImpProperties& properties,
      IFWL_Widget* pOuter);

  int32_t CountSelItems();
  IFWL_ListItem* GetSelItem(int32_t nIndexSel);
  int32_t GetSelIndex(int32_t nIndex);
  FWL_Error SetSelItem(IFWL_ListItem* pItem, FX_BOOL bSelect = TRUE);
  FWL_Error GetItemText(IFWL_ListItem* pItem, CFX_WideString& wsText);
  FWL_Error GetScrollPos(FX_FLOAT& fPos, FX_BOOL bVert = TRUE);

 protected:
  IFWL_ListBox();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_LISTBOX_H_
