// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_clippath.h"

#include <utility>

#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"
#include "third_party/base/stl_util.h"

#define FPDF_CLIPPATH_MAX_TEXTS 1024

uint32_t CPDF_ClipPath::GetPathCount() const {
  return pdfium::CollectionSize<uint32_t>(m_pObject->m_PathAndTypeList);
}

CPDF_Path CPDF_ClipPath::GetPath(size_t i) const {
  return m_pObject->m_PathAndTypeList[i].first;
}

uint8_t CPDF_ClipPath::GetClipType(size_t i) const {
  return m_pObject->m_PathAndTypeList[i].second;
}

uint32_t CPDF_ClipPath::GetTextCount() const {
  return pdfium::CollectionSize<uint32_t>(m_pObject->m_TextList);
}

CPDF_TextObject* CPDF_ClipPath::GetText(size_t i) const {
  return m_pObject->m_TextList[i].get();
}

CFX_FloatRect CPDF_ClipPath::GetClipBox() const {
  CFX_FloatRect rect;
  FX_BOOL bStarted = FALSE;
  int count = GetPathCount();
  if (count) {
    rect = GetPath(0).GetBoundingBox();
    for (int i = 1; i < count; i++) {
      CFX_FloatRect path_rect = GetPath(i).GetBoundingBox();
      rect.Intersect(path_rect);
    }
    bStarted = TRUE;
  }
  count = GetTextCount();
  if (count) {
    CFX_FloatRect layer_rect;
    FX_BOOL bLayerStarted = FALSE;
    for (int i = 0; i < count; i++) {
      CPDF_TextObject* pTextObj = GetText(i);
      if (!pTextObj) {
        if (!bStarted) {
          rect = layer_rect;
          bStarted = TRUE;
        } else {
          rect.Intersect(layer_rect);
        }
        bLayerStarted = FALSE;
      } else {
        if (!bLayerStarted) {
          layer_rect = CFX_FloatRect(pTextObj->GetBBox(nullptr));
          bLayerStarted = TRUE;
        } else {
          layer_rect.Union(CFX_FloatRect(pTextObj->GetBBox(nullptr)));
        }
      }
    }
  }
  return rect;
}

void CPDF_ClipPath::AppendPath(CPDF_Path path, uint8_t type, bool bAutoMerge) {
  CPDF_ClipPathData* pData = GetModify();
  if (!pData->m_PathAndTypeList.empty() && bAutoMerge) {
    const CPDF_Path& old_path = pData->m_PathAndTypeList.back().first;
    if (old_path.IsRect()) {
      CFX_FloatRect old_rect(old_path.GetPointX(0), old_path.GetPointY(0),
                             old_path.GetPointX(2), old_path.GetPointY(2));
      CFX_FloatRect new_rect = path.GetBoundingBox();
      if (old_rect.Contains(new_rect))
        pData->m_PathAndTypeList.pop_back();
    }
  }
  pData->m_PathAndTypeList.push_back(std::make_pair(path, type));
}

void CPDF_ClipPath::AppendTexts(
    std::vector<std::unique_ptr<CPDF_TextObject>>* pTexts) {
  CPDF_ClipPathData* pData = GetModify();
  if (pData->m_TextList.size() + pTexts->size() <= FPDF_CLIPPATH_MAX_TEXTS) {
    for (size_t i = 0; i < pTexts->size(); i++)
      pData->m_TextList.push_back(std::move((*pTexts)[i]));
    pData->m_TextList.push_back(std::unique_ptr<CPDF_TextObject>());
  }
  pTexts->clear();
}

void CPDF_ClipPath::Transform(const CFX_Matrix& matrix) {
  CPDF_ClipPathData* pData = GetModify();
  for (auto& obj : pData->m_PathAndTypeList)
    obj.first.Transform(&matrix);
  for (auto& text : pData->m_TextList) {
    if (text)
      text->Transform(matrix);
  }
}
