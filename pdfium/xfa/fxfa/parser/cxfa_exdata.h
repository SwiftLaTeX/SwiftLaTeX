// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_EXDATA_H_
#define XFA_FXFA_PARSER_CXFA_EXDATA_H_

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_ExData : public CXFA_Data {
 public:
  explicit CXFA_ExData(CXFA_Node* pNode);

  FX_BOOL SetContentType(const CFX_WideString& wsContentType);
};

#endif  // XFA_FXFA_PARSER_CXFA_EXDATA_H_
