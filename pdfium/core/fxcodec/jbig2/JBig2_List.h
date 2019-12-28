// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_LIST_H_
#define CORE_FXCODEC_JBIG2_JBIG2_LIST_H_

#include <stdlib.h>

#include <vector>

// A poor man's ScopedVector for pointers of TYPE.
// Owns all the pointers contained within and deletes them on destruction.
template <class TYPE>
class CJBig2_List {
 public:
  CJBig2_List() {}
  explicit CJBig2_List(size_t count) { resize(count); }

  ~CJBig2_List() { clear(); }

  TYPE* get(size_t index) const { return m_vector[index]; }
  TYPE* back() const { return m_vector.back(); }
  size_t size() const { return m_vector.size(); }

  // Deletes all the pointers contained within.
  void clear() {
    for (size_t i = 0; i < m_vector.size(); ++i)
      delete m_vector[i];
    m_vector.clear();
  }

  // Takes ownership of |pItem|.
  void push_back(TYPE* pItem) { m_vector.push_back(pItem); }

  // Takes ownership of |pItem|.
  void set(size_t index, TYPE* pItem) {
    delete m_vector[index];
    m_vector[index] = pItem;
  }

  void resize(size_t count) {
    for (size_t i = count; i < size(); ++i)
      delete m_vector[i];
    m_vector.resize(count);
  }

 private:
  std::vector<TYPE*> m_vector;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_LIST_H_
