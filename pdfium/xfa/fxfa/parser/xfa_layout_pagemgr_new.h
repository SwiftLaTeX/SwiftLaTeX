// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_LAYOUT_PAGEMGR_NEW_H_
#define XFA_FXFA_PARSER_XFA_LAYOUT_PAGEMGR_NEW_H_

#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_layout_itemlayout.h"

class CXFA_ContainerRecord {
 public:
  CXFA_ContainerRecord(CXFA_ContainerLayoutItem* pPageSet = nullptr,
                       CXFA_ContainerLayoutItem* pPageArea = nullptr,
                       CXFA_ContainerLayoutItem* pContentArea = nullptr)
      : pCurPageSet(pPageSet),
        pCurPageArea(pPageArea),
        pCurContentArea(pContentArea) {}
  CXFA_ContainerLayoutItem* pCurPageSet;
  CXFA_ContainerLayoutItem* pCurPageArea;
  CXFA_ContainerLayoutItem* pCurContentArea;
};

class CXFA_LayoutPageMgr {
 public:
  CXFA_LayoutPageMgr(CXFA_LayoutProcessor* pLayoutProcessor);
  ~CXFA_LayoutPageMgr();

  FX_BOOL InitLayoutPage(CXFA_Node* pFormNode);
  FX_BOOL PrepareFirstPage(CXFA_Node* pRootSubform);
  FX_FLOAT GetAvailHeight();
  FX_BOOL GetNextAvailContentHeight(FX_FLOAT fChildHeight);
  void SubmitContentItem(CXFA_ContentLayoutItem* pContentLayoutItem,
                         XFA_ItemLayoutProcessorResult eStatus);
  void FinishPaginatedPageSets();
  void SyncLayoutData();
  int32_t GetPageCount() const;
  CXFA_ContainerLayoutItem* GetPage(int32_t index) const;
  int32_t GetPageIndex(const CXFA_ContainerLayoutItem* pPage) const;
  inline CXFA_ContainerLayoutItem* GetRootLayoutItem() const {
    return m_pPageSetLayoutItemRoot;
  }
  FX_BOOL ProcessBreakBeforeOrAfter(CXFA_Node* pBreakNode,
                                    FX_BOOL bBefore,
                                    CXFA_Node*& pBreakLeaderNode,
                                    CXFA_Node*& pBreakTrailerNode,
                                    FX_BOOL& bCreatePage);
  FX_BOOL ProcessOverflow(CXFA_Node* pFormNode,
                          CXFA_Node*& pLeaderNode,
                          CXFA_Node*& pTrailerNode,
                          FX_BOOL bDataMerge = FALSE,
                          FX_BOOL bCreatePage = TRUE);
  CXFA_Node* QueryOverflow(CXFA_Node* pFormNode,
                           CXFA_LayoutContext* pLayoutContext = nullptr);
  FX_BOOL ProcessBookendLeaderOrTrailer(CXFA_Node* pBookendNode,
                                        FX_BOOL bLeader,
                                        CXFA_Node*& pBookendAppendNode);
  CXFA_LayoutItem* FindOrCreateLayoutItem(CXFA_Node* pFormNode);

 protected:
  FX_BOOL AppendNewPage(FX_BOOL bFirstTemPage = FALSE);
  void ReorderPendingLayoutRecordToTail(CXFA_ContainerRecord* pNewRecord,
                                        CXFA_ContainerRecord* pPrevRecord);
  void RemoveLayoutRecord(CXFA_ContainerRecord* pNewRecord,
                          CXFA_ContainerRecord* pPrevRecord);
  inline CXFA_ContainerRecord* GetCurrentContainerRecord() {
    CXFA_ContainerRecord* result =
        ((CXFA_ContainerRecord*)m_rgProposedContainerRecord.GetAt(
            m_pCurrentContainerRecord));
    ASSERT(result);
    return result;
  }
  CXFA_ContainerRecord* CreateContainerRecord(CXFA_Node* pPageNode = nullptr,
                                              FX_BOOL bCreateNew = FALSE);
  void AddPageAreaLayoutItem(CXFA_ContainerRecord* pNewRecord,
                             CXFA_Node* pNewPageArea);
  void AddContentAreaLayoutItem(CXFA_ContainerRecord* pNewRecord,
                                CXFA_Node* pContentArea);
  FX_BOOL RunBreak(XFA_Element eBreakType,
                   XFA_ATTRIBUTEENUM eTargetType,
                   CXFA_Node* pTarget,
                   FX_BOOL bStartNew);
  CXFA_Node* BreakOverflow(CXFA_Node* pOverflowNode,
                           CXFA_Node*& pLeaderTemplate,
                           CXFA_Node*& pTrailerTemplate,
                           FX_BOOL bCreatePage = TRUE);
  FX_BOOL ResolveBookendLeaderOrTrailer(CXFA_Node* pBookendNode,
                                        FX_BOOL bLeader,
                                        CXFA_Node*& pBookendAppendTemplate);
  FX_BOOL ExecuteBreakBeforeOrAfter(CXFA_Node* pCurNode,
                                    FX_BOOL bBefore,
                                    CXFA_Node*& pBreakLeaderTemplate,
                                    CXFA_Node*& pBreakTrailerTemplate);

  int32_t CreateMinPageRecord(CXFA_Node* pPageArea,
                              FX_BOOL bTargetPageArea,
                              FX_BOOL bCreateLast = FALSE);
  void CreateMinPageSetRecord(CXFA_Node* pPageSet, FX_BOOL bCreateAll = FALSE);
  void CreateNextMinRecord(CXFA_Node* pRecordNode);
  FX_BOOL FindPageAreaFromPageSet(CXFA_Node* pPageSet,
                                  CXFA_Node* pStartChild,
                                  CXFA_Node* pTargetPageArea = nullptr,
                                  CXFA_Node* pTargetContentArea = nullptr,
                                  FX_BOOL bNewPage = FALSE,
                                  FX_BOOL bQuery = FALSE);
  FX_BOOL FindPageAreaFromPageSet_Ordered(
      CXFA_Node* pPageSet,
      CXFA_Node* pStartChild,
      CXFA_Node* pTargetPageArea = nullptr,
      CXFA_Node* pTargetContentArea = nullptr,
      FX_BOOL bNewPage = FALSE,
      FX_BOOL bQuery = FALSE);
  FX_BOOL FindPageAreaFromPageSet_SimplexDuplex(
      CXFA_Node* pPageSet,
      CXFA_Node* pStartChild,
      CXFA_Node* pTargetPageArea = nullptr,
      CXFA_Node* pTargetContentArea = nullptr,
      FX_BOOL bNewPage = FALSE,
      FX_BOOL bQuery = FALSE,
      XFA_ATTRIBUTEENUM ePreferredPosition = XFA_ATTRIBUTEENUM_First);
  FX_BOOL MatchPageAreaOddOrEven(CXFA_Node* pPageArea, FX_BOOL bLastMatch);
  CXFA_Node* GetNextAvailPageArea(CXFA_Node* pTargetPageArea,
                                  CXFA_Node* pTargetContentArea = nullptr,
                                  FX_BOOL bNewPage = FALSE,
                                  FX_BOOL bQuery = FALSE);
  FX_BOOL GetNextContentArea(CXFA_Node* pTargetContentArea);
  void InitPageSetMap();
  void ProcessLastPageSet();
  inline FX_BOOL IsPageSetRootOrderedOccurrence() {
    return m_ePageSetMode == XFA_ATTRIBUTEENUM_OrderedOccurrence;
  }
  void ClearData();
  void ClearRecordList();
  void MergePageSetContents();
  void LayoutPageSetContents();
  void PrepareLayout();
  void SaveLayoutItem(CXFA_LayoutItem* pParentLayoutItem);
  CXFA_LayoutProcessor* m_pLayoutProcessor;
  CXFA_Node* m_pTemplatePageSetRoot;
  CXFA_ContainerLayoutItem* m_pPageSetLayoutItemRoot;
  CXFA_ContainerLayoutItem* m_pPageSetCurRoot;
  FX_POSITION m_pCurrentContainerRecord;
  CFX_PtrList m_rgProposedContainerRecord;
  CXFA_Node* m_pCurPageArea;
  int32_t m_nAvailPages;
  int32_t m_nCurPageCount;
  XFA_ATTRIBUTEENUM m_ePageSetMode;
  FX_BOOL m_bCreateOverFlowPage;
  CFX_MapPtrTemplate<CXFA_Node*, int32_t> m_pPageSetMap;
  CFX_ArrayTemplate<CXFA_ContainerLayoutItem*> m_PageArray;
};

#endif  // XFA_FXFA_PARSER_XFA_LAYOUT_PAGEMGR_NEW_H_
