// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_FDE_GEDEVICE_H_
#define XFA_FDE_FDE_GEDEVICE_H_

#include "core/fxge/include/fx_ge.h"
#include "xfa/fgas/crt/fgas_memory.h"

class CFDE_Brush;
class CFDE_Path;
class CFDE_Pen;
class CFGAS_GEFont;

class CFDE_RenderDevice : public CFX_Target {
 public:
  CFDE_RenderDevice(CFX_RenderDevice* pDevice, FX_BOOL bOwnerDevice);
  ~CFDE_RenderDevice() override;

  int32_t GetWidth() const;
  int32_t GetHeight() const;
  void SaveState();
  void RestoreState();
  FX_BOOL SetClipPath(const CFDE_Path* pClip);
  CFDE_Path* GetClipPath() const;
  FX_BOOL SetClipRect(const CFX_RectF& rtClip);
  const CFX_RectF& GetClipRect();

  FX_FLOAT GetDpiX() const;
  FX_FLOAT GetDpiY() const;

  FX_BOOL DrawImage(CFX_DIBSource* pDib,
                    const CFX_RectF* pSrcRect,
                    const CFX_RectF& dstRect,
                    const CFX_Matrix* pImgMatrix = nullptr,
                    const CFX_Matrix* pDevMatrix = nullptr);
  FX_BOOL DrawString(CFDE_Brush* pBrush,
                     CFGAS_GEFont* pFont,
                     const FXTEXT_CHARPOS* pCharPos,
                     int32_t iCount,
                     FX_FLOAT fFontSize,
                     const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawBezier(CFDE_Pen* pPen,
                     FX_FLOAT fPenWidth,
                     const CFX_PointF& pt1,
                     const CFX_PointF& pt2,
                     const CFX_PointF& pt3,
                     const CFX_PointF& pt4,
                     const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawCurve(CFDE_Pen* pPen,
                    FX_FLOAT fPenWidth,
                    const CFX_PointsF& points,
                    FX_BOOL bClosed,
                    FX_FLOAT fTension = 0.5f,
                    const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawEllipse(CFDE_Pen* pPen,
                      FX_FLOAT fPenWidth,
                      const CFX_RectF& rect,
                      const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawLines(CFDE_Pen* pPen,
                    FX_FLOAT fPenWidth,
                    const CFX_PointsF& points,
                    const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawLine(CFDE_Pen* pPen,
                   FX_FLOAT fPenWidth,
                   const CFX_PointF& pt1,
                   const CFX_PointF& pt2,
                   const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawPath(CFDE_Pen* pPen,
                   FX_FLOAT fPenWidth,
                   const CFDE_Path* pPath,
                   const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawPolygon(CFDE_Pen* pPen,
                      FX_FLOAT fPenWidth,
                      const CFX_PointsF& points,
                      const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL DrawRectangle(CFDE_Pen* pPen,
                        FX_FLOAT fPenWidth,
                        const CFX_RectF& rect,
                        const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL FillClosedCurve(CFDE_Brush* pBrush,
                          const CFX_PointsF& points,
                          FX_FLOAT fTension = 0.5f,
                          const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL FillEllipse(CFDE_Brush* pBrush,
                      const CFX_RectF& rect,
                      const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL FillPath(CFDE_Brush* pBrush,
                   const CFDE_Path* pPath,
                   const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL FillPolygon(CFDE_Brush* pBrush,
                      const CFX_PointsF& points,
                      const CFX_Matrix* pMatrix = nullptr);
  FX_BOOL FillRectangle(CFDE_Brush* pBrush,
                        const CFX_RectF& rect,
                        const CFX_Matrix* pMatrix = nullptr);

  FX_BOOL DrawSolidString(CFDE_Brush* pBrush,
                          CFGAS_GEFont* pFont,
                          const FXTEXT_CHARPOS* pCharPos,
                          int32_t iCount,
                          FX_FLOAT fFontSize,
                          const CFX_Matrix* pMatrix);
  FX_BOOL DrawStringPath(CFDE_Brush* pBrush,
                         CFGAS_GEFont* pFont,
                         const FXTEXT_CHARPOS* pCharPos,
                         int32_t iCount,
                         FX_FLOAT fFontSize,
                         const CFX_Matrix* pMatrix);

 protected:
  FX_BOOL CreatePen(CFDE_Pen* pPen,
                    FX_FLOAT fPenWidth,
                    CFX_GraphStateData& graphState);

  CFX_RenderDevice* m_pDevice;
  CFX_RectF m_rtClip;
  FX_BOOL m_bOwnerDevice;
  FXTEXT_CHARPOS* m_pCharPos;
  int32_t m_iCharCount;
};

#endif  // XFA_FDE_FDE_GEDEVICE_H_
