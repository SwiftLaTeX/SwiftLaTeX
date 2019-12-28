// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/cpdf_null.h"

CPDF_Null::CPDF_Null() {}

CPDF_Object::Type CPDF_Null::GetType() const {
  return NULLOBJ;
}

CPDF_Object* CPDF_Null::Clone(FX_BOOL bDirect) const {
  return new CPDF_Null;
}
