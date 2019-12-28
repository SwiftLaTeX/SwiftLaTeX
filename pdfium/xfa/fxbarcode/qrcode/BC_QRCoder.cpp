// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
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

#include "xfa/fxbarcode/common/BC_CommonByteMatrix.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMode.h"
#include "xfa/fxbarcode/utils.h"

CBC_QRCoder::CBC_QRCoder() {
  m_mode = nullptr;
  m_ecLevel = nullptr;
  m_version = -1;
  m_matrixWidth = -1;
  m_maskPattern = -1;
  m_numTotalBytes = -1;
  m_numDataBytes = -1;
  m_numECBytes = -1;
  m_numRSBlocks = -1;
  m_matrix = nullptr;
}
CBC_QRCoder::~CBC_QRCoder() {
  delete m_matrix;
}
CBC_QRCoderMode* CBC_QRCoder::GetMode() {
  return m_mode;
}
CBC_QRCoderErrorCorrectionLevel* CBC_QRCoder::GetECLevel() {
  return m_ecLevel;
}
int32_t CBC_QRCoder::GetVersion() {
  return m_version;
}
int32_t CBC_QRCoder::GetMatrixWidth() {
  return m_matrixWidth;
}
int32_t CBC_QRCoder::GetMaskPattern() {
  return m_maskPattern;
}
int32_t CBC_QRCoder::GetNumTotalBytes() {
  return m_numTotalBytes;
}
int32_t CBC_QRCoder::GetNumDataBytes() {
  return m_numDataBytes;
}
int32_t CBC_QRCoder::GetNumECBytes() {
  return m_numECBytes;
}
int32_t CBC_QRCoder::GetNumRSBlocks() {
  return m_numRSBlocks;
}
CBC_CommonByteMatrix* CBC_QRCoder::GetMatrix() {
  return m_matrix;
}
int32_t CBC_QRCoder::At(int32_t x, int32_t y, int32_t& e) {
  int32_t value = m_matrix->Get(x, y);
  if (!(value == 0 || value == 1)) {
    e = BCExceptionValueMustBeEither0or1;
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
  }
  return value;
}
FX_BOOL CBC_QRCoder::IsValid() {
  return m_mode && m_ecLevel && m_version != -1 && m_matrixWidth != -1 &&
         m_maskPattern != -1 && m_numTotalBytes != -1 && m_numDataBytes != -1 &&
         m_numECBytes != -1 && m_numRSBlocks != -1 &&
         IsValidMaskPattern(m_maskPattern) &&
         m_numTotalBytes == m_numDataBytes + m_numECBytes && m_matrix &&
         m_matrixWidth == m_matrix->GetWidth() &&
         m_matrix->GetWidth() == m_matrix->GetHeight();
}
void CBC_QRCoder::SetMode(CBC_QRCoderMode* value) {
  m_mode = value;
}
void CBC_QRCoder::SetECLevel(CBC_QRCoderErrorCorrectionLevel* ecLevel) {
  m_ecLevel = ecLevel;
}
void CBC_QRCoder::SetVersion(int32_t version) {
  m_version = version;
}
void CBC_QRCoder::SetMatrixWidth(int32_t width) {
  m_matrixWidth = width;
}
void CBC_QRCoder::SetMaskPattern(int32_t pattern) {
  m_maskPattern = pattern;
}
void CBC_QRCoder::SetNumDataBytes(int32_t bytes) {
  m_numDataBytes = bytes;
}
void CBC_QRCoder::SetNumTotalBytes(int32_t value) {
  m_numTotalBytes = value;
}
void CBC_QRCoder::SetNumRSBlocks(int32_t block) {
  m_numRSBlocks = block;
}
void CBC_QRCoder::SetNumECBytes(int32_t value) {
  m_numECBytes = value;
}
FX_BOOL CBC_QRCoder::IsValidMaskPattern(int32_t maskPattern) {
  return maskPattern >= 0 && maskPattern < NUM_MASK_PATTERNS;
}
void CBC_QRCoder::SetMatrix(CBC_CommonByteMatrix* value) {
  m_matrix = value;
}
const int32_t CBC_QRCoder::NUM_MASK_PATTERNS = 8;
