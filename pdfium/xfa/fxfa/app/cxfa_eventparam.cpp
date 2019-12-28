// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/cxfa_eventparam.h"

#include "xfa/fxfa/include/fxfa.h"

CXFA_EventParam::CXFA_EventParam()
    : m_pTarget(nullptr),
      m_eType(XFA_EVENT_Unknown),
      m_bCancelAction(FALSE),
      m_iCommitKey(0),
      m_bKeyDown(FALSE),
      m_bModifier(FALSE),
      m_bReenter(FALSE),
      m_iSelEnd(0),
      m_iSelStart(0),
      m_bShift(FALSE),
      m_bIsFormReady(FALSE),
      m_iValidateActivities(XFA_VALIDATE_preSubmit) {}

CXFA_EventParam::~CXFA_EventParam() {}

CXFA_EventParam::CXFA_EventParam(const CXFA_EventParam& other) = default;

void CXFA_EventParam::Reset() {
  m_wsChange.clear();
  m_bCancelAction = FALSE;
  m_iCommitKey = 0;
  m_wsFullText.clear();
  m_bKeyDown = FALSE;
  m_bModifier = FALSE;
  m_wsNewContentType.clear();
  m_wsNewText.clear();
  m_wsPrevContentType.clear();
  m_wsPrevText.clear();
  m_bReenter = FALSE;
  m_iSelEnd = 0;
  m_iSelStart = 0;
  m_bShift = FALSE;
  m_wsSoapFaultCode.clear();
  m_wsSoapFaultString.clear();
  m_bIsFormReady = FALSE;
  m_iValidateActivities = XFA_VALIDATE_preSubmit;
}
