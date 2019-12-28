// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_VALIDATE_H_
#define XFA_FXFA_PARSER_CXFA_VALIDATE_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_script.h"

class CXFA_Node;

class CXFA_Validate : public CXFA_Data {
 public:
  explicit CXFA_Validate(CXFA_Node* pNode);

  int32_t GetFormatTest();
  int32_t GetNullTest();
  FX_BOOL SetNullTest(CFX_WideString wsValue);
  int32_t GetScriptTest();
  void GetFormatMessageText(CFX_WideString& wsMessage);
  void SetFormatMessageText(CFX_WideString wsMessage);
  void GetNullMessageText(CFX_WideString& wsMessage);
  void SetNullMessageText(CFX_WideString wsMessage);
  void GetScriptMessageText(CFX_WideString& wsMessage);
  void SetScriptMessageText(CFX_WideString wsMessage);
  void GetPicture(CFX_WideString& wsPicture);
  CXFA_Script GetScript();

 protected:
  void GetMessageText(CFX_WideString& wsMessage,
                      const CFX_WideString& wsMessageType);
  void SetMessageText(CFX_WideString& wsMessage,
                      const CFX_WideString& wsMessageType);
  FX_BOOL SetTestValue(int32_t iType,
                       CFX_WideString& wsValue,
                       XFA_ATTRIBUTEENUM eName);
};

#endif  // XFA_FXFA_PARSER_CXFA_VALIDATE_H_
