// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_EDIT_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_EDIT_H_

#include <vector>

#include "xfa/fwl/lightwidget/cfwl_widget.h"

class CFWL_WidgetProperties;
class IFDE_TxtEdtDoRecord;

class CFWL_Edit : public CFWL_Widget {
 public:
  CFWL_Edit();
  ~CFWL_Edit() override;

  static CFWL_Edit* Create();
  FWL_Error Initialize(const CFWL_WidgetProperties* pProperties = nullptr);
  FWL_Error SetText(const CFX_WideString& wsText);
  int32_t GetTextLength() const;
  FWL_Error GetText(CFX_WideString& wsText,
                    int32_t nStart = 0,
                    int32_t nCount = -1) const;
  FWL_Error ClearText();
  int32_t GetCaretPos() const;
  int32_t SetCaretPos(int32_t nIndex, FX_BOOL bBefore = TRUE);
  int32_t AddSelRange(int32_t nStart, int32_t nCount = -1);
  int32_t CountSelRanges();
  int32_t GetSelRange(int32_t nIndex, int32_t& nStart);
  FWL_Error ClearSelections();
  int32_t GetLimit();
  FWL_Error SetLimit(int32_t nLimit);
  FWL_Error SetAliasChar(FX_WCHAR wAlias);
  FWL_Error SetFormatString(const CFX_WideString& wsFormat);
  FWL_Error Insert(int32_t nStart, const FX_WCHAR* lpText, int32_t nLen);
  FWL_Error DeleteSelections();
  FWL_Error DeleteRange(int32_t nStart, int32_t nCount = -1);
  FWL_Error Replace(int32_t nStart,
                    int32_t nLen,
                    const CFX_WideStringC& wsReplace);
  FWL_Error DoClipboard(int32_t iCmd);
  FX_BOOL Redo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL Undo(const IFDE_TxtEdtDoRecord* pRecord);
  FWL_Error SetTabWidth(FX_FLOAT fTabWidth, FX_BOOL bEquidistant);
  FWL_Error SetNumberRange(int32_t iMin, int32_t iMax);
  FWL_Error SetBackColor(uint32_t dwColor);
  FWL_Error SetFont(const CFX_WideString& wsFont, FX_FLOAT fSize);
  FX_BOOL CanUndo();
  FX_BOOL CanRedo();
  FX_BOOL Undo();
  FX_BOOL Redo();
  FX_BOOL Copy(CFX_WideString& wsCopy);
  FX_BOOL Cut(CFX_WideString& wsCut);
  FX_BOOL Paste(const CFX_WideString& wsPaste);
  FX_BOOL Delete();
  void SetScrollOffset(FX_FLOAT fScrollOffset);
  FX_BOOL GetSuggestWords(CFX_PointF pointf,
                          std::vector<CFX_ByteString>& sSuggest);
  FX_BOOL ReplaceSpellCheckWord(CFX_PointF pointf,
                                const CFX_ByteStringC& bsReplace);
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_EDIT_H_
