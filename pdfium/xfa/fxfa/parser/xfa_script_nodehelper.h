// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_NODEHELPER_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_NODEHELPER_H_

#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"

class CXFA_ScriptContext;

enum XFA_LOGIC_TYPE {
  XFA_LOGIC_NoTransparent,
  XFA_LOGIC_Transparent,
};

class CXFA_NodeHelper {
 public:
  CXFA_NodeHelper();
  ~CXFA_NodeHelper();

  CXFA_Node* ResolveNodes_GetOneChild(CXFA_Node* parent,
                                      const FX_WCHAR* pwsName,
                                      FX_BOOL bIsClassName = FALSE);
  CXFA_Node* ResolveNodes_GetParent(
      CXFA_Node* pNode,
      XFA_LOGIC_TYPE eLogicType = XFA_LOGIC_NoTransparent);

  int32_t NodeAcc_TraverseSiblings(CXFA_Node* parent,
                                   uint32_t dNameHash,
                                   CXFA_NodeArray* pSiblings,
                                   XFA_LOGIC_TYPE eLogicType,
                                   FX_BOOL bIsClassName = FALSE,
                                   FX_BOOL bIsFindProperty = TRUE);
  int32_t NodeAcc_TraverseAnySiblings(CXFA_Node* parent,
                                      uint32_t dNameHash,
                                      CXFA_NodeArray* pSiblings,
                                      FX_BOOL bIsClassName = FALSE);
  int32_t CountSiblings(CXFA_Node* pNode,
                        XFA_LOGIC_TYPE eLogicType,
                        CXFA_NodeArray* pSiblings,
                        FX_BOOL bIsClassName = FALSE);
  int32_t GetIndex(CXFA_Node* pNode,
                   XFA_LOGIC_TYPE eLogicType = XFA_LOGIC_NoTransparent,
                   FX_BOOL bIsProperty = FALSE,
                   FX_BOOL bIsClassIndex = FALSE);
  void GetNameExpression(CXFA_Node* refNode,
                         CFX_WideString& wsName,
                         FX_BOOL bIsAllPath,
                         XFA_LOGIC_TYPE eLogicType = XFA_LOGIC_NoTransparent);
  FX_BOOL NodeIsTransparent(CXFA_Node* refNode);
  FX_BOOL ResolveNodes_CreateNode(CFX_WideString wsName,
                                  CFX_WideString wsCondition,
                                  FX_BOOL bLastNode,
                                  CXFA_ScriptContext* pScriptContext);
  FX_BOOL CreateNode_ForCondition(CFX_WideString& wsCondition);
  void SetCreateNodeType(CXFA_Node* refNode);
  FX_BOOL NodeIsProperty(CXFA_Node* refNode);

 public:
  XFA_Element m_eLastCreateType;
  CXFA_Node* m_pCreateParent;
  int32_t m_iCreateCount;
  XFA_RESOVENODE_RSTYPE m_iCreateFlag;
  int32_t m_iCurAllStart;
  CXFA_Node* m_pAllStartParent;
};

#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_NODEHELPER_H_
