// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PDFWINDOW_PWL_COMBOBOX_H_
#define FPDFSDK_PDFWINDOW_PWL_COMBOBOX_H_

#include "fpdfsdk/pdfwindow/PWL_Edit.h"
#include "fpdfsdk/pdfwindow/PWL_ListBox.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"

class CPWL_CBEdit : public CPWL_Edit {
 public:
  CPWL_CBEdit() {}
  ~CPWL_CBEdit() override {}
};

class CPWL_CBListBox : public CPWL_ListBox {
 public:
  CPWL_CBListBox() {}
  ~CPWL_CBListBox() override {}

  // CPWL_ListBox
  FX_BOOL OnLButtonUp(const CFX_FloatPoint& point, uint32_t nFlag) override;

  FX_BOOL OnKeyDownWithExit(uint16_t nChar, FX_BOOL& bExit, uint32_t nFlag);
  FX_BOOL OnCharWithExit(uint16_t nChar, FX_BOOL& bExit, uint32_t nFlag);
};

#define PWL_COMBOBOX_BUTTON_WIDTH 13

class CPWL_CBButton : public CPWL_Wnd {
 public:
  CPWL_CBButton() {}
  ~CPWL_CBButton() override {}

  // CPWL_Wnd
  void GetThisAppearanceStream(CFX_ByteTextBuf& sAppStream) override;
  void DrawThisAppearance(CFX_RenderDevice* pDevice,
                          CFX_Matrix* pUser2Device) override;
  FX_BOOL OnLButtonDown(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnLButtonUp(const CFX_FloatPoint& point, uint32_t nFlag) override;
};

class CPWL_ComboBox : public CPWL_Wnd {
 public:
  CPWL_ComboBox();
  ~CPWL_ComboBox() override {}

  CPWL_Edit* GetEdit() const { return m_pEdit; }

  // CPWL_Wnd:
  CFX_ByteString GetClassName() const override;
  void OnCreate(PWL_CREATEPARAM& cp) override;
  FX_BOOL OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  FX_BOOL OnChar(uint16_t nChar, uint32_t nFlag) override;
  void OnNotify(CPWL_Wnd* pWnd,
                uint32_t msg,
                intptr_t wParam = 0,
                intptr_t lParam = 0) override;
  void CreateChildWnd(const PWL_CREATEPARAM& cp) override;
  void RePosChildWnd() override;
  CFX_FloatRect GetFocusRect() const override;
  void SetFocus() override;
  void KillFocus() override;

  void SetFillerNotify(IPWL_Filler_Notify* pNotify);

  CFX_WideString GetText() const;
  void SetText(const FX_WCHAR* text);

  void AddString(const FX_WCHAR* str);
  int32_t GetSelect() const;
  void SetSelect(int32_t nItemIndex);

  void SetEditSel(int32_t nStartChar, int32_t nEndChar);
  void GetEditSel(int32_t& nStartChar, int32_t& nEndChar) const;
  void Clear();
  void SelectAll();
  FX_BOOL IsPopup() const;

  void SetSelectText();

  void AttachFFLData(void* pData) { m_pFormFiller = pData; }

 private:
  void CreateEdit(const PWL_CREATEPARAM& cp);
  void CreateButton(const PWL_CREATEPARAM& cp);
  void CreateListBox(const PWL_CREATEPARAM& cp);
  void SetPopup(FX_BOOL bPopup);

  CPWL_CBEdit* m_pEdit;
  CPWL_CBButton* m_pButton;
  CPWL_CBListBox* m_pList;
  FX_BOOL m_bPopup;
  CFX_FloatRect m_rcOldWindow;
  int32_t m_nPopupWhere;
  int32_t m_nSelectItem;
  IPWL_Filler_Notify* m_pFillerNotify;

  void* m_pFormFiller;
};

#endif  // FPDFSDK_PDFWINDOW_PWL_COMBOBOX_H_
