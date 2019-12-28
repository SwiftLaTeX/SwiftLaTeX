// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/core/fwl_appimp.h"

#include "xfa/fwl/core/cfwl_widgetmgr.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/ifwl_app.h"
#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"

IFWL_App* IFWL_App::Create(CXFA_FFApp* pAdapter) {
  IFWL_App* pApp = new IFWL_App;
  pApp->SetImpl(new CFWL_AppImp(pApp, pAdapter));
  return pApp;
}

IFWL_App::IFWL_App() {}

IFWL_App::~IFWL_App() {}

void IFWL_App::Release() {}

FWL_Error IFWL_App::Initialize() {
  return static_cast<CFWL_AppImp*>(GetImpl())->Initialize();
}

FWL_Error IFWL_App::Finalize() {
  return static_cast<CFWL_AppImp*>(GetImpl())->Finalize();
}

CXFA_FFApp* IFWL_App::GetAdapterNative() {
  return static_cast<CFWL_AppImp*>(GetImpl())->GetAdapterNative();
}

CFWL_WidgetMgr* IFWL_App::GetWidgetMgr() {
  return static_cast<CFWL_AppImp*>(GetImpl())->GetWidgetMgr();
}

IFWL_ThemeProvider* IFWL_App::GetThemeProvider() {
  return static_cast<CFWL_AppImp*>(GetImpl())->GetThemeProvider();
}

void IFWL_App::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  static_cast<CFWL_AppImp*>(GetImpl())->SetThemeProvider(pThemeProvider);
}

void IFWL_App::Exit(int32_t iExitCode) {
  static_cast<CFWL_AppImp*>(GetImpl())->Exit(iExitCode);
}

CFWL_NoteDriver* IFWL_App::GetNoteDriver() const {
  return static_cast<CFWL_AppImp*>(GetImpl())->GetNoteDriver();
}

CFWL_AppImp::CFWL_AppImp(IFWL_App* pIface, CXFA_FFApp* pAdapter)
    : m_pAdapterNative(pAdapter),
      m_pThemeProvider(nullptr),
      m_pNoteDriver(new CFWL_NoteDriver),
      m_pIface(pIface) {}

CFWL_AppImp::~CFWL_AppImp() {
  CFWL_ToolTipContainer::DeleteInstance();
}

FWL_Error CFWL_AppImp::Initialize() {
  if (!m_pWidgetMgr) {
    m_pWidgetMgr.reset(new CFWL_WidgetMgr(m_pAdapterNative));
  }
  return FWL_Error::Succeeded;
}
FWL_Error CFWL_AppImp::Finalize() {
  m_pWidgetMgr.reset();
  return FWL_Error::Succeeded;
}
CXFA_FFApp* CFWL_AppImp::GetAdapterNative() const {
  return m_pAdapterNative;
}
CXFA_FWLAdapterWidgetMgr* FWL_GetAdapterWidgetMgr() {
  return CFWL_WidgetMgr::GetInstance()->GetAdapterWidgetMgr();
}
CFWL_WidgetMgr* CFWL_AppImp::GetWidgetMgr() const {
  return m_pWidgetMgr.get();
}

void CFWL_AppImp::SetThemeProvider(IFWL_ThemeProvider* pThemeProvider) {
  m_pThemeProvider = pThemeProvider;
}

void CFWL_AppImp::Exit(int32_t iExitCode) {
  while (m_pNoteDriver->PopNoteLoop()) {
    continue;
  }
}

IFWL_ThemeProvider* CFWL_AppImp::GetThemeProvider() const {
  return m_pThemeProvider;
}

CXFA_FFApp* FWL_GetAdapterNative() {
  IFWL_App* pApp = FWL_GetApp();
  if (!pApp)
    return nullptr;
  return pApp->GetAdapterNative();
}

static IFWL_App* g_theApp = nullptr;
IFWL_App* FWL_GetApp() {
  return g_theApp;
}

void FWL_SetApp(IFWL_App* pApp) {
  g_theApp = pApp;
}
