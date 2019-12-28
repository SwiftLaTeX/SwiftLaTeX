// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_object.h"

#include <memory>

#include "core/fxcrt/include/fx_ext.h"
#include "fxjs/include/cfxjse_arguments.h"
#include "third_party/base/stl_util.h"
#include "xfa/fde/xml/fde_xml_imp.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/crt/fgas_system.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_occur.h"
#include "xfa/fxfa/parser/cxfa_simple_parser.h"
#include "xfa/fxfa/parser/xfa_basic_imp.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_document_layout_imp.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_script_imp.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

void XFA_DeleteWideString(void* pData) {
  delete static_cast<CFX_WideString*>(pData);
}

void XFA_CopyWideString(void*& pData) {
  if (pData) {
    CFX_WideString* pNewData = new CFX_WideString(*(CFX_WideString*)pData);
    pData = pNewData;
  }
}

XFA_MAPDATABLOCKCALLBACKINFO deleteWideStringCallBack = {XFA_DeleteWideString,
                                                         XFA_CopyWideString};

void XFA_DataNodeDeleteBindItem(void* pData) {
  delete static_cast<CXFA_NodeArray*>(pData);
}

XFA_MAPDATABLOCKCALLBACKINFO deleteBindItemCallBack = {
    XFA_DataNodeDeleteBindItem, nullptr};

}  // namespace

CXFA_Object::CXFA_Object(CXFA_Document* pDocument,
                         XFA_ObjectType objectType,
                         XFA_Element elementType)
    : m_pDocument(pDocument),
      m_objectType(objectType),
      m_elementType(elementType) {}

CXFA_Object::~CXFA_Object() {}

CFX_WideStringC CXFA_Object::GetClassName() const {
  return XFA_GetElementByID(GetElementType())->pName;
}

uint32_t CXFA_Object::GetClassHashCode() const {
  return XFA_GetElementByID(GetElementType())->uHash;
}

XFA_Element CXFA_Object::GetElementType() const {
  return m_elementType;
}

void CXFA_Object::Script_ObjectClass_ClassName(CFXJSE_Value* pValue,
                                               FX_BOOL bSetting,
                                               XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    CFX_WideStringC className = GetClassName();
    pValue->SetString(
        FX_UTF8Encode(className.c_str(), className.GetLength()).AsStringC());
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}

void CXFA_Object::ThrowException(int32_t iStringID, ...) {
  IXFA_AppProvider* pAppProvider = m_pDocument->GetNotify()->GetAppProvider();
  ASSERT(pAppProvider);
  CFX_WideString wsFormat;
  pAppProvider->LoadString(iStringID, wsFormat);
  CFX_WideString wsMessage;
  va_list arg_ptr;
  va_start(arg_ptr, iStringID);
  wsMessage.FormatV(wsFormat.c_str(), arg_ptr);
  va_end(arg_ptr);
  FXJSE_ThrowMessage(
      FX_UTF8Encode(wsMessage.c_str(), wsMessage.GetLength()).AsStringC());
}

XFA_MAPMODULEDATA::XFA_MAPMODULEDATA() {}

XFA_MAPMODULEDATA::~XFA_MAPMODULEDATA() {}

CXFA_Node::CXFA_Node(CXFA_Document* pDoc,
                     uint16_t ePacket,
                     XFA_ObjectType oType,
                     XFA_Element eType)
    : CXFA_Object(pDoc, oType, eType),
      m_pNext(nullptr),
      m_pChild(nullptr),
      m_pLastChild(nullptr),
      m_pParent(nullptr),
      m_pXMLNode(nullptr),
      m_ePacket(ePacket),
      m_uNodeFlags(XFA_NodeFlag_None),
      m_dwNameHash(0),
      m_pAuxNode(nullptr),
      m_pMapModuleData(nullptr) {
  ASSERT(m_pDocument);
}

CXFA_Node::~CXFA_Node() {
  ASSERT(!m_pParent);
  RemoveMapModuleKey();
  CXFA_Node* pNode = m_pChild;
  while (pNode) {
    CXFA_Node* pNext = pNode->m_pNext;
    pNode->m_pParent = nullptr;
    delete pNode;
    pNode = pNext;
  }
  if (m_pXMLNode && IsOwnXMLNode())
    m_pXMLNode->Release();
}

CXFA_Node* CXFA_Node::Clone(FX_BOOL bRecursive) {
  CXFA_Node* pClone = m_pDocument->CreateNode(m_ePacket, m_elementType);
  if (!pClone)
    return nullptr;

  MergeAllData(pClone);
  pClone->UpdateNameHash();
  if (IsNeedSavingXMLNode()) {
    CFDE_XMLNode* pCloneXML = nullptr;
    if (IsAttributeInXML()) {
      CFX_WideString wsName;
      GetAttribute(XFA_ATTRIBUTE_Name, wsName, FALSE);
      CFDE_XMLElement* pCloneXMLElement = new CFDE_XMLElement(wsName);
      CFX_WideStringC wsValue = GetCData(XFA_ATTRIBUTE_Value);
      if (!wsValue.IsEmpty()) {
        pCloneXMLElement->SetTextData(CFX_WideString(wsValue));
      }
      pCloneXML = pCloneXMLElement;
      pCloneXMLElement = nullptr;
      pClone->SetEnum(XFA_ATTRIBUTE_Contains, XFA_ATTRIBUTEENUM_Unknown);
    } else {
      pCloneXML = m_pXMLNode->Clone(FALSE);
    }
    pClone->SetXMLMappingNode(pCloneXML);
    pClone->SetFlag(XFA_NodeFlag_OwnXMLNode, false);
  }
  if (bRecursive) {
    for (CXFA_Node* pChild = GetNodeItem(XFA_NODEITEM_FirstChild); pChild;
         pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      pClone->InsertChild(pChild->Clone(bRecursive));
    }
  }
  pClone->SetFlag(XFA_NodeFlag_Initialized, true);
  pClone->SetObject(XFA_ATTRIBUTE_BindingNode, nullptr);
  return pClone;
}

CXFA_Node* CXFA_Node::GetNodeItem(XFA_NODEITEM eItem) const {
  switch (eItem) {
    case XFA_NODEITEM_NextSibling:
      return m_pNext;
    case XFA_NODEITEM_FirstChild:
      return m_pChild;
    case XFA_NODEITEM_Parent:
      return m_pParent;
    case XFA_NODEITEM_PrevSibling:
      if (m_pParent) {
        CXFA_Node* pSibling = m_pParent->m_pChild;
        CXFA_Node* pPrev = nullptr;
        while (pSibling && pSibling != this) {
          pPrev = pSibling;
          pSibling = pSibling->m_pNext;
        }
        return pPrev;
      }
      return nullptr;
    default:
      break;
  }
  return nullptr;
}

CXFA_Node* CXFA_Node::GetNodeItem(XFA_NODEITEM eItem,
                                  XFA_ObjectType eType) const {
  CXFA_Node* pNode = nullptr;
  switch (eItem) {
    case XFA_NODEITEM_NextSibling:
      pNode = m_pNext;
      while (pNode && pNode->GetObjectType() != eType)
        pNode = pNode->m_pNext;
      break;
    case XFA_NODEITEM_FirstChild:
      pNode = m_pChild;
      while (pNode && pNode->GetObjectType() != eType)
        pNode = pNode->m_pNext;
      break;
    case XFA_NODEITEM_Parent:
      pNode = m_pParent;
      while (pNode && pNode->GetObjectType() != eType)
        pNode = pNode->m_pParent;
      break;
    case XFA_NODEITEM_PrevSibling:
      if (m_pParent) {
        CXFA_Node* pSibling = m_pParent->m_pChild;
        while (pSibling && pSibling != this) {
          if (eType == pSibling->GetObjectType())
            pNode = pSibling;

          pSibling = pSibling->m_pNext;
        }
      }
      break;
    default:
      break;
  }
  return pNode;
}

int32_t CXFA_Node::GetNodeList(CXFA_NodeArray& nodes,
                               uint32_t dwTypeFilter,
                               XFA_Element eTypeFilter,
                               int32_t iLevel) {
  if (--iLevel < 0) {
    return nodes.GetSize();
  }
  if (eTypeFilter != XFA_Element::Unknown) {
    CXFA_Node* pChild = m_pChild;
    while (pChild) {
      if (pChild->GetElementType() == eTypeFilter) {
        nodes.Add(pChild);
        if (iLevel > 0) {
          GetNodeList(nodes, dwTypeFilter, eTypeFilter, iLevel);
        }
      }
      pChild = pChild->m_pNext;
    }
  } else if (dwTypeFilter ==
             (XFA_NODEFILTER_Children | XFA_NODEFILTER_Properties)) {
    CXFA_Node* pChild = m_pChild;
    while (pChild) {
      nodes.Add(pChild);
      if (iLevel > 0) {
        GetNodeList(nodes, dwTypeFilter, eTypeFilter, iLevel);
      }
      pChild = pChild->m_pNext;
    }
  } else if (dwTypeFilter != 0) {
    bool bFilterChildren = !!(dwTypeFilter & XFA_NODEFILTER_Children);
    bool bFilterProperties = !!(dwTypeFilter & XFA_NODEFILTER_Properties);
    bool bFilterOneOfProperties =
        !!(dwTypeFilter & XFA_NODEFILTER_OneOfProperty);
    CXFA_Node* pChild = m_pChild;
    while (pChild) {
      const XFA_PROPERTY* pProperty = XFA_GetPropertyOfElement(
          GetElementType(), pChild->GetElementType(), XFA_XDPPACKET_UNKNOWN);
      if (pProperty) {
        if (bFilterProperties) {
          nodes.Add(pChild);
        } else if (bFilterOneOfProperties &&
                   (pProperty->uFlags & XFA_PROPERTYFLAG_OneOf)) {
          nodes.Add(pChild);
        } else if (bFilterChildren &&
                   (pChild->GetElementType() == XFA_Element::Variables ||
                    pChild->GetElementType() == XFA_Element::PageSet)) {
          nodes.Add(pChild);
        }
      } else if (bFilterChildren) {
        nodes.Add(pChild);
      }
      pChild = pChild->m_pNext;
    }
    if (bFilterOneOfProperties && nodes.GetSize() < 1) {
      int32_t iProperties = 0;
      const XFA_PROPERTY* pProperty =
          XFA_GetElementProperties(GetElementType(), iProperties);
      if (!pProperty || iProperties < 1)
        return 0;
      for (int32_t i = 0; i < iProperties; i++) {
        if (pProperty[i].uFlags & XFA_PROPERTYFLAG_DefaultOneOf) {
          const XFA_PACKETINFO* pPacket = XFA_GetPacketByID(GetPacketID());
          CXFA_Node* pNewNode =
              m_pDocument->CreateNode(pPacket, pProperty[i].eName);
          if (!pNewNode)
            break;
          InsertChild(pNewNode, nullptr);
          pNewNode->SetFlag(XFA_NodeFlag_Initialized, true);
          nodes.Add(pNewNode);
          break;
        }
      }
    }
  }
  return nodes.GetSize();
}

CXFA_Node* CXFA_Node::CreateSamePacketNode(XFA_Element eType,
                                           uint32_t dwFlags) {
  CXFA_Node* pNode = m_pDocument->CreateNode(m_ePacket, eType);
  pNode->SetFlag(dwFlags, true);
  return pNode;
}

CXFA_Node* CXFA_Node::CloneTemplateToForm(FX_BOOL bRecursive) {
  ASSERT(m_ePacket == XFA_XDPPACKET_Template);
  CXFA_Node* pClone =
      m_pDocument->CreateNode(XFA_XDPPACKET_Form, m_elementType);
  if (!pClone)
    return nullptr;

  pClone->SetTemplateNode(this);
  pClone->UpdateNameHash();
  pClone->SetXMLMappingNode(GetXMLMappingNode());
  if (bRecursive) {
    for (CXFA_Node* pChild = GetNodeItem(XFA_NODEITEM_FirstChild); pChild;
         pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      pClone->InsertChild(pChild->CloneTemplateToForm(bRecursive));
    }
  }
  pClone->SetFlag(XFA_NodeFlag_Initialized, true);
  return pClone;
}

CXFA_Node* CXFA_Node::GetTemplateNode() const {
  return m_pAuxNode;
}

void CXFA_Node::SetTemplateNode(CXFA_Node* pTemplateNode) {
  m_pAuxNode = pTemplateNode;
}

CXFA_Node* CXFA_Node::GetBindData() {
  ASSERT(GetPacketID() == XFA_XDPPACKET_Form);
  return static_cast<CXFA_Node*>(GetObject(XFA_ATTRIBUTE_BindingNode));
}

int32_t CXFA_Node::GetBindItems(CXFA_NodeArray& formItems) {
  if (BindsFormItems()) {
    CXFA_NodeArray* pItems = nullptr;
    TryObject(XFA_ATTRIBUTE_BindingNode, (void*&)pItems);
    formItems.Copy(*pItems);
    return formItems.GetSize();
  }
  CXFA_Node* pFormNode =
      static_cast<CXFA_Node*>(GetObject(XFA_ATTRIBUTE_BindingNode));
  if (pFormNode)
    formItems.Add(pFormNode);
  return formItems.GetSize();
}

int32_t CXFA_Node::AddBindItem(CXFA_Node* pFormNode) {
  ASSERT(pFormNode);
  if (BindsFormItems()) {
    CXFA_NodeArray* pItems = nullptr;
    TryObject(XFA_ATTRIBUTE_BindingNode, (void*&)pItems);
    ASSERT(pItems);
    if (pItems->Find(pFormNode) < 0) {
      pItems->Add(pFormNode);
    }
    return pItems->GetSize();
  }
  CXFA_Node* pOldFormItem =
      static_cast<CXFA_Node*>(GetObject(XFA_ATTRIBUTE_BindingNode));
  if (!pOldFormItem) {
    SetObject(XFA_ATTRIBUTE_BindingNode, pFormNode);
    return 1;
  } else if (pOldFormItem == pFormNode) {
    return 1;
  }
  CXFA_NodeArray* pItems = new CXFA_NodeArray;
  SetObject(XFA_ATTRIBUTE_BindingNode, pItems, &deleteBindItemCallBack);
  pItems->Add(pOldFormItem);
  pItems->Add(pFormNode);
  m_uNodeFlags |= XFA_NodeFlag_BindFormItems;
  return 2;
}

int32_t CXFA_Node::RemoveBindItem(CXFA_Node* pFormNode) {
  if (BindsFormItems()) {
    CXFA_NodeArray* pItems = nullptr;
    TryObject(XFA_ATTRIBUTE_BindingNode, (void*&)pItems);
    ASSERT(pItems);
    int32_t iIndex = pItems->Find(pFormNode);
    int32_t iCount = pItems->GetSize();
    if (iIndex >= 0) {
      if (iIndex != iCount - 1)
        pItems->SetAt(iIndex, pItems->GetAt(iCount - 1));
      pItems->RemoveAt(iCount - 1);
      if (iCount == 2) {
        CXFA_Node* pLastFormNode = pItems->GetAt(0);
        SetObject(XFA_ATTRIBUTE_BindingNode, pLastFormNode);
        m_uNodeFlags &= ~XFA_NodeFlag_BindFormItems;
      }
      iCount--;
    }
    return iCount;
  }
  CXFA_Node* pOldFormItem =
      static_cast<CXFA_Node*>(GetObject(XFA_ATTRIBUTE_BindingNode));
  if (pOldFormItem == pFormNode) {
    SetObject(XFA_ATTRIBUTE_BindingNode, nullptr);
    pOldFormItem = nullptr;
  }
  return pOldFormItem ? 1 : 0;
}

FX_BOOL CXFA_Node::HasBindItem() {
  return GetPacketID() == XFA_XDPPACKET_Datasets &&
         GetObject(XFA_ATTRIBUTE_BindingNode);
}

CXFA_WidgetData* CXFA_Node::GetWidgetData() {
  return (CXFA_WidgetData*)GetObject(XFA_ATTRIBUTE_WidgetData);
}

CXFA_WidgetData* CXFA_Node::GetContainerWidgetData() {
  if (GetPacketID() != XFA_XDPPACKET_Form)
    return nullptr;
  XFA_Element eType = GetElementType();
  if (eType == XFA_Element::ExclGroup)
    return nullptr;
  CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
  if (pParentNode && pParentNode->GetElementType() == XFA_Element::ExclGroup)
    return nullptr;

  if (eType == XFA_Element::Field) {
    CXFA_WidgetData* pFieldWidgetData = GetWidgetData();
    if (pFieldWidgetData &&
        pFieldWidgetData->GetChoiceListOpen() ==
            XFA_ATTRIBUTEENUM_MultiSelect) {
      return nullptr;
    } else {
      CFX_WideString wsPicture;
      if (pFieldWidgetData) {
        pFieldWidgetData->GetPictureContent(wsPicture,
                                            XFA_VALUEPICTURE_DataBind);
      }
      if (!wsPicture.IsEmpty())
        return pFieldWidgetData;
      CXFA_Node* pDataNode = GetBindData();
      if (!pDataNode)
        return nullptr;
      pFieldWidgetData = nullptr;
      CXFA_NodeArray formNodes;
      pDataNode->GetBindItems(formNodes);
      for (int32_t i = 0; i < formNodes.GetSize(); i++) {
        CXFA_Node* pFormNode = formNodes.GetAt(i);
        if (!pFormNode || pFormNode->HasRemovedChildren())
          continue;
        pFieldWidgetData = pFormNode->GetWidgetData();
        if (pFieldWidgetData) {
          pFieldWidgetData->GetPictureContent(wsPicture,
                                              XFA_VALUEPICTURE_DataBind);
        }
        if (!wsPicture.IsEmpty())
          break;
        pFieldWidgetData = nullptr;
      }
      return pFieldWidgetData;
    }
  }
  CXFA_Node* pGrandNode =
      pParentNode ? pParentNode->GetNodeItem(XFA_NODEITEM_Parent) : nullptr;
  CXFA_Node* pValueNode =
      (pParentNode && pParentNode->GetElementType() == XFA_Element::Value)
          ? pParentNode
          : nullptr;
  if (!pValueNode) {
    pValueNode =
        (pGrandNode && pGrandNode->GetElementType() == XFA_Element::Value)
            ? pGrandNode
            : nullptr;
  }
  CXFA_Node* pParentOfValueNode =
      pValueNode ? pValueNode->GetNodeItem(XFA_NODEITEM_Parent) : nullptr;
  return pParentOfValueNode ? pParentOfValueNode->GetContainerWidgetData()
                            : nullptr;
}

FX_BOOL CXFA_Node::GetLocaleName(CFX_WideString& wsLocaleName) {
  CXFA_Node* pForm = GetDocument()->GetXFAObject(XFA_HASHCODE_Form)->AsNode();
  CXFA_Node* pTopSubform = pForm->GetFirstChildByClass(XFA_Element::Subform);
  ASSERT(pTopSubform);
  CXFA_Node* pLocaleNode = this;
  FX_BOOL bLocale = FALSE;
  do {
    bLocale = pLocaleNode->TryCData(XFA_ATTRIBUTE_Locale, wsLocaleName, FALSE);
    if (!bLocale) {
      pLocaleNode = pLocaleNode->GetNodeItem(XFA_NODEITEM_Parent);
    }
  } while (pLocaleNode && pLocaleNode != pTopSubform && !bLocale);
  if (bLocale)
    return TRUE;
  CXFA_Node* pConfig = ToNode(GetDocument()->GetXFAObject(XFA_HASHCODE_Config));
  wsLocaleName = GetDocument()->GetLocalMgr()->GetConfigLocaleName(pConfig);
  if (!wsLocaleName.IsEmpty())
    return TRUE;
  if (pTopSubform &&
      pTopSubform->TryCData(XFA_ATTRIBUTE_Locale, wsLocaleName, FALSE)) {
    return TRUE;
  }
  IFX_Locale* pLocale = GetDocument()->GetLocalMgr()->GetDefLocale();
  if (pLocale) {
    wsLocaleName = pLocale->GetName();
    return TRUE;
  }
  return FALSE;
}

XFA_ATTRIBUTEENUM CXFA_Node::GetIntact() {
  CXFA_Node* pKeep = GetFirstChildByClass(XFA_Element::Keep);
  XFA_ATTRIBUTEENUM eLayoutType = GetEnum(XFA_ATTRIBUTE_Layout);
  if (pKeep) {
    XFA_ATTRIBUTEENUM eIntact;
    if (pKeep->TryEnum(XFA_ATTRIBUTE_Intact, eIntact, FALSE)) {
      if (eIntact == XFA_ATTRIBUTEENUM_None &&
          eLayoutType == XFA_ATTRIBUTEENUM_Row &&
          m_pDocument->GetCurVersionMode() < XFA_VERSION_208) {
        CXFA_Node* pPreviewRow = GetNodeItem(XFA_NODEITEM_PrevSibling,
                                             XFA_ObjectType::ContainerNode);
        if (pPreviewRow &&
            pPreviewRow->GetEnum(XFA_ATTRIBUTE_Layout) ==
                XFA_ATTRIBUTEENUM_Row) {
          XFA_ATTRIBUTEENUM eValue;
          if (pKeep->TryEnum(XFA_ATTRIBUTE_Previous, eValue, FALSE) &&
              (eValue == XFA_ATTRIBUTEENUM_ContentArea ||
               eValue == XFA_ATTRIBUTEENUM_PageArea)) {
            return XFA_ATTRIBUTEENUM_ContentArea;
          }
          CXFA_Node* pNode =
              pPreviewRow->GetFirstChildByClass(XFA_Element::Keep);
          if (pNode && pNode->TryEnum(XFA_ATTRIBUTE_Next, eValue, FALSE) &&
              (eValue == XFA_ATTRIBUTEENUM_ContentArea ||
               eValue == XFA_ATTRIBUTEENUM_PageArea)) {
            return XFA_ATTRIBUTEENUM_ContentArea;
          }
        }
      }
      return eIntact;
    }
  }
  switch (GetElementType()) {
    case XFA_Element::Subform:
      switch (eLayoutType) {
        case XFA_ATTRIBUTEENUM_Position:
        case XFA_ATTRIBUTEENUM_Row:
          return XFA_ATTRIBUTEENUM_ContentArea;
        case XFA_ATTRIBUTEENUM_Tb:
        case XFA_ATTRIBUTEENUM_Table:
        case XFA_ATTRIBUTEENUM_Lr_tb:
        case XFA_ATTRIBUTEENUM_Rl_tb:
          return XFA_ATTRIBUTEENUM_None;
        default:
          break;
      }
      break;
    case XFA_Element::Field: {
      CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
      if (!pParentNode ||
          pParentNode->GetElementType() == XFA_Element::PageArea)
        return XFA_ATTRIBUTEENUM_ContentArea;
      if (pParentNode->GetIntact() == XFA_ATTRIBUTEENUM_None) {
        XFA_ATTRIBUTEENUM eParLayout =
            pParentNode->GetEnum(XFA_ATTRIBUTE_Layout);
        if (eParLayout == XFA_ATTRIBUTEENUM_Position ||
            eParLayout == XFA_ATTRIBUTEENUM_Row ||
            eParLayout == XFA_ATTRIBUTEENUM_Table) {
          return XFA_ATTRIBUTEENUM_None;
        }
        XFA_VERSION version = m_pDocument->GetCurVersionMode();
        if (eParLayout == XFA_ATTRIBUTEENUM_Tb && version < XFA_VERSION_208) {
          CXFA_Measurement measureH;
          if (TryMeasure(XFA_ATTRIBUTE_H, measureH, FALSE))
            return XFA_ATTRIBUTEENUM_ContentArea;
        }
        return XFA_ATTRIBUTEENUM_None;
      }
      return XFA_ATTRIBUTEENUM_ContentArea;
    }
    case XFA_Element::Draw:
      return XFA_ATTRIBUTEENUM_ContentArea;
    default:
      break;
  }
  return XFA_ATTRIBUTEENUM_None;
}

CXFA_Node* CXFA_Node::GetDataDescriptionNode() {
  if (m_ePacket == XFA_XDPPACKET_Datasets)
    return m_pAuxNode;
  return nullptr;
}

void CXFA_Node::SetDataDescriptionNode(CXFA_Node* pDataDescriptionNode) {
  ASSERT(m_ePacket == XFA_XDPPACKET_Datasets);
  m_pAuxNode = pDataDescriptionNode;
}

void CXFA_Node::Script_TreeClass_ResolveNode(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"resolveNode");
    return;
  }
  CFX_WideString wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext)
    return;
  CXFA_Node* refNode = this;
  if (refNode->GetElementType() == XFA_Element::Xfa)
    refNode = ToNode(pScriptContext->GetThisObject());
  uint32_t dwFlag = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                    XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                    XFA_RESOLVENODE_Siblings;
  XFA_RESOLVENODE_RS resoveNodeRS;
  int32_t iRet = pScriptContext->ResolveObjects(
      refNode, wsExpression.AsStringC(), resoveNodeRS, dwFlag);
  if (iRet < 1) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  if (resoveNodeRS.dwFlags == XFA_RESOVENODE_RSTYPE_Nodes) {
    CXFA_Object* pNode = resoveNodeRS.nodes[0];
    pArguments->GetReturnValue()->Assign(
        pScriptContext->GetJSValueFromMap(pNode));
  } else {
    const XFA_SCRIPTATTRIBUTEINFO* lpAttributeInfo =
        resoveNodeRS.pScriptAttribute;
    if (lpAttributeInfo && lpAttributeInfo->eValueType == XFA_SCRIPT_Object) {
      std::unique_ptr<CFXJSE_Value> pValue(
          new CFXJSE_Value(pScriptContext->GetRuntime()));
      (resoveNodeRS.nodes[0]->*(lpAttributeInfo->lpfnCallback))(
          pValue.get(), FALSE, (XFA_ATTRIBUTE)lpAttributeInfo->eAttribute);
      pArguments->GetReturnValue()->Assign(pValue.get());
    } else {
      pArguments->GetReturnValue()->SetNull();
    }
  }
}

void CXFA_Node::Script_TreeClass_ResolveNodes(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"resolveNodes");
    return;
  }
  CFX_WideString wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (!pValue)
    return;
  uint32_t dwFlag = XFA_RESOLVENODE_Children | XFA_RESOLVENODE_Attributes |
                    XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                    XFA_RESOLVENODE_Siblings;
  CXFA_Node* refNode = this;
  if (refNode->GetElementType() == XFA_Element::Xfa)
    refNode = ToNode(m_pDocument->GetScriptContext()->GetThisObject());
  Script_Som_ResolveNodeList(pValue, wsExpression, dwFlag, refNode);
}

void CXFA_Node::Script_Som_ResolveNodeList(CFXJSE_Value* pValue,
                                           CFX_WideString wsExpression,
                                           uint32_t dwFlag,
                                           CXFA_Node* refNode) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext)
    return;
  XFA_RESOLVENODE_RS resoveNodeRS;
  if (!refNode)
    refNode = this;
  pScriptContext->ResolveObjects(refNode, wsExpression.AsStringC(),
                                 resoveNodeRS, dwFlag);
  CXFA_ArrayNodeList* pNodeList = new CXFA_ArrayNodeList(m_pDocument);
  if (resoveNodeRS.dwFlags == XFA_RESOVENODE_RSTYPE_Nodes) {
    for (int32_t i = 0; i < resoveNodeRS.nodes.GetSize(); i++) {
      if (resoveNodeRS.nodes[i]->IsNode())
        pNodeList->Append(resoveNodeRS.nodes[i]->AsNode());
    }
  } else {
    CXFA_ValueArray valueArray(pScriptContext->GetRuntime());
    if (resoveNodeRS.GetAttributeResult(valueArray) > 0) {
      CXFA_ObjArray objectArray;
      valueArray.GetAttributeObject(objectArray);
      for (int32_t i = 0; i < objectArray.GetSize(); i++) {
        if (objectArray[i]->IsNode())
          pNodeList->Append(objectArray[i]->AsNode());
      }
    }
  }
  pValue->SetObject(pNodeList, pScriptContext->GetJseNormalClass());
}

void CXFA_Node::Script_TreeClass_All(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    uint32_t dwFlag = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_ALL;
    CFX_WideString wsName;
    GetAttribute(XFA_ATTRIBUTE_Name, wsName);
    CFX_WideString wsExpression = wsName + FX_WSTRC(L"[*]");
    Script_Som_ResolveNodeList(pValue, wsExpression, dwFlag);
  }
}

void CXFA_Node::Script_TreeClass_Nodes(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext)
    return;
  if (bSetting) {
    IXFA_AppProvider* pAppProvider = m_pDocument->GetNotify()->GetAppProvider();
    ASSERT(pAppProvider);
    CFX_WideString wsMessage;
    pAppProvider->LoadString(XFA_IDS_Unable_TO_SET, wsMessage);
    FXJSE_ThrowMessage(
        FX_UTF8Encode(wsMessage.c_str(), wsMessage.GetLength()).AsStringC());
  } else {
    CXFA_AttachNodeList* pNodeList = new CXFA_AttachNodeList(m_pDocument, this);
    pValue->SetObject(pNodeList, pScriptContext->GetJseNormalClass());
  }
}

void CXFA_Node::Script_TreeClass_ClassAll(CFXJSE_Value* pValue,
                                          FX_BOOL bSetting,
                                          XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    uint32_t dwFlag = XFA_RESOLVENODE_Siblings | XFA_RESOLVENODE_ALL;
    CFX_WideString wsExpression =
        FX_WSTRC(L"#") + GetClassName() + FX_WSTRC(L"[*]");
    Script_Som_ResolveNodeList(pValue, wsExpression, dwFlag);
  }
}

void CXFA_Node::Script_TreeClass_Parent(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    CXFA_Node* pParent = GetNodeItem(XFA_NODEITEM_Parent);
    if (pParent) {
      pValue->Assign(
          m_pDocument->GetScriptContext()->GetJSValueFromMap(pParent));
    } else {
      pValue->SetNull();
    }
  }
}

void CXFA_Node::Script_TreeClass_Index(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  if (bSetting)
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  else
    pValue->SetInteger(GetNodeSameNameIndex());
}

void CXFA_Node::Script_TreeClass_ClassIndex(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  if (bSetting)
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  else
    pValue->SetInteger(GetNodeSameClassIndex());
}

void CXFA_Node::Script_TreeClass_SomExpression(CFXJSE_Value* pValue,
                                               FX_BOOL bSetting,
                                               XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    CFX_WideString wsSOMExpression;
    GetSOMExpression(wsSOMExpression);
    pValue->SetString(FX_UTF8Encode(wsSOMExpression).AsStringC());
  }
}

void CXFA_Node::Script_NodeClass_ApplyXSL(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"applyXSL");
    return;
  }
  CFX_WideString wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  // TODO(weili): check whether we need to implement this, pdfium:501.
  // For now, just put the variables here to avoid unused variable warning.
  (void)wsExpression;
}

void CXFA_Node::Script_NodeClass_AssignNode(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 3) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"assignNode");
    return;
  }
  CFX_WideString wsExpression;
  CFX_WideString wsValue;
  int32_t iAction = 0;
  wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  if (iLength >= 2) {
    wsValue =
        CFX_WideString::FromUTF8(pArguments->GetUTF8String(1).AsStringC());
  }
  if (iLength >= 3)
    iAction = pArguments->GetInt32(2);
  // TODO(weili): check whether we need to implement this, pdfium:501.
  // For now, just put the variables here to avoid unused variable warning.
  (void)wsExpression;
  (void)wsValue;
  (void)iAction;
}

void CXFA_Node::Script_NodeClass_Clone(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"clone");
    return;
  }
  bool bClone = !!pArguments->GetInt32(0);
  CXFA_Node* pCloneNode = Clone(bClone);
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pCloneNode));
}

void CXFA_Node::Script_NodeClass_GetAttribute(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getAttribute");
    return;
  }
  CFX_WideString wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  CFX_WideString wsValue;
  GetAttribute(wsExpression.AsStringC(), wsValue);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
}

void CXFA_Node::Script_NodeClass_GetElement(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getElement");
    return;
  }
  CFX_WideString wsExpression;
  int32_t iValue = 0;
  wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  if (iLength >= 2)
    iValue = pArguments->GetInt32(1);
  CXFA_Node* pNode =
      GetProperty(iValue, XFA_GetElementTypeForName(wsExpression.AsStringC()));
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pNode));
}

void CXFA_Node::Script_NodeClass_IsPropertySpecified(
    CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 3) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"isPropertySpecified");
    return;
  }
  CFX_WideString wsExpression;
  bool bParent = true;
  int32_t iIndex = 0;
  wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  if (iLength >= 2)
    bParent = !!pArguments->GetInt32(1);
  if (iLength >= 3)
    iIndex = pArguments->GetInt32(2);
  FX_BOOL bHas = FALSE;
  const XFA_ATTRIBUTEINFO* pAttributeInfo =
      XFA_GetAttributeByName(wsExpression.AsStringC());
  CFX_WideString wsValue;
  if (pAttributeInfo)
    bHas = HasAttribute(pAttributeInfo->eName);
  if (!bHas) {
    XFA_Element eType = XFA_GetElementTypeForName(wsExpression.AsStringC());
    bHas = !!GetProperty(iIndex, eType);
    if (!bHas && bParent && m_pParent) {
      // Also check on the parent.
      bHas = m_pParent->HasAttribute(pAttributeInfo->eName);
      if (!bHas)
        bHas = !!m_pParent->GetProperty(iIndex, eType);
    }
  }
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(bHas);
}

void CXFA_Node::Script_NodeClass_LoadXML(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 3) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"loadXML");
    return;
  }
  CFX_WideString wsExpression;
  bool bIgnoreRoot = true;
  bool bOverwrite = 0;
  wsExpression =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  if (wsExpression.IsEmpty())
    return;
  if (iLength >= 2)
    bIgnoreRoot = !!pArguments->GetInt32(1);
  if (iLength >= 3)
    bOverwrite = !!pArguments->GetInt32(2);
  std::unique_ptr<CXFA_SimpleParser> pParser(
      new CXFA_SimpleParser(m_pDocument, false));
  if (!pParser)
    return;
  CFDE_XMLNode* pXMLNode = nullptr;
  int32_t iParserStatus =
      pParser->ParseXMLData(wsExpression, pXMLNode, nullptr);
  if (iParserStatus != XFA_PARSESTATUS_Done || !pXMLNode)
    return;
  if (bIgnoreRoot &&
      (pXMLNode->GetType() != FDE_XMLNODE_Element ||
       XFA_RecognizeRichText(static_cast<CFDE_XMLElement*>(pXMLNode)))) {
    bIgnoreRoot = false;
  }
  CXFA_Node* pFakeRoot = Clone(FALSE);
  CFX_WideStringC wsContentType = GetCData(XFA_ATTRIBUTE_ContentType);
  if (!wsContentType.IsEmpty()) {
    pFakeRoot->SetCData(XFA_ATTRIBUTE_ContentType,
                        CFX_WideString(wsContentType));
  }
  CFDE_XMLNode* pFakeXMLRoot = pFakeRoot->GetXMLMappingNode();
  if (!pFakeXMLRoot) {
    CFDE_XMLNode* pThisXMLRoot = GetXMLMappingNode();
    pFakeXMLRoot = pThisXMLRoot ? pThisXMLRoot->Clone(FALSE) : nullptr;
  }
  if (!pFakeXMLRoot)
    pFakeXMLRoot = new CFDE_XMLElement(CFX_WideString(GetClassName()));

  if (bIgnoreRoot) {
    CFDE_XMLNode* pXMLChild = pXMLNode->GetNodeItem(CFDE_XMLNode::FirstChild);
    while (pXMLChild) {
      CFDE_XMLNode* pXMLSibling =
          pXMLChild->GetNodeItem(CFDE_XMLNode::NextSibling);
      pXMLNode->RemoveChildNode(pXMLChild);
      pFakeXMLRoot->InsertChildNode(pXMLChild);
      pXMLChild = pXMLSibling;
    }
  } else {
    CFDE_XMLNode* pXMLParent = pXMLNode->GetNodeItem(CFDE_XMLNode::Parent);
    if (pXMLParent) {
      pXMLParent->RemoveChildNode(pXMLNode);
    }
    pFakeXMLRoot->InsertChildNode(pXMLNode);
  }
  pParser->ConstructXFANode(pFakeRoot, pFakeXMLRoot);
  pFakeRoot = pParser->GetRootNode();
  if (pFakeRoot) {
    if (bOverwrite) {
      CXFA_Node* pChild = GetNodeItem(XFA_NODEITEM_FirstChild);
      CXFA_Node* pNewChild = pFakeRoot->GetNodeItem(XFA_NODEITEM_FirstChild);
      int32_t index = 0;
      while (pNewChild) {
        CXFA_Node* pItem = pNewChild->GetNodeItem(XFA_NODEITEM_NextSibling);
        pFakeRoot->RemoveChild(pNewChild);
        InsertChild(index++, pNewChild);
        pNewChild->SetFlag(XFA_NodeFlag_Initialized, true);
        pNewChild = pItem;
      }
      while (pChild) {
        CXFA_Node* pItem = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
        RemoveChild(pChild);
        pFakeRoot->InsertChild(pChild);
        pChild = pItem;
      }
      if (GetPacketID() == XFA_XDPPACKET_Form &&
          GetElementType() == XFA_Element::ExData) {
        CFDE_XMLNode* pTempXMLNode = GetXMLMappingNode();
        SetXMLMappingNode(pFakeXMLRoot);
        SetFlag(XFA_NodeFlag_OwnXMLNode, false);
        if (pTempXMLNode && !pTempXMLNode->GetNodeItem(CFDE_XMLNode::Parent)) {
          pFakeXMLRoot = pTempXMLNode;
        } else {
          pFakeXMLRoot = nullptr;
        }
      }
      MoveBufferMapData(pFakeRoot, this, XFA_CalcData, TRUE);
    } else {
      CXFA_Node* pChild = pFakeRoot->GetNodeItem(XFA_NODEITEM_FirstChild);
      while (pChild) {
        CXFA_Node* pItem = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
        pFakeRoot->RemoveChild(pChild);
        InsertChild(pChild);
        pChild->SetFlag(XFA_NodeFlag_Initialized, true);
        pChild = pItem;
      }
    }
    if (pFakeXMLRoot) {
      pFakeRoot->SetXMLMappingNode(pFakeXMLRoot);
      pFakeRoot->SetFlag(XFA_NodeFlag_OwnXMLNode, false);
    }
    pFakeRoot->SetFlag(XFA_NodeFlag_HasRemovedChildren, false);
  } else {
    if (pFakeXMLRoot) {
      pFakeXMLRoot->Release();
      pFakeXMLRoot = nullptr;
    }
  }
}

void CXFA_Node::Script_NodeClass_SaveFilteredXML(CFXJSE_Arguments* pArguments) {
  // TODO(weili): Check whether we need to implement this, pdfium:501.
}

void CXFA_Node::Script_NodeClass_SaveXML(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 0 || iLength > 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"saveXML");
    return;
  }
  bool bPrettyMode = false;
  if (iLength == 1) {
    if (pArguments->GetUTF8String(0) != "pretty") {
      ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
      return;
    }
    bPrettyMode = true;
  }
  CFX_ByteStringC bsXMLHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  if (GetPacketID() == XFA_XDPPACKET_Form ||
      GetPacketID() == XFA_XDPPACKET_Datasets) {
    CFDE_XMLNode* pElement = nullptr;
    if (GetPacketID() == XFA_XDPPACKET_Datasets) {
      pElement = GetXMLMappingNode();
      if (!pElement || pElement->GetType() != FDE_XMLNODE_Element) {
        pArguments->GetReturnValue()->SetString(bsXMLHeader);
        return;
      }
      XFA_DataExporter_DealWithDataGroupNode(this);
    }
    std::unique_ptr<IFX_MemoryStream, ReleaseDeleter<IFX_MemoryStream>>
        pMemoryStream(FX_CreateMemoryStream(TRUE));
    std::unique_ptr<IFX_Stream, ReleaseDeleter<IFX_Stream>> pStream(
        IFX_Stream::CreateStream(
            static_cast<IFX_FileWrite*>(pMemoryStream.get()),
            FX_STREAMACCESS_Text | FX_STREAMACCESS_Write |
                FX_STREAMACCESS_Append));
    if (!pStream) {
      pArguments->GetReturnValue()->SetString(bsXMLHeader);
      return;
    }
    pStream->SetCodePage(FX_CODEPAGE_UTF8);
    pStream->WriteData(bsXMLHeader.raw_str(), bsXMLHeader.GetLength());
    if (GetPacketID() == XFA_XDPPACKET_Form)
      XFA_DataExporter_RegenerateFormFile(this, pStream.get(), nullptr, TRUE);
    else
      pElement->SaveXMLNode(pStream.get());
    // TODO(weili): Check whether we need to save pretty print XML, pdfium:501.
    // For now, just put it here to avoid unused variable warning.
    (void)bPrettyMode;
    pArguments->GetReturnValue()->SetString(
        CFX_ByteStringC(pMemoryStream->GetBuffer(), pMemoryStream->GetSize()));
    return;
  }
  pArguments->GetReturnValue()->SetString("");
}

void CXFA_Node::Script_NodeClass_SetAttribute(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"setAttribute");
    return;
  }
  CFX_WideString wsAttributeValue =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(0).AsStringC());
  CFX_WideString wsAttribute =
      CFX_WideString::FromUTF8(pArguments->GetUTF8String(1).AsStringC());
  SetAttribute(wsAttribute.AsStringC(), wsAttributeValue.AsStringC(), true);
}

void CXFA_Node::Script_NodeClass_SetElement(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1 && iLength != 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"setElement");
    return;
  }
  CXFA_Node* pNode = nullptr;
  CFX_WideString wsName;
  pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
  if (iLength == 2)
    wsName = CFX_WideString::FromUTF8(pArguments->GetUTF8String(1).AsStringC());
  // TODO(weili): check whether we need to implement this, pdfium:501.
  // For now, just put the variables here to avoid unused variable warning.
  (void)pNode;
  (void)wsName;
}

void CXFA_Node::Script_NodeClass_Ns(CFXJSE_Value* pValue,
                                    FX_BOOL bSetting,
                                    XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    CFX_WideString wsNameSpace;
    TryNamespace(wsNameSpace);
    pValue->SetString(FX_UTF8Encode(wsNameSpace).AsStringC());
  }
}

void CXFA_Node::Script_NodeClass_Model(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    pValue->Assign(
        m_pDocument->GetScriptContext()->GetJSValueFromMap(GetModelNode()));
  }
}

void CXFA_Node::Script_NodeClass_IsContainer(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  if (bSetting)
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  else
    pValue->SetBoolean(IsContainerNode());
}

void CXFA_Node::Script_NodeClass_IsNull(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    if (GetElementType() == XFA_Element::Subform) {
      pValue->SetBoolean(FALSE);
      return;
    }
    CFX_WideString strValue;
    pValue->SetBoolean(!TryContent(strValue) || strValue.IsEmpty());
  }
}

void CXFA_Node::Script_NodeClass_OneOfChild(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    CXFA_NodeArray properts;
    int32_t iSize = GetNodeList(properts, XFA_NODEFILTER_OneOfProperty);
    if (iSize > 0) {
      pValue->Assign(
          m_pDocument->GetScriptContext()->GetJSValueFromMap(properts[0]));
    }
  }
}

void CXFA_Node::Script_ContainerClass_GetDelta(CFXJSE_Arguments* pArguments) {}

void CXFA_Node::Script_ContainerClass_GetDeltas(CFXJSE_Arguments* pArguments) {
  CXFA_ArrayNodeList* pFormNodes = new CXFA_ArrayNodeList(m_pDocument);
  pArguments->GetReturnValue()->SetObject(
      pFormNodes, m_pDocument->GetScriptContext()->GetJseNormalClass());
}
void CXFA_Node::Script_ModelClass_ClearErrorList(CFXJSE_Arguments* pArguments) {
}
void CXFA_Node::Script_ModelClass_CreateNode(CFXJSE_Arguments* pArguments) {
  Script_Template_CreateNode(pArguments);
}
void CXFA_Node::Script_ModelClass_IsCompatibleNS(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"isCompatibleNS");
    return;
  }
  CFX_WideString wsNameSpace;
  if (iLength >= 1) {
    CFX_ByteString bsNameSpace = pArguments->GetUTF8String(0);
    wsNameSpace = CFX_WideString::FromUTF8(bsNameSpace.AsStringC());
  }
  CFX_WideString wsNodeNameSpace;
  TryNamespace(wsNodeNameSpace);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(wsNodeNameSpace == wsNameSpace);
}
void CXFA_Node::Script_ModelClass_Context(CFXJSE_Value* pValue,
                                          FX_BOOL bSetting,
                                          XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_ModelClass_AliasNode(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Attribute_Integer(CFXJSE_Value* pValue,
                                         FX_BOOL bSetting,
                                         XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    SetInteger(eAttribute, pValue->ToInteger(), true);
  } else {
    pValue->SetInteger(GetInteger(eAttribute));
  }
}
void CXFA_Node::Script_Attribute_IntegerRead(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    pValue->SetInteger(GetInteger(eAttribute));
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
void CXFA_Node::Script_Attribute_BOOL(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    SetBoolean(eAttribute, pValue->ToBoolean(), true);
  } else {
    pValue->SetString(GetBoolean(eAttribute) ? "1" : "0");
  }
}
void CXFA_Node::Script_Attribute_BOOLRead(CFXJSE_Value* pValue,
                                          FX_BOOL bSetting,
                                          XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    pValue->SetString(GetBoolean(eAttribute) ? "1" : "0");
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}

void CXFA_Node::Script_Attribute_SendAttributeChangeMessage(
    XFA_ATTRIBUTE eAttribute,
    FX_BOOL bScriptModify) {
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (!pLayoutPro)
    return;

  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify)
    return;

  uint32_t dwPacket = GetPacketID();
  if (!(dwPacket & XFA_XDPPACKET_Form)) {
    pNotify->OnValueChanged(this, eAttribute, this, this);
    return;
  }

  bool bNeedFindContainer = false;
  switch (GetElementType()) {
    case XFA_Element::Caption:
      bNeedFindContainer = true;
      pNotify->OnValueChanged(this, eAttribute, this,
                              GetNodeItem(XFA_NODEITEM_Parent));
      break;
    case XFA_Element::Font:
    case XFA_Element::Para: {
      bNeedFindContainer = true;
      CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
      if (pParentNode->GetElementType() == XFA_Element::Caption) {
        pNotify->OnValueChanged(this, eAttribute, pParentNode,
                                pParentNode->GetNodeItem(XFA_NODEITEM_Parent));
      } else {
        pNotify->OnValueChanged(this, eAttribute, this, pParentNode);
      }
    } break;
    case XFA_Element::Margin: {
      bNeedFindContainer = true;
      CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
      XFA_Element eParentType = pParentNode->GetElementType();
      if (pParentNode->IsContainerNode()) {
        pNotify->OnValueChanged(this, eAttribute, this, pParentNode);
      } else if (eParentType == XFA_Element::Caption) {
        pNotify->OnValueChanged(this, eAttribute, pParentNode,
                                pParentNode->GetNodeItem(XFA_NODEITEM_Parent));
      } else {
        CXFA_Node* pNode = pParentNode->GetNodeItem(XFA_NODEITEM_Parent);
        if (pNode && pNode->GetElementType() == XFA_Element::Ui) {
          pNotify->OnValueChanged(this, eAttribute, pNode,
                                  pNode->GetNodeItem(XFA_NODEITEM_Parent));
        }
      }
    } break;
    case XFA_Element::Comb: {
      CXFA_Node* pEditNode = GetNodeItem(XFA_NODEITEM_Parent);
      XFA_Element eUIType = pEditNode->GetElementType();
      if (pEditNode && (eUIType == XFA_Element::DateTimeEdit ||
                        eUIType == XFA_Element::NumericEdit ||
                        eUIType == XFA_Element::TextEdit)) {
        CXFA_Node* pUINode = pEditNode->GetNodeItem(XFA_NODEITEM_Parent);
        if (pUINode) {
          pNotify->OnValueChanged(this, eAttribute, pUINode,
                                  pUINode->GetNodeItem(XFA_NODEITEM_Parent));
        }
      }
    } break;
    case XFA_Element::Button:
    case XFA_Element::Barcode:
    case XFA_Element::ChoiceList:
    case XFA_Element::DateTimeEdit:
    case XFA_Element::NumericEdit:
    case XFA_Element::PasswordEdit:
    case XFA_Element::TextEdit: {
      CXFA_Node* pUINode = GetNodeItem(XFA_NODEITEM_Parent);
      if (pUINode) {
        pNotify->OnValueChanged(this, eAttribute, pUINode,
                                pUINode->GetNodeItem(XFA_NODEITEM_Parent));
      }
    } break;
    case XFA_Element::CheckButton: {
      bNeedFindContainer = true;
      CXFA_Node* pUINode = GetNodeItem(XFA_NODEITEM_Parent);
      if (pUINode) {
        pNotify->OnValueChanged(this, eAttribute, pUINode,
                                pUINode->GetNodeItem(XFA_NODEITEM_Parent));
      }
    } break;
    case XFA_Element::Keep:
    case XFA_Element::Bookend:
    case XFA_Element::Break:
    case XFA_Element::BreakAfter:
    case XFA_Element::BreakBefore:
    case XFA_Element::Overflow:
      bNeedFindContainer = true;
      break;
    case XFA_Element::Area:
    case XFA_Element::Draw:
    case XFA_Element::ExclGroup:
    case XFA_Element::Field:
    case XFA_Element::Subform:
    case XFA_Element::SubformSet:
      pLayoutPro->AddChangedContainer(this);
      pNotify->OnValueChanged(this, eAttribute, this, this);
      break;
    case XFA_Element::Sharptext:
    case XFA_Element::Sharpxml:
    case XFA_Element::SharpxHTML: {
      CXFA_Node* pTextNode = GetNodeItem(XFA_NODEITEM_Parent);
      if (!pTextNode) {
        return;
      }
      CXFA_Node* pValueNode = pTextNode->GetNodeItem(XFA_NODEITEM_Parent);
      if (!pValueNode) {
        return;
      }
      XFA_Element eType = pValueNode->GetElementType();
      if (eType == XFA_Element::Value) {
        bNeedFindContainer = true;
        CXFA_Node* pNode = pValueNode->GetNodeItem(XFA_NODEITEM_Parent);
        if (pNode && pNode->IsContainerNode()) {
          if (bScriptModify) {
            pValueNode = pNode;
          }
          pNotify->OnValueChanged(this, eAttribute, pValueNode, pNode);
        } else {
          pNotify->OnValueChanged(this, eAttribute, pNode,
                                  pNode->GetNodeItem(XFA_NODEITEM_Parent));
        }
      } else {
        if (eType == XFA_Element::Items) {
          CXFA_Node* pNode = pValueNode->GetNodeItem(XFA_NODEITEM_Parent);
          if (pNode && pNode->IsContainerNode()) {
            pNotify->OnValueChanged(this, eAttribute, pValueNode, pNode);
          }
        }
      }
    } break;
    default:
      break;
  }
  if (bNeedFindContainer) {
    CXFA_Node* pParent = this;
    while (pParent) {
      if (pParent->IsContainerNode())
        break;

      pParent = pParent->GetNodeItem(XFA_NODEITEM_Parent);
    }
    if (pParent) {
      pLayoutPro->AddChangedContainer(pParent);
    }
  }
}

void CXFA_Node::Script_Attribute_String(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    CFX_WideString wsValue = pValue->ToWideString();
    SetAttribute(eAttribute, wsValue.AsStringC(), true);
    if (eAttribute == XFA_ATTRIBUTE_Use &&
        GetElementType() == XFA_Element::Desc) {
      CXFA_Node* pTemplateNode =
          ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Template));
      CXFA_Node* pProtoRoot =
          pTemplateNode->GetFirstChildByClass(XFA_Element::Subform)
              ->GetFirstChildByClass(XFA_Element::Proto);

      CFX_WideString wsID;
      CFX_WideString wsSOM;
      if (!wsValue.IsEmpty()) {
        if (wsValue[0] == '#') {
          wsID = CFX_WideString(wsValue.c_str() + 1, wsValue.GetLength() - 1);
        } else {
          wsSOM = wsValue;
        }
      }
      CXFA_Node* pProtoNode = nullptr;
      if (!wsSOM.IsEmpty()) {
        uint32_t dwFlag = XFA_RESOLVENODE_Children |
                          XFA_RESOLVENODE_Attributes |
                          XFA_RESOLVENODE_Properties | XFA_RESOLVENODE_Parent |
                          XFA_RESOLVENODE_Siblings;
        XFA_RESOLVENODE_RS resoveNodeRS;
        int32_t iRet = m_pDocument->GetScriptContext()->ResolveObjects(
            pProtoRoot, wsSOM.AsStringC(), resoveNodeRS, dwFlag);
        if (iRet > 0 && resoveNodeRS.nodes[0]->IsNode()) {
          pProtoNode = resoveNodeRS.nodes[0]->AsNode();
        }
      } else if (!wsID.IsEmpty()) {
        pProtoNode = m_pDocument->GetNodeByID(pProtoRoot, wsID.AsStringC());
      }
      if (pProtoNode) {
        CXFA_Node* pHeadChild = GetNodeItem(XFA_NODEITEM_FirstChild);
        while (pHeadChild) {
          CXFA_Node* pSibling =
              pHeadChild->GetNodeItem(XFA_NODEITEM_NextSibling);
          RemoveChild(pHeadChild);
          pHeadChild = pSibling;
        }
        CXFA_Node* pProtoForm = pProtoNode->CloneTemplateToForm(TRUE);
        pHeadChild = pProtoForm->GetNodeItem(XFA_NODEITEM_FirstChild);
        while (pHeadChild) {
          CXFA_Node* pSibling =
              pHeadChild->GetNodeItem(XFA_NODEITEM_NextSibling);
          pProtoForm->RemoveChild(pHeadChild);
          InsertChild(pHeadChild);
          pHeadChild = pSibling;
        }
        m_pDocument->RemovePurgeNode(pProtoForm);
        delete pProtoForm;
      }
    }
  } else {
    CFX_WideString wsValue;
    GetAttribute(eAttribute, wsValue);
    pValue->SetString(
        FX_UTF8Encode(wsValue.c_str(), wsValue.GetLength()).AsStringC());
  }
}
void CXFA_Node::Script_Attribute_StringRead(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    CFX_WideString wsValue;
    GetAttribute(eAttribute, wsValue);
    pValue->SetString(
        FX_UTF8Encode(wsValue.c_str(), wsValue.GetLength()).AsStringC());
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
void CXFA_Node::Script_WsdlConnection_Execute(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if ((argc == 0) || (argc == 1)) {
    pArguments->GetReturnValue()->SetBoolean(FALSE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execute");
  }
}
void CXFA_Node::Script_Delta_Restore(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"restore");
  }
}
void CXFA_Node::Script_Delta_CurrentValue(CFXJSE_Value* pValue,
                                          FX_BOOL bSetting,
                                          XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Delta_SavedValue(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Delta_Target(CFXJSE_Value* pValue,
                                    FX_BOOL bSetting,
                                    XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Som_Message(CFXJSE_Value* pValue,
                                   FX_BOOL bSetting,
                                   XFA_SOM_MESSAGETYPE iMessageType) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  FX_BOOL bNew = FALSE;
  CXFA_Validate validate = pWidgetData->GetValidate();
  if (!validate) {
    validate = pWidgetData->GetValidate(TRUE);
    bNew = TRUE;
  }
  if (bSetting) {
    switch (iMessageType) {
      case XFA_SOM_ValidationMessage:
        validate.SetScriptMessageText(pValue->ToWideString());
        break;
      case XFA_SOM_FormatMessage:
        validate.SetFormatMessageText(pValue->ToWideString());
        break;
      case XFA_SOM_MandatoryMessage:
        validate.SetNullMessageText(pValue->ToWideString());
        break;
      default:
        break;
    }
    if (!bNew) {
      CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
      if (!pNotify) {
        return;
      }
      pNotify->AddCalcValidate(this);
    }
  } else {
    CFX_WideString wsMessage;
    switch (iMessageType) {
      case XFA_SOM_ValidationMessage:
        validate.GetScriptMessageText(wsMessage);
        break;
      case XFA_SOM_FormatMessage:
        validate.GetFormatMessageText(wsMessage);
        break;
      case XFA_SOM_MandatoryMessage:
        validate.GetNullMessageText(wsMessage);
        break;
      default:
        break;
    }
    pValue->SetString(FX_UTF8Encode(wsMessage).AsStringC());
  }
}
void CXFA_Node::Script_Som_ValidationMessage(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  Script_Som_Message(pValue, bSetting, XFA_SOM_ValidationMessage);
}
void CXFA_Node::Script_Field_Length(CFXJSE_Value* pValue,
                                    FX_BOOL bSetting,
                                    XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    CXFA_WidgetData* pWidgetData = GetWidgetData();
    if (!pWidgetData) {
      pValue->SetInteger(0);
      return;
    }
    pValue->SetInteger(pWidgetData->CountChoiceListItems(TRUE));
  }
}
void CXFA_Node::Script_Som_DefaultValue(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  XFA_Element eType = GetElementType();
  if (eType == XFA_Element::Field) {
    Script_Field_DefaultValue(pValue, bSetting, eAttribute);
    return;
  }
  if (eType == XFA_Element::Draw) {
    Script_Draw_DefaultValue(pValue, bSetting, eAttribute);
    return;
  }
  if (eType == XFA_Element::Boolean) {
    Script_Boolean_Value(pValue, bSetting, eAttribute);
    return;
  }
  if (bSetting) {
    CFX_WideString wsNewValue;
    if (!(pValue && (pValue->IsNull() || pValue->IsUndefined())))
      wsNewValue = pValue->ToWideString();

    CFX_WideString wsFormatValue(wsNewValue);
    CXFA_WidgetData* pContainerWidgetData = nullptr;
    if (GetPacketID() == XFA_XDPPACKET_Datasets) {
      CXFA_NodeArray formNodes;
      GetBindItems(formNodes);
      CFX_WideString wsPicture;
      for (int32_t i = 0; i < formNodes.GetSize(); i++) {
        CXFA_Node* pFormNode = formNodes.GetAt(i);
        if (!pFormNode || pFormNode->HasRemovedChildren()) {
          continue;
        }
        pContainerWidgetData = pFormNode->GetContainerWidgetData();
        if (pContainerWidgetData) {
          pContainerWidgetData->GetPictureContent(wsPicture,
                                                  XFA_VALUEPICTURE_DataBind);
        }
        if (!wsPicture.IsEmpty()) {
          break;
        }
        pContainerWidgetData = nullptr;
      }
    } else if (GetPacketID() == XFA_XDPPACKET_Form) {
      pContainerWidgetData = GetContainerWidgetData();
    }
    if (pContainerWidgetData) {
      pContainerWidgetData->GetFormatDataValue(wsNewValue, wsFormatValue);
    }
    SetScriptContent(wsNewValue, wsFormatValue, true, TRUE);
  } else {
    CFX_WideString content = GetScriptContent(TRUE);
    if (content.IsEmpty() && eType != XFA_Element::Text &&
        eType != XFA_Element::SubmitUrl) {
      pValue->SetNull();
    } else if (eType == XFA_Element::Integer) {
      pValue->SetInteger(FXSYS_wtoi(content.c_str()));
    } else if (eType == XFA_Element::Float || eType == XFA_Element::Decimal) {
      CFX_Decimal decimal(content.AsStringC());
      pValue->SetFloat((FX_FLOAT)(double)decimal);
    } else {
      pValue->SetString(
          FX_UTF8Encode(content.c_str(), content.GetLength()).AsStringC());
    }
  }
}
void CXFA_Node::Script_Som_DefaultValue_Read(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
    return;
  }
  CFX_WideString content = GetScriptContent(TRUE);
  if (content.IsEmpty()) {
    pValue->SetNull();
  } else {
    pValue->SetString(
        FX_UTF8Encode(content.c_str(), content.GetLength()).AsStringC());
  }
}
void CXFA_Node::Script_Boolean_Value(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    CFX_ByteString newValue;
    if (!(pValue && (pValue->IsNull() || pValue->IsUndefined())))
      newValue = pValue->ToString();

    int32_t iValue = FXSYS_atoi(newValue.c_str());
    CFX_WideString wsNewValue(iValue == 0 ? L"0" : L"1");
    CFX_WideString wsFormatValue(wsNewValue);
    CXFA_WidgetData* pContainerWidgetData = GetContainerWidgetData();
    if (pContainerWidgetData) {
      pContainerWidgetData->GetFormatDataValue(wsNewValue, wsFormatValue);
    }
    SetScriptContent(wsNewValue, wsFormatValue, true, TRUE);
  } else {
    CFX_WideString wsValue = GetScriptContent(TRUE);
    pValue->SetBoolean(wsValue == FX_WSTRC(L"1"));
  }
}
struct XFA_ExecEventParaInfo {
 public:
  uint32_t m_uHash;
  const FX_WCHAR* m_lpcEventName;
  XFA_EVENTTYPE m_eventType;
  uint32_t m_validFlags;
};
static const XFA_ExecEventParaInfo gs_eventParaInfos[] = {
    {0x02a6c55a, L"postSubmit", XFA_EVENT_PostSubmit, 0},
    {0x0ab466bb, L"preSubmit", XFA_EVENT_PreSubmit, 0},
    {0x109d7ce7, L"mouseEnter", XFA_EVENT_MouseEnter, 5},
    {0x17fad373, L"postPrint", XFA_EVENT_PostPrint, 0},
    {0x1bfc72d9, L"preOpen", XFA_EVENT_PreOpen, 7},
    {0x2196a452, L"initialize", XFA_EVENT_Initialize, 1},
    {0x27410f03, L"mouseExit", XFA_EVENT_MouseExit, 5},
    {0x33c43dec, L"docClose", XFA_EVENT_DocClose, 0},
    {0x361fa1b6, L"preSave", XFA_EVENT_PreSave, 0},
    {0x36f1c6d8, L"preSign", XFA_EVENT_PreSign, 6},
    {0x4731d6ba, L"exit", XFA_EVENT_Exit, 2},
    {0x56bf456b, L"docReady", XFA_EVENT_DocReady, 0},
    {0x7233018a, L"validate", XFA_EVENT_Validate, 1},
    {0x8808385e, L"indexChange", XFA_EVENT_IndexChange, 3},
    {0x891f4606, L"change", XFA_EVENT_Change, 4},
    {0x9528a7b4, L"prePrint", XFA_EVENT_PrePrint, 0},
    {0x9f693b21, L"mouseDown", XFA_EVENT_MouseDown, 5},
    {0xcdce56b3, L"full", XFA_EVENT_Full, 4},
    {0xd576d08e, L"mouseUp", XFA_EVENT_MouseUp, 5},
    {0xd95657a6, L"click", XFA_EVENT_Click, 4},
    {0xdbfbe02e, L"calculate", XFA_EVENT_Calculate, 1},
    {0xe25fa7b8, L"postOpen", XFA_EVENT_PostOpen, 7},
    {0xe28dce7e, L"enter", XFA_EVENT_Enter, 2},
    {0xfc82d695, L"postSave", XFA_EVENT_PostSave, 0},
    {0xfd54fbb7, L"postSign", XFA_EVENT_PostSign, 6},
};
const XFA_ExecEventParaInfo* GetEventParaInfoByName(
    const CFX_WideStringC& wsEventName) {
  uint32_t uHash = FX_HashCode_GetW(wsEventName, false);
  int32_t iStart = 0;
  int32_t iEnd = (sizeof(gs_eventParaInfos) / sizeof(gs_eventParaInfos[0])) - 1;
  do {
    int32_t iMid = (iStart + iEnd) / 2;
    const XFA_ExecEventParaInfo* eventParaInfo = &gs_eventParaInfos[iMid];
    if (uHash == eventParaInfo->m_uHash) {
      return eventParaInfo;
    }
    if (uHash < eventParaInfo->m_uHash) {
      iEnd = iMid - 1;
    } else {
      iStart = iMid + 1;
    }
  } while (iStart <= iEnd);
  return nullptr;
}
void XFA_STRING_TO_RGB(const CFX_WideString& strRGB,
                       int32_t& r,
                       int32_t& g,
                       int32_t& b) {
  r = 0;
  g = 0;
  b = 0;

  FX_WCHAR zero = '0';
  int32_t iIndex = 0;
  int32_t iLen = strRGB.GetLength();
  for (int32_t i = 0; i < iLen; ++i) {
    FX_WCHAR ch = strRGB.GetAt(i);
    if (ch == L',') {
      ++iIndex;
    }
    if (iIndex > 2) {
      break;
    }
    int32_t iValue = ch - zero;
    if (iValue >= 0 && iValue <= 9) {
      switch (iIndex) {
        case 0:
          r = r * 10 + iValue;
          break;
        case 1:
          g = g * 10 + iValue;
          break;
        default:
          b = b * 10 + iValue;
          break;
      }
    }
  }
}
void CXFA_Node::Script_Som_BorderColor(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CXFA_Border border = pWidgetData->GetBorder(TRUE);
  int32_t iSize = border.CountEdges();
  if (bSetting) {
    int32_t r = 0;
    int32_t g = 0;
    int32_t b = 0;
    XFA_STRING_TO_RGB(pValue->ToWideString(), r, g, b);
    FX_ARGB rgb = ArgbEncode(100, r, g, b);
    for (int32_t i = 0; i < iSize; ++i) {
      CXFA_Edge edge = border.GetEdge(i);
      edge.SetColor(rgb);
    }
  } else {
    CXFA_Edge edge = border.GetEdge(0);
    FX_ARGB color = edge.GetColor();
    int32_t a, r, g, b;
    ArgbDecode(color, a, r, g, b);
    CFX_WideString strColor;
    strColor.Format(L"%d,%d,%d", r, g, b);
    pValue->SetString(FX_UTF8Encode(strColor).AsStringC());
  }
}
void CXFA_Node::Script_Som_BorderWidth(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CXFA_Border border = pWidgetData->GetBorder(TRUE);
  int32_t iSize = border.CountEdges();
  CFX_WideString wsThickness;
  if (bSetting) {
    wsThickness = pValue->ToWideString();
    for (int32_t i = 0; i < iSize; ++i) {
      CXFA_Edge edge = border.GetEdge(i);
      CXFA_Measurement thickness(wsThickness.AsStringC());
      edge.SetMSThickness(thickness);
    }
  } else {
    CXFA_Edge edge = border.GetEdge(0);
    CXFA_Measurement thickness = edge.GetMSThickness();
    thickness.ToString(wsThickness);
    pValue->SetString(FX_UTF8Encode(wsThickness).AsStringC());
  }
}
void CXFA_Node::Script_Som_FillColor(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CXFA_Border border = pWidgetData->GetBorder(TRUE);
  CXFA_Fill borderfill = border.GetFill(TRUE);
  CXFA_Node* pNode = borderfill.GetNode();
  if (!pNode) {
    return;
  }
  if (bSetting) {
    int32_t r;
    int32_t g;
    int32_t b;
    XFA_STRING_TO_RGB(pValue->ToWideString(), r, g, b);
    FX_ARGB color = ArgbEncode(0xff, r, g, b);
    borderfill.SetColor(color);
  } else {
    FX_ARGB color = borderfill.GetColor();
    int32_t a;
    int32_t r;
    int32_t g;
    int32_t b;
    ArgbDecode(color, a, r, g, b);
    CFX_WideString wsColor;
    wsColor.Format(L"%d,%d,%d", r, g, b);
    pValue->SetString(FX_UTF8Encode(wsColor).AsStringC());
  }
}
void CXFA_Node::Script_Som_DataNode(CFXJSE_Value* pValue,
                                    FX_BOOL bSetting,
                                    XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    CXFA_Node* pDataNode = GetBindData();
    if (pDataNode) {
      pValue->Assign(
          m_pDocument->GetScriptContext()->GetJSValueFromMap(pDataNode));
    } else {
      pValue->SetNull();
    }
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
void CXFA_Node::Script_Draw_DefaultValue(CFXJSE_Value* pValue,
                                         FX_BOOL bSetting,
                                         XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    if (pValue && pValue->IsString()) {
      CXFA_WidgetData* pWidgetData = GetWidgetData();
      ASSERT(pWidgetData);
      XFA_Element uiType = pWidgetData->GetUIType();
      if (uiType == XFA_Element::Text) {
        CFX_WideString wsNewValue = pValue->ToWideString();
        CFX_WideString wsFormatValue(wsNewValue);
        SetScriptContent(wsNewValue, wsFormatValue, true, TRUE);
      }
    }
  } else {
    CFX_WideString content = GetScriptContent(TRUE);
    if (content.IsEmpty()) {
      pValue->SetNull();
    } else {
      pValue->SetString(
          FX_UTF8Encode(content.c_str(), content.GetLength()).AsStringC());
    }
  }
}
void CXFA_Node::Script_Field_DefaultValue(CFXJSE_Value* pValue,
                                          FX_BOOL bSetting,
                                          XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  if (bSetting) {
    if (pValue && pValue->IsNull()) {
      pWidgetData->m_bPreNull = pWidgetData->m_bIsNull;
      pWidgetData->m_bIsNull = TRUE;
    } else {
      pWidgetData->m_bPreNull = pWidgetData->m_bIsNull;
      pWidgetData->m_bIsNull = FALSE;
    }
    CFX_WideString wsNewText;
    if (!(pValue && (pValue->IsNull() || pValue->IsUndefined())))
      wsNewText = pValue->ToWideString();

    CXFA_Node* pUIChild = pWidgetData->GetUIChild();
    if (pUIChild->GetElementType() == XFA_Element::NumericEdit) {
      int32_t iLeadDigits = 0;
      int32_t iFracDigits = 0;
      pWidgetData->GetLeadDigits(iLeadDigits);
      pWidgetData->GetFracDigits(iFracDigits);
      wsNewText =
          pWidgetData->NumericLimit(wsNewText, iLeadDigits, iFracDigits);
    }
    CXFA_WidgetData* pContainerWidgetData = GetContainerWidgetData();
    CFX_WideString wsFormatText(wsNewText);
    if (pContainerWidgetData) {
      pContainerWidgetData->GetFormatDataValue(wsNewText, wsFormatText);
    }
    SetScriptContent(wsNewText, wsFormatText, true, TRUE);
  } else {
    CFX_WideString content = GetScriptContent(TRUE);
    if (content.IsEmpty()) {
      pValue->SetNull();
    } else {
      CXFA_Node* pUIChild = pWidgetData->GetUIChild();
      CXFA_Value defVal = pWidgetData->GetFormValue();
      CXFA_Node* pNode = defVal.GetNode()->GetNodeItem(XFA_NODEITEM_FirstChild);
      if (pNode && pNode->GetElementType() == XFA_Element::Decimal) {
        if (pUIChild->GetElementType() == XFA_Element::NumericEdit &&
            (pNode->GetInteger(XFA_ATTRIBUTE_FracDigits) == -1)) {
          pValue->SetString(
              FX_UTF8Encode(content.c_str(), content.GetLength()).AsStringC());
        } else {
          CFX_Decimal decimal(content.AsStringC());
          pValue->SetFloat((FX_FLOAT)(double)decimal);
        }
      } else if (pNode && pNode->GetElementType() == XFA_Element::Integer) {
        pValue->SetInteger(FXSYS_wtoi(content.c_str()));
      } else if (pNode && pNode->GetElementType() == XFA_Element::Boolean) {
        pValue->SetBoolean(FXSYS_wtoi(content.c_str()) == 0 ? FALSE : TRUE);
      } else if (pNode && pNode->GetElementType() == XFA_Element::Float) {
        CFX_Decimal decimal(content.AsStringC());
        pValue->SetFloat((FX_FLOAT)(double)decimal);
      } else {
        pValue->SetString(
            FX_UTF8Encode(content.c_str(), content.GetLength()).AsStringC());
      }
    }
  }
}
void CXFA_Node::Script_Field_EditValue(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  if (bSetting) {
    pWidgetData->SetValue(pValue->ToWideString(), XFA_VALUEPICTURE_Edit);
  } else {
    CFX_WideString wsValue;
    pWidgetData->GetValue(wsValue, XFA_VALUEPICTURE_Edit);
    pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
  }
}
void CXFA_Node::Script_Som_FontColor(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CXFA_Font font = pWidgetData->GetFont(TRUE);
  CXFA_Node* pNode = font.GetNode();
  if (!pNode) {
    return;
  }
  if (bSetting) {
    int32_t r;
    int32_t g;
    int32_t b;
    XFA_STRING_TO_RGB(pValue->ToWideString(), r, g, b);
    FX_ARGB color = ArgbEncode(0xff, r, g, b);
    font.SetColor(color);
  } else {
    FX_ARGB color = font.GetColor();
    int32_t a;
    int32_t r;
    int32_t g;
    int32_t b;
    ArgbDecode(color, a, r, g, b);
    CFX_WideString wsColor;
    wsColor.Format(L"%d,%d,%d", r, g, b);
    pValue->SetString(FX_UTF8Encode(wsColor).AsStringC());
  }
}
void CXFA_Node::Script_Field_FormatMessage(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  Script_Som_Message(pValue, bSetting, XFA_SOM_FormatMessage);
}
void CXFA_Node::Script_Field_FormattedValue(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  if (bSetting) {
    pWidgetData->SetValue(pValue->ToWideString(), XFA_VALUEPICTURE_Display);
  } else {
    CFX_WideString wsValue;
    pWidgetData->GetValue(wsValue, XFA_VALUEPICTURE_Display);
    pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
  }
}
void CXFA_Node::Script_Som_Mandatory(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CXFA_Validate validate = pWidgetData->GetValidate(TRUE);
  if (bSetting) {
    validate.SetNullTest(pValue->ToWideString());
  } else {
    int32_t iValue = validate.GetNullTest();
    const XFA_ATTRIBUTEENUMINFO* pInfo =
        XFA_GetAttributeEnumByID((XFA_ATTRIBUTEENUM)iValue);
    CFX_WideString wsValue;
    if (pInfo)
      wsValue = pInfo->pName;
    pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
  }
}
void CXFA_Node::Script_Som_MandatoryMessage(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  Script_Som_Message(pValue, bSetting, XFA_SOM_MandatoryMessage);
}
void CXFA_Node::Script_Field_ParentSubform(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  } else {
    pValue->SetNull();
  }
}
void CXFA_Node::Script_Field_SelectedIndex(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  if (bSetting) {
    int32_t iIndex = pValue->ToInteger();
    if (iIndex == -1) {
      pWidgetData->ClearAllSelections();
      return;
    }
    pWidgetData->SetItemState(iIndex, TRUE, true, TRUE, TRUE);
  } else {
    pValue->SetInteger(pWidgetData->GetSelectedItem());
  }
}
void CXFA_Node::Script_Field_ClearItems(CFXJSE_Arguments* pArguments) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  pWidgetData->DeleteItem(-1, TRUE);
}
void CXFA_Node::Script_Field_ExecEvent(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString eventString = pArguments->GetUTF8String(0);
    int32_t iRet = execSingleEventByName(
        CFX_WideString::FromUTF8(eventString.AsStringC()).AsStringC(),
        XFA_Element::Field);
    if (eventString == "validate") {
      pArguments->GetReturnValue()->SetBoolean(
          (iRet == XFA_EVENTERROR_Error) ? FALSE : TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execEvent");
  }
}
void CXFA_Node::Script_Field_ExecInitialize(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Initialize, FALSE, FALSE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execInitialize");
  }
}
void CXFA_Node::Script_Field_DeleteItem(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"deleteItem");
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  FX_BOOL bValue = pWidgetData->DeleteItem(iIndex, TRUE, TRUE);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(bValue);
}
void CXFA_Node::Script_Field_GetSaveItem(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getSaveItem");
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  if (iIndex < 0) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  CFX_WideString wsValue;
  FX_BOOL bHasItem = pWidgetData->GetChoiceListItem(wsValue, iIndex, TRUE);
  if (bHasItem) {
    pArguments->GetReturnValue()->SetString(
        FX_UTF8Encode(wsValue.c_str(), wsValue.GetLength()).AsStringC());
  } else {
    pArguments->GetReturnValue()->SetNull();
  }
}
void CXFA_Node::Script_Field_BoundItem(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"boundItem");
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CFX_ByteString bsValue = pArguments->GetUTF8String(0);
  CFX_WideString wsValue = CFX_WideString::FromUTF8(bsValue.AsStringC());
  CFX_WideString wsBoundValue;
  pWidgetData->GetItemValue(wsValue.AsStringC(), wsBoundValue);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetString(FX_UTF8Encode(wsBoundValue).AsStringC());
}
void CXFA_Node::Script_Field_GetItemState(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getItemState");
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  FX_BOOL bValue = pWidgetData->GetItemState(iIndex);
  CFXJSE_Value* pValue = pArguments->GetReturnValue();
  if (pValue)
    pValue->SetBoolean(bValue);
}
void CXFA_Node::Script_Field_ExecCalculate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Calculate, FALSE, FALSE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execCalculate");
  }
}
void CXFA_Node::Script_Field_SetItems(CFXJSE_Arguments* pArguments) {}
void CXFA_Node::Script_Field_GetDisplayItem(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getDisplayItem");
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  if (iIndex < 0) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  CFX_WideString wsValue;
  FX_BOOL bHasItem = pWidgetData->GetChoiceListItem(wsValue, iIndex, FALSE);
  if (bHasItem) {
    pArguments->GetReturnValue()->SetString(
        FX_UTF8Encode(wsValue.c_str(), wsValue.GetLength()).AsStringC());
  } else {
    pArguments->GetReturnValue()->SetNull();
  }
}
void CXFA_Node::Script_Field_SetItemState(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength != 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"setItemState");
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData)
    return;

  int32_t iIndex = pArguments->GetInt32(0);
  if (pArguments->GetInt32(1) != 0) {
    pWidgetData->SetItemState(iIndex, TRUE, true, TRUE, TRUE);
  } else {
    if (pWidgetData->GetItemState(iIndex))
      pWidgetData->SetItemState(iIndex, FALSE, true, TRUE, TRUE);
  }
}
void CXFA_Node::Script_Field_AddItem(CFXJSE_Arguments* pArguments) {
  int32_t iLength = pArguments->GetLength();
  if (iLength < 1 || iLength > 2) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"addItem");
    return;
  }
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  CFX_WideString wsLabel;
  CFX_WideString wsValue;
  if (iLength >= 1) {
    CFX_ByteString bsLabel = pArguments->GetUTF8String(0);
    wsLabel = CFX_WideString::FromUTF8(bsLabel.AsStringC());
  }
  if (iLength >= 2) {
    CFX_ByteString bsValue = pArguments->GetUTF8String(1);
    wsValue = CFX_WideString::FromUTF8(bsValue.AsStringC());
  }
  pWidgetData->InsertItem(wsLabel, wsValue, -1, TRUE);
}
void CXFA_Node::Script_Field_ExecValidate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      int32_t iRet =
          pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Validate, FALSE, FALSE);
      pArguments->GetReturnValue()->SetBoolean(
          (iRet == XFA_EVENTERROR_Error) ? FALSE : TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execValidate");
  }
}
void CXFA_Node::Script_ExclGroup_ErrorText(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
void CXFA_Node::Script_ExclGroup_DefaultAndRawValue(CFXJSE_Value* pValue,
                                                    FX_BOOL bSetting,
                                                    XFA_ATTRIBUTE eAttribute) {
  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    return;
  }
  if (bSetting) {
    pWidgetData->SetSelectedMemberByValue(pValue->ToWideString().AsStringC(),
                                          true, TRUE, TRUE);
  } else {
    CFX_WideString wsValue = GetScriptContent(TRUE);
    XFA_VERSION curVersion = GetDocument()->GetCurVersionMode();
    if (wsValue.IsEmpty() && curVersion >= XFA_VERSION_300) {
      pValue->SetNull();
    } else {
      pValue->SetString(FX_UTF8Encode(wsValue).AsStringC());
    }
  }
}
void CXFA_Node::Script_ExclGroup_Transient(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_ExclGroup_ExecEvent(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString eventString = pArguments->GetUTF8String(0);
    execSingleEventByName(
        CFX_WideString::FromUTF8(eventString.AsStringC()).AsStringC(),
        XFA_Element::ExclGroup);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execEvent");
  }
}

void CXFA_Node::Script_ExclGroup_SelectedMember(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc < 0 || argc > 1) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"selectedMember");
    return;
  }

  CXFA_WidgetData* pWidgetData = GetWidgetData();
  if (!pWidgetData) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }

  CXFA_Node* pReturnNode = nullptr;
  if (argc == 0) {
    pReturnNode = pWidgetData->GetSelectedMember();
  } else {
    CFX_ByteString szName;
    szName = pArguments->GetUTF8String(0);
    pReturnNode = pWidgetData->SetSelectedMember(
        CFX_WideString::FromUTF8(szName.AsStringC()).AsStringC(), true);
  }
  if (!pReturnNode) {
    pArguments->GetReturnValue()->SetNull();
    return;
  }
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pReturnNode));
}

void CXFA_Node::Script_ExclGroup_ExecInitialize(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Initialize);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execInitialize");
  }
}
void CXFA_Node::Script_ExclGroup_ExecCalculate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Calculate);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execCalculate");
  }
}
void CXFA_Node::Script_ExclGroup_ExecValidate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      int32_t iRet = pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Validate);
      pArguments->GetReturnValue()->SetBoolean(
          (iRet == XFA_EVENTERROR_Error) ? FALSE : TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execValidate");
  }
}
static CXFA_Node* XFA_ScriptInstanceManager_GetItem(CXFA_Node* pInstMgrNode,
                                                    int32_t iIndex) {
  ASSERT(pInstMgrNode);
  int32_t iCount = 0;
  uint32_t dwNameHash = 0;
  for (CXFA_Node* pNode = pInstMgrNode->GetNodeItem(XFA_NODEITEM_NextSibling);
       pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    XFA_Element eCurType = pNode->GetElementType();
    if (eCurType == XFA_Element::InstanceManager) {
      break;
    }
    if ((eCurType != XFA_Element::Subform) &&
        (eCurType != XFA_Element::SubformSet)) {
      continue;
    }
    if (iCount == 0) {
      CFX_WideStringC wsName = pNode->GetCData(XFA_ATTRIBUTE_Name);
      CFX_WideStringC wsInstName = pInstMgrNode->GetCData(XFA_ATTRIBUTE_Name);
      if (wsInstName.GetLength() < 1 || wsInstName.GetAt(0) != '_' ||
          wsInstName.Mid(1) != wsName) {
        return nullptr;
      }
      dwNameHash = pNode->GetNameHash();
    }
    if (dwNameHash != pNode->GetNameHash()) {
      break;
    }
    iCount++;
    if (iCount > iIndex) {
      return pNode;
    }
  }
  return nullptr;
}
void CXFA_Node::Script_Som_InstanceIndex(CFXJSE_Value* pValue,
                                         FX_BOOL bSetting,
                                         XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    int32_t iTo = pValue->ToInteger();
    int32_t iFrom = Subform_and_SubformSet_InstanceIndex();
    CXFA_Node* pManagerNode = nullptr;
    for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_PrevSibling); pNode;
         pNode = pNode->GetNodeItem(XFA_NODEITEM_PrevSibling)) {
      if (pNode->GetElementType() == XFA_Element::InstanceManager) {
        pManagerNode = pNode;
        break;
      }
    }
    if (pManagerNode) {
      pManagerNode->InstanceManager_MoveInstance(iTo, iFrom);
      CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
      if (!pNotify) {
        return;
      }
      CXFA_Node* pToInstance =
          XFA_ScriptInstanceManager_GetItem(pManagerNode, iTo);
      if (pToInstance &&
          pToInstance->GetElementType() == XFA_Element::Subform) {
        pNotify->RunSubformIndexChange(pToInstance);
      }
      CXFA_Node* pFromInstance =
          XFA_ScriptInstanceManager_GetItem(pManagerNode, iFrom);
      if (pFromInstance &&
          pFromInstance->GetElementType() == XFA_Element::Subform) {
        pNotify->RunSubformIndexChange(pFromInstance);
      }
    }
  } else {
    pValue->SetInteger(Subform_and_SubformSet_InstanceIndex());
  }
}
void CXFA_Node::Script_Subform_InstanceManager(CFXJSE_Value* pValue,
                                               FX_BOOL bSetting,
                                               XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    CFX_WideStringC wsName = GetCData(XFA_ATTRIBUTE_Name);
    CXFA_Node* pInstanceMgr = nullptr;
    for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_PrevSibling); pNode;
         pNode = pNode->GetNodeItem(XFA_NODEITEM_PrevSibling)) {
      if (pNode->GetElementType() == XFA_Element::InstanceManager) {
        CFX_WideStringC wsInstMgrName = pNode->GetCData(XFA_ATTRIBUTE_Name);
        if (wsInstMgrName.GetLength() >= 1 && wsInstMgrName.GetAt(0) == '_' &&
            wsInstMgrName.Mid(1) == wsName) {
          pInstanceMgr = pNode;
        }
        break;
      }
    }
    if (pInstanceMgr) {
      pValue->Assign(
          m_pDocument->GetScriptContext()->GetJSValueFromMap(pInstanceMgr));
    } else {
      pValue->SetNull();
    }
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
void CXFA_Node::Script_Subform_Locale(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    SetCData(XFA_ATTRIBUTE_Locale, pValue->ToWideString(), true, TRUE);
  } else {
    CFX_WideString wsLocaleName;
    GetLocaleName(wsLocaleName);
    pValue->SetString(
        FX_UTF8Encode(wsLocaleName.c_str(), wsLocaleName.GetLength())
            .AsStringC());
  }
}
void CXFA_Node::Script_Subform_ExecEvent(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString eventString = pArguments->GetUTF8String(0);
    execSingleEventByName(
        CFX_WideString::FromUTF8(eventString.AsStringC()).AsStringC(),
        XFA_Element::Subform);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execEvent");
  }
}
void CXFA_Node::Script_Subform_ExecInitialize(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Initialize);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execInitialize");
  }
}
void CXFA_Node::Script_Subform_ExecCalculate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Calculate);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execCalculate");
  }
}
void CXFA_Node::Script_Subform_ExecValidate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      int32_t iRet = pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Validate);
      pArguments->GetReturnValue()->SetBoolean(
          (iRet == XFA_EVENTERROR_Error) ? FALSE : TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execValidate");
  }
}
void CXFA_Node::Script_Subform_GetInvalidObjects(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getInvalidObjects");
  }
}
int32_t CXFA_Node::Subform_and_SubformSet_InstanceIndex() {
  int32_t index = 0;
  for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_PrevSibling); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_PrevSibling)) {
    if ((pNode->GetElementType() == XFA_Element::Subform) ||
        (pNode->GetElementType() == XFA_Element::SubformSet)) {
      index++;
    } else {
      break;
    }
  }
  return index;
}
void CXFA_Node::Script_Template_FormNodes(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    pArguments->GetReturnValue()->SetBoolean(TRUE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"formNodes");
  }
}
void CXFA_Node::Script_Template_Remerge(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    m_pDocument->DoDataRemerge(TRUE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"remerge");
  }
}
void CXFA_Node::Script_Template_ExecInitialize(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_WidgetData* pWidgetData = GetWidgetData();
    if (!pWidgetData) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      pArguments->GetReturnValue()->SetBoolean(TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execInitialize");
  }
}
void CXFA_Node::Script_Template_CreateNode(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if ((argc > 0) && (argc < 4)) {
    CFX_WideString strTagName;
    CFX_WideString strName;
    CFX_WideString strNameSpace;
    CFX_ByteString bsTagName = pArguments->GetUTF8String(0);
    strTagName = CFX_WideString::FromUTF8(bsTagName.AsStringC());
    if (argc > 1) {
      CFX_ByteString bsName = pArguments->GetUTF8String(1);
      strName = CFX_WideString::FromUTF8(bsName.AsStringC());
      if (argc == 3) {
        CFX_ByteString bsNameSpace = pArguments->GetUTF8String(2);
        strNameSpace = CFX_WideString::FromUTF8(bsNameSpace.AsStringC());
      }
    }
    XFA_Element eType = XFA_GetElementTypeForName(strTagName.AsStringC());
    CXFA_Node* pNewNode = CreateSamePacketNode(eType);
    if (!pNewNode) {
      pArguments->GetReturnValue()->SetNull();
    } else {
      if (!strName.IsEmpty()) {
        if (XFA_GetAttributeOfElement(eType, XFA_ATTRIBUTE_Name,
                                      XFA_XDPPACKET_UNKNOWN)) {
          pNewNode->SetAttribute(XFA_ATTRIBUTE_Name, strName.AsStringC(), true);
          if (pNewNode->GetPacketID() == XFA_XDPPACKET_Datasets) {
            pNewNode->CreateXMLMappingNode();
          }
          pArguments->GetReturnValue()->Assign(
              m_pDocument->GetScriptContext()->GetJSValueFromMap(pNewNode));
        } else {
          ThrowException(XFA_IDS_NOT_HAVE_PROPERTY, strTagName.c_str(),
                         L"name");
        }
      } else {
        pArguments->GetReturnValue()->Assign(
            m_pDocument->GetScriptContext()->GetJSValueFromMap(pNewNode));
      }
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"createNode");
  }
}
void CXFA_Node::Script_Template_Recalculate(CFXJSE_Arguments* pArguments) {
  if (pArguments->GetLength() == 1) {
    pArguments->GetReturnValue()->SetBoolean(TRUE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"recalculate");
  }
}
void CXFA_Node::Script_Template_ExecCalculate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_WidgetData* pWidgetData = GetWidgetData();
    if (!pWidgetData) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      pArguments->GetReturnValue()->SetBoolean(TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execCalculate");
  }
}
void CXFA_Node::Script_Template_ExecValidate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_WidgetData* pWidgetData = GetWidgetData();
    if (!pWidgetData) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      pArguments->GetReturnValue()->SetBoolean(TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execValidate");
  }
}
void CXFA_Node::Script_Manifest_Evaluate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_WidgetData* pWidgetData = GetWidgetData();
    if (!pWidgetData) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      pArguments->GetReturnValue()->SetBoolean(TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"evaluate");
  }
}
void CXFA_Node::Script_InstanceManager_Max(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
    return;
  }
  CXFA_Occur nodeOccur(GetOccurNode());
  pValue->SetInteger(nodeOccur.GetMax());
}
void CXFA_Node::Script_InstanceManager_Min(CFXJSE_Value* pValue,
                                           FX_BOOL bSetting,
                                           XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
    return;
  }
  CXFA_Occur nodeOccur(GetOccurNode());
  pValue->SetInteger(nodeOccur.GetMin());
}
static int32_t XFA_ScriptInstanceManager_GetCount(CXFA_Node* pInstMgrNode) {
  ASSERT(pInstMgrNode);
  int32_t iCount = 0;
  uint32_t dwNameHash = 0;
  for (CXFA_Node* pNode = pInstMgrNode->GetNodeItem(XFA_NODEITEM_NextSibling);
       pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    XFA_Element eCurType = pNode->GetElementType();
    if (eCurType == XFA_Element::InstanceManager) {
      break;
    }
    if ((eCurType != XFA_Element::Subform) &&
        (eCurType != XFA_Element::SubformSet)) {
      continue;
    }
    if (iCount == 0) {
      CFX_WideStringC wsName = pNode->GetCData(XFA_ATTRIBUTE_Name);
      CFX_WideStringC wsInstName = pInstMgrNode->GetCData(XFA_ATTRIBUTE_Name);
      if (wsInstName.GetLength() < 1 || wsInstName.GetAt(0) != '_' ||
          wsInstName.Mid(1) != wsName) {
        return iCount;
      }
      dwNameHash = pNode->GetNameHash();
    }
    if (dwNameHash != pNode->GetNameHash()) {
      break;
    }
    iCount++;
  }
  return iCount;
}
static void
XFA_ScriptInstanceManager_ReorderDataNodes_SortNodeArrayByDocumentIdx(
    const CXFA_NodeSet& rgNodeSet,
    CXFA_NodeArray& rgNodeArray,
    CFX_ArrayTemplate<int32_t>& rgIdxArray) {
  int32_t iCount = pdfium::CollectionSize<int32_t>(rgNodeSet);
  rgNodeArray.SetSize(iCount);
  rgIdxArray.SetSize(iCount);
  if (iCount == 0)
    return;

  int32_t iIndex = -1;
  int32_t iTotalIndex = -1;
  CXFA_Node* pCommonParent =
      (*rgNodeSet.begin())->GetNodeItem(XFA_NODEITEM_Parent);
  for (CXFA_Node* pNode = pCommonParent->GetNodeItem(XFA_NODEITEM_FirstChild);
       pNode && iIndex < iCount;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    iTotalIndex++;
    if (pdfium::ContainsValue(rgNodeSet, pNode)) {
      iIndex++;
      rgNodeArray[iIndex] = pNode;
      rgIdxArray[iIndex] = iTotalIndex;
    }
  }
}

using CXFA_NodeSetPair = std::pair<CXFA_NodeSet, CXFA_NodeSet>;
using CXFA_NodeSetPairMap =
    std::map<uint32_t, std::unique_ptr<CXFA_NodeSetPair>>;
using CXFA_NodeSetPairMapMap =
    std::map<CXFA_Node*, std::unique_ptr<CXFA_NodeSetPairMap>>;

static CXFA_NodeSetPair* NodeSetPairForNode(CXFA_Node* pNode,
                                            CXFA_NodeSetPairMapMap* pMap) {
  CXFA_Node* pParentNode = pNode->GetNodeItem(XFA_NODEITEM_Parent);
  uint32_t dwNameHash = pNode->GetNameHash();
  if (!pParentNode || !dwNameHash)
    return nullptr;

  if (!(*pMap)[pParentNode])
    (*pMap)[pParentNode].reset(new CXFA_NodeSetPairMap);

  CXFA_NodeSetPairMap* pNodeSetPairMap = (*pMap)[pParentNode].get();
  if (!(*pNodeSetPairMap)[dwNameHash])
    (*pNodeSetPairMap)[dwNameHash].reset(new CXFA_NodeSetPair);

  return (*pNodeSetPairMap)[dwNameHash].get();
}

static void XFA_ScriptInstanceManager_ReorderDataNodes(
    const CXFA_NodeSet& sSet1,
    const CXFA_NodeSet& sSet2,
    FX_BOOL bInsertBefore) {
  CXFA_NodeSetPairMapMap rgMap;
  for (CXFA_Node* pNode : sSet1) {
    CXFA_NodeSetPair* pNodeSetPair = NodeSetPairForNode(pNode, &rgMap);
    if (pNodeSetPair)
      pNodeSetPair->first.insert(pNode);
  }
  for (CXFA_Node* pNode : sSet2) {
    CXFA_NodeSetPair* pNodeSetPair = NodeSetPairForNode(pNode, &rgMap);
    if (pNodeSetPair) {
      if (pdfium::ContainsValue(pNodeSetPair->first, pNode))
        pNodeSetPair->first.erase(pNode);
      else
        pNodeSetPair->second.insert(pNode);
    }
  }
  for (const auto& iter1 : rgMap) {
    CXFA_NodeSetPairMap* pNodeSetPairMap = iter1.second.get();
    if (!pNodeSetPairMap) {
      continue;
    }
    for (const auto& iter2 : *pNodeSetPairMap) {
      CXFA_NodeSetPair* pNodeSetPair = iter2.second.get();
      if (!pNodeSetPair) {
        continue;
      }
      if (!pNodeSetPair->first.empty() && !pNodeSetPair->second.empty()) {
        CXFA_NodeArray rgNodeArray1;
        CXFA_NodeArray rgNodeArray2;
        CFX_ArrayTemplate<int32_t> rgIdxArray1;
        CFX_ArrayTemplate<int32_t> rgIdxArray2;
        XFA_ScriptInstanceManager_ReorderDataNodes_SortNodeArrayByDocumentIdx(
            pNodeSetPair->first, rgNodeArray1, rgIdxArray1);
        XFA_ScriptInstanceManager_ReorderDataNodes_SortNodeArrayByDocumentIdx(
            pNodeSetPair->second, rgNodeArray2, rgIdxArray2);
        CXFA_Node* pParentNode = nullptr;
        CXFA_Node* pBeforeNode = nullptr;
        if (bInsertBefore) {
          pBeforeNode = rgNodeArray2[0];
          pParentNode = pBeforeNode->GetNodeItem(XFA_NODEITEM_Parent);
        } else {
          CXFA_Node* pLastNode = rgNodeArray2[rgIdxArray2.GetSize() - 1];
          pParentNode = pLastNode->GetNodeItem(XFA_NODEITEM_Parent);
          pBeforeNode = pLastNode->GetNodeItem(XFA_NODEITEM_NextSibling);
        }
        for (int32_t iIdx = 0; iIdx < rgIdxArray1.GetSize(); iIdx++) {
          CXFA_Node* pCurNode = rgNodeArray1[iIdx];
          pParentNode->RemoveChild(pCurNode);
          pParentNode->InsertChild(pCurNode, pBeforeNode);
        }
      }
    }
    pNodeSetPairMap->clear();
  }
}

static void XFA_ScriptInstanceManager_InsertItem(
    CXFA_Node* pInstMgrNode,
    CXFA_Node* pNewInstance,
    int32_t iPos,
    int32_t iCount = -1,
    FX_BOOL bMoveDataBindingNodes = TRUE) {
  if (iCount < 0) {
    iCount = XFA_ScriptInstanceManager_GetCount(pInstMgrNode);
  }
  if (iPos < 0) {
    iPos = iCount;
  }
  if (iPos == iCount) {
    CXFA_Node* pNextSibling =
        iCount > 0
            ? XFA_ScriptInstanceManager_GetItem(pInstMgrNode, iCount - 1)
                  ->GetNodeItem(XFA_NODEITEM_NextSibling)
            : pInstMgrNode->GetNodeItem(XFA_NODEITEM_NextSibling);
    pInstMgrNode->GetNodeItem(XFA_NODEITEM_Parent)
        ->InsertChild(pNewInstance, pNextSibling);
    if (bMoveDataBindingNodes) {
      CXFA_NodeSet sNew;
      CXFA_NodeSet sAfter;
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorNew(pNewInstance);
      for (CXFA_Node* pNode = sIteratorNew.GetCurrent(); pNode;
           pNode = sIteratorNew.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode) {
          continue;
        }
        sNew.insert(pDataNode);
      }
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorAfter(pNextSibling);
      for (CXFA_Node* pNode = sIteratorAfter.GetCurrent(); pNode;
           pNode = sIteratorAfter.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode) {
          continue;
        }
        sAfter.insert(pDataNode);
      }
      XFA_ScriptInstanceManager_ReorderDataNodes(sNew, sAfter, FALSE);
    }
  } else {
    CXFA_Node* pBeforeInstance =
        XFA_ScriptInstanceManager_GetItem(pInstMgrNode, iPos);
    pInstMgrNode->GetNodeItem(XFA_NODEITEM_Parent)
        ->InsertChild(pNewInstance, pBeforeInstance);
    if (bMoveDataBindingNodes) {
      CXFA_NodeSet sNew;
      CXFA_NodeSet sBefore;
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorNew(pNewInstance);
      for (CXFA_Node* pNode = sIteratorNew.GetCurrent(); pNode;
           pNode = sIteratorNew.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode) {
          continue;
        }
        sNew.insert(pDataNode);
      }
      CXFA_NodeIteratorTemplate<CXFA_Node,
                                CXFA_TraverseStrategy_XFAContainerNode>
          sIteratorBefore(pBeforeInstance);
      for (CXFA_Node* pNode = sIteratorBefore.GetCurrent(); pNode;
           pNode = sIteratorBefore.MoveToNext()) {
        CXFA_Node* pDataNode = pNode->GetBindData();
        if (!pDataNode) {
          continue;
        }
        sBefore.insert(pDataNode);
      }
      XFA_ScriptInstanceManager_ReorderDataNodes(sNew, sBefore, TRUE);
    }
  }
}
static void XFA_ScriptInstanceManager_RemoveItem(
    CXFA_Node* pInstMgrNode,
    CXFA_Node* pRemoveInstance,
    FX_BOOL bRemoveDataBinding = TRUE) {
  pInstMgrNode->GetNodeItem(XFA_NODEITEM_Parent)->RemoveChild(pRemoveInstance);
  if (!bRemoveDataBinding) {
    return;
  }
  CXFA_NodeIteratorTemplate<CXFA_Node, CXFA_TraverseStrategy_XFAContainerNode>
      sIterator(pRemoveInstance);
  for (CXFA_Node* pFormNode = sIterator.GetCurrent(); pFormNode;
       pFormNode = sIterator.MoveToNext()) {
    CXFA_Node* pDataNode = pFormNode->GetBindData();
    if (!pDataNode) {
      continue;
    }
    if (pDataNode->RemoveBindItem(pFormNode) == 0) {
      if (CXFA_Node* pDataParent =
              pDataNode->GetNodeItem(XFA_NODEITEM_Parent)) {
        pDataParent->RemoveChild(pDataNode);
      }
    }
    pFormNode->SetObject(XFA_ATTRIBUTE_BindingNode, nullptr);
  }
}
static CXFA_Node* XFA_ScriptInstanceManager_CreateInstance(
    CXFA_Node* pInstMgrNode,
    FX_BOOL bDataMerge) {
  CXFA_Document* pDocument = pInstMgrNode->GetDocument();
  CXFA_Node* pTemplateNode = pInstMgrNode->GetTemplateNode();
  CXFA_Node* pFormParent = pInstMgrNode->GetNodeItem(XFA_NODEITEM_Parent);
  CXFA_Node* pDataScope = nullptr;
  for (CXFA_Node* pRootBoundNode = pFormParent;
       pRootBoundNode && pRootBoundNode->IsContainerNode();
       pRootBoundNode = pRootBoundNode->GetNodeItem(XFA_NODEITEM_Parent)) {
    pDataScope = pRootBoundNode->GetBindData();
    if (pDataScope) {
      break;
    }
  }
  if (!pDataScope) {
    pDataScope = ToNode(pDocument->GetXFAObject(XFA_HASHCODE_Record));
    ASSERT(pDataScope);
  }
  CXFA_Node* pInstance = pDocument->DataMerge_CopyContainer(
      pTemplateNode, pFormParent, pDataScope, TRUE, bDataMerge);
  if (pInstance) {
    pDocument->DataMerge_UpdateBindingRelations(pInstance);
    pFormParent->RemoveChild(pInstance);
  }
  return pInstance;
}
void CXFA_Node::Script_InstanceManager_Count(CFXJSE_Value* pValue,
                                             FX_BOOL bSetting,
                                             XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    int32_t iDesired = pValue->ToInteger();
    InstanceManager_SetInstances(iDesired);
  } else {
    pValue->SetInteger(XFA_ScriptInstanceManager_GetCount(this));
  }
}
void CXFA_Node::Script_InstanceManager_MoveInstance(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc != 2) {
    pArguments->GetReturnValue()->SetUndefined();
    return;
  }
  int32_t iFrom = pArguments->GetInt32(0);
  int32_t iTo = pArguments->GetInt32(1);
  InstanceManager_MoveInstance(iTo, iFrom);
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  CXFA_Node* pToInstance = XFA_ScriptInstanceManager_GetItem(this, iTo);
  if (pToInstance && pToInstance->GetElementType() == XFA_Element::Subform) {
    pNotify->RunSubformIndexChange(pToInstance);
  }
  CXFA_Node* pFromInstance = XFA_ScriptInstanceManager_GetItem(this, iFrom);
  if (pFromInstance &&
      pFromInstance->GetElementType() == XFA_Element::Subform) {
    pNotify->RunSubformIndexChange(pFromInstance);
  }
}
void CXFA_Node::Script_InstanceManager_RemoveInstance(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc != 1) {
    pArguments->GetReturnValue()->SetUndefined();
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  int32_t iCount = XFA_ScriptInstanceManager_GetCount(this);
  if (iIndex < 0 || iIndex >= iCount) {
    ThrowException(XFA_IDS_INDEX_OUT_OF_BOUNDS);
    return;
  }
  CXFA_Occur nodeOccur(GetOccurNode());
  int32_t iMin = nodeOccur.GetMin();
  if (iCount - 1 < iMin) {
    ThrowException(XFA_IDS_VIOLATE_BOUNDARY, L"min");
    return;
  }
  CXFA_Node* pRemoveInstance = XFA_ScriptInstanceManager_GetItem(this, iIndex);
  XFA_ScriptInstanceManager_RemoveItem(this, pRemoveInstance);
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify) {
    for (int32_t i = iIndex; i < iCount - 1; i++) {
      CXFA_Node* pSubformInstance = XFA_ScriptInstanceManager_GetItem(this, i);
      if (pSubformInstance &&
          pSubformInstance->GetElementType() == XFA_Element::Subform) {
        pNotify->RunSubformIndexChange(pSubformInstance);
      }
    }
  }
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (!pLayoutPro) {
    return;
  }
  pLayoutPro->AddChangedContainer(
      ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form)));
}
void CXFA_Node::Script_InstanceManager_SetInstances(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc != 1) {
    pArguments->GetReturnValue()->SetUndefined();
    return;
  }
  int32_t iDesired = pArguments->GetInt32(0);
  InstanceManager_SetInstances(iDesired);
}
void CXFA_Node::Script_InstanceManager_AddInstance(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if ((argc != 0) && (argc != 1)) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"addInstance");
    return;
  }
  FX_BOOL fFlags = TRUE;
  if (argc == 1) {
    fFlags = pArguments->GetInt32(0) == 0 ? FALSE : TRUE;
  }
  int32_t iCount = XFA_ScriptInstanceManager_GetCount(this);
  CXFA_Occur nodeOccur(GetOccurNode());
  int32_t iMax = nodeOccur.GetMax();
  if (iMax >= 0 && iCount >= iMax) {
    ThrowException(XFA_IDS_VIOLATE_BOUNDARY, L"max");
    return;
  }
  CXFA_Node* pNewInstance =
      XFA_ScriptInstanceManager_CreateInstance(this, fFlags);
  XFA_ScriptInstanceManager_InsertItem(this, pNewInstance, iCount, iCount,
                                       FALSE);
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pNewInstance));
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  pNotify->RunNodeInitialize(pNewInstance);
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (!pLayoutPro) {
    return;
  }
  pLayoutPro->AddChangedContainer(
      ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form)));
}
void CXFA_Node::Script_InstanceManager_InsertInstance(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if ((argc != 1) && (argc != 2)) {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"insertInstance");
    return;
  }
  int32_t iIndex = pArguments->GetInt32(0);
  FX_BOOL bBind = FALSE;
  if (argc == 2) {
    bBind = pArguments->GetInt32(1) == 0 ? FALSE : TRUE;
  }
  CXFA_Occur nodeOccur(GetOccurNode());
  int32_t iCount = XFA_ScriptInstanceManager_GetCount(this);
  if (iIndex < 0 || iIndex > iCount) {
    ThrowException(XFA_IDS_INDEX_OUT_OF_BOUNDS);
    return;
  }
  int32_t iMax = nodeOccur.GetMax();
  if (iMax >= 0 && iCount >= iMax) {
    ThrowException(XFA_IDS_VIOLATE_BOUNDARY, L"max");
    return;
  }
  CXFA_Node* pNewInstance =
      XFA_ScriptInstanceManager_CreateInstance(this, bBind);
  XFA_ScriptInstanceManager_InsertItem(this, pNewInstance, iIndex, iCount,
                                       TRUE);
  pArguments->GetReturnValue()->Assign(
      m_pDocument->GetScriptContext()->GetJSValueFromMap(pNewInstance));
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (!pNotify) {
    return;
  }
  pNotify->RunNodeInitialize(pNewInstance);
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (!pLayoutPro) {
    return;
  }
  pLayoutPro->AddChangedContainer(
      ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form)));
}
int32_t CXFA_Node::InstanceManager_SetInstances(int32_t iDesired) {
  CXFA_Occur nodeOccur(GetOccurNode());
  int32_t iMax = nodeOccur.GetMax();
  int32_t iMin = nodeOccur.GetMin();
  if (iDesired < iMin) {
    ThrowException(XFA_IDS_VIOLATE_BOUNDARY, L"min");
    return 1;
  }
  if ((iMax >= 0) && (iDesired > iMax)) {
    ThrowException(XFA_IDS_VIOLATE_BOUNDARY, L"max");
    return 2;
  }
  int32_t iCount = XFA_ScriptInstanceManager_GetCount(this);
  if (iDesired == iCount) {
    return 0;
  }
  if (iDesired < iCount) {
    CFX_WideStringC wsInstManagerName = GetCData(XFA_ATTRIBUTE_Name);
    CFX_WideString wsInstanceName =
        CFX_WideString(wsInstManagerName.IsEmpty() ? wsInstManagerName
                                                   : wsInstManagerName.Mid(1));
    uint32_t dInstanceNameHash =
        FX_HashCode_GetW(wsInstanceName.AsStringC(), false);
    CXFA_Node* pPrevSibling =
        (iDesired == 0) ? this
                        : XFA_ScriptInstanceManager_GetItem(this, iDesired - 1);
    while (iCount > iDesired) {
      CXFA_Node* pRemoveInstance =
          pPrevSibling->GetNodeItem(XFA_NODEITEM_NextSibling);
      if (pRemoveInstance->GetElementType() != XFA_Element::Subform &&
          pRemoveInstance->GetElementType() != XFA_Element::SubformSet) {
        continue;
      }
      if (pRemoveInstance->GetElementType() == XFA_Element::InstanceManager) {
        ASSERT(FALSE);
        break;
      }
      if (pRemoveInstance->GetNameHash() == dInstanceNameHash) {
        XFA_ScriptInstanceManager_RemoveItem(this, pRemoveInstance);
        iCount--;
      }
    }
  } else if (iDesired > iCount) {
    while (iCount < iDesired) {
      CXFA_Node* pNewInstance =
          XFA_ScriptInstanceManager_CreateInstance(this, TRUE);
      XFA_ScriptInstanceManager_InsertItem(this, pNewInstance, iCount, iCount,
                                           FALSE);
      iCount++;
      CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
      if (!pNotify) {
        return 0;
      }
      pNotify->RunNodeInitialize(pNewInstance);
    }
  }
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (pLayoutPro) {
    pLayoutPro->AddChangedContainer(
        ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form)));
  }
  return 0;
}
int32_t CXFA_Node::InstanceManager_MoveInstance(int32_t iTo, int32_t iFrom) {
  int32_t iCount = XFA_ScriptInstanceManager_GetCount(this);
  if (iFrom > iCount || iTo > iCount - 1) {
    ThrowException(XFA_IDS_INDEX_OUT_OF_BOUNDS);
    return 1;
  }
  if (iFrom < 0 || iTo < 0 || iFrom == iTo) {
    return 0;
  }
  CXFA_Node* pMoveInstance = XFA_ScriptInstanceManager_GetItem(this, iFrom);
  XFA_ScriptInstanceManager_RemoveItem(this, pMoveInstance, FALSE);
  XFA_ScriptInstanceManager_InsertItem(this, pMoveInstance, iTo, iCount - 1,
                                       TRUE);
  CXFA_LayoutProcessor* pLayoutPro = m_pDocument->GetLayoutProcessor();
  if (pLayoutPro) {
    pLayoutPro->AddChangedContainer(
        ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form)));
  }
  return 0;
}
void CXFA_Node::Script_Occur_Max(CFXJSE_Value* pValue,
                                 FX_BOOL bSetting,
                                 XFA_ATTRIBUTE eAttribute) {
  CXFA_Occur occur(this);
  if (bSetting) {
    int32_t iMax = pValue->ToInteger();
    occur.SetMax(iMax);
  } else {
    pValue->SetInteger(occur.GetMax());
  }
}
void CXFA_Node::Script_Occur_Min(CFXJSE_Value* pValue,
                                 FX_BOOL bSetting,
                                 XFA_ATTRIBUTE eAttribute) {
  CXFA_Occur occur(this);
  if (bSetting) {
    int32_t iMin = pValue->ToInteger();
    occur.SetMin(iMin);
  } else {
    pValue->SetInteger(occur.GetMin());
  }
}
void CXFA_Node::Script_Desc_Metadata(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if ((argc == 0) || (argc == 1)) {
    pArguments->GetReturnValue()->SetString("");
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"metadata");
  }
}
void CXFA_Node::Script_Form_FormNodes(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CXFA_Node* pDataNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
    if (pDataNode) {
      CXFA_NodeArray formItems;
      CXFA_ArrayNodeList* pFormNodes = new CXFA_ArrayNodeList(m_pDocument);
      pFormNodes->SetArrayNodeList(formItems);
      pArguments->GetReturnValue()->SetObject(
          pFormNodes, m_pDocument->GetScriptContext()->GetJseNormalClass());
    } else {
      ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"formNodes");
  }
}
void CXFA_Node::Script_Form_Remerge(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    m_pDocument->DoDataRemerge(TRUE);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"remerge");
  }
}
void CXFA_Node::Script_Form_ExecInitialize(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Initialize);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execInitialize");
  }
}
void CXFA_Node::Script_Form_Recalculate(CFXJSE_Arguments* pArguments) {
  CXFA_EventParam* pEventParam =
      m_pDocument->GetScriptContext()->GetEventParam();
  if (pEventParam->m_eType == XFA_EVENT_Calculate ||
      pEventParam->m_eType == XFA_EVENT_InitCalculate) {
    return;
  }
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    const bool bScriptFlags = pArguments->GetInt32(0) != 0;
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    if (bScriptFlags) {
      pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Calculate);
      pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Validate);
      pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Ready, TRUE);
    } else {
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"recalculate");
  }
}
void CXFA_Node::Script_Form_ExecCalculate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return;
    }
    pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Calculate);
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execCalculate");
  }
}
void CXFA_Node::Script_Form_ExecValidate(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      pArguments->GetReturnValue()->SetBoolean(FALSE);
    } else {
      int32_t iRet = pNotify->ExecEventByDeepFirst(this, XFA_EVENT_Validate);
      pArguments->GetReturnValue()->SetBoolean(
          (iRet == XFA_EVENTERROR_Error) ? FALSE : TRUE);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"execValidate");
  }
}
void CXFA_Node::Script_Form_Checksum(CFXJSE_Value* pValue,
                                     FX_BOOL bSetting,
                                     XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    SetAttribute(XFA_ATTRIBUTE_Checksum, pValue->ToWideString().AsStringC());
  } else {
    CFX_WideString wsChecksum;
    GetAttribute(XFA_ATTRIBUTE_Checksum, wsChecksum, FALSE);
    pValue->SetString(
        FX_UTF8Encode(wsChecksum.c_str(), wsChecksum.GetLength()).AsStringC());
  }
}
void CXFA_Node::Script_Packet_GetAttribute(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString bsAttributeName = pArguments->GetUTF8String(0);
    CFX_WideString wsAttributeValue;
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (pXMLNode && pXMLNode->GetType() == FDE_XMLNODE_Element) {
      static_cast<CFDE_XMLElement*>(pXMLNode)->GetString(
          CFX_WideString::FromUTF8(bsAttributeName.AsStringC()).c_str(),
          wsAttributeValue);
    }
    pArguments->GetReturnValue()->SetString(
        FX_UTF8Encode(wsAttributeValue.c_str(), wsAttributeValue.GetLength())
            .AsStringC());
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"getAttribute");
  }
}
void CXFA_Node::Script_Packet_SetAttribute(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 2) {
    CFX_ByteString bsValue = pArguments->GetUTF8String(0);
    CFX_ByteString bsName = pArguments->GetUTF8String(1);
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (pXMLNode && pXMLNode->GetType() == FDE_XMLNODE_Element) {
      static_cast<CFDE_XMLElement*>(pXMLNode)
          ->SetString(CFX_WideString::FromUTF8(bsName.AsStringC()),
                      CFX_WideString::FromUTF8(bsValue.AsStringC()));
    }
    pArguments->GetReturnValue()->SetNull();
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"setAttribute");
  }
}
void CXFA_Node::Script_Packet_RemoveAttribute(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString bsName = pArguments->GetUTF8String(0);
    CFX_WideString wsName = CFX_WideString::FromUTF8(bsName.AsStringC());
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (pXMLNode && pXMLNode->GetType() == FDE_XMLNODE_Element) {
      CFDE_XMLElement* pXMLElement = static_cast<CFDE_XMLElement*>(pXMLNode);
      if (pXMLElement->HasAttribute(wsName.c_str())) {
        pXMLElement->RemoveAttribute(wsName.c_str());
      }
    }
    pArguments->GetReturnValue()->SetNull();
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"removeAttribute");
  }
}
void CXFA_Node::Script_Packet_Content(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (pXMLNode && pXMLNode->GetType() == FDE_XMLNODE_Element) {
      CFDE_XMLElement* pXMLElement = static_cast<CFDE_XMLElement*>(pXMLNode);
      pXMLElement->SetTextData(pValue->ToWideString());
    }
  } else {
    CFX_WideString wsTextData;
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (pXMLNode && pXMLNode->GetType() == FDE_XMLNODE_Element) {
      CFDE_XMLElement* pXMLElement = static_cast<CFDE_XMLElement*>(pXMLNode);
      pXMLElement->GetTextData(wsTextData);
    }
    pValue->SetString(
        FX_UTF8Encode(wsTextData.c_str(), wsTextData.GetLength()).AsStringC());
  }
}
void CXFA_Node::Script_Source_Next(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"next");
  }
}
void CXFA_Node::Script_Source_CancelBatch(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"cancelBatch");
  }
}
void CXFA_Node::Script_Source_First(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"first");
  }
}
void CXFA_Node::Script_Source_UpdateBatch(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"updateBatch");
  }
}
void CXFA_Node::Script_Source_Previous(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"previous");
  }
}
void CXFA_Node::Script_Source_IsBOF(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"isBOF");
  }
}
void CXFA_Node::Script_Source_IsEOF(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"isEOF");
  }
}
void CXFA_Node::Script_Source_Cancel(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"cancel");
  }
}
void CXFA_Node::Script_Source_Update(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"update");
  }
}
void CXFA_Node::Script_Source_Open(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"open");
  }
}
void CXFA_Node::Script_Source_Delete(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"delete");
  }
}
void CXFA_Node::Script_Source_AddNew(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"addNew");
  }
}
void CXFA_Node::Script_Source_Requery(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"requery");
  }
}
void CXFA_Node::Script_Source_Resync(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"resync");
  }
}
void CXFA_Node::Script_Source_Close(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"close");
  }
}
void CXFA_Node::Script_Source_Last(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"last");
  }
}
void CXFA_Node::Script_Source_HasDataChanged(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 0) {
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"hasDataChanged");
  }
}
void CXFA_Node::Script_Source_Db(CFXJSE_Value* pValue,
                                 FX_BOOL bSetting,
                                 XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Xfa_This(CFXJSE_Value* pValue,
                                FX_BOOL bSetting,
                                XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    CXFA_Object* pThis = m_pDocument->GetScriptContext()->GetThisObject();
    ASSERT(pThis);
    pValue->Assign(m_pDocument->GetScriptContext()->GetJSValueFromMap(pThis));
  }
}
void CXFA_Node::Script_Handler_Version(CFXJSE_Value* pValue,
                                       FX_BOOL bSetting,
                                       XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_SubmitFormat_Mode(CFXJSE_Value* pValue,
                                         FX_BOOL bSetting,
                                         XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Extras_Type(CFXJSE_Value* pValue,
                                   FX_BOOL bSetting,
                                   XFA_ATTRIBUTE eAttribute) {}
void CXFA_Node::Script_Script_Stateless(CFXJSE_Value* pValue,
                                        FX_BOOL bSetting,
                                        XFA_ATTRIBUTE eAttribute) {
  if (bSetting) {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
    return;
  }
  pValue->SetString(FX_UTF8Encode(FX_WSTRC(L"0")).AsStringC());
}
void CXFA_Node::Script_Encrypt_Format(CFXJSE_Value* pValue,
                                      FX_BOOL bSetting,
                                      XFA_ATTRIBUTE eAttribute) {}
enum XFA_KEYTYPE {
  XFA_KEYTYPE_Custom,
  XFA_KEYTYPE_Element,
};
void* XFA_GetMapKey_Custom(const CFX_WideStringC& wsKey) {
  uint32_t dwKey = FX_HashCode_GetW(wsKey, false);
  return (void*)(uintptr_t)((dwKey << 1) | XFA_KEYTYPE_Custom);
}
void* XFA_GetMapKey_Element(XFA_Element eType, XFA_ATTRIBUTE eAttribute) {
  return (void*)(uintptr_t)((static_cast<int32_t>(eType) << 16) |
                            (eAttribute << 8) | XFA_KEYTYPE_Element);
}
FX_BOOL CXFA_Node::HasAttribute(XFA_ATTRIBUTE eAttr, FX_BOOL bCanInherit) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  return HasMapModuleKey(pKey, bCanInherit);
}
FX_BOOL CXFA_Node::SetAttribute(XFA_ATTRIBUTE eAttr,
                                const CFX_WideStringC& wsValue,
                                bool bNotify) {
  const XFA_ATTRIBUTEINFO* pAttr = XFA_GetAttributeByID(eAttr);
  if (!pAttr)
    return FALSE;

  XFA_ATTRIBUTETYPE eType = pAttr->eType;
  if (eType == XFA_ATTRIBUTETYPE_NOTSURE) {
    const XFA_NOTSUREATTRIBUTE* pNotsure =
        XFA_GetNotsureAttribute(GetElementType(), pAttr->eName);
    eType = pNotsure ? pNotsure->eType : XFA_ATTRIBUTETYPE_Cdata;
  }
  switch (eType) {
    case XFA_ATTRIBUTETYPE_Enum: {
      const XFA_ATTRIBUTEENUMINFO* pEnum = XFA_GetAttributeEnumByName(wsValue);
      return SetEnum(pAttr->eName,
                     pEnum ? pEnum->eName
                           : (XFA_ATTRIBUTEENUM)(intptr_t)(pAttr->pDefValue),
                     bNotify);
    } break;
    case XFA_ATTRIBUTETYPE_Cdata:
      return SetCData(pAttr->eName, CFX_WideString(wsValue), bNotify);
    case XFA_ATTRIBUTETYPE_Boolean:
      return SetBoolean(pAttr->eName, wsValue != FX_WSTRC(L"0"), bNotify);
    case XFA_ATTRIBUTETYPE_Integer:
      return SetInteger(
          pAttr->eName,
          FXSYS_round(FX_wcstof(wsValue.c_str(), wsValue.GetLength())),
          bNotify);
    case XFA_ATTRIBUTETYPE_Measure:
      return SetMeasure(pAttr->eName, CXFA_Measurement(wsValue), bNotify);
    default:
      break;
  }
  return FALSE;
}
FX_BOOL CXFA_Node::GetAttribute(XFA_ATTRIBUTE eAttr,
                                CFX_WideString& wsValue,
                                FX_BOOL bUseDefault) {
  const XFA_ATTRIBUTEINFO* pAttr = XFA_GetAttributeByID(eAttr);
  if (!pAttr) {
    return FALSE;
  }
  XFA_ATTRIBUTETYPE eType = pAttr->eType;
  if (eType == XFA_ATTRIBUTETYPE_NOTSURE) {
    const XFA_NOTSUREATTRIBUTE* pNotsure =
        XFA_GetNotsureAttribute(GetElementType(), pAttr->eName);
    eType = pNotsure ? pNotsure->eType : XFA_ATTRIBUTETYPE_Cdata;
  }
  switch (eType) {
    case XFA_ATTRIBUTETYPE_Enum: {
      XFA_ATTRIBUTEENUM eValue;
      if (!TryEnum(pAttr->eName, eValue, bUseDefault)) {
        return FALSE;
      }
      wsValue = XFA_GetAttributeEnumByID(eValue)->pName;
      return TRUE;
    } break;
    case XFA_ATTRIBUTETYPE_Cdata: {
      CFX_WideStringC wsValueC;
      if (!TryCData(pAttr->eName, wsValueC, bUseDefault)) {
        return FALSE;
      }
      wsValue = wsValueC;
      return TRUE;
    } break;
    case XFA_ATTRIBUTETYPE_Boolean: {
      FX_BOOL bValue;
      if (!TryBoolean(pAttr->eName, bValue, bUseDefault)) {
        return FALSE;
      }
      wsValue = bValue ? FX_WSTRC(L"1") : FX_WSTRC(L"0");
      return TRUE;
    } break;
    case XFA_ATTRIBUTETYPE_Integer: {
      int32_t iValue;
      if (!TryInteger(pAttr->eName, iValue, bUseDefault)) {
        return FALSE;
      }
      wsValue.Format(L"%d", iValue);
      return TRUE;
    } break;
    case XFA_ATTRIBUTETYPE_Measure: {
      CXFA_Measurement mValue;
      if (!TryMeasure(pAttr->eName, mValue, bUseDefault)) {
        return FALSE;
      }
      mValue.ToString(wsValue);
      return TRUE;
    } break;
    default:
      break;
  }
  return FALSE;
}
FX_BOOL CXFA_Node::SetAttribute(const CFX_WideStringC& wsAttr,
                                const CFX_WideStringC& wsValue,
                                bool bNotify) {
  const XFA_ATTRIBUTEINFO* pAttributeInfo = XFA_GetAttributeByName(wsValue);
  if (pAttributeInfo) {
    return SetAttribute(pAttributeInfo->eName, wsValue, bNotify);
  }
  void* pKey = XFA_GetMapKey_Custom(wsAttr);
  SetMapModuleString(pKey, wsValue);
  return TRUE;
}
FX_BOOL CXFA_Node::GetAttribute(const CFX_WideStringC& wsAttr,
                                CFX_WideString& wsValue,
                                FX_BOOL bUseDefault) {
  const XFA_ATTRIBUTEINFO* pAttributeInfo = XFA_GetAttributeByName(wsAttr);
  if (pAttributeInfo) {
    return GetAttribute(pAttributeInfo->eName, wsValue, bUseDefault);
  }
  void* pKey = XFA_GetMapKey_Custom(wsAttr);
  CFX_WideStringC wsValueC;
  if (GetMapModuleString(pKey, wsValueC)) {
    wsValue = wsValueC;
  }
  return TRUE;
}
FX_BOOL CXFA_Node::RemoveAttribute(const CFX_WideStringC& wsAttr) {
  void* pKey = XFA_GetMapKey_Custom(wsAttr);
  RemoveMapModuleKey(pKey);
  return TRUE;
}
FX_BOOL CXFA_Node::TryBoolean(XFA_ATTRIBUTE eAttr,
                              FX_BOOL& bValue,
                              FX_BOOL bUseDefault) {
  void* pValue = nullptr;
  if (!GetValue(eAttr, XFA_ATTRIBUTETYPE_Boolean, bUseDefault, pValue))
    return FALSE;
  bValue = (FX_BOOL)(uintptr_t)pValue;
  return TRUE;
}
FX_BOOL CXFA_Node::TryInteger(XFA_ATTRIBUTE eAttr,
                              int32_t& iValue,
                              FX_BOOL bUseDefault) {
  void* pValue = nullptr;
  if (!GetValue(eAttr, XFA_ATTRIBUTETYPE_Integer, bUseDefault, pValue))
    return FALSE;
  iValue = (int32_t)(uintptr_t)pValue;
  return TRUE;
}
FX_BOOL CXFA_Node::TryEnum(XFA_ATTRIBUTE eAttr,
                           XFA_ATTRIBUTEENUM& eValue,
                           FX_BOOL bUseDefault) {
  void* pValue = nullptr;
  if (!GetValue(eAttr, XFA_ATTRIBUTETYPE_Enum, bUseDefault, pValue))
    return FALSE;
  eValue = (XFA_ATTRIBUTEENUM)(uintptr_t)pValue;
  return TRUE;
}

FX_BOOL CXFA_Node::SetMeasure(XFA_ATTRIBUTE eAttr,
                              CXFA_Measurement mValue,
                              bool bNotify) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  SetMapModuleBuffer(pKey, &mValue, sizeof(CXFA_Measurement));
  OnChanged(eAttr, bNotify, FALSE);
  return TRUE;
}

FX_BOOL CXFA_Node::TryMeasure(XFA_ATTRIBUTE eAttr,
                              CXFA_Measurement& mValue,
                              FX_BOOL bUseDefault) const {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  void* pValue;
  int32_t iBytes;
  if (GetMapModuleBuffer(pKey, pValue, iBytes) && iBytes == sizeof(mValue)) {
    FXSYS_memcpy(&mValue, pValue, sizeof(mValue));
    return TRUE;
  }
  if (bUseDefault &&
      XFA_GetAttributeDefaultValue(pValue, GetElementType(), eAttr,
                                   XFA_ATTRIBUTETYPE_Measure, m_ePacket)) {
    FXSYS_memcpy(&mValue, pValue, sizeof(mValue));
    return TRUE;
  }
  return FALSE;
}

CXFA_Measurement CXFA_Node::GetMeasure(XFA_ATTRIBUTE eAttr) const {
  CXFA_Measurement mValue;
  return TryMeasure(eAttr, mValue, TRUE) ? mValue : CXFA_Measurement();
}

FX_BOOL CXFA_Node::SetCData(XFA_ATTRIBUTE eAttr,
                            const CFX_WideString& wsValue,
                            bool bNotify,
                            FX_BOOL bScriptModify) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  if (eAttr == XFA_ATTRIBUTE_Value) {
    CFX_WideString* pClone = new CFX_WideString(wsValue);
    SetUserData(pKey, pClone, &deleteWideStringCallBack);
  } else {
    SetMapModuleString(pKey, wsValue.AsStringC());
    if (eAttr == XFA_ATTRIBUTE_Name)
      UpdateNameHash();
  }
  OnChanged(eAttr, bNotify, bScriptModify);

  if (!IsNeedSavingXMLNode() || eAttr == XFA_ATTRIBUTE_QualifiedName ||
      eAttr == XFA_ATTRIBUTE_BindingNode) {
    return TRUE;
  }

  if (eAttr == XFA_ATTRIBUTE_Name &&
      (m_elementType == XFA_Element::DataValue ||
       m_elementType == XFA_Element::DataGroup)) {
    return TRUE;
  }

  if (eAttr == XFA_ATTRIBUTE_Value) {
    FDE_XMLNODETYPE eXMLType = m_pXMLNode->GetType();
    switch (eXMLType) {
      case FDE_XMLNODE_Element:
        if (IsAttributeInXML()) {
          static_cast<CFDE_XMLElement*>(m_pXMLNode)
              ->SetString(CFX_WideString(GetCData(XFA_ATTRIBUTE_QualifiedName)),
                          wsValue);
        } else {
          FX_BOOL bDeleteChildren = TRUE;
          if (GetPacketID() == XFA_XDPPACKET_Datasets) {
            for (CXFA_Node* pChildDataNode =
                     GetNodeItem(XFA_NODEITEM_FirstChild);
                 pChildDataNode; pChildDataNode = pChildDataNode->GetNodeItem(
                                     XFA_NODEITEM_NextSibling)) {
              CXFA_NodeArray formNodes;
              if (pChildDataNode->GetBindItems(formNodes) > 0) {
                bDeleteChildren = FALSE;
                break;
              }
            }
          }
          if (bDeleteChildren) {
            static_cast<CFDE_XMLElement*>(m_pXMLNode)->DeleteChildren();
          }
          static_cast<CFDE_XMLElement*>(m_pXMLNode)->SetTextData(wsValue);
        }
        break;
      case FDE_XMLNODE_Text:
        static_cast<CFDE_XMLText*>(m_pXMLNode)->SetText(wsValue);
        break;
      default:
        ASSERT(0);
    }
    return TRUE;
  }

  const XFA_ATTRIBUTEINFO* pInfo = XFA_GetAttributeByID(eAttr);
  if (pInfo) {
    ASSERT(m_pXMLNode->GetType() == FDE_XMLNODE_Element);
    CFX_WideString wsAttrName = pInfo->pName;
    if (pInfo->eName == XFA_ATTRIBUTE_ContentType) {
      wsAttrName = FX_WSTRC(L"xfa:") + wsAttrName;
    }
    static_cast<CFDE_XMLElement*>(m_pXMLNode)->SetString(wsAttrName, wsValue);
  }
  return TRUE;
}

FX_BOOL CXFA_Node::SetAttributeValue(const CFX_WideString& wsValue,
                                     const CFX_WideString& wsXMLValue,
                                     bool bNotify,
                                     FX_BOOL bScriptModify) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), XFA_ATTRIBUTE_Value);
  OnChanging(XFA_ATTRIBUTE_Value, bNotify);
  CFX_WideString* pClone = new CFX_WideString(wsValue);
  SetUserData(pKey, pClone, &deleteWideStringCallBack);
  OnChanged(XFA_ATTRIBUTE_Value, bNotify, bScriptModify);
  if (IsNeedSavingXMLNode()) {
    FDE_XMLNODETYPE eXMLType = m_pXMLNode->GetType();
    switch (eXMLType) {
      case FDE_XMLNODE_Element:
        if (IsAttributeInXML()) {
          static_cast<CFDE_XMLElement*>(m_pXMLNode)
              ->SetString(CFX_WideString(GetCData(XFA_ATTRIBUTE_QualifiedName)),
                          wsXMLValue);
        } else {
          FX_BOOL bDeleteChildren = TRUE;
          if (GetPacketID() == XFA_XDPPACKET_Datasets) {
            for (CXFA_Node* pChildDataNode =
                     GetNodeItem(XFA_NODEITEM_FirstChild);
                 pChildDataNode; pChildDataNode = pChildDataNode->GetNodeItem(
                                     XFA_NODEITEM_NextSibling)) {
              CXFA_NodeArray formNodes;
              if (pChildDataNode->GetBindItems(formNodes) > 0) {
                bDeleteChildren = FALSE;
                break;
              }
            }
          }
          if (bDeleteChildren) {
            static_cast<CFDE_XMLElement*>(m_pXMLNode)->DeleteChildren();
          }
          static_cast<CFDE_XMLElement*>(m_pXMLNode)->SetTextData(wsXMLValue);
        }
        break;
      case FDE_XMLNODE_Text:
        static_cast<CFDE_XMLText*>(m_pXMLNode)->SetText(wsXMLValue);
        break;
      default:
        ASSERT(0);
    }
  }
  return TRUE;
}
FX_BOOL CXFA_Node::TryCData(XFA_ATTRIBUTE eAttr,
                            CFX_WideString& wsValue,
                            FX_BOOL bUseDefault,
                            FX_BOOL bProto) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  if (eAttr == XFA_ATTRIBUTE_Value) {
    CFX_WideString* pStr = (CFX_WideString*)GetUserData(pKey, bProto);
    if (pStr) {
      wsValue = *pStr;
      return TRUE;
    }
  } else {
    CFX_WideStringC wsValueC;
    if (GetMapModuleString(pKey, wsValueC)) {
      wsValue = wsValueC;
      return TRUE;
    }
  }
  if (!bUseDefault) {
    return FALSE;
  }
  void* pValue = nullptr;
  if (XFA_GetAttributeDefaultValue(pValue, GetElementType(), eAttr,
                                   XFA_ATTRIBUTETYPE_Cdata, m_ePacket)) {
    wsValue = (const FX_WCHAR*)pValue;
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_Node::TryCData(XFA_ATTRIBUTE eAttr,
                            CFX_WideStringC& wsValue,
                            FX_BOOL bUseDefault,
                            FX_BOOL bProto) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  if (eAttr == XFA_ATTRIBUTE_Value) {
    CFX_WideString* pStr = (CFX_WideString*)GetUserData(pKey, bProto);
    if (pStr) {
      wsValue = pStr->AsStringC();
      return TRUE;
    }
  } else {
    if (GetMapModuleString(pKey, wsValue)) {
      return TRUE;
    }
  }
  if (!bUseDefault) {
    return FALSE;
  }
  void* pValue = nullptr;
  if (XFA_GetAttributeDefaultValue(pValue, GetElementType(), eAttr,
                                   XFA_ATTRIBUTETYPE_Cdata, m_ePacket)) {
    wsValue = (CFX_WideStringC)(const FX_WCHAR*)pValue;
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_Node::SetObject(XFA_ATTRIBUTE eAttr,
                             void* pData,
                             XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  return SetUserData(pKey, pData, pCallbackInfo);
}
FX_BOOL CXFA_Node::TryObject(XFA_ATTRIBUTE eAttr, void*& pData) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  pData = GetUserData(pKey);
  return !!pData;
}
FX_BOOL CXFA_Node::SetValue(XFA_ATTRIBUTE eAttr,
                            XFA_ATTRIBUTETYPE eType,
                            void* pValue,
                            bool bNotify) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  OnChanging(eAttr, bNotify);
  SetMapModuleValue(pKey, pValue);
  OnChanged(eAttr, bNotify, FALSE);
  if (IsNeedSavingXMLNode()) {
    ASSERT(m_pXMLNode->GetType() == FDE_XMLNODE_Element);
    const XFA_ATTRIBUTEINFO* pInfo = XFA_GetAttributeByID(eAttr);
    if (pInfo) {
      switch (eType) {
        case XFA_ATTRIBUTETYPE_Enum:
          static_cast<CFDE_XMLElement*>(m_pXMLNode)
              ->SetString(
                  pInfo->pName,
                  XFA_GetAttributeEnumByID((XFA_ATTRIBUTEENUM)(uintptr_t)pValue)
                      ->pName);
          break;
        case XFA_ATTRIBUTETYPE_Boolean:
          static_cast<CFDE_XMLElement*>(m_pXMLNode)
              ->SetString(pInfo->pName, pValue ? L"1" : L"0");
          break;
        case XFA_ATTRIBUTETYPE_Integer:
          static_cast<CFDE_XMLElement*>(m_pXMLNode)
              ->SetInteger(pInfo->pName, (int32_t)(uintptr_t)pValue);
          break;
        default:
          ASSERT(0);
      }
    }
  }
  return TRUE;
}
FX_BOOL CXFA_Node::GetValue(XFA_ATTRIBUTE eAttr,
                            XFA_ATTRIBUTETYPE eType,
                            FX_BOOL bUseDefault,
                            void*& pValue) {
  void* pKey = XFA_GetMapKey_Element(GetElementType(), eAttr);
  if (GetMapModuleValue(pKey, pValue)) {
    return TRUE;
  }
  if (!bUseDefault) {
    return FALSE;
  }
  return XFA_GetAttributeDefaultValue(pValue, GetElementType(), eAttr, eType,
                                      m_ePacket);
}
static void XFA_DefaultFreeData(void* pData) {}
static XFA_MAPDATABLOCKCALLBACKINFO gs_XFADefaultFreeData = {
    XFA_DefaultFreeData, nullptr};
FX_BOOL CXFA_Node::SetUserData(void* pKey,
                               void* pData,
                               XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo) {
  SetMapModuleBuffer(pKey, &pData, sizeof(void*),
                     pCallbackInfo ? pCallbackInfo : &gs_XFADefaultFreeData);
  return TRUE;
}
FX_BOOL CXFA_Node::TryUserData(void* pKey, void*& pData, FX_BOOL bProtoAlso) {
  int32_t iBytes = 0;
  if (!GetMapModuleBuffer(pKey, pData, iBytes, bProtoAlso)) {
    return FALSE;
  }
  return iBytes == sizeof(void*) && FXSYS_memcpy(&pData, pData, iBytes);
}
FX_BOOL CXFA_Node::SetScriptContent(const CFX_WideString& wsContent,
                                    const CFX_WideString& wsXMLValue,
                                    bool bNotify,
                                    FX_BOOL bScriptModify,
                                    FX_BOOL bSyncData) {
  CXFA_Node* pNode = nullptr;
  CXFA_Node* pBindNode = nullptr;
  switch (GetObjectType()) {
    case XFA_ObjectType::ContainerNode: {
      if (XFA_FieldIsMultiListBox(this)) {
        CXFA_Node* pValue = GetProperty(0, XFA_Element::Value);
        CXFA_Node* pChildValue = pValue->GetNodeItem(XFA_NODEITEM_FirstChild);
        ASSERT(pChildValue);
        pChildValue->SetCData(XFA_ATTRIBUTE_ContentType, L"text/xml");
        pChildValue->SetScriptContent(wsContent, wsContent, bNotify,
                                      bScriptModify, FALSE);
        CXFA_Node* pBind = GetBindData();
        if (bSyncData && pBind) {
          CFX_WideStringArray wsSaveTextArray;
          int32_t iSize = 0;
          if (!wsContent.IsEmpty()) {
            int32_t iStart = 0;
            int32_t iLength = wsContent.GetLength();
            int32_t iEnd = wsContent.Find(L'\n', iStart);
            iEnd = (iEnd == -1) ? iLength : iEnd;
            while (iEnd >= iStart) {
              wsSaveTextArray.Add(wsContent.Mid(iStart, iEnd - iStart));
              iStart = iEnd + 1;
              if (iStart >= iLength) {
                break;
              }
              iEnd = wsContent.Find(L'\n', iStart);
              if (iEnd < 0) {
                wsSaveTextArray.Add(wsContent.Mid(iStart, iLength - iStart));
              }
            }
            iSize = wsSaveTextArray.GetSize();
          }
          if (iSize == 0) {
            while (CXFA_Node* pChildNode =
                       pBind->GetNodeItem(XFA_NODEITEM_FirstChild)) {
              pBind->RemoveChild(pChildNode);
            }
          } else {
            CXFA_NodeArray valueNodes;
            int32_t iDatas = pBind->GetNodeList(
                valueNodes, XFA_NODEFILTER_Children, XFA_Element::DataValue);
            if (iDatas < iSize) {
              int32_t iAddNodes = iSize - iDatas;
              CXFA_Node* pValueNodes = nullptr;
              while (iAddNodes-- > 0) {
                pValueNodes =
                    pBind->CreateSamePacketNode(XFA_Element::DataValue);
                pValueNodes->SetCData(XFA_ATTRIBUTE_Name, L"value");
                pValueNodes->CreateXMLMappingNode();
                pBind->InsertChild(pValueNodes);
              }
              pValueNodes = nullptr;
            } else if (iDatas > iSize) {
              int32_t iDelNodes = iDatas - iSize;
              while (iDelNodes-- > 0) {
                pBind->RemoveChild(pBind->GetNodeItem(XFA_NODEITEM_FirstChild));
              }
            }
            int32_t i = 0;
            for (CXFA_Node* pValueNode =
                     pBind->GetNodeItem(XFA_NODEITEM_FirstChild);
                 pValueNode; pValueNode = pValueNode->GetNodeItem(
                                 XFA_NODEITEM_NextSibling)) {
              pValueNode->SetAttributeValue(wsSaveTextArray[i],
                                            wsSaveTextArray[i], FALSE);
              i++;
            }
          }
          CXFA_NodeArray nodeArray;
          pBind->GetBindItems(nodeArray);
          for (int32_t i = 0; i < nodeArray.GetSize(); i++) {
            if (nodeArray[i] != this) {
              nodeArray[i]->SetScriptContent(wsContent, wsContent, bNotify,
                                             bScriptModify, FALSE);
            }
          }
        }
        break;
      } else if (GetElementType() == XFA_Element::ExclGroup) {
        pNode = this;
      } else {
        CXFA_Node* pValue = GetProperty(0, XFA_Element::Value);
        CXFA_Node* pChildValue = pValue->GetNodeItem(XFA_NODEITEM_FirstChild);
        ASSERT(pChildValue);
        pChildValue->SetScriptContent(wsContent, wsContent, bNotify,
                                      bScriptModify, FALSE);
      }
      pBindNode = GetBindData();
      if (pBindNode && bSyncData) {
        pBindNode->SetScriptContent(wsContent, wsXMLValue, bNotify,
                                    bScriptModify, FALSE);
        CXFA_NodeArray nodeArray;
        pBindNode->GetBindItems(nodeArray);
        for (int32_t i = 0; i < nodeArray.GetSize(); i++) {
          if (nodeArray[i] != this) {
            nodeArray[i]->SetScriptContent(wsContent, wsContent, bNotify, true,
                                           FALSE);
          }
        }
      }
      pBindNode = nullptr;
      break;
    }
    case XFA_ObjectType::ContentNode: {
      CFX_WideString wsContentType;
      if (GetElementType() == XFA_Element::ExData) {
        GetAttribute(XFA_ATTRIBUTE_ContentType, wsContentType, FALSE);
        if (wsContentType == FX_WSTRC(L"text/html")) {
          wsContentType = FX_WSTRC(L"");
          SetAttribute(XFA_ATTRIBUTE_ContentType, wsContentType.AsStringC());
        }
      }
      CXFA_Node* pContentRawDataNode = GetNodeItem(XFA_NODEITEM_FirstChild);
      if (!pContentRawDataNode) {
        pContentRawDataNode = CreateSamePacketNode(
            (wsContentType == FX_WSTRC(L"text/xml")) ? XFA_Element::Sharpxml
                                                     : XFA_Element::Sharptext);
        InsertChild(pContentRawDataNode);
      }
      return pContentRawDataNode->SetScriptContent(
          wsContent, wsXMLValue, bNotify, bScriptModify, bSyncData);
    } break;
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::TextNode:
      pNode = this;
      break;
    case XFA_ObjectType::NodeV:
      pNode = this;
      if (bSyncData && GetPacketID() == XFA_XDPPACKET_Form) {
        CXFA_Node* pParent = GetNodeItem(XFA_NODEITEM_Parent);
        if (pParent) {
          pParent = pParent->GetNodeItem(XFA_NODEITEM_Parent);
        }
        if (pParent && pParent->GetElementType() == XFA_Element::Value) {
          pParent = pParent->GetNodeItem(XFA_NODEITEM_Parent);
          if (pParent && pParent->IsContainerNode()) {
            pBindNode = pParent->GetBindData();
            if (pBindNode) {
              pBindNode->SetScriptContent(wsContent, wsXMLValue, bNotify,
                                          bScriptModify, FALSE);
            }
          }
        }
      }
      break;
    default:
      if (GetElementType() == XFA_Element::DataValue) {
        pNode = this;
        pBindNode = this;
      }
      break;
  }
  if (pNode) {
    SetAttributeValue(wsContent, wsXMLValue, bNotify, bScriptModify);
    if (pBindNode && bSyncData) {
      CXFA_NodeArray nodeArray;
      pBindNode->GetBindItems(nodeArray);
      for (int32_t i = 0; i < nodeArray.GetSize(); i++) {
        nodeArray[i]->SetScriptContent(wsContent, wsContent, bNotify,
                                       bScriptModify, FALSE);
      }
    }
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXFA_Node::SetContent(const CFX_WideString& wsContent,
                              const CFX_WideString& wsXMLValue,
                              bool bNotify,
                              FX_BOOL bScriptModify,
                              FX_BOOL bSyncData) {
  return SetScriptContent(wsContent, wsXMLValue, bNotify, bScriptModify,
                          bSyncData);
}
CFX_WideString CXFA_Node::GetScriptContent(FX_BOOL bScriptModify) {
  CFX_WideString wsContent;
  return TryContent(wsContent, bScriptModify) ? wsContent : CFX_WideString();
}
CFX_WideString CXFA_Node::GetContent() {
  return GetScriptContent();
}
FX_BOOL CXFA_Node::TryContent(CFX_WideString& wsContent,
                              FX_BOOL bScriptModify,
                              FX_BOOL bProto) {
  CXFA_Node* pNode = nullptr;
  switch (GetObjectType()) {
    case XFA_ObjectType::ContainerNode:
      if (GetElementType() == XFA_Element::ExclGroup) {
        pNode = this;
      } else {
        CXFA_Node* pValue = GetChild(0, XFA_Element::Value);
        if (!pValue) {
          return FALSE;
        }
        CXFA_Node* pChildValue = pValue->GetNodeItem(XFA_NODEITEM_FirstChild);
        if (pChildValue && XFA_FieldIsMultiListBox(this)) {
          pChildValue->SetAttribute(XFA_ATTRIBUTE_ContentType,
                                    FX_WSTRC(L"text/xml"));
        }
        return pChildValue
                   ? pChildValue->TryContent(wsContent, bScriptModify, bProto)
                   : FALSE;
      }
      break;
    case XFA_ObjectType::ContentNode: {
      CXFA_Node* pContentRawDataNode = GetNodeItem(XFA_NODEITEM_FirstChild);
      if (!pContentRawDataNode) {
        XFA_Element element = XFA_Element::Sharptext;
        if (GetElementType() == XFA_Element::ExData) {
          CFX_WideString wsContentType;
          GetAttribute(XFA_ATTRIBUTE_ContentType, wsContentType, FALSE);
          if (wsContentType == FX_WSTRC(L"text/html")) {
            element = XFA_Element::SharpxHTML;
          } else if (wsContentType == FX_WSTRC(L"text/xml")) {
            element = XFA_Element::Sharpxml;
          }
        }
        pContentRawDataNode = CreateSamePacketNode(element);
        InsertChild(pContentRawDataNode);
      }
      return pContentRawDataNode->TryContent(wsContent, bScriptModify, bProto);
    }
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::NodeV:
    case XFA_ObjectType::TextNode:
      pNode = this;
    default:
      if (GetElementType() == XFA_Element::DataValue) {
        pNode = this;
      }
      break;
  }
  if (pNode) {
    if (bScriptModify) {
      CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
      if (pScriptContext) {
        m_pDocument->GetScriptContext()->AddNodesOfRunScript(this);
      }
    }
    return TryCData(XFA_ATTRIBUTE_Value, wsContent, FALSE, bProto);
  }
  return FALSE;
}
CXFA_Node* CXFA_Node::GetModelNode() {
  switch (GetPacketID()) {
    case XFA_XDPPACKET_XDP:
      return m_pDocument->GetRoot();
    case XFA_XDPPACKET_Config:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Config));
    case XFA_XDPPACKET_Template:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Template));
    case XFA_XDPPACKET_Form:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Form));
    case XFA_XDPPACKET_Datasets:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Datasets));
    case XFA_XDPPACKET_LocaleSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_LocaleSet));
    case XFA_XDPPACKET_ConnectionSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_ConnectionSet));
    case XFA_XDPPACKET_SourceSet:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_SourceSet));
    case XFA_XDPPACKET_Xdc:
      return ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Xdc));
    default:
      return this;
  }
}
FX_BOOL CXFA_Node::TryNamespace(CFX_WideString& wsNamespace) {
  wsNamespace.clear();
  if (IsModelNode() || GetElementType() == XFA_Element::Packet) {
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (!pXMLNode || pXMLNode->GetType() != FDE_XMLNODE_Element) {
      return FALSE;
    }
    static_cast<CFDE_XMLElement*>(pXMLNode)->GetNamespaceURI(wsNamespace);
    return TRUE;
  } else if (GetPacketID() == XFA_XDPPACKET_Datasets) {
    CFDE_XMLNode* pXMLNode = GetXMLMappingNode();
    if (!pXMLNode) {
      return FALSE;
    }
    if (pXMLNode->GetType() != FDE_XMLNODE_Element) {
      return TRUE;
    }
    if (GetElementType() == XFA_Element::DataValue &&
        GetEnum(XFA_ATTRIBUTE_Contains) == XFA_ATTRIBUTEENUM_MetaData) {
      return XFA_FDEExtension_ResolveNamespaceQualifier(
          static_cast<CFDE_XMLElement*>(pXMLNode),
          GetCData(XFA_ATTRIBUTE_QualifiedName), wsNamespace);
    }
    static_cast<CFDE_XMLElement*>(pXMLNode)->GetNamespaceURI(wsNamespace);
    return TRUE;
  } else {
    CXFA_Node* pModelNode = GetModelNode();
    return pModelNode->TryNamespace(wsNamespace);
  }
}
CXFA_Node* CXFA_Node::GetProperty(int32_t index,
                                  XFA_Element eProperty,
                                  FX_BOOL bCreateProperty) {
  XFA_Element eType = GetElementType();
  uint32_t dwPacket = GetPacketID();
  const XFA_PROPERTY* pProperty =
      XFA_GetPropertyOfElement(eType, eProperty, dwPacket);
  if (!pProperty || index >= pProperty->uOccur)
    return nullptr;

  CXFA_Node* pNode = m_pChild;
  int32_t iCount = 0;
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() == eProperty) {
      iCount++;
      if (iCount > index) {
        return pNode;
      }
    }
  }
  if (!bCreateProperty)
    return nullptr;

  if (pProperty->uFlags & XFA_PROPERTYFLAG_OneOf) {
    pNode = m_pChild;
    for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      const XFA_PROPERTY* pExistProperty =
          XFA_GetPropertyOfElement(eType, pNode->GetElementType(), dwPacket);
      if (pExistProperty && (pExistProperty->uFlags & XFA_PROPERTYFLAG_OneOf))
        return nullptr;
    }
  }

  const XFA_PACKETINFO* pPacket = XFA_GetPacketByID(dwPacket);
  CXFA_Node* pNewNode = nullptr;
  for (; iCount <= index; iCount++) {
    pNewNode = m_pDocument->CreateNode(pPacket, eProperty);
    if (!pNewNode)
      return nullptr;
    InsertChild(pNewNode, nullptr);
    pNewNode->SetFlag(XFA_NodeFlag_Initialized, true);
  }
  return pNewNode;
}
int32_t CXFA_Node::CountChildren(XFA_Element eType, FX_BOOL bOnlyChild) {
  CXFA_Node* pNode = m_pChild;
  int32_t iCount = 0;
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() == eType || eType == XFA_Element::Unknown) {
      if (bOnlyChild) {
        const XFA_PROPERTY* pProperty = XFA_GetPropertyOfElement(
            GetElementType(), pNode->GetElementType(), XFA_XDPPACKET_UNKNOWN);
        if (pProperty) {
          continue;
        }
      }
      iCount++;
    }
  }
  return iCount;
}
CXFA_Node* CXFA_Node::GetChild(int32_t index,
                               XFA_Element eType,
                               FX_BOOL bOnlyChild) {
  ASSERT(index > -1);
  CXFA_Node* pNode = m_pChild;
  int32_t iCount = 0;
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() == eType || eType == XFA_Element::Unknown) {
      if (bOnlyChild) {
        const XFA_PROPERTY* pProperty = XFA_GetPropertyOfElement(
            GetElementType(), pNode->GetElementType(), XFA_XDPPACKET_UNKNOWN);
        if (pProperty) {
          continue;
        }
      }
      iCount++;
      if (iCount > index) {
        return pNode;
      }
    }
  }
  return nullptr;
}
int32_t CXFA_Node::InsertChild(int32_t index, CXFA_Node* pNode) {
  ASSERT(!pNode->m_pNext);
  pNode->m_pParent = this;
  FX_BOOL ret = m_pDocument->RemovePurgeNode(pNode);
  ASSERT(ret);
  (void)ret;  // Avoid unused variable warning.

  if (!m_pChild || index == 0) {
    if (index > 0) {
      return -1;
    }
    pNode->m_pNext = m_pChild;
    m_pChild = pNode;
    index = 0;
  } else if (index < 0) {
    m_pLastChild->m_pNext = pNode;
  } else {
    CXFA_Node* pPrev = m_pChild;
    int32_t iCount = 0;
    while (++iCount != index && pPrev->m_pNext) {
      pPrev = pPrev->m_pNext;
    }
    if (index > 0 && index != iCount) {
      return -1;
    }
    pNode->m_pNext = pPrev->m_pNext;
    pPrev->m_pNext = pNode;
    index = iCount;
  }
  if (!pNode->m_pNext) {
    m_pLastChild = pNode;
  }
  ASSERT(m_pLastChild);
  ASSERT(!m_pLastChild->m_pNext);
  pNode->ClearFlag(XFA_NodeFlag_HasRemovedChildren);
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify)
    pNotify->OnChildAdded(this);

  if (IsNeedSavingXMLNode() && pNode->m_pXMLNode) {
    ASSERT(!pNode->m_pXMLNode->GetNodeItem(CFDE_XMLNode::Parent));
    m_pXMLNode->InsertChildNode(pNode->m_pXMLNode, index);
    pNode->ClearFlag(XFA_NodeFlag_OwnXMLNode);
  }
  return index;
}

FX_BOOL CXFA_Node::InsertChild(CXFA_Node* pNode, CXFA_Node* pBeforeNode) {
  if (!pNode || pNode->m_pParent ||
      (pBeforeNode && pBeforeNode->m_pParent != this)) {
    ASSERT(false);
    return FALSE;
  }
  FX_BOOL ret = m_pDocument->RemovePurgeNode(pNode);
  ASSERT(ret);
  (void)ret;  // Avoid unused variable warning.

  int32_t nIndex = -1;
  pNode->m_pParent = this;
  if (!m_pChild || pBeforeNode == m_pChild) {
    pNode->m_pNext = m_pChild;
    m_pChild = pNode;
    nIndex = 0;
  } else if (!pBeforeNode) {
    pNode->m_pNext = m_pLastChild->m_pNext;
    m_pLastChild->m_pNext = pNode;
  } else {
    nIndex = 1;
    CXFA_Node* pPrev = m_pChild;
    while (pPrev->m_pNext != pBeforeNode) {
      pPrev = pPrev->m_pNext;
      nIndex++;
    }
    pNode->m_pNext = pPrev->m_pNext;
    pPrev->m_pNext = pNode;
  }
  if (!pNode->m_pNext) {
    m_pLastChild = pNode;
  }
  ASSERT(m_pLastChild);
  ASSERT(!m_pLastChild->m_pNext);
  pNode->ClearFlag(XFA_NodeFlag_HasRemovedChildren);
  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify)
    pNotify->OnChildAdded(this);

  if (IsNeedSavingXMLNode() && pNode->m_pXMLNode) {
    ASSERT(!pNode->m_pXMLNode->GetNodeItem(CFDE_XMLNode::Parent));
    m_pXMLNode->InsertChildNode(pNode->m_pXMLNode, nIndex);
    pNode->ClearFlag(XFA_NodeFlag_OwnXMLNode);
  }
  return TRUE;
}
CXFA_Node* CXFA_Node::Deprecated_GetPrevSibling() {
  if (!m_pParent) {
    return nullptr;
  }
  for (CXFA_Node* pSibling = m_pParent->m_pChild; pSibling;
       pSibling = pSibling->m_pNext) {
    if (pSibling->m_pNext == this) {
      return pSibling;
    }
  }
  return nullptr;
}
FX_BOOL CXFA_Node::RemoveChild(CXFA_Node* pNode, bool bNotify) {
  if (!pNode || pNode->m_pParent != this) {
    ASSERT(FALSE);
    return FALSE;
  }
  if (m_pChild == pNode) {
    m_pChild = pNode->m_pNext;
    if (m_pLastChild == pNode) {
      m_pLastChild = pNode->m_pNext;
    }
    pNode->m_pNext = nullptr;
    pNode->m_pParent = nullptr;
  } else {
    CXFA_Node* pPrev = pNode->Deprecated_GetPrevSibling();
    pPrev->m_pNext = pNode->m_pNext;
    if (m_pLastChild == pNode) {
      m_pLastChild = pNode->m_pNext ? pNode->m_pNext : pPrev;
    }
    pNode->m_pNext = nullptr;
    pNode->m_pParent = nullptr;
  }
  ASSERT(!m_pLastChild || !m_pLastChild->m_pNext);
  OnRemoved(bNotify);
  pNode->SetFlag(XFA_NodeFlag_HasRemovedChildren, true);
  m_pDocument->AddPurgeNode(pNode);
  if (IsNeedSavingXMLNode() && pNode->m_pXMLNode) {
    if (pNode->IsAttributeInXML()) {
      ASSERT(pNode->m_pXMLNode == m_pXMLNode &&
             m_pXMLNode->GetType() == FDE_XMLNODE_Element);
      if (pNode->m_pXMLNode->GetType() == FDE_XMLNODE_Element) {
        CFDE_XMLElement* pXMLElement =
            static_cast<CFDE_XMLElement*>(pNode->m_pXMLNode);
        CFX_WideStringC wsAttributeName =
            pNode->GetCData(XFA_ATTRIBUTE_QualifiedName);
        pXMLElement->RemoveAttribute(wsAttributeName.c_str());
      }
      CFX_WideString wsName;
      pNode->GetAttribute(XFA_ATTRIBUTE_Name, wsName, FALSE);
      CFDE_XMLElement* pNewXMLElement = new CFDE_XMLElement(wsName);
      CFX_WideStringC wsValue = GetCData(XFA_ATTRIBUTE_Value);
      if (!wsValue.IsEmpty()) {
        pNewXMLElement->SetTextData(CFX_WideString(wsValue));
      }
      pNode->m_pXMLNode = pNewXMLElement;
      pNode->SetEnum(XFA_ATTRIBUTE_Contains, XFA_ATTRIBUTEENUM_Unknown);
    } else {
      m_pXMLNode->RemoveChildNode(pNode->m_pXMLNode);
    }
    pNode->SetFlag(XFA_NodeFlag_OwnXMLNode, false);
  }
  return TRUE;
}
CXFA_Node* CXFA_Node::GetFirstChildByName(const CFX_WideStringC& wsName) const {
  return GetFirstChildByName(FX_HashCode_GetW(wsName, false));
}
CXFA_Node* CXFA_Node::GetFirstChildByName(uint32_t dwNameHash) const {
  for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_FirstChild); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetNameHash() == dwNameHash) {
      return pNode;
    }
  }
  return nullptr;
}
CXFA_Node* CXFA_Node::GetFirstChildByClass(XFA_Element eType) const {
  for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_FirstChild); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() == eType) {
      return pNode;
    }
  }
  return nullptr;
}
CXFA_Node* CXFA_Node::GetNextSameNameSibling(uint32_t dwNameHash) const {
  for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_NextSibling); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetNameHash() == dwNameHash) {
      return pNode;
    }
  }
  return nullptr;
}
CXFA_Node* CXFA_Node::GetNextSameNameSibling(
    const CFX_WideStringC& wsNodeName) const {
  return GetNextSameNameSibling(FX_HashCode_GetW(wsNodeName, false));
}
CXFA_Node* CXFA_Node::GetNextSameClassSibling(XFA_Element eType) const {
  for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_NextSibling); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() == eType) {
      return pNode;
    }
  }
  return nullptr;
}
int32_t CXFA_Node::GetNodeSameNameIndex() const {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext) {
    return -1;
  }
  return pScriptContext->GetIndexByName(const_cast<CXFA_Node*>(this));
}
int32_t CXFA_Node::GetNodeSameClassIndex() const {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext) {
    return -1;
  }
  return pScriptContext->GetIndexByClassName(const_cast<CXFA_Node*>(this));
}
void CXFA_Node::GetSOMExpression(CFX_WideString& wsSOMExpression) {
  CXFA_ScriptContext* pScriptContext = m_pDocument->GetScriptContext();
  if (!pScriptContext) {
    return;
  }
  pScriptContext->GetSomExpression(this, wsSOMExpression);
}
CXFA_Node* CXFA_Node::GetInstanceMgrOfSubform() {
  CXFA_Node* pInstanceMgr = nullptr;
  if (m_ePacket == XFA_XDPPACKET_Form) {
    CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
    if (!pParentNode || pParentNode->GetElementType() == XFA_Element::Area) {
      return pInstanceMgr;
    }
    for (CXFA_Node* pNode = GetNodeItem(XFA_NODEITEM_PrevSibling); pNode;
         pNode = pNode->GetNodeItem(XFA_NODEITEM_PrevSibling)) {
      XFA_Element eType = pNode->GetElementType();
      if ((eType == XFA_Element::Subform || eType == XFA_Element::SubformSet) &&
          pNode->m_dwNameHash != m_dwNameHash) {
        break;
      }
      if (eType == XFA_Element::InstanceManager) {
        CFX_WideStringC wsName = GetCData(XFA_ATTRIBUTE_Name);
        CFX_WideStringC wsInstName = pNode->GetCData(XFA_ATTRIBUTE_Name);
        if (wsInstName.GetLength() > 0 && wsInstName.GetAt(0) == '_' &&
            wsInstName.Mid(1) == wsName) {
          pInstanceMgr = pNode;
        }
        break;
      }
    }
  }
  return pInstanceMgr;
}
CXFA_Node* CXFA_Node::GetOccurNode() {
  return GetFirstChildByClass(XFA_Element::Occur);
}
bool CXFA_Node::HasFlag(XFA_NodeFlag dwFlag) const {
  if (m_uNodeFlags & dwFlag)
    return true;
  if (dwFlag == XFA_NodeFlag_HasRemovedChildren)
    return m_pParent && m_pParent->HasFlag(dwFlag);
  return false;
}

void CXFA_Node::SetFlag(uint32_t dwFlag, bool bNotify) {
  if (dwFlag == XFA_NodeFlag_Initialized && bNotify && !IsInitialized()) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (pNotify) {
      pNotify->OnNodeReady(this);
    }
  }
  m_uNodeFlags |= dwFlag;
}

void CXFA_Node::ClearFlag(uint32_t dwFlag) {
  m_uNodeFlags &= ~dwFlag;
}

FX_BOOL CXFA_Node::IsAttributeInXML() {
  return GetEnum(XFA_ATTRIBUTE_Contains) == XFA_ATTRIBUTEENUM_MetaData;
}

void CXFA_Node::OnRemoved(bool bNotify) {
  if (!bNotify)
    return;

  CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
  if (pNotify)
    pNotify->OnChildRemoved();
}

void CXFA_Node::OnChanging(XFA_ATTRIBUTE eAttr, bool bNotify) {
  if (bNotify && IsInitialized()) {
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (pNotify) {
      pNotify->OnValueChanging(this, eAttr);
    }
  }
}

void CXFA_Node::OnChanged(XFA_ATTRIBUTE eAttr,
                          bool bNotify,
                          FX_BOOL bScriptModify) {
  if (bNotify && IsInitialized()) {
    Script_Attribute_SendAttributeChangeMessage(eAttr, bScriptModify);
  }
}

int32_t CXFA_Node::execSingleEventByName(const CFX_WideStringC& wsEventName,
                                         XFA_Element eType) {
  int32_t iRet = XFA_EVENTERROR_NotExist;
  const XFA_ExecEventParaInfo* eventParaInfo =
      GetEventParaInfoByName(wsEventName);
  if (eventParaInfo) {
    uint32_t validFlags = eventParaInfo->m_validFlags;
    CXFA_FFNotify* pNotify = m_pDocument->GetNotify();
    if (!pNotify) {
      return iRet;
    }
    if (validFlags == 1) {
      iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType);
    } else if (validFlags == 2) {
      iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                           FALSE, FALSE);
    } else if (validFlags == 3) {
      if (eType == XFA_Element::Subform) {
        iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                             FALSE, FALSE);
      }
    } else if (validFlags == 4) {
      if (eType == XFA_Element::ExclGroup || eType == XFA_Element::Field) {
        CXFA_Node* pParentNode = GetNodeItem(XFA_NODEITEM_Parent);
        if (pParentNode &&
            pParentNode->GetElementType() == XFA_Element::ExclGroup) {
          iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                               FALSE, FALSE);
        }
        iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                             FALSE, FALSE);
      }
    } else if (validFlags == 5) {
      if (eType == XFA_Element::Field) {
        iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                             FALSE, FALSE);
      }
    } else if (validFlags == 6) {
      CXFA_WidgetData* pWidgetData = GetWidgetData();
      if (pWidgetData) {
        CXFA_Node* pUINode = pWidgetData->GetUIChild();
        if (pUINode->m_elementType == XFA_Element::Signature) {
          iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                               FALSE, FALSE);
        }
      }
    } else if (validFlags == 7) {
      CXFA_WidgetData* pWidgetData = GetWidgetData();
      if (pWidgetData) {
        CXFA_Node* pUINode = pWidgetData->GetUIChild();
        if ((pUINode->m_elementType == XFA_Element::ChoiceList) &&
            (!pWidgetData->IsListBox())) {
          iRet = pNotify->ExecEventByDeepFirst(this, eventParaInfo->m_eventType,
                                               FALSE, FALSE);
        }
      }
    }
  }
  return iRet;
}
void CXFA_Node::UpdateNameHash() {
  const XFA_NOTSUREATTRIBUTE* pNotsure =
      XFA_GetNotsureAttribute(GetElementType(), XFA_ATTRIBUTE_Name);
  CFX_WideStringC wsName;
  if (!pNotsure || pNotsure->eType == XFA_ATTRIBUTETYPE_Cdata) {
    wsName = GetCData(XFA_ATTRIBUTE_Name);
    m_dwNameHash = FX_HashCode_GetW(wsName, false);
  } else if (pNotsure->eType == XFA_ATTRIBUTETYPE_Enum) {
    wsName = XFA_GetAttributeEnumByID(GetEnum(XFA_ATTRIBUTE_Name))->pName;
    m_dwNameHash = FX_HashCode_GetW(wsName, false);
  }
}
CFDE_XMLNode* CXFA_Node::CreateXMLMappingNode() {
  if (!m_pXMLNode) {
    CFX_WideString wsTag(GetCData(XFA_ATTRIBUTE_Name));
    m_pXMLNode = new CFDE_XMLElement(wsTag);
    SetFlag(XFA_NodeFlag_OwnXMLNode, false);
  }
  return m_pXMLNode;
}
FX_BOOL CXFA_Node::IsNeedSavingXMLNode() {
  return m_pXMLNode && (GetPacketID() == XFA_XDPPACKET_Datasets ||
                        GetElementType() == XFA_Element::Xfa);
}

XFA_MAPMODULEDATA* CXFA_Node::CreateMapModuleData() {
  if (!m_pMapModuleData)
    m_pMapModuleData = new XFA_MAPMODULEDATA;
  return m_pMapModuleData;
}

XFA_MAPMODULEDATA* CXFA_Node::GetMapModuleData() const {
  return m_pMapModuleData;
}

void CXFA_Node::SetMapModuleValue(void* pKey, void* pValue) {
  XFA_MAPMODULEDATA* pModule = CreateMapModuleData();
  pModule->m_ValueMap.SetAt(pKey, pValue);
}

FX_BOOL CXFA_Node::GetMapModuleValue(void* pKey, void*& pValue) {
  CXFA_Node* pNode = this;
  while (pNode) {
    XFA_MAPMODULEDATA* pModule = pNode->GetMapModuleData();
    if (pModule && pModule->m_ValueMap.Lookup(pKey, pValue)) {
      return TRUE;
    }
    pNode = pNode->GetPacketID() != XFA_XDPPACKET_Datasets
                ? pNode->GetTemplateNode()
                : nullptr;
  }
  return FALSE;
}
void CXFA_Node::SetMapModuleString(void* pKey, const CFX_WideStringC& wsValue) {
  SetMapModuleBuffer(pKey, (void*)wsValue.c_str(),
                     wsValue.GetLength() * sizeof(FX_WCHAR));
}
FX_BOOL CXFA_Node::GetMapModuleString(void* pKey, CFX_WideStringC& wsValue) {
  void* pValue;
  int32_t iBytes;
  if (!GetMapModuleBuffer(pKey, pValue, iBytes)) {
    return FALSE;
  }
  wsValue = CFX_WideStringC((const FX_WCHAR*)pValue, iBytes / sizeof(FX_WCHAR));
  return TRUE;
}
void CXFA_Node::SetMapModuleBuffer(
    void* pKey,
    void* pValue,
    int32_t iBytes,
    XFA_MAPDATABLOCKCALLBACKINFO* pCallbackInfo) {
  XFA_MAPMODULEDATA* pModule = CreateMapModuleData();
  XFA_MAPDATABLOCK*& pBuffer = pModule->m_BufferMap[pKey];
  if (!pBuffer) {
    pBuffer =
        (XFA_MAPDATABLOCK*)FX_Alloc(uint8_t, sizeof(XFA_MAPDATABLOCK) + iBytes);
  } else if (pBuffer->iBytes != iBytes) {
    if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
      pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
    }
    pBuffer = (XFA_MAPDATABLOCK*)FX_Realloc(uint8_t, pBuffer,
                                            sizeof(XFA_MAPDATABLOCK) + iBytes);
  } else if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
    pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
  }
  if (!pBuffer)
    return;

  pBuffer->pCallbackInfo = pCallbackInfo;
  pBuffer->iBytes = iBytes;
  FXSYS_memcpy(pBuffer->GetData(), pValue, iBytes);
}
FX_BOOL CXFA_Node::GetMapModuleBuffer(void* pKey,
                                      void*& pValue,
                                      int32_t& iBytes,
                                      FX_BOOL bProtoAlso) const {
  XFA_MAPDATABLOCK* pBuffer = nullptr;
  const CXFA_Node* pNode = this;
  while (pNode) {
    XFA_MAPMODULEDATA* pModule = pNode->GetMapModuleData();
    if (pModule && pModule->m_BufferMap.Lookup(pKey, pBuffer)) {
      break;
    }
    pNode = (bProtoAlso && pNode->GetPacketID() != XFA_XDPPACKET_Datasets)
                ? pNode->GetTemplateNode()
                : nullptr;
  }
  if (!pBuffer) {
    return FALSE;
  }
  pValue = pBuffer->GetData();
  iBytes = pBuffer->iBytes;
  return TRUE;
}
FX_BOOL CXFA_Node::HasMapModuleKey(void* pKey, FX_BOOL bProtoAlso) {
  CXFA_Node* pNode = this;
  while (pNode) {
    void* pVal;
    XFA_MAPMODULEDATA* pModule = pNode->GetMapModuleData();
    if (pModule &&
        (pModule->m_ValueMap.Lookup(pKey, pVal) ||
         pModule->m_BufferMap.Lookup(pKey, (XFA_MAPDATABLOCK*&)pVal))) {
      return TRUE;
    }
    pNode = (bProtoAlso && pNode->GetPacketID() != XFA_XDPPACKET_Datasets)
                ? pNode->GetTemplateNode()
                : nullptr;
  }
  return FALSE;
}
void CXFA_Node::RemoveMapModuleKey(void* pKey) {
  XFA_MAPMODULEDATA* pModule = GetMapModuleData();
  if (!pModule)
    return;

  if (pKey) {
    XFA_MAPDATABLOCK* pBuffer = nullptr;
    pModule->m_BufferMap.Lookup(pKey, pBuffer);
    if (pBuffer) {
      if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
        pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
      }
      FX_Free(pBuffer);
    }
    pModule->m_BufferMap.RemoveKey(pKey);
    pModule->m_ValueMap.RemoveKey(pKey);
  } else {
    XFA_MAPDATABLOCK* pBuffer;
    FX_POSITION posBuffer = pModule->m_BufferMap.GetStartPosition();
    while (posBuffer) {
      pModule->m_BufferMap.GetNextAssoc(posBuffer, pKey, pBuffer);
      if (pBuffer) {
        if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
          pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
        }
        FX_Free(pBuffer);
      }
    }
    pModule->m_BufferMap.RemoveAll();
    pModule->m_ValueMap.RemoveAll();
    delete pModule;
  }
}
void CXFA_Node::MergeAllData(void* pDstModule, FX_BOOL bUseSrcAttr) {
  XFA_MAPMODULEDATA* pDstModuleData =
      static_cast<CXFA_Node*>(pDstModule)->CreateMapModuleData();
  XFA_MAPMODULEDATA* pSrcModuleData = GetMapModuleData();
  if (!pSrcModuleData) {
    return;
  }
  FX_POSITION psValue = pSrcModuleData->m_ValueMap.GetStartPosition();
  while (psValue) {
    void* pKey;
    void* pValue;
    pSrcModuleData->m_ValueMap.GetNextAssoc(psValue, pKey, pValue);
    if (bUseSrcAttr || !pDstModuleData->m_ValueMap.GetValueAt(pKey)) {
      pDstModuleData->m_ValueMap.SetAt(pKey, pValue);
    }
  }
  FX_POSITION psBuffer = pSrcModuleData->m_BufferMap.GetStartPosition();
  while (psBuffer) {
    void* pKey;
    XFA_MAPDATABLOCK* pSrcBuffer;
    pSrcModuleData->m_BufferMap.GetNextAssoc(psBuffer, pKey, pSrcBuffer);
    XFA_MAPDATABLOCK*& pBuffer = pDstModuleData->m_BufferMap[pKey];
    if (pBuffer && !bUseSrcAttr) {
      continue;
    }
    if (pSrcBuffer->pCallbackInfo && pSrcBuffer->pCallbackInfo->pFree &&
        !pSrcBuffer->pCallbackInfo->pCopy) {
      if (pBuffer) {
        pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
        pDstModuleData->m_BufferMap.RemoveKey(pKey);
      }
      continue;
    }
    if (!pBuffer) {
      pBuffer = (XFA_MAPDATABLOCK*)FX_Alloc(
          uint8_t, sizeof(XFA_MAPDATABLOCK) + pSrcBuffer->iBytes);
    } else if (pBuffer->iBytes != pSrcBuffer->iBytes) {
      if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
        pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
      }
      pBuffer = (XFA_MAPDATABLOCK*)FX_Realloc(
          uint8_t, pBuffer, sizeof(XFA_MAPDATABLOCK) + pSrcBuffer->iBytes);
    } else if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pFree) {
      pBuffer->pCallbackInfo->pFree(*(void**)pBuffer->GetData());
    }
    if (!pBuffer) {
      continue;
    }
    pBuffer->pCallbackInfo = pSrcBuffer->pCallbackInfo;
    pBuffer->iBytes = pSrcBuffer->iBytes;
    FXSYS_memcpy(pBuffer->GetData(), pSrcBuffer->GetData(), pSrcBuffer->iBytes);
    if (pBuffer->pCallbackInfo && pBuffer->pCallbackInfo->pCopy) {
      pBuffer->pCallbackInfo->pCopy(*(void**)pBuffer->GetData());
    }
  }
}
void CXFA_Node::MoveBufferMapData(CXFA_Node* pDstModule, void* pKey) {
  if (!pDstModule) {
    return;
  }
  FX_BOOL bNeedMove = TRUE;
  if (!pKey) {
    bNeedMove = FALSE;
  }
  if (pDstModule->GetElementType() != GetElementType()) {
    bNeedMove = FALSE;
  }
  XFA_MAPMODULEDATA* pSrcModuleData = nullptr;
  XFA_MAPMODULEDATA* pDstModuleData = nullptr;
  if (bNeedMove) {
    pSrcModuleData = GetMapModuleData();
    if (!pSrcModuleData) {
      bNeedMove = FALSE;
    }
    pDstModuleData = pDstModule->CreateMapModuleData();
  }
  if (bNeedMove) {
    void* pBufferBlockData = pSrcModuleData->m_BufferMap.GetValueAt(pKey);
    if (pBufferBlockData) {
      pSrcModuleData->m_BufferMap.RemoveKey(pKey);
      pDstModuleData->m_BufferMap.RemoveKey(pKey);
      pDstModuleData->m_BufferMap.SetAt(pKey,
                                        (XFA_MAPDATABLOCK*)pBufferBlockData);
    }
  }
  if (pDstModule->IsNodeV()) {
    CFX_WideString wsValue = pDstModule->GetScriptContent(FALSE);
    CFX_WideString wsFormatValue(wsValue);
    CXFA_WidgetData* pWidgetData = pDstModule->GetContainerWidgetData();
    if (pWidgetData) {
      pWidgetData->GetFormatDataValue(wsValue, wsFormatValue);
    }
    pDstModule->SetScriptContent(wsValue, wsFormatValue, true, TRUE);
  }
}
void CXFA_Node::MoveBufferMapData(CXFA_Node* pSrcModule,
                                  CXFA_Node* pDstModule,
                                  void* pKey,
                                  FX_BOOL bRecursive) {
  if (!pSrcModule || !pDstModule || !pKey) {
    return;
  }
  if (bRecursive) {
    CXFA_Node* pSrcChild = pSrcModule->GetNodeItem(XFA_NODEITEM_FirstChild);
    CXFA_Node* pDstChild = pDstModule->GetNodeItem(XFA_NODEITEM_FirstChild);
    for (; pSrcChild && pDstChild;
         pSrcChild = pSrcChild->GetNodeItem(XFA_NODEITEM_NextSibling),
         pDstChild = pDstChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      MoveBufferMapData(pSrcChild, pDstChild, pKey, TRUE);
    }
  }
  pSrcModule->MoveBufferMapData(pDstModule, pKey);
}

CXFA_ThisProxy::CXFA_ThisProxy(CXFA_Node* pThisNode, CXFA_Node* pScriptNode)
    : CXFA_Object(pThisNode->GetDocument(),
                  XFA_ObjectType::VariablesThis,
                  XFA_Element::Unknown),
      m_pThisNode(nullptr),
      m_pScriptNode(nullptr) {
  m_pThisNode = pThisNode;
  m_pScriptNode = pScriptNode;
}

CXFA_ThisProxy::~CXFA_ThisProxy() {}

CXFA_Node* CXFA_ThisProxy::GetThisNode() const {
  return m_pThisNode;
}

CXFA_Node* CXFA_ThisProxy::GetScriptNode() const {
  return m_pScriptNode;
}

CXFA_NodeList::CXFA_NodeList(CXFA_Document* pDocument)
    : CXFA_Object(pDocument, XFA_ObjectType::NodeList, XFA_Element::NodeList) {
  m_pDocument->GetScriptContext()->AddToCacheList(
      std::unique_ptr<CXFA_NodeList>(this));
}

CXFA_NodeList::~CXFA_NodeList() {}

CXFA_Node* CXFA_NodeList::NamedItem(const CFX_WideStringC& wsName) {
  uint32_t dwHashCode = FX_HashCode_GetW(wsName, false);
  int32_t iCount = GetLength();
  for (int32_t i = 0; i < iCount; i++) {
    CXFA_Node* ret = Item(i);
    if (dwHashCode == ret->GetNameHash())
      return ret;
  }
  return nullptr;
}
void CXFA_NodeList::Script_ListClass_Append(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CXFA_Node* pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
    if (pNode) {
      Append(pNode);
    } else {
      ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"append");
  }
}
void CXFA_NodeList::Script_ListClass_Insert(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 2) {
    CXFA_Node* pNewNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
    CXFA_Node* pBeforeNode = static_cast<CXFA_Node*>(pArguments->GetObject(1));
    if (pNewNode) {
      Insert(pNewNode, pBeforeNode);
    } else {
      ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"insert");
  }
}
void CXFA_NodeList::Script_ListClass_Remove(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CXFA_Node* pNode = static_cast<CXFA_Node*>(pArguments->GetObject(0));
    if (pNode) {
      Remove(pNode);
    } else {
      ThrowException(XFA_IDS_ARGUMENT_MISMATCH);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"remove");
  }
}
void CXFA_NodeList::Script_ListClass_Item(CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    int32_t iIndex = pArguments->GetInt32(0);
    if ((iIndex >= 0) && (iIndex + 1 <= GetLength())) {
      pArguments->GetReturnValue()->Assign(
          m_pDocument->GetScriptContext()->GetJSValueFromMap(Item(iIndex)));
    } else {
      ThrowException(XFA_IDS_INDEX_OUT_OF_BOUNDS);
    }
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"item");
  }
}
void CXFA_NodeList::Script_TreelistClass_NamedItem(
    CFXJSE_Arguments* pArguments) {
  int32_t argc = pArguments->GetLength();
  if (argc == 1) {
    CFX_ByteString szName = pArguments->GetUTF8String(0);
    CXFA_Node* pNode =
        NamedItem(CFX_WideString::FromUTF8(szName.AsStringC()).AsStringC());
    if (!pNode) {
      return;
    }
    pArguments->GetReturnValue()->Assign(
        m_pDocument->GetScriptContext()->GetJSValueFromMap(pNode));
  } else {
    ThrowException(XFA_IDS_INCORRECT_NUMBER_OF_METHOD, L"namedItem");
  }
}
void CXFA_NodeList::Script_ListClass_Length(CFXJSE_Value* pValue,
                                            FX_BOOL bSetting,
                                            XFA_ATTRIBUTE eAttribute) {
  if (!bSetting) {
    pValue->SetInteger(GetLength());
  } else {
    ThrowException(XFA_IDS_INVAlID_PROP_SET);
  }
}
CXFA_ArrayNodeList::CXFA_ArrayNodeList(CXFA_Document* pDocument)
    : CXFA_NodeList(pDocument) {}

CXFA_ArrayNodeList::~CXFA_ArrayNodeList() {}

void CXFA_ArrayNodeList::SetArrayNodeList(const CXFA_NodeArray& srcArray) {
  if (srcArray.GetSize() > 0) {
    m_array.Copy(srcArray);
  }
}
int32_t CXFA_ArrayNodeList::GetLength() {
  return m_array.GetSize();
}
FX_BOOL CXFA_ArrayNodeList::Append(CXFA_Node* pNode) {
  m_array.Add(pNode);
  return TRUE;
}
FX_BOOL CXFA_ArrayNodeList::Insert(CXFA_Node* pNewNode,
                                   CXFA_Node* pBeforeNode) {
  if (!pBeforeNode) {
    m_array.Add(pNewNode);
  } else {
    int32_t iSize = m_array.GetSize();
    for (int32_t i = 0; i < iSize; ++i) {
      if (m_array[i] == pBeforeNode) {
        m_array.InsertAt(i, pNewNode);
        break;
      }
    }
  }
  return TRUE;
}
FX_BOOL CXFA_ArrayNodeList::Remove(CXFA_Node* pNode) {
  int32_t iSize = m_array.GetSize();
  for (int32_t i = 0; i < iSize; ++i) {
    if (m_array[i] == pNode) {
      m_array.RemoveAt(i);
      break;
    }
  }
  return TRUE;
}
CXFA_Node* CXFA_ArrayNodeList::Item(int32_t iIndex) {
  int32_t iSize = m_array.GetSize();
  if (iIndex >= 0 && iIndex < iSize) {
    return m_array[iIndex];
  }
  return nullptr;
}
CXFA_AttachNodeList::CXFA_AttachNodeList(CXFA_Document* pDocument,
                                         CXFA_Node* pAttachNode)
    : CXFA_NodeList(pDocument) {
  m_pAttachNode = pAttachNode;
}
int32_t CXFA_AttachNodeList::GetLength() {
  return m_pAttachNode->CountChildren(
      XFA_Element::Unknown,
      m_pAttachNode->GetElementType() == XFA_Element::Subform);
}
FX_BOOL CXFA_AttachNodeList::Append(CXFA_Node* pNode) {
  CXFA_Node* pParent = pNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (pParent) {
    pParent->RemoveChild(pNode);
  }
  return m_pAttachNode->InsertChild(pNode);
}
FX_BOOL CXFA_AttachNodeList::Insert(CXFA_Node* pNewNode,
                                    CXFA_Node* pBeforeNode) {
  CXFA_Node* pParent = pNewNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (pParent) {
    pParent->RemoveChild(pNewNode);
  }
  return m_pAttachNode->InsertChild(pNewNode, pBeforeNode);
}
FX_BOOL CXFA_AttachNodeList::Remove(CXFA_Node* pNode) {
  return m_pAttachNode->RemoveChild(pNode);
}
CXFA_Node* CXFA_AttachNodeList::Item(int32_t iIndex) {
  return m_pAttachNode->GetChild(
      iIndex, XFA_Element::Unknown,
      m_pAttachNode->GetElementType() == XFA_Element::Subform);
}
