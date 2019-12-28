// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include <algorithm>

#include "core/fxge/android/fpf_skiafont.h"
#include "core/fxge/android/fpf_skiafontmgr.h"
#include "core/fxge/include/fx_freetype.h"

#define FPF_EM_ADJUST(em, a) (em == 0 ? (a) : (a)*1000 / em)

CFPF_SkiaFont::CFPF_SkiaFont()
    : m_pFontMgr(nullptr),
      m_pFontDes(nullptr),
      m_Face(nullptr),
      m_dwStyle(0),
      m_uCharset(0),
      m_dwRefCount(0) {}

CFPF_SkiaFont::~CFPF_SkiaFont() {
  if (m_Face)
    FXFT_Done_Face(m_Face);
}

void CFPF_SkiaFont::Release() {
  if (--m_dwRefCount == 0)
    delete this;
}

CFPF_SkiaFont* CFPF_SkiaFont::Retain() {
  m_dwRefCount++;
  return this;
}

CFX_ByteString CFPF_SkiaFont::GetFamilyName() {
  if (!m_Face)
    return CFX_ByteString();
  return CFX_ByteString(FXFT_Get_Face_Family_Name(m_Face));
}

CFX_ByteString CFPF_SkiaFont::GetPsName() {
  if (!m_Face)
    return CFX_ByteString();
  return FXFT_Get_Postscript_Name(m_Face);
}

int32_t CFPF_SkiaFont::GetGlyphIndex(FX_WCHAR wUnicode) {
  if (!m_Face)
    return wUnicode;
  if (FXFT_Select_Charmap(m_Face, FXFT_ENCODING_UNICODE))
    return 0;
  return FXFT_Get_Char_Index(m_Face, wUnicode);
}

int32_t CFPF_SkiaFont::GetGlyphWidth(int32_t iGlyphIndex) {
  if (!m_Face)
    return 0;
  if (FXFT_Load_Glyph(
          m_Face, iGlyphIndex,
          FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
    return 0;
  }
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Glyph_HoriAdvance(m_Face));
}

int32_t CFPF_SkiaFont::GetAscent() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Ascender(m_Face));
}

int32_t CFPF_SkiaFont::GetDescent() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Descender(m_Face));
}

FX_BOOL CFPF_SkiaFont::GetGlyphBBox(int32_t iGlyphIndex, FX_RECT& rtBBox) {
  if (!m_Face)
    return FALSE;
  if (FXFT_Is_Face_Tricky(m_Face)) {
    if (FXFT_Set_Char_Size(m_Face, 0, 1000 * 64, 72, 72))
      return FALSE;
    if (FXFT_Load_Glyph(m_Face, iGlyphIndex,
                        FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
      FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
      return FALSE;
    }
    FXFT_Glyph glyph;
    if (FXFT_Get_Glyph(m_Face->glyph, &glyph)) {
      FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
      return FALSE;
    }
    FXFT_BBox cbox;
    FXFT_Glyph_Get_CBox(glyph, FXFT_GLYPH_BBOX_PIXELS, &cbox);
    int32_t x_ppem = m_Face->size->metrics.x_ppem;
    int32_t y_ppem = m_Face->size->metrics.y_ppem;
    rtBBox.left = FPF_EM_ADJUST(x_ppem, cbox.xMin);
    rtBBox.right = FPF_EM_ADJUST(x_ppem, cbox.xMax);
    rtBBox.top = FPF_EM_ADJUST(y_ppem, cbox.yMax);
    rtBBox.bottom = FPF_EM_ADJUST(y_ppem, cbox.yMin);
    rtBBox.top = std::min(rtBBox.top, GetAscent());
    rtBBox.bottom = std::max(rtBBox.bottom, GetDescent());
    FXFT_Done_Glyph(glyph);
    return FXFT_Set_Pixel_Sizes(m_Face, 0, 64) == 0;
  }
  if (FXFT_Load_Glyph(
          m_Face, iGlyphIndex,
          FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
    return FALSE;
  }
  rtBBox.left = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                              FXFT_Get_Glyph_HoriBearingX(m_Face));
  rtBBox.bottom = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                                FXFT_Get_Glyph_HoriBearingY(m_Face));
  rtBBox.right = FPF_EM_ADJUST(
      FXFT_Get_Face_UnitsPerEM(m_Face),
      FXFT_Get_Glyph_HoriBearingX(m_Face) + FXFT_Get_Glyph_Width(m_Face));
  rtBBox.top = FPF_EM_ADJUST(
      FXFT_Get_Face_UnitsPerEM(m_Face),
      FXFT_Get_Glyph_HoriBearingY(m_Face) - FXFT_Get_Glyph_Height(m_Face));
  return TRUE;
}

FX_BOOL CFPF_SkiaFont::GetBBox(FX_RECT& rtBBox) {
  if (!m_Face) {
    return FALSE;
  }
  rtBBox.left = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                              FXFT_Get_Face_xMin(m_Face));
  rtBBox.top = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                             FXFT_Get_Face_yMin(m_Face));
  rtBBox.right = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                               FXFT_Get_Face_xMax(m_Face));
  rtBBox.bottom = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                                FXFT_Get_Face_yMax(m_Face));
  return TRUE;
}

int32_t CFPF_SkiaFont::GetHeight() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Height(m_Face));
}

int32_t CFPF_SkiaFont::GetItalicAngle() const {
  if (!m_Face)
    return 0;

  TT_Postscript* ttInfo =
      (TT_Postscript*)FT_Get_Sfnt_Table(m_Face, ft_sfnt_post);
  if (ttInfo)
    return ttInfo->italicAngle;
  return 0;
}

uint32_t CFPF_SkiaFont::GetFontData(uint32_t dwTable,
                                    uint8_t* pBuffer,
                                    uint32_t dwSize) {
  if (!m_Face)
    return 0;

  FT_ULong ulSize = pdfium::base::checked_cast<FT_ULong>(dwSize);
  if (FXFT_Load_Sfnt_Table(m_Face, dwTable, 0, pBuffer, &ulSize))
    return 0;
  return pdfium::base::checked_cast<uint32_t>(ulSize);
}

FX_BOOL CFPF_SkiaFont::InitFont(CFPF_SkiaFontMgr* pFontMgr,
                                CFPF_SkiaFontDescriptor* pFontDes,
                                const CFX_ByteStringC& bsFamily,
                                uint32_t dwStyle,
                                uint8_t uCharset) {
  if (!pFontMgr || !pFontDes)
    return FALSE;

  switch (pFontDes->GetType()) {
    case FPF_SKIAFONTTYPE_Path: {
      CFPF_SkiaPathFont* pFont = (CFPF_SkiaPathFont*)pFontDes;
      m_Face = pFontMgr->GetFontFace(pFont->m_pPath, pFont->m_iFaceIndex);
      break;
    }
    case FPF_SKIAFONTTYPE_File: {
      CFPF_SkiaFileFont* pFont = (CFPF_SkiaFileFont*)pFontDes;
      m_Face = pFontMgr->GetFontFace(pFont->m_pFile, pFont->m_iFaceIndex);
      break;
    }
    case FPF_SKIAFONTTYPE_Buffer: {
      CFPF_SkiaBufferFont* pFont = (CFPF_SkiaBufferFont*)pFontDes;
      m_Face = pFontMgr->GetFontFace((const uint8_t*)pFont->m_pBuffer,
                                     pFont->m_szBuffer, pFont->m_iFaceIndex);
      break;
    }
    default:
      return FALSE;
  }
  if (!m_Face)
    return FALSE;

  m_dwStyle = dwStyle;
  m_uCharset = uCharset;
  m_pFontMgr = pFontMgr;
  m_pFontDes = pFontDes;
  m_dwRefCount = 1;
  return TRUE;
}

#endif  // _FX_OS_ == _FX_ANDROID_
