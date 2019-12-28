// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_localevalue.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fgas/localization/fgas_localeimp.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

static const FX_DOUBLE fraction_scales[] = {0.1,
                                            0.01,
                                            0.001,
                                            0.0001,
                                            0.00001,
                                            0.000001,
                                            0.0000001,
                                            0.00000001,
                                            0.000000001,
                                            0.0000000001,
                                            0.00000000001,
                                            0.000000000001,
                                            0.0000000000001,
                                            0.00000000000001,
                                            0.000000000000001,
                                            0.0000000000000001};
CXFA_LocaleValue::CXFA_LocaleValue() {
  m_dwType = XFA_VT_NULL;
  m_bValid = TRUE;
  m_pLocaleMgr = nullptr;
}
CXFA_LocaleValue::CXFA_LocaleValue(const CXFA_LocaleValue& value) {
  m_dwType = XFA_VT_NULL;
  m_bValid = TRUE;
  m_pLocaleMgr = nullptr;
  *this = value;
}
CXFA_LocaleValue::CXFA_LocaleValue(uint32_t dwType,
                                   CXFA_LocaleMgr* pLocaleMgr) {
  m_dwType = dwType;
  m_bValid = (m_dwType != XFA_VT_NULL);
  m_pLocaleMgr = pLocaleMgr;
}
CXFA_LocaleValue::CXFA_LocaleValue(uint32_t dwType,
                                   const CFX_WideString& wsValue,
                                   CXFA_LocaleMgr* pLocaleMgr) {
  m_wsValue = wsValue;
  m_dwType = dwType;
  m_pLocaleMgr = pLocaleMgr;
  m_bValid = ValidateCanonicalValue(wsValue, dwType);
}
CXFA_LocaleValue::CXFA_LocaleValue(uint32_t dwType,
                                   const CFX_WideString& wsValue,
                                   const CFX_WideString& wsFormat,
                                   IFX_Locale* pLocale,
                                   CXFA_LocaleMgr* pLocaleMgr) {
  m_pLocaleMgr = pLocaleMgr;
  m_bValid = TRUE;
  m_dwType = dwType;
  m_bValid = ParsePatternValue(wsValue, wsFormat, pLocale);
}
CXFA_LocaleValue& CXFA_LocaleValue::operator=(const CXFA_LocaleValue& value) {
  m_wsValue = value.m_wsValue;
  m_dwType = value.m_dwType;
  m_bValid = value.m_bValid;
  m_pLocaleMgr = value.m_pLocaleMgr;
  return *this;
}
CXFA_LocaleValue::~CXFA_LocaleValue() {}
static FX_LOCALECATEGORY XFA_ValugeCategory(FX_LOCALECATEGORY eCategory,
                                            uint32_t dwValueType) {
  if (eCategory == FX_LOCALECATEGORY_Unknown) {
    switch (dwValueType) {
      case XFA_VT_BOOLEAN:
      case XFA_VT_INTEGER:
      case XFA_VT_DECIMAL:
      case XFA_VT_FLOAT:
        return FX_LOCALECATEGORY_Num;
      case XFA_VT_TEXT:
        return FX_LOCALECATEGORY_Text;
      case XFA_VT_DATE:
        return FX_LOCALECATEGORY_Date;
      case XFA_VT_TIME:
        return FX_LOCALECATEGORY_Time;
      case XFA_VT_DATETIME:
        return FX_LOCALECATEGORY_DateTime;
    }
  }
  return eCategory;
}
FX_BOOL CXFA_LocaleValue::ValidateValue(const CFX_WideString& wsValue,
                                        const CFX_WideString& wsPattern,
                                        IFX_Locale* pLocale,
                                        CFX_WideString* pMatchFormat) {
  CFX_WideString wsOutput;
  IFX_Locale* locale = m_pLocaleMgr->GetDefLocale();
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(pLocale);
  }
  CFX_FormatString* pFormat = nullptr;
  if (m_pLocaleMgr)
    pFormat = new CFX_FormatString(m_pLocaleMgr, FALSE);

  CFX_WideStringArray wsPatterns;
  pFormat->SplitFormatString(wsPattern, wsPatterns);
  FX_BOOL bRet = FALSE;
  int32_t iCount = wsPatterns.GetSize();
  int32_t i = 0;
  for (; i < iCount && !bRet; i++) {
    CFX_WideString wsFormat = wsPatterns[i];
    FX_LOCALECATEGORY eCategory = pFormat->GetCategory(wsFormat);
    eCategory = XFA_ValugeCategory(eCategory, m_dwType);
    switch (eCategory) {
      case FX_LOCALECATEGORY_Null:
        bRet = pFormat->ParseNull(wsValue, wsFormat);
        if (!bRet) {
          bRet = wsValue.IsEmpty();
        }
        break;
      case FX_LOCALECATEGORY_Zero:
        bRet = pFormat->ParseZero(wsValue, wsFormat);
        if (!bRet) {
          bRet = wsValue == FX_WSTRC(L"0");
        }
        break;
      case FX_LOCALECATEGORY_Num: {
        CFX_WideString fNum;
        bRet = pFormat->ParseNum(wsValue, wsFormat, fNum);
        if (!bRet) {
          bRet = pFormat->FormatNum(wsValue, wsFormat, wsOutput);
        }
        break;
      }
      case FX_LOCALECATEGORY_Text:
        bRet = pFormat->ParseText(wsValue, wsFormat, wsOutput);
        wsOutput.clear();
        if (!bRet) {
          bRet = pFormat->FormatText(wsValue, wsFormat, wsOutput);
        }
        break;
      case FX_LOCALECATEGORY_Date: {
        CFX_Unitime dt;
        bRet = ValidateCanonicalDate(wsValue, dt);
        if (!bRet) {
          bRet = pFormat->ParseDateTime(wsValue, wsFormat, FX_DATETIMETYPE_Date,
                                        dt);
          if (!bRet) {
            bRet = pFormat->FormatDateTime(wsValue, wsFormat, wsOutput,
                                           FX_DATETIMETYPE_Date);
          }
        }
        break;
      }
      case FX_LOCALECATEGORY_Time: {
        CFX_Unitime dt;
        bRet =
            pFormat->ParseDateTime(wsValue, wsFormat, FX_DATETIMETYPE_Time, dt);
        if (!bRet) {
          bRet = pFormat->FormatDateTime(wsValue, wsFormat, wsOutput,
                                         FX_DATETIMETYPE_Time);
        }
        break;
      }
      case FX_LOCALECATEGORY_DateTime: {
        CFX_Unitime dt;
        bRet = pFormat->ParseDateTime(wsValue, wsFormat,
                                      FX_DATETIMETYPE_DateTime, dt);
        if (!bRet) {
          bRet = pFormat->FormatDateTime(wsValue, wsFormat, wsOutput,
                                         FX_DATETIMETYPE_DateTime);
        }
        break;
      }
      default:
        bRet = FALSE;
        break;
    }
  }
  if (bRet && pMatchFormat) {
    *pMatchFormat = wsPatterns[i - 1];
  }
  pFormat->Release();
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(locale);
  }
  return bRet;
}
CFX_WideString CXFA_LocaleValue::GetValue() const {
  return m_wsValue;
}
uint32_t CXFA_LocaleValue::GetType() const {
  return m_dwType;
}
void CXFA_LocaleValue::SetValue(const CFX_WideString& wsValue,
                                uint32_t dwType) {
  m_wsValue = wsValue;
  m_dwType = dwType;
}
CFX_WideString CXFA_LocaleValue::GetText() const {
  if (m_bValid && m_dwType == XFA_VT_TEXT) {
    return m_wsValue;
  }
  return CFX_WideString();
}
FX_FLOAT CXFA_LocaleValue::GetNum() const {
  if (m_bValid && (m_dwType == XFA_VT_BOOLEAN || m_dwType == XFA_VT_INTEGER ||
                   m_dwType == XFA_VT_DECIMAL || m_dwType == XFA_VT_FLOAT)) {
    int64_t nIntegral = 0;
    uint32_t dwFractional = 0;
    int32_t nExponent = 0;
    int cc = 0;
    FX_BOOL bNegative = FALSE, bExpSign = FALSE;
    const FX_WCHAR* str = m_wsValue.c_str();
    int len = m_wsValue.GetLength();
    while (FXSYS_iswspace(str[cc]) && cc < len) {
      cc++;
    }
    if (cc >= len) {
      return 0;
    }
    if (str[0] == '+') {
      cc++;
    } else if (str[0] == '-') {
      bNegative = TRUE;
      cc++;
    }
    int nIntegralLen = 0;
    while (cc < len) {
      if (str[cc] == '.' || !FXSYS_isDecimalDigit(str[cc]) ||
          nIntegralLen > 17) {
        break;
      }
      nIntegral = nIntegral * 10 + str[cc] - '0';
      cc++;
      nIntegralLen++;
    }
    nIntegral = bNegative ? -nIntegral : nIntegral;
    int scale = 0;
    double fraction = 0.0;
    if (cc < len && str[cc] == '.') {
      cc++;
      while (cc < len) {
        fraction += fraction_scales[scale] * (str[cc] - '0');
        scale++;
        cc++;
        if (scale == sizeof fraction_scales / sizeof(double) ||
            !FXSYS_isDecimalDigit(str[cc])) {
          break;
        }
      }
      dwFractional = (uint32_t)(fraction * 4294967296.0);
    }
    if (cc < len && (str[cc] == 'E' || str[cc] == 'e')) {
      cc++;
      if (cc < len) {
        if (str[cc] == '+') {
          cc++;
        } else if (str[cc] == '-') {
          bExpSign = TRUE;
          cc++;
        }
      }
      while (cc < len) {
        if (str[cc] == '.' || !FXSYS_isDecimalDigit(str[cc])) {
          break;
        }
        nExponent = nExponent * 10 + str[cc] - '0';
        cc++;
      }
      nExponent = bExpSign ? -nExponent : nExponent;
    }
    FX_FLOAT fValue = (FX_FLOAT)(dwFractional / 4294967296.0);
    fValue = nIntegral + (nIntegral >= 0 ? fValue : -fValue);
    if (nExponent != 0) {
      fValue *= FXSYS_pow(10, (FX_FLOAT)nExponent);
    }
    return fValue;
  }
  return 0;
}
FX_DOUBLE CXFA_LocaleValue::GetDoubleNum() const {
  if (m_bValid && (m_dwType == XFA_VT_BOOLEAN || m_dwType == XFA_VT_INTEGER ||
                   m_dwType == XFA_VT_DECIMAL || m_dwType == XFA_VT_FLOAT)) {
    int64_t nIntegral = 0;
    uint32_t dwFractional = 0;
    int32_t nExponent = 0;
    int32_t cc = 0;
    FX_BOOL bNegative = FALSE, bExpSign = FALSE;
    const FX_WCHAR* str = m_wsValue.c_str();
    int len = m_wsValue.GetLength();
    while (FXSYS_iswspace(str[cc]) && cc < len) {
      cc++;
    }
    if (cc >= len) {
      return 0;
    }
    if (str[0] == '+') {
      cc++;
    } else if (str[0] == '-') {
      bNegative = TRUE;
      cc++;
    }
    int32_t nIntegralLen = 0;
    while (cc < len) {
      if (str[cc] == '.' || !FXSYS_isDecimalDigit(str[cc]) ||
          nIntegralLen > 17) {
        break;
      }
      nIntegral = nIntegral * 10 + str[cc] - '0';
      cc++;
      nIntegralLen++;
    }
    nIntegral = bNegative ? -nIntegral : nIntegral;
    int32_t scale = 0;
    FX_DOUBLE fraction = 0.0;
    if (cc < len && str[cc] == '.') {
      cc++;
      while (cc < len) {
        fraction += fraction_scales[scale] * (str[cc] - '0');
        scale++;
        cc++;
        if (scale == sizeof fraction_scales / sizeof(FX_DOUBLE) ||
            !FXSYS_isDecimalDigit(str[cc])) {
          break;
        }
      }
      dwFractional = (uint32_t)(fraction * 4294967296.0);
    }
    if (cc < len && (str[cc] == 'E' || str[cc] == 'e')) {
      cc++;
      if (cc < len) {
        if (str[cc] == '+') {
          cc++;
        } else if (str[cc] == '-') {
          bExpSign = TRUE;
          cc++;
        }
      }
      while (cc < len) {
        if (str[cc] == '.' || !FXSYS_isDecimalDigit(str[cc])) {
          break;
        }
        nExponent = nExponent * 10 + str[cc] - '0';
        cc++;
      }
      nExponent = bExpSign ? -nExponent : nExponent;
    }
    FX_DOUBLE dValue = (dwFractional / 4294967296.0);
    dValue = nIntegral + (nIntegral >= 0 ? dValue : -dValue);
    if (nExponent != 0) {
      dValue *= FXSYS_pow(10, (FX_FLOAT)nExponent);
    }
    return dValue;
  }
  return 0;
}
CFX_Unitime CXFA_LocaleValue::GetDate() const {
  if (m_bValid && m_dwType == XFA_VT_DATE) {
    CFX_Unitime dt;
    FX_DateFromCanonical(m_wsValue, dt);
    return dt;
  }
  return CFX_Unitime();
}
CFX_Unitime CXFA_LocaleValue::GetTime() const {
  if (m_bValid && m_dwType == XFA_VT_TIME) {
    CFX_Unitime dt(0);
    ASSERT(m_pLocaleMgr);
    FX_TimeFromCanonical(m_wsValue.AsStringC(), dt,
                         m_pLocaleMgr->GetDefLocale());
    return dt;
  }
  return CFX_Unitime();
}
CFX_Unitime CXFA_LocaleValue::GetDateTime() const {
  if (m_bValid && m_dwType == XFA_VT_DATETIME) {
    int32_t index = m_wsValue.Find('T');
    CFX_Unitime dt;
    FX_DateFromCanonical(m_wsValue.Left(index), dt);
    ASSERT(m_pLocaleMgr);
    FX_TimeFromCanonical(
        m_wsValue.Right(m_wsValue.GetLength() - index - 1).AsStringC(), dt,
        m_pLocaleMgr->GetDefLocale());
    return dt;
  }
  return CFX_Unitime();
}
FX_BOOL CXFA_LocaleValue::SetText(const CFX_WideString& wsText) {
  m_dwType = XFA_VT_TEXT;
  m_wsValue = wsText;
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::SetText(const CFX_WideString& wsText,
                                  const CFX_WideString& wsFormat,
                                  IFX_Locale* pLocale) {
  m_dwType = XFA_VT_TEXT;
  return m_bValid = ParsePatternValue(wsText, wsFormat, pLocale);
}
FX_BOOL CXFA_LocaleValue::SetNum(FX_FLOAT fNum) {
  m_dwType = XFA_VT_FLOAT;
  m_wsValue.Format(L"%.8g", (FX_DOUBLE)fNum);
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::SetNum(const CFX_WideString& wsNum,
                                 const CFX_WideString& wsFormat,
                                 IFX_Locale* pLocale) {
  m_dwType = XFA_VT_FLOAT;
  return m_bValid = ParsePatternValue(wsNum, wsFormat, pLocale);
}
FX_BOOL CXFA_LocaleValue::SetDate(const CFX_Unitime& d) {
  m_dwType = XFA_VT_DATE;
  m_wsValue.Format(L"%04d-%02d-%02d", d.GetYear(), d.GetMonth(), d.GetDay());
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::SetDate(const CFX_WideString& wsDate,
                                  const CFX_WideString& wsFormat,
                                  IFX_Locale* pLocale) {
  m_dwType = XFA_VT_DATE;
  return m_bValid = ParsePatternValue(wsDate, wsFormat, pLocale);
}
FX_BOOL CXFA_LocaleValue::SetTime(const CFX_Unitime& t) {
  m_dwType = XFA_VT_TIME;
  m_wsValue.Format(L"%02d:%02d:%02d", t.GetHour(), t.GetMinute(),
                   t.GetSecond());
  if (t.GetMillisecond() > 0) {
    CFX_WideString wsTemp;
    wsTemp.Format(L"%:03d", t.GetMillisecond());
    m_wsValue += wsTemp;
  }
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::SetTime(const CFX_WideString& wsTime,
                                  const CFX_WideString& wsFormat,
                                  IFX_Locale* pLocale) {
  m_dwType = XFA_VT_TIME;
  return m_bValid = ParsePatternValue(wsTime, wsFormat, pLocale);
}
FX_BOOL CXFA_LocaleValue::SetDateTime(const CFX_Unitime& dt) {
  m_dwType = XFA_VT_DATETIME;
  m_wsValue.Format(L"%04d-%02d-%02dT%02d:%02d:%02d", dt.GetYear(),
                   dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(),
                   dt.GetSecond());
  if (dt.GetMillisecond() > 0) {
    CFX_WideString wsTemp;
    wsTemp.Format(L"%:03d", dt.GetMillisecond());
    m_wsValue += wsTemp;
  }
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::SetDateTime(const CFX_WideString& wsDateTime,
                                      const CFX_WideString& wsFormat,
                                      IFX_Locale* pLocale) {
  m_dwType = XFA_VT_DATETIME;
  return m_bValid = ParsePatternValue(wsDateTime, wsFormat, pLocale);
}
FX_BOOL CXFA_LocaleValue::FormatPatterns(CFX_WideString& wsResult,
                                         const CFX_WideString& wsFormat,
                                         IFX_Locale* pLocale,
                                         XFA_VALUEPICTURE eValueType) const {
  wsResult.clear();
  FX_BOOL bRet = FALSE;

  CFX_FormatString* pFormat = nullptr;
  if (m_pLocaleMgr)
    pFormat = new CFX_FormatString(m_pLocaleMgr, FALSE);

  CFX_WideStringArray wsPatterns;
  pFormat->SplitFormatString(wsFormat, wsPatterns);
  int32_t iCount = wsPatterns.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    bRet = FormatSinglePattern(wsResult, wsPatterns[i], pLocale, eValueType);
    if (bRet) {
      break;
    }
  }
  pFormat->Release();
  return bRet;
}
FX_BOOL CXFA_LocaleValue::FormatSinglePattern(
    CFX_WideString& wsResult,
    const CFX_WideString& wsFormat,
    IFX_Locale* pLocale,
    XFA_VALUEPICTURE eValueType) const {
  IFX_Locale* locale = m_pLocaleMgr->GetDefLocale();
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(pLocale);
  }
  wsResult.clear();
  FX_BOOL bRet = FALSE;

  CFX_FormatString* pFormat = nullptr;
  if (m_pLocaleMgr)
    pFormat = new CFX_FormatString(m_pLocaleMgr, FALSE);

  FX_LOCALECATEGORY eCategory = pFormat->GetCategory(wsFormat);
  eCategory = XFA_ValugeCategory(eCategory, m_dwType);
  switch (eCategory) {
    case FX_LOCALECATEGORY_Null:
      if (m_wsValue.IsEmpty()) {
        bRet = pFormat->FormatNull(wsFormat, wsResult);
      }
      break;
    case FX_LOCALECATEGORY_Zero:
      if (m_wsValue == FX_WSTRC(L"0")) {
        bRet = pFormat->FormatZero(wsFormat, wsResult);
      }
      break;
    case FX_LOCALECATEGORY_Num:
      bRet = pFormat->FormatNum(m_wsValue, wsFormat, wsResult);
      break;
    case FX_LOCALECATEGORY_Text:
      bRet = pFormat->FormatText(m_wsValue, wsFormat, wsResult);
      break;
    case FX_LOCALECATEGORY_Date:
      bRet = pFormat->FormatDateTime(m_wsValue, wsFormat, wsResult,
                                     FX_DATETIMETYPE_Date);
      break;
    case FX_LOCALECATEGORY_Time:
      bRet = pFormat->FormatDateTime(m_wsValue, wsFormat, wsResult,
                                     FX_DATETIMETYPE_Time);
      break;
    case FX_LOCALECATEGORY_DateTime:
      bRet = pFormat->FormatDateTime(m_wsValue, wsFormat, wsResult,
                                     FX_DATETIMETYPE_DateTime);
      break;
    default:
      wsResult = m_wsValue;
      bRet = TRUE;
  }
  pFormat->Release();
  if (!bRet && (eCategory != FX_LOCALECATEGORY_Num ||
                eValueType != XFA_VALUEPICTURE_Display)) {
    wsResult = m_wsValue;
  }
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(locale);
  }
  return bRet;
}
static FX_BOOL XFA_ValueSplitDateTime(const CFX_WideString& wsDateTime,
                                      CFX_WideString& wsDate,
                                      CFX_WideString& wsTime) {
  wsDate = L"";
  wsTime = L"";
  if (wsDateTime.IsEmpty()) {
    return FALSE;
  }
  int nSplitIndex = -1;
  nSplitIndex = wsDateTime.Find('T');
  if (nSplitIndex < 0) {
    nSplitIndex = wsDateTime.Find(' ');
  }
  if (nSplitIndex < 0) {
    return FALSE;
  }
  wsDate = wsDateTime.Left(nSplitIndex);
  wsTime = wsDateTime.Right(wsDateTime.GetLength() - nSplitIndex - 1);
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::ValidateCanonicalValue(const CFX_WideString& wsValue,
                                                 uint32_t dwVType) {
  if (wsValue.IsEmpty()) {
    return TRUE;
  }
  CFX_Unitime dt;
  switch (dwVType) {
    case XFA_VT_DATE: {
      if (ValidateCanonicalDate(wsValue, dt)) {
        return TRUE;
      }
      CFX_WideString wsDate, wsTime;
      if (XFA_ValueSplitDateTime(wsValue, wsDate, wsTime) &&
          ValidateCanonicalDate(wsDate, dt)) {
        return TRUE;
      }
      return FALSE;
    }
    case XFA_VT_TIME: {
      if (ValidateCanonicalTime(wsValue)) {
        return TRUE;
      }
      CFX_WideString wsDate, wsTime;
      if (XFA_ValueSplitDateTime(wsValue, wsDate, wsTime) &&
          ValidateCanonicalTime(wsTime)) {
        return TRUE;
      }
      return FALSE;
    }
    case XFA_VT_DATETIME: {
      CFX_WideString wsDate, wsTime;
      if (XFA_ValueSplitDateTime(wsValue, wsDate, wsTime) &&
          ValidateCanonicalDate(wsDate, dt) && ValidateCanonicalTime(wsTime)) {
        return TRUE;
      }
    } break;
  }
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::ValidateCanonicalDate(const CFX_WideString& wsDate,
                                                CFX_Unitime& unDate) {
  const uint16_t LastDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  const uint16_t wCountY = 4, wCountM = 2, wCountD = 2;
  int nLen = wsDate.GetLength();
  if (nLen < wCountY || nLen > wCountY + wCountM + wCountD + 2) {
    return FALSE;
  }
  const bool bSymbol = wsDate.Find(0x2D) != -1;
  uint16_t wYear = 0;
  uint16_t wMonth = 0;
  uint16_t wDay = 0;
  const FX_WCHAR* pDate = wsDate.c_str();
  int nIndex = 0, nStart = 0;
  while (pDate[nIndex] != '\0' && nIndex < wCountY) {
    if (!FXSYS_isDecimalDigit(pDate[nIndex])) {
      return FALSE;
    }
    wYear = (pDate[nIndex] - '0') + wYear * 10;
    nIndex++;
  }
  if (bSymbol) {
    if (pDate[nIndex] != 0x2D) {
      return FALSE;
    }
    nIndex++;
  }
  nStart = nIndex;
  while (pDate[nIndex] != '\0' && nIndex - nStart < wCountM && nIndex < nLen) {
    if (!FXSYS_isDecimalDigit(pDate[nIndex])) {
      return FALSE;
    }
    wMonth = (pDate[nIndex] - '0') + wMonth * 10;
    nIndex++;
  }
  if (bSymbol) {
    if (pDate[nIndex] != 0x2D) {
      return FALSE;
    }
    nIndex++;
  }
  nStart = nIndex;
  while (pDate[nIndex] != '\0' && nIndex - nStart < wCountD && nIndex < nLen) {
    if (!FXSYS_isDecimalDigit(pDate[nIndex])) {
      return FALSE;
    }
    wDay = (pDate[nIndex] - '0') + wDay * 10;
    nIndex++;
  }
  if (nIndex != nLen) {
    return FALSE;
  }
  if (wYear < 1900 || wYear > 2029) {
    return FALSE;
  }
  if (wMonth < 1 || wMonth > 12) {
    if (wMonth == 0 && nLen == wCountY) {
      return TRUE;
    }
    return FALSE;
  }
  if (wDay < 1) {
    if (wDay == 0 && (nLen == wCountY + wCountM)) {
      return TRUE;
    }
    return FALSE;
  }
  if (wMonth == 2) {
    if (wYear % 400 == 0 || (wYear % 100 != 0 && wYear % 4 == 0)) {
      if (wDay > 29) {
        return FALSE;
      }
    } else {
      if (wDay > 28) {
        return FALSE;
      }
    }
  } else if (wDay > LastDay[wMonth - 1]) {
    return FALSE;
  }
  CFX_Unitime ut;
  ut.Set(wYear, static_cast<uint8_t>(wMonth), static_cast<uint8_t>(wDay));
  unDate = unDate + ut;
  return TRUE;
}
FX_BOOL CXFA_LocaleValue::ValidateCanonicalTime(const CFX_WideString& wsTime) {
  int nLen = wsTime.GetLength();
  if (nLen < 2)
    return FALSE;
  const uint16_t wCountH = 2;
  const uint16_t wCountM = 2;
  const uint16_t wCountS = 2;
  const uint16_t wCountF = 3;
  const bool bSymbol = wsTime.Find(':') != -1;
  uint16_t wHour = 0;
  uint16_t wMinute = 0;
  uint16_t wSecond = 0;
  uint16_t wFraction = 0;
  const FX_WCHAR* pTime = wsTime.c_str();
  int nIndex = 0;
  int nStart = 0;
  while (nIndex - nStart < wCountH && pTime[nIndex]) {
    if (!FXSYS_isDecimalDigit(pTime[nIndex]))
      return FALSE;
    wHour = pTime[nIndex] - '0' + wHour * 10;
    nIndex++;
  }
  if (bSymbol) {
    if (nIndex < nLen && pTime[nIndex] != ':')
      return FALSE;
    nIndex++;
  }
  nStart = nIndex;
  while (nIndex - nStart < wCountM && nIndex < nLen && pTime[nIndex]) {
    if (!FXSYS_isDecimalDigit(pTime[nIndex]))
      return FALSE;
    wMinute = pTime[nIndex] - '0' + wMinute * 10;
    nIndex++;
  }
  if (bSymbol) {
    if (nIndex < nLen && pTime[nIndex] != ':')
      return FALSE;
    nIndex++;
  }
  nStart = nIndex;
  while (nIndex - nStart < wCountS && nIndex < nLen && pTime[nIndex]) {
    if (!FXSYS_isDecimalDigit(pTime[nIndex]))
      return FALSE;
    wSecond = pTime[nIndex] - '0' + wSecond * 10;
    nIndex++;
  }
  if (wsTime.Find('.') > 0) {
    if (pTime[nIndex] != '.')
      return FALSE;
    nIndex++;
    nStart = nIndex;
    while (nIndex - nStart < wCountF && nIndex < nLen && pTime[nIndex]) {
      if (!FXSYS_isDecimalDigit(pTime[nIndex]))
        return FALSE;
      wFraction = pTime[nIndex] - '0' + wFraction * 10;
      nIndex++;
    }
  }
  if (nIndex < nLen) {
    if (pTime[nIndex] == 'Z') {
      nIndex++;
    } else if (pTime[nIndex] == '-' || pTime[nIndex] == '+') {
      int16_t nOffsetH = 0;
      int16_t nOffsetM = 0;
      nIndex++;
      nStart = nIndex;
      while (nIndex - nStart < wCountH && nIndex < nLen && pTime[nIndex]) {
        if (!FXSYS_isDecimalDigit(pTime[nIndex]))
          return FALSE;
        nOffsetH = pTime[nIndex] - '0' + nOffsetH * 10;
        nIndex++;
      }
      if (bSymbol) {
        if (nIndex < nLen && pTime[nIndex] != ':')
          return FALSE;
        nIndex++;
      }
      nStart = nIndex;
      while (nIndex - nStart < wCountM && nIndex < nLen && pTime[nIndex]) {
        if (!FXSYS_isDecimalDigit(pTime[nIndex]))
          return FALSE;
        nOffsetM = pTime[nIndex] - '0' + nOffsetM * 10;
        nIndex++;
      }
      if (nOffsetH > 12 || nOffsetM >= 60)
        return FALSE;
    }
  }
  return nIndex == nLen && wHour < 24 && wMinute < 60 && wSecond < 60 &&
         wFraction <= 999;
}
FX_BOOL CXFA_LocaleValue::ValidateCanonicalDateTime(
    const CFX_WideString& wsDateTime) {
  CFX_WideString wsDate, wsTime;
  if (wsDateTime.IsEmpty()) {
    return FALSE;
  }
  int nSplitIndex = -1;
  nSplitIndex = wsDateTime.Find('T');
  if (nSplitIndex < 0) {
    nSplitIndex = wsDateTime.Find(' ');
  }
  if (nSplitIndex < 0) {
    return FALSE;
  }
  wsDate = wsDateTime.Left(nSplitIndex);
  wsTime = wsDateTime.Right(wsDateTime.GetLength() - nSplitIndex - 1);
  CFX_Unitime dt;
  return ValidateCanonicalDate(wsDate, dt) && ValidateCanonicalTime(wsTime);
}
FX_BOOL CXFA_LocaleValue::ParsePatternValue(const CFX_WideString& wsValue,
                                            const CFX_WideString& wsPattern,
                                            IFX_Locale* pLocale) {
  IFX_Locale* locale = m_pLocaleMgr->GetDefLocale();
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(pLocale);
  }

  CFX_FormatString* pFormat = nullptr;
  if (m_pLocaleMgr)
    pFormat = new CFX_FormatString(m_pLocaleMgr, FALSE);

  CFX_WideStringArray wsPatterns;
  pFormat->SplitFormatString(wsPattern, wsPatterns);
  FX_BOOL bRet = FALSE;
  int32_t iCount = wsPatterns.GetSize();
  for (int32_t i = 0; i < iCount && !bRet; i++) {
    CFX_WideString wsFormat = wsPatterns[i];
    FX_LOCALECATEGORY eCategory = pFormat->GetCategory(wsFormat);
    eCategory = XFA_ValugeCategory(eCategory, m_dwType);
    switch (eCategory) {
      case FX_LOCALECATEGORY_Null:
        bRet = pFormat->ParseNull(wsValue, wsFormat);
        if (bRet) {
          m_wsValue.clear();
        }
        break;
      case FX_LOCALECATEGORY_Zero:
        bRet = pFormat->ParseZero(wsValue, wsFormat);
        if (bRet) {
          m_wsValue = FX_WSTRC(L"0");
        }
        break;
      case FX_LOCALECATEGORY_Num: {
        CFX_WideString fNum;
        bRet = pFormat->ParseNum(wsValue, wsFormat, fNum);
        if (bRet) {
          m_wsValue = fNum;
        }
        break;
      }
      case FX_LOCALECATEGORY_Text:
        bRet = pFormat->ParseText(wsValue, wsFormat, m_wsValue);
        break;
      case FX_LOCALECATEGORY_Date: {
        CFX_Unitime dt;
        bRet = ValidateCanonicalDate(wsValue, dt);
        if (!bRet) {
          bRet = pFormat->ParseDateTime(wsValue, wsFormat, FX_DATETIMETYPE_Date,
                                        dt);
        }
        if (bRet) {
          SetDate(dt);
        }
        break;
      }
      case FX_LOCALECATEGORY_Time: {
        CFX_Unitime dt;
        bRet =
            pFormat->ParseDateTime(wsValue, wsFormat, FX_DATETIMETYPE_Time, dt);
        if (bRet) {
          SetTime(dt);
        }
        break;
      }
      case FX_LOCALECATEGORY_DateTime: {
        CFX_Unitime dt;
        bRet = pFormat->ParseDateTime(wsValue, wsFormat,
                                      FX_DATETIMETYPE_DateTime, dt);
        if (bRet) {
          SetDateTime(dt);
        }
        break;
      }
      default:
        m_wsValue = wsValue;
        bRet = TRUE;
        break;
    }
  }
  if (!bRet) {
    m_wsValue = wsValue;
  }
  pFormat->Release();
  if (pLocale) {
    m_pLocaleMgr->SetDefLocale(locale);
  }
  return bRet;
}
void CXFA_LocaleValue::GetNumbericFormat(CFX_WideString& wsFormat,
                                         int32_t nIntLen,
                                         int32_t nDecLen,
                                         FX_BOOL bSign) {
  ASSERT(wsFormat.IsEmpty());
  ASSERT(nIntLen >= -1 && nDecLen >= -1);
  int32_t nTotalLen = (nIntLen >= 0 ? nIntLen : 2) + (bSign ? 1 : 0) +
                      (nDecLen >= 0 ? nDecLen : 2) + (nDecLen == 0 ? 0 : 1);
  FX_WCHAR* lpBuf = wsFormat.GetBuffer(nTotalLen);
  int32_t nPos = 0;
  if (bSign) {
    lpBuf[nPos++] = L's';
  }
  if (nIntLen == -1) {
    lpBuf[nPos++] = L'z';
    lpBuf[nPos++] = L'*';
  } else {
    while (nIntLen) {
      lpBuf[nPos++] = L'z';
      nIntLen--;
    }
  }
  if (nDecLen != 0) {
    lpBuf[nPos++] = L'.';
  }
  if (nDecLen == -1) {
    lpBuf[nPos++] = L'z';
    lpBuf[nPos++] = L'*';
  } else {
    while (nDecLen) {
      lpBuf[nPos++] = L'z';
      nDecLen--;
    }
  }
  wsFormat.ReleaseBuffer(nTotalLen);
}
FX_BOOL CXFA_LocaleValue::ValidateNumericTemp(CFX_WideString& wsNumeric,
                                              CFX_WideString& wsFormat,
                                              IFX_Locale* pLocale,
                                              int32_t* pos) {
  if (wsFormat.IsEmpty() || wsNumeric.IsEmpty()) {
    return TRUE;
  }
  const FX_WCHAR* pNum = wsNumeric.c_str();
  const FX_WCHAR* pFmt = wsFormat.c_str();
  int32_t n = 0, nf = 0;
  FX_WCHAR c = pNum[n];
  FX_WCHAR cf = pFmt[nf];
  if (cf == L's') {
    if (c == L'-' || c == L'+') {
      ++n;
    }
    ++nf;
  }
  FX_BOOL bLimit = TRUE;
  int32_t nCount = wsNumeric.GetLength();
  int32_t nCountFmt = wsFormat.GetLength();
  while (n < nCount && (bLimit ? nf < nCountFmt : TRUE) &&
         FXSYS_isDecimalDigit(c = pNum[n])) {
    if (bLimit == TRUE) {
      if ((cf = pFmt[nf]) == L'*') {
        bLimit = FALSE;
      } else if (cf == L'z') {
        nf++;
      } else {
        return FALSE;
      }
    }
    n++;
  }
  if (n == nCount) {
    return TRUE;
  }
  if (nf == nCountFmt) {
    return FALSE;
  }
  while (nf < nCountFmt && (cf = pFmt[nf]) != L'.') {
    ASSERT(cf == L'z' || cf == L'*');
    ++nf;
  }
  CFX_WideString wsDecimalSymbol;
  if (pLocale) {
    pLocale->GetNumbericSymbol(FX_LOCALENUMSYMBOL_Decimal, wsDecimalSymbol);
  } else {
    wsDecimalSymbol = CFX_WideString(L'.');
  }
  if (pFmt[nf] != L'.') {
    return FALSE;
  }
  if (wsDecimalSymbol != CFX_WideStringC(c) && c != L'.') {
    return FALSE;
  }
  ++nf;
  ++n;
  bLimit = TRUE;
  while (n < nCount && (bLimit ? nf < nCountFmt : TRUE) &&
         FXSYS_isDecimalDigit(c = pNum[n])) {
    if (bLimit == TRUE) {
      if ((cf = pFmt[nf]) == L'*') {
        bLimit = FALSE;
      } else if (cf == L'z') {
        nf++;
      } else {
        return FALSE;
      }
    }
    n++;
  }
  return n == nCount;
}
