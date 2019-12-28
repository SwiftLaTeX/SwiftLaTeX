// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fde/css/fde_cssdatatable.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(FDE_ParseCSSColor, HexEncodingParsing) {
  FX_ARGB color;

  // Length value invalid.
  EXPECT_FALSE(FDE_ParseCSSColor(L"#000", 3, color));
  EXPECT_FALSE(FDE_ParseCSSColor(L"#000000", 5, color));
  EXPECT_FALSE(FDE_ParseCSSColor(L"#000000", 8, color));

  // Invalid characters
  EXPECT_TRUE(FDE_ParseCSSColor(L"#zxytlm", 7, color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(FDE_ParseCSSColor(L"#000", 4, color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(FDE_ParseCSSColor(L"#FFF", 4, color));
  EXPECT_EQ(255, FXARGB_R(color));
  EXPECT_EQ(255, FXARGB_G(color));
  EXPECT_EQ(255, FXARGB_B(color));

  EXPECT_TRUE(FDE_ParseCSSColor(L"#F0F0F0", 7, color));
  EXPECT_EQ(240, FXARGB_R(color));
  EXPECT_EQ(240, FXARGB_G(color));
  EXPECT_EQ(240, FXARGB_B(color));

  // Upper and lower case characters.
  EXPECT_TRUE(FDE_ParseCSSColor(L"#1b2F3c", 7, color));
  EXPECT_EQ(27, FXARGB_R(color));
  EXPECT_EQ(47, FXARGB_G(color));
  EXPECT_EQ(60, FXARGB_B(color));
}

TEST(FDE_ParseCSSColor, RGBEncodingParsing) {
  FX_ARGB color;

  // Invalid input for rgb() syntax.
  EXPECT_FALSE(FDE_ParseCSSColor(L"blahblahblah", 11, color));

  EXPECT_TRUE(FDE_ParseCSSColor(L"rgb(0, 0, 0)", 12, color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(FDE_ParseCSSColor(L"rgb(128,255,48)", 15, color));
  EXPECT_EQ(128, FXARGB_R(color));
  EXPECT_EQ(255, FXARGB_G(color));
  EXPECT_EQ(48, FXARGB_B(color));
}
