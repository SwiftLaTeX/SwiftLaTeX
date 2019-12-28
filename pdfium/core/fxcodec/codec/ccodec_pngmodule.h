// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_

#include "core/fxcrt/include/fx_system.h"

class CFX_DIBAttribute;
struct FXPNG_Context;

#define PNG_ERROR_SIZE 256

class CCodec_PngModule {
 public:
  CCodec_PngModule() { FXSYS_memset(m_szLastError, 0, sizeof(m_szLastError)); }

  FXPNG_Context* Start(void* pModule);
  void Finish(FXPNG_Context* pContext);
  FX_BOOL Input(FXPNG_Context* pContext,
                const uint8_t* src_buf,
                uint32_t src_size,
                CFX_DIBAttribute* pAttribute);

  FX_BOOL (*ReadHeaderCallback)(void* pModule,
                                int width,
                                int height,
                                int bpc,
                                int pass,
                                int* color_type,
                                double* gamma);
  FX_BOOL (*AskScanlineBufCallback)(void* pModule, int line, uint8_t*& src_buf);
  void (*FillScanlineBufCompletedCallback)(void* pModule, int pass, int line);

 protected:
  FX_CHAR m_szLastError[PNG_ERROR_SIZE];
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_
