// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/include/cpdf_modulemgr.h"

#include "core/fpdfapi/fpdf_page/cpdf_pagemodule.h"
#include "core/fxcodec/include/fx_codec.h"

namespace {

CPDF_ModuleMgr* g_FPDFAPI_pDefaultMgr = nullptr;

}  // namespace

// static
CPDF_ModuleMgr* CPDF_ModuleMgr::Get() {
  return g_FPDFAPI_pDefaultMgr;
}

// static
void CPDF_ModuleMgr::Create() {
  ASSERT(!g_FPDFAPI_pDefaultMgr);
  g_FPDFAPI_pDefaultMgr = new CPDF_ModuleMgr;
}

// static
void CPDF_ModuleMgr::Destroy() {
  delete g_FPDFAPI_pDefaultMgr;
  g_FPDFAPI_pDefaultMgr = nullptr;
}

CPDF_ModuleMgr::CPDF_ModuleMgr() : m_pCodecModule(nullptr) {}

CPDF_ModuleMgr::~CPDF_ModuleMgr() {}

CCodec_FaxModule* CPDF_ModuleMgr::GetFaxModule() {
  return m_pCodecModule ? m_pCodecModule->GetFaxModule() : nullptr;
}

CCodec_JpegModule* CPDF_ModuleMgr::GetJpegModule() {
  return m_pCodecModule ? m_pCodecModule->GetJpegModule() : nullptr;
}

CCodec_JpxModule* CPDF_ModuleMgr::GetJpxModule() {
  return m_pCodecModule ? m_pCodecModule->GetJpxModule() : nullptr;
}

CCodec_Jbig2Module* CPDF_ModuleMgr::GetJbig2Module() {
  return m_pCodecModule ? m_pCodecModule->GetJbig2Module() : nullptr;
}

CCodec_IccModule* CPDF_ModuleMgr::GetIccModule() {
  return m_pCodecModule ? m_pCodecModule->GetIccModule() : nullptr;
}

CCodec_FlateModule* CPDF_ModuleMgr::GetFlateModule() {
  return m_pCodecModule ? m_pCodecModule->GetFlateModule() : nullptr;
}
