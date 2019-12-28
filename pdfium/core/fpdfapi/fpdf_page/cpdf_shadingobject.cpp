// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_shadingobject.h"

#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"

CPDF_ShadingObject::CPDF_ShadingObject() : m_pShading(nullptr) {}

CPDF_ShadingObject::~CPDF_ShadingObject() {}

CPDF_ShadingObject* CPDF_ShadingObject::Clone() const {
  CPDF_ShadingObject* obj = new CPDF_ShadingObject;
  obj->CopyData(this);

  obj->m_pShading = m_pShading;
  if (obj->m_pShading && obj->m_pShading->document()) {
    CPDF_DocPageData* pDocPageData = obj->m_pShading->document()->GetPageData();
    CPDF_Pattern* pattern = pDocPageData->GetPattern(
        obj->m_pShading->GetShadingObject(), m_pShading->IsShadingObject(),
        obj->m_pShading->parent_matrix());
    obj->m_pShading = pattern ? pattern->AsShadingPattern() : nullptr;
  }
  obj->m_Matrix = m_Matrix;
  return obj;
}

CPDF_PageObject::Type CPDF_ShadingObject::GetType() const {
  return SHADING;
}

void CPDF_ShadingObject::Transform(const CFX_Matrix& matrix) {
  if (!m_ClipPath.IsNull()) {
    m_ClipPath.GetModify();
    m_ClipPath.Transform(matrix);
  }
  m_Matrix.Concat(matrix);
  if (!m_ClipPath.IsNull()) {
    CalcBoundingBox();
  } else {
    matrix.TransformRect(m_Left, m_Right, m_Top, m_Bottom);
  }
}

bool CPDF_ShadingObject::IsShading() const {
  return true;
}

CPDF_ShadingObject* CPDF_ShadingObject::AsShading() {
  return this;
}

const CPDF_ShadingObject* CPDF_ShadingObject::AsShading() const {
  return this;
}

void CPDF_ShadingObject::CalcBoundingBox() {
  if (m_ClipPath.IsNull()) {
    return;
  }
  CFX_FloatRect rect = m_ClipPath.GetClipBox();
  m_Left = rect.left;
  m_Bottom = rect.bottom;
  m_Right = rect.right;
  m_Top = rect.top;
}
