// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include <string>

#include "core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_stream.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/utils/path_service.h"

TEST(cpdf_syntax_parser, ReadHexString) {
  {
    // Empty string.
    uint8_t data[] = "";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 0, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(0, parser.SavePos());
  }

  {
    // Blank string.
    uint8_t data[] = "  ";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 2, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(2, parser.SavePos());
  }

  {
    // Skips unknown characters.
    uint8_t data[] = "z12b";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 4, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x12\xb0", parser.ReadHexString());
    EXPECT_EQ(4, parser.SavePos());
  }

  {
    // Skips unknown characters.
    uint8_t data[] = "*<&*#$^&@1";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 10, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x10", parser.ReadHexString());
    EXPECT_EQ(10, parser.SavePos());
  }

  {
    // Skips unknown characters.
    uint8_t data[] = "\x80zab";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 4, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\xab", parser.ReadHexString());
    EXPECT_EQ(4, parser.SavePos());
  }

  {
    // Skips unknown characters.
    uint8_t data[] = "\xffzab";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 4, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\xab", parser.ReadHexString());
    EXPECT_EQ(4, parser.SavePos());
  }

  {
    // Regular conversion.
    uint8_t data[] = "1A2b>abcd";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 9, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x1a\x2b", parser.ReadHexString());
    EXPECT_EQ(5, parser.SavePos());
  }

  {
    // Position out of bounds.
    uint8_t data[] = "12ab>";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 5, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    parser.RestorePos(5);
    EXPECT_EQ("", parser.ReadHexString());

    parser.RestorePos(6);
    EXPECT_EQ("", parser.ReadHexString());

    parser.RestorePos(-1);
    EXPECT_EQ("", parser.ReadHexString());

    parser.RestorePos(std::numeric_limits<FX_FILESIZE>::max());
    EXPECT_EQ("", parser.ReadHexString());

    // Check string still parses when set to 0.
    parser.RestorePos(0);
    EXPECT_EQ("\x12\xab", parser.ReadHexString());
  }

  {
    // Missing ending >.
    uint8_t data[] = "1A2b";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 4, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x1a\x2b", parser.ReadHexString());
    EXPECT_EQ(4, parser.SavePos());
  }

  {
    // Missing ending >.
    uint8_t data[] = "12abz";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 5, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x12\xab", parser.ReadHexString());
    EXPECT_EQ(5, parser.SavePos());
  }

  {
    // Uneven number of bytes.
    uint8_t data[] = "1A2>asdf";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 8, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x1a\x20", parser.ReadHexString());
    EXPECT_EQ(4, parser.SavePos());
  }

  {
    // Uneven number of bytes.
    uint8_t data[] = "1A2zasdf";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 8, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("\x1a\x2a\xdf", parser.ReadHexString());
    EXPECT_EQ(8, parser.SavePos());
  }

  {
    // Just ending character.
    uint8_t data[] = ">";
    ScopedFileStream stream(FX_CreateMemoryStream(data, 1, FALSE));

    CPDF_SyntaxParser parser;
    parser.InitParser(stream.get(), 0);
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(1, parser.SavePos());
  }
}
