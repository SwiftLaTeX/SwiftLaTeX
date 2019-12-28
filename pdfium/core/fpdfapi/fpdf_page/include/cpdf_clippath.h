// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_CLIPPATH_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_CLIPPATH_H_

#include "core/fpdfapi/fpdf_page/cpdf_clippathdata.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_path.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_TextObject;

class CPDF_ClipPath : public CFX_CountRef<CPDF_ClipPathData> {
 public:
  uint32_t GetPathCount() const;
  CPDF_Path GetPath(size_t i) const;
  uint8_t GetClipType(size_t i) const;
  uint32_t GetTextCount() const;
  CPDF_TextObject* GetText(size_t i) const;
  CFX_FloatRect GetClipBox() const;
  void AppendPath(CPDF_Path path, uint8_t type, bool bAutoMerge);
  void AppendTexts(std::vector<std::unique_ptr<CPDF_TextObject>>* pTexts);
  void Transform(const CFX_Matrix& matrix);
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_CLIPPATH_H_
