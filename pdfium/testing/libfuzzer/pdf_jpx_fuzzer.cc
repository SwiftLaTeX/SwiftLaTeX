// Copyright 2016 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdint>
#include <memory>
#include <vector>

#include "core/fxcodec/codec/codec_int.h"
#include "core/fxge/include/fx_dib.h"

CCodec_JpxModule g_module;

struct DecoderDeleter {
  void operator()(CJPX_Decoder* decoder) { g_module.DestroyDecoder(decoder); }
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  std::unique_ptr<CJPX_Decoder, DecoderDeleter> decoder(
      g_module.CreateDecoder(data, size, nullptr));
  if (!decoder)
    return 0;

  uint32_t width;
  uint32_t height;
  uint32_t components;
  g_module.GetImageInfo(decoder.get(), &width, &height, &components);

  FXDIB_Format format;
  if (components == 1) {
    format = FXDIB_8bppRgb;
  } else if (components <= 3) {
    format = FXDIB_Rgb;
  } else if (components == 4) {
    format = FXDIB_Rgb32;
  } else {
    width = (width * components + 2) / 3;
    format = FXDIB_Rgb;
  }

  std::unique_ptr<CFX_DIBitmap> bitmap(new CFX_DIBitmap);
  if (!bitmap->Create(width, height, format))
    return 0;

  std::vector<uint8_t> output_offsets(components);
  for (uint32_t i = 0; i < components; ++i)
    output_offsets[i] = i;

  g_module.Decode(decoder.get(), bitmap->GetBuffer(), bitmap->GetPitch(),
                  output_offsets);
  return 0;
}
