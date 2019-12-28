// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_SCRIPT_EVENTPSEUDOMODEL_H_
#define XFA_FXFA_PARSER_XFA_SCRIPT_EVENTPSEUDOMODEL_H_

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/parser/xfa_object.h"

enum class XFA_Event {
  Change = 0,
  CommitKey,
  FullText,
  Keydown,
  Modifier,
  NewContentType,
  NewText,
  PreviousContentType,
  PreviousText,
  Reenter,
  SelectionEnd,
  SelectionStart,
  Shift,
  SoapFaultCode,
  SoapFaultString,
  Target,
  CancelAction
};

class CScript_EventPseudoModel : public CXFA_Object {
 public:
  explicit CScript_EventPseudoModel(CXFA_Document* pDocument);
  ~CScript_EventPseudoModel() override;

  void Change(CFXJSE_Value* pValue, FX_BOOL bSetting, XFA_ATTRIBUTE eAttribute);
  void CommitKey(CFXJSE_Value* pValue,
                 FX_BOOL bSetting,
                 XFA_ATTRIBUTE eAttribute);
  void FullText(CFXJSE_Value* pValue,
                FX_BOOL bSetting,
                XFA_ATTRIBUTE eAttribute);
  void KeyDown(CFXJSE_Value* pValue,
               FX_BOOL bSetting,
               XFA_ATTRIBUTE eAttribute);
  void Modifier(CFXJSE_Value* pValue,
                FX_BOOL bSetting,
                XFA_ATTRIBUTE eAttribute);
  void NewContentType(CFXJSE_Value* pValue,
                      FX_BOOL bSetting,
                      XFA_ATTRIBUTE eAttribute);
  void NewText(CFXJSE_Value* pValue,
               FX_BOOL bSetting,
               XFA_ATTRIBUTE eAttribute);
  void PrevContentType(CFXJSE_Value* pValue,
                       FX_BOOL bSetting,
                       XFA_ATTRIBUTE eAttribute);
  void PrevText(CFXJSE_Value* pValue,
                FX_BOOL bSetting,
                XFA_ATTRIBUTE eAttribute);
  void Reenter(CFXJSE_Value* pValue,
               FX_BOOL bSetting,
               XFA_ATTRIBUTE eAttribute);
  void SelEnd(CFXJSE_Value* pValue, FX_BOOL bSetting, XFA_ATTRIBUTE eAttribute);
  void SelStart(CFXJSE_Value* pValue,
                FX_BOOL bSetting,
                XFA_ATTRIBUTE eAttribute);
  void Shift(CFXJSE_Value* pValue, FX_BOOL bSetting, XFA_ATTRIBUTE eAttribute);
  void SoapFaultCode(CFXJSE_Value* pValue,
                     FX_BOOL bSetting,
                     XFA_ATTRIBUTE eAttribute);
  void SoapFaultString(CFXJSE_Value* pValue,
                       FX_BOOL bSetting,
                       XFA_ATTRIBUTE eAttribute);
  void Target(CFXJSE_Value* pValue, FX_BOOL bSetting, XFA_ATTRIBUTE eAttribute);

  void Emit(CFXJSE_Arguments* pArguments);
  void Reset(CFXJSE_Arguments* pArguments);

 protected:
  void Property(CFXJSE_Value* pValue, XFA_Event dwFlag, FX_BOOL bSetting);
};

#endif  // XFA_FXFA_PARSER_XFA_SCRIPT_EVENTPSEUDOMODEL_H_
