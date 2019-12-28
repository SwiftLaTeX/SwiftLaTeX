// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/core/ifwl_timer.h"
#include "xfa/fwl/core/include/ifwl_adaptertimermgr.h"
#include "xfa/fxfa/include/xfa_ffapp.h"

IFWL_TimerInfo* IFWL_Timer::StartTimer(uint32_t dwElapse, bool bImmediately) {
  CXFA_FFApp* pAdapterNative = FWL_GetAdapterNative();
  if (!pAdapterNative)
    return nullptr;

  IFWL_AdapterTimerMgr* pAdapterTimerMgr = pAdapterNative->GetTimerMgr();
  if (!pAdapterTimerMgr)
    return nullptr;

  IFWL_TimerInfo* pTimerInfo = nullptr;
  pAdapterTimerMgr->Start(this, dwElapse, bImmediately, &pTimerInfo);
  return pTimerInfo;
}

FWL_Error IFWL_TimerInfo::StopTimer() {
  CXFA_FFApp* pAdapterNative = FWL_GetAdapterNative();
  if (!pAdapterNative)
    return FWL_Error::Indefinite;

  IFWL_AdapterTimerMgr* pAdapterTimerMgr = pAdapterNative->GetTimerMgr();
  if (!pAdapterTimerMgr)
    return FWL_Error::Indefinite;

  return pAdapterTimerMgr->Stop(this);
}
