// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"

#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"

namespace {

ShadingType ToShadingType(int type) {
  return (type > static_cast<int>(kInvalidShading) &&
          type < static_cast<int>(kMaxShading))
             ? static_cast<ShadingType>(type)
             : kInvalidShading;
}

}  // namespace

CPDF_ShadingPattern::CPDF_ShadingPattern(CPDF_Document* pDoc,
                                         CPDF_Object* pPatternObj,
                                         FX_BOOL bShading,
                                         const CFX_Matrix& parentMatrix)
    : CPDF_Pattern(SHADING,
                   pDoc,
                   bShading ? nullptr : pPatternObj,
                   parentMatrix),
      m_ShadingType(kInvalidShading),
      m_bShadingObj(bShading),
      m_pShadingObj(pPatternObj),
      m_pCS(nullptr),
      m_pCountedCS(nullptr) {
  if (!bShading) {
    CPDF_Dictionary* pDict = m_pPatternObj->GetDict();
    m_Pattern2Form = pDict->GetMatrixBy("Matrix");
    m_pShadingObj = pDict->GetDirectObjectBy("Shading");
    m_Pattern2Form.Concat(parentMatrix);
  }
}

CPDF_ShadingPattern::~CPDF_ShadingPattern() {
  CPDF_ColorSpace* pCS = m_pCountedCS ? m_pCountedCS->get() : nullptr;
  if (pCS && m_pDocument)
    m_pDocument->GetPageData()->ReleaseColorSpace(pCS->GetArray());
}

CPDF_TilingPattern* CPDF_ShadingPattern::AsTilingPattern() {
  return nullptr;
}

CPDF_ShadingPattern* CPDF_ShadingPattern::AsShadingPattern() {
  return this;
}

bool CPDF_ShadingPattern::Load() {
  if (m_ShadingType != kInvalidShading)
    return TRUE;

  CPDF_Dictionary* pShadingDict =
      m_pShadingObj ? m_pShadingObj->GetDict() : nullptr;
  if (!pShadingDict)
    return FALSE;

  m_pFunctions.clear();
  CPDF_Object* pFunc = pShadingDict->GetDirectObjectBy("Function");
  if (pFunc) {
    if (CPDF_Array* pArray = pFunc->AsArray()) {
      m_pFunctions.resize(std::min<size_t>(pArray->GetCount(), 4));
      for (size_t i = 0; i < m_pFunctions.size(); ++i)
        m_pFunctions[i] = CPDF_Function::Load(pArray->GetDirectObjectAt(i));
    } else {
      m_pFunctions.push_back(CPDF_Function::Load(pFunc));
    }
  }
  CPDF_Object* pCSObj = pShadingDict->GetDirectObjectBy("ColorSpace");
  if (!pCSObj)
    return FALSE;

  CPDF_DocPageData* pDocPageData = m_pDocument->GetPageData();
  m_pCS = pDocPageData->GetColorSpace(pCSObj, nullptr);
  if (m_pCS)
    m_pCountedCS = pDocPageData->FindColorSpacePtr(m_pCS->GetArray());

  m_ShadingType = ToShadingType(pShadingDict->GetIntegerBy("ShadingType"));

  // We expect to have a stream if our shading type is a mesh.
  if (IsMeshShading() && !ToStream(m_pShadingObj))
    return FALSE;

  return TRUE;
}
