// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_caption.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Caption::CXFA_Caption(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Caption::GetPresence() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Visible;
  m_pNode->TryEnum(XFA_ATTRIBUTE_Presence, eAttr);
  return eAttr;
}

int32_t CXFA_Caption::GetPlacementType() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Left;
  m_pNode->TryEnum(XFA_ATTRIBUTE_Placement, eAttr);
  return eAttr;
}

FX_FLOAT CXFA_Caption::GetReserve() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_Reserve, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

CXFA_Margin CXFA_Caption::GetMargin() {
  return CXFA_Margin(m_pNode ? m_pNode->GetChild(0, XFA_Element::Margin)
                             : nullptr);
}

CXFA_Font CXFA_Caption::GetFont() {
  return CXFA_Font(m_pNode ? m_pNode->GetChild(0, XFA_Element::Font) : nullptr);
}

CXFA_Value CXFA_Caption::GetValue() {
  return CXFA_Value(m_pNode ? m_pNode->GetChild(0, XFA_Element::Value)
                            : nullptr);
}
