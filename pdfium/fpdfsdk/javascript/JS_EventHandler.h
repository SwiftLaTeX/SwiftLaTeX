// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_JS_EVENTHANDLER_H_
#define FPDFSDK_JAVASCRIPT_JS_EVENTHANDLER_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CJS_Context;
class CPDFSDK_Annot;
class CPDFSDK_Document;
class CPDF_Bookmark;
class CPDF_FormField;
class Field;

enum JS_EVENT_T {
  JET_UNKNOWN,
  JET_APP_INIT,
  JET_DOC_OPEN,
  JET_DOC_WILLPRINT,
  JET_DOC_DIDPRINT,
  JET_DOC_WILLSAVE,
  JET_DOC_DIDSAVE,
  JET_DOC_WILLCLOSE,
  JET_PAGE_OPEN,
  JET_PAGE_CLOSE,
  JET_PAGE_INVIEW,
  JET_PAGE_OUTVIEW,
  JET_FIELD_MOUSEDOWN,
  JET_FIELD_MOUSEUP,
  JET_FIELD_MOUSEENTER,
  JET_FIELD_MOUSEEXIT,
  JET_FIELD_FOCUS,
  JET_FIELD_BLUR,
  JET_FIELD_KEYSTROKE,
  JET_FIELD_VALIDATE,
  JET_FIELD_CALCULATE,
  JET_FIELD_FORMAT,
  JET_SCREEN_FOCUS,
  JET_SCREEN_BLUR,
  JET_SCREEN_OPEN,
  JET_SCREEN_CLOSE,
  JET_SCREEN_MOUSEDOWN,
  JET_SCREEN_MOUSEUP,
  JET_SCREEN_MOUSEENTER,
  JET_SCREEN_MOUSEEXIT,
  JET_SCREEN_INVIEW,
  JET_SCREEN_OUTVIEW,
  JET_BATCH_EXEC,
  JET_MENU_EXEC,
  JET_CONSOLE_EXEC,
  JET_EXTERNAL_EXEC,
  JET_BOOKMARK_MOUSEUP,
  JET_LINK_MOUSEUP
};

class CJS_EventHandler {
 public:
  CJS_EventHandler(CJS_Context* pContext);
  virtual ~CJS_EventHandler();

  void OnApp_Init();

  void OnDoc_Open(CPDFSDK_Document* pDoc, const CFX_WideString& strTargetName);
  void OnDoc_WillPrint(CPDFSDK_Document* pDoc);
  void OnDoc_DidPrint(CPDFSDK_Document* pDoc);
  void OnDoc_WillSave(CPDFSDK_Document* pDoc);
  void OnDoc_DidSave(CPDFSDK_Document* pDoc);
  void OnDoc_WillClose(CPDFSDK_Document* pDoc);

  void OnPage_Open(CPDFSDK_Document* pDoc);
  void OnPage_Close(CPDFSDK_Document* pDoc);
  void OnPage_InView(CPDFSDK_Document* pTarget);
  void OnPage_OutView(CPDFSDK_Document* pTarget);

  void OnField_Calculate(CPDF_FormField* pSource,
                         CPDF_FormField* pTarget,
                         CFX_WideString& Value,
                         FX_BOOL& bRc);
  void OnField_Format(CPDF_FormField* pTarget,
                      CFX_WideString& Value,
                      FX_BOOL bWillCommit);
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
                         FX_BOOL& bRc);
  void OnField_Validate(CFX_WideString& strChange,
                        const CFX_WideString& strChangeEx,
                        FX_BOOL bKeyDown,
                        FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDF_FormField* pTarget,
                        CFX_WideString& Value,
                        FX_BOOL& bRc);

  void OnField_MouseDown(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget);
  void OnField_MouseEnter(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDF_FormField* pTarget);
  void OnField_MouseExit(FX_BOOL bModifier,
                         FX_BOOL bShift,
                         CPDF_FormField* pTarget);
  void OnField_MouseUp(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDF_FormField* pTarget);
  void OnField_Blur(FX_BOOL bModifier,
                    FX_BOOL bShift,
                    CPDF_FormField* pTarget,
                    const CFX_WideString& Value);
  void OnField_Focus(FX_BOOL bModifier,
                     FX_BOOL bShift,
                     CPDF_FormField* pTarget,
                     const CFX_WideString& Value);

  void OnScreen_Focus(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen);
  void OnScreen_Blur(FX_BOOL bModifier, FX_BOOL bShift, CPDFSDK_Annot* pScreen);
  void OnScreen_Open(FX_BOOL bModifier, FX_BOOL bShift, CPDFSDK_Annot* pScreen);
  void OnScreen_Close(FX_BOOL bModifier,
                      FX_BOOL bShift,
                      CPDFSDK_Annot* pScreen);
  void OnScreen_MouseDown(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen);
  void OnScreen_MouseUp(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen);
  void OnScreen_MouseEnter(FX_BOOL bModifier,
                           FX_BOOL bShift,
                           CPDFSDK_Annot* pScreen);
  void OnScreen_MouseExit(FX_BOOL bModifier,
                          FX_BOOL bShift,
                          CPDFSDK_Annot* pScreen);
  void OnScreen_InView(FX_BOOL bModifier,
                       FX_BOOL bShift,
                       CPDFSDK_Annot* pScreen);
  void OnScreen_OutView(FX_BOOL bModifier,
                        FX_BOOL bShift,
                        CPDFSDK_Annot* pScreen);

  void OnBookmark_MouseUp(CPDF_Bookmark* pBookMark);
  void OnLink_MouseUp(CPDFSDK_Document* pTarget);

  void OnMenu_Exec(CPDFSDK_Document* pTarget,
                   const CFX_WideString& strTargetName);
  void OnBatchExec(CPDFSDK_Document* pTarget);
  void OnConsole_Exec();
  void OnExternal_Exec();

 public:
  void Initial(JS_EVENT_T type);
  void Destroy();
  FX_BOOL IsValid();

  CFX_WideString& Change();
  CFX_WideString ChangeEx();
  int CommitKey();
  FX_BOOL FieldFull();
  FX_BOOL KeyDown();
  FX_BOOL Modifier();
  const FX_WCHAR* Name();
  const FX_WCHAR* Type();
  FX_BOOL& Rc();
  int& SelEnd();
  int& SelStart();
  FX_BOOL Shift();
  Field* Source();
  Field* Target_Field();
  CFX_WideString& Value();
  FX_BOOL WillCommit();
  CFX_WideString TargetName();

  JS_EVENT_T EventType() { return m_eEventType; }

 public:
  CJS_Context* m_pJSContext;
  JS_EVENT_T m_eEventType;
  FX_BOOL m_bValid;

  CFX_WideString m_strTargetName;
  CFX_WideString m_strSourceName;
  CFX_WideString* m_pWideStrChange;
  CFX_WideString m_WideStrChangeDu;
  CFX_WideString m_WideStrChangeEx;
  int m_nCommitKey;
  FX_BOOL m_bKeyDown;
  FX_BOOL m_bModifier;
  FX_BOOL m_bShift;
  int* m_pISelEnd;
  int m_nSelEndDu;
  int* m_pISelStart;
  int m_nSelStartDu;
  FX_BOOL m_bWillCommit;
  CFX_WideString* m_pValue;
  FX_BOOL m_bFieldFull;
  FX_BOOL* m_pbRc;
  FX_BOOL m_bRcDu;

  CPDFSDK_Document* m_pSourceDoc;
  CPDF_Bookmark* m_pTargetBookMark;
  CPDFSDK_Document* m_pTargetDoc;
  CPDFSDK_Annot* m_pTargetAnnot;
};

#endif  // FPDFSDK_JAVASCRIPT_JS_EVENTHANDLER_H_
