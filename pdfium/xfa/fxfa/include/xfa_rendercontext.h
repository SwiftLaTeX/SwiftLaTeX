// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_RENDERCONTEXT_H_
#define XFA_FXFA_INCLUDE_XFA_RENDERCONTEXT_H_

#include <memory>

#include "xfa/fxfa/include/fxfa.h"

class CXFA_RenderOptions {
 public:
  CXFA_RenderOptions() : m_bPrint(FALSE), m_bHighlight(TRUE) {}

  FX_BOOL m_bPrint;
  FX_BOOL m_bHighlight;
};

class CXFA_RenderContext {
 public:
  CXFA_RenderContext();
  ~CXFA_RenderContext();

  int32_t StartRender(CXFA_FFPageView* pPageView,
                      CFX_Graphics* pGS,
                      const CFX_Matrix& matrix,
                      const CXFA_RenderOptions& options);
  int32_t DoRender(IFX_Pause* pPause = nullptr);
  void StopRender();

 protected:
  std::unique_ptr<IXFA_WidgetIterator> m_pWidgetIterator;
  CXFA_FFWidget* m_pWidget;
  CXFA_FFPageView* m_pPageView;
  CFX_Graphics* m_pGS;
  CFX_Matrix m_matrix;
  CXFA_RenderOptions m_options;
  uint32_t m_dwStatus;
  CFX_RectF m_rtClipRect;
};

#endif  // XFA_FXFA_INCLUDE_XFA_RENDERCONTEXT_H_
