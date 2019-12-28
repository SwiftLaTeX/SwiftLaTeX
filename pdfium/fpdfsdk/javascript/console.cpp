// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/console.h"

#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"

BEGIN_JS_STATIC_CONST(CJS_Console)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Console)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Console)
JS_STATIC_METHOD_ENTRY(clear)
JS_STATIC_METHOD_ENTRY(hide)
JS_STATIC_METHOD_ENTRY(println)
JS_STATIC_METHOD_ENTRY(show)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_Console, console)

console::console(CJS_Object* pJSObject) : CJS_EmbedObj(pJSObject) {}

console::~console() {}

FX_BOOL console::clear(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL console::hide(IJS_Context* cc,
                      const std::vector<CJS_Value>& params,
                      CJS_Value& vRet,
                      CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL console::println(IJS_Context* cc,
                         const std::vector<CJS_Value>& params,
                         CJS_Value& vRet,
                         CFX_WideString& sError) {
  if (params.size() < 1) {
    return FALSE;
  }
  return TRUE;
}

FX_BOOL console::show(IJS_Context* cc,
                      const std::vector<CJS_Value>& params,
                      CJS_Value& vRet,
                      CFX_WideString& sError) {
  return TRUE;
}
