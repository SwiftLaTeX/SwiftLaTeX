// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_GLOBAL_H_
#define FPDFSDK_JAVASCRIPT_GLOBAL_H_

#include <map>
#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"

class CJS_GlobalData;
class CJS_GlobalVariableArray;
class CJS_KeyValue;

struct JSGlobalData {
  JSGlobalData();
  ~JSGlobalData();

  int nType;  // 0:int 1:bool 2:string 3:obj
  double dData;
  bool bData;
  CFX_ByteString sData;
  v8::Global<v8::Object> pData;
  bool bPersistent;
  bool bDeleted;
};

class JSGlobalAlternate : public CJS_EmbedObj {
 public:
  JSGlobalAlternate(CJS_Object* pJSObject);
  ~JSGlobalAlternate() override;

  FX_BOOL setPersistent(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL QueryProperty(const FX_WCHAR* propname);
  FX_BOOL DoProperty(IJS_Context* cc,
                     const FX_WCHAR* propname,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL DelProperty(IJS_Context* cc,
                      const FX_WCHAR* propname,
                      CFX_WideString& sError);
  void Initial(CPDFDoc_Environment* pApp);

 private:
  void UpdateGlobalPersistentVariables();
  void CommitGlobalPersisitentVariables(IJS_Context* cc);
  void DestroyGlobalPersisitentVariables();
  FX_BOOL SetGlobalVariables(const CFX_ByteString& propname,
                             int nType,
                             double dData,
                             bool bData,
                             const CFX_ByteString& sData,
                             v8::Local<v8::Object> pData,
                             bool bDefaultPersistent);
  void ObjectToArray(IJS_Context* cc,
                     v8::Local<v8::Object> pObj,
                     CJS_GlobalVariableArray& array);
  void PutObjectProperty(v8::Local<v8::Object> obj, CJS_KeyValue* pData);

  std::map<CFX_ByteString, JSGlobalData*> m_mapGlobal;
  CFX_WideString m_sFilePath;
  CJS_GlobalData* m_pGlobalData;
  CPDFDoc_Environment* m_pApp;
};

class CJS_Global : public CJS_Object {
 public:
  explicit CJS_Global(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_Global() override {}

  // CJS_Object
  void InitInstance(IJS_Runtime* pIRuntime) override;

  DECLARE_SPECIAL_JS_CLASS();
  JS_SPECIAL_STATIC_METHOD(setPersistent, JSGlobalAlternate, global);
};

#endif  // FPDFSDK_JAVASCRIPT_GLOBAL_H_
