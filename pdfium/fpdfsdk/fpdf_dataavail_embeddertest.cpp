// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_doc.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class FPDFDataAvailEmbeddertest : public EmbedderTest {};

TEST_F(FPDFDataAvailEmbeddertest, TrailerUnterminated) {
  // Document must load without crashing but is too malformed to be available.
  EXPECT_FALSE(OpenDocument("trailer_unterminated.pdf"));
  EXPECT_FALSE(FPDFAvail_IsDocAvail(avail_, &hints_));
}

TEST_F(FPDFDataAvailEmbeddertest, TrailerAsHexstring) {
  // Document must load without crashing but is too malformed to be available.
  EXPECT_FALSE(OpenDocument("trailer_as_hexstring.pdf"));
  EXPECT_FALSE(FPDFAvail_IsDocAvail(avail_, &hints_));
}
