// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_
#define XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_

#include "core/fxcrt/include/fx_basic.h"
#include "xfa/fxbarcode/utils.h"

class CBC_ReedSolomonGF256Poly;
class CBC_ReedSolomonGF256 {
 public:
  static void Initialize();
  static void Finalize();
  static CBC_ReedSolomonGF256* QRCodeFild;
  static CBC_ReedSolomonGF256* DataMatrixField;
  CBC_ReedSolomonGF256(int32_t primitive);
  virtual ~CBC_ReedSolomonGF256();
  CBC_ReedSolomonGF256Poly* GetZero();
  CBC_ReedSolomonGF256Poly* GetOne();
  CBC_ReedSolomonGF256Poly* BuildMonomial(int32_t degree,
                                          int32_t coefficient,
                                          int32_t& e);
  static int32_t AddOrSubtract(int32_t a, int32_t b);
  int32_t Exp(int32_t a);
  int32_t Log(int32_t a, int32_t& e);
  int32_t Inverse(int32_t a, int32_t& e);
  int32_t Multiply(int32_t a, int32_t b);
  virtual void Init();

 private:
  int32_t m_expTable[256];
  int32_t m_logTable[256];
  CBC_ReedSolomonGF256Poly* m_zero;
  CBC_ReedSolomonGF256Poly* m_one;
};

#endif  // XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMONGF256_H_
