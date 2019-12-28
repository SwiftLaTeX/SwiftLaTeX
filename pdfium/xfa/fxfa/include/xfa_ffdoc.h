// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_INCLUDE_XFA_FFDOC_H_
#define XFA_FXFA_INCLUDE_XFA_FFDOC_H_

#include <map>
#include <memory>

#include "xfa/fxfa/include/fxfa.h"
#include "xfa/fxfa/parser/cxfa_document_parser.h"
#include "xfa/fxfa/parser/xfa_document.h"

class CXFA_ChecksumContext;
class CXFA_FFApp;
class CXFA_FFNotify;
class CXFA_FFDocView;

struct FX_IMAGEDIB_AND_DPI {
  CFX_DIBSource* pDibSource;
  int32_t iImageXDpi;
  int32_t iImageYDpi;
};

class CXFA_FFDoc {
 public:
  CXFA_FFDoc(CXFA_FFApp* pApp, IXFA_DocProvider* pDocProvider);
  ~CXFA_FFDoc();
  IXFA_DocProvider* GetDocProvider() { return m_pDocProvider; }
  uint32_t GetDocType();
  int32_t StartLoad();
  int32_t DoLoad(IFX_Pause* pPause = nullptr);
  void StopLoad();
  CXFA_FFDocView* CreateDocView(uint32_t dwView = 0);
  FX_BOOL OpenDoc(IFX_FileRead* pStream, FX_BOOL bTakeOverFile);
  FX_BOOL OpenDoc(CPDF_Document* pPDFDoc);
  FX_BOOL CloseDoc();
  void SetDocType(uint32_t dwType);
  CXFA_Document* GetXFADoc() { return m_pDocumentParser->GetDocument(); }
  CXFA_FFApp* GetApp() { return m_pApp; }
  CXFA_FFDocView* GetDocView(CXFA_LayoutProcessor* pLayout);
  CXFA_FFDocView* GetDocView();
  CPDF_Document* GetPDFDoc();
  CFX_DIBitmap* GetPDFNamedImage(const CFX_WideStringC& wsName,
                                 int32_t& iImageXDpi,
                                 int32_t& iImageYDpi);

  bool SavePackage(XFA_HashCode code,
                   IFX_FileWrite* pFile,
                   CXFA_ChecksumContext* pCSContext);
  FX_BOOL ImportData(IFX_FileRead* pStream, FX_BOOL bXDP = TRUE);

 protected:
  IXFA_DocProvider* m_pDocProvider;
  std::unique_ptr<CXFA_DocumentParser> m_pDocumentParser;
  IFX_FileRead* m_pStream;
  CXFA_FFApp* m_pApp;
  std::unique_ptr<CXFA_FFNotify> m_pNotify;
  CPDF_Document* m_pPDFDoc;
  std::map<uint32_t, FX_IMAGEDIB_AND_DPI> m_HashToDibDpiMap;
  std::map<uint32_t, std::unique_ptr<CXFA_FFDocView>> m_TypeToDocViewMap;
  uint32_t m_dwDocType;
  FX_BOOL m_bOwnStream;
};

#endif  // XFA_FXFA_INCLUDE_XFA_FFDOC_H_
