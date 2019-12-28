// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_MARGIN_H_
#define XFA_FXFA_PARSER_CXFA_MARGIN_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_data.h"

class CXFA_Node;

class CXFA_Margin : public CXFA_Data {
 public:
  explicit CXFA_Margin(CXFA_Node* pNode);

  FX_BOOL GetLeftInset(FX_FLOAT& fInset, FX_FLOAT fDefInset = 0) const;
  FX_BOOL GetTopInset(FX_FLOAT& fInset, FX_FLOAT fDefInset = 0) const;
  FX_BOOL GetRightInset(FX_FLOAT& fInset, FX_FLOAT fDefInset = 0) const;
  FX_BOOL GetBottomInset(FX_FLOAT& fInset, FX_FLOAT fDefInset = 0) const;
};

#endif  // XFA_FXFA_PARSER_CXFA_MARGIN_H_
