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
#include "xfa/fxbarcode/common/BC_CommonBitMatrix.h"
#include "xfa/fxbarcode/datamatrix/BC_C40Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EncoderContext.h"
#include "xfa/fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"

CBC_C40Encoder::CBC_C40Encoder() {}
CBC_C40Encoder::~CBC_C40Encoder() {}
int32_t CBC_C40Encoder::getEncodingMode() {
  return C40_ENCODATION;
}
void CBC_C40Encoder::Encode(CBC_EncoderContext& context, int32_t& e) {
  CFX_WideString buffer;
  while (context.hasMoreCharacters()) {
    FX_WCHAR c = context.getCurrentChar();
    context.m_pos++;
    int32_t lastCharSize = encodeChar(c, buffer, e);
    if (e != BCExceptionNO) {
      return;
    }
    int32_t unwritten = (buffer.GetLength() / 3) * 2;
    int32_t curCodewordCount = context.getCodewordCount() + unwritten;
    context.updateSymbolInfo(curCodewordCount, e);
    if (e != BCExceptionNO) {
      return;
    }
    int32_t available = context.m_symbolInfo->m_dataCapacity - curCodewordCount;
    if (!context.hasMoreCharacters()) {
      CFX_WideString removed;
      if ((buffer.GetLength() % 3) == 2) {
        if (available < 2 || available > 2) {
          lastCharSize =
              backtrackOneCharacter(context, buffer, removed, lastCharSize, e);
          if (e != BCExceptionNO) {
            return;
          }
        }
      }
      while ((buffer.GetLength() % 3) == 1 &&
             ((lastCharSize <= 3 && available != 1) || lastCharSize > 3)) {
        lastCharSize =
            backtrackOneCharacter(context, buffer, removed, lastCharSize, e);
        if (e != BCExceptionNO) {
          return;
        }
      }
      break;
    }
    int32_t count = buffer.GetLength();
    if ((count % 3) == 0) {
      int32_t newMode = CBC_HighLevelEncoder::lookAheadTest(
          context.m_msg, context.m_pos, getEncodingMode());
      if (newMode != getEncodingMode()) {
        context.signalEncoderChange(newMode);
        break;
      }
    }
  }
  handleEOD(context, buffer, e);
}
void CBC_C40Encoder::writeNextTriplet(CBC_EncoderContext& context,
                                      CFX_WideString& buffer) {
  context.writeCodewords(encodeToCodewords(buffer, 0));
  buffer.Delete(0, 3);
}
void CBC_C40Encoder::handleEOD(CBC_EncoderContext& context,
                               CFX_WideString& buffer,
                               int32_t& e) {
  int32_t unwritten = (buffer.GetLength() / 3) * 2;
  int32_t rest = buffer.GetLength() % 3;
  int32_t curCodewordCount = context.getCodewordCount() + unwritten;
  context.updateSymbolInfo(curCodewordCount, e);
  if (e != BCExceptionNO) {
    return;
  }
  int32_t available = context.m_symbolInfo->m_dataCapacity - curCodewordCount;
  if (rest == 2) {
    buffer += (FX_WCHAR)'\0';
    while (buffer.GetLength() >= 3) {
      writeNextTriplet(context, buffer);
    }
    if (context.hasMoreCharacters()) {
      context.writeCodeword(CBC_HighLevelEncoder::C40_UNLATCH);
    }
  } else if (available == 1 && rest == 1) {
    while (buffer.GetLength() >= 3) {
      writeNextTriplet(context, buffer);
    }
    if (context.hasMoreCharacters()) {
      context.writeCodeword(CBC_HighLevelEncoder::C40_UNLATCH);
    }
    context.m_pos--;
  } else if (rest == 0) {
    while (buffer.GetLength() >= 3) {
      writeNextTriplet(context, buffer);
    }
    if (available > 0 || context.hasMoreCharacters()) {
      context.writeCodeword(CBC_HighLevelEncoder::C40_UNLATCH);
    }
  } else {
    e = BCExceptionIllegalStateUnexpectedCase;
    return;
  }
  context.signalEncoderChange(ASCII_ENCODATION);
}
int32_t CBC_C40Encoder::encodeChar(FX_WCHAR c, CFX_WideString& sb, int32_t& e) {
  if (c == ' ') {
    sb += (FX_WCHAR)'\3';
    return 1;
  } else if ((c >= '0') && (c <= '9')) {
    sb += (FX_WCHAR)(c - 48 + 4);
    return 1;
  } else if ((c >= 'A') && (c <= 'Z')) {
    sb += (FX_WCHAR)(c - 65 + 14);
    return 1;
  } else if (c <= 0x1f) {
    sb += (FX_WCHAR)'\0';
    sb += c;
    return 2;
  } else if ((c >= '!') && (c <= '/')) {
    sb += (FX_WCHAR)'\1';
    sb += (FX_WCHAR)(c - 33);
    return 2;
  } else if ((c >= ':') && (c <= '@')) {
    sb += (FX_WCHAR)'\1';
    sb += (FX_WCHAR)(c - 58 + 15);
    return 2;
  } else if ((c >= '[') && (c <= '_')) {
    sb += (FX_WCHAR)'\1';
    sb += (FX_WCHAR)(c - 91 + 22);
    return 2;
  } else if ((c >= 60) && (c <= 0x7f)) {
    sb += (FX_WCHAR)'\2';
    sb += (FX_WCHAR)(c - 96);
    return 2;
  } else if (c >= 80) {
    sb += (FX_WCHAR)'\1';
    sb += (FX_WCHAR)0x001e;
    int32_t len = 2;
    len += encodeChar((c - 128), sb, e);
    BC_EXCEPTION_CHECK_ReturnValue(e, 0);
    return len;
  } else {
    e = BCExceptionIllegalArgument;
    return 0;
  }
}
int32_t CBC_C40Encoder::backtrackOneCharacter(CBC_EncoderContext& context,
                                              CFX_WideString& buffer,
                                              CFX_WideString& removed,
                                              int32_t lastCharSize,
                                              int32_t& e) {
  int32_t count = buffer.GetLength();
  buffer.Delete(count - lastCharSize, count);
  context.m_pos--;
  FX_WCHAR c = context.getCurrentChar();
  lastCharSize = encodeChar(c, removed, e);
  BC_EXCEPTION_CHECK_ReturnValue(e, -1);
  context.resetSymbolInfo();
  return lastCharSize;
}
CFX_WideString CBC_C40Encoder::encodeToCodewords(CFX_WideString sb,
                                                 int32_t startPos) {
  FX_WCHAR c1 = sb.GetAt(startPos);
  FX_WCHAR c2 = sb.GetAt(startPos + 1);
  FX_WCHAR c3 = sb.GetAt(startPos + 2);
  int32_t v = (1600 * c1) + (40 * c2) + c3 + 1;
  FX_WCHAR cw1 = (FX_WCHAR)(v / 256);
  FX_WCHAR cw2 = (FX_WCHAR)(v % 256);
  CFX_WideString b1(cw1);
  CFX_WideString b2(cw2);
  return b1 + b2;
}
