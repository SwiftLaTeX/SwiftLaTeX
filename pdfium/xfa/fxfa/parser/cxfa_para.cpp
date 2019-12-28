// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_para.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Para::CXFA_Para(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Para::GetHorizontalAlign() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Left;
  m_pNode->TryEnum(XFA_ATTRIBUTE_HAlign, eAttr);
  return eAttr;
}

int32_t CXFA_Para::GetVerticalAlign() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Top;
  m_pNode->TryEnum(XFA_ATTRIBUTE_VAlign, eAttr);
  return eAttr;
}

FX_FLOAT CXFA_Para::GetLineHeight() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_LineHeight, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Para::GetMarginLeft() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_MarginLeft, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Para::GetMarginRight() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_MarginRight, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Para::GetSpaceAbove() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_SpaceAbove, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Para::GetSpaceBelow() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_SpaceBelow, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Para::GetTextIndent() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_TextIndent, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}
