// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXBARCODE_BC_DIMENSION_H_
#define XFA_FXBARCODE_BC_DIMENSION_H_

#include <cstdint>

#include "core/fxcrt/include/fx_string.h"

class CBC_Dimension {
 public:
  CBC_Dimension();
  CBC_Dimension(int32_t width, int32_t height, int32_t& e);
  virtual ~CBC_Dimension();
  int32_t getWidth();
  int32_t getHeight();
  int32_t hashCode();
  CFX_WideString toString();

 private:
  int32_t m_width;
  int32_t m_height;
};

#endif  // XFA_FXBARCODE_BC_DIMENSION_H_
