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

#include "core/fxcrt/include/fx_memory.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderBitVector.h"
#include "xfa/fxbarcode/utils.h"

CBC_QRCoderBitVector::CBC_QRCoderBitVector() {
  m_sizeInBits = 0;
  m_size = 32;
}
void CBC_QRCoderBitVector::Init() {
  m_array = FX_Alloc(uint8_t, m_size);
}
CBC_QRCoderBitVector::~CBC_QRCoderBitVector() {
  FX_Free(m_array);
}
void CBC_QRCoderBitVector::Clear() {
  FX_Free(m_array);
  m_sizeInBits = 0;
  m_size = 32;
  m_array = FX_Alloc(uint8_t, m_size);
}
int32_t CBC_QRCoderBitVector::At(int32_t index, int32_t& e) {
  if (index < 0 || index >= m_sizeInBits) {
    e = BCExceptionBadIndexException;
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
  }
  int32_t value = m_array[index >> 3] & 0xff;
  return (value >> (7 - (index & 0x7))) & 1;
}
int32_t CBC_QRCoderBitVector::sizeInBytes() {
  return (m_sizeInBits + 7) >> 3;
}
int32_t CBC_QRCoderBitVector::Size() {
  return m_sizeInBits;
}
void CBC_QRCoderBitVector::AppendBit(int32_t bit, int32_t& e) {
  if (!(bit == 0 || bit == 1)) {
    e = BCExceptionBadValueException;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t numBitsInLastByte = m_sizeInBits & 0x7;
  if (numBitsInLastByte == 0) {
    AppendByte(0);
    m_sizeInBits -= 8;
  }
  m_array[m_sizeInBits >> 3] |= (bit << (7 - numBitsInLastByte));
  ++m_sizeInBits;
}
void CBC_QRCoderBitVector::AppendBits(int32_t value,
                                      int32_t numBits,
                                      int32_t& e) {
  if (numBits < 0 || numBits > 32) {
    e = BCExceptionBadNumBitsException;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t numBitsLeft = numBits;
  while (numBitsLeft > 0) {
    if ((m_sizeInBits & 0x7) == 0 && numBitsLeft >= 8) {
      int32_t newByte = (value >> (numBitsLeft - 8)) & 0xff;
      AppendByte(newByte);
      numBitsLeft -= 8;
    } else {
      int32_t bit = (value >> (numBitsLeft - 1)) & 1;
      AppendBit(bit, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      --numBitsLeft;
    }
  }
}
void CBC_QRCoderBitVector::AppendBitVector(CBC_QRCoderBitVector* bits,
                                           int32_t& e) {
  int32_t size = bits->Size();
  for (int32_t i = 0; i < size; i++) {
    int32_t num = bits->At(i, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
    AppendBit(num, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e)
  }
}
void CBC_QRCoderBitVector::XOR(CBC_QRCoderBitVector* other, int32_t& e) {
  if (m_sizeInBits != other->Size()) {
    e = BCExceptioncanNotOperatexorOperator;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t sizeInBytes = (m_sizeInBits + 7) >> 3;
  for (int32_t i = 0; i < sizeInBytes; ++i) {
    m_array[i] ^= (other->GetArray())[i];
  }
}
uint8_t* CBC_QRCoderBitVector::GetArray() {
  return m_array;
}
void CBC_QRCoderBitVector::AppendByte(int32_t value) {
  if ((m_sizeInBits >> 3) == m_size) {
    uint8_t* newArray = FX_Alloc(uint8_t, m_size << 1);
    FXSYS_memcpy(newArray, m_array, m_size);
    FX_Free(m_array);
    m_array = newArray;
    m_size = m_size << 1;
  }
  m_array[m_sizeInBits >> 3] = (uint8_t)value;
  m_sizeInBits += 8;
}
