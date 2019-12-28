// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_FPF_SKIAFONTMGR_H_
#define CORE_FXGE_ANDROID_FPF_SKIAFONTMGR_H_

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include <map>
#include <vector>

#include "core/fxge/include/fx_font.h"

#define FPF_SKIAFONTTYPE_Unknown 0
#define FPF_SKIAFONTTYPE_Path 1
#define FPF_SKIAFONTTYPE_File 2
#define FPF_SKIAFONTTYPE_Buffer 3

#define FPF_MATCHFONT_REPLACEANSI 1

class CFPF_SkiaFont;

class CFPF_SkiaFontDescriptor {
 public:
  CFPF_SkiaFontDescriptor()
      : m_pFamily(nullptr),
        m_dwStyle(0),
        m_iFaceIndex(0),
        m_dwCharsets(0),
        m_iGlyphNum(0) {}
  virtual ~CFPF_SkiaFontDescriptor() { FX_Free(m_pFamily); }

  virtual int32_t GetType() const { return FPF_SKIAFONTTYPE_Unknown; }

  void SetFamily(const FX_CHAR* pFamily) {
    FX_Free(m_pFamily);
    int32_t iSize = FXSYS_strlen(pFamily);
    m_pFamily = FX_Alloc(FX_CHAR, iSize + 1);
    FXSYS_memcpy(m_pFamily, pFamily, iSize * sizeof(FX_CHAR));
    m_pFamily[iSize] = 0;
  }
  FX_CHAR* m_pFamily;
  uint32_t m_dwStyle;
  int32_t m_iFaceIndex;
  uint32_t m_dwCharsets;
  int32_t m_iGlyphNum;
};

class CFPF_SkiaPathFont : public CFPF_SkiaFontDescriptor {
 public:
  CFPF_SkiaPathFont() : m_pPath(nullptr) {}
  ~CFPF_SkiaPathFont() override { FX_Free(m_pPath); }

  // CFPF_SkiaFontDescriptor
  int32_t GetType() const override { return FPF_SKIAFONTTYPE_Path; }

  void SetPath(const FX_CHAR* pPath) {
    FX_Free(m_pPath);
    int32_t iSize = FXSYS_strlen(pPath);
    m_pPath = FX_Alloc(FX_CHAR, iSize + 1);
    FXSYS_memcpy(m_pPath, pPath, iSize * sizeof(FX_CHAR));
    m_pPath[iSize] = 0;
  }
  FX_CHAR* m_pPath;
};

class CFPF_SkiaFileFont : public CFPF_SkiaFontDescriptor {
 public:
  CFPF_SkiaFileFont() : m_pFile(nullptr) {}

  // CFPF_SkiaFontDescriptor
  int32_t GetType() const override { return FPF_SKIAFONTTYPE_File; }
  IFX_FileRead* m_pFile;
};

class CFPF_SkiaBufferFont : public CFPF_SkiaFontDescriptor {
 public:
  CFPF_SkiaBufferFont() : m_pBuffer(nullptr), m_szBuffer(0) {}

  // CFPF_SkiaFontDescriptor
  int32_t GetType() const override { return FPF_SKIAFONTTYPE_Buffer; }

  void* m_pBuffer;
  size_t m_szBuffer;
};

class CFPF_SkiaFontMgr {
 public:
  CFPF_SkiaFontMgr();
  ~CFPF_SkiaFontMgr();

  void LoadSystemFonts();
  void LoadPrivateFont(IFX_FileRead* pFontFile);
  void LoadPrivateFont(const CFX_ByteStringC& bsFileName);
  void LoadPrivateFont(void* pBuffer, size_t szBuffer);
  CFPF_SkiaFont* CreateFont(const CFX_ByteStringC& bsFamilyname,
                            uint8_t uCharset,
                            uint32_t dwStyle,
                            uint32_t dwMatch = 0);

  FX_BOOL InitFTLibrary();
  FXFT_Face GetFontFace(IFX_FileRead* pFileRead, int32_t iFaceIndex = 0);
  FXFT_Face GetFontFace(const CFX_ByteStringC& bsFile, int32_t iFaceIndex = 0);
  FXFT_Face GetFontFace(const uint8_t* pBuffer,
                        size_t szBuffer,
                        int32_t iFaceIndex = 0);

 protected:
  void ScanPath(const CFX_ByteString& path);
  void ScanFile(const CFX_ByteString& file);
  void ReportFace(FXFT_Face face, CFPF_SkiaFontDescriptor* pFontDesc);
  void OutputSystemFonts();

  FX_BOOL m_bLoaded;
  FXFT_Library m_FTLibrary;
  std::vector<CFPF_SkiaFontDescriptor*> m_FontFaces;
  std::map<uint32_t, CFPF_SkiaFont*> m_FamilyFonts;
};

#endif  // _FX_OS_ == _FX_ANDROID_

#endif  // CORE_FXGE_ANDROID_FPF_SKIAFONTMGR_H_
