// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_font.h"

#include "core/fxge/include/fx_dib.h"
#include "xfa/fxfa/parser/cxfa_fill.h"
#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Font::CXFA_Font(CXFA_Node* pNode) : CXFA_Data(pNode) {}

FX_FLOAT CXFA_Font::GetBaselineShift() {
  return m_pNode->GetMeasure(XFA_ATTRIBUTE_BaselineShift).ToUnit(XFA_UNIT_Pt);
}

FX_FLOAT CXFA_Font::GetHorizontalScale() {
  CFX_WideString wsValue;
  m_pNode->TryCData(XFA_ATTRIBUTE_FontHorizontalScale, wsValue);
  int32_t iScale = FXSYS_wtoi(wsValue.c_str());
  return iScale > 0 ? (FX_FLOAT)iScale : 100.0f;
}

FX_FLOAT CXFA_Font::GetVerticalScale() {
  CFX_WideString wsValue;
  m_pNode->TryCData(XFA_ATTRIBUTE_FontVerticalScale, wsValue);
  int32_t iScale = FXSYS_wtoi(wsValue.c_str());
  return iScale > 0 ? (FX_FLOAT)iScale : 100.0f;
}

FX_FLOAT CXFA_Font::GetLetterSpacing() {
  CFX_WideStringC wsValue;
  if (!m_pNode->TryCData(XFA_ATTRIBUTE_LetterSpacing, wsValue))
    return 0;

  CXFA_Measurement ms(wsValue);
  if (ms.GetUnit() == XFA_UNIT_Em)
    return ms.GetValue() * GetFontSize();
  return ms.ToUnit(XFA_UNIT_Pt);
}

int32_t CXFA_Font::GetLineThrough() {
  int32_t iValue = 0;
  m_pNode->TryInteger(XFA_ATTRIBUTE_LineThrough, iValue);
  return iValue;
}

int32_t CXFA_Font::GetUnderline() {
  int32_t iValue = 0;
  m_pNode->TryInteger(XFA_ATTRIBUTE_Underline, iValue);
  return iValue;
}

int32_t CXFA_Font::GetUnderlinePeriod() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_All;
  m_pNode->TryEnum(XFA_ATTRIBUTE_UnderlinePeriod, eAttr);
  return eAttr;
}

FX_FLOAT CXFA_Font::GetFontSize() {
  CXFA_Measurement ms;
  m_pNode->TryMeasure(XFA_ATTRIBUTE_Size, ms);
  return ms.ToUnit(XFA_UNIT_Pt);
}

void CXFA_Font::GetTypeface(CFX_WideStringC& wsTypeFace) {
  m_pNode->TryCData(XFA_ATTRIBUTE_Typeface, wsTypeFace);
}

FX_BOOL CXFA_Font::IsBold() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Normal;
  m_pNode->TryEnum(XFA_ATTRIBUTE_Weight, eAttr);
  return eAttr == XFA_ATTRIBUTEENUM_Bold;
}

FX_BOOL CXFA_Font::IsItalic() {
  XFA_ATTRIBUTEENUM eAttr = XFA_ATTRIBUTEENUM_Normal;
  m_pNode->TryEnum(XFA_ATTRIBUTE_Posture, eAttr);
  return eAttr == XFA_ATTRIBUTEENUM_Italic;
}

void CXFA_Font::SetColor(FX_ARGB color) {
  CXFA_Fill fill(m_pNode->GetProperty(0, XFA_Element::Fill));
  fill.SetColor(color);
}

FX_ARGB CXFA_Font::GetColor() {
  CXFA_Fill fill(m_pNode->GetChild(0, XFA_Element::Fill));
  return fill ? fill.GetColor(TRUE) : 0xFF000000;
}
