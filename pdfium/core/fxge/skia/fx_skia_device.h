// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_
#define CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_

#if defined(_SKIA_SUPPORT_)

#include "core/fxge/include/ifx_renderdevicedriver.h"

class SkCanvas;
class SkMatrix;
class SkPaint;
class SkPath;
class SkPictureRecorder;
class SkiaState;
struct SkIRect;

class CFX_SkiaDeviceDriver : public IFX_RenderDeviceDriver {
 public:
  CFX_SkiaDeviceDriver(CFX_DIBitmap* pBitmap,
                       FX_BOOL bRgbByteOrder,
                       CFX_DIBitmap* pOriDevice,
                       FX_BOOL bGroupKnockout);
  explicit CFX_SkiaDeviceDriver(SkPictureRecorder* recorder);
  CFX_SkiaDeviceDriver(int size_x, int size_y);
  ~CFX_SkiaDeviceDriver() override;

  /** Options */
  int GetDeviceCaps(int caps_id) const override;

  /** Save and restore all graphic states */
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;

  /** Set clipping path using filled region */
  FX_BOOL SetClip_PathFill(
      const CFX_PathData* pPathData,     // path info
      const CFX_Matrix* pObject2Device,  // optional transformation
      int fill_mode) override;           // fill mode, WINDING or ALTERNATE

  /** Set clipping path using stroked region */
  FX_BOOL SetClip_PathStroke(
      const CFX_PathData* pPathData,     // path info
      const CFX_Matrix* pObject2Device,  // optional transformation
      const CFX_GraphStateData*
          pGraphState)  // graphic state, for pen attributes
      override;

  /** Draw a path */
  FX_BOOL DrawPath(const CFX_PathData* pPathData,
                   const CFX_Matrix* pObject2Device,
                   const CFX_GraphStateData* pGraphState,
                   uint32_t fill_color,
                   uint32_t stroke_color,
                   int fill_mode,
                   int blend_type) override;

  FX_BOOL FillRectWithBlend(const FX_RECT* pRect,
                            uint32_t fill_color,
                            int blend_type) override;

  /** Draw a single pixel (device dependant) line */
  FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           uint32_t color,
                           int blend_type) override;

  FX_BOOL GetClipBox(FX_RECT* pRect) override;

  /** Load device buffer into a DIB */
  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap, int left, int top) override;

  CFX_DIBitmap* GetBackDrop() override;

  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int dest_left,
                    int dest_top,
                    int blend_type) override;
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        uint32_t color,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        uint32_t flags,
                        int blend_type) override;

  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t flags,
                      void*& handle,
                      int blend_type) override;

  FX_BOOL ContinueDIBits(void* handle, IFX_Pause* pPause) override;

  void CancelDIBits(void* handle) override {}

  FX_BOOL DrawDeviceText(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         const CFX_Matrix* pObject2Device,
                         FX_FLOAT font_size,
                         uint32_t color) override;

  FX_BOOL DrawShading(const CPDF_ShadingPattern* pPattern,
                      const CFX_Matrix* pMatrix,
                      const FX_RECT& clip_rect,
                      int alpha,
                      FX_BOOL bAlphaMode) override;

  virtual uint8_t* GetBuffer() const;

  void PaintStroke(SkPaint* spaint,
                   const CFX_GraphStateData* pGraphState,
                   const SkMatrix& matrix);
  void Flush();
  SkPictureRecorder* GetRecorder() const { return m_pRecorder; }
  static void PreMultiply(CFX_DIBitmap* pDIBitmap);
  SkCanvas* SkiaCanvas() { return m_pCanvas; }
  void DebugVerifyBitmapIsPreMultiplied() const;
  void Dump() const;

 private:
  friend class SkiaState;

  CFX_DIBitmap* m_pBitmap;
  CFX_DIBitmap* m_pOriDevice;
  SkCanvas* m_pCanvas;
  SkPictureRecorder* const m_pRecorder;
  std::unique_ptr<SkiaState> m_pCache;
  FX_BOOL m_bGroupKnockout;
};
#endif  // defined(_SKIA_SUPPORT_)

#endif  // CORE_FXGE_SKIA_FX_SKIA_DEVICE_H_
