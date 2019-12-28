// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/formfiller/cffl_formfiller.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "fpdfsdk/formfiller/cba_fontmap.h"
#include "fpdfsdk/include/fsdk_common.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/pdfwindow/PWL_Utils.h"

#define GetRed(rgb) ((uint8_t)(rgb))
#define GetGreen(rgb) ((uint8_t)(((uint16_t)(rgb)) >> 8))
#define GetBlue(rgb) ((uint8_t)((rgb) >> 16))

#define FFL_HINT_ELAPSE 800

CFFL_FormFiller::CFFL_FormFiller(CPDFDoc_Environment* pApp,
                                 CPDFSDK_Annot* pAnnot)
    : m_pApp(pApp), m_pAnnot(pAnnot), m_bValid(FALSE), m_ptOldPos(0, 0) {
  m_pWidget = (CPDFSDK_Widget*)pAnnot;
}

CFFL_FormFiller::~CFFL_FormFiller() {
  for (const auto& it : m_Maps) {
    CPWL_Wnd* pWnd = it.second;
    CFFL_PrivateData* pData = (CFFL_PrivateData*)pWnd->GetAttachedData();
    pWnd->InvalidateProvider(this);
    pWnd->Destroy();
    delete pWnd;
    delete pData;
  }
  m_Maps.clear();
}

void CFFL_FormFiller::SetWindowRect(CPDFSDK_PageView* pPageView,
                                    const CFX_FloatRect& rcWindow) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    pWnd->Move(CFX_FloatRect(rcWindow), TRUE, FALSE);
  }
}

CFX_FloatRect CFFL_FormFiller::GetWindowRect(CPDFSDK_PageView* pPageView) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    return pWnd->GetWindowRect();
  }

  return CFX_FloatRect(0, 0, 0, 0);
}

FX_RECT CFFL_FormFiller::GetViewBBox(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot) {
  ASSERT(pPageView);
  ASSERT(pAnnot);

  CFX_FloatRect rcAnnot = m_pWidget->GetRect();

  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    CFX_FloatRect rcWindow = pWnd->GetWindowRect();
    rcAnnot = PWLtoFFL(rcWindow);
  }

  CFX_FloatRect rcWin = rcAnnot;

  CFX_FloatRect rcFocus = GetFocusBox(pPageView);
  if (!rcFocus.IsEmpty())
    rcWin.Union(rcFocus);

  CFX_FloatRect rect = CPWL_Utils::InflateRect(rcWin, 1);

  return rect.GetOutterRect();
}

void CFFL_FormFiller::OnDraw(CPDFSDK_PageView* pPageView,
                             CPDFSDK_Annot* pAnnot,
                             CFX_RenderDevice* pDevice,
                             CFX_Matrix* pUser2Device,
                             uint32_t dwFlags) {
  ASSERT(pAnnot->GetPDFAnnot()->GetSubType() == "Widget");

  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    CFX_Matrix mt = GetCurMatrix();
    mt.Concat(*pUser2Device);
    pWnd->DrawAppearance(pDevice, &mt);
  } else {
    CPDFSDK_Widget* pWidget = (CPDFSDK_Widget*)pAnnot;
    if (CFFL_IFormFiller::IsVisible(pWidget))
      pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal,
                              nullptr);
  }
}

void CFFL_FormFiller::OnDrawDeactive(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     CFX_RenderDevice* pDevice,
                                     CFX_Matrix* pUser2Device,
                                     uint32_t dwFlags) {
  CPDFSDK_Widget* pWidget = (CPDFSDK_Widget*)pAnnot;
  pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal, nullptr);
}

void CFFL_FormFiller::OnCreate(CPDFSDK_Annot* pAnnot) {}

void CFFL_FormFiller::OnLoad(CPDFSDK_Annot* pAnnot) {}

void CFFL_FormFiller::OnDelete(CPDFSDK_Annot* pAnnot) {}

void CFFL_FormFiller::OnMouseEnter(CPDFSDK_PageView* pPageView,
                                   CPDFSDK_Annot* pAnnot) {}

void CFFL_FormFiller::OnMouseExit(CPDFSDK_PageView* pPageView,
                                  CPDFSDK_Annot* pAnnot) {
  EndTimer();
  ASSERT(m_pWidget);
}

FX_BOOL CFFL_FormFiller::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                       CPDFSDK_Annot* pAnnot,
                                       FX_UINT nFlags,
                                       const CFX_FloatPoint& point) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, TRUE)) {
    m_bValid = TRUE;
    FX_RECT rect = GetViewBBox(pPageView, pAnnot);
    InvalidateRect(rect.left, rect.top, rect.right, rect.bottom);

    if (!rect.Contains((int)point.x, (int)point.y))
      return FALSE;

    return pWnd->OnLButtonDown(WndtoPWL(pPageView, point), nFlags);
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     FX_UINT nFlags,
                                     const CFX_FloatPoint& point) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    FX_RECT rcFFL = GetViewBBox(pPageView, pAnnot);
    InvalidateRect(rcFFL.left, rcFFL.top, rcFFL.right, rcFFL.bottom);
    pWnd->OnLButtonUp(WndtoPWL(pPageView, point), nFlags);
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnLButtonDblClk(CPDFSDK_PageView* pPageView,
                                         CPDFSDK_Annot* pAnnot,
                                         FX_UINT nFlags,
                                         const CFX_FloatPoint& point) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    pWnd->OnLButtonDblClk(WndtoPWL(pPageView, point), nFlags);
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnMouseMove(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     FX_UINT nFlags,
                                     const CFX_FloatPoint& point) {
  if ((m_ptOldPos.x != point.x) || (m_ptOldPos.y != point.y)) {
    m_ptOldPos = point;
  }

  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    pWnd->OnMouseMove(WndtoPWL(pPageView, point), nFlags);
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnMouseWheel(CPDFSDK_PageView* pPageView,
                                      CPDFSDK_Annot* pAnnot,
                                      FX_UINT nFlags,
                                      short zDelta,
                                      const CFX_FloatPoint& point) {
  if (!IsValid())
    return FALSE;

  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, TRUE)) {
    return pWnd->OnMouseWheel(zDelta, WndtoPWL(pPageView, point), nFlags);
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnRButtonDown(CPDFSDK_PageView* pPageView,
                                       CPDFSDK_Annot* pAnnot,
                                       FX_UINT nFlags,
                                       const CFX_FloatPoint& point) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, TRUE)) {
    pWnd->OnRButtonDown(WndtoPWL(pPageView, point), nFlags);
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnRButtonUp(CPDFSDK_PageView* pPageView,
                                     CPDFSDK_Annot* pAnnot,
                                     FX_UINT nFlags,
                                     const CFX_FloatPoint& point) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    pWnd->OnRButtonUp(WndtoPWL(pPageView, point), nFlags);
    return TRUE;
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnKeyDown(CPDFSDK_Annot* pAnnot,
                                   FX_UINT nKeyCode,
                                   FX_UINT nFlags) {
  if (IsValid()) {
    CPDFSDK_PageView* pPageView = GetCurPageView();
    ASSERT(pPageView);

    if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
      return pWnd->OnKeyDown(nKeyCode, nFlags);
    }
  }

  return FALSE;
}

FX_BOOL CFFL_FormFiller::OnChar(CPDFSDK_Annot* pAnnot,
                                FX_UINT nChar,
                                FX_UINT nFlags) {
  if (IsValid()) {
    CPDFSDK_PageView* pPageView = GetCurPageView();
    ASSERT(pPageView);

    if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
      return pWnd->OnChar(nChar, nFlags);
    }
  }

  return FALSE;
}

void CFFL_FormFiller::SetFocusForAnnot(CPDFSDK_Annot* pAnnot, FX_UINT nFlag) {
  CPDFSDK_Widget* pWidget = (CPDFSDK_Widget*)pAnnot;
  UnderlyingPageType* pPage = pWidget->GetUnderlyingPage();
  CPDFSDK_Document* pDoc = m_pApp->GetSDKDocument();
  CPDFSDK_PageView* pPageView = pDoc->GetPageView(pPage);
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, TRUE))
    pWnd->SetFocus();

  m_bValid = TRUE;
  FX_RECT rcRect = GetViewBBox(pPageView, pAnnot);
  InvalidateRect(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);
}

void CFFL_FormFiller::KillFocusForAnnot(CPDFSDK_Annot* pAnnot, FX_UINT nFlag) {
  if (!IsValid())
    return;

  CPDFSDK_PageView* pPageView = GetCurPageView();
  if (!pPageView)
    return;

  CommitData(pPageView, nFlag);

  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE))
    pWnd->KillFocus();

  FX_BOOL bDestroyPDFWindow;
  switch (m_pWidget->GetFieldType()) {
    case FIELDTYPE_PUSHBUTTON:
    case FIELDTYPE_CHECKBOX:
    case FIELDTYPE_RADIOBUTTON:
      bDestroyPDFWindow = TRUE;
      break;
    default:
      bDestroyPDFWindow = FALSE;
      break;
  }
  EscapeFiller(pPageView, bDestroyPDFWindow);
}

FX_BOOL CFFL_FormFiller::IsValid() const {
  return m_bValid;
}

PWL_CREATEPARAM CFFL_FormFiller::GetCreateParam() {
  ASSERT(m_pApp);

  PWL_CREATEPARAM cp;
  cp.pParentWnd = nullptr;
  cp.pProvider = this;
  cp.rcRectWnd = GetPDFWindowRect();

  uint32_t dwCreateFlags = PWS_BORDER | PWS_BACKGROUND | PWS_VISIBLE;
  uint32_t dwFieldFlag = m_pWidget->GetFieldFlags();
  if (dwFieldFlag & FIELDFLAG_READONLY) {
    dwCreateFlags |= PWS_READONLY;
  }

  FX_COLORREF color;
  if (m_pWidget->GetFillColor(color)) {
    cp.sBackgroundColor =
        CPWL_Color(GetRed(color), GetGreen(color), GetBlue(color));
  }

  if (m_pWidget->GetBorderColor(color)) {
    cp.sBorderColor =
        CPWL_Color(GetRed(color), GetGreen(color), GetBlue(color));
  }

  cp.sTextColor = CPWL_Color(COLORTYPE_GRAY, 0);

  if (m_pWidget->GetTextColor(color)) {
    cp.sTextColor = CPWL_Color(GetRed(color), GetGreen(color), GetBlue(color));
  }

  cp.fFontSize = m_pWidget->GetFontSize();
  cp.dwBorderWidth = m_pWidget->GetBorderWidth();

  cp.nBorderStyle = m_pWidget->GetBorderStyle();
  switch (cp.nBorderStyle) {
    case BorderStyle::DASH:
      cp.sDash = CPWL_Dash(3, 3, 0);
      break;
    case BorderStyle::BEVELED:
      cp.dwBorderWidth *= 2;
      break;
    case BorderStyle::INSET:
      cp.dwBorderWidth *= 2;
      break;
    default:
      break;
  }

  if (cp.fFontSize <= 0)
    dwCreateFlags |= PWS_AUTOFONTSIZE;

  cp.dwFlags = dwCreateFlags;
  cp.pSystemHandler = m_pApp->GetSysHandler();
  return cp;
}

CPWL_Wnd* CFFL_FormFiller::GetPDFWindow(CPDFSDK_PageView* pPageView,
                                        FX_BOOL bNew) {
  ASSERT(pPageView);

  auto it = m_Maps.find(pPageView);
  const bool found = it != m_Maps.end();
  CPWL_Wnd* pWnd = found ? it->second : nullptr;
  if (!bNew)
    return pWnd;

  if (found) {
    CFFL_PrivateData* pPrivateData = (CFFL_PrivateData*)pWnd->GetAttachedData();
    if (pPrivateData->nWidgetAge != m_pWidget->GetAppearanceAge()) {
      return ResetPDFWindow(
          pPageView, m_pWidget->GetValueAge() == pPrivateData->nValueAge);
    }
  } else {
    PWL_CREATEPARAM cp = GetCreateParam();
    cp.hAttachedWnd = (FX_HWND)m_pWidget;

    CFFL_PrivateData* pPrivateData = new CFFL_PrivateData;
    pPrivateData->pWidget = m_pWidget;
    pPrivateData->pPageView = pPageView;
    pPrivateData->nWidgetAge = m_pWidget->GetAppearanceAge();
    pPrivateData->nValueAge = 0;

    cp.pAttachedData = pPrivateData;

    pWnd = NewPDFWindow(cp, pPageView);
    m_Maps[pPageView] = pWnd;
  }

  return pWnd;
}

void CFFL_FormFiller::DestroyPDFWindow(CPDFSDK_PageView* pPageView) {
  auto it = m_Maps.find(pPageView);
  if (it == m_Maps.end())
    return;

  CPWL_Wnd* pWnd = it->second;
  CFFL_PrivateData* pData = (CFFL_PrivateData*)pWnd->GetAttachedData();
  pWnd->Destroy();
  delete pWnd;
  delete pData;

  m_Maps.erase(it);
}

CFX_Matrix CFFL_FormFiller::GetWindowMatrix(void* pAttachedData) {
  if (CFFL_PrivateData* pPrivateData = (CFFL_PrivateData*)pAttachedData) {
    if (pPrivateData->pPageView) {
      CFX_Matrix mtPageView;
      pPrivateData->pPageView->GetCurrentMatrix(mtPageView);
      CFX_Matrix mt = GetCurMatrix();
      mt.Concat(mtPageView);

      return mt;
    }
  }
  return CFX_Matrix(1, 0, 0, 1, 0, 0);
}

CFX_Matrix CFFL_FormFiller::GetCurMatrix() {
  CFX_Matrix mt;

  CFX_FloatRect rcDA;
  m_pWidget->GetPDFAnnot()->GetRect(rcDA);

  switch (m_pWidget->GetRotate()) {
    default:
    case 0:
      mt = CFX_Matrix(1, 0, 0, 1, 0, 0);
      break;
    case 90:
      mt = CFX_Matrix(0, 1, -1, 0, rcDA.right - rcDA.left, 0);
      break;
    case 180:
      mt = CFX_Matrix(-1, 0, 0, -1, rcDA.right - rcDA.left,
                      rcDA.top - rcDA.bottom);
      break;
    case 270:
      mt = CFX_Matrix(0, -1, 1, 0, 0, rcDA.top - rcDA.bottom);
      break;
  }
  mt.e += rcDA.left;
  mt.f += rcDA.bottom;

  return mt;
}

CFX_WideString CFFL_FormFiller::LoadPopupMenuString(int nIndex) {
  ASSERT(m_pApp);

  return L"";
}

CFX_FloatRect CFFL_FormFiller::GetPDFWindowRect() const {
  CFX_FloatRect rectAnnot;
  m_pWidget->GetPDFAnnot()->GetRect(rectAnnot);

  FX_FLOAT fWidth = rectAnnot.right - rectAnnot.left;
  FX_FLOAT fHeight = rectAnnot.top - rectAnnot.bottom;
  if ((m_pWidget->GetRotate() / 90) & 0x01)
    return CFX_FloatRect(0, 0, fHeight, fWidth);

  return CFX_FloatRect(0, 0, fWidth, fHeight);
}

CPDFSDK_PageView* CFFL_FormFiller::GetCurPageView() {
  UnderlyingPageType* pPage = m_pAnnot->GetUnderlyingPage();
  CPDFSDK_Document* pSDKDoc = m_pApp->GetSDKDocument();
  return pSDKDoc ? pSDKDoc->GetPageView(pPage) : nullptr;
}

CFX_FloatRect CFFL_FormFiller::GetFocusBox(CPDFSDK_PageView* pPageView) {
  if (CPWL_Wnd* pWnd = GetPDFWindow(pPageView, FALSE)) {
    CFX_FloatRect rcFocus = FFLtoWnd(pPageView, PWLtoFFL(pWnd->GetFocusRect()));
    CFX_FloatRect rcPage = pPageView->GetPDFPage()->GetPageBBox();
    if (rcPage.Contains(rcFocus))
      return rcFocus;
  }
  return CFX_FloatRect(0, 0, 0, 0);
}

CFX_FloatRect CFFL_FormFiller::FFLtoPWL(const CFX_FloatRect& rect) {
  CFX_Matrix mt;
  mt.SetReverse(GetCurMatrix());

  CFX_FloatRect temp = rect;
  mt.TransformRect(temp);

  return temp;
}

CFX_FloatRect CFFL_FormFiller::PWLtoFFL(const CFX_FloatRect& rect) {
  CFX_Matrix mt = GetCurMatrix();

  CFX_FloatRect temp = rect;
  mt.TransformRect(temp);

  return temp;
}

CFX_FloatPoint CFFL_FormFiller::FFLtoPWL(const CFX_FloatPoint& point) {
  CFX_Matrix mt;
  mt.SetReverse(GetCurMatrix());

  CFX_FloatPoint pt = point;
  mt.Transform(pt.x, pt.y);

  return pt;
}

CFX_FloatPoint CFFL_FormFiller::PWLtoFFL(const CFX_FloatPoint& point) {
  CFX_Matrix mt = GetCurMatrix();

  CFX_FloatPoint pt = point;
  mt.Transform(pt.x, pt.y);

  return pt;
}

CFX_FloatPoint CFFL_FormFiller::WndtoPWL(CPDFSDK_PageView* pPageView,
                                         const CFX_FloatPoint& pt) {
  return FFLtoPWL(pt);
}

CFX_FloatRect CFFL_FormFiller::FFLtoWnd(CPDFSDK_PageView* pPageView,
                                        const CFX_FloatRect& rect) {
  return rect;
}

FX_BOOL CFFL_FormFiller::CommitData(CPDFSDK_PageView* pPageView,
                                    FX_UINT nFlag) {
  if (IsDataChanged(pPageView)) {
    FX_BOOL bRC = TRUE;
    FX_BOOL bExit = FALSE;
    CFFL_IFormFiller* pIFormFiller = m_pApp->GetIFormFiller();
    pIFormFiller->OnKeyStrokeCommit(m_pWidget, pPageView, bRC, bExit, nFlag);
    if (bExit)
      return TRUE;
    if (!bRC) {
      ResetPDFWindow(pPageView, FALSE);
      return TRUE;
    }

    pIFormFiller->OnValidate(m_pWidget, pPageView, bRC, bExit, nFlag);
    if (bExit)
      return TRUE;
    if (!bRC) {
      ResetPDFWindow(pPageView, FALSE);
      return TRUE;
    }

    SaveData(pPageView);
    pIFormFiller->OnCalculate(m_pWidget, pPageView, bExit, nFlag);
    if (bExit)
      return TRUE;

    pIFormFiller->OnFormat(m_pWidget, pPageView, bExit, nFlag);
  }
  return TRUE;
}

FX_BOOL CFFL_FormFiller::IsDataChanged(CPDFSDK_PageView* pPageView) {
  return FALSE;
}

void CFFL_FormFiller::SaveData(CPDFSDK_PageView* pPageView) {}

#ifdef PDF_ENABLE_XFA
FX_BOOL CFFL_FormFiller::IsFieldFull(CPDFSDK_PageView* pPageView) {
  return FALSE;
}
#endif  // PDF_ENABLE_XFA

void CFFL_FormFiller::SetChangeMark() {
  m_pApp->FFI_OnChange();
}

void CFFL_FormFiller::GetActionData(CPDFSDK_PageView* pPageView,
                                    CPDF_AAction::AActionType type,
                                    PDFSDK_FieldAction& fa) {
  fa.sValue = m_pWidget->GetValue();
}

void CFFL_FormFiller::SetActionData(CPDFSDK_PageView* pPageView,
                                    CPDF_AAction::AActionType type,
                                    const PDFSDK_FieldAction& fa) {}

FX_BOOL CFFL_FormFiller::IsActionDataChanged(CPDF_AAction::AActionType type,
                                             const PDFSDK_FieldAction& faOld,
                                             const PDFSDK_FieldAction& faNew) {
  return FALSE;
}

void CFFL_FormFiller::SaveState(CPDFSDK_PageView* pPageView) {}

void CFFL_FormFiller::RestoreState(CPDFSDK_PageView* pPageView) {}

CPWL_Wnd* CFFL_FormFiller::ResetPDFWindow(CPDFSDK_PageView* pPageView,
                                          FX_BOOL bRestoreValue) {
  return GetPDFWindow(pPageView, FALSE);
}

void CFFL_FormFiller::TimerProc() {}

CFX_SystemHandler* CFFL_FormFiller::GetSystemHandler() const {
  return m_pApp->GetSysHandler();
}

void CFFL_FormFiller::EscapeFiller(CPDFSDK_PageView* pPageView,
                                   FX_BOOL bDestroyPDFWindow) {
  m_bValid = FALSE;

  FX_RECT rcRect = GetViewBBox(pPageView, m_pWidget);
  InvalidateRect(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);

  if (bDestroyPDFWindow)
    DestroyPDFWindow(pPageView);
}

void CFFL_FormFiller::InvalidateRect(double left,
                                     double top,
                                     double right,
                                     double bottom) {
  UnderlyingPageType* pPage = m_pWidget->GetUnderlyingPage();
  m_pApp->FFI_Invalidate(pPage, left, top, right, bottom);
}

CFFL_Button::CFFL_Button(CPDFDoc_Environment* pApp, CPDFSDK_Annot* pWidget)
    : CFFL_FormFiller(pApp, pWidget), m_bMouseIn(FALSE), m_bMouseDown(FALSE) {}

CFFL_Button::~CFFL_Button() {}

void CFFL_Button::OnMouseEnter(CPDFSDK_PageView* pPageView,
                               CPDFSDK_Annot* pAnnot) {
  m_bMouseIn = TRUE;
  FX_RECT rect = GetViewBBox(pPageView, pAnnot);
  InvalidateRect(rect.left, rect.top, rect.right, rect.bottom);
}

void CFFL_Button::OnMouseExit(CPDFSDK_PageView* pPageView,
                              CPDFSDK_Annot* pAnnot) {
  m_bMouseIn = FALSE;

  FX_RECT rect = GetViewBBox(pPageView, pAnnot);
  InvalidateRect(rect.left, rect.top, rect.right, rect.bottom);
  EndTimer();
  ASSERT(m_pWidget);
}

FX_BOOL CFFL_Button::OnLButtonDown(CPDFSDK_PageView* pPageView,
                                   CPDFSDK_Annot* pAnnot,
                                   FX_UINT nFlags,
                                   const CFX_FloatPoint& point) {
  CFX_FloatRect rcAnnot = pAnnot->GetRect();
  if (!rcAnnot.Contains(point.x, point.y))
    return FALSE;

  m_bMouseDown = TRUE;
  m_bValid = TRUE;
  FX_RECT rect = GetViewBBox(pPageView, pAnnot);
  InvalidateRect(rect.left, rect.top, rect.right, rect.bottom);
  return TRUE;
}

FX_BOOL CFFL_Button::OnLButtonUp(CPDFSDK_PageView* pPageView,
                                 CPDFSDK_Annot* pAnnot,
                                 FX_UINT nFlags,
                                 const CFX_FloatPoint& point) {
  CFX_FloatRect rcAnnot = pAnnot->GetRect();
  if (!rcAnnot.Contains(point.x, point.y))
    return FALSE;

  m_bMouseDown = FALSE;
  m_pWidget->GetPDFPage();

  FX_RECT rect = GetViewBBox(pPageView, pAnnot);
  InvalidateRect(rect.left, rect.top, rect.right, rect.bottom);
  return TRUE;
}

FX_BOOL CFFL_Button::OnMouseMove(CPDFSDK_PageView* pPageView,
                                 CPDFSDK_Annot* pAnnot,
                                 FX_UINT nFlags,
                                 const CFX_FloatPoint& point) {
  ASSERT(m_pApp);

  return TRUE;
}

void CFFL_Button::OnDraw(CPDFSDK_PageView* pPageView,
                         CPDFSDK_Annot* pAnnot,
                         CFX_RenderDevice* pDevice,
                         CFX_Matrix* pUser2Device,
                         uint32_t dwFlags) {
  ASSERT(pPageView);
  CPDFSDK_Widget* pWidget = (CPDFSDK_Widget*)pAnnot;
  CPDF_FormControl* pCtrl = pWidget->GetFormControl();
  CPDF_FormControl::HighlightingMode eHM = pCtrl->GetHighlightingMode();

  if (eHM != CPDF_FormControl::Push) {
    pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal, nullptr);
    return;
  }

  if (m_bMouseDown) {
    if (pWidget->IsWidgetAppearanceValid(CPDF_Annot::Down))
      pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Down, nullptr);
    else
      pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal,
                              nullptr);
  } else if (m_bMouseIn) {
    if (pWidget->IsWidgetAppearanceValid(CPDF_Annot::Rollover))
      pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Rollover,
                              nullptr);
    else
      pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal,
                              nullptr);
  } else {
    pWidget->DrawAppearance(pDevice, pUser2Device, CPDF_Annot::Normal, nullptr);
  }
}

void CFFL_Button::OnDrawDeactive(CPDFSDK_PageView* pPageView,
                                 CPDFSDK_Annot* pAnnot,
                                 CFX_RenderDevice* pDevice,
                                 CFX_Matrix* pUser2Device,
                                 uint32_t dwFlags) {
  OnDraw(pPageView, pAnnot, pDevice, pUser2Device, dwFlags);
}
