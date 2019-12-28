// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_IJS_CONTEXT_H_
#define FPDFSDK_JAVASCRIPT_IJS_CONTEXT_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Bookmark;
class CPDF_FormField;
class CPDFSDK_Annot;
class CPDFSDK_Document;

// Records the details of an event and triggers JS execution for it.
class IJS_Context {
 public:
  virtual FX_BOOL RunScript(const CFX_WideString& script,
                            CFX_WideString* info) = 0;

  virtual void OnApp_Init() = 0;

  virtual void OnDoc_Open(CPDFSDK_Document* pDoc,
                          const CFX_WideString& strTargetName) = 0;
  virtual void OnDoc_WillPrint(CPDFSDK_Document* pDoc) = 0;
  virtual void OnDoc_DidPrint(CPDFSDK_Document* pDoc) = 0;
  virtual void OnDoc_WillSave(CPDFSDK_Document* pDoc) = 0;
  virtual void OnDoc_DidSave(CPDFSDK_Document* pDoc) = 0;
  virtual void OnDoc_WillClose(CPDFSDK_Document* pDoc) = 0;

  virtual void OnPage_Open(CPDFSDK_Document* pTarget) = 0;
  virtual void OnPage_Close(CPDFSDK_Document* pTarget) = 0;
  virtual void OnPage_InView(CPDFSDK_Document* pTarget) = 0;
  virtual void OnPage_OutView(CPDFSDK_Document* pTarget) = 0;

  virtual void OnField_MouseDown(FX_BOOL bModifier,
                                 FX_BOOL bShift,
                                 CPDF_FormField* pTarget) = 0;
  virtual void OnField_MouseEnter(FX_BOOL bModifier,
                                  FX_BOOL bShift,
                                  CPDF_FormField* pTarget) = 0;
  virtual void OnField_MouseExit(FX_BOOL bModifier,
                                 FX_BOOL bShift,
                                 CPDF_FormField* pTarget) = 0;
  virtual void OnField_MouseUp(FX_BOOL bModifier,
                               FX_BOOL bShift,
                               CPDF_FormField* pTarget) = 0;
  virtual void OnField_Focus(FX_BOOL bModifier,
                             FX_BOOL bShift,
                             CPDF_FormField* pTarget,
                             const CFX_WideString& Value) = 0;
  virtual void OnField_Blur(FX_BOOL bModifier,
                            FX_BOOL bShift,
                            CPDF_FormField* pTarget,
                            const CFX_WideString& Value) = 0;

  virtual void OnField_Calculate(CPDF_FormField* pSource,
                                 CPDF_FormField* pTarget,
                                 CFX_WideString& Value,
                                 FX_BOOL& bRc) = 0;
  virtual void OnField_Format(CPDF_FormField* pTarget,
                              CFX_WideString& Value,
                              FX_BOOL bWillCommit) = 0;
  virtual void OnField_Keystroke(CFX_WideString& strChange,
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
                                 FX_BOOL& bRc) = 0;
  virtual void OnField_Validate(CFX_WideString& strChange,
                                const CFX_WideString& strChangeEx,
                                FX_BOOL bKeyDown,
                                FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDF_FormField* pTarget,
                                CFX_WideString& Value,
                                FX_BOOL& bRc) = 0;

  virtual void OnScreen_Focus(FX_BOOL bModifier,
                              FX_BOOL bShift,
                              CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_Blur(FX_BOOL bModifier,
                             FX_BOOL bShift,
                             CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_Open(FX_BOOL bModifier,
                             FX_BOOL bShift,
                             CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_Close(FX_BOOL bModifier,
                              FX_BOOL bShift,
                              CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_MouseDown(FX_BOOL bModifier,
                                  FX_BOOL bShift,
                                  CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_MouseUp(FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_MouseEnter(FX_BOOL bModifier,
                                   FX_BOOL bShift,
                                   CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_MouseExit(FX_BOOL bModifier,
                                  FX_BOOL bShift,
                                  CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_InView(FX_BOOL bModifier,
                               FX_BOOL bShift,
                               CPDFSDK_Annot* pScreen) = 0;
  virtual void OnScreen_OutView(FX_BOOL bModifier,
                                FX_BOOL bShift,
                                CPDFSDK_Annot* pScreen) = 0;

  virtual void OnBookmark_MouseUp(CPDF_Bookmark* pBookMark) = 0;
  virtual void OnLink_MouseUp(CPDFSDK_Document* pTarget) = 0;

  virtual void OnMenu_Exec(CPDFSDK_Document* pTarget,
                           const CFX_WideString&) = 0;
  virtual void OnBatchExec(CPDFSDK_Document* pTarget) = 0;
  virtual void OnConsole_Exec() = 0;
  virtual void OnExternal_Exec() = 0;

  virtual void EnableMessageBox(FX_BOOL bEnable) = 0;

 protected:
  virtual ~IJS_Context() {}
};

#endif  // FPDFSDK_JAVASCRIPT_IJS_CONTEXT_H_
