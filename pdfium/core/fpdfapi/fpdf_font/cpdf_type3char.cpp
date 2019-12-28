// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_font/cpdf_type3char.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fxge/include/fx_dib.h"

CPDF_Type3Char::CPDF_Type3Char(CPDF_Form* pForm)
    : m_pForm(pForm), m_bColored(FALSE) {}

CPDF_Type3Char::~CPDF_Type3Char() {
}

FX_BOOL CPDF_Type3Char::LoadBitmap(CPDF_RenderContext* pContext) {
  if (m_pBitmap || !m_pForm)
    return TRUE;

  if (m_pForm->GetPageObjectList()->size() != 1 || m_bColored)
    return FALSE;

  auto& pPageObj = m_pForm->GetPageObjectList()->front();
  if (!pPageObj->IsImage())
    return FALSE;

  m_ImageMatrix = pPageObj->AsImage()->m_Matrix;
  std::unique_ptr<CFX_DIBSource> pSource(
      pPageObj->AsImage()->m_pImage->LoadDIBSource());
  if (pSource)
    m_pBitmap.reset(pSource->Clone());
  m_pForm.reset();
  return TRUE;
}
