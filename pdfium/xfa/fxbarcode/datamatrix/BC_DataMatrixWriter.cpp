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

#include "xfa/fxbarcode/BC_Dimension.h"
#include "xfa/fxbarcode/BC_TwoDimWriter.h"
#include "xfa/fxbarcode/BC_UtilCodingConvert.h"
#include "xfa/fxbarcode/BC_Writer.h"
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/common/BC_CommonByteMatrix.h"
#include "xfa/fxbarcode/datamatrix/BC_ASCIIEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Base256Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_C40Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_DataMatrixSymbolInfo144.h"
#include "xfa/fxbarcode/datamatrix/BC_DataMatrixWriter.h"
#include "xfa/fxbarcode/datamatrix/BC_DefaultPlacement.h"
#include "xfa/fxbarcode/datamatrix/BC_EdifactEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EncoderContext.h"
#include "xfa/fxbarcode/datamatrix/BC_ErrorCorrection.h"
#include "xfa/fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"
#include "xfa/fxbarcode/datamatrix/BC_TextEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_X12Encoder.h"

CBC_DataMatrixWriter::CBC_DataMatrixWriter() {}
CBC_DataMatrixWriter::~CBC_DataMatrixWriter() {}
FX_BOOL CBC_DataMatrixWriter::SetErrorCorrectionLevel(int32_t level) {
  m_iCorrectLevel = level;
  return TRUE;
}
uint8_t* CBC_DataMatrixWriter::Encode(const CFX_WideString& contents,
                                      int32_t& outWidth,
                                      int32_t& outHeight,
                                      int32_t& e) {
  if (outWidth < 0 || outHeight < 0) {
    e = BCExceptionHeightAndWidthMustBeAtLeast1;
    BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  }
  CBC_SymbolShapeHint::SymbolShapeHint shape =
      CBC_SymbolShapeHint::FORCE_SQUARE;
  CBC_Dimension* minSize = nullptr;
  CBC_Dimension* maxSize = nullptr;
  CFX_WideString ecLevel;
  CFX_WideString encoded = CBC_HighLevelEncoder::encodeHighLevel(
      contents, ecLevel, shape, minSize, maxSize, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  CBC_SymbolInfo* symbolInfo = CBC_SymbolInfo::lookup(
      encoded.GetLength(), shape, minSize, maxSize, TRUE, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  CFX_WideString codewords =
      CBC_ErrorCorrection::encodeECC200(encoded, symbolInfo, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  CBC_DefaultPlacement* placement =
      new CBC_DefaultPlacement(codewords, symbolInfo->getSymbolDataWidth(e),
                               symbolInfo->getSymbolDataHeight(e));
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  placement->place();
  CBC_CommonByteMatrix* bytematrix = encodeLowLevel(placement, symbolInfo, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  outWidth = bytematrix->GetWidth();
  outHeight = bytematrix->GetHeight();
  uint8_t* result = FX_Alloc2D(uint8_t, outWidth, outHeight);
  FXSYS_memcpy(result, bytematrix->GetArray(), outWidth * outHeight);
  delete bytematrix;
  delete placement;
  return result;
}
CBC_CommonByteMatrix* CBC_DataMatrixWriter::encodeLowLevel(
    CBC_DefaultPlacement* placement,
    CBC_SymbolInfo* symbolInfo,
    int32_t& e) {
  int32_t symbolWidth = symbolInfo->getSymbolDataWidth(e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  int32_t symbolHeight = symbolInfo->getSymbolDataHeight(e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  CBC_CommonByteMatrix* matrix = new CBC_CommonByteMatrix(
      symbolInfo->getSymbolWidth(e), symbolInfo->getSymbolHeight(e));
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  matrix->Init();
  int32_t matrixY = 0;
  for (int32_t y = 0; y < symbolHeight; y++) {
    int32_t matrixX;
    if ((y % symbolInfo->m_matrixHeight) == 0) {
      matrixX = 0;
      for (int32_t x = 0; x < symbolInfo->getSymbolWidth(e); x++) {
        matrix->Set(matrixX, matrixY, (x % 2) == 0);
        matrixX++;
      }
      matrixY++;
    }
    matrixX = 0;
    for (int32_t x = 0; x < symbolWidth; x++) {
      if ((x % symbolInfo->m_matrixWidth) == 0) {
        matrix->Set(matrixX, matrixY, TRUE);
        matrixX++;
      }
      matrix->Set(matrixX, matrixY, placement->getBit(x, y));
      matrixX++;
      if ((x % symbolInfo->m_matrixWidth) == symbolInfo->m_matrixWidth - 1) {
        matrix->Set(matrixX, matrixY, (y % 2) == 0);
        matrixX++;
      }
    }
    matrixY++;
    if ((y % symbolInfo->m_matrixHeight) == symbolInfo->m_matrixHeight - 1) {
      matrixX = 0;
      for (int32_t x = 0; x < symbolInfo->getSymbolWidth(e); x++) {
        matrix->Set(matrixX, matrixY, TRUE);
        matrixX++;
      }
      matrixY++;
    }
  }
  return matrix;
}
