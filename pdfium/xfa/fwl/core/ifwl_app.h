// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_APP_H_
#define XFA_FWL_CORE_IFWL_APP_H_

// The FWL app code contains three parallel classes, which reference each
// other via pointers as follows:
//
//                    m_pIface               m_pImpl
//      CXFA_FFApp ------------> IFWL_App -----------> CFWL_AppImp
//                                        <-----------
//                                           m_pIface

#include <memory>

#include "core/fxcrt/include/fx_string.h"
#include "xfa/fwl/core/fwl_appimp.h"
#include "xfa/fwl/core/fwl_error.h"

class CFWL_NoteDriver;
class CFWL_WidgetMgr;
class CXFA_FFApp;
class CXFA_FWLAdapterWidgetMgr;
class IFWL_ThemeProvider;
class IFWL_Widget;

class IFWL_App {
 public:
  static IFWL_App* Create(CXFA_FFApp* pAdapter);

  virtual ~IFWL_App();

  FWL_Error Initialize();
  FWL_Error Finalize();
  CXFA_FFApp* GetAdapterNative();
  CFWL_WidgetMgr* GetWidgetMgr();
  IFWL_ThemeProvider* GetThemeProvider();
  void SetThemeProvider(IFWL_ThemeProvider* pThemeProvider);
  void Exit(int32_t iExitCode);

  // These call into polymorphic methods in the impl; no need to override.
  void Release();

  CFWL_AppImp* GetImpl() const { return m_pImpl.get(); }

  // Takes ownership of |pImpl|.
  void SetImpl(CFWL_AppImp* pImpl) { m_pImpl.reset(pImpl); }

  CFWL_NoteDriver* GetNoteDriver() const;

 private:
  IFWL_App();

  std::unique_ptr<CFWL_AppImp> m_pImpl;
};

IFWL_App* FWL_GetApp();
void FWL_SetApp(IFWL_App* pApp);

CXFA_FFApp* FWL_GetAdapterNative();
CXFA_FWLAdapterWidgetMgr* FWL_GetAdapterWidgetMgr();

#endif  // XFA_FWL_CORE_IFWL_APP_H_
