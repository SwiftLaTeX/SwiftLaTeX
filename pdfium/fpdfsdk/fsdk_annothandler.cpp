// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/include/fsdk_annothandler.h"

#include <algorithm>
#include <vector>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_page.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"
#include "xfa/fwl/core/include/fwl_widgethit.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"
#include "xfa/fxgraphics/include/cfx_graphics.h"
#endif  // PDF_ENABLE_XFA

CPDFSDK_AnnotHandlerMgr::CPDFSDK_AnnotHandlerMgr(CPDFDoc_Environment* pApp) {
  m_pApp = pApp;

  CPDFSDK_BFAnnotHandler* pHandler = new CPDFSDK_BFAnnotHandler(m_pApp);
  pHandler->SetFormFiller(m_pApp->GetIFormFiller());
  RegisterAnnotHandler(pHandler);
#ifdef PDF_ENABLE_XFA
  CPDFSDK_XFAAnnotHandler* pXFAAnnotHandler =
      new CPDFSDK_XFAAnnotHandler(m_pApp);
  RegisterAnnotHandler(pXFAAnnotHandler);
#endif  // PDF_ENABLE_XFA
}

CPDFSDK_AnnotHandlerMgr::~CPDFSDK_AnnotHandlerMgr() {
  for (int i = 0; i < m_Handlers.GetSize(); i++) {
    IPDFSDK_AnnotHandler* pHandler = m_Handlers.GetAt(i);
    delete pHandler;
  }
  m_Handlers.RemoveAll();
  m_mapType2Handler.clear();
}

void CPDFSDK_AnnotHandlerMgr::RegisterAnnotHandler(
    IPDFSDK_AnnotHandler* pAnnotHandler) {
  ASSERT(!GetAnnotHandler(pAnnotHandler->GetType()));

  m_Handlers.Add(pAnnotHandler);
  m_mapType2Handler[pAnnotHandler->GetType()] = pAnnotHandler;
}

void CPDFSDK_AnnotHandlerMgr::UnRegisterAnnotHandler(
    IPDFSDK_AnnotHandler* pAnnotHandler) {
  m_mapType2Handler.erase(pAnnotHandler->GetType());
  for (int i = 0, sz = m_Handlers.GetSize(); i < sz; i++) {
    if (m_Handlers.GetAt(i) == pAnnotHandler) {
      m_Handlers.RemoveAt(i);
      break;
    }
  }
}

CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::NewAnnot(CPDF_Annot* pAnnot,
                                                 CPDFSDK_PageView* pPageView) {
  ASSERT(pPageView);

  if (IPDFSDK_AnnotHandler* pAnnotHandler =
          GetAnnotHandler(pAnnot->GetSubType())) {
    return pAnnotHandler->NewAnnot(pAnnot, pPageView);
  }

  return new CPDFSDK_BAAnnot(pAnnot, pPageView);
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::NewAnnot(CXFA_FFWidget* pAnnot,
                                                 CPDFSDK_PageView* pPageView) {
  ASSERT(pAnnot);
  ASSERT(pPageView);

  if (IPDFSDK_AnnotHandler* pAnnotHandler =
          GetAnnotHandler(FSDK_XFAWIDGET_TYPENAME)) {
    return pAnnotHandler->NewAnnot(pAnnot, pPageView);
  }

  return nullptr;
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_AnnotHandlerMgr::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    pAnnotHandler->OnRelease(pAnnot);
    pAnnotHandler->ReleaseAnnot(pAnnot);
  } else {
    delete pAnnot;
  }
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnCreate(CPDFSDK_Annot* pAnnot) {
  CPDF_Annot* pPDFAnnot = pAnnot->GetPDFAnnot();

  CPDFSDK_DateTime curTime;
  pPDFAnnot->GetAnnotDict()->SetAtString("M", curTime.ToPDFDateTimeString());
  pPDFAnnot->GetAnnotDict()->SetAtNumber("F", 0);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    pAnnotHandler->OnCreate(pAnnot);
  }
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnLoad(CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    pAnnotHandler->OnLoad(pAnnot);
  }
}

IPDFSDK_AnnotHandler* CPDFSDK_AnnotHandlerMgr::GetAnnotHandler(
    CPDFSDK_Annot* pAnnot) const {
  CPDF_Annot* pPDFAnnot = pAnnot->GetPDFAnnot();
  if (pPDFAnnot)
    return GetAnnotHandler(pPDFAnnot->GetSubType());
#ifdef PDF_ENABLE_XFA
  if (pAnnot->GetXFAWidget())
    return GetAnnotHandler(FSDK_XFAWIDGET_TYPENAME);
#endif  // PDF_ENABLE_XFA
  return nullptr;
}

IPDFSDK_AnnotHandler* CPDFSDK_AnnotHandlerMgr::GetAnnotHandler(
    const CFX_ByteString& sType) const {
  auto it = m_mapType2Handler.find(sType);
  return it != m_mapType2Handler.end() ? it->second : nullptr;
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnDraw(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot* pAnnot,
                                           CFX_RenderDevice* pDevice,
                                           CFX_Matrix* pUser2Device,
                                           uint32_t dwFlags) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    pAnnotHandler->OnDraw(pPageView, pAnnot, pDevice, pUser2Device, dwFlags);
  } else {
#ifdef PDF_ENABLE_XFA
    if (pAnnot->IsXFAField())
      return;
#endif  // PDF_ENABLE_XFA
    static_cast<CPDFSDK_BAAnnot*>(pAnnot)
        ->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal, nullptr);
  }
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonDown(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnLButtonDown(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonUp(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnLButtonUp(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnLButtonDblClk(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnLButtonDblClk(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnMouseMove(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnMouseMove(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnMouseWheel(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    short zDelta,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnMouseWheel(pPageView, pAnnot, nFlags, zDelta,
                                       point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnRButtonDown(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnRButtonDown(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnRButtonUp(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot,
    uint32_t nFlags,
    const CFX_FloatPoint& point) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnRButtonUp(pPageView, pAnnot, nFlags, point);
  }
  return FALSE;
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnMouseEnter(CPDFSDK_PageView* pPageView,
                                                 CPDFSDK_Annot* pAnnot,
                                                 uint32_t nFlag) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot))
    pAnnotHandler->OnMouseEnter(pPageView, pAnnot, nFlag);
}

void CPDFSDK_AnnotHandlerMgr::Annot_OnMouseExit(CPDFSDK_PageView* pPageView,
                                                CPDFSDK_Annot* pAnnot,
                                                uint32_t nFlag) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot))
    pAnnotHandler->OnMouseExit(pPageView, pAnnot, nFlag);
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnChar(CPDFSDK_Annot* pAnnot,
                                              uint32_t nChar,
                                              uint32_t nFlags) {
  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnChar(pAnnot, nChar, nFlags);
  }
  return FALSE;
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnKeyDown(CPDFSDK_Annot* pAnnot,
                                                 int nKeyCode,
                                                 int nFlag) {
  if (!m_pApp->FFI_IsCTRLKeyDown(nFlag) && !m_pApp->FFI_IsALTKeyDown(nFlag)) {
    CPDFSDK_PageView* pPage = pAnnot->GetPageView();
    CPDFSDK_Annot* pFocusAnnot = pPage->GetFocusAnnot();
    if (pFocusAnnot && (nKeyCode == FWL_VKEY_Tab)) {
      CPDFSDK_Annot* pNext =
          GetNextAnnot(pFocusAnnot, !m_pApp->FFI_IsSHIFTKeyDown(nFlag));

      if (pNext && pNext != pFocusAnnot) {
        CPDFSDK_Document* pDocument = pPage->GetSDKDocument();
        pDocument->SetFocusAnnot(pNext);
        return TRUE;
      }
    }
  }

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    return pAnnotHandler->OnKeyDown(pAnnot, nKeyCode, nFlag);
  }
  return FALSE;
}
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnKeyUp(CPDFSDK_Annot* pAnnot,
                                               int nKeyCode,
                                               int nFlag) {
  return FALSE;
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnSetFocus(CPDFSDK_Annot* pAnnot,
                                                  uint32_t nFlag) {
  ASSERT(pAnnot);

  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    if (pAnnotHandler->OnSetFocus(pAnnot, nFlag)) {
      CPDFSDK_PageView* pPage = pAnnot->GetPageView();
      pPage->GetSDKDocument();
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnKillFocus(CPDFSDK_Annot* pAnnot,
                                                   uint32_t nFlag) {
  ASSERT(pAnnot);
  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot))
    return pAnnotHandler->OnKillFocus(pAnnot, nFlag);

  return FALSE;
}

#ifdef PDF_ENABLE_XFA
FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnChangeFocus(
    CPDFSDK_Annot* pSetAnnot,
    CPDFSDK_Annot* pKillAnnot) {
  FX_BOOL bXFA = (pSetAnnot && pSetAnnot->GetXFAWidget()) ||
                 (pKillAnnot && pKillAnnot->GetXFAWidget());

  if (bXFA) {
    if (IPDFSDK_AnnotHandler* pXFAAnnotHandler =
            GetAnnotHandler(FSDK_XFAWIDGET_TYPENAME))
      return pXFAAnnotHandler->OnXFAChangedFocus(pKillAnnot, pSetAnnot);
  }

  return TRUE;
}
#endif  // PDF_ENABLE_XFA

CFX_FloatRect CPDFSDK_AnnotHandlerMgr::Annot_OnGetViewBBox(
    CPDFSDK_PageView* pPageView,
    CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);
  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot))
    return pAnnotHandler->GetViewBBox(pPageView, pAnnot);

  return pAnnot->GetRect();
}

FX_BOOL CPDFSDK_AnnotHandlerMgr::Annot_OnHitTest(CPDFSDK_PageView* pPageView,
                                                 CPDFSDK_Annot* pAnnot,
                                                 const CFX_FloatPoint& point) {
  ASSERT(pAnnot);
  if (IPDFSDK_AnnotHandler* pAnnotHandler = GetAnnotHandler(pAnnot)) {
    if (pAnnotHandler->CanAnswer(pAnnot))
      return pAnnotHandler->HitTest(pPageView, pAnnot, point);
  }
  return FALSE;
}

CPDFSDK_Annot* CPDFSDK_AnnotHandlerMgr::GetNextAnnot(CPDFSDK_Annot* pSDKAnnot,
                                                     FX_BOOL bNext) {
#ifdef PDF_ENABLE_XFA
  CPDFSDK_PageView* pPageView = pSDKAnnot->GetPageView();
  CPDFXFA_Page* pPage = pPageView->GetPDFXFAPage();
  if (!pPage)
    return nullptr;
  if (pPage->GetPDFPage()) {  // for pdf annots.
    CBA_AnnotIterator ai(pSDKAnnot->GetPageView(), pSDKAnnot->GetType(), "");
    CPDFSDK_Annot* pNext =
        bNext ? ai.GetNextAnnot(pSDKAnnot) : ai.GetPrevAnnot(pSDKAnnot);
    return pNext;
  }
  // for xfa annots
  std::unique_ptr<IXFA_WidgetIterator> pWidgetIterator(
      pPage->GetXFAPageView()->CreateWidgetIterator(
          XFA_TRAVERSEWAY_Tranvalse, XFA_WidgetStatus_Visible |
                                         XFA_WidgetStatus_Viewable |
                                         XFA_WidgetStatus_Focused));
  if (!pWidgetIterator)
    return nullptr;
  if (pWidgetIterator->GetCurrentWidget() != pSDKAnnot->GetXFAWidget())
    pWidgetIterator->SetCurrentWidget(pSDKAnnot->GetXFAWidget());
  CXFA_FFWidget* hNextFocus =
      bNext ? pWidgetIterator->MoveToNext() : pWidgetIterator->MoveToPrevious();
  if (!hNextFocus && pSDKAnnot)
    hNextFocus = pWidgetIterator->MoveToFirst();

  return pPageView->GetAnnotByXFAWidget(hNextFocus);
#else   // PDF_ENABLE_XFA
  CBA_AnnotIterator ai(pSDKAnnot->GetPageView(), "Widget", "");
  return bNext ? ai.GetNextAnnot(pSDKAnnot) : ai.GetPrevAnnot(pSDKAnnot);
#endif  // PDF_ENABLE_XFA
}

CPDFSDK_BFAnnotHandler::CPDFSDK_BFAnnotHandler(CPDFDoc_Environment* pApp)
    : m_pApp(pApp), m_pFormFiller(nullptr) {}

CPDFSDK_BFAnnotHandler::~CPDFSDK_BFAnnotHandler() {}

CFX_ByteString CPDFSDK_BFAnnotHandler::GetType() {
  return CFX_ByteString("Widget");
}

CFX_ByteString CPDFSDK_BFAnnotHandler::GetName() {
  return CFX_ByteString("WidgetHandler");
}

FX_BOOL CPDFSDK_BFAnnotHandler::CanAnswer(CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot->GetType() == "Widget");
  if (pAnnot->GetSubType() == BFFT_SIGNATURE)
    return FALSE;

  CPDFSDK_Widget* pWidget = static_cast<CPDFSDK_Widget*>(pAnnot);
  if (!pWidget->IsVisible())
    return FALSE;

  int nFieldFlags = pWidget->GetFieldFlags();
  if ((nFieldFlags & FIELDFLAG_READONLY) == FIELDFLAG_READONLY)
    return FALSE;

  if (pWidget->GetFieldType() == FIELDTYPE_PUSHBUTTON)
    return TRUE;

  CPDF_Page* pPage = pWidget->GetPDFPage();
  CPDF_Document* pDocument = pPage->m_pDocument;
  uint32_t dwPermissions = pDocument->GetUserPermissions();
  return (dwPermissions & FPDFPERM_FILL_FORM) ||
         (dwPermissions & FPDFPERM_ANNOT_FORM);
}

CPDFSDK_Annot* CPDFSDK_BFAnnotHandler::NewAnnot(CPDF_Annot* pAnnot,
                                                CPDFSDK_PageView* pPage) {
  CPDFSDK_Document* pSDKDoc = m_pApp->GetSDKDocument();
  CPDFSDK_InterForm* pInterForm = pSDKDoc->GetInterForm();
  CPDF_FormControl* pCtrl = CPDFSDK_Widget::GetFormControl(
      pInterForm->GetInterForm(), pAnnot->GetAnnotDict());
  if (!pCtrl)
    return nullptr;

  CPDFSDK_Widget* pWidget = new CPDFSDK_Widget(pAnnot, pPage, pInterForm);
  pInterForm->AddMap(pCtrl, pWidget);
  CPDF_InterForm* pPDFInterForm = pInterForm->GetInterForm();
  if (pPDFInterForm && pPDFInterForm->NeedConstructAP())
    pWidget->ResetAppearance(nullptr, FALSE);

  return pWidget;
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_Annot* CPDFSDK_BFAnnotHandler::NewAnnot(CXFA_FFWidget* hWidget,
                                                CPDFSDK_PageView* pPage) {
  return nullptr;
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_BFAnnotHandler::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);

  if (m_pFormFiller)
    m_pFormFiller->OnDelete(pAnnot);

  std::unique_ptr<CPDFSDK_Widget> pWidget(static_cast<CPDFSDK_Widget*>(pAnnot));
  CPDFSDK_InterForm* pInterForm = pWidget->GetInterForm();
  CPDF_FormControl* pControl = pWidget->GetFormControl();
  pInterForm->RemoveMap(pControl);
}

void CPDFSDK_BFAnnotHandler::DeleteAnnot(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_BFAnnotHandler::OnDraw(CPDFSDK_PageView* pPageView,
                                    CPDFSDK_Annot* pAnnot,
                                    CFX_RenderDevice* pDevice,
                                    CFX_Matrix* pUser2Device,
                                    uint32_t dwFlags) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
    static_cast<CPDFSDK_BAAnnot*>(pAnnot)
        ->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal, nullptr);
  } else {
    if (m_pFormFiller) {
      m_pFormFiller->OnDraw(pPageView, pAnnot, pDevice, pUser2Device, dwFlags);
    }
  }
}

void CPDFSDK_BFAnnotHandler::OnDrawSleep(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot* pAnnot,
                                         CFX_RenderDevice* pDevice,
                                         CFX_Matrix* pUser2Device,
                                         const CFX_FloatRect& rcWindow,
                                         uint32_t dwFlags) {}

void CPDFSDK_BFAnnotHandler::OnDelete(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_BFAnnotHandler::OnRelease(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_BFAnnotHandler::OnMouseEnter(CPDFSDK_PageView* pPageView,
                                          CPDFSDK_Annot* pAnnot,
                                          uint32_t nFlag) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      m_pFormFiller->OnMouseEnter(pPageView, pAnnot, nFlag);
  }
}
void CPDFSDK_BFAnnotHandler::OnMouseExit(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot* pAnnot,
                                         uint32_t nFlag) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      m_pFormFiller->OnMouseExit(pPageView, pAnnot, nFlag);
  }
}
FX_BOOL CPDFSDK_BFAnnotHandler::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                              CPDFSDK_Annot* pAnnot,
                                              uint32_t nFlags,
                                              const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnLButtonDown(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot* pAnnot,
                                            uint32_t nFlags,
                                            const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnLButtonUp(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                                CPDFSDK_Annot* pAnnot,
                                                uint32_t nFlags,
                                                const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnLButtonDblClk(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnMouseMove(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot* pAnnot,
                                            uint32_t nFlags,
                                            const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnMouseMove(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnMouseWheel(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot* pAnnot,
                                             uint32_t nFlags,
                                             short zDelta,
                                             const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnMouseWheel(pPageView, pAnnot, nFlags, zDelta,
                                         point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnRButtonDown(CPDFSDK_PageView* pPageView,
                                              CPDFSDK_Annot* pAnnot,
                                              uint32_t nFlags,
                                              const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnRButtonDown(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}
FX_BOOL CPDFSDK_BFAnnotHandler::OnRButtonUp(CPDFSDK_PageView* pPageView,
                                            CPDFSDK_Annot* pAnnot,
                                            uint32_t nFlags,
                                            const CFX_FloatPoint& point) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnRButtonUp(pPageView, pAnnot, nFlags, point);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                                                CPDFSDK_Annot* pAnnot,
                                                uint32_t nFlags,
                                                const CFX_FloatPoint& point) {
  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnChar(CPDFSDK_Annot* pAnnot,
                                       uint32_t nChar,
                                       uint32_t nFlags) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnChar(pAnnot, nChar, nFlags);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                          int nKeyCode,
                                          int nFlag) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnKeyDown(pAnnot, nKeyCode, nFlag);
  }

  return FALSE;
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnKeyUp(CPDFSDK_Annot* pAnnot,
                                        int nKeyCode,
                                        int nFlag) {
  return FALSE;
}
void CPDFSDK_BFAnnotHandler::OnCreate(CPDFSDK_Annot* pAnnot) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      m_pFormFiller->OnCreate(pAnnot);
  }
}

void CPDFSDK_BFAnnotHandler::OnLoad(CPDFSDK_Annot* pAnnot) {
  if (pAnnot->GetSubType() == BFFT_SIGNATURE)
    return;

  CPDFSDK_Widget* pWidget = static_cast<CPDFSDK_Widget*>(pAnnot);
  if (!pWidget->IsAppearanceValid())
    pWidget->ResetAppearance(nullptr, FALSE);

  int nFieldType = pWidget->GetFieldType();
  if (nFieldType == FIELDTYPE_TEXTFIELD || nFieldType == FIELDTYPE_COMBOBOX) {
    FX_BOOL bFormated = FALSE;
    CFX_WideString sValue = pWidget->OnFormat(bFormated);
    if (bFormated && nFieldType == FIELDTYPE_COMBOBOX) {
      pWidget->ResetAppearance(sValue.c_str(), FALSE);
    }
  }

#ifdef PDF_ENABLE_XFA
  CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
  CPDFSDK_Document* pSDKDoc = pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (pDoc->GetDocType() == DOCTYPE_STATIC_XFA) {
    if (!pWidget->IsAppearanceValid() && !pWidget->GetValue().IsEmpty())
      pWidget->ResetAppearance(FALSE);
  }
#endif  // PDF_ENABLE_XFA
  if (m_pFormFiller)
    m_pFormFiller->OnLoad(pAnnot);
}

FX_BOOL CPDFSDK_BFAnnotHandler::OnSetFocus(CPDFSDK_Annot* pAnnot,
                                           uint32_t nFlag) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnSetFocus(pAnnot, nFlag);
  }

  return TRUE;
}
FX_BOOL CPDFSDK_BFAnnotHandler::OnKillFocus(CPDFSDK_Annot* pAnnot,
                                            uint32_t nFlag) {
  CFX_ByteString sSubType = pAnnot->GetSubType();

  if (sSubType == BFFT_SIGNATURE) {
  } else {
    if (m_pFormFiller)
      return m_pFormFiller->OnKillFocus(pAnnot, nFlag);
  }

  return TRUE;
}

#ifdef PDF_ENABLE_XFA

FX_BOOL CPDFSDK_BFAnnotHandler::OnXFAChangedFocus(CPDFSDK_Annot* pOldAnnot,
                                                  CPDFSDK_Annot* pNewAnnot) {
  return TRUE;
}

#endif  // PDF_ENABLE_XFA

CFX_FloatRect CPDFSDK_BFAnnotHandler::GetViewBBox(CPDFSDK_PageView* pPageView,
                                                  CPDFSDK_Annot* pAnnot) {
  CFX_ByteString sSubType = pAnnot->GetSubType();
  if (sSubType != BFFT_SIGNATURE && m_pFormFiller)
    return CFX_FloatRect(m_pFormFiller->GetViewBBox(pPageView, pAnnot));

  return CFX_FloatRect(0, 0, 0, 0);
}

FX_BOOL CPDFSDK_BFAnnotHandler::HitTest(CPDFSDK_PageView* pPageView,
                                        CPDFSDK_Annot* pAnnot,
                                        const CFX_FloatPoint& point) {
  ASSERT(pPageView);
  ASSERT(pAnnot);

  CFX_FloatRect rect = GetViewBBox(pPageView, pAnnot);
  return rect.Contains(point.x, point.y);
}

#ifdef PDF_ENABLE_XFA

CPDFSDK_XFAAnnotHandler::CPDFSDK_XFAAnnotHandler(CPDFDoc_Environment* pApp)
    : m_pApp(pApp) {}

CPDFSDK_XFAAnnotHandler::~CPDFSDK_XFAAnnotHandler() {}

CFX_ByteString CPDFSDK_XFAAnnotHandler::GetType() {
  return FSDK_XFAWIDGET_TYPENAME;
}

CFX_ByteString CPDFSDK_XFAAnnotHandler::GetName() {
  return "XFAWidgetHandler";
}

FX_BOOL CPDFSDK_XFAAnnotHandler::CanAnswer(CPDFSDK_Annot* pAnnot) {
  return !!pAnnot->GetXFAWidget();
}

CPDFSDK_Annot* CPDFSDK_XFAAnnotHandler::NewAnnot(CPDF_Annot* pAnnot,
                                                 CPDFSDK_PageView* pPage) {
  return nullptr;
}

CPDFSDK_Annot* CPDFSDK_XFAAnnotHandler::NewAnnot(CXFA_FFWidget* pAnnot,
                                                 CPDFSDK_PageView* pPage) {
  CPDFSDK_Document* pSDKDoc = m_pApp->GetSDKDocument();
  CPDFSDK_InterForm* pInterForm = pSDKDoc->GetInterForm();
  CPDFSDK_XFAWidget* pWidget = new CPDFSDK_XFAWidget(pAnnot, pPage, pInterForm);
  pInterForm->AddXFAMap(pAnnot, pWidget);
  return pWidget;
}

void CPDFSDK_XFAAnnotHandler::OnDraw(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     CFX_RenderDevice* pDevice,
                                     CFX_Matrix* pUser2Device,
                                     uint32_t dwFlags) {
  ASSERT(pPageView);
  ASSERT(pAnnot);

  CPDFSDK_Document* pSDKDoc = pPageView->GetSDKDocument();
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);

  CFX_Graphics gs;
  gs.Create(pDevice);

  CFX_Matrix mt;
  mt = *(CFX_Matrix*)pUser2Device;

  FX_BOOL bIsHighlight = FALSE;
  if (pSDKDoc->GetFocusAnnot() != pAnnot)
    bIsHighlight = TRUE;

  pWidgetHandler->RenderWidget(pAnnot->GetXFAWidget(), &gs, &mt, bIsHighlight);

  // to do highlight and shadow
}

void CPDFSDK_XFAAnnotHandler::OnDrawSleep(CPDFSDK_PageView* pPageView,
                                          CPDFSDK_Annot* pAnnot,
                                          CFX_RenderDevice* pDevice,
                                          CFX_Matrix* pUser2Device,
                                          const CFX_FloatRect& rcWindow,
                                          uint32_t dwFlags) {}

void CPDFSDK_XFAAnnotHandler::OnCreate(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAAnnotHandler::OnLoad(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAAnnotHandler::OnDelete(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAAnnotHandler::OnRelease(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAAnnotHandler::ReleaseAnnot(CPDFSDK_Annot* pAnnot) {
  CPDFSDK_XFAWidget* pWidget = (CPDFSDK_XFAWidget*)pAnnot;
  CPDFSDK_InterForm* pInterForm = pWidget->GetInterForm();
  pInterForm->RemoveXFAMap(pWidget->GetXFAWidget());

  delete pWidget;
}

void CPDFSDK_XFAAnnotHandler::DeleteAnnot(CPDFSDK_Annot* pAnnot) {}

CFX_FloatRect CPDFSDK_XFAAnnotHandler::GetViewBBox(CPDFSDK_PageView* pPageView,
                                                   CPDFSDK_Annot* pAnnot) {
  ASSERT(pAnnot);

  CFX_RectF rcBBox;
  XFA_Element eType = pAnnot->GetXFAWidget()->GetDataAcc()->GetUIType();
  if (eType == XFA_Element::Signature)
    pAnnot->GetXFAWidget()->GetBBox(rcBBox, XFA_WidgetStatus_Visible, TRUE);
  else
    pAnnot->GetXFAWidget()->GetBBox(rcBBox, XFA_WidgetStatus_None);

  CFX_FloatRect rcWidget(rcBBox.left, rcBBox.top, rcBBox.left + rcBBox.width,
                         rcBBox.top + rcBBox.height);
  rcWidget.left -= 1.0f;
  rcWidget.right += 1.0f;
  rcWidget.bottom -= 1.0f;
  rcWidget.top += 1.0f;

  return rcWidget;
}

FX_BOOL CPDFSDK_XFAAnnotHandler::HitTest(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot* pAnnot,
                                         const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CPDFSDK_Document* pSDKDoc = pPageView->GetSDKDocument();
  if (!pSDKDoc)
    return FALSE;

  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (!pDoc)
    return FALSE;

  CXFA_FFDocView* pDocView = pDoc->GetXFADocView();
  if (!pDocView)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = pDocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return FALSE;

  FWL_WidgetHit dwHitTest =
      pWidgetHandler->OnHitTest(pAnnot->GetXFAWidget(), point.x, point.y);
  return dwHitTest != FWL_WidgetHit::Unknown;
}

void CPDFSDK_XFAAnnotHandler::OnMouseEnter(CPDFSDK_PageView* pPageView,
                                           CPDFSDK_Annot* pAnnot,
                                           uint32_t nFlag) {
  if (!pPageView || !pAnnot)
    return;
  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  pWidgetHandler->OnMouseEnter(pAnnot->GetXFAWidget());
}

void CPDFSDK_XFAAnnotHandler::OnMouseExit(CPDFSDK_PageView* pPageView,
                                          CPDFSDK_Annot* pAnnot,
                                          uint32_t nFlag) {
  if (!pPageView || !pAnnot)
    return;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  pWidgetHandler->OnMouseExit(pAnnot->GetXFAWidget());
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                               CPDFSDK_Annot* pAnnot,
                                               uint32_t nFlags,
                                               const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnLButtonDown(pAnnot->GetXFAWidget(),
                                       GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot* pAnnot,
                                             uint32_t nFlags,
                                             const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnLButtonUp(pAnnot->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                                 CPDFSDK_Annot* pAnnot,
                                                 uint32_t nFlags,
                                                 const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnLButtonDblClk(pAnnot->GetXFAWidget(),
                                         GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnMouseMove(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot* pAnnot,
                                             uint32_t nFlags,
                                             const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnMouseMove(pAnnot->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnMouseWheel(CPDFSDK_PageView* pPageView,
                                              CPDFSDK_Annot* pAnnot,
                                              uint32_t nFlags,
                                              short zDelta,
                                              const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnMouseWheel(
      pAnnot->GetXFAWidget(), GetFWLFlags(nFlags), zDelta, point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnRButtonDown(CPDFSDK_PageView* pPageView,
                                               CPDFSDK_Annot* pAnnot,
                                               uint32_t nFlags,
                                               const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnRButtonDown(pAnnot->GetXFAWidget(),
                                       GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnRButtonUp(CPDFSDK_PageView* pPageView,
                                             CPDFSDK_Annot* pAnnot,
                                             uint32_t nFlags,
                                             const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnRButtonUp(pAnnot->GetXFAWidget(),
                                     GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnRButtonDblClk(CPDFSDK_PageView* pPageView,
                                                 CPDFSDK_Annot* pAnnot,
                                                 uint32_t nFlags,
                                                 const CFX_FloatPoint& point) {
  if (!pPageView || !pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnRButtonDblClk(pAnnot->GetXFAWidget(),
                                         GetFWLFlags(nFlags), point.x, point.y);
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnChar(CPDFSDK_Annot* pAnnot,
                                        uint32_t nChar,
                                        uint32_t nFlags) {
  if (!pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnChar(pAnnot->GetXFAWidget(), nChar,
                                GetFWLFlags(nFlags));
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                           int nKeyCode,
                                           int nFlag) {
  if (!pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnKeyDown(pAnnot->GetXFAWidget(), nKeyCode,
                                   GetFWLFlags(nFlag));
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnKeyUp(CPDFSDK_Annot* pAnnot,
                                         int nKeyCode,
                                         int nFlag) {
  if (!pAnnot)
    return FALSE;

  CXFA_FFWidgetHandler* pWidgetHandler = GetXFAWidgetHandler(pAnnot);
  return pWidgetHandler->OnKeyUp(pAnnot->GetXFAWidget(), nKeyCode,
                                 GetFWLFlags(nFlag));
}

void CPDFSDK_XFAAnnotHandler::OnDeSelected(CPDFSDK_Annot* pAnnot) {}

void CPDFSDK_XFAAnnotHandler::OnSelected(CPDFSDK_Annot* pAnnot) {}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnSetFocus(CPDFSDK_Annot* pAnnot,
                                            uint32_t nFlag) {
  return TRUE;
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnKillFocus(CPDFSDK_Annot* pAnnot,
                                             uint32_t nFlag) {
  return TRUE;
}

FX_BOOL CPDFSDK_XFAAnnotHandler::OnXFAChangedFocus(CPDFSDK_Annot* pOldAnnot,
                                                   CPDFSDK_Annot* pNewAnnot) {
  CXFA_FFWidgetHandler* pWidgetHandler = nullptr;

  if (pOldAnnot)
    pWidgetHandler = GetXFAWidgetHandler(pOldAnnot);
  else if (pNewAnnot)
    pWidgetHandler = GetXFAWidgetHandler(pNewAnnot);

  if (pWidgetHandler) {
    FX_BOOL bRet = TRUE;
    CXFA_FFWidget* hWidget = pNewAnnot ? pNewAnnot->GetXFAWidget() : nullptr;
    if (hWidget) {
      CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
      if (pXFAPageView) {
        bRet = pXFAPageView->GetDocView()->SetFocus(hWidget);
        if (pXFAPageView->GetDocView()->GetFocusWidget() == hWidget)
          bRet = TRUE;
      }
    }
    return bRet;
  }

  return TRUE;
}

CXFA_FFWidgetHandler* CPDFSDK_XFAAnnotHandler::GetXFAWidgetHandler(
    CPDFSDK_Annot* pAnnot) {
  if (!pAnnot)
    return nullptr;

  CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
  if (!pPageView)
    return nullptr;

  CPDFSDK_Document* pSDKDoc = pPageView->GetSDKDocument();
  if (!pSDKDoc)
    return nullptr;

  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (!pDoc)
    return nullptr;

  CXFA_FFDocView* pDocView = pDoc->GetXFADocView();
  if (!pDocView)
    return nullptr;

  return pDocView->GetWidgetHandler();
}

#define FWL_KEYFLAG_Ctrl (1 << 0)
#define FWL_KEYFLAG_Alt (1 << 1)
#define FWL_KEYFLAG_Shift (1 << 2)
#define FWL_KEYFLAG_LButton (1 << 3)
#define FWL_KEYFLAG_RButton (1 << 4)
#define FWL_KEYFLAG_MButton (1 << 5)

uint32_t CPDFSDK_XFAAnnotHandler::GetFWLFlags(uint32_t dwFlag) {
  uint32_t dwFWLFlag = 0;

  if (dwFlag & FWL_EVENTFLAG_ControlKey)
    dwFWLFlag |= FWL_KEYFLAG_Ctrl;
  if (dwFlag & FWL_EVENTFLAG_LeftButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_LButton;
  if (dwFlag & FWL_EVENTFLAG_MiddleButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_MButton;
  if (dwFlag & FWL_EVENTFLAG_RightButtonDown)
    dwFWLFlag |= FWL_KEYFLAG_RButton;
  if (dwFlag & FWL_EVENTFLAG_ShiftKey)
    dwFWLFlag |= FWL_KEYFLAG_Shift;
  if (dwFlag & FWL_EVENTFLAG_AltKey)
    dwFWLFlag |= FWL_KEYFLAG_Alt;

  return dwFWLFlag;
}
#endif  // PDF_ENABLE_XFA

CPDFSDK_AnnotIterator::CPDFSDK_AnnotIterator(CPDFSDK_PageView* pPageView,
                                             bool bReverse)
    : m_bReverse(bReverse), m_pos(0) {
  const std::vector<CPDFSDK_Annot*>& annots = pPageView->GetAnnotList();
  m_iteratorAnnotList.insert(m_iteratorAnnotList.begin(), annots.rbegin(),
                             annots.rend());
  std::stable_sort(m_iteratorAnnotList.begin(), m_iteratorAnnotList.end(),
                   [](CPDFSDK_Annot* p1, CPDFSDK_Annot* p2) {
                     return p1->GetLayoutOrder() < p2->GetLayoutOrder();
                   });

  CPDFSDK_Annot* pTopMostAnnot = pPageView->GetFocusAnnot();
  if (!pTopMostAnnot)
    return;

  auto it = std::find(m_iteratorAnnotList.begin(), m_iteratorAnnotList.end(),
                      pTopMostAnnot);
  if (it != m_iteratorAnnotList.end()) {
    CPDFSDK_Annot* pReaderAnnot = *it;
    m_iteratorAnnotList.erase(it);
    m_iteratorAnnotList.insert(m_iteratorAnnotList.begin(), pReaderAnnot);
  }
}

CPDFSDK_AnnotIterator::~CPDFSDK_AnnotIterator() {}

CPDFSDK_Annot* CPDFSDK_AnnotIterator::NextAnnot() {
  if (m_pos < m_iteratorAnnotList.size())
    return m_iteratorAnnotList[m_pos++];
  return nullptr;
}

CPDFSDK_Annot* CPDFSDK_AnnotIterator::PrevAnnot() {
  if (m_pos < m_iteratorAnnotList.size())
    return m_iteratorAnnotList[m_iteratorAnnotList.size() - ++m_pos];
  return nullptr;
}

CPDFSDK_Annot* CPDFSDK_AnnotIterator::Next() {
  return m_bReverse ? PrevAnnot() : NextAnnot();
}
