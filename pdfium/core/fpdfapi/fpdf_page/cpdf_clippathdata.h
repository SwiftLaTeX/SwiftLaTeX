// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_CLIPPATHDATA_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_CLIPPATHDATA_H_

#include <stdint.h>

#include <memory>
#include <utility>
#include <vector>

class CPDF_Path;
class CPDF_TextObject;

class CPDF_ClipPathData {
 public:
  using PathAndTypeData = std::pair<CPDF_Path, uint8_t>;
  CPDF_ClipPathData();
  CPDF_ClipPathData(const CPDF_ClipPathData&);
  ~CPDF_ClipPathData();

  std::vector<PathAndTypeData> m_PathAndTypeList;
  std::vector<std::unique_ptr<CPDF_TextObject>> m_TextList;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_CLIPPATHDATA_H_
