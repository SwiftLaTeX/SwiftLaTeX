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

#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderBitVector.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderECB.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderECBlocks.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderVersion.h"
#include "xfa/fxbarcode/utils.h"

namespace {

const uint8_t BITS_SET_IN_HALF_BYTE[] = {0, 1, 1, 2, 1, 2, 2, 3,
                                         1, 2, 2, 3, 2, 3, 3, 4};

int32_t NumBitsDiffering(int32_t a, int32_t b) {
  a ^= b;
  return BITS_SET_IN_HALF_BYTE[a & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 4) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 8) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 12) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 16) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 20) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 24) & 0x0F] +
         BITS_SET_IN_HALF_BYTE[(a >> 28) & 0x0F];
}

}  // namespace

const int32_t CBC_QRCoderVersion::VERSION_DECODE_INFO[] = {
    0x07C94, 0x085BC, 0x09A99, 0x0A4D3, 0x0BBF6, 0x0C762, 0x0D847,
    0x0E60D, 0x0F928, 0x10B78, 0x1145D, 0x12A17, 0x13532, 0x149A6,
    0x15683, 0x168C9, 0x177EC, 0x18EC4, 0x191E1, 0x1AFAB, 0x1B08E,
    0x1CC1A, 0x1D33F, 0x1ED75, 0x1F250, 0x209D5, 0x216F0, 0x228BA,
    0x2379F, 0x24B0B, 0x2542E, 0x26A64, 0x27541, 0x28C69};

CFX_ArrayTemplate<CBC_QRCoderVersion*>* CBC_QRCoderVersion::VERSION = nullptr;

void CBC_QRCoderVersion::Initialize() {
  VERSION = new CFX_ArrayTemplate<CBC_QRCoderVersion*>();
}
void CBC_QRCoderVersion::Finalize() {
  for (int32_t i = 0; i < VERSION->GetSize(); i++)
    delete VERSION->GetAt(i);

  delete VERSION;
  VERSION = nullptr;
}
CBC_QRCoderVersion::CBC_QRCoderVersion(int32_t versionNumber,
                                       CBC_QRCoderECBlocks* ecBlocks1,
                                       CBC_QRCoderECBlocks* ecBlocks2,
                                       CBC_QRCoderECBlocks* ecBlocks3,
                                       CBC_QRCoderECBlocks* ecBlocks4) {
  m_versionNumber = versionNumber;
  m_ecBlocksArray.Add(ecBlocks1);
  m_ecBlocksArray.Add(ecBlocks2);
  m_ecBlocksArray.Add(ecBlocks3);
  m_ecBlocksArray.Add(ecBlocks4);
  int32_t total = 0;
  int32_t ecCodeWords = ecBlocks1->GetECCodeWordsPerBlock();
  CFX_ArrayTemplate<CBC_QRCoderECB*>* ecbArray = ecBlocks1->GetECBlocks();
  for (int32_t i = 0; i < ecbArray->GetSize(); i++) {
    CBC_QRCoderECB* ecBlock = (*ecbArray)[i];
    total += ecBlock->GetCount() * (ecBlock->GetDataCodeWords() + ecCodeWords);
  }
  m_totalCodeWords = total;
  switch (versionNumber) {
    case 1:
      break;
    case 2:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(18);
      break;
    case 3:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(22);
      break;
    case 4:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      break;
    case 5:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      break;
    case 6:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      break;
    case 7:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(22);
      m_alignmentPatternCenters.Add(38);
      break;
    case 8:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(24);
      m_alignmentPatternCenters.Add(42);
      break;
    case 9:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(46);
      break;
    case 10:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(28);
      m_alignmentPatternCenters.Add(50);
      break;
    case 11:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(54);
      break;
    case 12:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(32);
      m_alignmentPatternCenters.Add(58);
      break;
    case 13:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      m_alignmentPatternCenters.Add(62);
      break;
    case 14:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(46);
      m_alignmentPatternCenters.Add(66);
      break;
    case 15:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(48);
      m_alignmentPatternCenters.Add(70);
      break;
    case 16:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(50);
      m_alignmentPatternCenters.Add(74);
      break;
    case 17:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(78);
      break;
    case 18:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(56);
      m_alignmentPatternCenters.Add(82);
      break;
    case 19:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(86);
      break;
    case 20:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      m_alignmentPatternCenters.Add(62);
      m_alignmentPatternCenters.Add(90);
      break;
    case 21:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(28);
      m_alignmentPatternCenters.Add(50);
      m_alignmentPatternCenters.Add(72);
      m_alignmentPatternCenters.Add(94);
      break;
    case 22:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(50);
      m_alignmentPatternCenters.Add(74);
      m_alignmentPatternCenters.Add(98);
      break;
    case 23:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(74);
      m_alignmentPatternCenters.Add(102);
      break;
    case 24:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(28);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(80);
      m_alignmentPatternCenters.Add(106);
      break;
    case 25:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(32);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(84);
      m_alignmentPatternCenters.Add(110);
      break;
    case 26:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(86);
      m_alignmentPatternCenters.Add(114);
      break;
    case 27:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      m_alignmentPatternCenters.Add(62);
      m_alignmentPatternCenters.Add(90);
      m_alignmentPatternCenters.Add(118);
      break;
    case 28:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(50);
      m_alignmentPatternCenters.Add(74);
      m_alignmentPatternCenters.Add(98);
      m_alignmentPatternCenters.Add(122);
      break;
    case 29:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(78);
      m_alignmentPatternCenters.Add(102);
      m_alignmentPatternCenters.Add(126);
      break;
    case 30:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(52);
      m_alignmentPatternCenters.Add(78);
      m_alignmentPatternCenters.Add(104);
      m_alignmentPatternCenters.Add(130);
      break;
    case 31:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(56);
      m_alignmentPatternCenters.Add(82);
      m_alignmentPatternCenters.Add(108);
      m_alignmentPatternCenters.Add(134);
      break;
    case 32:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      m_alignmentPatternCenters.Add(60);
      m_alignmentPatternCenters.Add(86);
      m_alignmentPatternCenters.Add(112);
      m_alignmentPatternCenters.Add(138);
      break;
    case 33:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(86);
      m_alignmentPatternCenters.Add(114);
      m_alignmentPatternCenters.Add(142);
      break;
    case 34:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(34);
      m_alignmentPatternCenters.Add(62);
      m_alignmentPatternCenters.Add(90);
      m_alignmentPatternCenters.Add(118);
      m_alignmentPatternCenters.Add(146);
      break;
    case 35:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(78);
      m_alignmentPatternCenters.Add(102);
      m_alignmentPatternCenters.Add(126);
      m_alignmentPatternCenters.Add(150);
      break;
    case 36:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(24);
      m_alignmentPatternCenters.Add(50);
      m_alignmentPatternCenters.Add(76);
      m_alignmentPatternCenters.Add(102);
      m_alignmentPatternCenters.Add(128);
      m_alignmentPatternCenters.Add(154);
      break;
    case 37:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(28);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(80);
      m_alignmentPatternCenters.Add(106);
      m_alignmentPatternCenters.Add(132);
      m_alignmentPatternCenters.Add(158);
      break;
    case 38:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(32);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(84);
      m_alignmentPatternCenters.Add(110);
      m_alignmentPatternCenters.Add(136);
      m_alignmentPatternCenters.Add(162);
      break;
    case 39:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(26);
      m_alignmentPatternCenters.Add(54);
      m_alignmentPatternCenters.Add(82);
      m_alignmentPatternCenters.Add(110);
      m_alignmentPatternCenters.Add(138);
      m_alignmentPatternCenters.Add(166);
      break;
    case 40:
      m_alignmentPatternCenters.Add(6);
      m_alignmentPatternCenters.Add(30);
      m_alignmentPatternCenters.Add(58);
      m_alignmentPatternCenters.Add(86);
      m_alignmentPatternCenters.Add(114);
      m_alignmentPatternCenters.Add(142);
      m_alignmentPatternCenters.Add(170);
      break;
  }
}

CBC_QRCoderVersion::~CBC_QRCoderVersion() {
  for (int32_t i = 0; i < m_ecBlocksArray.GetSize(); ++i)
    delete m_ecBlocksArray[i];
}

int32_t CBC_QRCoderVersion::GetVersionNumber() {
  return m_versionNumber;
}
CFX_Int32Array* CBC_QRCoderVersion::GetAlignmentPatternCenters() {
  return &m_alignmentPatternCenters;
}
int32_t CBC_QRCoderVersion::GetTotalCodeWords() {
  return m_totalCodeWords;
}
int32_t CBC_QRCoderVersion::GetDimensionForVersion() {
  return 17 + 4 * m_versionNumber;
}
CBC_QRCoderECBlocks* CBC_QRCoderVersion::GetECBlocksForLevel(
    CBC_QRCoderErrorCorrectionLevel* ecLevel) {
  return m_ecBlocksArray[ecLevel->Ordinal()];
}
CBC_QRCoderVersion* CBC_QRCoderVersion::GetProvisionalVersionForDimension(
    int32_t dimension,
    int32_t& e) {
  if ((dimension % 4) != 1) {
    e = BCExceptionRead;
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  }
  CBC_QRCoderVersion* qcv = GetVersionForNumber((dimension - 17) >> 2, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return qcv;
}
CBC_QRCoderVersion* CBC_QRCoderVersion::DecodeVersionInformation(
    int32_t versionBits,
    int32_t& e) {
  int32_t bestDifference = FXSYS_IntMax;
  int32_t bestVersion = 0;
  for (int32_t i = 0; i < 34; i++) {
    int32_t targetVersion = VERSION_DECODE_INFO[i];
    if (targetVersion == versionBits) {
      CBC_QRCoderVersion* qcv = GetVersionForNumber(i + 7, e);
      BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
      return qcv;
    }
    int32_t bitsDifference = NumBitsDiffering(versionBits, targetVersion);
    if (bitsDifference < bestDifference) {
      bestVersion = i + 7;
      bestDifference = bitsDifference;
    }
  }
  if (bestDifference <= 3) {
    CBC_QRCoderVersion* qcv = GetVersionForNumber(bestVersion, e);
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
    return qcv;
  }
  return nullptr;
}
CBC_CommonBitMatrix* CBC_QRCoderVersion::BuildFunctionPattern(int32_t& e) {
  int32_t dimension = GetDimensionForVersion();
  CBC_CommonBitMatrix* bitMatrix = new CBC_CommonBitMatrix();
  bitMatrix->Init(dimension);
  bitMatrix->SetRegion(0, 0, 9, 9, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  bitMatrix->SetRegion(dimension - 8, 0, 8, 9, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  bitMatrix->SetRegion(0, dimension - 8, 9, 8, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  int32_t max = m_alignmentPatternCenters.GetSize();
  for (int32_t x = 0; x < max; x++) {
    int32_t i = m_alignmentPatternCenters[x] - 2;
    for (int32_t y = 0; y < max; y++) {
      if ((x == 0 && (y == 0 || y == max - 1)) || (x == max - 1 && y == 0)) {
        continue;
      }
      bitMatrix->SetRegion(m_alignmentPatternCenters[y] - 2, i, 5, 5, e);
      BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
    }
  }
  bitMatrix->SetRegion(6, 9, 1, dimension - 17, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  bitMatrix->SetRegion(9, 6, dimension - 17, 1, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  if (m_versionNumber > 6) {
    bitMatrix->SetRegion(dimension - 11, 0, 3, 6, e);
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
    bitMatrix->SetRegion(0, dimension - 11, 6, 3, e);
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  }
  return bitMatrix;
}
CBC_QRCoderVersion* CBC_QRCoderVersion::GetVersionForNumber(
    int32_t versionNumber,
    int32_t& e) {
  if (VERSION->GetSize() == 0) {
    VERSION->Add(new CBC_QRCoderVersion(
        1, new CBC_QRCoderECBlocks(7, new CBC_QRCoderECB(1, 19)),
        new CBC_QRCoderECBlocks(10, new CBC_QRCoderECB(1, 16)),
        new CBC_QRCoderECBlocks(13, new CBC_QRCoderECB(1, 13)),
        new CBC_QRCoderECBlocks(17, new CBC_QRCoderECB(1, 9))));
    VERSION->Add(new CBC_QRCoderVersion(
        2, new CBC_QRCoderECBlocks(10, new CBC_QRCoderECB(1, 34)),
        new CBC_QRCoderECBlocks(16, new CBC_QRCoderECB(1, 28)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(1, 22)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(1, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        3, new CBC_QRCoderECBlocks(15, new CBC_QRCoderECB(1, 55)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(1, 44)),
        new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 17)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(2, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        4, new CBC_QRCoderECBlocks(20, new CBC_QRCoderECB(1, 80)),
        new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 32)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(2, 24)),
        new CBC_QRCoderECBlocks(16, new CBC_QRCoderECB(4, 9))));
    VERSION->Add(new CBC_QRCoderVersion(
        5, new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(1, 108)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(2, 43)),
        new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 15),
                                new CBC_QRCoderECB(2, 16)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(2, 11),
                                new CBC_QRCoderECB(2, 12))));
    VERSION->Add(new CBC_QRCoderVersion(
        6, new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 68)),
        new CBC_QRCoderECBlocks(16, new CBC_QRCoderECB(4, 27)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(4, 19)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(4, 15))));
    VERSION->Add(new CBC_QRCoderVersion(
        7, new CBC_QRCoderECBlocks(20, new CBC_QRCoderECB(2, 78)),
        new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(4, 31)),
        new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 14),
                                new CBC_QRCoderECB(4, 15)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(4, 13),
                                new CBC_QRCoderECB(1, 14))));
    VERSION->Add(new CBC_QRCoderVersion(
        8, new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(2, 97)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(2, 38),
                                new CBC_QRCoderECB(2, 39)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(4, 18),
                                new CBC_QRCoderECB(2, 19)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(4, 14),
                                new CBC_QRCoderECB(2, 15))));
    VERSION->Add(new CBC_QRCoderVersion(
        9, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(2, 116)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(3, 36),
                                new CBC_QRCoderECB(2, 37)),
        new CBC_QRCoderECBlocks(20, new CBC_QRCoderECB(4, 16),
                                new CBC_QRCoderECB(4, 17)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(4, 12),
                                new CBC_QRCoderECB(4, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        10, new CBC_QRCoderECBlocks(18, new CBC_QRCoderECB(2, 68),
                                    new CBC_QRCoderECB(2, 69)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(4, 43),
                                new CBC_QRCoderECB(1, 44)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(6, 19),
                                new CBC_QRCoderECB(2, 20)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(6, 15),
                                new CBC_QRCoderECB(2, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        11, new CBC_QRCoderECBlocks(20, new CBC_QRCoderECB(4, 81)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(1, 50),
                                new CBC_QRCoderECB(4, 51)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(4, 22),
                                new CBC_QRCoderECB(4, 23)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(3, 12),
                                new CBC_QRCoderECB(8, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        12, new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(2, 92),
                                    new CBC_QRCoderECB(2, 93)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(6, 36),
                                new CBC_QRCoderECB(2, 37)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(4, 20),
                                new CBC_QRCoderECB(6, 21)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(7, 14),
                                new CBC_QRCoderECB(4, 15))));
    VERSION->Add(new CBC_QRCoderVersion(
        13, new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(4, 107)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(8, 37),
                                new CBC_QRCoderECB(1, 38)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(8, 20),
                                new CBC_QRCoderECB(4, 21)),
        new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(12, 11),
                                new CBC_QRCoderECB(4, 12))));
    VERSION->Add(new CBC_QRCoderVersion(
        14, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(3, 115),
                                    new CBC_QRCoderECB(1, 116)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(4, 40),
                                new CBC_QRCoderECB(5, 41)),
        new CBC_QRCoderECBlocks(20, new CBC_QRCoderECB(11, 16),
                                new CBC_QRCoderECB(5, 17)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(11, 12),
                                new CBC_QRCoderECB(5, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        15, new CBC_QRCoderECBlocks(22, new CBC_QRCoderECB(5, 87),
                                    new CBC_QRCoderECB(1, 88)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(5, 41),
                                new CBC_QRCoderECB(5, 42)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(5, 24),
                                new CBC_QRCoderECB(7, 25)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(11, 12),
                                new CBC_QRCoderECB(7, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        16, new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(5, 98),
                                    new CBC_QRCoderECB(1, 99)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(7, 45),
                                new CBC_QRCoderECB(3, 46)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(15, 19),
                                new CBC_QRCoderECB(2, 20)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(3, 15),
                                new CBC_QRCoderECB(13, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        17, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(1, 107),
                                    new CBC_QRCoderECB(5, 108)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(10, 46),
                                new CBC_QRCoderECB(1, 47)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(1, 22),
                                new CBC_QRCoderECB(15, 23)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(2, 14),
                                new CBC_QRCoderECB(17, 15))));
    VERSION->Add(new CBC_QRCoderVersion(
        18, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(5, 120),
                                    new CBC_QRCoderECB(1, 121)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(9, 43),
                                new CBC_QRCoderECB(4, 44)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(17, 22),
                                new CBC_QRCoderECB(1, 23)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(2, 14),
                                new CBC_QRCoderECB(19, 15))));
    VERSION->Add(new CBC_QRCoderVersion(
        19, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(3, 113),
                                    new CBC_QRCoderECB(4, 114)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(3, 44),
                                new CBC_QRCoderECB(11, 45)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(17, 21),
                                new CBC_QRCoderECB(4, 22)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(9, 13),
                                new CBC_QRCoderECB(16, 14))));
    VERSION->Add(new CBC_QRCoderVersion(
        20, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(3, 107),
                                    new CBC_QRCoderECB(5, 108)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(3, 41),
                                new CBC_QRCoderECB(13, 42)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(15, 24),
                                new CBC_QRCoderECB(5, 25)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(15, 15),
                                new CBC_QRCoderECB(10, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        21, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(4, 116),
                                    new CBC_QRCoderECB(4, 117)),
        new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(17, 42)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(17, 22),
                                new CBC_QRCoderECB(6, 23)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(19, 16),
                                new CBC_QRCoderECB(6, 17))));
    VERSION->Add(new CBC_QRCoderVersion(
        22, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(2, 111),
                                    new CBC_QRCoderECB(7, 112)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(17, 46)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(7, 24),
                                new CBC_QRCoderECB(16, 25)),
        new CBC_QRCoderECBlocks(24, new CBC_QRCoderECB(34, 13))));
    VERSION->Add(new CBC_QRCoderVersion(
        23, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(4, 121),
                                    new CBC_QRCoderECB(5, 122)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(4, 47),
                                new CBC_QRCoderECB(14, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(11, 24),
                                new CBC_QRCoderECB(14, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(16, 15),
                                new CBC_QRCoderECB(14, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        24, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(6, 117),
                                    new CBC_QRCoderECB(4, 118)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(6, 45),
                                new CBC_QRCoderECB(14, 46)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(11, 24),
                                new CBC_QRCoderECB(16, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(30, 16),
                                new CBC_QRCoderECB(2, 17))));
    VERSION->Add(new CBC_QRCoderVersion(
        25, new CBC_QRCoderECBlocks(26, new CBC_QRCoderECB(8, 106),
                                    new CBC_QRCoderECB(4, 107)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(8, 47),
                                new CBC_QRCoderECB(13, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(7, 24),
                                new CBC_QRCoderECB(22, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(22, 15),
                                new CBC_QRCoderECB(13, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        26, new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(10, 114),
                                    new CBC_QRCoderECB(2, 115)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(19, 46),
                                new CBC_QRCoderECB(4, 47)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(28, 22),
                                new CBC_QRCoderECB(6, 23)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(33, 16),
                                new CBC_QRCoderECB(4, 17))));
    VERSION->Add(new CBC_QRCoderVersion(
        27, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(8, 122),
                                    new CBC_QRCoderECB(4, 123)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(22, 45),
                                new CBC_QRCoderECB(3, 46)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(8, 23),
                                new CBC_QRCoderECB(26, 24)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(12, 15),
                                new CBC_QRCoderECB(28, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        28, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(3, 117),
                                    new CBC_QRCoderECB(10, 118)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(3, 45),
                                new CBC_QRCoderECB(23, 46)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(4, 24),
                                new CBC_QRCoderECB(31, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(11, 15),
                                new CBC_QRCoderECB(31, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        29, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(7, 116),
                                    new CBC_QRCoderECB(7, 117)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(21, 45),
                                new CBC_QRCoderECB(7, 46)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(1, 23),
                                new CBC_QRCoderECB(37, 24)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(19, 15),
                                new CBC_QRCoderECB(26, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        30, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(5, 115),
                                    new CBC_QRCoderECB(10, 116)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(19, 47),
                                new CBC_QRCoderECB(10, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(15, 24),
                                new CBC_QRCoderECB(25, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(23, 15),
                                new CBC_QRCoderECB(25, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        31, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(13, 115),
                                    new CBC_QRCoderECB(3, 116)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(2, 46),
                                new CBC_QRCoderECB(29, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(42, 24),
                                new CBC_QRCoderECB(1, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(23, 15),
                                new CBC_QRCoderECB(28, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        32, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(17, 115)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(10, 46),
                                new CBC_QRCoderECB(23, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(10, 24),
                                new CBC_QRCoderECB(35, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(19, 15),
                                new CBC_QRCoderECB(35, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        33, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(17, 115),
                                    new CBC_QRCoderECB(1, 116)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(14, 46),
                                new CBC_QRCoderECB(21, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(29, 24),
                                new CBC_QRCoderECB(19, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(11, 15),
                                new CBC_QRCoderECB(46, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        34, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(13, 115),
                                    new CBC_QRCoderECB(6, 116)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(14, 46),
                                new CBC_QRCoderECB(23, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(44, 24),
                                new CBC_QRCoderECB(7, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(59, 16),
                                new CBC_QRCoderECB(1, 17))));
    VERSION->Add(new CBC_QRCoderVersion(
        35, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(12, 121),
                                    new CBC_QRCoderECB(7, 122)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(12, 47),
                                new CBC_QRCoderECB(26, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(39, 24),
                                new CBC_QRCoderECB(14, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(22, 15),
                                new CBC_QRCoderECB(41, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        36, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(6, 121),
                                    new CBC_QRCoderECB(14, 122)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(6, 47),
                                new CBC_QRCoderECB(34, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(46, 24),
                                new CBC_QRCoderECB(10, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(2, 15),
                                new CBC_QRCoderECB(64, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        37, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(17, 122),
                                    new CBC_QRCoderECB(4, 123)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(29, 46),
                                new CBC_QRCoderECB(14, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(49, 24),
                                new CBC_QRCoderECB(10, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(24, 15),
                                new CBC_QRCoderECB(46, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        38, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(4, 122),
                                    new CBC_QRCoderECB(18, 123)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(13, 46),
                                new CBC_QRCoderECB(32, 47)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(48, 24),
                                new CBC_QRCoderECB(14, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(42, 15),
                                new CBC_QRCoderECB(32, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        39, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(20, 117),
                                    new CBC_QRCoderECB(4, 118)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(40, 47),
                                new CBC_QRCoderECB(7, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(43, 24),
                                new CBC_QRCoderECB(22, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(10, 15),
                                new CBC_QRCoderECB(67, 16))));
    VERSION->Add(new CBC_QRCoderVersion(
        40, new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(19, 118),
                                    new CBC_QRCoderECB(6, 119)),
        new CBC_QRCoderECBlocks(28, new CBC_QRCoderECB(18, 47),
                                new CBC_QRCoderECB(31, 48)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(34, 24),
                                new CBC_QRCoderECB(34, 25)),
        new CBC_QRCoderECBlocks(30, new CBC_QRCoderECB(20, 15),
                                new CBC_QRCoderECB(61, 16))));
  }
  if (versionNumber < 1 || versionNumber > 40) {
    e = BCExceptionIllegalArgument;
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  }
  return (*VERSION)[versionNumber - 1];
}

void CBC_QRCoderVersion::Destroy() {
  for (int32_t i = 0; i < VERSION->GetSize(); i++)
    delete (*VERSION)[i];
  VERSION->RemoveAll();
}
