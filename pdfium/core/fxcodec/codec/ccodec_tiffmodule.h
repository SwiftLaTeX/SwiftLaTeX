// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_

#include "core/fxcrt/include/fx_system.h"

class CCodec_TiffContext;
class CFX_DIBAttribute;
class CFX_DIBitmap;
class IFX_FileRead;

class CCodec_TiffModule {
 public:
  ~CCodec_TiffModule() {}

  CCodec_TiffContext* CreateDecoder(IFX_FileRead* file_ptr);

  bool LoadFrameInfo(CCodec_TiffContext* ctx,
                     int32_t frame,
                     int32_t* width,
                     int32_t* height,
                     int32_t* comps,
                     int32_t* bpc,
                     CFX_DIBAttribute* pAttribute);
  bool Decode(CCodec_TiffContext* ctx, class CFX_DIBitmap* pDIBitmap);
  void DestroyDecoder(CCodec_TiffContext* ctx);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_
