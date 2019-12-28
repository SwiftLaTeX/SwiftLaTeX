// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_edit/include/cpdf_pagecontentgenerator.h"

#include "core/fpdfapi/fpdf_edit/include/cpdf_creator.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"

CFX_ByteTextBuf& operator<<(CFX_ByteTextBuf& ar, CFX_Matrix& matrix) {
  ar << matrix.a << " " << matrix.b << " " << matrix.c << " " << matrix.d << " "
     << matrix.e << " " << matrix.f;
  return ar;
}

CPDF_PageContentGenerator::CPDF_PageContentGenerator(CPDF_Page* pPage)
    : m_pPage(pPage), m_pDocument(m_pPage->m_pDocument) {
  for (const auto& pObj : *pPage->GetPageObjectList())
    InsertPageObject(pObj.get());
}

CPDF_PageContentGenerator::~CPDF_PageContentGenerator() {}

FX_BOOL CPDF_PageContentGenerator::InsertPageObject(
    CPDF_PageObject* pPageObject) {
  return pPageObject && m_pageObjects.Add(pPageObject);
}

void CPDF_PageContentGenerator::GenerateContent() {
  CFX_ByteTextBuf buf;
  CPDF_Dictionary* pPageDict = m_pPage->m_pFormDict;
  for (int i = 0; i < m_pageObjects.GetSize(); ++i) {
    CPDF_PageObject* pPageObj = m_pageObjects[i];
    if (!pPageObj || !pPageObj->IsImage()) {
      continue;
    }
    ProcessImage(buf, pPageObj->AsImage());
  }
  CPDF_Object* pContent =
      pPageDict ? pPageDict->GetDirectObjectBy("Contents") : nullptr;
  if (pContent) {
    pPageDict->RemoveAt("Contents");
  }
  CPDF_Stream* pStream = new CPDF_Stream(nullptr, 0, nullptr);
  pStream->SetData(buf.GetBuffer(), buf.GetLength(), FALSE, FALSE);
  m_pDocument->AddIndirectObject(pStream);
  pPageDict->SetAtReference("Contents", m_pDocument, pStream->GetObjNum());
}
CFX_ByteString CPDF_PageContentGenerator::RealizeResource(
    CPDF_Object* pResourceObj,
    const CFX_ByteString& bsType) {
  if (!m_pPage->m_pResources) {
    m_pPage->m_pResources = new CPDF_Dictionary;
    int objnum = m_pDocument->AddIndirectObject(m_pPage->m_pResources);
    m_pPage->m_pFormDict->SetAtReference("Resources", m_pDocument, objnum);
  }
  CPDF_Dictionary* pResList = m_pPage->m_pResources->GetDictBy(bsType);
  if (!pResList) {
    pResList = new CPDF_Dictionary;
    m_pPage->m_pResources->SetAt(bsType, pResList);
  }
  m_pDocument->AddIndirectObject(pResourceObj);
  CFX_ByteString name;
  int idnum = 1;
  while (1) {
    name.Format("FX%c%d", bsType[0], idnum);
    if (!pResList->KeyExist(name)) {
      break;
    }
    idnum++;
  }
  pResList->SetAtReference(name, m_pDocument, pResourceObj->GetObjNum());
  return name;
}
void CPDF_PageContentGenerator::ProcessImage(CFX_ByteTextBuf& buf,
                                             CPDF_ImageObject* pImageObj) {
  if ((pImageObj->m_Matrix.a == 0 && pImageObj->m_Matrix.b == 0) ||
      (pImageObj->m_Matrix.c == 0 && pImageObj->m_Matrix.d == 0)) {
    return;
  }
  buf << "q " << pImageObj->m_Matrix << " cm ";
  if (!pImageObj->m_pImage->IsInline()) {
    CPDF_Stream* pStream = pImageObj->m_pImage->GetStream();
    uint32_t dwSavedObjNum = pStream->GetObjNum();
    CFX_ByteString name = RealizeResource(pStream, "XObject");
    if (dwSavedObjNum == 0) {
      if (pImageObj->m_pImage)
        pImageObj->m_pImage->Release();
      pImageObj->m_pImage = m_pDocument->GetPageData()->GetImage(pStream);
    }
    buf << "/" << PDF_NameEncode(name) << " Do Q\n";
  }
}
void CPDF_PageContentGenerator::ProcessForm(CFX_ByteTextBuf& buf,
                                            const uint8_t* data,
                                            uint32_t size,
                                            CFX_Matrix& matrix) {
  if (!data || !size) {
    return;
  }
  CPDF_Stream* pStream = new CPDF_Stream(nullptr, 0, nullptr);
  CPDF_Dictionary* pFormDict = new CPDF_Dictionary;
  pFormDict->SetAtName("Type", "XObject");
  pFormDict->SetAtName("Subtype", "Form");
  CFX_FloatRect bbox = m_pPage->GetPageBBox();
  matrix.TransformRect(bbox);
  pFormDict->SetAtRect("BBox", bbox);
  pStream->InitStream(data, size, pFormDict);
  buf << "q " << matrix << " cm ";
  CFX_ByteString name = RealizeResource(pStream, "XObject");
  buf << "/" << PDF_NameEncode(name) << " Do Q\n";
}
void CPDF_PageContentGenerator::TransformContent(CFX_Matrix& matrix) {
  CPDF_Dictionary* pDict = m_pPage->m_pFormDict;
  CPDF_Object* pContent =
      pDict ? pDict->GetDirectObjectBy("Contents") : nullptr;
  if (!pContent)
    return;

  CFX_ByteTextBuf buf;
  if (CPDF_Array* pArray = pContent->AsArray()) {
    size_t iCount = pArray->GetCount();
    CPDF_StreamAcc** pContentArray = FX_Alloc(CPDF_StreamAcc*, iCount);
    size_t size = 0;
    for (size_t i = 0; i < iCount; ++i) {
      pContent = pArray->GetObjectAt(i);
      CPDF_Stream* pStream = ToStream(pContent);
      if (!pStream)
        continue;

      CPDF_StreamAcc* pStreamAcc = new CPDF_StreamAcc();
      pStreamAcc->LoadAllData(pStream);
      pContentArray[i] = pStreamAcc;
      size += pContentArray[i]->GetSize() + 1;
    }
    int pos = 0;
    uint8_t* pBuf = FX_Alloc(uint8_t, size);
    for (size_t i = 0; i < iCount; ++i) {
      FXSYS_memcpy(pBuf + pos, pContentArray[i]->GetData(),
                   pContentArray[i]->GetSize());
      pos += pContentArray[i]->GetSize() + 1;
      pBuf[pos - 1] = ' ';
      delete pContentArray[i];
    }
    ProcessForm(buf, pBuf, size, matrix);
    FX_Free(pBuf);
    FX_Free(pContentArray);
  } else if (CPDF_Stream* pStream = pContent->AsStream()) {
    CPDF_StreamAcc contentStream;
    contentStream.LoadAllData(pStream);
    ProcessForm(buf, contentStream.GetData(), contentStream.GetSize(), matrix);
  }
  CPDF_Stream* pStream = new CPDF_Stream(nullptr, 0, nullptr);
  pStream->SetData(buf.GetBuffer(), buf.GetLength(), FALSE, FALSE);
  m_pDocument->AddIndirectObject(pStream);
  m_pPage->m_pFormDict->SetAtReference("Contents", m_pDocument,
                                       pStream->GetObjNum());
}
