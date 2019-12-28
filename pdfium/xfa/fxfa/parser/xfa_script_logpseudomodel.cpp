// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_logpseudomodel.h"

#include "fxjs/include/cfxjse_arguments.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CScript_LogPseudoModel::CScript_LogPseudoModel(CXFA_Document* pDocument)
    : CXFA_Object(pDocument,
                  XFA_ObjectType::Object,
                  XFA_Element::LogPseudoModel) {}
CScript_LogPseudoModel::~CScript_LogPseudoModel() {}
void CScript_LogPseudoModel::Message(CFXJSE_Arguments* pArguments) {}
void CScript_LogPseudoModel::TraceEnabled(CFXJSE_Arguments* pArguments) {}
void CScript_LogPseudoModel::TraceActivate(CFXJSE_Arguments* pArguments) {}
void CScript_LogPseudoModel::TraceDeactivate(CFXJSE_Arguments* pArguments) {}
void CScript_LogPseudoModel::Trace(CFXJSE_Arguments* pArguments) {}
