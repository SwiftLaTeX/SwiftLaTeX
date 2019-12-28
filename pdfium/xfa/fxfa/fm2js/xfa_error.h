// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FM2JS_XFA_ERROR_H_
#define XFA_FXFA_FM2JS_XFA_ERROR_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

extern const FX_WCHAR kFMErrUnsupportedChar[];
extern const FX_WCHAR kFMErrBadSuffixNumber[];
extern const FX_WCHAR kFMErrExpectedIdentifier[];
extern const FX_WCHAR kFMErrExpectedToken[];
extern const FX_WCHAR kFMErrExpectedEndIf[];
extern const FX_WCHAR kFMErrUnexpectedExpression[];
extern const FX_WCHAR kFMErrExpectedNonEmptyExpression[];

class CXFA_FMErrorInfo {
 public:
  CXFA_FMErrorInfo() : linenum(0) {}
  ~CXFA_FMErrorInfo() {}
  uint32_t linenum;
  CFX_WideString message;
};

#endif  // XFA_FXFA_FM2JS_XFA_ERROR_H_
