// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
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

#include "xfa/fxbarcode/BC_Writer.h"
#include "xfa/fxbarcode/common/BC_CommonBitArray.h"
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/oned/BC_OneDimWriter.h"
#include "xfa/fxbarcode/oned/BC_OnedCodaBarWriter.h"

namespace {

const FX_CHAR ALPHABET_STRING[] = "0123456789-$:/.+ABCDTN";

const int32_t CHARACTER_ENCODINGS[22] = {
    0x003, 0x006, 0x009, 0x060, 0x012, 0x042, 0x021, 0x024,
    0x030, 0x048, 0x00c, 0x018, 0x045, 0x051, 0x054, 0x015,
    0x01A, 0x029, 0x00B, 0x00E, 0x01A, 0x029};

const FX_CHAR START_END_CHARS[] = {'A', 'B', 'C', 'D', 'T', 'N', '*', 'E',
                                   'a', 'b', 'c', 'd', 't', 'n', 'e'};
const FX_CHAR CONTENT_CHARS[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', '-', '$', '/', ':', '+', '.'};

}  // namespace

CBC_OnedCodaBarWriter::CBC_OnedCodaBarWriter() {
  m_chStart = 'A';
  m_chEnd = 'B';
  m_iWideNarrRatio = 2;
}
CBC_OnedCodaBarWriter::~CBC_OnedCodaBarWriter() {}
FX_BOOL CBC_OnedCodaBarWriter::SetStartChar(FX_CHAR start) {
  for (size_t i = 0; i < FX_ArraySize(START_END_CHARS); ++i) {
    if (START_END_CHARS[i] == start) {
      m_chStart = start;
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CBC_OnedCodaBarWriter::SetEndChar(FX_CHAR end) {
  for (size_t i = 0; i < FX_ArraySize(START_END_CHARS); ++i) {
    if (START_END_CHARS[i] == end) {
      m_chEnd = end;
      return TRUE;
    }
  }
  return FALSE;
}
void CBC_OnedCodaBarWriter::SetDataLength(int32_t length) {
  m_iDataLenth = length + 2;
}
FX_BOOL CBC_OnedCodaBarWriter::SetTextLocation(BC_TEXT_LOC location) {
  if (location < BC_TEXT_LOC_NONE || location > BC_TEXT_LOC_BELOWEMBED) {
    return FALSE;
  }
  m_locTextLoc = location;
  return TRUE;
}
FX_BOOL CBC_OnedCodaBarWriter::SetWideNarrowRatio(int32_t ratio) {
  if (ratio < 2 || ratio > 3) {
    return FALSE;
  }
  m_iWideNarrRatio = ratio;
  return TRUE;
}
FX_BOOL CBC_OnedCodaBarWriter::FindChar(FX_WCHAR ch, FX_BOOL isContent) {
  if (isContent) {
    for (size_t i = 0; i < FX_ArraySize(CONTENT_CHARS); ++i) {
      if (ch == (FX_WCHAR)CONTENT_CHARS[i]) {
        return TRUE;
      }
    }
    for (size_t j = 0; j < FX_ArraySize(START_END_CHARS); ++j) {
      if (ch == (FX_WCHAR)START_END_CHARS[j]) {
        return TRUE;
      }
    }
    return FALSE;
  } else {
    for (size_t i = 0; i < FX_ArraySize(CONTENT_CHARS); ++i) {
      if (ch == (FX_WCHAR)CONTENT_CHARS[i]) {
        return TRUE;
      }
    }
    return FALSE;
  }
}
FX_BOOL CBC_OnedCodaBarWriter::CheckContentValidity(
    const CFX_WideStringC& contents) {
  FX_WCHAR ch;
  int32_t index = 0;
  for (index = 0; index < contents.GetLength(); index++) {
    ch = contents.GetAt(index);
    if (FindChar(ch, FALSE)) {
      continue;
    } else {
      return FALSE;
    }
  }
  return TRUE;
}
CFX_WideString CBC_OnedCodaBarWriter::FilterContents(
    const CFX_WideStringC& contents) {
  CFX_WideString filtercontents;
  FX_WCHAR ch;
  for (int32_t index = 0; index < contents.GetLength(); index++) {
    ch = contents.GetAt(index);
    if (ch > 175) {
      index++;
      continue;
    }
    if (FindChar(ch, TRUE)) {
      filtercontents += ch;
    } else {
      continue;
    }
  }
  return filtercontents;
}
uint8_t* CBC_OnedCodaBarWriter::Encode(const CFX_ByteString& contents,
                                       BCFORMAT format,
                                       int32_t& outWidth,
                                       int32_t& outHeight,
                                       int32_t& e) {
  uint8_t* ret = Encode(contents, format, outWidth, outHeight, 0, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}
uint8_t* CBC_OnedCodaBarWriter::Encode(const CFX_ByteString& contents,
                                       BCFORMAT format,
                                       int32_t& outWidth,
                                       int32_t& outHeight,
                                       int32_t hints,
                                       int32_t& e) {
  if (format != BCFORMAT_CODABAR) {
    e = BCExceptionOnlyEncodeCODEBAR;
    return nullptr;
  }
  uint8_t* ret =
      CBC_OneDimWriter::Encode(contents, format, outWidth, outHeight, hints, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, nullptr);
  return ret;
}
uint8_t* CBC_OnedCodaBarWriter::Encode(const CFX_ByteString& contents,
                                       int32_t& outLength,
                                       int32_t& e) {
  CFX_ByteString data = m_chStart + contents + m_chEnd;
  m_iContentLen = data.GetLength();
  uint8_t* result = FX_Alloc2D(uint8_t, m_iWideNarrRatio * 7, data.GetLength());
  FX_CHAR ch;
  int32_t position = 0;
  for (int32_t index = 0; index < data.GetLength(); index++) {
    ch = data.GetAt(index);
    if (((ch >= 'a') && (ch <= 'z'))) {
      ch = ch - 32;
    }
    switch (ch) {
      case 'T':
        ch = 'A';
        break;
      case 'N':
        ch = 'B';
        break;
      case '*':
        ch = 'C';
        break;
      case 'E':
        ch = 'D';
        break;
      default:
        break;
    }
    int32_t code = 0;
    int32_t len = (int32_t)strlen(ALPHABET_STRING);
    for (int32_t i = 0; i < len; i++) {
      if (ch == ALPHABET_STRING[i]) {
        code = CHARACTER_ENCODINGS[i];
        break;
      }
    }
    uint8_t color = 1;
    int32_t counter = 0;
    int32_t bit = 0;
    while (bit < 7) {
      result[position] = color;
      position++;
      if (((code >> (6 - bit)) & 1) == 0 || counter == m_iWideNarrRatio - 1) {
        color = !color;
        bit++;
        counter = 0;
      } else {
        counter++;
      }
    }
    if (index < data.GetLength() - 1) {
      result[position] = 0;
      position++;
    }
  }
  outLength = position;
  return result;
}
CFX_WideString CBC_OnedCodaBarWriter::encodedContents(
    const CFX_WideStringC& contents) {
  CFX_WideString strStart(m_chStart);
  CFX_WideString strEnd(m_chEnd);
  return strStart + contents + strEnd;
}
void CBC_OnedCodaBarWriter::RenderResult(const CFX_WideStringC& contents,
                                         uint8_t* code,
                                         int32_t codeLength,
                                         FX_BOOL isDevice,
                                         int32_t& e) {
  CBC_OneDimWriter::RenderResult(encodedContents(contents).AsStringC(), code,
                                 codeLength, isDevice, e);
}
