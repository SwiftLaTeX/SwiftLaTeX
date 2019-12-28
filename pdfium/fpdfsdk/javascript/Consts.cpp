// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/Consts.h"

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"

BEGIN_JS_STATIC_CONST(CJS_Border)
JS_STATIC_CONST_ENTRY_STRING(L"s", L"solid")
JS_STATIC_CONST_ENTRY_STRING(L"b", L"beveled")
JS_STATIC_CONST_ENTRY_STRING(L"d", L"dashed")
JS_STATIC_CONST_ENTRY_STRING(L"i", L"inset")
JS_STATIC_CONST_ENTRY_STRING(L"u", L"underline")
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Border, border)

BEGIN_JS_STATIC_CONST(CJS_Display)
JS_STATIC_CONST_ENTRY_NUMBER(L"visible", 0)
JS_STATIC_CONST_ENTRY_NUMBER(L"hidden", 1)
JS_STATIC_CONST_ENTRY_NUMBER(L"noPrint", 2)
JS_STATIC_CONST_ENTRY_NUMBER(L"noView", 3)
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Display, display)

BEGIN_JS_STATIC_CONST(CJS_Font)
JS_STATIC_CONST_ENTRY_STRING(L"Times", L"Times-Roman")
JS_STATIC_CONST_ENTRY_STRING(L"TimesB", L"Times-Bold")
JS_STATIC_CONST_ENTRY_STRING(L"TimesI", L"Times-Italic")
JS_STATIC_CONST_ENTRY_STRING(L"TimesBI", L"Times-BoldItalic")
JS_STATIC_CONST_ENTRY_STRING(L"Helv", L"Helvetica")
JS_STATIC_CONST_ENTRY_STRING(L"HelvB", L"Helvetica-Bold")
JS_STATIC_CONST_ENTRY_STRING(L"HelvI", L"Helvetica-Oblique")
JS_STATIC_CONST_ENTRY_STRING(L"HelvBI", L"Helvetica-BoldOblique")
JS_STATIC_CONST_ENTRY_STRING(L"Cour", L"Courier")
JS_STATIC_CONST_ENTRY_STRING(L"CourB", L"Courier-Bold")
JS_STATIC_CONST_ENTRY_STRING(L"CourI", L"Courier-Oblique")
JS_STATIC_CONST_ENTRY_STRING(L"CourBI", L"Courier-BoldOblique")
JS_STATIC_CONST_ENTRY_STRING(L"Symbol", L"Symbol")
JS_STATIC_CONST_ENTRY_STRING(L"ZapfD", L"ZapfDingbats")
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Font, font)

BEGIN_JS_STATIC_CONST(CJS_Highlight)
JS_STATIC_CONST_ENTRY_STRING(L"n", L"none")
JS_STATIC_CONST_ENTRY_STRING(L"i", L"invert")
JS_STATIC_CONST_ENTRY_STRING(L"p", L"push")
JS_STATIC_CONST_ENTRY_STRING(L"o", L"outline")
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Highlight, highlight)

BEGIN_JS_STATIC_CONST(CJS_Position)
JS_STATIC_CONST_ENTRY_NUMBER(L"textOnly", 0)
JS_STATIC_CONST_ENTRY_NUMBER(L"iconOnly", 1)
JS_STATIC_CONST_ENTRY_NUMBER(L"iconTextV", 2)
JS_STATIC_CONST_ENTRY_NUMBER(L"textIconV", 3)
JS_STATIC_CONST_ENTRY_NUMBER(L"iconTextH", 4)
JS_STATIC_CONST_ENTRY_NUMBER(L"textIconH", 5)
JS_STATIC_CONST_ENTRY_NUMBER(L"overlay", 6)
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Position, position)

BEGIN_JS_STATIC_CONST(CJS_ScaleHow)
JS_STATIC_CONST_ENTRY_NUMBER(L"proportional", 0)
JS_STATIC_CONST_ENTRY_NUMBER(L"anamorphic", 1)
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_ScaleHow, scaleHow)

BEGIN_JS_STATIC_CONST(CJS_ScaleWhen)
JS_STATIC_CONST_ENTRY_NUMBER(L"always", 0)
JS_STATIC_CONST_ENTRY_NUMBER(L"never", 1)
JS_STATIC_CONST_ENTRY_NUMBER(L"tooBig", 2)
JS_STATIC_CONST_ENTRY_NUMBER(L"tooSmall", 3)
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_ScaleWhen, scaleWhen)

BEGIN_JS_STATIC_CONST(CJS_Style)
JS_STATIC_CONST_ENTRY_STRING(L"ch", L"check")
JS_STATIC_CONST_ENTRY_STRING(L"cr", L"cross")
JS_STATIC_CONST_ENTRY_STRING(L"di", L"diamond")
JS_STATIC_CONST_ENTRY_STRING(L"ci", L"circle")
JS_STATIC_CONST_ENTRY_STRING(L"st", L"star")
JS_STATIC_CONST_ENTRY_STRING(L"sq", L"square")
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Style, style)

BEGIN_JS_STATIC_CONST(CJS_Zoomtype)
JS_STATIC_CONST_ENTRY_STRING(L"none", L"NoVary")
JS_STATIC_CONST_ENTRY_STRING(L"fitP", L"FitPage")
JS_STATIC_CONST_ENTRY_STRING(L"fitW", L"FitWidth")
JS_STATIC_CONST_ENTRY_STRING(L"fitH", L"FitHeight")
JS_STATIC_CONST_ENTRY_STRING(L"fitV", L"FitVisibleWidth")
JS_STATIC_CONST_ENTRY_STRING(L"pref", L"Preferred")
JS_STATIC_CONST_ENTRY_STRING(L"refW", L"ReflowWidth")
END_JS_STATIC_CONST()
IMPLEMENT_JS_CLASS_CONST(CJS_Zoomtype, zoomtype)

#define GLOBAL_STRING(rt, name, value)                                         \
  FXJS_DefineGlobalConst(                                                      \
      (rt)->GetIsolate(), (name),                                              \
      [](const v8::FunctionCallbackInfo<v8::Value>& info) {                    \
        info.GetReturnValue().Set(FXJS_NewString(info.GetIsolate(), (value))); \
      })

void CJS_GlobalConsts::DefineJSObjects(CJS_Runtime* pRuntime) {
  GLOBAL_STRING(pRuntime, L"IDS_GREATER_THAN",
                L"Invalid value: must be greater than or equal to % s.");

  GLOBAL_STRING(pRuntime, L"IDS_GT_AND_LT",
                L"Invalid value: must be greater than or equal to % s "
                L"and less than or equal to % s.");

  GLOBAL_STRING(pRuntime, L"IDS_LESS_THAN",
                L"Invalid value: must be less than or equal to % s.");

  GLOBAL_STRING(pRuntime, L"IDS_INVALID_MONTH", L"**Invalid**");
  GLOBAL_STRING(
      pRuntime, L"IDS_INVALID_DATE",
      L"Invalid date / time: please ensure that the date / time exists.Field");

  GLOBAL_STRING(pRuntime, L"IDS_INVALID_VALUE",
                L"The value entered does not match the format of the field");

  GLOBAL_STRING(pRuntime, L"IDS_AM", L"am");
  GLOBAL_STRING(pRuntime, L"IDS_PM", L"pm");
  GLOBAL_STRING(pRuntime, L"IDS_MONTH_INFO",
                L"January[1] February[2] March[3] April[4] May[5] "
                L"June[6] July[7] August[8] September[9] October[10] "
                L"November[11] December[12] Sept[9] Jan[1] Feb[2] Mar[3] "
                L"Apr[4] Jun[6] Jul[7] Aug[8] Sep[9] Oct[10] Nov[11] "
                L"Dec[12]");

  GLOBAL_STRING(pRuntime, L"IDS_STARTUP_CONSOLE_MSG", L"** ^ _ ^ **");
}

#define GLOBAL_ARRAY(rt, name, ...)                                        \
  {                                                                        \
    const FX_WCHAR* values[] = {__VA_ARGS__};                              \
    v8::Local<v8::Array> array = FXJS_NewArray((rt)->GetIsolate());        \
    for (size_t i = 0; i < FX_ArraySize(values); ++i)                      \
      array->Set(i, FXJS_NewString((rt)->GetIsolate(), values[i]));        \
    rt->SetConstArray(name, array);                                        \
    FXJS_DefineGlobalConst(                                                \
        (rt)->GetIsolate(), (name),                                        \
        [](const v8::FunctionCallbackInfo<v8::Value>& info) {              \
          CJS_Runtime* pLocalRuntime = static_cast<CJS_Runtime*>(          \
              FXJS_GetRuntimeFromIsolate(info.GetIsolate()));              \
          if (pLocalRuntime)                                               \
            info.GetReturnValue().Set(pLocalRuntime->GetConstArray(name)); \
        });                                                                \
  }

void CJS_GlobalArrays::DefineJSObjects(CJS_Runtime* pRuntime) {
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_ENTRY_DOT_SEP", L"[+-]?\\d*\\.?\\d*");
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_COMMIT_DOT_SEP",
               L"[+-]?\\d+(\\.\\d+)?",  // -1.0 or -1
               L"[+-]?\\.\\d+",         // -.1
               L"[+-]?\\d+\\.");        // -1.

  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_ENTRY_COMMA_SEP", L"[+-]?\\d*,?\\d*");
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_COMMIT_COMMA_SEP",
               L"[+-]?\\d+([.,]\\d+)?",  // -1,0 or -1
               L"[+-]?[.,]\\d+",         // -,1
               L"[+-]?\\d+[.,]");        // -1,

  GLOBAL_ARRAY(pRuntime, L"RE_ZIP_ENTRY", L"\\d{0,5}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP_COMMIT", L"\\d{5}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP4_ENTRY", L"\\d{0,5}(\\.|[- ])?\\d{0,4}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP4_COMMIT", L"\\d{5}(\\.|[- ])?\\d{4}");
  GLOBAL_ARRAY(pRuntime, L"RE_PHONE_ENTRY",
               // 555-1234 or 408 555-1234
               L"\\d{0,3}(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // (408
               L"\\(\\d{0,3}",

               // (408) 555-1234
               // (allow the addition of parens as an afterthought)
               L"\\(\\d{0,3}\\)(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // (408 555-1234
               L"\\(\\d{0,3}(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // 408) 555-1234
               L"\\d{0,3}\\)(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // international
               L"011(\\.|[- \\d])*");

  GLOBAL_ARRAY(
      pRuntime, L"RE_PHONE_COMMIT", L"\\d{3}(\\.|[- ])?\\d{4}",  // 555-1234
      L"\\d{3}(\\.|[- ])?\\d{3}(\\.|[- ])?\\d{4}",               // 408 555-1234
      L"\\(\\d{3}\\)(\\.|[- ])?\\d{3}(\\.|[- ])?\\d{4}",  // (408) 555-1234
      L"011(\\.|[- \\d])*");                              // international

  GLOBAL_ARRAY(pRuntime, L"RE_SSN_ENTRY",
               L"\\d{0,3}(\\.|[- ])?\\d{0,2}(\\.|[- ])?\\d{0,4}");

  GLOBAL_ARRAY(pRuntime, L"RE_SSN_COMMIT",
               L"\\d{3}(\\.|[- ])?\\d{2}(\\.|[- ])?\\d{4}");
}
