// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATE_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATE_H_

#include "core/fpdfapi/fpdf_page/include/cpdf_generalstatedata.h"
#include "core/fxcrt/include/fx_basic.h"

class CPDF_GeneralState : public CFX_CountRef<CPDF_GeneralStateData> {
 public:
  void SetRenderIntent(const CFX_ByteString& ri);

  int GetBlendType() const {
    return m_pObject ? m_pObject->m_BlendType : FXDIB_BLEND_NORMAL;
  }

  int GetAlpha(FX_BOOL bStroke) const {
    if (!m_pObject)
      return 255;
    return FXSYS_round(
        (bStroke ? m_pObject->m_StrokeAlpha : m_pObject->m_FillAlpha) * 255);
  }
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATE_H_
