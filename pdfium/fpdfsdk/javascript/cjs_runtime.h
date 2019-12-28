// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_CJS_RUNTIME_H_
#define FPDFSDK_JAVASCRIPT_CJS_RUNTIME_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "core/fxcrt/include/fx_basic.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/ijs_runtime.h"
#include "fxjs/include/fxjs_v8.h"

class CJS_Context;

class CJS_Runtime : public IJS_Runtime {
 public:
  class Observer {
   public:
    virtual void OnDestroyed() = 0;

   protected:
    virtual ~Observer() {}
  };

  using FieldEvent = std::pair<CFX_WideString, JS_EVENT_T>;

  static CJS_Runtime* FromContext(const IJS_Context* cc);

  explicit CJS_Runtime(CPDFDoc_Environment* pApp);
  ~CJS_Runtime() override;

  // IJS_Runtime
  IJS_Context* NewContext() override;
  void ReleaseContext(IJS_Context* pContext) override;
  IJS_Context* GetCurrentContext() override;
  void SetReaderDocument(CPDFSDK_Document* pReaderDoc) override;
  CPDFSDK_Document* GetReaderDocument() override;
  int Execute(const CFX_WideString& script, CFX_WideString* info) override;

  CPDFDoc_Environment* GetReaderApp() const { return m_pApp; }

  // Returns true if the event isn't already found in the set.
  bool AddEventToSet(const FieldEvent& event);
  void RemoveEventFromSet(const FieldEvent& event);

  void BeginBlock() { m_bBlocking = TRUE; }
  void EndBlock() { m_bBlocking = FALSE; }
  FX_BOOL IsBlocking() const { return m_bBlocking; }

  v8::Isolate* GetIsolate() const { return m_isolate; }
  v8::Local<v8::Context> NewJSContext();

  void SetConstArray(const CFX_WideString& name, v8::Local<v8::Array> array);
  v8::Local<v8::Array> GetConstArray(const CFX_WideString& name);

#ifdef PDF_ENABLE_XFA
  FX_BOOL GetValueByName(const CFX_ByteStringC& utf8Name,
                         CFXJSE_Value* pValue) override;
  FX_BOOL SetValueByName(const CFX_ByteStringC& utf8Name,
                         CFXJSE_Value* pValue) override;
#endif  // PDF_ENABLE_XFA

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

 private:
  void DefineJSObjects();

  std::vector<std::unique_ptr<CJS_Context>> m_ContextArray;
  CPDFDoc_Environment* const m_pApp;
  CPDFSDK_Document* m_pDocument;
  FX_BOOL m_bBlocking;
  std::set<FieldEvent> m_FieldEventSet;
  v8::Isolate* m_isolate;
  bool m_isolateManaged;
  v8::Global<v8::Context> m_context;
  std::vector<v8::Global<v8::Object>*> m_StaticObjects;
  std::map<CFX_WideString, v8::Global<v8::Array>> m_ConstArrays;
  std::set<Observer*> m_observers;
};

#endif  // FPDFSDK_JAVASCRIPT_CJS_RUNTIME_H_
