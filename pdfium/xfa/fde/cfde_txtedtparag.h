// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTPARAG_H_
#define XFA_FDE_CFDE_TXTEDTPARAG_H_

#include <stdint.h>

class CFDE_TxtEdtEngine;

class CFDE_TxtEdtParag {
 public:
  explicit CFDE_TxtEdtParag(CFDE_TxtEdtEngine* pEngine);
  ~CFDE_TxtEdtParag();

  int32_t GetTextLength() const { return m_nCharCount; }
  int32_t GetStartIndex() const { return m_nCharStart; }
  int32_t GetLineCount() const { return m_nLineCount; }

  void SetTextLength(int32_t len) { m_nCharCount = len; }
  void IncrementTextLength(int32_t len) { m_nCharCount += len; }
  void SetStartIndex(int32_t idx) { m_nCharStart = idx; }
  void IncrementStartIndex(int32_t val) { m_nCharStart += val; }
  void DecrementStartIndex(int32_t val) { m_nCharStart -= val; }
  void SetLineCount(int32_t count) { m_nLineCount = count; }

  void GetLineRange(int32_t nLineIndex, int32_t& nStart, int32_t& nCount) const;
  void LoadParag();
  void UnloadParag();
  void CalcLines();

 private:
  int32_t m_nCharStart;
  int32_t m_nCharCount;
  int32_t m_nLineCount;
  int32_t* m_lpData;
  CFDE_TxtEdtEngine* m_pEngine;
};

#endif  // XFA_FDE_CFDE_TXTEDTPARAG_H_
