// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/css/fde_cssdeclaration.h"

#include "core/fxcrt/include/fx_ext.h"
#include "xfa/fgas/crt/fgas_system.h"

IFDE_CSSValue* CFDE_CSSDeclaration::GetProperty(FDE_CSSPROPERTY eProperty,
                                                FX_BOOL& bImportant) const {
  for (const FDE_CSSPropertyHolder* pHolder = m_pFirstProperty; pHolder;
       pHolder = pHolder->pNext) {
    if (pHolder->eProperty == eProperty) {
      bImportant = pHolder->bImportant;
      return pHolder->pValue;
    }
  }
  return nullptr;
}
FX_POSITION CFDE_CSSDeclaration::GetStartPosition() const {
  return (FX_POSITION)m_pFirstProperty;
}
void CFDE_CSSDeclaration::GetNextProperty(FX_POSITION& pos,
                                          FDE_CSSPROPERTY& eProperty,
                                          IFDE_CSSValue*& pValue,
                                          FX_BOOL& bImportant) const {
  const FDE_CSSPropertyHolder* pHolder = (const FDE_CSSPropertyHolder*)pos;
  bImportant = pHolder->bImportant;
  eProperty = (FDE_CSSPROPERTY)pHolder->eProperty;
  pValue = pHolder->pValue;
  pos = (FX_POSITION)pHolder->pNext;
}
FX_POSITION CFDE_CSSDeclaration::GetStartCustom() const {
  return (FX_POSITION)m_pFirstCustom;
}
void CFDE_CSSDeclaration::GetNextCustom(FX_POSITION& pos,
                                        CFX_WideString& wsName,
                                        CFX_WideString& wsValue) const {
  const FDE_CSSCustomProperty* pProperty = (const FDE_CSSCustomProperty*)pos;
  if (!pProperty)
    return;

  wsName = pProperty->pwsName;
  wsValue = pProperty->pwsValue;
  pos = (FX_POSITION)pProperty->pNext;
}
const FX_WCHAR* CFDE_CSSDeclaration::CopyToLocal(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen) {
  ASSERT(iValueLen > 0);
  std::unordered_map<uint32_t, FX_WCHAR*>* pCache = pArgs->pStringCache;
  uint32_t key = 0;
  if (pCache) {
    key = FX_HashCode_GetW(CFX_WideStringC(pszValue, iValueLen), false);
    auto it = pCache->find(key);
    if (it != pCache->end())
      return it->second;
  }
  FX_WCHAR* psz =
      (FX_WCHAR*)pArgs->pStaticStore->Alloc((iValueLen + 1) * sizeof(FX_WCHAR));
  FXSYS_wcsncpy(psz, pszValue, iValueLen);
  psz[iValueLen] = '\0';
  if (pCache)
    (*pCache)[key] = psz;

  return psz;
}
IFDE_CSSPrimitiveValue* CFDE_CSSDeclaration::NewNumberValue(
    IFX_MemoryAllocator* pStaticStore,
    FDE_CSSPRIMITIVETYPE eUnit,
    FX_FLOAT fValue) const {
  static CFDE_CSSPrimitiveValue s_ZeroValue(FDE_CSSPRIMITIVETYPE_Number, 0.0f);
  if (eUnit == FDE_CSSPRIMITIVETYPE_Number && FXSYS_fabs(fValue) < 0.001f) {
    return &s_ZeroValue;
  }
  return FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(eUnit, fValue);
}
inline IFDE_CSSPrimitiveValue* CFDE_CSSDeclaration::NewEnumValue(
    IFX_MemoryAllocator* pStaticStore,
    FDE_CSSPROPERTYVALUE eValue) const {
  return FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(eValue);
}
void CFDE_CSSDeclaration::AddPropertyHolder(IFX_MemoryAllocator* pStaticStore,
                                            FDE_CSSPROPERTY eProperty,
                                            IFDE_CSSValue* pValue,
                                            FX_BOOL bImportant) {
  FDE_CSSPropertyHolder* pHolder =
      FXTARGET_NewWith(pStaticStore) FDE_CSSPropertyHolder;
  pHolder->bImportant = bImportant;
  pHolder->eProperty = eProperty;
  pHolder->pValue = pValue;
  pHolder->pNext = nullptr;
  if (m_pLastProperty)
    m_pLastProperty->pNext = pHolder;
  else
    m_pFirstProperty = pHolder;
  m_pLastProperty = pHolder;
}
FX_BOOL CFDE_CSSDeclaration::AddProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                         const FX_WCHAR* pszValue,
                                         int32_t iValueLen) {
  ASSERT(iValueLen > 0);
  FX_BOOL bImportant = FALSE;
  if (iValueLen >= 10 && pszValue[iValueLen - 10] == '!' &&
      FX_wcsnicmp(L"important", pszValue + iValueLen - 9, 9) == 0) {
    if ((iValueLen -= 10) == 0) {
      return FALSE;
    }
    bImportant = TRUE;
  }
  const uint32_t dwType = pArgs->pProperty->dwType;
  switch (dwType & 0x0F) {
    case FDE_CSSVALUETYPE_Primitive: {
      static const uint32_t g_ValueGuessOrder[] = {
          FDE_CSSVALUETYPE_MaybeNumber,   FDE_CSSVALUETYPE_MaybeEnum,
          FDE_CSSVALUETYPE_MaybeColor,    FDE_CSSVALUETYPE_MaybeURI,
          FDE_CSSVALUETYPE_MaybeFunction, FDE_CSSVALUETYPE_MaybeString,
      };
      static const int32_t g_ValueGuessCount =
          sizeof(g_ValueGuessOrder) / sizeof(uint32_t);
      for (int32_t i = 0; i < g_ValueGuessCount; ++i) {
        const uint32_t dwMatch = dwType & g_ValueGuessOrder[i];
        if (dwMatch == 0) {
          continue;
        }
        IFDE_CSSValue* pCSSValue = nullptr;
        switch (dwMatch) {
          case FDE_CSSVALUETYPE_MaybeFunction:
            pCSSValue = ParseFunction(pArgs, pszValue, iValueLen);
            break;
          case FDE_CSSVALUETYPE_MaybeNumber:
            pCSSValue = ParseNumber(pArgs, pszValue, iValueLen);
            break;
          case FDE_CSSVALUETYPE_MaybeEnum:
            pCSSValue = ParseEnum(pArgs, pszValue, iValueLen);
            break;
          case FDE_CSSVALUETYPE_MaybeColor:
            pCSSValue = ParseColor(pArgs, pszValue, iValueLen);
            break;
          case FDE_CSSVALUETYPE_MaybeURI:
            pCSSValue = ParseURI(pArgs, pszValue, iValueLen);
            break;
          case FDE_CSSVALUETYPE_MaybeString:
            pCSSValue = ParseString(pArgs, pszValue, iValueLen);
            break;
          default:
            break;
        }
        if (pCSSValue) {
          AddPropertyHolder(pArgs->pStaticStore, pArgs->pProperty->eName,
                            pCSSValue, bImportant);
          return TRUE;
        }
        if (FDE_IsOnlyValue(dwType, g_ValueGuessOrder[i])) {
          return FALSE;
        }
      }
    } break;
    case FDE_CSSVALUETYPE_Shorthand: {
      IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
      IFDE_CSSValue *pColor, *pStyle, *pWidth;
      switch (pArgs->pProperty->eName) {
        case FDE_CSSPROPERTY_Font:
          return ParseFontProperty(pArgs, pszValue, iValueLen, bImportant);
        case FDE_CSSPROPERTY_Background:
          return ParseBackgroundProperty(pArgs, pszValue, iValueLen,
                                         bImportant);
        case FDE_CSSPROPERTY_ListStyle:
          return ParseListStyleProperty(pArgs, pszValue, iValueLen, bImportant);
        case FDE_CSSPROPERTY_Border:
          if (ParseBorderPropoerty(pStaticStore, pszValue, iValueLen, pColor,
                                   pStyle, pWidth)) {
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderLeftColor,
                              FDE_CSSPROPERTY_BorderLeftStyle,
                              FDE_CSSPROPERTY_BorderLeftWidth);
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderTopColor,
                              FDE_CSSPROPERTY_BorderTopStyle,
                              FDE_CSSPROPERTY_BorderTopWidth);
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderRightColor,
                              FDE_CSSPROPERTY_BorderRightStyle,
                              FDE_CSSPROPERTY_BorderRightWidth);
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderBottomColor,
                              FDE_CSSPROPERTY_BorderBottomStyle,
                              FDE_CSSPROPERTY_BorderBottomWidth);
            return TRUE;
          }
          break;
        case FDE_CSSPROPERTY_BorderLeft:
          if (ParseBorderPropoerty(pStaticStore, pszValue, iValueLen, pColor,
                                   pStyle, pWidth)) {
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderLeftColor,
                              FDE_CSSPROPERTY_BorderLeftStyle,
                              FDE_CSSPROPERTY_BorderLeftWidth);
            return TRUE;
          }
          break;
        case FDE_CSSPROPERTY_BorderTop:
          if (ParseBorderPropoerty(pStaticStore, pszValue, iValueLen, pColor,
                                   pStyle, pWidth)) {
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderTopColor,
                              FDE_CSSPROPERTY_BorderTopStyle,
                              FDE_CSSPROPERTY_BorderTopWidth);
            return TRUE;
          }
          break;
        case FDE_CSSPROPERTY_BorderRight:
          if (ParseBorderPropoerty(pStaticStore, pszValue, iValueLen, pColor,
                                   pStyle, pWidth)) {
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderRightColor,
                              FDE_CSSPROPERTY_BorderRightStyle,
                              FDE_CSSPROPERTY_BorderRightWidth);
            return TRUE;
          }
          break;
        case FDE_CSSPROPERTY_BorderBottom:
          if (ParseBorderPropoerty(pStaticStore, pszValue, iValueLen, pColor,
                                   pStyle, pWidth)) {
            AddBorderProperty(pStaticStore, pColor, pStyle, pWidth, bImportant,
                              FDE_CSSPROPERTY_BorderBottomColor,
                              FDE_CSSPROPERTY_BorderBottomStyle,
                              FDE_CSSPROPERTY_BorderBottomWidth);
            return TRUE;
          }
          break;
        case FDE_CSSPROPERTY_Overflow:
          return ParseOverflowProperty(pArgs, pszValue, iValueLen, bImportant);
        case FDE_CSSPROPERTY_ColumnRule:
          return ParseColumnRuleProperty(pArgs, pszValue, iValueLen,
                                         bImportant);
        default:
          break;
      }
    } break;
    case FDE_CSSVALUETYPE_List:
      switch (pArgs->pProperty->eName) {
        case FDE_CSSPROPERTY_CounterIncrement:
        case FDE_CSSPROPERTY_CounterReset:
          return ParseCounterProperty(pArgs, pszValue, iValueLen, bImportant);
        case FDE_CSSPROPERTY_Content:
          return ParseContentProperty(pArgs, pszValue, iValueLen, bImportant);
        default:
          return ParseValueListProperty(pArgs, pszValue, iValueLen, bImportant);
      }
    default:
      ASSERT(FALSE);
      break;
  }
  return FALSE;
}
FX_BOOL CFDE_CSSDeclaration::AddProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                         const FX_WCHAR* pszName,
                                         int32_t iNameLen,
                                         const FX_WCHAR* pszValue,
                                         int32_t iValueLen) {
  FDE_CSSCustomProperty* pProperty =
      FXTARGET_NewWith(pArgs->pStaticStore) FDE_CSSCustomProperty;
  pProperty->pwsName = CopyToLocal(pArgs, pszName, iNameLen);
  pProperty->pwsValue = CopyToLocal(pArgs, pszValue, iValueLen);
  pProperty->pNext = nullptr;
  if (m_pLastCustom)
    m_pLastCustom->pNext = pProperty;
  else
    m_pFirstCustom = pProperty;
  m_pLastCustom = pProperty;
  return TRUE;
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseNumber(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen) {
  FX_FLOAT fValue;
  FDE_CSSPRIMITIVETYPE eUnit;
  if (!FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eUnit)) {
    return nullptr;
  }
  return NewNumberValue(pArgs->pStaticStore, eUnit, fValue);
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseEnum(const FDE_CSSPROPERTYARGS* pArgs,
                                              const FX_WCHAR* pszValue,
                                              int32_t iValueLen) {
  const FDE_CSSPROPERTYVALUETABLE* pValue =
      FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
  return pValue ? NewEnumValue(pArgs->pStaticStore, pValue->eName) : nullptr;
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseColor(const FDE_CSSPROPERTYARGS* pArgs,
                                               const FX_WCHAR* pszValue,
                                               int32_t iValueLen) {
  FX_ARGB dwColor;
  if (!FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
    return nullptr;
  }
  return FXTARGET_NewWith(pArgs->pStaticStore) CFDE_CSSPrimitiveValue(dwColor);
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseURI(const FDE_CSSPROPERTYARGS* pArgs,
                                             const FX_WCHAR* pszValue,
                                             int32_t iValueLen) {
  int32_t iOffset;
  if (!FDE_ParseCSSURI(pszValue, iValueLen, iOffset, iValueLen)) {
    return nullptr;
  }
  if (iValueLen <= 0) {
    return nullptr;
  }
  pszValue = CopyToLocal(pArgs, pszValue + iOffset, iValueLen);
  return pszValue
             ? FXTARGET_NewWith(pArgs->pStaticStore)
                   CFDE_CSSPrimitiveValue(FDE_CSSPRIMITIVETYPE_URI, pszValue)
             : nullptr;
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseString(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen) {
  int32_t iOffset;
  if (!FDE_ParseCSSString(pszValue, iValueLen, iOffset, iValueLen)) {
    return nullptr;
  }
  if (iValueLen <= 0) {
    return nullptr;
  }
  pszValue = CopyToLocal(pArgs, pszValue + iOffset, iValueLen);
  return pszValue
             ? FXTARGET_NewWith(pArgs->pStaticStore)
                   CFDE_CSSPrimitiveValue(FDE_CSSPRIMITIVETYPE_String, pszValue)
             : nullptr;
}
IFDE_CSSValue* CFDE_CSSDeclaration::ParseFunction(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen) {
  if (pszValue[iValueLen - 1] != ')') {
    return nullptr;
  }
  int32_t iStartBracket = 0;
  while (pszValue[iStartBracket] != '(') {
    if (iStartBracket < iValueLen) {
      iStartBracket++;
    } else {
      return nullptr;
    }
  }
  if (iStartBracket == 0) {
    return nullptr;
  }
  const FX_WCHAR* pszFuncName = CopyToLocal(pArgs, pszValue, iStartBracket);
  pszValue += (iStartBracket + 1);
  iValueLen -= (iStartBracket + 2);
  CFDE_CSSValueArray argumentArr;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ',');
  FDE_CSSPRIMITIVETYPE ePrimitiveType;
  while (parser.NextValue(ePrimitiveType, pszValue, iValueLen)) {
    switch (ePrimitiveType) {
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pPropertyValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pPropertyValue) {
          argumentArr.Add(
              NewEnumValue(pArgs->pStaticStore, pPropertyValue->eName));
          continue;
        }
        IFDE_CSSValue* pFunctionValue =
            ParseFunction(pArgs, pszValue, iValueLen);
        if (pFunctionValue) {
          argumentArr.Add(pFunctionValue);
          continue;
        }
        argumentArr.Add(FXTARGET_NewWith(pArgs->pStaticStore)
                            CFDE_CSSPrimitiveValue(
                                FDE_CSSPRIMITIVETYPE_String,
                                CopyToLocal(pArgs, pszValue, iValueLen)));
      } break;
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, ePrimitiveType)) {
          argumentArr.Add(
              NewNumberValue(pArgs->pStaticStore, ePrimitiveType, fValue));
        }
      } break;
      default:
        argumentArr.Add(FXTARGET_NewWith(pArgs->pStaticStore)
                            CFDE_CSSPrimitiveValue(
                                FDE_CSSPRIMITIVETYPE_String,
                                CopyToLocal(pArgs, pszValue, iValueLen)));
        break;
    }
  }
  IFDE_CSSValueList* pArgumentList = FXTARGET_NewWith(pArgs->pStaticStore)
      CFDE_CSSValueList(pArgs->pStaticStore, argumentArr);
  CFDE_CSSFunction* pFunction = FXTARGET_NewWith(pArgs->pStaticStore)
      CFDE_CSSFunction(pszFuncName, pArgumentList);
  return FXTARGET_NewWith(pArgs->pStaticStore)
      CFDE_CSSPrimitiveValue(pFunction);
}
FX_BOOL CFDE_CSSDeclaration::ParseContentProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  FDE_CSSPRIMITIVETYPE eType;
  CFDE_CSSValueArray list;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_URI:
        list.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
            eType, CopyToLocal(pArgs, pszValue, iValueLen)));
        break;
      case FDE_CSSPRIMITIVETYPE_Number:
        return FALSE;
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pValue) {
          switch (pValue->eName) {
            case FDE_CSSPROPERTYVALUE_Normal:
            case FDE_CSSPROPERTYVALUE_None: {
              if (list.GetSize() == 0) {
                list.Add(NewEnumValue(pStaticStore, pValue->eName));
              } else {
                return FALSE;
              }
            } break;
            case FDE_CSSPROPERTYVALUE_OpenQuote:
            case FDE_CSSPROPERTYVALUE_CloseQuote:
            case FDE_CSSPROPERTYVALUE_NoOpenQuote:
            case FDE_CSSPROPERTYVALUE_NoCloseQuote:
              list.Add(NewEnumValue(pStaticStore, pValue->eName));
              break;
            default:
              return FALSE;
          }
          continue;
        }
        IFDE_CSSValue* pFunction = ParseFunction(pArgs, pszValue, iValueLen);
        if (pFunction) {
          list.Add(pFunction);
          continue;
        }
        list.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
            eType, CopyToLocal(pArgs, pszValue, iValueLen)));
      } break;
      case FDE_CSSPRIMITIVETYPE_RGB:
        return FALSE;
      default:
        break;
    }
  }
  if (list.GetSize() == 0) {
    return FALSE;
  }
  AddPropertyHolder(pStaticStore, pArgs->pProperty->eName,
                    FXTARGET_NewWith(pStaticStore)
                        CFDE_CSSValueList(pStaticStore, list),
                    bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseCounterProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  CFDE_CSSValueArray list;
  CFDE_CSSValueArray listFull;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
          if (list.GetSize() == 1) {
            list.Add(NewNumberValue(pStaticStore, eType, fValue));
            listFull.Add(FXTARGET_NewWith(pStaticStore)
                             CFDE_CSSValueList(pStaticStore, list));
            list.RemoveAll();
          } else {
            return FALSE;
          }
        }
      } break;
      case FDE_CSSPRIMITIVETYPE_String: {
        if (list.GetSize() == 0) {
          pszValue = CopyToLocal(pArgs, pszValue, iValueLen);
          list.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              FDE_CSSPRIMITIVETYPE_String, pszValue));
        } else {
          listFull.Add(FXTARGET_NewWith(pStaticStore)
                           CFDE_CSSValueList(pStaticStore, list));
          list.RemoveAll();
          pszValue = CopyToLocal(pArgs, pszValue, iValueLen);
          list.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              FDE_CSSPRIMITIVETYPE_String, pszValue));
        }
      } break;
      default:
        break;
    }
  }
  if (list.GetSize() == 1) {
    listFull.Add(FXTARGET_NewWith(pStaticStore)
                     CFDE_CSSValueList(pStaticStore, list));
  }
  if (listFull.GetSize() == 0) {
    return FALSE;
  }
  AddPropertyHolder(pStaticStore, pArgs->pProperty->eName,
                    FXTARGET_NewWith(pStaticStore)
                        CFDE_CSSValueList(pStaticStore, listFull),
                    bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseValueListProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  FX_WCHAR separator =
      (pArgs->pProperty->eName == FDE_CSSPROPERTY_FontFamily) ? ',' : ' ';
  CFDE_CSSValueListParser parser(pszValue, iValueLen, separator);
  const uint32_t dwType = pArgs->pProperty->dwType;
  FDE_CSSPRIMITIVETYPE eType;
  CFDE_CSSValueArray list;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_Number:
        if (dwType & FDE_CSSVALUETYPE_MaybeNumber) {
          FX_FLOAT fValue;
          if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
            list.Add(NewNumberValue(pStaticStore, eType, fValue));
          }
        }
        break;
      case FDE_CSSPRIMITIVETYPE_String:
        if (dwType & FDE_CSSVALUETYPE_MaybeColor) {
          FX_ARGB dwColor;
          if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
            list.Add(FXTARGET_NewWith(pStaticStore)
                         CFDE_CSSPrimitiveValue(dwColor));
            continue;
          }
        }
        if (dwType & FDE_CSSVALUETYPE_MaybeEnum) {
          const FDE_CSSPROPERTYVALUETABLE* pValue =
              FDE_GetCSSPropertyValueByName(
                  CFX_WideStringC(pszValue, iValueLen));
          if (pValue) {
            list.Add(NewEnumValue(pStaticStore, pValue->eName));
            continue;
          }
        }
        if (dwType & FDE_CSSVALUETYPE_MaybeString) {
          pszValue = CopyToLocal(pArgs, pszValue, iValueLen);
          list.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              FDE_CSSPRIMITIVETYPE_String, pszValue));
        }
        break;
      case FDE_CSSPRIMITIVETYPE_RGB:
        if (dwType & FDE_CSSVALUETYPE_MaybeColor) {
          FX_ARGB dwColor;
          if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
            list.Add(FXTARGET_NewWith(pStaticStore)
                         CFDE_CSSPrimitiveValue(dwColor));
          }
        }
        break;
      default:
        break;
    }
  }
  if (list.GetSize() == 0) {
    return FALSE;
  }
  switch (pArgs->pProperty->eName) {
    case FDE_CSSPROPERTY_BorderColor:
      return Add4ValuesProperty(
          pStaticStore, list, bImportant, FDE_CSSPROPERTY_BorderLeftColor,
          FDE_CSSPROPERTY_BorderTopColor, FDE_CSSPROPERTY_BorderRightColor,
          FDE_CSSPROPERTY_BorderBottomColor);
    case FDE_CSSPROPERTY_BorderStyle:
      return Add4ValuesProperty(
          pStaticStore, list, bImportant, FDE_CSSPROPERTY_BorderLeftStyle,
          FDE_CSSPROPERTY_BorderTopStyle, FDE_CSSPROPERTY_BorderRightStyle,
          FDE_CSSPROPERTY_BorderBottomStyle);
    case FDE_CSSPROPERTY_BorderWidth:
      return Add4ValuesProperty(
          pStaticStore, list, bImportant, FDE_CSSPROPERTY_BorderLeftWidth,
          FDE_CSSPROPERTY_BorderTopWidth, FDE_CSSPROPERTY_BorderRightWidth,
          FDE_CSSPROPERTY_BorderBottomWidth);
    case FDE_CSSPROPERTY_Margin:
      return Add4ValuesProperty(
          pStaticStore, list, bImportant, FDE_CSSPROPERTY_MarginLeft,
          FDE_CSSPROPERTY_MarginTop, FDE_CSSPROPERTY_MarginRight,
          FDE_CSSPROPERTY_MarginBottom);
    case FDE_CSSPROPERTY_Padding:
      return Add4ValuesProperty(
          pStaticStore, list, bImportant, FDE_CSSPROPERTY_PaddingLeft,
          FDE_CSSPROPERTY_PaddingTop, FDE_CSSPROPERTY_PaddingRight,
          FDE_CSSPROPERTY_PaddingBottom);
    default: {
      CFDE_CSSValueList* pList =
          FXTARGET_NewWith(pStaticStore) CFDE_CSSValueList(pStaticStore, list);
      AddPropertyHolder(pStaticStore, pArgs->pProperty->eName, pList,
                        bImportant);
      return TRUE;
    } break;
  }
  return FALSE;
}
FX_BOOL CFDE_CSSDeclaration::Add4ValuesProperty(
    IFX_MemoryAllocator* pStaticStore,
    const CFDE_CSSValueArray& list,
    FX_BOOL bImportant,
    FDE_CSSPROPERTY eLeft,
    FDE_CSSPROPERTY eTop,
    FDE_CSSPROPERTY eRight,
    FDE_CSSPROPERTY eBottom) {
  switch (list.GetSize()) {
    case 1:
      AddPropertyHolder(pStaticStore, eLeft, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eTop, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eRight, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eBottom, list[0], bImportant);
      return TRUE;
    case 2:
      AddPropertyHolder(pStaticStore, eLeft, list[1], bImportant);
      AddPropertyHolder(pStaticStore, eTop, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eRight, list[1], bImportant);
      AddPropertyHolder(pStaticStore, eBottom, list[0], bImportant);
      return TRUE;
    case 3:
      AddPropertyHolder(pStaticStore, eLeft, list[1], bImportant);
      AddPropertyHolder(pStaticStore, eTop, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eRight, list[1], bImportant);
      AddPropertyHolder(pStaticStore, eBottom, list[2], bImportant);
      return TRUE;
    case 4:
      AddPropertyHolder(pStaticStore, eLeft, list[3], bImportant);
      AddPropertyHolder(pStaticStore, eTop, list[0], bImportant);
      AddPropertyHolder(pStaticStore, eRight, list[1], bImportant);
      AddPropertyHolder(pStaticStore, eBottom, list[2], bImportant);
      return TRUE;
    default:
      break;
  }
  return FALSE;
}
FX_BOOL CFDE_CSSDeclaration::ParseBorderPropoerty(
    IFX_MemoryAllocator* pStaticStore,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    IFDE_CSSValue*& pColor,
    IFDE_CSSValue*& pStyle,
    IFDE_CSSValue*& pWidth) const {
  pColor = pStyle = pWidth = nullptr;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_Number:
        if (!pWidth) {
          FX_FLOAT fValue;
          if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
            pWidth = NewNumberValue(pStaticStore, eType, fValue);
          }
        }
        break;
      case FDE_CSSPRIMITIVETYPE_RGB:
        if (!pColor) {
          FX_ARGB dwColor;
          if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
            pColor =
                FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(dwColor);
          }
        }
        break;
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSCOLORTABLE* pColorItem =
            FDE_GetCSSColorByName(CFX_WideStringC(pszValue, iValueLen));
        if (pColorItem) {
          if (!pColor) {
            pColor = FXTARGET_NewWith(pStaticStore)
                CFDE_CSSPrimitiveValue(pColorItem->dwValue);
          }
          continue;
        }
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (!pValue)
          continue;

        switch (pValue->eName) {
          case FDE_CSSPROPERTYVALUE_Transparent:
            if (!pColor) {
              pColor = FXTARGET_NewWith(pStaticStore)
                  CFDE_CSSPrimitiveValue((FX_ARGB)0);
            }
            break;
          case FDE_CSSPROPERTYVALUE_Thin:
          case FDE_CSSPROPERTYVALUE_Thick:
          case FDE_CSSPROPERTYVALUE_Medium:
            if (!pWidth)
              pWidth = NewEnumValue(pStaticStore, pValue->eName);
            break;
          case FDE_CSSPROPERTYVALUE_None:
          case FDE_CSSPROPERTYVALUE_Hidden:
          case FDE_CSSPROPERTYVALUE_Dotted:
          case FDE_CSSPROPERTYVALUE_Dashed:
          case FDE_CSSPROPERTYVALUE_Solid:
          case FDE_CSSPROPERTYVALUE_Double:
          case FDE_CSSPROPERTYVALUE_Groove:
          case FDE_CSSPROPERTYVALUE_Ridge:
          case FDE_CSSPROPERTYVALUE_Inset:
          case FDE_CSSPROPERTYVALUE_Outset:
            if (!pStyle)
              pStyle = NewEnumValue(pStaticStore, pValue->eName);
            break;
          default:
            break;
        }
      }; break;
      default:
        break;
    }
  }
  if (!pColor)
    pColor = FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue((FX_ARGB)0);
  if (!pStyle)
    pStyle = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_None);
  if (!pWidth)
    pWidth = NewNumberValue(pStaticStore, FDE_CSSPRIMITIVETYPE_Number, 0.0f);
  return TRUE;
}
void CFDE_CSSDeclaration::AddBorderProperty(IFX_MemoryAllocator* pStaticStore,
                                            IFDE_CSSValue* pColor,
                                            IFDE_CSSValue* pStyle,
                                            IFDE_CSSValue* pWidth,
                                            FX_BOOL bImportant,
                                            FDE_CSSPROPERTY eColor,
                                            FDE_CSSPROPERTY eStyle,
                                            FDE_CSSPROPERTY eWidth) {
  AddPropertyHolder(pStaticStore, eStyle, pStyle, bImportant);
  AddPropertyHolder(pStaticStore, eWidth, pWidth, bImportant);
  AddPropertyHolder(pStaticStore, eColor, pColor, bImportant);
}
FX_BOOL CFDE_CSSDeclaration::ParseListStyleProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  IFDE_CSSPrimitiveValue* pType = nullptr;
  IFDE_CSSPrimitiveValue* pImage = nullptr;
  IFDE_CSSPrimitiveValue* pPosition = nullptr;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_URI:
        if (!pImage) {
          pImage = FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              eType, CopyToLocal(pArgs, pszValue, iValueLen));
        }
        break;
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (!pValue)
          break;

        switch (pValue->eName) {
          case FDE_CSSPROPERTYVALUE_None:
            if (!pImage)
              pImage = NewEnumValue(pStaticStore, pValue->eName);
            else if (!pType)
              pImage = NewEnumValue(pStaticStore, pValue->eName);
            break;
          case FDE_CSSPROPERTYVALUE_Inside:
          case FDE_CSSPROPERTYVALUE_Outside:
            if (!pPosition)
              pPosition = NewEnumValue(pStaticStore, pValue->eName);
            break;
          case FDE_CSSPROPERTYVALUE_Disc:
          case FDE_CSSPROPERTYVALUE_Circle:
          case FDE_CSSPROPERTYVALUE_Square:
          case FDE_CSSPROPERTYVALUE_Decimal:
          case FDE_CSSPROPERTYVALUE_DecimalLeadingZero:
          case FDE_CSSPROPERTYVALUE_LowerRoman:
          case FDE_CSSPROPERTYVALUE_UpperRoman:
          case FDE_CSSPROPERTYVALUE_LowerGreek:
          case FDE_CSSPROPERTYVALUE_LowerLatin:
          case FDE_CSSPROPERTYVALUE_UpperLatin:
          case FDE_CSSPROPERTYVALUE_Armenian:
          case FDE_CSSPROPERTYVALUE_Georgian:
          case FDE_CSSPROPERTYVALUE_LowerAlpha:
          case FDE_CSSPROPERTYVALUE_UpperAlpha:
            if (!pType)
              pType = NewEnumValue(pStaticStore, pValue->eName);
            break;
          default:
            break;
        }
      }; break;
      default:
        break;
    }
  }
  if (!pPosition)
    pPosition = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Outside);
  if (!pImage)
    pImage = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_None);
  if (!pType)
    pType = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_None);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ListStylePosition, pPosition,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ListStyleImage, pImage,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ListStyleType, pType,
                    bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseBackgroundProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  IFDE_CSSPrimitiveValue* pColor = nullptr;
  IFDE_CSSPrimitiveValue* pImage = nullptr;
  IFDE_CSSPrimitiveValue* pRepeat = nullptr;
  IFDE_CSSPrimitiveValue* pPosX = nullptr;
  IFDE_CSSPrimitiveValue* pPosY = nullptr;
  IFDE_CSSPrimitiveValue* pAttachment = nullptr;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_URI:
        if (!pImage) {
          pImage = FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              eType, CopyToLocal(pArgs, pszValue, iValueLen));
        }
        break;
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (!FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
          break;
        }
        if (!pPosX)
          pPosX = NewNumberValue(pStaticStore, eType, fValue);
        else if (!pPosY)
          pPosY = NewNumberValue(pStaticStore, eType, fValue);
      } break;
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pValue) {
          switch (pValue->eName) {
            case FDE_CSSPROPERTYVALUE_None:
              if (!pImage)
                pImage = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Transparent:
              if (!pColor) {
                pColor = FXTARGET_NewWith(pStaticStore)
                    CFDE_CSSPrimitiveValue((FX_ARGB)0);
              }
              break;
            case FDE_CSSPROPERTYVALUE_Fixed:
            case FDE_CSSPROPERTYVALUE_Scroll:
              if (!pAttachment)
                pAttachment = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Repeat:
            case FDE_CSSPROPERTYVALUE_RepeatX:
            case FDE_CSSPROPERTYVALUE_RepeatY:
            case FDE_CSSPROPERTYVALUE_NoRepeat:
              if (!pRepeat)
                pRepeat = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Left:
            case FDE_CSSPROPERTYVALUE_Right:
              if (!pPosX)
                pPosX = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Top:
            case FDE_CSSPROPERTYVALUE_Bottom:
              if (!pPosY)
                pPosX = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Center:
              if (!pPosX)
                pPosX = NewEnumValue(pStaticStore, pValue->eName);
              else if (!pPosY)
                pPosX = NewEnumValue(pStaticStore, pValue->eName);
              break;
            default:
              break;
          }
          break;
        }
        const FDE_CSSCOLORTABLE* pColorItem =
            FDE_GetCSSColorByName(CFX_WideStringC(pszValue, iValueLen));
        if (pColorItem) {
          if (!pColor) {
            pColor = FXTARGET_NewWith(pStaticStore)
                CFDE_CSSPrimitiveValue(pColorItem->dwValue);
          }
        }
      } break;
      case FDE_CSSPRIMITIVETYPE_RGB:
        if (!pColor) {
          FX_ARGB dwColor;
          if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
            pColor =
                FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(dwColor);
          }
        }
        break;
      default:
        break;
    }
  }
  if (!pColor) {
    pColor = FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue((FX_ARGB)0);
  }
  if (!pImage)
    pImage = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_None);

  if (!pRepeat)
    pRepeat = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Repeat);

  if (!pAttachment)
    pAttachment = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Scroll);

  if (!pPosX) {
    pPosX = NewNumberValue(pStaticStore, FDE_CSSPRIMITIVETYPE_Number, 0.0f);
    pPosY = NewNumberValue(pStaticStore, FDE_CSSPRIMITIVETYPE_Number, 0.0f);
  } else if (!pPosY) {
    pPosY = NewNumberValue(pStaticStore, FDE_CSSPRIMITIVETYPE_Number, 0.0f);
  }
  CFDE_CSSValueArray position;
  position.Add(pPosX);
  position.Add(pPosY);
  CFDE_CSSValueList* pPosList =
      FXTARGET_NewWith(pStaticStore) CFDE_CSSValueList(pStaticStore, position);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_BackgroundColor, pColor,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_BackgroundImage, pImage,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_BackgroundRepeat, pRepeat,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_BackgroundPosition, pPosList,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_BackgroundAttachment,
                    pAttachment, bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseFontProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                               const FX_WCHAR* pszValue,
                                               int32_t iValueLen,
                                               FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, '/');
  IFDE_CSSPrimitiveValue* pStyle = nullptr;
  IFDE_CSSPrimitiveValue* pVariant = nullptr;
  IFDE_CSSPrimitiveValue* pWeight = nullptr;
  IFDE_CSSPrimitiveValue* pFontSize = nullptr;
  IFDE_CSSPrimitiveValue* pLineHeight = nullptr;
  CFDE_CSSValueArray familyList;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pValue) {
          switch (pValue->eName) {
            case FDE_CSSPROPERTYVALUE_XxSmall:
            case FDE_CSSPROPERTYVALUE_XSmall:
            case FDE_CSSPROPERTYVALUE_Small:
            case FDE_CSSPROPERTYVALUE_Medium:
            case FDE_CSSPROPERTYVALUE_Large:
            case FDE_CSSPROPERTYVALUE_XLarge:
            case FDE_CSSPROPERTYVALUE_XxLarge:
            case FDE_CSSPROPERTYVALUE_Smaller:
            case FDE_CSSPROPERTYVALUE_Larger:
              if (!pFontSize)
                pFontSize = NewEnumValue(pStaticStore, pValue->eName);
              continue;
            case FDE_CSSPROPERTYVALUE_Bold:
            case FDE_CSSPROPERTYVALUE_Bolder:
            case FDE_CSSPROPERTYVALUE_Lighter:
              if (!pWeight)
                pWeight = NewEnumValue(pStaticStore, pValue->eName);
              continue;
            case FDE_CSSPROPERTYVALUE_Italic:
            case FDE_CSSPROPERTYVALUE_Oblique:
              if (!pStyle)
                pStyle = NewEnumValue(pStaticStore, pValue->eName);
              continue;
            case FDE_CSSPROPERTYVALUE_SmallCaps:
              if (!pVariant)
                pVariant = NewEnumValue(pStaticStore, pValue->eName);
              continue;
            case FDE_CSSPROPERTYVALUE_Normal:
              if (!pStyle)
                pStyle = NewEnumValue(pStaticStore, pValue->eName);
              else if (!pVariant)
                pVariant = NewEnumValue(pStaticStore, pValue->eName);
              else if (!pWeight)
                pWeight = NewEnumValue(pStaticStore, pValue->eName);
              else if (!pFontSize)
                pFontSize = NewEnumValue(pStaticStore, pValue->eName);
              else if (!pLineHeight)
                pLineHeight = NewEnumValue(pStaticStore, pValue->eName);
              continue;
            default:
              break;
          }
        }
        if (pFontSize) {
          familyList.Add(FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(
              eType, CopyToLocal(pArgs, pszValue, iValueLen)));
        }
        parser.m_Separator = ',';
      } break;
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (!FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
          break;
        }
        if (eType == FDE_CSSPRIMITIVETYPE_Number) {
          switch ((int32_t)fValue) {
            case 100:
            case 200:
            case 300:
            case 400:
            case 500:
            case 600:
            case 700:
            case 800:
            case 900:
              if (!pWeight) {
                pWeight = NewNumberValue(pStaticStore,
                                         FDE_CSSPRIMITIVETYPE_Number, fValue);
              }
              continue;
          }
        }
        if (!pFontSize)
          pFontSize = NewNumberValue(pStaticStore, eType, fValue);
        else if (!pLineHeight)
          pLineHeight = NewNumberValue(pStaticStore, eType, fValue);
      } break;
      default:
        break;
    }
  }
  if (!pStyle)
    pStyle = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Normal);
  if (!pVariant)
    pVariant = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Normal);
  if (!pWeight)
    pWeight = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Normal);
  if (!pFontSize)
    pFontSize = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Medium);
  if (!pLineHeight)
    pLineHeight = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Normal);

  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_FontStyle, pStyle,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_FontVariant, pVariant,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_FontWeight, pWeight,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_FontSize, pFontSize,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_LineHeight, pLineHeight,
                    bImportant);
  if (familyList.GetSize() > 0) {
    CFDE_CSSValueList* pList = FXTARGET_NewWith(pStaticStore)
        CFDE_CSSValueList(pStaticStore, familyList);
    AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_FontFamily, pList,
                      bImportant);
  }
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseColumnRuleProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  IFDE_CSSPrimitiveValue* pColumnRuleWidth = nullptr;
  IFDE_CSSPrimitiveValue* pColumnRuleStyle = nullptr;
  IFDE_CSSPrimitiveValue* pColumnRuleColor = nullptr;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pValue) {
          switch (pValue->eName) {
            case FDE_CSSPROPERTYVALUE_None:
            case FDE_CSSPROPERTYVALUE_Hidden:
            case FDE_CSSPROPERTYVALUE_Dotted:
            case FDE_CSSPROPERTYVALUE_Dashed:
            case FDE_CSSPROPERTYVALUE_Solid:
            case FDE_CSSPROPERTYVALUE_Double:
            case FDE_CSSPROPERTYVALUE_Groove:
            case FDE_CSSPROPERTYVALUE_Ridge:
            case FDE_CSSPROPERTYVALUE_Inset:
            case FDE_CSSPROPERTYVALUE_Outset:
              if (!pColumnRuleStyle)
                pColumnRuleStyle = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Transparent:
              if (!pColumnRuleColor)
                pColumnRuleColor = NewEnumValue(pStaticStore, pValue->eName);
              break;
            case FDE_CSSPROPERTYVALUE_Thin:
            case FDE_CSSPROPERTYVALUE_Medium:
            case FDE_CSSPROPERTYVALUE_Thick:
              if (!pColumnRuleWidth)
                pColumnRuleWidth = NewEnumValue(pStaticStore, pValue->eName);
              break;
            default:
              break;
          }
          continue;
        }
        FX_ARGB dwColor;
        if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor) &&
            !pColumnRuleColor) {
          pColumnRuleColor = FXTARGET_NewWith(pStaticStore)
              CFDE_CSSPrimitiveValue((FX_ARGB)dwColor);
          continue;
        }
      } break;
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType) &&
            !pColumnRuleWidth) {
          pColumnRuleWidth = NewNumberValue(pStaticStore, eType, fValue);
        }
      } break;
      case FDE_CSSPRIMITIVETYPE_RGB: {
        FX_ARGB dwColor;
        if (!pColumnRuleColor &&
            FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
          pColumnRuleColor = FXTARGET_NewWith(pStaticStore)
              CFDE_CSSPrimitiveValue((FX_ARGB)dwColor);
        }
      } break;
      default:
        break;
    }
  }
  if (!pColumnRuleColor && !pColumnRuleStyle && !pColumnRuleWidth)
    return FALSE;

  if (!pColumnRuleStyle)
    pColumnRuleStyle = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_None);
  if (!pColumnRuleWidth)
    pColumnRuleWidth = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Medium);
  if (!pColumnRuleColor) {
    pColumnRuleColor =
        FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue((FX_ARGB)0);
  }
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ColumnRuleStyle,
                    pColumnRuleStyle, bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ColumnRuleWidth,
                    pColumnRuleWidth, bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ColumnRuleColor,
                    pColumnRuleColor, bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseTextEmphasisProperty(
    FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  CFDE_CSSValueArray arrEmphasisStyle;
  FDE_CSSPRIMITIVETYPE eType;
  IFDE_CSSPrimitiveValue* pEmphasisColor = nullptr;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (pValue) {
          arrEmphasisStyle.Add(NewEnumValue(pStaticStore, pValue->eName));
          continue;
        }
        FX_ARGB dwColor;
        if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
          pEmphasisColor =
              FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(dwColor);
          continue;
        }
        pszValue = CopyToLocal(pArgs, pszValue, iValueLen);
        arrEmphasisStyle.Add(
            FXTARGET_NewWith(pStaticStore)
                CFDE_CSSPrimitiveValue(FDE_CSSPRIMITIVETYPE_String, pszValue));
      } break;
      case FDE_CSSPRIMITIVETYPE_RGB: {
        FX_ARGB dwColor;
        if (FDE_ParseCSSColor(pszValue, iValueLen, dwColor)) {
          pEmphasisColor =
              FXTARGET_NewWith(pStaticStore) CFDE_CSSPrimitiveValue(dwColor);
        }
      } break;
      default:
        break;
    }
  }
  if (arrEmphasisStyle.GetSize() != 0) {
    AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_TextEmphasisStyle,
                      FXTARGET_NewWith(pStaticStore)
                          CFDE_CSSValueList(pStaticStore, arrEmphasisStyle),
                      bImportant);
  }
  if (pEmphasisColor) {
    AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_TextEmphasisColor,
                      pEmphasisColor, bImportant);
  }
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseColumnsProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  IFDE_CSSPrimitiveValue* pColumnWidth = nullptr;
  IFDE_CSSPrimitiveValue* pColumnCount = nullptr;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    switch (eType) {
      case FDE_CSSPRIMITIVETYPE_String: {
        const FDE_CSSPROPERTYVALUETABLE* pValue =
            FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
        if (!pValue && pValue->eName == FDE_CSSPROPERTYVALUE_Auto) {
          pColumnWidth = NewEnumValue(pStaticStore, pValue->eName);
        }
      } break;
      case FDE_CSSPRIMITIVETYPE_Number: {
        FX_FLOAT fValue;
        if (FDE_ParseCSSNumber(pszValue, iValueLen, fValue, eType)) {
          switch (eType) {
            case FDE_CSSPRIMITIVETYPE_Number:
              if (!pColumnCount)
                pColumnCount = NewNumberValue(pStaticStore, eType, fValue);
              break;
            default:
              if (!pColumnWidth)
                pColumnWidth = NewNumberValue(pStaticStore, eType, fValue);
              break;
          }
        }
      } break;
      default:
        break;
    }
  }
  if (!pColumnWidth && !pColumnCount)
    return FALSE;

  if (!pColumnWidth)
    pColumnWidth = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Auto);
  else if (!pColumnCount)
    pColumnCount = NewEnumValue(pStaticStore, FDE_CSSPROPERTYVALUE_Auto);

  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ColumnWidth, pColumnWidth,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_ColumnCount, pColumnCount,
                    bImportant);
  return TRUE;
}
FX_BOOL CFDE_CSSDeclaration::ParseOverflowProperty(
    const FDE_CSSPROPERTYARGS* pArgs,
    const FX_WCHAR* pszValue,
    int32_t iValueLen,
    FX_BOOL bImportant) {
  IFX_MemoryAllocator* pStaticStore = pArgs->pStaticStore;
  CFDE_CSSValueListParser parser(pszValue, iValueLen, ' ');
  IFDE_CSSPrimitiveValue* pOverflowX = nullptr;
  IFDE_CSSPrimitiveValue* pOverflowY = nullptr;
  FDE_CSSPRIMITIVETYPE eType;
  while (parser.NextValue(eType, pszValue, iValueLen)) {
    if (eType == FDE_CSSPRIMITIVETYPE_String) {
      const FDE_CSSPROPERTYVALUETABLE* pValue =
          FDE_GetCSSPropertyValueByName(CFX_WideStringC(pszValue, iValueLen));
      if (pValue) {
        switch (pValue->eName) {
          case FDE_CSSOVERFLOW_Visible:
          case FDE_CSSOVERFLOW_Hidden:
          case FDE_CSSOVERFLOW_Scroll:
          case FDE_CSSOVERFLOW_Auto:
          case FDE_CSSOVERFLOW_NoDisplay:
          case FDE_CSSOVERFLOW_NoContent:
            if (pOverflowX && pOverflowY)
              return FALSE;
            if (!pOverflowX) {
              pOverflowX = NewEnumValue(pStaticStore, pValue->eName);
            } else if (!pOverflowY) {
              pOverflowY = NewEnumValue(pStaticStore, pValue->eName);
            }
            break;
          default:
            break;
        }
      }
    }
  }
  if (!pOverflowX && !pOverflowY)
    return FALSE;

  if (!pOverflowY)
    pOverflowY = NewEnumValue(pStaticStore, pOverflowX->GetEnum());

  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_OverflowX, pOverflowX,
                    bImportant);
  AddPropertyHolder(pStaticStore, FDE_CSSPROPERTY_OverflowY, pOverflowY,
                    bImportant);
  return TRUE;
}
