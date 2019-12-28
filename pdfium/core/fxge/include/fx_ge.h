// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_FX_GE_H_
#define CORE_FXGE_INCLUDE_FX_GE_H_

#include <memory>

#include "core/fxge/include/fx_dib.h"
#include "core/fxge/include/fx_font.h"

class CCodec_ModuleMgr;
class CFX_FaceCache;
class CFX_Font;
class CFX_FontCache;
class CFX_FontMgr;
class CPDF_ShadingPattern;
class IFX_RenderDeviceDriver;
class SkPictureRecorder;

class CFX_GEModule {
 public:
  static void Create(const char** pUserFontPaths,
                     CCodec_ModuleMgr* pCodecModule);
  static CFX_GEModule* Get();
  static void Destroy();

  CFX_FontCache* GetFontCache();
  CFX_FontMgr* GetFontMgr() { return m_pFontMgr.get(); }
  void SetTextGamma(FX_FLOAT gammaValue);
  const uint8_t* GetTextGammaTable() const;

  CCodec_ModuleMgr* GetCodecModule() { return m_pCodecModule; }
  void* GetPlatformData() { return m_pPlatformData; }

  FXFT_Library m_FTLibrary;

 private:
  CFX_GEModule(const char** pUserFontPaths, CCodec_ModuleMgr* pCodecModule);
  ~CFX_GEModule();

  void InitPlatform();
  void DestroyPlatform();

  uint8_t m_GammaValue[256];
  CFX_FontCache* m_pFontCache;
  std::unique_ptr<CFX_FontMgr> m_pFontMgr;
  CCodec_ModuleMgr* const m_pCodecModule;
  void* m_pPlatformData;
  const char** m_pUserFontPaths;
};

struct FX_PATHPOINT {
  FX_FLOAT m_PointX;
  FX_FLOAT m_PointY;
  int m_Flag;
};

class CFX_ClipRgn {
 public:
  enum ClipType { RectI, MaskF };

  CFX_ClipRgn(int device_width, int device_height);
  explicit CFX_ClipRgn(const FX_RECT& rect);
  CFX_ClipRgn(const CFX_ClipRgn& src);
  ~CFX_ClipRgn();

  ClipType GetType() const { return m_Type; }
  const FX_RECT& GetBox() const { return m_Box; }
  CFX_DIBitmapRef GetMask() const { return m_Mask; }

  void Reset(const FX_RECT& rect);
  void IntersectRect(const FX_RECT& rect);
  void IntersectMaskF(int left, int top, CFX_DIBitmapRef Mask);

 protected:
  void IntersectMaskRect(FX_RECT rect, FX_RECT mask_box, CFX_DIBitmapRef Mask);

  ClipType m_Type;
  FX_RECT m_Box;
  CFX_DIBitmapRef m_Mask;
};

class CFX_PathData {
 public:
  CFX_PathData();
  CFX_PathData(const CFX_PathData& src);
  ~CFX_PathData();

  int GetPointCount() const { return m_PointCount; }
  int GetFlag(int index) const { return m_pPoints[index].m_Flag; }
  FX_FLOAT GetPointX(int index) const { return m_pPoints[index].m_PointX; }
  FX_FLOAT GetPointY(int index) const { return m_pPoints[index].m_PointY; }
  FX_PATHPOINT* GetPoints() const { return m_pPoints; }

  void SetPointCount(int nPoints);
  void AllocPointCount(int nPoints);
  void AddPointCount(int addPoints);
  CFX_FloatRect GetBoundingBox() const;
  CFX_FloatRect GetBoundingBox(FX_FLOAT line_width, FX_FLOAT miter_limit) const;
  void Transform(const CFX_Matrix* pMatrix);
  FX_BOOL IsRect() const;
  FX_BOOL GetZeroAreaPath(CFX_PathData& NewPath,
                          CFX_Matrix* pMatrix,
                          FX_BOOL& bThin,
                          FX_BOOL bAdjust) const;
  FX_BOOL IsRect(const CFX_Matrix* pMatrix, CFX_FloatRect* rect) const;
  void Append(const CFX_PathData* pSrc, const CFX_Matrix* pMatrix);
  void AppendRect(FX_FLOAT left, FX_FLOAT bottom, FX_FLOAT right, FX_FLOAT top);
  void SetPoint(int index, FX_FLOAT x, FX_FLOAT y, int flag);
  void TrimPoints(int nPoints);
  void Copy(const CFX_PathData& src);

 protected:
  friend class CPDF_Path;

  int m_PointCount;
  FX_PATHPOINT* m_pPoints;
  int m_AllocCount;
};

class CFX_GraphStateData {
 public:
  enum LineCap { LineCapButt = 0, LineCapRound = 1, LineCapSquare = 2 };

  CFX_GraphStateData();
  CFX_GraphStateData(const CFX_GraphStateData& src);
  ~CFX_GraphStateData();

  void Copy(const CFX_GraphStateData& src);
  void SetDashCount(int count);

  LineCap m_LineCap;
  int m_DashCount;
  FX_FLOAT* m_DashArray;
  FX_FLOAT m_DashPhase;

  enum LineJoin {
    LineJoinMiter = 0,
    LineJoinRound = 1,
    LineJoinBevel = 2,
  };
  LineJoin m_LineJoin;
  FX_FLOAT m_MiterLimit;
  FX_FLOAT m_LineWidth;
};

#define FXDC_DEVICE_CLASS 1
#define FXDC_PIXEL_WIDTH 2
#define FXDC_PIXEL_HEIGHT 3
#define FXDC_BITS_PIXEL 4
#define FXDC_HORZ_SIZE 5
#define FXDC_VERT_SIZE 6
#define FXDC_RENDER_CAPS 7
#define FXDC_DISPLAY 1
#define FXDC_PRINTER 2

#define FXPT_CLOSEFIGURE 0x01
#define FXPT_LINETO 0x02
#define FXPT_BEZIERTO 0x04
#define FXPT_MOVETO 0x06
#define FXPT_TYPE 0x06

#define FXRC_GET_BITS 0x01
#define FXRC_BIT_MASK 0x02
#define FXRC_ALPHA_MASK 0x04
#define FXRC_ALPHA_PATH 0x10
#define FXRC_ALPHA_IMAGE 0x20
#define FXRC_ALPHA_OUTPUT 0x40
#define FXRC_BLEND_MODE 0x80
#define FXRC_SOFT_CLIP 0x100
#define FXRC_CMYK_OUTPUT 0x200
#define FXRC_BITMASK_OUTPUT 0x400
#define FXRC_BYTEMASK_OUTPUT 0x800
#define FXRENDER_IMAGE_LOSSY 0x1000
#define FXRC_FILLSTROKE_PATH 0x2000
#define FXRC_SHADING 0x4000

#define FXFILL_ALTERNATE 1
#define FXFILL_WINDING 2
#define FXFILL_FULLCOVER 4
#define FXFILL_RECT_AA 8
#define FX_FILL_STROKE 16
#define FX_STROKE_ADJUST 32
#define FX_STROKE_TEXT_MODE 64
#define FX_FILL_TEXT_MODE 128
#define FX_ZEROAREA_FILL 256
#define FXFILL_NOPATHSMOOTH 512

#define FXTEXT_CLEARTYPE 0x01
#define FXTEXT_BGR_STRIPE 0x02
#define FXTEXT_PRINTGRAPHICTEXT 0x04
#define FXTEXT_NO_NATIVETEXT 0x08
#define FXTEXT_PRINTIMAGETEXT 0x10
#define FXTEXT_NOSMOOTH 0x20

struct FXTEXT_CHARPOS {
  uint32_t m_GlyphIndex;
  FX_FLOAT m_OriginX;
  FX_FLOAT m_OriginY;
  int m_FontCharWidth;
  FX_BOOL m_bGlyphAdjust;
  FX_FLOAT m_AdjustMatrix[4];
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  uint32_t m_ExtGID;
#endif
  bool m_bFontStyle;
};

class CFX_RenderDevice {
 public:
  CFX_RenderDevice();
  virtual ~CFX_RenderDevice();

  void Flush();
  void SetDeviceDriver(IFX_RenderDeviceDriver* pDriver);
  IFX_RenderDeviceDriver* GetDeviceDriver() const { return m_pDeviceDriver; }

  FX_BOOL StartRendering();
  void EndRendering();
  void SaveState();
  void RestoreState(bool bKeepSaved);

  int GetWidth() const { return m_Width; }
  int GetHeight() const { return m_Height; }
  int GetDeviceClass() const { return m_DeviceClass; }
  int GetBPP() const { return m_bpp; }
  int GetRenderCaps() const { return m_RenderCaps; }
  int GetDeviceCaps(int id) const;
  CFX_Matrix GetCTM() const;
  CFX_DIBitmap* GetBitmap() const { return m_pBitmap; }
  void SetBitmap(CFX_DIBitmap* pBitmap) { m_pBitmap = pBitmap; }
  FX_BOOL CreateCompatibleBitmap(CFX_DIBitmap* pDIB,
                                 int width,
                                 int height) const;
  const FX_RECT& GetClipBox() const { return m_ClipBox; }
  FX_BOOL SetClip_PathFill(const CFX_PathData* pPathData,
                           const CFX_Matrix* pObject2Device,
                           int fill_mode);
  FX_BOOL SetClip_Rect(const FX_RECT& pRect);
  FX_BOOL SetClip_PathStroke(const CFX_PathData* pPathData,
                             const CFX_Matrix* pObject2Device,
                             const CFX_GraphStateData* pGraphState);
  FX_BOOL DrawPath(const CFX_PathData* pPathData,
                   const CFX_Matrix* pObject2Device,
                   const CFX_GraphStateData* pGraphState,
                   uint32_t fill_color,
                   uint32_t stroke_color,
                   int fill_mode) {
    return DrawPathWithBlend(pPathData, pObject2Device, pGraphState, fill_color,
                             stroke_color, fill_mode, FXDIB_BLEND_NORMAL);
  }
  FX_BOOL DrawPathWithBlend(const CFX_PathData* pPathData,
                            const CFX_Matrix* pObject2Device,
                            const CFX_GraphStateData* pGraphState,
                            uint32_t fill_color,
                            uint32_t stroke_color,
                            int fill_mode,
                            int blend_type);
  FX_BOOL SetPixel(int x, int y, uint32_t color);
  FX_BOOL FillRect(const FX_RECT* pRect, uint32_t color) {
    return FillRectWithBlend(pRect, color, FXDIB_BLEND_NORMAL);
  }
  FX_BOOL FillRectWithBlend(const FX_RECT* pRect,
                            uint32_t color,
                            int blend_type);
  FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           uint32_t color) {
    return DrawCosmeticLineWithFillModeAndBlend(x1, y1, x2, y2, color, 0,
                                                FXDIB_BLEND_NORMAL);
  }
  FX_BOOL DrawCosmeticLineWithFillModeAndBlend(FX_FLOAT x1,
                                               FX_FLOAT y1,
                                               FX_FLOAT x2,
                                               FX_FLOAT y2,
                                               uint32_t color,
                                               int fill_mode,
                                               int blend_type);

  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap, int left, int top);
  CFX_DIBitmap* GetBackDrop();
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap, int left, int top) {
    return SetDIBitsWithBlend(pBitmap, left, top, FXDIB_BLEND_NORMAL);
  }
  FX_BOOL SetDIBitsWithBlend(const CFX_DIBSource* pBitmap,
                             int left,
                             int top,
                             int blend_type);
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        int left,
                        int top,
                        int dest_width,
                        int dest_height) {
    return StretchDIBitsWithFlagsAndBlend(pBitmap, left, top, dest_width,
                                          dest_height, 0, FXDIB_BLEND_NORMAL);
  }
  FX_BOOL StretchDIBitsWithFlagsAndBlend(const CFX_DIBSource* pBitmap,
                                         int left,
                                         int top,
                                         int dest_width,
                                         int dest_height,
                                         uint32_t flags,
                                         int blend_type);
  FX_BOOL SetBitMask(const CFX_DIBSource* pBitmap,
                     int left,
                     int top,
                     uint32_t color);
  FX_BOOL StretchBitMask(const CFX_DIBSource* pBitmap,
                         int left,
                         int top,
                         int dest_width,
                         int dest_height,
                         uint32_t color);
  FX_BOOL StretchBitMaskWithFlags(const CFX_DIBSource* pBitmap,
                                  int left,
                                  int top,
                                  int dest_width,
                                  int dest_height,
                                  uint32_t color,
                                  uint32_t flags);
  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t flags,
                      void*& handle) {
    return StartDIBitsWithBlend(pBitmap, bitmap_alpha, color, pMatrix, flags,
                                handle, FXDIB_BLEND_NORMAL);
  }
  FX_BOOL StartDIBitsWithBlend(const CFX_DIBSource* pBitmap,
                               int bitmap_alpha,
                               uint32_t color,
                               const CFX_Matrix* pMatrix,
                               uint32_t flags,
                               void*& handle,
                               int blend_type);
  FX_BOOL ContinueDIBits(void* handle, IFX_Pause* pPause);
  void CancelDIBits(void* handle);

  FX_BOOL DrawNormalText(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         FX_FLOAT font_size,
                         const CFX_Matrix* pText2Device,
                         uint32_t fill_color,
                         uint32_t text_flags);
  FX_BOOL DrawTextPath(int nChars,
                       const FXTEXT_CHARPOS* pCharPos,
                       CFX_Font* pFont,
                       CFX_FontCache* pCache,
                       FX_FLOAT font_size,
                       const CFX_Matrix* pText2User,
                       const CFX_Matrix* pUser2Device,
                       const CFX_GraphStateData* pGraphState,
                       uint32_t fill_color,
                       uint32_t stroke_color,
                       CFX_PathData* pClippingPath) {
    return DrawTextPathWithFlags(nChars, pCharPos, pFont, pCache, font_size,
                                 pText2User, pUser2Device, pGraphState,
                                 fill_color, stroke_color, pClippingPath, 0);
  }
  FX_BOOL DrawTextPathWithFlags(int nChars,
                                const FXTEXT_CHARPOS* pCharPos,
                                CFX_Font* pFont,
                                CFX_FontCache* pCache,
                                FX_FLOAT font_size,
                                const CFX_Matrix* pText2User,
                                const CFX_Matrix* pUser2Device,
                                const CFX_GraphStateData* pGraphState,
                                uint32_t fill_color,
                                uint32_t stroke_color,
                                CFX_PathData* pClippingPath,
                                int nFlag);

#ifdef _SKIA_SUPPORT_
  virtual void DebugVerifyBitmapIsPreMultiplied() const;
#endif

 private:
  void InitDeviceInfo();
  void UpdateClipBox();
  FX_BOOL DrawFillStrokePath(const CFX_PathData* pPathData,
                             const CFX_Matrix* pObject2Device,
                             const CFX_GraphStateData* pGraphState,
                             uint32_t fill_color,
                             uint32_t stroke_color,
                             int fill_mode,
                             int blend_type);

  CFX_DIBitmap* m_pBitmap;
  int m_Width;
  int m_Height;
  int m_bpp;
  int m_RenderCaps;
  int m_DeviceClass;
  FX_RECT m_ClipBox;
  IFX_RenderDeviceDriver* m_pDeviceDriver;
};

class CFX_FxgeDevice : public CFX_RenderDevice {
 public:
  CFX_FxgeDevice();
  ~CFX_FxgeDevice() override;

  bool Attach(CFX_DIBitmap* pBitmap,
              bool bRgbByteOrder,
              CFX_DIBitmap* pOriDevice,
              bool bGroupKnockout);
  bool Create(int width,
              int height,
              FXDIB_Format format,
              CFX_DIBitmap* pOriDevice);

#ifdef _SKIA_SUPPORT_
  bool AttachRecorder(SkPictureRecorder* recorder);
  SkPictureRecorder* CreateRecorder(int size_x, int size_y);
  void DebugVerifyBitmapIsPreMultiplied() const override;
#endif

 protected:
  bool m_bOwnedBitmap;
};

#endif  // CORE_FXGE_INCLUDE_FX_GE_H_
