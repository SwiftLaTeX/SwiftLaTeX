// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_value.h"

#include "xfa/fxfa/parser/xfa_object.h"

XFA_Element CXFA_Value::GetChildValueClassID() {
  if (!m_pNode)
    return XFA_Element::Unknown;
  if (CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild))
    return pNode->GetElementType();
  return XFA_Element::Unknown;
}

FX_BOOL CXFA_Value::GetChildValueContent(CFX_WideString& wsContent) {
  if (!m_pNode)
    return FALSE;
  if (CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild))
    return pNode->TryContent(wsContent);
  return FALSE;
}

CXFA_Arc CXFA_Value::GetArc() {
  return CXFA_Arc(m_pNode ? m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)
                          : nullptr);
}

CXFA_Line CXFA_Value::GetLine() {
  return CXFA_Line(m_pNode ? m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)
                           : nullptr);
}

CXFA_Rectangle CXFA_Value::GetRectangle() {
  return CXFA_Rectangle(m_pNode ? m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)
                                : nullptr);
}

CXFA_Text CXFA_Value::GetText() {
  return CXFA_Text(m_pNode ? m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)
                           : nullptr);
}

CXFA_ExData CXFA_Value::GetExData() {
  return CXFA_ExData(m_pNode ? m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)
                             : nullptr);
}

CXFA_Image CXFA_Value::GetImage() {
  return CXFA_Image(
      m_pNode ? (m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild)) : nullptr,
      TRUE);
}
