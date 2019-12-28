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

#include "xfa/fxbarcode/cbc_onecode.h"

#include "xfa/fxbarcode/oned/BC_OneDimWriter.h"

CBC_OneCode::CBC_OneCode(CBC_Writer* pWriter) : CBC_CodeBase(pWriter) {}

CBC_OneCode::~CBC_OneCode() {}

FX_BOOL CBC_OneCode::CheckContentValidity(const CFX_WideStringC& contents) {
  return m_pBCWriter &&
         static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
             ->CheckContentValidity(contents);
}

CFX_WideString CBC_OneCode::FilterContents(const CFX_WideStringC& contents) {
  if (!m_pBCWriter)
    return CFX_WideString();
  return static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
      ->FilterContents(contents);
}

void CBC_OneCode::SetPrintChecksum(FX_BOOL checksum) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())
        ->SetPrintChecksum(checksum);
}

void CBC_OneCode::SetDataLength(int32_t length) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetDataLength(length);
}

void CBC_OneCode::SetCalChecksum(FX_BOOL calc) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetCalcChecksum(calc);
}

FX_BOOL CBC_OneCode::SetFont(CFX_Font* cFont) {
  if (m_pBCWriter)
    return static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetFont(cFont);
  return FALSE;
}

void CBC_OneCode::SetFontSize(FX_FLOAT size) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetFontSize(size);
}

void CBC_OneCode::SetFontStyle(int32_t style) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetFontStyle(style);
}

void CBC_OneCode::SetFontColor(FX_ARGB color) {
  if (m_pBCWriter)
    static_cast<CBC_OneDimWriter*>(m_pBCWriter.get())->SetFontColor(color);
}
