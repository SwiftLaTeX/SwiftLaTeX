// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_fill.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Fill::CXFA_Fill(CXFA_Node* pNode) : CXFA_Data(pNode) {}

CXFA_Fill::~CXFA_Fill() {}

int32_t CXFA_Fill::GetPresence() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Presence);
}

void CXFA_Fill::SetColor(FX_ARGB color) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Color);
  CFX_WideString wsColor;
  int a, r, g, b;
  ArgbDecode(color, a, r, g, b);
  wsColor.Format(L"%d,%d,%d", r, g, b);
  pNode->SetCData(XFA_ATTRIBUTE_Value, wsColor);
}

FX_ARGB CXFA_Fill::GetColor(FX_BOOL bText) {
  if (CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Color)) {
    CFX_WideStringC wsColor;
    if (pNode->TryCData(XFA_ATTRIBUTE_Value, wsColor, FALSE))
      return CXFA_Data::ToColor(wsColor);
  }
  if (bText)
    return 0xFF000000;
  return 0xFFFFFFFF;
}

XFA_Element CXFA_Fill::GetFillType() {
  CXFA_Node* pChild = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  while (pChild) {
    XFA_Element eType = pChild->GetElementType();
    if (eType != XFA_Element::Color && eType != XFA_Element::Extras)
      return eType;

    pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  return XFA_Element::Solid;
}

int32_t CXFA_Fill::GetPattern(FX_ARGB& foreColor) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Pattern);
  if (CXFA_Node* pColor = pNode->GetChild(0, XFA_Element::Color)) {
    CFX_WideStringC wsColor;
    pColor->TryCData(XFA_ATTRIBUTE_Value, wsColor, FALSE);
    foreColor = CXFA_Data::ToColor(wsColor);
  } else {
    foreColor = 0xFF000000;
  }
  return pNode->GetEnum(XFA_ATTRIBUTE_Type);
}

int32_t CXFA_Fill::GetStipple(FX_ARGB& stippleColor) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Stipple);
  int32_t eAttr = 50;
  pNode->TryInteger(XFA_ATTRIBUTE_Rate, eAttr);
  if (CXFA_Node* pColor = pNode->GetChild(0, XFA_Element::Color)) {
    CFX_WideStringC wsColor;
    pColor->TryCData(XFA_ATTRIBUTE_Value, wsColor, FALSE);
    stippleColor = CXFA_Data::ToColor(wsColor);
  } else {
    stippleColor = 0xFF000000;
  }
  return eAttr;
}

int32_t CXFA_Fill::GetLinear(FX_ARGB& endColor) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Linear);
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_ToRight;
  pNode->TryEnum(XFA_ATTRIBUTE_Type, eAttr);
  if (CXFA_Node* pColor = pNode->GetChild(0, XFA_Element::Color)) {
    CFX_WideStringC wsColor;
    pColor->TryCData(XFA_ATTRIBUTE_Value, wsColor, FALSE);
    endColor = CXFA_Data::ToColor(wsColor);
  } else {
    endColor = 0xFF000000;
  }
  return eAttr;
}

int32_t CXFA_Fill::GetRadial(FX_ARGB& endColor) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Radial);
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_ToEdge;
  pNode->TryEnum(XFA_ATTRIBUTE_Type, eAttr);
  if (CXFA_Node* pColor = pNode->GetChild(0, XFA_Element::Color)) {
    CFX_WideStringC wsColor;
    pColor->TryCData(XFA_ATTRIBUTE_Value, wsColor, FALSE);
    endColor = CXFA_Data::ToColor(wsColor);
  } else {
    endColor = 0xFF000000;
  }
  return eAttr;
}
