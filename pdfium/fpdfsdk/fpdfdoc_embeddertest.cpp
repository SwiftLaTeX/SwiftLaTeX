// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "core/fxcrt/include/fx_string.h"
#include "public/fpdf_doc.h"
#include "public/fpdf_edit.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

class FPDFDocEmbeddertest : public EmbedderTest {};

TEST_F(FPDFDocEmbeddertest, DestGetPageIndex) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // NULL FPDF_DEST case.
  EXPECT_EQ(0U, FPDFDest_GetPageIndex(document(), nullptr));

  // Page number directly in item from Dests NameTree.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1U, FPDFDest_GetPageIndex(document(), dest));

  // Page number via object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "Next");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1U, FPDFDest_GetPageIndex(document(), dest));

  // Page number directly in item from Dests dictionary.
  dest = FPDF_GetNamedDestByName(document(), "FirstAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(11U, FPDFDest_GetPageIndex(document(), dest));

  // Invalid object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "LastAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(0U, FPDFDest_GetPageIndex(document(), dest));
}

TEST_F(FPDFDocEmbeddertest, ActionGetFilePath) {
  EXPECT_TRUE(OpenDocument("launch_action.pdf"));

  FPDF_PAGE page = FPDF_LoadPage(document(), 0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);

  const char kExpectedResult[] = "test.pdf";
  const unsigned long kExpectedLength = sizeof(kExpectedResult);
  unsigned long bufsize = FPDFAction_GetFilePath(action, nullptr, 0);
  ASSERT_EQ(kExpectedLength, bufsize);

  char buf[kExpectedLength];
  EXPECT_EQ(bufsize, FPDFAction_GetFilePath(action, buf, bufsize));
  EXPECT_EQ(std::string(kExpectedResult), std::string(buf));

  FPDF_ClosePage(page);
}

TEST_F(FPDFDocEmbeddertest, NoBookmarks) {
  // Open a file with no bookmarks.
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // The non-existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  // The non-existent top-level bookmark has no children.
  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), nullptr));
}

TEST_F(FPDFDocEmbeddertest, Bookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // The existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  FPDF_BOOKMARK child = FPDFBookmark_GetFirstChild(document(), nullptr);
  EXPECT_TRUE(child);
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(CFX_WideString(L"A Good Beginning"),
            CFX_WideString::FromUTF16LE(buf, 16));

  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), child));

  FPDF_BOOKMARK sibling = FPDFBookmark_GetNextSibling(document(), child);
  EXPECT_TRUE(sibling);
  EXPECT_EQ(28u, FPDFBookmark_GetTitle(sibling, buf, sizeof(buf)));
  EXPECT_EQ(CFX_WideString(L"A Good Ending"),
            CFX_WideString::FromUTF16LE(buf, 13));

  EXPECT_EQ(nullptr, FPDFBookmark_GetNextSibling(document(), sibling));
}

TEST_F(FPDFDocEmbeddertest, FindBookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // Find the first one, based on its known title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"A Good Beginning");
  FPDF_BOOKMARK child = FPDFBookmark_Find(document(), title.get());
  EXPECT_TRUE(child);

  // Check that the string matches.
  unsigned short buf[128];
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(CFX_WideString(L"A Good Beginning"),
            CFX_WideString::FromUTF16LE(buf, 16));

  // Check that it is them same as the one returned by GetFirstChild.
  EXPECT_EQ(child, FPDFBookmark_GetFirstChild(document(), nullptr));

  // Try to find one using a non-existent title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> bad_title =
      GetFPDFWideString(L"A BAD Beginning");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), bad_title.get()));
}

// Check circular bookmarks will not cause infinite loop.
TEST_F(FPDFDocEmbeddertest, FindBookmarks_bug420) {
  // Open a file with circular bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks_circular.pdf"));

  // Try to find a title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"anything");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), title.get()));
}

TEST_F(FPDFDocEmbeddertest, DeletePage) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_EQ(1, FPDF_GetPageCount(document()));
  FPDFPage_Delete(document(), 0);
  EXPECT_EQ(0, FPDF_GetPageCount(document()));
}
