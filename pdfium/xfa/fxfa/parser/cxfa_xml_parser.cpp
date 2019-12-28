// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_xml_parser.h"

CXFA_XMLParser::CXFA_XMLParser(CFDE_XMLNode* pRoot, IFX_Stream* pStream)
    : m_nElementStart(0),
      m_dwCheckStatus(0),
      m_dwCurrentCheckStatus(0),
      m_pRoot(pRoot),
      m_pStream(pStream),
      m_pParser(new CFDE_XMLSyntaxParser),
      m_pParent(pRoot),
      m_pChild(nullptr),
      m_NodeStack(16),
      m_syntaxParserResult(FDE_XmlSyntaxResult::None) {
  ASSERT(m_pParent && m_pStream);
  m_NodeStack.Push(m_pParent);
  m_pParser->Init(m_pStream, 32 * 1024, 1024 * 1024);
}

CXFA_XMLParser::~CXFA_XMLParser() {
  m_NodeStack.RemoveAll();
  m_ws1.clear();
  m_ws2.clear();
}

void CXFA_XMLParser::Release() {
  delete this;
}

int32_t CXFA_XMLParser::DoParser(IFX_Pause* pPause) {
  if (m_syntaxParserResult == FDE_XmlSyntaxResult::Error)
    return -1;
  if (m_syntaxParserResult == FDE_XmlSyntaxResult::EndOfString)
    return 100;

  int32_t iCount = 0;
  while (TRUE) {
    m_syntaxParserResult = m_pParser->DoSyntaxParse();
    switch (m_syntaxParserResult) {
      case FDE_XmlSyntaxResult::InstructionOpen:
        break;
      case FDE_XmlSyntaxResult::InstructionClose:
        if (m_pChild) {
          if (m_pChild->GetType() != FDE_XMLNODE_Instruction) {
            m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
            break;
          }
        }
        m_pChild = m_pParent;
        break;
      case FDE_XmlSyntaxResult::ElementOpen:
        if (m_dwCheckStatus != 0x03 && m_NodeStack.GetSize() == 2) {
          m_nElementStart = m_pParser->GetCurrentPos() - 1;
        }
        break;
      case FDE_XmlSyntaxResult::ElementBreak:
        break;
      case FDE_XmlSyntaxResult::ElementClose:
        if (m_pChild->GetType() != FDE_XMLNODE_Element) {
          m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
          break;
        }
        m_pParser->GetTagName(m_ws1);
        static_cast<CFDE_XMLElement*>(m_pChild)->GetTagName(m_ws2);
        if (m_ws1.GetLength() > 0 && m_ws1 != m_ws2) {
          m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
          break;
        }
        m_NodeStack.Pop();
        if (m_NodeStack.GetSize() < 1) {
          m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
          break;
        } else if (m_dwCurrentCheckStatus != 0 && m_NodeStack.GetSize() == 2) {
          m_nSize[m_dwCurrentCheckStatus - 1] =
              m_pParser->GetCurrentBinaryPos() -
              m_nStart[m_dwCurrentCheckStatus - 1];
          m_dwCurrentCheckStatus = 0;
        }

        m_pParent = static_cast<CFDE_XMLNode*>(*m_NodeStack.GetTopElement());
        m_pChild = m_pParent;
        iCount++;
        break;
      case FDE_XmlSyntaxResult::TargetName:
        m_pParser->GetTargetName(m_ws1);
        if (m_ws1 == FX_WSTRC(L"originalXFAVersion") ||
            m_ws1 == FX_WSTRC(L"acrobat")) {
          m_pChild = new CFDE_XMLInstruction(m_ws1);
          m_pParent->InsertChildNode(m_pChild);
        } else {
          m_pChild = nullptr;
        }
        m_ws1.clear();
        break;
      case FDE_XmlSyntaxResult::TagName:
        m_pParser->GetTagName(m_ws1);
        m_pChild = new CFDE_XMLElement(m_ws1);
        m_pParent->InsertChildNode(m_pChild);
        m_NodeStack.Push(m_pChild);
        m_pParent = m_pChild;

        if (m_dwCheckStatus != 0x03 && m_NodeStack.GetSize() == 3) {
          CFX_WideString wsTag;
          static_cast<CFDE_XMLElement*>(m_pChild)->GetLocalTagName(wsTag);
          if (wsTag == FX_WSTRC(L"template")) {
            m_dwCheckStatus |= 0x01;
            m_dwCurrentCheckStatus = 0x01;
            m_nStart[0] = m_pParser->GetCurrentBinaryPos() -
                          (m_pParser->GetCurrentPos() - m_nElementStart);
          } else if (wsTag == FX_WSTRC(L"datasets")) {
            m_dwCheckStatus |= 0x02;
            m_dwCurrentCheckStatus = 0x02;
            m_nStart[1] = m_pParser->GetCurrentBinaryPos() -
                          (m_pParser->GetCurrentPos() - m_nElementStart);
          }
        }
        break;
      case FDE_XmlSyntaxResult::AttriName:
        m_pParser->GetAttributeName(m_ws1);
        break;
      case FDE_XmlSyntaxResult::AttriValue:
        if (m_pChild) {
          m_pParser->GetAttributeName(m_ws2);
          if (m_pChild->GetType() == FDE_XMLNODE_Element) {
            static_cast<CFDE_XMLElement*>(m_pChild)->SetString(m_ws1, m_ws2);
          }
        }
        m_ws1.clear();
        break;
      case FDE_XmlSyntaxResult::Text:
        m_pParser->GetTextData(m_ws1);
        m_pChild = new CFDE_XMLText(m_ws1);
        m_pParent->InsertChildNode(m_pChild);
        m_pChild = m_pParent;
        break;
      case FDE_XmlSyntaxResult::CData:
        m_pParser->GetTextData(m_ws1);
        m_pChild = new CFDE_XMLCharData(m_ws1);
        m_pParent->InsertChildNode(m_pChild);
        m_pChild = m_pParent;
        break;
      case FDE_XmlSyntaxResult::TargetData:
        if (m_pChild) {
          if (m_pChild->GetType() != FDE_XMLNODE_Instruction) {
            m_syntaxParserResult = FDE_XmlSyntaxResult::Error;
            break;
          }
          if (!m_ws1.IsEmpty()) {
            static_cast<CFDE_XMLInstruction*>(m_pChild)->AppendData(m_ws1);
          }
          m_pParser->GetTargetData(m_ws1);
          static_cast<CFDE_XMLInstruction*>(m_pChild)->AppendData(m_ws1);
        }
        m_ws1.clear();
        break;
      default:
        break;
    }
    if (m_syntaxParserResult == FDE_XmlSyntaxResult::Error ||
        m_syntaxParserResult == FDE_XmlSyntaxResult::EndOfString) {
      break;
    }
    if (pPause && iCount > 500 && pPause->NeedToPauseNow()) {
      break;
    }
  }
  return (m_syntaxParserResult == FDE_XmlSyntaxResult::Error ||
          m_NodeStack.GetSize() != 1)
             ? -1
             : m_pParser->GetStatus();
}
