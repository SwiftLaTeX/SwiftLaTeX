// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/pageint.h"

#include "core/fdrm/crypto/include/fx_crypt.h"
#include "core/fpdfapi/fpdf_font/cpdf_type1font.h"
#include "core/fpdfapi/fpdf_font/font_int.h"
#include "core/fpdfapi/fpdf_page/cpdf_pagemodule.h"
#include "core/fpdfapi/fpdf_page/cpdf_pattern.h"
#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"
#include "core/fpdfapi/fpdf_page/cpdf_tilingpattern.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "third_party/base/stl_util.h"

void CPDF_ModuleMgr::InitPageModule() {
  m_pPageModule.reset(new CPDF_PageModule);
}

CPDF_DocPageData::CPDF_DocPageData(CPDF_Document* pPDFDoc)
    : m_pPDFDoc(pPDFDoc), m_bForceClear(FALSE) {}

CPDF_DocPageData::~CPDF_DocPageData() {
  Clear(FALSE);
  Clear(TRUE);

  for (auto& it : m_PatternMap)
    delete it.second;
  m_PatternMap.clear();

  for (auto& it : m_FontMap)
    delete it.second;
  m_FontMap.clear();

  for (auto& it : m_ColorSpaceMap)
    delete it.second;
  m_ColorSpaceMap.clear();
}

void CPDF_DocPageData::Clear(FX_BOOL bForceRelease) {
  m_bForceClear = bForceRelease;

  for (auto& it : m_PatternMap) {
    CPDF_CountedPattern* ptData = it.second;
    if (!ptData->get())
      continue;

    if (bForceRelease || ptData->use_count() < 2)
      ptData->clear();
  }

  for (auto& it : m_FontMap) {
    CPDF_CountedFont* fontData = it.second;
    if (!fontData->get())
      continue;

    if (bForceRelease || fontData->use_count() < 2) {
      fontData->clear();
    }
  }

  for (auto& it : m_ColorSpaceMap) {
    CPDF_CountedColorSpace* csData = it.second;
    if (!csData->get())
      continue;

    if (bForceRelease || csData->use_count() < 2) {
      csData->get()->ReleaseCS();
      csData->reset(nullptr);
    }
  }

  for (auto it = m_IccProfileMap.begin(); it != m_IccProfileMap.end();) {
    auto curr_it = it++;
    CPDF_CountedIccProfile* ipData = curr_it->second;
    if (!ipData->get())
      continue;

    if (bForceRelease || ipData->use_count() < 2) {
      for (auto hash_it = m_HashProfileMap.begin();
           hash_it != m_HashProfileMap.end(); ++hash_it) {
        if (curr_it->first == hash_it->second) {
          m_HashProfileMap.erase(hash_it);
          break;
        }
      }
      delete ipData->get();
      delete ipData;
      m_IccProfileMap.erase(curr_it);
    }
  }

  for (auto it = m_FontFileMap.begin(); it != m_FontFileMap.end();) {
    auto curr_it = it++;
    CPDF_CountedStreamAcc* ftData = curr_it->second;
    if (!ftData->get())
      continue;

    if (bForceRelease || ftData->use_count() < 2) {
      delete ftData->get();
      delete ftData;
      m_FontFileMap.erase(curr_it);
    }
  }

  for (auto it = m_ImageMap.begin(); it != m_ImageMap.end();) {
    auto curr_it = it++;
    CPDF_CountedImage* imageData = curr_it->second;
    if (!imageData->get())
      continue;

    if (bForceRelease || imageData->use_count() < 2) {
      delete imageData->get();
      delete imageData;
      m_ImageMap.erase(curr_it);
    }
  }
}

CPDF_Font* CPDF_DocPageData::GetFont(CPDF_Dictionary* pFontDict,
                                     FX_BOOL findOnly) {
  if (!pFontDict) {
    return nullptr;
  }
  if (findOnly) {
    auto it = m_FontMap.find(pFontDict);
    if (it != m_FontMap.end() && it->second->get()) {
      return it->second->AddRef();
    }
    return nullptr;
  }

  CPDF_CountedFont* fontData = nullptr;
  auto it = m_FontMap.find(pFontDict);
  if (it != m_FontMap.end()) {
    fontData = it->second;
    if (fontData->get()) {
      return fontData->AddRef();
    }
  }

  CPDF_Font* pFont = CPDF_Font::CreateFontF(m_pPDFDoc, pFontDict);
  if (!pFont) {
    return nullptr;
  }
  if (!fontData) {
    fontData = new CPDF_CountedFont(pFont);
    m_FontMap[pFontDict] = fontData;
  } else {
    fontData->reset(pFont);
  }
  return fontData->AddRef();
}

CPDF_Font* CPDF_DocPageData::GetStandardFont(const CFX_ByteString& fontName,
                                             CPDF_FontEncoding* pEncoding) {
  if (fontName.IsEmpty())
    return nullptr;

  for (auto& it : m_FontMap) {
    CPDF_CountedFont* fontData = it.second;
    CPDF_Font* pFont = fontData->get();
    if (!pFont)
      continue;
    if (pFont->GetBaseFont() != fontName)
      continue;
    if (pFont->IsEmbedded())
      continue;
    if (!pFont->IsType1Font())
      continue;
    if (pFont->GetFontDict()->KeyExist("Widths"))
      continue;

    CPDF_Type1Font* pT1Font = pFont->AsType1Font();
    if (pEncoding && !pT1Font->GetEncoding()->IsIdentical(pEncoding))
      continue;

    return fontData->AddRef();
  }

  CPDF_Dictionary* pDict = new CPDF_Dictionary;
  pDict->SetAtName("Type", "Font");
  pDict->SetAtName("Subtype", "Type1");
  pDict->SetAtName("BaseFont", fontName);
  if (pEncoding) {
    pDict->SetAt("Encoding", pEncoding->Realize());
  }
  m_pPDFDoc->AddIndirectObject(pDict);
  CPDF_Font* pFont = CPDF_Font::CreateFontF(m_pPDFDoc, pDict);
  if (!pFont) {
    return nullptr;
  }
  CPDF_CountedFont* fontData = new CPDF_CountedFont(pFont);
  m_FontMap[pDict] = fontData;
  return fontData->AddRef();
}

void CPDF_DocPageData::ReleaseFont(CPDF_Dictionary* pFontDict) {
  if (!pFontDict)
    return;

  auto it = m_FontMap.find(pFontDict);
  if (it == m_FontMap.end())
    return;

  CPDF_CountedFont* fontData = it->second;
  if (fontData->get()) {
    fontData->RemoveRef();
    if (fontData->use_count() == 0) {
      fontData->clear();
    }
  }
}

CPDF_ColorSpace* CPDF_DocPageData::GetColorSpace(
    CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources) {
  std::set<CPDF_Object*> visited;
  return GetColorSpaceImpl(pCSObj, pResources, &visited);
}

CPDF_ColorSpace* CPDF_DocPageData::GetColorSpaceImpl(
    CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources,
    std::set<CPDF_Object*>* pVisited) {
  if (!pCSObj)
    return nullptr;

  if (pdfium::ContainsKey(*pVisited, pCSObj))
    return nullptr;

  if (pCSObj->IsName()) {
    CFX_ByteString name = pCSObj->GetString();
    CPDF_ColorSpace* pCS = CPDF_ColorSpace::ColorspaceFromName(name);
    if (!pCS && pResources) {
      CPDF_Dictionary* pList = pResources->GetDictBy("ColorSpace");
      if (pList) {
        pdfium::ScopedSetInsertion<CPDF_Object*> insertion(pVisited, pCSObj);
        return GetColorSpaceImpl(pList->GetDirectObjectBy(name), nullptr,
                                 pVisited);
      }
    }
    if (!pCS || !pResources)
      return pCS;

    CPDF_Dictionary* pColorSpaces = pResources->GetDictBy("ColorSpace");
    if (!pColorSpaces)
      return pCS;

    CPDF_Object* pDefaultCS = nullptr;
    switch (pCS->GetFamily()) {
      case PDFCS_DEVICERGB:
        pDefaultCS = pColorSpaces->GetDirectObjectBy("DefaultRGB");
        break;
      case PDFCS_DEVICEGRAY:
        pDefaultCS = pColorSpaces->GetDirectObjectBy("DefaultGray");
        break;
      case PDFCS_DEVICECMYK:
        pDefaultCS = pColorSpaces->GetDirectObjectBy("DefaultCMYK");
        break;
    }
    if (!pDefaultCS)
      return pCS;

    pdfium::ScopedSetInsertion<CPDF_Object*> insertion(pVisited, pCSObj);
    return GetColorSpaceImpl(pDefaultCS, nullptr, pVisited);
  }

  CPDF_Array* pArray = pCSObj->AsArray();
  if (!pArray || pArray->GetCount() == 0)
    return nullptr;

  if (pArray->GetCount() == 1) {
    pdfium::ScopedSetInsertion<CPDF_Object*> insertion(pVisited, pCSObj);
    return GetColorSpaceImpl(pArray->GetDirectObjectAt(0), pResources,
                             pVisited);
  }

  CPDF_CountedColorSpace* csData = nullptr;
  auto it = m_ColorSpaceMap.find(pCSObj);
  if (it != m_ColorSpaceMap.end()) {
    csData = it->second;
    if (csData->get()) {
      return csData->AddRef();
    }
  }

  CPDF_ColorSpace* pCS = CPDF_ColorSpace::Load(m_pPDFDoc, pArray);
  if (!pCS)
    return nullptr;

  if (!csData) {
    csData = new CPDF_CountedColorSpace(pCS);
    m_ColorSpaceMap[pCSObj] = csData;
  } else {
    csData->reset(pCS);
  }
  return csData->AddRef();
}

CPDF_ColorSpace* CPDF_DocPageData::GetCopiedColorSpace(CPDF_Object* pCSObj) {
  if (!pCSObj)
    return nullptr;

  auto it = m_ColorSpaceMap.find(pCSObj);
  if (it != m_ColorSpaceMap.end())
    return it->second->AddRef();

  return nullptr;
}

void CPDF_DocPageData::ReleaseColorSpace(CPDF_Object* pColorSpace) {
  if (!pColorSpace)
    return;

  auto it = m_ColorSpaceMap.find(pColorSpace);
  if (it == m_ColorSpaceMap.end())
    return;

  CPDF_CountedColorSpace* csData = it->second;
  if (csData->get()) {
    csData->RemoveRef();
    if (csData->use_count() == 0) {
      csData->get()->ReleaseCS();
      csData->reset(nullptr);
    }
  }
}

CPDF_Pattern* CPDF_DocPageData::GetPattern(CPDF_Object* pPatternObj,
                                           FX_BOOL bShading,
                                           const CFX_Matrix& matrix) {
  if (!pPatternObj)
    return nullptr;

  CPDF_CountedPattern* ptData = nullptr;
  auto it = m_PatternMap.find(pPatternObj);
  if (it != m_PatternMap.end()) {
    ptData = it->second;
    if (ptData->get()) {
      return ptData->AddRef();
    }
  }
  CPDF_Pattern* pPattern = nullptr;
  if (bShading) {
    pPattern = new CPDF_ShadingPattern(m_pPDFDoc, pPatternObj, TRUE, matrix);
  } else {
    CPDF_Dictionary* pDict = pPatternObj ? pPatternObj->GetDict() : nullptr;
    if (pDict) {
      int type = pDict->GetIntegerBy("PatternType");
      if (type == CPDF_Pattern::TILING) {
        pPattern = new CPDF_TilingPattern(m_pPDFDoc, pPatternObj, matrix);
      } else if (type == CPDF_Pattern::SHADING) {
        pPattern =
            new CPDF_ShadingPattern(m_pPDFDoc, pPatternObj, FALSE, matrix);
      }
    }
  }
  if (!pPattern)
    return nullptr;

  if (!ptData) {
    ptData = new CPDF_CountedPattern(pPattern);
    m_PatternMap[pPatternObj] = ptData;
  } else {
    ptData->reset(pPattern);
  }
  return ptData->AddRef();
}

void CPDF_DocPageData::ReleasePattern(CPDF_Object* pPatternObj) {
  if (!pPatternObj)
    return;

  auto it = m_PatternMap.find(pPatternObj);
  if (it == m_PatternMap.end())
    return;

  CPDF_CountedPattern* ptData = it->second;
  if (ptData->get()) {
    ptData->RemoveRef();
    if (ptData->use_count() == 0) {
      ptData->clear();
    }
  }
}

CPDF_Image* CPDF_DocPageData::GetImage(CPDF_Object* pImageStream) {
  if (!pImageStream)
    return nullptr;

  const uint32_t dwImageObjNum = pImageStream->GetObjNum();
  auto it = m_ImageMap.find(dwImageObjNum);
  if (it != m_ImageMap.end()) {
    return it->second->AddRef();
  }

  CPDF_Image* pImage = new CPDF_Image(m_pPDFDoc);
  pImage->LoadImageF(pImageStream->AsStream(), FALSE);

  CPDF_CountedImage* imageData = new CPDF_CountedImage(pImage);
  m_ImageMap[dwImageObjNum] = imageData;
  return imageData->AddRef();
}

void CPDF_DocPageData::ReleaseImage(CPDF_Object* pImageStream) {
  if (!pImageStream || !pImageStream->GetObjNum())
    return;

  auto it = m_ImageMap.find(pImageStream->GetObjNum());
  if (it == m_ImageMap.end())
    return;

  CPDF_CountedImage* image = it->second;
  if (!image)
    return;

  image->RemoveRef();
  if (image->use_count() == 0) {
    delete image->get();
    delete image;
    m_ImageMap.erase(it);
  }
}

CPDF_IccProfile* CPDF_DocPageData::GetIccProfile(
    CPDF_Stream* pIccProfileStream) {
  if (!pIccProfileStream)
    return nullptr;

  auto it = m_IccProfileMap.find(pIccProfileStream);
  if (it != m_IccProfileMap.end()) {
    return it->second->AddRef();
  }

  CPDF_StreamAcc stream;
  stream.LoadAllData(pIccProfileStream, FALSE);
  uint8_t digest[20];
  CRYPT_SHA1Generate(stream.GetData(), stream.GetSize(), digest);
  CFX_ByteString bsDigest(digest, 20);
  auto hash_it = m_HashProfileMap.find(bsDigest);
  if (hash_it != m_HashProfileMap.end()) {
    auto it_copied_stream = m_IccProfileMap.find(hash_it->second);
    return it_copied_stream->second->AddRef();
  }
  CPDF_IccProfile* pProfile =
      new CPDF_IccProfile(stream.GetData(), stream.GetSize());
  CPDF_CountedIccProfile* ipData = new CPDF_CountedIccProfile(pProfile);
  m_IccProfileMap[pIccProfileStream] = ipData;
  m_HashProfileMap[bsDigest] = pIccProfileStream;
  return ipData->AddRef();
}

void CPDF_DocPageData::ReleaseIccProfile(CPDF_IccProfile* pIccProfile) {
  ASSERT(pIccProfile);

  for (auto it = m_IccProfileMap.begin(); it != m_IccProfileMap.end(); ++it) {
    CPDF_CountedIccProfile* profile = it->second;
    if (profile->get() != pIccProfile)
      continue;

    profile->RemoveRef();
    if (profile->use_count() == 0) {
      delete profile->get();
      delete profile;
      m_IccProfileMap.erase(it);
      return;
    }
  }
}

CPDF_StreamAcc* CPDF_DocPageData::GetFontFileStreamAcc(
    CPDF_Stream* pFontStream) {
  ASSERT(pFontStream);

  auto it = m_FontFileMap.find(pFontStream);
  if (it != m_FontFileMap.end())
    return it->second->AddRef();

  CPDF_Dictionary* pFontDict = pFontStream->GetDict();
  int32_t org_size = pFontDict->GetIntegerBy("Length1") +
                     pFontDict->GetIntegerBy("Length2") +
                     pFontDict->GetIntegerBy("Length3");
  if (org_size < 0)
    org_size = 0;

  CPDF_StreamAcc* pFontFile = new CPDF_StreamAcc;
  pFontFile->LoadAllData(pFontStream, FALSE, org_size);

  CPDF_CountedStreamAcc* ftData = new CPDF_CountedStreamAcc(pFontFile);
  m_FontFileMap[pFontStream] = ftData;
  return ftData->AddRef();
}

void CPDF_DocPageData::ReleaseFontFileStreamAcc(CPDF_Stream* pFontStream,
                                                FX_BOOL bForce) {
  if (!pFontStream)
    return;

  auto it = m_FontFileMap.find(pFontStream);
  if (it == m_FontFileMap.end())
    return;

  CPDF_CountedStreamAcc* findData = it->second;
  if (!findData)
    return;

  findData->RemoveRef();
  if (findData->use_count() == 0 || bForce) {
    delete findData->get();
    delete findData;
    m_FontFileMap.erase(it);
  }
}

CPDF_CountedColorSpace* CPDF_DocPageData::FindColorSpacePtr(
    CPDF_Object* pCSObj) const {
  if (!pCSObj)
    return nullptr;

  auto it = m_ColorSpaceMap.find(pCSObj);
  return it != m_ColorSpaceMap.end() ? it->second : nullptr;
}

CPDF_CountedPattern* CPDF_DocPageData::FindPatternPtr(
    CPDF_Object* pPatternObj) const {
  if (!pPatternObj)
    return nullptr;

  auto it = m_PatternMap.find(pPatternObj);
  return it != m_PatternMap.end() ? it->second : nullptr;
}
