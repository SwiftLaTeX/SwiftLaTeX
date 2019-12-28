// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_EMBEDDER_TEST_H_
#define TESTING_EMBEDDER_TEST_H_

#include <map>
#include <memory>
#include <string>

#include "public/fpdf_dataavail.h"
#include "public/fpdf_ext.h"
#include "public/fpdf_formfill.h"
#include "public/fpdfview.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

#ifdef PDF_ENABLE_V8
#include "v8/include/v8.h"
#endif  // PDF_ENABLE_v8

class TestLoader;

// This class is used to load a PDF document, and then run programatic
// API tests against it.
class EmbedderTest : public ::testing::Test,
                     public UNSUPPORT_INFO,
                     public IPDF_JSPLATFORM,
                     public FPDF_FORMFILLINFO {
 public:
  class Delegate {
   public:
    virtual ~Delegate() {}

    // Equivalent to UNSUPPORT_INFO::FSDK_UnSupport_Handler().
    virtual void UnsupportedHandler(int type) {}

    // Equivalent to IPDF_JSPLATFORM::app_alert().
    virtual int Alert(FPDF_WIDESTRING message,
                      FPDF_WIDESTRING title,
                      int type,
                      int icon) {
      return 0;
    }

    // Equivalent to FPDF_FORMFILLINFO::FFI_SetTimer().
    virtual int SetTimer(int msecs, TimerCallback fn) { return 0; }

    // Equivalent to FPDF_FORMFILLINFO::FFI_KillTimer().
    virtual void KillTimer(int id) {}

    // Equivalent to FPDF_FORMFILLINFO::FFI_GetPage().
    virtual FPDF_PAGE GetPage(FPDF_FORMHANDLE form_handle,
                              FPDF_DOCUMENT document,
                              int page_index);

   private:
    std::map<int, FPDF_PAGE> m_pageMap;
  };

  EmbedderTest();
  virtual ~EmbedderTest();

  void SetUp() override;
  void TearDown() override;

#ifdef PDF_ENABLE_V8
  // Call before SetUp to pass shared isolate, otherwise PDFium creates one.
  void SetExternalIsolate(void* isolate) {
    external_isolate_ = static_cast<v8::Isolate*>(isolate);
  }
#endif  // PDF_ENABLE_V8

  void SetDelegate(Delegate* delegate) {
    delegate_ = delegate ? delegate : default_delegate_.get();
  }

  FPDF_DOCUMENT document() { return document_; }
  FPDF_FORMHANDLE form_handle() { return form_handle_; }

  // Create an empty document, and its form fill environment. Returns true
  // on success or false on failure.
  virtual bool CreateEmptyDocument();

  // Open the document specified by |filename|, and create its form fill
  // environment, or return false on failure.
  // The filename is relative to the test data directory where we store all the
  // test files.
  virtual bool OpenDocument(const std::string& filename,
                            const char* password = nullptr,
                            bool must_linearize = false);

  // Perform JavaScript actions that are to run at document open time.
  virtual void DoOpenActions();

  // Determine the page numbers present in the document.
  virtual int GetFirstPageNum();
  virtual int GetPageCount();

  // Load a specific page of the open document.
  virtual FPDF_PAGE LoadPage(int page_number);

  // Load a specific page of the open document using delegate_->GetPage.
  // delegate_->GetPage also caches loaded page.
  virtual FPDF_PAGE LoadAndCachePage(int page_number);

  // Convert a loaded page into a bitmap.
  virtual FPDF_BITMAP RenderPage(FPDF_PAGE page);

  // Relese the resources obtained from LoadPage(). Further use of |page|
  // is prohibited after this call is made.
  virtual void UnloadPage(FPDF_PAGE page);

 protected:
  void SetupFormFillEnvironment();

  Delegate* delegate_;
  std::unique_ptr<Delegate> default_delegate_;
  FPDF_DOCUMENT document_;
  FPDF_FORMHANDLE form_handle_;
  FPDF_AVAIL avail_;
  FX_DOWNLOADHINTS hints_;
  FPDF_FILEACCESS file_access_;
  FX_FILEAVAIL file_avail_;
#ifdef PDF_ENABLE_V8
  v8::Platform* platform_;
#endif  // PDF_ENABLE_V8
  void* external_isolate_;
  TestLoader* loader_;
  size_t file_length_;
  std::unique_ptr<char, pdfium::FreeDeleter> file_contents_;

 private:
  static void UnsupportedHandlerTrampoline(UNSUPPORT_INFO*, int type);
  static int AlertTrampoline(IPDF_JSPLATFORM* plaform,
                             FPDF_WIDESTRING message,
                             FPDF_WIDESTRING title,
                             int type,
                             int icon);
  static int SetTimerTrampoline(FPDF_FORMFILLINFO* info,
                                int msecs,
                                TimerCallback fn);
  static void KillTimerTrampoline(FPDF_FORMFILLINFO* info, int id);
  static FPDF_PAGE GetPageTrampoline(FPDF_FORMFILLINFO* info,
                                     FPDF_DOCUMENT document,
                                     int page_index);
};

#endif  // TESTING_EMBEDDER_TEST_H_
