// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_REFERENCE_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_REFERENCE_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"

class CPDF_IndirectObjectHolder;

class CPDF_Reference : public CPDF_Object {
 public:
  CPDF_Reference(CPDF_IndirectObjectHolder* pDoc, int objnum);

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  CPDF_Object* GetDirect() const override;
  CFX_ByteString GetString() const override;
  FX_FLOAT GetNumber() const override;
  int GetInteger() const override;
  CPDF_Dictionary* GetDict() const override;

  // TODO(weili): check whether GetUnicodeText() and GetArray() are needed.
  bool IsReference() const override;
  CPDF_Reference* AsReference() override;
  const CPDF_Reference* AsReference() const override;

  CPDF_IndirectObjectHolder* GetObjList() const { return m_pObjList; }
  uint32_t GetRefObjNum() const { return m_RefObjNum; }

  void SetRef(CPDF_IndirectObjectHolder* pDoc, uint32_t objnum);

 protected:
  ~CPDF_Reference() override;
  CPDF_Object* SafeGetDirect() const {
    CPDF_Object* obj = GetDirect();
    if (!obj || obj->IsReference())
      return nullptr;
    return obj;
  }

  CPDF_IndirectObjectHolder* m_pObjList;
  uint32_t m_RefObjNum;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_REFERENCE_H_
