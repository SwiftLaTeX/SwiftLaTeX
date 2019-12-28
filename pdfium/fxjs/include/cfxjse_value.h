// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_INCLUDE_CFXJSE_VALUE_H_
#define FXJS_INCLUDE_CFXJSE_VALUE_H_

#include "v8/include/v8.h"

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "fxjs/cfxjse_isolatetracker.h"
#include "fxjs/cfxjse_runtimedata.h"

class CFXJSE_Class;
class CFXJSE_HostObject;

class CFXJSE_Value {
 public:
  explicit CFXJSE_Value(v8::Isolate* pIsolate);
  ~CFXJSE_Value();

  FX_BOOL IsUndefined() const;
  FX_BOOL IsNull() const;
  FX_BOOL IsBoolean() const;
  FX_BOOL IsString() const;
  FX_BOOL IsNumber() const;
  FX_BOOL IsInteger() const;
  FX_BOOL IsObject() const;
  FX_BOOL IsArray() const;
  FX_BOOL IsFunction() const;
  FX_BOOL IsDate() const;
  FX_BOOL ToBoolean() const;
  FX_FLOAT ToFloat() const;
  double ToDouble() const;
  int32_t ToInteger() const;
  CFX_ByteString ToString() const;
  CFX_WideString ToWideString() const {
    return CFX_WideString::FromUTF8(ToString().AsStringC());
  }
  CFXJSE_HostObject* ToHostObject(CFXJSE_Class* lpClass) const;

  void SetUndefined();
  void SetNull();
  void SetBoolean(FX_BOOL bBoolean);
  void SetInteger(int32_t nInteger);
  void SetDouble(double dDouble);
  void SetString(const CFX_ByteStringC& szString);
  void SetFloat(FX_FLOAT fFloat);
  void SetJSObject();

  void SetObject(CFXJSE_HostObject* lpObject, CFXJSE_Class* pClass);
  void SetHostObject(CFXJSE_HostObject* lpObject, CFXJSE_Class* lpClass);
  void SetArray(uint32_t uValueCount, CFXJSE_Value** rgValues);
  void SetDate(double dDouble);

  FX_BOOL GetObjectProperty(const CFX_ByteStringC& szPropName,
                            CFXJSE_Value* lpPropValue);
  FX_BOOL SetObjectProperty(const CFX_ByteStringC& szPropName,
                            CFXJSE_Value* lpPropValue);
  FX_BOOL GetObjectPropertyByIdx(uint32_t uPropIdx, CFXJSE_Value* lpPropValue);
  FX_BOOL SetObjectProperty(uint32_t uPropIdx, CFXJSE_Value* lpPropValue);
  FX_BOOL DeleteObjectProperty(const CFX_ByteStringC& szPropName);
  FX_BOOL HasObjectOwnProperty(const CFX_ByteStringC& szPropName,
                               FX_BOOL bUseTypeGetter);
  FX_BOOL SetObjectOwnProperty(const CFX_ByteStringC& szPropName,
                               CFXJSE_Value* lpPropValue);
  FX_BOOL SetFunctionBind(CFXJSE_Value* lpOldFunction, CFXJSE_Value* lpNewThis);
  FX_BOOL Call(CFXJSE_Value* lpReceiver,
               CFXJSE_Value* lpRetValue,
               uint32_t nArgCount,
               CFXJSE_Value** lpArgs);

  v8::Isolate* GetIsolate() const { return m_pIsolate; }
  const v8::Global<v8::Value>& DirectGetValue() const { return m_hValue; }
  void ForceSetValue(v8::Local<v8::Value> hValue) {
    m_hValue.Reset(m_pIsolate, hValue);
  }
  void Assign(const CFXJSE_Value* lpValue) {
    ASSERT(lpValue);
    if (lpValue) {
      m_hValue.Reset(m_pIsolate, lpValue->m_hValue);
    } else {
      m_hValue.Reset();
    }
  }

 private:
  friend class CFXJSE_Class;
  friend class CFXJSE_Context;

  CFXJSE_Value();
  CFXJSE_Value(const CFXJSE_Value&);
  CFXJSE_Value& operator=(const CFXJSE_Value&);

  v8::Isolate* m_pIsolate;
  v8::Global<v8::Value> m_hValue;
};

#endif  // FXJS_INCLUDE_CFXJSE_VALUE_H_
