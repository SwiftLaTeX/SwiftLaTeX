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

#include "xfa/fxbarcode/qrcode/BC_QRCoderECB.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderECBlocks.h"

CBC_QRCoderECBlocks::CBC_QRCoderECBlocks(int32_t ecCodeWordsPerBlock,
                                         CBC_QRCoderECB* ecBlocks)
    : m_ecCodeWordsPerBlock(ecCodeWordsPerBlock) {
  m_ecBlocksArray.Add(ecBlocks);
}

CBC_QRCoderECBlocks::CBC_QRCoderECBlocks(int32_t ecCodeWordsPerBlock,
                                         CBC_QRCoderECB* ecBlocks1,
                                         CBC_QRCoderECB* ecBlocks2)
    : m_ecCodeWordsPerBlock(ecCodeWordsPerBlock) {
  m_ecBlocksArray.Add(ecBlocks1);
  m_ecBlocksArray.Add(ecBlocks2);
}

CBC_QRCoderECBlocks::~CBC_QRCoderECBlocks() {
  for (int32_t i = 0; i < m_ecBlocksArray.GetSize(); i++)
    delete m_ecBlocksArray[i];
}

int32_t CBC_QRCoderECBlocks::GetECCodeWordsPerBlock() const {
  return m_ecCodeWordsPerBlock;
}

int32_t CBC_QRCoderECBlocks::GetNumBlocks() const {
  int32_t total = 0;
  for (int32_t i = 0; i < m_ecBlocksArray.GetSize(); i++)
    total += m_ecBlocksArray[i]->GetCount();

  return total;
}

int32_t CBC_QRCoderECBlocks::GetTotalECCodeWords() const {
  return m_ecCodeWordsPerBlock * GetNumBlocks();
}

CFX_ArrayTemplate<CBC_QRCoderECB*>* CBC_QRCoderECBlocks::GetECBlocks() {
  return &m_ecBlocksArray;
}
