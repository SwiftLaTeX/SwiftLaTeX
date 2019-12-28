// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DICTIONARY_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DICTIONARY_H_

#include <map>

#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_string.h"

class CPDF_IndirectObjectHolder;

class CPDF_Dictionary : public CPDF_Object {
 public:
  using iterator = std::map<CFX_ByteString, CPDF_Object*>::iterator;
  using const_iterator = std::map<CFX_ByteString, CPDF_Object*>::const_iterator;

  CPDF_Dictionary();

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  CPDF_Dictionary* GetDict() const override;
  bool IsDictionary() const override;
  CPDF_Dictionary* AsDictionary() override;
  const CPDF_Dictionary* AsDictionary() const override;

  size_t GetCount() const { return m_Map.size(); }
  CPDF_Object* GetObjectBy(const CFX_ByteString& key) const;
  CPDF_Object* GetDirectObjectBy(const CFX_ByteString& key) const;
  CFX_ByteString GetStringBy(const CFX_ByteString& key) const;
  CFX_ByteString GetStringBy(const CFX_ByteString& key,
                             const CFX_ByteString& default_str) const;
  CFX_WideString GetUnicodeTextBy(const CFX_ByteString& key) const;
  int GetIntegerBy(const CFX_ByteString& key) const;
  int GetIntegerBy(const CFX_ByteString& key, int default_int) const;
  bool GetBooleanBy(const CFX_ByteString& key, bool bDefault = false) const;
  FX_FLOAT GetNumberBy(const CFX_ByteString& key) const;
  CPDF_Dictionary* GetDictBy(const CFX_ByteString& key) const;
  CPDF_Stream* GetStreamBy(const CFX_ByteString& key) const;
  CPDF_Array* GetArrayBy(const CFX_ByteString& key) const;
  CFX_FloatRect GetRectBy(const CFX_ByteString& key) const;
  CFX_Matrix GetMatrixBy(const CFX_ByteString& key) const;
  FX_FLOAT GetFloatBy(const CFX_ByteString& key) const {
    return GetNumberBy(key);
  }

  FX_BOOL KeyExist(const CFX_ByteString& key) const;
  bool IsSignatureDict() const;

  // Set* functions invalidate iterators for the element with the key |key|.
  void SetAt(const CFX_ByteString& key, CPDF_Object* pObj);
  void SetAtName(const CFX_ByteString& key, const CFX_ByteString& name);
  void SetAtString(const CFX_ByteString& key, const CFX_ByteString& str);
  void SetAtInteger(const CFX_ByteString& key, int i);
  void SetAtNumber(const CFX_ByteString& key, FX_FLOAT f);
  void SetAtReference(const CFX_ByteString& key,
                      CPDF_IndirectObjectHolder* pDoc,
                      uint32_t objnum);
  void SetAtReference(const CFX_ByteString& key,
                      CPDF_IndirectObjectHolder* pDoc,
                      CPDF_Object* obj) {
    SetAtReference(key, pDoc, obj->GetObjNum());
  }
  void SetAtRect(const CFX_ByteString& key, const CFX_FloatRect& rect);
  void SetAtMatrix(const CFX_ByteString& key, const CFX_Matrix& matrix);
  void SetAtBoolean(const CFX_ByteString& key, bool bValue);

  // Invalidates iterators for the element with the key |key|.
  void RemoveAt(const CFX_ByteString& key);

  // Invalidates iterators for the element with the key |oldkey|.
  void ReplaceKey(const CFX_ByteString& oldkey, const CFX_ByteString& newkey);

  iterator begin() { return m_Map.begin(); }
  iterator end() { return m_Map.end(); }
  const_iterator begin() const { return m_Map.begin(); }
  const_iterator end() const { return m_Map.end(); }

 protected:
  ~CPDF_Dictionary() override;

  std::map<CFX_ByteString, CPDF_Object*> m_Map;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_DICTIONARY_H_
