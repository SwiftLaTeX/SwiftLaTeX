// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2011 ZXing authors
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

#include "xfa/fxbarcode/pdf417/BC_PDF417BarcodeMatrix.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417BarcodeRow.h"

CBC_BarcodeMatrix::CBC_BarcodeMatrix(int32_t height, int32_t width) {
  m_matrix.SetSize(height + 2);
  for (int32_t i = 0, matrixLength = m_matrix.GetSize(); i < matrixLength;
       i++) {
    m_matrix[i] = new CBC_BarcodeRow((width + 4) * 17 + 1);
  }
  m_width = width * 17;
  m_height = height + 2;
  m_currentRow = 0;
  m_outHeight = 0;
  m_outWidth = 0;
}
CBC_BarcodeMatrix::~CBC_BarcodeMatrix() {
  for (int32_t i = 0; i < m_matrix.GetSize(); i++)
    delete m_matrix.GetAt(i);

  m_matrixOut.RemoveAll();
}
void CBC_BarcodeMatrix::set(int32_t x, int32_t y, uint8_t value) {
  m_matrix[y]->set(x, value);
}
void CBC_BarcodeMatrix::setMatrix(int32_t x, int32_t y, FX_BOOL black) {
  set(x, y, (uint8_t)(black ? 1 : 0));
}
void CBC_BarcodeMatrix::startRow() {
  ++m_currentRow;
}
CBC_BarcodeRow* CBC_BarcodeMatrix::getCurrentRow() {
  return m_matrix[m_currentRow];
}
int32_t CBC_BarcodeMatrix::getWidth() {
  return m_outWidth;
}
int32_t CBC_BarcodeMatrix::getHeight() {
  return m_outHeight;
}
CFX_ByteArray& CBC_BarcodeMatrix::getMatrix() {
  return getScaledMatrix(1, 1);
}
CFX_ByteArray& CBC_BarcodeMatrix::getScaledMatrix(int32_t scale) {
  return getScaledMatrix(scale, scale);
}
CFX_ByteArray& CBC_BarcodeMatrix::getScaledMatrix(int32_t xScale,
                                                  int32_t yScale) {
  int32_t yMax = m_height * yScale;
  CFX_ByteArray bytearray;
  bytearray.Copy(m_matrix[0]->getScaledRow(xScale));
  int32_t xMax = bytearray.GetSize();
  m_matrixOut.SetSize(xMax * yMax);
  m_outWidth = xMax;
  m_outHeight = yMax;
  int32_t k = 0;
  for (int32_t i = 0; i < yMax; i++) {
    if (i != 0) {
      bytearray.Copy(m_matrix[i / yScale]->getScaledRow(xScale));
    }
    k = i * xMax;
    for (int32_t l = 0; l < xMax; l++) {
      m_matrixOut[k + l] = bytearray.GetAt(l);
    }
  }
  return m_matrixOut;
}
