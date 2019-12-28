// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/include/fxjs_v8.h"

#include <vector>

#include "core/fxcrt/include/fx_basic.h"

// Keep this consistent with the values defined in gin/public/context_holder.h
// (without actually requiring a dependency on gin itself for the standalone
// embedders of PDFIum). The value we want to use is:
//   kPerContextDataStartIndex + kEmbedderPDFium, which is 3.
static const unsigned int kPerContextDataIndex = 3u;
static unsigned int g_embedderDataSlot = 1u;
static v8::Isolate* g_isolate = nullptr;
static size_t g_isolate_ref_count = 0;
static FXJS_ArrayBufferAllocator* g_arrayBufferAllocator = nullptr;
static v8::Global<v8::ObjectTemplate>* g_DefaultGlobalObjectTemplate = nullptr;

class CFXJS_PerObjectData {
 public:
  explicit CFXJS_PerObjectData(int nObjDefID)
      : m_ObjDefID(nObjDefID), m_pPrivate(nullptr) {}

  const int m_ObjDefID;
  void* m_pPrivate;
};

class CFXJS_ObjDefinition {
 public:
  static int MaxID(v8::Isolate* pIsolate) {
    return FXJS_PerIsolateData::Get(pIsolate)->m_ObjectDefnArray.size();
  }

  static CFXJS_ObjDefinition* ForID(v8::Isolate* pIsolate, int id) {
    // Note: GetAt() halts if out-of-range even in release builds.
    return FXJS_PerIsolateData::Get(pIsolate)->m_ObjectDefnArray[id];
  }

  CFXJS_ObjDefinition(v8::Isolate* isolate,
                      const wchar_t* sObjName,
                      FXJSOBJTYPE eObjType,
                      FXJS_CONSTRUCTOR pConstructor,
                      FXJS_DESTRUCTOR pDestructor)
      : m_ObjName(sObjName),
        m_ObjType(eObjType),
        m_pConstructor(pConstructor),
        m_pDestructor(pDestructor),
        m_pIsolate(isolate) {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(isolate);
    fun->InstanceTemplate()->SetInternalFieldCount(2);
    if (eObjType == FXJSOBJTYPE_GLOBAL) {
      fun->InstanceTemplate()->Set(
          v8::Symbol::GetToStringTag(isolate),
          v8::String::NewFromUtf8(isolate, "global", v8::NewStringType::kNormal)
              .ToLocalChecked());
    }
    m_FunctionTemplate.Reset(isolate, fun);

    v8::Local<v8::Signature> sig = v8::Signature::New(isolate, fun);
    m_Signature.Reset(isolate, sig);
  }

  int AssignID() {
    FXJS_PerIsolateData* pData = FXJS_PerIsolateData::Get(m_pIsolate);
    pData->m_ObjectDefnArray.push_back(this);
    return pData->m_ObjectDefnArray.size() - 1;
  }

  v8::Local<v8::ObjectTemplate> GetInstanceTemplate() {
    v8::EscapableHandleScope scope(m_pIsolate);
    v8::Local<v8::FunctionTemplate> function =
        m_FunctionTemplate.Get(m_pIsolate);
    return scope.Escape(function->InstanceTemplate());
  }

  v8::Local<v8::Signature> GetSignature() {
    v8::EscapableHandleScope scope(m_pIsolate);
    return scope.Escape(m_Signature.Get(m_pIsolate));
  }

  const wchar_t* const m_ObjName;
  const FXJSOBJTYPE m_ObjType;
  const FXJS_CONSTRUCTOR m_pConstructor;
  const FXJS_DESTRUCTOR m_pDestructor;

  v8::Isolate* m_pIsolate;
  v8::Global<v8::FunctionTemplate> m_FunctionTemplate;
  v8::Global<v8::Signature> m_Signature;
};

static v8::Local<v8::ObjectTemplate> GetGlobalObjectTemplate(
    v8::Isolate* pIsolate) {
  int maxID = CFXJS_ObjDefinition::MaxID(pIsolate);
  for (int i = 0; i < maxID; ++i) {
    CFXJS_ObjDefinition* pObjDef = CFXJS_ObjDefinition::ForID(pIsolate, i);
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL)
      return pObjDef->GetInstanceTemplate();
  }
  if (!g_DefaultGlobalObjectTemplate) {
    v8::Local<v8::ObjectTemplate> hGlobalTemplate =
        v8::ObjectTemplate::New(pIsolate);
    hGlobalTemplate->Set(
        v8::Symbol::GetToStringTag(pIsolate),
        v8::String::NewFromUtf8(pIsolate, "global", v8::NewStringType::kNormal)
            .ToLocalChecked());
    g_DefaultGlobalObjectTemplate =
        new v8::Global<v8::ObjectTemplate>(pIsolate, hGlobalTemplate);
  }
  return g_DefaultGlobalObjectTemplate->Get(pIsolate);
}

void* FXJS_ArrayBufferAllocator::Allocate(size_t length) {
  return calloc(1, length);
}

void* FXJS_ArrayBufferAllocator::AllocateUninitialized(size_t length) {
  return malloc(length);
}

void FXJS_ArrayBufferAllocator::Free(void* data, size_t length) {
  free(data);
}

void V8TemplateMapTraits::Dispose(v8::Isolate* isolate,
                                  v8::Global<v8::Object> value,
                                  void* key) {
  v8::Local<v8::Object> obj = value.Get(isolate);
  if (obj.IsEmpty())
    return;
  int id = FXJS_GetObjDefnID(obj);
  if (id == -1)
    return;

  CFXJS_ObjDefinition* pObjDef = CFXJS_ObjDefinition::ForID(isolate, id);
  if (!pObjDef)
    return;
  if (pObjDef->m_pDestructor)
    pObjDef->m_pDestructor(obj);
  FXJS_FreePrivate(obj);
}

V8TemplateMapTraits::MapType* V8TemplateMapTraits::MapFromWeakCallbackInfo(
    const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {
  V8TemplateMap* pMap =
      (FXJS_PerIsolateData::Get(data.GetIsolate()))->m_pDynamicObjsMap;
  return pMap ? &pMap->m_map : nullptr;
}

void FXJS_Initialize(unsigned int embedderDataSlot, v8::Isolate* pIsolate) {
  if (g_isolate) {
    ASSERT(g_embedderDataSlot == embedderDataSlot);
    ASSERT(g_isolate == pIsolate);
    return;
  }
  g_embedderDataSlot = embedderDataSlot;
  g_isolate = pIsolate;
}

void FXJS_Release() {
  ASSERT(!g_isolate || g_isolate_ref_count == 0);
  delete g_DefaultGlobalObjectTemplate;
  g_DefaultGlobalObjectTemplate = nullptr;
  g_isolate = nullptr;

  delete g_arrayBufferAllocator;
  g_arrayBufferAllocator = nullptr;
}

bool FXJS_GetIsolate(v8::Isolate** pResultIsolate) {
  if (g_isolate) {
    *pResultIsolate = g_isolate;
    return false;
  }
  // Provide backwards compatibility when no external isolate.
  if (!g_arrayBufferAllocator)
    g_arrayBufferAllocator = new FXJS_ArrayBufferAllocator();
  v8::Isolate::CreateParams params;
  params.array_buffer_allocator = g_arrayBufferAllocator;
  *pResultIsolate = v8::Isolate::New(params);
  return true;
}

size_t FXJS_GlobalIsolateRefCount() {
  return g_isolate_ref_count;
}

V8TemplateMap::V8TemplateMap(v8::Isolate* isolate) : m_map(isolate) {}

V8TemplateMap::~V8TemplateMap() {}

void V8TemplateMap::set(void* key, v8::Local<v8::Object> handle) {
  ASSERT(!m_map.Contains(key));
  m_map.Set(key, handle);
}

FXJS_PerIsolateData::~FXJS_PerIsolateData() {}

// static
void FXJS_PerIsolateData::SetUp(v8::Isolate* pIsolate) {
  if (!pIsolate->GetData(g_embedderDataSlot))
    pIsolate->SetData(g_embedderDataSlot, new FXJS_PerIsolateData());
}

// static
FXJS_PerIsolateData* FXJS_PerIsolateData::Get(v8::Isolate* pIsolate) {
  return static_cast<FXJS_PerIsolateData*>(
      pIsolate->GetData(g_embedderDataSlot));
}

#ifndef PDF_ENABLE_XFA
FXJS_PerIsolateData::FXJS_PerIsolateData() : m_pDynamicObjsMap(nullptr) {}
#else   // PDF_ENABLE_XFA
FXJS_PerIsolateData::FXJS_PerIsolateData()
    : m_pFXJSERuntimeData(nullptr), m_pDynamicObjsMap(nullptr) {}
#endif  // PDF_ENABLE_XFA

int FXJS_DefineObj(v8::Isolate* pIsolate,
                   const wchar_t* sObjName,
                   FXJSOBJTYPE eObjType,
                   FXJS_CONSTRUCTOR pConstructor,
                   FXJS_DESTRUCTOR pDestructor) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);

  FXJS_PerIsolateData::SetUp(pIsolate);
  CFXJS_ObjDefinition* pObjDef = new CFXJS_ObjDefinition(
      pIsolate, sObjName, eObjType, pConstructor, pDestructor);
  return pObjDef->AssignID();
}

void FXJS_DefineObjMethod(v8::Isolate* pIsolate,
                          int nObjDefnID,
                          const wchar_t* sMethodName,
                          v8::FunctionCallback pMethodCall) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFX_ByteString bsMethodName = CFX_WideString(sMethodName).UTF8Encode();
  CFXJS_ObjDefinition* pObjDef =
      CFXJS_ObjDefinition::ForID(pIsolate, nObjDefnID);
  v8::Local<v8::FunctionTemplate> fun = v8::FunctionTemplate::New(
      pIsolate, pMethodCall, v8::Local<v8::Value>(), pObjDef->GetSignature());
  fun->RemovePrototype();
  pObjDef->GetInstanceTemplate()->Set(
      v8::String::NewFromUtf8(pIsolate, bsMethodName.c_str(),
                              v8::NewStringType::kNormal)
          .ToLocalChecked(),
      fun, v8::ReadOnly);
}

void FXJS_DefineObjProperty(v8::Isolate* pIsolate,
                            int nObjDefnID,
                            const wchar_t* sPropName,
                            v8::AccessorGetterCallback pPropGet,
                            v8::AccessorSetterCallback pPropPut) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFX_ByteString bsPropertyName = CFX_WideString(sPropName).UTF8Encode();
  CFXJS_ObjDefinition* pObjDef =
      CFXJS_ObjDefinition::ForID(pIsolate, nObjDefnID);
  pObjDef->GetInstanceTemplate()->SetAccessor(
      v8::String::NewFromUtf8(pIsolate, bsPropertyName.c_str(),
                              v8::NewStringType::kNormal)
          .ToLocalChecked(),
      pPropGet, pPropPut);
}

void FXJS_DefineObjAllProperties(v8::Isolate* pIsolate,
                                 int nObjDefnID,
                                 v8::NamedPropertyQueryCallback pPropQurey,
                                 v8::NamedPropertyGetterCallback pPropGet,
                                 v8::NamedPropertySetterCallback pPropPut,
                                 v8::NamedPropertyDeleterCallback pPropDel) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFXJS_ObjDefinition* pObjDef =
      CFXJS_ObjDefinition::ForID(pIsolate, nObjDefnID);
  pObjDef->GetInstanceTemplate()->SetNamedPropertyHandler(pPropGet, pPropPut,
                                                          pPropQurey, pPropDel);
}

void FXJS_DefineObjConst(v8::Isolate* pIsolate,
                         int nObjDefnID,
                         const wchar_t* sConstName,
                         v8::Local<v8::Value> pDefault) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFX_ByteString bsConstName = CFX_WideString(sConstName).UTF8Encode();
  CFXJS_ObjDefinition* pObjDef =
      CFXJS_ObjDefinition::ForID(pIsolate, nObjDefnID);
  pObjDef->GetInstanceTemplate()->Set(pIsolate, bsConstName.c_str(), pDefault);
}

void FXJS_DefineGlobalMethod(v8::Isolate* pIsolate,
                             const wchar_t* sMethodName,
                             v8::FunctionCallback pMethodCall) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFX_ByteString bsMethodName = CFX_WideString(sMethodName).UTF8Encode();
  v8::Local<v8::FunctionTemplate> fun =
      v8::FunctionTemplate::New(pIsolate, pMethodCall);
  fun->RemovePrototype();
  GetGlobalObjectTemplate(pIsolate)->Set(
      v8::String::NewFromUtf8(pIsolate, bsMethodName.c_str(),
                              v8::NewStringType::kNormal)
          .ToLocalChecked(),
      fun, v8::ReadOnly);
}

void FXJS_DefineGlobalConst(v8::Isolate* pIsolate,
                            const wchar_t* sConstName,
                            v8::FunctionCallback pConstGetter) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  CFX_ByteString bsConst = CFX_WideString(sConstName).UTF8Encode();
  v8::Local<v8::FunctionTemplate> fun =
      v8::FunctionTemplate::New(pIsolate, pConstGetter);
  fun->RemovePrototype();
  GetGlobalObjectTemplate(pIsolate)->SetAccessorProperty(
      v8::String::NewFromUtf8(pIsolate, bsConst.c_str(),
                              v8::NewStringType::kNormal)
          .ToLocalChecked(),
      fun);
}

void FXJS_InitializeRuntime(
    v8::Isolate* pIsolate,
    IJS_Runtime* pIRuntime,
    v8::Global<v8::Context>* pV8PersistentContext,
    std::vector<v8::Global<v8::Object>*>* pStaticObjects) {
  if (pIsolate == g_isolate)
    ++g_isolate_ref_count;

  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);

  // This has to happen before we call GetGlobalObjectTemplate because that
  // method gets the PerIsolateData from pIsolate.
  FXJS_PerIsolateData::SetUp(pIsolate);

  v8::Local<v8::Context> v8Context =
      v8::Context::New(pIsolate, nullptr, GetGlobalObjectTemplate(pIsolate));
  v8::Context::Scope context_scope(v8Context);

  FXJS_PerIsolateData* pData = FXJS_PerIsolateData::Get(pIsolate);
  if (!pData)
    return;
  pData->CreateDynamicObjsMap(pIsolate);
  v8Context->SetAlignedPointerInEmbedderData(kPerContextDataIndex, pIRuntime);

  int maxID = CFXJS_ObjDefinition::MaxID(pIsolate);
  pStaticObjects->resize(maxID + 1);
  for (int i = 0; i < maxID; ++i) {
    CFXJS_ObjDefinition* pObjDef = CFXJS_ObjDefinition::ForID(pIsolate, i);
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL) {
      v8Context->Global()
          ->GetPrototype()
          ->ToObject(v8Context)
          .ToLocalChecked()
          ->SetAlignedPointerInInternalField(0, new CFXJS_PerObjectData(i));

      if (pObjDef->m_pConstructor)
        pObjDef->m_pConstructor(pIRuntime, v8Context->Global()
                                               ->GetPrototype()
                                               ->ToObject(v8Context)
                                               .ToLocalChecked());
    } else if (pObjDef->m_ObjType == FXJSOBJTYPE_STATIC) {
      CFX_ByteString bs = CFX_WideString(pObjDef->m_ObjName).UTF8Encode();
      v8::Local<v8::String> m_ObjName =
          v8::String::NewFromUtf8(pIsolate, bs.c_str(),
                                  v8::NewStringType::kNormal, bs.GetLength())
              .ToLocalChecked();

      v8::Local<v8::Object> obj =
          FXJS_NewFxDynamicObj(pIsolate, pIRuntime, i, true);
      v8Context->Global()->Set(v8Context, m_ObjName, obj).FromJust();
      pStaticObjects->at(i) = new v8::Global<v8::Object>(pIsolate, obj);
    }
  }
  pV8PersistentContext->Reset(pIsolate, v8Context);
}

void FXJS_ReleaseRuntime(v8::Isolate* pIsolate,
                         v8::Global<v8::Context>* pV8PersistentContext,
                         std::vector<v8::Global<v8::Object>*>* pStaticObjects) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope handle_scope(pIsolate);
  v8::Local<v8::Context> context =
      v8::Local<v8::Context>::New(pIsolate, *pV8PersistentContext);
  v8::Context::Scope context_scope(context);

  FXJS_PerIsolateData* pData = FXJS_PerIsolateData::Get(pIsolate);
  if (!pData)
    return;

  int maxID = CFXJS_ObjDefinition::MaxID(pIsolate);
  for (int i = 0; i < maxID; ++i) {
    CFXJS_ObjDefinition* pObjDef = CFXJS_ObjDefinition::ForID(pIsolate, i);
    v8::Local<v8::Object> pObj;
    if (pObjDef->m_ObjType == FXJSOBJTYPE_GLOBAL) {
      pObj =
          context->Global()->GetPrototype()->ToObject(context).ToLocalChecked();
    } else if (pStaticObjects->at(i) && !pStaticObjects->at(i)->IsEmpty()) {
      pObj = v8::Local<v8::Object>::New(pIsolate, *pStaticObjects->at(i));
      delete pStaticObjects->at(i);
      pStaticObjects->at(i) = nullptr;
    }

    if (!pObj.IsEmpty()) {
      if (pObjDef->m_pDestructor)
        pObjDef->m_pDestructor(pObj);
      FXJS_FreePrivate(pObj);
    }
  }

  if (pIsolate == g_isolate && --g_isolate_ref_count > 0)
    return;

  pData->ReleaseDynamicObjsMap();
  for (int i = 0; i < maxID; ++i)
    delete CFXJS_ObjDefinition::ForID(pIsolate, i);

  pIsolate->SetData(g_embedderDataSlot, nullptr);
  delete pData;
}

IJS_Runtime* FXJS_GetRuntimeFromIsolate(v8::Isolate* pIsolate) {
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return static_cast<IJS_Runtime*>(
      context->GetAlignedPointerFromEmbedderData(kPerContextDataIndex));
}

#ifdef PDF_ENABLE_XFA
void FXJS_SetRuntimeForV8Context(v8::Local<v8::Context> v8Context,
                                 IJS_Runtime* pIRuntime) {
  v8Context->SetAlignedPointerInEmbedderData(kPerContextDataIndex, pIRuntime);
}
#endif  // PDF_ENABLE_XFA

int FXJS_Execute(v8::Isolate* pIsolate,
                 const CFX_WideString& script,
                 FXJSErr* pError) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::TryCatch try_catch(pIsolate);
  CFX_ByteString bsScript = script.UTF8Encode();
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  v8::Local<v8::Script> compiled_script;
  if (!v8::Script::Compile(context,
                           v8::String::NewFromUtf8(pIsolate, bsScript.c_str(),
                                                   v8::NewStringType::kNormal,
                                                   bsScript.GetLength())
                               .ToLocalChecked())
           .ToLocal(&compiled_script)) {
    v8::String::Utf8Value error(try_catch.Exception());
    // TODO(tsepez): return error via pError->message.
    return -1;
  }

  v8::Local<v8::Value> result;
  if (!compiled_script->Run(context).ToLocal(&result)) {
    v8::String::Utf8Value error(try_catch.Exception());
    // TODO(tsepez): return error via pError->message.
    return -1;
  }
  return 0;
}

v8::Local<v8::Object> FXJS_NewFxDynamicObj(v8::Isolate* pIsolate,
                                           IJS_Runtime* pIRuntime,
                                           int nObjDefnID,
                                           bool bStatic) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  if (nObjDefnID == -1) {
    v8::Local<v8::ObjectTemplate> objTempl = v8::ObjectTemplate::New(pIsolate);
    v8::Local<v8::Object> obj;
    if (!objTempl->NewInstance(context).ToLocal(&obj))
      return v8::Local<v8::Object>();
    return obj;
  }

  FXJS_PerIsolateData* pData = FXJS_PerIsolateData::Get(pIsolate);
  if (!pData)
    return v8::Local<v8::Object>();

  if (nObjDefnID < 0 || nObjDefnID >= CFXJS_ObjDefinition::MaxID(pIsolate))
    return v8::Local<v8::Object>();

  CFXJS_ObjDefinition* pObjDef =
      CFXJS_ObjDefinition::ForID(pIsolate, nObjDefnID);
  v8::Local<v8::Object> obj;
  if (!pObjDef->GetInstanceTemplate()->NewInstance(context).ToLocal(&obj))
    return v8::Local<v8::Object>();

  CFXJS_PerObjectData* pPerObjData = new CFXJS_PerObjectData(nObjDefnID);
  obj->SetAlignedPointerInInternalField(0, pPerObjData);
  if (pObjDef->m_pConstructor)
    pObjDef->m_pConstructor(pIRuntime, obj);

  if (!bStatic && FXJS_PerIsolateData::Get(pIsolate)->m_pDynamicObjsMap) {
    FXJS_PerIsolateData::Get(pIsolate)->m_pDynamicObjsMap->set(pPerObjData,
                                                               obj);
  }
  return obj;
}

v8::Local<v8::Object> FXJS_GetThisObj(v8::Isolate* pIsolate) {
  v8::Isolate::Scope isolate_scope(pIsolate);
  if (!FXJS_PerIsolateData::Get(pIsolate))
    return v8::Local<v8::Object>();

  // Return the global object.
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return context->Global()->GetPrototype()->ToObject(context).ToLocalChecked();
}

int FXJS_GetObjDefnID(v8::Local<v8::Object> pObj) {
  if (pObj.IsEmpty() || !pObj->InternalFieldCount())
    return -1;
  CFXJS_PerObjectData* pPerObjectData = static_cast<CFXJS_PerObjectData*>(
      pObj->GetAlignedPointerFromInternalField(0));
  if (pPerObjectData)
    return pPerObjectData->m_ObjDefID;
  return -1;
}

void FXJS_Error(v8::Isolate* pIsolate, const CFX_WideString& message) {
  // Conversion from pdfium's wchar_t wide-strings to v8's uint16_t
  // wide-strings isn't handled by v8, so use UTF8 as a common
  // intermediate format.
  CFX_ByteString utf8_message = message.UTF8Encode();
  pIsolate->ThrowException(v8::String::NewFromUtf8(pIsolate,
                                                   utf8_message.c_str(),
                                                   v8::NewStringType::kNormal)
                               .ToLocalChecked());
}

void FXJS_SetPrivate(v8::Isolate* pIsolate,
                     v8::Local<v8::Object> pObj,
                     void* p) {
  if (pObj.IsEmpty() || !pObj->InternalFieldCount())
    return;
  CFXJS_PerObjectData* pPerObjectData = static_cast<CFXJS_PerObjectData*>(
      pObj->GetAlignedPointerFromInternalField(0));
  if (!pPerObjectData)
    return;
  pPerObjectData->m_pPrivate = p;
}

void* FXJS_GetPrivate(v8::Isolate* pIsolate, v8::Local<v8::Object> pObj) {
  if (pObj.IsEmpty())
    return nullptr;
  CFXJS_PerObjectData* pPerObjectData = nullptr;
  if (pObj->InternalFieldCount()) {
    pPerObjectData = static_cast<CFXJS_PerObjectData*>(
        pObj->GetAlignedPointerFromInternalField(0));
  } else {
    // It could be a global proxy object.
    v8::Local<v8::Value> v = pObj->GetPrototype();
    v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
    if (v->IsObject()) {
      pPerObjectData = static_cast<CFXJS_PerObjectData*>(
          v->ToObject(context)
              .ToLocalChecked()
              ->GetAlignedPointerFromInternalField(0));
    }
  }
  return pPerObjectData ? pPerObjectData->m_pPrivate : nullptr;
}

void FXJS_FreePrivate(void* pPerObjectData) {
  delete static_cast<CFXJS_PerObjectData*>(pPerObjectData);
}

void FXJS_FreePrivate(v8::Local<v8::Object> pObj) {
  if (pObj.IsEmpty() || !pObj->InternalFieldCount())
    return;
  FXJS_FreePrivate(pObj->GetAlignedPointerFromInternalField(0));
  pObj->SetAlignedPointerInInternalField(0, nullptr);
}

v8::Local<v8::String> FXJS_WSToJSString(v8::Isolate* pIsolate,
                                        const CFX_WideString& wsPropertyName) {
  CFX_ByteString bs = wsPropertyName.UTF8Encode();
  if (!pIsolate)
    pIsolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(pIsolate, bs.c_str(),
                                 v8::NewStringType::kNormal, bs.GetLength())
      .ToLocalChecked();
}

v8::Local<v8::Value> FXJS_GetObjectElement(
    v8::Isolate* pIsolate,
    v8::Local<v8::Object> pObj,
    const CFX_WideString& wsPropertyName) {
  if (pObj.IsEmpty())
    return v8::Local<v8::Value>();
  v8::Local<v8::Value> val;
  if (!pObj->Get(pIsolate->GetCurrentContext(),
                 FXJS_WSToJSString(pIsolate, wsPropertyName))
           .ToLocal(&val))
    return v8::Local<v8::Value>();
  return val;
}

v8::Local<v8::Array> FXJS_GetObjectElementNames(v8::Isolate* pIsolate,
                                                v8::Local<v8::Object> pObj) {
  if (pObj.IsEmpty())
    return v8::Local<v8::Array>();
  v8::Local<v8::Array> val;
  if (!pObj->GetPropertyNames(pIsolate->GetCurrentContext()).ToLocal(&val))
    return v8::Local<v8::Array>();
  return val;
}

void FXJS_PutObjectString(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          const CFX_WideString& wsValue) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            FXJS_WSToJSString(pIsolate, wsValue))
      .FromJust();
}

void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          int nValue) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            v8::Int32::New(pIsolate, nValue))
      .FromJust();
}

void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          float fValue) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            v8::Number::New(pIsolate, (double)fValue))
      .FromJust();
}

void FXJS_PutObjectNumber(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          double dValue) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            v8::Number::New(pIsolate, (double)dValue))
      .FromJust();
}

void FXJS_PutObjectBoolean(v8::Isolate* pIsolate,
                           v8::Local<v8::Object> pObj,
                           const CFX_WideString& wsPropertyName,
                           bool bValue) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            v8::Boolean::New(pIsolate, bValue))
      .FromJust();
}

void FXJS_PutObjectObject(v8::Isolate* pIsolate,
                          v8::Local<v8::Object> pObj,
                          const CFX_WideString& wsPropertyName,
                          v8::Local<v8::Object> pPut) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName), pPut)
      .FromJust();
}

void FXJS_PutObjectNull(v8::Isolate* pIsolate,
                        v8::Local<v8::Object> pObj,
                        const CFX_WideString& wsPropertyName) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(pIsolate->GetCurrentContext(),
            FXJS_WSToJSString(pIsolate, wsPropertyName),
            v8::Local<v8::Object>())
      .FromJust();
}

v8::Local<v8::Array> FXJS_NewArray(v8::Isolate* pIsolate) {
  return v8::Array::New(pIsolate);
}

unsigned FXJS_PutArrayElement(v8::Isolate* pIsolate,
                              v8::Local<v8::Array> pArray,
                              unsigned index,
                              v8::Local<v8::Value> pValue) {
  if (pArray.IsEmpty())
    return 0;
  if (pArray->Set(pIsolate->GetCurrentContext(), index, pValue).IsNothing())
    return 0;
  return 1;
}

v8::Local<v8::Value> FXJS_GetArrayElement(v8::Isolate* pIsolate,
                                          v8::Local<v8::Array> pArray,
                                          unsigned index) {
  if (pArray.IsEmpty())
    return v8::Local<v8::Value>();
  v8::Local<v8::Value> val;
  if (!pArray->Get(pIsolate->GetCurrentContext(), index).ToLocal(&val))
    return v8::Local<v8::Value>();
  return val;
}

unsigned FXJS_GetArrayLength(v8::Local<v8::Array> pArray) {
  if (pArray.IsEmpty())
    return 0;
  return pArray->Length();
}

v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, int number) {
  return v8::Int32::New(pIsolate, number);
}

v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, double number) {
  return v8::Number::New(pIsolate, number);
}

v8::Local<v8::Value> FXJS_NewNumber(v8::Isolate* pIsolate, float number) {
  return v8::Number::New(pIsolate, (float)number);
}

v8::Local<v8::Value> FXJS_NewBoolean(v8::Isolate* pIsolate, bool b) {
  return v8::Boolean::New(pIsolate, b);
}

v8::Local<v8::Value> FXJS_NewString(v8::Isolate* pIsolate, const wchar_t* str) {
  return FXJS_WSToJSString(pIsolate, str);
}

v8::Local<v8::Value> FXJS_NewNull(v8::Isolate* pIsolate) {
  return v8::Local<v8::Value>();
}

v8::Local<v8::Value> FXJS_NewDate(v8::Isolate* pIsolate, double d) {
  return v8::Date::New(pIsolate->GetCurrentContext(), d).ToLocalChecked();
}

int FXJS_ToInt32(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return 0;
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return pValue->ToInt32(context).ToLocalChecked()->Value();
}

bool FXJS_ToBoolean(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return false;
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return pValue->ToBoolean(context).ToLocalChecked()->Value();
}

double FXJS_ToNumber(v8::Isolate* pIsolate, v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return 0.0;
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return pValue->ToNumber(context).ToLocalChecked()->Value();
}

v8::Local<v8::Object> FXJS_ToObject(v8::Isolate* pIsolate,
                                    v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return v8::Local<v8::Object>();
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return pValue->ToObject(context).ToLocalChecked();
}

CFX_WideString FXJS_ToString(v8::Isolate* pIsolate,
                             v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return L"";
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  v8::String::Utf8Value s(pValue->ToString(context).ToLocalChecked());
  return CFX_WideString::FromUTF8(CFX_ByteStringC(*s, s.length()));
}

v8::Local<v8::Array> FXJS_ToArray(v8::Isolate* pIsolate,
                                  v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return v8::Local<v8::Array>();
  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();
  return v8::Local<v8::Array>::Cast(pValue->ToObject(context).ToLocalChecked());
}

void FXJS_ValueCopy(v8::Local<v8::Value>& pTo, v8::Local<v8::Value> pFrom) {
  pTo = pFrom;
}
