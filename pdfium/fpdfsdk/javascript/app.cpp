// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/app.h"

#include <memory>
#include <vector>

#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/Document.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"
#include "fpdfsdk/javascript/cjs_runtime.h"
#include "fpdfsdk/javascript/resource.h"

BEGIN_JS_STATIC_CONST(CJS_TimerObj)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_TimerObj)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_TimerObj)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_TimerObj, TimerObj)

TimerObj::TimerObj(CJS_Object* pJSObject)
    : CJS_EmbedObj(pJSObject), m_pTimer(nullptr) {}

TimerObj::~TimerObj() {}

void TimerObj::SetTimer(CJS_Timer* pTimer) {
  m_pTimer = pTimer;
}

CJS_Timer* TimerObj::GetTimer() const {
  return m_pTimer;
}

#define JS_STR_VIEWERTYPE L"pdfium"
#define JS_STR_VIEWERVARIATION L"Full"
#define JS_STR_PLATFORM L"WIN"
#define JS_STR_LANGUANGE L"ENU"
#define JS_NUM_VIEWERVERSION 8
#ifdef PDF_ENABLE_XFA
#define JS_NUM_VIEWERVERSION_XFA 11
#endif  // PDF_ENABLE_XFA
#define JS_NUM_FORMSVERSION 7

BEGIN_JS_STATIC_CONST(CJS_App)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_App)
JS_STATIC_PROP_ENTRY(activeDocs)
JS_STATIC_PROP_ENTRY(calculate)
JS_STATIC_PROP_ENTRY(formsVersion)
JS_STATIC_PROP_ENTRY(fs)
JS_STATIC_PROP_ENTRY(fullscreen)
JS_STATIC_PROP_ENTRY(language)
JS_STATIC_PROP_ENTRY(media)
JS_STATIC_PROP_ENTRY(platform)
JS_STATIC_PROP_ENTRY(runtimeHighlight)
JS_STATIC_PROP_ENTRY(viewerType)
JS_STATIC_PROP_ENTRY(viewerVariation)
JS_STATIC_PROP_ENTRY(viewerVersion)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_App)
JS_STATIC_METHOD_ENTRY(alert)
JS_STATIC_METHOD_ENTRY(beep)
JS_STATIC_METHOD_ENTRY(browseForDoc)
JS_STATIC_METHOD_ENTRY(clearInterval)
JS_STATIC_METHOD_ENTRY(clearTimeOut)
JS_STATIC_METHOD_ENTRY(execDialog)
JS_STATIC_METHOD_ENTRY(execMenuItem)
JS_STATIC_METHOD_ENTRY(findComponent)
JS_STATIC_METHOD_ENTRY(goBack)
JS_STATIC_METHOD_ENTRY(goForward)
JS_STATIC_METHOD_ENTRY(launchURL)
JS_STATIC_METHOD_ENTRY(mailMsg)
JS_STATIC_METHOD_ENTRY(newFDF)
JS_STATIC_METHOD_ENTRY(newDoc)
JS_STATIC_METHOD_ENTRY(openDoc)
JS_STATIC_METHOD_ENTRY(openFDF)
JS_STATIC_METHOD_ENTRY(popUpMenuEx)
JS_STATIC_METHOD_ENTRY(popUpMenu)
JS_STATIC_METHOD_ENTRY(response)
JS_STATIC_METHOD_ENTRY(setInterval)
JS_STATIC_METHOD_ENTRY(setTimeOut)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_App, app)

app::app(CJS_Object* pJSObject)
    : CJS_EmbedObj(pJSObject), m_bCalculate(true), m_bRuntimeHighLight(false) {}

app::~app() {
}

FX_BOOL app::activeDocs(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CPDFDoc_Environment* pApp = pContext->GetReaderApp();
  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  CPDFSDK_Document* pCurDoc = pContext->GetReaderDocument();
  CJS_Array aDocs(pRuntime);
  if (CPDFSDK_Document* pDoc = pApp->GetSDKDocument()) {
    CJS_Document* pJSDocument = nullptr;
    if (pDoc == pCurDoc) {
      v8::Local<v8::Object> pObj = FXJS_GetThisObj(pRuntime->GetIsolate());
      if (FXJS_GetObjDefnID(pObj) == CJS_Document::g_nObjDefnID)
        pJSDocument =
            (CJS_Document*)FXJS_GetPrivate(pRuntime->GetIsolate(), pObj);
    } else {
      v8::Local<v8::Object> pObj = FXJS_NewFxDynamicObj(
          pRuntime->GetIsolate(), pRuntime, CJS_Document::g_nObjDefnID);
      pJSDocument =
          (CJS_Document*)FXJS_GetPrivate(pRuntime->GetIsolate(), pObj);
      ASSERT(pJSDocument);
    }
    aDocs.SetElement(0, CJS_Value(pRuntime, pJSDocument));
  }
  if (aDocs.GetLength() > 0)
    vp << aDocs;
  else
    vp.SetNull();

  return TRUE;
}

FX_BOOL app::calculate(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError) {
  if (vp.IsSetting()) {
    bool bVP;
    vp >> bVP;
    m_bCalculate = (FX_BOOL)bVP;

    CJS_Context* pContext = (CJS_Context*)cc;
    CPDFDoc_Environment* pApp = pContext->GetReaderApp();
    CJS_Runtime* pRuntime = pContext->GetJSRuntime();
    CJS_Array aDocs(pRuntime);
    if (CPDFSDK_Document* pDoc = pApp->GetSDKDocument())
      pDoc->GetInterForm()->EnableCalculate((FX_BOOL)m_bCalculate);
  } else {
    vp << (bool)m_bCalculate;
  }
  return TRUE;
}

FX_BOOL app::formsVersion(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  if (vp.IsGetting()) {
    vp << JS_NUM_FORMSVERSION;
    return TRUE;
  }

  return FALSE;
}

FX_BOOL app::viewerType(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (vp.IsGetting()) {
    vp << JS_STR_VIEWERTYPE;
    return TRUE;
  }

  return FALSE;
}

FX_BOOL app::viewerVariation(IJS_Context* cc,
                             CJS_PropValue& vp,
                             CFX_WideString& sError) {
  if (vp.IsGetting()) {
    vp << JS_STR_VIEWERVARIATION;
    return TRUE;
  }

  return FALSE;
}

FX_BOOL app::viewerVersion(IJS_Context* cc,
                           CJS_PropValue& vp,
                           CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;
#ifdef PDF_ENABLE_XFA
  CJS_Context* pContext = (CJS_Context*)cc;
  CPDFSDK_Document* pCurDoc = pContext->GetReaderDocument();
  CPDFXFA_Document* pDoc = pCurDoc->GetXFADocument();
  if (pDoc->GetDocType() == 1 || pDoc->GetDocType() == 2) {
    vp << JS_NUM_VIEWERVERSION_XFA;
    return TRUE;
  }
#endif  // PDF_ENABLE_XFA
  vp << JS_NUM_VIEWERVERSION;
  return TRUE;
}

FX_BOOL app::platform(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;
#ifdef PDF_ENABLE_XFA
  CPDFDoc_Environment* pEnv =
      static_cast<CJS_Context*>(cc)->GetJSRuntime()->GetReaderApp();
  if (!pEnv)
    return FALSE;
  CFX_WideString platfrom = pEnv->FFI_GetPlatform();
  if (!platfrom.IsEmpty()) {
    vp << platfrom;
    return TRUE;
  }
#endif
  vp << JS_STR_PLATFORM;
  return TRUE;
}

FX_BOOL app::language(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;
#ifdef PDF_ENABLE_XFA
  CPDFDoc_Environment* pEnv =
      static_cast<CJS_Context*>(cc)->GetJSRuntime()->GetReaderApp();
  if (!pEnv)
    return FALSE;
  CFX_WideString language = pEnv->FFI_GetLanguage();
  if (!language.IsEmpty()) {
    vp << language;
    return TRUE;
  }
#endif
  vp << JS_STR_LANGUANGE;
  return TRUE;
}

// creates a new fdf object that contains no data
// comment: need reader support
// note:
// CFDF_Document * CPDFDoc_Environment::NewFDF();
FX_BOOL app::newFDF(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError) {
  return TRUE;
}
// opens a specified pdf document and returns its document object
// comment:need reader support
// note: as defined in js reference, the proto of this function's fourth
// parmeters, how old an fdf document while do not show it.
// CFDF_Document * CPDFDoc_Environment::OpenFDF(string strPath,bool bUserConv);

FX_BOOL app::openFDF(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL app::alert(IJS_Context* cc,
                   const std::vector<CJS_Value>& params,
                   CJS_Value& vRet,
                   CFX_WideString& sError) {
  CJS_Context* pContext = static_cast<CJS_Context*>(cc);
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  std::vector<CJS_Value> newParams = JS_ExpandKeywordParams(
      pRuntime, params, 4, L"cMsg", L"nIcon", L"nType", L"cTitle");

  if (newParams[0].GetType() == CJS_Value::VT_unknown) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CFX_WideString swMsg;
  if (newParams[0].GetType() == CJS_Value::VT_object) {
    CJS_Array carray(pRuntime);
    if (newParams[0].ConvertToArray(carray)) {
      swMsg = L"[";
      CJS_Value element(pRuntime);
      for (int i = 0; i < carray.GetLength(); ++i) {
        if (i)
          swMsg += L", ";
        carray.GetElement(i, element);
        swMsg += element.ToCFXWideString();
      }
      swMsg += L"]";
    } else {
      swMsg = newParams[0].ToCFXWideString();
    }
  } else {
    swMsg = newParams[0].ToCFXWideString();
  }

  int iIcon = 0;
  if (newParams[1].GetType() != CJS_Value::VT_unknown)
    iIcon = newParams[1].ToInt();

  int iType = 0;
  if (newParams[2].GetType() != CJS_Value::VT_unknown)
    iType = newParams[2].ToInt();

  CFX_WideString swTitle;
  if (newParams[3].GetType() != CJS_Value::VT_unknown)
    swTitle = newParams[3].ToCFXWideString();
  else
    swTitle = JSGetStringFromID(pContext, IDS_STRING_JSALERT);

  pRuntime->BeginBlock();
  vRet = MsgBox(pRuntime->GetReaderApp(), swMsg.c_str(), swTitle.c_str(), iType,
                iIcon);
  pRuntime->EndBlock();
  return TRUE;
}

FX_BOOL app::beep(IJS_Context* cc,
                  const std::vector<CJS_Value>& params,
                  CJS_Value& vRet,
                  CFX_WideString& sError) {
  if (params.size() == 1) {
    CJS_Context* pContext = (CJS_Context*)cc;
    CJS_Runtime* pRuntime = pContext->GetJSRuntime();
    CPDFDoc_Environment* pEnv = pRuntime->GetReaderApp();
    pEnv->JS_appBeep(params[0].ToInt());
    return TRUE;
  }

  sError = JSGetStringFromID((CJS_Context*)cc, IDS_STRING_JSPARAMERROR);
  return FALSE;
}

FX_BOOL app::findComponent(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL app::popUpMenuEx(IJS_Context* cc,
                         const std::vector<CJS_Value>& params,
                         CJS_Value& vRet,
                         CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::fs(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::setInterval(IJS_Context* cc,
                         const std::vector<CJS_Value>& params,
                         CJS_Value& vRet,
                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() > 2 || params.size() == 0) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CFX_WideString script = params.size() > 0 ? params[0].ToCFXWideString() : L"";
  if (script.IsEmpty()) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSAFNUMBER_KEYSTROKE);
    return TRUE;
  }

  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  uint32_t dwInterval = params.size() > 1 ? params[1].ToInt() : 1000;
  CPDFDoc_Environment* pApp = pRuntime->GetReaderApp();
  m_Timers.push_back(std::unique_ptr<CJS_Timer>(
      new CJS_Timer(this, pApp, pRuntime, 0, script, dwInterval, 0)));

  v8::Local<v8::Object> pRetObj = FXJS_NewFxDynamicObj(
      pRuntime->GetIsolate(), pRuntime, CJS_TimerObj::g_nObjDefnID);
  CJS_TimerObj* pJS_TimerObj = static_cast<CJS_TimerObj*>(
      FXJS_GetPrivate(pRuntime->GetIsolate(), pRetObj));
  TimerObj* pTimerObj = static_cast<TimerObj*>(pJS_TimerObj->GetEmbedObject());
  pTimerObj->SetTimer(m_Timers.back().get());

  vRet = pRetObj;
  return TRUE;
}

FX_BOOL app::setTimeOut(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError) {
  CJS_Context* pContext = static_cast<CJS_Context*>(cc);
  if (params.size() > 2 || params.size() == 0) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  CFX_WideString script = params[0].ToCFXWideString();
  if (script.IsEmpty()) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSAFNUMBER_KEYSTROKE);
    return TRUE;
  }

  uint32_t dwTimeOut = params.size() > 1 ? params[1].ToInt() : 1000;
  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  CPDFDoc_Environment* pApp = pRuntime->GetReaderApp();
  m_Timers.push_back(std::unique_ptr<CJS_Timer>(
      new CJS_Timer(this, pApp, pRuntime, 1, script, dwTimeOut, dwTimeOut)));

  v8::Local<v8::Object> pRetObj = FXJS_NewFxDynamicObj(
      pRuntime->GetIsolate(), pRuntime, CJS_TimerObj::g_nObjDefnID);

  CJS_TimerObj* pJS_TimerObj = static_cast<CJS_TimerObj*>(
      FXJS_GetPrivate(pRuntime->GetIsolate(), pRetObj));

  TimerObj* pTimerObj = static_cast<TimerObj*>(pJS_TimerObj->GetEmbedObject());
  pTimerObj->SetTimer(m_Timers.back().get());

  vRet = pRetObj;
  return TRUE;
}

FX_BOOL app::clearTimeOut(IJS_Context* cc,
                          const std::vector<CJS_Value>& params,
                          CJS_Value& vRet,
                          CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  app::ClearTimerCommon(params[0]);
  return TRUE;
}

FX_BOOL app::clearInterval(IJS_Context* cc,
                           const std::vector<CJS_Value>& params,
                           CJS_Value& vRet,
                           CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  if (params.size() != 1) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }

  app::ClearTimerCommon(params[0]);
  return TRUE;
}

void app::ClearTimerCommon(const CJS_Value& param) {
  if (param.GetType() != CJS_Value::VT_object)
    return;

  v8::Local<v8::Object> pObj = param.ToV8Object();
  if (FXJS_GetObjDefnID(pObj) != CJS_TimerObj::g_nObjDefnID)
    return;

  CJS_Object* pJSObj = param.ToCJSObject();
  if (!pJSObj)
    return;

  TimerObj* pTimerObj = static_cast<TimerObj*>(pJSObj->GetEmbedObject());
  if (!pTimerObj)
    return;

  CJS_Timer* pTimer = pTimerObj->GetTimer();
  if (!pTimer)
    return;

  pTimer->KillJSTimer();
  auto iter = std::find_if(m_Timers.begin(), m_Timers.end(),
                           [pTimer](const std::unique_ptr<CJS_Timer>& that) {
                             return pTimer == that.get();
                           });

  if (iter != m_Timers.end())
    m_Timers.erase(iter);

  pTimerObj->SetTimer(nullptr);
}

FX_BOOL app::execMenuItem(IJS_Context* cc,
                          const std::vector<CJS_Value>& params,
                          CJS_Value& vRet,
                          CFX_WideString& sError) {
  return FALSE;
}

void app::TimerProc(CJS_Timer* pTimer) {
  CJS_Runtime* pRuntime = pTimer->GetRuntime();

  switch (pTimer->GetType()) {
    case 0:  // interval
      if (pRuntime)
        RunJsScript(pRuntime, pTimer->GetJScript());
      break;
    case 1:
      if (pTimer->GetTimeOut() > 0) {
        if (pRuntime)
          RunJsScript(pRuntime, pTimer->GetJScript());
        pTimer->KillJSTimer();
      }
      break;
  }
}

void app::RunJsScript(CJS_Runtime* pRuntime, const CFX_WideString& wsScript) {
  if (!pRuntime->IsBlocking()) {
    IJS_Context* pContext = pRuntime->NewContext();
    pContext->OnExternal_Exec();
    CFX_WideString wtInfo;
    pContext->RunScript(wsScript, &wtInfo);
    pRuntime->ReleaseContext(pContext);
  }
}

FX_BOOL app::goBack(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError) {
  // Not supported.
  return TRUE;
}

FX_BOOL app::goForward(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  // Not supported.
  return TRUE;
}

FX_BOOL app::mailMsg(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError) {
  CJS_Context* pContext = static_cast<CJS_Context*>(cc);
  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  std::vector<CJS_Value> newParams =
      JS_ExpandKeywordParams(pRuntime, params, 6, L"bUI", L"cTo", L"cCc",
                             L"cBcc", L"cSubject", L"cMsg");

  if (newParams[0].GetType() == CJS_Value::VT_unknown) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }
  bool bUI = newParams[0].ToBool();

  CFX_WideString cTo;
  if (newParams[1].GetType() != CJS_Value::VT_unknown) {
    cTo = newParams[1].ToCFXWideString();
  } else {
    if (!bUI) {
      // cTo parameter required when UI not invoked.
      sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
      return FALSE;
    }
  }

  CFX_WideString cCc;
  if (newParams[2].GetType() != CJS_Value::VT_unknown)
    cCc = newParams[2].ToCFXWideString();

  CFX_WideString cBcc;
  if (newParams[3].GetType() != CJS_Value::VT_unknown)
    cBcc = newParams[3].ToCFXWideString();

  CFX_WideString cSubject;
  if (newParams[4].GetType() != CJS_Value::VT_unknown)
    cSubject = newParams[4].ToCFXWideString();

  CFX_WideString cMsg;
  if (newParams[5].GetType() != CJS_Value::VT_unknown)
    cMsg = newParams[5].ToCFXWideString();

  pRuntime->BeginBlock();
  pContext->GetReaderApp()->JS_docmailForm(nullptr, 0, bUI, cTo.c_str(),
                                           cSubject.c_str(), cCc.c_str(),
                                           cBcc.c_str(), cMsg.c_str());
  pRuntime->EndBlock();
  return TRUE;
}

FX_BOOL app::launchURL(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  // Unsafe, not supported.
  return TRUE;
}

FX_BOOL app::runtimeHighlight(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError) {
  if (vp.IsSetting()) {
    vp >> m_bRuntimeHighLight;
  } else {
    vp << m_bRuntimeHighLight;
  }
  return TRUE;
}

FX_BOOL app::fullscreen(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::popUpMenu(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::browseForDoc(IJS_Context* cc,
                          const std::vector<CJS_Value>& params,
                          CJS_Value& vRet,
                          CFX_WideString& sError) {
  // Unsafe, not supported.
  return TRUE;
}

CFX_WideString app::SysPathToPDFPath(const CFX_WideString& sOldPath) {
  CFX_WideString sRet = L"/";

  for (int i = 0, sz = sOldPath.GetLength(); i < sz; i++) {
    wchar_t c = sOldPath.GetAt(i);
    if (c == L':') {
    } else {
      if (c == L'\\') {
        sRet += L"/";
      } else {
        sRet += c;
      }
    }
  }

  return sRet;
}

FX_BOOL app::newDoc(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::openDoc(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::response(IJS_Context* cc,
                      const std::vector<CJS_Value>& params,
                      CJS_Value& vRet,
                      CFX_WideString& sError) {
  CJS_Context* pContext = static_cast<CJS_Context*>(cc);
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  std::vector<CJS_Value> newParams =
      JS_ExpandKeywordParams(pRuntime, params, 5, L"cQuestion", L"cTitle",
                             L"cDefault", L"bPassword", L"cLabel");

  if (newParams[0].GetType() == CJS_Value::VT_unknown) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAMERROR);
    return FALSE;
  }
  CFX_WideString swQuestion = newParams[0].ToCFXWideString();

  CFX_WideString swTitle = L"PDF";
  if (newParams[1].GetType() != CJS_Value::VT_unknown)
    swTitle = newParams[1].ToCFXWideString();

  CFX_WideString swDefault;
  if (newParams[2].GetType() != CJS_Value::VT_unknown)
    swDefault = newParams[2].ToCFXWideString();

  bool bPassword = false;
  if (newParams[3].GetType() != CJS_Value::VT_unknown)
    bPassword = newParams[3].ToBool();

  CFX_WideString swLabel;
  if (newParams[4].GetType() != CJS_Value::VT_unknown)
    swLabel = newParams[4].ToCFXWideString();

  const int MAX_INPUT_BYTES = 2048;
  std::unique_ptr<char[]> pBuff(new char[MAX_INPUT_BYTES + 2]);
  memset(pBuff.get(), 0, MAX_INPUT_BYTES + 2);

  int nLengthBytes = pContext->GetReaderApp()->JS_appResponse(
      swQuestion.c_str(), swTitle.c_str(), swDefault.c_str(), swLabel.c_str(),
      bPassword, pBuff.get(), MAX_INPUT_BYTES);

  if (nLengthBytes < 0 || nLengthBytes > MAX_INPUT_BYTES) {
    sError = JSGetStringFromID(pContext, IDS_STRING_JSPARAM_TOOLONG);
    return FALSE;
  }

  vRet = CFX_WideString::FromUTF16LE(reinterpret_cast<uint16_t*>(pBuff.get()),
                                     nLengthBytes / sizeof(uint16_t))
             .c_str();
  return TRUE;
}

FX_BOOL app::media(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL app::execDialog(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError) {
  return TRUE;
}
