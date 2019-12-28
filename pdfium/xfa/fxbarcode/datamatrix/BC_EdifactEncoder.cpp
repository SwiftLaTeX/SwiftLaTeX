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
#include "xfa/fxbarcode/datamatrix/BC_EdifactEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_Encoder.h"
#include "xfa/fxbarcode/datamatrix/BC_EncoderContext.h"
#include "xfa/fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "xfa/fxbarcode/datamatrix/BC_SymbolShapeHint.h"

CBC_EdifactEncoder::CBC_EdifactEncoder() {}
CBC_EdifactEncoder::~CBC_EdifactEncoder() {}
int32_t CBC_EdifactEncoder::getEncodingMode() {
  return EDIFACT_ENCODATION;
}
void CBC_EdifactEncoder::Encode(CBC_EncoderContext& context, int32_t& e) {
  CFX_WideString buffer;
  while (context.hasMoreCharacters()) {
    FX_WCHAR c = context.getCurrentChar();
    encodeChar(c, buffer, e);
    if (e != BCExceptionNO) {
      return;
    }
    context.m_pos++;
    int32_t count = buffer.GetLength();
    if (count >= 4) {
      context.writeCodewords(encodeToCodewords(buffer, 0, e));
      if (e != BCExceptionNO) {
        return;
      }
      buffer.Delete(0, 4);
      int32_t newMode = CBC_HighLevelEncoder::lookAheadTest(
          context.m_msg, context.m_pos, getEncodingMode());
      if (newMode != getEncodingMode()) {
        context.signalEncoderChange(ASCII_ENCODATION);
        break;
      }
    }
  }
  buffer += (FX_WCHAR)31;
  handleEOD(context, buffer, e);
}
void CBC_EdifactEncoder::handleEOD(CBC_EncoderContext& context,
                                   CFX_WideString buffer,
                                   int32_t& e) {
  int32_t count = buffer.GetLength();
  if (count == 0) {
    return;
  }
  if (count == 1) {
    context.updateSymbolInfo(e);
    if (e != BCExceptionNO) {
      return;
    }
    int32_t available =
        context.m_symbolInfo->m_dataCapacity - context.getCodewordCount();
    int32_t remaining = context.getRemainingCharacters();
    if (remaining == 0 && available <= 2) {
      return;
    }
  }
  if (count > 4) {
    e = BCExceptionIllegalStateCountMustNotExceed4;
    return;
  }
  int32_t restChars = count - 1;
  CFX_WideString encoded = encodeToCodewords(buffer, 0, e);
  if (e != BCExceptionNO) {
    return;
  }
  FX_BOOL endOfSymbolReached = !context.hasMoreCharacters();
  FX_BOOL restInAscii = endOfSymbolReached && restChars <= 2;
  if (restChars <= 2) {
    context.updateSymbolInfo(context.getCodewordCount() + restChars, e);
    if (e != BCExceptionNO) {
      return;
    }
    int32_t available =
        context.m_symbolInfo->m_dataCapacity - context.getCodewordCount();
    if (available >= 3) {
      restInAscii = FALSE;
      context.updateSymbolInfo(context.getCodewordCount() + encoded.GetLength(),
                               e);
      if (e != BCExceptionNO) {
        return;
      }
    }
  }
  if (restInAscii) {
    context.resetSymbolInfo();
    context.m_pos -= restChars;
  } else {
    context.writeCodewords(encoded);
  }
  context.signalEncoderChange(ASCII_ENCODATION);
}
void CBC_EdifactEncoder::encodeChar(FX_WCHAR c,
                                    CFX_WideString& sb,
                                    int32_t& e) {
  if (c >= ' ' && c <= '?') {
    sb += c;
  } else if (c >= '@' && c <= '^') {
    sb += (FX_WCHAR)(c - 64);
  } else {
    CBC_HighLevelEncoder::illegalCharacter(c, e);
  }
}
CFX_WideString CBC_EdifactEncoder::encodeToCodewords(CFX_WideString sb,
                                                     int32_t startPos,
                                                     int32_t& e) {
  int32_t len = sb.GetLength() - startPos;
  if (len == 0) {
    e = BCExceptionNoContents;
    return CFX_WideString();
  }
  FX_WCHAR c1 = sb.GetAt(startPos);
  FX_WCHAR c2 = len >= 2 ? sb.GetAt(startPos + 1) : 0;
  FX_WCHAR c3 = len >= 3 ? sb.GetAt(startPos + 2) : 0;
  FX_WCHAR c4 = len >= 4 ? sb.GetAt(startPos + 3) : 0;
  int32_t v = (c1 << 18) + (c2 << 12) + (c3 << 6) + c4;
  FX_WCHAR cw1 = (FX_WCHAR)((v >> 16) & 255);
  FX_WCHAR cw2 = (FX_WCHAR)((v >> 8) & 255);
  FX_WCHAR cw3 = (FX_WCHAR)(v & 255);
  CFX_WideString res;
  res += cw1;
  if (len >= 2) {
    res += cw2;
  }
  if (len >= 3) {
    res += cw3;
  }
  return res;
}
