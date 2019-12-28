// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_fmparse.h"

#include <memory>

CXFA_FMParse::CXFA_FMParse() : m_pToken(nullptr), m_pErrorInfo(0) {}

CXFA_FMParse::~CXFA_FMParse() {}

int32_t CXFA_FMParse::Init(const CFX_WideStringC& wsFormcalc,
                           CXFA_FMErrorInfo* pErrorInfo) {
  m_pErrorInfo = pErrorInfo;
  m_lexer.reset(new CXFA_FMLexer(wsFormcalc, m_pErrorInfo));
  return 0;
}

void CXFA_FMParse::NextToken() {
  m_pToken = m_lexer->NextToken();
  while (m_pToken->m_type == TOKreserver) {
    if (m_lexer->HasError()) {
      break;
    }
    m_pToken = m_lexer->NextToken();
  }
}

void CXFA_FMParse::Check(XFA_FM_TOKEN op) {
  if (m_pToken->m_type != op) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken, XFA_FM_KeywordToString(op),
          ws_TempString.c_str());
  }
  NextToken();
}

void CXFA_FMParse::Error(uint32_t lineNum, const FX_WCHAR* msg, ...) {
  m_pErrorInfo->linenum = lineNum;
  va_list ap;
  va_start(ap, msg);
  m_pErrorInfo->message.FormatV(msg, ap);
  va_end(ap);
}

CFX_ArrayTemplate<CXFA_FMExpression*>* CXFA_FMParse::ParseTopExpression() {
  std::unique_ptr<CXFA_FMExpression> e;
  CFX_ArrayTemplate<CXFA_FMExpression*>* expression =
      new CFX_ArrayTemplate<CXFA_FMExpression*>();
  while (1) {
    if (m_pToken->m_type == TOKeof || m_pToken->m_type == TOKendfunc ||
        m_pToken->m_type == TOKendif || m_pToken->m_type == TOKelseif ||
        m_pToken->m_type == TOKelse || m_pToken->m_type == TOKreserver) {
      return expression;
    }

    if (m_pToken->m_type == TOKfunc) {
      e.reset(ParseFunction());
      if (e) {
        expression->Add(e.release());
      } else {
        break;
      }
    } else {
      e.reset(ParseExpression());
      if (e) {
        expression->Add(e.release());
      } else {
        break;
      }
    }
  }
  return expression;
}

CXFA_FMExpression* CXFA_FMParse::ParseFunction() {
  std::unique_ptr<CXFA_FMExpression> e;
  CFX_WideStringC ident;
  std::unique_ptr<CFX_WideStringCArray> pArguments;
  std::unique_ptr<CFX_ArrayTemplate<CXFA_FMExpression*>> pExpressions;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  if (m_pToken->m_type != TOKidentifier) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
          ws_TempString.c_str());
  } else {
    ident = m_pToken->m_wstring;
    NextToken();
  }
  Check(TOKlparen);
  if (m_pToken->m_type == TOKrparen) {
    NextToken();
  } else {
    pArguments.reset(new CFX_WideStringCArray());
    CFX_WideStringC p;
    while (1) {
      if (m_pToken->m_type == TOKidentifier) {
        p = m_pToken->m_wstring;
        pArguments->Add(p);
        NextToken();
        if (m_pToken->m_type == TOKcomma) {
          NextToken();
          continue;
        } else if (m_pToken->m_type == TOKrparen) {
          NextToken();
          break;
        } else {
          Check(TOKrparen);
          break;
        }
      } else {
        CFX_WideString ws_TempString(m_pToken->m_wstring);
        Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
              ws_TempString.c_str());
        NextToken();
        break;
      }
    }
  }
  Check(TOKdo);
  if (m_pToken->m_type == TOKendfunc) {
    NextToken();
  } else {
    pExpressions.reset(ParseTopExpression());
    Check(TOKendfunc);
  }
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMFunctionDefinition(line, 0, ident, pArguments.release(),
                                          pExpressions.release()));
  } else {
    if (pArguments)
      pArguments->RemoveAll();
    if (pExpressions) {
      for (int i = 0; i < pExpressions->GetSize(); ++i)
        delete pExpressions->GetAt(i);
    }
  }
  return e.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseExpression() {
  std::unique_ptr<CXFA_FMExpression> e;
  uint32_t line = m_pToken->m_uLinenum;
  switch (m_pToken->m_type) {
    case TOKvar:
      e.reset(ParseVarExpression());
      break;
    case TOKnull:
    case TOKnumber:
    case TOKstring:
    case TOKplus:
    case TOKminus:
    case TOKksnot:
    case TOKidentifier:
    case TOKlparen:
      e.reset(ParseExpExpression());
      break;
    case TOKif:
      e.reset(ParseIfExpression());
      break;
    case TOKwhile:
      e.reset(ParseWhileExpression());
      break;
    case TOKfor:
      e.reset(ParseForExpression());
      break;
    case TOKforeach:
      e.reset(ParseForeachExpression());
      break;
    case TOKdo:
      e.reset(ParseDoExpression());
      break;
    case TOKbreak:
      e.reset(new CXFA_FMBreakExpression(line));
      NextToken();
      break;
    case TOKcontinue:
      e.reset(new CXFA_FMContinueExpression(line));
      NextToken();
      break;
    default:
      CFX_WideString ws_TempString(m_pToken->m_wstring);
      Error(m_pToken->m_uLinenum, kFMErrUnexpectedExpression,
            ws_TempString.c_str());
      NextToken();
      break;
  }
  return e.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseVarExpression() {
  std::unique_ptr<CXFA_FMExpression> e;
  CFX_WideStringC ident;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  if (m_pToken->m_type != TOKidentifier) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
          ws_TempString.c_str());
  } else {
    ident = m_pToken->m_wstring;
    NextToken();
  }
  if (m_pToken->m_type == TOKassign) {
    NextToken();
    e.reset(ParseExpExpression());
  }
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMVarExpression(line, ident, e.release()));
  } else {
    e.reset();
  }
  return e.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseSimpleExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> pExp1(ParseLogicalOrExpression());
  while (m_pToken->m_type == TOKassign) {
    NextToken();
    std::unique_ptr<CXFA_FMSimpleExpression> pExp2(ParseLogicalOrExpression());
    if (m_pErrorInfo->message.IsEmpty()) {
      pExp1.reset(new CXFA_FMAssignExpression(line, TOKassign, pExp1.release(),
                                              pExp2.release()));
    } else {
      pExp1.reset();
    }
  }
  return pExp1.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseExpExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> pExp1(ParseSimpleExpression());
  std::unique_ptr<CXFA_FMExpression> e;
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMExpExpression(line, pExp1.release()));
  } else {
    e.reset();
  }
  return e.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseLogicalOrExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseLogicalAndExpression());
  for (;;) {
    switch (m_pToken->m_type) {
      case TOKor:
      case TOKksor: {
        NextToken();
        std::unique_ptr<CXFA_FMSimpleExpression> e2(
            ParseLogicalAndExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMLogicalOrExpression(line, TOKor, e1.release(),
                                                  e2.release()));
        } else {
          e1.reset();
        }
        continue;
      }
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseLogicalAndExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseEqualityExpression());
  for (;;) {
    switch (m_pToken->m_type) {
      case TOKand:
      case TOKksand: {
        NextToken();
        std::unique_ptr<CXFA_FMSimpleExpression> e2(ParseEqualityExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMLogicalAndExpression(line, TOKand, e1.release(),
                                                   e2.release()));
        } else {
          e1.reset();
        }
        continue;
      }
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseEqualityExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseRelationalExpression());
  for (;;) {
    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_pToken->m_type) {
      case TOKeq:
      case TOKkseq:
        NextToken();
        e2.reset(ParseRelationalExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMEqualityExpression(line, TOKeq, e1.release(),
                                                 e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKne:
      case TOKksne:
        NextToken();
        e2.reset(ParseRelationalExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMEqualityExpression(line, TOKne, e1.release(),
                                                 e2.release()));
        } else {
          e1.reset();
        }
        continue;
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseRelationalExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseAddtiveExpression());
  for (;;) {
    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_pToken->m_type) {
      case TOKlt:
      case TOKkslt:
        NextToken();
        e2.reset(ParseAddtiveExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMRelationalExpression(line, TOKlt, e1.release(),
                                                   e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKgt:
      case TOKksgt:
        NextToken();
        e2.reset(ParseAddtiveExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMRelationalExpression(line, TOKgt, e1.release(),
                                                   e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKle:
      case TOKksle:
        NextToken();
        e2.reset(ParseAddtiveExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMRelationalExpression(line, TOKle, e1.release(),
                                                   e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKge:
      case TOKksge:
        NextToken();
        e2.reset(ParseAddtiveExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMRelationalExpression(line, TOKge, e1.release(),
                                                   e2.release()));
        } else {
          e1.reset();
        }
        continue;
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseAddtiveExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseMultiplicativeExpression());
  for (;;) {
    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_pToken->m_type) {
      case TOKplus:
        NextToken();
        e2.reset(ParseMultiplicativeExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMAdditiveExpression(line, TOKplus, e1.release(),
                                                 e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKminus:
        NextToken();
        e2.reset(ParseMultiplicativeExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMAdditiveExpression(line, TOKminus, e1.release(),
                                                 e2.release()));
        } else {
          e1.reset();
        }
        continue;
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseMultiplicativeExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> e1(ParseUnaryExpression());
  for (;;) {
    std::unique_ptr<CXFA_FMSimpleExpression> e2;
    switch (m_pToken->m_type) {
      case TOKmul:
        NextToken();
        e2.reset(ParseUnaryExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMMultiplicativeExpression(
              line, TOKmul, e1.release(), e2.release()));
        } else {
          e1.reset();
        }
        continue;
      case TOKdiv:
        NextToken();
        e2.reset(ParseUnaryExpression());
        if (m_pErrorInfo->message.IsEmpty()) {
          e1.reset(new CXFA_FMMultiplicativeExpression(
              line, TOKdiv, e1.release(), e2.release()));
        } else {
          e1.reset();
        }
        continue;
      default:
        break;
    }
    break;
  }
  return e1.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseUnaryExpression() {
  std::unique_ptr<CXFA_FMSimpleExpression> e;
  uint32_t line = m_pToken->m_uLinenum;
  switch (m_pToken->m_type) {
    case TOKplus:
      NextToken();
      e.reset(ParseUnaryExpression());
      if (m_pErrorInfo->message.IsEmpty()) {
        e.reset(new CXFA_FMPosExpression(line, e.release()));
      } else {
        e.reset();
      }
      break;
    case TOKminus:
      NextToken();
      e.reset(ParseUnaryExpression());
      if (m_pErrorInfo->message.IsEmpty()) {
        e.reset(new CXFA_FMNegExpression(line, e.release()));
      } else {
        e.reset();
      }
      break;
    case TOKksnot:
      NextToken();
      e.reset(ParseUnaryExpression());
      if (m_pErrorInfo->message.IsEmpty()) {
        e.reset(new CXFA_FMNotExpression(line, e.release()));
      } else {
        e.reset();
      }
      break;
    default:
      e.reset(ParsePrimaryExpression());
      break;
  }
  return e.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParsePrimaryExpression() {
  std::unique_ptr<CXFA_FMSimpleExpression> e;
  uint32_t line = m_pToken->m_uLinenum;
  switch (m_pToken->m_type) {
    case TOKnumber:
      e.reset(new CXFA_FMNumberExpression(line, m_pToken->m_wstring));
      NextToken();
      break;
    case TOKstring:
      e.reset(new CXFA_FMStringExpression(line, m_pToken->m_wstring));
      NextToken();
      break;
    case TOKidentifier: {
      CFX_WideStringC wsIdentifier(m_pToken->m_wstring);
      NextToken();
      if (m_pToken->m_type == TOKlbracket) {
        CXFA_FMSimpleExpression* s = ParseIndexExpression();
        if (s) {
          e.reset(new CXFA_FMDotAccessorExpression(line, nullptr, TOKdot,
                                                   wsIdentifier, s));
        }
        NextToken();
      } else {
        e.reset(new CXFA_FMIdentifierExpressionn(line, wsIdentifier));
      }
    } break;
    case TOKif:
      e.reset(new CXFA_FMIdentifierExpressionn(line, m_pToken->m_wstring));
      NextToken();
      break;
    case TOKnull:
      e.reset(new CXFA_FMNullExpression(line));
      NextToken();
      break;
    case TOKlparen:
      e.reset(ParseParenExpression());
      break;
    default:
      CFX_WideString ws_TempString(m_pToken->m_wstring);
      Error(m_pToken->m_uLinenum, kFMErrUnexpectedExpression,
            ws_TempString.c_str());
      NextToken();
      break;
  }
  e.reset(ParsePostExpression(e.release()));
  if (!(m_pErrorInfo->message.IsEmpty()))
    e.reset();
  return e.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParsePostExpression(
    CXFA_FMSimpleExpression* e) {
  uint32_t line = m_pToken->m_uLinenum;
  while (1) {
    switch (m_pToken->m_type) {
      case TOKlparen: {
        NextToken();
        std::unique_ptr<CFX_ArrayTemplate<CXFA_FMSimpleExpression*>> pArray;
        if (m_pToken->m_type != TOKrparen) {
          pArray.reset(new CFX_ArrayTemplate<CXFA_FMSimpleExpression*>());
          while (m_pToken->m_type != TOKrparen) {
            if (CXFA_FMSimpleExpression* expr = ParseSimpleExpression())
              pArray->Add(expr);
            if (m_pToken->m_type == TOKcomma) {
              NextToken();
            } else if (m_pToken->m_type == TOKeof ||
                       m_pToken->m_type == TOKreserver) {
              break;
            }
          }
          if (m_pToken->m_type != TOKrparen) {
            CFX_WideString ws_TempString(m_pToken->m_wstring);
            Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
                  XFA_FM_KeywordToString(TOKrparen), ws_TempString.c_str());
          }
        }
        if (m_pErrorInfo->message.IsEmpty()) {
          e = new CXFA_FMCallExpression(line, e, pArray.release(), FALSE);
          NextToken();
          if (m_pToken->m_type != TOKlbracket) {
            continue;
          }
          CXFA_FMSimpleExpression* s = ParseIndexExpression();
          if (s) {
            e = new CXFA_FMDotAccessorExpression(line, e, TOKcall,
                                                 FX_WSTRC(L""), s);
          } else {
            delete e;
            e = nullptr;
          }
        } else {
          if (pArray) {
            for (int32_t i = 0; i < pArray->GetSize(); ++i)
              delete pArray->GetAt(i);
          }
          delete e;
          e = nullptr;
        }
      } break;
      case TOKdot:
        NextToken();
        if (m_pToken->m_type == TOKidentifier) {
          CFX_WideStringC tempStr = m_pToken->m_wstring;
          uint32_t tempLine = m_pToken->m_uLinenum;
          NextToken();
          if (m_pToken->m_type == TOKlparen) {
            CXFA_FMSimpleExpression* pExpAccessor;
            CXFA_FMSimpleExpression* pExpCall;
            pExpAccessor = e;
            NextToken();
            std::unique_ptr<CFX_ArrayTemplate<CXFA_FMSimpleExpression*>> pArray;
            if (m_pToken->m_type != TOKrparen) {
              pArray.reset(new CFX_ArrayTemplate<CXFA_FMSimpleExpression*>());
              while (m_pToken->m_type != TOKrparen) {
                CXFA_FMSimpleExpression* exp = ParseSimpleExpression();
                pArray->Add(exp);
                if (m_pToken->m_type == TOKcomma) {
                  NextToken();
                } else if (m_pToken->m_type == TOKeof ||
                           m_pToken->m_type == TOKreserver) {
                  break;
                }
              }
              if (m_pToken->m_type != TOKrparen) {
                CFX_WideString ws_TempString(m_pToken->m_wstring);
                Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
                      XFA_FM_KeywordToString(TOKrparen), ws_TempString.c_str());
              }
            }
            if (m_pErrorInfo->message.IsEmpty()) {
              CXFA_FMSimpleExpression* pIdentifier =
                  new CXFA_FMIdentifierExpressionn(tempLine, tempStr);
              pExpCall = new CXFA_FMCallExpression(line, pIdentifier,
                                                   pArray.release(), TRUE);
              e = new CXFA_FMMethodCallExpression(line, pExpAccessor, pExpCall);
              NextToken();
              if (m_pToken->m_type != TOKlbracket) {
                continue;
              }
              CXFA_FMSimpleExpression* s = ParseIndexExpression();
              if (s) {
                e = new CXFA_FMDotAccessorExpression(line, e, TOKcall,
                                                     FX_WSTRC(L""), s);
              } else {
                delete e;
                e = nullptr;
              }
            } else {
              if (pArray) {
                for (int32_t i = 0; i < pArray->GetSize(); ++i)
                  delete pArray->GetAt(i);
              }
              delete e;
              e = nullptr;
            }
          } else if (m_pToken->m_type == TOKlbracket) {
            std::unique_ptr<CXFA_FMSimpleExpression> s(ParseIndexExpression());
            if (!(m_pErrorInfo->message.IsEmpty())) {
              delete e;
              return nullptr;
            }
            e = new CXFA_FMDotAccessorExpression(tempLine, e, TOKdot, tempStr,
                                                 s.release());
          } else {
            CXFA_FMSimpleExpression* s = new CXFA_FMIndexExpression(
                tempLine, ACCESSOR_NO_INDEX, nullptr, FALSE);
            e = new CXFA_FMDotAccessorExpression(line, e, TOKdot, tempStr, s);
            continue;
          }
        } else {
          CFX_WideString ws_TempString(m_pToken->m_wstring);
          Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
                ws_TempString.c_str());
          return e;
        }
        break;
      case TOKdotdot:
        NextToken();
        if (m_pToken->m_type == TOKidentifier) {
          CFX_WideStringC tempStr = m_pToken->m_wstring;
          uint32_t tempLine = m_pToken->m_uLinenum;
          NextToken();
          if (m_pToken->m_type == TOKlbracket) {
            std::unique_ptr<CXFA_FMSimpleExpression> s(ParseIndexExpression());
            if (!(m_pErrorInfo->message.IsEmpty())) {
              delete e;
              return nullptr;
            }
            e = new CXFA_FMDotDotAccessorExpression(tempLine, e, TOKdotdot,
                                                    tempStr, s.release());
          } else {
            CXFA_FMSimpleExpression* s = new CXFA_FMIndexExpression(
                tempLine, ACCESSOR_NO_INDEX, nullptr, FALSE);
            e = new CXFA_FMDotDotAccessorExpression(line, e, TOKdotdot, tempStr,
                                                    s);
            continue;
          }
        } else {
          CFX_WideString ws_TempString(m_pToken->m_wstring);
          Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
                ws_TempString.c_str());
          return e;
        }
        break;
      case TOKdotscream:
        NextToken();
        if (m_pToken->m_type == TOKidentifier) {
          CFX_WideStringC tempStr = m_pToken->m_wstring;
          uint32_t tempLine = m_pToken->m_uLinenum;
          NextToken();
          if (m_pToken->m_type == TOKlbracket) {
            std::unique_ptr<CXFA_FMSimpleExpression> s(ParseIndexExpression());
            if (!(m_pErrorInfo->message.IsEmpty())) {
              delete e;
              return nullptr;
            }
            e = new CXFA_FMDotAccessorExpression(tempLine, e, TOKdotscream,
                                                 tempStr, s.release());
          } else {
            CXFA_FMSimpleExpression* s = new CXFA_FMIndexExpression(
                tempLine, ACCESSOR_NO_INDEX, nullptr, FALSE);
            e = new CXFA_FMDotAccessorExpression(line, e, TOKdotscream, tempStr,
                                                 s);
            continue;
          }
        } else {
          CFX_WideString ws_TempString(m_pToken->m_wstring);
          Error(m_pToken->m_uLinenum, kFMErrExpectedIdentifier,
                ws_TempString.c_str());
          return e;
        }
        break;
      case TOKdotstar: {
        CXFA_FMSimpleExpression* s =
            new CXFA_FMIndexExpression(line, ACCESSOR_NO_INDEX, nullptr, FALSE);
        e = new CXFA_FMDotAccessorExpression(line, e, TOKdotstar,
                                             FX_WSTRC(L"*"), s);
      } break;
      default:
        return e;
    }
    NextToken();
  }
  return e;
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseIndexExpression() {
  std::unique_ptr<CXFA_FMSimpleExpression> pExp;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  std::unique_ptr<CXFA_FMSimpleExpression> s;
  XFA_FM_AccessorIndex accessorIndex = ACCESSOR_NO_RELATIVEINDEX;
  if (m_pToken->m_type == TOKmul) {
    pExp.reset(
        new CXFA_FMIndexExpression(line, accessorIndex, s.release(), TRUE));
    NextToken();
    if (m_pToken->m_type != TOKrbracket) {
      CFX_WideString ws_TempString(m_pToken->m_wstring);
      Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
            XFA_FM_KeywordToString(TOKrparen), ws_TempString.c_str());
      pExp.reset();
    }
    return pExp.release();
  }
  if (m_pToken->m_type == TOKplus) {
    accessorIndex = ACCESSOR_POSITIVE_INDEX;
    NextToken();
  } else if (m_pToken->m_type == TOKminus) {
    accessorIndex = ACCESSOR_NEGATIVE_INDEX;
    NextToken();
  }
  s.reset(ParseSimpleExpression());
  if (m_pToken->m_type != TOKrbracket) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
          XFA_FM_KeywordToString(TOKrparen), ws_TempString.c_str());
  } else {
    pExp.reset(
        new CXFA_FMIndexExpression(line, accessorIndex, s.release(), FALSE));
  }
  return pExp.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseParenExpression() {
  Check(TOKlparen);

  if (m_pToken->m_type == TOKrparen) {
    Error(m_pToken->m_uLinenum, kFMErrExpectedNonEmptyExpression);
    NextToken();
    return nullptr;
  }

  uint32_t line = m_pToken->m_uLinenum;
  std::unique_ptr<CXFA_FMSimpleExpression> pExp1(ParseLogicalOrExpression());

  while (m_pToken->m_type == TOKassign) {
    NextToken();
    std::unique_ptr<CXFA_FMSimpleExpression> pExp2(ParseLogicalOrExpression());
    if (m_pErrorInfo->message.IsEmpty()) {
      pExp1.reset(new CXFA_FMAssignExpression(line, TOKassign, pExp1.release(),
                                              pExp2.release()));
    } else {
      pExp1.reset();
    }
  }
  Check(TOKrparen);
  return pExp1.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseBlockExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  CXFA_FMExpression* e = nullptr;
  std::unique_ptr<CFX_ArrayTemplate<CXFA_FMExpression*>> expression(
      new CFX_ArrayTemplate<CXFA_FMExpression*>());

  while (1) {
    switch (m_pToken->m_type) {
      case TOKeof:
      case TOKendif:
      case TOKelseif:
      case TOKelse:
      case TOKendwhile:
      case TOKendfor:
      case TOKend:
      case TOKendfunc:
      case TOKreserver:
        break;
      case TOKfunc:
        e = ParseFunction();
        if (e) {
          expression->Add(e);
        }
        continue;
      default:
        e = ParseExpression();
        if (e) {
          expression->Add(e);
        }
        continue;
    }
    break;
  }
  std::unique_ptr<CXFA_FMBlockExpression> pExp;
  if (m_pErrorInfo->message.IsEmpty()) {
    pExp.reset(new CXFA_FMBlockExpression(line, expression.release()));
  } else {
    for (int i = 0; i < expression->GetSize(); ++i)
      delete static_cast<CXFA_FMExpression*>(expression->GetAt(i));
  }
  return pExp.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseIfExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  const FX_WCHAR* pStartPos = m_lexer->SavePos();
  NextToken();
  Check(TOKlparen);
  std::unique_ptr<CXFA_FMSimpleExpression> pExpression;
  while (m_pToken->m_type != TOKrparen) {
    pExpression.reset(ParseSimpleExpression());
    if (m_pToken->m_type != TOKcomma)
      break;
    NextToken();
  }
  Check(TOKrparen);
  if (m_pToken->m_type != TOKthen) {
    m_lexer->SetCurrentLine(line);
    m_pToken = new CXFA_FMToken(line);
    m_pToken->m_type = TOKidentifier;
    m_pToken->m_wstring = FX_WSTRC(L"if");
    m_lexer->SetToken(m_pToken);
    m_lexer->RestorePos(pStartPos);
    return ParseExpExpression();
  }
  Check(TOKthen);
  std::unique_ptr<CXFA_FMExpression> pIfExpression(ParseBlockExpression());
  std::unique_ptr<CXFA_FMExpression> pElseExpression;
  switch (m_pToken->m_type) {
    case TOKeof:
    case TOKendif:
      Check(TOKendif);
      break;
    case TOKif:
      pElseExpression.reset(ParseIfExpression());
      Check(TOKendif);
      break;
    case TOKelseif:
      pElseExpression.reset(ParseIfExpression());
      break;
    case TOKelse:
      NextToken();
      pElseExpression.reset(ParseBlockExpression());
      Check(TOKendif);
      break;
    default:
      CFX_WideString ws_TempString(m_pToken->m_wstring);
      Error(m_pToken->m_uLinenum, kFMErrExpectedEndIf, ws_TempString.c_str());
      NextToken();
      break;
  }
  std::unique_ptr<CXFA_FMIfExpression> pExp;
  if (m_pErrorInfo->message.IsEmpty()) {
    pExp.reset(new CXFA_FMIfExpression(line, pExpression.release(),
                                       pIfExpression.release(),
                                       pElseExpression.release()));
  }
  return pExp.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseWhileExpression() {
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  std::unique_ptr<CXFA_FMSimpleExpression> pCondition(ParseParenExpression());
  Check(TOKdo);
  std::unique_ptr<CXFA_FMExpression> pExpression(ParseBlockExpression());
  Check(TOKendwhile);
  std::unique_ptr<CXFA_FMExpression> e;
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMWhileExpression(line, pCondition.release(),
                                       pExpression.release()));
  }
  return e.release();
}

CXFA_FMSimpleExpression* CXFA_FMParse::ParseSubassignmentInForExpression() {
  std::unique_ptr<CXFA_FMSimpleExpression> e;
  switch (m_pToken->m_type) {
    case TOKidentifier:
      e.reset(ParseSimpleExpression());
      break;
    default:
      CFX_WideString ws_TempString(m_pToken->m_wstring);
      Error(m_pToken->m_uLinenum, kFMErrUnexpectedExpression,
            ws_TempString.c_str());
      NextToken();
      break;
  }
  return e.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseForExpression() {
  CFX_WideStringC wsVariant;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  if (m_pToken->m_type != TOKidentifier) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
          XFA_FM_KeywordToString(m_pToken->m_type), ws_TempString.c_str());
  }
  wsVariant = m_pToken->m_wstring;
  NextToken();
  std::unique_ptr<CXFA_FMSimpleExpression> pAssignment;
  if (m_pToken->m_type == TOKassign) {
    NextToken();
    pAssignment.reset(ParseSimpleExpression());
  } else {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
          XFA_FM_KeywordToString(m_pToken->m_type), ws_TempString.c_str());
  }
  int32_t iDirection = 0;
  if (m_pToken->m_type == TOKupto) {
    iDirection = 1;
  } else if (m_pToken->m_type == TOKdownto) {
    iDirection = -1;
  } else {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken, L"upto or downto",
          ws_TempString.c_str());
  }
  NextToken();
  std::unique_ptr<CXFA_FMSimpleExpression> pAccessor(ParseSimpleExpression());
  std::unique_ptr<CXFA_FMSimpleExpression> pStep;
  if (m_pToken->m_type == TOKstep) {
    NextToken();
    pStep.reset(ParseSimpleExpression());
  }
  Check(TOKdo);
  std::unique_ptr<CXFA_FMExpression> pList(ParseBlockExpression());
  Check(TOKendfor);
  std::unique_ptr<CXFA_FMExpression> e;
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMForExpression(line, wsVariant, pAssignment.release(),
                                     pAccessor.release(), iDirection,
                                     pStep.release(), pList.release()));
  }
  return e.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseForeachExpression() {
  std::unique_ptr<CXFA_FMExpression> e;
  CFX_WideStringC wsIdentifier;
  std::unique_ptr<CFX_ArrayTemplate<CXFA_FMSimpleExpression*>> pAccessors;
  std::unique_ptr<CXFA_FMExpression> pList;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  if (m_pToken->m_type != TOKidentifier) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrExpectedToken,
          XFA_FM_KeywordToString(m_pToken->m_type), ws_TempString.c_str());
  }
  wsIdentifier = m_pToken->m_wstring;
  NextToken();
  Check(TOKin);
  Check(TOKlparen);
  if (m_pToken->m_type == TOKrparen) {
    CFX_WideString ws_TempString(m_pToken->m_wstring);
    Error(m_pToken->m_uLinenum, kFMErrUnexpectedExpression,
          ws_TempString.c_str());
    NextToken();
  } else {
    pAccessors.reset(new CFX_ArrayTemplate<CXFA_FMSimpleExpression*>());
    while (m_pToken->m_type != TOKrparen) {
      CXFA_FMSimpleExpression* s = ParseSimpleExpression();
      if (s) {
        pAccessors->Add(s);
      }
      if (m_pToken->m_type == TOKcomma) {
        NextToken();
      } else {
        break;
      }
    }
    Check(TOKrparen);
  }
  Check(TOKdo);
  pList.reset(ParseBlockExpression());
  Check(TOKendfor);
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMForeachExpression(
        line, wsIdentifier, pAccessors.release(), pList.release()));
  } else {
    if (pAccessors) {
      for (int i = 0; i < pAccessors->GetSize(); ++i)
        delete static_cast<CXFA_FMSimpleExpression*>(pAccessors->GetAt(i));
    }
  }
  return e.release();
}

CXFA_FMExpression* CXFA_FMParse::ParseDoExpression() {
  std::unique_ptr<CXFA_FMExpression> e;
  uint32_t line = m_pToken->m_uLinenum;
  NextToken();
  e.reset(ParseBlockExpression());
  Check(TOKend);
  if (m_pErrorInfo->message.IsEmpty()) {
    e.reset(new CXFA_FMDoExpression(line, e.release()));
  } else {
    e.reset();
  }
  return e.release();
}
