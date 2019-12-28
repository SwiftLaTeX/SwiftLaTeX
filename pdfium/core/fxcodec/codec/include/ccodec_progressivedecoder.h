// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_INCLUDE_CCODEC_PROGRESSIVEDECODER_H_
#define CORE_FXCODEC_CODEC_INCLUDE_CCODEC_PROGRESSIVEDECODER_H_

#include <vector>

#include "core/fxcodec/include/fx_codec_def.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"

class CCodec_BmpModule;
class CCodec_GifContext;
class CCodec_GifModule;
class CCodec_JpegModule;
class CCodec_ModuleMgr;
class CCodec_PngContext;
class CCodec_TiffContext;
class CFX_DIBAttribute;
class IFX_FileRead;
class IFX_Pause;
struct FXBMP_Context;
struct FXGIF_Context;
struct FXJPEG_Context;
struct FXPNG_Context;

class CCodec_ProgressiveDecoder {
 public:
  enum FXCodec_Format {
    FXCodec_Invalid = 0,
    FXCodec_1bppGray = 0x101,
    FXCodec_1bppRgb = 0x001,
    FXCodec_8bppGray = 0x108,
    FXCodec_8bppRgb = 0x008,
    FXCodec_Rgb = 0x018,
    FXCodec_Rgb32 = 0x020,
    FXCodec_Argb = 0x220,
    FXCodec_Cmyk = 0x120
  };

  explicit CCodec_ProgressiveDecoder(CCodec_ModuleMgr* pCodecMgr);
  ~CCodec_ProgressiveDecoder();

  FXCODEC_STATUS LoadImageInfo(IFX_FileRead* pFile,
                               FXCODEC_IMAGE_TYPE imageType,
                               CFX_DIBAttribute* pAttribute,
                               bool bSkipImageTypeCheck);

  FXCODEC_IMAGE_TYPE GetType() const { return m_imagType; }
  int32_t GetWidth() const { return m_SrcWidth; }
  int32_t GetHeight() const { return m_SrcHeight; }
  int32_t GetNumComponents() const { return m_SrcComponents; }
  int32_t GetBPC() const { return m_SrcBPC; }
  void SetClipBox(FX_RECT* clip);

  FXCODEC_STATUS GetFrames(int32_t& frames, IFX_Pause* pPause = nullptr);
  FXCODEC_STATUS StartDecode(CFX_DIBitmap* pDIBitmap,
                             int start_x,
                             int start_y,
                             int size_x,
                             int size_y,
                             int32_t frames = 0,
                             FX_BOOL bInterpol = TRUE);

  FXCODEC_STATUS ContinueDecode(IFX_Pause* pPause = nullptr);

  struct PixelWeight {
    int m_SrcStart;
    int m_SrcEnd;
    int m_Weights[1];
  };

  class CFXCODEC_WeightTable {
   public:
    CFXCODEC_WeightTable();
    ~CFXCODEC_WeightTable();

    void Calc(int dest_len,
              int dest_min,
              int dest_max,
              int src_len,
              int src_min,
              int src_max,
              FX_BOOL bInterpol);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            (pixel - m_DestMin) * m_ItemSize);
    }

    int m_DestMin;
    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

  class CFXCODEC_HorzTable {
   public:
    CFXCODEC_HorzTable();
    ~CFXCODEC_HorzTable();

    void Calc(int dest_len, int src_len, FX_BOOL bInterpol);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            pixel * m_ItemSize);
    }

    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

  class CFXCODEC_VertTable {
   public:
    CFXCODEC_VertTable();
    ~CFXCODEC_VertTable();

    void Calc(int dest_len, int src_len);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            pixel * m_ItemSize);
    }
    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

  IFX_FileRead* m_pFile;
  CCodec_ModuleMgr* m_pCodecMgr;
  FXJPEG_Context* m_pJpegContext;
  FXPNG_Context* m_pPngContext;
  FXGIF_Context* m_pGifContext;
  FXBMP_Context* m_pBmpContext;
  CCodec_TiffContext* m_pTiffContext;
  FXCODEC_IMAGE_TYPE m_imagType;
  uint32_t m_offSet;
  uint8_t* m_pSrcBuf;
  uint32_t m_SrcSize;
  uint8_t* m_pDecodeBuf;
  int m_ScanlineSize;
  CFX_DIBitmap* m_pDeviceBitmap;
  FX_BOOL m_bInterpol;
  CFXCODEC_WeightTable m_WeightHorz;
  CFXCODEC_VertTable m_WeightVert;
  CFXCODEC_HorzTable m_WeightHorzOO;
  int m_SrcWidth;
  int m_SrcHeight;
  int m_SrcComponents;
  int m_SrcBPC;
  FX_RECT m_clipBox;
  int m_startX;
  int m_startY;
  int m_sizeX;
  int m_sizeY;
  int m_TransMethod;
  FX_ARGB* m_pSrcPalette;
  int m_SrcPaletteNumber;
  int m_SrcRow;
  FXCodec_Format m_SrcFormat;
  int m_SrcPassNumber;
  int m_FrameNumber;
  int m_FrameCur;
  int m_GifBgIndex;
  uint8_t* m_pGifPalette;
  int32_t m_GifPltNumber;
  int m_GifTransIndex;
  FX_RECT m_GifFrameRect;
  FX_BOOL m_BmpIsTopBottom;
  FXCODEC_STATUS m_status;

 protected:
  static FX_BOOL PngReadHeaderFunc(void* pModule,
                                   int width,
                                   int height,
                                   int bpc,
                                   int pass,
                                   int* color_type,
                                   double* gamma);
  static FX_BOOL PngAskScanlineBufFunc(void* pModule,
                                       int line,
                                       uint8_t*& src_buf);
  static void PngFillScanlineBufCompletedFunc(void* pModule,
                                              int pass,
                                              int line);
  static void GifRecordCurrentPositionCallback(void* pModule,
                                               uint32_t& cur_pos);
  static uint8_t* GifAskLocalPaletteBufCallback(void* pModule,
                                                int32_t frame_num,
                                                int32_t pal_size);
  static FX_BOOL GifInputRecordPositionBufCallback(void* pModule,
                                                   uint32_t rcd_pos,
                                                   const FX_RECT& img_rc,
                                                   int32_t pal_num,
                                                   void* pal_ptr,
                                                   int32_t delay_time,
                                                   FX_BOOL user_input,
                                                   int32_t trans_index,
                                                   int32_t disposal_method,
                                                   FX_BOOL interlace);
  static void GifReadScanlineCallback(void* pModule,
                                      int32_t row_num,
                                      uint8_t* row_buf);
  static FX_BOOL BmpInputImagePositionBufCallback(void* pModule,
                                                  uint32_t rcd_pos);
  static void BmpReadScanlineCallback(void* pModule,
                                      int32_t row_num,
                                      uint8_t* row_buf);

  FX_BOOL DetectImageType(FXCODEC_IMAGE_TYPE imageType,
                          CFX_DIBAttribute* pAttribute);
  void GetDownScale(int& down_scale);
  void GetTransMethod(FXDIB_Format des_format, FXCodec_Format src_format);
  void ReSampleScanline(CFX_DIBitmap* pDeviceBitmap,
                        int32_t des_line,
                        uint8_t* src_scan,
                        FXCodec_Format src_format);
  void Resample(CFX_DIBitmap* pDeviceBitmap,
                int32_t src_line,
                uint8_t* src_scan,
                FXCodec_Format src_format);
  void ResampleVert(CFX_DIBitmap* pDeviceBitmap, double scale_y, int des_row);
  FX_BOOL JpegReadMoreData(CCodec_JpegModule* pJpegModule,
                           FXCODEC_STATUS& err_status);
  void PngOneOneMapResampleHorz(CFX_DIBitmap* pDeviceBitmap,
                                int32_t des_line,
                                uint8_t* src_scan,
                                FXCodec_Format src_format);
  FX_BOOL GifReadMoreData(CCodec_GifModule* pGifModule,
                          FXCODEC_STATUS& err_status);
  void GifDoubleLineResampleVert(CFX_DIBitmap* pDeviceBitmap,
                                 double scale_y,
                                 int des_row);
  FX_BOOL BmpReadMoreData(CCodec_BmpModule* pBmpModule,
                          FXCODEC_STATUS& err_status);
  void ResampleVertBT(CFX_DIBitmap* pDeviceBitmap, double scale_y, int des_row);
};

#endif  // CORE_FXCODEC_CODEC_INCLUDE_CCODEC_PROGRESSIVEDECODER_H_
