// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_expression.h"

#include "core/fxcrt/include/fx_basic.h"

namespace {

const FX_WCHAR RUNTIMEBLOCKTEMPARRAY[] =
    L"foxit_xfa_formcalc_runtime_block_temp_array";

const FX_WCHAR RUNTIMEBLOCKTEMPARRAYINDEX[] =
    L"foxit_xfa_formcalc_runtime_block_temp_array_index";

}  // namespace

CXFA_FMExpression::CXFA_FMExpression(uint32_t line)
    : m_type(XFA_FM_EXPTYPE_UNKNOWN), m_line(line) {}

CXFA_FMExpression::CXFA_FMExpression(uint32_t line, XFA_FM_EXPTYPE type)
    : m_type(type), m_line(line) {}

void CXFA_FMExpression::ToJavaScript(CFX_WideTextBuf& javascript) {}

void CXFA_FMExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {}

CXFA_FMFunctionDefinition::CXFA_FMFunctionDefinition(
    uint32_t line,
    FX_BOOL isGlobal,
    const CFX_WideStringC& wsName,
    CFX_WideStringCArray* pArguments,
    CFX_ArrayTemplate<CXFA_FMExpression*>* pExpressions)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_FUNC),
      m_wsName(wsName),
      m_pArguments(pArguments),
      m_pExpressions(pExpressions),
      m_isGlobal(isGlobal) {}

CXFA_FMFunctionDefinition::~CXFA_FMFunctionDefinition() {
  delete m_pArguments;
  if (m_pExpressions) {
    for (int i = 0; i < m_pExpressions->GetSize(); ++i)
      delete m_pExpressions->GetAt(i);

    delete m_pExpressions;
  }
}

void CXFA_FMFunctionDefinition::ToJavaScript(CFX_WideTextBuf& javascript) {
  if (m_isGlobal && (!m_pExpressions || m_pExpressions->GetSize() == 0)) {
    javascript << FX_WSTRC(L"// comments only");
    return;
  }
  if (m_isGlobal) {
    javascript << FX_WSTRC(L"(\n");
  }
  javascript << FX_WSTRC(L"function ");
  if (m_wsName.GetAt(0) == L'!') {
    CFX_WideString tempName = EXCLAMATION_IN_IDENTIFIER + m_wsName.Mid(1);
    javascript << tempName;
  } else {
    javascript << m_wsName;
  }
  javascript << FX_WSTRC(L"(");
  if (m_pArguments != 0) {
    CFX_WideStringC identifier = 0;
    for (int i = 0; i < m_pArguments->GetSize(); ++i) {
      identifier = m_pArguments->GetAt(i);
      if (identifier.GetAt(0) == L'!') {
        CFX_WideString tempIdentifier =
            EXCLAMATION_IN_IDENTIFIER + identifier.Mid(1);
        javascript << tempIdentifier;
      } else {
        javascript << identifier;
      }
      if (i + 1 < m_pArguments->GetSize()) {
        javascript << FX_WSTRC(L", ");
      }
    }
  }
  javascript << FX_WSTRC(L")\n{\n");
  javascript << FX_WSTRC(L"var ");
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = null;\n");
  if (m_pExpressions) {
    for (int i = 0; i < m_pExpressions->GetSize(); ++i) {
      CXFA_FMExpression* e = m_pExpressions->GetAt(i);
      if (i + 1 < m_pExpressions->GetSize()) {
        e->ToJavaScript(javascript);
      } else {
        e->ToImpliedReturnJS(javascript);
      }
    }
  }
  javascript << FX_WSTRC(L"return ");
  if (m_isGlobal) {
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    javascript << RUNTIMEFUNCTIONRETURNVALUE;
    javascript << FX_WSTRC(L")");
  } else {
    javascript << RUNTIMEFUNCTIONRETURNVALUE;
  }
  javascript << FX_WSTRC(L";\n}\n");
  if (m_isGlobal) {
    javascript << FX_WSTRC(L").call(this);\n");
  }
}

void CXFA_FMFunctionDefinition::ToImpliedReturnJS(CFX_WideTextBuf&) {}

CXFA_FMVarExpression::CXFA_FMVarExpression(uint32_t line,
                                           const CFX_WideStringC& wsName,
                                           CXFA_FMExpression* pInit)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_VAR),
      m_wsName(wsName),
      m_pInit(pInit) {}

CXFA_FMVarExpression::~CXFA_FMVarExpression() {}

void CXFA_FMVarExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"var ");
  CFX_WideString tempName(m_wsName);
  if (m_wsName.GetAt(0) == L'!') {
    tempName = EXCLAMATION_IN_IDENTIFIER + m_wsName.Mid(1);
  }
  javascript << tempName;
  javascript << FX_WSTRC(L" = ");
  if (m_pInit) {
    m_pInit->ToJavaScript(javascript);
    javascript << tempName;
    javascript << FX_WSTRC(L" = ");
    javascript << XFA_FM_EXPTypeToString(VARFILTER);
    javascript << FX_WSTRC(L"(");
    javascript << tempName;
    javascript << FX_WSTRC(L");\n");
  } else {
    javascript << FX_WSTRC(L"\"\";\n");
  }
}

void CXFA_FMVarExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"var ");
  CFX_WideString tempName(m_wsName);
  if (m_wsName.GetAt(0) == L'!') {
    tempName = EXCLAMATION_IN_IDENTIFIER + m_wsName.Mid(1);
  }
  javascript << tempName;
  javascript << FX_WSTRC(L" = ");
  if (m_pInit) {
    m_pInit->ToJavaScript(javascript);
    javascript << tempName;
    javascript << FX_WSTRC(L" = ");
    javascript << XFA_FM_EXPTypeToString(VARFILTER);
    javascript << FX_WSTRC(L"(");
    javascript << tempName;
    javascript << FX_WSTRC(L");\n");
  } else {
    javascript << FX_WSTRC(L"\"\";\n");
  }
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = ");
  javascript << tempName;
  javascript << FX_WSTRC(L";\n");
}

CXFA_FMExpExpression::CXFA_FMExpExpression(uint32_t line,
                                           CXFA_FMSimpleExpression* pExpression)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_EXP), m_pExpression(pExpression) {}

CXFA_FMExpExpression::~CXFA_FMExpExpression() {}

void CXFA_FMExpExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  if (m_pExpression->GetOperatorToken() == TOKassign) {
    m_pExpression->ToJavaScript(javascript);
  } else {
    m_pExpression->ToJavaScript(javascript);
    javascript << FX_WSTRC(L";\n");
  }
}

void CXFA_FMExpExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  if (m_pExpression->GetOperatorToken() == TOKassign) {
    m_pExpression->ToImpliedReturnJS(javascript);
  } else {
    if (m_pExpression->GetOperatorToken() == TOKstar ||
        m_pExpression->GetOperatorToken() == TOKdotstar ||
        m_pExpression->GetOperatorToken() == TOKdotscream ||
        m_pExpression->GetOperatorToken() == TOKdotdot ||
        m_pExpression->GetOperatorToken() == TOKdot) {
      javascript << RUNTIMEFUNCTIONRETURNVALUE;
      javascript << FX_WSTRC(L" = ");
      javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
      javascript << FX_WSTRC(L"(");
      m_pExpression->ToJavaScript(javascript);
      javascript << FX_WSTRC(L");\n");
    } else {
      javascript << RUNTIMEFUNCTIONRETURNVALUE;
      javascript << FX_WSTRC(L" = ");
      m_pExpression->ToJavaScript(javascript);
      javascript << FX_WSTRC(L";\n");
    }
  }
}

CXFA_FMBlockExpression::CXFA_FMBlockExpression(
    uint32_t line,
    CFX_ArrayTemplate<CXFA_FMExpression*>* pExpressionList)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_BLOCK),
      m_pExpressionList(pExpressionList) {}

CXFA_FMBlockExpression::~CXFA_FMBlockExpression() {
  if (m_pExpressionList) {
    for (int i = 0; i < m_pExpressionList->GetSize(); ++i)
      delete m_pExpressionList->GetAt(i);

    delete m_pExpressionList;
  }
}

void CXFA_FMBlockExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"{\n");
  if (m_pExpressionList) {
    for (int i = 0; i < m_pExpressionList->GetSize(); ++i)
      m_pExpressionList->GetAt(i)->ToJavaScript(javascript);
  }
  javascript << FX_WSTRC(L"}\n");
}

void CXFA_FMBlockExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"{\n");
  if (m_pExpressionList) {
    for (int i = 0; i < m_pExpressionList->GetSize(); ++i) {
      CXFA_FMExpression* e = m_pExpressionList->GetAt(i);
      if (i + 1 == m_pExpressionList->GetSize()) {
        e->ToImpliedReturnJS(javascript);
      } else {
        e->ToJavaScript(javascript);
      }
    }
  }
  javascript << FX_WSTRC(L"}\n");
}

CXFA_FMDoExpression::CXFA_FMDoExpression(uint32_t line,
                                         CXFA_FMExpression* pList)
    : CXFA_FMExpression(line), m_pList(pList) {}

CXFA_FMDoExpression::~CXFA_FMDoExpression() {}

void CXFA_FMDoExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  m_pList->ToJavaScript(javascript);
}

void CXFA_FMDoExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  m_pList->ToImpliedReturnJS(javascript);
}

CXFA_FMIfExpression::CXFA_FMIfExpression(uint32_t line,
                                         CXFA_FMSimpleExpression* pExpression,
                                         CXFA_FMExpression* pIfExpression,
                                         CXFA_FMExpression* pElseExpression)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_IF),
      m_pExpression(pExpression),
      m_pIfExpression(pIfExpression),
      m_pElseExpression(pElseExpression) {}

CXFA_FMIfExpression::~CXFA_FMIfExpression() {}

void CXFA_FMIfExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"if (");
  if (m_pExpression) {
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pExpression->ToJavaScript(javascript);
    javascript << FX_WSTRC(L")");
  }
  javascript << FX_WSTRC(L")\n");
  if (m_pIfExpression) {
    m_pIfExpression->ToJavaScript(javascript);
  }
  if (m_pElseExpression) {
    if (m_pElseExpression->GetExpType() == XFA_FM_EXPTYPE_IF) {
      javascript << FX_WSTRC(L"else\n");
      javascript << FX_WSTRC(L"{\n");
      m_pElseExpression->ToJavaScript(javascript);
      javascript << FX_WSTRC(L"}\n");
    } else {
      javascript << FX_WSTRC(L"else\n");
      m_pElseExpression->ToJavaScript(javascript);
    }
  }
}

void CXFA_FMIfExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"if (");
  if (m_pExpression) {
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pExpression->ToJavaScript(javascript);
    javascript << FX_WSTRC(L")");
  }
  javascript << FX_WSTRC(L")\n");
  if (m_pIfExpression) {
    m_pIfExpression->ToImpliedReturnJS(javascript);
  }
  if (m_pElseExpression) {
    if (m_pElseExpression->GetExpType() == XFA_FM_EXPTYPE_IF) {
      javascript << FX_WSTRC(L"else\n");
      javascript << FX_WSTRC(L"{\n");
      m_pElseExpression->ToImpliedReturnJS(javascript);
      javascript << FX_WSTRC(L"}\n");
    } else {
      javascript << FX_WSTRC(L"else\n");
      m_pElseExpression->ToImpliedReturnJS(javascript);
    }
  }
}

CXFA_FMLoopExpression::~CXFA_FMLoopExpression() {}

void CXFA_FMLoopExpression::ToJavaScript(CFX_WideTextBuf& javascript) {}

void CXFA_FMLoopExpression::ToImpliedReturnJS(CFX_WideTextBuf&) {}

CXFA_FMWhileExpression::CXFA_FMWhileExpression(
    uint32_t line,
    CXFA_FMSimpleExpression* pCondition,
    CXFA_FMExpression* pExpression)
    : CXFA_FMLoopExpression(line),
      m_pCondition(pCondition),
      m_pExpression(pExpression) {}

CXFA_FMWhileExpression::~CXFA_FMWhileExpression() {}

void CXFA_FMWhileExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"while (");
  m_pCondition->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")\n");
  m_pExpression->ToJavaScript(javascript);
}

void CXFA_FMWhileExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"while (");
  m_pCondition->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")\n");
  m_pExpression->ToImpliedReturnJS(javascript);
}

CXFA_FMBreakExpression::CXFA_FMBreakExpression(uint32_t line)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_BREAK) {}

CXFA_FMBreakExpression::~CXFA_FMBreakExpression() {}

void CXFA_FMBreakExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"break;\n");
}

void CXFA_FMBreakExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"break;\n");
}

CXFA_FMContinueExpression::CXFA_FMContinueExpression(uint32_t line)
    : CXFA_FMExpression(line, XFA_FM_EXPTYPE_CONTINUE) {}

CXFA_FMContinueExpression::~CXFA_FMContinueExpression() {}

void CXFA_FMContinueExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"continue;\n");
}

void CXFA_FMContinueExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"continue;\n");
}

CXFA_FMForExpression::CXFA_FMForExpression(uint32_t line,
                                           const CFX_WideStringC& wsVariant,
                                           CXFA_FMSimpleExpression* pAssignment,
                                           CXFA_FMSimpleExpression* pAccessor,
                                           int32_t iDirection,
                                           CXFA_FMSimpleExpression* pStep,
                                           CXFA_FMExpression* pList)
    : CXFA_FMLoopExpression(line),
      m_wsVariant(wsVariant),
      m_pAssignment(pAssignment),
      m_pAccessor(pAccessor),
      m_iDirection(iDirection),
      m_pStep(pStep),
      m_pList(pList) {}

CXFA_FMForExpression::~CXFA_FMForExpression() {}

void CXFA_FMForExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"{\nvar ");
  CFX_WideString tempVariant;
  if (m_wsVariant.GetAt(0) == L'!') {
    tempVariant = EXCLAMATION_IN_IDENTIFIER + m_wsVariant.Mid(1);
    javascript << tempVariant;
  } else {
    tempVariant = m_wsVariant;
    javascript << m_wsVariant;
  }
  javascript << FX_WSTRC(L" = null;\n");
  javascript << FX_WSTRC(L"for (");
  javascript << tempVariant;
  javascript << FX_WSTRC(L" = ");
  javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
  javascript << FX_WSTRC(L"(");
  m_pAssignment->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"); ");
  javascript << tempVariant;
  if (m_iDirection == 1) {
    javascript << FX_WSTRC(L" <= ");
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pAccessor->ToJavaScript(javascript);
    javascript << FX_WSTRC(L"); ");
    javascript << tempVariant;
    javascript << FX_WSTRC(L" += ");
  } else {
    javascript << FX_WSTRC(L" >= ");
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pAccessor->ToJavaScript(javascript);
    javascript << FX_WSTRC(L"); ");
    javascript << tempVariant;
    javascript << FX_WSTRC(L" -= ");
  }
  if (m_pStep) {
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pStep->ToJavaScript(javascript);
    javascript << FX_WSTRC(L")");
  } else {
    javascript << FX_WSTRC(L"1");
  }
  javascript << FX_WSTRC(L")\n");
  m_pList->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"}\n");
}

void CXFA_FMForExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"{\nvar ");
  CFX_WideString tempVariant;
  if (m_wsVariant.GetAt(0) == L'!') {
    tempVariant = EXCLAMATION_IN_IDENTIFIER + m_wsVariant.Mid(1);
    javascript << tempVariant;
  } else {
    tempVariant = m_wsVariant;
    javascript << m_wsVariant;
  }
  javascript << FX_WSTRC(L" = null;\n");
  javascript << FX_WSTRC(L"for (");
  javascript << tempVariant;
  javascript << FX_WSTRC(L" = ");
  javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
  javascript << FX_WSTRC(L"(");
  m_pAssignment->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"); ");
  javascript << tempVariant;
  if (m_iDirection == 1) {
    javascript << FX_WSTRC(L" <= ");
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pAccessor->ToJavaScript(javascript);
    javascript << FX_WSTRC(L"); ");
    javascript << tempVariant;
    javascript << FX_WSTRC(L" += ");
  } else {
    javascript << FX_WSTRC(L" >= ");
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pAccessor->ToJavaScript(javascript);
    javascript << FX_WSTRC(L"); ");
    javascript << tempVariant;
    javascript << FX_WSTRC(L" -= ");
  }
  if (m_pStep) {
    javascript << XFA_FM_EXPTypeToString(GETFMVALUE);
    javascript << FX_WSTRC(L"(");
    m_pStep->ToJavaScript(javascript);
    javascript << FX_WSTRC(L")");
  } else {
    javascript << FX_WSTRC(L"1");
  }
  javascript << FX_WSTRC(L")\n");
  m_pList->ToImpliedReturnJS(javascript);
  javascript << FX_WSTRC(L"}\n");
}

CXFA_FMForeachExpression::CXFA_FMForeachExpression(
    uint32_t line,
    const CFX_WideStringC& wsIdentifier,
    CFX_ArrayTemplate<CXFA_FMSimpleExpression*>* pAccessors,
    CXFA_FMExpression* pList)
    : CXFA_FMLoopExpression(line),
      m_wsIdentifier(wsIdentifier),
      m_pAccessors(pAccessors),
      m_pList(pList) {}

CXFA_FMForeachExpression::~CXFA_FMForeachExpression() {
  if (m_pAccessors) {
    for (int i = 0; i < m_pAccessors->GetSize(); ++i)
      m_pAccessors->GetAt(i);

    delete m_pAccessors;
  }
}

void CXFA_FMForeachExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"{\n");
  javascript << FX_WSTRC(L"var ");
  if (m_wsIdentifier.GetAt(0) == L'!') {
    CFX_WideString tempIdentifier =
        EXCLAMATION_IN_IDENTIFIER + m_wsIdentifier.Mid(1);
    javascript << tempIdentifier;
  } else {
    javascript << m_wsIdentifier;
  }
  javascript << FX_WSTRC(L" = null;\n");
  javascript << FX_WSTRC(L"var ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L" = ");
  javascript << XFA_FM_EXPTypeToString(CONCATFMOBJECT);
  javascript << FX_WSTRC(L"(");

  for (int i = 0; i < m_pAccessors->GetSize(); ++i) {
    CXFA_FMSimpleExpression* s = m_pAccessors->GetAt(i);
    s->ToJavaScript(javascript);
    if (i + 1 < m_pAccessors->GetSize()) {
      javascript << FX_WSTRC(L", ");
    }
  }
  javascript << FX_WSTRC(L");\n");
  javascript << FX_WSTRC(L"var ");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"while(");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L" < ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L".length)\n{\n");
  if (m_wsIdentifier.GetAt(0) == L'!') {
    CFX_WideString tempIdentifier =
        EXCLAMATION_IN_IDENTIFIER + m_wsIdentifier.Mid(1);
    javascript << tempIdentifier;
  } else {
    javascript << m_wsIdentifier;
  }
  javascript << FX_WSTRC(L" = ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L"[");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L"++];\n");
  m_pList->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"}\n");
  javascript << FX_WSTRC(L"}\n");
}

void CXFA_FMForeachExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"{\n");
  javascript << FX_WSTRC(L"var ");
  if (m_wsIdentifier.GetAt(0) == L'!') {
    CFX_WideString tempIdentifier =
        EXCLAMATION_IN_IDENTIFIER + m_wsIdentifier.Mid(1);
    javascript << tempIdentifier;
  } else {
    javascript << m_wsIdentifier;
  }
  javascript << FX_WSTRC(L" = null;\n");
  javascript << FX_WSTRC(L"var ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L" = ");
  javascript << XFA_FM_EXPTypeToString(CONCATFMOBJECT);
  javascript << FX_WSTRC(L"(");
  for (int i = 0; i < m_pAccessors->GetSize(); ++i) {
    CXFA_FMSimpleExpression* s = m_pAccessors->GetAt(i);
    s->ToJavaScript(javascript);
    if (i + 1 < m_pAccessors->GetSize()) {
      javascript << FX_WSTRC(L", ");
    }
  }
  javascript << FX_WSTRC(L");\n");
  javascript << FX_WSTRC(L"var ");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L" = 0;\n");
  javascript << FX_WSTRC(L"while(");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L" < ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L".length)\n{\n");
  if (m_wsIdentifier.GetAt(0) == L'!') {
    CFX_WideString tempIdentifier =
        EXCLAMATION_IN_IDENTIFIER + m_wsIdentifier.Mid(1);
    javascript << tempIdentifier;
  } else {
    javascript << m_wsIdentifier;
  }
  javascript << FX_WSTRC(L" = ");
  javascript << RUNTIMEBLOCKTEMPARRAY;
  javascript << FX_WSTRC(L"[");
  javascript << RUNTIMEBLOCKTEMPARRAYINDEX;
  javascript << FX_WSTRC(L"++];\n");
  m_pList->ToImpliedReturnJS(javascript);
  javascript << FX_WSTRC(L"}\n");
  javascript << FX_WSTRC(L"}\n");
}
