// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_FWL_COMBOBOXIMP_H_
#define XFA_FWL_BASEWIDGET_FWL_COMBOBOXIMP_H_

#include <memory>

#include "xfa/fwl/basewidget/fwl_editimp.h"
#include "xfa/fwl/basewidget/fwl_listboximp.h"

class CFWL_WidgetImp;
class CFWL_WidgetImpProperties;
class CFWL_WidgetImpDelegate;
class CFWL_ListBoxImp;
class CFWL_ListBoxImpDelegate;
class CFWL_FormProxyImp;
class IFWL_Widget;
class CFWL_ComboEditImp;
class CFWL_ComboEditImpDelegate;
class CFWL_ComboListImp;
class CFWL_ComboListImpDelegate;
class CFWL_ComboBoxImp;
class CFWL_ComboBoxImpDelegate;
class CFWL_ComboProxyImpDelegate;
class CFWL_ComboEditImp : public CFWL_EditImp {
 public:
  CFWL_ComboEditImp(const CFWL_WidgetImpProperties& properties,
                    IFWL_Widget* pOuter);

  void ClearSelected();
  void SetSelected();
  void EndCaret();
  void FlagFocus(FX_BOOL bSet);

 protected:
  void SetComboBoxFocus(FX_BOOL bSet);
  CFWL_ComboBoxImp* m_pOuter;
  friend class CFWL_ComboEditImpDelegate;
};
class CFWL_ComboEditImpDelegate : public CFWL_EditImpDelegate {
 public:
  CFWL_ComboEditImpDelegate(CFWL_ComboEditImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;

 protected:
  CFWL_ComboEditImp* m_pOwner;
};
class CFWL_ComboListImp : public CFWL_ListBoxImp {
 public:
  CFWL_ComboListImp(const CFWL_WidgetImpProperties& properties,
                    IFWL_Widget* pOuter);

  // CFWL_WidgetImp
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;

  int32_t MatchItem(const CFX_WideString& wsMatch);
  void ChangeSelected(int32_t iSel);
  int32_t CountItems();
  void GetItemRect(int32_t nIndex, CFX_RectF& rtItem);
  void ClientToOuter(FX_FLOAT& fx, FX_FLOAT& fy);
  void SetFocus(FX_BOOL bSet);

  FX_BOOL m_bNotifyOwner;

  friend class CFWL_ComboListImpDelegate;
  friend class CFWL_ComboBoxImp;
};
class CFWL_ComboListImpDelegate : public CFWL_ListBoxImpDelegate {
 public:
  CFWL_ComboListImpDelegate(CFWL_ComboListImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;

 protected:
  void OnDropListFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  int32_t OnDropListMouseMove(CFWL_MsgMouse* pMsg);
  int32_t OnDropListLButtonDown(CFWL_MsgMouse* pMsg);
  int32_t OnDropListLButtonUp(CFWL_MsgMouse* pMsg);
  int32_t OnDropListKey(CFWL_MsgKey* pKey);
  void OnDropListKeyDown(CFWL_MsgKey* pKey);
  CFWL_ComboListImp* m_pOwner;
};
class CFWL_ComboBoxImp : public CFWL_WidgetImp {
 public:
  CFWL_ComboBoxImp(const CFWL_WidgetImpProperties& properties,
                   IFWL_Widget* pOuter);
  ~CFWL_ComboBoxImp() override;

  // CFWL_WidgetImp
  FWL_Error GetClassName(CFX_WideString& wsClass) const override;
  FWL_Type GetClassID() const override;
  FWL_Error Initialize() override;
  FWL_Error Finalize() override;
  FWL_Error GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize = FALSE) override;
  FWL_Error ModifyStylesEx(uint32_t dwStylesExAdded,
                           uint32_t dwStylesExRemoved) override;
  void SetStates(uint32_t dwStates, FX_BOOL bSet = TRUE) override;
  FWL_Error Update() override;
  FWL_WidgetHit HitTest(FX_FLOAT fx, FX_FLOAT fy) override;
  FWL_Error DrawWidget(CFX_Graphics* pGraphics,
                       const CFX_Matrix* pMatrix = nullptr) override;
  FWL_Error SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) override;

  int32_t GetCurSel();
  FWL_Error SetCurSel(int32_t iSel);
  FWL_Error SetEditText(const CFX_WideString& wsText);
  int32_t GetEditTextLength() const;
  FWL_Error GetEditText(CFX_WideString& wsText,
                        int32_t nStart = 0,
                        int32_t nCount = -1) const;
  FWL_Error SetEditSelRange(int32_t nStart, int32_t nCount = -1);
  int32_t GetEditSelRange(int32_t nIndex, int32_t& nStart);
  int32_t GetEditLimit();
  FWL_Error SetEditLimit(int32_t nLimit);
  FWL_Error EditDoClipboard(int32_t iCmd);
  FX_BOOL EditRedo(const IFDE_TxtEdtDoRecord* pRecord);
  FX_BOOL EditUndo(const IFDE_TxtEdtDoRecord* pRecord);
  IFWL_ListBox* GetListBoxt();
  FX_BOOL AfterFocusShowDropList();
  FWL_Error OpenDropDownList(FX_BOOL bActivate);
  FX_BOOL EditCanUndo();
  FX_BOOL EditCanRedo();
  FX_BOOL EditUndo();
  FX_BOOL EditRedo();
  FX_BOOL EditCanCopy();
  FX_BOOL EditCanCut();
  FX_BOOL EditCanSelectAll();
  FX_BOOL EditCopy(CFX_WideString& wsCopy);
  FX_BOOL EditCut(CFX_WideString& wsCut);
  FX_BOOL EditPaste(const CFX_WideString& wsPaste);
  FX_BOOL EditSelectAll();
  FX_BOOL EditDelete();
  FX_BOOL EditDeSelect();
  FWL_Error GetBBox(CFX_RectF& rect);
  FWL_Error EditModifyStylesEx(uint32_t dwStylesExAdded,
                               uint32_t dwStylesExRemoved);

 protected:
  friend class CFWL_ComboListImp;
  friend class CFWL_ComboEditImp;
  friend class CFWL_ComboEditImpDelegate;
  friend class CFWL_ComboListImpDelegate;
  friend class CFWL_ComboBoxImpDelegate;
  friend class CFWL_ComboProxyImpDelegate;

  void DrawStretchHandler(CFX_Graphics* pGraphics, const CFX_Matrix* pMatrix);
  FX_FLOAT GetListHeight();
  void ShowDropList(FX_BOOL bActivate);
  FX_BOOL IsDropListShowed();
  FX_BOOL IsDropDownStyle() const;
  void MatchEditText();
  void SynchrEditText(int32_t iListItem);
  void Layout();
  void ReSetTheme();
  void ReSetEditAlignment();
  void ReSetListItemAlignment();
  void ProcessSelChanged(FX_BOOL bLButtonUp);
  void InitProxyForm();
  FWL_Error DisForm_Initialize();
  void DisForm_InitComboList();
  void DisForm_InitComboEdit();
  void DisForm_ShowDropList(FX_BOOL bActivate);
  FX_BOOL DisForm_IsDropListShowed();
  FWL_Error DisForm_ModifyStylesEx(uint32_t dwStylesExAdded,
                                   uint32_t dwStylesExRemoved);
  FWL_Error DisForm_Update();
  FWL_WidgetHit DisForm_HitTest(FX_FLOAT fx, FX_FLOAT fy);
  FWL_Error DisForm_DrawWidget(CFX_Graphics* pGraphics,
                               const CFX_Matrix* pMatrix = nullptr);
  FWL_Error DisForm_GetBBox(CFX_RectF& rect);
  void DisForm_Layout();

  CFX_RectF m_rtClient;
  CFX_RectF m_rtContent;
  CFX_RectF m_rtBtn;
  CFX_RectF m_rtList;
  CFX_RectF m_rtProxy;
  CFX_RectF m_rtHandler;
  std::unique_ptr<IFWL_Edit> m_pEdit;
  std::unique_ptr<IFWL_ListBox> m_pListBox;
  IFWL_Form* m_pForm;
  FX_BOOL m_bLButtonDown;
  FX_BOOL m_bUpFormHandler;
  int32_t m_iCurSel;
  int32_t m_iBtnState;
  FX_FLOAT m_fComboFormHandler;
  FX_FLOAT m_fItemHeight;
  FX_BOOL m_bNeedShowList;
  CFWL_FormProxyImp* m_pProxy;
  CFWL_ComboProxyImpDelegate* m_pListProxyDelegate;
};
class CFWL_ComboBoxImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_ComboBoxImpDelegate(CFWL_ComboBoxImp* pOwner);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;

 protected:
  void OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnMouseLeave(CFWL_MsgMouse* pMsg);
  void OnKey(CFWL_MsgKey* pMsg);
  void DoSubCtrlKey(CFWL_MsgKey* pMsg);

 protected:
  void DisForm_OnProcessMessage(CFWL_Message* pMessage);
  void DisForm_OnLButtonDown(CFWL_MsgMouse* pMsg);
  void DisForm_OnFocusChanged(CFWL_Message* pMsg, FX_BOOL bSet = TRUE);
  void DisForm_OnKey(CFWL_MsgKey* pMsg);

 protected:
  CFWL_ComboBoxImp* m_pOwner;
  friend class CFWL_ComboEditImpDelegate;
  friend class CFWL_ComboListImpDelegate;
};
class CFWL_ComboProxyImpDelegate : public CFWL_WidgetImpDelegate {
 public:
  CFWL_ComboProxyImpDelegate(IFWL_Form* pForm, CFWL_ComboBoxImp* pComboBox);
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnDrawWidget(CFX_Graphics* pGraphics,
                    const CFX_Matrix* pMatrix = nullptr) override;
  void Reset() { m_bLButtonUpSelf = FALSE; }

 protected:
  void OnLButtonDown(CFWL_MsgMouse* pMsg);
  void OnLButtonUp(CFWL_MsgMouse* pMsg);
  void OnMouseMove(CFWL_MsgMouse* pMsg);
  void OnDeactive(CFWL_MsgDeactivate* pMsg);
  void OnFocusChanged(CFWL_MsgKillFocus* pMsg, FX_BOOL bSet);
  FX_BOOL m_bLButtonDown;
  FX_BOOL m_bLButtonUpSelf;
  FX_FLOAT m_fStartPos;
  IFWL_Form* m_pForm;
  CFWL_ComboBoxImp* m_pComboBox;
};

#endif  // XFA_FWL_BASEWIDGET_FWL_COMBOBOXIMP_H_
