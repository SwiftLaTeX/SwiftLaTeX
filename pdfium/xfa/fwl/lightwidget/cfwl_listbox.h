// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_LISTBOX_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_LISTBOX_H_

#include <memory>
#include <vector>

#include "xfa/fwl/basewidget/ifwl_listbox.h"
#include "xfa/fwl/core/fwl_error.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/lightwidget/cfwl_widget.h"

class CFWL_ListItem;

class CFWL_ListBox : public CFWL_Widget {
 public:
  CFWL_ListBox();
  ~CFWL_ListBox() override;

  static CFWL_ListBox* Create();
  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
  FWL_Error AddDIBitmap(CFX_DIBitmap* pDIB, IFWL_ListItem* pItem);
  IFWL_ListItem* AddString(const CFX_WideStringC& wsAdd,
                           FX_BOOL bSelect = FALSE);
  FX_BOOL DeleteString(IFWL_ListItem* pItem);
  void DeleteAll();
  int32_t CountSelItems();
  IFWL_ListItem* GetSelItem(int32_t nIndexSel);
  int32_t GetSelIndex(int32_t nIndex);
  FWL_Error SetSelItem(IFWL_ListItem* pItem, FX_BOOL bSelect = TRUE);
  FWL_Error GetItemText(IFWL_ListItem* pItem, CFX_WideString& wsText);
  FWL_Error GetScrollPos(FX_FLOAT& fPos, FX_BOOL bVert = TRUE);
  FWL_Error SetItemHeight(FX_FLOAT fItemHeight);
  IFWL_ListItem* GetFocusItem();
  FWL_Error SetFocusItem(IFWL_ListItem* pItem);
  int32_t CountItems();
  IFWL_ListItem* GetItem(int32_t nIndex);
  FWL_Error SetItemString(IFWL_ListItem* pItem, const CFX_WideStringC& wsText);
  FWL_Error GetItemString(IFWL_ListItem* pItem, CFX_WideString& wsText);
  FWL_Error SetItemData(IFWL_ListItem* pItem, void* pData);
  void* GetItemData(IFWL_ListItem* pItem);
  IFWL_ListItem* GetItemAtPoint(FX_FLOAT fx, FX_FLOAT fy);
  uint32_t GetItemStates(IFWL_ListItem* pItem);

 protected:
  class CFWL_ListBoxDP : public IFWL_ListBoxDP {
   public:
    CFWL_ListBoxDP();
    ~CFWL_ListBoxDP() override;

    // IFWL_DataProvider:
    FWL_Error GetCaption(IFWL_Widget* pWidget,
                         CFX_WideString& wsCaption) override;

    // IFWL_ListBoxDP:
    int32_t CountItems(IFWL_Widget* pWidget) override;
    IFWL_ListItem* GetItem(IFWL_Widget* pWidget, int32_t nIndex) override;
    int32_t GetItemIndex(IFWL_Widget* pWidget, IFWL_ListItem* pItem) override;
    FX_BOOL SetItemIndex(IFWL_Widget* pWidget,
                         IFWL_ListItem* pItem,
                         int32_t nIndex) override;
    uint32_t GetItemStyles(IFWL_Widget* pWidget, IFWL_ListItem* pItem) override;
    FWL_Error GetItemText(IFWL_Widget* pWidget,
                          IFWL_ListItem* pItem,
                          CFX_WideString& wsText) override;
    FWL_Error GetItemRect(IFWL_Widget* pWidget,
                          IFWL_ListItem* pItem,
                          CFX_RectF& rtItem) override;
    void* GetItemData(IFWL_Widget* pWidget, IFWL_ListItem* pItem) override;
    FWL_Error SetItemStyles(IFWL_Widget* pWidget,
                            IFWL_ListItem* pItem,
                            uint32_t dwStyle) override;
    FWL_Error SetItemText(IFWL_Widget* pWidget,
                          IFWL_ListItem* pItem,
                          const FX_WCHAR* pszText) override;
    FWL_Error SetItemRect(IFWL_Widget* pWidget,
                          IFWL_ListItem* pItem,
                          const CFX_RectF& rtItem) override;
    FX_FLOAT GetItemHeight(IFWL_Widget* pWidget) override;
    CFX_DIBitmap* GetItemIcon(IFWL_Widget* pWidget,
                              IFWL_ListItem* pItem) override;
    FWL_Error GetItemCheckRect(IFWL_Widget* pWidget,
                               IFWL_ListItem* pItem,
                               CFX_RectF& rtCheck) override;
    FWL_Error SetItemCheckRect(IFWL_Widget* pWidget,
                               IFWL_ListItem* pItem,
                               const CFX_RectF& rtCheck) override;
    uint32_t GetItemCheckState(IFWL_Widget* pWidget,
                               IFWL_ListItem* pItem) override;
    FWL_Error SetItemCheckState(IFWL_Widget* pWidget,
                                IFWL_ListItem* pItem,
                                uint32_t dwCheckState) override;

    std::vector<std::unique_ptr<CFWL_ListItem>> m_ItemArray;
    CFX_WideString m_wsData;
    FX_FLOAT m_fItemHeight;
  };

  CFWL_ListBoxDP m_ListBoxDP;
};

class CFWL_ListItem : public IFWL_ListItem {
 public:
  CFWL_ListItem();
  ~CFWL_ListItem();

  CFX_RectF m_rtItem;
  uint32_t m_dwStates;
  CFX_WideString m_wsText;
  CFX_DIBitmap* m_pDIB;
  void* m_pData;
  uint32_t m_dwCheckState;
  CFX_RectF m_rtCheckBox;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_LISTBOX_H_
