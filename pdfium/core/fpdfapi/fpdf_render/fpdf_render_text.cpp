// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_render/render_int.h"

#include <vector>

#include "core/fpdfapi/fpdf_font/cpdf_cidfont.h"
#include "core/fpdfapi/fpdf_font/cpdf_type3char.h"
#include "core/fpdfapi/fpdf_font/cpdf_type3font.h"
#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pathobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_textrenderer.h"
#include "core/fxge/include/fx_ge.h"

namespace {

struct CPDF_UniqueKeyGen {
  void Generate(int count, ...);
  FX_CHAR m_Key[128];
  int m_KeyLen;
};

void CPDF_UniqueKeyGen::Generate(int count, ...) {
  va_list argList;
  va_start(argList, count);
  for (int i = 0; i < count; i++) {
    int p = va_arg(argList, int);
    ((uint32_t*)m_Key)[i] = p;
  }
  va_end(argList);
  m_KeyLen = count * sizeof(uint32_t);
}

}  // namespace

CPDF_Type3Cache::CPDF_Type3Cache(CPDF_Type3Font* pFont) : m_pFont(pFont) {}

CPDF_Type3Cache::~CPDF_Type3Cache() {
  for (const auto& pair : m_SizeMap) {
    delete pair.second;
  }
  m_SizeMap.clear();
}

CFX_GlyphBitmap* CPDF_Type3Cache::LoadGlyph(uint32_t charcode,
                                            const CFX_Matrix* pMatrix,
                                            FX_FLOAT retinaScaleX,
                                            FX_FLOAT retinaScaleY) {
  CPDF_UniqueKeyGen keygen;
  keygen.Generate(
      4, FXSYS_round(pMatrix->a * 10000), FXSYS_round(pMatrix->b * 10000),
      FXSYS_round(pMatrix->c * 10000), FXSYS_round(pMatrix->d * 10000));
  CFX_ByteString FaceGlyphsKey(keygen.m_Key, keygen.m_KeyLen);
  CPDF_Type3Glyphs* pSizeCache;
  auto it = m_SizeMap.find(FaceGlyphsKey);
  if (it == m_SizeMap.end()) {
    pSizeCache = new CPDF_Type3Glyphs;
    m_SizeMap[FaceGlyphsKey] = pSizeCache;
  } else {
    pSizeCache = it->second;
  }
  auto it2 = pSizeCache->m_GlyphMap.find(charcode);
  if (it2 != pSizeCache->m_GlyphMap.end())
    return it2->second;

  CFX_GlyphBitmap* pGlyphBitmap =
      RenderGlyph(pSizeCache, charcode, pMatrix, retinaScaleX, retinaScaleY);
  pSizeCache->m_GlyphMap[charcode] = pGlyphBitmap;
  return pGlyphBitmap;
}

CPDF_Type3Glyphs::CPDF_Type3Glyphs()
    : m_TopBlueCount(0), m_BottomBlueCount(0) {}

CPDF_Type3Glyphs::~CPDF_Type3Glyphs() {
  for (const auto& pair : m_GlyphMap)
    delete pair.second;
}

static int _AdjustBlue(FX_FLOAT pos, int& count, int blues[]) {
  FX_FLOAT min_distance = 1000000.0f * 1.0f;
  int closest_pos = -1;
  for (int i = 0; i < count; i++) {
    FX_FLOAT distance = (FX_FLOAT)FXSYS_fabs(pos - (FX_FLOAT)blues[i]);
    if (distance < 1.0f * 80.0f / 100.0f && distance < min_distance) {
      min_distance = distance;
      closest_pos = i;
    }
  }
  if (closest_pos >= 0) {
    return blues[closest_pos];
  }
  int new_pos = FXSYS_round(pos);
  if (count == TYPE3_MAX_BLUES) {
    return new_pos;
  }
  blues[count++] = new_pos;
  return new_pos;
}
void CPDF_Type3Glyphs::AdjustBlue(FX_FLOAT top,
                                  FX_FLOAT bottom,
                                  int& top_line,
                                  int& bottom_line) {
  top_line = _AdjustBlue(top, m_TopBlueCount, m_TopBlue);
  bottom_line = _AdjustBlue(bottom, m_BottomBlueCount, m_BottomBlue);
}

static FX_BOOL _IsScanLine1bpp(uint8_t* pBuf, int width) {
  int size = width / 8;
  for (int i = 0; i < size; i++) {
    if (pBuf[i])
      return TRUE;
  }
  return (width % 8) && (pBuf[width / 8] & (0xff << (8 - width % 8)));
}

static FX_BOOL _IsScanLine8bpp(uint8_t* pBuf, int width) {
  for (int i = 0; i < width; i++) {
    if (pBuf[i] > 0x40)
      return TRUE;
  }
  return FALSE;
}

static int _DetectFirstLastScan(const CFX_DIBitmap* pBitmap, FX_BOOL bFirst) {
  int height = pBitmap->GetHeight(), pitch = pBitmap->GetPitch(),
      width = pBitmap->GetWidth();
  int bpp = pBitmap->GetBPP();
  if (bpp > 8) {
    width *= bpp / 8;
  }
  uint8_t* pBuf = pBitmap->GetBuffer();
  int line = bFirst ? 0 : height - 1;
  int line_step = bFirst ? 1 : -1;
  int line_end = bFirst ? height : -1;
  while (line != line_end) {
    if (bpp == 1) {
      if (_IsScanLine1bpp(pBuf + line * pitch, width)) {
        return line;
      }
    } else {
      if (_IsScanLine8bpp(pBuf + line * pitch, width)) {
        return line;
      }
    }
    line += line_step;
  }
  return -1;
}

CFX_GlyphBitmap* CPDF_Type3Cache::RenderGlyph(CPDF_Type3Glyphs* pSize,
                                              uint32_t charcode,
                                              const CFX_Matrix* pMatrix,
                                              FX_FLOAT retinaScaleX,
                                              FX_FLOAT retinaScaleY) {
  const CPDF_Type3Char* pChar = m_pFont->LoadChar(charcode);
  if (!pChar || !pChar->m_pBitmap)
    return nullptr;

  CFX_DIBitmap* pBitmap = pChar->m_pBitmap.get();
  CFX_Matrix image_matrix, text_matrix;
  image_matrix = pChar->m_ImageMatrix;
  text_matrix.Set(pMatrix->a, pMatrix->b, pMatrix->c, pMatrix->d, 0, 0);
  image_matrix.Concat(text_matrix);
  std::unique_ptr<CFX_DIBitmap> pResBitmap;
  int left = 0;
  int top = 0;
  if (FXSYS_fabs(image_matrix.b) < FXSYS_fabs(image_matrix.a) / 100 &&
      FXSYS_fabs(image_matrix.c) < FXSYS_fabs(image_matrix.d) / 100) {
    int top_line = _DetectFirstLastScan(pBitmap, TRUE);
    int bottom_line = _DetectFirstLastScan(pBitmap, FALSE);
    if (top_line == 0 && bottom_line == pBitmap->GetHeight() - 1) {
      FX_FLOAT top_y = image_matrix.d + image_matrix.f;
      FX_FLOAT bottom_y = image_matrix.f;
      FX_BOOL bFlipped = top_y > bottom_y;
      if (bFlipped) {
        FX_FLOAT temp = top_y;
        top_y = bottom_y;
        bottom_y = temp;
      }
      pSize->AdjustBlue(top_y, bottom_y, top_line, bottom_line);
      pResBitmap.reset(pBitmap->StretchTo(
          (int)(FXSYS_round(image_matrix.a) * retinaScaleX),
          (int)((bFlipped ? top_line - bottom_line : bottom_line - top_line) *
                retinaScaleY)));
      top = top_line;
      if (image_matrix.a < 0) {
        image_matrix.Scale(retinaScaleX, retinaScaleY);
        left = FXSYS_round(image_matrix.e + image_matrix.a);
      } else {
        left = FXSYS_round(image_matrix.e);
      }
    }
  }
  if (!pResBitmap) {
    image_matrix.Scale(retinaScaleX, retinaScaleY);
    pResBitmap.reset(pBitmap->TransformTo(&image_matrix, left, top));
  }
  if (!pResBitmap)
    return nullptr;

  CFX_GlyphBitmap* pGlyph = new CFX_GlyphBitmap;
  pGlyph->m_Left = left;
  pGlyph->m_Top = -top;
  pGlyph->m_Bitmap.TakeOver(pResBitmap.get());
  return pGlyph;
}

FX_BOOL CPDF_RenderStatus::ProcessText(const CPDF_TextObject* textobj,
                                       const CFX_Matrix* pObj2Device,
                                       CFX_PathData* pClippingPath) {
  if (textobj->m_nChars == 0)
    return TRUE;

  const TextRenderingMode& text_render_mode =
      textobj->m_TextState.GetObject()->m_TextMode;
  if (text_render_mode == TextRenderingMode::MODE_INVISIBLE)
    return TRUE;

  CPDF_Font* pFont = textobj->m_TextState.GetFont();
  if (pFont->IsType3Font())
    return ProcessType3Text(textobj, pObj2Device);

  bool bFill = false;
  bool bStroke = false;
  bool bClip = false;
  if (pClippingPath) {
    bClip = true;
  } else {
    switch (text_render_mode) {
      case TextRenderingMode::MODE_FILL:
      case TextRenderingMode::MODE_FILL_CLIP:
        bFill = true;
        break;
      case TextRenderingMode::MODE_STROKE:
      case TextRenderingMode::MODE_STROKE_CLIP:
        if (pFont->GetFace() ||
            (pFont->GetSubstFont()->m_SubstFlags & FXFONT_SUBST_GLYPHPATH)) {
          bStroke = true;
        } else {
          bFill = true;
        }
        break;
      case TextRenderingMode::MODE_FILL_STROKE:
      case TextRenderingMode::MODE_FILL_STROKE_CLIP:
        bFill = true;
        if (pFont->GetFace() ||
            (pFont->GetSubstFont()->m_SubstFlags & FXFONT_SUBST_GLYPHPATH)) {
          bStroke = true;
        }
        break;
      case TextRenderingMode::MODE_INVISIBLE:
        // Already handled above, but the compiler is not smart enough to
        // realize it. Fall through.
        ASSERT(false);
      case TextRenderingMode::MODE_CLIP:
        return TRUE;
    }
  }
  FX_ARGB stroke_argb = 0;
  FX_ARGB fill_argb = 0;
  bool bPattern = false;
  if (bStroke) {
    if (textobj->m_ColorState.GetStrokeColor()->IsPattern()) {
      bPattern = true;
    } else {
      stroke_argb = GetStrokeArgb(textobj);
    }
  }
  if (bFill) {
    if (textobj->m_ColorState.GetFillColor()->IsPattern()) {
      bPattern = true;
    } else {
      fill_argb = GetFillArgb(textobj);
    }
  }
  CFX_Matrix text_matrix;
  textobj->GetTextMatrix(&text_matrix);
  if (!IsAvailableMatrix(text_matrix))
    return TRUE;

  FX_FLOAT font_size = textobj->m_TextState.GetFontSize();
  if (bPattern) {
    DrawTextPathWithPattern(textobj, pObj2Device, pFont, font_size,
                            &text_matrix, bFill, bStroke);
    return TRUE;
  }
  if (bClip || bStroke) {
    const CFX_Matrix* pDeviceMatrix = pObj2Device;
    CFX_Matrix device_matrix;
    if (bStroke) {
      const FX_FLOAT* pCTM = textobj->m_TextState.GetObject()->m_CTM;
      if (pCTM[0] != 1.0f || pCTM[3] != 1.0f) {
        CFX_Matrix ctm(pCTM[0], pCTM[1], pCTM[2], pCTM[3], 0, 0);
        text_matrix.ConcatInverse(ctm);
        device_matrix.Copy(ctm);
        device_matrix.Concat(*pObj2Device);
        pDeviceMatrix = &device_matrix;
      }
    }
    int flag = 0;
    if (bStroke && bFill) {
      flag |= FX_FILL_STROKE;
      flag |= FX_STROKE_TEXT_MODE;
    }
    const CPDF_GeneralStateData* pGeneralData =
        static_cast<const CPDF_PageObject*>(textobj)
            ->m_GeneralState.GetObject();
    if (pGeneralData && pGeneralData->m_StrokeAdjust) {
      flag |= FX_STROKE_ADJUST;
    }
    if (m_Options.m_Flags & RENDER_NOTEXTSMOOTH) {
      flag |= FXFILL_NOPATHSMOOTH;
    }
    return CPDF_TextRenderer::DrawTextPath(
        m_pDevice, textobj->m_nChars, textobj->m_pCharCodes,
        textobj->m_pCharPos, pFont, font_size, &text_matrix, pDeviceMatrix,
        textobj->m_GraphState.GetObject(), fill_argb, stroke_argb,
        pClippingPath, flag);
  }
  text_matrix.Concat(*pObj2Device);
  return CPDF_TextRenderer::DrawNormalText(
      m_pDevice, textobj->m_nChars, textobj->m_pCharCodes, textobj->m_pCharPos,
      pFont, font_size, &text_matrix, fill_argb, &m_Options);
}

CPDF_Type3Cache* CPDF_RenderStatus::GetCachedType3(CPDF_Type3Font* pFont) {
  if (!pFont->m_pDocument) {
    return nullptr;
  }
  pFont->m_pDocument->GetPageData()->GetFont(pFont->GetFontDict(), FALSE);
  return pFont->m_pDocument->GetRenderData()->GetCachedType3(pFont);
}
static void ReleaseCachedType3(CPDF_Type3Font* pFont) {
  if (!pFont->m_pDocument) {
    return;
  }
  pFont->m_pDocument->GetRenderData()->ReleaseCachedType3(pFont);
  pFont->m_pDocument->GetPageData()->ReleaseFont(pFont->GetFontDict());
}

class CPDF_RefType3Cache {
 public:
  explicit CPDF_RefType3Cache(CPDF_Type3Font* pType3Font)
      : m_dwCount(0), m_pType3Font(pType3Font) {}
  ~CPDF_RefType3Cache() {
    while (m_dwCount--) {
      ReleaseCachedType3(m_pType3Font);
    }
  }
  uint32_t m_dwCount;
  CPDF_Type3Font* const m_pType3Font;
};

FX_BOOL CPDF_RenderStatus::ProcessType3Text(const CPDF_TextObject* textobj,
                                            const CFX_Matrix* pObj2Device) {
  CPDF_Type3Font* pType3Font = textobj->m_TextState.GetFont()->AsType3Font();
  for (int i = 0; i < m_Type3FontCache.GetSize(); ++i) {
    if (m_Type3FontCache.GetAt(i) == pType3Font)
      return TRUE;
  }

  CFX_Matrix dCTM = m_pDevice->GetCTM();
  FX_FLOAT sa = FXSYS_fabs(dCTM.a);
  FX_FLOAT sd = FXSYS_fabs(dCTM.d);
  CFX_Matrix text_matrix;
  textobj->GetTextMatrix(&text_matrix);
  CFX_Matrix char_matrix = pType3Font->GetFontMatrix();
  FX_FLOAT font_size = textobj->m_TextState.GetFontSize();
  char_matrix.Scale(font_size, font_size);
  FX_ARGB fill_argb = GetFillArgb(textobj, TRUE);
  int fill_alpha = FXARGB_A(fill_argb);
  int device_class = m_pDevice->GetDeviceClass();
  std::vector<FXTEXT_GLYPHPOS> glyphs;
  if (device_class == FXDC_DISPLAY)
    glyphs.resize(textobj->m_nChars);
  else if (fill_alpha < 255)
    return FALSE;

  CPDF_RefType3Cache refTypeCache(pType3Font);
  uint32_t* pChars = textobj->m_pCharCodes;
  if (textobj->m_nChars == 1)
    pChars = (uint32_t*)(&textobj->m_pCharCodes);

  for (int iChar = 0; iChar < textobj->m_nChars; iChar++) {
    uint32_t charcode = pChars[iChar];
    if (charcode == (uint32_t)-1)
      continue;

    CPDF_Type3Char* pType3Char = pType3Font->LoadChar(charcode);
    if (!pType3Char)
      continue;

    CFX_Matrix matrix = char_matrix;
    matrix.e += iChar ? textobj->m_pCharPos[iChar - 1] : 0;
    matrix.Concat(text_matrix);
    matrix.Concat(*pObj2Device);
    if (!pType3Char->LoadBitmap(m_pContext)) {
      if (!glyphs.empty()) {
        for (int i = 0; i < iChar; i++) {
          const FXTEXT_GLYPHPOS& glyph = glyphs[i];
          if (!glyph.m_pGlyph)
            continue;

          m_pDevice->SetBitMask(&glyph.m_pGlyph->m_Bitmap,
                                glyph.m_OriginX + glyph.m_pGlyph->m_Left,
                                glyph.m_OriginY - glyph.m_pGlyph->m_Top,
                                fill_argb);
        }
        glyphs.clear();
      }
      CPDF_GraphicStates* pStates = CloneObjStates(textobj, FALSE);
      CPDF_RenderOptions Options = m_Options;
      Options.m_Flags |= RENDER_FORCE_HALFTONE | RENDER_RECT_AA;
      Options.m_Flags &= ~RENDER_FORCE_DOWNSAMPLE;
      CPDF_Dictionary* pFormResource = nullptr;
      if (pType3Char->m_pForm && pType3Char->m_pForm->m_pFormDict) {
        pFormResource =
            pType3Char->m_pForm->m_pFormDict->GetDictBy("Resources");
      }
      if (fill_alpha == 255) {
        CPDF_RenderStatus status;
        status.Initialize(m_pContext, m_pDevice, nullptr, nullptr, this,
                          pStates, &Options,
                          pType3Char->m_pForm->m_Transparency, m_bDropObjects,
                          pFormResource, FALSE, pType3Char, fill_argb);
        status.m_Type3FontCache.Append(m_Type3FontCache);
        status.m_Type3FontCache.Add(pType3Font);
        m_pDevice->SaveState();
        status.RenderObjectList(pType3Char->m_pForm.get(), &matrix);
        m_pDevice->RestoreState(false);
      } else {
        CFX_FloatRect rect_f = pType3Char->m_pForm->CalcBoundingBox();
        rect_f.Transform(&matrix);
        FX_RECT rect = rect_f.GetOutterRect();
        CFX_FxgeDevice bitmap_device;
        if (!bitmap_device.Create((int)(rect.Width() * sa),
                                  (int)(rect.Height() * sd), FXDIB_Argb,
                                  nullptr)) {
          return TRUE;
        }
        bitmap_device.GetBitmap()->Clear(0);
        CPDF_RenderStatus status;
        status.Initialize(m_pContext, &bitmap_device, nullptr, nullptr, this,
                          pStates, &Options,
                          pType3Char->m_pForm->m_Transparency, m_bDropObjects,
                          pFormResource, FALSE, pType3Char, fill_argb);
        status.m_Type3FontCache.Append(m_Type3FontCache);
        status.m_Type3FontCache.Add(pType3Font);
        matrix.TranslateI(-rect.left, -rect.top);
        matrix.Scale(sa, sd);
        status.RenderObjectList(pType3Char->m_pForm.get(), &matrix);
        m_pDevice->SetDIBits(bitmap_device.GetBitmap(), rect.left, rect.top);
      }
      delete pStates;
    } else if (pType3Char->m_pBitmap) {
      if (device_class == FXDC_DISPLAY) {
        CPDF_Type3Cache* pCache = GetCachedType3(pType3Font);
        refTypeCache.m_dwCount++;
        CFX_GlyphBitmap* pBitmap = pCache->LoadGlyph(charcode, &matrix, sa, sd);
        if (!pBitmap)
          continue;

        int origin_x = FXSYS_round(matrix.e);
        int origin_y = FXSYS_round(matrix.f);
        if (glyphs.empty()) {
          m_pDevice->SetBitMask(&pBitmap->m_Bitmap, origin_x + pBitmap->m_Left,
                                origin_y - pBitmap->m_Top, fill_argb);
        } else {
          glyphs[iChar].m_pGlyph = pBitmap;
          glyphs[iChar].m_OriginX = origin_x;
          glyphs[iChar].m_OriginY = origin_y;
        }
      } else {
        CFX_Matrix image_matrix = pType3Char->m_ImageMatrix;
        image_matrix.Concat(matrix);
        CPDF_ImageRenderer renderer;
        if (renderer.Start(this, pType3Char->m_pBitmap.get(), fill_argb, 255,
                           &image_matrix, 0, FALSE)) {
          renderer.Continue(nullptr);
        }
        if (!renderer.m_Result)
          return FALSE;
      }
    }
  }

  if (glyphs.empty())
    return TRUE;

  FX_RECT rect = FXGE_GetGlyphsBBox(glyphs, 0, sa, sd);
  CFX_DIBitmap bitmap;
  if (!bitmap.Create(static_cast<int>(rect.Width() * sa),
                     static_cast<int>(rect.Height() * sd), FXDIB_8bppMask)) {
    return TRUE;
  }
  bitmap.Clear(0);
  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    if (!glyph.m_pGlyph)
      continue;

    bitmap.TransferBitmap(
        static_cast<int>(
            (glyph.m_OriginX + glyph.m_pGlyph->m_Left - rect.left) * sa),
        static_cast<int>((glyph.m_OriginY - glyph.m_pGlyph->m_Top - rect.top) *
                         sd),
        glyph.m_pGlyph->m_Bitmap.GetWidth(),
        glyph.m_pGlyph->m_Bitmap.GetHeight(), &glyph.m_pGlyph->m_Bitmap, 0, 0);
  }
  m_pDevice->SetBitMask(&bitmap, rect.left, rect.top, fill_argb);
  return TRUE;
}

class CPDF_CharPosList {
 public:
  CPDF_CharPosList();
  ~CPDF_CharPosList();
  void Load(int nChars,
            uint32_t* pCharCodes,
            FX_FLOAT* pCharPos,
            CPDF_Font* pFont,
            FX_FLOAT font_size);
  FXTEXT_CHARPOS* m_pCharPos;
  uint32_t m_nChars;
};

CPDF_CharPosList::CPDF_CharPosList() {
  m_pCharPos = nullptr;
}

CPDF_CharPosList::~CPDF_CharPosList() {
  FX_Free(m_pCharPos);
}

void CPDF_CharPosList::Load(int nChars,
                            uint32_t* pCharCodes,
                            FX_FLOAT* pCharPos,
                            CPDF_Font* pFont,
                            FX_FLOAT FontSize) {
  m_pCharPos = FX_Alloc(FXTEXT_CHARPOS, nChars);
  m_nChars = 0;
  CPDF_CIDFont* pCIDFont = pFont->AsCIDFont();
  FX_BOOL bVertWriting = pCIDFont && pCIDFont->IsVertWriting();
  for (int iChar = 0; iChar < nChars; iChar++) {
    uint32_t CharCode =
        nChars == 1 ? (uint32_t)(uintptr_t)pCharCodes : pCharCodes[iChar];
    if (CharCode == (uint32_t)-1) {
      continue;
    }
    bool bVert = false;
    FXTEXT_CHARPOS& charpos = m_pCharPos[m_nChars++];
    if (pCIDFont) {
      charpos.m_bFontStyle = true;
    }
    charpos.m_GlyphIndex = pFont->GlyphFromCharCode(CharCode, &bVert);
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
    charpos.m_ExtGID = pFont->GlyphFromCharCodeExt(CharCode);
#endif
    if (!pFont->IsEmbedded() && !pFont->IsCIDFont()) {
      charpos.m_FontCharWidth = pFont->GetCharWidthF(CharCode);
    } else {
      charpos.m_FontCharWidth = 0;
    }
    charpos.m_OriginX = iChar ? pCharPos[iChar - 1] : 0;
    charpos.m_OriginY = 0;
    charpos.m_bGlyphAdjust = FALSE;
    if (!pCIDFont) {
      continue;
    }
    uint16_t CID = pCIDFont->CIDFromCharCode(CharCode);
    if (bVertWriting) {
      charpos.m_OriginY = charpos.m_OriginX;
      charpos.m_OriginX = 0;
      short vx, vy;
      pCIDFont->GetVertOrigin(CID, vx, vy);
      charpos.m_OriginX -= FontSize * vx / 1000;
      charpos.m_OriginY -= FontSize * vy / 1000;
    }
    const uint8_t* pTransform = pCIDFont->GetCIDTransform(CID);
    if (pTransform && !bVert) {
      charpos.m_AdjustMatrix[0] = pCIDFont->CIDTransformToFloat(pTransform[0]);
      charpos.m_AdjustMatrix[2] = pCIDFont->CIDTransformToFloat(pTransform[2]);
      charpos.m_AdjustMatrix[1] = pCIDFont->CIDTransformToFloat(pTransform[1]);
      charpos.m_AdjustMatrix[3] = pCIDFont->CIDTransformToFloat(pTransform[3]);
      charpos.m_OriginX +=
          pCIDFont->CIDTransformToFloat(pTransform[4]) * FontSize;
      charpos.m_OriginY +=
          pCIDFont->CIDTransformToFloat(pTransform[5]) * FontSize;
      charpos.m_bGlyphAdjust = TRUE;
    }
  }
}

// static
FX_BOOL CPDF_TextRenderer::DrawTextPath(CFX_RenderDevice* pDevice,
                                        int nChars,
                                        uint32_t* pCharCodes,
                                        FX_FLOAT* pCharPos,
                                        CPDF_Font* pFont,
                                        FX_FLOAT font_size,
                                        const CFX_Matrix* pText2User,
                                        const CFX_Matrix* pUser2Device,
                                        const CFX_GraphStateData* pGraphState,
                                        FX_ARGB fill_argb,
                                        FX_ARGB stroke_argb,
                                        CFX_PathData* pClippingPath,
                                        int nFlag) {
  CFX_FontCache* pCache =
      pFont->m_pDocument ? pFont->m_pDocument->GetRenderData()->GetFontCache()
                         : nullptr;
  CPDF_CharPosList CharPosList;
  CharPosList.Load(nChars, pCharCodes, pCharPos, pFont, font_size);
  return pDevice->DrawTextPathWithFlags(
      CharPosList.m_nChars, CharPosList.m_pCharPos, &pFont->m_Font, pCache,
      font_size, pText2User, pUser2Device, pGraphState, fill_argb, stroke_argb,
      pClippingPath, nFlag);
}

// static
void CPDF_TextRenderer::DrawTextString(CFX_RenderDevice* pDevice,
                                       FX_FLOAT origin_x,
                                       FX_FLOAT origin_y,
                                       CPDF_Font* pFont,
                                       FX_FLOAT font_size,
                                       const CFX_Matrix* pMatrix,
                                       const CFX_ByteString& str,
                                       FX_ARGB fill_argb,
                                       FX_ARGB stroke_argb,
                                       const CFX_GraphStateData* pGraphState,
                                       const CPDF_RenderOptions* pOptions) {
  if (pFont->IsType3Font())
    return;

  int nChars = pFont->CountChar(str.c_str(), str.GetLength());
  if (nChars <= 0)
    return;

  int offset = 0;
  uint32_t* pCharCodes;
  FX_FLOAT* pCharPos;
  std::vector<uint32_t> codes;
  std::vector<FX_FLOAT> positions;
  if (nChars == 1) {
    pCharCodes = reinterpret_cast<uint32_t*>(
        pFont->GetNextChar(str.c_str(), str.GetLength(), offset));
    pCharPos = nullptr;
  } else {
    codes.resize(nChars);
    positions.resize(nChars - 1);
    FX_FLOAT cur_pos = 0;
    for (int i = 0; i < nChars; i++) {
      codes[i] = pFont->GetNextChar(str.c_str(), str.GetLength(), offset);
      if (i)
        positions[i - 1] = cur_pos;
      cur_pos += pFont->GetCharWidthF(codes[i]) * font_size / 1000;
    }
    pCharCodes = codes.data();
    pCharPos = positions.data();
  }
  CFX_Matrix matrix;
  if (pMatrix)
    matrix = *pMatrix;

  matrix.e = origin_x;
  matrix.f = origin_y;

  if (stroke_argb == 0) {
    DrawNormalText(pDevice, nChars, pCharCodes, pCharPos, pFont, font_size,
                   &matrix, fill_argb, pOptions);
  } else {
    DrawTextPath(pDevice, nChars, pCharCodes, pCharPos, pFont, font_size,
                 &matrix, nullptr, pGraphState, fill_argb, stroke_argb, nullptr,
                 0);
  }
}

// static
FX_BOOL CPDF_TextRenderer::DrawNormalText(CFX_RenderDevice* pDevice,
                                          int nChars,
                                          uint32_t* pCharCodes,
                                          FX_FLOAT* pCharPos,
                                          CPDF_Font* pFont,
                                          FX_FLOAT font_size,
                                          const CFX_Matrix* pText2Device,
                                          FX_ARGB fill_argb,
                                          const CPDF_RenderOptions* pOptions) {
  CFX_FontCache* pCache =
      pFont->m_pDocument ? pFont->m_pDocument->GetRenderData()->GetFontCache()
                         : nullptr;
  CPDF_CharPosList CharPosList;
  CharPosList.Load(nChars, pCharCodes, pCharPos, pFont, font_size);
  int FXGE_flags = 0;
  if (pOptions) {
    uint32_t dwFlags = pOptions->m_Flags;
    if (dwFlags & RENDER_CLEARTYPE) {
      FXGE_flags |= FXTEXT_CLEARTYPE;
      if (dwFlags & RENDER_BGR_STRIPE) {
        FXGE_flags |= FXTEXT_BGR_STRIPE;
      }
    }
    if (dwFlags & RENDER_NOTEXTSMOOTH) {
      FXGE_flags |= FXTEXT_NOSMOOTH;
    }
    if (dwFlags & RENDER_PRINTGRAPHICTEXT) {
      FXGE_flags |= FXTEXT_PRINTGRAPHICTEXT;
    }
    if (dwFlags & RENDER_NO_NATIVETEXT) {
      FXGE_flags |= FXTEXT_NO_NATIVETEXT;
    }
    if (dwFlags & RENDER_PRINTIMAGETEXT) {
      FXGE_flags |= FXTEXT_PRINTIMAGETEXT;
    }
  } else {
    FXGE_flags = FXTEXT_CLEARTYPE;
  }
  if (pFont->IsCIDFont()) {
    FXGE_flags |= FXFONT_CIDFONT;
  }
  return pDevice->DrawNormalText(CharPosList.m_nChars, CharPosList.m_pCharPos,
                                 &pFont->m_Font, pCache, font_size,
                                 pText2Device, fill_argb, FXGE_flags);
}

void CPDF_RenderStatus::DrawTextPathWithPattern(const CPDF_TextObject* textobj,
                                                const CFX_Matrix* pObj2Device,
                                                CPDF_Font* pFont,
                                                FX_FLOAT font_size,
                                                const CFX_Matrix* pTextMatrix,
                                                FX_BOOL bFill,
                                                FX_BOOL bStroke) {
  if (!bStroke) {
    CPDF_PathObject path;
    std::vector<std::unique_ptr<CPDF_TextObject>> pCopy;
    pCopy.push_back(std::unique_ptr<CPDF_TextObject>(textobj->Clone()));
    path.m_bStroke = FALSE;
    path.m_FillType = FXFILL_WINDING;
    path.m_ClipPath.AppendTexts(&pCopy);
    path.m_ColorState = textobj->m_ColorState;
    path.m_Path.New()->AppendRect(textobj->m_Left, textobj->m_Bottom,
                                  textobj->m_Right, textobj->m_Top);
    path.m_Left = textobj->m_Left;
    path.m_Bottom = textobj->m_Bottom;
    path.m_Right = textobj->m_Right;
    path.m_Top = textobj->m_Top;
    RenderSingleObject(&path, pObj2Device);
    return;
  }
  CFX_FontCache* pCache;
  if (pFont->m_pDocument) {
    pCache = pFont->m_pDocument->GetRenderData()->GetFontCache();
  } else {
    pCache = CFX_GEModule::Get()->GetFontCache();
  }
  CFX_FaceCache* pFaceCache = pCache->GetCachedFace(&pFont->m_Font);
  FX_FONTCACHE_DEFINE(pCache, &pFont->m_Font);
  CPDF_CharPosList CharPosList;
  CharPosList.Load(textobj->m_nChars, textobj->m_pCharCodes,
                   textobj->m_pCharPos, pFont, font_size);
  for (uint32_t i = 0; i < CharPosList.m_nChars; i++) {
    FXTEXT_CHARPOS& charpos = CharPosList.m_pCharPos[i];
    const CFX_PathData* pPath = pFaceCache->LoadGlyphPath(
        &pFont->m_Font, charpos.m_GlyphIndex, charpos.m_FontCharWidth);
    if (!pPath) {
      continue;
    }
    CPDF_PathObject path;
    path.m_GraphState = textobj->m_GraphState;
    path.m_ColorState = textobj->m_ColorState;
    CFX_Matrix matrix;
    if (charpos.m_bGlyphAdjust)
      matrix.Set(charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
                 charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3], 0, 0);
    matrix.Concat(font_size, 0, 0, font_size, charpos.m_OriginX,
                  charpos.m_OriginY);
    path.m_Path.New()->Append(pPath, &matrix);
    path.m_Matrix = *pTextMatrix;
    path.m_bStroke = bStroke;
    path.m_FillType = bFill ? FXFILL_WINDING : 0;
    path.CalcBoundingBox();
    ProcessPath(&path, pObj2Device);
  }
}
