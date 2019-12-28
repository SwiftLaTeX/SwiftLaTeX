// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_UTIL_H_
#define FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_UTIL_H_

#include <vector>

#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "xfa/fwl/core/include/ifwl_adaptertimermgr.h"

#define JS_STR_VIEWERTYPE_STANDARD L"Exchange"
#define JS_STR_LANGUANGE L"ENU"
#define JS_STR_VIEWERVARIATION L"Full"
#define JS_STR_VIEWERVERSION_XFA L"11"

struct CFWL_TimerInfo;

class CXFA_FWLAdapterTimerMgr : public IFWL_AdapterTimerMgr {
 public:
  CXFA_FWLAdapterTimerMgr(CPDFDoc_Environment* pEnv) : m_pEnv(pEnv) {}

  FWL_Error Start(IFWL_Timer* pTimer,
                  uint32_t dwElapse,
                  bool bImmediately,
                  IFWL_TimerInfo** pTimerInfo) override;
  FWL_Error Stop(IFWL_TimerInfo* pTimerInfo) override;

 protected:
  static void TimerProc(int32_t idEvent);

  static std::vector<CFWL_TimerInfo*>* s_TimerArray;
  CPDFDoc_Environment* const m_pEnv;
};

struct CFWL_TimerInfo : public IFWL_TimerInfo {
  CFWL_TimerInfo() : pTimer(nullptr) {}
  CFWL_TimerInfo(int32_t event, IFWL_Timer* timer)
      : idEvent(event), pTimer(timer) {}

  int32_t idEvent;
  IFWL_Timer* pTimer;
};

#endif  // FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_UTIL_H_
