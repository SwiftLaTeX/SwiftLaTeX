// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LAYOUT_FGAS_TEXTBREAK_H_
#define XFA_FGAS_LAYOUT_FGAS_TEXTBREAK_H_

#include "core/fxcrt/include/fx_ucd.h"
#include "core/fxge/include/fx_ge.h"
#include "xfa/fgas/crt/fgas_utils.h"
#include "xfa/fgas/layout/fgas_unicode.h"

class CFX_Char;
class CFGAS_GEFont;
class CFX_TxtChar;
class CFX_TxtPiece;
class IFX_TxtAccess;

#define FX_TXTBREAKPOLICY_None 0x00
#define FX_TXTBREAKPOLICY_Pagination 0x01
#define FX_TXTBREAKPOLICY_SpaceBreak 0x02
#define FX_TXTBREAKPOLICY_NumberBreak 0x04
#define FX_TXTBREAK_None 0x00
#define FX_TXTBREAK_PieceBreak 0x01
#define FX_TXTBREAK_LineBreak 0x02
#define FX_TXTBREAK_ParagraphBreak 0x03
#define FX_TXTBREAK_PageBreak 0x04
#define FX_TXTBREAK_ControlChar 0x10
#define FX_TXTBREAK_BreakChar 0x20
#define FX_TXTBREAK_UnknownChar 0x40
#define FX_TXTBREAK_RemoveChar 0x80
#define FX_TXTLAYOUTSTYLE_MutipleFormat 0x0001
#define FX_TXTLAYOUTSTYLE_VerticalLayout 0x0002
#define FX_TXTLAYOUTSTYLE_VerticalChars 0x0004
#define FX_TXTLAYOUTSTYLE_ReverseLine 0x0008
#define FX_TXTLAYOUTSTYLE_ArabicContext 0x0010
#define FX_TXTLAYOUTSTYLE_ArabicShapes 0x0020
#define FX_TXTLAYOUTSTYLE_RTLReadingOrder 0x0040
#define FX_TXTLAYOUTSTYLE_ExpandTab 0x0100
#define FX_TXTLAYOUTSTYLE_SingleLine 0x0200
#define FX_TXTLAYOUTSTYLE_CombText 0x0400
#define FX_TXTCHARSTYLE_Alignment 0x000F
#define FX_TXTCHARSTYLE_ArabicNumber 0x0010
#define FX_TXTCHARSTYLE_ArabicShadda 0x0020
#define FX_TXTCHARSTYLE_OddBidiLevel 0x0040
#define FX_TXTCHARSTYLE_RTLReadingOrder 0x0080
#define FX_TXTCHARSTYLE_ArabicContext 0x0300
#define FX_TXTCHARSTYLE_ArabicIndic 0x0400
#define FX_TXTCHARSTYLE_ArabicComma 0x0800
#define FX_TXTLINEALIGNMENT_Left 0
#define FX_TXTLINEALIGNMENT_Center 1
#define FX_TXTLINEALIGNMENT_Right 2
#define FX_TXTLINEALIGNMENT_Justified (1 << 2)
#define FX_TXTLINEALIGNMENT_Distributed (2 << 2)
#define FX_TXTLINEALIGNMENT_JustifiedLeft \
  (FX_TXTLINEALIGNMENT_Left | FX_TXTLINEALIGNMENT_Justified)
#define FX_TXTLINEALIGNMENT_JustifiedCenter \
  (FX_TXTLINEALIGNMENT_Center | FX_TXTLINEALIGNMENT_Justified)
#define FX_TXTLINEALIGNMENT_JustifiedRight \
  (FX_TXTLINEALIGNMENT_Right | FX_TXTLINEALIGNMENT_Justified)
#define FX_TXTLINEALIGNMENT_DistributedLeft \
  (FX_TXTLINEALIGNMENT_Left | FX_TXTLINEALIGNMENT_Distributed)
#define FX_TXTLINEALIGNMENT_DistributedCenter \
  (FX_TXTLINEALIGNMENT_Center | FX_TXTLINEALIGNMENT_Distributed)
#define FX_TXTLINEALIGNMENT_DistributedRight \
  (FX_TXTLINEALIGNMENT_Right | FX_TXTLINEALIGNMENT_Distributed)
#define FX_TXTLINEALIGNMENT_LowerMask 0x03
#define FX_TXTLINEALIGNMENT_HigherMask 0x0C
#define FX_TXTBREAK_MinimumTabWidth 160000

struct FDE_TEXTEDITPIECE;

class IFX_TxtAccess {
 public:
  virtual ~IFX_TxtAccess() {}
  virtual FX_WCHAR GetChar(const FDE_TEXTEDITPIECE* pIdentity,
                           int32_t index) const = 0;
  virtual int32_t GetWidth(const FDE_TEXTEDITPIECE* pIdentity,
                           int32_t index) const = 0;
};

struct FX_TXTRUN {
  FX_TXTRUN();
  FX_TXTRUN(const FX_TXTRUN& other);
  ~FX_TXTRUN();

  IFX_TxtAccess* pAccess;
  const FDE_TEXTEDITPIECE* pIdentity;
  CFX_WideString wsStr;
  int32_t* pWidths;
  int32_t iLength;
  CFGAS_GEFont* pFont;
  FX_FLOAT fFontSize;
  uint32_t dwStyles;
  int32_t iHorizontalScale;
  int32_t iVerticalScale;
  int32_t iCharRotation;
  uint32_t dwCharStyles;
  const CFX_RectF* pRect;
  FX_WCHAR wLineBreakChar;
  FX_BOOL bSkipSpace;
};

class CFX_TxtPiece : public CFX_Target {
 public:
  CFX_TxtPiece();

  int32_t GetEndPos() const {
    return m_iWidth < 0 ? m_iStartPos : m_iStartPos + m_iWidth;
  }
  int32_t GetLength() const { return m_iChars; }
  int32_t GetEndChar() const { return m_iStartChar + m_iChars; }
  CFX_TxtChar* GetCharPtr(int32_t index) const {
    ASSERT(index > -1 && index < m_iChars && m_pChars);
    return m_pChars->GetDataPtr(m_iStartChar + index);
  }
  void GetString(FX_WCHAR* pText) const {
    ASSERT(pText);
    int32_t iEndChar = m_iStartChar + m_iChars;
    CFX_Char* pChar;
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
    CFX_Char* pChar;
    for (int32_t i = m_iStartChar; i < iEndChar; i++) {
      pChar = m_pChars->GetDataPtr(i);
      *pWidths++ = pChar->m_iCharWidth;
    }
  }

  uint32_t m_dwStatus;
  int32_t m_iStartPos;
  int32_t m_iWidth;
  int32_t m_iStartChar;
  int32_t m_iChars;
  int32_t m_iBidiLevel;
  int32_t m_iBidiPos;
  int32_t m_iHorizontalScale;
  int32_t m_iVerticalScale;
  uint32_t m_dwCharStyles;
  CFX_TxtCharArray* m_pChars;
  void* m_pUserData;
};

typedef CFX_BaseArrayTemplate<CFX_TxtPiece> CFX_TxtPieceArray;

class CFX_TxtLine {
 public:
  CFX_TxtLine(int32_t iBlockSize);
  ~CFX_TxtLine();

  int32_t CountChars() const { return m_pLineChars->GetSize(); }
  CFX_TxtChar* GetCharPtr(int32_t index) const {
    ASSERT(index > -1 && index < m_pLineChars->GetSize());
    return m_pLineChars->GetDataPtr(index);
  }
  int32_t CountPieces() const { return m_pLinePieces->GetSize(); }
  CFX_TxtPiece* GetPiecePtr(int32_t index) const {
    ASSERT(index > -1 && index < m_pLinePieces->GetSize());
    return m_pLinePieces->GetPtrAt(index);
  }
  void GetString(CFX_WideString& wsStr) const {
    int32_t iCount = m_pLineChars->GetSize();
    FX_WCHAR* pBuf = wsStr.GetBuffer(iCount);
    CFX_Char* pChar;
    for (int32_t i = 0; i < iCount; i++) {
      pChar = m_pLineChars->GetDataPtr(i);
      *pBuf++ = (FX_WCHAR)pChar->m_wCharCode;
    }
    wsStr.ReleaseBuffer(iCount);
  }
  void RemoveAll(FX_BOOL bLeaveMemory = FALSE) {
    m_pLineChars->RemoveAll();
    m_pLinePieces->RemoveAll(bLeaveMemory);
    m_iWidth = 0;
    m_iArabicChars = 0;
  }

  CFX_TxtCharArray* m_pLineChars;
  CFX_TxtPieceArray* m_pLinePieces;
  int32_t m_iStart;
  int32_t m_iWidth;
  int32_t m_iArabicChars;
};

class CFX_TxtBreak {
 public:
  CFX_TxtBreak(uint32_t dwPolicies);
  ~CFX_TxtBreak();

  void SetLineWidth(FX_FLOAT fLineWidth);
  void SetLinePos(FX_FLOAT fLinePos);
  uint32_t GetLayoutStyles() const { return m_dwLayoutStyles; }
  void SetLayoutStyles(uint32_t dwLayoutStyles);
  void SetFont(CFGAS_GEFont* pFont);
  void SetFontSize(FX_FLOAT fFontSize);
  void SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant);
  void SetDefaultChar(FX_WCHAR wch);
  void SetParagraphBreakChar(FX_WCHAR wch);
  void SetLineBreakTolerance(FX_FLOAT fTolerance);
  void SetHorizontalScale(int32_t iScale);
  void SetVerticalScale(int32_t iScale);
  void SetCharRotation(int32_t iCharRotation);
  void SetCharSpace(FX_FLOAT fCharSpace);
  void SetAlignment(int32_t iAlignment);
  uint32_t GetContextCharStyles() const;
  void SetContextCharStyles(uint32_t dwCharStyles);
  void SetCombWidth(FX_FLOAT fCombWidth);
  void SetUserData(void* pUserData);
  uint32_t AppendChar(FX_WCHAR wch);
  uint32_t EndBreak(uint32_t dwStatus = FX_TXTBREAK_PieceBreak);
  int32_t CountBreakChars() const;
  int32_t CountBreakPieces() const;
  const CFX_TxtPiece* GetBreakPiece(int32_t index) const;
  void ClearBreakPieces();
  void Reset();
  int32_t GetDisplayPos(const FX_TXTRUN* pTxtRun,
                        FXTEXT_CHARPOS* pCharPos,
                        FX_BOOL bCharCode = FALSE,
                        CFX_WideString* pWSForms = nullptr,
                        FX_AdjustCharDisplayPos pAdjustPos = nullptr) const;
  int32_t GetCharRects(const FX_TXTRUN* pTxtRun,
                       CFX_RectFArray& rtArray,
                       FX_BOOL bCharBBox = FALSE) const;
  void AppendChar_PageLoad(CFX_TxtChar* pCurChar, uint32_t dwProps);
  uint32_t AppendChar_Combination(CFX_TxtChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Tab(CFX_TxtChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Control(CFX_TxtChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Arabic(CFX_TxtChar* pCurChar, int32_t iRotation);
  uint32_t AppendChar_Others(CFX_TxtChar* pCurChar, int32_t iRotation);

 private:
  void SetBreakStatus();
  int32_t GetLineRotation(uint32_t dwStyles) const;
  CFX_TxtChar* GetLastChar(int32_t index, FX_BOOL bOmitChar = TRUE) const;
  CFX_TxtLine* GetTxtLine(FX_BOOL bReady) const;
  CFX_TxtPieceArray* GetTxtPieces(FX_BOOL bReady) const;
  FX_CHARTYPE GetUnifiedCharType(FX_CHARTYPE dwType) const;
  void ResetArabicContext();
  void ResetContextCharStyles();
  void EndBreak_UpdateArabicShapes();
  FX_BOOL EndBreak_SplitLine(CFX_TxtLine* pNextLine,
                             FX_BOOL bAllChars,
                             uint32_t dwStatus);
  void EndBreak_BidiLine(CFX_TPOArray& tpos, uint32_t dwStatus);
  void EndBreak_Alignment(CFX_TPOArray& tpos,
                          FX_BOOL bAllChars,
                          uint32_t dwStatus);
  int32_t GetBreakPos(CFX_TxtCharArray& ca,
                      int32_t& iEndPos,
                      FX_BOOL bAllChars = FALSE,
                      FX_BOOL bOnlyBrk = FALSE);
  void SplitTextLine(CFX_TxtLine* pCurLine,
                     CFX_TxtLine* pNextLine,
                     FX_BOOL bAllChars = FALSE);

  uint32_t m_dwPolicies;
  FX_BOOL m_bPagination;
  int32_t m_iLineWidth;
  uint32_t m_dwLayoutStyles;
  FX_BOOL m_bVertical;
  FX_BOOL m_bArabicContext;
  FX_BOOL m_bArabicShapes;
  FX_BOOL m_bRTL;
  FX_BOOL m_bSingleLine;
  FX_BOOL m_bCombText;
  int32_t m_iArabicContext;
  int32_t m_iCurArabicContext;
  CFGAS_GEFont* m_pFont;
  int32_t m_iFontSize;
  FX_BOOL m_bEquidistant;
  int32_t m_iTabWidth;
  FX_WCHAR m_wDefChar;
  FX_WCHAR m_wParagBreakChar;
  int32_t m_iDefChar;
  int32_t m_iLineRotation;
  int32_t m_iCharRotation;
  int32_t m_iRotation;
  int32_t m_iAlignment;
  uint32_t m_dwContextCharStyles;
  int32_t m_iCombWidth;
  void* m_pUserData;
  FX_CHARTYPE m_eCharType;
  FX_BOOL m_bCurRTL;
  int32_t m_iCurAlignment;
  FX_BOOL m_bArabicNumber;
  FX_BOOL m_bArabicComma;
  CFX_TxtLine* m_pTxtLine1;
  CFX_TxtLine* m_pTxtLine2;
  CFX_TxtLine* m_pCurLine;
  int32_t m_iReady;
  int32_t m_iTolerance;
  int32_t m_iHorScale;
  int32_t m_iVerScale;
  int32_t m_iCharSpace;
};

#endif  // XFA_FGAS_LAYOUT_FGAS_TEXTBREAK_H_
