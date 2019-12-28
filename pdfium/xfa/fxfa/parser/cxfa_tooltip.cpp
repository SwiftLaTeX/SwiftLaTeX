// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_tooltip.h"

#include "xfa/fxfa/parser/xfa_object.h"

CXFA_ToolTip::CXFA_ToolTip(CXFA_Node* pNode) : CXFA_Data(pNode) {}

FX_BOOL CXFA_ToolTip::GetTip(CFX_WideString& wsTip) {
  return m_pNode->TryContent(wsTip);
}
