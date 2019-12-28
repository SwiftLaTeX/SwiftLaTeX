// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_QRCoderECB;

class CBC_QRCoderECBlocks {
 public:
  CBC_QRCoderECBlocks(int32_t ecCodeWordsPerBlock, CBC_QRCoderECB* ecBlocks);
  CBC_QRCoderECBlocks(int32_t ecCodeWordsPerBlock,
                      CBC_QRCoderECB* ecBlocks1,
                      CBC_QRCoderECB* ecBlocks2);
  ~CBC_QRCoderECBlocks();

  int32_t GetECCodeWordsPerBlock() const;
  int32_t GetNumBlocks() const;
  int32_t GetTotalECCodeWords() const;
  CFX_ArrayTemplate<CBC_QRCoderECB*>* GetECBlocks();

 private:
  int32_t m_ecCodeWordsPerBlock;
  CFX_ArrayTemplate<CBC_QRCoderECB*> m_ecBlocksArray;
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_
