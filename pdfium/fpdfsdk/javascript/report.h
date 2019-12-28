// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_REPORT_H_
#define FPDFSDK_JAVASCRIPT_REPORT_H_

#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"

class Report : public CJS_EmbedObj {
 public:
  Report(CJS_Object* pJSObject);
  ~Report() override;

 public:
  FX_BOOL save(IJS_Context* cc,
               const std::vector<CJS_Value>& params,
               CJS_Value& vRet,
               CFX_WideString& sError);
  FX_BOOL writeText(IJS_Context* cc,
                    const std::vector<CJS_Value>& params,
                    CJS_Value& vRet,
                    CFX_WideString& sError);
};

class CJS_Report : public CJS_Object {
 public:
  CJS_Report(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_Report() override {}

  DECLARE_JS_CLASS();

  JS_STATIC_METHOD(save, Report)
  JS_STATIC_METHOD(writeText, Report);
};

#endif  // FPDFSDK_JAVASCRIPT_REPORT_H_
