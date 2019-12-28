// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_INCLUDE_CPVT_LINE_H_
#define CORE_FPDFDOC_INCLUDE_CPVT_LINE_H_

#include "core/fpdfdoc/include/cpvt_wordplace.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

struct CPVT_Line {
  CPVT_Line()
      : ptLine(0, 0), fLineWidth(0.0f), fLineAscent(0.0f), fLineDescent(0.0f) {}

  CPVT_WordPlace lineplace;
  CPVT_WordPlace lineEnd;
  CFX_FloatPoint ptLine;
  FX_FLOAT fLineWidth;
  FX_FLOAT fLineAscent;
  FX_FLOAT fLineDescent;
};

#endif  // CORE_FPDFDOC_INCLUDE_CPVT_LINE_H_
