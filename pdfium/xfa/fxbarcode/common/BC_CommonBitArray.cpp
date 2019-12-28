// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xfa/fxbarcode/common/BC_CommonBitArray.h"
#include "xfa/fxbarcode/utils.h"

CBC_CommonBitArray::CBC_CommonBitArray(CBC_CommonBitArray* array) {
  m_size = array->GetSize();
  m_bits.Copy(array->GetBits());
}
CBC_CommonBitArray::CBC_CommonBitArray() {
  m_bits.SetSize(1);
  m_size = 0;
}
CBC_CommonBitArray::CBC_CommonBitArray(int32_t size) {
  m_bits.SetSize((size + 31) >> 5);
  m_size = size;
}
CBC_CommonBitArray::~CBC_CommonBitArray() {
  m_size = 0;
}
int32_t CBC_CommonBitArray::GetSize() {
  return m_size;
}
CFX_Int32Array& CBC_CommonBitArray::GetBits() {
  return m_bits;
}
int32_t CBC_CommonBitArray::GetSizeInBytes() {
  return (m_size + 7) >> 3;
}
FX_BOOL CBC_CommonBitArray::Get(int32_t i) {
  return (m_bits[i >> 5] & (1 << (i & 0x1f))) != 0;
}
void CBC_CommonBitArray::Set(int32_t i) {
  m_bits[i >> 5] |= 1 << (i & 0x1F);
}
void CBC_CommonBitArray::Flip(int32_t i) {
  m_bits[i >> 5] ^= 1 << (i & 0x1F);
}
void CBC_CommonBitArray::SetBulk(int32_t i, int32_t newBits) {
  m_bits[i >> 5] = newBits;
}
void CBC_CommonBitArray::Clear() {
  FXSYS_memset(&m_bits[0], 0x00, m_bits.GetSize() * sizeof(int32_t));
}
FX_BOOL CBC_CommonBitArray::IsRange(int32_t start,
                                    int32_t end,
                                    FX_BOOL value,
                                    int32_t& e) {
  if (end < start) {
    e = BCExceptionEndLessThanStart;
    return FALSE;
  }
  if (end == start) {
    return TRUE;
  }
  end--;
  int32_t firstInt = start >> 5;
  int32_t lastInt = end >> 5;
  int32_t i;
  for (i = firstInt; i <= lastInt; i++) {
    int32_t firstBit = i > firstInt ? 0 : start & 0x1F;
    int32_t lastBit = i < lastInt ? 31 : end & 0x1F;
    int32_t mask;
    if (firstBit == 0 && lastBit == 31) {
      mask = -1;
    } else {
      mask = 0;
      for (int32_t j = firstBit; j <= lastBit; j++) {
        mask |= 1 << j;
      }
    }
    if ((m_bits[i] & mask) != (value ? mask : 0)) {
      return FALSE;
    }
  }
  return TRUE;
}
int32_t* CBC_CommonBitArray::GetBitArray() {
  return &m_bits[0];
}
void CBC_CommonBitArray::Reverse() {
  int32_t* newBits = FX_Alloc(int32_t, m_bits.GetSize());
  FXSYS_memset(newBits, 0x00, m_bits.GetSize() * sizeof(int32_t));
  int32_t size = m_size;
  int32_t i;
  for (i = 0; i < size; i++) {
    if (Get(size - i - 1)) {
      newBits[i >> 5] |= 1 << (i & 0x1F);
    }
  }
  FXSYS_memcpy(&m_bits[0], newBits, m_bits.GetSize() * sizeof(int32_t));
  FX_Free(newBits);
}
