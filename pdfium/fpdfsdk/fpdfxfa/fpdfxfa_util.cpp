// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"

#include <vector>

#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"

std::vector<CFWL_TimerInfo*>* CXFA_FWLAdapterTimerMgr::s_TimerArray = nullptr;

FWL_Error CXFA_FWLAdapterTimerMgr::Start(IFWL_Timer* pTimer,
                                         uint32_t dwElapse,
                                         bool bImmediately,
                                         IFWL_TimerInfo** pTimerInfo) {
  if (!m_pEnv)
    return FWL_Error::Indefinite;

  int32_t id_event = m_pEnv->FFI_SetTimer(dwElapse, TimerProc);
  if (!s_TimerArray)
    s_TimerArray = new std::vector<CFWL_TimerInfo*>;

  s_TimerArray->push_back(new CFWL_TimerInfo(id_event, pTimer));
  *pTimerInfo = s_TimerArray->back();
  return FWL_Error::Succeeded;
}

FWL_Error CXFA_FWLAdapterTimerMgr::Stop(IFWL_TimerInfo* pTimerInfo) {
  if (!pTimerInfo || !m_pEnv)
    return FWL_Error::Indefinite;

  CFWL_TimerInfo* pInfo = static_cast<CFWL_TimerInfo*>(pTimerInfo);
  m_pEnv->FFI_KillTimer(pInfo->idEvent);
  if (s_TimerArray) {
    auto it = std::find(s_TimerArray->begin(), s_TimerArray->end(), pInfo);
    if (it != s_TimerArray->end()) {
      s_TimerArray->erase(it);
      delete pInfo;
    }
  }
  return FWL_Error::Succeeded;
}

// static
void CXFA_FWLAdapterTimerMgr::TimerProc(int32_t idEvent) {
  if (!s_TimerArray)
    return;

  for (CFWL_TimerInfo* pInfo : *s_TimerArray) {
    if (pInfo->idEvent == idEvent) {
      pInfo->pTimer->Run(pInfo);
      break;
    }
  }
}
