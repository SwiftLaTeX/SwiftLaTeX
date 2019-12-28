// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cfx_color.h"

CFX_Color::CFX_Color() : m_type(FX_COLOR_None) {}

CFX_Color::CFX_Color(const FX_ARGB argb) {
  Set(argb);
}

CFX_Color::CFX_Color(CFX_Pattern* pattern, const FX_ARGB argb) {
  Set(pattern, argb);
}

CFX_Color::CFX_Color(CFX_Shading* shading) {
  Set(shading);
}

CFX_Color::~CFX_Color() {
  m_type = FX_COLOR_None;
}

FWL_Error CFX_Color::Set(const FX_ARGB argb) {
  m_type = FX_COLOR_Solid;
  m_info.argb = argb;
  m_info.pattern = nullptr;
  return FWL_Error::Succeeded;
}

FWL_Error CFX_Color::Set(CFX_Pattern* pattern, const FX_ARGB argb) {
  if (!pattern)
    return FWL_Error::ParameterInvalid;
  m_type = FX_COLOR_Pattern;
  m_info.argb = argb;
  m_info.pattern = pattern;
  return FWL_Error::Succeeded;
}

FWL_Error CFX_Color::Set(CFX_Shading* shading) {
  if (!shading)
    return FWL_Error::ParameterInvalid;
  m_type = FX_COLOR_Shading;
  m_shading = shading;
  return FWL_Error::Succeeded;
}
