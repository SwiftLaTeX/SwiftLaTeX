// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/fgas_unicode.h"

void FX_TEXTLAYOUT_PieceSort(CFX_TPOArray& tpos, int32_t iStart, int32_t iEnd) {
  ASSERT(iStart > -1 && iStart < tpos.GetSize());
  ASSERT(iEnd > -1 && iEnd < tpos.GetSize());
  if (iStart >= iEnd) {
    return;
  }
  int32_t i = iStart, j = iEnd;
  FX_TPO *pCur = tpos.GetPtrAt(iStart), *pSort;
  int32_t v = pCur->pos;
  while (i < j) {
    while (j > i) {
      pSort = tpos.GetPtrAt(j);
      if (pSort->pos < v) {
        FX_TPO t = *pSort;
        *pSort = *pCur;
        *pCur = t;
        pCur = pSort;
        break;
      }
      j--;
    }
    while (i < j) {
      pSort = tpos.GetPtrAt(i);
      if (pSort->pos > v) {
        FX_TPO t = *pSort;
        *pSort = *pCur;
        *pCur = t;
        pCur = pSort;
        break;
      }
      i++;
    }
  }
  i--, j++;
  if (iStart < i) {
    FX_TEXTLAYOUT_PieceSort(tpos, iStart, i);
  }
  if (j < iEnd) {
    FX_TEXTLAYOUT_PieceSort(tpos, j, iEnd);
  }
}
