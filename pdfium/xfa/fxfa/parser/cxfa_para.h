// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_PARA_H_
#define XFA_FXFA_PARSER_CXFA_PARA_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_Para : public CXFA_Data {
 public:
  explicit CXFA_Para(CXFA_Node* pNode);

  int32_t GetHorizontalAlign();
  int32_t GetVerticalAlign();
  FX_FLOAT GetLineHeight();
  FX_FLOAT GetMarginLeft();
  FX_FLOAT GetMarginRight();
  FX_FLOAT GetSpaceAbove();
  FX_FLOAT GetSpaceBelow();
  FX_FLOAT GetTextIndent();
};

#endif  // XFA_FXFA_PARSER_CXFA_PARA_H_
