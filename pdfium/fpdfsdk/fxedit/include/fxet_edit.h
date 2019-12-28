// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FXEDIT_INCLUDE_FXET_EDIT_H_
#define FPDFSDK_FXEDIT_INCLUDE_FXET_EDIT_H_

#include <memory>

#include "core/fpdfdoc/include/cpvt_secprops.h"
#include "core/fpdfdoc/include/cpvt_wordprops.h"
#include "fpdfsdk/fxedit/include/fx_edit.h"

class CPDF_PageObjectHolder;
class CPDF_TextObject;
class CPWL_Edit;
class CPWL_EditCtrl;
class CFX_Edit;
class CFX_Edit_Iterator;
class CFX_Edit_Provider;
class CFX_RenderDevice;
class CFX_SystemHandler;
class IFX_Edit_UndoItem;

#define FX_EDIT_IsFloatZero(f) (f < 0.0001 && f > -0.0001)
#define FX_EDIT_IsFloatEqual(fa, fb) FX_EDIT_IsFloatZero(fa - fb)
#define FX_EDIT_IsFloatBigger(fa, fb) (fa > fb && !FX_EDIT_IsFloatEqual(fa, fb))
#define FX_EDIT_IsFloatSmaller(fa, fb) \
  (fa < fb && !FX_EDIT_IsFloatEqual(fa, fb))

enum REFRESH_PLAN_E { RP_ANALYSE, RP_NOANALYSE, RP_OPTIONAL };

enum EDIT_PROPS_E {
  EP_LINELEADING,
  EP_LINEINDENT,
  EP_ALIGNMENT,
  EP_FONTINDEX,
  EP_FONTSIZE,
  EP_WORDCOLOR,
  EP_SCRIPTTYPE,
  EP_UNDERLINE,
  EP_CROSSOUT,
  EP_CHARSPACE,
  EP_HORZSCALE,
  EP_BOLD,
  EP_ITALIC
};

struct CFX_Edit_LineRect {
  CFX_Edit_LineRect(const CPVT_WordRange& wrLine, const CFX_FloatRect& rcLine)
      : m_wrLine(wrLine), m_rcLine(rcLine) {}

  FX_BOOL operator!=(const CFX_Edit_LineRect& linerect) const {
    return FXSYS_memcmp(this, &linerect, sizeof(CFX_Edit_LineRect)) != 0;
  }

  FX_BOOL IsSameHeight(const CFX_Edit_LineRect& linerect) const {
    return FX_EDIT_IsFloatZero(
        (m_rcLine.top - m_rcLine.bottom) -
        (linerect.m_rcLine.top - linerect.m_rcLine.bottom));
  }

  FX_BOOL IsSameTop(const CFX_Edit_LineRect& linerect) const {
    return FX_EDIT_IsFloatZero(m_rcLine.top - linerect.m_rcLine.top);
  }

  FX_BOOL IsSameLeft(const CFX_Edit_LineRect& linerect) const {
    return FX_EDIT_IsFloatZero(m_rcLine.left - linerect.m_rcLine.left);
  }

  FX_BOOL IsSameRight(const CFX_Edit_LineRect& linerect) const {
    return FX_EDIT_IsFloatZero(m_rcLine.right - linerect.m_rcLine.right);
  }

  CPVT_WordRange m_wrLine;
  CFX_FloatRect m_rcLine;
};

class CFX_Edit_LineRectArray {
 public:
  CFX_Edit_LineRectArray();
  virtual ~CFX_Edit_LineRectArray();

  void Empty();
  void RemoveAll();
  void operator=(CFX_Edit_LineRectArray& rects);
  void Add(const CPVT_WordRange& wrLine, const CFX_FloatRect& rcLine);

  int32_t GetSize() const;
  CFX_Edit_LineRect* GetAt(int32_t nIndex) const;

 private:
  CFX_ArrayTemplate<CFX_Edit_LineRect*> m_LineRects;
};

class CFX_Edit_RectArray {
 public:
  CFX_Edit_RectArray();
  virtual ~CFX_Edit_RectArray();

  void Empty();
  void Add(const CFX_FloatRect& rect);

  int32_t GetSize() const;
  CFX_FloatRect* GetAt(int32_t nIndex) const;

 private:
  CFX_ArrayTemplate<CFX_FloatRect*> m_Rects;
};

class CFX_Edit_Refresh {
 public:
  CFX_Edit_Refresh();
  virtual ~CFX_Edit_Refresh();

  void BeginRefresh();
  void Push(const CPVT_WordRange& linerange, const CFX_FloatRect& rect);
  void NoAnalyse();
  void Analyse(int32_t nAlignment);
  void AddRefresh(const CFX_FloatRect& rect);
  const CFX_Edit_RectArray* GetRefreshRects() const;
  void EndRefresh();

 private:
  CFX_Edit_LineRectArray m_NewLineRects;
  CFX_Edit_LineRectArray m_OldLineRects;
  CFX_Edit_RectArray m_RefreshRects;
};

class CFX_Edit_Select {
 public:
  CFX_Edit_Select();
  CFX_Edit_Select(const CPVT_WordPlace& begin, const CPVT_WordPlace& end);
  explicit CFX_Edit_Select(const CPVT_WordRange& range);

  void Default();
  void Set(const CPVT_WordPlace& begin, const CPVT_WordPlace& end);
  void SetBeginPos(const CPVT_WordPlace& begin);
  void SetEndPos(const CPVT_WordPlace& end);

  CPVT_WordRange ConvertToWordRange() const;
  FX_BOOL IsExist() const;
  FX_BOOL operator!=(const CPVT_WordRange& wr) const;

  CPVT_WordPlace BeginPos, EndPos;
};

class CFX_Edit_Undo {
 public:
  explicit CFX_Edit_Undo(int32_t nBufsize);
  virtual ~CFX_Edit_Undo();

  void Undo();
  void Redo();

  void AddItem(IFX_Edit_UndoItem* pItem);

  FX_BOOL CanUndo() const;
  FX_BOOL CanRedo() const;
  FX_BOOL IsModified() const;
  FX_BOOL IsWorking() const;

  void Reset();

  IFX_Edit_UndoItem* GetItem(int32_t nIndex);
  int32_t GetItemCount() { return m_UndoItemStack.GetSize(); }
  int32_t GetCurUndoPos() { return m_nCurUndoPos; }

 private:
  void SetBufSize(int32_t nSize) { m_nBufSize = nSize; }
  int32_t GetBufSize() { return m_nBufSize; }

  void RemoveHeads();
  void RemoveTails();

 private:
  CFX_ArrayTemplate<IFX_Edit_UndoItem*> m_UndoItemStack;

  int32_t m_nCurUndoPos;
  int32_t m_nBufSize;
  FX_BOOL m_bModified;
  FX_BOOL m_bVirgin;
  FX_BOOL m_bWorking;
};

class IFX_Edit_UndoItem {
 public:
  virtual ~IFX_Edit_UndoItem() {}

  virtual void Undo() = 0;
  virtual void Redo() = 0;
  virtual CFX_WideString GetUndoTitle() = 0;
};

class CFX_Edit_UndoItem : public IFX_Edit_UndoItem {
 public:
  CFX_Edit_UndoItem();
  ~CFX_Edit_UndoItem() override;

  CFX_WideString GetUndoTitle() override;

  void SetFirst(FX_BOOL bFirst);
  FX_BOOL IsFirst();
  void SetLast(FX_BOOL bLast);
  FX_BOOL IsLast();

 private:
  FX_BOOL m_bFirst;
  FX_BOOL m_bLast;
};

class CFX_Edit_GroupUndoItem : public IFX_Edit_UndoItem {
 public:
  explicit CFX_Edit_GroupUndoItem(const CFX_WideString& sTitle);
  ~CFX_Edit_GroupUndoItem() override;

  // IFX_Edit_UndoItem
  void Undo() override;
  void Redo() override;
  CFX_WideString GetUndoTitle() override;

  void AddUndoItem(CFX_Edit_UndoItem* pUndoItem);
  void UpdateItems();

 private:
  CFX_WideString m_sTitle;
  CFX_ArrayTemplate<CFX_Edit_UndoItem*> m_Items;
};

class CFXEU_InsertWord : public CFX_Edit_UndoItem {
 public:
  CFXEU_InsertWord(CFX_Edit* pEdit,
                   const CPVT_WordPlace& wpOldPlace,
                   const CPVT_WordPlace& wpNewPlace,
                   uint16_t word,
                   int32_t charset,
                   const CPVT_WordProps* pWordProps);
  ~CFXEU_InsertWord() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordPlace m_wpOld;
  CPVT_WordPlace m_wpNew;
  uint16_t m_Word;
  int32_t m_nCharset;
  CPVT_WordProps m_WordProps;
};

class CFXEU_InsertReturn : public CFX_Edit_UndoItem {
 public:
  CFXEU_InsertReturn(CFX_Edit* pEdit,
                     const CPVT_WordPlace& wpOldPlace,
                     const CPVT_WordPlace& wpNewPlace,
                     const CPVT_SecProps* pSecProps,
                     const CPVT_WordProps* pWordProps);
  ~CFXEU_InsertReturn() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordPlace m_wpOld;
  CPVT_WordPlace m_wpNew;
  CPVT_SecProps m_SecProps;
  CPVT_WordProps m_WordProps;
};

class CFXEU_Backspace : public CFX_Edit_UndoItem {
 public:
  CFXEU_Backspace(CFX_Edit* pEdit,
                  const CPVT_WordPlace& wpOldPlace,
                  const CPVT_WordPlace& wpNewPlace,
                  uint16_t word,
                  int32_t charset,
                  const CPVT_SecProps& SecProps,
                  const CPVT_WordProps& WordProps);
  ~CFXEU_Backspace() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordPlace m_wpOld;
  CPVT_WordPlace m_wpNew;
  uint16_t m_Word;
  int32_t m_nCharset;
  CPVT_SecProps m_SecProps;
  CPVT_WordProps m_WordProps;
};

class CFXEU_Delete : public CFX_Edit_UndoItem {
 public:
  CFXEU_Delete(CFX_Edit* pEdit,
               const CPVT_WordPlace& wpOldPlace,
               const CPVT_WordPlace& wpNewPlace,
               uint16_t word,
               int32_t charset,
               const CPVT_SecProps& SecProps,
               const CPVT_WordProps& WordProps,
               FX_BOOL bSecEnd);
  ~CFXEU_Delete() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordPlace m_wpOld;
  CPVT_WordPlace m_wpNew;
  uint16_t m_Word;
  int32_t m_nCharset;
  CPVT_SecProps m_SecProps;
  CPVT_WordProps m_WordProps;
  FX_BOOL m_bSecEnd;
};

class CFXEU_Clear : public CFX_Edit_UndoItem {
 public:
  CFXEU_Clear(CFX_Edit* pEdit,
              const CPVT_WordRange& wrSel,
              const CFX_WideString& swText);
  ~CFXEU_Clear() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordRange m_wrSel;
  CFX_WideString m_swText;
};

class CFXEU_InsertText : public CFX_Edit_UndoItem {
 public:
  CFXEU_InsertText(CFX_Edit* pEdit,
                   const CPVT_WordPlace& wpOldPlace,
                   const CPVT_WordPlace& wpNewPlace,
                   const CFX_WideString& swText,
                   int32_t charset,
                   const CPVT_SecProps* pSecProps,
                   const CPVT_WordProps* pWordProps);
  ~CFXEU_InsertText() override;

  // CFX_Edit_UndoItem
  void Redo() override;
  void Undo() override;

 private:
  CFX_Edit* m_pEdit;

  CPVT_WordPlace m_wpOld;
  CPVT_WordPlace m_wpNew;
  CFX_WideString m_swText;
  int32_t m_nCharset;
  CPVT_SecProps m_SecProps;
  CPVT_WordProps m_WordProps;
};

class CFX_Edit {
 public:
  static CFX_ByteString GetEditAppearanceStream(
      CFX_Edit* pEdit,
      const CFX_FloatPoint& ptOffset,
      const CPVT_WordRange* pRange = nullptr,
      FX_BOOL bContinuous = TRUE,
      uint16_t SubWord = 0);
  static CFX_ByteString GetSelectAppearanceStream(
      CFX_Edit* pEdit,
      const CFX_FloatPoint& ptOffset,
      const CPVT_WordRange* pRange = nullptr);
  static void DrawEdit(CFX_RenderDevice* pDevice,
                       CFX_Matrix* pUser2Device,
                       CFX_Edit* pEdit,
                       FX_COLORREF crTextFill,
                       FX_COLORREF crTextStroke,
                       const CFX_FloatRect& rcClip,
                       const CFX_FloatPoint& ptOffset,
                       const CPVT_WordRange* pRange,
                       CFX_SystemHandler* pSystemHandler,
                       void* pFFLData);
  static void DrawUnderline(CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            CFX_Edit* pEdit,
                            FX_COLORREF color,
                            const CFX_FloatRect& rcClip,
                            const CFX_FloatPoint& ptOffset,
                            const CPVT_WordRange* pRange);
  static void DrawRichEdit(CFX_RenderDevice* pDevice,
                           CFX_Matrix* pUser2Device,
                           CFX_Edit* pEdit,
                           const CFX_FloatRect& rcClip,
                           const CFX_FloatPoint& ptOffset,
                           const CPVT_WordRange* pRange);
  static void GeneratePageObjects(
      CPDF_PageObjectHolder* pObjectHolder,
      CFX_Edit* pEdit,
      const CFX_FloatPoint& ptOffset,
      const CPVT_WordRange* pRange,
      FX_COLORREF crText,
      CFX_ArrayTemplate<CPDF_TextObject*>& ObjArray);
  static void GenerateRichPageObjects(
      CPDF_PageObjectHolder* pObjectHolder,
      CFX_Edit* pEdit,
      const CFX_FloatPoint& ptOffset,
      const CPVT_WordRange* pRange,
      CFX_ArrayTemplate<CPDF_TextObject*>& ObjArray);
  static void GenerateUnderlineObjects(CPDF_PageObjectHolder* pObjectHolder,
                                       CFX_Edit* pEdit,
                                       const CFX_FloatPoint& ptOffset,
                                       const CPVT_WordRange* pRange,
                                       FX_COLORREF color);

  CFX_Edit();
  ~CFX_Edit();

  void SetFontMap(IPVT_FontMap* pFontMap);
  void SetNotify(CPWL_EditCtrl* pNotify);
  void SetOprNotify(CPWL_Edit* pOprNotify);

  // Returns an iterator for the contents. Should not be released.
  CFX_Edit_Iterator* GetIterator();
  CPDF_VariableText* GetVariableText();
  IPVT_FontMap* GetFontMap();
  void Initialize();

  // Set the bounding box of the text area.
  void SetPlateRect(const CFX_FloatRect& rect, FX_BOOL bPaint = TRUE);
  void SetScrollPos(const CFX_FloatPoint& point);

  // Set the horizontal text alignment. (nFormat [0:left, 1:middle, 2:right])
  void SetAlignmentH(int32_t nFormat = 0, FX_BOOL bPaint = TRUE);
  // Set the vertical text alignment. (nFormat [0:left, 1:middle, 2:right])
  void SetAlignmentV(int32_t nFormat = 0, FX_BOOL bPaint = TRUE);

  // Set the substitution character for hidden text.
  void SetPasswordChar(uint16_t wSubWord = '*', FX_BOOL bPaint = TRUE);

  // Set the maximum number of words in the text.
  void SetLimitChar(int32_t nLimitChar = 0, FX_BOOL bPaint = TRUE);
  void SetCharArray(int32_t nCharArray = 0, FX_BOOL bPaint = TRUE);
  void SetCharSpace(FX_FLOAT fCharSpace = 0.0f, FX_BOOL bPaint = TRUE);
  void SetHorzScale(int32_t nHorzScale = 100, FX_BOOL bPaint = TRUE);
  void SetLineLeading(FX_FLOAT fLineLeading, FX_BOOL bPaint = TRUE);
  void SetMultiLine(FX_BOOL bMultiLine = TRUE, FX_BOOL bPaint = TRUE);
  void SetAutoReturn(FX_BOOL bAuto = TRUE, FX_BOOL bPaint = TRUE);
  void SetAutoFontSize(FX_BOOL bAuto = TRUE, FX_BOOL bPaint = TRUE);
  void SetAutoScroll(FX_BOOL bAuto = TRUE, FX_BOOL bPaint = TRUE);
  void SetFontSize(FX_FLOAT fFontSize, FX_BOOL bPaint = TRUE);
  void SetTextOverflow(FX_BOOL bAllowed = FALSE, FX_BOOL bPaint = TRUE);
  FX_BOOL IsRichText() const;
  void SetRichText(FX_BOOL bRichText = TRUE, FX_BOOL bPaint = TRUE);
  FX_BOOL SetRichFontSize(FX_FLOAT fFontSize);
  FX_BOOL SetRichFontIndex(int32_t nFontIndex);
  FX_BOOL SetRichTextColor(FX_COLORREF dwColor);
  FX_BOOL SetRichTextScript(CPDF_VariableText::ScriptType nScriptType);
  FX_BOOL SetRichTextBold(FX_BOOL bBold = TRUE);
  FX_BOOL SetRichTextItalic(FX_BOOL bItalic = TRUE);
  FX_BOOL SetRichTextUnderline(FX_BOOL bUnderline = TRUE);
  FX_BOOL SetRichTextCrossout(FX_BOOL bCrossout = TRUE);
  FX_BOOL SetRichTextCharSpace(FX_FLOAT fCharSpace);
  FX_BOOL SetRichTextHorzScale(int32_t nHorzScale = 100);
  FX_BOOL SetRichTextLineLeading(FX_FLOAT fLineLeading);
  FX_BOOL SetRichTextLineIndent(FX_FLOAT fLineIndent);
  FX_BOOL SetRichTextAlignment(int32_t nAlignment);
  void OnMouseDown(const CFX_FloatPoint& point, FX_BOOL bShift, FX_BOOL bCtrl);
  void OnMouseMove(const CFX_FloatPoint& point, FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_UP(FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_DOWN(FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_LEFT(FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_RIGHT(FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_HOME(FX_BOOL bShift, FX_BOOL bCtrl);
  void OnVK_END(FX_BOOL bShift, FX_BOOL bCtrl);
  void SetText(const FX_WCHAR* text,
               int32_t charset = DEFAULT_CHARSET,
               const CPVT_SecProps* pSecProps = nullptr,
               const CPVT_WordProps* pWordProps = nullptr);
  FX_BOOL InsertWord(uint16_t word,
                     int32_t charset = DEFAULT_CHARSET,
                     const CPVT_WordProps* pWordProps = nullptr);
  FX_BOOL InsertReturn(const CPVT_SecProps* pSecProps = nullptr,
                       const CPVT_WordProps* pWordProps = nullptr);
  FX_BOOL Backspace();
  FX_BOOL Delete();
  FX_BOOL Clear();
  FX_BOOL InsertText(const FX_WCHAR* text,
                     int32_t charset = DEFAULT_CHARSET,
                     const CPVT_SecProps* pSecProps = nullptr,
                     const CPVT_WordProps* pWordProps = nullptr);
  FX_BOOL Redo();
  FX_BOOL Undo();
  int32_t WordPlaceToWordIndex(const CPVT_WordPlace& place) const;
  CPVT_WordPlace WordIndexToWordPlace(int32_t index) const;
  CPVT_WordPlace GetLineBeginPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetLineEndPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetSectionBeginPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace GetSectionEndPlace(const CPVT_WordPlace& place) const;
  CPVT_WordPlace SearchWordPlace(const CFX_FloatPoint& point) const;
  int32_t GetCaret() const;
  CPVT_WordPlace GetCaretWordPlace() const;
  CFX_WideString GetSelText() const;
  CFX_WideString GetText() const;
  FX_FLOAT GetFontSize() const;
  uint16_t GetPasswordChar() const;
  CFX_FloatPoint GetScrollPos() const;
  int32_t GetCharArray() const;
  CFX_FloatRect GetPlateRect() const;
  CFX_FloatRect GetContentRect() const;
  CFX_WideString GetRangeText(const CPVT_WordRange& range) const;
  int32_t GetHorzScale() const;
  FX_FLOAT GetCharSpace() const;
  int32_t GetTotalWords() const;
  void SetSel(int32_t nStartChar, int32_t nEndChar);
  void GetSel(int32_t& nStartChar, int32_t& nEndChar) const;
  void SelectAll();
  void SelectNone();
  FX_BOOL IsSelected() const;
  void Paint();
  void EnableNotify(FX_BOOL bNotify);
  void EnableRefresh(FX_BOOL bRefresh);
  void RefreshWordRange(const CPVT_WordRange& wr);
  void SetCaret(int32_t nPos);
  CPVT_WordRange GetWholeWordRange() const;
  CPVT_WordRange GetSelectWordRange() const;
  void EnableUndo(FX_BOOL bUndo);
  void EnableOprNotify(FX_BOOL bNotify);
  FX_BOOL IsTextFull() const;
  FX_BOOL IsTextOverflow() const;
  FX_BOOL CanUndo() const;
  FX_BOOL CanRedo() const;
  FX_BOOL IsModified() const;
  CPVT_WordRange GetVisibleWordRange() const;
  void AddUndoItem(IFX_Edit_UndoItem* pUndoItem);

  FX_BOOL Empty();

  CPVT_WordPlace DoInsertText(const CPVT_WordPlace& place,
                              const FX_WCHAR* text,
                              int32_t charset,
                              const CPVT_SecProps* pSecProps,
                              const CPVT_WordProps* pWordProps);
  int32_t GetCharSetFromUnicode(uint16_t word, int32_t nOldCharset);

  int32_t GetTotalLines() const;

 private:
  friend class CFX_Edit_Iterator;
  friend class CFXEU_InsertWord;
  friend class CFXEU_InsertReturn;
  friend class CFXEU_Backspace;
  friend class CFXEU_Delete;
  friend class CFXEU_Clear;
  friend class CFXEU_InsertText;

  void SetSel(const CPVT_WordPlace& begin, const CPVT_WordPlace& end);

  void RearrangeAll();
  void RearrangePart(const CPVT_WordRange& range);
  void ScrollToCaret();
  void SetScrollInfo();
  void SetScrollPosX(FX_FLOAT fx);
  void SetScrollPosY(FX_FLOAT fy);
  void SetScrollLimit();
  void SetContentChanged();

  void SetText(const FX_WCHAR* text,
               int32_t charset,
               const CPVT_SecProps* pSecProps,
               const CPVT_WordProps* pWordProps,
               FX_BOOL bAddUndo,
               FX_BOOL bPaint);
  FX_BOOL InsertWord(uint16_t word,
                     int32_t charset,
                     const CPVT_WordProps* pWordProps,
                     FX_BOOL bAddUndo,
                     FX_BOOL bPaint);
  FX_BOOL InsertReturn(const CPVT_SecProps* pSecProps,
                       const CPVT_WordProps* pWordProps,
                       FX_BOOL bAddUndo,
                       FX_BOOL bPaint);
  FX_BOOL Backspace(FX_BOOL bAddUndo, FX_BOOL bPaint);
  FX_BOOL Delete(FX_BOOL bAddUndo, FX_BOOL bPaint);
  FX_BOOL Clear(FX_BOOL bAddUndo, FX_BOOL bPaint);
  FX_BOOL InsertText(const FX_WCHAR* text,
                     int32_t charset,
                     const CPVT_SecProps* pSecProps,
                     const CPVT_WordProps* pWordProps,
                     FX_BOOL bAddUndo,
                     FX_BOOL bPaint);
  void PaintInsertText(const CPVT_WordPlace& wpOld,
                       const CPVT_WordPlace& wpNew);

  inline CFX_FloatPoint VTToEdit(const CFX_FloatPoint& point) const;
  inline CFX_FloatPoint EditToVT(const CFX_FloatPoint& point) const;
  inline CFX_FloatRect VTToEdit(const CFX_FloatRect& rect) const;
  inline CFX_FloatRect EditToVT(const CFX_FloatRect& rect) const;

  void Refresh(REFRESH_PLAN_E ePlan,
               const CPVT_WordRange* pRange1 = nullptr,
               const CPVT_WordRange* pRange2 = nullptr);
  void RefreshPushLineRects(const CPVT_WordRange& wr);
  void RefreshPushRandomRects(const CPVT_WordRange& wr);

  void SetCaret(const CPVT_WordPlace& place);
  void SetCaretInfo();
  void SetCaretOrigin();

  CPVT_WordRange GetLatinWordsRange(const CPVT_WordPlace& place) const;
  CPVT_WordRange CombineWordRange(const CPVT_WordRange& wr1,
                                  const CPVT_WordRange& wr2);

  void BeginGroupUndo(const CFX_WideString& sTitle);
  void EndGroupUndo();
  void AddEditUndoItem(CFX_Edit_UndoItem* pEditUndoItem);

  void SetPageInfo(const CPVT_WordPlace& place);
  CPVT_WordPlace SearchPageEndPlace(const CPVT_WordPlace& wpPageBegin,
                                    const CFX_FloatPoint& point) const;
  FX_FLOAT GetLineTop(const CPVT_WordPlace& place) const;
  FX_FLOAT GetLineBottom(const CPVT_WordPlace& place) const;

 private:
  std::unique_ptr<CPDF_VariableText> m_pVT;
  CPWL_EditCtrl* m_pNotify;
  CPWL_Edit* m_pOprNotify;
  std::unique_ptr<CFX_Edit_Provider> m_pVTProvider;

  CPVT_WordPlace m_wpCaret;
  CPVT_WordPlace m_wpOldCaret;
  CFX_Edit_Select m_SelState;

  CFX_FloatPoint m_ptScrollPos;
  CFX_FloatPoint m_ptRefreshScrollPos;
  FX_BOOL m_bEnableScroll;
  std::unique_ptr<CFX_Edit_Iterator> m_pIterator;
  CFX_Edit_Refresh m_Refresh;
  CFX_FloatPoint m_ptCaret;
  CFX_Edit_Undo m_Undo;
  int32_t m_nAlignment;
  FX_BOOL m_bNotifyFlag;
  FX_BOOL m_bEnableOverflow;
  FX_BOOL m_bEnableRefresh;
  CFX_FloatRect m_rcOldContent;
  FX_BOOL m_bEnableUndo;
  FX_BOOL m_bOprNotify;
  CFX_Edit_GroupUndoItem* m_pGroupUndoItem;
};

class CFX_Edit_Iterator {
 public:
  CFX_Edit_Iterator(CFX_Edit* pEdit, CPDF_VariableText::Iterator* pVTIterator);
  ~CFX_Edit_Iterator();

  FX_BOOL NextWord();
  FX_BOOL NextLine();
  FX_BOOL NextSection();
  FX_BOOL PrevWord();
  FX_BOOL PrevLine();
  FX_BOOL PrevSection();
  FX_BOOL GetWord(CPVT_Word& word) const;
  FX_BOOL GetLine(CPVT_Line& line) const;
  FX_BOOL GetSection(CPVT_Section& section) const;
  void SetAt(int32_t nWordIndex);
  void SetAt(const CPVT_WordPlace& place);
  const CPVT_WordPlace& GetAt() const;
  CFX_Edit* GetEdit() const;

 private:
  CFX_Edit* m_pEdit;
  CPDF_VariableText::Iterator* m_pVTIterator;
};

class CFX_Edit_Provider : public CPDF_VariableText::Provider {
 public:
  explicit CFX_Edit_Provider(IPVT_FontMap* pFontMap);
  ~CFX_Edit_Provider() override;

  IPVT_FontMap* GetFontMap();

  // CPDF_VariableText::Provider:
  int32_t GetCharWidth(int32_t nFontIndex,
                       uint16_t word,
                       int32_t nWordStyle) override;
  int32_t GetTypeAscent(int32_t nFontIndex) override;
  int32_t GetTypeDescent(int32_t nFontIndex) override;
  int32_t GetWordFontIndex(uint16_t word,
                           int32_t charset,
                           int32_t nFontIndex) override;
  int32_t GetDefaultFontIndex() override;
  FX_BOOL IsLatinWord(uint16_t word) override;

 private:
  IPVT_FontMap* m_pFontMap;
};

#endif  // FPDFSDK_FXEDIT_INCLUDE_FXET_EDIT_H_
