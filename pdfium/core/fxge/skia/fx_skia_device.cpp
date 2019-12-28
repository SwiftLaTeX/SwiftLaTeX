// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(_SKIA_SUPPORT_)
#include <algorithm>
#include <vector>

#include "core/fxcodec/include/fx_codec.h"

#include "core/fpdfapi/fpdf_page/cpdf_shadingpattern.h"
#include "core/fpdfapi/fpdf_page/pageint.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fxge/skia/fx_skia_device.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkColorPriv.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/effects/SkDashPathEffect.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "third_party/skia/include/pathops/SkPathOps.h"

#ifdef SK_DEBUG
#include "third_party/skia/include/core/SkClipStack.h"
#endif

namespace {

#define SHOW_SKIA_PATH 0  // set to 1 to print the path contents
#define DRAW_SKIA_CLIP 0  // set to 1 to draw a green rectangle around the clip

void DebugShowSkiaPath(const SkPath& path) {
#if SHOW_SKIA_PATH
  char buffer[4096];
  sk_bzero(buffer, sizeof(buffer));
  SkMemoryWStream stream(buffer, sizeof(buffer));
  path.dump(&stream, false, false);
  printf("%s\n", buffer);
#endif  // SHOW_SKIA_PATH
}

void DebugShowCanvasMatrix(const SkCanvas* canvas) {
#if SHOW_SKIA_PATH
  SkMatrix matrix = canvas->getTotalMatrix();
  SkScalar m[9];
  matrix.get9(m);
  printf("(%g,%g,%g) (%g,%g,%g) (%g,%g,%g)\n", m[0], m[1], m[2], m[3], m[4],
         m[5], m[6], m[7], m[8]);
#endif  // SHOW_SKIA_PATH
}

#if DRAW_SKIA_CLIP

SkPaint DebugClipPaint() {
  SkPaint paint;
  paint.setAntiAlias(true);
  paint.setColor(SK_ColorGREEN);
  paint.setStyle(SkPaint::kStroke_Style);
  return paint;
}

void DebugDrawSkiaClipRect(SkCanvas* canvas, const SkRect& rect) {
  SkPaint paint = DebugClipPaint();
  canvas->drawRect(rect, paint);
}

void DebugDrawSkiaClipPath(SkCanvas* canvas, const SkPath& path) {
  SkPaint paint = DebugClipPaint();
  canvas->drawPath(path, paint);
}

#else  // DRAW_SKIA_CLIP

void DebugDrawSkiaClipRect(SkCanvas* canvas, const SkRect& rect) {}
void DebugDrawSkiaClipPath(SkCanvas* canvas, const SkPath& path) {}

#endif  // DRAW_SKIA_CLIP

#undef SHOW_SKIA_PATH
#undef DRAW_SKIA_CLIP

static void DebugValidate(const CFX_DIBitmap* bitmap,
                          const CFX_DIBitmap* device) {
  if (bitmap) {
    SkASSERT(bitmap->GetBPP() == 8 || bitmap->GetBPP() == 32);
    if (bitmap->GetBPP() == 32) {
      bitmap->DebugVerifyBitmapIsPreMultiplied();
    }
  }
  if (device) {
    SkASSERT(device->GetBPP() == 8 || device->GetBPP() == 32);
    if (device->GetBPP() == 32) {
      device->DebugVerifyBitmapIsPreMultiplied();
    }
  }
}

SkPath BuildPath(const CFX_PathData* pPathData) {
  SkPath skPath;
  const CFX_PathData* pFPath = pPathData;
  int nPoints = pFPath->GetPointCount();
  FX_PATHPOINT* pPoints = pFPath->GetPoints();
  for (int i = 0; i < nPoints; i++) {
    FX_FLOAT x = pPoints[i].m_PointX;
    FX_FLOAT y = pPoints[i].m_PointY;
    int point_type = pPoints[i].m_Flag & FXPT_TYPE;
    if (point_type == FXPT_MOVETO) {
      skPath.moveTo(x, y);
    } else if (point_type == FXPT_LINETO) {
      skPath.lineTo(x, y);
    } else if (point_type == FXPT_BEZIERTO) {
      FX_FLOAT x2 = pPoints[i + 1].m_PointX, y2 = pPoints[i + 1].m_PointY;
      FX_FLOAT x3 = pPoints[i + 2].m_PointX, y3 = pPoints[i + 2].m_PointY;
      skPath.cubicTo(x, y, x2, y2, x3, y3);
      i += 2;
    }
    if (pPoints[i].m_Flag & FXPT_CLOSEFIGURE)
      skPath.close();
  }
  return skPath;
}

SkMatrix ToSkMatrix(const CFX_Matrix& m) {
  SkMatrix skMatrix;
  skMatrix.setAll(m.a, m.b, m.e, m.c, m.d, m.f, 0, 0, 1);
  return skMatrix;
}

// use when pdf's y-axis points up insead of down
SkMatrix ToFlippedSkMatrix(const CFX_Matrix& m) {
  SkMatrix skMatrix;
  skMatrix.setAll(m.a, m.b, m.e, -m.c, -m.d, m.f, 0, 0, 1);
  return skMatrix;
}

SkXfermode::Mode GetSkiaBlendMode(int blend_type) {
  switch (blend_type) {
    case FXDIB_BLEND_MULTIPLY:
      return SkXfermode::kMultiply_Mode;
    case FXDIB_BLEND_SCREEN:
      return SkXfermode::kScreen_Mode;
    case FXDIB_BLEND_OVERLAY:
      return SkXfermode::kOverlay_Mode;
    case FXDIB_BLEND_DARKEN:
      return SkXfermode::kDarken_Mode;
    case FXDIB_BLEND_LIGHTEN:
      return SkXfermode::kLighten_Mode;
    case FXDIB_BLEND_COLORDODGE:
      return SkXfermode::kColorDodge_Mode;
    case FXDIB_BLEND_COLORBURN:
      return SkXfermode::kColorBurn_Mode;
    case FXDIB_BLEND_HARDLIGHT:
      return SkXfermode::kHardLight_Mode;
    case FXDIB_BLEND_SOFTLIGHT:
      return SkXfermode::kSoftLight_Mode;
    case FXDIB_BLEND_DIFFERENCE:
      return SkXfermode::kDifference_Mode;
    case FXDIB_BLEND_EXCLUSION:
      return SkXfermode::kExclusion_Mode;
    case FXDIB_BLEND_HUE:
      return SkXfermode::kHue_Mode;
    case FXDIB_BLEND_SATURATION:
      return SkXfermode::kSaturation_Mode;
    case FXDIB_BLEND_COLOR:
      return SkXfermode::kColor_Mode;
    case FXDIB_BLEND_LUMINOSITY:
      return SkXfermode::kLuminosity_Mode;
    case FXDIB_BLEND_NORMAL:
    default:
      return SkXfermode::kSrcOver_Mode;
  }
}

bool AddColors(const CPDF_ExpIntFunc* pFunc, SkTDArray<SkColor>* skColors) {
  if (pFunc->CountInputs() != 1)
    return false;
  if (pFunc->m_Exponent != 1)
    return false;
  if (pFunc->m_nOrigOutputs != 3)
    return false;
  skColors->push(
      SkColorSetARGB(0xFF, SkUnitScalarClampToByte(pFunc->m_pBeginValues[0]),
                     SkUnitScalarClampToByte(pFunc->m_pBeginValues[1]),
                     SkUnitScalarClampToByte(pFunc->m_pBeginValues[2])));
  skColors->push(
      SkColorSetARGB(0xFF, SkUnitScalarClampToByte(pFunc->m_pEndValues[0]),
                     SkUnitScalarClampToByte(pFunc->m_pEndValues[1]),
                     SkUnitScalarClampToByte(pFunc->m_pEndValues[2])));
  return true;
}

uint8_t FloatToByte(FX_FLOAT f) {
  ASSERT(0 <= f && f <= 1);
  return (uint8_t)(f * 255.99f);
}

bool AddSamples(const CPDF_SampledFunc* pFunc,
                SkTDArray<SkColor>* skColors,
                SkTDArray<SkScalar>* skPos) {
  if (pFunc->CountInputs() != 1)
    return false;
  if (pFunc->CountOutputs() != 3)  // expect rgb
    return false;
  if (pFunc->GetEncodeInfo().empty())
    return false;
  const CPDF_SampledFunc::SampleEncodeInfo& encodeInfo =
      pFunc->GetEncodeInfo()[0];
  if (encodeInfo.encode_min != 0)
    return false;
  if (encodeInfo.encode_max != encodeInfo.sizes - 1)
    return false;
  uint32_t sampleSize = pFunc->GetBitsPerSample();
  uint32_t sampleCount = encodeInfo.sizes;
  if (sampleCount != 1U << sampleSize)
    return false;
  if (pFunc->GetSampleStream()->GetSize() < sampleCount * 3 * sampleSize / 8)
    return false;

  FX_FLOAT colorsMin[3];
  FX_FLOAT colorsMax[3];
  for (int i = 0; i < 3; ++i) {
    colorsMin[i] = pFunc->GetRange(i * 2);
    colorsMax[i] = pFunc->GetRange(i * 2 + 1);
  }
  const uint8_t* pSampleData = pFunc->GetSampleStream()->GetData();
  for (uint32_t i = 0; i < sampleCount; ++i) {
    FX_FLOAT floatColors[3];
    for (uint32_t j = 0; j < 3; ++j) {
      int sample = GetBits32(pSampleData, (i * 3 + j) * sampleSize, sampleSize);
      FX_FLOAT interp = (FX_FLOAT)sample / (sampleCount - 1);
      floatColors[j] = colorsMin[j] + (colorsMax[j] - colorsMin[j]) * interp;
    }
    SkColor color =
        SkPackARGB32(0xFF, FloatToByte(floatColors[0]),
                     FloatToByte(floatColors[1]), FloatToByte(floatColors[2]));
    skColors->push(color);
    skPos->push((FX_FLOAT)i / (sampleCount - 1));
  }
  return true;
}

bool AddStitching(const CPDF_StitchFunc* pFunc,
                  SkTDArray<SkColor>* skColors,
                  SkTDArray<SkScalar>* skPos) {
  int inputs = pFunc->CountInputs();
  FX_FLOAT boundsStart = pFunc->GetDomain(0);

  const auto& subFunctions = pFunc->GetSubFunctions();
  for (int i = 0; i < inputs; ++i) {
    const CPDF_ExpIntFunc* pSubFunc = subFunctions[i]->ToExpIntFunc();
    if (!pSubFunc)
      return false;
    if (!AddColors(pSubFunc, skColors))
      return false;
    FX_FLOAT boundsEnd =
        i < inputs - 1 ? pFunc->GetBound(i) : pFunc->GetDomain(1);
    skPos->push(boundsStart);
    skPos->push(boundsEnd);
    boundsStart = boundsEnd;
  }
  return true;
}

// see https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
SkScalar LineSide(const SkPoint line[2], const SkPoint& pt) {
  return (line[1].fY - line[0].fY) * pt.fX - (line[1].fX - line[0].fX) * pt.fY +
         line[1].fX * line[0].fY - line[1].fY * line[0].fX;
}

SkPoint IntersectSides(const SkPoint& parallelPt,
                       const SkVector& paraRay,
                       const SkPoint& perpendicularPt) {
  SkVector perpRay = {paraRay.fY, -paraRay.fX};
  SkScalar denom = perpRay.fY * paraRay.fX - paraRay.fY * perpRay.fX;
  if (!denom) {
    SkPoint zeroPt = {0, 0};
    return zeroPt;
  }
  SkVector ab0 = parallelPt - perpendicularPt;
  SkScalar numerA = ab0.fY * perpRay.fX - perpRay.fY * ab0.fX;
  numerA /= denom;
  SkPoint result = {parallelPt.fX + paraRay.fX * numerA,
                    parallelPt.fY + paraRay.fY * numerA};
  return result;
}

void ClipAngledGradient(const SkPoint pts[2],
                        SkPoint rectPts[4],
                        bool clipStart,
                        bool clipEnd,
                        SkPath* clip) {
  // find the corners furthest from the gradient perpendiculars
  SkScalar minPerpDist = SK_ScalarMax;
  SkScalar maxPerpDist = SK_ScalarMin;
  int minPerpPtIndex = -1;
  int maxPerpPtIndex = -1;
  SkVector slope = pts[1] - pts[0];
  SkPoint startPerp[2] = {pts[0], {pts[0].fX + slope.fY, pts[0].fY - slope.fX}};
  SkPoint endPerp[2] = {pts[1], {pts[1].fX + slope.fY, pts[1].fY - slope.fX}};
  for (int i = 0; i < 4; ++i) {
    SkScalar sDist = LineSide(startPerp, rectPts[i]);
    SkScalar eDist = LineSide(endPerp, rectPts[i]);
    if (sDist * eDist <= 0)  // if the signs are different,
      continue;              // the point is inside the gradient
    if (sDist < 0) {
      SkScalar smaller = SkTMin(sDist, eDist);
      if (minPerpDist > smaller) {
        minPerpDist = smaller;
        minPerpPtIndex = i;
      }
    } else {
      SkScalar larger = SkTMax(sDist, eDist);
      if (maxPerpDist < larger) {
        maxPerpDist = larger;
        maxPerpPtIndex = i;
      }
    }
  }
  if (minPerpPtIndex < 0 && maxPerpPtIndex < 0)  // nothing's outside
    return;
  // determine if negative distances are before start or after end
  SkPoint beforeStart = {pts[0].fX * 2 - pts[1].fX, pts[0].fY * 2 - pts[1].fY};
  bool beforeNeg = LineSide(startPerp, beforeStart) < 0;
  const SkPoint& startEdgePt =
      clipStart ? pts[0] : beforeNeg ? rectPts[minPerpPtIndex]
                                     : rectPts[maxPerpPtIndex];
  const SkPoint& endEdgePt = clipEnd ? pts[1] : beforeNeg
                                                    ? rectPts[maxPerpPtIndex]
                                                    : rectPts[minPerpPtIndex];
  // find the corners that bound the gradient
  SkScalar minDist = SK_ScalarMax;
  SkScalar maxDist = SK_ScalarMin;
  int minBounds = -1;
  int maxBounds = -1;
  for (int i = 0; i < 4; ++i) {
    SkScalar dist = LineSide(pts, rectPts[i]);
    if (minDist > dist) {
      minDist = dist;
      minBounds = i;
    }
    if (maxDist < dist) {
      maxDist = dist;
      maxBounds = i;
    }
  }
  ASSERT(minBounds >= 0);
  ASSERT(maxBounds != minBounds && maxBounds >= 0);
  // construct a clip parallel to the gradient that goes through
  // rectPts[minBounds] and rectPts[maxBounds] and perpendicular to the
  // gradient that goes through startEdgePt, endEdgePt.
  clip->moveTo(IntersectSides(rectPts[minBounds], slope, startEdgePt));
  clip->lineTo(IntersectSides(rectPts[minBounds], slope, endEdgePt));
  clip->lineTo(IntersectSides(rectPts[maxBounds], slope, endEdgePt));
  clip->lineTo(IntersectSides(rectPts[maxBounds], slope, startEdgePt));
}

}  // namespace

// Encapsulate the state used for successive text and path draws so that
// they can be combined.
class SkiaState {
 public:
  enum class Clip {
    kSave,
    kPath,
  };

  // mark all cached state as uninitialized
  SkiaState()
      : m_pFont(nullptr),
        m_pCache(nullptr),
        m_fontSize(0),
        m_fillColor(0),
        m_strokeColor(0),
        m_blendType(0),
        m_commandIndex(0),
        m_drawText(false),
        m_drawPath(false),
        m_fillPath(false),
        m_debugDisable(false) {}

  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pMatrix,
                const CFX_GraphStateData* pDrawState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                int blend_type,
                CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    if (m_commandIndex < m_commands.count())
      FlushCommands(pDriver);
    if (m_drawText)
      FlushText(pDriver);
    if (m_drawPath && DrawChanged(pMatrix, pDrawState, fill_color, stroke_color,
                                  fill_mode, blend_type)) {
      FlushPath(pDriver);
    }
    if (!m_drawPath) {
      m_skPath.reset();
      m_fillPath = (fill_mode & 3) && fill_color;
      m_skPath.setFillType((fill_mode & 3) == FXFILL_ALTERNATE
                               ? SkPath::kEvenOdd_FillType
                               : SkPath::kWinding_FillType);
      if (pDrawState)
        m_drawState.Copy(*pDrawState);
      m_fillColor = fill_color;
      m_strokeColor = stroke_color;
      m_blendType = blend_type;
      if (pMatrix)
        m_drawMatrix = *pMatrix;
    }
    SkPath skPath = BuildPath(pPathData);
    SkPoint delta;
    if (MatrixOffset(pMatrix, &delta))
      skPath.offset(delta.fX, delta.fY);
    m_skPath.addPath(skPath);
    m_drawPath = true;
    return true;
  }

  void FlushPath(CFX_SkiaDeviceDriver* pDriver) {
    SkMatrix skMatrix = ToSkMatrix(m_drawMatrix);
    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    int stroke_alpha = FXARGB_A(m_strokeColor);
    if (stroke_alpha)
      pDriver->PaintStroke(&skPaint, &m_drawState, skMatrix);
    SkCanvas* skCanvas = pDriver->SkiaCanvas();
    skCanvas->save();
    skCanvas->concat(skMatrix);
    if (m_fillPath) {
      SkPath strokePath;
      const SkPath* fillPath = &m_skPath;
      if (stroke_alpha) {
        SkAlpha fillA = SkColorGetA(m_fillColor);
        SkAlpha strokeA = SkColorGetA(m_strokeColor);
        if (fillA && fillA < 0xFF && strokeA && strokeA < 0xFF) {
          skPaint.getFillPath(m_skPath, &strokePath);
          if (Op(m_skPath, strokePath, SkPathOp::kDifference_SkPathOp,
                 &strokePath)) {
            fillPath = &strokePath;
          }
        }
      }
      skPaint.setStyle(SkPaint::kFill_Style);
      skPaint.setColor(m_fillColor);
      skCanvas->drawPath(*fillPath, skPaint);
    }
    if (stroke_alpha) {
      DebugShowSkiaPath(m_skPath);
      DebugShowCanvasMatrix(skCanvas);
      skPaint.setStyle(SkPaint::kStroke_Style);
      skPaint.setColor(m_strokeColor);
      skCanvas->drawPath(m_skPath, skPaint);
    }
    skCanvas->restore();
    m_drawPath = false;
  }

  bool DrawText(int nChars,
                const FXTEXT_CHARPOS* pCharPos,
                CFX_Font* pFont,
                CFX_FontCache* pCache,
                const CFX_Matrix* pMatrix,
                FX_FLOAT font_size,
                uint32_t color,
                CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    if (m_commandIndex < m_commands.count())
      FlushCommands(pDriver);
    if (m_drawPath)
      FlushPath(pDriver);
    if (m_drawText && FontChanged(pFont, pCache, pMatrix, font_size, color))
      FlushText(pDriver);
    if (!m_drawText) {
      m_positions.setCount(0);
      m_glyphs.setCount(0);
      m_pFont = pFont;
      m_pCache = pCache;
      m_fontSize = font_size;
      m_fillColor = color;
      m_drawMatrix = *pMatrix;
    }
    int count = m_positions.count();
    m_positions.setCount(nChars + count);
    m_glyphs.setCount(nChars + count);
    for (int index = 0; index < nChars; ++index) {
      const FXTEXT_CHARPOS& cp = pCharPos[index];
      m_positions[index + count] = {cp.m_OriginX, cp.m_OriginY};
      m_glyphs[index + count] = (uint16_t)cp.m_GlyphIndex;
    }
    SkPoint delta;
    if (MatrixOffset(pMatrix, &delta)) {
      for (int index = 0; index < nChars; ++index)
        m_positions[index + count].offset(delta.fX, -delta.fY);
    }
    m_drawText = true;
    return true;
  }

  void FlushText(CFX_SkiaDeviceDriver* pDriver) {
    SkMatrix skMatrix = ToFlippedSkMatrix(m_drawMatrix);
    SkPaint skPaint;
    skPaint.setAntiAlias(true);
    skPaint.setColor(m_fillColor);
    if (m_pFont->GetFace() && m_pCache) {  // exclude placeholder test fonts
      sk_sp<SkTypeface> typeface(SkSafeRef(m_pCache->GetDeviceCache(m_pFont)));
      skPaint.setTypeface(typeface);
    }
    skPaint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
    skPaint.setTextSize(m_fontSize);
    skPaint.setSubpixelText(true);
    skPaint.setHinting(SkPaint::kNo_Hinting);
    SkCanvas* skCanvas = pDriver->SkiaCanvas();
    skCanvas->save();
    skCanvas->concat(skMatrix);
    skCanvas->drawPosText(m_glyphs.begin(), m_glyphs.count() * 2,
                          m_positions.begin(), skPaint);
    skCanvas->restore();
    m_drawText = false;
  }

  bool SetClipFill(const CFX_PathData* pPathData,
                   const CFX_Matrix* pMatrix,
                   int fill_mode,
                   CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    SkPath skClipPath = BuildPath(pPathData);
    skClipPath.setFillType((fill_mode & 3) == FXFILL_ALTERNATE
                               ? SkPath::kEvenOdd_FillType
                               : SkPath::kWinding_FillType);
    SkMatrix skMatrix = ToSkMatrix(*pMatrix);
    skClipPath.transform(skMatrix);
    return SetClip(skClipPath, pDriver);
  }

  bool SetClip(const SkPath& skClipPath, CFX_SkiaDeviceDriver* pDriver) {
    if (m_commandIndex < m_commands.count()) {
      if (m_commands[m_commandIndex] == Clip::kPath &&
          m_clips[m_commandIndex] == skClipPath) {
        ++m_commandIndex;
        return true;
      }
      FlushCommands(pDriver);
    }
    Flush(pDriver);
    m_commands.push(Clip::kPath);
    ++m_commandIndex;
    m_clips.push_back(skClipPath);
    return false;
  }

  bool SetClipStroke(const CFX_PathData* pPathData,
                     const CFX_Matrix* pMatrix,
                     const CFX_GraphStateData* pGraphState,
                     CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    SkPath skPath = BuildPath(pPathData);
    SkMatrix skMatrix = ToSkMatrix(*pMatrix);
    SkPaint skPaint;
    pDriver->PaintStroke(&skPaint, pGraphState, skMatrix);
    SkPath dst_path;
    skPaint.getFillPath(skPath, &dst_path);
    dst_path.transform(skMatrix);
    return SetClip(dst_path, pDriver);
  }

  bool MatrixOffset(const CFX_Matrix* pMatrix, SkPoint* delta) {
    CFX_Matrix identityMatrix;
    if (!pMatrix)
      pMatrix = &identityMatrix;
    delta->set(pMatrix->e - m_drawMatrix.e, pMatrix->f - m_drawMatrix.f);
    if (!delta->fX && !delta->fY)
      return true;
    SkMatrix drawMatrix = ToSkMatrix(m_drawMatrix);
    if (!(drawMatrix.getType() & ~SkMatrix::kTranslate_Mask))
      return true;
    SkMatrix invDrawMatrix;
    if (!drawMatrix.invert(&invDrawMatrix))
      return false;
    SkMatrix invNewMatrix;
    SkMatrix newMatrix = ToSkMatrix(*pMatrix);
    if (!newMatrix.invert(&invNewMatrix))
      return false;
    delta->set(invDrawMatrix.getTranslateX() - invNewMatrix.getTranslateX(),
               invDrawMatrix.getTranslateY() - invNewMatrix.getTranslateY());
    return true;
  }

  void FlushCommands(CFX_SkiaDeviceDriver* pDriver) {
    if (m_commandIndex == m_commands.count())
      return;
    if (m_commandIndex < m_commands.count())
      pDriver->SkiaCanvas()->restore();
    int index = m_commands.count() - 1;
    if (m_commandIndex == index && m_commands[index] == Clip::kSave)
      return;
    for (; index > m_commandIndex; --index) {
      if (m_commands[index] == Clip::kSave)
        pDriver->SkiaCanvas()->restore();
    }

    if (m_commandIndex > 0)
      pDriver->SkiaCanvas()->save();
    while (index > 0 && m_commands[index] != Clip::kSave)
      --index;
    while (++index < m_commandIndex) {
      SkASSERT(m_commands[index] == Clip::kPath);
      pDriver->SkiaCanvas()->clipPath(m_clips[index], SkRegion::kIntersect_Op,
                                      true);
    }
    m_commands.setCount(m_commandIndex);
    m_clips.resize_back(m_commandIndex);
  }

  // returns true if caller should apply command to skia canvas
  bool ClipSave(CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    int count = m_commands.count();
    if (m_commandIndex < count) {
      if (m_commands[m_commandIndex] == Clip::kSave) {
        ++m_commandIndex;
        return true;
      }
      FlushCommands(pDriver);
    }
    Flush(pDriver);
    m_commands.push(Clip::kSave);
    ++m_commandIndex;
    m_clips.push_back(m_skEmptyPath);
    return false;
  }

  bool ClipRestore(CFX_SkiaDeviceDriver* pDriver) {
    if (m_debugDisable)
      return false;
    while (m_commandIndex > 0) {
      if (m_commands[--m_commandIndex] == Clip::kSave)
        return true;
    }
    Flush(pDriver);
    return false;
  }

  bool DrawChanged(const CFX_Matrix* pMatrix,
                   const CFX_GraphStateData* pState,
                   uint32_t fill_color,
                   uint32_t stroke_color,
                   int fill_mode,
                   int blend_type) {
    return MatrixChanged(pMatrix, m_drawMatrix) ||
           StateChanged(pState, m_drawState) || fill_color != m_fillColor ||
           stroke_color != m_strokeColor ||
           ((fill_mode & 3) == FXFILL_ALTERNATE) !=
               (m_skPath.getFillType() == SkPath::kEvenOdd_FillType) ||
           blend_type != m_blendType;
  }

  bool FontChanged(CFX_Font* pFont,
                   CFX_FontCache* pCache,
                   const CFX_Matrix* pMatrix,
                   FX_FLOAT font_size,
                   uint32_t color) {
    return pFont != m_pFont || pCache != m_pCache ||
           MatrixChanged(pMatrix, m_drawMatrix) || font_size != m_fontSize ||
           color != m_fillColor;
  }

  bool MatrixChanged(const CFX_Matrix* pMatrix, const CFX_Matrix& refMatrix) {
    CFX_Matrix identityMatrix;
    if (!pMatrix)
      pMatrix = &identityMatrix;
    return pMatrix->a != refMatrix.a || pMatrix->b != refMatrix.b ||
           pMatrix->c != refMatrix.c || pMatrix->d != refMatrix.d;
  }

  bool StateChanged(const CFX_GraphStateData* pState,
                    const CFX_GraphStateData& refState) {
    CFX_GraphStateData identityState;
    if (!pState)
      pState = &identityState;
    return pState->m_LineWidth != refState.m_LineWidth ||
           pState->m_LineCap != refState.m_LineCap ||
           pState->m_LineJoin != refState.m_LineJoin ||
           pState->m_MiterLimit != refState.m_MiterLimit ||
           DashChanged(pState, refState);
  }

  bool DashChanged(const CFX_GraphStateData* pState,
                   const CFX_GraphStateData& refState) {
    bool dashArray = pState && pState->m_DashArray;
    if (!dashArray && !refState.m_DashArray)
      return false;
    if (!dashArray || !refState.m_DashArray)
      return true;
    if (pState->m_DashPhase != refState.m_DashPhase ||
        pState->m_DashCount != refState.m_DashCount) {
      return true;
    }
    for (int index = 0; index < pState->m_DashCount; ++index) {
      if (pState->m_DashArray[index] != refState.m_DashArray[index])
        return false;
    }
    return true;
  }

  void Flush(CFX_SkiaDeviceDriver* pDriver) {
    if (m_drawPath)
      FlushPath(pDriver);
    if (m_drawText)
      FlushText(pDriver);
  }

#ifdef SK_DEBUG
  void Dump(const CFX_SkiaDeviceDriver* pDriver) const {
    SkDebugf("\n\nSkia Save Count %d:\n", pDriver->m_pCanvas->getSaveCount());
    pDriver->m_pCanvas->getClipStack()->dump();
    SkDebugf("Cache:\n");
    for (int index = 0; index < m_commands.count(); ++index) {
      SkDebugf("%s ", m_commandIndex == index ? "-->" : "   ");
      switch (m_commands[index]) {
        case Clip::kSave:
          SkDebugf("Save\n");
          break;
        case Clip::kPath:
          m_clips[index].dump();
          break;
        default:
          SkDebugf("unknown\n");
      }
    }
    if (m_commandIndex == m_commands.count())
      SkDebugf("-->\n");
  }
#endif

 private:
  SkTArray<SkPath> m_clips;        // stack of clips that may be reused
  SkTDArray<Clip> m_commands;      // stack of clip-related commands
  SkTDArray<SkPoint> m_positions;  // accumulator for text positions
  SkTDArray<uint16_t> m_glyphs;    // accumulator for text glyphs
  SkPath m_skPath;                 // accumulator for path contours
  SkPath m_skEmptyPath;            // used as placehold in the clips array
  CFX_Matrix m_drawMatrix;
  CFX_GraphStateData m_clipState;
  CFX_GraphStateData m_drawState;
  CFX_Matrix m_clipMatrix;
  CFX_Font* m_pFont;
  CFX_FontCache* m_pCache;
  FX_FLOAT m_fontSize;
  uint32_t m_fillColor;
  uint32_t m_strokeColor;
  int m_blendType;
  int m_commandIndex;  // active position in clip command stack
  bool m_drawText;
  bool m_drawPath;
  bool m_fillPath;
  bool m_debugDisable;  // turn off cache for debugging
};

// convert a stroking path to scanlines
void CFX_SkiaDeviceDriver::PaintStroke(SkPaint* spaint,
                                       const CFX_GraphStateData* pGraphState,
                                       const SkMatrix& matrix) {
  SkPaint::Cap cap;
  switch (pGraphState->m_LineCap) {
    case CFX_GraphStateData::LineCapRound:
      cap = SkPaint::kRound_Cap;
      break;
    case CFX_GraphStateData::LineCapSquare:
      cap = SkPaint::kSquare_Cap;
      break;
    default:
      cap = SkPaint::kButt_Cap;
      break;
  }
  SkPaint::Join join;
  switch (pGraphState->m_LineJoin) {
    case CFX_GraphStateData::LineJoinRound:
      join = SkPaint::kRound_Join;
      break;
    case CFX_GraphStateData::LineJoinBevel:
      join = SkPaint::kBevel_Join;
      break;
    default:
      join = SkPaint::kMiter_Join;
      break;
  }
  SkMatrix inverse;
  if (!matrix.invert(&inverse))
    return;  // give up if the matrix is degenerate, and not invertable
  inverse.set(SkMatrix::kMTransX, 0);
  inverse.set(SkMatrix::kMTransY, 0);
  SkVector deviceUnits[2] = {{0, 1}, {1, 0}};
  inverse.mapPoints(deviceUnits, SK_ARRAY_COUNT(deviceUnits));
  FX_FLOAT width =
      SkTMax(pGraphState->m_LineWidth,
             SkTMin(deviceUnits[0].length(), deviceUnits[1].length()));
  if (pGraphState->m_DashArray) {
    int count = (pGraphState->m_DashCount + 1) / 2;
    SkScalar* intervals = FX_Alloc2D(SkScalar, count, sizeof(SkScalar));
    // Set dash pattern
    for (int i = 0; i < count; i++) {
      FX_FLOAT on = pGraphState->m_DashArray[i * 2];
      if (on <= 0.000001f)
        on = 1.f / 10;
      FX_FLOAT off = i * 2 + 1 == pGraphState->m_DashCount
                         ? on
                         : pGraphState->m_DashArray[i * 2 + 1];
      if (off < 0)
        off = 0;
      intervals[i * 2] = on;
      intervals[i * 2 + 1] = off;
    }
    spaint->setPathEffect(
        SkDashPathEffect::Make(intervals, count * 2, pGraphState->m_DashPhase));
  }
  spaint->setStyle(SkPaint::kStroke_Style);
  spaint->setAntiAlias(true);
  spaint->setStrokeWidth(width);
  spaint->setStrokeMiter(pGraphState->m_MiterLimit);
  spaint->setStrokeCap(cap);
  spaint->setStrokeJoin(join);
}

CFX_SkiaDeviceDriver::CFX_SkiaDeviceDriver(CFX_DIBitmap* pBitmap,
                                           FX_BOOL bRgbByteOrder,
                                           CFX_DIBitmap* pOriDevice,
                                           FX_BOOL bGroupKnockout)
    : m_pBitmap(pBitmap),
      m_pOriDevice(pOriDevice),
      m_pRecorder(nullptr),
      m_pCache(new SkiaState),
      m_bGroupKnockout(bGroupKnockout) {
  SkBitmap skBitmap;
  SkASSERT(pBitmap->GetBPP() == 8 || pBitmap->GetBPP() == 32);
  SkImageInfo imageInfo = SkImageInfo::Make(
      pBitmap->GetWidth(), pBitmap->GetHeight(),
      pBitmap->GetBPP() == 8 ? kAlpha_8_SkColorType : kN32_SkColorType,
      kOpaque_SkAlphaType);
  skBitmap.installPixels(imageInfo, pBitmap->GetBuffer(), pBitmap->GetPitch(),
                         nullptr, /* to do : set color table */
                         nullptr, nullptr);
  m_pCanvas = new SkCanvas(skBitmap);
  if (m_bGroupKnockout)
    SkDebugf("");  // FIXME(caryclark) suppress 'm_bGroupKnockout is unused'
}

CFX_SkiaDeviceDriver::CFX_SkiaDeviceDriver(int size_x, int size_y)
    : m_pBitmap(nullptr),
      m_pOriDevice(nullptr),
      m_pRecorder(new SkPictureRecorder),
      m_pCache(new SkiaState),
      m_bGroupKnockout(FALSE) {
  m_pRecorder->beginRecording(SkIntToScalar(size_x), SkIntToScalar(size_y));
  m_pCanvas = m_pRecorder->getRecordingCanvas();
}

CFX_SkiaDeviceDriver::CFX_SkiaDeviceDriver(SkPictureRecorder* recorder)
    : m_pBitmap(nullptr),
      m_pOriDevice(nullptr),
      m_pRecorder(recorder),
      m_pCache(new SkiaState),
      m_bGroupKnockout(FALSE) {
  m_pCanvas = m_pRecorder->getRecordingCanvas();
}

CFX_SkiaDeviceDriver::~CFX_SkiaDeviceDriver() {
  Flush();
  if (!m_pRecorder)
    delete m_pCanvas;
}

void CFX_SkiaDeviceDriver::Flush() {
  m_pCache->Flush(this);
  m_pCache->FlushCommands(this);
}

FX_BOOL CFX_SkiaDeviceDriver::DrawDeviceText(int nChars,
                                             const FXTEXT_CHARPOS* pCharPos,
                                             CFX_Font* pFont,
                                             CFX_FontCache* pCache,
                                             const CFX_Matrix* pObject2Device,
                                             FX_FLOAT font_size,
                                             uint32_t color) {
  if (!pCache)
    pCache = CFX_GEModule::Get()->GetFontCache();
  if (m_pCache->DrawText(nChars, pCharPos, pFont, pCache, pObject2Device,
                         font_size, color, this)) {
    return TRUE;
  }
  sk_sp<SkTypeface> typeface(
      SkSafeRef(pCache ? pCache->GetDeviceCache(pFont) : nullptr));
  SkPaint paint;
  paint.setAntiAlias(true);
  paint.setColor(color);
  paint.setTypeface(typeface);
  paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
  paint.setHinting(SkPaint::kNo_Hinting);
  paint.setTextSize(font_size);
  paint.setSubpixelText(true);
  m_pCanvas->save();
  SkMatrix skMatrix = ToFlippedSkMatrix(*pObject2Device);
  m_pCanvas->concat(skMatrix);
  SkTDArray<SkPoint> positions;
  positions.setCount(nChars);
  SkTDArray<uint16_t> glyphs;
  glyphs.setCount(nChars);
  for (int index = 0; index < nChars; ++index) {
    const FXTEXT_CHARPOS& cp = pCharPos[index];
    positions[index] = {cp.m_OriginX, cp.m_OriginY};
    glyphs[index] = (uint16_t)cp.m_GlyphIndex;
  }
  m_pCanvas->drawPosText(glyphs.begin(), nChars * 2, positions.begin(), paint);
  m_pCanvas->restore();
  return TRUE;
}

int CFX_SkiaDeviceDriver::GetDeviceCaps(int caps_id) const {
  switch (caps_id) {
    case FXDC_DEVICE_CLASS:
      return FXDC_DISPLAY;
    case FXDC_PIXEL_WIDTH:
      return m_pCanvas->imageInfo().width();
    case FXDC_PIXEL_HEIGHT:
      return m_pCanvas->imageInfo().height();
    case FXDC_BITS_PIXEL:
      return 32;
    case FXDC_HORZ_SIZE:
    case FXDC_VERT_SIZE:
      return 0;
    case FXDC_RENDER_CAPS:
      return FXRC_GET_BITS | FXRC_ALPHA_PATH | FXRC_ALPHA_IMAGE |
             FXRC_BLEND_MODE | FXRC_SOFT_CLIP | FXRC_ALPHA_OUTPUT |
             FXRC_FILLSTROKE_PATH | FXRC_SHADING;
  }
  return 0;
}

void CFX_SkiaDeviceDriver::SaveState() {
  if (!m_pCache->ClipSave(this))
    m_pCanvas->save();
}

void CFX_SkiaDeviceDriver::RestoreState(bool bKeepSaved) {
  if (!m_pCache->ClipRestore(this))
    m_pCanvas->restore();
  if (bKeepSaved)
    SaveState();
}

FX_BOOL CFX_SkiaDeviceDriver::SetClip_PathFill(
    const CFX_PathData* pPathData,     // path info
    const CFX_Matrix* pObject2Device,  // flips object's y-axis
    int fill_mode                      // fill mode, WINDING or ALTERNATE
    ) {
  CFX_Matrix identity;
  const CFX_Matrix* deviceMatrix = pObject2Device ? pObject2Device : &identity;
  if (m_pCache->SetClipFill(pPathData, deviceMatrix, fill_mode, this))
    return TRUE;
  if (pPathData->GetPointCount() == 5 || pPathData->GetPointCount() == 4) {
    CFX_FloatRect rectf;
    if (pPathData->IsRect(deviceMatrix, &rectf)) {
      rectf.Intersect(
          CFX_FloatRect(0, 0, (FX_FLOAT)GetDeviceCaps(FXDC_PIXEL_WIDTH),
                        (FX_FLOAT)GetDeviceCaps(FXDC_PIXEL_HEIGHT)));
      // note that PDF's y-axis goes up; Skia's y-axis goes down
      SkRect skClipRect =
          SkRect::MakeLTRB(rectf.left, rectf.bottom, rectf.right, rectf.top);
      DebugDrawSkiaClipRect(m_pCanvas, skClipRect);
      m_pCanvas->clipRect(skClipRect, SkRegion::kIntersect_Op, true);
      return TRUE;
    }
  }
  SkPath skClipPath = BuildPath(pPathData);
  skClipPath.setFillType((fill_mode & 3) == FXFILL_ALTERNATE
                             ? SkPath::kEvenOdd_FillType
                             : SkPath::kWinding_FillType);
  SkMatrix skMatrix = ToSkMatrix(*deviceMatrix);
  skClipPath.transform(skMatrix);
  DebugShowSkiaPath(skClipPath);
  DebugDrawSkiaClipPath(m_pCanvas, skClipPath);
  m_pCanvas->clipPath(skClipPath, SkRegion::kIntersect_Op, true);

  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,         // path info
    const CFX_Matrix* pObject2Device,      // optional transformation
    const CFX_GraphStateData* pGraphState  // graphic state, for pen attributes
    ) {
  if (m_pCache->SetClipStroke(pPathData, pObject2Device, pGraphState, this))
    return TRUE;
  // build path data
  SkPath skPath = BuildPath(pPathData);
  SkMatrix skMatrix = ToSkMatrix(*pObject2Device);
  SkPaint skPaint;
  PaintStroke(&skPaint, pGraphState, skMatrix);
  SkPath dst_path;
  skPaint.getFillPath(skPath, &dst_path);
  dst_path.transform(skMatrix);
  DebugDrawSkiaClipPath(m_pCanvas, dst_path);
  m_pCanvas->clipPath(dst_path, SkRegion::kIntersect_Op, true);
  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::DrawPath(
    const CFX_PathData* pPathData,          // path info
    const CFX_Matrix* pObject2Device,       // optional transformation
    const CFX_GraphStateData* pGraphState,  // graphic state, for pen attributes
    uint32_t fill_color,                    // fill color
    uint32_t stroke_color,                  // stroke color
    int fill_mode,  // fill mode, WINDING or ALTERNATE. 0 for not filled
    int blend_type) {
  if (m_pCache->DrawPath(pPathData, pObject2Device, pGraphState, fill_color,
                         stroke_color, fill_mode, blend_type, this)) {
    return TRUE;
  }
  SkIRect rect;
  rect.set(0, 0, GetDeviceCaps(FXDC_PIXEL_WIDTH),
           GetDeviceCaps(FXDC_PIXEL_HEIGHT));
  SkMatrix skMatrix;
  if (pObject2Device)
    skMatrix = ToSkMatrix(*pObject2Device);
  else
    skMatrix.setIdentity();
  SkPaint skPaint;
  skPaint.setAntiAlias(true);
  int stroke_alpha = FXARGB_A(stroke_color);
  if (pGraphState && stroke_alpha)
    PaintStroke(&skPaint, pGraphState, skMatrix);
  SkPath skPath = BuildPath(pPathData);
  m_pCanvas->save();
  m_pCanvas->concat(skMatrix);
  if ((fill_mode & 3) && fill_color) {
    skPath.setFillType((fill_mode & 3) == FXFILL_ALTERNATE
                           ? SkPath::kEvenOdd_FillType
                           : SkPath::kWinding_FillType);
    SkPath strokePath;
    const SkPath* fillPath = &skPath;
    if (pGraphState && stroke_alpha) {
      SkAlpha fillA = SkColorGetA(fill_color);
      SkAlpha strokeA = SkColorGetA(stroke_color);
      if (fillA && fillA < 0xFF && strokeA && strokeA < 0xFF) {
        skPaint.getFillPath(skPath, &strokePath);
        if (Op(skPath, strokePath, SkPathOp::kDifference_SkPathOp,
               &strokePath)) {
          fillPath = &strokePath;
        }
      }
    }
    skPaint.setStyle(SkPaint::kFill_Style);
    skPaint.setColor(fill_color);
    m_pCanvas->drawPath(*fillPath, skPaint);
  }
  if (pGraphState && stroke_alpha) {
    DebugShowSkiaPath(skPath);
    DebugShowCanvasMatrix(m_pCanvas);
    skPaint.setStyle(SkPaint::kStroke_Style);
    skPaint.setColor(stroke_color);
    m_pCanvas->drawPath(skPath, skPaint);
  }
  m_pCanvas->restore();
  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::DrawCosmeticLine(FX_FLOAT x1,
                                               FX_FLOAT y1,
                                               FX_FLOAT x2,
                                               FX_FLOAT y2,
                                               uint32_t color,
                                               int blend_type) {
  return FALSE;
}

FX_BOOL CFX_SkiaDeviceDriver::FillRectWithBlend(const FX_RECT* pRect,
                                                uint32_t fill_color,
                                                int blend_type) {
  SkPaint spaint;
  spaint.setAntiAlias(true);
  spaint.setColor(fill_color);
  spaint.setXfermodeMode(GetSkiaBlendMode(blend_type));

  m_pCanvas->drawRect(
      SkRect::MakeLTRB(pRect->left, pRect->top, pRect->right, pRect->bottom),
      spaint);
  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::DrawShading(const CPDF_ShadingPattern* pPattern,
                                          const CFX_Matrix* pMatrix,
                                          const FX_RECT& clip_rect,
                                          int alpha,
                                          FX_BOOL bAlphaMode) {
  if (kAxialShading != pPattern->GetShadingType() &&
      kRadialShading != pPattern->GetShadingType()) {
    // TODO(caryclark) more types
    return false;
  }
  const std::vector<std::unique_ptr<CPDF_Function>>& pFuncs =
      pPattern->GetFuncs();
  int nFuncs = pFuncs.size();
  if (nFuncs != 1)  // TODO(caryclark) remove this restriction
    return false;
  CPDF_Dictionary* pDict = pPattern->GetShadingObject()->GetDict();
  CPDF_Array* pCoords = pDict->GetArrayBy("Coords");
  if (!pCoords)
    return true;
  // TODO(caryclark) Respect Domain[0], Domain[1]. (Don't know what they do
  // yet.)
  SkTDArray<SkColor> skColors;
  SkTDArray<SkScalar> skPos;
  for (int j = 0; j < nFuncs; j++) {
    if (!pFuncs[j])
      continue;

    if (const CPDF_SampledFunc* pSampledFunc = pFuncs[j]->ToSampledFunc()) {
      /* TODO(caryclark)
         Type 0 Sampled Functions in PostScript can also have an Order integer
         in the dictionary. PDFium doesn't appear to check for this anywhere.
       */
      if (!AddSamples(pSampledFunc, &skColors, &skPos))
        return false;
    } else if (const CPDF_ExpIntFunc* pExpIntFuc = pFuncs[j]->ToExpIntFunc()) {
      if (!AddColors(pExpIntFuc, &skColors))
        return false;
      skPos.push(0);
      skPos.push(1);
    } else if (const CPDF_StitchFunc* pStitchFunc = pFuncs[j]->ToStitchFunc()) {
      if (!AddStitching(pStitchFunc, &skColors, &skPos))
        return false;
    } else {
      return false;
    }
  }
  CPDF_Array* pArray = pDict->GetArrayBy("Extend");
  bool clipStart = !pArray || !pArray->GetIntegerAt(0);
  bool clipEnd = !pArray || !pArray->GetIntegerAt(1);
  SkPaint paint;
  paint.setAntiAlias(true);
  paint.setAlpha(alpha);
  SkMatrix skMatrix = ToSkMatrix(*pMatrix);
  SkRect skRect = SkRect::MakeLTRB(clip_rect.left, clip_rect.top,
                                   clip_rect.right, clip_rect.bottom);
  SkPath skClip;
  SkPath skPath;
  if (kAxialShading == pPattern->GetShadingType()) {
    FX_FLOAT start_x = pCoords->GetNumberAt(0);
    FX_FLOAT start_y = pCoords->GetNumberAt(1);
    FX_FLOAT end_x = pCoords->GetNumberAt(2);
    FX_FLOAT end_y = pCoords->GetNumberAt(3);
    SkPoint pts[] = {{start_x, start_y}, {end_x, end_y}};
    skMatrix.mapPoints(pts, SK_ARRAY_COUNT(pts));
    paint.setShader(SkGradientShader::MakeLinear(
        pts, skColors.begin(), skPos.begin(), skColors.count(),
        SkShader::kClamp_TileMode));
    if (clipStart || clipEnd) {
      // if the gradient is horizontal or vertical, modify the draw rectangle
      if (pts[0].fX == pts[1].fX) {  // vertical
        if (pts[0].fY > pts[1].fY) {
          SkTSwap(pts[0].fY, pts[1].fY);
          SkTSwap(clipStart, clipEnd);
        }
        if (clipStart)
          skRect.fTop = SkTMax(skRect.fTop, pts[0].fY);
        if (clipEnd)
          skRect.fBottom = SkTMin(skRect.fBottom, pts[1].fY);
      } else if (pts[0].fY == pts[1].fY) {  // horizontal
        if (pts[0].fX > pts[1].fX) {
          SkTSwap(pts[0].fX, pts[1].fX);
          SkTSwap(clipStart, clipEnd);
        }
        if (clipStart)
          skRect.fLeft = SkTMax(skRect.fLeft, pts[0].fX);
        if (clipEnd)
          skRect.fRight = SkTMin(skRect.fRight, pts[1].fX);
      } else {  // if the gradient is angled and contained by the rect, clip
        SkPoint rectPts[4] = {{skRect.fLeft, skRect.fTop},
                              {skRect.fRight, skRect.fTop},
                              {skRect.fRight, skRect.fBottom},
                              {skRect.fLeft, skRect.fBottom}};
        ClipAngledGradient(pts, rectPts, clipStart, clipEnd, &skClip);
      }
    }
    skPath.addRect(skRect);
    skMatrix.setIdentity();
  } else {
    ASSERT(kRadialShading == pPattern->GetShadingType());
    FX_FLOAT start_x = pCoords->GetNumberAt(0);
    FX_FLOAT start_y = pCoords->GetNumberAt(1);
    FX_FLOAT start_r = pCoords->GetNumberAt(2);
    FX_FLOAT end_x = pCoords->GetNumberAt(3);
    FX_FLOAT end_y = pCoords->GetNumberAt(4);
    FX_FLOAT end_r = pCoords->GetNumberAt(5);
    SkPoint pts[] = {{start_x, start_y}, {end_x, end_y}};

    paint.setShader(SkGradientShader::MakeTwoPointConical(
        pts[0], start_r, pts[1], end_r, skColors.begin(), skPos.begin(),
        skColors.count(), SkShader::kClamp_TileMode));
    if (clipStart || clipEnd) {
      if (clipStart && start_r)
        skClip.addCircle(pts[0].fX, pts[0].fY, start_r);
      if (clipEnd)
        skClip.addCircle(pts[1].fX, pts[1].fY, end_r, SkPath::kCCW_Direction);
      else
        skClip.setFillType(SkPath::kInverseWinding_FillType);
      skClip.transform(skMatrix);
    }
    SkMatrix inverse;
    if (!skMatrix.invert(&inverse))
      return false;
    skPath.addRect(skRect);
    skPath.transform(inverse);
  }
  m_pCanvas->save();
  if (!skClip.isEmpty())
    m_pCanvas->clipPath(skClip, SkRegion::kIntersect_Op, true);
  m_pCanvas->concat(skMatrix);
  m_pCanvas->drawPath(skPath, paint);
  m_pCanvas->restore();
  return true;
}

uint8_t* CFX_SkiaDeviceDriver::GetBuffer() const {
  return m_pBitmap->GetBuffer();
}

FX_BOOL CFX_SkiaDeviceDriver::GetClipBox(FX_RECT* pRect) {
  // TODO(caryclark) call m_canvas->getClipDeviceBounds() instead
  pRect->left = 0;
  pRect->top = 0;
  const SkImageInfo& canvasSize = m_pCanvas->imageInfo();
  pRect->right = canvasSize.width();
  pRect->bottom = canvasSize.height();
  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::GetDIBits(CFX_DIBitmap* pBitmap,
                                        int left,
                                        int top) {
  if (!m_pBitmap)
    return TRUE;
  uint8_t* srcBuffer = m_pBitmap->GetBuffer();
  if (!srcBuffer)
    return TRUE;
  int srcWidth = m_pBitmap->GetWidth();
  int srcHeight = m_pBitmap->GetHeight();
  int srcRowBytes = srcWidth * sizeof(uint32_t);
  SkImageInfo srcImageInfo = SkImageInfo::Make(
      srcWidth, srcHeight, SkColorType::kN32_SkColorType, kPremul_SkAlphaType);
  SkBitmap skSrcBitmap;
  skSrcBitmap.installPixels(srcImageInfo, srcBuffer, srcRowBytes, nullptr,
                            nullptr, nullptr);
  SkASSERT(pBitmap);
  uint8_t* dstBuffer = pBitmap->GetBuffer();
  SkASSERT(dstBuffer);
  int dstWidth = pBitmap->GetWidth();
  int dstHeight = pBitmap->GetHeight();
  int dstRowBytes = dstWidth * sizeof(uint32_t);
  SkImageInfo dstImageInfo = SkImageInfo::Make(
      dstWidth, dstHeight, SkColorType::kN32_SkColorType, kPremul_SkAlphaType);
  SkBitmap skDstBitmap;
  skDstBitmap.installPixels(dstImageInfo, dstBuffer, dstRowBytes, nullptr,
                            nullptr, nullptr);
  SkCanvas canvas(skDstBitmap);
  canvas.drawBitmap(skSrcBitmap, left, top, nullptr);
  return TRUE;
}

CFX_DIBitmap* CFX_SkiaDeviceDriver::GetBackDrop() {
  return m_pOriDevice;
}

FX_BOOL CFX_SkiaDeviceDriver::SetDIBits(const CFX_DIBSource* pBitmap,
                                        uint32_t argb,
                                        const FX_RECT* pSrcRect,
                                        int left,
                                        int top,
                                        int blend_type) {
  if (!m_pBitmap || !m_pBitmap->GetBuffer())
    return TRUE;

  CFX_Matrix m(pBitmap->GetWidth(), 0, 0, -pBitmap->GetHeight(), left,
               top + pBitmap->GetHeight());
  void* dummy;
  return StartDIBits(pBitmap, 0xFF, argb, &m, 0, dummy, blend_type);
}

FX_BOOL CFX_SkiaDeviceDriver::StretchDIBits(const CFX_DIBSource* pSource,
                                            uint32_t argb,
                                            int dest_left,
                                            int dest_top,
                                            int dest_width,
                                            int dest_height,
                                            const FX_RECT* pClipRect,
                                            uint32_t flags,
                                            int blend_type) {
  if (!m_pBitmap->GetBuffer())
    return TRUE;
  CFX_Matrix m(dest_width, 0, 0, -dest_height, dest_left,
               dest_top + dest_height);

  m_pCanvas->save();
  SkRect skClipRect = SkRect::MakeLTRB(pClipRect->left, pClipRect->bottom,
                                       pClipRect->right, pClipRect->top);
  m_pCanvas->clipRect(skClipRect, SkRegion::kIntersect_Op, true);
  void* dummy;
  FX_BOOL result = StartDIBits(pSource, 0xFF, argb, &m, 0, dummy, blend_type);
  m_pCanvas->restore();

  return result;
}

FX_BOOL CFX_SkiaDeviceDriver::StartDIBits(const CFX_DIBSource* pSource,
                                          int bitmap_alpha,
                                          uint32_t argb,
                                          const CFX_Matrix* pMatrix,
                                          uint32_t render_flags,
                                          void*& handle,
                                          int blend_type) {
  DebugValidate(m_pBitmap, m_pOriDevice);
  SkColorType colorType = pSource->IsAlphaMask()
                              ? SkColorType::kAlpha_8_SkColorType
                              : SkColorType::kGray_8_SkColorType;
  SkAlphaType alphaType =
      pSource->IsAlphaMask() ? kPremul_SkAlphaType : kOpaque_SkAlphaType;
  SkColorTable* ct = nullptr;
  void* buffer = pSource->GetBuffer();
  if (!buffer)
    return FALSE;
  std::unique_ptr<uint8_t, FxFreeDeleter> dst8Storage;
  std::unique_ptr<uint32_t, FxFreeDeleter> dst32Storage;
  int width = pSource->GetWidth();
  int height = pSource->GetHeight();
  int rowBytes = pSource->GetPitch();
  switch (pSource->GetBPP()) {
    case 1: {
      dst8Storage.reset(FX_Alloc2D(uint8_t, width, height));
      uint8_t* dst8Pixels = dst8Storage.get();
      for (int y = 0; y < height; ++y) {
        const uint8_t* srcRow =
            static_cast<const uint8_t*>(buffer) + y * rowBytes;
        uint8_t* dstRow = dst8Pixels + y * width;
        for (int x = 0; x < width; ++x)
          dstRow[x] = srcRow[x >> 3] & (1 << (~x & 0x07)) ? 0xFF : 0x00;
      }
      buffer = dst8Storage.get();
      rowBytes = width;
    } break;
    case 8:
      if (pSource->GetPalette()) {
        ct = new SkColorTable(pSource->GetPalette(), pSource->GetPaletteSize());
        colorType = SkColorType::kIndex_8_SkColorType;
      }
      break;
    case 24: {
      dst32Storage.reset(FX_Alloc2D(uint32_t, width, height));
      uint32_t* dst32Pixels = dst32Storage.get();
      for (int y = 0; y < height; ++y) {
        const uint8_t* srcRow =
            static_cast<const uint8_t*>(buffer) + y * rowBytes;
        uint32_t* dstRow = dst32Pixels + y * width;
        for (int x = 0; x < width; ++x) {
          dstRow[x] = SkPackARGB32(0xFF, srcRow[x * 3 + 2], srcRow[x * 3 + 1],
                                   srcRow[x * 3 + 0]);
        }
      }
      buffer = dst32Storage.get();
      rowBytes = width * sizeof(uint32_t);
      colorType = SkColorType::kN32_SkColorType;
      alphaType = kOpaque_SkAlphaType;
    } break;
    case 32:
      colorType = SkColorType::kN32_SkColorType;
      alphaType = kPremul_SkAlphaType;
      pSource->DebugVerifyBitmapIsPreMultiplied(buffer);
      break;
    default:
      SkASSERT(0);  // TODO(caryclark) ensure that all cases are covered
      colorType = SkColorType::kUnknown_SkColorType;
  }
  SkImageInfo imageInfo =
      SkImageInfo::Make(width, height, colorType, alphaType);
  SkBitmap skBitmap;
  skBitmap.installPixels(imageInfo, buffer, rowBytes, ct, nullptr, nullptr);
  m_pCanvas->save();
  SkMatrix skMatrix;
  const CFX_Matrix& m = *pMatrix;
  skMatrix.setAll(m.a / width, -m.c / height, m.c + m.e, m.b / width,
                  -m.d / height, m.d + m.f, 0, 0, 1);
  m_pCanvas->concat(skMatrix);
  SkPaint paint;
  paint.setAntiAlias(true);
  if (pSource->IsAlphaMask()) {
    paint.setColorFilter(
        SkColorFilter::MakeModeFilter(argb, SkXfermode::kSrc_Mode));
  }
  // paint.setFilterQuality(kHigh_SkFilterQuality);
  paint.setXfermodeMode(GetSkiaBlendMode(blend_type));
  paint.setAlpha(bitmap_alpha);
  m_pCanvas->drawBitmap(skBitmap, 0, 0, &paint);
  m_pCanvas->restore();
  if (ct)
    ct->unref();
  DebugValidate(m_pBitmap, m_pOriDevice);
  return TRUE;
}

FX_BOOL CFX_SkiaDeviceDriver::ContinueDIBits(void* handle, IFX_Pause* pPause) {
  return FALSE;
}

void CFX_SkiaDeviceDriver::PreMultiply(CFX_DIBitmap* pDIBitmap) {
  void* buffer = pDIBitmap->GetBuffer();
  if (!buffer)
    return;
  if (pDIBitmap->GetBPP() != 32) {
    return;
  }
  int height = pDIBitmap->GetHeight();
  int width = pDIBitmap->GetWidth();
  int rowBytes = pDIBitmap->GetPitch();
  SkImageInfo unpremultipliedInfo =
      SkImageInfo::Make(width, height, kN32_SkColorType, kUnpremul_SkAlphaType);
  SkPixmap unpremultiplied(unpremultipliedInfo, buffer, rowBytes);
  SkImageInfo premultipliedInfo =
      SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType);
  SkPixmap premultiplied(premultipliedInfo, buffer, rowBytes);
  unpremultiplied.readPixels(premultiplied);
  pDIBitmap->DebugVerifyBitmapIsPreMultiplied();
}

void CFX_SkiaDeviceDriver::Dump() const {
#ifdef SK_DEBUG
  if (m_pCache)
    m_pCache->Dump(this);
#endif
}

void CFX_SkiaDeviceDriver::DebugVerifyBitmapIsPreMultiplied() const {
  if (m_pOriDevice)
    m_pOriDevice->DebugVerifyBitmapIsPreMultiplied();
}

CFX_FxgeDevice::CFX_FxgeDevice() {
  m_bOwnedBitmap = FALSE;
}

SkPictureRecorder* CFX_FxgeDevice::CreateRecorder(int size_x, int size_y) {
  CFX_SkiaDeviceDriver* skDriver = new CFX_SkiaDeviceDriver(size_x, size_y);
  SetDeviceDriver(skDriver);
  return skDriver->GetRecorder();
}

bool CFX_FxgeDevice::Attach(CFX_DIBitmap* pBitmap,
                            bool bRgbByteOrder,
                            CFX_DIBitmap* pOriDevice,
                            bool bGroupKnockout) {
  if (!pBitmap)
    return false;
  SetBitmap(pBitmap);
  SetDeviceDriver(new CFX_SkiaDeviceDriver(pBitmap, bRgbByteOrder, pOriDevice,
                                           bGroupKnockout));
  return true;
}

bool CFX_FxgeDevice::AttachRecorder(SkPictureRecorder* recorder) {
  if (!recorder)
    return false;
  SetDeviceDriver(new CFX_SkiaDeviceDriver(recorder));
  return true;
}

bool CFX_FxgeDevice::Create(int width,
                            int height,
                            FXDIB_Format format,
                            CFX_DIBitmap* pOriDevice) {
  m_bOwnedBitmap = TRUE;
  CFX_DIBitmap* pBitmap = new CFX_DIBitmap;
  if (!pBitmap->Create(width, height, format)) {
    delete pBitmap;
    return false;
  }
  SetBitmap(pBitmap);
  CFX_SkiaDeviceDriver* pDriver =
      new CFX_SkiaDeviceDriver(pBitmap, FALSE, pOriDevice, FALSE);
  SetDeviceDriver(pDriver);
  return true;
}

CFX_FxgeDevice::~CFX_FxgeDevice() {
  Flush();
  // call destructor of CFX_RenderDevice / CFX_SkiaDeviceDriver immediately
  if (m_bOwnedBitmap && GetBitmap())
    delete GetBitmap();
}

void CFX_FxgeDevice::DebugVerifyBitmapIsPreMultiplied() const {
#ifdef SK_DEBUG
  CFX_SkiaDeviceDriver* skDriver =
      static_cast<CFX_SkiaDeviceDriver*>(GetDeviceDriver());
  if (skDriver)
    skDriver->DebugVerifyBitmapIsPreMultiplied();
#endif
}

void CFX_DIBSource::DebugVerifyBitmapIsPreMultiplied(void* opt) const {
#ifdef SK_DEBUG
  SkASSERT(32 == GetBPP());
  const uint32_t* buffer = (const uint32_t*)(opt ? opt : GetBuffer());
  int width = GetWidth();
  int height = GetHeight();
  // verify that input is really premultiplied
  for (int y = 0; y < height; ++y) {
    const uint32_t* srcRow = buffer + y * width;
    for (int x = 0; x < width; ++x) {
      uint8_t a = SkGetPackedA32(srcRow[x]);
      uint8_t r = SkGetPackedR32(srcRow[x]);
      uint8_t g = SkGetPackedG32(srcRow[x]);
      uint8_t b = SkGetPackedB32(srcRow[x]);
      SkA32Assert(a);
      SkASSERT(r <= a);
      SkASSERT(g <= a);
      SkASSERT(b <= a);
    }
  }
#endif
}

#endif
