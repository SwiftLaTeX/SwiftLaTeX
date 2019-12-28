// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LOCALIZATION_FGAS_LOCALEIMP_H_
#define XFA_FGAS_LOCALIZATION_FGAS_LOCALEIMP_H_

#include "xfa/fgas/localization/fgas_locale.h"

class CFX_LCNumeric;

class CFX_FormatString {
 public:
  CFX_FormatString(IFX_LocaleMgr* pLocaleMgr, FX_BOOL bUseLCID);

  void Release() { delete this; }

  void SplitFormatString(const CFX_WideString& wsFormatString,
                         CFX_WideStringArray& wsPatterns);
  FX_LOCALECATEGORY GetCategory(const CFX_WideString& wsPattern);
  uint16_t GetLCID(const CFX_WideString& wsPattern);
  CFX_WideString GetLocaleName(const CFX_WideString& wsPattern);
  FX_BOOL ParseText(const CFX_WideString& wsSrcText,
                    const CFX_WideString& wsPattern,
                    CFX_WideString& wsValue);
  FX_BOOL ParseNum(const CFX_WideString& wsSrcNum,
                   const CFX_WideString& wsPattern,
                   FX_FLOAT& fValue);
  FX_BOOL ParseNum(const CFX_WideString& wsSrcNum,
                   const CFX_WideString& wsPattern,
                   CFX_WideString& wsValue);
  FX_BOOL ParseDateTime(const CFX_WideString& wsSrcDateTime,
                        const CFX_WideString& wsPattern,
                        FX_DATETIMETYPE eDateTimeType,
                        CFX_Unitime& dtValue);
  FX_BOOL ParseZero(const CFX_WideString& wsSrcText,
                    const CFX_WideString& wsPattern);
  FX_BOOL ParseNull(const CFX_WideString& wsSrcText,
                    const CFX_WideString& wsPattern);
  FX_BOOL FormatText(const CFX_WideString& wsSrcText,
                     const CFX_WideString& wsPattern,
                     CFX_WideString& wsOutput);
  FX_BOOL FormatNum(const CFX_WideString& wsSrcNum,
                    const CFX_WideString& wsPattern,
                    CFX_WideString& wsOutput);
  FX_BOOL FormatNum(FX_FLOAT fNum,
                    const CFX_WideString& wsPattern,
                    CFX_WideString& wsOutput);
  FX_BOOL FormatDateTime(const CFX_WideString& wsSrcDateTime,
                         const CFX_WideString& wsPattern,
                         CFX_WideString& wsOutput);
  FX_BOOL FormatDateTime(const CFX_WideString& wsSrcDateTime,
                         const CFX_WideString& wsPattern,
                         CFX_WideString& wsOutput,
                         FX_DATETIMETYPE eDateTimeType);
  FX_BOOL FormatDateTime(const CFX_Unitime& dt,
                         const CFX_WideString& wsPattern,
                         CFX_WideString& wsOutput);
  FX_BOOL FormatZero(const CFX_WideString& wsPattern, CFX_WideString& wsOutput);
  FX_BOOL FormatNull(const CFX_WideString& wsPattern, CFX_WideString& wsOutput);

 protected:
  ~CFX_FormatString();

  IFX_Locale* GetTextFormat(const CFX_WideString& wsPattern,
                            const CFX_WideStringC& wsCategory,
                            CFX_WideString& wsPurgePattern);
  IFX_Locale* GetNumericFormat(const CFX_WideString& wsPattern,
                               int32_t& iDotIndex,
                               uint32_t& dwStyle,
                               CFX_WideString& wsPurgePattern);
  FX_BOOL FormatStrNum(const CFX_WideStringC& wsInputNum,
                       const CFX_WideString& wsPattern,
                       CFX_WideString& wsOutput);
  FX_BOOL FormatLCNumeric(CFX_LCNumeric& lcNum,
                          const CFX_WideString& wsPattern,
                          CFX_WideString& wsOutput);
  FX_DATETIMETYPE GetDateTimeFormat(const CFX_WideString& wsPattern,
                                    IFX_Locale*& pLocale,
                                    CFX_WideString& wsDatePattern,
                                    CFX_WideString& wsTimePattern);
  IFX_Locale* GetPatternLocale(const CFX_WideString& wsLocale);

  IFX_LocaleMgr* m_pLocaleMgr;
  FX_BOOL m_bUseLCID;
};

#endif  // XFA_FGAS_LOCALIZATION_FGAS_LOCALEIMP_H_
