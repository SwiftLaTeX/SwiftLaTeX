// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_SymbolDict.h"

#include "core/fxcodec/jbig2/JBig2_Image.h"
#include "core/fxcrt/include/fx_memory.h"

CJBig2_SymbolDict::CJBig2_SymbolDict() {}

CJBig2_SymbolDict::~CJBig2_SymbolDict() {}

std::unique_ptr<CJBig2_SymbolDict> CJBig2_SymbolDict::DeepCopy() const {
  const CJBig2_SymbolDict* src = this;
  std::unique_ptr<CJBig2_SymbolDict> dst(new CJBig2_SymbolDict);
  for (size_t i = 0; i < src->m_SDEXSYMS.size(); ++i) {
    CJBig2_Image* image = src->m_SDEXSYMS.get(i);
    dst->m_SDEXSYMS.push_back(image ? new CJBig2_Image(*image) : nullptr);
  }
  dst->m_gbContext = src->m_gbContext;
  dst->m_grContext = src->m_grContext;
  return dst;
}
