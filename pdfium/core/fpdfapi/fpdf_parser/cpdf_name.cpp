// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"

#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"

CPDF_Name::CPDF_Name(const CFX_ByteString& str) : m_Name(str) {}

CPDF_Name::~CPDF_Name() {}

CPDF_Object::Type CPDF_Name::GetType() const {
  return NAME;
}

CPDF_Object* CPDF_Name::Clone(FX_BOOL bDirect) const {
  return new CPDF_Name(m_Name);
}

CFX_ByteString CPDF_Name::GetString() const {
  return m_Name;
}

void CPDF_Name::SetString(const CFX_ByteString& str) {
  m_Name = str;
}

bool CPDF_Name::IsName() const {
  return true;
}

CPDF_Name* CPDF_Name::AsName() {
  return this;
}

const CPDF_Name* CPDF_Name::AsName() const {
  return this;
}

CFX_WideString CPDF_Name::GetUnicodeText() const {
  return PDF_DecodeText(m_Name);
}
