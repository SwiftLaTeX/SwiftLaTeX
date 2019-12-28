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

#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomonGF256Poly.h"

CBC_ReedSolomonGF256* CBC_ReedSolomonGF256::QRCodeFild = nullptr;
CBC_ReedSolomonGF256* CBC_ReedSolomonGF256::DataMatrixField = nullptr;
void CBC_ReedSolomonGF256::Initialize() {
  QRCodeFild = new CBC_ReedSolomonGF256(0x011D);
  QRCodeFild->Init();
  DataMatrixField = new CBC_ReedSolomonGF256(0x012D);
  DataMatrixField->Init();
}

void CBC_ReedSolomonGF256::Finalize() {
  delete QRCodeFild;
  QRCodeFild = nullptr;
  delete DataMatrixField;
  DataMatrixField = nullptr;
}

CBC_ReedSolomonGF256::CBC_ReedSolomonGF256(int32_t primitive) {
  int32_t x = 1;
  for (int32_t j = 0; j < 256; j++) {
    m_expTable[j] = x;
    x <<= 1;
    if (x >= 0x100) {
      x ^= primitive;
    }
  }
  for (int32_t i = 0; i < 255; i++) {
    m_logTable[m_expTable[i]] = i;
  }
  m_logTable[0] = 0;
}
void CBC_ReedSolomonGF256::Init() {
  m_zero = new CBC_ReedSolomonGF256Poly(this, 0);
  m_one = new CBC_ReedSolomonGF256Poly(this, 1);
}
CBC_ReedSolomonGF256::~CBC_ReedSolomonGF256() {
  delete m_zero;
  delete m_one;
}
CBC_ReedSolomonGF256Poly* CBC_ReedSolomonGF256::GetZero() {
  return m_zero;
}
CBC_ReedSolomonGF256Poly* CBC_ReedSolomonGF256::GetOne() {
  return m_one;
}
CBC_ReedSolomonGF256Poly* CBC_ReedSolomonGF256::BuildMonomial(
    int32_t degree,
    int32_t coefficient,
    int32_t& e) {
  if (degree < 0) {
    e = BCExceptionDegreeIsNegative;
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  }
  if (coefficient == 0) {
    CBC_ReedSolomonGF256Poly* temp = m_zero->Clone(e);
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
    return temp;
  }
  CFX_Int32Array coefficients;
  coefficients.SetSize(degree + 1);
  coefficients[0] = coefficient;
  CBC_ReedSolomonGF256Poly* temp = new CBC_ReedSolomonGF256Poly();
  temp->Init(this, &coefficients, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return temp;
}
int32_t CBC_ReedSolomonGF256::AddOrSubtract(int32_t a, int32_t b) {
  return a ^ b;
}
int32_t CBC_ReedSolomonGF256::Exp(int32_t a) {
  return m_expTable[a];
}
int32_t CBC_ReedSolomonGF256::Log(int32_t a, int32_t& e) {
  if (a == 0) {
    e = BCExceptionAIsZero;
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
  }
  return m_logTable[a];
}
int32_t CBC_ReedSolomonGF256::Inverse(int32_t a, int32_t& e) {
  if (a == 0) {
    e = BCExceptionAIsZero;
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
  }
  return m_expTable[255 - m_logTable[a]];
}
int32_t CBC_ReedSolomonGF256::Multiply(int32_t a, int32_t b) {
  if (a == 0 || b == 0) {
    return 0;
  }
  if (a == 1) {
    return b;
  }
  if (b == 1) {
    return a;
  }
  return m_expTable[(m_logTable[a] + m_logTable[b]) % 255];
}
