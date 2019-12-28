// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2010 ZXing authors
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

#include "xfa/fxbarcode/BC_Writer.h"
#include "xfa/fxbarcode/oned/BC_OneDimWriter.h"
#include "xfa/fxbarcode/oned/BC_OnedCode128Writer.h"

namespace {

const int32_t CODE_PATTERNS[107][7] = {
    {2, 1, 2, 2, 2, 2, 0}, {2, 2, 2, 1, 2, 2, 0}, {2, 2, 2, 2, 2, 1, 0},
    {1, 2, 1, 2, 2, 3, 0}, {1, 2, 1, 3, 2, 2, 0}, {1, 3, 1, 2, 2, 2, 0},
    {1, 2, 2, 2, 1, 3, 0}, {1, 2, 2, 3, 1, 2, 0}, {1, 3, 2, 2, 1, 2, 0},
    {2, 2, 1, 2, 1, 3, 0}, {2, 2, 1, 3, 1, 2, 0}, {2, 3, 1, 2, 1, 2, 0},
    {1, 1, 2, 2, 3, 2, 0}, {1, 2, 2, 1, 3, 2, 0}, {1, 2, 2, 2, 3, 1, 0},
    {1, 1, 3, 2, 2, 2, 0}, {1, 2, 3, 1, 2, 2, 0}, {1, 2, 3, 2, 2, 1, 0},
    {2, 2, 3, 2, 1, 1, 0}, {2, 2, 1, 1, 3, 2, 0}, {2, 2, 1, 2, 3, 1, 0},
    {2, 1, 3, 2, 1, 2, 0}, {2, 2, 3, 1, 1, 2, 0}, {3, 1, 2, 1, 3, 1, 0},
    {3, 1, 1, 2, 2, 2, 0}, {3, 2, 1, 1, 2, 2, 0}, {3, 2, 1, 2, 2, 1, 0},
    {3, 1, 2, 2, 1, 2, 0}, {3, 2, 2, 1, 1, 2, 0}, {3, 2, 2, 2, 1, 1, 0},
    {2, 1, 2, 1, 2, 3, 0}, {2, 1, 2, 3, 2, 1, 0}, {2, 3, 2, 1, 2, 1, 0},
    {1, 1, 1, 3, 2, 3, 0}, {1, 3, 1, 1, 2, 3, 0}, {1, 3, 1, 3, 2, 1, 0},
    {1, 1, 2, 3, 1, 3, 0}, {1, 3, 2, 1, 1, 3, 0}, {1, 3, 2, 3, 1, 1, 0},
    {2, 1, 1, 3, 1, 3, 0}, {2, 3, 1, 1, 1, 3, 0}, {2, 3, 1, 3, 1, 1, 0},
    {1, 1, 2, 1, 3, 3, 0}, {1, 1, 2, 3, 3, 1, 0}, {1, 3, 2, 1, 3, 1, 0},
    {1, 1, 3, 1, 2, 3, 0}, {1, 1, 3, 3, 2, 1, 0}, {1, 3, 3, 1, 2, 1, 0},
    {3, 1, 3, 1, 2, 1, 0}, {2, 1, 1, 3, 3, 1, 0}, {2, 3, 1, 1, 3, 1, 0},
    {2, 1, 3, 1, 1, 3, 0}, {2, 1, 3, 3, 1, 1, 0}, {2, 1, 3, 1, 3, 1, 0},
    {3, 1, 1, 1, 2, 3, 0}, {3, 1, 1, 3, 2, 1, 0}, {3, 3, 1, 1, 2, 1, 0},
    {3, 1, 2, 1, 1, 3, 0}, {3, 1, 2, 3, 1, 1, 0}, {3, 3, 2, 1, 1, 1, 0},
    {3, 1, 4, 1, 1, 1, 0}, {2, 2, 1, 4, 1, 1, 0}, {4, 3, 1, 1, 1, 1, 0},
    {1, 1, 1, 2, 2, 4, 0}, {1, 1, 1, 4, 2, 2, 0}, {1, 2, 1, 1, 2, 4, 0},
    {1, 2, 1, 4, 2, 1, 0}, {1, 4, 1, 1, 2, 2, 0}, {1, 4, 1, 2, 2, 1, 0},
    {1, 1, 2, 2, 1, 4, 0}, {1, 1, 2, 4, 1, 2, 0}, {1, 2, 2, 1, 1, 4, 0},
    {1, 2, 2, 4, 1, 1, 0}, {1, 4, 2, 1, 1, 2, 0}, {1, 4, 2, 2, 1, 1, 0},
    {2, 4, 1, 2, 1, 1, 0}, {2, 2, 1, 1, 1, 4, 0}, {4, 1, 3, 1, 1, 1, 0},
    {2, 4, 1, 1, 1, 2, 0}, {1, 3, 4, 1, 1, 1, 0}, {1, 1, 1, 2, 4, 2, 0},
    {1, 2, 1, 1, 4, 2, 0}, {1, 2, 1, 2, 4, 1, 0}, {1, 1, 4, 2, 1, 2, 0},
    {1, 2, 4, 1, 1, 2, 0}, {1, 2, 4, 2, 1, 1, 0}, {4, 1, 1, 2, 1, 2, 0},
    {4, 2, 1, 1, 1, 2, 0}, {4, 2, 1, 2, 1, 1, 0}, {2, 1, 2, 1, 4, 1, 0},
    {2, 1, 4, 1, 2, 1, 0}, {4, 1, 2, 1, 2, 1, 0}, {1, 1, 1, 1, 4, 3, 0},
    {1, 1, 1, 3, 4, 1, 0}, {1, 3, 1, 1, 4, 1, 0}, {1, 1, 4, 1, 1, 3, 0},
    {1, 1, 4, 3, 1, 1, 0}, {4, 1, 1, 1, 1, 3, 0}, {4, 1, 1, 3, 1, 1, 0},
    {1, 1, 3, 1, 4, 1, 0}, {1, 1, 4, 1, 3, 1, 0}, {3, 1, 1, 1, 4, 1, 0},
    {4, 1, 1, 1, 3, 1, 0}, {2, 1, 1, 4, 1, 2, 0}, {2, 1, 1, 2, 1, 4, 0},
    {2, 1, 1, 2, 3, 2, 0}, {2, 3, 3, 1, 1, 1, 2}};

const int32_t CODE_START_B = 104;
const int32_t CODE_START_C = 105;
const int32_t CODE_STOP = 106;

}  // namespace

CBC_OnedCode128Writer::CBC_OnedCode128Writer() {
  m_codeFormat = BC_CODE128_B;
}
CBC_OnedCode128Writer::CBC_OnedCode128Writer(BC_TYPE type) {
  m_codeFormat = type;
}
CBC_OnedCode128Writer::~CBC_OnedCode128Writer() {}
BC_TYPE CBC_OnedCode128Writer::GetType() {
  return m_codeFormat;
}
FX_BOOL CBC_OnedCode128Writer::CheckContentValidity(
    const CFX_WideStringC& contents) {
  FX_BOOL ret = TRUE;
  int32_t position = 0;
  int32_t patternIndex = -1;
  if (m_codeFormat == BC_CODE128_B || m_codeFormat == BC_CODE128_C) {
    while (position < contents.GetLength()) {
      patternIndex = (int32_t)contents.GetAt(position);
      if (patternIndex < 32 || patternIndex > 126 || patternIndex == 34) {
        ret = FALSE;
        break;
      }
      position++;
    }
  } else {
    ret = FALSE;
  }
  return ret;
}
CFX_WideString CBC_OnedCode128Writer::FilterContents(
    const CFX_WideStringC& contents) {
  CFX_WideString filterChineseChar;
  FX_WCHAR ch;
  for (int32_t i = 0; i < contents.GetLength(); i++) {
    ch = contents.GetAt(i);
    if (ch > 175) {
      i++;
      continue;
    }
    filterChineseChar += ch;
  }
  CFX_WideString filtercontents;
  if (m_codeFormat == BC_CODE128_B) {
    for (int32_t i = 0; i < filterChineseChar.GetLength(); i++) {
      ch = filterChineseChar.GetAt(i);
      if (ch >= 32 && ch <= 126) {
        filtercontents += ch;
      } else {
        continue;
      }
    }
  } else if (m_codeFormat == BC_CODE128_C) {
    for (int32_t i = 0; i < filterChineseChar.GetLength(); i++) {
      ch = filterChineseChar.GetAt(i);
      if (ch >= 32 && ch <= 106) {
        filtercontents += ch;
      } else {
        continue;
      }
    }
  } else {
    filtercontents = contents;
  }
  return filtercontents;
}
FX_BOOL CBC_OnedCode128Writer::SetTextLocation(BC_TEXT_LOC location) {
  if (location < BC_TEXT_LOC_NONE || location > BC_TEXT_LOC_BELOWEMBED) {
    return FALSE;
  }
  m_locTextLoc = location;
  return TRUE;
}
uint8_t* CBC_OnedCode128Writer::Encode(const CFX_ByteString& contents,
                                       BCFORMAT format,
                                       int32_t& outWidth,
                                       int32_t& outHeight,
                                       int32_t hints,
                                       int32_t& e) {
  if (format != BCFORMAT_CODE_128) {
    e = BCExceptionOnlyEncodeCODE_128;
    return nullptr;
  }
  uint8_t* ret =
      CBC_OneDimWriter::Encode(contents, format, outWidth, outHeight, hints, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}
uint8_t* CBC_OnedCode128Writer::Encode(const CFX_ByteString& contents,
                                       BCFORMAT format,
                                       int32_t& outWidth,
                                       int32_t& outHeight,
                                       int32_t& e) {
  uint8_t* ret = Encode(contents, format, outWidth, outHeight, 0, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}
FX_BOOL CBC_OnedCode128Writer::IsDigits(const CFX_ByteString& contents,
                                        int32_t start,
                                        int32_t length) {
  int32_t end = start + length;
  for (int32_t i = start; i < end; i++) {
    if (contents[i] < '0' || contents[i] > '9') {
      return FALSE;
    }
  }
  return TRUE;
}

uint8_t* CBC_OnedCode128Writer::Encode(const CFX_ByteString& contents,
                                       int32_t& outLength,
                                       int32_t& e) {
  if (contents.GetLength() < 1 || contents.GetLength() > 80) {
    e = BCExceptionContentsLengthShouldBetween1and80;
    return nullptr;
  }
  CFX_ArrayTemplate<const int32_t*> patterns;
  int32_t checkSum = 0;
  if (m_codeFormat == BC_CODE128_B) {
    checkSum = Encode128B(contents, &patterns);
  } else if (m_codeFormat == BC_CODE128_C) {
    checkSum = Encode128C(contents, &patterns);
  } else {
    e = BCExceptionFormatException;
    return nullptr;
  }
  checkSum %= 103;
  patterns.Add(CODE_PATTERNS[checkSum]);
  patterns.Add(CODE_PATTERNS[CODE_STOP]);
  m_iContentLen = contents.GetLength() + 3;
  int32_t codeWidth = 0;
  for (int32_t k = 0; k < patterns.GetSize(); k++) {
    const int32_t* pattern = patterns[k];
    for (int32_t j = 0; j < 7; j++) {
      codeWidth += pattern[j];
    }
  }
  outLength = codeWidth;
  uint8_t* result = FX_Alloc(uint8_t, outLength);
  int32_t pos = 0;
  for (int32_t j = 0; j < patterns.GetSize(); j++) {
    const int32_t* pattern = patterns[j];
    pos += AppendPattern(result, pos, pattern, 7, 1, e);
    if (e != BCExceptionNO) {
      FX_Free(result);
      return nullptr;
    }
  }
  return result;
}

int32_t CBC_OnedCode128Writer::Encode128B(
    const CFX_ByteString& contents,
    CFX_ArrayTemplate<const int32_t*>* patterns) {
  int32_t checkSum = 0;
  int32_t checkWeight = 1;
  int32_t position = 0;
  patterns->Add(CODE_PATTERNS[CODE_START_B]);
  checkSum += CODE_START_B * checkWeight;
  while (position < contents.GetLength()) {
    int32_t patternIndex = 0;
    patternIndex = contents[position] - ' ';
    position += 1;
    patterns->Add(CODE_PATTERNS[patternIndex]);
    checkSum += patternIndex * checkWeight;
    if (position != 0) {
      checkWeight++;
    }
  }
  return checkSum;
}

int32_t CBC_OnedCode128Writer::Encode128C(
    const CFX_ByteString& contents,
    CFX_ArrayTemplate<const int32_t*>* patterns) {
  int32_t checkSum = 0;
  int32_t checkWeight = 1;
  int32_t position = 0;
  patterns->Add(CODE_PATTERNS[CODE_START_C]);
  checkSum += CODE_START_C * checkWeight;
  while (position < contents.GetLength()) {
    int32_t patternIndex = 0;
    FX_CHAR ch = contents.GetAt(position);
    if (ch < '0' || ch > '9') {
      patternIndex = (int32_t)ch;
      position++;
    } else {
      patternIndex = FXSYS_atoi(contents.Mid(position, 2).c_str());
      if (contents.GetAt(position + 1) < '0' ||
          contents.GetAt(position + 1) > '9') {
        position += 1;
      } else {
        position += 2;
      }
    }
    patterns->Add(CODE_PATTERNS[patternIndex]);
    checkSum += patternIndex * checkWeight;
    if (position != 0) {
      checkWeight++;
    }
  }
  return checkSum;
}
