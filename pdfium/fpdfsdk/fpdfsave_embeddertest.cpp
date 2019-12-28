// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string.h>

#include "core/fxcrt/include/fx_string.h"
#include "public/fpdf_save.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gmock/include/gmock/gmock-matchers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

class FPDFSaveEmbedderTest : public EmbedderTest, public TestSaver {};

TEST_F(FPDFSaveEmbedderTest, SaveSimpleDoc) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  EXPECT_THAT(GetString(), testing::StartsWith("%PDF-1.7\r\n"));
  EXPECT_EQ(843u, GetString().length());
}

TEST_F(FPDFSaveEmbedderTest, SaveSimpleDocWithVersion) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_TRUE(FPDF_SaveWithVersion(document(), this, 0, 14));
  EXPECT_THAT(GetString(), testing::StartsWith("%PDF-1.4\r\n"));
  EXPECT_EQ(843u, GetString().length());
}

TEST_F(FPDFSaveEmbedderTest, SaveSimpleDocWithBadVersion) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_TRUE(FPDF_SaveWithVersion(document(), this, 0, -1));
  EXPECT_THAT(GetString(), testing::StartsWith("%PDF-1.7\r\n"));

  ClearString();
  EXPECT_TRUE(FPDF_SaveWithVersion(document(), this, 0, 0));
  EXPECT_THAT(GetString(), testing::StartsWith("%PDF-1.7\r\n"));

  ClearString();
  EXPECT_TRUE(FPDF_SaveWithVersion(document(), this, 0, 18));
  EXPECT_THAT(GetString(), testing::StartsWith("%PDF-1.7\r\n"));
}

TEST_F(FPDFSaveEmbedderTest, BUG_342) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  EXPECT_THAT(GetString(), testing::HasSubstr("0000000000 65535 f\r\n"));
  EXPECT_THAT(GetString(),
              testing::Not(testing::HasSubstr("0000000000 65536 f\r\n")));
}
