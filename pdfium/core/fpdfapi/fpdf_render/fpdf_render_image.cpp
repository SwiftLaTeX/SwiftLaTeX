// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_render/render_int.h"

#include <utility>
#include <vector>

#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"
#include "core/fpdfapi/fpdf_page/cpdf_tilingpattern.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_render/cpdf_pagerendercache.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_rendercontext.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "core/fxge/include/fx_ge.h"

#ifdef _SKIA_SUPPORT_
#include "core/fxge/skia/fx_skia_device.h"
#endif

FX_BOOL CPDF_RenderStatus::ProcessImage(const CPDF_ImageObject* pImageObj,
                                        const CFX_Matrix* pObj2Device) {
  CPDF_ImageRenderer render;
  if (render.Start(this, pImageObj, pObj2Device, m_bStdCS, m_curBlend)) {
    render.Continue(nullptr);
  }
  return render.m_Result;
}
void CPDF_RenderStatus::CompositeDIBitmap(CFX_DIBitmap* pDIBitmap,
                                          int left,
                                          int top,
                                          FX_ARGB mask_argb,
                                          int bitmap_alpha,
                                          int blend_mode,
                                          int Transparency) {
  if (!pDIBitmap) {
    return;
  }
  if (blend_mode == FXDIB_BLEND_NORMAL) {
    if (!pDIBitmap->IsAlphaMask()) {
      if (bitmap_alpha < 255) {
#ifdef _SKIA_SUPPORT_
        void* dummy;
        CFX_Matrix m(pDIBitmap->GetWidth(), 0, 0, -pDIBitmap->GetHeight(), left,
                     top + pDIBitmap->GetHeight());
        m_pDevice->StartDIBits(pDIBitmap, bitmap_alpha, 0, &m, 0, dummy);
        return;
#else
        pDIBitmap->MultiplyAlpha(bitmap_alpha);
#endif
      }
#ifdef _SKIA_SUPPORT_
      CFX_SkiaDeviceDriver::PreMultiply(pDIBitmap);
#endif
      if (m_pDevice->SetDIBits(pDIBitmap, left, top)) {
        return;
      }
    } else {
      uint32_t fill_argb = m_Options.TranslateColor(mask_argb);
      if (bitmap_alpha < 255) {
        ((uint8_t*)&fill_argb)[3] =
            ((uint8_t*)&fill_argb)[3] * bitmap_alpha / 255;
      }
      if (m_pDevice->SetBitMask(pDIBitmap, left, top, fill_argb)) {
        return;
      }
    }
  }
  bool bIsolated = !!(Transparency & PDFTRANS_ISOLATED);
  bool bGroup = !!(Transparency & PDFTRANS_GROUP);
  bool bBackAlphaRequired = blend_mode && bIsolated && !m_bDropObjects;
  bool bGetBackGround =
      ((m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT)) ||
      (!(m_pDevice->GetRenderCaps() & FXRC_ALPHA_OUTPUT) &&
       (m_pDevice->GetRenderCaps() & FXRC_GET_BITS) && !bBackAlphaRequired);
  if (bGetBackGround) {
    if (bIsolated || !bGroup) {
      if (pDIBitmap->IsAlphaMask()) {
        return;
      }
      m_pDevice->SetDIBitsWithBlend(pDIBitmap, left, top, blend_mode);
    } else {
      FX_RECT rect(left, top, left + pDIBitmap->GetWidth(),
                   top + pDIBitmap->GetHeight());
      rect.Intersect(m_pDevice->GetClipBox());
      CFX_DIBitmap* pClone = nullptr;
      FX_BOOL bClone = FALSE;
      if (m_pDevice->GetBackDrop() && m_pDevice->GetBitmap()) {
        bClone = TRUE;
        pClone = m_pDevice->GetBackDrop()->Clone(&rect);
        CFX_DIBitmap* pForeBitmap = m_pDevice->GetBitmap();
        pClone->CompositeBitmap(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                                pForeBitmap, rect.left, rect.top);
        left = left >= 0 ? 0 : left;
        top = top >= 0 ? 0 : top;
        if (!pDIBitmap->IsAlphaMask())
          pClone->CompositeBitmap(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                                  pDIBitmap, left, top, blend_mode);
        else
          pClone->CompositeMask(0, 0, pClone->GetWidth(), pClone->GetHeight(),
                                pDIBitmap, mask_argb, left, top, blend_mode);
      } else {
        pClone = pDIBitmap;
      }
      if (m_pDevice->GetBackDrop()) {
        m_pDevice->SetDIBits(pClone, rect.left, rect.top);
      } else {
        if (pDIBitmap->IsAlphaMask()) {
          return;
        }
        m_pDevice->SetDIBitsWithBlend(pDIBitmap, rect.left, rect.top,
                                      blend_mode);
      }
      if (bClone) {
        delete pClone;
      }
    }
    return;
  }
  int back_left, back_top;
  FX_RECT rect(left, top, left + pDIBitmap->GetWidth(),
               top + pDIBitmap->GetHeight());
  std::unique_ptr<CFX_DIBitmap> pBackdrop(
      GetBackdrop(m_pCurObj, rect, back_left, back_top,
                  blend_mode > FXDIB_BLEND_NORMAL && bIsolated));
  if (!pBackdrop)
    return;

  if (!pDIBitmap->IsAlphaMask()) {
    pBackdrop->CompositeBitmap(left - back_left, top - back_top,
                               pDIBitmap->GetWidth(), pDIBitmap->GetHeight(),
                               pDIBitmap, 0, 0, blend_mode);
  } else {
    pBackdrop->CompositeMask(left - back_left, top - back_top,
                             pDIBitmap->GetWidth(), pDIBitmap->GetHeight(),
                             pDIBitmap, mask_argb, 0, 0, blend_mode);
  }

  std::unique_ptr<CFX_DIBitmap> pBackdrop1(new CFX_DIBitmap);
  pBackdrop1->Create(pBackdrop->GetWidth(), pBackdrop->GetHeight(),
                     FXDIB_Rgb32);
  pBackdrop1->Clear((uint32_t)-1);
  pBackdrop1->CompositeBitmap(0, 0, pBackdrop->GetWidth(),
                              pBackdrop->GetHeight(), pBackdrop.get(), 0, 0);
  pBackdrop = std::move(pBackdrop1);
  m_pDevice->SetDIBits(pBackdrop.get(), back_left, back_top);
}

CPDF_TransferFunc::CPDF_TransferFunc(CPDF_Document* pDoc) : m_pPDFDoc(pDoc) {}

FX_COLORREF CPDF_TransferFunc::TranslateColor(FX_COLORREF rgb) const {
  return FXSYS_RGB(m_Samples[FXSYS_GetRValue(rgb)],
                   m_Samples[256 + FXSYS_GetGValue(rgb)],
                   m_Samples[512 + FXSYS_GetBValue(rgb)]);
}

CFX_DIBSource* CPDF_TransferFunc::TranslateImage(const CFX_DIBSource* pSrc,
                                                 FX_BOOL bAutoDropSrc) {
  CPDF_DIBTransferFunc* pDest = new CPDF_DIBTransferFunc(this);
  pDest->LoadSrc(pSrc, bAutoDropSrc);
  return pDest;
}

CPDF_DIBTransferFunc::~CPDF_DIBTransferFunc() {}

FXDIB_Format CPDF_DIBTransferFunc::GetDestFormat() {
  if (m_pSrc->IsAlphaMask()) {
    return FXDIB_8bppMask;
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  return (m_pSrc->HasAlpha()) ? FXDIB_Argb : FXDIB_Rgb32;
#else
  return (m_pSrc->HasAlpha()) ? FXDIB_Argb : FXDIB_Rgb;
#endif
}

FX_ARGB* CPDF_DIBTransferFunc::GetDestPalette() {
  return nullptr;
}

CPDF_DIBTransferFunc::CPDF_DIBTransferFunc(
    const CPDF_TransferFunc* pTransferFunc) {
  m_RampR = pTransferFunc->m_Samples;
  m_RampG = &pTransferFunc->m_Samples[256];
  m_RampB = &pTransferFunc->m_Samples[512];
}
void CPDF_DIBTransferFunc::TranslateScanline(uint8_t* dest_buf,
                                             const uint8_t* src_buf) const {
  int i;
  FX_BOOL bSkip = FALSE;
  switch (m_pSrc->GetFormat()) {
    case FXDIB_1bppRgb: {
      int r0 = m_RampR[0], g0 = m_RampG[0], b0 = m_RampB[0];
      int r1 = m_RampR[255], g1 = m_RampG[255], b1 = m_RampB[255];
      for (i = 0; i < m_Width; i++) {
        if (src_buf[i / 8] & (1 << (7 - i % 8))) {
          *dest_buf++ = b1;
          *dest_buf++ = g1;
          *dest_buf++ = r1;
        } else {
          *dest_buf++ = b0;
          *dest_buf++ = g0;
          *dest_buf++ = r0;
        }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
        dest_buf++;
#endif
      }
      break;
    }
    case FXDIB_1bppMask: {
      int m0 = m_RampR[0], m1 = m_RampR[255];
      for (i = 0; i < m_Width; i++) {
        if (src_buf[i / 8] & (1 << (7 - i % 8))) {
          *dest_buf++ = m1;
        } else {
          *dest_buf++ = m0;
        }
      }
      break;
    }
    case FXDIB_8bppRgb: {
      FX_ARGB* pPal = m_pSrc->GetPalette();
      for (i = 0; i < m_Width; i++) {
        if (pPal) {
          FX_ARGB src_argb = pPal[*src_buf];
          *dest_buf++ = m_RampB[FXARGB_R(src_argb)];
          *dest_buf++ = m_RampG[FXARGB_G(src_argb)];
          *dest_buf++ = m_RampR[FXARGB_B(src_argb)];
        } else {
          uint32_t src_byte = *src_buf;
          *dest_buf++ = m_RampB[src_byte];
          *dest_buf++ = m_RampG[src_byte];
          *dest_buf++ = m_RampR[src_byte];
        }
        src_buf++;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
        dest_buf++;
#endif
      }
      break;
    }
    case FXDIB_8bppMask:
      for (i = 0; i < m_Width; i++) {
        *dest_buf++ = m_RampR[*(src_buf++)];
      }
      break;
    case FXDIB_Rgb:
      for (i = 0; i < m_Width; i++) {
        *dest_buf++ = m_RampB[*(src_buf++)];
        *dest_buf++ = m_RampG[*(src_buf++)];
        *dest_buf++ = m_RampR[*(src_buf++)];
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
        dest_buf++;
#endif
      }
      break;
    case FXDIB_Rgb32:
      bSkip = TRUE;
    case FXDIB_Argb:
      for (i = 0; i < m_Width; i++) {
        *dest_buf++ = m_RampB[*(src_buf++)];
        *dest_buf++ = m_RampG[*(src_buf++)];
        *dest_buf++ = m_RampR[*(src_buf++)];
        if (!bSkip) {
          *dest_buf++ = *src_buf;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
        } else {
          dest_buf++;
#endif
        }
        src_buf++;
      }
      break;
    default:
      break;
  }
}
void CPDF_DIBTransferFunc::TranslateDownSamples(uint8_t* dest_buf,
                                                const uint8_t* src_buf,
                                                int pixels,
                                                int Bpp) const {
  if (Bpp == 8) {
    for (int i = 0; i < pixels; i++) {
      *dest_buf++ = m_RampR[*(src_buf++)];
    }
  } else if (Bpp == 24) {
    for (int i = 0; i < pixels; i++) {
      *dest_buf++ = m_RampB[*(src_buf++)];
      *dest_buf++ = m_RampG[*(src_buf++)];
      *dest_buf++ = m_RampR[*(src_buf++)];
    }
  } else {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
    if (!m_pSrc->HasAlpha()) {
      for (int i = 0; i < pixels; i++) {
        *dest_buf++ = m_RampB[*(src_buf++)];
        *dest_buf++ = m_RampG[*(src_buf++)];
        *dest_buf++ = m_RampR[*(src_buf++)];
        dest_buf++;
        src_buf++;
      }
    } else {
#endif
      for (int i = 0; i < pixels; i++) {
        *dest_buf++ = m_RampB[*(src_buf++)];
        *dest_buf++ = m_RampG[*(src_buf++)];
        *dest_buf++ = m_RampR[*(src_buf++)];
        *dest_buf++ = *(src_buf++);
      }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
    }
#endif
  }
}

CPDF_ImageRenderer::CPDF_ImageRenderer() {
  m_pRenderStatus = nullptr;
  m_pImageObject = nullptr;
  m_Result = TRUE;
  m_Status = 0;
  m_DeviceHandle = nullptr;
  m_bStdCS = FALSE;
  m_bPatternColor = FALSE;
  m_BlendType = FXDIB_BLEND_NORMAL;
  m_pPattern = nullptr;
  m_pObj2Device = nullptr;
}

CPDF_ImageRenderer::~CPDF_ImageRenderer() {
  if (m_DeviceHandle) {
    m_pRenderStatus->m_pDevice->CancelDIBits(m_DeviceHandle);
  }
}

FX_BOOL CPDF_ImageRenderer::StartLoadDIBSource() {
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOutterRect();
  int dest_width = image_rect.Width();
  int dest_height = image_rect.Height();
  if (m_ImageMatrix.a < 0) {
    dest_width = -dest_width;
  }
  if (m_ImageMatrix.d > 0) {
    dest_height = -dest_height;
  }
  if (m_Loader.Start(m_pImageObject,
                     m_pRenderStatus->m_pContext->GetPageCache(), &m_LoadHandle,
                     m_bStdCS, m_pRenderStatus->m_GroupFamily,
                     m_pRenderStatus->m_bLoadMask, m_pRenderStatus, dest_width,
                     dest_height)) {
    if (m_LoadHandle) {
      m_Status = 4;
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CPDF_ImageRenderer::StartRenderDIBSource() {
  if (!m_Loader.m_pBitmap) {
    return FALSE;
  }
  m_BitmapAlpha = 255;
  const CPDF_GeneralStateData* pGeneralState =
      m_pImageObject->m_GeneralState.GetObject();
  if (pGeneralState) {
    m_BitmapAlpha = FXSYS_round(pGeneralState->m_FillAlpha * 255);
  }
  m_pDIBSource = m_Loader.m_pBitmap;
  if (m_pRenderStatus->m_Options.m_ColorMode == RENDER_COLOR_ALPHA &&
      !m_Loader.m_pMask) {
    return StartBitmapAlpha();
  }
  if (pGeneralState && pGeneralState->m_pTR) {
    if (!pGeneralState->m_pTransferFunc) {
      ((CPDF_GeneralStateData*)pGeneralState)->m_pTransferFunc =
          m_pRenderStatus->GetTransferFunc(pGeneralState->m_pTR);
    }
    if (pGeneralState->m_pTransferFunc &&
        !pGeneralState->m_pTransferFunc->m_bIdentity) {
      m_pDIBSource = m_Loader.m_pBitmap =
          pGeneralState->m_pTransferFunc->TranslateImage(m_Loader.m_pBitmap,
                                                         !m_Loader.m_bCached);
      if (m_Loader.m_bCached && m_Loader.m_pMask) {
        m_Loader.m_pMask = m_Loader.m_pMask->Clone();
      }
      m_Loader.m_bCached = FALSE;
    }
  }
  m_FillArgb = 0;
  m_bPatternColor = FALSE;
  m_pPattern = nullptr;
  if (m_pDIBSource->IsAlphaMask()) {
    CPDF_Color* pColor = m_pImageObject->m_ColorState.GetFillColor();
    if (pColor && pColor->IsPattern()) {
      m_pPattern = pColor->GetPattern();
      if (m_pPattern) {
        m_bPatternColor = TRUE;
      }
    }
    m_FillArgb = m_pRenderStatus->GetFillArgb(m_pImageObject);
  } else if (m_pRenderStatus->m_Options.m_ColorMode == RENDER_COLOR_GRAY) {
    m_pClone.reset(m_pDIBSource->Clone());
    m_pClone->ConvertColorScale(m_pRenderStatus->m_Options.m_BackColor,
                                m_pRenderStatus->m_Options.m_ForeColor);
    m_pDIBSource = m_pClone.get();
  }
  m_Flags = 0;
  if (m_pRenderStatus->m_Options.m_Flags & RENDER_FORCE_DOWNSAMPLE) {
    m_Flags |= RENDER_FORCE_DOWNSAMPLE;
  } else if (m_pRenderStatus->m_Options.m_Flags & RENDER_FORCE_HALFTONE) {
    m_Flags |= RENDER_FORCE_HALFTONE;
  }
  if (m_pRenderStatus->m_pDevice->GetDeviceClass() != FXDC_DISPLAY) {
    CPDF_Object* pFilters =
        m_pImageObject->m_pImage->GetStream()->GetDict()->GetDirectObjectBy(
            "Filter");
    if (pFilters) {
      if (pFilters->IsName()) {
        CFX_ByteString bsDecodeType = pFilters->GetString();
        if (bsDecodeType == "DCTDecode" || bsDecodeType == "JPXDecode") {
          m_Flags |= FXRENDER_IMAGE_LOSSY;
        }
      } else if (CPDF_Array* pArray = pFilters->AsArray()) {
        for (size_t i = 0; i < pArray->GetCount(); i++) {
          CFX_ByteString bsDecodeType = pArray->GetStringAt(i);
          if (bsDecodeType == "DCTDecode" || bsDecodeType == "JPXDecode") {
            m_Flags |= FXRENDER_IMAGE_LOSSY;
            break;
          }
        }
      }
    }
  }
  if (m_pRenderStatus->m_Options.m_Flags & RENDER_NOIMAGESMOOTH) {
    m_Flags |= FXDIB_NOSMOOTH;
  } else if (m_pImageObject->m_pImage->IsInterpol()) {
    m_Flags |= FXDIB_INTERPOL;
  }
  if (m_Loader.m_pMask) {
    return DrawMaskedImage();
  }
  if (m_bPatternColor) {
    return DrawPatternImage(m_pObj2Device);
  }
  if (m_BitmapAlpha == 255 && pGeneralState && pGeneralState->m_FillOP &&
      pGeneralState->m_OPMode == 0 &&
      pGeneralState->m_BlendType == FXDIB_BLEND_NORMAL &&
      pGeneralState->m_StrokeAlpha == 1 && pGeneralState->m_FillAlpha == 1) {
    CPDF_Document* pDocument = nullptr;
    CPDF_Page* pPage = nullptr;
    if (m_pRenderStatus->m_pContext->GetPageCache()) {
      pPage = m_pRenderStatus->m_pContext->GetPageCache()->GetPage();
      pDocument = pPage->m_pDocument;
    } else {
      pDocument = m_pImageObject->m_pImage->GetDocument();
    }
    CPDF_Dictionary* pPageResources = pPage ? pPage->m_pPageResources : nullptr;
    CPDF_Object* pCSObj =
        m_pImageObject->m_pImage->GetStream()->GetDict()->GetDirectObjectBy(
            "ColorSpace");
    CPDF_ColorSpace* pColorSpace =
        pDocument->LoadColorSpace(pCSObj, pPageResources);
    if (pColorSpace) {
      int format = pColorSpace->GetFamily();
      if (format == PDFCS_DEVICECMYK || format == PDFCS_SEPARATION ||
          format == PDFCS_DEVICEN) {
        m_BlendType = FXDIB_BLEND_DARKEN;
      }
      pDocument->GetPageData()->ReleaseColorSpace(pCSObj);
    }
  }
  return StartDIBSource();
}

FX_BOOL CPDF_ImageRenderer::Start(CPDF_RenderStatus* pStatus,
                                  const CPDF_PageObject* pObj,
                                  const CFX_Matrix* pObj2Device,
                                  FX_BOOL bStdCS,
                                  int blendType) {
  m_pRenderStatus = pStatus;
  m_bStdCS = bStdCS;
  m_pImageObject = pObj->AsImage();
  m_BlendType = blendType;
  m_pObj2Device = pObj2Device;
  CPDF_Dictionary* pOC = m_pImageObject->m_pImage->GetOC();
  if (pOC && m_pRenderStatus->m_Options.m_pOCContext &&
      !m_pRenderStatus->m_Options.m_pOCContext->CheckOCGVisible(pOC)) {
    return FALSE;
  }
  m_ImageMatrix = m_pImageObject->m_Matrix;
  m_ImageMatrix.Concat(*pObj2Device);
  if (StartLoadDIBSource()) {
    return TRUE;
  }
  return StartRenderDIBSource();
}

FX_BOOL CPDF_ImageRenderer::Start(CPDF_RenderStatus* pStatus,
                                  const CFX_DIBSource* pDIBSource,
                                  FX_ARGB bitmap_argb,
                                  int bitmap_alpha,
                                  const CFX_Matrix* pImage2Device,
                                  uint32_t flags,
                                  FX_BOOL bStdCS,
                                  int blendType) {
  m_pRenderStatus = pStatus;
  m_pDIBSource = pDIBSource;
  m_FillArgb = bitmap_argb;
  m_BitmapAlpha = bitmap_alpha;
  m_ImageMatrix = *pImage2Device;
  m_Flags = flags;
  m_bStdCS = bStdCS;
  m_BlendType = blendType;
  return StartDIBSource();
}

FX_BOOL CPDF_ImageRenderer::DrawPatternImage(const CFX_Matrix* pObj2Device) {
  if (m_pRenderStatus->m_bPrint &&
      !(m_pRenderStatus->m_pDevice->GetRenderCaps() & FXRC_BLEND_MODE)) {
    m_Result = FALSE;
    return FALSE;
  }
  FX_RECT rect = m_ImageMatrix.GetUnitRect().GetOutterRect();
  rect.Intersect(m_pRenderStatus->m_pDevice->GetClipBox());
  if (rect.IsEmpty()) {
    return FALSE;
  }
  CFX_Matrix new_matrix = m_ImageMatrix;
  new_matrix.TranslateI(-rect.left, -rect.top);
  int width = rect.Width();
  int height = rect.Height();
  CFX_FxgeDevice bitmap_device1;
  if (!bitmap_device1.Create(rect.Width(), rect.Height(), FXDIB_Rgb32, nullptr))
    return TRUE;

  bitmap_device1.GetBitmap()->Clear(0xffffff);
  {
    CPDF_RenderStatus bitmap_render;
    bitmap_render.Initialize(m_pRenderStatus->m_pContext, &bitmap_device1,
                             nullptr, nullptr, nullptr, nullptr,
                             &m_pRenderStatus->m_Options, 0,
                             m_pRenderStatus->m_bDropObjects, nullptr, TRUE);
    CFX_Matrix patternDevice = *pObj2Device;
    patternDevice.Translate((FX_FLOAT)-rect.left, (FX_FLOAT)-rect.top);
    if (CPDF_TilingPattern* pTilingPattern = m_pPattern->AsTilingPattern()) {
      bitmap_render.DrawTilingPattern(pTilingPattern, m_pImageObject,
                                      &patternDevice, FALSE);
    } else if (CPDF_ShadingPattern* pShadingPattern =
                   m_pPattern->AsShadingPattern()) {
      bitmap_render.DrawShadingPattern(pShadingPattern, m_pImageObject,
                                       &patternDevice, FALSE);
    }
  }
  {
    CFX_FxgeDevice bitmap_device2;
    if (!bitmap_device2.Create(rect.Width(), rect.Height(), FXDIB_8bppRgb,
                               nullptr)) {
      return TRUE;
    }
    bitmap_device2.GetBitmap()->Clear(0);
    CPDF_RenderStatus bitmap_render;
    bitmap_render.Initialize(m_pRenderStatus->m_pContext, &bitmap_device2,
                             nullptr, nullptr, nullptr, nullptr, nullptr, 0,
                             m_pRenderStatus->m_bDropObjects, nullptr, TRUE);
    CPDF_ImageRenderer image_render;
    if (image_render.Start(&bitmap_render, m_pDIBSource, 0xffffffff, 255,
                           &new_matrix, m_Flags, TRUE)) {
      image_render.Continue(nullptr);
    }
    if (m_Loader.m_MatteColor != 0xffffffff) {
      int matte_r = FXARGB_R(m_Loader.m_MatteColor);
      int matte_g = FXARGB_G(m_Loader.m_MatteColor);
      int matte_b = FXARGB_B(m_Loader.m_MatteColor);
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan =
            (uint8_t*)bitmap_device1.GetBitmap()->GetScanline(row);
        const uint8_t* mask_scan = bitmap_device2.GetBitmap()->GetScanline(row);
        for (int col = 0; col < width; col++) {
          int alpha = *mask_scan++;
          if (alpha) {
            int orig = (*dest_scan - matte_b) * 255 / alpha + matte_b;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            orig = (*dest_scan - matte_g) * 255 / alpha + matte_g;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            orig = (*dest_scan - matte_r) * 255 / alpha + matte_r;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            dest_scan++;
          } else {
            dest_scan += 4;
          }
        }
      }
    }
    bitmap_device2.GetBitmap()->ConvertFormat(FXDIB_8bppMask);
    bitmap_device1.GetBitmap()->MultiplyAlpha(bitmap_device2.GetBitmap());
    bitmap_device1.GetBitmap()->MultiplyAlpha(255);
  }
  m_pRenderStatus->m_pDevice->SetDIBitsWithBlend(
      bitmap_device1.GetBitmap(), rect.left, rect.top, m_BlendType);
  return FALSE;
}
FX_BOOL CPDF_ImageRenderer::DrawMaskedImage() {
  if (m_pRenderStatus->m_bPrint &&
      !(m_pRenderStatus->m_pDevice->GetRenderCaps() & FXRC_BLEND_MODE)) {
    m_Result = FALSE;
    return FALSE;
  }
  FX_RECT rect = m_ImageMatrix.GetUnitRect().GetOutterRect();
  rect.Intersect(m_pRenderStatus->m_pDevice->GetClipBox());
  if (rect.IsEmpty()) {
    return FALSE;
  }
  CFX_Matrix new_matrix = m_ImageMatrix;
  new_matrix.TranslateI(-rect.left, -rect.top);
  int width = rect.Width();
  int height = rect.Height();
  CFX_FxgeDevice bitmap_device1;
  if (!bitmap_device1.Create(width, height, FXDIB_Rgb32, nullptr))
    return TRUE;

  bitmap_device1.GetBitmap()->Clear(0xffffff);
  {
    CPDF_RenderStatus bitmap_render;
    bitmap_render.Initialize(m_pRenderStatus->m_pContext, &bitmap_device1,
                             nullptr, nullptr, nullptr, nullptr, nullptr, 0,
                             m_pRenderStatus->m_bDropObjects, nullptr, TRUE);
    CPDF_ImageRenderer image_render;
    if (image_render.Start(&bitmap_render, m_pDIBSource, 0, 255, &new_matrix,
                           m_Flags, TRUE)) {
      image_render.Continue(nullptr);
    }
  }
  {
    CFX_FxgeDevice bitmap_device2;
    if (!bitmap_device2.Create(width, height, FXDIB_8bppRgb, nullptr))
      return TRUE;

    bitmap_device2.GetBitmap()->Clear(0);
    CPDF_RenderStatus bitmap_render;
    bitmap_render.Initialize(m_pRenderStatus->m_pContext, &bitmap_device2,
                             nullptr, nullptr, nullptr, nullptr, nullptr, 0,
                             m_pRenderStatus->m_bDropObjects, nullptr, TRUE);
    CPDF_ImageRenderer image_render;
    if (image_render.Start(&bitmap_render, m_Loader.m_pMask, 0xffffffff, 255,
                           &new_matrix, m_Flags, TRUE)) {
      image_render.Continue(nullptr);
    }
    if (m_Loader.m_MatteColor != 0xffffffff) {
      int matte_r = FXARGB_R(m_Loader.m_MatteColor);
      int matte_g = FXARGB_G(m_Loader.m_MatteColor);
      int matte_b = FXARGB_B(m_Loader.m_MatteColor);
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan =
            (uint8_t*)bitmap_device1.GetBitmap()->GetScanline(row);
        const uint8_t* mask_scan = bitmap_device2.GetBitmap()->GetScanline(row);
        for (int col = 0; col < width; col++) {
          int alpha = *mask_scan++;
          if (alpha) {
            int orig = (*dest_scan - matte_b) * 255 / alpha + matte_b;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            orig = (*dest_scan - matte_g) * 255 / alpha + matte_g;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            orig = (*dest_scan - matte_r) * 255 / alpha + matte_r;
            if (orig < 0) {
              orig = 0;
            } else if (orig > 255) {
              orig = 255;
            }
            *dest_scan++ = orig;
            dest_scan++;
          } else {
            dest_scan += 4;
          }
        }
      }
    }
    bitmap_device2.GetBitmap()->ConvertFormat(FXDIB_8bppMask);
    bitmap_device1.GetBitmap()->MultiplyAlpha(bitmap_device2.GetBitmap());
#ifdef _SKIA_SUPPORT_
    CFX_SkiaDeviceDriver::PreMultiply(bitmap_device1.GetBitmap());
#endif
    if (m_BitmapAlpha < 255) {
      bitmap_device1.GetBitmap()->MultiplyAlpha(m_BitmapAlpha);
    }
  }
  m_pRenderStatus->m_pDevice->SetDIBitsWithBlend(
      bitmap_device1.GetBitmap(), rect.left, rect.top, m_BlendType);
  return FALSE;
}

FX_BOOL CPDF_ImageRenderer::StartDIBSource() {
  if (!(m_Flags & RENDER_FORCE_DOWNSAMPLE) && m_pDIBSource->GetBPP() > 1) {
    int image_size = m_pDIBSource->GetBPP() / 8 * m_pDIBSource->GetWidth() *
                     m_pDIBSource->GetHeight();
    if (image_size > FPDF_HUGE_IMAGE_SIZE &&
        !(m_Flags & RENDER_FORCE_HALFTONE)) {
      m_Flags |= RENDER_FORCE_DOWNSAMPLE;
    }
  }
#ifdef _SKIA_SUPPORT_
  CFX_DIBitmap* premultiplied = m_pDIBSource->Clone();
  CFX_SkiaDeviceDriver::PreMultiply(premultiplied);
  if (m_pRenderStatus->m_pDevice->StartDIBitsWithBlend(
          premultiplied, m_BitmapAlpha, m_FillArgb, &m_ImageMatrix, m_Flags,
          m_DeviceHandle, m_BlendType)) {
    if (m_DeviceHandle) {
      m_Status = 3;
      return TRUE;
    }
    return FALSE;
  }
#else
  if (m_pRenderStatus->m_pDevice->StartDIBitsWithBlend(
          m_pDIBSource, m_BitmapAlpha, m_FillArgb, &m_ImageMatrix, m_Flags,
          m_DeviceHandle, m_BlendType)) {
    if (m_DeviceHandle) {
      m_Status = 3;
      return TRUE;
    }
    return FALSE;
  }
#endif
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOutterRect();
  int dest_width = image_rect.Width();
  int dest_height = image_rect.Height();
  if ((FXSYS_fabs(m_ImageMatrix.b) >= 0.5f || m_ImageMatrix.a == 0) ||
      (FXSYS_fabs(m_ImageMatrix.c) >= 0.5f || m_ImageMatrix.d == 0)) {
    if (m_pRenderStatus->m_bPrint &&
        !(m_pRenderStatus->m_pDevice->GetRenderCaps() & FXRC_BLEND_MODE)) {
      m_Result = FALSE;
      return FALSE;
    }
    FX_RECT clip_box = m_pRenderStatus->m_pDevice->GetClipBox();
    clip_box.Intersect(image_rect);
    m_Status = 2;
    m_pTransformer.reset(new CFX_ImageTransformer(m_pDIBSource, &m_ImageMatrix,
                                                  m_Flags, &clip_box));
    m_pTransformer->Start();
    return TRUE;
  }
  if (m_ImageMatrix.a < 0)
    dest_width = -dest_width;

  if (m_ImageMatrix.d > 0)
    dest_height = -dest_height;

  int dest_left = dest_width > 0 ? image_rect.left : image_rect.right;
  int dest_top = dest_height > 0 ? image_rect.top : image_rect.bottom;
  if (m_pDIBSource->IsOpaqueImage() && m_BitmapAlpha == 255) {
    if (m_pRenderStatus->m_pDevice->StretchDIBitsWithFlagsAndBlend(
            m_pDIBSource, dest_left, dest_top, dest_width, dest_height, m_Flags,
            m_BlendType)) {
      return FALSE;
    }
  }
  if (m_pDIBSource->IsAlphaMask()) {
    if (m_BitmapAlpha != 255)
      m_FillArgb = FXARGB_MUL_ALPHA(m_FillArgb, m_BitmapAlpha);
    if (m_pRenderStatus->m_pDevice->StretchBitMaskWithFlags(
            m_pDIBSource, dest_left, dest_top, dest_width, dest_height,
            m_FillArgb, m_Flags)) {
      return FALSE;
    }
  }
  if (m_pRenderStatus->m_bPrint &&
      !(m_pRenderStatus->m_pDevice->GetRenderCaps() & FXRC_BLEND_MODE)) {
    m_Result = FALSE;
    return TRUE;
  }

  FX_RECT clip_box = m_pRenderStatus->m_pDevice->GetClipBox();
  FX_RECT dest_rect = clip_box;
  dest_rect.Intersect(image_rect);
  FX_RECT dest_clip(
      dest_rect.left - image_rect.left, dest_rect.top - image_rect.top,
      dest_rect.right - image_rect.left, dest_rect.bottom - image_rect.top);
  std::unique_ptr<CFX_DIBitmap> pStretched(
      m_pDIBSource->StretchTo(dest_width, dest_height, m_Flags, &dest_clip));
  if (pStretched) {
    m_pRenderStatus->CompositeDIBitmap(pStretched.get(), dest_rect.left,
                                       dest_rect.top, m_FillArgb, m_BitmapAlpha,
                                       m_BlendType, FALSE);
  }
  return FALSE;
}

FX_BOOL CPDF_ImageRenderer::StartBitmapAlpha() {
  if (m_pDIBSource->IsOpaqueImage()) {
    CFX_PathData path;
    path.AppendRect(0, 0, 1, 1);
    path.Transform(&m_ImageMatrix);
    uint32_t fill_color =
        ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha);
    m_pRenderStatus->m_pDevice->DrawPath(&path, nullptr, nullptr, fill_color, 0,
                                         FXFILL_WINDING);
  } else {
    const CFX_DIBSource* pAlphaMask = m_pDIBSource->IsAlphaMask()
                                          ? m_pDIBSource
                                          : m_pDIBSource->GetAlphaMask();
    if (FXSYS_fabs(m_ImageMatrix.b) >= 0.5f ||
        FXSYS_fabs(m_ImageMatrix.c) >= 0.5f) {
      int left, top;
      std::unique_ptr<CFX_DIBitmap> pTransformed(
          pAlphaMask->TransformTo(&m_ImageMatrix, left, top));
      if (!pTransformed)
        return TRUE;

      m_pRenderStatus->m_pDevice->SetBitMask(
          pTransformed.get(), left, top,
          ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha));
    } else {
      CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
      FX_RECT image_rect = image_rect_f.GetOutterRect();
      int dest_width =
          m_ImageMatrix.a > 0 ? image_rect.Width() : -image_rect.Width();
      int dest_height =
          m_ImageMatrix.d > 0 ? -image_rect.Height() : image_rect.Height();
      int left = dest_width > 0 ? image_rect.left : image_rect.right;
      int top = dest_height > 0 ? image_rect.top : image_rect.bottom;
      m_pRenderStatus->m_pDevice->StretchBitMask(
          pAlphaMask, left, top, dest_width, dest_height,
          ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha));
    }
    if (m_pDIBSource != pAlphaMask) {
      delete pAlphaMask;
    }
  }
  return FALSE;
}

FX_BOOL CPDF_ImageRenderer::Continue(IFX_Pause* pPause) {
  if (m_Status == 2) {
    if (m_pTransformer->Continue(pPause))
      return TRUE;

    std::unique_ptr<CFX_DIBitmap> pBitmap(m_pTransformer->DetachBitmap());
    if (!pBitmap)
      return FALSE;

    if (pBitmap->IsAlphaMask()) {
      if (m_BitmapAlpha != 255)
        m_FillArgb = FXARGB_MUL_ALPHA(m_FillArgb, m_BitmapAlpha);
      m_Result = m_pRenderStatus->m_pDevice->SetBitMask(
          pBitmap.get(), m_pTransformer->result().left,
          m_pTransformer->result().top, m_FillArgb);
    } else {
      if (m_BitmapAlpha != 255)
        pBitmap->MultiplyAlpha(m_BitmapAlpha);
      m_Result = m_pRenderStatus->m_pDevice->SetDIBitsWithBlend(
          pBitmap.get(), m_pTransformer->result().left,
          m_pTransformer->result().top, m_BlendType);
    }
    return FALSE;
  }
  if (m_Status == 3)
    return m_pRenderStatus->m_pDevice->ContinueDIBits(m_DeviceHandle, pPause);

  if (m_Status == 4) {
    if (m_Loader.Continue(m_LoadHandle.get(), pPause))
      return TRUE;

    if (StartRenderDIBSource())
      return Continue(pPause);
  }
  return FALSE;
}

CCodec_ScanlineDecoder* FPDFAPI_CreateFlateDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    int nComps,
    int bpc,
    const CPDF_Dictionary* pParams);

CFX_DIBitmap* CPDF_RenderStatus::LoadSMask(CPDF_Dictionary* pSMaskDict,
                                           FX_RECT* pClipRect,
                                           const CFX_Matrix* pMatrix) {
  if (!pSMaskDict)
    return nullptr;

  CPDF_Stream* pGroup = pSMaskDict->GetStreamBy("G");
  if (!pGroup)
    return nullptr;

  std::unique_ptr<CPDF_Function> pFunc;
  CPDF_Object* pFuncObj = pSMaskDict->GetDirectObjectBy("TR");
  if (pFuncObj && (pFuncObj->IsDictionary() || pFuncObj->IsStream()))
    pFunc = CPDF_Function::Load(pFuncObj);

  CFX_Matrix matrix = *pMatrix;
  matrix.TranslateI(-pClipRect->left, -pClipRect->top);

  CPDF_Form form(m_pContext->GetDocument(), m_pContext->GetPageResources(),
                 pGroup);
  form.ParseContent(nullptr, nullptr, nullptr);

  CFX_FxgeDevice bitmap_device;
  FX_BOOL bLuminosity = pSMaskDict->GetStringBy("S") != "Alpha";
  int width = pClipRect->right - pClipRect->left;
  int height = pClipRect->bottom - pClipRect->top;
  FXDIB_Format format;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_ || defined _SKIA_SUPPORT_
  format = bLuminosity ? FXDIB_Rgb32 : FXDIB_8bppMask;
#else
  format = bLuminosity ? FXDIB_Rgb : FXDIB_8bppMask;
#endif
  if (!bitmap_device.Create(width, height, format, nullptr))
    return nullptr;

  CFX_DIBitmap& bitmap = *bitmap_device.GetBitmap();
  CPDF_Object* pCSObj = nullptr;
  CPDF_ColorSpace* pCS = nullptr;
  if (bLuminosity) {
    CPDF_Array* pBC = pSMaskDict->GetArrayBy("BC");
    FX_ARGB back_color = 0xff000000;
    if (pBC) {
      CPDF_Dictionary* pDict = pGroup->GetDict();
      if (pDict && pDict->GetDictBy("Group"))
        pCSObj = pDict->GetDictBy("Group")->GetDirectObjectBy("CS");
      else
        pCSObj = nullptr;
      pCS = m_pContext->GetDocument()->LoadColorSpace(pCSObj);
      if (pCS) {
        FX_FLOAT R, G, B;
        uint32_t comps = 8;
        if (pCS->CountComponents() > comps) {
          comps = pCS->CountComponents();
        }
        CFX_FixedBufGrow<FX_FLOAT, 8> float_array(comps);
        FX_FLOAT* pFloats = float_array;
        FX_SAFE_UINT32 num_floats = comps;
        num_floats *= sizeof(FX_FLOAT);
        if (!num_floats.IsValid()) {
          return nullptr;
        }
        FXSYS_memset(pFloats, 0, num_floats.ValueOrDie());
        size_t count = pBC->GetCount() > 8 ? 8 : pBC->GetCount();
        for (size_t i = 0; i < count; i++) {
          pFloats[i] = pBC->GetNumberAt(i);
        }
        pCS->GetRGB(pFloats, R, G, B);
        back_color = 0xff000000 | ((int32_t)(R * 255) << 16) |
                     ((int32_t)(G * 255) << 8) | (int32_t)(B * 255);
        m_pContext->GetDocument()->GetPageData()->ReleaseColorSpace(pCSObj);
      }
    }
    bitmap.Clear(back_color);
  } else {
    bitmap.Clear(0);
  }
  CPDF_Dictionary* pFormResource = nullptr;
  if (form.m_pFormDict) {
    pFormResource = form.m_pFormDict->GetDictBy("Resources");
  }
  CPDF_RenderOptions options;
  options.m_ColorMode = bLuminosity ? RENDER_COLOR_NORMAL : RENDER_COLOR_ALPHA;
  CPDF_RenderStatus status;
  status.Initialize(m_pContext, &bitmap_device, nullptr, nullptr, nullptr,
                    nullptr, &options, 0, m_bDropObjects, pFormResource, TRUE,
                    nullptr, 0, pCS ? pCS->GetFamily() : 0, bLuminosity);
  status.RenderObjectList(&form, &matrix);
  std::unique_ptr<CFX_DIBitmap> pMask(new CFX_DIBitmap);
  if (!pMask->Create(width, height, FXDIB_8bppMask))
    return nullptr;

  uint8_t* dest_buf = pMask->GetBuffer();
  int dest_pitch = pMask->GetPitch();
  uint8_t* src_buf = bitmap.GetBuffer();
  int src_pitch = bitmap.GetPitch();
  std::vector<uint8_t> transfers(256);
  if (pFunc) {
    CFX_FixedBufGrow<FX_FLOAT, 16> results(pFunc->CountOutputs());
    for (int i = 0; i < 256; i++) {
      FX_FLOAT input = (FX_FLOAT)i / 255.0f;
      int nresult;
      pFunc->Call(&input, 1, results, nresult);
      transfers[i] = FXSYS_round(results[0] * 255);
    }
  } else {
    for (int i = 0; i < 256; i++) {
      transfers[i] = i;
    }
  }
  if (bLuminosity) {
    int Bpp = bitmap.GetBPP() / 8;
    for (int row = 0; row < height; row++) {
      uint8_t* dest_pos = dest_buf + row * dest_pitch;
      uint8_t* src_pos = src_buf + row * src_pitch;
      for (int col = 0; col < width; col++) {
        *dest_pos++ = transfers[FXRGB2GRAY(src_pos[2], src_pos[1], *src_pos)];
        src_pos += Bpp;
      }
    }
  } else if (pFunc) {
    int size = dest_pitch * height;
    for (int i = 0; i < size; i++) {
      dest_buf[i] = transfers[src_buf[i]];
    }
  } else {
    FXSYS_memcpy(dest_buf, src_buf, dest_pitch * height);
  }
  return pMask.release();
}
