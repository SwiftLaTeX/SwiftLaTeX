// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/fgas_rtfbreak.h"

#include <algorithm>

#include "core/fxcrt/include/fx_arabic.h"
#include "core/fxcrt/include/fx_arb.h"
#include "xfa/fgas/font/fgas_gefont.h"
#include "xfa/fgas/layout/fgas_linebreak.h"
#include "xfa/fgas/layout/fgas_unicode.h"

CFX_RTFBreak::CFX_RTFBreak(uint32_t dwPolicies)
    : m_dwPolicies(dwPolicies),
      m_iBoundaryStart(0),
      m_iBoundaryEnd(2000000),
      m_dwLayoutStyles(0),
      m_bPagination(FALSE),
      m_bVertical(FALSE),
      m_bSingleLine(FALSE),
      m_bCharCode(FALSE),
      m_pFont(nullptr),
      m_iFontHeight(240),
      m_iFontSize(240),
      m_iTabWidth(720000),
      m_PositionedTabs(),
      m_bOrphanLine(FALSE),
      m_wDefChar(0xFEFF),
      m_iDefChar(0),
      m_wLineBreakChar(L'\n'),
      m_iHorizontalScale(100),
      m_iVerticalScale(100),
      m_iLineRotation(0),
      m_iCharRotation(0),
      m_iRotation(0),
      m_iCharSpace(0),
      m_bWordSpace(FALSE),
      m_iWordSpace(0),
      m_bRTL(FALSE),
      m_iAlignment(FX_RTFLINEALIGNMENT_Left),
      m_pUserData(nullptr),
      m_eCharType(FX_CHARTYPE_Unknown),
      m_dwIdentity(0),
      m_RTFLine1(),
      m_RTFLine2(),
      m_pCurLine(nullptr),
      m_iReady(0),
      m_iTolerance(0) {
  m_pCurLine = &m_RTFLine1;
}
CFX_RTFBreak::~CFX_RTFBreak() {
  Reset();
  m_PositionedTabs.RemoveAll();
  if (m_pUserData) {
    m_pUserData->Release();
  }
}
void CFX_RTFBreak::SetLineBoundary(FX_FLOAT fLineStart, FX_FLOAT fLineEnd) {
  if (fLineStart > fLineEnd)
    return;
  m_iBoundaryStart = FXSYS_round(fLineStart * 20000.0f);
  m_iBoundaryEnd = FXSYS_round(fLineEnd * 20000.0f);
  m_pCurLine->m_iStart = std::min(m_pCurLine->m_iStart, m_iBoundaryEnd);
  m_pCurLine->m_iStart = std::max(m_pCurLine->m_iStart, m_iBoundaryStart);
}
void CFX_RTFBreak::SetLineStartPos(FX_FLOAT fLinePos) {
  int32_t iLinePos = FXSYS_round(fLinePos * 20000.0f);
  iLinePos = std::min(iLinePos, m_iBoundaryEnd);
  iLinePos = std::max(iLinePos, m_iBoundaryStart);
  m_pCurLine->m_iStart = iLinePos;
}
void CFX_RTFBreak::SetLayoutStyles(uint32_t dwLayoutStyles) {
  if (m_dwLayoutStyles == dwLayoutStyles) {
    return;
  }
  SetBreakStatus();
  m_dwLayoutStyles = dwLayoutStyles;
  m_bPagination = (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_Pagination) != 0;
  m_bVertical = (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_VerticalChars) != 0;
  m_bSingleLine = (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_SingleLine) != 0;
  m_bCharCode = (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_MBCSCode) != 0;
  m_iLineRotation = GetLineRotation(m_dwLayoutStyles);
  m_iRotation = m_iLineRotation + m_iCharRotation;
  m_iRotation %= 4;
}
void CFX_RTFBreak::SetFont(CFGAS_GEFont* pFont) {
  if (!pFont) {
    return;
  }
  if (m_pFont == pFont) {
    return;
  }
  SetBreakStatus();
  m_pFont = pFont;
  m_iDefChar = 0;
  if (m_pFont) {
    m_iFontHeight = m_iFontSize;
    if (m_wDefChar != 0xFEFF) {
      m_pFont->GetCharWidth(m_wDefChar, m_iDefChar, FALSE);
      m_iDefChar *= m_iFontSize;
    }
  }
}
void CFX_RTFBreak::SetFontSize(FX_FLOAT fFontSize) {
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  if (m_iFontSize == iFontSize) {
    return;
  }
  SetBreakStatus();
  m_iFontSize = iFontSize;
  m_iDefChar = 0;
  if (m_pFont) {
    m_iFontHeight = m_iFontSize;
    if (m_wDefChar != 0xFEFF) {
      m_pFont->GetCharWidth(m_wDefChar, m_iDefChar, FALSE);
      m_iDefChar *= m_iFontSize;
    }
  }
}
void CFX_RTFBreak::SetTabWidth(FX_FLOAT fTabWidth) {
  m_iTabWidth = FXSYS_round(fTabWidth * 20000.0f);
}
void CFX_RTFBreak::AddPositionedTab(FX_FLOAT fTabPos) {
  int32_t iLineEnd = m_iBoundaryEnd;
  int32_t iTabPos = FXSYS_round(fTabPos * 20000.0f) + m_iBoundaryStart;
  if (iTabPos > iLineEnd) {
    iTabPos = iLineEnd;
  }
  if (m_PositionedTabs.Find(iTabPos, 0) > -1) {
    return;
  }
  int32_t iCount = m_PositionedTabs.GetSize();
  int32_t iFind = 0;
  for (; iFind < iCount; iFind++) {
    if (m_PositionedTabs[iFind] > iTabPos) {
      break;
    }
  }
  m_PositionedTabs.InsertAt(iFind, iTabPos);
  if (m_dwPolicies & FX_RTFBREAKPOLICY_OrphanPositionedTab) {
    m_bOrphanLine = GetLastPositionedTab() >= iLineEnd;
  } else {
    m_bOrphanLine = FALSE;
  }
}
void CFX_RTFBreak::SetPositionedTabs(const CFX_FloatArray& tabs) {
  m_PositionedTabs.RemoveAll();
  int32_t iCount = tabs.GetSize();
  m_PositionedTabs.SetSize(iCount);
  int32_t iLineEnd = m_iBoundaryEnd;
  int32_t iTabPos;
  for (int32_t i = 0; i < iCount; i++) {
    iTabPos = FXSYS_round(tabs[i] * 20000.0f) + m_iBoundaryStart;
    if (iTabPos > iLineEnd) {
      iTabPos = iLineEnd;
    }
    m_PositionedTabs[i] = iTabPos;
  }
  if (m_dwPolicies & FX_RTFBREAKPOLICY_OrphanPositionedTab) {
    m_bOrphanLine = GetLastPositionedTab() >= iLineEnd;
  } else {
    m_bOrphanLine = FALSE;
  }
}
void CFX_RTFBreak::ClearPositionedTabs() {
  m_PositionedTabs.RemoveAll();
  m_bOrphanLine = FALSE;
}
void CFX_RTFBreak::SetDefaultChar(FX_WCHAR wch) {
  m_wDefChar = wch;
  m_iDefChar = 0;
  if (m_wDefChar != 0xFEFF && m_pFont) {
    m_pFont->GetCharWidth(m_wDefChar, m_iDefChar, FALSE);
    if (m_iDefChar < 0) {
      m_iDefChar = 0;
    } else {
      m_iDefChar *= m_iFontSize;
    }
  }
}
void CFX_RTFBreak::SetLineBreakChar(FX_WCHAR wch) {
  if (wch != L'\r' && wch != L'\n') {
    return;
  }
  m_wLineBreakChar = wch;
}
void CFX_RTFBreak::SetLineBreakTolerance(FX_FLOAT fTolerance) {
  m_iTolerance = FXSYS_round(fTolerance * 20000.0f);
}
void CFX_RTFBreak::SetHorizontalScale(int32_t iScale) {
  if (iScale < 0) {
    iScale = 0;
  }
  if (m_iHorizontalScale == iScale) {
    return;
  }
  SetBreakStatus();
  m_iHorizontalScale = iScale;
}
void CFX_RTFBreak::SetVerticalScale(int32_t iScale) {
  if (iScale < 0) {
    iScale = 0;
  }
  if (m_iVerticalScale == iScale) {
    return;
  }
  SetBreakStatus();
  m_iVerticalScale = iScale;
}
void CFX_RTFBreak::SetCharRotation(int32_t iCharRotation) {
  if (iCharRotation < 0) {
    iCharRotation += (-iCharRotation / 4 + 1) * 4;
  } else if (iCharRotation > 3) {
    iCharRotation -= (iCharRotation / 4) * 4;
  }
  if (m_iCharRotation == iCharRotation) {
    return;
  }
  SetBreakStatus();
  m_iCharRotation = iCharRotation;
  m_iRotation = m_iLineRotation + m_iCharRotation;
  m_iRotation %= 4;
}
void CFX_RTFBreak::SetCharSpace(FX_FLOAT fCharSpace) {
  m_iCharSpace = FXSYS_round(fCharSpace * 20000.0f);
}
void CFX_RTFBreak::SetWordSpace(FX_BOOL bDefault, FX_FLOAT fWordSpace) {
  m_bWordSpace = !bDefault;
  m_iWordSpace = FXSYS_round(fWordSpace * 20000.0f);
}
void CFX_RTFBreak::SetReadingOrder(FX_BOOL bRTL) {
  m_bRTL = bRTL;
}
void CFX_RTFBreak::SetAlignment(int32_t iAlignment) {
  ASSERT(iAlignment >= FX_RTFLINEALIGNMENT_Left &&
         iAlignment <= FX_RTFLINEALIGNMENT_Distributed);
  m_iAlignment = iAlignment;
}
void CFX_RTFBreak::SetUserData(IFX_Retainable* pUserData) {
  if (m_pUserData == pUserData) {
    return;
  }
  SetBreakStatus();
  if (m_pUserData) {
    m_pUserData->Release();
  }
  m_pUserData = pUserData;
  if (m_pUserData) {
    m_pUserData->Retain();
  }
}
static const int32_t gs_FX_RTFLineRotations[8] = {0, 3, 1, 0, 2, 1, 3, 2};
int32_t CFX_RTFBreak::GetLineRotation(uint32_t dwStyles) const {
  return gs_FX_RTFLineRotations[(dwStyles & 0x0E) >> 1];
}
void CFX_RTFBreak::SetBreakStatus() {
  m_dwIdentity++;
  int32_t iCount = m_pCurLine->CountChars();
  if (iCount < 1) {
    return;
  }
  CFX_RTFChar& tc = m_pCurLine->GetChar(iCount - 1);
  if (tc.m_dwStatus == 0) {
    tc.m_dwStatus = FX_RTFBREAK_PieceBreak;
  }
}
CFX_RTFChar* CFX_RTFBreak::GetLastChar(int32_t index) const {
  CFX_RTFCharArray& tca = m_pCurLine->m_LineChars;
  int32_t iCount = tca.GetSize();
  if (index < 0 || index >= iCount) {
    return nullptr;
  }
  CFX_RTFChar* pTC;
  int32_t iStart = iCount - 1;
  while (iStart > -1) {
    pTC = tca.GetDataPtr(iStart--);
    if (pTC->m_iCharWidth >= 0 ||
        pTC->GetCharType() != FX_CHARTYPE_Combination) {
      if (--index < 0) {
        return pTC;
      }
    }
  }
  return nullptr;
}
CFX_RTFLine* CFX_RTFBreak::GetRTFLine(FX_BOOL bReady) const {
  if (bReady) {
    if (m_iReady == 1) {
      return (CFX_RTFLine*)&m_RTFLine1;
    } else if (m_iReady == 2) {
      return (CFX_RTFLine*)&m_RTFLine2;
    } else {
      return nullptr;
    }
  }
  ASSERT(m_pCurLine);
  return m_pCurLine;
}
CFX_RTFPieceArray* CFX_RTFBreak::GetRTFPieces(FX_BOOL bReady) const {
  CFX_RTFLine* pRTFLine = GetRTFLine(bReady);
  return pRTFLine ? &pRTFLine->m_LinePieces : nullptr;
}
inline FX_CHARTYPE CFX_RTFBreak::GetUnifiedCharType(
    FX_CHARTYPE chartype) const {
  return chartype >= FX_CHARTYPE_ArabicAlef ? FX_CHARTYPE_Arabic : chartype;
}
int32_t CFX_RTFBreak::GetLastPositionedTab() const {
  int32_t iCount = m_PositionedTabs.GetSize();
  if (iCount < 1) {
    return m_iBoundaryStart;
  }
  return m_PositionedTabs[iCount - 1];
}
FX_BOOL CFX_RTFBreak::GetPositionedTab(int32_t& iTabPos) const {
  int32_t iCount = m_PositionedTabs.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    if (m_PositionedTabs[i] > iTabPos) {
      iTabPos = m_PositionedTabs[i];
      return TRUE;
    }
  }
  return FALSE;
}
typedef uint32_t (CFX_RTFBreak::*FX_RTFBreak_LPFAppendChar)(
    CFX_RTFChar* pCurChar,
    int32_t iRotation);
static const FX_RTFBreak_LPFAppendChar g_FX_RTFBreak_lpfAppendChar[16] = {
    &CFX_RTFBreak::AppendChar_Others,      &CFX_RTFBreak::AppendChar_Tab,
    &CFX_RTFBreak::AppendChar_Others,      &CFX_RTFBreak::AppendChar_Control,
    &CFX_RTFBreak::AppendChar_Combination, &CFX_RTFBreak::AppendChar_Others,
    &CFX_RTFBreak::AppendChar_Others,      &CFX_RTFBreak::AppendChar_Arabic,
    &CFX_RTFBreak::AppendChar_Arabic,      &CFX_RTFBreak::AppendChar_Arabic,
    &CFX_RTFBreak::AppendChar_Arabic,      &CFX_RTFBreak::AppendChar_Arabic,
    &CFX_RTFBreak::AppendChar_Arabic,      &CFX_RTFBreak::AppendChar_Others,
    &CFX_RTFBreak::AppendChar_Others,      &CFX_RTFBreak::AppendChar_Others,
};
uint32_t CFX_RTFBreak::AppendChar(FX_WCHAR wch) {
  ASSERT(m_pFont && m_pCurLine);
  if (m_bCharCode) {
    return AppendChar_CharCode(wch);
  }
  uint32_t dwProps = kTextLayoutCodeProperties[(uint16_t)wch];
  FX_CHARTYPE chartype = GetCharTypeFromProp(dwProps);
  CFX_RTFCharArray& tca = m_pCurLine->m_LineChars;
  CFX_RTFChar* pCurChar = tca.AddSpace();
  pCurChar->m_dwStatus = 0;
  pCurChar->m_wCharCode = wch;
  pCurChar->m_dwCharProps = dwProps;
  pCurChar->m_dwCharStyles = 0;
  pCurChar->m_dwLayoutStyles = 0;
  pCurChar->m_iFontSize = m_iFontSize;
  pCurChar->m_iFontHeight = m_iFontHeight;
  pCurChar->m_iHorizontalScale = m_iHorizontalScale;
  pCurChar->m_iVertialScale = m_iVerticalScale;
  pCurChar->m_nRotation = m_iCharRotation;
  pCurChar->m_iCharWidth = 0;
  pCurChar->m_dwIdentity = m_dwIdentity;
  if (m_pUserData) {
    m_pUserData->Retain();
  }
  pCurChar->m_pUserData = m_pUserData;
  uint32_t dwRet1 = FX_RTFBREAK_None;
  if (chartype != FX_CHARTYPE_Combination &&
      GetUnifiedCharType(m_eCharType) != GetUnifiedCharType(chartype)) {
    if (!m_bSingleLine && !m_bOrphanLine &&
        m_eCharType != FX_CHARTYPE_Unknown &&
        m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance) {
      if (m_eCharType != FX_CHARTYPE_Space || chartype != FX_CHARTYPE_Control) {
        dwRet1 = EndBreak(FX_RTFBREAK_LineBreak);
        int32_t iCount = m_pCurLine->CountChars();
        if (iCount > 0) {
          pCurChar = m_pCurLine->m_LineChars.GetDataPtr(iCount - 1);
        }
      }
    }
  }
  int32_t iRotation = m_iRotation;
  if (m_bVertical && (dwProps & 0x8000) != 0) {
    iRotation = (iRotation + 1) % 4;
  }
  uint32_t dwRet2 =
      (this->*g_FX_RTFBreak_lpfAppendChar[chartype >> FX_CHARTYPEBITS])(
          pCurChar, iRotation);
  m_eCharType = chartype;
  return std::max(dwRet1, dwRet2);
}
uint32_t CFX_RTFBreak::AppendChar_CharCode(FX_WCHAR wch) {
  ASSERT(m_pFont && m_pCurLine);
  ASSERT(m_bCharCode);
  m_pCurLine->m_iMBCSChars++;
  CFX_RTFCharArray& tca = m_pCurLine->m_LineChars;
  CFX_RTFChar* pCurChar = tca.AddSpace();
  pCurChar->m_dwStatus = 0;
  pCurChar->m_wCharCode = wch;
  pCurChar->m_dwCharProps = 0;
  pCurChar->m_dwCharStyles = 0;
  pCurChar->m_dwLayoutStyles = m_dwLayoutStyles;
  pCurChar->m_iFontSize = m_iFontSize;
  pCurChar->m_iFontHeight = m_iFontHeight;
  pCurChar->m_iHorizontalScale = m_iHorizontalScale;
  pCurChar->m_iVertialScale = m_iVerticalScale;
  pCurChar->m_nRotation = m_iCharRotation;
  pCurChar->m_iCharWidth = 0;
  pCurChar->m_dwIdentity = m_dwIdentity;
  if (m_pUserData) {
    m_pUserData->Retain();
  }
  pCurChar->m_pUserData = m_pUserData;
  int32_t iCharWidth = 0;
  if (m_bVertical != FX_IsOdd(m_iRotation)) {
    iCharWidth = 1000;
  } else {
    if (!m_pFont->GetCharWidth(wch, iCharWidth, TRUE)) {
      iCharWidth = m_iDefChar;
    }
  }
  iCharWidth *= m_iFontSize;
  iCharWidth = iCharWidth * m_iHorizontalScale / 100;
  iCharWidth += m_iCharSpace;
  pCurChar->m_iCharWidth = iCharWidth;
  m_pCurLine->m_iWidth += iCharWidth;
  m_eCharType = FX_CHARTYPE_Unknown;
  if (!m_bSingleLine &&
      m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance) {
    return EndBreak(FX_RTFBREAK_LineBreak);
  }
  return FX_RTFBREAK_None;
}
uint32_t CFX_RTFBreak::AppendChar_Combination(CFX_RTFChar* pCurChar,
                                              int32_t iRotation) {
  int32_t iCharWidth = 0;
  if (m_bVertical != FX_IsOdd(iRotation)) {
    iCharWidth = 1000;
  } else {
    if (!m_pFont->GetCharWidth(pCurChar->m_wCharCode, iCharWidth,
                               m_bCharCode)) {
      iCharWidth = 0;
    }
  }
  iCharWidth *= m_iFontSize;
  iCharWidth = iCharWidth * m_iHorizontalScale / 100;
  CFX_RTFChar* pLastChar = GetLastChar(0);
  if (pLastChar && pLastChar->GetCharType() > FX_CHARTYPE_Combination) {
    iCharWidth = -iCharWidth;
  } else {
    m_eCharType = FX_CHARTYPE_Combination;
  }
  pCurChar->m_iCharWidth = iCharWidth;
  if (iCharWidth > 0) {
    m_pCurLine->m_iWidth += iCharWidth;
  }
  return FX_RTFBREAK_None;
}
uint32_t CFX_RTFBreak::AppendChar_Tab(CFX_RTFChar* pCurChar,
                                      int32_t iRotation) {
  if (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_ExpandTab) {
    FX_BOOL bBreak = FALSE;
    if ((m_dwPolicies & FX_RTFBREAKPOLICY_TabBreak) != 0) {
      bBreak = (m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance);
    }
    int32_t& iLineWidth = m_pCurLine->m_iWidth;
    int32_t iCharWidth = iLineWidth;
    if (GetPositionedTab(iCharWidth)) {
      iCharWidth -= iLineWidth;
    } else {
      iCharWidth = m_iTabWidth * (iLineWidth / m_iTabWidth + 1) - iLineWidth;
    }
    pCurChar->m_iCharWidth = iCharWidth;
    iLineWidth += iCharWidth;
    if (!m_bSingleLine && !m_bOrphanLine && bBreak) {
      return EndBreak(FX_RTFBREAK_LineBreak);
    }
  }
  return FX_RTFBREAK_None;
}
uint32_t CFX_RTFBreak::AppendChar_Control(CFX_RTFChar* pCurChar,
                                          int32_t iRotation) {
  uint32_t dwRet2 = FX_RTFBREAK_None;
  if (!m_bSingleLine) {
    switch (pCurChar->m_wCharCode) {
      case L'\v':
      case 0x2028:
        dwRet2 = FX_RTFBREAK_LineBreak;
        break;
      case L'\f':
        dwRet2 = FX_RTFBREAK_PageBreak;
        break;
      case 0x2029:
        dwRet2 = FX_RTFBREAK_ParagraphBreak;
        break;
      default:
        if (pCurChar->m_wCharCode == m_wLineBreakChar) {
          dwRet2 = FX_RTFBREAK_ParagraphBreak;
        }
        break;
    }
    if (dwRet2 != FX_RTFBREAK_None) {
      dwRet2 = EndBreak(dwRet2);
    }
  }
  return dwRet2;
}
uint32_t CFX_RTFBreak::AppendChar_Arabic(CFX_RTFChar* pCurChar,
                                         int32_t iRotation) {
  CFX_RTFChar* pLastChar = nullptr;
  int32_t& iLineWidth = m_pCurLine->m_iWidth;
  int32_t iCharWidth = 0;
  FX_WCHAR wForm;
  FX_BOOL bAlef = FALSE;
  if (m_eCharType >= FX_CHARTYPE_ArabicAlef &&
      m_eCharType <= FX_CHARTYPE_ArabicDistortion) {
    pLastChar = GetLastChar(1);
    if (pLastChar) {
      iLineWidth -= pLastChar->m_iCharWidth;
      CFX_RTFChar* pPrevChar = GetLastChar(2);
      wForm = pdfium::arabic::GetFormChar(pLastChar, pPrevChar, pCurChar);
      bAlef = (wForm == 0xFEFF &&
               pLastChar->GetCharType() == FX_CHARTYPE_ArabicAlef);
      int32_t iLastRotation = pLastChar->m_nRotation + m_iLineRotation;
      if (m_bVertical && (pLastChar->m_dwCharProps & 0x8000) != 0) {
        iLastRotation++;
      }
      if (m_bVertical != FX_IsOdd(iLastRotation)) {
        iCharWidth = 1000;
      } else {
        if (!m_pFont->GetCharWidth(wForm, iCharWidth, m_bCharCode)) {
          if (!m_pFont->GetCharWidth(pLastChar->m_wCharCode, iCharWidth,
                                     m_bCharCode)) {
            iCharWidth = m_iDefChar;
          }
        }
      }
      iCharWidth *= m_iFontSize;
      iCharWidth = iCharWidth * m_iHorizontalScale / 100;
      pLastChar->m_iCharWidth = iCharWidth;
      iLineWidth += iCharWidth;
      iCharWidth = 0;
    }
  }
  wForm = pdfium::arabic::GetFormChar(pCurChar, bAlef ? nullptr : pLastChar,
                                      nullptr);
  if (m_bVertical != FX_IsOdd(iRotation)) {
    iCharWidth = 1000;
  } else if (!m_pFont->GetCharWidth(wForm, iCharWidth, m_bCharCode) &&
             !m_pFont->GetCharWidth(pCurChar->m_wCharCode, iCharWidth,
                                    m_bCharCode)) {
    iCharWidth = m_iDefChar;
  }

  iCharWidth *= m_iFontSize;
  iCharWidth = iCharWidth * m_iHorizontalScale / 100;
  pCurChar->m_iCharWidth = iCharWidth;
  iLineWidth += iCharWidth;
  m_pCurLine->m_iArabicChars++;
  if (!m_bSingleLine && !m_bOrphanLine &&
      m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance) {
    return EndBreak(FX_RTFBREAK_LineBreak);
  }
  return FX_RTFBREAK_None;
}
uint32_t CFX_RTFBreak::AppendChar_Others(CFX_RTFChar* pCurChar,
                                         int32_t iRotation) {
  FX_CHARTYPE chartype = pCurChar->GetCharType();
  FX_WCHAR wForm;
  if (chartype == FX_CHARTYPE_Numeric) {
    if (m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_ArabicNumber) {
      wForm = pCurChar->m_wCharCode + 0x0630;
    } else {
      wForm = pCurChar->m_wCharCode;
    }
  } else if (m_bRTL || m_bVertical) {
    wForm = FX_GetMirrorChar(pCurChar->m_wCharCode, pCurChar->m_dwCharProps,
                             m_bRTL, m_bVertical);
  } else {
    wForm = pCurChar->m_wCharCode;
  }
  int32_t iCharWidth = 0;
  if (m_bVertical != FX_IsOdd(iRotation)) {
    iCharWidth = 1000;
  } else {
    if (!m_pFont->GetCharWidth(wForm, iCharWidth, m_bCharCode)) {
      iCharWidth = m_iDefChar;
    }
  }
  iCharWidth *= m_iFontSize;
  iCharWidth = iCharWidth * m_iHorizontalScale / 100;
  iCharWidth += m_iCharSpace;
  if (chartype == FX_CHARTYPE_Space && m_bWordSpace) {
    iCharWidth += m_iWordSpace;
  }
  pCurChar->m_iCharWidth = iCharWidth;
  m_pCurLine->m_iWidth += iCharWidth;
  FX_BOOL bBreak = (chartype != FX_CHARTYPE_Space ||
                    (m_dwPolicies & FX_RTFBREAKPOLICY_SpaceBreak) != 0);
  if (!m_bSingleLine && !m_bOrphanLine && bBreak &&
      m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance) {
    return EndBreak(FX_RTFBREAK_LineBreak);
  }
  return FX_RTFBREAK_None;
}
uint32_t CFX_RTFBreak::EndBreak(uint32_t dwStatus) {
  ASSERT(dwStatus >= FX_RTFBREAK_PieceBreak &&
         dwStatus <= FX_RTFBREAK_PageBreak);
  m_dwIdentity++;
  CFX_RTFPieceArray* pCurPieces = &m_pCurLine->m_LinePieces;
  int32_t iCount = pCurPieces->GetSize();
  if (iCount > 0) {
    CFX_RTFPiece* pLastPiece = pCurPieces->GetPtrAt(--iCount);
    if (dwStatus > FX_RTFBREAK_PieceBreak) {
      pLastPiece->m_dwStatus = dwStatus;
    } else {
      dwStatus = pLastPiece->m_dwStatus;
    }
    return dwStatus;
  } else {
    CFX_RTFLine* pLastLine = GetRTFLine(TRUE);
    if (pLastLine) {
      pCurPieces = &pLastLine->m_LinePieces;
      iCount = pCurPieces->GetSize();
      if (iCount-- > 0) {
        CFX_RTFPiece* pLastPiece = pCurPieces->GetPtrAt(iCount);
        if (dwStatus > FX_RTFBREAK_PieceBreak) {
          pLastPiece->m_dwStatus = dwStatus;
        } else {
          dwStatus = pLastPiece->m_dwStatus;
        }
        return dwStatus;
      }
      return FX_RTFBREAK_None;
    }
    iCount = m_pCurLine->CountChars();
    if (iCount < 1) {
      return FX_RTFBREAK_None;
    }
    CFX_RTFChar& tc = m_pCurLine->GetChar(iCount - 1);
    tc.m_dwStatus = dwStatus;
    if (dwStatus <= FX_RTFBREAK_PieceBreak) {
      return dwStatus;
    }
  }
  m_iReady = (m_pCurLine == &m_RTFLine1) ? 1 : 2;
  CFX_RTFLine* pNextLine =
      (m_pCurLine == &m_RTFLine1) ? &m_RTFLine2 : &m_RTFLine1;
  FX_BOOL bAllChars = (m_iAlignment > FX_RTFLINEALIGNMENT_Right);
  CFX_TPOArray tpos;
  if (EndBreak_SplitLine(pNextLine, bAllChars, dwStatus)) {
    goto EndBreak_Ret;
  }
  if (!m_bCharCode) {
    EndBreak_BidiLine(tpos, dwStatus);
  }
  if (!m_bPagination && m_iAlignment > FX_RTFLINEALIGNMENT_Left) {
    EndBreak_Alignment(tpos, bAllChars, dwStatus);
  }
EndBreak_Ret:
  m_pCurLine = pNextLine;
  m_pCurLine->m_iStart = m_iBoundaryStart;
  CFX_RTFChar* pTC = GetLastChar(0);
  m_eCharType = pTC ? pTC->GetCharType() : FX_CHARTYPE_Unknown;
  return dwStatus;
}
FX_BOOL CFX_RTFBreak::EndBreak_SplitLine(CFX_RTFLine* pNextLine,
                                         FX_BOOL bAllChars,
                                         uint32_t dwStatus) {
  FX_BOOL bDone = FALSE;
  if (!m_bSingleLine && !m_bOrphanLine &&
      m_pCurLine->GetLineEnd() > m_iBoundaryEnd + m_iTolerance) {
    CFX_RTFChar& tc = m_pCurLine->GetChar(m_pCurLine->CountChars() - 1);
    switch (tc.GetCharType()) {
      case FX_CHARTYPE_Tab:
        if ((m_dwPolicies & FX_RTFBREAKPOLICY_TabBreak) != 0) {
          SplitTextLine(m_pCurLine, pNextLine, !m_bPagination && bAllChars);
          bDone = TRUE;
        }
        break;
      case FX_CHARTYPE_Control:
        break;
      case FX_CHARTYPE_Space:
        if ((m_dwPolicies & FX_RTFBREAKPOLICY_SpaceBreak) != 0) {
          SplitTextLine(m_pCurLine, pNextLine, !m_bPagination && bAllChars);
          bDone = TRUE;
        }
        break;
      default:
        SplitTextLine(m_pCurLine, pNextLine, !m_bPagination && bAllChars);
        bDone = TRUE;
        break;
    }
  }
  if (m_bPagination || m_pCurLine->m_iMBCSChars > 0) {
    const CFX_RTFChar* pCurChars = m_pCurLine->m_LineChars.GetData();
    const CFX_RTFChar* pTC;
    CFX_RTFPieceArray* pCurPieces = &m_pCurLine->m_LinePieces;
    CFX_RTFPiece tp;
    tp.m_pChars = &m_pCurLine->m_LineChars;
    FX_BOOL bNew = TRUE;
    uint32_t dwIdentity = (uint32_t)-1;
    int32_t iLast = m_pCurLine->CountChars() - 1, j = 0;
    for (int32_t i = 0; i <= iLast;) {
      pTC = pCurChars + i;
      if (bNew) {
        tp.m_iStartChar = i;
        tp.m_iStartPos += tp.m_iWidth;
        tp.m_iWidth = 0;
        tp.m_dwStatus = pTC->m_dwStatus;
        tp.m_iFontSize = pTC->m_iFontSize;
        tp.m_iFontHeight = pTC->m_iFontHeight;
        tp.m_iHorizontalScale = pTC->m_iHorizontalScale;
        tp.m_iVerticalScale = pTC->m_iVertialScale;
        tp.m_dwLayoutStyles = pTC->m_dwLayoutStyles;
        dwIdentity = pTC->m_dwIdentity;
        tp.m_dwIdentity = dwIdentity;
        tp.m_pUserData = pTC->m_pUserData;
        j = i;
        bNew = FALSE;
      }
      if (i == iLast || pTC->m_dwStatus != FX_RTFBREAK_None ||
          pTC->m_dwIdentity != dwIdentity) {
        tp.m_iChars = i - j;
        if (pTC->m_dwIdentity == dwIdentity) {
          tp.m_dwStatus = pTC->m_dwStatus;
          tp.m_iWidth += pTC->m_iCharWidth;
          tp.m_iChars += 1;
          i++;
        }
        pCurPieces->Add(tp);
        bNew = TRUE;
      } else {
        tp.m_iWidth += pTC->m_iCharWidth;
        i++;
      }
    }
    return TRUE;
  }
  if (bAllChars && !bDone) {
    int32_t iEndPos = m_pCurLine->GetLineEnd();
    GetBreakPos(m_pCurLine->m_LineChars, iEndPos, bAllChars, TRUE);
  }
  return FALSE;
}
void CFX_RTFBreak::EndBreak_BidiLine(CFX_TPOArray& tpos, uint32_t dwStatus) {
  FX_TPO tpo;
  CFX_RTFPiece tp;
  CFX_RTFChar* pTC;
  int32_t i, j;
  CFX_RTFCharArray& chars = m_pCurLine->m_LineChars;
  int32_t iCount = m_pCurLine->CountChars();
  FX_BOOL bDone = (!m_bPagination && !m_bCharCode &&
                   (m_pCurLine->m_iArabicChars > 0 || m_bRTL));
  if (bDone) {
    int32_t iBidiNum = 0;
    for (i = 0; i < iCount; i++) {
      pTC = chars.GetDataPtr(i);
      pTC->m_iBidiPos = i;
      if (pTC->GetCharType() != FX_CHARTYPE_Control) {
        iBidiNum = i;
      }
      if (i == 0) {
        pTC->m_iBidiLevel = 1;
      }
    }
    FX_BidiLine(chars, iBidiNum + 1, m_bRTL ? 1 : 0);
  } else {
    for (i = 0; i < iCount; i++) {
      pTC = chars.GetDataPtr(i);
      pTC->m_iBidiLevel = 0;
      pTC->m_iBidiPos = 0;
      pTC->m_iBidiOrder = 0;
    }
  }
  tp.m_dwStatus = FX_RTFBREAK_PieceBreak;
  tp.m_iStartPos = m_pCurLine->m_iStart;
  tp.m_pChars = &chars;
  CFX_RTFPieceArray* pCurPieces = &m_pCurLine->m_LinePieces;
  int32_t iBidiLevel = -1, iCharWidth;
  uint32_t dwIdentity = (uint32_t)-1;
  i = j = 0;
  while (i < iCount) {
    pTC = chars.GetDataPtr(i);
    if (iBidiLevel < 0) {
      iBidiLevel = pTC->m_iBidiLevel;
      iCharWidth = pTC->m_iCharWidth;
      if (iCharWidth < 1) {
        tp.m_iWidth = 0;
      } else {
        tp.m_iWidth = iCharWidth;
      }
      tp.m_iBidiLevel = iBidiLevel;
      tp.m_iBidiPos = pTC->m_iBidiOrder;
      tp.m_iFontSize = pTC->m_iFontSize;
      tp.m_iFontHeight = pTC->m_iFontHeight;
      tp.m_iHorizontalScale = pTC->m_iHorizontalScale;
      tp.m_iVerticalScale = pTC->m_iVertialScale;
      dwIdentity = pTC->m_dwIdentity;
      tp.m_dwIdentity = dwIdentity;
      tp.m_pUserData = pTC->m_pUserData;
      tp.m_dwStatus = FX_RTFBREAK_PieceBreak;
      i++;
    } else if (iBidiLevel != pTC->m_iBidiLevel ||
               pTC->m_dwIdentity != dwIdentity) {
      tp.m_iChars = i - tp.m_iStartChar;
      pCurPieces->Add(tp);
      tp.m_iStartPos += tp.m_iWidth;
      tp.m_iStartChar = i;
      tpo.index = j++;
      tpo.pos = tp.m_iBidiPos;
      tpos.Add(tpo);
      iBidiLevel = -1;
    } else {
      iCharWidth = pTC->m_iCharWidth;
      if (iCharWidth > 0) {
        tp.m_iWidth += iCharWidth;
      }
      i++;
    }
  }
  if (i > tp.m_iStartChar) {
    tp.m_dwStatus = dwStatus;
    tp.m_iChars = i - tp.m_iStartChar;
    pCurPieces->Add(tp);
    tpo.index = j;
    tpo.pos = tp.m_iBidiPos;
    tpos.Add(tpo);
  }
  if (!m_bCharCode) {
    j = tpos.GetSize() - 1;
    FX_TEXTLAYOUT_PieceSort(tpos, 0, j);
    int32_t iStartPos = m_pCurLine->m_iStart;
    for (i = 0; i <= j; i++) {
      tpo = tpos.GetAt(i);
      CFX_RTFPiece& ttp = pCurPieces->GetAt(tpo.index);
      ttp.m_iStartPos = iStartPos;
      iStartPos += ttp.m_iWidth;
    }
  }
}
void CFX_RTFBreak::EndBreak_Alignment(CFX_TPOArray& tpos,
                                      FX_BOOL bAllChars,
                                      uint32_t dwStatus) {
  CFX_RTFPieceArray* pCurPieces = &m_pCurLine->m_LinePieces;
  int32_t iNetWidth = m_pCurLine->m_iWidth, iGapChars = 0, iCharWidth;
  int32_t iCount = pCurPieces->GetSize();
  FX_BOOL bFind = FALSE;
  uint32_t dwCharType;
  int32_t i, j;
  FX_TPO tpo;
  for (i = iCount - 1; i > -1; i--) {
    tpo = tpos.GetAt(i);
    CFX_RTFPiece& ttp = pCurPieces->GetAt(tpo.index);
    if (!bFind) {
      iNetWidth = ttp.GetEndPos();
    }
    FX_BOOL bArabic = FX_IsOdd(ttp.m_iBidiLevel);
    j = bArabic ? 0 : ttp.m_iChars - 1;
    while (j > -1 && j < ttp.m_iChars) {
      const CFX_RTFChar& tc = ttp.GetChar(j);
      if (tc.m_nBreakType == FX_LBT_DIRECT_BRK) {
        iGapChars++;
      }
      if (!bFind || !bAllChars) {
        dwCharType = tc.GetCharType();
        if (dwCharType == FX_CHARTYPE_Space ||
            dwCharType == FX_CHARTYPE_Control) {
          if (!bFind) {
            iCharWidth = tc.m_iCharWidth;
            if (bAllChars && iCharWidth > 0) {
              iNetWidth -= iCharWidth;
            }
          }
        } else {
          bFind = TRUE;
          if (!bAllChars) {
            break;
          }
        }
      }
      j += bArabic ? 1 : -1;
    }
    if (!bAllChars && bFind) {
      break;
    }
  }
  int32_t iOffset = m_iBoundaryEnd - iNetWidth;
  int32_t iLowerAlignment = (m_iAlignment & FX_RTFLINEALIGNMENT_LowerMask);
  int32_t iHigherAlignment = (m_iAlignment & FX_RTFLINEALIGNMENT_HigherMask);
  if (iGapChars > 0 && (iHigherAlignment == FX_RTFLINEALIGNMENT_Distributed ||
                        (iHigherAlignment == FX_RTFLINEALIGNMENT_Justified &&
                         dwStatus != FX_RTFBREAK_ParagraphBreak))) {
    int32_t iStart = -1;
    for (i = 0; i < iCount; i++) {
      tpo = tpos.GetAt(i);
      CFX_RTFPiece& ttp = pCurPieces->GetAt(tpo.index);
      if (iStart < 0) {
        iStart = ttp.m_iStartPos;
      } else {
        ttp.m_iStartPos = iStart;
      }
      int32_t k;
      for (j = 0; j < ttp.m_iChars; j++) {
        CFX_RTFChar& tc = ttp.GetChar(j);
        if (tc.m_nBreakType != FX_LBT_DIRECT_BRK || tc.m_iCharWidth < 0) {
          continue;
        }
        k = iOffset / iGapChars;
        tc.m_iCharWidth += k;
        ttp.m_iWidth += k;
        iOffset -= k;
        iGapChars--;
        if (iGapChars < 1) {
          break;
        }
      }
      iStart += ttp.m_iWidth;
    }
  } else if (iLowerAlignment > FX_RTFLINEALIGNMENT_Left) {
    if (iLowerAlignment == FX_RTFLINEALIGNMENT_Center) {
      iOffset /= 2;
    }
    if (iOffset > 0) {
      for (i = 0; i < iCount; i++) {
        CFX_RTFPiece& ttp = pCurPieces->GetAt(i);
        ttp.m_iStartPos += iOffset;
      }
    }
  }
}
int32_t CFX_RTFBreak::GetBreakPos(CFX_RTFCharArray& tca,
                                  int32_t& iEndPos,
                                  FX_BOOL bAllChars,
                                  FX_BOOL bOnlyBrk) {
  int32_t iLength = tca.GetSize() - 1;
  if (iLength < 1) {
    return iLength;
  }
  int32_t iBreak = -1, iBreakPos = -1, iIndirect = -1, iIndirectPos = -1,
          iLast = -1, iLastPos = -1;
  if (m_bSingleLine || m_bOrphanLine || iEndPos <= m_iBoundaryEnd) {
    if (!bAllChars || m_bCharCode) {
      return iLength;
    }
    iBreak = iLength;
    iBreakPos = iEndPos;
  }
  CFX_RTFChar* pCharArray = tca.GetData();
  if (m_bCharCode) {
    const CFX_RTFChar* pChar;
    int32_t iCharWidth;
    while (iLength > 0) {
      if (iEndPos <= m_iBoundaryEnd) {
        break;
      }
      pChar = pCharArray + iLength--;
      iCharWidth = pChar->m_iCharWidth;
      if (iCharWidth > 0) {
        iEndPos -= iCharWidth;
      }
    }
    return iLength;
  }
  FX_BOOL bSpaceBreak = (m_dwPolicies & FX_RTFBREAKPOLICY_SpaceBreak) != 0;
  FX_BOOL bTabBreak = (m_dwPolicies & FX_RTFBREAKPOLICY_TabBreak) != 0;
  FX_BOOL bNumberBreak = (m_dwPolicies & FX_RTFBREAKPOLICY_NumberBreak) != 0;
  FX_BOOL bInfixBreak = (m_dwPolicies & FX_RTFBREAKPOLICY_InfixBreak) != 0;
  FX_LINEBREAKTYPE eType;
  uint32_t nCodeProp, nCur, nNext;
  CFX_RTFChar* pCur = pCharArray + iLength--;
  if (bAllChars) {
    pCur->m_nBreakType = FX_LBT_UNKNOWN;
  }
  nCodeProp = pCur->m_dwCharProps;
  nNext = nCodeProp & 0x003F;
  int32_t iCharWidth = pCur->m_iCharWidth;
  if (iCharWidth > 0) {
    iEndPos -= iCharWidth;
  }
  while (iLength >= 0) {
    pCur = pCharArray + iLength;
    nCodeProp = pCur->m_dwCharProps;
    nCur = nCodeProp & 0x003F;
    FX_BOOL bNeedBreak = FALSE;
    if (nCur == FX_CBP_SP) {
      bNeedBreak = !bSpaceBreak;
      if (nNext == FX_CBP_SP) {
        eType = bSpaceBreak ? FX_LBT_DIRECT_BRK : FX_LBT_PROHIBITED_BRK;
      } else {
        eType = gs_FX_LineBreak_PairTable[nCur][nNext];
      }
    } else if (nCur == FX_CBP_TB) {
      bNeedBreak = !bTabBreak;
      if (nNext == FX_CBP_TB) {
        eType = bTabBreak ? FX_LBT_DIRECT_BRK : FX_LBT_PROHIBITED_BRK;
      } else {
        eType = gs_FX_LineBreak_PairTable[nCur][nNext];
      }
    } else if ((bNumberBreak && nCur == FX_CBP_NU && nNext == FX_CBP_NU) ||
               (bInfixBreak && nCur == FX_CBP_IS && nNext == FX_CBP_IS)) {
      eType = FX_LBT_DIRECT_BRK;
    } else {
      if (nNext == FX_CBP_SP) {
        eType = FX_LBT_PROHIBITED_BRK;
      } else {
        eType = gs_FX_LineBreak_PairTable[nCur][nNext];
      }
    }
    if (bAllChars) {
      pCur->m_nBreakType = eType;
    }
    if (!bOnlyBrk) {
      iCharWidth = pCur->m_iCharWidth;
      FX_BOOL bBreak = FALSE;
      if (nCur == FX_CBP_TB && bTabBreak) {
        bBreak = iCharWidth > 0 && iEndPos - iCharWidth <= m_iBoundaryEnd;
      } else {
        bBreak = iEndPos <= m_iBoundaryEnd;
      }
      if (m_bSingleLine || m_bOrphanLine || bBreak || bNeedBreak) {
        if (eType == FX_LBT_DIRECT_BRK && iBreak < 0) {
          iBreak = iLength;
          iBreakPos = iEndPos;
          if (!bAllChars) {
            return iLength;
          }
        } else if (eType == FX_LBT_INDIRECT_BRK && iIndirect < 0) {
          iIndirect = iLength;
          iIndirectPos = iEndPos;
        }
        if (iLast < 0) {
          iLast = iLength;
          iLastPos = iEndPos;
        }
      }
      if (iCharWidth > 0) {
        iEndPos -= iCharWidth;
      }
    }
    nNext = nCodeProp & 0x003F;
    iLength--;
  }
  if (bOnlyBrk) {
    return 0;
  }
  if (iBreak > -1) {
    iEndPos = iBreakPos;
    return iBreak;
  }
  if (iIndirect > -1) {
    iEndPos = iIndirectPos;
    return iIndirect;
  }
  if (iLast > -1) {
    iEndPos = iLastPos;
    return iLast;
  }
  return 0;
}
void CFX_RTFBreak::SplitTextLine(CFX_RTFLine* pCurLine,
                                 CFX_RTFLine* pNextLine,
                                 FX_BOOL bAllChars) {
  ASSERT(pCurLine && pNextLine);
  int32_t iCount = pCurLine->CountChars();
  if (iCount < 2) {
    return;
  }
  int32_t iEndPos = pCurLine->GetLineEnd();
  CFX_RTFCharArray& curChars = pCurLine->m_LineChars;
  int32_t iCharPos = GetBreakPos(curChars, iEndPos, bAllChars, FALSE);
  if (iCharPos < 0) {
    iCharPos = 0;
  }
  iCharPos++;
  if (iCharPos >= iCount) {
    pNextLine->RemoveAll(TRUE);
    CFX_Char* pTC = curChars.GetDataPtr(iCharPos - 1);
    pTC->m_nBreakType = FX_LBT_UNKNOWN;
    return;
  }
  CFX_RTFCharArray& nextChars = pNextLine->m_LineChars;
  int cur_size = curChars.GetSize();
  nextChars.SetSize(cur_size - iCharPos);
  FXSYS_memcpy(nextChars.GetData(), curChars.GetDataPtr(iCharPos),
               (cur_size - iCharPos) * sizeof(CFX_RTFChar));
  iCount -= iCharPos;
  cur_size = curChars.GetSize();
  curChars.RemoveAt(cur_size - iCount, iCount);
  pNextLine->m_iStart = pCurLine->m_iStart;
  pNextLine->m_iWidth = pCurLine->GetLineEnd() - iEndPos;
  pCurLine->m_iWidth = iEndPos;
  curChars.GetDataPtr(iCharPos - 1)->m_nBreakType = FX_LBT_UNKNOWN;
  iCount = nextChars.GetSize();
  CFX_RTFChar* pNextChars = nextChars.GetData();
  for (int32_t i = 0; i < iCount; i++) {
    CFX_RTFChar* tc = pNextChars + i;
    if (tc->GetCharType() >= FX_CHARTYPE_ArabicAlef) {
      pCurLine->m_iArabicChars--;
      pNextLine->m_iArabicChars++;
    }
    if (tc->m_dwLayoutStyles & FX_RTFLAYOUTSTYLE_MBCSCode) {
      pCurLine->m_iMBCSChars--;
      pNextLine->m_iMBCSChars++;
    }
    tc->m_dwStatus = 0;
  }
}
int32_t CFX_RTFBreak::CountBreakPieces() const {
  CFX_RTFPieceArray* pRTFPieces = GetRTFPieces(TRUE);
  return pRTFPieces ? pRTFPieces->GetSize() : 0;
}
const CFX_RTFPiece* CFX_RTFBreak::GetBreakPiece(int32_t index) const {
  CFX_RTFPieceArray* pRTFPieces = GetRTFPieces(TRUE);
  if (!pRTFPieces) {
    return nullptr;
  }
  if (index < 0 || index >= pRTFPieces->GetSize()) {
    return nullptr;
  }
  return pRTFPieces->GetPtrAt(index);
}
void CFX_RTFBreak::GetLineRect(CFX_RectF& rect) const {
  rect.top = 0;
  CFX_RTFLine* pRTFLine = GetRTFLine(TRUE);
  if (!pRTFLine) {
    rect.left = ((FX_FLOAT)m_iBoundaryStart) / 20000.0f;
    rect.width = rect.height = 0;
    return;
  }
  rect.left = ((FX_FLOAT)pRTFLine->m_iStart) / 20000.0f;
  rect.width = ((FX_FLOAT)pRTFLine->m_iWidth) / 20000.0f;
  CFX_RTFPieceArray& rtfPieces = pRTFLine->m_LinePieces;
  int32_t iCount = rtfPieces.GetSize();
  if (iCount < 1) {
    rect.width = 0;
    return;
  }
  CFX_RTFPiece* pBreakPiece;
  int32_t iLineHeight = 0, iMax;
  for (int32_t i = 0; i < iCount; i++) {
    pBreakPiece = rtfPieces.GetPtrAt(i);
    int32_t iFontHeight = FXSYS_round(pBreakPiece->m_iFontHeight *
                                      pBreakPiece->m_iVerticalScale / 100.0f);
    iMax = std::max(pBreakPiece->m_iFontSize, iFontHeight);
    if (i == 0) {
      iLineHeight = iMax;
    } else if (iLineHeight < iMax) {
      iLineHeight = iMax;
    }
  }
  rect.height = ((FX_FLOAT)iLineHeight) / 20.0f;
}
void CFX_RTFBreak::ClearBreakPieces() {
  CFX_RTFLine* pRTFLine = GetRTFLine(TRUE);
  if (pRTFLine) {
    pRTFLine->RemoveAll(TRUE);
  }
  m_iReady = 0;
}
void CFX_RTFBreak::Reset() {
  m_eCharType = FX_CHARTYPE_Unknown;
  m_RTFLine1.RemoveAll(TRUE);
  m_RTFLine2.RemoveAll(TRUE);
}
int32_t CFX_RTFBreak::GetDisplayPos(const FX_RTFTEXTOBJ* pText,
                                    FXTEXT_CHARPOS* pCharPos,
                                    FX_BOOL bCharCode,
                                    CFX_WideString* pWSForms,
                                    FX_AdjustCharDisplayPos pAdjustPos) const {
  if (!pText || pText->iLength < 1) {
    return 0;
  }
  ASSERT(pText->pStr && pText->pWidths && pText->pFont && pText->pRect);
  const FX_WCHAR* pStr = pText->pStr;
  int32_t* pWidths = pText->pWidths;
  int32_t iLength = pText->iLength - 1;
  CFGAS_GEFont* pFont = pText->pFont;
  uint32_t dwStyles = pText->dwLayoutStyles;
  CFX_RectF rtText(*pText->pRect);
  FX_BOOL bRTLPiece = FX_IsOdd(pText->iBidiLevel);
  FX_FLOAT fFontSize = pText->fFontSize;
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  int32_t iAscent = pFont->GetAscent();
  int32_t iDescent = pFont->GetDescent();
  int32_t iMaxHeight = iAscent - iDescent;
  FX_FLOAT fFontHeight = fFontSize;
  FX_FLOAT fAscent = fFontHeight * (FX_FLOAT)iAscent / (FX_FLOAT)iMaxHeight;
  FX_FLOAT fDescent = fFontHeight * (FX_FLOAT)iDescent / (FX_FLOAT)iMaxHeight;
  FX_BOOL bVerticalDoc = (dwStyles & FX_RTFLAYOUTSTYLE_VerticalLayout) != 0;
  FX_BOOL bVerticalChar = (dwStyles & FX_RTFLAYOUTSTYLE_VerticalChars) != 0;
  FX_BOOL bArabicNumber = (dwStyles & FX_RTFLAYOUTSTYLE_ArabicNumber) != 0;
  FX_BOOL bMBCSCode = (dwStyles & FX_RTFLAYOUTSTYLE_MBCSCode) != 0;
  int32_t iRotation = GetLineRotation(dwStyles) + pText->iCharRotation;
  int32_t iCharRotation;
  FX_WCHAR wch, wPrev = 0xFEFF, wNext, wForm;
  int32_t iWidth, iCharWidth, iCharHeight;
  FX_FLOAT fX, fY, fCharWidth, fCharHeight;
  int32_t iHorScale = pText->iHorizontalScale;
  int32_t iVerScale = pText->iVerticalScale;
  FX_BOOL bEmptyChar;
  uint32_t dwProps, dwCharType;
  fX = rtText.left;
  fY = rtText.top;
  if (bVerticalDoc) {
    fX += (rtText.width - fFontSize) / 2.0f;
    if (bRTLPiece) {
      fY = rtText.bottom();
    }
  } else {
    if (bRTLPiece) {
      fX = rtText.right();
    }
    fY += fAscent;
  }
  int32_t iCount = 0;
  for (int32_t i = 0; i <= iLength; i++) {
    wch = *pStr++;
    iWidth = *pWidths++;
    if (!bMBCSCode) {
      dwProps = FX_GetUnicodeProperties(wch);
      dwCharType = (dwProps & FX_CHARTYPEBITSMASK);
      if (dwCharType == FX_CHARTYPE_ArabicAlef && iWidth == 0) {
        wPrev = 0xFEFF;
        continue;
      }
    } else {
      dwProps = 0;
      dwCharType = 0;
    }
    if (iWidth != 0) {
      iCharWidth = iWidth;
      if (iCharWidth < 0) {
        iCharWidth = -iCharWidth;
      }
      if (!bMBCSCode) {
        bEmptyChar = (dwCharType >= FX_CHARTYPE_Tab &&
                      dwCharType <= FX_CHARTYPE_Control);
      } else {
        bEmptyChar = FALSE;
      }
      if (!bEmptyChar) {
        iCount++;
      }
      if (pCharPos) {
        iCharWidth /= iFontSize;
        wForm = wch;
        if (!bMBCSCode) {
          if (dwCharType >= FX_CHARTYPE_ArabicAlef) {
            if (i < iLength) {
              wNext = *pStr;
              if (*pWidths < 0) {
                if (i + 1 < iLength) {
                  wNext = pStr[1];
                }
              }
            } else {
              wNext = 0xFEFF;
            }
            wForm = pdfium::arabic::GetFormChar(wch, wPrev, wNext);
          } else if (bRTLPiece || bVerticalChar) {
            wForm = FX_GetMirrorChar(wch, dwProps, bRTLPiece, bVerticalChar);
          } else if (dwCharType == FX_CHARTYPE_Numeric && bArabicNumber) {
            wForm = wch + 0x0630;
          }
          dwProps = FX_GetUnicodeProperties(wForm);
        }
        iCharRotation = iRotation;
        if (!bMBCSCode && bVerticalChar && (dwProps & 0x8000) != 0) {
          iCharRotation++;
          iCharRotation %= 4;
        }
        if (!bEmptyChar) {
          if (bCharCode) {
            pCharPos->m_GlyphIndex = wch;
          } else {
            pCharPos->m_GlyphIndex = pFont->GetGlyphIndex(wForm, bMBCSCode);
            if (pCharPos->m_GlyphIndex == 0xFFFF) {
              pCharPos->m_GlyphIndex = pFont->GetGlyphIndex(wch, bMBCSCode);
            }
          }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
          pCharPos->m_ExtGID = pCharPos->m_GlyphIndex;
#endif
          pCharPos->m_FontCharWidth = iCharWidth;
          if (pWSForms) {
            *pWSForms += wForm;
          }
        }
        if (bVerticalDoc) {
          iCharHeight = iCharWidth;
          iCharWidth = 1000;
        } else {
          iCharHeight = 1000;
        }
        fCharWidth = fFontSize * iCharWidth / 1000.0f;
        fCharHeight = fFontSize * iCharHeight / 1000.0f;
        if (!bMBCSCode && bRTLPiece && dwCharType != FX_CHARTYPE_Combination) {
          if (bVerticalDoc) {
            fY -= fCharHeight;
          } else {
            fX -= fCharWidth;
          }
        }
        if (!bEmptyChar) {
          CFX_PointF ptOffset;
          FX_BOOL bAdjusted = FALSE;
          if (pAdjustPos) {
            bAdjusted = pAdjustPos(wForm, bMBCSCode, pFont, fFontSize,
                                   bVerticalChar, ptOffset);
          }
          if (!bAdjusted && bVerticalChar && (dwProps & 0x00010000) != 0) {
            CFX_Rect rtBBox;
            rtBBox.Reset();
            if (pFont->GetCharBBox(wForm, rtBBox, bMBCSCode)) {
              ptOffset.x = fFontSize * (850 - rtBBox.right()) / 1000.0f;
              ptOffset.y = fFontSize * (1000 - rtBBox.height) / 2000.0f;
            }
          }
          pCharPos->m_OriginX = fX + ptOffset.x;
          pCharPos->m_OriginY = fY - ptOffset.y;
        }
        if (!bRTLPiece && dwCharType != FX_CHARTYPE_Combination) {
          if (bVerticalDoc) {
            fY += fCharHeight;
          } else {
            fX += fCharWidth;
          }
        }
        if (!bEmptyChar) {
          pCharPos->m_bGlyphAdjust = TRUE;
          if (bVerticalDoc) {
            if (iCharRotation == 0) {
              pCharPos->m_AdjustMatrix[0] = -1;
              pCharPos->m_AdjustMatrix[1] = 0;
              pCharPos->m_AdjustMatrix[2] = 0;
              pCharPos->m_AdjustMatrix[3] = 1;
              pCharPos->m_OriginY += fAscent * iVerScale / 100.0f;
            } else if (iCharRotation == 1) {
              pCharPos->m_AdjustMatrix[0] = 0;
              pCharPos->m_AdjustMatrix[1] = -1;
              pCharPos->m_AdjustMatrix[2] = -1;
              pCharPos->m_AdjustMatrix[3] = 0;
              pCharPos->m_OriginX -=
                  fDescent + fAscent * iVerScale / 100.0f - fAscent;
            } else if (iCharRotation == 2) {
              pCharPos->m_AdjustMatrix[0] = 1;
              pCharPos->m_AdjustMatrix[1] = 0;
              pCharPos->m_AdjustMatrix[2] = 0;
              pCharPos->m_AdjustMatrix[3] = -1;
              pCharPos->m_OriginX += fCharWidth;
              pCharPos->m_OriginY += fAscent;
            } else {
              pCharPos->m_AdjustMatrix[0] = 0;
              pCharPos->m_AdjustMatrix[1] = 1;
              pCharPos->m_AdjustMatrix[2] = 1;
              pCharPos->m_AdjustMatrix[3] = 0;
              pCharPos->m_OriginX += fAscent;
              pCharPos->m_OriginY += fCharWidth;
            }
          } else {
            if (iCharRotation == 0) {
              pCharPos->m_AdjustMatrix[0] = -1;
              pCharPos->m_AdjustMatrix[1] = 0;
              pCharPos->m_AdjustMatrix[2] = 0;
              pCharPos->m_AdjustMatrix[3] = 1;
              pCharPos->m_OriginY += fAscent * iVerScale / 100.0f - fAscent;
            } else if (iCharRotation == 1) {
              pCharPos->m_AdjustMatrix[0] = 0;
              pCharPos->m_AdjustMatrix[1] = -1;
              pCharPos->m_AdjustMatrix[2] = -1;
              pCharPos->m_AdjustMatrix[3] = 0;
              pCharPos->m_OriginX -= fDescent;
              pCharPos->m_OriginY -= fAscent + fDescent;
            } else if (iCharRotation == 2) {
              pCharPos->m_AdjustMatrix[0] = 1;
              pCharPos->m_AdjustMatrix[1] = 0;
              pCharPos->m_AdjustMatrix[2] = 0;
              pCharPos->m_AdjustMatrix[3] = -1;
              pCharPos->m_OriginX += fCharWidth;
              pCharPos->m_OriginY -= fAscent;
            } else {
              pCharPos->m_AdjustMatrix[0] = 0;
              pCharPos->m_AdjustMatrix[1] = 1;
              pCharPos->m_AdjustMatrix[2] = 1;
              pCharPos->m_AdjustMatrix[3] = 0;
              pCharPos->m_OriginX += fAscent * iVerScale / 100.0f;
            }
          }
          if (iHorScale != 100 || iVerScale != 100) {
            pCharPos->m_AdjustMatrix[0] =
                pCharPos->m_AdjustMatrix[0] * iHorScale / 100.0f;
            pCharPos->m_AdjustMatrix[1] =
                pCharPos->m_AdjustMatrix[1] * iHorScale / 100.0f;
            pCharPos->m_AdjustMatrix[2] =
                pCharPos->m_AdjustMatrix[2] * iVerScale / 100.0f;
            pCharPos->m_AdjustMatrix[3] =
                pCharPos->m_AdjustMatrix[3] * iVerScale / 100.0f;
          }
          pCharPos++;
        }
      }
    }
    if (iWidth > 0) {
      wPrev = wch;
    }
  }
  return iCount;
}
int32_t CFX_RTFBreak::GetCharRects(const FX_RTFTEXTOBJ* pText,
                                   CFX_RectFArray& rtArray,
                                   FX_BOOL bCharBBox) const {
  if (!pText || pText->iLength < 1) {
    return 0;
  }
  ASSERT(pText->pStr && pText->pWidths && pText->pFont && pText->pRect);
  const FX_WCHAR* pStr = pText->pStr;
  int32_t* pWidths = pText->pWidths;
  int32_t iLength = pText->iLength;
  CFX_RectF rect(*pText->pRect);
  FX_BOOL bRTLPiece = FX_IsOdd(pText->iBidiLevel);
  FX_FLOAT fFontSize = pText->fFontSize;
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  FX_FLOAT fScale = fFontSize / 1000.0f;
  CFGAS_GEFont* pFont = pText->pFont;
  if (!pFont) {
    bCharBBox = FALSE;
  }
  CFX_Rect bbox;
  bbox.Set(0, 0, 0, 0);
  if (bCharBBox) {
    bCharBBox = pFont->GetBBox(bbox);
  }
  FX_FLOAT fLeft = std::max(0.0f, bbox.left * fScale);
  FX_FLOAT fHeight = FXSYS_fabs(bbox.height * fScale);
  rtArray.RemoveAll();
  rtArray.SetSize(iLength);
  uint32_t dwStyles = pText->dwLayoutStyles;
  FX_BOOL bVertical = (dwStyles & FX_RTFLAYOUTSTYLE_VerticalLayout) != 0;
  FX_BOOL bSingleLine = (dwStyles & FX_RTFLAYOUTSTYLE_SingleLine) != 0;
  FX_BOOL bCombText = (dwStyles & FX_TXTLAYOUTSTYLE_CombText) != 0;
  FX_WCHAR wch, wLineBreakChar = pText->wLineBreakChar;
  int32_t iCharSize;
  FX_FLOAT fCharSize, fStart;
  if (bVertical) {
    fStart = bRTLPiece ? rect.bottom() : rect.top;
  } else {
    fStart = bRTLPiece ? rect.right() : rect.left;
  }
  for (int32_t i = 0; i < iLength; i++) {
    wch = *pStr++;
    iCharSize = *pWidths++;
    fCharSize = (FX_FLOAT)iCharSize / 20000.0f;
    FX_BOOL bRet = (!bSingleLine && FX_IsCtrlCode(wch));
    if (!(wch == L'\v' || wch == L'\f' || wch == 0x2028 || wch == 0x2029 ||
          (wLineBreakChar != 0xFEFF && wch == wLineBreakChar))) {
      bRet = FALSE;
    }
    if (bRet) {
      iCharSize = iFontSize * 500;
      fCharSize = fFontSize / 2.0f;
    }
    if (bVertical) {
      rect.top = fStart;
      if (bRTLPiece) {
        rect.top -= fCharSize;
        fStart -= fCharSize;
      } else {
        fStart += fCharSize;
      }
      rect.height = fCharSize;
    } else {
      rect.left = fStart;
      if (bRTLPiece) {
        rect.left -= fCharSize;
        fStart -= fCharSize;
      } else {
        fStart += fCharSize;
      }
      rect.width = fCharSize;
    }
    if (bCharBBox && !bRet) {
      int32_t iCharWidth = 1000;
      pFont->GetCharWidth(wch, iCharWidth);
      FX_FLOAT fRTLeft = 0, fCharWidth = 0;
      if (iCharWidth > 0) {
        fCharWidth = iCharWidth * fScale;
        fRTLeft = fLeft;
        if (bCombText) {
          fRTLeft = (rect.width - fCharWidth) / 2.0f;
        }
      }
      CFX_RectF rtBBoxF;
      if (bVertical) {
        rtBBoxF.top = rect.left + fRTLeft;
        rtBBoxF.left = rect.top + (rect.height - fHeight) / 2.0f;
        rtBBoxF.height = fCharWidth;
        rtBBoxF.width = fHeight;
        rtBBoxF.left = std::max(rtBBoxF.left, 0.0f);
      } else {
        rtBBoxF.left = rect.left + fRTLeft;
        rtBBoxF.top = rect.top + (rect.height - fHeight) / 2.0f;
        rtBBoxF.width = fCharWidth;
        rtBBoxF.height = fHeight;
        rtBBoxF.top = std::max(rtBBoxF.top, 0.0f);
      }
      rtArray.SetAt(i, rtBBoxF);
      continue;
    }
    rtArray.SetAt(i, rect);
  }
  return iLength;
}

CFX_RTFPiece::CFX_RTFPiece()
    : m_dwStatus(FX_RTFBREAK_PieceBreak),
      m_iStartPos(0),
      m_iWidth(-1),
      m_iStartChar(0),
      m_iChars(0),
      m_iBidiLevel(0),
      m_iBidiPos(0),
      m_iFontSize(0),
      m_iFontHeight(0),
      m_iHorizontalScale(100),
      m_iVerticalScale(100),
      m_dwLayoutStyles(0),
      m_dwIdentity(0),
      m_pChars(nullptr),
      m_pUserData(nullptr) {}

CFX_RTFPiece::~CFX_RTFPiece() {
  Reset();
}

CFX_RTFLine::CFX_RTFLine()
    : m_LinePieces(16),
      m_iStart(0),
      m_iWidth(0),
      m_iArabicChars(0),
      m_iMBCSChars(0) {}

CFX_RTFLine::~CFX_RTFLine() {
  RemoveAll();
}

FX_RTFTEXTOBJ::FX_RTFTEXTOBJ()
    : pStr(nullptr),
      pWidths(nullptr),
      iLength(0),
      pFont(nullptr),
      fFontSize(12.0f),
      dwLayoutStyles(0),
      iCharRotation(0),
      iBidiLevel(0),
      pRect(nullptr),
      wLineBreakChar(L'\n'),
      iHorizontalScale(100),
      iVerticalScale(100) {}
