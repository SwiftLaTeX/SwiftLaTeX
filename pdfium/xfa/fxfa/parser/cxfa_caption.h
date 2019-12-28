// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_CAPTION_H_
#define XFA_FXFA_PARSER_CXFA_CAPTION_H_

#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_font.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_value.h"

class CXFA_Node;

class CXFA_Caption : public CXFA_Data {
 public:
  explicit CXFA_Caption(CXFA_Node* pNode);

  int32_t GetPresence();
  int32_t GetPlacementType();
  FX_FLOAT GetReserve();
  CXFA_Margin GetMargin();
  CXFA_Font GetFont();
  CXFA_Value GetValue();
};

#endif  // XFA_FXFA_PARSER_CXFA_CAPTION_H_
