// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_event.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Event::CXFA_Event(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Event::GetActivity() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Activity);
}

XFA_Element CXFA_Event::GetEventType() {
  CXFA_Node* pChild = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  while (pChild) {
    XFA_Element eType = pChild->GetElementType();
    if (eType != XFA_Element::Extras)
      return eType;

    pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  return XFA_Element::Unknown;
}

void CXFA_Event::GetRef(CFX_WideStringC& wsRef) {
  m_pNode->TryCData(XFA_ATTRIBUTE_Ref, wsRef);
}

CXFA_Script CXFA_Event::GetScript() {
  return CXFA_Script(m_pNode->GetChild(0, XFA_Element::Script));
}

CXFA_Submit CXFA_Event::GetSubmit() {
  return CXFA_Submit(m_pNode->GetChild(0, XFA_Element::Submit));
}

void CXFA_Event::GetSignDataTarget(CFX_WideString& wsTarget) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::SignData);
  if (!pNode)
    return;

  CFX_WideStringC wsCData;
  pNode->TryCData(XFA_ATTRIBUTE_Target, wsCData);
  wsTarget = wsCData;
}
