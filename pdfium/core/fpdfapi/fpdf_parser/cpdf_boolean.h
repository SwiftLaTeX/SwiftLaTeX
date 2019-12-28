// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_CPDF_BOOLEAN_H_
#define CORE_FPDFAPI_FPDF_PARSER_CPDF_BOOLEAN_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Boolean : public CPDF_Object {
 public:
  CPDF_Boolean();
  explicit CPDF_Boolean(bool value);

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  CFX_ByteString GetString() const override;
  int GetInteger() const override;
  void SetString(const CFX_ByteString& str) override;
  bool IsBoolean() const override;
  CPDF_Boolean* AsBoolean() override;
  const CPDF_Boolean* AsBoolean() const override;

 protected:
  ~CPDF_Boolean() override;

  bool m_bValue;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_CPDF_BOOLEAN_H_
