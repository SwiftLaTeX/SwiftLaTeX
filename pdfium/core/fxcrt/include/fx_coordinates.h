// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_INCLUDE_FX_COORDINATES_H_
#define CORE_FXCRT_INCLUDE_FX_COORDINATES_H_

#include "core/fxcrt/include/fx_basic.h"

class CFX_Matrix;

template <class BaseType>
class CFX_PSTemplate {
 public:
  CFX_PSTemplate() : x(0), y(0) {}
  CFX_PSTemplate(BaseType new_x, BaseType new_y) : x(new_x), y(new_y) {}
  CFX_PSTemplate(const CFX_PSTemplate& other) : x(other.x), y(other.y) {}
  void clear() {
    x = 0;
    y = 0;
  }
  CFX_PSTemplate operator=(const CFX_PSTemplate& other) {
    if (this != &other) {
      x = other.x;
      y = other.y;
    }
    return *this;
  }
  bool operator==(const CFX_PSTemplate& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const CFX_PSTemplate& other) const {
    return !(*this == other);
  }
  CFX_PSTemplate& operator+=(const CFX_PSTemplate<BaseType>& obj) {
    x += obj.x;
    y += obj.y;
    return *this;
  }
  CFX_PSTemplate& operator-=(const CFX_PSTemplate<BaseType>& obj) {
    x -= obj.x;
    y -= obj.y;
    return *this;
  }
  CFX_PSTemplate& operator*=(BaseType factor) {
    x *= factor;
    y *= factor;
    return *this;
  }
  CFX_PSTemplate& operator/=(BaseType divisor) {
    x /= divisor;
    y /= divisor;
    return *this;
  }
  CFX_PSTemplate operator+(const CFX_PSTemplate& other) {
    return CFX_PSTemplate(x + other.x, y + other.y);
  }
  CFX_PSTemplate operator-(const CFX_PSTemplate& other) {
    return CFX_PSTemplate(x - other.x, y - other.y);
  }
  CFX_PSTemplate operator*(BaseType factor) {
    return CFX_PSTemplate(x * factor, y * factor);
  }
  CFX_PSTemplate operator/(BaseType divisor) {
    return CFX_PSTemplate(x / divisor, y / divisor);
  }

  BaseType x;
  BaseType y;
};
typedef CFX_PSTemplate<int32_t> CFX_Point;
typedef CFX_PSTemplate<FX_FLOAT> CFX_PointF;
typedef CFX_PSTemplate<int32_t> CFX_Size;
typedef CFX_PSTemplate<FX_FLOAT> CFX_SizeF;
typedef CFX_ArrayTemplate<CFX_Point> CFX_Points;
typedef CFX_ArrayTemplate<CFX_PointF> CFX_PointsF;

template <class BaseType>
class CFX_VTemplate : public CFX_PSTemplate<BaseType> {
 public:
  using CFX_PSTemplate<BaseType>::x;
  using CFX_PSTemplate<BaseType>::y;

  CFX_VTemplate() : CFX_PSTemplate<BaseType>() {}
  CFX_VTemplate(BaseType new_x, BaseType new_y)
      : CFX_PSTemplate<BaseType>(new_x, new_y) {}

  CFX_VTemplate(const CFX_VTemplate& other) : CFX_PSTemplate<BaseType>(other) {}

  CFX_VTemplate(const CFX_PSTemplate<BaseType>& point1,
                const CFX_PSTemplate<BaseType>& point2)
      : CFX_PSTemplate<BaseType>(point2.x - point1.x, point2.y - point1.y) {}

  FX_FLOAT Length() const { return FXSYS_sqrt(x * x + y * y); }
  void Normalize() {
    FX_FLOAT fLen = Length();
    if (fLen < 0.0001f)
      return;

    x /= fLen;
    y /= fLen;
  }
  void Translate(BaseType dx, BaseType dy) {
    x += dx;
    y += dy;
  }
  void Scale(BaseType sx, BaseType sy) {
    x *= sx;
    y *= sy;
  }
  void Rotate(FX_FLOAT fRadian) {
    FX_FLOAT cosValue = FXSYS_cos(fRadian);
    FX_FLOAT sinValue = FXSYS_sin(fRadian);
    x = x * cosValue - y * sinValue;
    y = x * sinValue + y * cosValue;
  }
};
typedef CFX_VTemplate<int32_t> CFX_Vector;
typedef CFX_VTemplate<FX_FLOAT> CFX_VectorF;

// Rectangles.
// TODO(tsepez): Consolidate all these different rectangle classes.

// LTRB rectangles (y-axis runs downwards).
struct FX_SMALL_RECT {
  FX_SMALL_RECT() : FX_SMALL_RECT(kInvalid, kInvalid, kInvalid, kInvalid) {}

  FX_SMALL_RECT(int16_t l, int16_t t, int16_t r, int16_t b)
      : left(l), top(t), right(r), bottom(b) {}

  static const int16_t kInvalid = -1;

  int16_t left;
  int16_t top;
  int16_t right;
  int16_t bottom;
};

struct FX_RECT {
  FX_RECT() : left(0), top(0), right(0), bottom(0) {}

  FX_RECT(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

  explicit FX_RECT(const FX_SMALL_RECT& other)
      : FX_RECT(other.left, other.top, other.right, other.bottom) {}

  int Width() const { return right - left; }
  int Height() const { return bottom - top; }
  bool IsEmpty() const { return right <= left || bottom <= top; }

  void Normalize();

  void Intersect(const FX_RECT& src);
  void Intersect(int l, int t, int r, int b) { Intersect(FX_RECT(l, t, r, b)); }

  void Union(const FX_RECT& other_rect);
  void Union(int l, int t, int r, int b) { Union(FX_RECT(l, t, r, b)); }

  void Offset(int dx, int dy) {
    left += dx;
    right += dx;
    top += dy;
    bottom += dy;
  }

  bool operator==(const FX_RECT& src) const {
    return left == src.left && right == src.right && top == src.top &&
           bottom == src.bottom;
  }

  FX_BOOL Contains(const FX_RECT& other_rect) const {
    return other_rect.left >= left && other_rect.right <= right &&
           other_rect.top >= top && other_rect.bottom <= bottom;
  }

  FX_BOOL Contains(int x, int y) const {
    return x >= left && x < right && y >= top && y < bottom;
  }

  FX_SMALL_RECT ToSmallRect() const {
    return FX_SMALL_RECT(
        static_cast<uint16_t>(left), static_cast<uint16_t>(top),
        static_cast<uint16_t>(right), static_cast<uint16_t>(bottom));
  }

  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

// LBRT rectangles (y-axis runs upwards).
class CFX_FloatPoint {
 public:
  CFX_FloatPoint(FX_FLOAT xx, FX_FLOAT yy) : x(xx), y(yy) {}

  FX_FLOAT x;
  FX_FLOAT y;
};

class CFX_FloatRect {
 public:
  CFX_FloatRect() : CFX_FloatRect(0.0f, 0.0f, 0.0f, 0.0f) {}
  CFX_FloatRect(FX_FLOAT l, FX_FLOAT b, FX_FLOAT r, FX_FLOAT t)
      : left(l), bottom(b), right(r), top(t) {}

  explicit CFX_FloatRect(const FX_FLOAT* pArray)
      : CFX_FloatRect(pArray[0], pArray[1], pArray[2], pArray[3]) {}

  explicit CFX_FloatRect(const FX_RECT& rect);

  void Normalize();

  void Reset() {
    left = 0.0f;
    right = 0.0f;
    bottom = 0.0f;
    top = 0.0f;
  }

  bool IsEmpty() const { return left >= right || bottom >= top; }
  bool Contains(const CFX_FloatRect& other_rect) const;
  bool Contains(FX_FLOAT x, FX_FLOAT y) const;

  void Transform(const CFX_Matrix* pMatrix);
  void Intersect(const CFX_FloatRect& other_rect);
  void Union(const CFX_FloatRect& other_rect);

  FX_RECT GetInnerRect() const;
  FX_RECT GetOutterRect() const;
  FX_RECT GetClosestRect() const;

  int Substract4(CFX_FloatRect& substract_rect, CFX_FloatRect* pRects);

  void InitRect(FX_FLOAT x, FX_FLOAT y) {
    left = x;
    right = x;
    bottom = y;
    top = y;
  }
  void UpdateRect(FX_FLOAT x, FX_FLOAT y);

  FX_FLOAT Width() const { return right - left; }
  FX_FLOAT Height() const { return top - bottom; }

  void Inflate(FX_FLOAT x, FX_FLOAT y) {
    Normalize();
    left -= x;
    right += x;
    bottom -= y;
    top += y;
  }

  void Inflate(FX_FLOAT other_left,
               FX_FLOAT other_bottom,
               FX_FLOAT other_right,
               FX_FLOAT other_top) {
    Normalize();
    left -= other_left;
    bottom -= other_bottom;
    right += other_right;
    top += other_top;
  }

  void Inflate(const CFX_FloatRect& rt) {
    Inflate(rt.left, rt.bottom, rt.right, rt.top);
  }

  void Deflate(FX_FLOAT x, FX_FLOAT y) {
    Normalize();
    left += x;
    right -= x;
    bottom += y;
    top -= y;
  }

  void Deflate(FX_FLOAT other_left,
               FX_FLOAT other_bottom,
               FX_FLOAT other_right,
               FX_FLOAT other_top) {
    Normalize();
    left += other_left;
    bottom += other_bottom;
    right -= other_right;
    top -= other_top;
  }

  void Deflate(const CFX_FloatRect& rt) {
    Deflate(rt.left, rt.bottom, rt.right, rt.top);
  }

  void Translate(FX_FLOAT e, FX_FLOAT f) {
    left += e;
    right += e;
    top += f;
    bottom += f;
  }

  static CFX_FloatRect GetBBox(const CFX_PointF* pPoints, int nPoints);

  FX_RECT ToFxRect() const {
    return FX_RECT(static_cast<int32_t>(left), static_cast<int32_t>(top),
                   static_cast<int32_t>(right), static_cast<int32_t>(bottom));
  }

  FX_FLOAT left;
  FX_FLOAT bottom;
  FX_FLOAT right;
  FX_FLOAT top;
};

// LTWH rectangles (y-axis runs downwards).
template <class baseType>
class CFX_RTemplate {
 public:
  typedef CFX_PSTemplate<baseType> FXT_POINT;
  typedef CFX_PSTemplate<baseType> FXT_SIZE;
  typedef CFX_VTemplate<baseType> FXT_VECTOR;
  typedef CFX_RTemplate<baseType> FXT_RECT;
  void Set(baseType dst_left,
           baseType dst_top,
           baseType dst_width,
           baseType dst_height) {
    left = dst_left;
    top = dst_top;
    width = dst_width;
    height = dst_height;
  }
  void Set(baseType dst_left, baseType dst_top, const FXT_SIZE& dst_size) {
    left = dst_left;
    top = dst_top;
    Size(dst_size);
  }
  void Set(const FXT_POINT& p, baseType dst_width, baseType dst_height) {
    TopLeft(p);
    width = dst_width;
    height = dst_height;
  }
  void Set(const FXT_POINT& p1, const FXT_POINT& p2) {
    TopLeft(p1);
    width = p2.x - p1.x;
    height = p2.y - p1.y;
    Normalize();
  }
  void Set(const FXT_POINT& p, const FXT_VECTOR& v) {
    TopLeft(p);
    width = v.x;
    height = v.y;
    Normalize();
  }
  void Reset() {
    left = 0;
    top = 0;
    width = 0;
    height = 0;
  }
  FXT_RECT& operator+=(const FXT_POINT& p) {
    left += p.x;
    top += p.y;
    return *this;
  }
  FXT_RECT& operator-=(const FXT_POINT& p) {
    left -= p.x;
    top -= p.y;
    return *this;
  }
  baseType right() const { return left + width; }
  baseType bottom() const { return top + height; }
  void Normalize() {
    if (width < 0) {
      left += width;
      width = -width;
    }
    if (height < 0) {
      top += height;
      height = -height;
    }
  }
  void Offset(baseType dx, baseType dy) {
    left += dx;
    top += dy;
  }
  void Inflate(baseType x, baseType y) {
    left -= x;
    width += x * 2;
    top -= y;
    height += y * 2;
  }
  void Inflate(const FXT_POINT& p) { Inflate(p.x, p.y); }
  void Inflate(baseType off_left,
               baseType off_top,
               baseType off_right,
               baseType off_bottom) {
    left -= off_left;
    top -= off_top;
    width += off_left + off_right;
    height += off_top + off_bottom;
  }
  void Inflate(const FXT_RECT& rt) {
    Inflate(rt.left, rt.top, rt.left + rt.width, rt.top + rt.height);
  }
  void Deflate(baseType x, baseType y) {
    left += x;
    width -= x * 2;
    top += y;
    height -= y * 2;
  }
  void Deflate(const FXT_POINT& p) { Deflate(p.x, p.y); }
  void Deflate(baseType off_left,
               baseType off_top,
               baseType off_right,
               baseType off_bottom) {
    left += off_left;
    top += off_top;
    width -= off_left + off_right;
    height -= off_top + off_bottom;
  }
  void Deflate(const FXT_RECT& rt) {
    Deflate(rt.left, rt.top, rt.top + rt.width, rt.top + rt.height);
  }
  FX_BOOL IsEmpty() const { return width <= 0 || height <= 0; }
  FX_BOOL IsEmpty(FX_FLOAT fEpsilon) const {
    return width <= fEpsilon || height <= fEpsilon;
  }
  void Empty() { width = height = 0; }
  FX_BOOL Contains(baseType x, baseType y) const {
    return x >= left && x < left + width && y >= top && y < top + height;
  }
  FX_BOOL Contains(const FXT_POINT& p) const { return Contains(p.x, p.y); }
  FX_BOOL Contains(const FXT_RECT& rt) const {
    return rt.left >= left && rt.right() <= right() && rt.top >= top &&
           rt.bottom() <= bottom();
  }
  baseType Width() const { return width; }
  baseType Height() const { return height; }
  FXT_SIZE Size() const {
    FXT_SIZE size;
    size.Set(width, height);
    return size;
  }
  void Size(FXT_SIZE s) { width = s.x, height = s.y; }
  FXT_POINT TopLeft() const {
    FXT_POINT p;
    p.x = left;
    p.y = top;
    return p;
  }
  FXT_POINT TopRight() const {
    FXT_POINT p;
    p.x = left + width;
    p.y = top;
    return p;
  }
  FXT_POINT BottomLeft() const {
    FXT_POINT p;
    p.x = left;
    p.y = top + height;
    return p;
  }
  FXT_POINT BottomRight() const {
    FXT_POINT p;
    p.x = left + width;
    p.y = top + height;
    return p;
  }
  void TopLeft(FXT_POINT tl) {
    left = tl.x;
    top = tl.y;
  }
  void TopRight(FXT_POINT tr) {
    width = tr.x - left;
    top = tr.y;
  }
  void BottomLeft(FXT_POINT bl) {
    left = bl.x;
    height = bl.y - top;
  }
  void BottomRight(FXT_POINT br) {
    width = br.x - left;
    height = br.y - top;
  }
  FXT_POINT Center() const {
    FXT_POINT p;
    p.x = left + width / 2;
    p.y = top + height / 2;
    return p;
  }
  void Union(baseType x, baseType y) {
    baseType r = right();
    baseType b = bottom();
    if (left > x)
      left = x;
    if (r < x)
      r = x;
    if (top > y)
      top = y;
    if (b < y)
      b = y;
    width = r - left;
    height = b - top;
  }
  void Union(const FXT_POINT& p) { Union(p.x, p.y); }
  void Union(const FXT_RECT& rt) {
    baseType r = right();
    baseType b = bottom();
    if (left > rt.left)
      left = rt.left;
    if (r < rt.right())
      r = rt.right();
    if (top > rt.top)
      top = rt.top;
    if (b < rt.bottom())
      b = rt.bottom();
    width = r - left;
    height = b - top;
  }
  void Intersect(const FXT_RECT& rt) {
    baseType r = right();
    baseType b = bottom();
    if (left < rt.left)
      left = rt.left;
    if (r > rt.right())
      r = rt.right();
    if (top < rt.top)
      top = rt.top;
    if (b > rt.bottom())
      b = rt.bottom();
    width = r - left;
    height = b - top;
  }
  FX_BOOL IntersectWith(const FXT_RECT& rt) const {
    FXT_RECT rect = rt;
    rect.Intersect(*this);
    return !rect.IsEmpty();
  }
  FX_BOOL IntersectWith(const FXT_RECT& rt, FX_FLOAT fEpsilon) const {
    FXT_RECT rect = rt;
    rect.Intersect(*this);
    return !rect.IsEmpty(fEpsilon);
  }
  friend bool operator==(const FXT_RECT& rc1, const FXT_RECT& rc2) {
    return rc1.left == rc2.left && rc1.top == rc2.top &&
           rc1.width == rc2.width && rc1.height == rc2.height;
  }
  friend bool operator!=(const FXT_RECT& rc1, const FXT_RECT& rc2) {
    return !(rc1 == rc2);
  }
  baseType left, top;
  baseType width, height;
};
typedef CFX_RTemplate<int32_t> CFX_Rect;
typedef CFX_RTemplate<FX_FLOAT> CFX_RectF;
typedef CFX_ArrayTemplate<CFX_RectF> CFX_RectFArray;

class CFX_Matrix {
 public:
  CFX_Matrix() { SetIdentity(); }

  CFX_Matrix(FX_FLOAT a1,
             FX_FLOAT b1,
             FX_FLOAT c1,
             FX_FLOAT d1,
             FX_FLOAT e1,
             FX_FLOAT f1) {
    a = a1;
    b = b1;
    c = c1;
    d = d1;
    e = e1;
    f = f1;
  }

  void Set(FX_FLOAT a,
           FX_FLOAT b,
           FX_FLOAT c,
           FX_FLOAT d,
           FX_FLOAT e,
           FX_FLOAT f);
  void Set(const FX_FLOAT n[6]);

  void SetIdentity() {
    a = d = 1;
    b = c = e = f = 0;
  }

  void SetReverse(const CFX_Matrix& m);

  void Concat(FX_FLOAT a,
              FX_FLOAT b,
              FX_FLOAT c,
              FX_FLOAT d,
              FX_FLOAT e,
              FX_FLOAT f,
              FX_BOOL bPrepended = FALSE);
  void Concat(const CFX_Matrix& m, FX_BOOL bPrepended = FALSE);
  void ConcatInverse(const CFX_Matrix& m, FX_BOOL bPrepended = FALSE);

  void Copy(const CFX_Matrix& m) { *this = m; }

  FX_BOOL IsIdentity() const {
    return a == 1 && b == 0 && c == 0 && d == 1 && e == 0 && f == 0;
  }
  FX_BOOL IsInvertible() const;

  FX_BOOL Is90Rotated() const;

  FX_BOOL IsScaled() const;

  void Translate(FX_FLOAT x, FX_FLOAT y, FX_BOOL bPrepended = FALSE);

  void TranslateI(int32_t x, int32_t y, FX_BOOL bPrepended = FALSE) {
    Translate((FX_FLOAT)x, (FX_FLOAT)y, bPrepended);
  }

  void Scale(FX_FLOAT sx, FX_FLOAT sy, FX_BOOL bPrepended = FALSE);

  void Rotate(FX_FLOAT fRadian, FX_BOOL bPrepended = FALSE);

  void RotateAt(FX_FLOAT fRadian,
                FX_FLOAT x,
                FX_FLOAT y,
                FX_BOOL bPrepended = FALSE);

  void Shear(FX_FLOAT fAlphaRadian,
             FX_FLOAT fBetaRadian,
             FX_BOOL bPrepended = FALSE);

  void MatchRect(const CFX_FloatRect& dest, const CFX_FloatRect& src);

  FX_FLOAT GetXUnit() const;

  FX_FLOAT GetYUnit() const;
  void GetUnitRect(CFX_RectF& rect) const;

  CFX_FloatRect GetUnitRect() const;

  FX_FLOAT GetUnitArea() const;
  FX_FLOAT TransformXDistance(FX_FLOAT dx) const;
  int32_t TransformXDistance(int32_t dx) const;
  FX_FLOAT TransformYDistance(FX_FLOAT dy) const;
  int32_t TransformYDistance(int32_t dy) const;
  FX_FLOAT TransformDistance(FX_FLOAT dx, FX_FLOAT dy) const;
  int32_t TransformDistance(int32_t dx, int32_t dy) const;
  FX_FLOAT TransformDistance(FX_FLOAT distance) const;

  void TransformPoint(FX_FLOAT& x, FX_FLOAT& y) const;
  void TransformPoint(int32_t& x, int32_t& y) const;

  void Transform(FX_FLOAT& x, FX_FLOAT& y) const { TransformPoint(x, y); }
  void Transform(FX_FLOAT x, FX_FLOAT y, FX_FLOAT& x1, FX_FLOAT& y1) const {
    x1 = x, y1 = y;
    TransformPoint(x1, y1);
  }

  void TransformVector(CFX_VectorF& v) const;
  void TransformVector(CFX_Vector& v) const;
  void TransformRect(CFX_RectF& rect) const;
  void TransformRect(CFX_Rect& rect) const;

  void TransformRect(FX_FLOAT& left,
                     FX_FLOAT& right,
                     FX_FLOAT& top,
                     FX_FLOAT& bottom) const;
  void TransformRect(CFX_FloatRect& rect) const {
    TransformRect(rect.left, rect.right, rect.top, rect.bottom);
  }

  FX_FLOAT GetA() const { return a; }
  FX_FLOAT GetB() const { return b; }
  FX_FLOAT GetC() const { return c; }
  FX_FLOAT GetD() const { return d; }
  FX_FLOAT GetE() const { return e; }
  FX_FLOAT GetF() const { return f; }

 public:
  FX_FLOAT a;
  FX_FLOAT b;
  FX_FLOAT c;
  FX_FLOAT d;
  FX_FLOAT e;
  FX_FLOAT f;
};

#endif  // CORE_FXCRT_INCLUDE_FX_COORDINATES_H_
