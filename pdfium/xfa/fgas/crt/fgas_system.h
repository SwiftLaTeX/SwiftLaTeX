// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_CRT_FGAS_SYSTEM_H_
#define XFA_FGAS_CRT_FGAS_SYSTEM_H_

#include "core/fxcrt/include/fx_system.h"

FX_FLOAT FX_wcstof(const FX_WCHAR* pwsStr,
                   int32_t iLength = -1,
                   int32_t* pUsedLen = nullptr);
int32_t FX_wcsnicmp(const FX_WCHAR* s1, const FX_WCHAR* s2, size_t count);

#endif  // XFA_FGAS_CRT_FGAS_SYSTEM_H_
