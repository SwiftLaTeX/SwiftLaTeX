// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/include/fx_codec.h"

#include <cmath>
#include <memory>
#include <utility>

#include "core/fxcodec/codec/codec_int.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "third_party/base/logging.h"

CCodec_ModuleMgr::CCodec_ModuleMgr()
    : m_pBasicModule(new CCodec_BasicModule),
      m_pFaxModule(new CCodec_FaxModule),
      m_pJpegModule(new CCodec_JpegModule),
      m_pJpxModule(new CCodec_JpxModule),
      m_pJbig2Module(new CCodec_Jbig2Module),
      m_pIccModule(new CCodec_IccModule),
#ifdef PDF_ENABLE_XFA
      m_pPngModule(new CCodec_PngModule),
      m_pGifModule(new CCodec_GifModule),
      m_pBmpModule(new CCodec_BmpModule),
      m_pTiffModule(new CCodec_TiffModule),
#endif  // PDF_ENABLE_XFA
      m_pFlateModule(new CCodec_FlateModule) {
}

CCodec_ModuleMgr::~CCodec_ModuleMgr() {}

CCodec_ScanlineDecoder::CCodec_ScanlineDecoder()
    : m_NextLine(-1), m_pLastScanline(nullptr) {}

CCodec_ScanlineDecoder::~CCodec_ScanlineDecoder() {}

const uint8_t* CCodec_ScanlineDecoder::GetScanline(int line) {
  if (m_NextLine == line + 1)
    return m_pLastScanline;

  if (m_NextLine < 0 || m_NextLine > line) {
    if (!v_Rewind())
      return nullptr;
    m_NextLine = 0;
  }
  while (m_NextLine < line) {
    ReadNextLine();
    m_NextLine++;
  }
  m_pLastScanline = ReadNextLine();
  m_NextLine++;
  return m_pLastScanline;
}

FX_BOOL CCodec_ScanlineDecoder::SkipToScanline(int line, IFX_Pause* pPause) {
  if (m_NextLine == line || m_NextLine == line + 1)
    return FALSE;

  if (m_NextLine < 0 || m_NextLine > line) {
    v_Rewind();
    m_NextLine = 0;
  }
  m_pLastScanline = nullptr;
  while (m_NextLine < line) {
    m_pLastScanline = ReadNextLine();
    m_NextLine++;
    if (pPause && pPause->NeedToPauseNow()) {
      return TRUE;
    }
  }
  return FALSE;
}

uint8_t* CCodec_ScanlineDecoder::ReadNextLine() {
  return v_GetNextLine();
}

#ifdef PDF_ENABLE_XFA
CFX_DIBAttribute::CFX_DIBAttribute()
    : m_nXDPI(-1),
      m_nYDPI(-1),
      m_fAspectRatio(-1.0f),
      m_wDPIUnit(0),
      m_nGifLeft(0),
      m_nGifTop(0),
      m_pGifLocalPalette(nullptr),
      m_nGifLocalPalNum(0),
      m_nBmpCompressType(0) {
  FXSYS_memset(m_strTime, 0, sizeof(m_strTime));
}
CFX_DIBAttribute::~CFX_DIBAttribute() {
  for (const auto& pair : m_Exif)
    FX_Free(pair.second);
}
#endif  // PDF_ENABLE_XFA

class CCodec_RLScanlineDecoder : public CCodec_ScanlineDecoder {
 public:
  CCodec_RLScanlineDecoder();
  ~CCodec_RLScanlineDecoder() override;

  FX_BOOL Create(const uint8_t* src_buf,
                 uint32_t src_size,
                 int width,
                 int height,
                 int nComps,
                 int bpc);

  // CCodec_ScanlineDecoder
  FX_BOOL v_Rewind() override;
  uint8_t* v_GetNextLine() override;
  uint32_t GetSrcOffset() override { return m_SrcOffset; }

 protected:
  FX_BOOL CheckDestSize();
  void GetNextOperator();
  void UpdateOperator(uint8_t used_bytes);

  uint8_t* m_pScanline;
  const uint8_t* m_pSrcBuf;
  uint32_t m_SrcSize;
  uint32_t m_dwLineBytes;
  uint32_t m_SrcOffset;
  FX_BOOL m_bEOD;
  uint8_t m_Operator;
};
CCodec_RLScanlineDecoder::CCodec_RLScanlineDecoder()
    : m_pScanline(nullptr),
      m_pSrcBuf(nullptr),
      m_SrcSize(0),
      m_dwLineBytes(0),
      m_SrcOffset(0),
      m_bEOD(FALSE),
      m_Operator(0) {}
CCodec_RLScanlineDecoder::~CCodec_RLScanlineDecoder() {
  FX_Free(m_pScanline);
}
FX_BOOL CCodec_RLScanlineDecoder::CheckDestSize() {
  uint32_t i = 0;
  uint32_t old_size = 0;
  uint32_t dest_size = 0;
  while (i < m_SrcSize) {
    if (m_pSrcBuf[i] < 128) {
      old_size = dest_size;
      dest_size += m_pSrcBuf[i] + 1;
      if (dest_size < old_size) {
        return FALSE;
      }
      i += m_pSrcBuf[i] + 2;
    } else if (m_pSrcBuf[i] > 128) {
      old_size = dest_size;
      dest_size += 257 - m_pSrcBuf[i];
      if (dest_size < old_size) {
        return FALSE;
      }
      i += 2;
    } else {
      break;
    }
  }
  if (((uint32_t)m_OrigWidth * m_nComps * m_bpc * m_OrigHeight + 7) / 8 >
      dest_size) {
    return FALSE;
  }
  return TRUE;
}
FX_BOOL CCodec_RLScanlineDecoder::Create(const uint8_t* src_buf,
                                         uint32_t src_size,
                                         int width,
                                         int height,
                                         int nComps,
                                         int bpc) {
  m_pSrcBuf = src_buf;
  m_SrcSize = src_size;
  m_OutputWidth = m_OrigWidth = width;
  m_OutputHeight = m_OrigHeight = height;
  m_nComps = nComps;
  m_bpc = bpc;
  // Aligning the pitch to 4 bytes requires an integer overflow check.
  FX_SAFE_UINT32 pitch = width;
  pitch *= nComps;
  pitch *= bpc;
  pitch += 31;
  pitch /= 32;
  pitch *= 4;
  if (!pitch.IsValid()) {
    return FALSE;
  }
  m_Pitch = pitch.ValueOrDie();
  // Overflow should already have been checked before this is called.
  m_dwLineBytes = (static_cast<uint32_t>(width) * nComps * bpc + 7) / 8;
  m_pScanline = FX_Alloc(uint8_t, m_Pitch);
  return CheckDestSize();
}
FX_BOOL CCodec_RLScanlineDecoder::v_Rewind() {
  FXSYS_memset(m_pScanline, 0, m_Pitch);
  m_SrcOffset = 0;
  m_bEOD = FALSE;
  m_Operator = 0;
  return TRUE;
}
uint8_t* CCodec_RLScanlineDecoder::v_GetNextLine() {
  if (m_SrcOffset == 0) {
    GetNextOperator();
  } else {
    if (m_bEOD) {
      return nullptr;
    }
  }
  FXSYS_memset(m_pScanline, 0, m_Pitch);
  uint32_t col_pos = 0;
  FX_BOOL eol = FALSE;
  while (m_SrcOffset < m_SrcSize && !eol) {
    if (m_Operator < 128) {
      uint32_t copy_len = m_Operator + 1;
      if (col_pos + copy_len >= m_dwLineBytes) {
        copy_len = m_dwLineBytes - col_pos;
        eol = TRUE;
      }
      if (copy_len >= m_SrcSize - m_SrcOffset) {
        copy_len = m_SrcSize - m_SrcOffset;
        m_bEOD = TRUE;
      }
      FXSYS_memcpy(m_pScanline + col_pos, m_pSrcBuf + m_SrcOffset, copy_len);
      col_pos += copy_len;
      UpdateOperator((uint8_t)copy_len);
    } else if (m_Operator > 128) {
      int fill = 0;
      if (m_SrcOffset - 1 < m_SrcSize - 1) {
        fill = m_pSrcBuf[m_SrcOffset];
      }
      uint32_t duplicate_len = 257 - m_Operator;
      if (col_pos + duplicate_len >= m_dwLineBytes) {
        duplicate_len = m_dwLineBytes - col_pos;
        eol = TRUE;
      }
      FXSYS_memset(m_pScanline + col_pos, fill, duplicate_len);
      col_pos += duplicate_len;
      UpdateOperator((uint8_t)duplicate_len);
    } else {
      m_bEOD = TRUE;
      break;
    }
  }
  return m_pScanline;
}
void CCodec_RLScanlineDecoder::GetNextOperator() {
  if (m_SrcOffset >= m_SrcSize) {
    m_Operator = 128;
    return;
  }
  m_Operator = m_pSrcBuf[m_SrcOffset];
  m_SrcOffset++;
}
void CCodec_RLScanlineDecoder::UpdateOperator(uint8_t used_bytes) {
  if (used_bytes == 0) {
    return;
  }
  if (m_Operator < 128) {
    ASSERT((uint32_t)m_Operator + 1 >= used_bytes);
    if (used_bytes == m_Operator + 1) {
      m_SrcOffset += used_bytes;
      GetNextOperator();
      return;
    }
    m_Operator -= used_bytes;
    m_SrcOffset += used_bytes;
    if (m_SrcOffset >= m_SrcSize) {
      m_Operator = 128;
    }
    return;
  }
  uint8_t count = 257 - m_Operator;
  ASSERT((uint32_t)count >= used_bytes);
  if (used_bytes == count) {
    m_SrcOffset++;
    GetNextOperator();
    return;
  }
  count -= used_bytes;
  m_Operator = 257 - count;
}

CCodec_ScanlineDecoder* CCodec_BasicModule::CreateRunLengthDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    int nComps,
    int bpc) {
  std::unique_ptr<CCodec_RLScanlineDecoder> pRLScanlineDecoder(
      new CCodec_RLScanlineDecoder);
  if (!pRLScanlineDecoder->Create(src_buf, src_size, width, height, nComps,
                                  bpc)) {
    return nullptr;
  }

  return pRLScanlineDecoder.release();
}
