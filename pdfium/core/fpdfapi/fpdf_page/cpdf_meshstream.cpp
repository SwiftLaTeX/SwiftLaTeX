// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_meshstream.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_colorspace.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"

namespace {

// See PDF Reference 1.7, page 315, table 4.32. (Also table 4.33 and 4.34)
bool ShouldCheckBPC(ShadingType type) {
  switch (type) {
    case kFreeFormGouraudTriangleMeshShading:
    case kLatticeFormGouraudTriangleMeshShading:
    case kCoonsPatchMeshShading:
    case kTensorProductPatchMeshShading:
      return true;
    default:
      return false;
  }
}

// Same references as ShouldCheckBPC() above.
bool IsValidBitsPerComponent(uint32_t x) {
  switch (x) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 12:
    case 16:
      return true;
    default:
      return false;
  }
}

// Same references as ShouldCheckBPC() above.
bool IsValidBitsPerCoordinate(uint32_t x) {
  switch (x) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 12:
    case 16:
    case 24:
    case 32:
      return true;
    default:
      return false;
  }
}

// See PDF Reference 1.7, page 315, table 4.32. (Also table 4.34)
bool ShouldCheckBitsPerFlag(ShadingType type) {
  switch (type) {
    case kFreeFormGouraudTriangleMeshShading:
    case kCoonsPatchMeshShading:
    case kTensorProductPatchMeshShading:
      return true;
    default:
      return false;
  }
}

// Same references as ShouldCheckBitsPerFlag() above.
bool IsValidBitsPerFlag(uint32_t x) {
  switch (x) {
    case 2:
    case 4:
    case 8:
      return true;
    default:
      return false;
  }
}

}  // namespace

CPDF_MeshStream::CPDF_MeshStream(
    ShadingType type,
    const std::vector<std::unique_ptr<CPDF_Function>>& funcs,
    CPDF_Stream* pShadingStream,
    CPDF_ColorSpace* pCS)
    : m_type(type),
      m_funcs(funcs),
      m_pShadingStream(pShadingStream),
      m_pCS(pCS),
      m_nCoordBits(0),
      m_nComponentBits(0),
      m_nFlagBits(0),
      m_nComponents(0),
      m_CoordMax(0),
      m_ComponentMax(0),
      m_xmin(0),
      m_xmax(0),
      m_ymin(0),
      m_ymax(0) {
  memset(&m_ColorMin, 0, sizeof(m_ColorMin));
  memset(&m_ColorMax, 0, sizeof(m_ColorMax));
}

bool CPDF_MeshStream::Load() {
  m_Stream.LoadAllData(m_pShadingStream);
  m_BitStream.Init(m_Stream.GetData(), m_Stream.GetSize());
  CPDF_Dictionary* pDict = m_pShadingStream->GetDict();
  m_nCoordBits = pDict->GetIntegerBy("BitsPerCoordinate");
  m_nComponentBits = pDict->GetIntegerBy("BitsPerComponent");
  if (ShouldCheckBPC(m_type)) {
    if (!IsValidBitsPerCoordinate(m_nCoordBits))
      return false;
    if (!IsValidBitsPerComponent(m_nComponentBits))
      return false;
  }

  m_nFlagBits = pDict->GetIntegerBy("BitsPerFlag");
  if (ShouldCheckBitsPerFlag(m_type) && !IsValidBitsPerFlag(m_nFlagBits))
    return false;

  uint32_t nComponents = m_pCS->CountComponents();
  if (nComponents > kMaxComponents)
    return false;

  m_nComponents = m_funcs.empty() ? nComponents : 1;
  CPDF_Array* pDecode = pDict->GetArrayBy("Decode");
  if (!pDecode || pDecode->GetCount() != 4 + m_nComponents * 2)
    return false;

  m_xmin = pDecode->GetNumberAt(0);
  m_xmax = pDecode->GetNumberAt(1);
  m_ymin = pDecode->GetNumberAt(2);
  m_ymax = pDecode->GetNumberAt(3);
  for (uint32_t i = 0; i < m_nComponents; ++i) {
    m_ColorMin[i] = pDecode->GetNumberAt(i * 2 + 4);
    m_ColorMax[i] = pDecode->GetNumberAt(i * 2 + 5);
  }

  if (ShouldCheckBPC(m_type)) {
    m_CoordMax = m_nCoordBits == 32 ? -1 : (1 << m_nCoordBits) - 1;
    m_ComponentMax = (1 << m_nComponentBits) - 1;
  }
  return true;
}

uint32_t CPDF_MeshStream::GetFlag() {
  ASSERT(ShouldCheckBitsPerFlag(m_type));
  return m_BitStream.GetBits(m_nFlagBits) & 0x03;
}

void CPDF_MeshStream::GetCoords(FX_FLOAT& x, FX_FLOAT& y) {
  ASSERT(ShouldCheckBPC(m_type));
  if (m_nCoordBits == 32) {
    x = m_xmin + (FX_FLOAT)(m_BitStream.GetBits(m_nCoordBits) *
                            (m_xmax - m_xmin) / (double)m_CoordMax);
    y = m_ymin + (FX_FLOAT)(m_BitStream.GetBits(m_nCoordBits) *
                            (m_ymax - m_ymin) / (double)m_CoordMax);
  } else {
    x = m_xmin +
        m_BitStream.GetBits(m_nCoordBits) * (m_xmax - m_xmin) / m_CoordMax;
    y = m_ymin +
        m_BitStream.GetBits(m_nCoordBits) * (m_ymax - m_ymin) / m_CoordMax;
  }
}

void CPDF_MeshStream::GetColor(FX_FLOAT& r, FX_FLOAT& g, FX_FLOAT& b) {
  ASSERT(ShouldCheckBPC(m_type));
  FX_FLOAT color_value[kMaxComponents];
  for (uint32_t i = 0; i < m_nComponents; ++i) {
    color_value[i] = m_ColorMin[i] +
                     m_BitStream.GetBits(m_nComponentBits) *
                         (m_ColorMax[i] - m_ColorMin[i]) / m_ComponentMax;
  }
  if (m_funcs.empty()) {
    m_pCS->GetRGB(color_value, r, g, b);
    return;
  }

  FX_FLOAT result[kMaxComponents];
  FXSYS_memset(result, 0, sizeof(result));
  int nResults;
  for (const auto& func : m_funcs) {
    if (func && func->CountOutputs() <= kMaxComponents)
      func->Call(color_value, 1, result, nResults);
  }
  m_pCS->GetRGB(result, r, g, b);
}

uint32_t CPDF_MeshStream::GetVertex(CPDF_MeshVertex& vertex,
                                    CFX_Matrix* pObject2Bitmap) {
  uint32_t flag = GetFlag();
  GetCoords(vertex.x, vertex.y);
  pObject2Bitmap->Transform(vertex.x, vertex.y);
  GetColor(vertex.r, vertex.g, vertex.b);
  m_BitStream.ByteAlign();
  return flag;
}

FX_BOOL CPDF_MeshStream::GetVertexRow(CPDF_MeshVertex* vertex,
                                      int count,
                                      CFX_Matrix* pObject2Bitmap) {
  for (int i = 0; i < count; i++) {
    if (m_BitStream.IsEOF())
      return FALSE;

    GetCoords(vertex[i].x, vertex[i].y);
    pObject2Bitmap->Transform(vertex[i].x, vertex[i].y);
    GetColor(vertex[i].r, vertex[i].g, vertex[i].b);
    m_BitStream.ByteAlign();
  }
  return TRUE;
}
