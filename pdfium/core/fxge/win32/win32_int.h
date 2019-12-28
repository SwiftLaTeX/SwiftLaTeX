// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_WIN32_WIN32_INT_H_
#define CORE_FXGE_WIN32_WIN32_INT_H_

#include "core/fxge/include/ifx_renderdevicedriver.h"
#include "core/fxge/win32/dwrite_int.h"

struct WINDIB_Open_Args_;
class CGdiplusExt {
 public:
  CGdiplusExt();
  ~CGdiplusExt();
  void Load();
  FX_BOOL IsAvailable() { return !!m_hModule; }
  FX_BOOL StretchBitMask(HDC hDC,
                         BOOL bMonoDevice,
                         const CFX_DIBitmap* pBitmap,
                         int dest_left,
                         int dest_top,
                         int dest_width,
                         int dest_height,
                         uint32_t argb,
                         const FX_RECT* pClipRect,
                         int flags);
  FX_BOOL StretchDIBits(HDC hDC,
                        const CFX_DIBitmap* pBitmap,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        int flags);
  FX_BOOL DrawPath(HDC hDC,
                   const CFX_PathData* pPathData,
                   const CFX_Matrix* pObject2Device,
                   const CFX_GraphStateData* pGraphState,
                   uint32_t fill_argb,
                   uint32_t stroke_argb,
                   int fill_mode);

  void* LoadMemFont(uint8_t* pData, uint32_t size);
  void DeleteMemFont(void* pFontCollection);
  FX_BOOL GdipCreateFromImage(void* bitmap, void** graphics);
  void GdipDeleteGraphics(void* graphics);
  void GdipSetTextRenderingHint(void* graphics, int mode);
  void GdipSetPageUnit(void* graphics, uint32_t unit);
  void GdipSetWorldTransform(void* graphics, void* pMatrix);
  FX_BOOL GdipDrawDriverString(void* graphics,
                               unsigned short* text,
                               int length,
                               void* font,
                               void* brush,
                               void* positions,
                               int flags,
                               const void* matrix);
  void GdipCreateBrush(uint32_t fill_argb, void** pBrush);
  void GdipDeleteBrush(void* pBrush);
  void GdipCreateMatrix(FX_FLOAT a,
                        FX_FLOAT b,
                        FX_FLOAT c,
                        FX_FLOAT d,
                        FX_FLOAT e,
                        FX_FLOAT f,
                        void** matrix);
  void GdipDeleteMatrix(void* matrix);
  FX_BOOL GdipCreateFontFamilyFromName(const FX_WCHAR* name,
                                       void* pFontCollection,
                                       void** pFamily);
  void GdipDeleteFontFamily(void* pFamily);
  FX_BOOL GdipCreateFontFromFamily(void* pFamily,
                                   FX_FLOAT font_size,
                                   int fontstyle,
                                   int flag,
                                   void** pFont);
  void* GdipCreateFontFromCollection(void* pFontCollection,
                                     FX_FLOAT font_size,
                                     int fontstyle);
  void GdipDeleteFont(void* pFont);
  FX_BOOL GdipCreateBitmap(CFX_DIBitmap* pBitmap, void** bitmap);
  void GdipDisposeImage(void* bitmap);
  void GdipGetFontSize(void* pFont, FX_FLOAT* size);
  void* GdiAddFontMemResourceEx(void* pFontdata,
                                uint32_t size,
                                void* pdv,
                                uint32_t* num_face);
  FX_BOOL GdiRemoveFontMemResourceEx(void* handle);
  void* m_Functions[100];
  void* m_pGdiAddFontMemResourceEx;
  void* m_pGdiRemoveFontMemResourseEx;
  CFX_DIBitmap* LoadDIBitmap(WINDIB_Open_Args_ args);

 protected:
  HMODULE m_hModule;
  HMODULE m_GdiModule;
};
class CWin32Platform {
 public:
  FX_BOOL m_bHalfTone;
  CGdiplusExt m_GdiplusExt;
  CDWriteExt m_DWriteExt;
};

class CGdiDeviceDriver : public IFX_RenderDeviceDriver {
 protected:
  CGdiDeviceDriver(HDC hDC, int device_class);
  ~CGdiDeviceDriver() override;

  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) const override;
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
  void* GetPlatformSurface() const override;

  void DrawLine(FX_FLOAT x1,
                FX_FLOAT y1,
                FX_FLOAT x2,
                FX_FLOAT y2,
                const CFX_Matrix* pMatrix);

  FX_BOOL GDI_SetDIBits(CFX_DIBitmap* pBitmap,
                        const FX_RECT* pSrcRect,
                        int left,
                        int top);
  FX_BOOL GDI_StretchDIBits(CFX_DIBitmap* pBitmap,
                            int dest_left,
                            int dest_top,
                            int dest_width,
                            int dest_height,
                            uint32_t flags);
  FX_BOOL GDI_StretchBitMask(CFX_DIBitmap* pBitmap,
                             int dest_left,
                             int dest_top,
                             int dest_width,
                             int dest_height,
                             uint32_t bitmap_color,
                             uint32_t flags);

  HDC m_hDC;
  int m_Width;
  int m_Height;
  int m_nBitsPerPixel;
  int m_DeviceClass;
  int m_RenderCaps;
};

class CGdiDisplayDriver : public CGdiDeviceDriver {
 public:
  explicit CGdiDisplayDriver(HDC hDC);
  ~CGdiDisplayDriver() override;

 protected:
  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap, int left, int top) override;
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int left,
                    int top,
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
                      uint32_t render_flags,
                      void*& handle,
                      int blend_type) override;
  FX_BOOL UseFoxitStretchEngine(const CFX_DIBSource* pSource,
                                uint32_t color,
                                int dest_left,
                                int dest_top,
                                int dest_width,
                                int dest_height,
                                const FX_RECT* pClipRect,
                                int render_flags);
};

class CGdiPrinterDriver : public CGdiDeviceDriver {
 public:
  explicit CGdiPrinterDriver(HDC hDC);
  ~CGdiPrinterDriver() override;

 protected:
  int GetDeviceCaps(int caps_id) const override;
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int left,
                    int top,
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
                      uint32_t render_flags,
                      void*& handle,
                      int blend_type) override;

  const int m_HorzSize;
  const int m_VertSize;
};

#endif  // CORE_FXGE_WIN32_WIN32_INT_H_
