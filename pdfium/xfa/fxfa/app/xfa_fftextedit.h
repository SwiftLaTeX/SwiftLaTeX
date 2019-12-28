// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFTEXTEDIT_H_
#define XFA_FXFA_APP_XFA_FFTEXTEDIT_H_

#include <vector>

#include "xfa/fxfa/app/xfa_fffield.h"

class CXFA_FFTextEdit : public CXFA_FFField {
 public:
  CXFA_FFTextEdit(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFTextEdit() override;

  // CXFA_FFField
  FX_BOOL LoadWidget() override;
  void UpdateWidgetProperty() override;
  FX_BOOL OnLButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL OnRButtonDown(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL OnRButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL OnSetFocus(CXFA_FFWidget* pOldWidget) override;
  FX_BOOL OnKillFocus(CXFA_FFWidget* pNewWidget) override;
  FX_BOOL CanUndo() override;
  FX_BOOL CanRedo() override;
  FX_BOOL Undo() override;
  FX_BOOL Redo() override;
  FX_BOOL CanCopy() override;
  FX_BOOL CanCut() override;
  FX_BOOL CanPaste() override;
  FX_BOOL CanSelectAll() override;
  FX_BOOL Copy(CFX_WideString& wsCopy) override;
  FX_BOOL Cut(CFX_WideString& wsCut) override;
  FX_BOOL Paste(const CFX_WideString& wsPaste) override;
  FX_BOOL SelectAll() override;
  FX_BOOL Delete() override;
  FX_BOOL DeSelect() override;
  FX_BOOL GetSuggestWords(CFX_PointF pointf,
                          std::vector<CFX_ByteString>& sSuggest) override;
  FX_BOOL ReplaceSpellCheckWord(CFX_PointF pointf,
                                const CFX_ByteStringC& bsReplace) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

  void OnTextChanged(IFWL_Widget* pWidget,
                     const CFX_WideString& wsChanged,
                     const CFX_WideString& wsPrevText);
  void OnTextFull(IFWL_Widget* pWidget);
  FX_BOOL CheckWord(const CFX_ByteStringC& sWord);
  FX_BOOL GetSuggestWords(const CFX_ByteStringC& sWord,
                          std::vector<CFX_ByteString>& sSuggest);

 protected:
  FX_BOOL CommitData() override;
  FX_BOOL UpdateFWLData() override;
  FX_BOOL IsDataChanged() override;

  uint32_t GetAlignment();
  void ValidateNumberField(const CFX_WideString& wsText);

  IFWL_WidgetDelegate* m_pOldDelegate;
};

class CXFA_FFNumericEdit : public CXFA_FFTextEdit {
 public:
  CXFA_FFNumericEdit(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFNumericEdit() override;

  // CXFA_FFTextEdit
  FX_BOOL LoadWidget() override;
  void UpdateWidgetProperty() override;
  void OnProcessEvent(CFWL_Event* pEvent) override;

 public:
  FX_BOOL OnValidate(IFWL_Widget* pWidget, CFX_WideString& wsText);
};

class CXFA_FFPasswordEdit : public CXFA_FFTextEdit {
 public:
  CXFA_FFPasswordEdit(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFPasswordEdit() override;

  // CXFA_FFTextEdit
  FX_BOOL LoadWidget() override;
  void UpdateWidgetProperty() override;

 protected:
};

enum XFA_DATETIMETYPE {
  XFA_DATETIMETYPE_Date = 0,
  XFA_DATETIMETYPE_Time,
  XFA_DATETIMETYPE_DateAndTime
};

class CXFA_FFDateTimeEdit : public CXFA_FFTextEdit {
 public:
  CXFA_FFDateTimeEdit(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFDateTimeEdit() override;

  // CXFA_FFTextEdit
  FX_BOOL GetBBox(CFX_RectF& rtBox,
                  uint32_t dwStatus,
                  FX_BOOL bDrawFocus = FALSE) override;
  FX_BOOL LoadWidget() override;
  void UpdateWidgetProperty() override;

  FX_BOOL CanUndo() override;
  FX_BOOL CanRedo() override;
  FX_BOOL Undo() override;
  FX_BOOL Redo() override;
  FX_BOOL CanCopy() override;
  FX_BOOL CanCut() override;
  FX_BOOL CanPaste() override;
  FX_BOOL CanSelectAll() override;
  FX_BOOL Copy(CFX_WideString& wsCopy) override;
  FX_BOOL Cut(CFX_WideString& wsCut) override;
  FX_BOOL Paste(const CFX_WideString& wsPaste) override;
  FX_BOOL SelectAll() override;
  FX_BOOL Delete() override;
  FX_BOOL DeSelect() override;
  void OnProcessEvent(CFWL_Event* pEvent) override;

  void OnSelectChanged(IFWL_Widget* pWidget,
                       int32_t iYear,
                       int32_t iMonth,
                       int32_t iDay);

 protected:
  FX_BOOL PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL CommitData() override;
  FX_BOOL UpdateFWLData() override;
  FX_BOOL IsDataChanged() override;

  uint32_t GetAlignment();
};

#endif  // XFA_FXFA_APP_XFA_FFTEXTEDIT_H_
