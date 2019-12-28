// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_ge.h"
#include "third_party/base/numerics/safe_math.h"

CFX_ClipRgn::CFX_ClipRgn(int width, int height)
    : m_Type(RectI), m_Box(0, 0, width, height) {}

CFX_ClipRgn::CFX_ClipRgn(const FX_RECT& rect) : m_Type(RectI), m_Box(rect) {}

CFX_ClipRgn::CFX_ClipRgn(const CFX_ClipRgn& src) {
  m_Type = src.m_Type;
  m_Box = src.m_Box;
  m_Mask = src.m_Mask;
}

CFX_ClipRgn::~CFX_ClipRgn() {}

void CFX_ClipRgn::Reset(const FX_RECT& rect) {
  m_Type = RectI;
  m_Box = rect;
  m_Mask.SetNull();
}

void CFX_ClipRgn::IntersectRect(const FX_RECT& rect) {
  if (m_Type == RectI) {
    m_Box.Intersect(rect);
    return;
  }
  if (m_Type == MaskF) {
    IntersectMaskRect(rect, m_Box, m_Mask);
    return;
  }
}

void CFX_ClipRgn::IntersectMaskRect(FX_RECT rect,
                                    FX_RECT mask_rect,
                                    CFX_DIBitmapRef Mask) {
  const CFX_DIBitmap* mask_dib = Mask.GetObject();
  m_Type = MaskF;
  m_Box = rect;
  m_Box.Intersect(mask_rect);
  if (m_Box.IsEmpty()) {
    m_Type = RectI;
    return;
  }
  if (m_Box == mask_rect) {
    m_Mask = Mask;
    return;
  }
  CFX_DIBitmap* new_dib = m_Mask.New();
  if (!new_dib) {
    return;
  }
  new_dib->Create(m_Box.Width(), m_Box.Height(), FXDIB_8bppMask);
  for (int row = m_Box.top; row < m_Box.bottom; row++) {
    uint8_t* dest_scan =
        new_dib->GetBuffer() + new_dib->GetPitch() * (row - m_Box.top);
    uint8_t* src_scan =
        mask_dib->GetBuffer() + mask_dib->GetPitch() * (row - mask_rect.top);
    for (int col = m_Box.left; col < m_Box.right; col++) {
      dest_scan[col - m_Box.left] = src_scan[col - mask_rect.left];
    }
  }
}

void CFX_ClipRgn::IntersectMaskF(int left, int top, CFX_DIBitmapRef Mask) {
  const CFX_DIBitmap* mask_dib = Mask.GetObject();
  ASSERT(mask_dib->GetFormat() == FXDIB_8bppMask);
  FX_RECT mask_box(left, top, left + mask_dib->GetWidth(),
                   top + mask_dib->GetHeight());
  if (m_Type == RectI) {
    IntersectMaskRect(m_Box, mask_box, Mask);
    return;
  }
  if (m_Type == MaskF) {
    FX_RECT new_box = m_Box;
    new_box.Intersect(mask_box);
    if (new_box.IsEmpty()) {
      m_Type = RectI;
      m_Mask.SetNull();
      m_Box = new_box;
      return;
    }
    CFX_DIBitmapRef new_mask;
    CFX_DIBitmap* new_dib = new_mask.New();
    if (!new_dib) {
      return;
    }
    new_dib->Create(new_box.Width(), new_box.Height(), FXDIB_8bppMask);
    const CFX_DIBitmap* old_dib = m_Mask.GetObject();
    for (int row = new_box.top; row < new_box.bottom; row++) {
      uint8_t* old_scan =
          old_dib->GetBuffer() + (row - m_Box.top) * old_dib->GetPitch();
      uint8_t* mask_scan =
          mask_dib->GetBuffer() + (row - top) * mask_dib->GetPitch();
      uint8_t* new_scan =
          new_dib->GetBuffer() + (row - new_box.top) * new_dib->GetPitch();
      for (int col = new_box.left; col < new_box.right; col++) {
        new_scan[col - new_box.left] =
            old_scan[col - m_Box.left] * mask_scan[col - left] / 255;
      }
    }
    m_Box = new_box;
    m_Mask = new_mask;
    return;
  }
  ASSERT(FALSE);
}

CFX_PathData::CFX_PathData()
    : m_PointCount(0), m_pPoints(nullptr), m_AllocCount(0) {}

CFX_PathData::~CFX_PathData() {
  FX_Free(m_pPoints);
}

void CFX_PathData::SetPointCount(int nPoints) {
  m_PointCount = nPoints;
  if (m_AllocCount < nPoints) {
    FX_Free(m_pPoints);
    m_pPoints = FX_Alloc(FX_PATHPOINT, nPoints);
    m_AllocCount = nPoints;
  }
}

void CFX_PathData::AllocPointCount(int nPoints) {
  if (m_AllocCount < nPoints) {
    FX_PATHPOINT* pNewBuf = FX_Alloc(FX_PATHPOINT, nPoints);
    if (m_PointCount) {
      FXSYS_memcpy(pNewBuf, m_pPoints, m_PointCount * sizeof(FX_PATHPOINT));
    }
    FX_Free(m_pPoints);
    m_pPoints = pNewBuf;
    m_AllocCount = nPoints;
  }
}

CFX_PathData::CFX_PathData(const CFX_PathData& src) {
  m_PointCount = m_AllocCount = src.m_PointCount;
  m_pPoints = FX_Alloc(FX_PATHPOINT, src.m_PointCount);
  FXSYS_memcpy(m_pPoints, src.m_pPoints, sizeof(FX_PATHPOINT) * m_PointCount);
}

void CFX_PathData::TrimPoints(int nPoints) {
  if (m_PointCount <= nPoints) {
    return;
  }
  SetPointCount(nPoints);
}

void CFX_PathData::AddPointCount(int addPoints) {
  pdfium::base::CheckedNumeric<int> safe_new_count = m_PointCount;
  safe_new_count += addPoints;
  int new_count = safe_new_count.ValueOrDie();
  AllocPointCount(new_count);
  m_PointCount = new_count;
}

void CFX_PathData::Append(const CFX_PathData* pSrc, const CFX_Matrix* pMatrix) {
  int old_count = m_PointCount;
  AddPointCount(pSrc->m_PointCount);
  FXSYS_memcpy(m_pPoints + old_count, pSrc->m_pPoints,
               pSrc->m_PointCount * sizeof(FX_PATHPOINT));
  if (pMatrix) {
    for (int i = 0; i < pSrc->m_PointCount; i++) {
      pMatrix->Transform(m_pPoints[old_count + i].m_PointX,
                         m_pPoints[old_count + i].m_PointY);
    }
  }
}

void CFX_PathData::SetPoint(int index, FX_FLOAT x, FX_FLOAT y, int flag) {
  ASSERT(index < m_PointCount);
  m_pPoints[index].m_PointX = x;
  m_pPoints[index].m_PointY = y;
  m_pPoints[index].m_Flag = flag;
}

void CFX_PathData::AppendRect(FX_FLOAT left,
                              FX_FLOAT bottom,
                              FX_FLOAT right,
                              FX_FLOAT top) {
  int old_count = m_PointCount;
  AddPointCount(5);
  FX_PATHPOINT* pPoints = m_pPoints + old_count;
  pPoints[0].m_PointX = pPoints[1].m_PointX = pPoints[4].m_PointX = left;
  pPoints[2].m_PointX = pPoints[3].m_PointX = right;
  pPoints[0].m_PointY = pPoints[3].m_PointY = pPoints[4].m_PointY = bottom;
  pPoints[1].m_PointY = pPoints[2].m_PointY = top;
  pPoints[0].m_Flag = FXPT_MOVETO;
  pPoints[1].m_Flag = pPoints[2].m_Flag = pPoints[3].m_Flag = FXPT_LINETO;
  pPoints[4].m_Flag = FXPT_LINETO | FXPT_CLOSEFIGURE;
}

CFX_FloatRect CFX_PathData::GetBoundingBox() const {
  CFX_FloatRect rect;
  if (m_PointCount) {
    rect.InitRect(m_pPoints[0].m_PointX, m_pPoints[0].m_PointY);
    for (int i = 1; i < m_PointCount; i++) {
      rect.UpdateRect(m_pPoints[i].m_PointX, m_pPoints[i].m_PointY);
    }
  }
  return rect;
}

static void _UpdateLineEndPoints(CFX_FloatRect& rect,
                                 FX_FLOAT start_x,
                                 FX_FLOAT start_y,
                                 FX_FLOAT end_x,
                                 FX_FLOAT end_y,
                                 FX_FLOAT hw) {
  if (start_x == end_x) {
    if (start_y == end_y) {
      rect.UpdateRect(end_x + hw, end_y + hw);
      rect.UpdateRect(end_x - hw, end_y - hw);
      return;
    }
    FX_FLOAT point_y;
    if (end_y < start_y) {
      point_y = end_y - hw;
    } else {
      point_y = end_y + hw;
    }
    rect.UpdateRect(end_x + hw, point_y);
    rect.UpdateRect(end_x - hw, point_y);
    return;
  }
  if (start_y == end_y) {
    FX_FLOAT point_x;
    if (end_x < start_x) {
      point_x = end_x - hw;
    } else {
      point_x = end_x + hw;
    }
    rect.UpdateRect(point_x, end_y + hw);
    rect.UpdateRect(point_x, end_y - hw);
    return;
  }
  FX_FLOAT dx = end_x - start_x;
  FX_FLOAT dy = end_y - start_y;
  FX_FLOAT ll = FXSYS_sqrt2(dx, dy);
  FX_FLOAT mx = end_x + hw * dx / ll;
  FX_FLOAT my = end_y + hw * dy / ll;
  FX_FLOAT dx1 = hw * dy / ll;
  FX_FLOAT dy1 = hw * dx / ll;
  rect.UpdateRect(mx - dx1, my + dy1);
  rect.UpdateRect(mx + dx1, my - dy1);
}

static void _UpdateLineJoinPoints(CFX_FloatRect& rect,
                                  FX_FLOAT start_x,
                                  FX_FLOAT start_y,
                                  FX_FLOAT middle_x,
                                  FX_FLOAT middle_y,
                                  FX_FLOAT end_x,
                                  FX_FLOAT end_y,
                                  FX_FLOAT half_width,
                                  FX_FLOAT miter_limit) {
  FX_FLOAT start_k = 0, start_c = 0, end_k = 0, end_c = 0, start_len = 0,
           start_dc = 0, end_len = 0, end_dc = 0;
  FX_BOOL bStartVert = FXSYS_fabs(start_x - middle_x) < 1.0f / 20;
  FX_BOOL bEndVert = FXSYS_fabs(middle_x - end_x) < 1.0f / 20;
  if (bStartVert && bEndVert) {
    int start_dir = middle_y > start_y ? 1 : -1;
    FX_FLOAT point_y = middle_y + half_width * start_dir;
    rect.UpdateRect(middle_x + half_width, point_y);
    rect.UpdateRect(middle_x - half_width, point_y);
    return;
  }
  if (!bStartVert) {
    start_k = (middle_y - start_y) / (middle_x - start_x);
    start_c = middle_y - (start_k * middle_x);
    start_len = FXSYS_sqrt2(start_x - middle_x, start_y - middle_y);
    start_dc =
        (FX_FLOAT)FXSYS_fabs(half_width * start_len / (start_x - middle_x));
  }
  if (!bEndVert) {
    end_k = (end_y - middle_y) / (end_x - middle_x);
    end_c = middle_y - (end_k * middle_x);
    end_len = FXSYS_sqrt2(end_x - middle_x, end_y - middle_y);
    end_dc = (FX_FLOAT)FXSYS_fabs(half_width * end_len / (end_x - middle_x));
  }
  if (bStartVert) {
    FX_FLOAT outside_x = start_x;
    if (end_x < start_x) {
      outside_x += half_width;
    } else {
      outside_x -= half_width;
    }
    FX_FLOAT outside_y;
    if (start_y < (end_k * start_x) + end_c) {
      outside_y = (end_k * outside_x) + end_c + end_dc;
    } else {
      outside_y = (end_k * outside_x) + end_c - end_dc;
    }
    rect.UpdateRect(outside_x, outside_y);
    return;
  }
  if (bEndVert) {
    FX_FLOAT outside_x = end_x;
    if (start_x < end_x) {
      outside_x += half_width;
    } else {
      outside_x -= half_width;
    }
    FX_FLOAT outside_y;
    if (end_y < (start_k * end_x) + start_c) {
      outside_y = (start_k * outside_x) + start_c + start_dc;
    } else {
      outside_y = (start_k * outside_x) + start_c - start_dc;
    }
    rect.UpdateRect(outside_x, outside_y);
    return;
  }
  if (FXSYS_fabs(start_k - end_k) < 1.0f / 20) {
    int start_dir = middle_x > start_x ? 1 : -1;
    int end_dir = end_x > middle_x ? 1 : -1;
    if (start_dir == end_dir) {
      _UpdateLineEndPoints(rect, middle_x, middle_y, end_x, end_y, half_width);
    } else {
      _UpdateLineEndPoints(rect, start_x, start_y, middle_x, middle_y,
                           half_width);
    }
    return;
  }
  FX_FLOAT start_outside_c = start_c;
  if (end_y < (start_k * end_x) + start_c) {
    start_outside_c += start_dc;
  } else {
    start_outside_c -= start_dc;
  }
  FX_FLOAT end_outside_c = end_c;
  if (start_y < (end_k * start_x) + end_c) {
    end_outside_c += end_dc;
  } else {
    end_outside_c -= end_dc;
  }
  FX_FLOAT join_x = (end_outside_c - start_outside_c) / (start_k - end_k);
  FX_FLOAT join_y = (start_k * join_x) + start_outside_c;
  rect.UpdateRect(join_x, join_y);
}

CFX_FloatRect CFX_PathData::GetBoundingBox(FX_FLOAT line_width,
                                           FX_FLOAT miter_limit) const {
  CFX_FloatRect rect(100000 * 1.0f, 100000 * 1.0f, -100000 * 1.0f,
                     -100000 * 1.0f);
  int iPoint = 0;
  FX_FLOAT half_width = line_width;
  int iStartPoint = 0;
  int iEndPoint = 0;
  int iMiddlePoint = 0;
  FX_BOOL bJoin;
  while (iPoint < m_PointCount) {
    if (m_pPoints[iPoint].m_Flag == FXPT_MOVETO) {
      iStartPoint = iPoint + 1;
      iEndPoint = iPoint;
      bJoin = FALSE;
    } else {
      if (m_pPoints[iPoint].m_Flag == FXPT_BEZIERTO) {
        rect.UpdateRect(m_pPoints[iPoint].m_PointX, m_pPoints[iPoint].m_PointY);
        rect.UpdateRect(m_pPoints[iPoint + 1].m_PointX,
                        m_pPoints[iPoint + 1].m_PointY);
        iPoint += 2;
      }
      if (iPoint == m_PointCount - 1 ||
          m_pPoints[iPoint + 1].m_Flag == FXPT_MOVETO) {
        iStartPoint = iPoint - 1;
        iEndPoint = iPoint;
        bJoin = FALSE;
      } else {
        iStartPoint = iPoint - 1;
        iMiddlePoint = iPoint;
        iEndPoint = iPoint + 1;
        bJoin = TRUE;
      }
    }
    FX_FLOAT start_x = m_pPoints[iStartPoint].m_PointX;
    FX_FLOAT start_y = m_pPoints[iStartPoint].m_PointY;
    FX_FLOAT end_x = m_pPoints[iEndPoint].m_PointX;
    FX_FLOAT end_y = m_pPoints[iEndPoint].m_PointY;
    if (bJoin) {
      FX_FLOAT middle_x = m_pPoints[iMiddlePoint].m_PointX;
      FX_FLOAT middle_y = m_pPoints[iMiddlePoint].m_PointY;
      _UpdateLineJoinPoints(rect, start_x, start_y, middle_x, middle_y, end_x,
                            end_y, half_width, miter_limit);
    } else {
      _UpdateLineEndPoints(rect, start_x, start_y, end_x, end_y, half_width);
    }
    iPoint++;
  }
  return rect;
}

void CFX_PathData::Transform(const CFX_Matrix* pMatrix) {
  if (!pMatrix) {
    return;
  }
  for (int i = 0; i < m_PointCount; i++) {
    pMatrix->Transform(m_pPoints[i].m_PointX, m_pPoints[i].m_PointY);
  }
}

FX_BOOL CFX_PathData::GetZeroAreaPath(CFX_PathData& NewPath,
                                      CFX_Matrix* pMatrix,
                                      FX_BOOL& bThin,
                                      FX_BOOL bAdjust) const {
  if (m_PointCount < 3) {
    return FALSE;
  }
  if (m_PointCount == 3 && (m_pPoints[0].m_Flag & FXPT_TYPE) == FXPT_MOVETO &&
      (m_pPoints[1].m_Flag & FXPT_TYPE) == FXPT_LINETO &&
      (m_pPoints[2].m_Flag & FXPT_TYPE) == FXPT_LINETO &&
      m_pPoints[0].m_PointX == m_pPoints[2].m_PointX &&
      m_pPoints[0].m_PointY == m_pPoints[2].m_PointY) {
    NewPath.AddPointCount(2);
    if (bAdjust) {
      if (pMatrix) {
        FX_FLOAT x = m_pPoints[0].m_PointX, y = m_pPoints[0].m_PointY;
        pMatrix->TransformPoint(x, y);
        x = (int)x + 0.5f;
        y = (int)y + 0.5f;
        NewPath.SetPoint(0, x, y, FXPT_MOVETO);
        x = m_pPoints[1].m_PointX, y = m_pPoints[1].m_PointY;
        pMatrix->TransformPoint(x, y);
        x = (int)x + 0.5f;
        y = (int)y + 0.5f;
        NewPath.SetPoint(1, x, y, FXPT_LINETO);
        pMatrix->SetIdentity();
      } else {
        FX_FLOAT x = (int)m_pPoints[0].m_PointX + 0.5f,
                 y = (int)m_pPoints[0].m_PointY + 0.5f;
        NewPath.SetPoint(0, x, y, FXPT_MOVETO);
        x = (int)m_pPoints[1].m_PointX + 0.5f,
        y = (int)m_pPoints[1].m_PointY + 0.5f;
        NewPath.SetPoint(1, x, y, FXPT_LINETO);
      }
    } else {
      NewPath.SetPoint(0, m_pPoints[0].m_PointX, m_pPoints[0].m_PointY,
                       FXPT_MOVETO);
      NewPath.SetPoint(1, m_pPoints[1].m_PointX, m_pPoints[1].m_PointY,
                       FXPT_LINETO);
    }
    if (m_pPoints[0].m_PointX != m_pPoints[1].m_PointX &&
        m_pPoints[0].m_PointY != m_pPoints[1].m_PointY) {
      bThin = TRUE;
    }
    return TRUE;
  }
  if (((m_PointCount > 3) && (m_PointCount % 2))) {
    int mid = m_PointCount / 2;
    FX_BOOL bZeroArea = FALSE;
    CFX_PathData t_path;
    for (int i = 0; i < mid; i++) {
      if (!(m_pPoints[mid - i - 1].m_PointX ==
                m_pPoints[mid + i + 1].m_PointX &&
            m_pPoints[mid - i - 1].m_PointY ==
                m_pPoints[mid + i + 1].m_PointY &&
            ((m_pPoints[mid - i - 1].m_Flag & FXPT_TYPE) != FXPT_BEZIERTO &&
             (m_pPoints[mid + i + 1].m_Flag & FXPT_TYPE) != FXPT_BEZIERTO))) {
        bZeroArea = TRUE;
        break;
      }
      int new_count = t_path.GetPointCount();
      t_path.AddPointCount(2);
      t_path.SetPoint(new_count, m_pPoints[mid - i].m_PointX,
                      m_pPoints[mid - i].m_PointY, FXPT_MOVETO);
      t_path.SetPoint(new_count + 1, m_pPoints[mid - i - 1].m_PointX,
                      m_pPoints[mid - i - 1].m_PointY, FXPT_LINETO);
    }
    if (!bZeroArea) {
      NewPath.Append(&t_path, nullptr);
      bThin = TRUE;
      return TRUE;
    }
  }
  int stratPoint = 0;
  int next = 0, i;
  for (i = 0; i < m_PointCount; i++) {
    int point_type = m_pPoints[i].m_Flag & FXPT_TYPE;
    if (point_type == FXPT_MOVETO) {
      stratPoint = i;
    } else if (point_type == FXPT_LINETO) {
      next = (i + 1 - stratPoint) % (m_PointCount - stratPoint) + stratPoint;
      if ((m_pPoints[next].m_Flag & FXPT_TYPE) != FXPT_BEZIERTO &&
          (m_pPoints[next].m_Flag & FXPT_TYPE) != FXPT_MOVETO) {
        if ((m_pPoints[i - 1].m_PointX == m_pPoints[i].m_PointX &&
             m_pPoints[i].m_PointX == m_pPoints[next].m_PointX) &&
            ((m_pPoints[i].m_PointY - m_pPoints[i - 1].m_PointY) *
                 (m_pPoints[i].m_PointY - m_pPoints[next].m_PointY) >
             0)) {
          int pre = i;
          if (FXSYS_fabs(m_pPoints[i].m_PointY - m_pPoints[i - 1].m_PointY) <
              FXSYS_fabs(m_pPoints[i].m_PointY - m_pPoints[next].m_PointY)) {
            pre--;
            next--;
          }
          int new_count = NewPath.GetPointCount();
          NewPath.AddPointCount(2);
          NewPath.SetPoint(new_count, m_pPoints[pre].m_PointX,
                           m_pPoints[pre].m_PointY, FXPT_MOVETO);
          NewPath.SetPoint(new_count + 1, m_pPoints[next].m_PointX,
                           m_pPoints[next].m_PointY, FXPT_LINETO);
        } else if ((m_pPoints[i - 1].m_PointY == m_pPoints[i].m_PointY &&
                    m_pPoints[i].m_PointY == m_pPoints[next].m_PointY) &&
                   ((m_pPoints[i].m_PointX - m_pPoints[i - 1].m_PointX) *
                        (m_pPoints[i].m_PointX - m_pPoints[next].m_PointX) >
                    0)) {
          int pre = i;
          if (FXSYS_fabs(m_pPoints[i].m_PointX - m_pPoints[i - 1].m_PointX) <
              FXSYS_fabs(m_pPoints[i].m_PointX - m_pPoints[next].m_PointX)) {
            pre--;
            next--;
          }
          int new_count = NewPath.GetPointCount();
          NewPath.AddPointCount(2);
          NewPath.SetPoint(new_count, m_pPoints[pre].m_PointX,
                           m_pPoints[pre].m_PointY, FXPT_MOVETO);
          NewPath.SetPoint(new_count + 1, m_pPoints[next].m_PointX,
                           m_pPoints[next].m_PointY, FXPT_LINETO);
        } else if ((m_pPoints[i - 1].m_Flag & FXPT_TYPE) == FXPT_MOVETO &&
                   (m_pPoints[next].m_Flag & FXPT_TYPE) == FXPT_LINETO &&
                   m_pPoints[i - 1].m_PointX == m_pPoints[next].m_PointX &&
                   m_pPoints[i - 1].m_PointY == m_pPoints[next].m_PointY &&
                   m_pPoints[next].m_Flag & FXPT_CLOSEFIGURE) {
          int new_count = NewPath.GetPointCount();
          NewPath.AddPointCount(2);
          NewPath.SetPoint(new_count, m_pPoints[i - 1].m_PointX,
                           m_pPoints[i - 1].m_PointY, FXPT_MOVETO);
          NewPath.SetPoint(new_count + 1, m_pPoints[i].m_PointX,
                           m_pPoints[i].m_PointY, FXPT_LINETO);
          bThin = TRUE;
        }
      }
    } else if (point_type == FXPT_BEZIERTO) {
      i += 2;
      continue;
    }
  }
  if (m_PointCount > 3 && NewPath.GetPointCount()) {
    bThin = TRUE;
  }
  if (NewPath.GetPointCount() == 0) {
    return FALSE;
  }
  return TRUE;
}

FX_BOOL CFX_PathData::IsRect() const {
  if (m_PointCount != 5 && m_PointCount != 4) {
    return FALSE;
  }
  if ((m_PointCount == 5 && (m_pPoints[0].m_PointX != m_pPoints[4].m_PointX ||
                             m_pPoints[0].m_PointY != m_pPoints[4].m_PointY)) ||
      (m_pPoints[0].m_PointX == m_pPoints[2].m_PointX &&
       m_pPoints[0].m_PointY == m_pPoints[2].m_PointY) ||
      (m_pPoints[1].m_PointX == m_pPoints[3].m_PointX &&
       m_pPoints[1].m_PointY == m_pPoints[3].m_PointY)) {
    return FALSE;
  }
  if (m_pPoints[0].m_PointX != m_pPoints[3].m_PointX &&
      m_pPoints[0].m_PointY != m_pPoints[3].m_PointY) {
    return FALSE;
  }
  for (int i = 1; i < 4; i++) {
    if ((m_pPoints[i].m_Flag & FXPT_TYPE) != FXPT_LINETO) {
      return FALSE;
    }
    if (m_pPoints[i].m_PointX != m_pPoints[i - 1].m_PointX &&
        m_pPoints[i].m_PointY != m_pPoints[i - 1].m_PointY) {
      return FALSE;
    }
  }
  return m_PointCount == 5 || (m_pPoints[3].m_Flag & FXPT_CLOSEFIGURE);
}

FX_BOOL CFX_PathData::IsRect(const CFX_Matrix* pMatrix,
                             CFX_FloatRect* pRect) const {
  if (!pMatrix) {
    if (!IsRect()) {
      return FALSE;
    }
    if (pRect) {
      pRect->left = m_pPoints[0].m_PointX;
      pRect->right = m_pPoints[2].m_PointX;
      pRect->bottom = m_pPoints[0].m_PointY;
      pRect->top = m_pPoints[2].m_PointY;
      pRect->Normalize();
    }
    return TRUE;
  }
  if (m_PointCount != 5 && m_PointCount != 4) {
    return FALSE;
  }
  if ((m_PointCount == 5 && (m_pPoints[0].m_PointX != m_pPoints[4].m_PointX ||
                             m_pPoints[0].m_PointY != m_pPoints[4].m_PointY)) ||
      (m_pPoints[1].m_PointX == m_pPoints[3].m_PointX &&
       m_pPoints[1].m_PointY == m_pPoints[3].m_PointY)) {
    return FALSE;
  }
  if (m_PointCount == 4 && m_pPoints[0].m_PointX != m_pPoints[3].m_PointX &&
      m_pPoints[0].m_PointY != m_pPoints[3].m_PointY) {
    return FALSE;
  }
  FX_FLOAT x[5], y[5];
  for (int i = 0; i < m_PointCount; i++) {
    pMatrix->Transform(m_pPoints[i].m_PointX, m_pPoints[i].m_PointY, x[i],
                       y[i]);
    if (i) {
      if ((m_pPoints[i].m_Flag & FXPT_TYPE) != FXPT_LINETO) {
        return FALSE;
      }
      if (x[i] != x[i - 1] && y[i] != y[i - 1]) {
        return FALSE;
      }
    }
  }
  if (pRect) {
    pRect->left = x[0];
    pRect->right = x[2];
    pRect->bottom = y[0];
    pRect->top = y[2];
    pRect->Normalize();
  }
  return TRUE;
}

void CFX_PathData::Copy(const CFX_PathData& src) {
  SetPointCount(src.m_PointCount);
  FXSYS_memcpy(m_pPoints, src.m_pPoints, sizeof(FX_PATHPOINT) * m_PointCount);
}

CFX_GraphStateData::CFX_GraphStateData()
    : m_LineCap(LineCapButt),
      m_DashCount(0),
      m_DashArray(nullptr),
      m_DashPhase(0),
      m_LineJoin(LineJoinMiter),
      m_MiterLimit(10 * 1.0f),
      m_LineWidth(1.0f) {}

CFX_GraphStateData::CFX_GraphStateData(const CFX_GraphStateData& src) {
  m_DashArray = nullptr;
  Copy(src);
}

void CFX_GraphStateData::Copy(const CFX_GraphStateData& src) {
  m_LineCap = src.m_LineCap;
  m_DashCount = src.m_DashCount;
  FX_Free(m_DashArray);
  m_DashArray = nullptr;
  m_DashPhase = src.m_DashPhase;
  m_LineJoin = src.m_LineJoin;
  m_MiterLimit = src.m_MiterLimit;
  m_LineWidth = src.m_LineWidth;
  if (m_DashCount) {
    m_DashArray = FX_Alloc(FX_FLOAT, m_DashCount);
    FXSYS_memcpy(m_DashArray, src.m_DashArray, m_DashCount * sizeof(FX_FLOAT));
  }
}

CFX_GraphStateData::~CFX_GraphStateData() {
  FX_Free(m_DashArray);
}

void CFX_GraphStateData::SetDashCount(int count) {
  FX_Free(m_DashArray);
  m_DashArray = nullptr;
  m_DashCount = count;
  if (count == 0) {
    return;
  }
  m_DashArray = FX_Alloc(FX_FLOAT, count);
}
