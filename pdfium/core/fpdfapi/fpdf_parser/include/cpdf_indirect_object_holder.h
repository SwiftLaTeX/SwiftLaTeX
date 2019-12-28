// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_INDIRECT_OBJECT_HOLDER_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_INDIRECT_OBJECT_HOLDER_H_

#include <map>

#include "core/fxcrt/include/fx_system.h"

class CPDF_Object;
class CPDF_Parser;

class CPDF_IndirectObjectHolder {
 public:
  using iterator = std::map<uint32_t, CPDF_Object*>::iterator;
  using const_iterator = std::map<uint32_t, CPDF_Object*>::const_iterator;

  explicit CPDF_IndirectObjectHolder(CPDF_Parser* pParser);
  ~CPDF_IndirectObjectHolder();

  CPDF_Object* GetIndirectObject(uint32_t objnum);
  uint32_t AddIndirectObject(CPDF_Object* pObj);
  void ReleaseIndirectObject(uint32_t objnum);

  // Takes ownership of |pObj|.
  bool InsertIndirectObject(uint32_t objnum, CPDF_Object* pObj);

  uint32_t GetLastObjNum() const { return m_LastObjNum; }
  iterator begin() { return m_IndirectObjs.begin(); }
  const_iterator begin() const { return m_IndirectObjs.begin(); }
  iterator end() { return m_IndirectObjs.end(); }
  const_iterator end() const { return m_IndirectObjs.end(); }

 protected:
  CPDF_Parser* m_pParser;
  uint32_t m_LastObjNum;
  std::map<uint32_t, CPDF_Object*> m_IndirectObjs;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_INDIRECT_OBJECT_HOLDER_H_
