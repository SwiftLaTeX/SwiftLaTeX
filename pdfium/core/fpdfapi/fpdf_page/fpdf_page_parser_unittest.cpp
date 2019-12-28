// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/fpdf_page/pageint.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(fpdf_page_parser, PDF_FindKeyAbbreviation) {
  EXPECT_EQ(CFX_ByteStringC("BitsPerComponent"),
            PDF_FindKeyAbbreviationForTesting(CFX_ByteStringC("BPC")));
  EXPECT_EQ(CFX_ByteStringC("Width"),
            PDF_FindKeyAbbreviationForTesting(CFX_ByteStringC("W")));
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindKeyAbbreviationForTesting(CFX_ByteStringC("")));
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindKeyAbbreviationForTesting(CFX_ByteStringC("NoInList")));
  // Prefix should not match.
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindKeyAbbreviationForTesting(CFX_ByteStringC("WW")));
}

TEST(fpdf_page_parser, PDF_FindValueAbbreviation) {
  EXPECT_EQ(CFX_ByteStringC("DeviceGray"),
            PDF_FindValueAbbreviationForTesting(CFX_ByteStringC("G")));
  EXPECT_EQ(CFX_ByteStringC("DCTDecode"),
            PDF_FindValueAbbreviationForTesting(CFX_ByteStringC("DCT")));
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindValueAbbreviationForTesting(CFX_ByteStringC("")));
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindValueAbbreviationForTesting(CFX_ByteStringC("NoInList")));
  // Prefix should not match.
  EXPECT_EQ(CFX_ByteStringC(""),
            PDF_FindValueAbbreviationForTesting(CFX_ByteStringC("II")));
}
