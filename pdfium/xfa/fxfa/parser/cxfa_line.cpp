// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_line.h"

#include "xfa/fxfa/parser/xfa_object.h"

int32_t CXFA_Line::GetHand() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Hand);
}

FX_BOOL CXFA_Line::GetSlop() {
  XFA_ATTRIBUTEENUM eSlop = m_pNode->GetEnum(XFA_ATTRIBUTE_Slope);
  return eSlop == XFA_ATTRIBUTEENUM_Slash;
}

CXFA_Edge CXFA_Line::GetEdge() {
  return CXFA_Edge(m_pNode->GetChild(0, XFA_Element::Edge));
}
