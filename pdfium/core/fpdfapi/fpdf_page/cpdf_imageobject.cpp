// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"

CPDF_ImageObject::CPDF_ImageObject() : m_pImage(nullptr) {}

CPDF_ImageObject::~CPDF_ImageObject() {
  if (!m_pImage) {
    return;
  }
  if (m_pImage->IsInline() ||
      (m_pImage->GetStream() && m_pImage->GetStream()->GetObjNum() == 0)) {
    delete m_pImage;
  } else {
    m_pImage->GetDocument()->GetPageData()->ReleaseImage(m_pImage->GetStream());
  }
}

CPDF_ImageObject* CPDF_ImageObject::Clone() const {
  CPDF_ImageObject* obj = new CPDF_ImageObject;
  obj->CopyData(this);

  obj->m_pImage = m_pImage->Clone();
  obj->m_Matrix = m_Matrix;
  return obj;
}

CPDF_PageObject::Type CPDF_ImageObject::GetType() const {
  return IMAGE;
}

void CPDF_ImageObject::Transform(const CFX_Matrix& matrix) {
  m_Matrix.Concat(matrix);
  CalcBoundingBox();
}

bool CPDF_ImageObject::IsImage() const {
  return true;
}

CPDF_ImageObject* CPDF_ImageObject::AsImage() {
  return this;
}

const CPDF_ImageObject* CPDF_ImageObject::AsImage() const {
  return this;
}

void CPDF_ImageObject::CalcBoundingBox() {
  m_Left = m_Bottom = 0;
  m_Right = m_Top = 1.0f;
  m_Matrix.TransformRect(m_Left, m_Right, m_Top, m_Bottom);
}
