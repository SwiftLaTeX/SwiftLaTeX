// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/pageint.h"

#include <limits.h>

#include <algorithm>

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcodec/include/fx_codec.h"

namespace {

uint32_t ComponentsForFamily(int family) {
  if (family == PDFCS_DEVICERGB)
    return 3;
  if (family == PDFCS_DEVICEGRAY)
    return 1;
  return 4;
}

}  // namespace

void sRGB_to_AdobeCMYK(FX_FLOAT R,
                       FX_FLOAT G,
                       FX_FLOAT B,
                       FX_FLOAT& c,
                       FX_FLOAT& m,
                       FX_FLOAT& y,
                       FX_FLOAT& k) {
  c = 1.0f - R;
  m = 1.0f - G;
  y = 1.0f - B;
  k = c;
  if (m < k) {
    k = m;
  }
  if (y < k) {
    k = y;
  }
}

void ReverseRGB(uint8_t* pDestBuf, const uint8_t* pSrcBuf, int pixels) {
  if (pDestBuf == pSrcBuf) {
    for (int i = 0; i < pixels; i++) {
      uint8_t temp = pDestBuf[2];
      pDestBuf[2] = pDestBuf[0];
      pDestBuf[0] = temp;
      pDestBuf += 3;
    }
  } else {
    for (int i = 0; i < pixels; i++) {
      *pDestBuf++ = pSrcBuf[2];
      *pDestBuf++ = pSrcBuf[1];
      *pDestBuf++ = pSrcBuf[0];
      pSrcBuf += 3;
    }
  }
}

CPDF_DeviceCS::CPDF_DeviceCS(CPDF_Document* pDoc, int family)
    : CPDF_ColorSpace(pDoc, family, ComponentsForFamily(family)) {}

FX_BOOL CPDF_DeviceCS::GetRGB(FX_FLOAT* pBuf,
                              FX_FLOAT& R,
                              FX_FLOAT& G,
                              FX_FLOAT& B) const {
  if (m_Family == PDFCS_DEVICERGB) {
    R = pBuf[0];
    if (R < 0) {
      R = 0;
    } else if (R > 1) {
      R = 1;
    }
    G = pBuf[1];
    if (G < 0) {
      G = 0;
    } else if (G > 1) {
      G = 1;
    }
    B = pBuf[2];
    if (B < 0) {
      B = 0;
    } else if (B > 1) {
      B = 1;
    }
  } else if (m_Family == PDFCS_DEVICEGRAY) {
    R = *pBuf;
    if (R < 0) {
      R = 0;
    } else if (R > 1) {
      R = 1;
    }
    G = B = R;
  } else if (m_Family == PDFCS_DEVICECMYK) {
    if (!m_dwStdConversion) {
      AdobeCMYK_to_sRGB(pBuf[0], pBuf[1], pBuf[2], pBuf[3], R, G, B);
    } else {
      FX_FLOAT k = pBuf[3];
      R = 1.0f - std::min(1.0f, pBuf[0] + k);
      G = 1.0f - std::min(1.0f, pBuf[1] + k);
      B = 1.0f - std::min(1.0f, pBuf[2] + k);
    }
  } else {
    ASSERT(m_Family == PDFCS_PATTERN);
    R = G = B = 0;
    return FALSE;
  }
  return TRUE;
}
FX_BOOL CPDF_DeviceCS::v_GetCMYK(FX_FLOAT* pBuf,
                                 FX_FLOAT& c,
                                 FX_FLOAT& m,
                                 FX_FLOAT& y,
                                 FX_FLOAT& k) const {
  if (m_Family != PDFCS_DEVICECMYK) {
    return FALSE;
  }
  c = pBuf[0];
  m = pBuf[1];
  y = pBuf[2];
  k = pBuf[3];
  return TRUE;
}
FX_BOOL CPDF_DeviceCS::SetRGB(FX_FLOAT* pBuf,
                              FX_FLOAT R,
                              FX_FLOAT G,
                              FX_FLOAT B) const {
  if (m_Family == PDFCS_DEVICERGB) {
    pBuf[0] = R;
    pBuf[1] = G;
    pBuf[2] = B;
    return TRUE;
  }
  if (m_Family == PDFCS_DEVICEGRAY) {
    if (R == G && R == B) {
      *pBuf = R;
      return TRUE;
    }
    return FALSE;
  }
  if (m_Family == PDFCS_DEVICECMYK) {
    sRGB_to_AdobeCMYK(R, G, B, pBuf[0], pBuf[1], pBuf[2], pBuf[3]);
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CPDF_DeviceCS::v_SetCMYK(FX_FLOAT* pBuf,
                                 FX_FLOAT c,
                                 FX_FLOAT m,
                                 FX_FLOAT y,
                                 FX_FLOAT k) const {
  if (m_Family == PDFCS_DEVICERGB) {
    AdobeCMYK_to_sRGB(c, m, y, k, pBuf[0], pBuf[1], pBuf[2]);
    return TRUE;
  }
  if (m_Family == PDFCS_DEVICECMYK) {
    pBuf[0] = c;
    pBuf[1] = m;
    pBuf[2] = y;
    pBuf[3] = k;
    return TRUE;
  }
  return FALSE;
}

void CPDF_DeviceCS::TranslateImageLine(uint8_t* pDestBuf,
                                       const uint8_t* pSrcBuf,
                                       int pixels,
                                       int image_width,
                                       int image_height,
                                       FX_BOOL bTransMask) const {
  if (bTransMask && m_Family == PDFCS_DEVICECMYK) {
    for (int i = 0; i < pixels; i++) {
      int k = 255 - pSrcBuf[3];
      pDestBuf[0] = ((255 - pSrcBuf[0]) * k) / 255;
      pDestBuf[1] = ((255 - pSrcBuf[1]) * k) / 255;
      pDestBuf[2] = ((255 - pSrcBuf[2]) * k) / 255;
      pDestBuf += 3;
      pSrcBuf += 4;
    }
    return;
  }
  if (m_Family == PDFCS_DEVICERGB) {
    ReverseRGB(pDestBuf, pSrcBuf, pixels);
  } else if (m_Family == PDFCS_DEVICEGRAY) {
    for (int i = 0; i < pixels; i++) {
      *pDestBuf++ = pSrcBuf[i];
      *pDestBuf++ = pSrcBuf[i];
      *pDestBuf++ = pSrcBuf[i];
    }
  } else {
    for (int i = 0; i < pixels; i++) {
      if (!m_dwStdConversion) {
        AdobeCMYK_to_sRGB1(pSrcBuf[0], pSrcBuf[1], pSrcBuf[2], pSrcBuf[3],
                           pDestBuf[2], pDestBuf[1], pDestBuf[0]);
      } else {
        uint8_t k = pSrcBuf[3];
        pDestBuf[2] = 255 - std::min(255, pSrcBuf[0] + k);
        pDestBuf[1] = 255 - std::min(255, pSrcBuf[1] + k);
        pDestBuf[0] = 255 - std::min(255, pSrcBuf[2] + k);
      }
      pSrcBuf += 4;
      pDestBuf += 3;
    }
  }
}

CPDF_IccProfile::CPDF_IccProfile(const uint8_t* pData, uint32_t dwSize)
    : m_bsRGB(FALSE), m_pTransform(nullptr), m_nSrcComponents(0) {
  if (dwSize == 3144 &&
      FXSYS_memcmp(pData + 0x190, "sRGB IEC61966-2.1", 17) == 0) {
    m_bsRGB = TRUE;
    m_nSrcComponents = 3;
  } else if (CPDF_ModuleMgr::Get()->GetIccModule()) {
    m_pTransform = CPDF_ModuleMgr::Get()->GetIccModule()->CreateTransform_sRGB(
        pData, dwSize, m_nSrcComponents);
  }
}
CPDF_IccProfile::~CPDF_IccProfile() {
  if (m_pTransform) {
    CPDF_ModuleMgr::Get()->GetIccModule()->DestroyTransform(m_pTransform);
  }
}

