// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_IFDE_TXTEDTENGINE_H_
#define XFA_FDE_IFDE_TXTEDTENGINE_H_

#include "core/fxge/include/fx_dib.h"
#include "xfa/fgas/font/fgas_font.h"

class CFDE_TxtEdtEngine;
class CFWL_EditImp;
class IFDE_TxtEdtPage;

#define FDE_TEXTEDITMODE_MultiLines (1L << 0)
#define FDE_TEXTEDITMODE_AutoLineWrap (1L << 1)
#define FDE_TEXTEDITMODE_ReadOnly (1L << 2)
#define FDE_TEXTEDITMODE_LimitArea_Vert (1L << 3)
#define FDE_TEXTEDITMODE_LimitArea_Horz (1L << 4)
#define FDE_TEXTEDITMODE_NoRedoUndo (1L << 5)
#define FDE_TEXTEDITMODE_FIELD_TAB (1L << 6)
#define FDE_TEXTEDITMODE_FIELD_AUTO (1L << 7)
#define FDE_TEXTEDITMODE_Validate (1L << 8)
#define FDE_TEXTEDITMODE_Password (1L << 9)

#define FDE_TEXTEDITALIGN_Left (0L << 0)
#define FDE_TEXTEDITALIGN_Center (1L << 0)
#define FDE_TEXTEDITALIGN_Right (2L << 0)
#define FDE_TEXTEDITALIGN_Normal (1L << 3)
#define FDE_TEXTEDITALIGN_Justified (1L << 4)
#define FDE_TEXTEDITALIGN_Distributed (1L << 5)

#define FDE_TEXTEDITLAYOUT_DocVertical (1L << 0)
#define FDE_TEXTEDITLAYOUT_CharVertial (1L << 1)
#define FDE_TEXTEDITLAYOUT_LineReserve (1L << 2)
#define FDE_TEXTEDITLAYOUT_RTL (1L << 3)
#define FDE_TEXTEDITLAYOUT_CombText (1L << 4)
#define FDE_TEXTEDITLAYOUT_ExpandTab (1L << 5)
#define FDE_TEXTEDITLAYOUT_ArabicContext (1L << 6)
#define FDE_TEXTEDITLAYOUT_ArabicShapes (1L << 7)
#define FDE_TEXTEDITLAYOUT_LastLineHeight (1L << 8)

enum FDE_TXTEDTMOVECARET {
  MC_MoveNone = 0,
  MC_Left,
  MC_Right,
  MC_Up,
  MC_Down,
  MC_WordBackward,
  MC_WordForward,
  MC_LineStart,
  MC_LineEnd,
  MC_ParagStart,
  MC_ParagEnd,
  MC_PageUp,
  MC_PageDown,
  MC_Home,
  MC_End,
};

enum FDE_TXTEDT_MODIFY_RET {
  FDE_TXTEDT_MODIFY_RET_F_Tab = -6,
  FDE_TXTEDT_MODIFY_RET_F_Locked = -5,
  FDE_TXTEDT_MODIFY_RET_F_Invalidate = -4,
  FDE_TXTEDT_MODIFY_RET_F_Boundary = -3,
  FDE_TXTEDT_MODIFY_RET_F_Full = -2,
  FDE_TXTEDT_MODIFY_RET_F_Normal = -1,
  FDE_TXTEDT_MODIFY_RET_S_Normal = 0,
  FDE_TXTEDT_MODIFY_RET_S_Full = 1,
  FDE_TXTEDT_MODIFY_RET_S_Part = 2,
  FDE_TXTEDT_MODIFY_RET_S_Empty = 3,
  FDE_TXTEDT_MODIFY_RET_T_Tab = 4,
};

enum FDE_TXTEDIT_LINEEND {
  FDE_TXTEDIT_LINEEND_Auto,
  FDE_TXTEDIT_LINEEND_CRLF,
  FDE_TXTEDIT_LINEEND_CR,
  FDE_TXTEDIT_LINEEND_LF,
};

struct FDE_TXTEDTPARAMS {
  FDE_TXTEDTPARAMS();

  FX_FLOAT fPlateWidth;
  FX_FLOAT fPlateHeight;
  int32_t nLineCount;
  uint32_t dwLayoutStyles;
  uint32_t dwAlignment;
  uint32_t dwMode;
  CFGAS_GEFont* pFont;
  FX_FLOAT fFontSize;
  FX_ARGB dwFontColor;
  FX_FLOAT fLineSpace;
  FX_FLOAT fTabWidth;
  FX_BOOL bTabEquidistant;
  FX_WCHAR wDefChar;
  FX_WCHAR wLineBreakChar;
  int32_t nCharRotation;
  int32_t nLineEnd;
  int32_t nHorzScale;
  FX_FLOAT fCharSpace;
  CFWL_EditImp* pEventSink;
};

enum FDE_TXTEDT_TEXTCHANGE_TYPE {
  FDE_TXTEDT_TEXTCHANGE_TYPE_Insert = 0,
  FDE_TXTEDT_TEXTCHANGE_TYPE_Delete,
  FDE_TXTEDT_TEXTCHANGE_TYPE_Replace,
};

struct FDE_TXTEDT_TEXTCHANGE_INFO {
  FDE_TXTEDT_TEXTCHANGE_INFO();
  ~FDE_TXTEDT_TEXTCHANGE_INFO();

  int32_t nChangeType;
  CFX_WideString wsInsert;
  CFX_WideString wsDelete;
  CFX_WideString wsPrevText;
};

#endif  // XFA_FDE_IFDE_TXTEDTENGINE_H_
