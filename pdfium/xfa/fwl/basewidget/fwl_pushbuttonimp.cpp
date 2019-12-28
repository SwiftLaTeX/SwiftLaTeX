// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_pushbuttonimp.h"

#include "xfa/fde/tto/fde_textout.h"
#include "xfa/fwl/basewidget/ifwl_pushbutton.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_themebackground.h"
#include "xfa/fwl/core/cfwl_themetext.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_themeprovider.h"

// static
IFWL_PushButton* IFWL_PushButton::Create(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter) {
  IFWL_PushButton* pPushButton = new IFWL_PushButton;
  CFWL_PushButtonImp* pPushButtonImpl =
      new CFWL_PushButtonImp(properties, pOuter);
  pPushButton->SetImpl(pPushButtonImpl);
  pPushButtonImpl->SetInterface(pPushButton);
  return pPushButton;
}
IFWL_PushButton::IFWL_PushButton() {}

CFWL_PushButtonImp::CFWL_PushButtonImp(
    const CFWL_WidgetImpProperties& properties,
    IFWL_Widget* pOuter)
    : CFWL_WidgetImp(properties, pOuter),
      m_bBtnDown(FALSE),
      m_dwTTOStyles(FDE_TTOSTYLE_SingleLine),
      m_iTTOAlign(FDE_TTOALIGNMENT_Center) {
  m_rtClient.Set(0, 0, 0, 0);
  m_rtCaption.Set(0, 0, 0, 0);
}

CFWL_PushButtonImp::~CFWL_PushButtonImp() {}

FWL_Error CFWL_PushButtonImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_PushButton;
  return FWL_Error::Succeeded;
}

FWL_Type CFWL_PushButtonImp::GetClassID() const {
  return FWL_Type::PushButton;
}

FWL_Error CFWL_PushButtonImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_Error::Succeeded)
    return FWL_Error::Indefinite;

  m_pDelegate = new CFWL_PushButtonImpDelegate(this);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_PushButtonImp::Finalize() {
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_Error CFWL_PushButtonImp::GetWidgetRect(CFX_RectF& rect,
                                            FX_BOOL bAutoSize) {
  if (bAutoSize) {
    rect.Set(0, 0, 0, 0);
    if (!m_pProperties->m_pThemeProvider) {
      m_pProperties->m_pThemeProvider = GetAvailableTheme();
    }
    CFX_WideString wsCaption;
    IFWL_PushButtonDP* pData =
        static_cast<IFWL_PushButtonDP*>(m_pProperties->m_pDataProvider);
    if (pData) {
      pData->GetCaption(m_pInterface, wsCaption);
    }
    int32_t iLen = wsCaption.GetLength();
    if (iLen > 0) {
      CFX_SizeF sz = CalcTextSize(wsCaption, m_pProperties->m_pThemeProvider);
      rect.Set(0, 0, sz.x, sz.y);
    }
    FX_FLOAT* fcaption =
        static_cast<FX_FLOAT*>(GetThemeCapacity(CFWL_WidgetCapacity::Margin));
    rect.Inflate(*fcaption, *fcaption);
    CFWL_WidgetImp::GetWidgetRect(rect, TRUE);
  } else {
    rect = m_pProperties->m_rtWidget;
  }
  return FWL_Error::Succeeded;
}

void CFWL_PushButtonImp::SetStates(uint32_t dwStates, FX_BOOL bSet) {
  if ((dwStates & FWL_WGTSTATE_Disabled) && bSet) {
    m_pProperties->m_dwStates = FWL_WGTSTATE_Disabled;
    return;
  }
  CFWL_WidgetImp::SetStates(dwStates, bSet);
}

FWL_Error CFWL_PushButtonImp::Update() {
  if (IsLocked()) {
    return FWL_Error::Indefinite;
  }
  if (!m_pProperties->m_pThemeProvider) {
    m_pProperties->m_pThemeProvider = GetAvailableTheme();
  }
  UpdateTextOutStyles();
  GetClientRect(m_rtClient);
  m_rtCaption = m_rtClient;
  FX_FLOAT* fcaption =
      static_cast<FX_FLOAT*>(GetThemeCapacity(CFWL_WidgetCapacity::Margin));
  m_rtCaption.Inflate(-*fcaption, -*fcaption);
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_PushButtonImp::DrawWidget(CFX_Graphics* pGraphics,
                                         const CFX_Matrix* pMatrix) {
  if (!pGraphics)
    return FWL_Error::Indefinite;
  if (!m_pProperties->m_pThemeProvider)
    return FWL_Error::Indefinite;
  IFWL_PushButtonDP* pData =
      static_cast<IFWL_PushButtonDP*>(m_pProperties->m_pDataProvider);
  CFX_DIBitmap* pPicture = nullptr;
  IFWL_ThemeProvider* pTheme = m_pProperties->m_pThemeProvider;
  if (HasBorder()) {
    DrawBorder(pGraphics, CFWL_Part::Border, m_pProperties->m_pThemeProvider,
               pMatrix);
  }
  if (HasEdge()) {
    DrawEdge(pGraphics, CFWL_Part::Edge, m_pProperties->m_pThemeProvider,
             pMatrix);
  }
  DrawBkground(pGraphics, m_pProperties->m_pThemeProvider, pMatrix);
  CFX_Matrix matrix;
  matrix.Concat(*pMatrix);
  FX_FLOAT iPicwidth = 0;
  FX_FLOAT ipicheight = 0;
  CFX_WideString wsCaption;
  if (pData) {
    pData->GetCaption(m_pInterface, wsCaption);
  }
  CFX_RectF rtText;
  rtText.Set(0, 0, 0, 0);
  if (!wsCaption.IsEmpty()) {
    CalcTextRect(wsCaption, pTheme, 0, m_iTTOAlign, rtText);
  }
  switch (m_pProperties->m_dwStyleExes & FWL_STYLEEXT_PSB_ModeMask) {
    case FWL_STYLEEXT_PSB_TextOnly:
      DrawText(pGraphics, m_pProperties->m_pThemeProvider, &matrix);
      break;
    case FWL_STYLEEXT_PSB_IconOnly:
      if (pData) {
        pPicture = pData->GetPicture(m_pInterface);
      }
      if (pPicture) {
        CFX_PointF point;
        switch (m_iTTOAlign) {
          case 0: {
            point.x = m_rtClient.left;
            point.y = m_rtClient.top;
            break;
          }
          case 1: {
            point.x = m_rtClient.left +
                      (m_rtClient.width / 2 - pPicture->GetWidth() / 2);
            point.y = m_rtClient.top;
            break;
          }
          case 2:
            point.x = m_rtClient.left + m_rtClient.width - pPicture->GetWidth();
            point.y = m_rtClient.top;
            break;
          case 4:
            point.x = m_rtClient.left;
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            break;
          case 5:
            point.x = m_rtClient.left +
                      (m_rtClient.width / 2 - pPicture->GetWidth() / 2);
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            break;
          case 6:
            point.x = m_rtClient.left + m_rtClient.width - pPicture->GetWidth();
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            break;
          case 8:
            point.x = m_rtClient.left;
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            break;
          case 9:
            point.x = m_rtClient.left +
                      (m_rtClient.width / 2 - pPicture->GetWidth() / 2);
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            break;
          case 10:
            point.x = m_rtClient.left + m_rtClient.width - pPicture->GetWidth();
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            break;
        }
        pGraphics->DrawImage(pPicture, point, &matrix);
      }
      break;
    case FWL_STYLEEXT_PSB_TextIcon:
      if (pPicture) {
        CFX_PointF point;
        switch (m_iTTOAlign) {
          case 0: {
            point.x = m_rtClient.left;
            point.y = m_rtClient.top;
            iPicwidth = (FX_FLOAT)(pPicture->GetWidth() - 7);
            ipicheight =
                pPicture->GetHeight() / 2 - m_rtCaption.top - rtText.height / 2;
            break;
          }
          case 1: {
            point.x =
                m_rtClient.left + (m_rtClient.width / 2 -
                                   (pPicture->GetWidth() + rtText.width) / 2);
            point.y = m_rtClient.top;
            iPicwidth = pPicture->GetWidth() -
                        ((m_rtClient.width) / 2 - rtText.width / 2 - point.x) +
                        rtText.width / 2 - 7;
            ipicheight =
                pPicture->GetHeight() / 2 - m_rtCaption.top - rtText.height / 2;
            break;
          }
          case 2:
            point.x = m_rtClient.left + m_rtClient.width -
                      pPicture->GetWidth() - rtText.width;
            point.y = m_rtClient.top;
            iPicwidth = m_rtClient.left + m_rtClient.width - point.x -
                        pPicture->GetWidth() - rtText.width + 7;
            ipicheight =
                pPicture->GetHeight() / 2 - m_rtCaption.top - rtText.height / 2;
            break;
          case 4:
            point.x = m_rtClient.left;
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            iPicwidth = m_rtClient.left + pPicture->GetWidth() - 7;
            break;
          case 5:
            point.x =
                m_rtClient.left + (m_rtClient.width / 2 -
                                   (pPicture->GetWidth() + rtText.width) / 2);
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            iPicwidth = pPicture->GetWidth() -
                        ((m_rtClient.width) / 2 - rtText.width / 2 - point.x) +
                        rtText.width / 2 - 7;
            break;
          case 6:
            point.x = m_rtClient.left + m_rtClient.width -
                      pPicture->GetWidth() - rtText.width;
            point.y = m_rtClient.top + m_rtClient.height / 2 -
                      pPicture->GetHeight() / 2;
            iPicwidth = m_rtClient.left + m_rtClient.width - point.x -
                        pPicture->GetWidth() - rtText.width + 7;
            break;
          case 8:
            point.x = m_rtClient.left;
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            iPicwidth = (FX_FLOAT)(pPicture->GetWidth() - 7);
            ipicheight -= rtText.height / 2;
            break;
          case 9:
            point.x =
                m_rtClient.left + (m_rtClient.width / 2 -
                                   (pPicture->GetWidth() + rtText.width) / 2);
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            iPicwidth = pPicture->GetWidth() -
                        ((m_rtClient.width) / 2 - rtText.width / 2 - point.x) +
                        rtText.width / 2 - 7;
            ipicheight -= rtText.height / 2;
            break;
          case 10:
            point.x = m_rtClient.left + m_rtClient.width -
                      pPicture->GetWidth() - rtText.width;
            point.y =
                m_rtClient.top + m_rtClient.height - pPicture->GetHeight();
            iPicwidth = m_rtClient.left + m_rtClient.width - point.x -
                        pPicture->GetWidth() - rtText.width + 7;
            ipicheight -= rtText.height / 2;
            break;
        }
        pGraphics->DrawImage(pPicture, point, &matrix);
      }
      matrix.e += m_rtClient.left + iPicwidth;
      matrix.f += m_rtClient.top + ipicheight;
      DrawText(pGraphics, m_pProperties->m_pThemeProvider, &matrix);
      break;
  }
  return FWL_Error::Succeeded;
}
void CFWL_PushButtonImp::DrawBkground(CFX_Graphics* pGraphics,
                                      IFWL_ThemeProvider* pTheme,
                                      const CFX_Matrix* pMatrix) {
  CFWL_ThemeBackground param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = CFWL_Part::Background;
  param.m_dwStates = GetPartStates();
  param.m_pGraphics = pGraphics;
  if (pMatrix) {
    param.m_matrix.Concat(*pMatrix);
  }
  param.m_rtPart = m_rtClient;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    param.m_pData = &m_rtCaption;
  }
  pTheme->DrawBackground(&param);
}
void CFWL_PushButtonImp::DrawText(CFX_Graphics* pGraphics,
                                  IFWL_ThemeProvider* pTheme,
                                  const CFX_Matrix* pMatrix) {
  if (!m_pProperties->m_pDataProvider)
    return;
  CFX_WideString wsCaption;
  m_pProperties->m_pDataProvider->GetCaption(m_pInterface, wsCaption);
  if (wsCaption.IsEmpty()) {
    return;
  }
  CFWL_ThemeText param;
  param.m_pWidget = m_pInterface;
  param.m_iPart = CFWL_Part::Caption;
  param.m_dwStates = GetPartStates();
  param.m_pGraphics = pGraphics;
  if (pMatrix) {
    param.m_matrix.Concat(*pMatrix);
  }
  param.m_rtPart = m_rtCaption;
  param.m_wsText = wsCaption;
  param.m_dwTTOStyles = m_dwTTOStyles;
  param.m_iTTOAlign = m_iTTOAlign;
  pTheme->DrawText(&param);
}
uint32_t CFWL_PushButtonImp::GetPartStates() {
  uint32_t dwStates = CFWL_PartState_Normal;
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) {
    dwStates |= CFWL_PartState_Focused;
  }
  if (m_pProperties->m_dwStates & FWL_WGTSTATE_Disabled) {
    dwStates = CFWL_PartState_Disabled;
  } else if (m_pProperties->m_dwStates & FWL_STATE_PSB_Pressed) {
    dwStates |= CFWL_PartState_Pressed;
  } else if (m_pProperties->m_dwStates & FWL_STATE_PSB_Hovered) {
    dwStates |= CFWL_PartState_Hovered;
  } else if (m_pProperties->m_dwStates & FWL_STATE_PSB_Default) {
    dwStates |= CFWL_PartState_Default;
  }
  return dwStates;
}
void CFWL_PushButtonImp::UpdateTextOutStyles() {
  m_iTTOAlign = FDE_TTOALIGNMENT_Center;
  switch (m_pProperties->m_dwStyleExes &
          (FWL_STYLEEXT_PSB_HLayoutMask | FWL_STYLEEXT_PSB_VLayoutMask)) {
    case FWL_STYLEEXT_PSB_Left | FWL_STYLEEXT_PSB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopLeft;
      break;
    }
    case FWL_STYLEEXT_PSB_Center | FWL_STYLEEXT_PSB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopCenter;
      break;
    }
    case FWL_STYLEEXT_PSB_Right | FWL_STYLEEXT_PSB_Top: {
      m_iTTOAlign = FDE_TTOALIGNMENT_TopRight;
      break;
    }
    case FWL_STYLEEXT_PSB_Left | FWL_STYLEEXT_PSB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_CenterLeft;
      break;
    }
    case FWL_STYLEEXT_PSB_Center | FWL_STYLEEXT_PSB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_Center;
      break;
    }
    case FWL_STYLEEXT_PSB_Right | FWL_STYLEEXT_PSB_VCenter: {
      m_iTTOAlign = FDE_TTOALIGNMENT_CenterRight;
      break;
    }
    case FWL_STYLEEXT_PSB_Left | FWL_STYLEEXT_PSB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomLeft;
      break;
    }
    case FWL_STYLEEXT_PSB_Center | FWL_STYLEEXT_PSB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomCenter;
      break;
    }
    case FWL_STYLEEXT_PSB_Right | FWL_STYLEEXT_PSB_Bottom: {
      m_iTTOAlign = FDE_TTOALIGNMENT_BottomRight;
      break;
    }
    default: {}
  }
  m_dwTTOStyles = FDE_TTOSTYLE_SingleLine;
  if (m_pProperties->m_dwStyleExes & FWL_WGTSTYLE_RTLReading) {
    m_dwTTOStyles |= FDE_TTOSTYLE_RTL;
  }
}

CFWL_PushButtonImpDelegate::CFWL_PushButtonImpDelegate(
    CFWL_PushButtonImp* pOwner)
    : m_pOwner(pOwner) {}

void CFWL_PushButtonImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  if (!pMessage)
    return;
  if (!m_pOwner->IsEnabled())
    return;

  CFWL_MessageType dwMsgCode = pMessage->GetClassID();
  switch (dwMsgCode) {
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

void CFWL_PushButtonImpDelegate::OnProcessEvent(CFWL_Event* pEvent) {}

void CFWL_PushButtonImpDelegate::OnDrawWidget(CFX_Graphics* pGraphics,
                                              const CFX_Matrix* pMatrix) {
  m_pOwner->DrawWidget(pGraphics, pMatrix);
}

void CFWL_PushButtonImpDelegate::OnFocusChanged(CFWL_Message* pMsg,
                                                FX_BOOL bSet) {
  if (bSet) {
    m_pOwner->m_pProperties->m_dwStates |= FWL_WGTSTATE_Focused;
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_WGTSTATE_Focused;
  }
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_PushButtonImpDelegate::OnLButtonDown(CFWL_MsgMouse* pMsg) {
  if ((m_pOwner->m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0) {
    m_pOwner->SetFocus(TRUE);
  }
  m_pOwner->m_bBtnDown = TRUE;
  m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Hovered;
  m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Pressed;
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_PushButtonImpDelegate::OnLButtonUp(CFWL_MsgMouse* pMsg) {
  m_pOwner->m_bBtnDown = FALSE;
  if (m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Pressed;
    m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Hovered;
  } else {
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Hovered;
    m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Pressed;
  }
  if (m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
    CFWL_EvtClick wmClick;
    wmClick.m_pSrcTarget = m_pOwner->m_pInterface;
    m_pOwner->DispatchEvent(&wmClick);
  }
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_PushButtonImpDelegate::OnMouseMove(CFWL_MsgMouse* pMsg) {
  FX_BOOL bRepaint = FALSE;
  if (m_pOwner->m_bBtnDown) {
    if (m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_PSB_Pressed) == 0) {
        m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Pressed;
        bRepaint = TRUE;
      }
      if (m_pOwner->m_pProperties->m_dwStates & FWL_STATE_PSB_Hovered) {
        m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Hovered;
        bRepaint = TRUE;
      }
    } else {
      if (m_pOwner->m_pProperties->m_dwStates & FWL_STATE_PSB_Pressed) {
        m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Pressed;
        bRepaint = TRUE;
      }
      if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_PSB_Hovered) == 0) {
        m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Hovered;
        bRepaint = TRUE;
      }
    }
  } else {
    if (!m_pOwner->m_rtClient.Contains(pMsg->m_fx, pMsg->m_fy)) {
      return;
    }
    if ((m_pOwner->m_pProperties->m_dwStates & FWL_STATE_PSB_Hovered) == 0) {
      m_pOwner->m_pProperties->m_dwStates |= FWL_STATE_PSB_Hovered;
      bRepaint = TRUE;
    }
  }
  if (bRepaint) {
    m_pOwner->Repaint(&m_pOwner->m_rtClient);
  }
}
void CFWL_PushButtonImpDelegate::OnMouseLeave(CFWL_MsgMouse* pMsg) {
  m_pOwner->m_bBtnDown = FALSE;
  m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Hovered;
  m_pOwner->m_pProperties->m_dwStates &= ~FWL_STATE_PSB_Pressed;
  m_pOwner->Repaint(&m_pOwner->m_rtClient);
}
void CFWL_PushButtonImpDelegate::OnKeyDown(CFWL_MsgKey* pMsg) {
  if (pMsg->m_dwKeyCode == FWL_VKEY_Return) {
    CFWL_EvtMouse wmMouse;
    wmMouse.m_pSrcTarget = m_pOwner->m_pInterface;
    wmMouse.m_dwCmd = FWL_MouseCommand::LeftButtonUp;
    m_pOwner->DispatchEvent(&wmMouse);
    CFWL_EvtClick wmClick;
    wmClick.m_pSrcTarget = m_pOwner->m_pInterface;
    m_pOwner->DispatchEvent(&wmClick);
    return;
  }
  if (pMsg->m_dwKeyCode != FWL_VKEY_Tab) {
    return;
  }
  m_pOwner->DispatchKeyEvent(pMsg);
}
