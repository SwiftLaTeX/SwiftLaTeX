// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_FWL_APPIMP_H_
#define XFA_FWL_CORE_FWL_APPIMP_H_

#include <memory>

#include "xfa/fwl/core/fwl_noteimp.h"

class CFWL_WidgetMgr;
class CXFA_FFApp;
class IFWL_App;
class IFWL_NoteThread;
class IFWL_ThemeProvider;

class CFWL_AppImp {
 public:
  CFWL_AppImp(IFWL_App* pIface, CXFA_FFApp* pAdapter);
  ~CFWL_AppImp();

  IFWL_App* GetInterface() const { return m_pIface; }
  CFWL_NoteDriver* GetNoteDriver() const { return m_pNoteDriver.get(); }

  FWL_Error Initialize();
  FWL_Error Finalize();
  CXFA_FFApp* GetAdapterNative() const;
  CFWL_WidgetMgr* GetWidgetMgr() const;
  IFWL_ThemeProvider* GetThemeProvider() const;
  void SetThemeProvider(IFWL_ThemeProvider* pThemeProvider);
  void Exit(int32_t iExitCode);

 private:
  CXFA_FFApp* const m_pAdapterNative;
  std::unique_ptr<CFWL_WidgetMgr> m_pWidgetMgr;
  IFWL_ThemeProvider* m_pThemeProvider;
  std::unique_ptr<CFWL_NoteDriver> m_pNoteDriver;
  IFWL_App* const m_pIface;
};

#endif  // XFA_FWL_CORE_FWL_APPIMP_H_
