// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/include/fx_ge.h"
#include "core/fxge/include/ifx_renderdevicedriver.h"

#if defined _SKIA_SUPPORT_
#include "third_party/skia/include/core/SkTypes.h"
#endif

CFX_RenderDevice::CFX_RenderDevice()
    : m_pBitmap(nullptr),
      m_Width(0),
      m_Height(0),
      m_bpp(0),
      m_RenderCaps(0),
      m_DeviceClass(0),
      m_pDeviceDriver(nullptr) {}

CFX_RenderDevice::~CFX_RenderDevice() {
  delete m_pDeviceDriver;
}

void CFX_RenderDevice::Flush() {
  delete m_pDeviceDriver;
  m_pDeviceDriver = nullptr;
}

void CFX_RenderDevice::SetDeviceDriver(IFX_RenderDeviceDriver* pDriver) {
  delete m_pDeviceDriver;
  m_pDeviceDriver = pDriver;
  InitDeviceInfo();
}

void CFX_RenderDevice::InitDeviceInfo() {
  m_Width = m_pDeviceDriver->GetDeviceCaps(FXDC_PIXEL_WIDTH);
  m_Height = m_pDeviceDriver->GetDeviceCaps(FXDC_PIXEL_HEIGHT);
  m_bpp = m_pDeviceDriver->GetDeviceCaps(FXDC_BITS_PIXEL);
  m_RenderCaps = m_pDeviceDriver->GetDeviceCaps(FXDC_RENDER_CAPS);
  m_DeviceClass = m_pDeviceDriver->GetDeviceCaps(FXDC_DEVICE_CLASS);
  if (!m_pDeviceDriver->GetClipBox(&m_ClipBox)) {
    m_ClipBox.left = 0;
    m_ClipBox.top = 0;
    m_ClipBox.right = m_Width;
    m_ClipBox.bottom = m_Height;
  }
}

FX_BOOL CFX_RenderDevice::StartRendering() {
  return m_pDeviceDriver->StartRendering();
}

void CFX_RenderDevice::EndRendering() {
  m_pDeviceDriver->EndRendering();
}

void CFX_RenderDevice::SaveState() {
  m_pDeviceDriver->SaveState();
}

void CFX_RenderDevice::RestoreState(bool bKeepSaved) {
  m_pDeviceDriver->RestoreState(bKeepSaved);
  UpdateClipBox();
}

int CFX_RenderDevice::GetDeviceCaps(int caps_id) const {
  return m_pDeviceDriver->GetDeviceCaps(caps_id);
}
CFX_Matrix CFX_RenderDevice::GetCTM() const {
  return m_pDeviceDriver->GetCTM();
}

FX_BOOL CFX_RenderDevice::CreateCompatibleBitmap(CFX_DIBitmap* pDIB,
                                                 int width,
                                                 int height) const {
  if (m_RenderCaps & FXRC_CMYK_OUTPUT) {
    return pDIB->Create(width, height, m_RenderCaps & FXRC_ALPHA_OUTPUT
                                           ? FXDIB_Cmyka
                                           : FXDIB_Cmyk);
  }
  if (m_RenderCaps & FXRC_BYTEMASK_OUTPUT) {
    return pDIB->Create(width, height, FXDIB_8bppMask);
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  return pDIB->Create(width, height, m_RenderCaps & FXRC_ALPHA_OUTPUT
                                         ? FXDIB_Argb
                                         : FXDIB_Rgb32);
#else
  return pDIB->Create(
      width, height, m_RenderCaps & FXRC_ALPHA_OUTPUT ? FXDIB_Argb : FXDIB_Rgb);
#endif
}

FX_BOOL CFX_RenderDevice::SetClip_PathFill(const CFX_PathData* pPathData,
                                           const CFX_Matrix* pObject2Device,
                                           int fill_mode) {
  if (!m_pDeviceDriver->SetClip_PathFill(pPathData, pObject2Device,
                                         fill_mode)) {
    return FALSE;
  }
  UpdateClipBox();
  return TRUE;
}

FX_BOOL CFX_RenderDevice::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  if (!m_pDeviceDriver->SetClip_PathStroke(pPathData, pObject2Device,
                                           pGraphState)) {
    return FALSE;
  }
  UpdateClipBox();
  return TRUE;
}

FX_BOOL CFX_RenderDevice::SetClip_Rect(const FX_RECT& rect) {
  CFX_PathData path;
  path.AppendRect(rect.left, rect.bottom, rect.right, rect.top);
  if (!SetClip_PathFill(&path, nullptr, FXFILL_WINDING))
    return FALSE;

  UpdateClipBox();
  return TRUE;
}

void CFX_RenderDevice::UpdateClipBox() {
  if (m_pDeviceDriver->GetClipBox(&m_ClipBox)) {
    return;
  }
  m_ClipBox.left = 0;
  m_ClipBox.top = 0;
  m_ClipBox.right = m_Width;
  m_ClipBox.bottom = m_Height;
}

FX_BOOL CFX_RenderDevice::DrawPathWithBlend(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState,
    uint32_t fill_color,
    uint32_t stroke_color,
    int fill_mode,
    int blend_type) {
  uint8_t stroke_alpha = pGraphState ? FXARGB_A(stroke_color) : 0;
  uint8_t fill_alpha = (fill_mode & 3) ? FXARGB_A(fill_color) : 0;
  if (stroke_alpha == 0 && pPathData->GetPointCount() == 2) {
    FX_PATHPOINT* pPoints = pPathData->GetPoints();
    FX_FLOAT x1, x2, y1, y2;
    if (pObject2Device) {
      pObject2Device->Transform(pPoints[0].m_PointX, pPoints[0].m_PointY, x1,
                                y1);
      pObject2Device->Transform(pPoints[1].m_PointX, pPoints[1].m_PointY, x2,
                                y2);
    } else {
      x1 = pPoints[0].m_PointX;
      y1 = pPoints[0].m_PointY;
      x2 = pPoints[1].m_PointX;
      y2 = pPoints[1].m_PointY;
    }
    DrawCosmeticLineWithFillModeAndBlend(x1, y1, x2, y2, fill_color, fill_mode,
                                         blend_type);
    return TRUE;
  }
  if ((pPathData->GetPointCount() == 5 || pPathData->GetPointCount() == 4) &&
      stroke_alpha == 0) {
    CFX_FloatRect rect_f;
    if (!(fill_mode & FXFILL_RECT_AA) &&
        pPathData->IsRect(pObject2Device, &rect_f)) {
      FX_RECT rect_i = rect_f.GetOutterRect();
      int width = (int)FXSYS_ceil(rect_f.right - rect_f.left);
      if (width < 1) {
        width = 1;
        if (rect_i.left == rect_i.right) {
          rect_i.right++;
        }
      }
      int height = (int)FXSYS_ceil(rect_f.top - rect_f.bottom);
      if (height < 1) {
        height = 1;
        if (rect_i.bottom == rect_i.top) {
          rect_i.bottom++;
        }
      }
      if (rect_i.Width() >= width + 1) {
        if (rect_f.left - (FX_FLOAT)(rect_i.left) >
            (FX_FLOAT)(rect_i.right) - rect_f.right) {
          rect_i.left++;
        } else {
          rect_i.right--;
        }
      }
      if (rect_i.Height() >= height + 1) {
        if (rect_f.top - (FX_FLOAT)(rect_i.top) >
            (FX_FLOAT)(rect_i.bottom) - rect_f.bottom) {
          rect_i.top++;
        } else {
          rect_i.bottom--;
        }
      }
      if (FillRectWithBlend(&rect_i, fill_color, blend_type)) {
        return TRUE;
      }
    }
  }
  if ((fill_mode & 3) && stroke_alpha == 0 && !(fill_mode & FX_FILL_STROKE) &&
      !(fill_mode & FX_FILL_TEXT_MODE)) {
    CFX_PathData newPath;
    FX_BOOL bThin = FALSE;
    if (pPathData->GetZeroAreaPath(newPath, (CFX_Matrix*)pObject2Device, bThin,
                                   m_pDeviceDriver->GetDriverType())) {
      CFX_GraphStateData graphState;
      graphState.m_LineWidth = 0.0f;
      uint32_t strokecolor = fill_color;
      if (bThin) {
        strokecolor = (((fill_alpha >> 2) << 24) | (strokecolor & 0x00ffffff));
      }
      CFX_Matrix* pMatrix = nullptr;
      if (pObject2Device && !pObject2Device->IsIdentity()) {
        pMatrix = (CFX_Matrix*)pObject2Device;
      }
      int smooth_path = FX_ZEROAREA_FILL;
      if (fill_mode & FXFILL_NOPATHSMOOTH) {
        smooth_path |= FXFILL_NOPATHSMOOTH;
      }
      m_pDeviceDriver->DrawPath(&newPath, pMatrix, &graphState, 0, strokecolor,
                                smooth_path, blend_type);
    }
  }
  if ((fill_mode & 3) && fill_alpha && stroke_alpha < 0xff &&
      (fill_mode & FX_FILL_STROKE)) {
    if (m_RenderCaps & FXRC_FILLSTROKE_PATH) {
      return m_pDeviceDriver->DrawPath(pPathData, pObject2Device, pGraphState,
                                       fill_color, stroke_color, fill_mode,
                                       blend_type);
    }
    return DrawFillStrokePath(pPathData, pObject2Device, pGraphState,
                              fill_color, stroke_color, fill_mode, blend_type);
  }
  return m_pDeviceDriver->DrawPath(pPathData, pObject2Device, pGraphState,
                                   fill_color, stroke_color, fill_mode,
                                   blend_type);
}

// This can be removed once PDFium entirely relies on Skia
FX_BOOL CFX_RenderDevice::DrawFillStrokePath(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState,
    uint32_t fill_color,
    uint32_t stroke_color,
    int fill_mode,
    int blend_type) {
    if (!(m_RenderCaps & FXRC_GET_BITS)) {
      return FALSE;
    }
    CFX_FloatRect bbox;
    if (pGraphState) {
      bbox = pPathData->GetBoundingBox(pGraphState->m_LineWidth,
                                       pGraphState->m_MiterLimit);
    } else {
      bbox = pPathData->GetBoundingBox();
    }
    if (pObject2Device) {
      bbox.Transform(pObject2Device);
    }
    CFX_Matrix ctm = GetCTM();
    FX_FLOAT fScaleX = FXSYS_fabs(ctm.a);
    FX_FLOAT fScaleY = FXSYS_fabs(ctm.d);
    FX_RECT rect = bbox.GetOutterRect();
    CFX_DIBitmap bitmap, Backdrop;
    if (!CreateCompatibleBitmap(&bitmap, FXSYS_round(rect.Width() * fScaleX),
                                FXSYS_round(rect.Height() * fScaleY))) {
      return FALSE;
    }
    if (bitmap.HasAlpha()) {
      bitmap.Clear(0);
      Backdrop.Copy(&bitmap);
    } else {
      if (!m_pDeviceDriver->GetDIBits(&bitmap, rect.left, rect.top))
        return FALSE;
      Backdrop.Copy(&bitmap);
    }
    CFX_FxgeDevice bitmap_device;
    bitmap_device.Attach(&bitmap, false, &Backdrop, true);
    CFX_Matrix matrix;
    if (pObject2Device) {
      matrix = *pObject2Device;
    }
    matrix.TranslateI(-rect.left, -rect.top);
    matrix.Concat(fScaleX, 0, 0, fScaleY, 0, 0);
    if (!bitmap_device.GetDeviceDriver()->DrawPath(
            pPathData, &matrix, pGraphState, fill_color, stroke_color,
            fill_mode, blend_type)) {
      return FALSE;
    }
    FX_RECT src_rect(0, 0, FXSYS_round(rect.Width() * fScaleX),
                     FXSYS_round(rect.Height() * fScaleY));
    return m_pDeviceDriver->SetDIBits(&bitmap, 0, &src_rect, rect.left,
                                      rect.top, FXDIB_BLEND_NORMAL);
}

FX_BOOL CFX_RenderDevice::SetPixel(int x, int y, uint32_t color) {
  if (m_pDeviceDriver->SetPixel(x, y, color))
    return TRUE;

  FX_RECT rect(x, y, x + 1, y + 1);
  return FillRectWithBlend(&rect, color, FXDIB_BLEND_NORMAL);
}

FX_BOOL CFX_RenderDevice::FillRectWithBlend(const FX_RECT* pRect,
                                            uint32_t fill_color,
                                            int blend_type) {
  if (m_pDeviceDriver->FillRectWithBlend(pRect, fill_color, blend_type))
    return TRUE;

  if (!(m_RenderCaps & FXRC_GET_BITS))
    return FALSE;

  CFX_DIBitmap bitmap;
  if (!CreateCompatibleBitmap(&bitmap, pRect->Width(), pRect->Height()))
    return FALSE;

  if (!m_pDeviceDriver->GetDIBits(&bitmap, pRect->left, pRect->top))
    return FALSE;

  if (!bitmap.CompositeRect(0, 0, pRect->Width(), pRect->Height(), fill_color,
                            0, nullptr)) {
    return FALSE;
  }
  FX_RECT src_rect(0, 0, pRect->Width(), pRect->Height());
  m_pDeviceDriver->SetDIBits(&bitmap, 0, &src_rect, pRect->left, pRect->top,
                             FXDIB_BLEND_NORMAL);
  return TRUE;
}

FX_BOOL CFX_RenderDevice::DrawCosmeticLineWithFillModeAndBlend(FX_FLOAT x1,
                                                               FX_FLOAT y1,
                                                               FX_FLOAT x2,
                                                               FX_FLOAT y2,
                                                               uint32_t color,
                                                               int fill_mode,
                                                               int blend_type) {
  if ((color >= 0xff000000) &&
      m_pDeviceDriver->DrawCosmeticLine(x1, y1, x2, y2, color, blend_type)) {
    return TRUE;
  }
  CFX_GraphStateData graph_state;
  CFX_PathData path;
  path.SetPointCount(2);
  path.SetPoint(0, x1, y1, FXPT_MOVETO);
  path.SetPoint(1, x2, y2, FXPT_LINETO);
  return m_pDeviceDriver->DrawPath(&path, nullptr, &graph_state, 0, color,
                                   fill_mode, blend_type);
}

FX_BOOL CFX_RenderDevice::GetDIBits(CFX_DIBitmap* pBitmap, int left, int top) {
  if (!(m_RenderCaps & FXRC_GET_BITS))
    return FALSE;
  return m_pDeviceDriver->GetDIBits(pBitmap, left, top);
}

CFX_DIBitmap* CFX_RenderDevice::GetBackDrop() {
  return m_pDeviceDriver->GetBackDrop();
}

FX_BOOL CFX_RenderDevice::SetDIBitsWithBlend(const CFX_DIBSource* pBitmap,
                                             int left,
                                             int top,
                                             int blend_mode) {
  ASSERT(!pBitmap->IsAlphaMask());
  CFX_Matrix ctm = GetCTM();
  FX_FLOAT fScaleX = FXSYS_fabs(ctm.a);
  FX_FLOAT fScaleY = FXSYS_fabs(ctm.d);
  FX_RECT dest_rect(left, top,
                    FXSYS_round(left + pBitmap->GetWidth() / fScaleX),
                    FXSYS_round(top + pBitmap->GetHeight() / fScaleY));
  dest_rect.Intersect(m_ClipBox);
  if (dest_rect.IsEmpty()) {
    return TRUE;
  }
  FX_RECT src_rect(dest_rect.left - left, dest_rect.top - top,
                   dest_rect.left - left + dest_rect.Width(),
                   dest_rect.top - top + dest_rect.Height());
  src_rect.left = FXSYS_round(src_rect.left * fScaleX);
  src_rect.top = FXSYS_round(src_rect.top * fScaleY);
  src_rect.right = FXSYS_round(src_rect.right * fScaleX);
  src_rect.bottom = FXSYS_round(src_rect.bottom * fScaleY);
  if ((blend_mode != FXDIB_BLEND_NORMAL && !(m_RenderCaps & FXRC_BLEND_MODE)) ||
      (pBitmap->HasAlpha() && !(m_RenderCaps & FXRC_ALPHA_IMAGE))) {
    if (!(m_RenderCaps & FXRC_GET_BITS)) {
      return FALSE;
    }
    int bg_pixel_width = FXSYS_round(dest_rect.Width() * fScaleX);
    int bg_pixel_height = FXSYS_round(dest_rect.Height() * fScaleY);
    CFX_DIBitmap background;
    if (!background.Create(
            bg_pixel_width, bg_pixel_height,
            (m_RenderCaps & FXRC_CMYK_OUTPUT) ? FXDIB_Cmyk : FXDIB_Rgb32)) {
      return FALSE;
    }
    if (!m_pDeviceDriver->GetDIBits(&background, dest_rect.left,
                                    dest_rect.top)) {
      return FALSE;
    }
    if (!background.CompositeBitmap(0, 0, bg_pixel_width, bg_pixel_height,
                                    pBitmap, src_rect.left, src_rect.top,
                                    blend_mode, nullptr, FALSE, nullptr)) {
      return FALSE;
    }
    FX_RECT rect(0, 0, bg_pixel_width, bg_pixel_height);
    return m_pDeviceDriver->SetDIBits(&background, 0, &rect, dest_rect.left,
                                      dest_rect.top, FXDIB_BLEND_NORMAL);
  }
  return m_pDeviceDriver->SetDIBits(pBitmap, 0, &src_rect, dest_rect.left,
                                    dest_rect.top, blend_mode);
}

FX_BOOL CFX_RenderDevice::StretchDIBitsWithFlagsAndBlend(
    const CFX_DIBSource* pBitmap,
    int left,
    int top,
    int dest_width,
    int dest_height,
    uint32_t flags,
    int blend_mode) {
  FX_RECT dest_rect(left, top, left + dest_width, top + dest_height);
  FX_RECT clip_box = m_ClipBox;
  clip_box.Intersect(dest_rect);
  if (clip_box.IsEmpty())
    return TRUE;
  return m_pDeviceDriver->StretchDIBits(pBitmap, 0, left, top, dest_width,
                                        dest_height, &clip_box, flags,
                                        blend_mode);
}

FX_BOOL CFX_RenderDevice::SetBitMask(const CFX_DIBSource* pBitmap,
                                     int left,
                                     int top,
                                     uint32_t argb) {
  FX_RECT src_rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
  return m_pDeviceDriver->SetDIBits(pBitmap, argb, &src_rect, left, top,
                                    FXDIB_BLEND_NORMAL);
}

FX_BOOL CFX_RenderDevice::StretchBitMask(const CFX_DIBSource* pBitmap,
                                         int left,
                                         int top,
                                         int dest_width,
                                         int dest_height,
                                         uint32_t color) {
  return StretchBitMaskWithFlags(pBitmap, left, top, dest_width, dest_height,
                                 color, 0);
}

FX_BOOL CFX_RenderDevice::StretchBitMaskWithFlags(const CFX_DIBSource* pBitmap,
                                                  int left,
                                                  int top,
                                                  int dest_width,
                                                  int dest_height,
                                                  uint32_t argb,
                                                  uint32_t flags) {
  FX_RECT dest_rect(left, top, left + dest_width, top + dest_height);
  FX_RECT clip_box = m_ClipBox;
  clip_box.Intersect(dest_rect);
  return m_pDeviceDriver->StretchDIBits(pBitmap, argb, left, top, dest_width,
                                        dest_height, &clip_box, flags,
                                        FXDIB_BLEND_NORMAL);
}

FX_BOOL CFX_RenderDevice::StartDIBitsWithBlend(const CFX_DIBSource* pBitmap,
                                               int bitmap_alpha,
                                               uint32_t argb,
                                               const CFX_Matrix* pMatrix,
                                               uint32_t flags,
                                               void*& handle,
                                               int blend_mode) {
  return m_pDeviceDriver->StartDIBits(pBitmap, bitmap_alpha, argb, pMatrix,
                                      flags, handle, blend_mode);
}

FX_BOOL CFX_RenderDevice::ContinueDIBits(void* handle, IFX_Pause* pPause) {
  return m_pDeviceDriver->ContinueDIBits(handle, pPause);
}

void CFX_RenderDevice::CancelDIBits(void* handle) {
  m_pDeviceDriver->CancelDIBits(handle);
}

#ifdef _SKIA_SUPPORT_

void CFX_RenderDevice::DebugVerifyBitmapIsPreMultiplied() const {
  SkASSERT(0);
}
#endif
