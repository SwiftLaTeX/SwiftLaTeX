// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CFX_SHADING_H_
#define XFA_FXGRAPHICS_CFX_SHADING_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"

#define FX_SHADING_Steps 256

enum CFX_Shading_Type { FX_SHADING_Axial = 1, FX_SHADING_Radial };

class CFX_Shading {
 public:
  // Axial shading.
  CFX_Shading(const CFX_PointF& beginPoint,
              const CFX_PointF& endPoint,
              FX_BOOL isExtendedBegin,
              FX_BOOL isExtendedEnd,
              const FX_ARGB beginArgb,
              const FX_ARGB endArgb);

  // Radial shading.
  CFX_Shading(const CFX_PointF& beginPoint,
              const CFX_PointF& endPoint,
              const FX_FLOAT beginRadius,
              const FX_FLOAT endRadius,
              FX_BOOL isExtendedBegin,
              FX_BOOL isExtendedEnd,
              const FX_ARGB beginArgb,
              const FX_ARGB endArgb);
  virtual ~CFX_Shading();

 private:
  friend class CFX_Graphics;

  void InitArgbArray();

  const CFX_Shading_Type m_type;
  const CFX_PointF m_beginPoint;
  const CFX_PointF m_endPoint;
  const FX_FLOAT m_beginRadius;
  const FX_FLOAT m_endRadius;
  const FX_BOOL m_isExtendedBegin;
  const FX_BOOL m_isExtendedEnd;
  const FX_ARGB m_beginArgb;
  const FX_ARGB m_endArgb;
  FX_ARGB m_argbArray[FX_SHADING_Steps];
};

#endif  // XFA_FXGRAPHICS_CFX_SHADING_H_
