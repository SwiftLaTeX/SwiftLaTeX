// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_UTILS_H_
#define XFA_FXFA_PARSER_XFA_UTILS_H_

#include "xfa/fde/xml/fde_xml.h"
#include "xfa/fgas/crt/fgas_stream.h"
#include "xfa/fgas/crt/fgas_utils.h"
#include "xfa/fxfa/include/fxfa_basic.h"

class CFDE_XMLElement;
class CFDE_XMLNode;
class CXFA_LocaleValue;
class CXFA_Node;
class CXFA_WidgetData;

FX_BOOL XFA_FDEExtension_ResolveNamespaceQualifier(
    CFDE_XMLElement* pNode,
    const CFX_WideStringC& wsQualifier,
    CFX_WideString& wsNamespaceURI);

template <class NodeType, class TraverseStrategy>
class CXFA_NodeIteratorTemplate {
 public:
  CXFA_NodeIteratorTemplate(NodeType* pRootNode = nullptr)
      : m_pRoot(pRootNode) {
    if (pRootNode) {
      m_NodeStack.Push(pRootNode);
    }
  }
  FX_BOOL Init(NodeType* pRootNode) {
    if (!pRootNode) {
      return FALSE;
    }
    m_pRoot = pRootNode;
    m_NodeStack.RemoveAll();
    m_NodeStack.Push(pRootNode);
    return TRUE;
  }
  void Clear() { m_NodeStack.RemoveAll(); }
  void Reset() {
    Clear();
    if (m_pRoot) {
      m_NodeStack.Push(m_pRoot);
    }
  }
  FX_BOOL SetCurrent(NodeType* pCurNode) {
    m_NodeStack.RemoveAll();
    if (pCurNode) {
      CFX_StackTemplate<NodeType*> revStack;
      NodeType* pNode;
      for (pNode = pCurNode; pNode && pNode != m_pRoot;
           pNode = TraverseStrategy::GetParent(pNode)) {
        revStack.Push(pNode);
      }
      if (!pNode) {
        return FALSE;
      }
      revStack.Push(m_pRoot);
      while (revStack.GetSize()) {
        m_NodeStack.Push(*revStack.GetTopElement());
        revStack.Pop();
      }
    }
    return TRUE;
  }
  NodeType* GetCurrent() const {
    return m_NodeStack.GetSize() ? *m_NodeStack.GetTopElement() : nullptr;
  }
  NodeType* GetRoot() const { return m_pRoot; }
  NodeType* MoveToPrev() {
    int32_t nStackLength = m_NodeStack.GetSize();
    if (nStackLength == 1) {
      return nullptr;
    } else if (nStackLength > 1) {
      NodeType* pCurItem = *m_NodeStack.GetTopElement();
      m_NodeStack.Pop();
      NodeType* pParentItem = *m_NodeStack.GetTopElement();
      NodeType* pParentFirstChildItem =
          TraverseStrategy::GetFirstChild(pParentItem);
      if (pCurItem == pParentFirstChildItem) {
        return pParentItem;
      }
      NodeType *pPrevItem = pParentFirstChildItem, *pPrevItemNext = nullptr;
      for (; pPrevItem; pPrevItem = pPrevItemNext) {
        pPrevItemNext = TraverseStrategy::GetNextSibling(pPrevItem);
        if (!pPrevItemNext || pPrevItemNext == pCurItem) {
          break;
        }
      }
      m_NodeStack.Push(pPrevItem);
    } else {
      m_NodeStack.RemoveAll();
      if (m_pRoot) {
        m_NodeStack.Push(m_pRoot);
      }
    }
    if (m_NodeStack.GetSize() > 0) {
      NodeType* pChildItem = *m_NodeStack.GetTopElement();
      while ((pChildItem = TraverseStrategy::GetFirstChild(pChildItem)) !=
             nullptr) {
        while (NodeType* pNextItem =
                   TraverseStrategy::GetNextSibling(pChildItem)) {
          pChildItem = pNextItem;
        }
        m_NodeStack.Push(pChildItem);
      }
      return *m_NodeStack.GetTopElement();
    }
    return nullptr;
  }
  NodeType* MoveToNext() {
    NodeType** ppNode = nullptr;
    NodeType* pCurrent = GetCurrent();
    while (m_NodeStack.GetSize() > 0) {
      while ((ppNode = m_NodeStack.GetTopElement()) != nullptr) {
        if (pCurrent != *ppNode) {
          return *ppNode;
        }
        NodeType* pChild = TraverseStrategy::GetFirstChild(*ppNode);
        if (!pChild) {
          break;
        }
        m_NodeStack.Push(pChild);
      }
      while ((ppNode = m_NodeStack.GetTopElement()) != nullptr) {
        NodeType* pNext = TraverseStrategy::GetNextSibling(*ppNode);
        m_NodeStack.Pop();
        if (m_NodeStack.GetSize() == 0) {
          break;
        }
        if (pNext) {
          m_NodeStack.Push(pNext);
          break;
        }
      }
    }
    return nullptr;
  }
  NodeType* SkipChildrenAndMoveToNext() {
    NodeType** ppNode = nullptr;
    while ((ppNode = m_NodeStack.GetTopElement()) != nullptr) {
      NodeType* pNext = TraverseStrategy::GetNextSibling(*ppNode);
      m_NodeStack.Pop();
      if (m_NodeStack.GetSize() == 0) {
        break;
      }
      if (pNext) {
        m_NodeStack.Push(pNext);
        break;
      }
    }
    return GetCurrent();
  }

 protected:
  NodeType* m_pRoot;
  CFX_StackTemplate<NodeType*> m_NodeStack;
};

CXFA_LocaleValue XFA_GetLocaleValue(CXFA_WidgetData* pWidgetData);
FX_DOUBLE XFA_ByteStringToDouble(const CFX_ByteStringC& szStringVal);
int32_t XFA_MapRotation(int32_t nRotation);

FX_BOOL XFA_RecognizeRichText(CFDE_XMLElement* pRichTextXMLNode);
void XFA_GetPlainTextFromRichText(CFDE_XMLNode* pXMLNode,
                                  CFX_WideString& wsPlainText);
FX_BOOL XFA_FieldIsMultiListBox(CXFA_Node* pFieldNode);
IFX_Stream* XFA_CreateWideTextRead(const CFX_WideString& wsBuffer);

void XFA_DataExporter_DealWithDataGroupNode(CXFA_Node* pDataNode);
void XFA_DataExporter_RegenerateFormFile(CXFA_Node* pNode,
                                         IFX_Stream* pStream,
                                         const FX_CHAR* pChecksum = nullptr,
                                         FX_BOOL bSaveXML = FALSE);

#endif  // XFA_FXFA_PARSER_XFA_UTILS_H_
