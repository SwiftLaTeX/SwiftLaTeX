// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_locale.h"

#include "core/fxcrt/include/fx_xml.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

static const FX_WCHAR g_FX_Percent[] = L"z,zzz,zzz,zzz,zzz,zzz%";
static const FX_WCHAR g_FX_Currency[] = L"$z,zzz,zzz,zzz,zzz,zz9.99";
static const FX_WCHAR g_FX_Decimal[] = L"z,zzz,zzz,zzz,zzz,zz9.zzz";
static const FX_WCHAR g_FX_Integer[] = L"z,zzz,zzz,zzz,zzz,zzz";

CXFA_XMLLocale::CXFA_XMLLocale(CXML_Element* pLocaleData)
    : m_pLocaleData(pLocaleData) {}

CXFA_XMLLocale::~CXFA_XMLLocale() {
  delete m_pLocaleData;
}

void CXFA_XMLLocale::Release() {
  delete this;
}
CFX_WideString CXFA_XMLLocale::GetName() {
  return m_pLocaleData ? m_pLocaleData->GetAttrValue("name") : CFX_WideString();
}
void CXFA_XMLLocale::GetNumbericSymbol(FX_LOCALENUMSYMBOL eType,
                                       CFX_WideString& wsNumSymbol) const {
  CFX_ByteString bsSymbols;
  CFX_WideString wsName;
  switch (eType) {
    case FX_LOCALENUMSYMBOL_Decimal:
      bsSymbols = "numberSymbols";
      wsName = FX_WSTRC(L"decimal");
      break;
    case FX_LOCALENUMSYMBOL_Grouping:
      bsSymbols = "numberSymbols";
      wsName = FX_WSTRC(L"grouping");
      break;
    case FX_LOCALENUMSYMBOL_Percent:
      bsSymbols = "numberSymbols";
      wsName = FX_WSTRC(L"percent");
      break;
    case FX_LOCALENUMSYMBOL_Minus:
      bsSymbols = "numberSymbols";
      wsName = FX_WSTRC(L"minus");
      break;
    case FX_LOCALENUMSYMBOL_Zero:
      bsSymbols = "numberSymbols";
      wsName = FX_WSTRC(L"zero");
      break;
    case FX_LOCALENUMSYMBOL_CurrencySymbol:
      bsSymbols = "currencySymbols";
      wsName = FX_WSTRC(L"symbol");
      break;
    case FX_LOCALENUMSYMBOL_CurrencyName:
      bsSymbols = "currencySymbols";
      wsName = FX_WSTRC(L"isoname");
      break;
    default:
      return;
  }
  CXML_Element* pElement = m_pLocaleData->GetElement("", bsSymbols.AsStringC());
  if (!pElement) {
    return;
  }
  GetPattern(pElement,
             CFX_ByteStringC(bsSymbols.c_str(), bsSymbols.GetLength() - 1),
             wsName.AsStringC(), wsNumSymbol);
}
void CXFA_XMLLocale::GetDateTimeSymbols(CFX_WideString& wsDtSymbol) const {
  if (!m_pLocaleData) {
    return;
  }
  CFX_ByteString bsSpace;
  CXML_Element* pNumberSymbols =
      m_pLocaleData->GetElement(bsSpace.AsStringC(), "dateTimeSymbols");
  if (!pNumberSymbols) {
    return;
  }
  wsDtSymbol = pNumberSymbols->GetContent(0);
}
void CXFA_XMLLocale::GetMonthName(int32_t nMonth,
                                  CFX_WideString& wsMonthName,
                                  FX_BOOL bAbbr) const {
  wsMonthName = GetCalendarSymbol("month", nMonth, bAbbr);
}
void CXFA_XMLLocale::GetDayName(int32_t nWeek,
                                CFX_WideString& wsDayName,
                                FX_BOOL bAbbr) const {
  wsDayName = GetCalendarSymbol("day", nWeek, bAbbr);
}
void CXFA_XMLLocale::GetMeridiemName(CFX_WideString& wsMeridiemName,
                                     FX_BOOL bAM) const {
  wsMeridiemName = GetCalendarSymbol("meridiem", bAM ? 0 : 1, FALSE);
}
void CXFA_XMLLocale::GetTimeZone(FX_TIMEZONE& tz) const {
  CXFA_TimeZoneProvider::Get()->GetTimeZone(tz);
}
void CXFA_XMLLocale::GetEraName(CFX_WideString& wsEraName, FX_BOOL bAD) const {
  wsEraName = GetCalendarSymbol("era", bAD ? 1 : 0, FALSE);
}
CFX_WideString CXFA_XMLLocale::GetCalendarSymbol(const CFX_ByteStringC& symbol,
                                                 int index,
                                                 FX_BOOL bAbbr) const {
  CFX_ByteString pstrSymbolNames = symbol + "Names";
  CFX_WideString wsSymbolName = L"";
  if (m_pLocaleData) {
    CXML_Element* pChild = m_pLocaleData->GetElement("", "calendarSymbols");
    if (pChild) {
      CXML_Element* pSymbolNames =
          pChild->GetElement("", pstrSymbolNames.AsStringC());
      if (pSymbolNames) {
        if (pSymbolNames->GetAttrInteger("abbr") != bAbbr) {
          pSymbolNames = pChild->GetElement("", pstrSymbolNames.AsStringC(), 1);
        }
        if (pSymbolNames && pSymbolNames->GetAttrInteger("abbr") == bAbbr) {
          CXML_Element* pSymbolName =
              pSymbolNames->GetElement("", symbol, index);
          if (pSymbolName) {
            wsSymbolName = pSymbolName->GetContent(0);
          }
        }
      }
    }
  }
  return wsSymbolName;
}
void CXFA_XMLLocale::GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY eType,
                                    CFX_WideString& wsPattern) const {
  CXML_Element* pElement = m_pLocaleData->GetElement("", "datePatterns");
  if (!pElement) {
    return;
  }
  CFX_WideString wsName;
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsName = L"short";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
      wsName = L"med";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsName = L"full";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsName = L"long";
      break;
  }
  GetPattern(pElement, "datePattern", wsName.AsStringC(), wsPattern);
}
void CXFA_XMLLocale::GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY eType,
                                    CFX_WideString& wsPattern) const {
  CXML_Element* pElement = m_pLocaleData->GetElement("", "timePatterns");
  if (!pElement) {
    return;
  }
  CFX_WideString wsName;
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsName = L"short";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
      wsName = L"med";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsName = L"full";
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsName = L"long";
      break;
  }
  GetPattern(pElement, "timePattern", wsName.AsStringC(), wsPattern);
}
void CXFA_XMLLocale::GetNumPattern(FX_LOCALENUMSUBCATEGORY eType,
                                   CFX_WideString& wsPattern) const {
  CXML_Element* pElement = m_pLocaleData->GetElement("", "numberPatterns");
  if (!pElement) {
    return;
  }
  switch (eType) {
    case FX_LOCALENUMPATTERN_Percent:
      wsPattern = g_FX_Percent;
      break;
    case FX_LOCALENUMPATTERN_Currency:
      wsPattern = g_FX_Currency;
      break;
    case FX_LOCALENUMPATTERN_Decimal:
      wsPattern = g_FX_Decimal;
      break;
    case FX_LOCALENUMPATTERN_Integer:
      wsPattern = g_FX_Integer;
      break;
  }
}
void CXFA_XMLLocale::GetPattern(CXML_Element* pElement,
                                const CFX_ByteStringC& bsTag,
                                const CFX_WideStringC& wsName,
                                CFX_WideString& wsPattern) const {
  int32_t iCount = pElement->CountElements("", bsTag);
  for (int32_t i = 0; i < iCount; i++) {
    CXML_Element* pChild = pElement->GetElement("", bsTag, i);
    if (pChild->GetAttrValue("name") == wsName) {
      wsPattern = pChild->GetContent(0);
      return;
    }
  }
}
CXFA_NodeLocale::CXFA_NodeLocale(CXFA_Node* pLocale) {
  m_pLocale = pLocale;
}
CXFA_NodeLocale::~CXFA_NodeLocale() {}
void CXFA_NodeLocale::Release() {
  delete this;
}
CFX_WideString CXFA_NodeLocale::GetName() {
  return CFX_WideString(m_pLocale ? m_pLocale->GetCData(XFA_ATTRIBUTE_Name)
                                  : nullptr);
}
void CXFA_NodeLocale::GetNumbericSymbol(FX_LOCALENUMSYMBOL eType,
                                        CFX_WideString& wsNumSymbol) const {
  switch (eType) {
    case FX_LOCALENUMSYMBOL_Decimal:
      wsNumSymbol = GetSymbol(XFA_Element::NumberSymbols, FX_WSTRC(L"decimal"));
      break;
    case FX_LOCALENUMSYMBOL_Grouping:
      wsNumSymbol =
          GetSymbol(XFA_Element::NumberSymbols, FX_WSTRC(L"grouping"));
      break;
    case FX_LOCALENUMSYMBOL_Percent:
      wsNumSymbol = GetSymbol(XFA_Element::NumberSymbols, FX_WSTRC(L"percent"));
      break;
    case FX_LOCALENUMSYMBOL_Minus:
      wsNumSymbol = GetSymbol(XFA_Element::NumberSymbols, FX_WSTRC(L"minus"));
      break;
    case FX_LOCALENUMSYMBOL_Zero:
      wsNumSymbol = GetSymbol(XFA_Element::NumberSymbols, FX_WSTRC(L"zero"));
      break;
    case FX_LOCALENUMSYMBOL_CurrencySymbol:
      wsNumSymbol =
          GetSymbol(XFA_Element::CurrencySymbols, FX_WSTRC(L"symbol"));
      break;
    case FX_LOCALENUMSYMBOL_CurrencyName:
      wsNumSymbol =
          GetSymbol(XFA_Element::CurrencySymbols, FX_WSTRC(L"isoname"));
      break;
  }
}
void CXFA_NodeLocale::GetDateTimeSymbols(CFX_WideString& wsDtSymbol) const {
  CXFA_Node* pSymbols =
      m_pLocale ? m_pLocale->GetChild(0, XFA_Element::DateTimeSymbols)
                : nullptr;
  wsDtSymbol = pSymbols ? pSymbols->GetContent() : CFX_WideString();
}
void CXFA_NodeLocale::GetMonthName(int32_t nMonth,
                                   CFX_WideString& wsMonthName,
                                   FX_BOOL bAbbr) const {
  wsMonthName = GetCalendarSymbol(XFA_Element::MonthNames, nMonth, bAbbr);
}
void CXFA_NodeLocale::GetDayName(int32_t nWeek,
                                 CFX_WideString& wsDayName,
                                 FX_BOOL bAbbr) const {
  wsDayName = GetCalendarSymbol(XFA_Element::DayNames, nWeek, bAbbr);
}
void CXFA_NodeLocale::GetMeridiemName(CFX_WideString& wsMeridiemName,
                                      FX_BOOL bAM) const {
  wsMeridiemName =
      GetCalendarSymbol(XFA_Element::MeridiemNames, bAM ? 0 : 1, FALSE);
}
void CXFA_NodeLocale::GetTimeZone(FX_TIMEZONE& tz) const {
  CXFA_TimeZoneProvider::Get()->GetTimeZone(tz);
}
void CXFA_NodeLocale::GetEraName(CFX_WideString& wsEraName, FX_BOOL bAD) const {
  wsEraName = GetCalendarSymbol(XFA_Element::EraNames, bAD ? 1 : 0, FALSE);
}
void CXFA_NodeLocale::GetDatePattern(FX_LOCALEDATETIMESUBCATEGORY eType,
                                     CFX_WideString& wsPattern) const {
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsPattern = GetSymbol(XFA_Element::DatePatterns, FX_WSTRC(L"short"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
      wsPattern = GetSymbol(XFA_Element::DatePatterns, FX_WSTRC(L"med"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsPattern = GetSymbol(XFA_Element::DatePatterns, FX_WSTRC(L"full"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsPattern = GetSymbol(XFA_Element::DatePatterns, FX_WSTRC(L"long"));
      break;
  }
}
void CXFA_NodeLocale::GetTimePattern(FX_LOCALEDATETIMESUBCATEGORY eType,
                                     CFX_WideString& wsPattern) const {
  switch (eType) {
    case FX_LOCALEDATETIMESUBCATEGORY_Short:
      wsPattern = GetSymbol(XFA_Element::TimePatterns, FX_WSTRC(L"short"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Medium:
    case FX_LOCALEDATETIMESUBCATEGORY_Default:
      wsPattern = GetSymbol(XFA_Element::TimePatterns, FX_WSTRC(L"med"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Full:
      wsPattern = GetSymbol(XFA_Element::TimePatterns, FX_WSTRC(L"full"));
      break;
    case FX_LOCALEDATETIMESUBCATEGORY_Long:
      wsPattern = GetSymbol(XFA_Element::TimePatterns, FX_WSTRC(L"long"));
      break;
  }
}
void CXFA_NodeLocale::GetNumPattern(FX_LOCALENUMSUBCATEGORY eType,
                                    CFX_WideString& wsPattern) const {
  switch (eType) {
    case FX_LOCALENUMPATTERN_Percent:
      wsPattern = g_FX_Percent;
      break;
    case FX_LOCALENUMPATTERN_Currency:
      wsPattern = g_FX_Currency;
      break;
    case FX_LOCALENUMPATTERN_Decimal:
      wsPattern = g_FX_Decimal;
      break;
    case FX_LOCALENUMPATTERN_Integer:
      wsPattern = g_FX_Integer;
      break;
  }
}
CXFA_Node* CXFA_NodeLocale::GetNodeByName(CXFA_Node* pParent,
                                          const CFX_WideStringC& wsName) const {
  CXFA_Node* pChild =
      pParent ? pParent->GetNodeItem(XFA_NODEITEM_FirstChild) : nullptr;
  while (pChild) {
    CFX_WideString wsChild;
    if (pChild->GetAttribute(XFA_ATTRIBUTE_Name, wsChild)) {
      if (wsChild == wsName) {
        return pChild;
      }
    }
    pChild = pChild->GetNodeItem(XFA_NODEITEM_NextSibling);
  }
  return nullptr;
}
CFX_WideString CXFA_NodeLocale::GetSymbol(
    XFA_Element eElement,
    const CFX_WideStringC& symbol_type) const {
  CXFA_Node* pSymbols = m_pLocale ? m_pLocale->GetChild(0, eElement) : nullptr;
  CXFA_Node* pSymbol = GetNodeByName(pSymbols, symbol_type);
  return pSymbol ? pSymbol->GetContent() : CFX_WideString();
}
CFX_WideString CXFA_NodeLocale::GetCalendarSymbol(XFA_Element eElement,
                                                  int index,
                                                  FX_BOOL bAbbr) const {
  CXFA_Node* pCalendar =
      m_pLocale ? m_pLocale->GetChild(0, XFA_Element::CalendarSymbols)
                : nullptr;
  if (pCalendar) {
    CXFA_Node* pNode = pCalendar->GetFirstChildByClass(eElement);
    for (; pNode; pNode = pNode->GetNextSameClassSibling(eElement)) {
      if (pNode->GetBoolean(XFA_ATTRIBUTE_Abbr) == bAbbr) {
        CXFA_Node* pSymbol = pNode->GetChild(index, XFA_Element::Unknown);
        return pSymbol ? pSymbol->GetContent() : CFX_WideString();
      }
    }
  }
  return CFX_WideString();
}
