// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_

#include "core/fxcrt/include/fx_system.h"

class CCodec_ScanlineDecoder;

class CCodec_BasicModule {
 public:
  CCodec_ScanlineDecoder* CreateRunLengthDecoder(const uint8_t* src_buf,
                                                 uint32_t src_size,
                                                 int width,
                                                 int height,
                                                 int nComps,
                                                 int bpc);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_
