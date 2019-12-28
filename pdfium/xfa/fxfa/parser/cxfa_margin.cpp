// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_margin.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Margin::CXFA_Margin(CXFA_Node* pNode) : CXFA_Data(pNode) {}

FX_BOOL CXFA_Margin::GetLeftInset(FX_FLOAT& fInset, FX_FLOAT fDefInset) const {
  fInset = fDefInset;
  return TryMeasure(XFA_ATTRIBUTE_LeftInset, fInset);
}

FX_BOOL CXFA_Margin::GetTopInset(FX_FLOAT& fInset, FX_FLOAT fDefInset) const {
  fInset = fDefInset;
  return TryMeasure(XFA_ATTRIBUTE_TopInset, fInset);
}

FX_BOOL CXFA_Margin::GetRightInset(FX_FLOAT& fInset, FX_FLOAT fDefInset) const {
  fInset = fDefInset;
  return TryMeasure(XFA_ATTRIBUTE_RightInset, fInset);
}

FX_BOOL CXFA_Margin::GetBottomInset(FX_FLOAT& fInset,
                                    FX_FLOAT fDefInset) const {
  fInset = fDefInset;
  return TryMeasure(XFA_ATTRIBUTE_BottomInset, fInset);
}
