// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_JS_OBJECT_H_
#define FPDFSDK_JAVASCRIPT_JS_OBJECT_H_

#include <map>
#include <memory>

#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/javascript/cjs_runtime.h"
#include "fxjs/include/fxjs_v8.h"

class CJS_Context;
class CJS_Object;
class CJS_Timer;
class CPDFDoc_Environment;
class CJS_EmbedObj {
 public:
  explicit CJS_EmbedObj(CJS_Object* pJSObject);
  virtual ~CJS_EmbedObj();

  virtual void TimerProc(CJS_Timer* pTimer) {}

  CJS_Object* GetJSObject() const { return m_pJSObject; }

  int MsgBox(CPDFDoc_Environment* pApp,
             const FX_WCHAR* swMsg,
             const FX_WCHAR* swTitle,
             FX_UINT nType,
             FX_UINT nIcon);
  void Alert(CJS_Context* pContext, const FX_WCHAR* swMsg);

 protected:
  CJS_Object* m_pJSObject;
};

class CJS_Object {
 public:
  explicit CJS_Object(v8::Local<v8::Object> pObject);
  virtual ~CJS_Object();

  void MakeWeak();
  void Dispose();

  virtual FX_BOOL IsType(const FX_CHAR* sClassName);
  virtual CFX_ByteString GetClassName();

  virtual void InitInstance(IJS_Runtime* pIRuntime);
  virtual void ExitInstance();

  v8::Local<v8::Object> ToV8Object() { return m_pV8Object.Get(m_pIsolate); }

  // Takes ownership of |pObj|.
  void SetEmbedObject(CJS_EmbedObj* pObj) { m_pEmbedObj.reset(pObj); }
  CJS_EmbedObj* GetEmbedObject() const { return m_pEmbedObj.get(); }

  static int MsgBox(CPDFDoc_Environment* pApp,
                    const FX_WCHAR* swMsg,
                    const FX_WCHAR* swTitle,
                    FX_UINT nType,
                    FX_UINT nIcon);
  static void Alert(CJS_Context* pContext, const FX_WCHAR* swMsg);

  v8::Isolate* GetIsolate() { return m_pIsolate; }

 protected:
  std::unique_ptr<CJS_EmbedObj> m_pEmbedObj;
  v8::Global<v8::Object> m_pV8Object;
  v8::Isolate* m_pIsolate;
};

class CJS_Timer : public CJS_Runtime::Observer {
 public:
  CJS_Timer(CJS_EmbedObj* pObj,
            CPDFDoc_Environment* pApp,
            CJS_Runtime* pRuntime,
            int nType,
            const CFX_WideString& script,
            uint32_t dwElapse,
            uint32_t dwTimeOut);
  ~CJS_Timer() override;

  void KillJSTimer();

  int GetType() const { return m_nType; }
  uint32_t GetTimeOut() const { return m_dwTimeOut; }
  CJS_Runtime* GetRuntime() const { return m_bValid ? m_pRuntime : nullptr; }
  CFX_WideString GetJScript() const { return m_swJScript; }

  static void TimerProc(int idEvent);

 private:
  using TimerMap = std::map<FX_UINT, CJS_Timer*>;
  static TimerMap* GetGlobalTimerMap();

  // CJS_Runtime::Observer
  void OnDestroyed() override;

  uint32_t m_nTimerID;
  CJS_EmbedObj* const m_pEmbedObj;
  bool m_bProcessing;
  bool m_bValid;

  // data
  const int m_nType;  // 0:Interval; 1:TimeOut
  const uint32_t m_dwTimeOut;
  const CFX_WideString m_swJScript;
  CJS_Runtime* const m_pRuntime;
  CPDFDoc_Environment* const m_pApp;
};

#endif  // FPDFSDK_JAVASCRIPT_JS_OBJECT_H_
