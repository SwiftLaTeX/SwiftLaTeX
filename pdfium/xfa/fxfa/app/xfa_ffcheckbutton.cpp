// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffcheckbutton.h"

#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/lightwidget/cfwl_checkbox.h"
#include "xfa/fxfa/app/xfa_ffexclgroup.h"
#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdoc.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"

CXFA_FFCheckButton::CXFA_FFCheckButton(CXFA_FFPageView* pPageView,
                                       CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFField(pPageView, pDataAcc), m_pOldDelegate(nullptr) {
  m_rtCheckBox.Set(0, 0, 0, 0);
}
CXFA_FFCheckButton::~CXFA_FFCheckButton() {}
FX_BOOL CXFA_FFCheckButton::LoadWidget() {
  CFWL_CheckBox* pCheckBox = CFWL_CheckBox::Create();
  pCheckBox->Initialize();
  m_pNormalWidget = pCheckBox;
  m_pNormalWidget->SetLayoutItem(this);
  IFWL_Widget* pWidget = m_pNormalWidget->GetWidget();
  CFWL_NoteDriver* pNoteDriver = FWL_GetApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(pWidget, pWidget);
  m_pOldDelegate = m_pNormalWidget->SetDelegate(this);
  if (m_pDataAcc->IsRadioButton()) {
    pCheckBox->ModifyStylesEx(FWL_STYLEEXT_CKB_RadioButton, 0xFFFFFFFF);
  }
  m_pNormalWidget->LockUpdate();
  UpdateWidgetProperty();
  SetFWLCheckState(m_pDataAcc->GetCheckState());
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}
void CXFA_FFCheckButton::UpdateWidgetProperty() {
  CFWL_CheckBox* pCheckBox = (CFWL_CheckBox*)m_pNormalWidget;
  if (!m_pNormalWidget) {
    return;
  }
  FX_FLOAT fSize = m_pDataAcc->GetCheckButtonSize();
  pCheckBox->SetBoxSize(fSize);
  uint32_t dwStyleEx = FWL_STYLEEXT_CKB_SignShapeCross;
  int32_t iCheckMark = m_pDataAcc->GetCheckButtonMark();
  switch (iCheckMark) {
    case XFA_ATTRIBUTEENUM_Check:
      dwStyleEx = FWL_STYLEEXT_CKB_SignShapeCheck;
      break;
    case XFA_ATTRIBUTEENUM_Circle:
      dwStyleEx = FWL_STYLEEXT_CKB_SignShapeCircle;
      break;
    case XFA_ATTRIBUTEENUM_Cross:
      break;
    case XFA_ATTRIBUTEENUM_Diamond:
      dwStyleEx = FWL_STYLEEXT_CKB_SignShapeDiamond;
      break;
    case XFA_ATTRIBUTEENUM_Square:
      dwStyleEx = FWL_STYLEEXT_CKB_SignShapeSquare;
      break;
    case XFA_ATTRIBUTEENUM_Star:
      dwStyleEx = FWL_STYLEEXT_CKB_SignShapeStar;
      break;
    default: {
      int32_t iShape = m_pDataAcc->GetCheckButtonShape();
      if (iShape == XFA_ATTRIBUTEENUM_Round) {
        dwStyleEx = FWL_STYLEEXT_CKB_SignShapeCircle;
      }
    } break;
  }
  if (m_pDataAcc->IsAllowNeutral()) {
    dwStyleEx |= FWL_STYLEEXT_CKB_3State;
  }
  pCheckBox->ModifyStylesEx(
      dwStyleEx, FWL_STYLEEXT_CKB_SignShapeMask | FWL_STYLEEXT_CKB_3State);
}
FX_BOOL CXFA_FFCheckButton::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  FX_FLOAT fCheckSize = m_pDataAcc->GetCheckButtonSize();
  CXFA_Margin mgWidget = m_pDataAcc->GetMargin();
  CFX_RectF rtWidget;
  GetRectWithoutRotate(rtWidget);
  if (mgWidget) {
    XFA_RectWidthoutMargin(rtWidget, mgWidget);
  }
  int32_t iCapPlacement = -1;
  FX_FLOAT fCapReserve = 0;
  CXFA_Caption caption = m_pDataAcc->GetCaption();
  if (caption && caption.GetPresence()) {
    m_rtCaption.Set(rtWidget.left, rtWidget.top, rtWidget.width,
                    rtWidget.height);
    iCapPlacement = caption.GetPlacementType();
    fCapReserve = caption.GetReserve();
    if (fCapReserve <= 0) {
      if (iCapPlacement == XFA_ATTRIBUTEENUM_Top ||
          iCapPlacement == XFA_ATTRIBUTEENUM_Bottom) {
        fCapReserve = rtWidget.height - fCheckSize;
      } else {
        fCapReserve = rtWidget.width - fCheckSize;
      }
    }
  }
  int32_t iHorzAlign = XFA_ATTRIBUTEENUM_Left;
  int32_t iVertAlign = XFA_ATTRIBUTEENUM_Top;
  if (CXFA_Para para = m_pDataAcc->GetPara()) {
    iHorzAlign = para.GetHorizontalAlign();
    iVertAlign = para.GetVerticalAlign();
  }
  m_rtUI = rtWidget;
  CXFA_Margin mgCap = caption.GetMargin();
  switch (iCapPlacement) {
    case XFA_ATTRIBUTEENUM_Left: {
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(mgCap);
      m_rtUI.width -= fCapReserve;
      m_rtUI.left += fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Top: {
      m_rtCaption.height = fCapReserve;
      XFA_RectWidthoutMargin(m_rtCaption, mgCap);
      m_rtUI.height -= fCapReserve;
      m_rtUI.top += fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Right: {
      m_rtCaption.left = m_rtCaption.right() - fCapReserve;
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(mgCap);
      m_rtUI.width -= fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Bottom: {
      m_rtCaption.top = m_rtCaption.bottom() - fCapReserve;
      m_rtCaption.height = fCapReserve;
      XFA_RectWidthoutMargin(m_rtCaption, mgCap);
      m_rtUI.height -= fCapReserve;
    } break;
    case XFA_ATTRIBUTEENUM_Inline:
      break;
    default:
      iHorzAlign = XFA_ATTRIBUTEENUM_Right;
      break;
  }
  if (iHorzAlign == XFA_ATTRIBUTEENUM_Center) {
    m_rtUI.left += (m_rtUI.width - fCheckSize) / 2;
  } else if (iHorzAlign == XFA_ATTRIBUTEENUM_Right) {
    m_rtUI.left = m_rtUI.right() - fCheckSize;
  }
  if (iVertAlign == XFA_ATTRIBUTEENUM_Middle) {
    m_rtUI.top += (m_rtUI.height - fCheckSize) / 2;
  } else if (iVertAlign == XFA_ATTRIBUTEENUM_Bottom) {
    m_rtUI.top = m_rtUI.bottom() - fCheckSize;
  }
  m_rtUI.width = fCheckSize;
  m_rtUI.height = fCheckSize;
  AddUIMargin(iCapPlacement);
  m_rtCheckBox = m_rtUI;
  CXFA_Border borderUI = m_pDataAcc->GetUIBorder();
  if (borderUI) {
    CXFA_Margin margin = borderUI.GetMargin();
    if (margin) {
      XFA_RectWidthoutMargin(m_rtUI, margin);
    }
  }
  m_rtUI.Normalize();
  LayoutCaption();
  SetFWLRect();
  if (m_pNormalWidget) {
    m_pNormalWidget->Update();
  }
  return TRUE;
}
void CXFA_FFCheckButton::CapLeftRightPlacement(CXFA_Margin mgCap) {
  XFA_RectWidthoutMargin(m_rtCaption, mgCap);
  if (m_rtCaption.height < 0) {
    m_rtCaption.top += m_rtCaption.height;
  }
  if (m_rtCaption.width < 0) {
    m_rtCaption.left += m_rtCaption.width;
    m_rtCaption.width = -m_rtCaption.width;
  }
}
void CXFA_FFCheckButton::AddUIMargin(int32_t iCapPlacement) {
  CFX_RectF rtUIMargin;
  m_pDataAcc->GetUIMargin(rtUIMargin);
  m_rtUI.top -= rtUIMargin.top / 2 - rtUIMargin.height / 2;
  FX_FLOAT fLeftAddRight = rtUIMargin.left + rtUIMargin.width;
  FX_FLOAT fTopAddBottom = rtUIMargin.top + rtUIMargin.height;
  if (m_rtUI.width < fLeftAddRight) {
    if (iCapPlacement == XFA_ATTRIBUTEENUM_Right ||
        iCapPlacement == XFA_ATTRIBUTEENUM_Left) {
      m_rtUI.left -= fLeftAddRight - m_rtUI.width;
    } else {
      m_rtUI.left -= 2 * (fLeftAddRight - m_rtUI.width);
    }
    m_rtUI.width += 2 * (fLeftAddRight - m_rtUI.width);
  }
  if (m_rtUI.height < fTopAddBottom) {
    if (iCapPlacement == XFA_ATTRIBUTEENUM_Right) {
      m_rtUI.left -= fTopAddBottom - m_rtUI.height;
    }
    m_rtUI.top -= fTopAddBottom - m_rtUI.height;
    m_rtUI.height += 2 * (fTopAddBottom - m_rtUI.height);
  }
}
void CXFA_FFCheckButton::RenderWidget(CFX_Graphics* pGS,
                                      CFX_Matrix* pMatrix,
                                      uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus)) {
    return;
  }
  CFX_Matrix mtRotate;
  GetRotateMatrix(mtRotate);
  if (pMatrix) {
    mtRotate.Concat(*pMatrix);
  }
  CXFA_FFWidget::RenderWidget(pGS, &mtRotate, dwStatus);
  CXFA_Border borderUI = m_pDataAcc->GetUIBorder();
  DrawBorder(pGS, borderUI, m_rtUI, &mtRotate,
             m_pDataAcc->GetCheckButtonShape() == XFA_ATTRIBUTEENUM_Round
                 ? XFA_DRAWBOX_ForceRound
                 : 0);
  RenderCaption(pGS, &mtRotate);
  DrawHighlight(pGS, &mtRotate, dwStatus,
                m_pDataAcc->GetCheckButtonShape() == XFA_ATTRIBUTEENUM_Round);
  CFX_Matrix mt;
  mt.Set(1, 0, 0, 1, m_rtCheckBox.left, m_rtCheckBox.top);
  mt.Concat(mtRotate);
  GetApp()->GetWidgetMgrDelegate()->OnDrawWidget(m_pNormalWidget->GetWidget(),
                                                 pGS, &mt);
}
FX_BOOL CXFA_FFCheckButton::OnLButtonUp(uint32_t dwFlags,
                                        FX_FLOAT fx,
                                        FX_FLOAT fy) {
  if (!m_pNormalWidget || !IsButtonDown())
    return FALSE;

  SetButtonDown(FALSE);
  CFWL_MsgMouse ms;
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonUp;
  ms.m_dwFlags = dwFlags;
  ms.m_fx = fx;
  ms.m_fy = fy;
  FWLToClient(ms.m_fx, ms.m_fy);
  ms.m_pDstTarget = m_pNormalWidget->m_pIface;
  TranslateFWLMessage(&ms);
  return TRUE;
}

XFA_CHECKSTATE CXFA_FFCheckButton::FWLState2XFAState() {
  uint32_t dwState = m_pNormalWidget->GetStates();
  if (dwState & FWL_STATE_CKB_Checked)
    return XFA_CHECKSTATE_On;
  if (dwState & FWL_STATE_CKB_Neutral)
    return XFA_CHECKSTATE_Neutral;
  return XFA_CHECKSTATE_Off;
}

FX_BOOL CXFA_FFCheckButton::CommitData() {
  XFA_CHECKSTATE eCheckState = FWLState2XFAState();
  m_pDataAcc->SetCheckState(eCheckState, true);
  return TRUE;
}

FX_BOOL CXFA_FFCheckButton::IsDataChanged() {
  XFA_CHECKSTATE eCheckState = FWLState2XFAState();
  return m_pDataAcc->GetCheckState() != eCheckState;
}
void CXFA_FFCheckButton::SetFWLCheckState(XFA_CHECKSTATE eCheckState) {
  if (eCheckState == XFA_CHECKSTATE_Neutral) {
    m_pNormalWidget->SetStates(FWL_STATE_CKB_Neutral, TRUE);
  } else {
    m_pNormalWidget->SetStates(FWL_STATE_CKB_Checked,
                               eCheckState == XFA_CHECKSTATE_On);
  }
}
FX_BOOL CXFA_FFCheckButton::UpdateFWLData() {
  if (!m_pNormalWidget) {
    return FALSE;
  }
  XFA_CHECKSTATE eState = m_pDataAcc->GetCheckState();
  SetFWLCheckState(eState);
  m_pNormalWidget->Update();
  return TRUE;
}

void CXFA_FFCheckButton::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFCheckButton::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  switch (pEvent->GetClassID()) {
    case CFWL_EventType::CheckStateChanged: {
      CXFA_EventParam eParam;
      eParam.m_eType = XFA_EVENT_Change;
      m_pDataAcc->GetValue(eParam.m_wsNewText, XFA_VALUEPICTURE_Raw);
      CXFA_WidgetAcc* pFFExclGroup = m_pDataAcc->GetExclGroup();
      if (ProcessCommittedData()) {
        eParam.m_pTarget = pFFExclGroup;
        if (pFFExclGroup) {
          m_pDocView->AddValidateWidget(pFFExclGroup);
          m_pDocView->AddCalculateWidgetAcc(pFFExclGroup);
          pFFExclGroup->ProcessEvent(XFA_ATTRIBUTEENUM_Change, &eParam);
        }
        eParam.m_pTarget = m_pDataAcc;
        m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Change, &eParam);
      } else {
        SetFWLCheckState(m_pDataAcc->GetCheckState());
      }
      if (pFFExclGroup) {
        eParam.m_pTarget = pFFExclGroup;
        pFFExclGroup->ProcessEvent(XFA_ATTRIBUTEENUM_Click, &eParam);
      }
      eParam.m_pTarget = m_pDataAcc;
      m_pDataAcc->ProcessEvent(XFA_ATTRIBUTEENUM_Click, &eParam);
      break;
    }
    default:
      break;
  }
  m_pOldDelegate->OnProcessEvent(pEvent);
}

void CXFA_FFCheckButton::OnDrawWidget(CFX_Graphics* pGraphics,
                                      const CFX_Matrix* pMatrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, pMatrix);
}
