// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_clippathdata.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_path.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"

CPDF_ClipPathData::CPDF_ClipPathData() {}

CPDF_ClipPathData::~CPDF_ClipPathData() {}

CPDF_ClipPathData::CPDF_ClipPathData(const CPDF_ClipPathData& src) {
  m_PathAndTypeList = src.m_PathAndTypeList;

  m_TextList.resize(src.m_TextList.size());
  for (size_t i = 0; i < src.m_TextList.size(); ++i) {
    if (src.m_TextList[i])
      m_TextList[i].reset(src.m_TextList[i]->Clone());
  }
}
