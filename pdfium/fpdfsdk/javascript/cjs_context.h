// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_CJS_CONTEXT_H_
#define FPDFSDK_JAVASCRIPT_CJS_CONTEXT_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "fpdfsdk/javascript/ijs_context.h"

class CJS_EventHandler;
class CJS_Runtime;
class CPDFDoc_Environment;

class CJS_Context : public IJS_Context {
 public:
  explicit CJS_Context(CJS_Runtime* pRuntime);
  ~CJS_Context() override;

  // IJS_Context
  FX_BOOL RunScript(const CFX_WideString& script,
                    CFX_WideString* info) override;
  void OnApp_Init() override;
  void OnDoc_Open(CPDFSDK_Document* pDoc,
                  const CFX_WideString& strTargetName) override;
  void OnDoc_WillPrint(CPDFSDK_Document* pDoc) override;
  void OnDoc_DidPrint(CPDFSDK_Document* pDoc) override;
  void OnDoc_WillSave(CPDFSDK_Document* pDoc) override;
  void OnDoc_DidSave(CPDFSDK_Document* pDoc) override;
  void OnDoc_WillClose(CPDFSDK_Document* pDoc) override;
  void OnPage_Open(CPDFSDK_Document* pTarget) override;
  void OnPage_Close(CPDFSDK_Document* pTarget) override;
  void OnPage_InView(CPDFSDK_Document* pTarget) override;
  void OnPage_OutView(CPDFSDK_Document* pTarget) override;
  void OnField_MouseDown(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget) override;
  void OnField_MouseEnter(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDF_FormField* pTarget) override;
  void OnField_MouseExit(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget) override;
  void OnField_MouseUp(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDF_FormField* pTarget) override;
  void OnField_Focus(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDF_FormField* pTarget,
                     const CFX_WideString& Value) override;
  void OnField_Blur(FX_BOOL bModifier,
                    FX_BOOL bShift,
                    CPDF_FormField* pTarget,
                    const CFX_WideString& Value) override;
  void OnField_Calculate(CPDF_FormField* pSource,
                         CPDF_FormField* pTarget,
                         CFX_WideString& Value,
                         FX_BOOL& bRc) override;
  void OnField_Format(CPDF_FormField* pTarget,
                      CFX_WideString& Value,
                      FX_BOOL bWillCommit) override;
  void OnField_Keystroke(CFX_WideString& strChange,
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
                         FX_BOOL& bRc) override;
  void OnField_Validate(CFX_WideString& strChange,
                        const CFX_WideString& strChangeEx,
                        FX_BOOL bKeyDown,
                        FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDF_FormField* pTarget,
                        CFX_WideString& Value,
                        FX_BOOL& bRc) override;
  void OnScreen_Focus(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen) override;
  void OnScreen_Blur(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDFSDK_Annot* pScreen) override;
  void OnScreen_Open(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDFSDK_Annot* pScreen) override;
  void OnScreen_Close(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen) override;
  void OnScreen_MouseDown(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen) override;
  void OnScreen_MouseUp(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen) override;
  void OnScreen_MouseEnter(FX_BOOL bModifier,
                           FX_BOOL bShift,
                           CPDFSDK_Annot* pScreen) override;
  void OnScreen_MouseExit(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen) override;
  void OnScreen_InView(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDFSDK_Annot* pScreen) override;
  void OnScreen_OutView(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen) override;
  void OnBookmark_MouseUp(CPDF_Bookmark* pBookMark) override;
  void OnLink_MouseUp(CPDFSDK_Document* pTarget) override;
  void OnMenu_Exec(CPDFSDK_Document* pTarget,
                   const CFX_WideString& strTargetName) override;
  void OnBatchExec(CPDFSDK_Document* pTarget) override;
  void OnConsole_Exec() override;
  void OnExternal_Exec() override;
  void EnableMessageBox(FX_BOOL bEnable) override;

  FX_BOOL IsMsgBoxEnabled() const { return m_bMsgBoxEnable; }
  CJS_Runtime* GetJSRuntime() const { return m_pRuntime; }
  CJS_EventHandler* GetEventHandler() const { return m_pEventHandler; }

  CPDFDoc_Environment* GetReaderApp();
  CPDFSDK_Document* GetReaderDocument();

 private:
  CJS_Runtime* m_pRuntime;
  CJS_EventHandler* m_pEventHandler;
  FX_BOOL m_bBusy;
  FX_BOOL m_bMsgBoxEnable;
};

#endif  // FPDFSDK_JAVASCRIPT_CJS_CONTEXT_H_
