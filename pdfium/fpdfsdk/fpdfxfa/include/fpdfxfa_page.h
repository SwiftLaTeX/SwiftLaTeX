// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_PAGE_H_
#define FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_PAGE_H_

#include <memory>

#include "core/fxcrt/include/fx_system.h"

class CFX_Matrix;
class CPDFXFA_Document;
class CPDF_Dictionary;
class CPDF_Page;
class CXFA_FFPageView;

class CPDFXFA_Page {
 public:
  CPDFXFA_Page(CPDFXFA_Document* pDoc, int page_index);

  void Retain() { m_iRef++; }
  void Release() {
    if (--m_iRef <= 0)
      delete this;
  }

  FX_BOOL LoadPage();
  FX_BOOL LoadPDFPage(CPDF_Dictionary* pageDict);
  CPDFXFA_Document* GetDocument() const { return m_pDocument; }
  int GetPageIndex() const { return m_iPageIndex; }
  CPDF_Page* GetPDFPage() const { return m_pPDFPage.get(); }
  CXFA_FFPageView* GetXFAPageView() const { return m_pXFAPageView; }

  void SetXFAPageView(CXFA_FFPageView* pPageView) {
    m_pXFAPageView = pPageView;
  }

  FX_FLOAT GetPageWidth() const;
  FX_FLOAT GetPageHeight() const;

  void DeviceToPage(int start_x,
                    int start_y,
                    int size_x,
                    int size_y,
                    int rotate,
                    int device_x,
                    int device_y,
                    double* page_x,
                    double* page_y);
  void PageToDevice(int start_x,
                    int start_y,
                    int size_x,
                    int size_y,
                    int rotate,
                    double page_x,
                    double page_y,
                    int* device_x,
                    int* device_y);

  void GetDisplayMatrix(CFX_Matrix& matrix,
                        int xPos,
                        int yPos,
                        int xSize,
                        int ySize,
                        int iRotate) const;

 protected:
  // Refcounted class.
  ~CPDFXFA_Page();

  FX_BOOL LoadPDFPage();
  FX_BOOL LoadXFAPageView();

 private:
  std::unique_ptr<CPDF_Page> m_pPDFPage;
  CXFA_FFPageView* m_pXFAPageView;
  CPDFXFA_Document* const m_pDocument;
  const int m_iPageIndex;
  int m_iRef;
};

#endif  // FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_PAGE_H_
