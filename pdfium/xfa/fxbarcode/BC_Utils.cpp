// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_basic.h"
#include "xfa/fxbarcode/utils.h"

FX_BOOL BC_FX_ByteString_Replace(CFX_ByteString& dst,
                                 uint32_t first,
                                 uint32_t last,
                                 int32_t count,
                                 FX_CHAR c) {
  if (first > last || count <= 0) {
    return FALSE;
  }
  dst.Delete(first, last - first);
  for (int32_t i = 0; i < count; i++) {
    dst.Insert(0, c);
  }
  return TRUE;
}
void BC_FX_ByteString_Append(CFX_ByteString& dst, int32_t count, FX_CHAR c) {
  for (int32_t i = 0; i < count; i++) {
    dst += c;
  }
}
void BC_FX_ByteString_Append(CFX_ByteString& dst, const CFX_ByteArray& ba) {
  for (int32_t i = 0; i < ba.GetSize(); i++) {
    dst += ba[i];
  }
}
