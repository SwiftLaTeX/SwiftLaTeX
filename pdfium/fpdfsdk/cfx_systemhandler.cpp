// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cfx_systemhandler.h"

#include <memory>

#include "core/fxge/include/fx_ge.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/include/fsdk_mgr.h"

namespace {

int CharSet2CP(int charset) {
  if (charset == FXFONT_SHIFTJIS_CHARSET)
    return 932;
  if (charset == FXFONT_GB2312_CHARSET)
    return 936;
  if (charset == FXFONT_HANGEUL_CHARSET)
    return 949;
  if (charset == FXFONT_CHINESEBIG5_CHARSET)
    return 950;
  return 0;
}

}  // namespace

void CFX_SystemHandler::SetCursor(int32_t nCursorType) {
  m_pEnv->FFI_SetCursor(nCursorType);
}

void CFX_SystemHandler::InvalidateRect(FX_HWND hWnd, FX_RECT rect) {
  CPDFSDK_Annot* pSDKAnnot = (CPDFSDK_Annot*)hWnd;
  CPDFSDK_PageView* pPageView = pSDKAnnot->GetPageView();
  UnderlyingPageType* pPage = pSDKAnnot->GetUnderlyingPage();
  if (!pPage || !pPageView)
    return;

  CFX_Matrix page2device;
  pPageView->GetCurrentMatrix(page2device);
  CFX_Matrix device2page;
  device2page.SetReverse(page2device);
  FX_FLOAT left, top, right, bottom;
  device2page.Transform((FX_FLOAT)rect.left, (FX_FLOAT)rect.top, left, top);
  device2page.Transform((FX_FLOAT)rect.right, (FX_FLOAT)rect.bottom, right,
                        bottom);
  CFX_FloatRect rcPDF(left, bottom, right, top);
  rcPDF.Normalize();

  m_pEnv->FFI_Invalidate(pPage, rcPDF.left, rcPDF.top, rcPDF.right,
                         rcPDF.bottom);
}

void CFX_SystemHandler::OutputSelectedRect(void* pFormFiller,
                                           CFX_FloatRect& rect) {
  CFFL_FormFiller* pFFL = (CFFL_FormFiller*)pFormFiller;
  if (!pFFL)
    return;

  CFX_FloatPoint leftbottom = CFX_FloatPoint(rect.left, rect.bottom);
  CFX_FloatPoint righttop = CFX_FloatPoint(rect.right, rect.top);
  CFX_FloatPoint ptA = pFFL->PWLtoFFL(leftbottom);
  CFX_FloatPoint ptB = pFFL->PWLtoFFL(righttop);
  CPDFSDK_Annot* pAnnot = pFFL->GetSDKAnnot();
  UnderlyingPageType* pPage = pAnnot->GetUnderlyingPage();
  ASSERT(pPage);

  m_pEnv->FFI_OutputSelectedRect(pPage, ptA.x, ptB.y, ptB.x, ptA.y);
}

bool CFX_SystemHandler::IsSelectionImplemented() const {
  if (m_pEnv) {
    FPDF_FORMFILLINFO* pInfo = m_pEnv->GetFormFillInfo();
    if (pInfo && pInfo->FFI_OutputSelectedRect)
      return true;
  }
  return false;
}

bool CFX_SystemHandler::FindNativeTrueTypeFont(CFX_ByteString sFontFaceName) {
  CFX_FontMgr* pFontMgr = CFX_GEModule::Get()->GetFontMgr();
  if (!pFontMgr)
    return false;

  CFX_FontMapper* pFontMapper = pFontMgr->GetBuiltinMapper();
  if (!pFontMapper)
    return false;

  if (pFontMapper->m_InstalledTTFonts.empty())
    pFontMapper->LoadInstalledFonts();

  for (const auto& font : pFontMapper->m_InstalledTTFonts) {
    if (font.Compare(sFontFaceName.AsStringC()))
      return true;
  }

  return false;
}

CPDF_Font* CFX_SystemHandler::AddNativeTrueTypeFontToPDF(
    CPDF_Document* pDoc,
    CFX_ByteString sFontFaceName,
    uint8_t nCharset) {
  if (!pDoc)
    return nullptr;

  std::unique_ptr<CFX_Font> pFXFont(new CFX_Font);
  pFXFont->LoadSubst(sFontFaceName, TRUE, 0, 0, 0, CharSet2CP(nCharset), FALSE);
  return pDoc->AddFont(pFXFont.get(), nCharset, FALSE);
}

int32_t CFX_SystemHandler::SetTimer(int32_t uElapse,
                                    TimerCallback lpTimerFunc) {
  return m_pEnv->FFI_SetTimer(uElapse, lpTimerFunc);
}

void CFX_SystemHandler::KillTimer(int32_t nID) {
  m_pEnv->FFI_KillTimer(nID);
}

FX_SYSTEMTIME CFX_SystemHandler::GetLocalTime() {
  return m_pEnv->FFI_GetLocalTime();
}

bool CFX_SystemHandler::IsSHIFTKeyDown(uint32_t nFlag) const {
  return !!m_pEnv->FFI_IsSHIFTKeyDown(nFlag);
}

bool CFX_SystemHandler::IsCTRLKeyDown(uint32_t nFlag) const {
  return !!m_pEnv->FFI_IsCTRLKeyDown(nFlag);
}

bool CFX_SystemHandler::IsALTKeyDown(uint32_t nFlag) const {
  return !!m_pEnv->FFI_IsALTKeyDown(nFlag);
}
