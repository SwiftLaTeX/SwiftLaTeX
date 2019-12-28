// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATEDATA_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATEDATA_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"

class CPDF_TransferFunc;
class CPDF_Object;

class CPDF_GeneralStateData {
 public:
  CPDF_GeneralStateData();
  CPDF_GeneralStateData(const CPDF_GeneralStateData& src);
  ~CPDF_GeneralStateData();

  void SetBlendMode(const CFX_ByteStringC& blend_mode);

  char m_BlendMode[16];
  int m_BlendType;
  CPDF_Object* m_pSoftMask;
  FX_FLOAT m_SMaskMatrix[6];
  FX_FLOAT m_StrokeAlpha;
  FX_FLOAT m_FillAlpha;
  CPDF_Object* m_pTR;
  CPDF_TransferFunc* m_pTransferFunc;
  CFX_Matrix m_Matrix;
  int m_RenderIntent;
  FX_BOOL m_StrokeAdjust;
  FX_BOOL m_AlphaSource;
  FX_BOOL m_TextKnockout;
  FX_BOOL m_StrokeOP;
  FX_BOOL m_FillOP;
  int m_OPMode;
  CPDF_Object* m_pBG;
  CPDF_Object* m_pUCR;
  CPDF_Object* m_pHT;
  FX_FLOAT m_Flatness;
  FX_FLOAT m_Smoothness;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_GENERALSTATEDATA_H_
