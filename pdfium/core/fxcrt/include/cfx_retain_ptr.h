// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_INCLUDE_CFX_RETAIN_PTR_H_
#define CORE_FXCRT_INCLUDE_CFX_RETAIN_PTR_H_

#include <memory>

#include "core/fxcrt/include/fx_memory.h"

template <class T>
class CFX_RetainPtr {
 public:
  explicit CFX_RetainPtr(T* pObj) : m_pObj(pObj) {
    if (m_pObj)
      m_pObj->Retain();
  }

  CFX_RetainPtr() : CFX_RetainPtr(nullptr) {}
  CFX_RetainPtr(const CFX_RetainPtr& that) : CFX_RetainPtr(that.Get()) {}
  CFX_RetainPtr(CFX_RetainPtr&& that) { Swap(that); }

  template <class U>
  CFX_RetainPtr(const CFX_RetainPtr<U>& that)
      : CFX_RetainPtr(that.Get()) {}

  void Reset(T* obj = nullptr) {
    if (obj)
      obj->Retain();
    m_pObj.reset(obj);
  }

  T* Get() const { return m_pObj.get(); }
  void Swap(CFX_RetainPtr& that) { m_pObj.swap(that.m_pObj); }

  CFX_RetainPtr& operator=(const CFX_RetainPtr& that) {
    if (*this != that)
      Reset(that.Get());
    return *this;
  }

  bool operator==(const CFX_RetainPtr& that) const {
    return Get() == that.Get();
  }

  bool operator!=(const CFX_RetainPtr& that) const { return !(*this == that); }

  operator bool() const { return !!m_pObj; }
  T& operator*() const { return *m_pObj.get(); }
  T* operator->() const { return m_pObj.get(); }

 private:
  std::unique_ptr<T, ReleaseDeleter<T>> m_pObj;
};

#endif  // CORE_FXCRT_INCLUDE_CFX_RETAIN_PTR_H_
