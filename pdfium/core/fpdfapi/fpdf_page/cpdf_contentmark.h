// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARK_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARK_H_

#include "core/fpdfapi/fpdf_page/cpdf_contentmarkdata.h"
#include "core/fxcrt/include/fx_basic.h"

class CPDF_ContentMark : public CFX_CountRef<CPDF_ContentMarkData> {
 public:
  int GetMCID() const { return m_pObject ? m_pObject->GetMCID() : -1; }

  FX_BOOL HasMark(const CFX_ByteStringC& mark) const;

  FX_BOOL LookupMark(const CFX_ByteStringC& mark,
                     CPDF_Dictionary*& pDict) const;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_CONTENTMARK_H_
