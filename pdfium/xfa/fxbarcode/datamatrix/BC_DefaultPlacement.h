// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_
#define XFA_FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_DefaultPlacement {
 public:
  CBC_DefaultPlacement(CFX_WideString codewords,
                       int32_t numcols,
                       int32_t numrows);
  virtual ~CBC_DefaultPlacement();

  int32_t getNumrows();
  int32_t getNumcols();
  CFX_ByteArray& getBits();
  FX_BOOL getBit(int32_t col, int32_t row);
  void setBit(int32_t col, int32_t row, FX_BOOL bit);
  FX_BOOL hasBit(int32_t col, int32_t row);
  void place();

 private:
  CFX_WideString m_codewords;
  int32_t m_numrows;
  int32_t m_numcols;
  CFX_ByteArray m_bits;
  void module(int32_t row, int32_t col, int32_t pos, int32_t bit);
  void utah(int32_t row, int32_t col, int32_t pos);
  void corner1(int32_t pos);
  void corner2(int32_t pos);
  void corner3(int32_t pos);
  void corner4(int32_t pos);
};

#endif  // XFA_FXBARCODE_DATAMATRIX_BC_DEFAULTPLACEMENT_H_
