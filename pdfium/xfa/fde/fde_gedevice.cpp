// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/fde_gedevice.h"

#include <algorithm>

#include "xfa/fde/cfde_path.h"
#include "xfa/fde/fde_object.h"
#include "xfa/fgas/font/fgas_font.h"
#include "xfa/fgas/font/fgas_gefont.h"

CFDE_RenderDevice::CFDE_RenderDevice(CFX_RenderDevice* pDevice,
                                     FX_BOOL bOwnerDevice)
    : m_pDevice(pDevice),
      m_bOwnerDevice(bOwnerDevice),
      m_pCharPos(nullptr),
      m_iCharCount(0) {
  ASSERT(pDevice);

  FX_RECT rt = m_pDevice->GetClipBox();
  m_rtClip.Set((FX_FLOAT)rt.left, (FX_FLOAT)rt.top, (FX_FLOAT)rt.Width(),
               (FX_FLOAT)rt.Height());
}

CFDE_RenderDevice::~CFDE_RenderDevice() {
  FX_Free(m_pCharPos);
  if (m_bOwnerDevice)
    delete m_pDevice;
}
int32_t CFDE_RenderDevice::GetWidth() const {
  return m_pDevice->GetWidth();
}
int32_t CFDE_RenderDevice::GetHeight() const {
  return m_pDevice->GetHeight();
}
void CFDE_RenderDevice::SaveState() {
  m_pDevice->SaveState();
}
void CFDE_RenderDevice::RestoreState() {
  m_pDevice->RestoreState(false);
  const FX_RECT& rt = m_pDevice->GetClipBox();
  m_rtClip.Set((FX_FLOAT)rt.left, (FX_FLOAT)rt.top, (FX_FLOAT)rt.Width(),
               (FX_FLOAT)rt.Height());
}
FX_BOOL CFDE_RenderDevice::SetClipRect(const CFX_RectF& rtClip) {
  m_rtClip = rtClip;
  return m_pDevice->SetClip_Rect(FX_RECT((int32_t)FXSYS_floor(rtClip.left),
                                         (int32_t)FXSYS_floor(rtClip.top),
                                         (int32_t)FXSYS_ceil(rtClip.right()),
                                         (int32_t)FXSYS_ceil(rtClip.bottom())));
}
const CFX_RectF& CFDE_RenderDevice::GetClipRect() {
  return m_rtClip;
}
FX_BOOL CFDE_RenderDevice::SetClipPath(const CFDE_Path* pClip) {
  return FALSE;
}
CFDE_Path* CFDE_RenderDevice::GetClipPath() const {
  return nullptr;
}
FX_FLOAT CFDE_RenderDevice::GetDpiX() const {
  return 96;
}
FX_FLOAT CFDE_RenderDevice::GetDpiY() const {
  return 96;
}
FX_BOOL CFDE_RenderDevice::DrawImage(CFX_DIBSource* pDib,
                                     const CFX_RectF* pSrcRect,
                                     const CFX_RectF& dstRect,
                                     const CFX_Matrix* pImgMatrix,
                                     const CFX_Matrix* pDevMatrix) {
  CFX_RectF srcRect;
  if (pSrcRect) {
    srcRect = *pSrcRect;
  } else {
    srcRect.Set(0, 0, (FX_FLOAT)pDib->GetWidth(), (FX_FLOAT)pDib->GetHeight());
  }
  if (srcRect.IsEmpty()) {
    return FALSE;
  }
  CFX_Matrix dib2fxdev;
  if (pImgMatrix) {
    dib2fxdev = *pImgMatrix;
  } else {
    dib2fxdev.SetIdentity();
  }
  dib2fxdev.a = dstRect.width;
  dib2fxdev.d = -dstRect.height;
  dib2fxdev.e = dstRect.left;
  dib2fxdev.f = dstRect.bottom();
  if (pDevMatrix) {
    dib2fxdev.Concat(*pDevMatrix);
  }
  void* handle = nullptr;
  m_pDevice->StartDIBits(pDib, 255, 0, (const CFX_Matrix*)&dib2fxdev, 0,
                         handle);
  while (m_pDevice->ContinueDIBits(handle, nullptr)) {
  }
  m_pDevice->CancelDIBits(handle);
  return !!handle;
}
FX_BOOL CFDE_RenderDevice::DrawString(CFDE_Brush* pBrush,
                                      CFGAS_GEFont* pFont,
                                      const FXTEXT_CHARPOS* pCharPos,
                                      int32_t iCount,
                                      FX_FLOAT fFontSize,
                                      const CFX_Matrix* pMatrix) {
  ASSERT(pBrush && pFont && pCharPos && iCount > 0);
  CFX_FontCache* pCache = CFX_GEModule::Get()->GetFontCache();
  CFX_Font* pFxFont = pFont->GetDevFont();
  FX_ARGB argb = pBrush->GetColor();
  if ((pFont->GetFontStyles() & FX_FONTSTYLE_Italic) != 0 &&
      !pFxFont->IsItalic()) {
    FXTEXT_CHARPOS* pCP = (FXTEXT_CHARPOS*)pCharPos;
    FX_FLOAT* pAM;
    for (int32_t i = 0; i < iCount; ++i) {
      static const FX_FLOAT mc = 0.267949f;
      pAM = pCP->m_AdjustMatrix;
      pAM[2] = mc * pAM[0] + pAM[2];
      pAM[3] = mc * pAM[1] + pAM[3];
      pCP++;
    }
  }
  FXTEXT_CHARPOS* pCP = (FXTEXT_CHARPOS*)pCharPos;
  CFGAS_GEFont* pCurFont = nullptr;
  CFGAS_GEFont* pSTFont = nullptr;
  FXTEXT_CHARPOS* pCurCP = nullptr;
  int32_t iCurCount = 0;

#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  uint32_t dwFontStyle = pFont->GetFontStyles();
  CFX_Font FxFont;
  CFX_SubstFont SubstFxFont;
  FxFont.SetSubstFont(&SubstFxFont);
  SubstFxFont.m_Weight = dwFontStyle & FX_FONTSTYLE_Bold ? 700 : 400;
  SubstFxFont.m_ItalicAngle = dwFontStyle & FX_FONTSTYLE_Italic ? -12 : 0;
  SubstFxFont.m_WeightCJK = SubstFxFont.m_Weight;
  SubstFxFont.m_bItalicCJK = !!(dwFontStyle & FX_FONTSTYLE_Italic);
#endif  // _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_

  for (int32_t i = 0; i < iCount; ++i) {
    pSTFont = pFont->GetSubstFont((int32_t)pCP->m_GlyphIndex);
    pCP->m_GlyphIndex &= 0x00FFFFFF;
    pCP->m_bFontStyle = false;
    if (pCurFont != pSTFont) {
      if (pCurFont) {
        pFxFont = pCurFont->GetDevFont();
#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
        FxFont.SetFace(pFxFont->GetFace());
        m_pDevice->DrawNormalText(iCurCount, pCurCP, &FxFont, pCache,
                                  -fFontSize, (const CFX_Matrix*)pMatrix, argb,
                                  FXTEXT_CLEARTYPE);
#else
        m_pDevice->DrawNormalText(iCurCount, pCurCP, pFxFont, pCache,
                                  -fFontSize, (const CFX_Matrix*)pMatrix, argb,
                                  FXTEXT_CLEARTYPE);
#endif  // _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
      }
      pCurFont = pSTFont;
      pCurCP = pCP;
      iCurCount = 1;
    } else {
      iCurCount++;
    }
    pCP++;
  }
  if (pCurFont && iCurCount) {
    pFxFont = pCurFont->GetDevFont();
#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
    FxFont.SetFace(pFxFont->GetFace());
    FX_BOOL bRet = m_pDevice->DrawNormalText(
        iCurCount, pCurCP, &FxFont, pCache, -fFontSize,
        (const CFX_Matrix*)pMatrix, argb, FXTEXT_CLEARTYPE);
    FxFont.SetSubstFont(nullptr);
    FxFont.SetFace(nullptr);
    return bRet;
#else
    return m_pDevice->DrawNormalText(iCurCount, pCurCP, pFxFont, pCache,
                                     -fFontSize, (const CFX_Matrix*)pMatrix,
                                     argb, FXTEXT_CLEARTYPE);
#endif  // _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  }

#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  FxFont.SetSubstFont(nullptr);
  FxFont.SetFace(nullptr);
#endif  // _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_

  return TRUE;
}

FX_BOOL CFDE_RenderDevice::DrawBezier(CFDE_Pen* pPen,
                                      FX_FLOAT fPenWidth,
                                      const CFX_PointF& pt1,
                                      const CFX_PointF& pt2,
                                      const CFX_PointF& pt3,
                                      const CFX_PointF& pt4,
                                      const CFX_Matrix* pMatrix) {
  CFX_PointsF points;
  points.Add(pt1);
  points.Add(pt2);
  points.Add(pt3);
  points.Add(pt4);
  CFDE_Path path;
  path.AddBezier(points);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawCurve(CFDE_Pen* pPen,
                                     FX_FLOAT fPenWidth,
                                     const CFX_PointsF& points,
                                     FX_BOOL bClosed,
                                     FX_FLOAT fTension,
                                     const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddCurve(points, bClosed, fTension);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawEllipse(CFDE_Pen* pPen,
                                       FX_FLOAT fPenWidth,
                                       const CFX_RectF& rect,
                                       const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddEllipse(rect);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawLines(CFDE_Pen* pPen,
                                     FX_FLOAT fPenWidth,
                                     const CFX_PointsF& points,
                                     const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddLines(points);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawLine(CFDE_Pen* pPen,
                                    FX_FLOAT fPenWidth,
                                    const CFX_PointF& pt1,
                                    const CFX_PointF& pt2,
                                    const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddLine(pt1, pt2);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawPath(CFDE_Pen* pPen,
                                    FX_FLOAT fPenWidth,
                                    const CFDE_Path* pPath,
                                    const CFX_Matrix* pMatrix) {
  CFDE_Path* pGePath = (CFDE_Path*)pPath;
  if (!pGePath)
    return FALSE;

  CFX_GraphStateData graphState;
  if (!CreatePen(pPen, fPenWidth, graphState)) {
    return FALSE;
  }
  return m_pDevice->DrawPath(&pGePath->m_Path, (const CFX_Matrix*)pMatrix,
                             &graphState, 0, pPen->GetColor(), 0);
}
FX_BOOL CFDE_RenderDevice::DrawPolygon(CFDE_Pen* pPen,
                                       FX_FLOAT fPenWidth,
                                       const CFX_PointsF& points,
                                       const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddPolygon(points);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::DrawRectangle(CFDE_Pen* pPen,
                                         FX_FLOAT fPenWidth,
                                         const CFX_RectF& rect,
                                         const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddRectangle(rect);
  return DrawPath(pPen, fPenWidth, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::FillClosedCurve(CFDE_Brush* pBrush,
                                           const CFX_PointsF& points,
                                           FX_FLOAT fTension,
                                           const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddCurve(points, TRUE, fTension);
  return FillPath(pBrush, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::FillEllipse(CFDE_Brush* pBrush,
                                       const CFX_RectF& rect,
                                       const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddEllipse(rect);
  return FillPath(pBrush, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::FillPolygon(CFDE_Brush* pBrush,
                                       const CFX_PointsF& points,
                                       const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddPolygon(points);
  return FillPath(pBrush, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::FillRectangle(CFDE_Brush* pBrush,
                                         const CFX_RectF& rect,
                                         const CFX_Matrix* pMatrix) {
  CFDE_Path path;
  path.AddRectangle(rect);
  return FillPath(pBrush, &path, pMatrix);
}
FX_BOOL CFDE_RenderDevice::CreatePen(CFDE_Pen* pPen,
                                     FX_FLOAT fPenWidth,
                                     CFX_GraphStateData& graphState) {
  if (!pPen)
    return FALSE;

  graphState.m_LineCap = CFX_GraphStateData::LineCapButt;
  graphState.m_LineJoin = CFX_GraphStateData::LineJoinMiter;
  graphState.m_LineWidth = fPenWidth;
  graphState.m_MiterLimit = 10;
  graphState.m_DashPhase = 0;
  return TRUE;
}

FX_BOOL CFDE_RenderDevice::FillPath(CFDE_Brush* pBrush,
                                    const CFDE_Path* pPath,
                                    const CFX_Matrix* pMatrix) {
  CFDE_Path* pGePath = (CFDE_Path*)pPath;
  if (!pGePath)
    return FALSE;
  if (!pBrush)
    return FALSE;
  return m_pDevice->DrawPath(&pGePath->m_Path, pMatrix, nullptr,
                             pBrush->GetColor(), 0, FXFILL_WINDING);
}

