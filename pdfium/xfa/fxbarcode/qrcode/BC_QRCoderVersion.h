// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_QRCODE_BC_QRCODERVERSION_H_
#define XFA_FXBARCODE_QRCODE_BC_QRCODERVERSION_H_

#include "core/fxcrt/include/fx_basic.h"

class CBC_CommonBitMatrix;
class CBC_QRCoderECBlocks;
class CBC_QRCoderErrorCorrectionLevel;

class CBC_QRCoderVersion {
 public:
  virtual ~CBC_QRCoderVersion();
  static void Initialize();
  static void Finalize();

  int32_t GetVersionNumber();
  int32_t GetTotalCodeWords();
  int32_t GetDimensionForVersion();
  CBC_CommonBitMatrix* BuildFunctionPattern(int32_t& e);
  CFX_Int32Array* GetAlignmentPatternCenters();
  CBC_QRCoderECBlocks* GetECBlocksForLevel(
      CBC_QRCoderErrorCorrectionLevel* ecLevel);
  static CBC_QRCoderVersion* GetVersionForNumber(int32_t versionNumber,
                                                 int32_t& e);
  static CBC_QRCoderVersion* GetProvisionalVersionForDimension(
      int32_t dimension,
      int32_t& e);
  static CBC_QRCoderVersion* DecodeVersionInformation(int32_t versionBits,
                                                      int32_t& e);
  static void Destroy();

 private:
  CBC_QRCoderVersion();
  CBC_QRCoderVersion(int32_t versionNumber,
                     CBC_QRCoderECBlocks* ecBlocks1,
                     CBC_QRCoderECBlocks* ecBlocks2,
                     CBC_QRCoderECBlocks* ecBlocks3,
                     CBC_QRCoderECBlocks* ecBlocks4);

  static const int32_t VERSION_DECODE_INFO[34];
  static CFX_ArrayTemplate<CBC_QRCoderVersion*>* VERSION;

  int32_t m_versionNumber;
  int32_t m_totalCodeWords;
  CFX_Int32Array m_alignmentPatternCenters;
  CFX_ArrayTemplate<CBC_QRCoderECBlocks*> m_ecBlocksArray;
};

#endif  // XFA_FXBARCODE_QRCODE_BC_QRCODERVERSION_H_
