// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_occur.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Occur::CXFA_Occur(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Occur::GetMax() {
  int32_t iMax = 1;
  if (m_pNode) {
    if (!m_pNode->TryInteger(XFA_ATTRIBUTE_Max, iMax, TRUE))
      iMax = GetMin();
  }
  return iMax;
}

int32_t CXFA_Occur::GetMin() {
  int32_t iMin = 1;
  if (m_pNode) {
    if (!m_pNode->TryInteger(XFA_ATTRIBUTE_Min, iMin, TRUE) || iMin < 0)
      iMin = 1;
  }
  return iMin;
}

FX_BOOL CXFA_Occur::GetOccurInfo(int32_t& iMin, int32_t& iMax, int32_t& iInit) {
  if (!m_pNode)
    return FALSE;
  if (!m_pNode->TryInteger(XFA_ATTRIBUTE_Min, iMin, FALSE) || iMin < 0)
    iMin = 1;
  if (!m_pNode->TryInteger(XFA_ATTRIBUTE_Max, iMax, FALSE)) {
    if (iMin == 0)
      iMax = 1;
    else
      iMax = iMin;
  }
  if (!m_pNode->TryInteger(XFA_ATTRIBUTE_Initial, iInit, FALSE) ||
      iInit < iMin) {
    iInit = iMin;
  }
  return TRUE;
}

void CXFA_Occur::SetMax(int32_t iMax) {
  iMax = (iMax != -1 && iMax < 1) ? 1 : iMax;
  m_pNode->SetInteger(XFA_ATTRIBUTE_Max, iMax, FALSE);
  int32_t iMin = GetMin();
  if (iMax != -1 && iMax < iMin) {
    iMin = iMax;
    m_pNode->SetInteger(XFA_ATTRIBUTE_Min, iMin, FALSE);
  }
}

void CXFA_Occur::SetMin(int32_t iMin) {
  iMin = (iMin < 0) ? 1 : iMin;
  m_pNode->SetInteger(XFA_ATTRIBUTE_Min, iMin, FALSE);
  int32_t iMax = GetMax();
  if (iMax > 0 && iMax < iMin) {
    iMax = iMin;
    m_pNode->SetInteger(XFA_ATTRIBUTE_Max, iMax, FALSE);
  }
}
