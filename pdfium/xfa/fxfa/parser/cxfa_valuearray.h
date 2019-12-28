// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_VALUEARRAY_H_
#define XFA_FXFA_PARSER_CXFA_VALUEARRAY_H_

#include "fxjs/include/cfxjse_value.h"
#include "xfa/fxfa/include/fxfa.h"

class CXFA_ValueArray : public CFX_ArrayTemplate<CFXJSE_Value*> {
 public:
  explicit CXFA_ValueArray(v8::Isolate* pIsolate);
  ~CXFA_ValueArray();

  void GetAttributeObject(CXFA_ObjArray& objArray);

  v8::Isolate* const m_pIsolate;
};

#endif  // XFA_FXFA_PARSER_CXFA_VALUEARRAY_H_
