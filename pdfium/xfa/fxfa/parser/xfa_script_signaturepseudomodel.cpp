// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_signaturepseudomodel.h"

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CScript_SignaturePseudoModel::CScript_SignaturePseudoModel(
    CXFA_Document* pDocument)
    : CXFA_Object(pDocument,
                  XFA_ObjectType::Object,
                  XFA_Element::SignaturePseudoModel) {}
CScript_SignaturePseudoModel::~CScript_SignaturePseudoModel() {}
void CScript_SignaturePseudoModel::Verify(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 4) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"verify");
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  CXFA_Node* pNode = nullptr;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  int32_t bVerify = pNotify->GetDocProvider()->Verify(hDoc, pNode);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(bVerify);
}
void CScript_SignaturePseudoModel::Sign(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 3 || iLength > 7) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"sign");
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  CXFA_NodeList* pNodeList = nullptr;
  CFX_WideString wsExpression;
  CFX_WideString wsXMLIdent;
  if (iLength >= 1) {
    pNodeList = (CXFA_NodeList*)pArguments->GetObject(0);
  }
  if (iLength >= 2) {
    CFX_ByteString bsExpression = pArguments->GetUTF8String(1);
    wsExpression = CFX_WideString::FromUTF8(bsExpression.AsStringC());
  }
  if (iLength >= 3) {
    CFX_ByteString bsXMLIdent = pArguments->GetUTF8String(2);
    wsXMLIdent = CFX_WideString::FromUTF8(bsXMLIdent.AsStringC());
  }
  FX_BOOL bSign = pNotify->GetDocProvider()->Sign(
      hDoc, pNodeList, wsExpression.AsStringC(), wsXMLIdent.AsStringC());
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(bSign);
}
void CScript_SignaturePseudoModel::Enumerate(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 0) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"enumerate");
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  CXFA_NodeList* pList = pNotify->GetDocProvider()->Enumerate(hDoc);
  if (!pList)
    return;
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pList));
}
void CScript_SignaturePseudoModel::Clear(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"clear");
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  CXFA_Node* pNode = nullptr;
  FX_BOOL bClear = TRUE;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  if (iLength >= 2) {
    bClear = pArguments->GetInt32(1) == 0 ? FALSE : TRUE;
  }
  FX_BOOL bFlag = pNotify->GetDocProvider()->Clear(hDoc, pNode, bClear);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(bFlag);
}
