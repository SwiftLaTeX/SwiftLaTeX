// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_resolveprocessor.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_script_nodehelper.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_ResolveProcessor::CXFA_ResolveProcessor()
    : m_iCurStart(0), m_pNodeHelper(new CXFA_NodeHelper) {}

CXFA_ResolveProcessor::~CXFA_ResolveProcessor() {
  delete m_pNodeHelper;
}

int32_t CXFA_ResolveProcessor::Resolve(CXFA_ResolveNodesData& rnd) {
  if (!rnd.m_CurNode) {
    return -1;
  }
  if (!rnd.m_CurNode->IsNode()) {
    if (rnd.m_dwStyles & XFA_RESOLVENODE_Attributes) {
      return ResolveForAttributeRs(rnd.m_CurNode, rnd,
                                   rnd.m_wsName.AsStringC());
    }
    return 0;
  }
  if (rnd.m_dwStyles & XFA_RESOLVENODE_AnyChild) {
    return ResolveAnyChild(rnd);
  }
  FX_WCHAR wch = rnd.m_wsName.GetAt(0);
  switch (wch) {
    case '$':
      return ResolveDollar(rnd);
    case '!':
      return ResolveExcalmatory(rnd);
    case '#':
      return ResolveNumberSign(rnd);
    case '*':
      return ResolveAsterisk(rnd);
    // TODO(dsinclair): We could probably remove this.
    case '.':
      return ResolveAnyChild(rnd);
    default:
      break;
  }
  if (rnd.m_uHashName == XFA_HASHCODE_This && rnd.m_nLevel == 0) {
    rnd.m_Nodes.Add(rnd.m_pSC->GetThisObject());
    return 1;
  } else if (rnd.m_CurNode->GetElementType() == XFA_Element::Xfa) {
    CXFA_Object* pObjNode =
        rnd.m_pSC->GetDocument()->GetXFAObject(rnd.m_uHashName);
    if (pObjNode) {
      rnd.m_Nodes.Add(pObjNode);
    } else if (rnd.m_uHashName == XFA_HASHCODE_Xfa) {
      rnd.m_Nodes.Add(rnd.m_CurNode);
    } else if ((rnd.m_dwStyles & XFA_RESOLVENODE_Attributes) &&
               ResolveForAttributeRs(rnd.m_CurNode, rnd,
                                     rnd.m_wsName.AsStringC())) {
      return 1;
    }
    if (rnd.m_Nodes.GetSize() > 0) {
      FilterCondition(rnd, rnd.m_wsCondition);
    }
    return rnd.m_Nodes.GetSize();
  }
  int32_t nRet = ResolveNormal(rnd);
  if (nRet < 1 && rnd.m_uHashName == XFA_HASHCODE_Xfa) {
    rnd.m_Nodes.Add(rnd.m_pSC->GetDocument()->GetRoot());
  }
  return rnd.m_Nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolveAnyChild(CXFA_ResolveNodesData& rnd) {
  CFX_WideString wsName = rnd.m_wsName;
  CFX_WideString wsCondition = rnd.m_wsCondition;
  CXFA_Node* findNode = nullptr;
  CXFA_NodeArray siblings;
  FX_BOOL bClassName = FALSE;
  if (wsName.GetAt(0) == '#') {
    bClassName = TRUE;
    wsName = wsName.Right(wsName.GetLength() - 1);
  }
  findNode = m_pNodeHelper->ResolveNodes_GetOneChild(
      ToNode(rnd.m_CurNode), wsName.c_str(), bClassName);
  if (!findNode) {
    return 0;
  }
  if (wsCondition.IsEmpty()) {
    rnd.m_Nodes.Add(findNode);
    return rnd.m_Nodes.GetSize();
  }
  m_pNodeHelper->CountSiblings(findNode, XFA_LOGIC_Transparent,
                               (CXFA_NodeArray*)&rnd.m_Nodes, bClassName);
  FilterCondition(rnd, wsCondition);
  return rnd.m_Nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolveDollar(CXFA_ResolveNodesData& rnd) {
  CXFA_ObjArray& nodes = rnd.m_Nodes;
  CFX_WideString wsName = rnd.m_wsName;
  CFX_WideString wsCondition = rnd.m_wsCondition;
  int32_t iNameLen = wsName.GetLength();
  if (iNameLen == 1) {
    nodes.Add(rnd.m_CurNode);
    return 1;
  }
  if (rnd.m_nLevel > 0) {
    return -1;
  }
  XFA_HashCode dwNameHash = static_cast<XFA_HashCode>(FX_HashCode_GetW(
      CFX_WideStringC(wsName.c_str() + 1, iNameLen - 1), false));
  if (dwNameHash == XFA_HASHCODE_Xfa) {
    nodes.Add(rnd.m_pSC->GetDocument()->GetRoot());
  } else {
    CXFA_Object* pObjNode = rnd.m_pSC->GetDocument()->GetXFAObject(dwNameHash);
    if (pObjNode) {
      rnd.m_Nodes.Add(pObjNode);
    }
  }
  if (rnd.m_Nodes.GetSize() > 0) {
    FilterCondition(rnd, wsCondition);
  }
  return rnd.m_Nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolveExcalmatory(CXFA_ResolveNodesData& rnd) {
  if (rnd.m_nLevel > 0) {
    return 0;
  }
  CXFA_Node* datasets =
      ToNode(rnd.m_pSC->GetDocument()->GetXFAObject(XFA_HASHCODE_Datasets));
  if (!datasets) {
    return 0;
  }
  CXFA_ResolveNodesData rndFind;
  rndFind.m_pSC = rnd.m_pSC;
  rndFind.m_CurNode = datasets;
  rndFind.m_wsName = rnd.m_wsName.Right(rnd.m_wsName.GetLength() - 1);
  rndFind.m_uHashName = static_cast<XFA_HashCode>(
      FX_HashCode_GetW(rndFind.m_wsName.AsStringC(), false));
  rndFind.m_nLevel = rnd.m_nLevel + 1;
  rndFind.m_dwStyles = XFA_RESOLVENODE_Children;
  rndFind.m_wsCondition = rnd.m_wsCondition;
  Resolve(rndFind);
  if (rndFind.m_Nodes.GetSize() > 0) {
    rnd.m_Nodes.Append(rndFind.m_Nodes);
    rndFind.m_Nodes.RemoveAll();
  }
  return rnd.m_Nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolveNumberSign(CXFA_ResolveNodesData& rnd) {
  CFX_WideString wsName = rnd.m_wsName.Right(rnd.m_wsName.GetLength() - 1);
  CFX_WideString wsCondition = rnd.m_wsCondition;
  CXFA_Node* curNode = ToNode(rnd.m_CurNode);
  if (ResolveForAttributeRs(curNode, rnd, wsName.AsStringC())) {
    return 1;
  }
  CXFA_ResolveNodesData rndFind;
  rndFind.m_pSC = rnd.m_pSC;
  rndFind.m_nLevel = rnd.m_nLevel + 1;
  rndFind.m_dwStyles = rnd.m_dwStyles;
  rndFind.m_dwStyles |= XFA_RESOLVENODE_TagName;
  rndFind.m_dwStyles &= ~XFA_RESOLVENODE_Attributes;
  rndFind.m_wsName = wsName;
  rndFind.m_uHashName = static_cast<XFA_HashCode>(
      FX_HashCode_GetW(rndFind.m_wsName.AsStringC(), false));
  rndFind.m_wsCondition = wsCondition;
  rndFind.m_CurNode = curNode;
  ResolveNormal(rndFind);
  if (rndFind.m_Nodes.GetSize() > 0) {
    if (wsCondition.GetLength() == 0 && rndFind.m_Nodes.Find(curNode) >= 0) {
      rnd.m_Nodes.Add(curNode);
    } else {
      rnd.m_Nodes.Append(rndFind.m_Nodes);
      rndFind.m_Nodes.RemoveAll();
    }
  }
  return rnd.m_Nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolveForAttributeRs(
    CXFA_Object* curNode,
    CXFA_ResolveNodesData& rnd,
    const CFX_WideStringC& strAttr) {
  const XFA_SCRIPTATTRIBUTEINFO* lpScriptAttribute =
      XFA_GetScriptAttributeByName(curNode->GetElementType(), strAttr);
  if (lpScriptAttribute) {
    rnd.m_pScriptAttribute = lpScriptAttribute;
    rnd.m_Nodes.Add(curNode);
    rnd.m_dwFlag = XFA_RESOVENODE_RSTYPE_Attribute;
    return 1;
  }
  return 0;
}
int32_t CXFA_ResolveProcessor::ResolveNormal(CXFA_ResolveNodesData& rnd) {
  if (rnd.m_nLevel > 32) {
    return 0;
  }
  if (!rnd.m_CurNode->IsNode()) {
    return 0;
  }
  CXFA_Node* curNode = ToNode(rnd.m_CurNode);
  CXFA_ObjArray& nodes = rnd.m_Nodes;
  int32_t nNum = nodes.GetSize();
  uint32_t dwStyles = rnd.m_dwStyles;
  CFX_WideString& wsName = rnd.m_wsName;
  XFA_HashCode uNameHash = rnd.m_uHashName;
  CFX_WideString& wsCondition = rnd.m_wsCondition;
  CXFA_ResolveNodesData rndFind;
  rndFind.m_wsName = rnd.m_wsName;
  rndFind.m_wsCondition = rnd.m_wsCondition;
  rndFind.m_pSC = rnd.m_pSC;
  rndFind.m_nLevel = rnd.m_nLevel + 1;
  rndFind.m_uHashName = uNameHash;
  CXFA_NodeArray children;
  CXFA_NodeArray properties;
  CXFA_Node* pVariablesNode = nullptr;
  CXFA_Node* pPageSetNode = nullptr;
  CXFA_Node* pChild = curNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  while (pChild) {
    if (pChild->GetElementType() == XFA_Element::Variables) {
      pVariablesNode = pChild;
      pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
      continue;
    } else if (pChild->GetElementType() == XFA_Element::PageSet) {
      pPageSetNode = pChild;
      pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
      continue;
    } else {
      const XFA_PROPERTY* pPropert = XFA_GetPropertyOfElement(
          curNode->GetElementType(), pChild->GetElementType(),
          XFA_XDPPACKET_UNKNOWN);
      if (pPropert) {
        properties.Add(pChild);
      } else {
        children.Add(pChild);
      }
    }
    pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  if ((dwStyles & XFA_RESOLVENODE_Properties) && pVariablesNode) {
    uint32_t uPropHash = pVariablesNode->GetClassHashCode();
    if (uPropHash == uNameHash) {
      nodes.Add(pVariablesNode);
    } else {
      rndFind.m_CurNode = pVariablesNode;
      SetStylesForChild(dwStyles, rndFind);
      CFX_WideString wsSaveCondition = rndFind.m_wsCondition;
      rndFind.m_wsCondition.clear();
      ResolveNormal(rndFind);
      rndFind.m_wsCondition = wsSaveCondition;
      if (rndFind.m_Nodes.GetSize() > 0) {
        nodes.Append(rndFind.m_Nodes);
        rndFind.m_Nodes.RemoveAll();
      }
    }
    if (nodes.GetSize() > nNum) {
      FilterCondition(rnd, wsCondition);
      if (nodes.GetSize() > 0) {
        return 1;
      }
      return 0;
    }
  }
  if (dwStyles & XFA_RESOLVENODE_Children) {
    FX_BOOL bSetFlag = FALSE;
    if (pPageSetNode && (dwStyles & XFA_RESOLVENODE_Properties)) {
      children.Add(pPageSetNode);
    }
    for (int32_t i = 0; i < children.GetSize(); i++) {
      CXFA_Node* child = children[i];
      if (dwStyles & XFA_RESOLVENODE_TagName) {
        if (child->GetClassHashCode() == uNameHash) {
          nodes.Add(child);
        }
      } else if (child->GetNameHash() == uNameHash) {
        nodes.Add(child);
      }
      if (m_pNodeHelper->NodeIsTransparent(child) &&
          child->GetElementType() != XFA_Element::PageSet) {
        if (!bSetFlag) {
          SetStylesForChild(dwStyles, rndFind);
          bSetFlag = TRUE;
        }
        rndFind.m_CurNode = child;
        CFX_WideString wsSaveCondition = rndFind.m_wsCondition;
        rndFind.m_wsCondition.clear();
        ResolveNormal(rndFind);
        rndFind.m_wsCondition = wsSaveCondition;
        if (rndFind.m_Nodes.GetSize() > 0) {
          nodes.Append(rndFind.m_Nodes);
          rndFind.m_Nodes.RemoveAll();
        }
      }
    }
    if (nodes.GetSize() > nNum) {
      if (!(dwStyles & XFA_RESOLVENODE_ALL)) {
        CXFA_NodeArray upArrayNodes;
        if (m_pNodeHelper->NodeIsTransparent(ToNode(curNode))) {
          m_pNodeHelper->CountSiblings(ToNode(nodes[0]), XFA_LOGIC_Transparent,
                                       &upArrayNodes,
                                       !!(dwStyles & XFA_RESOLVENODE_TagName));
        }
        if (upArrayNodes.GetSize() > nodes.GetSize()) {
          upArrayNodes[0] = ToNode(nodes[0]);
          nodes.RemoveAll();
          nodes.Append((CXFA_ObjArray&)upArrayNodes);
          upArrayNodes.RemoveAll();
        }
      }
      FilterCondition(rnd, wsCondition);
      if (nodes.GetSize() > 0) {
        return 1;
      }
      return 0;
    }
  }
  if (dwStyles & XFA_RESOLVENODE_Attributes) {
    if (ResolveForAttributeRs(curNode, rnd, wsName.AsStringC())) {
      return 1;
    }
  }
  if (dwStyles & XFA_RESOLVENODE_Properties) {
    for (int32_t i = 0; i < properties.GetSize(); i++) {
      CXFA_Node* childProperty = properties[i];
      if (childProperty->IsUnnamed()) {
        uint32_t uPropHash = childProperty->GetClassHashCode();
        if (uPropHash == uNameHash) {
          nodes.Add(childProperty);
        }
      } else if (childProperty->GetNameHash() == uNameHash &&
                 childProperty->GetElementType() != XFA_Element::Extras &&
                 childProperty->GetElementType() != XFA_Element::Items) {
        nodes.Add(childProperty);
      }
    }
    if (nodes.GetSize() > nNum) {
      FilterCondition(rnd, wsCondition);
      if (nodes.GetSize() > 0) {
        return 1;
      }
      return 0;
    }
    CXFA_Node* pProp = nullptr;
    if (XFA_Element::Subform == curNode->GetElementType() &&
        XFA_HASHCODE_Occur == uNameHash) {
      CXFA_Node* pInstanceManager =
          curNode->AsNode()->GetInstanceMgrOfSubform();
      if (pInstanceManager) {
        pProp = pInstanceManager->GetProperty(0, XFA_Element::Occur, TRUE);
      }
    } else {
      XFA_Element eType = XFA_GetElementTypeForName(wsName.AsStringC());
      if (eType != XFA_Element::Unknown) {
        pProp = curNode->AsNode()->GetProperty(0, eType,
                                               eType != XFA_Element::PageSet);
      }
    }
    if (pProp) {
      nodes.Add(pProp);
      return nodes.GetSize();
    }
  }
  CXFA_Node* parentNode = m_pNodeHelper->ResolveNodes_GetParent(
      curNode->AsNode(), XFA_LOGIC_NoTransparent);
  uint32_t uCurClassHash = curNode->GetClassHashCode();
  if (!parentNode) {
    if (uCurClassHash == uNameHash) {
      nodes.Add(curNode->AsNode());
      FilterCondition(rnd, wsCondition);
      if (nodes.GetSize() > 0) {
        return 1;
      }
    }
    return 0;
  }
  if (dwStyles & XFA_RESOLVENODE_Siblings) {
    CXFA_Node* child = parentNode->GetNodeItem(XFA_NODEITEM_FirstChild);
    uint32_t dwSubStyles =
        XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Properties;
    if (dwStyles & XFA_RESOLVENODE_TagName) {
      dwSubStyles |= XFA_RESOLVENODE_TagName;
    }
    if (dwStyles & XFA_RESOLVENODE_ALL) {
      dwSubStyles |= XFA_RESOLVENODE_ALL;
    }
    rndFind.m_dwStyles = dwSubStyles;
    while (child) {
      if (child == curNode) {
        if (dwStyles & XFA_RESOLVENODE_TagName) {
          if (uCurClassHash == uNameHash) {
            nodes.Add(curNode);
          }
        } else {
          if (child->GetNameHash() == uNameHash) {
            nodes.Add(curNode);
            if (rnd.m_nLevel == 0 && wsCondition.GetLength() == 0) {
              nodes.RemoveAll();
              nodes.Add(curNode);
              return 1;
            }
          }
        }
        child = child->GetNodeItem(XFA_NODEITEM_NextSibling);
        continue;
      }
      if (dwStyles & XFA_RESOLVENODE_TagName) {
        if (child->GetClassHashCode() == uNameHash) {
          nodes.Add(child);
        }
      } else if (child->GetNameHash() == uNameHash) {
        nodes.Add(child);
      }
      const XFA_PROPERTY* pPropert = XFA_GetPropertyOfElement(
          parentNode->GetElementType(), child->GetElementType(),
          XFA_XDPPACKET_UNKNOWN);
      FX_BOOL bInnerSearch = FALSE;
      if (pPropert) {
        if ((child->GetElementType() == XFA_Element::Variables ||
             child->GetElementType() == XFA_Element::PageSet)) {
          bInnerSearch = TRUE;
        }
      } else {
        if (m_pNodeHelper->NodeIsTransparent(child)) {
          bInnerSearch = TRUE;
        }
      }
      if (bInnerSearch) {
        rndFind.m_CurNode = child;
        CFX_WideString wsOriginCondition = rndFind.m_wsCondition;
        rndFind.m_wsCondition.clear();
        uint32_t dwOriginStyle = rndFind.m_dwStyles;
        rndFind.m_dwStyles = dwOriginStyle | XFA_RESOLVENODE_ALL;
        ResolveNormal(rndFind);
        rndFind.m_dwStyles = dwOriginStyle;
        rndFind.m_wsCondition = wsOriginCondition;
        if (rndFind.m_Nodes.GetSize() > 0) {
          nodes.Append(rndFind.m_Nodes);
          rndFind.m_Nodes.RemoveAll();
        }
      }
      child = child->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
    if (nodes.GetSize() > nNum) {
      if (m_pNodeHelper->NodeIsTransparent(parentNode)) {
        CXFA_NodeArray upArrayNodes;
        m_pNodeHelper->CountSiblings(ToNode(nodes[0]), XFA_LOGIC_Transparent,
                                     &upArrayNodes,
                                     !!(dwStyles & XFA_RESOLVENODE_TagName));
        if (upArrayNodes.GetSize() > nodes.GetSize()) {
          upArrayNodes[0] = ToNode(nodes[0]);
          nodes.RemoveAll();
          nodes.Append((CXFA_ObjArray&)upArrayNodes);
          upArrayNodes.RemoveAll();
        }
      }
      FilterCondition(rnd, wsCondition);
      if (nodes.GetSize() > 0) {
        return 1;
      }
      return 0;
    }
  }
  if (dwStyles & XFA_RESOLVENODE_Parent) {
    uint32_t dwSubStyles = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_Parent |
                           XFA_RESOLVENODE_Properties;
    if (dwStyles & XFA_RESOLVENODE_TagName) {
      dwSubStyles |= XFA_RESOLVENODE_TagName;
    }
    if (dwStyles & XFA_RESOLVENODE_ALL) {
      dwSubStyles |= XFA_RESOLVENODE_ALL;
    }
    rndFind.m_dwStyles = dwSubStyles;
    rndFind.m_CurNode = parentNode;
    CXFA_NodeArray& array = rnd.m_pSC->GetUpObjectArray();
    array.Add(parentNode);
    ResolveNormal(rndFind);
    if (rndFind.m_Nodes.GetSize() > 0) {
      nodes.Append(rndFind.m_Nodes);
      rndFind.m_Nodes.RemoveAll();
    }
    if (nodes.GetSize() > nNum) {
      return 1;
    }
  }
  return 0;
}
int32_t CXFA_ResolveProcessor::ResolveAsterisk(CXFA_ResolveNodesData& rnd) {
  CXFA_Node* curNode = ToNode(rnd.m_CurNode);
  CXFA_ObjArray& nodes = rnd.m_Nodes;
  CXFA_NodeArray array;
  curNode->GetNodeList(array,
                       XFA_NODEFILTER_Children | XFA_NODEFILTER_Properties);
  nodes.Append((CXFA_ObjArray&)array);
  return nodes.GetSize();
}
int32_t CXFA_ResolveProcessor::ResolvePopStack(CFX_Int32Array& stack) {
  int32_t nType = -1;
  int32_t iSize = stack.GetSize() - 1;
  if (iSize > -1) {
    nType = stack[iSize];
    stack.RemoveAt(iSize, 1);
  }
  return nType;
}
int32_t CXFA_ResolveProcessor::GetFilter(const CFX_WideStringC& wsExpression,
                                         int32_t nStart,
                                         CXFA_ResolveNodesData& rnd) {
  ASSERT(nStart > -1);
  int32_t iLength = wsExpression.GetLength();
  if (nStart >= iLength) {
    return 0;
  }
  CFX_WideString& wsName = rnd.m_wsName;
  CFX_WideString& wsCondition = rnd.m_wsCondition;
  FX_WCHAR* pNameBuf = wsName.GetBuffer(iLength - nStart);
  FX_WCHAR* pConditionBuf = wsCondition.GetBuffer(iLength - nStart);
  int32_t nNameCount = 0;
  int32_t nConditionCount = 0;
  CFX_Int32Array stack;
  int32_t nType = -1;
  const FX_WCHAR* pSrc = wsExpression.c_str();
  FX_WCHAR wPrev = 0, wCur;
  FX_BOOL bIsCondition = FALSE;
  while (nStart < iLength) {
    wCur = pSrc[nStart++];
    if (wCur == '.') {
      if (wPrev == '\\') {
        pNameBuf[nNameCount - 1] = wPrev = '.';
        continue;
      }
      if (nNameCount == 0) {
        rnd.m_dwStyles |= XFA_RESOLVENODE_AnyChild;
        continue;
      }
      FX_WCHAR wLookahead = nStart < iLength ? pSrc[nStart] : 0;
      if (wLookahead != '[' && wLookahead != '(') {
        if (nType < 0) {
          break;
        }
      }
    }
    if (wCur == '[' || wCur == '(') {
      bIsCondition = TRUE;
    } else if (wCur == '.' && nStart < iLength &&
               (pSrc[nStart] == '[' || pSrc[nStart] == '(')) {
      bIsCondition = TRUE;
    }
    if (bIsCondition) {
      pConditionBuf[nConditionCount++] = wCur;
    } else {
      pNameBuf[nNameCount++] = wCur;
    }
    FX_BOOL bRecursive = TRUE;
    switch (nType) {
      case 0:
        if (wCur == ']') {
          nType = ResolvePopStack(stack);
          bRecursive = FALSE;
        }
        break;
      case 1:
        if (wCur == ')') {
          nType = ResolvePopStack(stack);
          bRecursive = FALSE;
        }
        break;
      case 2:
        if (wCur == '"') {
          nType = ResolvePopStack(stack);
          bRecursive = FALSE;
        }
        break;
    }
    if (bRecursive) {
      switch (wCur) {
        case '[':
          stack.Add(nType);
          nType = 0;
          break;
        case '(':
          stack.Add(nType);
          nType = 1;
          break;
        case '"':
          stack.Add(nType);
          nType = 2;
          break;
      }
    }
    wPrev = wCur;
  }
  if (stack.GetSize() > 0) {
    return -1;
  }
  wsName.ReleaseBuffer(nNameCount);
  wsName.TrimLeft();
  wsName.TrimRight();
  wsCondition.ReleaseBuffer(nConditionCount);
  wsCondition.TrimLeft();
  wsCondition.TrimRight();
  rnd.m_uHashName =
      static_cast<XFA_HashCode>(FX_HashCode_GetW(wsName.AsStringC(), false));
  return nStart;
}
void CXFA_ResolveProcessor::ConditionArray(int32_t iCurIndex,
                                           CFX_WideString wsCondition,
                                           int32_t iFoundCount,
                                           CXFA_ResolveNodesData& rnd) {
  CXFA_NodeArray& findNodes = (CXFA_NodeArray&)rnd.m_Nodes;
  int32_t iLen = wsCondition.GetLength();
  FX_BOOL bRelative = FALSE;
  FX_BOOL bAll = FALSE;
  int32_t i = 1;
  for (; i < iLen; ++i) {
    FX_WCHAR ch = wsCondition[i];
    if (ch == ' ') {
      continue;
    }
    if (ch == '+' || ch == '-') {
      bRelative = TRUE;
      break;
    } else if (ch == '*') {
      bAll = TRUE;
      break;
    } else {
      break;
    }
  }
  if (bAll) {
    if (rnd.m_dwStyles & XFA_RESOLVENODE_CreateNode) {
      if (rnd.m_dwStyles & XFA_RESOLVENODE_Bind) {
        m_pNodeHelper->m_pCreateParent = ToNode(rnd.m_CurNode);
        m_pNodeHelper->m_iCreateCount = 1;
        findNodes.RemoveAll();
        m_pNodeHelper->m_iCurAllStart = -1;
        m_pNodeHelper->m_pAllStartParent = nullptr;
      } else {
        if (m_pNodeHelper->m_iCurAllStart == -1) {
          m_pNodeHelper->m_iCurAllStart = m_iCurStart;
          m_pNodeHelper->m_pAllStartParent = ToNode(rnd.m_CurNode);
        }
      }
    } else if (rnd.m_dwStyles & XFA_RESOLVENODE_BindNew) {
      if (m_pNodeHelper->m_iCurAllStart == -1) {
        m_pNodeHelper->m_iCurAllStart = m_iCurStart;
      }
    }
    return;
  }
  if (iFoundCount == 1 && !iLen) {
    return;
  }
  CFX_WideString wsIndex;
  wsIndex = wsCondition.Mid(i, iLen - 1 - i);
  int32_t iIndex = wsIndex.GetInteger();
  if (bRelative) {
    iIndex += iCurIndex;
  }
  if (iFoundCount <= iIndex || iIndex < 0) {
    if (rnd.m_dwStyles & XFA_RESOLVENODE_CreateNode) {
      m_pNodeHelper->m_pCreateParent = ToNode(rnd.m_CurNode);
      m_pNodeHelper->m_iCreateCount = iIndex - iFoundCount + 1;
    }
    findNodes.RemoveAll();
  } else {
    CXFA_Node* ret = findNodes[iIndex];
    findNodes.RemoveAll();
    findNodes.Add(ret);
  }
}
void CXFA_ResolveProcessor::DoPredicateFilter(int32_t iCurIndex,
                                              CFX_WideString wsCondition,
                                              int32_t iFoundCount,
                                              CXFA_ResolveNodesData& rnd) {
  CXFA_NodeArray& findNodes = (CXFA_NodeArray&)rnd.m_Nodes;
  ASSERT(iFoundCount == findNodes.GetSize());
  CFX_WideString wsExpression;
  XFA_SCRIPTLANGTYPE eLangType = XFA_SCRIPTLANGTYPE_Unkown;
  if (wsCondition.Left(2) == FX_WSTRC(L".[") &&
      wsCondition.Right(1) == FX_WSTRC(L"]")) {
    eLangType = XFA_SCRIPTLANGTYPE_Formcalc;
  } else if (wsCondition.Left(2) == FX_WSTRC(L".(") &&
             wsCondition.Right(1) == FX_WSTRC(L")")) {
    eLangType = XFA_SCRIPTLANGTYPE_Javascript;
  } else {
    return;
  }

  CXFA_ScriptContext* pContext = rnd.m_pSC;
  wsExpression = wsCondition.Mid(2, wsCondition.GetLength() - 3);
  for (int32_t i = iFoundCount - 1; i >= 0; i--) {
    CXFA_Object* node = findNodes[i];
    FX_BOOL bRet = FALSE;
    std::unique_ptr<CFXJSE_Value> pRetValue(
        new CFXJSE_Value(rnd.m_pSC->GetRuntime()));
    bRet = pContext->RunScript(eLangType, wsExpression.AsStringC(),
                               pRetValue.get(), node);
    if (!bRet || !pRetValue->ToBoolean())
      findNodes.RemoveAt(i);
  }
}

void CXFA_ResolveProcessor::FilterCondition(CXFA_ResolveNodesData& rnd,
                                            CFX_WideString wsCondition) {
  CXFA_NodeArray& findNodes = (CXFA_NodeArray&)rnd.m_Nodes;
  int32_t iCurrIndex = 0;
  const CXFA_NodeArray& array = rnd.m_pSC->GetUpObjectArray();
  int32_t iSize = array.GetSize();
  if (iSize) {
    CXFA_Node* curNode = array[iSize - 1];
    FX_BOOL bIsProperty = m_pNodeHelper->NodeIsProperty(curNode);
    if (curNode->IsUnnamed() ||
        (bIsProperty && curNode->GetElementType() != XFA_Element::PageSet)) {
      iCurrIndex = m_pNodeHelper->GetIndex(curNode, XFA_LOGIC_Transparent,
                                           bIsProperty, TRUE);
    } else {
      iCurrIndex = m_pNodeHelper->GetIndex(curNode, XFA_LOGIC_Transparent,
                                           bIsProperty, FALSE);
    }
  }
  int32_t iFoundCount = findNodes.GetSize();
  wsCondition.TrimLeft();
  wsCondition.TrimRight();
  int32_t iLen = wsCondition.GetLength();
  if (!iLen) {
    if (rnd.m_dwStyles & XFA_RESOLVENODE_ALL) {
      return;
    }
    if (iFoundCount == 1) {
      return;
    }
    if (iFoundCount <= iCurrIndex) {
      if (rnd.m_dwStyles & XFA_RESOLVENODE_CreateNode) {
        m_pNodeHelper->m_pCreateParent = ToNode(rnd.m_CurNode);
        m_pNodeHelper->m_iCreateCount = iCurrIndex - iFoundCount + 1;
      }
      findNodes.RemoveAll();
      return;
    } else {
      CXFA_Node* ret = findNodes[iCurrIndex];
      findNodes.RemoveAll();
      findNodes.Add(ret);
      return;
    }
  }
  FX_WCHAR wTypeChar = wsCondition[0];
  switch (wTypeChar) {
    case '[':
      ConditionArray(iCurrIndex, wsCondition, iFoundCount, rnd);
      return;
    case '(':
      return;
    case '"':
      return;
    case '.':
      if (iLen > 1 && (wsCondition[1] == '[' || wsCondition[1] == '(')) {
        DoPredicateFilter(iCurrIndex, wsCondition, iFoundCount, rnd);
      }
    default:
      return;
  }
}
void CXFA_ResolveProcessor::SetStylesForChild(uint32_t dwParentStyles,
                                              CXFA_ResolveNodesData& rnd) {
  uint32_t dwSubStyles = XFA_RESOLVENODE_Children;
  if (dwParentStyles & XFA_RESOLVENODE_TagName) {
    dwSubStyles |= XFA_RESOLVENODE_TagName;
  }
  dwSubStyles &= ~XFA_RESOLVENODE_Parent;
  dwSubStyles &= ~XFA_RESOLVENODE_Siblings;
  dwSubStyles &= ~XFA_RESOLVENODE_Properties;
  dwSubStyles |= XFA_RESOLVENODE_ALL;
  rnd.m_dwStyles = dwSubStyles;
}
int32_t CXFA_ResolveProcessor::SetResultCreateNode(
    XFA_RESOLVENODE_RS& resolveNodeRS,
    CFX_WideString& wsLastCondition) {
  if (m_pNodeHelper->m_pCreateParent) {
    resolveNodeRS.nodes.Add(m_pNodeHelper->m_pCreateParent);
  } else {
    m_pNodeHelper->CreateNode_ForCondition(wsLastCondition);
  }
  resolveNodeRS.dwFlags = m_pNodeHelper->m_iCreateFlag;
  if (resolveNodeRS.dwFlags == XFA_RESOLVENODE_RSTYPE_CreateNodeOne) {
    if (m_pNodeHelper->m_iCurAllStart != -1) {
      resolveNodeRS.dwFlags = XFA_RESOLVENODE_RSTYPE_CreateNodeMidAll;
    }
  }
  return resolveNodeRS.nodes.GetSize();
}
void CXFA_ResolveProcessor::SetIndexDataBind(CFX_WideString& wsNextCondition,
                                             int32_t& iIndex,
                                             int32_t iCount) {
  if (m_pNodeHelper->CreateNode_ForCondition(wsNextCondition)) {
    if (m_pNodeHelper->m_eLastCreateType == XFA_Element::DataGroup) {
      iIndex = 0;
    } else {
      iIndex = iCount - 1;
    }
  } else {
    iIndex = iCount - 1;
  }
}

CXFA_ResolveNodesData::CXFA_ResolveNodesData(CXFA_ScriptContext* pSC)
    : m_pSC(pSC),
      m_CurNode(nullptr),
      m_wsName(),
      m_uHashName(XFA_HASHCODE_None),
      m_wsCondition(),
      m_nLevel(0),
      m_Nodes(),
      m_dwStyles(XFA_RESOLVENODE_Children),
      m_pScriptAttribute(nullptr),
      m_dwFlag(XFA_RESOVENODE_RSTYPE_Nodes) {}

CXFA_ResolveNodesData::~CXFA_ResolveNodesData() {
  m_Nodes.RemoveAll();
}
