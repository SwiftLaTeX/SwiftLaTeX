// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_layout_appadapter.h"

#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_layout_itemlayout.h"
#include "xfa/fxfa/parser/xfa_layout_pagemgr_new.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

void XFA_ReleaseLayoutItem(CXFA_LayoutItem* pLayoutItem) {
  CXFA_LayoutItem* pNode = pLayoutItem->m_pFirstChild;
  CXFA_FFNotify* pNotify = pLayoutItem->m_pFormNode->GetDocument()->GetNotify();
  CXFA_LayoutProcessor* pDocLayout =
      pLayoutItem->m_pFormNode->GetDocument()->GetDocLayout();
  while (pNode) {
    CXFA_LayoutItem* pNext = pNode->m_pNextSibling;
    pNode->m_pParent = nullptr;
    pNotify->OnLayoutItemRemoving(pDocLayout,
                                  static_cast<CXFA_LayoutItem*>(pNode));
    XFA_ReleaseLayoutItem(pNode);
    pNode = pNext;
  }
  pNotify->OnLayoutItemRemoving(pDocLayout, pLayoutItem);
  if (pLayoutItem->m_pFormNode->GetElementType() == XFA_Element::PageArea) {
    pNotify->OnPageEvent(static_cast<CXFA_ContainerLayoutItem*>(pLayoutItem),
                         XFA_PAGEVIEWEVENT_PostRemoved);
  }
  delete pLayoutItem;
}
