// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdfview.h"

#include <memory>

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_render/include/cpdf_renderoptions.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "core/fxge/include/fx_ge.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/include/fsdk_rendercontext.h"
#include "fpdfsdk/javascript/ijs_runtime.h"
#include "public/fpdf_ext.h"
#include "public/fpdf_progressive.h"
#include "third_party/base/numerics/safe_conversions_impl.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_app.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_doc.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_page.h"
#include "fpdfsdk/fpdfxfa/include/fpdfxfa_util.h"
#include "public/fpdf_formfill.h"
#endif  // PDF_ENABLE_XFA

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#include "core/fxge/include/fx_ge_win32.h"
#endif

UnderlyingDocumentType* UnderlyingFromFPDFDocument(FPDF_DOCUMENT doc) {
  return static_cast<UnderlyingDocumentType*>(doc);
}

FPDF_DOCUMENT FPDFDocumentFromUnderlying(UnderlyingDocumentType* doc) {
  return static_cast<FPDF_DOCUMENT>(doc);
}

UnderlyingPageType* UnderlyingFromFPDFPage(FPDF_PAGE page) {
  return static_cast<UnderlyingPageType*>(page);
}

CPDF_Document* CPDFDocumentFromFPDFDocument(FPDF_DOCUMENT doc) {
#ifdef PDF_ENABLE_XFA
  return doc ? UnderlyingFromFPDFDocument(doc)->GetPDFDoc() : nullptr;
#else   // PDF_ENABLE_XFA
  return UnderlyingFromFPDFDocument(doc);
#endif  // PDF_ENABLE_XFA
}

FPDF_DOCUMENT FPDFDocumentFromCPDFDocument(CPDF_Document* doc) {
#ifdef PDF_ENABLE_XFA
  return doc ? FPDFDocumentFromUnderlying(
                   new CPDFXFA_Document(doc, CPDFXFA_App::GetInstance()))
             : nullptr;
#else   // PDF_ENABLE_XFA
  return FPDFDocumentFromUnderlying(doc);
#endif  // PDF_ENABLE_XFA
}

CPDF_Page* CPDFPageFromFPDFPage(FPDF_PAGE page) {
#ifdef PDF_ENABLE_XFA
  return page ? UnderlyingFromFPDFPage(page)->GetPDFPage() : nullptr;
#else   // PDF_ENABLE_XFA
  return UnderlyingFromFPDFPage(page);
#endif  // PDF_ENABLE_XFA
}

CFX_DIBitmap* CFXBitmapFromFPDFBitmap(FPDF_BITMAP bitmap) {
  return static_cast<CFX_DIBitmap*>(bitmap);
}

#ifdef PDF_ENABLE_XFA
CFPDF_FileStream::CFPDF_FileStream(FPDF_FILEHANDLER* pFS) {
  m_pFS = pFS;
  m_nCurPos = 0;
}

IFX_FileStream* CFPDF_FileStream::Retain() {
  return this;
}

void CFPDF_FileStream::Release() {
  if (m_pFS && m_pFS->Release)
    m_pFS->Release(m_pFS->clientData);
  delete this;
}

FX_FILESIZE CFPDF_FileStream::GetSize() {
  if (m_pFS && m_pFS->GetSize)
    return (FX_FILESIZE)m_pFS->GetSize(m_pFS->clientData);
  return 0;
}

FX_BOOL CFPDF_FileStream::IsEOF() {
  return m_nCurPos >= GetSize();
}

FX_FILESIZE CFPDF_FileStream::GetPosition() {
  return m_nCurPos;
}

FX_BOOL CFPDF_FileStream::ReadBlock(void* buffer,
                                    FX_FILESIZE offset,
                                    size_t size) {
  if (!buffer || !size || !m_pFS->ReadBlock)
    return FALSE;

  if (m_pFS->ReadBlock(m_pFS->clientData, (FPDF_DWORD)offset, buffer,
                       (FPDF_DWORD)size) == 0) {
    m_nCurPos = offset + size;
    return TRUE;
  }
  return FALSE;
}

size_t CFPDF_FileStream::ReadBlock(void* buffer, size_t size) {
  if (!buffer || !size || !m_pFS->ReadBlock)
    return 0;

  FX_FILESIZE nSize = GetSize();
  if (m_nCurPos >= nSize)
    return 0;
  FX_FILESIZE dwAvail = nSize - m_nCurPos;
  if (dwAvail < (FX_FILESIZE)size)
    size = (size_t)dwAvail;
  if (m_pFS->ReadBlock(m_pFS->clientData, (FPDF_DWORD)m_nCurPos, buffer,
                       (FPDF_DWORD)size) == 0) {
    m_nCurPos += size;
    return size;
  }

  return 0;
}

FX_BOOL CFPDF_FileStream::WriteBlock(const void* buffer,
                                     FX_FILESIZE offset,
                                     size_t size) {
  if (!m_pFS || !m_pFS->WriteBlock)
    return FALSE;

  if (m_pFS->WriteBlock(m_pFS->clientData, (FPDF_DWORD)offset, buffer,
                        (FPDF_DWORD)size) == 0) {
    m_nCurPos = offset + size;
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CFPDF_FileStream::Flush() {
  if (!m_pFS || !m_pFS->Flush)
    return TRUE;

  return m_pFS->Flush(m_pFS->clientData) == 0;
}
#endif  // PDF_ENABLE_XFA

CPDF_CustomAccess::CPDF_CustomAccess(FPDF_FILEACCESS* pFileAccess) {
  m_FileAccess = *pFileAccess;
#ifdef PDF_ENABLE_XFA
  m_BufferOffset = (uint32_t)-1;
#endif  // PDF_ENABLE_XFA
}

#ifdef PDF_ENABLE_XFA
CFX_ByteString CPDF_CustomAccess::GetFullPath() {
  return "";
}

FX_BOOL CPDF_CustomAccess::GetByte(uint32_t pos, uint8_t& ch) {
  if (pos >= m_FileAccess.m_FileLen)
    return FALSE;
  if (m_BufferOffset == (uint32_t)-1 || pos < m_BufferOffset ||
      pos >= m_BufferOffset + 512) {
    // Need to read from file access
    m_BufferOffset = pos;
    int size = 512;
    if (pos + 512 > m_FileAccess.m_FileLen)
      size = m_FileAccess.m_FileLen - pos;
    if (!m_FileAccess.m_GetBlock(m_FileAccess.m_Param, m_BufferOffset, m_Buffer,
                                 size))
      return FALSE;
  }
  ch = m_Buffer[pos - m_BufferOffset];
  return TRUE;
}

FX_BOOL CPDF_CustomAccess::GetBlock(uint32_t pos,
                                    uint8_t* pBuf,
                                    uint32_t size) {
  if (pos + size > m_FileAccess.m_FileLen)
    return FALSE;
  return m_FileAccess.m_GetBlock(m_FileAccess.m_Param, pos, pBuf, size);
}
#endif  // PDF_ENABLE_XFA

FX_FILESIZE CPDF_CustomAccess::GetSize() {
  return m_FileAccess.m_FileLen;
}

void CPDF_CustomAccess::Release() {
  delete this;
}

FX_BOOL CPDF_CustomAccess::ReadBlock(void* buffer,
                                     FX_FILESIZE offset,
                                     size_t size) {
  if (offset < 0) {
    return FALSE;
  }
  FX_SAFE_FILESIZE newPos =
      pdfium::base::checked_cast<FX_FILESIZE, size_t>(size);
  newPos += offset;
  if (!newPos.IsValid() ||
      newPos.ValueOrDie() > static_cast<FX_FILESIZE>(m_FileAccess.m_FileLen)) {
    return FALSE;
  }
  return m_FileAccess.m_GetBlock(m_FileAccess.m_Param, offset, (uint8_t*)buffer,
                                 size);
}

// 0 bit: FPDF_POLICY_MACHINETIME_ACCESS
static uint32_t foxit_sandbox_policy = 0xFFFFFFFF;

void FSDK_SetSandBoxPolicy(FPDF_DWORD policy, FPDF_BOOL enable) {
  switch (policy) {
    case FPDF_POLICY_MACHINETIME_ACCESS: {
      if (enable)
        foxit_sandbox_policy |= 0x01;
      else
        foxit_sandbox_policy &= 0xFFFFFFFE;
    } break;
    default:
      break;
  }
}

FPDF_BOOL FSDK_IsSandBoxPolicyEnabled(FPDF_DWORD policy) {
  switch (policy) {
    case FPDF_POLICY_MACHINETIME_ACCESS:
      return !!(foxit_sandbox_policy & 0x01);
    default:
      return FALSE;
  }
}

CCodec_ModuleMgr* g_pCodecModule = nullptr;

DLLEXPORT void STDCALL FPDF_InitLibrary() {
  FPDF_InitLibraryWithConfig(nullptr);
}

DLLEXPORT void STDCALL
FPDF_InitLibraryWithConfig(const FPDF_LIBRARY_CONFIG* cfg) {
  g_pCodecModule = new CCodec_ModuleMgr();

  CFX_GEModule::Create(cfg ? cfg->m_pUserFontPaths : nullptr, g_pCodecModule);
  CPDF_ModuleMgr::Create();
  CPDF_ModuleMgr* pModuleMgr = CPDF_ModuleMgr::Get();
  pModuleMgr->SetCodecModule(g_pCodecModule);
  pModuleMgr->InitPageModule();
  pModuleMgr->LoadEmbeddedGB1CMaps();
  pModuleMgr->LoadEmbeddedJapan1CMaps();
  pModuleMgr->LoadEmbeddedCNS1CMaps();
  pModuleMgr->LoadEmbeddedKorea1CMaps();

#ifdef PDF_ENABLE_XFA
  CPDFXFA_App::GetInstance()->Initialize(
      (cfg && cfg->version >= 2) ? static_cast<v8::Isolate*>(cfg->m_pIsolate)
                                 : nullptr);
#endif  // PDF_ENABLE_XFA

  if (cfg && cfg->version >= 2)
    IJS_Runtime::Initialize(cfg->m_v8EmbedderSlot, cfg->m_pIsolate);
}

DLLEXPORT void STDCALL FPDF_DestroyLibrary() {
#ifdef PDF_ENABLE_XFA
  CPDFXFA_App::ReleaseInstance();
#endif  // PDF_ENABLE_XFA
  CPDF_ModuleMgr::Destroy();
  CFX_GEModule::Destroy();

  delete g_pCodecModule;
  g_pCodecModule = nullptr;

  IJS_Runtime::Destroy();
}

#ifndef _WIN32
int g_LastError;
void SetLastError(int err) {
  g_LastError = err;
}

int GetLastError() {
  return g_LastError;
}
#endif  // _WIN32

void ProcessParseError(CPDF_Parser::Error err) {
  uint32_t err_code = FPDF_ERR_SUCCESS;
  // Translate FPDFAPI error code to FPDFVIEW error code
  switch (err) {
    case CPDF_Parser::SUCCESS:
      err_code = FPDF_ERR_SUCCESS;
      break;
    case CPDF_Parser::FILE_ERROR:
      err_code = FPDF_ERR_FILE;
      break;
    case CPDF_Parser::FORMAT_ERROR:
      err_code = FPDF_ERR_FORMAT;
      break;
    case CPDF_Parser::PASSWORD_ERROR:
      err_code = FPDF_ERR_PASSWORD;
      break;
    case CPDF_Parser::HANDLER_ERROR:
      err_code = FPDF_ERR_SECURITY;
      break;
  }
  SetLastError(err_code);
}

DLLEXPORT void STDCALL FPDF_SetSandBoxPolicy(FPDF_DWORD policy,
                                             FPDF_BOOL enable) {
  return FSDK_SetSandBoxPolicy(policy, enable);
}

DLLEXPORT FPDF_DOCUMENT STDCALL FPDF_LoadDocument(FPDF_STRING file_path,
                                                  FPDF_BYTESTRING password) {
  // NOTE: the creation of the file needs to be by the embedder on the
  // other side of this API.
  IFX_FileRead* pFileAccess = FX_CreateFileRead((const FX_CHAR*)file_path);
  if (!pFileAccess) {
    return nullptr;
  }

  CPDF_Parser* pParser = new CPDF_Parser;
  pParser->SetPassword(password);

  CPDF_Parser::Error error = pParser->StartParse(pFileAccess);
  if (error != CPDF_Parser::SUCCESS) {
    delete pParser;
    ProcessParseError(error);
    return nullptr;
  }
#ifdef PDF_ENABLE_XFA
  CPDF_Document* pPDFDoc = pParser->GetDocument();
  if (!pPDFDoc)
    return nullptr;

  CPDFXFA_App* pProvider = CPDFXFA_App::GetInstance();
  return new CPDFXFA_Document(pPDFDoc, pProvider);
#else   // PDF_ENABLE_XFA
  return pParser->GetDocument();
#endif  // PDF_ENABLE_XFA
}

#ifdef PDF_ENABLE_XFA
DLLEXPORT FPDF_BOOL STDCALL FPDF_HasXFAField(FPDF_DOCUMENT document,
                                             int* docType) {
  if (!document)
    return FALSE;

  CPDF_Document* pdfDoc =
      (static_cast<CPDFXFA_Document*>(document))->GetPDFDoc();
  if (!pdfDoc)
    return FALSE;

  CPDF_Dictionary* pRoot = pdfDoc->GetRoot();
  if (!pRoot)
    return FALSE;

  CPDF_Dictionary* pAcroForm = pRoot->GetDictBy("AcroForm");
  if (!pAcroForm)
    return FALSE;

  CPDF_Object* pXFA = pAcroForm->GetObjectBy("XFA");
  if (!pXFA)
    return FALSE;

  bool bDynamicXFA = pRoot->GetBooleanBy("NeedsRendering", false);
  *docType = bDynamicXFA ? DOCTYPE_DYNAMIC_XFA : DOCTYPE_STATIC_XFA;
  return TRUE;
}

DLLEXPORT FPDF_BOOL STDCALL FPDF_LoadXFA(FPDF_DOCUMENT document) {
  return document && (static_cast<CPDFXFA_Document*>(document))->LoadXFADoc();
}
#endif  // PDF_ENABLE_XFA

class CMemFile final : public IFX_FileRead {
 public:
  CMemFile(uint8_t* pBuf, FX_FILESIZE size) : m_pBuf(pBuf), m_size(size) {}

  void Release() override { delete this; }
  FX_FILESIZE GetSize() override { return m_size; }
  FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) override {
    if (offset < 0) {
      return FALSE;
    }
    FX_SAFE_FILESIZE newPos =
        pdfium::base::checked_cast<FX_FILESIZE, size_t>(size);
    newPos += offset;
    if (!newPos.IsValid() || newPos.ValueOrDie() > m_size) {
      return FALSE;
    }
    FXSYS_memcpy(buffer, m_pBuf + offset, size);
    return TRUE;
  }

 private:
  ~CMemFile() override {}

  uint8_t* const m_pBuf;
  const FX_FILESIZE m_size;
};

DLLEXPORT FPDF_DOCUMENT STDCALL FPDF_LoadMemDocument(const void* data_buf,
                                                     int size,
                                                     FPDF_BYTESTRING password) {
  CPDF_Parser* pParser = new CPDF_Parser;
  pParser->SetPassword(password);
  CMemFile* pMemFile = new CMemFile((uint8_t*)data_buf, size);
  CPDF_Parser::Error error = pParser->StartParse(pMemFile);
  if (error != CPDF_Parser::SUCCESS) {
    delete pParser;
    ProcessParseError(error);
    return nullptr;
  }
  CPDF_Document* pDoc = pParser ? pParser->GetDocument() : nullptr;
  CheckUnSupportError(pDoc, error);
  return FPDFDocumentFromCPDFDocument(pParser->GetDocument());
}

DLLEXPORT FPDF_DOCUMENT STDCALL
FPDF_LoadCustomDocument(FPDF_FILEACCESS* pFileAccess,
                        FPDF_BYTESTRING password) {
  CPDF_Parser* pParser = new CPDF_Parser;
  pParser->SetPassword(password);
  CPDF_CustomAccess* pFile = new CPDF_CustomAccess(pFileAccess);
  CPDF_Parser::Error error = pParser->StartParse(pFile);
  if (error != CPDF_Parser::SUCCESS) {
    delete pParser;
    ProcessParseError(error);
    return nullptr;
  }
  CPDF_Document* pDoc = pParser ? pParser->GetDocument() : nullptr;
  CheckUnSupportError(pDoc, error);
  return FPDFDocumentFromCPDFDocument(pParser->GetDocument());
}

DLLEXPORT FPDF_BOOL STDCALL FPDF_GetFileVersion(FPDF_DOCUMENT doc,
                                                int* fileVersion) {
  if (!fileVersion)
    return FALSE;

  *fileVersion = 0;
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(doc);
  if (!pDoc)
    return FALSE;

  CPDF_Parser* pParser = pDoc->GetParser();
  if (!pParser)
    return FALSE;

  *fileVersion = pParser->GetFileVersion();
  return TRUE;
}

// jabdelmalek: changed return type from uint32_t to build on Linux (and match
// header).
DLLEXPORT unsigned long STDCALL FPDF_GetDocPermissions(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  // https://bugs.chromium.org/p/pdfium/issues/detail?id=499
  if (!pDoc) {
#ifndef PDF_ENABLE_XFA
    return 0;
#else   // PDF_ENABLE_XFA
    return 0xFFFFFFFF;
#endif  // PDF_ENABLE_XFA
  }

  return pDoc->GetUserPermissions();
}

DLLEXPORT int STDCALL FPDF_GetSecurityHandlerRevision(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc || !pDoc->GetParser())
    return -1;

  CPDF_Dictionary* pDict = pDoc->GetParser()->GetEncryptDict();
  return pDict ? pDict->GetIntegerBy("R") : -1;
}

DLLEXPORT int STDCALL FPDF_GetPageCount(FPDF_DOCUMENT document) {
  UnderlyingDocumentType* pDoc = UnderlyingFromFPDFDocument(document);
  return pDoc ? pDoc->GetPageCount() : 0;
}

DLLEXPORT FPDF_PAGE STDCALL FPDF_LoadPage(FPDF_DOCUMENT document,
                                          int page_index) {
  UnderlyingDocumentType* pDoc = UnderlyingFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  if (page_index < 0 || page_index >= pDoc->GetPageCount())
    return nullptr;

#ifdef PDF_ENABLE_XFA
  return pDoc->GetPage(page_index);
#else   // PDF_ENABLE_XFA
  CPDF_Dictionary* pDict = pDoc->GetPage(page_index);
  if (!pDict)
    return nullptr;

  CPDF_Page* pPage = new CPDF_Page(pDoc, pDict, true);
  pPage->ParseContent();
  return pPage;
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT double STDCALL FPDF_GetPageWidth(FPDF_PAGE page) {
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  return pPage ? pPage->GetPageWidth() : 0.0;
}

DLLEXPORT double STDCALL FPDF_GetPageHeight(FPDF_PAGE page) {
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  return pPage ? pPage->GetPageHeight() : 0.0;
}

#if defined(_WIN32)
DLLEXPORT void STDCALL FPDF_RenderPage(HDC dc,
                                       FPDF_PAGE page,
                                       int start_x,
                                       int start_y,
                                       int size_x,
                                       int size_y,
                                       int rotate,
                                       int flags) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return;

  CRenderContext* pContext = new CRenderContext;
  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>(pContext));

  CFX_DIBitmap* pBitmap = nullptr;
  FX_BOOL bBackgroundAlphaNeeded = pPage->BackgroundAlphaNeeded();
  FX_BOOL bHasImageMask = pPage->HasImageMask();
  if (bBackgroundAlphaNeeded || bHasImageMask) {
    pBitmap = new CFX_DIBitmap;
    pBitmap->Create(size_x, size_y, FXDIB_Argb);
    pBitmap->Clear(0x00ffffff);
    CFX_FxgeDevice* pDevice = new CFX_FxgeDevice;
    pContext->m_pDevice = pDevice;
    pDevice->Attach(pBitmap, false, nullptr, false);
  } else {
    pContext->m_pDevice = new CFX_WindowsDevice(dc);
  }

  FPDF_RenderPage_Retail(pContext, page, start_x, start_y, size_x, size_y,
                         rotate, flags, TRUE, nullptr);

  if (bBackgroundAlphaNeeded || bHasImageMask) {
    if (pBitmap) {
      CFX_WindowsDevice WinDC(dc);

      if (WinDC.GetDeviceCaps(FXDC_DEVICE_CLASS) == FXDC_PRINTER) {
        CFX_DIBitmap* pDst = new CFX_DIBitmap;
        int pitch = pBitmap->GetPitch();
        pDst->Create(size_x, size_y, FXDIB_Rgb32);
        FXSYS_memset(pDst->GetBuffer(), -1, pitch * size_y);
        pDst->CompositeBitmap(0, 0, size_x, size_y, pBitmap, 0, 0,
                              FXDIB_BLEND_NORMAL, nullptr, FALSE, nullptr);
        WinDC.StretchDIBits(pDst, 0, 0, size_x, size_y);
        delete pDst;
      } else {
        WinDC.SetDIBits(pBitmap, 0, 0);
      }
    }
  }
  if (bBackgroundAlphaNeeded || bHasImageMask)
    delete pBitmap;

  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>());
}
#endif  // defined(_WIN32)

DLLEXPORT void STDCALL FPDF_RenderPageBitmap(FPDF_BITMAP bitmap,
                                             FPDF_PAGE page,
                                             int start_x,
                                             int start_y,
                                             int size_x,
                                             int size_y,
                                             int rotate,
                                             int flags) {
  if (!bitmap)
    return;

  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return;

  CRenderContext* pContext = new CRenderContext;
  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>(pContext));
  CFX_FxgeDevice* pDevice = new CFX_FxgeDevice;
  pContext->m_pDevice = pDevice;
  CFX_DIBitmap* pBitmap = CFXBitmapFromFPDFBitmap(bitmap);
  pDevice->Attach(pBitmap, !!(flags & FPDF_REVERSE_BYTE_ORDER), nullptr, false);

  FPDF_RenderPage_Retail(pContext, page, start_x, start_y, size_x, size_y,
                         rotate, flags, TRUE, nullptr);

  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>());
}

#ifdef _SKIA_SUPPORT_
DLLEXPORT FPDF_RECORDER STDCALL FPDF_RenderPageSkp(FPDF_PAGE page,
                                                   int size_x,
                                                   int size_y) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return nullptr;

  CRenderContext* pContext = new CRenderContext;
  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>(pContext));
  CFX_FxgeDevice* skDevice = new CFX_FxgeDevice;
  FPDF_RECORDER recorder = skDevice->CreateRecorder(size_x, size_y);
  pContext->m_pDevice = skDevice;
  FPDF_RenderPage_Retail(pContext, page, 0, 0, size_x, size_y, 0, 0, TRUE,
                         nullptr);
  pPage->SetRenderContext(std::unique_ptr<CFX_Deletable>());
  return recorder;
}
#endif

DLLEXPORT void STDCALL FPDF_ClosePage(FPDF_PAGE page) {
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  if (!page)
    return;
#ifdef PDF_ENABLE_XFA
  pPage->Release();
#else   // PDF_ENABLE_XFA
  CPDFSDK_PageView* pPageView =
      static_cast<CPDFSDK_PageView*>(pPage->GetView());
  if (pPageView && pPageView->IsLocked()) {
    pPageView->TakeOverPage();
    return;
  }
  delete pPage;
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT void STDCALL FPDF_CloseDocument(FPDF_DOCUMENT document) {
#ifdef PDF_ENABLE_XFA
  delete UnderlyingFromFPDFDocument(document);
#else   // PDF_ENABLE_XFA
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return;
  CPDF_Parser* pParser = pDoc->GetParser();
  if (!pParser) {
    delete pDoc;
    return;
  }
  delete pParser;
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT unsigned long STDCALL FPDF_GetLastError() {
  return GetLastError();
}

DLLEXPORT void STDCALL FPDF_DeviceToPage(FPDF_PAGE page,
                                         int start_x,
                                         int start_y,
                                         int size_x,
                                         int size_y,
                                         int rotate,
                                         int device_x,
                                         int device_y,
                                         double* page_x,
                                         double* page_y) {
  if (!page || !page_x || !page_y)
    return;
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
#ifdef PDF_ENABLE_XFA
  pPage->DeviceToPage(start_x, start_y, size_x, size_y, rotate, device_x,
                      device_y, page_x, page_y);
#else   // PDF_ENABLE_XFA
  CFX_Matrix page2device;
  pPage->GetDisplayMatrix(page2device, start_x, start_y, size_x, size_y,
                          rotate);
  CFX_Matrix device2page;
  device2page.SetReverse(page2device);
  FX_FLOAT page_x_f, page_y_f;
  device2page.Transform((FX_FLOAT)(device_x), (FX_FLOAT)(device_y), page_x_f,
                        page_y_f);
  *page_x = (page_x_f);
  *page_y = (page_y_f);
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT void STDCALL FPDF_PageToDevice(FPDF_PAGE page,
                                         int start_x,
                                         int start_y,
                                         int size_x,
                                         int size_y,
                                         int rotate,
                                         double page_x,
                                         double page_y,
                                         int* device_x,
                                         int* device_y) {
  if (!device_x || !device_y)
    return;
  UnderlyingPageType* pPage = UnderlyingFromFPDFPage(page);
  if (!pPage)
    return;
#ifdef PDF_ENABLE_XFA
  pPage->PageToDevice(start_x, start_y, size_x, size_y, rotate, page_x, page_y,
                      device_x, device_y);
#else   // PDF_ENABLE_XFA
  CFX_Matrix page2device;
  pPage->GetDisplayMatrix(page2device, start_x, start_y, size_x, size_y,
                          rotate);
  FX_FLOAT device_x_f, device_y_f;
  page2device.Transform(((FX_FLOAT)page_x), ((FX_FLOAT)page_y), device_x_f,
                        device_y_f);
  *device_x = FXSYS_round(device_x_f);
  *device_y = FXSYS_round(device_y_f);
#endif  // PDF_ENABLE_XFA
}

DLLEXPORT FPDF_BITMAP STDCALL FPDFBitmap_Create(int width,
                                                int height,
                                                int alpha) {
  std::unique_ptr<CFX_DIBitmap> pBitmap(new CFX_DIBitmap);
  if (!pBitmap->Create(width, height, alpha ? FXDIB_Argb : FXDIB_Rgb32)) {
    return nullptr;
  }
  return pBitmap.release();
}

DLLEXPORT FPDF_BITMAP STDCALL FPDFBitmap_CreateEx(int width,
                                                  int height,
                                                  int format,
                                                  void* first_scan,
                                                  int stride) {
  FXDIB_Format fx_format;
  switch (format) {
    case FPDFBitmap_Gray:
      fx_format = FXDIB_8bppRgb;
      break;
    case FPDFBitmap_BGR:
      fx_format = FXDIB_Rgb;
      break;
    case FPDFBitmap_BGRx:
      fx_format = FXDIB_Rgb32;
      break;
    case FPDFBitmap_BGRA:
      fx_format = FXDIB_Argb;
      break;
    default:
      return nullptr;
  }
  CFX_DIBitmap* pBitmap = new CFX_DIBitmap;
  pBitmap->Create(width, height, fx_format, (uint8_t*)first_scan, stride);
  return pBitmap;
}

DLLEXPORT void STDCALL FPDFBitmap_FillRect(FPDF_BITMAP bitmap,
                                           int left,
                                           int top,
                                           int width,
                                           int height,
                                           FPDF_DWORD color) {
  if (!bitmap)
    return;

  CFX_FxgeDevice device;
  CFX_DIBitmap* pBitmap = CFXBitmapFromFPDFBitmap(bitmap);
  device.Attach(pBitmap, false, nullptr, false);
  if (!pBitmap->HasAlpha())
    color |= 0xFF000000;
  FX_RECT rect(left, top, left + width, top + height);
  device.FillRect(&rect, color);
}

DLLEXPORT void* STDCALL FPDFBitmap_GetBuffer(FPDF_BITMAP bitmap) {
  return bitmap ? CFXBitmapFromFPDFBitmap(bitmap)->GetBuffer() : nullptr;
}

DLLEXPORT int STDCALL FPDFBitmap_GetWidth(FPDF_BITMAP bitmap) {
  return bitmap ? CFXBitmapFromFPDFBitmap(bitmap)->GetWidth() : 0;
}

DLLEXPORT int STDCALL FPDFBitmap_GetHeight(FPDF_BITMAP bitmap) {
  return bitmap ? CFXBitmapFromFPDFBitmap(bitmap)->GetHeight() : 0;
}

DLLEXPORT int STDCALL FPDFBitmap_GetStride(FPDF_BITMAP bitmap) {
  return bitmap ? CFXBitmapFromFPDFBitmap(bitmap)->GetPitch() : 0;
}

DLLEXPORT void STDCALL FPDFBitmap_Destroy(FPDF_BITMAP bitmap) {
  delete CFXBitmapFromFPDFBitmap(bitmap);
}

void FPDF_RenderPage_Retail(CRenderContext* pContext,
                            FPDF_PAGE page,
                            int start_x,
                            int start_y,
                            int size_x,
                            int size_y,
                            int rotate,
                            int flags,
                            FX_BOOL bNeedToRestore,
                            IFSDK_PAUSE_Adapter* pause) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return;

  if (!pContext->m_pOptions)
    pContext->m_pOptions = new CPDF_RenderOptions;

  if (flags & FPDF_LCD_TEXT)
    pContext->m_pOptions->m_Flags |= RENDER_CLEARTYPE;
  else
    pContext->m_pOptions->m_Flags &= ~RENDER_CLEARTYPE;
  if (flags & FPDF_NO_NATIVETEXT)
    pContext->m_pOptions->m_Flags |= RENDER_NO_NATIVETEXT;
  if (flags & FPDF_RENDER_LIMITEDIMAGECACHE)
    pContext->m_pOptions->m_Flags |= RENDER_LIMITEDIMAGECACHE;
  if (flags & FPDF_RENDER_FORCEHALFTONE)
    pContext->m_pOptions->m_Flags |= RENDER_FORCE_HALFTONE;
#ifndef PDF_ENABLE_XFA
  if (flags & FPDF_RENDER_NO_SMOOTHTEXT)
    pContext->m_pOptions->m_Flags |= RENDER_NOTEXTSMOOTH;
  if (flags & FPDF_RENDER_NO_SMOOTHIMAGE)
    pContext->m_pOptions->m_Flags |= RENDER_NOIMAGESMOOTH;
  if (flags & FPDF_RENDER_NO_SMOOTHPATH)
    pContext->m_pOptions->m_Flags |= RENDER_NOPATHSMOOTH;
#endif  // PDF_ENABLE_XFA
  // Grayscale output
  if (flags & FPDF_GRAYSCALE) {
    pContext->m_pOptions->m_ColorMode = RENDER_COLOR_GRAY;
    pContext->m_pOptions->m_ForeColor = 0;
    pContext->m_pOptions->m_BackColor = 0xffffff;
  }
  const CPDF_OCContext::UsageType usage =
      (flags & FPDF_PRINTING) ? CPDF_OCContext::Print : CPDF_OCContext::View;
  pContext->m_pOptions->m_AddFlags = flags >> 8;
  pContext->m_pOptions->m_pOCContext =
      new CPDF_OCContext(pPage->m_pDocument, usage);

  CFX_Matrix matrix;
  pPage->GetDisplayMatrix(matrix, start_x, start_y, size_x, size_y, rotate);

  pContext->m_pDevice->SaveState();
  pContext->m_pDevice->SetClip_Rect(
      FX_RECT(start_x, start_y, start_x + size_x, start_y + size_y));

  pContext->m_pContext = new CPDF_RenderContext(pPage);
  pContext->m_pContext->AppendLayer(pPage, &matrix);

  if (flags & FPDF_ANNOT) {
    pContext->m_pAnnots = new CPDF_AnnotList(pPage);
    FX_BOOL bPrinting = pContext->m_pDevice->GetDeviceClass() != FXDC_DISPLAY;
    pContext->m_pAnnots->DisplayAnnots(pPage, pContext->m_pContext, bPrinting,
                                       &matrix, TRUE, nullptr);
  }

  pContext->m_pRenderer = new CPDF_ProgressiveRenderer(
      pContext->m_pContext, pContext->m_pDevice, pContext->m_pOptions);
  pContext->m_pRenderer->Start(pause);
  if (bNeedToRestore)
    pContext->m_pDevice->RestoreState(false);
}

DLLEXPORT int STDCALL FPDF_GetPageSizeByIndex(FPDF_DOCUMENT document,
                                              int page_index,
                                              double* width,
                                              double* height) {
  UnderlyingDocumentType* pDoc = UnderlyingFromFPDFDocument(document);
  if (!pDoc)
    return FALSE;

#ifdef PDF_ENABLE_XFA
  int count = pDoc->GetPageCount();
  if (page_index < 0 || page_index >= count)
    return FALSE;
  CPDFXFA_Page* pPage = pDoc->GetPage(page_index);
  if (!pPage)
    return FALSE;
  *width = pPage->GetPageWidth();
  *height = pPage->GetPageHeight();
#else   // PDF_ENABLE_XFA
  CPDF_Dictionary* pDict = pDoc->GetPage(page_index);
  if (!pDict)
    return FALSE;

  CPDF_Page page(pDoc, pDict, true);
  *width = page.GetPageWidth();
  *height = page.GetPageHeight();
#endif  // PDF_ENABLE_XFA

  return TRUE;
}

DLLEXPORT FPDF_BOOL STDCALL
FPDF_VIEWERREF_GetPrintScaling(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return TRUE;
  CPDF_ViewerPreferences viewRef(pDoc);
  return viewRef.PrintScaling();
}

DLLEXPORT int STDCALL FPDF_VIEWERREF_GetNumCopies(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 1;
  CPDF_ViewerPreferences viewRef(pDoc);
  return viewRef.NumCopies();
}

DLLEXPORT FPDF_PAGERANGE STDCALL
FPDF_VIEWERREF_GetPrintPageRange(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;
  CPDF_ViewerPreferences viewRef(pDoc);
  return viewRef.PrintPageRange();
}

DLLEXPORT FPDF_DUPLEXTYPE STDCALL
FPDF_VIEWERREF_GetDuplex(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return DuplexUndefined;
  CPDF_ViewerPreferences viewRef(pDoc);
  CFX_ByteString duplex = viewRef.Duplex();
  if ("Simplex" == duplex)
    return Simplex;
  if ("DuplexFlipShortEdge" == duplex)
    return DuplexFlipShortEdge;
  if ("DuplexFlipLongEdge" == duplex)
    return DuplexFlipLongEdge;
  return DuplexUndefined;
}

DLLEXPORT FPDF_DWORD STDCALL FPDF_CountNamedDests(FPDF_DOCUMENT document) {
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return 0;

  CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return 0;

  CPDF_NameTree nameTree(pDoc, "Dests");
  pdfium::base::CheckedNumeric<FPDF_DWORD> count = nameTree.GetCount();
  CPDF_Dictionary* pDest = pRoot->GetDictBy("Dests");
  if (pDest)
    count += pDest->GetCount();

  if (!count.IsValid())
    return 0;

  return count.ValueOrDie();
}

DLLEXPORT FPDF_DEST STDCALL FPDF_GetNamedDestByName(FPDF_DOCUMENT document,
                                                    FPDF_BYTESTRING name) {
  if (!name || name[0] == 0)
    return nullptr;

  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  CPDF_NameTree name_tree(pDoc, "Dests");
  return name_tree.LookupNamedDest(pDoc, name);
}

#ifdef PDF_ENABLE_XFA
DLLEXPORT FPDF_RESULT STDCALL FPDF_BStr_Init(FPDF_BSTR* str) {
  if (!str)
    return -1;

  FXSYS_memset(str, 0, sizeof(FPDF_BSTR));
  return 0;
}

DLLEXPORT FPDF_RESULT STDCALL FPDF_BStr_Set(FPDF_BSTR* str,
                                            FPDF_LPCSTR bstr,
                                            int length) {
  if (!str)
    return -1;
  if (!bstr || !length)
    return -1;
  if (length == -1)
    length = FXSYS_strlen(bstr);

  if (length == 0) {
    if (str->str) {
      FX_Free(str->str);
      str->str = nullptr;
    }
    str->len = 0;
    return 0;
  }

  if (str->str && str->len < length)
    str->str = FX_Realloc(char, str->str, length + 1);
  else if (!str->str)
    str->str = FX_Alloc(char, length + 1);

  str->str[length] = 0;
  FXSYS_memcpy(str->str, bstr, length);
  str->len = length;

  return 0;
}

DLLEXPORT FPDF_RESULT STDCALL FPDF_BStr_Clear(FPDF_BSTR* str) {
  if (!str)
    return -1;

  if (str->str) {
    FX_Free(str->str);
    str->str = nullptr;
  }
  str->len = 0;
  return 0;
}
#endif  // PDF_ENABLE_XFA

DLLEXPORT FPDF_DEST STDCALL FPDF_GetNamedDest(FPDF_DOCUMENT document,
                                              int index,
                                              void* buffer,
                                              long* buflen) {
  if (!buffer)
    *buflen = 0;

  if (index < 0)
    return nullptr;

  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document);
  if (!pDoc)
    return nullptr;

  CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return nullptr;

  CPDF_Object* pDestObj = nullptr;
  CFX_ByteString bsName;
  CPDF_NameTree nameTree(pDoc, "Dests");
  int count = nameTree.GetCount();
  if (index >= count) {
    CPDF_Dictionary* pDest = pRoot->GetDictBy("Dests");
    if (!pDest)
      return nullptr;

    pdfium::base::CheckedNumeric<int> checked_count = count;
    checked_count += pDest->GetCount();
    if (!checked_count.IsValid() || index >= checked_count.ValueOrDie())
      return nullptr;

    index -= count;
    int i = 0;
    for (const auto& it : *pDest) {
      bsName = it.first;
      pDestObj = it.second;
      if (!pDestObj)
        continue;
      if (i == index)
        break;
      i++;
    }
  } else {
    pDestObj = nameTree.LookupValue(index, bsName);
  }
  if (!pDestObj)
    return nullptr;
  if (CPDF_Dictionary* pDict = pDestObj->AsDictionary()) {
    pDestObj = pDict->GetArrayBy("D");
    if (!pDestObj)
      return nullptr;
  }
  if (!pDestObj->IsArray())
    return nullptr;

  CFX_WideString wsName = PDF_DecodeText(bsName);
  CFX_ByteString utf16Name = wsName.UTF16LE_Encode();
  int len = utf16Name.GetLength();
  if (!buffer) {
    *buflen = len;
  } else if (*buflen >= len) {
    memcpy(buffer, utf16Name.c_str(), len);
    *buflen = len;
  } else {
    *buflen = -1;
  }
  return (FPDF_DEST)pDestObj;
}
