// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CSS_FDE_CSSDATATABLE_H_
#define XFA_FDE_CSS_FDE_CSSDATATABLE_H_

#include "core/fxcrt/include/fx_system.h"
#include "xfa/fde/css/fde_css.h"
#include "xfa/fgas/crt/fgas_memory.h"

class CFDE_CSSFunction : public CFX_Target {
 public:
  CFDE_CSSFunction(const FX_WCHAR* pszFuncName, IFDE_CSSValueList* pArgList)
      : m_pArgList(pArgList), m_pszFuncName(pszFuncName) {
    ASSERT(pArgList);
  }
  int32_t CountArgs() const { return m_pArgList->CountValues(); }
  IFDE_CSSValue* GetArgs(int32_t index) const {
    return m_pArgList->GetValue(index);
  }
  const FX_WCHAR* GetFuncName() const { return m_pszFuncName; }

 protected:
  IFDE_CSSValueList* m_pArgList;
  const FX_WCHAR* m_pszFuncName;
};

class CFDE_CSSPrimitiveValue : public IFDE_CSSPrimitiveValue,
                               public CFX_Target {
 public:
  explicit CFDE_CSSPrimitiveValue(FX_ARGB color);
  explicit CFDE_CSSPrimitiveValue(FDE_CSSPROPERTYVALUE eValue);
  explicit CFDE_CSSPrimitiveValue(CFDE_CSSFunction* pFunction);
  CFDE_CSSPrimitiveValue(FDE_CSSPRIMITIVETYPE eType, FX_FLOAT fValue);
  CFDE_CSSPrimitiveValue(FDE_CSSPRIMITIVETYPE eType, const FX_WCHAR* pValue);
  CFDE_CSSPrimitiveValue(const CFDE_CSSPrimitiveValue& src);

  // IFDE_CSSPrimitiveValue
  FDE_CSSPRIMITIVETYPE GetPrimitiveType() const override;
  FX_ARGB GetRGBColor() const override;
  FX_FLOAT GetFloat() const override;
  const FX_WCHAR* GetString(int32_t& iLength) const override;
  FDE_CSSPROPERTYVALUE GetEnum() const override;
  const FX_WCHAR* GetFuncName() const override;
  int32_t CountArgs() const override;
  IFDE_CSSValue* GetArgs(int32_t index) const override;

  FDE_CSSPRIMITIVETYPE m_eType;
  union {
    FX_ARGB m_dwColor;
    FX_FLOAT m_fNumber;
    const FX_WCHAR* m_pString;
    FDE_CSSPROPERTYVALUE m_eEnum;
    CFDE_CSSFunction* m_pFunction;
  };
};

typedef CFX_ArrayTemplate<IFDE_CSSPrimitiveValue*> CFDE_CSSPrimitiveArray;
typedef CFX_ArrayTemplate<IFDE_CSSValue*> CFDE_CSSValueArray;

class CFDE_CSSValueList : public IFDE_CSSValueList, public CFX_Target {
 public:
  CFDE_CSSValueList(IFX_MemoryAllocator* pStaticStore,
                    const CFDE_CSSValueArray& list);

  // IFDE_CSSValueList
  int32_t CountValues() const override;
  IFDE_CSSValue* GetValue(int32_t index) const override;

 protected:
  IFDE_CSSValue** m_ppList;
  int32_t m_iCount;
};

class CFDE_CSSValueListParser : public CFX_Target {
 public:
  CFDE_CSSValueListParser(const FX_WCHAR* psz, int32_t iLen, FX_WCHAR separator)
      : m_Separator(separator), m_pCur(psz), m_pEnd(psz + iLen) {
    ASSERT(psz && iLen > 0);
  }
  FX_BOOL NextValue(FDE_CSSPRIMITIVETYPE& eType,
                    const FX_WCHAR*& pStart,
                    int32_t& iLength);
  FX_WCHAR m_Separator;

 protected:
  int32_t SkipTo(FX_WCHAR wch,
                 FX_BOOL bWSSeparator = FALSE,
                 FX_BOOL bBrContinue = FALSE);

  const FX_WCHAR* m_pCur;
  const FX_WCHAR* m_pEnd;
};

#define FDE_CSSVALUETYPE_MaybeNumber 0x0100
#define FDE_CSSVALUETYPE_MaybeEnum 0x0200
#define FDE_CSSVALUETYPE_MaybeURI 0x0400
#define FDE_CSSVALUETYPE_MaybeString 0x0800
#define FDE_CSSVALUETYPE_MaybeColor 0x1000
#define FDE_CSSVALUETYPE_MaybeFunction 0x2000
#define FDE_IsOnlyValue(type, enum) \
  (((type) & ~(enum)) == FDE_CSSVALUETYPE_Primitive)

struct FDE_CSSPROPERTYTABLE {
  FDE_CSSPROPERTY eName;
  const FX_WCHAR* pszName;
  uint32_t dwHash;
  uint32_t dwType;
};

const FDE_CSSPROPERTYTABLE* FDE_GetCSSPropertyByName(
    const CFX_WideStringC& wsName);
const FDE_CSSPROPERTYTABLE* FDE_GetCSSPropertyByEnum(FDE_CSSPROPERTY eName);

struct FDE_CSSPROPERTYVALUETABLE {
  FDE_CSSPROPERTYVALUE eName;
  const FX_WCHAR* pszName;
  uint32_t dwHash;
};

const FDE_CSSPROPERTYVALUETABLE* FDE_GetCSSPropertyValueByName(
    const CFX_WideStringC& wsName);
const FDE_CSSPROPERTYVALUETABLE* FDE_GetCSSPropertyValueByEnum(
    FDE_CSSPROPERTYVALUE eName);

struct FDE_CSSMEDIATYPETABLE {
  uint16_t wHash;
  uint16_t wValue;
};
typedef FDE_CSSMEDIATYPETABLE const* FDE_LPCCSSMEDIATYPETABLE;
FDE_LPCCSSMEDIATYPETABLE FDE_GetCSSMediaTypeByName(
    const CFX_WideStringC& wsName);
struct FDE_CSSLENGTHUNITTABLE {
  uint16_t wHash;
  uint16_t wValue;
};
typedef FDE_CSSLENGTHUNITTABLE const* FDE_LPCCSSLENGTHUNITTABLE;
FDE_LPCCSSLENGTHUNITTABLE FDE_GetCSSLengthUnitByName(
    const CFX_WideStringC& wsName);
struct FDE_CSSCOLORTABLE {
  uint32_t dwHash;
  FX_ARGB dwValue;
};
typedef FDE_CSSCOLORTABLE const* FDE_LPCCSSCOLORTABLE;
FDE_LPCCSSCOLORTABLE FDE_GetCSSColorByName(const CFX_WideStringC& wsName);

struct FDE_CSSPERSUDOTABLE {
  FDE_CSSPERSUDO eName;
  const FX_WCHAR* pszName;
  uint32_t dwHash;
};
typedef FDE_CSSPERSUDOTABLE const* FDE_LPCCSSPERSUDOTABLE;

FDE_LPCCSSPERSUDOTABLE FDE_GetCSSPersudoByEnum(FDE_CSSPERSUDO ePersudo);
FX_BOOL FDE_ParseCSSNumber(const FX_WCHAR* pszValue,
                           int32_t iValueLen,
                           FX_FLOAT& fValue,
                           FDE_CSSPRIMITIVETYPE& eUnit);
FX_BOOL FDE_ParseCSSString(const FX_WCHAR* pszValue,
                           int32_t iValueLen,
                           int32_t& iOffset,
                           int32_t& iLength);
FX_BOOL FDE_ParseCSSColor(const FX_WCHAR* pszValue,
                          int32_t iValueLen,
                          FX_ARGB& dwColor);
FX_BOOL FDE_ParseCSSURI(const FX_WCHAR* pszValue,
                        int32_t iValueLen,
                        int32_t& iOffset,
                        int32_t& iLength);

#endif  // XFA_FDE_CSS_FDE_CSSDATATABLE_H_
