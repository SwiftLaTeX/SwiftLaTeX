// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_BINDITEMS_H_
#define XFA_FXFA_PARSER_CXFA_BINDITEMS_H_

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_BindItems : public CXFA_Data {
 public:
  explicit CXFA_BindItems(CXFA_Node* pNode);

  void GetLabelRef(CFX_WideStringC& wsLabelRef);
  void GetValueRef(CFX_WideStringC& wsValueRef);
  void GetRef(CFX_WideStringC& wsRef);
  FX_BOOL SetConnection(const CFX_WideString& wsConnection);
};

#endif  // XFA_FXFA_PARSER_CXFA_BINDITEMS_H_
