// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/fgas_stdfontmgr.h"

#include "core/fxcrt/include/fx_stream.h"
#include "core/fxge/include/fx_ge.h"
#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/font/fgas_fontutils.h"
#include "xfa/fgas/font/fgas_gefont.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_

IFGAS_FontMgr* IFGAS_FontMgr::Create(FX_LPEnumAllFonts pEnumerator) {
  return new CFGAS_StdFontMgrImp(pEnumerator);
}

CFGAS_StdFontMgrImp::CFGAS_StdFontMgrImp(FX_LPEnumAllFonts pEnumerator)
    : m_pEnumerator(pEnumerator),
      m_FontFaces(),
      m_CPFonts(8),
      m_FamilyFonts(16),
      m_UnicodeFonts(16),
      m_BufferFonts(4),
      m_StreamFonts(4),
      m_DeriveFonts(4) {
  if (m_pEnumerator) {
    m_pEnumerator(m_FontFaces, nullptr, 0xFEFF);
  }
}

CFGAS_StdFontMgrImp::~CFGAS_StdFontMgrImp() {
  m_FontFaces.RemoveAll();
  m_CPFonts.RemoveAll();
  m_FamilyFonts.RemoveAll();
  m_UnicodeFonts.RemoveAll();
  m_BufferFonts.RemoveAll();
  m_StreamFonts.RemoveAll();
  m_DeriveFonts.RemoveAll();
  for (int32_t i = m_Fonts.GetUpperBound(); i >= 0; i--)
    m_Fonts[i]->Release();
}

void CFGAS_StdFontMgrImp::Release() {
  delete this;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::GetDefFontByCodePage(
    uint16_t wCodePage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  uint32_t dwHash = FGAS_GetFontHashCode(wCodePage, dwFontStyles);
  CFGAS_GEFont* pFont = nullptr;
  if (m_CPFonts.Lookup((void*)(uintptr_t)dwHash, (void*&)pFont)) {
    return pFont ? LoadFont(pFont, dwFontStyles, wCodePage) : nullptr;
  }
  FX_FONTDESCRIPTOR const* pFD =
      FindFont(pszFontFamily, dwFontStyles, TRUE, wCodePage);
  if (!pFD)
    pFD = FindFont(nullptr, dwFontStyles, TRUE, wCodePage);
  if (!pFD)
    pFD = FindFont(nullptr, dwFontStyles, FALSE, wCodePage);
  if (!pFD)
    return nullptr;

  pFont =
      CFGAS_GEFont::LoadFont(pFD->wsFontFace, dwFontStyles, wCodePage, this);
  if (pFont) {
    m_Fonts.Add(pFont);
    m_CPFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    dwHash = FGAS_GetFontFamilyHash(pFD->wsFontFace, dwFontStyles, wCodePage);
    m_FamilyFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    return LoadFont(pFont, dwFontStyles, wCodePage);
  }
  return nullptr;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::GetDefFontByCharset(
    uint8_t nCharset,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return GetDefFontByCodePage(FX_GetCodePageFromCharset(nCharset), dwFontStyles,
                              pszFontFamily);
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::GetDefFontByUnicode(
    FX_WCHAR wUnicode,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  const FGAS_FONTUSB* pRet = FGAS_GetUnicodeBitField(wUnicode);
  if (pRet->wBitField == 999)
    return nullptr;

  uint32_t dwHash =
      FGAS_GetFontFamilyHash(pszFontFamily, dwFontStyles, pRet->wBitField);
  CFGAS_GEFont* pFont = nullptr;
  if (m_UnicodeFonts.Lookup((void*)(uintptr_t)dwHash, (void*&)pFont))
    return pFont ? LoadFont(pFont, dwFontStyles, pRet->wCodePage) : nullptr;

  FX_FONTDESCRIPTOR const* pFD =
      FindFont(pszFontFamily, dwFontStyles, FALSE, pRet->wCodePage,
               pRet->wBitField, wUnicode);
  if (!pFD && pszFontFamily) {
    pFD = FindFont(nullptr, dwFontStyles, FALSE, pRet->wCodePage,
                   pRet->wBitField, wUnicode);
  }
  if (!pFD)
    return nullptr;

  uint16_t wCodePage = FX_GetCodePageFromCharset(pFD->uCharSet);
  const FX_WCHAR* pFontFace = pFD->wsFontFace;
  pFont = CFGAS_GEFont::LoadFont(pFontFace, dwFontStyles, wCodePage, this);
  if (pFont) {
    m_Fonts.Add(pFont);
    m_UnicodeFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    dwHash = FGAS_GetFontHashCode(wCodePage, dwFontStyles);
    m_CPFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    dwHash = FGAS_GetFontFamilyHash(pFontFace, dwFontStyles, wCodePage);
    m_FamilyFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    return LoadFont(pFont, dwFontStyles, wCodePage);
  }
  return nullptr;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::GetDefFontByLanguage(
    uint16_t wLanguage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return GetDefFontByCodePage(FX_GetDefCodePageByLanguage(wLanguage),
                              dwFontStyles, pszFontFamily);
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::LoadFont(const FX_WCHAR* pszFontFamily,
                                            uint32_t dwFontStyles,
                                            uint16_t wCodePage) {
  uint32_t dwHash =
      FGAS_GetFontFamilyHash(pszFontFamily, dwFontStyles, wCodePage);
  CFGAS_GEFont* pFont = nullptr;
  if (m_FamilyFonts.Lookup((void*)(uintptr_t)dwHash, (void*&)pFont)) {
    return pFont ? LoadFont(pFont, dwFontStyles, wCodePage) : nullptr;
  }
  FX_FONTDESCRIPTOR const* pFD =
      FindFont(pszFontFamily, dwFontStyles, TRUE, wCodePage);
  if (!pFD)
    pFD = FindFont(pszFontFamily, dwFontStyles, FALSE, wCodePage);
  if (!pFD)
    return nullptr;

  if (wCodePage == 0xFFFF) {
    wCodePage = FX_GetCodePageFromCharset(pFD->uCharSet);
  }
  pFont =
      CFGAS_GEFont::LoadFont(pFD->wsFontFace, dwFontStyles, wCodePage, this);
  if (pFont) {
    m_Fonts.Add(pFont);
    m_FamilyFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    dwHash = FGAS_GetFontHashCode(wCodePage, dwFontStyles);
    m_CPFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    return LoadFont(pFont, dwFontStyles, wCodePage);
  }
  return nullptr;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::LoadFont(const uint8_t* pBuffer,
                                            int32_t iLength) {
  ASSERT(pBuffer && iLength > 0);
  CFGAS_GEFont* pFont = nullptr;
  if (m_BufferFonts.Lookup((void*)pBuffer, (void*&)pFont)) {
    if (pFont) {
      return pFont->Retain();
    }
  }
  pFont = CFGAS_GEFont::LoadFont(pBuffer, iLength, this);
  if (pFont) {
    m_Fonts.Add(pFont);
    m_BufferFonts.SetAt((void*)pBuffer, pFont);
    return pFont->Retain();
  }
  return nullptr;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::LoadFont(IFX_Stream* pFontStream,
                                            const FX_WCHAR* pszFontAlias,
                                            uint32_t dwFontStyles,
                                            uint16_t wCodePage,
                                            FX_BOOL bSaveStream) {
  ASSERT(pFontStream && pFontStream->GetLength() > 0);
  CFGAS_GEFont* pFont = nullptr;
  if (m_StreamFonts.Lookup((void*)pFontStream, (void*&)pFont)) {
    if (pFont) {
      if (pszFontAlias) {
        uint32_t dwHash =
            FGAS_GetFontFamilyHash(pszFontAlias, dwFontStyles, wCodePage);
        m_FamilyFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
      }
      return LoadFont(pFont, dwFontStyles, wCodePage);
    }
  }
  pFont = CFGAS_GEFont::LoadFont(pFontStream, this, bSaveStream);
  if (pFont) {
    m_Fonts.Add(pFont);
    m_StreamFonts.SetAt((void*)pFontStream, (void*)pFont);
    if (pszFontAlias) {
      uint32_t dwHash =
          FGAS_GetFontFamilyHash(pszFontAlias, dwFontStyles, wCodePage);
      m_FamilyFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    }
    return LoadFont(pFont, dwFontStyles, wCodePage);
  }
  return nullptr;
}

CFGAS_GEFont* CFGAS_StdFontMgrImp::LoadFont(CFGAS_GEFont* pSrcFont,
                                            uint32_t dwFontStyles,
                                            uint16_t wCodePage) {
  ASSERT(pSrcFont);
  if (pSrcFont->GetFontStyles() == dwFontStyles) {
    return pSrcFont->Retain();
  }
  void* buffer[3] = {pSrcFont, (void*)(uintptr_t)dwFontStyles,
                     (void*)(uintptr_t)wCodePage};
  uint32_t dwHash = FX_HashCode_GetA(
      CFX_ByteStringC((uint8_t*)buffer, sizeof(buffer)), false);
  CFGAS_GEFont* pFont = nullptr;
  if (m_DeriveFonts.GetCount() > 0) {
    m_DeriveFonts.Lookup((void*)(uintptr_t)dwHash, (void*&)pFont);
    if (pFont) {
      return pFont->Retain();
    }
  }
  pFont = pSrcFont->Derive(dwFontStyles, wCodePage);
  if (pFont) {
    m_DeriveFonts.SetAt((void*)(uintptr_t)dwHash, (void*)pFont);
    int32_t index = m_Fonts.Find(pFont);
    if (index < 0) {
      m_Fonts.Add(pFont);
      pFont->Retain();
    }
    return pFont;
  }
  return nullptr;
}

void CFGAS_StdFontMgrImp::ClearFontCache() {
  for (int32_t i = 0; i < m_Fonts.GetSize(); i++)
    m_Fonts[i]->Reset();
}

void CFGAS_StdFontMgrImp::RemoveFont(CFX_MapPtrToPtr& fontMap,
                                     CFGAS_GEFont* pFont) {
  FX_POSITION pos = fontMap.GetStartPosition();
  void* pKey;
  void* pFind;
  while (pos) {
    pFind = nullptr;
    fontMap.GetNextAssoc(pos, pKey, pFind);
    if (pFind != (void*)pFont) {
      continue;
    }
    fontMap.RemoveKey(pKey);
    break;
  }
}

void CFGAS_StdFontMgrImp::RemoveFont(CFGAS_GEFont* pFont) {
  RemoveFont(m_CPFonts, pFont);
  RemoveFont(m_FamilyFonts, pFont);
  RemoveFont(m_UnicodeFonts, pFont);
  RemoveFont(m_BufferFonts, pFont);
  RemoveFont(m_StreamFonts, pFont);
  RemoveFont(m_DeriveFonts, pFont);
  int32_t iFind = m_Fonts.Find(pFont);
  if (iFind > -1) {
    m_Fonts.RemoveAt(iFind, 1);
  }
}

FX_FONTDESCRIPTOR const* CFGAS_StdFontMgrImp::FindFont(
    const FX_WCHAR* pszFontFamily,
    uint32_t dwFontStyles,
    uint32_t dwMatchFlags,
    uint16_t wCodePage,
    uint32_t dwUSB,
    FX_WCHAR wUnicode) {
  FX_FONTMATCHPARAMS params;
  FXSYS_memset(&params, 0, sizeof(params));
  params.dwUSB = dwUSB;
  params.wUnicode = wUnicode;
  params.wCodePage = wCodePage;
  params.pwsFamily = pszFontFamily;
  params.dwFontStyles = dwFontStyles;
  params.dwMatchFlags = dwMatchFlags;
  FX_FONTDESCRIPTOR const* pDesc = FX_DefFontMatcher(&params, m_FontFaces);
  if (pDesc) {
    return pDesc;
  }
  if (pszFontFamily && m_pEnumerator) {
    CFX_FontDescriptors namedFonts;
    m_pEnumerator(namedFonts, pszFontFamily, wUnicode);
    params.pwsFamily = nullptr;
    pDesc = FX_DefFontMatcher(&params, namedFonts);
    if (!pDesc) {
      return nullptr;
    }
    for (int32_t i = m_FontFaces.GetSize() - 1; i >= 0; i--) {
      FX_FONTDESCRIPTOR const* pMatch = m_FontFaces.GetPtrAt(i);
      if (*pMatch == *pDesc) {
        return pMatch;
      }
    }
    int index = m_FontFaces.Add(*pDesc);
    return m_FontFaces.GetPtrAt(index);
  }
  return nullptr;
}

FX_FONTDESCRIPTOR const* FX_DefFontMatcher(FX_LPFONTMATCHPARAMS pParams,
                                           const CFX_FontDescriptors& fonts) {
  FX_FONTDESCRIPTOR const* pBestFont = nullptr;
  int32_t iBestSimilar = 0;
  FX_BOOL bMatchStyle =
      (pParams->dwMatchFlags & FX_FONTMATCHPARA_MacthStyle) > 0;
  int32_t iCount = fonts.GetSize();
  for (int32_t i = 0; i < iCount; ++i) {
    FX_FONTDESCRIPTOR const* pFont = fonts.GetPtrAt(i);
    if ((pFont->dwFontStyles & FX_FONTSTYLE_BoldItalic) ==
        FX_FONTSTYLE_BoldItalic) {
      continue;
    }
    if (pParams->pwsFamily) {
      if (FXSYS_wcsicmp(pParams->pwsFamily, pFont->wsFontFace)) {
        continue;
      }
      if (pFont->uCharSet == FX_CHARSET_Symbol) {
        return pFont;
      }
    }
    if (pFont->uCharSet == FX_CHARSET_Symbol) {
      continue;
    }
    if (pParams->wCodePage != 0xFFFF) {
      if (FX_GetCodePageFromCharset(pFont->uCharSet) != pParams->wCodePage) {
        continue;
      }
    } else {
      if (pParams->dwUSB < 128) {
        uint32_t dwByte = pParams->dwUSB / 32;
        uint32_t dwUSB = 1 << (pParams->dwUSB % 32);
        if ((pFont->FontSignature.fsUsb[dwByte] & dwUSB) == 0) {
          continue;
        }
      }
    }
    if (bMatchStyle) {
      if ((pFont->dwFontStyles & 0x0F) == (pParams->dwFontStyles & 0x0F))
        return pFont;
      continue;
    }
    if (pParams->pwsFamily) {
      if (FXSYS_wcsicmp(pParams->pwsFamily, pFont->wsFontFace) == 0) {
        return pFont;
      }
    }
    int32_t iSimilarValue = FX_GetSimilarValue(pFont, pParams->dwFontStyles);
    if (iBestSimilar < iSimilarValue) {
      iBestSimilar = iSimilarValue;
      pBestFont = pFont;
    }
  }
  return iBestSimilar < 1 ? nullptr : pBestFont;
}

int32_t FX_GetSimilarValue(FX_FONTDESCRIPTOR const* pFont,
                           uint32_t dwFontStyles) {
  int32_t iValue = 0;
  if ((dwFontStyles & FX_FONTSTYLE_Symbolic) ==
      (pFont->dwFontStyles & FX_FONTSTYLE_Symbolic)) {
    iValue += 64;
  }
  if ((dwFontStyles & FX_FONTSTYLE_FixedPitch) ==
      (pFont->dwFontStyles & FX_FONTSTYLE_FixedPitch)) {
    iValue += 32;
  }
  if ((dwFontStyles & FX_FONTSTYLE_Serif) ==
      (pFont->dwFontStyles & FX_FONTSTYLE_Serif)) {
    iValue += 16;
  }
  if ((dwFontStyles & FX_FONTSTYLE_Script) ==
      (pFont->dwFontStyles & FX_FONTSTYLE_Script)) {
    iValue += 8;
  }
  return iValue;
}

FX_LPMatchFont FX_GetDefFontMatchor() {
  return FX_DefFontMatcher;
}

uint32_t FX_GetGdiFontStyles(const LOGFONTW& lf) {
  uint32_t dwStyles = 0;
  if ((lf.lfPitchAndFamily & 0x03) == FIXED_PITCH) {
    dwStyles |= FX_FONTSTYLE_FixedPitch;
  }
  uint8_t nFamilies = lf.lfPitchAndFamily & 0xF0;
  if (nFamilies == FF_ROMAN) {
    dwStyles |= FX_FONTSTYLE_Serif;
  }
  if (nFamilies == FF_SCRIPT) {
    dwStyles |= FX_FONTSTYLE_Script;
  }
  if (lf.lfCharSet == SYMBOL_CHARSET) {
    dwStyles |= FX_FONTSTYLE_Symbolic;
  }
  return dwStyles;
}

static int32_t CALLBACK FX_GdiFontEnumProc(ENUMLOGFONTEX* lpelfe,
                                           NEWTEXTMETRICEX* lpntme,
                                           DWORD dwFontType,
                                           LPARAM lParam) {
  if (dwFontType != TRUETYPE_FONTTYPE) {
    return 1;
  }
  const LOGFONTW& lf = ((LPENUMLOGFONTEXW)lpelfe)->elfLogFont;
  if (lf.lfFaceName[0] == L'@') {
    return 1;
  }
  FX_FONTDESCRIPTOR* pFont = FX_Alloc(FX_FONTDESCRIPTOR, 1);
  FXSYS_memset(pFont, 0, sizeof(FX_FONTDESCRIPTOR));
  pFont->uCharSet = lf.lfCharSet;
  pFont->dwFontStyles = FX_GetGdiFontStyles(lf);
  FXSYS_wcsncpy(pFont->wsFontFace, (const FX_WCHAR*)lf.lfFaceName, 31);
  pFont->wsFontFace[31] = 0;
  FXSYS_memcpy(&pFont->FontSignature, &lpntme->ntmFontSig,
               sizeof(lpntme->ntmFontSig));
  ((CFX_FontDescriptors*)lParam)->Add(*pFont);
  FX_Free(pFont);
  return 1;
}

static void FX_EnumGdiFonts(CFX_FontDescriptors& fonts,
                            const FX_WCHAR* pwsFaceName,
                            FX_WCHAR wUnicode) {
  HDC hDC = ::GetDC(nullptr);
  LOGFONTW lfFind;
  FXSYS_memset(&lfFind, 0, sizeof(lfFind));
  lfFind.lfCharSet = DEFAULT_CHARSET;
  if (pwsFaceName) {
    FXSYS_wcsncpy(lfFind.lfFaceName, pwsFaceName, 31);
    lfFind.lfFaceName[31] = 0;
  }
  EnumFontFamiliesExW(hDC, (LPLOGFONTW)&lfFind,
                      (FONTENUMPROCW)FX_GdiFontEnumProc, (LPARAM)&fonts, 0);
  ::ReleaseDC(nullptr, hDC);
}

FX_LPEnumAllFonts FX_GetDefFontEnumerator() {
  return FX_EnumGdiFonts;
}

#else
const FX_CHAR* g_FontFolders[] = {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_LINUX_
    "/usr/share/fonts", "/usr/share/X11/fonts/Type1",
    "/usr/share/X11/fonts/TTF", "/usr/local/share/fonts",
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
    "~/Library/Fonts", "/Library/Fonts", "/System/Library/Fonts",
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_ANDROID_
    "/system/fonts",
#endif
};

CFX_FontDescriptor::CFX_FontDescriptor()
    : m_nFaceIndex(0), m_dwFontStyles(0), m_dwUsb(), m_dwCsb() {}

CFX_FontDescriptor::~CFX_FontDescriptor() {}

CFX_FontSourceEnum_File::CFX_FontSourceEnum_File() {
  for (size_t i = 0; i < FX_ArraySize(g_FontFolders); ++i)
    m_FolderPaths.Add(g_FontFolders[i]);
}

CFX_FontSourceEnum_File::~CFX_FontSourceEnum_File() {}

CFX_ByteString CFX_FontSourceEnum_File::GetNextFile() {
Restart:
  void* pCurHandle =
      m_FolderQueue.GetSize() != 0
          ? m_FolderQueue.GetDataPtr(m_FolderQueue.GetSize() - 1)->pFileHandle
          : nullptr;
  if (!pCurHandle) {
    if (m_FolderPaths.GetSize() < 1) {
      return "";
    }
    pCurHandle =
        FX_OpenFolder(m_FolderPaths[m_FolderPaths.GetSize() - 1].c_str());
    FX_HandleParentPath hpp;
    hpp.pFileHandle = pCurHandle;
    hpp.bsParentPath = m_FolderPaths[m_FolderPaths.GetSize() - 1];
    m_FolderQueue.Add(hpp);
  }
  CFX_ByteString bsName;
  FX_BOOL bFolder;
  CFX_ByteString bsFolderSpearator =
      CFX_ByteString::FromUnicode(CFX_WideString(FX_GetFolderSeparator()));
  while (TRUE) {
    if (!FX_GetNextFile(pCurHandle, bsName, bFolder)) {
      FX_CloseFolder(pCurHandle);
      m_FolderQueue.RemoveAt(m_FolderQueue.GetSize() - 1);
      if (m_FolderQueue.GetSize() == 0) {
        m_FolderPaths.RemoveAt(m_FolderPaths.GetSize() - 1);
        if (m_FolderPaths.GetSize() == 0) {
          return "";
        } else {
          goto Restart;
        }
      }
      pCurHandle =
          m_FolderQueue.GetDataPtr(m_FolderQueue.GetSize() - 1)->pFileHandle;
      continue;
    }
    if (bsName == "." || bsName == "..") {
      continue;
    }
    if (bFolder) {
      FX_HandleParentPath hpp;
      hpp.bsParentPath =
          m_FolderQueue.GetDataPtr(m_FolderQueue.GetSize() - 1)->bsParentPath +
          bsFolderSpearator + bsName;
      hpp.pFileHandle = FX_OpenFolder(hpp.bsParentPath.c_str());
      if (!hpp.pFileHandle) {
        continue;
      }
      m_FolderQueue.Add(hpp);
      pCurHandle = hpp.pFileHandle;
      continue;
    }
    bsName =
        m_FolderQueue.GetDataPtr(m_FolderQueue.GetSize() - 1)->bsParentPath +
        bsFolderSpearator + bsName;
    break;
  }
  return bsName;
}

FX_POSITION CFX_FontSourceEnum_File::GetStartPosition() {
  m_wsNext = GetNextFile().UTF8Decode();
  if (m_wsNext.GetLength() == 0) {
    return (FX_POSITION)0;
  }
  return (FX_POSITION)-1;
}

IFX_FileAccess* CFX_FontSourceEnum_File::GetNext(FX_POSITION& pos) {
  IFX_FileAccess* pAccess = FX_CreateDefaultFileAccess(m_wsNext.AsStringC());
  m_wsNext = GetNextFile().UTF8Decode();
  pos = m_wsNext.GetLength() != 0 ? pAccess : nullptr;
  return pAccess;
}

IFGAS_FontMgr* IFGAS_FontMgr::Create(CFX_FontSourceEnum_File* pFontEnum) {
  if (!pFontEnum)
    return nullptr;

  std::unique_ptr<CFGAS_FontMgrImp> pFontMgr(new CFGAS_FontMgrImp(pFontEnum));
  if (!pFontMgr->EnumFonts())
    return nullptr;
  return pFontMgr.release();
}

CFGAS_FontMgrImp::CFGAS_FontMgrImp(CFX_FontSourceEnum_File* pFontEnum)
    : m_pFontSource(pFontEnum) {}

CFGAS_FontMgrImp::~CFGAS_FontMgrImp() {}

FX_BOOL CFGAS_FontMgrImp::EnumFontsFromFontMapper() {
  CFX_FontMapper* pFontMapper =
      CFX_GEModule::Get()->GetFontMgr()->GetBuiltinMapper();
  if (!pFontMapper)
    return FALSE;

  IFX_SystemFontInfo* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return FALSE;

  pSystemFontInfo->EnumFontList(pFontMapper);
  for (int32_t i = 0; i < pFontMapper->GetFaceSize(); ++i) {
    IFX_FileRead* pFontStream =
        CreateFontStream(pFontMapper, pSystemFontInfo, i);
    if (!pFontStream)
      continue;

    CFX_WideString wsFaceName =
        CFX_WideString::FromLocal(pFontMapper->GetFaceName(i).c_str());
    RegisterFaces(pFontStream, &wsFaceName);
    pFontStream->Release();
  }
  if (m_InstalledFonts.GetSize() == 0)
    return FALSE;

  return TRUE;
}

FX_BOOL CFGAS_FontMgrImp::EnumFontsFromFiles() {
  CFX_GEModule::Get()->GetFontMgr()->InitFTLibrary();
  FX_POSITION pos = m_pFontSource->GetStartPosition();
  IFX_FileAccess* pFontSource = nullptr;
  IFX_FileRead* pFontStream = nullptr;
  while (pos) {
    pFontSource = m_pFontSource->GetNext(pos);
    pFontStream = pFontSource->CreateFileStream(FX_FILEMODE_ReadOnly);
    if (!pFontStream) {
      pFontSource->Release();
      continue;
    }
    RegisterFaces(pFontStream, nullptr);
    pFontStream->Release();
    pFontSource->Release();
  }
  if (m_InstalledFonts.GetSize() == 0)
    return FALSE;
  return TRUE;
}

FX_BOOL CFGAS_FontMgrImp::EnumFonts() {
  if (EnumFontsFromFontMapper())
    return TRUE;
  return EnumFontsFromFiles();
}

void CFGAS_FontMgrImp::Release() {
  for (int32_t i = 0; i < m_InstalledFonts.GetSize(); i++) {
    delete m_InstalledFonts[i];
  }
  FX_POSITION pos = m_Hash2CandidateList.GetStartPosition();
  while (pos) {
    uint32_t dwHash;
    CFX_FontDescriptorInfos* pDescs;
    m_Hash2CandidateList.GetNextAssoc(pos, dwHash, pDescs);
    delete pDescs;
  }
  pos = m_Hash2Fonts.GetStartPosition();
  while (pos) {
    uint32_t dwHash;
    CFX_ArrayTemplate<CFGAS_GEFont*>* pFonts;
    m_Hash2Fonts.GetNextAssoc(pos, dwHash, pFonts);
    delete pFonts;
  }
  m_Hash2Fonts.RemoveAll();
  pos = m_IFXFont2FileRead.GetStartPosition();
  while (pos) {
    CFGAS_GEFont* pFont;
    IFX_FileRead* pFileRead;
    m_IFXFont2FileRead.GetNextAssoc(pos, pFont, pFileRead);
    pFileRead->Release();
  }
  delete this;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetDefFontByCodePage(
    uint16_t wCodePage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return nullptr;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetDefFontByCharset(
    uint8_t nCharset,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return nullptr;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetDefFontByUnicode(
    FX_WCHAR wUnicode,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return nullptr;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetDefFontByLanguage(
    uint16_t wLanguage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return nullptr;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetFontByCodePage(
    uint16_t wCodePage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  CFX_ByteString bsHash;
  bsHash.Format("%d, %d", wCodePage, dwFontStyles);
  bsHash += CFX_WideString(pszFontFamily).UTF8Encode();
  uint32_t dwHash = FX_HashCode_GetA(bsHash.AsStringC(), false);
  CFX_ArrayTemplate<CFGAS_GEFont*>* pFonts = nullptr;
  if (m_Hash2Fonts.Lookup(dwHash, pFonts)) {
    if (!pFonts)
      return nullptr;

    if (pFonts->GetSize() != 0)
      return pFonts->GetAt(0)->Retain();
  }

  if (!pFonts)
    pFonts = new CFX_ArrayTemplate<CFGAS_GEFont*>;

  m_Hash2Fonts.SetAt(dwHash, pFonts);
  CFX_FontDescriptorInfos* sortedFonts = nullptr;
  if (!m_Hash2CandidateList.Lookup(dwHash, sortedFonts)) {
    sortedFonts = new CFX_FontDescriptorInfos;
    MatchFonts(*sortedFonts, wCodePage, dwFontStyles,
               CFX_WideString(pszFontFamily), 0);
    m_Hash2CandidateList.SetAt(dwHash, sortedFonts);
  }
  if (sortedFonts->GetSize() == 0)
    return nullptr;

  CFX_FontDescriptor* pDesc = sortedFonts->GetAt(0).pFont;
  CFGAS_GEFont* pFont =
      LoadFont(pDesc->m_wsFaceName, pDesc->m_nFaceIndex, nullptr);
  if (pFont)
    pFont->SetLogicalFontStyle(dwFontStyles);

  pFonts->Add(pFont);
  return pFont;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetFontByCharset(
    uint8_t nCharset,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return GetFontByCodePage(FX_GetCodePageFromCharset(nCharset), dwFontStyles,
                           pszFontFamily);
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetFontByUnicode(
    FX_WCHAR wUnicode,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  CFGAS_GEFont* pFont = nullptr;
  if (m_FailedUnicodes2Nullptr.Lookup(wUnicode, pFont))
    return nullptr;
  const FGAS_FONTUSB* x = FGAS_GetUnicodeBitField(wUnicode);
  uint16_t wCodePage = x ? x->wCodePage : 0xFFFF;
  uint16_t wBitField = x ? x->wBitField : 0x03E7;
  CFX_ByteString bsHash;
  if (wCodePage == 0xFFFF)
    bsHash.Format("%d, %d, %d", wCodePage, wBitField, dwFontStyles);
  else
    bsHash.Format("%d, %d", wCodePage, dwFontStyles);
  bsHash += CFX_WideString(pszFontFamily).UTF8Encode();
  uint32_t dwHash = FX_HashCode_GetA(bsHash.AsStringC(), false);
  CFX_ArrayTemplate<CFGAS_GEFont*>* pFonts = nullptr;
  if (m_Hash2Fonts.Lookup(dwHash, pFonts)) {
    if (!pFonts)
      return nullptr;
    for (int32_t i = 0; i < pFonts->GetSize(); ++i) {
      if (VerifyUnicode(pFonts->GetAt(i), wUnicode))
        return pFonts->GetAt(i)->Retain();
    }
  }
  if (!pFonts)
    pFonts = new CFX_ArrayTemplate<CFGAS_GEFont*>;
  m_Hash2Fonts.SetAt(dwHash, pFonts);
  CFX_FontDescriptorInfos* sortedFonts = nullptr;
  if (!m_Hash2CandidateList.Lookup(dwHash, sortedFonts)) {
    sortedFonts = new CFX_FontDescriptorInfos;
    MatchFonts(*sortedFonts, wCodePage, dwFontStyles,
               CFX_WideString(pszFontFamily), wUnicode);
    m_Hash2CandidateList.SetAt(dwHash, sortedFonts);
  }
  for (int32_t i = 0; i < sortedFonts->GetSize(); ++i) {
    CFX_FontDescriptor* pDesc = sortedFonts->GetAt(i).pFont;
    if (!VerifyUnicode(pDesc, wUnicode))
      continue;
    pFont = LoadFont(pDesc->m_wsFaceName, pDesc->m_nFaceIndex, nullptr);
    if (!pFont)
      continue;
    pFont->SetLogicalFontStyle(dwFontStyles);
    pFonts->Add(pFont);
    return pFont;
  }
  if (!pszFontFamily)
    m_FailedUnicodes2Nullptr.SetAt(wUnicode, nullptr);
  return nullptr;
}

FX_BOOL CFGAS_FontMgrImp::VerifyUnicode(CFX_FontDescriptor* pDesc,
                                        FX_WCHAR wcUnicode) {
  IFX_FileRead* pFileRead = CreateFontStream(pDesc->m_wsFaceName.UTF8Encode());
  if (!pFileRead)
    return FALSE;
  FXFT_Face pFace = LoadFace(pFileRead, pDesc->m_nFaceIndex);
  FT_Error retCharmap = FXFT_Select_Charmap(pFace, FXFT_ENCODING_UNICODE);
  FT_Error retIndex = FXFT_Get_Char_Index(pFace, wcUnicode);
  pFileRead->Release();
  if (!pFace)
    return FALSE;
  if (FXFT_Get_Face_External_Stream(pFace))
    FXFT_Clear_Face_External_Stream(pFace);
  FXFT_Done_Face(pFace);
  return !retCharmap && retIndex;
}

FX_BOOL CFGAS_FontMgrImp::VerifyUnicode(CFGAS_GEFont* pFont,
                                        FX_WCHAR wcUnicode) {
  if (!pFont)
    return FALSE;

  FXFT_Face pFace = pFont->GetDevFont()->GetFace();
  FXFT_CharMap charmap = FXFT_Get_Face_Charmap(pFace);
  if (FXFT_Select_Charmap(pFace, FXFT_ENCODING_UNICODE) != 0)
    return FALSE;

  if (FXFT_Get_Char_Index(pFace, wcUnicode) == 0) {
    FXFT_Set_Charmap(pFace, charmap);
    return FALSE;
  }
  return TRUE;
}

CFGAS_GEFont* CFGAS_FontMgrImp::GetFontByLanguage(
    uint16_t wLanguage,
    uint32_t dwFontStyles,
    const FX_WCHAR* pszFontFamily) {
  return GetFontByCodePage(FX_GetDefCodePageByLanguage(wLanguage), dwFontStyles,
                           pszFontFamily);
}

CFGAS_GEFont* CFGAS_FontMgrImp::LoadFont(const CFX_WideString& wsFaceName,
                                         int32_t iFaceIndex,
                                         int32_t* pFaceCount) {
  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  CFX_FontMapper* pFontMapper = pFontMgr->GetBuiltinMapper();
  if (!pFontMapper)
    return nullptr;

  IFX_SystemFontInfo* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return nullptr;

  IFX_FileRead* pFontStream = CreateFontStream(wsFaceName.UTF8Encode());
  if (!pFontStream)
    return nullptr;

  if (!LoadFace(pFontStream, 0)) {
    pFontStream->Release();
    return nullptr;
  }

  CFX_Font* pInternalFont = new CFX_Font();
  if (!pInternalFont->LoadFile(pFontStream, iFaceIndex)) {
    pFontStream->Release();
    return nullptr;
  }

  CFGAS_GEFont* pFont = CFGAS_GEFont::LoadFont(pInternalFont, this);
  if (!pFont) {
    pFontStream->Release();
    return nullptr;
  }

  m_IFXFont2FileRead.SetAt(pFont, pFontStream);
  if (pFaceCount)
    *pFaceCount = pFont->GetDevFont()->GetFace()->num_faces;

  return pFont;
}

extern "C" {

unsigned long _ftStreamRead(FXFT_Stream stream,
                            unsigned long offset,
                            unsigned char* buffer,
                            unsigned long count) {
  if (count == 0)
    return 0;

  IFX_FileRead* pFile = (IFX_FileRead*)stream->descriptor.pointer;
  int res = pFile->ReadBlock(buffer, offset, count);
  if (res)
    return count;
  return 0;
}

void _ftStreamClose(FXFT_Stream stream) {}

};  // extern "C"

FXFT_Face CFGAS_FontMgrImp::LoadFace(IFX_FileRead* pFontStream,
                                     int32_t iFaceIndex) {
  if (!pFontStream)
    return nullptr;

  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  pFontMgr->InitFTLibrary();
  FXFT_Library library = pFontMgr->GetFTLibrary();
  if (!library)
    return nullptr;

  FXFT_Stream ftStream = FX_Alloc(FXFT_StreamRec, 1);
  FXSYS_memset(ftStream, 0, sizeof(FXFT_StreamRec));
  ftStream->base = nullptr;
  ftStream->descriptor.pointer = pFontStream;
  ftStream->pos = 0;
  ftStream->size = (unsigned long)pFontStream->GetSize();
  ftStream->read = _ftStreamRead;
  ftStream->close = _ftStreamClose;

  FXFT_Open_Args ftArgs;
  FXSYS_memset(&ftArgs, 0, sizeof(FXFT_Open_Args));
  ftArgs.flags |= FT_OPEN_STREAM;
  ftArgs.stream = ftStream;

  FXFT_Face pFace = nullptr;
  if (FXFT_Open_Face(library, &ftArgs, iFaceIndex, &pFace)) {
    FX_Free(ftStream);
    return nullptr;
  }

  FXFT_Set_Pixel_Sizes(pFace, 0, 64);
  return pFace;
}

IFX_FileRead* CFGAS_FontMgrImp::CreateFontStream(
    CFX_FontMapper* pFontMapper,
    IFX_SystemFontInfo* pSystemFontInfo,
    uint32_t index) {
  int iExact = 0;
  void* hFont =
      pSystemFontInfo->MapFont(0, 0, FXFONT_DEFAULT_CHARSET, 0,
                               pFontMapper->GetFaceName(index).c_str(), iExact);
  if (!hFont)
    return nullptr;

  uint32_t dwFileSize = pSystemFontInfo->GetFontData(hFont, 0, nullptr, 0);
  if (dwFileSize == 0)
    return nullptr;

  uint8_t* pBuffer = FX_Alloc(uint8_t, dwFileSize + 1);
  dwFileSize = pSystemFontInfo->GetFontData(hFont, 0, pBuffer, dwFileSize);

  return FX_CreateMemoryStream(pBuffer, dwFileSize, TRUE);
}

IFX_FileRead* CFGAS_FontMgrImp::CreateFontStream(
    const CFX_ByteString& bsFaceName) {
  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  CFX_FontMapper* pFontMapper = pFontMgr->GetBuiltinMapper();

  if (!pFontMapper)
    return nullptr;

  IFX_SystemFontInfo* pSystemFontInfo = pFontMapper->GetSystemFontInfo();
  if (!pSystemFontInfo)
    return nullptr;

  pSystemFontInfo->EnumFontList(pFontMapper);
  for (int32_t i = 0; i < pFontMapper->GetFaceSize(); ++i) {
    if (pFontMapper->GetFaceName(i) == bsFaceName)
      return CreateFontStream(pFontMapper, pSystemFontInfo, i);
  }
  return nullptr;
}

int32_t CFGAS_FontMgrImp::MatchFonts(CFX_FontDescriptorInfos& MatchedFonts,
                                     uint16_t wCodePage,
                                     uint32_t dwFontStyles,
                                     const CFX_WideString& FontName,
                                     FX_WCHAR wcUnicode) {
  MatchedFonts.RemoveAll();
  CFX_WideString wsNormalizedFontName = FontName;

  CFX_FontDescriptor* pFont = nullptr;
  int32_t nCount = m_InstalledFonts.GetSize();
  for (int32_t i = 0; i < nCount; i++) {
    pFont = m_InstalledFonts[i];
    int32_t nPenalty = CalcPenalty(pFont, wCodePage, dwFontStyles,
                                   wsNormalizedFontName, wcUnicode);
    if (nPenalty >= 0xffff)
      continue;

    FX_FontDescriptorInfo FontInfo;
    FontInfo.pFont = pFont;
    FontInfo.nPenalty = nPenalty;
    MatchedFonts.Add(FontInfo);
    if (MatchedFonts.GetSize() == 0xffff)
      break;
  }
  if (MatchedFonts.GetSize() == 0)
    return 0;

  CFX_SSortTemplate<FX_FontDescriptorInfo> ssort;
  ssort.ShellSort(MatchedFonts.GetData(), MatchedFonts.GetSize());
  return MatchedFonts.GetSize();
}

struct FX_BitCodePage {
  uint16_t wBit;
  uint16_t wCodePage;
};
static const FX_BitCodePage g_Bit2CodePage[] = {
    {0, 1252}, {1, 1250}, {2, 1251}, {3, 1253},  {4, 1254}, {5, 1255},
    {6, 1256}, {7, 1257}, {8, 1258}, {9, 0},     {10, 0},   {11, 0},
    {12, 0},   {13, 0},   {14, 0},   {15, 0},    {16, 874}, {17, 932},
    {18, 936}, {19, 949}, {20, 950}, {21, 1361}, {22, 0},   {23, 0},
    {24, 0},   {25, 0},   {26, 0},   {27, 0},    {28, 0},   {29, 0},
    {30, 0},   {31, 0},   {32, 0},   {33, 0},    {34, 0},   {35, 0},
    {36, 0},   {37, 0},   {38, 0},   {39, 0},    {40, 0},   {41, 0},
    {42, 0},   {43, 0},   {44, 0},   {45, 0},    {46, 0},   {47, 0},
    {48, 869}, {49, 866}, {50, 865}, {51, 864},  {52, 863}, {53, 862},
    {54, 861}, {55, 860}, {56, 857}, {57, 855},  {58, 852}, {59, 775},
    {60, 737}, {61, 708}, {62, 850}, {63, 437},
};

uint16_t FX_GetCodePageBit(uint16_t wCodePage) {
  for (size_t i = 0; i < FX_ArraySize(g_Bit2CodePage); ++i) {
    if (g_Bit2CodePage[i].wCodePage == wCodePage)
      return g_Bit2CodePage[i].wBit;
  }
  return (uint16_t)-1;
}

uint16_t FX_GetUnicodeBit(FX_WCHAR wcUnicode) {
  const FGAS_FONTUSB* x = FGAS_GetUnicodeBitField(wcUnicode);
  return x ? x->wBitField : 999;
}

int32_t CFGAS_FontMgrImp::CalcPenalty(CFX_FontDescriptor* pInstalled,
                                      uint16_t wCodePage,
                                      uint32_t dwFontStyles,
                                      const CFX_WideString& FontName,
                                      FX_WCHAR wcUnicode) {
  int32_t nPenalty = 30000;
  if (0 != FontName.GetLength()) {
    if (FontName != pInstalled->m_wsFaceName) {
      int32_t i;
      for (i = 0; i < pInstalled->m_wsFamilyNames.GetSize(); i++) {
        if (pInstalled->m_wsFamilyNames[i] == FontName) {
          break;
        }
      }
      if (i == pInstalled->m_wsFamilyNames.GetSize()) {
        nPenalty += 0xFFFF;
      } else {
        nPenalty -= 28000;
      }
    } else {
      nPenalty -= 30000;
    }
    if (30000 == nPenalty &&
        0 == IsPartName(pInstalled->m_wsFaceName, FontName)) {
      int32_t i;
      for (i = 0; i < pInstalled->m_wsFamilyNames.GetSize(); i++) {
        if (0 != IsPartName(pInstalled->m_wsFamilyNames[i], FontName)) {
          break;
        }
      }
      if (i == pInstalled->m_wsFamilyNames.GetSize()) {
        nPenalty += 0xFFFF;
      } else {
        nPenalty -= 26000;
      }
    } else {
      nPenalty -= 27000;
    }
  }
  uint32_t dwStyleMask = pInstalled->m_dwFontStyles ^ dwFontStyles;
  if (dwStyleMask & FX_FONTSTYLE_Bold) {
    nPenalty += 4500;
  }
  if (dwStyleMask & FX_FONTSTYLE_FixedPitch) {
    nPenalty += 10000;
  }
  if (dwStyleMask & FX_FONTSTYLE_Italic) {
    nPenalty += 10000;
  }
  if (dwStyleMask & FX_FONTSTYLE_Serif) {
    nPenalty += 500;
  }
  if (dwStyleMask & FX_FONTSTYLE_Symbolic) {
    nPenalty += 0xFFFF;
  }
  if (nPenalty >= 0xFFFF) {
    return 0xFFFF;
  }
  uint16_t wBit =
      ((0 == wCodePage || 0xFFFF == wCodePage) ? (uint16_t)-1
                                               : FX_GetCodePageBit(wCodePage));
  if (wBit != (uint16_t)-1) {
    ASSERT(wBit < 64);
    if (0 == (pInstalled->m_dwCsb[wBit / 32] & (1 << (wBit % 32)))) {
      nPenalty += 0xFFFF;
    } else {
      nPenalty -= 60000;
    }
  }
  wBit =
      ((0 == wcUnicode || 0xFFFE == wcUnicode) ? (uint16_t)999
                                               : FX_GetUnicodeBit(wcUnicode));
  if (wBit != (uint16_t)999) {
    ASSERT(wBit < 128);
    if (0 == (pInstalled->m_dwUsb[wBit / 32] & (1 << (wBit % 32)))) {
      nPenalty += 0xFFFF;
    } else {
      nPenalty -= 60000;
    }
  }
  return nPenalty;
}

void CFGAS_FontMgrImp::ClearFontCache() {
  FX_POSITION pos = m_Hash2CandidateList.GetStartPosition();
  while (pos) {
    uint32_t dwHash;
    CFX_FontDescriptorInfos* pDescs;
    m_Hash2CandidateList.GetNextAssoc(pos, dwHash, pDescs);
    delete pDescs;
  }
  pos = m_IFXFont2FileRead.GetStartPosition();
  while (pos) {
    CFGAS_GEFont* pFont;
    IFX_FileRead* pFileRead;
    m_IFXFont2FileRead.GetNextAssoc(pos, pFont, pFileRead);
    pFileRead->Release();
  }
}

void CFGAS_FontMgrImp::RemoveFont(CFGAS_GEFont* pEFont) {
  if (!pEFont) {
    return;
  }
  IFX_FileRead* pFileRead;
  if (m_IFXFont2FileRead.Lookup(pEFont, pFileRead)) {
    pFileRead->Release();
    m_IFXFont2FileRead.RemoveKey(pEFont);
  }
  FX_POSITION pos;
  pos = m_Hash2Fonts.GetStartPosition();
  while (pos) {
    uint32_t dwHash;
    CFX_ArrayTemplate<CFGAS_GEFont*>* pFonts;
    m_Hash2Fonts.GetNextAssoc(pos, dwHash, pFonts);
    if (pFonts) {
      for (int32_t i = 0; i < pFonts->GetSize(); i++) {
        if (pFonts->GetAt(i) == pEFont) {
          pFonts->SetAt(i, nullptr);
        }
      }
    } else {
      m_Hash2Fonts.RemoveKey(dwHash);
    }
  }
}

void CFGAS_FontMgrImp::RegisterFace(FXFT_Face pFace,
                                    CFX_FontDescriptors& Fonts,
                                    const CFX_WideString* pFaceName) {
  if ((pFace->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
    return;

  std::unique_ptr<CFX_FontDescriptor> pFont(new CFX_FontDescriptor);
  pFont->m_dwFontStyles |= FXFT_Is_Face_Bold(pFace) ? FX_FONTSTYLE_Bold : 0;
  pFont->m_dwFontStyles |= FXFT_Is_Face_Italic(pFace) ? FX_FONTSTYLE_Italic : 0;
  pFont->m_dwFontStyles |= GetFlags(pFace);

  std::vector<uint16_t> charsets = GetCharsets(pFace);
  GetUSBCSB(pFace, pFont->m_dwUsb, pFont->m_dwCsb);

  FT_ULong dwTag;
  FT_ENC_TAG(dwTag, 'n', 'a', 'm', 'e');

  std::vector<uint8_t> table;
  unsigned long nLength = 0;
  unsigned int error = FXFT_Load_Sfnt_Table(pFace, dwTag, 0, nullptr, &nLength);
  if (error == 0 && nLength != 0) {
    table.resize(nLength);
    if (FXFT_Load_Sfnt_Table(pFace, dwTag, 0, table.data(), nullptr))
      table.clear();
  }
  GetNames(table.empty() ? nullptr : table.data(), pFont->m_wsFamilyNames);

  pFont->m_wsFamilyNames.Add(CFX_ByteString(pFace->family_name).UTF8Decode());
  pFont->m_wsFaceName =
      pFaceName ? *pFaceName
                : CFX_WideString::FromLocal(FXFT_Get_Postscript_Name(pFace));
  pFont->m_nFaceIndex = pFace->face_index;

  Fonts.Add(pFont.release());
}

void CFGAS_FontMgrImp::RegisterFaces(IFX_FileRead* pFontStream,
                                     const CFX_WideString* pFaceName) {
  int32_t index = 0;
  int32_t num_faces = 0;
  do {
    FXFT_Face pFace = LoadFace(pFontStream, index++);
    if (!pFace)
      continue;
    // All faces keep number of faces. It can be retrieved from any one face.
    if (num_faces == 0)
      num_faces = pFace->num_faces;
    RegisterFace(pFace, m_InstalledFonts, pFaceName);
    if (FXFT_Get_Face_External_Stream(pFace))
      FXFT_Clear_Face_External_Stream(pFace);
    FXFT_Done_Face(pFace);
  } while (index < num_faces);
}

uint32_t CFGAS_FontMgrImp::GetFlags(FXFT_Face pFace) {
  uint32_t flag = 0;
  if (FT_IS_FIXED_WIDTH(pFace)) {
    flag |= FX_FONTSTYLE_FixedPitch;
  }
  TT_OS2* pOS2 = (TT_OS2*)FT_Get_Sfnt_Table(pFace, ft_sfnt_os2);
  if (!pOS2) {
    return flag;
  }
  if (pOS2->ulCodePageRange1 & (1 << 31)) {
    flag |= FX_FONTSTYLE_Symbolic;
  }
  if (pOS2->panose[0] == 2) {
    uint8_t uSerif = pOS2->panose[1];
    if ((uSerif > 1 && uSerif < 10) || uSerif > 13) {
      flag |= FX_FONTSTYLE_Serif;
    }
  }
  return flag;
}

#define GetUInt8(p) ((uint8_t)((p)[0]))
#define GetUInt16(p) ((uint16_t)((p)[0] << 8 | (p)[1]))
#define GetUInt32(p) \
  ((uint32_t)((p)[0] << 24 | (p)[1] << 16 | (p)[2] << 8 | (p)[3]))

void CFGAS_FontMgrImp::GetNames(const uint8_t* name_table,
                                CFX_WideStringArray& Names) {
  if (!name_table) {
    return;
  }
  uint8_t* lpTable = (uint8_t*)name_table;
  CFX_WideString wsFamily;
  uint8_t* sp = lpTable + 2;
  uint8_t* lpNameRecord = lpTable + 6;
  uint16_t nNameCount = GetUInt16(sp);
  uint8_t* lpStr = lpTable + GetUInt16(sp + 2);
  for (uint16_t j = 0; j < nNameCount; j++) {
    uint16_t nNameID = GetUInt16(lpNameRecord + j * 12 + 6);
    if (nNameID != 1) {
      continue;
    }
    uint16_t nPlatformID = GetUInt16(lpNameRecord + j * 12 + 0);
    uint16_t nNameLength = GetUInt16(lpNameRecord + j * 12 + 8);
    uint16_t nNameOffset = GetUInt16(lpNameRecord + j * 12 + 10);
    wsFamily.clear();
    if (nPlatformID != 1) {
      for (uint16_t k = 0; k < nNameLength / 2; k++) {
        FX_WCHAR wcTemp = GetUInt16(lpStr + nNameOffset + k * 2);
        wsFamily += wcTemp;
      }
      Names.Add(wsFamily);
    } else {
      for (uint16_t k = 0; k < nNameLength; k++) {
        FX_WCHAR wcTemp = GetUInt8(lpStr + nNameOffset + k);
        wsFamily += wcTemp;
      }
      Names.Add(wsFamily);
    }
  }
}
#undef GetUInt8
#undef GetUInt16
#undef GetUInt32
struct FX_BIT2CHARSET {
  uint16_t wBit;
  uint16_t wCharset;
};

FX_BIT2CHARSET g_FX_Bit2Charset1[16] = {
    {1 << 0, FX_CHARSET_ANSI},
    {1 << 1, FX_CHARSET_MSWin_EasterEuropean},
    {1 << 2, FX_CHARSET_MSWin_Cyrillic},
    {1 << 3, FX_CHARSET_MSWin_Greek},
    {1 << 4, FX_CHARSET_MSWin_Turkish},
    {1 << 5, FX_CHARSET_MSWin_Hebrew},
    {1 << 6, FX_CHARSET_MSWin_Arabic},
    {1 << 7, FX_CHARSET_MSWin_Baltic},
    {1 << 8, FX_CHARSET_MSWin_Vietnamese},
    {1 << 9, FX_CHARSET_Default},
    {1 << 10, FX_CHARSET_Default},
    {1 << 11, FX_CHARSET_Default},
    {1 << 12, FX_CHARSET_Default},
    {1 << 13, FX_CHARSET_Default},
    {1 << 14, FX_CHARSET_Default},
    {1 << 15, FX_CHARSET_Default},
};

FX_BIT2CHARSET g_FX_Bit2Charset2[16] = {
    {1 << 0, FX_CHARSET_Thai},
    {1 << 1, FX_CHARSET_ShiftJIS},
    {1 << 2, FX_CHARSET_ChineseSimplified},
    {1 << 3, FX_CHARSET_Korean},
    {1 << 4, FX_CHARSET_ChineseTriditional},
    {1 << 5, FX_CHARSET_Johab},
    {1 << 6, FX_CHARSET_Default},
    {1 << 7, FX_CHARSET_Default},
    {1 << 8, FX_CHARSET_Default},
    {1 << 9, FX_CHARSET_Default},
    {1 << 10, FX_CHARSET_Default},
    {1 << 11, FX_CHARSET_Default},
    {1 << 12, FX_CHARSET_Default},
    {1 << 13, FX_CHARSET_Default},
    {1 << 14, FX_CHARSET_OEM},
    {1 << 15, FX_CHARSET_Symbol},
};

FX_BIT2CHARSET g_FX_Bit2Charset3[16] = {
    {1 << 0, FX_CHARSET_Default},  {1 << 1, FX_CHARSET_Default},
    {1 << 2, FX_CHARSET_Default},  {1 << 3, FX_CHARSET_Default},
    {1 << 4, FX_CHARSET_Default},  {1 << 5, FX_CHARSET_Default},
    {1 << 6, FX_CHARSET_Default},  {1 << 7, FX_CHARSET_Default},
    {1 << 8, FX_CHARSET_Default},  {1 << 9, FX_CHARSET_Default},
    {1 << 10, FX_CHARSET_Default}, {1 << 11, FX_CHARSET_Default},
    {1 << 12, FX_CHARSET_Default}, {1 << 13, FX_CHARSET_Default},
    {1 << 14, FX_CHARSET_Default}, {1 << 15, FX_CHARSET_Default},
};

FX_BIT2CHARSET g_FX_Bit2Charset4[16] = {
    {1 << 0, FX_CHARSET_Default},  {1 << 1, FX_CHARSET_Default},
    {1 << 2, FX_CHARSET_Default},  {1 << 3, FX_CHARSET_Default},
    {1 << 4, FX_CHARSET_Default},  {1 << 5, FX_CHARSET_Default},
    {1 << 6, FX_CHARSET_Default},  {1 << 7, FX_CHARSET_Default},
    {1 << 8, FX_CHARSET_Default},  {1 << 9, FX_CHARSET_Default},
    {1 << 10, FX_CHARSET_Default}, {1 << 11, FX_CHARSET_Default},
    {1 << 12, FX_CHARSET_Default}, {1 << 13, FX_CHARSET_Default},
    {1 << 14, FX_CHARSET_Default}, {1 << 15, FX_CHARSET_US},
};

#define CODEPAGERANGE_IMPLEMENT(n)                         \
  for (int32_t i = 0; i < 16; i++) {                       \
    if ((a##n & g_FX_Bit2Charset##n[i].wBit) != 0)         \
      charsets.push_back(g_FX_Bit2Charset##n[i].wCharset); \
  }

std::vector<uint16_t> CFGAS_FontMgrImp::GetCharsets(FXFT_Face pFace) const {
  std::vector<uint16_t> charsets;
  TT_OS2* pOS2 = (TT_OS2*)FT_Get_Sfnt_Table(pFace, ft_sfnt_os2);
  if (pOS2) {
    uint16_t a1 = pOS2->ulCodePageRange1 & 0xffff;
    CODEPAGERANGE_IMPLEMENT(1);
    uint16_t a2 = (pOS2->ulCodePageRange1 >> 16) & 0xffff;
    CODEPAGERANGE_IMPLEMENT(2);
    uint16_t a3 = pOS2->ulCodePageRange2 & 0xffff;
    CODEPAGERANGE_IMPLEMENT(3);
    uint16_t a4 = (pOS2->ulCodePageRange2 >> 16) & 0xffff;
    CODEPAGERANGE_IMPLEMENT(4);
  } else {
    charsets.push_back(FX_CHARSET_Default);
  }
  return charsets;
}

#undef CODEPAGERANGE_IMPLEMENT
void CFGAS_FontMgrImp::GetUSBCSB(FXFT_Face pFace,
                                 uint32_t* USB,
                                 uint32_t* CSB) {
  TT_OS2* pOS2 = (TT_OS2*)FT_Get_Sfnt_Table(pFace, ft_sfnt_os2);
  if (pOS2) {
    USB[0] = pOS2->ulUnicodeRange1;
    USB[1] = pOS2->ulUnicodeRange2;
    USB[2] = pOS2->ulUnicodeRange3;
    USB[3] = pOS2->ulUnicodeRange4;
    CSB[0] = pOS2->ulCodePageRange1;
    CSB[1] = pOS2->ulCodePageRange2;
  } else {
    USB[0] = 0;
    USB[1] = 0;
    USB[2] = 0;
    USB[3] = 0;
    CSB[0] = 0;
    CSB[1] = 0;
  }
}

int32_t CFGAS_FontMgrImp::IsPartName(const CFX_WideString& Name1,
                                     const CFX_WideString& Name2) {
  if (Name1.Find(Name2.c_str()) != -1) {
    return 1;
  }
  return 0;
}

#endif
