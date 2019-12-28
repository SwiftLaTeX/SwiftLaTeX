// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/color.h"

#include <vector>

#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"
#include "fpdfsdk/javascript/cjs_runtime.h"

BEGIN_JS_STATIC_CONST(CJS_Color)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Color)
JS_STATIC_PROP_ENTRY(black)
JS_STATIC_PROP_ENTRY(blue)
JS_STATIC_PROP_ENTRY(cyan)
JS_STATIC_PROP_ENTRY(dkGray)
JS_STATIC_PROP_ENTRY(gray)
JS_STATIC_PROP_ENTRY(green)
JS_STATIC_PROP_ENTRY(ltGray)
JS_STATIC_PROP_ENTRY(magenta)
JS_STATIC_PROP_ENTRY(red)
JS_STATIC_PROP_ENTRY(transparent)
JS_STATIC_PROP_ENTRY(white)
JS_STATIC_PROP_ENTRY(yellow)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Color)
JS_STATIC_METHOD_ENTRY(convert)
JS_STATIC_METHOD_ENTRY(equal)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_Color, color)

color::color(CJS_Object* pJSObject) : CJS_EmbedObj(pJSObject) {
  m_crTransparent = CPWL_Color(COLORTYPE_TRANSPARENT);
  m_crBlack = CPWL_Color(COLORTYPE_GRAY, 0);
  m_crWhite = CPWL_Color(COLORTYPE_GRAY, 1);
  m_crRed = CPWL_Color(COLORTYPE_RGB, 1, 0, 0);
  m_crGreen = CPWL_Color(COLORTYPE_RGB, 0, 1, 0);
  m_crBlue = CPWL_Color(COLORTYPE_RGB, 0, 0, 1);
  m_crCyan = CPWL_Color(COLORTYPE_CMYK, 1, 0, 0, 0);
  m_crMagenta = CPWL_Color(COLORTYPE_CMYK, 0, 1, 0, 0);
  m_crYellow = CPWL_Color(COLORTYPE_CMYK, 0, 0, 1, 0);
  m_crDKGray = CPWL_Color(COLORTYPE_GRAY, 0.25);
  m_crGray = CPWL_Color(COLORTYPE_GRAY, 0.5);
  m_crLTGray = CPWL_Color(COLORTYPE_GRAY, 0.75);
}

color::~color() {}

void color::ConvertPWLColorToArray(const CPWL_Color& color, CJS_Array& array) {
  switch (color.nColorType) {
    case COLORTYPE_TRANSPARENT:
      array.SetElement(0, CJS_Value(array.GetJSRuntime(), "T"));
      break;
    case COLORTYPE_GRAY:
      array.SetElement(0, CJS_Value(array.GetJSRuntime(), "G"));
      array.SetElement(1, CJS_Value(array.GetJSRuntime(), color.fColor1));
      break;
    case COLORTYPE_RGB:
      array.SetElement(0, CJS_Value(array.GetJSRuntime(), "RGB"));
      array.SetElement(1, CJS_Value(array.GetJSRuntime(), color.fColor1));
      array.SetElement(2, CJS_Value(array.GetJSRuntime(), color.fColor2));
      array.SetElement(3, CJS_Value(array.GetJSRuntime(), color.fColor3));
      break;
    case COLORTYPE_CMYK:
      array.SetElement(0, CJS_Value(array.GetJSRuntime(), "CMYK"));
      array.SetElement(1, CJS_Value(array.GetJSRuntime(), color.fColor1));
      array.SetElement(2, CJS_Value(array.GetJSRuntime(), color.fColor2));
      array.SetElement(3, CJS_Value(array.GetJSRuntime(), color.fColor3));
      array.SetElement(4, CJS_Value(array.GetJSRuntime(), color.fColor4));
      break;
  }
}

void color::ConvertArrayToPWLColor(CJS_Array& array, CPWL_Color& color) {
  int nArrayLen = array.GetLength();
  if (nArrayLen < 1)
    return;

  CJS_Value value(array.GetJSRuntime());
  array.GetElement(0, value);
  CFX_ByteString sSpace = value.ToCFXByteString();

  double d1 = 0;
  double d2 = 0;
  double d3 = 0;
  double d4 = 0;

  if (nArrayLen > 1) {
    array.GetElement(1, value);
    d1 = value.ToDouble();
  }

  if (nArrayLen > 2) {
    array.GetElement(2, value);
    d2 = value.ToDouble();
  }

  if (nArrayLen > 3) {
    array.GetElement(3, value);
    d3 = value.ToDouble();
  }

  if (nArrayLen > 4) {
    array.GetElement(4, value);
    d4 = value.ToDouble();
  }

  if (sSpace == "T") {
    color = CPWL_Color(COLORTYPE_TRANSPARENT);
  } else if (sSpace == "G") {
    color = CPWL_Color(COLORTYPE_GRAY, (FX_FLOAT)d1);
  } else if (sSpace == "RGB") {
    color = CPWL_Color(COLORTYPE_RGB, (FX_FLOAT)d1, (FX_FLOAT)d2, (FX_FLOAT)d3);
  } else if (sSpace == "CMYK") {
    color = CPWL_Color(COLORTYPE_CMYK, (FX_FLOAT)d1, (FX_FLOAT)d2, (FX_FLOAT)d3,
                       (FX_FLOAT)d4);
  }
}

#define JS_IMPLEMENT_COLORPROP(prop, var)                 \
  FX_BOOL color::prop(IJS_Context* cc, CJS_PropValue& vp, \
                      CFX_WideString& sError) {           \
    CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc); \
    CJS_Array array(pRuntime);                            \
    if (vp.IsGetting()) {                                 \
      ConvertPWLColorToArray(var, array);                 \
      vp << array;                                        \
    } else {                                              \
      if (!vp.ConvertToArray(array))                      \
        return FALSE;                                     \
      ConvertArrayToPWLColor(array, var);                 \
    }                                                     \
    return TRUE;                                          \
  }

JS_IMPLEMENT_COLORPROP(transparent, m_crTransparent)
JS_IMPLEMENT_COLORPROP(black, m_crBlack)
JS_IMPLEMENT_COLORPROP(white, m_crWhite)
JS_IMPLEMENT_COLORPROP(red, m_crRed)
JS_IMPLEMENT_COLORPROP(green, m_crGreen)
JS_IMPLEMENT_COLORPROP(blue, m_crBlue)
JS_IMPLEMENT_COLORPROP(cyan, m_crCyan)
JS_IMPLEMENT_COLORPROP(magenta, m_crMagenta)
JS_IMPLEMENT_COLORPROP(yellow, m_crYellow)
JS_IMPLEMENT_COLORPROP(dkGray, m_crDKGray)
JS_IMPLEMENT_COLORPROP(gray, m_crGray)
JS_IMPLEMENT_COLORPROP(ltGray, m_crLTGray)

FX_BOOL color::convert(IJS_Context* cc,
                       const std::vector<CJS_Value>& params,
                       CJS_Value& vRet,
                       CFX_WideString& sError) {
  int iSize = params.size();
  if (iSize < 2)
    return FALSE;

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array aSource(pRuntime);
  if (!params[0].ConvertToArray(aSource))
    return FALSE;

  CPWL_Color crSource;
  ConvertArrayToPWLColor(aSource, crSource);

  CFX_ByteString sDestSpace = params[1].ToCFXByteString();
  int nColorType = COLORTYPE_TRANSPARENT;

  if (sDestSpace == "T") {
    nColorType = COLORTYPE_TRANSPARENT;
  } else if (sDestSpace == "G") {
    nColorType = COLORTYPE_GRAY;
  } else if (sDestSpace == "RGB") {
    nColorType = COLORTYPE_RGB;
  } else if (sDestSpace == "CMYK") {
    nColorType = COLORTYPE_CMYK;
  }

  CJS_Array aDest(pRuntime);
  CPWL_Color crDest = crSource;
  crDest.ConvertColorType(nColorType);
  ConvertPWLColorToArray(crDest, aDest);
  vRet = aDest;

  return TRUE;
}

FX_BOOL color::equal(IJS_Context* cc,
                     const std::vector<CJS_Value>& params,
                     CJS_Value& vRet,
                     CFX_WideString& sError) {
  if (params.size() < 2)
    return FALSE;

  CJS_Runtime* pRuntime = CJS_Runtime::FromContext(cc);
  CJS_Array array1(pRuntime);
  CJS_Array array2(pRuntime);
  if (!params[0].ConvertToArray(array1))
    return FALSE;
  if (!params[1].ConvertToArray(array2))
    return FALSE;

  CPWL_Color color1;
  CPWL_Color color2;
  ConvertArrayToPWLColor(array1, color1);
  ConvertArrayToPWLColor(array2, color2);
  color1.ConvertColorType(color2.nColorType);
  vRet = color1 == color2;
  return TRUE;
}
