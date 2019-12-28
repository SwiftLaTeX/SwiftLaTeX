// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfx_chariter.h"

CFX_CharIter::CFX_CharIter(const CFX_WideString& wsText)
    : m_wsText(wsText), m_nIndex(0) {
  ASSERT(!wsText.IsEmpty());
}

CFX_CharIter::~CFX_CharIter() {}

FX_BOOL CFX_CharIter::Next(FX_BOOL bPrev) {
  if (bPrev) {
    if (m_nIndex <= 0)
      return FALSE;
    m_nIndex--;
  } else {
    if (m_nIndex + 1 >= m_wsText.GetLength())
      return FALSE;
    m_nIndex++;
  }
  return TRUE;
}

FX_WCHAR CFX_CharIter::GetChar() {
  return m_wsText.GetAt(m_nIndex);
}

void CFX_CharIter::SetAt(int32_t nIndex) {
  if (nIndex < 0 || nIndex >= m_wsText.GetLength())
    return;
  m_nIndex = nIndex;
}

int32_t CFX_CharIter::GetAt() const {
  return m_nIndex;
}

FX_BOOL CFX_CharIter::IsEOF(FX_BOOL bTail) const {
  return bTail ? (m_nIndex + 1 == m_wsText.GetLength()) : (m_nIndex == 0);
}

IFX_CharIter* CFX_CharIter::Clone() {
  CFX_CharIter* pIter = new CFX_CharIter(m_wsText);
  pIter->m_nIndex = m_nIndex;
  return pIter;
}
