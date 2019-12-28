// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_DOCLAYOUT_H_
#define XFA_FXFA_PARSER_XFA_DOCLAYOUT_H_

#include "xfa/fxfa/parser/xfa_document.h"

class CXFA_ContainerLayoutItem;
class CXFA_ContentLayoutItem;
class CXFA_LayoutProcessor;

class CXFA_LayoutItem {
 public:
  virtual ~CXFA_LayoutItem();

  FX_BOOL IsContainerLayoutItem() const { return !m_bIsContentLayoutItem; }
  FX_BOOL IsContentLayoutItem() const { return m_bIsContentLayoutItem; }
  inline CXFA_ContainerLayoutItem* AsContainerLayoutItem();
  inline CXFA_ContentLayoutItem* AsContentLayoutItem();

  CXFA_ContainerLayoutItem* GetPage() const;
  CXFA_Node* GetFormNode() const;
  void GetRect(CFX_RectF& rtLayout, FX_BOOL bRelative = FALSE) const;
  int32_t GetIndex() const;
  int32_t GetCount() const;
  CXFA_LayoutItem* GetParent() const;
  const CXFA_LayoutItem* GetFirst() const;
  CXFA_LayoutItem* GetFirst();
  const CXFA_LayoutItem* GetLast() const;
  CXFA_LayoutItem* GetLast();
  CXFA_LayoutItem* GetPrev() const;
  CXFA_LayoutItem* GetNext() const;

  void AddChild(CXFA_LayoutItem* pChildItem);
  void AddHeadChild(CXFA_LayoutItem* pChildItem);
  void RemoveChild(CXFA_LayoutItem* pChildItem);
  void InsertChild(CXFA_LayoutItem* pBeforeItem, CXFA_LayoutItem* pChildItem);

  CXFA_Node* m_pFormNode;
  CXFA_LayoutItem* m_pParent;
  CXFA_LayoutItem* m_pNextSibling;
  CXFA_LayoutItem* m_pFirstChild;

 protected:
  CXFA_LayoutItem(CXFA_Node* pNode, FX_BOOL bIsContentLayoutItem);

  FX_BOOL m_bIsContentLayoutItem;
};

class CXFA_ContainerLayoutItem : public CXFA_LayoutItem {
 public:
  CXFA_ContainerLayoutItem(CXFA_Node* pNode);

  CXFA_LayoutProcessor* GetLayout() const;
  int32_t GetPageIndex() const;
  void GetPageSize(CFX_SizeF& size) const;
  CXFA_Node* GetMasterPage() const;

  CXFA_Node* m_pOldSubform;
};

class CXFA_ContentLayoutItem : public CXFA_LayoutItem {
 public:
  CXFA_ContentLayoutItem(CXFA_Node* pNode);
  ~CXFA_ContentLayoutItem() override;

  CXFA_ContentLayoutItem* m_pPrev;
  CXFA_ContentLayoutItem* m_pNext;
  CFX_PointF m_sPos;
  CFX_SizeF m_sSize;
  uint32_t m_dwStatus;
};

CXFA_ContainerLayoutItem* CXFA_LayoutItem::AsContainerLayoutItem() {
  return IsContainerLayoutItem() ? static_cast<CXFA_ContainerLayoutItem*>(this)
                                 : nullptr;
}
CXFA_ContentLayoutItem* CXFA_LayoutItem::AsContentLayoutItem() {
  return IsContentLayoutItem() ? static_cast<CXFA_ContentLayoutItem*>(this)
                               : nullptr;
}
inline CXFA_ContainerLayoutItem* ToContainerLayoutItem(CXFA_LayoutItem* pItem) {
  return pItem ? pItem->AsContainerLayoutItem() : nullptr;
}
inline CXFA_ContentLayoutItem* ToContentLayoutItem(CXFA_LayoutItem* pItem) {
  return pItem ? pItem->AsContentLayoutItem() : nullptr;
}

class CXFA_TraverseStrategy_LayoutItem {
 public:
  static inline CXFA_LayoutItem* GetFirstChild(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pFirstChild;
  }
  static inline CXFA_LayoutItem* GetNextSibling(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pNextSibling;
  }
  static inline CXFA_LayoutItem* GetParent(CXFA_LayoutItem* pLayoutItem) {
    return pLayoutItem->m_pParent;
  }
};

#endif  // XFA_FXFA_PARSER_XFA_DOCLAYOUT_H_
