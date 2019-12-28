// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_DOCUMENT_H_
#define XFA_FXFA_PARSER_XFA_DOCUMENT_H_

#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"

class CFDE_XMLDoc;
class CXFA_Document;
class CXFA_LayoutItem;
class CXFA_LayoutProcessor;
class CXFA_Node;
class CXFA_LayoutProcessor;
class CXFA_DocumentParser;
class CXFA_ContainerLayoutItem;
class CXFA_FFNotify;
class CXFA_ScriptContext;

enum XFA_VERSION {
  XFA_VERSION_UNKNOWN = 0,
  XFA_VERSION_200 = 200,
  XFA_VERSION_202 = 202,
  XFA_VERSION_204 = 204,
  XFA_VERSION_205 = 205,
  XFA_VERSION_206 = 206,
  XFA_VERSION_207 = 207,
  XFA_VERSION_208 = 208,
  XFA_VERSION_300 = 300,
  XFA_VERSION_301 = 301,
  XFA_VERSION_303 = 303,
  XFA_VERSION_306 = 306,
  XFA_VERSION_DEFAULT = XFA_VERSION_303,
  XFA_VERSION_MIN = 200,
  XFA_VERSION_MAX = 400,
};

enum XFA_LAYOUTRESULT {
  XFA_LAYOUTRESULT_Continue,
  XFA_LAYOUTRESULT_Done,
  XFA_LAYOUTRESULT_NextContent,
};
#define XFA_LAYOUTNOTIFY_StrictHeight 0x0001
#define XFA_LAYOUTNOTIFY_NoParentBreak 0x0002

#define XFA_DOCFLAG_StrictScoping 0x0001
#define XFA_DOCFLAG_HasInteractive 0x0002
#define XFA_DOCFLAG_Interactive 0x0004
#define XFA_DOCFLAG_Scripting 0x0008
class CScript_DataWindow;
class CScript_EventPseudoModel;
class CScript_HostPseudoModel;
class CScript_LogPseudoModel;
class CScript_LayoutPseudoModel;
class CScript_SignaturePseudoModel;

class CXFA_Document {
 public:
  CXFA_Document(CXFA_DocumentParser* pParser);
  ~CXFA_Document();

  CXFA_Node* GetRoot() const { return m_pRootNode; }

  CFDE_XMLDoc* GetXMLDoc() const;
  CXFA_FFNotify* GetNotify() const;
  void SetRoot(CXFA_Node* pNewRoot);
  CXFA_Object* GetXFAObject(XFA_HashCode wsNodeNameHash);
  void AddPurgeNode(CXFA_Node* pNode);
  FX_BOOL RemovePurgeNode(CXFA_Node* pNode);
  void PurgeNodes();
  bool HasFlag(uint32_t dwFlag) { return (m_dwDocFlags & dwFlag) == dwFlag; }
  void SetFlag(uint32_t dwFlag, FX_BOOL bOn = TRUE);
  FX_BOOL IsInteractive();
  XFA_VERSION GetCurVersionMode() { return m_eCurVersionMode; }
  XFA_VERSION RecognizeXFAVersionNumber(CFX_WideString& wsTemplateNS);
  CXFA_LocaleMgr* GetLocalMgr();
  CXFA_Node* CreateNode(uint32_t dwPacket, XFA_Element eElement);
  CXFA_Node* CreateNode(const XFA_PACKETINFO* pPacket, XFA_Element eElement);
  void DoProtoMerge();
  CXFA_Node* GetNodeByID(CXFA_Node* pRoot, const CFX_WideStringC& wsID);
  void DoDataMerge();
  void DoDataRemerge(FX_BOOL bDoDataMerge);
  CXFA_Node* DataMerge_CopyContainer(CXFA_Node* pTemplateNode,
                                     CXFA_Node* pFormNode,
                                     CXFA_Node* pDataScope,
                                     FX_BOOL bOneInstance = FALSE,
                                     FX_BOOL bDataMerge = TRUE,
                                     FX_BOOL bUpLevel = TRUE);
  void DataMerge_UpdateBindingRelations(CXFA_Node* pFormUpdateRoot);
  CXFA_Node* GetNotBindNode(CXFA_ObjArray& arrayNodes);
  CXFA_LayoutProcessor* GetLayoutProcessor();
  CXFA_LayoutProcessor* GetDocLayout();
  CXFA_ScriptContext* InitScriptContext(v8::Isolate* pIsolate);
  CXFA_ScriptContext* GetScriptContext();
  void ClearLayoutData();

  CFX_MapPtrTemplate<uint32_t, CXFA_Node*> m_rgGlobalBinding;
  CXFA_NodeArray m_pPendingPageSet;

 protected:
  CXFA_DocumentParser* m_pParser;
  CXFA_ScriptContext* m_pScriptContext;
  CXFA_LayoutProcessor* m_pLayoutProcessor;
  CXFA_Node* m_pRootNode;
  CXFA_LocaleMgr* m_pLocalMgr;
  CScript_DataWindow* m_pScriptDataWindow;
  CScript_EventPseudoModel* m_pScriptEvent;
  CScript_HostPseudoModel* m_pScriptHost;
  CScript_LogPseudoModel* m_pScriptLog;
  CScript_LayoutPseudoModel* m_pScriptLayout;
  CScript_SignaturePseudoModel* m_pScriptSignature;
  CXFA_NodeSet m_PurgeNodes;
  XFA_VERSION m_eCurVersionMode;
  uint32_t m_dwDocFlags;
  friend class CXFA_SimpleParser;
};

#endif  // XFA_FXFA_PARSER_XFA_DOCUMENT_H_
