// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_FGAS_FONT_H_
#define XFA_FGAS_FONT_FGAS_FONT_H_

#include "core/fxge/include/fx_font.h"
#include "xfa/fgas/crt/fgas_stream.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#include "xfa/fgas/crt/fgas_memory.h"
#include "xfa/fgas/crt/fgas_utils.h"
#endif  // _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_

class CFX_FontSourceEnum_File;
class CFGAS_GEFont;
class CXFA_PDFFontMgr;
class IFGAS_FontMgr;

#define FX_FONTSTYLE_Normal 0x00
#define FX_FONTSTYLE_FixedPitch 0x01
#define FX_FONTSTYLE_Serif 0x02
#define FX_FONTSTYLE_Symbolic 0x04
#define FX_FONTSTYLE_Script 0x08
#define FX_FONTSTYLE_Italic 0x40
#define FX_FONTSTYLE_Bold 0x40000
#define FX_FONTSTYLE_BoldItalic (FX_FONTSTYLE_Bold | FX_FONTSTYLE_Italic)
#define FX_FONTSTYLE_ExactMatch 0x80000000

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
struct FX_FONTMATCHPARAMS {
  const FX_WCHAR* pwsFamily;
  uint32_t dwFontStyles;
  uint32_t dwUSB;
  uint32_t dwMatchFlags;
  FX_WCHAR wUnicode;
  uint16_t wCodePage;
};
typedef FX_FONTMATCHPARAMS* FX_LPFONTMATCHPARAMS;
typedef FX_FONTMATCHPARAMS const* FX_LPCFONTMATCHPARAMS;

struct FX_FONTSIGNATURE {
  uint32_t fsUsb[4];
  uint32_t fsCsb[2];
};
inline bool operator==(const FX_FONTSIGNATURE& left,
                       const FX_FONTSIGNATURE& right) {
  return left.fsUsb[0] == right.fsUsb[0] && left.fsUsb[1] == right.fsUsb[1] &&
         left.fsUsb[2] == right.fsUsb[2] && left.fsUsb[3] == right.fsUsb[3] &&
         left.fsCsb[0] == right.fsCsb[0] && left.fsCsb[1] == right.fsCsb[1];
}

struct FX_FONTDESCRIPTOR {
  FX_WCHAR wsFontFace[32];
  uint32_t dwFontStyles;
  uint8_t uCharSet;
  FX_FONTSIGNATURE FontSignature;
};

typedef CFX_MassArrayTemplate<FX_FONTDESCRIPTOR> CFX_FontDescriptors;
inline bool operator==(const FX_FONTDESCRIPTOR& left,
                       const FX_FONTDESCRIPTOR& right) {
  return left.uCharSet == right.uCharSet &&
         left.dwFontStyles == right.dwFontStyles &&
         left.FontSignature == right.FontSignature &&
         FXSYS_wcscmp(left.wsFontFace, right.wsFontFace) == 0;
}

#define FX_FONTMATCHPARA_MacthStyle 0x01
#define FX_FONTMATCHPARA_MacthFamily 0x02
#define FX_FONTMATCHPARA_MacthUnicode 0x04
typedef void (*FX_LPEnumAllFonts)(CFX_FontDescriptors& fonts,
                                  const FX_WCHAR* pwsFaceName,
                                  FX_WCHAR wUnicode);
FX_LPEnumAllFonts FX_GetDefFontEnumerator();
typedef FX_FONTDESCRIPTOR const* (*FX_LPMatchFont)(
    FX_LPFONTMATCHPARAMS pParams,
    const CFX_FontDescriptors& fonts);
FX_LPMatchFont FX_GetDefFontMatchor();
class IFGAS_FontMgr {
 public:
  static IFGAS_FontMgr* Create(FX_LPEnumAllFonts pEnumerator);
  virtual ~IFGAS_FontMgr() {}
  virtual void Release() = 0;
  virtual CFGAS_GEFont* GetDefFontByCodePage(
      uint16_t wCodePage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByCharset(
      uint8_t nCharset,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByUnicode(
      FX_WCHAR wUnicode,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByLanguage(
      uint16_t wLanguage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* LoadFont(const FX_WCHAR* pszFontFamily,
                                 uint32_t dwFontStyles,
                                 uint16_t wCodePage = 0xFFFF) = 0;
  virtual CFGAS_GEFont* LoadFont(const uint8_t* pBuffer, int32_t iLength) = 0;
  virtual CFGAS_GEFont* LoadFont(IFX_Stream* pFontStream,
                                 const FX_WCHAR* pszFontAlias = nullptr,
                                 uint32_t dwFontStyles = 0,
                                 uint16_t wCodePage = 0,
                                 FX_BOOL bSaveStream = FALSE) = 0;
  virtual CFGAS_GEFont* LoadFont(CFGAS_GEFont* pSrcFont,
                                 uint32_t dwFontStyles,
                                 uint16_t wCodePage = 0xFFFF) = 0;
  virtual void ClearFontCache() = 0;
  virtual void RemoveFont(CFGAS_GEFont* pFont) = 0;
};

#else   //  _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_

class IFGAS_FontMgr {
 public:
  static IFGAS_FontMgr* Create(CFX_FontSourceEnum_File* pFontEnum);
  virtual ~IFGAS_FontMgr() {}
  virtual void Release() = 0;
  virtual CFGAS_GEFont* GetDefFontByCodePage(
      uint16_t wCodePage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByCharset(
      uint8_t nCharset,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByUnicode(
      FX_WCHAR wUnicode,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetDefFontByLanguage(
      uint16_t wLanguage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetFontByCodePage(
      uint16_t wCodePage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  inline CFGAS_GEFont* LoadFont(const FX_WCHAR* pszFontFamily,
                                uint32_t dwFontStyles,
                                uint16_t wCodePage) {
    return GetFontByCodePage(wCodePage, dwFontStyles, pszFontFamily);
  }
  virtual CFGAS_GEFont* GetFontByCharset(
      uint8_t nCharset,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetFontByUnicode(
      FX_WCHAR wUnicode,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual CFGAS_GEFont* GetFontByLanguage(
      uint16_t wLanguage,
      uint32_t dwFontStyles,
      const FX_WCHAR* pszFontFamily = nullptr) = 0;
  virtual void ClearFontCache() = 0;
  virtual void RemoveFont(CFGAS_GEFont* pFont) = 0;
};
#endif  //  _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_

#endif  // XFA_FGAS_FONT_FGAS_FONT_H_
