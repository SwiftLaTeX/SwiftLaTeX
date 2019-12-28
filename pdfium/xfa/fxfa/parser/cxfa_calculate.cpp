// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_calculate.h"

#include "xfa/fxfa/parser/cxfa_text.h"
#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Calculate::CXFA_Calculate(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Calculate::GetOverride() {
  XFA_ATTRIBUTEENUM eAtt = XFA_ATTRIBUTEENUM_Error;
  m_pNode->TryEnum(XFA_ATTRIBUTE_Override, eAtt, FALSE);
  return eAtt;
}

CXFA_Script CXFA_Calculate::GetScript() {
  return CXFA_Script(m_pNode->GetChild(0, XFA_Element::Script));
}

void CXFA_Calculate::GetMessageText(CFX_WideString& wsMessage) {
  CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Message);
  if (!pNode)
    return;

  CXFA_Text text(pNode->GetChild(0, XFA_Element::Text));
  if (text)
    text.GetContent(wsMessage);
}
