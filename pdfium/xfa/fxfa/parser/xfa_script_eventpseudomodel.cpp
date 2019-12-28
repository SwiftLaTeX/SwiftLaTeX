// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_eventpseudomodel.h"

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

void StringProperty(CFXJSE_Value* pValue,
                    CFX_WideString& wsValue,
                    FX_BOOL bSetting) {
  if (bSetting) {
    wsValue = pValue->ToWideString();
    return;
  }
  pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
}

void InterProperty(CFXJSE_Value* pValue, int32_t& iValue, FX_BOOL bSetting) {
  if (bSetting) {
    iValue = pValue->ToInteger();
    return;
  }
  pValue->SetInteger(iValue);
}

void BooleanProperty(CFXJSE_Value* pValue, FX_BOOL& bValue, FX_BOOL bSetting) {
  if (bSetting) {
    bValue = pValue->ToBoolean();
    return;
  }
  pValue->SetBoolean(bValue);
}

}  // namespace

CScript_EventPseudoModel::CScript_EventPseudoModel(CXFA_Document* pDocument)
    : CXFA_Object(pDocument,
                  XFA_ObjectType::Object,
                  XFA_Element::EventPseudoModel) {}

CScript_EventPseudoModel::~CScript_EventPseudoModel() {}

void CScript_EventPseudoModel::Property(CFXJSE_Value* pValue,
                                        XFA_Event dwFlag,
                                        FX_BOOL bSetting) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext)
    return;

  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam)
    return;

  switch (dwFlag) {
    case XFA_Event::CancelAction:
      BooleanProperty(pValue, pEventParam->m_bCancelAction, bSetting);
      break;
    case XFA_Event::Change:
      StringProperty(pValue, pEventParam->m_wsChange, bSetting);
      break;
    case XFA_Event::CommitKey:
      InterProperty(pValue, pEventParam->m_iCommitKey, bSetting);
      break;
    case XFA_Event::FullText:
      StringProperty(pValue, pEventParam->m_wsFullText, bSetting);
      break;
    case XFA_Event::Keydown:
      BooleanProperty(pValue, pEventParam->m_bKeyDown, bSetting);
      break;
    case XFA_Event::Modifier:
      BooleanProperty(pValue, pEventParam->m_bModifier, bSetting);
      break;
    case XFA_Event::NewContentType:
      StringProperty(pValue, pEventParam->m_wsNewContentType, bSetting);
      break;
    case XFA_Event::NewText:
      StringProperty(pValue, pEventParam->m_wsNewText, bSetting);
      break;
    case XFA_Event::PreviousContentType:
      StringProperty(pValue, pEventParam->m_wsPrevContentType, bSetting);
      break;
    case XFA_Event::PreviousText:
      StringProperty(pValue, pEventParam->m_wsPrevText, bSetting);
      break;
    case XFA_Event::Reenter:
      BooleanProperty(pValue, pEventParam->m_bReenter, bSetting);
      break;
    case XFA_Event::SelectionEnd:
      InterProperty(pValue, pEventParam->m_iSelEnd, bSetting);
      break;
    case XFA_Event::SelectionStart:
      InterProperty(pValue, pEventParam->m_iSelStart, bSetting);
      break;
    case XFA_Event::Shift:
      BooleanProperty(pValue, pEventParam->m_bShift, bSetting);
      break;
    case XFA_Event::SoapFaultCode:
      StringProperty(pValue, pEventParam->m_wsSoapFaultCode, bSetting);
      break;
    case XFA_Event::SoapFaultString:
      StringProperty(pValue, pEventParam->m_wsSoapFaultString, bSetting);
      break;
    case XFA_Event::Target:
      break;
    default:
      break;
  }
}
void CScript_EventPseudoModel::Change(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Change, bSetting);
}
void CScript_EventPseudoModel::CommitKey(CFXJSE_Value* pValue,
                                         FX_BOOL bSetting,
                                         XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::CommitKey, bSetting);
}
void CScript_EventPseudoModel::FullText(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::FullText, bSetting);
}
void CScript_EventPseudoModel::KeyDown(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Keydown, bSetting);
}
void CScript_EventPseudoModel::Modifier(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Modifier, bSetting);
}
void CScript_EventPseudoModel::NewContentType(CFXJSE_Value* pValue,
                                              FX_BOOL bSetting,
                                              XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::NewContentType, bSetting);
}
void CScript_EventPseudoModel::NewText(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::NewText, bSetting);
}
void CScript_EventPseudoModel::PrevContentType(CFXJSE_Value* pValue,
                                               FX_BOOL bSetting,
                                               XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::PreviousContentType, bSetting);
}
void CScript_EventPseudoModel::PrevText(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::PreviousText, bSetting);
}
void CScript_EventPseudoModel::Reenter(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Reenter, bSetting);
}
void CScript_EventPseudoModel::SelEnd(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::SelectionEnd, bSetting);
}
void CScript_EventPseudoModel::SelStart(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::SelectionStart, bSetting);
}
void CScript_EventPseudoModel::Shift(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Shift, bSetting);
}
void CScript_EventPseudoModel::SoapFaultCode(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::SoapFaultCode, bSetting);
}
void CScript_EventPseudoModel::SoapFaultString(CFXJSE_Value* pValue,
                                               FX_BOOL bSetting,
                                               XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::SoapFaultString, bSetting);
}
void CScript_EventPseudoModel::Target(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  Property(pValue, XFA_Event::Target, bSetting);
}
void CScript_EventPseudoModel::Emit(CFXJSE_Arguments* pArguments) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext) {
    return;
  }
  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam) {
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFWidgetHandler* pWidgetHandler = pNotify->GetWidgetHandler();
  if (!pWidgetHandler) {
    return;
  }
  pWidgetHandler->ProcessEvent(pEventParam->m_pTarget, pEventParam);
}
void CScript_EventPseudoModel::Reset(CFXJSE_Arguments* pArguments) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext) {
    return;
  }
  CXFA_EventParam* pEventParam = pScriptContext->GetEventParam();
  if (!pEventParam) {
    return;
  }
  pEventParam->Reset();
}
