// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_TIMER_H_
#define XFA_FWL_CORE_IFWL_TIMER_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fwl/core/fwl_error.h"

class IFWL_TimerInfo;

class IFWL_Timer {
 public:
  virtual ~IFWL_Timer() {}
  virtual void Run(IFWL_TimerInfo* hTimer) = 0;
  IFWL_TimerInfo* StartTimer(uint32_t dwElapse, bool bImmediately);
};

class IFWL_TimerInfo {
 public:
  virtual ~IFWL_TimerInfo() {}
  FWL_Error StopTimer();
};

#endif  // XFA_FWL_CORE_IFWL_TIMER_H_
