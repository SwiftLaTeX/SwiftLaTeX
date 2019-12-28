// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_CFWL_THEMEBACKGROUND_H_
#define XFA_FWL_CORE_CFWL_THEMEBACKGROUND_H_

#include "core/fxge/include/fx_dib.h"
#include "xfa/fwl/core/cfwl_themepart.h"

class CFX_DIBitmpa;
class CFX_Graphics;
class CFX_Path;

class CFWL_ThemeBackground : public CFWL_ThemePart {
 public:
  CFWL_ThemeBackground()
      : m_pGraphics(nullptr), m_pImage(nullptr), m_pPath(nullptr) {}

  CFX_Graphics* m_pGraphics;
  CFX_DIBitmap* m_pImage;
  CFX_Path* m_pPath;
};

#endif  // XFA_FWL_CORE_CFWL_THEMEBACKGROUND_H_
