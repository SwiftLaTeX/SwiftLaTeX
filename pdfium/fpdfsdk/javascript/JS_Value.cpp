// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/JS_Value.h"

#include <time.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "fpdfsdk/javascript/Document.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_Object.h"

static const uint32_t g_nan[2] = {0, 0x7FF80000};
static double GetNan() {
  return *(double*)g_nan;
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime) : m_pJSRuntime(pRuntime) {}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, v8::Local<v8::Value> pValue)
    : m_pValue(pValue), m_pJSRuntime(pRuntime) {}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const int& iValue)
    : m_pJSRuntime(pRuntime) {
  operator=(iValue);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const bool& bValue)
    : m_pJSRuntime(pRuntime) {
  operator=(bValue);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const float& fValue)
    : m_pJSRuntime(pRuntime) {
  operator=(fValue);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const double& dValue)
    : m_pJSRuntime(pRuntime) {
  operator=(dValue);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, CJS_Object* pJsObj)
    : m_pJSRuntime(pRuntime) {
  operator=(pJsObj);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const FX_WCHAR* pWstr)
    : m_pJSRuntime(pRuntime) {
  operator=(pWstr);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, const FX_CHAR* pStr)
    : m_pJSRuntime(pRuntime) {
  operator=(pStr);
}

CJS_Value::CJS_Value(CJS_Runtime* pRuntime, CJS_Array& array)
    : m_pJSRuntime(pRuntime) {
  operator=(array);
}

CJS_Value::~CJS_Value() {}

CJS_Value::CJS_Value(const CJS_Value& other) = default;

void CJS_Value::Attach(v8::Local<v8::Value> pValue) {
  m_pValue = pValue;
}

void CJS_Value::Attach(CJS_Value* pValue) {
  if (pValue)
    Attach(pValue->ToV8Value());
}

void CJS_Value::Detach() {
  m_pValue = v8::Local<v8::Value>();
}

int CJS_Value::ToInt() const {
  return FXJS_ToInt32(m_pJSRuntime->GetIsolate(), m_pValue);
}

bool CJS_Value::ToBool() const {
  return FXJS_ToBoolean(m_pJSRuntime->GetIsolate(), m_pValue);
}

double CJS_Value::ToDouble() const {
  return FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pValue);
}

float CJS_Value::ToFloat() const {
  return (float)ToDouble();
}

CJS_Object* CJS_Value::ToCJSObject() const {
  v8::Local<v8::Object> pObj =
      FXJS_ToObject(m_pJSRuntime->GetIsolate(), m_pValue);
  return (CJS_Object*)FXJS_GetPrivate(m_pJSRuntime->GetIsolate(), pObj);
}

v8::Local<v8::Object> CJS_Value::ToV8Object() const {
  return FXJS_ToObject(m_pJSRuntime->GetIsolate(), m_pValue);
}

CFX_WideString CJS_Value::ToCFXWideString() const {
  return FXJS_ToString(m_pJSRuntime->GetIsolate(), m_pValue);
}

CFX_ByteString CJS_Value::ToCFXByteString() const {
  return CFX_ByteString::FromUnicode(ToCFXWideString());
}

v8::Local<v8::Value> CJS_Value::ToV8Value() const {
  return m_pValue;
}

v8::Local<v8::Array> CJS_Value::ToV8Array() const {
  if (IsArrayObject())
    return v8::Local<v8::Array>::Cast(
        FXJS_ToObject(m_pJSRuntime->GetIsolate(), m_pValue));
  return v8::Local<v8::Array>();
}

void CJS_Value::MaybeCoerceToNumber() {
  bool bAllowNaN = false;
  if (GetType() == VT_string) {
    CFX_ByteString bstr = ToCFXByteString();
    if (bstr.GetLength() == 0)
      return;
    if (bstr == "NaN")
      bAllowNaN = true;
  }
  v8::TryCatch try_catch(m_pJSRuntime->GetIsolate());
  v8::MaybeLocal<v8::Number> maybeNum =
      m_pValue->ToNumber(m_pJSRuntime->GetIsolate()->GetCurrentContext());
  if (maybeNum.IsEmpty())
    return;
  v8::Local<v8::Number> num = maybeNum.ToLocalChecked();
  if (std::isnan(num->Value()) && !bAllowNaN)
    return;
  m_pValue = num;
}

void CJS_Value::operator=(int iValue) {
  m_pValue = FXJS_NewNumber(m_pJSRuntime->GetIsolate(), iValue);
}

void CJS_Value::operator=(bool bValue) {
  m_pValue = FXJS_NewBoolean(m_pJSRuntime->GetIsolate(), bValue);
}

void CJS_Value::operator=(double dValue) {
  m_pValue = FXJS_NewNumber(m_pJSRuntime->GetIsolate(), dValue);
}

void CJS_Value::operator=(float fValue) {
  m_pValue = FXJS_NewNumber(m_pJSRuntime->GetIsolate(), fValue);
}

void CJS_Value::operator=(v8::Local<v8::Object> pObj) {
  m_pValue = pObj;
}

void CJS_Value::operator=(CJS_Object* pObj) {
  if (pObj)
    operator=(pObj->ToV8Object());
}

void CJS_Value::operator=(const FX_WCHAR* pWstr) {
  m_pValue = FXJS_NewString(m_pJSRuntime->GetIsolate(), (wchar_t*)pWstr);
}

void CJS_Value::SetNull() {
  m_pValue = FXJS_NewNull(m_pJSRuntime->GetIsolate());
}

void CJS_Value::operator=(const FX_CHAR* pStr) {
  operator=(CFX_WideString::FromLocal(pStr).c_str());
}

void CJS_Value::operator=(CJS_Array& array) {
  m_pValue = static_cast<v8::Local<v8::Array>>(array);
}

void CJS_Value::operator=(CJS_Date& date) {
  m_pValue = FXJS_NewDate(m_pJSRuntime->GetIsolate(), (double)date);
}

void CJS_Value::operator=(CJS_Value value) {
  m_pValue = value.ToV8Value();
  m_pJSRuntime = value.m_pJSRuntime;
}

// static
CJS_Value::Type CJS_Value::GetValueType(v8::Local<v8::Value> value) {
  if (value.IsEmpty())
    return VT_unknown;
  if (value->IsString())
    return VT_string;
  if (value->IsNumber())
    return VT_number;
  if (value->IsBoolean())
    return VT_boolean;
  if (value->IsDate())
    return VT_date;
  if (value->IsObject())
    return VT_object;
  if (value->IsNull())
    return VT_null;
  if (value->IsUndefined())
    return VT_undefined;
  return VT_unknown;
}

FX_BOOL CJS_Value::IsArrayObject() const {
  if (m_pValue.IsEmpty())
    return FALSE;
  return m_pValue->IsArray();
}

FX_BOOL CJS_Value::IsDateObject() const {
  if (m_pValue.IsEmpty())
    return FALSE;
  return m_pValue->IsDate();
}

// CJS_Value::operator CJS_Array()
FX_BOOL CJS_Value::ConvertToArray(CJS_Array& array) const {
  if (IsArrayObject()) {
    array.Attach(FXJS_ToArray(m_pJSRuntime->GetIsolate(), m_pValue));
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CJS_Value::ConvertToDate(CJS_Date& date) const {
  if (IsDateObject()) {
    date.Attach(m_pValue);
    return TRUE;
  }

  return FALSE;
}

CJS_PropValue::CJS_PropValue(const CJS_Value& value)
    : CJS_Value(value), m_bIsSetting(0) {}

CJS_PropValue::CJS_PropValue(CJS_Runtime* pRuntime)
    : CJS_Value(pRuntime), m_bIsSetting(0) {}

CJS_PropValue::~CJS_PropValue() {}

void CJS_PropValue::operator<<(int iValue) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(iValue);
}

void CJS_PropValue::operator>>(int& iValue) const {
  ASSERT(m_bIsSetting);
  iValue = CJS_Value::ToInt();
}

void CJS_PropValue::operator<<(bool bValue) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(bValue);
}

void CJS_PropValue::operator>>(bool& bValue) const {
  ASSERT(m_bIsSetting);
  bValue = CJS_Value::ToBool();
}

void CJS_PropValue::operator<<(double dValue) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(dValue);
}

void CJS_PropValue::operator>>(double& dValue) const {
  ASSERT(m_bIsSetting);
  dValue = CJS_Value::ToDouble();
}

void CJS_PropValue::operator<<(CJS_Object* pObj) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(pObj);
}

void CJS_PropValue::operator>>(CJS_Object*& ppObj) const {
  ASSERT(m_bIsSetting);
  ppObj = CJS_Value::ToCJSObject();
}

void CJS_PropValue::operator<<(CJS_Document* pJsDoc) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(pJsDoc);
}

void CJS_PropValue::operator>>(CJS_Document*& ppJsDoc) const {
  ASSERT(m_bIsSetting);
  ppJsDoc = static_cast<CJS_Document*>(CJS_Value::ToCJSObject());
}

void CJS_PropValue::operator<<(v8::Local<v8::Object> pObj) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(pObj);
}

void CJS_PropValue::operator>>(v8::Local<v8::Object>& ppObj) const {
  ASSERT(m_bIsSetting);
  ppObj = CJS_Value::ToV8Object();
}

void CJS_PropValue::StartSetting() {
  m_bIsSetting = 1;
}

void CJS_PropValue::StartGetting() {
  m_bIsSetting = 0;
}
void CJS_PropValue::operator<<(CFX_ByteString str) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(str.c_str());
}

void CJS_PropValue::operator>>(CFX_ByteString& str) const {
  ASSERT(m_bIsSetting);
  str = CJS_Value::ToCFXByteString();
}

void CJS_PropValue::operator<<(const FX_WCHAR* c_string) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(c_string);
}

void CJS_PropValue::operator>>(CFX_WideString& wide_string) const {
  ASSERT(m_bIsSetting);
  wide_string = CJS_Value::ToCFXWideString();
}

void CJS_PropValue::operator<<(CFX_WideString wide_string) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(wide_string.c_str());
}

void CJS_PropValue::operator>>(CJS_Array& array) const {
  ASSERT(m_bIsSetting);
  ConvertToArray(array);
}

void CJS_PropValue::operator<<(CJS_Array& array) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(array);
}

void CJS_PropValue::operator>>(CJS_Date& date) const {
  ASSERT(m_bIsSetting);
  ConvertToDate(date);
}

void CJS_PropValue::operator<<(CJS_Date& date) {
  ASSERT(!m_bIsSetting);
  CJS_Value::operator=(date);
}

CJS_PropValue::operator v8::Local<v8::Value>() const {
  return m_pValue;
}

CJS_Array::CJS_Array(CJS_Runtime* pRuntime) : m_pJSRuntime(pRuntime) {}

CJS_Array::~CJS_Array() {}

CJS_Array::CJS_Array(const CJS_Array& other) = default;

void CJS_Array::Attach(v8::Local<v8::Array> pArray) {
  m_pArray = pArray;
}

FX_BOOL CJS_Array::IsAttached() {
  return FALSE;
}

void CJS_Array::GetElement(unsigned index, CJS_Value& value) {
  if (m_pArray.IsEmpty())
    return;
  v8::Local<v8::Value> p =
      FXJS_GetArrayElement(m_pJSRuntime->GetIsolate(), m_pArray, index);
  value.Attach(p);
}

void CJS_Array::SetElement(unsigned index, CJS_Value value) {
  if (m_pArray.IsEmpty())
    m_pArray = FXJS_NewArray(m_pJSRuntime->GetIsolate());

  FXJS_PutArrayElement(m_pJSRuntime->GetIsolate(), m_pArray, index,
                       value.ToV8Value());
}

int CJS_Array::GetLength() {
  if (m_pArray.IsEmpty())
    return 0;
  return FXJS_GetArrayLength(m_pArray);
}

CJS_Array::operator v8::Local<v8::Array>() {
  if (m_pArray.IsEmpty())
    m_pArray = FXJS_NewArray(m_pJSRuntime->GetIsolate());

  return m_pArray;
}

CJS_Date::CJS_Date(CJS_Runtime* pRuntime) : m_pJSRuntime(pRuntime) {}

CJS_Date::CJS_Date(CJS_Runtime* pRuntime, double dMsecTime)
    : m_pJSRuntime(pRuntime) {
  m_pDate = FXJS_NewDate(pRuntime->GetIsolate(), dMsecTime);
}

CJS_Date::CJS_Date(CJS_Runtime* pRuntime,
                   int year,
                   int mon,
                   int day,
                   int hour,
                   int min,
                   int sec)
    : m_pJSRuntime(pRuntime) {
  m_pDate = FXJS_NewDate(pRuntime->GetIsolate(),
                         MakeDate(year, mon, day, hour, min, sec, 0));
}

double CJS_Date::MakeDate(int year,
                          int mon,
                          int day,
                          int hour,
                          int min,
                          int sec,
                          int ms) {
  return JS_MakeDate(JS_MakeDay(year, mon, day),
                     JS_MakeTime(hour, min, sec, ms));
}

CJS_Date::~CJS_Date() {}

FX_BOOL CJS_Date::IsValidDate() {
  if (m_pDate.IsEmpty())
    return FALSE;
  return !JS_PortIsNan(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate));
}

void CJS_Date::Attach(v8::Local<v8::Value> pDate) {
  m_pDate = pDate;
}

int CJS_Date::GetYear() {
  if (IsValidDate())
    return JS_GetYearFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetYear(int iYear) {
  double date = MakeDate(iYear, GetMonth(), GetDay(), GetHours(), GetMinutes(),
                         GetSeconds(), 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

int CJS_Date::GetMonth() {
  if (IsValidDate())
    return JS_GetMonthFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetMonth(int iMonth) {
  double date = MakeDate(GetYear(), iMonth, GetDay(), GetHours(), GetMinutes(),
                         GetSeconds(), 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

int CJS_Date::GetDay() {
  if (IsValidDate())
    return JS_GetDayFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetDay(int iDay) {
  double date = MakeDate(GetYear(), GetMonth(), iDay, GetHours(), GetMinutes(),
                         GetSeconds(), 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

int CJS_Date::GetHours() {
  if (IsValidDate())
    return JS_GetHourFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetHours(int iHours) {
  double date = MakeDate(GetYear(), GetMonth(), GetDay(), iHours, GetMinutes(),
                         GetSeconds(), 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

int CJS_Date::GetMinutes() {
  if (IsValidDate())
    return JS_GetMinFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetMinutes(int minutes) {
  double date = MakeDate(GetYear(), GetMonth(), GetDay(), GetHours(), minutes,
                         GetSeconds(), 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

int CJS_Date::GetSeconds() {
  if (IsValidDate())
    return JS_GetSecFromTime(
        JS_LocalTime(FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate)));

  return 0;
}

void CJS_Date::SetSeconds(int seconds) {
  double date = MakeDate(GetYear(), GetMonth(), GetDay(), GetHours(),
                         GetMinutes(), seconds, 0);
  FXJS_ValueCopy(m_pDate, FXJS_NewDate(m_pJSRuntime->GetIsolate(), date));
}

CJS_Date::operator v8::Local<v8::Value>() {
  return m_pDate;
}

CJS_Date::operator double() const {
  if (m_pDate.IsEmpty())
    return 0.0;
  return FXJS_ToNumber(m_pJSRuntime->GetIsolate(), m_pDate);
}

CFX_WideString CJS_Date::ToString() const {
  if (m_pDate.IsEmpty())
    return L"";
  return FXJS_ToString(m_pJSRuntime->GetIsolate(), m_pDate);
}

double _getLocalTZA() {
  if (!FSDK_IsSandBoxPolicyEnabled(FPDF_POLICY_MACHINETIME_ACCESS))
    return 0;
  time_t t = 0;
  time(&t);
  localtime(&t);
#if _MSC_VER >= 1900
  // In gcc and in Visual Studio prior to VS 2015 'timezone' is a global
  // variable declared in time.h. That variable was deprecated and in VS 2015
  // is removed, with _get_timezone replacing it.
  long timezone = 0;
  _get_timezone(&timezone);
#endif
  return (double)(-(timezone * 1000));
}

int _getDaylightSavingTA(double d) {
  if (!FSDK_IsSandBoxPolicyEnabled(FPDF_POLICY_MACHINETIME_ACCESS))
    return 0;
  time_t t = (time_t)(d / 1000);
  struct tm* tmp = localtime(&t);
  if (!tmp)
    return 0;
  if (tmp->tm_isdst > 0)
    // One hour.
    return (int)60 * 60 * 1000;
  return 0;
}

double _Mod(double x, double y) {
  double r = fmod(x, y);
  if (r < 0)
    r += y;
  return r;
}

int _isfinite(double v) {
#if _MSC_VER
  return ::_finite(v);
#else
  return std::fabs(v) < std::numeric_limits<double>::max();
#endif
}

double _toInteger(double n) {
  return (n >= 0) ? FXSYS_floor(n) : -FXSYS_floor(-n);
}

bool _isLeapYear(int year) {
  return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 != 0));
}

int _DayFromYear(int y) {
  return (int)(365 * (y - 1970.0) + FXSYS_floor((y - 1969.0) / 4) -
               FXSYS_floor((y - 1901.0) / 100) +
               FXSYS_floor((y - 1601.0) / 400));
}

double _TimeFromYear(int y) {
  return 86400000.0 * _DayFromYear(y);
}

static const uint16_t daysMonth[12] = {0,   31,  59,  90,  120, 151,
                                       181, 212, 243, 273, 304, 334};
static const uint16_t leapDaysMonth[12] = {0,   31,  60,  91,  121, 152,
                                           182, 213, 244, 274, 305, 335};

double _TimeFromYearMonth(int y, int m) {
  const uint16_t* pMonth = _isLeapYear(y) ? leapDaysMonth : daysMonth;
  return _TimeFromYear(y) + ((double)pMonth[m]) * 86400000;
}

int _Day(double t) {
  return (int)FXSYS_floor(t / 86400000);
}

int _YearFromTime(double t) {
  // estimate the time.
  int y = 1970 + static_cast<int>(t / (365.2425 * 86400000));
  if (_TimeFromYear(y) <= t) {
    while (_TimeFromYear(y + 1) <= t)
      y++;
  } else {
    while (_TimeFromYear(y) > t)
      y--;
  }
  return y;
}

int _DayWithinYear(double t) {
  int year = _YearFromTime(t);
  int day = _Day(t);
  return day - _DayFromYear(year);
}

int _MonthFromTime(double t) {
  int day = _DayWithinYear(t);
  int year = _YearFromTime(t);
  if (0 <= day && day < 31)
    return 0;
  if (31 <= day && day < 59 + _isLeapYear(year))
    return 1;
  if ((59 + _isLeapYear(year)) <= day && day < (90 + _isLeapYear(year)))
    return 2;
  if ((90 + _isLeapYear(year)) <= day && day < (120 + _isLeapYear(year)))
    return 3;
  if ((120 + _isLeapYear(year)) <= day && day < (151 + _isLeapYear(year)))
    return 4;
  if ((151 + _isLeapYear(year)) <= day && day < (181 + _isLeapYear(year)))
    return 5;
  if ((181 + _isLeapYear(year)) <= day && day < (212 + _isLeapYear(year)))
    return 6;
  if ((212 + _isLeapYear(year)) <= day && day < (243 + _isLeapYear(year)))
    return 7;
  if ((243 + _isLeapYear(year)) <= day && day < (273 + _isLeapYear(year)))
    return 8;
  if ((273 + _isLeapYear(year)) <= day && day < (304 + _isLeapYear(year)))
    return 9;
  if ((304 + _isLeapYear(year)) <= day && day < (334 + _isLeapYear(year)))
    return 10;
  if ((334 + _isLeapYear(year)) <= day && day < (365 + _isLeapYear(year)))
    return 11;

  return -1;
}

int _DateFromTime(double t) {
  int day = _DayWithinYear(t);
  int year = _YearFromTime(t);
  int leap = _isLeapYear(year);
  int month = _MonthFromTime(t);
  switch (month) {
    case 0:
      return day + 1;
    case 1:
      return day - 30;
    case 2:
      return day - 58 - leap;
    case 3:
      return day - 89 - leap;
    case 4:
      return day - 119 - leap;
    case 5:
      return day - 150 - leap;
    case 6:
      return day - 180 - leap;
    case 7:
      return day - 211 - leap;
    case 8:
      return day - 242 - leap;
    case 9:
      return day - 272 - leap;
    case 10:
      return day - 303 - leap;
    case 11:
      return day - 333 - leap;
    default:
      return 0;
  }
}

double JS_GetDateTime() {
  if (!FSDK_IsSandBoxPolicyEnabled(FPDF_POLICY_MACHINETIME_ACCESS))
    return 0;
  time_t t = time(nullptr);
  struct tm* pTm = localtime(&t);

  int year = pTm->tm_year + 1900;
  double t1 = _TimeFromYear(year);

  return t1 + pTm->tm_yday * 86400000.0 + pTm->tm_hour * 3600000.0 +
         pTm->tm_min * 60000.0 + pTm->tm_sec * 1000.0;
}

int JS_GetYearFromTime(double dt) {
  return _YearFromTime(dt);
}

int JS_GetMonthFromTime(double dt) {
  return _MonthFromTime(dt);
}

int JS_GetDayFromTime(double dt) {
  return _DateFromTime(dt);
}

int JS_GetHourFromTime(double dt) {
  return (int)_Mod(floor(dt / (60 * 60 * 1000)), 24);
}

int JS_GetMinFromTime(double dt) {
  return (int)_Mod(floor(dt / (60 * 1000)), 60);
}

int JS_GetSecFromTime(double dt) {
  return (int)_Mod(floor(dt / 1000), 60);
}

double JS_DateParse(const CFX_WideString& str) {
  v8::Isolate* pIsolate = v8::Isolate::GetCurrent();
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope scope(pIsolate);

  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();

  // Use the built-in object method.
  v8::Local<v8::Value> v =
      context->Global()
          ->Get(context, v8::String::NewFromUtf8(pIsolate, "Date",
                                                 v8::NewStringType::kNormal)
                             .ToLocalChecked())
          .ToLocalChecked();
  if (v->IsObject()) {
    v8::Local<v8::Object> o = v->ToObject(context).ToLocalChecked();
    v = o->Get(context, v8::String::NewFromUtf8(pIsolate, "parse",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked())
            .ToLocalChecked();
    if (v->IsFunction()) {
      v8::Local<v8::Function> funC = v8::Local<v8::Function>::Cast(v);
      const int argc = 1;
      v8::Local<v8::String> timeStr = FXJS_WSToJSString(pIsolate, str);
      v8::Local<v8::Value> argv[argc] = {timeStr};
      v = funC->Call(context, context->Global(), argc, argv).ToLocalChecked();
      if (v->IsNumber()) {
        double date = v->ToNumber(context).ToLocalChecked()->Value();
        if (!_isfinite(date))
          return date;
        return JS_LocalTime(date);
      }
    }
  }
  return 0;
}

double JS_MakeDay(int nYear, int nMonth, int nDate) {
  if (!_isfinite(nYear) || !_isfinite(nMonth) || !_isfinite(nDate))
    return GetNan();
  double y = _toInteger(nYear);
  double m = _toInteger(nMonth);
  double dt = _toInteger(nDate);
  double ym = y + FXSYS_floor((double)m / 12);
  double mn = _Mod(m, 12);

  double t = _TimeFromYearMonth((int)ym, (int)mn);

  if (_YearFromTime(t) != ym || _MonthFromTime(t) != mn ||
      _DateFromTime(t) != 1)
    return GetNan();
  return _Day(t) + dt - 1;
}

double JS_MakeTime(int nHour, int nMin, int nSec, int nMs) {
  if (!_isfinite(nHour) || !_isfinite(nMin) || !_isfinite(nSec) ||
      !_isfinite(nMs))
    return GetNan();

  double h = _toInteger(nHour);
  double m = _toInteger(nMin);
  double s = _toInteger(nSec);
  double milli = _toInteger(nMs);

  return h * 3600000 + m * 60000 + s * 1000 + milli;
}

double JS_MakeDate(double day, double time) {
  if (!_isfinite(day) || !_isfinite(time))
    return GetNan();

  return day * 86400000 + time;
}

bool JS_PortIsNan(double d) {
  return d != d;
}

double JS_LocalTime(double d) {
  return d + _getLocalTZA() + _getDaylightSavingTA(d);
}

std::vector<CJS_Value> JS_ExpandKeywordParams(
    CJS_Runtime* pRuntime,
    const std::vector<CJS_Value>& originals,
    size_t nKeywords,
    ...) {
  ASSERT(nKeywords);

  std::vector<CJS_Value> result(nKeywords, CJS_Value(pRuntime));
  size_t size = std::min(originals.size(), nKeywords);
  for (size_t i = 0; i < size; ++i)
    result[i] = originals[i];

  if (originals.size() != 1 || originals[0].GetType() != CJS_Value::VT_object ||
      originals[0].IsArrayObject()) {
    return result;
  }
  v8::Local<v8::Object> pObj = originals[0].ToV8Object();
  result[0] = CJS_Value(pRuntime);  // Make unknown.

  va_list ap;
  va_start(ap, nKeywords);
  for (size_t i = 0; i < nKeywords; ++i) {
    const wchar_t* property = va_arg(ap, const wchar_t*);
    v8::Local<v8::Value> v8Value =
        FXJS_GetObjectElement(pRuntime->GetIsolate(), pObj, property);
    if (!v8Value->IsUndefined())
      result[i] = CJS_Value(pRuntime, v8Value);
  }
  va_end(ap);
  return result;
}
