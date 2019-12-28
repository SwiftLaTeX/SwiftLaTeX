// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_IFDE_TXTEDTPAGE_H_
#define XFA_FDE_IFDE_TXTEDTPAGE_H_

#include "xfa/fde/fde_visualset.h"
#include "xfa/fgas/layout/fgas_textbreak.h"

class CFDE_TxtEdtEngine;

class IFDE_TxtEdtPage : public IFDE_CanvasSet, public IFX_TxtAccess {
 public:
  static IFDE_TxtEdtPage* Create(CFDE_TxtEdtEngine* pEngine, int32_t nIndex);
  ~IFDE_TxtEdtPage() override {}

  virtual CFDE_TxtEdtEngine* GetEngine() const = 0;
  virtual int32_t GetCharRect(int32_t nIndex,
                              CFX_RectF& rect,
                              FX_BOOL bBBox = FALSE) const = 0;
  virtual int32_t GetCharIndex(const CFX_PointF& fPoint, FX_BOOL& bBefore) = 0;
  virtual void CalcRangeRectArray(int32_t nStart,
                                  int32_t nCount,
                                  CFX_RectFArray& RectFArr) const = 0;
  virtual int32_t SelectWord(const CFX_PointF& fPoint, int32_t& nCount) = 0;
  virtual int32_t GetCharStart() const = 0;
  virtual int32_t GetCharCount() const = 0;

  virtual int32_t GetDisplayPos(const CFX_RectF& rtClip,
                                FXTEXT_CHARPOS*& pCharPos,
                                CFX_RectF* pBBox) const = 0;
  virtual FX_BOOL IsLoaded(const CFX_RectF* pClipBox) = 0;
  virtual int32_t LoadPage(const CFX_RectF* pClipBox, IFX_Pause* pPause) = 0;
  virtual void UnloadPage(const CFX_RectF* pClipBox) = 0;
  virtual const CFX_RectF& GetContentsBox() = 0;
};

#endif  // XFA_FDE_IFDE_TXTEDTPAGE_H_
