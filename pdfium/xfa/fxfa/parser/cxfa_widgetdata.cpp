// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_widgetdata.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fxbarcode/include/BC_Library.h"
#include "xfa/fxfa/app/xfa_ffnotify.h"
#include "xfa/fxfa/parser/cxfa_event.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localevalue.h"
#include "xfa/fxfa/parser/xfa_object.h"

namespace {

FX_FLOAT GetEdgeThickness(const CXFA_StrokeArray& strokes,
                          FX_BOOL b3DStyle,
                          int32_t nIndex) {
  FX_FLOAT fThickness = 0;

  if (strokes[nIndex * 2 + 1].GetPresence() == XFA_ATTRIBUTEENUM_Visible) {
    if (nIndex == 0)
      fThickness += 2.5f;

    fThickness += strokes[nIndex * 2 + 1].GetThickness() * (b3DStyle ? 4 : 2);
  }
  return fThickness;
}

FX_BOOL SplitDateTime(const CFX_WideString& wsDateTime,
                      CFX_WideString& wsDate,
                      CFX_WideString& wsTime) {
  wsDate = L"";
  wsTime = L"";
  if (wsDateTime.IsEmpty())
    return FALSE;

  int nSplitIndex = -1;
  nSplitIndex = wsDateTime.Find('T');
  if (nSplitIndex < 0)
    nSplitIndex = wsDateTime.Find(' ');
  if (nSplitIndex < 0)
    return FALSE;

  wsDate = wsDateTime.Left(nSplitIndex);
  if (!wsDate.IsEmpty()) {
    int32_t iCount = wsDate.GetLength();
    int32_t i = 0;
    for (i = 0; i < iCount; i++) {
      if (wsDate[i] >= '0' && wsDate[i] <= '9')
        break;
    }
    if (i == iCount)
      return FALSE;
  }
  wsTime = wsDateTime.Right(wsDateTime.GetLength() - nSplitIndex - 1);
  if (!wsTime.IsEmpty()) {
    int32_t iCount = wsTime.GetLength();
    int32_t i = 0;
    for (i = 0; i < iCount; i++) {
      if (wsTime[i] >= '0' && wsTime[i] <= '9')
        break;
    }
    if (i == iCount)
      return FALSE;
  }
  return TRUE;
}

CXFA_Node* CreateUIChild(CXFA_Node* pNode, XFA_Element& eWidgetType) {
  XFA_Element eType = pNode->GetElementType();
  eWidgetType = eType;
  if (eType != XFA_Element::Field && eType != XFA_Element::Draw)
    return nullptr;

  eWidgetType = XFA_Element::Unknown;
  XFA_Element eUIType = XFA_Element::Unknown;
  CXFA_Value defValue(pNode->GetProperty(0, XFA_Element::Value, TRUE));
  XFA_Element eValueType = defValue.GetChildValueClassID();
  switch (eValueType) {
    case XFA_Element::Boolean:
      eUIType = XFA_Element::CheckButton;
      break;
    case XFA_Element::Integer:
    case XFA_Element::Decimal:
    case XFA_Element::Float:
      eUIType = XFA_Element::NumericEdit;
      break;
    case XFA_Element::ExData:
    case XFA_Element::Text:
      eUIType = XFA_Element::TextEdit;
      eWidgetType = XFA_Element::Text;
      break;
    case XFA_Element::Date:
    case XFA_Element::Time:
    case XFA_Element::DateTime:
      eUIType = XFA_Element::DateTimeEdit;
      break;
    case XFA_Element::Image:
      eUIType = XFA_Element::ImageEdit;
      eWidgetType = XFA_Element::Image;
      break;
    case XFA_Element::Arc:
    case XFA_Element::Line:
    case XFA_Element::Rectangle:
      eUIType = XFA_Element::DefaultUi;
      eWidgetType = eValueType;
      break;
    default:
      break;
  }

  CXFA_Node* pUIChild = nullptr;
  CXFA_Node* pUI = pNode->GetProperty(0, XFA_Element::Ui, TRUE);
  CXFA_Node* pChild = pUI->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pChild; pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    XFA_Element eChildType = pChild->GetElementType();
    if (eChildType == XFA_Element::Extras ||
        eChildType == XFA_Element::Picture) {
      continue;
    }
    const XFA_PROPERTY* pProperty = XFA_GetPropertyOfElement(
        XFA_Element::Ui, eChildType, XFA_XDPPACKET_Form);
    if (pProperty && (pProperty->uFlags & XFA_PROPERTYFLAG_OneOf)) {
      pUIChild = pChild;
      break;
    }
  }

  if (eType == XFA_Element::Draw) {
    XFA_Element eDraw =
        pUIChild ? pUIChild->GetElementType() : XFA_Element::Unknown;
    switch (eDraw) {
      case XFA_Element::TextEdit:
        eWidgetType = XFA_Element::Text;
        break;
      case XFA_Element::ImageEdit:
        eWidgetType = XFA_Element::Image;
        break;
      default:
        eWidgetType = eWidgetType == XFA_Element::Unknown ? XFA_Element::Text
                                                          : eWidgetType;
        break;
    }
  } else {
    if (pUIChild && pUIChild->GetElementType() == XFA_Element::DefaultUi) {
      eWidgetType = XFA_Element::TextEdit;
    } else {
      eWidgetType =
          pUIChild ? pUIChild->GetElementType()
                   : (eUIType == XFA_Element::Unknown ? XFA_Element::TextEdit
                                                      : eUIType);
    }
  }

  if (!pUIChild) {
    if (eUIType == XFA_Element::Unknown) {
      eUIType = XFA_Element::TextEdit;
      defValue.GetNode()->GetProperty(0, XFA_Element::Text, TRUE);
    }
    return pUI->GetProperty(0, eUIType, TRUE);
  }

  if (eUIType != XFA_Element::Unknown)
    return pUIChild;

  switch (pUIChild->GetElementType()) {
    case XFA_Element::CheckButton: {
      eValueType = XFA_Element::Text;
      if (CXFA_Node* pItems = pNode->GetChild(0, XFA_Element::Items)) {
        if (CXFA_Node* pItem = pItems->GetChild(0, XFA_Element::Unknown))
          eValueType = pItem->GetElementType();
      }
      break;
    }
    case XFA_Element::DateTimeEdit:
      eValueType = XFA_Element::DateTime;
      break;
    case XFA_Element::ImageEdit:
      eValueType = XFA_Element::Image;
      break;
    case XFA_Element::NumericEdit:
      eValueType = XFA_Element::Float;
      break;
    case XFA_Element::ChoiceList: {
      eValueType = (pUIChild->GetEnum(XFA_ATTRIBUTE_Open) ==
                    XFA_ATTRIBUTEENUM_MultiSelect)
                       ? XFA_Element::ExData
                       : XFA_Element::Text;
      break;
    }
    case XFA_Element::Barcode:
    case XFA_Element::Button:
    case XFA_Element::PasswordEdit:
    case XFA_Element::Signature:
    case XFA_Element::TextEdit:
    default:
      eValueType = XFA_Element::Text;
      break;
  }
  defValue.GetNode()->GetProperty(0, eValueType, TRUE);

  return pUIChild;
}

}  // namespace

CXFA_WidgetData::CXFA_WidgetData(CXFA_Node* pNode)
    : CXFA_Data(pNode),
      m_bIsNull(TRUE),
      m_bPreNull(TRUE),
      m_pUiChildNode(nullptr),
      m_eUIType(XFA_Element::Unknown) {}

CXFA_Node* CXFA_WidgetData::GetUIChild() {
  if (m_eUIType == XFA_Element::Unknown)
    m_pUiChildNode = CreateUIChild(m_pNode, m_eUIType);

  return m_pUiChildNode;
}

XFA_Element CXFA_WidgetData::GetUIType() {
  GetUIChild();
  return m_eUIType;
}

CFX_WideString CXFA_WidgetData::GetRawValue() {
  return m_pNode->GetContent();
}

int32_t CXFA_WidgetData::GetAccess(FX_BOOL bTemplate) {
  if (bTemplate) {
    CXFA_Node* pNode = m_pNode->GetTemplateNode();
    if (pNode)
      return pNode->GetEnum(XFA_ATTRIBUTE_Access);
    return XFA_ATTRIBUTEENUM_Open;
  }
  CXFA_Node* pNode = m_pNode;
  while (pNode) {
    int32_t iAcc = pNode->GetEnum(XFA_ATTRIBUTE_Access);
    if (iAcc != XFA_ATTRIBUTEENUM_Open)
      return iAcc;

    pNode =
        pNode->GetNodeItem(XFA_NODEITEM_Parent, XFA_ObjectType::ContainerNode);
  }
  return XFA_ATTRIBUTEENUM_Open;
}

int32_t CXFA_WidgetData::GetRotate() {
  CXFA_Measurement ms;
  if (!m_pNode->TryMeasure(XFA_ATTRIBUTE_Rotate, ms, FALSE))
    return 0;

  int32_t iRotate = FXSYS_round(ms.GetValue());
  iRotate = XFA_MapRotation(iRotate);
  return iRotate / 90 * 90;
}

CXFA_Border CXFA_WidgetData::GetBorder(FX_BOOL bModified) {
  return CXFA_Border(m_pNode->GetProperty(0, XFA_Element::Border, bModified));
}

CXFA_Caption CXFA_WidgetData::GetCaption(FX_BOOL bModified) {
  return CXFA_Caption(m_pNode->GetProperty(0, XFA_Element::Caption, bModified));
}

CXFA_Font CXFA_WidgetData::GetFont(FX_BOOL bModified) {
  return CXFA_Font(m_pNode->GetProperty(0, XFA_Element::Font, bModified));
}

CXFA_Margin CXFA_WidgetData::GetMargin(FX_BOOL bModified) {
  return CXFA_Margin(m_pNode->GetProperty(0, XFA_Element::Margin, bModified));
}

CXFA_Para CXFA_WidgetData::GetPara(FX_BOOL bModified) {
  return CXFA_Para(m_pNode->GetProperty(0, XFA_Element::Para, bModified));
}

void CXFA_WidgetData::GetEventList(CXFA_NodeArray& events) {
  m_pNode->GetNodeList(events, 0, XFA_Element::Event);
}

int32_t CXFA_WidgetData::GetEventByActivity(int32_t iActivity,
                                            CXFA_NodeArray& events,
                                            FX_BOOL bIsFormReady) {
  CXFA_NodeArray allEvents;
  GetEventList(allEvents);
  int32_t iCount = allEvents.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CXFA_Event event(allEvents[i]);
    if (event.GetActivity() == iActivity) {
      if (iActivity == XFA_ATTRIBUTEENUM_Ready) {
        CFX_WideStringC wsRef;
        event.GetRef(wsRef);
        if (bIsFormReady) {
          if (wsRef == CFX_WideStringC(L"$form"))
            events.Add(allEvents[i]);
        } else {
          if (wsRef == CFX_WideStringC(L"$layout"))
            events.Add(allEvents[i]);
        }
      } else {
        events.Add(allEvents[i]);
      }
    }
  }
  return events.GetSize();
}

CXFA_Value CXFA_WidgetData::GetDefaultValue(FX_BOOL bModified) {
  CXFA_Node* pTemNode = m_pNode->GetTemplateNode();
  return CXFA_Value(
      pTemNode ? pTemNode->GetProperty(0, XFA_Element::Value, bModified)
               : nullptr);
}

CXFA_Value CXFA_WidgetData::GetFormValue(FX_BOOL bModified) {
  return CXFA_Value(m_pNode->GetProperty(0, XFA_Element::Value, bModified));
}

CXFA_Calculate CXFA_WidgetData::GetCalculate(FX_BOOL bModified) {
  return CXFA_Calculate(
      m_pNode->GetProperty(0, XFA_Element::Calculate, bModified));
}

CXFA_Validate CXFA_WidgetData::GetValidate(FX_BOOL bModified) {
  return CXFA_Validate(
      m_pNode->GetProperty(0, XFA_Element::Validate, bModified));
}

CXFA_Bind CXFA_WidgetData::GetBind(FX_BOOL bModified) {
  return CXFA_Bind(m_pNode->GetProperty(0, XFA_Element::Bind, bModified));
}

CXFA_Assist CXFA_WidgetData::GetAssist(FX_BOOL bModified) {
  return CXFA_Assist(m_pNode->GetProperty(0, XFA_Element::Assist, bModified));
}

FX_BOOL CXFA_WidgetData::GetWidth(FX_FLOAT& fWidth) {
  return TryMeasure(XFA_ATTRIBUTE_W, fWidth);
}

FX_BOOL CXFA_WidgetData::GetHeight(FX_FLOAT& fHeight) {
  return TryMeasure(XFA_ATTRIBUTE_H, fHeight);
}

FX_BOOL CXFA_WidgetData::GetMinWidth(FX_FLOAT& fMinWidth) {
  return TryMeasure(XFA_ATTRIBUTE_MinW, fMinWidth);
}

FX_BOOL CXFA_WidgetData::GetMinHeight(FX_FLOAT& fMinHeight) {
  return TryMeasure(XFA_ATTRIBUTE_MinH, fMinHeight);
}

FX_BOOL CXFA_WidgetData::GetMaxWidth(FX_FLOAT& fMaxWidth) {
  return TryMeasure(XFA_ATTRIBUTE_MaxW, fMaxWidth);
}

FX_BOOL CXFA_WidgetData::GetMaxHeight(FX_FLOAT& fMaxHeight) {
  return TryMeasure(XFA_ATTRIBUTE_MaxH, fMaxHeight);
}

CXFA_Border CXFA_WidgetData::GetUIBorder(FX_BOOL bModified) {
  CXFA_Node* pUIChild = GetUIChild();
  return CXFA_Border(
      pUIChild ? pUIChild->GetProperty(0, XFA_Element::Border, bModified)
               : nullptr);
}

CXFA_Margin CXFA_WidgetData::GetUIMargin(FX_BOOL bModified) {
  CXFA_Node* pUIChild = GetUIChild();
  return CXFA_Margin(
      pUIChild ? pUIChild->GetProperty(0, XFA_Element::Margin, bModified)
               : nullptr);
}

void CXFA_WidgetData::GetUIMargin(CFX_RectF& rtUIMargin) {
  rtUIMargin.Reset();
  CXFA_Margin mgUI = GetUIMargin();
  if (!mgUI)
    return;

  CXFA_Border border = GetUIBorder();
  if (border && border.GetPresence() != XFA_ATTRIBUTEENUM_Visible)
    return;

  FX_FLOAT fLeftInset, fTopInset, fRightInset, fBottomInset;
  FX_BOOL bLeft = mgUI.GetLeftInset(fLeftInset);
  FX_BOOL bTop = mgUI.GetTopInset(fTopInset);
  FX_BOOL bRight = mgUI.GetRightInset(fRightInset);
  FX_BOOL bBottom = mgUI.GetBottomInset(fBottomInset);
  if (border) {
    FX_BOOL bVisible = FALSE;
    FX_FLOAT fThickness = 0;
    border.Get3DStyle(bVisible, fThickness);
    if (!bLeft || !bTop || !bRight || !bBottom) {
      CXFA_StrokeArray strokes;
      border.GetStrokes(strokes);
      if (!bTop)
        fTopInset = GetEdgeThickness(strokes, bVisible, 0);
      if (!bRight)
        fRightInset = GetEdgeThickness(strokes, bVisible, 1);
      if (!bBottom)
        fBottomInset = GetEdgeThickness(strokes, bVisible, 2);
      if (!bLeft)
        fLeftInset = GetEdgeThickness(strokes, bVisible, 3);
    }
  }
  rtUIMargin.Set(fLeftInset, fTopInset, fRightInset, fBottomInset);
}

int32_t CXFA_WidgetData::GetButtonHighlight() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_Highlight);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::Button, XFA_ATTRIBUTE_Highlight, XFA_XDPPACKET_Form);
}

FX_BOOL CXFA_WidgetData::GetButtonRollover(CFX_WideString& wsRollover,
                                           FX_BOOL& bRichText) {
  if (CXFA_Node* pItems = m_pNode->GetChild(0, XFA_Element::Items)) {
    CXFA_Node* pText = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    while (pText) {
      CFX_WideStringC wsName;
      pText->TryCData(XFA_ATTRIBUTE_Name, wsName);
      if (wsName == FX_WSTRC(L"rollover")) {
        pText->TryContent(wsRollover);
        bRichText = pText->GetElementType() == XFA_Element::ExData;
        return !wsRollover.IsEmpty();
      }
      pText = pText->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetButtonDown(CFX_WideString& wsDown,
                                       FX_BOOL& bRichText) {
  if (CXFA_Node* pItems = m_pNode->GetChild(0, XFA_Element::Items)) {
    CXFA_Node* pText = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    while (pText) {
      CFX_WideStringC wsName;
      pText->TryCData(XFA_ATTRIBUTE_Name, wsName);
      if (wsName == FX_WSTRC(L"down")) {
        pText->TryContent(wsDown);
        bRichText = pText->GetElementType() == XFA_Element::ExData;
        return !wsDown.IsEmpty();
      }
      pText = pText->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
  }
  return FALSE;
}

int32_t CXFA_WidgetData::GetCheckButtonShape() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_Shape);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::CheckButton, XFA_ATTRIBUTE_Shape, XFA_XDPPACKET_Form);
}

int32_t CXFA_WidgetData::GetCheckButtonMark() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_Mark);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::CheckButton, XFA_ATTRIBUTE_Mark, XFA_XDPPACKET_Form);
}

FX_BOOL CXFA_WidgetData::IsRadioButton() {
  if (CXFA_Node* pParent = m_pNode->GetNodeItem(XFA_NODEITEM_Parent))
    return pParent->GetElementType() == XFA_Element::ExclGroup;
  return FALSE;
}

FX_FLOAT CXFA_WidgetData::GetCheckButtonSize() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetMeasure(XFA_ATTRIBUTE_Size).ToUnit(XFA_UNIT_Pt);
  return XFA_GetAttributeDefaultValue_Measure(
             XFA_Element::CheckButton, XFA_ATTRIBUTE_Size, XFA_XDPPACKET_Form)
      .ToUnit(XFA_UNIT_Pt);
}

FX_BOOL CXFA_WidgetData::IsAllowNeutral() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetBoolean(XFA_ATTRIBUTE_AllowNeutral);
  return XFA_GetAttributeDefaultValue_Boolean(
      XFA_Element::CheckButton, XFA_ATTRIBUTE_AllowNeutral, XFA_XDPPACKET_Form);
}

XFA_CHECKSTATE CXFA_WidgetData::GetCheckState() {
  CFX_WideString wsValue = GetRawValue();
  if (wsValue.IsEmpty())
    return XFA_CHECKSTATE_Off;

  if (CXFA_Node* pItems = m_pNode->GetChild(0, XFA_Element::Items)) {
    CXFA_Node* pText = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    int32_t i = 0;
    while (pText) {
      CFX_WideString wsContent;
      if (pText->TryContent(wsContent) && (wsContent == wsValue))
        return (XFA_CHECKSTATE)i;

      i++;
      pText = pText->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
  }
  return XFA_CHECKSTATE_Off;
}

void CXFA_WidgetData::SetCheckState(XFA_CHECKSTATE eCheckState, bool bNotify) {
  CXFA_WidgetData exclGroup(GetExclGroupNode());
  if (exclGroup) {
    CFX_WideString wsValue;
    if (eCheckState != XFA_CHECKSTATE_Off) {
      if (CXFA_Node* pItems = m_pNode->GetChild(0, XFA_Element::Items)) {
        CXFA_Node* pText = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
        if (pText)
          pText->TryContent(wsValue);
      }
    }
    CXFA_Node* pChild =
        exclGroup.GetNode()->GetNodeItem(XFA_NODEITEM_FirstChild);
    for (; pChild; pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      if (pChild->GetElementType() != XFA_Element::Field)
        continue;

      CXFA_Node* pItem = pChild->GetChild(0, XFA_Element::Items);
      if (!pItem)
        continue;

      CXFA_Node* pItemchild = pItem->GetNodeItem(XFA_NODEITEM_FirstChild);
      if (!pItemchild)
        continue;

      CFX_WideString text = pItemchild->GetContent();
      CFX_WideString wsChildValue = text;
      if (wsValue != text) {
        pItemchild = pItemchild->GetNodeItem(XFA_NODEITEM_NextSibling);
        if (pItemchild)
          wsChildValue = pItemchild->GetContent();
        else
          wsChildValue.clear();
      }
      CXFA_WidgetData ch(pChild);
      ch.SyncValue(wsChildValue, bNotify);
    }
    exclGroup.SyncValue(wsValue, bNotify);
  } else {
    CXFA_Node* pItems = m_pNode->GetChild(0, XFA_Element::Items);
    if (!pItems)
      return;

    int32_t i = -1;
    CXFA_Node* pText = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    CFX_WideString wsContent;
    while (pText) {
      i++;
      if (i == eCheckState) {
        pText->TryContent(wsContent);
        break;
      }
      pText = pText->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
    SyncValue(wsContent, bNotify);
  }
}

CXFA_Node* CXFA_WidgetData::GetExclGroupNode() {
  CXFA_Node* pExcl = ToNode(m_pNode->GetNodeItem(XFA_NODEITEM_Parent));
  if (!pExcl || pExcl->GetElementType() != XFA_Element::ExclGroup)
    return nullptr;
  return pExcl;
}

CXFA_Node* CXFA_WidgetData::GetSelectedMember() {
  CXFA_Node* pSelectedMember = nullptr;
  CFX_WideString wsState = GetRawValue();
  if (wsState.IsEmpty())
    return pSelectedMember;

  for (CXFA_Node* pNode = ToNode(m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild));
       pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    CXFA_WidgetData widgetData(pNode);
    if (widgetData.GetCheckState() == XFA_CHECKSTATE_On) {
      pSelectedMember = pNode;
      break;
    }
  }
  return pSelectedMember;
}

CXFA_Node* CXFA_WidgetData::SetSelectedMember(const CFX_WideStringC& wsName,
                                              bool bNotify) {
  uint32_t nameHash = FX_HashCode_GetW(wsName, false);
  for (CXFA_Node* pNode = ToNode(m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild));
       pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetNameHash() == nameHash) {
      CXFA_WidgetData widgetData(pNode);
      widgetData.SetCheckState(XFA_CHECKSTATE_On, bNotify);
      return pNode;
    }
  }
  return nullptr;
}

void CXFA_WidgetData::SetSelectedMemberByValue(const CFX_WideStringC& wsValue,
                                               bool bNotify,
                                               FX_BOOL bScriptModify,
                                               FX_BOOL bSyncData) {
  CFX_WideString wsExclGroup;
  for (CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild); pNode;
       pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() != XFA_Element::Field)
      continue;

    CXFA_Node* pItem = pNode->GetChild(0, XFA_Element::Items);
    if (!pItem)
      continue;

    CXFA_Node* pItemchild = pItem->GetNodeItem(XFA_NODEITEM_FirstChild);
    if (!pItemchild)
      continue;

    CFX_WideString wsChildValue = pItemchild->GetContent();
    if (wsValue != wsChildValue) {
      pItemchild = pItemchild->GetNodeItem(XFA_NODEITEM_NextSibling);
      if (pItemchild)
        wsChildValue = pItemchild->GetContent();
      else
        wsChildValue.clear();
    } else {
      wsExclGroup = wsValue;
    }
    pNode->SetContent(wsChildValue, wsChildValue, bNotify, bScriptModify,
                      FALSE);
  }
  if (m_pNode) {
    m_pNode->SetContent(wsExclGroup, wsExclGroup, bNotify, bScriptModify,
                        bSyncData);
  }
}

CXFA_Node* CXFA_WidgetData::GetExclGroupFirstMember() {
  CXFA_Node* pExcl = GetNode();
  if (!pExcl)
    return nullptr;

  CXFA_Node* pNode = pExcl->GetNodeItem(XFA_NODEITEM_FirstChild);
  while (pNode) {
    if (pNode->GetElementType() == XFA_Element::Field)
      return pNode;

    pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  return nullptr;
}
CXFA_Node* CXFA_WidgetData::GetExclGroupNextMember(CXFA_Node* pNode) {
  if (!pNode)
    return nullptr;

  CXFA_Node* pNodeField = pNode->GetNodeItem(XFA_NODEITEM_NextSibling);
  while (pNodeField) {
    if (pNodeField->GetElementType() == XFA_Element::Field)
      return pNodeField;

    pNodeField = pNodeField->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  return nullptr;
}

int32_t CXFA_WidgetData::GetChoiceListCommitOn() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_CommitOn);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::ChoiceList, XFA_ATTRIBUTE_CommitOn, XFA_XDPPACKET_Form);
}

FX_BOOL CXFA_WidgetData::IsChoiceListAllowTextEntry() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetBoolean(XFA_ATTRIBUTE_TextEntry);
  return XFA_GetAttributeDefaultValue_Boolean(
      XFA_Element::ChoiceList, XFA_ATTRIBUTE_TextEntry, XFA_XDPPACKET_Form);
}

int32_t CXFA_WidgetData::GetChoiceListOpen() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_Open);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::ChoiceList, XFA_ATTRIBUTE_Open, XFA_XDPPACKET_Form);
}

FX_BOOL CXFA_WidgetData::IsListBox() {
  int32_t iOpenMode = GetChoiceListOpen();
  return (iOpenMode == XFA_ATTRIBUTEENUM_Always ||
          iOpenMode == XFA_ATTRIBUTEENUM_MultiSelect);
}

int32_t CXFA_WidgetData::CountChoiceListItems(FX_BOOL bSaveValue) {
  CXFA_NodeArray pItems;
  CXFA_Node* pItem = nullptr;
  int32_t iCount = 0;
  CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() != XFA_Element::Items)
      continue;

    iCount++;
    pItems.Add(pNode);
    if (iCount == 2)
      break;
  }
  if (iCount == 0)
    return 0;

  pItem = pItems[0];
  if (iCount > 1) {
    FX_BOOL bItemOneHasSave = pItems[0]->GetBoolean(XFA_ATTRIBUTE_Save);
    FX_BOOL bItemTwoHasSave = pItems[1]->GetBoolean(XFA_ATTRIBUTE_Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItem = pItems[1];
  }
  pItems.RemoveAll();
  return pItem->CountChildren(XFA_Element::Unknown);
}

FX_BOOL CXFA_WidgetData::GetChoiceListItem(CFX_WideString& wsText,
                                           int32_t nIndex,
                                           FX_BOOL bSaveValue) {
  wsText.clear();
  CXFA_NodeArray pItemsArray;
  CXFA_Node* pItems = nullptr;
  int32_t iCount = 0;
  CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() != XFA_Element::Items)
      continue;

    iCount++;
    pItemsArray.Add(pNode);
    if (iCount == 2)
      break;
  }
  if (iCount == 0)
    return FALSE;

  pItems = pItemsArray[0];
  if (iCount > 1) {
    FX_BOOL bItemOneHasSave = pItemsArray[0]->GetBoolean(XFA_ATTRIBUTE_Save);
    FX_BOOL bItemTwoHasSave = pItemsArray[1]->GetBoolean(XFA_ATTRIBUTE_Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItems = pItemsArray[1];
  }
  if (pItems) {
    CXFA_Node* pItem = pItems->GetChild(nIndex, XFA_Element::Unknown);
    if (pItem) {
      pItem->TryContent(wsText);
      return TRUE;
    }
  }
  return FALSE;
}

void CXFA_WidgetData::GetChoiceListItems(CFX_WideStringArray& wsTextArray,
                                         FX_BOOL bSaveValue) {
  CXFA_NodeArray pItems;
  CXFA_Node* pItem = nullptr;
  int32_t iCount = 0;
  CXFA_Node* pNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pNode->GetElementType() != XFA_Element::Items)
      continue;

    iCount++;
    pItems.Add(pNode);
    if (iCount == 2)
      break;
  }
  if (iCount == 0)
    return;

  pItem = pItems[0];
  if (iCount > 1) {
    FX_BOOL bItemOneHasSave = pItems[0]->GetBoolean(XFA_ATTRIBUTE_Save);
    FX_BOOL bItemTwoHasSave = pItems[1]->GetBoolean(XFA_ATTRIBUTE_Save);
    if (bItemOneHasSave != bItemTwoHasSave && bSaveValue == bItemTwoHasSave)
      pItem = pItems[1];
  }
  pItems.RemoveAll();
  pNode = pItem->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pNode; pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling))
    pNode->TryContent(wsTextArray.Add());
}

int32_t CXFA_WidgetData::CountSelectedItems() {
  CFX_WideStringArray wsValueArray;
  GetSelectedItemsValue(wsValueArray);
  if (IsListBox() || !IsChoiceListAllowTextEntry())
    return wsValueArray.GetSize();

  int32_t iSelected = 0;
  CFX_WideStringArray wsSaveTextArray;
  GetChoiceListItems(wsSaveTextArray, TRUE);
  int32_t iValues = wsValueArray.GetSize();
  for (int32_t i = 0; i < iValues; i++) {
    int32_t iSaves = wsSaveTextArray.GetSize();
    for (int32_t j = 0; j < iSaves; j++) {
      if (wsValueArray[i] == wsSaveTextArray[j]) {
        iSelected++;
        break;
      }
    }
  }
  return iSelected;
}

int32_t CXFA_WidgetData::GetSelectedItem(int32_t nIndex) {
  CFX_WideStringArray wsValueArray;
  GetSelectedItemsValue(wsValueArray);
  CFX_WideStringArray wsSaveTextArray;
  GetChoiceListItems(wsSaveTextArray, TRUE);
  int32_t iSaves = wsSaveTextArray.GetSize();
  for (int32_t j = 0; j < iSaves; j++) {
    if (wsValueArray[nIndex] == wsSaveTextArray[j])
      return j;
  }
  return -1;
}

void CXFA_WidgetData::GetSelectedItems(CFX_Int32Array& iSelArray) {
  CFX_WideStringArray wsValueArray;
  GetSelectedItemsValue(wsValueArray);
  int32_t iValues = wsValueArray.GetSize();
  if (iValues < 1)
    return;

  CFX_WideStringArray wsSaveTextArray;
  GetChoiceListItems(wsSaveTextArray, TRUE);
  int32_t iSaves = wsSaveTextArray.GetSize();
  for (int32_t i = 0; i < iValues; i++) {
    for (int32_t j = 0; j < iSaves; j++) {
      if (wsValueArray[i] == wsSaveTextArray[j]) {
        iSelArray.Add(j);
        break;
      }
    }
  }
}

void CXFA_WidgetData::GetSelectedItemsValue(
    CFX_WideStringArray& wsSelTextArray) {
  CFX_WideString wsValue = GetRawValue();
  if (GetChoiceListOpen() == XFA_ATTRIBUTEENUM_MultiSelect) {
    if (!wsValue.IsEmpty()) {
      int32_t iStart = 0;
      int32_t iLength = wsValue.GetLength();
      int32_t iEnd = wsValue.Find(L'\n', iStart);
      iEnd = (iEnd == -1) ? iLength : iEnd;
      while (iEnd >= iStart) {
        wsSelTextArray.Add(wsValue.Mid(iStart, iEnd - iStart));
        iStart = iEnd + 1;
        if (iStart >= iLength)
          break;

        iEnd = wsValue.Find(L'\n', iStart);
        if (iEnd < 0)
          wsSelTextArray.Add(wsValue.Mid(iStart, iLength - iStart));
      }
    }
  } else {
    wsSelTextArray.Add(wsValue);
  }
}

FX_BOOL CXFA_WidgetData::GetItemState(int32_t nIndex) {
  if (nIndex < 0)
    return FALSE;

  CFX_WideStringArray wsSaveTextArray;
  GetChoiceListItems(wsSaveTextArray, TRUE);
  if (wsSaveTextArray.GetSize() <= nIndex)
    return FALSE;

  CFX_WideStringArray wsValueArray;
  GetSelectedItemsValue(wsValueArray);
  int32_t iValues = wsValueArray.GetSize();
  for (int32_t j = 0; j < iValues; j++) {
    if (wsValueArray[j] == wsSaveTextArray[nIndex])
      return TRUE;
  }
  return FALSE;
}

void CXFA_WidgetData::SetItemState(int32_t nIndex,
                                   FX_BOOL bSelected,
                                   bool bNotify,
                                   FX_BOOL bScriptModify,
                                   FX_BOOL bSyncData) {
  if (nIndex < 0)
    return;

  CFX_WideStringArray wsSaveTextArray;
  GetChoiceListItems(wsSaveTextArray, TRUE);
  if (wsSaveTextArray.GetSize() <= nIndex)
    return;

  int32_t iSel = -1;
  CFX_WideStringArray wsValueArray;
  GetSelectedItemsValue(wsValueArray);
  int32_t iValues = wsValueArray.GetSize();
  for (int32_t j = 0; j < iValues; j++) {
    if (wsValueArray[j] == wsSaveTextArray[nIndex]) {
      iSel = j;
      break;
    }
  }
  if (GetChoiceListOpen() == XFA_ATTRIBUTEENUM_MultiSelect) {
    if (bSelected) {
      if (iSel < 0) {
        CFX_WideString wsValue = GetRawValue();
        if (!wsValue.IsEmpty()) {
          wsValue += L"\n";
        }
        wsValue += wsSaveTextArray[nIndex];
        m_pNode->SetContent(wsValue, wsValue, bNotify, bScriptModify,
                            bSyncData);
      }
    } else if (iSel >= 0) {
      CFX_Int32Array iSelArray;
      GetSelectedItems(iSelArray);
      for (int32_t i = 0; i < iSelArray.GetSize(); i++) {
        if (iSelArray[i] == nIndex) {
          iSelArray.RemoveAt(i);
          break;
        }
      }
      SetSelectedItems(iSelArray, bNotify, bScriptModify, bSyncData);
    }
  } else {
    if (bSelected) {
      if (iSel < 0) {
        CFX_WideString wsSaveText = wsSaveTextArray[nIndex];
        CFX_WideString wsFormatText(wsSaveText);
        GetFormatDataValue(wsSaveText, wsFormatText);
        m_pNode->SetContent(wsSaveText, wsFormatText, bNotify, bScriptModify,
                            bSyncData);
      }
    } else if (iSel >= 0) {
      m_pNode->SetContent(CFX_WideString(), CFX_WideString(), bNotify,
                          bScriptModify, bSyncData);
    }
  }
}

void CXFA_WidgetData::SetSelectedItems(CFX_Int32Array& iSelArray,
                                       bool bNotify,
                                       FX_BOOL bScriptModify,
                                       FX_BOOL bSyncData) {
  CFX_WideString wsValue;
  int32_t iSize = iSelArray.GetSize();
  if (iSize >= 1) {
    CFX_WideStringArray wsSaveTextArray;
    GetChoiceListItems(wsSaveTextArray, TRUE);
    CFX_WideString wsItemValue;
    for (int32_t i = 0; i < iSize; i++) {
      wsItemValue = (iSize == 1)
                        ? wsSaveTextArray[iSelArray[i]]
                        : wsSaveTextArray[iSelArray[i]] + FX_WSTRC(L"\n");
      wsValue += wsItemValue;
    }
  }
  CFX_WideString wsFormat(wsValue);
  if (GetChoiceListOpen() != XFA_ATTRIBUTEENUM_MultiSelect)
    GetFormatDataValue(wsValue, wsFormat);

  m_pNode->SetContent(wsValue, wsFormat, bNotify, bScriptModify, bSyncData);
}

void CXFA_WidgetData::ClearAllSelections() {
  CXFA_Node* pBind = m_pNode->GetBindData();
  if (!pBind || GetChoiceListOpen() != XFA_ATTRIBUTEENUM_MultiSelect) {
    SyncValue(CFX_WideString(), false);
    return;
  }

  while (CXFA_Node* pChildNode = pBind->GetNodeItem(XFA_NODEITEM_FirstChild))
    pBind->RemoveChild(pChildNode);
}

void CXFA_WidgetData::InsertItem(const CFX_WideString& wsLabel,
                                 const CFX_WideString& wsValue,
                                 int32_t nIndex,
                                 FX_BOOL bNotify) {
  CFX_WideString wsNewValue(wsValue);
  if (wsNewValue.IsEmpty())
    wsNewValue = wsLabel;

  CXFA_NodeArray listitems;
  int32_t iCount = 0;
  CXFA_Node* pItemNode = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItemNode;
       pItemNode = pItemNode->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItemNode->GetElementType() != XFA_Element::Items)
      continue;

    listitems.Add(pItemNode);
    iCount++;
  }
  if (iCount < 1) {
    CXFA_Node* pItems = m_pNode->CreateSamePacketNode(XFA_Element::Items);
    m_pNode->InsertChild(-1, pItems);
    InsertListTextItem(pItems, wsLabel, nIndex);
    CXFA_Node* pSaveItems = m_pNode->CreateSamePacketNode(XFA_Element::Items);
    m_pNode->InsertChild(-1, pSaveItems);
    pSaveItems->SetBoolean(XFA_ATTRIBUTE_Save, TRUE);
    InsertListTextItem(pSaveItems, wsNewValue, nIndex);
  } else if (iCount > 1) {
    for (int32_t i = 0; i < 2; i++) {
      CXFA_Node* pNode = listitems[i];
      FX_BOOL bHasSave = pNode->GetBoolean(XFA_ATTRIBUTE_Save);
      if (bHasSave)
        InsertListTextItem(pNode, wsNewValue, nIndex);
      else
        InsertListTextItem(pNode, wsLabel, nIndex);
    }
  } else {
    CXFA_Node* pNode = listitems[0];
    pNode->SetBoolean(XFA_ATTRIBUTE_Save, FALSE);
    pNode->SetEnum(XFA_ATTRIBUTE_Presence, XFA_ATTRIBUTEENUM_Visible);
    CXFA_Node* pSaveItems = m_pNode->CreateSamePacketNode(XFA_Element::Items);
    m_pNode->InsertChild(-1, pSaveItems);
    pSaveItems->SetBoolean(XFA_ATTRIBUTE_Save, TRUE);
    pSaveItems->SetEnum(XFA_ATTRIBUTE_Presence, XFA_ATTRIBUTEENUM_Hidden);
    listitems.RemoveAll();
    CXFA_Node* pListNode = pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
    int32_t i = 0;
    while (pListNode) {
      CFX_WideString wsOldValue;
      pListNode->TryContent(wsOldValue);
      InsertListTextItem(pSaveItems, wsOldValue, i);
      i++;
      pListNode = pListNode->GetNodeItem(XFA_NODEITEM_NextSibling);
    }
    InsertListTextItem(pNode, wsLabel, nIndex);
    InsertListTextItem(pSaveItems, wsNewValue, nIndex);
  }
  if (!bNotify)
    return;

  m_pNode->GetDocument()->GetNotify()->OnWidgetListItemAdded(
      this, wsLabel.c_str(), wsValue.c_str(), nIndex);
}

void CXFA_WidgetData::GetItemLabel(const CFX_WideStringC& wsValue,
                                   CFX_WideString& wsLabel) {
  int32_t iCount = 0;
  CXFA_NodeArray listitems;
  CXFA_Node* pItems = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItems; pItems = pItems->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;

    iCount++;
    listitems.Add(pItems);
  }
  if (iCount <= 1) {
    wsLabel = wsValue;
  } else {
    CXFA_Node* pLabelItems = listitems[0];
    FX_BOOL bSave = pLabelItems->GetBoolean(XFA_ATTRIBUTE_Save);
    CXFA_Node* pSaveItems = nullptr;
    if (bSave) {
      pSaveItems = pLabelItems;
      pLabelItems = listitems[1];
    } else {
      pSaveItems = listitems[1];
    }
    iCount = 0;
    int32_t iSearch = -1;
    CFX_WideString wsContent;
    CXFA_Node* pChildItem = pSaveItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    for (; pChildItem;
         pChildItem = pChildItem->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      pChildItem->TryContent(wsContent);
      if (wsContent == wsValue) {
        iSearch = iCount;
        break;
      }
      iCount++;
    }
    if (iSearch < 0)
      return;
    if (CXFA_Node* pText =
            pLabelItems->GetChild(iSearch, XFA_Element::Unknown)) {
      pText->TryContent(wsLabel);
    }
  }
}

void CXFA_WidgetData::GetItemValue(const CFX_WideStringC& wsLabel,
                                   CFX_WideString& wsValue) {
  int32_t iCount = 0;
  CXFA_NodeArray listitems;
  CXFA_Node* pItems = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItems; pItems = pItems->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;

    iCount++;
    listitems.Add(pItems);
  }
  if (iCount <= 1) {
    wsValue = wsLabel;
  } else {
    CXFA_Node* pLabelItems = listitems[0];
    FX_BOOL bSave = pLabelItems->GetBoolean(XFA_ATTRIBUTE_Save);
    CXFA_Node* pSaveItems = nullptr;
    if (bSave) {
      pSaveItems = pLabelItems;
      pLabelItems = listitems[1];
    } else {
      pSaveItems = listitems[1];
    }
    iCount = 0;
    int32_t iSearch = -1;
    CFX_WideString wsContent;
    CXFA_Node* pChildItem = pLabelItems->GetNodeItem(XFA_NODEITEM_FirstChild);
    for (; pChildItem;
         pChildItem = pChildItem->GetNodeItem(XFA_NODEITEM_NextSibling)) {
      pChildItem->TryContent(wsContent);
      if (wsContent == wsLabel) {
        iSearch = iCount;
        break;
      }
      iCount++;
    }
    if (iSearch < 0)
      return;
    if (CXFA_Node* pText = pSaveItems->GetChild(iSearch, XFA_Element::Unknown))
      pText->TryContent(wsValue);
  }
}

FX_BOOL CXFA_WidgetData::DeleteItem(int32_t nIndex,
                                    FX_BOOL bNotify,
                                    FX_BOOL bScriptModify,
                                    FX_BOOL bSyncData) {
  FX_BOOL bSetValue = FALSE;
  CXFA_Node* pItems = m_pNode->GetNodeItem(XFA_NODEITEM_FirstChild);
  for (; pItems; pItems = pItems->GetNodeItem(XFA_NODEITEM_NextSibling)) {
    if (pItems->GetElementType() != XFA_Element::Items)
      continue;

    if (nIndex < 0) {
      while (CXFA_Node* pNode = pItems->GetNodeItem(XFA_NODEITEM_FirstChild)) {
        pItems->RemoveChild(pNode);
      }
    } else {
      if (!bSetValue && pItems->GetBoolean(XFA_ATTRIBUTE_Save)) {
        SetItemState(nIndex, FALSE, true, bScriptModify, bSyncData);
        bSetValue = TRUE;
      }
      int32_t i = 0;
      CXFA_Node* pNode = pItems->GetNodeItem(XFA_NODEITEM_FirstChild);
      while (pNode) {
        if (i == nIndex) {
          pItems->RemoveChild(pNode);
          break;
        }
        i++;
        pNode = pNode->GetNodeItem(XFA_NODEITEM_NextSibling);
      }
    }
  }
  if (bNotify)
    m_pNode->GetDocument()->GetNotify()->OnWidgetListItemRemoved(this, nIndex);
  return TRUE;
}

int32_t CXFA_WidgetData::GetHorizontalScrollPolicy() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_HScrollPolicy);
  return XFA_ATTRIBUTEENUM_Auto;
}

int32_t CXFA_WidgetData::GetNumberOfCells() {
  CXFA_Node* pUIChild = GetUIChild();
  if (!pUIChild)
    return -1;
  if (CXFA_Node* pNode = pUIChild->GetChild(0, XFA_Element::Comb))
    return pNode->GetInteger(XFA_ATTRIBUTE_NumberOfCells);
  return -1;
}

CFX_WideString CXFA_WidgetData::GetBarcodeType() {
  CXFA_Node* pUIChild = GetUIChild();
  return pUIChild ? CFX_WideString(pUIChild->GetCData(XFA_ATTRIBUTE_Type))
                  : CFX_WideString();
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_CharEncoding(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideString wsCharEncoding;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_CharEncoding, wsCharEncoding)) {
    if (wsCharEncoding.CompareNoCase(L"UTF-16")) {
      val = CHAR_ENCODING_UNICODE;
      return TRUE;
    }
    if (wsCharEncoding.CompareNoCase(L"UTF-8")) {
      val = CHAR_ENCODING_UTF8;
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_Checksum(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  XFA_ATTRIBUTEENUM eChecksum;
  if (pUIChild->TryEnum(XFA_ATTRIBUTE_Checksum, eChecksum)) {
    switch (eChecksum) {
      case XFA_ATTRIBUTEENUM_None:
        val = 0;
        return TRUE;
      case XFA_ATTRIBUTEENUM_Auto:
        val = 1;
        return TRUE;
      case XFA_ATTRIBUTEENUM_1mod10:
        break;
      case XFA_ATTRIBUTEENUM_1mod10_1mod11:
        break;
      case XFA_ATTRIBUTEENUM_2mod10:
        break;
      default:
        break;
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_DataLength(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideString wsDataLength;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_DataLength, wsDataLength)) {
    val = FXSYS_wtoi(wsDataLength.c_str());
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_StartChar(FX_CHAR& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideStringC wsStartEndChar;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_StartChar, wsStartEndChar)) {
    if (wsStartEndChar.GetLength()) {
      val = (FX_CHAR)wsStartEndChar.GetAt(0);
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_EndChar(FX_CHAR& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideStringC wsStartEndChar;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_EndChar, wsStartEndChar)) {
    if (wsStartEndChar.GetLength()) {
      val = (FX_CHAR)wsStartEndChar.GetAt(0);
      return TRUE;
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_ECLevel(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideString wsECLevel;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_ErrorCorrectionLevel, wsECLevel)) {
    val = FXSYS_wtoi(wsECLevel.c_str());
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_ModuleWidth(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CXFA_Measurement mModuleWidthHeight;
  if (pUIChild->TryMeasure(XFA_ATTRIBUTE_ModuleWidth, mModuleWidthHeight)) {
    val = (int32_t)mModuleWidthHeight.ToUnit(XFA_UNIT_Pt);
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_ModuleHeight(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CXFA_Measurement mModuleWidthHeight;
  if (pUIChild->TryMeasure(XFA_ATTRIBUTE_ModuleHeight, mModuleWidthHeight)) {
    val = (int32_t)mModuleWidthHeight.ToUnit(XFA_UNIT_Pt);
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_PrintChecksum(FX_BOOL& val) {
  CXFA_Node* pUIChild = GetUIChild();
  FX_BOOL bPrintCheckDigit;
  if (pUIChild->TryBoolean(XFA_ATTRIBUTE_PrintCheckDigit, bPrintCheckDigit)) {
    val = bPrintCheckDigit;
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_TextLocation(int32_t& val) {
  CXFA_Node* pUIChild = GetUIChild();
  XFA_ATTRIBUTEENUM eTextLocation;
  if (pUIChild->TryEnum(XFA_ATTRIBUTE_TextLocation, eTextLocation)) {
    switch (eTextLocation) {
      case XFA_ATTRIBUTEENUM_None:
        val = BC_TEXT_LOC_NONE;
        return TRUE;
      case XFA_ATTRIBUTEENUM_Above:
        val = BC_TEXT_LOC_ABOVE;
        return TRUE;
      case XFA_ATTRIBUTEENUM_Below:
        val = BC_TEXT_LOC_BELOW;
        return TRUE;
      case XFA_ATTRIBUTEENUM_AboveEmbedded:
        val = BC_TEXT_LOC_ABOVEEMBED;
        return TRUE;
      case XFA_ATTRIBUTEENUM_BelowEmbedded:
        val = BC_TEXT_LOC_BELOWEMBED;
        return TRUE;
      default:
        break;
    }
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_Truncate(FX_BOOL& val) {
  CXFA_Node* pUIChild = GetUIChild();
  FX_BOOL bTruncate;
  if (pUIChild->TryBoolean(XFA_ATTRIBUTE_Truncate, bTruncate)) {
    val = bTruncate;
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetBarcodeAttribute_WideNarrowRatio(FX_FLOAT& val) {
  CXFA_Node* pUIChild = GetUIChild();
  CFX_WideString wsWideNarrowRatio;
  if (pUIChild->TryCData(XFA_ATTRIBUTE_WideNarrowRatio, wsWideNarrowRatio)) {
    FX_STRSIZE ptPos = wsWideNarrowRatio.Find(':');
    FX_FLOAT fRatio = 0;
    if (ptPos >= 0) {
      fRatio = (FX_FLOAT)FXSYS_wtoi(wsWideNarrowRatio.c_str());
    } else {
      int32_t fA, fB;
      fA = FXSYS_wtoi(wsWideNarrowRatio.Left(ptPos).c_str());
      fB = FXSYS_wtoi(wsWideNarrowRatio.Mid(ptPos + 1).c_str());
      if (fB)
        fRatio = (FX_FLOAT)fA / fB;
    }
    val = fRatio;
    return TRUE;
  }
  return FALSE;
}

void CXFA_WidgetData::GetPasswordChar(CFX_WideString& wsPassWord) {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild) {
    pUIChild->TryCData(XFA_ATTRIBUTE_PasswordChar, wsPassWord);
  } else {
    wsPassWord = XFA_GetAttributeDefaultValue_Cdata(XFA_Element::PasswordEdit,
                                                    XFA_ATTRIBUTE_PasswordChar,
                                                    XFA_XDPPACKET_Form);
  }
}

FX_BOOL CXFA_WidgetData::IsMultiLine() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetBoolean(XFA_ATTRIBUTE_MultiLine);
  return XFA_GetAttributeDefaultValue_Boolean(
      XFA_Element::TextEdit, XFA_ATTRIBUTE_MultiLine, XFA_XDPPACKET_Form);
}

int32_t CXFA_WidgetData::GetVerticalScrollPolicy() {
  CXFA_Node* pUIChild = GetUIChild();
  if (pUIChild)
    return pUIChild->GetEnum(XFA_ATTRIBUTE_VScrollPolicy);
  return XFA_GetAttributeDefaultValue_Enum(
      XFA_Element::TextEdit, XFA_ATTRIBUTE_VScrollPolicy, XFA_XDPPACKET_Form);
}

int32_t CXFA_WidgetData::GetMaxChars(XFA_Element& eType) {
  if (CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Value)) {
    if (CXFA_Node* pChild = pNode->GetNodeItem(XFA_NODEITEM_FirstChild)) {
      switch (pChild->GetElementType()) {
        case XFA_Element::Text:
          eType = XFA_Element::Text;
          return pChild->GetInteger(XFA_ATTRIBUTE_MaxChars);
        case XFA_Element::ExData: {
          eType = XFA_Element::ExData;
          int32_t iMax = pChild->GetInteger(XFA_ATTRIBUTE_MaxLength);
          return iMax < 0 ? 0 : iMax;
        }
        default:
          break;
      }
    }
  }
  return 0;
}

FX_BOOL CXFA_WidgetData::GetFracDigits(int32_t& iFracDigits) {
  if (CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Value)) {
    if (CXFA_Node* pChild = pNode->GetChild(0, XFA_Element::Decimal))
      return pChild->TryInteger(XFA_ATTRIBUTE_FracDigits, iFracDigits);
  }
  iFracDigits = -1;
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetLeadDigits(int32_t& iLeadDigits) {
  if (CXFA_Node* pNode = m_pNode->GetChild(0, XFA_Element::Value)) {
    if (CXFA_Node* pChild = pNode->GetChild(0, XFA_Element::Decimal))
      return pChild->TryInteger(XFA_ATTRIBUTE_LeadDigits, iLeadDigits);
  }
  iLeadDigits = -1;
  return FALSE;
}

FX_BOOL CXFA_WidgetData::SetValue(const CFX_WideString& wsValue,
                                  XFA_VALUEPICTURE eValueType) {
  if (wsValue.IsEmpty()) {
    SyncValue(wsValue, true);
    return TRUE;
  }
  m_bPreNull = m_bIsNull;
  m_bIsNull = FALSE;
  CFX_WideString wsNewText(wsValue);
  CFX_WideString wsPicture;
  GetPictureContent(wsPicture, eValueType);
  FX_BOOL bValidate = TRUE;
  FX_BOOL bSyncData = FALSE;
  CXFA_Node* pNode = GetUIChild();
  if (!pNode)
    return TRUE;

  XFA_Element eType = pNode->GetElementType();
  if (!wsPicture.IsEmpty()) {
    CXFA_LocaleMgr* pLocalMgr = m_pNode->GetDocument()->GetLocalMgr();
    IFX_Locale* pLocale = GetLocal();
    CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
    bValidate =
        widgetValue.ValidateValue(wsValue, wsPicture, pLocale, &wsPicture);
    if (bValidate) {
      widgetValue = CXFA_LocaleValue(widgetValue.GetType(), wsNewText,
                                     wsPicture, pLocale, pLocalMgr);
      wsNewText = widgetValue.GetValue();
      if (eType == XFA_Element::NumericEdit) {
        int32_t iLeadDigits = 0;
        int32_t iFracDigits = 0;
        GetLeadDigits(iLeadDigits);
        GetFracDigits(iFracDigits);
        wsNewText = NumericLimit(wsNewText, iLeadDigits, iFracDigits);
      }
      bSyncData = TRUE;
    }
  } else {
    if (eType == XFA_Element::NumericEdit) {
      if (wsNewText != FX_WSTRC(L"0")) {
        int32_t iLeadDigits = 0;
        int32_t iFracDigits = 0;
        GetLeadDigits(iLeadDigits);
        GetFracDigits(iFracDigits);
        wsNewText = NumericLimit(wsNewText, iLeadDigits, iFracDigits);
      }
      bSyncData = TRUE;
    }
  }
  if (eType != XFA_Element::NumericEdit || bSyncData)
    SyncValue(wsNewText, true);

  return bValidate;
}

FX_BOOL CXFA_WidgetData::GetPictureContent(CFX_WideString& wsPicture,
                                           XFA_VALUEPICTURE ePicture) {
  if (ePicture == XFA_VALUEPICTURE_Raw)
    return FALSE;

  CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
  switch (ePicture) {
    case XFA_VALUEPICTURE_Display: {
      if (CXFA_Node* pFormat = m_pNode->GetChild(0, XFA_Element::Format)) {
        if (CXFA_Node* pPicture = pFormat->GetChild(0, XFA_Element::Picture)) {
          if (pPicture->TryContent(wsPicture))
            return TRUE;
        }
      }
      CFX_WideString wsDataPicture, wsTimePicture;
      IFX_Locale* pLocale = GetLocal();
      if (!pLocale)
        return FALSE;

      uint32_t dwType = widgetValue.GetType();
      switch (dwType) {
        case XFA_VT_DATE:
          pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium,
                                  wsPicture);
          break;
        case XFA_VT_TIME:
          pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium,
                                  wsPicture);
          break;
        case XFA_VT_DATETIME:
          pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium,
                                  wsDataPicture);
          pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Medium,
                                  wsTimePicture);
          wsPicture = wsDataPicture + FX_WSTRC(L"T") + wsTimePicture;
          break;
        case XFA_VT_DECIMAL:
        case XFA_VT_FLOAT:
          break;
        default:
          break;
      }
      return TRUE;
    }

    case XFA_VALUEPICTURE_Edit: {
      CXFA_Node* pUI = m_pNode->GetChild(0, XFA_Element::Ui);
      if (pUI) {
        if (CXFA_Node* pPicture = pUI->GetChild(0, XFA_Element::Picture)) {
          if (pPicture->TryContent(wsPicture))
            return TRUE;
        }
      }
      {
        CFX_WideString wsDataPicture, wsTimePicture;
        IFX_Locale* pLocale = GetLocal();
        if (!pLocale) {
          return FALSE;
        }
        uint32_t dwType = widgetValue.GetType();
        switch (dwType) {
          case XFA_VT_DATE:
            pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Short,
                                    wsPicture);
            break;
          case XFA_VT_TIME:
            pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Short,
                                    wsPicture);
            break;
          case XFA_VT_DATETIME:
            pLocale->GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY_Short,
                                    wsDataPicture);
            pLocale->GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY_Short,
                                    wsTimePicture);
            wsPicture = wsDataPicture + L"T" + wsTimePicture;
            break;
          default:
            break;
        }
      }
      return TRUE;
    }
    case XFA_VALUEPICTURE_DataBind: {
      if (CXFA_Bind bind = GetBind()) {
        bind.GetPicture(wsPicture);
        return TRUE;
      }
      break;
    }
    default:
      break;
  }
  return FALSE;
}

IFX_Locale* CXFA_WidgetData::GetLocal() {
  if (!m_pNode)
    return nullptr;

  CFX_WideString wsLocaleName;
  if (!m_pNode->GetLocaleName(wsLocaleName))
    return nullptr;
  if (wsLocaleName == FX_WSTRC(L"ambient"))
    return m_pNode->GetDocument()->GetLocalMgr()->GetDefLocale();
  return m_pNode->GetDocument()->GetLocalMgr()->GetLocaleByName(wsLocaleName);
}

FX_BOOL CXFA_WidgetData::GetValue(CFX_WideString& wsValue,
                                  XFA_VALUEPICTURE eValueType) {
  wsValue = m_pNode->GetContent();

  if (eValueType == XFA_VALUEPICTURE_Display)
    GetItemLabel(wsValue.AsStringC(), wsValue);

  CFX_WideString wsPicture;
  GetPictureContent(wsPicture, eValueType);
  CXFA_Node* pNode = GetUIChild();
  if (!pNode)
    return TRUE;

  switch (GetUIChild()->GetElementType()) {
    case XFA_Element::ChoiceList: {
      if (eValueType == XFA_VALUEPICTURE_Display) {
        int32_t iSelItemIndex = GetSelectedItem(0);
        if (iSelItemIndex >= 0) {
          GetChoiceListItem(wsValue, iSelItemIndex);
          wsPicture.clear();
        }
      }
    } break;
    case XFA_Element::NumericEdit:
      if (eValueType != XFA_VALUEPICTURE_Raw && wsPicture.IsEmpty()) {
        IFX_Locale* pLocale = GetLocal();
        if (eValueType == XFA_VALUEPICTURE_Display && pLocale) {
          CFX_WideString wsOutput;
          NormalizeNumStr(wsValue, wsOutput);
          FormatNumStr(wsOutput, pLocale, wsOutput);
          wsValue = wsOutput;
        }
      }
      break;
    default:
      break;
  }
  if (wsPicture.IsEmpty())
    return TRUE;

  if (IFX_Locale* pLocale = GetLocal()) {
    CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
    CXFA_LocaleMgr* pLocalMgr = m_pNode->GetDocument()->GetLocalMgr();
    switch (widgetValue.GetType()) {
      case XFA_VT_DATE: {
        CFX_WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue date(XFA_VT_DATE, wsDate, pLocalMgr);
          if (date.FormatPatterns(wsValue, wsPicture, pLocale, eValueType))
            return TRUE;
        }
        break;
      }
      case XFA_VT_TIME: {
        CFX_WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue time(XFA_VT_TIME, wsTime, pLocalMgr);
          if (time.FormatPatterns(wsValue, wsPicture, pLocale, eValueType))
            return TRUE;
        }
        break;
      }
      default:
        break;
    }
    widgetValue.FormatPatterns(wsValue, wsPicture, pLocale, eValueType);
  }
  return TRUE;
}

FX_BOOL CXFA_WidgetData::GetNormalizeDataValue(
    const CFX_WideString& wsValue,
    CFX_WideString& wsNormalizeValue) {
  wsNormalizeValue = wsValue;
  if (wsValue.IsEmpty())
    return TRUE;

  CFX_WideString wsPicture;
  GetPictureContent(wsPicture, XFA_VALUEPICTURE_DataBind);
  if (wsPicture.IsEmpty())
    return TRUE;

  ASSERT(GetNode());
  CXFA_LocaleMgr* pLocalMgr = GetNode()->GetDocument()->GetLocalMgr();
  IFX_Locale* pLocale = GetLocal();
  CXFA_LocaleValue widgetValue = XFA_GetLocaleValue(this);
  if (widgetValue.ValidateValue(wsValue, wsPicture, pLocale, &wsPicture)) {
    widgetValue = CXFA_LocaleValue(widgetValue.GetType(), wsNormalizeValue,
                                   wsPicture, pLocale, pLocalMgr);
    wsNormalizeValue = widgetValue.GetValue();
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CXFA_WidgetData::GetFormatDataValue(const CFX_WideString& wsValue,
                                            CFX_WideString& wsFormatedValue) {
  wsFormatedValue = wsValue;
  if (wsValue.IsEmpty())
    return TRUE;

  CFX_WideString wsPicture;
  GetPictureContent(wsPicture, XFA_VALUEPICTURE_DataBind);
  if (wsPicture.IsEmpty())
    return TRUE;

  if (IFX_Locale* pLocale = GetLocal()) {
    ASSERT(GetNode());
    CXFA_Node* pNodeValue = GetNode()->GetChild(0, XFA_Element::Value);
    if (!pNodeValue)
      return FALSE;

    CXFA_Node* pValueChild = pNodeValue->GetNodeItem(XFA_NODEITEM_FirstChild);
    if (!pValueChild)
      return FALSE;

    int32_t iVTType = XFA_VT_NULL;
    switch (pValueChild->GetElementType()) {
      case XFA_Element::Decimal:
        iVTType = XFA_VT_DECIMAL;
        break;
      case XFA_Element::Float:
        iVTType = XFA_VT_FLOAT;
        break;
      case XFA_Element::Date:
        iVTType = XFA_VT_DATE;
        break;
      case XFA_Element::Time:
        iVTType = XFA_VT_TIME;
        break;
      case XFA_Element::DateTime:
        iVTType = XFA_VT_DATETIME;
        break;
      case XFA_Element::Boolean:
        iVTType = XFA_VT_BOOLEAN;
        break;
      case XFA_Element::Integer:
        iVTType = XFA_VT_INTEGER;
        break;
      case XFA_Element::Text:
        iVTType = XFA_VT_TEXT;
        break;
      default:
        iVTType = XFA_VT_NULL;
        break;
    }
    CXFA_LocaleMgr* pLocalMgr = GetNode()->GetDocument()->GetLocalMgr();
    CXFA_LocaleValue widgetValue(iVTType, wsValue, pLocalMgr);
    switch (widgetValue.GetType()) {
      case XFA_VT_DATE: {
        CFX_WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue date(XFA_VT_DATE, wsDate, pLocalMgr);
          if (date.FormatPatterns(wsFormatedValue, wsPicture, pLocale,
                                  XFA_VALUEPICTURE_DataBind)) {
            return TRUE;
          }
        }
        break;
      }
      case XFA_VT_TIME: {
        CFX_WideString wsDate, wsTime;
        if (SplitDateTime(wsValue, wsDate, wsTime)) {
          CXFA_LocaleValue time(XFA_VT_TIME, wsTime, pLocalMgr);
          if (time.FormatPatterns(wsFormatedValue, wsPicture, pLocale,
                                  XFA_VALUEPICTURE_DataBind)) {
            return TRUE;
          }
        }
        break;
      }
      default:
        break;
    }
    widgetValue.FormatPatterns(wsFormatedValue, wsPicture, pLocale,
                               XFA_VALUEPICTURE_DataBind);
  }
  return FALSE;
}

void CXFA_WidgetData::NormalizeNumStr(const CFX_WideString& wsValue,
                                      CFX_WideString& wsOutput) {
  if (wsValue.IsEmpty())
    return;

  wsOutput = wsValue;
  wsOutput.TrimLeft('0');
  int32_t dot_index = wsOutput.Find('.');
  int32_t iFracDigits = 0;
  if (!wsOutput.IsEmpty() && dot_index >= 0 &&
      (!GetFracDigits(iFracDigits) || iFracDigits != -1)) {
    wsOutput.TrimRight(L"0");
    wsOutput.TrimRight(L".");
  }
  if (wsOutput.IsEmpty() || wsOutput[0] == '.')
    wsOutput.Insert(0, '0');
}

void CXFA_WidgetData::FormatNumStr(const CFX_WideString& wsValue,
                                   IFX_Locale* pLocale,
                                   CFX_WideString& wsOutput) {
  if (wsValue.IsEmpty())
    return;

  CFX_WideString wsSrcNum = wsValue;
  CFX_WideString wsGroupSymbol;
  pLocale->GetNumbericSymbol(FX_LOCALENUMSYMBOL_Grouping, wsGroupSymbol);
  FX_BOOL bNeg = FALSE;
  if (wsSrcNum[0] == '-') {
    bNeg = TRUE;
    wsSrcNum.Delete(0, 1);
  }
  int32_t len = wsSrcNum.GetLength();
  int32_t dot_index = wsSrcNum.Find('.');
  if (dot_index == -1)
    dot_index = len;

  int32_t cc = dot_index - 1;
  if (cc >= 0) {
    int nPos = dot_index % 3;
    wsOutput.clear();
    for (int32_t i = 0; i < dot_index; i++) {
      if (i % 3 == nPos && i != 0)
        wsOutput += wsGroupSymbol;

      wsOutput += wsSrcNum[i];
    }
    if (dot_index < len) {
      CFX_WideString wsSymbol;
      pLocale->GetNumbericSymbol(FX_LOCALENUMSYMBOL_Decimal, wsSymbol);
      wsOutput += wsSymbol;
      wsOutput += wsSrcNum.Right(len - dot_index - 1);
    }
    if (bNeg) {
      CFX_WideString wsMinusymbol;
      pLocale->GetNumbericSymbol(FX_LOCALENUMSYMBOL_Minus, wsMinusymbol);
      wsOutput = wsMinusymbol + wsOutput;
    }
  }
}

void CXFA_WidgetData::SyncValue(const CFX_WideString& wsValue, bool bNotify) {
  if (!m_pNode)
    return;

  CFX_WideString wsFormatValue(wsValue);
  CXFA_WidgetData* pContainerWidgetData = m_pNode->GetContainerWidgetData();
  if (pContainerWidgetData)
    pContainerWidgetData->GetFormatDataValue(wsValue, wsFormatValue);

  m_pNode->SetContent(wsValue, wsFormatValue, bNotify);
}

void CXFA_WidgetData::InsertListTextItem(CXFA_Node* pItems,
                                         const CFX_WideString& wsText,
                                         int32_t nIndex) {
  CXFA_Node* pText = pItems->CreateSamePacketNode(XFA_Element::Text);
  pItems->InsertChild(nIndex, pText);
  pText->SetContent(wsText, wsText, FALSE, FALSE, FALSE);
}

CFX_WideString CXFA_WidgetData::NumericLimit(const CFX_WideString& wsValue,
                                             int32_t iLead,
                                             int32_t iTread) const {
  if ((iLead == -1) && (iTread == -1))
    return wsValue;

  CFX_WideString wsRet;
  int32_t iLead_ = 0, iTread_ = -1;
  int32_t iCount = wsValue.GetLength();
  if (iCount == 0)
    return wsValue;

  int32_t i = 0;
  if (wsValue[i] == L'-') {
    wsRet += L'-';
    i++;
  }
  for (; i < iCount; i++) {
    FX_WCHAR wc = wsValue[i];
    if (FXSYS_isDecimalDigit(wc)) {
      if (iLead >= 0) {
        iLead_++;
        if (iLead_ > iLead)
          return L"0";
      } else if (iTread_ >= 0) {
        iTread_++;
        if (iTread_ > iTread) {
          if (iTread != -1) {
            CFX_Decimal wsDeci = CFX_Decimal(wsValue.AsStringC());
            wsDeci.SetScale(iTread);
            wsRet = wsDeci;
          }
          return wsRet;
        }
      }
    } else if (wc == L'.') {
      iTread_ = 0;
      iLead = -1;
    }
    wsRet += wc;
  }
  return wsRet;
}
