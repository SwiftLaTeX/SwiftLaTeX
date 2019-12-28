// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_TEXTSTATEDATA_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_TEXTSTATEDATA_H_

#include "core/fxcrt/include/fx_system.h"

class CPDF_Font;
class CPDF_Document;

// See PDF Reference 1.7, page 402, table 5.3.
enum class TextRenderingMode {
  MODE_FILL = 0,
  MODE_STROKE = 1,
  MODE_FILL_STROKE = 2,
  MODE_INVISIBLE = 3,
  MODE_FILL_CLIP = 4,
  MODE_STROKE_CLIP = 5,
  MODE_FILL_STROKE_CLIP = 6,
  MODE_CLIP = 7,
};

bool SetTextRenderingModeFromInt(int iMode, TextRenderingMode* mode);
bool TextRenderingModeIsClipMode(const TextRenderingMode& mode);
bool TextRenderingModeIsStrokeMode(const TextRenderingMode& mode);

class CPDF_TextStateData {
 public:
  CPDF_TextStateData();
  CPDF_TextStateData(const CPDF_TextStateData& src);
  ~CPDF_TextStateData();

  CPDF_Font* m_pFont;
  CPDF_Document* m_pDocument;
  FX_FLOAT m_FontSize;
  FX_FLOAT m_CharSpace;
  FX_FLOAT m_WordSpace;
  TextRenderingMode m_TextMode;
  FX_FLOAT m_Matrix[4];
  FX_FLOAT m_CTM[4];
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_TEXTSTATEDATA_H_
