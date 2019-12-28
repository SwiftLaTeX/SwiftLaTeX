// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/PublicMethods.h"

#include <algorithm>
#include <vector>

#include "core/fxcrt/include/fx_ext.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/Field.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"
#include "fpdfsdk/javascript/cjs_runtime.h"
#include "fpdfsdk/javascript/color.h"
#include "fpdfsdk/javascript/resource.h"
#include "fpdfsdk/javascript/util.h"

#define DOUBLE_CORRECT 0.000000000000001

BEGIN_JS_STATIC_GLOBAL_FUN(CJS_PublicMethods)
JS_STATIC_GLOBAL_FUN_ENTRY(AFNumber_Format)
JS_STATIC_GLOBAL_FUN_ENTRY(AFNumber_Keystroke)
JS_STATIC_GLOBAL_FUN_ENTRY(AFPercent_Format)
JS_STATIC_GLOBAL_FUN_ENTRY(AFPercent_Keystroke)
JS_STATIC_GLOBAL_FUN_ENTRY(AFDate_FormatEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFDate_KeystrokeEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFDate_Format)
JS_STATIC_GLOBAL_FUN_ENTRY(AFDate_Keystroke)
JS_STATIC_GLOBAL_FUN_ENTRY(AFTime_FormatEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFTime_KeystrokeEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFTime_Format)
JS_STATIC_GLOBAL_FUN_ENTRY(AFTime_Keystroke)
JS_STATIC_GLOBAL_FUN_ENTRY(AFSpecial_Format)
JS_STATIC_GLOBAL_FUN_ENTRY(AFSpecial_Keystroke)
JS_STATIC_GLOBAL_FUN_ENTRY(AFSpecial_KeystrokeEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFSimple)
JS_STATIC_GLOBAL_FUN_ENTRY(AFMakeNumber)
JS_STATIC_GLOBAL_FUN_ENTRY(AFSimple_Calculate)
JS_STATIC_GLOBAL_FUN_ENTRY(AFRange_Validate)
JS_STATIC_GLOBAL_FUN_ENTRY(AFMergeChange)
JS_STATIC_GLOBAL_FUN_ENTRY(AFParseDateEx)
JS_STATIC_GLOBAL_FUN_ENTRY(AFExtractNums)
END_JS_STATIC_GLOBAL_FUN()

IMPLEMENT_JS_STATIC_GLOBAL_FUN(CJS_PublicMethods)

namespace {

const FX_WCHAR* const months[] = {L"Jan", L"Feb", L"Mar", L"Apr",
                                  L"May", L"Jun", L"Jul", L"Aug",
                                  L"Sep", L"Oct", L"Nov", L"Dec"};

const FX_WCHAR* const fullmonths[] = {L"January", L"February", L"March",
                                      L"April",   L"May",      L"June",
                                      L"July",    L"August",   L"September",
                                      L"October", L"November", L"December"};

CFX_ByteString StrTrim(const CFX_ByteString& pStr) {
  CFX_ByteString result(pStr);
  result.TrimLeft(' ');
  result.TrimRight(' ');
  return result;
}

CFX_WideString StrTrim(const CFX_WideString& pStr) {
  CFX_WideString result(pStr);
  result.TrimLeft(' ');
  result.TrimRight(' ');
  return result;
}

}  // namespace

bool CJS_PublicMethods::IsNumber(const CFX_WideString& str) {
  CFX_WideString sTrim = StrTrim(str);
  const FX_WCHAR* pTrim = sTrim.c_str();
  const FX_WCHAR* p = pTrim;
  bool bDot = false;
  bool bKXJS = false;

  wchar_t c;
  while ((c = *p) != L'\0') {
    if (c == L'.' || c == L',') {
      if (bDot)
        return false;
      bDot = true;
    } else if (c == L'-' || c == L'+') {
      if (p != pTrim)
        return false;
    } else if (c == L'e' || c == L'E') {
      if (bKXJS)
        return false;

      p++;
      c = *p;
      if (c == L'+' || c == L'-') {
        bKXJS = true;
      } else {
        return false;
      }
    } else if (!FXSYS_iswdigit(c)) {
      return false;
    }
    p++;
  }

  return true;
}

bool CJS_PublicMethods::maskSatisfied(wchar_t c_Change, wchar_t c_Mask) {
  switch (c_Mask) {
    case L'9':
      return FXSYS_iswdigit(c_Change);
    case L'A':
      return FXSYS_iswalpha(c_Change);
    case L'O':
      return FXSYS_iswalnum(c_Change);
    case L'X':
      return true;
    default:
      return (c_Change == c_Mask);
  }
}

bool CJS_PublicMethods::isReservedMaskChar(wchar_t ch) {
  return ch == L'9' || ch == L'A' || ch == L'O' || ch == L'X';
}

double CJS_PublicMethods::AF_Simple(const FX_WCHAR* sFuction,
                                    double dValue1,
                                    double dValue2) {
  if (FXSYS_wcsicmp(sFuction, L"AVG") == 0 ||
      FXSYS_wcsicmp(sFuction, L"SUM") == 0) {
    return dValue1 + dValue2;
  }
  if (FXSYS_wcsicmp(sFuction, L"PRD") == 0) {
    return dValue1 * dValue2;
  }
  if (FXSYS_wcsicmp(sFuction, L"MIN") == 0) {
    return std::min(dValue1, dValue2);
  }
  if (FXSYS_wcsicmp(sFuction, L"MAX") == 0) {
    return std::max(dValue1, dValue2);
  }
  return dValue1;
}

CJS_Array CJS_PublicMethods::AF_MakeArrayFromList(CJS_Runtime* pRuntime,
                                                  CJS_Value val) {
  CJS_Array StrArray(pRuntime);
  if (val.IsArrayObject()) {
    val.ConvertToArray(StrArray);
    return StrArray;
  }
  CFX_WideString wsStr = val.ToCFXWideString();
  CFX_ByteString t = CFX_ByteString::FromUnicode(wsStr);
  const char* p = t.c_str();

  int ch = ',';
  int nIndex = 0;

  while (*p) {
    const char* pTemp = strchr(p, ch);
    if (!pTemp) {
      StrArray.SetElement(
          nIndex, CJS_Value(pRuntime, StrTrim(CFX_ByteString(p)).c_str()));
      break;
    }

    char* pSub = new char[pTemp - p + 1];
    strncpy(pSub, p, pTemp - p);
    *(pSub + (pTemp - p)) = '\0';

    StrArray.SetElement(
        nIndex, CJS_Value(pRuntime, StrTrim(CFX_ByteString(pSub)).c_str()));
    delete[] pSub;

    nIndex++;
    p = ++pTemp;
  }
  return StrArray;
}

int CJS_PublicMethods::ParseStringInteger(const CFX_WideString& str,
                                          int nStart,
                                          int& nSkip,
                                          int nMaxStep) {
  int nRet = 0;
  nSkip = 0;
  for (int i = nStart, sz = str.GetLength(); i < sz; i++) {
    if (i - nStart > 10)
      break;

    FX_WCHAR c = str.GetAt(i);
    if (!FXSYS_iswdigit(c))
      break;

    nRet = nRet * 10 + FXSYS_toDecimalDigit(c);
    nSkip = i - nStart + 1;
    if (nSkip >= nMaxStep)
      break;
  }

  return nRet;
}

CFX_WideString CJS_PublicMethods::ParseStringString(const CFX_WideString& str,
                                                    int nStart,
                                                    int& nSkip) {
  CFX_WideString swRet;
  nSkip = 0;
  for (int i = nStart, sz = str.GetLength(); i < sz; i++) {
    FX_WCHAR c = str.GetAt(i);
    if (!FXSYS_iswdigit(c))
      break;

    swRet += c;
    nSkip = i - nStart + 1;
  }

  return swRet;
}

double CJS_PublicMethods::ParseNormalDate(const CFX_WideString& value,
                                          bool* bWrongFormat) {
  double dt = JS_GetDateTime();

  int nYear = JS_GetYearFromTime(dt);
  int nMonth = JS_GetMonthFromTime(dt) + 1;
  int nDay = JS_GetDayFromTime(dt);
  int nHour = JS_GetHourFromTime(dt);
  int nMin = JS_GetMinFromTime(dt);
  int nSec = JS_GetSecFromTime(dt);

  int number[3];

  int nSkip = 0;
  int nLen = value.GetLength();
  int nIndex = 0;
  int i = 0;
  while (i < nLen) {
    if (nIndex > 2)
      break;

    FX_WCHAR c = value.GetAt(i);
    if (FXSYS_iswdigit(c)) {
      number[nIndex++] = ParseStringInteger(value, i, nSkip, 4);
      i += nSkip;
    } else {
      i++;
    }
  }

  if (nIndex == 2) {
    // case2: month/day
    // case3: day/month
    if ((number[0] >= 1 && number[0] <= 12) &&
        (number[1] >= 1 && number[1] <= 31)) {
      nMonth = number[0];
      nDay = number[1];
    } else if ((number[0] >= 1 && number[0] <= 31) &&
               (number[1] >= 1 && number[1] <= 12)) {
      nDay = number[0];
      nMonth = number[1];
    }

    if (bWrongFormat)
      *bWrongFormat = false;
  } else if (nIndex == 3) {
    // case1: year/month/day
    // case2: month/day/year
    // case3: day/month/year

    if (number[0] > 12 && (number[1] >= 1 && number[1] <= 12) &&
        (number[2] >= 1 && number[2] <= 31)) {
      nYear = number[0];
      nMonth = number[1];
      nDay = number[2];
    } else if ((number[0] >= 1 && number[0] <= 12) &&
               (number[1] >= 1 && number[1] <= 31) && number[2] > 31) {
      nMonth = number[0];
      nDay = number[1];
      nYear = number[2];
    } else if ((number[0] >= 1 && number[0] <= 31) &&
               (number[1] >= 1 && number[1] <= 12) && number[2] > 31) {
      nDay = number[0];
      nMonth = number[1];
      nYear = number[2];
    }

    if (bWrongFormat)
      *bWrongFormat = false;
  } else {
    if (bWrongFormat)
      *bWrongFormat = true;
    return dt;
  }

  CFX_WideString swTemp;
  swTemp.Format(L"%d/%d/%d %d:%d:%d", nMonth, nDay, nYear, nHour, nMin, nSec);
  return JS_DateParse(swTemp);
}

double CJS_PublicMethods::MakeRegularDate(const CFX_WideString& value,
                                          const CFX_WideString& format,
                                          bool* bWrongFormat) {
  double dt = JS_GetDateTime();

  if (format.IsEmpty() || value.IsEmpty())
    return dt;

  int nYear = JS_GetYearFromTime(dt);
  int nMonth = JS_GetMonthFromTime(dt) + 1;
  int nDay = JS_GetDayFromTime(dt);
  int nHour = JS_GetHourFromTime(dt);
  int nMin = JS_GetMinFromTime(dt);
  int nSec = JS_GetSecFromTime(dt);

  int nYearSub = 99;  // nYear - 2000;

  FX_BOOL bPm = FALSE;
  FX_BOOL bExit = FALSE;
  bool bBadFormat = false;

  int i = 0;
  int j = 0;

  while (i < format.GetLength()) {
    if (bExit)
      break;

    FX_WCHAR c = format.GetAt(i);
    switch (c) {
      case ':':
      case '.':
      case '-':
      case '\\':
      case '/':
        i++;
        j++;
        break;

      case 'y':
      case 'm':
      case 'd':
      case 'H':
      case 'h':
      case 'M':
      case 's':
      case 't': {
        int oldj = j;
        int nSkip = 0;
        int remaining = format.GetLength() - i - 1;

        if (remaining == 0 || format.GetAt(i + 1) != c) {
          switch (c) {
            case 'y':
              i++;
              j++;
              break;
            case 'm':
              nMonth = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 'd':
              nDay = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 'H':
              nHour = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 'h':
              nHour = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 'M':
              nMin = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 's':
              nSec = ParseStringInteger(value, j, nSkip, 2);
              i++;
              j += nSkip;
              break;
            case 't':
              bPm = (j < value.GetLength() && value.GetAt(j) == 'p');
              i++;
              j++;
              break;
          }
        } else if (remaining == 1 || format.GetAt(i + 2) != c) {
          switch (c) {
            case 'y':
              nYear = ParseStringInteger(value, j, nSkip, 4);
              i += 2;
              j += nSkip;
              break;
            case 'm':
              nMonth = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 'd':
              nDay = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 'H':
              nHour = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 'h':
              nHour = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 'M':
              nMin = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 's':
              nSec = ParseStringInteger(value, j, nSkip, 2);
              i += 2;
              j += nSkip;
              break;
            case 't':
              bPm = (j + 1 < value.GetLength() && value.GetAt(j) == 'p' &&
                     value.GetAt(j + 1) == 'm');
              i += 2;
              j += 2;
              break;
          }
        } else if (remaining == 2 || format.GetAt(i + 3) != c) {
          switch (c) {
            case 'm': {
              CFX_WideString sMonth = ParseStringString(value, j, nSkip);
              FX_BOOL bFind = FALSE;
              for (int m = 0; m < 12; m++) {
                if (sMonth.CompareNoCase(months[m]) == 0) {
                  nMonth = m + 1;
                  i += 3;
                  j += nSkip;
                  bFind = TRUE;
                  break;
                }
              }

              if (!bFind) {
                nMonth = ParseStringInteger(value, j, nSkip, 3);
                i += 3;
                j += nSkip;
              }
            } break;
            case 'y':
              break;
            default:
              i += 3;
              j += 3;
              break;
          }
        } else if (remaining == 3 || format.GetAt(i + 4) != c) {
          switch (c) {
            case 'y':
              nYear = ParseStringInteger(value, j, nSkip, 4);
              j += nSkip;
              i += 4;
              break;
            case 'm': {
              FX_BOOL bFind = FALSE;

              CFX_WideString sMonth = ParseStringString(value, j, nSkip);
              sMonth.MakeLower();

              for (int m = 0; m < 12; m++) {
                CFX_WideString sFullMonths = fullmonths[m];
                sFullMonths.MakeLower();

                if (sFullMonths.Find(sMonth.c_str(), 0) != -1) {
                  nMonth = m + 1;
                  i += 4;
                  j += nSkip;
                  bFind = TRUE;
                  break;
                }
              }

              if (!bFind) {
                nMonth = ParseStringInteger(value, j, nSkip, 4);
                i += 4;
                j += nSkip;
              }
            } break;
            default:
              i += 4;
              j += 4;
              break;
          }
        } else {
          if (j >= value.GetLength() || format.GetAt(i) != value.GetAt(j)) {
            bBadFormat = true;
            bExit = TRUE;
          }
          i++;
          j++;
        }

        if (oldj == j) {
          bBadFormat = true;
          bExit = TRUE;
        }
      }

      break;
      default:
        if (value.GetLength() <= j) {
          bExit = TRUE;
        } else if (format.GetAt(i) != value.GetAt(j)) {
          bBadFormat = true;
          bExit = TRUE;
        }

        i++;
        j++;
        break;
    }
  }

  if (bPm)
    nHour += 12;

  if (nYear >= 0 && nYear <= nYearSub)
    nYear += 2000;

  if (nMonth < 1 || nMonth > 12)
    bBadFormat = true;

  if (nDay < 1 || nDay > 31)
    bBadFormat = true;

  if (nHour < 0 || nHour > 24)
    bBadFormat = true;

  if (nMin < 0 || nMin > 60)
    bBadFormat = true;

  if (nSec < 0 || nSec > 60)
    bBadFormat = true;

  double dRet = 0;
  if (bBadFormat) {
    dRet = ParseNormalDate(value, &bBadFormat);
  } else {
    dRet = JS_MakeDate(JS_MakeDay(nYear, nMonth - 1, nDay),
                       JS_MakeTime(nHour, nMin, nSec, 0));
    if (JS_PortIsNan(dRet))
      dRet = JS_DateParse(value);
  }

  if (JS_PortIsNan(dRet))
    dRet = ParseNormalDate(value, &bBadFormat);

  if (bWrongFormat)
    *bWrongFormat = bBadFormat;

  return dRet;
}

CFX_WideString CJS_PublicMethods::MakeFormatDate(double dDate,
                                                 const CFX_WideString& format) {
  CFX_WideString sRet = L"", sPart = L"";

  int nYear = JS_GetYearFromTime(dDate);
  int nMonth = JS_GetMonthFromTime(dDate) + 1;
  int nDay = JS_GetDayFromTime(dDate);
  int nHour = JS_GetHourFromTime(dDate);
  int nMin = JS_GetMinFromTime(dDate);
  int nSec = JS_GetSecFromTime(dDate);

  int i = 0;
  while (i < format.GetLength()) {
    FX_WCHAR c = format.GetAt(i);
    int remaining = format.GetLength() - i - 1;
    sPart = L"";
    switch (c) {
      case 'y':
      case 'm':
      case 'd':
      case 'H':
      case 'h':
      case 'M':
      case 's':
      case 't':
        if (remaining == 0 || format.GetAt(i + 1) != c) {
          switch (c) {
            case 'y':
              sPart += c;
              break;
            case 'm':
              sPart.Format(L"%d", nMonth);
              break;
            case 'd':
              sPart.Format(L"%d", nDay);
              break;
            case 'H':
              sPart.Format(L"%d", nHour);
              break;
            case 'h':
              sPart.Format(L"%d", nHour > 12 ? nHour - 12 : nHour);
              break;
            case 'M':
              sPart.Format(L"%d", nMin);
              break;
            case 's':
              sPart.Format(L"%d", nSec);
              break;
            case 't':
              sPart += nHour > 12 ? 'p' : 'a';
              break;
          }
          i++;
        } else if (remaining == 1 || format.GetAt(i + 2) != c) {
          switch (c) {
            case 'y':
              sPart.Format(L"%02d", nYear - (nYear / 100) * 100);
              break;
            case 'm':
              sPart.Format(L"%02d", nMonth);
              break;
            case 'd':
              sPart.Format(L"%02d", nDay);
              break;
            case 'H':
              sPart.Format(L"%02d", nHour);
              break;
            case 'h':
              sPart.Format(L"%02d", nHour > 12 ? nHour - 12 : nHour);
              break;
            case 'M':
              sPart.Format(L"%02d", nMin);
              break;
            case 's':
              sPart.Format(L"%02d", nSec);
              break;
            case 't':
              sPart = nHour > 12 ? L"pm" : L"am";
              break;
          }
          i += 2;
        } else if (remaining == 2 || format.GetAt(i + 3) != c) {
          switch (c) {
            case 'm':
              i += 3;
              if (nMonth > 0 && nMonth <= 12)
                sPart += months[nMonth - 1];
              break;
            default:
              i += 3;
              sPart += c;
              sPart += c;
              sPart += c;
              break;
          }
        } else if (remaining == 3 || format.GetAt(i + 4) != c) {
          switch (c) {
            case 'y':
              sPart.Format(L"%04d", nYear);
              i += 4;
              break;
            case 'm':
              i += 4;
              if (nMonth > 0 && nMonth <= 12)
                sPart += fullmonths[nMonth - 1];
              break;
            default:
              i += 4;
              sPart += c;
              sPart += c;
              sPart += c;
              sPart += c;
              break;
          }
        } else {
          i++;
          sPart += c;
        }
        break;
      default:
        i++;
        sPart += c;
        break;
    }

    sRet += sPart;
  }

  return sRet;
}

// function AFNumber_Format(nDec, sepStyle, negStyle, currStyle, strCurrency,
// bCurrencyPrepend)
FX_BOOL CJS_PublicMethods::AFNumber_Format(IJS_Context* cc,
                                           const std::vector<CJS_Value>& params,
                                           CJS_Value& vRet,
                                           CFX_WideString& sError) {
#if _FX_OS_ != _FX_ANDROID_
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 6) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_EventHandler* pEvent = pContext->GetEventHandler();
  if (!pEvent->m_pValue)
    return FALSE;

  CFX_WideString& Value = pEvent->Value();
  CFX_ByteString strValue = StrTrim(CFX_ByteString::FromUnicode(Value));
  if (strValue.IsEmpty())
    return TRUE;

  int iDec = params[0].ToInt();
  int iSepStyle = params[1].ToInt();
  int iNegStyle = params[2].ToInt();
  // params[3] is iCurrStyle, it's not used.
  CFX_WideString wstrCurrency = params[4].ToCFXWideString();
  FX_BOOL bCurrencyPrepend = params[5].ToBool();

  if (iDec < 0)
    iDec = -iDec;

  if (iSepStyle < 0 || iSepStyle > 3)
    iSepStyle = 0;

  if (iNegStyle < 0 || iNegStyle > 3)
    iNegStyle = 0;

  // for processing decimal places
  strValue.Replace(",", ".");
  double dValue = atof(strValue.c_str());
  if (iDec > 0)
    dValue += DOUBLE_CORRECT;

  int iDec2;
  int iNegative = 0;

  strValue = fcvt(dValue, iDec, &iDec2, &iNegative);
  if (strValue.IsEmpty()) {
    dValue = 0;
    strValue = fcvt(dValue, iDec, &iDec2, &iNegative);
    if (strValue.IsEmpty()) {
      strValue = "0";
      iDec2 = 1;
    }
  }

  if (iDec2 < 0) {
    for (int iNum = 0; iNum < abs(iDec2); iNum++) {
      strValue = "0" + strValue;
    }
    iDec2 = 0;
  }
  int iMax = strValue.GetLength();
  if (iDec2 > iMax) {
    for (int iNum = 0; iNum <= iDec2 - iMax; iNum++) {
      strValue += "0";
    }
    iMax = iDec2 + 1;
  }

  // for processing seperator style
  if (iDec2 < iMax) {
    if (iSepStyle == 0 || iSepStyle == 1) {
      strValue.Insert(iDec2, '.');
      iMax++;
    } else if (iSepStyle == 2 || iSepStyle == 3) {
      strValue.Insert(iDec2, ',');
      iMax++;
    }

    if (iDec2 == 0)
      strValue.Insert(iDec2, '0');
  }
  if (iSepStyle == 0 || iSepStyle == 2) {
    char cSeperator;
    if (iSepStyle == 0)
      cSeperator = ',';
    else
      cSeperator = '.';

    for (int iDecPositive = iDec2 - 3; iDecPositive > 0; iDecPositive -= 3) {
      strValue.Insert(iDecPositive, cSeperator);
      iMax++;
    }
  }

  // for processing currency string
  Value = CFX_WideString::FromLocal(strValue.AsStringC());

  if (bCurrencyPrepend)
    Value = wstrCurrency + Value;
  else
    Value = Value + wstrCurrency;

  // for processing negative style
  if (iNegative) {
    if (iNegStyle == 0) {
      Value = L"-" + Value;
    } else if (iNegStyle == 2 || iNegStyle == 3) {
      Value = L"(" + Value + L")";
    }
    if (iNegStyle == 1 || iNegStyle == 3) {
      if (Field* fTarget = pEvent->Target_Field()) {
        CJS_Array arColor(pRuntime);
        CJS_Value vColElm(pRuntime);
        vColElm = L"RGB";
        arColor.SetElement(0, vColElm);
        vColElm = 1;
        arColor.SetElement(1, vColElm);
        vColElm = 0;
        arColor.SetElement(2, vColElm);

        arColor.SetElement(3, vColElm);

        CJS_PropValue vProp(pRuntime);
        vProp.StartGetting();
        vProp << arColor;
        vProp.StartSetting();
        fTarget->textColor(cc, vProp, sError);  // red
      }
    }
  } else {
    if (iNegStyle == 1 || iNegStyle == 3) {
      if (Field* fTarget = pEvent->Target_Field()) {
        CJS_Array arColor(pRuntime);
        CJS_Value vColElm(pRuntime);
        vColElm = L"RGB";
        arColor.SetElement(0, vColElm);
        vColElm = 0;
        arColor.SetElement(1, vColElm);
        arColor.SetElement(2, vColElm);
        arColor.SetElement(3, vColElm);

        CJS_PropValue vProp(pRuntime);
        vProp.StartGetting();
        fTarget->textColor(cc, vProp, sError);

        CJS_Array aProp(pRuntime);
        vProp.ConvertToArray(aProp);

        CPWL_Color crProp;
        CPWL_Color crColor;
        color::ConvertArrayToPWLColor(aProp, crProp);
        color::ConvertArrayToPWLColor(arColor, crColor);

        if (crColor != crProp) {
          CJS_PropValue vProp2(pRuntime);
          vProp2.StartGetting();
          vProp2 << arColor;
          vProp2.StartSetting();
          fTarget->textColor(cc, vProp2, sError);
        }
      }
    }
  }
#endif
  return TRUE;
}

// function AFNumber_Keystroke(nDec, sepStyle, negStyle, currStyle, strCurrency,
// bCurrencyPrepend)
FX_BOOL CJS_PublicMethods::AFNumber_Keystroke(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() < 2)
    return FALSE;

  if (!pEvent->m_pValue)
    return FALSE;

  CFX_WideString& val = pEvent->Value();
  CFX_WideString& wstrChange = pEvent->Change();
  CFX_WideString wstrValue = val;

  if (pEvent->WillCommit()) {
    CFX_WideString swTemp = StrTrim(wstrValue);
    if (swTemp.IsEmpty())
      return TRUE;

    swTemp.Replace(L",", L".");
    if (!IsNumber(swTemp.c_str())) {
      pEvent->Rc() = FALSE;
      sError = JSGetStringFromID(pContext, IDS_STRING_JSAFNUMBER_KEYSTROKE);
      Alert(pContext, sError.c_str());
    }
    return TRUE;  // it happens after the last keystroke and before validating,
  }

  CFX_WideString wstrSelected;
  if (pEvent->SelStart() != -1) {
    wstrSelected = wstrValue.Mid(pEvent->SelStart(),
                                 pEvent->SelEnd() - pEvent->SelStart());
  }

  bool bHasSign = wstrValue.Find(L'-') != -1 && wstrSelected.Find(L'-') == -1;
  if (bHasSign) {
    // can't insert "change" in front to sign postion.
    if (pEvent->SelStart() == 0) {
      FX_BOOL& bRc = pEvent->Rc();
      bRc = FALSE;
      return TRUE;
    }
  }

  int iSepStyle = params[1].ToInt();
  if (iSepStyle < 0 || iSepStyle > 3)
    iSepStyle = 0;
  const FX_WCHAR cSep = iSepStyle < 2 ? L'.' : L',';

  bool bHasSep = wstrValue.Find(cSep) != -1;
  for (FX_STRSIZE i = 0; i < wstrChange.GetLength(); ++i) {
    if (wstrChange[i] == cSep) {
      if (bHasSep) {
        FX_BOOL& bRc = pEvent->Rc();
        bRc = FALSE;
        return TRUE;
      }
      bHasSep = TRUE;
      continue;
    }
    if (wstrChange[i] == L'-') {
      if (bHasSign) {
        FX_BOOL& bRc = pEvent->Rc();
        bRc = FALSE;
        return TRUE;
      }
      // sign's position is not correct
      if (i != 0) {
        FX_BOOL& bRc = pEvent->Rc();
        bRc = FALSE;
        return TRUE;
      }
      if (pEvent->SelStart() != 0) {
        FX_BOOL& bRc = pEvent->Rc();
        bRc = FALSE;
        return TRUE;
      }
      bHasSign = TRUE;
      continue;
    }

    if (!FXSYS_iswdigit(wstrChange[i])) {
      FX_BOOL& bRc = pEvent->Rc();
      bRc = FALSE;
      return TRUE;
    }
  }

  CFX_WideString wprefix = wstrValue.Mid(0, pEvent->SelStart());
  CFX_WideString wpostfix;
  if (pEvent->SelEnd() < wstrValue.GetLength())
    wpostfix = wstrValue.Mid(pEvent->SelEnd());
  val = wprefix + wstrChange + wpostfix;
  return TRUE;
}

// function AFPercent_Format(nDec, sepStyle)
FX_BOOL CJS_PublicMethods::AFPercent_Format(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
#if _FX_OS_ != _FX_ANDROID_
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() != 2) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }
  if (!pEvent->m_pValue)
    return FALSE;

  CFX_WideString& Value = pEvent->Value();
  CFX_ByteString strValue = StrTrim(CFX_ByteString::FromUnicode(Value));
  if (strValue.IsEmpty())
    return TRUE;

  int iDec = params[0].ToInt();
  if (iDec < 0)
    iDec = -iDec;

  int iSepStyle = params[1].ToInt();
  if (iSepStyle < 0 || iSepStyle > 3)
    iSepStyle = 0;

  // for processing decimal places
  double dValue = atof(strValue.c_str());
  dValue *= 100;
  if (iDec > 0)
    dValue += DOUBLE_CORRECT;

  int iDec2;
  int iNegative = 0;
  strValue = fcvt(dValue, iDec, &iDec2, &iNegative);
  if (strValue.IsEmpty()) {
    dValue = 0;
    strValue = fcvt(dValue, iDec, &iDec2, &iNegative);
  }

  if (iDec2 < 0) {
    for (int iNum = 0; iNum < abs(iDec2); iNum++) {
      strValue = "0" + strValue;
    }
    iDec2 = 0;
  }
  int iMax = strValue.GetLength();
  if (iDec2 > iMax) {
    for (int iNum = 0; iNum <= iDec2 - iMax; iNum++) {
      strValue += "0";
    }
    iMax = iDec2 + 1;
  }

  // for processing seperator style
  if (iDec2 < iMax) {
    if (iSepStyle == 0 || iSepStyle == 1) {
      strValue.Insert(iDec2, '.');
      iMax++;
    } else if (iSepStyle == 2 || iSepStyle == 3) {
      strValue.Insert(iDec2, ',');
      iMax++;
    }

    if (iDec2 == 0)
      strValue.Insert(iDec2, '0');
  }
  if (iSepStyle == 0 || iSepStyle == 2) {
    char cSeperator;
    if (iSepStyle == 0)
      cSeperator = ',';
    else
      cSeperator = '.';

    for (int iDecPositive = iDec2 - 3; iDecPositive > 0; iDecPositive -= 3) {
      strValue.Insert(iDecPositive, cSeperator);
      iMax++;
    }
  }

  // negative mark
  if (iNegative)
    strValue = "-" + strValue;
  strValue += "%";
  Value = CFX_WideString::FromLocal(strValue.AsStringC());
#endif
  return TRUE;
}
// AFPercent_Keystroke(nDec, sepStyle)
FX_BOOL CJS_PublicMethods::AFPercent_Keystroke(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  return AFNumber_Keystroke(cc, params, vRet, sError);
}

// function AFDate_FormatEx(cFormat)
FX_BOOL CJS_PublicMethods::AFDate_FormatEx(IJS_Context* cc,
                                           const std::vector<CJS_Value>& params,
                                           CJS_Value& vRet,
                                           CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }
  if (!pEvent->m_pValue)
    return FALSE;

  CFX_WideString& val = pEvent->Value();
  CFX_WideString strValue = val;
  if (strValue.IsEmpty())
    return TRUE;

  CFX_WideString sFormat = params[0].ToCFXWideString();
  double dDate = 0.0f;

  if (strValue.Find(L"GMT") != -1) {
    // for GMT format time
    // such as "Tue Aug 11 14:24:16 GMT+08002009"
    dDate = MakeInterDate(strValue);
  } else {
    dDate = MakeRegularDate(strValue, sFormat, nullptr);
  }

  if (JS_PortIsNan(dDate)) {
    CFX_WideString swMsg;
    swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSPARSEDATE).c_str(),
                 sFormat.c_str());
    Alert(pContext, swMsg.c_str());
    return FALSE;
  }

  val = MakeFormatDate(dDate, sFormat);
  return TRUE;
}

double CJS_PublicMethods::MakeInterDate(const CFX_WideString& strValue) {
  std::vector<CFX_WideString> wsArray;
  CFX_WideString sTemp = L"";
  for (int i = 0; i < strValue.GetLength(); ++i) {
    FX_WCHAR c = strValue.GetAt(i);
    if (c == L' ' || c == L':') {
      wsArray.push_back(sTemp);
      sTemp = L"";
      continue;
    }
    sTemp += c;
  }
  wsArray.push_back(sTemp);
  if (wsArray.size() != 8)
    return 0;

  int nMonth = 1;
  sTemp = wsArray[1];
  if (sTemp.Compare(L"Jan") == 0)
    nMonth = 1;
  else if (sTemp.Compare(L"Feb") == 0)
    nMonth = 2;
  else if (sTemp.Compare(L"Mar") == 0)
    nMonth = 3;
  else if (sTemp.Compare(L"Apr") == 0)
    nMonth = 4;
  else if (sTemp.Compare(L"May") == 0)
    nMonth = 5;
  else if (sTemp.Compare(L"Jun") == 0)
    nMonth = 6;
  else if (sTemp.Compare(L"Jul") == 0)
    nMonth = 7;
  else if (sTemp.Compare(L"Aug") == 0)
    nMonth = 8;
  else if (sTemp.Compare(L"Sep") == 0)
    nMonth = 9;
  else if (sTemp.Compare(L"Oct") == 0)
    nMonth = 10;
  else if (sTemp.Compare(L"Nov") == 0)
    nMonth = 11;
  else if (sTemp.Compare(L"Dec") == 0)
    nMonth = 12;

  int nDay = FX_atof(wsArray[2].AsStringC());
  int nHour = FX_atof(wsArray[3].AsStringC());
  int nMin = FX_atof(wsArray[4].AsStringC());
  int nSec = FX_atof(wsArray[5].AsStringC());
  int nYear = FX_atof(wsArray[7].AsStringC());
  double dRet = JS_MakeDate(JS_MakeDay(nYear, nMonth - 1, nDay),
                            JS_MakeTime(nHour, nMin, nSec, 0));
  if (JS_PortIsNan(dRet))
    dRet = JS_DateParse(strValue);

  return dRet;
}

// AFDate_KeystrokeEx(cFormat)
FX_BOOL CJS_PublicMethods::AFDate_KeystrokeEx(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() != 1) {
    sError = L"AFDate_KeystrokeEx's parameters' size r not correct";
    return FALSE;
  }

  if (pEvent->WillCommit()) {
    if (!pEvent->m_pValue)
      return FALSE;
    CFX_WideString strValue = pEvent->Value();
    if (strValue.IsEmpty())
      return TRUE;

    CFX_WideString sFormat = params[0].ToCFXWideString();
    bool bWrongFormat = FALSE;
    double dRet = MakeRegularDate(strValue, sFormat, &bWrongFormat);
    if (bWrongFormat || JS_PortIsNan(dRet)) {
      CFX_WideString swMsg;
      swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSPARSEDATE).c_str(),
                   sFormat.c_str());
      Alert(pContext, swMsg.c_str());
      pEvent->Rc() = FALSE;
      return TRUE;
    }
  }
  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFDate_Format(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  int iIndex = params[0].ToInt();
  const FX_WCHAR* cFormats[] = {L"m/d",
                                L"m/d/yy",
                                L"mm/dd/yy",
                                L"mm/yy",
                                L"d-mmm",
                                L"d-mmm-yy",
                                L"dd-mmm-yy",
                                L"yy-mm-dd",
                                L"mmm-yy",
                                L"mmmm-yy",
                                L"mmm d, yyyy",
                                L"mmmm d, yyyy",
                                L"m/d/yy h:MM tt",
                                L"m/d/yy HH:MM"};

  if (iIndex < 0 || (static_cast<size_t>(iIndex) >= FX_ArraySize(cFormats)))
    iIndex = 0;

  std::vector<CJS_Value> newParams;
  newParams.push_back(
      CJS_Value(CJS_Runtime::FromContext(cc), cFormats[iIndex]));
  return AFDate_FormatEx(cc, newParams, vRet, sError);
}

// AFDate_KeystrokeEx(cFormat)
FX_BOOL CJS_PublicMethods::AFDate_Keystroke(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  int iIndex = params[0].ToInt();
  const FX_WCHAR* cFormats[] = {L"m/d",
                                L"m/d/yy",
                                L"mm/dd/yy",
                                L"mm/yy",
                                L"d-mmm",
                                L"d-mmm-yy",
                                L"dd-mmm-yy",
                                L"yy-mm-dd",
                                L"mmm-yy",
                                L"mmmm-yy",
                                L"mmm d, yyyy",
                                L"mmmm d, yyyy",
                                L"m/d/yy h:MM tt",
                                L"m/d/yy HH:MM"};

  if (iIndex < 0 || (static_cast<size_t>(iIndex) >= FX_ArraySize(cFormats)))
    iIndex = 0;

  std::vector<CJS_Value> newParams;
  newParams.push_back(
      CJS_Value(CJS_Runtime::FromContext(cc), cFormats[iIndex]));
  return AFDate_KeystrokeEx(cc, newParams, vRet, sError);
}

// function AFTime_Format(ptf)
FX_BOOL CJS_PublicMethods::AFTime_Format(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  int iIndex = params[0].ToInt();
  const FX_WCHAR* cFormats[] = {L"HH:MM", L"h:MM tt", L"HH:MM:ss",
                                L"h:MM:ss tt"};

  if (iIndex < 0 || (static_cast<size_t>(iIndex) >= FX_ArraySize(cFormats)))
    iIndex = 0;

  std::vector<CJS_Value> newParams;
  newParams.push_back(
      CJS_Value(CJS_Runtime::FromContext(cc), cFormats[iIndex]));
  return AFDate_FormatEx(cc, newParams, vRet, sError);
}

FX_BOOL CJS_PublicMethods::AFTime_Keystroke(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  int iIndex = params[0].ToInt();
  const FX_WCHAR* cFormats[] = {L"HH:MM", L"h:MM tt", L"HH:MM:ss",
                                L"h:MM:ss tt"};

  if (iIndex < 0 || (static_cast<size_t>(iIndex) >= FX_ArraySize(cFormats)))
    iIndex = 0;

  std::vector<CJS_Value> newParams;
  newParams.push_back(
      CJS_Value(CJS_Runtime::FromContext(cc), cFormats[iIndex]));
  return AFDate_KeystrokeEx(cc, newParams, vRet, sError);
}

FX_BOOL CJS_PublicMethods::AFTime_FormatEx(IJS_Context* cc,
                                           const std::vector<CJS_Value>& params,
                                           CJS_Value& vRet,
                                           CFX_WideString& sError) {
  return AFDate_FormatEx(cc, params, vRet, sError);
}

FX_BOOL CJS_PublicMethods::AFTime_KeystrokeEx(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  return AFDate_KeystrokeEx(cc, params, vRet, sError);
}

// function AFSpecial_Format(psf)
FX_BOOL CJS_PublicMethods::AFSpecial_Format(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;

  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CJS_EventHandler* pEvent = pContext->GetEventHandler();
  if (!pEvent->m_pValue)
    return FALSE;

  CFX_WideString wsSource = pEvent->Value();
  CFX_WideString wsFormat;
  switch (params[0].ToInt()) {
    case 0:
      wsFormat = L"99999";
      break;
    case 1:
      wsFormat = L"99999-9999";
      break;
    case 2:
      if (util::printx(L"9999999999", wsSource).GetLength() >= 10)
        wsFormat = L"(999) 999-9999";
      else
        wsFormat = L"999-9999";
      break;
    case 3:
      wsFormat = L"999-99-9999";
      break;
  }

  pEvent->Value() = util::printx(wsFormat, wsSource);
  return TRUE;
}

// function AFSpecial_KeystrokeEx(mask)
FX_BOOL CJS_PublicMethods::AFSpecial_KeystrokeEx(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() < 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  if (!pEvent->m_pValue)
    return FALSE;
  CFX_WideString& valEvent = pEvent->Value();

  CFX_WideString wstrMask = params[0].ToCFXWideString();
  if (wstrMask.IsEmpty())
    return TRUE;

  if (pEvent->WillCommit()) {
    if (valEvent.IsEmpty())
      return TRUE;

    FX_STRSIZE iIndexMask = 0;
    for (; iIndexMask < valEvent.GetLength(); ++iIndexMask) {
      if (!maskSatisfied(valEvent[iIndexMask], wstrMask[iIndexMask]))
        break;
    }

    if (iIndexMask != wstrMask.GetLength() ||
        (iIndexMask != valEvent.GetLength() && wstrMask.GetLength() != 0)) {
      Alert(
          pContext,
          JSGetStringFromID(pContext, IDS_STRING_JSAFNUMBER_KEYSTROKE).c_str());
      pEvent->Rc() = FALSE;
    }
    return TRUE;
  }

  CFX_WideString& wideChange = pEvent->Change();
  if (wideChange.IsEmpty())
    return TRUE;

  CFX_WideString wChange = wideChange;
  FX_STRSIZE iIndexMask = pEvent->SelStart();
  FX_STRSIZE combined_len = valEvent.GetLength() + wChange.GetLength() +
                            pEvent->SelStart() - pEvent->SelEnd();
  if (combined_len > wstrMask.GetLength()) {
    Alert(pContext,
          JSGetStringFromID(pContext, IDS_STRING_JSPARAM_TOOLONG).c_str());
    pEvent->Rc() = FALSE;
    return TRUE;
  }

  if (iIndexMask >= wstrMask.GetLength() && !wChange.IsEmpty()) {
    Alert(pContext,
          JSGetStringFromID(pContext, IDS_STRING_JSPARAM_TOOLONG).c_str());
    pEvent->Rc() = FALSE;
    return TRUE;
  }

  for (FX_STRSIZE i = 0; i < wChange.GetLength(); ++i) {
    if (iIndexMask >= wstrMask.GetLength()) {
      Alert(pContext,
            JSGetStringFromID(pContext, IDS_STRING_JSPARAM_TOOLONG).c_str());
      pEvent->Rc() = FALSE;
      return TRUE;
    }
    FX_WCHAR wMask = wstrMask[iIndexMask];
    if (!isReservedMaskChar(wMask))
      wChange.SetAt(i, wMask);

    if (!maskSatisfied(wChange[i], wMask)) {
      pEvent->Rc() = FALSE;
      return TRUE;
    }
    iIndexMask++;
  }
  wideChange = wChange;
  return TRUE;
}

// function AFSpecial_Keystroke(psf)
FX_BOOL CJS_PublicMethods::AFSpecial_Keystroke(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CJS_EventHandler* pEvent = pContext->GetEventHandler();
  if (!pEvent->m_pValue)
    return FALSE;

  const char* cFormat = "";
  switch (params[0].ToInt()) {
    case 0:
      cFormat = "99999";
      break;
    case 1:
      cFormat = "999999999";
      break;
    case 2:
      if (pEvent->Value().GetLength() + pEvent->Change().GetLength() > 7)
        cFormat = "9999999999";
      else
        cFormat = "9999999";
      break;
    case 3:
      cFormat = "999999999";
      break;
  }

  std::vector<CJS_Value> params2;
  params2.push_back(CJS_Value(CJS_Runtime::FromContext(cc), cFormat));
  return AFSpecial_KeystrokeEx(cc, params2, vRet, sError);
}

FX_BOOL CJS_PublicMethods::AFMergeChange(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEventHandler = pContext->GetEventHandler();

  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CFX_WideString swValue;
  if (pEventHandler->m_pValue)
    swValue = pEventHandler->Value();

  if (pEventHandler->WillCommit()) {
    vRet = swValue.c_str();
    return TRUE;
  }

  CFX_WideString prefix, postfix;

  if (pEventHandler->SelStart() >= 0)
    prefix = swValue.Mid(0, pEventHandler->SelStart());
  else
    prefix = L"";

  if (pEventHandler->SelEnd() >= 0 &&
      pEventHandler->SelEnd() <= swValue.GetLength())
    postfix = swValue.Mid(pEventHandler->SelEnd(),
                          swValue.GetLength() - pEventHandler->SelEnd());
  else
    postfix = L"";

  vRet = (prefix + pEventHandler->Change() + postfix).c_str();

  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFParseDateEx(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  ASSERT(pContext);

  if (params.size() != 2) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CFX_WideString sValue = params[0].ToCFXWideString();
  CFX_WideString sFormat = params[1].ToCFXWideString();

  double dDate = MakeRegularDate(sValue, sFormat, nullptr);

  if (JS_PortIsNan(dDate)) {
    CFX_WideString swMsg;
    swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSPARSEDATE).c_str(),
                 sFormat.c_str());
    Alert((CJS_Context*)cc, swMsg.c_str());
    return FALSE;
  }

  vRet = dDate;
  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFSimple(IJS_Context* cc,
                                    const std::vector<CJS_Value>& params,
                                    CJS_Value& vRet,
                                    CFX_WideString& sError) {
  if (params.size() != 3) {
    CJS_Context* pContext = (CJS_Context*)cc;
    ASSERT(pContext);

    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  vRet = (double)AF_Simple(params[0].ToCFXWideString().c_str(),
                           params[1].ToDouble(), params[2].ToDouble());
  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFMakeNumber(IJS_Context* cc,
                                        const std::vector<CJS_Value>& params,
                                        CJS_Value& vRet,
                                        CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }
  CFX_WideString ws = params[0].ToCFXWideString();
  ws.Replace(L",", L".");
  vRet = ws.c_str();
  vRet.MaybeCoerceToNumber();
  if (vRet.GetType() != CJS_Value::VT_number)
    vRet = 0;
  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFSimple_Calculate(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 2) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CJS_Value params1 = params[1];
  if (!params1.IsArrayObject() && params1.GetType() != CJS_Value::VT_string) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CPDFSDK_Document* pReaderDoc = pContext->GetReaderDocument();
  CPDFSDK_InterForm* pReaderInterForm = pReaderDoc->GetInterForm();
  CPDF_InterForm* pInterForm = pReaderInterForm->GetInterForm();

  CFX_WideString sFunction = params[0].ToCFXWideString();
  double dValue = wcscmp(sFunction.c_str(), L"PRD") == 0 ? 1.0 : 0.0;

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array FieldNameArray = AF_MakeArrayFromList(pRuntime, params1);
  int nFieldsCount = 0;

  for (int i = 0, isz = FieldNameArray.GetLength(); i < isz; i++) {
    CJS_Value jsValue(pRuntime);
    FieldNameArray.GetElement(i, jsValue);
    CFX_WideString wsFieldName = jsValue.ToCFXWideString();

    for (int j = 0, jsz = pInterForm->CountFields(wsFieldName); j < jsz; j++) {
      if (CPDF_FormField* pFormField = pInterForm->GetField(j, wsFieldName)) {
        double dTemp = 0.0;
        switch (pFormField->GetFieldType()) {
          case FIELDTYPE_TEXTFIELD:
          case FIELDTYPE_COMBOBOX: {
            CFX_WideString trimmed = pFormField->GetValue();
            trimmed.TrimRight();
            trimmed.TrimLeft();
            dTemp = FX_atof(trimmed.AsStringC());
          } break;
          case FIELDTYPE_PUSHBUTTON: {
            dTemp = 0.0;
          } break;
          case FIELDTYPE_CHECKBOX:
          case FIELDTYPE_RADIOBUTTON: {
            dTemp = 0.0;
            for (int c = 0, csz = pFormField->CountControls(); c < csz; c++) {
              if (CPDF_FormControl* pFormCtrl = pFormField->GetControl(c)) {
                if (pFormCtrl->IsChecked()) {
                  CFX_WideString trimmed = pFormCtrl->GetExportValue();
                  trimmed.TrimRight();
                  trimmed.TrimLeft();
                  dTemp = FX_atof(trimmed.AsStringC());
                  break;
                }
              }
            }
          } break;
          case FIELDTYPE_LISTBOX: {
            if (pFormField->CountSelectedItems() <= 1) {
              CFX_WideString trimmed = pFormField->GetValue();
              trimmed.TrimRight();
              trimmed.TrimLeft();
              dTemp = FX_atof(trimmed.AsStringC());
            }
          } break;
          default:
            break;
        }

        if (i == 0 && j == 0 && (wcscmp(sFunction.c_str(), L"MIN") == 0 ||
                                 wcscmp(sFunction.c_str(), L"MAX") == 0))
          dValue = dTemp;

        dValue = AF_Simple(sFunction.c_str(), dValue, dTemp);

        nFieldsCount++;
      }
    }
  }

  if (wcscmp(sFunction.c_str(), L"AVG") == 0 && nFieldsCount > 0)
    dValue /= nFieldsCount;

  dValue = (double)floor(dValue * FXSYS_pow((double)10, (double)6) + 0.49) /
           FXSYS_pow((double)10, (double)6);
  CJS_Value jsValue(pRuntime, dValue);
  if (pContext->GetEventHandler()->m_pValue)
    pContext->GetEventHandler()->Value() = jsValue.ToCFXWideString();

  return TRUE;
}

/* This function validates the current event to ensure that its value is
** within the specified range. */

FX_BOOL CJS_PublicMethods::AFRange_Validate(
    IJS_Context* cc,
    const std::vector<CJS_Value>& params,
    CJS_Value& vRet,
    CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (params.size() != 4) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  if (!pEvent->m_pValue)
    return FALSE;
  if (pEvent->Value().IsEmpty())
    return TRUE;
  double dEentValue =
      atof(CFX_ByteString::FromUnicode(pEvent->Value()).c_str());
  FX_BOOL bGreaterThan = params[0].ToBool();
  double dGreaterThan = params[1].ToDouble();
  FX_BOOL bLessThan = params[2].ToBool();
  double dLessThan = params[3].ToDouble();
  CFX_WideString swMsg;

  if (bGreaterThan && bLessThan) {
    if (dEentValue < dGreaterThan || dEentValue > dLessThan)
      swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSRANGE1).c_str(),
                   params[1].ToCFXWideString().c_str(),
                   params[3].ToCFXWideString().c_str());
  } else if (bGreaterThan) {
    if (dEentValue < dGreaterThan)
      swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSRANGE2).c_str(),
                   params[1].ToCFXWideString().c_str());
  } else if (bLessThan) {
    if (dEentValue > dLessThan)
      swMsg.Format(JSGetStringFromID(pContext, IDS_STRING_JSRANGE3).c_str(),
                   params[3].ToCFXWideString().c_str());
  }

  if (!swMsg.IsEmpty()) {
    Alert(pContext, swMsg.c_str());
    pEvent->Rc() = FALSE;
  }
  return TRUE;
}

FX_BOOL CJS_PublicMethods::AFExtractNums(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array nums(pRuntime);

  CFX_WideString str = params[0].ToCFXWideString();
  CFX_WideString sPart;

  if (str.GetAt(0) == L'.' || str.GetAt(0) == L',')
    str = L"0" + str;

  int nIndex = 0;
  for (int i = 0, sz = str.GetLength(); i < sz; i++) {
    FX_WCHAR wc = str.GetAt(i);
    if (FXSYS_iswdigit(wc)) {
      sPart += wc;
    } else {
      if (sPart.GetLength() > 0) {
        nums.SetElement(nIndex, CJS_Value(pRuntime, sPart.c_str()));
        sPart = L"";
        nIndex++;
      }
    }
  }

  if (sPart.GetLength() > 0) {
    nums.SetElement(nIndex, CJS_Value(pRuntime, sPart.c_str()));
  }

  if (nums.GetLength() > 0)
    vRet = nums;
  else
    vRet.SetNull();

  return TRUE;
}
