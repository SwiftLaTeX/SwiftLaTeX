// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_

#include "core/fxcrt/include/fx_system.h"

struct FXGIF_Context;

class CCodec_GifModule {
 public:
  CCodec_GifModule() { FXSYS_memset(m_szLastError, 0, sizeof(m_szLastError)); }

  FXGIF_Context* Start(void* pModule);
  void Finish(FXGIF_Context* pContext);

  uint32_t GetAvailInput(FXGIF_Context* pContext,
                         uint8_t** avail_buf_ptr = nullptr);
  void Input(FXGIF_Context* pContext,
             const uint8_t* src_buf,
             uint32_t src_size);

  int32_t ReadHeader(FXGIF_Context* pContext,
                     int* width,
                     int* height,
                     int* pal_num,
                     void** pal_pp,
                     int* bg_index,
                     CFX_DIBAttribute* pAttribute);

  int32_t LoadFrameInfo(FXGIF_Context* pContext, int* frame_num);

  int32_t LoadFrame(FXGIF_Context* pContext,
                    int frame_num,
                    CFX_DIBAttribute* pAttribute);

  void (*RecordCurrentPositionCallback)(void* pModule, uint32_t& cur_pos);
  uint8_t* (*AskLocalPaletteBufCallback)(void* pModule,
                                         int32_t frame_num,
                                         int32_t pal_size);
  FX_BOOL (*InputRecordPositionBufCallback)(void* pModule,
                                            uint32_t rcd_pos,
                                            const FX_RECT& img_rc,
                                            int32_t pal_num,
                                            void* pal_ptr,
                                            int32_t delay_time,
                                            FX_BOOL user_input,
                                            int32_t trans_index,
                                            int32_t disposal_method,
                                            FX_BOOL interlace);
  void (*ReadScanlineCallback)(void* pModule,
                               int32_t row_num,
                               uint8_t* row_buf);

 protected:
  FX_CHAR m_szLastError[256];
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_
