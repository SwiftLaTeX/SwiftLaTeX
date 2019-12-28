// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PAGE_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PAGE_H_

#include <memory>

#include "core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_coordinates.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Dictionary;
class CPDF_Document;
class CPDF_Object;
class CPDF_PageRenderCache;

class CPDF_Page : public CPDF_PageObjectHolder {
 public:
  class View {};  // Caller implements as desired, empty here due to layering.

  CPDF_Page(CPDF_Document* pDocument,
            CPDF_Dictionary* pPageDict,
            bool bPageCache);
  ~CPDF_Page() override;

  void ParseContent();

  void GetDisplayMatrix(CFX_Matrix& matrix,
                        int xPos,
                        int yPos,
                        int xSize,
                        int ySize,
                        int iRotate) const;

  FX_FLOAT GetPageWidth() const { return m_PageWidth; }
  FX_FLOAT GetPageHeight() const { return m_PageHeight; }
  CFX_FloatRect GetPageBBox() const { return m_BBox; }
  const CFX_Matrix& GetPageMatrix() const { return m_PageMatrix; }
  CPDF_Object* GetPageAttr(const CFX_ByteString& name) const;
  CPDF_PageRenderCache* GetRenderCache() const { return m_pPageRender.get(); }

  CFX_Deletable* GetRenderContext() const { return m_pRenderContext.get(); }
  void SetRenderContext(std::unique_ptr<CFX_Deletable> pContext) {
    m_pRenderContext = std::move(pContext);
  }

  View* GetView() const { return m_pView; }
  void SetView(View* pView) { m_pView = pView; }

 protected:
  friend class CPDF_ContentParser;

  void StartParse();

  FX_FLOAT m_PageWidth;
  FX_FLOAT m_PageHeight;
  CFX_Matrix m_PageMatrix;
  View* m_pView;
  std::unique_ptr<CPDF_PageRenderCache> m_pPageRender;
  std::unique_ptr<CFX_Deletable> m_pRenderContext;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_PAGE_H_
