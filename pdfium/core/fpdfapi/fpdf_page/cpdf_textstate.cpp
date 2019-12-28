// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/cpdf_textstate.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"

void CPDF_TextState::SetFont(CPDF_Font* pFont) {
  CPDF_TextStateData* pStateData = GetModify();
  if (pStateData) {
    CPDF_Document* pDoc = pStateData->m_pDocument;
    CPDF_DocPageData* pPageData = pDoc ? pDoc->GetPageData() : nullptr;
    if (pPageData && pStateData->m_pFont && !pPageData->IsForceClear()) {
      pPageData->ReleaseFont(pStateData->m_pFont->GetFontDict());
    }
    pStateData->m_pDocument = pFont ? pFont->m_pDocument : nullptr;
    pStateData->m_pFont = pFont;
  }
}

FX_FLOAT CPDF_TextState::GetFontSizeV() const {
  FX_FLOAT* pMatrix = GetMatrix();
  FX_FLOAT unit = FXSYS_sqrt2(pMatrix[1], pMatrix[3]);
  FX_FLOAT size = unit * GetFontSize();
  return (FX_FLOAT)FXSYS_fabs(size);
}

FX_FLOAT CPDF_TextState::GetFontSizeH() const {
  FX_FLOAT* pMatrix = GetMatrix();
  FX_FLOAT unit = FXSYS_sqrt2(pMatrix[0], pMatrix[2]);
  FX_FLOAT size = unit * GetFontSize();
  return (FX_FLOAT)FXSYS_fabs(size);
}

FX_FLOAT CPDF_TextState::GetBaselineAngle() const {
  FX_FLOAT* m_Matrix = GetMatrix();
  return FXSYS_atan2(m_Matrix[2], m_Matrix[0]);
}

FX_FLOAT CPDF_TextState::GetShearAngle() const {
  FX_FLOAT* m_Matrix = GetMatrix();
  FX_FLOAT shear_angle = FXSYS_atan2(m_Matrix[1], m_Matrix[3]);
  return GetBaselineAngle() + shear_angle;
}
