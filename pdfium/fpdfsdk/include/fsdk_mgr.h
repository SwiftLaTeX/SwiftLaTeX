// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_MGR_H_
#define FPDFSDK_INCLUDE_FSDK_MGR_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "fpdfsdk/cfx_systemhandler.h"
#include "fpdfsdk/include/fsdk_actionhandler.h"
#include "fpdfsdk/include/fsdk_annothandler.h"
#include "fpdfsdk/include/fsdk_baseannot.h"
#include "fpdfsdk/include/fsdk_baseform.h"
#include "fpdfsdk/include/fsdk_common.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "public/fpdf_formfill.h"
#include "public/fpdf_fwlevent.h"

class CFFL_IFormFiller;
class CFX_SystemHandler;
class CPDFSDK_ActionHandler;
class CPDFSDK_Annot;
class CPDFSDK_InterForm;
class CPDFSDK_PageView;
class CPDFSDK_Widget;
class IJS_Runtime;

class CPDFDoc_Environment final {
 public:
  CPDFDoc_Environment(UnderlyingDocumentType* pDoc, FPDF_FORMFILLINFO* pFFinfo);
  ~CPDFDoc_Environment();

  void FFI_Invalidate(FPDF_PAGE page,
                      double left,
                      double top,
                      double right,
                      double bottom) {
    if (m_pInfo && m_pInfo->FFI_Invalidate)
      m_pInfo->FFI_Invalidate(m_pInfo, page, left, top, right, bottom);
  }

  void FFI_OutputSelectedRect(FPDF_PAGE page,
                              double left,
                              double top,
                              double right,
                              double bottom) {
    if (m_pInfo && m_pInfo->FFI_OutputSelectedRect)
      m_pInfo->FFI_OutputSelectedRect(m_pInfo, page, left, top, right, bottom);
  }

  void FFI_SetCursor(int nCursorType) {
    if (m_pInfo && m_pInfo->FFI_SetCursor)
      m_pInfo->FFI_SetCursor(m_pInfo, nCursorType);
  }

  int FFI_SetTimer(int uElapse, TimerCallback lpTimerFunc) {
    if (m_pInfo && m_pInfo->FFI_SetTimer)
      return m_pInfo->FFI_SetTimer(m_pInfo, uElapse, lpTimerFunc);
    return -1;
  }

  void FFI_KillTimer(int nTimerID) {
    if (m_pInfo && m_pInfo->FFI_KillTimer)
      m_pInfo->FFI_KillTimer(m_pInfo, nTimerID);
  }

  FX_SYSTEMTIME FFI_GetLocalTime() const {
    FX_SYSTEMTIME fxtime;
    if (m_pInfo && m_pInfo->FFI_GetLocalTime) {
      FPDF_SYSTEMTIME systime = m_pInfo->FFI_GetLocalTime(m_pInfo);
      fxtime.wDay = systime.wDay;
      fxtime.wDayOfWeek = systime.wDayOfWeek;
      fxtime.wHour = systime.wHour;
      fxtime.wMilliseconds = systime.wMilliseconds;
      fxtime.wMinute = systime.wMinute;
      fxtime.wMonth = systime.wMonth;
      fxtime.wSecond = systime.wSecond;
      fxtime.wYear = systime.wYear;
    }
    return fxtime;
  }

  void FFI_OnChange() {
    if (m_pInfo && m_pInfo->FFI_OnChange)
      m_pInfo->FFI_OnChange(m_pInfo);
  }

  FX_BOOL FFI_IsSHIFTKeyDown(uint32_t nFlag) const {
    return (nFlag & FWL_EVENTFLAG_ShiftKey) != 0;
  }

  FX_BOOL FFI_IsCTRLKeyDown(uint32_t nFlag) const {
    return (nFlag & FWL_EVENTFLAG_ControlKey) != 0;
  }

  FX_BOOL FFI_IsALTKeyDown(uint32_t nFlag) const {
    return (nFlag & FWL_EVENTFLAG_AltKey) != 0;
  }

  FPDF_PAGE FFI_GetPage(FPDF_DOCUMENT document, int nPageIndex) {
    if (m_pInfo && m_pInfo->FFI_GetPage)
      return m_pInfo->FFI_GetPage(m_pInfo, document, nPageIndex);
    return nullptr;
  }

  FPDF_PAGE FFI_GetCurrentPage(FPDF_DOCUMENT document) {
    if (m_pInfo && m_pInfo->FFI_GetCurrentPage)
      return m_pInfo->FFI_GetCurrentPage(m_pInfo, document);
    return nullptr;
  }

  int FFI_GetRotation(FPDF_PAGE page) {
    if (m_pInfo && m_pInfo->FFI_GetRotation)
      return m_pInfo->FFI_GetRotation(m_pInfo, page);
    return 0;
  }

  void FFI_ExecuteNamedAction(const FX_CHAR* namedAction) {
    if (m_pInfo && m_pInfo->FFI_ExecuteNamedAction)
      m_pInfo->FFI_ExecuteNamedAction(m_pInfo, namedAction);
  }

  void FFI_OnSetFieldInputFocus(void* field,
                                FPDF_WIDESTRING focusText,
                                FPDF_DWORD nTextLen,
                                FX_BOOL bFocus) {
    if (m_pInfo && m_pInfo->FFI_SetTextFieldFocus)
      m_pInfo->FFI_SetTextFieldFocus(m_pInfo, focusText, nTextLen, bFocus);
  }

  void FFI_DoURIAction(const FX_CHAR* bsURI) {
    if (m_pInfo && m_pInfo->FFI_DoURIAction)
      m_pInfo->FFI_DoURIAction(m_pInfo, bsURI);
  }

  void FFI_DoGoToAction(int nPageIndex,
                        int zoomMode,
                        float* fPosArray,
                        int sizeOfArray) {
    if (m_pInfo && m_pInfo->FFI_DoGoToAction)
      m_pInfo->FFI_DoGoToAction(m_pInfo, nPageIndex, zoomMode, fPosArray,
                                sizeOfArray);
  }

#ifdef PDF_ENABLE_XFA
  void FFI_DisplayCaret(FPDF_PAGE page,
                        FPDF_BOOL bVisible,
                        double left,
                        double top,
                        double right,
                        double bottom) {
    if (m_pInfo && m_pInfo->FFI_DisplayCaret)
      m_pInfo->FFI_DisplayCaret(m_pInfo, page, bVisible, left, top, right,
                                bottom);
  }

  int FFI_GetCurrentPageIndex(FPDF_DOCUMENT document) {
    if (!m_pInfo || !m_pInfo->FFI_GetCurrentPageIndex) {
      return -1;
    }
    return m_pInfo->FFI_GetCurrentPageIndex(m_pInfo, document);
  }

  void FFI_SetCurrentPage(FPDF_DOCUMENT document, int iCurPage) {
    if (m_pInfo && m_pInfo->FFI_SetCurrentPage)
      m_pInfo->FFI_SetCurrentPage(m_pInfo, document, iCurPage);
  }

  CFX_WideString FFI_GetAppName() const { return CFX_WideString(L"Acrobat"); }

  CFX_WideString FFI_GetPlatform() {
    if (m_pInfo && m_pInfo->FFI_GetPlatform) {
      int nRequiredLen = m_pInfo->FFI_GetPlatform(m_pInfo, nullptr, 0);
      if (nRequiredLen <= 0)
        return L"";

      char* pbuff = new char[nRequiredLen];
      memset(pbuff, 0, nRequiredLen);
      int nActualLen = m_pInfo->FFI_GetPlatform(m_pInfo, pbuff, nRequiredLen);
      if (nActualLen <= 0 || nActualLen > nRequiredLen) {
        delete[] pbuff;
        return L"";
      }
      CFX_ByteString bsRet = CFX_ByteString(pbuff, nActualLen);
      CFX_WideString wsRet = CFX_WideString::FromUTF16LE(
          (unsigned short*)bsRet.GetBuffer(bsRet.GetLength()),
          bsRet.GetLength() / sizeof(unsigned short));
      delete[] pbuff;
      return wsRet;
    }
    return L"";
  }

  void FFI_GotoURL(FPDF_DOCUMENT document,
                   const CFX_WideStringC& wsURL,
                   FX_BOOL bAppend) {
    if (m_pInfo && m_pInfo->FFI_GotoURL) {
      CFX_ByteString bsTo = CFX_WideString(wsURL).UTF16LE_Encode();
      FPDF_WIDESTRING pTo = (FPDF_WIDESTRING)bsTo.GetBuffer(wsURL.GetLength());
      m_pInfo->FFI_GotoURL(m_pInfo, document, pTo);
      bsTo.ReleaseBuffer();
    }
  }

  void FFI_GetURL(FPDF_DOCUMENT document, CFX_WideString& wsURL) {
    wsURL = CFX_WideString();
  }

  void FFI_GetPageViewRect(FPDF_PAGE page, FS_RECTF& dstRect) {
    if (m_pInfo && m_pInfo->FFI_GetPageViewRect) {
      double left;
      double top;
      double right;
      double bottom;
      m_pInfo->FFI_GetPageViewRect(m_pInfo, page, &left, &top, &right, &bottom);

      dstRect.left = static_cast<float>(left);
      dstRect.top = static_cast<float>(top < bottom ? bottom : top);
      dstRect.bottom = static_cast<float>(top < bottom ? top : bottom);
      dstRect.right = static_cast<float>(right);
    }
  }

  FX_BOOL FFI_PopupMenu(FPDF_PAGE page,
                        FPDF_WIDGET hWidget,
                        int menuFlag,
                        CFX_PointF ptPopup,
                        const CFX_PointF* pRectExclude) {
    if (m_pInfo && m_pInfo->FFI_PopupMenu)
      return m_pInfo->FFI_PopupMenu(m_pInfo, page, hWidget, menuFlag, ptPopup.x,
                                    ptPopup.y);
    return FALSE;
  }

  void FFI_Alert(FPDF_WIDESTRING Msg,
                 FPDF_WIDESTRING Title,
                 int Type,
                 int Icon) {
    if (m_pInfo && m_pInfo->m_pJsPlatform && m_pInfo->m_pJsPlatform->app_alert)
      m_pInfo->m_pJsPlatform->app_alert(m_pInfo->m_pJsPlatform, Msg, Title,
                                        Type, Icon);
  }

  void FFI_EmailTo(FPDF_FILEHANDLER* fileHandler,
                   FPDF_WIDESTRING pTo,
                   FPDF_WIDESTRING pSubject,
                   FPDF_WIDESTRING pCC,
                   FPDF_WIDESTRING pBcc,
                   FPDF_WIDESTRING pMsg) {
    if (m_pInfo && m_pInfo->FFI_EmailTo)
      m_pInfo->FFI_EmailTo(m_pInfo, fileHandler, pTo, pSubject, pCC, pBcc,
                           pMsg);
  }

  void FFI_UploadTo(FPDF_FILEHANDLER* fileHandler,
                    int fileFlag,
                    FPDF_WIDESTRING uploadTo) {
    if (m_pInfo && m_pInfo->FFI_UploadTo)
      m_pInfo->FFI_UploadTo(m_pInfo, fileHandler, fileFlag, uploadTo);
  }

  FPDF_FILEHANDLER* FFI_OpenFile(int fileType,
                                 FPDF_WIDESTRING wsURL,
                                 const char* mode) {
    if (m_pInfo && m_pInfo->FFI_OpenFile)
      return m_pInfo->FFI_OpenFile(m_pInfo, fileType, wsURL, mode);
    return nullptr;
  }

  CFX_WideString FFI_GetFilePath(FPDF_FILEHANDLER* pFileHandler) const {
    return L"";
  }

  int FFI_GetDocumentCount() const { return 0; }
  int FFI_GetCurDocument() const { return 0; }

  IFX_FileRead* FFI_DownloadFromURL(const FX_WCHAR* url) {
    if (m_pInfo && m_pInfo->FFI_DownloadFromURL) {
      CFX_ByteString bstrURL = CFX_WideString(url).UTF16LE_Encode();
      FPDF_WIDESTRING wsURL =
          (FPDF_WIDESTRING)bstrURL.GetBuffer(bstrURL.GetLength());

      FPDF_LPFILEHANDLER fileHandler =
          m_pInfo->FFI_DownloadFromURL(m_pInfo, wsURL);

      return new CFPDF_FileStream(fileHandler);
    }
    return nullptr;
  }

  CFX_WideString FFI_PostRequestURL(const FX_WCHAR* wsURL,
                                    const FX_WCHAR* wsData,
                                    const FX_WCHAR* wsContentType,
                                    const FX_WCHAR* wsEncode,
                                    const FX_WCHAR* wsHeader) {
    if (m_pInfo && m_pInfo->FFI_PostRequestURL) {
      CFX_ByteString bsURL = CFX_WideString(wsURL).UTF16LE_Encode();
      FPDF_WIDESTRING URL = (FPDF_WIDESTRING)bsURL.GetBuffer(bsURL.GetLength());

      CFX_ByteString bsData = CFX_WideString(wsData).UTF16LE_Encode();
      FPDF_WIDESTRING data =
          (FPDF_WIDESTRING)bsData.GetBuffer(bsData.GetLength());

      CFX_ByteString bsContentType =
          CFX_WideString(wsContentType).UTF16LE_Encode();
      FPDF_WIDESTRING contentType =
          (FPDF_WIDESTRING)bsContentType.GetBuffer(bsContentType.GetLength());

      CFX_ByteString bsEncode = CFX_WideString(wsEncode).UTF16LE_Encode();
      FPDF_WIDESTRING encode =
          (FPDF_WIDESTRING)bsEncode.GetBuffer(bsEncode.GetLength());

      CFX_ByteString bsHeader = CFX_WideString(wsHeader).UTF16LE_Encode();
      FPDF_WIDESTRING header =
          (FPDF_WIDESTRING)bsHeader.GetBuffer(bsHeader.GetLength());

      FPDF_BSTR response;
      FPDF_BStr_Init(&response);
      m_pInfo->FFI_PostRequestURL(m_pInfo, URL, data, contentType, encode,
                                  header, &response);

      CFX_WideString wsRet = CFX_WideString::FromUTF16LE(
          (unsigned short*)response.str, response.len / sizeof(unsigned short));
      FPDF_BStr_Clear(&response);

      return wsRet;
    }
    return L"";
  }

  FPDF_BOOL FFI_PutRequestURL(const FX_WCHAR* wsURL,
                              const FX_WCHAR* wsData,
                              const FX_WCHAR* wsEncode) {
    if (m_pInfo && m_pInfo->FFI_PutRequestURL) {
      CFX_ByteString bsURL = CFX_WideString(wsURL).UTF16LE_Encode();
      FPDF_WIDESTRING URL = (FPDF_WIDESTRING)bsURL.GetBuffer(bsURL.GetLength());

      CFX_ByteString bsData = CFX_WideString(wsData).UTF16LE_Encode();
      FPDF_WIDESTRING data =
          (FPDF_WIDESTRING)bsData.GetBuffer(bsData.GetLength());

      CFX_ByteString bsEncode = CFX_WideString(wsEncode).UTF16LE_Encode();
      FPDF_WIDESTRING encode =
          (FPDF_WIDESTRING)bsEncode.GetBuffer(bsEncode.GetLength());

      return m_pInfo->FFI_PutRequestURL(m_pInfo, URL, data, encode);
    }
    return FALSE;
  }

  CFX_WideString FFI_GetLanguage() {
    if (m_pInfo && m_pInfo->FFI_GetLanguage) {
      int nRequiredLen = m_pInfo->FFI_GetLanguage(m_pInfo, nullptr, 0);
      if (nRequiredLen <= 0)
        return L"";

      char* pbuff = new char[nRequiredLen];
      memset(pbuff, 0, nRequiredLen);
      int nActualLen = m_pInfo->FFI_GetLanguage(m_pInfo, pbuff, nRequiredLen);
      if (nActualLen <= 0 || nActualLen > nRequiredLen) {
        delete[] pbuff;
        return L"";
      }
      CFX_ByteString bsRet = CFX_ByteString(pbuff, nActualLen);
      CFX_WideString wsRet = CFX_WideString::FromUTF16LE(
          (unsigned short*)bsRet.GetBuffer(bsRet.GetLength()),
          bsRet.GetLength() / sizeof(unsigned short));
      delete[] pbuff;
      return wsRet;
    }
    return L"";
  }

  void FFI_PageEvent(int iPageCount, uint32_t dwEventType) const {
    if (m_pInfo && m_pInfo->FFI_PageEvent)
      m_pInfo->FFI_PageEvent(m_pInfo, iPageCount, dwEventType);
  }
#endif  // PDF_ENABLE_XFA

  int JS_appAlert(const FX_WCHAR* Msg,
                  const FX_WCHAR* Title,
                  FX_UINT Type,
                  FX_UINT Icon);
  int JS_appResponse(const FX_WCHAR* Question,
                     const FX_WCHAR* Title,
                     const FX_WCHAR* Default,
                     const FX_WCHAR* cLabel,
                     FPDF_BOOL bPassword,
                     void* response,
                     int length);
  void JS_appBeep(int nType);
  CFX_WideString JS_fieldBrowse();
  CFX_WideString JS_docGetFilePath();
  void JS_docSubmitForm(void* formData, int length, const FX_WCHAR* URL);
  void JS_docmailForm(void* mailData,
                      int length,
                      FPDF_BOOL bUI,
                      const FX_WCHAR* To,
                      const FX_WCHAR* Subject,
                      const FX_WCHAR* CC,
                      const FX_WCHAR* BCC,
                      const FX_WCHAR* Msg);
  void JS_docprint(FPDF_BOOL bUI,
                   int nStart,
                   int nEnd,
                   FPDF_BOOL bSilent,
                   FPDF_BOOL bShrinkToFit,
                   FPDF_BOOL bPrintAsImage,
                   FPDF_BOOL bReverse,
                   FPDF_BOOL bAnnotations);
  void JS_docgotoPage(int nPageNum);

  FX_BOOL IsJSInitiated() const { return m_pInfo && m_pInfo->m_pJsPlatform; }
  void SetSDKDocument(CPDFSDK_Document* pFXDoc) { m_pSDKDoc = pFXDoc; }
  CPDFSDK_Document* GetSDKDocument() const { return m_pSDKDoc; }
  UnderlyingDocumentType* GetUnderlyingDocument() const {
    return m_pUnderlyingDoc;
  }
  CFX_ByteString GetAppName() const { return ""; }
  CFX_SystemHandler* GetSysHandler() const { return m_pSysHandler.get(); }
  FPDF_FORMFILLINFO* GetFormFillInfo() const { return m_pInfo; }

  CFFL_IFormFiller* GetIFormFiller();             // Creates if not present.
  CPDFSDK_AnnotHandlerMgr* GetAnnotHandlerMgr();  // Creates if not present.
  IJS_Runtime* GetJSRuntime();                    // Creates if not present.
  CPDFSDK_ActionHandler* GetActionHander();       // Creates if not present.

 private:
  std::unique_ptr<CPDFSDK_AnnotHandlerMgr> m_pAnnotHandlerMgr;
  std::unique_ptr<CPDFSDK_ActionHandler> m_pActionHandler;
  std::unique_ptr<IJS_Runtime> m_pJSRuntime;
  FPDF_FORMFILLINFO* const m_pInfo;
  CPDFSDK_Document* m_pSDKDoc;
  UnderlyingDocumentType* const m_pUnderlyingDoc;
  std::unique_ptr<CFFL_IFormFiller> m_pIFormFiller;
  std::unique_ptr<CFX_SystemHandler> m_pSysHandler;
};

class CPDFSDK_Document {
 public:
  static CPDFSDK_Document* FromFPDFFormHandle(FPDF_FORMHANDLE hHandle);

  CPDFSDK_Document(UnderlyingDocumentType* pDoc, CPDFDoc_Environment* pEnv);
  ~CPDFSDK_Document();

  CPDFSDK_InterForm* GetInterForm();

  // Gets the document object for the next layer down; for master this is
  // a CPDF_Document, but for XFA it is a CPDFXFA_Document.
  UnderlyingDocumentType* GetUnderlyingDocument() const {
#ifdef PDF_ENABLE_XFA
    return GetXFADocument();
#else   // PDF_ENABLE_XFA
    return GetPDFDocument();
#endif  // PDF_ENABLE_XFA
  }

  // Gets the CPDF_Document, either directly in master, or from the
  // CPDFXFA_Document for XFA.
  CPDF_Document* GetPDFDocument() const {
#ifdef PDF_ENABLE_XFA
    return m_pDoc ? m_pDoc->GetPDFDoc() : nullptr;
#else   // PDF_ENABLE_XFA
    return m_pDoc;
#endif  // PDF_ENABLE_XFA
  }

#ifdef PDF_ENABLE_XFA
  // Gets the XFA document directly (XFA-only).
  CPDFXFA_Document* GetXFADocument() const { return m_pDoc; }

  int GetPageViewCount() const { return m_pageMap.size(); }
#endif  // PDF_ENABLE_XFA

  CPDFSDK_PageView* GetPageView(UnderlyingPageType* pPage,
                                FX_BOOL ReNew = TRUE);
  CPDFSDK_PageView* GetPageView(int nIndex);
  CPDFSDK_PageView* GetCurrentView();
  void RemovePageView(UnderlyingPageType* pPage);
  void UpdateAllViews(CPDFSDK_PageView* pSender, CPDFSDK_Annot* pAnnot);

  CPDFSDK_Annot* GetFocusAnnot();

  IJS_Runtime* GetJsRuntime();

  FX_BOOL SetFocusAnnot(CPDFSDK_Annot* pAnnot, FX_UINT nFlag = 0);
  FX_BOOL KillFocusAnnot(FX_UINT nFlag = 0);

  FX_BOOL ExtractPages(const std::vector<uint16_t>& arrExtraPages,
                       CPDF_Document* pDstDoc);
  FX_BOOL InsertPages(int nInsertAt,
                      const CPDF_Document* pSrcDoc,
                      const std::vector<uint16_t>& arrSrcPages);
  FX_BOOL ReplacePages(int nPage,
                       const CPDF_Document* pSrcDoc,
                       const std::vector<uint16_t>& arrSrcPages);

  void OnCloseDocument();

  int GetPageCount() { return m_pDoc->GetPageCount(); }
  FX_BOOL GetPermissions(int nFlag);
  FX_BOOL GetChangeMark() { return m_bChangeMask; }
  void SetChangeMark() { m_bChangeMask = TRUE; }
  void ClearChangeMark() { m_bChangeMask = FALSE; }
  CFX_WideString GetPath();
  UnderlyingPageType* GetPage(int nIndex);
  CPDFDoc_Environment* GetEnv() { return m_pEnv; }
  void ProcJavascriptFun();
  FX_BOOL ProcOpenAction();
  CPDF_OCContext* GetOCContext();

 private:
  std::map<UnderlyingPageType*, CPDFSDK_PageView*> m_pageMap;
  UnderlyingDocumentType* m_pDoc;
  std::unique_ptr<CPDFSDK_InterForm> m_pInterForm;
  CPDFSDK_Annot* m_pFocusAnnot;
  CPDFDoc_Environment* m_pEnv;
  std::unique_ptr<CPDF_OCContext> m_pOccontent;
  FX_BOOL m_bChangeMask;
  FX_BOOL m_bBeingDestroyed;
};

class CPDFSDK_PageView final : public CPDF_Page::View {
 public:
  CPDFSDK_PageView(CPDFSDK_Document* pSDKDoc, UnderlyingPageType* page);
  ~CPDFSDK_PageView();

#ifdef PDF_ENABLE_XFA
  void PageView_OnDraw(CFX_RenderDevice* pDevice,
                       CFX_Matrix* pUser2Device,
                       CPDF_RenderOptions* pOptions,
                       const FX_RECT& pClip);
#else   // PDF_ENABLE_XFA
  void PageView_OnDraw(CFX_RenderDevice* pDevice,
                       CFX_Matrix* pUser2Device,
                       CPDF_RenderOptions* pOptions);
#endif  // PDF_ENABLE_XFA

  const CPDF_Annot* GetPDFAnnotAtPoint(FX_FLOAT pageX, FX_FLOAT pageY);
  CPDFSDK_Annot* GetFXAnnotAtPoint(FX_FLOAT pageX, FX_FLOAT pageY);
  const CPDF_Annot* GetPDFWidgetAtPoint(FX_FLOAT pageX, FX_FLOAT pageY);
  CPDFSDK_Annot* GetFXWidgetAtPoint(FX_FLOAT pageX, FX_FLOAT pageY);
  CPDFSDK_Annot* GetFocusAnnot();
  void SetFocusAnnot(CPDFSDK_Annot* pSDKAnnot, FX_UINT nFlag = 0) {
    m_pSDKDoc->SetFocusAnnot(pSDKAnnot, nFlag);
  }
  FX_BOOL KillFocusAnnot(FX_UINT nFlag = 0) {
    return m_pSDKDoc->KillFocusAnnot(nFlag);
  }
  void KillFocusAnnotIfNeeded();
  FX_BOOL Annot_HasAppearance(CPDF_Annot* pAnnot);

  CPDFSDK_Annot* AddAnnot(CPDF_Dictionary* pDict);
  CPDFSDK_Annot* AddAnnot(const FX_CHAR* lpSubType, CPDF_Dictionary* pDict);
  CPDFSDK_Annot* AddAnnot(CPDF_Annot* pPDFAnnot);

  FX_BOOL DeleteAnnot(CPDFSDK_Annot* pAnnot);
  size_t CountAnnots() const;
  CPDFSDK_Annot* GetAnnot(size_t nIndex);
  CPDFSDK_Annot* GetAnnotByDict(CPDF_Dictionary* pDict);

#ifdef PDF_ENABLE_XFA
  CPDFSDK_Annot* AddAnnot(CXFA_FFWidget* pPDFAnnot);
  CPDFSDK_Annot* GetAnnotByXFAWidget(CXFA_FFWidget* hWidget);
  CPDFXFA_Page* GetPDFXFAPage() { return m_page; }
  CPDF_Page* GetPDFPage();
#else
  CPDF_Page* GetPDFPage() { return m_page; }
#endif  // PDF_ENABLE_XFA

  CPDF_Document* GetPDFDocument();
  CPDFSDK_Document* GetSDKDocument() { return m_pSDKDoc; }
  FX_BOOL OnLButtonDown(const CFX_FloatPoint& point, FX_UINT nFlag);
  FX_BOOL OnLButtonUp(const CFX_FloatPoint& point, FX_UINT nFlag);
#ifdef PDF_ENABLE_XFA
  FX_BOOL OnRButtonDown(const CFX_FloatPoint& point, FX_UINT nFlag);
  FX_BOOL OnRButtonUp(const CFX_FloatPoint& point, FX_UINT nFlag);
#endif  // PDF_ENABLE_XFA
  FX_BOOL OnChar(int nChar, FX_UINT nFlag);
  FX_BOOL OnKeyDown(int nKeyCode, int nFlag);
  FX_BOOL OnKeyUp(int nKeyCode, int nFlag);

  FX_BOOL OnMouseMove(const CFX_FloatPoint& point, int nFlag);
  FX_BOOL OnMouseWheel(double deltaX,
                       double deltaY,
                       const CFX_FloatPoint& point,
                       int nFlag);
  bool IsValidAnnot(const CPDF_Annot* p) const;
  void GetCurrentMatrix(CFX_Matrix& matrix) { matrix = m_curMatrix; }
  void UpdateRects(const std::vector<CFX_FloatRect>& rects);
  void UpdateView(CPDFSDK_Annot* pAnnot);
  const std::vector<CPDFSDK_Annot*>& GetAnnotList() const {
    return m_fxAnnotArray;
  }

  int GetPageIndex() const;
  void LoadFXAnnots();
  void ClearFXAnnots();
  void SetValid(FX_BOOL bValid) { m_bValid = bValid; }
  FX_BOOL IsValid() { return m_bValid; }
  void SetLock(FX_BOOL bLocked) { m_bLocked = bLocked; }
  FX_BOOL IsLocked() { return m_bLocked; }
#ifndef PDF_ENABLE_XFA
  void TakeOverPage() { m_bTakeOverPage = TRUE; }
#endif  // PDF_ENABLE_XFA

 private:
  int GetPageIndexForStaticPDF() const;

  CFX_Matrix m_curMatrix;
  UnderlyingPageType* const m_page;
  std::unique_ptr<CPDF_AnnotList> m_pAnnotList;
  std::vector<CPDFSDK_Annot*> m_fxAnnotArray;
  CPDFSDK_Document* m_pSDKDoc;
#ifdef PDF_ENABLE_XFA
  CPDFSDK_Annot* m_CaptureWidget;
#else  // PDF_ENABLE_XFA
  CPDFSDK_Widget* m_CaptureWidget;
  FX_BOOL m_bTakeOverPage;
#endif  // PDF_ENABLE_XFA
  FX_BOOL m_bEnterWidget;
  FX_BOOL m_bExitWidget;
  FX_BOOL m_bOnWidget;
  FX_BOOL m_bValid;
  FX_BOOL m_bLocked;
};

#endif  // FPDFSDK_INCLUDE_FSDK_MGR_H_
