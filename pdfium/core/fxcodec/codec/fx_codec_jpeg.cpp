// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <setjmp.h>

#include "core/fxcodec/codec/codec_int.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "core/fxge/include/fx_dib.h"

extern "C" {
#undef FAR
#if defined(USE_SYSTEM_LIBJPEG)
#include <jpeglib.h>
#elif defined(USE_LIBJPEG_TURBO)
#include "third_party/libjpeg_turbo/jpeglib.h"
#else
#include "third_party/libjpeg/jpeglib.h"
#endif
}

extern "C" {
static void JpegScanSOI(const uint8_t** src_buf, uint32_t* src_size) {
  if (*src_size == 0)
    return;

  uint32_t offset = 0;
  while (offset < *src_size - 1) {
    if ((*src_buf)[offset] == 0xff && (*src_buf)[offset + 1] == 0xd8) {
      *src_buf += offset;
      *src_size -= offset;
      return;
    }
    offset++;
  }
}
};
extern "C" {
static void _src_do_nothing(struct jpeg_decompress_struct* cinfo) {}
};
extern "C" {
static void _error_fatal(j_common_ptr cinfo) {
  longjmp(*(jmp_buf*)cinfo->client_data, -1);
}
};
extern "C" {
static void _src_skip_data(struct jpeg_decompress_struct* cinfo, long num) {
  if (num > (long)cinfo->src->bytes_in_buffer) {
    _error_fatal((j_common_ptr)cinfo);
  }
  cinfo->src->next_input_byte += num;
  cinfo->src->bytes_in_buffer -= num;
}
};
extern "C" {
static boolean _src_fill_buffer(j_decompress_ptr cinfo) {
  return 0;
}
};
extern "C" {
static boolean _src_resync(j_decompress_ptr cinfo, int desired) {
  return 0;
}
};
extern "C" {
static void _error_do_nothing(j_common_ptr cinfo) {}
};
extern "C" {
static void _error_do_nothing1(j_common_ptr cinfo, int) {}
};
extern "C" {
static void _error_do_nothing2(j_common_ptr cinfo, char*) {}
};

#define JPEG_MARKER_ICC (JPEG_APP0 + 2)
#define JPEG_MARKER_MAXSIZE 0xFFFF

#ifdef PDF_ENABLE_XFA
static void JpegLoadAttribute(struct jpeg_decompress_struct* pInfo,
                              CFX_DIBAttribute* pAttribute) {
  if (!pAttribute)
    return;

  pAttribute->m_nXDPI = pInfo->X_density;
  pAttribute->m_nYDPI = pInfo->Y_density;
  pAttribute->m_wDPIUnit = pInfo->density_unit;
}
#endif  // PDF_ENABLE_XFA

static bool JpegLoadInfo(const uint8_t* src_buf,
                         uint32_t src_size,
                         int* width,
                         int* height,
                         int* num_components,
                         int* bits_per_components,
                         bool* color_transform) {
  JpegScanSOI(&src_buf, &src_size);
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  jerr.error_exit = _error_fatal;
  jerr.emit_message = _error_do_nothing1;
  jerr.output_message = _error_do_nothing;
  jerr.format_message = _error_do_nothing2;
  jerr.reset_error_mgr = _error_do_nothing;
  jerr.trace_level = 0;
  cinfo.err = &jerr;
  jmp_buf mark;
  cinfo.client_data = &mark;
  if (setjmp(mark) == -1)
    return false;

  jpeg_create_decompress(&cinfo);
  struct jpeg_source_mgr src;
  src.init_source = _src_do_nothing;
  src.term_source = _src_do_nothing;
  src.skip_input_data = _src_skip_data;
  src.fill_input_buffer = _src_fill_buffer;
  src.resync_to_restart = _src_resync;
  src.bytes_in_buffer = src_size;
  src.next_input_byte = src_buf;
  cinfo.src = &src;
  if (setjmp(mark) == -1) {
    jpeg_destroy_decompress(&cinfo);
    return false;
  }
  int ret = jpeg_read_header(&cinfo, TRUE);
  if (ret != JPEG_HEADER_OK) {
    jpeg_destroy_decompress(&cinfo);
    return false;
  }
  *width = cinfo.image_width;
  *height = cinfo.image_height;
  *num_components = cinfo.num_components;
  *color_transform =
      cinfo.jpeg_color_space == JCS_YCbCr || cinfo.jpeg_color_space == JCS_YCCK;
  *bits_per_components = cinfo.data_precision;
  jpeg_destroy_decompress(&cinfo);
  return true;
}

class CCodec_JpegDecoder : public CCodec_ScanlineDecoder {
 public:
  CCodec_JpegDecoder();
  ~CCodec_JpegDecoder() override;

  FX_BOOL Create(const uint8_t* src_buf,
                 uint32_t src_size,
                 int width,
                 int height,
                 int nComps,
                 FX_BOOL ColorTransform);

  // CCodec_ScanlineDecoder
  FX_BOOL v_Rewind() override;
  uint8_t* v_GetNextLine() override;
  uint32_t GetSrcOffset() override;

  FX_BOOL InitDecode();

  jmp_buf m_JmpBuf;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  struct jpeg_source_mgr src;
  const uint8_t* m_SrcBuf;
  uint32_t m_SrcSize;
  uint8_t* m_pScanlineBuf;

  FX_BOOL m_bInited;
  FX_BOOL m_bStarted;
  FX_BOOL m_bJpegTransform;

 protected:
  uint32_t m_nDefaultScaleDenom;
};

CCodec_JpegDecoder::CCodec_JpegDecoder() {
  m_pScanlineBuf = nullptr;
  m_bStarted = FALSE;
  m_bInited = FALSE;
  FXSYS_memset(&cinfo, 0, sizeof(cinfo));
  FXSYS_memset(&jerr, 0, sizeof(jerr));
  FXSYS_memset(&src, 0, sizeof(src));
  m_nDefaultScaleDenom = 1;
}

CCodec_JpegDecoder::~CCodec_JpegDecoder() {
  FX_Free(m_pScanlineBuf);
  if (m_bInited)
    jpeg_destroy_decompress(&cinfo);
}

FX_BOOL CCodec_JpegDecoder::InitDecode() {
  cinfo.err = &jerr;
  cinfo.client_data = &m_JmpBuf;
  if (setjmp(m_JmpBuf) == -1)
    return FALSE;

  jpeg_create_decompress(&cinfo);
  m_bInited = TRUE;
  cinfo.src = &src;
  src.bytes_in_buffer = m_SrcSize;
  src.next_input_byte = m_SrcBuf;
  if (setjmp(m_JmpBuf) == -1) {
    jpeg_destroy_decompress(&cinfo);
    m_bInited = FALSE;
    return FALSE;
  }
  cinfo.image_width = m_OrigWidth;
  cinfo.image_height = m_OrigHeight;
  int ret = jpeg_read_header(&cinfo, TRUE);
  if (ret != JPEG_HEADER_OK)
    return FALSE;

  if (cinfo.saw_Adobe_marker)
    m_bJpegTransform = TRUE;

  if (cinfo.num_components == 3 && !m_bJpegTransform)
    cinfo.out_color_space = cinfo.jpeg_color_space;

  m_OrigWidth = cinfo.image_width;
  m_OrigHeight = cinfo.image_height;
  m_OutputWidth = m_OrigWidth;
  m_OutputHeight = m_OrigHeight;
  m_nDefaultScaleDenom = cinfo.scale_denom;
  return TRUE;
}

FX_BOOL CCodec_JpegDecoder::Create(const uint8_t* src_buf,
                                   uint32_t src_size,
                                   int width,
                                   int height,
                                   int nComps,
                                   FX_BOOL ColorTransform) {
  JpegScanSOI(&src_buf, &src_size);
  m_SrcBuf = src_buf;
  m_SrcSize = src_size;
  jerr.error_exit = _error_fatal;
  jerr.emit_message = _error_do_nothing1;
  jerr.output_message = _error_do_nothing;
  jerr.format_message = _error_do_nothing2;
  jerr.reset_error_mgr = _error_do_nothing;
  src.init_source = _src_do_nothing;
  src.term_source = _src_do_nothing;
  src.skip_input_data = _src_skip_data;
  src.fill_input_buffer = _src_fill_buffer;
  src.resync_to_restart = _src_resync;
  m_bJpegTransform = ColorTransform;
  if (src_size > 1 &&
      FXSYS_memcmp(src_buf + src_size - 2, "\xFF\xD9", 2) != 0) {
    ((uint8_t*)src_buf)[src_size - 2] = 0xFF;
    ((uint8_t*)src_buf)[src_size - 1] = 0xD9;
  }
  m_OutputWidth = m_OrigWidth = width;
  m_OutputHeight = m_OrigHeight = height;
  if (!InitDecode())
    return FALSE;

  if (cinfo.num_components < nComps)
    return FALSE;

  if ((int)cinfo.image_width < width)
    return FALSE;

  m_Pitch =
      (static_cast<uint32_t>(cinfo.image_width) * cinfo.num_components + 3) /
      4 * 4;
  m_pScanlineBuf = FX_Alloc(uint8_t, m_Pitch);
  m_nComps = cinfo.num_components;
  m_bpc = 8;
  m_bStarted = FALSE;
  return TRUE;
}

FX_BOOL CCodec_JpegDecoder::v_Rewind() {
  if (m_bStarted) {
    jpeg_destroy_decompress(&cinfo);
    if (!InitDecode()) {
      return FALSE;
    }
  }
  if (setjmp(m_JmpBuf) == -1) {
    return FALSE;
  }
  cinfo.scale_denom = m_nDefaultScaleDenom;
  m_OutputWidth = m_OrigWidth;
  m_OutputHeight = m_OrigHeight;
  if (!jpeg_start_decompress(&cinfo)) {
    jpeg_destroy_decompress(&cinfo);
    return FALSE;
  }
  if ((int)cinfo.output_width > m_OrigWidth) {
    ASSERT(FALSE);
    return FALSE;
  }
  m_bStarted = TRUE;
  return TRUE;
}

uint8_t* CCodec_JpegDecoder::v_GetNextLine() {
  if (setjmp(m_JmpBuf) == -1)
    return nullptr;

  int nlines = jpeg_read_scanlines(&cinfo, &m_pScanlineBuf, 1);
  return nlines > 0 ? m_pScanlineBuf : nullptr;
}

uint32_t CCodec_JpegDecoder::GetSrcOffset() {
  return (uint32_t)(m_SrcSize - src.bytes_in_buffer);
}

CCodec_ScanlineDecoder* CCodec_JpegModule::CreateDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    int nComps,
    FX_BOOL ColorTransform) {
  if (!src_buf || src_size == 0)
    return nullptr;

  std::unique_ptr<CCodec_JpegDecoder> pDecoder(new CCodec_JpegDecoder);
  if (!pDecoder->Create(src_buf, src_size, width, height, nComps,
                        ColorTransform)) {
    return nullptr;
  }
  return pDecoder.release();
}

bool CCodec_JpegModule::LoadInfo(const uint8_t* src_buf,
                                 uint32_t src_size,
                                 int* width,
                                 int* height,
                                 int* num_components,
                                 int* bits_per_components,
                                 bool* color_transform) {
  return JpegLoadInfo(src_buf, src_size, width, height, num_components,
                      bits_per_components, color_transform);
}

struct FXJPEG_Context {
  jmp_buf m_JumpMark;
  jpeg_decompress_struct m_Info;
  jpeg_error_mgr m_ErrMgr;
  jpeg_source_mgr m_SrcMgr;
  unsigned int m_SkipSize;
  void* (*m_AllocFunc)(unsigned int);
  void (*m_FreeFunc)(void*);
};
extern "C" {
static void _error_fatal1(j_common_ptr cinfo) {
  longjmp(((FXJPEG_Context*)cinfo->client_data)->m_JumpMark, -1);
}
};
extern "C" {
static void _src_skip_data1(struct jpeg_decompress_struct* cinfo, long num) {
  if (cinfo->src->bytes_in_buffer < (size_t)num) {
    ((FXJPEG_Context*)cinfo->client_data)->m_SkipSize =
        (unsigned int)(num - cinfo->src->bytes_in_buffer);
    cinfo->src->bytes_in_buffer = 0;
  } else {
    cinfo->src->next_input_byte += num;
    cinfo->src->bytes_in_buffer -= num;
  }
}
};
static void* jpeg_alloc_func(unsigned int size) {
  return FX_Alloc(char, size);
}
static void jpeg_free_func(void* p) {
  FX_Free(p);
}
FXJPEG_Context* CCodec_JpegModule::Start() {
  FXJPEG_Context* p = FX_Alloc(FXJPEG_Context, 1);
  p->m_AllocFunc = jpeg_alloc_func;
  p->m_FreeFunc = jpeg_free_func;
  p->m_ErrMgr.error_exit = _error_fatal1;
  p->m_ErrMgr.emit_message = _error_do_nothing1;
  p->m_ErrMgr.output_message = _error_do_nothing;
  p->m_ErrMgr.format_message = _error_do_nothing2;
  p->m_ErrMgr.reset_error_mgr = _error_do_nothing;
  p->m_SrcMgr.init_source = _src_do_nothing;
  p->m_SrcMgr.term_source = _src_do_nothing;
  p->m_SrcMgr.skip_input_data = _src_skip_data1;
  p->m_SrcMgr.fill_input_buffer = _src_fill_buffer;
  p->m_SrcMgr.resync_to_restart = _src_resync;
  p->m_Info.client_data = p;
  p->m_Info.err = &p->m_ErrMgr;
  if (setjmp(p->m_JumpMark) == -1) {
    return 0;
  }
  jpeg_create_decompress(&p->m_Info);
  p->m_Info.src = &p->m_SrcMgr;
  p->m_SkipSize = 0;
  return p;
}

void CCodec_JpegModule::Finish(FXJPEG_Context* ctx) {
  jpeg_destroy_decompress(&ctx->m_Info);
  ctx->m_FreeFunc(ctx);
}

void CCodec_JpegModule::Input(FXJPEG_Context* ctx,
                              const unsigned char* src_buf,
                              uint32_t src_size) {
  if (ctx->m_SkipSize) {
    if (ctx->m_SkipSize > src_size) {
      ctx->m_SrcMgr.bytes_in_buffer = 0;
      ctx->m_SkipSize -= src_size;
      return;
    }
    src_size -= ctx->m_SkipSize;
    src_buf += ctx->m_SkipSize;
    ctx->m_SkipSize = 0;
  }
  ctx->m_SrcMgr.next_input_byte = src_buf;
  ctx->m_SrcMgr.bytes_in_buffer = src_size;
}

#ifdef PDF_ENABLE_XFA
int CCodec_JpegModule::ReadHeader(FXJPEG_Context* ctx,
                                  int* width,
                                  int* height,
                                  int* nComps,
                                  CFX_DIBAttribute* pAttribute) {
#else   // PDF_ENABLE_XFA
int CCodec_JpegModule::ReadHeader(FXJPEG_Context* ctx,
                                  int* width,
                                  int* height,
                                  int* nComps) {
#endif  // PDF_ENABLE_XFA
  if (setjmp(ctx->m_JumpMark) == -1)
    return 1;

  int ret = jpeg_read_header(&ctx->m_Info, true);
  if (ret == JPEG_SUSPENDED)
    return 2;
  if (ret != JPEG_HEADER_OK)
    return 1;

  *width = ctx->m_Info.image_width;
  *height = ctx->m_Info.image_height;
  *nComps = ctx->m_Info.num_components;
#ifdef PDF_ENABLE_XFA
  JpegLoadAttribute(&ctx->m_Info, pAttribute);
#endif
  return 0;
}

int CCodec_JpegModule::StartScanline(FXJPEG_Context* ctx, int down_scale) {
  if (setjmp(ctx->m_JumpMark) == -1)
    return 0;

  ctx->m_Info.scale_denom = down_scale;
  return jpeg_start_decompress(&ctx->m_Info);
}

FX_BOOL CCodec_JpegModule::ReadScanline(FXJPEG_Context* ctx,
                                        unsigned char* dest_buf) {
  if (setjmp(ctx->m_JumpMark) == -1)
    return FALSE;

  int nlines = jpeg_read_scanlines(&ctx->m_Info, &dest_buf, 1);
  return nlines == 1;
}

uint32_t CCodec_JpegModule::GetAvailInput(FXJPEG_Context* ctx,
                                          uint8_t** avail_buf_ptr) {
  if (avail_buf_ptr) {
    *avail_buf_ptr = nullptr;
    if (ctx->m_SrcMgr.bytes_in_buffer > 0) {
      *avail_buf_ptr = (uint8_t*)ctx->m_SrcMgr.next_input_byte;
    }
  }
  return (uint32_t)ctx->m_SrcMgr.bytes_in_buffer;
}
