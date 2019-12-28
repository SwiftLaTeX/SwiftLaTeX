// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_ARC_H_
#define XFA_FXFA_PARSER_CXFA_ARC_H_

#include "xfa/fxfa/parser/cxfa_box.h"

class CXFA_Node;

class CXFA_Arc : public CXFA_Box {
 public:
  explicit CXFA_Arc(CXFA_Node* pNode) : CXFA_Box(pNode) {}
};

#endif  // XFA_FXFA_PARSER_CXFA_ARC_H_
