// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_JS_EMBEDDER_TEST_H_
#define TESTING_JS_EMBEDDER_TEST_H_

#include <memory>
#include <vector>

#include "fxjs/include/fxjs_v8.h"
#include "testing/embedder_test.h"

class JSEmbedderTest : public EmbedderTest {
 public:
  JSEmbedderTest();
  ~JSEmbedderTest() override;

  void SetUp() override;
  void TearDown() override;

  v8::Isolate* isolate();
  v8::Local<v8::Context> GetV8Context();

 private:
  std::unique_ptr<FXJS_ArrayBufferAllocator> m_pArrayBufferAllocator;
  v8::Isolate* m_pIsolate;
  v8::Global<v8::Context> m_pPersistentContext;
  std::vector<v8::Global<v8::Object>*> m_StaticObjects;
};

#endif  // TESTING_JS_EMBEDDER_TEST_H_
