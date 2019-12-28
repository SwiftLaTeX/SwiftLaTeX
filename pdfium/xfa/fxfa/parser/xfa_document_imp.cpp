// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_document_parser.h"
#include "xfa/fxfa/parser/xfa_basic_imp.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_datawindow.h"
#include "xfa/fxfa/parser/xfa_script_eventpseudomodel.h"
#include "xfa/fxfa/parser/xfa_script_hostpseudomodel.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_script_layoutpseudomodel.h"
#include "xfa/fxfa/parser/xfa_script_logpseudomodel.h"
#include "xfa/fxfa/parser/xfa_script_signaturepseudomodel.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_Document::CXFA_Document(CXFA_DocumentParser* pParser)
    : m_pParser(pParser),
      m_pScriptContext(nullptr),
      m_pLayoutProcessor(nullptr),
      m_pRootNode(nullptr),
      m_pLocalMgr(nullptr),
      m_pScriptDataWindow(nullptr),
      m_pScriptEvent(nullptr),
      m_pScriptHost(nullptr),
      m_pScriptLog(nullptr),
      m_pScriptLayout(nullptr),
      m_pScriptSignature(nullptr),
      m_eCurVersionMode(XFA_VERSION_DEFAULT),
      m_dwDocFlags(0) {
  ASSERT(m_pParser);
}
CXFA_Document::~CXFA_Document() {
  delete m_pRootNode;
  PurgeNodes();
}

void CXFA_Document::ClearLayoutData() {
  delete m_pLayoutProcessor;
  m_pLayoutProcessor = nullptr;
  delete m_pScriptContext;
  m_pScriptContext = nullptr;
  delete m_pLocalMgr;
  m_pLocalMgr = nullptr;
  delete m_pScriptDataWindow;
  m_pScriptDataWindow = nullptr;
  delete m_pScriptEvent;
  m_pScriptEvent = nullptr;
  delete m_pScriptHost;
  m_pScriptHost = nullptr;
  delete m_pScriptLog;
  m_pScriptLog = nullptr;
  delete m_pScriptLayout;
  m_pScriptLayout = nullptr;
  delete m_pScriptSignature;
  m_pScriptSignature = nullptr;
}

void CXFA_Document::SetRoot(CXFA_Node* pNewRoot) {
  if (m_pRootNode) {
    AddPurgeNode(m_pRootNode);
  }
  m_pRootNode = pNewRoot;
  RemovePurgeNode(pNewRoot);
}

CFDE_XMLDoc* CXFA_Document::GetXMLDoc() const {
  return m_pParser->GetXMLDoc();
}

CXFA_FFNotify* CXFA_Document::GetNotify() const {
  return m_pParser->GetNotify();
}

CXFA_Object* CXFA_Document::GetXFAObject(XFA_HashCode dwNodeNameHash) {
  switch (dwNodeNameHash) {
    case XFA_HASHCODE_Data: {
      CXFA_Node* pDatasetsNode = ToNode(GetXFAObject(XFA_HASHCODE_Datasets));
      if (!pDatasetsNode) {
        return nullptr;
      }
      for (CXFA_Node* pDatasetsChild =
               pDatasetsNode->GetFirstChildByClass(XFA_Element::DataGroup);
           pDatasetsChild;
           pDatasetsChild = pDatasetsChild->GetNextSameClassSibling(
               XFA_Element::DataGroup)) {
        if (pDatasetsChild->GetNameHash() != XFA_HASHCODE_Data) {
          continue;
        }
        CFX_WideString wsNamespaceURI;
        if (!pDatasetsChild->TryNamespace(wsNamespaceURI)) {
          continue;
        }
        CFX_WideString wsDatasetsURI;
        if (!pDatasetsNode->TryNamespace(wsDatasetsURI)) {
          continue;
        }
        if (wsNamespaceURI == wsDatasetsURI) {
          return pDatasetsChild;
        }
      }
      return nullptr;
    }
    case XFA_HASHCODE_Record: {
      CXFA_Node* pData = ToNode(GetXFAObject(XFA_HASHCODE_Data));
      return pData ? pData->GetFirstChildByClass(XFA_Element::DataGroup)
                   : nullptr;
    }
    case XFA_HASHCODE_DataWindow: {
      if (!m_pScriptDataWindow)
        m_pScriptDataWindow = new CScript_DataWindow(this);
      return m_pScriptDataWindow;
    }
    case XFA_HASHCODE_Event: {
      if (!m_pScriptEvent)
        m_pScriptEvent = new CScript_EventPseudoModel(this);
      return m_pScriptEvent;
    }
    case XFA_HASHCODE_Host: {
      if (!m_pScriptHost)
        m_pScriptHost = new CScript_HostPseudoModel(this);
      return m_pScriptHost;
    }
    case XFA_HASHCODE_Log: {
      if (!m_pScriptLog)
        m_pScriptLog = new CScript_LogPseudoModel(this);
      return m_pScriptLog;
    }
    case XFA_HASHCODE_Signature: {
      if (!m_pScriptSignature)
        m_pScriptSignature = new CScript_SignaturePseudoModel(this);
      return m_pScriptSignature;
    }
    case XFA_HASHCODE_Layout: {
      if (!m_pScriptLayout)
        m_pScriptLayout = new CScript_LayoutPseudoModel(this);
      return m_pScriptLayout;
    }
    default:
      return m_pRootNode->GetFirstChildByName(dwNodeNameHash);
  }
}
CXFA_Node* CXFA_Document::CreateNode(uint32_t dwPacket, XFA_Element eElement) {
  return CreateNode(XFA_GetPacketByID(dwPacket), eElement);
}

CXFA_Node* CXFA_Document::CreateNode(const XFA_PACKETINFO* pPacket,
                                     XFA_Element eElement) {
  if (!pPacket)
    return nullptr;

  const XFA_ELEMENTINFO* pElement = XFA_GetElementByID(eElement);
  if (pElement && (pElement->dwPackets & pPacket->eName)) {
    CXFA_Node* pNode = new CXFA_Node(this, pPacket->eName,
                                     pElement->eObjectType, pElement->eName);
    AddPurgeNode(pNode);
    return pNode;
  }

  return nullptr;
}

void CXFA_Document::AddPurgeNode(CXFA_Node* pNode) {
  m_PurgeNodes.insert(pNode);
}

FX_BOOL CXFA_Document::RemovePurgeNode(CXFA_Node* pNode) {
  return !!m_PurgeNodes.erase(pNode);
}

void CXFA_Document::PurgeNodes() {
  for (CXFA_Node* pNode : m_PurgeNodes)
    delete pNode;

  m_PurgeNodes.clear();
}

void CXFA_Document::SetFlag(uint32_t dwFlag, FX_BOOL bOn) {
  if (bOn) {
    m_dwDocFlags |= dwFlag;
  } else {
    m_dwDocFlags &= ~dwFlag;
  }
}
FX_BOOL CXFA_Document::IsInteractive() {
  if (m_dwDocFlags & XFA_DOCFLAG_HasInteractive) {
    return !!(m_dwDocFlags & XFA_DOCFLAG_Interactive);
  }
  CXFA_Node* pConfig = ToNode(GetXFAObject(XFA_HASHCODE_Config));
  if (!pConfig) {
    return FALSE;
  }
  CFX_WideString wsInteractive;
  CXFA_Node* pPresent = pConfig->GetFirstChildByClass(XFA_Element::Present);
  if (!pPresent) {
    return FALSE;
  }
  CXFA_Node* pPDF = pPresent->GetFirstChildByClass(XFA_Element::Pdf);
  if (!pPDF) {
    return FALSE;
  }
  CXFA_Node* pInteractive = pPDF->GetChild(0, XFA_Element::Interactive);
  if (pInteractive) {
    m_dwDocFlags |= XFA_DOCFLAG_HasInteractive;
    if (pInteractive->TryContent(wsInteractive) &&
        wsInteractive == FX_WSTRC(L"1")) {
      m_dwDocFlags |= XFA_DOCFLAG_Interactive;
      return TRUE;
    }
  }
  return FALSE;
}
CXFA_LocaleMgr* CXFA_Document::GetLocalMgr() {
  if (!m_pLocalMgr) {
    CFX_WideString wsLanguage;
    GetNotify()->GetAppProvider()->GetLanguage(wsLanguage);
    m_pLocalMgr = new CXFA_LocaleMgr(
        ToNode(GetXFAObject(XFA_HASHCODE_LocaleSet)), wsLanguage);
  }
  return m_pLocalMgr;
}
CXFA_ScriptContext* CXFA_Document::InitScriptContext(v8::Isolate* pIsolate) {
  if (!m_pScriptContext)
    m_pScriptContext = new CXFA_ScriptContext(this);
  m_pScriptContext->Initialize(pIsolate);
  return m_pScriptContext;
}
CXFA_ScriptContext* CXFA_Document::GetScriptContext() {
  if (!m_pScriptContext)
    m_pScriptContext = new CXFA_ScriptContext(this);
  return m_pScriptContext;
}
XFA_VERSION CXFA_Document::RecognizeXFAVersionNumber(
    CFX_WideString& wsTemplateNS) {
  CFX_WideStringC wsTemplateURIPrefix =
      XFA_GetPacketByIndex(XFA_PACKET_Template)->pURI;
  FX_STRSIZE nPrefixLength = wsTemplateURIPrefix.GetLength();
  if (CFX_WideStringC(wsTemplateNS.c_str(), wsTemplateNS.GetLength()) !=
      wsTemplateURIPrefix) {
    return XFA_VERSION_UNKNOWN;
  }
  FX_STRSIZE nDotPos = wsTemplateNS.Find('.', nPrefixLength);
  if (nDotPos == (FX_STRSIZE)-1) {
    return XFA_VERSION_UNKNOWN;
  }
  int8_t iMajor = FXSYS_wtoi(
      wsTemplateNS.Mid(nPrefixLength, nDotPos - nPrefixLength).c_str());
  int8_t iMinor = FXSYS_wtoi(
      wsTemplateNS.Mid(nDotPos + 1, wsTemplateNS.GetLength() - nDotPos - 2)
          .c_str());
  XFA_VERSION eVersion = (XFA_VERSION)((int32_t)iMajor * 100 + iMinor);
  if (eVersion < XFA_VERSION_MIN || eVersion > XFA_VERSION_MAX) {
    return XFA_VERSION_UNKNOWN;
  }
  m_eCurVersionMode = eVersion;
  return eVersion;
}
CXFA_Node* CXFA_Document::GetNodeByID(CXFA_Node* pRoot,
                                      const CFX_WideStringC& wsID) {
  if (!pRoot || wsID.IsEmpty()) {
    return nullptr;
  }
  CXFA_NodeIterator sIterator(pRoot);
  for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
       pNode = sIterator.MoveToNext()) {
    CFX_WideStringC wsIDVal;
    if (pNode->TryCData(XFA_ATTRIBUTE_Id, wsIDVal) && !wsIDVal.IsEmpty()) {
      if (wsIDVal == wsID) {
        return pNode;
      }
    }
  }
  return nullptr;
}
static void XFA_ProtoMerge_MergeNodeRecurse(CXFA_Document* pDocument,
                                            CXFA_Node* pDestNodeParent,
                                            CXFA_Node* pProtoNode) {
  CXFA_Node* pExistingNode = nullptr;
  for (CXFA_Node* pFormChild =
           pDestNodeParent->GetNodeItem(XFA_NODEITEM_FirstChild);
       pFormChild;
       pFormChild = pFormChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pFormChild->GetElementType() == pProtoNode->GetElementType() &&
        pFormChild->GetNameHash() == pProtoNode->GetNameHash() &&
        pFormChild->IsUnusedNode()) {
      pFormChild->ClearFlag(XFA_NodeFlag_UnusedNode);
      pExistingNode = pFormChild;
      break;
    }
  }
  if (pExistingNode) {
    pExistingNode->SetTemplateNode(pProtoNode);
    for (CXFA_Node* pTemplateChild =
             pProtoNode->GetNodeItem(XFA_NODEITEM_FirstChild);
         pTemplateChild; pTemplateChild = pTemplateChild->GetNodeItem(
                             XFA_NODEITEM_NextSibling)) {
      XFA_ProtoMerge_MergeNodeRecurse(pDocument, pExistingNode, pTemplateChild);
    }
    return;
  }
  CXFA_Node* pNewNode = pProtoNode->Clone(TRUE);
  pNewNode->SetTemplateNode(pProtoNode);
  pDestNodeParent->InsertChild(pNewNode, nullptr);
}
static void XFA_ProtoMerge_MergeNode(CXFA_Document* pDocument,
                                     CXFA_Node* pDestNode,
                                     CXFA_Node* pProtoNode) {
  {
    CXFA_NodeIterator sIterator(pDestNode);
    for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
         pNode = sIterator.MoveToNext()) {
      pNode->SetFlag(XFA_NodeFlag_UnusedNode, true);
    }
  }
  pDestNode->SetTemplateNode(pProtoNode);
  for (CXFA_Node* pTemplateChild =
           pProtoNode->GetNodeItem(XFA_NODEITEM_FirstChild);
       pTemplateChild;
       pTemplateChild = pTemplateChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    XFA_ProtoMerge_MergeNodeRecurse(pDocument, pDestNode, pTemplateChild);
  }
  {
    CXFA_NodeIterator sIterator(pDestNode);
    for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
         pNode = sIterator.MoveToNext()) {
      pNode->ClearFlag(XFA_NodeFlag_UnusedNode);
    }
  }
}
void CXFA_Document::DoProtoMerge() {
  CXFA_Node* pTemplateRoot = ToNode(GetXFAObject(XFA_HASHCODE_Template));
  if (!pTemplateRoot) {
    return;
  }
  CFX_MapPtrTemplate<uint32_t, CXFA_Node*> mIDMap;
  CXFA_NodeSet sUseNodes;
  CXFA_NodeIterator sIterator(pTemplateRoot);
  for (CXFA_Node* pNode = sIterator.GetCurrent(); pNode;
       pNode = sIterator.MoveToNext()) {
    CFX_WideStringC wsIDVal;
    if (pNode->TryCData(XFA_ATTRIBUTE_Id, wsIDVal) && !wsIDVal.IsEmpty()) {
      mIDMap[FX_HashCode_GetW(wsIDVal, false)] = pNode;
    }
    CFX_WideStringC wsUseVal;
    if (pNode->TryCData(XFA_ATTRIBUTE_Use, wsUseVal) && !wsUseVal.IsEmpty()) {
      sUseNodes.insert(pNode);
    } else if (pNode->TryCData(XFA_ATTRIBUTE_Usehref, wsUseVal) &&
               !wsUseVal.IsEmpty()) {
      sUseNodes.insert(pNode);
    }
  }
  for (CXFA_Node* pUseHrefNode : sUseNodes) {
    CFX_WideString wsUseVal;
    CFX_WideStringC wsURI, wsID, wsSOM;
    if (pUseHrefNode->TryCData(XFA_ATTRIBUTE_Usehref, wsUseVal) &&
        !wsUseVal.IsEmpty()) {
      FX_STRSIZE uSharpPos = wsUseVal.Find('#');
      if (uSharpPos < 0) {
        wsURI = wsUseVal.AsStringC();
      } else {
        wsURI = CFX_WideStringC(wsUseVal.c_str(), uSharpPos);
        FX_STRSIZE uLen = wsUseVal.GetLength();
        if (uLen >= uSharpPos + 5 &&
            CFX_WideStringC(wsUseVal.c_str() + uSharpPos, 5) ==
                FX_WSTRC(L"#som(") &&
            wsUseVal[uLen - 1] == ')') {
          wsSOM = CFX_WideStringC(wsUseVal.c_str() + uSharpPos + 5,
                                  uLen - 1 - uSharpPos - 5);
        } else {
          wsID = CFX_WideStringC(wsUseVal.c_str() + uSharpPos + 1,
                                 uLen - uSharpPos - 1);
        }
      }
    } else if (pUseHrefNode->TryCData(XFA_ATTRIBUTE_Use, wsUseVal) &&
               !wsUseVal.IsEmpty()) {
      if (wsUseVal[0] == '#') {
        wsID = CFX_WideStringC(wsUseVal.c_str() + 1, wsUseVal.GetLength() - 1);
      } else {
        wsSOM = CFX_WideStringC(wsUseVal.c_str(), wsUseVal.GetLength());
      }
    }
    if (!wsURI.IsEmpty() && wsURI != FX_WSTRC(L".")) {
      continue;
    }
    CXFA_Node* pProtoNode = nullptr;
    if (!wsSOM.IsEmpty()) {
      uint32_t dwFlag = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                        XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                        XFA_RESOLVENODE_Siblings;
      XFA_RESOLVENODE_RS resoveNodeRS;
      int32_t iRet = m_pScriptContext->ResolveObjects(pUseHrefNode, wsSOM,
                                                      resoveNodeRS, dwFlag);
      if (iRet > 0 && resoveNodeRS.nodes[0]->IsNode()) {
        pProtoNode = resoveNodeRS.nodes[0]->AsNode();
      }
    } else if (!wsID.IsEmpty()) {
      if (!mIDMap.Lookup(FX_HashCode_GetW(wsID, false), pProtoNode)) {
        continue;
      }
    }
    if (!pProtoNode) {
      continue;
    }
    XFA_ProtoMerge_MergeNode(this, pUseHrefNode, pProtoNode);
  }
}
