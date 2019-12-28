// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(fpdf_parser_decode, A85Decode) {
  pdfium::DecodeTestData test_data[] = {
      // Empty src string.
      STR_IN_OUT_CASE("", "", 0),
      // Empty content in src string.
      STR_IN_OUT_CASE("~>", "", 0),
      // Regular conversion.
      STR_IN_OUT_CASE("FCfN8~>", "test", 7),
      // End at the ending mark.
      STR_IN_OUT_CASE("FCfN8~>FCfN8", "test", 7),
      // Skip whitespaces.
      STR_IN_OUT_CASE("\t F C\r\n \tf N 8 ~>", "test", 17),
      // No ending mark.
      STR_IN_OUT_CASE("@3B0)DJj_BF*)>@Gp#-s", "a funny story :)", 20),
      // Non-multiple length.
      STR_IN_OUT_CASE("12A", "2k", 3),
      // Stop at unknown characters.
      STR_IN_OUT_CASE("FCfN8FCfN8vw", "testtest", 11),
  };
  for (size_t i = 0; i < FX_ArraySize(test_data); ++i) {
    pdfium::DecodeTestData* ptr = &test_data[i];
    uint8_t* result = nullptr;
    uint32_t result_size = 0;
    EXPECT_EQ(ptr->processed_size,
              A85Decode(ptr->input, ptr->input_size, result, result_size))
        << "for case " << i;
    ASSERT_EQ(ptr->expected_size, result_size);
    for (size_t j = 0; j < result_size; ++j) {
      EXPECT_EQ(ptr->expected[j], result[j]) << "for case " << i << " char "
                                             << j;
    }
    FX_Free(result);
  }
}

TEST(fpdf_parser_decode, HexDecode) {
  pdfium::DecodeTestData test_data[] = {
      // Empty src string.
      STR_IN_OUT_CASE("", "", 0),
      // Empty content in src string.
      STR_IN_OUT_CASE(">", "", 1),
      // Only whitespaces in src string.
      STR_IN_OUT_CASE("\t   \r\n>", "", 7),
      // Regular conversion.
      STR_IN_OUT_CASE("12Ac>zzz", "\x12\xac", 5),
      // Skip whitespaces.
      STR_IN_OUT_CASE("12 Ac\t02\r\nBF>zzz>", "\x12\xac\x02\xbf", 13),
      // Non-multiple length.
      STR_IN_OUT_CASE("12A>zzz", "\x12\xa0", 4),
      // Skips unknown characters.
      STR_IN_OUT_CASE("12tk  \tAc>zzz", "\x12\xac", 10),
      // No ending mark.
      STR_IN_OUT_CASE("12AcED3c3456", "\x12\xac\xed\x3c\x34\x56", 12),
  };
  for (size_t i = 0; i < FX_ArraySize(test_data); ++i) {
    pdfium::DecodeTestData* ptr = &test_data[i];
    uint8_t* result = nullptr;
    uint32_t result_size = 0;
    EXPECT_EQ(ptr->processed_size,
              HexDecode(ptr->input, ptr->input_size, result, result_size))
        << "for case " << i;
    ASSERT_EQ(ptr->expected_size, result_size);
    for (size_t j = 0; j < result_size; ++j) {
      EXPECT_EQ(ptr->expected[j], result[j]) << "for case " << i << " char "
                                             << j;
    }
    FX_Free(result);
  }
}
