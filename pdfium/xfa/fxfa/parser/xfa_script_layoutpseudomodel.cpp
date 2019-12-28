// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_layoutpseudomodel.h"

#include <set>

#include "fxjs/include/cfxjse_arguments.h"
#include "third_party/base/stl_util.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_layout_appadapter.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CScript_LayoutPseudoModel::CScript_LayoutPseudoModel(CXFA_Document* pDocument)
    : CXFA_Object(pDocument,
                  XFA_ObjectType::Object,
                  XFA_Element::LayoutPseudoModel) {}
CScript_LayoutPseudoModel::~CScript_LayoutPseudoModel() {}
void CScript_LayoutPseudoModel::Ready(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  if (bSetting) {
    ThrowException(XFA_IDS_UNABLE_SET_READY);
    return;
  }
  int32_t iStatus = pNotify->GetLayoutStatus();
  pValue->SetBoolean(iStatus >= 2);
}
void CScript_LayoutPseudoModel::HWXY(CFXJSE_Arguments* pArguments,
                                     XFA_LAYOUTMODEL_HWXY layoutModel) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 3) {
    const FX_WCHAR* methodName = nullptr;
    switch (layoutModel) {
      case XFA_LAYOUTMODEL_H:
        methodName = L"h";
        break;
      case XFA_LAYOUTMODEL_W:
        methodName = L"w";
        break;
      case XFA_LAYOUTMODEL_X:
        methodName = L"x";
        break;
      case XFA_LAYOUTMODEL_Y:
        methodName = L"y";
        break;
    }
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, methodName);
    return;
  }
  CXFA_Node* pNode = nullptr;
  CFX_WideString wsUnit(L"pt");
  int32_t iIndex = 0;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  if (iLength >= 2) {
    CFX_ByteString bsUnit = pArguments->GetUTF8String(1);
    if (!bsUnit.IsEmpty()) {
      wsUnit = CFX_WideString::FromUTF8(bsUnit.AsStringC());
    }
  }
  if (iLength >= 3) {
    iIndex = pArguments->GetInt32(2);
  }
  if (!pNode) {
    return;
  }
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CFX_RectF rtRect;
  CXFA_Measurement measure;
  CXFA_LayoutItem* pLayoutItem = pDocLayout->GetLayoutItem(pNode);
  if (!pLayoutItem) {
    return;
  }
  while (iIndex > 0 && pLayoutItem) {
    pLayoutItem = pLayoutItem->GetNext();
    iIndex--;
  }
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (!pLayoutItem) {
    pValue->SetFloat(0);
    return;
  }
  pLayoutItem->GetRect(rtRect, TRUE);
  switch (layoutModel) {
    case XFA_LAYOUTMODEL_H:
      measure.Set(rtRect.height, XFA_UNIT_Pt);
      break;
    case XFA_LAYOUTMODEL_W:
      measure.Set(rtRect.width, XFA_UNIT_Pt);
      break;
    case XFA_LAYOUTMODEL_X:
      measure.Set(rtRect.left, XFA_UNIT_Pt);
      break;
    case XFA_LAYOUTMODEL_Y:
      measure.Set(rtRect.top, XFA_UNIT_Pt);
      break;
  }
  XFA_UNIT unit = measure.GetUnit(wsUnit.AsStringC());
  FX_FLOAT fValue = measure.ToUnit(unit);
  fValue = FXSYS_round(fValue * 1000) / 1000.0f;
  if (pValue)
    pValue->SetFloat(fValue);
}
void CScript_LayoutPseudoModel::H(CFXJSE_Arguments* pArguments) {
  HWXY(pArguments, XFA_LAYOUTMODEL_H);
}
void CScript_LayoutPseudoModel::W(CFXJSE_Arguments* pArguments) {
  HWXY(pArguments, XFA_LAYOUTMODEL_W);
}
void CScript_LayoutPseudoModel::X(CFXJSE_Arguments* pArguments) {
  HWXY(pArguments, XFA_LAYOUTMODEL_X);
}
void CScript_LayoutPseudoModel::Y(CFXJSE_Arguments* pArguments) {
  HWXY(pArguments, XFA_LAYOUTMODEL_Y);
}
void CScript_LayoutPseudoModel::NumberedPageCount(CFXJSE_Arguments* pArguments,
                                                  FX_BOOL bNumbered) {
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  int32_t iPageCount = 0;
  int32_t iPageNum = pDocLayout->CountPages();
  if (bNumbered) {
    for (int32_t i = 0; i < iPageNum; i++) {
      CXFA_ContainerLayoutItem* pLayoutPage = pDocLayout->GetPage(i);
      if (!pLayoutPage) {
        continue;
      }
      CXFA_Node* pMasterPage = pLayoutPage->GetMasterPage();
      if (pMasterPage->GetInteger(XFA_ATTRIBUTE_Numbered)) {
        iPageCount++;
      }
    }
  } else {
    iPageCount = iPageNum;
  }
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(iPageCount);
}
void CScript_LayoutPseudoModel::PageCount(CFXJSE_Arguments* pArguments) {
  NumberedPageCount(pArguments, TRUE);
}
void CScript_LayoutPseudoModel::PageSpan(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"pageSpan");
    return;
  }
  CXFA_Node* pNode = nullptr;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  if (!pNode) {
    return;
  }
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  CXFA_LayoutItem* pLayoutItem = pDocLayout->GetLayoutItem(pNode);
  if (!pLayoutItem) {
    pValue->SetInteger(-1);
    return;
  }
  int32_t iLast = pLayoutItem->GetLast()->GetPage()->GetPageIndex();
  int32_t iFirst = pLayoutItem->GetFirst()->GetPage()->GetPageIndex();
  int32_t iPageSpan = iLast - iFirst + 1;
  if (pValue)
    pValue->SetInteger(iPageSpan);
}
void CScript_LayoutPseudoModel::Page(CFXJSE_Arguments* pArguments) {
  PageImp(pArguments, FALSE);
}
void CScript_LayoutPseudoModel::GetObjArray(CXFA_LayoutProcessor* pDocLayout,
                                            int32_t iPageNo,
                                            const CFX_WideString& wsType,
                                            FX_BOOL bOnPageArea,
                                            CXFA_NodeArray& retArray) {
  CXFA_ContainerLayoutItem* pLayoutPage = pDocLayout->GetPage(iPageNo);
  if (!pLayoutPage) {
    return;
  }
  if (wsType == FX_WSTRC(L"pageArea")) {
    if (CXFA_Node* pMasterPage = pLayoutPage->m_pFormNode) {
      retArray.Add(pMasterPage);
    }
    return;
  }
  if (wsType == FX_WSTRC(L"contentArea")) {
    for (CXFA_LayoutItem* pItem = pLayoutPage->m_pFirstChild; pItem;
         pItem = pItem->m_pNextSibling) {
      if (pItem->m_pFormNode->GetElementType() == XFA_Element::ContentArea) {
        retArray.Add(pItem->m_pFormNode);
      }
    }
    return;
  }
  std::set<CXFA_Node*> formItems;
  if (wsType.IsEmpty()) {
    if (CXFA_Node* pMasterPage = pLayoutPage->m_pFormNode) {
      retArray.Add(pMasterPage);
    }
    for (CXFA_LayoutItem* pItem = pLayoutPage->m_pFirstChild; pItem;
         pItem = pItem->m_pNextSibling) {
      if (pItem->m_pFormNode->GetElementType() == XFA_Element::ContentArea) {
        retArray.Add(pItem->m_pFormNode);
        if (!bOnPageArea) {
          CXFA_NodeIteratorTemplate<CXFA_ContentLayoutItem,
                                    CXFA_TraverseStrategy_ContentLayoutItem>
          iterator(static_cast<CXFA_ContentLayoutItem*>(pItem->m_pFirstChild));
          for (CXFA_ContentLayoutItem* pItemChild = iterator.GetCurrent();
               pItemChild; pItemChild = iterator.MoveToNext()) {
            if (!pItemChild->IsContentLayoutItem()) {
              continue;
            }
            XFA_Element eType = pItemChild->m_pFormNode->GetElementType();
            if (eType != XFA_Element::Field && eType != XFA_Element::Draw &&
                eType != XFA_Element::Subform && eType != XFA_Element::Area) {
              continue;
            }
            if (pdfium::ContainsValue(formItems, pItemChild->m_pFormNode))
              continue;

            formItems.insert(pItemChild->m_pFormNode);
            retArray.Add(pItemChild->m_pFormNode);
          }
        }
      } else {
        if (bOnPageArea) {
          CXFA_NodeIteratorTemplate<CXFA_ContentLayoutItem,
                                    CXFA_TraverseStrategy_ContentLayoutItem>
          iterator(static_cast<CXFA_ContentLayoutItem*>(pItem));
          for (CXFA_ContentLayoutItem* pItemChild = iterator.GetCurrent();
               pItemChild; pItemChild = iterator.MoveToNext()) {
            if (!pItemChild->IsContentLayoutItem()) {
              continue;
            }
            XFA_Element eType = pItemChild->m_pFormNode->GetElementType();
            if (eType != XFA_Element::Field && eType != XFA_Element::Draw &&
                eType != XFA_Element::Subform && eType != XFA_Element::Area) {
              continue;
            }
            if (pdfium::ContainsValue(formItems, pItemChild->m_pFormNode))
              continue;
            formItems.insert(pItemChild->m_pFormNode);
            retArray.Add(pItemChild->m_pFormNode);
          }
        }
      }
    }
    return;
  }
  XFA_Element eType = XFA_Element::Unknown;
  if (wsType == FX_WSTRC(L"field")) {
    eType = XFA_Element::Field;
  } else if (wsType == FX_WSTRC(L"draw")) {
    eType = XFA_Element::Draw;
  } else if (wsType == FX_WSTRC(L"subform")) {
    eType = XFA_Element::Subform;
  } else if (wsType == FX_WSTRC(L"area")) {
    eType = XFA_Element::Area;
  }
  if (eType != XFA_Element::Unknown) {
    for (CXFA_LayoutItem* pItem = pLayoutPage->m_pFirstChild; pItem;
         pItem = pItem->m_pNextSibling) {
      if (pItem->m_pFormNode->GetElementType() == XFA_Element::ContentArea) {
        if (!bOnPageArea) {
          CXFA_NodeIteratorTemplate<CXFA_ContentLayoutItem,
                                    CXFA_TraverseStrategy_ContentLayoutItem>
          iterator(static_cast<CXFA_ContentLayoutItem*>(pItem->m_pFirstChild));
          for (CXFA_ContentLayoutItem* pItemChild = iterator.GetCurrent();
               pItemChild; pItemChild = iterator.MoveToNext()) {
            if (!pItemChild->IsContentLayoutItem())
              continue;
            if (pItemChild->m_pFormNode->GetElementType() != eType)
              continue;
            if (pdfium::ContainsValue(formItems, pItemChild->m_pFormNode))
              continue;
            formItems.insert(pItemChild->m_pFormNode);
            retArray.Add(pItemChild->m_pFormNode);
          }
        }
      } else {
        if (bOnPageArea) {
          CXFA_NodeIteratorTemplate<CXFA_ContentLayoutItem,
                                    CXFA_TraverseStrategy_ContentLayoutItem>
          iterator(static_cast<CXFA_ContentLayoutItem*>(pItem));
          for (CXFA_ContentLayoutItem* pItemChild = iterator.GetCurrent();
               pItemChild; pItemChild = iterator.MoveToNext()) {
            if (!pItemChild->IsContentLayoutItem())
              continue;
            if (pItemChild->m_pFormNode->GetElementType() != eType)
              continue;
            if (pdfium::ContainsValue(formItems, pItemChild->m_pFormNode))
              continue;
            formItems.insert(pItemChild->m_pFormNode);
            retArray.Add(pItemChild->m_pFormNode);
          }
        }
      }
    }
    return;
  }
}
void CScript_LayoutPseudoModel::PageContent(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 3) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"pageContent");
    return;
  }
  int32_t iIndex = 0;
  CFX_WideString wsType;
  FX_BOOL bOnPageArea = FALSE;
  if (iLength >= 1) {
    iIndex = pArguments->GetInt32(0);
  }
  if (iLength >= 2) {
    CFX_ByteString bsType = pArguments->GetUTF8String(1);
    wsType = CFX_WideString::FromUTF8(bsType.AsStringC());
  }
  if (iLength >= 3) {
    bOnPageArea = pArguments->GetInt32(2) == 0 ? FALSE : TRUE;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CXFA_NodeArray retArray;
  GetObjArray(pDocLayout, iIndex, wsType, bOnPageArea, retArray);
  CXFA_ArrayNodeList* pArrayNodeList = new CXFA_ArrayNodeList(m_pDocument);
  pArrayNodeList->SetArrayNodeList(retArray);
  pArguments->GetReturnValue()->SetObject(
      pArrayNodeList, m_pDocument->GetScriptContext()->GetJseNormalClass());
}
void CScript_LayoutPseudoModel::AbsPageCount(CFXJSE_Arguments* pArguments) {
  NumberedPageCount(pArguments, FALSE);
}
void CScript_LayoutPseudoModel::AbsPageCountInBatch(
    CFXJSE_Arguments* pArguments) {
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  int32_t iPageCount = pNotify->GetDocProvider()->AbsPageCountInBatch(hDoc);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(iPageCount);
}
void CScript_LayoutPseudoModel::SheetCountInBatch(
    CFXJSE_Arguments* pArguments) {
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  int32_t iPageCount = pNotify->GetDocProvider()->SheetCountInBatch(hDoc);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(iPageCount);
}
void CScript_LayoutPseudoModel::Relayout(CFXJSE_Arguments* pArguments) {
  CXFA_Node* pRootNode = m_pDocument->GetRoot();
  CXFA_Node* pFormRoot = pRootNode->GetFirstChildByClass(XFA_Element::Form);
  ASSERT(pFormRoot);
  CXFA_Node* pContentRootNode = pFormRoot->GetNodeItem(XFA_NODEITEM_FirstChild);
  CXFA_LayoutProcessor* pLayoutProcessor = m_pDocument->GetLayoutProcessor();
  if (pContentRootNode) {
    pLayoutProcessor->AddChangedContainer(pContentRootNode);
  }
  pLayoutProcessor->SetForceReLayout(TRUE);
}
void CScript_LayoutPseudoModel::AbsPageSpan(CFXJSE_Arguments* pArguments) {
  PageSpan(pArguments);
}
void CScript_LayoutPseudoModel::AbsPageInBatch(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"absPageInBatch");
    return;
  }
  CXFA_Node* pNode = nullptr;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  if (!pNode) {
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CXFA_FFWidget* hWidget =
      pNotify->GetHWidget(pDocLayout->GetLayoutItem(pNode));
  if (!hWidget) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  int32_t iPageCount = pNotify->GetDocProvider()->AbsPageInBatch(hDoc, hWidget);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(iPageCount);
}
void CScript_LayoutPseudoModel::SheetInBatch(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"sheetInBatch");
    return;
  }
  CXFA_Node* pNode = nullptr;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  if (!pNode) {
    return;
  }
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CXFA_FFWidget* hWidget =
      pNotify->GetHWidget(pDocLayout->GetLayoutItem(pNode));
  if (!hWidget) {
    return;
  }
  CXFA_FFDoc* hDoc = pNotify->GetHDOC();
  int32_t iPageCount = pNotify->GetDocProvider()->SheetInBatch(hDoc, hWidget);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetInteger(iPageCount);
}
void CScript_LayoutPseudoModel::Sheet(CFXJSE_Arguments* pArguments) {
  PageImp(pArguments, TRUE);
}
void CScript_LayoutPseudoModel::RelayoutPageArea(CFXJSE_Arguments* pArguments) {
}
void CScript_LayoutPseudoModel::SheetCount(CFXJSE_Arguments* pArguments) {
  NumberedPageCount(pArguments, FALSE);
}
void CScript_LayoutPseudoModel::AbsPage(CFXJSE_Arguments* pArguments) {
  PageImp(pArguments, TRUE);
}
void CScript_LayoutPseudoModel::PageImp(CFXJSE_Arguments* pArguments,
                                        FX_BOOL bAbsPage) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    const FX_WCHAR* methodName;
    if (bAbsPage) {
      methodName = L"absPage";
    } else {
      methodName = L"page";
    }
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, methodName);
    return;
  }
  CXFA_Node* pNode = nullptr;
  if (iLength >= 1) {
    pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  }
  int32_t iPage = 0;
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (!pNode && pValue)
    pValue->SetInteger(iPage);

  CXFA_LayoutProcessor* pDocLayout = m_pDocument->GetDocLayout();
  if (!pDocLayout) {
    return;
  }
  CXFA_LayoutItem* pLayoutItem = pDocLayout->GetLayoutItem(pNode);
  if (!pLayoutItem) {
    pValue->SetInteger(-1);
    return;
  }
  iPage = pLayoutItem->GetFirst()->GetPage()->GetPageIndex();
  if (pValue)
    pValue->SetInteger(bAbsPage ? iPage : iPage + 1);
}
