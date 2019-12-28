// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/fxedit/include/fxet_edit.h"

#include <algorithm>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pathobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_textrenderer.h"
#include "core/fpdfdoc/include/cpvt_section.h"
#include "core/fpdfdoc/include/cpvt_word.h"
#include "core/fpdfdoc/include/ipvt_fontmap.h"
#include "core/fxge/include/fx_ge.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/fxedit/include/fx_edit.h"
#include "fpdfsdk/fxedit/include/fxet_edit.h"
#include "fpdfsdk/pdfwindow/PWL_Edit.h"
#include "fpdfsdk/pdfwindow/PWL_EditCtrl.h"

#define PVTWORD_STYLE_UNDERLINE 0x0002L
#define PVTWORD_STYLE_CROSSOUT 0x0004L
#define PVTWORD_STYLE_BOLD 0x0020L
#define PVTWORD_STYLE_ITALIC 0x0040L

namespace {

const int kEditUndoMaxItems = 10000;

CFX_ByteString GetWordRenderString(const CFX_ByteString& strWords) {
  if (strWords.GetLength() > 0)
    return PDF_EncodeString(strWords) + " Tj\n";
  return CFX_ByteString();
}

CFX_ByteString GetFontSetString(IPVT_FontMap* pFontMap,
                                int32_t nFontIndex,
                                FX_FLOAT fFontSize) {
  if (!pFontMap)
    return CFX_ByteString();

  CFX_ByteString sFontAlias = pFontMap->GetPDFFontAlias(nFontIndex);
  if (sFontAlias.GetLength() <= 0 || fFontSize <= 0)
    return CFX_ByteString();

  CFX_ByteTextBuf sRet;
  sRet << "/" << sFontAlias << " " << fFontSize << " Tf\n";
  return sRet.MakeString();
}

CFX_FloatRect GetUnderLineRect(const CPVT_Word& word) {
  return CFX_FloatRect(word.ptWord.x, word.ptWord.y + word.fDescent * 0.5f,
                       word.ptWord.x + word.fWidth,
                       word.ptWord.y + word.fDescent * 0.25f);
}

CFX_FloatRect GetCrossoutRect(const CPVT_Word& word) {
  return CFX_FloatRect(word.ptWord.x,
                       word.ptWord.y + (word.fAscent + word.fDescent) * 0.5f +
                           word.fDescent * 0.25f,
                       word.ptWord.x + word.fWidth,
                       word.ptWord.y + (word.fAscent + word.fDescent) * 0.5f);
}

void DrawTextString(CFX_RenderDevice* pDevice,
                    const CFX_FloatPoint& pt,
                    CPDF_Font* pFont,
                    FX_FLOAT fFontSize,
                    CFX_Matrix* pUser2Device,
                    const CFX_ByteString& str,
                    FX_ARGB crTextFill,
                    FX_ARGB crTextStroke,
                    int32_t nHorzScale) {
  FX_FLOAT x = pt.x, y = pt.y;
  pUser2Device->Transform(x, y);

  if (pFont) {
    if (nHorzScale != 100) {
      CFX_Matrix mt(nHorzScale / 100.0f, 0, 0, 1, 0, 0);
      mt.Concat(*pUser2Device);

      CPDF_RenderOptions ro;
      ro.m_Flags = RENDER_CLEARTYPE;
      ro.m_ColorMode = RENDER_COLOR_NORMAL;

      if (crTextStroke != 0) {
        CFX_FloatPoint pt1(0, 0), pt2(1, 0);
        pUser2Device->Transform(pt1.x, pt1.y);
        pUser2Device->Transform(pt2.x, pt2.y);
        CFX_GraphStateData gsd;
        gsd.m_LineWidth =
            (FX_FLOAT)FXSYS_fabs((pt2.x + pt2.y) - (pt1.x + pt1.y));

        CPDF_TextRenderer::DrawTextString(pDevice, x, y, pFont, fFontSize, &mt,
                                          str, crTextFill, crTextStroke, &gsd,
                                          &ro);
      } else {
        CPDF_TextRenderer::DrawTextString(pDevice, x, y, pFont, fFontSize, &mt,
                                          str, crTextFill, 0, nullptr, &ro);
      }
    } else {
      CPDF_RenderOptions ro;
      ro.m_Flags = RENDER_CLEARTYPE;
      ro.m_ColorMode = RENDER_COLOR_NORMAL;

      if (crTextStroke != 0) {
        CFX_FloatPoint pt1(0, 0), pt2(1, 0);
        pUser2Device->Transform(pt1.x, pt1.y);
        pUser2Device->Transform(pt2.x, pt2.y);
        CFX_GraphStateData gsd;
        gsd.m_LineWidth =
            (FX_FLOAT)FXSYS_fabs((pt2.x + pt2.y) - (pt1.x + pt1.y));

        CPDF_TextRenderer::DrawTextString(pDevice, x, y, pFont, fFontSize,
                                          pUser2Device, str, crTextFill,
                                          crTextStroke, &gsd, &ro);
      } else {
        CPDF_TextRenderer::DrawTextString(pDevice, x, y, pFont, fFontSize,
                                          pUser2Device, str, crTextFill, 0,
                                          nullptr, &ro);
      }
    }
  }
}

void AddRectToPageObjects(CPDF_PageObjectHolder* pObjectHolder,
                          FX_COLORREF crFill,
                          const CFX_FloatRect& rcFill) {
  std::unique_ptr<CPDF_PathObject> pPathObj(new CPDF_PathObject);
  CFX_PathData* pPathData = pPathObj->m_Path.GetModify();
  pPathData->AppendRect(rcFill.left, rcFill.bottom, rcFill.right, rcFill.top);

  FX_FLOAT rgb[3];
  rgb[0] = FXARGB_R(crFill) / 255.0f;
  rgb[1] = FXARGB_G(crFill) / 255.0f;
  rgb[2] = FXARGB_B(crFill) / 255.0f;
  pPathObj->m_ColorState.SetFillColor(
      CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB), rgb, 3);

  pPathObj->m_FillType = FXFILL_ALTERNATE;
  pPathObj->m_bStroke = FALSE;
  pObjectHolder->GetPageObjectList()->push_back(std::move(pPathObj));
}

CPDF_TextObject* AddTextObjToPageObjects(CPDF_PageObjectHolder* pObjectHolder,
                                         FX_COLORREF crText,
                                         CPDF_Font* pFont,
                                         FX_FLOAT fFontSize,
                                         FX_FLOAT fCharSpace,
                                         int32_t nHorzScale,
                                         const CFX_FloatPoint& point,
                                         const CFX_ByteString& text) {
  std::unique_ptr<CPDF_TextObject> pTxtObj(new CPDF_TextObject);
  CPDF_TextStateData* pTextStateData = pTxtObj->m_TextState.GetModify();
  pTextStateData->m_pFont = pFont;
  pTextStateData->m_FontSize = fFontSize;
  pTextStateData->m_CharSpace = fCharSpace;
  pTextStateData->m_WordSpace = 0;
  pTextStateData->m_TextMode = TextRenderingMode::MODE_FILL;
  pTextStateData->m_Matrix[0] = nHorzScale / 100.0f;
  pTextStateData->m_Matrix[1] = 0;
  pTextStateData->m_Matrix[2] = 0;
  pTextStateData->m_Matrix[3] = 1;

  FX_FLOAT rgb[3];
  rgb[0] = FXARGB_R(crText) / 255.0f;
  rgb[1] = FXARGB_G(crText) / 255.0f;
  rgb[2] = FXARGB_B(crText) / 255.0f;
  pTxtObj->m_ColorState.SetFillColor(
      CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB), rgb, 3);
  pTxtObj->m_ColorState.SetStrokeColor(
      CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB), rgb, 3);

  pTxtObj->SetPosition(point.x, point.y);
  pTxtObj->SetText(text);

  CPDF_TextObject* pRet = pTxtObj.get();
  pObjectHolder->GetPageObjectList()->push_back(std::move(pTxtObj));
  return pRet;
}

}  // namespace

CFX_Edit_Iterator::CFX_Edit_Iterator(CFX_Edit* pEdit,
                                     CPDF_VariableText::Iterator* pVTIterator)
    : m_pEdit(pEdit), m_pVTIterator(pVTIterator) {}

CFX_Edit_Iterator::~CFX_Edit_Iterator() {}

FX_BOOL CFX_Edit_Iterator::NextWord() {
  return m_pVTIterator->NextWord();
}

FX_BOOL CFX_Edit_Iterator::NextLine() {
  return m_pVTIterator->NextLine();
}

FX_BOOL CFX_Edit_Iterator::NextSection() {
  return m_pVTIterator->NextSection();
}

FX_BOOL CFX_Edit_Iterator::PrevWord() {
  return m_pVTIterator->PrevWord();
}

FX_BOOL CFX_Edit_Iterator::PrevLine() {
  return m_pVTIterator->PrevLine();
}

FX_BOOL CFX_Edit_Iterator::PrevSection() {
  return m_pVTIterator->PrevSection();
}

FX_BOOL CFX_Edit_Iterator::GetWord(CPVT_Word& word) const {
  ASSERT(m_pEdit);

  if (m_pVTIterator->GetWord(word)) {
    word.ptWord = m_pEdit->VTToEdit(word.ptWord);
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CFX_Edit_Iterator::GetLine(CPVT_Line& line) const {
  ASSERT(m_pEdit);

  if (m_pVTIterator->GetLine(line)) {
    line.ptLine = m_pEdit->VTToEdit(line.ptLine);
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CFX_Edit_Iterator::GetSection(CPVT_Section& section) const {
  ASSERT(m_pEdit);

  if (m_pVTIterator->GetSection(section)) {
    section.rcSection = m_pEdit->VTToEdit(section.rcSection);
    return TRUE;
  }
  return FALSE;
}

void CFX_Edit_Iterator::SetAt(int32_t nWordIndex) {
  m_pVTIterator->SetAt(nWordIndex);
}

void CFX_Edit_Iterator::SetAt(const CPVT_WordPlace& place) {
  m_pVTIterator->SetAt(place);
}

const CPVT_WordPlace& CFX_Edit_Iterator::GetAt() const {
  return m_pVTIterator->GetAt();
}

CFX_Edit* CFX_Edit_Iterator::GetEdit() const {
  return m_pEdit;
}

CFX_Edit_Provider::CFX_Edit_Provider(IPVT_FontMap* pFontMap)
    : CPDF_VariableText::Provider(pFontMap), m_pFontMap(pFontMap) {
  ASSERT(m_pFontMap);
}

CFX_Edit_Provider::~CFX_Edit_Provider() {}

IPVT_FontMap* CFX_Edit_Provider::GetFontMap() {
  return m_pFontMap;
}

int32_t CFX_Edit_Provider::GetCharWidth(int32_t nFontIndex,
                                        uint16_t word,
                                        int32_t nWordStyle) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex)) {
    uint32_t charcode = word;

    if (pPDFFont->IsUnicodeCompatible())
      charcode = pPDFFont->CharCodeFromUnicode(word);
    else
      charcode = m_pFontMap->CharCodeFromUnicode(nFontIndex, word);

    if (charcode != CPDF_Font::kInvalidCharCode)
      return pPDFFont->GetCharWidthF(charcode);
  }

  return 0;
}

int32_t CFX_Edit_Provider::GetTypeAscent(int32_t nFontIndex) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex))
    return pPDFFont->GetTypeAscent();

  return 0;
}

int32_t CFX_Edit_Provider::GetTypeDescent(int32_t nFontIndex) {
  if (CPDF_Font* pPDFFont = m_pFontMap->GetPDFFont(nFontIndex))
    return pPDFFont->GetTypeDescent();

  return 0;
}

int32_t CFX_Edit_Provider::GetWordFontIndex(uint16_t word,
                                            int32_t charset,
                                            int32_t nFontIndex) {
  return m_pFontMap->GetWordFontIndex(word, charset, nFontIndex);
}

int32_t CFX_Edit_Provider::GetDefaultFontIndex() {
  return 0;
}

FX_BOOL CFX_Edit_Provider::IsLatinWord(uint16_t word) {
  return FX_EDIT_ISLATINWORD(word);
}

CFX_Edit_Refresh::CFX_Edit_Refresh() {}

CFX_Edit_Refresh::~CFX_Edit_Refresh() {}

void CFX_Edit_Refresh::BeginRefresh() {
  m_RefreshRects.Empty();
  m_OldLineRects = m_NewLineRects;
}

void CFX_Edit_Refresh::Push(const CPVT_WordRange& linerange,
                            const CFX_FloatRect& rect) {
  m_NewLineRects.Add(linerange, rect);
}

void CFX_Edit_Refresh::NoAnalyse() {
  {
    for (int32_t i = 0, sz = m_OldLineRects.GetSize(); i < sz; i++)
      if (CFX_Edit_LineRect* pOldRect = m_OldLineRects.GetAt(i))
        m_RefreshRects.Add(pOldRect->m_rcLine);
  }

  {
    for (int32_t i = 0, sz = m_NewLineRects.GetSize(); i < sz; i++)
      if (CFX_Edit_LineRect* pNewRect = m_NewLineRects.GetAt(i))
        m_RefreshRects.Add(pNewRect->m_rcLine);
  }
}

void CFX_Edit_Refresh::Analyse(int32_t nAlignment) {
  FX_BOOL bLineTopChanged = FALSE;
  CFX_FloatRect rcResult;
  FX_FLOAT fWidthDiff;

  int32_t szMax = std::max(m_OldLineRects.GetSize(), m_NewLineRects.GetSize());
  int32_t i = 0;

  while (i < szMax) {
    CFX_Edit_LineRect* pOldRect = m_OldLineRects.GetAt(i);
    CFX_Edit_LineRect* pNewRect = m_NewLineRects.GetAt(i);

    if (pOldRect) {
      if (pNewRect) {
        if (bLineTopChanged) {
          rcResult = pOldRect->m_rcLine;
          rcResult.Union(pNewRect->m_rcLine);
          m_RefreshRects.Add(rcResult);
        } else {
          if (*pNewRect != *pOldRect) {
            if (!pNewRect->IsSameTop(*pOldRect) ||
                !pNewRect->IsSameHeight(*pOldRect)) {
              bLineTopChanged = TRUE;
              continue;
            }

            if (nAlignment == 0) {
              if (pNewRect->m_wrLine.BeginPos != pOldRect->m_wrLine.BeginPos) {
                rcResult = pOldRect->m_rcLine;
                rcResult.Union(pNewRect->m_rcLine);
                m_RefreshRects.Add(rcResult);
              } else {
                if (!pNewRect->IsSameLeft(*pOldRect)) {
                  rcResult = pOldRect->m_rcLine;
                  rcResult.Union(pNewRect->m_rcLine);
                } else {
                  fWidthDiff =
                      pNewRect->m_rcLine.Width() - pOldRect->m_rcLine.Width();
                  rcResult = pNewRect->m_rcLine;
                  if (fWidthDiff > 0.0f) {
                    rcResult.left = rcResult.right - fWidthDiff;
                  } else {
                    rcResult.left = rcResult.right;
                    rcResult.right -= fWidthDiff;
                  }
                }
                m_RefreshRects.Add(rcResult);
              }
            } else {
              rcResult = pOldRect->m_rcLine;
              rcResult.Union(pNewRect->m_rcLine);
              m_RefreshRects.Add(rcResult);
            }
          }
        }
      } else {
        m_RefreshRects.Add(pOldRect->m_rcLine);
      }
    } else {
      if (pNewRect) {
        m_RefreshRects.Add(pNewRect->m_rcLine);
      }
    }
    i++;
  }
}

void CFX_Edit_Refresh::AddRefresh(const CFX_FloatRect& rect) {
  m_RefreshRects.Add(rect);
}

const CFX_Edit_RectArray* CFX_Edit_Refresh::GetRefreshRects() const {
  return &m_RefreshRects;
}

void CFX_Edit_Refresh::EndRefresh() {
  m_RefreshRects.Empty();
}

CFX_Edit_Undo::CFX_Edit_Undo(int32_t nBufsize)
    : m_nCurUndoPos(0),
      m_nBufSize(nBufsize),
      m_bModified(FALSE),
      m_bVirgin(TRUE),
      m_bWorking(FALSE) {}

CFX_Edit_Undo::~CFX_Edit_Undo() {
  Reset();
}

FX_BOOL CFX_Edit_Undo::CanUndo() const {
  return m_nCurUndoPos > 0;
}

void CFX_Edit_Undo::Undo() {
  m_bWorking = TRUE;

  if (m_nCurUndoPos > 0) {
    IFX_Edit_UndoItem* pItem = m_UndoItemStack.GetAt(m_nCurUndoPos - 1);
    pItem->Undo();

    m_nCurUndoPos--;
    m_bModified = (m_nCurUndoPos != 0);
  }

  m_bWorking = FALSE;
}

FX_BOOL CFX_Edit_Undo::CanRedo() const {
  return m_nCurUndoPos < m_UndoItemStack.GetSize();
}

void CFX_Edit_Undo::Redo() {
  m_bWorking = TRUE;

  int32_t nStackSize = m_UndoItemStack.GetSize();

  if (m_nCurUndoPos < nStackSize) {
    IFX_Edit_UndoItem* pItem = m_UndoItemStack.GetAt(m_nCurUndoPos);
    pItem->Redo();

    m_nCurUndoPos++;
    m_bModified = (m_nCurUndoPos != 0);
  }

  m_bWorking = FALSE;
}

FX_BOOL CFX_Edit_Undo::IsWorking() const {
  return m_bWorking;
}

void CFX_Edit_Undo::AddItem(IFX_Edit_UndoItem* pItem) {
  ASSERT(!m_bWorking);
  ASSERT(pItem);
  ASSERT(m_nBufSize > 1);

  if (m_nCurUndoPos < m_UndoItemStack.GetSize())
    RemoveTails();

  if (m_UndoItemStack.GetSize() >= m_nBufSize) {
    RemoveHeads();
    m_bVirgin = FALSE;
  }

  m_UndoItemStack.Add(pItem);
  m_nCurUndoPos = m_UndoItemStack.GetSize();

  m_bModified = (m_nCurUndoPos != 0);
}

FX_BOOL CFX_Edit_Undo::IsModified() const {
  return m_bVirgin ? m_bModified : TRUE;
}

IFX_Edit_UndoItem* CFX_Edit_Undo::GetItem(int32_t nIndex) {
  if (nIndex >= 0 && nIndex < m_UndoItemStack.GetSize())
    return m_UndoItemStack.GetAt(nIndex);

  return nullptr;
}

void CFX_Edit_Undo::RemoveHeads() {
  ASSERT(m_UndoItemStack.GetSize() > 1);

  delete m_UndoItemStack.GetAt(0);
  m_UndoItemStack.RemoveAt(0);
}

void CFX_Edit_Undo::RemoveTails() {
  for (int32_t i = m_UndoItemStack.GetSize() - 1; i >= m_nCurUndoPos; i--) {
    delete m_UndoItemStack.GetAt(i);
    m_UndoItemStack.RemoveAt(i);
  }
}

void CFX_Edit_Undo::Reset() {
  for (int32_t i = 0, sz = m_UndoItemStack.GetSize(); i < sz; i++) {
    delete m_UndoItemStack.GetAt(i);
  }
  m_nCurUndoPos = 0;
  m_UndoItemStack.RemoveAll();
}

CFX_Edit_UndoItem::CFX_Edit_UndoItem() : m_bFirst(TRUE), m_bLast(TRUE) {}

CFX_Edit_UndoItem::~CFX_Edit_UndoItem() {}

CFX_WideString CFX_Edit_UndoItem::GetUndoTitle() {
  return L"";
}

void CFX_Edit_UndoItem::SetFirst(FX_BOOL bFirst) {
  m_bFirst = bFirst;
}

FX_BOOL CFX_Edit_UndoItem::IsFirst() {
  return m_bFirst;
}

void CFX_Edit_UndoItem::SetLast(FX_BOOL bLast) {
  m_bLast = bLast;
}

FX_BOOL CFX_Edit_UndoItem::IsLast() {
  return m_bLast;
}

CFX_Edit_GroupUndoItem::CFX_Edit_GroupUndoItem(const CFX_WideString& sTitle)
    : m_sTitle(sTitle) {}

CFX_Edit_GroupUndoItem::~CFX_Edit_GroupUndoItem() {
  for (int i = 0, sz = m_Items.GetSize(); i < sz; i++) {
    delete m_Items[i];
  }

  m_Items.RemoveAll();
}

void CFX_Edit_GroupUndoItem::AddUndoItem(CFX_Edit_UndoItem* pUndoItem) {
  pUndoItem->SetFirst(FALSE);
  pUndoItem->SetLast(FALSE);

  m_Items.Add(pUndoItem);

  if (m_sTitle.IsEmpty())
    m_sTitle = pUndoItem->GetUndoTitle();
}

void CFX_Edit_GroupUndoItem::UpdateItems() {
  if (m_Items.GetSize() > 0) {
    CFX_Edit_UndoItem* pFirstItem = m_Items[0];
    pFirstItem->SetFirst(TRUE);

    CFX_Edit_UndoItem* pLastItem = m_Items[m_Items.GetSize() - 1];
    pLastItem->SetLast(TRUE);
  }
}

void CFX_Edit_GroupUndoItem::Undo() {
  for (int i = m_Items.GetSize() - 1; i >= 0; i--) {
    CFX_Edit_UndoItem* pUndoItem = m_Items[i];
    pUndoItem->Undo();
  }
}

void CFX_Edit_GroupUndoItem::Redo() {
  for (int i = 0, sz = m_Items.GetSize(); i < sz; i++) {
    CFX_Edit_UndoItem* pUndoItem = m_Items[i];
    pUndoItem->Redo();
  }
}

CFX_WideString CFX_Edit_GroupUndoItem::GetUndoTitle() {
  return m_sTitle;
}

CFXEU_InsertWord::CFXEU_InsertWord(CFX_Edit* pEdit,
                                   const CPVT_WordPlace& wpOldPlace,
                                   const CPVT_WordPlace& wpNewPlace,
                                   uint16_t word,
                                   int32_t charset,
                                   const CPVT_WordProps* pWordProps)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset),
      m_WordProps() {
  if (pWordProps)
    m_WordProps = *pWordProps;
}

CFXEU_InsertWord::~CFXEU_InsertWord() {}

void CFXEU_InsertWord::Redo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpOld);
    m_pEdit->InsertWord(m_Word, m_nCharset, &m_WordProps, FALSE, TRUE);
  }
}

void CFXEU_InsertWord::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpNew);
    m_pEdit->Backspace(FALSE, TRUE);
  }
}

CFXEU_InsertReturn::CFXEU_InsertReturn(CFX_Edit* pEdit,
                                       const CPVT_WordPlace& wpOldPlace,
                                       const CPVT_WordPlace& wpNewPlace,
                                       const CPVT_SecProps* pSecProps,
                                       const CPVT_WordProps* pWordProps)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_SecProps(),
      m_WordProps() {
  if (pSecProps)
    m_SecProps = *pSecProps;
  if (pWordProps)
    m_WordProps = *pWordProps;
}

CFXEU_InsertReturn::~CFXEU_InsertReturn() {}

void CFXEU_InsertReturn::Redo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpOld);
    m_pEdit->InsertReturn(&m_SecProps, &m_WordProps, FALSE, TRUE);
  }
}

void CFXEU_InsertReturn::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpNew);
    m_pEdit->Backspace(FALSE, TRUE);
  }
}

CFXEU_Backspace::CFXEU_Backspace(CFX_Edit* pEdit,
                                 const CPVT_WordPlace& wpOldPlace,
                                 const CPVT_WordPlace& wpNewPlace,
                                 uint16_t word,
                                 int32_t charset,
                                 const CPVT_SecProps& SecProps,
                                 const CPVT_WordProps& WordProps)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset),
      m_SecProps(SecProps),
      m_WordProps(WordProps) {}

CFXEU_Backspace::~CFXEU_Backspace() {}

void CFXEU_Backspace::Redo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpOld);
    m_pEdit->Backspace(FALSE, TRUE);
  }
}

void CFXEU_Backspace::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpNew);
    if (m_wpNew.SecCmp(m_wpOld) != 0) {
      m_pEdit->InsertReturn(&m_SecProps, &m_WordProps, FALSE, TRUE);
    } else {
      m_pEdit->InsertWord(m_Word, m_nCharset, &m_WordProps, FALSE, TRUE);
    }
  }
}

CFXEU_Delete::CFXEU_Delete(CFX_Edit* pEdit,
                           const CPVT_WordPlace& wpOldPlace,
                           const CPVT_WordPlace& wpNewPlace,
                           uint16_t word,
                           int32_t charset,
                           const CPVT_SecProps& SecProps,
                           const CPVT_WordProps& WordProps,
                           FX_BOOL bSecEnd)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_Word(word),
      m_nCharset(charset),
      m_SecProps(SecProps),
      m_WordProps(WordProps),
      m_bSecEnd(bSecEnd) {}

CFXEU_Delete::~CFXEU_Delete() {}

void CFXEU_Delete::Redo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpOld);
    m_pEdit->Delete(FALSE, TRUE);
  }
}

void CFXEU_Delete::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpNew);
    if (m_bSecEnd) {
      m_pEdit->InsertReturn(&m_SecProps, &m_WordProps, FALSE, TRUE);
    } else {
      m_pEdit->InsertWord(m_Word, m_nCharset, &m_WordProps, FALSE, TRUE);
    }
  }
}

CFXEU_Clear::CFXEU_Clear(CFX_Edit* pEdit,
                         const CPVT_WordRange& wrSel,
                         const CFX_WideString& swText)
    : m_pEdit(pEdit), m_wrSel(wrSel), m_swText(swText) {}

CFXEU_Clear::~CFXEU_Clear() {}

void CFXEU_Clear::Redo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetSel(m_wrSel.BeginPos, m_wrSel.EndPos);
    m_pEdit->Clear(FALSE, TRUE);
  }
}

void CFXEU_Clear::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wrSel.BeginPos);
    m_pEdit->InsertText(m_swText.c_str(), DEFAULT_CHARSET, nullptr, nullptr,
                        FALSE, TRUE);
    m_pEdit->SetSel(m_wrSel.BeginPos, m_wrSel.EndPos);
  }
}

CFXEU_InsertText::CFXEU_InsertText(CFX_Edit* pEdit,
                                   const CPVT_WordPlace& wpOldPlace,
                                   const CPVT_WordPlace& wpNewPlace,
                                   const CFX_WideString& swText,
                                   int32_t charset,
                                   const CPVT_SecProps* pSecProps,
                                   const CPVT_WordProps* pWordProps)
    : m_pEdit(pEdit),
      m_wpOld(wpOldPlace),
      m_wpNew(wpNewPlace),
      m_swText(swText),
      m_nCharset(charset),
      m_SecProps(),
      m_WordProps() {
  if (pSecProps)
    m_SecProps = *pSecProps;
  if (pWordProps)
    m_WordProps = *pWordProps;
}

CFXEU_InsertText::~CFXEU_InsertText() {}

void CFXEU_InsertText::Redo() {
  if (m_pEdit && IsLast()) {
    m_pEdit->SelectNone();
    m_pEdit->SetCaret(m_wpOld);
    m_pEdit->InsertText(m_swText.c_str(), m_nCharset, &m_SecProps, &m_WordProps,
                        FALSE, TRUE);
  }
}

void CFXEU_InsertText::Undo() {
  if (m_pEdit) {
    m_pEdit->SelectNone();
    m_pEdit->SetSel(m_wpOld, m_wpNew);
    m_pEdit->Clear(FALSE, TRUE);
  }
}

// static
CFX_ByteString CFX_Edit::GetEditAppearanceStream(CFX_Edit* pEdit,
                                                 const CFX_FloatPoint& ptOffset,
                                                 const CPVT_WordRange* pRange,
                                                 FX_BOOL bContinuous,
                                                 uint16_t SubWord) {
  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (pRange)
    pIterator->SetAt(pRange->BeginPos);
  else
    pIterator->SetAt(0);

  CFX_ByteTextBuf sEditStream;
  CFX_ByteTextBuf sWords;
  int32_t nCurFontIndex = -1;
  CFX_FloatPoint ptOld(0.0f, 0.0f);
  CFX_FloatPoint ptNew(0.0f, 0.0f);
  CPVT_WordPlace oldplace;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();

    if (pRange && place.WordCmp(pRange->EndPos) > 0)
      break;

    if (bContinuous) {
      if (place.LineCmp(oldplace) != 0) {
        if (sWords.GetSize() > 0) {
          sEditStream << GetWordRenderString(sWords.MakeString());
          sWords.Clear();
        }

        CPVT_Word word;
        if (pIterator->GetWord(word)) {
          ptNew = CFX_FloatPoint(word.ptWord.x + ptOffset.x,
                                 word.ptWord.y + ptOffset.y);
        } else {
          CPVT_Line line;
          pIterator->GetLine(line);
          ptNew = CFX_FloatPoint(line.ptLine.x + ptOffset.x,
                                 line.ptLine.y + ptOffset.y);
        }

        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";

          ptOld = ptNew;
        }
      }

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (word.nFontIndex != nCurFontIndex) {
          if (sWords.GetSize() > 0) {
            sEditStream << GetWordRenderString(sWords.MakeString());
            sWords.Clear();
          }
          sEditStream << GetFontSetString(pEdit->GetFontMap(), word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }

        sWords << GetPDFWordString(pEdit->GetFontMap(), nCurFontIndex,
                                   word.Word, SubWord);
      }

      oldplace = place;
    } else {
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        ptNew = CFX_FloatPoint(word.ptWord.x + ptOffset.x,
                               word.ptWord.y + ptOffset.y);

        if (ptNew.x != ptOld.x || ptNew.y != ptOld.y) {
          sEditStream << ptNew.x - ptOld.x << " " << ptNew.y - ptOld.y
                      << " Td\n";
          ptOld = ptNew;
        }

        if (word.nFontIndex != nCurFontIndex) {
          sEditStream << GetFontSetString(pEdit->GetFontMap(), word.nFontIndex,
                                          word.fFontSize);
          nCurFontIndex = word.nFontIndex;
        }

        sEditStream << GetWordRenderString(GetPDFWordString(
            pEdit->GetFontMap(), nCurFontIndex, word.Word, SubWord));
      }
    }
  }

  if (sWords.GetSize() > 0) {
    sEditStream << GetWordRenderString(sWords.MakeString());
    sWords.Clear();
  }

  CFX_ByteTextBuf sAppStream;
  if (sEditStream.GetSize() > 0) {
    int32_t nHorzScale = pEdit->GetHorzScale();
    if (nHorzScale != 100) {
      sAppStream << nHorzScale << " Tz\n";
    }

    FX_FLOAT fCharSpace = pEdit->GetCharSpace();
    if (!FX_EDIT_IsFloatZero(fCharSpace)) {
      sAppStream << fCharSpace << " Tc\n";
    }

    sAppStream << sEditStream;
  }

  return sAppStream.MakeString();
}

// static
CFX_ByteString CFX_Edit::GetSelectAppearanceStream(
    CFX_Edit* pEdit,
    const CFX_FloatPoint& ptOffset,
    const CPVT_WordRange* pRange) {
  if (!pRange || !pRange->IsExist())
    return CFX_ByteString();

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  pIterator->SetAt(pRange->BeginPos);

  CFX_ByteTextBuf sRet;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();
    if (place.WordCmp(pRange->EndPos) > 0)
      break;

    CPVT_Word word;
    CPVT_Line line;
    if (pIterator->GetWord(word) && pIterator->GetLine(line)) {
      sRet << word.ptWord.x + ptOffset.x << " "
           << line.ptLine.y + line.fLineDescent << " " << word.fWidth << " "
           << line.fLineAscent - line.fLineDescent << " re\nf\n";
    }
  }

  return sRet.MakeString();
}

// static
void CFX_Edit::DrawUnderline(CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             CFX_Edit* pEdit,
                             FX_COLORREF color,
                             const CFX_FloatRect& rcClip,
                             const CFX_FloatPoint& ptOffset,
                             const CPVT_WordRange* pRange) {
  pDevice->SaveState();

  if (!rcClip.IsEmpty()) {
    CFX_FloatRect rcTemp = rcClip;
    pUser2Device->TransformRect(rcTemp);
    pDevice->SetClip_Rect(rcTemp.ToFxRect());
  }

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        CFX_PathData pathUnderline;
        CFX_FloatRect rcUnderline = GetUnderLineRect(word);
        rcUnderline.left += ptOffset.x;
        rcUnderline.right += ptOffset.x;
        rcUnderline.top += ptOffset.y;
        rcUnderline.bottom += ptOffset.y;
        pathUnderline.AppendRect(rcUnderline.left, rcUnderline.bottom,
                                 rcUnderline.right, rcUnderline.top);

        pDevice->DrawPath(&pathUnderline, pUser2Device, nullptr, color, 0,
                          FXFILL_WINDING);
      }
    }
  }

  pDevice->RestoreState(false);
}

// static
void CFX_Edit::DrawEdit(CFX_RenderDevice* pDevice,
                        CFX_Matrix* pUser2Device,
                        CFX_Edit* pEdit,
                        FX_COLORREF crTextFill,
                        FX_COLORREF crTextStroke,
                        const CFX_FloatRect& rcClip,
                        const CFX_FloatPoint& ptOffset,
                        const CPVT_WordRange* pRange,
                        CFX_SystemHandler* pSystemHandler,
                        void* pFFLData) {
  const bool bContinuous =
      pEdit->GetCharArray() == 0 && pEdit->GetCharSpace() <= 0.0f;
  uint16_t SubWord = pEdit->GetPasswordChar();
  FX_FLOAT fFontSize = pEdit->GetFontSize();
  CPVT_WordRange wrSelect = pEdit->GetSelectWordRange();
  int32_t nHorzScale = pEdit->GetHorzScale();

  FX_COLORREF crCurFill = crTextFill;
  FX_COLORREF crOldFill = crCurFill;

  FX_BOOL bSelect = FALSE;
  const FX_COLORREF crWhite = ArgbEncode(255, 255, 255, 255);
  const FX_COLORREF crSelBK = ArgbEncode(255, 0, 51, 113);

  CFX_ByteTextBuf sTextBuf;
  int32_t nFontIndex = -1;
  CFX_FloatPoint ptBT(0.0f, 0.0f);

  pDevice->SaveState();

  if (!rcClip.IsEmpty()) {
    CFX_FloatRect rcTemp = rcClip;
    pUser2Device->TransformRect(rcTemp);
    pDevice->SetClip_Rect(rcTemp.ToFxRect());
  }

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (IPVT_FontMap* pFontMap = pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    CPVT_WordPlace oldplace;
    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      if (wrSelect.IsExist()) {
        bSelect = place.WordCmp(wrSelect.BeginPos) > 0 &&
                  place.WordCmp(wrSelect.EndPos) <= 0;
        crCurFill = bSelect ? crWhite : crTextFill;
      }
      if (pSystemHandler && pSystemHandler->IsSelectionImplemented()) {
        crCurFill = crTextFill;
        crOldFill = crCurFill;
      }
      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (bSelect) {
          CPVT_Line line;
          pIterator->GetLine(line);

          if (pSystemHandler && pSystemHandler->IsSelectionImplemented()) {
            CFX_FloatRect rc(word.ptWord.x, line.ptLine.y + line.fLineDescent,
                             word.ptWord.x + word.fWidth,
                             line.ptLine.y + line.fLineAscent);
            rc.Intersect(rcClip);
            pSystemHandler->OutputSelectedRect(pFFLData, rc);
          } else {
            CFX_PathData pathSelBK;
            pathSelBK.AppendRect(
                word.ptWord.x, line.ptLine.y + line.fLineDescent,
                word.ptWord.x + word.fWidth, line.ptLine.y + line.fLineAscent);

            pDevice->DrawPath(&pathSelBK, pUser2Device, nullptr, crSelBK, 0,
                              FXFILL_WINDING);
          }
        }

        if (bContinuous) {
          if (place.LineCmp(oldplace) != 0 || word.nFontIndex != nFontIndex ||
              crOldFill != crCurFill) {
            if (sTextBuf.GetLength() > 0) {
              DrawTextString(
                  pDevice,
                  CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                  pFontMap->GetPDFFont(nFontIndex), fFontSize, pUser2Device,
                  sTextBuf.MakeString(), crOldFill, crTextStroke, nHorzScale);

              sTextBuf.Clear();
            }
            nFontIndex = word.nFontIndex;
            ptBT = word.ptWord;
            crOldFill = crCurFill;
          }

          sTextBuf << GetPDFWordString(pFontMap, word.nFontIndex, word.Word,
                                       SubWord)
                          .AsStringC();
        } else {
          DrawTextString(
              pDevice, CFX_FloatPoint(word.ptWord.x + ptOffset.x,
                                      word.ptWord.y + ptOffset.y),
              pFontMap->GetPDFFont(word.nFontIndex), fFontSize, pUser2Device,
              GetPDFWordString(pFontMap, word.nFontIndex, word.Word, SubWord),
              crCurFill, crTextStroke, nHorzScale);
        }
        oldplace = place;
      }
    }

    if (sTextBuf.GetLength() > 0) {
      DrawTextString(
          pDevice, CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
          pFontMap->GetPDFFont(nFontIndex), fFontSize, pUser2Device,
          sTextBuf.MakeString(), crOldFill, crTextStroke, nHorzScale);
    }
  }

  pDevice->RestoreState(false);
}

// static
void CFX_Edit::DrawRichEdit(CFX_RenderDevice* pDevice,
                            CFX_Matrix* pUser2Device,
                            CFX_Edit* pEdit,
                            const CFX_FloatRect& rcClip,
                            const CFX_FloatPoint& ptOffset,
                            const CPVT_WordRange* pRange) {
  CPVT_WordRange wrSelect = pEdit->GetSelectWordRange();

  FX_COLORREF crCurText = ArgbEncode(255, 0, 0, 0);
  FX_COLORREF crOld = crCurText;
  FX_BOOL bSelect = FALSE;
  const FX_COLORREF crWhite = ArgbEncode(255, 255, 255, 255);
  const FX_COLORREF crSelBK = ArgbEncode(255, 0, 51, 113);

  CFX_ByteTextBuf sTextBuf;
  CPVT_WordProps wp;
  CFX_FloatPoint ptBT(0.0f, 0.0f);

  pDevice->SaveState();

  if (!rcClip.IsEmpty()) {
    CFX_FloatRect rcTemp = rcClip;
    pUser2Device->TransformRect(rcTemp);
    pDevice->SetClip_Rect(rcTemp.ToFxRect());
  }

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (IPVT_FontMap* pFontMap = pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    CPVT_WordPlace oldplace;

    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        word.WordProps.fFontSize = word.fFontSize;

        crCurText = ArgbEncode(255, word.WordProps.dwWordColor);

        if (wrSelect.IsExist()) {
          bSelect = place.WordCmp(wrSelect.BeginPos) > 0 &&
                    place.WordCmp(wrSelect.EndPos) <= 0;
          if (bSelect) {
            crCurText = crWhite;
          }
        }

        if (bSelect) {
          CPVT_Line line;
          pIterator->GetLine(line);

          CFX_PathData pathSelBK;
          pathSelBK.AppendRect(word.ptWord.x + ptOffset.x,
                               line.ptLine.y + line.fLineDescent + ptOffset.y,
                               word.ptWord.x + word.fWidth + ptOffset.x,
                               line.ptLine.y + line.fLineAscent + ptOffset.y);

          pDevice->DrawPath(&pathSelBK, pUser2Device, nullptr, crSelBK, 0,
                            FXFILL_WINDING);
        }

        if (place.LineCmp(oldplace) != 0 || word.WordProps.fCharSpace > 0.0f ||
            word.WordProps.nHorzScale != 100 ||
            FXSYS_memcmp(&word.WordProps, &wp, sizeof(CPVT_WordProps)) != 0 ||
            crOld != crCurText) {
          if (sTextBuf.GetLength() > 0) {
            DrawTextString(
                pDevice,
                CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                pFontMap->GetPDFFont(wp.nFontIndex), wp.fFontSize, pUser2Device,
                sTextBuf.MakeString(), crOld, 0, wp.nHorzScale);

            sTextBuf.Clear();
          }
          wp = word.WordProps;
          ptBT = word.ptWord;
          crOld = crCurText;
        }

        sTextBuf << GetPDFWordString(pFontMap, word.WordProps.nFontIndex,
                                     word.Word, 0)
                        .AsStringC();

        if (word.WordProps.nWordStyle & PVTWORD_STYLE_UNDERLINE) {
          CFX_PathData pathUnderline;
          CFX_FloatRect rcUnderline = GetUnderLineRect(word);
          pathUnderline.AppendRect(rcUnderline.left, rcUnderline.bottom,
                                   rcUnderline.right, rcUnderline.top);

          pDevice->DrawPath(&pathUnderline, pUser2Device, nullptr, crCurText, 0,
                            FXFILL_WINDING);
        }

        if (word.WordProps.nWordStyle & PVTWORD_STYLE_CROSSOUT) {
          CFX_PathData pathCrossout;
          CFX_FloatRect rcCrossout = GetCrossoutRect(word);
          pathCrossout.AppendRect(rcCrossout.left, rcCrossout.bottom,
                                  rcCrossout.right, rcCrossout.top);

          pDevice->DrawPath(&pathCrossout, pUser2Device, nullptr, crCurText, 0,
                            FXFILL_WINDING);
        }

        oldplace = place;
      }
    }

    if (sTextBuf.GetLength() > 0) {
      DrawTextString(
          pDevice, CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
          pFontMap->GetPDFFont(wp.nFontIndex), wp.fFontSize, pUser2Device,
          sTextBuf.MakeString(), crOld, 0, wp.nHorzScale);
    }
  }

  pDevice->RestoreState(false);
}

// static
void CFX_Edit::GeneratePageObjects(
    CPDF_PageObjectHolder* pObjectHolder,
    CFX_Edit* pEdit,
    const CFX_FloatPoint& ptOffset,
    const CPVT_WordRange* pRange,
    FX_COLORREF crText,
    CFX_ArrayTemplate<CPDF_TextObject*>& ObjArray) {
  FX_FLOAT fFontSize = pEdit->GetFontSize();

  int32_t nOldFontIndex = -1;

  CFX_ByteTextBuf sTextBuf;
  CFX_FloatPoint ptBT(0.0f, 0.0f);

  ObjArray.RemoveAll();

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (IPVT_FontMap* pFontMap = pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    CPVT_WordPlace oldplace;

    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        if (place.LineCmp(oldplace) != 0 || nOldFontIndex != word.nFontIndex) {
          if (sTextBuf.GetLength() > 0) {
            ObjArray.Add(AddTextObjToPageObjects(
                pObjectHolder, crText, pFontMap->GetPDFFont(nOldFontIndex),
                fFontSize, 0.0f, 100,
                CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                sTextBuf.MakeString()));

            sTextBuf.Clear();
          }

          ptBT = word.ptWord;
          nOldFontIndex = word.nFontIndex;
        }

        sTextBuf << GetPDFWordString(pFontMap, word.nFontIndex, word.Word, 0)
                        .AsStringC();
        oldplace = place;
      }
    }

    if (sTextBuf.GetLength() > 0) {
      ObjArray.Add(AddTextObjToPageObjects(
          pObjectHolder, crText, pFontMap->GetPDFFont(nOldFontIndex), fFontSize,
          0.0f, 100, CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
          sTextBuf.MakeString()));
    }
  }
}

// static
void CFX_Edit::GenerateRichPageObjects(
    CPDF_PageObjectHolder* pObjectHolder,
    CFX_Edit* pEdit,
    const CFX_FloatPoint& ptOffset,
    const CPVT_WordRange* pRange,
    CFX_ArrayTemplate<CPDF_TextObject*>& ObjArray) {
  FX_COLORREF crCurText = ArgbEncode(255, 0, 0, 0);
  FX_COLORREF crOld = crCurText;

  CFX_ByteTextBuf sTextBuf;
  CPVT_WordProps wp;
  CFX_FloatPoint ptBT(0.0f, 0.0f);

  ObjArray.RemoveAll();

  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (IPVT_FontMap* pFontMap = pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    CPVT_WordPlace oldplace;

    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        word.WordProps.fFontSize = word.fFontSize;

        crCurText = ArgbEncode(255, word.WordProps.dwWordColor);

        if (place.LineCmp(oldplace) != 0 || word.WordProps.fCharSpace > 0.0f ||
            word.WordProps.nHorzScale != 100 ||
            FXSYS_memcmp(&word.WordProps, &wp, sizeof(CPVT_WordProps)) != 0 ||
            crOld != crCurText) {
          if (sTextBuf.GetLength() > 0) {
            ObjArray.Add(AddTextObjToPageObjects(
                pObjectHolder, crOld, pFontMap->GetPDFFont(wp.nFontIndex),
                wp.fFontSize, wp.fCharSpace, wp.nHorzScale,
                CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
                sTextBuf.MakeString()));

            sTextBuf.Clear();
          }

          wp = word.WordProps;
          ptBT = word.ptWord;
          crOld = crCurText;
        }

        sTextBuf << GetPDFWordString(pFontMap, word.WordProps.nFontIndex,
                                     word.Word, 0)
                        .AsStringC();

        if (word.WordProps.nWordStyle & PVTWORD_STYLE_UNDERLINE) {
          CFX_FloatRect rcUnderline = GetUnderLineRect(word);
          rcUnderline.left += ptOffset.x;
          rcUnderline.right += ptOffset.x;
          rcUnderline.top += ptOffset.y;
          rcUnderline.bottom += ptOffset.y;

          AddRectToPageObjects(pObjectHolder, crCurText, rcUnderline);
        }

        if (word.WordProps.nWordStyle & PVTWORD_STYLE_CROSSOUT) {
          CFX_FloatRect rcCrossout = GetCrossoutRect(word);
          rcCrossout.left += ptOffset.x;
          rcCrossout.right += ptOffset.x;
          rcCrossout.top += ptOffset.y;
          rcCrossout.bottom += ptOffset.y;

          AddRectToPageObjects(pObjectHolder, crCurText, rcCrossout);
        }

        oldplace = place;
      }
    }

    if (sTextBuf.GetLength() > 0) {
      ObjArray.Add(AddTextObjToPageObjects(
          pObjectHolder, crOld, pFontMap->GetPDFFont(wp.nFontIndex),
          wp.fFontSize, wp.fCharSpace, wp.nHorzScale,
          CFX_FloatPoint(ptBT.x + ptOffset.x, ptBT.y + ptOffset.y),
          sTextBuf.MakeString()));
    }
  }
}

// static
void CFX_Edit::GenerateUnderlineObjects(CPDF_PageObjectHolder* pObjectHolder,
                                        CFX_Edit* pEdit,
                                        const CFX_FloatPoint& ptOffset,
                                        const CPVT_WordRange* pRange,
                                        FX_COLORREF color) {
  CFX_Edit_Iterator* pIterator = pEdit->GetIterator();
  if (pEdit->GetFontMap()) {
    if (pRange)
      pIterator->SetAt(pRange->BeginPos);
    else
      pIterator->SetAt(0);

    CPVT_WordPlace oldplace;

    while (pIterator->NextWord()) {
      CPVT_WordPlace place = pIterator->GetAt();
      if (pRange && place.WordCmp(pRange->EndPos) > 0)
        break;

      CPVT_Word word;
      if (pIterator->GetWord(word)) {
        CFX_FloatRect rcUnderline = GetUnderLineRect(word);
        rcUnderline.left += ptOffset.x;
        rcUnderline.right += ptOffset.x;
        rcUnderline.top += ptOffset.y;
        rcUnderline.bottom += ptOffset.y;
        AddRectToPageObjects(pObjectHolder, color, rcUnderline);
      }
    }
  }
}

CFX_Edit::CFX_Edit()
    : m_pVT(new CPDF_VariableText),
      m_pNotify(nullptr),
      m_pOprNotify(nullptr),
      m_wpCaret(-1, -1, -1),
      m_wpOldCaret(-1, -1, -1),
      m_SelState(),
      m_ptScrollPos(0, 0),
      m_ptRefreshScrollPos(0, 0),
      m_bEnableScroll(FALSE),
      m_ptCaret(0.0f, 0.0f),
      m_Undo(kEditUndoMaxItems),
      m_nAlignment(0),
      m_bNotifyFlag(FALSE),
      m_bEnableOverflow(FALSE),
      m_bEnableRefresh(TRUE),
      m_rcOldContent(0.0f, 0.0f, 0.0f, 0.0f),
      m_bEnableUndo(TRUE),
      m_bOprNotify(FALSE),
      m_pGroupUndoItem(nullptr) {}

CFX_Edit::~CFX_Edit() {
  ASSERT(!m_pGroupUndoItem);
}

void CFX_Edit::Initialize() {
  m_pVT->Initialize();
  SetCaret(m_pVT->GetBeginWordPlace());
  SetCaretOrigin();
}

void CFX_Edit::SetFontMap(IPVT_FontMap* pFontMap) {
  m_pVTProvider.reset(new CFX_Edit_Provider(pFontMap));
  m_pVT->SetProvider(m_pVTProvider.get());
}

void CFX_Edit::SetNotify(CPWL_EditCtrl* pNotify) {
  m_pNotify = pNotify;
}

void CFX_Edit::SetOprNotify(CPWL_Edit* pOprNotify) {
  m_pOprNotify = pOprNotify;
}

CFX_Edit_Iterator* CFX_Edit::GetIterator() {
  if (!m_pIterator)
    m_pIterator.reset(new CFX_Edit_Iterator(this, m_pVT->GetIterator()));
  return m_pIterator.get();
}

CPDF_VariableText* CFX_Edit::GetVariableText() {
  return m_pVT.get();
}

IPVT_FontMap* CFX_Edit::GetFontMap() {
  return m_pVTProvider ? m_pVTProvider->GetFontMap() : nullptr;
}

void CFX_Edit::SetPlateRect(const CFX_FloatRect& rect, FX_BOOL bPaint) {
  m_pVT->SetPlateRect(rect);
  m_ptScrollPos = CFX_FloatPoint(rect.left, rect.top);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetAlignmentH(int32_t nFormat, FX_BOOL bPaint) {
  m_pVT->SetAlignment(nFormat);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetAlignmentV(int32_t nFormat, FX_BOOL bPaint) {
  m_nAlignment = nFormat;
  if (bPaint)
    Paint();
}

void CFX_Edit::SetPasswordChar(uint16_t wSubWord, FX_BOOL bPaint) {
  m_pVT->SetPasswordChar(wSubWord);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetLimitChar(int32_t nLimitChar, FX_BOOL bPaint) {
  m_pVT->SetLimitChar(nLimitChar);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetCharArray(int32_t nCharArray, FX_BOOL bPaint) {
  m_pVT->SetCharArray(nCharArray);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetCharSpace(FX_FLOAT fCharSpace, FX_BOOL bPaint) {
  m_pVT->SetCharSpace(fCharSpace);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetHorzScale(int32_t nHorzScale, FX_BOOL bPaint) {
  m_pVT->SetHorzScale(nHorzScale);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetMultiLine(FX_BOOL bMultiLine, FX_BOOL bPaint) {
  m_pVT->SetMultiLine(bMultiLine);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetAutoReturn(FX_BOOL bAuto, FX_BOOL bPaint) {
  m_pVT->SetAutoReturn(bAuto);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetLineLeading(FX_FLOAT fLineLeading, FX_BOOL bPaint) {
  m_pVT->SetLineLeading(fLineLeading);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetAutoFontSize(FX_BOOL bAuto, FX_BOOL bPaint) {
  m_pVT->SetAutoFontSize(bAuto);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetFontSize(FX_FLOAT fFontSize, FX_BOOL bPaint) {
  m_pVT->SetFontSize(fFontSize);
  if (bPaint)
    Paint();
}

void CFX_Edit::SetAutoScroll(FX_BOOL bAuto, FX_BOOL bPaint) {
  m_bEnableScroll = bAuto;
  if (bPaint)
    Paint();
}

void CFX_Edit::SetTextOverflow(FX_BOOL bAllowed, FX_BOOL bPaint) {
  m_bEnableOverflow = bAllowed;
  if (bPaint)
    Paint();
}

void CFX_Edit::SetSel(int32_t nStartChar, int32_t nEndChar) {
  if (m_pVT->IsValid()) {
    if (nStartChar == 0 && nEndChar < 0) {
      SelectAll();
    } else if (nStartChar < 0) {
      SelectNone();
    } else {
      if (nStartChar < nEndChar) {
        SetSel(m_pVT->WordIndexToWordPlace(nStartChar),
               m_pVT->WordIndexToWordPlace(nEndChar));
      } else {
        SetSel(m_pVT->WordIndexToWordPlace(nEndChar),
               m_pVT->WordIndexToWordPlace(nStartChar));
      }
    }
  }
}

void CFX_Edit::SetSel(const CPVT_WordPlace& begin, const CPVT_WordPlace& end) {
  if (m_pVT->IsValid()) {
    SelectNone();

    m_SelState.Set(begin, end);

    SetCaret(m_SelState.EndPos);

    if (m_SelState.IsExist()) {
      ScrollToCaret();
      CPVT_WordRange wr(m_SelState.BeginPos, m_SelState.EndPos);
      Refresh(RP_OPTIONAL, &wr);
      SetCaretInfo();
    } else {
      ScrollToCaret();
      SetCaretInfo();
    }
  }
}

void CFX_Edit::GetSel(int32_t& nStartChar, int32_t& nEndChar) const {
  nStartChar = -1;
  nEndChar = -1;

  if (m_pVT->IsValid()) {
    if (m_SelState.IsExist()) {
      if (m_SelState.BeginPos.WordCmp(m_SelState.EndPos) < 0) {
        nStartChar = m_pVT->WordPlaceToWordIndex(m_SelState.BeginPos);
        nEndChar = m_pVT->WordPlaceToWordIndex(m_SelState.EndPos);
      } else {
        nStartChar = m_pVT->WordPlaceToWordIndex(m_SelState.EndPos);
        nEndChar = m_pVT->WordPlaceToWordIndex(m_SelState.BeginPos);
      }
    } else {
      nStartChar = m_pVT->WordPlaceToWordIndex(m_wpCaret);
      nEndChar = m_pVT->WordPlaceToWordIndex(m_wpCaret);
    }
  }
}

int32_t CFX_Edit::GetCaret() const {
  if (m_pVT->IsValid())
    return m_pVT->WordPlaceToWordIndex(m_wpCaret);

  return -1;
}

CPVT_WordPlace CFX_Edit::GetCaretWordPlace() const {
  return m_wpCaret;
}

CFX_WideString CFX_Edit::GetText() const {
  CFX_WideString swRet;

  if (!m_pVT->IsValid())
    return swRet;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(0);

  CPVT_Word wordinfo;
  CPVT_WordPlace oldplace = pIterator->GetAt();
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();

    if (pIterator->GetWord(wordinfo))
      swRet += wordinfo.Word;

    if (oldplace.SecCmp(place) != 0)
      swRet += L"\r\n";

    oldplace = place;
  }

  return swRet;
}

CFX_WideString CFX_Edit::GetRangeText(const CPVT_WordRange& range) const {
  CFX_WideString swRet;

  if (!m_pVT->IsValid())
    return swRet;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordRange wrTemp = range;
  m_pVT->UpdateWordPlace(wrTemp.BeginPos);
  m_pVT->UpdateWordPlace(wrTemp.EndPos);
  pIterator->SetAt(wrTemp.BeginPos);

  CPVT_Word wordinfo;
  CPVT_WordPlace oldplace = wrTemp.BeginPos;
  while (pIterator->NextWord()) {
    CPVT_WordPlace place = pIterator->GetAt();
    if (place.WordCmp(wrTemp.EndPos) > 0)
      break;

    if (pIterator->GetWord(wordinfo))
      swRet += wordinfo.Word;

    if (oldplace.SecCmp(place) != 0)
      swRet += L"\r\n";

    oldplace = place;
  }

  return swRet;
}

CFX_WideString CFX_Edit::GetSelText() const {
  return GetRangeText(m_SelState.ConvertToWordRange());
}

int32_t CFX_Edit::GetTotalWords() const {
  return m_pVT->GetTotalWords();
}

int32_t CFX_Edit::GetTotalLines() const {
  int32_t nLines = 1;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(0);
  while (pIterator->NextLine())
    ++nLines;

  return nLines;
}

CPVT_WordRange CFX_Edit::GetSelectWordRange() const {
  return m_SelState.ConvertToWordRange();
}

CPVT_WordRange CFX_Edit::CombineWordRange(const CPVT_WordRange& wr1,
                                          const CPVT_WordRange& wr2) {
  CPVT_WordRange wrRet;

  if (wr1.BeginPos.WordCmp(wr2.BeginPos) < 0) {
    wrRet.BeginPos = wr1.BeginPos;
  } else {
    wrRet.BeginPos = wr2.BeginPos;
  }

  if (wr1.EndPos.WordCmp(wr2.EndPos) < 0) {
    wrRet.EndPos = wr2.EndPos;
  } else {
    wrRet.EndPos = wr1.EndPos;
  }

  return wrRet;
}

void CFX_Edit::SetText(const FX_WCHAR* text,
                       int32_t charset,
                       const CPVT_SecProps* pSecProps,
                       const CPVT_WordProps* pWordProps) {
  SetText(text, charset, pSecProps, pWordProps, TRUE, TRUE);
}

FX_BOOL CFX_Edit::InsertWord(uint16_t word,
                             int32_t charset,
                             const CPVT_WordProps* pWordProps) {
  return InsertWord(word, charset, pWordProps, TRUE, TRUE);
}

FX_BOOL CFX_Edit::InsertReturn(const CPVT_SecProps* pSecProps,
                               const CPVT_WordProps* pWordProps) {
  return InsertReturn(pSecProps, pWordProps, TRUE, TRUE);
}

FX_BOOL CFX_Edit::Backspace() {
  return Backspace(TRUE, TRUE);
}

FX_BOOL CFX_Edit::Delete() {
  return Delete(TRUE, TRUE);
}

FX_BOOL CFX_Edit::Clear() {
  return Clear(TRUE, TRUE);
}

FX_BOOL CFX_Edit::InsertText(const FX_WCHAR* text,
                             int32_t charset,
                             const CPVT_SecProps* pSecProps,
                             const CPVT_WordProps* pWordProps) {
  return InsertText(text, charset, pSecProps, pWordProps, TRUE, TRUE);
}

FX_FLOAT CFX_Edit::GetFontSize() const {
  return m_pVT->GetFontSize();
}

uint16_t CFX_Edit::GetPasswordChar() const {
  return m_pVT->GetPasswordChar();
}

int32_t CFX_Edit::GetCharArray() const {
  return m_pVT->GetCharArray();
}

CFX_FloatRect CFX_Edit::GetPlateRect() const {
  return m_pVT->GetPlateRect();
}

CFX_FloatRect CFX_Edit::GetContentRect() const {
  return VTToEdit(m_pVT->GetContentRect());
}

int32_t CFX_Edit::GetHorzScale() const {
  return m_pVT->GetHorzScale();
}

FX_FLOAT CFX_Edit::GetCharSpace() const {
  return m_pVT->GetCharSpace();
}

CPVT_WordRange CFX_Edit::GetWholeWordRange() const {
  if (m_pVT->IsValid())
    return CPVT_WordRange(m_pVT->GetBeginWordPlace(), m_pVT->GetEndWordPlace());

  return CPVT_WordRange();
}

CPVT_WordRange CFX_Edit::GetVisibleWordRange() const {
  if (m_bEnableOverflow)
    return GetWholeWordRange();

  if (m_pVT->IsValid()) {
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

    CPVT_WordPlace place1 = m_pVT->SearchWordPlace(
        EditToVT(CFX_FloatPoint(rcPlate.left, rcPlate.top)));
    CPVT_WordPlace place2 = m_pVT->SearchWordPlace(
        EditToVT(CFX_FloatPoint(rcPlate.right, rcPlate.bottom)));

    return CPVT_WordRange(place1, place2);
  }

  return CPVT_WordRange();
}

CPVT_WordPlace CFX_Edit::SearchWordPlace(const CFX_FloatPoint& point) const {
  if (m_pVT->IsValid()) {
    return m_pVT->SearchWordPlace(EditToVT(point));
  }

  return CPVT_WordPlace();
}

void CFX_Edit::Paint() {
  if (m_pVT->IsValid()) {
    RearrangeAll();
    ScrollToCaret();
    Refresh(RP_NOANALYSE);
    SetCaretOrigin();
    SetCaretInfo();
  }
}

void CFX_Edit::RearrangeAll() {
  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);
    m_pVT->RearrangeAll();
    m_pVT->UpdateWordPlace(m_wpCaret);
    SetScrollInfo();
    SetContentChanged();
  }
}

void CFX_Edit::RearrangePart(const CPVT_WordRange& range) {
  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);
    m_pVT->RearrangePart(range);
    m_pVT->UpdateWordPlace(m_wpCaret);
    SetScrollInfo();
    SetContentChanged();
  }
}

void CFX_Edit::SetContentChanged() {
  if (m_pNotify) {
    CFX_FloatRect rcContent = m_pVT->GetContentRect();
    if (rcContent.Width() != m_rcOldContent.Width() ||
        rcContent.Height() != m_rcOldContent.Height()) {
      if (!m_bNotifyFlag) {
        m_bNotifyFlag = TRUE;
        m_pNotify->IOnContentChange(rcContent);
        m_bNotifyFlag = FALSE;
      }
      m_rcOldContent = rcContent;
    }
  }
}

void CFX_Edit::SelectAll() {
  if (m_pVT->IsValid()) {
    m_SelState = CFX_Edit_Select(GetWholeWordRange());
    SetCaret(m_SelState.EndPos);

    ScrollToCaret();
    CPVT_WordRange wrVisible = GetVisibleWordRange();
    Refresh(RP_OPTIONAL, &wrVisible);
    SetCaretInfo();
  }
}

void CFX_Edit::SelectNone() {
  if (m_pVT->IsValid()) {
    if (m_SelState.IsExist()) {
      CPVT_WordRange wrTemp = m_SelState.ConvertToWordRange();
      m_SelState.Default();
      Refresh(RP_OPTIONAL, &wrTemp);
    }
  }
}

FX_BOOL CFX_Edit::IsSelected() const {
  return m_SelState.IsExist();
}

CFX_FloatPoint CFX_Edit::VTToEdit(const CFX_FloatPoint& point) const {
  CFX_FloatRect rcContent = m_pVT->GetContentRect();
  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

  FX_FLOAT fPadding = 0.0f;

  switch (m_nAlignment) {
    case 0:
      fPadding = 0.0f;
      break;
    case 1:
      fPadding = (rcPlate.Height() - rcContent.Height()) * 0.5f;
      break;
    case 2:
      fPadding = rcPlate.Height() - rcContent.Height();
      break;
  }

  return CFX_FloatPoint(point.x - (m_ptScrollPos.x - rcPlate.left),
                        point.y - (m_ptScrollPos.y + fPadding - rcPlate.top));
}

CFX_FloatPoint CFX_Edit::EditToVT(const CFX_FloatPoint& point) const {
  CFX_FloatRect rcContent = m_pVT->GetContentRect();
  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

  FX_FLOAT fPadding = 0.0f;

  switch (m_nAlignment) {
    case 0:
      fPadding = 0.0f;
      break;
    case 1:
      fPadding = (rcPlate.Height() - rcContent.Height()) * 0.5f;
      break;
    case 2:
      fPadding = rcPlate.Height() - rcContent.Height();
      break;
  }

  return CFX_FloatPoint(point.x + (m_ptScrollPos.x - rcPlate.left),
                        point.y + (m_ptScrollPos.y + fPadding - rcPlate.top));
}

CFX_FloatRect CFX_Edit::VTToEdit(const CFX_FloatRect& rect) const {
  CFX_FloatPoint ptLeftBottom =
      VTToEdit(CFX_FloatPoint(rect.left, rect.bottom));
  CFX_FloatPoint ptRightTop = VTToEdit(CFX_FloatPoint(rect.right, rect.top));

  return CFX_FloatRect(ptLeftBottom.x, ptLeftBottom.y, ptRightTop.x,
                       ptRightTop.y);
}

CFX_FloatRect CFX_Edit::EditToVT(const CFX_FloatRect& rect) const {
  CFX_FloatPoint ptLeftBottom =
      EditToVT(CFX_FloatPoint(rect.left, rect.bottom));
  CFX_FloatPoint ptRightTop = EditToVT(CFX_FloatPoint(rect.right, rect.top));

  return CFX_FloatRect(ptLeftBottom.x, ptLeftBottom.y, ptRightTop.x,
                       ptRightTop.y);
}

void CFX_Edit::SetScrollInfo() {
  if (m_pNotify) {
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();
    CFX_FloatRect rcContent = m_pVT->GetContentRect();

    if (!m_bNotifyFlag) {
      m_bNotifyFlag = TRUE;
      m_pNotify->IOnSetScrollInfoY(rcPlate.bottom, rcPlate.top,
                                   rcContent.bottom, rcContent.top,
                                   rcPlate.Height() / 3, rcPlate.Height());
      m_bNotifyFlag = FALSE;
    }
  }
}

void CFX_Edit::SetScrollPosX(FX_FLOAT fx) {
  if (!m_bEnableScroll)
    return;

  if (m_pVT->IsValid()) {
    if (!FX_EDIT_IsFloatEqual(m_ptScrollPos.x, fx)) {
      m_ptScrollPos.x = fx;
      Refresh(RP_NOANALYSE);
    }
  }
}

void CFX_Edit::SetScrollPosY(FX_FLOAT fy) {
  if (!m_bEnableScroll)
    return;

  if (m_pVT->IsValid()) {
    if (!FX_EDIT_IsFloatEqual(m_ptScrollPos.y, fy)) {
      m_ptScrollPos.y = fy;
      Refresh(RP_NOANALYSE);

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          m_bNotifyFlag = TRUE;
          m_pNotify->IOnSetScrollPosY(fy);
          m_bNotifyFlag = FALSE;
        }
      }
    }
  }
}

void CFX_Edit::SetScrollPos(const CFX_FloatPoint& point) {
  SetScrollPosX(point.x);
  SetScrollPosY(point.y);
  SetScrollLimit();
  SetCaretInfo();
}

CFX_FloatPoint CFX_Edit::GetScrollPos() const {
  return m_ptScrollPos;
}

void CFX_Edit::SetScrollLimit() {
  if (m_pVT->IsValid()) {
    CFX_FloatRect rcContent = m_pVT->GetContentRect();
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

    if (rcPlate.Width() > rcContent.Width()) {
      SetScrollPosX(rcPlate.left);
    } else {
      if (FX_EDIT_IsFloatSmaller(m_ptScrollPos.x, rcContent.left)) {
        SetScrollPosX(rcContent.left);
      } else if (FX_EDIT_IsFloatBigger(m_ptScrollPos.x,
                                       rcContent.right - rcPlate.Width())) {
        SetScrollPosX(rcContent.right - rcPlate.Width());
      }
    }

    if (rcPlate.Height() > rcContent.Height()) {
      SetScrollPosY(rcPlate.top);
    } else {
      if (FX_EDIT_IsFloatSmaller(m_ptScrollPos.y,
                                 rcContent.bottom + rcPlate.Height())) {
        SetScrollPosY(rcContent.bottom + rcPlate.Height());
      } else if (FX_EDIT_IsFloatBigger(m_ptScrollPos.y, rcContent.top)) {
        SetScrollPosY(rcContent.top);
      }
    }
  }
}

void CFX_Edit::ScrollToCaret() {
  SetScrollLimit();

  if (!m_pVT->IsValid())
    return;

  CFX_FloatPoint ptHead(0, 0);
  CFX_FloatPoint ptFoot(0, 0);

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(m_wpCaret);

  CPVT_Word word;
  CPVT_Line line;
  if (pIterator->GetWord(word)) {
    ptHead.x = word.ptWord.x + word.fWidth;
    ptHead.y = word.ptWord.y + word.fAscent;
    ptFoot.x = word.ptWord.x + word.fWidth;
    ptFoot.y = word.ptWord.y + word.fDescent;
  } else if (pIterator->GetLine(line)) {
    ptHead.x = line.ptLine.x;
    ptHead.y = line.ptLine.y + line.fLineAscent;
    ptFoot.x = line.ptLine.x;
    ptFoot.y = line.ptLine.y + line.fLineDescent;
  }

  CFX_FloatPoint ptHeadEdit = VTToEdit(ptHead);
  CFX_FloatPoint ptFootEdit = VTToEdit(ptFoot);

  CFX_FloatRect rcPlate = m_pVT->GetPlateRect();

  if (!FX_EDIT_IsFloatEqual(rcPlate.left, rcPlate.right)) {
    if (FX_EDIT_IsFloatSmaller(ptHeadEdit.x, rcPlate.left) ||
        FX_EDIT_IsFloatEqual(ptHeadEdit.x, rcPlate.left)) {
      SetScrollPosX(ptHead.x);
    } else if (FX_EDIT_IsFloatBigger(ptHeadEdit.x, rcPlate.right)) {
      SetScrollPosX(ptHead.x - rcPlate.Width());
    }
  }

  if (!FX_EDIT_IsFloatEqual(rcPlate.top, rcPlate.bottom)) {
    if (FX_EDIT_IsFloatSmaller(ptFootEdit.y, rcPlate.bottom) ||
        FX_EDIT_IsFloatEqual(ptFootEdit.y, rcPlate.bottom)) {
      if (FX_EDIT_IsFloatSmaller(ptHeadEdit.y, rcPlate.top)) {
        SetScrollPosY(ptFoot.y + rcPlate.Height());
      }
    } else if (FX_EDIT_IsFloatBigger(ptHeadEdit.y, rcPlate.top)) {
      if (FX_EDIT_IsFloatBigger(ptFootEdit.y, rcPlate.bottom)) {
        SetScrollPosY(ptHead.y);
      }
    }
  }
}

void CFX_Edit::Refresh(REFRESH_PLAN_E ePlan,
                       const CPVT_WordRange* pRange1,
                       const CPVT_WordRange* pRange2) {
  if (m_bEnableRefresh && m_pVT->IsValid()) {
    m_Refresh.BeginRefresh();
    RefreshPushLineRects(GetVisibleWordRange());

    m_Refresh.NoAnalyse();
    m_ptRefreshScrollPos = m_ptScrollPos;

    if (m_pNotify) {
      if (!m_bNotifyFlag) {
        m_bNotifyFlag = TRUE;
        if (const CFX_Edit_RectArray* pRects = m_Refresh.GetRefreshRects()) {
          for (int32_t i = 0, sz = pRects->GetSize(); i < sz; i++)
            m_pNotify->IOnInvalidateRect(pRects->GetAt(i));
        }
        m_bNotifyFlag = FALSE;
      }
    }

    m_Refresh.EndRefresh();
  }
}

void CFX_Edit::RefreshPushLineRects(const CPVT_WordRange& wr) {
  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordPlace wpBegin = wr.BeginPos;
  m_pVT->UpdateWordPlace(wpBegin);
  CPVT_WordPlace wpEnd = wr.EndPos;
  m_pVT->UpdateWordPlace(wpEnd);
  pIterator->SetAt(wpBegin);

  CPVT_Line lineinfo;
  do {
    if (!pIterator->GetLine(lineinfo))
      break;
    if (lineinfo.lineplace.LineCmp(wpEnd) > 0)
      break;

    CFX_FloatRect rcLine(lineinfo.ptLine.x,
                         lineinfo.ptLine.y + lineinfo.fLineDescent,
                         lineinfo.ptLine.x + lineinfo.fLineWidth,
                         lineinfo.ptLine.y + lineinfo.fLineAscent);

    m_Refresh.Push(CPVT_WordRange(lineinfo.lineplace, lineinfo.lineEnd),
                   VTToEdit(rcLine));
  } while (pIterator->NextLine());
}

void CFX_Edit::RefreshPushRandomRects(const CPVT_WordRange& wr) {
  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordRange wrTemp = wr;

  m_pVT->UpdateWordPlace(wrTemp.BeginPos);
  m_pVT->UpdateWordPlace(wrTemp.EndPos);
  pIterator->SetAt(wrTemp.BeginPos);

  CPVT_Word wordinfo;
  CPVT_Line lineinfo;
  CPVT_WordPlace place;

  while (pIterator->NextWord()) {
    place = pIterator->GetAt();
    if (place.WordCmp(wrTemp.EndPos) > 0)
      break;

    pIterator->GetWord(wordinfo);
    pIterator->GetLine(lineinfo);

    if (place.LineCmp(wrTemp.BeginPos) == 0 ||
        place.LineCmp(wrTemp.EndPos) == 0) {
      CFX_FloatRect rcWord(wordinfo.ptWord.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           wordinfo.ptWord.x + wordinfo.fWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      m_Refresh.AddRefresh(VTToEdit(rcWord));
    } else {
      CFX_FloatRect rcLine(lineinfo.ptLine.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           lineinfo.ptLine.x + lineinfo.fLineWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      m_Refresh.AddRefresh(VTToEdit(rcLine));

      pIterator->NextLine();
    }
  }
}

void CFX_Edit::RefreshWordRange(const CPVT_WordRange& wr) {
  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordRange wrTemp = wr;

  m_pVT->UpdateWordPlace(wrTemp.BeginPos);
  m_pVT->UpdateWordPlace(wrTemp.EndPos);
  pIterator->SetAt(wrTemp.BeginPos);

  CPVT_Word wordinfo;
  CPVT_Line lineinfo;
  CPVT_WordPlace place;

  while (pIterator->NextWord()) {
    place = pIterator->GetAt();
    if (place.WordCmp(wrTemp.EndPos) > 0)
      break;

    pIterator->GetWord(wordinfo);
    pIterator->GetLine(lineinfo);

    if (place.LineCmp(wrTemp.BeginPos) == 0 ||
        place.LineCmp(wrTemp.EndPos) == 0) {
      CFX_FloatRect rcWord(wordinfo.ptWord.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           wordinfo.ptWord.x + wordinfo.fWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          m_bNotifyFlag = TRUE;
          CFX_FloatRect rcRefresh = VTToEdit(rcWord);
          m_pNotify->IOnInvalidateRect(&rcRefresh);
          m_bNotifyFlag = FALSE;
        }
      }
    } else {
      CFX_FloatRect rcLine(lineinfo.ptLine.x,
                           lineinfo.ptLine.y + lineinfo.fLineDescent,
                           lineinfo.ptLine.x + lineinfo.fLineWidth,
                           lineinfo.ptLine.y + lineinfo.fLineAscent);

      if (m_pNotify) {
        if (!m_bNotifyFlag) {
          m_bNotifyFlag = TRUE;
          CFX_FloatRect rcRefresh = VTToEdit(rcLine);
          m_pNotify->IOnInvalidateRect(&rcRefresh);
          m_bNotifyFlag = FALSE;
        }
      }

      pIterator->NextLine();
    }
  }
}

void CFX_Edit::SetCaret(const CPVT_WordPlace& place) {
  m_wpOldCaret = m_wpCaret;
  m_wpCaret = place;
}

void CFX_Edit::SetCaretInfo() {
  if (m_pNotify) {
    if (!m_bNotifyFlag) {
      CFX_FloatPoint ptHead(0.0f, 0.0f), ptFoot(0.0f, 0.0f);

      CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
      pIterator->SetAt(m_wpCaret);
      CPVT_Word word;
      CPVT_Line line;
      if (pIterator->GetWord(word)) {
        ptHead.x = word.ptWord.x + word.fWidth;
        ptHead.y = word.ptWord.y + word.fAscent;
        ptFoot.x = word.ptWord.x + word.fWidth;
        ptFoot.y = word.ptWord.y + word.fDescent;
      } else if (pIterator->GetLine(line)) {
        ptHead.x = line.ptLine.x;
        ptHead.y = line.ptLine.y + line.fLineAscent;
        ptFoot.x = line.ptLine.x;
        ptFoot.y = line.ptLine.y + line.fLineDescent;
      }

      m_bNotifyFlag = TRUE;
      m_pNotify->IOnSetCaret(!m_SelState.IsExist(), VTToEdit(ptHead),
                             VTToEdit(ptFoot), m_wpCaret);
      m_bNotifyFlag = FALSE;
    }
  }
}

void CFX_Edit::SetCaret(int32_t nPos) {
  if (m_pVT->IsValid()) {
    SelectNone();
    SetCaret(m_pVT->WordIndexToWordPlace(nPos));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    ScrollToCaret();
    SetCaretOrigin();
    SetCaretInfo();
  }
}

void CFX_Edit::OnMouseDown(const CFX_FloatPoint& point,
                           FX_BOOL bShift,
                           FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    SelectNone();
    SetCaret(m_pVT->SearchWordPlace(EditToVT(point)));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    ScrollToCaret();
    SetCaretOrigin();
    SetCaretInfo();
  }
}

void CFX_Edit::OnMouseMove(const CFX_FloatPoint& point,
                           FX_BOOL bShift,
                           FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    SetCaret(m_pVT->SearchWordPlace(EditToVT(point)));

    if (m_wpCaret != m_wpOldCaret) {
      m_SelState.SetEndPos(m_wpCaret);

      ScrollToCaret();
      CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
      Refresh(RP_OPTIONAL, &wr);
      SetCaretOrigin();
      SetCaretInfo();
    }
  }
}

void CFX_Edit::OnVK_UP(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    SetCaret(m_pVT->GetUpWordPlace(m_wpCaret, m_ptCaret));

    if (bShift) {
      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      if (m_wpOldCaret != m_wpCaret) {
        ScrollToCaret();
        CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
        Refresh(RP_OPTIONAL, &wr);
        SetCaretInfo();
      }
    } else {
      SelectNone();

      ScrollToCaret();
      SetCaretInfo();
    }
  }
}

void CFX_Edit::OnVK_DOWN(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    SetCaret(m_pVT->GetDownWordPlace(m_wpCaret, m_ptCaret));

    if (bShift) {
      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      if (m_wpOldCaret != m_wpCaret) {
        ScrollToCaret();
        CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
        Refresh(RP_OPTIONAL, &wr);
        SetCaretInfo();
      }
    } else {
      SelectNone();

      ScrollToCaret();
      SetCaretInfo();
    }
  }
}

void CFX_Edit::OnVK_LEFT(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    if (bShift) {
      if (m_wpCaret == m_pVT->GetLineBeginPlace(m_wpCaret) &&
          m_wpCaret != m_pVT->GetSectionBeginPlace(m_wpCaret))
        SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));

      SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));

      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      if (m_wpOldCaret != m_wpCaret) {
        ScrollToCaret();
        CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
        Refresh(RP_OPTIONAL, &wr);
        SetCaretInfo();
      }
    } else {
      if (m_SelState.IsExist()) {
        if (m_SelState.BeginPos.WordCmp(m_SelState.EndPos) < 0)
          SetCaret(m_SelState.BeginPos);
        else
          SetCaret(m_SelState.EndPos);

        SelectNone();
        ScrollToCaret();
        SetCaretInfo();
      } else {
        if (m_wpCaret == m_pVT->GetLineBeginPlace(m_wpCaret) &&
            m_wpCaret != m_pVT->GetSectionBeginPlace(m_wpCaret))
          SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));

        SetCaret(m_pVT->GetPrevWordPlace(m_wpCaret));

        ScrollToCaret();
        SetCaretOrigin();
        SetCaretInfo();
      }
    }
  }
}

void CFX_Edit::OnVK_RIGHT(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    if (bShift) {
      SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));

      if (m_wpCaret == m_pVT->GetLineEndPlace(m_wpCaret) &&
          m_wpCaret != m_pVT->GetSectionEndPlace(m_wpCaret))
        SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));

      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      if (m_wpOldCaret != m_wpCaret) {
        ScrollToCaret();
        CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
        Refresh(RP_OPTIONAL, &wr);
        SetCaretInfo();
      }
    } else {
      if (m_SelState.IsExist()) {
        if (m_SelState.BeginPos.WordCmp(m_SelState.EndPos) > 0)
          SetCaret(m_SelState.BeginPos);
        else
          SetCaret(m_SelState.EndPos);

        SelectNone();
        ScrollToCaret();
        SetCaretInfo();
      } else {
        SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));

        if (m_wpCaret == m_pVT->GetLineEndPlace(m_wpCaret) &&
            m_wpCaret != m_pVT->GetSectionEndPlace(m_wpCaret))
          SetCaret(m_pVT->GetNextWordPlace(m_wpCaret));

        ScrollToCaret();
        SetCaretOrigin();
        SetCaretInfo();
      }
    }
  }
}

void CFX_Edit::OnVK_HOME(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    if (bShift) {
      if (bCtrl)
        SetCaret(m_pVT->GetBeginWordPlace());
      else
        SetCaret(m_pVT->GetLineBeginPlace(m_wpCaret));

      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      ScrollToCaret();
      CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
      Refresh(RP_OPTIONAL, &wr);
      SetCaretInfo();
    } else {
      if (m_SelState.IsExist()) {
        if (m_SelState.BeginPos.WordCmp(m_SelState.EndPos) < 0)
          SetCaret(m_SelState.BeginPos);
        else
          SetCaret(m_SelState.EndPos);

        SelectNone();
        ScrollToCaret();
        SetCaretInfo();
      } else {
        if (bCtrl)
          SetCaret(m_pVT->GetBeginWordPlace());
        else
          SetCaret(m_pVT->GetLineBeginPlace(m_wpCaret));

        ScrollToCaret();
        SetCaretOrigin();
        SetCaretInfo();
      }
    }
  }
}

void CFX_Edit::OnVK_END(FX_BOOL bShift, FX_BOOL bCtrl) {
  if (m_pVT->IsValid()) {
    if (bShift) {
      if (bCtrl)
        SetCaret(m_pVT->GetEndWordPlace());
      else
        SetCaret(m_pVT->GetLineEndPlace(m_wpCaret));

      if (m_SelState.IsExist())
        m_SelState.SetEndPos(m_wpCaret);
      else
        m_SelState.Set(m_wpOldCaret, m_wpCaret);

      ScrollToCaret();
      CPVT_WordRange wr(m_wpOldCaret, m_wpCaret);
      Refresh(RP_OPTIONAL, &wr);
      SetCaretInfo();
    } else {
      if (m_SelState.IsExist()) {
        if (m_SelState.BeginPos.WordCmp(m_SelState.EndPos) > 0)
          SetCaret(m_SelState.BeginPos);
        else
          SetCaret(m_SelState.EndPos);

        SelectNone();
        ScrollToCaret();
        SetCaretInfo();
      } else {
        if (bCtrl)
          SetCaret(m_pVT->GetEndWordPlace());
        else
          SetCaret(m_pVT->GetLineEndPlace(m_wpCaret));

        ScrollToCaret();
        SetCaretOrigin();
        SetCaretInfo();
      }
    }
  }
}

void CFX_Edit::SetText(const FX_WCHAR* text,
                       int32_t charset,
                       const CPVT_SecProps* pSecProps,
                       const CPVT_WordProps* pWordProps,
                       FX_BOOL bAddUndo,
                       FX_BOOL bPaint) {
  Empty();
  DoInsertText(CPVT_WordPlace(0, 0, -1), text, charset, pSecProps, pWordProps);
  if (bPaint)
    Paint();
}

FX_BOOL CFX_Edit::InsertWord(uint16_t word,
                             int32_t charset,
                             const CPVT_WordProps* pWordProps,
                             FX_BOOL bAddUndo,
                             FX_BOOL bPaint) {
  if (IsTextOverflow())
    return FALSE;

  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);

    SetCaret(m_pVT->InsertWord(
        m_wpCaret, word, GetCharSetFromUnicode(word, charset), pWordProps));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    if (m_wpCaret != m_wpOldCaret) {
      if (bAddUndo && m_bEnableUndo) {
        AddEditUndoItem(new CFXEU_InsertWord(this, m_wpOldCaret, m_wpCaret,
                                             word, charset, pWordProps));
      }

      if (bPaint)
        PaintInsertText(m_wpOldCaret, m_wpCaret);

      if (m_bOprNotify && m_pOprNotify)
        m_pOprNotify->OnInsertWord(m_wpCaret, m_wpOldCaret);

      return TRUE;
    }
  }

  return FALSE;
}

FX_BOOL CFX_Edit::InsertReturn(const CPVT_SecProps* pSecProps,
                               const CPVT_WordProps* pWordProps,
                               FX_BOOL bAddUndo,
                               FX_BOOL bPaint) {
  if (IsTextOverflow())
    return FALSE;

  if (m_pVT->IsValid()) {
    m_pVT->UpdateWordPlace(m_wpCaret);
    SetCaret(m_pVT->InsertSection(m_wpCaret, pSecProps, pWordProps));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    if (m_wpCaret != m_wpOldCaret) {
      if (bAddUndo && m_bEnableUndo) {
        AddEditUndoItem(new CFXEU_InsertReturn(this, m_wpOldCaret, m_wpCaret,
                                               pSecProps, pWordProps));
      }

      if (bPaint) {
        RearrangePart(CPVT_WordRange(m_wpOldCaret, m_wpCaret));
        ScrollToCaret();
        CPVT_WordRange wr(m_wpOldCaret, GetVisibleWordRange().EndPos);
        Refresh(RP_ANALYSE, &wr);
        SetCaretOrigin();
        SetCaretInfo();
      }

      if (m_bOprNotify && m_pOprNotify)
        m_pOprNotify->OnInsertReturn(m_wpCaret, m_wpOldCaret);

      return TRUE;
    }
  }

  return FALSE;
}

FX_BOOL CFX_Edit::Backspace(FX_BOOL bAddUndo, FX_BOOL bPaint) {
  if (m_pVT->IsValid()) {
    if (m_wpCaret == m_pVT->GetBeginWordPlace())
      return FALSE;

    CPVT_Section section;
    CPVT_Word word;

    if (bAddUndo) {
      CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
      pIterator->SetAt(m_wpCaret);
      pIterator->GetSection(section);
      pIterator->GetWord(word);
    }

    m_pVT->UpdateWordPlace(m_wpCaret);
    SetCaret(m_pVT->BackSpaceWord(m_wpCaret));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    if (m_wpCaret != m_wpOldCaret) {
      if (bAddUndo && m_bEnableUndo) {
        if (m_wpCaret.SecCmp(m_wpOldCaret) != 0)
          AddEditUndoItem(new CFXEU_Backspace(
              this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset,
              section.SecProps, section.WordProps));
        else
          AddEditUndoItem(new CFXEU_Backspace(
              this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset,
              section.SecProps, word.WordProps));
      }

      if (bPaint) {
        RearrangePart(CPVT_WordRange(m_wpCaret, m_wpOldCaret));
        ScrollToCaret();

        CPVT_WordRange wr;
        if (m_wpCaret.SecCmp(m_wpOldCaret) != 0)
          wr = CPVT_WordRange(m_pVT->GetPrevWordPlace(m_wpCaret),
                              GetVisibleWordRange().EndPos);
        else if (m_wpCaret.LineCmp(m_wpOldCaret) != 0)
          wr = CPVT_WordRange(m_pVT->GetLineBeginPlace(m_wpCaret),
                              m_pVT->GetSectionEndPlace(m_wpCaret));
        else
          wr = CPVT_WordRange(m_pVT->GetPrevWordPlace(m_wpCaret),
                              m_pVT->GetSectionEndPlace(m_wpCaret));

        Refresh(RP_ANALYSE, &wr);

        SetCaretOrigin();
        SetCaretInfo();
      }

      if (m_bOprNotify && m_pOprNotify)
        m_pOprNotify->OnBackSpace(m_wpCaret, m_wpOldCaret);

      return TRUE;
    }
  }

  return FALSE;
}

FX_BOOL CFX_Edit::Delete(FX_BOOL bAddUndo, FX_BOOL bPaint) {
  if (m_pVT->IsValid()) {
    if (m_wpCaret == m_pVT->GetEndWordPlace())
      return FALSE;

    CPVT_Section section;
    CPVT_Word word;

    if (bAddUndo) {
      CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
      pIterator->SetAt(m_pVT->GetNextWordPlace(m_wpCaret));
      pIterator->GetSection(section);
      pIterator->GetWord(word);
    }

    m_pVT->UpdateWordPlace(m_wpCaret);
    FX_BOOL bSecEnd = (m_wpCaret == m_pVT->GetSectionEndPlace(m_wpCaret));

    SetCaret(m_pVT->DeleteWord(m_wpCaret));
    m_SelState.Set(m_wpCaret, m_wpCaret);

    if (bAddUndo && m_bEnableUndo) {
      if (bSecEnd)
        AddEditUndoItem(new CFXEU_Delete(
            this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset,
            section.SecProps, section.WordProps, bSecEnd));
      else
        AddEditUndoItem(new CFXEU_Delete(
            this, m_wpOldCaret, m_wpCaret, word.Word, word.nCharset,
            section.SecProps, word.WordProps, bSecEnd));
    }

    if (bPaint) {
      RearrangePart(CPVT_WordRange(m_wpOldCaret, m_wpCaret));
      ScrollToCaret();

      CPVT_WordRange wr;
      if (bSecEnd)
        wr = CPVT_WordRange(m_pVT->GetPrevWordPlace(m_wpOldCaret),
                            GetVisibleWordRange().EndPos);
      else if (m_wpCaret.LineCmp(m_wpOldCaret) != 0)
        wr = CPVT_WordRange(m_pVT->GetLineBeginPlace(m_wpCaret),
                            m_pVT->GetSectionEndPlace(m_wpCaret));
      else
        wr = CPVT_WordRange(m_pVT->GetPrevWordPlace(m_wpOldCaret),
                            m_pVT->GetSectionEndPlace(m_wpCaret));

      Refresh(RP_ANALYSE, &wr);

      SetCaretOrigin();
      SetCaretInfo();
    }

    if (m_bOprNotify && m_pOprNotify)
      m_pOprNotify->OnDelete(m_wpCaret, m_wpOldCaret);

    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFX_Edit::Empty() {
  if (m_pVT->IsValid()) {
    m_pVT->DeleteWords(GetWholeWordRange());
    SetCaret(m_pVT->GetBeginWordPlace());

    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFX_Edit::Clear(FX_BOOL bAddUndo, FX_BOOL bPaint) {
  if (!m_pVT->IsValid())
    return FALSE;

  if (!m_SelState.IsExist())
    return FALSE;

  CPVT_WordRange range = m_SelState.ConvertToWordRange();

  if (bAddUndo && m_bEnableUndo)
    AddEditUndoItem(new CFXEU_Clear(this, range, GetSelText()));

  SelectNone();
  SetCaret(m_pVT->DeleteWords(range));
  m_SelState.Set(m_wpCaret, m_wpCaret);

  if (bPaint) {
    RearrangePart(range);
    ScrollToCaret();

    CPVT_WordRange wr(m_wpOldCaret, GetVisibleWordRange().EndPos);
    Refresh(RP_ANALYSE, &wr);

    SetCaretOrigin();
    SetCaretInfo();
  }

  if (m_bOprNotify && m_pOprNotify)
    m_pOprNotify->OnClear(m_wpCaret, m_wpOldCaret);

  return TRUE;
}

FX_BOOL CFX_Edit::InsertText(const FX_WCHAR* text,
                             int32_t charset,
                             const CPVT_SecProps* pSecProps,
                             const CPVT_WordProps* pWordProps,
                             FX_BOOL bAddUndo,
                             FX_BOOL bPaint) {
  if (IsTextOverflow())
    return FALSE;

  m_pVT->UpdateWordPlace(m_wpCaret);
  SetCaret(DoInsertText(m_wpCaret, text, charset, pSecProps, pWordProps));
  m_SelState.Set(m_wpCaret, m_wpCaret);

  if (m_wpCaret != m_wpOldCaret) {
    if (bAddUndo && m_bEnableUndo) {
      AddEditUndoItem(new CFXEU_InsertText(this, m_wpOldCaret, m_wpCaret, text,
                                           charset, pSecProps, pWordProps));
    }

    if (bPaint)
      PaintInsertText(m_wpOldCaret, m_wpCaret);

    if (m_bOprNotify && m_pOprNotify)
      m_pOprNotify->OnInsertText(m_wpCaret, m_wpOldCaret);

    return TRUE;
  }
  return FALSE;
}

void CFX_Edit::PaintInsertText(const CPVT_WordPlace& wpOld,
                               const CPVT_WordPlace& wpNew) {
  if (m_pVT->IsValid()) {
    RearrangePart(CPVT_WordRange(wpOld, wpNew));
    ScrollToCaret();

    CPVT_WordRange wr;
    if (m_wpCaret.LineCmp(wpOld) != 0)
      wr = CPVT_WordRange(m_pVT->GetLineBeginPlace(wpOld),
                          m_pVT->GetSectionEndPlace(wpNew));
    else
      wr = CPVT_WordRange(wpOld, m_pVT->GetSectionEndPlace(wpNew));
    Refresh(RP_ANALYSE, &wr);
    SetCaretOrigin();
    SetCaretInfo();
  }
}

FX_BOOL CFX_Edit::Redo() {
  if (m_bEnableUndo) {
    if (m_Undo.CanRedo()) {
      m_Undo.Redo();
      return TRUE;
    }
  }

  return FALSE;
}

FX_BOOL CFX_Edit::Undo() {
  if (m_bEnableUndo) {
    if (m_Undo.CanUndo()) {
      m_Undo.Undo();
      return TRUE;
    }
  }

  return FALSE;
}

void CFX_Edit::SetCaretOrigin() {
  if (!m_pVT->IsValid())
    return;

  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  pIterator->SetAt(m_wpCaret);
  CPVT_Word word;
  CPVT_Line line;
  if (pIterator->GetWord(word)) {
    m_ptCaret.x = word.ptWord.x + word.fWidth;
    m_ptCaret.y = word.ptWord.y;
  } else if (pIterator->GetLine(line)) {
    m_ptCaret.x = line.ptLine.x;
    m_ptCaret.y = line.ptLine.y;
  }
}

int32_t CFX_Edit::WordPlaceToWordIndex(const CPVT_WordPlace& place) const {
  if (m_pVT->IsValid())
    return m_pVT->WordPlaceToWordIndex(place);

  return -1;
}

CPVT_WordPlace CFX_Edit::WordIndexToWordPlace(int32_t index) const {
  if (m_pVT->IsValid())
    return m_pVT->WordIndexToWordPlace(index);

  return CPVT_WordPlace();
}

FX_BOOL CFX_Edit::IsTextFull() const {
  int32_t nTotalWords = m_pVT->GetTotalWords();
  int32_t nLimitChar = m_pVT->GetLimitChar();
  int32_t nCharArray = m_pVT->GetCharArray();

  return IsTextOverflow() || (nLimitChar > 0 && nTotalWords >= nLimitChar) ||
         (nCharArray > 0 && nTotalWords >= nCharArray);
}

FX_BOOL CFX_Edit::IsTextOverflow() const {
  if (!m_bEnableScroll && !m_bEnableOverflow) {
    CFX_FloatRect rcPlate = m_pVT->GetPlateRect();
    CFX_FloatRect rcContent = m_pVT->GetContentRect();

    if (m_pVT->IsMultiLine() && GetTotalLines() > 1) {
      if (FX_EDIT_IsFloatBigger(rcContent.Height(), rcPlate.Height()))
        return TRUE;
    }

    if (FX_EDIT_IsFloatBigger(rcContent.Width(), rcPlate.Width()))
      return TRUE;
  }

  return FALSE;
}

CPVT_WordPlace CFX_Edit::GetLineBeginPlace(const CPVT_WordPlace& place) const {
  return m_pVT->GetLineBeginPlace(place);
}

CPVT_WordPlace CFX_Edit::GetLineEndPlace(const CPVT_WordPlace& place) const {
  return m_pVT->GetLineEndPlace(place);
}

CPVT_WordPlace CFX_Edit::GetSectionBeginPlace(
    const CPVT_WordPlace& place) const {
  return m_pVT->GetSectionBeginPlace(place);
}

CPVT_WordPlace CFX_Edit::GetSectionEndPlace(const CPVT_WordPlace& place) const {
  return m_pVT->GetSectionEndPlace(place);
}

FX_BOOL CFX_Edit::CanUndo() const {
  if (m_bEnableUndo) {
    return m_Undo.CanUndo();
  }

  return FALSE;
}

FX_BOOL CFX_Edit::CanRedo() const {
  if (m_bEnableUndo) {
    return m_Undo.CanRedo();
  }

  return FALSE;
}

FX_BOOL CFX_Edit::IsModified() const {
  if (m_bEnableUndo) {
    return m_Undo.IsModified();
  }

  return FALSE;
}

void CFX_Edit::EnableRefresh(FX_BOOL bRefresh) {
  m_bEnableRefresh = bRefresh;
}

void CFX_Edit::EnableUndo(FX_BOOL bUndo) {
  m_bEnableUndo = bUndo;
}

void CFX_Edit::EnableOprNotify(FX_BOOL bNotify) {
  m_bOprNotify = bNotify;
}

FX_FLOAT CFX_Edit::GetLineTop(const CPVT_WordPlace& place) const {
  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordPlace wpOld = pIterator->GetAt();

  pIterator->SetAt(place);
  CPVT_Line line;
  pIterator->GetLine(line);

  pIterator->SetAt(wpOld);

  return line.ptLine.y + line.fLineAscent;
}

FX_FLOAT CFX_Edit::GetLineBottom(const CPVT_WordPlace& place) const {
  CPDF_VariableText::Iterator* pIterator = m_pVT->GetIterator();
  CPVT_WordPlace wpOld = pIterator->GetAt();

  pIterator->SetAt(place);
  CPVT_Line line;
  pIterator->GetLine(line);

  pIterator->SetAt(wpOld);

  return line.ptLine.y + line.fLineDescent;
}

CPVT_WordPlace CFX_Edit::DoInsertText(const CPVT_WordPlace& place,
                                      const FX_WCHAR* text,
                                      int32_t charset,
                                      const CPVT_SecProps* pSecProps,
                                      const CPVT_WordProps* pWordProps) {
  CPVT_WordPlace wp = place;

  if (m_pVT->IsValid()) {
    CFX_WideString sText = text;

    for (int32_t i = 0, sz = sText.GetLength(); i < sz; i++) {
      uint16_t word = sText[i];
      switch (word) {
        case 0x0D:
          wp = m_pVT->InsertSection(wp, pSecProps, pWordProps);
          if (sText[i + 1] == 0x0A)
            i++;
          break;
        case 0x0A:
          wp = m_pVT->InsertSection(wp, pSecProps, pWordProps);
          if (sText[i + 1] == 0x0D)
            i++;
          break;
        case 0x09:
          word = 0x20;
        default:
          wp = m_pVT->InsertWord(wp, word, GetCharSetFromUnicode(word, charset),
                                 pWordProps);
          break;
      }
    }
  }

  return wp;
}

int32_t CFX_Edit::GetCharSetFromUnicode(uint16_t word, int32_t nOldCharset) {
  if (IPVT_FontMap* pFontMap = GetFontMap())
    return pFontMap->CharSetFromUnicode(word, nOldCharset);
  return nOldCharset;
}

void CFX_Edit::BeginGroupUndo(const CFX_WideString& sTitle) {
  ASSERT(!m_pGroupUndoItem);

  m_pGroupUndoItem = new CFX_Edit_GroupUndoItem(sTitle);
}

void CFX_Edit::EndGroupUndo() {
  m_pGroupUndoItem->UpdateItems();
  m_Undo.AddItem(m_pGroupUndoItem);
  m_pGroupUndoItem = nullptr;
}

void CFX_Edit::AddEditUndoItem(CFX_Edit_UndoItem* pEditUndoItem) {
  if (m_pGroupUndoItem) {
    m_pGroupUndoItem->AddUndoItem(pEditUndoItem);
  } else {
    m_Undo.AddItem(pEditUndoItem);
  }
}

void CFX_Edit::AddUndoItem(IFX_Edit_UndoItem* pUndoItem) {
  m_Undo.AddItem(pUndoItem);
}

CFX_Edit_LineRectArray::CFX_Edit_LineRectArray() {}

CFX_Edit_LineRectArray::~CFX_Edit_LineRectArray() {
  Empty();
}

void CFX_Edit_LineRectArray::Empty() {
  for (int32_t i = 0, sz = m_LineRects.GetSize(); i < sz; i++)
    delete m_LineRects.GetAt(i);

  m_LineRects.RemoveAll();
}

void CFX_Edit_LineRectArray::RemoveAll() {
  m_LineRects.RemoveAll();
}

void CFX_Edit_LineRectArray::operator=(CFX_Edit_LineRectArray& rects) {
  Empty();
  for (int32_t i = 0, sz = rects.GetSize(); i < sz; i++)
    m_LineRects.Add(rects.GetAt(i));

  rects.RemoveAll();
}

void CFX_Edit_LineRectArray::Add(const CPVT_WordRange& wrLine,
                                 const CFX_FloatRect& rcLine) {
  m_LineRects.Add(new CFX_Edit_LineRect(wrLine, rcLine));
}

int32_t CFX_Edit_LineRectArray::GetSize() const {
  return m_LineRects.GetSize();
}

CFX_Edit_LineRect* CFX_Edit_LineRectArray::GetAt(int32_t nIndex) const {
  if (nIndex < 0 || nIndex >= m_LineRects.GetSize())
    return nullptr;

  return m_LineRects.GetAt(nIndex);
}

CFX_Edit_Select::CFX_Edit_Select() {}

CFX_Edit_Select::CFX_Edit_Select(const CPVT_WordPlace& begin,
                                 const CPVT_WordPlace& end) {
  Set(begin, end);
}

CFX_Edit_Select::CFX_Edit_Select(const CPVT_WordRange& range) {
  Set(range.BeginPos, range.EndPos);
}

CPVT_WordRange CFX_Edit_Select::ConvertToWordRange() const {
  return CPVT_WordRange(BeginPos, EndPos);
}

void CFX_Edit_Select::Default() {
  BeginPos.Default();
  EndPos.Default();
}

void CFX_Edit_Select::Set(const CPVT_WordPlace& begin,
                          const CPVT_WordPlace& end) {
  BeginPos = begin;
  EndPos = end;
}

void CFX_Edit_Select::SetBeginPos(const CPVT_WordPlace& begin) {
  BeginPos = begin;
}

void CFX_Edit_Select::SetEndPos(const CPVT_WordPlace& end) {
  EndPos = end;
}

FX_BOOL CFX_Edit_Select::IsExist() const {
  return BeginPos != EndPos;
}

FX_BOOL CFX_Edit_Select::operator!=(const CPVT_WordRange& wr) const {
  return wr.BeginPos != BeginPos || wr.EndPos != EndPos;
}

CFX_Edit_RectArray::CFX_Edit_RectArray() {}

CFX_Edit_RectArray::~CFX_Edit_RectArray() {
  Empty();
}

void CFX_Edit_RectArray::Empty() {
  for (int32_t i = 0, sz = m_Rects.GetSize(); i < sz; i++)
    delete m_Rects.GetAt(i);

  m_Rects.RemoveAll();
}

void CFX_Edit_RectArray::Add(const CFX_FloatRect& rect) {
  // check for overlapped area
  for (int32_t i = 0, sz = m_Rects.GetSize(); i < sz; i++) {
    CFX_FloatRect* pRect = m_Rects.GetAt(i);
    if (pRect && pRect->Contains(rect))
      return;
  }

  m_Rects.Add(new CFX_FloatRect(rect));
}

int32_t CFX_Edit_RectArray::GetSize() const {
  return m_Rects.GetSize();
}

CFX_FloatRect* CFX_Edit_RectArray::GetAt(int32_t nIndex) const {
  if (nIndex < 0 || nIndex >= m_Rects.GetSize())
    return nullptr;

  return m_Rects.GetAt(nIndex);
}
