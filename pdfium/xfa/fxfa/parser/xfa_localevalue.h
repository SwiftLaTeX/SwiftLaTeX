// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_LOCALEVALUE_H_
#define XFA_FXFA_PARSER_XFA_LOCALEVALUE_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "xfa/fxfa/parser/cxfa_widgetdata.h"

class IFX_Locale;
class CFX_Unitime;
class CXFA_LocaleMgr;

#define XFA_VT_NULL 0
#define XFA_VT_BOOLEAN 1
#define XFA_VT_INTEGER 2
#define XFA_VT_DECIMAL 4
#define XFA_VT_FLOAT 8
#define XFA_VT_TEXT 16
#define XFA_VT_DATE 32
#define XFA_VT_TIME 64
#define XFA_VT_DATETIME 128

class CXFA_LocaleValue {
 public:
  CXFA_LocaleValue();
  CXFA_LocaleValue(const CXFA_LocaleValue& value);
  CXFA_LocaleValue(uint32_t dwType, CXFA_LocaleMgr* pLocaleMgr);
  CXFA_LocaleValue(uint32_t dwType,
                   const CFX_WideString& wsValue,
                   CXFA_LocaleMgr* pLocaleMgr);
  CXFA_LocaleValue(uint32_t dwType,
                   const CFX_WideString& wsValue,
                   const CFX_WideString& wsFormat,
                   IFX_Locale* pLocale,
                   CXFA_LocaleMgr* pLocaleMgr);
  ~CXFA_LocaleValue();
  CXFA_LocaleValue& operator=(const CXFA_LocaleValue& value);

  FX_BOOL ValidateValue(const CFX_WideString& wsValue,
                        const CFX_WideString& wsPattern,
                        IFX_Locale* pLocale,
                        CFX_WideString* pMatchFormat = nullptr);
  FX_BOOL FormatPatterns(CFX_WideString& wsResult,
                         const CFX_WideString& wsFormat,
                         IFX_Locale* pLocale,
                         XFA_VALUEPICTURE eValueType) const;
  FX_BOOL FormatSinglePattern(CFX_WideString& wsResult,
                              const CFX_WideString& wsFormat,
                              IFX_Locale* pLocale,
                              XFA_VALUEPICTURE eValueType) const;
  FX_BOOL ValidateCanonicalValue(const CFX_WideString& wsValue,
                                 uint32_t dwVType);
  FX_BOOL ValidateCanonicalDate(const CFX_WideString& wsDate,
                                CFX_Unitime& unDate);
  FX_BOOL ValidateCanonicalTime(const CFX_WideString& wsTime);
  FX_BOOL ValidateCanonicalDateTime(const CFX_WideString& wsDateTime);
  void GetNumbericFormat(CFX_WideString& wsFormat,
                         int32_t nIntLen,
                         int32_t nDecLen,
                         FX_BOOL bSign = TRUE);
  FX_BOOL ValidateNumericTemp(CFX_WideString& wsNumeric,
                              CFX_WideString& wsFormat,
                              IFX_Locale* pLocale = nullptr,
                              int32_t* pos = nullptr);

  CFX_WideString GetValue() const;
  uint32_t GetType() const;
  void SetValue(const CFX_WideString& wsValue, uint32_t dwType);
  CFX_WideString GetText() const;
  FX_FLOAT GetNum() const;
  FX_DOUBLE GetDoubleNum() const;
  CFX_Unitime GetDate() const;
  CFX_Unitime GetTime() const;
  CFX_Unitime GetDateTime() const;
  FX_BOOL SetText(const CFX_WideString& wsText);
  FX_BOOL SetText(const CFX_WideString& wsText,
                  const CFX_WideString& wsFormat,
                  IFX_Locale* pLocale);
  FX_BOOL SetNum(FX_FLOAT fNum);
  FX_BOOL SetNum(const CFX_WideString& wsNum,
                 const CFX_WideString& wsFormat,
                 IFX_Locale* pLocale);
  FX_BOOL SetDate(const CFX_Unitime& d);
  FX_BOOL SetDate(const CFX_WideString& wsDate,
                  const CFX_WideString& wsFormat,
                  IFX_Locale* pLocale);
  FX_BOOL SetTime(const CFX_Unitime& t);
  FX_BOOL SetTime(const CFX_WideString& wsTime,
                  const CFX_WideString& wsFormat,
                  IFX_Locale* pLocale);
  FX_BOOL SetDateTime(const CFX_Unitime& dt);
  FX_BOOL SetDateTime(const CFX_WideString& wsDateTime,
                      const CFX_WideString& wsFormat,
                      IFX_Locale* pLocale);
  bool IsNull() const { return m_dwType == XFA_VT_NULL; }
  FX_BOOL IsEmpty() const { return m_wsValue.IsEmpty(); }
  FX_BOOL IsValid() const { return m_bValid; }

 protected:
  FX_BOOL ParsePatternValue(const CFX_WideString& wsValue,
                            const CFX_WideString& wsPattern,
                            IFX_Locale* pLocale);
  CXFA_LocaleMgr* m_pLocaleMgr;
  CFX_WideString m_wsValue;
  uint32_t m_dwType;
  FX_BOOL m_bValid;
};

#endif  // XFA_FXFA_PARSER_XFA_LOCALEVALUE_H_
