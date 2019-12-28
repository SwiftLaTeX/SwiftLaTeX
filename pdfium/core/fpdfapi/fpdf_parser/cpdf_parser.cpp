// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"

#include <vector>

#include "core/fpdfapi/fpdf_parser/cpdf_crypto_handler.h"
#include "core/fpdfapi/fpdf_parser/cpdf_security_handler.h"
#include "core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/fpdf_parser/fpdf_parser_utility.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "third_party/base/stl_util.h"

namespace {

// A limit on the size of the xref table. Theoretical limits are higher, but
// this may be large enough in practice.
const int32_t kMaxXRefSize = 1048576;

// A limit on the maximum object number in the xref table. Theoretical limits
// are higher, but this may be large enough in practice.
const uint32_t kMaxObjectNumber = 1048576;

uint32_t GetVarInt(const uint8_t* p, int32_t n) {
  uint32_t result = 0;
  for (int32_t i = 0; i < n; ++i)
    result = result * 256 + p[i];
  return result;
}

int32_t GetStreamNCount(CPDF_StreamAcc* pObjStream) {
  return pObjStream->GetDict()->GetIntegerBy("N");
}

int32_t GetStreamFirst(CPDF_StreamAcc* pObjStream) {
  return pObjStream->GetDict()->GetIntegerBy("First");
}

}  // namespace

CPDF_Parser::CPDF_Parser()
    : m_bOwnFileRead(true),
      m_FileVersion(0),
      m_pTrailer(nullptr),
      m_pEncryptDict(nullptr),
      m_bVersionUpdated(false),
      m_pLinearized(nullptr),
      m_dwFirstPageNo(0),
      m_dwXrefStartObjNum(0) {
  m_pSyntax.reset(new CPDF_SyntaxParser);
}

CPDF_Parser::~CPDF_Parser() {
  CloseParser();
}

uint32_t CPDF_Parser::GetLastObjNum() const {
  return m_ObjectInfo.empty() ? 0 : m_ObjectInfo.rbegin()->first;
}

bool CPDF_Parser::IsValidObjectNumber(uint32_t objnum) const {
  return !m_ObjectInfo.empty() && objnum <= m_ObjectInfo.rbegin()->first;
}

FX_FILESIZE CPDF_Parser::GetObjectPositionOrZero(uint32_t objnum) const {
  auto it = m_ObjectInfo.find(objnum);
  return it != m_ObjectInfo.end() ? it->second.pos : 0;
}

uint8_t CPDF_Parser::GetObjectType(uint32_t objnum) const {
  ASSERT(IsValidObjectNumber(objnum));
  auto it = m_ObjectInfo.find(objnum);
  return it != m_ObjectInfo.end() ? it->second.type : 0;
}

uint16_t CPDF_Parser::GetObjectGenNum(uint32_t objnum) const {
  ASSERT(IsValidObjectNumber(objnum));
  auto it = m_ObjectInfo.find(objnum);
  return it != m_ObjectInfo.end() ? it->second.gennum : 0;
}

bool CPDF_Parser::IsObjectFreeOrNull(uint32_t objnum) const {
  uint8_t type = GetObjectType(objnum);
  return type == 0 || type == 255;
}

void CPDF_Parser::SetEncryptDictionary(CPDF_Dictionary* pDict) {
  m_pEncryptDict = pDict;
}

CPDF_CryptoHandler* CPDF_Parser::GetCryptoHandler() {
  return m_pSyntax->m_pCryptoHandler.get();
}

IFX_FileRead* CPDF_Parser::GetFileAccess() const {
  return m_pSyntax->m_pFileAccess;
}

void CPDF_Parser::ShrinkObjectMap(uint32_t objnum) {
  if (objnum == 0) {
    m_ObjectInfo.clear();
    return;
  }

  auto it = m_ObjectInfo.lower_bound(objnum);
  while (it != m_ObjectInfo.end()) {
    auto saved_it = it++;
    m_ObjectInfo.erase(saved_it);
  }

  if (!pdfium::ContainsKey(m_ObjectInfo, objnum - 1))
    m_ObjectInfo[objnum - 1].pos = 0;
}

void CPDF_Parser::CloseParser() {
  m_bVersionUpdated = false;
  m_pDocument.reset();

  if (m_pTrailer) {
    m_pTrailer->Release();
    m_pTrailer = nullptr;
  }
  ReleaseEncryptHandler();
  SetEncryptDictionary(nullptr);

  if (m_bOwnFileRead && m_pSyntax->m_pFileAccess) {
    m_pSyntax->m_pFileAccess->Release();
    m_pSyntax->m_pFileAccess = nullptr;
  }

  m_ObjectStreamMap.clear();
  m_ObjCache.clear();
  m_SortedOffset.clear();
  m_ObjectInfo.clear();

  int32_t iLen = m_Trailers.GetSize();
  for (int32_t i = 0; i < iLen; ++i) {
    if (CPDF_Dictionary* trailer = m_Trailers.GetAt(i))
      trailer->Release();
  }
  m_Trailers.RemoveAll();

  if (m_pLinearized) {
    m_pLinearized->Release();
    m_pLinearized = nullptr;
  }
}

CPDF_Parser::Error CPDF_Parser::StartParse(IFX_FileRead* pFileAccess) {
  CloseParser();

  m_bXRefStream = FALSE;
  m_LastXRefOffset = 0;
  m_bOwnFileRead = true;

  int32_t offset = GetHeaderOffset(pFileAccess);
  if (offset == -1) {
    if (pFileAccess)
      pFileAccess->Release();
    return FORMAT_ERROR;
  }
  m_pSyntax->InitParser(pFileAccess, offset);

  uint8_t ch;
  if (!m_pSyntax->GetCharAt(5, ch))
    return FORMAT_ERROR;
  if (std::isdigit(ch))
    m_FileVersion = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch)) * 10;

  if (!m_pSyntax->GetCharAt(7, ch))
    return FORMAT_ERROR;
  if (std::isdigit(ch))
    m_FileVersion += FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));

  if (m_pSyntax->m_FileLen < m_pSyntax->m_HeaderOffset + 9)
    return FORMAT_ERROR;

  m_pSyntax->RestorePos(m_pSyntax->m_FileLen - m_pSyntax->m_HeaderOffset - 9);
  m_pDocument.reset(new CPDF_Document(this));

  FX_BOOL bXRefRebuilt = FALSE;
  if (m_pSyntax->SearchWord("startxref", TRUE, FALSE, 4096)) {
    m_SortedOffset.insert(m_pSyntax->SavePos());
    m_pSyntax->GetKeyword();

    bool bNumber;
    CFX_ByteString xrefpos_str = m_pSyntax->GetNextWord(&bNumber);
    if (!bNumber)
      return FORMAT_ERROR;

    m_LastXRefOffset = (FX_FILESIZE)FXSYS_atoi64(xrefpos_str.c_str());
    if (!LoadAllCrossRefV4(m_LastXRefOffset) &&
        !LoadAllCrossRefV5(m_LastXRefOffset)) {
      if (!RebuildCrossRef())
        return FORMAT_ERROR;

      bXRefRebuilt = TRUE;
      m_LastXRefOffset = 0;
    }
  } else {
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    bXRefRebuilt = TRUE;
  }
  Error eRet = SetEncryptHandler();
  if (eRet != SUCCESS)
    return eRet;

  m_pDocument->LoadDoc();
  if (!m_pDocument->GetRoot() || m_pDocument->GetPageCount() == 0) {
    if (bXRefRebuilt)
      return FORMAT_ERROR;

    ReleaseEncryptHandler();
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;

    m_pDocument->LoadDoc();
    if (!m_pDocument->GetRoot())
      return FORMAT_ERROR;
  }
  if (GetRootObjNum() == 0) {
    ReleaseEncryptHandler();
    if (!RebuildCrossRef() || GetRootObjNum() == 0)
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;
  }
  if (m_pSecurityHandler && !m_pSecurityHandler->IsMetadataEncrypted()) {
    CPDF_Reference* pMetadata =
        ToReference(m_pDocument->GetRoot()->GetObjectBy("Metadata"));
    if (pMetadata)
      m_pSyntax->m_MetadataObjnum = pMetadata->GetRefObjNum();
  }
  return SUCCESS;
}
CPDF_Parser::Error CPDF_Parser::SetEncryptHandler() {
  ReleaseEncryptHandler();
  SetEncryptDictionary(nullptr);

  if (!m_pTrailer)
    return FORMAT_ERROR;

  CPDF_Object* pEncryptObj = m_pTrailer->GetObjectBy("Encrypt");
  if (pEncryptObj) {
    if (CPDF_Dictionary* pEncryptDict = pEncryptObj->AsDictionary()) {
      SetEncryptDictionary(pEncryptDict);
    } else if (CPDF_Reference* pRef = pEncryptObj->AsReference()) {
      pEncryptObj = m_pDocument->GetIndirectObject(pRef->GetRefObjNum());
      if (pEncryptObj)
        SetEncryptDictionary(pEncryptObj->GetDict());
    }
  }

  if (m_pEncryptDict) {
    CFX_ByteString filter = m_pEncryptDict->GetStringBy("Filter");
    std::unique_ptr<CPDF_SecurityHandler> pSecurityHandler;
    Error err = HANDLER_ERROR;
    if (filter == "Standard") {
      pSecurityHandler.reset(new CPDF_SecurityHandler);
      err = PASSWORD_ERROR;
    }
    if (!pSecurityHandler)
      return HANDLER_ERROR;

    if (!pSecurityHandler->OnInit(this, m_pEncryptDict))
      return err;

    m_pSecurityHandler = std::move(pSecurityHandler);
    std::unique_ptr<CPDF_CryptoHandler> pCryptoHandler(
        m_pSecurityHandler->CreateCryptoHandler());
    if (!pCryptoHandler->Init(m_pEncryptDict, m_pSecurityHandler.get()))
      return HANDLER_ERROR;
    m_pSyntax->SetEncrypt(std::move(pCryptoHandler));
  }
  return SUCCESS;
}

void CPDF_Parser::ReleaseEncryptHandler() {
  m_pSyntax->m_pCryptoHandler.reset();
  m_pSecurityHandler.reset();
}

FX_FILESIZE CPDF_Parser::GetObjectOffset(uint32_t objnum) const {
  if (!IsValidObjectNumber(objnum))
    return 0;

  if (GetObjectType(objnum) == 1)
    return GetObjectPositionOrZero(objnum);

  if (GetObjectType(objnum) == 2) {
    FX_FILESIZE pos = GetObjectPositionOrZero(objnum);
    return GetObjectPositionOrZero(pos);
  }
  return 0;
}

// Ideally, all the cross reference entries should be verified.
// In reality, we rarely see well-formed cross references don't match
// with the objects. crbug/602650 showed a case where object numbers
// in the cross reference table are all off by one.
bool CPDF_Parser::VerifyCrossRefV4() {
  for (const auto& it : m_ObjectInfo) {
    if (it.second.pos == 0)
      continue;
    // Find the first non-zero position.
    FX_FILESIZE SavedPos = m_pSyntax->SavePos();
    m_pSyntax->RestorePos(it.second.pos);
    bool is_num = false;
    CFX_ByteString num_str = m_pSyntax->GetNextWord(&is_num);
    m_pSyntax->RestorePos(SavedPos);
    if (!is_num || num_str.IsEmpty() ||
        FXSYS_atoui(num_str.c_str()) != it.first) {
      // If the object number read doesn't match the one stored,
      // something is wrong with the cross reference table.
      return false;
    } else {
      return true;
    }
  }
  return true;
}

FX_BOOL CPDF_Parser::LoadAllCrossRefV4(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV4(xrefpos, 0, TRUE))
    return FALSE;

  m_pTrailer = LoadTrailerV4();
  if (!m_pTrailer)
    return FALSE;

  int32_t xrefsize = GetDirectInteger(m_pTrailer, "Size");
  if (xrefsize > 0 && xrefsize <= kMaxXRefSize)
    ShrinkObjectMap(xrefsize);

  std::vector<FX_FILESIZE> CrossRefList;
  std::vector<FX_FILESIZE> XRefStreamList;
  std::set<FX_FILESIZE> seen_xrefpos;

  CrossRefList.push_back(xrefpos);
  XRefStreamList.push_back(GetDirectInteger(m_pTrailer, "XRefStm"));
  seen_xrefpos.insert(xrefpos);

  // When |m_pTrailer| doesn't have Prev entry or Prev entry value is not
  // numerical, GetDirectInteger() returns 0. Loading will end.
  xrefpos = GetDirectInteger(m_pTrailer, "Prev");
  while (xrefpos) {
    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return FALSE;

    seen_xrefpos.insert(xrefpos);

    // SLOW ...
    CrossRefList.insert(CrossRefList.begin(), xrefpos);
    LoadCrossRefV4(xrefpos, 0, TRUE);

    std::unique_ptr<CPDF_Dictionary, ReleaseDeleter<CPDF_Dictionary>> pDict(
        LoadTrailerV4());
    if (!pDict)
      return FALSE;

    xrefpos = GetDirectInteger(pDict.get(), "Prev");

    // SLOW ...
    XRefStreamList.insert(XRefStreamList.begin(),
                          pDict->GetIntegerBy("XRefStm"));
    m_Trailers.Add(pDict.release());
  }

  for (size_t i = 0; i < CrossRefList.size(); ++i) {
    if (!LoadCrossRefV4(CrossRefList[i], XRefStreamList[i], FALSE))
      return FALSE;
    if (i == 0 && !VerifyCrossRefV4())
      return FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_Parser::LoadLinearizedAllCrossRefV4(FX_FILESIZE xrefpos,
                                                 uint32_t dwObjCount) {
  if (!LoadLinearizedCrossRefV4(xrefpos, dwObjCount))
    return FALSE;

  m_pTrailer = LoadTrailerV4();
  if (!m_pTrailer)
    return FALSE;

  int32_t xrefsize = GetDirectInteger(m_pTrailer, "Size");
  if (xrefsize == 0)
    return FALSE;

  std::vector<FX_FILESIZE> CrossRefList;
  std::vector<FX_FILESIZE> XRefStreamList;
  std::set<FX_FILESIZE> seen_xrefpos;

  CrossRefList.push_back(xrefpos);
  XRefStreamList.push_back(GetDirectInteger(m_pTrailer, "XRefStm"));
  seen_xrefpos.insert(xrefpos);

  xrefpos = GetDirectInteger(m_pTrailer, "Prev");
  while (xrefpos) {
    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return FALSE;

    seen_xrefpos.insert(xrefpos);

    // SLOW ...
    CrossRefList.insert(CrossRefList.begin(), xrefpos);
    LoadCrossRefV4(xrefpos, 0, TRUE);

    std::unique_ptr<CPDF_Dictionary, ReleaseDeleter<CPDF_Dictionary>> pDict(
        LoadTrailerV4());
    if (!pDict)
      return FALSE;

    xrefpos = GetDirectInteger(pDict.get(), "Prev");

    // SLOW ...
    XRefStreamList.insert(XRefStreamList.begin(),
                          pDict->GetIntegerBy("XRefStm"));
    m_Trailers.Add(pDict.release());
  }

  for (size_t i = 1; i < CrossRefList.size(); ++i) {
    if (!LoadCrossRefV4(CrossRefList[i], XRefStreamList[i], FALSE))
      return FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_Parser::LoadLinearizedCrossRefV4(FX_FILESIZE pos,
                                              uint32_t dwObjCount) {
  FX_FILESIZE dwStartPos = pos - m_pSyntax->m_HeaderOffset;

  m_pSyntax->RestorePos(dwStartPos);
  m_SortedOffset.insert(pos);

  uint32_t start_objnum = 0;
  uint32_t count = dwObjCount;
  FX_FILESIZE SavedPos = m_pSyntax->SavePos();

  const int32_t recordsize = 20;
  std::vector<char> buf(1024 * recordsize + 1);
  buf[1024 * recordsize] = '\0';

  int32_t nBlocks = count / 1024 + 1;
  for (int32_t block = 0; block < nBlocks; block++) {
    int32_t block_size = block == nBlocks - 1 ? count % 1024 : 1024;
    uint32_t dwReadSize = block_size * recordsize;
    if ((FX_FILESIZE)(dwStartPos + dwReadSize) > m_pSyntax->m_FileLen)
      return FALSE;

    if (!m_pSyntax->ReadBlock(reinterpret_cast<uint8_t*>(buf.data()),
                              dwReadSize)) {
      return FALSE;
    }

    for (int32_t i = 0; i < block_size; i++) {
      uint32_t objnum = start_objnum + block * 1024 + i;
      char* pEntry = &buf[i * recordsize];
      if (pEntry[17] == 'f') {
        m_ObjectInfo[objnum].pos = 0;
        m_ObjectInfo[objnum].type = 0;
      } else {
        int32_t offset = FXSYS_atoi(pEntry);
        if (offset == 0) {
          for (int32_t c = 0; c < 10; c++) {
            if (!std::isdigit(pEntry[c]))
              return FALSE;
          }
        }

        m_ObjectInfo[objnum].pos = offset;
        int32_t version = FXSYS_atoi(pEntry + 11);
        if (version >= 1)
          m_bVersionUpdated = true;

        m_ObjectInfo[objnum].gennum = version;
        if (m_ObjectInfo[objnum].pos < m_pSyntax->m_FileLen)
          m_SortedOffset.insert(m_ObjectInfo[objnum].pos);

        m_ObjectInfo[objnum].type = 1;
      }
    }
  }
  m_pSyntax->RestorePos(SavedPos + count * recordsize);
  return TRUE;
}

bool CPDF_Parser::LoadCrossRefV4(FX_FILESIZE pos,
                                 FX_FILESIZE streampos,
                                 FX_BOOL bSkip) {
  m_pSyntax->RestorePos(pos);
  if (m_pSyntax->GetKeyword() != "xref")
    return false;

  m_SortedOffset.insert(pos);
  if (streampos)
    m_SortedOffset.insert(streampos);

  while (1) {
    FX_FILESIZE SavedPos = m_pSyntax->SavePos();
    bool bIsNumber;
    CFX_ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
    if (word.IsEmpty())
      return false;

    if (!bIsNumber) {
      m_pSyntax->RestorePos(SavedPos);
      break;
    }

    uint32_t start_objnum = FXSYS_atoui(word.c_str());
    if (start_objnum >= kMaxObjectNumber)
      return false;

    uint32_t count = m_pSyntax->GetDirectNum();
    m_pSyntax->ToNextWord();
    SavedPos = m_pSyntax->SavePos();
    const int32_t recordsize = 20;

    m_dwXrefStartObjNum = start_objnum;
    if (!bSkip) {
      std::vector<char> buf(1024 * recordsize + 1);
      buf[1024 * recordsize] = '\0';

      int32_t nBlocks = count / 1024 + 1;
      for (int32_t block = 0; block < nBlocks; block++) {
        int32_t block_size = block == nBlocks - 1 ? count % 1024 : 1024;
        m_pSyntax->ReadBlock(reinterpret_cast<uint8_t*>(buf.data()),
                             block_size * recordsize);

        for (int32_t i = 0; i < block_size; i++) {
          uint32_t objnum = start_objnum + block * 1024 + i;
          char* pEntry = &buf[i * recordsize];
          if (pEntry[17] == 'f') {
            m_ObjectInfo[objnum].pos = 0;
            m_ObjectInfo[objnum].type = 0;
          } else {
            FX_FILESIZE offset = (FX_FILESIZE)FXSYS_atoi64(pEntry);
            if (offset == 0) {
              for (int32_t c = 0; c < 10; c++) {
                if (!std::isdigit(pEntry[c]))
                  return false;
              }
            }

            m_ObjectInfo[objnum].pos = offset;
            int32_t version = FXSYS_atoi(pEntry + 11);
            if (version >= 1)
              m_bVersionUpdated = true;

            m_ObjectInfo[objnum].gennum = version;
            if (m_ObjectInfo[objnum].pos < m_pSyntax->m_FileLen)
              m_SortedOffset.insert(m_ObjectInfo[objnum].pos);

            m_ObjectInfo[objnum].type = 1;
          }
        }
      }
    }
    m_pSyntax->RestorePos(SavedPos + count * recordsize);
  }
  return !streampos || LoadCrossRefV5(&streampos, FALSE);
}

FX_BOOL CPDF_Parser::LoadAllCrossRefV5(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV5(&xrefpos, TRUE))
    return FALSE;

  std::set<FX_FILESIZE> seen_xrefpos;
  while (xrefpos) {
    seen_xrefpos.insert(xrefpos);
    if (!LoadCrossRefV5(&xrefpos, FALSE))
      return FALSE;

    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return FALSE;
  }
  m_ObjectStreamMap.clear();
  m_bXRefStream = TRUE;
  return TRUE;
}

FX_BOOL CPDF_Parser::RebuildCrossRef() {
  m_ObjectInfo.clear();
  m_SortedOffset.clear();
  if (m_pTrailer) {
    m_pTrailer->Release();
    m_pTrailer = nullptr;
  }

  ParserState state = ParserState::kDefault;

  int32_t inside_index = 0;
  uint32_t objnum = 0;
  uint32_t gennum = 0;
  int32_t depth = 0;

  const uint32_t kBufferSize = 4096;
  std::vector<uint8_t> buffer(kBufferSize);

  FX_FILESIZE pos = m_pSyntax->m_HeaderOffset;
  FX_FILESIZE start_pos = 0;
  FX_FILESIZE start_pos1 = 0;
  FX_FILESIZE last_obj = -1;
  FX_FILESIZE last_xref = -1;
  FX_FILESIZE last_trailer = -1;

  while (pos < m_pSyntax->m_FileLen) {
    const FX_FILESIZE saved_pos = pos;
    bool bOverFlow = false;
    uint32_t size =
        std::min((uint32_t)(m_pSyntax->m_FileLen - pos), kBufferSize);
    if (!m_pSyntax->m_pFileAccess->ReadBlock(buffer.data(), pos, size))
      break;

    for (uint32_t i = 0; i < size; i++) {
      uint8_t byte = buffer[i];
      switch (state) {
        case ParserState::kDefault:
          if (PDFCharIsWhitespace(byte)) {
            state = ParserState::kWhitespace;
          } else if (std::isdigit(byte)) {
            --i;
            state = ParserState::kWhitespace;
          } else if (byte == '%') {
            inside_index = 0;
            state = ParserState::kComment;
          } else if (byte == '(') {
            state = ParserState::kString;
            depth = 1;
          } else if (byte == '<') {
            inside_index = 1;
            state = ParserState::kHexString;
          } else if (byte == '\\') {
            state = ParserState::kEscapedString;
          } else if (byte == 't') {
            state = ParserState::kTrailer;
            inside_index = 1;
          }
          break;

        case ParserState::kWhitespace:
          if (std::isdigit(byte)) {
            start_pos = pos + i;
            state = ParserState::kObjNum;
            objnum = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(byte));
          } else if (byte == 't') {
            state = ParserState::kTrailer;
            inside_index = 1;
          } else if (byte == 'x') {
            state = ParserState::kXref;
            inside_index = 1;
          } else if (!PDFCharIsWhitespace(byte)) {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kObjNum:
          if (std::isdigit(byte)) {
            objnum =
                objnum * 10 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(byte));
          } else if (PDFCharIsWhitespace(byte)) {
            state = ParserState::kPostObjNum;
          } else {
            --i;
            state = ParserState::kEndObj;
            inside_index = 0;
          }
          break;

        case ParserState::kPostObjNum:
          if (std::isdigit(byte)) {
            start_pos1 = pos + i;
            state = ParserState::kGenNum;
            gennum = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(byte));
          } else if (byte == 't') {
            state = ParserState::kTrailer;
            inside_index = 1;
          } else if (!PDFCharIsWhitespace(byte)) {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kGenNum:
          if (std::isdigit(byte)) {
            gennum =
                gennum * 10 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(byte));
          } else if (PDFCharIsWhitespace(byte)) {
            state = ParserState::kPostGenNum;
          } else {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kPostGenNum:
          if (byte == 'o') {
            state = ParserState::kBeginObj;
            inside_index = 1;
          } else if (std::isdigit(byte)) {
            objnum = gennum;
            gennum = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(byte));
            start_pos = start_pos1;
            start_pos1 = pos + i;
            state = ParserState::kGenNum;
          } else if (byte == 't') {
            state = ParserState::kTrailer;
            inside_index = 1;
          } else if (!PDFCharIsWhitespace(byte)) {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kBeginObj:
          switch (inside_index) {
            case 1:
              if (byte != 'b') {
                --i;
                state = ParserState::kDefault;
              } else {
                inside_index++;
              }
              break;
            case 2:
              if (byte != 'j') {
                --i;
                state = ParserState::kDefault;
              } else {
                inside_index++;
              }
              break;
            case 3:
              if (PDFCharIsWhitespace(byte) || PDFCharIsDelimiter(byte)) {
                FX_FILESIZE obj_pos = start_pos - m_pSyntax->m_HeaderOffset;
                m_SortedOffset.insert(obj_pos);
                last_obj = start_pos;
                FX_FILESIZE obj_end = 0;
                CPDF_Object* pObject = ParseIndirectObjectAtByStrict(
                    m_pDocument.get(), obj_pos, objnum, &obj_end);
                if (CPDF_Stream* pStream = ToStream(pObject)) {
                  if (CPDF_Dictionary* pDict = pStream->GetDict()) {
                    if ((pDict->KeyExist("Type")) &&
                        (pDict->GetStringBy("Type") == "XRef" &&
                         pDict->KeyExist("Size"))) {
                      CPDF_Object* pRoot = pDict->GetObjectBy("Root");
                      if (pRoot && pRoot->GetDict() &&
                          pRoot->GetDict()->GetObjectBy("Pages")) {
                        if (m_pTrailer)
                          m_pTrailer->Release();
                        m_pTrailer = ToDictionary(pDict->Clone());
                      }
                    }
                  }
                }

                FX_FILESIZE offset = 0;
                m_pSyntax->RestorePos(obj_pos);
                offset = m_pSyntax->FindTag("obj", 0);
                if (offset == -1)
                  offset = 0;
                else
                  offset += 3;

                FX_FILESIZE nLen = obj_end - obj_pos - offset;
                if ((uint32_t)nLen > size - i) {
                  pos = obj_end + m_pSyntax->m_HeaderOffset;
                  bOverFlow = true;
                } else {
                  i += (uint32_t)nLen;
                }

                if (!m_ObjectInfo.empty() && IsValidObjectNumber(objnum) &&
                    m_ObjectInfo[objnum].pos) {
                  if (pObject) {
                    uint32_t oldgen = GetObjectGenNum(objnum);
                    m_ObjectInfo[objnum].pos = obj_pos;
                    m_ObjectInfo[objnum].gennum = gennum;
                    if (oldgen != gennum)
                      m_bVersionUpdated = true;
                  }
                } else {
                  m_ObjectInfo[objnum].pos = obj_pos;
                  m_ObjectInfo[objnum].type = 1;
                  m_ObjectInfo[objnum].gennum = gennum;
                }

                if (pObject)
                  pObject->Release();
              }
              --i;
              state = ParserState::kDefault;
              break;
          }
          break;

        case ParserState::kTrailer:
          if (inside_index == 7) {
            if (PDFCharIsWhitespace(byte) || PDFCharIsDelimiter(byte)) {
              last_trailer = pos + i - 7;
              m_pSyntax->RestorePos(pos + i - m_pSyntax->m_HeaderOffset);

              CPDF_Object* pObj =
                  m_pSyntax->GetObject(m_pDocument.get(), 0, 0, true);
              if (pObj) {
                if (!pObj->IsDictionary() && !pObj->AsStream()) {
                  pObj->Release();
                } else {
                  CPDF_Stream* pStream = pObj->AsStream();
                  if (CPDF_Dictionary* pTrailer =
                          pStream ? pStream->GetDict() : pObj->AsDictionary()) {
                    if (m_pTrailer) {
                      CPDF_Object* pRoot = pTrailer->GetObjectBy("Root");
                      CPDF_Reference* pRef = ToReference(pRoot);
                      if (!pRoot ||
                          (pRef && IsValidObjectNumber(pRef->GetRefObjNum()) &&
                           m_ObjectInfo[pRef->GetRefObjNum()].pos != 0)) {
                        auto it = pTrailer->begin();
                        while (it != pTrailer->end()) {
                          const CFX_ByteString& key = it->first;
                          CPDF_Object* pElement = it->second;
                          ++it;
                          uint32_t dwObjNum =
                              pElement ? pElement->GetObjNum() : 0;
                          if (dwObjNum) {
                            m_pTrailer->SetAtReference(key, m_pDocument.get(),
                                                       dwObjNum);
                          } else {
                            m_pTrailer->SetAt(key, pElement->Clone());
                          }
                        }
                      }
                      pObj->Release();
                    } else {
                      if (pObj->IsStream()) {
                        m_pTrailer = ToDictionary(pTrailer->Clone());
                        pObj->Release();
                      } else {
                        m_pTrailer = pTrailer;
                      }

                      FX_FILESIZE dwSavePos = m_pSyntax->SavePos();
                      CFX_ByteString strWord = m_pSyntax->GetKeyword();
                      if (!strWord.Compare("startxref")) {
                        bool bNumber;
                        CFX_ByteString bsOffset =
                            m_pSyntax->GetNextWord(&bNumber);
                        if (bNumber)
                          m_LastXRefOffset = FXSYS_atoi(bsOffset.c_str());
                      }
                      m_pSyntax->RestorePos(dwSavePos);
                    }
                  } else {
                    pObj->Release();
                  }
                }
              }
            }
            --i;
            state = ParserState::kDefault;
          } else if (byte == "trailer"[inside_index]) {
            inside_index++;
          } else {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kXref:
          if (inside_index == 4) {
            last_xref = pos + i - 4;
            state = ParserState::kWhitespace;
          } else if (byte == "xref"[inside_index]) {
            inside_index++;
          } else {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kComment:
          if (PDFCharIsLineEnding(byte))
            state = ParserState::kDefault;
          break;

        case ParserState::kString:
          if (byte == ')') {
            if (depth > 0)
              depth--;
          } else if (byte == '(') {
            depth++;
          }

          if (!depth)
            state = ParserState::kDefault;
          break;

        case ParserState::kHexString:
          if (byte == '>' || (byte == '<' && inside_index == 1))
            state = ParserState::kDefault;
          inside_index = 0;
          break;

        case ParserState::kEscapedString:
          if (PDFCharIsDelimiter(byte) || PDFCharIsWhitespace(byte)) {
            --i;
            state = ParserState::kDefault;
          }
          break;

        case ParserState::kEndObj:
          if (PDFCharIsWhitespace(byte)) {
            state = ParserState::kDefault;
          } else if (byte == '%' || byte == '(' || byte == '<' ||
                     byte == '\\') {
            state = ParserState::kDefault;
            --i;
          } else if (inside_index == 6) {
            state = ParserState::kDefault;
            --i;
          } else if (byte == "endobj"[inside_index]) {
            inside_index++;
          }
          break;
      }

      if (bOverFlow) {
        size = 0;
        break;
      }
    }
    pos += size;

    // If the position has not changed at all or went backwards in a loop
    // iteration, then break out to prevent infinite looping.
    if (pos <= saved_pos)
      break;
  }

  if (last_xref != -1 && last_xref > last_obj)
    last_trailer = last_xref;
  else if (last_trailer == -1 || last_xref < last_obj)
    last_trailer = m_pSyntax->m_FileLen;

  m_SortedOffset.insert(last_trailer - m_pSyntax->m_HeaderOffset);
  return m_pTrailer && !m_ObjectInfo.empty();
}

FX_BOOL CPDF_Parser::LoadCrossRefV5(FX_FILESIZE* pos, FX_BOOL bMainXRef) {
  CPDF_Object* pObject = ParseIndirectObjectAt(m_pDocument.get(), *pos, 0);
  if (!pObject)
    return FALSE;

  if (m_pDocument) {
    CPDF_Dictionary* pRootDict = m_pDocument->GetRoot();
    if (pRootDict && pRootDict->GetObjNum() == pObject->m_ObjNum) {
      if (pObject->IsStream())
        pObject->Release();
      return FALSE;
    }
    if (!m_pDocument->InsertIndirectObject(pObject->m_ObjNum, pObject))
      return FALSE;
  }

  CPDF_Stream* pStream = pObject->AsStream();
  if (!pStream)
    return FALSE;

  CPDF_Dictionary* pDict = pStream->GetDict();
  *pos = pDict->GetIntegerBy("Prev");
  int32_t size = pDict->GetIntegerBy("Size");
  if (size < 0) {
    pStream->Release();
    return FALSE;
  }

  CPDF_Dictionary* pNewTrailer = ToDictionary(pDict->Clone());
  if (bMainXRef) {
    m_pTrailer = pNewTrailer;
    ShrinkObjectMap(size);
    for (auto& it : m_ObjectInfo)
      it.second.type = 0;
  } else {
    m_Trailers.Add(pNewTrailer);
  }

  std::vector<std::pair<int32_t, int32_t>> arrIndex;
  CPDF_Array* pArray = pDict->GetArrayBy("Index");
  if (pArray) {
    for (size_t i = 0; i < pArray->GetCount() / 2; i++) {
      CPDF_Object* pStartNumObj = pArray->GetObjectAt(i * 2);
      CPDF_Object* pCountObj = pArray->GetObjectAt(i * 2 + 1);

      if (ToNumber(pStartNumObj) && ToNumber(pCountObj)) {
        int nStartNum = pStartNumObj->GetInteger();
        int nCount = pCountObj->GetInteger();
        if (nStartNum >= 0 && nCount > 0)
          arrIndex.push_back(std::make_pair(nStartNum, nCount));
      }
    }
  }

  if (arrIndex.size() == 0)
    arrIndex.push_back(std::make_pair(0, size));

  pArray = pDict->GetArrayBy("W");
  if (!pArray) {
    pStream->Release();
    return FALSE;
  }

  CFX_ArrayTemplate<uint32_t> WidthArray;
  FX_SAFE_UINT32 dwAccWidth = 0;
  for (size_t i = 0; i < pArray->GetCount(); ++i) {
    WidthArray.Add(pArray->GetIntegerAt(i));
    dwAccWidth += WidthArray[i];
  }

  if (!dwAccWidth.IsValid() || WidthArray.GetSize() < 3) {
    pStream->Release();
    return FALSE;
  }

  uint32_t totalWidth = dwAccWidth.ValueOrDie();
  CPDF_StreamAcc acc;
  acc.LoadAllData(pStream);

  const uint8_t* pData = acc.GetData();
  uint32_t dwTotalSize = acc.GetSize();
  uint32_t segindex = 0;
  for (uint32_t i = 0; i < arrIndex.size(); i++) {
    int32_t startnum = arrIndex[i].first;
    if (startnum < 0)
      continue;

    m_dwXrefStartObjNum =
        pdfium::base::checked_cast<uint32_t, int32_t>(startnum);
    uint32_t count =
        pdfium::base::checked_cast<uint32_t, int32_t>(arrIndex[i].second);
    FX_SAFE_UINT32 dwCaculatedSize = segindex;
    dwCaculatedSize += count;
    dwCaculatedSize *= totalWidth;
    if (!dwCaculatedSize.IsValid() ||
        dwCaculatedSize.ValueOrDie() > dwTotalSize) {
      continue;
    }

    const uint8_t* segstart = pData + segindex * totalWidth;
    FX_SAFE_UINT32 dwMaxObjNum = startnum;
    dwMaxObjNum += count;
    uint32_t dwV5Size = m_ObjectInfo.empty() ? 0 : GetLastObjNum() + 1;
    if (!dwMaxObjNum.IsValid() || dwMaxObjNum.ValueOrDie() > dwV5Size)
      continue;

    for (uint32_t j = 0; j < count; j++) {
      int32_t type = 1;
      const uint8_t* entrystart = segstart + j * totalWidth;
      if (WidthArray[0])
        type = GetVarInt(entrystart, WidthArray[0]);

      if (GetObjectType(startnum + j) == 255) {
        FX_FILESIZE offset =
            GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
        m_ObjectInfo[startnum + j].pos = offset;
        m_SortedOffset.insert(offset);
        continue;
      }

      if (GetObjectType(startnum + j))
        continue;

      m_ObjectInfo[startnum + j].type = type;
      if (type == 0) {
        m_ObjectInfo[startnum + j].pos = 0;
      } else {
        FX_FILESIZE offset =
            GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
        m_ObjectInfo[startnum + j].pos = offset;
        if (type == 1) {
          m_SortedOffset.insert(offset);
        } else {
          if (offset < 0 || !IsValidObjectNumber(offset)) {
            pStream->Release();
            return FALSE;
          }
          m_ObjectInfo[offset].type = 255;
        }
      }
    }
    segindex += count;
  }
  pStream->Release();
  return TRUE;
}

CPDF_Array* CPDF_Parser::GetIDArray() {
  CPDF_Object* pID = m_pTrailer ? m_pTrailer->GetObjectBy("ID") : nullptr;
  if (!pID)
    return nullptr;

  if (CPDF_Reference* pRef = pID->AsReference()) {
    pID = ParseIndirectObject(nullptr, pRef->GetRefObjNum());
    m_pTrailer->SetAt("ID", pID);
  }
  return ToArray(pID);
}

uint32_t CPDF_Parser::GetRootObjNum() {
  CPDF_Reference* pRef =
      ToReference(m_pTrailer ? m_pTrailer->GetObjectBy("Root") : nullptr);
  return pRef ? pRef->GetRefObjNum() : 0;
}

uint32_t CPDF_Parser::GetInfoObjNum() {
  CPDF_Reference* pRef =
      ToReference(m_pTrailer ? m_pTrailer->GetObjectBy("Info") : nullptr);
  return pRef ? pRef->GetRefObjNum() : 0;
}

FX_BOOL CPDF_Parser::IsFormStream(uint32_t objnum, FX_BOOL& bForm) {
  bForm = FALSE;
  if (!IsValidObjectNumber(objnum))
    return TRUE;

  if (GetObjectType(objnum) == 0)
    return TRUE;

  if (GetObjectType(objnum) == 2)
    return TRUE;

  FX_FILESIZE pos = m_ObjectInfo[objnum].pos;
  auto it = m_SortedOffset.find(pos);
  if (it == m_SortedOffset.end())
    return TRUE;

  if (++it == m_SortedOffset.end())
    return FALSE;

  FX_FILESIZE size = *it - pos;
  FX_FILESIZE SavedPos = m_pSyntax->SavePos();
  m_pSyntax->RestorePos(pos);

  const char kFormStream[] = "/Form\0stream";
  const CFX_ByteStringC kFormStreamStr(kFormStream, sizeof(kFormStream) - 1);
  bForm = m_pSyntax->SearchMultiWord(kFormStreamStr, TRUE, size) == 0;
  m_pSyntax->RestorePos(SavedPos);
  return TRUE;
}

CPDF_Object* CPDF_Parser::ParseIndirectObject(
    CPDF_IndirectObjectHolder* pObjList,
    uint32_t objnum) {
  if (!IsValidObjectNumber(objnum))
    return nullptr;

  // Prevent circular parsing the same object.
  if (pdfium::ContainsKey(m_ParsingObjNums, objnum))
    return nullptr;

  pdfium::ScopedSetInsertion<uint32_t> local_insert(&m_ParsingObjNums, objnum);
  if (GetObjectType(objnum) == 1 || GetObjectType(objnum) == 255) {
    FX_FILESIZE pos = m_ObjectInfo[objnum].pos;
    if (pos <= 0)
      return nullptr;
    return ParseIndirectObjectAt(pObjList, pos, objnum);
  }
  if (GetObjectType(objnum) != 2)
    return nullptr;

  CPDF_StreamAcc* pObjStream = GetObjectStream(m_ObjectInfo[objnum].pos);
  if (!pObjStream)
    return nullptr;

  ScopedFileStream file(FX_CreateMemoryStream(
      (uint8_t*)pObjStream->GetData(), (size_t)pObjStream->GetSize(), FALSE));
  CPDF_SyntaxParser syntax;
  syntax.InitParser(file.get(), 0);
  const int32_t offset = GetStreamFirst(pObjStream);

  // Read object numbers from |pObjStream| into a cache.
  if (!pdfium::ContainsKey(m_ObjCache, pObjStream)) {
    for (int32_t i = GetStreamNCount(pObjStream); i > 0; --i) {
      uint32_t thisnum = syntax.GetDirectNum();
      uint32_t thisoff = syntax.GetDirectNum();
      m_ObjCache[pObjStream][thisnum] = thisoff;
    }
  }

  const auto it = m_ObjCache[pObjStream].find(objnum);
  if (it == m_ObjCache[pObjStream].end())
    return nullptr;

  syntax.RestorePos(offset + it->second);
  return syntax.GetObject(pObjList, 0, 0, true);
}

CPDF_StreamAcc* CPDF_Parser::GetObjectStream(uint32_t objnum) {
  auto it = m_ObjectStreamMap.find(objnum);
  if (it != m_ObjectStreamMap.end())
    return it->second.get();

  if (!m_pDocument)
    return nullptr;

  const CPDF_Stream* pStream = ToStream(m_pDocument->GetIndirectObject(objnum));
  if (!pStream)
    return nullptr;

  CPDF_StreamAcc* pStreamAcc = new CPDF_StreamAcc;
  pStreamAcc->LoadAllData(pStream);
  m_ObjectStreamMap[objnum].reset(pStreamAcc);
  return pStreamAcc;
}

FX_FILESIZE CPDF_Parser::GetObjectSize(uint32_t objnum) const {
  if (!IsValidObjectNumber(objnum))
    return 0;

  if (GetObjectType(objnum) == 2)
    objnum = GetObjectPositionOrZero(objnum);

  if (GetObjectType(objnum) != 1 && GetObjectType(objnum) != 255)
    return 0;

  FX_FILESIZE offset = GetObjectPositionOrZero(objnum);
  if (offset == 0)
    return 0;

  auto it = m_SortedOffset.find(offset);
  if (it == m_SortedOffset.end() || ++it == m_SortedOffset.end())
    return 0;

  return *it - offset;
}

void CPDF_Parser::GetIndirectBinary(uint32_t objnum,
                                    uint8_t*& pBuffer,
                                    uint32_t& size) {
  pBuffer = nullptr;
  size = 0;
  if (!IsValidObjectNumber(objnum))
    return;

  if (GetObjectType(objnum) == 2) {
    CPDF_StreamAcc* pObjStream = GetObjectStream(m_ObjectInfo[objnum].pos);
    if (!pObjStream)
      return;

    int32_t offset = GetStreamFirst(pObjStream);
    const uint8_t* pData = pObjStream->GetData();
    uint32_t totalsize = pObjStream->GetSize();
    ScopedFileStream file(
        FX_CreateMemoryStream((uint8_t*)pData, (size_t)totalsize, FALSE));

    CPDF_SyntaxParser syntax;
    syntax.InitParser(file.get(), 0);
    for (int i = GetStreamNCount(pObjStream); i > 0; --i) {
      uint32_t thisnum = syntax.GetDirectNum();
      uint32_t thisoff = syntax.GetDirectNum();
      if (thisnum != objnum)
        continue;

      if (i == 1) {
        size = totalsize - (thisoff + offset);
      } else {
        syntax.GetDirectNum();  // Skip nextnum.
        uint32_t nextoff = syntax.GetDirectNum();
        size = nextoff - thisoff;
      }

      pBuffer = FX_Alloc(uint8_t, size);
      FXSYS_memcpy(pBuffer, pData + thisoff + offset, size);
      return;
    }
    return;
  }

  if (GetObjectType(objnum) != 1)
    return;

  FX_FILESIZE pos = m_ObjectInfo[objnum].pos;
  if (pos == 0)
    return;

  FX_FILESIZE SavedPos = m_pSyntax->SavePos();
  m_pSyntax->RestorePos(pos);

  bool bIsNumber;
  CFX_ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return;
  }

  uint32_t parser_objnum = FXSYS_atoui(word.c_str());
  if (parser_objnum && parser_objnum != objnum) {
    m_pSyntax->RestorePos(SavedPos);
    return;
  }

  word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return;
  }

  if (m_pSyntax->GetKeyword() != "obj") {
    m_pSyntax->RestorePos(SavedPos);
    return;
  }

  auto it = m_SortedOffset.find(pos);
  if (it == m_SortedOffset.end() || ++it == m_SortedOffset.end()) {
    m_pSyntax->RestorePos(SavedPos);
    return;
  }

  FX_FILESIZE nextoff = *it;
  FX_BOOL bNextOffValid = FALSE;
  if (nextoff != pos) {
    m_pSyntax->RestorePos(nextoff);
    word = m_pSyntax->GetNextWord(&bIsNumber);
    if (word == "xref") {
      bNextOffValid = TRUE;
    } else if (bIsNumber) {
      word = m_pSyntax->GetNextWord(&bIsNumber);
      if (bIsNumber && m_pSyntax->GetKeyword() == "obj") {
        bNextOffValid = TRUE;
      }
    }
  }

  if (!bNextOffValid) {
    m_pSyntax->RestorePos(pos);
    while (1) {
      if (m_pSyntax->GetKeyword() == "endobj")
        break;

      if (m_pSyntax->SavePos() == m_pSyntax->m_FileLen)
        break;
    }
    nextoff = m_pSyntax->SavePos();
  }

  size = (uint32_t)(nextoff - pos);
  pBuffer = FX_Alloc(uint8_t, size);
  m_pSyntax->RestorePos(pos);
  m_pSyntax->ReadBlock(pBuffer, size);
  m_pSyntax->RestorePos(SavedPos);
}

CPDF_Object* CPDF_Parser::ParseIndirectObjectAt(
    CPDF_IndirectObjectHolder* pObjList,
    FX_FILESIZE pos,
    uint32_t objnum) {
  FX_FILESIZE SavedPos = m_pSyntax->SavePos();
  m_pSyntax->RestorePos(pos);
  bool bIsNumber;
  CFX_ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  FX_FILESIZE objOffset = m_pSyntax->SavePos();
  objOffset -= word.GetLength();
  uint32_t parser_objnum = FXSYS_atoui(word.c_str());
  if (objnum && parser_objnum != objnum) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  uint32_t parser_gennum = FXSYS_atoui(word.c_str());
  if (m_pSyntax->GetKeyword() != "obj") {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  CPDF_Object* pObj =
      m_pSyntax->GetObject(pObjList, objnum, parser_gennum, true);
  m_pSyntax->SavePos();

  CFX_ByteString bsWord = m_pSyntax->GetKeyword();
  if (bsWord == "endobj")
    m_pSyntax->SavePos();

  m_pSyntax->RestorePos(SavedPos);
  if (pObj) {
    if (!objnum)
      pObj->m_ObjNum = parser_objnum;
    pObj->m_GenNum = parser_gennum;
  }
  return pObj;
}

CPDF_Object* CPDF_Parser::ParseIndirectObjectAtByStrict(
    CPDF_IndirectObjectHolder* pObjList,
    FX_FILESIZE pos,
    uint32_t objnum,
    FX_FILESIZE* pResultPos) {
  FX_FILESIZE SavedPos = m_pSyntax->SavePos();
  m_pSyntax->RestorePos(pos);

  bool bIsNumber;
  CFX_ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  uint32_t parser_objnum = FXSYS_atoui(word.c_str());
  if (objnum && parser_objnum != objnum) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber) {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  uint32_t gennum = FXSYS_atoui(word.c_str());
  if (m_pSyntax->GetKeyword() != "obj") {
    m_pSyntax->RestorePos(SavedPos);
    return nullptr;
  }

  CPDF_Object* pObj = m_pSyntax->GetObjectByStrict(pObjList, objnum, gennum);
  if (pResultPos)
    *pResultPos = m_pSyntax->m_Pos;

  m_pSyntax->RestorePos(SavedPos);
  return pObj;
}

CPDF_Dictionary* CPDF_Parser::LoadTrailerV4() {
  if (m_pSyntax->GetKeyword() != "trailer")
    return nullptr;

  std::unique_ptr<CPDF_Object, ReleaseDeleter<CPDF_Object>> pObj(
      m_pSyntax->GetObject(m_pDocument.get(), 0, 0, true));
  if (!ToDictionary(pObj.get()))
    return nullptr;
  return pObj.release()->AsDictionary();
}

uint32_t CPDF_Parser::GetPermissions() const {
  if (!m_pSecurityHandler)
    return 0xFFFFFFFF;

  uint32_t dwPermission = m_pSecurityHandler->GetPermissions();
  if (m_pEncryptDict && m_pEncryptDict->GetStringBy("Filter") == "Standard") {
    // See PDF Reference 1.7, page 123, table 3.20.
    dwPermission &= 0xFFFFFFFC;
    dwPermission |= 0xFFFFF0C0;
  }
  return dwPermission;
}

FX_BOOL CPDF_Parser::IsLinearizedFile(IFX_FileRead* pFileAccess,
                                      uint32_t offset) {
  m_pSyntax->InitParser(pFileAccess, offset);
  m_pSyntax->RestorePos(m_pSyntax->m_HeaderOffset + 9);

  FX_FILESIZE SavedPos = m_pSyntax->SavePos();
  bool bIsNumber;
  CFX_ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber)
    return FALSE;

  uint32_t objnum = FXSYS_atoui(word.c_str());
  word = m_pSyntax->GetNextWord(&bIsNumber);
  if (!bIsNumber)
    return FALSE;

  uint32_t gennum = FXSYS_atoui(word.c_str());
  if (m_pSyntax->GetKeyword() != "obj") {
    m_pSyntax->RestorePos(SavedPos);
    return FALSE;
  }

  m_pLinearized = m_pSyntax->GetObject(nullptr, objnum, gennum, true);
  if (!m_pLinearized)
    return FALSE;

  CPDF_Dictionary* pDict = m_pLinearized->GetDict();
  if (pDict && pDict->GetObjectBy("Linearized")) {
    m_pSyntax->GetNextWord(nullptr);

    CPDF_Object* pLen = pDict->GetObjectBy("L");
    if (!pLen) {
      m_pLinearized->Release();
      m_pLinearized = nullptr;
      return FALSE;
    }

    if (pLen->GetInteger() != (int)pFileAccess->GetSize())
      return FALSE;

    if (CPDF_Number* pNo = ToNumber(pDict->GetObjectBy("P")))
      m_dwFirstPageNo = pNo->GetInteger();

    if (CPDF_Number* pTable = ToNumber(pDict->GetObjectBy("T")))
      m_LastXRefOffset = pTable->GetInteger();

    return TRUE;
  }
  m_pLinearized->Release();
  m_pLinearized = nullptr;
  return FALSE;
}

CPDF_Parser::Error CPDF_Parser::StartAsyncParse(IFX_FileRead* pFileAccess) {
  CloseParser();
  m_bXRefStream = FALSE;
  m_LastXRefOffset = 0;
  m_bOwnFileRead = true;

  int32_t offset = GetHeaderOffset(pFileAccess);
  if (offset == -1)
    return FORMAT_ERROR;

  if (!IsLinearizedFile(pFileAccess, offset)) {
    m_pSyntax->m_pFileAccess = nullptr;
    return StartParse(pFileAccess);
  }

  m_pDocument.reset(new CPDF_Document(this));
  FX_FILESIZE dwFirstXRefOffset = m_pSyntax->SavePos();

  FX_BOOL bXRefRebuilt = FALSE;
  FX_BOOL bLoadV4 = LoadCrossRefV4(dwFirstXRefOffset, 0, FALSE);
  if (!bLoadV4 && !LoadCrossRefV5(&dwFirstXRefOffset, TRUE)) {
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    bXRefRebuilt = TRUE;
    m_LastXRefOffset = 0;
  }

  if (bLoadV4) {
    m_pTrailer = LoadTrailerV4();
    if (!m_pTrailer)
      return SUCCESS;

    int32_t xrefsize = GetDirectInteger(m_pTrailer, "Size");
    if (xrefsize > 0)
      ShrinkObjectMap(xrefsize);
  }

  Error eRet = SetEncryptHandler();
  if (eRet != SUCCESS)
    return eRet;

  m_pDocument->LoadAsynDoc(m_pLinearized->GetDict());
  if (!m_pDocument->GetRoot() || m_pDocument->GetPageCount() == 0) {
    if (bXRefRebuilt)
      return FORMAT_ERROR;

    ReleaseEncryptHandler();
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;

    m_pDocument->LoadAsynDoc(m_pLinearized->GetDict());
    if (!m_pDocument->GetRoot())
      return FORMAT_ERROR;
  }

  if (GetRootObjNum() == 0) {
    ReleaseEncryptHandler();
    if (!RebuildCrossRef() || GetRootObjNum() == 0)
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;
  }

  if (m_pSecurityHandler && m_pSecurityHandler->IsMetadataEncrypted()) {
    if (CPDF_Reference* pMetadata =
            ToReference(m_pDocument->GetRoot()->GetObjectBy("Metadata")))
      m_pSyntax->m_MetadataObjnum = pMetadata->GetRefObjNum();
  }
  return SUCCESS;
}

FX_BOOL CPDF_Parser::LoadLinearizedAllCrossRefV5(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV5(&xrefpos, FALSE))
    return FALSE;

  std::set<FX_FILESIZE> seen_xrefpos;
  while (xrefpos) {
    seen_xrefpos.insert(xrefpos);
    if (!LoadCrossRefV5(&xrefpos, FALSE))
      return FALSE;

    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return FALSE;
  }
  m_ObjectStreamMap.clear();
  m_bXRefStream = TRUE;
  return TRUE;
}

CPDF_Parser::Error CPDF_Parser::LoadLinearizedMainXRefTable() {
  uint32_t dwSaveMetadataObjnum = m_pSyntax->m_MetadataObjnum;
  m_pSyntax->m_MetadataObjnum = 0;
  if (m_pTrailer) {
    m_pTrailer->Release();
    m_pTrailer = nullptr;
  }

  m_pSyntax->RestorePos(m_LastXRefOffset - m_pSyntax->m_HeaderOffset);
  uint8_t ch = 0;
  uint32_t dwCount = 0;
  m_pSyntax->GetNextChar(ch);
  while (PDFCharIsWhitespace(ch)) {
    ++dwCount;
    if (m_pSyntax->m_FileLen >=
        (FX_FILESIZE)(m_pSyntax->SavePos() + m_pSyntax->m_HeaderOffset)) {
      break;
    }
    m_pSyntax->GetNextChar(ch);
  }
  m_LastXRefOffset += dwCount;
  m_ObjectStreamMap.clear();
  m_ObjCache.clear();

  if (!LoadLinearizedAllCrossRefV4(m_LastXRefOffset, m_dwXrefStartObjNum) &&
      !LoadLinearizedAllCrossRefV5(m_LastXRefOffset)) {
    m_LastXRefOffset = 0;
    m_pSyntax->m_MetadataObjnum = dwSaveMetadataObjnum;
    return FORMAT_ERROR;
  }

  m_pSyntax->m_MetadataObjnum = dwSaveMetadataObjnum;
  return SUCCESS;
}
