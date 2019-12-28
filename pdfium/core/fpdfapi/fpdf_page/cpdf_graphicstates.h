// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_GRAPHICSTATES_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_GRAPHICSTATES_H_

#include "core/fpdfapi/fpdf_page/cpdf_colorstate.h"
#include "core/fpdfapi/fpdf_page/cpdf_graphstate.h"
#include "core/fpdfapi/fpdf_page/cpdf_textstate.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_clippath.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_generalstate.h"

class CPDF_GraphicStates {
 public:
  CPDF_GraphicStates();
  virtual ~CPDF_GraphicStates();

  void CopyStates(const CPDF_GraphicStates& src);
  void DefaultStates();

  CPDF_ClipPath m_ClipPath;
  CPDF_GraphState m_GraphState;
  CPDF_ColorState m_ColorState;
  CPDF_TextState m_TextState;
  CPDF_GeneralState m_GeneralState;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_GRAPHICSTATES_H_
