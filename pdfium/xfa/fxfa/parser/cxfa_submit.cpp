// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_submit.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Submit::CXFA_Submit(CXFA_Node* pNode) : CXFA_Data(pNode) {}

FX_BOOL CXFA_Submit::IsSubmitEmbedPDF() {
  return m_pNode->GetBoolean(XFA_ATTRIBUTE_EmbedPDF);
}

int32_t CXFA_Submit::GetSubmitFormat() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Format);
}

void CXFA_Submit::GetSubmitTarget(CFX_WideStringC& wsTarget) {
  m_pNode->TryCData(XFA_ATTRIBUTE_Target, wsTarget);
}

void CXFA_Submit::GetSubmitXDPContent(CFX_WideStringC& wsContent) {
  m_pNode->TryCData(XFA_ATTRIBUTE_XdpContent, wsContent);
}
