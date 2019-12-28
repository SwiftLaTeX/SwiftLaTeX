// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <stdint.h>

#include "xfa/fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "xfa/fxbarcode/datamatrix/BC_ErrorCorrection.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417HighLevelEncoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMode.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderVersion.h"

void BC_Library_Init() {
  CBC_QRCoderErrorCorrectionLevel::Initialize();
  CBC_QRCoderMode::Initialize();
  CBC_QRCoderVersion::Initialize();
  CBC_ReedSolomonGF256::Initialize();
  CBC_SymbolInfo::Initialize();
  CBC_ErrorCorrection::Initialize();
  CBC_PDF417HighLevelEncoder::Initialize();
}
void BC_Library_Destory() {
  CBC_QRCoderErrorCorrectionLevel::Finalize();
  CBC_QRCoderMode::Finalize();
  CBC_QRCoderVersion::Finalize();
  CBC_ReedSolomonGF256::Finalize();
  CBC_SymbolInfo::Finalize();
  CBC_ErrorCorrection::Finalize();
  CBC_PDF417HighLevelEncoder::Finalize();
}
