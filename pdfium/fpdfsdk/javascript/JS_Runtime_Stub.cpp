// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <memory>

#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/ijs_context.h"
#include "fpdfsdk/javascript/ijs_runtime.h"

class CJS_ContextStub final : public IJS_Context {
 public:
  CJS_ContextStub() {}
  ~CJS_ContextStub() override {}

  // IJS_Context:
  FX_BOOL RunScript(const CFX_WideString& script,
                    CFX_WideString* info) override {
    return FALSE;
  }

  void OnApp_Init() override {}
  void OnDoc_Open(CPDFSDK_Document* pDoc,
                  const CFX_WideString& strTargetName) override {}
  void OnDoc_WillPrint(CPDFSDK_Document* pDoc) override {}
  void OnDoc_DidPrint(CPDFSDK_Document* pDoc) override {}
  void OnDoc_WillSave(CPDFSDK_Document* pDoc) override {}
  void OnDoc_DidSave(CPDFSDK_Document* pDoc) override {}
  void OnDoc_WillClose(CPDFSDK_Document* pDoc) override {}
  void OnPage_Open(CPDFSDK_Document* pTarget) override {}
  void OnPage_Close(CPDFSDK_Document* pTarget) override {}
  void OnPage_InView(CPDFSDK_Document* pTarget) override {}
  void OnPage_OutView(CPDFSDK_Document* pTarget) override {}
  void OnField_MouseDown(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget) override {}
  void OnField_MouseEnter(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDF_FormField* pTarget) override {}
  void OnField_MouseExit(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget) override {}
  void OnField_MouseUp(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDF_FormField* pTarget) override {}
  void OnField_Focus(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDF_FormField* pTarget,
                     const CFX_WideString& Value) override {}
  void OnField_Blur(FX_BOOL bModifier,
                    FX_BOOL bShift,
                    CPDF_FormField* pTarget,
                    const CFX_WideString& Value) override {}
  void OnField_Calculate(CPDF_FormField* pSource,
                         CPDF_FormField* pTarget,
                         CFX_WideString& Value,
                         FX_BOOL& bRc) override {}
  void OnField_Format(CPDF_FormField* pTarget,
                      CFX_WideString& Value,
                      FX_BOOL bWillCommit) override {}
  void OnField_Keystroke(CFX_WideString& strChange,
                         const CFX_WideString& strChangeEx,
                         FX_BOOL KeyDown,
                         FX_BOOL bModifier,
                         int& nSelEnd,
                         int& nSelStart,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget,
                         CFX_WideString& Value,
                         FX_BOOL bWillCommit,
                         FX_BOOL bFieldFull,
                         FX_BOOL& bRc) override {}
  void OnField_Validate(CFX_WideString& strChange,
                        const CFX_WideString& strChangeEx,
                        FX_BOOL bKeyDown,
                        FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDF_FormField* pTarget,
                        CFX_WideString& Value,
                        FX_BOOL& bRc) override {}
  void OnScreen_Focus(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Blur(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Open(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDFSDK_Annot* pScreen) override {}
  void OnScreen_Close(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseDown(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseUp(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseEnter(FX_BOOL bModifier,
                           FX_BOOL bShift,
                           CPDFSDK_Annot* pScreen) override {}
  void OnScreen_MouseExit(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen) override {}
  void OnScreen_InView(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDFSDK_Annot* pScreen) override {}
  void OnScreen_OutView(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen) override {}
  void OnBookmark_MouseUp(CPDF_Bookmark* pBookMark) override {}
  void OnLink_MouseUp(CPDFSDK_Document* pTarget) override {}
  void OnMenu_Exec(CPDFSDK_Document* pTarget, const CFX_WideString&) override {}
  void OnBatchExec(CPDFSDK_Document* pTarget) override {}
  void OnConsole_Exec() override {}
  void OnExternal_Exec() override {}
  void EnableMessageBox(FX_BOOL bEnable) override {}
};

class CJS_RuntimeStub final : public IJS_Runtime {
 public:
  CJS_RuntimeStub() : m_pDoc(nullptr) {}
  ~CJS_RuntimeStub() override {}

  IJS_Context* NewContext() override {
    if (!m_pContext)
      m_pContext.reset(new CJS_ContextStub());
    return GetCurrentContext();
  }

  IJS_Context* GetCurrentContext() override { return m_pContext.get(); }
  void ReleaseContext(IJS_Context* pContext) override {}

  void SetReaderDocument(CPDFSDK_Document* pReaderDoc) override {
    m_pDoc = pReaderDoc;
  }
  CPDFSDK_Document* GetReaderDocument() override { return m_pDoc; }

#ifdef PDF_ENABLE_XFA
  FX_BOOL GetValueByName(const CFX_ByteStringC&, CFXJSE_Value*) override {
    return FALSE;
  }

  FX_BOOL SetValueByName(const CFX_ByteStringC&, CFXJSE_Value*) override {
    return FALSE;
  }
#endif  // PDF_ENABLE_XFA

  int Execute(const CFX_WideString& script, CFX_WideString* info) override {
    return 0;
  }

 protected:
  CPDFSDK_Document* m_pDoc;
  std::unique_ptr<CJS_ContextStub> m_pContext;
};

// static
void IJS_Runtime::Initialize(unsigned int slot, void* isolate) {}

// static
void IJS_Runtime::Destroy() {}

// static
IJS_Runtime* IJS_Runtime::Create(CPDFDoc_Environment* pEnv) {
  return new CJS_RuntimeStub;
}
