// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_EVENT_H_
#define FPDFSDK_JAVASCRIPT_EVENT_H_

#include "fpdfsdk/javascript/JS_Define.h"

class event : public CJS_EmbedObj {
 public:
  event(CJS_Object* pJSObject);
  ~event() override;

 public:
  FX_BOOL change(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL changeEx(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL commitKey(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL fieldFull(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL keyDown(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL modifier(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL name(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL rc(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL richChange(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL richChangeEx(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError);
  FX_BOOL richValue(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL selEnd(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL selStart(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL shift(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL source(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL target(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL targetName(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
  FX_BOOL type(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL value(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  FX_BOOL willCommit(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError);
};

class CJS_Event : public CJS_Object {
 public:
  CJS_Event(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_Event() override {}

  DECLARE_JS_CLASS();
  JS_STATIC_PROP(change, event);
  JS_STATIC_PROP(changeEx, event);
  JS_STATIC_PROP(commitKey, event);
  JS_STATIC_PROP(fieldFull, event);
  JS_STATIC_PROP(keyDown, event);
  JS_STATIC_PROP(modifier, event);
  JS_STATIC_PROP(name, event);
  JS_STATIC_PROP(rc, event);
  JS_STATIC_PROP(richChange, event);
  JS_STATIC_PROP(richChangeEx, event);
  JS_STATIC_PROP(richValue, event);
  JS_STATIC_PROP(selEnd, event);
  JS_STATIC_PROP(selStart, event);
  JS_STATIC_PROP(shift, event);
  JS_STATIC_PROP(source, event);
  JS_STATIC_PROP(target, event);
  JS_STATIC_PROP(targetName, event);
  JS_STATIC_PROP(type, event);
  JS_STATIC_PROP(value, event);
  JS_STATIC_PROP(willCommit, event);
};

#endif  // FPDFSDK_JAVASCRIPT_EVENT_H_
