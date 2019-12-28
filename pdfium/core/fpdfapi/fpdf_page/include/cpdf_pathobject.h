// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATHOBJECT_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATHOBJECT_H_

#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_path.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_PathObject : public CPDF_PageObject {
 public:
  CPDF_PathObject();
  ~CPDF_PathObject() override;

  // CPDF_PageObject
  CPDF_PathObject* Clone() const override;
  Type GetType() const override;
  void Transform(const CFX_Matrix& maxtrix) override;
  bool IsPath() const override;
  CPDF_PathObject* AsPath() override;
  const CPDF_PathObject* AsPath() const override;

  void CalcBoundingBox();

  CPDF_Path m_Path;
  int m_FillType;
  FX_BOOL m_bStroke;
  CFX_Matrix m_Matrix;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PATHOBJECT_H_
