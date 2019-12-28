// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_rendercontext.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fpdfdoc/cpvt_generateap.h"
#include "core/fpdfdoc/include/fpdf_doc.h"
#include "core/fxge/include/fx_ge.h"

CPDF_AnnotList::CPDF_AnnotList(CPDF_Page* pPage)
    : m_pDocument(pPage->m_pDocument) {
  if (!pPage->m_pFormDict)
    return;

  CPDF_Array* pAnnots = pPage->m_pFormDict->GetArrayBy("Annots");
  if (!pAnnots)
    return;

  CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
  CPDF_Dictionary* pAcroForm = pRoot->GetDictBy("AcroForm");
  FX_BOOL bRegenerateAP =
      pAcroForm && pAcroForm->GetBooleanBy("NeedAppearances");
  for (size_t i = 0; i < pAnnots->GetCount(); ++i) {
    CPDF_Dictionary* pDict = ToDictionary(pAnnots->GetDirectObjectAt(i));
    if (!pDict)
      continue;

    uint32_t dwObjNum = pDict->GetObjNum();
    if (dwObjNum == 0) {
      dwObjNum = m_pDocument->AddIndirectObject(pDict);
      CPDF_Reference* pAction = new CPDF_Reference(m_pDocument, dwObjNum);
      pAnnots->InsertAt(i, pAction);
      pAnnots->RemoveAt(i + 1);
      pDict = pAnnots->GetDictAt(i);
    }
    m_AnnotList.push_back(
        std::unique_ptr<CPDF_Annot>(new CPDF_Annot(pDict, this)));
    if (bRegenerateAP && pDict->GetStringBy("Subtype") == "Widget" &&
        CPDF_InterForm::IsUpdateAPEnabled()) {
      FPDF_GenerateAP(m_pDocument, pDict);
    }
  }
}

CPDF_AnnotList::~CPDF_AnnotList() {}

void CPDF_AnnotList::DisplayPass(CPDF_Page* pPage,
                                 CFX_RenderDevice* pDevice,
                                 CPDF_RenderContext* pContext,
                                 FX_BOOL bPrinting,
                                 CFX_Matrix* pMatrix,
                                 FX_BOOL bWidgetPass,
                                 CPDF_RenderOptions* pOptions,
                                 FX_RECT* clip_rect) {
  for (const auto& pAnnot : m_AnnotList) {
    bool bWidget = pAnnot->GetSubType() == "Widget";
    if ((bWidgetPass && !bWidget) || (!bWidgetPass && bWidget))
      continue;

    uint32_t annot_flags = pAnnot->GetFlags();
    if (annot_flags & ANNOTFLAG_HIDDEN)
      continue;

    if (bPrinting && (annot_flags & ANNOTFLAG_PRINT) == 0)
      continue;

    if (!bPrinting && (annot_flags & ANNOTFLAG_NOVIEW))
      continue;

    if (pOptions) {
      CPDF_OCContext* pOCContext = pOptions->m_pOCContext;
      CPDF_Dictionary* pAnnotDict = pAnnot->GetAnnotDict();
      if (pOCContext && pAnnotDict &&
          !pOCContext->CheckOCGVisible(pAnnotDict->GetDictBy("OC"))) {
        continue;
      }
    }
    CFX_FloatRect annot_rect_f;
    pAnnot->GetRect(annot_rect_f);
    CFX_Matrix matrix = *pMatrix;
    if (clip_rect) {
      annot_rect_f.Transform(&matrix);
      FX_RECT annot_rect = annot_rect_f.GetOutterRect();
      annot_rect.Intersect(*clip_rect);
      if (annot_rect.IsEmpty()) {
        continue;
      }
    }
    if (pContext) {
      pAnnot->DrawInContext(pPage, pContext, &matrix, CPDF_Annot::Normal);
    } else if (!pAnnot->DrawAppearance(pPage, pDevice, &matrix,
                                       CPDF_Annot::Normal, pOptions)) {
      pAnnot->DrawBorder(pDevice, &matrix, pOptions);
    }
  }
}

void CPDF_AnnotList::DisplayAnnots(CPDF_Page* pPage,
                                   CFX_RenderDevice* pDevice,
                                   CPDF_RenderContext* pContext,
                                   FX_BOOL bPrinting,
                                   CFX_Matrix* pUser2Device,
                                   uint32_t dwAnnotFlags,
                                   CPDF_RenderOptions* pOptions,
                                   FX_RECT* pClipRect) {
  if (dwAnnotFlags & 0x01) {
    DisplayPass(pPage, pDevice, pContext, bPrinting, pUser2Device, FALSE,
                pOptions, pClipRect);
  }
  if (dwAnnotFlags & 0x02) {
    DisplayPass(pPage, pDevice, pContext, bPrinting, pUser2Device, TRUE,
                pOptions, pClipRect);
  }
}

CPDF_Annot::CPDF_Annot(CPDF_Dictionary* pDict, CPDF_AnnotList* pList)
    : m_pAnnotDict(pDict),
      m_pList(pList),
      m_sSubtype(m_pAnnotDict->GetStringBy("Subtype")) {}

CPDF_Annot::~CPDF_Annot() {
  ClearCachedAP();
}

void CPDF_Annot::ClearCachedAP() {
  for (const auto& pair : m_APMap) {
    delete pair.second;
  }
  m_APMap.clear();
}
CFX_ByteString CPDF_Annot::GetSubType() const {
  return m_sSubtype;
}

void CPDF_Annot::GetRect(CFX_FloatRect& rect) const {
  if (!m_pAnnotDict) {
    return;
  }
  rect = m_pAnnotDict->GetRectBy("Rect");
  rect.Normalize();
}

uint32_t CPDF_Annot::GetFlags() const {
  return m_pAnnotDict->GetIntegerBy("F");
}

CPDF_Stream* FPDFDOC_GetAnnotAP(CPDF_Dictionary* pAnnotDict,
                                CPDF_Annot::AppearanceMode mode) {
  CPDF_Dictionary* pAP = pAnnotDict->GetDictBy("AP");
  if (!pAP) {
    return nullptr;
  }
  const FX_CHAR* ap_entry = "N";
  if (mode == CPDF_Annot::Down)
    ap_entry = "D";
  else if (mode == CPDF_Annot::Rollover)
    ap_entry = "R";
  if (!pAP->KeyExist(ap_entry))
    ap_entry = "N";

  CPDF_Object* psub = pAP->GetDirectObjectBy(ap_entry);
  if (!psub)
    return nullptr;
  if (CPDF_Stream* pStream = psub->AsStream())
    return pStream;

  if (CPDF_Dictionary* pDict = psub->AsDictionary()) {
    CFX_ByteString as = pAnnotDict->GetStringBy("AS");
    if (as.IsEmpty()) {
      CFX_ByteString value = pAnnotDict->GetStringBy("V");
      if (value.IsEmpty()) {
        CPDF_Dictionary* pParentDict = pAnnotDict->GetDictBy("Parent");
        value = pParentDict ? pParentDict->GetStringBy("V") : CFX_ByteString();
      }
      if (value.IsEmpty() || !pDict->KeyExist(value))
        as = "Off";
      else
        as = value;
    }
    return pDict->GetStreamBy(as);
  }
  return nullptr;
}

CPDF_Form* CPDF_Annot::GetAPForm(const CPDF_Page* pPage, AppearanceMode mode) {
  CPDF_Stream* pStream = FPDFDOC_GetAnnotAP(m_pAnnotDict, mode);
  if (!pStream)
    return nullptr;

  auto it = m_APMap.find(pStream);
  if (it != m_APMap.end())
    return it->second;

  CPDF_Form* pNewForm =
      new CPDF_Form(m_pList->GetDocument(), pPage->m_pResources, pStream);
  pNewForm->ParseContent(nullptr, nullptr, nullptr);
  m_APMap[pStream] = pNewForm;
  return pNewForm;
}

static CPDF_Form* FPDFDOC_Annot_GetMatrix(const CPDF_Page* pPage,
                                          CPDF_Annot* pAnnot,
                                          CPDF_Annot::AppearanceMode mode,
                                          const CFX_Matrix* pUser2Device,
                                          CFX_Matrix& matrix) {
  CPDF_Form* pForm = pAnnot->GetAPForm(pPage, mode);
  if (!pForm) {
    return nullptr;
  }
  CFX_FloatRect form_bbox = pForm->m_pFormDict->GetRectBy("BBox");
  CFX_Matrix form_matrix = pForm->m_pFormDict->GetMatrixBy("Matrix");
  form_matrix.TransformRect(form_bbox);
  CFX_FloatRect arect;
  pAnnot->GetRect(arect);
  matrix.MatchRect(arect, form_bbox);
  matrix.Concat(*pUser2Device);
  return pForm;
}
FX_BOOL CPDF_Annot::DrawAppearance(CPDF_Page* pPage,
                                   CFX_RenderDevice* pDevice,
                                   const CFX_Matrix* pUser2Device,
                                   AppearanceMode mode,
                                   const CPDF_RenderOptions* pOptions) {
  CFX_Matrix matrix;
  CPDF_Form* pForm =
      FPDFDOC_Annot_GetMatrix(pPage, this, mode, pUser2Device, matrix);
  if (!pForm) {
    return FALSE;
  }
  CPDF_RenderContext context(pPage);
  context.AppendLayer(pForm, &matrix);
  context.Render(pDevice, pOptions, nullptr);
  return TRUE;
}
FX_BOOL CPDF_Annot::DrawInContext(const CPDF_Page* pPage,
                                  CPDF_RenderContext* pContext,
                                  const CFX_Matrix* pUser2Device,
                                  AppearanceMode mode) {
  CFX_Matrix matrix;
  CPDF_Form* pForm =
      FPDFDOC_Annot_GetMatrix(pPage, this, mode, pUser2Device, matrix);
  if (!pForm) {
    return FALSE;
  }
  pContext->AppendLayer(pForm, &matrix);
  return TRUE;
}
void CPDF_Annot::DrawBorder(CFX_RenderDevice* pDevice,
                            const CFX_Matrix* pUser2Device,
                            const CPDF_RenderOptions* pOptions) {
  if (GetSubType() == "Popup") {
    return;
  }
  uint32_t annot_flags = GetFlags();
  if (annot_flags & ANNOTFLAG_HIDDEN) {
    return;
  }
  bool bPrinting = pDevice->GetDeviceClass() == FXDC_PRINTER ||
                   (pOptions && (pOptions->m_Flags & RENDER_PRINTPREVIEW));
  if (bPrinting && (annot_flags & ANNOTFLAG_PRINT) == 0) {
    return;
  }
  if (!bPrinting && (annot_flags & ANNOTFLAG_NOVIEW)) {
    return;
  }
  CPDF_Dictionary* pBS = m_pAnnotDict->GetDictBy("BS");
  char style_char;
  FX_FLOAT width;
  CPDF_Array* pDashArray = nullptr;
  if (!pBS) {
    CPDF_Array* pBorderArray = m_pAnnotDict->GetArrayBy("Border");
    style_char = 'S';
    if (pBorderArray) {
      width = pBorderArray->GetNumberAt(2);
      if (pBorderArray->GetCount() == 4) {
        pDashArray = pBorderArray->GetArrayAt(3);
        if (!pDashArray) {
          return;
        }
        size_t nLen = pDashArray->GetCount();
        size_t i = 0;
        for (; i < nLen; ++i) {
          CPDF_Object* pObj = pDashArray->GetDirectObjectAt(i);
          if (pObj && pObj->GetInteger()) {
            break;
          }
        }
        if (i == nLen) {
          return;
        }
        style_char = 'D';
      }
    } else {
      width = 1;
    }
  } else {
    CFX_ByteString style = pBS->GetStringBy("S");
    pDashArray = pBS->GetArrayBy("D");
    style_char = style[1];
    width = pBS->GetNumberBy("W");
  }
  if (width <= 0) {
    return;
  }
  CPDF_Array* pColor = m_pAnnotDict->GetArrayBy("C");
  uint32_t argb = 0xff000000;
  if (pColor) {
    int R = (int32_t)(pColor->GetNumberAt(0) * 255);
    int G = (int32_t)(pColor->GetNumberAt(1) * 255);
    int B = (int32_t)(pColor->GetNumberAt(2) * 255);
    argb = ArgbEncode(0xff, R, G, B);
  }
  CFX_GraphStateData graph_state;
  graph_state.m_LineWidth = width;
  if (style_char == 'D') {
    if (pDashArray) {
      size_t dash_count = pDashArray->GetCount();
      if (dash_count % 2) {
        dash_count++;
      }
      graph_state.m_DashArray = FX_Alloc(FX_FLOAT, dash_count);
      graph_state.m_DashCount = dash_count;
      size_t i;
      for (i = 0; i < pDashArray->GetCount(); ++i) {
        graph_state.m_DashArray[i] = pDashArray->GetNumberAt(i);
      }
      if (i < dash_count) {
        graph_state.m_DashArray[i] = graph_state.m_DashArray[i - 1];
      }
    } else {
      graph_state.m_DashArray = FX_Alloc(FX_FLOAT, 2);
      graph_state.m_DashCount = 2;
      graph_state.m_DashArray[0] = graph_state.m_DashArray[1] = 3 * 1.0f;
    }
  }
  CFX_FloatRect rect;
  GetRect(rect);
  CFX_PathData path;
  width /= 2;
  path.AppendRect(rect.left + width, rect.bottom + width, rect.right - width,
                  rect.top - width);
  int fill_type = 0;
  if (pOptions && (pOptions->m_Flags & RENDER_NOPATHSMOOTH)) {
    fill_type |= FXFILL_NOPATHSMOOTH;
  }
  pDevice->DrawPath(&path, pUser2Device, &graph_state, argb, argb, fill_type);
}
