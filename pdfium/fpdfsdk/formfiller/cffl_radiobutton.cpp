// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_radiobutton.h"

#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/pdfwindow/PWL_SpecialButton.h"

CFFL_RadioButton::CFFL_RadioButton(CPDFDoc_Environment* pApp,
                                   CPDFSDK_Annot* pWidget)
    : CFFL_Button(pApp, pWidget) {}

CFFL_RadioButton::~CFFL_RadioButton() {}

CPWL_Wnd* CFFL_RadioButton::NewPDFWindow(const PWL_CREATEPARAM& cp,
                                         CPDFSDK_PageView* pPageView) {
  CPWL_RadioButton* pWnd = new CPWL_RadioButton();
  pWnd->Create(cp);

  pWnd->SetCheck(m_pWidget->IsChecked());

  return pWnd;
}

FX_BOOL CFFL_RadioButton::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                    FX_UINT nKeyCode,
                                    FX_UINT nFlags) {
  switch (nKeyCode) {
    case FWL_VKEY_Return:
    case FWL_VKEY_Space:
      return TRUE;
    default:
      return CFFL_FormFiller::OnKeyDown(pAnnot, nKeyCode, nFlags);
  }
}

FX_BOOL CFFL_RadioButton::OnChar(CPDFSDK_Annot* pAnnot,
                                 FX_UINT nChar,
                                 FX_UINT nFlags) {
  switch (nChar) {
    case FWL_VKEY_Return:
    case FWL_VKEY_Space: {
      CFFL_IFormFiller* pIFormFiller = m_pApp->GetIFormFiller();
      CPDFSDK_PageView* pPageView = pAnnot->GetPageView();
      ASSERT(pPageView);

      FX_BOOL bReset = FALSE;
      FX_BOOL bExit = FALSE;

      pIFormFiller->OnButtonUp(m_pWidget, pPageView, bReset, bExit, nFlags);

      if (bReset)
        return TRUE;
      if (bExit)
        return TRUE;

      CFFL_FormFiller::OnChar(pAnnot, nChar, nFlags);

      if (CPWL_RadioButton* pWnd =
              (CPWL_RadioButton*)GetPDFWindow(pPageView, TRUE))
        pWnd->SetCheck(TRUE);
      CommitData(pPageView, nFlags);
      return TRUE;
    }
    default:
      return CFFL_FormFiller::OnChar(pAnnot, nChar, nFlags);
  }
}

FX_BOOL CFFL_RadioButton::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                      CPDFSDK_Annot* pAnnot,
                                      FX_UINT nFlags,
                                      const CFX_FloatPoint& point) {
  CFFL_Button::OnLButtonUp(pPageView, pAnnot, nFlags, point);

  if (IsValid()) {
    if (CPWL_RadioButton* pWnd =
            (CPWL_RadioButton*)GetPDFWindow(pPageView, TRUE))
      pWnd->SetCheck(TRUE);

    if (!CommitData(pPageView, nFlags))
      return FALSE;
  }

  return TRUE;
}

FX_BOOL CFFL_RadioButton::IsDataChanged(CPDFSDK_PageView* pPageView) {
  if (CPWL_RadioButton* pWnd =
          (CPWL_RadioButton*)GetPDFWindow(pPageView, FALSE)) {
    return pWnd->IsChecked() != m_pWidget->IsChecked();
  }

  return FALSE;
}

void CFFL_RadioButton::SaveData(CPDFSDK_PageView* pPageView) {
  if (CPWL_RadioButton* pWnd =
          (CPWL_RadioButton*)GetPDFWindow(pPageView, FALSE)) {
    bool bNewChecked = pWnd->IsChecked();

    if (bNewChecked) {
      CPDF_FormField* pField = m_pWidget->GetFormField();
      for (int32_t i = 0, sz = pField->CountControls(); i < sz; i++) {
        if (CPDF_FormControl* pCtrl = pField->GetControl(i)) {
          if (pCtrl->IsChecked()) {
            break;
          }
        }
      }
    }

    m_pWidget->SetCheck(bNewChecked, false);
    m_pWidget->UpdateField();
    SetChangeMark();
  }
}
