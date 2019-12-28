// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_FPDF_PARSER_UTILITY_H_
#define CORE_FPDFAPI_FPDF_PARSER_FPDF_PARSER_UTILITY_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class IFX_FileRead;
class CPDF_Dictionary;

// Use the accessors below instead of directly accessing PDF_CharType.
extern const char PDF_CharType[256];

inline bool PDFCharIsWhitespace(uint8_t c) {
  return PDF_CharType[c] == 'W';
}
inline bool PDFCharIsNumeric(uint8_t c) {
  return PDF_CharType[c] == 'N';
}
inline bool PDFCharIsDelimiter(uint8_t c) {
  return PDF_CharType[c] == 'D';
}
inline bool PDFCharIsOther(uint8_t c) {
  return PDF_CharType[c] == 'R';
}

inline bool PDFCharIsLineEnding(uint8_t c) {
  return c == '\r' || c == '\n';
}

int32_t GetHeaderOffset(IFX_FileRead* pFile);
int32_t GetDirectInteger(CPDF_Dictionary* pDict, const CFX_ByteString& key);

#endif  // CORE_FPDFAPI_FPDF_PARSER_FPDF_PARSER_UTILITY_H_
