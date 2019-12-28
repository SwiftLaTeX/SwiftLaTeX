// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/cjs_context.h"

#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/cjs_runtime.h"
#include "fpdfsdk/javascript/resource.h"

CJS_Context::CJS_Context(CJS_Runtime* pRuntime)
    : m_pRuntime(pRuntime), m_bBusy(FALSE), m_bMsgBoxEnable(TRUE) {
  m_pEventHandler = new CJS_EventHandler(this);
}

CJS_Context::~CJS_Context() {
  delete m_pEventHandler;
}

CPDFSDK_Document* CJS_Context::GetReaderDocument() {
  return m_pRuntime->GetReaderDocument();
}

CPDFDoc_Environment* CJS_Context::GetReaderApp() {
  return m_pRuntime->GetReaderApp();
}

FX_BOOL CJS_Context::RunScript(const CFX_WideString& script,
                               CFX_WideString* info) {
  v8::Isolate::Scope isolate_scope(m_pRuntime->GetIsolate());
  v8::HandleScope handle_scope(m_pRuntime->GetIsolate());
  v8::Local<v8::Context> context = m_pRuntime->NewJSContext();
  v8::Context::Scope context_scope(context);

  if (m_bBusy) {
    *info = JSGetStringFromID(this, IDS_STRING_JSBUSY);
    return FALSE;
  }
  m_bBusy = TRUE;

  ASSERT(m_pEventHandler->IsValid());
  CJS_Runtime::FieldEvent event(m_pEventHandler->TargetName(),
                                m_pEventHandler->EventType());
  if (!m_pRuntime->AddEventToSet(event)) {
    *info = JSGetStringFromID(this, IDS_STRING_JSEVENT);
    return FALSE;
  }

  CFX_WideString sErrorMessage;
  int nRet = 0;
  if (script.GetLength() > 0) {
    nRet = m_pRuntime->Execute(script.c_str(), &sErrorMessage);
  }

  if (nRet < 0) {
    *info += sErrorMessage;
  } else {
    *info = JSGetStringFromID(this, IDS_STRING_RUN);
  }

  m_pRuntime->RemoveEventFromSet(event);
  m_pEventHandler->Destroy();
  m_bBusy = FALSE;

  return nRet >= 0;
}

void CJS_Context::OnApp_Init() {
  m_pEventHandler->OnApp_Init();
}

void CJS_Context::OnDoc_Open(CPDFSDK_Document* pDoc,
                             const CFX_WideString& strTargetName) {
  m_pEventHandler->OnDoc_Open(pDoc, strTargetName);
}

void CJS_Context::OnDoc_WillPrint(CPDFSDK_Document* pDoc) {
  m_pEventHandler->OnDoc_WillPrint(pDoc);
}

void CJS_Context::OnDoc_DidPrint(CPDFSDK_Document* pDoc) {
  m_pEventHandler->OnDoc_DidPrint(pDoc);
}

void CJS_Context::OnDoc_WillSave(CPDFSDK_Document* pDoc) {
  m_pEventHandler->OnDoc_WillSave(pDoc);
}

void CJS_Context::OnDoc_DidSave(CPDFSDK_Document* pDoc) {
  m_pEventHandler->OnDoc_DidSave(pDoc);
}

void CJS_Context::OnDoc_WillClose(CPDFSDK_Document* pDoc) {
  m_pEventHandler->OnDoc_WillClose(pDoc);
}

void CJS_Context::OnPage_Open(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnPage_Open(pTarget);
}

void CJS_Context::OnPage_Close(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnPage_Close(pTarget);
}

void CJS_Context::OnPage_InView(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnPage_InView(pTarget);
}

void CJS_Context::OnPage_OutView(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnPage_OutView(pTarget);
}

void CJS_Context::OnField_MouseDown(FX_BOOL bModifier,
                                    FX_BOOL bShift,
                                    CPDF_FormField* pTarget) {
  m_pEventHandler->OnField_MouseDown(bModifier, bShift, pTarget);
}

void CJS_Context::OnField_MouseEnter(FX_BOOL bModifier,
                                     FX_BOOL bShift,
                                     CPDF_FormField* pTarget) {
  m_pEventHandler->OnField_MouseEnter(bModifier, bShift, pTarget);
}

void CJS_Context::OnField_MouseExit(FX_BOOL bModifier,
                                    FX_BOOL bShift,
                                    CPDF_FormField* pTarget) {
  m_pEventHandler->OnField_MouseExit(bModifier, bShift, pTarget);
}

void CJS_Context::OnField_MouseUp(FX_BOOL bModifier,
                                  FX_BOOL bShift,
                                  CPDF_FormField* pTarget) {
  m_pEventHandler->OnField_MouseUp(bModifier, bShift, pTarget);
}

void CJS_Context::OnField_Focus(FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDF_FormField* pTarget,
                                const CFX_WideString& Value) {
  m_pEventHandler->OnField_Focus(bModifier, bShift, pTarget, Value);
}

void CJS_Context::OnField_Blur(FX_BOOL bModifier,
                               FX_BOOL bShift,
                               CPDF_FormField* pTarget,
                               const CFX_WideString& Value) {
  m_pEventHandler->OnField_Blur(bModifier, bShift, pTarget, Value);
}

void CJS_Context::OnField_Calculate(CPDF_FormField* pSource,
                                    CPDF_FormField* pTarget,
                                    CFX_WideString& Value,
                                    FX_BOOL& bRc) {
  m_pEventHandler->OnField_Calculate(pSource, pTarget, Value, bRc);
}

void CJS_Context::OnField_Format(CPDF_FormField* pTarget,
                                 CFX_WideString& Value,
                                 FX_BOOL bWillCommit) {
  m_pEventHandler->OnField_Format(pTarget, Value, bWillCommit);
}

void CJS_Context::OnField_Keystroke(CFX_WideString& strChange,
                                    const CFX_WideString& strChangeEx,
                                    FX_BOOL bKeyDown,
                                    FX_BOOL bModifier,
                                    int& nSelEnd,
                                    int& nSelStart,
                                    FX_BOOL bShift,
                                    CPDF_FormField* pTarget,
                                    CFX_WideString& Value,
                                    FX_BOOL bWillCommit,
                                    FX_BOOL bFieldFull,
                                    FX_BOOL& bRc) {
  m_pEventHandler->OnField_Keystroke(
      strChange, strChangeEx, bKeyDown, bModifier, nSelEnd, nSelStart, bShift,
      pTarget, Value, bWillCommit, bFieldFull, bRc);
}

void CJS_Context::OnField_Validate(CFX_WideString& strChange,
                                   const CFX_WideString& strChangeEx,
                                   FX_BOOL bKeyDown,
                                   FX_BOOL bModifier,
                                   FX_BOOL bShift,
                                   CPDF_FormField* pTarget,
                                   CFX_WideString& Value,
                                   FX_BOOL& bRc) {
  m_pEventHandler->OnField_Validate(strChange, strChangeEx, bKeyDown, bModifier,
                                    bShift, pTarget, Value, bRc);
}

void CJS_Context::OnScreen_Focus(FX_BOOL bModifier,
                                 FX_BOOL bShift,
                                 CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_Focus(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_Blur(FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_Blur(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_Open(FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_Open(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_Close(FX_BOOL bModifier,
                                 FX_BOOL bShift,
                                 CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_Close(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_MouseDown(FX_BOOL bModifier,
                                     FX_BOOL bShift,
                                     CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_MouseDown(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_MouseUp(FX_BOOL bModifier,
                                   FX_BOOL bShift,
                                   CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_MouseUp(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_MouseEnter(FX_BOOL bModifier,
                                      FX_BOOL bShift,
                                      CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_MouseEnter(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_MouseExit(FX_BOOL bModifier,
                                     FX_BOOL bShift,
                                     CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_MouseExit(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_InView(FX_BOOL bModifier,
                                  FX_BOOL bShift,
                                  CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_InView(bModifier, bShift, pScreen);
}

void CJS_Context::OnScreen_OutView(FX_BOOL bModifier,
                                   FX_BOOL bShift,
                                   CPDFSDK_Annot* pScreen) {
  m_pEventHandler->OnScreen_OutView(bModifier, bShift, pScreen);
}

void CJS_Context::OnBookmark_MouseUp(CPDF_Bookmark* pBookMark) {
  m_pEventHandler->OnBookmark_MouseUp(pBookMark);
}

void CJS_Context::OnLink_MouseUp(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnLink_MouseUp(pTarget);
}

void CJS_Context::OnConsole_Exec() {
  m_pEventHandler->OnConsole_Exec();
}

void CJS_Context::OnExternal_Exec() {
  m_pEventHandler->OnExternal_Exec();
}

void CJS_Context::EnableMessageBox(FX_BOOL bEnable) {
  m_bMsgBoxEnable = bEnable;
}

void CJS_Context::OnBatchExec(CPDFSDK_Document* pTarget) {
  m_pEventHandler->OnBatchExec(pTarget);
}

void CJS_Context::OnMenu_Exec(CPDFSDK_Document* pTarget,
                              const CFX_WideString& strTargetName) {
  m_pEventHandler->OnMenu_Exec(pTarget, strTargetName);
}
