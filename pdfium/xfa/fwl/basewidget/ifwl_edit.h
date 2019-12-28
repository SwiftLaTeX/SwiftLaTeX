// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_EDIT_H_
#define XFA_FWL_BASEWIDGET_IFWL_EDIT_H_

#include <vector>

#include "xfa/fwl/core/cfwl_event.h"
#include "xfa/fwl/core/ifwl_dataprovider.h"
#include "xfa/fwl/lightwidget/cfwl_widget.h"

#define FWL_CLASS_Edit L"FWL_EDIT"
#define FWL_STYLEEXT_EDT_ReadOnly (1L << 0)
#define FWL_STYLEEXT_EDT_MultiLine (1L << 1)
#define FWL_STYLEEXT_EDT_WantReturn (1L << 2)
#define FWL_STYLEEXT_EDT_NoHideSel (1L << 3)
#define FWL_STYLEEXT_EDT_AutoHScroll (1L << 4)
#define FWL_STYLEEXT_EDT_AutoVScroll (1L << 5)
#define FWL_STYLEEXT_EDT_NoRedoUndo (1L << 6)
#define FWL_STYLEEXT_EDT_Validate (1L << 7)
#define FWL_STYLEEXT_EDT_Password (1L << 8)
#define FWL_STYLEEXT_EDT_Number (1L << 9)
#define FWL_STYLEEXT_EDT_HSelfAdaption (1L << 10)
#define FWL_STYLEEXT_EDT_VSelfAdaption (1L << 11)
#define FWL_STYLEEXT_EDT_VerticalLayout (1L << 12)
#define FWL_STYLEEXT_EDT_VerticalChars (1L << 13)
#define FWL_STYLEEXT_EDT_ReverseLine (1L << 14)
#define FWL_STYLEEXT_EDT_ArabicShapes (1L << 15)
#define FWL_STYLEEXT_EDT_ExpandTab (1L << 16)
#define FWL_STYLEEXT_EDT_CombText (1L << 17)
#define FWL_STYLEEXT_EDT_HNear (0L << 18)
#define FWL_STYLEEXT_EDT_HCenter (1L << 18)
#define FWL_STYLEEXT_EDT_HFar (2L << 18)
#define FWL_STYLEEXT_EDT_VNear (0L << 20)
#define FWL_STYLEEXT_EDT_VCenter (1L << 20)
#define FWL_STYLEEXT_EDT_VFar (2L << 20)
#define FWL_STYLEEXT_EDT_Justified (1L << 22)
#define FWL_STYLEEXT_EDT_Distributed (2L << 22)
#define FWL_STYLEEXT_EDT_HAlignMask (3L << 18)
#define FWL_STYLEEXT_EDT_VAlignMask (3L << 20)
#define FWL_STYLEEXT_EDT_HAlignModeMask (3L << 22)
#define FWL_STYLEEXT_EDT_InnerCaret (1L << 24)
#define FWL_STYLEEXT_EDT_ShowScrollbarFocus (1L << 25)
#define FWL_STYLEEXT_EDT_OuterScrollbar (1L << 26)
#define FWL_STYLEEXT_EDT_LastLineHeight (1L << 27)

enum FWL_EDT_TEXTCHANGED {
  FWL_EDT_TEXTCHANGED_Insert = 0,
  FWL_EDT_TEXTCHANGED_Delete,
  FWL_EDT_TEXTCHANGED_Replace,
};

FWL_EVENT_DEF(CFWL_EvtEdtTextChanged,
              CFWL_EventType::TextChanged,
              int32_t nChangeType;
              CFX_WideString wsInsert;
              CFX_WideString wsDelete;
              CFX_WideString wsPrevText;)

FWL_EVENT_DEF(CFWL_EvtEdtTextFull, CFWL_EventType::TextFull)

FWL_EVENT_DEF(CFWL_EvtEdtPreSelfAdaption,
              CFWL_EventType::PreSelfAdaption,
              FX_BOOL bHSelfAdaption;
              FX_BOOL bVSelfAdaption;
              CFX_RectF rtAfterChange;)

FWL_EVENT_DEF(CFWL_EvtEdtValidate,
              CFWL_EventType::Validate,
              IFWL_Widget* pDstWidget;
              CFX_WideString wsInsert;
              FX_BOOL bValidate;)

FWL_EVENT_DEF(CFWL_EvtEdtCheckWord,
              CFWL_EventType::CheckWord,
              CFX_ByteString bsWord;
              FX_BOOL bCheckWord;)

FWL_EVENT_DEF(CFWL_EvtEdtGetSuggestWords,
              CFWL_EventType::GetSuggestedWords,
              FX_BOOL bSuggestWords;
              CFX_ByteString bsWord;
              std::vector<CFX_ByteString> bsArraySuggestWords;)

class CFWL_WidgetImpProperties;
class IFDE_TxtEdtDoRecord;

class IFWL_EditDP : public IFWL_DataProvider {};

class IFWL_Edit : public IFWL_Widget {
 public:
  static IFWL_Edit* Create(const CFWL_WidgetImpProperties& properties,
                           IFWL_Widget* pOuter);
  static IFWL_Edit* CreateComboEdit(const CFWL_WidgetImpProperties& properties,
                                    IFWL_Widget* pOuter);

  FWL_Error SetText(const CFX_WideString& wsText);
  int32_t GetTextLength() const;
  FWL_Error GetText(CFX_WideString& wsText,
                    int32_t nStart = 0,
                    int32_t nCount = -1) const;
  FWL_Error ClearText();
  int32_t GetCaretPos() const;
  int32_t SetCaretPos(int32_t nIndex, FX_BOOL bBefore = TRUE);
  FWL_Error AddSelRange(int32_t nStart, int32_t nCount = -1);
  int32_t CountSelRanges();
  int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  FWL_Error ClearSelections();
  int32_t GetLimit();
  FWL_Error SetLimit(int32_t nLimit);
  FWL_Error SetAliasChar(FX_WCHAR wAlias);
  FWL_Error Insert(int32_t nStart, const FX_WCHAR* lpText, int32_t nLen);
  FWL_Error DeleteSelections();
  FWL_Error DeleteRange(int32_t nStart, int32_t nCount = -1);
  FWL_Error Replace(int32_t nStart,
                    int32_t nLen,
                    const CFX_WideStringC& wsReplace);
  FWL_Error DoClipboard(int32_t iCmd);
  FX_BOOL Copy(CFX_WideString& wsCopy);
  FX_BOOL Cut(CFX_WideString& wsCut);
  FX_BOOL Paste(const CFX_WideString& wsPaste);
  FX_BOOL Delete();
  FX_BOOL Redo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL Undo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL Undo();
  FX_BOOL Redo();
  FX_BOOL CanUndo();
  FX_BOOL CanRedo();
  FWL_Error SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant);
  FWL_Error SetOuter(IFWL_Widget* pOuter);
  FWL_Error SetNumberRange(int32_t iMin, int32_t iMax);
  FWL_Error SetBackColor(uint32_t dwColor);
  FWL_Error SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize);
  void SetScrollOffset(FX_FLOAT fScrollOffset);
  FX_BOOL GetSuggestWords(CFX_PointF pointf,
                          std::vector<CFX_ByteString>& sSuggest);
  FX_BOOL ReplaceSpellCheckWord(CFX_PointF pointf,
                                const CFX_ByteStringC& bsReplace);

 protected:
  IFWL_Edit();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_EDIT_H_
