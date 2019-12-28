// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_DOCUMENT_LAYOUT_IMP_H_
#define XFA_FXFA_PARSER_XFA_DOCUMENT_LAYOUT_IMP_H_

#include "xfa/fxfa/parser/xfa_doclayout.h"

class CXFA_ItemLayoutProcessor;
class CXFA_LayoutPageMgr;
class CXFA_ContainerLayoutItem;

class CXFA_LayoutProcessor {
 public:
  CXFA_LayoutProcessor(CXFA_Document* pDocument);
  ~CXFA_LayoutProcessor();

  CXFA_Document* GetDocument() const;
  int32_t StartLayout(FX_BOOL bForceRestart = FALSE);
  int32_t DoLayout(IFX_Pause* pPause = nullptr);
  FX_BOOL IncrementLayout();
  int32_t CountPages() const;
  CXFA_ContainerLayoutItem* GetPage(int32_t index) const;
  CXFA_LayoutItem* GetLayoutItem(CXFA_Node* pFormItem);

  void AddChangedContainer(CXFA_Node* pContainer);
  void SetForceReLayout(FX_BOOL bForceRestart) { m_bNeeLayout = bForceRestart; }
  CXFA_ContainerLayoutItem* GetRootLayoutItem() const;
  CXFA_ItemLayoutProcessor* GetRootRootItemLayoutProcessor() {
    return m_pRootItemLayoutProcessor;
  }
  CXFA_LayoutPageMgr* GetLayoutPageMgr() { return m_pLayoutPageMgr; }

 protected:
  void ClearLayoutData();

  FX_BOOL IsNeedLayout();

  CXFA_Document* m_pDocument;
  CXFA_ItemLayoutProcessor* m_pRootItemLayoutProcessor;
  CXFA_LayoutPageMgr* m_pLayoutPageMgr;
  CXFA_NodeArray m_rgChangedContainers;
  uint32_t m_nProgressCounter;
  FX_BOOL m_bNeeLayout;
};

#endif  // XFA_FXFA_PARSER_XFA_DOCUMENT_LAYOUT_IMP_H_
