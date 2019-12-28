// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/js_embedder_test.h"

namespace {

const double kExpected0 = 6.0;
const double kExpected1 = 7.0;
const double kExpected2 = 8.0;

const wchar_t kScript0[] = L"fred = 6";
const wchar_t kScript1[] = L"fred = 7";
const wchar_t kScript2[] = L"fred = 8";

}  // namespace

class FXJSV8EmbedderTest : public JSEmbedderTest {
 public:
  void ExecuteInCurrentContext(const CFX_WideString& script) {
    FXJSErr error;
    int sts = FXJS_Execute(isolate(), script, &error);
    EXPECT_EQ(0, sts);
  }
  void CheckAssignmentInCurrentContext(double expected) {
    v8::Local<v8::Object> This = FXJS_GetThisObj(isolate());
    v8::Local<v8::Value> fred = FXJS_GetObjectElement(isolate(), This, L"fred");
    EXPECT_TRUE(fred->IsNumber());
    EXPECT_EQ(expected, fred->ToNumber(isolate()->GetCurrentContext())
                            .ToLocalChecked()
                            ->Value());
  }
};

TEST_F(FXJSV8EmbedderTest, Getters) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());
  v8::Context::Scope context_scope(GetV8Context());

  ExecuteInCurrentContext(CFX_WideString(kScript1));
  CheckAssignmentInCurrentContext(kExpected1);
}

TEST_F(FXJSV8EmbedderTest, MultipleRutimes) {
  v8::Isolate::Scope isolate_scope(isolate());
  v8::HandleScope handle_scope(isolate());

  v8::Global<v8::Context> global_context1;
  std::vector<v8::Global<v8::Object>*> static_objects1;
  FXJS_InitializeRuntime(isolate(), nullptr, &global_context1,
                         &static_objects1);

  v8::Global<v8::Context> global_context2;
  std::vector<v8::Global<v8::Object>*> static_objects2;
  FXJS_InitializeRuntime(isolate(), nullptr, &global_context2,
                         &static_objects2);

  v8::Context::Scope context_scope(GetV8Context());
  ExecuteInCurrentContext(CFX_WideString(kScript0));
  CheckAssignmentInCurrentContext(kExpected0);

  {
    v8::Local<v8::Context> context1 =
        v8::Local<v8::Context>::New(isolate(), global_context1);
    v8::Context::Scope context_scope1(context1);
    ExecuteInCurrentContext(CFX_WideString(kScript1));
    CheckAssignmentInCurrentContext(kExpected1);
  }
  FXJS_ReleaseRuntime(isolate(), &global_context1, &static_objects1);

  {
    v8::Local<v8::Context> context2 =
        v8::Local<v8::Context>::New(isolate(), global_context2);
    v8::Context::Scope context_scope2(context2);
    ExecuteInCurrentContext(CFX_WideString(kScript2));
    CheckAssignmentInCurrentContext(kExpected2);
  }
  FXJS_ReleaseRuntime(isolate(), &global_context2, &static_objects2);

  CheckAssignmentInCurrentContext(kExpected0);
}
