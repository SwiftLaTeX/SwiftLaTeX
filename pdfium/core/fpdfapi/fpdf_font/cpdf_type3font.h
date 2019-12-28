// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_FONT_CPDF_TYPE3FONT_H_
#define CORE_FPDFAPI_FPDF_FONT_CPDF_TYPE3FONT_H_

#include <map>

#include "core/fpdfapi/fpdf_font/cpdf_simplefont.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Dictionary;
class CPDF_Type3Char;

class CPDF_Type3Font : public CPDF_SimpleFont {
 public:
  CPDF_Type3Font();
  ~CPDF_Type3Font() override;

  // CPDF_Font:
  bool IsType3Font() const override;
  const CPDF_Type3Font* AsType3Font() const override;
  CPDF_Type3Font* AsType3Font() override;
  int GetCharWidthF(uint32_t charcode, int level = 0) override;
  FX_RECT GetCharBBox(uint32_t charcode, int level = 0) override;

  void SetPageResources(CPDF_Dictionary* pResources) {
    m_pPageResources = pResources;
  }
  CPDF_Type3Char* LoadChar(uint32_t charcode, int level = 0);
  void CheckType3FontMetrics();

  CFX_Matrix& GetFontMatrix() { return m_FontMatrix; }

 protected:
  CFX_Matrix m_FontMatrix;

 private:
  // CPDF_Font:
  FX_BOOL Load() override;

  // CPDF_SimpleFont:
  void LoadGlyphMap() override {}

  int m_CharWidthL[256];
  CPDF_Dictionary* m_pCharProcs;
  CPDF_Dictionary* m_pPageResources;
  CPDF_Dictionary* m_pFontResources;
  std::map<uint32_t, CPDF_Type3Char*> m_CacheMap;
};

#endif  // CORE_FPDFAPI_FPDF_FONT_CPDF_TYPE3FONT_H_
