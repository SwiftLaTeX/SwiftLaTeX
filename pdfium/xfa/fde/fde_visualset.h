// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_FDE_VISUALSET_H_
#define XFA_FDE_FDE_VISUALSET_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"
#include "xfa/fde/cfde_path.h"
#include "xfa/fde/fde_object.h"
#include "xfa/fgas/crt/fgas_memory.h"
#include "xfa/fgas/font/fgas_font.h"

enum FDE_VISUALOBJTYPE {
  FDE_VISUALOBJ_Canvas = 0x00,
  FDE_VISUALOBJ_Text = 0x01
};

struct FDE_TEXTEDITPIECE {
  int32_t nStart;
  int32_t nCount;
  int32_t nBidiLevel;
  CFX_RectF rtPiece;
  uint32_t dwCharStyles;
};

class IFDE_VisualSet {
 public:
  virtual ~IFDE_VisualSet() {}
  virtual FDE_VISUALOBJTYPE GetType() = 0;
  virtual void GetRect(FDE_TEXTEDITPIECE* hVisualObj, CFX_RectF& rt) = 0;
};

class IFDE_CanvasSet : public IFDE_VisualSet {
 public:
  virtual FX_POSITION GetFirstPosition() = 0;
  virtual FDE_TEXTEDITPIECE* GetNext(FX_POSITION& pos,
                                     IFDE_VisualSet*& pVisualSet) = 0;
};

class IFDE_TextSet : public IFDE_VisualSet {
 public:
  virtual int32_t GetString(FDE_TEXTEDITPIECE* hText,
                            CFX_WideString& wsText) = 0;
  virtual CFGAS_GEFont* GetFont() = 0;
  virtual FX_FLOAT GetFontSize() = 0;
  virtual FX_ARGB GetFontColor() = 0;
  virtual int32_t GetDisplayPos(FDE_TEXTEDITPIECE* hText,
                                FXTEXT_CHARPOS* pCharPos,
                                FX_BOOL bCharCode = FALSE,
                                CFX_WideString* pWSForms = nullptr) = 0;
  virtual int32_t GetCharRects(const FDE_TEXTEDITPIECE* hText,
                               CFX_RectFArray& rtArray,
                               FX_BOOL bbox) = 0;
};

#endif  // XFA_FDE_FDE_VISUALSET_H_
