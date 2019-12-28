// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_stroke.h"

#include "xfa/fxfa/parser/xfa_object.h"

int32_t CXFA_Stroke::GetPresence() const {
  return m_pNode ? m_pNode->GetEnum(XFA_ATTRIBUTE_Presence)
                 : XFA_ATTRIBUTEENUM_Invisible;
}

int32_t CXFA_Stroke::GetCapType() const {
  if (!m_pNode)
    return XFA_ATTRIBUTEENUM_Square;
  return m_pNode->GetEnum(XFA_ATTRIBUTE_Cap);
}

int32_t CXFA_Stroke::GetStrokeType() const {
  return m_pNode ? m_pNode->GetEnum(XFA_ATTRIBUTE_Stroke)
                 : XFA_ATTRIBUTEENUM_Solid;
}

FX_FLOAT CXFA_Stroke::GetThickness() const {
  return GetMSThickness().ToUnit(XFA_UNIT_Pt);
}

CXFA_Measurement CXFA_Stroke::GetMSThickness() const {
  return m_pNode ? m_pNode->GetMeasure(XFA_ATTRIBUTE_Thickness)
                 : XFA_GetAttributeDefaultValue_Measure(XFA_Element::Edge,
                                                        XFA_ATTRIBUTE_Thickness,
                                                        XFA_XDPPACKET_Form);
}

void CXFA_Stroke::SetMSThickness(CXFA_Measurement msThinkness) {
  if (!m_pNode)
    return;

  m_pNode->SetMeasure(XFA_ATTRIBUTE_Thickness, msThinkness);
}

FX_ARGB CXFA_Stroke::GetColor() const {
  if (!m_pNode)
    return 0xFF000000;

  CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Color);
  if (!pNode)
    return 0xFF000000;

  CFX_WideStringC wsColor;
  pNode->TryCData(XFA_ATTRIBUTE_Value, wsColor);
  return CXFA_Data::ToColor(wsColor);
}

void CXFA_Stroke::SetColor(FX_ARGB argb) {
  if (!m_pNode)
    return;

  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Color);
  CFX_WideString wsColor;
  int a;
  int r;
  int g;
  int b;
  ArgbDecode(argb, a, r, g, b);
  wsColor.Format(L"%d,%d,%d", r, g, b);
  pNode->SetCData(XFA_ATTRIBUTE_Value, wsColor);
}

int32_t CXFA_Stroke::GetJoinType() const {
  return m_pNode ? m_pNode->GetEnum(XFA_ATTRIBUTE_Join)
                 : XFA_ATTRIBUTEENUM_Square;
}

FX_BOOL CXFA_Stroke::IsInverted() const {
  return m_pNode ? m_pNode->GetBoolean(XFA_ATTRIBUTE_Inverted) : FALSE;
}

FX_FLOAT CXFA_Stroke::GetRadius() const {
  return m_pNode ? m_pNode->GetMeasure(XFA_ATTRIBUTE_Radius).ToUnit(XFA_UNIT_Pt)
                 : 0;
}

FX_BOOL CXFA_Stroke::SameStyles(CXFA_Stroke stroke, uint32_t dwFlags) const {
  if (m_pNode == stroke.GetNode())
    return TRUE;
  if (FXSYS_fabs(GetThickness() - stroke.GetThickness()) >= 0.01f)
    return FALSE;
  if ((dwFlags & XFA_STROKE_SAMESTYLE_NoPresence) == 0 &&
      IsVisible() != stroke.IsVisible()) {
    return FALSE;
  }
  if (GetStrokeType() != stroke.GetStrokeType())
    return FALSE;
  if (GetColor() != stroke.GetColor())
    return FALSE;
  if ((dwFlags & XFA_STROKE_SAMESTYLE_Corner) != 0 &&
      FXSYS_fabs(GetRadius() - stroke.GetRadius()) >= 0.01f) {
    return FALSE;
  }
  return TRUE;
}
