// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_contentmarkitem.h"

#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"

CPDF_ContentMarkItem::CPDF_ContentMarkItem()
    : m_ParamType(None), m_pParam(nullptr) {}

CPDF_ContentMarkItem::CPDF_ContentMarkItem(const CPDF_ContentMarkItem& src) {
  m_MarkName = src.m_MarkName;
  m_ParamType = src.m_ParamType;
  if (m_ParamType == DirectDict) {
    m_pParam = ToDictionary(src.m_pParam->Clone());
  } else {
    m_pParam = src.m_pParam;
  }
}

CPDF_ContentMarkItem::~CPDF_ContentMarkItem() {
  if (m_ParamType == DirectDict && m_pParam)
    m_pParam->Release();
}

FX_BOOL CPDF_ContentMarkItem::HasMCID() const {
  if (m_pParam &&
      (m_ParamType == DirectDict || m_ParamType == PropertiesDict)) {
    return m_pParam->KeyExist("MCID");
  }
  return FALSE;
}
