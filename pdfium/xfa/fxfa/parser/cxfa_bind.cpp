// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_bind.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Bind::CXFA_Bind(CXFA_Node* pNode) : CXFA_Data(pNode) {}

void CXFA_Bind::GetPicture(CFX_WideString& wsPicture) {
  if (CXFA_Node* pPicture = m_pNode->GetChild(0, XFA_Element::Picture))
    pPicture->TryContent(wsPicture);
}
