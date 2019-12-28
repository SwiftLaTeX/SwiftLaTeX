// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/xfa_ffpageview.h"

#include "xfa/fde/fde_render.h"
#include "xfa/fxfa/app/xfa_ffcheckbutton.h"
#include "xfa/fxfa/app/xfa_ffchoicelist.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/app/xfa_ffimageedit.h"
#include "xfa/fxfa/app/xfa_ffpushbutton.h"
#include "xfa/fxfa/app/xfa_fftextedit.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

namespace {

void GetPageMatrix(CFX_Matrix& pageMatrix,
                   const CFX_RectF& docPageRect,
                   const CFX_Rect& devicePageRect,
                   int32_t iRotate,
                   uint32_t dwCoordinatesType) {
  ASSERT(iRotate >= 0 && iRotate <= 3);
  FX_BOOL bFlipX = (dwCoordinatesType & 0x01) != 0;
  FX_BOOL bFlipY = (dwCoordinatesType & 0x02) != 0;
  CFX_Matrix m;
  m.Set((bFlipX ? -1.0f : 1.0f), 0, 0, (bFlipY ? -1.0f : 1.0f), 0, 0);
  if (iRotate == 0 || iRotate == 2) {
    m.a *= (FX_FLOAT)devicePageRect.width / docPageRect.width;
    m.d *= (FX_FLOAT)devicePageRect.height / docPageRect.height;
  } else {
    m.a *= (FX_FLOAT)devicePageRect.height / docPageRect.width;
    m.d *= (FX_FLOAT)devicePageRect.width / docPageRect.height;
  }
  m.Rotate(iRotate * 1.57079632675f);
  switch (iRotate) {
    case 0:
      m.e = bFlipX ? (FX_FLOAT)devicePageRect.right()
                   : (FX_FLOAT)devicePageRect.left;
      m.f = bFlipY ? (FX_FLOAT)devicePageRect.bottom()
                   : (FX_FLOAT)devicePageRect.top;
      break;
    case 1:
      m.e = bFlipY ? (FX_FLOAT)devicePageRect.left
                   : (FX_FLOAT)devicePageRect.right();
      m.f = bFlipX ? (FX_FLOAT)devicePageRect.bottom()
                   : (FX_FLOAT)devicePageRect.top;
      break;
    case 2:
      m.e = bFlipX ? (FX_FLOAT)devicePageRect.left
                   : (FX_FLOAT)devicePageRect.right();
      m.f = bFlipY ? (FX_FLOAT)devicePageRect.top
                   : (FX_FLOAT)devicePageRect.bottom();
      break;
    case 3:
      m.e = bFlipY ? (FX_FLOAT)devicePageRect.right()
                   : (FX_FLOAT)devicePageRect.left;
      m.f = bFlipX ? (FX_FLOAT)devicePageRect.top
                   : (FX_FLOAT)devicePageRect.bottom();
      break;
    default:
      break;
  }
  pageMatrix = m;
}

bool PageWidgetFilter(CXFA_FFWidget* pWidget,
                      uint32_t dwFilter,
                      FX_BOOL bTraversal,
                      FX_BOOL bIgnorerelevant) {
  CXFA_WidgetAcc* pWidgetAcc = pWidget->GetDataAcc();

  if (!!(dwFilter & XFA_WidgetStatus_Focused) &&
      pWidgetAcc->GetElementType() != XFA_Element::Field) {
    return false;
  }

  uint32_t dwStatus = pWidget->GetStatus();
  if (bTraversal && (dwStatus & XFA_WidgetStatus_Disabled))
    return false;
  if (bIgnorerelevant)
    return !!(dwStatus & XFA_WidgetStatus_Visible);

  dwFilter &= (XFA_WidgetStatus_Visible | XFA_WidgetStatus_Viewable |
               XFA_WidgetStatus_Printable);
  return (dwFilter & dwStatus) == dwFilter;
}

bool IsLayoutElement(XFA_Element eElement, bool bLayoutContainer) {
  switch (eElement) {
    case XFA_Element::Draw:
    case XFA_Element::Field:
    case XFA_Element::InstanceManager:
      return !bLayoutContainer;
    case XFA_Element::Area:
    case XFA_Element::Subform:
    case XFA_Element::ExclGroup:
    case XFA_Element::SubformSet:
    case XFA_Element::PageArea:
    case XFA_Element::Form:
      return true;
    default:
      return false;
  }
}

}  // namespace

CXFA_FFPageView::CXFA_FFPageView(CXFA_FFDocView* pDocView, CXFA_Node* pPageArea)
    : CXFA_ContainerLayoutItem(pPageArea), m_pDocView(pDocView) {}

CXFA_FFPageView::~CXFA_FFPageView() {}

CXFA_FFDocView* CXFA_FFPageView::GetDocView() const {
  return m_pDocView;
}

void CXFA_FFPageView::GetPageViewRect(CFX_RectF& rtPage) const {
  CFX_SizeF sz;
  GetPageSize(sz);
  rtPage.Set(0, 0, sz);
}
void CXFA_FFPageView::GetDisplayMatrix(CFX_Matrix& mt,
                                       const CFX_Rect& rtDisp,
                                       int32_t iRotate) const {
  CFX_SizeF sz;
  GetPageSize(sz);
  CFX_RectF fdePage;
  fdePage.Set(0, 0, sz.x, sz.y);
  GetPageMatrix(mt, fdePage, rtDisp, iRotate, 0);
}

IXFA_WidgetIterator* CXFA_FFPageView::CreateWidgetIterator(
    uint32_t dwTraverseWay,
    uint32_t dwWidgetFilter) {
  switch (dwTraverseWay) {
    case XFA_TRAVERSEWAY_Tranvalse:
      return new CXFA_FFTabOrderPageWidgetIterator(this, dwWidgetFilter);
    case XFA_TRAVERSEWAY_Form:
      return new CXFA_FFPageWidgetIterator(this, dwWidgetFilter);
  }
  return nullptr;
}

CXFA_FFPageWidgetIterator::CXFA_FFPageWidgetIterator(CXFA_FFPageView* pPageView,
                                                     uint32_t dwFilter) {
  m_pPageView = pPageView;
  m_dwFilter = dwFilter;
  m_sIterator.Init(pPageView);
  m_bIgnorerelevant =
      m_pPageView->GetDocView()->GetDoc()->GetXFADoc()->GetCurVersionMode() <
      XFA_VERSION_205;
}
CXFA_FFPageWidgetIterator::~CXFA_FFPageWidgetIterator() {}
void CXFA_FFPageWidgetIterator::Reset() {
  m_sIterator.Reset();
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::MoveToFirst() {
  m_sIterator.Reset();
  for (CXFA_LayoutItem* pLayoutItem = m_sIterator.GetCurrent(); pLayoutItem;
       pLayoutItem = m_sIterator.MoveToNext()) {
    if (CXFA_FFWidget* hWidget = GetWidget(pLayoutItem)) {
      return hWidget;
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::MoveToLast() {
  m_sIterator.SetCurrent(nullptr);
  return MoveToPrevious();
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::MoveToNext() {
  for (CXFA_LayoutItem* pLayoutItem = m_sIterator.MoveToNext(); pLayoutItem;
       pLayoutItem = m_sIterator.MoveToNext()) {
    if (CXFA_FFWidget* hWidget = GetWidget(pLayoutItem)) {
      return hWidget;
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::MoveToPrevious() {
  for (CXFA_LayoutItem* pLayoutItem = m_sIterator.MoveToPrev(); pLayoutItem;
       pLayoutItem = m_sIterator.MoveToPrev()) {
    if (CXFA_FFWidget* hWidget = GetWidget(pLayoutItem)) {
      return hWidget;
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::GetCurrentWidget() {
  CXFA_LayoutItem* pLayoutItem = m_sIterator.GetCurrent();
  return pLayoutItem ? XFA_GetWidgetFromLayoutItem(pLayoutItem) : nullptr;
}
FX_BOOL CXFA_FFPageWidgetIterator::SetCurrentWidget(CXFA_FFWidget* hWidget) {
  return hWidget && m_sIterator.SetCurrent(hWidget);
}
CXFA_FFWidget* CXFA_FFPageWidgetIterator::GetWidget(
    CXFA_LayoutItem* pLayoutItem) {
  if (CXFA_FFWidget* pWidget = XFA_GetWidgetFromLayoutItem(pLayoutItem)) {
    if (!PageWidgetFilter(pWidget, m_dwFilter, FALSE, m_bIgnorerelevant)) {
      return nullptr;
    }
    if (!pWidget->IsLoaded() &&
        (pWidget->GetStatus() & XFA_WidgetStatus_Visible) != 0) {
      pWidget->LoadWidget();
    }
    return pWidget;
  }
  return nullptr;
}

CXFA_FFTabOrderPageWidgetIterator::CXFA_FFTabOrderPageWidgetIterator(
    CXFA_FFPageView* pPageView,
    uint32_t dwFilter)
    : m_pPageView(pPageView), m_dwFilter(dwFilter), m_iCurWidget(-1) {
  m_bIgnorerelevant =
      m_pPageView->GetDocView()->GetDoc()->GetXFADoc()->GetCurVersionMode() <
      XFA_VERSION_205;
  Reset();
}

CXFA_FFTabOrderPageWidgetIterator::~CXFA_FFTabOrderPageWidgetIterator() {}

void CXFA_FFTabOrderPageWidgetIterator::Reset() {
  CreateTabOrderWidgetArray();
  m_iCurWidget = -1;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::MoveToFirst() {
  if (m_TabOrderWidgetArray.GetSize() > 0) {
    for (int32_t i = 0; i < m_TabOrderWidgetArray.GetSize(); i++) {
      if (PageWidgetFilter(m_TabOrderWidgetArray[i], m_dwFilter, TRUE,
                           m_bIgnorerelevant)) {
        m_iCurWidget = i;
        return m_TabOrderWidgetArray[m_iCurWidget];
      }
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::MoveToLast() {
  if (m_TabOrderWidgetArray.GetSize() > 0) {
    for (int32_t i = m_TabOrderWidgetArray.GetSize() - 1; i >= 0; i--) {
      if (PageWidgetFilter(m_TabOrderWidgetArray[i], m_dwFilter, TRUE,
                           m_bIgnorerelevant)) {
        m_iCurWidget = i;
        return m_TabOrderWidgetArray[m_iCurWidget];
      }
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::MoveToNext() {
  for (int32_t i = m_iCurWidget + 1; i < m_TabOrderWidgetArray.GetSize(); i++) {
    if (PageWidgetFilter(m_TabOrderWidgetArray[i], m_dwFilter, TRUE,
                         m_bIgnorerelevant)) {
      m_iCurWidget = i;
      return m_TabOrderWidgetArray[m_iCurWidget];
    }
  }
  m_iCurWidget = -1;
  return nullptr;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::MoveToPrevious() {
  for (int32_t i = m_iCurWidget - 1; i >= 0; i--) {
    if (PageWidgetFilter(m_TabOrderWidgetArray[i], m_dwFilter, TRUE,
                         m_bIgnorerelevant)) {
      m_iCurWidget = i;
      return m_TabOrderWidgetArray[m_iCurWidget];
    }
  }
  m_iCurWidget = -1;
  return nullptr;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::GetCurrentWidget() {
  if (m_iCurWidget >= 0) {
    return m_TabOrderWidgetArray[m_iCurWidget];
  }
  return nullptr;
}
FX_BOOL CXFA_FFTabOrderPageWidgetIterator::SetCurrentWidget(
    CXFA_FFWidget* hWidget) {
  int32_t iWidgetIndex = m_TabOrderWidgetArray.Find(hWidget);
  if (iWidgetIndex >= 0) {
    m_iCurWidget = iWidgetIndex;
    return TRUE;
  }
  return FALSE;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::GetTraverseWidget(
    CXFA_FFWidget* pWidget) {
  CXFA_WidgetAcc* pAcc = pWidget->GetDataAcc();
  CXFA_Node* pTraversal = pAcc->GetNode()->GetChild(0, XFA_Element::Traversal);
  if (pTraversal) {
    CXFA_Node* pTraverse = pTraversal->GetChild(0, XFA_Element::Traverse);
    if (pTraverse) {
      CFX_WideString wsTraverseWidgetName;
      if (pTraverse->GetAttribute(XFA_ATTRIBUTE_Ref, wsTraverseWidgetName)) {
        return FindWidgetByName(wsTraverseWidgetName, pWidget);
      }
    }
  }
  return nullptr;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::FindWidgetByName(
    const CFX_WideString& wsWidgetName,
    CXFA_FFWidget* pRefWidget) {
  return pRefWidget->GetDocView()->GetWidgetByName(wsWidgetName, pRefWidget);
}
void CXFA_FFTabOrderPageWidgetIterator::CreateTabOrderWidgetArray() {
  m_TabOrderWidgetArray.RemoveAll();
  CXFA_WidgetArray SpaceOrderWidgetArray;
  CreateSpaceOrderWidgetArray(SpaceOrderWidgetArray);
  int32_t nWidgetCount = SpaceOrderWidgetArray.GetSize();
  if (nWidgetCount < 1) {
    return;
  }
  CXFA_FFWidget* hWidget = SpaceOrderWidgetArray[0];
  for (; m_TabOrderWidgetArray.GetSize() < nWidgetCount;) {
    if (m_TabOrderWidgetArray.Find(hWidget) < 0) {
      m_TabOrderWidgetArray.Add(hWidget);
      CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc();
      if (pWidgetAcc->GetUIType() == XFA_Element::ExclGroup) {
        int32_t iWidgetIndex = SpaceOrderWidgetArray.Find(hWidget) + 1;
        while (TRUE) {
          CXFA_FFWidget* pRadio =
              SpaceOrderWidgetArray[(iWidgetIndex) % nWidgetCount];
          if (pRadio->GetDataAcc()->GetExclGroup() != pWidgetAcc) {
            break;
          }
          if (m_TabOrderWidgetArray.Find(hWidget) < 0) {
            m_TabOrderWidgetArray.Add(pRadio);
          }
          iWidgetIndex++;
        }
      }
      if (CXFA_FFWidget* hTraverseWidget = GetTraverseWidget(hWidget)) {
        hWidget = hTraverseWidget;
        continue;
      }
    }
    int32_t iWidgetIndex = SpaceOrderWidgetArray.Find(hWidget);
    hWidget = SpaceOrderWidgetArray[(iWidgetIndex + 1) % nWidgetCount];
  }
}
static int32_t XFA_TabOrderWidgetComparator(const void* phWidget1,
                                            const void* phWidget2) {
  CXFA_FFWidget* pWidget1 = (*(CXFA_TabParam**)phWidget1)->m_pWidget;
  CXFA_FFWidget* pWidget2 = (*(CXFA_TabParam**)phWidget2)->m_pWidget;
  CFX_RectF rt1, rt2;
  pWidget1->GetWidgetRect(rt1);
  pWidget2->GetWidgetRect(rt2);
  FX_FLOAT x1 = rt1.left, y1 = rt1.top, x2 = rt2.left, y2 = rt2.top;
  if (y1 < y2 || (y1 - y2 < XFA_FLOAT_PERCISION && x1 < x2)) {
    return -1;
  }
  return 1;
}
void CXFA_FFTabOrderPageWidgetIterator::OrderContainer(
    CXFA_LayoutItemIterator* sIterator,
    CXFA_LayoutItem* pContainerItem,
    CXFA_TabParam* pContainer,
    FX_BOOL& bCurrentItem,
    FX_BOOL& bContentArea,
    FX_BOOL bMarsterPage) {
  CFX_ArrayTemplate<CXFA_TabParam*> tabParams;
  CXFA_LayoutItem* pSearchItem = sIterator->MoveToNext();
  while (pSearchItem) {
    if (!pSearchItem->IsContentLayoutItem()) {
      bContentArea = TRUE;
      pSearchItem = sIterator->MoveToNext();
      continue;
    }
    if (bMarsterPage && bContentArea) {
      break;
    }
    if (bMarsterPage || bContentArea) {
      CXFA_FFWidget* hWidget = GetWidget(pSearchItem);
      if (!hWidget) {
        pSearchItem = sIterator->MoveToNext();
        continue;
      }
      if (pContainerItem && (pSearchItem->GetParent() != pContainerItem)) {
        bCurrentItem = TRUE;
        break;
      }
      CXFA_TabParam* pParam = new CXFA_TabParam;
      pParam->m_pWidget = hWidget;
      tabParams.Add(pParam);
      if (IsLayoutElement(pSearchItem->GetFormNode()->GetElementType(), true)) {
        OrderContainer(sIterator, pSearchItem, pParam, bCurrentItem,
                       bContentArea, bMarsterPage);
      }
    }
    if (bCurrentItem) {
      pSearchItem = sIterator->GetCurrent();
      bCurrentItem = FALSE;
    } else {
      pSearchItem = sIterator->MoveToNext();
    }
  }
  int32_t iChildren = tabParams.GetSize();
  if (iChildren > 1) {
    FXSYS_qsort(tabParams.GetData(), iChildren, sizeof(void*),
                XFA_TabOrderWidgetComparator);
  }
  for (int32_t iStart = 0; iStart < iChildren; iStart++) {
    CXFA_TabParam* pParam = tabParams[iStart];
    pContainer->m_Children.Add(pParam->m_pWidget);
    if (pParam->m_Children.GetSize() > 0) {
      pContainer->m_Children.Append(pParam->m_Children);
    }
    delete pParam;
  }
  tabParams.RemoveAll();
}
void CXFA_FFTabOrderPageWidgetIterator::CreateSpaceOrderWidgetArray(
    CXFA_WidgetArray& WidgetArray) {
  CXFA_LayoutItemIterator sIterator;
  sIterator.Init(m_pPageView);
  CXFA_TabParam* pParam = new CXFA_TabParam;
  FX_BOOL bCurrentItem = FALSE;
  FX_BOOL bContentArea = FALSE;
  OrderContainer(&sIterator, nullptr, pParam, bCurrentItem, bContentArea);
  if (pParam->m_Children.GetSize() > 0) {
    WidgetArray.Append(pParam->m_Children);
  }
  sIterator.Reset();
  bCurrentItem = FALSE;
  bContentArea = FALSE;
  pParam->m_Children.RemoveAll();
  OrderContainer(&sIterator, nullptr, pParam, bCurrentItem, bContentArea, TRUE);
  if (pParam->m_Children.GetSize() > 0) {
    WidgetArray.Append(pParam->m_Children);
  }
  delete pParam;
}
CXFA_FFWidget* CXFA_FFTabOrderPageWidgetIterator::GetWidget(
    CXFA_LayoutItem* pLayoutItem) {
  if (CXFA_FFWidget* pWidget = XFA_GetWidgetFromLayoutItem(pLayoutItem)) {
    if (!pWidget->IsLoaded() &&
        (pWidget->GetStatus() & XFA_WidgetStatus_Visible)) {
      pWidget->LoadWidget();
    }
    return pWidget;
  }
  return nullptr;
}

CXFA_TabParam::CXFA_TabParam() : m_pWidget(nullptr) {}

CXFA_TabParam::~CXFA_TabParam() {}
