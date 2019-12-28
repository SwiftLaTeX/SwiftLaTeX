// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_APP_XFA_FFCHOICELIST_H_
#define XFA_FXFA_APP_XFA_FFCHOICELIST_H_

#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"

class CXFA_FFListBox : public CXFA_FFField {
 public:
  CXFA_FFListBox(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFListBox() override;

  // CXFA_FFField
  FX_BOOL LoadWidget() override;
  FX_BOOL OnKillFocus(CXFA_FFWidget* pNewWidget) override;
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

  void OnSelectChanged(IFWL_Widget* pWidget, const CFX_Int32Array& arrSels);
  void SetItemState(int32_t nIndex, FX_BOOL bSelected);
  void InsertItem(const CFX_WideStringC& wsLabel, int32_t nIndex = -1);
  void DeleteItem(int32_t nIndex);

 protected:
  FX_BOOL CommitData() override;
  FX_BOOL UpdateFWLData() override;
  FX_BOOL IsDataChanged() override;

  uint32_t GetAlignment();

  IFWL_WidgetDelegate* m_pOldDelegate;
};

class CXFA_FFComboBox : public CXFA_FFField {
 public:
  CXFA_FFComboBox(CXFA_FFPageView* pPageView, CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFComboBox() override;

  // CXFA_FFField
  FX_BOOL GetBBox(CFX_RectF& rtBox,
                  uint32_t dwStatus,
                  FX_BOOL bDrawFocus = FALSE) override;
  FX_BOOL LoadWidget() override;
  void UpdateWidgetProperty() override;
  FX_BOOL OnRButtonUp(uint32_t dwFlags, FX_FLOAT fx, FX_FLOAT fy) override;
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

  // IFWL_WidgetDelegate
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

  virtual void OpenDropDownList();

  void OnTextChanged(IFWL_Widget* pWidget, const CFX_WideString& wsChanged);
  void OnSelectChanged(IFWL_Widget* pWidget,
                       const CFX_Int32Array& arrSels,
                       FX_BOOL bLButtonUp);
  void OnPreOpen(IFWL_Widget* pWidget);
  void OnPostOpen(IFWL_Widget* pWidget);
  void SetItemState(int32_t nIndex, FX_BOOL bSelected);
  void InsertItem(const CFX_WideStringC& wsLabel, int32_t nIndex = -1);
  void DeleteItem(int32_t nIndex);

 protected:
  // CXFA_FFField
  FX_BOOL PtInActiveRect(FX_FLOAT fx, FX_FLOAT fy) override;
  FX_BOOL CommitData() override;
  FX_BOOL UpdateFWLData() override;
  FX_BOOL IsDataChanged() override;

  uint32_t GetAlignment();
  void FWLEventSelChange(CXFA_EventParam* pParam);

  CFX_WideString m_wsNewValue;
  IFWL_WidgetDelegate* m_pOldDelegate;
};

#endif  // XFA_FXFA_APP_XFA_FFCHOICELIST_H_
