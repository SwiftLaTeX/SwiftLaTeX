// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTTEXTSET_H_
#define XFA_FDE_CFDE_TXTEDTTEXTSET_H_

#include "xfa/fde/fde_visualset.h"

class CFDE_TxtEdtPage;

class CFDE_TxtEdtTextSet : public IFDE_TextSet {
 public:
  explicit CFDE_TxtEdtTextSet(CFDE_TxtEdtPage* pPage);
  ~CFDE_TxtEdtTextSet() override;

  // IFDE_VisualSet
  FDE_VISUALOBJTYPE GetType() override;
  void GetRect(FDE_TEXTEDITPIECE* hVisualObj, CFX_RectF& rt) override;

  // IFDE_TextSet
  int32_t GetString(FDE_TEXTEDITPIECE* pPiece, CFX_WideString& wsText) override;
  CFGAS_GEFont* GetFont() override;
  FX_FLOAT GetFontSize() override;
  FX_ARGB GetFontColor() override;
  int32_t GetDisplayPos(FDE_TEXTEDITPIECE* pPiece,
                        FXTEXT_CHARPOS* pCharPos,
                        FX_BOOL bCharCode = FALSE,
                        CFX_WideString* pWSForms = nullptr) override;
  int32_t GetCharRects(const FDE_TEXTEDITPIECE* pPiece,
                       CFX_RectFArray& rtArray,
                       FX_BOOL bBBox) override;

 private:
  CFDE_TxtEdtPage* const m_pPage;
};

#endif  // XFA_FDE_CFDE_TXTEDTTEXTSET_H_
