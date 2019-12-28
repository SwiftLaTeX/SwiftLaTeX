// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CFX_PATH_H_
#define XFA_FXGRAPHICS_CFX_PATH_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

class CFX_PathData;
class CFX_PathGenerator;

class CFX_Path {
 public:
  CFX_Path();
  virtual ~CFX_Path();

  FWL_Error Create();
  FWL_Error MoveTo(FX_FLOAT x, FX_FLOAT y);
  FWL_Error LineTo(FX_FLOAT x, FX_FLOAT y);
  FWL_Error BezierTo(FX_FLOAT ctrlX1,
                     FX_FLOAT ctrlY1,
                     FX_FLOAT ctrlX2,
                     FX_FLOAT ctrlY2,
                     FX_FLOAT toX,
                     FX_FLOAT toY);
  FWL_Error ArcTo(FX_FLOAT left,
                  FX_FLOAT top,
                  FX_FLOAT width,
                  FX_FLOAT height,
                  FX_FLOAT startAngle,
                  FX_FLOAT sweepAngle);
  FWL_Error Close();

  FWL_Error AddLine(FX_FLOAT x1, FX_FLOAT y1, FX_FLOAT x2, FX_FLOAT y2);
  FWL_Error AddBezier(FX_FLOAT startX,
                      FX_FLOAT startY,
                      FX_FLOAT ctrlX1,
                      FX_FLOAT ctrlY1,
                      FX_FLOAT ctrlX2,
                      FX_FLOAT ctrlY2,
                      FX_FLOAT endX,
                      FX_FLOAT endY);
  FWL_Error AddRectangle(FX_FLOAT left,
                         FX_FLOAT top,
                         FX_FLOAT width,
                         FX_FLOAT height);
  FWL_Error AddEllipse(FX_FLOAT left,
                       FX_FLOAT top,
                       FX_FLOAT width,
                       FX_FLOAT height);
  FWL_Error AddEllipse(const CFX_RectF& rect);
  FWL_Error AddArc(FX_FLOAT left,
                   FX_FLOAT top,
                   FX_FLOAT width,
                   FX_FLOAT height,
                   FX_FLOAT startAngle,
                   FX_FLOAT sweepAngle);
  FWL_Error AddPie(FX_FLOAT left,
                   FX_FLOAT top,
                   FX_FLOAT width,
                   FX_FLOAT height,
                   FX_FLOAT startAngle,
                   FX_FLOAT sweepAngle);
  FWL_Error AddSubpath(CFX_Path* path);
  FWL_Error Clear();

  FX_BOOL IsEmpty();
  CFX_PathData* GetPathData();

 private:
  CFX_PathGenerator* m_generator;
};

#endif  // XFA_FXGRAPHICS_CFX_PATH_H_
