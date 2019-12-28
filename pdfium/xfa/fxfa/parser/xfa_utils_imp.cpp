// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_utils.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fde/xml/fde_xml_imp.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_localevalue.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"

namespace {

const FX_DOUBLE fraction_scales[] = {0.1,
                                     0.01,
                                     0.001,
                                     0.0001,
                                     0.00001,
                                     0.000001,
                                     0.0000001,
                                     0.00000001,
                                     0.000000001,
                                     0.0000000001,
                                     0.00000000001,
                                     0.000000000001,
                                     0.0000000000001,
                                     0.00000000000001,
                                     0.000000000000001,
                                     0.0000000000000001};

FX_DOUBLE WideStringToDouble(const CFX_WideString& wsStringVal) {
  CFX_WideString wsValue = wsStringVal;
  wsValue.TrimLeft();
  wsValue.TrimRight();
  int64_t nIntegral = 0;
  uint32_t dwFractional = 0;
  int32_t nExponent = 0;
  int32_t cc = 0;
  bool bNegative = false;
  bool bExpSign = false;
  const FX_WCHAR* str = wsValue.c_str();
  int32_t len = wsValue.GetLength();
  if (str[0] == '+') {
    cc++;
  } else if (str[0] == '-') {
    bNegative = true;
    cc++;
  }
  int32_t nIntegralLen = 0;
  while (cc < len) {
    if (str[cc] == '.' || str[cc] == 'E' || str[cc] == 'e' ||
        nIntegralLen > 17) {
      break;
    }
    if (!FXSYS_isDecimalDigit(str[cc])) {
      return 0;
    }
    nIntegral = nIntegral * 10 + str[cc] - '0';
    cc++;
    nIntegralLen++;
  }
  nIntegral = bNegative ? -nIntegral : nIntegral;
  int32_t scale = 0;
  FX_DOUBLE fraction = 0.0;
  if (cc < len && str[cc] == '.') {
    cc++;
    while (cc < len) {
      fraction += fraction_scales[scale] * (str[cc] - '0');
      scale++;
      cc++;
      if (cc == len) {
        break;
      }
      if (scale == sizeof(fraction_scales) / sizeof(FX_DOUBLE) ||
          str[cc] == 'E' || str[cc] == 'e') {
        break;
      }
      if (!FXSYS_isDecimalDigit(str[cc])) {
        return 0;
      }
    }
    dwFractional = (uint32_t)(fraction * 4294967296.0);
  }
  if (cc < len && (str[cc] == 'E' || str[cc] == 'e')) {
    cc++;
    if (cc < len) {
      if (str[cc] == '+') {
        cc++;
      } else if (str[cc] == '-') {
        bExpSign = true;
        cc++;
      }
    }
    while (cc < len) {
      if (str[cc] == '.' || !FXSYS_isDecimalDigit(str[cc])) {
        return 0;
      }
      nExponent = nExponent * 10 + str[cc] - '0';
      cc++;
    }
    nExponent = bExpSign ? -nExponent : nExponent;
  }
  FX_DOUBLE dValue = (dwFractional / 4294967296.0);
  dValue = nIntegral + (nIntegral >= 0 ? dValue : -dValue);
  if (nExponent != 0) {
    dValue *= FXSYS_pow(10, (FX_FLOAT)nExponent);
  }
  return dValue;
}

}  // namespace

CXFA_LocaleValue XFA_GetLocaleValue(CXFA_WidgetData* pWidgetData) {
  CXFA_Node* pNodeValue =
      pWidgetData->GetNode()->GetChild(0, XFA_Element::Value);
  if (!pNodeValue) {
    return CXFA_LocaleValue();
  }
  CXFA_Node* pValueChild = pNodeValue->GetNodeItem(XFA_NODEITEM_FirstChild);
  if (!pValueChild) {
    return CXFA_LocaleValue();
  }
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
  return CXFA_LocaleValue(iVTType, pWidgetData->GetRawValue(),
                          pWidgetData->GetNode()->GetDocument()->GetLocalMgr());
}
void XFA_GetPlainTextFromRichText(CFDE_XMLNode* pXMLNode,
                                  CFX_WideString& wsPlainText) {
  if (!pXMLNode) {
    return;
  }
  switch (pXMLNode->GetType()) {
    case FDE_XMLNODE_Element: {
      CFDE_XMLElement* pXMLElement = static_cast<CFDE_XMLElement*>(pXMLNode);
      CFX_WideString wsTag;
      pXMLElement->GetLocalTagName(wsTag);
      uint32_t uTag = FX_HashCode_GetW(wsTag.AsStringC(), true);
      if (uTag == 0x0001f714) {
        wsPlainText += L"\n";
      } else if (uTag == 0x00000070) {
        if (!wsPlainText.IsEmpty()) {
          wsPlainText += L"\n";
        }
      } else if (uTag == 0xa48ac63) {
        if (!wsPlainText.IsEmpty() &&
            wsPlainText[wsPlainText.GetLength() - 1] != '\n') {
          wsPlainText += L"\n";
        }
      }
    } break;
    case FDE_XMLNODE_Text: {
      CFX_WideString wsContent;
      static_cast<CFDE_XMLText*>(pXMLNode)->GetText(wsContent);
      wsPlainText += wsContent;
    } break;
    case FDE_XMLNODE_CharData: {
      CFX_WideString wsCharData;
      static_cast<CFDE_XMLCharData*>(pXMLNode)->GetCharData(wsCharData);
      wsPlainText += wsCharData;
    } break;
    default:
      break;
  }
  for (CFDE_XMLNode* pChildXML =
           pXMLNode->GetNodeItem(CFDE_XMLNode::FirstChild);
       pChildXML;
       pChildXML = pChildXML->GetNodeItem(CFDE_XMLNode::NextSibling)) {
    XFA_GetPlainTextFromRichText(pChildXML, wsPlainText);
  }
}
FX_BOOL XFA_FieldIsMultiListBox(CXFA_Node* pFieldNode) {
  FX_BOOL bRet = FALSE;
  if (!pFieldNode) {
    return bRet;
  }
  CXFA_Node* pUIChild = pFieldNode->GetChild(0, XFA_Element::Ui);
  if (pUIChild) {
    CXFA_Node* pFirstChild = pUIChild->GetNodeItem(XFA_NODEITEM_FirstChild);
    if (pFirstChild &&
        pFirstChild->GetElementType() == XFA_Element::ChoiceList) {
      bRet = pFirstChild->GetEnum(XFA_ATTRIBUTE_Open) ==
             XFA_ATTRIBUTEENUM_MultiSelect;
    }
  }
  return bRet;
}

FX_DOUBLE XFA_ByteStringToDouble(const CFX_ByteStringC& szStringVal) {
  CFX_WideString wsValue = CFX_WideString::FromUTF8(szStringVal);
  return WideStringToDouble(wsValue);
}

int32_t XFA_MapRotation(int32_t nRotation) {
  nRotation = nRotation % 360;
  nRotation = nRotation < 0 ? nRotation + 360 : nRotation;
  return nRotation;
}
