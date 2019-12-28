// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_IMAGE_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_IMAGE_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fxcrt/include/fx_system.h"

#define PDF_IMAGE_NO_COMPRESS 0x0000
#define PDF_IMAGE_LOSSY_COMPRESS 0x0001
#define PDF_IMAGE_LOSSLESS_COMPRESS 0x0002
#define PDF_IMAGE_MASK_LOSSY_COMPRESS 0x0004
#define PDF_IMAGE_MASK_LOSSLESS_COMPRESS 0x0008

class CFX_DIBitmap;
class CFX_DIBSource;
class CPDF_Dictionay;
class CPDF_Document;
class CPDF_Page;
class IFX_FileRead;
class IFX_FileWrite;
class IFX_Pause;

class CPDF_Image {
 public:
  explicit CPDF_Image(CPDF_Document* pDoc);
  ~CPDF_Image();

  FX_BOOL LoadImageF(CPDF_Stream* pImageStream, FX_BOOL bInline);

  void Release();

  CPDF_Image* Clone();

  CPDF_Dictionary* GetInlineDict() const { return m_pInlineDict; }
  CPDF_Stream* GetStream() const { return m_pStream; }
  CPDF_Dictionary* GetDict() const {
    return m_pStream ? m_pStream->GetDict() : nullptr;
  }
  CPDF_Dictionary* GetOC() const { return m_pOC; }
  CPDF_Document* GetDocument() const { return m_pDocument; }

  int32_t GetPixelHeight() const { return m_Height; }
  int32_t GetPixelWidth() const { return m_Width; }

  FX_BOOL IsInline() { return m_bInline; }
  FX_BOOL IsMask() const { return m_bIsMask; }
  FX_BOOL IsInterpol() const { return m_bInterpolate; }

  CFX_DIBSource* LoadDIBSource(CFX_DIBSource** ppMask = nullptr,
                               uint32_t* pMatteColor = nullptr,
                               FX_BOOL bStdCS = FALSE,
                               uint32_t GroupFamily = 0,
                               FX_BOOL bLoadMask = FALSE) const;

  void SetInlineDict(CPDF_Dictionary* pDict) { m_pInlineDict = pDict; }
  void SetImage(const CFX_DIBitmap* pDIBitmap, int32_t iCompress);
  void SetJpegImage(uint8_t* pImageData, uint32_t size);
  void SetJpegImage(IFX_FileRead* pFile);

  void ResetCache(CPDF_Page* pPage, const CFX_DIBitmap* pDIBitmap);

  FX_BOOL StartLoadDIBSource(CPDF_Dictionary* pFormResource,
                             CPDF_Dictionary* pPageResource,
                             FX_BOOL bStdCS = FALSE,
                             uint32_t GroupFamily = 0,
                             FX_BOOL bLoadMask = FALSE);
  FX_BOOL Continue(IFX_Pause* pPause);
  CFX_DIBSource* DetachBitmap();
  CFX_DIBSource* DetachMask();

  CFX_DIBSource* m_pDIBSource;
  CFX_DIBSource* m_pMask;
  uint32_t m_MatteColor;

 private:
  CPDF_Dictionary* InitJPEG(uint8_t* pData, uint32_t size);

  CPDF_Stream* m_pStream;
  FX_BOOL m_bInline;
  CPDF_Dictionary* m_pInlineDict;
  int32_t m_Height;
  int32_t m_Width;
  FX_BOOL m_bIsMask;
  FX_BOOL m_bInterpolate;
  CPDF_Document* m_pDocument;
  CPDF_Dictionary* m_pOC;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_IMAGE_H_
