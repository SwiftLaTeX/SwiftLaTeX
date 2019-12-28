// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

// FXJS_V8 is a layer that makes it easier to define native objects in V8, but
// has no knowledge of PDF-specific native objects. It could in theory be used
// to implement other sets of native objects.

// PDFium code should include this file rather than including V8 headers
// directly.

#ifndef FXJS_INCLUDE_FXJS_V8_H_
#define FXJS_INCLUDE_FXJS_V8_H_

#include <v8-util.h>
#include <v8.h>

#include <vector>

#include "core/fxcrt/include/fx_string.h"

class CFXJS_ObjDefinition;

// FXJS_V8 places no restrictions on these two classes; it merely passes them
// on to caller-provided methods.
class IJS_Context;  // A description of the event that caused JS execution.
class IJS_Runtime;  // A native runtime, typically owns the v8::Context.

#ifdef PDF_ENABLE_XFA
// FXJS_V8 places no interpreation on this calass; it merely passes it
// along to XFA.
class CFXJSE_RuntimeData;
#endif  // PDF_ENABLE_XFA

enum FXJSOBJTYPE {
  FXJSOBJTYPE_DYNAMIC = 0,  // Created by native method and returned to JS.
  FXJSOBJTYPE_STATIC,       // Created by init and hung off of global object.
  FXJSOBJTYPE_GLOBAL,       // The global object itself (may only appear once).
};

struct FXJSErr {
  const wchar_t* message;
  const wchar_t* srcline;
  unsigned linnum;
};

// Global weak map to save dynamic objects.
class V8TemplateMapTraits : public v8::StdMapTraits<void*, v8::Object> {
 public:
  typedef v8::GlobalValueMap<void*, v8::Object, V8TemplateMapTraits> MapType;
  typedef void WeakCallbackDataType;

  static WeakCallbackDataType* WeakCallbackParameter(
      MapType* map,
      void* key,
      const v8::Local<v8::Object>& value) {
    return key;
  }
  static MapType* MapFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>&);

  static void* KeyFromWeakCallbackInfo(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
    return data.GetParameter();
  }
  static const v8::PersistentContainerCallbackType kCallbackType =
      v8::kWeakWithInternalFields;
  static void DisposeWeak(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}
  static void OnWeakCallback(
      const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}
  static void Dispose(v8::Isolate* isolate,
                      v8::Global<v8::Object> value,
                      void* key);
  static void DisposeCallbackData(WeakCallbackDataType* callbackData) {}
};

class V8TemplateMap {
 public:
  typedef v8::GlobalValueMap<void*, v8::Object, V8TemplateMapTraits> MapType;

  explicit V8TemplateMap(v8::Isolate* isolate);
  ~V8TemplateMap();

  void set(void* key, v8::Local<v8::Object> handle);

  friend class V8TemplateMapTraits;

 private:
  MapType m_map;
};

class FXJS_PerIsolateData {
 public:
  ~FXJS_PerIsolateData();

  static void SetUp(v8::Isolate* pIsolate);
  static FXJS_PerIsolateData* Get(v8::Isolate* pIsolate);

  void CreateDynamicObjsMap(v8::Isolate* pIsolate) {
    if (!m_pDynamicObjsMap)
      m_pDynamicObjsMap = new V8TemplateMap(pIsolate);
  }
  void ReleaseDynamicObjsMap() {
    delete m_pDynamicObjsMap;
    m_pDynamicObjsMap = nullptr;
  }

  std::vector<CFXJS_ObjDefinition*> m_ObjectDefnArray;
#ifdef PDF_ENABLE_XFA
  CFXJSE_RuntimeData* m_pFXJSERuntimeData;
#endif  // PDF_ENABLE_XFA
  V8TemplateMap* m_pDynamicObjsMap;

 protected:
  FXJS_PerIsolateData();
};

class FXJS_ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  void* Allocate(size_t length) override;
  void* AllocateUninitialized(size_t length) override;
  void Free(void* data, size_t length) override;
};

using FXJS_CONSTRUCTOR = void (*)(IJS_Runtime* cc, v8::Local<v8::Object> obj);
using FXJS_DESTRUCTOR = void (*)(v8::Local<v8::Object> obj);

void FXJS_Initialize(unsigned int embedderDataSlot, v8::Isolate* pIsolate);
void FXJS_Release();

// Gets the global isolate set by FXJS_Initialize(), or makes a new one each
// time if there is no such isolate. Returns true if a new isolate had to be
// created.
bool FXJS_GetIsolate(v8::Isolate** pResultIsolate);

// Get the global isolate's ref count.
size_t FXJS_GlobalIsolateRefCount();

// Always returns a valid, newly-created objDefnID.
int FXJS_DefineObj(v8::Isolate* pIsolate,
                   const wchar_t* sObjName,
                   FXJSOBJTYPE eObjType,
                   FXJS_CONSTRUCTOR pConstructor,
                   FXJS_DESTRUCTOR pDestructor);

void FXJS_DefineObjMethod(v8::Isolate* pIsolate,
                          int nObjDefnID,
                          const wchar_t* sMethodName,
                          v8::FunctionCallback pMethodCall);
void FXJS_DefineObjProperty(v8::Isolate* pIsolate,
                            int nObjDefnID,
                            const wchar_t* sPropName,
                            v8::AccessorGetterCallback pPropGet,
                            v8::AccessorSetterCallback pPropPut);
void FXJS_DefineObjAllProperties(v8::Isolate* pIsolate,
                                 int nObjDefnID,
                                 v8::NamedPropertyQueryCallback pPropQurey,
                                 v8::NamedPropertyGetterCallback pPropGet,
                                 v8::NamedPropertySetterCallback pPropPut,
                                 v8::NamedPropertyDeleterCallback pPropDel);
void FXJS_DefineObjConst(v8::Isolate* pIsolate,
                         int nObjDefnID,
                         const wchar_t* sConstName,
                         v8::Local<v8::Value> pDefault);
void FXJS_DefineGlobalMethod(v8::Isolate* pIsolate,
                             const wchar_t* sMethodName,
                             v8::FunctionCallback pMethodCall);
void FXJS_DefineGlobalConst(v8::Isolate* pIsolate,
                            const wchar_t* sConstName,
                            v8::FunctionCallback pConstGetter);

// Called after FXJS_Define* calls made.
void FXJS_InitializeRuntime(
    v8::Isolate* pIsolate,
    IJS_Runtime* pIRuntime,
    v8::Global<v8::Context>* pV8PersistentContext,
    std::vector<v8::Global<v8::Object>*>* pStaticObjects);
void FXJS_ReleaseRuntime(v8::Isolate* pIsolate,
                         v8::Global<v8::Context>* pV8PersistentContext,
                         std::vector<v8::Global<v8::Object>*>* pStaticObjects);
IJS_Runtime* FXJS_GetRuntimeFromIsolate(v8::Isolate* pIsolate);

#ifdef PDF_ENABLE_XFA
// Called as part of FXJS_InitializeRuntime, exposed so PDF can make its
// own contexts compatible with XFA or vice versa.
void FXJS_SetRuntimeForV8Context(v8::Local<v8::Context> v8Context,
                                 IJS_Runtime* pIRuntime);
#endif  // PDF_ENABLE_XFA

// Called after FXJS_InitializeRuntime call made.
int FXJS_Execute(v8::Isolate* pIsolate,
                 const CFX_WideString& script,
                 FXJSErr* perror);

v8::Local<v8::Object> FXJS_NewFxDynamicObj(v8::Isolate* pIsolate,
                                           IJS_Runtime* pJSContext,
                                           int nObjDefnID,
                                           bool bStatic = false);
v8::Local<v8::Object> FXJS_GetThisObj(v8::Isolate* pIsolate);
int FXJS_GetObjDefnID(v8::Local<v8::Object> pObj);

void FXJS_SetPrivate(v8::Isolate* pIsolate,
                     v8::Local<v8::Object> pObj,
                     void* p);
void* FXJS_GetPrivate(v8::Isolate* pIsolate, v8::Local<v8::Object> pObj);
void FXJS_FreePrivate(void* p);
void FXJS_FreePrivate(v8::Local<v8::Object> pObj);
void FXJS_Error(v8::Isolate* isolate, const CFX_WideString& message);

v8::Local<v8::String> FXJS_WSToJSString(v8::Isolate* pIsolate,
                                        const CFX_WideString& wsPropertyName);
v8::Local<v8::Value> FXJS_GetObjectElement(v8::Isolate* pIsolate,
                                           v8::Local<v8::Object> pObj,
                                           const CFX_WideString& PropertyName);
v8::Local<v8::Array> FXJS_GetObjectElementNames(v8::Isolate* pIsolate,
                                                v8::Local<v8::Object> pObj);
v8::Local<v8::Value> FXJS_GetArrayElement(v8::Isolate* pIsolate,
                                          v8::Local<v8::Array> pArray,
                                          unsigned index);

unsigned FXJS_GetArrayLength(v8::Local<v8::Array> pArray);
void FXJS_PutObjectString(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          const CFX_WideString& wsValue);
void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& PropertyName,
                          int nValue);
void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& PropertyName,
                          float fValue);
void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& PropertyName,
                          double dValue);
void FXJS_PutObjectBoolean(v8::Isolate* pIsolate,
                           v8::Local<v8::Object> pObj,
                           const CFX_WideString& PropertyName,
                           bool bValue);
void FXJS_PutObjectObject(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& PropertyName,
                          v8::Local<v8::Object> pPut);
void FXJS_PutObjectNull(v8::Isolate* pIsolate,
                        v8::Local<v8::Object> pObj,
                        const CFX_WideString& PropertyName);
unsigned FXJS_PutArrayElement(v8::Isolate* pIsolate,
                              v8::Local<v8::Array> pArray,
                              unsigned index,
                              v8::Local<v8::Value> pValue);

v8::Local<v8::Value> FXJS_NewNull(v8::Isolate* pIsolate);
v8::Local<v8::Array> FXJS_NewArray(v8::Isolate* pIsolate);
v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, int number);
v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, double number);
v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, float number);
v8::Local<v8::Value> FXJS_NewBoolean(v8::Isolate* pIsolate, bool b);
v8::Local<v8::Value> FXJS_NewString(v8::Isolate* pIsolate, const wchar_t* str);
v8::Local<v8::Value> FXJS_NewDate(v8::Isolate* pIsolate, double d);

int FXJS_ToInt32(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue);
bool FXJS_ToBoolean(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue);
double FXJS_ToNumber(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue);
v8::Local<v8::Object> FXJS_ToObject(v8::Isolate* pIsolate,
                                    v8::Local<v8::Value> pValue);
CFX_WideString FXJS_ToString(v8::Isolate* pIsolate,
                             v8::Local<v8::Value> pValue);
v8::Local<v8::Array> FXJS_ToArray(v8::Isolate* pIsolate,
                                  v8::Local<v8::Value> pValue);
void FXJS_ValueCopy(v8::Local<v8::Value>& pTo, v8::Local<v8::Value> pFrom);

#endif  // FXJS_INCLUDE_FXJS_V8_H_
