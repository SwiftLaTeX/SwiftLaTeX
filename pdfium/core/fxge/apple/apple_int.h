// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_APPLE_APPLE_INT_H_
#define CORE_FXGE_APPLE_APPLE_INT_H_

#include "core/fxcrt/include/fx_system.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_

#include "core/fxge/include/fx_dib.h"
#include "core/fxge/include/ifx_renderdevicedriver.h"

#if _FX_OS_ == _FX_MACOSX_
#include <Carbon/Carbon.h>
#endif

class CQuartz2D {
 public:
  void* createGraphics(CFX_DIBitmap* bitmap);
  void destroyGraphics(void* graphics);

  void* CreateFont(const uint8_t* pFontData, uint32_t dwFontSize);
  void DestroyFont(void* pFont);
  void setGraphicsTextMatrix(void* graphics, CFX_Matrix* matrix);
  FX_BOOL drawGraphicsString(void* graphics,
                             void* font,
                             FX_FLOAT fontSize,
                             uint16_t* glyphIndices,
                             CGPoint* glyphPositions,
                             int32_t chars,
                             FX_ARGB argb,
                             CFX_Matrix* matrix = nullptr);
  void saveGraphicsState(void* graphics);
  void restoreGraphicsState(void* graphics);
};

class CApplePlatform {
 public:
  CApplePlatform() {}
  ~CApplePlatform() {}

  CQuartz2D m_quartz2d;
};

class CFX_QuartzDeviceDriver : public IFX_RenderDeviceDriver {
 public:
  CFX_QuartzDeviceDriver(CGContextRef context, int32_t deviceClass);
  ~CFX_QuartzDeviceDriver() override;

  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) const override;
  CFX_Matrix GetCTM() const override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  FX_BOOL SetClip_PathFill(const CFX_PathData* pPathData,
                           const CFX_Matrix* pObject2Device,
                           int fill_mode) override;
  FX_BOOL SetClip_PathStroke(const CFX_PathData* pPathData,
                             const CFX_Matrix* pObject2Device,
                             const CFX_GraphStateData* pGraphState) override;
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
  FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           uint32_t color,
                           int blend_type) override;
  FX_BOOL GetClipBox(FX_RECT* pRect) override;
  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap, int left, int top) override;
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
  FX_BOOL DrawDeviceText(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         const CFX_Matrix* pObject2Device,
                         FX_FLOAT font_size,
                         uint32_t color) override;
  void ClearDriver() override;

 protected:
  void setStrokeInfo(const CFX_GraphStateData* graphState,
                     FX_ARGB argb,
                     FX_FLOAT lineWidth);
  void setFillInfo(FX_ARGB argb);
  void setPathToContext(const CFX_PathData* pathData);
  FX_FLOAT getLineWidth(const CFX_GraphStateData* graphState,
                        CGAffineTransform ctm);
  FX_BOOL CG_DrawGlyphRun(int nChars,
                          const FXTEXT_CHARPOS* pCharPos,
                          CFX_Font* pFont,
                          CFX_FontCache* pCache,
                          const CFX_Matrix* pGlyphMatrix,
                          const CFX_Matrix* pObject2Device,
                          FX_FLOAT font_size,
                          uint32_t argb);
  void CG_SetImageTransform(int dest_left,
                            int dest_top,
                            int dest_width,
                            int dest_height,
                            CGRect* rect);

  CGContextRef m_context;
  CGAffineTransform m_foxitDevice2User;
  CGAffineTransform m_user2FoxitDevice;
  int32_t m_saveCount;

  int32_t m_width;
  int32_t m_height;
  int32_t m_bitsPerPixel;
  int32_t m_deviceClass;
  int32_t m_renderCaps;
  int32_t m_horzSize;
  int32_t m_vertSize;
};
#endif  // _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_

#endif  // CORE_FXGE_APPLE_APPLE_INT_H_
