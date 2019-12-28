// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/include/fx_memory.h"
#include "core/fxcrt/plex.h"

CFX_Plex* CFX_Plex::Create(CFX_Plex*& pHead,
                           uint32_t nMax,
                           uint32_t cbElement) {
  CFX_Plex* p =
      (CFX_Plex*)FX_Alloc(uint8_t, sizeof(CFX_Plex) + nMax * cbElement);
  p->pNext = pHead;
  pHead = p;
  return p;
}
void CFX_Plex::FreeDataChain() {
  CFX_Plex* p = this;
  while (p) {
    CFX_Plex* old = p;
    p = p->pNext;
    FX_Free(old);
  }
}
