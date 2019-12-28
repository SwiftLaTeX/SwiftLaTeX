// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/javascript/event.h"

#include "fpdfsdk/javascript/Field.h"
#include "fpdfsdk/javascript/JS_Define.h"
#include "fpdfsdk/javascript/JS_EventHandler.h"
#include "fpdfsdk/javascript/JS_Object.h"
#include "fpdfsdk/javascript/JS_Value.h"
#include "fpdfsdk/javascript/cjs_context.h"

BEGIN_JS_STATIC_CONST(CJS_Event)
END_JS_STATIC_CONST()

BEGIN_JS_STATIC_PROP(CJS_Event)
JS_STATIC_PROP_ENTRY(change)
JS_STATIC_PROP_ENTRY(changeEx)
JS_STATIC_PROP_ENTRY(commitKey)
JS_STATIC_PROP_ENTRY(fieldFull)
JS_STATIC_PROP_ENTRY(keyDown)
JS_STATIC_PROP_ENTRY(modifier)
JS_STATIC_PROP_ENTRY(name)
JS_STATIC_PROP_ENTRY(rc)
JS_STATIC_PROP_ENTRY(richChange)
JS_STATIC_PROP_ENTRY(richChangeEx)
JS_STATIC_PROP_ENTRY(richValue)
JS_STATIC_PROP_ENTRY(selEnd)
JS_STATIC_PROP_ENTRY(selStart)
JS_STATIC_PROP_ENTRY(shift)
JS_STATIC_PROP_ENTRY(source)
JS_STATIC_PROP_ENTRY(target)
JS_STATIC_PROP_ENTRY(targetName)
JS_STATIC_PROP_ENTRY(type)
JS_STATIC_PROP_ENTRY(value)
JS_STATIC_PROP_ENTRY(willCommit)
END_JS_STATIC_PROP()

BEGIN_JS_STATIC_METHOD(CJS_Event)
END_JS_STATIC_METHOD()

IMPLEMENT_JS_CLASS(CJS_Event, event)

event::event(CJS_Object* pJsObject) : CJS_EmbedObj(pJsObject) {}

event::~event() {}

FX_BOOL event::change(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();
  CFX_WideString& wChange = pEvent->Change();
  if (vp.IsSetting()) {
    if (vp.GetType() == CJS_Value::VT_string)
      vp >> wChange;
  } else {
    vp << wChange;
  }
  return TRUE;
}

FX_BOOL event::changeEx(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->ChangeEx();
  return TRUE;
}

FX_BOOL event::commitKey(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->CommitKey();
  return TRUE;
}

FX_BOOL event::fieldFull(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (!vp.IsGetting() &&
      wcscmp((const wchar_t*)pEvent->Name(), L"Keystroke") != 0)
    return FALSE;

  if (pEvent->FieldFull())
    vp << TRUE;
  else
    vp << FALSE;
  return TRUE;
}

FX_BOOL event::keyDown(IJS_Context* cc,
                       CJS_PropValue& vp,
                       CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (pEvent->KeyDown())
    vp << TRUE;
  else
    vp << FALSE;
  return TRUE;
}

FX_BOOL event::modifier(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (pEvent->Modifier())
    vp << TRUE;
  else
    vp << FALSE;
  return TRUE;
}

FX_BOOL event::name(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->Name();
  return TRUE;
}

FX_BOOL event::rc(IJS_Context* cc, CJS_PropValue& vp, CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  FX_BOOL& bRc = pEvent->Rc();
  if (vp.IsSetting()) {
    vp >> bRc;
  } else {
    vp << bRc;
  }
  return TRUE;
}

FX_BOOL event::richChange(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL event::richChangeEx(IJS_Context* cc,
                            CJS_PropValue& vp,
                            CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL event::richValue(IJS_Context* cc,
                         CJS_PropValue& vp,
                         CFX_WideString& sError) {
  return TRUE;
}

FX_BOOL event::selEnd(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (wcscmp((const wchar_t*)pEvent->Name(), L"Keystroke") != 0) {
    return TRUE;
  }

  int& iSelEnd = pEvent->SelEnd();
  if (vp.IsSetting()) {
    vp >> iSelEnd;
  } else {
    vp << iSelEnd;
  }
  return TRUE;
}

FX_BOOL event::selStart(IJS_Context* cc,
                        CJS_PropValue& vp,
                        CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (wcscmp((const wchar_t*)pEvent->Name(), L"Keystroke") != 0) {
    return TRUE;
  }
  int& iSelStart = pEvent->SelStart();
  if (vp.IsSetting()) {
    vp >> iSelStart;
  } else {
    vp << iSelStart;
  }
  return TRUE;
}

FX_BOOL event::shift(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (pEvent->Shift())
    vp << TRUE;
  else
    vp << FALSE;
  return TRUE;
}

FX_BOOL event::source(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->Source()->GetJSObject();
  return TRUE;
}

FX_BOOL event::target(IJS_Context* cc,
                      CJS_PropValue& vp,
                      CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->Target_Field()->GetJSObject();
  return TRUE;
}

FX_BOOL event::targetName(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->TargetName();
  return TRUE;
}

FX_BOOL event::type(IJS_Context* cc,
                    CJS_PropValue& vp,
                    CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  vp << pEvent->Type();
  return TRUE;
}

FX_BOOL event::value(IJS_Context* cc,
                     CJS_PropValue& vp,
                     CFX_WideString& sError) {
  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (wcscmp((const wchar_t*)pEvent->Type(), L"Field") != 0)
    return FALSE;
  if (!pEvent->m_pValue)
    return FALSE;
  CFX_WideString& val = pEvent->Value();
  if (vp.IsSetting()) {
    vp >> val;
  } else {
    vp << val;
  }
  return TRUE;
}

FX_BOOL event::willCommit(IJS_Context* cc,
                          CJS_PropValue& vp,
                          CFX_WideString& sError) {
  if (!vp.IsGetting())
    return FALSE;

  CJS_Context* pContext = (CJS_Context*)cc;
  CJS_EventHandler* pEvent = pContext->GetEventHandler();

  if (pEvent->WillCommit())
    vp << TRUE;
  else
    vp << FALSE;
  return TRUE;
}
