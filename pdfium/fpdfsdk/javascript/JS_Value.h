// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_JS_VALUE_H_
#define FPDFSDK_JAVASCRIPT_JS_VALUE_H_

#include <vector>

#include "core/fxcrt/include/fx_basic.h"
#include "fxjs/include/fxjs_v8.h"

class CJS_Array;
class CJS_Date;
class CJS_Document;
class CJS_Object;
class CJS_Runtime;

class CJS_Value {
 public:
  enum Type {
    VT_unknown,
    VT_string,
    VT_number,
    VT_boolean,
    VT_date,
    VT_object,
    VT_null,
    VT_undefined
  };

  CJS_Value(CJS_Runtime* pRuntime);
  CJS_Value(CJS_Runtime* pRuntime, v8::Local<v8::Value> pValue);
  CJS_Value(CJS_Runtime* pRuntime, const int& iValue);
  CJS_Value(CJS_Runtime* pRuntime, const double& dValue);
  CJS_Value(CJS_Runtime* pRuntime, const float& fValue);
  CJS_Value(CJS_Runtime* pRuntime, const bool& bValue);
  CJS_Value(CJS_Runtime* pRuntime, CJS_Object* pObj);
  CJS_Value(CJS_Runtime* pRuntime, const FX_CHAR* pStr);
  CJS_Value(CJS_Runtime* pRuntime, const FX_WCHAR* pWstr);
  CJS_Value(CJS_Runtime* pRuntime, CJS_Array& array);

  ~CJS_Value();
  CJS_Value(const CJS_Value& other);

  void SetNull();
  void Attach(v8::Local<v8::Value> pValue);
  void Attach(CJS_Value* pValue);
  void Detach();

  static Type GetValueType(v8::Local<v8::Value> value);
  Type GetType() const { return GetValueType(m_pValue); }
  int ToInt() const;
  bool ToBool() const;
  double ToDouble() const;
  float ToFloat() const;
  CJS_Object* ToCJSObject() const;
  CFX_WideString ToCFXWideString() const;
  CFX_ByteString ToCFXByteString() const;
  v8::Local<v8::Object> ToV8Object() const;
  v8::Local<v8::Array> ToV8Array() const;
  v8::Local<v8::Value> ToV8Value() const;

  // Replace the current |m_pValue| with a v8::Number if possible
  // to make one from the current |m_pValue|.
  void MaybeCoerceToNumber();

  void operator=(int iValue);
  void operator=(bool bValue);
  void operator=(double val);
  void operator=(float val);
  void operator=(CJS_Object* val);
  void operator=(v8::Local<v8::Object> val);
  void operator=(CJS_Array& val);
  void operator=(CJS_Date& val);
  void operator=(const FX_WCHAR* pWstr);
  void operator=(const FX_CHAR* pStr);
  void operator=(CJS_Value value);

  FX_BOOL IsArrayObject() const;
  FX_BOOL IsDateObject() const;
  FX_BOOL ConvertToArray(CJS_Array&) const;
  FX_BOOL ConvertToDate(CJS_Date&) const;

  CJS_Runtime* GetJSRuntime() const { return m_pJSRuntime; }

 protected:
  v8::Local<v8::Value> m_pValue;
  CJS_Runtime* m_pJSRuntime;
};

class CJS_PropValue : public CJS_Value {
 public:
  CJS_PropValue(const CJS_Value&);
  CJS_PropValue(CJS_Runtime* pRuntime);
  ~CJS_PropValue();

  FX_BOOL IsSetting() const { return m_bIsSetting; }
  FX_BOOL IsGetting() const { return !m_bIsSetting; }

  void operator<<(int val);
  void operator>>(int&) const;
  void operator<<(bool val);
  void operator>>(bool&) const;
  void operator<<(double val);
  void operator>>(double&) const;
  void operator<<(CJS_Object* pObj);
  void operator>>(CJS_Object*& ppObj) const;
  void operator<<(CJS_Document* pJsDoc);
  void operator>>(CJS_Document*& ppJsDoc) const;
  void operator<<(CFX_ByteString);
  void operator>>(CFX_ByteString&) const;
  void operator<<(CFX_WideString);
  void operator>>(CFX_WideString&) const;
  void operator<<(const FX_WCHAR* c_string);
  void operator<<(v8::Local<v8::Object>);
  void operator>>(v8::Local<v8::Object>&) const;
  void operator>>(CJS_Array& array) const;
  void operator<<(CJS_Array& array);
  void operator<<(CJS_Date& date);
  void operator>>(CJS_Date& date) const;
  operator v8::Local<v8::Value>() const;
  void StartSetting();
  void StartGetting();

 private:
  FX_BOOL m_bIsSetting;
};

class CJS_Array {
 public:
  CJS_Array(CJS_Runtime* pRuntime);
  virtual ~CJS_Array();
  CJS_Array(const CJS_Array& other);

  void Attach(v8::Local<v8::Array> pArray);
  void GetElement(unsigned index, CJS_Value& value);
  void SetElement(unsigned index, CJS_Value value);
  int GetLength();
  FX_BOOL IsAttached();
  operator v8::Local<v8::Array>();

  CJS_Runtime* GetJSRuntime() const { return m_pJSRuntime; }

 private:
  v8::Local<v8::Array> m_pArray;
  CJS_Runtime* m_pJSRuntime;
};

class CJS_Date {
  friend class CJS_Value;

 public:
  CJS_Date(CJS_Runtime* pRuntime);
  CJS_Date(CJS_Runtime* pRuntime, double dMsec_time);
  CJS_Date(CJS_Runtime* pRuntime,
           int year,
           int mon,
           int day,
           int hour,
           int min,
           int sec);
  virtual ~CJS_Date();
  void Attach(v8::Local<v8::Value> pDate);

  int GetYear();
  void SetYear(int iYear);

  int GetMonth();
  void SetMonth(int iMonth);

  int GetDay();
  void SetDay(int iDay);

  int GetHours();
  void SetHours(int iHours);

  int GetMinutes();
  void SetMinutes(int minutes);

  int GetSeconds();
  void SetSeconds(int seconds);

  operator v8::Local<v8::Value>();
  operator double() const;

  CFX_WideString ToString() const;

  static double
  MakeDate(int year, int mon, int mday, int hour, int min, int sec, int ms);

  FX_BOOL IsValidDate();

 protected:
  v8::Local<v8::Value> m_pDate;
  CJS_Runtime* m_pJSRuntime;
};

double JS_GetDateTime();
int JS_GetYearFromTime(double dt);
int JS_GetMonthFromTime(double dt);
int JS_GetDayFromTime(double dt);
int JS_GetHourFromTime(double dt);
int JS_GetMinFromTime(double dt);
int JS_GetSecFromTime(double dt);
double JS_DateParse(const CFX_WideString& str);
double JS_MakeDay(int nYear, int nMonth, int nDay);
double JS_MakeTime(int nHour, int nMin, int nSec, int nMs);
double JS_MakeDate(double day, double time);
bool JS_PortIsNan(double d);
double JS_LocalTime(double d);

// Some JS methods have the bizarre convention that they may also be called
// with a single argument which is an object containing the actual arguments
// as its properties. The varying arguments to this method are the property
// names as wchar_t string literals corresponding to each positional argument.
// The result will always contain |nKeywords| value, with unspecified ones
// being set to type VT_unknown.
std::vector<CJS_Value> JS_ExpandKeywordParams(
    CJS_Runtime* pRuntime,
    const std::vector<CJS_Value>& originals,
    size_t nKeywords,
    ...);

#endif  // FPDFSDK_JAVASCRIPT_JS_VALUE_H_
