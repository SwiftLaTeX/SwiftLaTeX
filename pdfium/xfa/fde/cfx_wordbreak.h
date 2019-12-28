// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFX_WORDBREAK_H_
#define XFA_FDE_CFX_WORDBREAK_H_

#include <memory>

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class IFX_CharIter;

class CFX_WordBreak {
 public:
  CFX_WordBreak();
  ~CFX_WordBreak();

  void Attach(IFX_CharIter* pIter);
  void Attach(const CFX_WideString& wsText);
  FX_BOOL Next(FX_BOOL bPrev);
  void SetAt(int32_t nIndex);
  int32_t GetWordPos() const;
  int32_t GetWordLength() const;
  void GetWord(CFX_WideString& wsWord) const;
  FX_BOOL IsEOF(FX_BOOL bTail) const;

 protected:
  FX_BOOL FindNextBreakPos(IFX_CharIter* pIter,
                           FX_BOOL bPrev,
                           FX_BOOL bFromNext = TRUE);

 private:
  std::unique_ptr<IFX_CharIter> m_pPreIter;
  std::unique_ptr<IFX_CharIter> m_pCurIter;
};

#endif  // XFA_FDE_CFX_WORDBREAK_H_
