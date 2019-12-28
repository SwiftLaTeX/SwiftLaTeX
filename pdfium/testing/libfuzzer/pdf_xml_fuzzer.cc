// Copyright 2016 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/xml/fde_xml_imp.h"
#include "xfa/fxfa/parser/cxfa_xml_parser.h"
#include "xfa/fxfa/parser/xfa_utils.h"

namespace {

CFDE_XMLNode* XFA_FDEExtension_GetDocumentNode(
    CFDE_XMLDoc* pXMLDoc,
    FX_BOOL bVerifyWellFormness = FALSE) {
  if (!pXMLDoc) {
    return nullptr;
  }
  CFDE_XMLNode* pXMLFakeRoot = pXMLDoc->GetRoot();
  for (CFDE_XMLNode* pXMLNode =
           pXMLFakeRoot->GetNodeItem(CFDE_XMLNode::FirstChild);
       pXMLNode; pXMLNode = pXMLNode->GetNodeItem(CFDE_XMLNode::NextSibling)) {
    if (pXMLNode->GetType() == FDE_XMLNODE_Element) {
      if (bVerifyWellFormness) {
        for (CFDE_XMLNode* pNextNode =
                 pXMLNode->GetNodeItem(CFDE_XMLNode::NextSibling);
             pNextNode;
             pNextNode = pNextNode->GetNodeItem(CFDE_XMLNode::NextSibling)) {
          if (pNextNode->GetType() == FDE_XMLNODE_Element) {
            return FALSE;
          }
        }
      }
      return pXMLNode;
    }
  }
  return nullptr;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size > std::numeric_limits<FX_STRSIZE>::max())
    return 0;

  CFX_WideString input = CFX_WideString::FromUTF8(
      CFX_ByteStringC(data, static_cast<FX_STRSIZE>(size)));
  std::unique_ptr<IFX_Stream, ReleaseDeleter<IFX_Stream>> stream(
      XFA_CreateWideTextRead(input));
  if (!stream)
    return 0;

  std::unique_ptr<CFDE_XMLDoc> doc(new CFDE_XMLDoc);
  std::unique_ptr<CFDE_XMLParser, ReleaseDeleter<CFDE_XMLParser>> parser(
      new CXFA_XMLParser(doc->GetRoot(), stream.get()));

  if (!doc->LoadXML(parser.release()))
    return 0;

  int32_t load_result = doc->DoLoad(nullptr);
  if (load_result < 100)
    return 0;

  (void)XFA_FDEExtension_GetDocumentNode(doc.get());
  return 0;
}
