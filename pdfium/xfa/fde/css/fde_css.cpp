// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/css/fde_css.h"

FDE_CSSVALUETYPE IFDE_CSSPrimitiveValue::GetType() const {
  return FDE_CSSVALUETYPE_Primitive;
}

FDE_CSSVALUETYPE IFDE_CSSValueList::GetType() const {
  return FDE_CSSVALUETYPE_List;
}

FDE_CSSRULETYPE IFDE_CSSStyleRule::GetType() const {
  return FDE_CSSRULETYPE_Style;
}

FDE_CSSRULETYPE IFDE_CSSMediaRule::GetType() const {
  return FDE_CSSRULETYPE_Media;
}

FDE_CSSRULETYPE IFDE_CSSFontFaceRule::GetType() const {
  return FDE_CSSRULETYPE_FontFace;
}
