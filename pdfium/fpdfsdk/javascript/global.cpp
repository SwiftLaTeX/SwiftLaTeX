// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/global.h"

#include <vector>

#include "core/fxcrt/include/fx_ext.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_GlobalData.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"
#include "fpdfsdk/javascript/resource.h"

BEGIN_JS_STATIC_CONST(CJS_Global)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Global)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Global)
JS_STATIC_METHOD_ENTRY(setPersistent)
END_JS_STATIC_METHOD()

IMPLEMENT_SPECIAL_JS_CLASS(CJS_Global, JSGlobalAlternate, global);

void CJS_Global::InitInstance(IJS_Runtime* pIRuntime) {
  CJS_Runtime* pRuntime = static_cast<CJS_Runtime*>(pIRuntime);
  JSGlobalAlternate* pGlobal =
      static_cast<JSGlobalAlternate*>(GetEmbedObject());
  pGlobal->Initial(pRuntime->GetReaderApp());
}

JSGlobalData::JSGlobalData()
    : nType(0),
      dData(0),
      bData(FALSE),
      sData(""),
      bPersistent(FALSE),
      bDeleted(FALSE) {}

JSGlobalData::~JSGlobalData() {
  pData.Reset();
}

JSGlobalAlternate::JSGlobalAlternate(CJS_Object* pJSObject)
    : CJS_EmbedObj(pJSObject), m_pApp(nullptr) {}

JSGlobalAlternate::~JSGlobalAlternate() {
  DestroyGlobalPersisitentVariables();
  m_pGlobalData->Release();
}

void JSGlobalAlternate::Initial(CPDFDoc_Environment* pApp) {
  m_pApp = pApp;
  m_pGlobalData = CJS_GlobalData::GetRetainedInstance(pApp);
  UpdateGlobalPersistentVariables();
}

FX_BOOL JSGlobalAlternate::QueryProperty(const FX_WCHAR* propname) {
  return CFX_WideString(propname) != L"setPersistent";
}

FX_BOOL JSGlobalAlternate::DelProperty(IJS_Context* cc,
                                       const FX_WCHAR* propname,
                                       CFX_WideString& sError) {
  auto it = m_mapGlobal.find(CFX_ByteString::FromUnicode(propname));
  if (it == m_mapGlobal.end())
    return FALSE;

  it->second->bDeleted = TRUE;
  return TRUE;
}

FX_BOOL JSGlobalAlternate::DoProperty(IJS_Context* cc,
                                      const FX_WCHAR* propname,
                                      CJS_PropValue& vp,
                                      CFX_WideString& sError) {
  if (vp.IsSetting()) {
    CFX_ByteString sPropName = CFX_ByteString::FromUnicode(propname);
    switch (vp.GetType()) {
      case CJS_Value::VT_number: {
        double dData;
        vp >> dData;
        return SetGlobalVariables(sPropName, JS_GLOBALDATA_TYPE_NUMBER, dData,
                                  false, "", v8::Local<v8::Object>(), FALSE);
      }
      case CJS_Value::VT_boolean: {
        bool bData;
        vp >> bData;
        return SetGlobalVariables(sPropName, JS_GLOBALDATA_TYPE_BOOLEAN, 0,
                                  bData, "", v8::Local<v8::Object>(), FALSE);
      }
      case CJS_Value::VT_string: {
        CFX_ByteString sData;
        vp >> sData;
        return SetGlobalVariables(sPropName, JS_GLOBALDATA_TYPE_STRING, 0,
                                  false, sData, v8::Local<v8::Object>(), FALSE);
      }
      case CJS_Value::VT_object: {
        v8::Local<v8::Object> pData;
        vp >> pData;
        return SetGlobalVariables(sPropName, JS_GLOBALDATA_TYPE_OBJECT, 0,
                                  false, "", pData, FALSE);
      }
      case CJS_Value::VT_null: {
        return SetGlobalVariables(sPropName, JS_GLOBALDATA_TYPE_NULL, 0, false,
                                  "", v8::Local<v8::Object>(), FALSE);
      }
      case CJS_Value::VT_undefined: {
        DelProperty(cc, propname, sError);
        return TRUE;
      }
      default:
        break;
    }
  } else {
    auto it = m_mapGlobal.find(CFX_ByteString::FromUnicode(propname));
    if (it == m_mapGlobal.end()) {
      vp.SetNull();
      return TRUE;
    }
    JSGlobalData* pData = it->second;
    if (pData->bDeleted) {
      vp.SetNull();
      return TRUE;
    }
    switch (pData->nType) {
      case JS_GLOBALDATA_TYPE_NUMBER:
        vp << pData->dData;
        return TRUE;
      case JS_GLOBALDATA_TYPE_BOOLEAN:
        vp << pData->bData;
        return TRUE;
      case JS_GLOBALDATA_TYPE_STRING:
        vp << pData->sData;
        return TRUE;
      case JS_GLOBALDATA_TYPE_OBJECT: {
        v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(
            vp.GetJSRuntime()->GetIsolate(), pData->pData);
        vp << obj;
        return TRUE;
      }
      case JS_GLOBALDATA_TYPE_NULL:
        vp.SetNull();
        return TRUE;
      default:
        break;
    }
  }
  return FALSE;
}

FX_BOOL JSGlobalAlternate::setPersistent(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  CJS_Context* pContext = static_cast<CJS_Context*>(cc);
  if (params.size() != 2) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  auto it = m_mapGlobal.find(params[0].ToCFXByteString());
  if (it != m_mapGlobal.end()) {
    JSGlobalData* pData = it->second;
    if (!pData->bDeleted) {
      pData->bPersistent = params[1].ToBool();
      return TRUE;
    }
  }

  sError = JSGetStringFromID(pContext, IDS_STRING_JSNOGLOBAL);
  return FALSE;
}

void JSGlobalAlternate::UpdateGlobalPersistentVariables() {
  for (int i = 0, sz = m_pGlobalData->GetSize(); i < sz; i++) {
    CJS_GlobalData_Element* pData = m_pGlobalData->GetAt(i);
    switch (pData->data.nType) {
      case JS_GLOBALDATA_TYPE_NUMBER:
        SetGlobalVariables(pData->data.sKey, JS_GLOBALDATA_TYPE_NUMBER,
                           pData->data.dData, false, "",
                           v8::Local<v8::Object>(), pData->bPersistent == 1);
        FXJS_PutObjectNumber(nullptr, m_pJSObject->ToV8Object(),
                             pData->data.sKey.UTF8Decode(), pData->data.dData);
        break;
      case JS_GLOBALDATA_TYPE_BOOLEAN:
        SetGlobalVariables(pData->data.sKey, JS_GLOBALDATA_TYPE_BOOLEAN, 0,
                           (bool)(pData->data.bData == 1), "",
                           v8::Local<v8::Object>(), pData->bPersistent == 1);
        FXJS_PutObjectBoolean(nullptr, m_pJSObject->ToV8Object(),
                              pData->data.sKey.UTF8Decode(),
                              (bool)(pData->data.bData == 1));
        break;
      case JS_GLOBALDATA_TYPE_STRING:
        SetGlobalVariables(pData->data.sKey, JS_GLOBALDATA_TYPE_STRING, 0,
                           false, pData->data.sData, v8::Local<v8::Object>(),
                           pData->bPersistent == 1);
        FXJS_PutObjectString(nullptr, m_pJSObject->ToV8Object(),
                             pData->data.sKey.UTF8Decode(),
                             pData->data.sData.UTF8Decode());
        break;
      case JS_GLOBALDATA_TYPE_OBJECT: {
        v8::Isolate* pRuntime = m_pJSObject->ToV8Object()->GetIsolate();
        v8::Local<v8::Object> pObj =
            FXJS_NewFxDynamicObj(pRuntime, nullptr, -1);

        PutObjectProperty(pObj, &pData->data);

        SetGlobalVariables(pData->data.sKey, JS_GLOBALDATA_TYPE_OBJECT, 0,
                           false, "", pObj, pData->bPersistent == 1);
        FXJS_PutObjectObject(nullptr, m_pJSObject->ToV8Object(),
                             pData->data.sKey.UTF8Decode(), pObj);
      } break;
      case JS_GLOBALDATA_TYPE_NULL:
        SetGlobalVariables(pData->data.sKey, JS_GLOBALDATA_TYPE_NULL, 0, false,
                           "", v8::Local<v8::Object>(),
                           pData->bPersistent == 1);
        FXJS_PutObjectNull(nullptr, m_pJSObject->ToV8Object(),
                           pData->data.sKey.UTF8Decode());
        break;
    }
  }
}

void JSGlobalAlternate::CommitGlobalPersisitentVariables(IJS_Context* cc) {
  for (auto it = m_mapGlobal.begin(); it != m_mapGlobal.end(); ++it) {
    CFX_ByteString name = it->first;
    JSGlobalData* pData = it->second;
    if (pData->bDeleted) {
      m_pGlobalData->DeleteGlobalVariable(name);
    } else {
      switch (pData->nType) {
        case JS_GLOBALDATA_TYPE_NUMBER:
          m_pGlobalData->SetGlobalVariableNumber(name, pData->dData);
          m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
          break;
        case JS_GLOBALDATA_TYPE_BOOLEAN:
          m_pGlobalData->SetGlobalVariableBoolean(name, pData->bData);
          m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
          break;
        case JS_GLOBALDATA_TYPE_STRING:
          m_pGlobalData->SetGlobalVariableString(name, pData->sData);
          m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
          break;
        case JS_GLOBALDATA_TYPE_OBJECT: {
          CJS_GlobalVariableArray array;
          v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(
              GetJSObject()->GetIsolate(), pData->pData);
          ObjectToArray(cc, obj, array);
          m_pGlobalData->SetGlobalVariableObject(name, array);
          m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
        } break;
        case JS_GLOBALDATA_TYPE_NULL:
          m_pGlobalData->SetGlobalVariableNull(name);
          m_pGlobalData->SetGlobalVariablePersistent(name, pData->bPersistent);
          break;
      }
    }
  }
}

void JSGlobalAlternate::ObjectToArray(IJS_Context* cc,
                                      v8::Local<v8::Object> pObj,
                                      CJS_GlobalVariableArray& array) {
  v8::Isolate* isolate = pObj->GetIsolate();
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);

  v8::Local<v8::Array> pKeyList = FXJS_GetObjectElementNames(isolate, pObj);
  int nObjElements = pKeyList->Length();
  for (int i = 0; i < nObjElements; i++) {
    CFX_WideString ws =
        FXJS_ToString(isolate, FXJS_GetArrayElement(isolate, pKeyList, i));
    CFX_ByteString sKey = ws.UTF8Encode();
    v8::Local<v8::Value> v = FXJS_GetObjectElement(isolate, pObj, ws);
    switch (CJS_Value::GetValueType(v)) {
      case CJS_Value::VT_number: {
        CJS_KeyValue* pObjElement = new CJS_KeyValue;
        pObjElement->nType = JS_GLOBALDATA_TYPE_NUMBER;
        pObjElement->sKey = sKey;
        pObjElement->dData = FXJS_ToNumber(isolate, v);
        array.Add(pObjElement);
      } break;
      case CJS_Value::VT_boolean: {
        CJS_KeyValue* pObjElement = new CJS_KeyValue;
        pObjElement->nType = JS_GLOBALDATA_TYPE_BOOLEAN;
        pObjElement->sKey = sKey;
        pObjElement->dData = FXJS_ToBoolean(isolate, v);
        array.Add(pObjElement);
      } break;
      case CJS_Value::VT_string: {
        CFX_ByteString sValue = CJS_Value(pRuntime, v).ToCFXByteString();
        CJS_KeyValue* pObjElement = new CJS_KeyValue;
        pObjElement->nType = JS_GLOBALDATA_TYPE_STRING;
        pObjElement->sKey = sKey;
        pObjElement->sData = sValue;
        array.Add(pObjElement);
      } break;
      case CJS_Value::VT_object: {
        CJS_KeyValue* pObjElement = new CJS_KeyValue;
        pObjElement->nType = JS_GLOBALDATA_TYPE_OBJECT;
        pObjElement->sKey = sKey;
        ObjectToArray(cc, FXJS_ToObject(isolate, v), pObjElement->objData);
        array.Add(pObjElement);
      } break;
      case CJS_Value::VT_null: {
        CJS_KeyValue* pObjElement = new CJS_KeyValue;
        pObjElement->nType = JS_GLOBALDATA_TYPE_NULL;
        pObjElement->sKey = sKey;
        array.Add(pObjElement);
      } break;
      default:
        break;
    }
  }
}

void JSGlobalAlternate::PutObjectProperty(v8::Local<v8::Object> pObj,
                                          CJS_KeyValue* pData) {
  for (int i = 0, sz = pData->objData.Count(); i < sz; i++) {
    CJS_KeyValue* pObjData = pData->objData.GetAt(i);
    switch (pObjData->nType) {
      case JS_GLOBALDATA_TYPE_NUMBER:
        FXJS_PutObjectNumber(nullptr, pObj, pObjData->sKey.UTF8Decode(),
                             pObjData->dData);
        break;
      case JS_GLOBALDATA_TYPE_BOOLEAN:
        FXJS_PutObjectBoolean(nullptr, pObj, pObjData->sKey.UTF8Decode(),
                              pObjData->bData == 1);
        break;
      case JS_GLOBALDATA_TYPE_STRING:
        FXJS_PutObjectString(nullptr, pObj, pObjData->sKey.UTF8Decode(),
                             pObjData->sData.UTF8Decode());
        break;
      case JS_GLOBALDATA_TYPE_OBJECT: {
        v8::Isolate* pRuntime = m_pJSObject->ToV8Object()->GetIsolate();
        v8::Local<v8::Object> pNewObj =
            FXJS_NewFxDynamicObj(pRuntime, nullptr, -1);
        PutObjectProperty(pNewObj, pObjData);
        FXJS_PutObjectObject(nullptr, pObj, pObjData->sKey.UTF8Decode(),
                             pNewObj);
      } break;
      case JS_GLOBALDATA_TYPE_NULL:
        FXJS_PutObjectNull(nullptr, pObj, pObjData->sKey.UTF8Decode());
        break;
    }
  }
}

void JSGlobalAlternate::DestroyGlobalPersisitentVariables() {
  for (const auto& pair : m_mapGlobal) {
    delete pair.second;
  }
  m_mapGlobal.clear();
}

FX_BOOL JSGlobalAlternate::SetGlobalVariables(const CFX_ByteString& propname,
                                              int nType,
                                              double dData,
                                              bool bData,
                                              const CFX_ByteString& sData,
                                              v8::Local<v8::Object> pData,
                                              bool bDefaultPersistent) {
  if (propname.IsEmpty())
    return FALSE;

  auto it = m_mapGlobal.find(propname);
  if (it != m_mapGlobal.end()) {
    JSGlobalData* pTemp = it->second;
    if (pTemp->bDeleted || pTemp->nType != nType) {
      pTemp->dData = 0;
      pTemp->bData = 0;
      pTemp->sData = "";
      pTemp->nType = nType;
    }

    pTemp->bDeleted = FALSE;
    switch (nType) {
      case JS_GLOBALDATA_TYPE_NUMBER: {
        pTemp->dData = dData;
      } break;
      case JS_GLOBALDATA_TYPE_BOOLEAN: {
        pTemp->bData = bData;
      } break;
      case JS_GLOBALDATA_TYPE_STRING: {
        pTemp->sData = sData;
      } break;
      case JS_GLOBALDATA_TYPE_OBJECT: {
        pTemp->pData.Reset(pData->GetIsolate(), pData);
      } break;
      case JS_GLOBALDATA_TYPE_NULL:
        break;
      default:
        return FALSE;
    }
    return TRUE;
  }

  JSGlobalData* pNewData = nullptr;

  switch (nType) {
    case JS_GLOBALDATA_TYPE_NUMBER: {
      pNewData = new JSGlobalData;
      pNewData->nType = JS_GLOBALDATA_TYPE_NUMBER;
      pNewData->dData = dData;
      pNewData->bPersistent = bDefaultPersistent;
    } break;
    case JS_GLOBALDATA_TYPE_BOOLEAN: {
      pNewData = new JSGlobalData;
      pNewData->nType = JS_GLOBALDATA_TYPE_BOOLEAN;
      pNewData->bData = bData;
      pNewData->bPersistent = bDefaultPersistent;
    } break;
    case JS_GLOBALDATA_TYPE_STRING: {
      pNewData = new JSGlobalData;
      pNewData->nType = JS_GLOBALDATA_TYPE_STRING;
      pNewData->sData = sData;
      pNewData->bPersistent = bDefaultPersistent;
    } break;
    case JS_GLOBALDATA_TYPE_OBJECT: {
      pNewData = new JSGlobalData;
      pNewData->nType = JS_GLOBALDATA_TYPE_OBJECT;
      pNewData->pData.Reset(pData->GetIsolate(), pData);
      pNewData->bPersistent = bDefaultPersistent;
    } break;
    case JS_GLOBALDATA_TYPE_NULL: {
      pNewData = new JSGlobalData;
      pNewData->nType = JS_GLOBALDATA_TYPE_NULL;
      pNewData->bPersistent = bDefaultPersistent;
    } break;
    default:
      return FALSE;
  }

  m_mapGlobal[propname] = pNewData;
  return TRUE;
}
