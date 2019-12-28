// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_JAVASCRIPT_ICON_H_
#define FPDFSDK_JAVASCRIPT_ICON_H_

#include "fpdfsdk/javascript/JS_Define.h"

class Icon : public CJS_EmbedObj {
 public:
  Icon(CJS_Object* pJSObject);
  ~Icon() override;

  FX_BOOL name(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError);
  void SetStream(CPDF_Stream* pIconStream);
  CPDF_Stream* GetStream();
  void SetIconName(CFX_WideString name);
  CFX_WideString GetIconName();

 private:
  CPDF_Stream* m_pIconStream;
  CFX_WideString m_swIconName;
};

class CJS_Icon : public CJS_Object {
 public:
  CJS_Icon(v8::Local<v8::Object> pObject) : CJS_Object(pObject) {}
  ~CJS_Icon() override {}

  DECLARE_JS_CLASS();
  JS_STATIC_PROP(name, Icon);
};

#endif  // FPDFSDK_JAVASCRIPT_ICON_H_
