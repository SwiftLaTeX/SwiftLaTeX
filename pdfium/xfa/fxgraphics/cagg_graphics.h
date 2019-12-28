// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_CAGG_GRAPHICS_H_
#define XFA_FXGRAPHICS_CAGG_GRAPHICS_H_

#include "core/fxge/include/fx_dib.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"

class CFX_Graphics;

class CAGG_Graphics {
 public:
  CAGG_Graphics();
  virtual ~CAGG_Graphics();

  FWL_Error Create(CFX_Graphics* owner,
                   int32_t width,
                   int32_t height,
                   FXDIB_Format format);

 private:
  CFX_Graphics* m_owner;
};

#endif  // XFA_FXGRAPHICS_CAGG_GRAPHICS_H_
