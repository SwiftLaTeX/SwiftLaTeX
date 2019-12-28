// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_doc.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_app.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#endif  // PDF_ENABLE_XFA

class CPDF_TestDocument : public CPDF_Document {
 public:
  CPDF_TestDocument() : CPDF_Document(nullptr) {}

  void SetRoot(CPDF_Dictionary* root) { m_pRootDict = root; }
  CPDF_IndirectObjectHolder* GetHolder() { return this; }
};

#ifdef PDF_ENABLE_XFA
class CPDF_TestXFADocument : public CPDFXFA_Document {
 public:
  CPDF_TestXFADocument()
      : CPDFXFA_Document(new CPDF_TestDocument(), CPDFXFA_App::GetInstance()) {}

  void SetRoot(CPDF_Dictionary* root) {
    reinterpret_cast<CPDF_TestDocument*>(GetPDFDoc())->SetRoot(root);
  }

  CPDF_IndirectObjectHolder* GetHolder() { return GetPDFDoc(); }
};
using CPDF_TestPdfDocument = CPDF_TestXFADocument;
#else   // PDF_ENABLE_XFA
using CPDF_TestPdfDocument = CPDF_TestDocument;
#endif  // PDF_ENABLE_XFA

class PDFDocTest : public testing::Test {
 public:
  struct DictObjInfo {
    uint32_t num;
    CPDF_Dictionary* obj;
  };

  void SetUp() override {
    // We don't need page module or render module, but
    // initialize them to keep the code sane.
    CPDF_ModuleMgr::Create();
    CPDF_ModuleMgr* module_mgr = CPDF_ModuleMgr::Get();
    module_mgr->InitPageModule();

    m_pDoc.reset(new CPDF_TestPdfDocument());
    m_pIndirectObjs = m_pDoc->GetHolder();
    // Setup the root directory.
    m_pRootObj.reset(new CPDF_Dictionary());
    m_pDoc->SetRoot(m_pRootObj.get());
  }

  void TearDown() override {
    m_pRootObj.reset();
    m_pIndirectObjs = nullptr;
    m_pDoc.reset();
    CPDF_ModuleMgr::Destroy();
  }

  std::vector<DictObjInfo> CreateDictObjs(int num) {
    std::vector<DictObjInfo> info;
    for (int i = 0; i < num; ++i) {
      // Objects created will be released by the document.
      CPDF_Dictionary* obj(new CPDF_Dictionary());
      m_pIndirectObjs->AddIndirectObject(obj);
      info.push_back({obj->GetObjNum(), obj});
    }
    return info;
  }

 protected:
  std::unique_ptr<CPDF_TestPdfDocument> m_pDoc;
  CPDF_IndirectObjectHolder* m_pIndirectObjs;
  std::unique_ptr<CPDF_Dictionary, ReleaseDeleter<CPDF_Dictionary>> m_pRootObj;
};

TEST_F(PDFDocTest, FindBookmark) {
  {
    // No bookmark information.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
        GetFPDFWideString(L"");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    title = GetFPDFWideString(L"Preface");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));
  }
  {
    // Empty bookmark tree.
    m_pRootObj->SetAt("Outlines", new CPDF_Dictionary());
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
        GetFPDFWideString(L"");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    title = GetFPDFWideString(L"Preface");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));
  }
  {
    // Check on a regular bookmark tree.
    auto bookmarks = CreateDictObjs(3);

    bookmarks[1].obj->SetAt("Title", new CPDF_String(L"Chapter 1"));
    bookmarks[1].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[1].obj->SetAt(
        "Next", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    bookmarks[2].obj->SetAt("Title", new CPDF_String(L"Chapter 2"));
    bookmarks[2].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[2].obj->SetAt(
        "Prev", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));

    bookmarks[0].obj->SetAt("Type", new CPDF_Name("Outlines"));
    bookmarks[0].obj->SetAt("Count", new CPDF_Number(2));
    bookmarks[0].obj->SetAt(
        "First", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));
    bookmarks[0].obj->SetAt(
        "Last", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    m_pRootObj->SetAt("Outlines",
                      new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));

    // Title with no match.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
        GetFPDFWideString(L"Chapter 3");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    // Title with partial match only.
    title = GetFPDFWideString(L"Chapter");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    // Title with a match.
    title = GetFPDFWideString(L"Chapter 2");
    EXPECT_EQ(bookmarks[2].obj, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    // Title match is case insensitive.
    title = GetFPDFWideString(L"cHaPter 2");
    EXPECT_EQ(bookmarks[2].obj, FPDFBookmark_Find(m_pDoc.get(), title.get()));
  }
  {
    // Circular bookmarks in depth.
    auto bookmarks = CreateDictObjs(3);

    bookmarks[1].obj->SetAt("Title", new CPDF_String(L"Chapter 1"));
    bookmarks[1].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[1].obj->SetAt(
        "First", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    bookmarks[2].obj->SetAt("Title", new CPDF_String(L"Chapter 2"));
    bookmarks[2].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));
    bookmarks[2].obj->SetAt(
        "First", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));

    bookmarks[0].obj->SetAt("Type", new CPDF_Name("Outlines"));
    bookmarks[0].obj->SetAt("Count", new CPDF_Number(2));
    bookmarks[0].obj->SetAt(
        "First", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));
    bookmarks[0].obj->SetAt(
        "Last", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    m_pRootObj->SetAt("Outlines",
                      new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));

    // Title with no match.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
        GetFPDFWideString(L"Chapter 3");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    // Title with a match.
    title = GetFPDFWideString(L"Chapter 2");
    EXPECT_EQ(bookmarks[2].obj, FPDFBookmark_Find(m_pDoc.get(), title.get()));
  }
  {
    // Circular bookmarks in breadth.
    auto bookmarks = CreateDictObjs(4);

    bookmarks[1].obj->SetAt("Title", new CPDF_String(L"Chapter 1"));
    bookmarks[1].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[1].obj->SetAt(
        "Next", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    bookmarks[2].obj->SetAt("Title", new CPDF_String(L"Chapter 2"));
    bookmarks[2].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[2].obj->SetAt(
        "Next", new CPDF_Reference(m_pIndirectObjs, bookmarks[3].num));

    bookmarks[3].obj->SetAt("Title", new CPDF_String(L"Chapter 3"));
    bookmarks[3].obj->SetAt(
        "Parent", new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));
    bookmarks[3].obj->SetAt(
        "Next", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));

    bookmarks[0].obj->SetAt("Type", new CPDF_Name("Outlines"));
    bookmarks[0].obj->SetAt("Count", new CPDF_Number(2));
    bookmarks[0].obj->SetAt(
        "First", new CPDF_Reference(m_pIndirectObjs, bookmarks[1].num));
    bookmarks[0].obj->SetAt(
        "Last", new CPDF_Reference(m_pIndirectObjs, bookmarks[2].num));

    m_pRootObj->SetAt("Outlines",
                      new CPDF_Reference(m_pIndirectObjs, bookmarks[0].num));

    // Title with no match.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
        GetFPDFWideString(L"Chapter 8");
    EXPECT_EQ(nullptr, FPDFBookmark_Find(m_pDoc.get(), title.get()));

    // Title with a match.
    title = GetFPDFWideString(L"Chapter 3");
    EXPECT_EQ(bookmarks[3].obj, FPDFBookmark_Find(m_pDoc.get(), title.get()));
  }
}
