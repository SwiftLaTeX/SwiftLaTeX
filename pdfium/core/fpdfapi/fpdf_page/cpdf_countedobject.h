// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_CPDF_COUNTEDOBJECT_H_
#define CORE_FPDFAPI_FPDF_PAGE_CPDF_COUNTEDOBJECT_H_

#include "core/fpdfapi/fpdf_page/cpdf_pattern.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_colorspace.h"
#include "core/fxcrt/include/fx_system.h"

template <class T>
class CPDF_CountedObject {
 public:
  explicit CPDF_CountedObject(T* ptr) : m_nCount(1), m_pObj(ptr) {}
  void reset(T* ptr) {  // CAUTION: tosses prior ref counts.
    m_nCount = 1;
    m_pObj = ptr;
  }
  void clear() {  // Now you're all weak ptrs ...
    delete m_pObj;
    m_pObj = nullptr;
  }
  T* get() const { return m_pObj; }
  T* AddRef() {
    ASSERT(m_pObj);
    ++m_nCount;
    return m_pObj;
  }
  void RemoveRef() {
    if (m_nCount)
      --m_nCount;
  }
  size_t use_count() const { return m_nCount; }

 protected:
  size_t m_nCount;
  T* m_pObj;
};
using CPDF_CountedColorSpace = CPDF_CountedObject<CPDF_ColorSpace>;
using CPDF_CountedPattern = CPDF_CountedObject<CPDF_Pattern>;

#endif  // CORE_FPDFAPI_FPDF_PAGE_CPDF_COUNTEDOBJECT_H_
