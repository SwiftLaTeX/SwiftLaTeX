// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_PDF417_BC_PDF417BARCODEMATRIX_H_
#define XFA_FXBARCODE_PDF417_BC_PDF417BARCODEMATRIX_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_BarcodeRow;

class CBC_BarcodeMatrix {
 public:
  CBC_BarcodeMatrix();
  CBC_BarcodeMatrix(int32_t height, int32_t width);
  virtual ~CBC_BarcodeMatrix();

  void set(int32_t x, int32_t y, uint8_t value);
  void setMatrix(int32_t x, int32_t y, FX_BOOL black);
  void startRow();
  CBC_BarcodeRow* getCurrentRow();
  CFX_ByteArray& getMatrix();
  CFX_ByteArray& getScaledMatrix(int32_t scale);
  CFX_ByteArray& getScaledMatrix(int32_t xScale, int32_t yScale);
  int32_t getWidth();
  int32_t getHeight();

 private:
  CFX_ArrayTemplate<CBC_BarcodeRow*> m_matrix;
  CFX_ByteArray m_matrixOut;
  int32_t m_currentRow;
  int32_t m_height;
  int32_t m_width;
  int32_t m_outWidth;
  int32_t m_outHeight;
};

#endif  // XFA_FXBARCODE_PDF417_BC_PDF417BARCODEMATRIX_H_
