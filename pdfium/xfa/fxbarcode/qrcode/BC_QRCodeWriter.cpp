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

#include "xfa/fxbarcode/BC_TwoDimWriter.h"
#include "xfa/fxbarcode/common/BC_CommonByteMatrix.h"
#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderEncoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMode.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderVersion.h"
#include "xfa/fxbarcode/qrcode/BC_QRCodeWriter.h"

CBC_QRCodeWriter::CBC_QRCodeWriter() {
  m_bFixedSize = TRUE;
  m_iCorrectLevel = 1;
  m_iVersion = 0;
}
CBC_QRCodeWriter::~CBC_QRCodeWriter() {}
void CBC_QRCodeWriter::ReleaseAll() {
  delete CBC_ReedSolomonGF256::QRCodeFild;
  CBC_ReedSolomonGF256::QRCodeFild = nullptr;
  delete CBC_ReedSolomonGF256::DataMatrixField;
  CBC_ReedSolomonGF256::DataMatrixField = nullptr;
  CBC_QRCoderMode::Destroy();
  CBC_QRCoderErrorCorrectionLevel::Destroy();
  CBC_QRCoderVersion::Destroy();
}
FX_BOOL CBC_QRCodeWriter::SetVersion(int32_t version) {
  if (version < 0 || version > 40) {
    return FALSE;
  }
  m_iVersion = version;
  return TRUE;
}
FX_BOOL CBC_QRCodeWriter::SetErrorCorrectionLevel(int32_t level) {
  if (level < 0 || level > 3) {
    return FALSE;
  }
  m_iCorrectLevel = level;
  return TRUE;
}
uint8_t* CBC_QRCodeWriter::Encode(const CFX_WideString& contents,
                                  int32_t ecLevel,
                                  int32_t& outWidth,
                                  int32_t& outHeight,
                                  int32_t& e) {
  CBC_QRCoderErrorCorrectionLevel* ec = nullptr;
  switch (ecLevel) {
    case 0:
      ec = CBC_QRCoderErrorCorrectionLevel::L;
      break;
    case 1:
      ec = CBC_QRCoderErrorCorrectionLevel::M;
      break;
    case 2:
      ec = CBC_QRCoderErrorCorrectionLevel::Q;
      break;
    case 3:
      ec = CBC_QRCoderErrorCorrectionLevel::H;
      break;
    default: {
      e = BCExceptionUnSupportEclevel;
      BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
    }
  }
  CBC_QRCoder qr;
  if (m_iVersion > 0 && m_iVersion < 41) {
    CFX_ByteString byteStr = contents.UTF8Encode();
    CBC_QRCoderEncoder::Encode(byteStr, ec, &qr, e, m_iVersion);
  } else {
    CBC_QRCoderEncoder::Encode(contents, ec, &qr, e);
  }
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  outWidth = qr.GetMatrixWidth();
  outHeight = qr.GetMatrixWidth();
  uint8_t* result = FX_Alloc2D(uint8_t, outWidth, outHeight);
  FXSYS_memcpy(result, qr.GetMatrix()->GetArray(), outWidth * outHeight);
  return result;
}
uint8_t* CBC_QRCodeWriter::Encode(const CFX_ByteString& contents,
                                  BCFORMAT format,
                                  int32_t& outWidth,
                                  int32_t& outHeight,
                                  int32_t hints,
                                  int32_t& e) {
  return nullptr;
}
uint8_t* CBC_QRCodeWriter::Encode(const CFX_ByteString& contents,
                                  BCFORMAT format,
                                  int32_t& outWidth,
                                  int32_t& outHeight,
                                  int32_t& e) {
  return nullptr;
}
