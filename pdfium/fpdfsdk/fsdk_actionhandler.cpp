// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/include/fsdk_actionhandler.h"

#include <set>

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/ijs_context.h"
#include "fpdfsdk/javascript/ijs_runtime.h"
#include "third_party/base/stl_util.h"

CPDFSDK_ActionHandler::CPDFSDK_ActionHandler()
    : m_pFormActionHandler(new CPDFSDK_FormActionHandler) {}

CPDFSDK_ActionHandler::~CPDFSDK_ActionHandler() {}

FX_BOOL CPDFSDK_ActionHandler::DoAction_DocOpen(const CPDF_Action& action,
                                                CPDFSDK_Document* pDocument) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteDocumentOpenAction(action, pDocument, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_JavaScript(
    const CPDF_Action& JsAction,
    CFX_WideString csJSName,
    CPDFSDK_Document* pDocument) {
  if (JsAction.GetType() == CPDF_Action::JavaScript) {
    CFX_WideString swJS = JsAction.GetJavaScript();
    if (!swJS.IsEmpty()) {
      RunDocumentOpenJavaScript(pDocument, csJSName, swJS);
      return TRUE;
    }
  }

  return FALSE;
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_FieldJavaScript(
    const CPDF_Action& JsAction,
    CPDF_AAction::AActionType type,
    CPDFSDK_Document* pDocument,
    CPDF_FormField* pFormField,
    PDFSDK_FieldAction& data) {
  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (pEnv->IsJSInitiated() && JsAction.GetType() == CPDF_Action::JavaScript) {
    CFX_WideString swJS = JsAction.GetJavaScript();
    if (!swJS.IsEmpty()) {
      RunFieldJavaScript(pDocument, pFormField, type, data, swJS);
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_Page(
    const CPDF_Action& action,
    enum CPDF_AAction::AActionType eType,
    CPDFSDK_Document* pDocument) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteDocumentPageAction(action, eType, pDocument, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_Document(
    const CPDF_Action& action,
    enum CPDF_AAction::AActionType eType,
    CPDFSDK_Document* pDocument) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteDocumentPageAction(action, eType, pDocument, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_BookMark(CPDF_Bookmark* pBookMark,
                                                 const CPDF_Action& action,
                                                 CPDF_AAction::AActionType type,
                                                 CPDFSDK_Document* pDocument) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteBookMark(action, pDocument, pBookMark, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_Screen(const CPDF_Action& action,
                                               CPDF_AAction::AActionType type,
                                               CPDFSDK_Document* pDocument,
                                               CPDFSDK_Annot* pScreen) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteScreenAction(action, type, pDocument, pScreen, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_Link(const CPDF_Action& action,
                                             CPDFSDK_Document* pDocument) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteLinkAction(action, pDocument, &visited);
}

FX_BOOL CPDFSDK_ActionHandler::DoAction_Field(const CPDF_Action& action,
                                              CPDF_AAction::AActionType type,
                                              CPDFSDK_Document* pDocument,
                                              CPDF_FormField* pFormField,
                                              PDFSDK_FieldAction& data) {
  std::set<CPDF_Dictionary*> visited;
  return ExecuteFieldAction(action, type, pDocument, pFormField, data,
                            &visited);
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteDocumentOpenAction(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        RunDocumentOpenJavaScript(pDocument, L"", swJS);
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteDocumentOpenAction(subaction, pDocument, visited))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteLinkAction(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
        pRuntime->SetReaderDocument(pDocument);

        IJS_Context* pContext = pRuntime->NewContext();
        pContext->OnLink_MouseUp(pDocument);

        CFX_WideString csInfo;
        FX_BOOL bRet = pContext->RunScript(swJS, &csInfo);
        if (!bRet) {
          // FIXME: return error.
        }

        pRuntime->ReleaseContext(pContext);
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteLinkAction(subaction, pDocument, visited))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteDocumentPageAction(
    const CPDF_Action& action,
    CPDF_AAction::AActionType type,
    CPDFSDK_Document* pDocument,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        RunDocumentPageJavaScript(pDocument, type, swJS);
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  if (!IsValidDocView(pDocument))
    return FALSE;

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteDocumentPageAction(subaction, type, pDocument, visited))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CPDFSDK_ActionHandler::IsValidField(CPDFSDK_Document* pDocument,
                                            CPDF_Dictionary* pFieldDict) {
  ASSERT(pFieldDict);

  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  CPDF_InterForm* pPDFInterForm = pInterForm->GetInterForm();
  return !!pPDFInterForm->GetFieldByDict(pFieldDict);
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteFieldAction(
    const CPDF_Action& action,
    CPDF_AAction::AActionType type,
    CPDFSDK_Document* pDocument,
    CPDF_FormField* pFormField,
    PDFSDK_FieldAction& data,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        RunFieldJavaScript(pDocument, pFormField, type, data, swJS);
        if (!IsValidField(pDocument, pFormField->GetFieldDict()))
          return FALSE;
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteFieldAction(subaction, type, pDocument, pFormField, data,
                            visited))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteScreenAction(
    const CPDF_Action& action,
    CPDF_AAction::AActionType type,
    CPDFSDK_Document* pDocument,
    CPDFSDK_Annot* pScreen,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
        pRuntime->SetReaderDocument(pDocument);

        IJS_Context* pContext = pRuntime->NewContext();
        CFX_WideString csInfo;
        FX_BOOL bRet = pContext->RunScript(swJS, &csInfo);
        if (!bRet) {
          // FIXME: return error.
        }

        pRuntime->ReleaseContext(pContext);
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteScreenAction(subaction, type, pDocument, pScreen, visited))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CPDFSDK_ActionHandler::ExecuteBookMark(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument,
    CPDF_Bookmark* pBookmark,
    std::set<CPDF_Dictionary*>* visited) {
  CPDF_Dictionary* pDict = action.GetDict();
  if (pdfium::ContainsKey(*visited, pDict))
    return FALSE;

  visited->insert(pDict);

  CPDFDoc_Environment* pEnv = pDocument->GetEnv();
  ASSERT(pEnv);
  if (action.GetType() == CPDF_Action::JavaScript) {
    if (pEnv->IsJSInitiated()) {
      CFX_WideString swJS = action.GetJavaScript();
      if (!swJS.IsEmpty()) {
        IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
        pRuntime->SetReaderDocument(pDocument);

        IJS_Context* pContext = pRuntime->NewContext();
        pContext->OnBookmark_MouseUp(pBookmark);

        CFX_WideString csInfo;
        FX_BOOL bRet = pContext->RunScript(swJS, &csInfo);
        if (!bRet) {
          // FIXME: return error.
        }

        pRuntime->ReleaseContext(pContext);
      }
    }
  } else {
    DoAction_NoJs(action, pDocument);
  }

  for (int32_t i = 0, sz = action.GetSubActionsCount(); i < sz; i++) {
    CPDF_Action subaction = action.GetSubAction(i);
    if (!ExecuteBookMark(subaction, pDocument, pBookmark, visited))
      return FALSE;
  }

  return TRUE;
}

void CPDFSDK_ActionHandler::DoAction_NoJs(const CPDF_Action& action,
                                          CPDFSDK_Document* pDocument) {
  ASSERT(pDocument);

  switch (action.GetType()) {
    case CPDF_Action::GoTo:
      DoAction_GoTo(pDocument, action);
      break;
    case CPDF_Action::GoToR:
      DoAction_GoToR(pDocument, action);
      break;
    case CPDF_Action::GoToE:
      break;
    case CPDF_Action::Launch:
      DoAction_Launch(pDocument, action);
      break;
    case CPDF_Action::Thread:
      break;
    case CPDF_Action::URI:
      DoAction_URI(pDocument, action);
      break;
    case CPDF_Action::Sound:
      break;
    case CPDF_Action::Movie:
      break;
    case CPDF_Action::Hide:
      if (m_pFormActionHandler) {
        m_pFormActionHandler->DoAction_Hide(action, pDocument);
      }
      break;
    case CPDF_Action::Named:
      DoAction_Named(pDocument, action);
      break;
    case CPDF_Action::SubmitForm:
      if (m_pFormActionHandler) {
        m_pFormActionHandler->DoAction_SubmitForm(action, pDocument);
      }
      break;
    case CPDF_Action::ResetForm:
      if (m_pFormActionHandler) {
        m_pFormActionHandler->DoAction_ResetForm(action, pDocument);
      }
      break;
    case CPDF_Action::ImportData:
      if (m_pFormActionHandler) {
        m_pFormActionHandler->DoAction_ImportData(action, pDocument);
      }
      break;
    case CPDF_Action::JavaScript:
      ASSERT(FALSE);
      break;
    case CPDF_Action::SetOCGState:
      DoAction_SetOCGState(pDocument, action);
      break;
    case CPDF_Action::Rendition:
      break;
    case CPDF_Action::Trans:
      break;
    case CPDF_Action::GoTo3DView:
      break;
    default:
      break;
  }
}

FX_BOOL CPDFSDK_ActionHandler::IsValidDocView(CPDFSDK_Document* pDocument) {
  ASSERT(pDocument);
  return TRUE;
}

void CPDFSDK_ActionHandler::DoAction_GoTo(CPDFSDK_Document* pDocument,
                                          const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CPDF_Document* pPDFDocument = pDocument->GetPDFDocument();
  ASSERT(pPDFDocument);

  CPDF_Dest MyDest = action.GetDest(pPDFDocument);
  int nPageIndex = MyDest.GetPageIndex(pPDFDocument);
  int nFitType = MyDest.GetZoomMode();
  const CPDF_Array* pMyArray = ToArray(MyDest.GetObject());
  float* pPosAry = nullptr;
  int sizeOfAry = 0;
  if (pMyArray) {
    pPosAry = new float[pMyArray->GetCount()];
    int j = 0;
    for (size_t i = 2; i < pMyArray->GetCount(); i++) {
      pPosAry[j++] = pMyArray->GetFloatAt(i);
    }
    sizeOfAry = j;
  }

  CPDFDoc_Environment* pApp = pDocument->GetEnv();
  pApp->FFI_DoGoToAction(nPageIndex, nFitType, pPosAry, sizeOfAry);
  delete[] pPosAry;
}

void CPDFSDK_ActionHandler::DoAction_GoToR(CPDFSDK_Document* pDocument,
                                           const CPDF_Action& action) {}

void CPDFSDK_ActionHandler::DoAction_Launch(CPDFSDK_Document* pDocument,
                                            const CPDF_Action& action) {}

void CPDFSDK_ActionHandler::DoAction_URI(CPDFSDK_Document* pDocument,
                                         const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CPDFDoc_Environment* pApp = pDocument->GetEnv();
  CFX_ByteString sURI = action.GetURI(pDocument->GetPDFDocument());
  pApp->FFI_DoURIAction(sURI.c_str());
}

void CPDFSDK_ActionHandler::DoAction_Named(CPDFSDK_Document* pDocument,
                                           const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CFX_ByteString csName = action.GetNamedAction();
  pDocument->GetEnv()->FFI_ExecuteNamedAction(csName.c_str());
}

void CPDFSDK_ActionHandler::DoAction_SetOCGState(CPDFSDK_Document* pDocument,
                                                 const CPDF_Action& action) {}

void CPDFSDK_ActionHandler::RunFieldJavaScript(CPDFSDK_Document* pDocument,
                                               CPDF_FormField* pFormField,
                                               CPDF_AAction::AActionType type,
                                               PDFSDK_FieldAction& data,
                                               const CFX_WideString& script) {
  ASSERT(type != CPDF_AAction::Calculate);
  ASSERT(type != CPDF_AAction::Format);

  IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
  pRuntime->SetReaderDocument(pDocument);

  IJS_Context* pContext = pRuntime->NewContext();
  switch (type) {
    case CPDF_AAction::CursorEnter:
      pContext->OnField_MouseEnter(data.bModifier, data.bShift, pFormField);
      break;
    case CPDF_AAction::CursorExit:
      pContext->OnField_MouseExit(data.bModifier, data.bShift, pFormField);
      break;
    case CPDF_AAction::ButtonDown:
      pContext->OnField_MouseDown(data.bModifier, data.bShift, pFormField);
      break;
    case CPDF_AAction::ButtonUp:
      pContext->OnField_MouseUp(data.bModifier, data.bShift, pFormField);
      break;
    case CPDF_AAction::GetFocus:
      pContext->OnField_Focus(data.bModifier, data.bShift, pFormField,
                              data.sValue);
      break;
    case CPDF_AAction::LoseFocus:
      pContext->OnField_Blur(data.bModifier, data.bShift, pFormField,
                             data.sValue);
      break;
    case CPDF_AAction::KeyStroke:
      pContext->OnField_Keystroke(data.sChange, data.sChangeEx, data.bKeyDown,
                                  data.bModifier, data.nSelEnd, data.nSelStart,
                                  data.bShift, pFormField, data.sValue,
                                  data.bWillCommit, data.bFieldFull, data.bRC);
      break;
    case CPDF_AAction::Validate:
      pContext->OnField_Validate(data.sChange, data.sChangeEx, data.bKeyDown,
                                 data.bModifier, data.bShift, pFormField,
                                 data.sValue, data.bRC);
      break;
    default:
      ASSERT(FALSE);
      break;
  }

  CFX_WideString csInfo;
  FX_BOOL bRet = pContext->RunScript(script, &csInfo);
  if (!bRet) {
    // FIXME: return error.
  }

  pRuntime->ReleaseContext(pContext);
}

void CPDFSDK_ActionHandler::RunDocumentOpenJavaScript(
    CPDFSDK_Document* pDocument,
    const CFX_WideString& sScriptName,
    const CFX_WideString& script) {
  IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
  pRuntime->SetReaderDocument(pDocument);
  IJS_Context* pContext = pRuntime->NewContext();
  pContext->OnDoc_Open(pDocument, sScriptName);

  CFX_WideString csInfo;
  FX_BOOL bRet = pContext->RunScript(script, &csInfo);
  if (!bRet) {
    // FIXME: return error.
  }

  pRuntime->ReleaseContext(pContext);
}

void CPDFSDK_ActionHandler::RunDocumentPageJavaScript(
    CPDFSDK_Document* pDocument,
    CPDF_AAction::AActionType type,
    const CFX_WideString& script) {
  IJS_Runtime* pRuntime = pDocument->GetJsRuntime();
  pRuntime->SetReaderDocument(pDocument);

  IJS_Context* pContext = pRuntime->NewContext();
  switch (type) {
    case CPDF_AAction::OpenPage:
      pContext->OnPage_Open(pDocument);
      break;
    case CPDF_AAction::ClosePage:
      pContext->OnPage_Close(pDocument);
      break;
    case CPDF_AAction::CloseDocument:
      pContext->OnDoc_WillClose(pDocument);
      break;
    case CPDF_AAction::SaveDocument:
      pContext->OnDoc_WillSave(pDocument);
      break;
    case CPDF_AAction::DocumentSaved:
      pContext->OnDoc_DidSave(pDocument);
      break;
    case CPDF_AAction::PrintDocument:
      pContext->OnDoc_WillPrint(pDocument);
      break;
    case CPDF_AAction::DocumentPrinted:
      pContext->OnDoc_DidPrint(pDocument);
      break;
    case CPDF_AAction::PageVisible:
      pContext->OnPage_InView(pDocument);
      break;
    case CPDF_AAction::PageInvisible:
      pContext->OnPage_OutView(pDocument);
      break;
    default:
      ASSERT(FALSE);
      break;
  }

  CFX_WideString csInfo;
  FX_BOOL bRet = pContext->RunScript(script, &csInfo);
  if (!bRet) {
    // FIXME: return error.
  }

  pRuntime->ReleaseContext(pContext);
}

FX_BOOL CPDFSDK_FormActionHandler::DoAction_Hide(const CPDF_Action& action,
                                                 CPDFSDK_Document* pDocument) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  if (pInterForm->DoAction_Hide(action)) {
    pDocument->SetChangeMark();
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CPDFSDK_FormActionHandler::DoAction_SubmitForm(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  return pInterForm->DoAction_SubmitForm(action);
}

FX_BOOL CPDFSDK_FormActionHandler::DoAction_ResetForm(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  return pInterForm->DoAction_ResetForm(action);
}

FX_BOOL CPDFSDK_FormActionHandler::DoAction_ImportData(
    const CPDF_Action& action,
    CPDFSDK_Document* pDocument) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  if (pInterForm->DoAction_ImportData(action)) {
    pDocument->SetChangeMark();
    return TRUE;
  }

  return FALSE;
}
