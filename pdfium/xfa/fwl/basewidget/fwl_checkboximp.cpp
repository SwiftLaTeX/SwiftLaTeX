// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_checkboximp.h"

#include <algorithm>

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_checkbox.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"

namespace {

const int kCaptionMargin = 5;

}  // namespace

// static
IFWL_CheckBox* IFWL_CheckBox::Create(const CFWL_WidgetImpProperties& properties,
                                     IFWL_Widget* pOuter) {
  IFWL_CheckBox* pCheckBox = new IFWL_CheckBox;
  CFWL_CheckBoxImp* pCheckBoxImpl = new CFWL_CheckBoxImp(properties, pOuter);
  pCheckBox->SetImpl(pCheckBoxImpl);
  pCheckBoxImpl->SetInterface(pCheckBox);
  return pCheckBox;
}
IFWL_CheckBox::IFWL_CheckBox() {}
int32_t IFWL_CheckBox::GetCheckState() {
  return static_cast<CFWL_CheckBoxImp*>(GetImpl())->GetCheckState();
}
FWL_Error IFWL_CheckBox::SetCheckState(int32_t iCheck) {
  return static_cast<CFWL_CheckBoxImp*>(GetImpl())->SetCheckState(iCheck);
}

CFWL_CheckBoxImp::CFWL_CheckBoxImp(const CFWL_WidgetImpProperties& properties,
                                   IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_dwTTOStyles(FDE_TTOSTYLE_SingleLine),
      m_iTTOAlign(FDE_TTOALIGNMENT_Center),
      m_bBtnDown(FALSE) {
  m_rtClient.Reset();
  m_rtBox.Reset();
  m_rtCaption.Reset();
  m_rtFocus.Reset();
}

CFWL_CheckBoxImp::~CFWL_CheckBoxImp() {}

FWL_Error CFWL_CheckBoxImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_CheckBox;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_CheckBoxImp::GetClassID() const {
  return FWL_Type::CheckBox;
}

FWL_Error CFWL_CheckBoxImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;

  m_pDelegate = new CFWL_CheckBoxImpDelegate(this);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_CheckBoxImp::Finalize() {
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_CheckBoxImp::GetWidgetRect(CFX_RectF& rect, FX_BOOL bAutoSize) {
  if (bAutoSize) {
    rect.Set(0, 0, 0, 0);
    if (!m_pProperties->m_pThemeProvider)
      m_pProperties->m_pThemeProvider = GetAvailableTheme();
    if (!m_pProperties->m_pThemeProvider)
      return FWL_Error::Indefinite;
    if (!m_pProperties->m_pDataProvider)
      return FWL_Error::Indefinite;
    CFX_WideString wsCaption;
    m_pProperties->m_pDataProvider->GetCaption(m_pInterface, wsCaption);
    if (wsCaption.GetLength() > 0) {
      CFX_SizeF sz = CalcTextSize(
          wsCaption, m_pProperties->m_pThemeProvider,
          m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_MultiLine);
      rect.Set(0, 0, sz.x, sz.y);
    }
    rect.Inflate(kCaptionMargin, kCaptionMargin);
    IFWL_CheckBoxDP* pData =
        static_cast<IFWL_CheckBoxDP*>(m_pProperties->m_pDataProvider);
    FX_FLOAT fCheckBox = pData->GetBoxSize(m_pInterface);
    rect.width += fCheckBox;
    if (rect.height < fCheckBox) {
      rect.height = fCheckBox;
    }
    CFWL_WidgetImp::GetWidgetRect(rect, TRUE);
  } else {
    rect = m_pProperties->m_rtWidget;
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_CheckBoxImp::Update() {
  if (IsLocked()) {
    return FWL_Error::Indefinite;
  }
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  UpdateTextOutStyles();
  Layout();
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_CheckBoxImp::DrawWidget(CFX_Graphics* pGraphics,
                                       const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider)
    return FWL_Error::Indefinite;
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  if (HasBorder()) {
    DrawBorder(pGraphics, CFWL_Part::Border, m_pProperties->m_pThemeProvider,
               pMatrix);
  }
  if (HasEdge()) {
    DrawEdge(pGraphics, CFWL_Part::Edge, pTheme, pMatrix);
  }
  int32_t dwStates = GetPartStates();
  {
    CFWL_ThemeBackground param;
    param.m_pWidget = m_pInterface;
    param.m_iPart = CFWL_Part::Background;
    param.m_dwStates = dwStates;
    param.m_pGraphics = pGraphics;
    if (pMatrix) {
      param.m_matrix.Concat(*pMatrix);
    }
    param.m_rtPart = m_rtClient;
    if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
      param.m_pData = &m_rtFocus;
    }
    pTheme->DrawBackground(&param);
    param.m_iPart = CFWL_Part::CheckBox;
    param.m_rtPart = m_rtBox;
    pTheme->DrawBackground(&param);
  }
  if (!m_pProperties->m_pDataProvider)
    return FWL_Error::Indefinite;
  {
    CFX_WideString wsCaption;
    m_pProperties->m_pDataProvider->GetCaption(m_pInterface, wsCaption);
    int32_t iLen = wsCaption.GetLength();
    if (iLen <= 0)
      return FWL_Error::Indefinite;
    CFWL_ThemeText textParam;
    textParam.m_pWidget = m_pInterface;
    textParam.m_iPart = CFWL_Part::Caption;
    textParam.m_dwStates = dwStates;
    textParam.m_pGraphics = pGraphics;
    if (pMatrix) {
      textParam.m_matrix.Concat(*pMatrix);
    }
    textParam.m_rtPart = m_rtCaption;
    textParam.m_wsText = wsCaption;
    textParam.m_dwTTOStyles = m_dwTTOStyles;
    textParam.m_iTTOAlign = m_iTTOAlign;
    pTheme->DrawText(&textParam);
  }
  return FWL_Error::Succeeded;
}
int32_t CFWL_CheckBoxImp::GetCheckState() {
  if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State) &&
      ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
       FWL_STATE_CKB_Neutral)) {
    return 2;
  }
  if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
      FWL_STATE_CKB_Checked) {
    return 1;
  }
  return 0;
}
FWL_Error CFWL_CheckBoxImp::SetCheckState(int32_t iCheck) {
  m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
  switch (iCheck) {
    case 0: {
      break;
    }
    case 1: {
      m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
      break;
    }
    case 2: {
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State) {
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Neutral;
      }
      break;
    }
    default: {}
  }
  Repaint(&m_rtClient);
  return FWL_Error::Succeeded;
}
void CFWL_CheckBoxImp::Layout() {
  int32_t width = int32_t(m_pProperties->m_rtWidget.width + 0.5f);
  int32_t height = int32_t(m_pProperties->m_rtWidget.height + 0.5f);
  m_pProperties->m_rtWidget.width = (FX_FLOAT)width;
  m_pProperties->m_rtWidget.height = (FX_FLOAT)height;
  GetClientRect(m_rtClient);
  FX_FLOAT fBoxTop = m_rtClient.top;
  FX_FLOAT fBoxLeft = m_rtClient.left;
  FX_FLOAT fTextLeft = 0.0, fTextRight = 0.0;
  FX_FLOAT fClientRight = m_rtClient.right();
  FX_FLOAT fClientBottom = m_rtClient.bottom();
  if (!m_pProperties->m_pDataProvider)
    return;
  IFWL_CheckBoxDP* pData =
      static_cast<IFWL_CheckBoxDP*>(m_pProperties->m_pDataProvider);
  FX_FLOAT fCheckBox = pData->GetBoxSize(m_pInterface);
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_VLayoutMask) {
    case FWL_STYLEEXT_CKB_Top: {
      fBoxTop = m_rtClient.top;
      break;
    }
    case FWL_STYLEEXT_CKB_Bottom: {
      fBoxTop = fClientBottom - fCheckBox;
      break;
    }
    case FWL_STYLEEXT_CKB_VCenter:
    default: {
      fBoxTop = m_rtClient.top + (m_rtClient.height - fCheckBox) / 2;
      fBoxTop = FXSYS_floor(fBoxTop);
    }
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_LeftText) {
    fBoxLeft = fClientRight - fCheckBox;
    fTextLeft = m_rtClient.left;
    fTextRight = fBoxLeft;
  } else {
    fTextLeft = fBoxLeft + fCheckBox;
    fTextRight = fClientRight;
  }
  m_rtBox.Set(fBoxLeft, fBoxTop, fCheckBox, fCheckBox);
  m_rtCaption.Set(fTextLeft, m_rtClient.top, fTextRight - fTextLeft,
                  m_rtClient.height);
  m_rtCaption.Inflate(-kCaptionMargin, -kCaptionMargin);
  CFX_RectF rtFocus;
  rtFocus.Set(m_rtCaption.left, m_rtCaption.top, m_rtCaption.width,
              m_rtCaption.height);
  CFX_WideString wsCaption;
  m_pProperties->m_pDataProvider->GetCaption(m_pInterface, wsCaption);
  if (wsCaption.IsEmpty()) {
    m_rtFocus.Set(0, 0, 0, 0);
  } else {
    CalcTextRect(wsCaption, m_pProperties->m_pThemeProvider, m_dwTTOStyles,
                 m_iTTOAlign, rtFocus);
    if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_MultiLine) == 0) {
      FX_FLOAT fWidth = std::max(m_rtCaption.width, rtFocus.width);
      FX_FLOAT fHeight = std::min(m_rtCaption.height, rtFocus.height);
      FX_FLOAT fLeft = m_rtCaption.left;
      FX_FLOAT fTop = m_rtCaption.top;
      if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_HLayoutMask) ==
          FWL_STYLEEXT_CKB_Center) {
        fLeft = m_rtCaption.left + (m_rtCaption.width - fWidth) / 2;
      } else if ((m_pProperties->m_dwStyleExes &
                  FWL_STYLEEXT_CKB_HLayoutMask) == FWL_STYLEEXT_CKB_Right) {
        fLeft = m_rtCaption.right() - fWidth;
      }
      if ((m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_VLayoutMask) ==
          FWL_STYLEEXT_CKB_VCenter) {
        fTop = m_rtCaption.top + (m_rtCaption.height - fHeight) / 2;
      } else if ((m_pProperties->m_dwStyleExes &
                  FWL_STYLEEXT_CKB_VLayoutMask) == FWL_STYLEEXT_CKB_Bottom) {
        fTop = m_rtCaption.bottom() - fHeight;
      }
      m_rtFocus.Set(fLeft, fTop, fWidth, fHeight);
    } else {
      m_rtFocus.Set(rtFocus.left, rtFocus.top, rtFocus.width, rtFocus.height);
    }
    m_rtFocus.Inflate(1, 1);
  }
}
uint32_t CFWL_CheckBoxImp::GetPartStates() {
  int32_t dwStates = CFWL_PartState_Normal;
  if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
      FWL_STATE_CKB_Neutral) {
    dwStates = CFWL_PartState_Neutral;
  } else if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
             FWL_STATE_CKB_Checked) {
    dwStates = CFWL_PartState_Checked;
  }
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    dwStates |= CFWL_PartState_Disabled;
  } else if (m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered) {
    dwStates |= CFWL_PartState_Hovered;
  } else if (m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed) {
    dwStates |= CFWL_PartState_Pressed;
  } else {
    dwStates |= CFWL_PartState_Normal;
  }
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    dwStates |= CFWL_PartState_Focused;
  }
  return dwStates;
}
void CFWL_CheckBoxImp::UpdateTextOutStyles() {
  m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  switch (m_pProperties->m_dwStyleExes &
          (FWL_STYLEEXT_CKB_HLayoutMask | FWL_STYLEEXT_CKB_VLayoutMask)) {
    case FWL_STYLEEXT_CKB_Left | FWL_STYLEEXT_CKB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopLeft;
      break;
    }
    case FWL_STYLEEXT_CKB_Center | FWL_STYLEEXT_CKB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopCenter;
      break;
    }
    case FWL_STYLEEXT_CKB_Right | FWL_STYLEEXT_CKB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopRight;
      break;
    }
    case FWL_STYLEEXT_CKB_Left | FWL_STYLEEXT_CKB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_CenterLeft;
      break;
    }
    case FWL_STYLEEXT_CKB_Center | FWL_STYLEEXT_CKB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_Center;
      break;
    }
    case FWL_STYLEEXT_CKB_Right | FWL_STYLEEXT_CKB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_CenterRight;
      break;
    }
    case FWL_STYLEEXT_CKB_Left | FWL_STYLEEXT_CKB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomLeft;
      break;
    }
    case FWL_STYLEEXT_CKB_Center | FWL_STYLEEXT_CKB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomCenter;
      break;
    }
    case FWL_STYLEEXT_CKB_Right | FWL_STYLEEXT_CKB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomRight;
      break;
    }
    default: {}
  }
  m_dwTTOStyles = 0;
  if (m_pProperties->m_dwStyleExes & FWL_WGTSTYLE_RTLReading) {
    m_dwTTOStyles |= FDE_TTOSTYLE_RTL;
  }
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_MultiLine) {
    m_dwTTOStyles |= FDE_TTOSTYLE_LineWrap;
  } else {
    m_dwTTOStyles |= FDE_TTOSTYLE_SingleLine;
  }
}
void CFWL_CheckBoxImp::NextStates() {
  uint32_t dwFirststate = m_pProperties->m_dwStates;
  if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_RadioButton) {
    if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
        FWL_STATE_CKB_Unchecked) {
      CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
      if (!pWidgetMgr->IsFormDisabled()) {
        CFX_ArrayTemplate<IFWL_Widget*> radioarr;
        pWidgetMgr->GetSameGroupRadioButton(m_pInterface, radioarr);
        IFWL_CheckBox* pCheckBox = nullptr;
        int32_t iCount = radioarr.GetSize();
        for (int32_t i = 0; i < iCount; i++) {
          pCheckBox = static_cast<IFWL_CheckBox*>(radioarr[i]);
          if (pCheckBox != m_pInterface &&
              pCheckBox->GetStates() & FWL_STATE_CKB_Checked) {
            pCheckBox->SetCheckState(0);
            CFX_RectF rt;
            pCheckBox->GetWidgetRect(rt);
            rt.left = rt.top = 0;
            m_pWidgetMgr->RepaintWidget(pCheckBox, &rt);
            break;
          }
        }
      }
      m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
    }
  } else {
    if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
        FWL_STATE_CKB_Neutral) {
      m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State) {
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
      }
    } else if ((m_pProperties->m_dwStates & FWL_STATE_CKB_CheckMask) ==
               FWL_STATE_CKB_Checked) {
      m_pProperties->m_dwStates &= ~FWL_STATE_CKB_CheckMask;
    } else {
      if (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_CKB_3State) {
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Neutral;
      } else {
        m_pProperties->m_dwStates |= FWL_STATE_CKB_Checked;
      }
    }
  }
  Repaint(&m_rtClient);
  uint32_t dwLaststate = m_pProperties->m_dwStates;
  if (dwFirststate != dwLaststate) {
    CFWL_EvtCkbCheckStateChanged wmCheckBoxState;
    wmCheckBoxState.m_pSrcTarget = m_pInterface;
    DispatchEvent(&wmCheckBoxState);
  }
}
CFWL_CheckBoxImpDelegate::CFWL_CheckBoxImpDelegate(CFWL_CheckBoxImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_CheckBoxImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;

  switch (pMessage->GetClassID()) {
    case CFWL_MessageType::Activate: {
      OnActivate(pMessage);
      break;
    }
    case CFWL_MessageType::SetFocus: {
      OnFocusChanged(pMessage, TRUE);
      break;
    }
    case CFWL_MessageType::KillFocus: {
      OnFocusChanged(pMessage, FALSE);
      break;
    }
    case CFWL_MessageType::Mouse: {
      CFWL_MsgMouse* pMsg = static_cast<CFWL_MsgMouse*>(pMessage);
      switch (pMsg->m_dwCmd) {
        case FWL_MouseCommand::LeftButtonDown: {
          OnLButtonDown(pMsg);
          break;
        }
        case FWL_MouseCommand::LeftButtonUp: {
          OnLButtonUp(pMsg);
          break;
        }
        case FWL_MouseCommand::Move: {
          OnMouseMove(pMsg);
          break;
        }
        case FWL_MouseCommand::Leave: {
          OnMouseLeave(pMsg);
          break;
        }
        default:
          break;
      }
      break;
    }
    case CFWL_MessageType::Key: {
      CFWL_MsgKey* pKey = static_cast<CFWL_MsgKey*>(pMessage);
      if (pKey->m_dwCmd == FWL_KeyCommand::KeyDown)
        OnKeyDown(pKey);
      break;
    }
    default: {
      break;
    }
  }

  CFWL_WidgetImpDelegate::OnProcessMessage(pMessage);
}

void CFWL_CheckBoxImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                            const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}

void CFWL_CheckBoxImpDelegate::OnActivate(CFWL_Message* pMsg) {
  m_pOwner->m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Deactivated;
  m_pOwner->Repaint(&(m_pOwner->m_rtClient));
}
void CFWL_CheckBoxImpDelegate::OnFocusChanged(CFWL_Message* pMsg,
                                              FX_BOOL bSet) {
  if (bSet) {
    m_pOwner->m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
  }
  m_pOwner->Repaint(&(m_pOwner->m_rtClient));
}
void CFWL_CheckBoxImpDelegate::OnLButtonDown(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    return;
  }
  if ((m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0) {
    m_pOwner->SetFocus(TRUE);
  }
  m_pOwner->m_bBtnDown = TRUE;
  m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;
  m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Pressed;
  m_pOwner->Repaint(&(m_pOwner->m_rtClient));
}
void CFWL_CheckBoxImpDelegate::OnLButtonUp(CFWL_MsgMouse* pMsg) {
  if (!m_pOwner->m_bBtnDown) {
    return;
  }
  m_pOwner->m_bBtnDown = FALSE;
  if (!m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
    return;
  }
  m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
  m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Pressed;
  m_pOwner->NextStates();
}
void CFWL_CheckBoxImpDelegate::OnMouseMove(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    return;
  }
  FX_BOOL bRepaint = FALSE;
  if (m_pOwner->m_bBtnDown) {
    if (m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed) == 0) {
        bRepaint = TRUE;
        m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Pressed;
      }
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered)) {
        bRepaint = TRUE;
        m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;
      }
    } else {
      if (m_pOwner->m_pProperties->m_dwStates & FWL_STATE_CKB_Pressed) {
        bRepaint = TRUE;
        m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Pressed;
      }
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered) == 0) {
        bRepaint = TRUE;
        m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
      }
    }
  } else {
    if (m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_CKB_Hovered) == 0) {
        bRepaint = TRUE;
        m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
      }
    }
  }
  if (bRepaint) {
    m_pOwner->Repaint(&(m_pOwner->m_rtBox));
  }
}
void CFWL_CheckBoxImpDelegate::OnMouseLeave(CFWL_MsgMouse* pMsg) {
  if (m_pOwner->m_bBtnDown) {
    m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_CKB_Hovered;
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_CKB_Hovered;
  }
  m_pOwner->Repaint(&(m_pOwner->m_rtBox));
}
void CFWL_CheckBoxImpDelegate::OnKeyDown(CFWL_MsgKey* pMsg) {
  if (pMsg->m_dwKeyCode == FWL_VKEY_Tab) {
    m_pOwner->DispatchKeyEvent(pMsg);
    return;
  }
  if (pMsg->m_dwKeyCode == FWL_VKEY_Return ||
      pMsg->m_dwKeyCode == FWL_VKEY_Space) {
    m_pOwner->NextStates();
  }
}
