// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cfx_path_generator.h"

CFX_PathGenerator::CFX_PathGenerator() : m_pPathData(nullptr) {}

void CFX_PathGenerator::Create() {
  m_pPathData = new CFX_PathData;
}

CFX_PathGenerator::~CFX_PathGenerator() {
  delete m_pPathData;
}

void CFX_PathGenerator::AddPathData(CFX_PathData* pPathData) {
  if (pPathData && pPathData->GetPointCount() > 0) {
    int nCount = pPathData->GetPointCount();
    FX_PATHPOINT* pPoints = pPathData->GetPoints();
    AddPathData(pPoints, nCount);
  }
}
void CFX_PathGenerator::AddPathData(FX_PATHPOINT* pPoints, int nCount) {
  if (pPoints && nCount > 0) {
    int nOldCount = m_pPathData->GetPointCount();
    m_pPathData->AddPointCount(nCount);
    FX_PATHPOINT* pDstPoints = m_pPathData->GetPoints();
    FXSYS_memcpy(pDstPoints + nOldCount, pPoints,
                 sizeof(FX_PATHPOINT) * nCount);
  }
}
void CFX_PathGenerator::MoveTo(FX_FLOAT x, FX_FLOAT y) {
  m_pPathData->AddPointCount(1);
  m_pPathData->SetPoint(m_pPathData->GetPointCount() - 1, x, y, FXPT_MOVETO);
}
void CFX_PathGenerator::LineTo(FX_FLOAT x, FX_FLOAT y) {
  m_pPathData->AddPointCount(1);
  m_pPathData->SetPoint(m_pPathData->GetPointCount() - 1, x, y, FXPT_LINETO);
}
void CFX_PathGenerator::BezierTo(FX_FLOAT ctrl_x1,
                                 FX_FLOAT ctrl_y1,
                                 FX_FLOAT ctrl_x2,
                                 FX_FLOAT ctrl_y2,
                                 FX_FLOAT to_x,
                                 FX_FLOAT to_y) {
  int old_count = m_pPathData->GetPointCount();
  m_pPathData->AddPointCount(3);
  m_pPathData->SetPoint(old_count, ctrl_x1, ctrl_y1, FXPT_BEZIERTO);
  m_pPathData->SetPoint(old_count + 1, ctrl_x2, ctrl_y2, FXPT_BEZIERTO);
  m_pPathData->SetPoint(old_count + 2, to_x, to_y, FXPT_BEZIERTO);
}
void CFX_PathGenerator::Close() {
  if (m_pPathData->GetPointCount() > 0) {
    int index = m_pPathData->GetPointCount() - 1;
    FX_PATHPOINT* pPoints = m_pPathData->GetPoints();
    pPoints[index].m_Flag |= FXPT_CLOSEFIGURE;
  }
}
void CFX_PathGenerator::AddLine(FX_FLOAT x1,
                                FX_FLOAT y1,
                                FX_FLOAT x2,
                                FX_FLOAT y2) {
  int old_count = m_pPathData->GetPointCount();
  m_pPathData->AddPointCount(2);
  m_pPathData->SetPoint(old_count, x1, y1, FXPT_MOVETO);
  m_pPathData->SetPoint(old_count + 1, x2, y2, FXPT_LINETO);
}
void CFX_PathGenerator::AddBezier(FX_FLOAT start_x,
                                  FX_FLOAT start_y,
                                  FX_FLOAT ctrl_x1,
                                  FX_FLOAT ctrl_y1,
                                  FX_FLOAT ctrl_x2,
                                  FX_FLOAT ctrl_y2,
                                  FX_FLOAT end_x,
                                  FX_FLOAT end_y) {
  int old_count = m_pPathData->GetPointCount();
  m_pPathData->AddPointCount(4);
  m_pPathData->SetPoint(old_count, start_x, start_y, FXPT_MOVETO);
  m_pPathData->SetPoint(old_count + 1, ctrl_x1, ctrl_y1, FXPT_BEZIERTO);
  m_pPathData->SetPoint(old_count + 2, ctrl_x2, ctrl_y2, FXPT_BEZIERTO);
  m_pPathData->SetPoint(old_count + 3, end_x, end_y, FXPT_BEZIERTO);
}
void CFX_PathGenerator::AddRectangle(FX_FLOAT x1,
                                     FX_FLOAT y1,
                                     FX_FLOAT x2,
                                     FX_FLOAT y2) {
  m_pPathData->AppendRect(x1, y1, x2, y2);
}
void CFX_PathGenerator::AddEllipse(FX_FLOAT x,
                                   FX_FLOAT y,
                                   FX_FLOAT width,
                                   FX_FLOAT height) {
  AddArc(x, y, width, height, 0, FX_PI * 2);
}
void CFX_PathGenerator::ArcTo(FX_FLOAT x,
                              FX_FLOAT y,
                              FX_FLOAT width,
                              FX_FLOAT height,
                              FX_FLOAT start_angle,
                              FX_FLOAT sweep_angle) {
  FX_FLOAT x0 = FXSYS_cos(sweep_angle / 2);
  FX_FLOAT y0 = FXSYS_sin(sweep_angle / 2);
  FX_FLOAT tx = ((1.0f - x0) * 4) / (3 * 1.0f);
  FX_FLOAT ty = y0 - ((tx * x0) / y0);
  FX_FLOAT px[3], py[3];
  px[0] = x0 + tx;
  py[0] = -ty;
  px[1] = x0 + tx;
  py[1] = ty;
  FX_FLOAT sn = FXSYS_sin(start_angle + sweep_angle / 2);
  FX_FLOAT cs = FXSYS_cos(start_angle + sweep_angle / 2);
  int old_count = m_pPathData->GetPointCount();
  m_pPathData->AddPointCount(3);
  FX_FLOAT bezier_x, bezier_y;
  bezier_x = x + (width * ((px[0] * cs) - (py[0] * sn)));
  bezier_y = y + (height * ((px[0] * sn) + (py[0] * cs)));
  m_pPathData->SetPoint(old_count, bezier_x, bezier_y, FXPT_BEZIERTO);
  bezier_x = x + (width * ((px[1] * cs) - (py[1] * sn)));
  bezier_y = y + (height * ((px[1] * sn) + (py[1] * cs)));
  m_pPathData->SetPoint(old_count + 1, bezier_x, bezier_y, FXPT_BEZIERTO);
  bezier_x = x + (width * FXSYS_cos(start_angle + sweep_angle));
  bezier_y = y + (height * FXSYS_sin(start_angle + sweep_angle));
  m_pPathData->SetPoint(old_count + 2, bezier_x, bezier_y, FXPT_BEZIERTO);
}
void CFX_PathGenerator::AddArc(FX_FLOAT x,
                               FX_FLOAT y,
                               FX_FLOAT width,
                               FX_FLOAT height,
                               FX_FLOAT start_angle,
                               FX_FLOAT sweep_angle) {
  if (sweep_angle == 0) {
    return;
  }

  const FX_FLOAT bezier_arc_angle_epsilon = 0.01f;
  while (start_angle > FX_PI * 2) {
    start_angle -= FX_PI * 2;
  }
  while (start_angle < 0) {
    start_angle += FX_PI * 2;
  }
  if (sweep_angle >= FX_PI * 2) {
    sweep_angle = FX_PI * 2;
  }
  if (sweep_angle <= -FX_PI * 2) {
    sweep_angle = -FX_PI * 2;
  }
  m_pPathData->AddPointCount(1);
  m_pPathData->SetPoint(m_pPathData->GetPointCount() - 1,
                        x + (width * FXSYS_cos(start_angle)),
                        y + (height * FXSYS_sin(start_angle)), FXPT_MOVETO);
  FX_FLOAT total_sweep = 0, local_sweep = 0, prev_sweep = 0;
  FX_BOOL done = FALSE;
  do {
    if (sweep_angle < 0) {
      prev_sweep = total_sweep;
      local_sweep = -FX_PI / 2;
      total_sweep -= FX_PI / 2;
      if (total_sweep <= sweep_angle + bezier_arc_angle_epsilon) {
        local_sweep = sweep_angle - prev_sweep;
        done = TRUE;
      }
    } else {
      prev_sweep = total_sweep;
      local_sweep = FX_PI / 2;
      total_sweep += FX_PI / 2;
      if (total_sweep >= sweep_angle - bezier_arc_angle_epsilon) {
        local_sweep = sweep_angle - prev_sweep;
        done = TRUE;
      }
    }
    ArcTo(x, y, width, height, start_angle, local_sweep);
    start_angle += local_sweep;
  } while (!done);
}
void CFX_PathGenerator::AddPie(FX_FLOAT x,
                               FX_FLOAT y,
                               FX_FLOAT width,
                               FX_FLOAT height,
                               FX_FLOAT start_angle,
                               FX_FLOAT sweep_angle) {
  if (sweep_angle == 0) {
    int old_count = m_pPathData->GetPointCount();
    m_pPathData->AddPointCount(2);
    m_pPathData->SetPoint(old_count, x, y, FXPT_MOVETO);
    m_pPathData->SetPoint(old_count + 1, x + (width * FXSYS_cos(start_angle)),
                          y + (height * FXSYS_sin(start_angle)), FXPT_LINETO);
    return;
  }
  AddArc(x, y, width, height, start_angle, sweep_angle);
  m_pPathData->AddPointCount(1);
  m_pPathData->SetPoint(m_pPathData->GetPointCount() - 1, x, y,
                        FXPT_LINETO | FXPT_CLOSEFIGURE);
}
