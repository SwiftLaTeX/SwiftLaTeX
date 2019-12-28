// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_RENDER_INCLUDE_CPDF_TEXTRENDERER_H_
#define CORE_FPDFAPI_FPDF_RENDER_INCLUDE_CPDF_TEXTRENDERER_H_

#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"
#include "core/fxge/include/fx_dib.h"

class CFX_RenderDevice;
class CFX_GraphStateData;
class CFX_PathData;
class CPDF_RenderOptions;
class CPDF_Font;

class CPDF_TextRenderer {
 public:
  static void DrawTextString(CFX_RenderDevice* pDevice,
                             FX_FLOAT origin_x,
                             FX_FLOAT origin_y,
                             CPDF_Font* pFont,
                             FX_FLOAT font_size,
                             const CFX_Matrix* matrix,
                             const CFX_ByteString& str,
                             FX_ARGB fill_argb,
                             FX_ARGB stroke_argb,
                             const CFX_GraphStateData* pGraphState,
                             const CPDF_RenderOptions* pOptions);

  static FX_BOOL DrawTextPath(CFX_RenderDevice* pDevice,
                              int nChars,
                              uint32_t* pCharCodes,
                              FX_FLOAT* pCharPos,
                              CPDF_Font* pFont,
                              FX_FLOAT font_size,
                              const CFX_Matrix* pText2User,
                              const CFX_Matrix* pUser2Device,
                              const CFX_GraphStateData* pGraphState,
                              FX_ARGB fill_argb,
                              FX_ARGB stroke_argb,
                              CFX_PathData* pClippingPath,
                              int nFlag);

  static FX_BOOL DrawNormalText(CFX_RenderDevice* pDevice,
                                int nChars,
                                uint32_t* pCharCodes,
                                FX_FLOAT* pCharPos,
                                CPDF_Font* pFont,
                                FX_FLOAT font_size,
                                const CFX_Matrix* pText2Device,
                                FX_ARGB fill_argb,
                                const CPDF_RenderOptions* pOptions);
};

#endif  // CORE_FPDFAPI_FPDF_RENDER_INCLUDE_CPDF_TEXTRENDERER_H_
