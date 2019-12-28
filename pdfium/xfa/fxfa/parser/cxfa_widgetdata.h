// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_WIDGETDATA_H_
#define XFA_FXFA_PARSER_CXFA_WIDGETDATA_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_assist.h"
#include "xfa/fxfa/parser/cxfa_bind.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_calculate.h"
#include "xfa/fxfa/parser/cxfa_caption.h"
#include "xfa/fxfa/parser/cxfa_data.h"
#include "xfa/fxfa/parser/cxfa_font.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_para.h"
#include "xfa/fxfa/parser/cxfa_validate.h"
#include "xfa/fxfa/parser/xfa_object.h"

enum XFA_CHECKSTATE {
  XFA_CHECKSTATE_On = 0,
  XFA_CHECKSTATE_Off = 1,
  XFA_CHECKSTATE_Neutral = 2,
};

enum XFA_VALUEPICTURE {
  XFA_VALUEPICTURE_Raw = 0,
  XFA_VALUEPICTURE_Display,
  XFA_VALUEPICTURE_Edit,
  XFA_VALUEPICTURE_DataBind,
};

class CXFA_Node;
class IFX_Locale;

class CXFA_WidgetData : public CXFA_Data {
 public:
  explicit CXFA_WidgetData(CXFA_Node* pNode);

  CXFA_Node* GetUIChild();
  XFA_Element GetUIType();
  CFX_WideString GetRawValue();
  int32_t GetAccess(FX_BOOL bTemplate = FALSE);
  int32_t GetRotate();
  CXFA_Border GetBorder(FX_BOOL bModified = FALSE);
  CXFA_Caption GetCaption(FX_BOOL bModified = FALSE);
  CXFA_Font GetFont(FX_BOOL bModified = FALSE);
  CXFA_Margin GetMargin(FX_BOOL bModified = FALSE);
  CXFA_Para GetPara(FX_BOOL bModified = FALSE);
  void GetEventList(CXFA_NodeArray& events);
  int32_t GetEventByActivity(int32_t iActivity,
                             CXFA_NodeArray& events,
                             FX_BOOL bIsFormReady = FALSE);
  CXFA_Value GetDefaultValue(FX_BOOL bModified = FALSE);
  CXFA_Value GetFormValue(FX_BOOL bModified = FALSE);
  CXFA_Calculate GetCalculate(FX_BOOL bModified = FALSE);
  CXFA_Validate GetValidate(FX_BOOL bModified = FALSE);
  CXFA_Bind GetBind(FX_BOOL bModified = FALSE);
  CXFA_Assist GetAssist(FX_BOOL bModified = FALSE);
  FX_BOOL GetWidth(FX_FLOAT& fWidth);
  FX_BOOL GetHeight(FX_FLOAT& fHeight);
  FX_BOOL GetMinWidth(FX_FLOAT& fMinWidth);
  FX_BOOL GetMinHeight(FX_FLOAT& fMinHeight);
  FX_BOOL GetMaxWidth(FX_FLOAT& fMaxWidth);
  FX_BOOL GetMaxHeight(FX_FLOAT& fMaxHeight);
  CXFA_Border GetUIBorder(FX_BOOL bModified = FALSE);
  CXFA_Margin GetUIMargin(FX_BOOL bModified = FALSE);
  void GetUIMargin(CFX_RectF& rtUIMargin);
  int32_t GetButtonHighlight();
  FX_BOOL GetButtonRollover(CFX_WideString& wsRollover, FX_BOOL& bRichText);
  FX_BOOL GetButtonDown(CFX_WideString& wsDown, FX_BOOL& bRichText);
  int32_t GetCheckButtonShape();
  int32_t GetCheckButtonMark();
  FX_FLOAT GetCheckButtonSize();
  FX_BOOL IsAllowNeutral();
  FX_BOOL IsRadioButton();
  XFA_CHECKSTATE GetCheckState();
  void SetCheckState(XFA_CHECKSTATE eCheckState, bool bNotify);
  CXFA_Node* GetExclGroupNode();
  CXFA_Node* GetSelectedMember();
  CXFA_Node* SetSelectedMember(const CFX_WideStringC& wsName, bool bNotify);
  void SetSelectedMemberByValue(const CFX_WideStringC& wsValue,
                                bool bNotify,
                                FX_BOOL bScriptModify,
                                FX_BOOL bSyncData);
  CXFA_Node* GetExclGroupFirstMember();
  CXFA_Node* GetExclGroupNextMember(CXFA_Node* pNode);
  int32_t GetChoiceListCommitOn();
  FX_BOOL IsChoiceListAllowTextEntry();
  int32_t GetChoiceListOpen();
  FX_BOOL IsListBox();
  int32_t CountChoiceListItems(FX_BOOL bSaveValue = FALSE);
  FX_BOOL GetChoiceListItem(CFX_WideString& wsText,
                            int32_t nIndex,
                            FX_BOOL bSaveValue = FALSE);
  void GetChoiceListItems(CFX_WideStringArray& wsTextArray,
                          FX_BOOL bSaveValue = FALSE);
  int32_t CountSelectedItems();
  int32_t GetSelectedItem(int32_t nIndex = 0);
  void GetSelectedItems(CFX_Int32Array& iSelArray);
  void GetSelectedItemsValue(CFX_WideStringArray& wsSelTextArray);
  FX_BOOL GetItemState(int32_t nIndex);
  void SetItemState(int32_t nIndex,
                    FX_BOOL bSelected,
                    bool bNotify,
                    FX_BOOL bScriptModify,
                    FX_BOOL bSyncData);
  void SetSelectedItems(CFX_Int32Array& iSelArray,
                        bool bNotify,
                        FX_BOOL bScriptModify,
                        FX_BOOL bSyncData);
  void ClearAllSelections();
  void InsertItem(const CFX_WideString& wsLabel,
                  const CFX_WideString& wsValue,
                  int32_t nIndex = -1,
                  FX_BOOL bNotify = FALSE);
  void GetItemLabel(const CFX_WideStringC& wsValue, CFX_WideString& wsLabel);
  void GetItemValue(const CFX_WideStringC& wsLabel, CFX_WideString& wsValue);
  FX_BOOL DeleteItem(int32_t nIndex,
                     FX_BOOL bNotify = FALSE,
                     FX_BOOL bScriptModify = FALSE,
                     FX_BOOL bSyncData = TRUE);
  int32_t GetHorizontalScrollPolicy();
  int32_t GetNumberOfCells();
  FX_BOOL SetValue(const CFX_WideString& wsValue, XFA_VALUEPICTURE eValueType);
  FX_BOOL GetPictureContent(CFX_WideString& wsPicture,
                            XFA_VALUEPICTURE ePicture);
  IFX_Locale* GetLocal();
  FX_BOOL GetValue(CFX_WideString& wsValue, XFA_VALUEPICTURE eValueType);
  FX_BOOL GetNormalizeDataValue(const CFX_WideString& wsValue,
                                CFX_WideString& wsNormalizeValue);
  FX_BOOL GetFormatDataValue(const CFX_WideString& wsValue,
                             CFX_WideString& wsFormatedValue);
  void NormalizeNumStr(const CFX_WideString& wsValue, CFX_WideString& wsOutput);
  CFX_WideString GetBarcodeType();
  FX_BOOL GetBarcodeAttribute_CharEncoding(int32_t& val);
  FX_BOOL GetBarcodeAttribute_Checksum(int32_t& val);
  FX_BOOL GetBarcodeAttribute_DataLength(int32_t& val);
  FX_BOOL GetBarcodeAttribute_StartChar(FX_CHAR& val);
  FX_BOOL GetBarcodeAttribute_EndChar(FX_CHAR& val);
  FX_BOOL GetBarcodeAttribute_ECLevel(int32_t& val);
  FX_BOOL GetBarcodeAttribute_ModuleWidth(int32_t& val);
  FX_BOOL GetBarcodeAttribute_ModuleHeight(int32_t& val);
  FX_BOOL GetBarcodeAttribute_PrintChecksum(FX_BOOL& val);
  FX_BOOL GetBarcodeAttribute_TextLocation(int32_t& val);
  FX_BOOL GetBarcodeAttribute_Truncate(FX_BOOL& val);
  FX_BOOL GetBarcodeAttribute_WideNarrowRatio(FX_FLOAT& val);
  void GetPasswordChar(CFX_WideString& wsPassWord);
  FX_BOOL IsMultiLine();
  int32_t GetVerticalScrollPolicy();
  int32_t GetMaxChars(XFA_Element& eType);
  FX_BOOL GetFracDigits(int32_t& iFracDigits);
  FX_BOOL GetLeadDigits(int32_t& iLeadDigits);

  CFX_WideString NumericLimit(const CFX_WideString& wsValue,
                              int32_t iLead,
                              int32_t iTread) const;

  FX_BOOL m_bIsNull;
  FX_BOOL m_bPreNull;

 protected:
  void SyncValue(const CFX_WideString& wsValue, bool bNotify);
  void InsertListTextItem(CXFA_Node* pItems,
                          const CFX_WideString& wsText,
                          int32_t nIndex = -1);
  void FormatNumStr(const CFX_WideString& wsValue,
                    IFX_Locale* pLocale,
                    CFX_WideString& wsOutput);

  CXFA_Node* m_pUiChildNode;
  XFA_Element m_eUIType;
};

#endif  // XFA_FXFA_PARSER_CXFA_WIDGETDATA_H_
