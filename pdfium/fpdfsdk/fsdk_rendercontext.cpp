// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/include/fsdk_rendercontext.h"

#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fxge/include/fx_ge.h"

void CRenderContext::Clear() {
  m_pDevice = nullptr;
  m_pContext = nullptr;
  m_pRenderer = nullptr;
  m_pAnnots = nullptr;
  m_pOptions = nullptr;
}

CRenderContext::~CRenderContext() {
  delete m_pRenderer;
  delete m_pContext;
  delete m_pDevice;
  delete m_pAnnots;
  delete m_pOptions->m_pOCContext;
  delete m_pOptions;
}

IFSDK_PAUSE_Adapter::IFSDK_PAUSE_Adapter(IFSDK_PAUSE* IPause) {
  m_IPause = IPause;
}

IFSDK_PAUSE_Adapter::~IFSDK_PAUSE_Adapter() {}

FX_BOOL IFSDK_PAUSE_Adapter::NeedToPauseNow() {
  return m_IPause->NeedToPauseNow && m_IPause->NeedToPauseNow(m_IPause);
}
