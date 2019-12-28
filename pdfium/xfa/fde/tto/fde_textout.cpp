// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/tto/fde_textout.h"

#include <algorithm>

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/cfde_path.h"
#include "xfa/fde/fde_gedevice.h"
#include "xfa/fde/fde_object.h"
#include "xfa/fgas/crt/fgas_memory.h"
#include "xfa/fgas/crt/fgas_utils.h"
#include "xfa/fgas/layout/fgas_textbreak.h"

CFDE_TextOut::CFDE_TextOut()
    : m_pFont(nullptr),
      m_fFontSize(12.0f),
      m_fLineSpace(m_fFontSize),
      m_fLinePos(0.0f),
      m_fTolerance(0.0f),
      m_iAlignment(0),
      m_iTxtBkAlignment(0),
      m_pCharWidths(nullptr),
      m_iChars(0),
      m_pEllCharWidths(nullptr),
      m_iEllChars(0),
      m_wParagraphBkChar(L'\n'),
      m_TxtColor(0xFF000000),
      m_dwStyles(0),
      m_dwTxtBkStyles(0),
      m_bElliChanged(FALSE),
      m_iEllipsisWidth(0),
      m_ttoLines(5),
      m_iCurLine(0),
      m_iCurPiece(0),
      m_iTotalLines(0),
      m_pCharPos(nullptr),
      m_iCharPosSize(0) {
  m_pTxtBreak = new CFX_TxtBreak(FX_TXTBREAKPOLICY_None);
  m_Matrix.SetIdentity();
  m_rtClip.Reset();
  m_rtLogicClip.Reset();
}
CFDE_TextOut::~CFDE_TextOut() {
  delete m_pTxtBreak;
  FX_Free(m_pCharWidths);
  FX_Free(m_pEllCharWidths);
  FX_Free(m_pCharPos);
  m_ttoLines.RemoveAll();
}
void CFDE_TextOut::SetFont(CFGAS_GEFont* pFont) {
  ASSERT(pFont);
  m_pFont = pFont;
  m_pTxtBreak->SetFont(pFont);
}
void CFDE_TextOut::SetFontSize(FX_FLOAT fFontSize) {
  ASSERT(fFontSize > 0);
  m_fFontSize = fFontSize;
  m_pTxtBreak->SetFontSize(fFontSize);
}
void CFDE_TextOut::SetTextColor(FX_ARGB color) {
  m_TxtColor = color;
}
void CFDE_TextOut::SetStyles(uint32_t dwStyles) {
  m_dwStyles = dwStyles;
  m_dwTxtBkStyles = 0;
  if (dwStyles & FDE_TTOSTYLE_SingleLine) {
    m_dwTxtBkStyles |= FX_TXTLAYOUTSTYLE_SingleLine;
  }
  if (dwStyles & FDE_TTOSTYLE_ExpandTab) {
    m_dwTxtBkStyles |= FX_TXTLAYOUTSTYLE_ExpandTab;
  }
  if (dwStyles & FDE_TTOSTYLE_ArabicShapes) {
    m_dwTxtBkStyles |= FX_TXTLAYOUTSTYLE_ArabicShapes;
  }
  if (dwStyles & FDE_TTOSTYLE_RTL) {
    m_dwTxtBkStyles |= FX_TXTLAYOUTSTYLE_RTLReadingOrder;
  }
  if (dwStyles & FDE_TTOSTYLE_ArabicContext) {
    m_dwTxtBkStyles |= FX_TXTLAYOUTSTYLE_ArabicContext;
  }
  if (dwStyles & FDE_TTOSTYLE_VerticalLayout) {
    m_dwTxtBkStyles |=
        (FX_TXTLAYOUTSTYLE_VerticalChars | FX_TXTLAYOUTSTYLE_VerticalLayout);
  }
  m_pTxtBreak->SetLayoutStyles(m_dwTxtBkStyles);
}
void CFDE_TextOut::SetTabWidth(FX_FLOAT fTabWidth) {
  ASSERT(fTabWidth > 1.0f);
  m_pTxtBreak->SetTabWidth(fTabWidth, FALSE);
}
void CFDE_TextOut::SetEllipsisString(const CFX_WideString& wsEllipsis) {
  m_bElliChanged = TRUE;
  m_wsEllipsis = wsEllipsis;
}
void CFDE_TextOut::SetParagraphBreakChar(FX_WCHAR wch) {
  m_wParagraphBkChar = wch;
  m_pTxtBreak->SetParagraphBreakChar(wch);
}
void CFDE_TextOut::SetAlignment(int32_t iAlignment) {
  m_iAlignment = iAlignment;
  switch (m_iAlignment) {
    case FDE_TTOALIGNMENT_TopCenter:
    case FDE_TTOALIGNMENT_Center:
    case FDE_TTOALIGNMENT_BottomCenter:
      m_iTxtBkAlignment = FX_TXTLINEALIGNMENT_Center;
      break;
    case FDE_TTOALIGNMENT_TopRight:
    case FDE_TTOALIGNMENT_CenterRight:
    case FDE_TTOALIGNMENT_BottomRight:
      m_iTxtBkAlignment = FX_TXTLINEALIGNMENT_Right;
      break;
    default:
      m_iTxtBkAlignment = FX_TXTLINEALIGNMENT_Left;
      break;
  }
  m_pTxtBreak->SetAlignment(m_iTxtBkAlignment);
}
void CFDE_TextOut::SetLineSpace(FX_FLOAT fLineSpace) {
  ASSERT(fLineSpace > 1.0f);
  m_fLineSpace = fLineSpace;
}

void CFDE_TextOut::SetDIBitmap(CFX_DIBitmap* pDIB) {
  ASSERT(pDIB);

  m_pRenderDevice.reset();
  CFX_FxgeDevice* device = new CFX_FxgeDevice;
  device->Attach(pDIB, false, nullptr, false);
  m_pRenderDevice.reset(new CFDE_RenderDevice(device, FALSE));
}

void CFDE_TextOut::SetRenderDevice(CFX_RenderDevice* pDevice) {
  ASSERT(pDevice);
  m_pRenderDevice.reset(new CFDE_RenderDevice(pDevice, FALSE));
}

void CFDE_TextOut::SetClipRect(const CFX_Rect& rtClip) {
  m_rtClip.Set((FX_FLOAT)rtClip.left, (FX_FLOAT)rtClip.top,
               (FX_FLOAT)rtClip.Width(), (FX_FLOAT)rtClip.Height());
}
void CFDE_TextOut::SetClipRect(const CFX_RectF& rtClip) {
  m_rtClip = rtClip;
}
void CFDE_TextOut::SetLogicClipRect(const CFX_RectF& rtClip) {
  m_rtLogicClip = rtClip;
}
void CFDE_TextOut::SetMatrix(const CFX_Matrix& matrix) {
  m_Matrix = matrix;
}
void CFDE_TextOut::SetLineBreakTolerance(FX_FLOAT fTolerance) {
  m_fTolerance = fTolerance;
  m_pTxtBreak->SetLineBreakTolerance(m_fTolerance);
}
int32_t CFDE_TextOut::GetTotalLines() {
  return m_iTotalLines;
}
void CFDE_TextOut::CalcSize(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            CFX_Size& size) {
  CFX_RectF rtText;
  rtText.Set(0.0f, 0.0f, (FX_FLOAT)size.x, (FX_FLOAT)size.y);
  CalcSize(pwsStr, iLength, rtText);
  size.x = (int32_t)rtText.Width();
  size.y = (int32_t)rtText.Height();
}
void CFDE_TextOut::CalcSize(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            CFX_SizeF& size) {
  CFX_RectF rtText;
  rtText.Set(0.0f, 0.0f, size.x, size.y);
  CalcSize(pwsStr, iLength, rtText);
  size.x = rtText.Width();
  size.y = rtText.Height();
}
void CFDE_TextOut::CalcSize(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            CFX_Rect& rect) {
  CFX_RectF rtText;
  rtText.Set((FX_FLOAT)rect.left, (FX_FLOAT)rect.top, (FX_FLOAT)rect.Width(),
             (FX_FLOAT)rect.Height());
  CalcSize(pwsStr, iLength, rtText);
  rect.Set((int32_t)rtText.left, (int32_t)rtText.top, (int32_t)rtText.Width(),
           (int32_t)rtText.Height());
}
void CFDE_TextOut::CalcSize(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            CFX_RectF& rect) {
  if (!pwsStr || iLength < 1) {
    rect.width = 0.0f;
    rect.height = 0.0f;
  } else {
    CFX_Matrix rm;
    rm.SetReverse(m_Matrix);
    rm.TransformRect(rect);
    CalcTextSize(pwsStr, iLength, rect);
    m_Matrix.TransformRect(rect);
  }
}
void CFDE_TextOut::CalcLogicSize(const FX_WCHAR* pwsStr,
                                 int32_t iLength,
                                 CFX_SizeF& size) {
  CFX_RectF rtText;
  rtText.Set(0.0f, 0.0f, size.x, size.y);
  CalcLogicSize(pwsStr, iLength, rtText);
  size.x = rtText.Width();
  size.y = rtText.Height();
}
void CFDE_TextOut::CalcLogicSize(const FX_WCHAR* pwsStr,
                                 int32_t iLength,
                                 CFX_RectF& rect) {
  if (!pwsStr || iLength < 1) {
    rect.width = 0.0f;
    rect.height = 0.0f;
  } else {
    CalcTextSize(pwsStr, iLength, rect);
  }
}
void CFDE_TextOut::CalcTextSize(const FX_WCHAR* pwsStr,
                                int32_t iLength,
                                CFX_RectF& rect) {
  ASSERT(m_pFont && m_fFontSize >= 1.0f);
  SetLineWidth(rect);
  m_iTotalLines = 0;
  const FX_WCHAR* pStr = pwsStr;
  FX_BOOL bHotKey = !!(m_dwStyles & FDE_TTOSTYLE_HotKey);
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  FX_FLOAT fWidth = 0.0f;
  FX_FLOAT fHeight = 0.0f;
  FX_FLOAT fStartPos = bVertical ? rect.bottom() : rect.right();
  uint32_t dwBreakStatus = 0;
  FX_WCHAR wPreChar = 0;
  FX_WCHAR wch;
  FX_WCHAR wBreak = 0;
  while (iLength-- > 0) {
    wch = *pStr++;
    if (wBreak == 0 && (wch == L'\n' || wch == L'\r')) {
      wBreak = wch;
      m_pTxtBreak->SetParagraphBreakChar(wch);
    }
    if (bHotKey && wch == L'&' && wPreChar != L'&') {
      wPreChar = wch;
      continue;
    }
    dwBreakStatus = m_pTxtBreak->AppendChar(wch);
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      RetrieveLineWidth(dwBreakStatus, fStartPos, fWidth, fHeight);
    }
    wPreChar = 0;
  }
  dwBreakStatus = m_pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
    RetrieveLineWidth(dwBreakStatus, fStartPos, fWidth, fHeight);
  }
  m_pTxtBreak->Reset();
  FX_FLOAT fInc = rect.Height() - fHeight;
  if (bVertical) {
    fInc = rect.Width() - fHeight;
  }
  if (m_iAlignment >= FDE_TTOALIGNMENT_CenterLeft &&
      m_iAlignment < FDE_TTOALIGNMENT_BottomLeft) {
    fInc /= 2.0f;
  } else if (m_iAlignment < FDE_TTOALIGNMENT_CenterLeft) {
    fInc = 0.0f;
  }
  if (bVertical) {
    rect.top += fStartPos;
    rect.left += fInc;
    rect.width = fHeight;
    rect.height = std::min(fWidth, rect.Height());
  } else {
    rect.left += fStartPos;
    rect.top += fInc;
    rect.width = std::min(fWidth, rect.Width());
    rect.height = fHeight;
    if (m_dwStyles & FDE_TTOSTYLE_LastLineHeight) {
      rect.height -= m_fLineSpace - m_fFontSize;
    }
  }
}
void CFDE_TextOut::SetLineWidth(CFX_RectF& rect) {
  if ((m_dwStyles & FDE_TTOSTYLE_SingleLine) == 0) {
    FX_FLOAT fLineWidth = 0.0f;
    if (m_dwStyles & FDE_TTOSTYLE_VerticalLayout) {
      if (rect.Height() < 1.0f) {
        rect.height = m_fFontSize * 1000.0f;
      }
      fLineWidth = rect.Height();
    } else {
      if (rect.Width() < 1.0f) {
        rect.width = m_fFontSize * 1000.0f;
      }
      fLineWidth = rect.Width();
    }
    m_pTxtBreak->SetLineWidth(fLineWidth);
  }
}
FX_BOOL CFDE_TextOut::RetrieveLineWidth(uint32_t dwBreakStatus,
                                        FX_FLOAT& fStartPos,
                                        FX_FLOAT& fWidth,
                                        FX_FLOAT& fHeight) {
  if (dwBreakStatus <= FX_TXTBREAK_PieceBreak) {
    return FALSE;
  }
  FX_FLOAT fLineStep =
      (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  FX_BOOL bLineWrap = !!(m_dwStyles & FDE_TTOSTYLE_LineWrap);
  FX_FLOAT fLineWidth = 0.0f;
  int32_t iCount = m_pTxtBreak->CountBreakPieces();
  for (int32_t i = 0; i < iCount; i++) {
    const CFX_TxtPiece* pPiece = m_pTxtBreak->GetBreakPiece(i);
    fLineWidth += (FX_FLOAT)pPiece->m_iWidth / 20000.0f;
    fStartPos = std::min(fStartPos, (FX_FLOAT)pPiece->m_iStartPos / 20000.0f);
  }
  m_pTxtBreak->ClearBreakPieces();
  if (dwBreakStatus == FX_TXTBREAK_ParagraphBreak) {
    m_pTxtBreak->Reset();
  }
  if (!bLineWrap && dwBreakStatus == FX_TXTBREAK_LineBreak) {
    fWidth += fLineWidth;
  } else {
    fWidth = std::max(fWidth, fLineWidth);
    fHeight += fLineStep;
  }
  m_iTotalLines++;
  return TRUE;
}
void CFDE_TextOut::DrawText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            int32_t x,
                            int32_t y) {
  CFX_RectF rtText;
  rtText.Set((FX_FLOAT)x, (FX_FLOAT)y, m_fFontSize * 1000.0f,
             m_fFontSize * 1000.0f);
  DrawText(pwsStr, iLength, rtText);
}
void CFDE_TextOut::DrawText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            FX_FLOAT x,
                            FX_FLOAT y) {
  CFX_RectF rtText;
  rtText.Set(x, y, m_fFontSize * 1000.0f, m_fFontSize * 1000.0f);
  DrawText(pwsStr, iLength, rtText);
}
void CFDE_TextOut::DrawText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            const CFX_Rect& rect) {
  CFX_RectF rtText;
  rtText.Set((FX_FLOAT)rect.left, (FX_FLOAT)rect.top, (FX_FLOAT)rect.width,
             (FX_FLOAT)rect.height);
  DrawText(pwsStr, iLength, rtText);
}
void CFDE_TextOut::DrawText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            const CFX_RectF& rect) {
  CFX_RectF rtText;
  rtText.Set(rect.left, rect.top, rect.width, rect.height);
  CFX_Matrix rm;
  rm.SetReverse(m_Matrix);
  rm.TransformRect(rtText);
  DrawText(pwsStr, iLength, rtText, m_rtClip);
}
void CFDE_TextOut::DrawLogicText(const FX_WCHAR* pwsStr,
                                 int32_t iLength,
                                 FX_FLOAT x,
                                 FX_FLOAT y) {
  CFX_RectF rtText;
  rtText.Set(x, y, m_fFontSize * 1000.0f, m_fFontSize * 1000.0f);
  DrawLogicText(pwsStr, iLength, rtText);
}
void CFDE_TextOut::DrawLogicText(const FX_WCHAR* pwsStr,
                                 int32_t iLength,
                                 const CFX_RectF& rect) {
  CFX_RectF rtClip;
  rtClip.Set(m_rtLogicClip.left, m_rtLogicClip.top, m_rtLogicClip.width,
             m_rtLogicClip.height);
  m_Matrix.TransformRect(rtClip);
  DrawText(pwsStr, iLength, rect, rtClip);
}
void CFDE_TextOut::DrawText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            const CFX_RectF& rect,
                            const CFX_RectF& rtClip) {
  ASSERT(m_pFont && m_fFontSize >= 1.0f);
  if (!pwsStr || iLength < 1)
    return;

  if (rect.width < m_fFontSize || rect.height < m_fFontSize) {
    return;
  }
  FX_FLOAT fLineWidth = rect.width;
  if (m_dwStyles & FDE_TTOSTYLE_VerticalLayout) {
    fLineWidth = rect.height;
  }
  m_pTxtBreak->SetLineWidth(fLineWidth);
  m_ttoLines.RemoveAll(TRUE);
  m_wsText.clear();
  LoadText(pwsStr, iLength, rect);
  if (m_dwStyles & FDE_TTOSTYLE_Ellipsis) {
    ReplaceWidthEllipsis();
  }
  Reload(rect);
  DoAlignment(rect);
  OnDraw(rtClip);
}
void CFDE_TextOut::ExpandBuffer(int32_t iSize, int32_t iType) {
  switch (iType) {
    case 0:
      if (!m_pCharWidths) {
        m_pCharWidths = FX_Alloc(int32_t, iSize);
        m_iChars = iSize;
      } else if (m_iChars < iSize) {
        m_pCharWidths = FX_Realloc(int32_t, m_pCharWidths, iSize);
        m_iChars = iSize;
      }
      FXSYS_memset(m_pCharWidths, 0, iSize * sizeof(int32_t));
      break;
    case 1:
      if (!m_pEllCharWidths) {
        m_pEllCharWidths = FX_Alloc(int32_t, iSize);
        m_iEllChars = iSize;
      } else if (m_iEllChars < iSize) {
        m_pEllCharWidths = FX_Realloc(int32_t, m_pEllCharWidths, iSize);
        m_iEllChars = iSize;
      }
      FXSYS_memset(m_pEllCharWidths, 0, iSize * sizeof(int32_t));
      break;
    case 2:
      if (!m_pCharPos) {
        m_pCharPos = FX_Alloc(FXTEXT_CHARPOS, iSize);
        m_iCharPosSize = iSize;
      } else if (m_iCharPosSize < iSize) {
        m_pCharPos = FX_Realloc(FXTEXT_CHARPOS, m_pCharPos, iSize);
        m_iCharPosSize = iSize;
      }
      break;
  }
}
void CFDE_TextOut::LoadEllipsis() {
  if (!m_bElliChanged) {
    return;
  }
  m_bElliChanged = FALSE;
  m_iEllipsisWidth = 0;
  int32_t iLength = m_wsEllipsis.GetLength();
  if (iLength < 1) {
    return;
  }
  ExpandBuffer(iLength, 1);
  const FX_WCHAR* pStr = m_wsEllipsis.c_str();
  int32_t* pCharWidths = m_pEllCharWidths;
  uint32_t dwBreakStatus;
  FX_WCHAR wch;
  while (iLength-- > 0) {
    wch = *pStr++;
    dwBreakStatus = m_pTxtBreak->AppendChar(wch);
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      RetrieveEllPieces(pCharWidths);
    }
  }
  dwBreakStatus = m_pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
    RetrieveEllPieces(pCharWidths);
  }
  m_pTxtBreak->Reset();
}
void CFDE_TextOut::RetrieveEllPieces(int32_t*& pCharWidths) {
  int32_t iCount = m_pTxtBreak->CountBreakPieces();
  CFX_Char* pTC;
  for (int32_t i = 0; i < iCount; i++) {
    const CFX_TxtPiece* pPiece = m_pTxtBreak->GetBreakPiece(i);
    int32_t iPieceChars = pPiece->GetLength();
    for (int32_t j = 0; j < iPieceChars; j++) {
      pTC = pPiece->GetCharPtr(j);
      if (pTC->m_iCharWidth <= 0) {
        *pCharWidths = 0;
      } else {
        *pCharWidths = pTC->m_iCharWidth;
      }
      m_iEllipsisWidth += *pCharWidths;
      pCharWidths++;
    }
  }
  m_pTxtBreak->ClearBreakPieces();
}
void CFDE_TextOut::LoadText(const FX_WCHAR* pwsStr,
                            int32_t iLength,
                            const CFX_RectF& rect) {
  FX_WCHAR* pStr = m_wsText.GetBuffer(iLength);
  int32_t iTxtLength = iLength;
  ExpandBuffer(iTxtLength, 0);
  FX_BOOL bHotKey = !!(m_dwStyles & FDE_TTOSTYLE_HotKey);
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  FX_BOOL bLineWrap = !!(m_dwStyles & FDE_TTOSTYLE_LineWrap);
  FX_FLOAT fLineStep =
      (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  FX_FLOAT fLineStop = bVertical ? rect.left : rect.bottom();
  m_fLinePos = bVertical ? rect.right() : rect.top;
  if (bVertical) {
    fLineStep = -fLineStep;
  }
  m_hotKeys.RemoveAll();
  int32_t iStartChar = 0;
  int32_t iChars = 0;
  int32_t iPieceWidths = 0;
  uint32_t dwBreakStatus;
  FX_WCHAR wch;
  FX_BOOL bRet = FALSE;
  while (iTxtLength-- > 0) {
    wch = *pwsStr++;
    if (bHotKey && wch == L'&' && *(pStr - 1) != L'&') {
      if (iTxtLength > 0)
        m_hotKeys.Add(iChars);
      continue;
    }
    *pStr++ = wch;
    iChars++;
    dwBreakStatus = m_pTxtBreak->AppendChar(wch);
    if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
      FX_BOOL bEndofLine =
          RetriecePieces(dwBreakStatus, iStartChar, iPieceWidths, FALSE, rect);
      if (bEndofLine && (bLineWrap || (dwBreakStatus > FX_TXTBREAK_LineBreak &&
                                       !bLineWrap))) {
        iPieceWidths = 0;
        m_iCurLine++;
        m_fLinePos += fLineStep;
      }
      if ((bVertical && m_fLinePos + fLineStep < fLineStop) ||
          (!bVertical && m_fLinePos + fLineStep > fLineStop)) {
        int32_t iCurLine = m_iCurLine;
        if (bEndofLine) {
          iCurLine--;
        }
        CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(iCurLine);
        pLine->m_bNewReload = TRUE;
        bRet = TRUE;
        break;
      }
    }
  }
  dwBreakStatus = m_pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  if (dwBreakStatus > FX_TXTBREAK_PieceBreak && !bRet) {
    RetriecePieces(dwBreakStatus, iStartChar, iPieceWidths, FALSE, rect);
  }
  m_pTxtBreak->ClearBreakPieces();
  m_pTxtBreak->Reset();
  m_wsText.ReleaseBuffer(iLength);
}
FX_BOOL CFDE_TextOut::RetriecePieces(uint32_t dwBreakStatus,
                                     int32_t& iStartChar,
                                     int32_t& iPieceWidths,
                                     FX_BOOL bReload,
                                     const CFX_RectF& rect) {
  FX_BOOL bSingleLine = !!(m_dwStyles & FDE_TTOSTYLE_SingleLine);
  FX_BOOL bLineWrap = !!(m_dwStyles & FDE_TTOSTYLE_LineWrap);
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  FX_FLOAT fLineStep =
      (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  if (bVertical) {
    fLineStep = -fLineStep;
  }
  CFX_Char* pTC = nullptr;
  FX_BOOL bNeedReload = FALSE;
  FX_FLOAT fLineWidth = bVertical ? rect.Height() : rect.Width();
  int32_t iLineWidth = FXSYS_round(fLineWidth * 20000.0f);
  int32_t iCount = m_pTxtBreak->CountBreakPieces();
  for (int32_t i = 0; i < iCount; i++) {
    const CFX_TxtPiece* pPiece = m_pTxtBreak->GetBreakPiece(i);
    int32_t iPieceChars = pPiece->GetLength();
    int32_t iChar = iStartChar;
    int32_t iWidth = 0;
    int32_t j = 0;
    for (; j < iPieceChars; j++) {
      pTC = pPiece->GetCharPtr(j);
      int32_t iCurCharWidth = pTC->m_iCharWidth > 0 ? pTC->m_iCharWidth : 0;
      if (bSingleLine || !bLineWrap) {
        if (iLineWidth - iPieceWidths - iWidth < iCurCharWidth) {
          bNeedReload = TRUE;
          break;
        }
      }
      iWidth += iCurCharWidth;
      m_pCharWidths[iChar++] = iCurCharWidth;
    }
    if (j == 0 && !bReload) {
      CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(m_iCurLine);
      pLine->m_bNewReload = TRUE;
    } else if (j > 0) {
      CFX_RectF rtPiece;
      if (bVertical) {
        rtPiece.left = m_fLinePos;
        rtPiece.top = rect.top + (FX_FLOAT)pPiece->m_iStartPos / 20000.0f;
        rtPiece.width = fLineStep;
        rtPiece.height = iWidth / 20000.0f;
      } else {
        rtPiece.left = rect.left + (FX_FLOAT)pPiece->m_iStartPos / 20000.0f;
        rtPiece.top = m_fLinePos;
        rtPiece.width = iWidth / 20000.0f;
        rtPiece.height = fLineStep;
      }
      FDE_TTOPIECE ttoPiece;
      ttoPiece.iStartChar = iStartChar;
      ttoPiece.iChars = j;
      ttoPiece.rtPiece = rtPiece;
      ttoPiece.dwCharStyles = pPiece->m_dwCharStyles;
      if (FX_IsOdd(pPiece->m_iBidiLevel)) {
        ttoPiece.dwCharStyles |= FX_TXTCHARSTYLE_OddBidiLevel;
      }
      AppendPiece(ttoPiece, bNeedReload, (bReload && i == iCount - 1));
    }
    iStartChar += iPieceChars;
    iPieceWidths += iWidth;
  }
  m_pTxtBreak->ClearBreakPieces();
  FX_BOOL bRet = bSingleLine || bLineWrap || (!bLineWrap && bNeedReload) ||
                 dwBreakStatus == FX_TXTBREAK_ParagraphBreak;
  return bRet;
}
void CFDE_TextOut::AppendPiece(const FDE_TTOPIECE& ttoPiece,
                               FX_BOOL bNeedReload,
                               FX_BOOL bEnd) {
  if (m_iCurLine >= m_ttoLines.GetSize()) {
    CFDE_TTOLine ttoLine;
    ttoLine.m_bNewReload = bNeedReload;
    m_iCurPiece = ttoLine.AddPiece(m_iCurPiece, ttoPiece);
    m_iCurLine = m_ttoLines.Add(ttoLine);
  } else {
    CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(m_iCurLine);
    pLine->m_bNewReload = bNeedReload;
    m_iCurPiece = pLine->AddPiece(m_iCurPiece, ttoPiece);
    if (bEnd) {
      int32_t iPieces = pLine->GetSize();
      if (m_iCurPiece < iPieces) {
        pLine->RemoveLast(iPieces - m_iCurPiece - 1);
      }
    }
  }
  if (!bEnd && bNeedReload) {
    m_iCurPiece = 0;
  }
}
void CFDE_TextOut::ReplaceWidthEllipsis() {
  LoadEllipsis();
  int32_t iLength = m_wsEllipsis.GetLength();
  if (iLength < 1) {
    return;
  }
  int32_t iLines = m_ttoLines.GetSize();
  for (int32_t i = 0; i < iLines; i++) {
    CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(i);
    if (!pLine->m_bNewReload) {
      continue;
    }
    int32_t iEllipsisCharIndex = iLength - 1;
    int32_t iCharWidth = 0;
    int32_t iCharCount = 0;
    int32_t iPiece = pLine->GetSize();
    while (iPiece-- > 0) {
      FDE_TTOPIECE* pPiece = pLine->GetPtrAt(iPiece);
      if (!pPiece)
        break;

      for (int32_t j = pPiece->iChars - 1; j >= 0; j--) {
        if (iEllipsisCharIndex < 0) {
          break;
        }
        int32_t index = pPiece->iStartChar + j;
        iCharWidth += m_pCharWidths[index];
        iCharCount++;
        if (iCharCount <= iLength) {
          m_wsText.SetAt(index, m_wsEllipsis.GetAt(iEllipsisCharIndex));
          m_pCharWidths[index] = m_pEllCharWidths[iEllipsisCharIndex];
        } else if (iCharWidth <= m_iEllipsisWidth) {
          m_wsText.SetAt(index, 0);
          m_pCharWidths[index] = 0;
        }
        iEllipsisCharIndex--;
      }
      if (iEllipsisCharIndex < 0) {
        break;
      }
    }
  }
}
void CFDE_TextOut::Reload(const CFX_RectF& rect) {
  int32_t iCount = m_ttoLines.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(i);
    if (!pLine || !pLine->m_bNewReload)
      continue;

    m_iCurLine = i;
    m_iCurPiece = 0;
    ReloadLinePiece(pLine, rect);
  }
}
void CFDE_TextOut::ReloadLinePiece(CFDE_TTOLine* pLine, const CFX_RectF& rect) {
  const FX_WCHAR* pwsStr = m_wsText.c_str();
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  int32_t iPieceWidths = 0;
  FDE_TTOPIECE* pPiece = pLine->GetPtrAt(0);
  int32_t iStartChar = pPiece->iStartChar;
  m_fLinePos = bVertical ? pPiece->rtPiece.left : pPiece->rtPiece.top;
  int32_t iPieceCount = pLine->GetSize();
  int32_t iPieceIndex = 0;
  uint32_t dwBreakStatus = 0;
  FX_WCHAR wch;
  while (iPieceIndex < iPieceCount) {
    int32_t iStar = iStartChar;
    int32_t iEnd = pPiece->iChars + iStar;
    while (iStar < iEnd) {
      wch = *(pwsStr + iStar);
      dwBreakStatus = m_pTxtBreak->AppendChar(wch);
      if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
        RetriecePieces(dwBreakStatus, iStartChar, iPieceWidths, TRUE, rect);
      }
      iStar++;
    }
    iPieceIndex++;
    pPiece = pLine->GetPtrAt(iPieceIndex);
  }
  dwBreakStatus = m_pTxtBreak->EndBreak(FX_TXTBREAK_ParagraphBreak);
  if (dwBreakStatus > FX_TXTBREAK_PieceBreak) {
    RetriecePieces(dwBreakStatus, iStartChar, iPieceWidths, TRUE, rect);
  }
  m_pTxtBreak->Reset();
}
void CFDE_TextOut::DoAlignment(const CFX_RectF& rect) {
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  FX_FLOAT fLineStopS = bVertical ? rect.right() : rect.bottom();
  int32_t iLines = m_ttoLines.GetSize();
  if (iLines < 1)
    return;
  FDE_TTOPIECE* pFirstPiece = m_ttoLines.GetPtrAt(iLines - 1)->GetPtrAt(0);
  if (!pFirstPiece)
    return;

  FX_FLOAT fLineStopD =
      bVertical ? pFirstPiece->rtPiece.right() : pFirstPiece->rtPiece.bottom();
  FX_FLOAT fInc = fLineStopS - fLineStopD;
  if (m_iAlignment >= FDE_TTOALIGNMENT_CenterLeft &&
      m_iAlignment < FDE_TTOALIGNMENT_BottomLeft) {
    fInc /= 2.0f;
  } else if (m_iAlignment < FDE_TTOALIGNMENT_CenterLeft) {
    fInc = 0.0f;
  }
  if (fInc < 1.0f)
    return;
  for (int32_t i = 0; i < iLines; i++) {
    CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(i);
    int32_t iPieces = pLine->GetSize();
    for (int32_t j = 0; j < iPieces; j++) {
      FDE_TTOPIECE* pPiece = pLine->GetPtrAt(j);
      if (bVertical)
        pPiece->rtPiece.left += fInc;
      else
        pPiece->rtPiece.top += fInc;
    }
  }
}
void CFDE_TextOut::OnDraw(const CFX_RectF& rtClip) {
  if (!m_pRenderDevice)
    return;

  int32_t iLines = m_ttoLines.GetSize();
  if (iLines < 1)
    return;

  CFDE_Brush* pBrush = new CFDE_Brush;
  pBrush->SetColor(m_TxtColor);
  CFDE_Pen* pPen = nullptr;
  m_pRenderDevice->SaveState();
  if (rtClip.Width() > 0.0f && rtClip.Height() > 0.0f) {
    m_pRenderDevice->SetClipRect(rtClip);
  }
  for (int32_t i = 0; i < iLines; i++) {
    CFDE_TTOLine* pLine = m_ttoLines.GetPtrAt(i);
    int32_t iPieces = pLine->GetSize();
    for (int32_t j = 0; j < iPieces; j++) {
      FDE_TTOPIECE* pPiece = pLine->GetPtrAt(j);
      if (!pPiece)
        continue;

      int32_t iCount = GetDisplayPos(pPiece);
      if (iCount > 0) {
        m_pRenderDevice->DrawString(pBrush, m_pFont, m_pCharPos, iCount,
                                    m_fFontSize, &m_Matrix);
      }
      DrawLine(pPiece, pPen);
    }
  }
  m_pRenderDevice->RestoreState();
  delete pBrush;
  delete pPen;
}

int32_t CFDE_TextOut::GetDisplayPos(FDE_TTOPIECE* pPiece) {
  FX_TXTRUN tr = ToTextRun(pPiece);
  ExpandBuffer(tr.iLength, 2);
  return m_pTxtBreak->GetDisplayPos(&tr, m_pCharPos);
}

int32_t CFDE_TextOut::GetCharRects(const FDE_TTOPIECE* pPiece) {
  FX_TXTRUN tr = ToTextRun(pPiece);
  m_rectArray.RemoveAll();
  return m_pTxtBreak->GetCharRects(&tr, m_rectArray);
}

FX_TXTRUN CFDE_TextOut::ToTextRun(const FDE_TTOPIECE* pPiece) {
  FX_TXTRUN tr;
  tr.wsStr = m_wsText + pPiece->iStartChar;
  tr.pWidths = m_pCharWidths + pPiece->iStartChar;
  tr.iLength = pPiece->iChars;
  tr.pFont = m_pFont;
  tr.fFontSize = m_fFontSize;
  tr.dwStyles = m_dwTxtBkStyles;
  tr.dwCharStyles = pPiece->dwCharStyles;
  tr.wLineBreakChar = m_wParagraphBkChar;
  tr.pRect = &pPiece->rtPiece;
  return tr;
}

void CFDE_TextOut::DrawLine(const FDE_TTOPIECE* pPiece, CFDE_Pen*& pPen) {
  FX_BOOL bUnderLine = !!(m_dwStyles & FDE_TTOSTYLE_Underline);
  FX_BOOL bStrikeOut = !!(m_dwStyles & FDE_TTOSTYLE_Strikeout);
  FX_BOOL bHotKey = !!(m_dwStyles & FDE_TTOSTYLE_HotKey);
  FX_BOOL bVertical = !!(m_dwStyles & FDE_TTOSTYLE_VerticalLayout);
  if (!bUnderLine && !bStrikeOut && !bHotKey)
    return;

  if (!pPen) {
    pPen = new CFDE_Pen;
    pPen->SetColor(m_TxtColor);
  }
  std::unique_ptr<CFDE_Path> pPath(new CFDE_Path);
  int32_t iLineCount = 0;
  CFX_RectF rtText = pPiece->rtPiece;
  CFX_PointF pt1, pt2;
  if (bUnderLine) {
    if (bVertical) {
      pt1.x = rtText.left;
      pt1.y = rtText.top;
      pt2.x = rtText.left;
      pt2.y = rtText.bottom();
    } else {
      pt1.x = rtText.left;
      pt1.y = rtText.bottom();
      pt2.x = rtText.right();
      pt2.y = rtText.bottom();
    }
    pPath->AddLine(pt1, pt2);
    iLineCount++;
  }
  if (bStrikeOut) {
    if (bVertical) {
      pt1.x = rtText.left + rtText.width * 2.0f / 5.0f;
      pt1.y = rtText.top;
      pt2.x = pt1.x;
      pt2.y = rtText.bottom();
    } else {
      pt1.x = rtText.left;
      pt1.y = rtText.bottom() - rtText.height * 2.0f / 5.0f;
      pt2.x = rtText.right();
      pt2.y = pt1.y;
    }
    pPath->AddLine(pt1, pt2);
    iLineCount++;
  }
  if (bHotKey) {
    int32_t iHotKeys = m_hotKeys.GetSize();
    int32_t iCount = GetCharRects(pPiece);
    if (iCount > 0) {
      for (int32_t i = 0; i < iHotKeys; i++) {
        int32_t iCharIndex = m_hotKeys.GetAt(i);
        if (iCharIndex >= pPiece->iStartChar &&
            iCharIndex < pPiece->iStartChar + pPiece->iChars) {
          CFX_RectF rect = m_rectArray.GetAt(iCharIndex - pPiece->iStartChar);
          if (bVertical) {
            pt1.x = rect.left;
            pt1.y = rect.top;
            pt2.x = rect.left;
            pt2.y = rect.bottom();
          } else {
            pt1.x = rect.left;
            pt1.y = rect.bottom();
            pt2.x = rect.right();
            pt2.y = rect.bottom();
          }
          pPath->AddLine(pt1, pt2);
          iLineCount++;
        }
      }
    }
  }
  if (iLineCount > 0)
    m_pRenderDevice->DrawPath(pPen, 1, pPath.get(), &m_Matrix);
}

CFDE_TTOLine::CFDE_TTOLine()
    : m_bNewReload(FALSE), m_pieces(5), m_iPieceCount(0) {}

CFDE_TTOLine::CFDE_TTOLine(const CFDE_TTOLine& ttoLine) : m_pieces(5) {
  m_bNewReload = ttoLine.m_bNewReload;
  m_iPieceCount = ttoLine.m_iPieceCount;
  m_pieces.Copy(ttoLine.m_pieces);
}

CFDE_TTOLine::~CFDE_TTOLine() {}

int32_t CFDE_TTOLine::AddPiece(int32_t index, const FDE_TTOPIECE& ttoPiece) {
  if (index >= m_iPieceCount) {
    index = m_pieces.Add(ttoPiece) + 1;
    m_iPieceCount++;
  } else {
    FDE_TTOPIECE& piece = m_pieces.GetAt(index);
    piece = ttoPiece;
  }
  return index;
}
int32_t CFDE_TTOLine::GetSize() const {
  return m_iPieceCount;
}
FDE_TTOPIECE* CFDE_TTOLine::GetPtrAt(int32_t index) {
  if (index >= m_iPieceCount) {
    return nullptr;
  }
  return m_pieces.GetPtrAt(index);
}
void CFDE_TTOLine::RemoveLast(int32_t iCount) {
  m_pieces.RemoveLast(iCount);
}
void CFDE_TTOLine::RemoveAll(FX_BOOL bLeaveMemory) {
  m_pieces.RemoveAll(bLeaveMemory);
}
