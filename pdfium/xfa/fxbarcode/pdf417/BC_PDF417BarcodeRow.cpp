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

#include "xfa/fxbarcode/pdf417/BC_PDF417BarcodeRow.h"

CBC_BarcodeRow::CBC_BarcodeRow(int32_t width) {
  m_row.SetSize(width);
  m_currentLocation = 0;
}
CBC_BarcodeRow::~CBC_BarcodeRow() {
  m_output.RemoveAll();
  m_row.RemoveAll();
}
void CBC_BarcodeRow::set(int32_t x, uint8_t value) {
  m_row.SetAt(x, value);
}
void CBC_BarcodeRow::set(int32_t x, FX_BOOL black) {
  m_row.SetAt(x, (uint8_t)(black ? 1 : 0));
}
void CBC_BarcodeRow::addBar(FX_BOOL black, int32_t width) {
  for (int32_t ii = 0; ii < width; ii++) {
    set(m_currentLocation++, black);
  }
}
CFX_ByteArray& CBC_BarcodeRow::getRow() {
  return m_row;
}
CFX_ByteArray& CBC_BarcodeRow::getScaledRow(int32_t scale) {
  m_output.SetSize(m_row.GetSize() * scale);
  for (int32_t i = 0; i < m_output.GetSize(); i++) {
    m_output[i] = (m_row[i / scale]);
  }
  return m_output;
}
