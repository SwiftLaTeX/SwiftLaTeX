// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CFX_COLOR_H_
#define XFA_FXGRAPHICS_CFX_COLOR_H_

#include "core/fxge/include/fx_dib.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

class CFX_Pattern;
class CFX_Shading;

enum { FX_COLOR_None = 0, FX_COLOR_Solid, FX_COLOR_Pattern, FX_COLOR_Shading };

class CFX_Color {
 public:
  CFX_Color();
  // TODO(weili): Remove implicit conversions. Make this explicit.
  CFX_Color(const FX_ARGB argb);

  explicit CFX_Color(CFX_Pattern* pattern, const FX_ARGB argb = 0x0);
  explicit CFX_Color(CFX_Shading* shading);
  virtual ~CFX_Color();

  FWL_Error Set(const FX_ARGB argb);
  FWL_Error Set(CFX_Pattern* pattern, const FX_ARGB argb = 0x0);
  FWL_Error Set(CFX_Shading* shading);

 private:
  friend class CFX_Graphics;

  int32_t m_type;
  union {
    struct {
      FX_ARGB argb;
      CFX_Pattern* pattern;
    } m_info;
    CFX_Shading* m_shading;
  };
};

#endif  // XFA_FXGRAPHICS_CFX_COLOR_H_
