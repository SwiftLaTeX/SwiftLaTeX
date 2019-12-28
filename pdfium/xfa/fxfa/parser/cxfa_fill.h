// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_FILL_H_
#define XFA_FXFA_PARSER_CXFA_FILL_H_

#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_Fill : public CXFA_Data {
 public:
  explicit CXFA_Fill(CXFA_Node* pNode);
  ~CXFA_Fill();

  int32_t GetPresence();
  FX_ARGB GetColor(FX_BOOL bText = FALSE);
  XFA_Element GetFillType();
  int32_t GetPattern(FX_ARGB& foreColor);
  int32_t GetStipple(FX_ARGB& stippleColor);
  int32_t GetLinear(FX_ARGB& endColor);
  int32_t GetRadial(FX_ARGB& endColor);
  void SetColor(FX_ARGB color);
};

#endif  // XFA_FXFA_PARSER_CXFA_FILL_H_
