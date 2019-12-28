// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"

CPDF_Form::CPDF_Form(CPDF_Document* pDoc,
                     CPDF_Dictionary* pPageResources,
                     CPDF_Stream* pFormStream,
                     CPDF_Dictionary* pParentResources) {
  m_pDocument = pDoc;
  m_pFormStream = pFormStream;
  m_pFormDict = pFormStream ? pFormStream->GetDict() : nullptr;
  m_pResources = m_pFormDict->GetDictBy("Resources");
  m_pPageResources = pPageResources;
  if (!m_pResources)
    m_pResources = pParentResources;
  if (!m_pResources)
    m_pResources = pPageResources;
  m_Transparency = 0;
  LoadTransInfo();
}

CPDF_Form::~CPDF_Form() {}

void CPDF_Form::StartParse(CPDF_AllStates* pGraphicStates,
                           const CFX_Matrix* pParentMatrix,
                           CPDF_Type3Char* pType3Char,
                           int level) {
  if (m_ParseState == CONTENT_PARSED || m_ParseState == CONTENT_PARSING)
    return;

  m_pParser.reset(new CPDF_ContentParser);
  m_pParser->Start(this, pGraphicStates, pParentMatrix, pType3Char, level);
  m_ParseState = CONTENT_PARSING;
}

void CPDF_Form::ParseContent(CPDF_AllStates* pGraphicStates,
                             const CFX_Matrix* pParentMatrix,
                             CPDF_Type3Char* pType3Char,
                             int level) {
  StartParse(pGraphicStates, pParentMatrix, pType3Char, level);
  ContinueParse(nullptr);
}

CPDF_Form* CPDF_Form::Clone() const {
  CPDF_Form* pCloneForm =
      new CPDF_Form(m_pDocument, m_pPageResources, m_pFormStream, m_pResources);
  for (const auto& pObj : m_PageObjectList)
    pCloneForm->m_PageObjectList.emplace_back(pObj->Clone());

  return pCloneForm;
}
