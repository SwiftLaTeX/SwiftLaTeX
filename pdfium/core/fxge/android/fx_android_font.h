// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_FX_ANDROID_FONT_H_
#define CORE_FXGE_ANDROID_FX_ANDROID_FONT_H_

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include "core/fxge/include/fx_font.h"

class CFPF_SkiaFontMgr;

class CFX_AndroidFontInfo : public IFX_SystemFontInfo {
 public:
  CFX_AndroidFontInfo();
  ~CFX_AndroidFontInfo() override;

  FX_BOOL Init(CFPF_SkiaFontMgr* pFontMgr);

  // IFX_SystemFontInfo:
  FX_BOOL EnumFontList(CFX_FontMapper* pMapper) override;
  void* MapFont(int weight,
                FX_BOOL bItalic,
                int charset,
                int pitch_family,
                const FX_CHAR* face,
                int& bExact) override;
  void* GetFont(const FX_CHAR* face) override;
  uint32_t GetFontData(void* hFont,
                       uint32_t table,
                       uint8_t* buffer,
                       uint32_t size) override;
  FX_BOOL GetFaceName(void* hFont, CFX_ByteString& name) override;
  FX_BOOL GetFontCharset(void* hFont, int& charset) override;
  void DeleteFont(void* hFont) override;
  void* RetainFont(void* hFont) override;

 protected:
  CFPF_SkiaFontMgr* m_pFontMgr;
};

#endif  // _FX_OS_ == _FX_ANDROID_

#endif  // CORE_FXGE_ANDROID_FX_ANDROID_FONT_H_
