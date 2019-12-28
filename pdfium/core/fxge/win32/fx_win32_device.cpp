// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/include/fx_ge.h"

#include <algorithm>
#include <memory>
#include <vector>

#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_DESKTOP_
#include <crtdbg.h>

#include "core/fxcodec/include/fx_codec.h"

#ifndef _SKIA_SUPPORT_
#include "core/fxge/agg/fx_agg_driver.h"
#endif

#include "core/fxge/dib/dib_int.h"
#include "core/fxge/ge/fx_text_int.h"
#include "core/fxge/include/fx_font.h"
#include "core/fxge/include/fx_freetype.h"
#include "core/fxge/include/fx_ge_win32.h"
#include "core/fxge/win32/dwrite_int.h"
#include "core/fxge/win32/win32_int.h"
#include "third_party/base/stl_util.h"

namespace {

const struct {
  const FX_CHAR* m_pFaceName;
  const FX_CHAR* m_pVariantName;
} g_VariantNames[] = {
    {"DFKai-SB", "\x19\x6A\x77\x69\xD4\x9A"},
};

const struct {
  const FX_CHAR* m_pName;
  const FX_CHAR* m_pWinName;
  bool m_bBold;
  bool m_bItalic;
} g_Base14Substs[] = {
    {"Courier", "Courier New", false, false},
    {"Courier-Bold", "Courier New", true, false},
    {"Courier-BoldOblique", "Courier New", true, true},
    {"Courier-Oblique", "Courier New", false, true},
    {"Helvetica", "Arial", false, false},
    {"Helvetica-Bold", "Arial", true, false},
    {"Helvetica-BoldOblique", "Arial", true, true},
    {"Helvetica-Oblique", "Arial", false, true},
    {"Times-Roman", "Times New Roman", false, false},
    {"Times-Bold", "Times New Roman", true, false},
    {"Times-BoldItalic", "Times New Roman", true, true},
    {"Times-Italic", "Times New Roman", false, true},
};

struct FontNameMap {
  const FX_CHAR* m_pSubFontName;
  const FX_CHAR* m_pSrcFontName;
};
const FontNameMap g_JpFontNameMap[] = {
    {"MS Mincho", "Heiseimin-W3"},
    {"MS Gothic", "Jun101-Light"},
};

bool GetSubFontName(CFX_ByteString* name) {
  for (size_t i = 0; i < FX_ArraySize(g_JpFontNameMap); ++i) {
    if (!FXSYS_stricmp(name->c_str(), g_JpFontNameMap[i].m_pSrcFontName)) {
      *name = g_JpFontNameMap[i].m_pSubFontName;
      return true;
    }
  }
  return false;
}

bool IsGDIEnabled() {
  // If GDI is disabled then GetDC for the desktop will fail.
  HDC hdc = ::GetDC(nullptr);
  if (!hdc)
    return false;
  ::ReleaseDC(nullptr, hdc);
  return true;
}

HPEN CreatePen(const CFX_GraphStateData* pGraphState,
               const CFX_Matrix* pMatrix,
               uint32_t argb) {
  FX_FLOAT width;
  FX_FLOAT scale = 1.f;
  if (pMatrix)
    scale = FXSYS_fabs(pMatrix->a) > FXSYS_fabs(pMatrix->b)
                ? FXSYS_fabs(pMatrix->a)
                : FXSYS_fabs(pMatrix->b);
  if (pGraphState) {
    width = scale * pGraphState->m_LineWidth;
  } else {
    width = 1.0f;
  }
  uint32_t PenStyle = PS_GEOMETRIC;
  if (width < 1) {
    width = 1;
  }
  if (pGraphState->m_DashCount) {
    PenStyle |= PS_USERSTYLE;
  } else {
    PenStyle |= PS_SOLID;
  }
  switch (pGraphState->m_LineCap) {
    case 0:
      PenStyle |= PS_ENDCAP_FLAT;
      break;
    case 1:
      PenStyle |= PS_ENDCAP_ROUND;
      break;
    case 2:
      PenStyle |= PS_ENDCAP_SQUARE;
      break;
  }
  switch (pGraphState->m_LineJoin) {
    case 0:
      PenStyle |= PS_JOIN_MITER;
      break;
    case 1:
      PenStyle |= PS_JOIN_ROUND;
      break;
    case 2:
      PenStyle |= PS_JOIN_BEVEL;
      break;
  }
  int a;
  FX_COLORREF rgb;
  ArgbDecode(argb, a, rgb);
  LOGBRUSH lb;
  lb.lbColor = rgb;
  lb.lbStyle = BS_SOLID;
  lb.lbHatch = 0;
  std::vector<uint32_t> dashes;
  if (pGraphState->m_DashCount) {
    dashes.resize(pGraphState->m_DashCount);
    for (int i = 0; i < pGraphState->m_DashCount; i++) {
      dashes[i] = FXSYS_round(
          pMatrix ? pMatrix->TransformDistance(pGraphState->m_DashArray[i])
                  : pGraphState->m_DashArray[i]);
      dashes[i] = std::max(dashes[i], 1U);
    }
  }
  return ExtCreatePen(PenStyle, (DWORD)FXSYS_ceil(width), &lb,
                      pGraphState->m_DashCount,
                      reinterpret_cast<const DWORD*>(dashes.data()));
}

HBRUSH CreateBrush(uint32_t argb) {
  int a;
  FX_COLORREF rgb;
  ArgbDecode(argb, a, rgb);
  return CreateSolidBrush(rgb);
}

void SetPathToDC(HDC hDC,
                 const CFX_PathData* pPathData,
                 const CFX_Matrix* pMatrix) {
  BeginPath(hDC);
  int nPoints = pPathData->GetPointCount();
  FX_PATHPOINT* pPoints = pPathData->GetPoints();
  for (int i = 0; i < nPoints; i++) {
    FX_FLOAT posx = pPoints[i].m_PointX, posy = pPoints[i].m_PointY;
    if (pMatrix) {
      pMatrix->Transform(posx, posy);
    }
    int screen_x = FXSYS_round(posx), screen_y = FXSYS_round(posy);
    int point_type = pPoints[i].m_Flag & FXPT_TYPE;
    if (point_type == PT_MOVETO) {
      MoveToEx(hDC, screen_x, screen_y, nullptr);
    } else if (point_type == PT_LINETO) {
      if (pPoints[i].m_PointY == pPoints[i - 1].m_PointY &&
          pPoints[i].m_PointX == pPoints[i - 1].m_PointX) {
        screen_x++;
      }
      LineTo(hDC, screen_x, screen_y);
    } else if (point_type == PT_BEZIERTO) {
      POINT lppt[3];
      lppt[0].x = screen_x;
      lppt[0].y = screen_y;
      posx = pPoints[i + 1].m_PointX;
      posy = pPoints[i + 1].m_PointY;
      if (pMatrix) {
        pMatrix->Transform(posx, posy);
      }
      lppt[1].x = FXSYS_round(posx);
      lppt[1].y = FXSYS_round(posy);
      posx = pPoints[i + 2].m_PointX;
      posy = pPoints[i + 2].m_PointY;
      if (pMatrix) {
        pMatrix->Transform(posx, posy);
      }
      lppt[2].x = FXSYS_round(posx);
      lppt[2].y = FXSYS_round(posy);
      PolyBezierTo(hDC, lppt, 3);
      i += 2;
    }
    if (pPoints[i].m_Flag & PT_CLOSEFIGURE) {
      CloseFigure(hDC);
    }
  }
  EndPath(hDC);
}

#ifdef _SKIA_SUPPORT_
// TODO(caryclark)  This antigrain function is duplicated here to permit
// removing the last remaining dependency. Eventually, this will be elminiated
// altogether and replace by Skia code.

struct rect_base {
  FX_FLOAT x1;
  FX_FLOAT y1;
  FX_FLOAT x2;
  FX_FLOAT y2;
};

unsigned clip_liang_barsky(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           const rect_base& clip_box,
                           FX_FLOAT* x,
                           FX_FLOAT* y) {
  const FX_FLOAT nearzero = 1e-30f;
  FX_FLOAT deltax = x2 - x1;
  FX_FLOAT deltay = y2 - y1;
  unsigned np = 0;
  if (deltax == 0)
    deltax = (x1 > clip_box.x1) ? -nearzero : nearzero;
  FX_FLOAT xin, xout;
  if (deltax > 0) {
    xin = clip_box.x1;
    xout = clip_box.x2;
  } else {
    xin = clip_box.x2;
    xout = clip_box.x1;
  }
  FX_FLOAT tinx = (xin - x1) / deltax;
  if (deltay == 0)
    deltay = (y1 > clip_box.y1) ? -nearzero : nearzero;
  FX_FLOAT yin, yout;
  if (deltay > 0) {
    yin = clip_box.y1;
    yout = clip_box.y2;
  } else {
    yin = clip_box.y2;
    yout = clip_box.y1;
  }
  FX_FLOAT tiny = (yin - y1) / deltay;
  FX_FLOAT tin1, tin2;
  if (tinx < tiny) {
    tin1 = tinx;
    tin2 = tiny;
  } else {
    tin1 = tiny;
    tin2 = tinx;
  }
  if (tin1 <= 1.0f) {
    if (0 < tin1) {
      *x++ = xin;
      *y++ = yin;
      ++np;
    }
    if (tin2 <= 1.0f) {
      FX_FLOAT toutx = (xout - x1) / deltax;
      FX_FLOAT touty = (yout - y1) / deltay;
      FX_FLOAT tout1 = (toutx < touty) ? toutx : touty;
      if (tin2 > 0 || tout1 > 0) {
        if (tin2 <= tout1) {
          if (tin2 > 0) {
            if (tinx > tiny) {
              *x++ = xin;
              *y++ = y1 + (deltay * tinx);
            } else {
              *x++ = x1 + (deltax * tiny);
              *y++ = yin;
            }
            ++np;
          }
          if (tout1 < 1.0f) {
            if (toutx < touty) {
              *x++ = xout;
              *y++ = y1 + (deltay * toutx);
            } else {
              *x++ = x1 + (deltax * touty);
              *y++ = yout;
            }
          } else {
            *x++ = x2;
            *y++ = y2;
          }
          ++np;
        } else {
          if (tinx > tiny) {
            *x++ = xin;
            *y++ = yout;
          } else {
            *x++ = xout;
            *y++ = yin;
          }
          ++np;
        }
      }
    }
  }
  return np;
}
#endif  // _SKIA_SUPPORT_

FX_BOOL MatrixNoScaled(const CFX_Matrix* pMatrix) {
  return pMatrix->GetA() == 1.0f && pMatrix->GetB() == 0 &&
         pMatrix->GetC() == 0 && pMatrix->GetD() == 1.0f;
}

class CFX_Win32FallbackFontInfo final : public CFX_FolderFontInfo {
 public:
  CFX_Win32FallbackFontInfo() {}
  ~CFX_Win32FallbackFontInfo() override {}

  // CFX_FolderFontInfo:
  void* MapFont(int weight,
                FX_BOOL bItalic,
                int charset,
                int pitch_family,
                const FX_CHAR* family,
                int& iExact) override;
};

class CFX_Win32FontInfo final : public IFX_SystemFontInfo {
 public:
  CFX_Win32FontInfo();
  ~CFX_Win32FontInfo() override;

  // IFX_SystemFontInfo
  FX_BOOL EnumFontList(CFX_FontMapper* pMapper) override;
  void* MapFont(int weight,
                FX_BOOL bItalic,
                int charset,
                int pitch_family,
                const FX_CHAR* face,
                int& iExact) override;
  void* GetFont(const FX_CHAR* face) override { return nullptr; }
  uint32_t GetFontData(void* hFont,
                       uint32_t table,
                       uint8_t* buffer,
                       uint32_t size) override;
  FX_BOOL GetFaceName(void* hFont, CFX_ByteString& name) override;
  FX_BOOL GetFontCharset(void* hFont, int& charset) override;
  void DeleteFont(void* hFont) override;

  FX_BOOL IsOpenTypeFromDiv(const LOGFONTA* plf);
  FX_BOOL IsSupportFontFormDiv(const LOGFONTA* plf);
  void AddInstalledFont(const LOGFONTA* plf, uint32_t FontType);
  void GetGBPreference(CFX_ByteString& face, int weight, int picth_family);
  void GetJapanesePreference(CFX_ByteString& face,
                             int weight,
                             int picth_family);
  CFX_ByteString FindFont(const CFX_ByteString& name);

  HDC m_hDC;
  CFX_FontMapper* m_pMapper;
  CFX_ByteString m_LastFamily;
  CFX_ByteString m_KaiTi, m_FangSong;
};

int CALLBACK FontEnumProc(const LOGFONTA* plf,
                          const TEXTMETRICA* lpntme,
                          uint32_t FontType,
                          LPARAM lParam) {
  CFX_Win32FontInfo* pFontInfo = reinterpret_cast<CFX_Win32FontInfo*>(lParam);
  pFontInfo->AddInstalledFont(plf, FontType);
  return 1;
}

CFX_Win32FontInfo::CFX_Win32FontInfo() : m_hDC(CreateCompatibleDC(nullptr)) {}

CFX_Win32FontInfo::~CFX_Win32FontInfo() {
  DeleteDC(m_hDC);
}

FX_BOOL CFX_Win32FontInfo::IsOpenTypeFromDiv(const LOGFONTA* plf) {
  HFONT hFont = CreateFontIndirectA(plf);
  FX_BOOL ret = FALSE;
  uint32_t font_size = GetFontData(hFont, 0, nullptr, 0);
  if (font_size != GDI_ERROR && font_size >= sizeof(uint32_t)) {
    uint32_t lVersion = 0;
    GetFontData(hFont, 0, (uint8_t*)(&lVersion), sizeof(uint32_t));
    lVersion = (((uint32_t)(uint8_t)(lVersion)) << 24) |
               ((uint32_t)((uint8_t)(lVersion >> 8))) << 16 |
               ((uint32_t)((uint8_t)(lVersion >> 16))) << 8 |
               ((uint8_t)(lVersion >> 24));
    if (lVersion == FXBSTR_ID('O', 'T', 'T', 'O') || lVersion == 0x00010000 ||
        lVersion == FXBSTR_ID('t', 't', 'c', 'f') ||
        lVersion == FXBSTR_ID('t', 'r', 'u', 'e') || lVersion == 0x00020000) {
      ret = TRUE;
    }
  }
  DeleteFont(hFont);
  return ret;
}

FX_BOOL CFX_Win32FontInfo::IsSupportFontFormDiv(const LOGFONTA* plf) {
  HFONT hFont = CreateFontIndirectA(plf);
  FX_BOOL ret = FALSE;
  uint32_t font_size = GetFontData(hFont, 0, nullptr, 0);
  if (font_size != GDI_ERROR && font_size >= sizeof(uint32_t)) {
    uint32_t lVersion = 0;
    GetFontData(hFont, 0, (uint8_t*)(&lVersion), sizeof(uint32_t));
    lVersion = (((uint32_t)(uint8_t)(lVersion)) << 24) |
               ((uint32_t)((uint8_t)(lVersion >> 8))) << 16 |
               ((uint32_t)((uint8_t)(lVersion >> 16))) << 8 |
               ((uint8_t)(lVersion >> 24));
    if (lVersion == FXBSTR_ID('O', 'T', 'T', 'O') || lVersion == 0x00010000 ||
        lVersion == FXBSTR_ID('t', 't', 'c', 'f') ||
        lVersion == FXBSTR_ID('t', 'r', 'u', 'e') || lVersion == 0x00020000 ||
        (lVersion & 0xFFFF0000) == FXBSTR_ID(0x80, 0x01, 0x00, 0x00) ||
        (lVersion & 0xFFFF0000) == FXBSTR_ID('%', '!', 0, 0)) {
      ret = TRUE;
    }
  }
  DeleteFont(hFont);
  return ret;
}

void CFX_Win32FontInfo::AddInstalledFont(const LOGFONTA* plf,
                                         uint32_t FontType) {
  CFX_ByteString name(plf->lfFaceName);
  if (name[0] == '@')
    return;

  if (name == m_LastFamily) {
    m_pMapper->AddInstalledFont(name, plf->lfCharSet);
    return;
  }
  if (!(FontType & TRUETYPE_FONTTYPE)) {
    if (!(FontType & DEVICE_FONTTYPE) || !IsSupportFontFormDiv(plf))
      return;
  }

  m_pMapper->AddInstalledFont(name, plf->lfCharSet);
  m_LastFamily = name;
}

FX_BOOL CFX_Win32FontInfo::EnumFontList(CFX_FontMapper* pMapper) {
  m_pMapper = pMapper;
  LOGFONTA lf;
  FXSYS_memset(&lf, 0, sizeof(LOGFONTA));
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfFaceName[0] = 0;
  lf.lfPitchAndFamily = 0;
  EnumFontFamiliesExA(m_hDC, &lf, (FONTENUMPROCA)FontEnumProc, (uintptr_t) this,
                      0);
  return TRUE;
}

CFX_ByteString CFX_Win32FontInfo::FindFont(const CFX_ByteString& name) {
  if (!m_pMapper)
    return name;

  for (size_t i = 0; i < m_pMapper->m_InstalledTTFonts.size(); ++i) {
    CFX_ByteString thisname = m_pMapper->m_InstalledTTFonts[i];
    if (thisname[0] == ' ') {
      if (thisname.Mid(1, name.GetLength()) == name) {
        return m_pMapper->m_InstalledTTFonts[i + 1];
      }
    } else if (thisname.Left(name.GetLength()) == name) {
      return m_pMapper->m_InstalledTTFonts[i];
    }
  }
  return CFX_ByteString();
}

void* CFX_Win32FallbackFontInfo::MapFont(int weight,
                                         FX_BOOL bItalic,
                                         int charset,
                                         int pitch_family,
                                         const FX_CHAR* cstr_face,
                                         int& iExact) {
  void* font = GetSubstFont(cstr_face);
  if (font) {
    iExact = 1;
    return font;
  }
  FX_BOOL bCJK = TRUE;
  switch (charset) {
    case FXFONT_SHIFTJIS_CHARSET:
    case FXFONT_GB2312_CHARSET:
    case FXFONT_CHINESEBIG5_CHARSET:
    case FXFONT_HANGEUL_CHARSET:
    default:
      bCJK = FALSE;
      break;
  }
  return FindFont(weight, bItalic, charset, pitch_family, cstr_face, !bCJK);
}

void CFX_Win32FontInfo::GetGBPreference(CFX_ByteString& face,
                                        int weight,
                                        int picth_family) {
  if (face.Find("KaiTi") >= 0 || face.Find("\xbf\xac") >= 0) {
    if (m_KaiTi.IsEmpty()) {
      m_KaiTi = FindFont("KaiTi");
      if (m_KaiTi.IsEmpty()) {
        m_KaiTi = "SimSun";
      }
    }
    face = m_KaiTi;
  } else if (face.Find("FangSong") >= 0 || face.Find("\xb7\xc2\xcb\xce") >= 0) {
    if (m_FangSong.IsEmpty()) {
      m_FangSong = FindFont("FangSong");
      if (m_FangSong.IsEmpty()) {
        m_FangSong = "SimSun";
      }
    }
    face = m_FangSong;
  } else if (face.Find("SimSun") >= 0 || face.Find("\xcb\xce") >= 0) {
    face = "SimSun";
  } else if (face.Find("SimHei") >= 0 || face.Find("\xba\xda") >= 0) {
    face = "SimHei";
  } else if (!(picth_family & FF_ROMAN) && weight > 550) {
    face = "SimHei";
  } else {
    face = "SimSun";
  }
}

void CFX_Win32FontInfo::GetJapanesePreference(CFX_ByteString& face,
                                              int weight,
                                              int picth_family) {
  if (face.Find("Gothic") >= 0 ||
      face.Find("\x83\x53\x83\x56\x83\x62\x83\x4e") >= 0) {
    if (face.Find("PGothic") >= 0 ||
        face.Find("\x82\x6f\x83\x53\x83\x56\x83\x62\x83\x4e") >= 0) {
      face = "MS PGothic";
    } else if (face.Find("UI Gothic") >= 0) {
      face = "MS UI Gothic";
    } else {
      if (face.Find("HGSGothicM") >= 0 || face.Find("HGMaruGothicMPRO") >= 0) {
        face = "MS PGothic";
      } else {
        face = "MS Gothic";
      }
    }
    return;
  }
  if (face.Find("Mincho") >= 0 || face.Find("\x96\xbe\x92\xa9") >= 0) {
    if (face.Find("PMincho") >= 0 ||
        face.Find("\x82\x6f\x96\xbe\x92\xa9") >= 0) {
      face = "MS PMincho";
    } else {
      face = "MS Mincho";
    }
    return;
  }
  if (GetSubFontName(&face))
    return;

  if (!(picth_family & FF_ROMAN) && weight > 400) {
    face = "MS PGothic";
  } else {
    face = "MS PMincho";
  }
}

void* CFX_Win32FontInfo::MapFont(int weight,
                                 FX_BOOL bItalic,
                                 int charset,
                                 int pitch_family,
                                 const FX_CHAR* cstr_face,
                                 int& iExact) {
  CFX_ByteString face = cstr_face;
  int iBaseFont;
  for (iBaseFont = 0; iBaseFont < 12; iBaseFont++)
    if (face == CFX_ByteStringC(g_Base14Substs[iBaseFont].m_pName)) {
      face = g_Base14Substs[iBaseFont].m_pWinName;
      weight = g_Base14Substs[iBaseFont].m_bBold ? FW_BOLD : FW_NORMAL;
      bItalic = g_Base14Substs[iBaseFont].m_bItalic;
      iExact = TRUE;
      break;
    }
  if (charset == ANSI_CHARSET || charset == SYMBOL_CHARSET) {
    charset = DEFAULT_CHARSET;
  }
  int subst_pitch_family = pitch_family;
  switch (charset) {
    case SHIFTJIS_CHARSET:
      subst_pitch_family = FF_ROMAN;
      break;
    case CHINESEBIG5_CHARSET:
    case HANGUL_CHARSET:
    case GB2312_CHARSET:
      subst_pitch_family = 0;
      break;
  }
  HFONT hFont =
      ::CreateFontA(-10, 0, 0, 0, weight, bItalic, 0, 0, charset,
                    OUT_TT_ONLY_PRECIS, 0, 0, subst_pitch_family, face.c_str());
  char facebuf[100];
  HFONT hOldFont = (HFONT)::SelectObject(m_hDC, hFont);
  ::GetTextFaceA(m_hDC, 100, facebuf);
  ::SelectObject(m_hDC, hOldFont);
  if (face.EqualNoCase(facebuf))
    return hFont;

  CFX_WideString wsFace = CFX_WideString::FromLocal(facebuf);
  for (size_t i = 0; i < FX_ArraySize(g_VariantNames); ++i) {
    if (face != g_VariantNames[i].m_pFaceName)
      continue;

    const unsigned short* pName = reinterpret_cast<const unsigned short*>(
        g_VariantNames[i].m_pVariantName);
    FX_STRSIZE len = CFX_WideString::WStringLength(pName);
    CFX_WideString wsName = CFX_WideString::FromUTF16LE(pName, len);
    if (wsFace == wsName)
      return hFont;
  }
  ::DeleteObject(hFont);
  if (charset == DEFAULT_CHARSET)
    return nullptr;

  switch (charset) {
    case SHIFTJIS_CHARSET:
      GetJapanesePreference(face, weight, pitch_family);
      break;
    case GB2312_CHARSET:
      GetGBPreference(face, weight, pitch_family);
      break;
    case HANGUL_CHARSET:
      face = "Gulim";
      break;
    case CHINESEBIG5_CHARSET:
      if (face.Find("MSung") >= 0) {
        face = "MingLiU";
      } else {
        face = "PMingLiU";
      }
      break;
  }
  hFont =
      ::CreateFontA(-10, 0, 0, 0, weight, bItalic, 0, 0, charset,
                    OUT_TT_ONLY_PRECIS, 0, 0, subst_pitch_family, face.c_str());
  return hFont;
}

void CFX_Win32FontInfo::DeleteFont(void* hFont) {
  ::DeleteObject(hFont);
}

uint32_t CFX_Win32FontInfo::GetFontData(void* hFont,
                                        uint32_t table,
                                        uint8_t* buffer,
                                        uint32_t size) {
  HFONT hOldFont = (HFONT)::SelectObject(m_hDC, (HFONT)hFont);
  table = FXDWORD_GET_MSBFIRST(reinterpret_cast<uint8_t*>(&table));
  size = ::GetFontData(m_hDC, table, 0, buffer, size);
  ::SelectObject(m_hDC, hOldFont);
  if (size == GDI_ERROR) {
    return 0;
  }
  return size;
}

FX_BOOL CFX_Win32FontInfo::GetFaceName(void* hFont, CFX_ByteString& name) {
  char facebuf[100];
  HFONT hOldFont = (HFONT)::SelectObject(m_hDC, (HFONT)hFont);
  int ret = ::GetTextFaceA(m_hDC, 100, facebuf);
  ::SelectObject(m_hDC, hOldFont);
  if (ret == 0) {
    return FALSE;
  }
  name = facebuf;
  return TRUE;
}

FX_BOOL CFX_Win32FontInfo::GetFontCharset(void* hFont, int& charset) {
  TEXTMETRIC tm;
  HFONT hOldFont = (HFONT)::SelectObject(m_hDC, (HFONT)hFont);
  ::GetTextMetrics(m_hDC, &tm);
  ::SelectObject(m_hDC, hOldFont);
  charset = tm.tmCharSet;
  return TRUE;
}

}  // namespace

std::unique_ptr<IFX_SystemFontInfo> IFX_SystemFontInfo::CreateDefault(
    const char** pUnused) {
  if (IsGDIEnabled())
    return std::unique_ptr<IFX_SystemFontInfo>(new CFX_Win32FontInfo);

  // Select the fallback font information class if GDI is disabled.
  CFX_Win32FallbackFontInfo* pInfoFallback = new CFX_Win32FallbackFontInfo;
  // Construct the font path manually, SHGetKnownFolderPath won't work under
  // a restrictive sandbox.
  CHAR windows_path[MAX_PATH] = {};
  DWORD path_len = ::GetWindowsDirectoryA(windows_path, MAX_PATH);
  if (path_len > 0 && path_len < MAX_PATH) {
    CFX_ByteString fonts_path(windows_path);
    fonts_path += "\\Fonts";
    pInfoFallback->AddPath(fonts_path.AsStringC());
  }
  return std::unique_ptr<IFX_SystemFontInfo>(pInfoFallback);
}

void CFX_GEModule::InitPlatform() {
  CWin32Platform* pPlatformData = new CWin32Platform;
  OSVERSIONINFO ver;
  ver.dwOSVersionInfoSize = sizeof(ver);
  GetVersionEx(&ver);
  pPlatformData->m_bHalfTone = ver.dwMajorVersion >= 5;
  if (IsGDIEnabled())
    pPlatformData->m_GdiplusExt.Load();
  m_pPlatformData = pPlatformData;
  m_pFontMgr->SetSystemFontInfo(IFX_SystemFontInfo::CreateDefault(nullptr));
}

void CFX_GEModule::DestroyPlatform() {
  delete (CWin32Platform*)m_pPlatformData;
  m_pPlatformData = nullptr;
}

CGdiDeviceDriver::CGdiDeviceDriver(HDC hDC, int device_class) {
  m_hDC = hDC;
  m_DeviceClass = device_class;
  CWin32Platform* pPlatform =
      (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
  SetStretchBltMode(hDC, pPlatform->m_bHalfTone ? HALFTONE : COLORONCOLOR);
  if (GetObjectType(m_hDC) == OBJ_MEMDC) {
    HBITMAP hBitmap = CreateBitmap(1, 1, 1, 1, nullptr);
    hBitmap = (HBITMAP)SelectObject(m_hDC, hBitmap);
    BITMAP bitmap;
    GetObject(hBitmap, sizeof bitmap, &bitmap);
    m_nBitsPerPixel = bitmap.bmBitsPixel;
    m_Width = bitmap.bmWidth;
    m_Height = abs(bitmap.bmHeight);
    hBitmap = (HBITMAP)SelectObject(m_hDC, hBitmap);
    DeleteObject(hBitmap);
  } else {
    m_nBitsPerPixel = ::GetDeviceCaps(m_hDC, BITSPIXEL);
    m_Width = ::GetDeviceCaps(m_hDC, HORZRES);
    m_Height = ::GetDeviceCaps(m_hDC, VERTRES);
  }
  if (m_DeviceClass != FXDC_DISPLAY) {
    m_RenderCaps = FXRC_BIT_MASK;
  } else {
    m_RenderCaps = FXRC_GET_BITS | FXRC_BIT_MASK;
  }
}

CGdiDeviceDriver::~CGdiDeviceDriver() {}

int CGdiDeviceDriver::GetDeviceCaps(int caps_id) const {
  switch (caps_id) {
    case FXDC_DEVICE_CLASS:
      return m_DeviceClass;
    case FXDC_PIXEL_WIDTH:
      return m_Width;
    case FXDC_PIXEL_HEIGHT:
      return m_Height;
    case FXDC_BITS_PIXEL:
      return m_nBitsPerPixel;
    case FXDC_RENDER_CAPS:
      return m_RenderCaps;
  }
  return 0;
}

void CGdiDeviceDriver::SaveState() {
  SaveDC(m_hDC);
}

void CGdiDeviceDriver::RestoreState(bool bKeepSaved) {
  RestoreDC(m_hDC, -1);
  if (bKeepSaved)
    SaveDC(m_hDC);
}

FX_BOOL CGdiDeviceDriver::GDI_SetDIBits(CFX_DIBitmap* pBitmap1,
                                        const FX_RECT* pSrcRect,
                                        int left,
                                        int top) {
  if (m_DeviceClass == FXDC_PRINTER) {
    std::unique_ptr<CFX_DIBitmap> pBitmap(pBitmap1->FlipImage(FALSE, TRUE));
    if (!pBitmap)
      return FALSE;

    if (pBitmap->IsCmykImage() && !pBitmap->ConvertFormat(FXDIB_Rgb))
      return FALSE;

    int width = pSrcRect->Width(), height = pSrcRect->Height();
    LPBYTE pBuffer = pBitmap->GetBuffer();
    CFX_ByteString info = CFX_WindowsDIB::GetBitmapInfo(pBitmap.get());
    ((BITMAPINFOHEADER*)info.c_str())->biHeight *= -1;
    FX_RECT dst_rect(0, 0, width, height);
    dst_rect.Intersect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
    int dst_width = dst_rect.Width();
    int dst_height = dst_rect.Height();
    ::StretchDIBits(m_hDC, left, top, dst_width, dst_height, 0, 0, dst_width,
                    dst_height, pBuffer, (BITMAPINFO*)info.c_str(),
                    DIB_RGB_COLORS, SRCCOPY);
  } else {
    CFX_DIBitmap* pBitmap = pBitmap1;
    if (pBitmap->IsCmykImage()) {
      pBitmap = pBitmap->CloneConvert(FXDIB_Rgb);
      if (!pBitmap)
        return FALSE;
    }
    int width = pSrcRect->Width(), height = pSrcRect->Height();
    LPBYTE pBuffer = pBitmap->GetBuffer();
    CFX_ByteString info = CFX_WindowsDIB::GetBitmapInfo(pBitmap);
    ::SetDIBitsToDevice(m_hDC, left, top, width, height, pSrcRect->left,
                        pBitmap->GetHeight() - pSrcRect->bottom, 0,
                        pBitmap->GetHeight(), pBuffer,
                        (BITMAPINFO*)info.c_str(), DIB_RGB_COLORS);
    if (pBitmap != pBitmap1) {
      delete pBitmap;
    }
  }
  return TRUE;
}

FX_BOOL CGdiDeviceDriver::GDI_StretchDIBits(CFX_DIBitmap* pBitmap1,
                                            int dest_left,
                                            int dest_top,
                                            int dest_width,
                                            int dest_height,
                                            uint32_t flags) {
  CFX_DIBitmap* pBitmap = pBitmap1;
  if (!pBitmap || dest_width == 0 || dest_height == 0)
    return FALSE;

  if (pBitmap->IsCmykImage() && !pBitmap->ConvertFormat(FXDIB_Rgb))
    return FALSE;

  CFX_ByteString info = CFX_WindowsDIB::GetBitmapInfo(pBitmap);
  if ((int64_t)abs(dest_width) * abs(dest_height) <
          (int64_t)pBitmap1->GetWidth() * pBitmap1->GetHeight() * 4 ||
      (flags & FXDIB_INTERPOL) || (flags & FXDIB_BICUBIC_INTERPOL)) {
    SetStretchBltMode(m_hDC, HALFTONE);
  } else {
    SetStretchBltMode(m_hDC, COLORONCOLOR);
  }
  CFX_DIBitmap* pToStrechBitmap = pBitmap;
  bool del = false;
  if (m_DeviceClass == FXDC_PRINTER &&
      ((int64_t)pBitmap->GetWidth() * pBitmap->GetHeight() >
       (int64_t)abs(dest_width) * abs(dest_height))) {
    pToStrechBitmap = pBitmap->StretchTo(dest_width, dest_height);
    del = true;
  }
  CFX_ByteString toStrechBitmapInfo =
      CFX_WindowsDIB::GetBitmapInfo(pToStrechBitmap);
  ::StretchDIBits(m_hDC, dest_left, dest_top, dest_width, dest_height, 0, 0,
                  pToStrechBitmap->GetWidth(), pToStrechBitmap->GetHeight(),
                  pToStrechBitmap->GetBuffer(),
                  (BITMAPINFO*)toStrechBitmapInfo.c_str(), DIB_RGB_COLORS,
                  SRCCOPY);
  if (del)
    delete pToStrechBitmap;
  return TRUE;
}

FX_BOOL CGdiDeviceDriver::GDI_StretchBitMask(CFX_DIBitmap* pBitmap1,
                                             int dest_left,
                                             int dest_top,
                                             int dest_width,
                                             int dest_height,
                                             uint32_t bitmap_color,
                                             uint32_t flags) {
  CFX_DIBitmap* pBitmap = pBitmap1;
  if (!pBitmap || dest_width == 0 || dest_height == 0)
    return FALSE;

  int width = pBitmap->GetWidth(), height = pBitmap->GetHeight();
  struct {
    BITMAPINFOHEADER bmiHeader;
    uint32_t bmiColors[2];
  } bmi;
  FXSYS_memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biBitCount = 1;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biWidth = width;
  if (m_nBitsPerPixel != 1) {
    SetStretchBltMode(m_hDC, HALFTONE);
  }
  bmi.bmiColors[0] = 0xffffff;
  bmi.bmiColors[1] = 0;

  HBRUSH hPattern = CreateSolidBrush(bitmap_color & 0xffffff);
  HBRUSH hOld = (HBRUSH)SelectObject(m_hDC, hPattern);

  // In PDF, when image mask is 1, use device bitmap; when mask is 0, use brush
  // bitmap.
  // A complete list of the boolen operations is as follows:

  /* P(bitmap_color)    S(ImageMask)    D(DeviceBitmap)    Result
   *        0                 0                0              0
   *        0                 0                1              0
   *        0                 1                0              0
   *        0                 1                1              1
   *        1                 0                0              1
   *        1                 0                1              1
   *        1                 1                0              0
   *        1                 1                1              1
   */
  // The boolen codes is B8. Based on
  // http://msdn.microsoft.com/en-us/library/aa932106.aspx, the ROP3 code is
  // 0xB8074A

  ::StretchDIBits(m_hDC, dest_left, dest_top, dest_width, dest_height, 0, 0,
                  width, height, pBitmap->GetBuffer(), (BITMAPINFO*)&bmi,
                  DIB_RGB_COLORS, 0xB8074A);

  SelectObject(m_hDC, hOld);
  DeleteObject(hPattern);

  return TRUE;
}

FX_BOOL CGdiDeviceDriver::GetClipBox(FX_RECT* pRect) {
  return ::GetClipBox(m_hDC, (RECT*)pRect);
}

void* CGdiDeviceDriver::GetPlatformSurface() const {
  return (void*)m_hDC;
}

void CGdiDeviceDriver::DrawLine(FX_FLOAT x1,
                                FX_FLOAT y1,
                                FX_FLOAT x2,
                                FX_FLOAT y2,
                                const CFX_Matrix* pMatrix) {
  bool bStartOutOfBounds = x1 < 0 || x1 > m_Width || y1 < 0 || y1 > m_Height;
  bool bEndOutOfBounds = x2 < 0 || x2 > m_Width || y2 < 0 || y2 > m_Height;
  if (bStartOutOfBounds & bEndOutOfBounds)
    return;

  if (bStartOutOfBounds || bEndOutOfBounds) {
    FX_FLOAT x[2];
    FX_FLOAT y[2];
    int np;
#ifdef _SKIA_SUPPORT_
    // TODO(caryclark) temporary replacement of antigrain in line function
    // to permit removing antigrain altogether
    rect_base rect = {0.0f, 0.0f, (FX_FLOAT)(m_Width), (FX_FLOAT)(m_Height)};
    np = clip_liang_barsky(x1, y1, x2, y2, rect, x, y);
#else
    agg::rect_base<FX_FLOAT> rect(0.0f, 0.0f, (FX_FLOAT)(m_Width),
                                  (FX_FLOAT)(m_Height));
    np = agg::clip_liang_barsky<FX_FLOAT>(x1, y1, x2, y2, rect, x, y);
#endif
    if (np == 0)
      return;

    if (np == 1) {
      x2 = x[0];
      y2 = y[0];
    } else {
      ASSERT(np == 2);
      x1 = x[0];
      y1 = y[0];
      x2 = x[1];
      y2 = y[1];
    }
  }
  if (pMatrix) {
    pMatrix->Transform(x1, y1);
    pMatrix->Transform(x2, y2);
  }
  MoveToEx(m_hDC, FXSYS_round(x1), FXSYS_round(y1), nullptr);
  LineTo(m_hDC, FXSYS_round(x2), FXSYS_round(y2));
}

FX_BOOL CGdiDeviceDriver::DrawPath(const CFX_PathData* pPathData,
                                   const CFX_Matrix* pMatrix,
                                   const CFX_GraphStateData* pGraphState,
                                   uint32_t fill_color,
                                   uint32_t stroke_color,
                                   int fill_mode,
                                   int blend_type) {
  if (blend_type != FXDIB_BLEND_NORMAL)
    return FALSE;

  CWin32Platform* pPlatform =
      (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
  if (!(pGraphState || stroke_color == 0) &&
      !pPlatform->m_GdiplusExt.IsAvailable()) {
    CFX_FloatRect bbox_f = pPathData->GetBoundingBox();
    if (pMatrix) {
      bbox_f.Transform(pMatrix);
    }
    FX_RECT bbox = bbox_f.GetInnerRect();
    if (bbox.Width() <= 0) {
      return DrawCosmeticLine(
          (FX_FLOAT)(bbox.left), (FX_FLOAT)(bbox.top), (FX_FLOAT)(bbox.left),
          (FX_FLOAT)(bbox.bottom + 1), fill_color, FXDIB_BLEND_NORMAL);
    }
    if (bbox.Height() <= 0) {
      return DrawCosmeticLine((FX_FLOAT)(bbox.left), (FX_FLOAT)(bbox.top),
                              (FX_FLOAT)(bbox.right + 1), (FX_FLOAT)(bbox.top),
                              fill_color, FXDIB_BLEND_NORMAL);
    }
  }
  int fill_alpha = FXARGB_A(fill_color);
  int stroke_alpha = FXARGB_A(stroke_color);
  FX_BOOL bDrawAlpha = (fill_alpha > 0 && fill_alpha < 255) ||
                       (stroke_alpha > 0 && stroke_alpha < 255 && pGraphState);
  if (!pPlatform->m_GdiplusExt.IsAvailable() && bDrawAlpha)
    return FALSE;

  if (pPlatform->m_GdiplusExt.IsAvailable()) {
    if (bDrawAlpha ||
        ((m_DeviceClass != FXDC_PRINTER && !(fill_mode & FXFILL_FULLCOVER)) ||
         (pGraphState && pGraphState->m_DashCount))) {
      if (!((!pMatrix || MatrixNoScaled(pMatrix)) && pGraphState &&
            pGraphState->m_LineWidth == 1.f &&
            (pPathData->GetPointCount() == 5 ||
             pPathData->GetPointCount() == 4) &&
            pPathData->IsRect())) {
        if (pPlatform->m_GdiplusExt.DrawPath(m_hDC, pPathData, pMatrix,
                                             pGraphState, fill_color,
                                             stroke_color, fill_mode)) {
          return TRUE;
        }
      }
    }
  }
  int old_fill_mode = fill_mode;
  fill_mode &= 3;
  HPEN hPen = nullptr;
  HBRUSH hBrush = nullptr;
  if (pGraphState && stroke_alpha) {
    SetMiterLimit(m_hDC, pGraphState->m_MiterLimit, nullptr);
    hPen = CreatePen(pGraphState, pMatrix, stroke_color);
    hPen = (HPEN)SelectObject(m_hDC, hPen);
  }
  if (fill_mode && fill_alpha) {
    SetPolyFillMode(m_hDC, fill_mode);
    hBrush = CreateBrush(fill_color);
    hBrush = (HBRUSH)SelectObject(m_hDC, hBrush);
  }
  if (pPathData->GetPointCount() == 2 && pGraphState &&
      pGraphState->m_DashCount) {
    FX_FLOAT x1 = pPathData->GetPointX(0);
    FX_FLOAT y1 = pPathData->GetPointY(0);
    FX_FLOAT x2 = pPathData->GetPointX(1);
    FX_FLOAT y2 = pPathData->GetPointY(1);
    DrawLine(x1, y1, x2, y2, pMatrix);
  } else {
    SetPathToDC(m_hDC, pPathData, pMatrix);
    if (pGraphState && stroke_alpha) {
      if (fill_mode && fill_alpha) {
        if (old_fill_mode & FX_FILL_TEXT_MODE) {
          StrokeAndFillPath(m_hDC);
        } else {
          FillPath(m_hDC);
          SetPathToDC(m_hDC, pPathData, pMatrix);
          StrokePath(m_hDC);
        }
      } else {
        StrokePath(m_hDC);
      }
    } else if (fill_mode && fill_alpha) {
      FillPath(m_hDC);
    }
  }
  if (hPen) {
    hPen = (HPEN)SelectObject(m_hDC, hPen);
    DeleteObject(hPen);
  }
  if (hBrush) {
    hBrush = (HBRUSH)SelectObject(m_hDC, hBrush);
    DeleteObject(hBrush);
  }
  return TRUE;
}

FX_BOOL CGdiDeviceDriver::FillRectWithBlend(const FX_RECT* pRect,
                                            uint32_t fill_color,
                                            int blend_type) {
  if (blend_type != FXDIB_BLEND_NORMAL)
    return FALSE;

  int alpha;
  FX_COLORREF rgb;
  ArgbDecode(fill_color, alpha, rgb);
  if (alpha == 0)
    return TRUE;

  if (alpha < 255)
    return FALSE;

  HBRUSH hBrush = CreateSolidBrush(rgb);
  ::FillRect(m_hDC, (RECT*)pRect, hBrush);
  DeleteObject(hBrush);
  return TRUE;
}

FX_BOOL CGdiDeviceDriver::SetClip_PathFill(const CFX_PathData* pPathData,
                                           const CFX_Matrix* pMatrix,
                                           int fill_mode) {
  if (pPathData->GetPointCount() == 5) {
    CFX_FloatRect rectf;
    if (pPathData->IsRect(pMatrix, &rectf)) {
      FX_RECT rect = rectf.GetOutterRect();
      IntersectClipRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
      return TRUE;
    }
  }
  SetPathToDC(m_hDC, pPathData, pMatrix);
  SetPolyFillMode(m_hDC, fill_mode & 3);
  SelectClipPath(m_hDC, RGN_AND);
  return TRUE;
}

FX_BOOL CGdiDeviceDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pMatrix,
    const CFX_GraphStateData* pGraphState) {
  HPEN hPen = CreatePen(pGraphState, pMatrix, 0xff000000);
  hPen = (HPEN)SelectObject(m_hDC, hPen);
  SetPathToDC(m_hDC, pPathData, pMatrix);
  WidenPath(m_hDC);
  SetPolyFillMode(m_hDC, WINDING);
  FX_BOOL ret = SelectClipPath(m_hDC, RGN_AND);
  hPen = (HPEN)SelectObject(m_hDC, hPen);
  DeleteObject(hPen);
  return ret;
}

FX_BOOL CGdiDeviceDriver::DrawCosmeticLine(FX_FLOAT x1,
                                           FX_FLOAT y1,
                                           FX_FLOAT x2,
                                           FX_FLOAT y2,
                                           uint32_t color,
                                           int blend_type) {
  if (blend_type != FXDIB_BLEND_NORMAL)
    return FALSE;

  int a;
  FX_COLORREF rgb;
  ArgbDecode(color, a, rgb);
  if (a == 0)
    return TRUE;

  HPEN hPen = CreatePen(PS_SOLID, 1, rgb);
  hPen = (HPEN)SelectObject(m_hDC, hPen);
  MoveToEx(m_hDC, FXSYS_round(x1), FXSYS_round(y1), nullptr);
  LineTo(m_hDC, FXSYS_round(x2), FXSYS_round(y2));
  hPen = (HPEN)SelectObject(m_hDC, hPen);
  DeleteObject(hPen);
  return TRUE;
}

CGdiDisplayDriver::CGdiDisplayDriver(HDC hDC)
    : CGdiDeviceDriver(hDC, FXDC_DISPLAY) {
  CWin32Platform* pPlatform =
      (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
  if (pPlatform->m_GdiplusExt.IsAvailable()) {
    m_RenderCaps |= FXRC_ALPHA_PATH | FXRC_ALPHA_IMAGE;
  }
}

CGdiDisplayDriver::~CGdiDisplayDriver() {}

FX_BOOL CGdiDisplayDriver::GetDIBits(CFX_DIBitmap* pBitmap, int left, int top) {
  FX_BOOL ret = FALSE;
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  HBITMAP hbmp = CreateCompatibleBitmap(m_hDC, width, height);
  HDC hDCMemory = CreateCompatibleDC(m_hDC);
  HBITMAP holdbmp = (HBITMAP)SelectObject(hDCMemory, hbmp);
  BitBlt(hDCMemory, 0, 0, width, height, m_hDC, left, top, SRCCOPY);
  SelectObject(hDCMemory, holdbmp);
  BITMAPINFO bmi;
  FXSYS_memset(&bmi, 0, sizeof bmi);
  bmi.bmiHeader.biSize = sizeof bmi.bmiHeader;
  bmi.bmiHeader.biBitCount = pBitmap->GetBPP();
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biWidth = width;
  if (pBitmap->GetBPP() > 8 && !pBitmap->IsCmykImage()) {
    ret = ::GetDIBits(hDCMemory, hbmp, 0, height, pBitmap->GetBuffer(), &bmi,
                      DIB_RGB_COLORS) == height;
  } else {
    CFX_DIBitmap bitmap;
    if (bitmap.Create(width, height, FXDIB_Rgb)) {
      bmi.bmiHeader.biBitCount = 24;
      ::GetDIBits(hDCMemory, hbmp, 0, height, bitmap.GetBuffer(), &bmi,
                  DIB_RGB_COLORS);
      ret = pBitmap->TransferBitmap(0, 0, width, height, &bitmap, 0, 0);
    } else {
      ret = FALSE;
    }
  }
  if (pBitmap->HasAlpha() && ret)
    pBitmap->LoadChannel(FXDIB_Alpha, 0xff);

  DeleteObject(hbmp);
  DeleteObject(hDCMemory);
  return ret;
}

FX_BOOL CGdiDisplayDriver::SetDIBits(const CFX_DIBSource* pSource,
                                     uint32_t color,
                                     const FX_RECT* pSrcRect,
                                     int left,
                                     int top,
                                     int blend_type) {
  ASSERT(blend_type == FXDIB_BLEND_NORMAL);
  if (pSource->IsAlphaMask()) {
    int width = pSource->GetWidth(), height = pSource->GetHeight();
    int alpha = FXARGB_A(color);
    if (pSource->GetBPP() != 1 || alpha != 255) {
      CFX_DIBitmap background;
      if (!background.Create(width, height, FXDIB_Rgb32) ||
          !GetDIBits(&background, left, top) ||
          !background.CompositeMask(0, 0, width, height, pSource, color, 0, 0,
                                    FXDIB_BLEND_NORMAL, nullptr, FALSE, 0,
                                    nullptr)) {
        return FALSE;
      }
      FX_RECT src_rect(0, 0, width, height);
      return SetDIBits(&background, 0, &src_rect, left, top,
                       FXDIB_BLEND_NORMAL);
    }
    FX_RECT clip_rect(left, top, left + pSrcRect->Width(),
                      top + pSrcRect->Height());
    return StretchDIBits(pSource, color, left - pSrcRect->left,
                         top - pSrcRect->top, width, height, &clip_rect, 0,
                         FXDIB_BLEND_NORMAL);
  }
  int width = pSrcRect->Width(), height = pSrcRect->Height();
  if (pSource->HasAlpha()) {
    CFX_DIBitmap bitmap;
    if (!bitmap.Create(width, height, FXDIB_Rgb) ||
        !GetDIBits(&bitmap, left, top) ||
        !bitmap.CompositeBitmap(0, 0, width, height, pSource, pSrcRect->left,
                                pSrcRect->top, FXDIB_BLEND_NORMAL, nullptr,
                                FALSE, nullptr)) {
      return FALSE;
    }
    FX_RECT src_rect(0, 0, width, height);
    return SetDIBits(&bitmap, 0, &src_rect, left, top, FXDIB_BLEND_NORMAL);
  }
  CFX_DIBExtractor temp(pSource);
  CFX_DIBitmap* pBitmap = temp;
  if (!pBitmap)
    return FALSE;
  return GDI_SetDIBits(pBitmap, pSrcRect, left, top);
}

FX_BOOL CGdiDisplayDriver::UseFoxitStretchEngine(const CFX_DIBSource* pSource,
                                                 uint32_t color,
                                                 int dest_left,
                                                 int dest_top,
                                                 int dest_width,
                                                 int dest_height,
                                                 const FX_RECT* pClipRect,
                                                 int render_flags) {
  FX_RECT bitmap_clip = *pClipRect;
  if (dest_width < 0)
    dest_left += dest_width;

  if (dest_height < 0)
    dest_top += dest_height;

  bitmap_clip.Offset(-dest_left, -dest_top);
  std::unique_ptr<CFX_DIBitmap> pStretched(
      pSource->StretchTo(dest_width, dest_height, render_flags, &bitmap_clip));
  if (!pStretched)
    return TRUE;

  FX_RECT src_rect(0, 0, pStretched->GetWidth(), pStretched->GetHeight());
  return SetDIBits(pStretched.get(), color, &src_rect, pClipRect->left,
                   pClipRect->top, FXDIB_BLEND_NORMAL);
}

FX_BOOL CGdiDisplayDriver::StretchDIBits(const CFX_DIBSource* pSource,
                                         uint32_t color,
                                         int dest_left,
                                         int dest_top,
                                         int dest_width,
                                         int dest_height,
                                         const FX_RECT* pClipRect,
                                         uint32_t flags,
                                         int blend_type) {
  ASSERT(pSource && pClipRect);
  if (flags || dest_width > 10000 || dest_width < -10000 ||
      dest_height > 10000 || dest_height < -10000) {
    return UseFoxitStretchEngine(pSource, color, dest_left, dest_top,
                                 dest_width, dest_height, pClipRect, flags);
  }
  if (pSource->IsAlphaMask()) {
    FX_RECT image_rect;
    image_rect.left = dest_width > 0 ? dest_left : dest_left + dest_width;
    image_rect.right = dest_width > 0 ? dest_left + dest_width : dest_left;
    image_rect.top = dest_height > 0 ? dest_top : dest_top + dest_height;
    image_rect.bottom = dest_height > 0 ? dest_top + dest_height : dest_top;
    FX_RECT clip_rect = image_rect;
    clip_rect.Intersect(*pClipRect);
    clip_rect.Offset(-image_rect.left, -image_rect.top);
    int clip_width = clip_rect.Width(), clip_height = clip_rect.Height();
    std::unique_ptr<CFX_DIBitmap> pStretched(
        pSource->StretchTo(dest_width, dest_height, flags, &clip_rect));
    if (!pStretched)
      return TRUE;

    CFX_DIBitmap background;
    if (!background.Create(clip_width, clip_height, FXDIB_Rgb32) ||
        !GetDIBits(&background, image_rect.left + clip_rect.left,
                   image_rect.top + clip_rect.top) ||
        !background.CompositeMask(
            0, 0, clip_width, clip_height, pStretched.get(), color, 0, 0,
            FXDIB_BLEND_NORMAL, nullptr, FALSE, 0, nullptr)) {
      return FALSE;
    }

    FX_RECT src_rect(0, 0, clip_width, clip_height);
    return SetDIBits(&background, 0, &src_rect,
                     image_rect.left + clip_rect.left,
                     image_rect.top + clip_rect.top, FXDIB_BLEND_NORMAL);
  }
  if (pSource->HasAlpha()) {
    CWin32Platform* pPlatform =
        (CWin32Platform*)CFX_GEModule::Get()->GetPlatformData();
    if (pPlatform->m_GdiplusExt.IsAvailable() && !pSource->IsCmykImage()) {
      CFX_DIBExtractor temp(pSource);
      CFX_DIBitmap* pBitmap = temp;
      if (!pBitmap)
        return FALSE;
      return pPlatform->m_GdiplusExt.StretchDIBits(
          m_hDC, pBitmap, dest_left, dest_top, dest_width, dest_height,
          pClipRect, flags);
    }
    return UseFoxitStretchEngine(pSource, color, dest_left, dest_top,
                                 dest_width, dest_height, pClipRect, flags);
  }
  CFX_DIBExtractor temp(pSource);
  CFX_DIBitmap* pBitmap = temp;
  if (!pBitmap)
    return FALSE;
  return GDI_StretchDIBits(pBitmap, dest_left, dest_top, dest_width,
                           dest_height, flags);
}

FX_BOOL CGdiDisplayDriver::StartDIBits(const CFX_DIBSource* pBitmap,
                                       int bitmap_alpha,
                                       uint32_t color,
                                       const CFX_Matrix* pMatrix,
                                       uint32_t render_flags,
                                       void*& handle,
                                       int blend_type) {
  return FALSE;
}

CFX_WindowsDevice::CFX_WindowsDevice(HDC hDC) {
  SetDeviceDriver(CreateDriver(hDC));
}

CFX_WindowsDevice::~CFX_WindowsDevice() {}

HDC CFX_WindowsDevice::GetDC() const {
  IFX_RenderDeviceDriver* pRDD = GetDeviceDriver();
  return pRDD ? reinterpret_cast<HDC>(pRDD->GetPlatformSurface()) : nullptr;
}

// static
IFX_RenderDeviceDriver* CFX_WindowsDevice::CreateDriver(HDC hDC) {
  int device_type = ::GetDeviceCaps(hDC, TECHNOLOGY);
  int obj_type = ::GetObjectType(hDC);
  bool use_printer = device_type == DT_RASPRINTER ||
                     device_type == DT_PLOTTER || obj_type == OBJ_ENHMETADC;
  if (use_printer)
    return new CGdiPrinterDriver(hDC);
  return new CGdiDisplayDriver(hDC);
}

CFX_WinBitmapDevice::CFX_WinBitmapDevice(int width,
                                         int height,
                                         FXDIB_Format format) {
  BITMAPINFOHEADER bmih;
  FXSYS_memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
  bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmih.biBitCount = format & 0xff;
  bmih.biHeight = -height;
  bmih.biPlanes = 1;
  bmih.biWidth = width;
  void* pBufferPtr;
  m_hBitmap = CreateDIBSection(nullptr, reinterpret_cast<BITMAPINFO*>(&bmih),
                               DIB_RGB_COLORS, &pBufferPtr, nullptr, 0);
  if (!m_hBitmap)
    return;

  uint8_t* pBuffer = static_cast<uint8_t*>(pBufferPtr);
  CFX_DIBitmap* pBitmap = new CFX_DIBitmap;
  pBitmap->Create(width, height, format, pBuffer);
  SetBitmap(pBitmap);
  m_hDC = ::CreateCompatibleDC(nullptr);
  m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);
  IFX_RenderDeviceDriver* pDriver = new CGdiDisplayDriver(m_hDC);
  SetDeviceDriver(pDriver);
}

CFX_WinBitmapDevice::~CFX_WinBitmapDevice() {
  if (m_hDC) {
    SelectObject(m_hDC, m_hOldBitmap);
    DeleteDC(m_hDC);
  }
  if (m_hBitmap)
    DeleteObject(m_hBitmap);
  delete GetBitmap();
}

#endif  // _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_
