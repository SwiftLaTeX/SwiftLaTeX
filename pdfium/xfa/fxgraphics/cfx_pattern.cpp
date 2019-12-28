// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cfx_pattern.h"

CFX_Pattern::CFX_Pattern(FX_HatchStyle hatchStyle,
                         const FX_ARGB foreArgb,
                         const FX_ARGB backArgb,
                         CFX_Matrix* matrix)
    : m_hatchStyle(hatchStyle), m_foreArgb(foreArgb), m_backArgb(backArgb) {
  ASSERT(m_hatchStyle >= FX_HATCHSTYLE_Horizontal &&
         m_hatchStyle <= FX_HATCHSTYLE_SolidDiamond);

  if (matrix) {
    // TODO(dsinclair): Add a Set(const CFX_Matrix&) method. pdfium:436
    m_matrix.Set(matrix->a, matrix->b, matrix->c, matrix->d, matrix->e,
                 matrix->f);
  } else {
    m_matrix.SetIdentity();
  }
}

CFX_Pattern::~CFX_Pattern() {}
