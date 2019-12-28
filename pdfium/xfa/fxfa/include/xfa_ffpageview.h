// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_FFPAGEVIEW_H_
#define XFA_FXFA_INCLUDE_XFA_FFPAGEVIEW_H_

#include "xfa/fxfa/parser/xfa_doclayout.h"

class CXFA_FFWidget;
class CXFA_FFDocView;

class CXFA_FFPageView : public CXFA_ContainerLayoutItem {
 public:
  CXFA_FFPageView(CXFA_FFDocView* pDocView, CXFA_Node* pPageArea);
  ~CXFA_FFPageView() override;

  CXFA_FFDocView* GetDocView() const;
  void GetPageViewRect(CFX_RectF& rtPage) const;
  void GetDisplayMatrix(CFX_Matrix& mt,
                        const CFX_Rect& rtDisp,
                        int32_t iRotate) const;
  IXFA_WidgetIterator* CreateWidgetIterator(
      uint32_t dwTraverseWay = XFA_TRAVERSEWAY_Form,
      uint32_t dwWidgetFilter = XFA_WidgetStatus_Visible |
                                XFA_WidgetStatus_Viewable);

 protected:
  CXFA_FFDocView* const m_pDocView;
};

typedef CXFA_NodeIteratorTemplate<CXFA_LayoutItem,
                                  CXFA_TraverseStrategy_LayoutItem>
    CXFA_LayoutItemIterator;

class CXFA_FFPageWidgetIterator : public IXFA_WidgetIterator {
 public:
  CXFA_FFPageWidgetIterator(CXFA_FFPageView* pPageView, uint32_t dwFilter);
  ~CXFA_FFPageWidgetIterator() override;

  void Reset() override;
  CXFA_FFWidget* MoveToFirst() override;
  CXFA_FFWidget* MoveToLast() override;
  CXFA_FFWidget* MoveToNext() override;
  CXFA_FFWidget* MoveToPrevious() override;
  CXFA_FFWidget* GetCurrentWidget() override;
  FX_BOOL SetCurrentWidget(CXFA_FFWidget* hWidget) override;

 protected:
  CXFA_FFWidget* GetWidget(CXFA_LayoutItem* pLayoutItem);

  CXFA_FFPageView* m_pPageView;
  CXFA_FFWidget* m_hCurWidget;
  uint32_t m_dwFilter;
  FX_BOOL m_bIgnorerelevant;
  CXFA_LayoutItemIterator m_sIterator;
};
typedef CFX_ArrayTemplate<CXFA_FFWidget*> CXFA_WidgetArray;

class CXFA_TabParam {
 public:
  CXFA_TabParam();
  ~CXFA_TabParam();

  CXFA_FFWidget* m_pWidget;
  CXFA_WidgetArray m_Children;
};

class CXFA_FFTabOrderPageWidgetIterator : public IXFA_WidgetIterator {
 public:
  CXFA_FFTabOrderPageWidgetIterator(CXFA_FFPageView* pPageView,
                                    uint32_t dwFilter);
  ~CXFA_FFTabOrderPageWidgetIterator() override;

  void Reset() override;
  CXFA_FFWidget* MoveToFirst() override;
  CXFA_FFWidget* MoveToLast() override;
  CXFA_FFWidget* MoveToNext() override;
  CXFA_FFWidget* MoveToPrevious() override;
  CXFA_FFWidget* GetCurrentWidget() override;
  FX_BOOL SetCurrentWidget(CXFA_FFWidget* hWidget) override;

 protected:
  CXFA_FFWidget* GetTraverseWidget(CXFA_FFWidget* pWidget);
  CXFA_FFWidget* FindWidgetByName(const CFX_WideString& wsWidgetName,
                                  CXFA_FFWidget* pRefWidget);
  void CreateTabOrderWidgetArray();
  void CreateSpaceOrderWidgetArray(CXFA_WidgetArray& WidgetArray);
  CXFA_FFWidget* GetWidget(CXFA_LayoutItem* pLayoutItem);
  void OrderContainer(CXFA_LayoutItemIterator* sIterator,
                      CXFA_LayoutItem* pContainerItem,
                      CXFA_TabParam* pContainer,
                      FX_BOOL& bCurrentItem,
                      FX_BOOL& bContentArea,
                      FX_BOOL bMarsterPage = FALSE);

  CXFA_WidgetArray m_TabOrderWidgetArray;
  CXFA_FFPageView* m_pPageView;
  uint32_t m_dwFilter;
  int32_t m_iCurWidget;
  FX_BOOL m_bIgnorerelevant;
};

#endif  // XFA_FXFA_INCLUDE_XFA_FFPAGEVIEW_H_
