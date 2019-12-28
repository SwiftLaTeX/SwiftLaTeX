// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_BC_UTILCODINGCONVERT_H_
#define XFA_FXBARCODE_BC_UTILCODINGCONVERT_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_UtilCodingConvert {
 public:
  CBC_UtilCodingConvert();
  virtual ~CBC_UtilCodingConvert();
  static void UnicodeToLocale(const CFX_WideString& source,
                              CFX_ByteString& result);
  static void LocaleToUtf8(const CFX_ByteString& source,
                           CFX_ByteString& result);
  static void LocaleToUtf8(const CFX_ByteString& source, CFX_ByteArray& result);
  static void Utf8ToLocale(const CFX_ByteArray& source, CFX_ByteString& result);
  static void Utf8ToLocale(const uint8_t* source,
                           int32_t count,
                           CFX_ByteString& result);
  static void UnicodeToUTF8(const CFX_WideString& source,
                            CFX_ByteString& result);
};

#endif  // XFA_FXBARCODE_BC_UTILCODINGCONVERT_H_
