// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FM2JS_XFA_FM2JSCONTEXT_H_
#define XFA_FXFA_FM2JS_XFA_FM2JSCONTEXT_H_

#include "fxjs/include/cfxjse_arguments.h"
#include "fxjs/include/cfxjse_context.h"
#include "xfa/fxfa/parser/xfa_script.h"

class CXFA_FM2JSContext : public CFXJSE_HostObject {
 public:
  CXFA_FM2JSContext(v8::Isolate* pScriptIsolate,
                    CFXJSE_Context* pScriptContext,
                    CXFA_Document* pDoc);
  ~CXFA_FM2JSContext() override;

  static void Abs(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Avg(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Ceil(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Count(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Floor(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Max(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Min(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Mod(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Round(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Sum(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Date(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Date2Num(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void DateFmt(CFXJSE_Value* pThis,
                      const CFX_ByteStringC& szFuncName,
                      CFXJSE_Arguments& args);
  static void IsoDate2Num(CFXJSE_Value* pThis,
                          const CFX_ByteStringC& szFuncName,
                          CFXJSE_Arguments& args);
  static void IsoTime2Num(CFXJSE_Value* pThis,
                          const CFX_ByteStringC& szFuncName,
                          CFXJSE_Arguments& args);
  static void LocalDateFmt(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void LocalTimeFmt(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void Num2Date(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void Num2GMTime(CFXJSE_Value* pThis,
                         const CFX_ByteStringC& szFuncName,
                         CFXJSE_Arguments& args);
  static void Num2Time(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void Time(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Time2Num(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void TimeFmt(CFXJSE_Value* pThis,
                      const CFX_ByteStringC& szFuncName,
                      CFXJSE_Arguments& args);

  static FX_BOOL IsIsoDateFormat(const FX_CHAR* pData,
                                 int32_t iLength,
                                 int32_t& iStyle,
                                 int32_t& iYear,
                                 int32_t& iMonth,
                                 int32_t& iDay);
  static FX_BOOL IsIsoTimeFormat(const FX_CHAR* pData,
                                 int32_t iLength,
                                 int32_t& iHour,
                                 int32_t& iMinute,
                                 int32_t& iSecond,
                                 int32_t& iMilliSecond,
                                 int32_t& iZoneHour,
                                 int32_t& iZoneMinute);
  static FX_BOOL IsIsoDateTimeFormat(const FX_CHAR* pData,
                                     int32_t iLength,
                                     int32_t& iYear,
                                     int32_t& iMonth,
                                     int32_t& iDay,
                                     int32_t& iHour,
                                     int32_t& iMinute,
                                     int32_t& iSecond,
                                     int32_t& iMillionSecond,
                                     int32_t& iZoneHour,
                                     int32_t& iZoneMinute);
  static FX_BOOL Local2IsoDate(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szDate,
                               const CFX_ByteStringC& szFormat,
                               const CFX_ByteStringC& szLocale,
                               CFX_ByteString& strIsoDate);
  static FX_BOOL Local2IsoTime(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szTime,
                               const CFX_ByteStringC& szFormat,
                               const CFX_ByteStringC& szLocale,
                               CFX_ByteString& strIsoTime);
  static FX_BOOL IsoDate2Local(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szDate,
                               const CFX_ByteStringC& szFormat,
                               const CFX_ByteStringC& szLocale,
                               CFX_ByteString& strLocalDate);
  static FX_BOOL IsoTime2Local(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szTime,
                               const CFX_ByteStringC& szFormat,
                               const CFX_ByteStringC& szLocale,
                               CFX_ByteString& strLocalTime);
  static FX_BOOL GetGMTTime(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szTime,
                            const CFX_ByteStringC& szFormat,
                            const CFX_ByteStringC& szLocale,
                            CFX_ByteString& strGMTTime);
  static int32_t DateString2Num(const CFX_ByteStringC& szDateString);
  static void GetLocalDateFormat(CFXJSE_Value* pThis,
                                 int32_t iStyle,
                                 const CFX_ByteStringC& szLocalStr,
                                 CFX_ByteString& strFormat,
                                 FX_BOOL bStandard);
  static void GetLocalTimeFormat(CFXJSE_Value* pThis,
                                 int32_t iStyle,
                                 const CFX_ByteStringC& szLocalStr,
                                 CFX_ByteString& strFormat,
                                 FX_BOOL bStandard);
  static void GetStandardDateFormat(CFXJSE_Value* pThis,
                                    int32_t iStyle,
                                    const CFX_ByteStringC& szLocalStr,
                                    CFX_ByteString& strFormat);
  static void GetStandardTimeFormat(CFXJSE_Value* pThis,
                                    int32_t iStyle,
                                    const CFX_ByteStringC& szLocalStr,
                                    CFX_ByteString& strFormat);

  static void Num2AllTime(CFXJSE_Value* pThis,
                          int32_t iTime,
                          const CFX_ByteStringC& szFormat,
                          const CFX_ByteStringC& szLocale,
                          FX_BOOL bGM,
                          CFX_ByteString& strTime);
  static void GetLocalTimeZone(int32_t& iHour, int32_t& iMin, int32_t& iSec);

  static void Apr(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void CTerm(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void FV(CFXJSE_Value* pThis,
                 const CFX_ByteStringC& szFuncName,
                 CFXJSE_Arguments& args);
  static void IPmt(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void NPV(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Pmt(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void PPmt(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void PV(CFXJSE_Value* pThis,
                 const CFX_ByteStringC& szFuncName,
                 CFXJSE_Arguments& args);
  static void Rate(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Term(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Choose(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void Exists(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void HasValue(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void Oneof(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Within(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void If(CFXJSE_Value* pThis,
                 const CFX_ByteStringC& szFuncName,
                 CFXJSE_Arguments& args);
  static void Eval(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Ref(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void UnitType(CFXJSE_Value* pThis,
                       const CFX_ByteStringC& szFuncName,
                       CFXJSE_Arguments& args);
  static void UnitValue(CFXJSE_Value* pThis,
                        const CFX_ByteStringC& szFuncName,
                        CFXJSE_Arguments& args);

  static void At(CFXJSE_Value* pThis,
                 const CFX_ByteStringC& szFuncName,
                 CFXJSE_Arguments& args);
  static void Concat(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void Decode(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void DecodeURL(const CFX_ByteStringC& szURLString,
                        CFX_ByteTextBuf& szResultBuf);
  static void DecodeHTML(const CFX_ByteStringC& szHTMLString,
                         CFX_ByteTextBuf& szResultBuf);
  static void DecodeXML(const CFX_ByteStringC& szXMLString,
                        CFX_ByteTextBuf& szResultBuf);
  static void Encode(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void EncodeURL(const CFX_ByteStringC& szURLString,
                        CFX_ByteTextBuf& szResultBuf);
  static void EncodeHTML(const CFX_ByteStringC& szHTMLString,
                         CFX_ByteTextBuf& szResultBuf);
  static void EncodeXML(const CFX_ByteStringC& szXMLString,
                        CFX_ByteTextBuf& szResultBuf);
  static FX_BOOL HTMLSTR2Code(const CFX_WideStringC& pData, uint32_t& iCode);
  static FX_BOOL HTMLCode2STR(uint32_t iCode, CFX_WideString& wsHTMLReserve);
  static void Format(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void Left(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Len(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Lower(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Ltrim(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Parse(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Replace(CFXJSE_Value* pThis,
                      const CFX_ByteStringC& szFuncName,
                      CFXJSE_Arguments& args);
  static void Right(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Rtrim(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Space(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Str(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Stuff(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void Substr(CFXJSE_Value* pThis,
                     const CFX_ByteStringC& szFuncName,
                     CFXJSE_Arguments& args);
  static void Uuid(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Upper(CFXJSE_Value* pThis,
                    const CFX_ByteStringC& szFuncName,
                    CFXJSE_Arguments& args);
  static void WordNum(CFXJSE_Value* pThis,
                      const CFX_ByteStringC& szFuncName,
                      CFXJSE_Arguments& args);
  static void TrillionUS(const CFX_ByteStringC& szData,
                         CFX_ByteTextBuf& strBuf);
  static void WordUS(const CFX_ByteStringC& szData,
                     int32_t iStyle,
                     CFX_ByteTextBuf& strBuf);

  static void Get(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void Post(CFXJSE_Value* pThis,
                   const CFX_ByteStringC& szFuncName,
                   CFXJSE_Arguments& args);
  static void Put(CFXJSE_Value* pThis,
                  const CFX_ByteStringC& szFuncName,
                  CFXJSE_Arguments& args);
  static void assign_value_operator(CFXJSE_Value* pThis,
                                    const CFX_ByteStringC& szFuncName,
                                    CFXJSE_Arguments& args);
  static void logical_or_operator(CFXJSE_Value* pThis,
                                  const CFX_ByteStringC& szFuncName,
                                  CFXJSE_Arguments& args);
  static void logical_and_operator(CFXJSE_Value* pThis,
                                   const CFX_ByteStringC& szFuncName,
                                   CFXJSE_Arguments& args);
  static void equality_operator(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args);
  static void notequality_operator(CFXJSE_Value* pThis,
                                   const CFX_ByteStringC& szFuncName,
                                   CFXJSE_Arguments& args);
  static FX_BOOL fm_ref_equal(CFXJSE_Value* pThis, CFXJSE_Arguments& args);
  static void less_operator(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args);
  static void lessequal_operator(CFXJSE_Value* pThis,
                                 const CFX_ByteStringC& szFuncName,
                                 CFXJSE_Arguments& args);
  static void greater_operator(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args);
  static void greaterequal_operator(CFXJSE_Value* pThis,
                                    const CFX_ByteStringC& szFuncName,
                                    CFXJSE_Arguments& args);
  static void plus_operator(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args);
  static void minus_operator(CFXJSE_Value* pThis,
                             const CFX_ByteStringC& szFuncName,
                             CFXJSE_Arguments& args);
  static void multiple_operator(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args);
  static void divide_operator(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args);
  static void positive_operator(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args);
  static void negative_operator(CFXJSE_Value* pThis,
                                const CFX_ByteStringC& szFuncName,
                                CFXJSE_Arguments& args);
  static void logical_not_operator(CFXJSE_Value* pThis,
                                   const CFX_ByteStringC& szFuncName,
                                   CFXJSE_Arguments& args);
  static void dot_accessor(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void dotdot_accessor(CFXJSE_Value* pThis,
                              const CFX_ByteStringC& szFuncName,
                              CFXJSE_Arguments& args);
  static void eval_translation(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args);
  static void is_fm_object(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void is_fm_array(CFXJSE_Value* pThis,
                          const CFX_ByteStringC& szFuncName,
                          CFXJSE_Arguments& args);
  static void get_fm_value(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void get_fm_jsobj(CFXJSE_Value* pThis,
                           const CFX_ByteStringC& szFuncName,
                           CFXJSE_Arguments& args);
  static void fm_var_filter(CFXJSE_Value* pThis,
                            const CFX_ByteStringC& szFuncName,
                            CFXJSE_Arguments& args);
  static void concat_fm_object(CFXJSE_Value* pThis,
                               const CFX_ByteStringC& szFuncName,
                               CFXJSE_Arguments& args);

  static int32_t hvalue_get_array_length(CFXJSE_Value* pThis,
                                         CFXJSE_Value* arg);
  static FX_BOOL simpleValueCompare(CFXJSE_Value* pThis,
                                    CFXJSE_Value* firstValue,
                                    CFXJSE_Value* secondValue);
  static void unfoldArgs(CFXJSE_Value* pThis,
                         CFXJSE_Arguments& args,
                         CFXJSE_Value**& resultValues,
                         int32_t& iCount,
                         int32_t iStart = 0);
  static void GetObjectDefaultValue(CFXJSE_Value* pObjectValue,
                                    CFXJSE_Value* pDefaultValue);
  static FX_BOOL SetObjectDefaultValue(CFXJSE_Value* pObjectValue,
                                       CFXJSE_Value* pNewValue);
  static void GenerateSomExpression(const CFX_ByteStringC& szName,
                                    int32_t iIndexFlags,
                                    int32_t iIndexValue,
                                    bool bIsStar,
                                    CFX_ByteString& szSomExp);
  static FX_BOOL GetObjectByName(CFXJSE_Value* pThis,
                                 CFXJSE_Value* accessorValue,
                                 const CFX_ByteStringC& szAccessorName);
  static int32_t ResolveObjects(CFXJSE_Value* pThis,
                                CFXJSE_Value* pParentValue,
                                const CFX_ByteStringC& bsSomExp,
                                XFA_RESOLVENODE_RS& resoveNodeRS,
                                FX_BOOL bdotAccessor = TRUE,
                                FX_BOOL bHasNoResolveName = FALSE);
  static void ParseResolveResult(CFXJSE_Value* pThis,
                                 const XFA_RESOLVENODE_RS& resoveNodeRS,
                                 CFXJSE_Value* pParentValue,
                                 CFXJSE_Value**& resultValues,
                                 int32_t& iSize,
                                 FX_BOOL& bAttribute);

  static std::unique_ptr<CFXJSE_Value> GetSimpleValue(CFXJSE_Value* pThis,
                                                      CFXJSE_Arguments& args,
                                                      uint32_t index);
  static FX_BOOL ValueIsNull(CFXJSE_Value* pThis, CFXJSE_Value* pValue);
  static int32_t ValueToInteger(CFXJSE_Value* pThis, CFXJSE_Value* pValue);
  static FX_FLOAT ValueToFloat(CFXJSE_Value* pThis, CFXJSE_Value* pValue);
  static FX_DOUBLE ValueToDouble(CFXJSE_Value* pThis, CFXJSE_Value* pValue);
  static void ValueToUTF8String(CFXJSE_Value* pValue,
                                CFX_ByteString& outputValue);
  static double ExtractDouble(CFXJSE_Value* pThis,
                              CFXJSE_Value* src,
                              bool* ret);

  static int32_t Translate(const CFX_WideStringC& wsFormcalc,
                           CFX_WideTextBuf& wsJavascript,
                           CFX_WideString& wsError);

  void GlobalPropertyGetter(CFXJSE_Value* pValue);

 private:
  v8::Isolate* GetScriptRuntime() const { return m_pIsolate; }
  CXFA_Document* GetDocument() const { return m_pDocument; }
  void ThrowException(int32_t iStringID, ...);

  v8::Isolate* m_pIsolate;
  CFXJSE_Class* m_pFMClass;
  std::unique_ptr<CFXJSE_Value> m_pValue;
  CXFA_Document* const m_pDocument;
};

#endif  // XFA_FXFA_FM2JS_XFA_FM2JSCONTEXT_H_
