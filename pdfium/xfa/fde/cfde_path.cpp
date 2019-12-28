// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_path.h"

#include "xfa/fde/fde_object.h"

FX_BOOL CFDE_Path::StartFigure() {
  return CloseFigure();
}

FX_BOOL CFDE_Path::CloseFigure() {
  FX_PATHPOINT* pPoint = GetLastPoint();
  if (pPoint)
    pPoint->m_Flag |= FXPT_CLOSEFIGURE;
  return TRUE;
}

FX_PATHPOINT* CFDE_Path::GetLastPoint(int32_t iCount) const {
  if (iCount < 1)
    return nullptr;

  int32_t iPoints = m_Path.GetPointCount();
  if (iCount > iPoints)
    return nullptr;
  return m_Path.GetPoints() + iPoints - iCount;
}

FX_BOOL CFDE_Path::FigureClosed() const {
  FX_PATHPOINT* pPoint = GetLastPoint();
  return pPoint ? (pPoint->m_Flag & FXPT_CLOSEFIGURE) : TRUE;
}

FX_PATHPOINT* CFDE_Path::AddPoints(int32_t iCount) {
  if (iCount < 1)
    return nullptr;

  int32_t iPoints = m_Path.GetPointCount();
  m_Path.AddPointCount(iCount);
  return m_Path.GetPoints() + iPoints;
}

void CFDE_Path::MoveTo(FX_FLOAT fx, FX_FLOAT fy) {
  FX_PATHPOINT* pPoint = AddPoints(1);
  pPoint->m_PointX = fx;
  pPoint->m_PointY = fy;
  pPoint->m_Flag = FXPT_MOVETO;
}

void CFDE_Path::LineTo(FX_FLOAT fx, FX_FLOAT fy) {
  FX_PATHPOINT* pPoint = AddPoints(1);
  pPoint->m_PointX = fx;
  pPoint->m_PointY = fy;
  pPoint->m_Flag = FXPT_LINETO;
}

void CFDE_Path::BezierTo(const CFX_PointF& p1,
                         const CFX_PointF& p2,
                         const CFX_PointF& p3) {
  FX_PATHPOINT* p = AddPoints(3);
  p[0].m_PointX = p1.x;
  p[0].m_PointY = p1.y;
  p[0].m_Flag = FXPT_BEZIERTO;
  p[1].m_PointX = p2.x;
  p[1].m_PointY = p2.y;
  p[1].m_Flag = FXPT_BEZIERTO;
  p[2].m_PointX = p3.x;
  p[2].m_PointY = p3.y;
  p[2].m_Flag = FXPT_BEZIERTO;
}

void CFDE_Path::ArcTo(FX_BOOL bStart,
                      const CFX_RectF& rect,
                      FX_FLOAT startAngle,
                      FX_FLOAT endAngle) {
  FX_FLOAT rx = rect.width / 2;
  FX_FLOAT ry = rect.height / 2;
  FX_FLOAT cx = rect.left + rx;
  FX_FLOAT cy = rect.top + ry;
  FX_FLOAT alpha =
      FXSYS_atan2(rx * FXSYS_sin(startAngle), ry * FXSYS_cos(startAngle));
  FX_FLOAT beta =
      FXSYS_atan2(rx * FXSYS_sin(endAngle), ry * FXSYS_cos(endAngle));
  if (FXSYS_fabs(beta - alpha) > FX_PI) {
    if (beta > alpha)
      beta -= 2 * FX_PI;
    else
      alpha -= 2 * FX_PI;
  }
  FX_FLOAT half_delta = (beta - alpha) / 2;
  FX_FLOAT bcp = 4.0f / 3 * (1 - FXSYS_cos(half_delta)) / FXSYS_sin(half_delta);
  FX_FLOAT sin_alpha = FXSYS_sin(alpha);
  FX_FLOAT sin_beta = FXSYS_sin(beta);
  FX_FLOAT cos_alpha = FXSYS_cos(alpha);
  FX_FLOAT cos_beta = FXSYS_cos(beta);
  if (bStart)
    MoveTo(CFX_PointF(cx + rx * cos_alpha, cy + ry * sin_alpha));

  BezierTo(CFX_PointF(cx + rx * (cos_alpha - bcp * sin_alpha),
                      cy + ry * (sin_alpha + bcp * cos_alpha)),
           CFX_PointF(cx + rx * (cos_beta + bcp * sin_beta),
                      cy + ry * (sin_beta - bcp * cos_beta)),
           CFX_PointF(cx + rx * cos_beta, cy + ry * sin_beta));
}

void CFDE_Path::AddBezier(const CFX_PointsF& points) {
  if (points.GetSize() != 4)
    return;

  const CFX_PointF* p = points.GetData();
  MoveTo(p[0]);
  BezierTo(p[1], p[2], p[3]);
}

void CFDE_Path::AddBeziers(const CFX_PointsF& points) {
  int32_t iCount = points.GetSize();
  if (iCount < 4)
    return;

  const CFX_PointF* p = points.GetData();
  const CFX_PointF* pEnd = p + iCount;
  MoveTo(p[0]);
  for (++p; p <= pEnd - 3; p += 3)
    BezierTo(p[0], p[1], p[2]);
}

void CFDE_Path::GetCurveTangents(const CFX_PointsF& points,
                                 CFX_PointsF& tangents,
                                 FX_BOOL bClosed,
                                 FX_FLOAT fTension) const {
  int32_t iCount = points.GetSize();
  tangents.SetSize(iCount);
  if (iCount < 3)
    return;

  FX_FLOAT fCoefficient = fTension / 3.0f;
  const CFX_PointF* pPoints = points.GetData();
  CFX_PointF* pTangents = tangents.GetData();
  for (int32_t i = 0; i < iCount; ++i) {
    int32_t r = i + 1;
    int32_t s = i - 1;
    if (r >= iCount)
      r = bClosed ? (r - iCount) : (iCount - 1);
    if (s < 0)
      s = bClosed ? (s + iCount) : 0;

    pTangents[i].x += (fCoefficient * (pPoints[r].x - pPoints[s].x));
    pTangents[i].y += (fCoefficient * (pPoints[r].y - pPoints[s].y));
  }
}

void CFDE_Path::AddCurve(const CFX_PointsF& points,
                         FX_BOOL bClosed,
                         FX_FLOAT fTension) {
  int32_t iLast = points.GetUpperBound();
  if (iLast < 1)
    return;

  CFX_PointsF tangents;
  GetCurveTangents(points, tangents, bClosed, fTension);
  const CFX_PointF* pPoints = points.GetData();
  CFX_PointF* pTangents = tangents.GetData();
  MoveTo(pPoints[0]);
  for (int32_t i = 0; i < iLast; ++i) {
    BezierTo(CFX_PointF(pPoints[i].x + pTangents[i].x,
                        pPoints[i].y + pTangents[i].y),
             CFX_PointF(pPoints[i + 1].x - pTangents[i + 1].x,
                        pPoints[i + 1].y - pTangents[i + 1].y),
             CFX_PointF(pPoints[i + 1].x, pPoints[i + 1].y));
  }
  if (bClosed) {
    BezierTo(CFX_PointF(pPoints[iLast].x + pTangents[iLast].x,
                        pPoints[iLast].y + pTangents[iLast].y),
             CFX_PointF(pPoints[0].x - pTangents[0].x,
                        pPoints[0].y - pTangents[0].y),
             CFX_PointF(pPoints[0].x, pPoints[0].y));
    CloseFigure();
  }
}

void CFDE_Path::AddEllipse(const CFX_RectF& rect) {
  FX_FLOAT fStartAngle = 0;
  FX_FLOAT fEndAngle = FX_PI / 2;
  for (int32_t i = 0; i < 4; ++i) {
    ArcTo(i == 0, rect, fStartAngle, fEndAngle);
    fStartAngle += FX_PI / 2;
    fEndAngle += FX_PI / 2;
  }
  CloseFigure();
}

void CFDE_Path::AddLine(const CFX_PointF& pt1, const CFX_PointF& pt2) {
  FX_PATHPOINT* pLast = GetLastPoint();
  if (!pLast || FXSYS_fabs(pLast->m_PointX - pt1.x) > 0.001 ||
      FXSYS_fabs(pLast->m_PointY - pt1.y) > 0.001) {
    MoveTo(pt1);
  }
  LineTo(pt2);
}

void CFDE_Path::AddPath(const CFDE_Path* pSrc, FX_BOOL bConnect) {
  if (!pSrc)
    return;

  int32_t iCount = pSrc->m_Path.GetPointCount();
  if (iCount < 1)
    return;
  if (bConnect)
    LineTo(pSrc->m_Path.GetPointX(0), pSrc->m_Path.GetPointY(0));

  m_Path.Append(&pSrc->m_Path, nullptr);
}

void CFDE_Path::AddPolygon(const CFX_PointsF& points) {
  int32_t iCount = points.GetSize();
  if (iCount < 2)
    return;

  AddLines(points);
  const CFX_PointF* p = points.GetData();
  if (FXSYS_fabs(p[0].x - p[iCount - 1].x) < 0.01f ||
      FXSYS_fabs(p[0].y - p[iCount - 1].y) < 0.01f) {
    LineTo(p[0]);
  }
  CloseFigure();
}

void CFDE_Path::AddLines(const CFX_PointsF& points) {
  int32_t iCount = points.GetSize();
  if (iCount < 2)
    return;

  const CFX_PointF* p = points.GetData();
  const CFX_PointF* pEnd = p + iCount;
  MoveTo(p[0]);
  for (++p; p < pEnd; ++p)
    LineTo(*p);
}

void CFDE_Path::AddRectangle(const CFX_RectF& rect) {
  MoveTo(rect.TopLeft());
  LineTo(rect.TopRight());
  LineTo(rect.BottomRight());
  LineTo(rect.BottomLeft());
  CloseFigure();
}

void CFDE_Path::GetBBox(CFX_RectF& bbox) const {
  CFX_FloatRect rect = m_Path.GetBoundingBox();
  bbox.Set(rect.left, rect.top, rect.Width(), rect.Height());
  bbox.Normalize();
}

void CFDE_Path::GetBBox(CFX_RectF& bbox,
                        FX_FLOAT fLineWidth,
                        FX_FLOAT fMiterLimit) const {
  CFX_FloatRect rect = m_Path.GetBoundingBox(fLineWidth, fMiterLimit);
  bbox.Set(rect.left, rect.top, rect.Width(), rect.Height());
  bbox.Normalize();
}
