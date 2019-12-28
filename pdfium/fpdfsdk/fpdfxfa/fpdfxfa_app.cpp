// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fpdfxfa/include/fpdfxfa_app.h"

#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "xfa/fxbarcode/include/BC_Library.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_fontmgr.h"

CPDFXFA_App* CPDFXFA_App::g_pApp = nullptr;

CPDFXFA_App* CPDFXFA_App::GetInstance() {
  if (!g_pApp) {
    g_pApp = new CPDFXFA_App();
  }
  return g_pApp;
}

void CPDFXFA_App::ReleaseInstance() {
  delete g_pApp;
  g_pApp = nullptr;
}

CPDFXFA_App::CPDFXFA_App()
    : m_bJavaScriptInitialized(FALSE),
      m_pXFAApp(nullptr),
      m_pIsolate(nullptr),
      m_csAppType(JS_STR_VIEWERTYPE_STANDARD) {
  m_pEnvList.RemoveAll();
}

CPDFXFA_App::~CPDFXFA_App() {
  delete m_pXFAApp;
  m_pXFAApp = nullptr;

  FXJSE_Runtime_Release(m_pIsolate);
  m_pIsolate = nullptr;

  FXJSE_Finalize();
  BC_Library_Destory();
}

FX_BOOL CPDFXFA_App::Initialize(v8::Isolate* pIsolate) {
  BC_Library_Init();
  FXJSE_Initialize();

  m_pIsolate = pIsolate ? pIsolate : FXJSE_Runtime_Create_Own();
  if (!m_pIsolate)
    return FALSE;

  m_pXFAApp = new CXFA_FFApp(this);
  m_pXFAApp->SetDefaultFontMgr(
      std::unique_ptr<CXFA_DefFontMgr>(new CXFA_DefFontMgr));

  return TRUE;
}

FX_BOOL CPDFXFA_App::AddFormFillEnv(CPDFDoc_Environment* pEnv) {
  if (!pEnv)
    return FALSE;

  m_pEnvList.Add(pEnv);
  return TRUE;
}

FX_BOOL CPDFXFA_App::RemoveFormFillEnv(CPDFDoc_Environment* pEnv) {
  if (!pEnv)
    return FALSE;

  int nFind = m_pEnvList.Find(pEnv);
  if (nFind != -1) {
    m_pEnvList.RemoveAt(nFind);
    return TRUE;
  }

  return FALSE;
}

void CPDFXFA_App::GetAppType(CFX_WideString& wsAppType) {
  wsAppType = m_csAppType;
}

void CPDFXFA_App::GetAppName(CFX_WideString& wsName) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    wsName = pEnv->FFI_GetAppName();
  }
}

void CPDFXFA_App::GetFoxitAppName(CFX_WideString& wsFoxitName) {
  wsFoxitName = L"Foxit";
}

void CPDFXFA_App::SetAppType(const CFX_WideStringC& wsAppType) {
  m_csAppType = wsAppType;
}

void CPDFXFA_App::SetFoxitAppType(const CFX_WideStringC& wsFoxitAppType) {}

void CPDFXFA_App::GetFoxitAppType(CFX_WideString& wsFoxitAppType) {
  wsFoxitAppType.clear();
}

void CPDFXFA_App::GetLanguage(CFX_WideString& wsLanguage) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    wsLanguage = pEnv->FFI_GetLanguage();
  }
}

void CPDFXFA_App::GetPlatform(CFX_WideString& wsPlatform) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    wsPlatform = pEnv->FFI_GetPlatform();
  }
}

void CPDFXFA_App::GetVariation(CFX_WideString& wsVariation) {
  wsVariation = JS_STR_VIEWERVARIATION;
}

void CPDFXFA_App::GetVersion(CFX_WideString& wsVersion) {
  wsVersion = JS_STR_VIEWERVERSION_XFA;
}

void CPDFXFA_App::GetFoxitVersion(CFX_WideString& wsFoxitVersion) {
  wsFoxitVersion = L"7.0";
}

void CPDFXFA_App::Beep(uint32_t dwType) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    pEnv->JS_appBeep(dwType);
  }
}

int32_t CPDFXFA_App::MsgBox(const CFX_WideString& wsMessage,
                            const CFX_WideString& wsTitle,
                            uint32_t dwIconType,
                            uint32_t dwButtonType) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (!pEnv)
    return -1;

  uint32_t iconType = 0;
  int iButtonType = 0;
  switch (dwIconType) {
    case XFA_MBICON_Error:
      iconType |= 0;
      break;
    case XFA_MBICON_Warning:
      iconType |= 1;
      break;
    case XFA_MBICON_Question:
      iconType |= 2;
      break;
    case XFA_MBICON_Status:
      iconType |= 3;
      break;
  }
  switch (dwButtonType) {
    case XFA_MB_OK:
      iButtonType |= 0;
      break;
    case XFA_MB_OKCancel:
      iButtonType |= 1;
      break;
    case XFA_MB_YesNo:
      iButtonType |= 2;
      break;
    case XFA_MB_YesNoCancel:
      iButtonType |= 3;
      break;
  }
  int32_t iRet = pEnv->JS_appAlert(wsMessage.c_str(), wsTitle.c_str(),
                                   iButtonType, iconType);
  switch (iRet) {
    case 1:
      return XFA_IDOK;
    case 2:
      return XFA_IDCancel;
    case 3:
      return XFA_IDNo;
    case 4:
      return XFA_IDYes;
  }
  return XFA_IDYes;
}

CFX_WideString CPDFXFA_App::Response(const CFX_WideString& wsQuestion,
                                     const CFX_WideString& wsTitle,
                                     const CFX_WideString& wsDefaultAnswer,
                                     FX_BOOL bMark) {
  CFX_WideString wsAnswer;
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    int nLength = 2048;
    char* pBuff = new char[nLength];
    nLength = pEnv->JS_appResponse(wsQuestion.c_str(), wsTitle.c_str(),
                                   wsDefaultAnswer.c_str(), nullptr, bMark,
                                   pBuff, nLength);
    if (nLength > 0) {
      nLength = nLength > 2046 ? 2046 : nLength;
      pBuff[nLength] = 0;
      pBuff[nLength + 1] = 0;
      wsAnswer = CFX_WideString::FromUTF16LE(
          reinterpret_cast<const unsigned short*>(pBuff),
          nLength / sizeof(unsigned short));
    }
    delete[] pBuff;
  }
  return wsAnswer;
}

int32_t CPDFXFA_App::GetCurDocumentInBatch() {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    return pEnv->FFI_GetCurDocument();
  }
  return 0;
}

int32_t CPDFXFA_App::GetDocumentCountInBatch() {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv) {
    return pEnv->FFI_GetDocumentCount();
  }

  return 0;
}

IFX_FileRead* CPDFXFA_App::DownloadURL(const CFX_WideString& wsURL) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  return pEnv ? pEnv->FFI_DownloadFromURL(wsURL.c_str()) : nullptr;
}

FX_BOOL CPDFXFA_App::PostRequestURL(const CFX_WideString& wsURL,
                                    const CFX_WideString& wsData,
                                    const CFX_WideString& wsContentType,
                                    const CFX_WideString& wsEncode,
                                    const CFX_WideString& wsHeader,
                                    CFX_WideString& wsResponse) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (!pEnv)
    return FALSE;

  wsResponse = pEnv->FFI_PostRequestURL(wsURL.c_str(), wsData.c_str(),
                                        wsContentType.c_str(), wsEncode.c_str(),
                                        wsHeader.c_str());
  return TRUE;
}

FX_BOOL CPDFXFA_App::PutRequestURL(const CFX_WideString& wsURL,
                                   const CFX_WideString& wsData,
                                   const CFX_WideString& wsEncode) {
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  return pEnv &&
         pEnv->FFI_PutRequestURL(wsURL.c_str(), wsData.c_str(),
                                 wsEncode.c_str());
}

void CPDFXFA_App::LoadString(int32_t iStringID, CFX_WideString& wsString) {
  switch (iStringID) {
    case XFA_IDS_ValidateFailed:
      wsString = L"%s validation failed";
      return;
    case XFA_IDS_CalcOverride:
      wsString = L"Calculate Override";
      return;
    case XFA_IDS_ModifyField:
      wsString = L"Are you sure you want to modify this field?";
      return;
    case XFA_IDS_NotModifyField:
      wsString = L"You are not allowed to modify this field.";
      return;
    case XFA_IDS_AppName:
      wsString = L"pdfium";
      return;
    case XFA_IDS_Unable_TO_SET:
      wsString = L"Unable to set ";
      return;
    case XFA_IDS_INVAlID_PROP_SET:
      wsString = L"Invalid property set operation.";
      return;
    case XFA_IDS_NOT_DEFAUL_VALUE:
      wsString = L" doesn't have a default property.";
      return;
    case XFA_IDS_UNABLE_SET_LANGUAGE:
      wsString = L"Unable to set language value.";
      return;
    case XFA_IDS_UNABLE_SET_NUMPAGES:
      wsString = L"Unable to set numPages value.";
      return;
    case XFA_IDS_UNABLE_SET_PLATFORM:
      wsString = L"Unable to set platform value.";
      return;
    case XFA_IDS_UNABLE_SET_VARIATION:
      wsString = L"Unable to set variation value.";
      return;
    case XFA_IDS_UNABLE_SET_VERSION:
      wsString = L"Unable to set version value.";
      return;
    case XFA_IDS_UNABLE_SET_READY:
      wsString = L"Unable to set ready value.";
      return;
    case XFA_IDS_COMPILER_ERROR:
      wsString = L"Compiler error.";
      return;
    case XFA_IDS_DIVIDE_ZERO:
      wsString = L"Divide by zero.";
      return;
    case XFA_IDS_ACCESS_PROPERTY_IN_NOT_OBJECT:
      wsString =
          L"An attempt was made to reference property '%s' of a non-object in "
          L"SOM expression %s.";
      return;
    case XFA_IDS_INDEX_OUT_OF_BOUNDS:
      wsString = L"Index value is out of bounds.";
      return;
    case XFA_IDS_INCORRECT_NUMBER_OF_METHOD:
      wsString = L"Incorrect number of parameters calling method '%s'.";
      return;
    case XFA_IDS_ARGUMENT_MISMATCH:
      wsString = L"Argument mismatch in property or function argument.";
      return;
    case XFA_IDS_NOT_HAVE_PROPERTY:
      wsString = L"'%s' doesn't have property '%s'.";
      return;
    case XFA_IDS_VIOLATE_BOUNDARY:
      wsString =
          L"The element [%s] has violated its allowable number of occurrences.";
      return;
    case XFA_IDS_SERVER_DENY:
      wsString = L"Server does not permit.";
      return;
    case XFA_IDS_ValidateLimit:
      wsString =
          L"Message limit exceeded. Remaining %d validation errors not "
          L"reported.";
      return;
    case XFA_IDS_ValidateNullWarning:
      wsString =
          L"%s cannot be blank. To ignore validations for %s, click Ignore.";
      return;
    case XFA_IDS_ValidateNullError:
      wsString = L"%s cannot be blank.";
      return;
    case XFA_IDS_ValidateWarning:
      wsString =
          L"The value you entered for %s is invalid. To ignore validations for "
          L"%s, click Ignore.";
      return;
    case XFA_IDS_ValidateError:
      wsString = L"The value you entered for %s is invalid.";
      return;
  }
}

IFWL_AdapterTimerMgr* CPDFXFA_App::GetTimerMgr() {
  CXFA_FWLAdapterTimerMgr* pAdapter = nullptr;
  CPDFDoc_Environment* pEnv = m_pEnvList.GetAt(0);
  if (pEnv)
    pAdapter = new CXFA_FWLAdapterTimerMgr(pEnv);
  return pAdapter;
}
