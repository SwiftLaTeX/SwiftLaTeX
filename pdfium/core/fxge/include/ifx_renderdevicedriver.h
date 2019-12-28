// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_IFX_RENDERDEVICEDRIVER_H_
#define CORE_FXGE_INCLUDE_IFX_RENDERDEVICEDRIVER_H_

#include "core/fxcrt/include/fx_system.h"

class CFX_DIBitmap;
class CFX_DIBSource;
class CFX_Font;
class CFX_FontCache;
class CFX_GraphStateData;
class CFX_Matrix;
class CFX_PathData;
class CPDF_ShadingPattern;
class IFX_Pause;
struct FXTEXT_CHARPOS;
struct FX_RECT;

class IFX_RenderDeviceDriver {
 public:
  virtual ~IFX_RenderDeviceDriver();

  virtual int GetDeviceCaps(int caps_id) const = 0;
  virtual CFX_Matrix GetCTM() const;

  virtual FX_BOOL StartRendering();
  virtual void EndRendering();
  virtual void SaveState() = 0;
  virtual void RestoreState(bool bKeepSaved) = 0;

  virtual FX_BOOL SetClip_PathFill(const CFX_PathData* pPathData,
                                   const CFX_Matrix* pObject2Device,
                                   int fill_mode) = 0;
  virtual FX_BOOL SetClip_PathStroke(const CFX_PathData* pPathData,
                                     const CFX_Matrix* pObject2Device,
                                     const CFX_GraphStateData* pGraphState);
  virtual FX_BOOL DrawPath(const CFX_PathData* pPathData,
                           const CFX_Matrix* pObject2Device,
                           const CFX_GraphStateData* pGraphState,
                           uint32_t fill_color,
                           uint32_t stroke_color,
                           int fill_mode,
                           int blend_type) = 0;
  virtual FX_BOOL SetPixel(int x, int y, uint32_t color);
  virtual FX_BOOL FillRectWithBlend(const FX_RECT* pRect,
                                    uint32_t fill_color,
                                    int blend_type);
  virtual FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                                   FX_FLOAT y1,
                                   FX_FLOAT x2,
                                   FX_FLOAT y2,
                                   uint32_t color,
                                   int blend_type);

  virtual FX_BOOL GetClipBox(FX_RECT* pRect) = 0;
  virtual FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap, int left, int top);
  virtual CFX_DIBitmap* GetBackDrop();
  virtual FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                            uint32_t color,
                            const FX_RECT* pSrcRect,
                            int dest_left,
                            int dest_top,
                            int blend_type) = 0;
  virtual FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                                uint32_t color,
                                int dest_left,
                                int dest_top,
                                int dest_width,
                                int dest_height,
                                const FX_RECT* pClipRect,
                                uint32_t flags,
                                int blend_type) = 0;
  virtual FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                              int bitmap_alpha,
                              uint32_t color,
                              const CFX_Matrix* pMatrix,
                              uint32_t flags,
                              void*& handle,
                              int blend_type) = 0;
  virtual FX_BOOL ContinueDIBits(void* handle, IFX_Pause* pPause);
  virtual void CancelDIBits(void* handle);
  virtual FX_BOOL DrawDeviceText(int nChars,
                                 const FXTEXT_CHARPOS* pCharPos,
                                 CFX_Font* pFont,
                                 CFX_FontCache* pCache,
                                 const CFX_Matrix* pObject2Device,
                                 FX_FLOAT font_size,
                                 uint32_t color);
  virtual void* GetPlatformSurface() const;
  virtual int GetDriverType() const;
  virtual void ClearDriver();
  virtual FX_BOOL DrawShading(const CPDF_ShadingPattern* pPattern,
                              const CFX_Matrix* pMatrix,
                              const FX_RECT& clip_rect,
                              int alpha,
                              FX_BOOL bAlphaMode);
};

#endif  // CORE_FXGE_INCLUDE_IFX_RENDERDEVICEDRIVER_H_
