// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_FDE_OBJECT_H_
#define XFA_FDE_FDE_OBJECT_H_

#include <cstdint>

#include "core/fxge/include/fx_dib.h"
#include "xfa/fgas/crt/fgas_memory.h"

class CFDE_Brush : public CFX_Target {
 public:
  CFDE_Brush() : m_Color(0xFF000000) {}

  FX_ARGB GetColor() const { return m_Color; }
  void SetColor(FX_ARGB color) { m_Color = color; }

 private:
  FX_ARGB m_Color;
};

class CFDE_Pen : public CFX_Target {
 public:
  CFDE_Pen() : m_Color(0) {}
  ~CFDE_Pen() override {}

  FX_ARGB GetColor() const { return m_Color; }
  void SetColor(FX_ARGB color) { m_Color = color; }

 private:
  FX_ARGB m_Color;
};

#endif  // XFA_FDE_FDE_OBJECT_H_
