// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_txtedttextset.h"

#include "xfa/fde/cfde_txtedtengine.h"
#include "xfa/fde/cfde_txtedtpage.h"

CFDE_TxtEdtTextSet::CFDE_TxtEdtTextSet(CFDE_TxtEdtPage* pPage)
    : m_pPage(pPage) {}

CFDE_TxtEdtTextSet::~CFDE_TxtEdtTextSet() {}

FDE_VISUALOBJTYPE CFDE_TxtEdtTextSet::GetType() {
  return FDE_VISUALOBJ_Text;
}

void CFDE_TxtEdtTextSet::GetRect(FDE_TEXTEDITPIECE* pPiece, CFX_RectF& rt) {
  rt = pPiece->rtPiece;
}

int32_t CFDE_TxtEdtTextSet::GetString(FDE_TEXTEDITPIECE* pPiece,
                                      CFX_WideString& wsText) {
  FX_WCHAR* pBuffer = wsText.GetBuffer(pPiece->nCount);
  for (int32_t i = 0; i < pPiece->nCount; i++) {
    pBuffer[i] = m_pPage->GetChar(pPiece, i);
  }
  wsText.ReleaseBuffer(pPiece->nCount);
  return pPiece->nCount;
}

CFGAS_GEFont* CFDE_TxtEdtTextSet::GetFont() {
  return m_pPage->GetEngine()->GetEditParams()->pFont;
}

FX_FLOAT CFDE_TxtEdtTextSet::GetFontSize() {
  return m_pPage->GetEngine()->GetEditParams()->fFontSize;
}

FX_ARGB CFDE_TxtEdtTextSet::GetFontColor() {
  return m_pPage->GetEngine()->GetEditParams()->dwFontColor;
}

int32_t CFDE_TxtEdtTextSet::GetDisplayPos(FDE_TEXTEDITPIECE* pPiece,
                                          FXTEXT_CHARPOS* pCharPos,
                                          FX_BOOL bCharCode,
                                          CFX_WideString* pWSForms) {
  if (!pPiece)
    return 0;

  int32_t nLength = pPiece->nCount;
  if (nLength < 1)
    return 0;

  CFDE_TxtEdtEngine* pEngine =
      static_cast<CFDE_TxtEdtEngine*>(m_pPage->GetEngine());
  const FDE_TXTEDTPARAMS* pTextParams = pEngine->GetEditParams();
  CFX_TxtBreak* pBreak = pEngine->GetTextBreak();
  uint32_t dwLayoutStyle = pBreak->GetLayoutStyles();
  FX_TXTRUN tr;
  tr.pAccess = m_pPage;
  tr.pIdentity = pPiece;
  tr.iLength = nLength;
  tr.pFont = pTextParams->pFont;
  tr.fFontSize = pTextParams->fFontSize;
  tr.dwStyles = dwLayoutStyle;
  tr.iCharRotation = pTextParams->nCharRotation;
  tr.dwCharStyles = pPiece->dwCharStyles;
  tr.pRect = &(pPiece->rtPiece);
  tr.wLineBreakChar = pTextParams->wLineBreakChar;
  return pBreak->GetDisplayPos(&tr, pCharPos, bCharCode, pWSForms);
}

int32_t CFDE_TxtEdtTextSet::GetCharRects(const FDE_TEXTEDITPIECE* pPiece,
                                         CFX_RectFArray& rtArray,
                                         FX_BOOL bBBox) {
  if (!pPiece)
    return 0;

  CFDE_TxtEdtEngine* pEngine =
      static_cast<CFDE_TxtEdtEngine*>(m_pPage->GetEngine());
  int32_t nLength = pPiece->nCount;
  if (nLength < 1)
    return 0;

  const FDE_TXTEDTPARAMS* pTextParams = pEngine->GetEditParams();
  uint32_t dwLayoutStyle = pEngine->GetTextBreak()->GetLayoutStyles();
  FX_TXTRUN tr;
  tr.pAccess = m_pPage;
  tr.pIdentity = pPiece;
  tr.iLength = nLength;
  tr.pFont = pTextParams->pFont;
  tr.fFontSize = pTextParams->fFontSize;
  tr.dwStyles = dwLayoutStyle;
  tr.iCharRotation = pTextParams->nCharRotation;
  tr.dwCharStyles = pPiece->dwCharStyles;
  tr.pRect = &(pPiece->rtPiece);
  tr.wLineBreakChar = pTextParams->wLineBreakChar;
  return pEngine->GetTextBreak()->GetCharRects(&tr, rtArray, bBBox);
}
