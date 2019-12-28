// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"

#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"

CPDF_String::CPDF_String() : m_bHex(FALSE) {}

CPDF_String::CPDF_String(const CFX_ByteString& str, FX_BOOL bHex)
    : m_String(str), m_bHex(bHex) {}

CPDF_String::CPDF_String(const CFX_WideString& str) : m_bHex(FALSE) {
  m_String = PDF_EncodeText(str);
}

CPDF_String::~CPDF_String() {}

CPDF_Object::Type CPDF_String::GetType() const {
  return STRING;
}

CPDF_Object* CPDF_String::Clone(FX_BOOL bDirect) const {
  return new CPDF_String(m_String, m_bHex);
}

CFX_ByteString CPDF_String::GetString() const {
  return m_String;
}

void CPDF_String::SetString(const CFX_ByteString& str) {
  m_String = str;
}

bool CPDF_String::IsString() const {
  return true;
}

CPDF_String* CPDF_String::AsString() {
  return this;
}

const CPDF_String* CPDF_String::AsString() const {
  return this;
}

CFX_WideString CPDF_String::GetUnicodeText() const {
  return PDF_DecodeText(m_String);
}
