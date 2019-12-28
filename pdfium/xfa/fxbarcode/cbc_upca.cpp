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

#include "xfa/fxbarcode/cbc_upca.h"

#include "xfa/fxbarcode/oned/BC_OnedUPCAWriter.h"

CBC_UPCA::CBC_UPCA() : CBC_OneCode(new CBC_OnedUPCAWriter) {}

CBC_UPCA::~CBC_UPCA() {}

CFX_WideString CBC_UPCA::Preprocess(const CFX_WideStringC& contents) {
  CBC_OnedUPCAWriter* pWriter =
      static_cast<CBC_OnedUPCAWriter*>(m_pBCWriter.get());
  CFX_WideString encodeContents = pWriter->FilterContents(contents);
  int32_t length = encodeContents.GetLength();
  if (length <= 11) {
    for (int32_t i = 0; i < 11 - length; i++)
      encodeContents = FX_WCHAR('0') + encodeContents;

    CFX_ByteString byteString = encodeContents.UTF8Encode();
    int32_t checksum = pWriter->CalcChecksum(byteString);
    byteString += checksum - 0 + '0';
    encodeContents = byteString.UTF8Decode();
  }
  if (length > 12)
    encodeContents = encodeContents.Mid(0, 12);

  return encodeContents;
}

FX_BOOL CBC_UPCA::Encode(const CFX_WideStringC& contents,
                         FX_BOOL isDevice,
                         int32_t& e) {
  if (contents.IsEmpty()) {
    e = BCExceptionNoContents;
    return FALSE;
  }
  BCFORMAT format = BCFORMAT_UPC_A;
  int32_t outWidth = 0;
  int32_t outHeight = 0;
  CFX_WideString encodeContents = Preprocess(contents);
  CFX_ByteString byteString = encodeContents.UTF8Encode();
  m_renderContents = encodeContents;

  CBC_OnedUPCAWriter* pWriter =
      static_cast<CBC_OnedUPCAWriter*>(m_pBCWriter.get());

  pWriter->Init();
  uint8_t* data = pWriter->Encode(byteString, format, outWidth, outHeight, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  pWriter->RenderResult(encodeContents.AsStringC(), data, outWidth, isDevice,
                        e);
  FX_Free(data);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_UPCA::RenderDevice(CFX_RenderDevice* device,
                               const CFX_Matrix* matrix,
                               int32_t& e) {
  static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->RenderDeviceResult(device, matrix, m_renderContents.AsStringC(), e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_UPCA::RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e) {
  static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->RenderBitmapResult(pOutBitmap, m_renderContents.AsStringC(), e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

BC_TYPE CBC_UPCA::GetType() {
  return BC_UPCA;
}
