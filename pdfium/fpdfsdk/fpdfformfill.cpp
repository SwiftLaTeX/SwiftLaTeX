// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_formfill.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fxge/include/fx_ge.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "public/fpdfview.h"
#include "third_party/base/stl_util.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_app.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_page.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffpageview.h"
#include "xfa/fxfa/include/xfa_ffwidget.h"
#endif  // PDF_ENABLE_XFA

namespace {

CPDFSDK_InterForm* FormHandleToInterForm(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  return pSDKDoc ? pSDKDoc->GetInterForm() : nullptr;
}

CPDFSDK_PageView* FormHandleToPageView(FPDF_FORMHANDLE hHandle,
                                       FPDF_PAGE page) {
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  if (!pPage)
    return nullptr;

  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  return pSDKDoc ? pSDKDoc->GetPageView(pPage, TRUE) : nullptr;
}

#ifdef PDF_ENABLE_XFA
std::vector<CFX_ByteString>* FromFPDFStringHandle(FPDF_STRINGHANDLE handle) {
  return reinterpret_cast<std::vector<CFX_ByteString>*>(handle);
}

FPDF_STRINGHANDLE ToFPDFStringHandle(std::vector<CFX_ByteString>* strings) {
  return reinterpret_cast<FPDF_STRINGHANDLE>(strings);
}
#endif  // PDF_ENABLE_XFA

void FFLCommon(FPDF_FORMHANDLE hHandle,
               FPDF_BITMAP bitmap,
               FPDF_RECORDER recorder,
               FPDF_PAGE page,
               int start_x,
               int start_y,
               int size_x,
               int size_y,
               int rotate,
               int flags) {
  if (!hHandle)
    return;

  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  if (!pPage)
    return;

#ifndef PDF_ENABLE_XFA
  CPDF_RenderOptions options;
  if (flags & FPDF_LCD_TEXT)
    options.m_Flags |= RENDER_CLEARTYPE;
  else
    options.m_Flags &= ~RENDER_CLEARTYPE;
  // Grayscale output
  if (flags & FPDF_GRAYSCALE) {
    options.m_ColorMode = RENDER_COLOR_GRAY;
    options.m_ForeColor = 0;
    options.m_BackColor = 0xffffff;
  }
  options.m_AddFlags = flags >> 8;
  options.m_pOCContext =
      new CPDF_OCContext(pPage->m_pDocument, CPDF_OCContext::View);
#else   // PDF_ENABLE_XFA
  CPDFXFA_Document* pDocument = pPage->GetDocument();
  if (!pDocument)
    return;
  CPDF_Document* pPDFDoc = pDocument->GetPDFDoc();
  if (!pPDFDoc)
    return;
  CPDFDoc_Environment* pEnv = (CPDFDoc_Environment*)hHandle;
  CPDFSDK_Document* pFXDoc = pEnv->GetSDKDocument();
  if (!pFXDoc)
    return;
#endif  // PDF_ENABLE_XFA

  CFX_Matrix matrix;
  pPage->GetDisplayMatrix(matrix, start_x, start_y, size_x, size_y, rotate);

  FX_RECT clip(start_x, start_y, start_x + size_x, start_y + size_y);

  std::unique_ptr<CFX_FxgeDevice> pDevice(new CFX_FxgeDevice);
#ifdef _SKIA_SUPPORT_
  pDevice->AttachRecorder(static_cast<SkPictureRecorder*>(recorder));
#endif
  pDevice->Attach(CFXBitmapFromFPDFBitmap(bitmap), false, nullptr, false);
  pDevice->SaveState();
  pDevice->SetClip_Rect(clip);

#ifndef PDF_ENABLE_XFA
  if (CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, pPage))
    pPageView->PageView_OnDraw(pDevice.get(), &matrix, &options);
#else   // PDF_ENABLE_XFA
  CPDF_RenderOptions options;
  if (flags & FPDF_LCD_TEXT)
    options.m_Flags |= RENDER_CLEARTYPE;
  else
    options.m_Flags &= ~RENDER_CLEARTYPE;

  // Grayscale output
  if (flags & FPDF_GRAYSCALE) {
    options.m_ColorMode = RENDER_COLOR_GRAY;
    options.m_ForeColor = 0;
    options.m_BackColor = 0xffffff;
  }
  options.m_AddFlags = flags >> 8;
  options.m_pOCContext = new CPDF_OCContext(pPDFDoc, CPDF_OCContext::View);

  if (CPDFSDK_PageView* pPageView = pFXDoc->GetPageView(pPage))
    pPageView->PageView_OnDraw(pDevice.get(), &matrix, &options, clip);
#endif  // PDF_ENABLE_XFA

  pDevice->RestoreState(false);
  delete options.m_pOCContext;
#ifdef PDF_ENABLE_XFA
  options.m_pOCContext = nullptr;
#endif  // PDF_ENABLE_XFA
}

}  // namespace

DLLEXPORT int STDCALL FPDFPage_HasFormFieldAtPoint(FPDF_FORMHANDLE hHandle,
                                                   FPDF_PAGE page,
                                                   double page_x,
                                                   double page_y) {
  if (!hHandle)
    return -1;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (pPage) {
    CPDF_InterForm interform(pPage->m_pDocument);
    CPDF_FormControl* pFormCtrl =
        interform.GetControlAtPoint(pPage, static_cast<FX_FLOAT>(page_x),
                                    static_cast<FX_FLOAT>(page_y), nullptr);
    if (!pFormCtrl)
      return -1;
    CPDF_FormField* pFormField = pFormCtrl->GetField();
    return pFormField ? pFormField->GetFieldType() : -1;
  }

#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* pXFAPage = UnderlyingFromFPDFPage(page);
  if (!pXFAPage)
    return -1;

  CXFA_FFPageView* pPageView = pXFAPage->GetXFAPageView();
  if (!pPageView)
    return -1;

  CXFA_FFDocView* pDocView = pPageView->GetDocView();
  if (!pDocView)
    return -1;

  CXFA_FFWidgetHandler* pWidgetHandler = pDocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return -1;

  std::unique_ptr<IXFA_WidgetIterator> pWidgetIterator(
      pPageView->CreateWidgetIterator(XFA_TRAVERSEWAY_Form,
                                      XFA_WidgetStatus_Viewable));
  if (!pWidgetIterator)
    return -1;

  CXFA_FFWidget* pXFAAnnot = pWidgetIterator->MoveToNext();
  while (pXFAAnnot) {
    CFX_RectF rcBBox;
    pXFAAnnot->GetBBox(rcBBox, 0);
    CFX_FloatRect rcWidget(rcBBox.left, rcBBox.top, rcBBox.left + rcBBox.width,
                           rcBBox.top + rcBBox.height);
    rcWidget.left -= 1.0f;
    rcWidget.right += 1.0f;
    rcWidget.bottom -= 1.0f;
    rcWidget.top += 1.0f;

    if (rcWidget.Contains(static_cast<FX_FLOAT>(page_x),
                          static_cast<FX_FLOAT>(page_y))) {
      return FPDF_FORMFIELD_XFA;
    }
    pXFAAnnot = pWidgetIterator->MoveToNext();
  }
#endif  // PDF_ENABLE_XFA
  return -1;
}

DLLEXPORT int STDCALL FPDPage_HasFormFieldAtPoint(FPDF_FORMHANDLE hHandle,
                                                  FPDF_PAGE page,
                                                  double page_x,
                                                  double page_y) {
  return FPDFPage_HasFormFieldAtPoint(hHandle, page, page_x, page_y);
}

DLLEXPORT int STDCALL FPDFPage_FormFieldZOrderAtPoint(FPDF_FORMHANDLE hHandle,
                                                      FPDF_PAGE page,
                                                      double page_x,
                                                      double page_y) {
  if (!hHandle)
    return -1;
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return -1;
  CPDF_InterForm interform(pPage->m_pDocument);
  int z_order = -1;
  (void)interform.GetControlAtPoint(pPage, (FX_FLOAT)page_x, (FX_FLOAT)page_y,
                                    &z_order);
  return z_order;
}

DLLEXPORT FPDF_FORMHANDLE STDCALL
FPDFDOC_InitFormFillEnvironment(FPDF_DOCUMENT document,
                                FPDF_FORMFILLINFO* formInfo) {
#ifdef PDF_ENABLE_XFA
  const int kRequiredVersion = 2;
#else   // PDF_ENABLE_XFA
  const int kRequiredVersion = 1;
#endif  // PDF_ENABLE_XFA
  if (!formInfo || formInfo->version != kRequiredVersion)
    return nullptr;

  UnderlyingDocumentType* pDocument = UnderlyingFromFPDFDocument(document);
  if (!pDocument)
    return nullptr;

  CPDFDoc_Environment* pEnv = new CPDFDoc_Environment(pDocument, formInfo);
#ifdef PDF_ENABLE_XFA
  pEnv->SetSDKDocument(pDocument->GetSDKDocument(pEnv));
  CPDFXFA_App* pApp = CPDFXFA_App::GetInstance();
  pApp->AddFormFillEnv(pEnv);
#else  // PDF_ENABLE_XFA
  pEnv->SetSDKDocument(new CPDFSDK_Document(pDocument, pEnv));
#endif  // PDF_ENABLE_XFA
  return pEnv;
}

DLLEXPORT void STDCALL
FPDFDOC_ExitFormFillEnvironment(FPDF_FORMHANDLE hHandle) {
  if (!hHandle)
    return;
  CPDFDoc_Environment* pEnv = (CPDFDoc_Environment*)hHandle;
#ifdef PDF_ENABLE_XFA
  CPDFXFA_App* pApp = CPDFXFA_App::GetInstance();
  pApp->RemoveFormFillEnv(pEnv);
#else   // PDF_ENABLE_XFA
  if (CPDFSDK_Document* pSDKDoc = pEnv->GetSDKDocument()) {
    pEnv->SetSDKDocument(nullptr);
    delete pSDKDoc;
  }
#endif  // PDF_ENABLE_XFA
  delete pEnv;
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnMouseMove(FPDF_FORMHANDLE hHandle,
                                             FPDF_PAGE page,
                                             int modifier,
                                             double page_x,
                                             double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  CFX_FloatPoint pt((FX_FLOAT)page_x, (FX_FLOAT)page_y);
  return pPageView->OnMouseMove(pt, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnLButtonDown(FPDF_FORMHANDLE hHandle,
                                               FPDF_PAGE page,
                                               int modifier,
                                               double page_x,
                                               double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  CFX_FloatPoint pt((FX_FLOAT)page_x, (FX_FLOAT)page_y);
  return pPageView->OnLButtonDown(pt, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnLButtonUp(FPDF_FORMHANDLE hHandle,
                                             FPDF_PAGE page,
                                             int modifier,
                                             double page_x,
                                             double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  CFX_FloatPoint pt((FX_FLOAT)page_x, (FX_FLOAT)page_y);
  return pPageView->OnLButtonUp(pt, modifier);
}

#ifdef PDF_ENABLE_XFA
DLLEXPORT FPDF_BOOL STDCALL FORM_OnRButtonDown(FPDF_FORMHANDLE hHandle,
                                               FPDF_PAGE page,
                                               int modifier,
                                               double page_x,
                                               double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  CFX_FloatPoint pt((FX_FLOAT)page_x, (FX_FLOAT)page_y);
  return pPageView->OnRButtonDown(pt, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnRButtonUp(FPDF_FORMHANDLE hHandle,
                                             FPDF_PAGE page,
                                             int modifier,
                                             double page_x,
                                             double page_y) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  CFX_FloatPoint pt((FX_FLOAT)page_x, (FX_FLOAT)page_y);
  return pPageView->OnRButtonUp(pt, modifier);
}
#endif  // PDF_ENABLE_XFA

DLLEXPORT FPDF_BOOL STDCALL FORM_OnKeyDown(FPDF_FORMHANDLE hHandle,
                                           FPDF_PAGE page,
                                           int nKeyCode,
                                           int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  return pPageView->OnKeyDown(nKeyCode, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnKeyUp(FPDF_FORMHANDLE hHandle,
                                         FPDF_PAGE page,
                                         int nKeyCode,
                                         int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  return pPageView->OnKeyUp(nKeyCode, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_OnChar(FPDF_FORMHANDLE hHandle,
                                        FPDF_PAGE page,
                                        int nChar,
                                        int modifier) {
  CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page);
  if (!pPageView)
    return FALSE;

  return pPageView->OnChar(nChar, modifier);
}

DLLEXPORT FPDF_BOOL STDCALL FORM_ForceToKillFocus(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  if (!pSDKDoc)
    return FALSE;

  return pSDKDoc->KillFocusAnnot(0);
}

DLLEXPORT void STDCALL FPDF_FFLDraw(FPDF_FORMHANDLE hHandle,
                                    FPDF_BITMAP bitmap,
                                    FPDF_PAGE page,
                                    int start_x,
                                    int start_y,
                                    int size_x,
                                    int size_y,
                                    int rotate,
                                    int flags) {
  FFLCommon(hHandle, bitmap, nullptr, page, start_x, start_y, size_x, size_y,
            rotate, flags);
}

#ifdef _SKIA_SUPPORT_
DLLEXPORT void STDCALL FPDF_FFLRecord(FPDF_FORMHANDLE hHandle,
                                      FPDF_RECORDER recorder,
                                      FPDF_PAGE page,
                                      int start_x,
                                      int start_y,
                                      int size_x,
                                      int size_y,
                                      int rotate,
                                      int flags) {
  FFLCommon(hHandle, nullptr, recorder, page, start_x, start_y, size_x, size_y,
            rotate, flags);
}
#endif

#ifdef PDF_ENABLE_XFA
DLLEXPORT void STDCALL FPDF_Widget_Undo(FPDF_DOCUMENT document,
                                        FPDF_WIDGET hWidget) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  static_cast<CXFA_FFWidget*>(hWidget)->Undo();
}
DLLEXPORT void STDCALL FPDF_Widget_Redo(FPDF_DOCUMENT document,
                                        FPDF_WIDGET hWidget) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  static_cast<CXFA_FFWidget*>(hWidget)->Redo();
}

DLLEXPORT void STDCALL FPDF_Widget_SelectAll(FPDF_DOCUMENT document,
                                             FPDF_WIDGET hWidget) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  static_cast<CXFA_FFWidget*>(hWidget)->SelectAll();
}
DLLEXPORT void STDCALL FPDF_Widget_Copy(FPDF_DOCUMENT document,
                                        FPDF_WIDGET hWidget,
                                        FPDF_WIDESTRING wsText,
                                        FPDF_DWORD* size) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  CFX_WideString wsCpText;
  static_cast<CXFA_FFWidget*>(hWidget)->Copy(wsCpText);

  CFX_ByteString bsCpText = wsCpText.UTF16LE_Encode();
  uint32_t len = bsCpText.GetLength() / sizeof(unsigned short);
  if (!wsText) {
    *size = len;
    return;
  }

  uint32_t real_size = len < *size ? len : *size;
  if (real_size > 0) {
    FXSYS_memcpy((void*)wsText,
                 bsCpText.GetBuffer(real_size * sizeof(unsigned short)),
                 real_size * sizeof(unsigned short));
    bsCpText.ReleaseBuffer(real_size * sizeof(unsigned short));
  }
  *size = real_size;
}

DLLEXPORT void STDCALL FPDF_Widget_Cut(FPDF_DOCUMENT document,
                                       FPDF_WIDGET hWidget,
                                       FPDF_WIDESTRING wsText,
                                       FPDF_DWORD* size) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  CFX_WideString wsCpText;
  static_cast<CXFA_FFWidget*>(hWidget)->Cut(wsCpText);

  CFX_ByteString bsCpText = wsCpText.UTF16LE_Encode();
  uint32_t len = bsCpText.GetLength() / sizeof(unsigned short);
  if (!wsText) {
    *size = len;
    return;
  }

  uint32_t real_size = len < *size ? len : *size;
  if (real_size > 0) {
    FXSYS_memcpy((void*)wsText,
                 bsCpText.GetBuffer(real_size * sizeof(unsigned short)),
                 real_size * sizeof(unsigned short));
    bsCpText.ReleaseBuffer(real_size * sizeof(unsigned short));
  }
  *size = real_size;
}

DLLEXPORT void STDCALL FPDF_Widget_Paste(FPDF_DOCUMENT document,
                                         FPDF_WIDGET hWidget,
                                         FPDF_WIDESTRING wsText,
                                         FPDF_DWORD size) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  CFX_WideString wstr = CFX_WideString::FromUTF16LE(wsText, size);
  static_cast<CXFA_FFWidget*>(hWidget)->Paste(wstr);
}

DLLEXPORT void STDCALL
FPDF_Widget_ReplaceSpellCheckWord(FPDF_DOCUMENT document,
                                  FPDF_WIDGET hWidget,
                                  float x,
                                  float y,
                                  FPDF_BYTESTRING bsText) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  CFX_PointF ptPopup;
  ptPopup.x = x;
  ptPopup.y = y;
  CFX_ByteStringC bs(bsText);
  static_cast<CXFA_FFWidget*>(hWidget)->ReplaceSpellCheckWord(ptPopup, bs);
}

DLLEXPORT void STDCALL
FPDF_Widget_GetSpellCheckWords(FPDF_DOCUMENT document,
                               FPDF_WIDGET hWidget,
                               float x,
                               float y,
                               FPDF_STRINGHANDLE* stringHandle) {
  if (!hWidget || !document)
    return;

  CPDFXFA_Document* pDocument = (CPDFXFA_Document*)document;
  if (pDocument->GetDocType() != XFA_DOCTYPE_Dynamic &&
      pDocument->GetDocType() != XFA_DOCTYPE_Static)
    return;

  std::vector<CFX_ByteString>* sSuggestWords = new std::vector<CFX_ByteString>;
  CFX_PointF ptPopup;
  ptPopup.x = x;
  ptPopup.y = y;
  static_cast<CXFA_FFWidget*>(hWidget)
      ->GetSuggestWords(ptPopup, *sSuggestWords);
  *stringHandle = ToFPDFStringHandle(sSuggestWords);
}

DLLEXPORT int STDCALL FPDF_StringHandleCounts(FPDF_STRINGHANDLE sHandle) {
  std::vector<CFX_ByteString>* sSuggestWords = FromFPDFStringHandle(sHandle);
  return sSuggestWords ? pdfium::CollectionSize<int>(*sSuggestWords) : -1;
}

DLLEXPORT FPDF_BOOL STDCALL
FPDF_StringHandleGetStringByIndex(FPDF_STRINGHANDLE sHandle,
                                  int index,
                                  FPDF_BYTESTRING bsText,
                                  FPDF_DWORD* size) {
  if (!sHandle || !size)
    return FALSE;

  int count = FPDF_StringHandleCounts(sHandle);
  if (index < 0 || index >= count)
    return FALSE;

  std::vector<CFX_ByteString>* sSuggestWords = FromFPDFStringHandle(sHandle);
  uint32_t len = (*sSuggestWords)[index].GetLength();
  if (!bsText) {
    *size = len;
    return TRUE;
  }

  uint32_t real_size = len < *size ? len : *size;
  if (real_size > 0)
    FXSYS_memcpy((void*)bsText, (*sSuggestWords)[index].c_str(), real_size);
  *size = real_size;
  return TRUE;
}

DLLEXPORT void STDCALL
FPDF_StringHandleRelease(FPDF_STRINGHANDLE stringHandle) {
  delete FromFPDFStringHandle(stringHandle);
}

DLLEXPORT FPDF_BOOL STDCALL
FPDF_StringHandleAddString(FPDF_STRINGHANDLE stringHandle,
                           FPDF_BYTESTRING bsText,
                           FPDF_DWORD size) {
  if (!stringHandle || !bsText || size == 0)
    return FALSE;

  FromFPDFStringHandle(stringHandle)->push_back(CFX_ByteString(bsText, size));
  return TRUE;
}
#endif  // PDF_ENABLE_XFA

DLLEXPORT void STDCALL FPDF_SetFormFieldHighlightColor(FPDF_FORMHANDLE hHandle,
                                                       int fieldType,
                                                       unsigned long color) {
  if (CPDFSDK_InterForm* pInterForm = FormHandleToInterForm(hHandle))
    pInterForm->SetHighlightColor(color, fieldType);
}

DLLEXPORT void STDCALL FPDF_SetFormFieldHighlightAlpha(FPDF_FORMHANDLE hHandle,
                                                       unsigned char alpha) {
  if (CPDFSDK_InterForm* pInterForm = FormHandleToInterForm(hHandle))
    pInterForm->SetHighlightAlpha(alpha);
}

DLLEXPORT void STDCALL FPDF_RemoveFormFieldHighlight(FPDF_FORMHANDLE hHandle) {
  if (CPDFSDK_InterForm* pInterForm = FormHandleToInterForm(hHandle))
    pInterForm->RemoveAllHighLight();
}

DLLEXPORT void STDCALL FORM_OnAfterLoadPage(FPDF_PAGE page,
                                            FPDF_FORMHANDLE hHandle) {
  if (CPDFSDK_PageView* pPageView = FormHandleToPageView(hHandle, page))
    pPageView->SetValid(TRUE);
}

DLLEXPORT void STDCALL FORM_OnBeforeClosePage(FPDF_PAGE page,
                                              FPDF_FORMHANDLE hHandle) {
  if (!hHandle)
    return;

  CPDFSDK_Document* pSDKDoc = ((CPDFDoc_Environment*)hHandle)->GetSDKDocument();
  if (!pSDKDoc)
    return;

  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  if (!pPage)
    return;

  CPDFSDK_PageView* pPageView = pSDKDoc->GetPageView(pPage, FALSE);
  if (pPageView) {
    pPageView->SetValid(FALSE);
    // RemovePageView() takes care of the delete for us.
    pSDKDoc->RemovePageView(pPage);
  }
}

DLLEXPORT void STDCALL FORM_DoDocumentJSAction(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  if (pSDKDoc && ((CPDFDoc_Environment*)hHandle)->IsJSInitiated())
    pSDKDoc->ProcJavascriptFun();
}

DLLEXPORT void STDCALL FORM_DoDocumentOpenAction(FPDF_FORMHANDLE hHandle) {
  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  if (pSDKDoc && ((CPDFDoc_Environment*)hHandle)->IsJSInitiated())
    pSDKDoc->ProcOpenAction();
}

DLLEXPORT void STDCALL FORM_DoDocumentAAction(FPDF_FORMHANDLE hHandle,
                                              int aaType) {
  CPDFSDK_Document* pSDKDoc = CPDFSDK_Document::FromFPDFFormHandle(hHandle);
  if (!pSDKDoc)
    return;

  CPDF_Document* pDoc = pSDKDoc->GetPDFDocument();
  CPDF_Dictionary* pDic = pDoc->GetRoot();
  if (!pDic)
    return;

  CPDF_AAction aa(pDic->GetDictBy("AA"));
  if (aa.ActionExist((CPDF_AAction::AActionType)aaType)) {
    CPDF_Action action = aa.GetAction((CPDF_AAction::AActionType)aaType);
    CPDFSDK_ActionHandler* pActionHandler =
        ((CPDFDoc_Environment*)hHandle)->GetActionHander();
    pActionHandler->DoAction_Document(action, (CPDF_AAction::AActionType)aaType,
                                      pSDKDoc);
  }
}

DLLEXPORT void STDCALL FORM_DoPageAAction(FPDF_PAGE page,
                                          FPDF_FORMHANDLE hHandle,
                                          int aaType) {
  if (!hHandle)
    return;
  CPDFSDK_Document* pSDKDoc = ((CPDFDoc_Environment*)hHandle)->GetSDKDocument();
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  CPDF_Page* pPDFPage = CPDFPageFromFPDFPage(page);
  if (!pPDFPage)
    return;
  if (pSDKDoc->GetPageView(pPage, FALSE)) {
    CPDFDoc_Environment* pEnv = pSDKDoc->GetEnv();
    CPDFSDK_ActionHandler* pActionHandler = pEnv->GetActionHander();
    CPDF_Dictionary* pPageDict = pPDFPage->m_pFormDict;
    CPDF_AAction aa(pPageDict->GetDictBy("AA"));
    if (FPDFPAGE_AACTION_OPEN == aaType) {
      if (aa.ActionExist(CPDF_AAction::OpenPage)) {
        CPDF_Action action = aa.GetAction(CPDF_AAction::OpenPage);
        pActionHandler->DoAction_Page(action, CPDF_AAction::OpenPage, pSDKDoc);
      }
    } else {
      if (aa.ActionExist(CPDF_AAction::ClosePage)) {
        CPDF_Action action = aa.GetAction(CPDF_AAction::ClosePage);
        pActionHandler->DoAction_Page(action, CPDF_AAction::ClosePage, pSDKDoc);
      }
    }
  }
}
