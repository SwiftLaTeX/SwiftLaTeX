// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_XML_FDE_XML_IMP_H_
#define XFA_FDE_XML_FDE_XML_IMP_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/xml/fde_xml.h"
#include "xfa/fgas/crt/fgas_memory.h"
#include "xfa/fgas/crt/fgas_stream.h"
#include "xfa/fgas/crt/fgas_utils.h"

class CFDE_BlockBuffer;
class CFDE_XMLInstruction;
class CFDE_XMLElement;
class CFDE_XMLText;
class CFDE_XMLDoc;
class CFDE_XMLDOMParser;
class CFDE_XMLParser;
class CFDE_XMLSyntaxParser;

class CFDE_XMLNode : public CFX_Target {
 public:
  enum NodeItem {
    Root = 0,
    Parent,
    FirstSibling,
    PriorSibling,
    NextSibling,
    LastSibling,
    FirstNeighbor,
    PriorNeighbor,
    NextNeighbor,
    LastNeighbor,
    FirstChild,
    LastChild
  };

  CFDE_XMLNode();
  ~CFDE_XMLNode() override;

  virtual void Release();
  virtual FDE_XMLNODETYPE GetType() const;
  virtual CFDE_XMLNode* Clone(FX_BOOL bRecursive);

  int32_t CountChildNodes() const;
  CFDE_XMLNode* GetChildNode(int32_t index) const;
  int32_t GetChildNodeIndex(CFDE_XMLNode* pNode) const;
  int32_t InsertChildNode(CFDE_XMLNode* pNode, int32_t index = -1);
  void RemoveChildNode(CFDE_XMLNode* pNode);
  void DeleteChildren();
  void CloneChildren(CFDE_XMLNode* pClone);

  CFDE_XMLNode* GetPath(const FX_WCHAR* pPath,
                        int32_t iLength = -1,
                        FX_BOOL bQualifiedName = TRUE) const;

  int32_t GetNodeLevel() const;
  CFDE_XMLNode* GetNodeItem(CFDE_XMLNode::NodeItem eItem) const;
  FX_BOOL InsertNodeItem(CFDE_XMLNode::NodeItem eItem, CFDE_XMLNode* pNode);
  CFDE_XMLNode* RemoveNodeItem(CFDE_XMLNode::NodeItem eItem);

  void SaveXMLNode(IFX_Stream* pXMLStream);

  CFDE_XMLNode* m_pParent;
  CFDE_XMLNode* m_pChild;
  CFDE_XMLNode* m_pPrior;
  CFDE_XMLNode* m_pNext;
};

class CFDE_XMLInstruction : public CFDE_XMLNode {
 public:
  explicit CFDE_XMLInstruction(const CFX_WideString& wsTarget);
  ~CFDE_XMLInstruction() override;

  // CFDE_XMLNode
  void Release() override;
  FDE_XMLNODETYPE GetType() const override;
  CFDE_XMLNode* Clone(FX_BOOL bRecursive) override;

  void GetTargetName(CFX_WideString& wsTarget) const { wsTarget = m_wsTarget; }
  int32_t CountAttributes() const;
  FX_BOOL GetAttribute(int32_t index,
                       CFX_WideString& wsAttriName,
                       CFX_WideString& wsAttriValue) const;
  FX_BOOL HasAttribute(const FX_WCHAR* pwsAttriName) const;
  void GetString(const FX_WCHAR* pwsAttriName,
                 CFX_WideString& wsAttriValue,
                 const FX_WCHAR* pwsDefValue = nullptr) const;
  void SetString(const CFX_WideString& wsAttriName,
                 const CFX_WideString& wsAttriValue);
  int32_t GetInteger(const FX_WCHAR* pwsAttriName, int32_t iDefValue = 0) const;
  void SetInteger(const FX_WCHAR* pwsAttriName, int32_t iAttriValue);
  FX_FLOAT GetFloat(const FX_WCHAR* pwsAttriName, FX_FLOAT fDefValue = 0) const;
  void SetFloat(const FX_WCHAR* pwsAttriName, FX_FLOAT fAttriValue);
  void RemoveAttribute(const FX_WCHAR* pwsAttriName);
  int32_t CountData() const;
  FX_BOOL GetData(int32_t index, CFX_WideString& wsData) const;
  void AppendData(const CFX_WideString& wsData);
  void RemoveData(int32_t index);

  CFX_WideString m_wsTarget;
  CFX_WideStringArray m_Attributes;
  CFX_WideStringArray m_TargetData;
};

class CFDE_XMLElement : public CFDE_XMLNode {
 public:
  explicit CFDE_XMLElement(const CFX_WideString& wsTag);
  ~CFDE_XMLElement() override;

  // CFDE_XMLNode
  void Release() override;
  FDE_XMLNODETYPE GetType() const override;
  CFDE_XMLNode* Clone(FX_BOOL bRecursive) override;

  void GetTagName(CFX_WideString& wsTag) const;
  void GetLocalTagName(CFX_WideString& wsTag) const;

  void GetNamespacePrefix(CFX_WideString& wsPrefix) const;
  void GetNamespaceURI(CFX_WideString& wsNamespace) const;

  int32_t CountAttributes() const;
  FX_BOOL GetAttribute(int32_t index,
                       CFX_WideString& wsAttriName,
                       CFX_WideString& wsAttriValue) const;
  FX_BOOL HasAttribute(const FX_WCHAR* pwsAttriName) const;
  void RemoveAttribute(const FX_WCHAR* pwsAttriName);

  void GetString(const FX_WCHAR* pwsAttriName,
                 CFX_WideString& wsAttriValue,
                 const FX_WCHAR* pwsDefValue = nullptr) const;
  void SetString(const CFX_WideString& wsAttriName,
                 const CFX_WideString& wsAttriValue);

  int32_t GetInteger(const FX_WCHAR* pwsAttriName, int32_t iDefValue = 0) const;
  void SetInteger(const FX_WCHAR* pwsAttriName, int32_t iAttriValue);

  FX_FLOAT GetFloat(const FX_WCHAR* pwsAttriName, FX_FLOAT fDefValue = 0) const;
  void SetFloat(const FX_WCHAR* pwsAttriName, FX_FLOAT fAttriValue);

  void GetTextData(CFX_WideString& wsText) const;
  void SetTextData(const CFX_WideString& wsText);

  CFX_WideString m_wsTag;
  CFX_WideStringArray m_Attributes;
};

class CFDE_XMLText : public CFDE_XMLNode {
 public:
  explicit CFDE_XMLText(const CFX_WideString& wsText);
  ~CFDE_XMLText() override;

  // CFDE_XMLNode
  void Release() override;
  FDE_XMLNODETYPE GetType() const override;
  CFDE_XMLNode* Clone(FX_BOOL bRecursive) override;

  void GetText(CFX_WideString& wsText) const { wsText = m_wsText; }
  void SetText(const CFX_WideString& wsText) { m_wsText = wsText; }

  CFX_WideString m_wsText;
};

class CFDE_XMLDeclaration : public CFDE_XMLNode {
 public:
  CFDE_XMLDeclaration() {}
  ~CFDE_XMLDeclaration() override {}
};

class CFDE_XMLCharData : public CFDE_XMLDeclaration {
 public:
  explicit CFDE_XMLCharData(const CFX_WideString& wsCData);
  ~CFDE_XMLCharData() override;

  void Release() override;
  FDE_XMLNODETYPE GetType() const override;
  CFDE_XMLNode* Clone(FX_BOOL bRecursive) override;

  void GetCharData(CFX_WideString& wsCharData) const {
    wsCharData = m_wsCharData;
  }
  void SetCharData(const CFX_WideString& wsCData) { m_wsCharData = wsCData; }

  CFX_WideString m_wsCharData;
};

class CFDE_XMLDoc : public CFX_Target {
 public:
  CFDE_XMLDoc();
  ~CFDE_XMLDoc() override;

  FX_BOOL LoadXML(CFDE_XMLParser* pXMLParser);
  int32_t DoLoad(IFX_Pause* pPause = nullptr);
  void CloseXML();
  CFDE_XMLNode* GetRoot() const { return m_pRoot; }
  void SaveXML(IFX_Stream* pXMLStream = nullptr, FX_BOOL bSaveBOM = TRUE);
  void SaveXMLNode(IFX_Stream* pXMLStream, CFDE_XMLNode* pNode);

 protected:
  void Reset(FX_BOOL bInitRoot);
  void ReleaseParser();

  IFX_Stream* m_pStream;
  int32_t m_iStatus;
  CFDE_XMLNode* m_pRoot;
  CFDE_XMLSyntaxParser* m_pSyntaxParser;
  CFDE_XMLParser* m_pXMLParser;
};

class CFDE_XMLParser {
 public:
  virtual ~CFDE_XMLParser() {}

  virtual void Release() = 0;
  virtual int32_t DoParser(IFX_Pause* pPause) = 0;
};

class CFDE_BlockBuffer : public CFX_Target {
 public:
  explicit CFDE_BlockBuffer(int32_t iAllocStep = 1024 * 1024);
  ~CFDE_BlockBuffer() override;

  FX_BOOL InitBuffer(int32_t iBufferSize = 1024 * 1024);
  FX_BOOL IsInitialized() { return m_iBufferSize / m_iAllocStep >= 1; }
  void ReleaseBuffer() { delete this; }
  FX_WCHAR* GetAvailableBlock(int32_t& iIndexInBlock);
  inline int32_t GetAllocStep() const { return m_iAllocStep; }
  inline int32_t& GetDataLengthRef() { return m_iDataLength; }
  inline void Reset(FX_BOOL bReserveData = TRUE) {
    if (!bReserveData) {
      m_iStartPosition = 0;
    }
    m_iDataLength = 0;
  }
  void SetTextChar(int32_t iIndex, FX_WCHAR ch);
  int32_t DeleteTextChars(int32_t iCount, FX_BOOL bDirection = TRUE);
  void GetTextData(CFX_WideString& wsTextData,
                   int32_t iStart = 0,
                   int32_t iLength = -1) const;

 protected:
  inline void TextDataIndex2BufIndex(const int32_t iIndex,
                                     int32_t& iBlockIndex,
                                     int32_t& iInnerIndex) const;
  void ClearBuffer();

  CFX_ArrayTemplate<FX_WCHAR*> m_BlockArray;
  int32_t m_iDataLength;
  int32_t m_iBufferSize;
  int32_t m_iAllocStep;
  int32_t m_iStartPosition;
};

class CFDE_XMLSyntaxParser : public CFX_Target {
 public:
  CFDE_XMLSyntaxParser();
  ~CFDE_XMLSyntaxParser() override;

  void Release() { delete this; }
  void Init(IFX_Stream* pStream,
            int32_t iXMLPlaneSize,
            int32_t iTextDataSize = 256);

  FDE_XmlSyntaxResult DoSyntaxParse();

  int32_t GetStatus() const;
  int32_t GetCurrentPos() const {
    return m_iParsedChars + (m_pStart - m_pBuffer);
  }
  FX_FILESIZE GetCurrentBinaryPos() const;
  int32_t GetCurrentNodeNumber() const { return m_iCurrentNodeNum; }
  int32_t GetLastNodeNumber() const { return m_iLastNodeNum; }

  void GetTargetName(CFX_WideString& wsTarget) const {
    m_BlockBuffer.GetTextData(wsTarget, 0, m_iTextDataLength);
  }
  void GetTagName(CFX_WideString& wsTag) const {
    m_BlockBuffer.GetTextData(wsTag, 0, m_iTextDataLength);
  }
  void GetAttributeName(CFX_WideString& wsAttriName) const {
    m_BlockBuffer.GetTextData(wsAttriName, 0, m_iTextDataLength);
  }
  void GetAttributeValue(CFX_WideString& wsAttriValue) const {
    m_BlockBuffer.GetTextData(wsAttriValue, 0, m_iTextDataLength);
  }
  void GetTextData(CFX_WideString& wsText) const {
    m_BlockBuffer.GetTextData(wsText, 0, m_iTextDataLength);
  }
  void GetTargetData(CFX_WideString& wsData) const {
    m_BlockBuffer.GetTextData(wsData, 0, m_iTextDataLength);
  }

 protected:
  enum class FDE_XmlSyntaxState {
    Text,
    Node,
    Target,
    Tag,
    AttriName,
    AttriEqualSign,
    AttriQuotation,
    AttriValue,
    Entity,
    EntityDecimal,
    EntityHex,
    CloseInstruction,
    BreakElement,
    CloseElement,
    SkipDeclNode,
    DeclCharData,
    SkipComment,
    SkipCommentOrDecl,
    SkipCData,
    TargetData
  };

  void ParseTextChar(FX_WCHAR ch);

  IFX_Stream* m_pStream;
  int32_t m_iXMLPlaneSize;
  int32_t m_iCurrentPos;
  int32_t m_iCurrentNodeNum;
  int32_t m_iLastNodeNum;
  int32_t m_iParsedChars;
  int32_t m_iParsedBytes;
  FX_WCHAR* m_pBuffer;
  int32_t m_iBufferChars;
  FX_BOOL m_bEOS;
  FX_WCHAR* m_pStart;
  FX_WCHAR* m_pEnd;
  FDE_XMLNODE m_CurNode;
  CFX_StackTemplate<FDE_XMLNODE> m_XMLNodeStack;
  CFDE_BlockBuffer m_BlockBuffer;
  int32_t m_iAllocStep;
  int32_t& m_iDataLength;
  FX_WCHAR* m_pCurrentBlock;
  int32_t m_iIndexInBlock;
  int32_t m_iTextDataLength;
  FDE_XmlSyntaxResult m_syntaxParserResult;
  FDE_XmlSyntaxState m_syntaxParserState;
  FX_WCHAR m_wQuotationMark;
  int32_t m_iEntityStart;
  CFX_DWordStack m_SkipStack;
  FX_WCHAR m_SkipChar;
};

#endif  // XFA_FDE_XML_FDE_XML_IMP_H_
