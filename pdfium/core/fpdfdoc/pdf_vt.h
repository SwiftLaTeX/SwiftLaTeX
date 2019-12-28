// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_PDF_VT_H_
#define CORE_FPDFDOC_PDF_VT_H_

#include "core/fpdfdoc/cpvt_floatrect.h"
#include "core/fpdfdoc/cpvt_lineinfo.h"
#include "core/fpdfdoc/include/cpvt_wordrange.h"

class CPDF_VariableText;

struct CPVT_WordInfo;

#define IsFloatZero(f) ((f) < 0.0001 && (f) > -0.0001)
#define IsFloatBigger(fa, fb) ((fa) > (fb) && !IsFloatZero((fa) - (fb)))
#define IsFloatSmaller(fa, fb) ((fa) < (fb) && !IsFloatZero((fa) - (fb)))
#define IsFloatEqual(fa, fb) IsFloatZero((fa) - (fb))

template <class TYPE>
class CPVT_ArrayTemplate : public CFX_ArrayTemplate<TYPE> {
 public:
  FX_BOOL IsEmpty() { return CFX_ArrayTemplate<TYPE>::GetSize() <= 0; }
  TYPE GetAt(int nIndex) const {
    if (nIndex >= 0 && nIndex < CFX_ArrayTemplate<TYPE>::GetSize()) {
      return CFX_ArrayTemplate<TYPE>::GetAt(nIndex);
    }
    return nullptr;
  }
  void RemoveAt(int nIndex) {
    if (nIndex >= 0 && nIndex < CFX_ArrayTemplate<TYPE>::GetSize()) {
      CFX_ArrayTemplate<TYPE>::RemoveAt(nIndex);
    }
  }
};
class CLine final {
 public:
  CLine();
  ~CLine();

  CPVT_WordPlace GetBeginWordPlace() const;
  CPVT_WordPlace GetEndWordPlace() const;
  CPVT_WordPlace GetPrevWordPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetNextWordPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace LinePlace;
  CPVT_LineInfo m_LineInfo;
};

class CPDF_EditContainer {
 public:
  CPDF_EditContainer();
  virtual ~CPDF_EditContainer();

  virtual void SetPlateRect(const CFX_FloatRect& rect);
  virtual const CFX_FloatRect& GetPlateRect() const;
  virtual void SetContentRect(const CPVT_FloatRect& rect);
  virtual CFX_FloatRect GetContentRect() const;

  FX_FLOAT GetPlateWidth() const { return m_rcPlate.right - m_rcPlate.left; }
  FX_FLOAT GetPlateHeight() const { return m_rcPlate.top - m_rcPlate.bottom; }
  CFX_SizeF GetPlateSize() const {
    return CFX_SizeF(GetPlateWidth(), GetPlateHeight());
  }
  CFX_FloatPoint GetBTPoint() const {
    return CFX_FloatPoint(m_rcPlate.left, m_rcPlate.top);
  }
  CFX_FloatPoint GetETPoint() const {
    return CFX_FloatPoint(m_rcPlate.right, m_rcPlate.bottom);
  }
  CFX_FloatPoint InToOut(const CFX_FloatPoint& point) const {
    return CFX_FloatPoint(point.x + GetBTPoint().x, GetBTPoint().y - point.y);
  }
  CFX_FloatPoint OutToIn(const CFX_FloatPoint& point) const {
    return CFX_FloatPoint(point.x - GetBTPoint().x, GetBTPoint().y - point.y);
  }
  CFX_FloatRect InToOut(const CPVT_FloatRect& rect) const {
    CFX_FloatPoint ptLeftTop = InToOut(CFX_FloatPoint(rect.left, rect.top));
    CFX_FloatPoint ptRightBottom =
        InToOut(CFX_FloatPoint(rect.right, rect.bottom));
    return CFX_FloatRect(ptLeftTop.x, ptRightBottom.y, ptRightBottom.x,
                         ptLeftTop.y);
  }
  CPVT_FloatRect OutToIn(const CFX_FloatRect& rect) const {
    CFX_FloatPoint ptLeftTop = OutToIn(CFX_FloatPoint(rect.left, rect.top));
    CFX_FloatPoint ptRightBottom =
        OutToIn(CFX_FloatPoint(rect.right, rect.bottom));
    return CPVT_FloatRect(ptLeftTop.x, ptLeftTop.y, ptRightBottom.x,
                          ptRightBottom.y);
  }

 private:
  CFX_FloatRect m_rcPlate;
  CPVT_FloatRect m_rcContent;
};

#endif  // CORE_FPDFDOC_PDF_VT_H_
