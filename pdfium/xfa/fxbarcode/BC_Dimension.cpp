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

#include "xfa/fxbarcode/BC_Dimension.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"

CBC_Dimension::CBC_Dimension() {}
CBC_Dimension::CBC_Dimension(int32_t width, int32_t height, int32_t& e) {
  if (width < 0 || height < 0) {
    e = BCExceptionHeightAndWidthMustBeAtLeast1;
  }
  m_width = width;
  m_height = height;
}
CBC_Dimension::~CBC_Dimension() {}
int32_t CBC_Dimension::getWidth() {
  return m_width;
}
int32_t CBC_Dimension::getHeight() {
  return m_height;
}
int32_t CBC_Dimension::hashCode() {
  return m_width * 32713 + m_height;
}
CFX_WideString CBC_Dimension::toString() {
  return (FX_WCHAR)(m_width + (FX_WCHAR)'x' + m_height);
}
