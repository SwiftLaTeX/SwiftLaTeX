// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/fm2js/xfa_lexer.h"

#include "core/fxcrt/include/fx_ext.h"

namespace {

struct XFA_FMDChar {
  static const FX_WCHAR* inc(const FX_WCHAR*& p) {
    ++p;
    return p;
  }
  static const FX_WCHAR* dec(const FX_WCHAR*& p) {
    --p;
    return p;
  }
  static uint16_t get(const FX_WCHAR* p) { return *p; }
  static FX_BOOL isWhiteSpace(const FX_WCHAR* p) {
    return (*p) == 0x09 || (*p) == 0x0b || (*p) == 0x0c || (*p) == 0x20;
  }
  static FX_BOOL isLineTerminator(const FX_WCHAR* p) {
    return *p == 0x0A || *p == 0x0D;
  }
  static FX_BOOL isBinary(const FX_WCHAR* p) {
    return (*p) >= '0' && (*p) <= '1';
  }
  static FX_BOOL isOctal(const FX_WCHAR* p) {
    return (*p) >= '0' && (*p) <= '7';
  }
  static FX_BOOL isDigital(const FX_WCHAR* p) {
    return (*p) >= '0' && (*p) <= '9';
  }
  static FX_BOOL isHex(const FX_WCHAR* p) {
    return isDigital(p) || ((*p) >= 'a' && (*p) <= 'f') ||
           ((*p) >= 'A' && (*p) <= 'F');
  }
  static FX_BOOL isAlpha(const FX_WCHAR* p) {
    return ((*p) <= 'z' && (*p) >= 'a') || ((*p) <= 'Z' && (*p) >= 'A');
  }
  static FX_BOOL isAvalid(const FX_WCHAR* p, FX_BOOL flag = 0);
  static FX_BOOL string2number(const FX_WCHAR* s,
                               FX_DOUBLE* pValue,
                               const FX_WCHAR*& pEnd);
  static FX_BOOL isUnicodeAlpha(uint16_t ch);
};

inline FX_BOOL XFA_FMDChar::isAvalid(const FX_WCHAR* p, FX_BOOL flag) {
  if (*p == 0) {
    return 1;
  }
  if ((*p <= 0x0A && *p >= 0x09) || *p == 0x0D ||
      (*p <= 0xd7ff && *p >= 0x20) || (*p <= 0xfffd && *p >= 0xe000)) {
    return 1;
  }
  if (!flag) {
    if (*p == 0x0B || *p == 0x0C) {
      return 1;
    }
  }
  return 0;
}

inline FX_BOOL XFA_FMDChar::string2number(const FX_WCHAR* s,
                                          FX_DOUBLE* pValue,
                                          const FX_WCHAR*& pEnd) {
  if (s) {
    *pValue = wcstod((wchar_t*)s, (wchar_t**)&pEnd);
  }
  return 0;
}

inline FX_BOOL XFA_FMDChar::isUnicodeAlpha(uint16_t ch) {
  if (ch == 0 || ch == 0x0A || ch == 0x0D || ch == 0x09 || ch == 0x0B ||
      ch == 0x0C || ch == 0x20 || ch == '.' || ch == ';' || ch == '"' ||
      ch == '=' || ch == '<' || ch == '>' || ch == ',' || ch == '(' ||
      ch == ')' || ch == ']' || ch == '[' || ch == '&' || ch == '|' ||
      ch == '+' || ch == '-' || ch == '*' || ch == '/') {
    return FALSE;
  }
  return TRUE;
}

const XFA_FMKeyword keyWords[] = {
    {TOKand, 0x00000026, L"&"},
    {TOKlparen, 0x00000028, L"("},
    {TOKrparen, 0x00000029, L")"},
    {TOKmul, 0x0000002a, L"*"},
    {TOKplus, 0x0000002b, L"+"},
    {TOKcomma, 0x0000002c, L","},
    {TOKminus, 0x0000002d, L"-"},
    {TOKdot, 0x0000002e, L"."},
    {TOKdiv, 0x0000002f, L"/"},
    {TOKlt, 0x0000003c, L"<"},
    {TOKassign, 0x0000003d, L"="},
    {TOKgt, 0x0000003e, L">"},
    {TOKlbracket, 0x0000005b, L"["},
    {TOKrbracket, 0x0000005d, L"]"},
    {TOKor, 0x0000007c, L"|"},
    {TOKdotscream, 0x0000ec11, L".#"},
    {TOKdotstar, 0x0000ec18, L".*"},
    {TOKdotdot, 0x0000ec1c, L".."},
    {TOKle, 0x000133f9, L"<="},
    {TOKne, 0x000133fa, L"<>"},
    {TOKeq, 0x0001391a, L"=="},
    {TOKge, 0x00013e3b, L">="},
    {TOKdo, 0x00020153, L"do"},
    {TOKkseq, 0x00020676, L"eq"},
    {TOKksge, 0x000210ac, L"ge"},
    {TOKksgt, 0x000210bb, L"gt"},
    {TOKif, 0x00021aef, L"if"},
    {TOKin, 0x00021af7, L"in"},
    {TOKksle, 0x00022a51, L"le"},
    {TOKkslt, 0x00022a60, L"lt"},
    {TOKksne, 0x00023493, L"ne"},
    {TOKksor, 0x000239c1, L"or"},
    {TOKnull, 0x052931bb, L"null"},
    {TOKbreak, 0x05518c25, L"break"},
    {TOKksand, 0x09f9db33, L"and"},
    {TOKend, 0x0a631437, L"end"},
    {TOKeof, 0x0a63195a, L"eof"},
    {TOKfor, 0x0a7d67a7, L"for"},
    {TOKnan, 0x0b4f91dd, L"nan"},
    {TOKksnot, 0x0b4fd9b1, L"not"},
    {TOKvar, 0x0c2203e9, L"var"},
    {TOKthen, 0x2d5738cf, L"then"},
    {TOKelse, 0x45f65ee9, L"else"},
    {TOKexit, 0x4731d6ba, L"exit"},
    {TOKdownto, 0x4caadc3b, L"downto"},
    {TOKreturn, 0x4db8bd60, L"return"},
    {TOKinfinity, 0x5c0a010a, L"infinity"},
    {TOKendwhile, 0x5c64bff0, L"endwhile"},
    {TOKforeach, 0x67e31f38, L"foreach"},
    {TOKendfunc, 0x68f984a3, L"endfunc"},
    {TOKelseif, 0x78253218, L"elseif"},
    {TOKwhile, 0x84229259, L"while"},
    {TOKendfor, 0x8ab49d7e, L"endfor"},
    {TOKthrow, 0x8db05c94, L"throw"},
    {TOKstep, 0xa7a7887c, L"step"},
    {TOKupto, 0xb5155328, L"upto"},
    {TOKcontinue, 0xc0340685, L"continue"},
    {TOKfunc, 0xcdce60ec, L"func"},
    {TOKendif, 0xe0e8fee6, L"endif"},
};

const XFA_FM_TOKEN KEYWORD_START = TOKdo;
const XFA_FM_TOKEN KEYWORD_END = TOKendif;

}  // namespace

const FX_WCHAR* XFA_FM_KeywordToString(XFA_FM_TOKEN op) {
  if (op < KEYWORD_START || op > KEYWORD_END)
    return L"";
  return keyWords[op].m_keyword;
}

CXFA_FMToken::CXFA_FMToken() : m_type(TOKreserver), m_uLinenum(1) {}

CXFA_FMToken::CXFA_FMToken(uint32_t uLineNum)
    : m_type(TOKreserver), m_uLinenum(uLineNum) {}

CXFA_FMLexer::CXFA_FMLexer(const CFX_WideStringC& wsFormCalc,
                           CXFA_FMErrorInfo* pErrorInfo)
    : m_ptr(wsFormCalc.c_str()), m_uCurrentLine(1), m_pErrorInfo(pErrorInfo) {}

CXFA_FMLexer::~CXFA_FMLexer() {}

CXFA_FMToken* CXFA_FMLexer::NextToken() {
  m_pToken.reset(Scan());
  return m_pToken.get();
}

CXFA_FMToken* CXFA_FMLexer::Scan() {
  uint16_t ch = 0;
  CXFA_FMToken* p = new CXFA_FMToken(m_uCurrentLine);
  if (!XFA_FMDChar::isAvalid(m_ptr)) {
    ch = XFA_FMDChar::get(m_ptr);
    Error(kFMErrUnsupportedChar, ch);
    return p;
  }
  int iRet = 0;
  while (1) {
    if (!XFA_FMDChar::isAvalid(m_ptr)) {
      ch = XFA_FMDChar::get(m_ptr);
      Error(kFMErrUnsupportedChar, ch);
      return p;
    }
    ch = XFA_FMDChar::get(m_ptr);
    switch (ch) {
      case 0:
        p->m_type = TOKeof;
        return p;
      case 0x0A:
        ++m_uCurrentLine;
        p->m_uLinenum = m_uCurrentLine;
        XFA_FMDChar::inc(m_ptr);
        break;
      case 0x0D:
        XFA_FMDChar::inc(m_ptr);
        break;
      case ';': {
        const FX_WCHAR* pTemp = 0;
        Comment(m_ptr, pTemp);
        m_ptr = pTemp;
      } break;
      case '"': {
        const FX_WCHAR* pTemp = 0;
        p->m_type = TOKstring;
        iRet = String(p, m_ptr, pTemp);
        m_ptr = pTemp;
      }
        return p;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        p->m_type = TOKnumber;
        const FX_WCHAR* pTemp = 0;
        iRet = Number(p, m_ptr, pTemp);
        m_ptr = pTemp;
        if (iRet) {
          Error(kFMErrBadSuffixNumber);
          return p;
        }
      }
        return p;
      case '=':
        XFA_FMDChar::inc(m_ptr);
        if (XFA_FMDChar::isAvalid(m_ptr)) {
          ch = XFA_FMDChar::get(m_ptr);
          if (ch == '=') {
            p->m_type = TOKeq;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else {
            p->m_type = TOKassign;
            return p;
          }
        } else {
          ch = XFA_FMDChar::get(m_ptr);
          Error(kFMErrUnsupportedChar, ch);
          return p;
        }
        break;
      case '<':
        XFA_FMDChar::inc(m_ptr);
        if (XFA_FMDChar::isAvalid(m_ptr)) {
          ch = XFA_FMDChar::get(m_ptr);
          if (ch == '=') {
            p->m_type = TOKle;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else if (ch == '>') {
            p->m_type = TOKne;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else {
            p->m_type = TOKlt;
            return p;
          }
        } else {
          ch = XFA_FMDChar::get(m_ptr);
          Error(kFMErrUnsupportedChar, ch);
          return p;
        }
        break;
      case '>':
        XFA_FMDChar::inc(m_ptr);
        if (XFA_FMDChar::isAvalid(m_ptr)) {
          ch = XFA_FMDChar::get(m_ptr);
          if (ch == '=') {
            p->m_type = TOKge;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else {
            p->m_type = TOKgt;
            return p;
          }
        } else {
          ch = XFA_FMDChar::get(m_ptr);
          Error(kFMErrUnsupportedChar, ch);
          return p;
        }
        break;
      case ',':
        p->m_type = TOKcomma;
        XFA_FMDChar::inc(m_ptr);
        return p;
      case '(':
        p->m_type = TOKlparen;
        XFA_FMDChar::inc(m_ptr);
        return p;
      case ')':
        p->m_type = TOKrparen;
        XFA_FMDChar::inc(m_ptr);
        return p;
      case '[':
        p->m_type = TOKlbracket;
        XFA_FMDChar::inc(m_ptr);
        return p;
      case ']':
        p->m_type = TOKrbracket;
        XFA_FMDChar::inc(m_ptr);
        return p;
      case '&':
        XFA_FMDChar::inc(m_ptr);
        p->m_type = TOKand;
        return p;
      case '|':
        XFA_FMDChar::inc(m_ptr);
        p->m_type = TOKor;
        return p;
      case '+':
        XFA_FMDChar::inc(m_ptr);
        p->m_type = TOKplus;
        return p;
      case '-':
        XFA_FMDChar::inc(m_ptr);
        p->m_type = TOKminus;
        return p;
      case '*':
        XFA_FMDChar::inc(m_ptr);
        p->m_type = TOKmul;
        return p;
      case '/':
        XFA_FMDChar::inc(m_ptr);
        if (XFA_FMDChar::isAvalid(m_ptr)) {
          ch = XFA_FMDChar::get(m_ptr);
          if (ch == '/') {
            const FX_WCHAR* pTemp = 0;
            Comment(m_ptr, pTemp);
            m_ptr = pTemp;
            break;
          } else {
            p->m_type = TOKdiv;
            return p;
          }
        } else {
          ch = XFA_FMDChar::get(m_ptr);
          Error(kFMErrUnsupportedChar, ch);
          return p;
        }
        break;
      case '.':
        XFA_FMDChar::inc(m_ptr);
        if (XFA_FMDChar::isAvalid(m_ptr)) {
          ch = XFA_FMDChar::get(m_ptr);
          if (ch == '.') {
            p->m_type = TOKdotdot;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else if (ch == '*') {
            p->m_type = TOKdotstar;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else if (ch == '#') {
            p->m_type = TOKdotscream;
            XFA_FMDChar::inc(m_ptr);
            return p;
          } else if (ch <= '9' && ch >= '0') {
            p->m_type = TOKnumber;
            const FX_WCHAR* pTemp = 0;
            XFA_FMDChar::dec(m_ptr);
            iRet = Number(p, m_ptr, pTemp);
            m_ptr = pTemp;
            if (iRet) {
              Error(kFMErrBadSuffixNumber);
            }
            return p;
          } else {
            p->m_type = TOKdot;
            return p;
          }
        } else {
          ch = XFA_FMDChar::get(m_ptr);
          Error(kFMErrUnsupportedChar, ch);
          return p;
        }
      case 0x09:
      case 0x0B:
      case 0x0C:
      case 0x20:
        XFA_FMDChar::inc(m_ptr);
        break;
      default: {
        const FX_WCHAR* pTemp = 0;
        iRet = Identifiers(p, m_ptr, pTemp);
        m_ptr = pTemp;
        if (iRet) {
          return p;
        }
        p->m_type = IsKeyword(p->m_wstring);
      }
        return p;
    }
  }
}

uint32_t CXFA_FMLexer::Number(CXFA_FMToken* t,
                              const FX_WCHAR* p,
                              const FX_WCHAR*& pEnd) {
  FX_DOUBLE number = 0;
  if (XFA_FMDChar::string2number(p, &number, pEnd)) {
    return 1;
  }
  if (pEnd && XFA_FMDChar::isAlpha(pEnd)) {
    return 1;
  }
  t->m_wstring = CFX_WideStringC(p, (pEnd - p));
  return 0;
}

uint32_t CXFA_FMLexer::String(CXFA_FMToken* t,
                              const FX_WCHAR* p,
                              const FX_WCHAR*& pEnd) {
  const FX_WCHAR* pStart = p;
  uint16_t ch = 0;
  XFA_FMDChar::inc(p);
  ch = XFA_FMDChar::get(p);
  while (ch) {
    if (!XFA_FMDChar::isAvalid(p)) {
      ch = XFA_FMDChar::get(p);
      pEnd = p;
      t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
      Error(kFMErrUnsupportedChar, ch);
      return 1;
    }
    if (ch == '"') {
      XFA_FMDChar::inc(p);
      if (!XFA_FMDChar::isAvalid(p)) {
        ch = XFA_FMDChar::get(p);
        pEnd = p;
        t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
        Error(kFMErrUnsupportedChar, ch);
        return 1;
      }
      ch = XFA_FMDChar::get(p);
      if (ch == '"') {
        goto NEXT;
      } else {
        break;
      }
    }
  NEXT:
    XFA_FMDChar::inc(p);
    ch = XFA_FMDChar::get(p);
  }
  pEnd = p;
  t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
  return 0;
}

uint32_t CXFA_FMLexer::Identifiers(CXFA_FMToken* t,
                                   const FX_WCHAR* p,
                                   const FX_WCHAR*& pEnd) {
  const FX_WCHAR* pStart = p;
  uint16_t ch = 0;
  ch = XFA_FMDChar::get(p);
  XFA_FMDChar::inc(p);
  if (!XFA_FMDChar::isAvalid(p)) {
    pEnd = p;
    t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
    Error(kFMErrUnsupportedChar, ch);
    return 1;
  }
  ch = XFA_FMDChar::get(p);
  while (ch) {
    if (!XFA_FMDChar::isAvalid(p)) {
      pEnd = p;
      t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
      Error(kFMErrUnsupportedChar, ch);
      return 1;
    }
    ch = XFA_FMDChar::get(p);
    if (XFA_FMDChar::isUnicodeAlpha(ch)) {
      XFA_FMDChar::inc(p);
    } else {
      pEnd = p;
      t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
      return 0;
    }
  }
  pEnd = p;
  t->m_wstring = CFX_WideStringC(pStart, (pEnd - pStart));
  return 0;
}

void CXFA_FMLexer::Comment(const FX_WCHAR* p, const FX_WCHAR*& pEnd) {
  unsigned ch = 0;
  XFA_FMDChar::inc(p);
  ch = XFA_FMDChar::get(p);
  while (ch) {
    if (ch == 0x0D) {
      XFA_FMDChar::inc(p);
      pEnd = p;
      return;
    }
    if (ch == 0x0A) {
      ++m_uCurrentLine;
      XFA_FMDChar::inc(p);
      pEnd = p;
      return;
    }
    XFA_FMDChar::inc(p);
    ch = XFA_FMDChar::get(p);
  }
  pEnd = p;
}

XFA_FM_TOKEN CXFA_FMLexer::IsKeyword(const CFX_WideStringC& str) {
  uint32_t uHash = FX_HashCode_GetW(str, true);
  int32_t iStart = KEYWORD_START;
  int32_t iEnd = KEYWORD_END;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    XFA_FMKeyword keyword = keyWords[iMid];
    if (uHash == keyword.m_uHash)
      return keyword.m_type;
    if (uHash < keyword.m_uHash)
      iEnd = iMid - 1;
    else
      iStart = iMid + 1;
  } while (iStart <= iEnd);
  return TOKidentifier;
}

void CXFA_FMLexer::Error(const FX_WCHAR* msg, ...) {
  m_pErrorInfo->linenum = m_uCurrentLine;
  va_list ap;
  va_start(ap, msg);
  m_pErrorInfo->message.FormatV(msg, ap);
  va_end(ap);
}

FX_BOOL CXFA_FMLexer::HasError() const {
  if (m_pErrorInfo->message.IsEmpty()) {
    return FALSE;
  }
  return TRUE;
}
