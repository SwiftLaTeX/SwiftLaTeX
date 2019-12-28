// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_SCRIPT_H_
#define XFA_FXFA_PARSER_CXFA_SCRIPT_H_

#include <stdint.h>

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_data.h"

enum XFA_SCRIPTTYPE {
  XFA_SCRIPTTYPE_Formcalc = 0,
  XFA_SCRIPTTYPE_Javascript,
  XFA_SCRIPTTYPE_Unkown,
};

class CXFA_Node;

class CXFA_Script : public CXFA_Data {
 public:
  explicit CXFA_Script(CXFA_Node* pNode);

  XFA_SCRIPTTYPE GetContentType();
  int32_t GetRunAt();
  void GetExpression(CFX_WideString& wsExpression);
};

#endif  // XFA_FXFA_PARSER_CXFA_SCRIPT_H_
