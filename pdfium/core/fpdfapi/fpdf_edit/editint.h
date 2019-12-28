// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_EDIT_EDITINT_H_
#define CORE_FPDFAPI_FPDF_EDIT_EDITINT_H_

#include <vector>

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_stream.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Creator;
class CPDF_Object;

class CPDF_ObjectStream {
 public:
  struct Item {
    uint32_t objnum;
    FX_STRSIZE offset;
  };

  CPDF_ObjectStream();
  ~CPDF_ObjectStream();

  void Start();
  FX_FILESIZE End(CPDF_Creator* pCreator);
  void CompressIndirectObject(uint32_t dwObjNum, const CPDF_Object* pObj);
  void CompressIndirectObject(uint32_t dwObjNum,
                              const uint8_t* pBuffer,
                              uint32_t dwSize);

  std::vector<Item> m_Items;
  CFX_ByteTextBuf m_Buffer;
  uint32_t m_dwObjNum;
  int32_t m_index;
};

class CPDF_XRefStream {
 public:
  struct Index {
    uint32_t objnum;
    uint32_t count;
  };

  CPDF_XRefStream();
  ~CPDF_XRefStream();

  FX_BOOL Start();
  int32_t CompressIndirectObject(uint32_t dwObjNum,
                                 const CPDF_Object* pObj,
                                 CPDF_Creator* pCreator);
  int32_t CompressIndirectObject(uint32_t dwObjNum,
                                 const uint8_t* pBuffer,
                                 uint32_t dwSize,
                                 CPDF_Creator* pCreator);
  FX_BOOL End(CPDF_Creator* pCreator, FX_BOOL bEOF = FALSE);
  void AddObjectNumberToIndexArray(uint32_t objnum);
  FX_BOOL EndXRefStream(CPDF_Creator* pCreator);

  std::vector<Index> m_IndexArray;
  FX_FILESIZE m_PrevOffset;
  uint32_t m_dwTempObjNum;

 protected:
  int32_t EndObjectStream(CPDF_Creator* pCreator, FX_BOOL bEOF = TRUE);
  FX_BOOL GenerateXRefStream(CPDF_Creator* pCreator, FX_BOOL bEOF);

  size_t m_iSeg;
  CPDF_ObjectStream m_ObjStream;
  CFX_ByteTextBuf m_Buffer;
};

#endif  // CORE_FPDFAPI_FPDF_EDIT_EDITINT_H_
