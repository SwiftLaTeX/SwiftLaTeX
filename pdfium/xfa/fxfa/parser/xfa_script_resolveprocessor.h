// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_RESOLVEPROCESSOR_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_RESOLVEPROCESSOR_H_

#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"

class CXFA_NodeHelper;
class CXFA_ScriptContext;

class CXFA_ResolveNodesData {
 public:
  CXFA_ResolveNodesData(CXFA_ScriptContext* pSC = nullptr);
  ~CXFA_ResolveNodesData();

  CXFA_ScriptContext* m_pSC;
  CXFA_Object* m_CurNode;
  CFX_WideString m_wsName;
  XFA_HashCode m_uHashName;
  CFX_WideString m_wsCondition;
  int32_t m_nLevel;
  CXFA_ObjArray m_Nodes;
  uint32_t m_dwStyles;
  const XFA_SCRIPTATTRIBUTEINFO* m_pScriptAttribute;
  XFA_RESOVENODE_RSTYPE m_dwFlag;
};

class CXFA_ResolveProcessor {
 public:
  CXFA_ResolveProcessor();
  ~CXFA_ResolveProcessor();

  int32_t Resolve(CXFA_ResolveNodesData& rnd);
  int32_t GetFilter(const CFX_WideStringC& wsExpression,
                    int32_t nStart,
                    CXFA_ResolveNodesData& rnd);
  int32_t SetResultCreateNode(XFA_RESOLVENODE_RS& resolveNodeRS,
                              CFX_WideString& wsLastCondition);
  void SetIndexDataBind(CFX_WideString& wsNextCondition,
                        int32_t& iIndex,
                        int32_t iCount);
  void SetCurStart(int32_t start) { m_iCurStart = start; }

  CXFA_NodeHelper* GetNodeHelper() { return m_pNodeHelper; }

 private:
  int32_t ResolveForAttributeRs(CXFA_Object* curNode,
                                CXFA_ResolveNodesData& rnd,
                                const CFX_WideStringC& strAttr);
  int32_t ResolveAnyChild(CXFA_ResolveNodesData& rnd);
  int32_t ResolveDollar(CXFA_ResolveNodesData& rnd);
  int32_t ResolveExcalmatory(CXFA_ResolveNodesData& rnd);
  int32_t ResolveNumberSign(CXFA_ResolveNodesData& rnd);
  int32_t ResolveAsterisk(CXFA_ResolveNodesData& rnd);
  int32_t ResolveNormal(CXFA_ResolveNodesData& rnd);
  int32_t ResolvePopStack(CFX_Int32Array& stack);
  void SetStylesForChild(uint32_t dwParentStyles, CXFA_ResolveNodesData& rnd);

  void ConditionArray(int32_t iCurIndex,
                      CFX_WideString wsCondition,
                      int32_t iFoundCount,
                      CXFA_ResolveNodesData& rnd);
  void DoPredicateFilter(int32_t iCurIndex,
                         CFX_WideString wsCondition,
                         int32_t iFoundCount,
                         CXFA_ResolveNodesData& rnd);
  void FilterCondition(CXFA_ResolveNodesData& rnd, CFX_WideString wsCondition);

  int32_t m_iCurStart;
  CXFA_NodeHelper* m_pNodeHelper;
};

#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_RESOLVEPROCESSOR_H_
