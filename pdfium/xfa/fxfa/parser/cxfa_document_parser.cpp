// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_document_parser.h"

#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/xfa_document.h"

CXFA_DocumentParser::CXFA_DocumentParser(CXFA_FFNotify* pNotify)
    : m_nodeParser(nullptr, TRUE), m_pNotify(pNotify) {}

CXFA_DocumentParser::~CXFA_DocumentParser() {
}

int32_t CXFA_DocumentParser::StartParse(IFX_FileRead* pStream,
                                        XFA_XDPPACKET ePacketID) {
  m_pDocument.reset();
  m_nodeParser.CloseParser();

  int32_t nRetStatus = m_nodeParser.StartParse(pStream, ePacketID);
  if (nRetStatus == XFA_PARSESTATUS_Ready) {
    m_pDocument.reset(new CXFA_Document(this));
    m_nodeParser.SetFactory(m_pDocument.get());
  }
  return nRetStatus;
}

int32_t CXFA_DocumentParser::DoParse(IFX_Pause* pPause) {
  int32_t nRetStatus = m_nodeParser.DoParse(pPause);
  if (nRetStatus >= XFA_PARSESTATUS_Done) {
    ASSERT(m_pDocument);
    m_pDocument->SetRoot(m_nodeParser.GetRootNode());
  }
  return nRetStatus;
}

CFDE_XMLDoc* CXFA_DocumentParser::GetXMLDoc() const {
  return m_nodeParser.GetXMLDoc();
}

CXFA_FFNotify* CXFA_DocumentParser::GetNotify() const {
  return m_pNotify;
}

CXFA_Document* CXFA_DocumentParser::GetDocument() const {
  return m_pDocument.get();
}
