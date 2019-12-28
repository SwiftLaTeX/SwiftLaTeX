// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_XML_PARSER_H_
#define XFA_FXFA_PARSER_CXFA_XML_PARSER_H_

#include "xfa/fde/xml/fde_xml_imp.h"

class IFX_Stream;
class IFX_Pause;

class CXFA_XMLParser : public CFDE_XMLParser {
 public:
  CXFA_XMLParser(CFDE_XMLNode* pRoot, IFX_Stream* pStream);
  ~CXFA_XMLParser() override;

  // CFDE_XMLParser
  void Release() override;
  int32_t DoParser(IFX_Pause* pPause) override;

  FX_FILESIZE m_nStart[2];
  size_t m_nSize[2];
  FX_FILESIZE m_nElementStart;
  uint16_t m_dwCheckStatus;
  uint16_t m_dwCurrentCheckStatus;

 protected:
  CFDE_XMLNode* m_pRoot;
  IFX_Stream* m_pStream;
  std::unique_ptr<CFDE_XMLSyntaxParser, ReleaseDeleter<CFDE_XMLSyntaxParser>>
      m_pParser;
  CFDE_XMLNode* m_pParent;
  CFDE_XMLNode* m_pChild;
  CFX_StackTemplate<CFDE_XMLNode*> m_NodeStack;
  CFX_WideString m_ws1;
  CFX_WideString m_ws2;
  FDE_XmlSyntaxResult m_syntaxParserResult;
};

#endif  // XFA_FXFA_PARSER_CXFA_XML_PARSER_H_
