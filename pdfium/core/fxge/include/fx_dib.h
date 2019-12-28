// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_INCLUDE_FX_DIB_H_
#define CORE_FXGE_INCLUDE_FX_DIB_H_

#include <memory>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"

enum FXDIB_Format {
  FXDIB_Invalid = 0,
  FXDIB_1bppMask = 0x101,
  FXDIB_1bppRgb = 0x001,
  FXDIB_1bppCmyk = 0x401,
  FXDIB_8bppMask = 0x108,
  FXDIB_8bppRgb = 0x008,
  FXDIB_8bppRgba = 0x208,
  FXDIB_8bppCmyk = 0x408,
  FXDIB_8bppCmyka = 0x608,
  FXDIB_Rgb = 0x018,
  FXDIB_Rgba = 0x218,
  FXDIB_Rgb32 = 0x020,
  FXDIB_Argb = 0x220,
  FXDIB_Cmyk = 0x420,
  FXDIB_Cmyka = 0x620,
};

enum FXDIB_Channel {
  FXDIB_Red = 1,
  FXDIB_Green,
  FXDIB_Blue,
  FXDIB_Cyan,
  FXDIB_Magenta,
  FXDIB_Yellow,
  FXDIB_Black,
  FXDIB_Alpha
};

#define FXDIB_DOWNSAMPLE 0x04
#define FXDIB_INTERPOL 0x20
#define FXDIB_BICUBIC_INTERPOL 0x80
#define FXDIB_NOSMOOTH 0x100
#define FXDIB_BLEND_NORMAL 0
#define FXDIB_BLEND_MULTIPLY 1
#define FXDIB_BLEND_SCREEN 2
#define FXDIB_BLEND_OVERLAY 3
#define FXDIB_BLEND_DARKEN 4
#define FXDIB_BLEND_LIGHTEN 5

#define FXDIB_BLEND_COLORDODGE 6
#define FXDIB_BLEND_COLORBURN 7
#define FXDIB_BLEND_HARDLIGHT 8
#define FXDIB_BLEND_SOFTLIGHT 9
#define FXDIB_BLEND_DIFFERENCE 10
#define FXDIB_BLEND_EXCLUSION 11
#define FXDIB_BLEND_NONSEPARABLE 21
#define FXDIB_BLEND_HUE 21
#define FXDIB_BLEND_SATURATION 22
#define FXDIB_BLEND_COLOR 23
#define FXDIB_BLEND_LUMINOSITY 24
#define FXDIB_BLEND_UNSUPPORTED -1
typedef uint32_t FX_ARGB;
typedef uint32_t FX_COLORREF;
typedef uint32_t FX_CMYK;
class CFX_ClipRgn;
class CFX_DIBSource;
class CFX_DIBitmap;
class CStretchEngine;

#define FXSYS_RGB(r, g, b) ((r) | ((g) << 8) | ((b) << 16))
#define FXSYS_GetRValue(rgb) ((rgb)&0xff)
#define FXSYS_GetGValue(rgb) (((rgb) >> 8) & 0xff)
#define FXSYS_GetBValue(rgb) (((rgb) >> 16) & 0xff)
#define FX_CCOLOR(val) (255 - (val))
#define FXSYS_CMYK(c, m, y, k) (((c) << 24) | ((m) << 16) | ((y) << 8) | (k))
#define FXSYS_GetCValue(cmyk) ((uint8_t)((cmyk) >> 24) & 0xff)
#define FXSYS_GetMValue(cmyk) ((uint8_t)((cmyk) >> 16) & 0xff)
#define FXSYS_GetYValue(cmyk) ((uint8_t)((cmyk) >> 8) & 0xff)
#define FXSYS_GetKValue(cmyk) ((uint8_t)(cmyk)&0xff)
void CmykDecode(FX_CMYK cmyk, int& c, int& m, int& y, int& k);
inline FX_CMYK CmykEncode(int c, int m, int y, int k) {
  return (c << 24) | (m << 16) | (y << 8) | k;
}
void ArgbDecode(FX_ARGB argb, int& a, int& r, int& g, int& b);
void ArgbDecode(FX_ARGB argb, int& a, FX_COLORREF& rgb);
inline FX_ARGB ArgbEncode(int a, int r, int g, int b) {
  return (a << 24) | (r << 16) | (g << 8) | b;
}
FX_ARGB ArgbEncode(int a, FX_COLORREF rgb);
#define FXARGB_A(argb) ((uint8_t)((argb) >> 24))
#define FXARGB_R(argb) ((uint8_t)((argb) >> 16))
#define FXARGB_G(argb) ((uint8_t)((argb) >> 8))
#define FXARGB_B(argb) ((uint8_t)(argb))
#define FXARGB_MAKE(a, r, g, b) \
  (((uint32_t)(a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define FXARGB_MUL_ALPHA(argb, alpha) \
  (((((argb) >> 24) * (alpha) / 255) << 24) | ((argb)&0xffffff))
#define FXRGB2GRAY(r, g, b) (((b)*11 + (g)*59 + (r)*30) / 100)
#define FXCMYK2GRAY(c, m, y, k)                                       \
  (((255 - (c)) * (255 - (k)) * 30 + (255 - (m)) * (255 - (k)) * 59 + \
    (255 - (y)) * (255 - (k)) * 11) /                                 \
   25500)
#define FXDIB_ALPHA_MERGE(backdrop, source, source_alpha) \
  (((backdrop) * (255 - (source_alpha)) + (source) * (source_alpha)) / 255)
#define FXDIB_ALPHA_UNION(dest, src) ((dest) + (src) - (dest) * (src) / 255)
#define FXCMYK_GETDIB(p)                                    \
  ((((uint8_t*)(p))[0] << 24 | (((uint8_t*)(p))[1] << 16) | \
    (((uint8_t*)(p))[2] << 8) | ((uint8_t*)(p))[3]))
#define FXCMYK_SETDIB(p, cmyk)  ((uint8_t*)(p))[0] = (uint8_t)((cmyk) >> 24), \
        ((uint8_t*)(p))[1] = (uint8_t)((cmyk) >> 16), \
                              ((uint8_t*)(p))[2] = (uint8_t)((cmyk) >> 8), \
                                      ((uint8_t*)(p))[3] = (uint8_t)(cmyk))
#define FXARGB_GETDIB(p)                              \
  ((((uint8_t*)(p))[0]) | (((uint8_t*)(p))[1] << 8) | \
   (((uint8_t*)(p))[2] << 16) | (((uint8_t*)(p))[3] << 24))
#define FXARGB_SETDIB(p, argb)                  \
  ((uint8_t*)(p))[0] = (uint8_t)(argb),         \
  ((uint8_t*)(p))[1] = (uint8_t)((argb) >> 8),  \
  ((uint8_t*)(p))[2] = (uint8_t)((argb) >> 16), \
  ((uint8_t*)(p))[3] = (uint8_t)((argb) >> 24)
#define FXARGB_COPY(dest, src)                      \
  *(uint8_t*)(dest) = *(uint8_t*)(src),             \
  *((uint8_t*)(dest) + 1) = *((uint8_t*)(src) + 1), \
  *((uint8_t*)(dest) + 2) = *((uint8_t*)(src) + 2), \
  *((uint8_t*)(dest) + 3) = *((uint8_t*)(src) + 3)
#define FXCMYK_COPY(dest, src)                      \
  *(uint8_t*)(dest) = *(uint8_t*)(src),             \
  *((uint8_t*)(dest) + 1) = *((uint8_t*)(src) + 1), \
  *((uint8_t*)(dest) + 2) = *((uint8_t*)(src) + 2), \
  *((uint8_t*)(dest) + 3) = *((uint8_t*)(src) + 3)
#define FXARGB_SETRGBORDERDIB(p, argb)          \
  ((uint8_t*)(p))[3] = (uint8_t)(argb >> 24),   \
  ((uint8_t*)(p))[0] = (uint8_t)((argb) >> 16), \
  ((uint8_t*)(p))[1] = (uint8_t)((argb) >> 8),  \
  ((uint8_t*)(p))[2] = (uint8_t)(argb)
#define FXARGB_GETRGBORDERDIB(p)                     \
  (((uint8_t*)(p))[2]) | (((uint8_t*)(p))[1] << 8) | \
      (((uint8_t*)(p))[0] << 16) | (((uint8_t*)(p))[3] << 24)
#define FXARGB_RGBORDERCOPY(dest, src)                                   \
  *((uint8_t*)(dest) + 3) = *((uint8_t*)(src) + 3),                      \
                       *(uint8_t*)(dest) = *((uint8_t*)(src) + 2),       \
                       *((uint8_t*)(dest) + 1) = *((uint8_t*)(src) + 1), \
                       *((uint8_t*)(dest) + 2) = *((uint8_t*)(src))
#define FXARGB_TODIB(argb) (argb)
#define FXCMYK_TODIB(cmyk)                                    \
  ((uint8_t)((cmyk) >> 24) | ((uint8_t)((cmyk) >> 16)) << 8 | \
   ((uint8_t)((cmyk) >> 8)) << 16 | ((uint8_t)(cmyk) << 24))
#define FXARGB_TOBGRORDERDIB(argb)                       \
  ((uint8_t)(argb >> 16) | ((uint8_t)(argb >> 8)) << 8 | \
   ((uint8_t)(argb)) << 16 | ((uint8_t)(argb >> 24) << 24))
#define FXGETFLAG_COLORTYPE(flag) (uint8_t)((flag) >> 8)
#define FXGETFLAG_ALPHA_FILL(flag) (uint8_t)(flag)

FX_BOOL ConvertBuffer(FXDIB_Format dest_format,
                      uint8_t* dest_buf,
                      int dest_pitch,
                      int width,
                      int height,
                      const CFX_DIBSource* pSrcBitmap,
                      int src_left,
                      int src_top,
                      uint32_t*& pal);

class CFX_DIBSource {
 public:
  virtual ~CFX_DIBSource();

  int GetWidth() const { return m_Width; }
  int GetHeight() const { return m_Height; }

  FXDIB_Format GetFormat() const {
    return (FXDIB_Format)(m_AlphaFlag * 0x100 + m_bpp);
  }
  uint32_t GetPitch() const { return m_Pitch; }
  uint32_t* GetPalette() const { return m_pPalette; }

  virtual uint8_t* GetBuffer() const;
  virtual const uint8_t* GetScanline(int line) const = 0;
  virtual FX_BOOL SkipToScanline(int line, IFX_Pause* pPause) const;
  virtual void DownSampleScanline(int line,
                                  uint8_t* dest_scan,
                                  int dest_bpp,
                                  int dest_width,
                                  FX_BOOL bFlipX,
                                  int clip_left,
                                  int clip_width) const = 0;

  int GetBPP() const { return m_bpp; }

  // TODO(thestig): Investigate this. Given the possible values of FXDIB_Format,
  // it feels as though this should be implemented as !!(m_AlphaFlag & 1) and
  // IsOpaqueImage() below should never be able to return TRUE.
  bool IsAlphaMask() const { return m_AlphaFlag == 1; }
  bool HasAlpha() const { return !!(m_AlphaFlag & 2); }
  bool IsOpaqueImage() const { return !(m_AlphaFlag & 3); }
  bool IsCmykImage() const { return !!(m_AlphaFlag & 4); }

  int GetPaletteSize() const {
    return IsAlphaMask() ? 0 : (m_bpp == 1 ? 2 : (m_bpp == 8 ? 256 : 0));
  }

  uint32_t GetPaletteEntry(int index) const;

  void SetPaletteEntry(int index, uint32_t color);
  uint32_t GetPaletteArgb(int index) const { return GetPaletteEntry(index); }
  void SetPaletteArgb(int index, uint32_t color) {
    SetPaletteEntry(index, color);
  }

  void CopyPalette(const uint32_t* pSrcPal, uint32_t size = 256);

  CFX_DIBitmap* Clone(const FX_RECT* pClip = nullptr) const;
  CFX_DIBitmap* CloneConvert(FXDIB_Format format) const;

  CFX_DIBitmap* StretchTo(int dest_width,
                          int dest_height,
                          uint32_t flags = 0,
                          const FX_RECT* pClip = nullptr) const;
  CFX_DIBitmap* TransformTo(const CFX_Matrix* pMatrix,
                            int& left,
                            int& top,
                            uint32_t flags = 0,
                            const FX_RECT* pClip = nullptr) const;

  CFX_DIBitmap* GetAlphaMask(const FX_RECT* pClip = nullptr) const;
  FX_BOOL CopyAlphaMask(const CFX_DIBSource* pAlphaMask,
                        const FX_RECT* pClip = nullptr);

  CFX_DIBitmap* SwapXY(FX_BOOL bXFlip,
                       FX_BOOL bYFlip,
                       const FX_RECT* pClip = nullptr) const;

  CFX_DIBitmap* FlipImage(FX_BOOL bXFlip, FX_BOOL bYFlip) const;

  void GetOverlapRect(int& dest_left,
                      int& dest_top,
                      int& width,
                      int& height,
                      int src_width,
                      int src_height,
                      int& src_left,
                      int& src_top,
                      const CFX_ClipRgn* pClipRgn);

#if defined _SKIA_SUPPORT_
  void DebugVerifyBitmapIsPreMultiplied(void* buffer = nullptr) const;
#endif

  CFX_DIBitmap* m_pAlphaMask;

 protected:
  CFX_DIBSource();

  int m_Width;
  int m_Height;
  int m_bpp;
  uint32_t m_AlphaFlag;
  uint32_t m_Pitch;
  uint32_t* m_pPalette;

  void BuildPalette();
  FX_BOOL BuildAlphaMask();
  int FindPalette(uint32_t color) const;
  void GetPalette(uint32_t* pal, int alpha) const;
};

class CFX_DIBitmap : public CFX_DIBSource {
 public:
  CFX_DIBitmap();
  explicit CFX_DIBitmap(const CFX_DIBitmap& src);
  ~CFX_DIBitmap() override;

  FX_BOOL Create(int width,
                 int height,
                 FXDIB_Format format,
                 uint8_t* pBuffer = nullptr,
                 int pitch = 0);

  FX_BOOL Copy(const CFX_DIBSource* pSrc);

  // CFX_DIBSource
  uint8_t* GetBuffer() const override;
  const uint8_t* GetScanline(int line) const override;
  void DownSampleScanline(int line,
                          uint8_t* dest_scan,
                          int dest_bpp,
                          int dest_width,
                          FX_BOOL bFlipX,
                          int clip_left,
                          int clip_width) const override;

  void TakeOver(CFX_DIBitmap* pSrcBitmap);

  FX_BOOL ConvertFormat(FXDIB_Format format);

  void Clear(uint32_t color);

  uint32_t GetPixel(int x, int y) const;

  void SetPixel(int x, int y, uint32_t color);

  FX_BOOL LoadChannel(FXDIB_Channel destChannel,
                      const CFX_DIBSource* pSrcBitmap,
                      FXDIB_Channel srcChannel);

  FX_BOOL LoadChannel(FXDIB_Channel destChannel, int value);

  FX_BOOL MultiplyAlpha(int alpha);

  FX_BOOL MultiplyAlpha(const CFX_DIBSource* pAlphaMask);

  FX_BOOL TransferBitmap(int dest_left,
                         int dest_top,
                         int width,
                         int height,
                         const CFX_DIBSource* pSrcBitmap,
                         int src_left,
                         int src_top);

  FX_BOOL CompositeBitmap(int dest_left,
                          int dest_top,
                          int width,
                          int height,
                          const CFX_DIBSource* pSrcBitmap,
                          int src_left,
                          int src_top,
                          int blend_type = FXDIB_BLEND_NORMAL,
                          const CFX_ClipRgn* pClipRgn = nullptr,
                          FX_BOOL bRgbByteOrder = FALSE,
                          void* pIccTransform = nullptr);

  FX_BOOL TransferMask(int dest_left,
                       int dest_top,
                       int width,
                       int height,
                       const CFX_DIBSource* pMask,
                       uint32_t color,
                       int src_left,
                       int src_top,
                       int alpha_flag = 0,
                       void* pIccTransform = nullptr);

  FX_BOOL CompositeMask(int dest_left,
                        int dest_top,
                        int width,
                        int height,
                        const CFX_DIBSource* pMask,
                        uint32_t color,
                        int src_left,
                        int src_top,
                        int blend_type = FXDIB_BLEND_NORMAL,
                        const CFX_ClipRgn* pClipRgn = nullptr,
                        FX_BOOL bRgbByteOrder = FALSE,
                        int alpha_flag = 0,
                        void* pIccTransform = nullptr);

  FX_BOOL CompositeRect(int dest_left,
                        int dest_top,
                        int width,
                        int height,
                        uint32_t color,
                        int alpha_flag = 0,
                        void* pIccTransform = nullptr);

  FX_BOOL ConvertColorScale(uint32_t forecolor, uint32_t backcolor);

 protected:
  FX_BOOL GetGrayData(void* pIccTransform = nullptr);

  uint8_t* m_pBuffer;
  FX_BOOL m_bExtBuf;
};

class CFX_DIBExtractor {
 public:
  explicit CFX_DIBExtractor(const CFX_DIBSource* pSrc);
  ~CFX_DIBExtractor();

  operator CFX_DIBitmap*() { return m_pBitmap; }

 private:
  CFX_DIBitmap* m_pBitmap;
};

typedef CFX_CountRef<CFX_DIBitmap> CFX_DIBitmapRef;
class CFX_FilteredDIB : public CFX_DIBSource {
 public:
  CFX_FilteredDIB();
  ~CFX_FilteredDIB() override;

  void LoadSrc(const CFX_DIBSource* pSrc, FX_BOOL bAutoDropSrc = FALSE);

  virtual FXDIB_Format GetDestFormat() = 0;

  virtual uint32_t* GetDestPalette() = 0;

  virtual void TranslateScanline(uint8_t* dest_buf,
                                 const uint8_t* src_buf) const = 0;

  virtual void TranslateDownSamples(uint8_t* dest_buf,
                                    const uint8_t* src_buf,
                                    int pixels,
                                    int Bpp) const = 0;

 protected:
  // CFX_DIBSource
  const uint8_t* GetScanline(int line) const override;
  void DownSampleScanline(int line,
                          uint8_t* dest_scan,
                          int dest_bpp,
                          int dest_width,
                          FX_BOOL bFlipX,
                          int clip_left,
                          int clip_width) const override;

  const CFX_DIBSource* m_pSrc;

  FX_BOOL m_bAutoDropSrc;

  uint8_t* m_pScanline;
};

class IFX_ScanlineComposer {
 public:
  virtual ~IFX_ScanlineComposer() {}

  virtual void ComposeScanline(int line,
                               const uint8_t* scanline,
                               const uint8_t* scan_extra_alpha = nullptr) = 0;

  virtual FX_BOOL SetInfo(int width,
                          int height,
                          FXDIB_Format src_format,
                          uint32_t* pSrcPalette) = 0;
};
class CFX_ScanlineCompositor {
 public:
  CFX_ScanlineCompositor();

  ~CFX_ScanlineCompositor();

  FX_BOOL Init(FXDIB_Format dest_format,
               FXDIB_Format src_format,
               int32_t width,
               uint32_t* pSrcPalette,
               uint32_t mask_color,
               int blend_type,
               FX_BOOL bClip,
               FX_BOOL bRgbByteOrder = FALSE,
               int alpha_flag = 0,
               void* pIccTransform = nullptr);

  void CompositeRgbBitmapLine(uint8_t* dest_scan,
                              const uint8_t* src_scan,
                              int width,
                              const uint8_t* clip_scan,
                              const uint8_t* src_extra_alpha = nullptr,
                              uint8_t* dst_extra_alpha = nullptr);

  void CompositePalBitmapLine(uint8_t* dest_scan,
                              const uint8_t* src_scan,
                              int src_left,
                              int width,
                              const uint8_t* clip_scan,
                              const uint8_t* src_extra_alpha = nullptr,
                              uint8_t* dst_extra_alpha = nullptr);

  void CompositeByteMaskLine(uint8_t* dest_scan,
                             const uint8_t* src_scan,
                             int width,
                             const uint8_t* clip_scan,
                             uint8_t* dst_extra_alpha = nullptr);

  void CompositeBitMaskLine(uint8_t* dest_scan,
                            const uint8_t* src_scan,
                            int src_left,
                            int width,
                            const uint8_t* clip_scan,
                            uint8_t* dst_extra_alpha = nullptr);

 protected:
  int m_Transparency;
  FXDIB_Format m_SrcFormat, m_DestFormat;
  uint32_t* m_pSrcPalette;

  int m_MaskAlpha, m_MaskRed, m_MaskGreen, m_MaskBlue, m_MaskBlack;
  int m_BlendType;
  void* m_pIccTransform;
  uint8_t* m_pCacheScanline;
  int m_CacheSize;
  FX_BOOL m_bRgbByteOrder;
};

class CFX_BitmapComposer : public IFX_ScanlineComposer {
 public:
  CFX_BitmapComposer();
  ~CFX_BitmapComposer() override;

  void Compose(CFX_DIBitmap* pDest,
               const CFX_ClipRgn* pClipRgn,
               int bitmap_alpha,
               uint32_t mask_color,
               FX_RECT& dest_rect,
               FX_BOOL bVertical,
               FX_BOOL bFlipX,
               FX_BOOL bFlipY,
               FX_BOOL bRgbByteOrder = FALSE,
               int alpha_flag = 0,
               void* pIccTransform = nullptr,
               int blend_type = FXDIB_BLEND_NORMAL);

  // IFX_ScanlineComposer
  FX_BOOL SetInfo(int width,
                  int height,
                  FXDIB_Format src_format,
                  uint32_t* pSrcPalette) override;

  void ComposeScanline(int line,
                       const uint8_t* scanline,
                       const uint8_t* scan_extra_alpha) override;

 protected:
  void DoCompose(uint8_t* dest_scan,
                 const uint8_t* src_scan,
                 int dest_width,
                 const uint8_t* clip_scan,
                 const uint8_t* src_extra_alpha = nullptr,
                 uint8_t* dst_extra_alpha = nullptr);
  CFX_DIBitmap* m_pBitmap;
  const CFX_ClipRgn* m_pClipRgn;
  FXDIB_Format m_SrcFormat;
  int m_DestLeft, m_DestTop, m_DestWidth, m_DestHeight, m_BitmapAlpha;
  uint32_t m_MaskColor;
  const CFX_DIBitmap* m_pClipMask;
  CFX_ScanlineCompositor m_Compositor;
  FX_BOOL m_bVertical, m_bFlipX, m_bFlipY;
  int m_AlphaFlag;
  void* m_pIccTransform;
  FX_BOOL m_bRgbByteOrder;
  int m_BlendType;
  void ComposeScanlineV(int line,
                        const uint8_t* scanline,
                        const uint8_t* scan_extra_alpha = nullptr);
  uint8_t* m_pScanlineV;
  uint8_t* m_pClipScanV;
  uint8_t* m_pAddClipScan;
  uint8_t* m_pScanlineAlphaV;
};

class CFX_BitmapStorer : public IFX_ScanlineComposer {
 public:
  CFX_BitmapStorer();
  ~CFX_BitmapStorer() override;

  // IFX_ScanlineComposer
  void ComposeScanline(int line,
                       const uint8_t* scanline,
                       const uint8_t* scan_extra_alpha) override;
  FX_BOOL SetInfo(int width,
                  int height,
                  FXDIB_Format src_format,
                  uint32_t* pSrcPalette) override;

  CFX_DIBitmap* GetBitmap() { return m_pBitmap.get(); }
  std::unique_ptr<CFX_DIBitmap> Detach();
  void Replace(std::unique_ptr<CFX_DIBitmap> pBitmap);

 private:
  std::unique_ptr<CFX_DIBitmap> m_pBitmap;
};

class CFX_ImageStretcher {
 public:
  CFX_ImageStretcher(IFX_ScanlineComposer* pDest,
                     const CFX_DIBSource* pSource,
                     int dest_width,
                     int dest_height,
                     const FX_RECT& bitmap_rect,
                     uint32_t flags);
  ~CFX_ImageStretcher();

  FX_BOOL Start();
  FX_BOOL Continue(IFX_Pause* pPause);

  const CFX_DIBSource* source() { return m_pSource; }

 private:
  FX_BOOL StartQuickStretch();
  FX_BOOL StartStretch();
  FX_BOOL ContinueQuickStretch(IFX_Pause* pPause);
  FX_BOOL ContinueStretch(IFX_Pause* pPause);

  IFX_ScanlineComposer* const m_pDest;
  const CFX_DIBSource* const m_pSource;
  std::unique_ptr<CStretchEngine> m_pStretchEngine;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pScanline;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pMaskScanline;
  const uint32_t m_Flags;
  FX_BOOL m_bFlipX;
  FX_BOOL m_bFlipY;
  int m_DestWidth;
  int m_DestHeight;
  FX_RECT m_ClipRect;
  const FXDIB_Format m_DestFormat;
  const int m_DestBPP;
  int m_LineIndex;
};

class CFX_ImageTransformer {
 public:
  CFX_ImageTransformer(const CFX_DIBSource* pSrc,
                       const CFX_Matrix* pMatrix,
                       int flags,
                       const FX_RECT* pClip);
  ~CFX_ImageTransformer();

  FX_BOOL Start();
  FX_BOOL Continue(IFX_Pause* pPause);

  const FX_RECT& result() const { return m_result; }
  std::unique_ptr<CFX_DIBitmap> DetachBitmap();

 private:
  const CFX_DIBSource* const m_pSrc;
  const CFX_Matrix* const m_pMatrix;
  const FX_RECT* const m_pClip;
  FX_RECT m_StretchClip;
  FX_RECT m_result;
  CFX_Matrix m_dest2stretch;
  std::unique_ptr<CFX_ImageStretcher> m_Stretcher;
  CFX_BitmapStorer m_Storer;
  const uint32_t m_Flags;
  int m_Status;
};

class CFX_ImageRenderer {
 public:
  CFX_ImageRenderer();
  ~CFX_ImageRenderer();

  FX_BOOL Start(CFX_DIBitmap* pDevice,
                const CFX_ClipRgn* pClipRgn,
                const CFX_DIBSource* pSource,
                int bitmap_alpha,
                uint32_t mask_color,
                const CFX_Matrix* pMatrix,
                uint32_t dib_flags,
                FX_BOOL bRgbByteOrder = FALSE,
                int alpha_flag = 0,
                void* pIccTransform = nullptr,
                int blend_type = FXDIB_BLEND_NORMAL);

  FX_BOOL Continue(IFX_Pause* pPause);

 protected:
  CFX_DIBitmap* m_pDevice;
  const CFX_ClipRgn* m_pClipRgn;
  int m_BitmapAlpha;
  uint32_t m_MaskColor;
  CFX_Matrix m_Matrix;
  CFX_ImageTransformer* m_pTransformer;
  std::unique_ptr<CFX_ImageStretcher> m_Stretcher;
  CFX_BitmapComposer m_Composer;
  int m_Status;
  FX_RECT m_ClipBox;
  uint32_t m_Flags;
  int m_AlphaFlag;
  void* m_pIccTransform;
  FX_BOOL m_bRgbByteOrder;
  int m_BlendType;
};

#endif  // CORE_FXGE_INCLUDE_FX_DIB_H_
