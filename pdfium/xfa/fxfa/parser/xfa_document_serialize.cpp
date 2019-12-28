// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_document_serialize.h"

#include "xfa/fde/xml/fde_xml_imp.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fxfa/parser/cxfa_simple_parser.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_DataImporter::CXFA_DataImporter(CXFA_Document* pDocument)
    : m_pDocument(pDocument) {
  ASSERT(m_pDocument);
}
FX_BOOL CXFA_DataImporter::ImportData(IFX_FileRead* pDataDocument) {
  std::unique_ptr<CXFA_SimpleParser> pDataDocumentParser(
      new CXFA_SimpleParser(m_pDocument, false));
  if (!pDataDocumentParser)
    return FALSE;

  if (pDataDocumentParser->StartParse(pDataDocument, XFA_XDPPACKET_Datasets) !=
      XFA_PARSESTATUS_Ready) {
    return FALSE;
  }
  if (pDataDocumentParser->DoParse(nullptr) < XFA_PARSESTATUS_Done)
    return FALSE;

  CXFA_Node* pImportDataRoot = pDataDocumentParser->GetRootNode();
  if (!pImportDataRoot)
    return FALSE;

  CXFA_Node* pDataModel =
      ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Datasets));
  if (!pDataModel)
    return FALSE;

  CXFA_Node* pDataNode = ToNode(m_pDocument->GetXFAObject(XFA_HASHCODE_Data));
  if (pDataNode)
    pDataModel->RemoveChild(pDataNode);

  if (pImportDataRoot->GetElementType() == XFA_Element::DataModel) {
    while (CXFA_Node* pChildNode =
               pImportDataRoot->GetNodeItem(XFA_NODEITEM_FirstChild)) {
      pImportDataRoot->RemoveChild(pChildNode);
      pDataModel->InsertChild(pChildNode);
    }
  } else {
    CFDE_XMLNode* pXMLNode = pImportDataRoot->GetXMLMappingNode();
    CFDE_XMLNode* pParentXMLNode = pXMLNode->GetNodeItem(CFDE_XMLNode::Parent);
    if (pParentXMLNode)
      pParentXMLNode->RemoveChildNode(pXMLNode);
    pDataModel->InsertChild(pImportDataRoot);
  }
  m_pDocument->DoDataRemerge(FALSE);
  return TRUE;
}

CFX_WideString XFA_ExportEncodeAttribute(const CFX_WideString& str) {
  CFX_WideTextBuf textBuf;
  int32_t iLen = str.GetLength();
  for (int32_t i = 0; i < iLen; i++) {
    switch (str[i]) {
      case '&':
        textBuf << FX_WSTRC(L"&amp;");
        break;
      case '<':
        textBuf << FX_WSTRC(L"&lt;");
        break;
      case '>':
        textBuf << FX_WSTRC(L"&gt;");
        break;
      case '\'':
        textBuf << FX_WSTRC(L"&apos;");
        break;
      case '\"':
        textBuf << FX_WSTRC(L"&quot;");
        break;
      default:
        textBuf.AppendChar(str[i]);
    }
  }
  return textBuf.MakeString();
}
CFX_WideString XFA_ExportEncodeContent(const CFX_WideStringC& str) {
  CFX_WideTextBuf textBuf;
  int32_t iLen = str.GetLength();
  for (int32_t i = 0; i < iLen; i++) {
    FX_WCHAR ch = str.GetAt(i);
    if (!FDE_IsXMLValidChar(ch)) {
      continue;
    }
    if (ch == '&') {
      textBuf << FX_WSTRC(L"&amp;");
    } else if (ch == '<') {
      textBuf << FX_WSTRC(L"&lt;");
    } else if (ch == '>') {
      textBuf << FX_WSTRC(L"&gt;");
    } else if (ch == '\'') {
      textBuf << FX_WSTRC(L"&apos;");
    } else if (ch == '\"') {
      textBuf << FX_WSTRC(L"&quot;");
    } else if (ch == ' ') {
      if (i && str.GetAt(i - 1) != ' ') {
        textBuf.AppendChar(' ');
      } else {
        textBuf << FX_WSTRC(L"&#x20;");
      }
    } else {
      textBuf.AppendChar(str.GetAt(i));
    }
  }
  return textBuf.MakeString();
}
static void XFA_SaveAttribute(CXFA_Node* pNode,
                              XFA_ATTRIBUTE eName,
                              const CFX_WideStringC& wsName,
                              FX_BOOL bProto,
                              CFX_WideString& wsOutput) {
  CFX_WideString wsValue;
  if ((!bProto && !pNode->HasAttribute((XFA_ATTRIBUTE)eName, bProto)) ||
      !pNode->GetAttribute((XFA_ATTRIBUTE)eName, wsValue, FALSE)) {
    return;
  }
  wsValue = XFA_ExportEncodeAttribute(wsValue);
  wsOutput += FX_WSTRC(L" ");
  wsOutput += wsName;
  wsOutput += FX_WSTRC(L"=\"");
  wsOutput += wsValue;
  wsOutput += FX_WSTRC(L"\"");
}
static FX_BOOL XFA_DataExporter_AttributeSaveInDataModel(
    CXFA_Node* pNode,
    XFA_ATTRIBUTE eAttribute) {
  FX_BOOL bSaveInDataModel = FALSE;
  if (pNode->GetElementType() != XFA_Element::Image) {
    return bSaveInDataModel;
  }
  CXFA_Node* pValueNode = pNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (!pValueNode || pValueNode->GetElementType() != XFA_Element::Value) {
    return bSaveInDataModel;
  }
  CXFA_Node* pFieldNode = pValueNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (pFieldNode && pFieldNode->GetBindData() &&
      eAttribute == XFA_ATTRIBUTE_Href) {
    bSaveInDataModel = TRUE;
  }
  return bSaveInDataModel;
}
FX_BOOL XFA_DataExporter_ContentNodeNeedtoExport(CXFA_Node* pContentNode) {
  CFX_WideString wsContent;
  if (!pContentNode->TryContent(wsContent, FALSE, FALSE))
    return FALSE;

  ASSERT(pContentNode->IsContentNode());
  CXFA_Node* pParentNode = pContentNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (!pParentNode || pParentNode->GetElementType() != XFA_Element::Value)
    return TRUE;

  CXFA_Node* pGrandParentNode = pParentNode->GetNodeItem(XFA_NODEITEM_Parent);
  if (!pGrandParentNode || !pGrandParentNode->IsContainerNode())
    return TRUE;
  if (pGrandParentNode->GetBindData())
    return FALSE;

  CXFA_WidgetData* pWidgetData = pGrandParentNode->GetWidgetData();
  XFA_Element eUIType = pWidgetData->GetUIType();
  if (eUIType == XFA_Element::PasswordEdit) {
    return FALSE;
  }
  return TRUE;
}
static void XFA_DataExporter_RecognizeXFAVersionNumber(
    CXFA_Node* pTemplateRoot,
    CFX_WideString& wsVersionNumber) {
  wsVersionNumber.clear();
  if (!pTemplateRoot) {
    return;
  }
  CFX_WideString wsTemplateNS;
  if (!pTemplateRoot->TryNamespace(wsTemplateNS)) {
    return;
  }
  XFA_VERSION eVersion =
      pTemplateRoot->GetDocument()->RecognizeXFAVersionNumber(wsTemplateNS);
  if (eVersion == XFA_VERSION_UNKNOWN) {
    eVersion = XFA_VERSION_DEFAULT;
  }
  wsVersionNumber.Format(L"%i.%i", eVersion / 100, eVersion % 100);
}
static void XFA_DataExporter_RegenerateFormFile_Changed(
    CXFA_Node* pNode,
    CFX_WideTextBuf& buf,
    FX_BOOL bSaveXML = FALSE) {
  CFX_WideString wsAttrs;
  int32_t iAttrs = 0;
  const uint8_t* pAttrs =
      XFA_GetElementAttributes(pNode->GetElementType(), iAttrs);
  while (iAttrs--) {
    const XFA_ATTRIBUTEINFO* pAttr =
        XFA_GetAttributeByID((XFA_ATTRIBUTE)pAttrs[iAttrs]);
    if (pAttr->eName == XFA_ATTRIBUTE_Name ||
        (XFA_DataExporter_AttributeSaveInDataModel(pNode, pAttr->eName) &&
         !bSaveXML)) {
      continue;
    }
    CFX_WideString wsAttr;
    XFA_SaveAttribute(pNode, pAttr->eName, pAttr->pName, bSaveXML, wsAttr);
    wsAttrs += wsAttr;
  }
  CFX_WideString wsChildren;
  switch (pNode->GetObjectType()) {
    case XFA_ObjectType::ContentNode: {
      if (!bSaveXML && !XFA_DataExporter_ContentNodeNeedtoExport(pNode)) {
        break;
      }
      CXFA_Node* pRawValueNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
      while (pRawValueNode &&
             pRawValueNode->GetElementType() != XFA_Element::SharpxHTML &&
             pRawValueNode->GetElementType() != XFA_Element::Sharptext &&
             pRawValueNode->GetElementType() != XFA_Element::Sharpxml) {
        pRawValueNode = pRawValueNode->GetNodeItem(XFA_NODEITEM_NextSibling);
      }
      if (!pRawValueNode) {
        break;
      }
      CFX_WideString wsContentType;
      pNode->GetAttribute(XFA_ATTRIBUTE_ContentType, wsContentType, FALSE);
      if (pRawValueNode->GetElementType() == XFA_Element::SharpxHTML &&
          wsContentType == FX_WSTRC(L"text/html")) {
        CFDE_XMLNode* pExDataXML = pNode->GetXMLMappingNode();
        if (!pExDataXML) {
          break;
        }
        CFDE_XMLNode* pRichTextXML =
            pExDataXML->GetNodeItem(CFDE_XMLNode::FirstChild);
        if (!pRichTextXML) {
          break;
        }
        IFX_MemoryStream* pMemStream = FX_CreateMemoryStream(TRUE);
        IFX_Stream* pTempStream = IFX_Stream::CreateStream(
            (IFX_FileWrite*)pMemStream, FX_STREAMACCESS_Text |
                                            FX_STREAMACCESS_Write |
                                            FX_STREAMACCESS_Append);
        pTempStream->SetCodePage(FX_CODEPAGE_UTF8);
        pRichTextXML->SaveXMLNode(pTempStream);
        wsChildren += CFX_WideString::FromUTF8(
            CFX_ByteStringC(pMemStream->GetBuffer(), pMemStream->GetSize()));
        pTempStream->Release();
        pMemStream->Release();
      } else if (pRawValueNode->GetElementType() == XFA_Element::Sharpxml &&
                 wsContentType == FX_WSTRC(L"text/xml")) {
        CFX_WideString wsRawValue;
        pRawValueNode->GetAttribute(XFA_ATTRIBUTE_Value, wsRawValue, FALSE);
        if (wsRawValue.IsEmpty()) {
          break;
        }
        CFX_WideStringArray wsSelTextArray;
        int32_t iStart = 0;
        int32_t iEnd = wsRawValue.Find(L'\n', iStart);
        iEnd = (iEnd == -1) ? wsRawValue.GetLength() : iEnd;
        while (iEnd >= iStart) {
          wsSelTextArray.Add(wsRawValue.Mid(iStart, iEnd - iStart));
          iStart = iEnd + 1;
          if (iStart >= wsRawValue.GetLength()) {
            break;
          }
          iEnd = wsRawValue.Find(L'\n', iStart);
        }
        CXFA_Node* pParentNode = pNode->GetNodeItem(XFA_NODEITEM_Parent);
        ASSERT(pParentNode);
        CXFA_Node* pGrandparentNode =
            pParentNode->GetNodeItem(XFA_NODEITEM_Parent);
        ASSERT(pGrandparentNode);
        CFX_WideString bodyTagName;
        bodyTagName = pGrandparentNode->GetCData(XFA_ATTRIBUTE_Name);
        if (bodyTagName.IsEmpty()) {
          bodyTagName = FX_WSTRC(L"ListBox1");
        }
        buf << FX_WSTRC(L"<");
        buf << bodyTagName;
        buf << FX_WSTRC(L" xmlns=\"\"\n>");
        for (int32_t i = 0; i < wsSelTextArray.GetSize(); i++) {
          buf << FX_WSTRC(L"<value\n>");
          buf << XFA_ExportEncodeContent(wsSelTextArray[i].AsStringC());
          buf << FX_WSTRC(L"</value\n>");
        }
        buf << FX_WSTRC(L"</");
        buf << bodyTagName;
        buf << FX_WSTRC(L"\n>");
        wsChildren += buf.AsStringC();
        buf.Clear();
      } else {
        CFX_WideStringC wsValue = pRawValueNode->GetCData(XFA_ATTRIBUTE_Value);
        wsChildren += XFA_ExportEncodeContent(wsValue);
      }
    } break;
    case XFA_ObjectType::TextNode:
    case XFA_ObjectType::NodeC:
    case XFA_ObjectType::NodeV: {
      CFX_WideStringC wsValue = pNode->GetCData(XFA_ATTRIBUTE_Value);
      wsChildren += XFA_ExportEncodeContent(wsValue);
    } break;
    default:
      if (pNode->GetElementType() == XFA_Element::Items) {
        CXFA_Node* pTemplateNode = pNode->GetTemplateNode();
        if (!pTemplateNode ||
            pTemplateNode->CountChildren(XFA_Element::Unknown) !=
                pNode->CountChildren(XFA_Element::Unknown)) {
          bSaveXML = TRUE;
        }
      }
      CFX_WideTextBuf newBuf;
      CXFA_Node* pChildNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
      while (pChildNode) {
        XFA_DataExporter_RegenerateFormFile_Changed(pChildNode, newBuf,
                                                    bSaveXML);
        wsChildren += newBuf.AsStringC();
        newBuf.Clear();
        pChildNode = pChildNode->GetNodeItem(XFA_NODEITEM_NextSibling);
      }
      if (!bSaveXML && !wsChildren.IsEmpty() &&
          pNode->GetElementType() == XFA_Element::Items) {
        wsChildren.clear();
        bSaveXML = TRUE;
        CXFA_Node* pChild = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
        while (pChild) {
          XFA_DataExporter_RegenerateFormFile_Changed(pChild, newBuf, bSaveXML);
          wsChildren += newBuf.AsStringC();
          newBuf.Clear();
          pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
        }
      }
      break;
  }
  if (!wsChildren.IsEmpty() || !wsAttrs.IsEmpty() ||
      pNode->HasAttribute(XFA_ATTRIBUTE_Name)) {
    CFX_WideStringC wsElement = pNode->GetClassName();
    CFX_WideString wsName;
    XFA_SaveAttribute(pNode, XFA_ATTRIBUTE_Name, FX_WSTRC(L"name"), TRUE,
                      wsName);
    buf << FX_WSTRC(L"<");
    buf << wsElement;
    buf << wsName;
    buf << wsAttrs;
    if (wsChildren.IsEmpty()) {
      buf << FX_WSTRC(L"\n/>");
    } else {
      buf << FX_WSTRC(L"\n>");
      buf << wsChildren;
      buf << FX_WSTRC(L"</");
      buf << wsElement;
      buf << FX_WSTRC(L"\n>");
    }
  }
}
static void XFA_DataExporter_RegenerateFormFile_Container(
    CXFA_Node* pNode,
    IFX_Stream* pStream,
    FX_BOOL bSaveXML = FALSE) {
  XFA_Element eType = pNode->GetElementType();
  if (eType == XFA_Element::Field || eType == XFA_Element::Draw ||
      !pNode->IsContainerNode()) {
    CFX_WideTextBuf buf;
    XFA_DataExporter_RegenerateFormFile_Changed(pNode, buf, bSaveXML);
    FX_STRSIZE nLen = buf.GetLength();
    if (nLen > 0) {
      pStream->WriteString((const FX_WCHAR*)buf.GetBuffer(), nLen);
    }
    return;
  }
  CFX_WideStringC wsElement = pNode->GetClassName();
  pStream->WriteString(L"<", 1);
  pStream->WriteString(wsElement.c_str(), wsElement.GetLength());
  CFX_WideString wsOutput;
  XFA_SaveAttribute(pNode, XFA_ATTRIBUTE_Name, FX_WSTRC(L"name"), TRUE,
                    wsOutput);
  CFX_WideString wsAttrs;
  int32_t iAttrs = 0;
  const uint8_t* pAttrs =
      XFA_GetElementAttributes(pNode->GetElementType(), iAttrs);
  while (iAttrs--) {
    const XFA_ATTRIBUTEINFO* pAttr =
        XFA_GetAttributeByID((XFA_ATTRIBUTE)pAttrs[iAttrs]);
    if (pAttr->eName == XFA_ATTRIBUTE_Name) {
      continue;
    }
    CFX_WideString wsAttr;
    XFA_SaveAttribute(pNode, pAttr->eName, pAttr->pName, FALSE, wsAttr);
    wsOutput += wsAttr;
  }
  if (!wsOutput.IsEmpty()) {
    pStream->WriteString(wsOutput.c_str(), wsOutput.GetLength());
  }
  CXFA_Node* pChildNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  if (pChildNode) {
    pStream->WriteString(L"\n>", 2);
    while (pChildNode) {
      XFA_DataExporter_RegenerateFormFile_Container(pChildNode, pStream,
                                                    bSaveXML);
      pChildNode = pChildNode->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
    pStream->WriteString(L"</", 2);
    pStream->WriteString(wsElement.c_str(), wsElement.GetLength());
    pStream->WriteString(L"\n>", 2);
  } else {
    pStream->WriteString(L"\n/>", 3);
  }
}
void XFA_DataExporter_RegenerateFormFile(CXFA_Node* pNode,
                                         IFX_Stream* pStream,
                                         const FX_CHAR* pChecksum,
                                         FX_BOOL bSaveXML) {
  if (pNode->IsModelNode()) {
    static const FX_WCHAR s_pwsTagName[] = L"<form";
    static const FX_WCHAR s_pwsClose[] = L"</form\n>";
    pStream->WriteString(s_pwsTagName, FXSYS_wcslen(s_pwsTagName));
    if (pChecksum) {
      static const FX_WCHAR s_pwChecksum[] = L" checksum=\"";
      CFX_WideString wsChecksum = CFX_WideString::FromUTF8(pChecksum);
      pStream->WriteString(s_pwChecksum, FXSYS_wcslen(s_pwChecksum));
      pStream->WriteString(wsChecksum.c_str(), wsChecksum.GetLength());
      pStream->WriteString(L"\"", 1);
    }
    pStream->WriteString(L" xmlns=\"", FXSYS_wcslen(L" xmlns=\""));
    const FX_WCHAR* pURI = XFA_GetPacketByIndex(XFA_PACKET_Form)->pURI;
    pStream->WriteString(pURI, FXSYS_wcslen(pURI));
    CFX_WideString wsVersionNumber;
    XFA_DataExporter_RecognizeXFAVersionNumber(
        ToNode(pNode->GetDocument()->GetXFAObject(XFA_HASHCODE_Template)),
        wsVersionNumber);
    if (wsVersionNumber.IsEmpty()) {
      wsVersionNumber = FX_WSTRC(L"2.8");
    }
    wsVersionNumber += FX_WSTRC(L"/\"\n>");
    pStream->WriteString(wsVersionNumber.c_str(), wsVersionNumber.GetLength());
    CXFA_Node* pChildNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
    while (pChildNode) {
      XFA_DataExporter_RegenerateFormFile_Container(pChildNode, pStream);
      pChildNode = pChildNode->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
    pStream->WriteString(s_pwsClose, FXSYS_wcslen(s_pwsClose));
  } else {
    XFA_DataExporter_RegenerateFormFile_Container(pNode, pStream, bSaveXML);
  }
}

CXFA_DataExporter::CXFA_DataExporter(CXFA_Document* pDocument)
    : m_pDocument(pDocument) {
  ASSERT(m_pDocument);
}
FX_BOOL CXFA_DataExporter::Export(IFX_FileWrite* pWrite) {
  return Export(pWrite, m_pDocument->GetRoot());
}
FX_BOOL CXFA_DataExporter::Export(IFX_FileWrite* pWrite,
                                  CXFA_Node* pNode,
                                  uint32_t dwFlag,
                                  const FX_CHAR* pChecksum) {
  if (!pWrite) {
    ASSERT(false);
    return FALSE;
  }
  IFX_Stream* pStream = IFX_Stream::CreateStream(
      pWrite,
      FX_STREAMACCESS_Text | FX_STREAMACCESS_Write | FX_STREAMACCESS_Append);
  if (!pStream) {
    return FALSE;
  }
  pStream->SetCodePage(FX_CODEPAGE_UTF8);
  FX_BOOL bRet = Export(pStream, pNode, dwFlag, pChecksum);
  pStream->Release();
  return bRet;
}
FX_BOOL CXFA_DataExporter::Export(IFX_Stream* pStream,
                                  CXFA_Node* pNode,
                                  uint32_t dwFlag,
                                  const FX_CHAR* pChecksum) {
  CFDE_XMLDoc* pXMLDoc = m_pDocument->GetXMLDoc();
  if (pNode->IsModelNode()) {
    switch (pNode->GetPacketID()) {
      case XFA_XDPPACKET_XDP: {
        static const FX_WCHAR s_pwsPreamble[] =
            L"<xdp:xdp xmlns:xdp=\"http://ns.adobe.com/xdp/\">";
        pStream->WriteString(s_pwsPreamble, FXSYS_wcslen(s_pwsPreamble));
        for (CXFA_Node* pChild = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
             pChild; pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
          Export(pStream, pChild, dwFlag, pChecksum);
        }
        static const FX_WCHAR s_pwsPostamble[] = L"</xdp:xdp\n>";
        pStream->WriteString(s_pwsPostamble, FXSYS_wcslen(s_pwsPostamble));
      } break;
      case XFA_XDPPACKET_Datasets: {
        CFDE_XMLElement* pElement =
            static_cast<CFDE_XMLElement*>(pNode->GetXMLMappingNode());
        if (!pElement || pElement->GetType() != FDE_XMLNODE_Element) {
          return FALSE;
        }
        CXFA_Node* pDataNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
        ASSERT(pDataNode);
        XFA_DataExporter_DealWithDataGroupNode(pDataNode);
        pXMLDoc->SaveXMLNode(pStream, pElement);
      } break;
      case XFA_XDPPACKET_Form: {
        XFA_DataExporter_RegenerateFormFile(pNode, pStream, pChecksum);
      } break;
      case XFA_XDPPACKET_Template:
      default: {
        CFDE_XMLElement* pElement =
            static_cast<CFDE_XMLElement*>(pNode->GetXMLMappingNode());
        if (!pElement || pElement->GetType() != FDE_XMLNODE_Element) {
          return FALSE;
        }
        pXMLDoc->SaveXMLNode(pStream, pElement);
      } break;
    }
  } else {
    CXFA_Node* pDataNode = pNode->GetNodeItem(XFA_NODEITEM_Parent);
    CXFA_Node* pExportNode = pNode;
    for (CXFA_Node* pChildNode =
             pDataNode->GetNodeItem(XFA_NODEITEM_FirstChild);
         pChildNode;
         pChildNode = pChildNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      if (pChildNode != pNode) {
        pExportNode = pDataNode;
        break;
      }
    }
    CFDE_XMLElement* pElement =
        static_cast<CFDE_XMLElement*>(pExportNode->GetXMLMappingNode());
    if (!pElement || pElement->GetType() != FDE_XMLNODE_Element) {
      return FALSE;
    }
    XFA_DataExporter_DealWithDataGroupNode(pExportNode);
    pElement->SetString(L"xmlns:xfa",
                        L"http://www.xfa.org/schema/xfa-data/1.0/");
    pXMLDoc->SaveXMLNode(pStream, pElement);
    pElement->RemoveAttribute(L"xmlns:xfa");
  }
  return TRUE;
}
void XFA_DataExporter_DealWithDataGroupNode(CXFA_Node* pDataNode) {
  if (!pDataNode || pDataNode->GetElementType() == XFA_Element::DataValue) {
    return;
  }
  int32_t iChildNum = 0;
  for (CXFA_Node* pChildNode = pDataNode->GetNodeItem(XFA_NODEITEM_FirstChild);
       pChildNode;
       pChildNode = pChildNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    iChildNum++;
    XFA_DataExporter_DealWithDataGroupNode(pChildNode);
  }
  if (pDataNode->GetElementType() == XFA_Element::DataGroup) {
    if (iChildNum > 0) {
      CFDE_XMLNode* pXMLNode = pDataNode->GetXMLMappingNode();
      ASSERT(pXMLNode->GetType() == FDE_XMLNODE_Element);
      CFDE_XMLElement* pXMLElement = static_cast<CFDE_XMLElement*>(pXMLNode);
      if (pXMLElement->HasAttribute(L"xfa:dataNode")) {
        pXMLElement->RemoveAttribute(L"xfa:dataNode");
      }
    } else {
      CFDE_XMLNode* pXMLNode = pDataNode->GetXMLMappingNode();
      ASSERT(pXMLNode->GetType() == FDE_XMLNODE_Element);
      static_cast<CFDE_XMLElement*>(pXMLNode)->SetString(L"xfa:dataNode",
                                                         L"dataGroup");
    }
  }
}
