// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_DOC_H_
#define FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_DOC_H_

#include "xfa/fxfa/include/xfa_ffdoc.h"

#include <vector>

#include "public/fpdfview.h"
#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/include/xfa_ffdochandler.h"

class CPDFXFA_App;
class CPDFXFA_Document;
class CPDFXFA_Page;
class CPDFSDK_Document;
class CPDFDoc_Environment;
class IJS_Runtime;
class IJS_Context;
class CXFA_FFDocHandler;

class CPDFXFA_Document : public IXFA_DocProvider {
 public:
  CPDFXFA_Document(CPDF_Document* pPDFDoc, CPDFXFA_App* pProvider);
  ~CPDFXFA_Document() override;

  FX_BOOL LoadXFADoc();
  CPDFXFA_App* GetApp() { return m_pApp; }
  CPDF_Document* GetPDFDoc() { return m_pPDFDoc; }
  CXFA_FFDoc* GetXFADoc() { return m_pXFADoc; }
  CXFA_FFDocView* GetXFADocView() { return m_pXFADocView; }

  int GetPageCount();
  CPDFXFA_Page* GetPage(int page_index);
  CPDFXFA_Page* GetPage(CXFA_FFPageView* pPage);

  void DeletePage(int page_index);
  void RemovePage(CPDFXFA_Page* page);
  int GetDocType() { return m_iDocType; }

  CPDFSDK_Document* GetSDKDocument(CPDFDoc_Environment* pFormFillEnv);

  void FXRect2PDFRect(const CFX_RectF& fxRectF, CFX_FloatRect& pdfRect);

  // IXFA_DocProvider
  void SetChangeMark(CXFA_FFDoc* hDoc) override;
  // used in dynamic xfa, dwFlags refer to XFA_INVALIDATE_XXX macros.
  void InvalidateRect(CXFA_FFPageView* pPageView,
                      const CFX_RectF& rt,
                      uint32_t dwFlags = 0) override;
  // show or hide caret
  void DisplayCaret(CXFA_FFWidget* hWidget,
                    FX_BOOL bVisible,
                    const CFX_RectF* pRtAnchor) override;
  // dwPos: (0:bottom 1:top)
  FX_BOOL GetPopupPos(CXFA_FFWidget* hWidget,
                      FX_FLOAT fMinPopup,
                      FX_FLOAT fMaxPopup,
                      const CFX_RectF& rtAnchor,
                      CFX_RectF& rtPopup) override;
  FX_BOOL PopupMenu(CXFA_FFWidget* hWidget,
                    CFX_PointF ptPopup,
                    const CFX_RectF* pRectExclude = nullptr) override;

  // dwFlags XFA_PAGEVIEWEVENT_Added, XFA_PAGEVIEWEVENT_Removing
  void PageViewEvent(CXFA_FFPageView* pPageView, uint32_t dwFlags) override;
  void WidgetPostAdd(CXFA_FFWidget* hWidget,
                     CXFA_WidgetAcc* pWidgetData) override;
  void WidgetPreRemove(CXFA_FFWidget* hWidget,
                       CXFA_WidgetAcc* pWidgetData) override;

  // return true if render it.
  FX_BOOL RenderCustomWidget(CXFA_FFWidget* hWidget,
                             CFX_Graphics* pGS,
                             CFX_Matrix* pMatrix,
                             const CFX_RectF& rtUI) override;

  // Host method
  int32_t CountPages(CXFA_FFDoc* hDoc) override;
  int32_t GetCurrentPage(CXFA_FFDoc* hDoc) override;
  void SetCurrentPage(CXFA_FFDoc* hDoc, int32_t iCurPage) override;
  FX_BOOL IsCalculationsEnabled(CXFA_FFDoc* hDoc) override;
  void SetCalculationsEnabled(CXFA_FFDoc* hDoc, FX_BOOL bEnabled) override;
  void GetTitle(CXFA_FFDoc* hDoc, CFX_WideString& wsTitle) override;
  void SetTitle(CXFA_FFDoc* hDoc, const CFX_WideString& wsTitle) override;
  void ExportData(CXFA_FFDoc* hDoc,
                  const CFX_WideString& wsFilePath,
                  FX_BOOL bXDP = TRUE) override;
  void ImportData(CXFA_FFDoc* hDoc, const CFX_WideString& wsFilePath) override;
  void GotoURL(CXFA_FFDoc* hDoc,
               const CFX_WideString& bsURL,
               FX_BOOL bAppend = TRUE) override;
  FX_BOOL IsValidationsEnabled(CXFA_FFDoc* hDoc) override;
  void SetValidationsEnabled(CXFA_FFDoc* hDoc, FX_BOOL bEnabled) override;
  void SetFocusWidget(CXFA_FFDoc* hDoc, CXFA_FFWidget* hWidget) override;
  void Print(CXFA_FFDoc* hDoc,
             int32_t nStartPage,
             int32_t nEndPage,
             uint32_t dwOptions) override;

  // LayoutPseudo method
  int32_t AbsPageCountInBatch(CXFA_FFDoc* hDoc) override;
  int32_t AbsPageInBatch(CXFA_FFDoc* hDoc, CXFA_FFWidget* hWidget) override;
  int32_t SheetCountInBatch(CXFA_FFDoc* hDoc) override;
  int32_t SheetInBatch(CXFA_FFDoc* hDoc, CXFA_FFWidget* hWidget) override;

  int32_t Verify(CXFA_FFDoc* hDoc,
                 CXFA_Node* pSigNode,
                 FX_BOOL bUsed = TRUE) override;
  FX_BOOL Sign(CXFA_FFDoc* hDoc,
               CXFA_NodeList* pNodeList,
               const CFX_WideStringC& wsExpression,
               const CFX_WideStringC& wsXMLIdent,
               const CFX_WideStringC& wsValue = FX_WSTRC(L"open"),
               FX_BOOL bUsed = TRUE) override;
  CXFA_NodeList* Enumerate(CXFA_FFDoc* hDoc) override;
  FX_BOOL Clear(CXFA_FFDoc* hDoc,
                CXFA_Node* pSigNode,
                FX_BOOL bCleared = TRUE) override;

  // Get document path
  void GetURL(CXFA_FFDoc* hDoc, CFX_WideString& wsDocURL) override;
  FX_ARGB GetHighlightColor(CXFA_FFDoc* hDoc) override;

  /**
   *Submit data to email, http, ftp.
   * @param[in] hDoc The document handler.
   * @param[in] eFormat Determines the format in which the data will be
   *submitted. XFA_ATTRIBUTEENUM_Xdp, XFA_ATTRIBUTEENUM_Xml...
   * @param[in] wsTarget The URL to which the data will be submitted.
   * @param[in] eEncoding The encoding of text content.
   * @param[in] pXDPContent Controls what subset of the data is submitted, used
   *only when the format property is xdp.
   * @param[in] bEmbedPDF, specifies whether PDF is embedded in the submitted
   *content or not.
   */
  FX_BOOL SubmitData(CXFA_FFDoc* hDoc, CXFA_Submit submit) override;

  FX_BOOL CheckWord(CXFA_FFDoc* hDoc, const CFX_ByteStringC& sWord) override;
  FX_BOOL GetSuggestWords(CXFA_FFDoc* hDoc,
                          const CFX_ByteStringC& sWord,
                          std::vector<CFX_ByteString>& sSuggest) override;

  // Get PDF javascript object, set the object to pValue.
  FX_BOOL GetPDFScriptObject(CXFA_FFDoc* hDoc,
                             const CFX_ByteStringC& utf8Name,
                             CFXJSE_Value* pValue) override;

  FX_BOOL GetGlobalProperty(CXFA_FFDoc* hDoc,
                            const CFX_ByteStringC& szPropName,
                            CFXJSE_Value* pValue) override;
  FX_BOOL SetGlobalProperty(CXFA_FFDoc* hDoc,
                            const CFX_ByteStringC& szPropName,
                            CFXJSE_Value* pValue) override;
  CPDF_Document* OpenPDF(CXFA_FFDoc* hDoc,
                         IFX_FileRead* pFile,
                         FX_BOOL bTakeOverFile) override;

  IFX_FileRead* OpenLinkedFile(CXFA_FFDoc* hDoc,
                               const CFX_WideString& wsLink) override;

  FX_BOOL _OnBeforeNotifySumbit();
  void _OnAfterNotifySumbit();
  FX_BOOL _NotifySubmit(FX_BOOL bPrevOrPost);
  FX_BOOL _SubmitData(CXFA_FFDoc* hDoc, CXFA_Submit submit);
  FX_BOOL _MailToInfo(CFX_WideString& csURL,
                      CFX_WideString& csToAddress,
                      CFX_WideString& csCCAddress,
                      CFX_WideString& csBCCAddress,
                      CFX_WideString& csSubject,
                      CFX_WideString& csMsg);
  FX_BOOL _ExportSubmitFile(FPDF_FILEHANDLER* ppFileHandler,
                            int fileType,
                            FPDF_DWORD encodeType,
                            FPDF_DWORD flag = 0x01111111);
  void _ToXFAContentFlags(CFX_WideString csSrcContent, FPDF_DWORD& flag);
  void _ClearChangeMark();

 private:
  enum LoadStatus {
    FXFA_LOADSTATUS_PRELOAD = 0,
    FXFA_LOADSTATUS_LOADING,
    FXFA_LOADSTATUS_LOADED,
    FXFA_LOADSTATUS_CLOSING,
    FXFA_LOADSTATUS_CLOSED
  };

  void CloseXFADoc(CXFA_FFDocHandler* pDoc) {
    if (pDoc) {
      m_pXFADoc->CloseDoc();
      delete m_pXFADoc;
      m_pXFADoc = nullptr;
      m_pXFADocView = nullptr;
    }
  }

  int m_iDocType;
  CPDF_Document* m_pPDFDoc;
  CPDFSDK_Document* m_pSDKDoc;
  CXFA_FFDoc* m_pXFADoc;
  CXFA_FFDocView* m_pXFADocView;
  CPDFXFA_App* m_pApp;
  IJS_Context* m_pJSContext;
  CFX_ArrayTemplate<CPDFXFA_Page*> m_XFAPageList;
  LoadStatus m_nLoadStatus;
  int m_nPageCount;
};

#endif  // FPDFSDK_FPDFXFA_INCLUDE_FPDFXFA_DOC_H_
