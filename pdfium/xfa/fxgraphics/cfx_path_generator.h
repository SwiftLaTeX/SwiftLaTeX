// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CFX_PATH_GENERATOR_H_
#define XFA_FXGRAPHICS_CFX_PATH_GENERATOR_H_

#include "core/fxge/include/fx_ge.h"

class CFX_PathGenerator {
 public:
  CFX_PathGenerator();
  ~CFX_PathGenerator();

  void Create();
  CFX_PathData* GetPathData() const { return m_pPathData; }

  void AddPathData(CFX_PathData* path_data);
  void AddPathData(FX_PATHPOINT* points, int count);

  void MoveTo(FX_FLOAT x, FX_FLOAT y);
  void LineTo(FX_FLOAT x, FX_FLOAT y);
  void BezierTo(FX_FLOAT ctrl_x1,
                FX_FLOAT ctrl_y1,
                FX_FLOAT ctrl_x2,
                FX_FLOAT ctrl_y2,
                FX_FLOAT to_x,
                FX_FLOAT to_y);
  void Close();
  void ArcTo(FX_FLOAT x,
             FX_FLOAT y,
             FX_FLOAT width,
             FX_FLOAT height,
             FX_FLOAT start_angle,
             FX_FLOAT sweep_angle);

  void AddLine(FX_FLOAT x1, FX_FLOAT y1, FX_FLOAT x2, FX_FLOAT y2);
  void AddBezier(FX_FLOAT start_x,
                 FX_FLOAT start_y,
                 FX_FLOAT ctrl_x1,
                 FX_FLOAT ctrl_y1,
                 FX_FLOAT ctrl_x2,
                 FX_FLOAT ctrl_y2,
                 FX_FLOAT end_x,
                 FX_FLOAT end_y);
  void AddRectangle(FX_FLOAT x1, FX_FLOAT y1, FX_FLOAT x2, FX_FLOAT y2);
  void AddEllipse(FX_FLOAT x, FX_FLOAT y, FX_FLOAT width, FX_FLOAT height);
  void AddArc(FX_FLOAT x,
              FX_FLOAT y,
              FX_FLOAT width,
              FX_FLOAT height,
              FX_FLOAT start_angle,
              FX_FLOAT sweep_angle);
  void AddPie(FX_FLOAT x,
              FX_FLOAT y,
              FX_FLOAT width,
              FX_FLOAT height,
              FX_FLOAT start_angle,
              FX_FLOAT sweep_angle);

 protected:
  CFX_PathData* m_pPathData;
};

#endif  // XFA_FXGRAPHICS_CFX_PATH_GENERATOR_H_
