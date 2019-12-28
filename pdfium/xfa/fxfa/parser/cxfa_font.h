// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_FONT_H_
#define XFA_FXFA_PARSER_CXFA_FONT_H_

#include "core/fxge/include/fx_dib.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_Font : public CXFA_Data {
 public:
  explicit CXFA_Font(CXFA_Node* pNode);

  FX_FLOAT GetBaselineShift();
  FX_FLOAT GetHorizontalScale();
  FX_FLOAT GetVerticalScale();
  FX_FLOAT GetLetterSpacing();
  int32_t GetLineThrough();
  int32_t GetUnderline();
  int32_t GetUnderlinePeriod();
  FX_FLOAT GetFontSize();
  void GetTypeface(CFX_WideStringC& wsTypeFace);

  FX_BOOL IsBold();
  FX_BOOL IsItalic();

  FX_ARGB GetColor();
  void SetColor(FX_ARGB color);
};

#endif  // XFA_FXFA_PARSER_CXFA_FONT_H_
