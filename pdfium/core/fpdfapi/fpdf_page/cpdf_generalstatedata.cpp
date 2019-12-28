// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_generalstatedata.h"

#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_render/render_int.h"

namespace {

static int GetBlendTypeInternal(const CFX_ByteStringC& mode) {
  switch (mode.GetID()) {
    case FXBSTR_ID('N', 'o', 'r', 'm'):
    case FXBSTR_ID('C', 'o', 'm', 'p'):
      return FXDIB_BLEND_NORMAL;
    case FXBSTR_ID('M', 'u', 'l', 't'):
      return FXDIB_BLEND_MULTIPLY;
    case FXBSTR_ID('S', 'c', 'r', 'e'):
      return FXDIB_BLEND_SCREEN;
    case FXBSTR_ID('O', 'v', 'e', 'r'):
      return FXDIB_BLEND_OVERLAY;
    case FXBSTR_ID('D', 'a', 'r', 'k'):
      return FXDIB_BLEND_DARKEN;
    case FXBSTR_ID('L', 'i', 'g', 'h'):
      return FXDIB_BLEND_LIGHTEN;
    case FXBSTR_ID('C', 'o', 'l', 'o'):
      if (mode.GetLength() == 10)
        return FXDIB_BLEND_COLORDODGE;

      if (mode.GetLength() == 9)
        return FXDIB_BLEND_COLORBURN;

      return FXDIB_BLEND_COLOR;
    case FXBSTR_ID('H', 'a', 'r', 'd'):
      return FXDIB_BLEND_HARDLIGHT;
    case FXBSTR_ID('S', 'o', 'f', 't'):
      return FXDIB_BLEND_SOFTLIGHT;
    case FXBSTR_ID('D', 'i', 'f', 'f'):
      return FXDIB_BLEND_DIFFERENCE;
    case FXBSTR_ID('E', 'x', 'c', 'l'):
      return FXDIB_BLEND_EXCLUSION;
    case FXBSTR_ID('H', 'u', 'e', 0):
      return FXDIB_BLEND_HUE;
    case FXBSTR_ID('S', 'a', 't', 'u'):
      return FXDIB_BLEND_SATURATION;
    case FXBSTR_ID('L', 'u', 'm', 'i'):
      return FXDIB_BLEND_LUMINOSITY;
  }
  return FXDIB_BLEND_NORMAL;
}

}  // namespace

CPDF_GeneralStateData::CPDF_GeneralStateData() {
  FXSYS_memset(this, 0, sizeof(CPDF_GeneralStateData));
  FXSYS_strcpy((FX_CHAR*)m_BlendMode, "Normal");
  m_StrokeAlpha = 1.0f;
  m_FillAlpha = 1.0f;
  m_Flatness = 1.0f;
  m_Matrix.SetIdentity();
}

CPDF_GeneralStateData::CPDF_GeneralStateData(const CPDF_GeneralStateData& src) {
  FXSYS_memcpy(this, &src, sizeof(CPDF_GeneralStateData));
  if (src.m_pTransferFunc && src.m_pTransferFunc->m_pPDFDoc) {
    CPDF_DocRenderData* pDocCache =
        src.m_pTransferFunc->m_pPDFDoc->GetRenderData();
    if (!pDocCache)
      return;

    m_pTransferFunc = pDocCache->GetTransferFunc(m_pTR);
  }
}

CPDF_GeneralStateData::~CPDF_GeneralStateData() {
  if (m_pTransferFunc && m_pTransferFunc->m_pPDFDoc) {
    CPDF_DocRenderData* pDocCache = m_pTransferFunc->m_pPDFDoc->GetRenderData();
    if (!pDocCache)
      return;

    pDocCache->ReleaseTransferFunc(m_pTR);
  }
}

void CPDF_GeneralStateData::SetBlendMode(const CFX_ByteStringC& blend_mode) {
  if (blend_mode.GetLength() > 15) {
    return;
  }
  FXSYS_memcpy(m_BlendMode, blend_mode.raw_str(), blend_mode.GetLength());
  m_BlendMode[blend_mode.GetLength()] = 0;
  m_BlendType = GetBlendTypeInternal(blend_mode);
}
