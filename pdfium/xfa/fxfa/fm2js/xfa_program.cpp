// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_program.h"

CXFA_FMProgram::CXFA_FMProgram() : m_globalFunction(0) {}
CXFA_FMProgram::~CXFA_FMProgram() {
  if (m_globalFunction != 0) {
    delete m_globalFunction;
    m_globalFunction = 0;
  }
}
int32_t CXFA_FMProgram::Init(const CFX_WideStringC& wsFormcalc) {
  return m_parse.Init(wsFormcalc, &m_pErrorInfo);
}
int32_t CXFA_FMProgram::ParseProgram() {
  CFX_ArrayTemplate<CXFA_FMExpression*>* expressions = nullptr;
  m_parse.NextToken();
  if (!m_pErrorInfo.message.IsEmpty()) {
    return -1;
  }
  expressions = m_parse.ParseTopExpression();
  if (!m_pErrorInfo.message.IsEmpty()) {
    for (int32_t u = 0; u < expressions->GetSize(); ++u)
      delete expressions->GetAt(u);

    delete expressions;
    return -1;
  }
  m_globalFunction =
      new CXFA_FMFunctionDefinition(1, 1, FX_WSTRC(L""), 0, expressions);
  return 0;
}
int32_t CXFA_FMProgram::TranslateProgram(CFX_WideTextBuf& wsJavaScript) {
  m_globalFunction->ToJavaScript(wsJavaScript);
  wsJavaScript.AppendChar(0);
  return 0;
}
