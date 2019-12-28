// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_RENDERCONTEXT_H_
#define FPDFSDK_INCLUDE_FSDK_RENDERCONTEXT_H_

#include "core/fpdfapi/fpdf_render/include/cpdf_progressiverenderer.h"
#include "core/fxcrt/include/fx_basic.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "public/fpdf_progressive.h"

// Everything about rendering is put here: for OOM recovery
class CRenderContext : public CFX_Deletable {
 public:
  CRenderContext() { Clear(); }
  ~CRenderContext() override;

  void Clear();

  CFX_RenderDevice* m_pDevice;
  CPDF_RenderContext* m_pContext;
  CPDF_ProgressiveRenderer* m_pRenderer;
  CPDF_AnnotList* m_pAnnots;
  CPDF_RenderOptions* m_pOptions;
};

class IFSDK_PAUSE_Adapter : public IFX_Pause {
 public:
  explicit IFSDK_PAUSE_Adapter(IFSDK_PAUSE* IPause);
  ~IFSDK_PAUSE_Adapter() override;

  FX_BOOL NeedToPauseNow() override;

 private:
  IFSDK_PAUSE* m_IPause;
};

#endif  // FPDFSDK_INCLUDE_FSDK_RENDERCONTEXT_H_
