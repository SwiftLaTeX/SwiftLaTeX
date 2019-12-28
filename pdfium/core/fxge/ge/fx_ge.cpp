// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/include/fx_ge.h"

#include "core/fxge/ge/fx_text_int.h"

static CFX_GEModule* g_pGEModule = nullptr;

CFX_GEModule::CFX_GEModule(const char** pUserFontPaths,
                           CCodec_ModuleMgr* pCodecModule)
    : m_FTLibrary(nullptr),
      m_pFontCache(nullptr),
      m_pFontMgr(new CFX_FontMgr),
      m_pCodecModule(pCodecModule),
      m_pPlatformData(nullptr),
      m_pUserFontPaths(pUserFontPaths) {
  InitPlatform();
  SetTextGamma(2.2f);
}

CFX_GEModule::~CFX_GEModule() {
  delete m_pFontCache;
  DestroyPlatform();
}

// static
void CFX_GEModule::Create(const char** userFontPaths,
                          CCodec_ModuleMgr* pCodecModule) {
  ASSERT(!g_pGEModule);
  g_pGEModule = new CFX_GEModule(userFontPaths, pCodecModule);
}

// static
CFX_GEModule* CFX_GEModule::Get() {
  return g_pGEModule;
}

// static
void CFX_GEModule::Destroy() {
  ASSERT(g_pGEModule);
  delete g_pGEModule;
  g_pGEModule = nullptr;
}

CFX_FontCache* CFX_GEModule::GetFontCache() {
  if (!m_pFontCache)
    m_pFontCache = new CFX_FontCache();
  return m_pFontCache;
}

void CFX_GEModule::SetTextGamma(FX_FLOAT gammaValue) {
  gammaValue /= 2.2f;
  for (int i = 0; i < 256; ++i) {
    m_GammaValue[i] = static_cast<uint8_t>(
        FXSYS_pow(static_cast<FX_FLOAT>(i) / 255, gammaValue) * 255.0f + 0.5f);
  }
}

const uint8_t* CFX_GEModule::GetTextGammaTable() const {
  return m_GammaValue;
}
