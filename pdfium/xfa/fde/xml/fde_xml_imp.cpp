// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/xml/fde_xml_imp.h"

#include <algorithm>

#include "core/fxcrt/include/fx_safe_types.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/crt/fgas_system.h"

namespace {

const uint16_t g_XMLValidCharRange[][2] = {{0x09, 0x09},
                                           {0x0A, 0x0A},
                                           {0x0D, 0x0D},
                                           {0x20, 0xD7FF},
                                           {0xE000, 0xFFFD}};

bool FDE_IsXMLWhiteSpace(FX_WCHAR ch) {
  return ch == L' ' || ch == 0x0A || ch == 0x0D || ch == 0x09;
}

struct FDE_XMLNAMECHAR {
  uint16_t wStart;
  uint16_t wEnd;
  bool bStartChar;
};

const FDE_XMLNAMECHAR g_XMLNameChars[] = {
    {L'-', L'.', false},    {L'0', L'9', false},     {L':', L':', false},
    {L'A', L'Z', true},     {L'_', L'_', true},      {L'a', L'z', true},
    {0xB7, 0xB7, false},    {0xC0, 0xD6, true},      {0xD8, 0xF6, true},
    {0xF8, 0x02FF, true},   {0x0300, 0x036F, false}, {0x0370, 0x037D, true},
    {0x037F, 0x1FFF, true}, {0x200C, 0x200D, true},  {0x203F, 0x2040, false},
    {0x2070, 0x218F, true}, {0x2C00, 0x2FEF, true},  {0x3001, 0xD7FF, true},
    {0xF900, 0xFDCF, true}, {0xFDF0, 0xFFFD, true},
};

bool FDE_IsXMLNameChar(FX_WCHAR ch, FX_BOOL bFirstChar) {
  int32_t iStart = 0;
  int32_t iEnd = FX_ArraySize(g_XMLNameChars) - 1;
  while (iStart <= iEnd) {
    int32_t iMid = (iStart + iEnd) / 2;
    if (ch < g_XMLNameChars[iMid].wStart) {
      iEnd = iMid - 1;
    } else if (ch > g_XMLNameChars[iMid].wEnd) {
      iStart = iMid + 1;
    } else {
      return bFirstChar ? g_XMLNameChars[iMid].bStartChar : true;
    }
  }
  return false;
}

}  // namespace

FX_BOOL FDE_IsXMLValidChar(FX_WCHAR ch) {
  int32_t iStart = 0;
  int32_t iEnd = FX_ArraySize(g_XMLValidCharRange) - 1;
  while (iStart <= iEnd) {
    int32_t iMid = (iStart + iEnd) / 2;
    if (ch < g_XMLValidCharRange[iMid][0]) {
      iEnd = iMid - 1;
    } else if (ch > g_XMLValidCharRange[iMid][1]) {
      iStart = iMid + 1;
    } else {
      return TRUE;
    }
  }
  return FALSE;
}

CFDE_XMLNode::CFDE_XMLNode()
    : m_pParent(nullptr),
      m_pChild(nullptr),
      m_pPrior(nullptr),
      m_pNext(nullptr) {}

void CFDE_XMLNode::Release() {
  delete this;
}

FDE_XMLNODETYPE CFDE_XMLNode::GetType() const {
  return FDE_XMLNODE_Unknown;
}

CFDE_XMLNode::~CFDE_XMLNode() {
  DeleteChildren();
}

void CFDE_XMLNode::DeleteChildren() {
  CFDE_XMLNode* pChild = m_pChild;
  while (pChild) {
    CFDE_XMLNode* pTemp = pChild->m_pNext;
    pChild->Release();
    pChild = pTemp;
  }
  m_pChild = nullptr;
}

int32_t CFDE_XMLNode::CountChildNodes() const {
  int32_t iCount = 0;
  CFDE_XMLNode* pChild = m_pChild;
  while (pChild) {
    iCount++;
    pChild = pChild->m_pNext;
  }
  return iCount;
}

CFDE_XMLNode* CFDE_XMLNode::GetChildNode(int32_t index) const {
  CFDE_XMLNode* pChild = m_pChild;
  while (pChild) {
    if (index == 0) {
      return pChild;
    }
    index--;
    pChild = pChild->m_pNext;
  }
  return nullptr;
}

int32_t CFDE_XMLNode::GetChildNodeIndex(CFDE_XMLNode* pNode) const {
  int32_t index = 0;
  CFDE_XMLNode* pChild = m_pChild;
  while (pChild) {
    if (pChild == pNode) {
      return index;
    }
    index++;
    pChild = pChild->m_pNext;
  }
  return -1;
}

CFDE_XMLNode* CFDE_XMLNode::GetPath(const FX_WCHAR* pPath,
                                    int32_t iLength,
                                    FX_BOOL bQualifiedName) const {
  ASSERT(pPath);
  if (iLength < 0) {
    iLength = FXSYS_wcslen(pPath);
  }
  if (iLength == 0) {
    return nullptr;
  }
  CFX_WideString csPath;
  const FX_WCHAR* pStart = pPath;
  const FX_WCHAR* pEnd = pPath + iLength;
  FX_WCHAR ch;
  while (pStart < pEnd) {
    ch = *pStart++;
    if (ch == L'/') {
      break;
    } else {
      csPath += ch;
    }
  }
  iLength -= pStart - pPath;
  CFDE_XMLNode* pFind = nullptr;
  if (csPath.GetLength() < 1) {
    pFind = GetNodeItem(CFDE_XMLNode::Root);
  } else if (csPath.Compare(L"..") == 0) {
    pFind = m_pParent;
  } else if (csPath.Compare(L".") == 0) {
    pFind = (CFDE_XMLNode*)this;
  } else {
    CFX_WideString wsTag;
    CFDE_XMLNode* pNode = m_pChild;
    while (pNode) {
      if (pNode->GetType() == FDE_XMLNODE_Element) {
        if (bQualifiedName) {
          ((CFDE_XMLElement*)pNode)->GetTagName(wsTag);
        } else {
          ((CFDE_XMLElement*)pNode)->GetLocalTagName(wsTag);
        }
        if (wsTag.Compare(csPath) == 0) {
          if (iLength < 1) {
            pFind = pNode;
          } else {
            pFind = pNode->GetPath(pStart, iLength, bQualifiedName);
          }
          if (pFind)
            return pFind;
        }
      }
      pNode = pNode->m_pNext;
    }
  }
  if (!pFind || iLength < 1)
    return pFind;
  return pFind->GetPath(pStart, iLength, bQualifiedName);
}

int32_t CFDE_XMLNode::InsertChildNode(CFDE_XMLNode* pNode, int32_t index) {
  pNode->m_pParent = this;
  if (!m_pChild) {
    m_pChild = pNode;
    pNode->m_pPrior = nullptr;
    pNode->m_pNext = nullptr;
    return 0;
  }
  if (index == 0) {
    pNode->m_pNext = m_pChild;
    pNode->m_pPrior = nullptr;
    m_pChild->m_pPrior = pNode;
    m_pChild = pNode;
    return 0;
  }
  int32_t iCount = 0;
  CFDE_XMLNode* pFind = m_pChild;
  while (++iCount != index && pFind->m_pNext) {
    pFind = pFind->m_pNext;
  }
  pNode->m_pPrior = pFind;
  pNode->m_pNext = pFind->m_pNext;
  if (pFind->m_pNext)
    pFind->m_pNext->m_pPrior = pNode;
  pFind->m_pNext = pNode;
  return iCount;
}

void CFDE_XMLNode::RemoveChildNode(CFDE_XMLNode* pNode) {
  ASSERT(m_pChild && pNode);
  if (m_pChild == pNode) {
    m_pChild = pNode->m_pNext;
  } else {
    pNode->m_pPrior->m_pNext = pNode->m_pNext;
  }
  if (pNode->m_pNext)
    pNode->m_pNext->m_pPrior = pNode->m_pPrior;
  pNode->m_pParent = nullptr;
  pNode->m_pNext = nullptr;
  pNode->m_pPrior = nullptr;
}

CFDE_XMLNode* CFDE_XMLNode::GetNodeItem(CFDE_XMLNode::NodeItem eItem) const {
  switch (eItem) {
    case CFDE_XMLNode::Root: {
      CFDE_XMLNode* pParent = (CFDE_XMLNode*)this;
      while (pParent->m_pParent) {
        pParent = pParent->m_pParent;
      }
      return pParent;
    }
    case CFDE_XMLNode::Parent:
      return m_pParent;
    case CFDE_XMLNode::FirstSibling: {
      CFDE_XMLNode* pItem = (CFDE_XMLNode*)this;
      while (pItem->m_pPrior) {
        pItem = pItem->m_pPrior;
      }
      return pItem == (CFDE_XMLNode*)this ? nullptr : pItem;
    }
    case CFDE_XMLNode::PriorSibling:
      return m_pPrior;
    case CFDE_XMLNode::NextSibling:
      return m_pNext;
    case CFDE_XMLNode::LastSibling: {
      CFDE_XMLNode* pItem = (CFDE_XMLNode*)this;
      while (pItem->m_pNext)
        pItem = pItem->m_pNext;
      return pItem == (CFDE_XMLNode*)this ? nullptr : pItem;
    }
    case CFDE_XMLNode::FirstNeighbor: {
      CFDE_XMLNode* pParent = (CFDE_XMLNode*)this;
      while (pParent->m_pParent)
        pParent = pParent->m_pParent;
      return pParent == (CFDE_XMLNode*)this ? nullptr : pParent;
    }
    case CFDE_XMLNode::PriorNeighbor: {
      if (!m_pPrior)
        return m_pParent;

      CFDE_XMLNode* pItem = m_pPrior;
      while (pItem->m_pChild) {
        pItem = pItem->m_pChild;
        while (pItem->m_pNext)
          pItem = pItem->m_pNext;
      }
      return pItem;
    }
    case CFDE_XMLNode::NextNeighbor: {
      if (m_pChild)
        return m_pChild;
      if (m_pNext)
        return m_pNext;
      CFDE_XMLNode* pItem = m_pParent;
      while (pItem) {
        if (pItem->m_pNext)
          return pItem->m_pNext;
        pItem = pItem->m_pParent;
      }
      return nullptr;
    }
    case CFDE_XMLNode::LastNeighbor: {
      CFDE_XMLNode* pItem = (CFDE_XMLNode*)this;
      while (pItem->m_pParent) {
        pItem = pItem->m_pParent;
      }
      while (TRUE) {
        while (pItem->m_pNext)
          pItem = pItem->m_pNext;
        if (!pItem->m_pChild)
          break;
        pItem = pItem->m_pChild;
      }
      return pItem == (CFDE_XMLNode*)this ? nullptr : pItem;
    }
    case CFDE_XMLNode::FirstChild:
      return m_pChild;
    case CFDE_XMLNode::LastChild: {
      if (!m_pChild)
        return nullptr;

      CFDE_XMLNode* pChild = m_pChild;
      while (pChild->m_pNext)
        pChild = pChild->m_pNext;
      return pChild;
    }
    default:
      break;
  }
  return nullptr;
}

int32_t CFDE_XMLNode::GetNodeLevel() const {
  int32_t iLevel = 0;
  const CFDE_XMLNode* pItem = m_pParent;
  while (pItem) {
    iLevel++;
    pItem = pItem->m_pParent;
  }
  return iLevel;
}

FX_BOOL CFDE_XMLNode::InsertNodeItem(CFDE_XMLNode::NodeItem eItem,
                                     CFDE_XMLNode* pNode) {
  switch (eItem) {
    case CFDE_XMLNode::NextSibling: {
      pNode->m_pParent = m_pParent;
      pNode->m_pNext = m_pNext;
      pNode->m_pPrior = this;
      if (m_pNext) {
        m_pNext->m_pPrior = pNode;
      }
      m_pNext = pNode;
      return TRUE;
    }
    case CFDE_XMLNode::PriorSibling: {
      pNode->m_pParent = m_pParent;
      pNode->m_pNext = this;
      pNode->m_pPrior = m_pPrior;
      if (m_pPrior) {
        m_pPrior->m_pNext = pNode;
      } else if (m_pParent) {
        m_pParent->m_pChild = pNode;
      }
      m_pPrior = pNode;
      return TRUE;
    }
    default:
      return FALSE;
  }
}

CFDE_XMLNode* CFDE_XMLNode::RemoveNodeItem(CFDE_XMLNode::NodeItem eItem) {
  CFDE_XMLNode* pNode = nullptr;
  switch (eItem) {
    case CFDE_XMLNode::NextSibling:
      if (m_pNext) {
        pNode = m_pNext;
        m_pNext = pNode->m_pNext;
        if (m_pNext) {
          m_pNext->m_pPrior = this;
        }
        pNode->m_pParent = nullptr;
        pNode->m_pNext = nullptr;
        pNode->m_pPrior = nullptr;
      }
      break;
    default:
      break;
  }
  return pNode;
}

CFDE_XMLNode* CFDE_XMLNode::Clone(FX_BOOL bRecursive) {
  return nullptr;
}

void CFDE_XMLNode::SaveXMLNode(IFX_Stream* pXMLStream) {
  CFDE_XMLNode* pNode = (CFDE_XMLNode*)this;
  switch (pNode->GetType()) {
    case FDE_XMLNODE_Instruction: {
      CFX_WideString ws;
      CFDE_XMLInstruction* pInstruction = (CFDE_XMLInstruction*)pNode;
      if (pInstruction->m_wsTarget.CompareNoCase(L"xml") == 0) {
        ws = L"<?xml version=\"1.0\" encoding=\"";
        uint16_t wCodePage = pXMLStream->GetCodePage();
        if (wCodePage == FX_CODEPAGE_UTF16LE) {
          ws += L"UTF-16";
        } else if (wCodePage == FX_CODEPAGE_UTF16BE) {
          ws += L"UTF-16be";
        } else {
          ws += L"UTF-8";
        }
        ws += L"\"?>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      } else {
        ws.Format(L"<?%s", pInstruction->m_wsTarget.c_str());
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        CFX_WideStringArray& attributes = pInstruction->m_Attributes;
        int32_t i, iCount = attributes.GetSize();
        CFX_WideString wsValue;
        for (i = 0; i < iCount; i += 2) {
          ws = L" ";
          ws += attributes[i];
          ws += L"=\"";
          wsValue = attributes[i + 1];
          wsValue.Replace(L"&", L"&amp;");
          wsValue.Replace(L"<", L"&lt;");
          wsValue.Replace(L">", L"&gt;");
          wsValue.Replace(L"\'", L"&apos;");
          wsValue.Replace(L"\"", L"&quot;");
          ws += wsValue;
          ws += L"\"";
          pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        }
        CFX_WideStringArray& targetdata = pInstruction->m_TargetData;
        iCount = targetdata.GetSize();
        for (i = 0; i < iCount; i++) {
          ws = L" \"";
          ws += targetdata[i];
          ws += L"\"";
          pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        }
        ws = L"?>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      }
    } break;
    case FDE_XMLNODE_Element: {
      CFX_WideString ws;
      ws = L"<";
      ws += ((CFDE_XMLElement*)pNode)->m_wsTag;
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      CFX_WideStringArray& attributes = ((CFDE_XMLElement*)pNode)->m_Attributes;
      int32_t iCount = attributes.GetSize();
      CFX_WideString wsValue;
      for (int32_t i = 0; i < iCount; i += 2) {
        ws = L" ";
        ws += attributes[i];
        ws += L"=\"";
        wsValue = attributes[i + 1];
        wsValue.Replace(L"&", L"&amp;");
        wsValue.Replace(L"<", L"&lt;");
        wsValue.Replace(L">", L"&gt;");
        wsValue.Replace(L"\'", L"&apos;");
        wsValue.Replace(L"\"", L"&quot;");
        ws += wsValue;
        ws += L"\"";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      }
      if (pNode->m_pChild) {
        ws = L"\n>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        CFDE_XMLNode* pChild = pNode->m_pChild;
        while (pChild) {
          pChild->SaveXMLNode(pXMLStream);
          pChild = pChild->m_pNext;
        }
        ws = L"</";
        ws += ((CFDE_XMLElement*)pNode)->m_wsTag;
        ws += L"\n>";
      } else {
        ws = L"\n/>";
      }
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_Text: {
      CFX_WideString ws = ((CFDE_XMLText*)pNode)->m_wsText;
      ws.Replace(L"&", L"&amp;");
      ws.Replace(L"<", L"&lt;");
      ws.Replace(L">", L"&gt;");
      ws.Replace(L"\'", L"&apos;");
      ws.Replace(L"\"", L"&quot;");
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_CharData: {
      CFX_WideString ws = L"<![CDATA[";
      ws += ((CFDE_XMLCharData*)pNode)->m_wsCharData;
      ws += L"]]>";
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_Unknown:
      break;
    default:
      break;
  }
}

void CFDE_XMLNode::CloneChildren(CFDE_XMLNode* pClone) {
  if (!m_pChild) {
    return;
  }
  CFDE_XMLNode* pNext = m_pChild;
  CFDE_XMLNode* pCloneNext = pNext->Clone(TRUE);
  pClone->InsertChildNode(pCloneNext);
  pNext = pNext->m_pNext;
  while (pNext) {
    CFDE_XMLNode* pChild = pNext->Clone(TRUE);
    pCloneNext->InsertNodeItem(CFDE_XMLNode::NextSibling, pChild);
    pCloneNext = pChild;
    pNext = pNext->m_pNext;
  }
}

CFDE_XMLInstruction::CFDE_XMLInstruction(const CFX_WideString& wsTarget)
    : m_wsTarget(wsTarget) {
  ASSERT(m_wsTarget.GetLength() > 0);
}

void CFDE_XMLInstruction::Release() {
  delete this;
}

FDE_XMLNODETYPE CFDE_XMLInstruction::GetType() const {
  return FDE_XMLNODE_Instruction;
}

CFDE_XMLNode* CFDE_XMLInstruction::Clone(FX_BOOL bRecursive) {
  CFDE_XMLInstruction* pClone = new CFDE_XMLInstruction(m_wsTarget);
  if (!pClone) {
    return pClone;
  }
  pClone->m_Attributes.Copy(m_Attributes);
  pClone->m_TargetData.Copy(m_TargetData);
  if (bRecursive) {
    CloneChildren(pClone);
  }
  return pClone;
}

int32_t CFDE_XMLInstruction::CountAttributes() const {
  return m_Attributes.GetSize() / 2;
}

FX_BOOL CFDE_XMLInstruction::GetAttribute(int32_t index,
                                          CFX_WideString& wsAttriName,
                                          CFX_WideString& wsAttriValue) const {
  int32_t iCount = m_Attributes.GetSize();
  ASSERT(index > -1 && index < iCount / 2);
  for (int32_t i = 0; i < iCount; i += 2) {
    if (index == 0) {
      wsAttriName = m_Attributes[i];
      wsAttriValue = m_Attributes[i + 1];
      return TRUE;
    }
    index--;
  }
  return FALSE;
}

FX_BOOL CFDE_XMLInstruction::HasAttribute(const FX_WCHAR* pwsAttriName) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

void CFDE_XMLInstruction::GetString(const FX_WCHAR* pwsAttriName,
                                    CFX_WideString& wsAttriValue,
                                    const FX_WCHAR* pwsDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      wsAttriValue = m_Attributes[i + 1];
      return;
    }
  }
  wsAttriValue = pwsDefValue;
}

void CFDE_XMLInstruction::SetString(const CFX_WideString& wsAttriName,
                                    const CFX_WideString& wsAttriValue) {
  ASSERT(wsAttriName.GetLength() > 0);
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(wsAttriName) == 0) {
      m_Attributes[i] = wsAttriName;
      m_Attributes[i + 1] = wsAttriValue;
      return;
    }
  }
  m_Attributes.Add(wsAttriName);
  m_Attributes.Add(wsAttriValue);
}

int32_t CFDE_XMLInstruction::GetInteger(const FX_WCHAR* pwsAttriName,
                                        int32_t iDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return FXSYS_wtoi(m_Attributes[i + 1].c_str());
    }
  }
  return iDefValue;
}

void CFDE_XMLInstruction::SetInteger(const FX_WCHAR* pwsAttriName,
                                     int32_t iAttriValue) {
  CFX_WideString wsValue;
  wsValue.Format(L"%d", iAttriValue);
  SetString(pwsAttriName, wsValue);
}

FX_FLOAT CFDE_XMLInstruction::GetFloat(const FX_WCHAR* pwsAttriName,
                                       FX_FLOAT fDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return FX_wcstof(m_Attributes[i + 1].c_str());
    }
  }
  return fDefValue;
}

void CFDE_XMLInstruction::SetFloat(const FX_WCHAR* pwsAttriName,
                                   FX_FLOAT fAttriValue) {
  CFX_WideString wsValue;
  wsValue.Format(L"%f", fAttriValue);
  SetString(pwsAttriName, wsValue);
}

void CFDE_XMLInstruction::RemoveAttribute(const FX_WCHAR* pwsAttriName) {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      m_Attributes.RemoveAt(i + 1);
      m_Attributes.RemoveAt(i);
      return;
    }
  }
}

int32_t CFDE_XMLInstruction::CountData() const {
  return m_TargetData.GetSize();
}

FX_BOOL CFDE_XMLInstruction::GetData(int32_t index,
                                     CFX_WideString& wsData) const {
  if (index < 0 || index >= m_TargetData.GetSize()) {
    return FALSE;
  }
  wsData = m_TargetData[index];
  return TRUE;
}

void CFDE_XMLInstruction::AppendData(const CFX_WideString& wsData) {
  m_TargetData.Add(wsData);
}

void CFDE_XMLInstruction::RemoveData(int32_t index) {
  m_TargetData.RemoveAt(index);
}

CFDE_XMLInstruction::~CFDE_XMLInstruction() {}

CFDE_XMLElement::CFDE_XMLElement(const CFX_WideString& wsTag)
    : CFDE_XMLNode(), m_wsTag(wsTag), m_Attributes() {
  ASSERT(m_wsTag.GetLength() > 0);
}

CFDE_XMLElement::~CFDE_XMLElement() {
  m_Attributes.RemoveAll();
}

void CFDE_XMLElement::Release() {
  delete this;
}

FDE_XMLNODETYPE CFDE_XMLElement::GetType() const {
  return FDE_XMLNODE_Element;
}

CFDE_XMLNode* CFDE_XMLElement::Clone(FX_BOOL bRecursive) {
  CFDE_XMLElement* pClone = new CFDE_XMLElement(m_wsTag);
  if (!pClone) {
    return nullptr;
  }
  pClone->m_Attributes.Copy(m_Attributes);
  if (bRecursive) {
    CloneChildren(pClone);
  } else {
    CFX_WideString wsText;
    CFDE_XMLNode* pChild = m_pChild;
    while (pChild) {
      switch (pChild->GetType()) {
        case FDE_XMLNODE_Text:
          wsText += ((CFDE_XMLText*)pChild)->m_wsText;
          break;
        default:
          break;
      }
      pChild = pChild->m_pNext;
    }
    pClone->SetTextData(wsText);
  }
  return pClone;
}

void CFDE_XMLElement::GetTagName(CFX_WideString& wsTag) const {
  wsTag = m_wsTag;
}

void CFDE_XMLElement::GetLocalTagName(CFX_WideString& wsTag) const {
  FX_STRSIZE iFind = m_wsTag.Find(L':', 0);
  if (iFind < 0) {
    wsTag = m_wsTag;
  } else {
    wsTag = m_wsTag.Right(m_wsTag.GetLength() - iFind - 1);
  }
}

void CFDE_XMLElement::GetNamespacePrefix(CFX_WideString& wsPrefix) const {
  FX_STRSIZE iFind = m_wsTag.Find(L':', 0);
  if (iFind < 0) {
    wsPrefix.clear();
  } else {
    wsPrefix = m_wsTag.Left(iFind);
  }
}

void CFDE_XMLElement::GetNamespaceURI(CFX_WideString& wsNamespace) const {
  CFX_WideString wsAttri(L"xmlns"), wsPrefix;
  GetNamespacePrefix(wsPrefix);
  if (wsPrefix.GetLength() > 0) {
    wsAttri += L":";
    wsAttri += wsPrefix;
  }
  wsNamespace.clear();
  CFDE_XMLNode* pNode = (CFDE_XMLNode*)this;
  while (pNode) {
    if (pNode->GetType() != FDE_XMLNODE_Element) {
      break;
    }
    CFDE_XMLElement* pElement = (CFDE_XMLElement*)pNode;
    if (!pElement->HasAttribute(wsAttri.c_str())) {
      pNode = pNode->GetNodeItem(CFDE_XMLNode::Parent);
      continue;
    }
    pElement->GetString(wsAttri.c_str(), wsNamespace);
    break;
  }
}

int32_t CFDE_XMLElement::CountAttributes() const {
  return m_Attributes.GetSize() / 2;
}

FX_BOOL CFDE_XMLElement::GetAttribute(int32_t index,
                                      CFX_WideString& wsAttriName,
                                      CFX_WideString& wsAttriValue) const {
  int32_t iCount = m_Attributes.GetSize();
  ASSERT(index > -1 && index < iCount / 2);
  for (int32_t i = 0; i < iCount; i += 2) {
    if (index == 0) {
      wsAttriName = m_Attributes[i];
      wsAttriValue = m_Attributes[i + 1];
      return TRUE;
    }
    index--;
  }
  return FALSE;
}

FX_BOOL CFDE_XMLElement::HasAttribute(const FX_WCHAR* pwsAttriName) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

void CFDE_XMLElement::GetString(const FX_WCHAR* pwsAttriName,
                                CFX_WideString& wsAttriValue,
                                const FX_WCHAR* pwsDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      wsAttriValue = m_Attributes[i + 1];
      return;
    }
  }
  wsAttriValue = pwsDefValue;
}

void CFDE_XMLElement::SetString(const CFX_WideString& wsAttriName,
                                const CFX_WideString& wsAttriValue) {
  ASSERT(wsAttriName.GetLength() > 0);
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(wsAttriName) == 0) {
      m_Attributes[i] = wsAttriName;
      m_Attributes[i + 1] = wsAttriValue;
      return;
    }
  }
  m_Attributes.Add(wsAttriName);
  m_Attributes.Add(wsAttriValue);
}

int32_t CFDE_XMLElement::GetInteger(const FX_WCHAR* pwsAttriName,
                                    int32_t iDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return FXSYS_wtoi(m_Attributes[i + 1].c_str());
    }
  }
  return iDefValue;
}

void CFDE_XMLElement::SetInteger(const FX_WCHAR* pwsAttriName,
                                 int32_t iAttriValue) {
  CFX_WideString wsValue;
  wsValue.Format(L"%d", iAttriValue);
  SetString(pwsAttriName, wsValue);
}

FX_FLOAT CFDE_XMLElement::GetFloat(const FX_WCHAR* pwsAttriName,
                                   FX_FLOAT fDefValue) const {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      return FX_wcstof(m_Attributes[i + 1].c_str());
    }
  }
  return fDefValue;
}

void CFDE_XMLElement::SetFloat(const FX_WCHAR* pwsAttriName,
                               FX_FLOAT fAttriValue) {
  CFX_WideString wsValue;
  wsValue.Format(L"%f", fAttriValue);
  SetString(pwsAttriName, wsValue);
}

void CFDE_XMLElement::RemoveAttribute(const FX_WCHAR* pwsAttriName) {
  int32_t iCount = m_Attributes.GetSize();
  for (int32_t i = 0; i < iCount; i += 2) {
    if (m_Attributes[i].Compare(pwsAttriName) == 0) {
      m_Attributes.RemoveAt(i + 1);
      m_Attributes.RemoveAt(i);
      return;
    }
  }
}

void CFDE_XMLElement::GetTextData(CFX_WideString& wsText) const {
  CFX_WideTextBuf buffer;
  CFDE_XMLNode* pChild = m_pChild;
  while (pChild) {
    switch (pChild->GetType()) {
      case FDE_XMLNODE_Text:
        buffer << ((CFDE_XMLText*)pChild)->m_wsText;
        break;
      case FDE_XMLNODE_CharData:
        buffer << ((CFDE_XMLCharData*)pChild)->m_wsCharData;
        break;
      default:
        break;
    }
    pChild = pChild->m_pNext;
  }
  wsText = buffer.AsStringC();
}

void CFDE_XMLElement::SetTextData(const CFX_WideString& wsText) {
  if (wsText.GetLength() < 1) {
    return;
  }
  InsertChildNode(new CFDE_XMLText(wsText));
}

CFDE_XMLText::CFDE_XMLText(const CFX_WideString& wsText)
    : CFDE_XMLNode(), m_wsText(wsText) {}

void CFDE_XMLText::Release() {
  delete this;
}

FDE_XMLNODETYPE CFDE_XMLText::GetType() const {
  return FDE_XMLNODE_Text;
}

CFDE_XMLNode* CFDE_XMLText::Clone(FX_BOOL bRecursive) {
  CFDE_XMLText* pClone = new CFDE_XMLText(m_wsText);
  return pClone;
}

CFDE_XMLText::~CFDE_XMLText() {}

CFDE_XMLCharData::CFDE_XMLCharData(const CFX_WideString& wsCData)
    : CFDE_XMLDeclaration(), m_wsCharData(wsCData) {}

void CFDE_XMLCharData::Release() {
  delete this;
}

FDE_XMLNODETYPE CFDE_XMLCharData::GetType() const {
  return FDE_XMLNODE_CharData;
}

CFDE_XMLNode* CFDE_XMLCharData::Clone(FX_BOOL bRecursive) {
  CFDE_XMLCharData* pClone = new CFDE_XMLCharData(m_wsCharData);
  return pClone;
}

CFDE_XMLCharData::~CFDE_XMLCharData() {}

CFDE_XMLDoc::CFDE_XMLDoc()
    : m_pRoot(nullptr), m_pSyntaxParser(nullptr), m_pXMLParser(nullptr) {
  Reset(TRUE);
  CFDE_XMLInstruction* pXML = new CFDE_XMLInstruction(L"xml");
  m_pRoot->InsertChildNode(pXML);
}

CFDE_XMLDoc::~CFDE_XMLDoc() {
  Reset(FALSE);
}

void CFDE_XMLDoc::Reset(FX_BOOL bInitRoot) {
  m_iStatus = 0;
  m_pStream = nullptr;
  if (bInitRoot) {
    if (m_pRoot)
      m_pRoot->DeleteChildren();
    else
      m_pRoot = new CFDE_XMLNode;
  } else {
    if (m_pRoot) {
      m_pRoot->Release();
      m_pRoot = nullptr;
    }
  }
  ReleaseParser();
}

void CFDE_XMLDoc::ReleaseParser() {
  if (m_pXMLParser) {
    m_pXMLParser->Release();
    m_pXMLParser = nullptr;
  }
  if (m_pSyntaxParser) {
    m_pSyntaxParser->Release();
    m_pSyntaxParser = nullptr;
  }
}

FX_BOOL CFDE_XMLDoc::LoadXML(CFDE_XMLParser* pXMLParser) {
  if (!pXMLParser)
    return FALSE;

  Reset(TRUE);
  m_pXMLParser = pXMLParser;
  return !!m_pXMLParser;
}

int32_t CFDE_XMLDoc::DoLoad(IFX_Pause* pPause) {
  if (m_iStatus >= 100)
    return m_iStatus;
  return m_iStatus = m_pXMLParser->DoParser(pPause);
}

void CFDE_XMLDoc::CloseXML() {
  ReleaseParser();
}

void CFDE_XMLDoc::SaveXMLNode(IFX_Stream* pXMLStream, CFDE_XMLNode* pINode) {
  CFDE_XMLNode* pNode = (CFDE_XMLNode*)pINode;
  switch (pNode->GetType()) {
    case FDE_XMLNODE_Instruction: {
      CFX_WideString ws;
      CFDE_XMLInstruction* pInstruction = (CFDE_XMLInstruction*)pNode;
      if (pInstruction->m_wsTarget.CompareNoCase(L"xml") == 0) {
        ws = L"<?xml version=\"1.0\" encoding=\"";
        uint16_t wCodePage = pXMLStream->GetCodePage();
        if (wCodePage == FX_CODEPAGE_UTF16LE) {
          ws += L"UTF-16";
        } else if (wCodePage == FX_CODEPAGE_UTF16BE) {
          ws += L"UTF-16be";
        } else {
          ws += L"UTF-8";
        }
        ws += L"\"?>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      } else {
        ws.Format(L"<?%s", pInstruction->m_wsTarget.c_str());
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        CFX_WideStringArray& attributes = pInstruction->m_Attributes;
        int32_t i, iCount = attributes.GetSize();
        CFX_WideString wsValue;
        for (i = 0; i < iCount; i += 2) {
          ws = L" ";
          ws += attributes[i];
          ws += L"=\"";
          wsValue = attributes[i + 1];
          wsValue.Replace(L"&", L"&amp;");
          wsValue.Replace(L"<", L"&lt;");
          wsValue.Replace(L">", L"&gt;");
          wsValue.Replace(L"\'", L"&apos;");
          wsValue.Replace(L"\"", L"&quot;");
          ws += wsValue;
          ws += L"\"";
          pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        }
        CFX_WideStringArray& targetdata = pInstruction->m_TargetData;
        iCount = targetdata.GetSize();
        for (i = 0; i < iCount; i++) {
          ws = L" \"";
          ws += targetdata[i];
          ws += L"\"";
          pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        }
        ws = L"?>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      }
    } break;
    case FDE_XMLNODE_Element: {
      CFX_WideString ws;
      ws = L"<";
      ws += ((CFDE_XMLElement*)pNode)->m_wsTag;
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      CFX_WideStringArray& attributes = ((CFDE_XMLElement*)pNode)->m_Attributes;
      int32_t iCount = attributes.GetSize();
      CFX_WideString wsValue;
      for (int32_t i = 0; i < iCount; i += 2) {
        ws = L" ";
        ws += attributes[i];
        ws += L"=\"";
        wsValue = attributes[i + 1];
        wsValue.Replace(L"&", L"&amp;");
        wsValue.Replace(L"<", L"&lt;");
        wsValue.Replace(L">", L"&gt;");
        wsValue.Replace(L"\'", L"&apos;");
        wsValue.Replace(L"\"", L"&quot;");
        ws += wsValue;
        ws += L"\"";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
      }
      if (pNode->m_pChild) {
        ws = L"\n>";
        pXMLStream->WriteString(ws.c_str(), ws.GetLength());
        CFDE_XMLNode* pChild = pNode->m_pChild;
        while (pChild) {
          SaveXMLNode(pXMLStream, static_cast<CFDE_XMLNode*>(pChild));
          pChild = pChild->m_pNext;
        }
        ws = L"</";
        ws += ((CFDE_XMLElement*)pNode)->m_wsTag;
        ws += L"\n>";
      } else {
        ws = L"\n/>";
      }
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_Text: {
      CFX_WideString ws = ((CFDE_XMLText*)pNode)->m_wsText;
      ws.Replace(L"&", L"&amp;");
      ws.Replace(L"<", L"&lt;");
      ws.Replace(L">", L"&gt;");
      ws.Replace(L"\'", L"&apos;");
      ws.Replace(L"\"", L"&quot;");
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_CharData: {
      CFX_WideString ws = L"<![CDATA[";
      ws += ((CFDE_XMLCharData*)pNode)->m_wsCharData;
      ws += L"]]>";
      pXMLStream->WriteString(ws.c_str(), ws.GetLength());
    } break;
    case FDE_XMLNODE_Unknown:
      break;
    default:
      break;
  }
}

void CFDE_XMLDoc::SaveXML(IFX_Stream* pXMLStream, FX_BOOL bSaveBOM) {
  if (!pXMLStream || pXMLStream == m_pStream) {
    m_pStream->Seek(FX_STREAMSEEK_Begin, 0);
    pXMLStream = m_pStream;
  }
  ASSERT((pXMLStream->GetAccessModes() & FX_STREAMACCESS_Text) != 0);
  ASSERT((pXMLStream->GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  uint16_t wCodePage = pXMLStream->GetCodePage();
  if (wCodePage != FX_CODEPAGE_UTF16LE && wCodePage != FX_CODEPAGE_UTF16BE &&
      wCodePage != FX_CODEPAGE_UTF8) {
    wCodePage = FX_CODEPAGE_UTF8;
    pXMLStream->SetCodePage(wCodePage);
  }
  if (bSaveBOM) {
    pXMLStream->WriteString(L"\xFEFF", 1);
  }
  CFDE_XMLNode* pNode = m_pRoot->m_pChild;
  while (pNode) {
    SaveXMLNode(pXMLStream, static_cast<CFDE_XMLNode*>(pNode));
    pNode = pNode->m_pNext;
  }
  if (pXMLStream == m_pStream) {
    int32_t iPos = pXMLStream->GetPosition();
    pXMLStream->SetLength(iPos);
  }
}

CFDE_BlockBuffer::CFDE_BlockBuffer(int32_t iAllocStep)
    : m_iDataLength(0),
      m_iBufferSize(0),
      m_iAllocStep(iAllocStep),
      m_iStartPosition(0) {}

CFDE_BlockBuffer::~CFDE_BlockBuffer() {
  ClearBuffer();
}

FX_WCHAR* CFDE_BlockBuffer::GetAvailableBlock(int32_t& iIndexInBlock) {
  iIndexInBlock = 0;
  if (!m_BlockArray.GetSize()) {
    return nullptr;
  }
  int32_t iRealIndex = m_iStartPosition + m_iDataLength;
  if (iRealIndex == m_iBufferSize) {
    FX_WCHAR* pBlock = FX_Alloc(FX_WCHAR, m_iAllocStep);
    m_BlockArray.Add(pBlock);
    m_iBufferSize += m_iAllocStep;
    return pBlock;
  }
  iIndexInBlock = iRealIndex % m_iAllocStep;
  return m_BlockArray[iRealIndex / m_iAllocStep];
}

FX_BOOL CFDE_BlockBuffer::InitBuffer(int32_t iBufferSize) {
  ClearBuffer();
  int32_t iNumOfBlock = (iBufferSize - 1) / m_iAllocStep + 1;
  for (int32_t i = 0; i < iNumOfBlock; i++) {
    m_BlockArray.Add(FX_Alloc(FX_WCHAR, m_iAllocStep));
  }
  m_iBufferSize = iNumOfBlock * m_iAllocStep;
  return TRUE;
}

void CFDE_BlockBuffer::SetTextChar(int32_t iIndex, FX_WCHAR ch) {
  if (iIndex < 0) {
    return;
  }
  int32_t iRealIndex = m_iStartPosition + iIndex;
  int32_t iBlockIndex = iRealIndex / m_iAllocStep;
  int32_t iInnerIndex = iRealIndex % m_iAllocStep;
  int32_t iBlockSize = m_BlockArray.GetSize();
  if (iBlockIndex >= iBlockSize) {
    int32_t iNewBlocks = iBlockIndex - iBlockSize + 1;
    do {
      FX_WCHAR* pBlock = FX_Alloc(FX_WCHAR, m_iAllocStep);
      m_BlockArray.Add(pBlock);
      m_iBufferSize += m_iAllocStep;
    } while (--iNewBlocks);
  }
  FX_WCHAR* pTextData = m_BlockArray[iBlockIndex];
  *(pTextData + iInnerIndex) = ch;
  if (m_iDataLength <= iIndex) {
    m_iDataLength = iIndex + 1;
  }
}

int32_t CFDE_BlockBuffer::DeleteTextChars(int32_t iCount, FX_BOOL bDirection) {
  if (iCount <= 0) {
    return m_iDataLength;
  }
  if (iCount >= m_iDataLength) {
    Reset(FALSE);
    return 0;
  }
  if (bDirection) {
    m_iStartPosition += iCount;
    m_iDataLength -= iCount;
  } else {
    m_iDataLength -= iCount;
  }
  return m_iDataLength;
}

void CFDE_BlockBuffer::GetTextData(CFX_WideString& wsTextData,
                                   int32_t iStart,
                                   int32_t iLength) const {
  wsTextData.clear();
  int32_t iMaybeDataLength = m_iBufferSize - 1 - m_iStartPosition;
  if (iStart < 0 || iStart > iMaybeDataLength) {
    return;
  }
  if (iLength == -1 || iLength > iMaybeDataLength) {
    iLength = iMaybeDataLength;
  }
  if (iLength <= 0) {
    return;
  }
  FX_WCHAR* pBuf = wsTextData.GetBuffer(iLength);
  if (!pBuf) {
    return;
  }
  int32_t iStartBlockIndex = 0;
  int32_t iStartInnerIndex = 0;
  TextDataIndex2BufIndex(iStart, iStartBlockIndex, iStartInnerIndex);
  int32_t iEndBlockIndex = 0;
  int32_t iEndInnerIndex = 0;
  TextDataIndex2BufIndex(iStart + iLength, iEndBlockIndex, iEndInnerIndex);
  int32_t iPointer = 0;
  for (int32_t i = iStartBlockIndex; i <= iEndBlockIndex; i++) {
    int32_t iBufferPointer = 0;
    int32_t iCopyLength = m_iAllocStep;
    if (i == iStartBlockIndex) {
      iCopyLength -= iStartInnerIndex;
      iBufferPointer = iStartInnerIndex;
    }
    if (i == iEndBlockIndex) {
      iCopyLength -= ((m_iAllocStep - 1) - iEndInnerIndex);
    }
    FX_WCHAR* pBlockBuf = m_BlockArray[i];
    FXSYS_memcpy(pBuf + iPointer, pBlockBuf + iBufferPointer,
                 iCopyLength * sizeof(FX_WCHAR));
    iPointer += iCopyLength;
  }
  wsTextData.ReleaseBuffer(iLength);
}

void CFDE_BlockBuffer::TextDataIndex2BufIndex(const int32_t iIndex,
                                              int32_t& iBlockIndex,
                                              int32_t& iInnerIndex) const {
  ASSERT(iIndex >= 0);
  int32_t iRealIndex = m_iStartPosition + iIndex;
  iBlockIndex = iRealIndex / m_iAllocStep;
  iInnerIndex = iRealIndex % m_iAllocStep;
}

void CFDE_BlockBuffer::ClearBuffer() {
  m_iBufferSize = 0;
  int32_t iSize = m_BlockArray.GetSize();
  for (int32_t i = 0; i < iSize; i++) {
    FX_Free(m_BlockArray[i]);
  }
  m_BlockArray.RemoveAll();
}

CFDE_XMLSyntaxParser::CFDE_XMLSyntaxParser()
    : m_pStream(nullptr),
      m_iXMLPlaneSize(-1),
      m_iCurrentPos(0),
      m_iCurrentNodeNum(-1),
      m_iLastNodeNum(-1),
      m_iParsedChars(0),
      m_iParsedBytes(0),
      m_pBuffer(nullptr),
      m_iBufferChars(0),
      m_bEOS(FALSE),
      m_pStart(nullptr),
      m_pEnd(nullptr),
      m_XMLNodeStack(16),
      m_iAllocStep(m_BlockBuffer.GetAllocStep()),
      m_iDataLength(m_BlockBuffer.GetDataLengthRef()),
      m_pCurrentBlock(nullptr),
      m_iIndexInBlock(0),
      m_iTextDataLength(0),
      m_syntaxParserResult(FDE_XmlSyntaxResult::None),
      m_syntaxParserState(FDE_XmlSyntaxState::Text),
      m_wQuotationMark(0),
      m_iEntityStart(-1),
      m_SkipStack(16) {
  m_CurNode.iNodeNum = -1;
  m_CurNode.eNodeType = FDE_XMLNODE_Unknown;
}

void CFDE_XMLSyntaxParser::Init(IFX_Stream* pStream,
                                int32_t iXMLPlaneSize,
                                int32_t iTextDataSize) {
  ASSERT(!m_pStream && !m_pBuffer);
  ASSERT(pStream && iXMLPlaneSize > 0);
  int32_t iStreamLength = pStream->GetLength();
  ASSERT(iStreamLength > 0);
  m_pStream = pStream;
  m_iXMLPlaneSize = std::min(iXMLPlaneSize, iStreamLength);
  uint8_t bom[4];
  m_iCurrentPos = m_pStream->GetBOM(bom);
  ASSERT(!m_pBuffer);

  FX_SAFE_INT32 alloc_size_safe = m_iXMLPlaneSize;
  alloc_size_safe += 1;  // For NUL.
  if (!alloc_size_safe.IsValid() || alloc_size_safe.ValueOrDie() <= 0) {
    m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
    return;
  }

  m_pBuffer = FX_Alloc(FX_WCHAR, alloc_size_safe.ValueOrDie());
  m_pStart = m_pEnd = m_pBuffer;
  ASSERT(!m_BlockBuffer.IsInitialized());
  m_BlockBuffer.InitBuffer();
  m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
  m_iParsedBytes = m_iParsedChars = 0;
  m_iBufferChars = 0;
}

FDE_XmlSyntaxResult CFDE_XMLSyntaxParser::DoSyntaxParse() {
  if (m_syntaxParserResult == FDE_XmlSyntaxResult::Error ||
      m_syntaxParserResult == FDE_XmlSyntaxResult::EndOfString) {
    return m_syntaxParserResult;
  }
  ASSERT(m_pStream && m_pBuffer && m_BlockBuffer.IsInitialized());
  int32_t iStreamLength = m_pStream->GetLength();
  int32_t iPos;

  FDE_XmlSyntaxResult syntaxParserResult = FDE_XmlSyntaxResult::None;
  while (TRUE) {
    if (m_pStart >= m_pEnd) {
      if (m_bEOS || m_iCurrentPos >= iStreamLength) {
        m_syntaxParserResult = FDE_XmlSyntaxResult::EndOfString;
        return m_syntaxParserResult;
      }
      m_iParsedChars += (m_pEnd - m_pBuffer);
      m_iParsedBytes = m_iCurrentPos;
      if (m_pStream->GetPosition() != m_iCurrentPos) {
        m_pStream->Seek(FX_STREAMSEEK_Begin, m_iCurrentPos);
      }
      m_iBufferChars =
          m_pStream->ReadString(m_pBuffer, m_iXMLPlaneSize, m_bEOS);
      iPos = m_pStream->GetPosition();
      if (m_iBufferChars < 1) {
        m_iCurrentPos = iStreamLength;
        m_syntaxParserResult = FDE_XmlSyntaxResult::EndOfString;
        return m_syntaxParserResult;
      }
      m_iCurrentPos = iPos;
      m_pStart = m_pBuffer;
      m_pEnd = m_pBuffer + m_iBufferChars;
    }

    while (m_pStart < m_pEnd) {
      FX_WCHAR ch = *m_pStart;
      switch (m_syntaxParserState) {
        case FDE_XmlSyntaxState::Text:
          if (ch == L'<') {
            if (m_iDataLength > 0) {
              m_iTextDataLength = m_iDataLength;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              m_iEntityStart = -1;
              syntaxParserResult = FDE_XmlSyntaxResult::Text;
            } else {
              m_pStart++;
              m_syntaxParserState = FDE_XmlSyntaxState::Node;
            }
          } else {
            ParseTextChar(ch);
          }
          break;
        case FDE_XmlSyntaxState::Node:
          if (ch == L'!') {
            m_pStart++;
            m_syntaxParserState = FDE_XmlSyntaxState::SkipCommentOrDecl;
          } else if (ch == L'/') {
            m_pStart++;
            m_syntaxParserState = FDE_XmlSyntaxState::CloseElement;
          } else if (ch == L'?') {
            m_iLastNodeNum++;
            m_iCurrentNodeNum = m_iLastNodeNum;
            m_CurNode.iNodeNum = m_iLastNodeNum;
            m_CurNode.eNodeType = FDE_XMLNODE_Instruction;
            m_XMLNodeStack.Push(m_CurNode);
            m_pStart++;
            m_syntaxParserState = FDE_XmlSyntaxState::Target;
            syntaxParserResult = FDE_XmlSyntaxResult::InstructionOpen;
          } else {
            m_iLastNodeNum++;
            m_iCurrentNodeNum = m_iLastNodeNum;
            m_CurNode.iNodeNum = m_iLastNodeNum;
            m_CurNode.eNodeType = FDE_XMLNODE_Element;
            m_XMLNodeStack.Push(m_CurNode);
            m_syntaxParserState = FDE_XmlSyntaxState::Tag;
            syntaxParserResult = FDE_XmlSyntaxResult::ElementOpen;
          }
          break;
        case FDE_XmlSyntaxState::Target:
        case FDE_XmlSyntaxState::Tag:
          if (!FDE_IsXMLNameChar(ch, m_iDataLength < 1)) {
            if (m_iDataLength < 1) {
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            } else {
              m_iTextDataLength = m_iDataLength;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (m_syntaxParserState != FDE_XmlSyntaxState::Target) {
                syntaxParserResult = FDE_XmlSyntaxResult::TagName;
              } else {
                syntaxParserResult = FDE_XmlSyntaxResult::TargetName;
              }
              m_syntaxParserState = FDE_XmlSyntaxState::AttriName;
            }
          } else {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock) {
                return FDE_XmlSyntaxResult::Error;
              }
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
            m_pStart++;
          }
          break;
        case FDE_XmlSyntaxState::AttriName:
          if (m_iDataLength < 1 && FDE_IsXMLWhiteSpace(ch)) {
            m_pStart++;
            break;
          }
          if (!FDE_IsXMLNameChar(ch, m_iDataLength < 1)) {
            if (m_iDataLength < 1) {
              if (m_CurNode.eNodeType == FDE_XMLNODE_Element) {
                if (ch == L'>' || ch == L'/') {
                  m_syntaxParserState = FDE_XmlSyntaxState::BreakElement;
                  break;
                }
              } else if (m_CurNode.eNodeType == FDE_XMLNODE_Instruction) {
                if (ch == L'?') {
                  m_syntaxParserState = FDE_XmlSyntaxState::CloseInstruction;
                  m_pStart++;
                } else {
                  m_syntaxParserState = FDE_XmlSyntaxState::TargetData;
                }
                break;
              }
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            } else {
              if (m_CurNode.eNodeType == FDE_XMLNODE_Instruction) {
                if (ch != '=' && !FDE_IsXMLWhiteSpace(ch)) {
                  m_syntaxParserState = FDE_XmlSyntaxState::TargetData;
                  break;
                }
              }
              m_iTextDataLength = m_iDataLength;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              m_syntaxParserState = FDE_XmlSyntaxState::AttriEqualSign;
              syntaxParserResult = FDE_XmlSyntaxResult::AttriName;
            }
          } else {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock) {
                return FDE_XmlSyntaxResult::Error;
              }
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
            m_pStart++;
          }
          break;
        case FDE_XmlSyntaxState::AttriEqualSign:
          if (FDE_IsXMLWhiteSpace(ch)) {
            m_pStart++;
            break;
          }
          if (ch != L'=') {
            if (m_CurNode.eNodeType == FDE_XMLNODE_Instruction) {
              m_syntaxParserState = FDE_XmlSyntaxState::TargetData;
              break;
            }
            m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
            return m_syntaxParserResult;
          } else {
            m_syntaxParserState = FDE_XmlSyntaxState::AttriQuotation;
            m_pStart++;
          }
          break;
        case FDE_XmlSyntaxState::AttriQuotation:
          if (FDE_IsXMLWhiteSpace(ch)) {
            m_pStart++;
            break;
          }
          if (ch != L'\"' && ch != L'\'') {
            m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
            return m_syntaxParserResult;
          } else {
            m_wQuotationMark = ch;
            m_syntaxParserState = FDE_XmlSyntaxState::AttriValue;
            m_pStart++;
          }
          break;
        case FDE_XmlSyntaxState::AttriValue:
          if (ch == m_wQuotationMark) {
            if (m_iEntityStart > -1) {
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            }
            m_iTextDataLength = m_iDataLength;
            m_wQuotationMark = 0;
            m_BlockBuffer.Reset();
            m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
            m_pStart++;
            m_syntaxParserState = FDE_XmlSyntaxState::AttriName;
            syntaxParserResult = FDE_XmlSyntaxResult::AttriValue;
          } else {
            ParseTextChar(ch);
          }
          break;
        case FDE_XmlSyntaxState::CloseInstruction:
          if (ch != L'>') {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock) {
                return FDE_XmlSyntaxResult::Error;
              }
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
            m_syntaxParserState = FDE_XmlSyntaxState::TargetData;
          } else if (m_iDataLength > 0) {
            m_iTextDataLength = m_iDataLength;
            m_BlockBuffer.Reset();
            m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
            syntaxParserResult = FDE_XmlSyntaxResult::TargetData;
          } else {
            m_pStart++;
            FDE_XMLNODE* pXMLNode = m_XMLNodeStack.GetTopElement();
            if (!pXMLNode) {
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            }
            m_XMLNodeStack.Pop();
            pXMLNode = m_XMLNodeStack.GetTopElement();
            if (pXMLNode) {
              m_CurNode = *pXMLNode;
            } else {
              m_CurNode.iNodeNum = -1;
              m_CurNode.eNodeType = FDE_XMLNODE_Unknown;
            }
            m_iCurrentNodeNum = m_CurNode.iNodeNum;
            m_BlockBuffer.Reset();
            m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
            m_syntaxParserState = FDE_XmlSyntaxState::Text;
            syntaxParserResult = FDE_XmlSyntaxResult::InstructionClose;
          }
          break;
        case FDE_XmlSyntaxState::BreakElement:
          if (ch == L'>') {
            m_syntaxParserState = FDE_XmlSyntaxState::Text;
            syntaxParserResult = FDE_XmlSyntaxResult::ElementBreak;
          } else if (ch == L'/') {
            m_syntaxParserState = FDE_XmlSyntaxState::CloseElement;
          } else {
            m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
            return m_syntaxParserResult;
          }
          m_pStart++;
          break;
        case FDE_XmlSyntaxState::CloseElement:
          if (!FDE_IsXMLNameChar(ch, m_iDataLength < 1)) {
            if (ch == L'>') {
              FDE_XMLNODE* pXMLNode = m_XMLNodeStack.GetTopElement();
              if (!pXMLNode) {
                m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
                return m_syntaxParserResult;
              }
              m_XMLNodeStack.Pop();
              pXMLNode = m_XMLNodeStack.GetTopElement();
              if (pXMLNode) {
                m_CurNode = *pXMLNode;
              } else {
                m_CurNode.iNodeNum = -1;
                m_CurNode.eNodeType = FDE_XMLNODE_Unknown;
              }
              m_iCurrentNodeNum = m_CurNode.iNodeNum;
              m_iTextDataLength = m_iDataLength;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              m_syntaxParserState = FDE_XmlSyntaxState::Text;
              syntaxParserResult = FDE_XmlSyntaxResult::ElementClose;
            } else if (!FDE_IsXMLWhiteSpace(ch)) {
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            }
          } else {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock) {
                return FDE_XmlSyntaxResult::Error;
              }
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
          }
          m_pStart++;
          break;
        case FDE_XmlSyntaxState::SkipCommentOrDecl:
          if (FX_wcsnicmp(m_pStart, L"--", 2) == 0) {
            m_pStart += 2;
            m_syntaxParserState = FDE_XmlSyntaxState::SkipComment;
          } else if (FX_wcsnicmp(m_pStart, L"[CDATA[", 7) == 0) {
            m_pStart += 7;
            m_syntaxParserState = FDE_XmlSyntaxState::SkipCData;
          } else {
            m_syntaxParserState = FDE_XmlSyntaxState::SkipDeclNode;
            m_SkipChar = L'>';
            m_SkipStack.Push(L'>');
          }
          break;
        case FDE_XmlSyntaxState::SkipCData: {
          if (FX_wcsnicmp(m_pStart, L"]]>", 3) == 0) {
            m_pStart += 3;
            syntaxParserResult = FDE_XmlSyntaxResult::CData;
            m_iTextDataLength = m_iDataLength;
            m_BlockBuffer.Reset();
            m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
            m_syntaxParserState = FDE_XmlSyntaxState::Text;
          } else {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock)
                return FDE_XmlSyntaxResult::Error;
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
            m_pStart++;
          }
          break;
        }
        case FDE_XmlSyntaxState::SkipDeclNode:
          if (m_SkipChar == L'\'' || m_SkipChar == L'\"') {
            m_pStart++;
            if (ch != m_SkipChar)
              break;

            m_SkipStack.Pop();
            uint32_t* pDWord = m_SkipStack.GetTopElement();
            if (!pDWord)
              m_syntaxParserState = FDE_XmlSyntaxState::Text;
            else
              m_SkipChar = (FX_WCHAR)*pDWord;
          } else {
            switch (ch) {
              case L'<':
                m_SkipChar = L'>';
                m_SkipStack.Push(L'>');
                break;
              case L'[':
                m_SkipChar = L']';
                m_SkipStack.Push(L']');
                break;
              case L'(':
                m_SkipChar = L')';
                m_SkipStack.Push(L')');
                break;
              case L'\'':
                m_SkipChar = L'\'';
                m_SkipStack.Push(L'\'');
                break;
              case L'\"':
                m_SkipChar = L'\"';
                m_SkipStack.Push(L'\"');
                break;
              default:
                if (ch == m_SkipChar) {
                  m_SkipStack.Pop();
                  uint32_t* pDWord = m_SkipStack.GetTopElement();
                  if (!pDWord) {
                    if (m_iDataLength >= 9) {
                      CFX_WideString wsHeader;
                      m_BlockBuffer.GetTextData(wsHeader, 0, 7);
                    }
                    m_iTextDataLength = m_iDataLength;
                    m_BlockBuffer.Reset();
                    m_pCurrentBlock =
                        m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
                    m_syntaxParserState = FDE_XmlSyntaxState::Text;
                  } else {
                    m_SkipChar = static_cast<FX_WCHAR>(*pDWord);
                  }
                }
                break;
            }
            if (m_SkipStack.GetSize() > 0) {
              if (m_iIndexInBlock == m_iAllocStep) {
                m_pCurrentBlock =
                    m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
                if (!m_pCurrentBlock) {
                  return FDE_XmlSyntaxResult::Error;
                }
              }
              m_pCurrentBlock[m_iIndexInBlock++] = ch;
              m_iDataLength++;
            }
            m_pStart++;
          }
          break;
        case FDE_XmlSyntaxState::SkipComment:
          if (FX_wcsnicmp(m_pStart, L"-->", 3) == 0) {
            m_pStart += 2;
            m_syntaxParserState = FDE_XmlSyntaxState::Text;
          }

          m_pStart++;
          break;
        case FDE_XmlSyntaxState::TargetData:
          if (FDE_IsXMLWhiteSpace(ch)) {
            if (m_iDataLength < 1) {
              m_pStart++;
              break;
            } else if (m_wQuotationMark == 0) {
              m_iTextDataLength = m_iDataLength;
              m_wQuotationMark = 0;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              m_pStart++;
              syntaxParserResult = FDE_XmlSyntaxResult::TargetData;
              break;
            }
          }
          if (ch == '?') {
            m_syntaxParserState = FDE_XmlSyntaxState::CloseInstruction;
            m_pStart++;
          } else if (ch == '\"') {
            if (m_wQuotationMark == 0) {
              m_wQuotationMark = ch;
              m_pStart++;
            } else if (ch == m_wQuotationMark) {
              m_iTextDataLength = m_iDataLength;
              m_wQuotationMark = 0;
              m_BlockBuffer.Reset();
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              m_pStart++;
              syntaxParserResult = FDE_XmlSyntaxResult::TargetData;
            } else {
              m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
              return m_syntaxParserResult;
            }
          } else {
            if (m_iIndexInBlock == m_iAllocStep) {
              m_pCurrentBlock =
                  m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
              if (!m_pCurrentBlock) {
                return FDE_XmlSyntaxResult::Error;
              }
            }
            m_pCurrentBlock[m_iIndexInBlock++] = ch;
            m_iDataLength++;
            m_pStart++;
          }
          break;
        default:
          break;
      }
      if (syntaxParserResult != FDE_XmlSyntaxResult::None)
        return syntaxParserResult;
    }
  }
  return FDE_XmlSyntaxResult::Text;
}

CFDE_XMLSyntaxParser::~CFDE_XMLSyntaxParser() {
  m_pCurrentBlock = nullptr;
  FX_Free(m_pBuffer);
}

int32_t CFDE_XMLSyntaxParser::GetStatus() const {
  if (!m_pStream)
    return -1;

  int32_t iStreamLength = m_pStream->GetLength();
  if (iStreamLength < 1)
    return 100;

  if (m_syntaxParserResult == FDE_XmlSyntaxResult::Error)
    return -1;

  if (m_syntaxParserResult == FDE_XmlSyntaxResult::EndOfString)
    return 100;
  return m_iParsedBytes * 100 / iStreamLength;
}

static int32_t FX_GetUTF8EncodeLength(const FX_WCHAR* pSrc, int32_t iSrcLen) {
  uint32_t unicode = 0;
  int32_t iDstNum = 0;
  while (iSrcLen-- > 0) {
    unicode = *pSrc++;
    int nbytes = 0;
    if ((uint32_t)unicode < 0x80) {
      nbytes = 1;
    } else if ((uint32_t)unicode < 0x800) {
      nbytes = 2;
    } else if ((uint32_t)unicode < 0x10000) {
      nbytes = 3;
    } else if ((uint32_t)unicode < 0x200000) {
      nbytes = 4;
    } else if ((uint32_t)unicode < 0x4000000) {
      nbytes = 5;
    } else {
      nbytes = 6;
    }
    iDstNum += nbytes;
  }
  return iDstNum;
}

FX_FILESIZE CFDE_XMLSyntaxParser::GetCurrentBinaryPos() const {
  if (!m_pStream)
    return 0;

  int32_t nSrcLen = m_pStart - m_pBuffer;
  int32_t nDstLen = FX_GetUTF8EncodeLength(m_pBuffer, nSrcLen);
  return m_iParsedBytes + nDstLen;
}

void CFDE_XMLSyntaxParser::ParseTextChar(FX_WCHAR ch) {
  if (m_iIndexInBlock == m_iAllocStep) {
    m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
    if (!m_pCurrentBlock) {
      return;
    }
  }
  m_pCurrentBlock[m_iIndexInBlock++] = ch;
  m_iDataLength++;
  if (m_iEntityStart > -1 && ch == L';') {
    CFX_WideString csEntity;
    m_BlockBuffer.GetTextData(csEntity, m_iEntityStart + 1,
                              (m_iDataLength - 1) - m_iEntityStart - 1);
    int32_t iLen = csEntity.GetLength();
    if (iLen > 0) {
      if (csEntity[0] == L'#') {
        ch = 0;
        FX_WCHAR w;
        if (iLen > 1 && csEntity[1] == L'x') {
          for (int32_t i = 2; i < iLen; i++) {
            w = csEntity[i];
            if (w >= L'0' && w <= L'9') {
              ch = (ch << 4) + w - L'0';
            } else if (w >= L'A' && w <= L'F') {
              ch = (ch << 4) + w - 55;
            } else if (w >= L'a' && w <= L'f') {
              ch = (ch << 4) + w - 87;
            } else {
              break;
            }
          }
        } else {
          for (int32_t i = 1; i < iLen; i++) {
            w = csEntity[i];
            if (w < L'0' || w > L'9') {
              break;
            }
            ch = ch * 10 + w - L'0';
          }
        }
        if (ch != 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, ch);
          m_iEntityStart++;
        }
      } else {
        if (csEntity.Compare(L"amp") == 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, L'&');
          m_iEntityStart++;
        } else if (csEntity.Compare(L"lt") == 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, L'<');
          m_iEntityStart++;
        } else if (csEntity.Compare(L"gt") == 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, L'>');
          m_iEntityStart++;
        } else if (csEntity.Compare(L"apos") == 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, L'\'');
          m_iEntityStart++;
        } else if (csEntity.Compare(L"quot") == 0) {
          m_BlockBuffer.SetTextChar(m_iEntityStart, L'\"');
          m_iEntityStart++;
        }
      }
    }
    m_BlockBuffer.DeleteTextChars(m_iDataLength - m_iEntityStart, FALSE);
    m_pCurrentBlock = m_BlockBuffer.GetAvailableBlock(m_iIndexInBlock);
    m_iEntityStart = -1;
  } else {
    if (m_iEntityStart < 0 && ch == L'&') {
      m_iEntityStart = m_iDataLength - 1;
    }
  }
  m_pStart++;
}
