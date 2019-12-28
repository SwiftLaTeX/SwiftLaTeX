// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include <memory>
#include <utility>

#include "core/fxge/android/fpf_skiamodule.h"
#include "core/fxge/android/fx_android_font.h"
#include "core/fxge/include/fx_ge.h"

void CFX_GEModule::InitPlatform() {
  CFPF_SkiaDeviceModule* pDeviceModule = CFPF_GetSkiaDeviceModule();
  if (!pDeviceModule)
    return;

  CFPF_SkiaFontMgr* pFontMgr = pDeviceModule->GetFontMgr();
  if (pFontMgr) {
    std::unique_ptr<CFX_AndroidFontInfo> pFontInfo(new CFX_AndroidFontInfo);
    pFontInfo->Init(pFontMgr);
    m_pFontMgr->SetSystemFontInfo(std::move(pFontInfo));
  }
  m_pPlatformData = pDeviceModule;
}

void CFX_GEModule::DestroyPlatform() {
  if (m_pPlatformData)
    static_cast<CFPF_SkiaDeviceModule*>(m_pPlatformData)->Destroy();
}

#endif  // _FX_OS_ == _FX_ANDROID_
