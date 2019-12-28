// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_FGAS_GEFONT_H_
#define XFA_FGAS_FONT_FGAS_GEFONT_H_

#include <map>

#include "xfa/fgas/crt/fgas_utils.h"
#include "xfa/fgas/font/fgas_font.h"

#define FXFONT_SUBST_ITALIC 0x02

class CXFA_PDFFontMgr;

class CFGAS_GEFont {
 public:
  static CFGAS_GEFont* LoadFont(const FX_WCHAR* pszFontFamily,
                                uint32_t dwFontStyles,
                                uint16_t wCodePage,
                                IFGAS_FontMgr* pFontMgr);
  static CFGAS_GEFont* LoadFont(CFX_Font* pExtFont, IFGAS_FontMgr* pFontMgr);
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  static CFGAS_GEFont* LoadFont(const uint8_t* pBuffer,
                                int32_t iLength,
                                IFGAS_FontMgr* pFontMgr);
  static CFGAS_GEFont* LoadFont(IFX_Stream* pFontStream,
                                IFGAS_FontMgr* pFontMgr,
                                FX_BOOL bSaveStream);
#endif

  ~CFGAS_GEFont();

  void Release();
  CFGAS_GEFont* Retain();
  CFGAS_GEFont* Derive(uint32_t dwFontStyles, uint16_t wCodePage = 0);
  void GetFamilyName(CFX_WideString& wsFamily) const;
  uint32_t GetFontStyles() const;
  FX_BOOL GetCharWidth(FX_WCHAR wUnicode,
                       int32_t& iWidth,
                       FX_BOOL bCharCode = FALSE);
  int32_t GetGlyphIndex(FX_WCHAR wUnicode, FX_BOOL bCharCode = FALSE);
  int32_t GetAscent() const;
  int32_t GetDescent() const;
  FX_BOOL GetCharBBox(FX_WCHAR wUnicode,
                      CFX_Rect& bbox,
                      FX_BOOL bCharCode = FALSE);
  FX_BOOL GetBBox(CFX_Rect& bbox);
  int32_t GetItalicAngle() const;
  void Reset();
  CFGAS_GEFont* GetSubstFont(int32_t iGlyphIndex) const;
  CFX_Font* GetDevFont() const { return m_pFont; }
  void SetFontProvider(CXFA_PDFFontMgr* pProvider) { m_pProvider = pProvider; }
#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  void SetLogicalFontStyle(uint32_t dwLogFontStyle) {
    m_bUseLogFontStyle = TRUE;
    m_dwLogFontStyle = dwLogFontStyle;
  }
#endif

 protected:
  explicit CFGAS_GEFont(IFGAS_FontMgr* pFontMgr);
  CFGAS_GEFont(const CFGAS_GEFont& src, uint32_t dwFontStyles);

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  FX_BOOL LoadFontInternal(const FX_WCHAR* pszFontFamily,
                           uint32_t dwFontStyles,
                           uint16_t wCodePage);
  FX_BOOL LoadFontInternal(const uint8_t* pBuffer, int32_t length);
  FX_BOOL LoadFontInternal(IFX_Stream* pFontStream, FX_BOOL bSaveStream);
#endif
  FX_BOOL LoadFontInternal(CFX_Font* pExtFont);
  FX_BOOL InitFont();
  FX_BOOL GetCharBBoxInternal(FX_WCHAR wUnicode,
                              CFX_Rect& bbox,
                              FX_BOOL bRecursive,
                              FX_BOOL bCharCode = FALSE);
  FX_BOOL GetCharWidthInternal(FX_WCHAR wUnicode,
                               int32_t& iWidth,
                               FX_BOOL bRecursive,
                               FX_BOOL bCharCode = FALSE);
  int32_t GetGlyphIndex(FX_WCHAR wUnicode,
                        FX_BOOL bRecursive,
                        CFGAS_GEFont** ppFont,
                        FX_BOOL bCharCode = FALSE);

#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  FX_BOOL m_bUseLogFontStyle;
  uint32_t m_dwLogFontStyle;
#endif
  CFX_Font* m_pFont;
  IFGAS_FontMgr* m_pFontMgr;
  int32_t m_iRefCount;
  FX_BOOL m_bExtFont;
  IFX_Stream* m_pStream;
  IFX_FileRead* m_pFileRead;
  CFX_UnicodeEncoding* m_pFontEncoding;
  CFX_WordDiscreteArray* m_pCharWidthMap;
  CFX_MassArrayTemplate<CFX_Rect>* m_pRectArray;
  CFX_MapPtrToPtr* m_pBBoxMap;
  CXFA_PDFFontMgr* m_pProvider;
  CFX_ArrayTemplate<CFGAS_GEFont*> m_SubstFonts;
  std::map<FX_WCHAR, CFGAS_GEFont*> m_FontMapper;
};

#endif  // XFA_FGAS_FONT_FGAS_GEFONT_H_
