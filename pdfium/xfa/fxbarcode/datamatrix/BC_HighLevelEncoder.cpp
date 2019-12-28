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

#include <limits>
#include <memory>
#include <vector>

#include "xfa/fxbarcode/BC_Dimension.h"
#include "xfa/fxbarcode/BC_UtilCodingConvert.h"
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/datamatrix/BC_ASCIIEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Base256Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_C40Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EdifactEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EncoderContext.h"
#include "xfa/fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"
#include "xfa/fxbarcode/datamatrix/BC_TextEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_X12Encoder.h"
#include "xfa/fxbarcode/utils.h"

FX_WCHAR CBC_HighLevelEncoder::LATCH_TO_C40 = 230;
FX_WCHAR CBC_HighLevelEncoder::LATCH_TO_BASE256 = 231;
FX_WCHAR CBC_HighLevelEncoder::UPPER_SHIFT = 235;
FX_WCHAR CBC_HighLevelEncoder::LATCH_TO_ANSIX12 = 238;
FX_WCHAR CBC_HighLevelEncoder::LATCH_TO_TEXT = 239;
FX_WCHAR CBC_HighLevelEncoder::LATCH_TO_EDIFACT = 240;
FX_WCHAR CBC_HighLevelEncoder::C40_UNLATCH = 254;
FX_WCHAR CBC_HighLevelEncoder::X12_UNLATCH = 254;
FX_WCHAR CBC_HighLevelEncoder::PAD = 129;
FX_WCHAR CBC_HighLevelEncoder::MACRO_05 = 236;
FX_WCHAR CBC_HighLevelEncoder::MACRO_06 = 237;
const wchar_t* CBC_HighLevelEncoder::MACRO_05_HEADER = L"[)>05";
const wchar_t* CBC_HighLevelEncoder::MACRO_06_HEADER = L"[)>06";
const wchar_t CBC_HighLevelEncoder::MACRO_TRAILER = 0x0004;

CBC_HighLevelEncoder::CBC_HighLevelEncoder() {}
CBC_HighLevelEncoder::~CBC_HighLevelEncoder() {}

CFX_ByteArray& CBC_HighLevelEncoder::getBytesForMessage(CFX_WideString msg) {
  CFX_ByteString bytestr;
  CBC_UtilCodingConvert::UnicodeToUTF8(msg, bytestr);
  for (int32_t i = 0; i < bytestr.GetLength(); i++) {
    m_bytearray.Add(bytestr.GetAt(i));
  }
  return m_bytearray;
}
CFX_WideString CBC_HighLevelEncoder::encodeHighLevel(CFX_WideString msg,
                                                     CFX_WideString ecLevel,
                                                     int32_t& e) {
  return encodeHighLevel(msg, ecLevel, FORCE_NONE, nullptr, nullptr, e);
}
CFX_WideString CBC_HighLevelEncoder::encodeHighLevel(CFX_WideString msg,
                                                     CFX_WideString ecLevel,
                                                     SymbolShapeHint shape,
                                                     CBC_Dimension* minSize,
                                                     CBC_Dimension* maxSize,
                                                     int32_t& e) {
  CBC_EncoderContext context(msg, ecLevel, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, CFX_WideString());
  context.setSymbolShape(shape);
  context.setSizeConstraints(minSize, maxSize);
  if ((msg.Mid(0, 6) == MACRO_05_HEADER) &&
      (msg.Mid(msg.GetLength() - 1, 1) == MACRO_TRAILER)) {
    context.writeCodeword(MACRO_05);
    context.setSkipAtEnd(2);
    context.m_pos += 6;
  } else if ((msg.Mid(0, 6) == MACRO_06_HEADER) &&
             (msg.Mid(msg.GetLength() - 1, 1) == MACRO_TRAILER)) {
    context.writeCodeword(MACRO_06);
    context.setSkipAtEnd(2);
    context.m_pos += 6;
  }

  std::vector<std::unique_ptr<CBC_Encoder>> encoders;
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_ASCIIEncoder()));
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_C40Encoder()));
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_TextEncoder()));
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_X12Encoder()));
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_EdifactEncoder()));
  encoders.push_back(std::unique_ptr<CBC_Encoder>(new CBC_Base256Encoder()));
  int32_t encodingMode = ASCII_ENCODATION;
  while (context.hasMoreCharacters()) {
    encoders[encodingMode]->Encode(context, e);
    if (e != BCExceptionNO)
      return L"";

    if (context.m_newEncoding >= 0) {
      encodingMode = context.m_newEncoding;
      context.resetEncoderSignal();
    }
  }
  int32_t len = context.m_codewords.GetLength();
  context.updateSymbolInfo(e);
  if (e != BCExceptionNO)
    return L"";

  int32_t capacity = context.m_symbolInfo->m_dataCapacity;
  if (len < capacity) {
    if (encodingMode != ASCII_ENCODATION &&
        encodingMode != BASE256_ENCODATION) {
      context.writeCodeword(0x00fe);
    }
  }
  CFX_WideString codewords = context.m_codewords;
  if (codewords.GetLength() < capacity) {
    codewords += PAD;
  }
  while (codewords.GetLength() < capacity) {
    codewords += (randomize253State(PAD, codewords.GetLength() + 1));
  }
  return codewords;
}
int32_t CBC_HighLevelEncoder::lookAheadTest(CFX_WideString msg,
                                            int32_t startpos,
                                            int32_t currentMode) {
  if (startpos >= msg.GetLength()) {
    return currentMode;
  }
  CFX_FloatArray charCounts;
  if (currentMode == ASCII_ENCODATION) {
    charCounts.Add(0);
    charCounts.Add(1);
    charCounts.Add(1);
    charCounts.Add(1);
    charCounts.Add(1);
    charCounts.Add(1.25f);
  } else {
    charCounts.Add(1);
    charCounts.Add(2);
    charCounts.Add(2);
    charCounts.Add(2);
    charCounts.Add(2);
    charCounts.Add(2.25f);
    charCounts[currentMode] = 0;
  }
  int32_t charsProcessed = 0;
  while (TRUE) {
    if ((startpos + charsProcessed) == msg.GetLength()) {
      int32_t min = std::numeric_limits<int32_t>::max();
      CFX_ByteArray mins;
      mins.SetSize(6);
      CFX_Int32Array intCharCounts;
      intCharCounts.SetSize(6);
      min = findMinimums(charCounts, intCharCounts, min, mins);
      int32_t minCount = getMinimumCount(mins);
      if (intCharCounts[ASCII_ENCODATION] == min) {
        return ASCII_ENCODATION;
      }
      if (minCount == 1 && mins[BASE256_ENCODATION] > 0) {
        return BASE256_ENCODATION;
      }
      if (minCount == 1 && mins[EDIFACT_ENCODATION] > 0) {
        return EDIFACT_ENCODATION;
      }
      if (minCount == 1 && mins[TEXT_ENCODATION] > 0) {
        return TEXT_ENCODATION;
      }
      if (minCount == 1 && mins[X12_ENCODATION] > 0) {
        return X12_ENCODATION;
      }
      return C40_ENCODATION;
    }
    FX_WCHAR c = msg.GetAt(startpos + charsProcessed);
    charsProcessed++;
    if (isDigit(c)) {
      charCounts[ASCII_ENCODATION] += 0.5;
    } else if (isExtendedASCII(c)) {
      charCounts[ASCII_ENCODATION] =
          (FX_FLOAT)ceil(charCounts[ASCII_ENCODATION]);
      charCounts[ASCII_ENCODATION] += 2;
    } else {
      charCounts[ASCII_ENCODATION] =
          (FX_FLOAT)ceil(charCounts[ASCII_ENCODATION]);
      charCounts[ASCII_ENCODATION]++;
    }
    if (isNativeC40(c)) {
      charCounts[C40_ENCODATION] += 2.0f / 3.0f;
    } else if (isExtendedASCII(c)) {
      charCounts[C40_ENCODATION] += 8.0f / 3.0f;
    } else {
      charCounts[C40_ENCODATION] += 4.0f / 3.0f;
    }
    if (isNativeText(c)) {
      charCounts[TEXT_ENCODATION] += 2.0f / 3.0f;
    } else if (isExtendedASCII(c)) {
      charCounts[TEXT_ENCODATION] += 8.0f / 3.0f;
    } else {
      charCounts[TEXT_ENCODATION] += 4.0f / 3.0f;
    }
    if (isNativeX12(c)) {
      charCounts[X12_ENCODATION] += 2.0f / 3.0f;
    } else if (isExtendedASCII(c)) {
      charCounts[X12_ENCODATION] += 13.0f / 3.0f;
    } else {
      charCounts[X12_ENCODATION] += 10.0f / 3.0f;
    }
    if (isNativeEDIFACT(c)) {
      charCounts[EDIFACT_ENCODATION] += 3.0f / 4.0f;
    } else if (isExtendedASCII(c)) {
      charCounts[EDIFACT_ENCODATION] += 17.0f / 4.0f;
    } else {
      charCounts[EDIFACT_ENCODATION] += 13.0f / 4.0f;
    }
    if (isSpecialB256(c)) {
      charCounts[BASE256_ENCODATION] += 4;
    } else {
      charCounts[BASE256_ENCODATION]++;
    }
    if (charsProcessed >= 4) {
      CFX_Int32Array intCharCounts;
      intCharCounts.SetSize(6);
      CFX_ByteArray mins;
      mins.SetSize(6);
      findMinimums(charCounts, intCharCounts,
                   std::numeric_limits<int32_t>::max(), mins);
      int32_t minCount = getMinimumCount(mins);
      if (intCharCounts[ASCII_ENCODATION] < intCharCounts[BASE256_ENCODATION] &&
          intCharCounts[ASCII_ENCODATION] < intCharCounts[C40_ENCODATION] &&
          intCharCounts[ASCII_ENCODATION] < intCharCounts[TEXT_ENCODATION] &&
          intCharCounts[ASCII_ENCODATION] < intCharCounts[X12_ENCODATION] &&
          intCharCounts[ASCII_ENCODATION] < intCharCounts[EDIFACT_ENCODATION]) {
        return ASCII_ENCODATION;
      }
      if (intCharCounts[BASE256_ENCODATION] < intCharCounts[ASCII_ENCODATION] ||
          (mins[C40_ENCODATION] + mins[TEXT_ENCODATION] + mins[X12_ENCODATION] +
           mins[EDIFACT_ENCODATION]) == 0) {
        return BASE256_ENCODATION;
      }
      if (minCount == 1 && mins[EDIFACT_ENCODATION] > 0) {
        return EDIFACT_ENCODATION;
      }
      if (minCount == 1 && mins[TEXT_ENCODATION] > 0) {
        return TEXT_ENCODATION;
      }
      if (minCount == 1 && mins[X12_ENCODATION] > 0) {
        return X12_ENCODATION;
      }
      if (intCharCounts[C40_ENCODATION] + 1 < intCharCounts[ASCII_ENCODATION] &&
          intCharCounts[C40_ENCODATION] + 1 <
              intCharCounts[BASE256_ENCODATION] &&
          intCharCounts[C40_ENCODATION] + 1 <
              intCharCounts[EDIFACT_ENCODATION] &&
          intCharCounts[C40_ENCODATION] + 1 < intCharCounts[TEXT_ENCODATION]) {
        if (intCharCounts[C40_ENCODATION] < intCharCounts[X12_ENCODATION]) {
          return C40_ENCODATION;
        }
        if (intCharCounts[C40_ENCODATION] == intCharCounts[X12_ENCODATION]) {
          int32_t p = startpos + charsProcessed + 1;
          while (p < msg.GetLength()) {
            FX_WCHAR tc = msg.GetAt(p);
            if (isX12TermSep(tc)) {
              return X12_ENCODATION;
            }
            if (!isNativeX12(tc)) {
              break;
            }
            p++;
          }
          return C40_ENCODATION;
        }
      }
    }
  }
}
FX_BOOL CBC_HighLevelEncoder::isDigit(FX_WCHAR ch) {
  return ch >= '0' && ch <= '9';
}
FX_BOOL CBC_HighLevelEncoder::isExtendedASCII(FX_WCHAR ch) {
  return ch >= 128 && ch <= 255;
}
int32_t CBC_HighLevelEncoder::determineConsecutiveDigitCount(CFX_WideString msg,
                                                             int32_t startpos) {
  int32_t count = 0;
  int32_t len = msg.GetLength();
  int32_t idx = startpos;
  if (idx < len) {
    FX_WCHAR ch = msg.GetAt(idx);
    while (isDigit(ch) && idx < len) {
      count++;
      idx++;
      if (idx < len) {
        ch = msg.GetAt(idx);
      }
    }
  }
  return count;
}
void CBC_HighLevelEncoder::illegalCharacter(FX_WCHAR c, int32_t& e) {
  e = BCExceptionIllegalArgument;
}
FX_WCHAR CBC_HighLevelEncoder::randomize253State(FX_WCHAR ch,
                                                 int32_t codewordPosition) {
  int32_t pseudoRandom = ((149 * codewordPosition) % 253) + 1;
  int32_t tempVariable = ch + pseudoRandom;
  return tempVariable <= 254 ? (FX_WCHAR)tempVariable
                             : (FX_WCHAR)(tempVariable - 254);
}
int32_t CBC_HighLevelEncoder::findMinimums(CFX_FloatArray& charCounts,
                                           CFX_Int32Array& intCharCounts,
                                           int32_t min,
                                           CFX_ByteArray& mins) {
  for (int32_t l = 0; l < mins.GetSize(); l++) {
    mins[l] = (uint8_t)0;
  }
  for (int32_t i = 0; i < 6; i++) {
    intCharCounts[i] = (int32_t)ceil(charCounts[i]);
    int32_t current = intCharCounts[i];
    if (min > current) {
      min = current;
      for (int32_t j = 0; j < mins.GetSize(); j++) {
        mins[j] = (uint8_t)0;
      }
    }
    if (min == current) {
      mins[i]++;
    }
  }
  return min;
}
int32_t CBC_HighLevelEncoder::getMinimumCount(CFX_ByteArray& mins) {
  int32_t minCount = 0;
  for (int32_t i = 0; i < 6; i++) {
    minCount += mins[i];
  }
  return minCount;
}
FX_BOOL CBC_HighLevelEncoder::isNativeC40(FX_WCHAR ch) {
  return (ch == ' ') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z');
}
FX_BOOL CBC_HighLevelEncoder::isNativeText(FX_WCHAR ch) {
  return (ch == ' ') || (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z');
}
FX_BOOL CBC_HighLevelEncoder::isNativeX12(FX_WCHAR ch) {
  return isX12TermSep(ch) || (ch == ' ') || (ch >= '0' && ch <= '9') ||
         (ch >= 'A' && ch <= 'Z');
}
FX_BOOL CBC_HighLevelEncoder::isX12TermSep(FX_WCHAR ch) {
  return (ch == '\r') || (ch == '*') || (ch == '>');
}
FX_BOOL CBC_HighLevelEncoder::isNativeEDIFACT(FX_WCHAR ch) {
  return ch >= ' ' && ch <= '^';
}
FX_BOOL CBC_HighLevelEncoder::isSpecialB256(FX_WCHAR ch) {
  return FALSE;
}
