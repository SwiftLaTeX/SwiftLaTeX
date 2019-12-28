// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_layout_itemlayout.h"

#include <algorithm>
#include <memory>

#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_layout_appadapter.h"
#include "xfa/fxfa/parser/xfa_layout_pagemgr_new.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

int32_t SeparateStringW(const FX_WCHAR* pStr,
                        int32_t iStrLen,
                        FX_WCHAR delimiter,
                        CFX_WideStringArray& pieces) {
  if (!pStr)
    return 0;
  if (iStrLen < 0)
    iStrLen = FXSYS_wcslen(pStr);

  const FX_WCHAR* pToken = pStr;
  const FX_WCHAR* pEnd = pStr + iStrLen;
  while (TRUE) {
    if (pStr >= pEnd || delimiter == *pStr) {
      CFX_WideString sub(pToken, pStr - pToken);
      pieces.Add(sub);
      pToken = pStr + 1;
      if (pStr >= pEnd)
        break;
    }
    pStr++;
  }
  return pieces.GetSize();
}

}  // namespace

CXFA_ItemLayoutProcessor::CXFA_ItemLayoutProcessor(CXFA_Node* pNode,
                                                   CXFA_LayoutPageMgr* pPageMgr)
    : m_bKeepBreakFinish(FALSE),
      m_bIsProcessKeep(FALSE),
      m_pKeepHeadNode(nullptr),
      m_pKeepTailNode(nullptr),
      m_pFormNode(pNode),
      m_pLayoutItem(nullptr),
      m_pOldLayoutItem(nullptr),
      m_pCurChildNode(XFA_LAYOUT_INVALIDNODE),
      m_pCurChildPreprocessor(nullptr),
      m_nCurChildNodeStage(XFA_ItemLayoutProcessorStages_None),
      m_fUsedSize(0),
      m_pPageMgr(pPageMgr),
      m_bBreakPending(TRUE),
      m_fLastRowWidth(0),
      m_fLastRowY(0),
      m_fWidthLimite(0),
      m_bUseInheriated(FALSE),
      m_ePreProcessRs(XFA_ItemLayoutProcessorResult_Done),
      m_bHasAvailHeight(TRUE) {
  ASSERT(m_pFormNode && (m_pFormNode->IsContainerNode() ||
                         m_pFormNode->GetElementType() == XFA_Element::Form));
  m_pOldLayoutItem =
      (CXFA_ContentLayoutItem*)m_pFormNode->GetUserData(XFA_LAYOUTITEMKEY);
}

CXFA_ItemLayoutProcessor::~CXFA_ItemLayoutProcessor() {}

CXFA_ContentLayoutItem* CXFA_ItemLayoutProcessor::CreateContentLayoutItem(
    CXFA_Node* pFormNode) {
  if (!pFormNode) {
    return nullptr;
  }
  CXFA_ContentLayoutItem* pLayoutItem = nullptr;
  if (m_pOldLayoutItem) {
    pLayoutItem = m_pOldLayoutItem;
    m_pOldLayoutItem = m_pOldLayoutItem->m_pNext;
    return pLayoutItem;
  }
  pLayoutItem = (CXFA_ContentLayoutItem*)pFormNode->GetDocument()
                    ->GetNotify()
                    ->OnCreateLayoutItem(pFormNode);
  CXFA_ContentLayoutItem* pPrevLayoutItem =
      (CXFA_ContentLayoutItem*)pFormNode->GetUserData(XFA_LAYOUTITEMKEY);
  if (pPrevLayoutItem) {
    while (pPrevLayoutItem->m_pNext) {
      pPrevLayoutItem = pPrevLayoutItem->m_pNext;
    }
    pPrevLayoutItem->m_pNext = pLayoutItem;
    pLayoutItem->m_pPrev = pPrevLayoutItem;
  } else {
    pFormNode->SetUserData(XFA_LAYOUTITEMKEY, pLayoutItem);
  }
  return pLayoutItem;
}
FX_BOOL CXFA_ItemLayoutProcessor::FindLayoutItemSplitPos(
    CXFA_ContentLayoutItem* pLayoutItem,
    FX_FLOAT fCurVerticalOffset,
    FX_FLOAT& fProposedSplitPos,
    FX_BOOL& bAppChange,
    FX_BOOL bCalculateMargin) {
  CXFA_Node* pFormNode = pLayoutItem->m_pFormNode;
  if (fProposedSplitPos > fCurVerticalOffset + XFA_LAYOUT_FLOAT_PERCISION &&
      fProposedSplitPos <= fCurVerticalOffset + pLayoutItem->m_sSize.y -
                               XFA_LAYOUT_FLOAT_PERCISION) {
    switch (pFormNode->GetIntact()) {
      case XFA_ATTRIBUTEENUM_None: {
        FX_BOOL bAnyChanged = FALSE;
        CXFA_Document* pDocument = pFormNode->GetDocument();
        CXFA_FFNotify* pNotify = pDocument->GetNotify();
        FX_FLOAT fCurTopMargin = 0, fCurBottomMargin = 0;
        CXFA_Node* pMarginNode =
            pFormNode->GetFirstChildByClass(XFA_Element::Margin);
        if (pMarginNode && bCalculateMargin) {
          fCurTopMargin = pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset)
                              .ToUnit(XFA_UNIT_Pt);
          fCurBottomMargin = pMarginNode->GetMeasure(XFA_ATTRIBUTE_BottomInset)
                                 .ToUnit(XFA_UNIT_Pt);
        }
        FX_BOOL bChanged = TRUE;
        while (bChanged) {
          bChanged = FALSE;
          {
            FX_FLOAT fRelSplitPos = fProposedSplitPos - fCurVerticalOffset;
            if (pNotify->FindSplitPos(pFormNode, pLayoutItem->GetIndex(),
                                      fRelSplitPos)) {
              bAnyChanged = TRUE;
              bChanged = TRUE;
              fProposedSplitPos = fCurVerticalOffset + fRelSplitPos;
              bAppChange = TRUE;
              if (fProposedSplitPos <=
                  fCurVerticalOffset + XFA_LAYOUT_FLOAT_PERCISION) {
                return TRUE;
              }
            }
          }
          FX_FLOAT fRelSplitPos = fProposedSplitPos - fCurBottomMargin;
          for (CXFA_ContentLayoutItem* pChildItem =
                   (CXFA_ContentLayoutItem*)pLayoutItem->m_pFirstChild;
               pChildItem;
               pChildItem =
                   (CXFA_ContentLayoutItem*)pChildItem->m_pNextSibling) {
            FX_FLOAT fChildOffset =
                fCurVerticalOffset + fCurTopMargin + pChildItem->m_sPos.y;
            FX_BOOL bChange = FALSE;
            if (FindLayoutItemSplitPos(pChildItem, fChildOffset, fRelSplitPos,
                                       bChange, bCalculateMargin)) {
              if (fRelSplitPos - fChildOffset < XFA_LAYOUT_FLOAT_PERCISION &&
                  bChange) {
                fProposedSplitPos = fRelSplitPos - fCurTopMargin;
              } else {
                fProposedSplitPos = fRelSplitPos + fCurBottomMargin;
              }
              bAnyChanged = TRUE;
              bChanged = TRUE;
              if (fProposedSplitPos <=
                  fCurVerticalOffset + XFA_LAYOUT_FLOAT_PERCISION) {
                return TRUE;
              }
              if (bAnyChanged) {
                break;
              }
            }
          }
        }
        return bAnyChanged;
      } break;
      case XFA_ATTRIBUTEENUM_ContentArea:
      case XFA_ATTRIBUTEENUM_PageArea: {
        fProposedSplitPos = fCurVerticalOffset;
        return TRUE;
      }
      default:
        return FALSE;
    }
  }
  return FALSE;
}
static XFA_ATTRIBUTEENUM XFA_ItemLayoutProcessor_GetLayout(
    CXFA_Node* pFormNode,
    FX_BOOL& bRootForceTb) {
  bRootForceTb = FALSE;
  XFA_ATTRIBUTEENUM eLayoutMode;
  if (pFormNode->TryEnum(XFA_ATTRIBUTE_Layout, eLayoutMode, FALSE)) {
    return eLayoutMode;
  }
  CXFA_Node* pParentNode = pFormNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (pParentNode && pParentNode->GetElementType() == XFA_Element::Form) {
    bRootForceTb = TRUE;
    return XFA_ATTRIBUTEENUM_Tb;
  }
  return XFA_ATTRIBUTEENUM_Position;
}
static FX_BOOL XFA_ExistContainerKeep(CXFA_Node* pCurNode, FX_BOOL bPreFind) {
  if (!pCurNode || !XFA_ItemLayoutProcessor_IsTakingSpace(pCurNode)) {
    return FALSE;
  }
  XFA_NODEITEM eItemType = XFA_NODEITEM_PrevSibling;
  if (!bPreFind) {
    eItemType = XFA_NODEITEM_NextSibling;
  }
  CXFA_Node* pPreContainer =
      pCurNode->GetNodeItem(eItemType, XFA_ObjectType::ContainerNode);
  if (!pPreContainer) {
    return FALSE;
  }
  CXFA_Node* pKeep = pCurNode->GetFirstChildByClass(XFA_Element::Keep);
  if (pKeep) {
    XFA_ATTRIBUTEENUM ePrevious;
    XFA_ATTRIBUTE eKeepType = XFA_ATTRIBUTE_Previous;
    if (!bPreFind) {
      eKeepType = XFA_ATTRIBUTE_Next;
    }
    if (pKeep->TryEnum(eKeepType, ePrevious, FALSE)) {
      if (ePrevious == XFA_ATTRIBUTEENUM_ContentArea ||
          ePrevious == XFA_ATTRIBUTEENUM_PageArea) {
        return TRUE;
      }
    }
  }
  pKeep = pPreContainer->GetFirstChildByClass(XFA_Element::Keep);
  if (!pKeep) {
    return FALSE;
  }
  XFA_ATTRIBUTEENUM eNext;
  XFA_ATTRIBUTE eKeepType = XFA_ATTRIBUTE_Next;
  if (!bPreFind) {
    eKeepType = XFA_ATTRIBUTE_Previous;
  }
  if (!pKeep->TryEnum(eKeepType, eNext, FALSE)) {
    return FALSE;
  }
  if (eNext == XFA_ATTRIBUTEENUM_ContentArea ||
      eNext == XFA_ATTRIBUTEENUM_PageArea) {
    return TRUE;
  }
  return FALSE;
}
FX_FLOAT CXFA_ItemLayoutProcessor::FindSplitPos(FX_FLOAT fProposedSplitPos) {
  ASSERT(m_pLayoutItem);
  XFA_ATTRIBUTEENUM eLayout = m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
  FX_BOOL bCalculateMargin = TRUE;
  if (eLayout == XFA_ATTRIBUTEENUM_Position) {
    bCalculateMargin = FALSE;
  }
  while (fProposedSplitPos > XFA_LAYOUT_FLOAT_PERCISION) {
    FX_BOOL bAppChange = FALSE;
    if (!FindLayoutItemSplitPos(m_pLayoutItem, 0, fProposedSplitPos, bAppChange,
                                bCalculateMargin)) {
      break;
    }
  }
  return fProposedSplitPos;
}
void CXFA_ItemLayoutProcessor::SplitLayoutItem(
    CXFA_ContentLayoutItem* pLayoutItem,
    CXFA_ContentLayoutItem* pSecondParent,
    FX_FLOAT fSplitPos) {
  FX_FLOAT fCurTopMargin = 0, fCurBottomMargin = 0;
  XFA_ATTRIBUTEENUM eLayout = m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
  FX_BOOL bCalculateMargin = TRUE;
  if (eLayout == XFA_ATTRIBUTEENUM_Position) {
    bCalculateMargin = FALSE;
  }
  CXFA_Node* pMarginNode =
      pLayoutItem->m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  if (pMarginNode && bCalculateMargin) {
    fCurTopMargin =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset).ToUnit(XFA_UNIT_Pt);
    fCurBottomMargin =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_BottomInset).ToUnit(XFA_UNIT_Pt);
  }
  CXFA_ContentLayoutItem* pSecondLayoutItem = nullptr;
  if (m_pCurChildPreprocessor &&
      m_pCurChildPreprocessor->m_pFormNode == pLayoutItem->m_pFormNode) {
    pSecondLayoutItem = m_pCurChildPreprocessor->CreateContentLayoutItem(
        pLayoutItem->m_pFormNode);
  } else {
    pSecondLayoutItem = CreateContentLayoutItem(pLayoutItem->m_pFormNode);
  }
  pSecondLayoutItem->m_sPos.x = pLayoutItem->m_sPos.x;
  pSecondLayoutItem->m_sSize.x = pLayoutItem->m_sSize.x;
  pSecondLayoutItem->m_sPos.y = 0;
  pSecondLayoutItem->m_sSize.y = pLayoutItem->m_sSize.y - fSplitPos;
  pLayoutItem->m_sSize.y -= pSecondLayoutItem->m_sSize.y;
  if (pLayoutItem->m_pFirstChild) {
    pSecondLayoutItem->m_sSize.y += fCurTopMargin;
  }
  if (pSecondParent) {
    pSecondParent->AddChild(pSecondLayoutItem);
    if (fCurTopMargin > 0 && pLayoutItem->m_pFirstChild) {
      pSecondParent->m_sSize.y += fCurTopMargin;
      CXFA_ContentLayoutItem* pParentItem =
          (CXFA_ContentLayoutItem*)pSecondParent->m_pParent;
      while (pParentItem) {
        pParentItem->m_sSize.y += fCurTopMargin;
        pParentItem = (CXFA_ContentLayoutItem*)pParentItem->m_pParent;
      }
    }
  } else {
    pSecondLayoutItem->m_pParent = pLayoutItem->m_pParent;
    pSecondLayoutItem->m_pNextSibling = pLayoutItem->m_pNextSibling;
    pLayoutItem->m_pNextSibling = pSecondLayoutItem;
  }
  CXFA_ContentLayoutItem* pChildren =
      (CXFA_ContentLayoutItem*)pLayoutItem->m_pFirstChild;
  pLayoutItem->m_pFirstChild = nullptr;
  FX_FLOAT lHeightForKeep = 0;
  CFX_ArrayTemplate<CXFA_ContentLayoutItem*> keepLayoutItems;
  FX_FLOAT fAddMarginHeight = 0;
  for (CXFA_ContentLayoutItem *pChildItem = pChildren, *pChildNext = nullptr;
       pChildItem; pChildItem = pChildNext) {
    pChildNext = (CXFA_ContentLayoutItem*)pChildItem->m_pNextSibling;
    pChildItem->m_pNextSibling = nullptr;
    if (fSplitPos <= fCurTopMargin + pChildItem->m_sPos.y + fCurBottomMargin +
                         XFA_LAYOUT_FLOAT_PERCISION) {
      if (!XFA_ExistContainerKeep(pChildItem->m_pFormNode, TRUE)) {
        pChildItem->m_sPos.y -= fSplitPos - fCurBottomMargin;
        pChildItem->m_sPos.y += lHeightForKeep;
        pChildItem->m_sPos.y += fAddMarginHeight;
        pSecondLayoutItem->AddChild(pChildItem);
      } else {
        if (lHeightForKeep < XFA_LAYOUT_FLOAT_PERCISION) {
          for (int32_t iIndex = 0; iIndex < keepLayoutItems.GetSize();
               iIndex++) {
            CXFA_ContentLayoutItem* pPreItem = keepLayoutItems[iIndex];
            pLayoutItem->RemoveChild(pPreItem);
            pPreItem->m_sPos.y -= fSplitPos;
            if (pPreItem->m_sPos.y < 0) {
              pPreItem->m_sPos.y = 0;
            }
            if (pPreItem->m_sPos.y + pPreItem->m_sSize.y > lHeightForKeep) {
              pPreItem->m_sPos.y = lHeightForKeep;
              lHeightForKeep += pPreItem->m_sSize.y;
              pSecondLayoutItem->m_sSize.y += pPreItem->m_sSize.y;
              if (pSecondParent) {
                pSecondParent->m_sSize.y += pPreItem->m_sSize.y;
              }
            }
            pSecondLayoutItem->AddChild(pPreItem);
          }
        }
        pChildItem->m_sPos.y -= fSplitPos;
        pChildItem->m_sPos.y += lHeightForKeep;
        pChildItem->m_sPos.y += fAddMarginHeight;
        pSecondLayoutItem->AddChild(pChildItem);
      }
    } else if (fSplitPos + XFA_LAYOUT_FLOAT_PERCISION >=
               fCurTopMargin + fCurBottomMargin + pChildItem->m_sPos.y +
                   pChildItem->m_sSize.y) {
      pLayoutItem->AddChild(pChildItem);
      if (XFA_ExistContainerKeep(pChildItem->m_pFormNode, FALSE)) {
        keepLayoutItems.Add(pChildItem);
      } else {
        keepLayoutItems.RemoveAll();
      }
    } else {
      FX_FLOAT fOldHeight = pSecondLayoutItem->m_sSize.y;
      SplitLayoutItem(
          pChildItem, pSecondLayoutItem,
          fSplitPos - fCurTopMargin - fCurBottomMargin - pChildItem->m_sPos.y);
      fAddMarginHeight = pSecondLayoutItem->m_sSize.y - fOldHeight;
      pLayoutItem->AddChild(pChildItem);
    }
  }
}
void CXFA_ItemLayoutProcessor::SplitLayoutItem(FX_FLOAT fSplitPos) {
  ASSERT(m_pLayoutItem);
  SplitLayoutItem(m_pLayoutItem, nullptr, fSplitPos);
}

CXFA_ContainerLayoutItem* CXFA_LayoutItem::GetPage() const {
  for (CXFA_LayoutItem* pCurNode = const_cast<CXFA_LayoutItem*>(this); pCurNode;
       pCurNode = pCurNode->m_pParent) {
    if (pCurNode->m_pFormNode->GetElementType() == XFA_Element::PageArea)
      return static_cast<CXFA_ContainerLayoutItem*>(pCurNode);
  }
  return nullptr;
}

CXFA_Node* CXFA_LayoutItem::GetFormNode() const {
  return m_pFormNode;
}

void CXFA_LayoutItem::GetRect(CFX_RectF& rtLayout, FX_BOOL bRelative) const {
  ASSERT(m_bIsContentLayoutItem);
  const CXFA_ContentLayoutItem* pThis =
      static_cast<const CXFA_ContentLayoutItem*>(this);
  CFX_PointF sPos = pThis->m_sPos;
  CFX_SizeF sSize = pThis->m_sSize;
  if (!bRelative) {
    for (CXFA_LayoutItem* pLayoutItem = pThis->m_pParent; pLayoutItem;
         pLayoutItem = pLayoutItem->m_pParent) {
      if (CXFA_ContentLayoutItem* pContent =
              pLayoutItem->AsContentLayoutItem()) {
        sPos += pContent->m_sPos;
        if (CXFA_Node* pMarginNode =
                pLayoutItem->m_pFormNode->GetFirstChildByClass(
                    XFA_Element::Margin)) {
          sPos += CFX_PointF(pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset)
                                 .ToUnit(XFA_UNIT_Pt),
                             pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset)
                                 .ToUnit(XFA_UNIT_Pt));
        }
      } else {
        if (pLayoutItem->m_pFormNode->GetElementType() ==
            XFA_Element::ContentArea) {
          sPos +=
              CFX_PointF(pLayoutItem->m_pFormNode->GetMeasure(XFA_ATTRIBUTE_X)
                             .ToUnit(XFA_UNIT_Pt),
                         pLayoutItem->m_pFormNode->GetMeasure(XFA_ATTRIBUTE_Y)
                             .ToUnit(XFA_UNIT_Pt));
          break;
        } else if (pLayoutItem->m_pFormNode->GetElementType() ==
                   XFA_Element::PageArea) {
          break;
        }
      }
    }
  }
  rtLayout.Set(sPos.x, sPos.y, sSize.x, sSize.y);
}

CXFA_LayoutItem* CXFA_LayoutItem::GetParent() const {
  return m_pParent;
}

const CXFA_LayoutItem* CXFA_LayoutItem::GetFirst() const {
  ASSERT(m_bIsContentLayoutItem);
  const CXFA_ContentLayoutItem* pCurNode =
      static_cast<const CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pPrev) {
    pCurNode = pCurNode->m_pPrev;
  }
  return pCurNode;
}

CXFA_LayoutItem* CXFA_LayoutItem::GetFirst() {
  ASSERT(m_bIsContentLayoutItem);
  CXFA_ContentLayoutItem* pCurNode = static_cast<CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pPrev) {
    pCurNode = pCurNode->m_pPrev;
  }
  return pCurNode;
}

CXFA_LayoutItem* CXFA_LayoutItem::GetLast() {
  ASSERT(m_bIsContentLayoutItem);
  CXFA_ContentLayoutItem* pCurNode = static_cast<CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pNext) {
    pCurNode = pCurNode->m_pNext;
  }
  return pCurNode;
}

const CXFA_LayoutItem* CXFA_LayoutItem::GetLast() const {
  ASSERT(m_bIsContentLayoutItem);
  const CXFA_ContentLayoutItem* pCurNode =
      static_cast<const CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pNext) {
    pCurNode = pCurNode->m_pNext;
  }
  return pCurNode;
}

CXFA_LayoutItem* CXFA_LayoutItem::GetPrev() const {
  ASSERT(m_bIsContentLayoutItem);
  return static_cast<const CXFA_ContentLayoutItem*>(this)->m_pPrev;
}

CXFA_LayoutItem* CXFA_LayoutItem::GetNext() const {
  ASSERT(m_bIsContentLayoutItem);
  return static_cast<const CXFA_ContentLayoutItem*>(this)->m_pNext;
}

int32_t CXFA_LayoutItem::GetIndex() const {
  ASSERT(m_bIsContentLayoutItem);
  int32_t iIndex = 0;
  const CXFA_ContentLayoutItem* pCurNode =
      static_cast<const CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pPrev) {
    pCurNode = pCurNode->m_pPrev;
    ++iIndex;
  }
  return iIndex;
}

int32_t CXFA_LayoutItem::GetCount() const {
  ASSERT(m_bIsContentLayoutItem);
  int32_t iCount = GetIndex() + 1;
  const CXFA_ContentLayoutItem* pCurNode =
      static_cast<const CXFA_ContentLayoutItem*>(this);
  while (pCurNode->m_pNext) {
    pCurNode = pCurNode->m_pNext;
    iCount++;
  }
  return iCount;
}

void CXFA_LayoutItem::AddChild(CXFA_LayoutItem* pChildItem) {
  if (pChildItem->m_pParent) {
    pChildItem->m_pParent->RemoveChild(pChildItem);
  }
  pChildItem->m_pParent = this;
  if (!m_pFirstChild) {
    m_pFirstChild = pChildItem;
  } else {
    CXFA_LayoutItem* pExistingChildItem = m_pFirstChild;
    while (pExistingChildItem->m_pNextSibling) {
      pExistingChildItem = pExistingChildItem->m_pNextSibling;
    }
    pExistingChildItem->m_pNextSibling = pChildItem;
  }
}
void CXFA_LayoutItem::AddHeadChild(CXFA_LayoutItem* pChildItem) {
  if (pChildItem->m_pParent) {
    pChildItem->m_pParent->RemoveChild(pChildItem);
  }
  pChildItem->m_pParent = this;
  if (!m_pFirstChild) {
    m_pFirstChild = pChildItem;
  } else {
    CXFA_LayoutItem* pExistingChildItem = m_pFirstChild;
    m_pFirstChild = pChildItem;
    m_pFirstChild->m_pNextSibling = pExistingChildItem;
  }
}
void CXFA_LayoutItem::InsertChild(CXFA_LayoutItem* pBeforeItem,
                                  CXFA_LayoutItem* pChildItem) {
  if (pBeforeItem->m_pParent != this) {
    return;
  }
  if (pChildItem->m_pParent) {
    pChildItem->m_pParent = nullptr;
  }
  pChildItem->m_pParent = this;
  CXFA_LayoutItem* pExistingChildItem = pBeforeItem->m_pNextSibling;
  pBeforeItem->m_pNextSibling = pChildItem;
  pChildItem->m_pNextSibling = pExistingChildItem;
}
void CXFA_LayoutItem::RemoveChild(CXFA_LayoutItem* pChildItem) {
  if (pChildItem->m_pParent != this) {
    return;
  }
  if (m_pFirstChild == pChildItem) {
    m_pFirstChild = pChildItem->m_pNextSibling;
  } else {
    CXFA_LayoutItem* pExistingChildItem = m_pFirstChild;
    while (pExistingChildItem &&
           pExistingChildItem->m_pNextSibling != pChildItem) {
      pExistingChildItem = pExistingChildItem->m_pNextSibling;
    }
    if (pExistingChildItem) {
      pExistingChildItem->m_pNextSibling = pChildItem->m_pNextSibling;
    }
  }
  pChildItem->m_pNextSibling = nullptr;
  pChildItem->m_pParent = nullptr;
}
CXFA_ContentLayoutItem* CXFA_ItemLayoutProcessor::ExtractLayoutItem() {
  CXFA_ContentLayoutItem* pLayoutItem = m_pLayoutItem;
  if (pLayoutItem) {
    m_pLayoutItem =
        static_cast<CXFA_ContentLayoutItem*>(pLayoutItem->m_pNextSibling);
    pLayoutItem->m_pNextSibling = nullptr;
  }
  if (m_nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Done ||
      !ToContentLayoutItem(m_pOldLayoutItem))
    return pLayoutItem;
  if (m_pOldLayoutItem->m_pPrev)
    m_pOldLayoutItem->m_pPrev->m_pNext = nullptr;
  CXFA_FFNotify* pNotify =
      m_pOldLayoutItem->m_pFormNode->GetDocument()->GetNotify();
  CXFA_LayoutProcessor* pDocLayout =
      m_pOldLayoutItem->m_pFormNode->GetDocument()->GetDocLayout();
  CXFA_ContentLayoutItem* pOldLayoutItem = m_pOldLayoutItem;
  while (pOldLayoutItem) {
    CXFA_ContentLayoutItem* pNextOldLayoutItem = pOldLayoutItem->m_pNext;
    pNotify->OnLayoutItemRemoving(pDocLayout, pOldLayoutItem);
    if (pOldLayoutItem->m_pParent)
      pOldLayoutItem->m_pParent->RemoveChild(pOldLayoutItem);
    delete pOldLayoutItem;
    pOldLayoutItem = pNextOldLayoutItem;
  }
  m_pOldLayoutItem = nullptr;
  return pLayoutItem;
}
static FX_BOOL XFA_ItemLayoutProcessor_FindBreakNode(
    CXFA_Node* pContainerNode,
    CXFA_Node*& pCurActionNode,
    XFA_ItemLayoutProcessorStages& nCurStage,
    FX_BOOL bBreakBefore) {
  FX_BOOL bFindRs = FALSE;
  for (CXFA_Node* pBreakNode = pContainerNode; pBreakNode;
       pBreakNode = pBreakNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    XFA_ATTRIBUTE eAttributeType = XFA_ATTRIBUTE_Before;
    if (!bBreakBefore) {
      eAttributeType = XFA_ATTRIBUTE_After;
    }
    switch (pBreakNode->GetElementType()) {
      case XFA_Element::BreakBefore: {
        if (bBreakBefore) {
          pCurActionNode = pBreakNode;
          nCurStage = XFA_ItemLayoutProcessorStages_BreakBefore;
          bFindRs = TRUE;
        }
      } break;
      case XFA_Element::BreakAfter: {
        if (!bBreakBefore) {
          pCurActionNode = pBreakNode;
          nCurStage = XFA_ItemLayoutProcessorStages_BreakAfter;
          bFindRs = TRUE;
        }
      } break;
      case XFA_Element::Break:
        if (pBreakNode->GetEnum(eAttributeType) != XFA_ATTRIBUTEENUM_Auto) {
          pCurActionNode = pBreakNode;
          nCurStage = XFA_ItemLayoutProcessorStages_BreakBefore;
          if (!bBreakBefore) {
            nCurStage = XFA_ItemLayoutProcessorStages_BreakAfter;
          }
          bFindRs = TRUE;
          break;
        }
      default:
        break;
    }
    if (bFindRs) {
      break;
    }
  }
  return bFindRs;
}
static void XFA_DeleteLayoutGeneratedNode(CXFA_Node* pGenerateNode) {
  CXFA_FFNotify* pNotify = pGenerateNode->GetDocument()->GetNotify();
  CXFA_LayoutProcessor* pDocLayout =
      pGenerateNode->GetDocument()->GetDocLayout();
  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode> sIterator(
      pGenerateNode);
  for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
       pNode = sIterator.MoveToNext()) {
    CXFA_ContentLayoutItem* pCurLayoutItem =
        (CXFA_ContentLayoutItem*)pNode->GetUserData(XFA_LAYOUTITEMKEY);
    CXFA_ContentLayoutItem* pNextLayoutItem = nullptr;
    while (pCurLayoutItem) {
      pNextLayoutItem = pCurLayoutItem->m_pNext;
      pNotify->OnLayoutItemRemoving(pDocLayout, pCurLayoutItem);
      delete pCurLayoutItem;
      pCurLayoutItem = pNextLayoutItem;
    }
  }
  pGenerateNode->GetNodeItem(XFA_NODEITEM_Parent)->RemoveChild(pGenerateNode);
}
void CXFA_ItemLayoutProcessor::XFA_ItemLayoutProcessor_GotoNextContainerNode(
    CXFA_Node*& pCurActionNode,
    XFA_ItemLayoutProcessorStages& nCurStage,
    CXFA_Node* pParentContainer,
    FX_BOOL bUsePageBreak) {
  CXFA_Node* pEntireContainer = pParentContainer;
  CXFA_Node* pChildContainer = XFA_LAYOUT_INVALIDNODE;
  switch (nCurStage) {
    case XFA_ItemLayoutProcessorStages_BreakBefore:
    case XFA_ItemLayoutProcessorStages_BreakAfter: {
      pChildContainer = pCurActionNode->GetNodeItem(XFA_NODEITEM_Parent);
    } break;
    case XFA_ItemLayoutProcessorStages_Keep:
    case XFA_ItemLayoutProcessorStages_Container:
      pChildContainer = pCurActionNode;
      break;
    default:
      pChildContainer = XFA_LAYOUT_INVALIDNODE;
      break;
  }
  switch (nCurStage) {
    case XFA_ItemLayoutProcessorStages_Keep: {
      CXFA_Node* pBreakAfterNode =
          pChildContainer->GetNodeItem(XFA_NODEITEM_FirstChild);
      if (!m_bKeepBreakFinish &&
          XFA_ItemLayoutProcessor_FindBreakNode(pBreakAfterNode, pCurActionNode,
                                                nCurStage, FALSE)) {
        return;
      }
      goto CheckNextChildContainer;
    }
    case XFA_ItemLayoutProcessorStages_None: {
      pCurActionNode = XFA_LAYOUT_INVALIDNODE;
      case XFA_ItemLayoutProcessorStages_BookendLeader:
        for (CXFA_Node* pBookendNode =
                 pCurActionNode == XFA_LAYOUT_INVALIDNODE
                     ? pEntireContainer->GetNodeItem(XFA_NODEITEM_FirstChild)
                     : pCurActionNode->GetNodeItem(XFA_NODEITEM_NextSibling);
             pBookendNode; pBookendNode = pBookendNode->GetNodeItem(
                               XFA_NODEITEM_NextSibling)) {
          switch (pBookendNode->GetElementType()) {
            case XFA_Element::Bookend:
            case XFA_Element::Break:
              pCurActionNode = pBookendNode;
              nCurStage = XFA_ItemLayoutProcessorStages_BookendLeader;
              return;
            default:
              break;
          }
        }
    }
      {
        pCurActionNode = XFA_LAYOUT_INVALIDNODE;
        case XFA_ItemLayoutProcessorStages_BreakBefore:
          if (pCurActionNode != XFA_LAYOUT_INVALIDNODE) {
            CXFA_Node* pBreakBeforeNode =
                pCurActionNode->GetNodeItem(XFA_NODEITEM_NextSibling);
            if (!m_bKeepBreakFinish &&
                XFA_ItemLayoutProcessor_FindBreakNode(
                    pBreakBeforeNode, pCurActionNode, nCurStage, TRUE)) {
              return;
            }
            if (m_bIsProcessKeep) {
              if (ProcessKeepNodesForBreakBefore(pCurActionNode, nCurStage,
                                                 pChildContainer)) {
                return;
              }
              goto CheckNextChildContainer;
            }
            pCurActionNode = pChildContainer;
            nCurStage = XFA_ItemLayoutProcessorStages_Container;
            return;
          }
          goto CheckNextChildContainer;
      }
    case XFA_ItemLayoutProcessorStages_Container: {
      pCurActionNode = XFA_LAYOUT_INVALIDNODE;
      case XFA_ItemLayoutProcessorStages_BreakAfter: {
        if (pCurActionNode == XFA_LAYOUT_INVALIDNODE) {
          CXFA_Node* pBreakAfterNode =
              pChildContainer->GetNodeItem(XFA_NODEITEM_FirstChild);
          if (!m_bKeepBreakFinish &&
              XFA_ItemLayoutProcessor_FindBreakNode(
                  pBreakAfterNode, pCurActionNode, nCurStage, FALSE)) {
            return;
          }
        } else {
          CXFA_Node* pBreakAfterNode =
              pCurActionNode->GetNodeItem(XFA_NODEITEM_NextSibling);
          if (XFA_ItemLayoutProcessor_FindBreakNode(
                  pBreakAfterNode, pCurActionNode, nCurStage, FALSE)) {
            return;
          }
        }
        goto CheckNextChildContainer;
      }
    }
    CheckNextChildContainer : {
      CXFA_Node* pNextChildContainer =
          pChildContainer == XFA_LAYOUT_INVALIDNODE
              ? pEntireContainer->GetNodeItem(XFA_NODEITEM_FirstChild,
                                              XFA_ObjectType::ContainerNode)
              : pChildContainer->GetNodeItem(XFA_NODEITEM_NextSibling,
                                             XFA_ObjectType::ContainerNode);
      while (pNextChildContainer &&
             pNextChildContainer->IsLayoutGeneratedNode()) {
        CXFA_Node* pSaveNode = pNextChildContainer;
        pNextChildContainer = pNextChildContainer->GetNodeItem(
            XFA_NODEITEM_NextSibling, XFA_ObjectType::ContainerNode);
        if (pSaveNode->IsUnusedNode())
          XFA_DeleteLayoutGeneratedNode(pSaveNode);
      }
      if (!pNextChildContainer) {
        goto NoMoreChildContainer;
      }
      FX_BOOL bLastKeep = FALSE;
      if (ProcessKeepNodesForCheckNext(pCurActionNode, nCurStage,
                                       pNextChildContainer, bLastKeep)) {
        return;
      }
      if (!m_bKeepBreakFinish && !bLastKeep &&
          XFA_ItemLayoutProcessor_FindBreakNode(
              pNextChildContainer->GetNodeItem(XFA_NODEITEM_FirstChild),
              pCurActionNode, nCurStage, TRUE)) {
        return;
      }
      pCurActionNode = pNextChildContainer;
      if (m_bIsProcessKeep) {
        nCurStage = XFA_ItemLayoutProcessorStages_Keep;
      } else {
        nCurStage = XFA_ItemLayoutProcessorStages_Container;
      }
      return;
    }
    NoMoreChildContainer : {
      pCurActionNode = XFA_LAYOUT_INVALIDNODE;
      case XFA_ItemLayoutProcessorStages_BookendTrailer:
        for (CXFA_Node* pBookendNode =
                 pCurActionNode == XFA_LAYOUT_INVALIDNODE
                     ? pEntireContainer->GetNodeItem(XFA_NODEITEM_FirstChild)
                     : pCurActionNode->GetNodeItem(XFA_NODEITEM_NextSibling);
             pBookendNode; pBookendNode = pBookendNode->GetNodeItem(
                               XFA_NODEITEM_NextSibling)) {
          switch (pBookendNode->GetElementType()) {
            case XFA_Element::Bookend:
            case XFA_Element::Break:
              pCurActionNode = pBookendNode;
              nCurStage = XFA_ItemLayoutProcessorStages_BookendTrailer;
              return;
            default:
              break;
          }
        }
    }
    default:
      pCurActionNode = nullptr;
      nCurStage = XFA_ItemLayoutProcessorStages_Done;
  }
}
FX_BOOL CXFA_ItemLayoutProcessor::ProcessKeepNodesForCheckNext(
    CXFA_Node*& pCurActionNode,
    XFA_ItemLayoutProcessorStages& nCurStage,
    CXFA_Node*& pNextContainer,
    FX_BOOL& bLastKeepNode) {
  const bool bCanSplit = pNextContainer->GetIntact() == XFA_ATTRIBUTEENUM_None;
  FX_BOOL bNextKeep = FALSE;
  if (XFA_ExistContainerKeep(pNextContainer, FALSE)) {
    bNextKeep = TRUE;
  }
  if (bNextKeep && !bCanSplit) {
    if (!m_bIsProcessKeep && !m_bKeepBreakFinish) {
      m_pKeepHeadNode = pNextContainer;
      m_bIsProcessKeep = TRUE;
    }
  } else {
    if (m_bIsProcessKeep && m_pKeepHeadNode) {
      m_pKeepTailNode = pNextContainer;
      if (!m_bKeepBreakFinish &&
          XFA_ItemLayoutProcessor_FindBreakNode(
              pNextContainer->GetNodeItem(XFA_NODEITEM_FirstChild),
              pCurActionNode, nCurStage, TRUE)) {
        return TRUE;
      } else {
        pNextContainer = m_pKeepHeadNode;
        m_bKeepBreakFinish = TRUE;
        m_pKeepHeadNode = nullptr;
        m_pKeepTailNode = nullptr;
        m_bIsProcessKeep = FALSE;
      }
    } else {
      if (m_bKeepBreakFinish) {
        bLastKeepNode = TRUE;
      }
      m_bKeepBreakFinish = FALSE;
    }
  }
  return FALSE;
}
FX_BOOL CXFA_ItemLayoutProcessor::ProcessKeepNodesForBreakBefore(
    CXFA_Node*& pCurActionNode,
    XFA_ItemLayoutProcessorStages& nCurStage,
    CXFA_Node* pContainerNode) {
  if (m_pKeepTailNode == pContainerNode) {
    pCurActionNode = m_pKeepHeadNode;
    m_bKeepBreakFinish = TRUE;
    m_pKeepHeadNode = nullptr;
    m_pKeepTailNode = nullptr;
    m_bIsProcessKeep = FALSE;
    nCurStage = XFA_ItemLayoutProcessorStages_Container;
    return TRUE;
  }
  CXFA_Node* pBreakAfterNode =
      pContainerNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  if (XFA_ItemLayoutProcessor_FindBreakNode(pBreakAfterNode, pCurActionNode,
                                            nCurStage, FALSE)) {
    return TRUE;
  }
  return FALSE;
}
FX_BOOL XFA_ItemLayoutProcessor_IsTakingSpace(CXFA_Node* pNode) {
  XFA_ATTRIBUTEENUM ePresence = pNode->GetEnum(XFA_ATTRIBUTE_Presence);
  return ePresence == XFA_ATTRIBUTEENUM_Visible ||
         ePresence == XFA_ATTRIBUTEENUM_Invisible;
}
static inline void XFA_ItemLayoutProcessor_CalculateContainerSpecfiedSize(
    CXFA_Node* pFormNode,
    FX_FLOAT& fContainerWidth,
    FX_FLOAT& fContainerHeight,
    FX_BOOL& bContainerWidthAutoSize,
    FX_BOOL& bContainerHeightAutoSize) {
  fContainerWidth = 0;
  fContainerHeight = 0;
  bContainerWidthAutoSize = TRUE;
  bContainerHeightAutoSize = TRUE;
  XFA_Element eType = pFormNode->GetElementType();
  CXFA_Measurement mTmpValue;
  if (bContainerWidthAutoSize &&
      (eType == XFA_Element::Subform || eType == XFA_Element::ExclGroup) &&
      pFormNode->TryMeasure(XFA_ATTRIBUTE_W, mTmpValue, FALSE) &&
      mTmpValue.GetValue() > XFA_LAYOUT_FLOAT_PERCISION) {
    fContainerWidth = mTmpValue.ToUnit(XFA_UNIT_Pt);
    bContainerWidthAutoSize = FALSE;
  }
  if (bContainerHeightAutoSize &&
      (eType == XFA_Element::Subform || eType == XFA_Element::ExclGroup) &&
      pFormNode->TryMeasure(XFA_ATTRIBUTE_H, mTmpValue, FALSE) &&
      mTmpValue.GetValue() > XFA_LAYOUT_FLOAT_PERCISION) {
    fContainerHeight = mTmpValue.ToUnit(XFA_UNIT_Pt);
    bContainerHeightAutoSize = FALSE;
  }
  if (bContainerWidthAutoSize && eType == XFA_Element::Subform &&
      pFormNode->TryMeasure(XFA_ATTRIBUTE_MaxW, mTmpValue, FALSE) &&
      mTmpValue.GetValue() > XFA_LAYOUT_FLOAT_PERCISION) {
    fContainerWidth = mTmpValue.ToUnit(XFA_UNIT_Pt);
    bContainerWidthAutoSize = FALSE;
  }
  if (bContainerHeightAutoSize && eType == XFA_Element::Subform &&
      pFormNode->TryMeasure(XFA_ATTRIBUTE_MaxH, mTmpValue, FALSE) &&
      mTmpValue.GetValue() > XFA_LAYOUT_FLOAT_PERCISION) {
    fContainerHeight = mTmpValue.ToUnit(XFA_UNIT_Pt);
    bContainerHeightAutoSize = FALSE;
  }
}
static inline void
XFA_ItemLayoutProcessor_CalculateContainerComponentSizeFromContentSize(
    CXFA_Node* pFormNode,
    FX_BOOL bContainerWidthAutoSize,
    FX_FLOAT fContentCalculatedWidth,
    FX_FLOAT& fContainerWidth,
    FX_BOOL bContainerHeightAutoSize,
    FX_FLOAT fContentCalculatedHeight,
    FX_FLOAT& fContainerHeight) {
  CXFA_Node* pMarginNode = pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  CXFA_Measurement mTmpValue;
  if (bContainerWidthAutoSize) {
    fContainerWidth = fContentCalculatedWidth;
    if (pMarginNode) {
      if (pMarginNode->TryMeasure(XFA_ATTRIBUTE_LeftInset, mTmpValue, FALSE)) {
        fContainerWidth += mTmpValue.ToUnit(XFA_UNIT_Pt);
      }
      if (pMarginNode->TryMeasure(XFA_ATTRIBUTE_RightInset, mTmpValue, FALSE)) {
        fContainerWidth += mTmpValue.ToUnit(XFA_UNIT_Pt);
      }
    }
  }
  if (bContainerHeightAutoSize) {
    fContainerHeight = fContentCalculatedHeight;
    if (pMarginNode) {
      if (pMarginNode->TryMeasure(XFA_ATTRIBUTE_TopInset, mTmpValue, FALSE)) {
        fContainerHeight += mTmpValue.ToUnit(XFA_UNIT_Pt);
      }
      if (pMarginNode->TryMeasure(XFA_ATTRIBUTE_BottomInset, mTmpValue,
                                  FALSE)) {
        fContainerHeight += mTmpValue.ToUnit(XFA_UNIT_Pt);
      }
    }
  }
}
void CXFA_ItemLayoutProcessor::CalculatePositionedContainerPos(
    CXFA_Node* pNode,
    FX_FLOAT fWidth,
    FX_FLOAT fHeight,
    FX_FLOAT& fAbsoluteX,
    FX_FLOAT& fAbsoluteY) {
  XFA_ATTRIBUTEENUM eAnchorType = pNode->GetEnum(XFA_ATTRIBUTE_AnchorType);
  int32_t nAnchorType = 0;
  switch (eAnchorType) {
    case XFA_ATTRIBUTEENUM_TopLeft:
      nAnchorType = 0;
      break;
    case XFA_ATTRIBUTEENUM_TopCenter:
      nAnchorType = 1;
      break;
    case XFA_ATTRIBUTEENUM_TopRight:
      nAnchorType = 2;
      break;
    case XFA_ATTRIBUTEENUM_MiddleLeft:
      nAnchorType = 3;
      break;
    case XFA_ATTRIBUTEENUM_MiddleCenter:
      nAnchorType = 4;
      break;
    case XFA_ATTRIBUTEENUM_MiddleRight:
      nAnchorType = 5;
      break;
    case XFA_ATTRIBUTEENUM_BottomLeft:
      nAnchorType = 6;
      break;
    case XFA_ATTRIBUTEENUM_BottomCenter:
      nAnchorType = 7;
      break;
    case XFA_ATTRIBUTEENUM_BottomRight:
      nAnchorType = 8;
      break;
    default:
      break;
  }
  static const uint8_t nNextPos[4][9] = {{0, 1, 2, 3, 4, 5, 6, 7, 8},
                                         {6, 3, 0, 7, 4, 1, 8, 5, 2},
                                         {8, 7, 6, 5, 4, 3, 2, 1, 0},
                                         {2, 5, 8, 1, 4, 7, 0, 3, 6}};

  FX_FLOAT fAnchorX = pNode->GetMeasure(XFA_ATTRIBUTE_X).ToUnit(XFA_UNIT_Pt);
  FX_FLOAT fAnchorY = pNode->GetMeasure(XFA_ATTRIBUTE_Y).ToUnit(XFA_UNIT_Pt);
  int32_t nRotate =
      FXSYS_round(pNode->GetMeasure(XFA_ATTRIBUTE_Rotate).GetValue());
  nRotate = XFA_MapRotation(nRotate) / 90;
  int32_t nAbsoluteAnchorType = nNextPos[nRotate][nAnchorType];
  fAbsoluteX = fAnchorX;
  fAbsoluteY = fAnchorY;
  switch (nAbsoluteAnchorType / 3) {
    case 1:
      fAbsoluteY -= fHeight / 2;
      break;
    case 2:
      fAbsoluteY -= fHeight;
      break;
    default:
      break;
  }
  switch (nAbsoluteAnchorType % 3) {
    case 1:
      fAbsoluteX -= fWidth / 2;
      break;
    case 2:
      fAbsoluteX -= fWidth;
      break;
    default:
      break;
  }
}
FX_BOOL CXFA_ItemLayoutProcessor::IncrementRelayoutNode(
    CXFA_LayoutProcessor* pLayoutProcessor,
    CXFA_Node* pNode,
    CXFA_Node* pParentNode) {
  return FALSE;
}
void CXFA_ItemLayoutProcessor::DoLayoutPageArea(
    CXFA_ContainerLayoutItem* pPageAreaLayoutItem) {
  CXFA_Node* pFormNode = pPageAreaLayoutItem->m_pFormNode;
  CXFA_Node* pCurChildNode = XFA_LAYOUT_INVALIDNODE;
  XFA_ItemLayoutProcessorStages nCurChildNodeStage =
      XFA_ItemLayoutProcessorStages_None;
  CXFA_LayoutItem* pBeforeItem = nullptr;
  for (XFA_ItemLayoutProcessor_GotoNextContainerNode(
           pCurChildNode, nCurChildNodeStage, pFormNode, FALSE);
       pCurChildNode; XFA_ItemLayoutProcessor_GotoNextContainerNode(
           pCurChildNode, nCurChildNodeStage, pFormNode, FALSE)) {
    if (nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Container) {
      continue;
    }
    if (pCurChildNode->GetElementType() == XFA_Element::Variables) {
      continue;
    }
    CXFA_ItemLayoutProcessor* pProcessor =
        new CXFA_ItemLayoutProcessor(pCurChildNode, nullptr);
    pProcessor->DoLayout(FALSE, XFA_LAYOUT_FLOAT_MAX);
    if (!pProcessor->HasLayoutItem()) {
      delete pProcessor;
      continue;
    }
    FX_FLOAT fWidth, fHeight;
    pProcessor->GetCurrentComponentSize(fWidth, fHeight);
    FX_FLOAT fAbsoluteX = 0, fAbsoluteY = 0;
    CalculatePositionedContainerPos(pCurChildNode, fWidth, fHeight, fAbsoluteX,
                                    fAbsoluteY);
    pProcessor->SetCurrentComponentPos(fAbsoluteX, fAbsoluteY);
    CXFA_LayoutItem* pProcessItem = pProcessor->ExtractLayoutItem();
    if (!pBeforeItem) {
      pPageAreaLayoutItem->AddHeadChild(pProcessItem);
    } else {
      pPageAreaLayoutItem->InsertChild(pBeforeItem, pProcessItem);
    }
    pBeforeItem = pProcessItem;
    delete pProcessor;
  }
  pBeforeItem = nullptr;
  CXFA_LayoutItem* pLayoutItem = pPageAreaLayoutItem->m_pFirstChild;
  while (pLayoutItem) {
    if (!pLayoutItem->IsContentLayoutItem() ||
        pLayoutItem->m_pFormNode->GetElementType() != XFA_Element::Draw) {
      pLayoutItem = pLayoutItem->m_pNextSibling;
      continue;
    }
    if (pLayoutItem->m_pFormNode->GetElementType() == XFA_Element::Draw) {
      CXFA_LayoutItem* pNextLayoutItem = pLayoutItem->m_pNextSibling;
      pPageAreaLayoutItem->RemoveChild(pLayoutItem);
      if (!pBeforeItem) {
        pPageAreaLayoutItem->AddHeadChild(pLayoutItem);
      } else {
        pPageAreaLayoutItem->InsertChild(pBeforeItem, pLayoutItem);
      }
      pBeforeItem = pLayoutItem;
      pLayoutItem = pNextLayoutItem;
    }
  }
}
void CXFA_ItemLayoutProcessor::DoLayoutPositionedContainer(
    CXFA_LayoutContext* pContext) {
  if (m_pLayoutItem)
    return;

  m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
  FX_BOOL bIgnoreXY = (m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout) !=
                       XFA_ATTRIBUTEENUM_Position);
  FX_FLOAT fContainerWidth = 0, fContainerHeight = 0;
  FX_BOOL bContainerWidthAutoSize = TRUE, bContainerHeightAutoSize = TRUE;
  XFA_ItemLayoutProcessor_CalculateContainerSpecfiedSize(
      m_pFormNode, fContainerWidth, fContainerHeight, bContainerWidthAutoSize,
      bContainerHeightAutoSize);
  FX_FLOAT fContentCalculatedWidth = 0, fContentCalculatedHeight = 0;
  FX_FLOAT fHiddenContentCalculatedWidth = 0,
           fHiddenContentCalculatedHeight = 0;
  if (m_pCurChildNode == XFA_LAYOUT_INVALIDNODE) {
    XFA_ItemLayoutProcessor_GotoNextContainerNode(
        m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, FALSE);
  }
  int32_t iColIndex = 0;
  for (; m_pCurChildNode; XFA_ItemLayoutProcessor_GotoNextContainerNode(
           m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, FALSE)) {
    if (m_nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Container) {
      continue;
    }
    if (m_pCurChildNode->GetElementType() == XFA_Element::Variables) {
      continue;
    }
    CXFA_ItemLayoutProcessor* pProcessor =
        new CXFA_ItemLayoutProcessor(m_pCurChildNode, m_pPageMgr);
    if (pContext && pContext->m_prgSpecifiedColumnWidths) {
      int32_t iColSpan = m_pCurChildNode->GetInteger(XFA_ATTRIBUTE_ColSpan);
      if (iColSpan <=
          pContext->m_prgSpecifiedColumnWidths->GetSize() - iColIndex) {
        pContext->m_fCurColumnWidth = 0;
        pContext->m_bCurColumnWidthAvaiable = TRUE;
        if (iColSpan == -1)
          iColSpan = pContext->m_prgSpecifiedColumnWidths->GetSize();
        for (int32_t i = 0; iColIndex + i < iColSpan; ++i) {
          pContext->m_fCurColumnWidth +=
              pContext->m_prgSpecifiedColumnWidths->GetAt(iColIndex + i);
        }
        if (pContext->m_fCurColumnWidth == 0)
          pContext->m_bCurColumnWidthAvaiable = FALSE;
        iColIndex += iColSpan >= 0 ? iColSpan : 0;
      }
    }
    pProcessor->DoLayout(FALSE, XFA_LAYOUT_FLOAT_MAX, XFA_LAYOUT_FLOAT_MAX,
                         pContext);
    if (!pProcessor->HasLayoutItem()) {
      delete pProcessor;
      continue;
    }
    FX_FLOAT fWidth, fHeight;
    pProcessor->GetCurrentComponentSize(fWidth, fHeight);
    FX_BOOL bChangeParentSize = FALSE;
    if (XFA_ItemLayoutProcessor_IsTakingSpace(m_pCurChildNode)) {
      bChangeParentSize = TRUE;
    }
    FX_FLOAT fAbsoluteX = 0, fAbsoluteY = 0;
    if (!bIgnoreXY) {
      CalculatePositionedContainerPos(m_pCurChildNode, fWidth, fHeight,
                                      fAbsoluteX, fAbsoluteY);
    }
    pProcessor->SetCurrentComponentPos(fAbsoluteX, fAbsoluteY);
    if (bContainerWidthAutoSize) {
      FX_FLOAT fChildSuppliedWidth = fAbsoluteX + fWidth;
      if (bChangeParentSize) {
        if (fContentCalculatedWidth < fChildSuppliedWidth) {
          fContentCalculatedWidth = fChildSuppliedWidth;
        }
      } else {
        if (fHiddenContentCalculatedWidth < fChildSuppliedWidth &&
            m_pCurChildNode->GetElementType() != XFA_Element::Subform) {
          fHiddenContentCalculatedWidth = fChildSuppliedWidth;
        }
      }
    }
    if (bContainerHeightAutoSize) {
      FX_FLOAT fChildSuppliedHeight = fAbsoluteY + fHeight;
      if (bChangeParentSize) {
        if (fContentCalculatedHeight < fChildSuppliedHeight) {
          fContentCalculatedHeight = fChildSuppliedHeight;
        }
      } else {
        if (fHiddenContentCalculatedHeight < fChildSuppliedHeight &&
            m_pCurChildNode->GetElementType() != XFA_Element::Subform) {
          fHiddenContentCalculatedHeight = fChildSuppliedHeight;
        }
      }
    }
    m_pLayoutItem->AddChild(pProcessor->ExtractLayoutItem());
    delete pProcessor;
  }
  XFA_VERSION eVersion = m_pFormNode->GetDocument()->GetCurVersionMode();
  if (fContentCalculatedWidth == 0 && eVersion < XFA_VERSION_207) {
    fContentCalculatedWidth = fHiddenContentCalculatedWidth;
  }
  if (fContentCalculatedHeight == 0 && eVersion < XFA_VERSION_207) {
    fContentCalculatedHeight = fHiddenContentCalculatedHeight;
  }
  XFA_ItemLayoutProcessor_CalculateContainerComponentSizeFromContentSize(
      m_pFormNode, bContainerWidthAutoSize, fContentCalculatedWidth,
      fContainerWidth, bContainerHeightAutoSize, fContentCalculatedHeight,
      fContainerHeight);
  SetCurrentComponentSize(fContainerWidth, fContainerHeight);
}
static inline void XFA_ItemLayoutProcessor_UpdateWidgetSize(
    CXFA_ContentLayoutItem* pLayoutItem,
    FX_FLOAT& fWidth,
    FX_FLOAT& fHeight) {
  CXFA_Node* pNode = pLayoutItem->m_pFormNode;
  ASSERT(pNode);
  switch (pNode->GetElementType()) {
    case XFA_Element::Subform:
    case XFA_Element::Area:
    case XFA_Element::ExclGroup:
    case XFA_Element::SubformSet: {
      if (fWidth < -XFA_LAYOUT_FLOAT_PERCISION) {
        fWidth = pLayoutItem->m_sSize.x;
      }
      if (fHeight < -XFA_LAYOUT_FLOAT_PERCISION) {
        fHeight = pLayoutItem->m_sSize.y;
      }
      break;
    }
    case XFA_Element::Draw:
    case XFA_Element::Field: {
      pNode->GetDocument()->GetNotify()->StartFieldDrawLayout(pNode, fWidth,
                                                              fHeight);
      break;
    }
    default:
      ASSERT(FALSE);
  }
}
static inline void XFA_ItemLayoutProcessor_RelocateTableRowCells(
    CXFA_ContentLayoutItem* pLayoutRow,
    const CFX_ArrayTemplate<FX_FLOAT>& rgSpecifiedColumnWidths,
    XFA_ATTRIBUTEENUM eLayout) {
  FX_FLOAT fContainerWidth = 0, fContainerHeight = 0;
  FX_BOOL bContainerWidthAutoSize = TRUE, bContainerHeightAutoSize = TRUE;
  XFA_ItemLayoutProcessor_CalculateContainerSpecfiedSize(
      pLayoutRow->m_pFormNode, fContainerWidth, fContainerHeight,
      bContainerWidthAutoSize, bContainerHeightAutoSize);
  CXFA_Node* pMarginNode =
      pLayoutRow->m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  FX_FLOAT fLeftInset = 0, fTopInset = 0, fRightInset = 0, fBottomInset = 0;
  if (pMarginNode) {
    fLeftInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset).ToUnit(XFA_UNIT_Pt);
    fTopInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset).ToUnit(XFA_UNIT_Pt);
    fRightInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_RightInset).ToUnit(XFA_UNIT_Pt);
    fBottomInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_BottomInset).ToUnit(XFA_UNIT_Pt);
  }
  FX_FLOAT fContentWidthLimit =
      bContainerWidthAutoSize ? XFA_LAYOUT_FLOAT_MAX
                              : fContainerWidth - fLeftInset - fRightInset;
  FX_FLOAT fContentCurrentHeight =
      pLayoutRow->m_sSize.y - fTopInset - fBottomInset;
  FX_FLOAT fContentCalculatedWidth = 0, fContentCalculatedHeight = 0;
  FX_FLOAT fCurrentColX = 0;
  int32_t nCurrentColIdx = 0;
  FX_BOOL bMetWholeRowCell = FALSE;
  for (CXFA_ContentLayoutItem* pLayoutChild =
           (CXFA_ContentLayoutItem*)pLayoutRow->m_pFirstChild;
       pLayoutChild;
       pLayoutChild = (CXFA_ContentLayoutItem*)pLayoutChild->m_pNextSibling) {
    int32_t nOriginalColSpan =
        pLayoutChild->m_pFormNode->GetInteger(XFA_ATTRIBUTE_ColSpan);
    int32_t nColSpan = nOriginalColSpan;
    FX_FLOAT fColSpanWidth = 0;
    if (nColSpan == -1 ||
        nCurrentColIdx + nColSpan > rgSpecifiedColumnWidths.GetSize()) {
      nColSpan = rgSpecifiedColumnWidths.GetSize() - nCurrentColIdx;
    }
    for (int32_t i = 0; i < nColSpan; i++) {
      fColSpanWidth += rgSpecifiedColumnWidths[nCurrentColIdx + i];
    }
    if (nColSpan != nOriginalColSpan) {
      fColSpanWidth = bMetWholeRowCell ? 0 : std::max(fColSpanWidth,
                                                      pLayoutChild->m_sSize.y);
    }
    if (nOriginalColSpan == -1) {
      bMetWholeRowCell = TRUE;
    }
    pLayoutChild->m_sPos = CFX_PointF(fCurrentColX, 0);
    pLayoutChild->m_sSize.x = fColSpanWidth;
    if (XFA_ItemLayoutProcessor_IsTakingSpace(pLayoutChild->m_pFormNode)) {
      fCurrentColX += fColSpanWidth;
      nCurrentColIdx += nColSpan;
      FX_FLOAT fNewHeight =
          bContainerHeightAutoSize ? -1 : fContentCurrentHeight;
      XFA_ItemLayoutProcessor_UpdateWidgetSize(pLayoutChild, fColSpanWidth,
                                               fNewHeight);
      pLayoutChild->m_sSize.y = fNewHeight;
      if (bContainerHeightAutoSize) {
        FX_FLOAT fChildSuppliedHeight = pLayoutChild->m_sSize.y;
        if (fContentCalculatedHeight < fChildSuppliedHeight) {
          fContentCalculatedHeight = fChildSuppliedHeight;
        }
      }
    }
  }
  if (bContainerHeightAutoSize) {
    for (CXFA_ContentLayoutItem* pLayoutChild =
             (CXFA_ContentLayoutItem*)pLayoutRow->m_pFirstChild;
         pLayoutChild;
         pLayoutChild = (CXFA_ContentLayoutItem*)pLayoutChild->m_pNextSibling) {
      XFA_ItemLayoutProcessor_UpdateWidgetSize(
          pLayoutChild, pLayoutChild->m_sSize.x, fContentCalculatedHeight);
      FX_FLOAT fOldChildHeight = pLayoutChild->m_sSize.y;
      pLayoutChild->m_sSize.y = fContentCalculatedHeight;
      CXFA_Node* pParaNode =
          pLayoutChild->m_pFormNode->GetFirstChildByClass(XFA_Element::Para);
      if (pParaNode && pLayoutChild->m_pFirstChild) {
        FX_FLOAT fOffHeight = fContentCalculatedHeight - fOldChildHeight;
        XFA_ATTRIBUTEENUM eVType = pParaNode->GetEnum(XFA_ATTRIBUTE_VAlign);
        switch (eVType) {
          case XFA_ATTRIBUTEENUM_Middle:
            fOffHeight = fOffHeight / 2;
            break;
          case XFA_ATTRIBUTEENUM_Bottom:
            break;
          case XFA_ATTRIBUTEENUM_Top:
          default:
            fOffHeight = 0;
            break;
        }
        if (fOffHeight > 0) {
          for (CXFA_ContentLayoutItem* pInnerLayoutChild =
                   (CXFA_ContentLayoutItem*)pLayoutChild->m_pFirstChild;
               pInnerLayoutChild;
               pInnerLayoutChild =
                   (CXFA_ContentLayoutItem*)pInnerLayoutChild->m_pNextSibling) {
            pInnerLayoutChild->m_sPos.y += fOffHeight;
          }
        }
      }
    }
  }
  if (bContainerWidthAutoSize) {
    FX_FLOAT fChildSuppliedWidth = fCurrentColX;
    if (fContentWidthLimit < XFA_LAYOUT_FLOAT_MAX &&
        fContentWidthLimit > fChildSuppliedWidth) {
      fChildSuppliedWidth = fContentWidthLimit;
    }
    if (fContentCalculatedWidth < fChildSuppliedWidth) {
      fContentCalculatedWidth = fChildSuppliedWidth;
    }
  } else {
    fContentCalculatedWidth = fContainerWidth - fLeftInset - fRightInset;
  }
  if (pLayoutRow->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout) ==
      XFA_ATTRIBUTEENUM_Rl_row) {
    for (CXFA_ContentLayoutItem* pLayoutChild =
             (CXFA_ContentLayoutItem*)pLayoutRow->m_pFirstChild;
         pLayoutChild;
         pLayoutChild = (CXFA_ContentLayoutItem*)pLayoutChild->m_pNextSibling) {
      pLayoutChild->m_sPos.x = fContentCalculatedWidth -
                               pLayoutChild->m_sPos.x - pLayoutChild->m_sSize.x;
    }
  }
  XFA_ItemLayoutProcessor_CalculateContainerComponentSizeFromContentSize(
      pLayoutRow->m_pFormNode, bContainerWidthAutoSize, fContentCalculatedWidth,
      fContainerWidth, bContainerHeightAutoSize, fContentCalculatedHeight,
      fContainerHeight);
  pLayoutRow->m_sSize = CFX_SizeF(fContainerWidth, fContainerHeight);
}
void CXFA_ItemLayoutProcessor::DoLayoutTableContainer(CXFA_Node* pLayoutNode) {
  if (m_pLayoutItem)
    return;

  if (!pLayoutNode) {
    pLayoutNode = m_pFormNode;
  }
  ASSERT(m_pCurChildNode == XFA_LAYOUT_INVALIDNODE);
  m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
  FX_FLOAT fContainerWidth = 0, fContainerHeight = 0;
  FX_BOOL bContainerWidthAutoSize = TRUE, bContainerHeightAutoSize = TRUE;
  XFA_ItemLayoutProcessor_CalculateContainerSpecfiedSize(
      m_pFormNode, fContainerWidth, fContainerHeight, bContainerWidthAutoSize,
      bContainerHeightAutoSize);
  FX_FLOAT fContentCalculatedWidth = 0, fContentCalculatedHeight = 0;
  CXFA_Node* pMarginNode =
      m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  FX_FLOAT fLeftInset = 0;
  FX_FLOAT fRightInset = 0;
  if (pMarginNode) {
    fLeftInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset).ToUnit(XFA_UNIT_Pt);
    fRightInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_RightInset).ToUnit(XFA_UNIT_Pt);
  }
  FX_FLOAT fContentWidthLimit =
      bContainerWidthAutoSize ? XFA_LAYOUT_FLOAT_MAX
                              : fContainerWidth - fLeftInset - fRightInset;
  CFX_WideStringC wsColumnWidths;
  if (pLayoutNode->TryCData(XFA_ATTRIBUTE_ColumnWidths, wsColumnWidths)) {
    CFX_WideStringArray widths;
    if (SeparateStringW(wsColumnWidths.c_str(), wsColumnWidths.GetLength(),
                        L' ', widths) > 0) {
      int32_t iCols = widths.GetSize();
      CFX_WideString wsWidth;
      for (int32_t i = 0; i < iCols; i++) {
        wsWidth = widths[i];
        wsWidth.TrimLeft(L' ');
        if (!wsWidth.IsEmpty()) {
          CXFA_Measurement measure(wsWidth.AsStringC());
          m_rgSpecifiedColumnWidths.Add(measure.ToUnit(XFA_UNIT_Pt));
        }
      }
    }
  }
  int32_t iSpecifiedColumnCount = m_rgSpecifiedColumnWidths.GetSize();
  CXFA_LayoutContext layoutContext;
  layoutContext.m_prgSpecifiedColumnWidths = &m_rgSpecifiedColumnWidths;
  CXFA_LayoutContext* pLayoutContext =
      iSpecifiedColumnCount > 0 ? &layoutContext : nullptr;
  if (m_pCurChildNode == XFA_LAYOUT_INVALIDNODE) {
    XFA_ItemLayoutProcessor_GotoNextContainerNode(
        m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, FALSE);
  }
  for (; m_pCurChildNode; XFA_ItemLayoutProcessor_GotoNextContainerNode(
           m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, FALSE)) {
    layoutContext.m_bCurColumnWidthAvaiable = FALSE;
    layoutContext.m_fCurColumnWidth = 0;
    if (m_nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Container) {
      continue;
    }
    CXFA_ItemLayoutProcessor* pProcessor =
        new CXFA_ItemLayoutProcessor(m_pCurChildNode, m_pPageMgr);
    pProcessor->DoLayout(FALSE, XFA_LAYOUT_FLOAT_MAX, XFA_LAYOUT_FLOAT_MAX,
                         pLayoutContext);
    if (!pProcessor->HasLayoutItem()) {
      delete pProcessor;
      continue;
    }
    m_pLayoutItem->AddChild(pProcessor->ExtractLayoutItem());
    delete pProcessor;
  }
  int32_t iRowCount = 0, iColCount = 0;
  {
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*> rgRowItems;
    CFX_ArrayTemplate<int32_t> rgRowItemsSpan;
    CFX_ArrayTemplate<FX_FLOAT> rgRowItemsWidth;
    for (CXFA_ContentLayoutItem* pLayoutChild =
             (CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild;
         pLayoutChild;
         pLayoutChild = (CXFA_ContentLayoutItem*)pLayoutChild->m_pNextSibling) {
      if (pLayoutChild->m_pFormNode->GetElementType() != XFA_Element::Subform) {
        continue;
      }
      if (!XFA_ItemLayoutProcessor_IsTakingSpace(pLayoutChild->m_pFormNode)) {
        continue;
      }
      XFA_ATTRIBUTEENUM eLayout =
          pLayoutChild->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
      if (eLayout != XFA_ATTRIBUTEENUM_Row &&
          eLayout != XFA_ATTRIBUTEENUM_Rl_row) {
        continue;
      }
      if (CXFA_ContentLayoutItem* pRowLayoutCell =
              (CXFA_ContentLayoutItem*)pLayoutChild->m_pFirstChild) {
        rgRowItems.Add(pRowLayoutCell);
        int32_t iColSpan =
            pRowLayoutCell->m_pFormNode->GetInteger(XFA_ATTRIBUTE_ColSpan);
        rgRowItemsSpan.Add(iColSpan);
        rgRowItemsWidth.Add(pRowLayoutCell->m_sSize.x);
      }
    }
    iRowCount = rgRowItems.GetSize();
    iColCount = 0;
    FX_BOOL bMoreColumns = TRUE;
    while (bMoreColumns) {
      bMoreColumns = FALSE;
      FX_BOOL bAutoCol = FALSE;
      for (int32_t i = 0; i < iRowCount; i++) {
        while (rgRowItems[i] && (rgRowItemsSpan[i] <= 0 ||
                                 !XFA_ItemLayoutProcessor_IsTakingSpace(
                                     rgRowItems[i]->m_pFormNode))) {
          CXFA_ContentLayoutItem* pNewCell =
              (CXFA_ContentLayoutItem*)rgRowItems[i]->m_pNextSibling;
          if (rgRowItemsSpan[i] < 0 && XFA_ItemLayoutProcessor_IsTakingSpace(
                                           rgRowItems[i]->m_pFormNode)) {
            pNewCell = nullptr;
          }
          rgRowItems[i] = pNewCell;
          rgRowItemsSpan[i] =
              pNewCell
                  ? pNewCell->m_pFormNode->GetInteger(XFA_ATTRIBUTE_ColSpan)
                  : 0;
          rgRowItemsWidth[i] = pNewCell ? pNewCell->m_sSize.x : 0;
        }
        CXFA_ContentLayoutItem* pCell = rgRowItems[i];
        if (!pCell) {
          continue;
        }
        bMoreColumns = TRUE;
        if (rgRowItemsSpan[i] == 1) {
          if (iColCount >= iSpecifiedColumnCount) {
            for (int32_t j = 0, c = iColCount + 1 -
                                    m_rgSpecifiedColumnWidths.GetSize();
                 j < c; j++) {
              m_rgSpecifiedColumnWidths.Add(0);
            }
          }
          if (m_rgSpecifiedColumnWidths[iColCount] <
              XFA_LAYOUT_FLOAT_PERCISION) {
            bAutoCol = TRUE;
          }
          if (bAutoCol &&
              m_rgSpecifiedColumnWidths[iColCount] < rgRowItemsWidth[i]) {
            m_rgSpecifiedColumnWidths[iColCount] = rgRowItemsWidth[i];
          }
        }
      }
      if (bMoreColumns) {
        FX_FLOAT fFinalColumnWidth = 0.0f;
        if (iColCount < m_rgSpecifiedColumnWidths.GetSize())
          fFinalColumnWidth = m_rgSpecifiedColumnWidths[iColCount];
        for (int32_t i = 0; i < iRowCount; ++i) {
          if (!rgRowItems[i])
            continue;
          --rgRowItemsSpan[i];
          rgRowItemsWidth[i] -= fFinalColumnWidth;
        }
        ++iColCount;
      }
    }
  }
  FX_FLOAT fCurrentRowY = 0;
  for (CXFA_ContentLayoutItem* pLayoutChild =
           (CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild;
       pLayoutChild;
       pLayoutChild = (CXFA_ContentLayoutItem*)pLayoutChild->m_pNextSibling) {
    if (!XFA_ItemLayoutProcessor_IsTakingSpace(pLayoutChild->m_pFormNode)) {
      continue;
    }
    if (pLayoutChild->m_pFormNode->GetElementType() == XFA_Element::Subform) {
      XFA_ATTRIBUTEENUM eSubformLayout =
          pLayoutChild->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
      if (eSubformLayout == XFA_ATTRIBUTEENUM_Row ||
          eSubformLayout == XFA_ATTRIBUTEENUM_Rl_row) {
        XFA_ItemLayoutProcessor_RelocateTableRowCells(
            pLayoutChild, m_rgSpecifiedColumnWidths, eSubformLayout);
      }
    }
    pLayoutChild->m_sPos.y = fCurrentRowY;
    if (bContainerWidthAutoSize) {
      pLayoutChild->m_sPos.x = 0;
    } else {
      switch (pLayoutChild->m_pFormNode->GetEnum(XFA_ATTRIBUTE_HAlign)) {
        case XFA_ATTRIBUTEENUM_Left:
        default:
          pLayoutChild->m_sPos.x = 0;
          break;
        case XFA_ATTRIBUTEENUM_Center:
          pLayoutChild->m_sPos.x =
              (fContentWidthLimit - pLayoutChild->m_sSize.x) / 2;
          break;
        case XFA_ATTRIBUTEENUM_Right:
          pLayoutChild->m_sPos.x = fContentWidthLimit - pLayoutChild->m_sSize.x;
          break;
      }
    }
    if (bContainerWidthAutoSize) {
      FX_FLOAT fChildSuppliedWidth =
          pLayoutChild->m_sPos.x + pLayoutChild->m_sSize.x;
      if (fContentWidthLimit < XFA_LAYOUT_FLOAT_MAX &&
          fContentWidthLimit > fChildSuppliedWidth) {
        fChildSuppliedWidth = fContentWidthLimit;
      }
      if (fContentCalculatedWidth < fChildSuppliedWidth) {
        fContentCalculatedWidth = fChildSuppliedWidth;
      }
    }
    fCurrentRowY += pLayoutChild->m_sSize.y;
  }
  if (bContainerHeightAutoSize) {
    FX_FLOAT fChildSuppliedHeight = fCurrentRowY;
    if (fContentCalculatedHeight < fChildSuppliedHeight) {
      fContentCalculatedHeight = fChildSuppliedHeight;
    }
  }
  XFA_ItemLayoutProcessor_CalculateContainerComponentSizeFromContentSize(
      m_pFormNode, bContainerWidthAutoSize, fContentCalculatedWidth,
      fContainerWidth, bContainerHeightAutoSize, fContentCalculatedHeight,
      fContainerHeight);
  SetCurrentComponentSize(fContainerWidth, fContainerHeight);
}
static uint8_t XFA_ItemLayoutProcessor_HAlignEnumToInt(
    XFA_ATTRIBUTEENUM eHAlign) {
  switch (eHAlign) {
    case XFA_ATTRIBUTEENUM_Center:
      return 1;
    case XFA_ATTRIBUTEENUM_Right:
      return 2;
    case XFA_ATTRIBUTEENUM_Left:
    default:
      return 0;
  }
}
static void XFA_ItemLayoutProcessor_UpdatePendedItemLayout(
    CXFA_ItemLayoutProcessor* pProcessor,
    CXFA_ContentLayoutItem* pLayoutItem) {
  XFA_ATTRIBUTEENUM eLayout =
      pLayoutItem->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
  switch (eLayout) {
    case XFA_ATTRIBUTEENUM_Row:
    case XFA_ATTRIBUTEENUM_Rl_row:
      XFA_ItemLayoutProcessor_RelocateTableRowCells(
          pLayoutItem, pProcessor->m_rgSpecifiedColumnWidths, eLayout);
      break;
    default:
      break;
  }
}
FX_BOOL CXFA_ItemLayoutProcessor::IsAddNewRowForTrailer(
    CXFA_ContentLayoutItem* pTrailerItem) {
  if (!pTrailerItem) {
    return FALSE;
  }
  FX_FLOAT fWidth = pTrailerItem->m_sSize.x;
  XFA_ATTRIBUTEENUM eLayout = m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
  if (eLayout != XFA_ATTRIBUTEENUM_Tb && m_fWidthLimite > fWidth) {
    return FALSE;
  }
  return TRUE;
}
static void XFA_ItemLayoutProcessor_AddTrailerBeforeSplit(
    CXFA_ItemLayoutProcessor* pProcessor,
    FX_FLOAT fSplitPos,
    CXFA_ContentLayoutItem* pTrailerLayoutItem,
    FX_BOOL bUseInherited = FALSE) {
  if (!pTrailerLayoutItem) {
    return;
  }
  FX_FLOAT fHeight = pTrailerLayoutItem->m_sSize.y;
  if (bUseInherited) {
    FX_FLOAT fNewSplitPos = 0;
    if (fSplitPos - fHeight > XFA_LAYOUT_FLOAT_PERCISION) {
      fNewSplitPos = pProcessor->FindSplitPos(fSplitPos - fHeight);
    }
    if (fNewSplitPos > XFA_LAYOUT_FLOAT_PERCISION) {
      pProcessor->SplitLayoutItem(fNewSplitPos);
    }
    return;
  }
  XFA_ItemLayoutProcessor_UpdatePendedItemLayout(pProcessor,
                                                 pTrailerLayoutItem);
  CXFA_Node* pMarginNode =
      pProcessor->m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  FX_FLOAT fLeftInset = 0, fTopInset = 0, fRightInset = 0, fBottomInset = 0;
  if (pMarginNode) {
    fLeftInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset).ToUnit(XFA_UNIT_Pt);
    fTopInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset).ToUnit(XFA_UNIT_Pt);
    fRightInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_RightInset).ToUnit(XFA_UNIT_Pt);
    fBottomInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_BottomInset).ToUnit(XFA_UNIT_Pt);
  }
  if (!pProcessor->IsAddNewRowForTrailer(pTrailerLayoutItem)) {
    pTrailerLayoutItem->m_sPos.y = pProcessor->m_fLastRowY;
    pTrailerLayoutItem->m_sPos.x = pProcessor->m_fLastRowWidth;
    pProcessor->m_pLayoutItem->m_sSize.x += pTrailerLayoutItem->m_sSize.x;
    pProcessor->m_pLayoutItem->AddChild(pTrailerLayoutItem);
    return;
  }
  FX_FLOAT fNewSplitPos = 0;
  if (fSplitPos - fHeight > XFA_LAYOUT_FLOAT_PERCISION) {
    fNewSplitPos = pProcessor->FindSplitPos(fSplitPos - fHeight);
  }
  if (fNewSplitPos > XFA_LAYOUT_FLOAT_PERCISION) {
    pProcessor->SplitLayoutItem(fNewSplitPos);
    pTrailerLayoutItem->m_sPos.y = fNewSplitPos - fTopInset - fBottomInset;
  } else {
    pTrailerLayoutItem->m_sPos.y = fSplitPos - fTopInset - fBottomInset;
  }
  switch (pTrailerLayoutItem->m_pFormNode->GetEnum(XFA_ATTRIBUTE_HAlign)) {
    case XFA_ATTRIBUTEENUM_Left:
    default:
      pTrailerLayoutItem->m_sPos.x = fLeftInset;
      break;
    case XFA_ATTRIBUTEENUM_Right:
      pTrailerLayoutItem->m_sPos.x = pProcessor->m_pLayoutItem->m_sSize.x -
                                     fRightInset -
                                     pTrailerLayoutItem->m_sSize.x;
      break;
    case XFA_ATTRIBUTEENUM_Center:
      pTrailerLayoutItem->m_sPos.x =
          (pProcessor->m_pLayoutItem->m_sSize.x - fLeftInset - fRightInset -
           pTrailerLayoutItem->m_sSize.x) /
          2;
      break;
  }
  pProcessor->m_pLayoutItem->m_sSize.y += fHeight;
  pProcessor->m_pLayoutItem->AddChild(pTrailerLayoutItem);
}
static void XFA_ItemLayoutProcessor_AddLeaderAfterSplit(
    CXFA_ItemLayoutProcessor* pProcessor,
    CXFA_ContentLayoutItem* pLeaderLayoutItem) {
  XFA_ItemLayoutProcessor_UpdatePendedItemLayout(pProcessor, pLeaderLayoutItem);
  CXFA_Node* pMarginNode =
      pProcessor->m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  FX_FLOAT fLeftInset = 0;
  FX_FLOAT fRightInset = 0;
  if (pMarginNode) {
    fLeftInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset).ToUnit(XFA_UNIT_Pt);
    fRightInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_RightInset).ToUnit(XFA_UNIT_Pt);
  }
  FX_FLOAT fHeight = pLeaderLayoutItem->m_sSize.y;
  for (CXFA_ContentLayoutItem* pChildItem =
           (CXFA_ContentLayoutItem*)pProcessor->m_pLayoutItem->m_pFirstChild;
       pChildItem;
       pChildItem = (CXFA_ContentLayoutItem*)pChildItem->m_pNextSibling) {
    pChildItem->m_sPos.y += fHeight;
  }
  pLeaderLayoutItem->m_sPos.y = 0;
  switch (pLeaderLayoutItem->m_pFormNode->GetEnum(XFA_ATTRIBUTE_HAlign)) {
    case XFA_ATTRIBUTEENUM_Left:
    default:
      pLeaderLayoutItem->m_sPos.x = fLeftInset;
      break;
    case XFA_ATTRIBUTEENUM_Right:
      pLeaderLayoutItem->m_sPos.x = pProcessor->m_pLayoutItem->m_sSize.x -
                                    fRightInset - pLeaderLayoutItem->m_sSize.x;
      break;
    case XFA_ATTRIBUTEENUM_Center:
      pLeaderLayoutItem->m_sPos.x =
          (pProcessor->m_pLayoutItem->m_sSize.x - fLeftInset - fRightInset -
           pLeaderLayoutItem->m_sSize.x) /
          2;
      break;
  }
  pProcessor->m_pLayoutItem->m_sSize.y += fHeight;
  pProcessor->m_pLayoutItem->AddChild(pLeaderLayoutItem);
}
static void XFA_ItemLayoutProcessor_AddPendingNode(
    CXFA_ItemLayoutProcessor* pProcessor,
    CXFA_Node* pPendingNode,
    FX_BOOL bBreakPending) {
  pProcessor->m_PendingNodes.push_back(pPendingNode);
  pProcessor->m_bBreakPending = bBreakPending;
}
static FX_FLOAT XFA_ItemLayoutProcessor_InsertPendingItems(
    CXFA_ItemLayoutProcessor* pProcessor,
    CXFA_Node* pCurChildNode) {
  FX_FLOAT fTotalHeight = 0;
  if (pProcessor->m_PendingNodes.empty()) {
    return fTotalHeight;
  }
  if (!pProcessor->m_pLayoutItem) {
    pProcessor->m_pLayoutItem =
        pProcessor->CreateContentLayoutItem(pCurChildNode);
    pProcessor->m_pLayoutItem->m_sSize.clear();
  }
  while (!pProcessor->m_PendingNodes.empty()) {
    std::unique_ptr<CXFA_ItemLayoutProcessor> pPendingProcessor(
        new CXFA_ItemLayoutProcessor(pProcessor->m_PendingNodes.front(),
                                     nullptr));
    pProcessor->m_PendingNodes.pop_front();
    pPendingProcessor->DoLayout(FALSE, XFA_LAYOUT_FLOAT_MAX);
    CXFA_ContentLayoutItem* pPendingLayoutItem =
        pPendingProcessor->HasLayoutItem()
            ? pPendingProcessor->ExtractLayoutItem()
            : nullptr;
    if (pPendingLayoutItem) {
      XFA_ItemLayoutProcessor_AddLeaderAfterSplit(pProcessor,
                                                  pPendingLayoutItem);
      if (pProcessor->m_bBreakPending) {
        fTotalHeight += pPendingLayoutItem->m_sSize.y;
      }
    }
  }
  return fTotalHeight;
}
FX_FLOAT CXFA_ItemLayoutProcessor::InsertKeepLayoutItems() {
  FX_FLOAT fTotalHeight = 0;
  if (m_arrayKeepItems.GetSize()) {
    if (!m_pLayoutItem) {
      m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
      m_pLayoutItem->m_sSize.clear();
    }
    for (int32_t iIndex = m_arrayKeepItems.GetSize() - 1; iIndex >= 0;
         iIndex--) {
      XFA_ItemLayoutProcessor_AddLeaderAfterSplit(this,
                                                  m_arrayKeepItems[iIndex]);
      fTotalHeight += m_arrayKeepItems[iIndex]->m_sSize.y;
    }
    m_arrayKeepItems.RemoveAll();
  }
  return fTotalHeight;
}
FX_BOOL CXFA_ItemLayoutProcessor::ProcessKeepForSplite(
    CXFA_ItemLayoutProcessor* pParentProcessor,
    CXFA_ItemLayoutProcessor* pChildProcessor,
    XFA_ItemLayoutProcessorResult eRetValue,
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*>& rgCurLineLayoutItem,
    FX_FLOAT& fContentCurRowAvailWidth,
    FX_FLOAT& fContentCurRowHeight,
    FX_FLOAT& fContentCurRowY,
    FX_BOOL& bAddedItemInRow,
    FX_BOOL& bForceEndPage,
    XFA_ItemLayoutProcessorResult& result) {
  if (!pParentProcessor || !pChildProcessor) {
    return FALSE;
  }
  if (pParentProcessor->m_pCurChildNode->GetIntact() !=
          XFA_ATTRIBUTEENUM_None ||
      !pChildProcessor->m_bHasAvailHeight) {
    if (XFA_ExistContainerKeep(pParentProcessor->m_pCurChildNode, TRUE)) {
      FX_FLOAT fChildWidth, fChildHeight;
      pChildProcessor->GetCurrentComponentSize(fChildWidth, fChildHeight);
      CFX_ArrayTemplate<CXFA_ContentLayoutItem*> keepLayoutItems;
      if (pParentProcessor->JudgePutNextPage(pParentProcessor->m_pLayoutItem,
                                             fChildHeight, keepLayoutItems)) {
        m_arrayKeepItems.RemoveAll();
        for (int32_t iIndex = 0; iIndex < keepLayoutItems.GetSize(); iIndex++) {
          CXFA_ContentLayoutItem* pItem = keepLayoutItems.GetAt(iIndex);
          pParentProcessor->m_pLayoutItem->RemoveChild(pItem);
          fContentCurRowY -= pItem->m_sSize.y;
          m_arrayKeepItems.Add(pItem);
        }
        bAddedItemInRow = TRUE;
        bForceEndPage = TRUE;
        result = XFA_ItemLayoutProcessorResult_PageFullBreak;
        return TRUE;
      }
      rgCurLineLayoutItem.Add(pChildProcessor->ExtractLayoutItem());
      bAddedItemInRow = TRUE;
      fContentCurRowAvailWidth -= fChildWidth;
      if (fContentCurRowHeight < fChildHeight) {
        fContentCurRowHeight = fChildHeight;
      }
      result = eRetValue;
      return TRUE;
    }
  }
  return FALSE;
}
FX_BOOL CXFA_ItemLayoutProcessor::JudgePutNextPage(
    CXFA_ContentLayoutItem* pParentLayoutItem,
    FX_FLOAT fChildHeight,
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*>& pKeepItems) {
  if (!pParentLayoutItem) {
    return FALSE;
  }
  FX_FLOAT fItemsHeight = 0;
  for (CXFA_ContentLayoutItem* pChildLayoutItem =
           (CXFA_ContentLayoutItem*)pParentLayoutItem->m_pFirstChild;
       pChildLayoutItem;
       pChildLayoutItem =
           (CXFA_ContentLayoutItem*)pChildLayoutItem->m_pNextSibling) {
    if (XFA_ExistContainerKeep(pChildLayoutItem->m_pFormNode, FALSE)) {
      pKeepItems.Add(pChildLayoutItem);
      fItemsHeight += pChildLayoutItem->m_sSize.y;
    } else {
      pKeepItems.RemoveAll();
      fItemsHeight = 0;
    }
  }
  fItemsHeight += fChildHeight;
  if (m_pPageMgr->GetNextAvailContentHeight(fItemsHeight)) {
    return TRUE;
  }
  return FALSE;
}
void CXFA_ItemLayoutProcessor::ProcessUnUseBinds(CXFA_Node* pFormNode) {
  if (!pFormNode) {
    return;
  }
  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFANode> sIterator(
      pFormNode);
  for (CXFA_Node* pNode = sIterator.MoveToNext(); pNode;
       pNode = sIterator.MoveToNext()) {
    if (pNode->IsContainerNode()) {
      CXFA_Node* pBindNode = pNode->GetBindData();
      if (pBindNode) {
        pBindNode->RemoveBindItem(pNode);
        pNode->SetObject(XFA_ATTRIBUTE_BindingNode, nullptr);
      }
    }
    pNode->SetFlag(XFA_NodeFlag_UnusedNode, true);
  }
}
void CXFA_ItemLayoutProcessor::ProcessUnUseOverFlow(
    CXFA_Node* pLeaderNode,
    CXFA_Node* pTrailerNode,
    CXFA_ContentLayoutItem* pTrailerItem,
    CXFA_Node* pFormNode) {
  ProcessUnUseBinds(pLeaderNode);
  ProcessUnUseBinds(pTrailerNode);
  if (!pFormNode) {
    return;
  }
  if (pFormNode->GetElementType() == XFA_Element::Overflow ||
      pFormNode->GetElementType() == XFA_Element::Break) {
    pFormNode = pFormNode->GetNodeItem(XFA_NODEITEM_Parent);
  }
  if (pLeaderNode && pFormNode) {
    pFormNode->RemoveChild(pLeaderNode);
  }
  if (pTrailerNode && pFormNode) {
    pFormNode->RemoveChild(pTrailerNode);
  }
  if (pTrailerItem) {
    XFA_ReleaseLayoutItem(pTrailerItem);
  }
}
static XFA_ItemLayoutProcessorResult XFA_ItemLayoutProcessor_InsertFlowedItem(
    CXFA_ItemLayoutProcessor* pThis,
    CXFA_ItemLayoutProcessor* pProcessor,
    FX_BOOL bContainerWidthAutoSize,
    FX_BOOL bContainerHeightAutoSize,
    FX_FLOAT fContainerHeight,
    XFA_ATTRIBUTEENUM eFlowStrategy,
    uint8_t& uCurHAlignState,
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*> (&rgCurLineLayoutItems)[3],
    FX_BOOL bUseBreakControl,
    FX_FLOAT fAvailHeight,
    FX_FLOAT fRealHeight,
    FX_FLOAT& fContentCurRowY,
    FX_FLOAT& fContentWidthLimit,
    FX_FLOAT& fContentCurRowAvailWidth,
    FX_FLOAT& fContentCurRowHeight,
    FX_BOOL& bAddedItemInRow,
    FX_BOOL& bForceEndPage,
    CXFA_LayoutContext* pLayoutContext = nullptr,
    FX_BOOL bNewRow = FALSE) {
  FX_BOOL bTakeSpace =
      XFA_ItemLayoutProcessor_IsTakingSpace(pProcessor->m_pFormNode);
  uint8_t uHAlign = XFA_ItemLayoutProcessor_HAlignEnumToInt(
      pThis->m_pCurChildNode->GetEnum(XFA_ATTRIBUTE_HAlign));
  if (bContainerWidthAutoSize) {
    uHAlign = 0;
  }
  if ((eFlowStrategy != XFA_ATTRIBUTEENUM_Rl_tb && uHAlign < uCurHAlignState) ||
      (eFlowStrategy == XFA_ATTRIBUTEENUM_Rl_tb && uHAlign > uCurHAlignState)) {
    return XFA_ItemLayoutProcessorResult_RowFullBreak;
  }
  uCurHAlignState = uHAlign;
  FX_BOOL bIsOwnSplite =
      pProcessor->m_pFormNode->GetIntact() == XFA_ATTRIBUTEENUM_None;
  FX_BOOL bUseRealHeight =
      bTakeSpace && bContainerHeightAutoSize && bIsOwnSplite &&
      pProcessor->m_pFormNode->GetNodeItem(XFA_NODEITEM_Parent)->GetIntact() ==
          XFA_ATTRIBUTEENUM_None;
  FX_BOOL bIsTransHeight = bTakeSpace;
  if (bIsTransHeight && !bIsOwnSplite) {
    FX_BOOL bRootForceTb = FALSE;
    XFA_ATTRIBUTEENUM eLayoutStrategy = XFA_ItemLayoutProcessor_GetLayout(
        pProcessor->m_pFormNode, bRootForceTb);
    if (eLayoutStrategy == XFA_ATTRIBUTEENUM_Lr_tb ||
        eLayoutStrategy == XFA_ATTRIBUTEENUM_Rl_tb) {
      bIsTransHeight = FALSE;
    }
  }
  FX_BOOL bUseInherited = FALSE;
  CXFA_LayoutContext layoutContext;
  if (pThis->m_pPageMgr) {
    CXFA_Node* pOverflowNode =
        pThis->m_pPageMgr->QueryOverflow(pThis->m_pFormNode);
    if (pOverflowNode) {
      layoutContext.m_pOverflowNode = pOverflowNode;
      layoutContext.m_pOverflowProcessor = pThis;
      pLayoutContext = &layoutContext;
    }
  }
  XFA_ItemLayoutProcessorResult eRetValue = XFA_ItemLayoutProcessorResult_Done;
  if (!bNewRow ||
      pProcessor->m_ePreProcessRs == XFA_ItemLayoutProcessorResult_Done) {
    eRetValue = pProcessor->DoLayout(
        bTakeSpace ? bUseBreakControl : FALSE,
        bUseRealHeight ? fRealHeight - fContentCurRowY : XFA_LAYOUT_FLOAT_MAX,
        bIsTransHeight ? fRealHeight - fContentCurRowY : XFA_LAYOUT_FLOAT_MAX,
        pLayoutContext);
    pProcessor->m_ePreProcessRs = eRetValue;
  } else {
    eRetValue = pProcessor->m_ePreProcessRs;
    pProcessor->m_ePreProcessRs = XFA_ItemLayoutProcessorResult_Done;
  }
  if (pProcessor->HasLayoutItem() == FALSE) {
    return eRetValue;
  }
  FX_FLOAT fChildWidth, fChildHeight;
  pProcessor->GetCurrentComponentSize(fChildWidth, fChildHeight);
  if (bUseRealHeight && fRealHeight < XFA_LAYOUT_FLOAT_PERCISION) {
    fRealHeight = XFA_LAYOUT_FLOAT_MAX;
    fAvailHeight = XFA_LAYOUT_FLOAT_MAX;
  }
  if (!bTakeSpace ||
      (fChildWidth <= fContentCurRowAvailWidth + XFA_LAYOUT_FLOAT_PERCISION) ||
      (fContentWidthLimit - fContentCurRowAvailWidth <=
       XFA_LAYOUT_FLOAT_PERCISION)) {
    CXFA_Node* pOverflowLeaderNode = nullptr;
    CXFA_Node* pOverflowTrailerNode = nullptr;
    CXFA_Node* pFormNode = nullptr;
    CXFA_ContentLayoutItem* pTrailerLayoutItem = nullptr;
    FX_BOOL bIsAddTrailerHeight = FALSE;
    if (pThis->m_pPageMgr &&
        pProcessor->m_pFormNode->GetIntact() == XFA_ATTRIBUTEENUM_None) {
      pFormNode = pThis->m_pPageMgr->QueryOverflow(pProcessor->m_pFormNode);
      if (!pFormNode && pLayoutContext &&
          pLayoutContext->m_pOverflowProcessor) {
        pFormNode = pLayoutContext->m_pOverflowNode;
        bUseInherited = TRUE;
      }
      if (pThis->m_pPageMgr->ProcessOverflow(pFormNode, pOverflowLeaderNode,
                                             pOverflowTrailerNode, FALSE,
                                             FALSE)) {
        if (pProcessor->JudgeLeaderOrTrailerForOccur(pOverflowTrailerNode)) {
          if (pOverflowTrailerNode) {
            CXFA_ItemLayoutProcessor* pOverflowLeaderProcessor =
                new CXFA_ItemLayoutProcessor(pOverflowTrailerNode, nullptr);
            pOverflowLeaderProcessor->DoLayout(FALSE, XFA_LAYOUT_FLOAT_MAX);
            pTrailerLayoutItem =
                pOverflowLeaderProcessor->HasLayoutItem()
                    ? pOverflowLeaderProcessor->ExtractLayoutItem()
                    : nullptr;
            delete pOverflowLeaderProcessor;
          }
          if (bUseInherited) {
            bIsAddTrailerHeight =
                pThis->IsAddNewRowForTrailer(pTrailerLayoutItem);
          } else {
            bIsAddTrailerHeight =
                pProcessor->IsAddNewRowForTrailer(pTrailerLayoutItem);
          }
          if (bIsAddTrailerHeight) {
            FX_FLOAT fTrailerHeight = pTrailerLayoutItem->m_sSize.y;
            fChildHeight += fTrailerHeight;
            bIsAddTrailerHeight = TRUE;
          }
        }
      }
    }
    if (!bTakeSpace ||
        fContentCurRowY + fChildHeight <=
            fAvailHeight + XFA_LAYOUT_FLOAT_PERCISION ||
        (!bContainerHeightAutoSize &&
         pThis->m_fUsedSize + fAvailHeight + XFA_LAYOUT_FLOAT_PERCISION >=
             fContainerHeight)) {
      if (!bTakeSpace || eRetValue == XFA_ItemLayoutProcessorResult_Done) {
        if (pProcessor->m_bUseInheriated) {
          if (pTrailerLayoutItem) {
            XFA_ItemLayoutProcessor_AddTrailerBeforeSplit(
                pProcessor, fChildHeight, pTrailerLayoutItem);
          }
          if (pProcessor->JudgeLeaderOrTrailerForOccur(pOverflowLeaderNode)) {
            XFA_ItemLayoutProcessor_AddPendingNode(pProcessor,
                                                   pOverflowLeaderNode, FALSE);
          }
          pProcessor->m_bUseInheriated = FALSE;
        } else {
          if (bIsAddTrailerHeight) {
            fChildHeight -= pTrailerLayoutItem->m_sSize.y;
          }
          pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                           pOverflowTrailerNode,
                                           pTrailerLayoutItem, pFormNode);
        }
        CXFA_ContentLayoutItem* pChildLayoutItem =
            pProcessor->ExtractLayoutItem();
        if (XFA_ExistContainerKeep(pProcessor->m_pFormNode, FALSE) &&
            pProcessor->m_pFormNode->GetIntact() == XFA_ATTRIBUTEENUM_None) {
          pThis->m_arrayKeepItems.Add(pChildLayoutItem);
        } else {
          pThis->m_arrayKeepItems.RemoveAll();
        }
        rgCurLineLayoutItems[uHAlign].Add(pChildLayoutItem);
        bAddedItemInRow = TRUE;
        if (bTakeSpace) {
          fContentCurRowAvailWidth -= fChildWidth;
          if (fContentCurRowHeight < fChildHeight) {
            fContentCurRowHeight = fChildHeight;
          }
        }
        return XFA_ItemLayoutProcessorResult_Done;
      } else {
        if (eRetValue == XFA_ItemLayoutProcessorResult_PageFullBreak) {
          if (pProcessor->m_bUseInheriated) {
            if (pTrailerLayoutItem) {
              XFA_ItemLayoutProcessor_AddTrailerBeforeSplit(
                  pProcessor, fChildHeight, pTrailerLayoutItem);
            }
            if (pProcessor->JudgeLeaderOrTrailerForOccur(pOverflowLeaderNode)) {
              XFA_ItemLayoutProcessor_AddPendingNode(
                  pProcessor, pOverflowLeaderNode, FALSE);
            }
            pProcessor->m_bUseInheriated = FALSE;
          } else {
            if (bIsAddTrailerHeight) {
              fChildHeight -= pTrailerLayoutItem->m_sSize.y;
            }
            pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                             pOverflowTrailerNode,
                                             pTrailerLayoutItem, pFormNode);
          }
        }
        rgCurLineLayoutItems[uHAlign].Add(pProcessor->ExtractLayoutItem());
        bAddedItemInRow = TRUE;
        fContentCurRowAvailWidth -= fChildWidth;
        if (fContentCurRowHeight < fChildHeight) {
          fContentCurRowHeight = fChildHeight;
        }
        return eRetValue;
      }
    } else {
      XFA_ItemLayoutProcessorResult eResult;
      if (pThis->ProcessKeepForSplite(
              pThis, pProcessor, eRetValue, rgCurLineLayoutItems[uHAlign],
              fContentCurRowAvailWidth, fContentCurRowHeight, fContentCurRowY,
              bAddedItemInRow, bForceEndPage, eResult)) {
        return eResult;
      }
      bForceEndPage = TRUE;
      FX_FLOAT fSplitPos =
          pProcessor->FindSplitPos(fAvailHeight - fContentCurRowY);
      if (fSplitPos > XFA_LAYOUT_FLOAT_PERCISION) {
        XFA_ATTRIBUTEENUM eLayout =
            pProcessor->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
        if (eLayout == XFA_ATTRIBUTEENUM_Tb &&
            eRetValue == XFA_ItemLayoutProcessorResult_Done) {
          pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                           pOverflowTrailerNode,
                                           pTrailerLayoutItem, pFormNode);
          rgCurLineLayoutItems[uHAlign].Add(pProcessor->ExtractLayoutItem());
          bAddedItemInRow = TRUE;
          if (bTakeSpace) {
            fContentCurRowAvailWidth -= fChildWidth;
            if (fContentCurRowHeight < fChildHeight) {
              fContentCurRowHeight = fChildHeight;
            }
          }
          return XFA_ItemLayoutProcessorResult_PageFullBreak;
        }
        CXFA_Node* pTempLeaderNode = nullptr;
        CXFA_Node* pTempTrailerNode = nullptr;
        if (pThis->m_pPageMgr && !pProcessor->m_bUseInheriated &&
            eRetValue != XFA_ItemLayoutProcessorResult_PageFullBreak) {
          pThis->m_pPageMgr->ProcessOverflow(pFormNode, pTempLeaderNode,
                                             pTempTrailerNode, FALSE, TRUE);
        }
        if (pTrailerLayoutItem && bIsAddTrailerHeight) {
          XFA_ItemLayoutProcessor_AddTrailerBeforeSplit(
              pProcessor, fSplitPos, pTrailerLayoutItem, bUseInherited);
        } else {
          pProcessor->SplitLayoutItem(fSplitPos);
        }
        if (bUseInherited) {
          pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                           pOverflowTrailerNode,
                                           pTrailerLayoutItem, pFormNode);
          pThis->m_bUseInheriated = TRUE;
        } else {
          CXFA_LayoutItem* firstChild =
              pProcessor->m_pLayoutItem->m_pFirstChild;
          if (firstChild && !firstChild->m_pNextSibling &&
              firstChild->m_pFormNode->IsLayoutGeneratedNode()) {
            pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                             pOverflowTrailerNode,
                                             pTrailerLayoutItem, pFormNode);
          } else {
            if (pProcessor->JudgeLeaderOrTrailerForOccur(pOverflowLeaderNode)) {
              XFA_ItemLayoutProcessor_AddPendingNode(
                  pProcessor, pOverflowLeaderNode, FALSE);
            }
          }
        }
        if (pProcessor->m_pLayoutItem->m_pNextSibling) {
          pProcessor->GetCurrentComponentSize(fChildWidth, fChildHeight);
          rgCurLineLayoutItems[uHAlign].Add(pProcessor->ExtractLayoutItem());
          bAddedItemInRow = TRUE;
          if (bTakeSpace) {
            fContentCurRowAvailWidth -= fChildWidth;
            if (fContentCurRowHeight < fChildHeight) {
              fContentCurRowHeight = fChildHeight;
            }
          }
        }
        return XFA_ItemLayoutProcessorResult_PageFullBreak;
      } else if (fContentCurRowY <= XFA_LAYOUT_FLOAT_PERCISION) {
        pProcessor->GetCurrentComponentSize(fChildWidth, fChildHeight);
        if (pProcessor->m_pPageMgr->GetNextAvailContentHeight(fChildHeight)) {
          CXFA_Node* pTempLeaderNode = nullptr;
          CXFA_Node* pTempTrailerNode = nullptr;
          if (pThis->m_pPageMgr) {
            if (!pFormNode && pLayoutContext) {
              pFormNode = pLayoutContext->m_pOverflowProcessor->m_pFormNode;
            }
            pThis->m_pPageMgr->ProcessOverflow(pFormNode, pTempLeaderNode,
                                               pTempTrailerNode, FALSE, TRUE);
          }
          if (bUseInherited) {
            pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                             pOverflowTrailerNode,
                                             pTrailerLayoutItem, pFormNode);
            pThis->m_bUseInheriated = TRUE;
          }
          return XFA_ItemLayoutProcessorResult_PageFullBreak;
        }
        rgCurLineLayoutItems[uHAlign].Add(pProcessor->ExtractLayoutItem());
        bAddedItemInRow = TRUE;
        if (bTakeSpace) {
          fContentCurRowAvailWidth -= fChildWidth;
          if (fContentCurRowHeight < fChildHeight) {
            fContentCurRowHeight = fChildHeight;
          }
        }
        if (eRetValue == XFA_ItemLayoutProcessorResult_Done) {
          bForceEndPage = FALSE;
        }
        return eRetValue;
      } else {
        XFA_ATTRIBUTEENUM eLayout =
            pProcessor->m_pFormNode->GetEnum(XFA_ATTRIBUTE_Layout);
        if (pProcessor->m_pFormNode->GetIntact() == XFA_ATTRIBUTEENUM_None &&
            eLayout == XFA_ATTRIBUTEENUM_Tb) {
          if (pThis->m_pPageMgr) {
            pThis->m_pPageMgr->ProcessOverflow(pFormNode, pOverflowLeaderNode,
                                               pOverflowTrailerNode, FALSE,
                                               TRUE);
          }
          if (pTrailerLayoutItem) {
            XFA_ItemLayoutProcessor_AddTrailerBeforeSplit(pProcessor, fSplitPos,
                                                          pTrailerLayoutItem);
          }
          if (pProcessor->JudgeLeaderOrTrailerForOccur(pOverflowLeaderNode)) {
            XFA_ItemLayoutProcessor_AddPendingNode(pProcessor,
                                                   pOverflowLeaderNode, FALSE);
          }
        } else {
          if (eRetValue == XFA_ItemLayoutProcessorResult_Done) {
            if (!pFormNode && pLayoutContext) {
              pFormNode = pLayoutContext->m_pOverflowProcessor->m_pFormNode;
            }
            if (pThis->m_pPageMgr) {
              pThis->m_pPageMgr->ProcessOverflow(pFormNode, pOverflowLeaderNode,
                                                 pOverflowTrailerNode, FALSE,
                                                 TRUE);
            }
            if (bUseInherited) {
              pProcessor->ProcessUnUseOverFlow(pOverflowLeaderNode,
                                               pOverflowTrailerNode,
                                               pTrailerLayoutItem, pFormNode);
              pThis->m_bUseInheriated = TRUE;
            }
          }
        }
        return XFA_ItemLayoutProcessorResult_PageFullBreak;
      }
    }
  } else {
    return XFA_ItemLayoutProcessorResult_RowFullBreak;
  }
  return XFA_ItemLayoutProcessorResult_Done;
}

XFA_ItemLayoutProcessorResult CXFA_ItemLayoutProcessor::DoLayoutFlowedContainer(
    FX_BOOL bUseBreakControl,
    XFA_ATTRIBUTEENUM eFlowStrategy,
    FX_FLOAT fHeightLimit,
    FX_FLOAT fRealHeight,
    CXFA_LayoutContext* pContext,
    FX_BOOL bRootForceTb) {
  m_bHasAvailHeight = TRUE;
  FX_FLOAT fContainerWidth = 0, fContainerHeight = 0;
  FX_BOOL bBreakDone = FALSE;
  FX_BOOL bContainerWidthAutoSize = TRUE, bContainerHeightAutoSize = TRUE;
  FX_BOOL bForceEndPage = FALSE;
  FX_BOOL bIsManualBreak = FALSE;
  if (m_pCurChildPreprocessor) {
    m_pCurChildPreprocessor->m_ePreProcessRs =
        XFA_ItemLayoutProcessorResult_Done;
  }
  XFA_ItemLayoutProcessor_CalculateContainerSpecfiedSize(
      m_pFormNode, fContainerWidth, fContainerHeight, bContainerWidthAutoSize,
      bContainerHeightAutoSize);
  if (pContext && pContext->m_bCurColumnWidthAvaiable) {
    bContainerWidthAutoSize = FALSE;
    fContainerWidth = pContext->m_fCurColumnWidth;
  }
  if (!bContainerHeightAutoSize) {
    fContainerHeight -= m_fUsedSize;
  }
  if (!bContainerHeightAutoSize) {
    CXFA_Node* pParentNode = m_pFormNode->GetNodeItem(XFA_NODEITEM_Parent);
    FX_BOOL bFocrTb = FALSE;
    if (pParentNode &&
        XFA_ItemLayoutProcessor_GetLayout(pParentNode, bFocrTb) ==
            XFA_ATTRIBUTEENUM_Row) {
      CXFA_Node* pChildContainer = m_pFormNode->GetNodeItem(
          XFA_NODEITEM_FirstChild, XFA_ObjectType::ContainerNode);
      if (pChildContainer &&
          pChildContainer->GetNodeItem(XFA_NODEITEM_NextSibling,
                                       XFA_ObjectType::ContainerNode)) {
        fContainerHeight = 0;
        bContainerHeightAutoSize = TRUE;
      }
    }
  }
  CXFA_Node* pMarginNode =
      m_pFormNode->GetFirstChildByClass(XFA_Element::Margin);
  FX_FLOAT fLeftInset = 0, fTopInset = 0, fRightInset = 0, fBottomInset = 0;
  if (pMarginNode) {
    fLeftInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_LeftInset).ToUnit(XFA_UNIT_Pt);
    fTopInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_TopInset).ToUnit(XFA_UNIT_Pt);
    fRightInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_RightInset).ToUnit(XFA_UNIT_Pt);
    fBottomInset =
        pMarginNode->GetMeasure(XFA_ATTRIBUTE_BottomInset).ToUnit(XFA_UNIT_Pt);
  }
  FX_FLOAT fContentWidthLimit =
      bContainerWidthAutoSize ? XFA_LAYOUT_FLOAT_MAX
                              : fContainerWidth - fLeftInset - fRightInset;
  FX_FLOAT fContentCalculatedWidth = 0, fContentCalculatedHeight = 0;
  FX_FLOAT fAvailHeight = fHeightLimit - fTopInset - fBottomInset;
  if (fAvailHeight < 0) {
    m_bHasAvailHeight = FALSE;
  }
  fRealHeight = fRealHeight - fTopInset - fBottomInset;
  FX_FLOAT fContentCurRowY = 0;
  CXFA_ContentLayoutItem* pLayoutChild = nullptr;
  if (m_pLayoutItem) {
    if (m_nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Done &&
        eFlowStrategy != XFA_ATTRIBUTEENUM_Tb) {
      pLayoutChild = (CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild;
      for (CXFA_ContentLayoutItem* pLayoutNext = pLayoutChild; pLayoutNext;
           pLayoutNext = (CXFA_ContentLayoutItem*)pLayoutNext->m_pNextSibling) {
        if (pLayoutNext->m_sPos.y != pLayoutChild->m_sPos.y) {
          pLayoutChild = pLayoutNext;
        }
      }
    }
    for (CXFA_ContentLayoutItem* pLayoutTempChild =
             (CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild;
         pLayoutTempChild != pLayoutChild;
         pLayoutTempChild =
             (CXFA_ContentLayoutItem*)pLayoutTempChild->m_pNextSibling) {
      if (XFA_ItemLayoutProcessor_IsTakingSpace(
              pLayoutTempChild->m_pFormNode)) {
        FX_FLOAT fChildContentWidth =
            pLayoutTempChild->m_sPos.x + pLayoutTempChild->m_sSize.x;
        FX_FLOAT fChildContentHeight =
            pLayoutTempChild->m_sPos.y + pLayoutTempChild->m_sSize.y;
        if (fContentCalculatedWidth < fChildContentWidth) {
          fContentCalculatedWidth = fChildContentWidth;
        }
        if (fContentCalculatedHeight < fChildContentHeight) {
          fContentCalculatedHeight = fChildContentHeight;
        }
      }
    }
    if (pLayoutChild) {
      fContentCurRowY = pLayoutChild->m_sPos.y;
    } else {
      fContentCurRowY = fContentCalculatedHeight;
    }
  }
  fContentCurRowY += InsertKeepLayoutItems();
  if (m_nCurChildNodeStage == XFA_ItemLayoutProcessorStages_None) {
    XFA_ItemLayoutProcessor_GotoNextContainerNode(
        m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, TRUE);
  }
  fContentCurRowY +=
      XFA_ItemLayoutProcessor_InsertPendingItems(this, m_pFormNode);
  if (m_pCurChildPreprocessor &&
      m_nCurChildNodeStage == XFA_ItemLayoutProcessorStages_Container) {
    if (XFA_ExistContainerKeep(m_pCurChildPreprocessor->GetFormNode(), FALSE)) {
      m_pKeepHeadNode = m_pCurChildNode;
      m_bIsProcessKeep = TRUE;
      m_nCurChildNodeStage = XFA_ItemLayoutProcessorStages_Keep;
    }
  }
  while (m_nCurChildNodeStage != XFA_ItemLayoutProcessorStages_Done) {
    FX_FLOAT fContentCurRowHeight = 0;
    FX_FLOAT fContentCurRowAvailWidth = fContentWidthLimit;
    m_fWidthLimite = fContentCurRowAvailWidth;
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*> rgCurLineLayoutItems[3];
    uint8_t uCurHAlignState =
        (eFlowStrategy != XFA_ATTRIBUTEENUM_Rl_tb ? 0 : 2);
    if (pLayoutChild) {
      for (CXFA_ContentLayoutItem* pLayoutNext = pLayoutChild; pLayoutNext;
           pLayoutNext = (CXFA_ContentLayoutItem*)pLayoutNext->m_pNextSibling) {
        if (!pLayoutNext->m_pNextSibling && m_pCurChildPreprocessor &&
            m_pCurChildPreprocessor->m_pFormNode == pLayoutNext->m_pFormNode) {
          pLayoutNext->m_pNext = m_pCurChildPreprocessor->m_pLayoutItem;
          m_pCurChildPreprocessor->m_pLayoutItem = pLayoutNext;
          break;
        }
        uint8_t uHAlign = XFA_ItemLayoutProcessor_HAlignEnumToInt(
            pLayoutNext->m_pFormNode->GetEnum(XFA_ATTRIBUTE_HAlign));
        rgCurLineLayoutItems[uHAlign].Add(pLayoutNext);
        if (eFlowStrategy == XFA_ATTRIBUTEENUM_Lr_tb) {
          if (uHAlign > uCurHAlignState) {
            uCurHAlignState = uHAlign;
          }
        } else if (uHAlign < uCurHAlignState) {
          uCurHAlignState = uHAlign;
        }
        if (XFA_ItemLayoutProcessor_IsTakingSpace(pLayoutNext->m_pFormNode)) {
          if (pLayoutNext->m_sSize.y > fContentCurRowHeight) {
            fContentCurRowHeight = pLayoutNext->m_sSize.y;
          }
          fContentCurRowAvailWidth -= pLayoutNext->m_sSize.x;
        }
      }
      if ((CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild ==
          pLayoutChild) {
        m_pLayoutItem->m_pFirstChild = nullptr;
      } else {
        CXFA_ContentLayoutItem* pLayoutNext =
            (CXFA_ContentLayoutItem*)m_pLayoutItem->m_pFirstChild;
        for (; pLayoutNext;
             pLayoutNext =
                 (CXFA_ContentLayoutItem*)pLayoutNext->m_pNextSibling) {
          if ((CXFA_ContentLayoutItem*)pLayoutNext->m_pNextSibling ==
              pLayoutChild) {
            pLayoutNext->m_pNextSibling = nullptr;
            break;
          }
        }
      }
      CXFA_ContentLayoutItem* pLayoutNextTemp =
          (CXFA_ContentLayoutItem*)pLayoutChild;
      while (pLayoutNextTemp) {
        pLayoutNextTemp->m_pParent = nullptr;
        CXFA_ContentLayoutItem* pSaveLayoutNext =
            (CXFA_ContentLayoutItem*)pLayoutNextTemp->m_pNextSibling;
        pLayoutNextTemp->m_pNextSibling = nullptr;
        pLayoutNextTemp = pSaveLayoutNext;
      }
      pLayoutChild = nullptr;
    }
    while (m_pCurChildNode) {
      CXFA_ItemLayoutProcessor* pProcessor = nullptr;
      FX_BOOL bAddedItemInRow = FALSE;
      fContentCurRowY +=
          XFA_ItemLayoutProcessor_InsertPendingItems(this, m_pFormNode);
      switch (m_nCurChildNodeStage) {
        case XFA_ItemLayoutProcessorStages_Keep:
        case XFA_ItemLayoutProcessorStages_None:
          break;
        case XFA_ItemLayoutProcessorStages_BreakBefore: {
          for (int32_t iIndex = 0; iIndex < m_arrayKeepItems.GetSize();
               iIndex++) {
            CXFA_ContentLayoutItem* pItem = m_arrayKeepItems.GetAt(iIndex);
            m_pLayoutItem->RemoveChild(pItem);
            fContentCalculatedHeight -= pItem->m_sSize.y;
          }
          CXFA_Node* pLeaderNode = nullptr;
          CXFA_Node* pTrailerNode = nullptr;
          FX_BOOL bCreatePage = FALSE;
          if (bUseBreakControl && m_pPageMgr &&
              m_pPageMgr->ProcessBreakBeforeOrAfter(m_pCurChildNode, TRUE,
                                                    pLeaderNode, pTrailerNode,
                                                    bCreatePage) &&
              m_pFormNode->GetElementType() != XFA_Element::Form &&
              bCreatePage) {
            if (JudgeLeaderOrTrailerForOccur(pLeaderNode)) {
              XFA_ItemLayoutProcessor_AddPendingNode(this, pLeaderNode, TRUE);
            }
            if (JudgeLeaderOrTrailerForOccur(pTrailerNode)) {
              if (m_pFormNode->GetNodeItem(XFA_NODEITEM_Parent)
                          ->GetElementType() == XFA_Element::Form &&
                  !m_pLayoutItem) {
                XFA_ItemLayoutProcessor_AddPendingNode(this, pTrailerNode,
                                                       TRUE);
              } else {
                std::unique_ptr<CXFA_ItemLayoutProcessor> pTempProcessor(
                    new CXFA_ItemLayoutProcessor(pTrailerNode, nullptr));
                XFA_ItemLayoutProcessor_InsertFlowedItem(
                    this, pTempProcessor.get(), bContainerWidthAutoSize,
                    bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                    uCurHAlignState, rgCurLineLayoutItems, FALSE,
                    XFA_LAYOUT_FLOAT_MAX, XFA_LAYOUT_FLOAT_MAX, fContentCurRowY,
                    fContentWidthLimit, fContentCurRowAvailWidth,
                    fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                    pContext);
              }
            }
            XFA_ItemLayoutProcessor_GotoNextContainerNode(
                m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, TRUE);
            bForceEndPage = TRUE;
            bIsManualBreak = TRUE;
            goto SuspendAndCreateNewRow;
          }
        } break;
        case XFA_ItemLayoutProcessorStages_BreakAfter: {
          CXFA_Node* pLeaderNode = nullptr;
          CXFA_Node* pTrailerNode = nullptr;
          FX_BOOL bCreatePage = FALSE;
          if (bUseBreakControl && m_pPageMgr &&
              m_pPageMgr->ProcessBreakBeforeOrAfter(m_pCurChildNode, FALSE,
                                                    pLeaderNode, pTrailerNode,
                                                    bCreatePage) &&
              m_pFormNode->GetElementType() != XFA_Element::Form) {
            if (JudgeLeaderOrTrailerForOccur(pTrailerNode)) {
              std::unique_ptr<CXFA_ItemLayoutProcessor> pTempProcessor(
                  new CXFA_ItemLayoutProcessor(pTrailerNode, nullptr));
              XFA_ItemLayoutProcessor_InsertFlowedItem(
                  this, pTempProcessor.get(), bContainerWidthAutoSize,
                  bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                  uCurHAlignState, rgCurLineLayoutItems, FALSE,
                  XFA_LAYOUT_FLOAT_MAX, XFA_LAYOUT_FLOAT_MAX, fContentCurRowY,
                  fContentWidthLimit, fContentCurRowAvailWidth,
                  fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                  pContext);
            }
            if (!bCreatePage) {
              if (JudgeLeaderOrTrailerForOccur(pLeaderNode)) {
                CalculateRowChildPosition(
                    rgCurLineLayoutItems, eFlowStrategy,
                    bContainerHeightAutoSize, bContainerWidthAutoSize,
                    fContentCalculatedWidth, fContentCalculatedHeight,
                    fContentCurRowY, fContentCurRowHeight, fContentWidthLimit);
                rgCurLineLayoutItems->RemoveAll();
                std::unique_ptr<CXFA_ItemLayoutProcessor> pTempProcessor(
                    new CXFA_ItemLayoutProcessor(pLeaderNode, nullptr));
                XFA_ItemLayoutProcessor_InsertFlowedItem(
                    this, pTempProcessor.get(), bContainerWidthAutoSize,
                    bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                    uCurHAlignState, rgCurLineLayoutItems, FALSE,
                    XFA_LAYOUT_FLOAT_MAX, XFA_LAYOUT_FLOAT_MAX, fContentCurRowY,
                    fContentWidthLimit, fContentCurRowAvailWidth,
                    fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                    pContext);
              }
            } else {
              if (JudgeLeaderOrTrailerForOccur(pLeaderNode)) {
                XFA_ItemLayoutProcessor_AddPendingNode(this, pLeaderNode, TRUE);
              }
            }
            XFA_ItemLayoutProcessor_GotoNextContainerNode(
                m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, TRUE);
            if (bCreatePage) {
              bForceEndPage = TRUE;
              bIsManualBreak = TRUE;
              if (m_nCurChildNodeStage == XFA_ItemLayoutProcessorStages_Done) {
                bBreakDone = TRUE;
              }
            }
            goto SuspendAndCreateNewRow;
          }
        } break;
        case XFA_ItemLayoutProcessorStages_BookendLeader: {
          CXFA_Node* pLeaderNode = nullptr;
          if (m_pCurChildPreprocessor) {
            pProcessor = m_pCurChildPreprocessor;
            m_pCurChildPreprocessor = nullptr;
          } else if (m_pPageMgr &&
                     m_pPageMgr->ProcessBookendLeaderOrTrailer(
                         m_pCurChildNode, TRUE, pLeaderNode)) {
            pProcessor = new CXFA_ItemLayoutProcessor(pLeaderNode, m_pPageMgr);
          }
          if (pProcessor) {
            if (XFA_ItemLayoutProcessor_InsertFlowedItem(
                    this, pProcessor, bContainerWidthAutoSize,
                    bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                    uCurHAlignState, rgCurLineLayoutItems, bUseBreakControl,
                    fAvailHeight, fRealHeight, fContentCurRowY,
                    fContentWidthLimit, fContentCurRowAvailWidth,
                    fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                    pContext) != XFA_ItemLayoutProcessorResult_Done) {
              goto SuspendAndCreateNewRow;
            } else {
              delete pProcessor;
              pProcessor = nullptr;
            }
          }
        } break;
        case XFA_ItemLayoutProcessorStages_BookendTrailer: {
          CXFA_Node* pTrailerNode = nullptr;
          if (m_pCurChildPreprocessor) {
            pProcessor = m_pCurChildPreprocessor;
            m_pCurChildPreprocessor = nullptr;
          } else if (m_pPageMgr &&
                     m_pPageMgr->ProcessBookendLeaderOrTrailer(
                         m_pCurChildNode, FALSE, pTrailerNode)) {
            pProcessor = new CXFA_ItemLayoutProcessor(pTrailerNode, m_pPageMgr);
          }
          if (pProcessor) {
            if (XFA_ItemLayoutProcessor_InsertFlowedItem(
                    this, pProcessor, bContainerWidthAutoSize,
                    bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                    uCurHAlignState, rgCurLineLayoutItems, bUseBreakControl,
                    fAvailHeight, fRealHeight, fContentCurRowY,
                    fContentWidthLimit, fContentCurRowAvailWidth,
                    fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                    pContext) != XFA_ItemLayoutProcessorResult_Done) {
              goto SuspendAndCreateNewRow;
            } else {
              delete pProcessor;
              pProcessor = nullptr;
            }
          }
        } break;
        case XFA_ItemLayoutProcessorStages_Container:
          ASSERT(m_pCurChildNode->IsContainerNode());
          if (m_pCurChildNode->GetElementType() == XFA_Element::Variables) {
            break;
          }
          if (fContentCurRowY >= fHeightLimit + XFA_LAYOUT_FLOAT_PERCISION &&
              XFA_ItemLayoutProcessor_IsTakingSpace(m_pCurChildNode)) {
            bForceEndPage = TRUE;
            goto SuspendAndCreateNewRow;
          }
          if (m_pCurChildNode->IsContainerNode()) {
            FX_BOOL bNewRow = FALSE;
            if (m_pCurChildPreprocessor) {
              pProcessor = m_pCurChildPreprocessor;
              m_pCurChildPreprocessor = nullptr;
              bNewRow = TRUE;
            } else {
              pProcessor =
                  new CXFA_ItemLayoutProcessor(m_pCurChildNode, m_pPageMgr);
            }
            XFA_ItemLayoutProcessor_InsertPendingItems(pProcessor,
                                                       m_pCurChildNode);
            XFA_ItemLayoutProcessorResult rs =
                XFA_ItemLayoutProcessor_InsertFlowedItem(
                    this, pProcessor, bContainerWidthAutoSize,
                    bContainerHeightAutoSize, fContainerHeight, eFlowStrategy,
                    uCurHAlignState, rgCurLineLayoutItems, bUseBreakControl,
                    fAvailHeight, fRealHeight, fContentCurRowY,
                    fContentWidthLimit, fContentCurRowAvailWidth,
                    fContentCurRowHeight, bAddedItemInRow, bForceEndPage,
                    pContext, bNewRow);
            switch (rs) {
              case XFA_ItemLayoutProcessorResult_ManualBreak:
                bIsManualBreak = TRUE;
              case XFA_ItemLayoutProcessorResult_PageFullBreak:
                bForceEndPage = TRUE;
              case XFA_ItemLayoutProcessorResult_RowFullBreak:
                goto SuspendAndCreateNewRow;
              case XFA_ItemLayoutProcessorResult_Done:
              default:
                fContentCurRowY += XFA_ItemLayoutProcessor_InsertPendingItems(
                    pProcessor, m_pCurChildNode);
                delete pProcessor;
                pProcessor = nullptr;
            }
          }
          break;
        case XFA_ItemLayoutProcessorStages_Done:
          break;
        default:
          break;
      }
      XFA_ItemLayoutProcessor_GotoNextContainerNode(
          m_pCurChildNode, m_nCurChildNodeStage, m_pFormNode, TRUE);
      if (bAddedItemInRow && eFlowStrategy == XFA_ATTRIBUTEENUM_Tb) {
        break;
      } else {
        continue;
      }
    SuspendAndCreateNewRow:
      if (pProcessor) {
        m_pCurChildPreprocessor = pProcessor;
      }
      break;
    }
    CalculateRowChildPosition(rgCurLineLayoutItems, eFlowStrategy,
                              bContainerHeightAutoSize, bContainerWidthAutoSize,
                              fContentCalculatedWidth, fContentCalculatedHeight,
                              fContentCurRowY, fContentCurRowHeight,
                              fContentWidthLimit, bRootForceTb);
    m_fWidthLimite = fContentCurRowAvailWidth;
    if (bForceEndPage) {
      break;
    }
  }
  FX_BOOL bRetValue =
      m_nCurChildNodeStage == XFA_ItemLayoutProcessorStages_Done &&
      m_PendingNodes.empty();
  if (bBreakDone) {
    bRetValue = FALSE;
  }
  XFA_ItemLayoutProcessor_CalculateContainerComponentSizeFromContentSize(
      m_pFormNode, bContainerWidthAutoSize, fContentCalculatedWidth,
      fContainerWidth, bContainerHeightAutoSize, fContentCalculatedHeight,
      fContainerHeight);
  if (fContainerHeight >= XFA_LAYOUT_FLOAT_PERCISION || m_pLayoutItem ||
      bRetValue) {
    if (!m_pLayoutItem) {
      m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
    }
    if (fContainerHeight < 0) {
      fContainerHeight = 0;
    }
    SetCurrentComponentSize(fContainerWidth, fContainerHeight);
    if (bForceEndPage) {
      m_fUsedSize = 0;
    } else {
      m_fUsedSize += m_pLayoutItem->m_sSize.y;
    }
  }
  return bRetValue
             ? XFA_ItemLayoutProcessorResult_Done
             : (bIsManualBreak ? XFA_ItemLayoutProcessorResult_ManualBreak
                               : XFA_ItemLayoutProcessorResult_PageFullBreak);
}

FX_BOOL CXFA_ItemLayoutProcessor::CalculateRowChildPosition(
    CFX_ArrayTemplate<CXFA_ContentLayoutItem*>(&rgCurLineLayoutItems)[3],
    XFA_ATTRIBUTEENUM eFlowStrategy,
    FX_BOOL bContainerHeightAutoSize,
    FX_BOOL bContainerWidthAutoSize,
    FX_FLOAT& fContentCalculatedWidth,
    FX_FLOAT& fContentCalculatedHeight,
    FX_FLOAT& fContentCurRowY,
    FX_FLOAT fContentCurRowHeight,
    FX_FLOAT fContentWidthLimit,
    FX_BOOL bRootForceTb) {
  int32_t nGroupLengths[3] = {0, 0, 0};
  FX_FLOAT fGroupWidths[3] = {0, 0, 0};
  int32_t nTotalLength = 0;
  for (int32_t i = 0; i < 3; i++) {
    nGroupLengths[i] = rgCurLineLayoutItems[i].GetSize();
    for (int32_t c = nGroupLengths[i], j = 0; j < c; j++) {
      nTotalLength++;
      if (XFA_ItemLayoutProcessor_IsTakingSpace(
              rgCurLineLayoutItems[i][j]->m_pFormNode)) {
        fGroupWidths[i] += rgCurLineLayoutItems[i][j]->m_sSize.x;
      }
    }
  }
  if (!nTotalLength) {
    if (bContainerHeightAutoSize) {
      FX_FLOAT fNewHeight = fContentCurRowY;
      if (fContentCalculatedHeight > fNewHeight) {
        fContentCalculatedHeight = fNewHeight;
      }
    }
    return FALSE;
  }
  if (!m_pLayoutItem) {
    m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
  }
  if (eFlowStrategy != XFA_ATTRIBUTEENUM_Rl_tb) {
    FX_FLOAT fCurPos;
    fCurPos = 0;
    for (int32_t c = nGroupLengths[0], j = 0; j < c; j++) {
      if (bRootForceTb) {
        FX_FLOAT fAbsoluteX, fAbsoluteY;
        CalculatePositionedContainerPos(rgCurLineLayoutItems[0][j]->m_pFormNode,
                                        rgCurLineLayoutItems[0][j]->m_sSize.x,
                                        rgCurLineLayoutItems[0][j]->m_sSize.y,
                                        fAbsoluteX, fAbsoluteY);
        rgCurLineLayoutItems[0][j]->m_sPos = CFX_PointF(fAbsoluteX, fAbsoluteY);
      } else {
        rgCurLineLayoutItems[0][j]->m_sPos =
            CFX_PointF(fCurPos, fContentCurRowY);
        if (XFA_ItemLayoutProcessor_IsTakingSpace(
                rgCurLineLayoutItems[0][j]->m_pFormNode)) {
          fCurPos += rgCurLineLayoutItems[0][j]->m_sSize.x;
        }
      }
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[0][j]);
      m_fLastRowWidth = fCurPos;
    }
    fCurPos = (fContentWidthLimit + fGroupWidths[0] - fGroupWidths[1] -
               fGroupWidths[2]) /
              2;
    for (int32_t c = nGroupLengths[1], j = 0; j < c; j++) {
      if (bRootForceTb) {
        FX_FLOAT fAbsoluteX, fAbsoluteY;
        CalculatePositionedContainerPos(rgCurLineLayoutItems[1][j]->m_pFormNode,
                                        rgCurLineLayoutItems[1][j]->m_sSize.x,
                                        rgCurLineLayoutItems[1][j]->m_sSize.y,
                                        fAbsoluteX, fAbsoluteY);
        rgCurLineLayoutItems[1][j]->m_sPos = CFX_PointF(fAbsoluteX, fAbsoluteY);
      } else {
        rgCurLineLayoutItems[1][j]->m_sPos =
            CFX_PointF(fCurPos, fContentCurRowY);
        if (XFA_ItemLayoutProcessor_IsTakingSpace(
                rgCurLineLayoutItems[1][j]->m_pFormNode)) {
          fCurPos += rgCurLineLayoutItems[1][j]->m_sSize.x;
        }
      }
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[1][j]);
      m_fLastRowWidth = fCurPos;
    }
    fCurPos = fContentWidthLimit - fGroupWidths[2];
    for (int32_t c = nGroupLengths[2], j = 0; j < c; j++) {
      if (bRootForceTb) {
        FX_FLOAT fAbsoluteX, fAbsoluteY;
        CalculatePositionedContainerPos(rgCurLineLayoutItems[2][j]->m_pFormNode,
                                        rgCurLineLayoutItems[2][j]->m_sSize.x,
                                        rgCurLineLayoutItems[2][j]->m_sSize.y,
                                        fAbsoluteX, fAbsoluteY);
        rgCurLineLayoutItems[2][j]->m_sPos = CFX_PointF(fAbsoluteX, fAbsoluteY);
      } else {
        rgCurLineLayoutItems[2][j]->m_sPos =
            CFX_PointF(fCurPos, fContentCurRowY);
        if (XFA_ItemLayoutProcessor_IsTakingSpace(
                rgCurLineLayoutItems[2][j]->m_pFormNode)) {
          fCurPos += rgCurLineLayoutItems[2][j]->m_sSize.x;
        }
      }
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[2][j]);
      m_fLastRowWidth = fCurPos;
    }
  } else {
    FX_FLOAT fCurPos;
    fCurPos = fGroupWidths[0];
    for (int32_t c = nGroupLengths[0], j = 0; j < c; j++) {
      if (XFA_ItemLayoutProcessor_IsTakingSpace(
              rgCurLineLayoutItems[0][j]->m_pFormNode)) {
        fCurPos -= rgCurLineLayoutItems[0][j]->m_sSize.x;
      }
      rgCurLineLayoutItems[0][j]->m_sPos = CFX_PointF(fCurPos, fContentCurRowY);
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[0][j]);
      m_fLastRowWidth = fCurPos;
    }
    fCurPos = (fContentWidthLimit + fGroupWidths[0] + fGroupWidths[1] -
               fGroupWidths[2]) /
              2;
    for (int32_t c = nGroupLengths[1], j = 0; j < c; j++) {
      if (XFA_ItemLayoutProcessor_IsTakingSpace(
              rgCurLineLayoutItems[1][j]->m_pFormNode)) {
        fCurPos -= rgCurLineLayoutItems[1][j]->m_sSize.x;
      }
      rgCurLineLayoutItems[1][j]->m_sPos = CFX_PointF(fCurPos, fContentCurRowY);
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[1][j]);
      m_fLastRowWidth = fCurPos;
    }
    fCurPos = fContentWidthLimit;
    for (int32_t c = nGroupLengths[2], j = 0; j < c; j++) {
      if (XFA_ItemLayoutProcessor_IsTakingSpace(
              rgCurLineLayoutItems[2][j]->m_pFormNode)) {
        fCurPos -= rgCurLineLayoutItems[2][j]->m_sSize.x;
      }
      rgCurLineLayoutItems[2][j]->m_sPos = CFX_PointF(fCurPos, fContentCurRowY);
      m_pLayoutItem->AddChild(rgCurLineLayoutItems[2][j]);
      m_fLastRowWidth = fCurPos;
    }
  }
  m_fLastRowY = fContentCurRowY;
  fContentCurRowY += fContentCurRowHeight;
  if (bContainerWidthAutoSize) {
    FX_FLOAT fChildSuppliedWidth = fGroupWidths[0];
    if (fContentWidthLimit < XFA_LAYOUT_FLOAT_MAX &&
        fContentWidthLimit > fChildSuppliedWidth) {
      fChildSuppliedWidth = fContentWidthLimit;
    }
    if (fContentCalculatedWidth < fChildSuppliedWidth) {
      fContentCalculatedWidth = fChildSuppliedWidth;
    }
  }
  if (bContainerHeightAutoSize) {
    FX_FLOAT fChildSuppliedHeight = fContentCurRowY;
    if (fContentCalculatedHeight < fChildSuppliedHeight) {
      fContentCalculatedHeight = fChildSuppliedHeight;
    }
  }
  return TRUE;
}
CXFA_Node* CXFA_ItemLayoutProcessor::GetSubformSetParent(
    CXFA_Node* pSubformSet) {
  if (pSubformSet && pSubformSet->GetElementType() == XFA_Element::SubformSet) {
    CXFA_Node* pParent = pSubformSet->GetNodeItem(XFA_NODEITEM_Parent);
    while (pParent) {
      if (pParent->GetElementType() != XFA_Element::SubformSet) {
        return pParent;
      }
      pParent = pParent->GetNodeItem(XFA_NODEITEM_Parent);
    }
  }
  return pSubformSet;
}
void CXFA_ItemLayoutProcessor::DoLayoutField() {
  if (m_pLayoutItem)
    return;

  ASSERT(m_pCurChildNode == XFA_LAYOUT_INVALIDNODE);
  m_pLayoutItem = CreateContentLayoutItem(m_pFormNode);
  if (!m_pLayoutItem) {
    return;
  }
  CXFA_Document* pDocument = m_pFormNode->GetDocument();
  CXFA_FFNotify* pNotify = pDocument->GetNotify();
  FX_FLOAT fHeight = -1;
  FX_FLOAT fWidth = -1;
  pNotify->StartFieldDrawLayout(m_pFormNode, fWidth, fHeight);
  int32_t nRotate =
      FXSYS_round(m_pFormNode->GetMeasure(XFA_ATTRIBUTE_Rotate).GetValue());
  nRotate = XFA_MapRotation(nRotate);
  if (nRotate == 90 || nRotate == 270) {
    FX_FLOAT fTmp = fWidth;
    fWidth = fHeight;
    fHeight = fTmp;
  }
  SetCurrentComponentSize(fWidth, fHeight);
}
XFA_ItemLayoutProcessorResult CXFA_ItemLayoutProcessor::DoLayout(
    FX_BOOL bUseBreakControl,
    FX_FLOAT fHeightLimit,
    FX_FLOAT fRealHeight,
    CXFA_LayoutContext* pContext) {
  switch (m_pFormNode->GetElementType()) {
    case XFA_Element::Subform:
    case XFA_Element::Area:
    case XFA_Element::ExclGroup:
    case XFA_Element::SubformSet: {
      FX_BOOL bRootForceTb = FALSE;
      CXFA_Node* pLayoutNode = GetSubformSetParent(m_pFormNode);
      XFA_ATTRIBUTEENUM eLayoutStrategy =
          XFA_ItemLayoutProcessor_GetLayout(pLayoutNode, bRootForceTb);
      switch (eLayoutStrategy) {
        case XFA_ATTRIBUTEENUM_Tb:
        case XFA_ATTRIBUTEENUM_Lr_tb:
        case XFA_ATTRIBUTEENUM_Rl_tb:
          return DoLayoutFlowedContainer(bUseBreakControl, eLayoutStrategy,
                                         fHeightLimit, fRealHeight, pContext,
                                         bRootForceTb);
        case XFA_ATTRIBUTEENUM_Position:
        case XFA_ATTRIBUTEENUM_Row:
        case XFA_ATTRIBUTEENUM_Rl_row:
        default:
          DoLayoutPositionedContainer(pContext);
          m_nCurChildNodeStage = XFA_ItemLayoutProcessorStages_Done;
          return XFA_ItemLayoutProcessorResult_Done;
        case XFA_ATTRIBUTEENUM_Table:
          DoLayoutTableContainer(pLayoutNode);
          m_nCurChildNodeStage = XFA_ItemLayoutProcessorStages_Done;
          return XFA_ItemLayoutProcessorResult_Done;
      }
    }
    case XFA_Element::Draw:
    case XFA_Element::Field:
      DoLayoutField();
      m_nCurChildNodeStage = XFA_ItemLayoutProcessorStages_Done;
      return XFA_ItemLayoutProcessorResult_Done;
    case XFA_Element::ContentArea:
      return XFA_ItemLayoutProcessorResult_Done;
    default:
      return XFA_ItemLayoutProcessorResult_Done;
  }
}
void CXFA_ItemLayoutProcessor::GetCurrentComponentPos(FX_FLOAT& fAbsoluteX,
                                                      FX_FLOAT& fAbsoluteY) {
  ASSERT(m_pLayoutItem);
  fAbsoluteX = m_pLayoutItem->m_sPos.x;
  fAbsoluteY = m_pLayoutItem->m_sPos.y;
}
void CXFA_ItemLayoutProcessor::GetCurrentComponentSize(FX_FLOAT& fWidth,
                                                       FX_FLOAT& fHeight) {
  ASSERT(m_pLayoutItem);
  fWidth = m_pLayoutItem->m_sSize.x;
  fHeight = m_pLayoutItem->m_sSize.y;
}
void CXFA_ItemLayoutProcessor::SetCurrentComponentPos(FX_FLOAT fAbsoluteX,
                                                      FX_FLOAT fAbsoluteY) {
  m_pLayoutItem->m_sPos = CFX_PointF(fAbsoluteX, fAbsoluteY);
}
void CXFA_ItemLayoutProcessor::SetCurrentComponentSize(FX_FLOAT fWidth,
                                                       FX_FLOAT fHeight) {
  m_pLayoutItem->m_sSize = CFX_SizeF(fWidth, fHeight);
}

FX_BOOL CXFA_ItemLayoutProcessor::JudgeLeaderOrTrailerForOccur(
    CXFA_Node* pFormNode) {
  if (!pFormNode)
    return FALSE;

  CXFA_Node* pTemplate = pFormNode->GetTemplateNode();
  if (!pTemplate)
    pTemplate = pFormNode;

  CXFA_Occur NodeOccur(pTemplate->GetFirstChildByClass(XFA_Element::Occur));
  int32_t iMax = NodeOccur.GetMax();
  if (iMax < 0)
    return TRUE;

  int32_t iCount = m_PendingNodesCount[pTemplate];
  if (iCount >= iMax)
    return FALSE;

  m_PendingNodesCount[pTemplate] = iCount + 1;
  return TRUE;
}
