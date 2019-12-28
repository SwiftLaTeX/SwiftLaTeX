// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/css/fde_cssstyleselector.h"

#include <algorithm>
#include <memory>

#include "xfa/fde/css/fde_csscache.h"
#include "xfa/fde/css/fde_cssdeclaration.h"
#include "xfa/fde/css/fde_cssstylesheet.h"
#include "xfa/fde/css/fde_csssyntax.h"
#include "xfa/fxfa/app/xfa_textlayout.h"

#define FDE_CSSUNIVERSALHASH ('*')

int32_t CFDE_CSSCounterStyle::FindIndex(const FX_WCHAR* pszIdentifier) {
  int32_t iCount = m_arrCounterData.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    if (FXSYS_wcscmp(pszIdentifier, m_arrCounterData.ElementAt(i).m_pszIdent) ==
        0) {
      return i;
    }
  }
  return -1;
}
void CFDE_CSSCounterStyle::DoUpdateIndex(IFDE_CSSValueList* pList) {
  if (!pList)
    return;

  int32_t iCount = pList->CountValues();
  FX_FLOAT fDefValue = 1.0;
  FX_BOOL bDefIncrement = TRUE;
  if (pList == m_pCounterReset) {
    fDefValue = 0.0;
    bDefIncrement = FALSE;
  }
  for (int32_t i = 0; i < iCount; i++) {
    IFDE_CSSValueList* pCounter =
        static_cast<IFDE_CSSValueList*>(pList->GetValue(i));
    int32_t iLen;
    const FX_WCHAR* pszIdentifier =
        static_cast<IFDE_CSSPrimitiveValue*>(pCounter->GetValue(0))
            ->GetString(iLen);
    FX_FLOAT fValue = fDefValue;
    if (pCounter->CountValues() > 1) {
      fValue = static_cast<IFDE_CSSPrimitiveValue*>(pCounter->GetValue(1))
                   ->GetFloat();
    }
    int32_t iIndex = FindIndex(pszIdentifier);
    if (iIndex == -1) {
      FDE_CSSCOUNTERDATA data;
      data.m_pszIdent = pszIdentifier;
      if (bDefIncrement) {
        data.m_bIncrement = TRUE;
        data.m_iIncVal = (int32_t)fValue;
      } else {
        data.m_iResetVal = (int32_t)fValue;
        data.m_bReset = TRUE;
      }
      m_arrCounterData.Add(data);
    } else {
      FDE_CSSCOUNTERDATA& data = m_arrCounterData.ElementAt(iIndex);
      if (bDefIncrement) {
        data.m_bIncrement = TRUE;
        data.m_iIncVal += (int32_t)fValue;
      } else {
        data.m_bReset = TRUE;
        data.m_iResetVal = (int32_t)fValue;
      }
    }
  }
}

CFDE_CSSCounterStyle::CFDE_CSSCounterStyle()
    : m_pCounterInc(nullptr), m_pCounterReset(nullptr), m_bIndexDirty(FALSE) {}

CFDE_CSSCounterStyle::~CFDE_CSSCounterStyle() {}

void CFDE_CSSCounterStyle::UpdateIndex() {
  if (!m_bIndexDirty) {
    return;
  }
  m_arrCounterData.RemoveAll();
  DoUpdateIndex(m_pCounterInc);
  DoUpdateIndex(m_pCounterReset);
  m_bIndexDirty = FALSE;
}

FDE_CSSRuleData::FDE_CSSRuleData(CFDE_CSSSelector* pSel,
                                 CFDE_CSSDeclaration* pDecl,
                                 uint32_t dwPos)
    : pSelector(pSel), pDeclaration(pDecl), dwPriority(dwPos), pNext(nullptr) {
  static const uint32_t s_Specific[5] = {0x00010000, 0x00010000, 0x00100000,
                                         0x00100000, 0x01000000};
  for (; pSel; pSel = pSel->GetNextSelector()) {
    FDE_CSSSELECTORTYPE eType = pSel->GetType();
    if (eType > FDE_CSSSELECTORTYPE_Descendant ||
        pSel->GetNameHash() != FDE_CSSUNIVERSALHASH) {
      dwPriority += s_Specific[eType];
    }
  }
}

void CFDE_CSSRuleCollection::Clear() {
  m_IDRules.RemoveAll();
  m_TagRules.RemoveAll();
  m_ClassRules.RemoveAll();
  m_pUniversalRules = nullptr;
  m_pStaticStore = nullptr;
  m_iSelectors = 0;
}

CFDE_CSSRuleCollection::CFDE_CSSRuleCollection()
    : m_pStaticStore(nullptr),
      m_pUniversalRules(nullptr),
      m_pPersudoRules(nullptr),
      m_iSelectors(0) {}

CFDE_CSSRuleCollection::~CFDE_CSSRuleCollection() {
  Clear();
}

void CFDE_CSSRuleCollection::AddRulesFrom(const CFDE_CSSStyleSheetArray& sheets,
                                          uint32_t dwMediaList,
                                          IFGAS_FontMgr* pFontMgr) {
  int32_t iSheets = sheets.GetSize();
  for (int32_t i = 0; i < iSheets; ++i) {
    IFDE_CSSStyleSheet* pSheet = sheets.GetAt(i);
    if (uint32_t dwMatchMedia = pSheet->GetMediaList() & dwMediaList) {
      int32_t iRules = pSheet->CountRules();
      for (int32_t j = 0; j < iRules; j++) {
        AddRulesFrom(pSheet, pSheet->GetRule(j), dwMatchMedia, pFontMgr);
      }
    }
  }
}

void CFDE_CSSRuleCollection::AddRulesFrom(IFDE_CSSStyleSheet* pStyleSheet,
                                          IFDE_CSSRule* pRule,
                                          uint32_t dwMediaList,
                                          IFGAS_FontMgr* pFontMgr) {
  switch (pRule->GetType()) {
    case FDE_CSSRULETYPE_Style: {
      IFDE_CSSStyleRule* pStyleRule = static_cast<IFDE_CSSStyleRule*>(pRule);
      CFDE_CSSDeclaration* pDeclaration = pStyleRule->GetDeclaration();
      int32_t iSelectors = pStyleRule->CountSelectorLists();
      for (int32_t i = 0; i < iSelectors; ++i) {
        CFDE_CSSSelector* pSelector = pStyleRule->GetSelectorList(i);
        if (pSelector->GetType() == FDE_CSSSELECTORTYPE_Persudo) {
          FDE_CSSRuleData* pData = NewRuleData(pSelector, pDeclaration);
          AddRuleTo(m_pPersudoRules, pData);
          continue;
        }
        if (pSelector->GetNameHash() != FDE_CSSUNIVERSALHASH) {
          AddRuleTo(m_TagRules, pSelector->GetNameHash(), pSelector,
                    pDeclaration);
          continue;
        }
        CFDE_CSSSelector* pNext = pSelector->GetNextSelector();
        if (!pNext) {
          FDE_CSSRuleData* pData = NewRuleData(pSelector, pDeclaration);
          AddRuleTo(m_pUniversalRules, pData);
          continue;
        }
        switch (pNext->GetType()) {
          case FDE_CSSSELECTORTYPE_ID:
            AddRuleTo(m_IDRules, pNext->GetNameHash(), pSelector, pDeclaration);
            break;
          case FDE_CSSSELECTORTYPE_Class:
            AddRuleTo(m_ClassRules, pNext->GetNameHash(), pSelector,
                      pDeclaration);
            break;
          case FDE_CSSSELECTORTYPE_Descendant:
          case FDE_CSSSELECTORTYPE_Element:
            AddRuleTo(m_pUniversalRules, NewRuleData(pSelector, pDeclaration));
            break;
          default:
            ASSERT(FALSE);
            break;
        }
      }
    } break;
    case FDE_CSSRULETYPE_Media: {
      IFDE_CSSMediaRule* pMediaRule = static_cast<IFDE_CSSMediaRule*>(pRule);
      if (pMediaRule->GetMediaList() & dwMediaList) {
        int32_t iRules = pMediaRule->CountRules();
        for (int32_t i = 0; i < iRules; ++i) {
          AddRulesFrom(pStyleSheet, pMediaRule->GetRule(i), dwMediaList,
                       pFontMgr);
        }
      }
    } break;
    default:
      break;
  }
}

void CFDE_CSSRuleCollection::AddRuleTo(CFX_MapPtrToPtr& map,
                                       uint32_t dwKey,
                                       CFDE_CSSSelector* pSel,
                                       CFDE_CSSDeclaration* pDecl) {
  void* pKey = (void*)(uintptr_t)dwKey;
  FDE_CSSRuleData* pData = NewRuleData(pSel, pDecl);
  FDE_CSSRuleData* pList = nullptr;
  if (!map.Lookup(pKey, (void*&)pList)) {
    map.SetAt(pKey, pData);
  } else if (AddRuleTo(pList, pData)) {
    map.SetAt(pKey, pList);
  }
}

FX_BOOL CFDE_CSSRuleCollection::AddRuleTo(FDE_CSSRuleData*& pList,
                                          FDE_CSSRuleData* pData) {
  if (pList) {
    pData->pNext = pList->pNext;
    pList->pNext = pData;
    return FALSE;
  }

  pList = pData;
  return TRUE;
}

FDE_CSSRuleData* CFDE_CSSRuleCollection::NewRuleData(
    CFDE_CSSSelector* pSel,
    CFDE_CSSDeclaration* pDecl) {
  return FXTARGET_NewWith(m_pStaticStore)
      FDE_CSSRuleData(pSel, pDecl, ++m_iSelectors);
}

CFDE_CSSStyleSelector::CFDE_CSSStyleSelector()
    : m_pFontMgr(nullptr),
      m_fDefFontSize(12.0f),
      m_pRuleDataStore(nullptr),
      m_pInlineStyleStore(nullptr),
      m_pFixedStyleStore(nullptr),
      m_pAccelerator(nullptr) {
  m_ePriorities[FDE_CSSSTYLESHEETPRIORITY_High] = FDE_CSSSTYLESHEETGROUP_Author;
  m_ePriorities[FDE_CSSSTYLESHEETPRIORITY_Mid] = FDE_CSSSTYLESHEETGROUP_User;
  m_ePriorities[FDE_CSSSTYLESHEETPRIORITY_Low] =
      FDE_CSSSTYLESHEETGROUP_UserAgent;
}

CFDE_CSSStyleSelector::~CFDE_CSSStyleSelector() {
  Reset();
  delete m_pInlineStyleStore;
  delete m_pFixedStyleStore;
  delete m_pAccelerator;
}

void CFDE_CSSStyleSelector::SetFontMgr(IFGAS_FontMgr* pFontMgr) {
  m_pFontMgr = pFontMgr;
}
void CFDE_CSSStyleSelector::SetDefFontSize(FX_FLOAT fFontSize) {
  ASSERT(fFontSize > 0);
  m_fDefFontSize = fFontSize;
}

CFDE_CSSAccelerator* CFDE_CSSStyleSelector::InitAccelerator() {
  if (!m_pAccelerator)
    m_pAccelerator = new CFDE_CSSAccelerator;
  m_pAccelerator->Clear();
  return m_pAccelerator;
}

IFDE_CSSComputedStyle* CFDE_CSSStyleSelector::CreateComputedStyle(
    IFDE_CSSComputedStyle* pParentStyle) {
  if (!m_pFixedStyleStore) {
    m_pFixedStyleStore = IFX_MemoryAllocator::Create(
        FX_ALLOCTYPE_Fixed, 16, sizeof(CFDE_CSSComputedStyle));
  }
  CFDE_CSSComputedStyle* pStyle = FXTARGET_NewWith(m_pFixedStyleStore)
      CFDE_CSSComputedStyle(m_pFixedStyleStore);
  if (pParentStyle) {
    pStyle->m_InheritedData =
        static_cast<CFDE_CSSComputedStyle*>(pParentStyle)->m_InheritedData;
  } else {
    pStyle->m_InheritedData.Reset();
  }
  pStyle->m_NonInheritedData.Reset();
  return pStyle;
}
FX_BOOL CFDE_CSSStyleSelector::SetStyleSheet(FDE_CSSSTYLESHEETGROUP eType,
                                             IFDE_CSSStyleSheet* pSheet) {
  ASSERT(eType < FDE_CSSSTYLESHEETGROUP_MAX);
  CFDE_CSSStyleSheetArray& dest = m_SheetGroups[eType];
  dest.RemoveAt(0, dest.GetSize());
  if (pSheet)
    dest.Add(pSheet);
  return TRUE;
}
FX_BOOL CFDE_CSSStyleSelector::SetStyleSheets(
    FDE_CSSSTYLESHEETGROUP eType,
    const CFDE_CSSStyleSheetArray* pArray) {
  ASSERT(eType < FDE_CSSSTYLESHEETGROUP_MAX);
  CFDE_CSSStyleSheetArray& dest = m_SheetGroups[eType];
  if (pArray)
    dest.Copy(*pArray);
  else
    dest.RemoveAt(0, dest.GetSize());
  return TRUE;
}
void CFDE_CSSStyleSelector::SetStylePriority(
    FDE_CSSSTYLESHEETGROUP eType,
    FDE_CSSSTYLESHEETPRIORITY ePriority) {
  m_ePriorities[ePriority] = eType;
}
void CFDE_CSSStyleSelector::UpdateStyleIndex(uint32_t dwMediaList) {
  Reset();
  m_pRuleDataStore = IFX_MemoryAllocator::Create(FX_ALLOCTYPE_Static, 1024, 0);
  for (int32_t iGroup = 0; iGroup < FDE_CSSSTYLESHEETGROUP_MAX; ++iGroup) {
    CFDE_CSSRuleCollection& rules = m_RuleCollection[iGroup];
    rules.m_pStaticStore = m_pRuleDataStore;
    rules.AddRulesFrom(m_SheetGroups[iGroup], dwMediaList, m_pFontMgr);
  }
}
void CFDE_CSSStyleSelector::Reset() {
  for (int32_t iGroup = 0; iGroup < FDE_CSSSTYLESHEETGROUP_MAX; ++iGroup) {
    m_RuleCollection[iGroup].Clear();
  }
  delete m_pRuleDataStore;
  m_pRuleDataStore = nullptr;
}
int32_t CFDE_CSSStyleSelector::MatchDeclarations(
    CXFA_CSSTagProvider* pTag,
    CFDE_CSSDeclarationArray& matchedDecls,
    FDE_CSSPERSUDO ePersudoType) {
  ASSERT(m_pAccelerator && pTag);

  FDE_CSSTagCache* pCache = m_pAccelerator->GetTopElement();
  ASSERT(pCache && pCache->GetTag() == pTag);

  matchedDecls.RemoveAt(0, matchedDecls.GetSize());
  for (int32_t ePriority = FDE_CSSSTYLESHEETPRIORITY_MAX - 1; ePriority >= 0;
       --ePriority) {
    FDE_CSSSTYLESHEETGROUP eGroup = m_ePriorities[ePriority];
    CFDE_CSSRuleCollection& rules = m_RuleCollection[eGroup];
    if (rules.CountSelectors() == 0)
      continue;

    if (ePersudoType == FDE_CSSPERSUDO_NONE) {
      MatchRules(pCache, rules.GetUniversalRuleData(), ePersudoType);
      if (pCache->HashTag()) {
        MatchRules(pCache, rules.GetTagRuleData(pCache->HashTag()),
                   ePersudoType);
      }
      int32_t iCount = pCache->CountHashClass();
      for (int32_t i = 0; i < iCount; i++) {
        pCache->SetClassIndex(i);
        MatchRules(pCache, rules.GetClassRuleData(pCache->HashClass()),
                   ePersudoType);
      }
    } else {
      MatchRules(pCache, rules.GetPersudoRuleData(), ePersudoType);
    }

    std::sort(m_MatchedRules.begin(), m_MatchedRules.end(),
              [](const FDE_CSSRuleData* p1, const FDE_CSSRuleData* p2) {
                return p1->dwPriority < p2->dwPriority;
              });
    for (const auto& rule : m_MatchedRules)
      matchedDecls.Add(rule->pDeclaration);
    m_MatchedRules.clear();
  }
  return matchedDecls.GetSize();
}

void CFDE_CSSStyleSelector::MatchRules(FDE_CSSTagCache* pCache,
                                       FDE_CSSRuleData* pList,
                                       FDE_CSSPERSUDO ePersudoType) {
  while (pList) {
    if (MatchSelector(pCache, pList->pSelector, ePersudoType))
      m_MatchedRules.push_back(pList);
    pList = pList->pNext;
  }
}

FX_BOOL CFDE_CSSStyleSelector::MatchSelector(FDE_CSSTagCache* pCache,
                                             CFDE_CSSSelector* pSel,
                                             FDE_CSSPERSUDO ePersudoType) {
  uint32_t dwHash;
  while (pSel && pCache) {
    switch (pSel->GetType()) {
      case FDE_CSSSELECTORTYPE_Descendant:
        dwHash = pSel->GetNameHash();
        while ((pCache = pCache->GetParent()) != nullptr) {
          if (dwHash != FDE_CSSUNIVERSALHASH && dwHash != pCache->HashTag()) {
            continue;
          }
          if (MatchSelector(pCache, pSel->GetNextSelector(), ePersudoType)) {
            return TRUE;
          }
        }
        return FALSE;
      case FDE_CSSSELECTORTYPE_ID:
        dwHash = pCache->HashID();
        if (dwHash != pSel->GetNameHash()) {
          return FALSE;
        }
        break;
      case FDE_CSSSELECTORTYPE_Class:
        dwHash = pCache->HashClass();
        if (dwHash != pSel->GetNameHash()) {
          return FALSE;
        }
        break;
      case FDE_CSSSELECTORTYPE_Element:
        dwHash = pSel->GetNameHash();
        if (dwHash != FDE_CSSUNIVERSALHASH && dwHash != pCache->HashTag()) {
          return FALSE;
        }
        break;
      case FDE_CSSSELECTORTYPE_Persudo:
        dwHash = FDE_GetCSSPersudoByEnum(ePersudoType)->dwHash;
        if (dwHash != pSel->GetNameHash()) {
          return FALSE;
        }
        break;
      default:
        ASSERT(FALSE);
        break;
    }
    pSel = pSel->GetNextSelector();
  }
  return !pSel && pCache;
}

void CFDE_CSSStyleSelector::ComputeStyle(
    CXFA_CSSTagProvider* pTag,
    const CFDE_CSSDeclaration** ppDeclArray,
    int32_t iDeclCount,
    IFDE_CSSComputedStyle* pDestStyle) {
  ASSERT(iDeclCount >= 0);
  ASSERT(pDestStyle);

  static const uint32_t s_dwStyleHash = FX_HashCode_GetW(L"style", true);
  static const uint32_t s_dwAlignHash = FX_HashCode_GetW(L"align", true);

  if (!pTag->empty()) {
    if (!m_pInlineStyleStore) {
      m_pInlineStyleStore =
          IFX_MemoryAllocator::Create(FX_ALLOCTYPE_Static, 2048, 0);
    }
    CFDE_CSSDeclaration* pDecl = nullptr;
    for (auto it : *pTag) {
      CFX_WideString wsAttri = it.first;
      CFX_WideString wsValue = it.second;
      uint32_t dwAttriHash = FX_HashCode_GetW(wsAttri.AsStringC(), true);
      if (dwAttriHash == s_dwStyleHash) {
        if (!pDecl)
          pDecl = FXTARGET_NewWith(m_pInlineStyleStore) CFDE_CSSDeclaration;

        AppendInlineStyle(pDecl, wsValue.c_str(), wsValue.GetLength());
      } else if (dwAttriHash == s_dwAlignHash) {
        if (!pDecl)
          pDecl = FXTARGET_NewWith(m_pInlineStyleStore) CFDE_CSSDeclaration;

        FDE_CSSPROPERTYARGS args;
        args.pStringCache = nullptr;
        args.pStaticStore = m_pInlineStyleStore;
        args.pProperty = FDE_GetCSSPropertyByEnum(FDE_CSSPROPERTY_TextAlign);
        pDecl->AddProperty(&args, wsValue.c_str(), wsValue.GetLength());
      }
    }

    if (pDecl) {
      CFDE_CSSDeclarationArray decls;
      decls.SetSize(iDeclCount + 1);
      CFDE_CSSDeclaration** ppInline = decls.GetData();
      FXSYS_memcpy(ppInline, ppDeclArray,
                   iDeclCount * sizeof(CFDE_CSSDeclaration*));
      ppInline[iDeclCount++] = pDecl;
      ApplyDeclarations(TRUE, const_cast<const CFDE_CSSDeclaration**>(ppInline),
                        iDeclCount, pDestStyle);
      ApplyDeclarations(FALSE,
                        const_cast<const CFDE_CSSDeclaration**>(ppInline),
                        iDeclCount, pDestStyle);
      return;
    }
  }

  if (iDeclCount > 0) {
    ASSERT(ppDeclArray);

    ApplyDeclarations(TRUE, ppDeclArray, iDeclCount, pDestStyle);
    ApplyDeclarations(FALSE, ppDeclArray, iDeclCount, pDestStyle);
  }
}

void CFDE_CSSStyleSelector::ApplyDeclarations(
    FX_BOOL bPriority,
    const CFDE_CSSDeclaration** ppDeclArray,
    int32_t iDeclCount,
    IFDE_CSSComputedStyle* pDestStyle) {
  CFDE_CSSComputedStyle* pComputedStyle =
      static_cast<CFDE_CSSComputedStyle*>(pDestStyle);
  IFDE_CSSValue* pVal;
  FX_BOOL bImportant;
  int32_t i;
  if (bPriority) {
    IFDE_CSSValue* pLastest = nullptr;
    IFDE_CSSValue* pImportant = nullptr;
    for (i = 0; i < iDeclCount; ++i) {
      pVal = ppDeclArray[i]->GetProperty(FDE_CSSPROPERTY_FontSize, bImportant);
      if (!pVal)
        continue;

      if (bImportant)
        pImportant = pVal;
      else
        pLastest = pVal;
    }
    if (pImportant) {
      ApplyProperty(FDE_CSSPROPERTY_FontSize, pImportant, pComputedStyle);
    } else if (pLastest) {
      ApplyProperty(FDE_CSSPROPERTY_FontSize, pLastest, pComputedStyle);
    }
  } else {
    CFDE_CSSDeclarationArray importants;
    const CFDE_CSSDeclaration* pDecl = nullptr;
    FDE_CSSPROPERTY eProp;
    FX_POSITION pos;
    for (i = 0; i < iDeclCount; ++i) {
      pDecl = ppDeclArray[i];
      pos = pDecl->GetStartPosition();
      while (pos) {
        pDecl->GetNextProperty(pos, eProp, pVal, bImportant);
        if (eProp == FDE_CSSPROPERTY_FontSize) {
          continue;
        } else if (!bImportant) {
          ApplyProperty(eProp, pVal, pComputedStyle);
        } else if (importants.GetSize() == 0 ||
                   importants[importants.GetUpperBound()] != pDecl) {
          importants.Add(const_cast<CFDE_CSSDeclaration*>(pDecl));
        }
      }
    }
    iDeclCount = importants.GetSize();
    for (i = 0; i < iDeclCount; ++i) {
      pDecl = importants[i];
      pos = pDecl->GetStartPosition();
      while (pos) {
        pDecl->GetNextProperty(pos, eProp, pVal, bImportant);
        if (bImportant && eProp != FDE_CSSPROPERTY_FontSize) {
          ApplyProperty(eProp, pVal, pComputedStyle);
        }
      }
    }
    CFX_WideString wsName, wsValue;
    pos = pDecl->GetStartCustom();
    while (pos) {
      pDecl->GetNextCustom(pos, wsName, wsValue);
      pComputedStyle->AddCustomStyle(wsName, wsValue);
    }
  }
}
void CFDE_CSSStyleSelector::AppendInlineStyle(CFDE_CSSDeclaration* pDecl,
                                              const FX_WCHAR* psz,
                                              int32_t iLen) {
  ASSERT(pDecl && psz && iLen > 0);
  std::unique_ptr<CFDE_CSSSyntaxParser> pSyntax(new CFDE_CSSSyntaxParser);
  if (!pSyntax->Init(psz, iLen, 32, TRUE))
    return;

  int32_t iLen2 = 0;
  const FX_WCHAR* psz2;
  FDE_CSSPROPERTYARGS args;
  args.pStringCache = nullptr;
  args.pStaticStore = m_pInlineStyleStore;
  args.pProperty = nullptr;
  CFX_WideString wsName;
  while (1) {
    FDE_CSSSYNTAXSTATUS eStatus = pSyntax->DoSyntaxParse();
    if (eStatus == FDE_CSSSYNTAXSTATUS_PropertyName) {
      psz2 = pSyntax->GetCurrentString(iLen2);
      args.pProperty = FDE_GetCSSPropertyByName(CFX_WideStringC(psz2, iLen2));
      if (!args.pProperty)
        wsName = CFX_WideStringC(psz2, iLen2);
    } else if (eStatus == FDE_CSSSYNTAXSTATUS_PropertyValue) {
      if (args.pProperty) {
        psz2 = pSyntax->GetCurrentString(iLen2);
        if (iLen2 > 0)
          pDecl->AddProperty(&args, psz2, iLen2);
      } else if (iLen2 > 0) {
        psz2 = pSyntax->GetCurrentString(iLen2);
        if (iLen2 > 0) {
          pDecl->AddProperty(&args, wsName.c_str(), wsName.GetLength(), psz2,
                             iLen2);
        }
      }
    } else {
      break;
    }
  }
}

#define FDE_CSSNONINHERITS (pComputedStyle->m_NonInheritedData)
#define FDE_CSSINHERITS (pComputedStyle->m_InheritedData)
#define FDE_CSSFONTSIZE (FDE_CSSINHERITS.m_fFontSize)
void CFDE_CSSStyleSelector::ApplyProperty(
    FDE_CSSPROPERTY eProperty,
    IFDE_CSSValue* pValue,
    CFDE_CSSComputedStyle* pComputedStyle) {
  if (pValue->GetType() == FDE_CSSVALUETYPE_Primitive) {
    IFDE_CSSPrimitiveValue* pPrimitive =
        static_cast<IFDE_CSSPrimitiveValue*>(pValue);
    FDE_CSSPRIMITIVETYPE eType = pPrimitive->GetPrimitiveType();
    switch (eProperty) {
      case FDE_CSSPROPERTY_Display:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eDisplay = ToDisplay(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_FontSize: {
        FX_FLOAT& fFontSize = FDE_CSSFONTSIZE;
        if (eType >= FDE_CSSPRIMITIVETYPE_Number &&
            eType <= FDE_CSSPRIMITIVETYPE_PC) {
          fFontSize = ApplyNumber(eType, pPrimitive->GetFloat(), fFontSize);
        } else if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          fFontSize = ToFontSize(pPrimitive->GetEnum(), fFontSize);
        }
      } break;
      case FDE_CSSPROPERTY_LineHeight:
        if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          FDE_CSSINHERITS.m_fLineHeight =
              pPrimitive->GetFloat() * FDE_CSSFONTSIZE;
        } else if (eType > FDE_CSSPRIMITIVETYPE_Number &&
                   eType <= FDE_CSSPRIMITIVETYPE_PC) {
          FDE_CSSINHERITS.m_fLineHeight =
              ApplyNumber(eType, pPrimitive->GetFloat(), FDE_CSSFONTSIZE);
        }
        break;
      case FDE_CSSPROPERTY_TextAlign:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eTextAligh = ToTextAlign(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_TextIndent:
        SetLengthWithPercent(FDE_CSSINHERITS.m_TextIndent, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_FontWeight:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_wFontWeight = ToFontWeight(pPrimitive->GetEnum());
        } else if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          int32_t iValue = (int32_t)pPrimitive->GetFloat() / 100;
          if (iValue >= 1 && iValue <= 9) {
            FDE_CSSINHERITS.m_wFontWeight = iValue * 100;
          }
        }
        break;
      case FDE_CSSPROPERTY_FontStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eFontStyle = ToFontStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_Color:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSINHERITS.m_dwFontColor = pPrimitive->GetRGBColor();
        }
        break;
      case FDE_CSSPROPERTY_MarginLeft:
        FDE_CSSNONINHERITS.m_bHasMargin |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_MarginWidth.left, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MarginTop:
        FDE_CSSNONINHERITS.m_bHasMargin |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_MarginWidth.top, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MarginRight:
        FDE_CSSNONINHERITS.m_bHasMargin |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_MarginWidth.right, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MarginBottom:
        FDE_CSSNONINHERITS.m_bHasMargin |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_MarginWidth.bottom, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_PaddingLeft:
        FDE_CSSNONINHERITS.m_bHasPadding |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_PaddingWidth.left, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_PaddingTop:
        FDE_CSSNONINHERITS.m_bHasPadding |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_PaddingWidth.top, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_PaddingRight:
        FDE_CSSNONINHERITS.m_bHasPadding |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_PaddingWidth.right, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_PaddingBottom:
        FDE_CSSNONINHERITS.m_bHasPadding |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_PaddingWidth.bottom,
                                 eType, pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BorderLeftWidth:
        FDE_CSSNONINHERITS.m_bHasBorder |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_BorderWidth.left, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BorderTopWidth:
        FDE_CSSNONINHERITS.m_bHasBorder |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_BorderWidth.top, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BorderRightWidth:
        FDE_CSSNONINHERITS.m_bHasBorder |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_BorderWidth.right, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BorderBottomWidth:
        FDE_CSSNONINHERITS.m_bHasBorder |=
            SetLengthWithPercent(FDE_CSSNONINHERITS.m_BorderWidth.bottom, eType,
                                 pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BorderLeftStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBDRLeftStyle =
              ToBorderStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BorderTopStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBDRTopStyle =
              ToBorderStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BorderRightStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBDRRightStyle =
              ToBorderStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BorderBottomStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBDRBottomStyle =
              ToBorderStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BorderLeftColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwBDRLeftColor = pPrimitive->GetRGBColor();
        }
        break;
      case FDE_CSSPROPERTY_BorderTopColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwBDRTopColor = pPrimitive->GetRGBColor();
        }
        break;
      case FDE_CSSPROPERTY_BorderRightColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwBDRRightColor = pPrimitive->GetRGBColor();
        }
        break;
      case FDE_CSSPROPERTY_BorderBottomColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwBDRBottomColor = pPrimitive->GetRGBColor();
        }
        break;
      case FDE_CSSPROPERTY_ListStyleType:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eListStyleType =
              ToListStyleType(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_ListStylePosition:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eListStylePosition =
              ToListStylePosition(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BackgroundColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwBKGColor = pPrimitive->GetRGBColor();
        } else if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_dwBKGColor = 0;
        }
        break;
      case FDE_CSSPROPERTY_Visibility:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eVisibility = ToVisibility(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_Width:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_BoxSize.cx, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_Height:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_BoxSize.cy, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MinWidth:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_MinBoxSize.cx, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MinHeight:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_MinBoxSize.cy, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MaxWidth:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_MaxBoxSize.cx, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_MaxHeight:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_MaxBoxSize.cy, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_VerticalAlign:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eVerticalAlign =
              ToVerticalAlign(pPrimitive->GetEnum());
        } else if (eType >= FDE_CSSPRIMITIVETYPE_Number &&
                   eType <= FDE_CSSPRIMITIVETYPE_PC) {
          FDE_CSSNONINHERITS.m_eVerticalAlign = FDE_CSSVERTICALALIGN_Number;
          FDE_CSSNONINHERITS.m_fVerticalAlign =
              ApplyNumber(eType, pPrimitive->GetFloat(), FDE_CSSFONTSIZE);
        }
        break;
      case FDE_CSSPROPERTY_WhiteSpace:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eWhiteSpace = ToWhiteSpace(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_TextTransform:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eTextTransform =
              ToTextTransform(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_FontVariant:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eFontVariant = ToFontVariant(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_LetterSpacing:
        if (eType == FDE_CSSPRIMITIVETYPE_Percent) {
          break;
        } else if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_LetterSpacing.Set(FDE_CSSLENGTHUNIT_Normal);
        } else if (eType >= FDE_CSSPRIMITIVETYPE_Number &&
                   eType <= FDE_CSSPRIMITIVETYPE_PC) {
          SetLengthWithPercent(FDE_CSSINHERITS.m_LetterSpacing, eType,
                               pPrimitive, FDE_CSSFONTSIZE);
        }
        break;
      case FDE_CSSPROPERTY_WordSpacing:
        if (eType == FDE_CSSPRIMITIVETYPE_Percent) {
          break;
        } else if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_WordSpacing.Set(FDE_CSSLENGTHUNIT_Normal);
        } else if (eType >= FDE_CSSPRIMITIVETYPE_Number &&
                   eType <= FDE_CSSPRIMITIVETYPE_PC) {
          SetLengthWithPercent(FDE_CSSINHERITS.m_WordSpacing, eType, pPrimitive,
                               FDE_CSSFONTSIZE);
        }
        break;
      case FDE_CSSPROPERTY_Float:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eFloat = ToFloat(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_Clear:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eClear = ToClear(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_WritingMode:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eWritingMode = ToWritingMode(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_WordBreak:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eWordBreak = ToWordBreak(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_Widows:
        if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          FDE_CSSINHERITS.m_iWidows = (int32_t)pPrimitive->GetFloat();
        }
        break;
      case FDE_CSSPROPERTY_Orphans:
        if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          FDE_CSSINHERITS.m_iOrphans = (int32_t)pPrimitive->GetFloat();
        }
        break;
      case FDE_CSSPROPERTY_TextEmphasisColor:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          switch (pPrimitive->GetEnum()) {
            case FDE_CSSPROPERTYVALUE_Transparent:
              FDE_CSSINHERITS.m_dwTextEmphasisColor = 0;
              FDE_CSSINHERITS.m_bTextEmphasisColorCurrent = FALSE;
              break;
            case FDE_CSSPROPERTYVALUE_Currentcolor:
              FDE_CSSINHERITS.m_bTextEmphasisColorCurrent = TRUE;
              break;
            default:
              break;
          }
        } else if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSINHERITS.m_dwTextEmphasisColor = pPrimitive->GetRGBColor();
          FDE_CSSINHERITS.m_bTextEmphasisColorCurrent = FALSE;
        }
        break;
      case FDE_CSSPROPERTY_PageBreakBefore:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_ePageBreakBefore =
              ToPageBreak(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_PageBreakAfter:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_ePageBreakAfter =
              ToPageBreak(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_PageBreakInside:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_ePageBreakInside =
              ToPageBreak(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_OverflowX:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eOverflowX = ToOverflow(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_OverflowY:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eOverflowY = ToOverflow(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_LineBreak:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eLineBreak = ToLineBreak(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_ColumnCount:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_ColumnCount.Set(FDE_CSSLENGTHUNIT_Auto);
        } else if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          FDE_CSSNONINHERITS.m_ColumnCount.Set(FDE_CSSLENGTHUNIT_Point,
                                               pPrimitive->GetFloat());
        }
        break;
      case FDE_CSSPROPERTY_ColumnGap:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_ColumnGap, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_ColumnRuleColor:
        if (eType == FDE_CSSPRIMITIVETYPE_RGB) {
          FDE_CSSNONINHERITS.m_dwColumnRuleColor = pPrimitive->GetRGBColor();
          FDE_CSSNONINHERITS.m_bColumnRuleColorSame = FALSE;
        }
        break;
      case FDE_CSSPROPERTY_ColumnRuleStyle:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eColumnRuleStyle =
              ToBorderStyle(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_ColumnRuleWidth:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_ColumnRuleWidth, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_ColumnWidth:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_ColumnWidth, eType,
                             pPrimitive, FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_BackgroundImage:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_pszBKGImage = nullptr;
        } else if (eType == FDE_CSSPRIMITIVETYPE_URI) {
          int32_t iLength;
          FDE_CSSNONINHERITS.m_pszBKGImage = pPrimitive->GetString(iLength);
        }
        break;
      case FDE_CSSPROPERTY_Position:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_ePosition = ToPosition(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_Top:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_Top, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_Bottom:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_Bottom, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_Left:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_Left, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_Right:
        SetLengthWithPercent(FDE_CSSNONINHERITS.m_Right, eType, pPrimitive,
                             FDE_CSSFONTSIZE);
        break;
      case FDE_CSSPROPERTY_ListStyleImage:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_pszListStyleImage = nullptr;
        } else if (eType == FDE_CSSPRIMITIVETYPE_URI) {
          int32_t iLength;
          FDE_CSSINHERITS.m_pszListStyleImage = pPrimitive->GetString(iLength);
        }
        break;
      case FDE_CSSPROPERTY_CaptionSide:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eCaptionSide = ToCaptionSide(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BackgroundRepeat:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBKGRepeat = ToBKGRepeat(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_BackgroundAttachment:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSNONINHERITS.m_eBKGAttachment =
              ToBKGAttachment(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_RubyAlign:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eRubyAlign = ToRubyAlign(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_RubyOverhang:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eRubyOverhang =
              ToRubyOverhang(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_RubyPosition:
        if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
          FDE_CSSINHERITS.m_eRubyPosition =
              ToRubyPosition(pPrimitive->GetEnum());
        }
        break;
      case FDE_CSSPROPERTY_RubySpan:
        FDE_CSSNONINHERITS.m_pRubySpan = pPrimitive;
        break;
      default:
        break;
    }
  } else if (pValue->GetType() == FDE_CSSVALUETYPE_List) {
    IFDE_CSSValueList* pList = static_cast<IFDE_CSSValueList*>(pValue);
    int32_t iCount = pList->CountValues();
    if (iCount > 0) {
      switch (eProperty) {
        case FDE_CSSPROPERTY_FontFamily:
          FDE_CSSINHERITS.m_pFontFamily = pList;
          break;
        case FDE_CSSPROPERTY_TextDecoration:
          FDE_CSSNONINHERITS.m_dwTextDecoration = ToTextDecoration(pList);
          break;
        case FDE_CSSPROPERTY_CounterIncrement: {
          if (!FDE_CSSNONINHERITS.m_pCounterStyle)
            FDE_CSSNONINHERITS.m_pCounterStyle = new CFDE_CSSCounterStyle;
          FDE_CSSNONINHERITS.m_pCounterStyle->SetCounterIncrementList(pList);
        } break;
        case FDE_CSSPROPERTY_CounterReset: {
          if (!FDE_CSSNONINHERITS.m_pCounterStyle)
            FDE_CSSNONINHERITS.m_pCounterStyle = new CFDE_CSSCounterStyle;
          FDE_CSSNONINHERITS.m_pCounterStyle->SetCounterResetList(pList);
        } break;
        case FDE_CSSPROPERTY_Content:
          FDE_CSSNONINHERITS.m_pContentList = pList;
          break;
        case FDE_CSSPROPERTY_Quotes:
          FDE_CSSINHERITS.m_pQuotes = pList;
          break;
        case FDE_CSSPROPERTY_TextCombine: {
          for (int32_t i = 0; i < pList->CountValues(); i++) {
            IFDE_CSSPrimitiveValue* pVal =
                static_cast<IFDE_CSSPrimitiveValue*>(pList->GetValue(i));
            switch (pVal->GetPrimitiveType()) {
              case FDE_CSSPRIMITIVETYPE_Enum: {
                switch (pVal->GetEnum()) {
                  case FDE_CSSPROPERTYVALUE_None: {
                    FDE_CSSNONINHERITS.m_eTextCombine = FDE_CSSTEXTCOMBINE_None;
                    FDE_CSSNONINHERITS.m_bHasTextCombineNumber = FALSE;
                  } break;
                  case FDE_CSSPROPERTYVALUE_Horizontal: {
                    FDE_CSSNONINHERITS.m_eTextCombine =
                        FDE_CSSTEXTCOMBINE_Horizontal;
                    FDE_CSSNONINHERITS.m_bHasTextCombineNumber = FALSE;
                  } break;
                  default:
                    break;
                }
              } break;
              case FDE_CSSPRIMITIVETYPE_Number:
                FDE_CSSNONINHERITS.m_fTextCombineNumber = pVal->GetFloat();
                break;
              default:
                break;
            }
          }
        } break;
        case FDE_CSSPROPERTY_TextEmphasisStyle: {
          FDE_CSSTEXTEMPHASISFILL eFill;
          FDE_CSSTEXTEMPHASISMARK eMark;
          for (int32_t i = 0; i < pList->CountValues(); i++) {
            IFDE_CSSPrimitiveValue* pVal =
                static_cast<IFDE_CSSPrimitiveValue*>(pList->GetValue(i));
            switch (pVal->GetPrimitiveType()) {
              case FDE_CSSPRIMITIVETYPE_Enum: {
                if (ToTextEmphasisFill(pVal->GetEnum(), eFill)) {
                  FDE_CSSINHERITS.m_eTextEmphasisFill = eFill;
                  continue;
                } else if (ToTextEmphasisMark(pVal->GetEnum(), eMark)) {
                  FDE_CSSINHERITS.m_eTextEmphasisMark = eMark;
                }
              } break;
              case FDE_CSSPRIMITIVETYPE_String: {
                FDE_CSSINHERITS.m_eTextEmphasisMark =
                    FDE_CSSTEXTEMPHASISMARK_Custom;
                int32_t iLen;
                FDE_CSSINHERITS.m_pszTextEmphasisCustomMark =
                    pVal->GetString(iLen);
              } break;
              default:
                break;
            }
          }
        } break;
        default:
          break;
      }
    }
  } else {
    ASSERT(FALSE);
  }
}
FX_FLOAT CFDE_CSSStyleSelector::ApplyNumber(FDE_CSSPRIMITIVETYPE eUnit,
                                            FX_FLOAT fValue,
                                            FX_FLOAT fPercentBase) {
  switch (eUnit) {
    case FDE_CSSPRIMITIVETYPE_PX:
    case FDE_CSSPRIMITIVETYPE_Number:
      return fValue * 72 / 96;
    case FDE_CSSPRIMITIVETYPE_PT:
      return fValue;
    case FDE_CSSPRIMITIVETYPE_EMS:
    case FDE_CSSPRIMITIVETYPE_EXS:
      return fValue * fPercentBase;
    case FDE_CSSPRIMITIVETYPE_Percent:
      return fValue * fPercentBase / 100.0f;
    case FDE_CSSPRIMITIVETYPE_CM:
      return fValue * 28.3464f;
    case FDE_CSSPRIMITIVETYPE_MM:
      return fValue * 2.8346f;
    case FDE_CSSPRIMITIVETYPE_IN:
      return fValue * 72.0f;
    case FDE_CSSPRIMITIVETYPE_PC:
      return fValue / 12.0f;
    default:
      return fValue;
  }
}
FDE_CSSRUBYSPAN CFDE_CSSStyleSelector::ToRubySpan(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
    default:
      return FDE_CSSRUBYSPAN_None;
  }
}
FDE_CSSRUBYPOSITION CFDE_CSSStyleSelector::ToRubyPosition(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Before:
      return FDE_CSSRUBYPOSITION_Before;
    case FDE_CSSPROPERTYVALUE_After:
      return FDE_CSSRUBYPOSITION_After;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSRUBYPOSITION_Right;
    case FDE_CSSPROPERTYVALUE_Inline:
      return FDE_CSSRUBYPOSITION_Inline;
    default:
      return FDE_CSSRUBYPOSITION_Before;
  }
}
FDE_CSSRUBYOVERHANG CFDE_CSSStyleSelector::ToRubyOverhang(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSRUBYOVERHANG_Auto;
    case FDE_CSSPROPERTYVALUE_Start:
      return FDE_CSSRUBYOVERHANG_Start;
    case FDE_CSSPROPERTYVALUE_End:
      return FDE_CSSRUBYOVERHANG_End;
    case FDE_CSSPROPERTYVALUE_None:
    default:
      return FDE_CSSRUBYOVERHANG_None;
  }
}
FDE_CSSRUBYALIGN CFDE_CSSStyleSelector::ToRubyAlign(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSRUBYALIGN_Auto;
    case FDE_CSSPROPERTYVALUE_Start:
      return FDE_CSSRUBYALIGN_Start;
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSRUBYALIGN_End;
    case FDE_CSSPROPERTYVALUE_Center:
      return FDE_CSSRUBYALIGN_Center;
    case FDE_CSSPROPERTYVALUE_End:
      return FDE_CSSRUBYALIGN_End;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSRUBYALIGN_Right;
    case FDE_CSSPROPERTYVALUE_DistributeLetter:
      return FDE_CSSRUBYALIGN_DistributeLetter;
    case FDE_CSSPROPERTYVALUE_DistributeSpace:
      return FDE_CSSRUBYALIGN_DistributeSpace;
    case FDE_CSSPROPERTYVALUE_LineEdge:
      return FDE_CSSRUBYALIGN_LineEdge;
    default:
      return FDE_CSSRUBYALIGN_Auto;
  }
}
FX_BOOL CFDE_CSSStyleSelector::ToTextEmphasisMark(
    FDE_CSSPROPERTYVALUE eValue,
    FDE_CSSTEXTEMPHASISMARK& eMark) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
      eMark = FDE_CSSTEXTEMPHASISMARK_None;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_Dot:
      eMark = FDE_CSSTEXTEMPHASISMARK_Dot;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_Circle:
      eMark = FDE_CSSTEXTEMPHASISMARK_Circle;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_DoubleCircle:
      eMark = FDE_CSSTEXTEMPHASISMARK_DoubleCircle;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_Triangle:
      eMark = FDE_CSSTEXTEMPHASISMARK_Triangle;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_Sesame:
      eMark = FDE_CSSTEXTEMPHASISMARK_Sesame;
      return TRUE;
    default:
      return FALSE;
  }
}
FX_BOOL CFDE_CSSStyleSelector::ToTextEmphasisFill(
    FDE_CSSPROPERTYVALUE eValue,
    FDE_CSSTEXTEMPHASISFILL& eFill) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Filled:
      eFill = FDE_CSSTEXTEMPHASISFILL_Filled;
      return TRUE;
    case FDE_CSSPROPERTYVALUE_Open:
      eFill = FDE_CSSTEXTEMPHASISFILL_Open;
      return TRUE;
    default:
      return FALSE;
  }
}
FDE_CSSBKGATTACHMENT CFDE_CSSStyleSelector::ToBKGAttachment(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Fixed:
      return FDE_CSSBKGATTACHMENT_Fixed;
    case FDE_CSSPROPERTYVALUE_Scroll:
      return FDE_CSSBKGATTACHMENT_Scroll;
    default:
      return FDE_CSSBKGATTACHMENT_Fixed;
  }
}
FDE_CSSCAPTIONSIDE CFDE_CSSStyleSelector::ToCaptionSide(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Top:
      return FDE_CSSCAPTIONSIDE_Top;
    case FDE_CSSPROPERTYVALUE_Bottom:
      return FDE_CSSCAPTIONSIDE_Bottom;
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSCAPTIONSIDE_Left;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSCAPTIONSIDE_Right;
    case FDE_CSSPROPERTYVALUE_Before:
      return FDE_CSSCAPTIONSIDE_Before;
    case FDE_CSSPROPERTYVALUE_After:
      return FDE_CSSCAPTIONSIDE_After;
    default:
      return FDE_CSSCAPTIONSIDE_Top;
  }
}
FDE_CSSPOSITION CFDE_CSSStyleSelector::ToPosition(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Static:
      return FDE_CSSPOSITION_Static;
    case FDE_CSSPROPERTYVALUE_Relative:
      return FDE_CSSPOSITION_Relative;
    case FDE_CSSPROPERTYVALUE_Fixed:
      return FDE_CSSPOSITION_Fixed;
    case FDE_CSSPROPERTYVALUE_Absolute:
      return FDE_CSSPOSITION_Absolute;
    default:
      return FDE_CSSPOSITION_Static;
  }
}
FDE_CSSCURSOR CFDE_CSSStyleSelector::ToCursor(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSCURSOR_Auto;
    case FDE_CSSPROPERTYVALUE_Crosshair:
      return FDE_CSSCURSOR_Crosshair;
    case FDE_CSSPROPERTYVALUE_Default:
      return FDE_CSSCURSOR_Default;
    case FDE_CSSPROPERTYVALUE_Pointer:
      return FDE_CSSCURSOR_Pointer;
    case FDE_CSSPROPERTYVALUE_Move:
      return FDE_CSSCURSOR_Move;
    case FDE_CSSPROPERTYVALUE_EResize:
      return FDE_CSSCURSOR_EResize;
    case FDE_CSSPROPERTYVALUE_NeResize:
      return FDE_CSSCURSOR_NeResize;
    case FDE_CSSPROPERTYVALUE_NwResize:
      return FDE_CSSCURSOR_NwResize;
    case FDE_CSSPROPERTYVALUE_NResize:
      return FDE_CSSCURSOR_NResize;
    case FDE_CSSPROPERTYVALUE_SeResize:
      return FDE_CSSCURSOR_SeResize;
    case FDE_CSSPROPERTYVALUE_SwResize:
      return FDE_CSSCURSOR_SwResize;
    default:
      return FDE_CSSCURSOR_Auto;
  }
}
FDE_CSSBKGREPEAT CFDE_CSSStyleSelector::ToBKGRepeat(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Repeat:
      return FDE_CSSBKGREPEAT_Repeat;
    case FDE_CSSPROPERTYVALUE_RepeatX:
      return FDE_CSSBKGREPEAT_RepeatX;
    case FDE_CSSPROPERTYVALUE_RepeatY:
      return FDE_CSSBKGREPEAT_RepeatY;
    case FDE_CSSPROPERTYVALUE_NoRepeat:
      return FDE_CSSBKGREPEAT_NoRepeat;
    default:
      return FDE_CSSBKGREPEAT_Repeat;
  }
}
FDE_CSSTEXTCOMBINE CFDE_CSSStyleSelector::ToTextCombine(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Horizontal:
      return FDE_CSSTEXTCOMBINE_Horizontal;
    case FDE_CSSPROPERTYVALUE_None:
    default:
      return FDE_CSSTEXTCOMBINE_None;
  }
}
FDE_CSSLINEBREAK CFDE_CSSStyleSelector::ToLineBreak(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSLINEBREAK_Auto;
    case FDE_CSSPROPERTYVALUE_Loose:
      return FDE_CSSLINEBREAK_Loose;
    case FDE_CSSPROPERTYVALUE_Normal:
      return FDE_CSSLINEBREAK_Normal;
    case FDE_CSSPROPERTYVALUE_Strict:
      return FDE_CSSLINEBREAK_Strict;
    default:
      return FDE_CSSLINEBREAK_Auto;
  }
}
FDE_CSSOVERFLOW CFDE_CSSStyleSelector::ToOverflow(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Visible:
      return FDE_CSSOVERFLOW_Visible;
    case FDE_CSSPROPERTYVALUE_Hidden:
      return FDE_CSSOVERFLOW_Hidden;
    case FDE_CSSPROPERTYVALUE_Scroll:
      return FDE_CSSOVERFLOW_Scroll;
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSOVERFLOW_Auto;
    case FDE_CSSPROPERTYVALUE_NoDisplay:
      return FDE_CSSOVERFLOW_NoDisplay;
    case FDE_CSSPROPERTYVALUE_NoContent:
      return FDE_CSSOVERFLOW_NoContent;
    default:
      return FDE_CSSOVERFLOW_Visible;
  }
}
FDE_CSSWRITINGMODE CFDE_CSSStyleSelector::ToWritingMode(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_HorizontalTb:
      return FDE_CSSWRITINGMODE_HorizontalTb;
    case FDE_CSSPROPERTYVALUE_VerticalRl:
      return FDE_CSSWRITINGMODE_VerticalRl;
    case FDE_CSSPROPERTYVALUE_VerticalLr:
      return FDE_CSSWRITINGMODE_VerticalLr;
    default:
      return FDE_CSSWRITINGMODE_HorizontalTb;
  }
}
FDE_CSSWORDBREAK CFDE_CSSStyleSelector::ToWordBreak(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Normal:
      return FDE_CSSWORDBREAK_Normal;
    case FDE_CSSPROPERTYVALUE_KeepAll:
      return FDE_CSSWORDBREAK_KeepAll;
    case FDE_CSSPROPERTYVALUE_BreakAll:
      return FDE_CSSWORDBREAK_BreakAll;
    case FDE_CSSPROPERTYVALUE_KeepWords:
      return FDE_CSSWORDBREAK_KeepWords;
    default:
      return FDE_CSSWORDBREAK_Normal;
  }
}
FDE_CSSFLOAT CFDE_CSSStyleSelector::ToFloat(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSFLOAT_Left;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSFLOAT_Right;
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSFLOAT_None;
    default:
      return FDE_CSSFLOAT_None;
  }
}
FDE_CSSCLEAR CFDE_CSSStyleSelector::ToClear(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSCLEAR_None;
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSCLEAR_Left;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSCLEAR_Right;
    case FDE_CSSPROPERTYVALUE_Both:
      return FDE_CSSCLEAR_Both;
    default:
      return FDE_CSSCLEAR_None;
  }
}
FDE_CSSPAGEBREAK CFDE_CSSStyleSelector::ToPageBreak(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Avoid:
      return FDE_CSSPAGEBREAK_Avoid;
    case FDE_CSSPROPERTYVALUE_Auto:
      return FDE_CSSPAGEBREAK_Auto;
    case FDE_CSSPROPERTYVALUE_Always:
      return FDE_CSSPAGEBREAK_Always;
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSPAGEBREAK_Left;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSPAGEBREAK_Right;
    default:
      return FDE_CSSPAGEBREAK_Auto;
  }
}
FDE_CSSDISPLAY CFDE_CSSStyleSelector::ToDisplay(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Inline:
      return FDE_CSSDISPLAY_Inline;
    case FDE_CSSPROPERTYVALUE_Block:
      return FDE_CSSDISPLAY_Block;
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSDISPLAY_None;
    case FDE_CSSPROPERTYVALUE_ListItem:
      return FDE_CSSDISPLAY_ListItem;
    case FDE_CSSPROPERTYVALUE_TableCell:
      return FDE_CSSDISPLAY_TableCell;
    case FDE_CSSPROPERTYVALUE_TableRow:
      return FDE_CSSDISPLAY_TableRow;
    case FDE_CSSPROPERTYVALUE_Table:
      return FDE_CSSDISPLAY_Table;
    case FDE_CSSPROPERTYVALUE_TableCaption:
      return FDE_CSSDISPLAY_TableCaption;
    case FDE_CSSPROPERTYVALUE_TableRowGroup:
      return FDE_CSSDISPLAY_TableRowGroup;
    case FDE_CSSPROPERTYVALUE_TableHeaderGroup:
      return FDE_CSSDISPLAY_TableHeaderGroup;
    case FDE_CSSPROPERTYVALUE_TableFooterGroup:
      return FDE_CSSDISPLAY_TableFooterGroup;
    case FDE_CSSPROPERTYVALUE_TableColumnGroup:
      return FDE_CSSDISPLAY_TableColumnGroup;
    case FDE_CSSPROPERTYVALUE_TableColumn:
      return FDE_CSSDISPLAY_TableColumn;
    case FDE_CSSPROPERTYVALUE_InlineTable:
      return FDE_CSSDISPLAY_InlineTable;
    case FDE_CSSPROPERTYVALUE_InlineBlock:
      return FDE_CSSDISPLAY_InlineBlock;
    case FDE_CSSPROPERTYVALUE_RunIn:
      return FDE_CSSDISPLAY_RunIn;
    case FDE_CSSPROPERTYVALUE_Ruby:
      return FDE_CSSDISPLAY_Ruby;
    case FDE_CSSPROPERTYVALUE_RubyBase:
      return FDE_CSSDISPLAY_RubyBase;
    case FDE_CSSPROPERTYVALUE_RubyText:
      return FDE_CSSDISPLAY_RubyText;
    case FDE_CSSPROPERTYVALUE_RubyBaseGroup:
      return FDE_CSSDISPLSY_RubyBaseGroup;
    case FDE_CSSPROPERTYVALUE_RubyTextGroup:
      return FDE_CSSDISPLAY_RubyTextGroup;
    default:
      return FDE_CSSDISPLAY_Inline;
  }
}
FDE_CSSTEXTALIGN CFDE_CSSStyleSelector::ToTextAlign(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Left:
      return FDE_CSSTEXTALIGN_Left;
    case FDE_CSSPROPERTYVALUE_Center:
      return FDE_CSSTEXTALIGN_Center;
    case FDE_CSSPROPERTYVALUE_Right:
      return FDE_CSSTEXTALIGN_Right;
    case FDE_CSSPROPERTYVALUE_Justify:
      return FDE_CSSTEXTALIGN_Justify;
    default:
      return FDE_CSSTEXTALIGN_Left;
  }
}
uint16_t CFDE_CSSStyleSelector::ToFontWeight(FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Normal:
      return 400;
    case FDE_CSSPROPERTYVALUE_Bold:
      return 700;
    case FDE_CSSPROPERTYVALUE_Bolder:
      return 900;
    case FDE_CSSPROPERTYVALUE_Lighter:
      return 200;
    default:
      return 400;
  }
}
FDE_CSSFONTSTYLE CFDE_CSSStyleSelector::ToFontStyle(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Italic:
    case FDE_CSSPROPERTYVALUE_Oblique:
      return FDE_CSSFONTSTYLE_Italic;
    default:
      return FDE_CSSFONTSTYLE_Normal;
  }
}
FDE_CSSBORDERSTYLE CFDE_CSSStyleSelector::ToBorderStyle(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSBORDERSTYLE_None;
    case FDE_CSSPROPERTYVALUE_Solid:
      return FDE_CSSBORDERSTYLE_Solid;
    case FDE_CSSPROPERTYVALUE_Hidden:
      return FDE_CSSBORDERSTYLE_Hidden;
    case FDE_CSSPROPERTYVALUE_Dotted:
      return FDE_CSSBORDERSTYLE_Dotted;
    case FDE_CSSPROPERTYVALUE_Dashed:
      return FDE_CSSBORDERSTYLE_Dashed;
    case FDE_CSSPROPERTYVALUE_Double:
      return FDE_CSSBORDERSTYLE_Double;
    case FDE_CSSPROPERTYVALUE_Groove:
      return FDE_CSSBORDERSTYLE_Groove;
    case FDE_CSSPROPERTYVALUE_Ridge:
      return FDE_CSSBORDERSTYLE_Ridge;
    case FDE_CSSPROPERTYVALUE_Inset:
      return FDE_CSSBORDERSTYLE_Inset;
    case FDE_CSSPROPERTYVALUE_Outset:
      return FDE_CSSBORDERSTYLE_outset;
    default:
      return FDE_CSSBORDERSTYLE_None;
  }
}
FX_BOOL CFDE_CSSStyleSelector::SetLengthWithPercent(
    FDE_CSSLENGTH& width,
    FDE_CSSPRIMITIVETYPE eType,
    IFDE_CSSPrimitiveValue* pPrimitive,
    FX_FLOAT fFontSize) {
  if (eType == FDE_CSSPRIMITIVETYPE_Percent) {
    width.Set(FDE_CSSLENGTHUNIT_Percent, pPrimitive->GetFloat() / 100.0f);
    return width.NonZero();
  } else if (eType >= FDE_CSSPRIMITIVETYPE_Number &&
             eType <= FDE_CSSPRIMITIVETYPE_PC) {
    FX_FLOAT fValue = ApplyNumber(eType, pPrimitive->GetFloat(), fFontSize);
    width.Set(FDE_CSSLENGTHUNIT_Point, fValue);
    return width.NonZero();
  } else if (eType == FDE_CSSPRIMITIVETYPE_Enum) {
    switch (pPrimitive->GetEnum()) {
      case FDE_CSSPROPERTYVALUE_Auto:
        width.Set(FDE_CSSLENGTHUNIT_Auto);
        return TRUE;
      case FDE_CSSPROPERTYVALUE_None:
        width.Set(FDE_CSSLENGTHUNIT_None);
        return TRUE;
      case FDE_CSSPROPERTYVALUE_Thin:
        width.Set(FDE_CSSLENGTHUNIT_Point, 2);
        return TRUE;
      case FDE_CSSPROPERTYVALUE_Medium:
        width.Set(FDE_CSSLENGTHUNIT_Point, 3);
        return TRUE;
      case FDE_CSSPROPERTYVALUE_Thick:
        width.Set(FDE_CSSLENGTHUNIT_Point, 4);
        return TRUE;
      default:
        return FALSE;
    }
  }
  return FALSE;
}
FX_FLOAT CFDE_CSSStyleSelector::ToFontSize(FDE_CSSPROPERTYVALUE eValue,
                                           FX_FLOAT fCurFontSize) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_XxSmall:
      return m_fDefFontSize / 1.2f / 1.2f / 1.2f;
    case FDE_CSSPROPERTYVALUE_XSmall:
      return m_fDefFontSize / 1.2f / 1.2f;
    case FDE_CSSPROPERTYVALUE_Small:
      return m_fDefFontSize / 1.2f;
    case FDE_CSSPROPERTYVALUE_Medium:
      return m_fDefFontSize;
    case FDE_CSSPROPERTYVALUE_Large:
      return m_fDefFontSize * 1.2f;
    case FDE_CSSPROPERTYVALUE_XLarge:
      return m_fDefFontSize * 1.2f * 1.2f;
    case FDE_CSSPROPERTYVALUE_XxLarge:
      return m_fDefFontSize * 1.2f * 1.2f * 1.2f;
    case FDE_CSSPROPERTYVALUE_Larger:
      return fCurFontSize * 1.2f;
    case FDE_CSSPROPERTYVALUE_Smaller:
      return fCurFontSize / 1.2f;
    default:
      return fCurFontSize;
  }
}
FDE_CSSVERTICALALIGN CFDE_CSSStyleSelector::ToVerticalAlign(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Baseline:
      return FDE_CSSVERTICALALIGN_Baseline;
    case FDE_CSSPROPERTYVALUE_Middle:
      return FDE_CSSVERTICALALIGN_Middle;
    case FDE_CSSPROPERTYVALUE_Bottom:
      return FDE_CSSVERTICALALIGN_Bottom;
    case FDE_CSSPROPERTYVALUE_Super:
      return FDE_CSSVERTICALALIGN_Super;
    case FDE_CSSPROPERTYVALUE_Sub:
      return FDE_CSSVERTICALALIGN_Sub;
    case FDE_CSSPROPERTYVALUE_Top:
      return FDE_CSSVERTICALALIGN_Top;
    case FDE_CSSPROPERTYVALUE_TextTop:
      return FDE_CSSVERTICALALIGN_TextTop;
    case FDE_CSSPROPERTYVALUE_TextBottom:
      return FDE_CSSVERTICALALIGN_TextBottom;
    default:
      return FDE_CSSVERTICALALIGN_Baseline;
  }
}
FDE_CSSLISTSTYLETYPE CFDE_CSSStyleSelector::ToListStyleType(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSLISTSTYLETYPE_None;
    case FDE_CSSPROPERTYVALUE_Disc:
      return FDE_CSSLISTSTYLETYPE_Disc;
    case FDE_CSSPROPERTYVALUE_Circle:
      return FDE_CSSLISTSTYLETYPE_Circle;
    case FDE_CSSPROPERTYVALUE_Square:
      return FDE_CSSLISTSTYLETYPE_Square;
    case FDE_CSSPROPERTYVALUE_Decimal:
      return FDE_CSSLISTSTYLETYPE_Decimal;
    case FDE_CSSPROPERTYVALUE_DecimalLeadingZero:
      return FDE_CSSLISTSTYLETYPE_DecimalLeadingZero;
    case FDE_CSSPROPERTYVALUE_LowerRoman:
      return FDE_CSSLISTSTYLETYPE_LowerRoman;
    case FDE_CSSPROPERTYVALUE_UpperRoman:
      return FDE_CSSLISTSTYLETYPE_UpperRoman;
    case FDE_CSSPROPERTYVALUE_LowerGreek:
      return FDE_CSSLISTSTYLETYPE_LowerGreek;
    case FDE_CSSPROPERTYVALUE_LowerLatin:
      return FDE_CSSLISTSTYLETYPE_LowerLatin;
    case FDE_CSSPROPERTYVALUE_UpperLatin:
      return FDE_CSSLISTSTYLETYPE_UpperLatin;
    case FDE_CSSPROPERTYVALUE_Armenian:
      return FDE_CSSLISTSTYLETYPE_Armenian;
    case FDE_CSSPROPERTYVALUE_Georgian:
      return FDE_CSSLISTSTYLETYPE_Georgian;
    case FDE_CSSPROPERTYVALUE_LowerAlpha:
      return FDE_CSSLISTSTYLETYPE_LowerAlpha;
    case FDE_CSSPROPERTYVALUE_UpperAlpha:
      return FDE_CSSLISTSTYLETYPE_UpperAlpha;
    case FDE_CSSPROPERTYVALUE_CjkIdeographic:
      return FDE_CSSLISTSTYLETYPE_CjkIdeographic;
    case FDE_CSSPROPERTYVALUE_Hebrew:
      return FDE_CSSLISTSTYLETYPE_Hebrew;
    case FDE_CSSLISTSTYLETYPE_Hiragana:
      return FDE_CSSLISTSTYLETYPE_Hiragana;
    case FDE_CSSLISTSTYLETYPE_HiraganaIroha:
      return FDE_CSSLISTSTYLETYPE_HiraganaIroha;
    case FDE_CSSLISTSTYLETYPE_Katakana:
      return FDE_CSSLISTSTYLETYPE_Katakana;
    case FDE_CSSLISTSTYLETYPE_KatakanaIroha:
      return FDE_CSSLISTSTYLETYPE_KatakanaIroha;
    default:
      return FDE_CSSLISTSTYLETYPE_Disc;
  }
}
FDE_CSSLISTSTYLEPOSITION CFDE_CSSStyleSelector::ToListStylePosition(
    FDE_CSSPROPERTYVALUE eValue) {
  return eValue == FDE_CSSPROPERTYVALUE_Inside
             ? FDE_CSSLISTSTYLEPOSITION_Inside
             : FDE_CSSLISTSTYLEPOSITION_Outside;
}
FDE_CSSVISIBILITY CFDE_CSSStyleSelector::ToVisibility(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Visible:
      return FDE_CSSVISIBILITY_Visible;
    case FDE_CSSPROPERTYVALUE_Hidden:
      return FDE_CSSVISIBILITY_Hidden;
    case FDE_CSSPROPERTYVALUE_Collapse:
      return FDE_CSSVISIBILITY_Collapse;
    default:
      return FDE_CSSVISIBILITY_Visible;
  }
}
FDE_CSSWHITESPACE CFDE_CSSStyleSelector::ToWhiteSpace(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_Normal:
      return FDE_CSSWHITESPACE_Normal;
    case FDE_CSSPROPERTYVALUE_Pre:
      return FDE_CSSWHITESPACE_Pre;
    case FDE_CSSPROPERTYVALUE_Nowrap:
      return FDE_CSSWHITESPACE_Nowrap;
    case FDE_CSSPROPERTYVALUE_PreWrap:
      return FDE_CSSWHITESPACE_PreWrap;
    case FDE_CSSPROPERTYVALUE_PreLine:
      return FDE_CSSWHITESPACE_PreLine;
    default:
      return FDE_CSSWHITESPACE_Normal;
  }
}
uint32_t CFDE_CSSStyleSelector::ToTextDecoration(IFDE_CSSValueList* pValue) {
  uint32_t dwDecoration = 0;
  for (int32_t i = pValue->CountValues() - 1; i >= 0; --i) {
    IFDE_CSSPrimitiveValue* pPrimitive =
        static_cast<IFDE_CSSPrimitiveValue*>(pValue->GetValue(i));
    if (pPrimitive->GetPrimitiveType() == FDE_CSSPRIMITIVETYPE_Enum) {
      switch (pPrimitive->GetEnum()) {
        case FDE_CSSPROPERTYVALUE_Underline:
          dwDecoration |= FDE_CSSTEXTDECORATION_Underline;
          break;
        case FDE_CSSPROPERTYVALUE_LineThrough:
          dwDecoration |= FDE_CSSTEXTDECORATION_LineThrough;
          break;
        case FDE_CSSPROPERTYVALUE_Overline:
          dwDecoration |= FDE_CSSTEXTDECORATION_Overline;
          break;
        case FDE_CSSPROPERTYVALUE_Blink:
          dwDecoration |= FDE_CSSTEXTDECORATION_Blink;
          break;
        case FDE_CSSPROPERTYVALUE_Double:
          dwDecoration |= FDE_CSSTEXTDECORATION_Double;
          break;
        default:
          break;
      }
    }
  }
  return dwDecoration;
}
FDE_CSSTEXTTRANSFORM CFDE_CSSStyleSelector::ToTextTransform(
    FDE_CSSPROPERTYVALUE eValue) {
  switch (eValue) {
    case FDE_CSSPROPERTYVALUE_None:
      return FDE_CSSTEXTTRANSFORM_None;
    case FDE_CSSPROPERTYVALUE_Capitalize:
      return FDE_CSSTEXTTRANSFORM_Capitalize;
    case FDE_CSSPROPERTYVALUE_Uppercase:
      return FDE_CSSTEXTTRANSFORM_UpperCase;
    case FDE_CSSPROPERTYVALUE_Lowercase:
      return FDE_CSSTEXTTRANSFORM_LowerCase;
    default:
      return FDE_CSSTEXTTRANSFORM_None;
  }
}
FDE_CSSFONTVARIANT CFDE_CSSStyleSelector::ToFontVariant(
    FDE_CSSPROPERTYVALUE eValue) {
  return eValue == FDE_CSSPROPERTYVALUE_SmallCaps ? FDE_CSSFONTVARIANT_SmallCaps
                                                  : FDE_CSSFONTVARIANT_Normal;
}

CFDE_CSSComputedStyle::CFDE_CSSComputedStyle(IFX_MemoryAllocator* pAlloc)
    : m_dwRefCount(1), m_pAllocator(pAlloc) {}

CFDE_CSSComputedStyle::~CFDE_CSSComputedStyle() {}

uint32_t CFDE_CSSComputedStyle::Retain() {
  return ++m_dwRefCount;
}

uint32_t CFDE_CSSComputedStyle::Release() {
  uint32_t dwRefCount = --m_dwRefCount;
  if (dwRefCount == 0) {
    delete m_NonInheritedData.m_pCounterStyle;
    FXTARGET_DeleteWith(CFDE_CSSComputedStyle, m_pAllocator, this);
  }
  return dwRefCount;
}

void CFDE_CSSComputedStyle::Reset() {
  m_InheritedData.Reset();
  m_NonInheritedData.Reset();
}

IFDE_CSSFontStyle* CFDE_CSSComputedStyle::GetFontStyles() {
  return static_cast<IFDE_CSSFontStyle*>(this);
}

IFDE_CSSBoundaryStyle* CFDE_CSSComputedStyle::GetBoundaryStyles() {
  return static_cast<IFDE_CSSBoundaryStyle*>(this);
}

IFDE_CSSPositionStyle* CFDE_CSSComputedStyle::GetPositionStyles() {
  return static_cast<IFDE_CSSPositionStyle*>(this);
}

IFDE_CSSParagraphStyle* CFDE_CSSComputedStyle::GetParagraphStyles() {
  return static_cast<IFDE_CSSParagraphStyle*>(this);
}

FX_BOOL CFDE_CSSComputedStyle::GetCustomStyle(const CFX_WideStringC& wsName,
                                              CFX_WideString& wsValue) const {
  for (int32_t i = m_CustomProperties.GetSize() - 2; i > -1; i -= 2) {
    if (wsName == m_CustomProperties[i]) {
      wsValue = m_CustomProperties[i + 1];
      return TRUE;
    }
  }
  return FALSE;
}

int32_t CFDE_CSSComputedStyle::CountFontFamilies() const {
  return m_InheritedData.m_pFontFamily
             ? m_InheritedData.m_pFontFamily->CountValues()
             : 0;
}

const FX_WCHAR* CFDE_CSSComputedStyle::GetFontFamily(int32_t index) const {
  return (static_cast<IFDE_CSSPrimitiveValue*>(
              m_InheritedData.m_pFontFamily->GetValue(index)))
      ->GetString(index);
}

uint16_t CFDE_CSSComputedStyle::GetFontWeight() const {
  return m_InheritedData.m_wFontWeight;
}

FDE_CSSFONTVARIANT CFDE_CSSComputedStyle::GetFontVariant() const {
  return static_cast<FDE_CSSFONTVARIANT>(m_InheritedData.m_eFontVariant);
}

FDE_CSSFONTSTYLE CFDE_CSSComputedStyle::GetFontStyle() const {
  return static_cast<FDE_CSSFONTSTYLE>(m_InheritedData.m_eFontStyle);
}

FX_FLOAT CFDE_CSSComputedStyle::GetFontSize() const {
  return m_InheritedData.m_fFontSize;
}

FX_ARGB CFDE_CSSComputedStyle::GetColor() const {
  return m_InheritedData.m_dwFontColor;
}

void CFDE_CSSComputedStyle::SetFontWeight(uint16_t wFontWeight) {
  m_InheritedData.m_wFontWeight = wFontWeight;
}

void CFDE_CSSComputedStyle::SetFontVariant(FDE_CSSFONTVARIANT eFontVariant) {
  m_InheritedData.m_eFontVariant = eFontVariant;
}

void CFDE_CSSComputedStyle::SetFontStyle(FDE_CSSFONTSTYLE eFontStyle) {
  m_InheritedData.m_eFontStyle = eFontStyle;
}

void CFDE_CSSComputedStyle::SetFontSize(FX_FLOAT fFontSize) {
  m_InheritedData.m_fFontSize = fFontSize;
}

void CFDE_CSSComputedStyle::SetColor(FX_ARGB dwFontColor) {
  m_InheritedData.m_dwFontColor = dwFontColor;
}

const FDE_CSSRECT* CFDE_CSSComputedStyle::GetBorderWidth() const {
  return m_NonInheritedData.m_bHasBorder ? &(m_NonInheritedData.m_BorderWidth)
                                         : nullptr;
}

const FDE_CSSRECT* CFDE_CSSComputedStyle::GetMarginWidth() const {
  return m_NonInheritedData.m_bHasMargin ? &(m_NonInheritedData.m_MarginWidth)
                                         : nullptr;
}

const FDE_CSSRECT* CFDE_CSSComputedStyle::GetPaddingWidth() const {
  return m_NonInheritedData.m_bHasPadding ? &(m_NonInheritedData.m_PaddingWidth)
                                          : nullptr;
}

void CFDE_CSSComputedStyle::SetMarginWidth(const FDE_CSSRECT& rect) {
  m_NonInheritedData.m_MarginWidth = rect;
  m_NonInheritedData.m_bHasMargin = TRUE;
}

void CFDE_CSSComputedStyle::SetPaddingWidth(const FDE_CSSRECT& rect) {
  m_NonInheritedData.m_PaddingWidth = rect;
  m_NonInheritedData.m_bHasPadding = TRUE;
}

FDE_CSSDISPLAY CFDE_CSSComputedStyle::GetDisplay() const {
  return static_cast<FDE_CSSDISPLAY>(m_NonInheritedData.m_eDisplay);
}

FX_FLOAT CFDE_CSSComputedStyle::GetLineHeight() const {
  return m_InheritedData.m_fLineHeight;
}

const FDE_CSSLENGTH& CFDE_CSSComputedStyle::GetTextIndent() const {
  return m_InheritedData.m_TextIndent;
}

FDE_CSSTEXTALIGN CFDE_CSSComputedStyle::GetTextAlign() const {
  return static_cast<FDE_CSSTEXTALIGN>(m_InheritedData.m_eTextAligh);
}

FDE_CSSVERTICALALIGN CFDE_CSSComputedStyle::GetVerticalAlign() const {
  return static_cast<FDE_CSSVERTICALALIGN>(m_NonInheritedData.m_eVerticalAlign);
}

FX_FLOAT CFDE_CSSComputedStyle::GetNumberVerticalAlign() const {
  return m_NonInheritedData.m_fVerticalAlign;
}

uint32_t CFDE_CSSComputedStyle::GetTextDecoration() const {
  return m_NonInheritedData.m_dwTextDecoration;
}

const FDE_CSSLENGTH& CFDE_CSSComputedStyle::GetLetterSpacing() const {
  return m_InheritedData.m_LetterSpacing;
}

void CFDE_CSSComputedStyle::SetLineHeight(FX_FLOAT fLineHeight) {
  m_InheritedData.m_fLineHeight = fLineHeight;
}

void CFDE_CSSComputedStyle::SetTextIndent(const FDE_CSSLENGTH& textIndent) {
  m_InheritedData.m_TextIndent = textIndent;
}

void CFDE_CSSComputedStyle::SetTextAlign(FDE_CSSTEXTALIGN eTextAlign) {
  m_InheritedData.m_eTextAligh = eTextAlign;
}

void CFDE_CSSComputedStyle::SetNumberVerticalAlign(FX_FLOAT fAlign) {
  m_NonInheritedData.m_eVerticalAlign = FDE_CSSVERTICALALIGN_Number,
  m_NonInheritedData.m_fVerticalAlign = fAlign;
}

void CFDE_CSSComputedStyle::SetTextDecoration(uint32_t dwTextDecoration) {
  m_NonInheritedData.m_dwTextDecoration = dwTextDecoration;
}

void CFDE_CSSComputedStyle::SetLetterSpacing(
    const FDE_CSSLENGTH& letterSpacing) {
  m_InheritedData.m_LetterSpacing = letterSpacing;
}

void CFDE_CSSComputedStyle::AddCustomStyle(const CFX_WideString& wsName,
                                           const CFX_WideString& wsValue) {
  m_CustomProperties.Add(wsName);
  m_CustomProperties.Add(wsValue);
}

CFDE_CSSInheritedData::CFDE_CSSInheritedData() {
  Reset();
}

void CFDE_CSSInheritedData::Reset() {
  FXSYS_memset(this, 0, sizeof(CFDE_CSSInheritedData));
  m_LetterSpacing.Set(FDE_CSSLENGTHUNIT_Normal);
  m_WordSpacing.Set(FDE_CSSLENGTHUNIT_Normal);
  m_TextIndent.Set(FDE_CSSLENGTHUNIT_Point, 0);
  m_fFontSize = 12.0f;
  m_fLineHeight = 14.0f;
  m_wFontWeight = 400;
  m_dwFontColor = 0xFF000000;
  m_iWidows = 2;
  m_bTextEmphasisColorCurrent = TRUE;
  m_iOrphans = 2;
}

CFDE_CSSNonInheritedData::CFDE_CSSNonInheritedData() {
  Reset();
}

void CFDE_CSSNonInheritedData::Reset() {
  FXSYS_memset(this, 0, sizeof(CFDE_CSSNonInheritedData));
  m_PaddingWidth.Set(FDE_CSSLENGTHUNIT_Point, 0);
  m_MarginWidth = m_PaddingWidth;
  m_BorderWidth = m_PaddingWidth;
  m_MinBoxSize.Set(FDE_CSSLENGTHUNIT_Point, 0);
  m_MaxBoxSize.Set(FDE_CSSLENGTHUNIT_None);
  m_eDisplay = FDE_CSSDISPLAY_Inline;
  m_fVerticalAlign = 0.0f;
  m_ColumnCount.Set(FDE_CSSLENGTHUNIT_Auto);
  m_ColumnGap.Set(FDE_CSSLENGTHUNIT_Normal);
  m_bColumnRuleColorSame = TRUE;
  m_ColumnWidth.Set(FDE_CSSLENGTHUNIT_Auto);
  m_ColumnRuleWidth.Set(FDE_CSSLENGTHUNIT_Auto);
  m_eTextCombine = FDE_CSSTEXTCOMBINE_None;
}
