// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_IFX_CHARITER_H_
#define XFA_FDE_IFX_CHARITER_H_

#include "core/fxcrt/include/fx_system.h"

class IFX_CharIter {
 public:
  virtual ~IFX_CharIter() {}

  virtual FX_BOOL Next(FX_BOOL bPrev = FALSE) = 0;
  virtual FX_WCHAR GetChar() = 0;
  virtual void SetAt(int32_t nIndex) = 0;
  virtual int32_t GetAt() const = 0;
  virtual FX_BOOL IsEOF(FX_BOOL bTail = TRUE) const = 0;
  virtual IFX_CharIter* Clone() = 0;
};

#endif  // XFA_FDE_IFX_CHARITER_H_
