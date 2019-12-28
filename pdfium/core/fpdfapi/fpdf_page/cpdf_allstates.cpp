// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_allstates.h"

#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"

namespace {

FX_FLOAT ClipFloat(FX_FLOAT f) {
  return std::max(0.0f, std::min(1.0f, f));
}

}  // namespace

CPDF_AllStates::CPDF_AllStates() {
  m_TextX = m_TextY = m_TextLineX = m_TextLineY = 0;
  m_TextLeading = 0;
  m_TextRise = 0;
  m_TextHorzScale = 1.0f;
}

CPDF_AllStates::~CPDF_AllStates() {}

void CPDF_AllStates::Copy(const CPDF_AllStates& src) {
  CopyStates(src);
  m_TextMatrix.Copy(src.m_TextMatrix);
  m_ParentMatrix.Copy(src.m_ParentMatrix);
  m_CTM.Copy(src.m_CTM);
  m_TextX = src.m_TextX;
  m_TextY = src.m_TextY;
  m_TextLineX = src.m_TextLineX;
  m_TextLineY = src.m_TextLineY;
  m_TextLeading = src.m_TextLeading;
  m_TextRise = src.m_TextRise;
  m_TextHorzScale = src.m_TextHorzScale;
}

void CPDF_AllStates::SetLineDash(CPDF_Array* pArray,
                                 FX_FLOAT phase,
                                 FX_FLOAT scale) {
  CFX_GraphStateData* pData = m_GraphState.GetModify();
  pData->m_DashPhase = phase * scale;
  pData->SetDashCount(static_cast<int>(pArray->GetCount()));
  for (size_t i = 0; i < pArray->GetCount(); i++)
    pData->m_DashArray[i] = pArray->GetNumberAt(i) * scale;
}

void CPDF_AllStates::ProcessExtGS(CPDF_Dictionary* pGS,
                                  CPDF_StreamContentParser* pParser) {
  CPDF_GeneralStateData* pGeneralState = m_GeneralState.GetModify();
  for (const auto& it : *pGS) {
    const CFX_ByteString& key_str = it.first;
    CPDF_Object* pElement = it.second;
    CPDF_Object* pObject = pElement ? pElement->GetDirect() : nullptr;
    if (!pObject)
      continue;

    uint32_t key = key_str.GetID();
    switch (key) {
      case FXBSTR_ID('L', 'W', 0, 0):
        m_GraphState.GetModify()->m_LineWidth = pObject->GetNumber();
        break;
      case FXBSTR_ID('L', 'C', 0, 0):
        m_GraphState.GetModify()->m_LineCap =
            (CFX_GraphStateData::LineCap)pObject->GetInteger();
        break;
      case FXBSTR_ID('L', 'J', 0, 0):
        m_GraphState.GetModify()->m_LineJoin =
            (CFX_GraphStateData::LineJoin)pObject->GetInteger();
        break;
      case FXBSTR_ID('M', 'L', 0, 0):
        m_GraphState.GetModify()->m_MiterLimit = pObject->GetNumber();
        break;
      case FXBSTR_ID('D', 0, 0, 0): {
        CPDF_Array* pDash = pObject->AsArray();
        if (!pDash)
          break;

        CPDF_Array* pArray = pDash->GetArrayAt(0);
        if (!pArray)
          break;

        SetLineDash(pArray, pDash->GetNumberAt(1), 1.0f);
        break;
      }
      case FXBSTR_ID('R', 'I', 0, 0):
        m_GeneralState.SetRenderIntent(pObject->GetString());
        break;
      case FXBSTR_ID('F', 'o', 'n', 't'): {
        CPDF_Array* pFont = pObject->AsArray();
        if (!pFont)
          break;

        m_TextState.GetModify()->m_FontSize = pFont->GetNumberAt(1);
        m_TextState.SetFont(pParser->FindFont(pFont->GetStringAt(0)));
        break;
      }
      case FXBSTR_ID('T', 'R', 0, 0):
        if (pGS->KeyExist("TR2")) {
          continue;
        }
      case FXBSTR_ID('T', 'R', '2', 0):
        pGeneralState->m_pTR =
            (pObject && !pObject->IsName()) ? pObject : nullptr;
        break;
      case FXBSTR_ID('B', 'M', 0, 0): {
        CPDF_Array* pArray = pObject->AsArray();
        CFX_ByteString mode =
            pArray ? pArray->GetStringAt(0) : pObject->GetString();

        pGeneralState->SetBlendMode(mode.AsStringC());
        if (pGeneralState->m_BlendType > FXDIB_BLEND_MULTIPLY) {
          pParser->GetPageObjectHolder()->SetBackgroundAlphaNeeded(TRUE);
        }
        break;
      }
      case FXBSTR_ID('S', 'M', 'a', 's'):
        if (ToDictionary(pObject)) {
          pGeneralState->m_pSoftMask = pObject;
          FXSYS_memcpy(pGeneralState->m_SMaskMatrix,
                       &pParser->GetCurStates()->m_CTM, sizeof(CFX_Matrix));
        } else {
          pGeneralState->m_pSoftMask = nullptr;
        }
        break;
      case FXBSTR_ID('C', 'A', 0, 0):
        pGeneralState->m_StrokeAlpha = ClipFloat(pObject->GetNumber());
        break;
      case FXBSTR_ID('c', 'a', 0, 0):
        pGeneralState->m_FillAlpha = ClipFloat(pObject->GetNumber());
        break;
      case FXBSTR_ID('O', 'P', 0, 0):
        pGeneralState->m_StrokeOP = pObject->GetInteger();
        if (!pGS->KeyExist("op")) {
          pGeneralState->m_FillOP = pObject->GetInteger();
        }
        break;
      case FXBSTR_ID('o', 'p', 0, 0):
        pGeneralState->m_FillOP = pObject->GetInteger();
        break;
      case FXBSTR_ID('O', 'P', 'M', 0):
        pGeneralState->m_OPMode = pObject->GetInteger();
        break;
      case FXBSTR_ID('B', 'G', 0, 0):
        if (pGS->KeyExist("BG2")) {
          continue;
        }
      case FXBSTR_ID('B', 'G', '2', 0):
        pGeneralState->m_pBG = pObject;
        break;
      case FXBSTR_ID('U', 'C', 'R', 0):
        if (pGS->KeyExist("UCR2")) {
          continue;
        }
      case FXBSTR_ID('U', 'C', 'R', '2'):
        pGeneralState->m_pUCR = pObject;
        break;
      case FXBSTR_ID('H', 'T', 0, 0):
        pGeneralState->m_pHT = pObject;
        break;
      case FXBSTR_ID('F', 'L', 0, 0):
        pGeneralState->m_Flatness = pObject->GetNumber();
        break;
      case FXBSTR_ID('S', 'M', 0, 0):
        pGeneralState->m_Smoothness = pObject->GetNumber();
        break;
      case FXBSTR_ID('S', 'A', 0, 0):
        pGeneralState->m_StrokeAdjust = pObject->GetInteger();
        break;
      case FXBSTR_ID('A', 'I', 'S', 0):
        pGeneralState->m_AlphaSource = pObject->GetInteger();
        break;
      case FXBSTR_ID('T', 'K', 0, 0):
        pGeneralState->m_TextKnockout = pObject->GetInteger();
        break;
    }
  }
  pGeneralState->m_Matrix = m_CTM;
}
