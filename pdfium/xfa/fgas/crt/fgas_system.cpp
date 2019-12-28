// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/fgas_system.h"

#include <algorithm>

#include "core/fxcrt/include/fx_system.h"

namespace {

inline FX_BOOL FX_isupper(int32_t ch) {
  return ch >= 'A' && ch <= 'Z';
}

inline int32_t FX_tolower(int32_t ch) {
  return FX_isupper(ch) ? (ch + 0x20) : ch;
}

}  // namespace

int32_t FX_wcsnicmp(const FX_WCHAR* s1, const FX_WCHAR* s2, size_t count) {
  ASSERT(s1 && s2 && count > 0);
  FX_WCHAR wch1 = 0;
  FX_WCHAR wch2 = 0;
  while (count-- > 0) {
    wch1 = (FX_WCHAR)FX_tolower(*s1++);
    wch2 = (FX_WCHAR)FX_tolower(*s2++);
    if (wch1 != wch2) {
      break;
    }
  }
  return wch1 - wch2;
}

FX_FLOAT FX_wcstof(const FX_WCHAR* pwsStr, int32_t iLength, int32_t* pUsedLen) {
  ASSERT(pwsStr);
  if (iLength < 0) {
    iLength = FXSYS_wcslen(pwsStr);
  }
  if (iLength == 0) {
    return 0.0f;
  }
  int32_t iUsedLen = 0;
  FX_BOOL bNegtive = FALSE;
  switch (pwsStr[iUsedLen]) {
    case '-':
      bNegtive = TRUE;
    case '+':
      iUsedLen++;
      break;
  }
  FX_FLOAT fValue = 0.0f;
  while (iUsedLen < iLength) {
    FX_WCHAR wch = pwsStr[iUsedLen];
    if (wch >= L'0' && wch <= L'9') {
      fValue = fValue * 10.0f + (wch - L'0');
    } else {
      break;
    }
    iUsedLen++;
  }
  if (iUsedLen < iLength && pwsStr[iUsedLen] == L'.') {
    FX_FLOAT fPrecise = 0.1f;
    while (++iUsedLen < iLength) {
      FX_WCHAR wch = pwsStr[iUsedLen];
      if (wch >= L'0' && wch <= L'9') {
        fValue += (wch - L'0') * fPrecise;
        fPrecise *= 0.1f;
      } else {
        break;
      }
    }
  }
  if (pUsedLen) {
    *pUsedLen = iUsedLen;
  }
  return bNegtive ? -fValue : fValue;
}
