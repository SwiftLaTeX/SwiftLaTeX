// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cfdf_document.h"

#include "core/fpdfapi/fpdf_edit/include/cpdf_creator.h"
#include "core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"

CFDF_Document::CFDF_Document() : CPDF_IndirectObjectHolder(nullptr) {
  m_pRootDict = nullptr;
  m_pFile = nullptr;
  m_bOwnFile = FALSE;
}
CFDF_Document::~CFDF_Document() {
  if (m_bOwnFile && m_pFile) {
    m_pFile->Release();
  }
}
CFDF_Document* CFDF_Document::CreateNewDoc() {
  CFDF_Document* pDoc = new CFDF_Document;
  pDoc->m_pRootDict = new CPDF_Dictionary;
  pDoc->AddIndirectObject(pDoc->m_pRootDict);
  CPDF_Dictionary* pFDFDict = new CPDF_Dictionary;
  pDoc->m_pRootDict->SetAt("FDF", pFDFDict);
  return pDoc;
}

CFDF_Document* CFDF_Document::ParseFile(IFX_FileRead* pFile, FX_BOOL bOwnFile) {
  if (!pFile)
    return nullptr;

  std::unique_ptr<CFDF_Document> pDoc(new CFDF_Document);
  pDoc->ParseStream(pFile, bOwnFile);
  return pDoc->m_pRootDict ? pDoc.release() : nullptr;
}

CFDF_Document* CFDF_Document::ParseMemory(const uint8_t* pData, uint32_t size) {
  return CFDF_Document::ParseFile(FX_CreateMemoryStream((uint8_t*)pData, size),
                                  TRUE);
}

void CFDF_Document::ParseStream(IFX_FileRead* pFile, FX_BOOL bOwnFile) {
  m_pFile = pFile;
  m_bOwnFile = bOwnFile;
  CPDF_SyntaxParser parser;
  parser.InitParser(m_pFile, 0);
  while (1) {
    bool bNumber;
    CFX_ByteString word = parser.GetNextWord(&bNumber);
    if (bNumber) {
      uint32_t objnum = FXSYS_atoui(word.c_str());
      word = parser.GetNextWord(&bNumber);
      if (!bNumber)
        break;

      word = parser.GetNextWord(nullptr);
      if (word != "obj")
        break;

      CPDF_Object* pObj = parser.GetObject(this, objnum, 0, true);
      if (!pObj)
        break;

      InsertIndirectObject(objnum, pObj);
      word = parser.GetNextWord(nullptr);
      if (word != "endobj")
        break;
    } else {
      if (word != "trailer")
        break;

      if (CPDF_Dictionary* pMainDict =
              ToDictionary(parser.GetObject(this, 0, 0, true))) {
        m_pRootDict = pMainDict->GetDictBy("Root");
        pMainDict->Release();
      }
      break;
    }
  }
}

FX_BOOL CFDF_Document::WriteBuf(CFX_ByteTextBuf& buf) const {
  if (!m_pRootDict) {
    return FALSE;
  }
  buf << "%FDF-1.2\r\n";
  for (const auto& pair : m_IndirectObjs) {
    buf << pair.first << " 0 obj\r\n" << pair.second << "\r\nendobj\r\n\r\n";
  }
  buf << "trailer\r\n<</Root " << m_pRootDict->GetObjNum()
      << " 0 R>>\r\n%%EOF\r\n";
  return TRUE;
}
