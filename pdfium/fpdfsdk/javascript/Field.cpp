// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/Field.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/Document.h"
#include "fpdfsdk/javascript/Icon.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/PublicMethods.h"
#include "fpdfsdk/javascript/cjs_context.h"
#include "fpdfsdk/javascript/cjs_runtime.h"
#include "fpdfsdk/javascript/color.h"

BEGIN_JS_STATIC_CONST(CJS_Field)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Field)
JS_STATIC_PROP_ENTRY(alignment)
JS_STATIC_PROP_ENTRY(borderStyle)
JS_STATIC_PROP_ENTRY(buttonAlignX)
JS_STATIC_PROP_ENTRY(buttonAlignY)
JS_STATIC_PROP_ENTRY(buttonFitBounds)
JS_STATIC_PROP_ENTRY(buttonPosition)
JS_STATIC_PROP_ENTRY(buttonScaleHow)
JS_STATIC_PROP_ENTRY(buttonScaleWhen)
JS_STATIC_PROP_ENTRY(calcOrderIndex)
JS_STATIC_PROP_ENTRY(charLimit)
JS_STATIC_PROP_ENTRY(comb)
JS_STATIC_PROP_ENTRY(commitOnSelChange)
JS_STATIC_PROP_ENTRY(currentValueIndices)
JS_STATIC_PROP_ENTRY(defaultStyle)
JS_STATIC_PROP_ENTRY(defaultValue)
JS_STATIC_PROP_ENTRY(doNotScroll)
JS_STATIC_PROP_ENTRY(doNotSpellCheck)
JS_STATIC_PROP_ENTRY(delay)
JS_STATIC_PROP_ENTRY(display)
JS_STATIC_PROP_ENTRY(doc)
JS_STATIC_PROP_ENTRY(editable)
JS_STATIC_PROP_ENTRY(exportValues)
JS_STATIC_PROP_ENTRY(hidden)
JS_STATIC_PROP_ENTRY(fileSelect)
JS_STATIC_PROP_ENTRY(fillColor)
JS_STATIC_PROP_ENTRY(lineWidth)
JS_STATIC_PROP_ENTRY(highlight)
JS_STATIC_PROP_ENTRY(multiline)
JS_STATIC_PROP_ENTRY(multipleSelection)
JS_STATIC_PROP_ENTRY(name)
JS_STATIC_PROP_ENTRY(numItems)
JS_STATIC_PROP_ENTRY(page)
JS_STATIC_PROP_ENTRY(password)
JS_STATIC_PROP_ENTRY(print)
JS_STATIC_PROP_ENTRY(radiosInUnison)
JS_STATIC_PROP_ENTRY(readonly)
JS_STATIC_PROP_ENTRY(rect)
JS_STATIC_PROP_ENTRY(required)
JS_STATIC_PROP_ENTRY(richText)
JS_STATIC_PROP_ENTRY(richValue)
JS_STATIC_PROP_ENTRY(rotation)
JS_STATIC_PROP_ENTRY(strokeColor)
JS_STATIC_PROP_ENTRY(style)
JS_STATIC_PROP_ENTRY(submitName)
JS_STATIC_PROP_ENTRY(textColor)
JS_STATIC_PROP_ENTRY(textFont)
JS_STATIC_PROP_ENTRY(textSize)
JS_STATIC_PROP_ENTRY(type)
JS_STATIC_PROP_ENTRY(userName)
JS_STATIC_PROP_ENTRY(value)
JS_STATIC_PROP_ENTRY(valueAsString)
JS_STATIC_PROP_ENTRY(source)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Field)
JS_STATIC_METHOD_ENTRY(browseForFileToSubmit)
JS_STATIC_METHOD_ENTRY(buttonGetCaption)
JS_STATIC_METHOD_ENTRY(buttonGetIcon)
JS_STATIC_METHOD_ENTRY(buttonImportIcon)
JS_STATIC_METHOD_ENTRY(buttonSetCaption)
JS_STATIC_METHOD_ENTRY(buttonSetIcon)
JS_STATIC_METHOD_ENTRY(checkThisBox)
JS_STATIC_METHOD_ENTRY(clearItems)
JS_STATIC_METHOD_ENTRY(defaultIsChecked)
JS_STATIC_METHOD_ENTRY(deleteItemAt)
JS_STATIC_METHOD_ENTRY(getArray)
JS_STATIC_METHOD_ENTRY(getItemAt)
JS_STATIC_METHOD_ENTRY(getLock)
JS_STATIC_METHOD_ENTRY(insertItemAt)
JS_STATIC_METHOD_ENTRY(isBoxChecked)
JS_STATIC_METHOD_ENTRY(isDefaultChecked)
JS_STATIC_METHOD_ENTRY(setAction)
JS_STATIC_METHOD_ENTRY(setFocus)
JS_STATIC_METHOD_ENTRY(setItems)
JS_STATIC_METHOD_ENTRY(setLock)
JS_STATIC_METHOD_ENTRY(signatureGetModifications)
JS_STATIC_METHOD_ENTRY(signatureGetSeedValue)
JS_STATIC_METHOD_ENTRY(signatureInfo)
JS_STATIC_METHOD_ENTRY(signatureSetSeedValue)
JS_STATIC_METHOD_ENTRY(signatureSign)
JS_STATIC_METHOD_ENTRY(signatureValidate)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_Field, Field)

CJS_DelayData::CJS_DelayData(FIELD_PROP prop,
                             int idx,
                             const CFX_WideString& name)
    : eProp(prop), nControlIndex(idx), sFieldName(name) {}

CJS_DelayData::~CJS_DelayData() {}

void CJS_Field::InitInstance(IJS_Runtime* pIRuntime) {
  CJS_Runtime* pRuntime = static_cast<CJS_Runtime*>(pIRuntime);
  Field* pField = static_cast<Field*>(GetEmbedObject());
  pField->SetIsolate(pRuntime->GetIsolate());
}

Field::Field(CJS_Object* pJSObject)
    : CJS_EmbedObj(pJSObject),
      m_pJSDoc(nullptr),
      m_pDocument(nullptr),
      m_nFormControlIndex(-1),
      m_bCanSet(FALSE),
      m_bDelay(FALSE),
      m_isolate(nullptr) {}

Field::~Field() {}

// note: iControlNo = -1, means not a widget.
void Field::ParseFieldName(const std::wstring& strFieldNameParsed,
                           std::wstring& strFieldName,
                           int& iControlNo) {
  int iStart = strFieldNameParsed.find_last_of(L'.');
  if (iStart == -1) {
    strFieldName = strFieldNameParsed;
    iControlNo = -1;
    return;
  }
  std::wstring suffixal = strFieldNameParsed.substr(iStart + 1);
  iControlNo = FXSYS_wtoi(suffixal.c_str());
  if (iControlNo == 0) {
    int iSpaceStart;
    while ((iSpaceStart = suffixal.find_last_of(L" ")) != -1) {
      suffixal.erase(iSpaceStart, 1);
    }

    if (suffixal.compare(L"0") != 0) {
      strFieldName = strFieldNameParsed;
      iControlNo = -1;
      return;
    }
  }
  strFieldName = strFieldNameParsed.substr(0, iStart);
}

FX_BOOL Field::AttachField(Document* pDocument,
                           const CFX_WideString& csFieldName) {
  m_pJSDoc = pDocument;
  m_pDocument = pDocument->GetReaderDoc();
  m_bCanSet = m_pDocument->GetPermissions(FPDFPERM_FILL_FORM) ||
              m_pDocument->GetPermissions(FPDFPERM_ANNOT_FORM) ||
              m_pDocument->GetPermissions(FPDFPERM_MODIFY);

  CPDFSDK_InterForm* pRDInterForm = m_pDocument->GetInterForm();
  CPDF_InterForm* pInterForm = pRDInterForm->GetInterForm();
  CFX_WideString swFieldNameTemp = csFieldName;
  swFieldNameTemp.Replace(L"..", L".");

  if (pInterForm->CountFields(swFieldNameTemp) <= 0) {
    std::wstring strFieldName;
    int iControlNo = -1;
    ParseFieldName(swFieldNameTemp.c_str(), strFieldName, iControlNo);
    if (iControlNo == -1)
      return FALSE;

    m_FieldName = strFieldName.c_str();
    m_nFormControlIndex = iControlNo;
    return TRUE;
  }

  m_FieldName = swFieldNameTemp;
  m_nFormControlIndex = -1;

  return TRUE;
}

std::vector<CPDF_FormField*> Field::GetFormFields(
    CPDFSDK_Document* pDocument,
    const CFX_WideString& csFieldName) {
  std::vector<CPDF_FormField*> fields;
  CPDFSDK_InterForm* pReaderInterForm = pDocument->GetInterForm();
  CPDF_InterForm* pInterForm = pReaderInterForm->GetInterForm();
  for (int i = 0, sz = pInterForm->CountFields(csFieldName); i < sz; ++i) {
    if (CPDF_FormField* pFormField = pInterForm->GetField(i, csFieldName))
      fields.push_back(pFormField);
  }
  return fields;
}

std::vector<CPDF_FormField*> Field::GetFormFields(
    const CFX_WideString& csFieldName) const {
  return Field::GetFormFields(m_pDocument, csFieldName);
}

void Field::UpdateFormField(CPDFSDK_Document* pDocument,
                            CPDF_FormField* pFormField,
                            FX_BOOL bChangeMark,
                            FX_BOOL bResetAP,
                            FX_BOOL bRefresh) {
  std::vector<CPDFSDK_Widget*> widgets;
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  pInterForm->GetWidgets(pFormField, &widgets);

  if (bResetAP) {
    int nFieldType = pFormField->GetFieldType();
    if (nFieldType == FIELDTYPE_COMBOBOX || nFieldType == FIELDTYPE_TEXTFIELD) {
      for (CPDFSDK_Widget* pWidget : widgets) {
        FX_BOOL bFormatted = FALSE;
        CFX_WideString sValue = pWidget->OnFormat(bFormatted);
        pWidget->ResetAppearance(bFormatted ? sValue.c_str() : nullptr, FALSE);
      }
    } else {
      for (CPDFSDK_Widget* pWidget : widgets) {
        pWidget->ResetAppearance(nullptr, FALSE);
      }
    }
  }

  if (bRefresh) {
    for (CPDFSDK_Widget* pWidget : widgets) {
      CPDFSDK_Document* pDoc = pWidget->GetInterForm()->GetDocument();
      pDoc->UpdateAllViews(nullptr, pWidget);
    }
  }

  if (bChangeMark)
    pDocument->SetChangeMark();
}

void Field::UpdateFormControl(CPDFSDK_Document* pDocument,
                              CPDF_FormControl* pFormControl,
                              FX_BOOL bChangeMark,
                              FX_BOOL bResetAP,
                              FX_BOOL bRefresh) {
  ASSERT(pFormControl);

  CPDFSDK_InterForm* pForm = pDocument->GetInterForm();
  CPDFSDK_Widget* pWidget = pForm->GetWidget(pFormControl);

  if (pWidget) {
    if (bResetAP) {
      int nFieldType = pWidget->GetFieldType();
      if (nFieldType == FIELDTYPE_COMBOBOX ||
          nFieldType == FIELDTYPE_TEXTFIELD) {
        FX_BOOL bFormated = FALSE;
        CFX_WideString sValue = pWidget->OnFormat(bFormated);
        if (bFormated)
          pWidget->ResetAppearance(sValue.c_str(), FALSE);
        else
          pWidget->ResetAppearance(nullptr, FALSE);
      } else {
        pWidget->ResetAppearance(nullptr, FALSE);
      }
    }

    if (bRefresh) {
      CPDFSDK_InterForm* pInterForm = pWidget->GetInterForm();
      CPDFSDK_Document* pDoc = pInterForm->GetDocument();
      pDoc->UpdateAllViews(nullptr, pWidget);
    }
  }

  if (bChangeMark)
    pDocument->SetChangeMark();
}

CPDFSDK_Widget* Field::GetWidget(CPDFSDK_Document* pDocument,
                                 CPDF_FormControl* pFormControl) {
  CPDFSDK_InterForm* pInterForm =
      static_cast<CPDFSDK_InterForm*>(pDocument->GetInterForm());
  return pInterForm ? pInterForm->GetWidget(pFormControl) : nullptr;
}

FX_BOOL Field::ValueIsOccur(CPDF_FormField* pFormField,
                            CFX_WideString csOptLabel) {
  for (int i = 0, sz = pFormField->CountOptions(); i < sz; i++) {
    if (csOptLabel.Compare(pFormField->GetOptionLabel(i)) == 0)
      return TRUE;
  }

  return FALSE;
}

CPDF_FormControl* Field::GetSmartFieldControl(CPDF_FormField* pFormField) {
  if (!pFormField->CountControls() ||
      m_nFormControlIndex >= pFormField->CountControls())
    return nullptr;

  if (m_nFormControlIndex < 0)
    return pFormField->GetControl(0);

  return pFormField->GetControl(m_nFormControlIndex);
}

FX_BOOL Field::alignment(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_ByteString alignStr;
    vp >> alignStr;

    if (m_bDelay) {
      AddDelay_String(FP_ALIGNMENT, alignStr);
    } else {
      Field::SetAlignment(m_pDocument, m_FieldName, m_nFormControlIndex,
                          alignStr);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    switch (pFormControl->GetControlAlignment()) {
      case 1:
        vp << L"center";
        break;
      case 0:
        vp << L"left";
        break;
      case 2:
        vp << L"right";
        break;
      default:
        vp << L"";
    }
  }

  return TRUE;
}

void Field::SetAlignment(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         const CFX_ByteString& string) {
  // Not supported.
}

FX_BOOL Field::borderStyle(IJS_Context* cc,
                           CJS_PropValue& vp,
                           CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_ByteString strType = "";
    vp >> strType;

    if (m_bDelay) {
      AddDelay_String(FP_BORDERSTYLE, strType);
    } else {
      Field::SetBorderStyle(m_pDocument, m_FieldName, m_nFormControlIndex,
                            strType);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (!pFormField)
      return FALSE;

    CPDFSDK_Widget* pWidget =
        GetWidget(m_pDocument, GetSmartFieldControl(pFormField));
    if (!pWidget)
      return FALSE;

    switch (pWidget->GetBorderStyle()) {
      case BorderStyle::SOLID:
        vp << L"solid";
        break;
      case BorderStyle::DASH:
        vp << L"dashed";
        break;
      case BorderStyle::BEVELED:
        vp << L"beveled";
        break;
      case BorderStyle::INSET:
        vp << L"inset";
        break;
      case BorderStyle::UNDERLINE:
        vp << L"underline";
        break;
      default:
        vp << L"";
        break;
    }
  }

  return TRUE;
}

void Field::SetBorderStyle(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CFX_ByteString& string) {
  ASSERT(pDocument);

  BorderStyle nBorderStyle = BorderStyle::SOLID;
  if (string == "solid")
    nBorderStyle = BorderStyle::SOLID;
  else if (string == "beveled")
    nBorderStyle = BorderStyle::BEVELED;
  else if (string == "dashed")
    nBorderStyle = BorderStyle::DASH;
  else if (string == "inset")
    nBorderStyle = BorderStyle::INSET;
  else if (string == "underline")
    nBorderStyle = BorderStyle::UNDERLINE;
  else
    return;

  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);
  for (CPDF_FormField* pFormField : FieldArray) {
    if (nControlIndex < 0) {
      FX_BOOL bSet = FALSE;
      for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
        if (CPDFSDK_Widget* pWidget =
                GetWidget(pDocument, pFormField->GetControl(i))) {
          if (pWidget->GetBorderStyle() != nBorderStyle) {
            pWidget->SetBorderStyle(nBorderStyle);
            bSet = TRUE;
          }
        }
      }
      if (bSet)
        UpdateFormField(pDocument, pFormField, TRUE, TRUE, TRUE);
    } else {
      if (nControlIndex >= pFormField->CountControls())
        return;
      if (CPDF_FormControl* pFormControl =
              pFormField->GetControl(nControlIndex)) {
        if (CPDFSDK_Widget* pWidget = GetWidget(pDocument, pFormControl)) {
          if (pWidget->GetBorderStyle() != nBorderStyle) {
            pWidget->SetBorderStyle(nBorderStyle);
            UpdateFormControl(pDocument, pFormControl, TRUE, TRUE, TRUE);
          }
        }
      }
    }
  }
}

FX_BOOL Field::buttonAlignX(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_BUTTONALIGNX, nVP);
    } else {
      Field::SetButtonAlignX(m_pDocument, m_FieldName, m_nFormControlIndex,
                             nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDF_IconFit IconFit = pFormControl->GetIconFit();

    FX_FLOAT fLeft, fBottom;
    IconFit.GetIconPosition(fLeft, fBottom);

    vp << (int32_t)fLeft;
  }

  return TRUE;
}

void Field::SetButtonAlignX(CPDFSDK_Document* pDocument,
                            const CFX_WideString& swFieldName,
                            int nControlIndex,
                            int number) {
  // Not supported.
}

FX_BOOL Field::buttonAlignY(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_BUTTONALIGNY, nVP);
    } else {
      Field::SetButtonAlignY(m_pDocument, m_FieldName, m_nFormControlIndex,
                             nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDF_IconFit IconFit = pFormControl->GetIconFit();

    FX_FLOAT fLeft, fBottom;
    IconFit.GetIconPosition(fLeft, fBottom);

    vp << (int32_t)fBottom;
  }

  return TRUE;
}

void Field::SetButtonAlignY(CPDFSDK_Document* pDocument,
                            const CFX_WideString& swFieldName,
                            int nControlIndex,
                            int number) {
  // Not supported.
}

FX_BOOL Field::buttonFitBounds(IJS_Context* cc,
                               CJS_PropValue& vp,
                               CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_BUTTONFITBOUNDS, bVP);
    } else {
      Field::SetButtonFitBounds(m_pDocument, m_FieldName, m_nFormControlIndex,
                                bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    vp << pFormControl->GetIconFit().GetFittingBounds();
  }

  return TRUE;
}

void Field::SetButtonFitBounds(CPDFSDK_Document* pDocument,
                               const CFX_WideString& swFieldName,
                               int nControlIndex,
                               bool b) {
  // Not supported.
}

FX_BOOL Field::buttonPosition(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_BUTTONPOSITION, nVP);
    } else {
      Field::SetButtonPosition(m_pDocument, m_FieldName, m_nFormControlIndex,
                               nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    vp << pFormControl->GetTextPosition();
  }
  return TRUE;
}

void Field::SetButtonPosition(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              int number) {
  // Not supported.
}

FX_BOOL Field::buttonScaleHow(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_BUTTONSCALEHOW, nVP);
    } else {
      Field::SetButtonScaleHow(m_pDocument, m_FieldName, m_nFormControlIndex,
                               nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDF_IconFit IconFit = pFormControl->GetIconFit();
    if (IconFit.IsProportionalScale())
      vp << (int32_t)0;
    else
      vp << (int32_t)1;
  }

  return TRUE;
}

void Field::SetButtonScaleHow(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              int number) {
  // Not supported.
}

FX_BOOL Field::buttonScaleWhen(IJS_Context* cc,
                               CJS_PropValue& vp,
                               CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_BUTTONSCALEWHEN, nVP);
    } else {
      Field::SetButtonScaleWhen(m_pDocument, m_FieldName, m_nFormControlIndex,
                                nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDF_IconFit IconFit = pFormControl->GetIconFit();
    int ScaleM = IconFit.GetScaleMethod();
    switch (ScaleM) {
      case CPDF_IconFit::Always:
        vp << (int32_t)CPDF_IconFit::Always;
        break;
      case CPDF_IconFit::Bigger:
        vp << (int32_t)CPDF_IconFit::Bigger;
        break;
      case CPDF_IconFit::Never:
        vp << (int32_t)CPDF_IconFit::Never;
        break;
      case CPDF_IconFit::Smaller:
        vp << (int32_t)CPDF_IconFit::Smaller;
        break;
    }
  }

  return TRUE;
}

void Field::SetButtonScaleWhen(CPDFSDK_Document* pDocument,
                               const CFX_WideString& swFieldName,
                               int nControlIndex,
                               int number) {
  // Not supported.
}

FX_BOOL Field::calcOrderIndex(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_CALCORDERINDEX, nVP);
    } else {
      Field::SetCalcOrderIndex(m_pDocument, m_FieldName, m_nFormControlIndex,
                               nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_COMBOBOX &&
        pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD) {
      return FALSE;
    }

    CPDFSDK_InterForm* pRDInterForm = m_pDocument->GetInterForm();
    CPDF_InterForm* pInterForm = pRDInterForm->GetInterForm();
    vp << (int32_t)pInterForm->FindFieldInCalculationOrder(pFormField);
  }

  return TRUE;
}

void Field::SetCalcOrderIndex(CPDFSDK_Document* pDocument,
                              const CFX_WideString& swFieldName,
                              int nControlIndex,
                              int number) {
  // Not supported.
}

FX_BOOL Field::charLimit(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_CHARLIMIT, nVP);
    } else {
      Field::SetCharLimit(m_pDocument, m_FieldName, m_nFormControlIndex, nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    vp << (int32_t)pFormField->GetMaxLen();
  }
  return TRUE;
}

void Field::SetCharLimit(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         int number) {
  // Not supported.
}

FX_BOOL Field::comb(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_COMB, bVP);
    } else {
      Field::SetComb(m_pDocument, m_FieldName, m_nFormControlIndex, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_COMB)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetComb(CPDFSDK_Document* pDocument,
                    const CFX_WideString& swFieldName,
                    int nControlIndex,
                    bool b) {
  // Not supported.
}

FX_BOOL Field::commitOnSelChange(IJS_Context* cc,
                                 CJS_PropValue& vp,
                                 CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_COMMITONSELCHANGE, bVP);
    } else {
      Field::SetCommitOnSelChange(m_pDocument, m_FieldName, m_nFormControlIndex,
                                  bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_COMBOBOX &&
        pFormField->GetFieldType() != FIELDTYPE_LISTBOX) {
      return FALSE;
    }

    if (pFormField->GetFieldFlags() & FIELDFLAG_COMMITONSELCHANGE)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetCommitOnSelChange(CPDFSDK_Document* pDocument,
                                 const CFX_WideString& swFieldName,
                                 int nControlIndex,
                                 bool b) {
  // Not supported.
}

FX_BOOL Field::currentValueIndices(IJS_Context* cc,
                                   CJS_PropValue& vp,
                                   CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    std::vector<uint32_t> array;
    if (vp.GetType() == CJS_Value::VT_number) {
      int iSelecting = 0;
      vp >> iSelecting;
      array.push_back(iSelecting);
    } else if (vp.IsArrayObject()) {
      CJS_Array SelArray(pRuntime);
      CJS_Value SelValue(pRuntime);
      int iSelecting;
      vp >> SelArray;
      for (int i = 0, sz = SelArray.GetLength(); i < sz; i++) {
        SelArray.GetElement(i, SelValue);
        iSelecting = SelValue.ToInt();
        array.push_back(iSelecting);
      }
    }

    if (m_bDelay) {
      AddDelay_WordArray(FP_CURRENTVALUEINDICES, array);
    } else {
      Field::SetCurrentValueIndices(m_pDocument, m_FieldName,
                                    m_nFormControlIndex, array);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_COMBOBOX &&
        pFormField->GetFieldType() != FIELDTYPE_LISTBOX) {
      return FALSE;
    }

    if (pFormField->CountSelectedItems() == 1) {
      vp << pFormField->GetSelectedIndex(0);
    } else if (pFormField->CountSelectedItems() > 1) {
      CJS_Array SelArray(pRuntime);
      for (int i = 0, sz = pFormField->CountSelectedItems(); i < sz; i++) {
        SelArray.SetElement(
            i, CJS_Value(pRuntime, pFormField->GetSelectedIndex(i)));
      }
      vp << SelArray;
    } else {
      vp << -1;
    }
  }

  return TRUE;
}

void Field::SetCurrentValueIndices(CPDFSDK_Document* pDocument,
                                   const CFX_WideString& swFieldName,
                                   int nControlIndex,
                                   const std::vector<uint32_t>& array) {
  ASSERT(pDocument);
  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);

  for (CPDF_FormField* pFormField : FieldArray) {
    int nFieldType = pFormField->GetFieldType();
    if (nFieldType == FIELDTYPE_COMBOBOX || nFieldType == FIELDTYPE_LISTBOX) {
      uint32_t dwFieldFlags = pFormField->GetFieldFlags();
      pFormField->ClearSelection(TRUE);
      for (size_t i = 0; i < array.size(); ++i) {
        if (i != 0 && !(dwFieldFlags & (1 << 21)))
          break;
        if (array[i] < static_cast<uint32_t>(pFormField->CountOptions()) &&
            !pFormField->IsItemSelected(array[i])) {
          pFormField->SetItemSelection(array[i], TRUE);
        }
      }
      UpdateFormField(pDocument, pFormField, TRUE, TRUE, TRUE);
    }
  }
}

FX_BOOL Field::defaultStyle(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  return FALSE;
}

void Field::SetDefaultStyle(CPDFSDK_Document* pDocument,
                            const CFX_WideString& swFieldName,
                            int nControlIndex) {
  // Not supported.
}

FX_BOOL Field::defaultValue(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_WideString WideStr;
    vp >> WideStr;

    if (m_bDelay) {
      AddDelay_WideString(FP_DEFAULTVALUE, WideStr);
    } else {
      Field::SetDefaultValue(m_pDocument, m_FieldName, m_nFormControlIndex,
                             WideStr);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() == FIELDTYPE_PUSHBUTTON ||
        pFormField->GetFieldType() == FIELDTYPE_SIGNATURE) {
      return FALSE;
    }

    vp << pFormField->GetDefaultValue();
  }
  return TRUE;
}

void Field::SetDefaultValue(CPDFSDK_Document* pDocument,
                            const CFX_WideString& swFieldName,
                            int nControlIndex,
                            const CFX_WideString& string) {
  // Not supported.
}

FX_BOOL Field::doNotScroll(IJS_Context* cc,
                           CJS_PropValue& vp,
                           CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_DONOTSCROLL, bVP);
    } else {
      Field::SetDoNotScroll(m_pDocument, m_FieldName, m_nFormControlIndex, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_DONOTSCROLL)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetDoNotScroll(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           bool b) {
  // Not supported.
}

FX_BOOL Field::doNotSpellCheck(IJS_Context* cc,
                               CJS_PropValue& vp,
                               CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD &&
        pFormField->GetFieldType() != FIELDTYPE_COMBOBOX) {
      return FALSE;
    }

    if (pFormField->GetFieldFlags() & FIELDFLAG_DONOTSPELLCHECK)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetDelay(FX_BOOL bDelay) {
  m_bDelay = bDelay;

  if (!m_bDelay) {
    if (m_pJSDoc)
      m_pJSDoc->DoFieldDelay(m_FieldName, m_nFormControlIndex);
  }
}

FX_BOOL Field::delay(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    SetDelay(bVP);
  } else {
    vp << m_bDelay;
  }
  return TRUE;
}

FX_BOOL Field::display(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError) {
  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_DISPLAY, nVP);
    } else {
      Field::SetDisplay(m_pDocument, m_FieldName, m_nFormControlIndex, nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
    CPDFSDK_Widget* pWidget =
        pInterForm->GetWidget(GetSmartFieldControl(pFormField));
    if (!pWidget)
      return FALSE;

    uint32_t dwFlag = pWidget->GetFlags();

    if (ANNOTFLAG_INVISIBLE & dwFlag || ANNOTFLAG_HIDDEN & dwFlag) {
      vp << (int32_t)1;
    } else {
      if (ANNOTFLAG_PRINT & dwFlag) {
        if (ANNOTFLAG_NOVIEW & dwFlag) {
          vp << (int32_t)3;
        } else {
          vp << (int32_t)0;
        }
      } else {
        vp << (int32_t)2;
      }
    }
  }

  return TRUE;
}

void Field::SetDisplay(CPDFSDK_Document* pDocument,
                       const CFX_WideString& swFieldName,
                       int nControlIndex,
                       int number) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);
  for (CPDF_FormField* pFormField : FieldArray) {
    if (nControlIndex < 0) {
      FX_BOOL bSet = FALSE;
      for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
        CPDF_FormControl* pFormControl = pFormField->GetControl(i);
        ASSERT(pFormControl);

        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          uint32_t dwFlag = pWidget->GetFlags();
          switch (number) {
            case 0:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              dwFlag |= ANNOTFLAG_PRINT;
              break;
            case 1:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              dwFlag |= (ANNOTFLAG_HIDDEN | ANNOTFLAG_PRINT);
              break;
            case 2:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_PRINT);
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              break;
            case 3:
              dwFlag |= ANNOTFLAG_NOVIEW;
              dwFlag |= ANNOTFLAG_PRINT;
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              break;
          }

          if (dwFlag != pWidget->GetFlags()) {
            pWidget->SetFlags(dwFlag);
            bSet = TRUE;
          }
        }
      }

      if (bSet)
        UpdateFormField(pDocument, pFormField, TRUE, FALSE, TRUE);
    } else {
      if (nControlIndex >= pFormField->CountControls())
        return;
      if (CPDF_FormControl* pFormControl =
              pFormField->GetControl(nControlIndex)) {
        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          uint32_t dwFlag = pWidget->GetFlags();
          switch (number) {
            case 0:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              dwFlag |= ANNOTFLAG_PRINT;
              break;
            case 1:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              dwFlag |= (ANNOTFLAG_HIDDEN | ANNOTFLAG_PRINT);
              break;
            case 2:
              dwFlag &= (~ANNOTFLAG_INVISIBLE);
              dwFlag &= (~ANNOTFLAG_PRINT);
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              dwFlag &= (~ANNOTFLAG_NOVIEW);
              break;
            case 3:
              dwFlag |= ANNOTFLAG_NOVIEW;
              dwFlag |= ANNOTFLAG_PRINT;
              dwFlag &= (~ANNOTFLAG_HIDDEN);
              break;
          }
          if (dwFlag != pWidget->GetFlags()) {
            pWidget->SetFlags(dwFlag);
            UpdateFormControl(pDocument, pFormControl, TRUE, FALSE, TRUE);
          }
        }
      }
    }
  }
}

FX_BOOL Field::doc(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError) {
  if (!vp.IsGetting()) {
    return FALSE;
  }
  vp << m_pJSDoc->GetCJSDoc();
  return TRUE;
}

FX_BOOL Field::editable(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_COMBOBOX)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_EDIT)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::exportValues(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_CHECKBOX &&
      pFormField->GetFieldType() != FIELDTYPE_RADIOBUTTON) {
    return FALSE;
  }

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    if (!vp.IsArrayObject())
      return FALSE;
  } else {
    CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
    CJS_Array ExportValusArray(pRuntime);
    if (m_nFormControlIndex < 0) {
      for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
        CPDF_FormControl* pFormControl = pFormField->GetControl(i);
        ExportValusArray.SetElement(
            i, CJS_Value(pRuntime, pFormControl->GetExportValue().c_str()));
      }
    } else {
      if (m_nFormControlIndex >= pFormField->CountControls())
        return FALSE;

      CPDF_FormControl* pFormControl =
          pFormField->GetControl(m_nFormControlIndex);
      if (!pFormControl)
        return FALSE;

      ExportValusArray.SetElement(
          0, CJS_Value(pRuntime, pFormControl->GetExportValue().c_str()));
    }
    vp << ExportValusArray;
  }
  return TRUE;
}

FX_BOOL Field::fileSelect(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;
  } else {
    if (pFormField->GetFieldFlags() & FIELDFLAG_FILESELECT)
      vp << true;
    else
      vp << false;
  }
  return TRUE;
}

FX_BOOL Field::fillColor(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array crArray(pRuntime);
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    if (!vp.IsArrayObject())
      return FALSE;

    vp >> crArray;

    CPWL_Color color;
    color::ConvertArrayToPWLColor(crArray, color);
    if (m_bDelay) {
      AddDelay_Color(FP_FILLCOLOR, color);
    } else {
      Field::SetFillColor(m_pDocument, m_FieldName, m_nFormControlIndex, color);
    }
  } else {
    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    int iColorType;
    pFormControl->GetBackgroundColor(iColorType);

    CPWL_Color color;
    if (iColorType == COLORTYPE_TRANSPARENT) {
      color = CPWL_Color(COLORTYPE_TRANSPARENT);
    } else if (iColorType == COLORTYPE_GRAY) {
      color = CPWL_Color(COLORTYPE_GRAY,
                         pFormControl->GetOriginalBackgroundColor(0));
    } else if (iColorType == COLORTYPE_RGB) {
      color =
          CPWL_Color(COLORTYPE_RGB, pFormControl->GetOriginalBackgroundColor(0),
                     pFormControl->GetOriginalBackgroundColor(1),
                     pFormControl->GetOriginalBackgroundColor(2));
    } else if (iColorType == COLORTYPE_CMYK) {
      color = CPWL_Color(COLORTYPE_CMYK,
                         pFormControl->GetOriginalBackgroundColor(0),
                         pFormControl->GetOriginalBackgroundColor(1),
                         pFormControl->GetOriginalBackgroundColor(2),
                         pFormControl->GetOriginalBackgroundColor(3));
    } else {
      return FALSE;
    }

    color::ConvertPWLColorToArray(color, crArray);
    vp << crArray;
  }

  return TRUE;
}

void Field::SetFillColor(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         const CPWL_Color& color) {
  // Not supported.
}

FX_BOOL Field::hidden(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_HIDDEN, bVP);
    } else {
      Field::SetHidden(m_pDocument, m_FieldName, m_nFormControlIndex, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
    CPDFSDK_Widget* pWidget =
        pInterForm->GetWidget(GetSmartFieldControl(pFormField));
    if (!pWidget)
      return FALSE;

    uint32_t dwFlags = pWidget->GetFlags();

    if (ANNOTFLAG_INVISIBLE & dwFlags || ANNOTFLAG_HIDDEN & dwFlags)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetHidden(CPDFSDK_Document* pDocument,
                      const CFX_WideString& swFieldName,
                      int nControlIndex,
                      bool b) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);
  for (CPDF_FormField* pFormField : FieldArray) {
    if (nControlIndex < 0) {
      FX_BOOL bSet = FALSE;
      for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
        if (CPDFSDK_Widget* pWidget =
                pInterForm->GetWidget(pFormField->GetControl(i))) {
          uint32_t dwFlags = pWidget->GetFlags();

          if (b) {
            dwFlags &= (~ANNOTFLAG_INVISIBLE);
            dwFlags &= (~ANNOTFLAG_NOVIEW);
            dwFlags |= (ANNOTFLAG_HIDDEN | ANNOTFLAG_PRINT);
          } else {
            dwFlags &= (~ANNOTFLAG_INVISIBLE);
            dwFlags &= (~ANNOTFLAG_HIDDEN);
            dwFlags &= (~ANNOTFLAG_NOVIEW);
            dwFlags |= ANNOTFLAG_PRINT;
          }

          if (dwFlags != pWidget->GetFlags()) {
            pWidget->SetFlags(dwFlags);
            bSet = TRUE;
          }
        }
      }

      if (bSet)
        UpdateFormField(pDocument, pFormField, TRUE, FALSE, TRUE);
    } else {
      if (nControlIndex >= pFormField->CountControls())
        return;
      if (CPDF_FormControl* pFormControl =
              pFormField->GetControl(nControlIndex)) {
        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          uint32_t dwFlags = pWidget->GetFlags();

          if (b) {
            dwFlags &= (~ANNOTFLAG_INVISIBLE);
            dwFlags &= (~ANNOTFLAG_NOVIEW);
            dwFlags |= (ANNOTFLAG_HIDDEN | ANNOTFLAG_PRINT);
          } else {
            dwFlags &= (~ANNOTFLAG_INVISIBLE);
            dwFlags &= (~ANNOTFLAG_HIDDEN);
            dwFlags &= (~ANNOTFLAG_NOVIEW);
            dwFlags |= ANNOTFLAG_PRINT;
          }

          if (dwFlags != pWidget->GetFlags()) {
            pWidget->SetFlags(dwFlags);
            UpdateFormControl(pDocument, pFormControl, TRUE, FALSE, TRUE);
          }
        }
      }
    }
  }
}

FX_BOOL Field::highlight(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_ByteString strMode;
    vp >> strMode;

    if (m_bDelay) {
      AddDelay_String(FP_HIGHLIGHT, strMode);
    } else {
      Field::SetHighlight(m_pDocument, m_FieldName, m_nFormControlIndex,
                          strMode);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
      return FALSE;

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    int eHM = pFormControl->GetHighlightingMode();
    switch (eHM) {
      case CPDF_FormControl::None:
        vp << L"none";
        break;
      case CPDF_FormControl::Push:
        vp << L"push";
        break;
      case CPDF_FormControl::Invert:
        vp << L"invert";
        break;
      case CPDF_FormControl::Outline:
        vp << L"outline";
        break;
      case CPDF_FormControl::Toggle:
        vp << L"toggle";
        break;
    }
  }

  return TRUE;
}

void Field::SetHighlight(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         const CFX_ByteString& string) {
  // Not supported.
}

FX_BOOL Field::lineWidth(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int iWidth;
    vp >> iWidth;

    if (m_bDelay) {
      AddDelay_Int(FP_LINEWIDTH, iWidth);
    } else {
      Field::SetLineWidth(m_pDocument, m_FieldName, m_nFormControlIndex,
                          iWidth);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
    if (!pFormField->CountControls())
      return FALSE;

    CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormField->GetControl(0));
    if (!pWidget)
      return FALSE;

    vp << (int32_t)pWidget->GetBorderWidth();
  }

  return TRUE;
}

void Field::SetLineWidth(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         int number) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);
  for (CPDF_FormField* pFormField : FieldArray) {
    if (nControlIndex < 0) {
      FX_BOOL bSet = FALSE;
      for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
        CPDF_FormControl* pFormControl = pFormField->GetControl(i);
        ASSERT(pFormControl);

        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          if (number != pWidget->GetBorderWidth()) {
            pWidget->SetBorderWidth(number);
            bSet = TRUE;
          }
        }
      }
      if (bSet)
        UpdateFormField(pDocument, pFormField, TRUE, TRUE, TRUE);
    } else {
      if (nControlIndex >= pFormField->CountControls())
        return;
      if (CPDF_FormControl* pFormControl =
              pFormField->GetControl(nControlIndex)) {
        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          if (number != pWidget->GetBorderWidth()) {
            pWidget->SetBorderWidth(number);
            UpdateFormControl(pDocument, pFormControl, TRUE, TRUE, TRUE);
          }
        }
      }
    }
  }
}

FX_BOOL Field::multiline(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_MULTILINE, bVP);
    } else {
      Field::SetMultiline(m_pDocument, m_FieldName, m_nFormControlIndex, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_MULTILINE)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetMultiline(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         bool b) {
  // Not supported.
}

FX_BOOL Field::multipleSelection(IJS_Context* cc,
                                 CJS_PropValue& vp,
                                 CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_MULTIPLESELECTION, bVP);
    } else {
      Field::SetMultipleSelection(m_pDocument, m_FieldName, m_nFormControlIndex,
                                  bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_LISTBOX)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_MULTISELECT)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetMultipleSelection(CPDFSDK_Document* pDocument,
                                 const CFX_WideString& swFieldName,
                                 int nControlIndex,
                                 bool b) {
  // Not supported.
}

FX_BOOL Field::name(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  vp << m_FieldName;

  return TRUE;
}

FX_BOOL Field::numItems(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_COMBOBOX &&
      pFormField->GetFieldType() != FIELDTYPE_LISTBOX) {
    return FALSE;
  }

  vp << (int32_t)pFormField->CountOptions();
  return TRUE;
}

FX_BOOL Field::page(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (!pFormField)
    return FALSE;

  std::vector<CPDFSDK_Widget*> widgets;
  m_pDocument->GetInterForm()->GetWidgets(pFormField, &widgets);

  if (widgets.empty()) {
    vp << (int32_t)-1;
    return TRUE;
  }

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array PageArray(pRuntime);
  for (size_t i = 0; i < widgets.size(); ++i) {
    CPDFSDK_PageView* pPageView = widgets[i]->GetPageView();
    if (!pPageView)
      return FALSE;

    PageArray.SetElement(
        i, CJS_Value(pRuntime, (int32_t)pPageView->GetPageIndex()));
  }

  vp << PageArray;
  return TRUE;
}

FX_BOOL Field::password(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_PASSWORD, bVP);
    } else {
      Field::SetPassword(m_pDocument, m_FieldName, m_nFormControlIndex, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_PASSWORD)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

void Field::SetPassword(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        bool b) {
  // Not supported.
}

FX_BOOL Field::print(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    for (CPDF_FormField* pFormField : FieldArray) {
      if (m_nFormControlIndex < 0) {
        FX_BOOL bSet = FALSE;
        for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
          if (CPDFSDK_Widget* pWidget =
                  pInterForm->GetWidget(pFormField->GetControl(i))) {
            uint32_t dwFlags = pWidget->GetFlags();
            if (bVP)
              dwFlags |= ANNOTFLAG_PRINT;
            else
              dwFlags &= ~ANNOTFLAG_PRINT;

            if (dwFlags != pWidget->GetFlags()) {
              pWidget->SetFlags(dwFlags);
              bSet = TRUE;
            }
          }
        }

        if (bSet)
          UpdateFormField(m_pDocument, pFormField, TRUE, FALSE, TRUE);
      } else {
        if (m_nFormControlIndex >= pFormField->CountControls())
          return FALSE;
        if (CPDF_FormControl* pFormControl =
                pFormField->GetControl(m_nFormControlIndex)) {
          if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
            uint32_t dwFlags = pWidget->GetFlags();
            if (bVP)
              dwFlags |= ANNOTFLAG_PRINT;
            else
              dwFlags &= ~ANNOTFLAG_PRINT;

            if (dwFlags != pWidget->GetFlags()) {
              pWidget->SetFlags(dwFlags);
              UpdateFormControl(m_pDocument,
                                pFormField->GetControl(m_nFormControlIndex),
                                TRUE, FALSE, TRUE);
            }
          }
        }
      }
    }
  } else {
    CPDF_FormField* pFormField = FieldArray[0];
    CPDFSDK_Widget* pWidget =
        pInterForm->GetWidget(GetSmartFieldControl(pFormField));
    if (!pWidget)
      return FALSE;

    if (pWidget->GetFlags() & ANNOTFLAG_PRINT)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::radiosInUnison(IJS_Context* cc,
                              CJS_PropValue& vp,
                              CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

  } else {
    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_RADIOBUTTON)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_RADIOSINUNISON)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::readonly(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

  } else {
    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldFlags() & FIELDFLAG_READONLY)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::rect(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Value Upper_Leftx(pRuntime);
  CJS_Value Upper_Lefty(pRuntime);
  CJS_Value Lower_Rightx(pRuntime);
  CJS_Value Lower_Righty(pRuntime);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;
    if (!vp.IsArrayObject())
      return FALSE;

    CJS_Array rcArray(pRuntime);
    vp >> rcArray;
    rcArray.GetElement(0, Upper_Leftx);
    rcArray.GetElement(1, Upper_Lefty);
    rcArray.GetElement(2, Lower_Rightx);
    rcArray.GetElement(3, Lower_Righty);

    FX_FLOAT pArray[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    pArray[0] = (FX_FLOAT)Upper_Leftx.ToInt();
    pArray[1] = (FX_FLOAT)Lower_Righty.ToInt();
    pArray[2] = (FX_FLOAT)Lower_Rightx.ToInt();
    pArray[3] = (FX_FLOAT)Upper_Lefty.ToInt();

    CFX_FloatRect crRect(pArray);
    if (m_bDelay) {
      AddDelay_Rect(FP_RECT, crRect);
    } else {
      Field::SetRect(m_pDocument, m_FieldName, m_nFormControlIndex, crRect);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
    CPDFSDK_Widget* pWidget =
        pInterForm->GetWidget(GetSmartFieldControl(pFormField));
    if (!pWidget)
      return FALSE;

    CFX_FloatRect crRect = pWidget->GetRect();
    Upper_Leftx = (int32_t)crRect.left;
    Upper_Lefty = (int32_t)crRect.top;
    Lower_Rightx = (int32_t)crRect.right;
    Lower_Righty = (int32_t)crRect.bottom;

    CJS_Array rcArray(pRuntime);
    rcArray.SetElement(0, Upper_Leftx);
    rcArray.SetElement(1, Upper_Lefty);
    rcArray.SetElement(2, Lower_Rightx);
    rcArray.SetElement(3, Lower_Righty);
    vp << rcArray;
  }
  return TRUE;
}

void Field::SetRect(CPDFSDK_Document* pDocument,
                    const CFX_WideString& swFieldName,
                    int nControlIndex,
                    const CFX_FloatRect& rect) {
  CPDFSDK_InterForm* pInterForm = pDocument->GetInterForm();
  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);
  for (CPDF_FormField* pFormField : FieldArray) {
    if (nControlIndex < 0) {
      FX_BOOL bSet = FALSE;
      for (int i = 0, sz = pFormField->CountControls(); i < sz; ++i) {
        CPDF_FormControl* pFormControl = pFormField->GetControl(i);
        ASSERT(pFormControl);

        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          CFX_FloatRect crRect = rect;

          CPDF_Page* pPDFPage = pWidget->GetPDFPage();
          crRect.Intersect(pPDFPage->GetPageBBox());

          if (!crRect.IsEmpty()) {
            CFX_FloatRect rcOld = pWidget->GetRect();
            if (crRect.left != rcOld.left || crRect.right != rcOld.right ||
                crRect.top != rcOld.top || crRect.bottom != rcOld.bottom) {
              pWidget->SetRect(crRect);
              bSet = TRUE;
            }
          }
        }
      }

      if (bSet)
        UpdateFormField(pDocument, pFormField, TRUE, TRUE, TRUE);
    } else {
      if (nControlIndex >= pFormField->CountControls())
        return;
      if (CPDF_FormControl* pFormControl =
              pFormField->GetControl(nControlIndex)) {
        if (CPDFSDK_Widget* pWidget = pInterForm->GetWidget(pFormControl)) {
          CFX_FloatRect crRect = rect;

          CPDF_Page* pPDFPage = pWidget->GetPDFPage();
          crRect.Intersect(pPDFPage->GetPageBBox());

          if (!crRect.IsEmpty()) {
            CFX_FloatRect rcOld = pWidget->GetRect();
            if (crRect.left != rcOld.left || crRect.right != rcOld.right ||
                crRect.top != rcOld.top || crRect.bottom != rcOld.bottom) {
              pWidget->SetRect(crRect);
              UpdateFormControl(pDocument, pFormControl, TRUE, TRUE, TRUE);
            }
          }
        }
      }
    }
  }
}

FX_BOOL Field::required(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

  } else {
    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() == FIELDTYPE_PUSHBUTTON)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_REQUIRED)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::richText(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    bool bVP;
    vp >> bVP;

    if (m_bDelay) {
      AddDelay_Bool(FP_RICHTEXT, bVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_TEXTFIELD)
      return FALSE;

    if (pFormField->GetFieldFlags() & FIELDFLAG_RICHTEXT)
      vp << true;
    else
      vp << false;
  }

  return TRUE;
}

FX_BOOL Field::richValue(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::rotation(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_ROTATION, nVP);
    } else {
      Field::SetRotation(m_pDocument, m_FieldName, m_nFormControlIndex, nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    vp << (int32_t)pFormControl->GetRotation();
  }

  return TRUE;
}

void Field::SetRotation(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        int number) {
  // Not supported.
}

FX_BOOL Field::strokeColor(IJS_Context* cc,
                           CJS_PropValue& vp,
                           CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array crArray(pRuntime);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    if (!vp.IsArrayObject())
      return FALSE;

    vp >> crArray;

    CPWL_Color color;
    color::ConvertArrayToPWLColor(crArray, color);

    if (m_bDelay) {
      AddDelay_Color(FP_STROKECOLOR, color);
    } else {
      Field::SetStrokeColor(m_pDocument, m_FieldName, m_nFormControlIndex,
                            color);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    int iColorType;
    pFormControl->GetBorderColor(iColorType);

    CPWL_Color color;
    if (iColorType == COLORTYPE_TRANSPARENT) {
      color = CPWL_Color(COLORTYPE_TRANSPARENT);
    } else if (iColorType == COLORTYPE_GRAY) {
      color =
          CPWL_Color(COLORTYPE_GRAY, pFormControl->GetOriginalBorderColor(0));
    } else if (iColorType == COLORTYPE_RGB) {
      color = CPWL_Color(COLORTYPE_RGB, pFormControl->GetOriginalBorderColor(0),
                         pFormControl->GetOriginalBorderColor(1),
                         pFormControl->GetOriginalBorderColor(2));
    } else if (iColorType == COLORTYPE_CMYK) {
      color =
          CPWL_Color(COLORTYPE_CMYK, pFormControl->GetOriginalBorderColor(0),
                     pFormControl->GetOriginalBorderColor(1),
                     pFormControl->GetOriginalBorderColor(2),
                     pFormControl->GetOriginalBorderColor(3));
    } else {
      return FALSE;
    }

    color::ConvertPWLColorToArray(color, crArray);
    vp << crArray;
  }
  return TRUE;
}

void Field::SetStrokeColor(CPDFSDK_Document* pDocument,
                           const CFX_WideString& swFieldName,
                           int nControlIndex,
                           const CPWL_Color& color) {
  // Not supported.
}

FX_BOOL Field::style(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_ByteString csBCaption;
    vp >> csBCaption;

    if (m_bDelay) {
      AddDelay_String(FP_STYLE, csBCaption);
    } else {
      Field::SetStyle(m_pDocument, m_FieldName, m_nFormControlIndex,
                      csBCaption);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    if (pFormField->GetFieldType() != FIELDTYPE_RADIOBUTTON &&
        pFormField->GetFieldType() != FIELDTYPE_CHECKBOX) {
      return FALSE;
    }

    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CFX_WideString csWCaption = pFormControl->GetNormalCaption();
    CFX_ByteString csBCaption;

    switch (csWCaption[0]) {
      case L'l':
        csBCaption = "circle";
        break;
      case L'8':
        csBCaption = "cross";
        break;
      case L'u':
        csBCaption = "diamond";
        break;
      case L'n':
        csBCaption = "square";
        break;
      case L'H':
        csBCaption = "star";
        break;
      default:  // L'4'
        csBCaption = "check";
        break;
    }
    vp << csBCaption;
  }

  return TRUE;
}

void Field::SetStyle(CPDFSDK_Document* pDocument,
                     const CFX_WideString& swFieldName,
                     int nControlIndex,
                     const CFX_ByteString& string) {
  // Not supported.
}

FX_BOOL Field::submitName(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::textColor(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array crArray(pRuntime);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    if (!vp.IsArrayObject())
      return FALSE;

    vp >> crArray;

    CPWL_Color color;
    color::ConvertArrayToPWLColor(crArray, color);

    if (m_bDelay) {
      AddDelay_Color(FP_TEXTCOLOR, color);
    } else {
      Field::SetTextColor(m_pDocument, m_FieldName, m_nFormControlIndex, color);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    int iColorType;
    FX_ARGB color;
    CPDF_DefaultAppearance FieldAppearance =
        pFormControl->GetDefaultAppearance();
    FieldAppearance.GetColor(color, iColorType);
    int32_t a, r, g, b;
    ArgbDecode(color, a, r, g, b);

    CPWL_Color crRet =
        CPWL_Color(COLORTYPE_RGB, r / 255.0f, g / 255.0f, b / 255.0f);

    if (iColorType == COLORTYPE_TRANSPARENT)
      crRet = CPWL_Color(COLORTYPE_TRANSPARENT);

    color::ConvertPWLColorToArray(crRet, crArray);
    vp << crArray;
  }
  return TRUE;
}

void Field::SetTextColor(CPDFSDK_Document* pDocument,
                         const CFX_WideString& swFieldName,
                         int nControlIndex,
                         const CPWL_Color& color) {
  // Not supported.
}

FX_BOOL Field::textFont(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_ByteString csFontName;
    vp >> csFontName;
    if (csFontName.IsEmpty())
      return FALSE;

    if (m_bDelay) {
      AddDelay_String(FP_TEXTFONT, csFontName);
    } else {
      Field::SetTextFont(m_pDocument, m_FieldName, m_nFormControlIndex,
                         csFontName);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    int nFieldType = pFormField->GetFieldType();

    if (nFieldType == FIELDTYPE_PUSHBUTTON ||
        nFieldType == FIELDTYPE_COMBOBOX || nFieldType == FIELDTYPE_LISTBOX ||
        nFieldType == FIELDTYPE_TEXTFIELD) {
      CPDF_Font* pFont = pFormControl->GetDefaultControlFont();
      if (!pFont)
        return FALSE;

      vp << pFont->GetBaseFont();
    } else {
      return FALSE;
    }
  }

  return TRUE;
}

void Field::SetTextFont(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        const CFX_ByteString& string) {
  // Not supported.
}

FX_BOOL Field::textSize(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    int nVP;
    vp >> nVP;

    if (m_bDelay) {
      AddDelay_Int(FP_TEXTSIZE, nVP);
    } else {
      Field::SetTextSize(m_pDocument, m_FieldName, m_nFormControlIndex, nVP);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    ASSERT(pFormField);
    CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
    if (!pFormControl)
      return FALSE;

    CPDF_DefaultAppearance FieldAppearance =
        pFormControl->GetDefaultAppearance();

    CFX_ByteString csFontNameTag;
    FX_FLOAT fFontSize;
    FieldAppearance.GetFont(csFontNameTag, fFontSize);

    vp << (int)fFontSize;
  }

  return TRUE;
}

void Field::SetTextSize(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        int number) {
  // Not supported.
}

FX_BOOL Field::type(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  switch (pFormField->GetFieldType()) {
    case FIELDTYPE_UNKNOWN:
      vp << L"unknown";
      break;
    case FIELDTYPE_PUSHBUTTON:
      vp << L"button";
      break;
    case FIELDTYPE_CHECKBOX:
      vp << L"checkbox";
      break;
    case FIELDTYPE_RADIOBUTTON:
      vp << L"radiobutton";
      break;
    case FIELDTYPE_COMBOBOX:
      vp << L"combobox";
      break;
    case FIELDTYPE_LISTBOX:
      vp << L"listbox";
      break;
    case FIELDTYPE_TEXTFIELD:
      vp << L"text";
      break;
    case FIELDTYPE_SIGNATURE:
      vp << L"signature";
      break;
    default:
      vp << L"unknown";
      break;
  }

  return TRUE;
}

FX_BOOL Field::userName(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    CFX_WideString swName;
    vp >> swName;

    if (m_bDelay) {
      AddDelay_WideString(FP_USERNAME, swName);
    } else {
      Field::SetUserName(m_pDocument, m_FieldName, m_nFormControlIndex, swName);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    vp << (CFX_WideString)pFormField->GetAlternateName();
  }

  return TRUE;
}

void Field::SetUserName(CPDFSDK_Document* pDocument,
                        const CFX_WideString& swFieldName,
                        int nControlIndex,
                        const CFX_WideString& string) {
  // Not supported.
}

FX_BOOL Field::value(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);

  if (vp.IsSetting()) {
    if (!m_bCanSet)
      return FALSE;

    std::vector<CFX_WideString> strArray;
    if (vp.IsArrayObject()) {
      CJS_Array ValueArray(pRuntime);
      vp.ConvertToArray(ValueArray);
      for (int i = 0, sz = ValueArray.GetLength(); i < sz; i++) {
        CJS_Value ElementValue(pRuntime);
        ValueArray.GetElement(i, ElementValue);
        strArray.push_back(ElementValue.ToCFXWideString());
      }
    } else {
      CFX_WideString swValue;
      vp >> swValue;
      strArray.push_back(swValue);
    }

    if (m_bDelay) {
      AddDelay_WideStringArray(FP_VALUE, strArray);
    } else {
      Field::SetValue(m_pDocument, m_FieldName, m_nFormControlIndex, strArray);
    }
  } else {
    std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
    if (FieldArray.empty())
      return FALSE;

    CPDF_FormField* pFormField = FieldArray[0];
    switch (pFormField->GetFieldType()) {
      case FIELDTYPE_PUSHBUTTON:
        return FALSE;
      case FIELDTYPE_COMBOBOX:
      case FIELDTYPE_TEXTFIELD: {
        vp << pFormField->GetValue();
      } break;
      case FIELDTYPE_LISTBOX: {
        if (pFormField->CountSelectedItems() > 1) {
          CJS_Array ValueArray(pRuntime);
          CJS_Value ElementValue(pRuntime);
          int iIndex;
          for (int i = 0, sz = pFormField->CountSelectedItems(); i < sz; i++) {
            iIndex = pFormField->GetSelectedIndex(i);
            ElementValue = pFormField->GetOptionValue(iIndex).c_str();
            if (FXSYS_wcslen(ElementValue.ToCFXWideString().c_str()) == 0)
              ElementValue = pFormField->GetOptionLabel(iIndex).c_str();
            ValueArray.SetElement(i, ElementValue);
          }
          vp << ValueArray;
        } else {
          vp << pFormField->GetValue();
        }
      } break;
      case FIELDTYPE_CHECKBOX:
      case FIELDTYPE_RADIOBUTTON: {
        bool bFind = false;
        for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
          if (pFormField->GetControl(i)->IsChecked()) {
            vp << pFormField->GetControl(i)->GetExportValue();
            bFind = true;
            break;
          }
        }
        if (!bFind)
          vp << L"Off";
      } break;
      default:
        vp << pFormField->GetValue();
        break;
    }
  }
  vp.MaybeCoerceToNumber();
  return TRUE;
}

void Field::SetValue(CPDFSDK_Document* pDocument,
                     const CFX_WideString& swFieldName,
                     int nControlIndex,
                     const std::vector<CFX_WideString>& strArray) {
  ASSERT(pDocument);
  if (strArray.empty())
    return;

  std::vector<CPDF_FormField*> FieldArray =
      GetFormFields(pDocument, swFieldName);

  for (CPDF_FormField* pFormField : FieldArray) {
    if (pFormField->GetFullName().Compare(swFieldName) != 0)
      continue;

    switch (pFormField->GetFieldType()) {
      case FIELDTYPE_TEXTFIELD:
      case FIELDTYPE_COMBOBOX:
        if (pFormField->GetValue() != strArray[0]) {
          pFormField->SetValue(strArray[0], TRUE);
          UpdateFormField(pDocument, pFormField, TRUE, FALSE, TRUE);
        }
        break;
      case FIELDTYPE_CHECKBOX:
      case FIELDTYPE_RADIOBUTTON: {
        if (pFormField->GetValue() != strArray[0]) {
          pFormField->SetValue(strArray[0], TRUE);
          UpdateFormField(pDocument, pFormField, TRUE, FALSE, TRUE);
        }
      } break;
      case FIELDTYPE_LISTBOX: {
        FX_BOOL bModified = FALSE;
        for (const auto& str : strArray) {
          if (!pFormField->IsItemSelected(pFormField->FindOption(str))) {
            bModified = TRUE;
            break;
          }
        }
        if (bModified) {
          pFormField->ClearSelection(TRUE);
          for (const auto& str : strArray) {
            int index = pFormField->FindOption(str);
            if (!pFormField->IsItemSelected(index))
              pFormField->SetItemSelection(index, TRUE, TRUE);
          }
          UpdateFormField(pDocument, pFormField, TRUE, FALSE, TRUE);
        }
      } break;
      default:
        break;
    }
  }
}

FX_BOOL Field::valueAsString(IJS_Context* cc,
                             CJS_PropValue& vp,
                             CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() == FIELDTYPE_PUSHBUTTON)
    return FALSE;

  if (pFormField->GetFieldType() == FIELDTYPE_CHECKBOX) {
    if (!pFormField->CountControls())
      return FALSE;

    if (pFormField->GetControl(0)->IsChecked())
      vp << L"Yes";
    else
      vp << L"Off";
  } else if (pFormField->GetFieldType() == FIELDTYPE_RADIOBUTTON &&
             !(pFormField->GetFieldFlags() & FIELDFLAG_RADIOSINUNISON)) {
    for (int i = 0, sz = pFormField->CountControls(); i < sz; i++) {
      if (pFormField->GetControl(i)->IsChecked()) {
        vp << pFormField->GetControl(i)->GetExportValue().c_str();
        break;
      } else {
        vp << L"Off";
      }
    }
  } else if (pFormField->GetFieldType() == FIELDTYPE_LISTBOX &&
             (pFormField->CountSelectedItems() > 1)) {
    vp << L"";
  } else {
    vp << pFormField->GetValue().c_str();
  }

  return TRUE;
}

FX_BOOL Field::browseForFileToSubmit(IJS_Context* cc,
                                     const std::vector<CJS_Value>& params,
                                     CJS_Value& vRet,
                                     CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  CPDFDoc_Environment* pApp = m_pDocument->GetEnv();
  if ((pFormField->GetFieldFlags() & FIELDFLAG_FILESELECT) &&
      (pFormField->GetFieldType() == FIELDTYPE_TEXTFIELD)) {
    CFX_WideString wsFileName = pApp->JS_fieldBrowse();
    if (!wsFileName.IsEmpty()) {
      pFormField->SetValue(wsFileName);
      UpdateFormField(m_pDocument, pFormField, TRUE, TRUE, TRUE);
    }
    return TRUE;
  }
  return FALSE;
}

FX_BOOL Field::buttonGetCaption(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError) {
  int nface = 0;
  int iSize = params.size();
  if (iSize >= 1)
    nface = params[0].ToInt();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
    return FALSE;

  CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
  if (!pFormControl)
    return FALSE;

  if (nface == 0)
    vRet = pFormControl->GetNormalCaption().c_str();
  else if (nface == 1)
    vRet = pFormControl->GetDownCaption().c_str();
  else if (nface == 2)
    vRet = pFormControl->GetRolloverCaption().c_str();
  else
    return FALSE;

  return TRUE;
}

FX_BOOL Field::buttonGetIcon(IJS_Context* cc,
                             const std::vector<CJS_Value>& params,
                             CJS_Value& vRet,
                             CFX_WideString& sError) {
  int nface = 0;
  int iSize = params.size();
  if (iSize >= 1)
    nface = params[0].ToInt();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_PUSHBUTTON)
    return FALSE;

  CPDF_FormControl* pFormControl = GetSmartFieldControl(pFormField);
  if (!pFormControl)
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  v8::Local<v8::Object> pObj = FXJS_NewFxDynamicObj(
      pRuntime->GetIsolate(), pRuntime, CJS_Icon::g_nObjDefnID);
  ASSERT(pObj.IsEmpty() == FALSE);

  CJS_Icon* pJS_Icon = (CJS_Icon*)FXJS_GetPrivate(pRuntime->GetIsolate(), pObj);
  Icon* pIcon = (Icon*)pJS_Icon->GetEmbedObject();

  CPDF_Stream* pIconStream = nullptr;
  if (nface == 0)
    pIconStream = pFormControl->GetNormalIcon();
  else if (nface == 1)
    pIconStream = pFormControl->GetDownIcon();
  else if (nface == 2)
    pIconStream = pFormControl->GetRolloverIcon();
  else
    return FALSE;

  pIcon->SetStream(pIconStream);
  vRet = pJS_Icon;

  return TRUE;
}

FX_BOOL Field::buttonImportIcon(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::buttonSetCaption(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::buttonSetIcon(IJS_Context* cc,
                             const std::vector<CJS_Value>& params,
                             CJS_Value& vRet,
                             CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::checkThisBox(IJS_Context* cc,
                            const std::vector<CJS_Value>& params,
                            CJS_Value& vRet,
                            CFX_WideString& sError) {
  ASSERT(m_pDocument);

  if (!m_bCanSet)
    return FALSE;

  int iSize = params.size();
  if (iSize < 1)
    return FALSE;

  int nWidget = params[0].ToInt();

  bool bCheckit = true;
  if (iSize >= 2)
    bCheckit = params[1].ToBool();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (pFormField->GetFieldType() != FIELDTYPE_CHECKBOX &&
      pFormField->GetFieldType() != FIELDTYPE_RADIOBUTTON)
    return FALSE;
  if (nWidget < 0 || nWidget >= pFormField->CountControls())
    return FALSE;
  // TODO(weili): Check whether anything special needed for radio button,
  // otherwise merge these branches.
  if (pFormField->GetFieldType() == FIELDTYPE_RADIOBUTTON)
    pFormField->CheckControl(nWidget, bCheckit, true);
  else
    pFormField->CheckControl(nWidget, bCheckit, true);

  UpdateFormField(m_pDocument, pFormField, TRUE, TRUE, TRUE);
  return TRUE;
}

FX_BOOL Field::clearItems(IJS_Context* cc,
                          const std::vector<CJS_Value>& params,
                          CJS_Value& vRet,
                          CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::defaultIsChecked(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError) {
  if (!m_bCanSet)
    return FALSE;

  int iSize = params.size();
  if (iSize < 1)
    return FALSE;

  int nWidget = params[0].ToInt();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (nWidget < 0 || nWidget >= pFormField->CountControls()) {
    vRet = FALSE;
    return FALSE;
  }
  vRet = pFormField->GetFieldType() == FIELDTYPE_CHECKBOX ||
         pFormField->GetFieldType() == FIELDTYPE_RADIOBUTTON;

  return TRUE;
}

FX_BOOL Field::deleteItemAt(IJS_Context* cc,
                            const std::vector<CJS_Value>& params,
                            CJS_Value& vRet,
                            CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::getArray(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  std::vector<std::unique_ptr<CFX_WideString>> swSort;
  for (CPDF_FormField* pFormField : FieldArray) {
    swSort.push_back(std::unique_ptr<CFX_WideString>(
        new CFX_WideString(pFormField->GetFullName())));
  }

  std::sort(
      swSort.begin(), swSort.end(),
      [](const std::unique_ptr<CFX_WideString>& p1,
         const std::unique_ptr<CFX_WideString>& p2) { return *p1 < *p2; });

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_Runtime* pRuntime = pContext->GetJSRuntime();
  CJS_Array FormFieldArray(pRuntime);

  int j = 0;
  for (const auto& pStr : swSort) {
    v8::Local<v8::Object> pObj = FXJS_NewFxDynamicObj(
        pRuntime->GetIsolate(), pRuntime, CJS_Field::g_nObjDefnID);
    ASSERT(!pObj.IsEmpty());

    CJS_Field* pJSField =
        static_cast<CJS_Field*>(FXJS_GetPrivate(pRuntime->GetIsolate(), pObj));
    Field* pField = static_cast<Field*>(pJSField->GetEmbedObject());
    pField->AttachField(m_pJSDoc, *pStr);

    CJS_Value FormFieldValue(pRuntime);
    FormFieldValue = pJSField;
    FormFieldArray.SetElement(j++, FormFieldValue);
  }

  vRet = FormFieldArray;
  return TRUE;
}

FX_BOOL Field::getItemAt(IJS_Context* cc,
                         const std::vector<CJS_Value>& params,
                         CJS_Value& vRet,
                         CFX_WideString& sError) {
  int iSize = params.size();

  int nIdx = -1;
  if (iSize >= 1)
    nIdx = params[0].ToInt();

  FX_BOOL bExport = TRUE;
  if (iSize >= 2)
    bExport = params[1].ToBool();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if ((pFormField->GetFieldType() == FIELDTYPE_LISTBOX) ||
      (pFormField->GetFieldType() == FIELDTYPE_COMBOBOX)) {
    if (nIdx == -1 || nIdx > pFormField->CountOptions())
      nIdx = pFormField->CountOptions() - 1;
    if (bExport) {
      CFX_WideString strval = pFormField->GetOptionValue(nIdx);
      if (strval.IsEmpty())
        vRet = pFormField->GetOptionLabel(nIdx).c_str();
      else
        vRet = strval.c_str();
    } else {
      vRet = pFormField->GetOptionLabel(nIdx).c_str();
    }
  } else {
    return FALSE;
  }

  return TRUE;
}

FX_BOOL Field::getLock(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::insertItemAt(IJS_Context* cc,
                            const std::vector<CJS_Value>& params,
                            CJS_Value& vRet,
                            CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::isBoxChecked(IJS_Context* cc,
                            const std::vector<CJS_Value>& params,
                            CJS_Value& vRet,
                            CFX_WideString& sError) {
  int nIndex = -1;
  if (params.size() >= 1)
    nIndex = params[0].ToInt();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (nIndex < 0 || nIndex >= pFormField->CountControls()) {
    vRet = FALSE;
    return FALSE;
  }

  if ((pFormField->GetFieldType() == FIELDTYPE_CHECKBOX) ||
      (pFormField->GetFieldType() == FIELDTYPE_RADIOBUTTON)) {
    if (pFormField->GetControl(nIndex)->IsChecked() != 0)
      vRet = TRUE;
    else
      vRet = FALSE;
  } else {
    vRet = FALSE;
  }

  return TRUE;
}

FX_BOOL Field::isDefaultChecked(IJS_Context* cc,
                                const std::vector<CJS_Value>& params,
                                CJS_Value& vRet,
                                CFX_WideString& sError) {
  int nIndex = -1;
  if (params.size() >= 1)
    nIndex = params[0].ToInt();

  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  if (nIndex < 0 || nIndex >= pFormField->CountControls()) {
    vRet = FALSE;
    return FALSE;
  }
  if ((pFormField->GetFieldType() == FIELDTYPE_CHECKBOX) ||
      (pFormField->GetFieldType() == FIELDTYPE_RADIOBUTTON)) {
    if (pFormField->GetControl(nIndex)->IsDefaultChecked() != 0)
      vRet = TRUE;
    else
      vRet = FALSE;
  } else {
    vRet = FALSE;
  }

  return TRUE;
}

FX_BOOL Field::setAction(IJS_Context* cc,
                         const std::vector<CJS_Value>& params,
                         CJS_Value& vRet,
                         CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::setFocus(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError) {
  std::vector<CPDF_FormField*> FieldArray = GetFormFields(m_FieldName);
  if (FieldArray.empty())
    return FALSE;

  CPDF_FormField* pFormField = FieldArray[0];
  int32_t nCount = pFormField->CountControls();
  if (nCount < 1)
    return FALSE;

  CPDFSDK_InterForm* pInterForm = m_pDocument->GetInterForm();
  CPDFSDK_Widget* pWidget = nullptr;
  if (nCount == 1) {
    pWidget = pInterForm->GetWidget(pFormField->GetControl(0));
  } else {
    CPDFDoc_Environment* pEnv = m_pDocument->GetEnv();
    UnderlyingPageType* pPage = UnderlyingFromFPDFPage(
        pEnv->FFI_GetCurrentPage(m_pDocument->GetUnderlyingDocument()));
    if (!pPage)
      return FALSE;
    if (CPDFSDK_PageView* pCurPageView = m_pDocument->GetPageView(pPage)) {
      for (int32_t i = 0; i < nCount; i++) {
        if (CPDFSDK_Widget* pTempWidget =
                pInterForm->GetWidget(pFormField->GetControl(i))) {
          if (pTempWidget->GetPDFPage() == pCurPageView->GetPDFPage()) {
            pWidget = pTempWidget;
            break;
          }
        }
      }
    }
  }

  if (pWidget) {
    m_pDocument->SetFocusAnnot(pWidget);
  }

  return TRUE;
}

FX_BOOL Field::setItems(IJS_Context* cc,
                        const std::vector<CJS_Value>& params,
                        CJS_Value& vRet,
                        CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL Field::setLock(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureGetModifications(IJS_Context* cc,
                                         const std::vector<CJS_Value>& params,
                                         CJS_Value& vRet,
                                         CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureGetSeedValue(IJS_Context* cc,
                                     const std::vector<CJS_Value>& params,
                                     CJS_Value& vRet,
                                     CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureInfo(IJS_Context* cc,
                             const std::vector<CJS_Value>& params,
                             CJS_Value& vRet,
                             CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureSetSeedValue(IJS_Context* cc,
                                     const std::vector<CJS_Value>& params,
                                     CJS_Value& vRet,
                                     CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureSign(IJS_Context* cc,
                             const std::vector<CJS_Value>& params,
                             CJS_Value& vRet,
                             CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::signatureValidate(IJS_Context* cc,
                                 const std::vector<CJS_Value>& params,
                                 CJS_Value& vRet,
                                 CFX_WideString& sError) {
  return FALSE;
}

FX_BOOL Field::source(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (vp.IsGetting()) {
    vp << (CJS_Object*)nullptr;
  }

  return TRUE;
}

void Field::AddDelay_Int(FIELD_PROP prop, int32_t n) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->num = n;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_Bool(FIELD_PROP prop, bool b) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->b = b;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_String(FIELD_PROP prop, const CFX_ByteString& string) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->string = string;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_WideString(FIELD_PROP prop, const CFX_WideString& string) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->widestring = string;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_Rect(FIELD_PROP prop, const CFX_FloatRect& rect) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->rect = rect;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_Color(FIELD_PROP prop, const CPWL_Color& color) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->color = color;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_WordArray(FIELD_PROP prop,
                               const std::vector<uint32_t>& array) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->wordarray = array;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::AddDelay_WideStringArray(FIELD_PROP prop,
                                     const std::vector<CFX_WideString>& array) {
  CJS_DelayData* pNewData =
      new CJS_DelayData(prop, m_nFormControlIndex, m_FieldName);
  pNewData->widestringarray = array;
  m_pJSDoc->AddDelayData(pNewData);
}

void Field::DoDelay(CPDFSDK_Document* pDocument, CJS_DelayData* pData) {
  ASSERT(pDocument);
  switch (pData->eProp) {
    case FP_ALIGNMENT:
      Field::SetAlignment(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->string);
      break;
    case FP_BORDERSTYLE:
      Field::SetBorderStyle(pDocument, pData->sFieldName, pData->nControlIndex,
                            pData->string);
      break;
    case FP_BUTTONALIGNX:
      Field::SetButtonAlignX(pDocument, pData->sFieldName, pData->nControlIndex,
                             pData->num);
      break;
    case FP_BUTTONALIGNY:
      Field::SetButtonAlignY(pDocument, pData->sFieldName, pData->nControlIndex,
                             pData->num);
      break;
    case FP_BUTTONFITBOUNDS:
      Field::SetButtonFitBounds(pDocument, pData->sFieldName,
                                pData->nControlIndex, pData->b);
      break;
    case FP_BUTTONPOSITION:
      Field::SetButtonPosition(pDocument, pData->sFieldName,
                               pData->nControlIndex, pData->num);
      break;
    case FP_BUTTONSCALEHOW:
      Field::SetButtonScaleHow(pDocument, pData->sFieldName,
                               pData->nControlIndex, pData->num);
      break;
    case FP_BUTTONSCALEWHEN:
      Field::SetButtonScaleWhen(pDocument, pData->sFieldName,
                                pData->nControlIndex, pData->num);
      break;
    case FP_CALCORDERINDEX:
      Field::SetCalcOrderIndex(pDocument, pData->sFieldName,
                               pData->nControlIndex, pData->num);
      break;
    case FP_CHARLIMIT:
      Field::SetCharLimit(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->num);
      break;
    case FP_COMB:
      Field::SetComb(pDocument, pData->sFieldName, pData->nControlIndex,
                     pData->b);
      break;
    case FP_COMMITONSELCHANGE:
      Field::SetCommitOnSelChange(pDocument, pData->sFieldName,
                                  pData->nControlIndex, pData->b);
      break;
    case FP_CURRENTVALUEINDICES:
      Field::SetCurrentValueIndices(pDocument, pData->sFieldName,
                                    pData->nControlIndex, pData->wordarray);
      break;
    case FP_DEFAULTVALUE:
      Field::SetDefaultValue(pDocument, pData->sFieldName, pData->nControlIndex,
                             pData->widestring);
      break;
    case FP_DONOTSCROLL:
      Field::SetDoNotScroll(pDocument, pData->sFieldName, pData->nControlIndex,
                            pData->b);
      break;
    case FP_DISPLAY:
      Field::SetDisplay(pDocument, pData->sFieldName, pData->nControlIndex,
                        pData->num);
      break;
    case FP_FILLCOLOR:
      Field::SetFillColor(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->color);
      break;
    case FP_HIDDEN:
      Field::SetHidden(pDocument, pData->sFieldName, pData->nControlIndex,
                       pData->b);
      break;
    case FP_HIGHLIGHT:
      Field::SetHighlight(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->string);
      break;
    case FP_LINEWIDTH:
      Field::SetLineWidth(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->num);
      break;
    case FP_MULTILINE:
      Field::SetMultiline(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->b);
      break;
    case FP_MULTIPLESELECTION:
      Field::SetMultipleSelection(pDocument, pData->sFieldName,
                                  pData->nControlIndex, pData->b);
      break;
    case FP_PASSWORD:
      Field::SetPassword(pDocument, pData->sFieldName, pData->nControlIndex,
                         pData->b);
      break;
    case FP_RECT:
      Field::SetRect(pDocument, pData->sFieldName, pData->nControlIndex,
                     pData->rect);
      break;
    case FP_RICHTEXT:
      // Not supported.
      break;
    case FP_RICHVALUE:
      break;
    case FP_ROTATION:
      Field::SetRotation(pDocument, pData->sFieldName, pData->nControlIndex,
                         pData->num);
      break;
    case FP_STROKECOLOR:
      Field::SetStrokeColor(pDocument, pData->sFieldName, pData->nControlIndex,
                            pData->color);
      break;
    case FP_STYLE:
      Field::SetStyle(pDocument, pData->sFieldName, pData->nControlIndex,
                      pData->string);
      break;
    case FP_TEXTCOLOR:
      Field::SetTextColor(pDocument, pData->sFieldName, pData->nControlIndex,
                          pData->color);
      break;
    case FP_TEXTFONT:
      Field::SetTextFont(pDocument, pData->sFieldName, pData->nControlIndex,
                         pData->string);
      break;
    case FP_TEXTSIZE:
      Field::SetTextSize(pDocument, pData->sFieldName, pData->nControlIndex,
                         pData->num);
      break;
    case FP_USERNAME:
      Field::SetUserName(pDocument, pData->sFieldName, pData->nControlIndex,
                         pData->widestring);
      break;
    case FP_VALUE:
      Field::SetValue(pDocument, pData->sFieldName, pData->nControlIndex,
                      pData->widestringarray);
      break;
  }
}

void Field::AddField(CPDFSDK_Document* pDocument,
                     int nPageIndex,
                     int nFieldType,
                     const CFX_WideString& sName,
                     const CFX_FloatRect& rcCoords) {
  // Not supported.
}
