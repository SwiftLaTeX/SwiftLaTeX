// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LAYOUT_FGAS_RTFBREAK_H_
#define XFA_FGAS_LAYOUT_FGAS_RTFBREAK_H_

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_ucd.h"
#include "xfa/fgas/crt/fgas_memory.h"
#include "xfa/fgas/crt/fgas_utils.h"
#include "xfa/fgas/layout/fgas_textbreak.h"
#include "xfa/fgas/layout/fgas_unicode.h"

class CFGAS_GEFont;

#define FX_RTFBREAKPOLICY_None 0x00
#define FX_RTFBREAKPOLICY_SpaceBreak 0x01
#define FX_RTFBREAKPOLICY_NumberBreak 0x02
#define FX_RTFBREAKPOLICY_InfixBreak 0x04
#define FX_RTFBREAKPOLICY_TabBreak 0x08
#define FX_RTFBREAKPOLICY_OrphanPositionedTab 0x10
#define FX_RTFBREAK_None 0x00
#define FX_RTFBREAK_PieceBreak 0x01
#define FX_RTFBREAK_LineBreak 0x02
#define FX_RTFBREAK_ParagraphBreak 0x03
#define FX_RTFBREAK_PageBreak 0x04
#define FX_RTFLAYOUTSTYLE_Pagination 0x01
#define FX_RTFLAYOUTSTYLE_VerticalLayout 0x02
#define FX_RTFLAYOUTSTYLE_VerticalChars 0x04
#define FX_RTFLAYOUTSTYLE_LineDirection 0x08
#define FX_RTFLAYOUTSTYLE_ExpandTab 0x10
#define FX_RTFLAYOUTSTYLE_ArabicNumber 0x20
#define FX_RTFLAYOUTSTYLE_SingleLine 0x40
#define FX_RTFLAYOUTSTYLE_MBCSCode 0x80
#define FX_RTFCHARSTYLE_Alignment 0x000F
#define FX_RTFCHARSTYLE_ArabicNumber 0x0010
#define FX_RTFCHARSTYLE_ArabicShadda 0x0020
#define FX_RTFCHARSTYLE_OddBidiLevel 0x0040
#define FX_RTFCHARSTYLE_RTLReadingOrder 0x0080
#define FX_RTFCHARSTYLE_ArabicContext 0x0300
#define FX_RTFCHARSTYLE_ArabicIndic 0x0400
#define FX_RTFCHARSTYLE_ArabicComma 0x0800
#define FX_RTFLINEALIGNMENT_Left 0
#define FX_RTFLINEALIGNMENT_Center 1
#define FX_RTFLINEALIGNMENT_Right 2
#define FX_RTFLINEALIGNMENT_Justified (1 << 2)
#define FX_RTFLINEALIGNMENT_Distributed (2 << 2)
#define FX_RTFLINEALIGNMENT_JustifiedLeft \
  (FX_RTFLINEALIGNMENT_Left | FX_RTFLINEALIGNMENT_Justified)
#define FX_RTFLINEALIGNMENT_JustifiedCenter \
  (FX_RTFLINEALIGNMENT_Center | FX_RTFLINEALIGNMENT_Justified)
#define FX_RTFLINEALIGNMENT_JustifiedRight \
  (FX_RTFLINEALIGNMENT_Right | FX_RTFLINEALIGNMENT_Justified)
#define FX_RTFLINEALIGNMENT_DistributedLeft \
  (FX_RTFLINEALIGNMENT_Left | FX_RTFLINEALIGNMENT_Distributed)
#define FX_RTFLINEALIGNMENT_DistributedCenter \
  (FX_RTFLINEALIGNMENT_Center | FX_RTFLINEALIGNMENT_Distributed)
#define FX_RTFLINEALIGNMENT_DistributedRight \
  (FX_RTFLINEALIGNMENT_Right | FX_RTFLINEALIGNMENT_Distributed)
#define FX_RTFLINEALIGNMENT_LowerMask 0x03
#define FX_RTFLINEALIGNMENT_HigherMask 0x0C

struct FX_RTFTEXTOBJ {
  FX_RTFTEXTOBJ();

  const FX_WCHAR* pStr;
  int32_t* pWidths;
  int32_t iLength;
  CFGAS_GEFont* pFont;
  FX_FLOAT fFontSize;
  uint32_t dwLayoutStyles;
  int32_t iCharRotation;
  int32_t iBidiLevel;
  const CFX_RectF* pRect;
  FX_WCHAR wLineBreakChar;
  int32_t iHorizontalScale;
  int32_t iVerticalScale;
};

class CFX_RTFPiece : public CFX_Target {
 public:
  CFX_RTFPiece();
  ~CFX_RTFPiece() override;

  void AppendChar(const CFX_RTFChar& tc) {
    ASSERT(m_pChars);
    m_pChars->Add(tc);
    if (m_iWidth < 0) {
      m_iWidth = tc.m_iCharWidth;
    } else {
      m_iWidth += tc.m_iCharWidth;
    }
    m_iChars++;
  }
  int32_t GetEndPos() const {
    return m_iWidth < 0 ? m_iStartPos : m_iStartPos + m_iWidth;
  }
  int32_t GetLength() const { return m_iChars; }
  int32_t GetEndChar() const { return m_iStartChar + m_iChars; }
  CFX_RTFChar& GetChar(int32_t index) {
    ASSERT(index > -1 && index < m_iChars && m_pChars);
    return *m_pChars->GetDataPtr(m_iStartChar + index);
  }
  CFX_RTFChar* GetCharPtr(int32_t index) const {
    ASSERT(index > -1 && index < m_iChars && m_pChars);
    return m_pChars->GetDataPtr(m_iStartChar + index);
  }
  void GetString(FX_WCHAR* pText) const {
    ASSERT(pText);
    int32_t iEndChar = m_iStartChar + m_iChars;
    CFX_RTFChar* pChar;
    for (int32_t i = m_iStartChar; i < iEndChar; i++) {
      pChar = m_pChars->GetDataPtr(i);
      *pText++ = (FX_WCHAR)pChar->m_wCharCode;
    }
  }
  void GetString(CFX_WideString& wsText) const {
    FX_WCHAR* pText = wsText.GetBuffer(m_iChars);
    GetString(pText);
    wsText.ReleaseBuffer(m_iChars);
  }
  void GetWidths(int32_t* pWidths) const {
    ASSERT(pWidths);
    int32_t iEndChar = m_iStartChar + m_iChars;
    CFX_RTFChar* pChar;
    for (int32_t i = m_iStartChar; i < iEndChar; i++) {
      pChar = m_pChars->GetDataPtr(i);
      *pWidths++ = pChar->m_iCharWidth;
    }
  }
  void Reset() {
    m_dwStatus = FX_RTFBREAK_PieceBreak;
    if (m_iWidth > -1) {
      m_iStartPos += m_iWidth;
    }
    m_iWidth = -1;
    m_iStartChar += m_iChars;
    m_iChars = 0;
    m_iBidiLevel = 0;
    m_iBidiPos = 0;
    m_iHorizontalScale = 100;
    m_iVerticalScale = 100;
  }

  uint32_t m_dwStatus;
  int32_t m_iStartPos;
  int32_t m_iWidth;
  int32_t m_iStartChar;
  int32_t m_iChars;
  int32_t m_iBidiLevel;
  int32_t m_iBidiPos;
  int32_t m_iFontSize;
  int32_t m_iFontHeight;
  int32_t m_iHorizontalScale;
  int32_t m_iVerticalScale;
  uint32_t m_dwLayoutStyles;
  uint32_t m_dwIdentity;
  CFX_RTFCharArray* m_pChars;
  IFX_Retainable* m_pUserData;
};

typedef CFX_BaseArrayTemplate<CFX_RTFPiece> CFX_RTFPieceArray;

class CFX_RTFLine {
 public:
  CFX_RTFLine();
  ~CFX_RTFLine();

  int32_t CountChars() const { return m_LineChars.GetSize(); }
  CFX_RTFChar& GetChar(int32_t index) {
    ASSERT(index > -1 && index < m_LineChars.GetSize());
    return *m_LineChars.GetDataPtr(index);
  }
  CFX_RTFChar* GetCharPtr(int32_t index) {
    ASSERT(index > -1 && index < m_LineChars.GetSize());
    return m_LineChars.GetDataPtr(index);
  }
  int32_t CountPieces() const { return m_LinePieces.GetSize(); }
  CFX_RTFPiece& GetPiece(int32_t index) const {
    ASSERT(index > -1 && index < m_LinePieces.GetSize());
    return m_LinePieces.GetAt(index);
  }
  CFX_RTFPiece* GetPiecePtr(int32_t index) const {
    ASSERT(index > -1 && index < m_LinePieces.GetSize());
    return m_LinePieces.GetPtrAt(index);
  }
  int32_t GetLineEnd() const { return m_iStart + m_iWidth; }
  void RemoveAll(FX_BOOL bLeaveMemory = FALSE) {
    CFX_RTFChar* pChar;
    int32_t iCount = m_LineChars.GetSize();
    for (int32_t i = 0; i < iCount; i++) {
      pChar = m_LineChars.GetDataPtr(i);
      if (pChar->m_pUserData)
        pChar->m_pUserData->Release();
    }
    m_LineChars.RemoveAll();
    m_LinePieces.RemoveAll(bLeaveMemory);
    m_iWidth = 0;
    m_iArabicChars = 0;
    m_iMBCSChars = 0;
  }

  CFX_RTFCharArray m_LineChars;
  CFX_RTFPieceArray m_LinePieces;
  int32_t m_iStart;
  int32_t m_iWidth;
  int32_t m_iArabicChars;
  int32_t m_iMBCSChars;
};

class CFX_RTFBreak {
 public:
  explicit CFX_RTFBreak(uint32_t dwPolicies);
  ~CFX_RTFBreak();

  void SetLineBoundary(FX_FLOAT fLineStart, FX_FLOAT fLineEnd);
  void SetLineStartPos(FX_FLOAT fLinePos);
  uint32_t GetLayoutStyles() const { return m_dwLayoutStyles; }
  void SetLayoutStyles(uint32_t dwLayoutStyles);
  void SetFont(CFGAS_GEFont* pFont);
  void SetFontSize(FX_FLOAT fFontSize);
  void SetTabWidth(FX_FLOAT fTabWidth);
  void AddPositionedTab(FX_FLOAT fTabPos);
  void SetPositionedTabs(const CFX_FloatArray& tabs);
  void ClearPositionedTabs();
  void SetDefaultChar(FX_WCHAR wch);
  void SetLineBreakChar(FX_WCHAR wch);
  void SetLineBreakTolerance(FX_FLOAT fTolerance);
  void SetHorizontalScale(int32_t iScale);
  void SetVerticalScale(int32_t iScale);
  void SetCharRotation(int32_t iCharRotation);
  void SetCharSpace(FX_FLOAT fCharSpace);
  void SetWordSpace(FX_BOOL bDefault, FX_FLOAT fWordSpace);
  void SetReadingOrder(FX_BOOL bRTL = FALSE);
  void SetAlignment(int32_t iAlignment = FX_RTFLINEALIGNMENT_Left);
  void SetUserData(IFX_Retainable* pUserData);
  uint32_t AppendChar(FX_WCHAR wch);
  uint32_t EndBreak(uint32_t dwStatus = FX_RTFBREAK_PieceBreak);
  int32_t CountBreakPieces() const;
  const CFX_RTFPiece* GetBreakPiece(int32_t index) const;
  void GetLineRect(CFX_RectF& rect) const;
  void ClearBreakPieces();
  void Reset();
  int32_t GetDisplayPos(const FX_RTFTEXTOBJ* pText,
                        FXTEXT_CHARPOS* pCharPos,
                        FX_BOOL bCharCode = FALSE,
                        CFX_WideString* pWSForms = nullptr,
                        FX_AdjustCharDisplayPos pAdjustPos = nullptr) const;
  int32_t GetCharRects(const FX_RTFTEXTOBJ* pText,
                       CFX_RectFArray& rtArray,
                       FX_BOOL bCharBBox = FALSE) const;
  uint32_t AppendChar_CharCode(FX_WCHAR wch);
  uint32_t AppendChar_Combination(CFX_RTFChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Tab(CFX_RTFChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Control(CFX_RTFChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Arabic(CFX_RTFChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Others(CFX_RTFChar* pCurChar, int32_t iRotation);

 protected:
  int32_t GetLineRotation(uint32_t dwStyles) const;
  void SetBreakStatus();
  CFX_RTFChar* GetLastChar(int32_t index) const;
  CFX_RTFLine* GetRTFLine(FX_BOOL bReady) const;
  CFX_RTFPieceArray* GetRTFPieces(FX_BOOL bReady) const;
  FX_CHARTYPE GetUnifiedCharType(FX_CHARTYPE chartype) const;
  int32_t GetLastPositionedTab() const;
  FX_BOOL GetPositionedTab(int32_t& iTabPos) const;

  int32_t GetBreakPos(CFX_RTFCharArray& tca,
                      int32_t& iEndPos,
                      FX_BOOL bAllChars = FALSE,
                      FX_BOOL bOnlyBrk = FALSE);
  void SplitTextLine(CFX_RTFLine* pCurLine,
                     CFX_RTFLine* pNextLine,
                     FX_BOOL bAllChars = FALSE);
  FX_BOOL EndBreak_SplitLine(CFX_RTFLine* pNextLine,
                             FX_BOOL bAllChars,
                             uint32_t dwStatus);
  void EndBreak_BidiLine(CFX_TPOArray& tpos, uint32_t dwStatus);
  void EndBreak_Alignment(CFX_TPOArray& tpos,
                          FX_BOOL bAllChars,
                          uint32_t dwStatus);

  uint32_t m_dwPolicies;
  int32_t m_iBoundaryStart;
  int32_t m_iBoundaryEnd;
  uint32_t m_dwLayoutStyles;
  FX_BOOL m_bPagination;
  FX_BOOL m_bVertical;
  FX_BOOL m_bSingleLine;
  FX_BOOL m_bCharCode;
  CFGAS_GEFont* m_pFont;
  int32_t m_iFontHeight;
  int32_t m_iFontSize;
  int32_t m_iTabWidth;
  CFX_Int32Array m_PositionedTabs;
  FX_BOOL m_bOrphanLine;
  FX_WCHAR m_wDefChar;
  int32_t m_iDefChar;
  FX_WCHAR m_wLineBreakChar;
  int32_t m_iHorizontalScale;
  int32_t m_iVerticalScale;
  int32_t m_iLineRotation;
  int32_t m_iCharRotation;
  int32_t m_iRotation;
  int32_t m_iCharSpace;
  FX_BOOL m_bWordSpace;
  int32_t m_iWordSpace;
  FX_BOOL m_bRTL;
  int32_t m_iAlignment;
  IFX_Retainable* m_pUserData;
  FX_CHARTYPE m_eCharType;
  uint32_t m_dwIdentity;
  CFX_RTFLine m_RTFLine1;
  CFX_RTFLine m_RTFLine2;
  CFX_RTFLine* m_pCurLine;
  int32_t m_iReady;
  int32_t m_iTolerance;
};

#endif  // XFA_FGAS_LAYOUT_FGAS_RTFBREAK_H_
