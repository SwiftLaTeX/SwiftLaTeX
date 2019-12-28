// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_THEMETEXT_H_
#define XFA_FWL_CORE_CFWL_THEMETEXT_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/cfwl_themepart.h"

class CFWL_ThemeText : public CFWL_ThemePart {
 public:
  CFWL_ThemeText() : m_pGraphics(nullptr) {}
  CFX_WideString m_wsText;
  uint32_t m_dwTTOStyles;
  int32_t m_iTTOAlign;
  CFX_Graphics* m_pGraphics;
};

#endif  // XFA_FWL_CORE_CFWL_THEMETEXT_H_
