// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LINE_H_
#define XFA_FXFA_PARSER_CXFA_LINE_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_edge.h"

class CXFA_Node;

class CXFA_Line : public CXFA_Data {
 public:
  explicit CXFA_Line(CXFA_Node* pNode) : CXFA_Data(pNode) {}

  int32_t GetHand();
  FX_BOOL GetSlop();
  CXFA_Edge GetEdge();
};

#endif  // XFA_FXFA_PARSER_CXFA_LINE_H_
