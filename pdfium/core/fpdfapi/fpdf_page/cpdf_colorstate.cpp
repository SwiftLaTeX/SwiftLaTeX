// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_colorstate.h"

#include "core/fpdfapi/fpdf_page/cpdf_pattern.h"
#include "core/fpdfapi/fpdf_page/cpdf_tilingpattern.h"
#include "core/fxge/include/fx_dib.h"

void CPDF_ColorState::SetFillColor(CPDF_ColorSpace* pCS,
                                   FX_FLOAT* pValue,
                                   uint32_t nValues) {
  CPDF_ColorStateData* pData = GetModify();
  SetColor(pData->m_FillColor, pData->m_FillRGB, pCS, pValue, nValues);
}

void CPDF_ColorState::SetStrokeColor(CPDF_ColorSpace* pCS,
                                     FX_FLOAT* pValue,
                                     uint32_t nValues) {
  CPDF_ColorStateData* pData = GetModify();
  SetColor(pData->m_StrokeColor, pData->m_StrokeRGB, pCS, pValue, nValues);
}

void CPDF_ColorState::SetColor(CPDF_Color& color,
                               uint32_t& rgb,
                               CPDF_ColorSpace* pCS,
                               FX_FLOAT* pValue,
                               uint32_t nValues) {
  if (pCS) {
    color.SetColorSpace(pCS);
  } else if (color.IsNull()) {
    color.SetColorSpace(CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY));
  }
  if (color.GetColorSpace()->CountComponents() > nValues)
    return;

  color.SetValue(pValue);
  int R, G, B;
  rgb = color.GetRGB(R, G, B) ? FXSYS_RGB(R, G, B) : (uint32_t)-1;
}

void CPDF_ColorState::SetFillPattern(CPDF_Pattern* pPattern,
                                     FX_FLOAT* pValue,
                                     uint32_t nValues) {
  CPDF_ColorStateData* pData = GetModify();
  pData->m_FillColor.SetValue(pPattern, pValue, nValues);
  int R, G, B;
  FX_BOOL ret = pData->m_FillColor.GetRGB(R, G, B);
  if (CPDF_TilingPattern* pTilingPattern = pPattern->AsTilingPattern()) {
    if (!ret && pTilingPattern->colored()) {
      pData->m_FillRGB = 0x00BFBFBF;
      return;
    }
  }
  pData->m_FillRGB = ret ? FXSYS_RGB(R, G, B) : (uint32_t)-1;
}

void CPDF_ColorState::SetStrokePattern(CPDF_Pattern* pPattern,
                                       FX_FLOAT* pValue,
                                       uint32_t nValues) {
  CPDF_ColorStateData* pData = GetModify();
  pData->m_StrokeColor.SetValue(pPattern, pValue, nValues);
  int R, G, B;
  FX_BOOL ret = pData->m_StrokeColor.GetRGB(R, G, B);
  if (CPDF_TilingPattern* pTilingPattern = pPattern->AsTilingPattern()) {
    if (!ret && pTilingPattern->colored()) {
      pData->m_StrokeRGB = 0x00BFBFBF;
      return;
    }
  }
  pData->m_StrokeRGB =
      pData->m_StrokeColor.GetRGB(R, G, B) ? FXSYS_RGB(R, G, B) : (uint32_t)-1;
}
