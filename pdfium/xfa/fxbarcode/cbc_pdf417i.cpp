// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
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

#include "xfa/fxbarcode/cbc_pdf417i.h"

#include "xfa/fxbarcode/pdf417/BC_PDF417Writer.h"

CBC_PDF417I::CBC_PDF417I() : CBC_CodeBase(new CBC_PDF417Writer) {}

CBC_PDF417I::~CBC_PDF417I() {}

FX_BOOL CBC_PDF417I::SetErrorCorrectionLevel(int32_t level) {
  static_cast<CBC_PDF417Writer*>(m_pBCWriter.get())
      ->SetErrorCorrectionLevel(level);
  return TRUE;
}

void CBC_PDF417I::SetTruncated(FX_BOOL truncated) {
  static_cast<CBC_PDF417Writer*>(m_pBCWriter.get())->SetTruncated(truncated);
}

FX_BOOL CBC_PDF417I::Encode(const CFX_WideStringC& contents,
                            FX_BOOL isDevice,
                            int32_t& e) {
  int32_t outWidth = 0;
  int32_t outHeight = 0;
  uint8_t* data =
      static_cast<CBC_PDF417Writer*>(m_pBCWriter.get())
          ->Encode(CFX_WideString(contents), outWidth, outHeight, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
      ->RenderResult(data, outWidth, outHeight, e);
  FX_Free(data);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_PDF417I::RenderDevice(CFX_RenderDevice* device,
                                  const CFX_Matrix* matrix,
                                  int32_t& e) {
  static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
      ->RenderDeviceResult(device, matrix);
  return TRUE;
}

FX_BOOL CBC_PDF417I::RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e) {
  static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
      ->RenderBitmapResult(pOutBitmap, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

BC_TYPE CBC_PDF417I::GetType() {
  return BC_PDF417;
}
