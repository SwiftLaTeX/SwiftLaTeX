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

#include "xfa/fxbarcode/cbc_codebase.h"

#include "xfa/fxbarcode/BC_Writer.h"

CBC_CodeBase::CBC_CodeBase(CBC_Writer* pWriter) : m_pBCWriter(pWriter) {}

CBC_CodeBase::~CBC_CodeBase() {}

FX_BOOL CBC_CodeBase::SetCharEncoding(int32_t encoding) {
  return m_pBCWriter && m_pBCWriter->SetCharEncoding(encoding);
}

FX_BOOL CBC_CodeBase::SetModuleHeight(int32_t moduleHeight) {
  return m_pBCWriter && m_pBCWriter->SetModuleHeight(moduleHeight);
}

FX_BOOL CBC_CodeBase::SetModuleWidth(int32_t moduleWidth) {
  return m_pBCWriter && m_pBCWriter->SetModuleWidth(moduleWidth);
}

FX_BOOL CBC_CodeBase::SetHeight(int32_t height) {
  return m_pBCWriter && m_pBCWriter->SetHeight(height);
}

FX_BOOL CBC_CodeBase::SetWidth(int32_t width) {
  return m_pBCWriter && m_pBCWriter->SetWidth(width);
}

void CBC_CodeBase::SetBackgroundColor(FX_ARGB backgroundColor) {
  if (m_pBCWriter)
    m_pBCWriter->SetBackgroundColor(backgroundColor);
}

void CBC_CodeBase::SetBarcodeColor(FX_ARGB foregroundColor) {
  if (m_pBCWriter)
    m_pBCWriter->SetBarcodeColor(foregroundColor);
}
