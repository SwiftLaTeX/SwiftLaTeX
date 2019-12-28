// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_APP_H_
#define FPDFSDK_JAVASCRIPT_APP_H_

#include <memory>
#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"

class CJS_Runtime;
class CJS_Timer;

class TimerObj : public CJS_EmbedObj {
 public:
  TimerObj(CJS_Object* pJSObject);
  ~TimerObj() override;

 public:
  void SetTimer(CJS_Timer* pTimer);
  CJS_Timer* GetTimer() const;

 private:
  CJS_Timer* m_pTimer;
};

class CJS_TimerObj : public CJS_Object {
 public:
  CJS_TimerObj(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_TimerObj() override {}

  DECLARE_JS_CLASS();
};

class app : public CJS_EmbedObj {
 public:
  app(CJS_Object* pJSObject);
  ~app() override;

  FX_BOOL activeDocs(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL calculate(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL formsVersion(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL fs(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL fullscreen(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL language(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL media(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL platform(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL runtimeHighlight(IJS_Context* cc,
                           CJS_PropValue& vp,
                           CFX_WideString& sError);
  FX_BOOL viewerType(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL viewerVariation(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError);
  FX_BOOL viewerVersion(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError);

  FX_BOOL alert(IJS_Context* cc,
                const std::vector<CJS_Value>& params,
                CJS_Value& vRet,
                CFX_WideString& sError);
  FX_BOOL beep(IJS_Context* cc,
               const std::vector<CJS_Value>& params,
               CJS_Value& vRet,
               CFX_WideString& sError);
  FX_BOOL browseForDoc(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL clearInterval(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL clearTimeOut(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL execDialog(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError);
  FX_BOOL execMenuItem(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError);
  FX_BOOL findComponent(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError);
  FX_BOOL goBack(IJS_Context* cc,
                 const std::vector<CJS_Value>& params,
                 CJS_Value& vRet,
                 CFX_WideString& sError);
  FX_BOOL goForward(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
  FX_BOOL launchURL(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
  FX_BOOL mailMsg(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError);
  FX_BOOL newFDF(IJS_Context* cc,
                 const std::vector<CJS_Value>& params,
                 CJS_Value& vRet,
                 CFX_WideString& sError);
  FX_BOOL newDoc(IJS_Context* cc,
                 const std::vector<CJS_Value>& params,
                 CJS_Value& vRet,
                 CFX_WideString& sError);
  FX_BOOL openDoc(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError);
  FX_BOOL openFDF(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError);
  FX_BOOL popUpMenuEx(IJS_Context* cc,
                      const std::vector<CJS_Value>& params,
                      CJS_Value& vRet,
                      CFX_WideString& sError);
  FX_BOOL popUpMenu(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
  FX_BOOL response(IJS_Context* cc,
                   const std::vector<CJS_Value>& params,
                   CJS_Value& vRet,
                   CFX_WideString& sError);
  FX_BOOL setInterval(IJS_Context* cc,
                      const std::vector<CJS_Value>& params,
                      CJS_Value& vRet,
                      CFX_WideString& sError);
  FX_BOOL setTimeOut(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError);

  static CFX_WideString SysPathToPDFPath(const CFX_WideString& sOldPath);

 private:
  // CJS_EmbedObj
  void TimerProc(CJS_Timer* pTimer) override;
  void RunJsScript(CJS_Runtime* pRuntime, const CFX_WideString& wsScript);

  void ClearTimerCommon(const CJS_Value& param);

  bool m_bCalculate;
  bool m_bRuntimeHighLight;
  std::vector<std::unique_ptr<CJS_Timer>> m_Timers;
};

class CJS_App : public CJS_Object {
 public:
  explicit CJS_App(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_App() override {}

  DECLARE_JS_CLASS();

  JS_STATIC_PROP(activeDocs, app);
  JS_STATIC_PROP(calculate, app);
  JS_STATIC_PROP(formsVersion, app);
  JS_STATIC_PROP(fs, app);
  JS_STATIC_PROP(fullscreen, app);
  JS_STATIC_PROP(language, app);
  JS_STATIC_PROP(media, app);
  JS_STATIC_PROP(platform, app);
  JS_STATIC_PROP(runtimeHighlight, app);
  JS_STATIC_PROP(viewerType, app);
  JS_STATIC_PROP(viewerVariation, app);
  JS_STATIC_PROP(viewerVersion, app);

  JS_STATIC_METHOD(alert, app);
  JS_STATIC_METHOD(beep, app);
  JS_STATIC_METHOD(browseForDoc, app);
  JS_STATIC_METHOD(clearInterval, app);
  JS_STATIC_METHOD(clearTimeOut, app);
  JS_STATIC_METHOD(execDialog, app);
  JS_STATIC_METHOD(execMenuItem, app);
  JS_STATIC_METHOD(findComponent, app);
  JS_STATIC_METHOD(goBack, app);
  JS_STATIC_METHOD(goForward, app);
  JS_STATIC_METHOD(launchURL, app);
  JS_STATIC_METHOD(mailMsg, app);
  JS_STATIC_METHOD(newFDF, app);
  JS_STATIC_METHOD(newDoc, app);
  JS_STATIC_METHOD(openDoc, app);
  JS_STATIC_METHOD(openFDF, app);
  JS_STATIC_METHOD(popUpMenuEx, app);
  JS_STATIC_METHOD(popUpMenu, app);
  JS_STATIC_METHOD(response, app);
  JS_STATIC_METHOD(setInterval, app);
  JS_STATIC_METHOD(setTimeOut, app);
};

#endif  // FPDFSDK_JAVASCRIPT_APP_H_
