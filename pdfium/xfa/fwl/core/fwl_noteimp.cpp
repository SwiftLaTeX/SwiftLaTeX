// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/core/fwl_noteimp.h"

#include "core/fxcrt/include/fx_ext.h"
#include "third_party/base/stl_util.h"
#include "xfa/fwl/basewidget/fwl_tooltipctrlimp.h"
#include "xfa/fwl/basewidget/ifwl_tooltip.h"
#include "xfa/fwl/core/cfwl_message.h"
#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/fwl_formimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"
#include "xfa/fwl/core/ifwl_app.h"

CFWL_NoteLoop::CFWL_NoteLoop(CFWL_WidgetImp* pForm)
    : m_pForm(pForm), m_bContinueModal(TRUE) {}

FWL_Error CFWL_NoteLoop::Idle(int32_t count) {
#if (_FX_OS_ == _FX_WIN32_DESKTOP_)
  if (count <= 0) {
#endif
    CFWL_EvtIdle ev;
    IFWL_App* pApp = FWL_GetApp();
    if (!pApp)
      return FWL_Error::Indefinite;
    CFWL_NoteDriver* pDriver = pApp->GetNoteDriver();
    if (!pDriver)
      return FWL_Error::Indefinite;
    pDriver->SendEvent(&ev);
#if (_FX_OS_ == _FX_WIN32_DESKTOP_)
  }
#endif
  return FWL_Error::Indefinite;
}
CFWL_WidgetImp* CFWL_NoteLoop::GetForm() {
  return m_pForm;
}
FX_BOOL CFWL_NoteLoop::ContinueModal() {
  return m_bContinueModal;
}
FWL_Error CFWL_NoteLoop::EndModalLoop() {
  m_bContinueModal = FALSE;
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_NoteLoop::SetMainForm(CFWL_WidgetImp* pForm) {
  m_pForm = pForm;
  return FWL_Error::Succeeded;
}
void CFWL_NoteLoop::GenerateCommondEvent(uint32_t dwCommand) {
  CFWL_EvtMenuCommand ev;
  ev.m_iCommand = dwCommand;
  IFWL_App* pApp = m_pForm->GetOwnerApp();
  if (!pApp)
    return;

  CFWL_NoteDriver* pDriver = pApp->GetNoteDriver();
  if (!pDriver)
    return;

  pDriver->SendEvent(&ev);
}
CFWL_NoteDriver::CFWL_NoteDriver()
    : m_pHover(nullptr),
      m_pFocus(nullptr),
      m_pGrab(nullptr),
      m_pNoteLoop(new CFWL_NoteLoop) {
  PushNoteLoop(m_pNoteLoop);
}
CFWL_NoteDriver::~CFWL_NoteDriver() {
  delete m_pNoteLoop;
  ClearInvalidEventTargets(TRUE);
}

void CFWL_NoteDriver::SendEvent(CFWL_Event* pNote) {
  if (m_eventTargets.empty())
    return;

  for (const auto& pair : m_eventTargets) {
    CFWL_EventTarget* pEventTarget = pair.second;
    if (pEventTarget && !pEventTarget->IsInvalid())
      pEventTarget->ProcessEvent(pNote);
  }
}

FWL_Error CFWL_NoteDriver::RegisterEventTarget(IFWL_Widget* pListener,
                                               IFWL_Widget* pEventSource,
                                               uint32_t dwFilter) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0) {
    do {
      key = rand();
    } while (key == 0 || pdfium::ContainsKey(m_eventTargets, key));
    pListener->SetEventKey(key);
  }
  if (!m_eventTargets[key])
    m_eventTargets[key] = new CFWL_EventTarget(this, pListener);

  m_eventTargets[key]->SetEventSource(pEventSource, dwFilter);
  return FWL_Error::Succeeded;
}

FWL_Error CFWL_NoteDriver::UnregisterEventTarget(IFWL_Widget* pListener) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0)
    return FWL_Error::Indefinite;

  auto it = m_eventTargets.find(key);
  if (it != m_eventTargets.end())
    it->second->FlagInvalid();

  return FWL_Error::Succeeded;
}

void CFWL_NoteDriver::ClearEventTargets(FX_BOOL bRemoveAll) {
  ClearInvalidEventTargets(bRemoveAll);
}
IFWL_App* CFWL_NoteDriver::GetOwnerApp() const {
  return FWL_GetApp();
}
FWL_Error CFWL_NoteDriver::PushNoteLoop(CFWL_NoteLoop* pNoteLoop) {
  m_noteLoopQueue.Add(pNoteLoop);
  return FWL_Error::Succeeded;
}
CFWL_NoteLoop* CFWL_NoteDriver::PopNoteLoop() {
  int32_t pos = m_noteLoopQueue.GetSize();
  if (pos <= 0)
    return nullptr;

  CFWL_NoteLoop* p = m_noteLoopQueue.GetAt(pos - 1);
  m_noteLoopQueue.RemoveAt(pos - 1);
  return p;
}
FX_BOOL CFWL_NoteDriver::SetFocus(IFWL_Widget* pFocus, FX_BOOL bNotify) {
  if (m_pFocus == pFocus) {
    return TRUE;
  }
  IFWL_Widget* pPrev = m_pFocus;
  m_pFocus = pFocus;
  if (pPrev) {
    CFWL_MsgKillFocus ms;
    ms.m_pDstTarget = pPrev;
    ms.m_pSrcTarget = pPrev;
    if (bNotify) {
      ms.m_dwExtend = 1;
    }
    IFWL_WidgetDelegate* pDelegate = pPrev->SetDelegate(nullptr);
    if (pDelegate) {
      pDelegate->OnProcessMessage(&ms);
    }
  }
  if (pFocus) {
    IFWL_Widget* pWidget =
        CFWL_WidgetMgr::GetInstance()->GetSystemFormWidget(pFocus);
    CFWL_FormImp* pForm =
        pWidget ? static_cast<CFWL_FormImp*>(pWidget->GetImpl()) : nullptr;
    if (pForm) {
      CFWL_WidgetImp* pNewFocus =
          static_cast<CFWL_WidgetImp*>(pFocus->GetImpl());
      pForm->SetSubFocus(pNewFocus);
    }
    CFWL_MsgSetFocus ms;
    ms.m_pDstTarget = pFocus;
    if (bNotify) {
      ms.m_dwExtend = 1;
    }
    IFWL_WidgetDelegate* pDelegate = pFocus->SetDelegate(nullptr);
    if (pDelegate) {
      pDelegate->OnProcessMessage(&ms);
    }
  }
  return TRUE;
}
FWL_Error CFWL_NoteDriver::Run() {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (!pWidgetMgr)
    return FWL_Error::Indefinite;

#if (_FX_OS_ == _FX_LINUX_DESKTOP_ || _FX_OS_ == _FX_WIN32_DESKTOP_ || \
     _FX_OS_ == _FX_WIN64_)
  CFWL_NoteLoop* pTopLoop = nullptr;
  for (;;) {
    pTopLoop = GetTopLoop();
    if (!pTopLoop || !pTopLoop->ContinueModal())
      break;
    if (UnqueueMessage(pTopLoop))
      continue;
  }
#endif

  return FWL_Error::Succeeded;
}

IFWL_Widget* CFWL_NoteDriver::GetFocus() {
  return m_pFocus;
}
IFWL_Widget* CFWL_NoteDriver::GetHover() {
  return m_pHover;
}
void CFWL_NoteDriver::SetHover(IFWL_Widget* pHover) {
  m_pHover = pHover;
}
void CFWL_NoteDriver::SetGrab(IFWL_Widget* pGrab, FX_BOOL bSet) {
  m_pGrab = bSet ? pGrab : nullptr;
}
void CFWL_NoteDriver::NotifyTargetHide(IFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget) {
    m_pFocus = nullptr;
  }
  if (m_pHover == pNoteTarget) {
    m_pHover = nullptr;
  }
  if (m_pGrab == pNoteTarget) {
    m_pGrab = nullptr;
  }
}
void CFWL_NoteDriver::NotifyTargetDestroy(IFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget) {
    m_pFocus = nullptr;
  }
  if (m_pHover == pNoteTarget) {
    m_pHover = nullptr;
  }
  if (m_pGrab == pNoteTarget) {
    m_pGrab = nullptr;
  }
  UnregisterEventTarget(pNoteTarget);
  int32_t count = m_forms.GetSize();
  for (int32_t nIndex = 0; nIndex < count; nIndex++) {
    CFWL_FormImp* pForm = static_cast<CFWL_FormImp*>(m_forms[nIndex]);
    if (!pForm) {
      continue;
    }
    CFWL_WidgetImp* pSubFocus = pForm->GetSubFocus();
    if (!pSubFocus)
      return;
    if (pSubFocus && pSubFocus->GetInterface() == pNoteTarget) {
      pForm->SetSubFocus(nullptr);
    }
  }
}

FWL_Error CFWL_NoteDriver::RegisterForm(CFWL_WidgetImp* pForm) {
  if (!pForm)
    return FWL_Error::Indefinite;
  if (m_forms.Find(pForm) >= 0) {
    return FWL_Error::Indefinite;
  }
  m_forms.Add(pForm);
  if (m_forms.GetSize() == 1) {
    CFWL_NoteLoop* pLoop =
        static_cast<CFWL_NoteLoop*>(m_noteLoopQueue.GetAt(0));
    if (!pLoop)
      return FWL_Error::Indefinite;
    pLoop->SetMainForm(pForm);
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_NoteDriver::UnRegisterForm(CFWL_WidgetImp* pForm) {
  if (!pForm)
    return FWL_Error::Indefinite;
  int32_t nIndex = m_forms.Find(pForm);
  if (nIndex < 0) {
    return FWL_Error::Indefinite;
  }
  m_forms.RemoveAt(nIndex);
  return FWL_Error::Succeeded;
}
FX_BOOL CFWL_NoteDriver::QueueMessage(CFWL_Message* pMessage) {
  pMessage->Retain();
  m_noteQueue.Add(pMessage);
  return TRUE;
}
FX_BOOL CFWL_NoteDriver::UnqueueMessage(CFWL_NoteLoop* pNoteLoop) {
  if (m_noteQueue.GetSize() < 1) {
    return FALSE;
  }
  CFWL_Message* pMessage = m_noteQueue[0];
  m_noteQueue.RemoveAt(0);
  if (!IsValidMessage(pMessage)) {
    pMessage->Release();
    return TRUE;
  }
  ProcessMessage(pMessage);

  pMessage->Release();
  return TRUE;
}
CFWL_NoteLoop* CFWL_NoteDriver::GetTopLoop() {
  int32_t size = m_noteLoopQueue.GetSize();
  if (size <= 0)
    return nullptr;
  return static_cast<CFWL_NoteLoop*>(m_noteLoopQueue[size - 1]);
}
int32_t CFWL_NoteDriver::CountLoop() {
  return m_noteLoopQueue.GetSize();
}

FX_BOOL CFWL_NoteDriver::ProcessMessage(CFWL_Message* pMessage) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  IFWL_Widget* pMessageForm = pWidgetMgr->IsFormDisabled()
                                  ? pMessage->m_pDstTarget
                                  : GetMessageForm(pMessage->m_pDstTarget);
  if (!pMessageForm)
    return FALSE;
  if (DispatchMessage(pMessage, pMessageForm)) {
    if (pMessage->GetClassID() == CFWL_MessageType::Mouse)
      MouseSecondary(static_cast<CFWL_MsgMouse*>(pMessage));
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CFWL_NoteDriver::DispatchMessage(CFWL_Message* pMessage,
                                         IFWL_Widget* pMessageForm) {
  FX_BOOL bRet = FALSE;
  switch (pMessage->GetClassID()) {
    case CFWL_MessageType::Activate: {
      bRet = DoActivate(static_cast<CFWL_MsgActivate*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::Deactivate: {
      bRet = DoDeactivate(static_cast<CFWL_MsgDeactivate*>(pMessage),
                          pMessageForm);
      break;
    }
    case CFWL_MessageType::SetFocus: {
      bRet = DoSetFocus(static_cast<CFWL_MsgSetFocus*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::KillFocus: {
      bRet =
          DoKillFocus(static_cast<CFWL_MsgKillFocus*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::Key: {
      bRet = DoKey(static_cast<CFWL_MsgKey*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::Mouse: {
      bRet = DoMouse(static_cast<CFWL_MsgMouse*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::MouseWheel: {
      bRet = DoWheel(static_cast<CFWL_MsgMouseWheel*>(pMessage), pMessageForm);
      break;
    }
    case CFWL_MessageType::Size: {
      bRet = DoSize(static_cast<CFWL_MsgSize*>(pMessage));
      break;
    }
    case CFWL_MessageType::Cursor: {
      bRet = TRUE;
      break;
    }
    case CFWL_MessageType::WindowMove: {
      bRet = DoWindowMove(static_cast<CFWL_MsgWindowMove*>(pMessage),
                          pMessageForm);
      break;
    }
    case CFWL_MessageType::DropFiles: {
      bRet =
          DoDragFiles(static_cast<CFWL_MsgDropFiles*>(pMessage), pMessageForm);
      break;
    }
    default: {
      bRet = TRUE;
      break;
    }
  }
  if (bRet) {
    IFWL_WidgetDelegate* pDelegate =
        pMessage->m_pDstTarget->SetDelegate(nullptr);
    if (pDelegate)
      pDelegate->OnProcessMessage(pMessage);
  }
  return bRet;
}

FX_BOOL CFWL_NoteDriver::DoActivate(CFWL_MsgActivate* pMsg,
                                    IFWL_Widget* pMessageForm) {
  pMsg->m_pDstTarget = pMessageForm;
  return (pMsg->m_pDstTarget)->GetStates() & FWL_WGTSTATE_Deactivated;
}
FX_BOOL CFWL_NoteDriver::DoDeactivate(CFWL_MsgDeactivate* pMsg,
                                      IFWL_Widget* pMessageForm) {
  int32_t iTrackLoop = m_noteLoopQueue.GetSize();
  if (iTrackLoop <= 0)
    return FALSE;
  if (iTrackLoop == 1) {
    if (pMessageForm->IsInstance(FX_WSTRC(L"FWL_FORMPROXY"))) {
      return FALSE;
    }
    if (pMsg->m_pSrcTarget &&
        pMsg->m_pSrcTarget->IsInstance(FX_WSTRC(L"FWL_FORMPROXY"))) {
      return FALSE;
    }
    if (pMsg->m_pSrcTarget &&
        pMsg->m_pSrcTarget->GetClassID() == FWL_Type::ToolTip) {
      return FALSE;
    }
    return TRUE;
  }
  IFWL_Widget* pDst = pMsg->m_pDstTarget;
  if (!pDst)
    return FALSE;
#if (_FX_OS_ == _FX_MACOSX_)
  if (pDst == pMessageForm && pDst->IsInstance(L"FWL_FORMPROXY")) {
    return TRUE;
  }
#endif
  return pDst != pMessageForm &&
         !pDst->IsInstance(FX_WSTRC(L"FWL_FORMPROXY")) &&
         !pMessageForm->IsInstance(FX_WSTRC(L"FWL_FORMPROXY"));
}
FX_BOOL CFWL_NoteDriver::DoSetFocus(CFWL_MsgSetFocus* pMsg,
                                    IFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (pWidgetMgr->IsFormDisabled()) {
    m_pFocus = pMsg->m_pDstTarget;
    return TRUE;
  }
  IFWL_Widget* pWidget = pMsg->m_pDstTarget;
  CFWL_FormImp* pForm =
      pWidget ? static_cast<CFWL_FormImp*>(pWidget->GetImpl()) : nullptr;
  if (pForm) {
    CFWL_WidgetImp* pSubFocus = pForm->GetSubFocus();
    if (pSubFocus && ((pSubFocus->GetStates() & FWL_WGTSTATE_Focused) == 0)) {
      pMsg->m_pDstTarget = pSubFocus->GetInterface();
      if (m_pFocus != pMsg->m_pDstTarget) {
        m_pFocus = pMsg->m_pDstTarget;
        return TRUE;
      }
    }
  }
  return FALSE;
}
FX_BOOL CFWL_NoteDriver::DoKillFocus(CFWL_MsgKillFocus* pMsg,
                                     IFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (pWidgetMgr->IsFormDisabled()) {
    if (m_pFocus == pMsg->m_pDstTarget) {
      m_pFocus = nullptr;
    }
    return TRUE;
  }
  IFWL_Widget* pWidget = pMsg->m_pDstTarget;
  CFWL_FormImp* pForm =
      pWidget ? static_cast<CFWL_FormImp*>(pWidget->GetImpl()) : nullptr;
  if (pForm) {
    CFWL_WidgetImp* pSubFocus = pForm->GetSubFocus();
    if (pSubFocus && (pSubFocus->GetStates() & FWL_WGTSTATE_Focused)) {
      pMsg->m_pDstTarget = pSubFocus->GetInterface();
      if (m_pFocus == pMsg->m_pDstTarget) {
        m_pFocus = nullptr;
        return TRUE;
      }
    }
  }
  return FALSE;
}
FX_BOOL CFWL_NoteDriver::DoKey(CFWL_MsgKey* pMsg, IFWL_Widget* pMessageForm) {
#if (_FX_OS_ != _FX_MACOSX_)
  if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
      pMsg->m_dwKeyCode == FWL_VKEY_Tab) {
    CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
    IFWL_Widget* pForm = GetMessageForm(pMsg->m_pDstTarget);
    IFWL_Widget* pFocus = m_pFocus;
    if (m_pFocus) {
      if (pWidgetMgr->GetSystemFormWidget(m_pFocus) != pForm)
        pFocus = nullptr;
    }
    FX_BOOL bFind = FALSE;
    IFWL_Widget* pNextTabStop = pWidgetMgr->nextTab(pForm, pFocus, bFind);
    if (!pNextTabStop) {
      bFind = FALSE;
      pNextTabStop = pWidgetMgr->nextTab(pForm, nullptr, bFind);
    }
    if (pNextTabStop == pFocus) {
      return TRUE;
    }
    if (pNextTabStop) {
      SetFocus(pNextTabStop);
    }
    return TRUE;
  }
#endif
  if (!m_pFocus) {
    if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
        pMsg->m_dwKeyCode == FWL_VKEY_Return) {
      CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
      IFWL_Widget* defButton = pWidgetMgr->GetDefaultButton(pMessageForm);
      if (defButton) {
        pMsg->m_pDstTarget = defButton;
        return TRUE;
      }
    }
    return FALSE;
  }
  pMsg->m_pDstTarget = m_pFocus;
  return TRUE;
}
FX_BOOL CFWL_NoteDriver::DoMouse(CFWL_MsgMouse* pMsg,
                                 IFWL_Widget* pMessageForm) {
  if (pMsg->m_dwCmd == FWL_MouseCommand::Leave ||
      pMsg->m_dwCmd == FWL_MouseCommand::Hover ||
      pMsg->m_dwCmd == FWL_MouseCommand::Enter) {
    return !!pMsg->m_pDstTarget;
  }
  if (pMsg->m_pDstTarget != pMessageForm) {
    pMsg->m_pDstTarget->TransformTo(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  }
  if (!DoMouseEx(pMsg, pMessageForm)) {
    pMsg->m_pDstTarget = pMessageForm;
  }
  return TRUE;
}
FX_BOOL CFWL_NoteDriver::DoWheel(CFWL_MsgMouseWheel* pMsg,
                                 IFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (!pWidgetMgr)
    return FALSE;

  IFWL_Widget* pDst =
      pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  if (!pDst)
    return FALSE;

  pMessageForm->TransformTo(pDst, pMsg->m_fx, pMsg->m_fy);
  pMsg->m_pDstTarget = pDst;
  return TRUE;
}
FX_BOOL CFWL_NoteDriver::DoSize(CFWL_MsgSize* pMsg) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (!pWidgetMgr)
    return FALSE;
  pWidgetMgr->NotifySizeChanged(pMsg->m_pDstTarget, (FX_FLOAT)pMsg->m_iWidth,
                                (FX_FLOAT)pMsg->m_iHeight);
  return TRUE;
}
FX_BOOL CFWL_NoteDriver::DoWindowMove(CFWL_MsgWindowMove* pMsg,
                                      IFWL_Widget* pMessageForm) {
  return pMsg->m_pDstTarget == pMessageForm;
}
FX_BOOL CFWL_NoteDriver::DoDragFiles(CFWL_MsgDropFiles* pMsg,
                                     IFWL_Widget* pMessageForm) {
  return pMsg->m_pDstTarget == pMessageForm;
}
FX_BOOL CFWL_NoteDriver::DoMouseEx(CFWL_MsgMouse* pMsg,
                                   IFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
  if (!pWidgetMgr)
    return FALSE;
  IFWL_Widget* pTarget = nullptr;
  if (m_pGrab)
    pTarget = m_pGrab;
  if (!pTarget) {
    pTarget =
        pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_fx, pMsg->m_fy);
  }
  if (pTarget) {
    if (pMessageForm != pTarget) {
      pMessageForm->TransformTo(pTarget, pMsg->m_fx, pMsg->m_fy);
    }
  }
  if (!pTarget)
    return FALSE;
  pMsg->m_pDstTarget = pTarget;
  return TRUE;
}
void CFWL_NoteDriver::MouseSecondary(CFWL_MsgMouse* pMsg) {
  IFWL_Widget* pTarget = pMsg->m_pDstTarget;
  if (pTarget == m_pHover) {
    return;
  }
  if (m_pHover) {
    CFWL_MsgMouse msLeave;
    msLeave.m_pDstTarget = m_pHover;
    msLeave.m_fx = pMsg->m_fx;
    msLeave.m_fy = pMsg->m_fy;
    pTarget->TransformTo(m_pHover, msLeave.m_fx, msLeave.m_fy);
    msLeave.m_dwFlags = 0;
    msLeave.m_dwCmd = FWL_MouseCommand::Leave;
    DispatchMessage(&msLeave, nullptr);
  }
  if (pTarget->GetClassID() == FWL_Type::Form) {
    m_pHover = nullptr;
    return;
  }
  m_pHover = pTarget;
  CFWL_MsgMouse msHover;
  msHover.m_pDstTarget = pTarget;
  msHover.m_fx = pMsg->m_fx;
  msHover.m_fy = pMsg->m_fy;
  msHover.m_dwFlags = 0;
  msHover.m_dwCmd = FWL_MouseCommand::Hover;
  DispatchMessage(&msHover, nullptr);
}
FX_BOOL CFWL_NoteDriver::IsValidMessage(CFWL_Message* pMessage) {
  if (pMessage->GetClassID() == CFWL_MessageType::Post)
    return TRUE;

  int32_t iCount = m_noteLoopQueue.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CFWL_NoteLoop* pNoteLoop = static_cast<CFWL_NoteLoop*>(m_noteLoopQueue[i]);
    CFWL_WidgetImp* pForm = pNoteLoop->GetForm();
    if (pForm && (pForm->GetInterface() == pMessage->m_pDstTarget))
      return TRUE;
  }
  iCount = m_forms.GetSize();
  for (int32_t j = 0; j < iCount; j++) {
    CFWL_FormImp* pForm = static_cast<CFWL_FormImp*>(m_forms[j]);
    if (pForm->GetInterface() == pMessage->m_pDstTarget)
      return TRUE;
  }
  return FALSE;
}

IFWL_Widget* CFWL_NoteDriver::GetMessageForm(IFWL_Widget* pDstTarget) {
  int32_t iTrackLoop = m_noteLoopQueue.GetSize();
  if (iTrackLoop <= 0)
    return nullptr;
  IFWL_Widget* pMessageForm = nullptr;
  if (iTrackLoop > 1) {
    CFWL_NoteLoop* pNootLoop =
        static_cast<CFWL_NoteLoop*>(m_noteLoopQueue[iTrackLoop - 1]);
    pMessageForm = pNootLoop->GetForm()->GetInterface();
  } else if (m_forms.Find(pDstTarget->GetImpl()) < 0) {
    pMessageForm = pDstTarget;
  }
  if (!pMessageForm && pDstTarget) {
    CFWL_WidgetMgr* pWidgetMgr = CFWL_WidgetMgr::GetInstance();
    if (!pWidgetMgr)
      return nullptr;
    pMessageForm = pWidgetMgr->GetSystemFormWidget(pDstTarget);
  }
  return pMessageForm;
}

void CFWL_NoteDriver::ClearInvalidEventTargets(FX_BOOL bRemoveAll) {
  auto it = m_eventTargets.begin();
  while (it != m_eventTargets.end()) {
    auto old = it++;
    if (old->second && (bRemoveAll || old->second->IsInvalid())) {
      delete old->second;
      m_eventTargets.erase(old);
    }
  }
}

class CFWL_CoreToolTipDP : public IFWL_ToolTipDP {
 public:
  // IFWL_ToolTipDP
  FWL_Error GetCaption(IFWL_Widget* pWidget,
                       CFX_WideString& wsCaption) override;
  int32_t GetInitialDelay(IFWL_Widget* pWidget) override;
  int32_t GetAutoPopDelay(IFWL_Widget* pWidget) override;
  CFX_DIBitmap* GetToolTipIcon(IFWL_Widget* pWidget) override;
  CFX_SizeF GetToolTipIconSize(IFWL_Widget* pWidget) override;

  CFX_RectF GetAnchor();
  CFWL_CoreToolTipDP();

  CFX_WideString m_wsCaption;
  int32_t m_nInitDelayTime;
  int32_t m_nAutoPopDelayTime;
  CFX_RectF m_fAnchor;
};

CFWL_CoreToolTipDP::CFWL_CoreToolTipDP() {
  m_nInitDelayTime = 500;
  m_nAutoPopDelayTime = 50000;
  m_fAnchor.Set(0.0, 0.0, 0.0, 0.0);
}

FWL_Error CFWL_CoreToolTipDP::GetCaption(IFWL_Widget* pWidget,
                                         CFX_WideString& wsCaption) {
  wsCaption = m_wsCaption;
  return FWL_Error::Succeeded;
}

int32_t CFWL_CoreToolTipDP::GetInitialDelay(IFWL_Widget* pWidget) {
  return m_nInitDelayTime;
}

int32_t CFWL_CoreToolTipDP::GetAutoPopDelay(IFWL_Widget* pWidget) {
  return m_nAutoPopDelayTime;
}

CFX_DIBitmap* CFWL_CoreToolTipDP::GetToolTipIcon(IFWL_Widget* pWidget) {
  return nullptr;
}

CFX_SizeF CFWL_CoreToolTipDP::GetToolTipIconSize(IFWL_Widget* pWidget) {
  return CFX_SizeF();
}

CFX_RectF CFWL_CoreToolTipDP::GetAnchor() {
  return m_fAnchor;
}

CFWL_EventTarget::CFWL_EventTarget(CFWL_NoteDriver* pNoteDriver,
                                   IFWL_Widget* pListener)
    : m_pListener(pListener), m_pNoteDriver(pNoteDriver), m_bInvalid(FALSE) {}
CFWL_EventTarget::~CFWL_EventTarget() {
  m_eventSources.RemoveAll();
}

int32_t CFWL_EventTarget::SetEventSource(IFWL_Widget* pSource,
                                         uint32_t dwFilter) {
  if (pSource) {
    m_eventSources.SetAt(pSource, dwFilter);
    return m_eventSources.GetCount();
  }
  return 1;
}

FX_BOOL CFWL_EventTarget::ProcessEvent(CFWL_Event* pEvent) {
  IFWL_WidgetDelegate* pDelegate = m_pListener->SetDelegate(nullptr);
  if (!pDelegate)
    return FALSE;
  if (m_eventSources.GetCount() == 0) {
    pDelegate->OnProcessEvent(pEvent);
    return TRUE;
  }
  FX_POSITION pos = m_eventSources.GetStartPosition();
  while (pos) {
    IFWL_Widget* pSource = nullptr;
    uint32_t dwFilter = 0;
    m_eventSources.GetNextAssoc(pos, (void*&)pSource, dwFilter);
    if (pSource == pEvent->m_pSrcTarget ||
        pEvent->GetClassID() == CFWL_EventType::Idle) {
      if (IsFilterEvent(pEvent, dwFilter)) {
        pDelegate->OnProcessEvent(pEvent);
        return TRUE;
      }
    }
  }
  return FALSE;
}

FX_BOOL CFWL_EventTarget::IsFilterEvent(CFWL_Event* pEvent, uint32_t dwFilter) {
  if (dwFilter == FWL_EVENT_ALL_MASK)
    return TRUE;

  switch (pEvent->GetClassID()) {
    case CFWL_EventType::Mouse:
      return !!(dwFilter & FWL_EVENT_MOUSE_MASK);
    case CFWL_EventType::MouseWheel:
      return !!(dwFilter & FWL_EVENT_MOUSEWHEEL_MASK);
    case CFWL_EventType::Key:
      return !!(dwFilter & FWL_EVENT_KEY_MASK);
    case CFWL_EventType::SetFocus:
    case CFWL_EventType::KillFocus:
      return !!(dwFilter & FWL_EVENT_FOCUSCHANGED_MASK);
    case CFWL_EventType::Draw:
      return !!(dwFilter & FWL_EVENT_DRAW_MASK);
    case CFWL_EventType::Close:
      return !!(dwFilter & FWL_EVENT_CLOSE_MASK);
    case CFWL_EventType::SizeChanged:
      return !!(dwFilter & FWL_EVENT_SIZECHANGED_MASK);
    case CFWL_EventType::Idle:
      return !!(dwFilter & FWL_EVENT_IDLE_MASK);
    default:
      return !!(dwFilter & FWL_EVENT_CONTROL_MASK);
  }
}

CFWL_ToolTipContainer* CFWL_ToolTipContainer::s_pInstance = nullptr;

CFWL_ToolTipContainer::CFWL_ToolTipContainer() : m_pToolTipImp(nullptr) {
  m_ToolTipDp = new CFWL_CoreToolTipDP;
  m_ToolTipDp->m_nInitDelayTime = 0;
  m_ToolTipDp->m_nAutoPopDelayTime = 2000;
}
CFWL_ToolTipContainer::~CFWL_ToolTipContainer() {
  if (m_pToolTipImp) {
    IFWL_ToolTip* pToolTip =
        static_cast<IFWL_ToolTip*>(m_pToolTipImp->GetInterface());
    pToolTip->Finalize();
    delete pToolTip;
  }
  delete m_ToolTipDp;
}

// static
CFWL_ToolTipContainer* CFWL_ToolTipContainer::getInstance() {
  if (!s_pInstance)
    s_pInstance = new CFWL_ToolTipContainer;
  return s_pInstance;
}

// static
void CFWL_ToolTipContainer::DeleteInstance() {
  delete s_pInstance;
  s_pInstance = nullptr;
}
