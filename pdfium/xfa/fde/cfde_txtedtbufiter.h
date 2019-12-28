// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTBUFITER_H_
#define XFA_FDE_CFDE_TXTEDTBUFITER_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/ifx_chariter.h"

class CFDE_TxtEdtBuf;

class CFDE_TxtEdtBufIter : public IFX_CharIter {
 public:
  CFDE_TxtEdtBufIter(CFDE_TxtEdtBuf* pBuf, FX_WCHAR wcAlias = 0);
  ~CFDE_TxtEdtBufIter() override;

  FX_BOOL Next(FX_BOOL bPrev = FALSE) override;
  FX_WCHAR GetChar() override;
  void SetAt(int32_t nIndex) override;
  int32_t GetAt() const override;
  FX_BOOL IsEOF(FX_BOOL bTail = TRUE) const override;
  IFX_CharIter* Clone() override;

 private:
  CFDE_TxtEdtBuf* m_pBuf;
  int32_t m_nCurChunk;
  int32_t m_nCurIndex;
  int32_t m_nIndex;
  FX_WCHAR m_Alias;
};

#endif  // XFA_FDE_CFDE_TXTEDTBUFITER_H_
