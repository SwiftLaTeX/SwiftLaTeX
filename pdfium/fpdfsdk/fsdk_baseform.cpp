// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/include/fsdk_baseform.h"

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cfdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fxge/include/fx_ge.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/fxedit/include/fxet_edit.h"
#include "fpdfsdk/include/fsdk_actionhandler.h"
#include "fpdfsdk/include/fsdk_baseannot.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/ijs_context.h"
#include "fpdfsdk/javascript/ijs_runtime.h"
#include "fpdfsdk/pdfwindow/PWL_Utils.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"
#endif  // PDF_ENABLE_XFA

PDFSDK_FieldAction::PDFSDK_FieldAction()
    : bModifier(FALSE),
      bShift(FALSE),
      nCommitKey(0),
      bKeyDown(FALSE),
      nSelEnd(0),
      nSelStart(0),
      bWillCommit(FALSE),
      bFieldFull(FALSE),
      bRC(TRUE) {}

CPDFSDK_Widget::CPDFSDK_Widget(CPDF_Annot* pAnnot,
                               CPDFSDK_PageView* pPageView,
                               CPDFSDK_InterForm* pInterForm)
    : CPDFSDK_BAAnnot(pAnnot, pPageView),
      m_pInterForm(pInterForm),
      m_nAppAge(0),
      m_nValueAge(0)
#ifdef PDF_ENABLE_XFA
      ,
      m_hMixXFAWidget(nullptr),
      m_pWidgetHandler(nullptr)
#endif  // PDF_ENABLE_XFA
{
}

CPDFSDK_Widget::~CPDFSDK_Widget() {}

#ifdef PDF_ENABLE_XFA
CXFA_FFWidget* CPDFSDK_Widget::GetMixXFAWidget() const {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (pDoc->GetDocType() == DOCTYPE_STATIC_XFA) {
    if (!m_hMixXFAWidget) {
      if (CXFA_FFDocView* pDocView = pDoc->GetXFADocView()) {
        CFX_WideString sName;
        if (GetFieldType() == FIELDTYPE_RADIOBUTTON) {
          sName = GetAnnotName();
          if (sName.IsEmpty())
            sName = GetName();
        } else {
          sName = GetName();
        }

        if (!sName.IsEmpty())
          m_hMixXFAWidget = pDocView->GetWidgetByName(sName);
      }
    }
    return m_hMixXFAWidget;
  }

  return nullptr;
}

CXFA_FFWidget* CPDFSDK_Widget::GetGroupMixXFAWidget() {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (pDoc->GetDocType() == DOCTYPE_STATIC_XFA) {
    if (CXFA_FFDocView* pDocView = pDoc->GetXFADocView()) {
      CFX_WideString sName = GetName();
      if (!sName.IsEmpty())
        return pDocView->GetWidgetByName(sName);
    }
  }

  return nullptr;
}

CXFA_FFWidgetHandler* CPDFSDK_Widget::GetXFAWidgetHandler() const {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (pDoc->GetDocType() == DOCTYPE_STATIC_XFA) {
    if (!m_pWidgetHandler) {
      if (CXFA_FFDocView* pDocView = pDoc->GetXFADocView()) {
        m_pWidgetHandler = pDocView->GetWidgetHandler();
      }
    }
    return m_pWidgetHandler;
  }

  return nullptr;
}

static XFA_EVENTTYPE GetXFAEventType(PDFSDK_XFAAActionType eXFAAAT) {
  XFA_EVENTTYPE eEventType = XFA_EVENT_Unknown;

  switch (eXFAAAT) {
    case PDFSDK_XFA_Click:
      eEventType = XFA_EVENT_Click;
      break;
    case PDFSDK_XFA_Full:
      eEventType = XFA_EVENT_Full;
      break;
    case PDFSDK_XFA_PreOpen:
      eEventType = XFA_EVENT_PreOpen;
      break;
    case PDFSDK_XFA_PostOpen:
      eEventType = XFA_EVENT_PostOpen;
      break;
  }

  return eEventType;
}

static XFA_EVENTTYPE GetXFAEventType(CPDF_AAction::AActionType eAAT,
                                     FX_BOOL bWillCommit) {
  XFA_EVENTTYPE eEventType = XFA_EVENT_Unknown;

  switch (eAAT) {
    case CPDF_AAction::CursorEnter:
      eEventType = XFA_EVENT_MouseEnter;
      break;
    case CPDF_AAction::CursorExit:
      eEventType = XFA_EVENT_MouseExit;
      break;
    case CPDF_AAction::ButtonDown:
      eEventType = XFA_EVENT_MouseDown;
      break;
    case CPDF_AAction::ButtonUp:
      eEventType = XFA_EVENT_MouseUp;
      break;
    case CPDF_AAction::GetFocus:
      eEventType = XFA_EVENT_Enter;
      break;
    case CPDF_AAction::LoseFocus:
      eEventType = XFA_EVENT_Exit;
      break;
    case CPDF_AAction::PageOpen:
      break;
    case CPDF_AAction::PageClose:
      break;
    case CPDF_AAction::PageVisible:
      break;
    case CPDF_AAction::PageInvisible:
      break;
    case CPDF_AAction::KeyStroke:
      if (!bWillCommit) {
        eEventType = XFA_EVENT_Change;
      }
      break;
    case CPDF_AAction::Validate:
      eEventType = XFA_EVENT_Validate;
      break;
    case CPDF_AAction::OpenPage:
    case CPDF_AAction::ClosePage:
    case CPDF_AAction::Format:
    case CPDF_AAction::Calculate:
    case CPDF_AAction::CloseDocument:
    case CPDF_AAction::SaveDocument:
    case CPDF_AAction::DocumentSaved:
    case CPDF_AAction::PrintDocument:
    case CPDF_AAction::DocumentPrinted:
      break;
  }

  return eEventType;
}

FX_BOOL CPDFSDK_Widget::HasXFAAAction(PDFSDK_XFAAActionType eXFAAAT) {
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler()) {
      XFA_EVENTTYPE eEventType = GetXFAEventType(eXFAAAT);

      if ((eEventType == XFA_EVENT_Click || eEventType == XFA_EVENT_Change) &&
          GetFieldType() == FIELDTYPE_RADIOBUTTON) {
        if (CXFA_FFWidget* hGroupWidget = GetGroupMixXFAWidget()) {
          CXFA_WidgetAcc* pAcc = hGroupWidget->GetDataAcc();
          if (pXFAWidgetHandler->HasEvent(pAcc, eEventType))
            return TRUE;
        }
      }

      {
        CXFA_WidgetAcc* pAcc = hWidget->GetDataAcc();
        return pXFAWidgetHandler->HasEvent(pAcc, eEventType);
      }
    }
  }

  return FALSE;
}

FX_BOOL CPDFSDK_Widget::OnXFAAAction(PDFSDK_XFAAActionType eXFAAAT,
                                     PDFSDK_FieldAction& data,
                                     CPDFSDK_PageView* pPageView) {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    XFA_EVENTTYPE eEventType = GetXFAEventType(eXFAAAT);

    if (eEventType != XFA_EVENT_Unknown) {
      if (CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler()) {
        CXFA_EventParam param;
        param.m_eType = eEventType;
        param.m_wsChange = data.sChange;
        param.m_iCommitKey = data.nCommitKey;
        param.m_bShift = data.bShift;
        param.m_iSelStart = data.nSelStart;
        param.m_iSelEnd = data.nSelEnd;
        param.m_wsFullText = data.sValue;
        param.m_bKeyDown = data.bKeyDown;
        param.m_bModifier = data.bModifier;
        param.m_wsNewText = data.sValue;
        if (data.nSelEnd > data.nSelStart)
          param.m_wsNewText.Delete(data.nSelStart,
                                   data.nSelEnd - data.nSelStart);
        for (int i = 0; i < data.sChange.GetLength(); i++)
          param.m_wsNewText.Insert(data.nSelStart, data.sChange[i]);
        param.m_wsPrevText = data.sValue;

        if ((eEventType == XFA_EVENT_Click || eEventType == XFA_EVENT_Change) &&
            GetFieldType() == FIELDTYPE_RADIOBUTTON) {
          if (CXFA_FFWidget* hGroupWidget = GetGroupMixXFAWidget()) {
            CXFA_WidgetAcc* pAcc = hGroupWidget->GetDataAcc();
            param.m_pTarget = pAcc;
            if (pXFAWidgetHandler->ProcessEvent(pAcc, &param) !=
                XFA_EVENTERROR_Success) {
              return FALSE;
            }
          }
        }
        CXFA_WidgetAcc* pAcc = hWidget->GetDataAcc();
        param.m_pTarget = pAcc;
        int32_t nRet = pXFAWidgetHandler->ProcessEvent(pAcc, &param);

        if (CXFA_FFDocView* pDocView = pDoc->GetXFADocView()) {
          pDocView->UpdateDocView();
        }
        return nRet == XFA_EVENTERROR_Success;
      }
    }
  }

  return FALSE;
}

void CPDFSDK_Widget::Synchronize(FX_BOOL bSynchronizeElse) {
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    CPDF_FormField* pFormField = GetFormField();
    if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
      switch (GetFieldType()) {
        case FIELDTYPE_CHECKBOX:
        case FIELDTYPE_RADIOBUTTON: {
          CPDF_FormControl* pFormCtrl = GetFormControl();
          XFA_CHECKSTATE eCheckState =
              pFormCtrl->IsChecked() ? XFA_CHECKSTATE_On : XFA_CHECKSTATE_Off;
          pWidgetAcc->SetCheckState(eCheckState, true);
        } break;
        case FIELDTYPE_TEXTFIELD:
          pWidgetAcc->SetValue(pFormField->GetValue(), XFA_VALUEPICTURE_Edit);
          break;
        case FIELDTYPE_LISTBOX: {
          pWidgetAcc->ClearAllSelections();

          for (int i = 0, sz = pFormField->CountSelectedItems(); i < sz; i++) {
            int nIndex = pFormField->GetSelectedIndex(i);
            if (nIndex > -1 && nIndex < pWidgetAcc->CountChoiceListItems())
              pWidgetAcc->SetItemState(nIndex, TRUE, false, FALSE, TRUE);
          }
        } break;
        case FIELDTYPE_COMBOBOX: {
          pWidgetAcc->ClearAllSelections();

          for (int i = 0, sz = pFormField->CountSelectedItems(); i < sz; i++) {
            int nIndex = pFormField->GetSelectedIndex(i);
            if (nIndex > -1 && nIndex < pWidgetAcc->CountChoiceListItems())
              pWidgetAcc->SetItemState(nIndex, TRUE, false, FALSE, TRUE);
          }
        }

          pWidgetAcc->SetValue(pFormField->GetValue(), XFA_VALUEPICTURE_Edit);
          break;
      }

      if (bSynchronizeElse)
        pWidgetAcc->ProcessValueChanged();
    }
  }
}

void CPDFSDK_Widget::SynchronizeXFAValue() {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  CXFA_FFDocView* pXFADocView = pDoc->GetXFADocView();
  if (!pXFADocView)
    return;

  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (GetXFAWidgetHandler()) {
      CPDFSDK_Widget::SynchronizeXFAValue(pXFADocView, hWidget, GetFormField(),
                                          GetFormControl());
    }
  }
}

void CPDFSDK_Widget::SynchronizeXFAItems() {
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  CXFA_FFDocView* pXFADocView = pDoc->GetXFADocView();
  if (!pXFADocView)
    return;

  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (GetXFAWidgetHandler())
      SynchronizeXFAItems(pXFADocView, hWidget, GetFormField(), nullptr);
  }
}

void CPDFSDK_Widget::SynchronizeXFAValue(CXFA_FFDocView* pXFADocView,
                                         CXFA_FFWidget* hWidget,
                                         CPDF_FormField* pFormField,
                                         CPDF_FormControl* pFormControl) {
  ASSERT(hWidget);
  ASSERT(pFormControl);

  switch (pFormField->GetFieldType()) {
    case FIELDTYPE_CHECKBOX: {
      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        pFormField->CheckControl(
            pFormField->GetControlIndex(pFormControl),
            pWidgetAcc->GetCheckState() == XFA_CHECKSTATE_On, true);
      }
    } break;
    case FIELDTYPE_RADIOBUTTON: {
      // TODO(weili): Check whether we need to handle checkbox and radio
      // button differently, otherwise, merge these two cases.
      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        pFormField->CheckControl(
            pFormField->GetControlIndex(pFormControl),
            pWidgetAcc->GetCheckState() == XFA_CHECKSTATE_On, true);
      }
    } break;
    case FIELDTYPE_TEXTFIELD: {
      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        CFX_WideString sValue;
        pWidgetAcc->GetValue(sValue, XFA_VALUEPICTURE_Display);
        pFormField->SetValue(sValue, TRUE);
      }
    } break;
    case FIELDTYPE_LISTBOX: {
      pFormField->ClearSelection(FALSE);

      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        for (int i = 0, sz = pWidgetAcc->CountSelectedItems(); i < sz; i++) {
          int nIndex = pWidgetAcc->GetSelectedItem(i);

          if (nIndex > -1 && nIndex < pFormField->CountOptions()) {
            pFormField->SetItemSelection(nIndex, TRUE, TRUE);
          }
        }
      }
    } break;
    case FIELDTYPE_COMBOBOX: {
      pFormField->ClearSelection(FALSE);

      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        for (int i = 0, sz = pWidgetAcc->CountSelectedItems(); i < sz; i++) {
          int nIndex = pWidgetAcc->GetSelectedItem(i);

          if (nIndex > -1 && nIndex < pFormField->CountOptions()) {
            pFormField->SetItemSelection(nIndex, TRUE, TRUE);
          }
        }

        CFX_WideString sValue;
        pWidgetAcc->GetValue(sValue, XFA_VALUEPICTURE_Display);
        pFormField->SetValue(sValue, TRUE);
      }
    } break;
  }
}

void CPDFSDK_Widget::SynchronizeXFAItems(CXFA_FFDocView* pXFADocView,
                                         CXFA_FFWidget* hWidget,
                                         CPDF_FormField* pFormField,
                                         CPDF_FormControl* pFormControl) {
  ASSERT(hWidget);

  switch (pFormField->GetFieldType()) {
    case FIELDTYPE_LISTBOX: {
      pFormField->ClearSelection(FALSE);
      pFormField->ClearOptions(TRUE);

      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        for (int i = 0, sz = pWidgetAcc->CountChoiceListItems(); i < sz; i++) {
          CFX_WideString swText;
          pWidgetAcc->GetChoiceListItem(swText, i);

          pFormField->InsertOption(swText, i, TRUE);
        }
      }
    } break;
    case FIELDTYPE_COMBOBOX: {
      pFormField->ClearSelection(FALSE);
      pFormField->ClearOptions(FALSE);

      if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
        for (int i = 0, sz = pWidgetAcc->CountChoiceListItems(); i < sz; i++) {
          CFX_WideString swText;
          pWidgetAcc->GetChoiceListItem(swText, i);

          pFormField->InsertOption(swText, i, FALSE);
        }
      }

      pFormField->SetValue(L"", TRUE);
    } break;
  }
}
#endif  // PDF_ENABLE_XFA

FX_BOOL CPDFSDK_Widget::IsWidgetAppearanceValid(
    CPDF_Annot::AppearanceMode mode) {
  CPDF_Dictionary* pAP = m_pAnnot->GetAnnotDict()->GetDictBy("AP");
  if (!pAP)
    return FALSE;

  // Choose the right sub-ap
  const FX_CHAR* ap_entry = "N";
  if (mode == CPDF_Annot::Down)
    ap_entry = "D";
  else if (mode == CPDF_Annot::Rollover)
    ap_entry = "R";
  if (!pAP->KeyExist(ap_entry))
    ap_entry = "N";

  // Get the AP stream or subdirectory
  CPDF_Object* psub = pAP->GetDirectObjectBy(ap_entry);
  if (!psub)
    return FALSE;

  int nFieldType = GetFieldType();
  switch (nFieldType) {
    case FIELDTYPE_PUSHBUTTON:
    case FIELDTYPE_COMBOBOX:
    case FIELDTYPE_LISTBOX:
    case FIELDTYPE_TEXTFIELD:
    case FIELDTYPE_SIGNATURE:
      return psub->IsStream();
    case FIELDTYPE_CHECKBOX:
    case FIELDTYPE_RADIOBUTTON:
      if (CPDF_Dictionary* pSubDict = psub->AsDictionary()) {
        return !!pSubDict->GetStreamBy(GetAppState());
      }
      return FALSE;
  }
  return TRUE;
}

int CPDFSDK_Widget::GetFieldType() const {
  return GetFormField()->GetFieldType();
}

FX_BOOL CPDFSDK_Widget::IsAppearanceValid() {
#ifdef PDF_ENABLE_XFA
  CPDFSDK_Document* pSDKDoc = m_pPageView->GetSDKDocument();
  CPDFXFA_Document* pDoc = pSDKDoc->GetXFADocument();
  int nDocType = pDoc->GetDocType();
  if (nDocType != DOCTYPE_PDF && nDocType != DOCTYPE_STATIC_XFA)
    return TRUE;
#endif  // PDF_ENABLE_XFA
  return CPDFSDK_BAAnnot::IsAppearanceValid();
}

int CPDFSDK_Widget::GetLayoutOrder() const {
  return 2;
}

int CPDFSDK_Widget::GetFieldFlags() const {
  CPDF_InterForm* pPDFInterForm = m_pInterForm->GetInterForm();
  CPDF_FormControl* pFormControl =
      pPDFInterForm->GetControlByDict(m_pAnnot->GetAnnotDict());
  CPDF_FormField* pFormField = pFormControl->GetField();
  return pFormField->GetFieldFlags();
}

CFX_ByteString CPDFSDK_Widget::GetSubType() const {
  int nType = GetFieldType();

  if (nType == FIELDTYPE_SIGNATURE)
    return BFFT_SIGNATURE;
  return CPDFSDK_Annot::GetSubType();
}

CPDF_FormField* CPDFSDK_Widget::GetFormField() const {
  return GetFormControl()->GetField();
}

CPDF_FormControl* CPDFSDK_Widget::GetFormControl() const {
  CPDF_InterForm* pPDFInterForm = m_pInterForm->GetInterForm();
  return pPDFInterForm->GetControlByDict(GetAnnotDict());
}

CPDF_FormControl* CPDFSDK_Widget::GetFormControl(
    CPDF_InterForm* pInterForm,
    const CPDF_Dictionary* pAnnotDict) {
  ASSERT(pAnnotDict);
  return pInterForm->GetControlByDict(pAnnotDict);
}

int CPDFSDK_Widget::GetRotate() const {
  CPDF_FormControl* pCtrl = GetFormControl();
  return pCtrl->GetRotation() % 360;
}

#ifdef PDF_ENABLE_XFA
CFX_WideString CPDFSDK_Widget::GetName() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetFullName();
}
#endif  // PDF_ENABLE_XFA

FX_BOOL CPDFSDK_Widget::GetFillColor(FX_COLORREF& color) const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  int iColorType = 0;
  color = FX_ARGBTOCOLORREF(pFormCtrl->GetBackgroundColor(iColorType));

  return iColorType != COLORTYPE_TRANSPARENT;
}

FX_BOOL CPDFSDK_Widget::GetBorderColor(FX_COLORREF& color) const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  int iColorType = 0;
  color = FX_ARGBTOCOLORREF(pFormCtrl->GetBorderColor(iColorType));

  return iColorType != COLORTYPE_TRANSPARENT;
}

FX_BOOL CPDFSDK_Widget::GetTextColor(FX_COLORREF& color) const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance da = pFormCtrl->GetDefaultAppearance();
  if (da.HasColor()) {
    FX_ARGB argb;
    int iColorType = COLORTYPE_TRANSPARENT;
    da.GetColor(argb, iColorType);
    color = FX_ARGBTOCOLORREF(argb);

    return iColorType != COLORTYPE_TRANSPARENT;
  }

  return FALSE;
}

FX_FLOAT CPDFSDK_Widget::GetFontSize() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance pDa = pFormCtrl->GetDefaultAppearance();
  CFX_ByteString csFont = "";
  FX_FLOAT fFontSize = 0.0f;
  pDa.GetFont(csFont, fFontSize);

  return fFontSize;
}

int CPDFSDK_Widget::GetSelectedIndex(int nIndex) const {
#ifdef PDF_ENABLE_XFA
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
      if (nIndex < pWidgetAcc->CountSelectedItems())
        return pWidgetAcc->GetSelectedItem(nIndex);
    }
  }
#endif  // PDF_ENABLE_XFA
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetSelectedIndex(nIndex);
}

#ifdef PDF_ENABLE_XFA
CFX_WideString CPDFSDK_Widget::GetValue(FX_BOOL bDisplay) const {
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
      CFX_WideString sValue;
      pWidgetAcc->GetValue(
          sValue, bDisplay ? XFA_VALUEPICTURE_Display : XFA_VALUEPICTURE_Edit);
      return sValue;
    }
  }
#else
CFX_WideString CPDFSDK_Widget::GetValue() const {
#endif  // PDF_ENABLE_XFA
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetValue();
}

CFX_WideString CPDFSDK_Widget::GetDefaultValue() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetDefaultValue();
}

CFX_WideString CPDFSDK_Widget::GetOptionLabel(int nIndex) const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetOptionLabel(nIndex);
}

int CPDFSDK_Widget::CountOptions() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->CountOptions();
}

FX_BOOL CPDFSDK_Widget::IsOptionSelected(int nIndex) const {
#ifdef PDF_ENABLE_XFA
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc()) {
      if (nIndex > -1 && nIndex < pWidgetAcc->CountChoiceListItems())
        return pWidgetAcc->GetItemState(nIndex);

      return FALSE;
    }
  }
#endif  // PDF_ENABLE_XFA
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->IsItemSelected(nIndex);
}

int CPDFSDK_Widget::GetTopVisibleIndex() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetTopVisibleIndex();
}

bool CPDFSDK_Widget::IsChecked() const {
#ifdef PDF_ENABLE_XFA
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    if (CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc())
      return pWidgetAcc->GetCheckState() == XFA_CHECKSTATE_On;
  }
#endif  // PDF_ENABLE_XFA
  CPDF_FormControl* pFormCtrl = GetFormControl();
  return pFormCtrl->IsChecked();
}

int CPDFSDK_Widget::GetAlignment() const {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  return pFormCtrl->GetControlAlignment();
}

int CPDFSDK_Widget::GetMaxLen() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetMaxLen();
}

void CPDFSDK_Widget::SetCheck(bool bChecked, bool bNotify) {
  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_FormField* pFormField = pFormCtrl->GetField();
  pFormField->CheckControl(pFormField->GetControlIndex(pFormCtrl), bChecked,
                           bNotify);
#ifdef PDF_ENABLE_XFA
  if (!IsWidgetAppearanceValid(CPDF_Annot::Normal))
    ResetAppearance(TRUE);
  if (!bNotify)
    Synchronize(TRUE);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetValue(const CFX_WideString& sValue, FX_BOOL bNotify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->SetValue(sValue, bNotify);
#ifdef PDF_ENABLE_XFA
  if (!bNotify)
    Synchronize(TRUE);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetDefaultValue(const CFX_WideString& sValue) {}
void CPDFSDK_Widget::SetOptionSelection(int index,
                                        FX_BOOL bSelected,
                                        FX_BOOL bNotify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->SetItemSelection(index, bSelected, bNotify);
#ifdef PDF_ENABLE_XFA
  if (!bNotify)
    Synchronize(TRUE);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::ClearSelection(FX_BOOL bNotify) {
  CPDF_FormField* pFormField = GetFormField();
  pFormField->ClearSelection(bNotify);
#ifdef PDF_ENABLE_XFA
  if (!bNotify)
    Synchronize(TRUE);
#endif  // PDF_ENABLE_XFA
}

void CPDFSDK_Widget::SetTopVisibleIndex(int index) {}

void CPDFSDK_Widget::SetAppModified() {
  m_bAppModified = TRUE;
}

void CPDFSDK_Widget::ClearAppModified() {
  m_bAppModified = FALSE;
}

FX_BOOL CPDFSDK_Widget::IsAppModified() const {
  return m_bAppModified;
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_Widget::ResetAppearance(FX_BOOL bValueChanged) {
  switch (GetFieldType()) {
    case FIELDTYPE_TEXTFIELD:
    case FIELDTYPE_COMBOBOX: {
      FX_BOOL bFormated = FALSE;
      CFX_WideString sValue = OnFormat(bFormated);
      ResetAppearance(bFormated ? sValue.c_str() : nullptr, TRUE);
    } break;
    default:
      ResetAppearance(nullptr, FALSE);
      break;
  }
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_Widget::ResetAppearance(const FX_WCHAR* sValue,
                                     FX_BOOL bValueChanged) {
  SetAppModified();

  m_nAppAge++;
  if (m_nAppAge > 999999)
    m_nAppAge = 0;
  if (bValueChanged)
    m_nValueAge++;

  int nFieldType = GetFieldType();

  switch (nFieldType) {
    case FIELDTYPE_PUSHBUTTON:
      ResetAppearance_PushButton();
      break;
    case FIELDTYPE_CHECKBOX:
      ResetAppearance_CheckBox();
      break;
    case FIELDTYPE_RADIOBUTTON:
      ResetAppearance_RadioButton();
      break;
    case FIELDTYPE_COMBOBOX:
      ResetAppearance_ComboBox(sValue);
      break;
    case FIELDTYPE_LISTBOX:
      ResetAppearance_ListBox();
      break;
    case FIELDTYPE_TEXTFIELD:
      ResetAppearance_TextField(sValue);
      break;
  }

  m_pAnnot->ClearCachedAP();
}

CFX_WideString CPDFSDK_Widget::OnFormat(FX_BOOL& bFormated) {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  return m_pInterForm->OnFormat(pFormField, bFormated);
}

void CPDFSDK_Widget::ResetFieldAppearance(FX_BOOL bValueChanged) {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  m_pInterForm->ResetFieldAppearance(pFormField, nullptr, bValueChanged);
}

void CPDFSDK_Widget::DrawAppearance(CFX_RenderDevice* pDevice,
                                    const CFX_Matrix* pUser2Device,
                                    CPDF_Annot::AppearanceMode mode,
                                    const CPDF_RenderOptions* pOptions) {
  int nFieldType = GetFieldType();

  if ((nFieldType == FIELDTYPE_CHECKBOX ||
       nFieldType == FIELDTYPE_RADIOBUTTON) &&
      mode == CPDF_Annot::Normal &&
      !IsWidgetAppearanceValid(CPDF_Annot::Normal)) {
    CFX_PathData pathData;

    CFX_FloatRect rcAnnot = GetRect();

    pathData.AppendRect(rcAnnot.left, rcAnnot.bottom, rcAnnot.right,
                        rcAnnot.top);

    CFX_GraphStateData gsd;
    gsd.m_LineWidth = 0.0f;

    pDevice->DrawPath(&pathData, pUser2Device, &gsd, 0, 0xFFAAAAAA,
                      FXFILL_ALTERNATE);
  } else {
    CPDFSDK_BAAnnot::DrawAppearance(pDevice, pUser2Device, mode, pOptions);
  }
}

void CPDFSDK_Widget::UpdateField() {
  CPDF_FormField* pFormField = GetFormField();
  ASSERT(pFormField);
  m_pInterForm->UpdateField(pFormField);
}

void CPDFSDK_Widget::DrawShadow(CFX_RenderDevice* pDevice,
                                CPDFSDK_PageView* pPageView) {
  int nFieldType = GetFieldType();
  if (m_pInterForm->IsNeedHighLight(nFieldType)) {
    CFX_FloatRect rc = GetRect();
    FX_COLORREF color = m_pInterForm->GetHighlightColor(nFieldType);
    uint8_t alpha = m_pInterForm->GetHighlightAlpha();

    CFX_FloatRect rcDevice;
    ASSERT(m_pInterForm->GetDocument());
    CPDFDoc_Environment* pEnv = m_pInterForm->GetDocument()->GetEnv();
    if (!pEnv)
      return;
    CFX_Matrix page2device;
    pPageView->GetCurrentMatrix(page2device);
    page2device.Transform(((FX_FLOAT)rc.left), ((FX_FLOAT)rc.bottom),
                          rcDevice.left, rcDevice.bottom);
    page2device.Transform(((FX_FLOAT)rc.right), ((FX_FLOAT)rc.top),
                          rcDevice.right, rcDevice.top);

    rcDevice.Normalize();

    FX_ARGB argb = ArgbEncode((int)alpha, color);
    FX_RECT rcDev((int)rcDevice.left, (int)rcDevice.top, (int)rcDevice.right,
                  (int)rcDevice.bottom);
    pDevice->FillRect(&rcDev, argb);
  }
}

void CPDFSDK_Widget::ResetAppearance_PushButton() {
  CPDF_FormControl* pControl = GetFormControl();
  CFX_FloatRect rcWindow = GetRotatedRect();
  int32_t nLayout = 0;
  switch (pControl->GetTextPosition()) {
    case TEXTPOS_ICON:
      nLayout = PPBL_ICON;
      break;
    case TEXTPOS_BELOW:
      nLayout = PPBL_ICONTOPLABELBOTTOM;
      break;
    case TEXTPOS_ABOVE:
      nLayout = PPBL_LABELTOPICONBOTTOM;
      break;
    case TEXTPOS_RIGHT:
      nLayout = PPBL_ICONLEFTLABELRIGHT;
      break;
    case TEXTPOS_LEFT:
      nLayout = PPBL_LABELLEFTICONRIGHT;
      break;
    case TEXTPOS_OVERLAID:
      nLayout = PPBL_LABELOVERICON;
      break;
    default:
      nLayout = PPBL_LABEL;
      break;
  }

  CPWL_Color crBackground, crBorder;

  int iColorType;
  FX_FLOAT fc[4];

  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  FX_FLOAT fBorderWidth = (FX_FLOAT)GetBorderWidth();
  CPWL_Dash dsBorder(3, 0, 0);
  CPWL_Color crLeftTop, crRightBottom;

  BorderStyle nBorderStyle = GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 1);
      crRightBottom = CPWL_Utils::DevideColor(crBackground, 2);
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0.5);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcClient = CPWL_Utils::DeflateRect(rcWindow, fBorderWidth);

  CPWL_Color crText(COLORTYPE_GRAY, 0);

  FX_FLOAT fFontSize = 12.0f;
  CFX_ByteString csNameTag;

  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  if (da.HasColor()) {
    da.GetColor(iColorType, fc);
    crText = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  if (da.HasFont())
    da.GetFont(csNameTag, fFontSize);

  CFX_WideString csWCaption;
  CFX_WideString csNormalCaption, csRolloverCaption, csDownCaption;

  if (pControl->HasMKEntry("CA")) {
    csNormalCaption = pControl->GetNormalCaption();
  }
  if (pControl->HasMKEntry("RC")) {
    csRolloverCaption = pControl->GetRolloverCaption();
  }
  if (pControl->HasMKEntry("AC")) {
    csDownCaption = pControl->GetDownCaption();
  }

  CPDF_Stream* pNormalIcon = nullptr;
  CPDF_Stream* pRolloverIcon = nullptr;
  CPDF_Stream* pDownIcon = nullptr;

  if (pControl->HasMKEntry("I")) {
    pNormalIcon = pControl->GetNormalIcon();
  }
  if (pControl->HasMKEntry("RI")) {
    pRolloverIcon = pControl->GetRolloverIcon();
  }
  if (pControl->HasMKEntry("IX")) {
    pDownIcon = pControl->GetDownIcon();
  }

  if (pNormalIcon) {
    if (CPDF_Dictionary* pImageDict = pNormalIcon->GetDict()) {
      if (pImageDict->GetStringBy("Name").IsEmpty())
        pImageDict->SetAtString("Name", "ImgA");
    }
  }

  if (pRolloverIcon) {
    if (CPDF_Dictionary* pImageDict = pRolloverIcon->GetDict()) {
      if (pImageDict->GetStringBy("Name").IsEmpty())
        pImageDict->SetAtString("Name", "ImgB");
    }
  }

  if (pDownIcon) {
    if (CPDF_Dictionary* pImageDict = pDownIcon->GetDict()) {
      if (pImageDict->GetStringBy("Name").IsEmpty())
        pImageDict->SetAtString("Name", "ImgC");
    }
  }

  CPDF_IconFit iconFit = pControl->GetIconFit();

  CPDFSDK_Document* pDoc = m_pInterForm->GetDocument();
  CPDFDoc_Environment* pEnv = pDoc->GetEnv();

  CBA_FontMap font_map(this, pEnv->GetSysHandler());
  font_map.SetAPType("N");

  CFX_ByteString csAP =
      CPWL_Utils::GetRectFillAppStream(rcWindow, crBackground) +
      CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                     crLeftTop, crRightBottom, nBorderStyle,
                                     dsBorder) +
      CPWL_Utils::GetPushButtonAppStream(
          iconFit.GetFittingBounds() ? rcWindow : rcClient, &font_map,
          pNormalIcon, iconFit, csNormalCaption, crText, fFontSize, nLayout);

  WriteAppearance("N", GetRotatedRect(), GetMatrix(), csAP);
  if (pNormalIcon)
    AddImageToAppearance("N", pNormalIcon);

  CPDF_FormControl::HighlightingMode eHLM = pControl->GetHighlightingMode();
  if (eHLM == CPDF_FormControl::Push || eHLM == CPDF_FormControl::Toggle) {
    if (csRolloverCaption.IsEmpty() && !pRolloverIcon) {
      csRolloverCaption = csNormalCaption;
      pRolloverIcon = pNormalIcon;
    }

    font_map.SetAPType("R");

    csAP = CPWL_Utils::GetRectFillAppStream(rcWindow, crBackground) +
           CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                          crLeftTop, crRightBottom,
                                          nBorderStyle, dsBorder) +
           CPWL_Utils::GetPushButtonAppStream(
               iconFit.GetFittingBounds() ? rcWindow : rcClient, &font_map,
               pRolloverIcon, iconFit, csRolloverCaption, crText, fFontSize,
               nLayout);

    WriteAppearance("R", GetRotatedRect(), GetMatrix(), csAP);
    if (pRolloverIcon)
      AddImageToAppearance("R", pRolloverIcon);

    if (csDownCaption.IsEmpty() && !pDownIcon) {
      csDownCaption = csNormalCaption;
      pDownIcon = pNormalIcon;
    }

    switch (nBorderStyle) {
      case BorderStyle::BEVELED: {
        CPWL_Color crTemp = crLeftTop;
        crLeftTop = crRightBottom;
        crRightBottom = crTemp;
        break;
      }
      case BorderStyle::INSET: {
        crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0);
        crRightBottom = CPWL_Color(COLORTYPE_GRAY, 1);
        break;
      }
      default:
        break;
    }

    font_map.SetAPType("D");

    csAP = CPWL_Utils::GetRectFillAppStream(
               rcWindow, CPWL_Utils::SubstractColor(crBackground, 0.25f)) +
           CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                          crLeftTop, crRightBottom,
                                          nBorderStyle, dsBorder) +
           CPWL_Utils::GetPushButtonAppStream(
               iconFit.GetFittingBounds() ? rcWindow : rcClient, &font_map,
               pDownIcon, iconFit, csDownCaption, crText, fFontSize, nLayout);

    WriteAppearance("D", GetRotatedRect(), GetMatrix(), csAP);
    if (pDownIcon)
      AddImageToAppearance("D", pDownIcon);
  } else {
    RemoveAppearance("D");
    RemoveAppearance("R");
  }
}

void CPDFSDK_Widget::ResetAppearance_CheckBox() {
  CPDF_FormControl* pControl = GetFormControl();
  CPWL_Color crBackground, crBorder, crText;
  int iColorType;
  FX_FLOAT fc[4];

  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  FX_FLOAT fBorderWidth = (FX_FLOAT)GetBorderWidth();
  CPWL_Dash dsBorder(3, 0, 0);
  CPWL_Color crLeftTop, crRightBottom;

  BorderStyle nBorderStyle = GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 1);
      crRightBottom = CPWL_Utils::DevideColor(crBackground, 2);
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0.5);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcWindow = GetRotatedRect();
  CFX_FloatRect rcClient = CPWL_Utils::DeflateRect(rcWindow, fBorderWidth);

  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  if (da.HasColor()) {
    da.GetColor(iColorType, fc);
    crText = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  int32_t nStyle = 0;

  CFX_WideString csWCaption = pControl->GetNormalCaption();
  if (csWCaption.GetLength() > 0) {
    switch (csWCaption[0]) {
      case L'l':
        nStyle = PCS_CIRCLE;
        break;
      case L'8':
        nStyle = PCS_CROSS;
        break;
      case L'u':
        nStyle = PCS_DIAMOND;
        break;
      case L'n':
        nStyle = PCS_SQUARE;
        break;
      case L'H':
        nStyle = PCS_STAR;
        break;
      default:  // L'4'
        nStyle = PCS_CHECK;
        break;
    }
  } else {
    nStyle = PCS_CHECK;
  }

  CFX_ByteString csAP_N_ON =
      CPWL_Utils::GetRectFillAppStream(rcWindow, crBackground) +
      CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                     crLeftTop, crRightBottom, nBorderStyle,
                                     dsBorder);

  CFX_ByteString csAP_N_OFF = csAP_N_ON;

  switch (nBorderStyle) {
    case BorderStyle::BEVELED: {
      CPWL_Color crTemp = crLeftTop;
      crLeftTop = crRightBottom;
      crRightBottom = crTemp;
      break;
    }
    case BorderStyle::INSET: {
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 1);
      break;
    }
    default:
      break;
  }

  CFX_ByteString csAP_D_ON =
      CPWL_Utils::GetRectFillAppStream(
          rcWindow, CPWL_Utils::SubstractColor(crBackground, 0.25f)) +
      CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                     crLeftTop, crRightBottom, nBorderStyle,
                                     dsBorder);

  CFX_ByteString csAP_D_OFF = csAP_D_ON;

  csAP_N_ON += CPWL_Utils::GetCheckBoxAppStream(rcClient, nStyle, crText);
  csAP_D_ON += CPWL_Utils::GetCheckBoxAppStream(rcClient, nStyle, crText);

  WriteAppearance("N", GetRotatedRect(), GetMatrix(), csAP_N_ON,
                  pControl->GetCheckedAPState());
  WriteAppearance("N", GetRotatedRect(), GetMatrix(), csAP_N_OFF, "Off");

  WriteAppearance("D", GetRotatedRect(), GetMatrix(), csAP_D_ON,
                  pControl->GetCheckedAPState());
  WriteAppearance("D", GetRotatedRect(), GetMatrix(), csAP_D_OFF, "Off");

  CFX_ByteString csAS = GetAppState();
  if (csAS.IsEmpty())
    SetAppState("Off");
}

void CPDFSDK_Widget::ResetAppearance_RadioButton() {
  CPDF_FormControl* pControl = GetFormControl();
  CPWL_Color crBackground, crBorder, crText;
  int iColorType;
  FX_FLOAT fc[4];

  pControl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crBackground = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  pControl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  FX_FLOAT fBorderWidth = (FX_FLOAT)GetBorderWidth();
  CPWL_Dash dsBorder(3, 0, 0);
  CPWL_Color crLeftTop, crRightBottom;

  BorderStyle nBorderStyle = GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 1);
      crRightBottom = CPWL_Utils::DevideColor(crBackground, 2);
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0.5);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 0.75);
      break;
    default:
      break;
  }

  CFX_FloatRect rcWindow = GetRotatedRect();
  CFX_FloatRect rcClient = CPWL_Utils::DeflateRect(rcWindow, fBorderWidth);

  CPDF_DefaultAppearance da = pControl->GetDefaultAppearance();
  if (da.HasColor()) {
    da.GetColor(iColorType, fc);
    crText = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  int32_t nStyle = 0;

  CFX_WideString csWCaption = pControl->GetNormalCaption();
  if (csWCaption.GetLength() > 0) {
    switch (csWCaption[0]) {
      default:  // L'l':
        nStyle = PCS_CIRCLE;
        break;
      case L'8':
        nStyle = PCS_CROSS;
        break;
      case L'u':
        nStyle = PCS_DIAMOND;
        break;
      case L'n':
        nStyle = PCS_SQUARE;
        break;
      case L'H':
        nStyle = PCS_STAR;
        break;
      case L'4':
        nStyle = PCS_CHECK;
        break;
    }
  } else {
    nStyle = PCS_CIRCLE;
  }

  CFX_ByteString csAP_N_ON;

  CFX_FloatRect rcCenter =
      CPWL_Utils::DeflateRect(CPWL_Utils::GetCenterSquare(rcWindow), 1.0f);

  if (nStyle == PCS_CIRCLE) {
    if (nBorderStyle == BorderStyle::BEVELED) {
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 1);
      crRightBottom = CPWL_Utils::SubstractColor(crBackground, 0.25f);
    } else if (nBorderStyle == BorderStyle::INSET) {
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0.5f);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 0.75f);
    }

    csAP_N_ON = CPWL_Utils::GetCircleFillAppStream(rcCenter, crBackground) +
                CPWL_Utils::GetCircleBorderAppStream(
                    rcCenter, fBorderWidth, crBorder, crLeftTop, crRightBottom,
                    nBorderStyle, dsBorder);
  } else {
    csAP_N_ON = CPWL_Utils::GetRectFillAppStream(rcWindow, crBackground) +
                CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                               crLeftTop, crRightBottom,
                                               nBorderStyle, dsBorder);
  }

  CFX_ByteString csAP_N_OFF = csAP_N_ON;

  switch (nBorderStyle) {
    case BorderStyle::BEVELED: {
      CPWL_Color crTemp = crLeftTop;
      crLeftTop = crRightBottom;
      crRightBottom = crTemp;
      break;
    }
    case BorderStyle::INSET: {
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 1);
      break;
    }
    default:
      break;
  }

  CFX_ByteString csAP_D_ON;

  if (nStyle == PCS_CIRCLE) {
    CPWL_Color crBK = CPWL_Utils::SubstractColor(crBackground, 0.25f);
    if (nBorderStyle == BorderStyle::BEVELED) {
      crLeftTop = CPWL_Utils::SubstractColor(crBackground, 0.25f);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 1);
      crBK = crBackground;
    } else if (nBorderStyle == BorderStyle::INSET) {
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 1);
    }

    csAP_D_ON = CPWL_Utils::GetCircleFillAppStream(rcCenter, crBK) +
                CPWL_Utils::GetCircleBorderAppStream(
                    rcCenter, fBorderWidth, crBorder, crLeftTop, crRightBottom,
                    nBorderStyle, dsBorder);
  } else {
    csAP_D_ON = CPWL_Utils::GetRectFillAppStream(
                    rcWindow, CPWL_Utils::SubstractColor(crBackground, 0.25f)) +
                CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                               crLeftTop, crRightBottom,
                                               nBorderStyle, dsBorder);
  }

  CFX_ByteString csAP_D_OFF = csAP_D_ON;

  csAP_N_ON += CPWL_Utils::GetRadioButtonAppStream(rcClient, nStyle, crText);
  csAP_D_ON += CPWL_Utils::GetRadioButtonAppStream(rcClient, nStyle, crText);

  WriteAppearance("N", GetRotatedRect(), GetMatrix(), csAP_N_ON,
                  pControl->GetCheckedAPState());
  WriteAppearance("N", GetRotatedRect(), GetMatrix(), csAP_N_OFF, "Off");

  WriteAppearance("D", GetRotatedRect(), GetMatrix(), csAP_D_ON,
                  pControl->GetCheckedAPState());
  WriteAppearance("D", GetRotatedRect(), GetMatrix(), csAP_D_OFF, "Off");

  CFX_ByteString csAS = GetAppState();
  if (csAS.IsEmpty())
    SetAppState("Off");
}

void CPDFSDK_Widget::ResetAppearance_ComboBox(const FX_WCHAR* sValue) {
  CPDF_FormControl* pControl = GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  CFX_ByteTextBuf sBody, sLines;

  CFX_FloatRect rcClient = GetClientRect();
  CFX_FloatRect rcButton = rcClient;
  rcButton.left = rcButton.right - 13;
  rcButton.Normalize();

  std::unique_ptr<CFX_Edit> pEdit(new CFX_Edit);
  pEdit->EnableRefresh(FALSE);

  CPDFSDK_Document* pDoc = m_pInterForm->GetDocument();
  CPDFDoc_Environment* pEnv = pDoc->GetEnv();
  CBA_FontMap font_map(this, pEnv->GetSysHandler());
  pEdit->SetFontMap(&font_map);

  CFX_FloatRect rcEdit = rcClient;
  rcEdit.right = rcButton.left;
  rcEdit.Normalize();

  pEdit->SetPlateRect(rcEdit);
  pEdit->SetAlignmentV(1);

  FX_FLOAT fFontSize = GetFontSize();
  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(TRUE);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();

  if (sValue) {
    pEdit->SetText(sValue);
  } else {
    int32_t nCurSel = pField->GetSelectedIndex(0);

    if (nCurSel < 0)
      pEdit->SetText(pField->GetValue().c_str());
    else
      pEdit->SetText(pField->GetOptionLabel(nCurSel).c_str());
  }

  CFX_FloatRect rcContent = pEdit->GetContentRect();

  CFX_ByteString sEdit =
      CPWL_Utils::GetEditAppStream(pEdit.get(), CFX_FloatPoint(0.0f, 0.0f));
  if (sEdit.GetLength() > 0) {
    sBody << "/Tx BMC\n"
          << "q\n";
    if (rcContent.Width() > rcEdit.Width() ||
        rcContent.Height() > rcEdit.Height()) {
      sBody << rcEdit.left << " " << rcEdit.bottom << " " << rcEdit.Width()
            << " " << rcEdit.Height() << " re\nW\nn\n";
    }

    CPWL_Color crText = GetTextPWLColor();
    sBody << "BT\n"
          << CPWL_Utils::GetColorAppStream(crText) << sEdit << "ET\n"
          << "Q\nEMC\n";
  }

  sBody << CPWL_Utils::GetDropButtonAppStream(rcButton);

  CFX_ByteString sAP = GetBackgroundAppStream() + GetBorderAppStream() +
                       sLines.AsStringC() + sBody.AsStringC();

  WriteAppearance("N", GetRotatedRect(), GetMatrix(), sAP);
}

void CPDFSDK_Widget::ResetAppearance_ListBox() {
  CPDF_FormControl* pControl = GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  CFX_FloatRect rcClient = GetClientRect();
  CFX_ByteTextBuf sBody, sLines;

  std::unique_ptr<CFX_Edit> pEdit(new CFX_Edit);
  pEdit->EnableRefresh(FALSE);

  CPDFSDK_Document* pDoc = m_pInterForm->GetDocument();
  CPDFDoc_Environment* pEnv = pDoc->GetEnv();

  CBA_FontMap font_map(this, pEnv->GetSysHandler());
  pEdit->SetFontMap(&font_map);

  pEdit->SetPlateRect(CFX_FloatRect(rcClient.left, 0.0f, rcClient.right, 0.0f));

  FX_FLOAT fFontSize = GetFontSize();

  pEdit->SetFontSize(IsFloatZero(fFontSize) ? 12.0f : fFontSize);

  pEdit->Initialize();

  CFX_ByteTextBuf sList;
  FX_FLOAT fy = rcClient.top;

  int32_t nTop = pField->GetTopVisibleIndex();
  int32_t nCount = pField->CountOptions();
  int32_t nSelCount = pField->CountSelectedItems();

  for (int32_t i = nTop; i < nCount; ++i) {
    bool bSelected = false;
    for (int32_t j = 0; j < nSelCount; ++j) {
      if (pField->GetSelectedIndex(j) == i) {
        bSelected = true;
        break;
      }
    }

    pEdit->SetText(pField->GetOptionLabel(i).c_str());

    CFX_FloatRect rcContent = pEdit->GetContentRect();
    FX_FLOAT fItemHeight = rcContent.Height();

    if (bSelected) {
      CFX_FloatRect rcItem =
          CFX_FloatRect(rcClient.left, fy - fItemHeight, rcClient.right, fy);
      sList << "q\n"
            << CPWL_Utils::GetColorAppStream(
                   CPWL_Color(COLORTYPE_RGB, 0, 51.0f / 255.0f,
                              113.0f / 255.0f),
                   TRUE)
            << rcItem.left << " " << rcItem.bottom << " " << rcItem.Width()
            << " " << rcItem.Height() << " re f\n"
            << "Q\n";

      sList << "BT\n"
            << CPWL_Utils::GetColorAppStream(CPWL_Color(COLORTYPE_GRAY, 1),
                                             TRUE)
            << CPWL_Utils::GetEditAppStream(pEdit.get(),
                                            CFX_FloatPoint(0.0f, fy))
            << "ET\n";
    } else {
      CPWL_Color crText = GetTextPWLColor();
      sList << "BT\n"
            << CPWL_Utils::GetColorAppStream(crText, TRUE)
            << CPWL_Utils::GetEditAppStream(pEdit.get(),
                                            CFX_FloatPoint(0.0f, fy))
            << "ET\n";
    }

    fy -= fItemHeight;
  }

  if (sList.GetSize() > 0) {
    sBody << "/Tx BMC\n"
          << "q\n"
          << rcClient.left << " " << rcClient.bottom << " " << rcClient.Width()
          << " " << rcClient.Height() << " re\nW\nn\n";
    sBody << sList << "Q\nEMC\n";
  }

  CFX_ByteString sAP = GetBackgroundAppStream() + GetBorderAppStream() +
                       sLines.AsStringC() + sBody.AsStringC();

  WriteAppearance("N", GetRotatedRect(), GetMatrix(), sAP);
}

void CPDFSDK_Widget::ResetAppearance_TextField(const FX_WCHAR* sValue) {
  CPDF_FormControl* pControl = GetFormControl();
  CPDF_FormField* pField = pControl->GetField();
  CFX_ByteTextBuf sBody, sLines;

  std::unique_ptr<CFX_Edit> pEdit(new CFX_Edit);
  pEdit->EnableRefresh(FALSE);

  CPDFSDK_Document* pDoc = m_pInterForm->GetDocument();
  CPDFDoc_Environment* pEnv = pDoc->GetEnv();

  CBA_FontMap font_map(this, pEnv->GetSysHandler());
  pEdit->SetFontMap(&font_map);

  CFX_FloatRect rcClient = GetClientRect();
  pEdit->SetPlateRect(rcClient);
  pEdit->SetAlignmentH(pControl->GetControlAlignment());

  uint32_t dwFieldFlags = pField->GetFieldFlags();
  FX_BOOL bMultiLine = (dwFieldFlags >> 12) & 1;

  if (bMultiLine) {
    pEdit->SetMultiLine(TRUE);
    pEdit->SetAutoReturn(TRUE);
  } else {
    pEdit->SetAlignmentV(1);
  }

  uint16_t subWord = 0;
  if ((dwFieldFlags >> 13) & 1) {
    subWord = '*';
    pEdit->SetPasswordChar(subWord);
  }

  int nMaxLen = pField->GetMaxLen();
  FX_BOOL bCharArray = (dwFieldFlags >> 24) & 1;
  FX_FLOAT fFontSize = GetFontSize();

#ifdef PDF_ENABLE_XFA
  CFX_WideString sValueTmp;
  if (!sValue && GetMixXFAWidget()) {
    sValueTmp = GetValue(TRUE);
    sValue = sValueTmp.c_str();
  }
#endif  // PDF_ENABLE_XFA

  if (nMaxLen > 0) {
    if (bCharArray) {
      pEdit->SetCharArray(nMaxLen);

      if (IsFloatZero(fFontSize)) {
        fFontSize = CPWL_Edit::GetCharArrayAutoFontSize(font_map.GetPDFFont(0),
                                                        rcClient, nMaxLen);
      }
    } else {
      if (sValue)
        nMaxLen = wcslen((const wchar_t*)sValue);
      pEdit->SetLimitChar(nMaxLen);
    }
  }

  if (IsFloatZero(fFontSize))
    pEdit->SetAutoFontSize(TRUE);
  else
    pEdit->SetFontSize(fFontSize);

  pEdit->Initialize();

  if (sValue)
    pEdit->SetText(sValue);
  else
    pEdit->SetText(pField->GetValue().c_str());

  CFX_FloatRect rcContent = pEdit->GetContentRect();

  CFX_ByteString sEdit = CPWL_Utils::GetEditAppStream(
      pEdit.get(), CFX_FloatPoint(0.0f, 0.0f), nullptr, !bCharArray, subWord);

  if (sEdit.GetLength() > 0) {
    sBody << "/Tx BMC\n"
          << "q\n";
    if (rcContent.Width() > rcClient.Width() ||
        rcContent.Height() > rcClient.Height()) {
      sBody << rcClient.left << " " << rcClient.bottom << " "
            << rcClient.Width() << " " << rcClient.Height() << " re\nW\nn\n";
    }
    CPWL_Color crText = GetTextPWLColor();
    sBody << "BT\n"
          << CPWL_Utils::GetColorAppStream(crText) << sEdit << "ET\n"
          << "Q\nEMC\n";
  }

  if (bCharArray) {
    switch (GetBorderStyle()) {
      case BorderStyle::SOLID: {
        CFX_ByteString sColor =
            CPWL_Utils::GetColorAppStream(GetBorderPWLColor(), FALSE);
        if (sColor.GetLength() > 0) {
          sLines << "q\n"
                 << GetBorderWidth() << " w\n"
                 << CPWL_Utils::GetColorAppStream(GetBorderPWLColor(), FALSE)
                 << " 2 J 0 j\n";

          for (int32_t i = 1; i < nMaxLen; ++i) {
            sLines << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.bottom << " m\n"
                   << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.top << " l S\n";
          }

          sLines << "Q\n";
        }
        break;
      }
      case BorderStyle::DASH: {
        CFX_ByteString sColor =
            CPWL_Utils::GetColorAppStream(GetBorderPWLColor(), FALSE);
        if (sColor.GetLength() > 0) {
          CPWL_Dash dsBorder = CPWL_Dash(3, 3, 0);

          sLines << "q\n"
                 << GetBorderWidth() << " w\n"
                 << CPWL_Utils::GetColorAppStream(GetBorderPWLColor(), FALSE)
                 << "[" << dsBorder.nDash << " " << dsBorder.nGap << "] "
                 << dsBorder.nPhase << " d\n";

          for (int32_t i = 1; i < nMaxLen; ++i) {
            sLines << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.bottom << " m\n"
                   << rcClient.left +
                          ((rcClient.right - rcClient.left) / nMaxLen) * i
                   << " " << rcClient.top << " l S\n";
          }

          sLines << "Q\n";
        }
        break;
      }
      default:
        break;
    }
  }

  CFX_ByteString sAP = GetBackgroundAppStream() + GetBorderAppStream() +
                       sLines.AsStringC() + sBody.AsStringC();
  WriteAppearance("N", GetRotatedRect(), GetMatrix(), sAP);
}

CFX_FloatRect CPDFSDK_Widget::GetClientRect() const {
  CFX_FloatRect rcWindow = GetRotatedRect();
  FX_FLOAT fBorderWidth = (FX_FLOAT)GetBorderWidth();
  switch (GetBorderStyle()) {
    case BorderStyle::BEVELED:
    case BorderStyle::INSET:
      fBorderWidth *= 2.0f;
      break;
    default:
      break;
  }

  return CPWL_Utils::DeflateRect(rcWindow, fBorderWidth);
}

CFX_FloatRect CPDFSDK_Widget::GetRotatedRect() const {
  CFX_FloatRect rectAnnot = GetRect();
  FX_FLOAT fWidth = rectAnnot.right - rectAnnot.left;
  FX_FLOAT fHeight = rectAnnot.top - rectAnnot.bottom;

  CPDF_FormControl* pControl = GetFormControl();
  CFX_FloatRect rcPDFWindow;
  switch (abs(pControl->GetRotation() % 360)) {
    case 0:
    case 180:
    default:
      rcPDFWindow = CFX_FloatRect(0, 0, fWidth, fHeight);
      break;
    case 90:
    case 270:
      rcPDFWindow = CFX_FloatRect(0, 0, fHeight, fWidth);
      break;
  }

  return rcPDFWindow;
}

CFX_ByteString CPDFSDK_Widget::GetBackgroundAppStream() const {
  CPWL_Color crBackground = GetFillPWLColor();
  if (crBackground.nColorType != COLORTYPE_TRANSPARENT) {
    return CPWL_Utils::GetRectFillAppStream(GetRotatedRect(), crBackground);
  }
  return "";
}

CFX_ByteString CPDFSDK_Widget::GetBorderAppStream() const {
  CFX_FloatRect rcWindow = GetRotatedRect();
  CPWL_Color crBorder = GetBorderPWLColor();
  CPWL_Color crBackground = GetFillPWLColor();
  CPWL_Color crLeftTop, crRightBottom;

  FX_FLOAT fBorderWidth = (FX_FLOAT)GetBorderWidth();
  CPWL_Dash dsBorder(3, 0, 0);

  BorderStyle nBorderStyle = GetBorderStyle();
  switch (nBorderStyle) {
    case BorderStyle::DASH:
      dsBorder = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 1);
      crRightBottom = CPWL_Utils::DevideColor(crBackground, 2);
      break;
    case BorderStyle::INSET:
      fBorderWidth *= 2;
      crLeftTop = CPWL_Color(COLORTYPE_GRAY, 0.5);
      crRightBottom = CPWL_Color(COLORTYPE_GRAY, 0.75);
      break;
    default:
      break;
  }

  return CPWL_Utils::GetBorderAppStream(rcWindow, fBorderWidth, crBorder,
                                        crLeftTop, crRightBottom, nBorderStyle,
                                        dsBorder);
}

CFX_Matrix CPDFSDK_Widget::GetMatrix() const {
  CFX_Matrix mt;
  CPDF_FormControl* pControl = GetFormControl();
  CFX_FloatRect rcAnnot = GetRect();
  FX_FLOAT fWidth = rcAnnot.right - rcAnnot.left;
  FX_FLOAT fHeight = rcAnnot.top - rcAnnot.bottom;

  switch (abs(pControl->GetRotation() % 360)) {
    case 0:
    default:
      mt = CFX_Matrix(1, 0, 0, 1, 0, 0);
      break;
    case 90:
      mt = CFX_Matrix(0, 1, -1, 0, fWidth, 0);
      break;
    case 180:
      mt = CFX_Matrix(-1, 0, 0, -1, fWidth, fHeight);
      break;
    case 270:
      mt = CFX_Matrix(0, -1, 1, 0, 0, fHeight);
      break;
  }

  return mt;
}

CPWL_Color CPDFSDK_Widget::GetTextPWLColor() const {
  CPWL_Color crText = CPWL_Color(COLORTYPE_GRAY, 0);

  CPDF_FormControl* pFormCtrl = GetFormControl();
  CPDF_DefaultAppearance da = pFormCtrl->GetDefaultAppearance();
  if (da.HasColor()) {
    int32_t iColorType;
    FX_FLOAT fc[4];
    da.GetColor(iColorType, fc);
    crText = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);
  }

  return crText;
}

CPWL_Color CPDFSDK_Widget::GetBorderPWLColor() const {
  CPWL_Color crBorder;

  CPDF_FormControl* pFormCtrl = GetFormControl();
  int32_t iColorType;
  FX_FLOAT fc[4];
  pFormCtrl->GetOriginalBorderColor(iColorType, fc);
  if (iColorType > 0)
    crBorder = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  return crBorder;
}

CPWL_Color CPDFSDK_Widget::GetFillPWLColor() const {
  CPWL_Color crFill;

  CPDF_FormControl* pFormCtrl = GetFormControl();
  int32_t iColorType;
  FX_FLOAT fc[4];
  pFormCtrl->GetOriginalBackgroundColor(iColorType, fc);
  if (iColorType > 0)
    crFill = CPWL_Color(iColorType, fc[0], fc[1], fc[2], fc[3]);

  return crFill;
}

void CPDFSDK_Widget::AddImageToAppearance(const CFX_ByteString& sAPType,
                                          CPDF_Stream* pImage) {
  CPDF_Document* pDoc = m_pPageView->GetPDFDocument();
  ASSERT(pDoc);

  CPDF_Dictionary* pAPDict = m_pAnnot->GetAnnotDict()->GetDictBy("AP");
  CPDF_Stream* pStream = pAPDict->GetStreamBy(sAPType);
  CPDF_Dictionary* pStreamDict = pStream->GetDict();
  CFX_ByteString sImageAlias = "IMG";

  if (CPDF_Dictionary* pImageDict = pImage->GetDict()) {
    sImageAlias = pImageDict->GetStringBy("Name");
    if (sImageAlias.IsEmpty())
      sImageAlias = "IMG";
  }

  CPDF_Dictionary* pStreamResList = pStreamDict->GetDictBy("Resources");
  if (!pStreamResList) {
    pStreamResList = new CPDF_Dictionary();
    pStreamDict->SetAt("Resources", pStreamResList);
  }

  if (pStreamResList) {
    CPDF_Dictionary* pXObject = new CPDF_Dictionary;
    pXObject->SetAtReference(sImageAlias, pDoc, pImage);
    pStreamResList->SetAt("XObject", pXObject);
  }
}

void CPDFSDK_Widget::RemoveAppearance(const CFX_ByteString& sAPType) {
  if (CPDF_Dictionary* pAPDict = m_pAnnot->GetAnnotDict()->GetDictBy("AP"))
    pAPDict->RemoveAt(sAPType);
}

FX_BOOL CPDFSDK_Widget::OnAAction(CPDF_AAction::AActionType type,
                                  PDFSDK_FieldAction& data,
                                  CPDFSDK_PageView* pPageView) {
  CPDFSDK_Document* pDocument = pPageView->GetSDKDocument();
  CPDFDoc_Environment* pEnv = pDocument->GetEnv();

#ifdef PDF_ENABLE_XFA
  CPDFXFA_Document* pDoc = pDocument->GetXFADocument();
  if (CXFA_FFWidget* hWidget = GetMixXFAWidget()) {
    XFA_EVENTTYPE eEventType = GetXFAEventType(type, data.bWillCommit);

    if (eEventType != XFA_EVENT_Unknown) {
      if (CXFA_FFWidgetHandler* pXFAWidgetHandler = GetXFAWidgetHandler()) {
        CXFA_EventParam param;
        param.m_eType = eEventType;
        param.m_wsChange = data.sChange;
        param.m_iCommitKey = data.nCommitKey;
        param.m_bShift = data.bShift;
        param.m_iSelStart = data.nSelStart;
        param.m_iSelEnd = data.nSelEnd;
        param.m_wsFullText = data.sValue;
        param.m_bKeyDown = data.bKeyDown;
        param.m_bModifier = data.bModifier;
        param.m_wsNewText = data.sValue;
        if (data.nSelEnd > data.nSelStart)
          param.m_wsNewText.Delete(data.nSelStart,
                                   data.nSelEnd - data.nSelStart);
        for (int i = data.sChange.GetLength() - 1; i >= 0; i--)
          param.m_wsNewText.Insert(data.nSelStart, data.sChange[i]);
        param.m_wsPrevText = data.sValue;

        CXFA_WidgetAcc* pAcc = hWidget->GetDataAcc();
        param.m_pTarget = pAcc;
        int32_t nRet = pXFAWidgetHandler->ProcessEvent(pAcc, &param);

        if (CXFA_FFDocView* pDocView = pDoc->GetXFADocView()) {
          pDocView->UpdateDocView();
        }

        if (nRet == XFA_EVENTERROR_Success)
          return TRUE;
      }
    }
  }
#endif  // PDF_ENABLE_XFA

  CPDF_Action action = GetAAction(type);
  if (action.GetDict() && action.GetType() != CPDF_Action::Unknown) {
    CPDFSDK_ActionHandler* pActionHandler = pEnv->GetActionHander();
    return pActionHandler->DoAction_Field(action, type, pDocument,
                                          GetFormField(), data);
  }
  return FALSE;
}

CPDF_Action CPDFSDK_Widget::GetAAction(CPDF_AAction::AActionType eAAT) {
  switch (eAAT) {
    case CPDF_AAction::CursorEnter:
    case CPDF_AAction::CursorExit:
    case CPDF_AAction::ButtonDown:
    case CPDF_AAction::ButtonUp:
    case CPDF_AAction::GetFocus:
    case CPDF_AAction::LoseFocus:
    case CPDF_AAction::PageOpen:
    case CPDF_AAction::PageClose:
    case CPDF_AAction::PageVisible:
    case CPDF_AAction::PageInvisible:
      return CPDFSDK_BAAnnot::GetAAction(eAAT);

    case CPDF_AAction::KeyStroke:
    case CPDF_AAction::Format:
    case CPDF_AAction::Validate:
    case CPDF_AAction::Calculate: {
      CPDF_FormField* pField = GetFormField();
      if (pField->GetAdditionalAction().GetDict())
        return pField->GetAdditionalAction().GetAction(eAAT);
      return CPDFSDK_BAAnnot::GetAAction(eAAT);
    }
    default:
      break;
  }

  return CPDF_Action();
}

CFX_WideString CPDFSDK_Widget::GetAlternateName() const {
  CPDF_FormField* pFormField = GetFormField();
  return pFormField->GetAlternateName();
}

int32_t CPDFSDK_Widget::GetAppearanceAge() const {
  return m_nAppAge;
}

int32_t CPDFSDK_Widget::GetValueAge() const {
  return m_nValueAge;
}

FX_BOOL CPDFSDK_Widget::HitTest(FX_FLOAT pageX, FX_FLOAT pageY) {
  CPDF_Annot* pAnnot = GetPDFAnnot();
  CFX_FloatRect annotRect;
  pAnnot->GetRect(annotRect);
  if (annotRect.Contains(pageX, pageY)) {
    if (!IsVisible())
      return FALSE;

    int nFieldFlags = GetFieldFlags();
    if ((nFieldFlags & FIELDFLAG_READONLY) == FIELDFLAG_READONLY)
      return FALSE;

    return TRUE;
  }
  return FALSE;
}

#ifdef PDF_ENABLE_XFA
CPDFSDK_XFAWidget::CPDFSDK_XFAWidget(CXFA_FFWidget* pAnnot,
                                     CPDFSDK_PageView* pPageView,
                                     CPDFSDK_InterForm* pInterForm)
    : CPDFSDK_Annot(pPageView),
      m_pInterForm(pInterForm),
      m_hXFAWidget(pAnnot) {}

FX_BOOL CPDFSDK_XFAWidget::IsXFAField() {
  return TRUE;
}

CXFA_FFWidget* CPDFSDK_XFAWidget::GetXFAWidget() const {
  return m_hXFAWidget;
}

CFX_ByteString CPDFSDK_XFAWidget::GetType() const {
  return FSDK_XFAWIDGET_TYPENAME;
}

CFX_ByteString CPDFSDK_XFAWidget::GetSubType() const {
  return "";
}

CFX_FloatRect CPDFSDK_XFAWidget::GetRect() const {
  CFX_RectF rcBBox;
  GetXFAWidget()->GetRect(rcBBox);
  return CFX_FloatRect(rcBBox.left, rcBBox.top, rcBBox.left + rcBBox.width,
                       rcBBox.top + rcBBox.height);
}
#endif  // PDF_ENABLE_XFA

CPDFSDK_InterForm::CPDFSDK_InterForm(CPDFSDK_Document* pDocument)
    : m_pDocument(pDocument),
      m_pInterForm(new CPDF_InterForm(m_pDocument->GetPDFDocument())),
#ifdef PDF_ENABLE_XFA
      m_bXfaCalculate(TRUE),
      m_bXfaValidationsEnabled(TRUE),
#endif  // PDF_ENABLE_XFA
      m_bCalculate(TRUE),
      m_bBusy(FALSE) {
  m_pInterForm->SetFormNotify(this);
  for (int i = 0; i < kNumFieldTypes; ++i)
    m_bNeedHightlight[i] = FALSE;
  m_iHighlightAlpha = 0;
}

CPDFSDK_InterForm::~CPDFSDK_InterForm() {
  m_Map.clear();
#ifdef PDF_ENABLE_XFA
  m_XFAMap.clear();
#endif  // PDF_ENABLE_XFA
}

FX_BOOL CPDFSDK_InterForm::HighlightWidgets() {
  return FALSE;
}

CPDFSDK_Widget* CPDFSDK_InterForm::GetSibling(CPDFSDK_Widget* pWidget,
                                              FX_BOOL bNext) const {
  std::unique_ptr<CBA_AnnotIterator> pIterator(
      new CBA_AnnotIterator(pWidget->GetPageView(), "Widget", ""));

  if (bNext) {
    return (CPDFSDK_Widget*)pIterator->GetNextAnnot(pWidget);
  }
  return (CPDFSDK_Widget*)pIterator->GetPrevAnnot(pWidget);
}

CPDFSDK_Widget* CPDFSDK_InterForm::GetWidget(CPDF_FormControl* pControl) const {
  if (!pControl || !m_pInterForm)
    return nullptr;

  CPDFSDK_Widget* pWidget = nullptr;
  const auto it = m_Map.find(pControl);
  if (it != m_Map.end())
    pWidget = it->second;

  if (pWidget)
    return pWidget;

  CPDF_Dictionary* pControlDict = pControl->GetWidget();
  CPDF_Document* pDocument = m_pDocument->GetPDFDocument();
  CPDFSDK_PageView* pPage = nullptr;

  if (CPDF_Dictionary* pPageDict = pControlDict->GetDictBy("P")) {
    int nPageIndex = pDocument->GetPageIndex(pPageDict->GetObjNum());
    if (nPageIndex >= 0) {
      pPage = m_pDocument->GetPageView(nPageIndex);
    }
  }

  if (!pPage) {
    int nPageIndex = GetPageIndexByAnnotDict(pDocument, pControlDict);
    if (nPageIndex >= 0) {
      pPage = m_pDocument->GetPageView(nPageIndex);
    }
  }

  if (!pPage)
    return nullptr;
  return (CPDFSDK_Widget*)pPage->GetAnnotByDict(pControlDict);
}

void CPDFSDK_InterForm::GetWidgets(
    const CFX_WideString& sFieldName,
    std::vector<CPDFSDK_Widget*>* widgets) const {
  for (int i = 0, sz = m_pInterForm->CountFields(sFieldName); i < sz; ++i) {
    CPDF_FormField* pFormField = m_pInterForm->GetField(i, sFieldName);
    ASSERT(pFormField);
    GetWidgets(pFormField, widgets);
  }
}

void CPDFSDK_InterForm::GetWidgets(
    CPDF_FormField* pField,
    std::vector<CPDFSDK_Widget*>* widgets) const {
  for (int i = 0, sz = pField->CountControls(); i < sz; ++i) {
    CPDF_FormControl* pFormCtrl = pField->GetControl(i);
    ASSERT(pFormCtrl);
    CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl);
    if (pWidget)
      widgets->push_back(pWidget);
  }
}

int CPDFSDK_InterForm::GetPageIndexByAnnotDict(
    CPDF_Document* pDocument,
    CPDF_Dictionary* pAnnotDict) const {
  ASSERT(pAnnotDict);

  for (int i = 0, sz = pDocument->GetPageCount(); i < sz; i++) {
    if (CPDF_Dictionary* pPageDict = pDocument->GetPage(i)) {
      if (CPDF_Array* pAnnots = pPageDict->GetArrayBy("Annots")) {
        for (int j = 0, jsz = pAnnots->GetCount(); j < jsz; j++) {
          CPDF_Object* pDict = pAnnots->GetDirectObjectAt(j);
          if (pAnnotDict == pDict) {
            return i;
          }
        }
      }
    }
  }

  return -1;
}

void CPDFSDK_InterForm::AddMap(CPDF_FormControl* pControl,
                               CPDFSDK_Widget* pWidget) {
  m_Map[pControl] = pWidget;
}

void CPDFSDK_InterForm::RemoveMap(CPDF_FormControl* pControl) {
  m_Map.erase(pControl);
}

void CPDFSDK_InterForm::EnableCalculate(FX_BOOL bEnabled) {
  m_bCalculate = bEnabled;
}

FX_BOOL CPDFSDK_InterForm::IsCalculateEnabled() const {
  return m_bCalculate;
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_InterForm::AddXFAMap(CXFA_FFWidget* hWidget,
                                  CPDFSDK_XFAWidget* pWidget) {
  ASSERT(hWidget);
  m_XFAMap[hWidget] = pWidget;
}

void CPDFSDK_InterForm::RemoveXFAMap(CXFA_FFWidget* hWidget) {
  ASSERT(hWidget);
  m_XFAMap.erase(hWidget);
}

CPDFSDK_XFAWidget* CPDFSDK_InterForm::GetXFAWidget(CXFA_FFWidget* hWidget) {
  ASSERT(hWidget);
  auto it = m_XFAMap.find(hWidget);
  return it != m_XFAMap.end() ? it->second : nullptr;
}

void CPDFSDK_InterForm::XfaEnableCalculate(FX_BOOL bEnabled) {
  m_bXfaCalculate = bEnabled;
}
FX_BOOL CPDFSDK_InterForm::IsXfaCalculateEnabled() const {
  return m_bXfaCalculate;
}

FX_BOOL CPDFSDK_InterForm::IsXfaValidationsEnabled() {
  return m_bXfaValidationsEnabled;
}
void CPDFSDK_InterForm::XfaSetValidationsEnabled(FX_BOOL bEnabled) {
  m_bXfaValidationsEnabled = bEnabled;
}
#endif  // PDF_ENABLE_XFA

void CPDFSDK_InterForm::OnCalculate(CPDF_FormField* pFormField) {
  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
  ASSERT(pEnv);
  if (!pEnv->IsJSInitiated())
    return;

  if (m_bBusy)
    return;

  m_bBusy = TRUE;

  if (IsCalculateEnabled()) {
    IJS_Runtime* pRuntime = m_pDocument->GetJsRuntime();
    pRuntime->SetReaderDocument(m_pDocument);

    int nSize = m_pInterForm->CountFieldsInCalculationOrder();
    for (int i = 0; i < nSize; i++) {
      if (CPDF_FormField* pField =
              m_pInterForm->GetFieldInCalculationOrder(i)) {
        int nType = pField->GetFieldType();
        if (nType == FIELDTYPE_COMBOBOX || nType == FIELDTYPE_TEXTFIELD) {
          CPDF_AAction aAction = pField->GetAdditionalAction();
          if (aAction.GetDict() &&
              aAction.ActionExist(CPDF_AAction::Calculate)) {
            CPDF_Action action = aAction.GetAction(CPDF_AAction::Calculate);
            if (action.GetDict()) {
              CFX_WideString csJS = action.GetJavaScript();
              if (!csJS.IsEmpty()) {
                IJS_Context* pContext = pRuntime->NewContext();
                CFX_WideString sOldValue = pField->GetValue();
                CFX_WideString sValue = sOldValue;
                FX_BOOL bRC = TRUE;
                pContext->OnField_Calculate(pFormField, pField, sValue, bRC);

                CFX_WideString sInfo;
                FX_BOOL bRet = pContext->RunScript(csJS, &sInfo);
                pRuntime->ReleaseContext(pContext);

                if (bRet) {
                  if (bRC) {
                    if (sValue.Compare(sOldValue) != 0)
                      pField->SetValue(sValue, TRUE);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  m_bBusy = FALSE;
}

CFX_WideString CPDFSDK_InterForm::OnFormat(CPDF_FormField* pFormField,
                                           FX_BOOL& bFormated) {
  CFX_WideString sValue = pFormField->GetValue();
  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
  ASSERT(pEnv);
  if (!pEnv->IsJSInitiated()) {
    bFormated = FALSE;
    return sValue;
  }

  IJS_Runtime* pRuntime = m_pDocument->GetJsRuntime();
  pRuntime->SetReaderDocument(m_pDocument);

  if (pFormField->GetFieldType() == FIELDTYPE_COMBOBOX) {
    if (pFormField->CountSelectedItems() > 0) {
      int index = pFormField->GetSelectedIndex(0);
      if (index >= 0)
        sValue = pFormField->GetOptionLabel(index);
    }
  }

  bFormated = FALSE;

  CPDF_AAction aAction = pFormField->GetAdditionalAction();
  if (aAction.GetDict() && aAction.ActionExist(CPDF_AAction::Format)) {
    CPDF_Action action = aAction.GetAction(CPDF_AAction::Format);
    if (action.GetDict()) {
      CFX_WideString script = action.GetJavaScript();
      if (!script.IsEmpty()) {
        CFX_WideString Value = sValue;

        IJS_Context* pContext = pRuntime->NewContext();
        pContext->OnField_Format(pFormField, Value, TRUE);

        CFX_WideString sInfo;
        FX_BOOL bRet = pContext->RunScript(script, &sInfo);
        pRuntime->ReleaseContext(pContext);

        if (bRet) {
          sValue = Value;
          bFormated = TRUE;
        }
      }
    }
  }

  return sValue;
}

void CPDFSDK_InterForm::ResetFieldAppearance(CPDF_FormField* pFormField,
                                             const FX_WCHAR* sValue,
                                             FX_BOOL bValueChanged) {
  for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
    CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
    ASSERT(pFormCtrl);
    if (CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl))
      pWidget->ResetAppearance(sValue, bValueChanged);
  }
}

void CPDFSDK_InterForm::UpdateField(CPDF_FormField* pFormField) {
  for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
    CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
    ASSERT(pFormCtrl);

    if (CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl)) {
      CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
      CFFL_IFormFiller* pIFormFiller = pEnv->GetIFormFiller();
      UnderlyingPageType* pPage = pWidget->GetUnderlyingPage();
      CPDFSDK_PageView* pPageView = m_pDocument->GetPageView(pPage, FALSE);
      FX_RECT rcBBox = pIFormFiller->GetViewBBox(pPageView, pWidget);

      pEnv->FFI_Invalidate(pPage, rcBBox.left, rcBBox.top, rcBBox.right,
                           rcBBox.bottom);
    }
  }
}

FX_BOOL CPDFSDK_InterForm::OnKeyStrokeCommit(CPDF_FormField* pFormField,
                                             const CFX_WideString& csValue) {
  CPDF_AAction aAction = pFormField->GetAdditionalAction();
  if (!aAction.GetDict() || !aAction.ActionExist(CPDF_AAction::KeyStroke))
    return TRUE;

  CPDF_Action action = aAction.GetAction(CPDF_AAction::KeyStroke);
  if (!action.GetDict())
    return TRUE;

  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
  CPDFSDK_ActionHandler* pActionHandler = pEnv->GetActionHander();
  PDFSDK_FieldAction fa;
  fa.bModifier = pEnv->FFI_IsCTRLKeyDown(0);
  fa.bShift = pEnv->FFI_IsSHIFTKeyDown(0);
  fa.sValue = csValue;
  pActionHandler->DoAction_FieldJavaScript(action, CPDF_AAction::KeyStroke,
                                           m_pDocument, pFormField, fa);
  return fa.bRC;
}

FX_BOOL CPDFSDK_InterForm::OnValidate(CPDF_FormField* pFormField,
                                      const CFX_WideString& csValue) {
  CPDF_AAction aAction = pFormField->GetAdditionalAction();
  if (!aAction.GetDict() || !aAction.ActionExist(CPDF_AAction::Validate))
    return TRUE;

  CPDF_Action action = aAction.GetAction(CPDF_AAction::Validate);
  if (!action.GetDict())
    return TRUE;

  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
  CPDFSDK_ActionHandler* pActionHandler = pEnv->GetActionHander();
  PDFSDK_FieldAction fa;
  fa.bModifier = pEnv->FFI_IsCTRLKeyDown(0);
  fa.bShift = pEnv->FFI_IsSHIFTKeyDown(0);
  fa.sValue = csValue;
  pActionHandler->DoAction_FieldJavaScript(action, CPDF_AAction::Validate,
                                           m_pDocument, pFormField, fa);
  return fa.bRC;
}

FX_BOOL CPDFSDK_InterForm::DoAction_Hide(const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CPDF_ActionFields af(&action);
  std::vector<CPDF_Object*> fieldObjects = af.GetAllFields();
  std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);

  bool bHide = action.GetHideStatus();
  FX_BOOL bChanged = FALSE;

  for (CPDF_FormField* pField : fields) {
    for (int i = 0, sz = pField->CountControls(); i < sz; ++i) {
      CPDF_FormControl* pControl = pField->GetControl(i);
      ASSERT(pControl);

      if (CPDFSDK_Widget* pWidget = GetWidget(pControl)) {
        uint32_t nFlags = pWidget->GetFlags();
        nFlags &= ~ANNOTFLAG_INVISIBLE;
        nFlags &= ~ANNOTFLAG_NOVIEW;
        if (bHide)
          nFlags |= ANNOTFLAG_HIDDEN;
        else
          nFlags &= ~ANNOTFLAG_HIDDEN;
        pWidget->SetFlags(nFlags);
        pWidget->GetPageView()->UpdateView(pWidget);
        bChanged = TRUE;
      }
    }
  }

  return bChanged;
}

FX_BOOL CPDFSDK_InterForm::DoAction_SubmitForm(const CPDF_Action& action) {
  CFX_WideString sDestination = action.GetFilePath();
  if (sDestination.IsEmpty())
    return FALSE;

  CPDF_Dictionary* pActionDict = action.GetDict();
  if (pActionDict->KeyExist("Fields")) {
    CPDF_ActionFields af(&action);
    uint32_t dwFlags = action.GetFlags();
    std::vector<CPDF_Object*> fieldObjects = af.GetAllFields();
    std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);
    if (!fields.empty()) {
      bool bIncludeOrExclude = !(dwFlags & 0x01);
      if (m_pInterForm->CheckRequiredFields(&fields, bIncludeOrExclude))
        return FALSE;

      return SubmitFields(sDestination, fields, bIncludeOrExclude, false);
    }
  }
  if (m_pInterForm->CheckRequiredFields(nullptr, true))
    return FALSE;

  return SubmitForm(sDestination, FALSE);
}

FX_BOOL CPDFSDK_InterForm::SubmitFields(
    const CFX_WideString& csDestination,
    const std::vector<CPDF_FormField*>& fields,
    bool bIncludeOrExclude,
    bool bUrlEncoded) {
  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();

  CFX_ByteTextBuf textBuf;
  ExportFieldsToFDFTextBuf(fields, bIncludeOrExclude, textBuf);

  uint8_t* pBuffer = textBuf.GetBuffer();
  FX_STRSIZE nBufSize = textBuf.GetLength();

  if (bUrlEncoded && !FDFToURLEncodedData(pBuffer, nBufSize))
    return FALSE;

  pEnv->JS_docSubmitForm(pBuffer, nBufSize, csDestination.c_str());
  return TRUE;
}

FX_BOOL CPDFSDK_InterForm::FDFToURLEncodedData(CFX_WideString csFDFFile,
                                               CFX_WideString csTxtFile) {
  return TRUE;
}

FX_BOOL CPDFSDK_InterForm::FDFToURLEncodedData(uint8_t*& pBuf,
                                               FX_STRSIZE& nBufSize) {
  CFDF_Document* pFDF = CFDF_Document::ParseMemory(pBuf, nBufSize);
  if (pFDF) {
    CPDF_Dictionary* pMainDict = pFDF->GetRoot()->GetDictBy("FDF");
    if (!pMainDict)
      return FALSE;

    // Get fields
    CPDF_Array* pFields = pMainDict->GetArrayBy("Fields");
    if (!pFields)
      return FALSE;

    CFX_ByteTextBuf fdfEncodedData;
    for (uint32_t i = 0; i < pFields->GetCount(); i++) {
      CPDF_Dictionary* pField = pFields->GetDictAt(i);
      if (!pField)
        continue;
      CFX_WideString name;
      name = pField->GetUnicodeTextBy("T");
      CFX_ByteString name_b = CFX_ByteString::FromUnicode(name);
      CFX_ByteString csBValue = pField->GetStringBy("V");
      CFX_WideString csWValue = PDF_DecodeText(csBValue);
      CFX_ByteString csValue_b = CFX_ByteString::FromUnicode(csWValue);

      fdfEncodedData << name_b.GetBuffer(name_b.GetLength());
      name_b.ReleaseBuffer();
      fdfEncodedData << "=";
      fdfEncodedData << csValue_b.GetBuffer(csValue_b.GetLength());
      csValue_b.ReleaseBuffer();
      if (i != pFields->GetCount() - 1)
        fdfEncodedData << "&";
    }

    nBufSize = fdfEncodedData.GetLength();
    pBuf = FX_Alloc(uint8_t, nBufSize);
    FXSYS_memcpy(pBuf, fdfEncodedData.GetBuffer(), nBufSize);
  }
  return TRUE;
}

FX_BOOL CPDFSDK_InterForm::ExportFieldsToFDFTextBuf(
    const std::vector<CPDF_FormField*>& fields,
    bool bIncludeOrExclude,
    CFX_ByteTextBuf& textBuf) {
  std::unique_ptr<CFDF_Document> pFDF(m_pInterForm->ExportToFDF(
      m_pDocument->GetPath().AsStringC(), fields, bIncludeOrExclude));
  return pFDF ? pFDF->WriteBuf(textBuf) : FALSE;
}

#ifdef PDF_ENABLE_XFA
void CPDFSDK_InterForm::SynchronizeField(CPDF_FormField* pFormField,
                                         FX_BOOL bSynchronizeElse) {
  for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
    CPDF_FormControl* pFormCtrl = pFormField->GetControl(i);
    if (CPDFSDK_Widget* pWidget = GetWidget(pFormCtrl)) {
      pWidget->Synchronize(bSynchronizeElse);
    }
  }
}
#endif  // PDF_ENABLE_XFA

CFX_WideString CPDFSDK_InterForm::GetTemporaryFileName(
    const CFX_WideString& sFileExt) {
  CFX_WideString sFileName;
  return L"";
}

FX_BOOL CPDFSDK_InterForm::SubmitForm(const CFX_WideString& sDestination,
                                      FX_BOOL bUrlEncoded) {
  if (sDestination.IsEmpty())
    return FALSE;

  if (!m_pDocument || !m_pInterForm)
    return FALSE;

  CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
  CFX_WideString wsPDFFilePath = m_pDocument->GetPath();
  CFDF_Document* pFDFDoc = m_pInterForm->ExportToFDF(wsPDFFilePath.AsStringC());
  if (!pFDFDoc)
    return FALSE;

  CFX_ByteTextBuf FdfBuffer;
  FX_BOOL bRet = pFDFDoc->WriteBuf(FdfBuffer);
  delete pFDFDoc;
  if (!bRet)
    return FALSE;

  uint8_t* pBuffer = FdfBuffer.GetBuffer();
  FX_STRSIZE nBufSize = FdfBuffer.GetLength();

  if (bUrlEncoded) {
    if (!FDFToURLEncodedData(pBuffer, nBufSize))
      return FALSE;
  }

  pEnv->JS_docSubmitForm(pBuffer, nBufSize, sDestination.c_str());

  if (bUrlEncoded)
    FX_Free(pBuffer);

  return TRUE;
}

FX_BOOL CPDFSDK_InterForm::ExportFormToFDFTextBuf(CFX_ByteTextBuf& textBuf) {
  CFDF_Document* pFDF =
      m_pInterForm->ExportToFDF(m_pDocument->GetPath().AsStringC());
  if (!pFDF)
    return FALSE;

  FX_BOOL bRet = pFDF->WriteBuf(textBuf);
  delete pFDF;

  return bRet;
}

FX_BOOL CPDFSDK_InterForm::DoAction_ResetForm(const CPDF_Action& action) {
  ASSERT(action.GetDict());

  CPDF_Dictionary* pActionDict = action.GetDict();
  if (!pActionDict->KeyExist("Fields"))
    return m_pInterForm->ResetForm(true);

  CPDF_ActionFields af(&action);
  uint32_t dwFlags = action.GetFlags();

  std::vector<CPDF_Object*> fieldObjects = af.GetAllFields();
  std::vector<CPDF_FormField*> fields = GetFieldFromObjects(fieldObjects);
  return m_pInterForm->ResetForm(fields, !(dwFlags & 0x01), true);
}

FX_BOOL CPDFSDK_InterForm::DoAction_ImportData(const CPDF_Action& action) {
  return FALSE;
}

std::vector<CPDF_FormField*> CPDFSDK_InterForm::GetFieldFromObjects(
    const std::vector<CPDF_Object*>& objects) const {
  std::vector<CPDF_FormField*> fields;
  for (CPDF_Object* pObject : objects) {
    if (pObject && pObject->IsString()) {
      CFX_WideString csName = pObject->GetUnicodeText();
      CPDF_FormField* pField = m_pInterForm->GetField(0, csName);
      if (pField)
        fields.push_back(pField);
    }
  }
  return fields;
}

int CPDFSDK_InterForm::BeforeValueChange(CPDF_FormField* pField,
                                         const CFX_WideString& csValue) {
  int nType = pField->GetFieldType();
  if (nType != FIELDTYPE_COMBOBOX && nType != FIELDTYPE_TEXTFIELD)
    return 0;

  if (!OnKeyStrokeCommit(pField, csValue))
    return -1;

  if (!OnValidate(pField, csValue))
    return -1;

  return 1;
}

void CPDFSDK_InterForm::AfterValueChange(CPDF_FormField* pField) {
#ifdef PDF_ENABLE_XFA
  SynchronizeField(pField, FALSE);
#endif  // PDF_ENABLE_XFA
  int nType = pField->GetFieldType();
  if (nType == FIELDTYPE_COMBOBOX || nType == FIELDTYPE_TEXTFIELD) {
    OnCalculate(pField);
    FX_BOOL bFormated = FALSE;
    CFX_WideString sValue = OnFormat(pField, bFormated);
    ResetFieldAppearance(pField, bFormated ? sValue.c_str() : nullptr, TRUE);
    UpdateField(pField);
  }
}

int CPDFSDK_InterForm::BeforeSelectionChange(CPDF_FormField* pField,
                                             const CFX_WideString& csValue) {
  if (pField->GetFieldType() != FIELDTYPE_LISTBOX)
    return 0;

  if (!OnKeyStrokeCommit(pField, csValue))
    return -1;

  if (!OnValidate(pField, csValue))
    return -1;

  return 1;
}

void CPDFSDK_InterForm::AfterSelectionChange(CPDF_FormField* pField) {
  if (pField->GetFieldType() == FIELDTYPE_LISTBOX) {
    OnCalculate(pField);
    ResetFieldAppearance(pField, nullptr, TRUE);
    UpdateField(pField);
  }
}

void CPDFSDK_InterForm::AfterCheckedStatusChange(CPDF_FormField* pField) {
  int nType = pField->GetFieldType();
  if (nType == FIELDTYPE_CHECKBOX || nType == FIELDTYPE_RADIOBUTTON) {
    OnCalculate(pField);
    UpdateField(pField);
  }
}

int CPDFSDK_InterForm::BeforeFormReset(CPDF_InterForm* pForm) {
  return 0;
}

void CPDFSDK_InterForm::AfterFormReset(CPDF_InterForm* pForm) {
  OnCalculate(nullptr);
}

int CPDFSDK_InterForm::BeforeFormImportData(CPDF_InterForm* pForm) {
  return 0;
}

void CPDFSDK_InterForm::AfterFormImportData(CPDF_InterForm* pForm) {
  OnCalculate(nullptr);
}

FX_BOOL CPDFSDK_InterForm::IsNeedHighLight(int nFieldType) {
  if (nFieldType < 1 || nFieldType > kNumFieldTypes)
    return FALSE;
  return m_bNeedHightlight[nFieldType - 1];
}

void CPDFSDK_InterForm::RemoveAllHighLight() {
  for (int i = 0; i < kNumFieldTypes; ++i)
    m_bNeedHightlight[i] = FALSE;
}

void CPDFSDK_InterForm::SetHighlightColor(FX_COLORREF clr, int nFieldType) {
  if (nFieldType < 0 || nFieldType > kNumFieldTypes)
    return;
  switch (nFieldType) {
    case 0: {
      for (int i = 0; i < kNumFieldTypes; ++i) {
        m_aHighlightColor[i] = clr;
        m_bNeedHightlight[i] = TRUE;
      }
      break;
    }
    default: {
      m_aHighlightColor[nFieldType - 1] = clr;
      m_bNeedHightlight[nFieldType - 1] = TRUE;
      break;
    }
  }
}

FX_COLORREF CPDFSDK_InterForm::GetHighlightColor(int nFieldType) {
  if (nFieldType < 0 || nFieldType > kNumFieldTypes)
    return FXSYS_RGB(255, 255, 255);
  if (nFieldType == 0)
    return m_aHighlightColor[0];
  return m_aHighlightColor[nFieldType - 1];
}

CBA_AnnotIterator::CBA_AnnotIterator(CPDFSDK_PageView* pPageView,
                                     const CFX_ByteString& sType,
                                     const CFX_ByteString& sSubType)
    : m_eTabOrder(STRUCTURE),
      m_pPageView(pPageView),
      m_sType(sType),
      m_sSubType(sSubType) {
  CPDF_Page* pPDFPage = m_pPageView->GetPDFPage();
  CFX_ByteString sTabs = pPDFPage->m_pFormDict->GetStringBy("Tabs");
  if (sTabs == "R")
    m_eTabOrder = ROW;
  else if (sTabs == "C")
    m_eTabOrder = COLUMN;

  GenerateResults();
}

CBA_AnnotIterator::~CBA_AnnotIterator() {}

CPDFSDK_Annot* CBA_AnnotIterator::GetFirstAnnot() {
  return m_Annots.empty() ? nullptr : m_Annots.front();
}

CPDFSDK_Annot* CBA_AnnotIterator::GetLastAnnot() {
  return m_Annots.empty() ? nullptr : m_Annots.back();
}

CPDFSDK_Annot* CBA_AnnotIterator::GetNextAnnot(CPDFSDK_Annot* pAnnot) {
  auto iter = std::find(m_Annots.begin(), m_Annots.end(), pAnnot);
  if (iter == m_Annots.end())
    return nullptr;
  ++iter;
  if (iter == m_Annots.end())
    iter = m_Annots.begin();
  return *iter;
}

CPDFSDK_Annot* CBA_AnnotIterator::GetPrevAnnot(CPDFSDK_Annot* pAnnot) {
  auto iter = std::find(m_Annots.begin(), m_Annots.end(), pAnnot);
  if (iter == m_Annots.end())
    return nullptr;
  if (iter == m_Annots.begin())
    iter = m_Annots.end();
  return *(--iter);
}

// static
bool CBA_AnnotIterator::CompareByLeftAscending(const CPDFSDK_Annot* p1,
                                               const CPDFSDK_Annot* p2) {
  return GetAnnotRect(p1).left < GetAnnotRect(p2).left;
}

// static
bool CBA_AnnotIterator::CompareByTopDescending(const CPDFSDK_Annot* p1,
                                               const CPDFSDK_Annot* p2) {
  return GetAnnotRect(p1).top > GetAnnotRect(p2).top;
}

void CBA_AnnotIterator::GenerateResults() {
  switch (m_eTabOrder) {
    case STRUCTURE: {
      for (size_t i = 0; i < m_pPageView->CountAnnots(); ++i) {
        CPDFSDK_Annot* pAnnot = m_pPageView->GetAnnot(i);
        if (pAnnot->GetType() == m_sType && pAnnot->GetSubType() == m_sSubType)
          m_Annots.push_back(pAnnot);
      }
    } break;
    case ROW: {
      std::vector<CPDFSDK_Annot*> sa;
      for (size_t i = 0; i < m_pPageView->CountAnnots(); ++i) {
        CPDFSDK_Annot* pAnnot = m_pPageView->GetAnnot(i);
        if (pAnnot->GetType() == m_sType && pAnnot->GetSubType() == m_sSubType)
          sa.push_back(pAnnot);
      }

      std::sort(sa.begin(), sa.end(), CompareByLeftAscending);
      while (!sa.empty()) {
        int nLeftTopIndex = -1;
        FX_FLOAT fTop = 0.0f;
        for (int i = sa.size() - 1; i >= 0; i--) {
          CFX_FloatRect rcAnnot = GetAnnotRect(sa[i]);
          if (rcAnnot.top > fTop) {
            nLeftTopIndex = i;
            fTop = rcAnnot.top;
          }
        }
        if (nLeftTopIndex >= 0) {
          CPDFSDK_Annot* pLeftTopAnnot = sa[nLeftTopIndex];
          CFX_FloatRect rcLeftTop = GetAnnotRect(pLeftTopAnnot);
          m_Annots.push_back(pLeftTopAnnot);
          sa.erase(sa.begin() + nLeftTopIndex);

          std::vector<int> aSelect;
          for (size_t i = 0; i < sa.size(); ++i) {
            CFX_FloatRect rcAnnot = GetAnnotRect(sa[i]);
            FX_FLOAT fCenterY = (rcAnnot.top + rcAnnot.bottom) / 2.0f;
            if (fCenterY > rcLeftTop.bottom && fCenterY < rcLeftTop.top)
              aSelect.push_back(i);
          }
          for (size_t i = 0; i < aSelect.size(); ++i)
            m_Annots.push_back(sa[aSelect[i]]);

          for (int i = aSelect.size() - 1; i >= 0; --i)
            sa.erase(sa.begin() + aSelect[i]);
        }
      }
    } break;
    case COLUMN: {
      std::vector<CPDFSDK_Annot*> sa;
      for (size_t i = 0; i < m_pPageView->CountAnnots(); ++i) {
        CPDFSDK_Annot* pAnnot = m_pPageView->GetAnnot(i);
        if (pAnnot->GetType() == m_sType && pAnnot->GetSubType() == m_sSubType)
          sa.push_back(pAnnot);
      }

      std::sort(sa.begin(), sa.end(), CompareByTopDescending);
      while (!sa.empty()) {
        int nLeftTopIndex = -1;
        FX_FLOAT fLeft = -1.0f;
        for (int i = sa.size() - 1; i >= 0; --i) {
          CFX_FloatRect rcAnnot = GetAnnotRect(sa[i]);
          if (fLeft < 0) {
            nLeftTopIndex = 0;
            fLeft = rcAnnot.left;
          } else if (rcAnnot.left < fLeft) {
            nLeftTopIndex = i;
            fLeft = rcAnnot.left;
          }
        }

        if (nLeftTopIndex >= 0) {
          CPDFSDK_Annot* pLeftTopAnnot = sa[nLeftTopIndex];
          CFX_FloatRect rcLeftTop = GetAnnotRect(pLeftTopAnnot);
          m_Annots.push_back(pLeftTopAnnot);
          sa.erase(sa.begin() + nLeftTopIndex);

          std::vector<int> aSelect;
          for (size_t i = 0; i < sa.size(); ++i) {
            CFX_FloatRect rcAnnot = GetAnnotRect(sa[i]);
            FX_FLOAT fCenterX = (rcAnnot.left + rcAnnot.right) / 2.0f;
            if (fCenterX > rcLeftTop.left && fCenterX < rcLeftTop.right)
              aSelect.push_back(i);
          }
          for (size_t i = 0; i < aSelect.size(); ++i)
            m_Annots.push_back(sa[aSelect[i]]);

          for (int i = aSelect.size() - 1; i >= 0; --i)
            sa.erase(sa.begin() + aSelect[i]);
        }
      }
      break;
    }
  }
}

CFX_FloatRect CBA_AnnotIterator::GetAnnotRect(const CPDFSDK_Annot* pAnnot) {
  CFX_FloatRect rcAnnot;
  pAnnot->GetPDFAnnot()->GetRect(rcAnnot);
  return rcAnnot;
}
