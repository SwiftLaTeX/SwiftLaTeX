// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_contentmark.h"

FX_BOOL CPDF_ContentMark::HasMark(const CFX_ByteStringC& mark) const {
  if (!m_pObject)
    return FALSE;

  for (int i = 0; i < m_pObject->CountItems(); i++) {
    CPDF_ContentMarkItem& item = m_pObject->GetItem(i);
    if (item.GetName() == mark)
      return TRUE;
  }
  return FALSE;
}

FX_BOOL CPDF_ContentMark::LookupMark(const CFX_ByteStringC& mark,
                                     CPDF_Dictionary*& pDict) const {
  if (!m_pObject)
    return FALSE;

  for (int i = 0; i < m_pObject->CountItems(); i++) {
    CPDF_ContentMarkItem& item = m_pObject->GetItem(i);
    if (item.GetName() == mark) {
      pDict = nullptr;
      if (item.GetParamType() == CPDF_ContentMarkItem::PropertiesDict ||
          item.GetParamType() == CPDF_ContentMarkItem::DirectDict) {
        pDict = item.GetParam();
      }
      return TRUE;
    }
  }
  return FALSE;
}
