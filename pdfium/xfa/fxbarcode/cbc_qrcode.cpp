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

#include "xfa/fxbarcode/cbc_qrcode.h"

#include "xfa/fxbarcode/qrcode/BC_QRCodeWriter.h"

CBC_QRCode::CBC_QRCode() : CBC_CodeBase(new CBC_QRCodeWriter) {}

CBC_QRCode::~CBC_QRCode() {}

FX_BOOL CBC_QRCode::SetVersion(int32_t version) {
  if (version < 0 || version > 40)
    return FALSE;
  return m_pBCWriter &&
         static_cast<CBC_QRCodeWriter*>(m_pBCWriter.get())->SetVersion(version);
}

FX_BOOL CBC_QRCode::SetErrorCorrectionLevel(int32_t level) {
  if (level < 0 || level > 3)
    return FALSE;

  return m_pBCWriter &&
         static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
             ->SetErrorCorrectionLevel(level);
}

FX_BOOL CBC_QRCode::Encode(const CFX_WideStringC& contents,
                           FX_BOOL isDevice,
                           int32_t& e) {
  int32_t outWidth = 0;
  int32_t outHeight = 0;
  CBC_QRCodeWriter* pWriter = static_cast<CBC_QRCodeWriter*>(m_pBCWriter.get());
  uint8_t* data = pWriter->Encode(CFX_WideString(contents),
                                  pWriter->GetErrorCorrectionLevel(), outWidth,
                                  outHeight, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  pWriter->RenderResult(data, outWidth, outHeight, e);
  FX_Free(data);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_QRCode::RenderDevice(CFX_RenderDevice* device,
                                 const CFX_Matrix* matrix,
                                 int32_t& e) {
  static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
      ->RenderDeviceResult(device, matrix);
  return TRUE;
}

FX_BOOL CBC_QRCode::RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e) {
  static_cast<CBC_TwoDimWriter*>(m_pBCWriter.get())
      ->RenderBitmapResult(pOutBitmap, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

BC_TYPE CBC_QRCode::GetType() {
  return BC_QR_CODE;
}
