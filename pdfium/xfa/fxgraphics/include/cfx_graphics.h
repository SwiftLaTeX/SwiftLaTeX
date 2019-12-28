// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXGRAPHICS_INCLUDE_CFX_GRAPHICS_H_
#define XFA_FXGRAPHICS_INCLUDE_CFX_GRAPHICS_H_

#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"
#include "core/fxge/include/fx_ge.h"
#include "xfa/fwl/core/fwl_error.h"

class CFX_Color;
class CFX_Path;
class CAGG_Graphics;

using FX_DeviceCap = int32_t;
using FX_FillMode = int32_t;

enum FX_DashStyle {
  FX_DASHSTYLE_Solid = 0,
  FX_DASHSTYLE_Dash = 1,
  FX_DASHSTYLE_Dot = 2,
  FX_DASHSTYLE_DashDot = 3,
  FX_DASHSTYLE_DashDotDot = 4
};

enum FX_StrokeAlignment {
  FX_STROKEALIGNMENT_Center = 0,
  FX_STROKEALIGNMENT_Inset = 1,
  FX_STROKEALIGNMENT_Outset = 2,
  FX_STROKEALIGNMENT_Left = 3,
  FX_STROKEALIGNMENT_Right = 4
};

enum FX_HatchStyle {
  FX_HATCHSTYLE_Horizontal = 0,
  FX_HATCHSTYLE_Vertical = 1,
  FX_HATCHSTYLE_ForwardDiagonal = 2,
  FX_HATCHSTYLE_BackwardDiagonal = 3,
  FX_HATCHSTYLE_Cross = 4,
  FX_HATCHSTYLE_DiagonalCross = 5,
  FX_HATCHSTYLE_05Percent = 6,
  FX_HATCHSTYLE_10Percent = 7,
  FX_HATCHSTYLE_20Percent = 8,
  FX_HATCHSTYLE_25Percent = 9,
  FX_HATCHSTYLE_30Percent = 10,
  FX_HATCHSTYLE_40Percent = 11,
  FX_HATCHSTYLE_50Percent = 12,
  FX_HATCHSTYLE_60Percent = 13,
  FX_HATCHSTYLE_70Percent = 14,
  FX_HATCHSTYLE_75Percent = 15,
  FX_HATCHSTYLE_80Percent = 16,
  FX_HATCHSTYLE_90Percent = 17,
  FX_HATCHSTYLE_LightDownwardDiagonal = 18,
  FX_HATCHSTYLE_LightUpwardDiagonal = 19,
  FX_HATCHSTYLE_DarkDownwardDiagonal = 20,
  FX_HATCHSTYLE_DarkUpwardDiagonal = 21,
  FX_HATCHSTYLE_WideDownwardDiagonal = 22,
  FX_HATCHSTYLE_WideUpwardDiagonal = 23,
  FX_HATCHSTYLE_LightVertical = 24,
  FX_HATCHSTYLE_LightHorizontal = 25,
  FX_HATCHSTYLE_NarrowVertical = 26,
  FX_HATCHSTYLE_NarrowHorizontal = 27,
  FX_HATCHSTYLE_DarkVertical = 28,
  FX_HATCHSTYLE_DarkHorizontal = 29,
  FX_HATCHSTYLE_DashedDownwardDiagonal = 30,
  FX_HATCHSTYLE_DashedUpwardDiagonal = 31,
  FX_HATCHSTYLE_DashedHorizontal = 32,
  FX_HATCHSTYLE_DashedVertical = 33,
  FX_HATCHSTYLE_SmallConfetti = 34,
  FX_HATCHSTYLE_LargeConfetti = 35,
  FX_HATCHSTYLE_ZigZag = 36,
  FX_HATCHSTYLE_Wave = 37,
  FX_HATCHSTYLE_DiagonalBrick = 38,
  FX_HATCHSTYLE_HorizontalBrick = 39,
  FX_HATCHSTYLE_Weave = 40,
  FX_HATCHSTYLE_Plaid = 41,
  FX_HATCHSTYLE_Divot = 42,
  FX_HATCHSTYLE_DottedGrid = 43,
  FX_HATCHSTYLE_DottedDiamond = 44,
  FX_HATCHSTYLE_Shingle = 45,
  FX_HATCHSTYLE_Trellis = 46,
  FX_HATCHSTYLE_Sphere = 47,
  FX_HATCHSTYLE_SmallGrid = 48,
  FX_HATCHSTYLE_SmallCheckerBoard = 49,
  FX_HATCHSTYLE_LargeCheckerBoard = 50,
  FX_HATCHSTYLE_OutlinedDiamond = 51,
  FX_HATCHSTYLE_SolidDiamond = 52
};

class CFX_RenderDevice;

class CFX_Graphics {
 public:
  CFX_Graphics();
  virtual ~CFX_Graphics();

  FWL_Error Create(CFX_RenderDevice* renderDevice,
                   FX_BOOL isAntialiasing = TRUE);
  FWL_Error Create(int32_t width,
                   int32_t height,
                   FXDIB_Format format,
                   FX_BOOL isNative = TRUE,
                   FX_BOOL isAntialiasing = TRUE);

  FWL_Error GetDeviceCap(const int32_t capID, FX_DeviceCap& capVal);
  FWL_Error IsPrinterDevice(FX_BOOL& isPrinter);
  FWL_Error EnableAntialiasing(FX_BOOL isAntialiasing);

  FWL_Error SaveGraphState();
  FWL_Error RestoreGraphState();

  FWL_Error GetLineCap(CFX_GraphStateData::LineCap& lineCap) const;
  FWL_Error GetDashCount(int32_t& dashCount) const;
  FWL_Error GetLineDash(FX_FLOAT& dashPhase, FX_FLOAT* dashArray) const;
  FWL_Error GetLineJoin(CFX_GraphStateData::LineJoin& lineJoin) const;
  FWL_Error GetMiterLimit(FX_FLOAT& miterLimit) const;
  FWL_Error GetLineWidth(FX_FLOAT& lineWidth) const;
  FWL_Error GetStrokeAlignment(FX_StrokeAlignment& strokeAlignment) const;
  FWL_Error GetClipRect(CFX_RectF& rect) const;
  CFX_Matrix* GetMatrix();
  CFX_RenderDevice* GetRenderDevice();

  FWL_Error SetLineCap(CFX_GraphStateData::LineCap lineCap);
  FWL_Error SetLineDash(FX_FLOAT dashPhase,
                        FX_FLOAT* dashArray,
                        int32_t dashCount);
  FWL_Error SetLineDash(FX_DashStyle dashStyle);
  FWL_Error SetLineJoin(CFX_GraphStateData::LineJoin lineJoin);
  FWL_Error SetMiterLimit(FX_FLOAT miterLimit);
  FWL_Error SetLineWidth(FX_FLOAT lineWidth, FX_BOOL isActOnDash = FALSE);
  FWL_Error SetStrokeAlignment(FX_StrokeAlignment strokeAlignment);
  FWL_Error SetStrokeColor(CFX_Color* color);
  FWL_Error SetFillColor(CFX_Color* color);
  FWL_Error SetClipRect(const CFX_RectF& rect);
  FWL_Error SetFont(CFX_Font* font);
  FWL_Error SetFontSize(const FX_FLOAT size);
  FWL_Error SetFontHScale(const FX_FLOAT scale);
  FWL_Error SetCharSpacing(const FX_FLOAT spacing);
  FWL_Error SetTextDrawingMode(const int32_t mode);

  FWL_Error StrokePath(CFX_Path* path, CFX_Matrix* matrix = nullptr);
  FWL_Error FillPath(CFX_Path* path,
                     FX_FillMode fillMode = FXFILL_WINDING,
                     CFX_Matrix* matrix = nullptr);
  FWL_Error ClipPath(CFX_Path* path,
                     FX_FillMode fillMode = FXFILL_WINDING,
                     CFX_Matrix* matrix = nullptr);
  FWL_Error DrawImage(CFX_DIBSource* source,
                      const CFX_PointF& point,
                      CFX_Matrix* matrix = nullptr);
  FWL_Error StretchImage(CFX_DIBSource* source,
                         const CFX_RectF& rect,
                         CFX_Matrix* matrix = nullptr);
  FWL_Error ConcatMatrix(const CFX_Matrix* matrix);
  FWL_Error ClearClip();
  FWL_Error ShowText(const CFX_PointF& point,
                     const CFX_WideString& text,
                     CFX_Matrix* matrix = nullptr);
  FWL_Error CalcTextRect(CFX_RectF& rect,
                         const CFX_WideString& text,
                         FX_BOOL isMultiline = FALSE,
                         CFX_Matrix* matrix = nullptr);
  FWL_Error Transfer(CFX_Graphics* graphics, const CFX_Matrix* matrix);
  FWL_Error Transfer(CFX_Graphics* graphics,
                     FX_FLOAT srcLeft,
                     FX_FLOAT srcTop,
                     const CFX_RectF& dstRect,
                     const CFX_Matrix* matrix);

  FWL_Error InverseRect(const CFX_RectF& rect);
  FWL_Error XorDIBitmap(const CFX_DIBitmap* srcBitmap, const CFX_RectF& rect);
  FWL_Error EqvDIBitmap(const CFX_DIBitmap* srcBitmap, const CFX_RectF& rect);

 protected:
  int32_t m_type;

 private:
  struct TInfo {
    TInfo();
    explicit TInfo(const TInfo& info);
    TInfo& operator=(const TInfo& other);

    CFX_GraphStateData graphState;
    FX_BOOL isAntialiasing;
    FX_StrokeAlignment strokeAlignment;
    CFX_Matrix CTM;
    FX_BOOL isActOnDash;
    CFX_Color* strokeColor;
    CFX_Color* fillColor;
    CFX_Font* font;
    FX_FLOAT fontSize;
    FX_FLOAT fontHScale;
    FX_FLOAT fontSpacing;
  } m_info;

  FWL_Error RenderDeviceSetLineDash(FX_DashStyle dashStyle);
  FWL_Error RenderDeviceStrokePath(CFX_Path* path, CFX_Matrix* matrix);
  FWL_Error RenderDeviceFillPath(CFX_Path* path,
                                 FX_FillMode fillMode,
                                 CFX_Matrix* matrix);
  FWL_Error RenderDeviceDrawImage(CFX_DIBSource* source,
                                  const CFX_PointF& point,
                                  CFX_Matrix* matrix);
  FWL_Error RenderDeviceStretchImage(CFX_DIBSource* source,
                                     const CFX_RectF& rect,
                                     CFX_Matrix* matrix);
  FWL_Error RenderDeviceShowText(const CFX_PointF& point,
                                 const CFX_WideString& text,
                                 CFX_Matrix* matrix);

  FWL_Error StrokePathWithPattern(CFX_Path* path, CFX_Matrix* matrix);
  FWL_Error StrokePathWithShading(CFX_Path* path, CFX_Matrix* matrix);

  FWL_Error FillPathWithPattern(CFX_Path* path,
                                FX_FillMode fillMode,
                                CFX_Matrix* matrix);
  FWL_Error FillPathWithShading(CFX_Path* path,
                                FX_FillMode fillMode,
                                CFX_Matrix* matrix);

  FWL_Error SetDIBitsWithMatrix(CFX_DIBSource* source, CFX_Matrix* matrix);
  FWL_Error CalcTextInfo(const CFX_WideString& text,
                         uint32_t* charCodes,
                         FXTEXT_CHARPOS* charPos,
                         CFX_RectF& rect);

  CFX_RenderDevice* m_renderDevice;
  CFX_ArrayTemplate<TInfo*> m_infoStack;
  CAGG_Graphics* m_aggGraphics;
  friend class CAGG_Graphics;
};

#endif  // XFA_FXGRAPHICS_INCLUDE_CFX_GRAPHICS_H_
