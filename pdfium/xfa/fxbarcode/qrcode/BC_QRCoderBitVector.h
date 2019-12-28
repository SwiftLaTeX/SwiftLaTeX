// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODERBITVECTOR_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODERBITVECTOR_H_

#include <stdint.h>

class CBC_QRCoderBitVector {
 private:
  int32_t m_sizeInBits;
  uint8_t* m_array;
  int32_t m_size;

  void AppendByte(int32_t value);

 public:
  CBC_QRCoderBitVector();
  virtual ~CBC_QRCoderBitVector();
  int32_t At(int32_t index, int32_t& e);
  int32_t Size();
  int32_t sizeInBytes();
  void AppendBit(int32_t bit, int32_t& e);
  void AppendBits(int32_t value, int32_t numBits, int32_t& e);
  void AppendBitVector(CBC_QRCoderBitVector* bits, int32_t& e);
  void XOR(CBC_QRCoderBitVector* other, int32_t& e);
  uint8_t* GetArray();
  void Clear();
  virtual void Init();
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODERBITVECTOR_H_
