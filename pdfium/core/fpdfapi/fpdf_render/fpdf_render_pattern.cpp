// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_render/render_int.h"

#include <algorithm>

#include "core/fpdfapi/fpdf_page/cpdf_graphicstates.h"
#include "core/fpdfapi/fpdf_page/cpdf_meshstream.h"
#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"
#include "core/fpdfapi/fpdf_page/cpdf_tilingpattern.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pathobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_shadingobject.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_rendercontext.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fxge/include/ifx_renderdevicedriver.h"

namespace {

uint32_t CountOutputs(
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs) {
  uint32_t total = 0;
  for (const auto& func : funcs) {
    if (func)
      total += func->CountOutputs();
  }
  return total;
}

#define SHADING_STEPS 256
void DrawAxialShading(CFX_DIBitmap* pBitmap,
                      CFX_Matrix* pObject2Bitmap,
                      CPDF_Dictionary* pDict,
                      const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                      CPDF_ColorSpace* pCS,
                      int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);
  CPDF_Array* pCoords = pDict->GetArrayBy("Coords");
  if (!pCoords) {
    return;
  }
  FX_FLOAT start_x = pCoords->GetNumberAt(0);
  FX_FLOAT start_y = pCoords->GetNumberAt(1);
  FX_FLOAT end_x = pCoords->GetNumberAt(2);
  FX_FLOAT end_y = pCoords->GetNumberAt(3);
  FX_FLOAT t_min = 0, t_max = 1.0f;
  CPDF_Array* pArray = pDict->GetArrayBy("Domain");
  if (pArray) {
    t_min = pArray->GetNumberAt(0);
    t_max = pArray->GetNumberAt(1);
  }
  FX_BOOL bStartExtend = FALSE, bEndExtend = FALSE;
  pArray = pDict->GetArrayBy("Extend");
  if (pArray) {
    bStartExtend = pArray->GetIntegerAt(0);
    bEndExtend = pArray->GetIntegerAt(1);
  }
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  FX_FLOAT x_span = end_x - start_x;
  FX_FLOAT y_span = end_y - start_y;
  FX_FLOAT axis_len_square = (x_span * x_span) + (y_span * y_span);
  CFX_Matrix matrix;
  matrix.SetReverse(*pObject2Bitmap);
  uint32_t total_results =
      std::max(CountOutputs(funcs), pCS->CountComponents());
  CFX_FixedBufGrow<FX_FLOAT, 16> result_array(total_results);
  FX_FLOAT* pResults = result_array;
  FXSYS_memset(pResults, 0, total_results * sizeof(FX_FLOAT));
  uint32_t rgb_array[SHADING_STEPS];
  for (int i = 0; i < SHADING_STEPS; i++) {
    FX_FLOAT input = (t_max - t_min) * i / SHADING_STEPS + t_min;
    int offset = 0;
    for (const auto& func : funcs) {
      if (func) {
        int nresults = 0;
        if (func->Call(&input, 1, pResults + offset, nresults))
          offset += nresults;
      }
    }
    FX_FLOAT R = 0.0f, G = 0.0f, B = 0.0f;
    pCS->GetRGB(pResults, R, G, B);
    rgb_array[i] =
        FXARGB_TODIB(FXARGB_MAKE(alpha, FXSYS_round(R * 255),
                                 FXSYS_round(G * 255), FXSYS_round(B * 255)));
  }
  int pitch = pBitmap->GetPitch();
  for (int row = 0; row < height; row++) {
    uint32_t* dib_buf = (uint32_t*)(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      FX_FLOAT x = (FX_FLOAT)column, y = (FX_FLOAT)row;
      matrix.Transform(x, y);
      FX_FLOAT scale = (((x - start_x) * x_span) + ((y - start_y) * y_span)) /
                       axis_len_square;
      int index = (int32_t)(scale * (SHADING_STEPS - 1));
      if (index < 0) {
        if (!bStartExtend) {
          continue;
        }
        index = 0;
      } else if (index >= SHADING_STEPS) {
        if (!bEndExtend) {
          continue;
        }
        index = SHADING_STEPS - 1;
      }
      dib_buf[column] = rgb_array[index];
    }
  }
}

void DrawRadialShading(CFX_DIBitmap* pBitmap,
                       CFX_Matrix* pObject2Bitmap,
                       CPDF_Dictionary* pDict,
                       const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                       CPDF_ColorSpace* pCS,
                       int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);
  CPDF_Array* pCoords = pDict->GetArrayBy("Coords");
  if (!pCoords) {
    return;
  }
  FX_FLOAT start_x = pCoords->GetNumberAt(0);
  FX_FLOAT start_y = pCoords->GetNumberAt(1);
  FX_FLOAT start_r = pCoords->GetNumberAt(2);
  FX_FLOAT end_x = pCoords->GetNumberAt(3);
  FX_FLOAT end_y = pCoords->GetNumberAt(4);
  FX_FLOAT end_r = pCoords->GetNumberAt(5);
  CFX_Matrix matrix;
  matrix.SetReverse(*pObject2Bitmap);
  FX_FLOAT t_min = 0, t_max = 1.0f;
  CPDF_Array* pArray = pDict->GetArrayBy("Domain");
  if (pArray) {
    t_min = pArray->GetNumberAt(0);
    t_max = pArray->GetNumberAt(1);
  }
  FX_BOOL bStartExtend = FALSE, bEndExtend = FALSE;
  pArray = pDict->GetArrayBy("Extend");
  if (pArray) {
    bStartExtend = pArray->GetIntegerAt(0);
    bEndExtend = pArray->GetIntegerAt(1);
  }
  uint32_t total_results =
      std::max(CountOutputs(funcs), pCS->CountComponents());
  CFX_FixedBufGrow<FX_FLOAT, 16> result_array(total_results);
  FX_FLOAT* pResults = result_array;
  FXSYS_memset(pResults, 0, total_results * sizeof(FX_FLOAT));
  uint32_t rgb_array[SHADING_STEPS];
  for (int i = 0; i < SHADING_STEPS; i++) {
    FX_FLOAT input = (t_max - t_min) * i / SHADING_STEPS + t_min;
    int offset = 0;
    for (const auto& func : funcs) {
      if (func) {
        int nresults;
        if (func->Call(&input, 1, pResults + offset, nresults))
          offset += nresults;
      }
    }
    FX_FLOAT R = 0.0f, G = 0.0f, B = 0.0f;
    pCS->GetRGB(pResults, R, G, B);
    rgb_array[i] =
        FXARGB_TODIB(FXARGB_MAKE(alpha, FXSYS_round(R * 255),
                                 FXSYS_round(G * 255), FXSYS_round(B * 255)));
  }
  FX_FLOAT a = ((start_x - end_x) * (start_x - end_x)) +
               ((start_y - end_y) * (start_y - end_y)) -
               ((start_r - end_r) * (start_r - end_r));
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  int pitch = pBitmap->GetPitch();
  FX_BOOL bDecreasing = FALSE;
  if (start_r > end_r) {
    int length = (int)FXSYS_sqrt((((start_x - end_x) * (start_x - end_x)) +
                                  ((start_y - end_y) * (start_y - end_y))));
    if (length < start_r - end_r) {
      bDecreasing = TRUE;
    }
  }
  for (int row = 0; row < height; row++) {
    uint32_t* dib_buf = (uint32_t*)(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      FX_FLOAT x = (FX_FLOAT)column, y = (FX_FLOAT)row;
      matrix.Transform(x, y);
      FX_FLOAT b = -2 * (((x - start_x) * (end_x - start_x)) +
                         ((y - start_y) * (end_y - start_y)) +
                         (start_r * (end_r - start_r)));
      FX_FLOAT c = ((x - start_x) * (x - start_x)) +
                   ((y - start_y) * (y - start_y)) - (start_r * start_r);
      FX_FLOAT s;
      if (a == 0) {
        s = -c / b;
      } else {
        FX_FLOAT b2_4ac = (b * b) - 4 * (a * c);
        if (b2_4ac < 0) {
          continue;
        }
        FX_FLOAT root = FXSYS_sqrt(b2_4ac);
        FX_FLOAT s1, s2;
        if (a > 0) {
          s1 = (-b - root) / (2 * a);
          s2 = (-b + root) / (2 * a);
        } else {
          s2 = (-b - root) / (2 * a);
          s1 = (-b + root) / (2 * a);
        }
        if (bDecreasing) {
          if (s1 >= 0 || bStartExtend) {
            s = s1;
          } else {
            s = s2;
          }
        } else {
          if (s2 <= 1.0f || bEndExtend) {
            s = s2;
          } else {
            s = s1;
          }
        }
        if ((start_r + s * (end_r - start_r)) < 0) {
          continue;
        }
      }
      int index = (int32_t)(s * (SHADING_STEPS - 1));
      if (index < 0) {
        if (!bStartExtend) {
          continue;
        }
        index = 0;
      }
      if (index >= SHADING_STEPS) {
        if (!bEndExtend) {
          continue;
        }
        index = SHADING_STEPS - 1;
      }
      dib_buf[column] = rgb_array[index];
    }
  }
}

void DrawFuncShading(CFX_DIBitmap* pBitmap,
                     CFX_Matrix* pObject2Bitmap,
                     CPDF_Dictionary* pDict,
                     const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
                     CPDF_ColorSpace* pCS,
                     int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);
  CPDF_Array* pDomain = pDict->GetArrayBy("Domain");
  FX_FLOAT xmin = 0, ymin = 0, xmax = 1.0f, ymax = 1.0f;
  if (pDomain) {
    xmin = pDomain->GetNumberAt(0);
    xmax = pDomain->GetNumberAt(1);
    ymin = pDomain->GetNumberAt(2);
    ymax = pDomain->GetNumberAt(3);
  }
  CFX_Matrix mtDomain2Target = pDict->GetMatrixBy("Matrix");
  CFX_Matrix matrix, reverse_matrix;
  matrix.SetReverse(*pObject2Bitmap);
  reverse_matrix.SetReverse(mtDomain2Target);
  matrix.Concat(reverse_matrix);
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  int pitch = pBitmap->GetPitch();
  uint32_t total_results =
      std::max(CountOutputs(funcs), pCS->CountComponents());
  CFX_FixedBufGrow<FX_FLOAT, 16> result_array(total_results);
  FX_FLOAT* pResults = result_array;
  FXSYS_memset(pResults, 0, total_results * sizeof(FX_FLOAT));
  for (int row = 0; row < height; row++) {
    uint32_t* dib_buf = (uint32_t*)(pBitmap->GetBuffer() + row * pitch);
    for (int column = 0; column < width; column++) {
      FX_FLOAT x = (FX_FLOAT)column, y = (FX_FLOAT)row;
      matrix.Transform(x, y);
      if (x < xmin || x > xmax || y < ymin || y > ymax) {
        continue;
      }
      FX_FLOAT input[2];
      int offset = 0;
      input[0] = x;
      input[1] = y;
      for (const auto& func : funcs) {
        if (func) {
          int nresults;
          if (func->Call(input, 2, pResults + offset, nresults))
            offset += nresults;
        }
      }
      FX_FLOAT R = 0.0f, G = 0.0f, B = 0.0f;
      pCS->GetRGB(pResults, R, G, B);
      dib_buf[column] = FXARGB_TODIB(FXARGB_MAKE(
          alpha, (int32_t)(R * 255), (int32_t)(G * 255), (int32_t)(B * 255)));
    }
  }
}

bool GetScanlineIntersect(int y,
                          FX_FLOAT x1,
                          FX_FLOAT y1,
                          FX_FLOAT x2,
                          FX_FLOAT y2,
                          FX_FLOAT* x) {
  if (y1 == y2)
    return FALSE;

  if (y1 < y2) {
    if (y < y1 || y > y2)
      return FALSE;
  } else {
    if (y < y2 || y > y1)
      return FALSE;
  }
  *x = x1 + ((x2 - x1) * (y - y1) / (y2 - y1));
  return TRUE;
}

void DrawGouraud(CFX_DIBitmap* pBitmap,
                 int alpha,
                 CPDF_MeshVertex triangle[3]) {
  FX_FLOAT min_y = triangle[0].y, max_y = triangle[0].y;
  for (int i = 1; i < 3; i++) {
    if (min_y > triangle[i].y) {
      min_y = triangle[i].y;
    }
    if (max_y < triangle[i].y) {
      max_y = triangle[i].y;
    }
  }
  if (min_y == max_y) {
    return;
  }
  int min_yi = (int)FXSYS_floor(min_y), max_yi = (int)FXSYS_ceil(max_y);
  if (min_yi < 0) {
    min_yi = 0;
  }
  if (max_yi >= pBitmap->GetHeight()) {
    max_yi = pBitmap->GetHeight() - 1;
  }
  for (int y = min_yi; y <= max_yi; y++) {
    int nIntersects = 0;
    FX_FLOAT inter_x[3], r[3], g[3], b[3];
    for (int i = 0; i < 3; i++) {
      CPDF_MeshVertex& vertex1 = triangle[i];
      CPDF_MeshVertex& vertex2 = triangle[(i + 1) % 3];
      bool bIntersect = GetScanlineIntersect(y, vertex1.x, vertex1.y, vertex2.x,
                                             vertex2.y, &inter_x[nIntersects]);
      if (!bIntersect)
        continue;

      FX_FLOAT y_dist = (y - vertex1.y) / (vertex2.y - vertex1.y);
      r[nIntersects] = vertex1.r + ((vertex2.r - vertex1.r) * y_dist);
      g[nIntersects] = vertex1.g + ((vertex2.g - vertex1.g) * y_dist);
      b[nIntersects] = vertex1.b + ((vertex2.b - vertex1.b) * y_dist);
      nIntersects++;
    }
    if (nIntersects != 2) {
      continue;
    }
    int min_x, max_x, start_index, end_index;
    if (inter_x[0] < inter_x[1]) {
      min_x = (int)FXSYS_floor(inter_x[0]);
      max_x = (int)FXSYS_ceil(inter_x[1]);
      start_index = 0;
      end_index = 1;
    } else {
      min_x = (int)FXSYS_floor(inter_x[1]);
      max_x = (int)FXSYS_ceil(inter_x[0]);
      start_index = 1;
      end_index = 0;
    }
    int start_x = min_x, end_x = max_x;
    if (start_x < 0) {
      start_x = 0;
    }
    if (end_x > pBitmap->GetWidth()) {
      end_x = pBitmap->GetWidth();
    }
    uint8_t* dib_buf =
        pBitmap->GetBuffer() + y * pBitmap->GetPitch() + start_x * 4;
    FX_FLOAT r_unit = (r[end_index] - r[start_index]) / (max_x - min_x);
    FX_FLOAT g_unit = (g[end_index] - g[start_index]) / (max_x - min_x);
    FX_FLOAT b_unit = (b[end_index] - b[start_index]) / (max_x - min_x);
    FX_FLOAT R = r[start_index] + (start_x - min_x) * r_unit;
    FX_FLOAT G = g[start_index] + (start_x - min_x) * g_unit;
    FX_FLOAT B = b[start_index] + (start_x - min_x) * b_unit;
    for (int x = start_x; x < end_x; x++) {
      R += r_unit;
      G += g_unit;
      B += b_unit;
      FXARGB_SETDIB(dib_buf,
                    FXARGB_MAKE(alpha, (int32_t)(R * 255), (int32_t)(G * 255),
                                (int32_t)(B * 255)));
      dib_buf += 4;
    }
  }
}

void DrawFreeGouraudShading(
    CFX_DIBitmap* pBitmap,
    CFX_Matrix* pObject2Bitmap,
    CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    CPDF_ColorSpace* pCS,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  CPDF_MeshStream stream(kFreeFormGouraudTriangleMeshShading, funcs,
                         pShadingStream, pCS);
  if (!stream.Load())
    return;

  CPDF_MeshVertex triangle[3];
  FXSYS_memset(triangle, 0, sizeof(triangle));

  while (!stream.BitStream()->IsEOF()) {
    CPDF_MeshVertex vertex;
    uint32_t flag = stream.GetVertex(vertex, pObject2Bitmap);
    if (flag == 0) {
      triangle[0] = vertex;
      for (int j = 1; j < 3; j++) {
        stream.GetVertex(triangle[j], pObject2Bitmap);
      }
    } else {
      if (flag == 1) {
        triangle[0] = triangle[1];
      }
      triangle[1] = triangle[2];
      triangle[2] = vertex;
    }
    DrawGouraud(pBitmap, alpha, triangle);
  }
}

void DrawLatticeGouraudShading(
    CFX_DIBitmap* pBitmap,
    CFX_Matrix* pObject2Bitmap,
    CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    CPDF_ColorSpace* pCS,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);

  int row_verts = pShadingStream->GetDict()->GetIntegerBy("VerticesPerRow");
  if (row_verts < 2)
    return;

  CPDF_MeshStream stream(kLatticeFormGouraudTriangleMeshShading, funcs,
                         pShadingStream, pCS);
  if (!stream.Load())
    return;

  std::unique_ptr<CPDF_MeshVertex, FxFreeDeleter> vertex(
      FX_Alloc2D(CPDF_MeshVertex, row_verts, 2));
  if (!stream.GetVertexRow(vertex.get(), row_verts, pObject2Bitmap))
    return;

  int last_index = 0;
  while (1) {
    CPDF_MeshVertex* last_row = vertex.get() + last_index * row_verts;
    CPDF_MeshVertex* this_row = vertex.get() + (1 - last_index) * row_verts;
    if (!stream.GetVertexRow(this_row, row_verts, pObject2Bitmap))
      return;

    CPDF_MeshVertex triangle[3];
    for (int i = 1; i < row_verts; i++) {
      triangle[0] = last_row[i];
      triangle[1] = this_row[i - 1];
      triangle[2] = last_row[i - 1];
      DrawGouraud(pBitmap, alpha, triangle);
      triangle[2] = this_row[i];
      DrawGouraud(pBitmap, alpha, triangle);
    }
    last_index = 1 - last_index;
  }
}

struct Coon_BezierCoeff {
  float a, b, c, d;
  void FromPoints(float p0, float p1, float p2, float p3) {
    a = -p0 + 3 * p1 - 3 * p2 + p3;
    b = 3 * p0 - 6 * p1 + 3 * p2;
    c = -3 * p0 + 3 * p1;
    d = p0;
  }
  Coon_BezierCoeff first_half() {
    Coon_BezierCoeff result;
    result.a = a / 8;
    result.b = b / 4;
    result.c = c / 2;
    result.d = d;
    return result;
  }
  Coon_BezierCoeff second_half() {
    Coon_BezierCoeff result;
    result.a = a / 8;
    result.b = 3 * a / 8 + b / 4;
    result.c = 3 * a / 8 + b / 2 + c / 2;
    result.d = a / 8 + b / 4 + c / 2 + d;
    return result;
  }
  void GetPoints(float p[4]) {
    p[0] = d;
    p[1] = c / 3 + p[0];
    p[2] = b / 3 - p[0] + 2 * p[1];
    p[3] = a + p[0] - 3 * p[1] + 3 * p[2];
  }
  void GetPointsReverse(float p[4]) {
    p[3] = d;
    p[2] = c / 3 + p[3];
    p[1] = b / 3 - p[3] + 2 * p[2];
    p[0] = a + p[3] - 3 * p[2] + 3 * p[1];
  }
  void BezierInterpol(Coon_BezierCoeff& C1,
                      Coon_BezierCoeff& C2,
                      Coon_BezierCoeff& D1,
                      Coon_BezierCoeff& D2) {
    a = (D1.a + D2.a) / 2;
    b = (D1.b + D2.b) / 2;
    c = (D1.c + D2.c) / 2 - (C1.a / 8 + C1.b / 4 + C1.c / 2) +
        (C2.a / 8 + C2.b / 4) + (-C1.d + D2.d) / 2 - (C2.a + C2.b) / 2;
    d = C1.a / 8 + C1.b / 4 + C1.c / 2 + C1.d;
  }
  float Distance() {
    float dis = a + b + c;
    return dis < 0 ? -dis : dis;
  }
};

struct Coon_Bezier {
  Coon_BezierCoeff x, y;
  void FromPoints(float x0,
                  float y0,
                  float x1,
                  float y1,
                  float x2,
                  float y2,
                  float x3,
                  float y3) {
    x.FromPoints(x0, x1, x2, x3);
    y.FromPoints(y0, y1, y2, y3);
  }
  Coon_Bezier first_half() {
    Coon_Bezier result;
    result.x = x.first_half();
    result.y = y.first_half();
    return result;
  }
  Coon_Bezier second_half() {
    Coon_Bezier result;
    result.x = x.second_half();
    result.y = y.second_half();
    return result;
  }
  void BezierInterpol(Coon_Bezier& C1,
                      Coon_Bezier& C2,
                      Coon_Bezier& D1,
                      Coon_Bezier& D2) {
    x.BezierInterpol(C1.x, C2.x, D1.x, D2.x);
    y.BezierInterpol(C1.y, C2.y, D1.y, D2.y);
  }
  void GetPoints(FX_PATHPOINT* pPoints) {
    float p[4];
    int i;
    x.GetPoints(p);
    for (i = 0; i < 4; i++) {
      pPoints[i].m_PointX = p[i];
    }
    y.GetPoints(p);
    for (i = 0; i < 4; i++) {
      pPoints[i].m_PointY = p[i];
    }
  }
  void GetPointsReverse(FX_PATHPOINT* pPoints) {
    float p[4];
    int i;
    x.GetPointsReverse(p);
    for (i = 0; i < 4; i++) {
      pPoints[i].m_PointX = p[i];
    }
    y.GetPointsReverse(p);
    for (i = 0; i < 4; i++) {
      pPoints[i].m_PointY = p[i];
    }
  }
  float Distance() { return x.Distance() + y.Distance(); }
};

int BiInterpolImpl(int c0,
                   int c1,
                   int c2,
                   int c3,
                   int x,
                   int y,
                   int x_scale,
                   int y_scale) {
  int x1 = c0 + (c3 - c0) * x / x_scale;
  int x2 = c1 + (c2 - c1) * x / x_scale;
  return x1 + (x2 - x1) * y / y_scale;
}

struct Coon_Color {
  Coon_Color() { FXSYS_memset(comp, 0, sizeof(int) * 3); }
  int comp[3];

  void BiInterpol(Coon_Color colors[4],
                  int x,
                  int y,
                  int x_scale,
                  int y_scale) {
    for (int i = 0; i < 3; i++) {
      comp[i] = BiInterpolImpl(colors[0].comp[i], colors[1].comp[i],
                               colors[2].comp[i], colors[3].comp[i], x, y,
                               x_scale, y_scale);
    }
  }

  int Distance(Coon_Color& o) {
    return std::max({FXSYS_abs(comp[0] - o.comp[0]),
                     FXSYS_abs(comp[1] - o.comp[1]),
                     FXSYS_abs(comp[2] - o.comp[2])});
  }
};

struct CPDF_PatchDrawer {
  Coon_Color patch_colors[4];
  int max_delta;
  CFX_PathData path;
  CFX_RenderDevice* pDevice;
  int fill_mode;
  int alpha;
  void Draw(int x_scale,
            int y_scale,
            int left,
            int bottom,
            Coon_Bezier C1,
            Coon_Bezier C2,
            Coon_Bezier D1,
            Coon_Bezier D2) {
    FX_BOOL bSmall = C1.Distance() < 2 && C2.Distance() < 2 &&
                     D1.Distance() < 2 && D2.Distance() < 2;
    Coon_Color div_colors[4];
    int d_bottom = 0;
    int d_left = 0;
    int d_top = 0;
    int d_right = 0;
    div_colors[0].BiInterpol(patch_colors, left, bottom, x_scale, y_scale);
    if (!bSmall) {
      div_colors[1].BiInterpol(patch_colors, left, bottom + 1, x_scale,
                               y_scale);
      div_colors[2].BiInterpol(patch_colors, left + 1, bottom + 1, x_scale,
                               y_scale);
      div_colors[3].BiInterpol(patch_colors, left + 1, bottom, x_scale,
                               y_scale);
      d_bottom = div_colors[3].Distance(div_colors[0]);
      d_left = div_colors[1].Distance(div_colors[0]);
      d_top = div_colors[1].Distance(div_colors[2]);
      d_right = div_colors[2].Distance(div_colors[3]);
    }
#define COONCOLOR_THRESHOLD 4
    if (bSmall ||
        (d_bottom < COONCOLOR_THRESHOLD && d_left < COONCOLOR_THRESHOLD &&
         d_top < COONCOLOR_THRESHOLD && d_right < COONCOLOR_THRESHOLD)) {
      FX_PATHPOINT* pPoints = path.GetPoints();
      C1.GetPoints(pPoints);
      D2.GetPoints(pPoints + 3);
      C2.GetPointsReverse(pPoints + 6);
      D1.GetPointsReverse(pPoints + 9);
      int fillFlags = FXFILL_WINDING | FXFILL_FULLCOVER;
      if (fill_mode & RENDER_NOPATHSMOOTH) {
        fillFlags |= FXFILL_NOPATHSMOOTH;
      }
      pDevice->DrawPath(
          &path, nullptr, nullptr,
          FXARGB_MAKE(alpha, div_colors[0].comp[0], div_colors[0].comp[1],
                      div_colors[0].comp[2]),
          0, fillFlags);
    } else {
      if (d_bottom < COONCOLOR_THRESHOLD && d_top < COONCOLOR_THRESHOLD) {
        Coon_Bezier m1;
        m1.BezierInterpol(D1, D2, C1, C2);
        y_scale *= 2;
        bottom *= 2;
        Draw(x_scale, y_scale, left, bottom, C1, m1, D1.first_half(),
             D2.first_half());
        Draw(x_scale, y_scale, left, bottom + 1, m1, C2, D1.second_half(),
             D2.second_half());
      } else if (d_left < COONCOLOR_THRESHOLD &&
                 d_right < COONCOLOR_THRESHOLD) {
        Coon_Bezier m2;
        m2.BezierInterpol(C1, C2, D1, D2);
        x_scale *= 2;
        left *= 2;
        Draw(x_scale, y_scale, left, bottom, C1.first_half(), C2.first_half(),
             D1, m2);
        Draw(x_scale, y_scale, left + 1, bottom, C1.second_half(),
             C2.second_half(), m2, D2);
      } else {
        Coon_Bezier m1, m2;
        m1.BezierInterpol(D1, D2, C1, C2);
        m2.BezierInterpol(C1, C2, D1, D2);
        Coon_Bezier m1f = m1.first_half();
        Coon_Bezier m1s = m1.second_half();
        Coon_Bezier m2f = m2.first_half();
        Coon_Bezier m2s = m2.second_half();
        x_scale *= 2;
        y_scale *= 2;
        left *= 2;
        bottom *= 2;
        Draw(x_scale, y_scale, left, bottom, C1.first_half(), m1f,
             D1.first_half(), m2f);
        Draw(x_scale, y_scale, left, bottom + 1, m1f, C2.first_half(),
             D1.second_half(), m2s);
        Draw(x_scale, y_scale, left + 1, bottom, C1.second_half(), m1s, m2f,
             D2.first_half());
        Draw(x_scale, y_scale, left + 1, bottom + 1, m1s, C2.second_half(), m2s,
             D2.second_half());
      }
    }
  }
};

void DrawCoonPatchMeshes(
    ShadingType type,
    CFX_DIBitmap* pBitmap,
    CFX_Matrix* pObject2Bitmap,
    CPDF_Stream* pShadingStream,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    CPDF_ColorSpace* pCS,
    int fill_mode,
    int alpha) {
  ASSERT(pBitmap->GetFormat() == FXDIB_Argb);
  ASSERT(type == kCoonsPatchMeshShading ||
         type == kTensorProductPatchMeshShading);

  CFX_FxgeDevice device;
  device.Attach(pBitmap, false, nullptr, false);
  CPDF_MeshStream stream(type, funcs, pShadingStream, pCS);
  if (!stream.Load())
    return;

  CPDF_PatchDrawer patch;
  patch.alpha = alpha;
  patch.pDevice = &device;
  patch.fill_mode = fill_mode;
  patch.path.SetPointCount(13);
  FX_PATHPOINT* pPoints = patch.path.GetPoints();
  pPoints[0].m_Flag = FXPT_MOVETO;
  for (int i = 1; i < 13; i++)
    pPoints[i].m_Flag = FXPT_BEZIERTO;
  CFX_PointF coords[16];
  int point_count = type == kTensorProductPatchMeshShading ? 16 : 12;
  while (!stream.BitStream()->IsEOF()) {
    uint32_t flag = stream.GetFlag();
    int iStartPoint = 0, iStartColor = 0, i = 0;
    if (flag) {
      iStartPoint = 4;
      iStartColor = 2;
      CFX_PointF tempCoords[4];
      for (i = 0; i < 4; i++) {
        tempCoords[i] = coords[(flag * 3 + i) % 12];
      }
      FXSYS_memcpy(coords, tempCoords, sizeof(tempCoords));
      Coon_Color tempColors[2];
      tempColors[0] = patch.patch_colors[flag];
      tempColors[1] = patch.patch_colors[(flag + 1) % 4];
      FXSYS_memcpy(patch.patch_colors, tempColors, sizeof(Coon_Color) * 2);
    }
    for (i = iStartPoint; i < point_count; i++) {
      stream.GetCoords(coords[i].x, coords[i].y);
      pObject2Bitmap->Transform(coords[i].x, coords[i].y);
    }
    for (i = iStartColor; i < 4; i++) {
      FX_FLOAT r = 0.0f, g = 0.0f, b = 0.0f;
      stream.GetColor(r, g, b);
      patch.patch_colors[i].comp[0] = (int32_t)(r * 255);
      patch.patch_colors[i].comp[1] = (int32_t)(g * 255);
      patch.patch_colors[i].comp[2] = (int32_t)(b * 255);
    }
    CFX_FloatRect bbox = CFX_FloatRect::GetBBox(coords, point_count);
    if (bbox.right <= 0 || bbox.left >= (FX_FLOAT)pBitmap->GetWidth() ||
        bbox.top <= 0 || bbox.bottom >= (FX_FLOAT)pBitmap->GetHeight()) {
      continue;
    }
    Coon_Bezier C1, C2, D1, D2;
    C1.FromPoints(coords[0].x, coords[0].y, coords[11].x, coords[11].y,
                  coords[10].x, coords[10].y, coords[9].x, coords[9].y);
    C2.FromPoints(coords[3].x, coords[3].y, coords[4].x, coords[4].y,
                  coords[5].x, coords[5].y, coords[6].x, coords[6].y);
    D1.FromPoints(coords[0].x, coords[0].y, coords[1].x, coords[1].y,
                  coords[2].x, coords[2].y, coords[3].x, coords[3].y);
    D2.FromPoints(coords[9].x, coords[9].y, coords[8].x, coords[8].y,
                  coords[7].x, coords[7].y, coords[6].x, coords[6].y);
    patch.Draw(1, 1, 0, 0, C1, C2, D1, D2);
  }
}

std::unique_ptr<CFX_DIBitmap> DrawPatternBitmap(
    CPDF_Document* pDoc,
    CPDF_PageRenderCache* pCache,
    CPDF_TilingPattern* pPattern,
    const CFX_Matrix* pObject2Device,
    int width,
    int height,
    int flags) {
  std::unique_ptr<CFX_DIBitmap> pBitmap(new CFX_DIBitmap);
  if (!pBitmap->Create(width, height,
                       pPattern->colored() ? FXDIB_Argb : FXDIB_8bppMask)) {
    return std::unique_ptr<CFX_DIBitmap>();
  }
  CFX_FxgeDevice bitmap_device;
  bitmap_device.Attach(pBitmap.get(), false, nullptr, false);
  pBitmap->Clear(0);
  CFX_FloatRect cell_bbox = pPattern->bbox();
  pPattern->pattern_to_form()->TransformRect(cell_bbox);
  pObject2Device->TransformRect(cell_bbox);
  CFX_FloatRect bitmap_rect(0.0f, 0.0f, (FX_FLOAT)width, (FX_FLOAT)height);
  CFX_Matrix mtAdjust;
  mtAdjust.MatchRect(bitmap_rect, cell_bbox);
  CFX_Matrix mtPattern2Bitmap = *pObject2Device;
  mtPattern2Bitmap.Concat(mtAdjust);
  CPDF_RenderOptions options;
  if (!pPattern->colored())
    options.m_ColorMode = RENDER_COLOR_ALPHA;

  flags |= RENDER_FORCE_HALFTONE;
  options.m_Flags = flags;
  CPDF_RenderContext context(pDoc, pCache);
  context.AppendLayer(pPattern->form(), &mtPattern2Bitmap);
  context.Render(&bitmap_device, &options, nullptr);
  return pBitmap;
}

}  // namespace

void CPDF_RenderStatus::DrawShading(CPDF_ShadingPattern* pPattern,
                                    CFX_Matrix* pMatrix,
                                    FX_RECT& clip_rect,
                                    int alpha,
                                    FX_BOOL bAlphaMode) {
  const auto& funcs = pPattern->GetFuncs();
  CPDF_Dictionary* pDict = pPattern->GetShadingObject()->GetDict();
  CPDF_ColorSpace* pColorSpace = pPattern->GetCS();
  if (!pColorSpace)
    return;

  FX_ARGB background = 0;
  if (!pPattern->IsShadingObject() && pDict->KeyExist("Background")) {
    CPDF_Array* pBackColor = pDict->GetArrayBy("Background");
    if (pBackColor &&
        pBackColor->GetCount() >= pColorSpace->CountComponents()) {
      CFX_FixedBufGrow<FX_FLOAT, 16> comps(pColorSpace->CountComponents());
      for (uint32_t i = 0; i < pColorSpace->CountComponents(); i++)
        comps[i] = pBackColor->GetNumberAt(i);
      FX_FLOAT R = 0.0f, G = 0.0f, B = 0.0f;
      pColorSpace->GetRGB(comps, R, G, B);
      background = ArgbEncode(255, (int32_t)(R * 255), (int32_t)(G * 255),
                              (int32_t)(B * 255));
    }
  }
  if (pDict->KeyExist("BBox")) {
    CFX_FloatRect rect = pDict->GetRectBy("BBox");
    rect.Transform(pMatrix);
    clip_rect.Intersect(rect.GetOutterRect());
  }
  if (m_pDevice->GetDeviceCaps(FXDC_RENDER_CAPS) & FXRC_SHADING &&
      m_pDevice->GetDeviceDriver()->DrawShading(pPattern, pMatrix, clip_rect,
                                                alpha, bAlphaMode)) {
    return;
  }
  CPDF_DeviceBuffer buffer;
  buffer.Initialize(m_pContext, m_pDevice, &clip_rect, m_pCurObj, 150);
  CFX_Matrix FinalMatrix = *pMatrix;
  FinalMatrix.Concat(*buffer.GetMatrix());
  CFX_DIBitmap* pBitmap = buffer.GetBitmap();
  if (!pBitmap->GetBuffer())
    return;

  pBitmap->Clear(background);
  int fill_mode = m_Options.m_Flags;
  switch (pPattern->GetShadingType()) {
    case kInvalidShading:
    case kMaxShading:
      return;
    case kFunctionBasedShading:
      DrawFuncShading(pBitmap, &FinalMatrix, pDict, funcs, pColorSpace, alpha);
      break;
    case kAxialShading:
      DrawAxialShading(pBitmap, &FinalMatrix, pDict, funcs, pColorSpace, alpha);
      break;
    case kRadialShading:
      DrawRadialShading(pBitmap, &FinalMatrix, pDict, funcs, pColorSpace,
                        alpha);
      break;
    case kFreeFormGouraudTriangleMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawFreeGouraudShading(pBitmap, &FinalMatrix, pStream, funcs,
                               pColorSpace, alpha);
      }
    } break;
    case kLatticeFormGouraudTriangleMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawLatticeGouraudShading(pBitmap, &FinalMatrix, pStream, funcs,
                                  pColorSpace, alpha);
      }
    } break;
    case kCoonsPatchMeshShading:
    case kTensorProductPatchMeshShading: {
      // The shading object can be a stream or a dictionary. We do not handle
      // the case of dictionary at the moment.
      if (CPDF_Stream* pStream = ToStream(pPattern->GetShadingObject())) {
        DrawCoonPatchMeshes(pPattern->GetShadingType(), pBitmap, &FinalMatrix,
                            pStream, funcs, pColorSpace, fill_mode, alpha);
      }
    } break;
  }
  if (bAlphaMode)
    pBitmap->LoadChannel(FXDIB_Red, pBitmap, FXDIB_Alpha);

  if (m_Options.m_ColorMode == RENDER_COLOR_GRAY)
    pBitmap->ConvertColorScale(m_Options.m_ForeColor, m_Options.m_BackColor);
  buffer.OutputToDevice();
}

void CPDF_RenderStatus::DrawShadingPattern(CPDF_ShadingPattern* pattern,
                                           const CPDF_PageObject* pPageObj,
                                           const CFX_Matrix* pObj2Device,
                                           FX_BOOL bStroke) {
  if (!pattern->Load())
    return;

  m_pDevice->SaveState();
  if (pPageObj->IsPath()) {
    if (!SelectClipPath(pPageObj->AsPath(), pObj2Device, bStroke)) {
      m_pDevice->RestoreState(false);
      return;
    }
  } else if (pPageObj->IsImage()) {
    m_pDevice->SetClip_Rect(pPageObj->GetBBox(pObj2Device));
  } else {
    return;
  }
  FX_RECT rect;
  if (GetObjectClippedRect(pPageObj, pObj2Device, FALSE, rect)) {
    m_pDevice->RestoreState(false);
    return;
  }
  CFX_Matrix matrix = *pattern->pattern_to_form();
  matrix.Concat(*pObj2Device);
  GetScaledMatrix(matrix);
  int alpha = pPageObj->m_GeneralState.GetAlpha(bStroke);
  DrawShading(pattern, &matrix, rect, alpha,
              m_Options.m_ColorMode == RENDER_COLOR_ALPHA);
  m_pDevice->RestoreState(false);
}

void CPDF_RenderStatus::ProcessShading(const CPDF_ShadingObject* pShadingObj,
                                       const CFX_Matrix* pObj2Device) {
  FX_RECT rect = pShadingObj->GetBBox(pObj2Device);
  FX_RECT clip_box = m_pDevice->GetClipBox();
  rect.Intersect(clip_box);
  if (rect.IsEmpty())
    return;

  CFX_Matrix matrix = pShadingObj->m_Matrix;
  matrix.Concat(*pObj2Device);
  DrawShading(pShadingObj->m_pShading, &matrix, rect,
              pShadingObj->m_GeneralState.GetAlpha(FALSE),
              m_Options.m_ColorMode == RENDER_COLOR_ALPHA);
}

void CPDF_RenderStatus::DrawTilingPattern(CPDF_TilingPattern* pPattern,
                                          const CPDF_PageObject* pPageObj,
                                          const CFX_Matrix* pObj2Device,
                                          FX_BOOL bStroke) {
  if (!pPattern->Load()) {
    return;
  }
  m_pDevice->SaveState();
  if (pPageObj->IsPath()) {
    if (!SelectClipPath(pPageObj->AsPath(), pObj2Device, bStroke)) {
      m_pDevice->RestoreState(false);
      return;
    }
  } else if (pPageObj->IsImage()) {
    m_pDevice->SetClip_Rect(pPageObj->GetBBox(pObj2Device));
  } else {
    return;
  }
  FX_RECT clip_box = m_pDevice->GetClipBox();
  if (clip_box.IsEmpty()) {
    m_pDevice->RestoreState(false);
    return;
  }
  CFX_Matrix dCTM = m_pDevice->GetCTM();
  FX_FLOAT sa = FXSYS_fabs(dCTM.a);
  FX_FLOAT sd = FXSYS_fabs(dCTM.d);
  clip_box.right = clip_box.left + (int32_t)FXSYS_ceil(clip_box.Width() * sa);
  clip_box.bottom = clip_box.top + (int32_t)FXSYS_ceil(clip_box.Height() * sd);
  CFX_Matrix mtPattern2Device = *pPattern->pattern_to_form();
  mtPattern2Device.Concat(*pObj2Device);
  GetScaledMatrix(mtPattern2Device);
  FX_BOOL bAligned = FALSE;
  if (pPattern->bbox().left == 0 && pPattern->bbox().bottom == 0 &&
      pPattern->bbox().right == pPattern->x_step() &&
      pPattern->bbox().top == pPattern->y_step() &&
      (mtPattern2Device.IsScaled() || mtPattern2Device.Is90Rotated())) {
    bAligned = TRUE;
  }
  CFX_FloatRect cell_bbox = pPattern->bbox();
  mtPattern2Device.TransformRect(cell_bbox);
  int width = (int)FXSYS_ceil(cell_bbox.Width());
  int height = (int)FXSYS_ceil(cell_bbox.Height());
  if (width == 0) {
    width = 1;
  }
  if (height == 0) {
    height = 1;
  }
  int min_col, max_col, min_row, max_row;
  CFX_Matrix mtDevice2Pattern;
  mtDevice2Pattern.SetReverse(mtPattern2Device);
  CFX_FloatRect clip_box_p(clip_box);
  clip_box_p.Transform(&mtDevice2Pattern);

  min_col = (int)FXSYS_ceil((clip_box_p.left - pPattern->bbox().right) /
                            pPattern->x_step());
  max_col = (int)FXSYS_floor((clip_box_p.right - pPattern->bbox().left) /
                             pPattern->x_step());
  min_row = (int)FXSYS_ceil((clip_box_p.bottom - pPattern->bbox().top) /
                            pPattern->y_step());
  max_row = (int)FXSYS_floor((clip_box_p.top - pPattern->bbox().bottom) /
                             pPattern->y_step());

  if (width > clip_box.Width() || height > clip_box.Height() ||
      width * height > clip_box.Width() * clip_box.Height()) {
    CPDF_GraphicStates* pStates = nullptr;
    if (!pPattern->colored())
      pStates = CloneObjStates(pPageObj, bStroke);

    CPDF_Dictionary* pFormResource = nullptr;
    if (pPattern->form()->m_pFormDict)
      pFormResource = pPattern->form()->m_pFormDict->GetDictBy("Resources");

    for (int col = min_col; col <= max_col; col++)
      for (int row = min_row; row <= max_row; row++) {
        FX_FLOAT orig_x, orig_y;
        orig_x = col * pPattern->x_step();
        orig_y = row * pPattern->y_step();
        mtPattern2Device.Transform(orig_x, orig_y);
        CFX_Matrix matrix = *pObj2Device;
        matrix.Translate(orig_x - mtPattern2Device.e,
                         orig_y - mtPattern2Device.f);
        m_pDevice->SaveState();
        CPDF_RenderStatus status;
        status.Initialize(m_pContext, m_pDevice, nullptr, nullptr, this,
                          pStates, &m_Options, pPattern->form()->m_Transparency,
                          m_bDropObjects, pFormResource);
        status.RenderObjectList(pPattern->form(), &matrix);
        m_pDevice->RestoreState(false);
      }
    m_pDevice->RestoreState(false);
    delete pStates;
    return;
  }
  if (bAligned) {
    int orig_x = FXSYS_round(mtPattern2Device.e);
    int orig_y = FXSYS_round(mtPattern2Device.f);
    min_col = (clip_box.left - orig_x) / width;
    if (clip_box.left < orig_x) {
      min_col--;
    }
    max_col = (clip_box.right - orig_x) / width;
    if (clip_box.right <= orig_x) {
      max_col--;
    }
    min_row = (clip_box.top - orig_y) / height;
    if (clip_box.top < orig_y) {
      min_row--;
    }
    max_row = (clip_box.bottom - orig_y) / height;
    if (clip_box.bottom <= orig_y) {
      max_row--;
    }
  }
  FX_FLOAT left_offset = cell_bbox.left - mtPattern2Device.e;
  FX_FLOAT top_offset = cell_bbox.bottom - mtPattern2Device.f;
  std::unique_ptr<CFX_DIBitmap> pPatternBitmap;
  if (width * height < 16) {
    std::unique_ptr<CFX_DIBitmap> pEnlargedBitmap =
        DrawPatternBitmap(m_pContext->GetDocument(), m_pContext->GetPageCache(),
                          pPattern, pObj2Device, 8, 8, m_Options.m_Flags);
    pPatternBitmap.reset(pEnlargedBitmap->StretchTo(width, height));
  } else {
    pPatternBitmap = DrawPatternBitmap(
        m_pContext->GetDocument(), m_pContext->GetPageCache(), pPattern,
        pObj2Device, width, height, m_Options.m_Flags);
  }
  if (!pPatternBitmap) {
    m_pDevice->RestoreState(false);
    return;
  }
  if (m_Options.m_ColorMode == RENDER_COLOR_GRAY) {
    pPatternBitmap->ConvertColorScale(m_Options.m_ForeColor,
                                      m_Options.m_BackColor);
  }
  FX_ARGB fill_argb = GetFillArgb(pPageObj);
  int clip_width = clip_box.right - clip_box.left;
  int clip_height = clip_box.bottom - clip_box.top;
  CFX_DIBitmap screen;
  if (!screen.Create(clip_width, clip_height, FXDIB_Argb)) {
    return;
  }
  screen.Clear(0);
  uint32_t* src_buf = (uint32_t*)pPatternBitmap->GetBuffer();
  for (int col = min_col; col <= max_col; col++) {
    for (int row = min_row; row <= max_row; row++) {
      int start_x, start_y;
      if (bAligned) {
        start_x = FXSYS_round(mtPattern2Device.e) + col * width - clip_box.left;
        start_y = FXSYS_round(mtPattern2Device.f) + row * height - clip_box.top;
      } else {
        FX_FLOAT orig_x = col * pPattern->x_step();
        FX_FLOAT orig_y = row * pPattern->y_step();
        mtPattern2Device.Transform(orig_x, orig_y);
        start_x = FXSYS_round(orig_x + left_offset) - clip_box.left;
        start_y = FXSYS_round(orig_y + top_offset) - clip_box.top;
      }
      if (width == 1 && height == 1) {
        if (start_x < 0 || start_x >= clip_box.Width() || start_y < 0 ||
            start_y >= clip_box.Height()) {
          continue;
        }
        uint32_t* dest_buf =
            (uint32_t*)(screen.GetBuffer() + screen.GetPitch() * start_y +
                        start_x * 4);
        if (pPattern->colored())
          *dest_buf = *src_buf;
        else
          *dest_buf = (*(uint8_t*)src_buf << 24) | (fill_argb & 0xffffff);
      } else {
        if (pPattern->colored()) {
          screen.CompositeBitmap(start_x, start_y, width, height,
                                 pPatternBitmap.get(), 0, 0);
        } else {
          screen.CompositeMask(start_x, start_y, width, height,
                               pPatternBitmap.get(), fill_argb, 0, 0);
        }
      }
    }
  }
  CompositeDIBitmap(&screen, clip_box.left, clip_box.top, 0, 255,
                    FXDIB_BLEND_NORMAL, FALSE);
  m_pDevice->RestoreState(false);
}

void CPDF_RenderStatus::DrawPathWithPattern(const CPDF_PathObject* pPathObj,
                                            const CFX_Matrix* pObj2Device,
                                            CPDF_Color* pColor,
                                            FX_BOOL bStroke) {
  CPDF_Pattern* pattern = pColor->GetPattern();
  if (!pattern)
    return;

  if (CPDF_TilingPattern* pTilingPattern = pattern->AsTilingPattern())
    DrawTilingPattern(pTilingPattern, pPathObj, pObj2Device, bStroke);
  else if (CPDF_ShadingPattern* pShadingPattern = pattern->AsShadingPattern())
    DrawShadingPattern(pShadingPattern, pPathObj, pObj2Device, bStroke);
}

void CPDF_RenderStatus::ProcessPathPattern(const CPDF_PathObject* pPathObj,
                                           const CFX_Matrix* pObj2Device,
                                           int& filltype,
                                           FX_BOOL& bStroke) {
  if (filltype) {
    CPDF_Color& FillColor = *pPathObj->m_ColorState.GetFillColor();
    if (FillColor.IsPattern()) {
      DrawPathWithPattern(pPathObj, pObj2Device, &FillColor, FALSE);
      filltype = 0;
    }
  }
  if (bStroke) {
    CPDF_Color& StrokeColor = *pPathObj->m_ColorState.GetStrokeColor();
    if (StrokeColor.IsPattern()) {
      DrawPathWithPattern(pPathObj, pObj2Device, &StrokeColor, TRUE);
      bStroke = FALSE;
    }
  }
}
