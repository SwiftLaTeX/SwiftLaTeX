// Copyright 2016 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_LIBFUZZER_XFA_CODEC_FUZZER_H_
#define TESTING_LIBFUZZER_XFA_CODEC_FUZZER_H_

#include <memory>

#include "core/fxcodec/codec/include/ccodec_progressivedecoder.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_stream.h"

class XFACodecFuzzer {
 public:
  static int Fuzz(const uint8_t* data, size_t size, FXCODEC_IMAGE_TYPE type) {
    std::unique_ptr<CCodec_ModuleMgr> mgr(new CCodec_ModuleMgr());
    std::unique_ptr<CCodec_ProgressiveDecoder> decoder(
        mgr->CreateProgressiveDecoder());
    Reader source(data, size);

    FXCODEC_STATUS status =
        decoder->LoadImageInfo(&source, type, nullptr, true);
    if (status != FXCODEC_STATUS_FRAME_READY)
      return 0;

    std::unique_ptr<CFX_DIBitmap> bitmap(new CFX_DIBitmap);
    bitmap->Create(decoder->GetWidth(), decoder->GetHeight(), FXDIB_Argb);

    int32_t frames;
    if (decoder->GetFrames(frames) != FXCODEC_STATUS_DECODE_READY ||
        frames == 0)
      return 0;

    status = decoder->StartDecode(bitmap.get(), 0, 0, bitmap->GetWidth(),
                                  bitmap->GetHeight());
    while (status == FXCODEC_STATUS_DECODE_TOBECONTINUE)
      status = decoder->ContinueDecode();

    return 0;
  }

 private:
  class Reader : public IFX_FileRead {
   public:
    Reader(const uint8_t* data, size_t size) : m_data(data), m_size(size) {}
    ~Reader() {}

    void Release() override {}

    FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) override {
      if (offset + size > m_size)
        size = m_size - offset;
      memcpy(buffer, m_data + offset, size);
      return TRUE;
    }

    FX_FILESIZE GetSize() override { return static_cast<FX_FILESIZE>(m_size); }

   private:
    const uint8_t* const m_data;
    size_t m_size;
  };
};

#endif  // TESTING_LIBFUZZER_XFA_CODEC_FUZZER_H_
