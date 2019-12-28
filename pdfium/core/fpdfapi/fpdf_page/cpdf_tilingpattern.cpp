// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_tilingpattern.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"

CPDF_TilingPattern::CPDF_TilingPattern(CPDF_Document* pDoc,
                                       CPDF_Object* pPatternObj,
                                       const CFX_Matrix& parentMatrix)
    : CPDF_Pattern(TILING, pDoc, pPatternObj, parentMatrix) {
  CPDF_Dictionary* pDict = m_pPatternObj->GetDict();
  m_Pattern2Form = pDict->GetMatrixBy("Matrix");
  m_bColored = pDict->GetIntegerBy("PaintType") == 1;
  m_Pattern2Form.Concat(parentMatrix);
}

CPDF_TilingPattern::~CPDF_TilingPattern() {
}

CPDF_TilingPattern* CPDF_TilingPattern::AsTilingPattern() {
  return this;
}

CPDF_ShadingPattern* CPDF_TilingPattern::AsShadingPattern() {
  return nullptr;
}

FX_BOOL CPDF_TilingPattern::Load() {
  if (m_pForm)
    return TRUE;

  CPDF_Dictionary* pDict = m_pPatternObj->GetDict();
  if (!pDict)
    return FALSE;

  m_bColored = pDict->GetIntegerBy("PaintType") == 1;
  m_XStep = (FX_FLOAT)FXSYS_fabs(pDict->GetNumberBy("XStep"));
  m_YStep = (FX_FLOAT)FXSYS_fabs(pDict->GetNumberBy("YStep"));

  CPDF_Stream* pStream = m_pPatternObj->AsStream();
  if (!pStream)
    return FALSE;

  m_pForm.reset(new CPDF_Form(m_pDocument, nullptr, pStream));
  m_pForm->ParseContent(nullptr, &m_ParentMatrix, nullptr);
  m_BBox = pDict->GetRectBy("BBox");
  return TRUE;
}
