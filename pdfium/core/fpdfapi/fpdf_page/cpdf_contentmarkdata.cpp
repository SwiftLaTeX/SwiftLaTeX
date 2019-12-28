// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_contentmarkdata.h"

#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "third_party/base/stl_util.h"

CPDF_ContentMarkData::CPDF_ContentMarkData() {}

CPDF_ContentMarkData::CPDF_ContentMarkData(const CPDF_ContentMarkData& src)
    : m_Marks(src.m_Marks) {}

CPDF_ContentMarkData::~CPDF_ContentMarkData() {}

int CPDF_ContentMarkData::CountItems() const {
  return pdfium::CollectionSize<int>(m_Marks);
}

CPDF_ContentMarkItem& CPDF_ContentMarkData::GetItem(int index) {
  return m_Marks[index];
}

const CPDF_ContentMarkItem& CPDF_ContentMarkData::GetItem(int index) const {
  return m_Marks[index];
}

int CPDF_ContentMarkData::GetMCID() const {
  for (const auto& mark : m_Marks) {
    CPDF_ContentMarkItem::ParamType type = mark.GetParamType();
    if (type == CPDF_ContentMarkItem::PropertiesDict ||
        type == CPDF_ContentMarkItem::DirectDict) {
      CPDF_Dictionary* pDict = mark.GetParam();
      if (pDict->KeyExist("MCID"))
        return pDict->GetIntegerBy("MCID");
    }
  }
  return -1;
}

void CPDF_ContentMarkData::AddMark(const CFX_ByteString& name,
                                   CPDF_Dictionary* pDict,
                                   FX_BOOL bDirect) {
  CPDF_ContentMarkItem item;
  item.SetName(name);
  if (pDict) {
    if (bDirect) {
      item.SetParam(CPDF_ContentMarkItem::DirectDict,
                    ToDictionary(pDict->Clone()));
    } else {
      item.SetParam(CPDF_ContentMarkItem::PropertiesDict, pDict);
    }
  }
  m_Marks.push_back(item);
}

void CPDF_ContentMarkData::DeleteLastMark() {
  if (!m_Marks.empty())
    m_Marks.pop_back();
}
