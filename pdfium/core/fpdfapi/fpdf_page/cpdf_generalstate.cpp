// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_generalstate.h"

namespace {

int RI_StringToId(const CFX_ByteString& ri) {
  uint32_t id = ri.GetID();
  if (id == FXBSTR_ID('A', 'b', 's', 'o'))
    return 1;

  if (id == FXBSTR_ID('S', 'a', 't', 'u'))
    return 2;

  if (id == FXBSTR_ID('P', 'e', 'r', 'c'))
    return 3;

  return 0;
}

}  // namespace

void CPDF_GeneralState::SetRenderIntent(const CFX_ByteString& ri) {
  GetModify()->m_RenderIntent = RI_StringToId(ri);
}
