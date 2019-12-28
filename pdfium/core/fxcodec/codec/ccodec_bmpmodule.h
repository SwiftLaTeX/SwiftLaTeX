// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_BMPMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_BMPMODULE_H_

#include "core/fxcrt/include/fx_system.h"

struct FXBMP_Context;
class CFX_DIBAttribute;

class CCodec_BmpModule {
 public:
  CCodec_BmpModule() { FXSYS_memset(m_szLastError, 0, sizeof(m_szLastError)); }

  FXBMP_Context* Start(void* pModule);
  void Finish(FXBMP_Context* pContext);

  uint32_t GetAvailInput(FXBMP_Context* pContext, uint8_t** avail_buf_ptr);
  void Input(FXBMP_Context* pContext,
             const uint8_t* src_buf,
             uint32_t src_size);

  int32_t ReadHeader(FXBMP_Context* pContext,
                     int32_t* width,
                     int32_t* height,
                     FX_BOOL* tb_flag,
                     int32_t* components,
                     int32_t* pal_num,
                     uint32_t** pal_pp,
                     CFX_DIBAttribute* pAttribute);
  int32_t LoadImage(FXBMP_Context* pContext);

  FX_BOOL (*InputImagePositionBufCallback)(void* pModule, uint32_t rcd_pos);
  void (*ReadScanlineCallback)(void* pModule,
                               int32_t row_num,
                               uint8_t* row_buf);

 protected:
  FX_CHAR m_szLastError[256];
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_BMPMODULE_H_
