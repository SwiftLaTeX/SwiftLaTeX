// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_TEXTSTATE_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_TEXTSTATE_H_

#include "core/fpdfapi/fpdf_page/include/cpdf_textstatedata.h"
#include "core/fxcrt/include/fx_basic.h"

class CPDF_Font;

class CPDF_TextState : public CFX_CountRef<CPDF_TextStateData> {
 public:
  CPDF_Font* GetFont() const { return m_pObject->m_pFont; }
  void SetFont(CPDF_Font* pFont);

  FX_FLOAT GetFontSize() const { return m_pObject->m_FontSize; }
  FX_FLOAT* GetMatrix() const { return m_pObject->m_Matrix; }
  FX_FLOAT GetFontSizeV() const;
  FX_FLOAT GetFontSizeH() const;
  FX_FLOAT GetBaselineAngle() const;
  FX_FLOAT GetShearAngle() const;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_TEXTSTATE_H_
