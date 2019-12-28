// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATH_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATH_H_

#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_ge.h"

class CPDF_Path : public CFX_CountRef<CFX_PathData> {
 public:
  int GetPointCount() const { return m_pObject->m_PointCount; }
  int GetFlag(int index) const { return m_pObject->m_pPoints[index].m_Flag; }

  FX_FLOAT GetPointX(int index) const {
    return m_pObject->m_pPoints[index].m_PointX;
  }

  FX_FLOAT GetPointY(int index) const {
    return m_pObject->m_pPoints[index].m_PointY;
  }

  FX_PATHPOINT* GetPoints() const { return m_pObject->m_pPoints; }
  CFX_FloatRect GetBoundingBox() const { return m_pObject->GetBoundingBox(); }

  CFX_FloatRect GetBoundingBox(FX_FLOAT line_width,
                               FX_FLOAT miter_limit) const {
    return m_pObject->GetBoundingBox(line_width, miter_limit);
  }

  FX_BOOL IsRect() const { return m_pObject->IsRect(); }

  void Transform(const CFX_Matrix* pMatrix) { GetModify()->Transform(pMatrix); }

  void Append(CPDF_Path src, const CFX_Matrix* pMatrix) {
    m_pObject->Append(src.m_pObject, pMatrix);
  }

  void AppendRect(FX_FLOAT left,
                  FX_FLOAT bottom,
                  FX_FLOAT right,
                  FX_FLOAT top) {
    m_pObject->AppendRect(left, bottom, right, top);
  }
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATH_H_
