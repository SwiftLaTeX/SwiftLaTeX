// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/report.h"

#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"

BEGIN_JS_STATIC_CONST(CJS_Report)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Report)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Report)
JS_STATIC_METHOD_ENTRY(save)
JS_STATIC_METHOD_ENTRY(writeText)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_Report, Report)

Report::Report(CJS_Object* pJSObject) : CJS_EmbedObj(pJSObject) {}

Report::~Report() {}

FX_BOOL Report::writeText(IJS_Context* cc,
                          const std::vector<CJS_Value>& params,
                          CJS_Value& vRet,
                          CFX_WideString& sError) {
  // Unsafe, not supported.
  return TRUE;
}

FX_BOOL Report::save(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError) {
  // Unsafe, not supported.
  return TRUE;
}
