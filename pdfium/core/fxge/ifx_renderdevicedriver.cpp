// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/include/ifx_renderdevicedriver.h"

#include "core/fxcrt/include/fx_coordinates.h"

IFX_RenderDeviceDriver::~IFX_RenderDeviceDriver() {}

CFX_Matrix IFX_RenderDeviceDriver::GetCTM() const {
  return CFX_Matrix();
}

FX_BOOL IFX_RenderDeviceDriver::StartRendering() {
  return TRUE;
}

void IFX_RenderDeviceDriver::EndRendering() {}

FX_BOOL IFX_RenderDeviceDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  return FALSE;
}

FX_BOOL IFX_RenderDeviceDriver::SetPixel(int x, int y, uint32_t color) {
  return FALSE;
}

FX_BOOL IFX_RenderDeviceDriver::FillRectWithBlend(const FX_RECT* pRect,
                                                  uint32_t fill_color,
                                                  int blend_type) {
  return FALSE;
}

FX_BOOL IFX_RenderDeviceDriver::DrawCosmeticLine(FX_FLOAT x1,
                                                 FX_FLOAT y1,
                                                 FX_FLOAT x2,
                                                 FX_FLOAT y2,
                                                 uint32_t color,
                                                 int blend_type) {
  return FALSE;
}

FX_BOOL IFX_RenderDeviceDriver::GetDIBits(CFX_DIBitmap* pBitmap,
                                          int left,
                                          int top) {
  return FALSE;
}

CFX_DIBitmap* IFX_RenderDeviceDriver::GetBackDrop() {
  return nullptr;
}

FX_BOOL IFX_RenderDeviceDriver::ContinueDIBits(void* handle,
                                               IFX_Pause* pPause) {
  return FALSE;
}

void IFX_RenderDeviceDriver::CancelDIBits(void* handle) {}

FX_BOOL IFX_RenderDeviceDriver::DrawDeviceText(int nChars,
                                               const FXTEXT_CHARPOS* pCharPos,
                                               CFX_Font* pFont,
                                               CFX_FontCache* pCache,
                                               const CFX_Matrix* pObject2Device,
                                               FX_FLOAT font_size,
                                               uint32_t color) {
  return FALSE;
}

void* IFX_RenderDeviceDriver::GetPlatformSurface() const {
  return nullptr;
}

int IFX_RenderDeviceDriver::GetDriverType() const {
  return 0;
}

void IFX_RenderDeviceDriver::ClearDriver() {}

FX_BOOL IFX_RenderDeviceDriver::DrawShading(const CPDF_ShadingPattern* pPattern,
                                            const CFX_Matrix* pMatrix,
                                            const FX_RECT& clip_rect,
                                            int alpha,
                                            FX_BOOL bAlphaMode) {
  return false;
}
