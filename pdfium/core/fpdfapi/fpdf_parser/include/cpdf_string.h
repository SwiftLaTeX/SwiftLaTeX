// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STRING_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STRING_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_String : public CPDF_Object {
 public:
  CPDF_String();
  CPDF_String(const CFX_ByteString& str, FX_BOOL bHex);
  explicit CPDF_String(const CFX_WideString& str);

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  CFX_ByteString GetString() const override;
  CFX_WideString GetUnicodeText() const override;
  void SetString(const CFX_ByteString& str) override;
  bool IsString() const override;
  CPDF_String* AsString() override;
  const CPDF_String* AsString() const override;

  FX_BOOL IsHex() const { return m_bHex; }

 protected:
  ~CPDF_String() override;

  CFX_ByteString m_String;
  FX_BOOL m_bHex;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STRING_H_
