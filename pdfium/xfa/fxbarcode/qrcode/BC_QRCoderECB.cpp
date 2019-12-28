// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
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

#include "xfa/fxbarcode/qrcode/BC_QRCoderECB.h"

CBC_QRCoderECB::CBC_QRCoderECB(int32_t count, int32_t dataCodeWords) {
  m_dataCodeWords = dataCodeWords;
  m_count = count;
}
CBC_QRCoderECB::~CBC_QRCoderECB() {}
int32_t CBC_QRCoderECB::GetCount() {
  return m_count;
}
int32_t CBC_QRCoderECB::GetDataCodeWords() {
  return m_dataCodeWords;
}
