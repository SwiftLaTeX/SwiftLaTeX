// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxcrt/include/fx_xml.h"

CPDF_Metadata::CPDF_Metadata(CPDF_Document* pDoc) {
  CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return;

  CPDF_Stream* pStream = pRoot->GetStreamBy("Metadata");
  if (!pStream)
    return;

  CPDF_StreamAcc acc;
  acc.LoadAllData(pStream, FALSE);
  m_pXmlElement.reset(CXML_Element::Parse(acc.GetData(), acc.GetSize()));
}

CPDF_Metadata::~CPDF_Metadata() {}

const CXML_Element* CPDF_Metadata::GetRoot() const {
  return m_pXmlElement.get();
}
