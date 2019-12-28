// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BIND_H_
#define XFA_FXFA_PARSER_CXFA_BIND_H_

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_Bind : public CXFA_Data {
 public:
  explicit CXFA_Bind(CXFA_Node* pNode);

  void GetPicture(CFX_WideString& wsPicture);
};

#endif  // XFA_FXFA_PARSER_CXFA_BIND_H_
