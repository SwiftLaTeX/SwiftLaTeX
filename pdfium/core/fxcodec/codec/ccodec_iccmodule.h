// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_

#include "core/fxcodec/include/fx_codec_def.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CFX_BinaryBuf;
class CFX_IccProfileCache;
class CFX_IccTransformCache;
class IFX_FileRead;

class CCodec_IccModule {
 public:
  enum IccCS {
    IccCS_Unknown = 0,
    IccCS_XYZ,
    IccCS_Lab,
    IccCS_Luv,
    IccCS_YCbCr,
    IccCS_Yxy,
    IccCS_Hsv,
    IccCS_Hls,
    IccCS_Gray,
    IccCS_Rgb,
    IccCS_Cmyk,
    IccCS_Cmy
  };

  struct IccParam {
    uint32_t Version;
    IccCS ColorSpace;
    uint32_t dwProfileType;
    uint32_t dwFormat;
    uint8_t* pProfileData;
    uint32_t dwProfileSize;
    double Gamma;
  };

  CCodec_IccModule();
  ~CCodec_IccModule();

  IccCS GetProfileCS(const uint8_t* pProfileData, unsigned int dwProfileSize);
  IccCS GetProfileCS(IFX_FileRead* pFile);
  void* CreateTransform(CCodec_IccModule::IccParam* pInputParam,
                        CCodec_IccModule::IccParam* pOutputParam,
                        CCodec_IccModule::IccParam* pProofParam = nullptr,
                        uint32_t dwIntent = Icc_INTENT_PERCEPTUAL,
                        uint32_t dwFlag = Icc_FLAGS_DEFAULT,
                        uint32_t dwPrfIntent = Icc_INTENT_ABSOLUTE_COLORIMETRIC,
                        uint32_t dwPrfFlag = Icc_FLAGS_SOFTPROOFING);
  void* CreateTransform_sRGB(const uint8_t* pProfileData,
                             uint32_t dwProfileSize,
                             uint32_t& nComponents,
                             int32_t intent = 0,
                             uint32_t dwSrcFormat = Icc_FORMAT_DEFAULT);
  void* CreateTransform_CMYK(const uint8_t* pSrcProfileData,
                             uint32_t dwSrcProfileSize,
                             uint32_t& nSrcComponents,
                             const uint8_t* pDstProfileData,
                             uint32_t dwDstProfileSize,
                             int32_t intent = 0,
                             uint32_t dwSrcFormat = Icc_FORMAT_DEFAULT,
                             uint32_t dwDstFormat = Icc_FORMAT_DEFAULT);
  void DestroyTransform(void* pTransform);
  void Translate(void* pTransform, FX_FLOAT* pSrcValues, FX_FLOAT* pDestValues);
  void TranslateScanline(void* pTransform,
                         uint8_t* pDest,
                         const uint8_t* pSrc,
                         int pixels);
  void SetComponents(uint32_t nComponents) { m_nComponents = nComponents; }

 protected:
  enum Icc_CLASS {
    Icc_CLASS_INPUT = 0,
    Icc_CLASS_OUTPUT,
    Icc_CLASS_PROOF,
    Icc_CLASS_MAX
  };
  void* CreateProfile(CCodec_IccModule::IccParam* pIccParam,
                      Icc_CLASS ic,
                      CFX_BinaryBuf* pTransformKey);

  uint32_t m_nComponents;
  std::map<CFX_ByteString, CFX_IccTransformCache*> m_MapTranform;
  std::map<CFX_ByteString, CFX_IccProfileCache*> m_MapProfile;
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_ICCMODULE_H_
