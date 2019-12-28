// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_simpleexpression.h"

#include "core/fxcrt/include/fx_ext.h"

namespace {

const FX_WCHAR* const gs_lpStrExpFuncName[] = {
    L"foxit_xfa_formcalc_runtime.assign_value_operator",
    L"foxit_xfa_formcalc_runtime.logical_or_operator",
    L"foxit_xfa_formcalc_runtime.logical_and_operator",
    L"foxit_xfa_formcalc_runtime.equality_operator",
    L"foxit_xfa_formcalc_runtime.notequality_operator",
    L"foxit_xfa_formcalc_runtime.less_operator",
    L"foxit_xfa_formcalc_runtime.lessequal_operator",
    L"foxit_xfa_formcalc_runtime.greater_operator",
    L"foxit_xfa_formcalc_runtime.greaterequal_operator",
    L"foxit_xfa_formcalc_runtime.plus_operator",
    L"foxit_xfa_formcalc_runtime.minus_operator",
    L"foxit_xfa_formcalc_runtime.multiple_operator",
    L"foxit_xfa_formcalc_runtime.divide_operator",
    L"foxit_xfa_formcalc_runtime.positive_operator",
    L"foxit_xfa_formcalc_runtime.negative_operator",
    L"foxit_xfa_formcalc_runtime.logical_not_operator",
    L"foxit_xfa_formcalc_runtime.",
    L"foxit_xfa_formcalc_runtime.dot_accessor",
    L"foxit_xfa_formcalc_runtime.dotdot_accessor",
    L"foxit_xfa_formcalc_runtime.concat_fm_object",
    L"foxit_xfa_formcalc_runtime.is_fm_object",
    L"foxit_xfa_formcalc_runtime.is_fm_array",
    L"foxit_xfa_formcalc_runtime.get_fm_value",
    L"foxit_xfa_formcalc_runtime.get_fm_jsobj",
    L"foxit_xfa_formcalc_runtime.fm_var_filter",
};

struct XFA_FMBuildInFunc {
  uint32_t m_uHash;
  const FX_WCHAR* m_buildinfunc;
};

const XFA_FMBuildInFunc g_BuildInFuncs[] = {
    {0x0001f1f5, L"At"},           {0x00020b9c, L"FV"},
    {0x00021aef, L"If"},           {0x00023ee6, L"PV"},
    {0x04b5c9ee, L"Encode"},       {0x08e96685, L"DateFmt"},
    {0x09f99db6, L"Abs"},          {0x09f9e583, L"Apr"},
    {0x09fa043e, L"Avg"},          {0x0a9782a0, L"Get"},
    {0x0b1b09df, L"Len"},          {0x0b3543a6, L"Max"},
    {0x0b356ca4, L"Min"},          {0x0b358b60, L"Mod"},
    {0x0b4fded4, L"NPV"},          {0x0b846bf1, L"Pmt"},
    {0x0b8494f9, L"Put"},          {0x0bb8df5d, L"Ref"},
    {0x0bd37a99, L"Str"},          {0x0bd37fb5, L"Sum"},
    {0x1048469b, L"Cterm"},        {0x11e03660, L"Exists"},
    {0x126236e6, L"Post"},         {0x127c6661, L"PPmt"},
    {0x193ade3e, L"Right"},        {0x1ec8ab2c, L"Rate"},
    {0x20e476dc, L"IsoTime2Num"},  {0x23eb6816, L"TimeFmt"},
    {0x24fb17b0, L"LocalDateFmt"}, {0x28dee6e9, L"Format"},
    {0x2d0890b8, L"Term"},         {0x2d71b00f, L"Time"},
    {0x2f890fb1, L"Num2Time"},     {0x3767511d, L"Ceil"},
    {0x3ffd1941, L"LocalTimeFmt"}, {0x442f68c8, L"Round"},
    {0x46fd1128, L"Eval"},         {0x4d629440, L"Date2Num"},
    {0x4dcf25f8, L"Concat"},       {0x4e00255d, L"UnitValue"},
    {0x55a5cc29, L"Lower"},        {0x5e43e04c, L"WordNum"},
    {0x620ce6ba, L"Ipmt"},         {0x6f544d49, L"Count"},
    {0x7e241013, L"Within"},       {0x9b9a6e2b, L"IsoDate2Num"},
    {0xb2c941c2, L"UnitType"},     {0xb598a1f7, L"Uuid"},
    {0xbde9abde, L"Date"},         {0xc0010b80, L"Num2Date"},
    {0xc1f6144c, L"Upper"},        {0xc44028f7, L"Oneof"},
    {0xc62c1b2c, L"Space"},        {0xd0ff50f9, L"HasValue"},
    {0xd1537042, L"Floor"},        {0xd2ac9cf1, L"Time2Num"},
    {0xd907aee5, L"Num2GMTime"},   {0xdf24f7c4, L"Decode"},
    {0xe2664803, L"Substr"},       {0xe3e7b528, L"Stuff"},
    {0xe6792d4e, L"Rtrim"},        {0xe8c23f5b, L"Parse"},
    {0xea18d121, L"Choose"},       {0xebfef69c, L"Replace"},
    {0xf5ad782b, L"Left"},         {0xf7bb2248, L"Ltrim"},
};

struct XFA_FMSOMMethod {
  uint32_t m_uHash;
  const FX_WCHAR* m_wsSomMethodName;
  uint32_t m_dParameters;
};
const XFA_FMSOMMethod gs_FMSomMethods[] = {
    {0x00000068, L"h", 0x01},
    {0x00000077, L"w", 0x01},
    {0x00000078, L"x", 0x01},
    {0x00000079, L"y", 0x01},
    {0x05eb5b0f, L"pageSpan", 0x01},
    {0x10f1b1bd, L"page", 0x01},
    {0x3bf1c2a5, L"absPageSpan", 0x01},
    {0x3c752495, L"verify", 0x0d},
    {0x44c352ad, L"formNodes", 0x01},
    {0x5775c2cc, L"absPageInBatch", 0x01},
    {0x5ee00996, L"setElement", 0x01},
    {0x7033bfd5, L"insert", 0x03},
    {0x8c5feb32, L"sheetInBatch", 0x01},
    {0x8f3a8379, L"sheet", 0x01},
    {0x92dada4f, L"saveFilteredXML", 0x01},
    {0x9cab7cae, L"remove", 0x01},
    {0xa68635f1, L"sign", 0x61},
    {0xaac241c8, L"isRecordGroup", 0x01},
    {0xd8ed1467, L"clear", 0x01},
    {0xda12e518, L"append", 0x01},
    {0xe74f0653, L"absPage", 0x01},
};

}  // namespace

CFX_WideStringC XFA_FM_EXPTypeToString(
    XFA_FM_SimpleExpressionType simpleExpType) {
  return gs_lpStrExpFuncName[simpleExpType];
}

CXFA_FMSimpleExpression::CXFA_FMSimpleExpression(uint32_t line, XFA_FM_TOKEN op)
    : m_line(line), m_op(op) {}

void CXFA_FMSimpleExpression::ToJavaScript(CFX_WideTextBuf& javascript) {}

void CXFA_FMSimpleExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {}

XFA_FM_TOKEN CXFA_FMSimpleExpression::GetOperatorToken() const {
  return m_op;
}

CXFA_FMNullExpression::CXFA_FMNullExpression(uint32_t line)
    : CXFA_FMSimpleExpression(line, TOKnull) {}

void CXFA_FMNullExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"null");
}

CXFA_FMNumberExpression::CXFA_FMNumberExpression(uint32_t line,
                                                 CFX_WideStringC wsNumber)
    : CXFA_FMSimpleExpression(line, TOKnumber), m_wsNumber(wsNumber) {}

CXFA_FMNumberExpression::~CXFA_FMNumberExpression() {}

void CXFA_FMNumberExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << m_wsNumber;
}

CXFA_FMStringExpression::CXFA_FMStringExpression(uint32_t line,
                                                 CFX_WideStringC wsString)
    : CXFA_FMSimpleExpression(line, TOKstring), m_wsString(wsString) {}

CXFA_FMStringExpression::~CXFA_FMStringExpression() {}

void CXFA_FMStringExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  CFX_WideString tempStr(m_wsString);
  if (tempStr.GetLength() > 2) {
    javascript.AppendChar(L'\"');
    FX_WCHAR oneChar;
    for (int16_t i = 1; i < tempStr.GetLength() - 1; i++) {
      oneChar = tempStr[i];
      switch (oneChar) {
        case L'\"': {
          i++;
          javascript << FX_WSTRC(L"\\\"");
        } break;
        case 0x0d:
          break;
        case 0x0a: {
          javascript << FX_WSTRC(L"\\n");
        } break;
        default: { javascript.AppendChar(oneChar); } break;
      }
    }
    javascript.AppendChar(L'\"');
  } else {
    javascript << tempStr;
  }
}

CXFA_FMIdentifierExpressionn::CXFA_FMIdentifierExpressionn(
    uint32_t line,
    CFX_WideStringC wsIdentifier)
    : CXFA_FMSimpleExpression(line, TOKidentifier),
      m_wsIdentifier(wsIdentifier) {}

CXFA_FMIdentifierExpressionn::~CXFA_FMIdentifierExpressionn() {}

void CXFA_FMIdentifierExpressionn::ToJavaScript(CFX_WideTextBuf& javascript) {
  CFX_WideString tempStr(m_wsIdentifier);
  if (tempStr == FX_WSTRC(L"$")) {
    tempStr = FX_WSTRC(L"this");
  } else if (tempStr == FX_WSTRC(L"!")) {
    tempStr = FX_WSTRC(L"xfa.datasets");
  } else if (tempStr == FX_WSTRC(L"$data")) {
    tempStr = FX_WSTRC(L"xfa.datasets.data");
  } else if (tempStr == FX_WSTRC(L"$event")) {
    tempStr = FX_WSTRC(L"xfa.event");
  } else if (tempStr == FX_WSTRC(L"$form")) {
    tempStr = FX_WSTRC(L"xfa.form");
  } else if (tempStr == FX_WSTRC(L"$host")) {
    tempStr = FX_WSTRC(L"xfa.host");
  } else if (tempStr == FX_WSTRC(L"$layout")) {
    tempStr = FX_WSTRC(L"xfa.layout");
  } else if (tempStr == FX_WSTRC(L"$template")) {
    tempStr = FX_WSTRC(L"xfa.template");
  } else if (tempStr[0] == L'!') {
    tempStr = EXCLAMATION_IN_IDENTIFIER + tempStr.Mid(1);
  }
  javascript << tempStr;
}

CXFA_FMUnaryExpression::CXFA_FMUnaryExpression(uint32_t line,
                                               XFA_FM_TOKEN op,
                                               CXFA_FMSimpleExpression* pExp)
    : CXFA_FMSimpleExpression(line, op), m_pExp(pExp) {}

CXFA_FMUnaryExpression::~CXFA_FMUnaryExpression() {}

void CXFA_FMUnaryExpression::ToJavaScript(CFX_WideTextBuf& javascript) {}

CXFA_FMBinExpression::CXFA_FMBinExpression(uint32_t line,
                                           XFA_FM_TOKEN op,
                                           CXFA_FMSimpleExpression* pExp1,
                                           CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMSimpleExpression(line, op), m_pExp1(pExp1), m_pExp2(pExp2) {}

CXFA_FMBinExpression::~CXFA_FMBinExpression() {}

void CXFA_FMBinExpression::ToJavaScript(CFX_WideTextBuf& javascript) {}

CXFA_FMAssignExpression::CXFA_FMAssignExpression(uint32_t line,
                                                 XFA_FM_TOKEN op,
                                                 CXFA_FMSimpleExpression* pExp1,
                                                 CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMAssignExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"if (");
  javascript << gs_lpStrExpFuncName[ISFMOBJECT];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"))\n{\n");
  javascript << gs_lpStrExpFuncName[ASSIGN];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L");\n}\n");
  CFX_WideTextBuf tempExp1;
  m_pExp1->ToJavaScript(tempExp1);
  if (m_pExp1->GetOperatorToken() == TOKidentifier &&
      tempExp1.AsStringC() != FX_WSTRC(L"this")) {
    javascript << FX_WSTRC(L"else\n{\n");
    javascript << tempExp1;
    javascript << FX_WSTRC(L" = ");
    javascript << gs_lpStrExpFuncName[ASSIGN];
    javascript << FX_WSTRC(L"(");
    m_pExp1->ToJavaScript(javascript);
    javascript << FX_WSTRC(L", ");
    m_pExp2->ToJavaScript(javascript);
    javascript << FX_WSTRC(L");\n}\n");
  }
}

void CXFA_FMAssignExpression::ToImpliedReturnJS(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"if (");
  javascript << gs_lpStrExpFuncName[ISFMOBJECT];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L"))\n{\n");
  javascript << RUNTIMEFUNCTIONRETURNVALUE;
  javascript << FX_WSTRC(L" = ");
  javascript << gs_lpStrExpFuncName[ASSIGN];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L");\n}\n");
  CFX_WideTextBuf tempExp1;
  m_pExp1->ToJavaScript(tempExp1);
  if (m_pExp1->GetOperatorToken() == TOKidentifier &&
      tempExp1.AsStringC() != FX_WSTRC(L"this")) {
    javascript << FX_WSTRC(L"else\n{\n");
    javascript << RUNTIMEFUNCTIONRETURNVALUE;
    javascript << FX_WSTRC(L" = ");
    javascript << tempExp1;
    javascript << FX_WSTRC(L" = ");
    javascript << gs_lpStrExpFuncName[ASSIGN];
    javascript << FX_WSTRC(L"(");
    m_pExp1->ToJavaScript(javascript);
    javascript << FX_WSTRC(L", ");
    m_pExp2->ToJavaScript(javascript);
    javascript << FX_WSTRC(L");\n}\n");
  }
}

CXFA_FMLogicalOrExpression::CXFA_FMLogicalOrExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMLogicalOrExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[LOGICALOR];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMLogicalAndExpression::CXFA_FMLogicalAndExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMLogicalAndExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[LOGICALAND];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMEqualityExpression::CXFA_FMEqualityExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMEqualityExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  switch (m_op) {
    case TOKeq:
    case TOKkseq:
      javascript << gs_lpStrExpFuncName[EQUALITY];
      break;
    case TOKne:
    case TOKksne:
      javascript << gs_lpStrExpFuncName[NOTEQUALITY];
      break;
    default:
      ASSERT(FALSE);
      break;
  }
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMRelationalExpression::CXFA_FMRelationalExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMRelationalExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  switch (m_op) {
    case TOKlt:
    case TOKkslt:
      javascript << gs_lpStrExpFuncName[LESS];
      break;
    case TOKgt:
    case TOKksgt:
      javascript << gs_lpStrExpFuncName[GREATER];
      break;
    case TOKle:
    case TOKksle:
      javascript << gs_lpStrExpFuncName[LESSEQUAL];
      break;
    case TOKge:
    case TOKksge:
      javascript << gs_lpStrExpFuncName[GREATEREQUAL];
      break;
    default:
      ASSERT(FALSE);
      break;
  }
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMAdditiveExpression::CXFA_FMAdditiveExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMAdditiveExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  switch (m_op) {
    case TOKplus:
      javascript << gs_lpStrExpFuncName[PLUS];
      break;
    case TOKminus:
      javascript << gs_lpStrExpFuncName[MINUS];
      break;
    default:
      ASSERT(FALSE);
      break;
  }
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMMultiplicativeExpression::CXFA_FMMultiplicativeExpression(
    uint32_t line,
    XFA_FM_TOKEN op,
    CXFA_FMSimpleExpression* pExp1,
    CXFA_FMSimpleExpression* pExp2)
    : CXFA_FMBinExpression(line, op, pExp1, pExp2) {}

void CXFA_FMMultiplicativeExpression::ToJavaScript(
    CFX_WideTextBuf& javascript) {
  switch (m_op) {
    case TOKmul:
      javascript << gs_lpStrExpFuncName[MULTIPLE];
      break;
    case TOKdiv:
      javascript << gs_lpStrExpFuncName[DIVIDE];
      break;
    default:
      ASSERT(FALSE);
      break;
  }
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMPosExpression::CXFA_FMPosExpression(uint32_t line,
                                           CXFA_FMSimpleExpression* pExp)
    : CXFA_FMUnaryExpression(line, TOKplus, pExp) {}

void CXFA_FMPosExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[POSITIVE];
  javascript << FX_WSTRC(L"(");
  m_pExp->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMNegExpression::CXFA_FMNegExpression(uint32_t line,
                                           CXFA_FMSimpleExpression* pExp)
    : CXFA_FMUnaryExpression(line, TOKminus, pExp) {}

void CXFA_FMNegExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[NEGATIVE];
  javascript << FX_WSTRC(L"(");
  m_pExp->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMNotExpression::CXFA_FMNotExpression(uint32_t line,
                                           CXFA_FMSimpleExpression* pExp)
    : CXFA_FMUnaryExpression(line, TOKksnot, pExp) {}

void CXFA_FMNotExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[NOT];
  javascript << FX_WSTRC(L"(");
  m_pExp->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMCallExpression::CXFA_FMCallExpression(
    uint32_t line,
    CXFA_FMSimpleExpression* pExp,
    CFX_ArrayTemplate<CXFA_FMSimpleExpression*>* pArguments,
    FX_BOOL bIsSomMethod)
    : CXFA_FMUnaryExpression(line, TOKcall, pExp),
      m_bIsSomMethod(bIsSomMethod),
      m_pArguments(pArguments) {}

CXFA_FMCallExpression::~CXFA_FMCallExpression() {
  if (m_pArguments) {
    for (int i = 0; i < m_pArguments->GetSize(); ++i)
      delete m_pArguments->GetAt(i);

    delete m_pArguments;
  }
}

bool CXFA_FMCallExpression::IsBuildInFunc(CFX_WideTextBuf* funcName) {
  uint32_t uHash = FX_HashCode_GetW(funcName->AsStringC(), true);
  const XFA_FMBuildInFunc* pEnd = g_BuildInFuncs + FX_ArraySize(g_BuildInFuncs);
  const XFA_FMBuildInFunc* pFunc =
      std::lower_bound(g_BuildInFuncs, pEnd, uHash,
                       [](const XFA_FMBuildInFunc& func, uint32_t hash) {
                         return func.m_uHash < hash;
                       });
  if (pFunc < pEnd && uHash == pFunc->m_uHash) {
    funcName->Clear();
    *funcName << pFunc->m_buildinfunc;
    return true;
  }
  return false;
}

uint32_t CXFA_FMCallExpression::IsMethodWithObjParam(
    const CFX_WideStringC& methodName) {
  uint32_t uHash = FX_HashCode_GetW(methodName, false);
  XFA_FMSOMMethod somMethodWithObjPara;
  uint32_t parameters = 0x00;
  int32_t iStart = 0,
          iEnd = (sizeof(gs_FMSomMethods) / sizeof(gs_FMSomMethods[0])) - 1;
  int32_t iMid = (iStart + iEnd) / 2;
  do {
    iMid = (iStart + iEnd) / 2;
    somMethodWithObjPara = gs_FMSomMethods[iMid];
    if (uHash == somMethodWithObjPara.m_uHash) {
      parameters = somMethodWithObjPara.m_dParameters;
      break;
    } else if (uHash < somMethodWithObjPara.m_uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return parameters;
}

void CXFA_FMCallExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  CFX_WideTextBuf funcName;
  m_pExp->ToJavaScript(funcName);
  if (m_bIsSomMethod) {
    javascript << funcName;
    javascript << FX_WSTRC(L"(");
    if (m_pArguments) {
      uint32_t methodPara = IsMethodWithObjParam(funcName.AsStringC());
      if (methodPara > 0) {
        for (int i = 0; i < m_pArguments->GetSize(); ++i) {
          if ((methodPara & (0x01 << i)) > 0) {
            javascript << gs_lpStrExpFuncName[GETFMJSOBJ];
          } else {
            javascript << gs_lpStrExpFuncName[GETFMVALUE];
          }
          javascript << FX_WSTRC(L"(");
          CXFA_FMSimpleExpression* e = m_pArguments->GetAt(i);
          e->ToJavaScript(javascript);
          javascript << FX_WSTRC(L")");
          if (i + 1 < m_pArguments->GetSize()) {
            javascript << FX_WSTRC(L", ");
          }
        }
      } else {
        for (int i = 0; i < m_pArguments->GetSize(); ++i) {
          javascript << gs_lpStrExpFuncName[GETFMVALUE];
          javascript << FX_WSTRC(L"(");
          CXFA_FMSimpleExpression* e = m_pArguments->GetAt(i);
          e->ToJavaScript(javascript);
          javascript << FX_WSTRC(L")");
          if (i + 1 < m_pArguments->GetSize()) {
            javascript << FX_WSTRC(L", ");
          }
        }
      }
    }
    javascript << FX_WSTRC(L")");
  } else {
    bool isEvalFunc = false;
    bool isExistsFunc = false;
    if (IsBuildInFunc(&funcName)) {
      if (funcName.AsStringC() == FX_WSTRC(L"Eval")) {
        isEvalFunc = true;
        javascript << FX_WSTRC(L"eval.call(this, ");
        javascript << gs_lpStrExpFuncName[CALL];
        javascript << FX_WSTRC(L"Translate");
      } else if (funcName.AsStringC() == FX_WSTRC(L"Exists")) {
        isExistsFunc = true;
        javascript << gs_lpStrExpFuncName[CALL];
        javascript << funcName;
      } else {
        javascript << gs_lpStrExpFuncName[CALL];
        javascript << funcName;
      }
    } else {
      javascript << funcName;
    }
    javascript << FX_WSTRC(L"(");
    if (isExistsFunc) {
      javascript << FX_WSTRC(L"\n(\nfunction ()\n{\ntry\n{\n");
      if (m_pArguments && m_pArguments->GetSize() > 0) {
        CXFA_FMSimpleExpression* e = m_pArguments->GetAt(0);
        javascript << FX_WSTRC(L"return ");
        e->ToJavaScript(javascript);
        javascript << FX_WSTRC(L";\n}\n");
      } else {
        javascript << FX_WSTRC(L"return 0;\n}\n");
      }
      javascript << FX_WSTRC(
          L"catch(accessExceptions)\n{\nreturn 0;\n}\n}\n).call(this)\n");
    } else if (m_pArguments) {
      for (int i = 0; i < m_pArguments->GetSize(); ++i) {
        CXFA_FMSimpleExpression* e = m_pArguments->GetAt(i);
        e->ToJavaScript(javascript);
        if (i + 1 < m_pArguments->GetSize()) {
          javascript << FX_WSTRC(L", ");
        }
      }
    }
    javascript << FX_WSTRC(L")");
    if (isEvalFunc) {
      javascript << FX_WSTRC(L")");
    }
  }
}

CXFA_FMDotAccessorExpression::CXFA_FMDotAccessorExpression(
    uint32_t line,
    CXFA_FMSimpleExpression* pAccessor,
    XFA_FM_TOKEN op,
    CFX_WideStringC wsIdentifier,
    CXFA_FMSimpleExpression* pIndexExp)
    : CXFA_FMBinExpression(line, op, pAccessor, pIndexExp),
      m_wsIdentifier(wsIdentifier) {}

CXFA_FMDotAccessorExpression::~CXFA_FMDotAccessorExpression() {}

void CXFA_FMDotAccessorExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[DOT];
  javascript << FX_WSTRC(L"(");
  if (m_pExp1) {
    m_pExp1->ToJavaScript(javascript);
  } else {
    javascript << FX_WSTRC(L"null");
  }
  javascript << FX_WSTRC(L", ");
  javascript << FX_WSTRC(L"\"");
  if (m_pExp1 && m_pExp1->GetOperatorToken() == TOKidentifier) {
    m_pExp1->ToJavaScript(javascript);
  }
  javascript << FX_WSTRC(L"\", ");
  if (m_op == TOKdotscream) {
    javascript << FX_WSTRC(L"\"#");
    javascript << m_wsIdentifier;
    javascript << FX_WSTRC(L"\", ");
  } else if (m_op == TOKdotstar) {
    javascript << FX_WSTRC(L"\"*\", ");
  } else if (m_op == TOKcall) {
    javascript << FX_WSTRC(L"\"\", ");
  } else {
    javascript << FX_WSTRC(L"\"");
    javascript << m_wsIdentifier;
    javascript << FX_WSTRC(L"\", ");
  }
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMIndexExpression::CXFA_FMIndexExpression(
    uint32_t line,
    XFA_FM_AccessorIndex accessorIndex,
    CXFA_FMSimpleExpression* pIndexExp,
    FX_BOOL bIsStarIndex)
    : CXFA_FMUnaryExpression(line, TOKlbracket, pIndexExp),
      m_accessorIndex(accessorIndex),
      m_bIsStarIndex(bIsStarIndex) {}

void CXFA_FMIndexExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  switch (m_accessorIndex) {
    case ACCESSOR_NO_INDEX:
      javascript << FX_WSTRC(L"0");
      break;
    case ACCESSOR_NO_RELATIVEINDEX:
      javascript << FX_WSTRC(L"1");
      break;
    case ACCESSOR_POSITIVE_INDEX:
      javascript << FX_WSTRC(L"2");
      break;
    case ACCESSOR_NEGATIVE_INDEX:
      javascript << FX_WSTRC(L"3");
      break;
    default:
      javascript << FX_WSTRC(L"0");
  }
  if (!m_bIsStarIndex) {
    javascript << FX_WSTRC(L", ");
    if (m_pExp) {
      m_pExp->ToJavaScript(javascript);
    } else {
      javascript << FX_WSTRC(L"0");
    }
  }
}

CXFA_FMDotDotAccessorExpression::CXFA_FMDotDotAccessorExpression(
    uint32_t line,
    CXFA_FMSimpleExpression* pAccessor,
    XFA_FM_TOKEN op,
    CFX_WideStringC wsIdentifier,
    CXFA_FMSimpleExpression* pIndexExp)
    : CXFA_FMBinExpression(line, op, pAccessor, pIndexExp),
      m_wsIdentifier(wsIdentifier) {}

CXFA_FMDotDotAccessorExpression::~CXFA_FMDotDotAccessorExpression() {}

void CXFA_FMDotDotAccessorExpression::ToJavaScript(
    CFX_WideTextBuf& javascript) {
  javascript << gs_lpStrExpFuncName[DOTDOT];
  javascript << FX_WSTRC(L"(");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L", ");
  javascript << FX_WSTRC(L"\"");
  if (m_pExp1 && m_pExp1->GetOperatorToken() == TOKidentifier) {
    m_pExp1->ToJavaScript(javascript);
  }
  javascript << FX_WSTRC(L"\", ");
  javascript << FX_WSTRC(L"\"");
  javascript << m_wsIdentifier;
  javascript << FX_WSTRC(L"\", ");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L")");
}

CXFA_FMMethodCallExpression::CXFA_FMMethodCallExpression(
    uint32_t line,
    CXFA_FMSimpleExpression* pAccessorExp1,
    CXFA_FMSimpleExpression* pCallExp)
    : CXFA_FMBinExpression(line, TOKdot, pAccessorExp1, pCallExp) {}

void CXFA_FMMethodCallExpression::ToJavaScript(CFX_WideTextBuf& javascript) {
  javascript << FX_WSTRC(L"(\nfunction ()\n{\n");
  javascript << FX_WSTRC(L"var method_return_value = null;\n");
  javascript << FX_WSTRC(L"var accessor_object = ");
  m_pExp1->ToJavaScript(javascript);
  javascript << FX_WSTRC(L";\n");
  javascript << FX_WSTRC(L"if (");
  javascript << gs_lpStrExpFuncName[ISFMARRAY];
  javascript << FX_WSTRC(L"(accessor_object))\n{\n");
  javascript << FX_WSTRC(
      L"for(var index = accessor_object.length - 1; index > 1; index--)\n{\n");
  javascript << FX_WSTRC(L"method_return_value = accessor_object[index].");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L";\n}\n}\n");
  javascript << FX_WSTRC(L"else\n{\nmethod_return_value = accessor_object.");
  m_pExp2->ToJavaScript(javascript);
  javascript << FX_WSTRC(L";\n}\n");
  javascript << FX_WSTRC(L"return method_return_value;\n");
  javascript << FX_WSTRC(L"}\n).call(this)");
}
