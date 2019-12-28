// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_font/cpdf_type3font.h"

#include "core/fpdfapi/fpdf_font/cpdf_type3char.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fxcrt/include/fx_system.h"
#include "third_party/base/stl_util.h"

CPDF_Type3Font::CPDF_Type3Font()
    : m_pCharProcs(nullptr),
      m_pPageResources(nullptr),
      m_pFontResources(nullptr) {
  FXSYS_memset(m_CharWidthL, 0, sizeof(m_CharWidthL));
}

CPDF_Type3Font::~CPDF_Type3Font() {
  for (auto it : m_CacheMap)
    delete it.second;
}

bool CPDF_Type3Font::IsType3Font() const {
  return true;
}

const CPDF_Type3Font* CPDF_Type3Font::AsType3Font() const {
  return this;
}

CPDF_Type3Font* CPDF_Type3Font::AsType3Font() {
  return this;
}

FX_BOOL CPDF_Type3Font::Load() {
  m_pFontResources = m_pFontDict->GetDictBy("Resources");
  CPDF_Array* pMatrix = m_pFontDict->GetArrayBy("FontMatrix");
  FX_FLOAT xscale = 1.0f, yscale = 1.0f;
  if (pMatrix) {
    m_FontMatrix = pMatrix->GetMatrix();
    xscale = m_FontMatrix.a;
    yscale = m_FontMatrix.d;
  }
  CPDF_Array* pBBox = m_pFontDict->GetArrayBy("FontBBox");
  if (pBBox) {
    m_FontBBox.left = (int32_t)(pBBox->GetNumberAt(0) * xscale * 1000);
    m_FontBBox.bottom = (int32_t)(pBBox->GetNumberAt(1) * yscale * 1000);
    m_FontBBox.right = (int32_t)(pBBox->GetNumberAt(2) * xscale * 1000);
    m_FontBBox.top = (int32_t)(pBBox->GetNumberAt(3) * yscale * 1000);
  }
  int StartChar = m_pFontDict->GetIntegerBy("FirstChar");
  CPDF_Array* pWidthArray = m_pFontDict->GetArrayBy("Widths");
  if (pWidthArray && (StartChar >= 0 && StartChar < 256)) {
    size_t count = pWidthArray->GetCount();
    if (count > 256)
      count = 256;
    if (StartChar + count > 256)
      count = 256 - StartChar;
    for (size_t i = 0; i < count; i++) {
      m_CharWidthL[StartChar + i] =
          FXSYS_round(pWidthArray->GetNumberAt(i) * xscale * 1000);
    }
  }
  m_pCharProcs = m_pFontDict->GetDictBy("CharProcs");
  CPDF_Object* pEncoding = m_pFontDict->GetDirectObjectBy("Encoding");
  if (pEncoding) {
    LoadPDFEncoding(pEncoding, m_BaseEncoding, m_pCharNames, FALSE, FALSE);
    if (m_pCharNames) {
      for (int i = 0; i < 256; i++) {
        m_Encoding.m_Unicodes[i] =
            PDF_UnicodeFromAdobeName(m_pCharNames[i].c_str());
        if (m_Encoding.m_Unicodes[i] == 0) {
          m_Encoding.m_Unicodes[i] = i;
        }
      }
    }
  }
  return TRUE;
}

void CPDF_Type3Font::CheckType3FontMetrics() {
  CheckFontMetrics();
}

CPDF_Type3Char* CPDF_Type3Font::LoadChar(uint32_t charcode, int level) {
  if (level >= _FPDF_MAX_TYPE3_FORM_LEVEL_)
    return nullptr;

  auto it = m_CacheMap.find(charcode);
  if (it != m_CacheMap.end())
    return it->second;

  const FX_CHAR* name =
      GetAdobeCharName(m_BaseEncoding, m_pCharNames, charcode);
  if (!name)
    return nullptr;

  CPDF_Stream* pStream =
      ToStream(m_pCharProcs ? m_pCharProcs->GetDirectObjectBy(name) : nullptr);
  if (!pStream)
    return nullptr;

  std::unique_ptr<CPDF_Type3Char> pNewChar(new CPDF_Type3Char(new CPDF_Form(
      m_pDocument, m_pFontResources ? m_pFontResources : m_pPageResources,
      pStream, nullptr)));

  // This can trigger recursion into this method. The content of |m_CacheMap|
  // can change as a result. Thus after it returns, check the cache again for
  // a cache hit.
  pNewChar->m_pForm->ParseContent(nullptr, nullptr, pNewChar.get(), level + 1);
  it = m_CacheMap.find(charcode);
  if (it != m_CacheMap.end())
    return it->second;

  FX_FLOAT scale = m_FontMatrix.GetXUnit();
  pNewChar->m_Width = (int32_t)(pNewChar->m_Width * scale + 0.5f);
  FX_RECT& rcBBox = pNewChar->m_BBox;
  CFX_FloatRect char_rect(
      (FX_FLOAT)rcBBox.left / 1000.0f, (FX_FLOAT)rcBBox.bottom / 1000.0f,
      (FX_FLOAT)rcBBox.right / 1000.0f, (FX_FLOAT)rcBBox.top / 1000.0f);
  if (rcBBox.right <= rcBBox.left || rcBBox.bottom >= rcBBox.top)
    char_rect = pNewChar->m_pForm->CalcBoundingBox();

  char_rect.Transform(&m_FontMatrix);
  rcBBox.left = FXSYS_round(char_rect.left * 1000);
  rcBBox.right = FXSYS_round(char_rect.right * 1000);
  rcBBox.top = FXSYS_round(char_rect.top * 1000);
  rcBBox.bottom = FXSYS_round(char_rect.bottom * 1000);

  ASSERT(!pdfium::ContainsKey(m_CacheMap, charcode));
  CPDF_Type3Char* pCachedChar = pNewChar.release();
  m_CacheMap[charcode] = pCachedChar;
  if (pCachedChar->m_pForm->GetPageObjectList()->empty())
    pCachedChar->m_pForm.reset();
  return pCachedChar;
}

int CPDF_Type3Font::GetCharWidthF(uint32_t charcode, int level) {
  if (charcode >= FX_ArraySize(m_CharWidthL))
    charcode = 0;

  if (m_CharWidthL[charcode])
    return m_CharWidthL[charcode];

  const CPDF_Type3Char* pChar = LoadChar(charcode, level);
  return pChar ? pChar->m_Width : 0;
}

FX_RECT CPDF_Type3Font::GetCharBBox(uint32_t charcode, int level) {
  const CPDF_Type3Char* pChar = LoadChar(charcode, level);
  return pChar ? pChar->m_BBox : FX_RECT();
}
