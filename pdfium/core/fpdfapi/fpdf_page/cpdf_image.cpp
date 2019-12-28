// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/cpdf_boolean.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/fpdf_render/cpdf_pagerendercache.h"
#include "core/fpdfapi/fpdf_render/render_int.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxge/include/fx_dib.h"

CPDF_Image::CPDF_Image(CPDF_Document* pDoc)
    : m_pDIBSource(nullptr),
      m_pMask(nullptr),
      m_MatteColor(0),
      m_pStream(nullptr),
      m_bInline(FALSE),
      m_pInlineDict(nullptr),
      m_pDocument(pDoc),
      m_pOC(nullptr) {}

CPDF_Image::~CPDF_Image() {
  if (m_bInline) {
    if (m_pStream)
      m_pStream->Release();
    if (m_pInlineDict)
      m_pInlineDict->Release();
  }
}

void CPDF_Image::Release() {
  if (m_bInline || (m_pStream && m_pStream->GetObjNum() == 0))
    delete this;
}

CPDF_Image* CPDF_Image::Clone() {
  if (m_pStream->GetObjNum())
    return m_pDocument->GetPageData()->GetImage(m_pStream);

  CPDF_Image* pImage = new CPDF_Image(m_pDocument);
  pImage->LoadImageF(ToStream(m_pStream->Clone()), m_bInline);
  if (m_bInline)
    pImage->SetInlineDict(ToDictionary(m_pInlineDict->Clone(TRUE)));

  return pImage;
}

FX_BOOL CPDF_Image::LoadImageF(CPDF_Stream* pStream, FX_BOOL bInline) {
  m_pStream = pStream;
  if (m_bInline && m_pInlineDict) {
    m_pInlineDict->Release();
    m_pInlineDict = nullptr;
  }
  m_bInline = bInline;
  CPDF_Dictionary* pDict = pStream->GetDict();
  if (m_bInline) {
    m_pInlineDict = ToDictionary(pDict->Clone());
  }
  m_pOC = pDict->GetDictBy("OC");
  m_bIsMask =
      !pDict->KeyExist("ColorSpace") || pDict->GetIntegerBy("ImageMask");
  m_bInterpolate = pDict->GetIntegerBy("Interpolate");
  m_Height = pDict->GetIntegerBy("Height");
  m_Width = pDict->GetIntegerBy("Width");
  return TRUE;
}

CPDF_Dictionary* CPDF_Image::InitJPEG(uint8_t* pData, uint32_t size) {
  int32_t width;
  int32_t height;
  int32_t num_comps;
  int32_t bits;
  bool color_trans;
  if (!CPDF_ModuleMgr::Get()->GetJpegModule()->LoadInfo(
          pData, size, &width, &height, &num_comps, &bits, &color_trans)) {
    return nullptr;
  }

  CPDF_Dictionary* pDict = new CPDF_Dictionary;
  pDict->SetAtName("Type", "XObject");
  pDict->SetAtName("Subtype", "Image");
  pDict->SetAtInteger("Width", width);
  pDict->SetAtInteger("Height", height);
  const FX_CHAR* csname = nullptr;
  if (num_comps == 1) {
    csname = "DeviceGray";
  } else if (num_comps == 3) {
    csname = "DeviceRGB";
  } else if (num_comps == 4) {
    csname = "DeviceCMYK";
    CPDF_Array* pDecode = new CPDF_Array;
    for (int n = 0; n < 4; n++) {
      pDecode->AddInteger(1);
      pDecode->AddInteger(0);
    }
    pDict->SetAt("Decode", pDecode);
  }
  pDict->SetAtName("ColorSpace", csname);
  pDict->SetAtInteger("BitsPerComponent", bits);
  pDict->SetAtName("Filter", "DCTDecode");
  if (!color_trans) {
    CPDF_Dictionary* pParms = new CPDF_Dictionary;
    pDict->SetAt("DecodeParms", pParms);
    pParms->SetAtInteger("ColorTransform", 0);
  }
  m_bIsMask = FALSE;
  m_Width = width;
  m_Height = height;
  if (!m_pStream)
    m_pStream = new CPDF_Stream(nullptr, 0, nullptr);
  return pDict;
}

void CPDF_Image::SetJpegImage(uint8_t* pData, uint32_t size) {
  CPDF_Dictionary* pDict = InitJPEG(pData, size);
  if (!pDict) {
    return;
  }
  m_pStream->InitStream(pData, size, pDict);
}

void CPDF_Image::SetJpegImage(IFX_FileRead* pFile) {
  uint32_t size = (uint32_t)pFile->GetSize();
  if (!size) {
    return;
  }
  uint32_t dwEstimateSize = size;
  if (dwEstimateSize > 8192) {
    dwEstimateSize = 8192;
  }
  uint8_t* pData = FX_Alloc(uint8_t, dwEstimateSize);
  pFile->ReadBlock(pData, 0, dwEstimateSize);
  CPDF_Dictionary* pDict = InitJPEG(pData, dwEstimateSize);
  FX_Free(pData);
  if (!pDict && size > dwEstimateSize) {
    pData = FX_Alloc(uint8_t, size);
    pFile->ReadBlock(pData, 0, size);
    pDict = InitJPEG(pData, size);
    FX_Free(pData);
  }
  if (!pDict) {
    return;
  }
  m_pStream->InitStreamFromFile(pFile, pDict);
}

void CPDF_Image::SetImage(const CFX_DIBitmap* pBitmap, int32_t iCompress) {
  int32_t BitmapWidth = pBitmap->GetWidth();
  int32_t BitmapHeight = pBitmap->GetHeight();
  if (BitmapWidth < 1 || BitmapHeight < 1) {
    return;
  }
  uint8_t* src_buf = pBitmap->GetBuffer();
  int32_t src_pitch = pBitmap->GetPitch();
  int32_t bpp = pBitmap->GetBPP();

  CPDF_Dictionary* pDict = new CPDF_Dictionary;
  pDict->SetAtName("Type", "XObject");
  pDict->SetAtName("Subtype", "Image");
  pDict->SetAtInteger("Width", BitmapWidth);
  pDict->SetAtInteger("Height", BitmapHeight);
  uint8_t* dest_buf = nullptr;
  FX_STRSIZE dest_pitch = 0, dest_size = 0, opType = -1;
  if (bpp == 1) {
    int32_t reset_a = 0, reset_r = 0, reset_g = 0, reset_b = 0;
    int32_t set_a = 0, set_r = 0, set_g = 0, set_b = 0;
    if (!pBitmap->IsAlphaMask()) {
      ArgbDecode(pBitmap->GetPaletteArgb(0), reset_a, reset_r, reset_g,
                 reset_b);
      ArgbDecode(pBitmap->GetPaletteArgb(1), set_a, set_r, set_g, set_b);
    }
    if (set_a == 0 || reset_a == 0) {
      pDict->SetAt("ImageMask", new CPDF_Boolean(TRUE));
      if (reset_a == 0) {
        CPDF_Array* pArray = new CPDF_Array;
        pArray->AddInteger(1);
        pArray->AddInteger(0);
        pDict->SetAt("Decode", pArray);
      }
    } else {
      CPDF_Array* pCS = new CPDF_Array;
      pCS->AddName("Indexed");
      pCS->AddName("DeviceRGB");
      pCS->AddInteger(1);
      CFX_ByteString ct;
      FX_CHAR* pBuf = ct.GetBuffer(6);
      pBuf[0] = (FX_CHAR)reset_r;
      pBuf[1] = (FX_CHAR)reset_g;
      pBuf[2] = (FX_CHAR)reset_b;
      pBuf[3] = (FX_CHAR)set_r;
      pBuf[4] = (FX_CHAR)set_g;
      pBuf[5] = (FX_CHAR)set_b;
      ct.ReleaseBuffer(6);
      pCS->Add(new CPDF_String(ct, TRUE));
      pDict->SetAt("ColorSpace", pCS);
    }
    pDict->SetAtInteger("BitsPerComponent", 1);
    dest_pitch = (BitmapWidth + 7) / 8;
    if ((iCompress & 0x03) == PDF_IMAGE_NO_COMPRESS) {
      opType = 1;
    } else {
      opType = 0;
    }
  } else if (bpp == 8) {
    int32_t iPalette = pBitmap->GetPaletteSize();
    if (iPalette > 0) {
      CPDF_Array* pCS = new CPDF_Array;
      m_pDocument->AddIndirectObject(pCS);
      pCS->AddName("Indexed");
      pCS->AddName("DeviceRGB");
      pCS->AddInteger(iPalette - 1);
      uint8_t* pColorTable = FX_Alloc2D(uint8_t, iPalette, 3);
      uint8_t* ptr = pColorTable;
      for (int32_t i = 0; i < iPalette; i++) {
        uint32_t argb = pBitmap->GetPaletteArgb(i);
        ptr[0] = (uint8_t)(argb >> 16);
        ptr[1] = (uint8_t)(argb >> 8);
        ptr[2] = (uint8_t)argb;
        ptr += 3;
      }
      CPDF_Stream* pCTS =
          new CPDF_Stream(pColorTable, iPalette * 3, new CPDF_Dictionary);
      m_pDocument->AddIndirectObject(pCTS);
      pCS->AddReference(m_pDocument, pCTS);
      pDict->SetAtReference("ColorSpace", m_pDocument, pCS);
    } else {
      pDict->SetAtName("ColorSpace", "DeviceGray");
    }
    pDict->SetAtInteger("BitsPerComponent", 8);
    if ((iCompress & 0x03) == PDF_IMAGE_NO_COMPRESS) {
      dest_pitch = BitmapWidth;
      opType = 1;
    } else {
      opType = 0;
    }
  } else {
    pDict->SetAtName("ColorSpace", "DeviceRGB");
    pDict->SetAtInteger("BitsPerComponent", 8);
    if ((iCompress & 0x03) == PDF_IMAGE_NO_COMPRESS) {
      dest_pitch = BitmapWidth * 3;
      opType = 2;
    } else {
      opType = 0;
    }
  }
  const CFX_DIBitmap* pMaskBitmap = nullptr;
  FX_BOOL bDeleteMask = FALSE;
  if (pBitmap->HasAlpha()) {
    pMaskBitmap = pBitmap->GetAlphaMask();
    bDeleteMask = TRUE;
  }
  if (pMaskBitmap) {
    int32_t maskWidth = pMaskBitmap->GetWidth();
    int32_t maskHeight = pMaskBitmap->GetHeight();
    uint8_t* mask_buf = nullptr;
    FX_STRSIZE mask_size = 0;
    CPDF_Dictionary* pMaskDict = new CPDF_Dictionary;
    pMaskDict->SetAtName("Type", "XObject");
    pMaskDict->SetAtName("Subtype", "Image");
    pMaskDict->SetAtInteger("Width", maskWidth);
    pMaskDict->SetAtInteger("Height", maskHeight);
    pMaskDict->SetAtName("ColorSpace", "DeviceGray");
    pMaskDict->SetAtInteger("BitsPerComponent", 8);
    if (pMaskBitmap->GetBPP() == 8 &&
        (iCompress & PDF_IMAGE_MASK_LOSSY_COMPRESS) != 0) {
    } else if (pMaskBitmap->GetFormat() == FXDIB_1bppMask) {
    } else {
      mask_buf = FX_Alloc2D(uint8_t, maskHeight, maskWidth);
      mask_size = maskHeight * maskWidth;  // Safe since checked alloc returned.
      for (int32_t a = 0; a < maskHeight; a++) {
        FXSYS_memcpy(mask_buf + a * maskWidth, pMaskBitmap->GetScanline(a),
                     maskWidth);
      }
    }
    pMaskDict->SetAtInteger("Length", mask_size);

    CPDF_Stream* pMaskStream = new CPDF_Stream(mask_buf, mask_size, pMaskDict);
    m_pDocument->AddIndirectObject(pMaskStream);
    pDict->SetAtReference("SMask", m_pDocument, pMaskStream);
    if (bDeleteMask) {
      delete pMaskBitmap;
    }
  }
  if (opType == 0) {
    if (iCompress & PDF_IMAGE_LOSSLESS_COMPRESS) {
    } else {
      if (pBitmap->GetBPP() == 1) {
      } else if (pBitmap->GetBPP() >= 8 && pBitmap->GetPalette()) {
        CFX_DIBitmap* pNewBitmap = new CFX_DIBitmap();
        pNewBitmap->Copy(pBitmap);
        pNewBitmap->ConvertFormat(FXDIB_Rgb);
        SetImage(pNewBitmap, iCompress);
        if (pDict) {
          pDict->Release();
          pDict = nullptr;
        }
        FX_Free(dest_buf);
        dest_buf = nullptr;
        dest_size = 0;
        delete pNewBitmap;
        return;
      }
    }
  } else if (opType == 1) {
    dest_buf = FX_Alloc2D(uint8_t, dest_pitch, BitmapHeight);
    dest_size = dest_pitch * BitmapHeight;  // Safe as checked alloc returned.

    uint8_t* pDest = dest_buf;
    for (int32_t i = 0; i < BitmapHeight; i++) {
      FXSYS_memcpy(pDest, src_buf, dest_pitch);
      pDest += dest_pitch;
      src_buf += src_pitch;
    }
  } else if (opType == 2) {
    dest_buf = FX_Alloc2D(uint8_t, dest_pitch, BitmapHeight);
    dest_size = dest_pitch * BitmapHeight;  // Safe as checked alloc returned.

    uint8_t* pDest = dest_buf;
    int32_t src_offset = 0;
    int32_t dest_offset = 0;
    for (int32_t row = 0; row < BitmapHeight; row++) {
      src_offset = row * src_pitch;
      for (int32_t column = 0; column < BitmapWidth; column++) {
        FX_FLOAT alpha = 1;
        pDest[dest_offset] = (uint8_t)(src_buf[src_offset + 2] * alpha);
        pDest[dest_offset + 1] = (uint8_t)(src_buf[src_offset + 1] * alpha);
        pDest[dest_offset + 2] = (uint8_t)(src_buf[src_offset] * alpha);
        dest_offset += 3;
        src_offset += bpp == 24 ? 3 : 4;
      }

      pDest += dest_pitch;
      dest_offset = 0;
    }
  }
  if (!m_pStream) {
    m_pStream = new CPDF_Stream(nullptr, 0, nullptr);
  }
  m_pStream->InitStream(dest_buf, dest_size, pDict);
  m_bIsMask = pBitmap->IsAlphaMask();
  m_Width = BitmapWidth;
  m_Height = BitmapHeight;
  FX_Free(dest_buf);
}

void CPDF_Image::ResetCache(CPDF_Page* pPage, const CFX_DIBitmap* pBitmap) {
  pPage->GetRenderCache()->ResetBitmap(m_pStream, pBitmap);
}

CFX_DIBSource* CPDF_Image::LoadDIBSource(CFX_DIBSource** ppMask,
                                         uint32_t* pMatteColor,
                                         FX_BOOL bStdCS,
                                         uint32_t GroupFamily,
                                         FX_BOOL bLoadMask) const {
  std::unique_ptr<CPDF_DIBSource> source(new CPDF_DIBSource);
  if (source->Load(m_pDocument, m_pStream,
                   reinterpret_cast<CPDF_DIBSource**>(ppMask), pMatteColor,
                   nullptr, nullptr, bStdCS, GroupFamily, bLoadMask)) {
    return source.release();
  }
  return nullptr;
}

CFX_DIBSource* CPDF_Image::DetachBitmap() {
  CFX_DIBSource* pBitmap = m_pDIBSource;
  m_pDIBSource = nullptr;
  return pBitmap;
}

CFX_DIBSource* CPDF_Image::DetachMask() {
  CFX_DIBSource* pBitmap = m_pMask;
  m_pMask = nullptr;
  return pBitmap;
}

FX_BOOL CPDF_Image::StartLoadDIBSource(CPDF_Dictionary* pFormResource,
                                       CPDF_Dictionary* pPageResource,
                                       FX_BOOL bStdCS,
                                       uint32_t GroupFamily,
                                       FX_BOOL bLoadMask) {
  std::unique_ptr<CPDF_DIBSource> source(new CPDF_DIBSource);
  int ret =
      source->StartLoadDIBSource(m_pDocument, m_pStream, TRUE, pFormResource,
                                 pPageResource, bStdCS, GroupFamily, bLoadMask);
  if (ret == 2) {
    m_pDIBSource = source.release();
    return TRUE;
  }
  if (!ret) {
    m_pDIBSource = nullptr;
    return FALSE;
  }
  m_pMask = source->DetachMask();
  m_MatteColor = source->GetMatteColor();
  m_pDIBSource = source.release();
  return FALSE;
}

FX_BOOL CPDF_Image::Continue(IFX_Pause* pPause) {
  CPDF_DIBSource* pSource = static_cast<CPDF_DIBSource*>(m_pDIBSource);
  int ret = pSource->ContinueLoadDIBSource(pPause);
  if (ret == 2) {
    return TRUE;
  }
  if (!ret) {
    delete m_pDIBSource;
    m_pDIBSource = nullptr;
    return FALSE;
  }
  m_pMask = pSource->DetachMask();
  m_MatteColor = pSource->GetMatteColor();
  return FALSE;
}
