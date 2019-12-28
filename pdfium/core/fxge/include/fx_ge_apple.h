// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_FX_GE_APPLE_H_
#define CORE_FXGE_INCLUDE_FX_GE_APPLE_H_

#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_

#include "core/fxge/include/fx_ge.h"

class CFX_QuartzDevice : public CFX_RenderDevice {
 public:
  CFX_QuartzDevice();
  ~CFX_QuartzDevice() override;

  FX_BOOL Attach(CGContextRef context, int32_t nDeviceClass = FXDC_DISPLAY);
  FX_BOOL Attach(CFX_DIBitmap* pBitmap);
  FX_BOOL Create(int32_t width, int32_t height, FXDIB_Format format);

  CGContextRef GetContext();

 protected:
  CGContextRef m_pContext;
  FX_BOOL m_bOwnedBitmap;
};
#endif

#endif  // CORE_FXGE_INCLUDE_FX_GE_APPLE_H_
