// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cba_fontmap.h"

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "fpdfsdk/include/fsdk_baseannot.h"

CBA_FontMap::CBA_FontMap(CPDFSDK_Annot* pAnnot,
                         CFX_SystemHandler* pSystemHandler)
    : CPWL_FontMap(pSystemHandler),
      m_pDocument(nullptr),
      m_pAnnotDict(nullptr),
      m_pDefaultFont(nullptr),
      m_sAPType("N") {
  CPDF_Page* pPage = pAnnot->GetPDFPage();

  m_pDocument = pPage->m_pDocument;
  m_pAnnotDict = pAnnot->GetPDFAnnot()->GetAnnotDict();
  Initialize();
}

CBA_FontMap::~CBA_FontMap() {}

void CBA_FontMap::Reset() {
  Empty();
  m_pDefaultFont = nullptr;
  m_sDefaultFontName = "";
}

void CBA_FontMap::Initialize() {
  int32_t nCharset = DEFAULT_CHARSET;

  if (!m_pDefaultFont) {
    m_pDefaultFont = GetAnnotDefaultFont(m_sDefaultFontName);
    if (m_pDefaultFont) {
      if (const CFX_SubstFont* pSubstFont = m_pDefaultFont->GetSubstFont()) {
        nCharset = pSubstFont->m_Charset;
      } else {
        if (m_sDefaultFontName == "Wingdings" ||
            m_sDefaultFontName == "Wingdings2" ||
            m_sDefaultFontName == "Wingdings3" ||
            m_sDefaultFontName == "Webdings")
          nCharset = SYMBOL_CHARSET;
        else
          nCharset = ANSI_CHARSET;
      }
      AddFontData(m_pDefaultFont, m_sDefaultFontName, nCharset);
      AddFontToAnnotDict(m_pDefaultFont, m_sDefaultFontName);
    }
  }

  if (nCharset != ANSI_CHARSET)
    CPWL_FontMap::Initialize();
}

void CBA_FontMap::SetDefaultFont(CPDF_Font* pFont,
                                 const CFX_ByteString& sFontName) {
  ASSERT(pFont);

  if (m_pDefaultFont)
    return;

  m_pDefaultFont = pFont;
  m_sDefaultFontName = sFontName;

  int32_t nCharset = DEFAULT_CHARSET;
  if (const CFX_SubstFont* pSubstFont = m_pDefaultFont->GetSubstFont())
    nCharset = pSubstFont->m_Charset;
  AddFontData(m_pDefaultFont, m_sDefaultFontName, nCharset);
}

CPDF_Font* CBA_FontMap::FindFontSameCharset(CFX_ByteString& sFontAlias,
                                            int32_t nCharset) {
  if (m_pAnnotDict->GetStringBy("Subtype") != "Widget")
    return nullptr;

  CPDF_Document* pDocument = GetDocument();
  CPDF_Dictionary* pRootDict = pDocument->GetRoot();
  if (!pRootDict)
    return nullptr;

  CPDF_Dictionary* pAcroFormDict = pRootDict->GetDictBy("AcroForm");
  if (!pAcroFormDict)
    return nullptr;

  CPDF_Dictionary* pDRDict = pAcroFormDict->GetDictBy("DR");
  if (!pDRDict)
    return nullptr;

  return FindResFontSameCharset(pDRDict, sFontAlias, nCharset);
}

CPDF_Document* CBA_FontMap::GetDocument() {
  return m_pDocument;
}

CPDF_Font* CBA_FontMap::FindResFontSameCharset(CPDF_Dictionary* pResDict,
                                               CFX_ByteString& sFontAlias,
                                               int32_t nCharset) {
  if (!pResDict)
    return nullptr;

  CPDF_Dictionary* pFonts = pResDict->GetDictBy("Font");
  if (!pFonts)
    return nullptr;

  CPDF_Document* pDocument = GetDocument();
  CPDF_Font* pFind = nullptr;
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj)
      continue;

    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font")
      continue;

    CPDF_Font* pFont = pDocument->LoadFont(pElement);
    if (!pFont)
      continue;
    const CFX_SubstFont* pSubst = pFont->GetSubstFont();
    if (!pSubst)
      continue;
    if (pSubst->m_Charset == nCharset) {
      sFontAlias = csKey;
      pFind = pFont;
    }
  }
  return pFind;
}

void CBA_FontMap::AddedFont(CPDF_Font* pFont,
                            const CFX_ByteString& sFontAlias) {
  AddFontToAnnotDict(pFont, sFontAlias);
}

void CBA_FontMap::AddFontToAnnotDict(CPDF_Font* pFont,
                                     const CFX_ByteString& sAlias) {
  if (!pFont)
    return;

  CPDF_Dictionary* pAPDict = m_pAnnotDict->GetDictBy("AP");

  if (!pAPDict) {
    pAPDict = new CPDF_Dictionary;
    m_pAnnotDict->SetAt("AP", pAPDict);
  }

  // to avoid checkbox and radiobutton
  CPDF_Object* pObject = pAPDict->GetObjectBy(m_sAPType);
  if (ToDictionary(pObject))
    return;

  CPDF_Stream* pStream = pAPDict->GetStreamBy(m_sAPType);
  if (!pStream) {
    pStream = new CPDF_Stream(nullptr, 0, nullptr);
    int32_t objnum = m_pDocument->AddIndirectObject(pStream);
    pAPDict->SetAtReference(m_sAPType, m_pDocument, objnum);
  }

  CPDF_Dictionary* pStreamDict = pStream->GetDict();

  if (!pStreamDict) {
    pStreamDict = new CPDF_Dictionary;
    pStream->InitStream(nullptr, 0, pStreamDict);
  }

  if (pStreamDict) {
    CPDF_Dictionary* pStreamResList = pStreamDict->GetDictBy("Resources");
    if (!pStreamResList) {
      pStreamResList = new CPDF_Dictionary();
      pStreamDict->SetAt("Resources", pStreamResList);
    }

    if (pStreamResList) {
      CPDF_Dictionary* pStreamResFontList = pStreamResList->GetDictBy("Font");
      if (!pStreamResFontList) {
        pStreamResFontList = new CPDF_Dictionary;
        int32_t objnum = m_pDocument->AddIndirectObject(pStreamResFontList);
        pStreamResList->SetAtReference("Font", m_pDocument, objnum);
      }
      if (!pStreamResFontList->KeyExist(sAlias))
        pStreamResFontList->SetAtReference(sAlias, m_pDocument,
                                           pFont->GetFontDict());
    }
  }
}

CPDF_Font* CBA_FontMap::GetAnnotDefaultFont(CFX_ByteString& sAlias) {
  CPDF_Dictionary* pAcroFormDict = nullptr;
  const bool bWidget = (m_pAnnotDict->GetStringBy("Subtype") == "Widget");
  if (bWidget) {
    if (CPDF_Dictionary* pRootDict = m_pDocument->GetRoot())
      pAcroFormDict = pRootDict->GetDictBy("AcroForm");
  }

  CFX_ByteString sDA;
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pAnnotDict, "DA");
  if (pObj)
    sDA = pObj->GetString();

  if (bWidget) {
    if (sDA.IsEmpty()) {
      pObj = FPDF_GetFieldAttr(pAcroFormDict, "DA");
      sDA = pObj ? pObj->GetString() : CFX_ByteString();
    }
  }

  CPDF_Dictionary* pFontDict = nullptr;

  if (!sDA.IsEmpty()) {
    CPDF_SimpleParser syntax(sDA.AsStringC());
    syntax.FindTagParamFromStart("Tf", 2);
    CFX_ByteString sFontName(syntax.GetWord());
    sAlias = PDF_NameDecode(sFontName).Mid(1);

    if (CPDF_Dictionary* pDRDict = m_pAnnotDict->GetDictBy("DR"))
      if (CPDF_Dictionary* pDRFontDict = pDRDict->GetDictBy("Font"))
        pFontDict = pDRFontDict->GetDictBy(sAlias);

    if (!pFontDict)
      if (CPDF_Dictionary* pAPDict = m_pAnnotDict->GetDictBy("AP"))
        if (CPDF_Dictionary* pNormalDict = pAPDict->GetDictBy("N"))
          if (CPDF_Dictionary* pNormalResDict =
                  pNormalDict->GetDictBy("Resources"))
            if (CPDF_Dictionary* pResFontDict =
                    pNormalResDict->GetDictBy("Font"))
              pFontDict = pResFontDict->GetDictBy(sAlias);

    if (bWidget) {
      if (!pFontDict) {
        if (pAcroFormDict) {
          if (CPDF_Dictionary* pDRDict = pAcroFormDict->GetDictBy("DR"))
            if (CPDF_Dictionary* pDRFontDict = pDRDict->GetDictBy("Font"))
              pFontDict = pDRFontDict->GetDictBy(sAlias);
        }
      }
    }
  }

  return pFontDict ? m_pDocument->LoadFont(pFontDict) : nullptr;
}

void CBA_FontMap::SetAPType(const CFX_ByteString& sAPType) {
  m_sAPType = sAPType;

  Reset();
  Initialize();
}
