// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/pdfwindow/PWL_FontMap.h"

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_font/include/cpdf_fontencoding.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fpdfdoc/include/ipvt_fontmap.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"

namespace {

const char kDefaultFontName[] = "Helvetica";

const char* const g_sDEStandardFontName[] = {"Courier",
                                             "Courier-Bold",
                                             "Courier-BoldOblique",
                                             "Courier-Oblique",
                                             "Helvetica",
                                             "Helvetica-Bold",
                                             "Helvetica-BoldOblique",
                                             "Helvetica-Oblique",
                                             "Times-Roman",
                                             "Times-Bold",
                                             "Times-Italic",
                                             "Times-BoldItalic",
                                             "Symbol",
                                             "ZapfDingbats"};

}  // namespace

CPWL_FontMap::CPWL_FontMap(CFX_SystemHandler* pSystemHandler)
    : m_pPDFDoc(nullptr), m_pSystemHandler(pSystemHandler) {
  ASSERT(m_pSystemHandler);
}

CPWL_FontMap::~CPWL_FontMap() {
  delete m_pPDFDoc;
  m_pPDFDoc = nullptr;

  Empty();
}

void CPWL_FontMap::SetSystemHandler(CFX_SystemHandler* pSystemHandler) {
  m_pSystemHandler = pSystemHandler;
}

CPDF_Document* CPWL_FontMap::GetDocument() {
  if (!m_pPDFDoc) {
    if (CPDF_ModuleMgr::Get()) {
      m_pPDFDoc = new CPDF_Document(nullptr);
      m_pPDFDoc->CreateNewDoc();
    }
  }

  return m_pPDFDoc;
}

CPDF_Font* CPWL_FontMap::GetPDFFont(int32_t nFontIndex) {
  if (nFontIndex >= 0 && nFontIndex < m_aData.GetSize()) {
    if (CPWL_FontMap_Data* pData = m_aData.GetAt(nFontIndex)) {
      return pData->pFont;
    }
  }

  return nullptr;
}

CFX_ByteString CPWL_FontMap::GetPDFFontAlias(int32_t nFontIndex) {
  if (nFontIndex >= 0 && nFontIndex < m_aData.GetSize()) {
    if (CPWL_FontMap_Data* pData = m_aData.GetAt(nFontIndex)) {
      return pData->sFontName;
    }
  }

  return "";
}

FX_BOOL CPWL_FontMap::KnowWord(int32_t nFontIndex, uint16_t word) {
  if (nFontIndex >= 0 && nFontIndex < m_aData.GetSize()) {
    if (m_aData.GetAt(nFontIndex)) {
      return CharCodeFromUnicode(nFontIndex, word) >= 0;
    }
  }

  return FALSE;
}

int32_t CPWL_FontMap::GetWordFontIndex(uint16_t word,
                                       int32_t nCharset,
                                       int32_t nFontIndex) {
  if (nFontIndex > 0) {
    if (KnowWord(nFontIndex, word))
      return nFontIndex;
  } else {
    if (const CPWL_FontMap_Data* pData = GetFontMapData(0)) {
      if (nCharset == DEFAULT_CHARSET || pData->nCharset == SYMBOL_CHARSET ||
          nCharset == pData->nCharset) {
        if (KnowWord(0, word))
          return 0;
      }
    }
  }

  int32_t nNewFontIndex =
      GetFontIndex(GetNativeFontName(nCharset), nCharset, TRUE);
  if (nNewFontIndex >= 0) {
    if (KnowWord(nNewFontIndex, word))
      return nNewFontIndex;
  }
  nNewFontIndex = GetFontIndex("Arial Unicode MS", DEFAULT_CHARSET, FALSE);
  if (nNewFontIndex >= 0) {
    if (KnowWord(nNewFontIndex, word))
      return nNewFontIndex;
  }
  return -1;
}

int32_t CPWL_FontMap::CharCodeFromUnicode(int32_t nFontIndex, uint16_t word) {
  CPWL_FontMap_Data* pData = m_aData.GetAt(nFontIndex);
  if (!pData)
    return -1;

  if (!pData->pFont)
    return -1;

  if (pData->pFont->IsUnicodeCompatible())
    return pData->pFont->CharCodeFromUnicode(word);

  return word < 0xFF ? word : -1;
}

CFX_ByteString CPWL_FontMap::GetNativeFontName(int32_t nCharset) {
  // searching native font is slow, so we must save time
  for (int32_t i = 0, sz = m_aNativeFont.GetSize(); i < sz; i++) {
    if (CPWL_FontMap_Native* pData = m_aNativeFont.GetAt(i)) {
      if (pData->nCharset == nCharset)
        return pData->sFontName;
    }
  }

  CFX_ByteString sNew = GetNativeFont(nCharset);

  if (!sNew.IsEmpty()) {
    CPWL_FontMap_Native* pNewData = new CPWL_FontMap_Native;
    pNewData->nCharset = nCharset;
    pNewData->sFontName = sNew;

    m_aNativeFont.Add(pNewData);
  }

  return sNew;
}

void CPWL_FontMap::Empty() {
  {
    for (int32_t i = 0, sz = m_aData.GetSize(); i < sz; i++)
      delete m_aData.GetAt(i);

    m_aData.RemoveAll();
  }
  {
    for (int32_t i = 0, sz = m_aNativeFont.GetSize(); i < sz; i++)
      delete m_aNativeFont.GetAt(i);

    m_aNativeFont.RemoveAll();
  }
}

void CPWL_FontMap::Initialize() {
  GetFontIndex(kDefaultFontName, ANSI_CHARSET, FALSE);
}

FX_BOOL CPWL_FontMap::IsStandardFont(const CFX_ByteString& sFontName) {
  for (size_t i = 0; i < FX_ArraySize(g_sDEStandardFontName); ++i) {
    if (sFontName == g_sDEStandardFontName[i])
      return TRUE;
  }

  return FALSE;
}

int32_t CPWL_FontMap::FindFont(const CFX_ByteString& sFontName,
                               int32_t nCharset) {
  for (int32_t i = 0, sz = m_aData.GetSize(); i < sz; i++) {
    if (CPWL_FontMap_Data* pData = m_aData.GetAt(i)) {
      if (nCharset == DEFAULT_CHARSET || nCharset == pData->nCharset) {
        if (sFontName.IsEmpty() || pData->sFontName == sFontName)
          return i;
      }
    }
  }

  return -1;
}

int32_t CPWL_FontMap::GetFontIndex(const CFX_ByteString& sFontName,
                                   int32_t nCharset,
                                   FX_BOOL bFind) {
  int32_t nFontIndex = FindFont(EncodeFontAlias(sFontName, nCharset), nCharset);
  if (nFontIndex >= 0)
    return nFontIndex;

  CFX_ByteString sAlias;
  CPDF_Font* pFont = nullptr;
  if (bFind)
    pFont = FindFontSameCharset(sAlias, nCharset);

  if (!pFont) {
    CFX_ByteString sTemp = sFontName;
    pFont = AddFontToDocument(GetDocument(), sTemp, nCharset);
    sAlias = EncodeFontAlias(sTemp, nCharset);
  }
  AddedFont(pFont, sAlias);
  return AddFontData(pFont, sAlias, nCharset);
}

int32_t CPWL_FontMap::GetPWLFontIndex(uint16_t word, int32_t nCharset) {
  int32_t nFind = -1;

  for (int32_t i = 0, sz = m_aData.GetSize(); i < sz; i++) {
    if (CPWL_FontMap_Data* pData = m_aData.GetAt(i)) {
      if (pData->nCharset == nCharset) {
        nFind = i;
        break;
      }
    }
  }

  CPDF_Font* pNewFont = GetPDFFont(nFind);

  if (!pNewFont)
    return -1;

  CFX_ByteString sAlias = EncodeFontAlias("Arial_Chrome", nCharset);
  AddedFont(pNewFont, sAlias);

  return AddFontData(pNewFont, sAlias, nCharset);
}

CPDF_Font* CPWL_FontMap::FindFontSameCharset(CFX_ByteString& sFontAlias,
                                             int32_t nCharset) {
  return nullptr;
}

int32_t CPWL_FontMap::AddFontData(CPDF_Font* pFont,
                                  const CFX_ByteString& sFontAlias,
                                  int32_t nCharset) {
  CPWL_FontMap_Data* pNewData = new CPWL_FontMap_Data;
  pNewData->pFont = pFont;
  pNewData->sFontName = sFontAlias;
  pNewData->nCharset = nCharset;

  m_aData.Add(pNewData);

  return m_aData.GetSize() - 1;
}

void CPWL_FontMap::AddedFont(CPDF_Font* pFont,
                             const CFX_ByteString& sFontAlias) {}

CFX_ByteString CPWL_FontMap::GetFontName(int32_t nFontIndex) {
  if (nFontIndex >= 0 && nFontIndex < m_aData.GetSize()) {
    if (CPWL_FontMap_Data* pData = m_aData.GetAt(nFontIndex)) {
      return pData->sFontName;
    }
  }

  return "";
}

CFX_ByteString CPWL_FontMap::GetNativeFont(int32_t nCharset) {
  if (nCharset == DEFAULT_CHARSET)
    nCharset = GetNativeCharset();

  CFX_ByteString sFontName = GetDefaultFontByCharset(nCharset);
  if (m_pSystemHandler) {
    if (m_pSystemHandler->FindNativeTrueTypeFont(sFontName))
      return sFontName;

    sFontName.clear();
  }
  return sFontName;
}

CPDF_Font* CPWL_FontMap::AddFontToDocument(CPDF_Document* pDoc,
                                           CFX_ByteString& sFontName,
                                           uint8_t nCharset) {
  if (IsStandardFont(sFontName))
    return AddStandardFont(pDoc, sFontName);

  return AddSystemFont(pDoc, sFontName, nCharset);
}

CPDF_Font* CPWL_FontMap::AddStandardFont(CPDF_Document* pDoc,
                                         CFX_ByteString& sFontName) {
  if (!pDoc)
    return nullptr;

  CPDF_Font* pFont = nullptr;

  if (sFontName == "ZapfDingbats") {
    pFont = pDoc->AddStandardFont(sFontName.c_str(), nullptr);
  } else {
    CPDF_FontEncoding fe(PDFFONT_ENCODING_WINANSI);
    pFont = pDoc->AddStandardFont(sFontName.c_str(), &fe);
  }

  return pFont;
}

CPDF_Font* CPWL_FontMap::AddSystemFont(CPDF_Document* pDoc,
                                       CFX_ByteString& sFontName,
                                       uint8_t nCharset) {
  if (!pDoc)
    return nullptr;

  if (sFontName.IsEmpty())
    sFontName = GetNativeFont(nCharset);
  if (nCharset == DEFAULT_CHARSET)
    nCharset = GetNativeCharset();

  if (m_pSystemHandler)
    return m_pSystemHandler->AddNativeTrueTypeFontToPDF(pDoc, sFontName,
                                                        nCharset);

  return nullptr;
}

CFX_ByteString CPWL_FontMap::EncodeFontAlias(const CFX_ByteString& sFontName,
                                             int32_t nCharset) {
  CFX_ByteString sPostfix;
  sPostfix.Format("_%02X", nCharset);
  return EncodeFontAlias(sFontName) + sPostfix;
}

CFX_ByteString CPWL_FontMap::EncodeFontAlias(const CFX_ByteString& sFontName) {
  CFX_ByteString sRet = sFontName;
  sRet.Remove(' ');
  return sRet;
}

int32_t CPWL_FontMap::GetFontMapCount() const {
  return m_aData.GetSize();
}

const CPWL_FontMap_Data* CPWL_FontMap::GetFontMapData(int32_t nIndex) const {
  if (nIndex >= 0 && nIndex < m_aData.GetSize()) {
    return m_aData.GetAt(nIndex);
  }

  return nullptr;
}

int32_t CPWL_FontMap::GetNativeCharset() {
  uint8_t nCharset = ANSI_CHARSET;
  int32_t iCodePage = FXSYS_GetACP();
  switch (iCodePage) {
    case 932:  // Japan
      nCharset = SHIFTJIS_CHARSET;
      break;
    case 936:  // Chinese (PRC, Singapore)
      nCharset = GB2312_CHARSET;
      break;
    case 950:  // Chinese (Taiwan; Hong Kong SAR, PRC)
      nCharset = GB2312_CHARSET;
      break;
    case 1252:  // Windows 3.1 Latin 1 (US, Western Europe)
      nCharset = ANSI_CHARSET;
      break;
    case 874:  // Thai
      nCharset = THAI_CHARSET;
      break;
    case 949:  // Korean
      nCharset = HANGUL_CHARSET;
      break;
    case 1200:  // Unicode (BMP of ISO 10646)
      nCharset = ANSI_CHARSET;
      break;
    case 1250:  // Windows 3.1 Eastern European
      nCharset = EASTEUROPE_CHARSET;
      break;
    case 1251:  // Windows 3.1 Cyrillic
      nCharset = RUSSIAN_CHARSET;
      break;
    case 1253:  // Windows 3.1 Greek
      nCharset = GREEK_CHARSET;
      break;
    case 1254:  // Windows 3.1 Turkish
      nCharset = TURKISH_CHARSET;
      break;
    case 1255:  // Hebrew
      nCharset = HEBREW_CHARSET;
      break;
    case 1256:  // Arabic
      nCharset = ARABIC_CHARSET;
      break;
    case 1257:  // Baltic
      nCharset = BALTIC_CHARSET;
      break;
    case 1258:  // Vietnamese
      nCharset = VIETNAMESE_CHARSET;
      break;
    case 1361:  // Korean(Johab)
      nCharset = JOHAB_CHARSET;
      break;
  }
  return nCharset;
}

const CPWL_FontMap::CharsetFontMap CPWL_FontMap::defaultTTFMap[] = {
    {ANSI_CHARSET, "Helvetica"},      {GB2312_CHARSET, "SimSun"},
    {CHINESEBIG5_CHARSET, "MingLiU"}, {SHIFTJIS_CHARSET, "MS Gothic"},
    {HANGUL_CHARSET, "Batang"},       {RUSSIAN_CHARSET, "Arial"},
#if _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_ || \
    _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
    {EASTEUROPE_CHARSET, "Arial"},
#else
    {EASTEUROPE_CHARSET, "Tahoma"},
#endif
    {ARABIC_CHARSET, "Arial"},        {-1, nullptr}};

CFX_ByteString CPWL_FontMap::GetDefaultFontByCharset(int32_t nCharset) {
  int i = 0;
  while (defaultTTFMap[i].charset != -1) {
    if (nCharset == defaultTTFMap[i].charset)
      return defaultTTFMap[i].fontname;
    ++i;
  }
  return "";
}

int32_t CPWL_FontMap::CharSetFromUnicode(uint16_t word, int32_t nOldCharset) {
  // to avoid CJK Font to show ASCII
  if (word < 0x7F)
    return ANSI_CHARSET;
  // follow the old charset
  if (nOldCharset != DEFAULT_CHARSET)
    return nOldCharset;

  // find new charset
  if ((word >= 0x4E00 && word <= 0x9FA5) ||
      (word >= 0xE7C7 && word <= 0xE7F3) ||
      (word >= 0x3000 && word <= 0x303F) ||
      (word >= 0x2000 && word <= 0x206F)) {
    return GB2312_CHARSET;
  }

  if (((word >= 0x3040) && (word <= 0x309F)) ||
      ((word >= 0x30A0) && (word <= 0x30FF)) ||
      ((word >= 0x31F0) && (word <= 0x31FF)) ||
      ((word >= 0xFF00) && (word <= 0xFFEF))) {
    return SHIFTJIS_CHARSET;
  }

  if (((word >= 0xAC00) && (word <= 0xD7AF)) ||
      ((word >= 0x1100) && (word <= 0x11FF)) ||
      ((word >= 0x3130) && (word <= 0x318F))) {
    return HANGUL_CHARSET;
  }

  if (word >= 0x0E00 && word <= 0x0E7F)
    return THAI_CHARSET;

  if ((word >= 0x0370 && word <= 0x03FF) || (word >= 0x1F00 && word <= 0x1FFF))
    return GREEK_CHARSET;

  if ((word >= 0x0600 && word <= 0x06FF) || (word >= 0xFB50 && word <= 0xFEFC))
    return ARABIC_CHARSET;

  if (word >= 0x0590 && word <= 0x05FF)
    return HEBREW_CHARSET;

  if (word >= 0x0400 && word <= 0x04FF)
    return RUSSIAN_CHARSET;

  if (word >= 0x0100 && word <= 0x024F)
    return EASTEUROPE_CHARSET;

  if (word >= 0x1E00 && word <= 0x1EFF)
    return VIETNAMESE_CHARSET;

  return ANSI_CHARSET;
}

CPWL_DocFontMap::CPWL_DocFontMap(CFX_SystemHandler* pSystemHandler,
                                 CPDF_Document* pAttachedDoc)
    : CPWL_FontMap(pSystemHandler), m_pAttachedDoc(pAttachedDoc) {}

CPWL_DocFontMap::~CPWL_DocFontMap() {}

CPDF_Document* CPWL_DocFontMap::GetDocument() {
  return m_pAttachedDoc;
}
