// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2012 ZXing authors
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
#include "xfa/fxbarcode/common/BC_CommonBitArray.h"
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417BarcodeMatrix.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417Compaction.h"
#include "xfa/fxbarcode/pdf417/BC_PDF417Writer.h"

CBC_PDF417Writer::CBC_PDF417Writer() {
  m_bFixedSize = FALSE;
}
CBC_PDF417Writer::~CBC_PDF417Writer() {
  m_bTruncated = TRUE;
}
FX_BOOL CBC_PDF417Writer::SetErrorCorrectionLevel(int32_t level) {
  if (level < 0 || level > 8) {
    return FALSE;
  }
  m_iCorrectLevel = level;
  return TRUE;
}
void CBC_PDF417Writer::SetTruncated(FX_BOOL truncated) {
  m_bTruncated = truncated;
}
uint8_t* CBC_PDF417Writer::Encode(const CFX_WideString& contents,
                                  int32_t& outWidth,
                                  int32_t& outHeight,
                                  int32_t& e) {
  CBC_PDF417 encoder;
  int32_t col = (m_Width / m_ModuleWidth - 69) / 17;
  int32_t row = m_Height / (m_ModuleWidth * 20);
  if (row >= 3 && row <= 90 && col >= 1 && col <= 30) {
    encoder.setDimensions(col, col, row, row);
  } else if (col >= 1 && col <= 30) {
    encoder.setDimensions(col, col, 90, 3);
  } else if (row >= 3 && row <= 90) {
    encoder.setDimensions(30, 1, row, row);
  }
  encoder.generateBarcodeLogic(contents, m_iCorrectLevel, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  int32_t lineThickness = 2;
  int32_t aspectRatio = 4;
  CBC_BarcodeMatrix* barcodeMatrix = encoder.getBarcodeMatrix();
  CFX_ByteArray originalScale;
  originalScale.Copy(barcodeMatrix->getScaledMatrix(
      lineThickness, aspectRatio * lineThickness));
  int32_t width = outWidth;
  int32_t height = outHeight;
  outWidth = barcodeMatrix->getWidth();
  outHeight = barcodeMatrix->getHeight();
  FX_BOOL rotated = FALSE;
  if ((height > width) ^ (outWidth < outHeight)) {
    rotateArray(originalScale, outHeight, outWidth);
    rotated = TRUE;
    int32_t temp = outHeight;
    outHeight = outWidth;
    outWidth = temp;
  }
  int32_t scaleX = width / outWidth;
  int32_t scaleY = height / outHeight;
  int32_t scale;
  if (scaleX < scaleY) {
    scale = scaleX;
  } else {
    scale = scaleY;
  }
  if (scale > 1) {
    originalScale.RemoveAll();
    originalScale.Copy(barcodeMatrix->getScaledMatrix(
        scale * lineThickness, scale * aspectRatio * lineThickness));
    if (rotated) {
      rotateArray(originalScale, outHeight, outWidth);
      int32_t temp = outHeight;
      outHeight = outWidth;
      outWidth = temp;
    }
  }
  uint8_t* result = FX_Alloc2D(uint8_t, outHeight, outWidth);
  FXSYS_memcpy(result, originalScale.GetData(), outHeight * outWidth);
  return result;
}
void CBC_PDF417Writer::rotateArray(CFX_ByteArray& bitarray,
                                   int32_t height,
                                   int32_t width) {
  CFX_ByteArray temp;
  temp.Copy(bitarray);
  for (int32_t ii = 0; ii < height; ii++) {
    int32_t inverseii = height - ii - 1;
    for (int32_t jj = 0; jj < width; jj++) {
      bitarray[jj * height + inverseii] = temp[ii * width + jj];
    }
  }
}
