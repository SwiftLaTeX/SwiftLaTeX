// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_COMMON_BC_COMMONBITARRAY_H_
#define XFA_FXBARCODE_COMMON_BC_COMMONBITARRAY_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_CommonBitArray {
 public:
  explicit CBC_CommonBitArray(CBC_CommonBitArray* array);
  explicit CBC_CommonBitArray(int32_t size);
  CBC_CommonBitArray();
  virtual ~CBC_CommonBitArray();

  int32_t GetSize();
  CFX_Int32Array& GetBits();
  int32_t GetSizeInBytes();
  FX_BOOL Get(int32_t i);
  void Set(int32_t i);
  void Flip(int32_t i);
  void SetBulk(int32_t i, int32_t newBits);
  FX_BOOL IsRange(int32_t start, int32_t end, FX_BOOL value, int32_t& e);
  int32_t* GetBitArray();
  void Reverse();
  void Clear();

 private:
  int32_t m_size;
  CFX_Int32Array m_bits;
};

#endif  // XFA_FXBARCODE_COMMON_BC_COMMONBITARRAY_H_
