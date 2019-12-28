// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cfx_shading.h"

CFX_Shading::CFX_Shading(const CFX_PointF& beginPoint,
                         const CFX_PointF& endPoint,
                         FX_BOOL isExtendedBegin,
                         FX_BOOL isExtendedEnd,
                         const FX_ARGB beginArgb,
                         const FX_ARGB endArgb)
    : m_type(FX_SHADING_Axial),
      m_beginPoint(beginPoint),
      m_endPoint(endPoint),
      m_beginRadius(0),
      m_endRadius(0),
      m_isExtendedBegin(isExtendedBegin),
      m_isExtendedEnd(isExtendedEnd),
      m_beginArgb(beginArgb),
      m_endArgb(endArgb) {
  InitArgbArray();
}

CFX_Shading::CFX_Shading(const CFX_PointF& beginPoint,
                         const CFX_PointF& endPoint,
                         const FX_FLOAT beginRadius,
                         const FX_FLOAT endRadius,
                         FX_BOOL isExtendedBegin,
                         FX_BOOL isExtendedEnd,
                         const FX_ARGB beginArgb,
                         const FX_ARGB endArgb)
    : m_type(FX_SHADING_Radial),
      m_beginPoint(beginPoint),
      m_endPoint(endPoint),
      m_beginRadius(beginRadius),
      m_endRadius(endRadius),
      m_isExtendedBegin(isExtendedBegin),
      m_isExtendedEnd(isExtendedEnd),
      m_beginArgb(beginArgb),
      m_endArgb(endArgb) {
  InitArgbArray();
}

CFX_Shading::~CFX_Shading() {}

void CFX_Shading::InitArgbArray() {
  int32_t a1;
  int32_t r1;
  int32_t g1;
  int32_t b1;
  ArgbDecode(m_beginArgb, a1, r1, g1, b1);

  int32_t a2;
  int32_t r2;
  int32_t g2;
  int32_t b2;
  ArgbDecode(m_endArgb, a2, r2, g2, b2);

  FX_FLOAT f = (FX_FLOAT)(FX_SHADING_Steps - 1);
  FX_FLOAT aScale = 1.0 * (a2 - a1) / f;
  FX_FLOAT rScale = 1.0 * (r2 - r1) / f;
  FX_FLOAT gScale = 1.0 * (g2 - g1) / f;
  FX_FLOAT bScale = 1.0 * (b2 - b1) / f;

  for (int32_t i = 0; i < FX_SHADING_Steps; i++) {
    int32_t a3 = static_cast<int32_t>(i * aScale);
    int32_t r3 = static_cast<int32_t>(i * rScale);
    int32_t g3 = static_cast<int32_t>(i * gScale);
    int32_t b3 = static_cast<int32_t>(i * bScale);

    // TODO(dsinclair): Add overloads for FX_ARGB. pdfium:437
    m_argbArray[i] =
        FXARGB_TODIB(FXARGB_MAKE(a1 + a3, r1 + r3, g1 + g3, b1 + b3));
  }
}
