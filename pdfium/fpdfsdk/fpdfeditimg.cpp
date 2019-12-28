// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_edit.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_image.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_imageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "fpdfsdk/include/fsdk_define.h"

DLLEXPORT FPDF_PAGEOBJECT STDCALL
FPDFPageObj_NewImgeObj(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_ImageObject* pImageObj = new CPDF_ImageObject;
  CPDF_Image* pImg = new CPDF_Image(pDoc);
  pImageObj->m_pImage = pImg;
  return pImageObj;
}

DLLEXPORT FPDF_BOOL STDCALL
FPDFImageObj_LoadJpegFile(FPDF_PAGE* pages,
                          int nCount,
                          FPDF_PAGEOBJECT image_object,
                          FPDF_FILEACCESS* fileAccess) {
  if (!image_object || !fileAccess || !pages)
    return FALSE;

  IFX_FileRead* pFile = new CPDF_CustomAccess(fileAccess);
  CPDF_ImageObject* pImgObj = (CPDF_ImageObject*)image_object;
  pImgObj->m_GeneralState.GetModify();
  for (int index = 0; index < nCount; index++) {
    CPDF_Page* pPage = CPDFPageFromFPDFPage(pages[index]);
    if (!pPage)
      continue;
    pImgObj->m_pImage->ResetCache(pPage, nullptr);
  }
  pImgObj->m_pImage->SetJpegImage(pFile);

  return TRUE;
}

DLLEXPORT FPDF_BOOL STDCALL FPDFImageObj_SetMatrix(FPDF_PAGEOBJECT image_object,
                                                   double a,
                                                   double b,
                                                   double c,
                                                   double d,
                                                   double e,
                                                   double f) {
  if (!image_object)
    return FALSE;
  CPDF_ImageObject* pImgObj = (CPDF_ImageObject*)image_object;
  pImgObj->m_Matrix.a = (FX_FLOAT)a;
  pImgObj->m_Matrix.b = (FX_FLOAT)b;
  pImgObj->m_Matrix.c = (FX_FLOAT)c;
  pImgObj->m_Matrix.d = (FX_FLOAT)d;
  pImgObj->m_Matrix.e = (FX_FLOAT)e;
  pImgObj->m_Matrix.f = (FX_FLOAT)f;
  pImgObj->CalcBoundingBox();
  return TRUE;
}

DLLEXPORT FPDF_BOOL STDCALL FPDFImageObj_SetBitmap(FPDF_PAGE* pages,
                                                   int nCount,
                                                   FPDF_PAGEOBJECT image_object,
                                                   FPDF_BITMAP bitmap) {
  if (!image_object || !bitmap || !pages)
    return FALSE;
  CFX_DIBitmap* pBmp = nullptr;
  pBmp = (CFX_DIBitmap*)bitmap;
  CPDF_ImageObject* pImgObj = (CPDF_ImageObject*)image_object;
  pImgObj->m_GeneralState.GetModify();
  for (int index = 0; index < nCount; index++) {
    CPDF_Page* pPage = CPDFPageFromFPDFPage(pages[index]);
    if (!pPage)
      continue;
    pImgObj->m_pImage->ResetCache(pPage, nullptr);
  }
  pImgObj->m_pImage->SetImage(pBmp, FALSE);
  pImgObj->CalcBoundingBox();
  return TRUE;
}
