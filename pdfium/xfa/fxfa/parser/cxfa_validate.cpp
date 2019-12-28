// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_validate.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_Validate::CXFA_Validate(CXFA_Node* pNode) : CXFA_Data(pNode) {}

int32_t CXFA_Validate::GetFormatTest() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_FormatTest);
}

FX_BOOL CXFA_Validate::SetTestValue(int32_t iType,
                                    CFX_WideString& wsValue,
                                    XFA_ATTRIBUTEENUM eName) {
  const XFA_ATTRIBUTEENUMINFO* pInfo =
      XFA_GetAttributeEnumByName(wsValue.AsStringC());
  if (pInfo)
    eName = pInfo->eName;

  m_pNode->SetEnum((XFA_ATTRIBUTE)iType, eName, FALSE);
  return TRUE;
}

FX_BOOL CXFA_Validate::SetNullTest(CFX_WideString wsValue) {
  return SetTestValue(XFA_ATTRIBUTE_NullTest, wsValue,
                      XFA_ATTRIBUTEENUM_Disabled);
}

int32_t CXFA_Validate::GetNullTest() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_NullTest);
}

int32_t CXFA_Validate::GetScriptTest() {
  return m_pNode->GetEnum(XFA_ATTRIBUTE_ScriptTest);
}

void CXFA_Validate::GetMessageText(CFX_WideString& wsMessage,
                                   const CFX_WideString& wsMessageType) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Message, FALSE);
  if (!pNode)
    return;

  CXFA_Node* pItemNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItemNode;
       pItemNode = pItemNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItemNode->GetElementType() != XFA_Element::Text)
      continue;

    CFX_WideStringC wsName;
    pItemNode->TryCData(XFA_ATTRIBUTE_Name, wsName);
    if (wsName.IsEmpty() || wsName == wsMessageType) {
      pItemNode->TryContent(wsMessage);
      return;
    }
  }
}

void CXFA_Validate::SetFormatMessageText(CFX_WideString wsMessage) {
  SetMessageText(wsMessage, L"formatTest");
}

void CXFA_Validate::GetFormatMessageText(CFX_WideString& wsMessage) {
  GetMessageText(wsMessage, L"formatTest");
}

void CXFA_Validate::SetNullMessageText(CFX_WideString wsMessage) {
  SetMessageText(wsMessage, L"nullTest");
}

void CXFA_Validate::GetNullMessageText(CFX_WideString& wsMessage) {
  GetMessageText(wsMessage, L"nullTest");
}

void CXFA_Validate::SetMessageText(CFX_WideString& wsMessage,
                                   const CFX_WideString& wsMessageType) {
  CXFA_Node* pNode = m_pNode->GetProperty(0, XFA_Element::Message, TRUE);
  if (!pNode)
    return;

  CXFA_Node* pItemNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItemNode;
       pItemNode = pItemNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItemNode->GetElementType() != XFA_Element::Text)
      continue;

    CFX_WideStringC wsName;
    pItemNode->TryCData(XFA_ATTRIBUTE_Name, wsName);
    if (wsName.IsEmpty() || wsName == wsMessageType) {
      pItemNode->SetContent(wsMessage, wsMessage, FALSE);
      return;
    }
  }
  CXFA_Node* pTextNode = pNode->CreateSamePacketNode(XFA_Element::Text);
  pNode->InsertChild(pTextNode);
  pTextNode->SetCData(XFA_ATTRIBUTE_Name, wsMessageType, FALSE);
  pTextNode->SetContent(wsMessage, wsMessage, FALSE);
}

void CXFA_Validate::GetScriptMessageText(CFX_WideString& wsMessage) {
  GetMessageText(wsMessage, L"scriptTest");
}

void CXFA_Validate::SetScriptMessageText(CFX_WideString wsMessage) {
  SetMessageText(wsMessage, L"scriptTest");
}

void CXFA_Validate::GetPicture(CFX_WideString& wsPicture) {
  if (CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Picture))
    pNode->TryContent(wsPicture);
}

CXFA_Script CXFA_Validate::GetScript() {
  return CXFA_Script(m_pNode->GetChild(0, XFA_Element::Script));
}
