// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_colorspace.h"

#include <memory>

#include "core/fpdfapi/fpdf_page/cpdf_pagemodule.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcodec/include/fx_codec.h"

namespace {

const uint8_t g_sRGBSamples1[] = {
    0,   3,   6,   10,  13,  15,  18,  20,  22,  23,  25,  27,  28,  30,  31,
    32,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  49,  50,  51,  52,  53,  53,  54,  55,  56,  56,  57,  58,  58,
    59,  60,  61,  61,  62,  62,  63,  64,  64,  65,  66,  66,  67,  67,  68,
    68,  69,  70,  70,  71,  71,  72,  72,  73,  73,  74,  74,  75,  76,  76,
    77,  77,  78,  78,  79,  79,  79,  80,  80,  81,  81,  82,  82,  83,  83,
    84,  84,  85,  85,  85,  86,  86,  87,  87,  88,  88,  88,  89,  89,  90,
    90,  91,  91,  91,  92,  92,  93,  93,  93,  94,  94,  95,  95,  95,  96,
    96,  97,  97,  97,  98,  98,  98,  99,  99,  99,  100, 100, 101, 101, 101,
    102, 102, 102, 103, 103, 103, 104, 104, 104, 105, 105, 106, 106, 106, 107,
    107, 107, 108, 108, 108, 109, 109, 109, 110, 110, 110, 110, 111, 111, 111,
    112, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 115, 116, 116,
    116, 117, 117, 117, 118, 118, 118, 118, 119, 119, 119, 120,
};

const uint8_t g_sRGBSamples2[] = {
    120, 121, 122, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149,
    150, 151, 152, 153, 154, 155, 155, 156, 157, 158, 159, 159, 160, 161, 162,
    163, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173,
    174, 175, 175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184,
    185, 185, 186, 187, 187, 188, 189, 189, 190, 190, 191, 192, 192, 193, 194,
    194, 195, 196, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
    203, 204, 205, 205, 206, 206, 207, 208, 208, 209, 209, 210, 210, 211, 212,
    212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 218, 218, 219, 219, 220,
    220, 221, 221, 222, 222, 223, 223, 224, 224, 225, 226, 226, 227, 227, 228,
    228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235,
    236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242,
    243, 243, 244, 244, 245, 245, 246, 246, 246, 247, 247, 248, 248, 249, 249,
    250, 250, 251, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255,
};

class CPDF_CalGray : public CPDF_ColorSpace {
 public:
  explicit CPDF_CalGray(CPDF_Document* pDoc);

  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;

  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  FX_BOOL SetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT R,
                 FX_FLOAT G,
                 FX_FLOAT B) const override;

  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          FX_BOOL bTransMask = FALSE) const override;

 private:
  FX_FLOAT m_WhitePoint[3];
  FX_FLOAT m_BlackPoint[3];
  FX_FLOAT m_Gamma;
};

class CPDF_CalRGB : public CPDF_ColorSpace {
 public:
  explicit CPDF_CalRGB(CPDF_Document* pDoc);

  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;

  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  FX_BOOL SetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT R,
                 FX_FLOAT G,
                 FX_FLOAT B) const override;

  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          FX_BOOL bTransMask = FALSE) const override;

  FX_FLOAT m_WhitePoint[3];
  FX_FLOAT m_BlackPoint[3];
  FX_FLOAT m_Gamma[3];
  FX_FLOAT m_Matrix[9];
  FX_BOOL m_bGamma;
  FX_BOOL m_bMatrix;
};

class CPDF_LabCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_LabCS(CPDF_Document* pDoc);

  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;

  void GetDefaultValue(int iComponent,
                       FX_FLOAT& value,
                       FX_FLOAT& min,
                       FX_FLOAT& max) const override;
  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  FX_BOOL SetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT R,
                 FX_FLOAT G,
                 FX_FLOAT B) const override;

  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          FX_BOOL bTransMask = FALSE) const override;

  FX_FLOAT m_WhitePoint[3];
  FX_FLOAT m_BlackPoint[3];
  FX_FLOAT m_Ranges[4];
};

class CPDF_ICCBasedCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_ICCBasedCS(CPDF_Document* pDoc);
  ~CPDF_ICCBasedCS() override;

  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;

  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  FX_BOOL SetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT R,
                 FX_FLOAT G,
                 FX_FLOAT B) const override;

  FX_BOOL v_GetCMYK(FX_FLOAT* pBuf,
                    FX_FLOAT& c,
                    FX_FLOAT& m,
                    FX_FLOAT& y,
                    FX_FLOAT& k) const override;

  void EnableStdConversion(FX_BOOL bEnabled) override;
  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          FX_BOOL bTransMask = FALSE) const override;

  CPDF_ColorSpace* m_pAlterCS;
  CPDF_IccProfile* m_pProfile;
  uint8_t* m_pCache;
  FX_FLOAT* m_pRanges;
  FX_BOOL m_bOwn;
};

class CPDF_IndexedCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_IndexedCS(CPDF_Document* pDoc);
  ~CPDF_IndexedCS() override;

  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;

  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  CPDF_ColorSpace* GetBaseCS() const override;

  void EnableStdConversion(FX_BOOL bEnabled) override;

  CPDF_ColorSpace* m_pBaseCS;
  CPDF_CountedColorSpace* m_pCountedBaseCS;
  int m_nBaseComponents;
  int m_MaxIndex;
  CFX_ByteString m_Table;
  FX_FLOAT* m_pCompMinMax;
};

class CPDF_SeparationCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_SeparationCS(CPDF_Document* pDoc);
  ~CPDF_SeparationCS() override;

  // CPDF_ColorSpace:
  void GetDefaultValue(int iComponent,
                       FX_FLOAT& value,
                       FX_FLOAT& min,
                       FX_FLOAT& max) const override;
  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;
  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  void EnableStdConversion(FX_BOOL bEnabled) override;

  CPDF_ColorSpace* m_pAltCS;
  std::unique_ptr<CPDF_Function> m_pFunc;
  enum { None, All, Colorant } m_Type;
};

class CPDF_DeviceNCS : public CPDF_ColorSpace {
 public:
  explicit CPDF_DeviceNCS(CPDF_Document* pDoc);
  ~CPDF_DeviceNCS() override;

  // CPDF_ColorSpace:
  void GetDefaultValue(int iComponent,
                       FX_FLOAT& value,
                       FX_FLOAT& min,
                       FX_FLOAT& max) const override;
  FX_BOOL v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) override;
  FX_BOOL GetRGB(FX_FLOAT* pBuf,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) const override;
  void EnableStdConversion(FX_BOOL bEnabled) override;

  CPDF_ColorSpace* m_pAltCS;
  std::unique_ptr<CPDF_Function> m_pFunc;
};

FX_FLOAT RGB_Conversion(FX_FLOAT colorComponent) {
  if (colorComponent > 1)
    colorComponent = 1;
  if (colorComponent < 0)
    colorComponent = 0;

  int scale = (int)(colorComponent * 1023);
  if (scale < 0)
    scale = 0;
  if (scale < 192)
    colorComponent = (g_sRGBSamples1[scale] / 255.0f);
  else
    colorComponent = (g_sRGBSamples2[scale / 4 - 48] / 255.0f);
  return colorComponent;
}

void XYZ_to_sRGB(FX_FLOAT X,
                 FX_FLOAT Y,
                 FX_FLOAT Z,
                 FX_FLOAT& R,
                 FX_FLOAT& G,
                 FX_FLOAT& B) {
  FX_FLOAT R1 = 3.2410f * X - 1.5374f * Y - 0.4986f * Z;
  FX_FLOAT G1 = -0.9692f * X + 1.8760f * Y + 0.0416f * Z;
  FX_FLOAT B1 = 0.0556f * X - 0.2040f * Y + 1.0570f * Z;

  R = RGB_Conversion(R1);
  G = RGB_Conversion(G1);
  B = RGB_Conversion(B1);
}

void XYZ_to_sRGB_WhitePoint(FX_FLOAT X,
                            FX_FLOAT Y,
                            FX_FLOAT Z,
                            FX_FLOAT& R,
                            FX_FLOAT& G,
                            FX_FLOAT& B,
                            FX_FLOAT Xw,
                            FX_FLOAT Yw,
                            FX_FLOAT Zw) {
  // The following RGB_xyz is based on
  // sRGB value {Rx,Ry}={0.64, 0.33}, {Gx,Gy}={0.30, 0.60}, {Bx,By}={0.15, 0.06}

  FX_FLOAT Rx = 0.64f, Ry = 0.33f;
  FX_FLOAT Gx = 0.30f, Gy = 0.60f;
  FX_FLOAT Bx = 0.15f, By = 0.06f;
  CFX_Matrix_3by3 RGB_xyz(Rx, Gx, Bx, Ry, Gy, By, 1 - Rx - Ry, 1 - Gx - Gy,
                          1 - Bx - By);
  CFX_Vector_3by1 whitePoint(Xw, Yw, Zw);
  CFX_Vector_3by1 XYZ(X, Y, Z);

  CFX_Vector_3by1 RGB_Sum_XYZ = RGB_xyz.Inverse().TransformVector(whitePoint);
  CFX_Matrix_3by3 RGB_SUM_XYZ_DIAG(RGB_Sum_XYZ.a, 0, 0, 0, RGB_Sum_XYZ.b, 0, 0,
                                   0, RGB_Sum_XYZ.c);
  CFX_Matrix_3by3 M = RGB_xyz.Multiply(RGB_SUM_XYZ_DIAG);
  CFX_Vector_3by1 RGB = M.Inverse().TransformVector(XYZ);

  R = RGB_Conversion(RGB.a);
  G = RGB_Conversion(RGB.b);
  B = RGB_Conversion(RGB.c);
}

}  // namespace

CPDF_ColorSpace* CPDF_ColorSpace::ColorspaceFromName(
    const CFX_ByteString& name) {
  if (name == "DeviceRGB" || name == "RGB") {
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB);
  }
  if (name == "DeviceGray" || name == "G") {
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY);
  }
  if (name == "DeviceCMYK" || name == "CMYK") {
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICECMYK);
  }
  if (name == "Pattern") {
    return CPDF_ColorSpace::GetStockCS(PDFCS_PATTERN);
  }
  return nullptr;
}

CPDF_ColorSpace* CPDF_ColorSpace::GetStockCS(int family) {
  return CPDF_ModuleMgr::Get()->GetPageModule()->GetStockCS(family);
}

CPDF_ColorSpace* CPDF_ColorSpace::Load(CPDF_Document* pDoc, CPDF_Object* pObj) {
  if (!pObj)
    return nullptr;
  if (pObj->IsName())
    return ColorspaceFromName(pObj->GetString());

  if (CPDF_Stream* pStream = pObj->AsStream()) {
    CPDF_Dictionary* pDict = pStream->GetDict();
    if (!pDict)
      return nullptr;

    for (const auto& it : *pDict) {
      CPDF_ColorSpace* pRet = nullptr;
      CPDF_Object* pValue = it.second;
      if (ToName(pValue))
        pRet = ColorspaceFromName(pValue->GetString());
      if (pRet)
        return pRet;
    }
    return nullptr;
  }

  CPDF_Array* pArray = pObj->AsArray();
  if (!pArray || pArray->GetCount() == 0)
    return nullptr;

  CPDF_Object* pFamilyObj = pArray->GetDirectObjectAt(0);
  if (!pFamilyObj)
    return nullptr;

  CFX_ByteString familyname = pFamilyObj->GetString();
  if (pArray->GetCount() == 1)
    return ColorspaceFromName(familyname);

  CPDF_ColorSpace* pCS = nullptr;
  uint32_t id = familyname.GetID();
  if (id == FXBSTR_ID('C', 'a', 'l', 'G')) {
    pCS = new CPDF_CalGray(pDoc);
  } else if (id == FXBSTR_ID('C', 'a', 'l', 'R')) {
    pCS = new CPDF_CalRGB(pDoc);
  } else if (id == FXBSTR_ID('L', 'a', 'b', 0)) {
    pCS = new CPDF_LabCS(pDoc);
  } else if (id == FXBSTR_ID('I', 'C', 'C', 'B')) {
    pCS = new CPDF_ICCBasedCS(pDoc);
  } else if (id == FXBSTR_ID('I', 'n', 'd', 'e') ||
             id == FXBSTR_ID('I', 0, 0, 0)) {
    pCS = new CPDF_IndexedCS(pDoc);
  } else if (id == FXBSTR_ID('S', 'e', 'p', 'a')) {
    pCS = new CPDF_SeparationCS(pDoc);
  } else if (id == FXBSTR_ID('D', 'e', 'v', 'i')) {
    pCS = new CPDF_DeviceNCS(pDoc);
  } else if (id == FXBSTR_ID('P', 'a', 't', 't')) {
    pCS = new CPDF_PatternCS(pDoc);
  } else {
    return nullptr;
  }
  pCS->m_pArray = pArray;
  if (!pCS->v_Load(pDoc, pArray)) {
    pCS->ReleaseCS();
    return nullptr;
  }
  return pCS;
}

void CPDF_ColorSpace::ReleaseCS() {
  if (this == GetStockCS(PDFCS_DEVICERGB)) {
    return;
  }
  if (this == GetStockCS(PDFCS_DEVICEGRAY)) {
    return;
  }
  if (this == GetStockCS(PDFCS_DEVICECMYK)) {
    return;
  }
  if (this == GetStockCS(PDFCS_PATTERN)) {
    return;
  }
  delete this;
}

int CPDF_ColorSpace::GetBufSize() const {
  if (m_Family == PDFCS_PATTERN) {
    return sizeof(PatternValue);
  }
  return m_nComponents * sizeof(FX_FLOAT);
}

FX_FLOAT* CPDF_ColorSpace::CreateBuf() {
  int size = GetBufSize();
  uint8_t* pBuf = FX_Alloc(uint8_t, size);
  return (FX_FLOAT*)pBuf;
}

FX_BOOL CPDF_ColorSpace::sRGB() const {
  if (m_Family == PDFCS_DEVICERGB) {
    return TRUE;
  }
  if (m_Family != PDFCS_ICCBASED) {
    return FALSE;
  }
  CPDF_ICCBasedCS* pCS = (CPDF_ICCBasedCS*)this;
  return pCS->m_pProfile->m_bsRGB;
}

FX_BOOL CPDF_ColorSpace::SetRGB(FX_FLOAT* pBuf,
                                FX_FLOAT R,
                                FX_FLOAT G,
                                FX_FLOAT B) const {
  return FALSE;
}

FX_BOOL CPDF_ColorSpace::GetCMYK(FX_FLOAT* pBuf,
                                 FX_FLOAT& c,
                                 FX_FLOAT& m,
                                 FX_FLOAT& y,
                                 FX_FLOAT& k) const {
  if (v_GetCMYK(pBuf, c, m, y, k)) {
    return TRUE;
  }
  FX_FLOAT R, G, B;
  if (!GetRGB(pBuf, R, G, B)) {
    return FALSE;
  }
  sRGB_to_AdobeCMYK(R, G, B, c, m, y, k);
  return TRUE;
}

FX_BOOL CPDF_ColorSpace::SetCMYK(FX_FLOAT* pBuf,
                                 FX_FLOAT c,
                                 FX_FLOAT m,
                                 FX_FLOAT y,
                                 FX_FLOAT k) const {
  if (v_SetCMYK(pBuf, c, m, y, k)) {
    return TRUE;
  }
  FX_FLOAT R, G, B;
  AdobeCMYK_to_sRGB(c, m, y, k, R, G, B);
  return SetRGB(pBuf, R, G, B);
}

void CPDF_ColorSpace::GetDefaultColor(FX_FLOAT* buf) const {
  if (!buf || m_Family == PDFCS_PATTERN) {
    return;
  }
  FX_FLOAT min, max;
  for (uint32_t i = 0; i < m_nComponents; i++) {
    GetDefaultValue(i, buf[i], min, max);
  }
}

uint32_t CPDF_ColorSpace::CountComponents() const {
  return m_nComponents;
}

void CPDF_ColorSpace::GetDefaultValue(int iComponent,
                                      FX_FLOAT& value,
                                      FX_FLOAT& min,
                                      FX_FLOAT& max) const {
  value = 0;
  min = 0;
  max = 1.0f;
}

void CPDF_ColorSpace::TranslateImageLine(uint8_t* dest_buf,
                                         const uint8_t* src_buf,
                                         int pixels,
                                         int image_width,
                                         int image_height,
                                         FX_BOOL bTransMask) const {
  CFX_FixedBufGrow<FX_FLOAT, 16> srcbuf(m_nComponents);
  FX_FLOAT* src = srcbuf;
  FX_FLOAT R, G, B;
  for (int i = 0; i < pixels; i++) {
    for (uint32_t j = 0; j < m_nComponents; j++)
      if (m_Family == PDFCS_INDEXED) {
        src[j] = (FX_FLOAT)(*src_buf++);
      } else {
        src[j] = (FX_FLOAT)(*src_buf++) / 255;
      }
    GetRGB(src, R, G, B);
    *dest_buf++ = (int32_t)(B * 255);
    *dest_buf++ = (int32_t)(G * 255);
    *dest_buf++ = (int32_t)(R * 255);
  }
}

CPDF_ColorSpace* CPDF_ColorSpace::GetBaseCS() const {
  return nullptr;
}

void CPDF_ColorSpace::EnableStdConversion(FX_BOOL bEnabled) {
  if (bEnabled)
    m_dwStdConversion++;
  else if (m_dwStdConversion)
    m_dwStdConversion--;
}

CPDF_ColorSpace::CPDF_ColorSpace(CPDF_Document* pDoc,
                                 int family,
                                 uint32_t nComponents)
    : m_pDocument(pDoc),
      m_Family(family),
      m_nComponents(nComponents),
      m_pArray(nullptr),
      m_dwStdConversion(0) {}

CPDF_ColorSpace::~CPDF_ColorSpace() {}

FX_BOOL CPDF_ColorSpace::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  return TRUE;
}

FX_BOOL CPDF_ColorSpace::v_GetCMYK(FX_FLOAT* pBuf,
                                   FX_FLOAT& c,
                                   FX_FLOAT& m,
                                   FX_FLOAT& y,
                                   FX_FLOAT& k) const {
  return FALSE;
}

FX_BOOL CPDF_ColorSpace::v_SetCMYK(FX_FLOAT* pBuf,
                                   FX_FLOAT c,
                                   FX_FLOAT m,
                                   FX_FLOAT y,
                                   FX_FLOAT k) const {
  return FALSE;
}

CPDF_CalGray::CPDF_CalGray(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_CALGRAY, 1) {}

FX_BOOL CPDF_CalGray::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return FALSE;

  CPDF_Array* pParam = pDict->GetArrayBy("WhitePoint");
  int i;
  for (i = 0; i < 3; i++)
    m_WhitePoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  pParam = pDict->GetArrayBy("BlackPoint");
  for (i = 0; i < 3; i++)
    m_BlackPoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  m_Gamma = pDict->GetNumberBy("Gamma");
  if (m_Gamma == 0)
    m_Gamma = 1.0f;
  return TRUE;
}

FX_BOOL CPDF_CalGray::GetRGB(FX_FLOAT* pBuf,
                             FX_FLOAT& R,
                             FX_FLOAT& G,
                             FX_FLOAT& B) const {
  R = G = B = *pBuf;
  return TRUE;
}

FX_BOOL CPDF_CalGray::SetRGB(FX_FLOAT* pBuf,
                             FX_FLOAT R,
                             FX_FLOAT G,
                             FX_FLOAT B) const {
  if (R == G && R == B) {
    *pBuf = R;
    return TRUE;
  }
  return FALSE;
}

void CPDF_CalGray::TranslateImageLine(uint8_t* pDestBuf,
                                      const uint8_t* pSrcBuf,
                                      int pixels,
                                      int image_width,
                                      int image_height,
                                      FX_BOOL bTransMask) const {
  for (int i = 0; i < pixels; i++) {
    *pDestBuf++ = pSrcBuf[i];
    *pDestBuf++ = pSrcBuf[i];
    *pDestBuf++ = pSrcBuf[i];
  }
}

CPDF_CalRGB::CPDF_CalRGB(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_CALRGB, 3) {}

FX_BOOL CPDF_CalRGB::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return FALSE;

  CPDF_Array* pParam = pDict->GetArrayBy("WhitePoint");
  int i;
  for (i = 0; i < 3; i++)
    m_WhitePoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  pParam = pDict->GetArrayBy("BlackPoint");
  for (i = 0; i < 3; i++)
    m_BlackPoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  pParam = pDict->GetArrayBy("Gamma");
  if (pParam) {
    m_bGamma = TRUE;
    for (i = 0; i < 3; i++)
      m_Gamma[i] = pParam->GetNumberAt(i);
  } else {
    m_bGamma = FALSE;
  }

  pParam = pDict->GetArrayBy("Matrix");
  if (pParam) {
    m_bMatrix = TRUE;
    for (i = 0; i < 9; i++)
      m_Matrix[i] = pParam->GetNumberAt(i);
  } else {
    m_bMatrix = FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_CalRGB::GetRGB(FX_FLOAT* pBuf,
                            FX_FLOAT& R,
                            FX_FLOAT& G,
                            FX_FLOAT& B) const {
  FX_FLOAT A_ = pBuf[0];
  FX_FLOAT B_ = pBuf[1];
  FX_FLOAT C_ = pBuf[2];
  if (m_bGamma) {
    A_ = (FX_FLOAT)FXSYS_pow(A_, m_Gamma[0]);
    B_ = (FX_FLOAT)FXSYS_pow(B_, m_Gamma[1]);
    C_ = (FX_FLOAT)FXSYS_pow(C_, m_Gamma[2]);
  }

  FX_FLOAT X;
  FX_FLOAT Y;
  FX_FLOAT Z;
  if (m_bMatrix) {
    X = m_Matrix[0] * A_ + m_Matrix[3] * B_ + m_Matrix[6] * C_;
    Y = m_Matrix[1] * A_ + m_Matrix[4] * B_ + m_Matrix[7] * C_;
    Z = m_Matrix[2] * A_ + m_Matrix[5] * B_ + m_Matrix[8] * C_;
  } else {
    X = A_;
    Y = B_;
    Z = C_;
  }
  XYZ_to_sRGB_WhitePoint(X, Y, Z, R, G, B, m_WhitePoint[0], m_WhitePoint[1],
                         m_WhitePoint[2]);
  return TRUE;
}

FX_BOOL CPDF_CalRGB::SetRGB(FX_FLOAT* pBuf,
                            FX_FLOAT R,
                            FX_FLOAT G,
                            FX_FLOAT B) const {
  pBuf[0] = R;
  pBuf[1] = G;
  pBuf[2] = B;
  return TRUE;
}

void CPDF_CalRGB::TranslateImageLine(uint8_t* pDestBuf,
                                     const uint8_t* pSrcBuf,
                                     int pixels,
                                     int image_width,
                                     int image_height,
                                     FX_BOOL bTransMask) const {
  if (bTransMask) {
    FX_FLOAT Cal[3];
    FX_FLOAT R;
    FX_FLOAT G;
    FX_FLOAT B;
    for (int i = 0; i < pixels; i++) {
      Cal[0] = ((FX_FLOAT)pSrcBuf[2]) / 255;
      Cal[1] = ((FX_FLOAT)pSrcBuf[1]) / 255;
      Cal[2] = ((FX_FLOAT)pSrcBuf[0]) / 255;
      GetRGB(Cal, R, G, B);
      pDestBuf[0] = FXSYS_round(B * 255);
      pDestBuf[1] = FXSYS_round(G * 255);
      pDestBuf[2] = FXSYS_round(R * 255);
      pSrcBuf += 3;
      pDestBuf += 3;
    }
  }
  ReverseRGB(pDestBuf, pSrcBuf, pixels);
}

CPDF_LabCS::CPDF_LabCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_LAB, 3) {}

void CPDF_LabCS::GetDefaultValue(int iComponent,
                                 FX_FLOAT& value,
                                 FX_FLOAT& min,
                                 FX_FLOAT& max) const {
  ASSERT(iComponent < 3);
  value = 0;
  if (iComponent == 0) {
    min = 0;
    max = 100 * 1.0f;
  } else {
    min = m_Ranges[iComponent * 2 - 2];
    max = m_Ranges[iComponent * 2 - 1];
    if (value < min)
      value = min;
    else if (value > max)
      value = max;
  }
}

FX_BOOL CPDF_LabCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return FALSE;

  CPDF_Array* pParam = pDict->GetArrayBy("WhitePoint");
  int i;
  for (i = 0; i < 3; i++)
    m_WhitePoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  pParam = pDict->GetArrayBy("BlackPoint");
  for (i = 0; i < 3; i++)
    m_BlackPoint[i] = pParam ? pParam->GetNumberAt(i) : 0;

  pParam = pDict->GetArrayBy("Range");
  const FX_FLOAT def_ranges[4] = {-100 * 1.0f, 100 * 1.0f, -100 * 1.0f,
                                  100 * 1.0f};
  for (i = 0; i < 4; i++)
    m_Ranges[i] = pParam ? pParam->GetNumberAt(i) : def_ranges[i];
  return TRUE;
}

FX_BOOL CPDF_LabCS::GetRGB(FX_FLOAT* pBuf,
                           FX_FLOAT& R,
                           FX_FLOAT& G,
                           FX_FLOAT& B) const {
  FX_FLOAT Lstar = pBuf[0];
  FX_FLOAT astar = pBuf[1];
  FX_FLOAT bstar = pBuf[2];
  FX_FLOAT M = (Lstar + 16.0f) / 116.0f;
  FX_FLOAT L = M + astar / 500.0f;
  FX_FLOAT N = M - bstar / 200.0f;
  FX_FLOAT X, Y, Z;
  if (L < 0.2069f)
    X = 0.957f * 0.12842f * (L - 0.1379f);
  else
    X = 0.957f * L * L * L;

  if (M < 0.2069f)
    Y = 0.12842f * (M - 0.1379f);
  else
    Y = M * M * M;

  if (N < 0.2069f)
    Z = 1.0889f * 0.12842f * (N - 0.1379f);
  else
    Z = 1.0889f * N * N * N;

  XYZ_to_sRGB(X, Y, Z, R, G, B);
  return TRUE;
}

FX_BOOL CPDF_LabCS::SetRGB(FX_FLOAT* pBuf,
                           FX_FLOAT R,
                           FX_FLOAT G,
                           FX_FLOAT B) const {
  return FALSE;
}

void CPDF_LabCS::TranslateImageLine(uint8_t* pDestBuf,
                                    const uint8_t* pSrcBuf,
                                    int pixels,
                                    int image_width,
                                    int image_height,
                                    FX_BOOL bTransMask) const {
  for (int i = 0; i < pixels; i++) {
    FX_FLOAT lab[3];
    FX_FLOAT R, G, B;
    lab[0] = (pSrcBuf[0] * 100 / 255.0f);
    lab[1] = (FX_FLOAT)(pSrcBuf[1] - 128);
    lab[2] = (FX_FLOAT)(pSrcBuf[2] - 128);
    GetRGB(lab, R, G, B);
    pDestBuf[0] = (int32_t)(B * 255);
    pDestBuf[1] = (int32_t)(G * 255);
    pDestBuf[2] = (int32_t)(R * 255);
    pDestBuf += 3;
    pSrcBuf += 3;
  }
}

CPDF_ICCBasedCS::CPDF_ICCBasedCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_ICCBASED, 0),
      m_pAlterCS(nullptr),
      m_pProfile(nullptr),
      m_pCache(nullptr),
      m_pRanges(nullptr),
      m_bOwn(FALSE) {}

CPDF_ICCBasedCS::~CPDF_ICCBasedCS() {
  FX_Free(m_pCache);
  FX_Free(m_pRanges);
  if (m_pAlterCS && m_bOwn)
    m_pAlterCS->ReleaseCS();
  if (m_pProfile && m_pDocument)
    m_pDocument->GetPageData()->ReleaseIccProfile(m_pProfile);
}

FX_BOOL CPDF_ICCBasedCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Stream* pStream = pArray->GetStreamAt(1);
  if (!pStream)
    return FALSE;

  m_pProfile = pDoc->LoadIccProfile(pStream);
  if (!m_pProfile)
    return FALSE;

  m_nComponents =
      m_pProfile
          ->GetComponents();  // Try using the nComponents from ICC profile
  CPDF_Dictionary* pDict = pStream->GetDict();
  if (!m_pProfile->m_pTransform) {  // No valid ICC profile or using sRGB
    CPDF_Object* pAlterCSObj =
        pDict ? pDict->GetDirectObjectBy("Alternate") : nullptr;
    if (pAlterCSObj) {
      CPDF_ColorSpace* pAlterCS = CPDF_ColorSpace::Load(pDoc, pAlterCSObj);
      if (pAlterCS) {
        if (m_nComponents == 0) {                 // NO valid ICC profile
          if (pAlterCS->CountComponents() > 0) {  // Use Alternative colorspace
            m_nComponents = pAlterCS->CountComponents();
            m_pAlterCS = pAlterCS;
            m_bOwn = TRUE;
          } else {  // No valid alternative colorspace
            pAlterCS->ReleaseCS();
            int32_t nDictComponents = pDict ? pDict->GetIntegerBy("N") : 0;
            if (nDictComponents != 1 && nDictComponents != 3 &&
                nDictComponents != 4) {
              return FALSE;
            }
            m_nComponents = nDictComponents;
          }

        } else {  // Using sRGB
          if (pAlterCS->CountComponents() != m_nComponents) {
            pAlterCS->ReleaseCS();
          } else {
            m_pAlterCS = pAlterCS;
            m_bOwn = TRUE;
          }
        }
      }
    }
    if (!m_pAlterCS) {
      if (m_nComponents == 1)
        m_pAlterCS = GetStockCS(PDFCS_DEVICEGRAY);
      else if (m_nComponents == 3)
        m_pAlterCS = GetStockCS(PDFCS_DEVICERGB);
      else if (m_nComponents == 4)
        m_pAlterCS = GetStockCS(PDFCS_DEVICECMYK);
    }
  }
  CPDF_Array* pRanges = pDict->GetArrayBy("Range");
  m_pRanges = FX_Alloc2D(FX_FLOAT, m_nComponents, 2);
  for (uint32_t i = 0; i < m_nComponents * 2; i++) {
    if (pRanges)
      m_pRanges[i] = pRanges->GetNumberAt(i);
    else if (i % 2)
      m_pRanges[i] = 1.0f;
    else
      m_pRanges[i] = 0;
  }
  return TRUE;
}

FX_BOOL CPDF_ICCBasedCS::GetRGB(FX_FLOAT* pBuf,
                                FX_FLOAT& R,
                                FX_FLOAT& G,
                                FX_FLOAT& B) const {
  if (m_pProfile && m_pProfile->m_bsRGB) {
    R = pBuf[0];
    G = pBuf[1];
    B = pBuf[2];
    return TRUE;
  }
  CCodec_IccModule* pIccModule = CPDF_ModuleMgr::Get()->GetIccModule();
  if (!m_pProfile->m_pTransform || !pIccModule) {
    if (m_pAlterCS)
      return m_pAlterCS->GetRGB(pBuf, R, G, B);

    R = 0.0f;
    G = 0.0f;
    B = 0.0f;
    return TRUE;
  }
  FX_FLOAT rgb[3];
  pIccModule->SetComponents(m_nComponents);
  pIccModule->Translate(m_pProfile->m_pTransform, pBuf, rgb);
  R = rgb[0];
  G = rgb[1];
  B = rgb[2];
  return TRUE;
}

FX_BOOL CPDF_ICCBasedCS::SetRGB(FX_FLOAT* pBuf,
                                FX_FLOAT R,
                                FX_FLOAT G,
                                FX_FLOAT B) const {
  return FALSE;
}

FX_BOOL CPDF_ICCBasedCS::v_GetCMYK(FX_FLOAT* pBuf,
                                   FX_FLOAT& c,
                                   FX_FLOAT& m,
                                   FX_FLOAT& y,
                                   FX_FLOAT& k) const {
  if (m_nComponents != 4)
    return FALSE;

  c = pBuf[0];
  m = pBuf[1];
  y = pBuf[2];
  k = pBuf[3];
  return TRUE;
}

void CPDF_ICCBasedCS::EnableStdConversion(FX_BOOL bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAlterCS)
    m_pAlterCS->EnableStdConversion(bEnabled);
}

void CPDF_ICCBasedCS::TranslateImageLine(uint8_t* pDestBuf,
                                         const uint8_t* pSrcBuf,
                                         int pixels,
                                         int image_width,
                                         int image_height,
                                         FX_BOOL bTransMask) const {
  if (m_pProfile->m_bsRGB) {
    ReverseRGB(pDestBuf, pSrcBuf, pixels);
  } else if (m_pProfile->m_pTransform) {
    int nMaxColors = 1;
    for (uint32_t i = 0; i < m_nComponents; i++) {
      nMaxColors *= 52;
    }
    if (m_nComponents > 3 || image_width * image_height < nMaxColors * 3 / 2) {
      CPDF_ModuleMgr::Get()->GetIccModule()->TranslateScanline(
          m_pProfile->m_pTransform, pDestBuf, pSrcBuf, pixels);
    } else {
      if (!m_pCache) {
        ((CPDF_ICCBasedCS*)this)->m_pCache = FX_Alloc2D(uint8_t, nMaxColors, 3);
        uint8_t* temp_src = FX_Alloc2D(uint8_t, nMaxColors, m_nComponents);
        uint8_t* pSrc = temp_src;
        for (int i = 0; i < nMaxColors; i++) {
          uint32_t color = i;
          uint32_t order = nMaxColors / 52;
          for (uint32_t c = 0; c < m_nComponents; c++) {
            *pSrc++ = (uint8_t)(color / order * 5);
            color %= order;
            order /= 52;
          }
        }
        CPDF_ModuleMgr::Get()->GetIccModule()->TranslateScanline(
            m_pProfile->m_pTransform, m_pCache, temp_src, nMaxColors);
        FX_Free(temp_src);
      }
      for (int i = 0; i < pixels; i++) {
        int index = 0;
        for (uint32_t c = 0; c < m_nComponents; c++) {
          index = index * 52 + (*pSrcBuf) / 5;
          pSrcBuf++;
        }
        index *= 3;
        *pDestBuf++ = m_pCache[index];
        *pDestBuf++ = m_pCache[index + 1];
        *pDestBuf++ = m_pCache[index + 2];
      }
    }
  } else if (m_pAlterCS) {
    m_pAlterCS->TranslateImageLine(pDestBuf, pSrcBuf, pixels, image_width,
                                   image_height);
  }
}

CPDF_IndexedCS::CPDF_IndexedCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_INDEXED, 1),
      m_pBaseCS(nullptr),
      m_pCountedBaseCS(nullptr),
      m_pCompMinMax(nullptr) {}

CPDF_IndexedCS::~CPDF_IndexedCS() {
  FX_Free(m_pCompMinMax);
  CPDF_ColorSpace* pCS = m_pCountedBaseCS ? m_pCountedBaseCS->get() : nullptr;
  if (pCS && m_pDocument) {
    m_pDocument->GetPageData()->ReleaseColorSpace(pCS->GetArray());
  }
}

FX_BOOL CPDF_IndexedCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  if (pArray->GetCount() < 4) {
    return FALSE;
  }
  CPDF_Object* pBaseObj = pArray->GetDirectObjectAt(1);
  if (pBaseObj == m_pArray) {
    return FALSE;
  }
  CPDF_DocPageData* pDocPageData = pDoc->GetPageData();
  m_pBaseCS = pDocPageData->GetColorSpace(pBaseObj, nullptr);
  if (!m_pBaseCS) {
    return FALSE;
  }
  m_pCountedBaseCS = pDocPageData->FindColorSpacePtr(m_pBaseCS->GetArray());
  m_nBaseComponents = m_pBaseCS->CountComponents();
  m_pCompMinMax = FX_Alloc2D(FX_FLOAT, m_nBaseComponents, 2);
  FX_FLOAT defvalue;
  for (int i = 0; i < m_nBaseComponents; i++) {
    m_pBaseCS->GetDefaultValue(i, defvalue, m_pCompMinMax[i * 2],
                               m_pCompMinMax[i * 2 + 1]);
    m_pCompMinMax[i * 2 + 1] -= m_pCompMinMax[i * 2];
  }
  m_MaxIndex = pArray->GetIntegerAt(2);

  CPDF_Object* pTableObj = pArray->GetDirectObjectAt(3);
  if (!pTableObj)
    return FALSE;

  if (CPDF_String* pString = pTableObj->AsString()) {
    m_Table = pString->GetString();
  } else if (CPDF_Stream* pStream = pTableObj->AsStream()) {
    CPDF_StreamAcc acc;
    acc.LoadAllData(pStream, FALSE);
    m_Table = CFX_ByteStringC(acc.GetData(), acc.GetSize());
  }
  return TRUE;
}

FX_BOOL CPDF_IndexedCS::GetRGB(FX_FLOAT* pBuf,
                               FX_FLOAT& R,
                               FX_FLOAT& G,
                               FX_FLOAT& B) const {
  int index = (int32_t)(*pBuf);
  if (index < 0 || index > m_MaxIndex) {
    return FALSE;
  }
  if (m_nBaseComponents) {
    if (index == INT_MAX || (index + 1) > INT_MAX / m_nBaseComponents ||
        (index + 1) * m_nBaseComponents > (int)m_Table.GetLength()) {
      R = G = B = 0;
      return FALSE;
    }
  }
  CFX_FixedBufGrow<FX_FLOAT, 16> Comps(m_nBaseComponents);
  FX_FLOAT* comps = Comps;
  const uint8_t* pTable = m_Table.raw_str();
  for (int i = 0; i < m_nBaseComponents; i++) {
    comps[i] =
        m_pCompMinMax[i * 2] +
        m_pCompMinMax[i * 2 + 1] * pTable[index * m_nBaseComponents + i] / 255;
  }
  return m_pBaseCS->GetRGB(comps, R, G, B);
}

CPDF_ColorSpace* CPDF_IndexedCS::GetBaseCS() const {
  return m_pBaseCS;
}

void CPDF_IndexedCS::EnableStdConversion(FX_BOOL bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pBaseCS) {
    m_pBaseCS->EnableStdConversion(bEnabled);
  }
}

CPDF_PatternCS::CPDF_PatternCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_PATTERN, 1),
      m_pBaseCS(nullptr),
      m_pCountedBaseCS(nullptr) {}

CPDF_PatternCS::~CPDF_PatternCS() {
  CPDF_ColorSpace* pCS = m_pCountedBaseCS ? m_pCountedBaseCS->get() : nullptr;
  if (pCS && m_pDocument) {
    m_pDocument->GetPageData()->ReleaseColorSpace(pCS->GetArray());
  }
}

FX_BOOL CPDF_PatternCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Object* pBaseCS = pArray->GetDirectObjectAt(1);
  if (pBaseCS == m_pArray) {
    return FALSE;
  }
  CPDF_DocPageData* pDocPageData = pDoc->GetPageData();
  m_pBaseCS = pDocPageData->GetColorSpace(pBaseCS, nullptr);
  if (m_pBaseCS) {
    if (m_pBaseCS->GetFamily() == PDFCS_PATTERN) {
      return FALSE;
    }
    m_pCountedBaseCS = pDocPageData->FindColorSpacePtr(m_pBaseCS->GetArray());
    m_nComponents = m_pBaseCS->CountComponents() + 1;
    if (m_pBaseCS->CountComponents() > MAX_PATTERN_COLORCOMPS) {
      return FALSE;
    }
  } else {
    m_nComponents = 1;
  }
  return TRUE;
}

FX_BOOL CPDF_PatternCS::GetRGB(FX_FLOAT* pBuf,
                               FX_FLOAT& R,
                               FX_FLOAT& G,
                               FX_FLOAT& B) const {
  if (m_pBaseCS) {
    ASSERT(m_pBaseCS->GetFamily() != PDFCS_PATTERN);
    PatternValue* pvalue = (PatternValue*)pBuf;
    if (m_pBaseCS->GetRGB(pvalue->m_Comps, R, G, B)) {
      return TRUE;
    }
  }
  R = G = B = 0.75f;
  return FALSE;
}

CPDF_ColorSpace* CPDF_PatternCS::GetBaseCS() const {
  return m_pBaseCS;
}

CPDF_SeparationCS::CPDF_SeparationCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_SEPARATION, 1), m_pAltCS(nullptr) {}

CPDF_SeparationCS::~CPDF_SeparationCS() {
  if (m_pAltCS)
    m_pAltCS->ReleaseCS();
}

void CPDF_SeparationCS::GetDefaultValue(int iComponent,
                                        FX_FLOAT& value,
                                        FX_FLOAT& min,
                                        FX_FLOAT& max) const {
  value = 1.0f;
  min = 0;
  max = 1.0f;
}

FX_BOOL CPDF_SeparationCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CFX_ByteString name = pArray->GetStringAt(1);
  if (name == "None") {
    m_Type = None;
    return TRUE;
  }

  m_Type = Colorant;
  CPDF_Object* pAltCS = pArray->GetDirectObjectAt(2);
  if (pAltCS == m_pArray)
    return FALSE;

  m_pAltCS = Load(pDoc, pAltCS);
  if (!m_pAltCS)
    return FALSE;

  CPDF_Object* pFuncObj = pArray->GetDirectObjectAt(3);
  if (pFuncObj && !pFuncObj->IsName())
    m_pFunc = CPDF_Function::Load(pFuncObj);

  if (m_pFunc && m_pFunc->CountOutputs() < m_pAltCS->CountComponents())
    m_pFunc.reset();
  return TRUE;
}

FX_BOOL CPDF_SeparationCS::GetRGB(FX_FLOAT* pBuf,
                                  FX_FLOAT& R,
                                  FX_FLOAT& G,
                                  FX_FLOAT& B) const {
  if (m_Type == None)
    return FALSE;

  if (!m_pFunc) {
    if (!m_pAltCS)
      return FALSE;

    int nComps = m_pAltCS->CountComponents();
    CFX_FixedBufGrow<FX_FLOAT, 16> results(nComps);
    for (int i = 0; i < nComps; i++)
      results[i] = *pBuf;
    return m_pAltCS->GetRGB(results, R, G, B);
  }

  CFX_FixedBufGrow<FX_FLOAT, 16> results(m_pFunc->CountOutputs());
  int nresults = 0;
  m_pFunc->Call(pBuf, 1, results, nresults);
  if (nresults == 0)
    return FALSE;

  if (m_pAltCS)
    return m_pAltCS->GetRGB(results, R, G, B);

  R = 0;
  G = 0;
  B = 0;
  return FALSE;
}

void CPDF_SeparationCS::EnableStdConversion(FX_BOOL bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAltCS)
    m_pAltCS->EnableStdConversion(bEnabled);
}

CPDF_DeviceNCS::CPDF_DeviceNCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_DEVICEN, 0), m_pAltCS(nullptr) {}

CPDF_DeviceNCS::~CPDF_DeviceNCS() {
  if (m_pAltCS)
    m_pAltCS->ReleaseCS();
}

void CPDF_DeviceNCS::GetDefaultValue(int iComponent,
                                     FX_FLOAT& value,
                                     FX_FLOAT& min,
                                     FX_FLOAT& max) const {
  value = 1.0f;
  min = 0;
  max = 1.0f;
}

FX_BOOL CPDF_DeviceNCS::v_Load(CPDF_Document* pDoc, CPDF_Array* pArray) {
  CPDF_Array* pObj = ToArray(pArray->GetDirectObjectAt(1));
  if (!pObj)
    return FALSE;

  m_nComponents = pObj->GetCount();
  CPDF_Object* pAltCS = pArray->GetDirectObjectAt(2);
  if (!pAltCS || pAltCS == m_pArray)
    return FALSE;

  m_pAltCS = Load(pDoc, pAltCS);
  m_pFunc = CPDF_Function::Load(pArray->GetDirectObjectAt(3));
  if (!m_pAltCS || !m_pFunc)
    return FALSE;

  return m_pFunc->CountOutputs() >= m_pAltCS->CountComponents();
}

FX_BOOL CPDF_DeviceNCS::GetRGB(FX_FLOAT* pBuf,
                               FX_FLOAT& R,
                               FX_FLOAT& G,
                               FX_FLOAT& B) const {
  if (!m_pFunc)
    return FALSE;

  CFX_FixedBufGrow<FX_FLOAT, 16> results(m_pFunc->CountOutputs());
  int nresults = 0;
  m_pFunc->Call(pBuf, m_nComponents, results, nresults);
  if (nresults == 0)
    return FALSE;

  return m_pAltCS->GetRGB(results, R, G, B);
}

void CPDF_DeviceNCS::EnableStdConversion(FX_BOOL bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAltCS) {
    m_pAltCS->EnableStdConversion(bEnabled);
  }
}
