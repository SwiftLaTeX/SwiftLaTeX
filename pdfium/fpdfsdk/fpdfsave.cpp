// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_save.h"

#include <vector>

#include "core/fpdfapi/fpdf_edit/include/cpdf_creator.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fxcrt/include/fx_ext.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "public/fpdf_edit.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_app.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"
#include "public/fpdf_formfill.h"
#include "xfa/fxfa/include/cxfa_eventparam.h"
#include "xfa/fxfa/include/xfa_checksum.h"
#include "xfa/fxfa/include/xfa_ffapp.h"
#include "xfa/fxfa/include/xfa_ffdocview.h"
#include "xfa/fxfa/include/xfa_ffwidgethandler.h"
#endif

#if _FX_OS_ == _FX_ANDROID_
#include "time.h"
#else
#include <ctime>
#endif

class CFX_IFileWrite final : public IFX_StreamWrite {
 public:
  CFX_IFileWrite();
  FX_BOOL Init(FPDF_FILEWRITE* pFileWriteStruct);
  FX_BOOL WriteBlock(const void* pData, size_t size) override;
  void Release() override;

 protected:
  ~CFX_IFileWrite() override {}

  FPDF_FILEWRITE* m_pFileWriteStruct;
};

CFX_IFileWrite::CFX_IFileWrite() {
  m_pFileWriteStruct = nullptr;
}

FX_BOOL CFX_IFileWrite::Init(FPDF_FILEWRITE* pFileWriteStruct) {
  if (!pFileWriteStruct)
    return FALSE;

  m_pFileWriteStruct = pFileWriteStruct;
  return TRUE;
}

FX_BOOL CFX_IFileWrite::WriteBlock(const void* pData, size_t size) {
  if (!m_pFileWriteStruct)
    return FALSE;

  m_pFileWriteStruct->WriteBlock(m_pFileWriteStruct, pData, size);
  return TRUE;
}

void CFX_IFileWrite::Release() {
  delete this;
}

namespace {

#ifdef PDF_ENABLE_XFA
bool SaveXFADocumentData(CPDFXFA_Document* pDocument,
                         std::vector<ScopedFileStream>* fileList) {
  if (!pDocument)
    return false;

  if (pDocument->GetDocType() != DOCTYPE_DYNAMIC_XFA &&
      pDocument->GetDocType() != DOCTYPE_STATIC_XFA)
    return true;

  if (!CPDFXFA_App::GetInstance()->GetXFAApp())
    return true;

  CXFA_FFDocView* pXFADocView = pDocument->GetXFADocView();
  if (!pXFADocView)
    return true;

  CPDF_Document* pPDFDocument = pDocument->GetPDFDoc();
  if (!pDocument)
    return false;

  CPDF_Dictionary* pRoot = pPDFDocument->GetRoot();
  if (!pRoot)
    return false;

  CPDF_Dictionary* pAcroForm = pRoot->GetDictBy("AcroForm");
  if (!pAcroForm)
    return false;

  CPDF_Object* pXFA = pAcroForm->GetObjectBy("XFA");
  if (!pXFA)
    return true;

  CPDF_Array* pArray = pXFA->AsArray();
  if (!pArray)
    return false;

  int size = pArray->GetCount();
  int iFormIndex = -1;
  int iDataSetsIndex = -1;
  int iTemplate = -1;
  int iLast = size - 2;
  for (int i = 0; i < size - 1; i++) {
    CPDF_Object* pPDFObj = pArray->GetObjectAt(i);
    if (!pPDFObj->IsString())
      continue;
    if (pPDFObj->GetString() == "form")
      iFormIndex = i + 1;
    else if (pPDFObj->GetString() == "datasets")
      iDataSetsIndex = i + 1;
    else if (pPDFObj->GetString() == "template")
      iTemplate = i + 1;
  }
  std::unique_ptr<CXFA_ChecksumContext> pContext(new CXFA_ChecksumContext);
  pContext->StartChecksum();

  // template
  if (iTemplate > -1) {
    CPDF_Stream* pTemplateStream = pArray->GetStreamAt(iTemplate);
    CPDF_StreamAcc streamAcc;
    streamAcc.LoadAllData(pTemplateStream);
    uint8_t* pData = (uint8_t*)streamAcc.GetData();
    uint32_t dwSize2 = streamAcc.GetSize();
    ScopedFileStream pTemplate(FX_CreateMemoryStream(pData, dwSize2));
    pContext->UpdateChecksum(pTemplate.get());
  }
  CPDF_Stream* pFormStream = nullptr;
  CPDF_Stream* pDataSetsStream = nullptr;
  if (iFormIndex != -1) {
    // Get form CPDF_Stream
    CPDF_Object* pFormPDFObj = pArray->GetObjectAt(iFormIndex);
    if (pFormPDFObj->IsReference()) {
      CPDF_Object* pFormDirectObj = pFormPDFObj->GetDirect();
      if (pFormDirectObj && pFormDirectObj->IsStream()) {
        pFormStream = (CPDF_Stream*)pFormDirectObj;
      }
    } else if (pFormPDFObj->IsStream()) {
      pFormStream = (CPDF_Stream*)pFormPDFObj;
    }
  }

  if (iDataSetsIndex != -1) {
    // Get datasets CPDF_Stream
    CPDF_Object* pDataSetsPDFObj = pArray->GetObjectAt(iDataSetsIndex);
    if (pDataSetsPDFObj->IsReference()) {
      CPDF_Reference* pDataSetsRefObj = (CPDF_Reference*)pDataSetsPDFObj;
      CPDF_Object* pDataSetsDirectObj = pDataSetsRefObj->GetDirect();
      if (pDataSetsDirectObj && pDataSetsDirectObj->IsStream()) {
        pDataSetsStream = (CPDF_Stream*)pDataSetsDirectObj;
      }
    } else if (pDataSetsPDFObj->IsStream()) {
      pDataSetsStream = (CPDF_Stream*)pDataSetsPDFObj;
    }
  }
  // L"datasets"
  {
    ScopedFileStream pDsfileWrite(FX_CreateMemoryStream());
    if (pXFADocView->GetDoc()->SavePackage(XFA_HASHCODE_Datasets,
                                           pDsfileWrite.get(), nullptr) &&
        pDsfileWrite->GetSize() > 0) {
      // Datasets
      pContext->UpdateChecksum(pDsfileWrite.get());
      pContext->FinishChecksum();
      CPDF_Dictionary* pDataDict = new CPDF_Dictionary;
      if (iDataSetsIndex != -1) {
        if (pDataSetsStream)
          pDataSetsStream->InitStreamFromFile(pDsfileWrite.get(), pDataDict);
      } else {
        CPDF_Stream* pData = new CPDF_Stream(nullptr, 0, nullptr);
        pData->InitStreamFromFile(pDsfileWrite.get(), pDataDict);
        pPDFDocument->AddIndirectObject(pData);
        iLast = pArray->GetCount() - 2;
        pArray->InsertAt(iLast, new CPDF_String("datasets", FALSE));
        pArray->InsertAt(iLast + 1, pData, pPDFDocument);
      }
      fileList->push_back(std::move(pDsfileWrite));
    }
  }
  // L"form"
  {
    ScopedFileStream pfileWrite(FX_CreateMemoryStream());
    if (pXFADocView->GetDoc()->SavePackage(XFA_HASHCODE_Form, pfileWrite.get(),
                                           pContext.get()) &&
        pfileWrite->GetSize() > 0) {
      CPDF_Dictionary* pDataDict = new CPDF_Dictionary;
      if (iFormIndex != -1) {
        if (pFormStream)
          pFormStream->InitStreamFromFile(pfileWrite.get(), pDataDict);
      } else {
        CPDF_Stream* pData = new CPDF_Stream(nullptr, 0, nullptr);
        pData->InitStreamFromFile(pfileWrite.get(), pDataDict);
        pPDFDocument->AddIndirectObject(pData);
        iLast = pArray->GetCount() - 2;
        pArray->InsertAt(iLast, new CPDF_String("form", FALSE));
        pArray->InsertAt(iLast + 1, pData, pPDFDocument);
      }
      fileList->push_back(std::move(pfileWrite));
    }
  }
  return true;
}

bool SendPostSaveToXFADoc(CPDFXFA_Document* pDocument) {
  if (!pDocument)
    return false;

  if (pDocument->GetDocType() != DOCTYPE_DYNAMIC_XFA &&
      pDocument->GetDocType() != DOCTYPE_STATIC_XFA)
    return true;

  CXFA_FFDocView* pXFADocView = pDocument->GetXFADocView();
  if (!pXFADocView)
    return false;

  CXFA_FFWidgetHandler* pWidgetHander = pXFADocView->GetWidgetHandler();
  std::unique_ptr<CXFA_WidgetAccIterator> pWidgetAccIterator(
      pXFADocView->CreateWidgetAccIterator());
  while (CXFA_WidgetAcc* pWidgetAcc = pWidgetAccIterator->MoveToNext()) {
    CXFA_EventParam preParam;
    preParam.m_eType = XFA_EVENT_PostSave;
    pWidgetHander->ProcessEvent(pWidgetAcc, &preParam);
  }
  pXFADocView->UpdateDocView();
  pDocument->_ClearChangeMark();
  return true;
}

bool SendPreSaveToXFADoc(CPDFXFA_Document* pDocument,
                         std::vector<ScopedFileStream>* fileList) {
  if (pDocument->GetDocType() != DOCTYPE_DYNAMIC_XFA &&
      pDocument->GetDocType() != DOCTYPE_STATIC_XFA)
    return true;

  CXFA_FFDocView* pXFADocView = pDocument->GetXFADocView();
  if (!pXFADocView)
    return true;

  CXFA_FFWidgetHandler* pWidgetHander = pXFADocView->GetWidgetHandler();
  std::unique_ptr<CXFA_WidgetAccIterator> pWidgetAccIterator(
      pXFADocView->CreateWidgetAccIterator());
  while (CXFA_WidgetAcc* pWidgetAcc = pWidgetAccIterator->MoveToNext()) {
    CXFA_EventParam preParam;
    preParam.m_eType = XFA_EVENT_PreSave;
    pWidgetHander->ProcessEvent(pWidgetAcc, &preParam);
  }
  pXFADocView->UpdateDocView();
  return SaveXFADocumentData(pDocument, fileList);
}
#endif  // PDF_ENABLE_XFA

bool FPDF_Doc_Save(FPDF_DOCUMENT document,
                   FPDF_FILEWRITE* pFileWrite,
                   FPDF_DWORD flags,
                   FPDF_BOOL bSetVersion,
                   int fileVerion) {
  CPDF_Document* pPDFDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pPDFDoc)
    return 0;

#ifdef PDF_ENABLE_XFA
  CPDFXFA_Document* pDoc = static_cast<CPDFXFA_Document*>(document);
  std::vector<ScopedFileStream> fileList;
  SendPreSaveToXFADoc(pDoc, &fileList);
#endif  // PDF_ENABLE_XFA

  if (flags < FPDF_INCREMENTAL || flags > FPDF_REMOVE_SECURITY)
    flags = 0;

  CPDF_Creator FileMaker(pPDFDoc);
  if (bSetVersion)
    FileMaker.SetFileVersion(fileVerion);
  if (flags == FPDF_REMOVE_SECURITY) {
    flags = 0;
    FileMaker.RemoveSecurity();
  }

  CFX_IFileWrite* pStreamWrite = new CFX_IFileWrite;
  pStreamWrite->Init(pFileWrite);
  bool bRet = FileMaker.Create(pStreamWrite, flags);
#ifdef PDF_ENABLE_XFA
  SendPostSaveToXFADoc(pDoc);
#endif  // PDF_ENABLE_XFA
  pStreamWrite->Release();
  return bRet;
}

}  // namespace

DLLEXPORT FPDF_BOOL STDCALL FPDF_SaveAsCopy(FPDF_DOCUMENT document,
                                            FPDF_FILEWRITE* pFileWrite,
                                            FPDF_DWORD flags) {
  return FPDF_Doc_Save(document, pFileWrite, flags, FALSE, 0);
}

DLLEXPORT FPDF_BOOL STDCALL FPDF_SaveWithVersion(FPDF_DOCUMENT document,
                                                 FPDF_FILEWRITE* pFileWrite,
                                                 FPDF_DWORD flags,
                                                 int fileVersion) {
  return FPDF_Doc_Save(document, pFileWrite, flags, TRUE, fileVersion);
}
