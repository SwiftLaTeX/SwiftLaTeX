// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <limits>
#include <vector>

#include "core/fxcodec/include/fx_codec.h"
#include "core/fxge/ge/fx_text_int.h"
#include "core/fxge/include/fx_freetype.h"
#include "core/fxge/include/fx_ge.h"
#include "core/fxge/include/ifx_renderdevicedriver.h"

#ifdef _SKIA_SUPPORT_
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"
#endif

namespace {

void ResetTransform(FT_Face face) {
  FXFT_Matrix matrix;
  matrix.xx = 0x10000L;
  matrix.xy = 0;
  matrix.yx = 0;
  matrix.yy = 0x10000L;
  FXFT_Set_Transform(face, &matrix, 0);
}

// Sets the given transform on the font, and resets it to the identity when it
// goes out of scope.
class ScopedFontTransform {
 public:
  ScopedFontTransform(FT_Face face, FXFT_Matrix* matrix) : m_Face(face) {
    FXFT_Set_Transform(m_Face, matrix, 0);
  }

  ~ScopedFontTransform() { ResetTransform(m_Face); }

 private:
  FT_Face m_Face;
};

void AdjustGlyphSpace(std::vector<FXTEXT_GLYPHPOS>* pGlyphAndPos) {
  ASSERT(pGlyphAndPos->size() > 1);
  std::vector<FXTEXT_GLYPHPOS>& glyphs = *pGlyphAndPos;
  bool bVertical = glyphs.back().m_OriginX == glyphs.front().m_OriginX;
  if (!bVertical && (glyphs.back().m_OriginY != glyphs.front().m_OriginY))
    return;

  for (size_t i = glyphs.size() - 1; i > 1; --i) {
    FXTEXT_GLYPHPOS& next = glyphs[i];
    int next_origin = bVertical ? next.m_OriginY : next.m_OriginX;
    FX_FLOAT next_origin_f = bVertical ? next.m_fOriginY : next.m_fOriginX;

    FXTEXT_GLYPHPOS& current = glyphs[i - 1];
    int& current_origin = bVertical ? current.m_OriginY : current.m_OriginX;
    FX_FLOAT current_origin_f =
        bVertical ? current.m_fOriginY : current.m_fOriginX;

    int space = next_origin - current_origin;
    FX_FLOAT space_f = next_origin_f - current_origin_f;
    FX_FLOAT error =
        FXSYS_fabs(space_f) - FXSYS_fabs(static_cast<FX_FLOAT>(space));
    if (error > 0.5f)
      current_origin += space > 0 ? -1 : 1;
  }
}

const uint8_t g_TextGammaAdjust[256] = {
    0,   2,   3,   4,   6,   7,   8,   10,  11,  12,  13,  15,  16,  17,  18,
    19,  21,  22,  23,  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,
    36,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  51,  52,
    53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
    68,  69,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
    84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,
    99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
    114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
    129, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
    143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 156,
    157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
    172, 173, 174, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
    186, 187, 188, 189, 190, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213,
    214, 215, 216, 217, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
    228, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 250, 251, 252, 253, 254,
    255,
};

int TextGammaAdjust(int value) {
  ASSERT(value >= 0);
  ASSERT(value <= 255);
  return g_TextGammaAdjust[value];
}

int CalcAlpha(int src, int alpha) {
  return src * alpha / 255;
}

void Merge(uint8_t src, int channel, int alpha, uint8_t* dest) {
  *dest = FXDIB_ALPHA_MERGE(*dest, channel, CalcAlpha(src, alpha));
}

void MergeGammaAdjust(uint8_t src, int channel, int alpha, uint8_t* dest) {
  *dest =
      FXDIB_ALPHA_MERGE(*dest, channel, CalcAlpha(TextGammaAdjust(src), alpha));
}

void MergeGammaAdjustBgr(const uint8_t* src,
                         int r,
                         int g,
                         int b,
                         int a,
                         uint8_t* dest) {
  MergeGammaAdjust(src[0], b, a, &dest[0]);
  MergeGammaAdjust(src[1], g, a, &dest[1]);
  MergeGammaAdjust(src[2], r, a, &dest[2]);
}

void MergeGammaAdjustRgb(const uint8_t* src,
                         int r,
                         int g,
                         int b,
                         int a,
                         uint8_t* dest) {
  MergeGammaAdjust(src[2], b, a, &dest[0]);
  MergeGammaAdjust(src[1], g, a, &dest[1]);
  MergeGammaAdjust(src[0], r, a, &dest[2]);
}

int AverageRgb(const uint8_t* src) {
  return (src[0] + src[1] + src[2]) / 3;
}

uint8_t CalculateDestAlpha(uint8_t back_alpha, int src_alpha) {
  return back_alpha + src_alpha - back_alpha * src_alpha / 255;
}

void ApplyDestAlpha(uint8_t back_alpha,
                    int src_alpha,
                    int r,
                    int g,
                    int b,
                    uint8_t* dest) {
  uint8_t dest_alpha = CalculateDestAlpha(back_alpha, src_alpha);
  int alpha_ratio = src_alpha * 255 / dest_alpha;
  dest[0] = FXDIB_ALPHA_MERGE(dest[0], b, alpha_ratio);
  dest[1] = FXDIB_ALPHA_MERGE(dest[1], g, alpha_ratio);
  dest[2] = FXDIB_ALPHA_MERGE(dest[2], r, alpha_ratio);
  dest[3] = dest_alpha;
}

void NormalizeRgbDst(int src_value, int r, int g, int b, int a, uint8_t* dest) {
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  dest[0] = FXDIB_ALPHA_MERGE(dest[0], b, src_alpha);
  dest[1] = FXDIB_ALPHA_MERGE(dest[1], g, src_alpha);
  dest[2] = FXDIB_ALPHA_MERGE(dest[2], r, src_alpha);
}

void NormalizeRgbSrc(int src_value, int r, int g, int b, int a, uint8_t* dest) {
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  if (src_alpha == 0)
    return;

  dest[0] = FXDIB_ALPHA_MERGE(dest[0], b, src_alpha);
  dest[1] = FXDIB_ALPHA_MERGE(dest[1], g, src_alpha);
  dest[2] = FXDIB_ALPHA_MERGE(dest[2], r, src_alpha);
}

void NormalizeArgbDest(int src_value,
                       int r,
                       int g,
                       int b,
                       int a,
                       uint8_t* dest) {
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  uint8_t back_alpha = dest[3];
  if (back_alpha == 0) {
    FXARGB_SETDIB(dest, FXARGB_MAKE(src_alpha, r, g, b));
  } else if (src_alpha != 0) {
    ApplyDestAlpha(back_alpha, src_alpha, r, g, b, dest);
  }
}

void NormalizeArgbSrc(int src_value,
                      int r,
                      int g,
                      int b,
                      int a,
                      uint8_t* dest) {
  int src_alpha = CalcAlpha(TextGammaAdjust(src_value), a);
  if (src_alpha == 0)
    return;

  uint8_t back_alpha = dest[3];
  if (back_alpha == 0) {
    FXARGB_SETDIB(dest, FXARGB_MAKE(src_alpha, r, g, b));
  } else {
    ApplyDestAlpha(back_alpha, src_alpha, r, g, b, dest);
  }
}

void NextPixel(uint8_t** src_scan, uint8_t** dst_scan, int bpp) {
  *src_scan += 3;
  *dst_scan += bpp;
}

void SetAlpha(uint8_t* alpha) {
  alpha[3] = 255;
}

void SetAlphaDoNothing(uint8_t* alpha) {}

void DrawNormalTextHelper(CFX_DIBitmap* bitmap,
                          const CFX_DIBitmap* pGlyph,
                          int nrows,
                          int left,
                          int top,
                          int start_col,
                          int end_col,
                          bool bNormal,
                          bool bBGRStripe,
                          int x_subpixel,
                          int a,
                          int r,
                          int g,
                          int b) {
  const bool has_alpha = bitmap->GetFormat() == FXDIB_Argb;
  uint8_t* src_buf = pGlyph->GetBuffer();
  int src_pitch = pGlyph->GetPitch();
  uint8_t* dest_buf = bitmap->GetBuffer();
  int dest_pitch = bitmap->GetPitch();
  const int Bpp = has_alpha ? 4 : bitmap->GetBPP() / 8;
  auto* pNormalizeSrcFunc = has_alpha ? &NormalizeArgbSrc : &NormalizeRgbDst;
  auto* pNormalizeDstFunc = has_alpha ? &NormalizeArgbDest : &NormalizeRgbSrc;
  auto* pSetAlpha = has_alpha ? &SetAlpha : &SetAlphaDoNothing;

  for (int row = 0; row < nrows; row++) {
    int dest_row = row + top;
    if (dest_row < 0 || dest_row >= bitmap->GetHeight())
      continue;

    uint8_t* src_scan = src_buf + row * src_pitch + (start_col - left) * 3;
    uint8_t* dest_scan = dest_buf + dest_row * dest_pitch + start_col * Bpp;
    if (bBGRStripe) {
      if (x_subpixel == 0) {
        for (int col = start_col; col < end_col; col++) {
          if (has_alpha) {
            Merge(src_scan[2], r, a, &dest_scan[2]);
            Merge(src_scan[1], g, a, &dest_scan[1]);
            Merge(src_scan[0], b, a, &dest_scan[0]);
          } else {
            MergeGammaAdjustBgr(&src_scan[0], r, g, b, a, &dest_scan[0]);
          }
          pSetAlpha(dest_scan);
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      } else if (x_subpixel == 1) {
        MergeGammaAdjust(src_scan[1], r, a, &dest_scan[2]);
        MergeGammaAdjust(src_scan[0], g, a, &dest_scan[1]);
        if (start_col > left)
          MergeGammaAdjust(src_scan[-1], b, a, &dest_scan[0]);
        pSetAlpha(dest_scan);
        NextPixel(&src_scan, &dest_scan, Bpp);
        for (int col = start_col + 1; col < end_col - 1; col++) {
          MergeGammaAdjustBgr(&src_scan[-1], r, g, b, a, &dest_scan[0]);
          pSetAlpha(dest_scan);
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      } else {
        MergeGammaAdjust(src_scan[0], r, a, &dest_scan[2]);
        if (start_col > left) {
          MergeGammaAdjust(src_scan[-1], g, a, &dest_scan[1]);
          MergeGammaAdjust(src_scan[-2], b, a, &dest_scan[0]);
        }
        pSetAlpha(dest_scan);
        NextPixel(&src_scan, &dest_scan, Bpp);
        for (int col = start_col + 1; col < end_col - 1; col++) {
          MergeGammaAdjustBgr(&src_scan[-2], r, g, b, a, &dest_scan[0]);
          pSetAlpha(dest_scan);
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      }
    } else {
      if (x_subpixel == 0) {
        for (int col = start_col; col < end_col; col++) {
          if (bNormal) {
            int src_value = AverageRgb(&src_scan[0]);
            pNormalizeDstFunc(src_value, r, g, b, a, dest_scan);
          } else {
            MergeGammaAdjustRgb(&src_scan[0], r, g, b, a, &dest_scan[0]);
            pSetAlpha(dest_scan);
          }
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      } else if (x_subpixel == 1) {
        if (bNormal) {
          int src_value = start_col > left ? AverageRgb(&src_scan[-1])
                                           : (src_scan[0] + src_scan[1]) / 3;
          pNormalizeSrcFunc(src_value, r, g, b, a, dest_scan);
        } else {
          if (start_col > left)
            MergeGammaAdjust(src_scan[-1], r, a, &dest_scan[2]);
          MergeGammaAdjust(src_scan[0], g, a, &dest_scan[1]);
          MergeGammaAdjust(src_scan[1], b, a, &dest_scan[0]);
          pSetAlpha(dest_scan);
        }
        NextPixel(&src_scan, &dest_scan, Bpp);
        for (int col = start_col + 1; col < end_col; col++) {
          if (bNormal) {
            int src_value = AverageRgb(&src_scan[-1]);
            pNormalizeDstFunc(src_value, r, g, b, a, dest_scan);
          } else {
            MergeGammaAdjustRgb(&src_scan[-1], r, g, b, a, &dest_scan[0]);
            pSetAlpha(dest_scan);
          }
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      } else {
        if (bNormal) {
          int src_value =
              start_col > left ? AverageRgb(&src_scan[-2]) : src_scan[0] / 3;
          pNormalizeSrcFunc(src_value, r, g, b, a, dest_scan);
        } else {
          if (start_col > left) {
            MergeGammaAdjust(src_scan[-2], r, a, &dest_scan[2]);
            MergeGammaAdjust(src_scan[-1], g, a, &dest_scan[1]);
          }
          MergeGammaAdjust(src_scan[0], b, a, &dest_scan[0]);
          pSetAlpha(dest_scan);
        }
        NextPixel(&src_scan, &dest_scan, Bpp);
        for (int col = start_col + 1; col < end_col; col++) {
          if (bNormal) {
            int src_value = AverageRgb(&src_scan[-2]);
            pNormalizeDstFunc(src_value, r, g, b, a, dest_scan);
          } else {
            MergeGammaAdjustRgb(&src_scan[-2], r, g, b, a, &dest_scan[0]);
            pSetAlpha(dest_scan);
          }
          NextPixel(&src_scan, &dest_scan, Bpp);
        }
      }
    }
  }
}

bool ShouldDrawDeviceText(const CFX_Font* pFont, uint32_t text_flags) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  if (text_flags & FXFONT_CIDFONT)
    return false;

  const CFX_ByteString bsPsName = pFont->GetPsName();
  if (bsPsName.Find("+ZJHL") != -1)
    return false;

  if (bsPsName == "CNAAJI+cmex10")
    return false;
#endif
  return true;
}

}  // namespace

FX_RECT FXGE_GetGlyphsBBox(const std::vector<FXTEXT_GLYPHPOS>& glyphs,
                           int anti_alias,
                           FX_FLOAT retinaScaleX,
                           FX_FLOAT retinaScaleY) {
  FX_RECT rect(0, 0, 0, 0);
  bool bStarted = false;
  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    const CFX_GlyphBitmap* pGlyph = glyph.m_pGlyph;
    if (!pGlyph)
      continue;

    int char_left = glyph.m_OriginX + pGlyph->m_Left;
    int char_width = (int)(pGlyph->m_Bitmap.GetWidth() / retinaScaleX);
    if (anti_alias == FXFT_RENDER_MODE_LCD) {
      char_width /= 3;
    }
    int char_right = char_left + char_width;
    int char_top = glyph.m_OriginY - pGlyph->m_Top;
    int char_bottom =
        char_top + (int)(pGlyph->m_Bitmap.GetHeight() / retinaScaleY);
    if (!bStarted) {
      rect.left = char_left;
      rect.right = char_right;
      rect.top = char_top;
      rect.bottom = char_bottom;
      bStarted = true;
    } else {
      rect.left = std::min(rect.left, char_left);
      rect.right = std::max(rect.right, char_right);
      rect.top = std::min(rect.top, char_top);
      rect.bottom = std::max(rect.bottom, char_bottom);
    }
  }
  return rect;
}

FX_BOOL CFX_RenderDevice::DrawNormalText(int nChars,
                                         const FXTEXT_CHARPOS* pCharPos,
                                         CFX_Font* pFont,
                                         CFX_FontCache* pCache,
                                         FX_FLOAT font_size,
                                         const CFX_Matrix* pText2Device,
                                         uint32_t fill_color,
                                         uint32_t text_flags) {
  int nativetext_flags = text_flags;
  if (m_DeviceClass != FXDC_DISPLAY) {
    if (!(text_flags & FXTEXT_PRINTGRAPHICTEXT)) {
      if (ShouldDrawDeviceText(pFont, text_flags) &&
          m_pDeviceDriver->DrawDeviceText(nChars, pCharPos, pFont, pCache,
                                          pText2Device, font_size,
                                          fill_color)) {
        return TRUE;
      }
    }
    if (FXARGB_A(fill_color) < 255)
      return FALSE;
  } else if (!(text_flags & FXTEXT_NO_NATIVETEXT)) {
    if (ShouldDrawDeviceText(pFont, text_flags) &&
        m_pDeviceDriver->DrawDeviceText(nChars, pCharPos, pFont, pCache,
                                        pText2Device, font_size, fill_color)) {
      return TRUE;
    }
  }
  CFX_Matrix char2device;
  CFX_Matrix text2Device;
  if (pText2Device) {
    char2device = *pText2Device;
    text2Device = *pText2Device;
  }
  char2device.Scale(font_size, -font_size);
  if (FXSYS_fabs(char2device.a) + FXSYS_fabs(char2device.b) > 50 * 1.0f ||
      ((m_DeviceClass == FXDC_PRINTER) &&
       !(text_flags & FXTEXT_PRINTIMAGETEXT))) {
    if (pFont->GetFace() ||
        (pFont->GetSubstFont()->m_SubstFlags & FXFONT_SUBST_GLYPHPATH)) {
      int nPathFlags =
          (text_flags & FXTEXT_NOSMOOTH) == 0 ? 0 : FXFILL_NOPATHSMOOTH;
      return DrawTextPathWithFlags(nChars, pCharPos, pFont, pCache, font_size,
                                   pText2Device, nullptr, nullptr, fill_color,
                                   0, nullptr, nPathFlags);
    }
  }
  int anti_alias = FXFT_RENDER_MODE_MONO;
  bool bNormal = false;
  if ((text_flags & FXTEXT_NOSMOOTH) == 0) {
    if (m_DeviceClass == FXDC_DISPLAY && m_bpp > 1) {
      if (!CFX_GEModule::Get()->GetFontMgr()->FTLibrarySupportsHinting()) {
        // Some Freetype implementations (like the one packaged with Fedora) do
        // not support hinting due to patents 6219025, 6239783, 6307566,
        // 6225973, 6243070, 6393145, 6421054, 6282327, and 6624828; the latest
        // one expires 10/7/19.  This makes LCD antialiasing very ugly, so we
        // instead fall back on NORMAL antialiasing.
        anti_alias = FXFT_RENDER_MODE_NORMAL;
      } else if ((m_RenderCaps & (FXRC_ALPHA_OUTPUT | FXRC_CMYK_OUTPUT))) {
        anti_alias = FXFT_RENDER_MODE_LCD;
        bNormal = true;
      } else if (m_bpp < 16) {
        anti_alias = FXFT_RENDER_MODE_NORMAL;
      } else {
        anti_alias = FXFT_RENDER_MODE_LCD;

        bool bClearType = false;
        if (pFont->GetFace() ||
            (pFont->GetSubstFont()->m_SubstFlags & FXFONT_SUBST_CLEARTYPE)) {
          bClearType = !!(text_flags & FXTEXT_CLEARTYPE);
        }
        bNormal = !bClearType;
      }
    }
  }
  if (!pCache)
    pCache = CFX_GEModule::Get()->GetFontCache();

  CFX_FaceCache* pFaceCache = pCache->GetCachedFace(pFont);
  FX_FONTCACHE_DEFINE(pCache, pFont);
  std::vector<FXTEXT_GLYPHPOS> glyphs(nChars);
  CFX_Matrix matrixCTM = GetCTM();
  FX_FLOAT scale_x = FXSYS_fabs(matrixCTM.a);
  FX_FLOAT scale_y = FXSYS_fabs(matrixCTM.d);
  CFX_Matrix deviceCtm = char2device;
  deviceCtm.Concat(scale_x, 0, 0, scale_y, 0, 0);
  text2Device.Concat(scale_x, 0, 0, scale_y, 0, 0);
  for (size_t i = 0; i < glyphs.size(); ++i) {
    FXTEXT_GLYPHPOS& glyph = glyphs[i];
    const FXTEXT_CHARPOS& charpos = pCharPos[i];
    glyph.m_fOriginX = charpos.m_OriginX;
    glyph.m_fOriginY = charpos.m_OriginY;
    text2Device.Transform(glyph.m_fOriginX, glyph.m_fOriginY);
    if (anti_alias < FXFT_RENDER_MODE_LCD) {
      glyph.m_OriginX = FXSYS_round(glyph.m_fOriginX);
    } else {
      glyph.m_OriginX = (int)FXSYS_floor(glyph.m_fOriginX);
    }
    glyph.m_OriginY = FXSYS_round(glyph.m_fOriginY);
    if (charpos.m_bGlyphAdjust) {
      CFX_Matrix new_matrix(
          charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
          charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3], 0, 0);
      new_matrix.Concat(deviceCtm);
      glyph.m_pGlyph = pFaceCache->LoadGlyphBitmap(
          pFont, charpos.m_GlyphIndex, charpos.m_bFontStyle, &new_matrix,
          charpos.m_FontCharWidth, anti_alias, nativetext_flags);
    } else {
      glyph.m_pGlyph = pFaceCache->LoadGlyphBitmap(
          pFont, charpos.m_GlyphIndex, charpos.m_bFontStyle, &deviceCtm,
          charpos.m_FontCharWidth, anti_alias, nativetext_flags);
    }
  }
  if (anti_alias < FXFT_RENDER_MODE_LCD && glyphs.size() > 1)
    AdjustGlyphSpace(&glyphs);

  FX_RECT bmp_rect1 = FXGE_GetGlyphsBBox(glyphs, anti_alias);
  if (scale_x > 1 && scale_y > 1) {
    bmp_rect1.left--;
    bmp_rect1.top--;
    bmp_rect1.right++;
    bmp_rect1.bottom++;
  }
  FX_RECT bmp_rect(FXSYS_round((FX_FLOAT)(bmp_rect1.left) / scale_x),
                   FXSYS_round((FX_FLOAT)(bmp_rect1.top) / scale_y),
                   FXSYS_round((FX_FLOAT)bmp_rect1.right / scale_x),
                   FXSYS_round((FX_FLOAT)bmp_rect1.bottom / scale_y));
  bmp_rect.Intersect(m_ClipBox);
  if (bmp_rect.IsEmpty())
    return TRUE;

  int pixel_width = FXSYS_round(bmp_rect.Width() * scale_x);
  int pixel_height = FXSYS_round(bmp_rect.Height() * scale_y);
  int pixel_left = FXSYS_round(bmp_rect.left * scale_x);
  int pixel_top = FXSYS_round(bmp_rect.top * scale_y);
  if (anti_alias == FXFT_RENDER_MODE_MONO) {
    CFX_DIBitmap bitmap;
    if (!bitmap.Create(pixel_width, pixel_height, FXDIB_1bppMask))
      return FALSE;

    bitmap.Clear(0);
    for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
      if (!glyph.m_pGlyph)
        continue;

      const CFX_DIBitmap* pGlyph = &glyph.m_pGlyph->m_Bitmap;
      bitmap.TransferBitmap(
          glyph.m_OriginX + glyph.m_pGlyph->m_Left - pixel_left,
          glyph.m_OriginY - glyph.m_pGlyph->m_Top - pixel_top,
          pGlyph->GetWidth(), pGlyph->GetHeight(), pGlyph, 0, 0);
    }
    return SetBitMask(&bitmap, bmp_rect.left, bmp_rect.top, fill_color);
  }
  CFX_DIBitmap bitmap;
  if (m_bpp == 8) {
    if (!bitmap.Create(pixel_width, pixel_height, FXDIB_8bppMask))
      return FALSE;
  } else {
    if (!CreateCompatibleBitmap(&bitmap, pixel_width, pixel_height))
      return FALSE;
  }

  if (!bitmap.HasAlpha() && !bitmap.IsAlphaMask()) {
    bitmap.Clear(0xFFFFFFFF);
    if (!GetDIBits(&bitmap, bmp_rect.left, bmp_rect.top))
      return FALSE;
  } else {
    bitmap.Clear(0);
    if (bitmap.m_pAlphaMask) {
      bitmap.m_pAlphaMask->Clear(0);
    }
  }

  int dest_width = pixel_width;
  int a = 0;
  int r = 0;
  int g = 0;
  int b = 0;
  if (anti_alias == FXFT_RENDER_MODE_LCD)
    ArgbDecode(fill_color, a, r, g, b);

  for (const FXTEXT_GLYPHPOS& glyph : glyphs) {
    if (!glyph.m_pGlyph)
      continue;

    const CFX_DIBitmap* pGlyph = &glyph.m_pGlyph->m_Bitmap;
    int left = glyph.m_OriginX + glyph.m_pGlyph->m_Left - pixel_left;
    int top = glyph.m_OriginY - glyph.m_pGlyph->m_Top - pixel_top;
    int ncols = pGlyph->GetWidth();
    int nrows = pGlyph->GetHeight();
    if (anti_alias == FXFT_RENDER_MODE_NORMAL) {
      if (!bitmap.CompositeMask(left, top, ncols, nrows, pGlyph, fill_color, 0,
                                0, FXDIB_BLEND_NORMAL, nullptr, FALSE, 0,
                                nullptr)) {
        return FALSE;
      }
      continue;
    }

    bool bBGRStripe = !!(text_flags & FXTEXT_BGR_STRIPE);
    ncols /= 3;
    int x_subpixel = (int)(glyph.m_fOriginX * 3) % 3;
    int start_col = std::max(left, 0);
    int end_col = std::min(left + ncols, dest_width);
    if (start_col >= end_col)
      continue;

    DrawNormalTextHelper(&bitmap, pGlyph, nrows, left, top, start_col, end_col,
                         bNormal, bBGRStripe, x_subpixel, a, r, g, b);
  }
  if (bitmap.IsAlphaMask())
    SetBitMask(&bitmap, bmp_rect.left, bmp_rect.top, fill_color);
  else
    SetDIBits(&bitmap, bmp_rect.left, bmp_rect.top);
  return TRUE;
}

FX_BOOL CFX_RenderDevice::DrawTextPathWithFlags(
    int nChars,
    const FXTEXT_CHARPOS* pCharPos,
    CFX_Font* pFont,
    CFX_FontCache* pCache,
    FX_FLOAT font_size,
    const CFX_Matrix* pText2User,
    const CFX_Matrix* pUser2Device,
    const CFX_GraphStateData* pGraphState,
    uint32_t fill_color,
    FX_ARGB stroke_color,
    CFX_PathData* pClippingPath,
    int nFlag) {
  if (!pCache)
    pCache = CFX_GEModule::Get()->GetFontCache();

  CFX_FaceCache* pFaceCache = pCache->GetCachedFace(pFont);
  FX_FONTCACHE_DEFINE(pCache, pFont);
  for (int iChar = 0; iChar < nChars; iChar++) {
    const FXTEXT_CHARPOS& charpos = pCharPos[iChar];
    CFX_Matrix matrix;
    if (charpos.m_bGlyphAdjust)
      matrix.Set(charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
                 charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3], 0, 0);
    matrix.Concat(font_size, 0, 0, font_size, charpos.m_OriginX,
                  charpos.m_OriginY);
    const CFX_PathData* pPath = pFaceCache->LoadGlyphPath(
        pFont, charpos.m_GlyphIndex, charpos.m_FontCharWidth);
    if (!pPath)
      continue;

    matrix.Concat(*pText2User);
    CFX_PathData TransformedPath(*pPath);
    TransformedPath.Transform(&matrix);
    if (fill_color || stroke_color) {
      int fill_mode = nFlag;
      if (fill_color)
        fill_mode |= FXFILL_WINDING;
      fill_mode |= FX_FILL_TEXT_MODE;
      if (!DrawPathWithBlend(&TransformedPath, pUser2Device, pGraphState,
                             fill_color, stroke_color, fill_mode,
                             FXDIB_BLEND_NORMAL)) {
        return FALSE;
      }
    }
    if (pClippingPath)
      pClippingPath->Append(&TransformedPath, pUser2Device);
  }
  return TRUE;
}

CFX_FontCache::CFX_FontCache() {}

CFX_FontCache::~CFX_FontCache() {
  FreeCache(TRUE);
}

CFX_FaceCache* CFX_FontCache::GetCachedFace(CFX_Font* pFont) {
  FXFT_Face internal_face = pFont->GetFace();
  const bool bExternal = !internal_face;
  FXFT_Face face =
      bExternal ? (FXFT_Face)pFont->GetSubstFont()->m_ExtHandle : internal_face;
  CFX_FTCacheMap& map = bExternal ? m_ExtFaceMap : m_FTFaceMap;
  auto it = map.find(face);
  if (it != map.end()) {
    CFX_CountedFaceCache* counted_face_cache = it->second;
    counted_face_cache->m_nCount++;
    return counted_face_cache->m_Obj;
  }

  CFX_FaceCache* face_cache = new CFX_FaceCache(bExternal ? nullptr : face);
  CFX_CountedFaceCache* counted_face_cache = new CFX_CountedFaceCache;
  counted_face_cache->m_nCount = 2;
  counted_face_cache->m_Obj = face_cache;
  map[face] = counted_face_cache;
  return face_cache;
}

#ifdef _SKIA_SUPPORT_
CFX_TypeFace* CFX_FontCache::GetDeviceCache(CFX_Font* pFont) {
  return GetCachedFace(pFont)->GetDeviceCache(pFont);
}

CFX_TypeFace* CFX_FaceCache::GetDeviceCache(CFX_Font* pFont) {
  if (!m_pTypeface) {
    m_pTypeface =
        SkTypeface::MakeFromStream(
            new SkMemoryStream(pFont->GetFontData(), pFont->GetSize()))
            .release();
  }
  return m_pTypeface;
}
#endif

void CFX_FontCache::ReleaseCachedFace(CFX_Font* pFont) {
  FXFT_Face internal_face = pFont->GetFace();
  const bool bExternal = !internal_face;
  FXFT_Face face =
      bExternal ? (FXFT_Face)pFont->GetSubstFont()->m_ExtHandle : internal_face;
  CFX_FTCacheMap& map = bExternal ? m_ExtFaceMap : m_FTFaceMap;

  auto it = map.find(face);
  if (it == map.end())
    return;

  CFX_CountedFaceCache* counted_face_cache = it->second;
  if (counted_face_cache->m_nCount > 1) {
    counted_face_cache->m_nCount--;
  }
}

void CFX_FontCache::FreeCache(FX_BOOL bRelease) {
  for (auto it = m_FTFaceMap.begin(); it != m_FTFaceMap.end();) {
    auto curr_it = it++;
    CFX_CountedFaceCache* cache = curr_it->second;
    if (bRelease || cache->m_nCount < 2) {
      delete cache->m_Obj;
      delete cache;
      m_FTFaceMap.erase(curr_it);
    }
  }

  for (auto it = m_ExtFaceMap.begin(); it != m_ExtFaceMap.end();) {
    auto curr_it = it++;
    CFX_CountedFaceCache* cache = curr_it->second;
    if (bRelease || cache->m_nCount < 2) {
      delete cache->m_Obj;
      delete cache;
      m_ExtFaceMap.erase(curr_it);
    }
  }
}

CFX_FaceCache::CFX_FaceCache(FXFT_Face face)
    : m_Face(face)
#ifdef _SKIA_SUPPORT_
      ,
      m_pTypeface(nullptr)
#endif
{
}

CFX_FaceCache::~CFX_FaceCache() {
  for (const auto& pair : m_SizeMap) {
    delete pair.second;
  }
  m_SizeMap.clear();
  for (const auto& pair : m_PathMap) {
    delete pair.second;
  }
  m_PathMap.clear();
#ifdef _SKIA_SUPPORT_
  SkSafeUnref(m_pTypeface);
#endif
}

#if _FXM_PLATFORM_ != _FXM_PLATFORM_APPLE_
void CFX_FaceCache::InitPlatform() {}
#endif
CFX_GlyphBitmap* CFX_FaceCache::LookUpGlyphBitmap(
    CFX_Font* pFont,
    const CFX_Matrix* pMatrix,
    const CFX_ByteString& FaceGlyphsKey,
    uint32_t glyph_index,
    FX_BOOL bFontStyle,
    int dest_width,
    int anti_alias) {
  CFX_SizeGlyphCache* pSizeCache;
  auto it = m_SizeMap.find(FaceGlyphsKey);
  if (it == m_SizeMap.end()) {
    pSizeCache = new CFX_SizeGlyphCache;
    m_SizeMap[FaceGlyphsKey] = pSizeCache;
  } else {
    pSizeCache = it->second;
  }
  auto it2 = pSizeCache->m_GlyphMap.find(glyph_index);
  if (it2 != pSizeCache->m_GlyphMap.end())
    return it2->second;

  CFX_GlyphBitmap* pGlyphBitmap = RenderGlyph(pFont, glyph_index, bFontStyle,
                                              pMatrix, dest_width, anti_alias);
  if (!pGlyphBitmap)
    return nullptr;

  pSizeCache->m_GlyphMap[glyph_index] = pGlyphBitmap;
  return pGlyphBitmap;
}
const CFX_GlyphBitmap* CFX_FaceCache::LoadGlyphBitmap(CFX_Font* pFont,
                                                      uint32_t glyph_index,
                                                      FX_BOOL bFontStyle,
                                                      const CFX_Matrix* pMatrix,
                                                      int dest_width,
                                                      int anti_alias,
                                                      int& text_flags) {
  if (glyph_index == (uint32_t)-1) {
    return nullptr;
  }
  _CFX_UniqueKeyGen keygen;
#if _FXM_PLATFORM_ != _FXM_PLATFORM_APPLE_
  if (pFont->GetSubstFont())
    keygen.Generate(9, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                    (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                    dest_width, anti_alias, pFont->GetSubstFont()->m_Weight,
                    pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical());
  else
    keygen.Generate(6, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                    (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                    dest_width, anti_alias);
#else
  if (text_flags & FXTEXT_NO_NATIVETEXT) {
    if (pFont->GetSubstFont())
      keygen.Generate(9, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                      (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                      dest_width, anti_alias, pFont->GetSubstFont()->m_Weight,
                      pFont->GetSubstFont()->m_ItalicAngle,
                      pFont->IsVertical());
    else
      keygen.Generate(6, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                      (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                      dest_width, anti_alias);
  } else {
    if (pFont->GetSubstFont())
      keygen.Generate(10, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                      (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                      dest_width, anti_alias, pFont->GetSubstFont()->m_Weight,
                      pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical(),
                      3);
    else
      keygen.Generate(7, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                      (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                      dest_width, anti_alias, 3);
  }
#endif
  CFX_ByteString FaceGlyphsKey(keygen.m_Key, keygen.m_KeyLen);
#if _FXM_PLATFORM_ != _FXM_PLATFORM_APPLE_
  return LookUpGlyphBitmap(pFont, pMatrix, FaceGlyphsKey, glyph_index,
                           bFontStyle, dest_width, anti_alias);
#else
  if (text_flags & FXTEXT_NO_NATIVETEXT) {
    return LookUpGlyphBitmap(pFont, pMatrix, FaceGlyphsKey, glyph_index,
                             bFontStyle, dest_width, anti_alias);
  }
  CFX_GlyphBitmap* pGlyphBitmap;
  auto it = m_SizeMap.find(FaceGlyphsKey);
  if (it != m_SizeMap.end()) {
    CFX_SizeGlyphCache* pSizeCache = it->second;
    auto it2 = pSizeCache->m_GlyphMap.find(glyph_index);
    if (it2 != pSizeCache->m_GlyphMap.end())
      return it2->second;

    pGlyphBitmap = RenderGlyph_Nativetext(pFont, glyph_index, pMatrix,
                                          dest_width, anti_alias);
    if (pGlyphBitmap) {
      pSizeCache->m_GlyphMap[glyph_index] = pGlyphBitmap;
      return pGlyphBitmap;
    }
  } else {
    pGlyphBitmap = RenderGlyph_Nativetext(pFont, glyph_index, pMatrix,
                                          dest_width, anti_alias);
    if (pGlyphBitmap) {
      CFX_SizeGlyphCache* pSizeCache = new CFX_SizeGlyphCache;
      m_SizeMap[FaceGlyphsKey] = pSizeCache;
      pSizeCache->m_GlyphMap[glyph_index] = pGlyphBitmap;
      return pGlyphBitmap;
    }
  }
  if (pFont->GetSubstFont())
    keygen.Generate(9, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                    (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                    dest_width, anti_alias, pFont->GetSubstFont()->m_Weight,
                    pFont->GetSubstFont()->m_ItalicAngle, pFont->IsVertical());
  else
    keygen.Generate(6, (int)(pMatrix->a * 10000), (int)(pMatrix->b * 10000),
                    (int)(pMatrix->c * 10000), (int)(pMatrix->d * 10000),
                    dest_width, anti_alias);
  CFX_ByteString FaceGlyphsKey2(keygen.m_Key, keygen.m_KeyLen);
  text_flags |= FXTEXT_NO_NATIVETEXT;
  return LookUpGlyphBitmap(pFont, pMatrix, FaceGlyphsKey2, glyph_index,
                           bFontStyle, dest_width, anti_alias);
#endif
}

CFX_SizeGlyphCache::CFX_SizeGlyphCache() {}

CFX_SizeGlyphCache::~CFX_SizeGlyphCache() {
  for (const auto& pair : m_GlyphMap) {
    delete pair.second;
  }
  m_GlyphMap.clear();
}

#define CONTRAST_RAMP_STEP 1
void CFX_Font::AdjustMMParams(int glyph_index, int dest_width, int weight) {
  FXFT_MM_Var pMasters = nullptr;
  FXFT_Get_MM_Var(m_Face, &pMasters);
  if (!pMasters) {
    return;
  }
  long coords[2];
  if (weight == 0) {
    coords[0] = FXFT_Get_MM_Axis_Def(FXFT_Get_MM_Axis(pMasters, 0)) / 65536;
  } else {
    coords[0] = weight;
  }
  if (dest_width == 0) {
    coords[1] = FXFT_Get_MM_Axis_Def(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
  } else {
    int min_param = FXFT_Get_MM_Axis_Min(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
    int max_param = FXFT_Get_MM_Axis_Max(FXFT_Get_MM_Axis(pMasters, 1)) / 65536;
    coords[1] = min_param;
    (void)FXFT_Set_MM_Design_Coordinates(m_Face, 2, coords);
    (void)FXFT_Load_Glyph(
        m_Face, glyph_index,
        FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
    int min_width = FXFT_Get_Glyph_HoriAdvance(m_Face) * 1000 /
                    FXFT_Get_Face_UnitsPerEM(m_Face);
    coords[1] = max_param;
    (void)FXFT_Set_MM_Design_Coordinates(m_Face, 2, coords);
    (void)FXFT_Load_Glyph(
        m_Face, glyph_index,
        FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
    int max_width = FXFT_Get_Glyph_HoriAdvance(m_Face) * 1000 /
                    FXFT_Get_Face_UnitsPerEM(m_Face);
    if (max_width == min_width) {
      FXFT_Free(m_Face, pMasters);
      return;
    }
    int param = min_param +
                (max_param - min_param) * (dest_width - min_width) /
                    (max_width - min_width);
    coords[1] = param;
  }
  FXFT_Free(m_Face, pMasters);
  FXFT_Set_MM_Design_Coordinates(m_Face, 2, coords);
}
static const size_t ANGLESKEW_ARRAY_SIZE = 30;
static const char g_AngleSkew[ANGLESKEW_ARRAY_SIZE] = {
    0,  2,  3,  5,  7,  9,  11, 12, 14, 16, 18, 19, 21, 23, 25,
    27, 29, 31, 32, 34, 36, 38, 40, 42, 45, 47, 49, 51, 53, 55,
};
static const size_t WEIGHTPOW_ARRAY_SIZE = 100;
static const uint8_t g_WeightPow[WEIGHTPOW_ARRAY_SIZE] = {
    0,  3,  6,  7,  8,  9,  11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 36, 37,
    37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42,
    42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47,
    47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50,
    51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53,
};
static const uint8_t g_WeightPow_11[WEIGHTPOW_ARRAY_SIZE] = {
    0,  4,  7,  8,  9,  10, 12, 13, 15, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 39, 40, 40, 41,
    41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46,
    46, 43, 47, 47, 48, 48, 48, 48, 45, 50, 50, 50, 46, 51, 51, 51, 52,
    52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 55,
    56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58,
};
static const uint8_t g_WeightPow_SHIFTJIS[WEIGHTPOW_ARRAY_SIZE] = {
    0,  0,  1,  2,  3,  4,  5,  7,  8,  10, 11, 13, 14, 16, 17, 19, 21,
    22, 24, 26, 28, 30, 32, 33, 35, 37, 39, 41, 43, 45, 48, 48, 48, 48,
    49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53,
    53, 53, 53, 53, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56,
    56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58,
    59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60,
};
static void _GammaAdjust(uint8_t* pData,
                         int nWid,
                         int nHei,
                         int src_pitch,
                         const uint8_t* gammaTable) {
  int count = nHei * src_pitch;
  for (int i = 0; i < count; i++) {
    pData[i] = gammaTable[pData[i]];
  }
}
static void _ContrastAdjust(uint8_t* pDataIn,
                            uint8_t* pDataOut,
                            int nWid,
                            int nHei,
                            int nSrcRowBytes,
                            int nDstRowBytes) {
  int col, row, temp;
  int max = 0, min = 255;
  FX_FLOAT rate;
  for (row = 0; row < nHei; row++) {
    uint8_t* pRow = pDataIn + row * nSrcRowBytes;
    for (col = 0; col < nWid; col++) {
      temp = *pRow++;
      if (temp > max) {
        max = temp;
      }
      if (temp < min) {
        min = temp;
      }
    }
  }
  temp = max - min;
  if (0 == temp || 255 == temp) {
    int rowbytes = FXSYS_abs(nSrcRowBytes) > nDstRowBytes
                       ? nDstRowBytes
                       : FXSYS_abs(nSrcRowBytes);
    for (row = 0; row < nHei; row++) {
      FXSYS_memcpy(pDataOut + row * nDstRowBytes, pDataIn + row * nSrcRowBytes,
                   rowbytes);
    }
    return;
  }
  rate = 255.f / temp;
  for (row = 0; row < nHei; row++) {
    uint8_t* pSrcRow = pDataIn + row * nSrcRowBytes;
    uint8_t* pDstRow = pDataOut + row * nDstRowBytes;
    for (col = 0; col < nWid; col++) {
      temp = (int)((*(pSrcRow++) - min) * rate + 0.5);
      if (temp > 255) {
        temp = 255;
      } else if (temp < 0) {
        temp = 0;
      }
      *pDstRow++ = (uint8_t)temp;
    }
  }
}
CFX_GlyphBitmap* CFX_FaceCache::RenderGlyph(CFX_Font* pFont,
                                            uint32_t glyph_index,
                                            FX_BOOL bFontStyle,
                                            const CFX_Matrix* pMatrix,
                                            int dest_width,
                                            int anti_alias) {
  if (!m_Face) {
    return nullptr;
  }
  FXFT_Matrix ft_matrix;
  ft_matrix.xx = (signed long)(pMatrix->GetA() / 64 * 65536);
  ft_matrix.xy = (signed long)(pMatrix->GetC() / 64 * 65536);
  ft_matrix.yx = (signed long)(pMatrix->GetB() / 64 * 65536);
  ft_matrix.yy = (signed long)(pMatrix->GetD() / 64 * 65536);
  FX_BOOL bUseCJKSubFont = FALSE;
  const CFX_SubstFont* pSubstFont = pFont->GetSubstFont();
  if (pSubstFont) {
    bUseCJKSubFont = pSubstFont->m_bSubstCJK && bFontStyle;
    int skew = 0;
    if (bUseCJKSubFont) {
      skew = pSubstFont->m_bItalicCJK ? -15 : 0;
    } else {
      skew = pSubstFont->m_ItalicAngle;
    }
    if (skew) {
      // |skew| is nonpositive so |-skew| is used as the index. We need to make
      // sure |skew| != INT_MIN since -INT_MIN is undefined.
      if (skew <= 0 && skew != std::numeric_limits<int>::min() &&
          static_cast<size_t>(-skew) < ANGLESKEW_ARRAY_SIZE) {
        skew = -g_AngleSkew[-skew];
      } else {
        skew = -58;
      }
      if (pFont->IsVertical())
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy -= ft_matrix.xx * skew / 100;
    }
    if (pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) {
      pFont->AdjustMMParams(glyph_index, dest_width,
                            pFont->GetSubstFont()->m_Weight);
    }
  }
  ScopedFontTransform scoped_transform(m_Face, &ft_matrix);
  int load_flags = (m_Face->face_flags & FT_FACE_FLAG_SFNT)
                       ? FXFT_LOAD_NO_BITMAP
                       : (FXFT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
  int error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);
  if (error) {
    // if an error is returned, try to reload glyphs without hinting.
    if (load_flags & FT_LOAD_NO_HINTING || load_flags & FT_LOAD_NO_SCALE) {
      return nullptr;
    }

    load_flags |= FT_LOAD_NO_HINTING;
    error = FXFT_Load_Glyph(m_Face, glyph_index, load_flags);

    if (error) {
      return nullptr;
    }
  }
  int weight = 0;
  if (bUseCJKSubFont) {
    weight = pSubstFont->m_WeightCJK;
  } else {
    weight = pSubstFont ? pSubstFont->m_Weight : 0;
  }
  if (pSubstFont && !(pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) &&
      weight > 400) {
    uint32_t index = (weight - 400) / 10;
    if (index >= WEIGHTPOW_ARRAY_SIZE)
      return nullptr;
    int level = 0;
    if (pSubstFont->m_Charset == FXFONT_SHIFTJIS_CHARSET) {
      level =
          g_WeightPow_SHIFTJIS[index] * 2 *
          (FXSYS_abs((int)(ft_matrix.xx)) + FXSYS_abs((int)(ft_matrix.xy))) /
          36655;
    } else {
      level = g_WeightPow_11[index] * (FXSYS_abs((int)(ft_matrix.xx)) +
                                       FXSYS_abs((int)(ft_matrix.xy))) /
              36655;
    }
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face), level);
  }
  FXFT_Library_SetLcdFilter(CFX_GEModule::Get()->GetFontMgr()->GetFTLibrary(),
                            FT_LCD_FILTER_DEFAULT);
  error = FXFT_Render_Glyph(m_Face, anti_alias);
  if (error) {
    return nullptr;
  }
  int bmwidth = FXFT_Get_Bitmap_Width(FXFT_Get_Glyph_Bitmap(m_Face));
  int bmheight = FXFT_Get_Bitmap_Rows(FXFT_Get_Glyph_Bitmap(m_Face));
  if (bmwidth > 2048 || bmheight > 2048) {
    return nullptr;
  }
  int dib_width = bmwidth;
  CFX_GlyphBitmap* pGlyphBitmap = new CFX_GlyphBitmap;
  pGlyphBitmap->m_Bitmap.Create(
      dib_width, bmheight,
      anti_alias == FXFT_RENDER_MODE_MONO ? FXDIB_1bppMask : FXDIB_8bppMask);
  pGlyphBitmap->m_Left = FXFT_Get_Glyph_BitmapLeft(m_Face);
  pGlyphBitmap->m_Top = FXFT_Get_Glyph_BitmapTop(m_Face);
  int dest_pitch = pGlyphBitmap->m_Bitmap.GetPitch();
  int src_pitch = FXFT_Get_Bitmap_Pitch(FXFT_Get_Glyph_Bitmap(m_Face));
  uint8_t* pDestBuf = pGlyphBitmap->m_Bitmap.GetBuffer();
  uint8_t* pSrcBuf =
      (uint8_t*)FXFT_Get_Bitmap_Buffer(FXFT_Get_Glyph_Bitmap(m_Face));
  if (anti_alias != FXFT_RENDER_MODE_MONO &&
      FXFT_Get_Bitmap_PixelMode(FXFT_Get_Glyph_Bitmap(m_Face)) ==
          FXFT_PIXEL_MODE_MONO) {
    int bytes = anti_alias == FXFT_RENDER_MODE_LCD ? 3 : 1;
    for (int i = 0; i < bmheight; i++)
      for (int n = 0; n < bmwidth; n++) {
        uint8_t data =
            (pSrcBuf[i * src_pitch + n / 8] & (0x80 >> (n % 8))) ? 255 : 0;
        for (int b = 0; b < bytes; b++) {
          pDestBuf[i * dest_pitch + n * bytes + b] = data;
        }
      }
  } else {
    FXSYS_memset(pDestBuf, 0, dest_pitch * bmheight);
    if (anti_alias == FXFT_RENDER_MODE_MONO &&
        FXFT_Get_Bitmap_PixelMode(FXFT_Get_Glyph_Bitmap(m_Face)) ==
            FXFT_PIXEL_MODE_MONO) {
      int rowbytes =
          FXSYS_abs(src_pitch) > dest_pitch ? dest_pitch : FXSYS_abs(src_pitch);
      for (int row = 0; row < bmheight; row++) {
        FXSYS_memcpy(pDestBuf + row * dest_pitch, pSrcBuf + row * src_pitch,
                     rowbytes);
      }
    } else {
      _ContrastAdjust(pSrcBuf, pDestBuf, bmwidth, bmheight, src_pitch,
                      dest_pitch);
      _GammaAdjust(pDestBuf, bmwidth, bmheight, dest_pitch,
                   CFX_GEModule::Get()->GetTextGammaTable());
    }
  }
  return pGlyphBitmap;
}
const CFX_PathData* CFX_FaceCache::LoadGlyphPath(CFX_Font* pFont,
                                                 uint32_t glyph_index,
                                                 int dest_width) {
  if (!m_Face || glyph_index == (uint32_t)-1)
    return nullptr;

  uint32_t key = glyph_index;
  if (pFont->GetSubstFont()) {
    key += (((pFont->GetSubstFont()->m_Weight / 16) << 15) +
            ((pFont->GetSubstFont()->m_ItalicAngle / 2) << 21) +
            ((dest_width / 16) << 25) + (pFont->IsVertical() << 31));
  }
  auto it = m_PathMap.find(key);
  if (it != m_PathMap.end())
    return it->second;

  CFX_PathData* pGlyphPath = pFont->LoadGlyphPath(glyph_index, dest_width);
  m_PathMap[key] = pGlyphPath;
  return pGlyphPath;
}
typedef struct {
  FX_BOOL m_bCount;
  int m_PointCount;
  FX_PATHPOINT* m_pPoints;
  int m_CurX;
  int m_CurY;
  FX_FLOAT m_CoordUnit;
} OUTLINE_PARAMS;
void _Outline_CheckEmptyContour(OUTLINE_PARAMS* param) {
  if (param->m_PointCount >= 2 &&
      param->m_pPoints[param->m_PointCount - 2].m_Flag == FXPT_MOVETO &&
      param->m_pPoints[param->m_PointCount - 2].m_PointX ==
          param->m_pPoints[param->m_PointCount - 1].m_PointX &&
      param->m_pPoints[param->m_PointCount - 2].m_PointY ==
          param->m_pPoints[param->m_PointCount - 1].m_PointY) {
    param->m_PointCount -= 2;
  }
  if (param->m_PointCount >= 4 &&
      param->m_pPoints[param->m_PointCount - 4].m_Flag == FXPT_MOVETO &&
      param->m_pPoints[param->m_PointCount - 3].m_Flag == FXPT_BEZIERTO &&
      param->m_pPoints[param->m_PointCount - 3].m_PointX ==
          param->m_pPoints[param->m_PointCount - 4].m_PointX &&
      param->m_pPoints[param->m_PointCount - 3].m_PointY ==
          param->m_pPoints[param->m_PointCount - 4].m_PointY &&
      param->m_pPoints[param->m_PointCount - 2].m_PointX ==
          param->m_pPoints[param->m_PointCount - 4].m_PointX &&
      param->m_pPoints[param->m_PointCount - 2].m_PointY ==
          param->m_pPoints[param->m_PointCount - 4].m_PointY &&
      param->m_pPoints[param->m_PointCount - 1].m_PointX ==
          param->m_pPoints[param->m_PointCount - 4].m_PointX &&
      param->m_pPoints[param->m_PointCount - 1].m_PointY ==
          param->m_pPoints[param->m_PointCount - 4].m_PointY) {
    param->m_PointCount -= 4;
  }
}
extern "C" {
static int _Outline_MoveTo(const FXFT_Vector* to, void* user) {
  OUTLINE_PARAMS* param = (OUTLINE_PARAMS*)user;
  if (!param->m_bCount) {
    _Outline_CheckEmptyContour(param);
    param->m_pPoints[param->m_PointCount].m_PointX = to->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_PointY = to->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_Flag = FXPT_MOVETO;
    param->m_CurX = to->x;
    param->m_CurY = to->y;
    if (param->m_PointCount) {
      param->m_pPoints[param->m_PointCount - 1].m_Flag |= FXPT_CLOSEFIGURE;
    }
  }
  param->m_PointCount++;
  return 0;
}
};
extern "C" {
static int _Outline_LineTo(const FXFT_Vector* to, void* user) {
  OUTLINE_PARAMS* param = (OUTLINE_PARAMS*)user;
  if (!param->m_bCount) {
    param->m_pPoints[param->m_PointCount].m_PointX = to->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_PointY = to->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_Flag = FXPT_LINETO;
    param->m_CurX = to->x;
    param->m_CurY = to->y;
  }
  param->m_PointCount++;
  return 0;
}
};
extern "C" {
static int _Outline_ConicTo(const FXFT_Vector* control,
                            const FXFT_Vector* to,
                            void* user) {
  OUTLINE_PARAMS* param = (OUTLINE_PARAMS*)user;
  if (!param->m_bCount) {
    param->m_pPoints[param->m_PointCount].m_PointX =
        (param->m_CurX + (control->x - param->m_CurX) * 2 / 3) /
        param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_PointY =
        (param->m_CurY + (control->y - param->m_CurY) * 2 / 3) /
        param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_Flag = FXPT_BEZIERTO;
    param->m_pPoints[param->m_PointCount + 1].m_PointX =
        (control->x + (to->x - control->x) / 3) / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 1].m_PointY =
        (control->y + (to->y - control->y) / 3) / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 1].m_Flag = FXPT_BEZIERTO;
    param->m_pPoints[param->m_PointCount + 2].m_PointX =
        to->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 2].m_PointY =
        to->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 2].m_Flag = FXPT_BEZIERTO;
    param->m_CurX = to->x;
    param->m_CurY = to->y;
  }
  param->m_PointCount += 3;
  return 0;
}
};
extern "C" {
static int _Outline_CubicTo(const FXFT_Vector* control1,
                            const FXFT_Vector* control2,
                            const FXFT_Vector* to,
                            void* user) {
  OUTLINE_PARAMS* param = (OUTLINE_PARAMS*)user;
  if (!param->m_bCount) {
    param->m_pPoints[param->m_PointCount].m_PointX =
        control1->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_PointY =
        control1->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount].m_Flag = FXPT_BEZIERTO;
    param->m_pPoints[param->m_PointCount + 1].m_PointX =
        control2->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 1].m_PointY =
        control2->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 1].m_Flag = FXPT_BEZIERTO;
    param->m_pPoints[param->m_PointCount + 2].m_PointX =
        to->x / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 2].m_PointY =
        to->y / param->m_CoordUnit;
    param->m_pPoints[param->m_PointCount + 2].m_Flag = FXPT_BEZIERTO;
    param->m_CurX = to->x;
    param->m_CurY = to->y;
  }
  param->m_PointCount += 3;
  return 0;
}
};
CFX_PathData* CFX_Font::LoadGlyphPath(uint32_t glyph_index, int dest_width) {
  if (!m_Face) {
    return nullptr;
  }
  FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
  FXFT_Matrix ft_matrix = {65536, 0, 0, 65536};
  if (m_pSubstFont) {
    if (m_pSubstFont->m_ItalicAngle) {
      int skew = m_pSubstFont->m_ItalicAngle;
      // |skew| is nonpositive so |-skew| is used as the index. We need to make
      // sure |skew| != INT_MIN since -INT_MIN is undefined.
      if (skew <= 0 && skew != std::numeric_limits<int>::min() &&
          static_cast<size_t>(-skew) < ANGLESKEW_ARRAY_SIZE) {
        skew = -g_AngleSkew[-skew];
      } else {
        skew = -58;
      }
      if (m_bVertical)
        ft_matrix.yx += ft_matrix.yy * skew / 100;
      else
        ft_matrix.xy -= ft_matrix.xx * skew / 100;
    }
    if (m_pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) {
      AdjustMMParams(glyph_index, dest_width, m_pSubstFont->m_Weight);
    }
  }
  ScopedFontTransform scoped_transform(m_Face, &ft_matrix);
  int load_flags = FXFT_LOAD_NO_BITMAP;
  if (!(m_Face->face_flags & FT_FACE_FLAG_SFNT) || !FT_IS_TRICKY(m_Face)) {
    load_flags |= FT_LOAD_NO_HINTING;
  }
  if (FXFT_Load_Glyph(m_Face, glyph_index, load_flags))
    return nullptr;
  if (m_pSubstFont && !(m_pSubstFont->m_SubstFlags & FXFONT_SUBST_MM) &&
      m_pSubstFont->m_Weight > 400) {
    uint32_t index = (m_pSubstFont->m_Weight - 400) / 10;
    if (index >= WEIGHTPOW_ARRAY_SIZE)
      index = WEIGHTPOW_ARRAY_SIZE - 1;
    int level = 0;
    if (m_pSubstFont->m_Charset == FXFONT_SHIFTJIS_CHARSET)
      level = g_WeightPow_SHIFTJIS[index] * 2 * 65536 / 36655;
    else
      level = g_WeightPow[index] * 2;
    FXFT_Outline_Embolden(FXFT_Get_Glyph_Outline(m_Face), level);
  }
  FXFT_Outline_Funcs funcs;
  funcs.move_to = _Outline_MoveTo;
  funcs.line_to = _Outline_LineTo;
  funcs.conic_to = _Outline_ConicTo;
  funcs.cubic_to = _Outline_CubicTo;
  funcs.shift = 0;
  funcs.delta = 0;
  OUTLINE_PARAMS params;
  params.m_bCount = TRUE;
  params.m_PointCount = 0;
  FXFT_Outline_Decompose(FXFT_Get_Glyph_Outline(m_Face), &funcs, &params);
  if (params.m_PointCount == 0) {
    return nullptr;
  }
  CFX_PathData* pPath = new CFX_PathData;
  pPath->SetPointCount(params.m_PointCount);
  params.m_bCount = FALSE;
  params.m_PointCount = 0;
  params.m_pPoints = pPath->GetPoints();
  params.m_CurX = params.m_CurY = 0;
  params.m_CoordUnit = 64 * 64.0;
  FXFT_Outline_Decompose(FXFT_Get_Glyph_Outline(m_Face), &funcs, &params);
  _Outline_CheckEmptyContour(&params);
  pPath->TrimPoints(params.m_PointCount);
  if (params.m_PointCount) {
    pPath->GetPoints()[params.m_PointCount - 1].m_Flag |= FXPT_CLOSEFIGURE;
  }
  return pPath;
}
void _CFX_UniqueKeyGen::Generate(int count, ...) {
  va_list argList;
  va_start(argList, count);
  for (int i = 0; i < count; i++) {
    int p = va_arg(argList, int);
    ((uint32_t*)m_Key)[i] = p;
  }
  va_end(argList);
  m_KeyLen = count * sizeof(uint32_t);
}
