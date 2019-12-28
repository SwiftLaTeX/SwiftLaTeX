// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_EVENT_H_
#define XFA_FXFA_PARSER_CXFA_EVENT_H_

#include <stdint.h>

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxfa/parser/cxfa_submit.h"

class CXFA_Node;

class CXFA_Event : public CXFA_Data {
 public:
  explicit CXFA_Event(CXFA_Node* pNode);

  int32_t GetActivity();
  XFA_Element GetEventType();
  void GetRef(CFX_WideStringC& wsRef);

  CXFA_Script GetScript();
  CXFA_Submit GetSubmit();

  void GetSignDataTarget(CFX_WideString& wsTarget);
};

#endif  // XFA_FXFA_PARSER_CXFA_EVENT_H_
