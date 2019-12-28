// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/include/fx_basic.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

uint32_t ReferenceGetBits32(const uint8_t* pData, int bitpos, int nbits) {
  int result = 0;
  for (int i = 0; i < nbits; i++) {
    if (pData[(bitpos + i) / 8] & (1 << (7 - (bitpos + i) % 8)))
      result |= 1 << (nbits - i - 1);
  }
  return result;
}

}  // namespace

TEST(fxge, GetBits32) {
  unsigned char data[] = {0xDE, 0x3F, 0xB1, 0x7C, 0x12, 0x9A, 0x04, 0x56};
  for (int nbits = 1; nbits <= 32; ++nbits) {
    for (int bitpos = 0; bitpos < (int)sizeof(data) * 8 - nbits; ++bitpos) {
      EXPECT_EQ(ReferenceGetBits32(data, bitpos, nbits),
                GetBits32(data, bitpos, nbits));
    }
  }
}
