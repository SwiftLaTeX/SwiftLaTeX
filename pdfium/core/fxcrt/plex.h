// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_PLEX_H_
#define CORE_FXCRT_PLEX_H_

#include "core/fxcrt/include/fx_system.h"

struct CFX_Plex {
  CFX_Plex* pNext;
  void* data() { return this + 1; }
  static CFX_Plex* Create(CFX_Plex*& head, uint32_t nMax, uint32_t cbElement);
  void FreeDataChain();
};

#endif  // CORE_FXCRT_PLEX_H_
