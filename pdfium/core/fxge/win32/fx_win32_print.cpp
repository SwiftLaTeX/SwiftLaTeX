// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/include/fx_ge.h"

#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_DESKTOP_

#include <windows.h>

#include "core/fxge/dib/dib_int.h"
#include "core/fxge/ge/fx_text_int.h"
#include "core/fxge/include/fx_freetype.h"
#include "core/fxge/include/fx_ge_win32.h"
#include "core/fxge/win32/win32_int.h"

CGdiPrinterDriver::CGdiPrinterDriver(HDC hDC)
    : CGdiDeviceDriver(hDC, FXDC_PRINTER),
      m_HorzSize(::GetDeviceCaps(m_hDC, HORZSIZE)),
      m_VertSize(::GetDeviceCaps(m_hDC, VERTSIZE)) {}

CGdiPrinterDriver::~CGdiPrinterDriver() {}

int CGdiPrinterDriver::GetDeviceCaps(int caps_id) const {
  if (caps_id == FXDC_HORZ_SIZE)
    return m_HorzSize;
  if (caps_id == FXDC_VERT_SIZE)
    return m_VertSize;
  return CGdiDeviceDriver::GetDeviceCaps(caps_id);
}

FX_BOOL CGdiPrinterDriver::SetDIBits(const CFX_DIBSource* pSource,
                                     uint32_t color,
                                     const FX_RECT* pSrcRect,
                                     int left,
                                     int top,
                                     int blend_type) {
  if (pSource->IsAlphaMask()) {
    FX_RECT clip_rect(left, top, left + pSrcRect->Width(),
                      top + pSrcRect->Height());
    return StretchDIBits(pSource, color, left - pSrcRect->left,
                         top - pSrcRect->top, pSource->GetWidth(),
                         pSource->GetHeight(), &clip_rect, 0,
                         FXDIB_BLEND_NORMAL);
  }
  ASSERT(pSource && !pSource->IsAlphaMask() && pSrcRect);
  ASSERT(blend_type == FXDIB_BLEND_NORMAL);
  if (pSource->HasAlpha())
    return FALSE;

  CFX_DIBExtractor temp(pSource);
  CFX_DIBitmap* pBitmap = temp;
  if (!pBitmap)
    return FALSE;

  return GDI_SetDIBits(pBitmap, pSrcRect, left, top);
}

FX_BOOL CGdiPrinterDriver::StretchDIBits(const CFX_DIBSource* pSource,
                                         uint32_t color,
                                         int dest_left,
                                         int dest_top,
                                         int dest_width,
                                         int dest_height,
                                         const FX_RECT* pClipRect,
                                         uint32_t flags,
                                         int blend_type) {
  if (pSource->IsAlphaMask()) {
    int alpha = FXARGB_A(color);
    if (pSource->GetBPP() != 1 || alpha != 255)
      return FALSE;

    if (dest_width < 0 || dest_height < 0) {
      std::unique_ptr<CFX_DIBitmap> pFlipped(
          pSource->FlipImage(dest_width < 0, dest_height < 0));
      if (!pFlipped)
        return FALSE;

      if (dest_width < 0)
        dest_left += dest_width;
      if (dest_height < 0)
        dest_top += dest_height;

      return GDI_StretchBitMask(pFlipped.get(), dest_left, dest_top,
                                abs(dest_width), abs(dest_height), color,
                                flags);
    }

    CFX_DIBExtractor temp(pSource);
    CFX_DIBitmap* pBitmap = temp;
    if (!pBitmap)
      return FALSE;
    return GDI_StretchBitMask(pBitmap, dest_left, dest_top, dest_width,
                              dest_height, color, flags);
  }

  if (pSource->HasAlpha())
    return FALSE;

  if (dest_width < 0 || dest_height < 0) {
    std::unique_ptr<CFX_DIBitmap> pFlipped(
        pSource->FlipImage(dest_width < 0, dest_height < 0));
    if (!pFlipped)
      return FALSE;

    if (dest_width < 0)
      dest_left += dest_width;
    if (dest_height < 0)
      dest_top += dest_height;

    return GDI_StretchDIBits(pFlipped.get(), dest_left, dest_top,
                             abs(dest_width), abs(dest_height), flags);
  }

  CFX_DIBExtractor temp(pSource);
  CFX_DIBitmap* pBitmap = temp;
  if (!pBitmap)
    return FALSE;
  return GDI_StretchDIBits(pBitmap, dest_left, dest_top, dest_width,
                           dest_height, flags);
}

FX_BOOL CGdiPrinterDriver::StartDIBits(const CFX_DIBSource* pSource,
                                       int bitmap_alpha,
                                       uint32_t color,
                                       const CFX_Matrix* pMatrix,
                                       uint32_t render_flags,
                                       void*& handle,
                                       int blend_type) {
  if (bitmap_alpha < 255 || pSource->HasAlpha() ||
      (pSource->IsAlphaMask() && (pSource->GetBPP() != 1))) {
    return FALSE;
  }
  CFX_FloatRect unit_rect = pMatrix->GetUnitRect();
  FX_RECT full_rect = unit_rect.GetOutterRect();
  if (FXSYS_fabs(pMatrix->b) < 0.5f && pMatrix->a != 0 &&
      FXSYS_fabs(pMatrix->c) < 0.5f && pMatrix->d != 0) {
    FX_BOOL bFlipX = pMatrix->a < 0;
    FX_BOOL bFlipY = pMatrix->d > 0;
    return StretchDIBits(pSource, color,
                         bFlipX ? full_rect.right : full_rect.left,
                         bFlipY ? full_rect.bottom : full_rect.top,
                         bFlipX ? -full_rect.Width() : full_rect.Width(),
                         bFlipY ? -full_rect.Height() : full_rect.Height(),
                         nullptr, 0, blend_type);
  }
  if (FXSYS_fabs(pMatrix->a) < 0.5f && FXSYS_fabs(pMatrix->d) < 0.5f) {
    std::unique_ptr<CFX_DIBitmap> pTransformed(
        pSource->SwapXY(pMatrix->c > 0, pMatrix->b < 0));
    if (!pTransformed)
      return FALSE;

    return StretchDIBits(pTransformed.get(), color, full_rect.left,
                         full_rect.top, full_rect.Width(), full_rect.Height(),
                         nullptr, 0, blend_type);
  }
  return FALSE;
}

#endif
