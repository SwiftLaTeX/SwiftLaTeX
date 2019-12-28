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
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/utils.h"

CBC_CommonBitMatrix::CBC_CommonBitMatrix() {
  m_width = 0;
  m_height = 0;
  m_rowSize = 0;
  m_bits = nullptr;
}
void CBC_CommonBitMatrix::Init(int32_t dimension) {
  m_width = dimension;
  m_height = dimension;
  int32_t rowSize = (m_height + 31) >> 5;
  m_rowSize = rowSize;
  m_bits = FX_Alloc2D(int32_t, m_rowSize, m_height);
  FXSYS_memset(m_bits, 0, m_rowSize * m_height * sizeof(int32_t));
}
void CBC_CommonBitMatrix::Init(int32_t width, int32_t height) {
  m_width = width;
  m_height = height;
  int32_t rowSize = (width + 31) >> 5;
  m_rowSize = rowSize;
  m_bits = FX_Alloc2D(int32_t, m_rowSize, m_height);
  FXSYS_memset(m_bits, 0, m_rowSize * m_height * sizeof(int32_t));
}
CBC_CommonBitMatrix::~CBC_CommonBitMatrix() {
  FX_Free(m_bits);
}
FX_BOOL CBC_CommonBitMatrix::Get(int32_t x, int32_t y) {
  int32_t offset = y * m_rowSize + (x >> 5);
  if (offset >= m_rowSize * m_height || offset < 0) {
    return false;
  }
  return ((((uint32_t)m_bits[offset]) >> (x & 0x1f)) & 1) != 0;
}
int32_t* CBC_CommonBitMatrix::GetBits() {
  return m_bits;
}
void CBC_CommonBitMatrix::Set(int32_t x, int32_t y) {
  int32_t offset = y * m_rowSize + (x >> 5);
  if (offset >= m_rowSize * m_height || offset < 0) {
    return;
  }
  m_bits[offset] |= 1 << (x & 0x1f);
}
void CBC_CommonBitMatrix::Flip(int32_t x, int32_t y) {
  int32_t offset = y * m_rowSize + (x >> 5);
  m_bits[offset] ^= 1 << (x & 0x1f);
}
void CBC_CommonBitMatrix::Clear() {
  FXSYS_memset(m_bits, 0, m_rowSize * m_height * sizeof(int32_t));
}
void CBC_CommonBitMatrix::SetRegion(int32_t left,
                                    int32_t top,
                                    int32_t width,
                                    int32_t height,
                                    int32_t& e) {
  if (top < 0 || left < 0) {
    e = BCExceptionLeftAndTopMustBeNonnegative;
    return;
  }
  if (height < 1 || width < 1) {
    e = BCExceptionHeightAndWidthMustBeAtLeast1;
    return;
  }
  int32_t right = left + width;
  int32_t bottom = top + height;
  if (m_height < bottom || m_width < right) {
    e = BCExceptionRegionMustFitInsideMatrix;
    return;
  }
  int32_t y;
  for (y = top; y < bottom; y++) {
    int32_t offset = y * m_rowSize;
    int32_t x;
    for (x = left; x < right; x++) {
      m_bits[offset + (x >> 5)] |= 1 << (x & 0x1f);
    }
  }
}
CBC_CommonBitArray* CBC_CommonBitMatrix::GetRow(int32_t y,
                                                CBC_CommonBitArray* row) {
  CBC_CommonBitArray* rowArray = nullptr;
  if (!row || row->GetSize() < m_width) {
    rowArray = new CBC_CommonBitArray(m_width);
  } else {
    rowArray = new CBC_CommonBitArray(row);
  }
  int32_t offset = y * m_rowSize;
  int32_t x;
  for (x = 0; x < m_rowSize; x++) {
    rowArray->SetBulk(x << 5, m_bits[offset + x]);
  }
  return rowArray;
}
void CBC_CommonBitMatrix::SetRow(int32_t y, CBC_CommonBitArray* row) {
  int32_t l = y * m_rowSize;
  for (int32_t i = 0; i < m_rowSize; i++) {
    m_bits[l] = row->GetBitArray()[i];
    l++;
  }
}
void CBC_CommonBitMatrix::SetCol(int32_t y, CBC_CommonBitArray* col) {
  for (int32_t i = 0; i < col->GetBits().GetSize(); i++) {
    m_bits[i * m_rowSize + y] = col->GetBitArray()[i];
  }
}
int32_t CBC_CommonBitMatrix::GetWidth() {
  return m_width;
}
int32_t CBC_CommonBitMatrix::GetHeight() {
  return m_height;
}
int32_t CBC_CommonBitMatrix::GetRowSize() {
  return m_rowSize;
}
int32_t CBC_CommonBitMatrix::GetDimension(int32_t& e) {
  if (m_width != m_height) {
    e = BCExceptionCanNotCallGetDimensionOnNonSquareMatrix;
    return 0;
  }
  return m_width;
}
