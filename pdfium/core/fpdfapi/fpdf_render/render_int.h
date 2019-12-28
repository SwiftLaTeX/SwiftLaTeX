// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_RENDER_RENDER_INT_H_
#define CORE_FPDFAPI_FPDF_RENDER_RENDER_INT_H_

#include <map>
#include <memory>

#include "core/fpdfapi/fpdf_page/cpdf_countedobject.h"
#include "core/fpdfapi/fpdf_page/cpdf_graphicstates.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_clippath.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"

class CCodec_Jbig2Context;
class CCodec_ScanlineDecoder;
class CFX_FontCache;
class CFX_GlyphBitmap;
class CFX_ImageTransformer;
class CPDF_Color;
class CPDF_Dictionary;
class CPDF_Document;
class CPDF_Font;
class CPDF_FormObject;
class CPDF_ImageCacheEntry;
class CPDF_ImageLoaderHandle;
class CPDF_ImageObject;
class CPDF_ImageRenderer;
class CPDF_Object;
class CPDF_PageObject;
class CPDF_PageObjectHolder;
class CPDF_PageRenderCache;
class CPDF_PathObject;
class CPDF_RenderStatus;
class CPDF_ShadingObject;
class CPDF_ShadingPattern;
class CPDF_Stream;
class CPDF_TilingPattern;
class CPDF_TransferFunc;
class CPDF_Type3Cache;
class CPDF_Type3Char;
class CPDF_Type3Font;

#define TYPE3_MAX_BLUES 16

FX_BOOL IsAvailableMatrix(const CFX_Matrix& matrix);

class CPDF_Type3Glyphs {
 public:
  CPDF_Type3Glyphs();
  ~CPDF_Type3Glyphs();

  void AdjustBlue(FX_FLOAT top,
                  FX_FLOAT bottom,
                  int& top_line,
                  int& bottom_line);

  std::map<uint32_t, CFX_GlyphBitmap*> m_GlyphMap;
  int m_TopBlue[TYPE3_MAX_BLUES];
  int m_BottomBlue[TYPE3_MAX_BLUES];
  int m_TopBlueCount;
  int m_BottomBlueCount;
};

class CPDF_Type3Cache {
 public:
  explicit CPDF_Type3Cache(CPDF_Type3Font* pFont);
  ~CPDF_Type3Cache();

  CFX_GlyphBitmap* LoadGlyph(uint32_t charcode,
                             const CFX_Matrix* pMatrix,
                             FX_FLOAT retinaScaleX = 1.0f,
                             FX_FLOAT retinaScaleY = 1.0f);

 protected:
  CFX_GlyphBitmap* RenderGlyph(CPDF_Type3Glyphs* pSize,
                               uint32_t charcode,
                               const CFX_Matrix* pMatrix,
                               FX_FLOAT retinaScaleX = 1.0f,
                               FX_FLOAT retinaScaleY = 1.0f);
  CPDF_Type3Font* const m_pFont;
  std::map<CFX_ByteString, CPDF_Type3Glyphs*> m_SizeMap;
};

class CPDF_TransferFunc {
 public:
  explicit CPDF_TransferFunc(CPDF_Document* pDoc);

  FX_COLORREF TranslateColor(FX_COLORREF src) const;
  CFX_DIBSource* TranslateImage(const CFX_DIBSource* pSrc,
                                FX_BOOL bAutoDropSrc);

  CPDF_Document* const m_pPDFDoc;
  FX_BOOL m_bIdentity;
  uint8_t m_Samples[256 * 3];
};

class CPDF_DocRenderData {
 public:
  CPDF_DocRenderData(CPDF_Document* pPDFDoc = nullptr);
  ~CPDF_DocRenderData();
  CPDF_Type3Cache* GetCachedType3(CPDF_Type3Font* pFont);
  CPDF_TransferFunc* GetTransferFunc(CPDF_Object* pObj);
  CFX_FontCache* GetFontCache() { return m_pFontCache; }
  void Clear(FX_BOOL bRelease = FALSE);
  void ReleaseCachedType3(CPDF_Type3Font* pFont);
  void ReleaseTransferFunc(CPDF_Object* pObj);

 private:
  using CPDF_Type3CacheMap =
      std::map<CPDF_Font*, CPDF_CountedObject<CPDF_Type3Cache>*>;
  using CPDF_TransferFuncMap =
      std::map<CPDF_Object*, CPDF_CountedObject<CPDF_TransferFunc>*>;

  CPDF_Document* m_pPDFDoc;
  CFX_FontCache* m_pFontCache;
  CPDF_Type3CacheMap m_Type3FaceMap;
  CPDF_TransferFuncMap m_TransferFuncMap;
};

class CPDF_RenderStatus {
 public:
  CPDF_RenderStatus();
  ~CPDF_RenderStatus();

  FX_BOOL Initialize(class CPDF_RenderContext* pContext,
                     CFX_RenderDevice* pDevice,
                     const CFX_Matrix* pDeviceMatrix,
                     const CPDF_PageObject* pStopObj,
                     const CPDF_RenderStatus* pParentStatus,
                     const CPDF_GraphicStates* pInitialStates,
                     const CPDF_RenderOptions* pOptions,
                     int transparency,
                     FX_BOOL bDropObjects,
                     CPDF_Dictionary* pFormResource = nullptr,
                     FX_BOOL bStdCS = FALSE,
                     CPDF_Type3Char* pType3Char = nullptr,
                     FX_ARGB fill_color = 0,
                     uint32_t GroupFamily = 0,
                     FX_BOOL bLoadMask = FALSE);
  void RenderObjectList(const CPDF_PageObjectHolder* pObjectHolder,
                        const CFX_Matrix* pObj2Device);
  void RenderSingleObject(const CPDF_PageObject* pObj,
                          const CFX_Matrix* pObj2Device);
  FX_BOOL ContinueSingleObject(const CPDF_PageObject* pObj,
                               const CFX_Matrix* pObj2Device,
                               IFX_Pause* pPause);
  CPDF_RenderContext* GetContext() { return m_pContext; }

#if defined _SKIA_SUPPORT_
  void DebugVerifyDeviceIsPreMultiplied() const;
#endif

  CPDF_RenderOptions m_Options;
  CPDF_Dictionary* m_pFormResource;
  CPDF_Dictionary* m_pPageResource;
  CFX_ArrayTemplate<CPDF_Type3Font*> m_Type3FontCache;

 protected:
  friend class CPDF_ImageRenderer;
  friend class CPDF_RenderContext;

  void ProcessClipPath(CPDF_ClipPath ClipPath, const CFX_Matrix* pObj2Device);
  void DrawClipPath(CPDF_ClipPath ClipPath, const CFX_Matrix* pObj2Device);
  FX_BOOL ProcessTransparency(const CPDF_PageObject* PageObj,
                              const CFX_Matrix* pObj2Device);
  void ProcessObjectNoClip(const CPDF_PageObject* PageObj,
                           const CFX_Matrix* pObj2Device);
  void DrawObjWithBackground(const CPDF_PageObject* pObj,
                             const CFX_Matrix* pObj2Device);
  FX_BOOL DrawObjWithBlend(const CPDF_PageObject* pObj,
                           const CFX_Matrix* pObj2Device);
  FX_BOOL ProcessPath(const CPDF_PathObject* pPathObj,
                      const CFX_Matrix* pObj2Device);
  void ProcessPathPattern(const CPDF_PathObject* pPathObj,
                          const CFX_Matrix* pObj2Device,
                          int& filltype,
                          FX_BOOL& bStroke);
  void DrawPathWithPattern(const CPDF_PathObject* pPathObj,
                           const CFX_Matrix* pObj2Device,
                           CPDF_Color* pColor,
                           FX_BOOL bStroke);
  void DrawTilingPattern(CPDF_TilingPattern* pPattern,
                         const CPDF_PageObject* pPageObj,
                         const CFX_Matrix* pObj2Device,
                         FX_BOOL bStroke);
  void DrawShadingPattern(CPDF_ShadingPattern* pPattern,
                          const CPDF_PageObject* pPageObj,
                          const CFX_Matrix* pObj2Device,
                          FX_BOOL bStroke);
  FX_BOOL SelectClipPath(const CPDF_PathObject* pPathObj,
                         const CFX_Matrix* pObj2Device,
                         FX_BOOL bStroke);
  FX_BOOL ProcessImage(const CPDF_ImageObject* pImageObj,
                       const CFX_Matrix* pObj2Device);
  FX_BOOL OutputBitmapAlpha(CPDF_ImageObject* pImageObj,
                            const CFX_Matrix* pImage2Device);
  FX_BOOL OutputImage(CPDF_ImageObject* pImageObj,
                      const CFX_Matrix* pImage2Device);
  FX_BOOL OutputDIBSource(const CFX_DIBSource* pOutputBitmap,
                          FX_ARGB fill_argb,
                          int bitmap_alpha,
                          const CFX_Matrix* pImage2Device,
                          CPDF_ImageCacheEntry* pImageCache,
                          uint32_t flags);
  void CompositeDIBitmap(CFX_DIBitmap* pDIBitmap,
                         int left,
                         int top,
                         FX_ARGB mask_argb,
                         int bitmap_alpha,
                         int blend_mode,
                         int bIsolated);
  void ProcessShading(const CPDF_ShadingObject* pShadingObj,
                      const CFX_Matrix* pObj2Device);
  void DrawShading(CPDF_ShadingPattern* pPattern,
                   CFX_Matrix* pMatrix,
                   FX_RECT& clip_rect,
                   int alpha,
                   FX_BOOL bAlphaMode);
  FX_BOOL ProcessType3Text(const CPDF_TextObject* textobj,
                           const CFX_Matrix* pObj2Device);
  FX_BOOL ProcessText(const CPDF_TextObject* textobj,
                      const CFX_Matrix* pObj2Device,
                      CFX_PathData* pClippingPath);
  void DrawTextPathWithPattern(const CPDF_TextObject* textobj,
                               const CFX_Matrix* pObj2Device,
                               CPDF_Font* pFont,
                               FX_FLOAT font_size,
                               const CFX_Matrix* pTextMatrix,
                               FX_BOOL bFill,
                               FX_BOOL bStroke);
  FX_BOOL ProcessForm(const CPDF_FormObject* pFormObj,
                      const CFX_Matrix* pObj2Device);
  CFX_DIBitmap* GetBackdrop(const CPDF_PageObject* pObj,
                            const FX_RECT& rect,
                            int& left,
                            int& top,
                            FX_BOOL bBackAlphaRequired);
  CFX_DIBitmap* LoadSMask(CPDF_Dictionary* pSMaskDict,
                          FX_RECT* pClipRect,
                          const CFX_Matrix* pMatrix);
  void Init(CPDF_RenderContext* pParent);
  static class CPDF_Type3Cache* GetCachedType3(CPDF_Type3Font* pFont);
  static CPDF_GraphicStates* CloneObjStates(const CPDF_GraphicStates* pPathObj,
                                            FX_BOOL bStroke);
  CPDF_TransferFunc* GetTransferFunc(CPDF_Object* pObject) const;
  FX_ARGB GetFillArgb(const CPDF_PageObject* pObj,
                      FX_BOOL bType3 = FALSE) const;
  FX_ARGB GetStrokeArgb(const CPDF_PageObject* pObj) const;
  FX_BOOL GetObjectClippedRect(const CPDF_PageObject* pObj,
                               const CFX_Matrix* pObj2Device,
                               FX_BOOL bLogical,
                               FX_RECT& rect) const;
  void GetScaledMatrix(CFX_Matrix& matrix) const;

  static const int kRenderMaxRecursionDepth = 64;
  static int s_CurrentRecursionDepth;

  CPDF_RenderContext* m_pContext;
  FX_BOOL m_bStopped;
  CFX_RenderDevice* m_pDevice;
  CFX_Matrix m_DeviceMatrix;
  CPDF_ClipPath m_LastClipPath;
  const CPDF_PageObject* m_pCurObj;
  const CPDF_PageObject* m_pStopObj;
  CPDF_GraphicStates m_InitialStates;
  int m_HalftoneLimit;
  std::unique_ptr<CPDF_ImageRenderer> m_pImageRenderer;
  FX_BOOL m_bPrint;
  int m_Transparency;
  FX_BOOL m_bDropObjects;
  FX_BOOL m_bStdCS;
  uint32_t m_GroupFamily;
  FX_BOOL m_bLoadMask;
  CPDF_Type3Char* m_pType3Char;
  FX_ARGB m_T3FillColor;
  int m_curBlend;
};

class CPDF_ImageLoader {
 public:
  CPDF_ImageLoader()
      : m_pBitmap(nullptr),
        m_pMask(nullptr),
        m_MatteColor(0),
        m_bCached(FALSE),
        m_nDownsampleWidth(0),
        m_nDownsampleHeight(0) {}
  ~CPDF_ImageLoader();

  FX_BOOL Start(const CPDF_ImageObject* pImage,
                CPDF_PageRenderCache* pCache,
                std::unique_ptr<CPDF_ImageLoaderHandle>* pLoadHandle,
                FX_BOOL bStdCS = FALSE,
                uint32_t GroupFamily = 0,
                FX_BOOL bLoadMask = FALSE,
                CPDF_RenderStatus* pRenderStatus = nullptr,
                int32_t nDownsampleWidth = 0,
                int32_t nDownsampleHeight = 0);
  FX_BOOL Continue(CPDF_ImageLoaderHandle* LoadHandle, IFX_Pause* pPause);

  CFX_DIBSource* m_pBitmap;
  CFX_DIBSource* m_pMask;
  uint32_t m_MatteColor;
  FX_BOOL m_bCached;

 protected:
  int32_t m_nDownsampleWidth;
  int32_t m_nDownsampleHeight;
};
class CPDF_ImageLoaderHandle {
 public:
  CPDF_ImageLoaderHandle();
  ~CPDF_ImageLoaderHandle();

  FX_BOOL Start(CPDF_ImageLoader* pImageLoader,
                const CPDF_ImageObject* pImage,
                CPDF_PageRenderCache* pCache,
                FX_BOOL bStdCS = FALSE,
                uint32_t GroupFamily = 0,
                FX_BOOL bLoadMask = FALSE,
                CPDF_RenderStatus* pRenderStatus = nullptr,
                int32_t nDownsampleWidth = 0,
                int32_t nDownsampleHeight = 0);
  FX_BOOL Continue(IFX_Pause* pPause);

 protected:
  CPDF_ImageLoader* m_pImageLoader;
  CPDF_PageRenderCache* m_pCache;
  CPDF_ImageObject* m_pImage;
  int32_t m_nDownsampleWidth;
  int32_t m_nDownsampleHeight;
};

class CPDF_ImageRenderer {
 public:
  CPDF_ImageRenderer();
  ~CPDF_ImageRenderer();

  FX_BOOL Start(CPDF_RenderStatus* pStatus,
                const CPDF_PageObject* pObj,
                const CFX_Matrix* pObj2Device,
                FX_BOOL bStdCS,
                int blendType = FXDIB_BLEND_NORMAL);
  FX_BOOL Continue(IFX_Pause* pPause);

  FX_BOOL Start(CPDF_RenderStatus* pStatus,
                const CFX_DIBSource* pDIBSource,
                FX_ARGB bitmap_argb,
                int bitmap_alpha,
                const CFX_Matrix* pImage2Device,
                uint32_t flags,
                FX_BOOL bStdCS,
                int blendType = FXDIB_BLEND_NORMAL);

  FX_BOOL m_Result;

 protected:
  FX_BOOL StartBitmapAlpha();
  FX_BOOL StartDIBSource();
  FX_BOOL StartRenderDIBSource();
  FX_BOOL StartLoadDIBSource();
  FX_BOOL DrawMaskedImage();
  FX_BOOL DrawPatternImage(const CFX_Matrix* pObj2Device);

  CPDF_RenderStatus* m_pRenderStatus;
  const CPDF_ImageObject* m_pImageObject;
  int m_Status;
  const CFX_Matrix* m_pObj2Device;
  CFX_Matrix m_ImageMatrix;
  CPDF_ImageLoader m_Loader;
  const CFX_DIBSource* m_pDIBSource;
  std::unique_ptr<CFX_DIBitmap> m_pClone;
  int m_BitmapAlpha;
  FX_BOOL m_bPatternColor;
  CPDF_Pattern* m_pPattern;
  FX_ARGB m_FillArgb;
  uint32_t m_Flags;
  std::unique_ptr<CFX_ImageTransformer> m_pTransformer;
  void* m_DeviceHandle;
  std::unique_ptr<CPDF_ImageLoaderHandle> m_LoadHandle;
  FX_BOOL m_bStdCS;
  int m_BlendType;
};

class CPDF_ScaledRenderBuffer {
 public:
  CPDF_ScaledRenderBuffer();
  ~CPDF_ScaledRenderBuffer();

  FX_BOOL Initialize(CPDF_RenderContext* pContext,
                     CFX_RenderDevice* pDevice,
                     const FX_RECT& pRect,
                     const CPDF_PageObject* pObj,
                     const CPDF_RenderOptions* pOptions = nullptr,
                     int max_dpi = 0);
  CFX_RenderDevice* GetDevice() {
    return m_pBitmapDevice ? m_pBitmapDevice.get() : m_pDevice;
  }
  CFX_Matrix* GetMatrix() { return &m_Matrix; }
  void OutputToDevice();

 private:
  CFX_RenderDevice* m_pDevice;
  CPDF_RenderContext* m_pContext;
  FX_RECT m_Rect;
  const CPDF_PageObject* m_pObject;
  std::unique_ptr<CFX_FxgeDevice> m_pBitmapDevice;
  CFX_Matrix m_Matrix;
};

class CPDF_DeviceBuffer {
 public:
  CPDF_DeviceBuffer();
  ~CPDF_DeviceBuffer();
  FX_BOOL Initialize(CPDF_RenderContext* pContext,
                     CFX_RenderDevice* pDevice,
                     FX_RECT* pRect,
                     const CPDF_PageObject* pObj,
                     int max_dpi = 0);
  void OutputToDevice();
  CFX_DIBitmap* GetBitmap() const { return m_pBitmap.get(); }
  const CFX_Matrix* GetMatrix() const { return &m_Matrix; }

 private:
  CFX_RenderDevice* m_pDevice;
  CPDF_RenderContext* m_pContext;
  FX_RECT m_Rect;
  const CPDF_PageObject* m_pObject;
  std::unique_ptr<CFX_DIBitmap> m_pBitmap;
  CFX_Matrix m_Matrix;
};

class CPDF_ImageCacheEntry {
 public:
  CPDF_ImageCacheEntry(CPDF_Document* pDoc, CPDF_Stream* pStream);
  ~CPDF_ImageCacheEntry();

  void Reset(const CFX_DIBitmap* pBitmap);
  FX_BOOL GetCachedBitmap(CFX_DIBSource*& pBitmap,
                          CFX_DIBSource*& pMask,
                          uint32_t& MatteColor,
                          CPDF_Dictionary* pPageResources,
                          FX_BOOL bStdCS = FALSE,
                          uint32_t GroupFamily = 0,
                          FX_BOOL bLoadMask = FALSE,
                          CPDF_RenderStatus* pRenderStatus = nullptr,
                          int32_t downsampleWidth = 0,
                          int32_t downsampleHeight = 0);
  uint32_t EstimateSize() const { return m_dwCacheSize; }
  uint32_t GetTimeCount() const { return m_dwTimeCount; }
  CPDF_Stream* GetStream() const { return m_pStream; }
  void SetTimeCount(uint32_t dwTimeCount) { m_dwTimeCount = dwTimeCount; }
  int m_dwTimeCount;

 public:
  int StartGetCachedBitmap(CPDF_Dictionary* pFormResources,
                           CPDF_Dictionary* pPageResources,
                           FX_BOOL bStdCS = FALSE,
                           uint32_t GroupFamily = 0,
                           FX_BOOL bLoadMask = FALSE,
                           CPDF_RenderStatus* pRenderStatus = nullptr,
                           int32_t downsampleWidth = 0,
                           int32_t downsampleHeight = 0);
  int Continue(IFX_Pause* pPause);
  CFX_DIBSource* DetachBitmap();
  CFX_DIBSource* DetachMask();
  CFX_DIBSource* m_pCurBitmap;
  CFX_DIBSource* m_pCurMask;
  uint32_t m_MatteColor;
  CPDF_RenderStatus* m_pRenderStatus;

 protected:
  void ContinueGetCachedBitmap();

  CPDF_Document* m_pDocument;
  CPDF_Stream* m_pStream;
  CFX_DIBSource* m_pCachedBitmap;
  CFX_DIBSource* m_pCachedMask;
  uint32_t m_dwCacheSize;
  void CalcSize();
};
typedef struct {
  FX_FLOAT m_DecodeMin;
  FX_FLOAT m_DecodeStep;
  int m_ColorKeyMin;
  int m_ColorKeyMax;
} DIB_COMP_DATA;

class CPDF_DIBSource : public CFX_DIBSource {
 public:
  CPDF_DIBSource();
  ~CPDF_DIBSource() override;

  FX_BOOL Load(CPDF_Document* pDoc,
               const CPDF_Stream* pStream,
               CPDF_DIBSource** ppMask,
               uint32_t* pMatteColor,
               CPDF_Dictionary* pFormResources,
               CPDF_Dictionary* pPageResources,
               FX_BOOL bStdCS = FALSE,
               uint32_t GroupFamily = 0,
               FX_BOOL bLoadMask = FALSE);

  // CFX_DIBSource
  FX_BOOL SkipToScanline(int line, IFX_Pause* pPause) const override;
  uint8_t* GetBuffer() const override;
  const uint8_t* GetScanline(int line) const override;
  void DownSampleScanline(int line,
                          uint8_t* dest_scan,
                          int dest_bpp,
                          int dest_width,
                          FX_BOOL bFlipX,
                          int clip_left,
                          int clip_width) const override;

  CFX_DIBitmap* GetBitmap() const;
  void ReleaseBitmap(CFX_DIBitmap* pBitmap) const;
  uint32_t GetMatteColor() const { return m_MatteColor; }

  int StartLoadDIBSource(CPDF_Document* pDoc,
                         const CPDF_Stream* pStream,
                         FX_BOOL bHasMask,
                         CPDF_Dictionary* pFormResources,
                         CPDF_Dictionary* pPageResources,
                         FX_BOOL bStdCS = FALSE,
                         uint32_t GroupFamily = 0,
                         FX_BOOL bLoadMask = FALSE);
  int ContinueLoadDIBSource(IFX_Pause* pPause);
  int StratLoadMask();
  int StartLoadMaskDIB();
  int ContinueLoadMaskDIB(IFX_Pause* pPause);
  int ContinueToLoadMask();
  CPDF_DIBSource* DetachMask();

 private:
  bool LoadColorInfo(const CPDF_Dictionary* pFormResources,
                     const CPDF_Dictionary* pPageResources);
  DIB_COMP_DATA* GetDecodeAndMaskArray(FX_BOOL& bDefaultDecode,
                                       FX_BOOL& bColorKey);
  CPDF_DIBSource* LoadMask(uint32_t& MatteColor);
  CPDF_DIBSource* LoadMaskDIB(CPDF_Stream* pMask);
  void LoadJpxBitmap();
  void LoadPalette();
  int CreateDecoder();
  void TranslateScanline24bpp(uint8_t* dest_scan,
                              const uint8_t* src_scan) const;
  void ValidateDictParam();
  void DownSampleScanline1Bit(int orig_Bpp,
                              int dest_Bpp,
                              uint32_t src_width,
                              const uint8_t* pSrcLine,
                              uint8_t* dest_scan,
                              int dest_width,
                              FX_BOOL bFlipX,
                              int clip_left,
                              int clip_width) const;
  void DownSampleScanline8Bit(int orig_Bpp,
                              int dest_Bpp,
                              uint32_t src_width,
                              const uint8_t* pSrcLine,
                              uint8_t* dest_scan,
                              int dest_width,
                              FX_BOOL bFlipX,
                              int clip_left,
                              int clip_width) const;
  void DownSampleScanline32Bit(int orig_Bpp,
                               int dest_Bpp,
                               uint32_t src_width,
                               const uint8_t* pSrcLine,
                               uint8_t* dest_scan,
                               int dest_width,
                               FX_BOOL bFlipX,
                               int clip_left,
                               int clip_width) const;
  FX_BOOL TransMask() const;

  CPDF_Document* m_pDocument;
  const CPDF_Stream* m_pStream;
  std::unique_ptr<CPDF_StreamAcc> m_pStreamAcc;
  const CPDF_Dictionary* m_pDict;
  CPDF_ColorSpace* m_pColorSpace;
  uint32_t m_Family;
  uint32_t m_bpc;
  uint32_t m_bpc_orig;
  uint32_t m_nComponents;
  uint32_t m_GroupFamily;
  uint32_t m_MatteColor;
  FX_BOOL m_bLoadMask;
  FX_BOOL m_bDefaultDecode;
  FX_BOOL m_bImageMask;
  FX_BOOL m_bDoBpcCheck;
  FX_BOOL m_bColorKey;
  FX_BOOL m_bHasMask;
  FX_BOOL m_bStdCS;
  DIB_COMP_DATA* m_pCompData;
  uint8_t* m_pLineBuf;
  uint8_t* m_pMaskedLine;
  std::unique_ptr<CFX_DIBitmap> m_pCachedBitmap;
  std::unique_ptr<CCodec_ScanlineDecoder> m_pDecoder;
  CCodec_Jbig2Context* m_pJbig2Context;
  CPDF_DIBSource* m_pMask;
  std::unique_ptr<CPDF_StreamAcc> m_pGlobalStream;
  CPDF_Stream* m_pMaskStream;
  int m_Status;
};

#define FPDF_HUGE_IMAGE_SIZE 60000000
class CPDF_DIBTransferFunc : public CFX_FilteredDIB {
 public:
  explicit CPDF_DIBTransferFunc(const CPDF_TransferFunc* pTransferFunc);
  ~CPDF_DIBTransferFunc() override;

  // CFX_FilteredDIB
  FXDIB_Format GetDestFormat() override;
  FX_ARGB* GetDestPalette() override;
  void TranslateScanline(uint8_t* dest_buf,
                         const uint8_t* src_buf) const override;
  void TranslateDownSamples(uint8_t* dest_buf,
                            const uint8_t* src_buf,
                            int pixels,
                            int Bpp) const override;

  const uint8_t* m_RampR;
  const uint8_t* m_RampG;
  const uint8_t* m_RampB;
};

#endif  // CORE_FPDFAPI_FPDF_RENDER_RENDER_INT_H_
