// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cagg_graphics.h"

CAGG_Graphics::CAGG_Graphics() : m_owner(nullptr) {}

FWL_Error CAGG_Graphics::Create(CFX_Graphics* owner,
                                int32_t width,
                                int32_t height,
                                FXDIB_Format format) {
  if (owner->m_renderDevice)
    return FWL_Error::ParameterInvalid;
  if (m_owner)
    return FWL_Error::PropertyInvalid;

  CFX_FxgeDevice* device = new CFX_FxgeDevice;
  device->Create(width, height, format, nullptr);
  m_owner = owner;
  m_owner->m_renderDevice = device;
  m_owner->m_renderDevice->GetBitmap()->Clear(0xFFFFFFFF);
  return FWL_Error::Succeeded;
}

CAGG_Graphics::~CAGG_Graphics() {
  delete m_owner->m_renderDevice;
}
