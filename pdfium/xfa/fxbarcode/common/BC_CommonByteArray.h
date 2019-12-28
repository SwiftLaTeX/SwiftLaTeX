// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_COMMON_BC_COMMONBYTEARRAY_H_
#define XFA_FXBARCODE_COMMON_BC_COMMONBYTEARRAY_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_CommonByteArray {
 public:
  CBC_CommonByteArray();
  explicit CBC_CommonByteArray(int32_t size);
  CBC_CommonByteArray(uint8_t* byteArray, int32_t size);
  virtual ~CBC_CommonByteArray();

  int32_t At(int32_t index);
  void Set(int32_t index, int32_t value);
  int32_t Size();
  FX_BOOL IsEmpty();
  void AppendByte(int32_t value);
  void Reserve(int32_t capacity);
  void Set(uint8_t* source, int32_t offset, int32_t count);
  void Set(CFX_ByteArray* source, int32_t offset, int32_t count);

 private:
  int32_t m_size;
  int32_t m_index;
  uint8_t* m_bytes;
};

#endif  // XFA_FXBARCODE_COMMON_BC_COMMONBYTEARRAY_H_
