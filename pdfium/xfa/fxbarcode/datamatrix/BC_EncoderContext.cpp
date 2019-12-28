// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2006-2007 Jeremias Maerki.
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
#include "xfa/fxbarcode/BC_UtilCodingConvert.h"
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EncoderContext.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"

CBC_EncoderContext::CBC_EncoderContext(const CFX_WideString msg,
                                       CFX_WideString ecLevel,
                                       int32_t& e) {
  CFX_ByteString dststr;
  CBC_UtilCodingConvert::UnicodeToUTF8(msg, dststr);
  CFX_WideString sb;
  int32_t c = dststr.GetLength();
  for (int32_t i = 0; i < c; i++) {
    FX_WCHAR ch = (FX_WCHAR)(dststr.GetAt(i) & 0xff);
    if (ch == '?' && dststr.GetAt(i) != '?') {
      e = BCExceptionCharactersOutsideISO88591Encoding;
    }
    sb += ch;
  }
  m_msg = sb;
  m_shape = FORCE_NONE;
  m_newEncoding = -1;
  m_pos = 0;
  m_symbolInfo = nullptr;
  m_skipAtEnd = 0;
  m_maxSize = nullptr;
  m_minSize = nullptr;
}
CBC_EncoderContext::~CBC_EncoderContext() {}
void CBC_EncoderContext::setSymbolShape(SymbolShapeHint shape) {
  m_shape = shape;
}
void CBC_EncoderContext::setSizeConstraints(CBC_Dimension* minSize,
                                            CBC_Dimension* maxSize) {
  m_maxSize = maxSize;
  m_minSize = minSize;
}
CFX_WideString CBC_EncoderContext::getMessage() {
  return m_msg;
}
void CBC_EncoderContext::setSkipAtEnd(int32_t count) {
  m_skipAtEnd = count;
}
FX_WCHAR CBC_EncoderContext::getCurrentChar() {
  return m_msg.GetAt(m_pos);
}
FX_WCHAR CBC_EncoderContext::getCurrent() {
  return m_msg.GetAt(m_pos);
}
void CBC_EncoderContext::writeCodewords(CFX_WideString codewords) {
  m_codewords += codewords;
}
void CBC_EncoderContext::writeCodeword(FX_WCHAR codeword) {
  m_codewords += codeword;
}
int32_t CBC_EncoderContext::getCodewordCount() {
  return m_codewords.GetLength();
}
void CBC_EncoderContext::signalEncoderChange(int32_t encoding) {
  m_newEncoding = encoding;
}
void CBC_EncoderContext::resetEncoderSignal() {
  m_newEncoding = -1;
}
FX_BOOL CBC_EncoderContext::hasMoreCharacters() {
  return m_pos < getTotalMessageCharCount();
}
int32_t CBC_EncoderContext::getRemainingCharacters() {
  return getTotalMessageCharCount() - m_pos;
}
void CBC_EncoderContext::updateSymbolInfo(int32_t& e) {
  updateSymbolInfo(getCodewordCount(), e);
}
void CBC_EncoderContext::updateSymbolInfo(int32_t len, int32_t& e) {
  if (!m_symbolInfo || len > m_symbolInfo->m_dataCapacity) {
    m_symbolInfo =
        CBC_SymbolInfo::lookup(len, m_shape, m_minSize, m_maxSize, true, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_EncoderContext::resetSymbolInfo() {
  m_shape = FORCE_NONE;
}
int32_t CBC_EncoderContext::getTotalMessageCharCount() {
  return m_msg.GetLength() - m_skipAtEnd;
}
