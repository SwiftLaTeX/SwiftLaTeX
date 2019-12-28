// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_ASSIST_H_
#define XFA_FXFA_PARSER_CXFA_ASSIST_H_

#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_tooltip.h"

class CXFA_Node;

class CXFA_Assist : public CXFA_Data {
 public:
  explicit CXFA_Assist(CXFA_Node* pNode);

  CXFA_ToolTip GetToolTip();
};

#endif  // XFA_FXFA_PARSER_CXFA_ASSIST_H_
