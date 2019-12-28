// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_LOGPSEUDOMODEL_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_LOGPSEUDOMODEL_H_

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/parser/xfa_object.h"

class CScript_LogPseudoModel : public CXFA_Object {
 public:
  explicit CScript_LogPseudoModel(CXFA_Document* pDocument);
  ~CScript_LogPseudoModel() override;

  void Message(CFXJSE_Arguments* pArguments);
  void TraceEnabled(CFXJSE_Arguments* pArguments);
  void TraceActivate(CFXJSE_Arguments* pArguments);
  void TraceDeactivate(CFXJSE_Arguments* pArguments);
  void Trace(CFXJSE_Arguments* pArguments);
};

#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_LOGPSEUDOMODEL_H_
