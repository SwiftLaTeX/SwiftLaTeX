// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TXTEDTENGINE_H_
#define XFA_FDE_CFDE_TXTEDTENGINE_H_

#include "xfa/fde/ifde_txtedtengine.h"

class CFDE_TxtEdtBuf;
class CFDE_TxtEdtParag;
class CFX_TxtBreak;
class IFDE_TxtEdtDoRecord;
class IFX_CharIter;

class CFDE_TxtEdtEngine {
 public:
  CFDE_TxtEdtEngine();
  ~CFDE_TxtEdtEngine();

  void SetEditParams(const FDE_TXTEDTPARAMS& params);
  FDE_TXTEDTPARAMS* GetEditParams();

  int32_t CountPages() const;
  IFDE_TxtEdtPage* GetPage(int32_t nIndex);

  void SetTextByStream(IFX_Stream* pStream);
  void SetText(const CFX_WideString& wsText);
  int32_t GetTextLength() const;
  void GetText(CFX_WideString& wsText, int32_t nStart, int32_t nCount = -1);
  void ClearText();

  int32_t GetCaretRect(CFX_RectF& rtCaret) const;
  int32_t GetCaretPos() const;
  int32_t SetCaretPos(int32_t nIndex, FX_BOOL bBefore);
  int32_t MoveCaretPos(FDE_TXTEDTMOVECARET eMoveCaret,
                       FX_BOOL bShift = FALSE,
                       FX_BOOL bCtrl = FALSE);
  void Lock();
  void Unlock();
  FX_BOOL IsLocked() const;

  int32_t Insert(int32_t nStart, const FX_WCHAR* lpText, int32_t nLength);
  int32_t Delete(int32_t nStart, FX_BOOL bBackspace = FALSE);
  int32_t DeleteRange(int32_t nStart, int32_t nCount = -1);
  int32_t Replace(int32_t nStart,
                  int32_t nLength,
                  const CFX_WideString& wsReplace);

  void SetLimit(int32_t nLimit);
  void SetAliasChar(FX_WCHAR wcAlias);

  void RemoveSelRange(int32_t nStart, int32_t nCount = -1);

  void AddSelRange(int32_t nStart, int32_t nCount = -1);
  int32_t CountSelRanges();
  int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  void ClearSelection();

  FX_BOOL Redo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL Undo(const IFDE_TxtEdtDoRecord* pRecord);

  int32_t StartLayout();
  int32_t DoLayout(IFX_Pause* pPause);
  void EndLayout();

  FX_BOOL Optimize(IFX_Pause* pPause = nullptr);
  int32_t CountParags() const;
  CFDE_TxtEdtParag* GetParag(int32_t nParagIndex) const;
  IFX_CharIter* CreateCharIter();
  CFDE_TxtEdtBuf* GetTextBuf() const;
  int32_t GetTextBufLength() const;
  CFX_TxtBreak* GetTextBreak() const;
  int32_t GetLineCount() const;
  int32_t GetPageLineCount() const;

  int32_t Line2Parag(int32_t nStartParag,
                     int32_t nStartLineofParag,
                     int32_t nLineIndex,
                     int32_t& nStartLine) const;
  FX_WCHAR GetAliasChar() const { return m_wcAliasChar; }

 private:
  friend class CFDE_TxtEdtDoRecord_Insert;
  friend class CFDE_TxtEdtDoRecord_DeleteRange;
  friend class CFDE_TxtEdtPage;

  struct FDE_TXTEDTSELRANGE {
    int32_t nStart;
    int32_t nCount;
  };

  struct FDE_TXTEDTPARAGPOS {
    int32_t nParagIndex;
    int32_t nCharIndex;
  };

  void Inner_Insert(int32_t nStart, const FX_WCHAR* lpText, int32_t nLength);
  void GetPreDeleteText(CFX_WideString& wsText,
                        int32_t nIndex,
                        int32_t nLength);
  void GetPreInsertText(CFX_WideString& wsText,
                        int32_t nIndex,
                        const FX_WCHAR* lpText,
                        int32_t nLength);
  void GetPreReplaceText(CFX_WideString& wsText,
                         int32_t nIndex,
                         int32_t nOriginLength,
                         const FX_WCHAR* lpText,
                         int32_t nLength);

  void Inner_DeleteRange(int32_t nStart, int32_t nCount = -1);
  void DeleteRange_DoRecord(int32_t nStart,
                            int32_t nCount,
                            FX_BOOL bSel = FALSE);
  void ResetEngine();
  void RebuildParagraphs();
  void RemoveAllParags();
  void RemoveAllPages();
  void UpdateParags();
  void UpdatePages();
  void UpdateTxtBreak();

  FX_BOOL ReplaceParagEnd(FX_WCHAR*& lpText,
                          int32_t& nLength,
                          FX_BOOL bPreIsCR = FALSE);
  void RecoverParagEnd(CFX_WideString& wsText);
  int32_t MovePage2Char(int32_t nIndex);
  void TextPos2ParagPos(int32_t nIndex, FDE_TXTEDTPARAGPOS& ParagPos) const;
  int32_t MoveForward(FX_BOOL& bBefore);
  int32_t MoveBackward(FX_BOOL& bBefore);
  FX_BOOL MoveUp(CFX_PointF& ptCaret);
  FX_BOOL MoveDown(CFX_PointF& ptCaret);
  FX_BOOL MoveLineStart();
  FX_BOOL MoveLineEnd();
  FX_BOOL MoveParagStart();
  FX_BOOL MoveParagEnd();
  FX_BOOL MoveHome();
  FX_BOOL MoveEnd();
  FX_BOOL IsFitArea(CFX_WideString& wsText);
  void UpdateCaretRect(int32_t nIndex, FX_BOOL bBefore = TRUE);
  void GetCaretRect(CFX_RectF& rtCaret,
                    int32_t nPageIndex,
                    int32_t nCaret,
                    FX_BOOL bBefore = TRUE);
  void UpdateCaretIndex(const CFX_PointF& ptCaret);

  FX_BOOL IsSelect();
  void DeleteSelect();

  CFDE_TxtEdtBuf* m_pTxtBuf;
  CFX_TxtBreak* m_pTextBreak;
  FDE_TXTEDTPARAMS m_Param;
  CFX_ArrayTemplate<IFDE_TxtEdtPage*> m_PagePtrArray;
  CFX_ArrayTemplate<CFDE_TxtEdtParag*> m_ParagPtrArray;
  CFX_ArrayTemplate<FDE_TXTEDTSELRANGE*> m_SelRangePtrArr;
  int32_t m_nPageLineCount;
  int32_t m_nLineCount;
  int32_t m_nAnchorPos;
  int32_t m_nLayoutPos;
  FX_FLOAT m_fCaretPosReserve;
  int32_t m_nCaret;
  FX_BOOL m_bBefore;
  int32_t m_nCaretPage;
  CFX_RectF m_rtCaret;
  uint32_t m_dwFindFlags;
  FX_BOOL m_bLock;
  int32_t m_nLimit;
  FX_WCHAR m_wcAliasChar;
  int32_t m_nFirstLineEnd;
  FX_BOOL m_bAutoLineEnd;
  FX_WCHAR m_wLineEnd;
  FDE_TXTEDT_TEXTCHANGE_INFO m_ChangeInfo;
};

#endif  // XFA_FDE_CFDE_TXTEDTENGINE_H_
