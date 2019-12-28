// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_pathobject.h"

CPDF_PathObject::CPDF_PathObject() {}

CPDF_PathObject::~CPDF_PathObject() {}

CPDF_PathObject* CPDF_PathObject::Clone() const {
  CPDF_PathObject* obj = new CPDF_PathObject;
  obj->CopyData(this);

  obj->m_Path = m_Path;
  obj->m_FillType = m_FillType;
  obj->m_bStroke = m_bStroke;
  obj->m_Matrix = m_Matrix;
  return obj;
}

CPDF_PageObject::Type CPDF_PathObject::GetType() const {
  return PATH;
}

void CPDF_PathObject::Transform(const CFX_Matrix& matrix) {
  m_Matrix.Concat(matrix);
  CalcBoundingBox();
}

bool CPDF_PathObject::IsPath() const {
  return true;
}

CPDF_PathObject* CPDF_PathObject::AsPath() {
  return this;
}

const CPDF_PathObject* CPDF_PathObject::AsPath() const {
  return this;
}

void CPDF_PathObject::CalcBoundingBox() {
  if (m_Path.IsNull()) {
    return;
  }
  CFX_FloatRect rect;
  FX_FLOAT width = m_GraphState.GetObject()->m_LineWidth;
  if (m_bStroke && width != 0) {
    rect = m_Path.GetBoundingBox(width, m_GraphState.GetObject()->m_MiterLimit);
  } else {
    rect = m_Path.GetBoundingBox();
  }
  rect.Transform(&m_Matrix);
  if (width == 0 && m_bStroke) {
    rect.left += -0.5f;
    rect.right += 0.5f;
    rect.bottom += -0.5f;
    rect.top += 0.5f;
  }
  m_Left = rect.left;
  m_Right = rect.right;
  m_Top = rect.top;
  m_Bottom = rect.bottom;
}
