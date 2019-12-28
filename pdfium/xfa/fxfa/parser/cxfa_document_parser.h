// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_DOCUMENT_PARSER_H_
#define XFA_FXFA_PARSER_CXFA_DOCUMENT_PARSER_H_

#include <memory>

#include "xfa/fxfa/parser/cxfa_simple_parser.h"

class CFDE_XMLDoc;
class CXFA_Document;
class CXFA_FFNotify;
class CXFA_Notify;
class IFX_FileRead;
class IFX_Pause;

class CXFA_DocumentParser {
 public:
  explicit CXFA_DocumentParser(CXFA_FFNotify* pNotify);
  ~CXFA_DocumentParser();

  int32_t StartParse(IFX_FileRead* pStream, XFA_XDPPACKET ePacketID);
  int32_t DoParse(IFX_Pause* pPause);

  CFDE_XMLDoc* GetXMLDoc() const;
  CXFA_FFNotify* GetNotify() const;
  CXFA_Document* GetDocument() const;

 protected:
  CXFA_SimpleParser m_nodeParser;
  CXFA_FFNotify* m_pNotify;
  std::unique_ptr<CXFA_Document> m_pDocument;
};

#endif  // XFA_FXFA_PARSER_CXFA_DOCUMENT_PARSER_H_
