// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_CORNER_H_
#define XFA_FXFA_PARSER_CXFA_CORNER_H_

#include "xfa/fxfa/parser/cxfa_stroke.h"

class CXFA_Node;

class CXFA_Corner : public CXFA_Stroke {
 public:
  explicit CXFA_Corner(CXFA_Node* pNode) : CXFA_Stroke(pNode) {}
};

#endif  // XFA_FXFA_PARSER_CXFA_CORNER_H_
