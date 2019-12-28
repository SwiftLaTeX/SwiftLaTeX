// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTPAGE_H_
#define XFA_FDE_CFDE_TXTEDTPAGE_H_

#include "xfa/fde/ifde_txtedtpage.h"
#include "xfa/fde/ifx_chariter.h"

class CFDE_TxtEdtEngine;
class CFDE_TxtEdtParag;
class CFDE_TxtEdtTextSet;

class CFDE_TxtEdtPage : public IFDE_TxtEdtPage {
 public:
  CFDE_TxtEdtPage(CFDE_TxtEdtEngine* pEngine, int32_t nLineIndex);
  ~CFDE_TxtEdtPage() override;

  // IFDE_TxtEditPage:
  CFDE_TxtEdtEngine* GetEngine() const override;
  int32_t GetCharRect(int32_t nIndex,
                      CFX_RectF& rect,
                      FX_BOOL bBBox = FALSE) const override;
  int32_t GetCharIndex(const CFX_PointF& fPoint, FX_BOOL& bBefore) override;
  void CalcRangeRectArray(int32_t nStart,
                          int32_t nCount,
                          CFX_RectFArray& RectFArr) const override;
  int32_t SelectWord(const CFX_PointF& fPoint, int32_t& nCount) override;
  int32_t GetCharStart() const override;
  int32_t GetCharCount() const override;
  int32_t GetDisplayPos(const CFX_RectF& rtClip,
                        FXTEXT_CHARPOS*& pCharPos,
                        CFX_RectF* pBBox) const override;
  FX_BOOL IsLoaded(const CFX_RectF* pClipBox) override;
  int32_t LoadPage(const CFX_RectF* pClipBox, IFX_Pause* pPause) override;
  void UnloadPage(const CFX_RectF* pClipBox) override;
  const CFX_RectF& GetContentsBox() override;

  // IFDE_VisualSet:
  FDE_VISUALOBJTYPE GetType() override;
  void GetRect(FDE_TEXTEDITPIECE* pPiece, CFX_RectF& rt) override;

  // IFDE_CanvasSet:
  FX_POSITION GetFirstPosition() override;
  FDE_TEXTEDITPIECE* GetNext(FX_POSITION& pos,
                             IFDE_VisualSet*& pVisualSet) override;

  // IFX_TxtAccess:
  FX_WCHAR GetChar(const FDE_TEXTEDITPIECE* pIdentity,
                   int32_t index) const override;
  int32_t GetWidth(const FDE_TEXTEDITPIECE* pIdentity,
                   int32_t index) const override;

 private:
  void NormalizePt2Rect(CFX_PointF& ptF,
                        const CFX_RectF& rtF,
                        FX_FLOAT fTolerance) const;

  std::unique_ptr<IFX_CharIter> m_pIter;
  CFDE_TxtEdtTextSet* m_pTextSet;
  CFDE_TxtEdtEngine* m_pEditEngine;
  CFX_MassArrayTemplate<FDE_TEXTEDITPIECE> m_PieceMassArr;
  CFDE_TxtEdtParag* m_pBgnParag;
  CFDE_TxtEdtParag* m_pEndParag;
  int32_t m_nRefCount;
  int32_t m_nPageStart;
  int32_t m_nCharCount;
  int32_t m_nPageIndex;
  FX_BOOL m_bLoaded;
  CFX_RectF m_rtPage;
  CFX_RectF m_rtPageMargin;
  CFX_RectF m_rtPageContents;
  CFX_RectF m_rtPageCanvas;
  int32_t* m_pCharWidth;
};

#endif  // XFA_FDE_CFDE_TXTEDTPAGE_H_
