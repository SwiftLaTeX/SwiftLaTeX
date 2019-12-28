// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class FPDFPageFuncEmbeddertest : public EmbedderTest {};

TEST_F(FPDFPageFuncEmbeddertest, Bug_551460) {
  // Should not crash under ASan.
  // Tests that the number of inputs is not simply calculated from the domain
  // and trusted. The number of inputs has to be 1.
  EXPECT_TRUE(OpenDocument("bug_551460.pdf"));
  FPDF_PAGE page = LoadPage(0);
  EXPECT_NE(nullptr, page);
  FPDF_BITMAP bitmap = RenderPage(page);
  FPDFBitmap_Destroy(bitmap);
  UnloadPage(page);
}
