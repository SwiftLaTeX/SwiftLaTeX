// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_FPF_SKIAMODULE_H_
#define CORE_FXGE_ANDROID_FPF_SKIAMODULE_H_

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include "core/fxge/android/fpf_skiafontmgr.h"

class CFPF_SkiaFontMgr;

class CFPF_SkiaDeviceModule {
 public:
  CFPF_SkiaDeviceModule() : m_pFontMgr(nullptr) {}
  ~CFPF_SkiaDeviceModule();

  void Destroy();
  CFPF_SkiaFontMgr* GetFontMgr();

 protected:
  CFPF_SkiaFontMgr* m_pFontMgr;
};

CFPF_SkiaDeviceModule* CFPF_GetSkiaDeviceModule();

#endif  // _FX_OS_ == _FX_ANDROID_

#endif  // CORE_FXGE_ANDROID_FPF_SKIAMODULE_H_
