// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKDATA_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKDATA_H_

#include <vector>

#include "core/fpdfapi/fpdf_page/cpdf_contentmarkitem.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Dictionary;

class CPDF_ContentMarkData {
 public:
  CPDF_ContentMarkData();
  CPDF_ContentMarkData(const CPDF_ContentMarkData& src);
  ~CPDF_ContentMarkData();

  int CountItems() const;
  CPDF_ContentMarkItem& GetItem(int index);
  const CPDF_ContentMarkItem& GetItem(int index) const;

  int GetMCID() const;
  void AddMark(const CFX_ByteString& name,
               CPDF_Dictionary* pDict,
               FX_BOOL bDictNeedClone);
  void DeleteLastMark();

 private:
  std::vector<CPDF_ContentMarkItem> m_Marks;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARKDATA_H_
