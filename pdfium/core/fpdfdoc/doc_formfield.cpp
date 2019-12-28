// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cfdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfdoc/cpvt_generateap.h"
#include "core/fpdfdoc/doc_utils.h"
#include "core/fpdfdoc/include/fpdf_doc.h"

namespace {

bool PDF_FormField_IsUnison(CPDF_FormField* pField) {
  if (pField->GetType() == CPDF_FormField::CheckBox)
    return true;

  return (pField->GetFieldFlags() & 0x2000000) != 0;
}

}  // namespace

CPDF_FormField::CPDF_FormField(CPDF_InterForm* pForm, CPDF_Dictionary* pDict)
    : m_Type(Unknown),
      m_pForm(pForm),
      m_pDict(pDict),
      m_FontSize(0),
      m_pFont(nullptr) {
  SyncFieldFlags();
}

CPDF_FormField::~CPDF_FormField() {}

void CPDF_FormField::SyncFieldFlags() {
  CFX_ByteString type_name = FPDF_GetFieldAttr(m_pDict, "FT")
                                 ? FPDF_GetFieldAttr(m_pDict, "FT")->GetString()
                                 : CFX_ByteString();
  uint32_t flags = FPDF_GetFieldAttr(m_pDict, "Ff")
                       ? FPDF_GetFieldAttr(m_pDict, "Ff")->GetInteger()
                       : 0;
  m_Flags = 0;
  if (flags & 1) {
    m_Flags |= FORMFIELD_READONLY;
  }
  if (flags & 2) {
    m_Flags |= FORMFIELD_REQUIRED;
  }
  if (flags & 4) {
    m_Flags |= FORMFIELD_NOEXPORT;
  }
  if (type_name == "Btn") {
    if (flags & 0x8000) {
      m_Type = RadioButton;
      if (flags & 0x4000) {
        m_Flags |= FORMRADIO_NOTOGGLEOFF;
      }
      if (flags & 0x2000000) {
        m_Flags |= FORMRADIO_UNISON;
      }
    } else if (flags & 0x10000) {
      m_Type = PushButton;
    } else {
      m_Type = CheckBox;
    }
  } else if (type_name == "Tx") {
    if (flags & 0x100000) {
      m_Type = File;
    } else if (flags & 0x2000000) {
      m_Type = RichText;
    } else {
      m_Type = Text;
      if (flags & 0x1000) {
        m_Flags |= FORMTEXT_MULTILINE;
      }
      if (flags & 0x2000) {
        m_Flags |= FORMTEXT_PASSWORD;
      }
      if (flags & 0x800000) {
        m_Flags |= FORMTEXT_NOSCROLL;
      }
      if (flags & 0x100000) {
        m_Flags |= FORMTEXT_COMB;
      }
    }
    LoadDA();
  } else if (type_name == "Ch") {
    if (flags & 0x20000) {
      m_Type = ComboBox;
      if (flags & 0x40000) {
        m_Flags |= FORMCOMBO_EDIT;
      }
    } else {
      m_Type = ListBox;
      if (flags & 0x200000) {
        m_Flags |= FORMLIST_MULTISELECT;
      }
    }
    LoadDA();
  } else if (type_name == "Sig") {
    m_Type = Sign;
  }
}
CFX_WideString CPDF_FormField::GetFullName() const {
  return ::GetFullName(m_pDict);
}

FX_BOOL CPDF_FormField::ResetField(FX_BOOL bNotify) {
  switch (m_Type) {
    case CPDF_FormField::CheckBox:
    case CPDF_FormField::RadioButton: {
      int iCount = CountControls();
      if (iCount) {
        // TODO(weili): Check whether anything special needs to be done for
        // unison field. Otherwise, merge these branches.
        if (PDF_FormField_IsUnison(this)) {
          for (int i = 0; i < iCount; i++) {
            CheckControl(i, GetControl(i)->IsDefaultChecked(), FALSE);
          }
        } else {
          for (int i = 0; i < iCount; i++) {
            CheckControl(i, GetControl(i)->IsDefaultChecked(), FALSE);
          }
        }
      }
      if (bNotify && m_pForm->m_pFormNotify) {
        m_pForm->m_pFormNotify->AfterCheckedStatusChange(this);
      }
    } break;
    case CPDF_FormField::ComboBox:
    case CPDF_FormField::ListBox: {
      CFX_WideString csValue;
      ClearSelection();
      int iIndex = GetDefaultSelectedItem();
      if (iIndex >= 0)
        csValue = GetOptionLabel(iIndex);

      if (bNotify && !NotifyListOrComboBoxBeforeChange(csValue))
        return FALSE;

      SetItemSelection(iIndex, TRUE);
      if (bNotify)
        NotifyListOrComboBoxAfterChange();
    } break;
    case CPDF_FormField::Text:
    case CPDF_FormField::RichText:
    case CPDF_FormField::File:
    default: {
      CPDF_Object* pDV = FPDF_GetFieldAttr(m_pDict, "DV");
      CFX_WideString csDValue;
      if (pDV)
        csDValue = pDV->GetUnicodeText();

      CPDF_Object* pV = FPDF_GetFieldAttr(m_pDict, "V");
      CFX_WideString csValue;
      if (pV)
        csValue = pV->GetUnicodeText();

      CPDF_Object* pRV = FPDF_GetFieldAttr(m_pDict, "RV");
      if (!pRV && (csDValue == csValue))
        return FALSE;

      if (bNotify && !NotifyBeforeValueChange(csDValue))
        return FALSE;

      if (pDV) {
        CPDF_Object* pClone = pDV->Clone();
        if (!pClone)
          return FALSE;

        m_pDict->SetAt("V", pClone);
        if (pRV) {
          CPDF_Object* pCloneR = pDV->Clone();
          m_pDict->SetAt("RV", pCloneR);
        }
      } else {
        m_pDict->RemoveAt("V");
        m_pDict->RemoveAt("RV");
      }
      if (bNotify)
        NotifyAfterValueChange();
    } break;
  }
  return TRUE;
}

int CPDF_FormField::GetControlIndex(const CPDF_FormControl* pControl) const {
  if (!pControl)
    return -1;

  for (int i = 0; i < m_ControlList.GetSize(); i++) {
    if (m_ControlList.GetAt(i) == pControl)
      return i;
  }
  return -1;
}

int CPDF_FormField::GetFieldType() const {
  switch (m_Type) {
    case PushButton:
      return FIELDTYPE_PUSHBUTTON;
    case CheckBox:
      return FIELDTYPE_CHECKBOX;
    case RadioButton:
      return FIELDTYPE_RADIOBUTTON;
    case ComboBox:
      return FIELDTYPE_COMBOBOX;
    case ListBox:
      return FIELDTYPE_LISTBOX;
    case Text:
    case RichText:
    case File:
      return FIELDTYPE_TEXTFIELD;
    case Sign:
      return FIELDTYPE_SIGNATURE;
    default:
      break;
  }
  return FIELDTYPE_UNKNOWN;
}

CPDF_AAction CPDF_FormField::GetAdditionalAction() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "AA");
  return CPDF_AAction(pObj ? pObj->GetDict() : nullptr);
}

CFX_WideString CPDF_FormField::GetAlternateName() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "TU");
  if (!pObj) {
    return L"";
  }
  return pObj->GetUnicodeText();
}
CFX_WideString CPDF_FormField::GetMappingName() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "TM");
  if (!pObj) {
    return L"";
  }
  return pObj->GetUnicodeText();
}
uint32_t CPDF_FormField::GetFieldFlags() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "Ff");
  if (!pObj) {
    return 0;
  }
  return pObj->GetInteger();
}
CFX_ByteString CPDF_FormField::GetDefaultStyle() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "DS");
  if (!pObj) {
    return "";
  }
  return pObj->GetString();
}
CFX_WideString CPDF_FormField::GetRichTextString() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "RV");
  if (!pObj) {
    return L"";
  }
  return pObj->GetUnicodeText();
}
CFX_WideString CPDF_FormField::GetValue(FX_BOOL bDefault) const {
  if (GetType() == CheckBox || GetType() == RadioButton)
    return GetCheckValue(bDefault);

  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, bDefault ? "DV" : "V");
  if (!pValue) {
    if (!bDefault) {
      if (m_Type == RichText) {
        pValue = FPDF_GetFieldAttr(m_pDict, "V");
      }
      if (!pValue && m_Type != Text) {
        pValue = FPDF_GetFieldAttr(m_pDict, "DV");
      }
    }
    if (!pValue)
      return CFX_WideString();
  }
  switch (pValue->GetType()) {
    case CPDF_Object::STRING:
    case CPDF_Object::STREAM:
      return pValue->GetUnicodeText();
    case CPDF_Object::ARRAY:
      pValue = pValue->AsArray()->GetDirectObjectAt(0);
      if (pValue)
        return pValue->GetUnicodeText();
      break;
    default:
      break;
  }
  return CFX_WideString();
}

CFX_WideString CPDF_FormField::GetValue() const {
  return GetValue(FALSE);
}

CFX_WideString CPDF_FormField::GetDefaultValue() const {
  return GetValue(TRUE);
}

FX_BOOL CPDF_FormField::SetValue(const CFX_WideString& value,
                                 FX_BOOL bDefault,
                                 FX_BOOL bNotify) {
  switch (m_Type) {
    case CheckBox:
    case RadioButton: {
      SetCheckValue(value, bDefault, bNotify);
      return TRUE;
    }
    case File:
    case RichText:
    case Text:
    case ComboBox: {
      CFX_WideString csValue = value;
      if (bNotify && !NotifyBeforeValueChange(csValue))
        return FALSE;

      int iIndex = FindOptionValue(csValue);
      if (iIndex < 0) {
        CFX_ByteString bsEncodeText = PDF_EncodeText(csValue);
        m_pDict->SetAtString(bDefault ? "DV" : "V", bsEncodeText);
        if (m_Type == RichText && !bDefault)
          m_pDict->SetAtString("RV", bsEncodeText);
        m_pDict->RemoveAt("I");
      } else {
        m_pDict->SetAtString(bDefault ? "DV" : "V", PDF_EncodeText(csValue));
        if (!bDefault) {
          ClearSelection();
          SetItemSelection(iIndex, TRUE);
        }
      }
      if (bNotify)
        NotifyAfterValueChange();
    } break;
    case ListBox: {
      int iIndex = FindOptionValue(value);
      if (iIndex < 0)
        return FALSE;

      if (bDefault && iIndex == GetDefaultSelectedItem())
        return FALSE;

      if (bNotify && !NotifyBeforeSelectionChange(value))
        return FALSE;

      if (!bDefault) {
        ClearSelection();
        SetItemSelection(iIndex, TRUE);
      }
      if (bNotify)
        NotifyAfterSelectionChange();
      break;
    }
    default:
      break;
  }
  return TRUE;
}

FX_BOOL CPDF_FormField::SetValue(const CFX_WideString& value, FX_BOOL bNotify) {
  return SetValue(value, FALSE, bNotify);
}

int CPDF_FormField::GetMaxLen() const {
  if (CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "MaxLen"))
    return pObj->GetInteger();

  for (int i = 0; i < m_ControlList.GetSize(); i++) {
    CPDF_FormControl* pControl = m_ControlList.GetAt(i);
    if (!pControl)
      continue;

    CPDF_Dictionary* pWidgetDict = pControl->m_pWidgetDict;
    if (pWidgetDict->KeyExist("MaxLen"))
      return pWidgetDict->GetIntegerBy("MaxLen");
  }
  return 0;
}

int CPDF_FormField::CountSelectedItems() const {
  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "V");
  if (!pValue) {
    pValue = FPDF_GetFieldAttr(m_pDict, "I");
    if (!pValue)
      return 0;
  }

  if (pValue->IsString() || pValue->IsNumber())
    return pValue->GetString().IsEmpty() ? 0 : 1;
  if (CPDF_Array* pArray = pValue->AsArray())
    return pArray->GetCount();
  return 0;
}

int CPDF_FormField::GetSelectedIndex(int index) const {
  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "V");
  if (!pValue) {
    pValue = FPDF_GetFieldAttr(m_pDict, "I");
    if (!pValue)
      return -1;
  }
  if (pValue->IsNumber())
    return pValue->GetInteger();

  CFX_WideString sel_value;
  if (pValue->IsString()) {
    if (index != 0)
      return -1;
    sel_value = pValue->GetUnicodeText();
  } else {
    CPDF_Array* pArray = pValue->AsArray();
    if (!pArray || index < 0)
      return -1;

    CPDF_Object* elementValue = pArray->GetDirectObjectAt(index);
    sel_value =
        elementValue ? elementValue->GetUnicodeText() : CFX_WideString();
  }
  if (index < CountSelectedOptions()) {
    int iOptIndex = GetSelectedOptionIndex(index);
    CFX_WideString csOpt = GetOptionValue(iOptIndex);
    if (csOpt == sel_value) {
      return iOptIndex;
    }
  }
  for (int i = 0; i < CountOptions(); i++) {
    if (sel_value == GetOptionValue(i))
      return i;
  }
  return -1;
}

FX_BOOL CPDF_FormField::ClearSelection(FX_BOOL bNotify) {
  if (bNotify && m_pForm->m_pFormNotify) {
    CFX_WideString csValue;
    int iIndex = GetSelectedIndex(0);
    if (iIndex >= 0)
      csValue = GetOptionLabel(iIndex);

    if (!NotifyListOrComboBoxBeforeChange(csValue))
      return FALSE;
  }
  m_pDict->RemoveAt("V");
  m_pDict->RemoveAt("I");
  if (bNotify)
    NotifyListOrComboBoxAfterChange();
  return TRUE;
}

FX_BOOL CPDF_FormField::IsItemSelected(int index) const {
  ASSERT(GetType() == ComboBox || GetType() == ListBox);
  if (index < 0 || index >= CountOptions()) {
    return FALSE;
  }
  if (IsOptionSelected(index)) {
    return TRUE;
  }
  CFX_WideString opt_value = GetOptionValue(index);
  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "V");
  if (!pValue) {
    pValue = FPDF_GetFieldAttr(m_pDict, "I");
    if (!pValue) {
      return FALSE;
    }
  }

  if (pValue->IsString())
    return pValue->GetUnicodeText() == opt_value;

  if (pValue->IsNumber()) {
    if (pValue->GetString().IsEmpty())
      return FALSE;
    return (pValue->GetInteger() == index);
  }

  CPDF_Array* pArray = pValue->AsArray();
  if (!pArray)
    return FALSE;

  int iPos = -1;
  for (int j = 0; j < CountSelectedOptions(); j++) {
    if (GetSelectedOptionIndex(j) == index) {
      iPos = j;
      break;
    }
  }
  for (int i = 0; i < static_cast<int>(pArray->GetCount()); i++)
    if (pArray->GetDirectObjectAt(i)->GetUnicodeText() == opt_value &&
        i == iPos) {
      return TRUE;
    }
  return FALSE;
}

FX_BOOL CPDF_FormField::SetItemSelection(int index,
                                         FX_BOOL bSelected,
                                         FX_BOOL bNotify) {
  ASSERT(GetType() == ComboBox || GetType() == ListBox);
  if (index < 0 || index >= CountOptions())
    return FALSE;

  CFX_WideString opt_value = GetOptionValue(index);
  if (bNotify && !NotifyListOrComboBoxBeforeChange(opt_value))
    return FALSE;

  if (bSelected) {
    if (GetType() == ListBox) {
      SelectOption(index, TRUE);
      if (!(m_Flags & FORMLIST_MULTISELECT)) {
        m_pDict->SetAtString("V", PDF_EncodeText(opt_value));
      } else {
        CPDF_Array* pArray = new CPDF_Array;
        for (int i = 0; i < CountOptions(); i++) {
          if (i == index || IsItemSelected(i)) {
            opt_value = GetOptionValue(i);
            pArray->AddString(PDF_EncodeText(opt_value));
          }
        }
        m_pDict->SetAt("V", pArray);
      }
    } else {
      m_pDict->SetAtString("V", PDF_EncodeText(opt_value));
      CPDF_Array* pI = new CPDF_Array;
      pI->AddInteger(index);
      m_pDict->SetAt("I", pI);
    }
  } else {
    CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "V");
    if (pValue) {
      if (GetType() == ListBox) {
        SelectOption(index, FALSE);
        if (pValue->IsString()) {
          if (pValue->GetUnicodeText() == opt_value)
            m_pDict->RemoveAt("V");
        } else if (pValue->IsArray()) {
          CPDF_Array* pArray = new CPDF_Array;
          for (int i = 0; i < CountOptions(); i++) {
            if (i != index && IsItemSelected(i)) {
              opt_value = GetOptionValue(i);
              pArray->AddString(PDF_EncodeText(opt_value));
            }
          }
          if (pArray->GetCount() < 1)
            pArray->Release();
          else
            m_pDict->SetAt("V", pArray);
        }
      } else {
        m_pDict->RemoveAt("V");
        m_pDict->RemoveAt("I");
      }
    }
  }
  if (bNotify)
    NotifyListOrComboBoxAfterChange();
  return TRUE;
}

FX_BOOL CPDF_FormField::IsItemDefaultSelected(int index) const {
  ASSERT(GetType() == ComboBox || GetType() == ListBox);
  if (index < 0 || index >= CountOptions())
    return FALSE;
  int iDVIndex = GetDefaultSelectedItem();
  return iDVIndex >= 0 && iDVIndex == index;
}

int CPDF_FormField::GetDefaultSelectedItem() const {
  ASSERT(GetType() == ComboBox || GetType() == ListBox);
  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "DV");
  if (!pValue)
    return -1;
  CFX_WideString csDV = pValue->GetUnicodeText();
  if (csDV.IsEmpty())
    return -1;
  for (int i = 0; i < CountOptions(); i++) {
    if (csDV == GetOptionValue(i))
      return i;
  }
  return -1;
}

int CPDF_FormField::CountOptions() const {
  CPDF_Array* pArray = ToArray(FPDF_GetFieldAttr(m_pDict, "Opt"));
  return pArray ? pArray->GetCount() : 0;
}

CFX_WideString CPDF_FormField::GetOptionText(int index, int sub_index) const {
  CPDF_Array* pArray = ToArray(FPDF_GetFieldAttr(m_pDict, "Opt"));
  if (!pArray)
    return CFX_WideString();

  CPDF_Object* pOption = pArray->GetDirectObjectAt(index);
  if (!pOption)
    return CFX_WideString();
  if (CPDF_Array* pOptionArray = pOption->AsArray())
    pOption = pOptionArray->GetDirectObjectAt(sub_index);

  CPDF_String* pString = ToString(pOption);
  return pString ? pString->GetUnicodeText() : CFX_WideString();
}
CFX_WideString CPDF_FormField::GetOptionLabel(int index) const {
  return GetOptionText(index, 1);
}
CFX_WideString CPDF_FormField::GetOptionValue(int index) const {
  return GetOptionText(index, 0);
}

int CPDF_FormField::FindOption(CFX_WideString csOptLabel) const {
  for (int i = 0; i < CountOptions(); i++) {
    if (GetOptionValue(i) == csOptLabel)
      return i;
  }
  return -1;
}

int CPDF_FormField::FindOptionValue(const CFX_WideString& csOptValue) const {
  for (int i = 0; i < CountOptions(); i++) {
    if (GetOptionValue(i) == csOptValue)
      return i;
  }
  return -1;
}

#ifdef PDF_ENABLE_XFA
int CPDF_FormField::InsertOption(CFX_WideString csOptLabel,
                                 int index,
                                 FX_BOOL bNotify) {
  if (csOptLabel.IsEmpty())
    return -1;

  if (bNotify && !NotifyListOrComboBoxBeforeChange(csOptLabel))
    return -1;

  CFX_ByteString csStr =
      PDF_EncodeText(csOptLabel.c_str(), csOptLabel.GetLength());
  CPDF_Object* pValue = FPDF_GetFieldAttr(m_pDict, "Opt");
  CPDF_Array* pOpt = ToArray(pValue);
  if (!pOpt) {
    pOpt = new CPDF_Array;
    m_pDict->SetAt("Opt", pOpt);
  }

  int iCount = (int)pOpt->GetCount();
  if (index < 0 || index >= iCount) {
    pOpt->AddString(csStr);
    index = iCount;
  } else {
    CPDF_String* pString = new CPDF_String(csStr, FALSE);
    pOpt->InsertAt(index, pString);
  }

  if (bNotify)
    NotifyListOrComboBoxAfterChange();
  return index;
}

FX_BOOL CPDF_FormField::ClearOptions(FX_BOOL bNotify) {
  if (bNotify && m_pForm->m_pFormNotify) {
    CFX_WideString csValue;
    int iIndex = GetSelectedIndex(0);
    if (iIndex >= 0)
      csValue = GetOptionLabel(iIndex);
    if (!NotifyListOrComboBoxBeforeChange(csValue))
      return FALSE;
  }

  m_pDict->RemoveAt("Opt");
  m_pDict->RemoveAt("V");
  m_pDict->RemoveAt("DV");
  m_pDict->RemoveAt("I");
  m_pDict->RemoveAt("TI");

  if (bNotify)
    NotifyListOrComboBoxAfterChange();

  return TRUE;
}
#endif  // PDF_ENABLE_XFA

FX_BOOL CPDF_FormField::CheckControl(int iControlIndex,
                                     bool bChecked,
                                     bool bNotify) {
  ASSERT(GetType() == CheckBox || GetType() == RadioButton);
  CPDF_FormControl* pControl = GetControl(iControlIndex);
  if (!pControl) {
    return FALSE;
  }
  if (!bChecked && pControl->IsChecked() == bChecked) {
    return FALSE;
  }
  CFX_WideString csWExport = pControl->GetExportValue();
  CFX_ByteString csBExport = PDF_EncodeText(csWExport);
  int iCount = CountControls();
  bool bUnison = PDF_FormField_IsUnison(this);
  for (int i = 0; i < iCount; i++) {
    CPDF_FormControl* pCtrl = GetControl(i);
    if (bUnison) {
      CFX_WideString csEValue = pCtrl->GetExportValue();
      if (csEValue == csWExport) {
        if (pCtrl->GetOnStateName() == pControl->GetOnStateName()) {
          pCtrl->CheckControl(bChecked);
        } else if (bChecked) {
          pCtrl->CheckControl(FALSE);
        }
      } else if (bChecked) {
        pCtrl->CheckControl(FALSE);
      }
    } else {
      if (i == iControlIndex) {
        pCtrl->CheckControl(bChecked);
      } else if (bChecked) {
        pCtrl->CheckControl(FALSE);
      }
    }
  }
  CPDF_Object* pOpt = FPDF_GetFieldAttr(m_pDict, "Opt");
  if (!ToArray(pOpt)) {
    if (bChecked) {
      m_pDict->SetAtName("V", csBExport);
    } else {
      CFX_ByteString csV;
      CPDF_Object* pV = FPDF_GetFieldAttr(m_pDict, "V");
      if (pV) {
        csV = pV->GetString();
      }
      if (csV == csBExport) {
        m_pDict->SetAtName("V", "Off");
      }
    }
  } else if (bChecked) {
    CFX_ByteString csIndex;
    csIndex.Format("%d", iControlIndex);
    m_pDict->SetAtName("V", csIndex);
  }
  if (bNotify && m_pForm->m_pFormNotify)
    m_pForm->m_pFormNotify->AfterCheckedStatusChange(this);
  return TRUE;
}

CFX_WideString CPDF_FormField::GetCheckValue(FX_BOOL bDefault) const {
  ASSERT(GetType() == CheckBox || GetType() == RadioButton);
  CFX_WideString csExport = L"Off";
  int iCount = CountControls();
  for (int i = 0; i < iCount; i++) {
    CPDF_FormControl* pControl = GetControl(i);
    FX_BOOL bChecked =
        bDefault ? pControl->IsDefaultChecked() : pControl->IsChecked();
    if (bChecked) {
      csExport = pControl->GetExportValue();
      break;
    }
  }
  return csExport;
}

FX_BOOL CPDF_FormField::SetCheckValue(const CFX_WideString& value,
                                      FX_BOOL bDefault,
                                      FX_BOOL bNotify) {
  ASSERT(GetType() == CheckBox || GetType() == RadioButton);
  int iCount = CountControls();
  for (int i = 0; i < iCount; i++) {
    CPDF_FormControl* pControl = GetControl(i);
    CFX_WideString csExport = pControl->GetExportValue();
    bool val = csExport == value;
    if (!bDefault)
      CheckControl(GetControlIndex(pControl), val);
    if (val)
      break;
  }
  if (bNotify && m_pForm->m_pFormNotify)
    m_pForm->m_pFormNotify->AfterCheckedStatusChange(this);
  return TRUE;
}

int CPDF_FormField::GetTopVisibleIndex() const {
  CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "TI");
  return pObj ? pObj->GetInteger() : 0;
}

int CPDF_FormField::CountSelectedOptions() const {
  CPDF_Array* pArray = ToArray(FPDF_GetFieldAttr(m_pDict, "I"));
  return pArray ? pArray->GetCount() : 0;
}

int CPDF_FormField::GetSelectedOptionIndex(int index) const {
  CPDF_Array* pArray = ToArray(FPDF_GetFieldAttr(m_pDict, "I"));
  if (!pArray)
    return -1;

  int iCount = pArray->GetCount();
  if (iCount < 0 || index >= iCount)
    return -1;
  return pArray->GetIntegerAt(index);
}

FX_BOOL CPDF_FormField::IsOptionSelected(int iOptIndex) const {
  CPDF_Array* pArray = ToArray(FPDF_GetFieldAttr(m_pDict, "I"));
  if (!pArray)
    return FALSE;

  for (CPDF_Object* pObj : *pArray) {
    if (pObj->GetInteger() == iOptIndex)
      return TRUE;
  }
  return FALSE;
}

FX_BOOL CPDF_FormField::SelectOption(int iOptIndex,
                                     FX_BOOL bSelected,
                                     FX_BOOL bNotify) {
  CPDF_Array* pArray = m_pDict->GetArrayBy("I");
  if (!pArray) {
    if (!bSelected)
      return TRUE;

    pArray = new CPDF_Array;
    m_pDict->SetAt("I", pArray);
  }

  FX_BOOL bReturn = FALSE;
  for (size_t i = 0; i < pArray->GetCount(); i++) {
    int iFind = pArray->GetIntegerAt(i);
    if (iFind == iOptIndex) {
      if (bSelected)
        return TRUE;

      if (bNotify && m_pForm->m_pFormNotify) {
        CFX_WideString csValue = GetOptionLabel(iOptIndex);
        if (!NotifyListOrComboBoxBeforeChange(csValue))
          return FALSE;
      }
      pArray->RemoveAt(i);
      bReturn = TRUE;
      break;
    }

    if (iFind > iOptIndex) {
      if (!bSelected)
        continue;

      if (bNotify && m_pForm->m_pFormNotify) {
        CFX_WideString csValue = GetOptionLabel(iOptIndex);
        if (!NotifyListOrComboBoxBeforeChange(csValue))
          return FALSE;
      }
      pArray->InsertAt(i, new CPDF_Number(iOptIndex));
      bReturn = TRUE;
      break;
    }
  }
  if (!bReturn) {
    if (bSelected)
      pArray->AddInteger(iOptIndex);

    if (pArray->GetCount() == 0)
      m_pDict->RemoveAt("I");
  }
  if (bNotify)
    NotifyListOrComboBoxAfterChange();

  return TRUE;
}

FX_BOOL CPDF_FormField::ClearSelectedOptions(FX_BOOL bNotify) {
  if (bNotify && m_pForm->m_pFormNotify) {
    CFX_WideString csValue;
    int iIndex = GetSelectedIndex(0);
    if (iIndex >= 0)
      csValue = GetOptionLabel(iIndex);

    if (!NotifyListOrComboBoxBeforeChange(csValue))
      return FALSE;
  }
  m_pDict->RemoveAt("I");
  if (bNotify)
    NotifyListOrComboBoxAfterChange();

  return TRUE;
}

void CPDF_FormField::LoadDA() {
  CPDF_Dictionary* pFormDict = m_pForm->m_pFormDict;
  if (!pFormDict)
    return;

  CFX_ByteString DA;
  if (CPDF_Object* pObj = FPDF_GetFieldAttr(m_pDict, "DA"))
    DA = pObj->GetString();

  if (DA.IsEmpty())
    DA = pFormDict->GetStringBy("DA");

  if (DA.IsEmpty())
    return;

  CPDF_Dictionary* pDR = pFormDict->GetDictBy("DR");
  if (!pDR)
    return;

  CPDF_Dictionary* pFont = pDR->GetDictBy("Font");
  if (!pFont)
    return;

  CPDF_SimpleParser syntax(DA.AsStringC());
  syntax.FindTagParamFromStart("Tf", 2);
  CFX_ByteString font_name(syntax.GetWord());
  CPDF_Dictionary* pFontDict = pFont->GetDictBy(font_name);
  if (!pFontDict)
    return;

  m_pFont = m_pForm->m_pDocument->LoadFont(pFontDict);
  m_FontSize = FX_atof(syntax.GetWord());
}

bool CPDF_FormField::NotifyBeforeSelectionChange(const CFX_WideString& value) {
  if (!m_pForm->m_pFormNotify)
    return true;
  return m_pForm->m_pFormNotify->BeforeSelectionChange(this, value) >= 0;
}

void CPDF_FormField::NotifyAfterSelectionChange() {
  if (!m_pForm->m_pFormNotify)
    return;
  m_pForm->m_pFormNotify->AfterSelectionChange(this);
}

bool CPDF_FormField::NotifyBeforeValueChange(const CFX_WideString& value) {
  if (!m_pForm->m_pFormNotify)
    return true;
  return m_pForm->m_pFormNotify->BeforeValueChange(this, value) >= 0;
}

void CPDF_FormField::NotifyAfterValueChange() {
  if (!m_pForm->m_pFormNotify)
    return;
  m_pForm->m_pFormNotify->AfterValueChange(this);
}

bool CPDF_FormField::NotifyListOrComboBoxBeforeChange(
    const CFX_WideString& value) {
  switch (GetType()) {
    case ListBox:
      return NotifyBeforeSelectionChange(value);
    case ComboBox:
      return NotifyBeforeValueChange(value);
    default:
      return true;
  }
}

void CPDF_FormField::NotifyListOrComboBoxAfterChange() {
  switch (GetType()) {
    case ListBox:
      NotifyAfterSelectionChange();
      break;
    case ComboBox:
      NotifyAfterValueChange();
      break;
    default:
      break;
  }
}
