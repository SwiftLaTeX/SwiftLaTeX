// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_script.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Script::CXFA_Script(CXFA_Node* pNode) : CXFA_Data(pNode) {}

XFA_SCRIPTTYPE CXFA_Script::GetContentType() {
  CFX_WideStringC cData;
  if (m_pNode->TryCData(XFA_ATTRIBUTE_ContentType, cData, FALSE)) {
    if (cData == FX_WSTRC(L"application/x-javascript"))
      return XFA_SCRIPTTYPE_Javascript;
    if (cData == FX_WSTRC(L"application/x-formcalc"))
      return XFA_SCRIPTTYPE_Formcalc;
    return XFA_SCRIPTTYPE_Unkown;
  }
  return XFA_SCRIPTTYPE_Formcalc;
}

int32_t CXFA_Script::GetRunAt() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_RunAt);
}

void CXFA_Script::GetExpression(CFX_WideString& wsExpression) {
  m_pNode->TryContent(wsExpression);
}
