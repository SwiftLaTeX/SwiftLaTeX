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

#include "xfa/fxbarcode/cbc_code39.h"

#include "xfa/fxbarcode/oned/BC_OnedCode39Writer.h"

CBC_Code39::CBC_Code39() : CBC_OneCode(new CBC_OnedCode39Writer) {}

CBC_Code39::~CBC_Code39() {}

FX_BOOL CBC_Code39::Encode(const CFX_WideStringC& contents,
                           FX_BOOL isDevice,
                           int32_t& e) {
  if (contents.IsEmpty()) {
    e = BCExceptionNoContents;
    return FALSE;
  }
  BCFORMAT format = BCFORMAT_CODE_39;
  int32_t outWidth = 0;
  int32_t outHeight = 0;
  CFX_WideString filtercontents =
      static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
          ->FilterContents(contents);
  CFX_WideString renderContents =
      static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
          ->RenderTextContents(contents);
  m_renderContents = renderContents;
  CFX_ByteString byteString = filtercontents.UTF8Encode();
  uint8_t* data = static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
                      ->Encode(byteString, format, outWidth, outHeight, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->RenderResult(renderContents.AsStringC(), data, outWidth, isDevice, e);
  FX_Free(data);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_Code39::RenderDevice(CFX_RenderDevice* device,
                                 const CFX_Matrix* matrix,
                                 int32_t& e) {
  CFX_WideString renderCon =
      static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
          ->encodedContents(m_renderContents.AsStringC(), e);
  static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->RenderDeviceResult(device, matrix, renderCon.AsStringC(), e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

FX_BOOL CBC_Code39::RenderBitmap(CFX_DIBitmap*& pOutBitmap, int32_t& e) {
  CFX_WideString renderCon =
      static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
          ->encodedContents(m_renderContents.AsStringC(), e);
  static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->RenderBitmapResult(pOutBitmap, renderCon.AsStringC(), e);
  BC_EXCEPTION_CHECK_ReturnValue(e, FALSE);
  return TRUE;
}

BC_TYPE CBC_Code39::GetType() {
  return BC_CODE39;
}

FX_BOOL CBC_Code39::SetTextLocation(BC_TEXT_LOC location) {
  if (m_pBCWriter)
    return static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
        ->SetTextLocation(location);
  return FALSE;
}

FX_BOOL CBC_Code39::SetWideNarrowRatio(int32_t ratio) {
  if (m_pBCWriter)
    return static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get())
        ->SetWideNarrowRatio(ratio);
  return FALSE;
}
