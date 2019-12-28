// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_ARRAY_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_ARRAY_H_

#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"

class CPDF_Array : public CPDF_Object {
 public:
  using iterator = std::vector<CPDF_Object*>::iterator;
  using const_iterator = std::vector<CPDF_Object*>::const_iterator;

  CPDF_Array();

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  bool IsArray() const override;
  CPDF_Array* AsArray() override;
  const CPDF_Array* AsArray() const override;

  size_t GetCount() const { return m_Objects.size(); }
  CPDF_Object* GetObjectAt(size_t index) const;
  CPDF_Object* GetDirectObjectAt(size_t index) const;
  CFX_ByteString GetStringAt(size_t index) const;
  int GetIntegerAt(size_t index) const;
  FX_FLOAT GetNumberAt(size_t index) const;
  CPDF_Dictionary* GetDictAt(size_t index) const;
  CPDF_Stream* GetStreamAt(size_t index) const;
  CPDF_Array* GetArrayAt(size_t index) const;
  FX_FLOAT GetFloatAt(size_t index) const { return GetNumberAt(index); }
  CFX_Matrix GetMatrix();
  CFX_FloatRect GetRect();

  void SetAt(size_t index,
             CPDF_Object* pObj,
             CPDF_IndirectObjectHolder* pObjs = nullptr);
  void InsertAt(size_t index,
                CPDF_Object* pObj,
                CPDF_IndirectObjectHolder* pObjs = nullptr);
  void RemoveAt(size_t index, size_t nCount = 1);

  void Add(CPDF_Object* pObj, CPDF_IndirectObjectHolder* pObjs = nullptr);
  void AddNumber(FX_FLOAT f);
  void AddInteger(int i);
  void AddString(const CFX_ByteString& str);
  void AddName(const CFX_ByteString& str);
  void AddReference(CPDF_IndirectObjectHolder* pDoc, uint32_t objnum);
  void AddReference(CPDF_IndirectObjectHolder* pDoc, CPDF_Object* obj) {
    AddReference(pDoc, obj->GetObjNum());
  }

  iterator begin() { return m_Objects.begin(); }
  iterator end() { return m_Objects.end(); }
  const_iterator begin() const { return m_Objects.begin(); }
  const_iterator end() const { return m_Objects.end(); }

 protected:
  ~CPDF_Array() override;

  std::vector<CPDF_Object*> m_Objects;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_ARRAY_H_
