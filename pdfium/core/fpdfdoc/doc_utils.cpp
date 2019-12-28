// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <algorithm>
#include <vector>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfdoc/doc_utils.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxge/include/fx_font.h"

namespace {

const int FPDFDOC_UTILS_MAXRECURSION = 32;

CPDF_Object* SearchNumberNode(const CPDF_Dictionary* pNode, int num) {
  CPDF_Array* pLimits = pNode->GetArrayBy("Limits");
  if (pLimits &&
      (num < pLimits->GetIntegerAt(0) || num > pLimits->GetIntegerAt(1))) {
    return nullptr;
  }
  CPDF_Array* pNumbers = pNode->GetArrayBy("Nums");
  if (pNumbers) {
    for (size_t i = 0; i < pNumbers->GetCount() / 2; i++) {
      int index = pNumbers->GetIntegerAt(i * 2);
      if (num == index) {
        return pNumbers->GetDirectObjectAt(i * 2 + 1);
      }
      if (index > num) {
        break;
      }
    }
    return nullptr;
  }
  CPDF_Array* pKids = pNode->GetArrayBy("Kids");
  if (!pKids) {
    return nullptr;
  }
  for (size_t i = 0; i < pKids->GetCount(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid) {
      continue;
    }
    CPDF_Object* pFound = SearchNumberNode(pKid, num);
    if (pFound) {
      return pFound;
    }
  }
  return nullptr;
}

}  // namespace

CPDF_Object* CPDF_NumberTree::LookupValue(int num) const {
  return SearchNumberNode(m_pRoot, num);
}

CFX_WideString GetFullName(CPDF_Dictionary* pFieldDict) {
  CFX_WideString full_name;
  CPDF_Dictionary* pLevel = pFieldDict;
  while (pLevel) {
    CFX_WideString short_name = pLevel->GetUnicodeTextBy("T");
    if (short_name != L"") {
      if (full_name == L"") {
        full_name = short_name;
      } else {
        full_name = short_name + L"." + full_name;
      }
    }
    pLevel = pLevel->GetDictBy("Parent");
  }
  return full_name;
}
FX_BOOL CPDF_DefaultAppearance::HasFont() {
  if (m_csDA.IsEmpty()) {
    return FALSE;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  return syntax.FindTagParamFromStart("Tf", 2);
}
CFX_ByteString CPDF_DefaultAppearance::GetFontString() {
  CFX_ByteString csFont;
  if (m_csDA.IsEmpty()) {
    return csFont;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart("Tf", 2)) {
    csFont += syntax.GetWord();
    csFont += " ";
    csFont += syntax.GetWord();
    csFont += " ";
    csFont += syntax.GetWord();
  }
  return csFont;
}
void CPDF_DefaultAppearance::GetFont(CFX_ByteString& csFontNameTag,
                                     FX_FLOAT& fFontSize) {
  csFontNameTag = "";
  fFontSize = 0;
  if (m_csDA.IsEmpty()) {
    return;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart("Tf", 2)) {
    csFontNameTag = CFX_ByteString(syntax.GetWord());
    csFontNameTag.Delete(0, 1);
    fFontSize = FX_atof(syntax.GetWord());
  }
  csFontNameTag = PDF_NameDecode(csFontNameTag);
}
FX_BOOL CPDF_DefaultAppearance::HasColor(FX_BOOL bStrokingOperation) {
  if (m_csDA.IsEmpty()) {
    return FALSE;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "G" : "g", 1)) {
    return TRUE;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "RG" : "rg", 3)) {
    return TRUE;
  }
  return syntax.FindTagParamFromStart(bStrokingOperation ? "K" : "k", 4);
}
CFX_ByteString CPDF_DefaultAppearance::GetColorString(
    FX_BOOL bStrokingOperation) {
  CFX_ByteString csColor;
  if (m_csDA.IsEmpty()) {
    return csColor;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "G" : "g", 1)) {
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    return csColor;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "RG" : "rg", 3)) {
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    return csColor;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "K" : "k", 4)) {
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
    csColor += " ";
    csColor += syntax.GetWord();
  }
  return csColor;
}
void CPDF_DefaultAppearance::GetColor(int& iColorType,
                                      FX_FLOAT fc[4],
                                      FX_BOOL bStrokingOperation) {
  iColorType = COLORTYPE_TRANSPARENT;
  for (int c = 0; c < 4; c++) {
    fc[c] = 0;
  }
  if (m_csDA.IsEmpty()) {
    return;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "G" : "g", 1)) {
    iColorType = COLORTYPE_GRAY;
    fc[0] = FX_atof(syntax.GetWord());
    return;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "RG" : "rg", 3)) {
    iColorType = COLORTYPE_RGB;
    fc[0] = FX_atof(syntax.GetWord());
    fc[1] = FX_atof(syntax.GetWord());
    fc[2] = FX_atof(syntax.GetWord());
    return;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "K" : "k", 4)) {
    iColorType = COLORTYPE_CMYK;
    fc[0] = FX_atof(syntax.GetWord());
    fc[1] = FX_atof(syntax.GetWord());
    fc[2] = FX_atof(syntax.GetWord());
    fc[3] = FX_atof(syntax.GetWord());
  }
}
void CPDF_DefaultAppearance::GetColor(FX_ARGB& color,
                                      int& iColorType,
                                      FX_BOOL bStrokingOperation) {
  color = 0;
  iColorType = COLORTYPE_TRANSPARENT;
  if (m_csDA.IsEmpty()) {
    return;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "G" : "g", 1)) {
    iColorType = COLORTYPE_GRAY;
    FX_FLOAT g = FX_atof(syntax.GetWord()) * 255 + 0.5f;
    color = ArgbEncode(255, (int)g, (int)g, (int)g);
    return;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "RG" : "rg", 3)) {
    iColorType = COLORTYPE_RGB;
    FX_FLOAT r = FX_atof(syntax.GetWord()) * 255 + 0.5f;
    FX_FLOAT g = FX_atof(syntax.GetWord()) * 255 + 0.5f;
    FX_FLOAT b = FX_atof(syntax.GetWord()) * 255 + 0.5f;
    color = ArgbEncode(255, (int)r, (int)g, (int)b);
    return;
  }
  if (syntax.FindTagParamFromStart(bStrokingOperation ? "K" : "k", 4)) {
    iColorType = COLORTYPE_CMYK;
    FX_FLOAT c = FX_atof(syntax.GetWord());
    FX_FLOAT m = FX_atof(syntax.GetWord());
    FX_FLOAT y = FX_atof(syntax.GetWord());
    FX_FLOAT k = FX_atof(syntax.GetWord());
    FX_FLOAT r = 1.0f - std::min(1.0f, c + k);
    FX_FLOAT g = 1.0f - std::min(1.0f, m + k);
    FX_FLOAT b = 1.0f - std::min(1.0f, y + k);
    color = ArgbEncode(255, (int)(r * 255 + 0.5f), (int)(g * 255 + 0.5f),
                       (int)(b * 255 + 0.5f));
  }
}
FX_BOOL CPDF_DefaultAppearance::HasTextMatrix() {
  if (m_csDA.IsEmpty()) {
    return FALSE;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  return syntax.FindTagParamFromStart("Tm", 6);
}
CFX_ByteString CPDF_DefaultAppearance::GetTextMatrixString() {
  CFX_ByteString csTM;
  if (m_csDA.IsEmpty()) {
    return csTM;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart("Tm", 6)) {
    for (int i = 0; i < 6; i++) {
      csTM += syntax.GetWord();
      csTM += " ";
    }
    csTM += syntax.GetWord();
  }
  return csTM;
}
CFX_Matrix CPDF_DefaultAppearance::GetTextMatrix() {
  CFX_Matrix tm;
  if (m_csDA.IsEmpty()) {
    return tm;
  }
  CPDF_SimpleParser syntax(m_csDA.AsStringC());
  if (syntax.FindTagParamFromStart("Tm", 6)) {
    FX_FLOAT f[6];
    for (int i = 0; i < 6; i++) {
      f[i] = FX_atof(syntax.GetWord());
    }
    tm.Set(f[0], f[1], f[2], f[3], f[4], f[5]);
  }
  return tm;
}

void InitInterFormDict(CPDF_Dictionary*& pFormDict, CPDF_Document* pDocument) {
  if (!pDocument)
    return;

  if (!pFormDict) {
    pFormDict = new CPDF_Dictionary;
    uint32_t dwObjNum = pDocument->AddIndirectObject(pFormDict);
    CPDF_Dictionary* pRoot = pDocument->GetRoot();
    pRoot->SetAtReference("AcroForm", pDocument, dwObjNum);
  }
  CFX_ByteString csDA;
  if (!pFormDict->KeyExist("DR")) {
    CFX_ByteString csBaseName;
    CFX_ByteString csDefault;
    uint8_t charSet = CPDF_InterForm::GetNativeCharSet();
    CPDF_Font* pFont = CPDF_InterForm::AddStandardFont(pDocument, "Helvetica");
    if (pFont) {
      AddInterFormFont(pFormDict, pDocument, pFont, csBaseName);
      csDefault = csBaseName;
    }
    if (charSet != FXFONT_ANSI_CHARSET) {
      CFX_ByteString csFontName =
          CPDF_InterForm::GetNativeFont(charSet, nullptr);
      if (!pFont || csFontName != "Helvetica") {
        pFont = CPDF_InterForm::AddNativeFont(pDocument);
        if (pFont) {
          csBaseName = "";
          AddInterFormFont(pFormDict, pDocument, pFont, csBaseName);
          csDefault = csBaseName;
        }
      }
    }
    if (pFont) {
      csDA = "/" + PDF_NameEncode(csDefault) + " 0 Tf";
    }
  }
  if (!csDA.IsEmpty()) {
    csDA += " ";
  }
  csDA += "0 g";
  if (!pFormDict->KeyExist("DA")) {
    pFormDict->SetAtString("DA", csDA);
  }
}
uint32_t CountInterFormFonts(CPDF_Dictionary* pFormDict) {
  if (!pFormDict) {
    return 0;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return 0;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return 0;
  }
  uint32_t dwCount = 0;
  for (const auto& it : *pFonts) {
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    if (CPDF_Dictionary* pDirect = ToDictionary(pObj->GetDirect())) {
      if (pDirect->GetStringBy("Type") == "Font") {
        dwCount++;
      }
    }
  }
  return dwCount;
}
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            uint32_t index,
                            CFX_ByteString& csNameTag) {
  if (!pFormDict) {
    return nullptr;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return nullptr;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return nullptr;
  }
  uint32_t dwCount = 0;
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font")
      continue;
    if (dwCount == index) {
      csNameTag = csKey;
      return pDocument->LoadFont(pElement);
    }
    dwCount++;
  }
  return nullptr;
}
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            CFX_ByteString csNameTag) {
  CFX_ByteString csAlias = PDF_NameDecode(csNameTag);
  if (!pFormDict || csAlias.IsEmpty()) {
    return nullptr;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return nullptr;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return nullptr;
  }
  CPDF_Dictionary* pElement = pFonts->GetDictBy(csAlias);
  if (!pElement) {
    return nullptr;
  }
  if (pElement->GetStringBy("Type") == "Font") {
    return pDocument->LoadFont(pElement);
  }
  return nullptr;
}
CPDF_Font* GetInterFormFont(CPDF_Dictionary* pFormDict,
                            CPDF_Document* pDocument,
                            CFX_ByteString csFontName,
                            CFX_ByteString& csNameTag) {
  if (!pFormDict || csFontName.IsEmpty()) {
    return nullptr;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return nullptr;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return nullptr;
  }
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font")
      continue;

    CPDF_Font* pFind = pDocument->LoadFont(pElement);
    if (!pFind)
      continue;

    CFX_ByteString csBaseFont;
    csBaseFont = pFind->GetBaseFont();
    csBaseFont.Remove(' ');
    if (csBaseFont == csFontName) {
      csNameTag = csKey;
      return pFind;
    }
  }
  return nullptr;
}
CPDF_Font* GetNativeInterFormFont(CPDF_Dictionary* pFormDict,
                                  CPDF_Document* pDocument,
                                  uint8_t charSet,
                                  CFX_ByteString& csNameTag) {
  if (!pFormDict) {
    return nullptr;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return nullptr;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return nullptr;
  }
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font")
      continue;
    CPDF_Font* pFind = pDocument->LoadFont(pElement);
    if (!pFind) {
      continue;
    }
    CFX_SubstFont* pSubst = pFind->GetSubstFont();
    if (!pSubst) {
      continue;
    }
    if (pSubst->m_Charset == (int)charSet) {
      csNameTag = csKey;
      return pFind;
    }
  }
  return nullptr;
}

CPDF_Font* GetNativeInterFormFont(CPDF_Dictionary* pFormDict,
                                  CPDF_Document* pDocument,
                                  CFX_ByteString& csNameTag) {
  csNameTag.clear();
  uint8_t charSet = CPDF_InterForm::GetNativeCharSet();
  CPDF_Font* pFont = GetDefaultInterFormFont(pFormDict, pDocument);
  if (pFont) {
    CFX_SubstFont* pSubst = pFont->GetSubstFont();
    if (pSubst && pSubst->m_Charset == (int)charSet) {
      FindInterFormFont(pFormDict, pFont, csNameTag);
      return pFont;
    }
  }
  return GetNativeInterFormFont(pFormDict, pDocument, charSet, csNameTag);
}

FX_BOOL FindInterFormFont(CPDF_Dictionary* pFormDict,
                          const CPDF_Font* pFont,
                          CFX_ByteString& csNameTag) {
  if (!pFormDict || !pFont) {
    return FALSE;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return FALSE;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return FALSE;
  }
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font") {
      continue;
    }
    if (pFont->GetFontDict() == pElement) {
      csNameTag = csKey;
      return TRUE;
    }
  }
  return FALSE;
}
FX_BOOL FindInterFormFont(CPDF_Dictionary* pFormDict,
                          CPDF_Document* pDocument,
                          CFX_ByteString csFontName,
                          CPDF_Font*& pFont,
                          CFX_ByteString& csNameTag) {
  if (!pFormDict) {
    return FALSE;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return FALSE;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return FALSE;
  }
  if (csFontName.GetLength() > 0) {
    csFontName.Remove(' ');
  }
  for (const auto& it : *pFonts) {
    const CFX_ByteString& csKey = it.first;
    CPDF_Object* pObj = it.second;
    if (!pObj) {
      continue;
    }
    CPDF_Dictionary* pElement = ToDictionary(pObj->GetDirect());
    if (!pElement)
      continue;
    if (pElement->GetStringBy("Type") != "Font") {
      continue;
    }
    pFont = pDocument->LoadFont(pElement);
    if (!pFont) {
      continue;
    }
    CFX_ByteString csBaseFont;
    csBaseFont = pFont->GetBaseFont();
    csBaseFont.Remove(' ');
    if (csBaseFont == csFontName) {
      csNameTag = csKey;
      return TRUE;
    }
  }
  return FALSE;
}
void AddInterFormFont(CPDF_Dictionary*& pFormDict,
                      CPDF_Document* pDocument,
                      const CPDF_Font* pFont,
                      CFX_ByteString& csNameTag) {
  if (!pFont) {
    return;
  }
  if (!pFormDict) {
    InitInterFormDict(pFormDict, pDocument);
  }
  CFX_ByteString csTag;
  if (FindInterFormFont(pFormDict, pFont, csTag)) {
    csNameTag = csTag;
    return;
  }
  if (!pFormDict) {
    InitInterFormDict(pFormDict, pDocument);
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    pDR = new CPDF_Dictionary;
    pFormDict->SetAt("DR", pDR);
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    pFonts = new CPDF_Dictionary;
    pDR->SetAt("Font", pFonts);
  }
  if (csNameTag.IsEmpty()) {
    csNameTag = pFont->GetBaseFont();
  }
  csNameTag.Remove(' ');
  csNameTag = CPDF_InterForm::GenerateNewResourceName(pDR, "Font", 4,
                                                      csNameTag.c_str());
  pFonts->SetAtReference(csNameTag, pDocument, pFont->GetFontDict());
}
CPDF_Font* AddNativeInterFormFont(CPDF_Dictionary*& pFormDict,
                                  CPDF_Document* pDocument,
                                  uint8_t charSet,
                                  CFX_ByteString& csNameTag) {
  if (!pFormDict) {
    InitInterFormDict(pFormDict, pDocument);
  }
  CFX_ByteString csTemp;
  CPDF_Font* pFont =
      GetNativeInterFormFont(pFormDict, pDocument, charSet, csTemp);
  if (pFont) {
    csNameTag = csTemp;
    return pFont;
  }
  CFX_ByteString csFontName = CPDF_InterForm::GetNativeFont(charSet);
  if (!csFontName.IsEmpty() &&
      FindInterFormFont(pFormDict, pDocument, csFontName, pFont, csNameTag)) {
    return pFont;
  }
  pFont = CPDF_InterForm::AddNativeFont(charSet, pDocument);
  if (pFont) {
    AddInterFormFont(pFormDict, pDocument, pFont, csNameTag);
  }
  return pFont;
}
CPDF_Font* AddNativeInterFormFont(CPDF_Dictionary*& pFormDict,
                                  CPDF_Document* pDocument,
                                  CFX_ByteString& csNameTag) {
  uint8_t charSet = CPDF_InterForm::GetNativeCharSet();
  return AddNativeInterFormFont(pFormDict, pDocument, charSet, csNameTag);
}
void RemoveInterFormFont(CPDF_Dictionary* pFormDict, const CPDF_Font* pFont) {
  if (!pFormDict || !pFont) {
    return;
  }
  CFX_ByteString csTag;
  if (!FindInterFormFont(pFormDict, pFont, csTag)) {
    return;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  pFonts->RemoveAt(csTag);
}

void RemoveInterFormFont(CPDF_Dictionary* pFormDict, CFX_ByteString csNameTag) {
  if (!pFormDict || csNameTag.IsEmpty()) {
    return;
  }
  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR) {
    return;
  }
  CPDF_Dictionary* pFonts = pDR->GetDictBy("Font");
  if (!pFonts) {
    return;
  }
  pFonts->RemoveAt(csNameTag);
}

CPDF_Font* GetDefaultInterFormFont(CPDF_Dictionary* pFormDict,
                                   CPDF_Document* pDocument) {
  if (!pFormDict) {
    return nullptr;
  }
  CPDF_DefaultAppearance cDA(pFormDict->GetStringBy("DA"));
  CFX_ByteString csFontNameTag;
  FX_FLOAT fFontSize;
  cDA.GetFont(csFontNameTag, fFontSize);
  return GetInterFormFont(pFormDict, pDocument, csFontNameTag);
}

CPDF_IconFit::ScaleMethod CPDF_IconFit::GetScaleMethod() {
  if (!m_pDict) {
    return Always;
  }
  CFX_ByteString csSW = m_pDict->GetStringBy("SW", "A");
  if (csSW == "B") {
    return Bigger;
  }
  if (csSW == "S") {
    return Smaller;
  }
  if (csSW == "N") {
    return Never;
  }
  return Always;
}
FX_BOOL CPDF_IconFit::IsProportionalScale() {
  if (!m_pDict) {
    return TRUE;
  }
  return m_pDict->GetStringBy("S", "P") != "A";
}
void CPDF_IconFit::GetIconPosition(FX_FLOAT& fLeft, FX_FLOAT& fBottom) {
  fLeft = fBottom = 0.5;
  if (!m_pDict) {
    return;
  }
  CPDF_Array* pA = m_pDict->GetArrayBy("A");
  if (pA) {
    uint32_t dwCount = pA->GetCount();
    if (dwCount > 0) {
      fLeft = pA->GetNumberAt(0);
    }
    if (dwCount > 1) {
      fBottom = pA->GetNumberAt(1);
    }
  }
}

bool CPDF_IconFit::GetFittingBounds() {
  return m_pDict ? m_pDict->GetBooleanBy("FB") : false;
}

std::vector<bool> SaveCheckedFieldStatus(CPDF_FormField* pField) {
  std::vector<bool> result;
  int iCount = pField->CountControls();
  for (int i = 0; i < iCount; ++i) {
    if (CPDF_FormControl* pControl = pField->GetControl(i))
      result.push_back(pControl->IsChecked());
  }
  return result;
}

CPDF_Object* FPDF_GetFieldAttr(CPDF_Dictionary* pFieldDict,
                               const FX_CHAR* name,
                               int nLevel) {
  if (nLevel > FPDFDOC_UTILS_MAXRECURSION) {
    return nullptr;
  }
  if (!pFieldDict) {
    return nullptr;
  }
  CPDF_Object* pAttr = pFieldDict->GetDirectObjectBy(name);
  if (pAttr) {
    return pAttr;
  }
  CPDF_Dictionary* pParent = pFieldDict->GetDictBy("Parent");
  if (!pParent) {
    return nullptr;
  }
  return FPDF_GetFieldAttr(pParent, name, nLevel + 1);
}
