// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_colorstatedata.h"

CPDF_ColorStateData::CPDF_ColorStateData(const CPDF_ColorStateData& src) {
  m_FillColor.Copy(&src.m_FillColor);
  m_FillRGB = src.m_FillRGB;
  m_StrokeColor.Copy(&src.m_StrokeColor);
  m_StrokeRGB = src.m_StrokeRGB;
}

void CPDF_ColorStateData::Default() {
  m_FillRGB = m_StrokeRGB = 0;
  m_FillColor.SetColorSpace(CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY));
  m_StrokeColor.SetColorSpace(CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY));
}
