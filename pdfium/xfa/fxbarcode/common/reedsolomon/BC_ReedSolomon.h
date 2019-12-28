// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_
#define XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_ReedSolomonGF256;
class CBC_ReedSolomonGF256Poly;

class CBC_ReedSolomonEncoder {
 public:
  CBC_ReedSolomonEncoder(CBC_ReedSolomonGF256* field);
  virtual ~CBC_ReedSolomonEncoder();

  void Encode(CFX_Int32Array* toEncode, int32_t ecBytes, int32_t& e);
  virtual void Init();

 private:
  CBC_ReedSolomonGF256Poly* BuildGenerator(int32_t degree, int32_t& e);

  CBC_ReedSolomonGF256* m_field;
  CFX_ArrayTemplate<CBC_ReedSolomonGF256Poly*> m_cachedGenerators;
};

#endif  // XFA_FXBARCODE_COMMON_REEDSOLOMON_BC_REEDSOLOMON_H_
