// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"

#include "core/fpdfapi/fpdf_font/cpdf_cidfont.h"
#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"

CPDF_TextObject::CPDF_TextObject()
    : m_PosX(0),
      m_PosY(0),
      m_nChars(0),
      m_pCharCodes(nullptr),
      m_pCharPos(nullptr) {}

CPDF_TextObject::~CPDF_TextObject() {
  if (m_nChars > 1) {
    FX_Free(m_pCharCodes);
  }
  FX_Free(m_pCharPos);
}

int CPDF_TextObject::CountItems() const {
  return m_nChars;
}

void CPDF_TextObject::GetItemInfo(int index, CPDF_TextObjectItem* pInfo) const {
  pInfo->m_CharCode =
      m_nChars == 1 ? (uint32_t)(uintptr_t)m_pCharCodes : m_pCharCodes[index];
  pInfo->m_OriginX = index ? m_pCharPos[index - 1] : 0;
  pInfo->m_OriginY = 0;
  if (pInfo->m_CharCode == CPDF_Font::kInvalidCharCode) {
    return;
  }
  CPDF_Font* pFont = m_TextState.GetFont();
  if (!pFont->IsCIDFont()) {
    return;
  }
  if (!pFont->AsCIDFont()->IsVertWriting()) {
    return;
  }
  uint16_t CID = pFont->AsCIDFont()->CIDFromCharCode(pInfo->m_CharCode);
  pInfo->m_OriginY = pInfo->m_OriginX;
  pInfo->m_OriginX = 0;
  short vx, vy;
  pFont->AsCIDFont()->GetVertOrigin(CID, vx, vy);
  FX_FLOAT fontsize = m_TextState.GetFontSize();
  pInfo->m_OriginX -= fontsize * vx / 1000;
  pInfo->m_OriginY -= fontsize * vy / 1000;
}

int CPDF_TextObject::CountChars() const {
  if (m_nChars == 1) {
    return 1;
  }
  int count = 0;
  for (int i = 0; i < m_nChars; ++i)
    if (m_pCharCodes[i] != CPDF_Font::kInvalidCharCode) {
      ++count;
    }
  return count;
}

void CPDF_TextObject::GetCharInfo(int index,
                                  uint32_t& charcode,
                                  FX_FLOAT& kerning) const {
  if (m_nChars == 1) {
    charcode = (uint32_t)(uintptr_t)m_pCharCodes;
    kerning = 0;
    return;
  }
  int count = 0;
  for (int i = 0; i < m_nChars; ++i) {
    if (m_pCharCodes[i] != CPDF_Font::kInvalidCharCode) {
      if (count == index) {
        charcode = m_pCharCodes[i];
        if (i == m_nChars - 1 ||
            m_pCharCodes[i + 1] != CPDF_Font::kInvalidCharCode) {
          kerning = 0;
        } else {
          kerning = m_pCharPos[i];
        }
        return;
      }
      ++count;
    }
  }
}

void CPDF_TextObject::GetCharInfo(int index, CPDF_TextObjectItem* pInfo) const {
  if (m_nChars == 1) {
    GetItemInfo(0, pInfo);
    return;
  }
  int count = 0;
  for (int i = 0; i < m_nChars; ++i) {
    uint32_t charcode = m_pCharCodes[i];
    if (charcode == CPDF_Font::kInvalidCharCode) {
      continue;
    }
    if (count == index) {
      GetItemInfo(i, pInfo);
      break;
    }
    ++count;
  }
}

CPDF_TextObject* CPDF_TextObject::Clone() const {
  CPDF_TextObject* obj = new CPDF_TextObject;
  obj->CopyData(this);

  obj->m_nChars = m_nChars;
  if (m_nChars > 1) {
    obj->m_pCharCodes = FX_Alloc(uint32_t, m_nChars);
    FXSYS_memcpy(obj->m_pCharCodes, m_pCharCodes, m_nChars * sizeof(uint32_t));
    obj->m_pCharPos = FX_Alloc(FX_FLOAT, m_nChars - 1);
    FXSYS_memcpy(obj->m_pCharPos, m_pCharPos,
                 (m_nChars - 1) * sizeof(FX_FLOAT));
  } else {
    obj->m_pCharCodes = m_pCharCodes;
  }
  obj->m_PosX = m_PosX;
  obj->m_PosY = m_PosY;
  return obj;
}

CPDF_PageObject::Type CPDF_TextObject::GetType() const {
  return TEXT;
}

void CPDF_TextObject::Transform(const CFX_Matrix& matrix) {
  m_TextState.GetModify();
  CFX_Matrix text_matrix;
  GetTextMatrix(&text_matrix);
  text_matrix.Concat(matrix);
  FX_FLOAT* pTextMatrix = m_TextState.GetMatrix();
  pTextMatrix[0] = text_matrix.GetA();
  pTextMatrix[1] = text_matrix.GetC();
  pTextMatrix[2] = text_matrix.GetB();
  pTextMatrix[3] = text_matrix.GetD();
  m_PosX = text_matrix.GetE();
  m_PosY = text_matrix.GetF();
  CalcPositionData(nullptr, nullptr, 0);
}

bool CPDF_TextObject::IsText() const {
  return true;
}

CPDF_TextObject* CPDF_TextObject::AsText() {
  return this;
}

const CPDF_TextObject* CPDF_TextObject::AsText() const {
  return this;
}

void CPDF_TextObject::GetTextMatrix(CFX_Matrix* pMatrix) const {
  FX_FLOAT* pTextMatrix = m_TextState.GetMatrix();
  pMatrix->Set(pTextMatrix[0], pTextMatrix[2], pTextMatrix[1], pTextMatrix[3],
               m_PosX, m_PosY);
}

void CPDF_TextObject::SetSegments(const CFX_ByteString* pStrs,
                                  FX_FLOAT* pKerning,
                                  int nsegs) {
  if (m_nChars > 1) {
    FX_Free(m_pCharCodes);
    m_pCharCodes = nullptr;
  }
  FX_Free(m_pCharPos);
  m_pCharPos = nullptr;
  CPDF_Font* pFont = m_TextState.GetFont();
  m_nChars = 0;
  for (int i = 0; i < nsegs; ++i) {
    m_nChars += pFont->CountChar(pStrs[i].c_str(), pStrs[i].GetLength());
  }
  m_nChars += nsegs - 1;
  if (m_nChars > 1) {
    m_pCharCodes = FX_Alloc(uint32_t, m_nChars);
    m_pCharPos = FX_Alloc(FX_FLOAT, m_nChars - 1);
    int index = 0;
    for (int i = 0; i < nsegs; ++i) {
      const FX_CHAR* segment = pStrs[i].c_str();
      int len = pStrs[i].GetLength();
      int offset = 0;
      while (offset < len) {
        m_pCharCodes[index++] = pFont->GetNextChar(segment, len, offset);
      }
      if (i != nsegs - 1) {
        m_pCharPos[index - 1] = pKerning[i];
        m_pCharCodes[index++] = CPDF_Font::kInvalidCharCode;
      }
    }
  } else {
    int offset = 0;
    m_pCharCodes = (uint32_t*)(uintptr_t)pFont->GetNextChar(
        pStrs[0].c_str(), pStrs[0].GetLength(), offset);
  }
}

void CPDF_TextObject::SetText(const CFX_ByteString& str) {
  SetSegments(&str, nullptr, 1);
  RecalcPositionData();
}

FX_FLOAT CPDF_TextObject::GetCharWidth(uint32_t charcode) const {
  FX_FLOAT fontsize = m_TextState.GetFontSize() / 1000;
  CPDF_Font* pFont = m_TextState.GetFont();
  FX_BOOL bVertWriting = FALSE;
  CPDF_CIDFont* pCIDFont = pFont->AsCIDFont();
  if (pCIDFont) {
    bVertWriting = pCIDFont->IsVertWriting();
  }
  if (!bVertWriting)
    return pFont->GetCharWidthF(charcode, 0) * fontsize;

  uint16_t CID = pCIDFont->CIDFromCharCode(charcode);
  return pCIDFont->GetVertWidth(CID) * fontsize;
}

FX_FLOAT CPDF_TextObject::GetPosX() const {
  return m_PosX;
}

FX_FLOAT CPDF_TextObject::GetPosY() const {
  return m_PosY;
}

CPDF_Font* CPDF_TextObject::GetFont() const {
  return m_TextState.GetFont();
}

FX_FLOAT CPDF_TextObject::GetFontSize() const {
  return m_TextState.GetFontSize();
}

void CPDF_TextObject::CalcPositionData(FX_FLOAT* pTextAdvanceX,
                                       FX_FLOAT* pTextAdvanceY,
                                       FX_FLOAT horz_scale,
                                       int level) {
  FX_FLOAT curpos = 0;
  FX_FLOAT min_x = 10000 * 1.0f;
  FX_FLOAT max_x = -10000 * 1.0f;
  FX_FLOAT min_y = 10000 * 1.0f;
  FX_FLOAT max_y = -10000 * 1.0f;
  CPDF_Font* pFont = m_TextState.GetFont();
  FX_BOOL bVertWriting = FALSE;
  CPDF_CIDFont* pCIDFont = pFont->AsCIDFont();
  if (pCIDFont) {
    bVertWriting = pCIDFont->IsVertWriting();
  }
  FX_FLOAT fontsize = m_TextState.GetFontSize();
  for (int i = 0; i < m_nChars; ++i) {
    uint32_t charcode =
        m_nChars == 1 ? (uint32_t)(uintptr_t)m_pCharCodes : m_pCharCodes[i];
    if (i > 0) {
      if (charcode == CPDF_Font::kInvalidCharCode) {
        curpos -= (m_pCharPos[i - 1] * fontsize) / 1000;
        continue;
      }
      m_pCharPos[i - 1] = curpos;
    }
    FX_RECT char_rect = pFont->GetCharBBox(charcode, level);
    FX_FLOAT charwidth;
    if (!bVertWriting) {
      if (min_y > char_rect.top) {
        min_y = (FX_FLOAT)char_rect.top;
      }
      if (max_y < char_rect.top) {
        max_y = (FX_FLOAT)char_rect.top;
      }
      if (min_y > char_rect.bottom) {
        min_y = (FX_FLOAT)char_rect.bottom;
      }
      if (max_y < char_rect.bottom) {
        max_y = (FX_FLOAT)char_rect.bottom;
      }
      FX_FLOAT char_left = curpos + char_rect.left * fontsize / 1000;
      FX_FLOAT char_right = curpos + char_rect.right * fontsize / 1000;
      if (min_x > char_left) {
        min_x = char_left;
      }
      if (max_x < char_left) {
        max_x = char_left;
      }
      if (min_x > char_right) {
        min_x = char_right;
      }
      if (max_x < char_right) {
        max_x = char_right;
      }
      charwidth = pFont->GetCharWidthF(charcode, level) * fontsize / 1000;
    } else {
      uint16_t CID = pCIDFont->CIDFromCharCode(charcode);
      short vx;
      short vy;
      pCIDFont->GetVertOrigin(CID, vx, vy);
      char_rect.left -= vx;
      char_rect.right -= vx;
      char_rect.top -= vy;
      char_rect.bottom -= vy;
      if (min_x > char_rect.left) {
        min_x = (FX_FLOAT)char_rect.left;
      }
      if (max_x < char_rect.left) {
        max_x = (FX_FLOAT)char_rect.left;
      }
      if (min_x > char_rect.right) {
        min_x = (FX_FLOAT)char_rect.right;
      }
      if (max_x < char_rect.right) {
        max_x = (FX_FLOAT)char_rect.right;
      }
      FX_FLOAT char_top = curpos + char_rect.top * fontsize / 1000;
      FX_FLOAT char_bottom = curpos + char_rect.bottom * fontsize / 1000;
      if (min_y > char_top) {
        min_y = char_top;
      }
      if (max_y < char_top) {
        max_y = char_top;
      }
      if (min_y > char_bottom) {
        min_y = char_bottom;
      }
      if (max_y < char_bottom) {
        max_y = char_bottom;
      }
      charwidth = pCIDFont->GetVertWidth(CID) * fontsize / 1000;
    }
    curpos += charwidth;
    if (charcode == ' ' && (!pCIDFont || pCIDFont->GetCharSize(32) == 1)) {
      curpos += m_TextState.GetObject()->m_WordSpace;
    }
    curpos += m_TextState.GetObject()->m_CharSpace;
  }
  if (bVertWriting) {
    if (pTextAdvanceX) {
      *pTextAdvanceX = 0;
    }
    if (pTextAdvanceY) {
      *pTextAdvanceY = curpos;
    }
    min_x = min_x * fontsize / 1000;
    max_x = max_x * fontsize / 1000;
  } else {
    if (pTextAdvanceX) {
      *pTextAdvanceX = curpos * horz_scale;
    }
    if (pTextAdvanceY) {
      *pTextAdvanceY = 0;
    }
    min_y = min_y * fontsize / 1000;
    max_y = max_y * fontsize / 1000;
  }
  CFX_Matrix matrix;
  GetTextMatrix(&matrix);
  m_Left = min_x;
  m_Right = max_x;
  m_Bottom = min_y;
  m_Top = max_y;
  matrix.TransformRect(m_Left, m_Right, m_Top, m_Bottom);
  if (TextRenderingModeIsStrokeMode(m_TextState.GetObject()->m_TextMode)) {
    FX_FLOAT half_width = m_GraphState.GetObject()->m_LineWidth / 2;
    m_Left -= half_width;
    m_Right += half_width;
    m_Top += half_width;
    m_Bottom -= half_width;
  }
}

void CPDF_TextObject::SetPosition(FX_FLOAT x, FX_FLOAT y) {
  FX_FLOAT dx = x - m_PosX;
  FX_FLOAT dy = y - m_PosY;
  m_PosX = x;
  m_PosY = y;
  m_Left += dx;
  m_Right += dx;
  m_Top += dy;
  m_Bottom += dy;
}

void CPDF_TextObject::RecalcPositionData() {
  CalcPositionData(nullptr, nullptr, 1);
}
