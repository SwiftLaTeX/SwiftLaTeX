// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PDFWINDOW_PWL_LISTBOX_H_
#define FPDFSDK_PDFWINDOW_PWL_LISTBOX_H_

#include "fpdfsdk/fxedit/include/fx_edit.h"
#include "fpdfsdk/pdfwindow/PWL_Wnd.h"

class CFX_ListCtrl;
class CPWL_List_Notify;
class CPWL_ListBox;
class IPWL_Filler_Notify;
struct CPVT_SecProps;
struct CPVT_WordPlace;
struct CPVT_WordProps;

class CPWL_List_Notify {
 public:
  CPWL_List_Notify(CPWL_ListBox* pList);
  ~CPWL_List_Notify();

  void IOnSetScrollInfoY(FX_FLOAT fPlateMin,
                         FX_FLOAT fPlateMax,
                         FX_FLOAT fContentMin,
                         FX_FLOAT fContentMax,
                         FX_FLOAT fSmallStep,
                         FX_FLOAT fBigStep);
  void IOnSetScrollPosY(FX_FLOAT fy);
  void IOnInvalidateRect(CFX_FloatRect* pRect);

  void IOnSetCaret(FX_BOOL bVisible,
                   const CFX_FloatPoint& ptHead,
                   const CFX_FloatPoint& ptFoot,
                   const CPVT_WordPlace& place);

 private:
  CPWL_ListBox* m_pList;
};

class CPWL_ListBox : public CPWL_Wnd {
 public:
  CPWL_ListBox();
  ~CPWL_ListBox() override;

  // CPWL_Wnd
  CFX_ByteString GetClassName() const override;
  void OnCreated() override;
  void OnDestroy() override;
  void GetThisAppearanceStream(CFX_ByteTextBuf& sAppStream) override;
  void DrawThisAppearance(CFX_RenderDevice* pDevice,
                          CFX_Matrix* pUser2Device) override;
  FX_BOOL OnKeyDown(uint16_t nChar, uint32_t nFlag) override;
  FX_BOOL OnChar(uint16_t nChar, uint32_t nFlag) override;
  FX_BOOL OnLButtonDown(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnLButtonUp(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnMouseMove(const CFX_FloatPoint& point, uint32_t nFlag) override;
  FX_BOOL OnMouseWheel(short zDelta,
                       const CFX_FloatPoint& point,
                       uint32_t nFlag) override;
  void KillFocus() override;
  void OnNotify(CPWL_Wnd* pWnd,
                uint32_t msg,
                intptr_t wParam = 0,
                intptr_t lParam = 0) override;
  void RePosChildWnd() override;
  CFX_FloatRect GetFocusRect() const override;
  void SetFontSize(FX_FLOAT fFontSize) override;
  FX_FLOAT GetFontSize() const override;

  virtual CFX_WideString GetText() const;

  void OnNotifySelChanged(FX_BOOL bKeyDown, FX_BOOL& bExit, uint32_t nFlag);

  void AddString(const FX_WCHAR* str);
  void SetTopVisibleIndex(int32_t nItemIndex);
  void ScrollToListItem(int32_t nItemIndex);
  void ResetContent();
  void Reset();
  void Select(int32_t nItemIndex);
  void SetCaret(int32_t nItemIndex);
  void SetHoverSel(FX_BOOL bHoverSel);

  int32_t GetCount() const;
  FX_BOOL IsMultipleSel() const;
  int32_t GetCaretIndex() const;
  int32_t GetCurSel() const;
  FX_BOOL IsItemSelected(int32_t nItemIndex) const;
  int32_t GetTopVisibleIndex() const;
  int32_t FindNext(int32_t nIndex, FX_WCHAR nChar) const;
  CFX_FloatRect GetContentRect() const;
  FX_FLOAT GetFirstHeight() const;
  CFX_FloatRect GetListRect() const;

  void SetFillerNotify(IPWL_Filler_Notify* pNotify) {
    m_pFillerNotify = pNotify;
  }

 protected:
  std::unique_ptr<CFX_ListCtrl> m_pList;
  std::unique_ptr<CPWL_List_Notify> m_pListNotify;
  FX_BOOL m_bMouseDown;
  FX_BOOL m_bHoverSel;
  IPWL_Filler_Notify* m_pFillerNotify;

 public:
  void AttachFFLData(void* pData) { m_pFormFiller = pData; }

 private:
  void* m_pFormFiller;
};

#endif  // FPDFSDK_PDFWINDOW_PWL_LISTBOX_H_
