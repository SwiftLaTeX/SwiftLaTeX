// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_data_avail.h"

#include <algorithm>

#include "core/fpdfapi/fpdf_parser/cpdf_hint_tables.h"
#include "core/fpdfapi/fpdf_parser/fpdf_parser_utility.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "third_party/base/stl_util.h"

CPDF_DataAvail::FileAvail::~FileAvail() {}

CPDF_DataAvail::DownloadHints::~DownloadHints() {}

// static
int CPDF_DataAvail::s_CurrentDataAvailRecursionDepth = 0;

CPDF_DataAvail::CPDF_DataAvail(FileAvail* pFileAvail,
                               IFX_FileRead* pFileRead,
                               FX_BOOL bSupportHintTable)
    : m_pFileAvail(pFileAvail), m_pFileRead(pFileRead) {
  m_Pos = 0;
  m_dwFileLen = 0;
  if (m_pFileRead) {
    m_dwFileLen = (uint32_t)m_pFileRead->GetSize();
  }
  m_dwCurrentOffset = 0;
  m_dwXRefOffset = 0;
  m_bufferOffset = 0;
  m_dwFirstPageNo = 0;
  m_bufferSize = 0;
  m_PagesObjNum = 0;
  m_dwCurrentXRefSteam = 0;
  m_dwAcroFormObjNum = 0;
  m_dwInfoObjNum = 0;
  m_pDocument = 0;
  m_dwEncryptObjNum = 0;
  m_dwPrevXRefOffset = 0;
  m_dwLastXRefOffset = 0;
  m_bDocAvail = FALSE;
  m_bMainXRefLoadTried = FALSE;
  m_bDocAvail = FALSE;
  m_bLinearized = FALSE;
  m_bPagesLoad = FALSE;
  m_bPagesTreeLoad = FALSE;
  m_bMainXRefLoadedOK = FALSE;
  m_bAnnotsLoad = FALSE;
  m_bHaveAcroForm = FALSE;
  m_bAcroFormLoad = FALSE;
  m_bPageLoadedOK = FALSE;
  m_bNeedDownLoadResource = FALSE;
  m_bLinearizedFormParamLoad = FALSE;
  m_pLinearized = nullptr;
  m_pRoot = nullptr;
  m_pTrailer = nullptr;
  m_pCurrentParser = nullptr;
  m_pAcroForm = nullptr;
  m_pPageDict = nullptr;
  m_pPageResource = nullptr;
  m_docStatus = PDF_DATAAVAIL_HEADER;
  m_parser.m_bOwnFileRead = false;
  m_bTotalLoadPageTree = FALSE;
  m_bCurPageDictLoadOK = FALSE;
  m_bLinearedDataOK = FALSE;
  m_bSupportHintTable = bSupportHintTable;
}
CPDF_DataAvail::~CPDF_DataAvail() {
  m_pHintTables.reset();
  if (m_pLinearized)
    m_pLinearized->Release();

  if (m_pRoot)
    m_pRoot->Release();

  if (m_pTrailer)
    m_pTrailer->Release();

  int iSize = m_arrayAcroforms.GetSize();
  for (int i = 0; i < iSize; ++i)
    m_arrayAcroforms.GetAt(i)->Release();
}

void CPDF_DataAvail::SetDocument(CPDF_Document* pDoc) {
  m_pDocument = pDoc;
}

uint32_t CPDF_DataAvail::GetObjectSize(uint32_t objnum, FX_FILESIZE& offset) {
  CPDF_Parser* pParser = m_pDocument->GetParser();
  if (!pParser || !pParser->IsValidObjectNumber(objnum))
    return 0;

  if (pParser->GetObjectType(objnum) == 2)
    objnum = pParser->GetObjectPositionOrZero(objnum);

  if (pParser->GetObjectType(objnum) != 1 &&
      pParser->GetObjectType(objnum) != 255) {
    return 0;
  }

  offset = pParser->GetObjectPositionOrZero(objnum);
  if (offset == 0)
    return 0;

  auto it = pParser->m_SortedOffset.find(offset);
  if (it == pParser->m_SortedOffset.end() ||
      ++it == pParser->m_SortedOffset.end()) {
    return 0;
  }
  return *it - offset;
}

FX_BOOL CPDF_DataAvail::IsObjectsAvail(
    CFX_ArrayTemplate<CPDF_Object*>& obj_array,
    FX_BOOL bParsePage,
    DownloadHints* pHints,
    CFX_ArrayTemplate<CPDF_Object*>& ret_array) {
  if (!obj_array.GetSize())
    return TRUE;

  uint32_t count = 0;
  CFX_ArrayTemplate<CPDF_Object*> new_obj_array;
  for (int i = 0; i < obj_array.GetSize(); i++) {
    CPDF_Object* pObj = obj_array[i];
    if (!pObj)
      continue;

    int32_t type = pObj->GetType();
    switch (type) {
      case CPDF_Object::ARRAY: {
        CPDF_Array* pArray = pObj->AsArray();
        for (size_t k = 0; k < pArray->GetCount(); ++k)
          new_obj_array.Add(pArray->GetObjectAt(k));
      } break;
      case CPDF_Object::STREAM:
        pObj = pObj->GetDict();
      case CPDF_Object::DICTIONARY: {
        CPDF_Dictionary* pDict = pObj->GetDict();
        if (pDict && pDict->GetStringBy("Type") == "Page" && !bParsePage)
          continue;

        for (const auto& it : *pDict) {
          const CFX_ByteString& key = it.first;
          CPDF_Object* value = it.second;
          if (key != "Parent")
            new_obj_array.Add(value);
        }
      } break;
      case CPDF_Object::REFERENCE: {
        CPDF_Reference* pRef = pObj->AsReference();
        uint32_t dwNum = pRef->GetRefObjNum();

        FX_FILESIZE offset;
        uint32_t size = GetObjectSize(dwNum, offset);
        if (size == 0 || offset < 0 || offset >= m_dwFileLen)
          break;

        if (!IsDataAvail(offset, size, pHints)) {
          ret_array.Add(pObj);
          count++;
        } else if (!pdfium::ContainsKey(m_ObjectSet, dwNum)) {
          m_ObjectSet.insert(dwNum);
          CPDF_Object* pReferred =
              m_pDocument->GetIndirectObject(pRef->GetRefObjNum());
          if (pReferred)
            new_obj_array.Add(pReferred);
        }
      } break;
    }
  }

  if (count > 0) {
    for (int i = 0; i < new_obj_array.GetSize(); ++i) {
      CPDF_Object* pObj = new_obj_array[i];
      if (CPDF_Reference* pRef = pObj->AsReference()) {
        uint32_t dwNum = pRef->GetRefObjNum();
        if (!pdfium::ContainsKey(m_ObjectSet, dwNum))
          ret_array.Add(pObj);
      } else {
        ret_array.Add(pObj);
      }
    }
    return FALSE;
  }

  obj_array.RemoveAll();
  obj_array.Append(new_obj_array);
  return IsObjectsAvail(obj_array, FALSE, pHints, ret_array);
}

CPDF_DataAvail::DocAvailStatus CPDF_DataAvail::IsDocAvail(
    DownloadHints* pHints) {
  if (!m_dwFileLen && m_pFileRead) {
    m_dwFileLen = (uint32_t)m_pFileRead->GetSize();
    if (!m_dwFileLen)
      return DataError;
  }

  while (!m_bDocAvail) {
    if (!CheckDocStatus(pHints))
      return DataNotAvailable;
  }

  return DataAvailable;
}

FX_BOOL CPDF_DataAvail::CheckAcroFormSubObject(DownloadHints* pHints) {
  if (!m_objs_array.GetSize()) {
    m_objs_array.RemoveAll();
    m_ObjectSet.clear();
    CFX_ArrayTemplate<CPDF_Object*> obj_array;
    obj_array.Append(m_arrayAcroforms);
    FX_BOOL bRet = IsObjectsAvail(obj_array, FALSE, pHints, m_objs_array);
    if (bRet)
      m_objs_array.RemoveAll();
    return bRet;
  }

  CFX_ArrayTemplate<CPDF_Object*> new_objs_array;
  FX_BOOL bRet = IsObjectsAvail(m_objs_array, FALSE, pHints, new_objs_array);
  if (bRet) {
    int32_t iSize = m_arrayAcroforms.GetSize();
    for (int32_t i = 0; i < iSize; ++i) {
      m_arrayAcroforms.GetAt(i)->Release();
    }
    m_arrayAcroforms.RemoveAll();
  } else {
    m_objs_array.RemoveAll();
    m_objs_array.Append(new_objs_array);
  }
  return bRet;
}

FX_BOOL CPDF_DataAvail::CheckAcroForm(DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  m_pAcroForm = GetObject(m_dwAcroFormObjNum, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_PAGETREE;
    return TRUE;
  }

  if (!m_pAcroForm) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR) {
      m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      return TRUE;
    }
    return FALSE;
  }

  m_arrayAcroforms.Add(m_pAcroForm);
  m_docStatus = PDF_DATAAVAIL_PAGETREE;
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckDocStatus(DownloadHints* pHints) {
  switch (m_docStatus) {
    case PDF_DATAAVAIL_HEADER:
      return CheckHeader(pHints);
    case PDF_DATAAVAIL_FIRSTPAGE:
    case PDF_DATAAVAIL_FIRSTPAGE_PREPARE:
      return CheckFirstPage(pHints);
    case PDF_DATAAVAIL_HINTTABLE:
      return CheckHintTables(pHints);
    case PDF_DATAAVAIL_END:
      return CheckEnd(pHints);
    case PDF_DATAAVAIL_CROSSREF:
      return CheckCrossRef(pHints);
    case PDF_DATAAVAIL_CROSSREF_ITEM:
      return CheckCrossRefItem(pHints);
    case PDF_DATAAVAIL_CROSSREF_STREAM:
      return CheckAllCrossRefStream(pHints);
    case PDF_DATAAVAIL_TRAILER:
      return CheckTrailer(pHints);
    case PDF_DATAAVAIL_TRAILER_APPEND:
      return CheckTrailerAppend(pHints);
    case PDF_DATAAVAIL_LOADALLCROSSREF:
      return LoadAllXref(pHints);
    case PDF_DATAAVAIL_LOADALLFILE:
      return LoadAllFile(pHints);
    case PDF_DATAAVAIL_ROOT:
      return CheckRoot(pHints);
    case PDF_DATAAVAIL_INFO:
      return CheckInfo(pHints);
    case PDF_DATAAVAIL_ACROFORM:
      return CheckAcroForm(pHints);
    case PDF_DATAAVAIL_PAGETREE:
      if (m_bTotalLoadPageTree)
        return CheckPages(pHints);
      return LoadDocPages(pHints);
    case PDF_DATAAVAIL_PAGE:
      if (m_bTotalLoadPageTree)
        return CheckPage(pHints);
      m_docStatus = PDF_DATAAVAIL_PAGE_LATERLOAD;
      return TRUE;
    case PDF_DATAAVAIL_ERROR:
      return LoadAllFile(pHints);
    case PDF_DATAAVAIL_PAGE_LATERLOAD:
      m_docStatus = PDF_DATAAVAIL_PAGE;
    default:
      m_bDocAvail = TRUE;
      return TRUE;
  }
}

FX_BOOL CPDF_DataAvail::CheckPageStatus(DownloadHints* pHints) {
  switch (m_docStatus) {
    case PDF_DATAAVAIL_PAGETREE:
      return CheckPages(pHints);
    case PDF_DATAAVAIL_PAGE:
      return CheckPage(pHints);
    case PDF_DATAAVAIL_ERROR:
      return LoadAllFile(pHints);
    default:
      m_bPagesTreeLoad = TRUE;
      m_bPagesLoad = TRUE;
      return TRUE;
  }
}

FX_BOOL CPDF_DataAvail::LoadAllFile(DownloadHints* pHints) {
  if (m_pFileAvail->IsDataAvail(0, (uint32_t)m_dwFileLen)) {
    m_docStatus = PDF_DATAAVAIL_DONE;
    return TRUE;
  }

  pHints->AddSegment(0, (uint32_t)m_dwFileLen);
  return FALSE;
}

FX_BOOL CPDF_DataAvail::LoadAllXref(DownloadHints* pHints) {
  m_parser.m_pSyntax->InitParser(m_pFileRead, (uint32_t)m_dwHeaderOffset);
  m_parser.m_bOwnFileRead = false;
  if (!m_parser.LoadAllCrossRefV4(m_dwLastXRefOffset) &&
      !m_parser.LoadAllCrossRefV5(m_dwLastXRefOffset)) {
    m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
    return FALSE;
  }

  m_dwRootObjNum = m_parser.GetRootObjNum();
  m_dwInfoObjNum = m_parser.GetInfoObjNum();
  m_pCurrentParser = &m_parser;
  m_docStatus = PDF_DATAAVAIL_ROOT;
  return TRUE;
}

CPDF_Object* CPDF_DataAvail::GetObject(uint32_t objnum,
                                       DownloadHints* pHints,
                                       FX_BOOL* pExistInFile) {
  CPDF_Object* pRet = nullptr;
  uint32_t size = 0;
  FX_FILESIZE offset = 0;
  CPDF_Parser* pParser = nullptr;

  if (pExistInFile)
    *pExistInFile = TRUE;

  if (m_pDocument) {
    size = GetObjectSize(objnum, offset);
    pParser = m_pDocument->GetParser();
  } else {
    size = (uint32_t)m_parser.GetObjectSize(objnum);
    offset = m_parser.GetObjectOffset(objnum);
    pParser = &m_parser;
  }

  if (!IsDataAvail(offset, size, pHints))
    return nullptr;

  if (pParser)
    pRet = pParser->ParseIndirectObject(nullptr, objnum);

  if (!pRet && pExistInFile)
    *pExistInFile = FALSE;

  return pRet;
}

FX_BOOL CPDF_DataAvail::CheckInfo(DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  CPDF_Object* pInfo = GetObject(m_dwInfoObjNum, pHints, &bExist);
  if (!bExist) {
    m_docStatus =
        (m_bHaveAcroForm ? PDF_DATAAVAIL_ACROFORM : PDF_DATAAVAIL_PAGETREE);
    return TRUE;
  }

  if (!pInfo) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR) {
      m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      return TRUE;
    }

    if (m_Pos == m_dwFileLen)
      m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (pInfo)
    pInfo->Release();

  m_docStatus =
      (m_bHaveAcroForm ? PDF_DATAAVAIL_ACROFORM : PDF_DATAAVAIL_PAGETREE);

  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckRoot(DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  m_pRoot = GetObject(m_dwRootObjNum, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
    return TRUE;
  }

  if (!m_pRoot) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR) {
      m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      return TRUE;
    }
    return FALSE;
  }

  CPDF_Dictionary* pDict = m_pRoot->GetDict();
  if (!pDict) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  CPDF_Reference* pRef = ToReference(pDict->GetObjectBy("Pages"));
  if (!pRef) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  m_PagesObjNum = pRef->GetRefObjNum();
  CPDF_Reference* pAcroFormRef =
      ToReference(m_pRoot->GetDict()->GetObjectBy("AcroForm"));
  if (pAcroFormRef) {
    m_bHaveAcroForm = TRUE;
    m_dwAcroFormObjNum = pAcroFormRef->GetRefObjNum();
  }

  if (m_dwInfoObjNum) {
    m_docStatus = PDF_DATAAVAIL_INFO;
  } else {
    m_docStatus =
        m_bHaveAcroForm ? PDF_DATAAVAIL_ACROFORM : PDF_DATAAVAIL_PAGETREE;
  }
  return TRUE;
}

FX_BOOL CPDF_DataAvail::PreparePageItem() {
  CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
  CPDF_Reference* pRef =
      ToReference(pRoot ? pRoot->GetObjectBy("Pages") : nullptr);
  if (!pRef) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  m_PagesObjNum = pRef->GetRefObjNum();
  m_pCurrentParser = m_pDocument->GetParser();
  m_docStatus = PDF_DATAAVAIL_PAGETREE;
  return TRUE;
}

bool CPDF_DataAvail::IsFirstCheck(int iPage) {
  return m_pageMapCheckState.insert(iPage).second;
}

void CPDF_DataAvail::ResetFirstCheck(int iPage) {
  m_pageMapCheckState.erase(iPage);
}

FX_BOOL CPDF_DataAvail::CheckPage(DownloadHints* pHints) {
  uint32_t iPageObjs = m_PageObjList.GetSize();
  CFX_ArrayTemplate<uint32_t> UnavailObjList;
  for (uint32_t i = 0; i < iPageObjs; ++i) {
    uint32_t dwPageObjNum = m_PageObjList.GetAt(i);
    FX_BOOL bExist = FALSE;
    CPDF_Object* pObj = GetObject(dwPageObjNum, pHints, &bExist);
    if (!pObj) {
      if (bExist)
        UnavailObjList.Add(dwPageObjNum);
      continue;
    }

    CPDF_Array* pArray = ToArray(pObj);
    if (pArray) {
      for (CPDF_Object* pArrayObj : *pArray) {
        if (CPDF_Reference* pRef = ToReference(pArrayObj))
          UnavailObjList.Add(pRef->GetRefObjNum());
      }
    }

    if (!pObj->IsDictionary()) {
      pObj->Release();
      continue;
    }

    CFX_ByteString type = pObj->GetDict()->GetStringBy("Type");
    if (type == "Pages") {
      m_PagesArray.Add(pObj);
      continue;
    }
    pObj->Release();
  }

  m_PageObjList.RemoveAll();
  if (UnavailObjList.GetSize()) {
    m_PageObjList.Append(UnavailObjList);
    return FALSE;
  }

  uint32_t iPages = m_PagesArray.GetSize();
  for (uint32_t i = 0; i < iPages; i++) {
    CPDF_Object* pPages = m_PagesArray.GetAt(i);
    if (!pPages)
      continue;

    if (!GetPageKids(m_pCurrentParser, pPages)) {
      pPages->Release();
      while (++i < iPages) {
        pPages = m_PagesArray.GetAt(i);
        pPages->Release();
      }
      m_PagesArray.RemoveAll();

      m_docStatus = PDF_DATAAVAIL_ERROR;
      return FALSE;
    }
    pPages->Release();
  }

  m_PagesArray.RemoveAll();
  if (!m_PageObjList.GetSize())
    m_docStatus = PDF_DATAAVAIL_DONE;
  return TRUE;
}

FX_BOOL CPDF_DataAvail::GetPageKids(CPDF_Parser* pParser, CPDF_Object* pPages) {
  if (!pParser) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  CPDF_Dictionary* pDict = pPages->GetDict();
  CPDF_Object* pKids = pDict ? pDict->GetObjectBy("Kids") : nullptr;
  if (!pKids)
    return TRUE;

  switch (pKids->GetType()) {
    case CPDF_Object::REFERENCE:
      m_PageObjList.Add(pKids->AsReference()->GetRefObjNum());
      break;
    case CPDF_Object::ARRAY: {
      CPDF_Array* pKidsArray = pKids->AsArray();
      for (size_t i = 0; i < pKidsArray->GetCount(); ++i) {
        if (CPDF_Reference* pRef = ToReference(pKidsArray->GetObjectAt(i)))
          m_PageObjList.Add(pRef->GetRefObjNum());
      }
    } break;
    default:
      m_docStatus = PDF_DATAAVAIL_ERROR;
      return FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckPages(DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  CPDF_Object* pPages = GetObject(m_PagesObjNum, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
    return TRUE;
  }

  if (!pPages) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR) {
      m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      return TRUE;
    }
    return FALSE;
  }

  if (!GetPageKids(m_pCurrentParser, pPages)) {
    pPages->Release();
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  pPages->Release();
  m_docStatus = PDF_DATAAVAIL_PAGE;
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckHeader(DownloadHints* pHints) {
  ASSERT(m_dwFileLen >= 0);
  const uint32_t kReqSize = std::min(static_cast<uint32_t>(m_dwFileLen), 1024U);

  if (m_pFileAvail->IsDataAvail(0, kReqSize)) {
    uint8_t buffer[1024];
    m_pFileRead->ReadBlock(buffer, 0, kReqSize);

    if (IsLinearizedFile(buffer, kReqSize)) {
      m_docStatus = PDF_DATAAVAIL_FIRSTPAGE;
    } else {
      if (m_docStatus == PDF_DATAAVAIL_ERROR)
        return FALSE;
      m_docStatus = PDF_DATAAVAIL_END;
    }
    return TRUE;
  }

  pHints->AddSegment(0, kReqSize);
  return FALSE;
}

FX_BOOL CPDF_DataAvail::CheckFirstPage(DownloadHints* pHints) {
  CPDF_Dictionary* pDict = m_pLinearized->GetDict();
  CPDF_Object* pEndOffSet = pDict ? pDict->GetObjectBy("E") : nullptr;
  if (!pEndOffSet) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  CPDF_Object* pXRefOffset = pDict ? pDict->GetObjectBy("T") : nullptr;
  if (!pXRefOffset) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  CPDF_Object* pFileLen = pDict ? pDict->GetObjectBy("L") : nullptr;
  if (!pFileLen) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  FX_BOOL bNeedDownLoad = FALSE;
  if (pEndOffSet->IsNumber()) {
    uint32_t dwEnd = pEndOffSet->GetInteger();
    dwEnd += 512;
    if ((FX_FILESIZE)dwEnd > m_dwFileLen)
      dwEnd = (uint32_t)m_dwFileLen;

    int32_t iStartPos = (int32_t)(m_dwFileLen > 1024 ? 1024 : m_dwFileLen);
    int32_t iSize = dwEnd > 1024 ? dwEnd - 1024 : 0;
    if (!m_pFileAvail->IsDataAvail(iStartPos, iSize)) {
      pHints->AddSegment(iStartPos, iSize);
      bNeedDownLoad = TRUE;
    }
  }

  m_dwLastXRefOffset = 0;
  FX_FILESIZE dwFileLen = 0;
  if (pXRefOffset->IsNumber())
    m_dwLastXRefOffset = pXRefOffset->GetInteger();

  if (pFileLen->IsNumber())
    dwFileLen = pFileLen->GetInteger();

  if (!m_pFileAvail->IsDataAvail(m_dwLastXRefOffset,
                                 (uint32_t)(dwFileLen - m_dwLastXRefOffset))) {
    if (m_docStatus == PDF_DATAAVAIL_FIRSTPAGE) {
      uint32_t dwSize = (uint32_t)(dwFileLen - m_dwLastXRefOffset);
      FX_FILESIZE offset = m_dwLastXRefOffset;
      if (dwSize < 512 && dwFileLen > 512) {
        dwSize = 512;
        offset = dwFileLen - 512;
      }
      pHints->AddSegment(offset, dwSize);
    }
  } else {
    m_docStatus = PDF_DATAAVAIL_FIRSTPAGE_PREPARE;
  }

  if (bNeedDownLoad || m_docStatus != PDF_DATAAVAIL_FIRSTPAGE_PREPARE) {
    m_docStatus = PDF_DATAAVAIL_FIRSTPAGE_PREPARE;
    return FALSE;
  }

  m_docStatus =
      m_bSupportHintTable ? PDF_DATAAVAIL_HINTTABLE : PDF_DATAAVAIL_DONE;
  return TRUE;
}

FX_BOOL CPDF_DataAvail::IsDataAvail(FX_FILESIZE offset,
                                    uint32_t size,
                                    DownloadHints* pHints) {
  if (offset < 0 || offset > m_dwFileLen)
    return TRUE;

  FX_SAFE_FILESIZE safeSize = offset;
  safeSize += size;
  safeSize += 512;
  if (!safeSize.IsValid() || safeSize.ValueOrDie() > m_dwFileLen)
    size = m_dwFileLen - offset;
  else
    size += 512;

  if (!m_pFileAvail->IsDataAvail(offset, size)) {
    pHints->AddSegment(offset, size);
    return FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckHintTables(DownloadHints* pHints) {
  CPDF_Dictionary* pDict = m_pLinearized->GetDict();
  if (!pDict) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  // The actual value is not required here, but validate its existence and type.
  CPDF_Number* pFirstPage = ToNumber(pDict->GetDirectObjectBy("O"));
  if (!pFirstPage || !pFirstPage->IsInteger()) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  CPDF_Number* pPageCount = ToNumber(pDict->GetDirectObjectBy("N"));
  if (!pPageCount || !pPageCount->IsInteger()) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  int nPageCount = pPageCount->GetInteger();
  if (nPageCount <= 1) {
    m_docStatus = PDF_DATAAVAIL_DONE;
    return TRUE;
  }

  CPDF_Array* pHintStreamRange = pDict->GetArrayBy("H");
  size_t nHintStreamSize = pHintStreamRange ? pHintStreamRange->GetCount() : 0;
  if (nHintStreamSize != 2 && nHintStreamSize != 4) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  for (const CPDF_Object* pArrayObject : *pHintStreamRange) {
    const CPDF_Number* pNumber = ToNumber(pArrayObject->GetDirect());
    if (!pNumber || !pNumber->IsInteger()) {
      m_docStatus = PDF_DATAAVAIL_ERROR;
      return FALSE;
    }
  }

  FX_FILESIZE szHintStart = pHintStreamRange->GetIntegerAt(0);
  FX_FILESIZE szHintLength = pHintStreamRange->GetIntegerAt(1);
  if (szHintStart < 0 || szHintLength <= 0) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (!IsDataAvail(szHintStart, szHintLength, pHints))
    return FALSE;

  m_syntaxParser.InitParser(m_pFileRead, m_dwHeaderOffset);

  std::unique_ptr<CPDF_HintTables> pHintTables(
      new CPDF_HintTables(this, pDict));
  std::unique_ptr<CPDF_Object, ReleaseDeleter<CPDF_Object>> pHintStream(
      ParseIndirectObjectAt(szHintStart, 0));
  CPDF_Stream* pStream = ToStream(pHintStream.get());
  if (pStream && pHintTables->LoadHintStream(pStream))
    m_pHintTables = std::move(pHintTables);

  m_docStatus = PDF_DATAAVAIL_DONE;
  return TRUE;
}

CPDF_Object* CPDF_DataAvail::ParseIndirectObjectAt(
    FX_FILESIZE pos,
    uint32_t objnum,
    CPDF_IndirectObjectHolder* pObjList) {
  FX_FILESIZE SavedPos = m_syntaxParser.SavePos();
  m_syntaxParser.RestorePos(pos);

  bool bIsNumber;
  CFX_ByteString word = m_syntaxParser.GetNextWord(&bIsNumber);
  if (!bIsNumber)
    return nullptr;

  uint32_t parser_objnum = FXSYS_atoui(word.c_str());
  if (objnum && parser_objnum != objnum)
    return nullptr;

  word = m_syntaxParser.GetNextWord(&bIsNumber);
  if (!bIsNumber)
    return nullptr;

  uint32_t gennum = FXSYS_atoui(word.c_str());
  if (m_syntaxParser.GetKeyword() != "obj") {
    m_syntaxParser.RestorePos(SavedPos);
    return nullptr;
  }

  CPDF_Object* pObj =
      m_syntaxParser.GetObject(pObjList, parser_objnum, gennum, true);
  m_syntaxParser.RestorePos(SavedPos);
  return pObj;
}

CPDF_DataAvail::DocLinearizationStatus CPDF_DataAvail::IsLinearizedPDF() {
  const uint32_t kReqSize = 1024;
  if (!m_pFileAvail->IsDataAvail(0, kReqSize))
    return LinearizationUnknown;

  if (!m_pFileRead)
    return NotLinearized;

  FX_FILESIZE dwSize = m_pFileRead->GetSize();
  if (dwSize < (FX_FILESIZE)kReqSize)
    return LinearizationUnknown;

  uint8_t buffer[1024];
  m_pFileRead->ReadBlock(buffer, 0, kReqSize);
  if (IsLinearizedFile(buffer, kReqSize))
    return Linearized;

  return NotLinearized;
}

FX_BOOL CPDF_DataAvail::IsLinearized() {
  return m_bLinearized;
}

FX_BOOL CPDF_DataAvail::IsLinearizedFile(uint8_t* pData, uint32_t dwLen) {
  if (m_pLinearized)
    return m_bLinearized;

  ScopedFileStream file(FX_CreateMemoryStream(pData, (size_t)dwLen, FALSE));

  int32_t offset = GetHeaderOffset(file.get());
  if (offset == -1) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  m_dwHeaderOffset = offset;
  m_syntaxParser.InitParser(file.get(), offset);
  m_syntaxParser.RestorePos(m_syntaxParser.m_HeaderOffset + 9);

  bool bNumber;
  CFX_ByteString wordObjNum = m_syntaxParser.GetNextWord(&bNumber);
  if (!bNumber)
    return FALSE;

  uint32_t objnum = FXSYS_atoui(wordObjNum.c_str());
  m_pLinearized =
      ParseIndirectObjectAt(m_syntaxParser.m_HeaderOffset + 9, objnum);
  if (!m_pLinearized)
    return FALSE;

  CPDF_Dictionary* pDict = m_pLinearized->GetDict();
  if (!pDict || !pDict->GetObjectBy("Linearized"))
    return FALSE;

  CPDF_Object* pLen = pDict->GetObjectBy("L");
  if (!pLen)
    return FALSE;

  if ((FX_FILESIZE)pLen->GetInteger() != m_pFileRead->GetSize())
    return FALSE;

  m_bLinearized = TRUE;

  if (CPDF_Number* pNo = ToNumber(pDict->GetObjectBy("P")))
    m_dwFirstPageNo = pNo->GetInteger();

  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckEnd(DownloadHints* pHints) {
  uint32_t req_pos = (uint32_t)(m_dwFileLen > 1024 ? m_dwFileLen - 1024 : 0);
  uint32_t dwSize = (uint32_t)(m_dwFileLen - req_pos);

  if (m_pFileAvail->IsDataAvail(req_pos, dwSize)) {
    uint8_t buffer[1024];
    m_pFileRead->ReadBlock(buffer, req_pos, dwSize);

    ScopedFileStream file(FX_CreateMemoryStream(buffer, (size_t)dwSize, FALSE));
    m_syntaxParser.InitParser(file.get(), 0);
    m_syntaxParser.RestorePos(dwSize - 1);

    if (m_syntaxParser.SearchWord("startxref", TRUE, FALSE, dwSize)) {
      m_syntaxParser.GetNextWord(nullptr);

      bool bNumber;
      CFX_ByteString xrefpos_str = m_syntaxParser.GetNextWord(&bNumber);
      if (!bNumber) {
        m_docStatus = PDF_DATAAVAIL_ERROR;
        return FALSE;
      }

      m_dwXRefOffset = (FX_FILESIZE)FXSYS_atoi64(xrefpos_str.c_str());
      if (!m_dwXRefOffset || m_dwXRefOffset > m_dwFileLen) {
        m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
        return TRUE;
      }

      m_dwLastXRefOffset = m_dwXRefOffset;
      SetStartOffset(m_dwXRefOffset);
      m_docStatus = PDF_DATAAVAIL_CROSSREF;
      return TRUE;
    }

    m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
    return TRUE;
  }

  pHints->AddSegment(req_pos, dwSize);
  return FALSE;
}

int32_t CPDF_DataAvail::CheckCrossRefStream(DownloadHints* pHints,
                                            FX_FILESIZE& xref_offset) {
  xref_offset = 0;
  uint32_t req_size =
      (uint32_t)(m_Pos + 512 > m_dwFileLen ? m_dwFileLen - m_Pos : 512);

  if (m_pFileAvail->IsDataAvail(m_Pos, req_size)) {
    int32_t iSize = (int32_t)(m_Pos + req_size - m_dwCurrentXRefSteam);
    CFX_BinaryBuf buf(iSize);
    uint8_t* pBuf = buf.GetBuffer();

    m_pFileRead->ReadBlock(pBuf, m_dwCurrentXRefSteam, iSize);

    ScopedFileStream file(FX_CreateMemoryStream(pBuf, (size_t)iSize, FALSE));
    m_parser.m_pSyntax->InitParser(file.get(), 0);

    bool bNumber;
    CFX_ByteString objnum = m_parser.m_pSyntax->GetNextWord(&bNumber);
    if (!bNumber)
      return -1;

    uint32_t objNum = FXSYS_atoui(objnum.c_str());
    CPDF_Object* pObj = m_parser.ParseIndirectObjectAt(nullptr, 0, objNum);
    if (!pObj) {
      m_Pos += m_parser.m_pSyntax->SavePos();
      return 0;
    }

    CPDF_Dictionary* pDict = pObj->GetDict();
    CPDF_Name* pName = ToName(pDict ? pDict->GetObjectBy("Type") : nullptr);
    if (pName) {
      if (pName->GetString() == "XRef") {
        m_Pos += m_parser.m_pSyntax->SavePos();
        xref_offset = pObj->GetDict()->GetIntegerBy("Prev");
        pObj->Release();
        return 1;
      }
    }
    pObj->Release();
    return -1;
  }
  pHints->AddSegment(m_Pos, req_size);
  return 0;
}

void CPDF_DataAvail::SetStartOffset(FX_FILESIZE dwOffset) {
  m_Pos = dwOffset;
}

FX_BOOL CPDF_DataAvail::GetNextToken(CFX_ByteString& token) {
  uint8_t ch;
  if (!GetNextChar(ch))
    return FALSE;

  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!GetNextChar(ch))
        return FALSE;
    }

    if (ch != '%')
      break;

    while (1) {
      if (!GetNextChar(ch))
        return FALSE;
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  uint8_t buffer[256];
  uint32_t index = 0;
  if (PDFCharIsDelimiter(ch)) {
    buffer[index++] = ch;
    if (ch == '/') {
      while (1) {
        if (!GetNextChar(ch))
          return FALSE;

        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          m_Pos--;
          CFX_ByteString ret(buffer, index);
          token = ret;
          return TRUE;
        }

        if (index < sizeof(buffer))
          buffer[index++] = ch;
      }
    } else if (ch == '<') {
      if (!GetNextChar(ch))
        return FALSE;

      if (ch == '<')
        buffer[index++] = ch;
      else
        m_Pos--;
    } else if (ch == '>') {
      if (!GetNextChar(ch))
        return FALSE;

      if (ch == '>')
        buffer[index++] = ch;
      else
        m_Pos--;
    }

    CFX_ByteString ret(buffer, index);
    token = ret;
    return TRUE;
  }

  while (1) {
    if (index < sizeof(buffer))
      buffer[index++] = ch;

    if (!GetNextChar(ch))
      return FALSE;

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }

  token = CFX_ByteString(buffer, index);
  return TRUE;
}

FX_BOOL CPDF_DataAvail::GetNextChar(uint8_t& ch) {
  FX_FILESIZE pos = m_Pos;
  if (pos >= m_dwFileLen)
    return FALSE;

  if (m_bufferOffset >= pos ||
      (FX_FILESIZE)(m_bufferOffset + m_bufferSize) <= pos) {
    FX_FILESIZE read_pos = pos;
    uint32_t read_size = 512;
    if ((FX_FILESIZE)read_size > m_dwFileLen)
      read_size = (uint32_t)m_dwFileLen;

    if ((FX_FILESIZE)(read_pos + read_size) > m_dwFileLen)
      read_pos = m_dwFileLen - read_size;

    if (!m_pFileRead->ReadBlock(m_bufferData, read_pos, read_size))
      return FALSE;

    m_bufferOffset = read_pos;
    m_bufferSize = read_size;
  }
  ch = m_bufferData[pos - m_bufferOffset];
  m_Pos++;
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckCrossRefItem(DownloadHints* pHints) {
  int32_t iSize = 0;
  CFX_ByteString token;
  while (1) {
    if (!GetNextToken(token)) {
      iSize = (int32_t)(m_Pos + 512 > m_dwFileLen ? m_dwFileLen - m_Pos : 512);
      pHints->AddSegment(m_Pos, iSize);
      return FALSE;
    }

    if (token == "trailer") {
      m_dwTrailerOffset = m_Pos;
      m_docStatus = PDF_DATAAVAIL_TRAILER;
      return TRUE;
    }
  }
}

FX_BOOL CPDF_DataAvail::CheckAllCrossRefStream(DownloadHints* pHints) {
  FX_FILESIZE xref_offset = 0;

  int32_t nRet = CheckCrossRefStream(pHints, xref_offset);
  if (nRet == 1) {
    if (!xref_offset) {
      m_docStatus = PDF_DATAAVAIL_LOADALLCROSSREF;
    } else {
      m_dwCurrentXRefSteam = xref_offset;
      m_Pos = xref_offset;
    }
    return TRUE;
  }

  if (nRet == -1)
    m_docStatus = PDF_DATAAVAIL_ERROR;
  return FALSE;
}

FX_BOOL CPDF_DataAvail::CheckCrossRef(DownloadHints* pHints) {
  int32_t iSize = 0;
  CFX_ByteString token;
  if (!GetNextToken(token)) {
    iSize = (int32_t)(m_Pos + 512 > m_dwFileLen ? m_dwFileLen - m_Pos : 512);
    pHints->AddSegment(m_Pos, iSize);
    return FALSE;
  }

  if (token == "xref") {
    while (1) {
      if (!GetNextToken(token)) {
        iSize =
            (int32_t)(m_Pos + 512 > m_dwFileLen ? m_dwFileLen - m_Pos : 512);
        pHints->AddSegment(m_Pos, iSize);
        m_docStatus = PDF_DATAAVAIL_CROSSREF_ITEM;
        return FALSE;
      }

      if (token == "trailer") {
        m_dwTrailerOffset = m_Pos;
        m_docStatus = PDF_DATAAVAIL_TRAILER;
        return TRUE;
      }
    }
  } else {
    m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
    return TRUE;
  }
  return FALSE;
}

FX_BOOL CPDF_DataAvail::CheckTrailerAppend(DownloadHints* pHints) {
  if (m_Pos < m_dwFileLen) {
    FX_FILESIZE dwAppendPos = m_Pos + m_syntaxParser.SavePos();
    int32_t iSize = (int32_t)(
        dwAppendPos + 512 > m_dwFileLen ? m_dwFileLen - dwAppendPos : 512);

    if (!m_pFileAvail->IsDataAvail(dwAppendPos, iSize)) {
      pHints->AddSegment(dwAppendPos, iSize);
      return FALSE;
    }
  }

  if (m_dwPrevXRefOffset) {
    SetStartOffset(m_dwPrevXRefOffset);
    m_docStatus = PDF_DATAAVAIL_CROSSREF;
  } else {
    m_docStatus = PDF_DATAAVAIL_LOADALLCROSSREF;
  }
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckTrailer(DownloadHints* pHints) {
  int32_t iTrailerSize =
      (int32_t)(m_Pos + 512 > m_dwFileLen ? m_dwFileLen - m_Pos : 512);
  if (m_pFileAvail->IsDataAvail(m_Pos, iTrailerSize)) {
    int32_t iSize = (int32_t)(m_Pos + iTrailerSize - m_dwTrailerOffset);
    CFX_BinaryBuf buf(iSize);
    uint8_t* pBuf = buf.GetBuffer();
    if (!pBuf) {
      m_docStatus = PDF_DATAAVAIL_ERROR;
      return FALSE;
    }

    if (!m_pFileRead->ReadBlock(pBuf, m_dwTrailerOffset, iSize))
      return FALSE;

    ScopedFileStream file(FX_CreateMemoryStream(pBuf, (size_t)iSize, FALSE));
    m_syntaxParser.InitParser(file.get(), 0);

    std::unique_ptr<CPDF_Object, ReleaseDeleter<CPDF_Object>> pTrailer(
        m_syntaxParser.GetObject(nullptr, 0, 0, true));
    if (!pTrailer) {
      m_Pos += m_syntaxParser.SavePos();
      pHints->AddSegment(m_Pos, iTrailerSize);
      return FALSE;
    }

    if (!pTrailer->IsDictionary())
      return FALSE;

    CPDF_Dictionary* pTrailerDict = pTrailer->GetDict();
    CPDF_Object* pEncrypt = pTrailerDict->GetObjectBy("Encrypt");
    if (ToReference(pEncrypt)) {
      m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      return TRUE;
    }

    uint32_t xrefpos = GetDirectInteger(pTrailerDict, "Prev");
    if (xrefpos) {
      m_dwPrevXRefOffset = GetDirectInteger(pTrailerDict, "XRefStm");
      if (m_dwPrevXRefOffset) {
        m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
      } else {
        m_dwPrevXRefOffset = xrefpos;
        if (m_dwPrevXRefOffset >= m_dwFileLen) {
          m_docStatus = PDF_DATAAVAIL_LOADALLFILE;
        } else {
          SetStartOffset(m_dwPrevXRefOffset);
          m_docStatus = PDF_DATAAVAIL_TRAILER_APPEND;
        }
      }
      return TRUE;
    }
    m_dwPrevXRefOffset = 0;
    m_docStatus = PDF_DATAAVAIL_TRAILER_APPEND;
    return TRUE;
  }
  pHints->AddSegment(m_Pos, iTrailerSize);
  return FALSE;
}

FX_BOOL CPDF_DataAvail::CheckPage(int32_t iPage, DownloadHints* pHints) {
  while (TRUE) {
    switch (m_docStatus) {
      case PDF_DATAAVAIL_PAGETREE:
        if (!LoadDocPages(pHints))
          return FALSE;
        break;
      case PDF_DATAAVAIL_PAGE:
        if (!LoadDocPage(iPage, pHints))
          return FALSE;
        break;
      case PDF_DATAAVAIL_ERROR:
        return LoadAllFile(pHints);
      default:
        m_bPagesTreeLoad = TRUE;
        m_bPagesLoad = TRUE;
        m_bCurPageDictLoadOK = TRUE;
        m_docStatus = PDF_DATAAVAIL_PAGE;
        return TRUE;
    }
  }
}

FX_BOOL CPDF_DataAvail::CheckArrayPageNode(uint32_t dwPageNo,
                                           PageNode* pPageNode,
                                           DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  CPDF_Object* pPages = GetObject(dwPageNo, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (!pPages) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR) {
      m_docStatus = PDF_DATAAVAIL_ERROR;
      return FALSE;
    }
    return FALSE;
  }

  CPDF_Array* pArray = pPages->AsArray();
  if (!pArray) {
    pPages->Release();
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  pPageNode->m_type = PDF_PAGENODE_PAGES;
  for (size_t i = 0; i < pArray->GetCount(); ++i) {
    CPDF_Reference* pKid = ToReference(pArray->GetObjectAt(i));
    if (!pKid)
      continue;

    PageNode* pNode = new PageNode();
    pPageNode->m_childNode.Add(pNode);
    pNode->m_dwPageNo = pKid->GetRefObjNum();
  }
  pPages->Release();
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckUnkownPageNode(uint32_t dwPageNo,
                                            PageNode* pPageNode,
                                            DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  CPDF_Object* pPage = GetObject(dwPageNo, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (!pPage) {
    if (m_docStatus == PDF_DATAAVAIL_ERROR)
      m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (pPage->IsArray()) {
    pPageNode->m_dwPageNo = dwPageNo;
    pPageNode->m_type = PDF_PAGENODE_ARRAY;
    pPage->Release();
    return TRUE;
  }

  if (!pPage->IsDictionary()) {
    pPage->Release();
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  pPageNode->m_dwPageNo = dwPageNo;
  CPDF_Dictionary* pDict = pPage->GetDict();
  CFX_ByteString type = pDict->GetStringBy("Type");
  if (type == "Pages") {
    pPageNode->m_type = PDF_PAGENODE_PAGES;
    CPDF_Object* pKids = pDict->GetObjectBy("Kids");
    if (!pKids) {
      m_docStatus = PDF_DATAAVAIL_PAGE;
      return TRUE;
    }

    switch (pKids->GetType()) {
      case CPDF_Object::REFERENCE: {
        CPDF_Reference* pKid = pKids->AsReference();
        PageNode* pNode = new PageNode();
        pPageNode->m_childNode.Add(pNode);
        pNode->m_dwPageNo = pKid->GetRefObjNum();
      } break;
      case CPDF_Object::ARRAY: {
        CPDF_Array* pKidsArray = pKids->AsArray();
        for (size_t i = 0; i < pKidsArray->GetCount(); ++i) {
          CPDF_Reference* pKid = ToReference(pKidsArray->GetObjectAt(i));
          if (!pKid)
            continue;

          PageNode* pNode = new PageNode();
          pPageNode->m_childNode.Add(pNode);
          pNode->m_dwPageNo = pKid->GetRefObjNum();
        }
      } break;
      default:
        break;
    }
  } else if (type == "Page") {
    pPageNode->m_type = PDF_PAGENODE_PAGE;
  } else {
    pPage->Release();
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }
  pPage->Release();
  return TRUE;
}

FX_BOOL CPDF_DataAvail::CheckPageNode(CPDF_DataAvail::PageNode& pageNodes,
                                      int32_t iPage,
                                      int32_t& iCount,
                                      DownloadHints* pHints,
                                      int level) {
  if (level >= kMaxPageRecursionDepth)
    return FALSE;

  int32_t iSize = pageNodes.m_childNode.GetSize();
  if (iSize <= 0 || iPage >= iSize) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  for (int32_t i = 0; i < iSize; ++i) {
    PageNode* pNode = pageNodes.m_childNode.GetAt(i);
    if (!pNode)
      continue;

    switch (pNode->m_type) {
      case PDF_PAGENODE_UNKNOWN:
        if (!CheckUnkownPageNode(pNode->m_dwPageNo, pNode, pHints)) {
          return FALSE;
        }
        --i;
        break;
      case PDF_PAGENODE_PAGE:
        iCount++;
        if (iPage == iCount && m_pDocument)
          m_pDocument->m_PageList.SetAt(iPage, pNode->m_dwPageNo);
        break;
      case PDF_PAGENODE_PAGES:
        if (!CheckPageNode(*pNode, iPage, iCount, pHints, level + 1))
          return FALSE;
        break;
      case PDF_PAGENODE_ARRAY:
        if (!CheckArrayPageNode(pNode->m_dwPageNo, pNode, pHints))
          return FALSE;
        --i;
        break;
    }

    if (iPage == iCount) {
      m_docStatus = PDF_DATAAVAIL_DONE;
      return TRUE;
    }
  }
  return TRUE;
}

FX_BOOL CPDF_DataAvail::LoadDocPage(int32_t iPage, DownloadHints* pHints) {
  if (m_pDocument->GetPageCount() <= iPage ||
      m_pDocument->m_PageList.GetAt(iPage)) {
    m_docStatus = PDF_DATAAVAIL_DONE;
    return TRUE;
  }

  if (m_pageNodes.m_type == PDF_PAGENODE_PAGE) {
    if (iPage == 0) {
      m_docStatus = PDF_DATAAVAIL_DONE;
      return TRUE;
    }
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return TRUE;
  }
  int32_t iCount = -1;
  return CheckPageNode(m_pageNodes, iPage, iCount, pHints, 0);
}

FX_BOOL CPDF_DataAvail::CheckPageCount(DownloadHints* pHints) {
  FX_BOOL bExist = FALSE;
  CPDF_Object* pPages = GetObject(m_PagesObjNum, pHints, &bExist);
  if (!bExist) {
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (!pPages)
    return FALSE;

  CPDF_Dictionary* pPagesDict = pPages->GetDict();
  if (!pPagesDict) {
    pPages->Release();
    m_docStatus = PDF_DATAAVAIL_ERROR;
    return FALSE;
  }

  if (!pPagesDict->KeyExist("Kids")) {
    pPages->Release();
    return TRUE;
  }

  int count = pPagesDict->GetIntegerBy("Count");
  if (count > 0) {
    pPages->Release();
    return TRUE;
  }

  pPages->Release();
  return FALSE;
}

FX_BOOL CPDF_DataAvail::LoadDocPages(DownloadHints* pHints) {
  if (!CheckUnkownPageNode(m_PagesObjNum, &m_pageNodes, pHints))
    return FALSE;

  if (CheckPageCount(pHints)) {
    m_docStatus = PDF_DATAAVAIL_PAGE;
    return TRUE;
  }

  m_bTotalLoadPageTree = TRUE;
  return FALSE;
}

FX_BOOL CPDF_DataAvail::LoadPages(DownloadHints* pHints) {
  while (!m_bPagesTreeLoad) {
    if (!CheckPageStatus(pHints))
      return FALSE;
  }

  if (m_bPagesLoad)
    return TRUE;

  m_pDocument->LoadPages();
  return FALSE;
}

CPDF_DataAvail::DocAvailStatus CPDF_DataAvail::CheckLinearizedData(
    DownloadHints* pHints) {
  if (m_bLinearedDataOK)
    return DataAvailable;

  if (!m_bMainXRefLoadTried) {
    FX_SAFE_UINT32 data_size = m_dwFileLen;
    data_size -= m_dwLastXRefOffset;
    if (!data_size.IsValid())
      return DataError;

    if (!m_pFileAvail->IsDataAvail(m_dwLastXRefOffset,
                                   data_size.ValueOrDie())) {
      pHints->AddSegment(m_dwLastXRefOffset, data_size.ValueOrDie());
      return DataNotAvailable;
    }

    CPDF_Parser::Error eRet =
        m_pDocument->GetParser()->LoadLinearizedMainXRefTable();
    m_bMainXRefLoadTried = TRUE;
    if (eRet != CPDF_Parser::SUCCESS)
      return DataError;

    if (!PreparePageItem())
      return DataNotAvailable;

    m_bMainXRefLoadedOK = TRUE;
    m_bLinearedDataOK = TRUE;
  }

  return m_bLinearedDataOK ? DataAvailable : DataNotAvailable;
}

FX_BOOL CPDF_DataAvail::CheckPageAnnots(int32_t iPage, DownloadHints* pHints) {
  if (!m_objs_array.GetSize()) {
    m_objs_array.RemoveAll();
    m_ObjectSet.clear();

    CPDF_Dictionary* pPageDict = m_pDocument->GetPage(iPage);
    if (!pPageDict)
      return TRUE;

    CPDF_Object* pAnnots = pPageDict->GetObjectBy("Annots");
    if (!pAnnots)
      return TRUE;

    CFX_ArrayTemplate<CPDF_Object*> obj_array;
    obj_array.Add(pAnnots);

    FX_BOOL bRet = IsObjectsAvail(obj_array, FALSE, pHints, m_objs_array);
    if (bRet)
      m_objs_array.RemoveAll();

    return bRet;
  }

  CFX_ArrayTemplate<CPDF_Object*> new_objs_array;
  FX_BOOL bRet = IsObjectsAvail(m_objs_array, FALSE, pHints, new_objs_array);
  m_objs_array.RemoveAll();
  if (!bRet)
    m_objs_array.Append(new_objs_array);

  return bRet;
}

CPDF_DataAvail::DocAvailStatus CPDF_DataAvail::CheckLinearizedFirstPage(
    int32_t iPage,
    DownloadHints* pHints) {
  if (!m_bAnnotsLoad) {
    if (!CheckPageAnnots(iPage, pHints))
      return DataNotAvailable;
    m_bAnnotsLoad = TRUE;
  }

  DocAvailStatus nRet = CheckLinearizedData(pHints);
  if (nRet == DataAvailable)
    m_bPageLoadedOK = FALSE;
  return nRet;
}

FX_BOOL CPDF_DataAvail::HaveResourceAncestor(CPDF_Dictionary* pDict) {
  CFX_AutoRestorer<int> restorer(&s_CurrentDataAvailRecursionDepth);
  if (++s_CurrentDataAvailRecursionDepth > kMaxDataAvailRecursionDepth)
    return FALSE;

  CPDF_Object* pParent = pDict->GetObjectBy("Parent");
  if (!pParent)
    return FALSE;

  CPDF_Dictionary* pParentDict = pParent->GetDict();
  if (!pParentDict)
    return FALSE;

  CPDF_Object* pRet = pParentDict->GetObjectBy("Resources");
  if (pRet) {
    m_pPageResource = pRet;
    return TRUE;
  }

  return HaveResourceAncestor(pParentDict);
}

CPDF_DataAvail::DocAvailStatus CPDF_DataAvail::IsPageAvail(
    int32_t iPage,
    DownloadHints* pHints) {
  if (!m_pDocument)
    return DataError;

  if (IsFirstCheck(iPage)) {
    m_bCurPageDictLoadOK = FALSE;
    m_bPageLoadedOK = FALSE;
    m_bAnnotsLoad = FALSE;
    m_bNeedDownLoadResource = FALSE;
    m_objs_array.RemoveAll();
    m_ObjectSet.clear();
  }

  if (pdfium::ContainsKey(m_pagesLoadState, iPage))
    return DataAvailable;

  if (m_bLinearized) {
    if ((uint32_t)iPage == m_dwFirstPageNo) {
      DocAvailStatus nRet = CheckLinearizedFirstPage(iPage, pHints);
      if (nRet == DataAvailable)
        m_pagesLoadState.insert(iPage);
      return nRet;
    }

    DocAvailStatus nResult = CheckLinearizedData(pHints);
    if (nResult != DataAvailable)
      return nResult;

    if (m_pHintTables) {
      nResult = m_pHintTables->CheckPage(iPage, pHints);
      if (nResult != DataAvailable)
        return nResult;
      m_pagesLoadState.insert(iPage);
      return DataAvailable;
    }

    if (m_bMainXRefLoadedOK) {
      if (m_bTotalLoadPageTree) {
        if (!LoadPages(pHints))
          return DataNotAvailable;
      } else {
        if (!m_bCurPageDictLoadOK && !CheckPage(iPage, pHints))
          return DataNotAvailable;
      }
    } else {
      if (!LoadAllFile(pHints))
        return DataNotAvailable;
      m_pDocument->GetParser()->RebuildCrossRef();
      ResetFirstCheck(iPage);
      return DataAvailable;
    }
  } else {
    if (!m_bTotalLoadPageTree && !m_bCurPageDictLoadOK &&
        !CheckPage(iPage, pHints)) {
      return DataNotAvailable;
    }
  }

  if (m_bHaveAcroForm && !m_bAcroFormLoad) {
    if (!CheckAcroFormSubObject(pHints))
      return DataNotAvailable;
    m_bAcroFormLoad = TRUE;
  }

  if (!m_bPageLoadedOK) {
    if (!m_objs_array.GetSize()) {
      m_objs_array.RemoveAll();
      m_ObjectSet.clear();

      m_pPageDict = m_pDocument->GetPage(iPage);
      if (!m_pPageDict) {
        ResetFirstCheck(iPage);
        return DataAvailable;
      }

      CFX_ArrayTemplate<CPDF_Object*> obj_array;
      obj_array.Add(m_pPageDict);
      FX_BOOL bRet = IsObjectsAvail(obj_array, TRUE, pHints, m_objs_array);
      if (!bRet)
        return DataNotAvailable;

      m_objs_array.RemoveAll();
    } else {
      CFX_ArrayTemplate<CPDF_Object*> new_objs_array;
      FX_BOOL bRet =
          IsObjectsAvail(m_objs_array, FALSE, pHints, new_objs_array);

      m_objs_array.RemoveAll();
      if (!bRet) {
        m_objs_array.Append(new_objs_array);
        return DataNotAvailable;
      }
    }
    m_bPageLoadedOK = TRUE;
  }

  if (!m_bAnnotsLoad) {
    if (!CheckPageAnnots(iPage, pHints))
      return DataNotAvailable;
    m_bAnnotsLoad = TRUE;
  }

  if (m_pPageDict && !m_bNeedDownLoadResource) {
    m_pPageResource = m_pPageDict->GetObjectBy("Resources");
    if (!m_pPageResource)
      m_bNeedDownLoadResource = HaveResourceAncestor(m_pPageDict);
    else
      m_bNeedDownLoadResource = TRUE;
  }

  if (m_bNeedDownLoadResource) {
    FX_BOOL bRet = CheckResources(pHints);
    if (!bRet)
      return DataNotAvailable;
    m_bNeedDownLoadResource = FALSE;
  }

  m_bPageLoadedOK = FALSE;
  m_bAnnotsLoad = FALSE;
  m_bCurPageDictLoadOK = FALSE;

  ResetFirstCheck(iPage);
  m_pagesLoadState.insert(iPage);
  return DataAvailable;
}

FX_BOOL CPDF_DataAvail::CheckResources(DownloadHints* pHints) {
  if (!m_objs_array.GetSize()) {
    m_objs_array.RemoveAll();
    CFX_ArrayTemplate<CPDF_Object*> obj_array;
    obj_array.Add(m_pPageResource);

    FX_BOOL bRet = IsObjectsAvail(obj_array, TRUE, pHints, m_objs_array);
    if (bRet)
      m_objs_array.RemoveAll();
    return bRet;
  }

  CFX_ArrayTemplate<CPDF_Object*> new_objs_array;
  FX_BOOL bRet = IsObjectsAvail(m_objs_array, FALSE, pHints, new_objs_array);
  m_objs_array.RemoveAll();
  if (!bRet)
    m_objs_array.Append(new_objs_array);
  return bRet;
}

void CPDF_DataAvail::GetLinearizedMainXRefInfo(FX_FILESIZE* pPos,
                                               uint32_t* pSize) {
  if (pPos)
    *pPos = m_dwLastXRefOffset;
  if (pSize)
    *pSize = (uint32_t)(m_dwFileLen - m_dwLastXRefOffset);
}

int CPDF_DataAvail::GetPageCount() const {
  if (m_pLinearized) {
    CPDF_Dictionary* pDict = m_pLinearized->GetDict();
    CPDF_Object* pObj = pDict ? pDict->GetDirectObjectBy("N") : nullptr;
    return pObj ? pObj->GetInteger() : 0;
  }
  return m_pDocument ? m_pDocument->GetPageCount() : 0;
}

CPDF_Dictionary* CPDF_DataAvail::GetPage(int index) {
  if (!m_pDocument || index < 0 || index >= GetPageCount())
    return nullptr;

  if (m_pLinearized) {
    CPDF_Dictionary* pDict = m_pLinearized->GetDict();
    CPDF_Object* pObj = pDict ? pDict->GetDirectObjectBy("P") : nullptr;

    int pageNum = pObj ? pObj->GetInteger() : 0;
    if (m_pHintTables && index != pageNum) {
      FX_FILESIZE szPageStartPos = 0;
      FX_FILESIZE szPageLength = 0;
      uint32_t dwObjNum = 0;
      FX_BOOL bPagePosGot = m_pHintTables->GetPagePos(index, szPageStartPos,
                                                      szPageLength, dwObjNum);
      if (!bPagePosGot)
        return nullptr;

      m_syntaxParser.InitParser(m_pFileRead, (uint32_t)szPageStartPos);
      CPDF_Object* pPageDict = ParseIndirectObjectAt(0, dwObjNum, m_pDocument);
      if (!pPageDict)
        return nullptr;

      if (!m_pDocument->InsertIndirectObject(dwObjNum, pPageDict))
        return nullptr;
      return pPageDict->GetDict();
    }
  }
  return m_pDocument->GetPage(index);
}

CPDF_DataAvail::DocFormStatus CPDF_DataAvail::IsFormAvail(
    DownloadHints* pHints) {
  if (!m_pDocument)
    return FormAvailable;

  if (!m_bLinearizedFormParamLoad) {
    CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
    if (!pRoot)
      return FormAvailable;

    CPDF_Object* pAcroForm = pRoot->GetObjectBy("AcroForm");
    if (!pAcroForm)
      return FormNotExist;

    DocAvailStatus nDocStatus = CheckLinearizedData(pHints);
    if (nDocStatus == DataError)
      return FormError;
    if (nDocStatus == DataNotAvailable)
      return FormNotAvailable;

    if (!m_objs_array.GetSize())
      m_objs_array.Add(pAcroForm->GetDict());
    m_bLinearizedFormParamLoad = TRUE;
  }

  CFX_ArrayTemplate<CPDF_Object*> new_objs_array;
  FX_BOOL bRet = IsObjectsAvail(m_objs_array, FALSE, pHints, new_objs_array);
  m_objs_array.RemoveAll();
  if (!bRet) {
    m_objs_array.Append(new_objs_array);
    return FormNotAvailable;
  }
  return FormAvailable;
}

CPDF_DataAvail::PageNode::PageNode() : m_type(PDF_PAGENODE_UNKNOWN) {}

CPDF_DataAvail::PageNode::~PageNode() {
  for (int32_t i = 0; i < m_childNode.GetSize(); ++i)
    delete m_childNode[i];
  m_childNode.RemoveAll();
}
