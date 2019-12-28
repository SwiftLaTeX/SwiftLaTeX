// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <utility>

#include "core/fxcrt/extension.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_ext.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#include <wincrypt.h>
#else
#include <ctime>
#endif

#ifdef PDF_ENABLE_XFA

CFX_CRTFileAccess::CFX_CRTFileAccess() : m_RefCount(0) {}

CFX_CRTFileAccess::~CFX_CRTFileAccess() {}

void CFX_CRTFileAccess::Release() {
  if (--m_RefCount == 0)
    delete this;
}

IFX_FileAccess* CFX_CRTFileAccess::Retain() {
  m_RefCount++;
  return (IFX_FileAccess*)this;
}

void CFX_CRTFileAccess::GetPath(CFX_WideString& wsPath) {
  wsPath = m_path;
}

IFX_FileStream* CFX_CRTFileAccess::CreateFileStream(uint32_t dwModes) {
  return FX_CreateFileStream(m_path.c_str(), dwModes);
}

FX_BOOL CFX_CRTFileAccess::Init(const CFX_WideStringC& wsPath) {
  m_path = wsPath;
  m_RefCount = 1;
  return TRUE;
}

#endif  // PDF_ENABLE_XFA

CFX_CRTFileStream::CFX_CRTFileStream(std::unique_ptr<IFXCRT_FileAccess> pFA)
    : m_pFile(std::move(pFA)), m_dwCount(1) {}

CFX_CRTFileStream::~CFX_CRTFileStream() {}

CFX_MemoryStream::CFX_MemoryStream(FX_BOOL bConsecutive)
    : m_dwCount(1),
      m_nTotalSize(0),
      m_nCurSize(0),
      m_nCurPos(0),
      m_nGrowSize(FX_MEMSTREAM_BlockSize) {
  m_dwFlags =
      FX_MEMSTREAM_TakeOver | (bConsecutive ? FX_MEMSTREAM_Consecutive : 0);
}

CFX_MemoryStream::CFX_MemoryStream(uint8_t* pBuffer,
                                   size_t nSize,
                                   FX_BOOL bTakeOver)
    : m_dwCount(1),
      m_nTotalSize(nSize),
      m_nCurSize(nSize),
      m_nCurPos(0),
      m_nGrowSize(FX_MEMSTREAM_BlockSize) {
  m_Blocks.Add(pBuffer);
  m_dwFlags =
      FX_MEMSTREAM_Consecutive | (bTakeOver ? FX_MEMSTREAM_TakeOver : 0);
}

CFX_MemoryStream::~CFX_MemoryStream() {
  if (m_dwFlags & FX_MEMSTREAM_TakeOver) {
    for (int32_t i = 0; i < m_Blocks.GetSize(); i++) {
      FX_Free(m_Blocks[i]);
    }
  }
  m_Blocks.RemoveAll();
}

IFX_FileStream* CFX_MemoryStream::Retain() {
  m_dwCount++;
  return this;
}

void CFX_MemoryStream::Release() {
  uint32_t nCount = --m_dwCount;
  if (nCount) {
    return;
  }
  delete this;
}

FX_FILESIZE CFX_MemoryStream::GetSize() {
  return (FX_FILESIZE)m_nCurSize;
}

FX_BOOL CFX_MemoryStream::IsEOF() {
  return m_nCurPos >= (size_t)GetSize();
}

FX_FILESIZE CFX_MemoryStream::GetPosition() {
  return (FX_FILESIZE)m_nCurPos;
}

FX_BOOL CFX_MemoryStream::ReadBlock(void* buffer,
                                    FX_FILESIZE offset,
                                    size_t size) {
  if (!buffer || !size) {
    return FALSE;
  }

  FX_SAFE_SIZE_T newPos = size;
  newPos += offset;
  if (!newPos.IsValid() || newPos.ValueOrDefault(0) == 0 ||
      newPos.ValueOrDie() > m_nCurSize) {
    return FALSE;
  }

  m_nCurPos = newPos.ValueOrDie();
  if (m_dwFlags & FX_MEMSTREAM_Consecutive) {
    FXSYS_memcpy(buffer, m_Blocks[0] + (size_t)offset, size);
    return TRUE;
  }
  size_t nStartBlock = (size_t)offset / m_nGrowSize;
  offset -= (FX_FILESIZE)(nStartBlock * m_nGrowSize);
  while (size) {
    size_t nRead = m_nGrowSize - (size_t)offset;
    if (nRead > size) {
      nRead = size;
    }
    FXSYS_memcpy(buffer, m_Blocks[(int)nStartBlock] + (size_t)offset, nRead);
    buffer = ((uint8_t*)buffer) + nRead;
    size -= nRead;
    nStartBlock++;
    offset = 0;
  }
  return TRUE;
}

size_t CFX_MemoryStream::ReadBlock(void* buffer, size_t size) {
  if (m_nCurPos >= m_nCurSize) {
    return 0;
  }
  size_t nRead = std::min(size, m_nCurSize - m_nCurPos);
  if (!ReadBlock(buffer, (int32_t)m_nCurPos, nRead)) {
    return 0;
  }
  return nRead;
}

FX_BOOL CFX_MemoryStream::WriteBlock(const void* buffer,
                                     FX_FILESIZE offset,
                                     size_t size) {
  if (!buffer || !size) {
    return FALSE;
  }
  if (m_dwFlags & FX_MEMSTREAM_Consecutive) {
    FX_SAFE_SIZE_T newPos = size;
    newPos += offset;
    if (!newPos.IsValid())
      return FALSE;

    m_nCurPos = newPos.ValueOrDie();
    if (m_nCurPos > m_nTotalSize) {
      m_nTotalSize = (m_nCurPos + m_nGrowSize - 1) / m_nGrowSize * m_nGrowSize;
      if (m_Blocks.GetSize() < 1) {
        uint8_t* block = FX_Alloc(uint8_t, m_nTotalSize);
        m_Blocks.Add(block);
      } else {
        m_Blocks[0] = FX_Realloc(uint8_t, m_Blocks[0], m_nTotalSize);
      }
      if (!m_Blocks[0]) {
        m_Blocks.RemoveAll();
        return FALSE;
      }
    }
    FXSYS_memcpy(m_Blocks[0] + (size_t)offset, buffer, size);
    if (m_nCurSize < m_nCurPos) {
      m_nCurSize = m_nCurPos;
    }
    return TRUE;
  }

  FX_SAFE_SIZE_T newPos = size;
  newPos += offset;
  if (!newPos.IsValid()) {
    return FALSE;
  }

  if (!ExpandBlocks(newPos.ValueOrDie())) {
    return FALSE;
  }
  m_nCurPos = newPos.ValueOrDie();
  size_t nStartBlock = (size_t)offset / m_nGrowSize;
  offset -= (FX_FILESIZE)(nStartBlock * m_nGrowSize);
  while (size) {
    size_t nWrite = m_nGrowSize - (size_t)offset;
    if (nWrite > size) {
      nWrite = size;
    }
    FXSYS_memcpy(m_Blocks[(int)nStartBlock] + (size_t)offset, buffer, nWrite);
    buffer = ((uint8_t*)buffer) + nWrite;
    size -= nWrite;
    nStartBlock++;
    offset = 0;
  }
  return TRUE;
}

FX_BOOL CFX_MemoryStream::Flush() {
  return TRUE;
}

FX_BOOL CFX_MemoryStream::IsConsecutive() const {
  return !!(m_dwFlags & FX_MEMSTREAM_Consecutive);
}

void CFX_MemoryStream::EstimateSize(size_t nInitSize, size_t nGrowSize) {
  if (m_dwFlags & FX_MEMSTREAM_Consecutive) {
    if (m_Blocks.GetSize() < 1) {
      uint8_t* pBlock =
          FX_Alloc(uint8_t, std::max(nInitSize, static_cast<size_t>(4096)));
      m_Blocks.Add(pBlock);
    }
    m_nGrowSize = std::max(nGrowSize, static_cast<size_t>(4096));
  } else if (m_Blocks.GetSize() < 1) {
    m_nGrowSize = std::max(nGrowSize, static_cast<size_t>(4096));
  }
}

uint8_t* CFX_MemoryStream::GetBuffer() const {
  return m_Blocks.GetSize() ? m_Blocks[0] : nullptr;
}

void CFX_MemoryStream::AttachBuffer(uint8_t* pBuffer,
                                    size_t nSize,
                                    FX_BOOL bTakeOver) {
  if (!(m_dwFlags & FX_MEMSTREAM_Consecutive)) {
    return;
  }
  m_Blocks.RemoveAll();
  m_Blocks.Add(pBuffer);
  m_nTotalSize = m_nCurSize = nSize;
  m_nCurPos = 0;
  m_dwFlags =
      FX_MEMSTREAM_Consecutive | (bTakeOver ? FX_MEMSTREAM_TakeOver : 0);
}

void CFX_MemoryStream::DetachBuffer() {
  if (!(m_dwFlags & FX_MEMSTREAM_Consecutive)) {
    return;
  }
  m_Blocks.RemoveAll();
  m_nTotalSize = m_nCurSize = m_nCurPos = 0;
  m_dwFlags = FX_MEMSTREAM_TakeOver;
}

FX_BOOL CFX_MemoryStream::ExpandBlocks(size_t size) {
  if (m_nCurSize < size) {
    m_nCurSize = size;
  }
  if (size <= m_nTotalSize) {
    return TRUE;
  }
  int32_t iCount = m_Blocks.GetSize();
  size = (size - m_nTotalSize + m_nGrowSize - 1) / m_nGrowSize;
  m_Blocks.SetSize(m_Blocks.GetSize() + (int32_t)size);
  while (size--) {
    uint8_t* pBlock = FX_Alloc(uint8_t, m_nGrowSize);
    m_Blocks.SetAt(iCount++, pBlock);
    m_nTotalSize += m_nGrowSize;
  }
  return TRUE;
}

IFX_FileStream* CFX_CRTFileStream::Retain() {
  m_dwCount++;
  return this;
}

void CFX_CRTFileStream::Release() {
  uint32_t nCount = --m_dwCount;
  if (!nCount) {
    delete this;
  }
}

FX_FILESIZE CFX_CRTFileStream::GetSize() {
  return m_pFile->GetSize();
}

FX_BOOL CFX_CRTFileStream::IsEOF() {
  return GetPosition() >= GetSize();
}

FX_FILESIZE CFX_CRTFileStream::GetPosition() {
  return m_pFile->GetPosition();
}

FX_BOOL CFX_CRTFileStream::ReadBlock(void* buffer,
                                     FX_FILESIZE offset,
                                     size_t size) {
  return (FX_BOOL)m_pFile->ReadPos(buffer, size, offset);
}

size_t CFX_CRTFileStream::ReadBlock(void* buffer, size_t size) {
  return m_pFile->Read(buffer, size);
}

FX_BOOL CFX_CRTFileStream::WriteBlock(const void* buffer,
                                      FX_FILESIZE offset,
                                      size_t size) {
  return (FX_BOOL)m_pFile->WritePos(buffer, size, offset);
}

FX_BOOL CFX_CRTFileStream::Flush() {
  return m_pFile->Flush();
}

#ifdef PDF_ENABLE_XFA
IFX_FileAccess* FX_CreateDefaultFileAccess(const CFX_WideStringC& wsPath) {
  if (wsPath.GetLength() == 0)
    return nullptr;

  CFX_CRTFileAccess* pFA = new CFX_CRTFileAccess;
  pFA->Init(wsPath);
  return pFA;
}
#endif  // PDF_ENABLE_XFA

IFX_FileStream* FX_CreateFileStream(const FX_CHAR* filename, uint32_t dwModes) {
  std::unique_ptr<IFXCRT_FileAccess> pFA(IFXCRT_FileAccess::Create());
  if (!pFA->Open(filename, dwModes))
    return nullptr;
  return new CFX_CRTFileStream(std::move(pFA));
}

IFX_FileStream* FX_CreateFileStream(const FX_WCHAR* filename,
                                    uint32_t dwModes) {
  std::unique_ptr<IFXCRT_FileAccess> pFA(IFXCRT_FileAccess::Create());
  if (!pFA->Open(filename, dwModes))
    return nullptr;
  return new CFX_CRTFileStream(std::move(pFA));
}
IFX_FileRead* FX_CreateFileRead(const FX_CHAR* filename) {
  return FX_CreateFileStream(filename, FX_FILEMODE_ReadOnly);
}
IFX_FileRead* FX_CreateFileRead(const FX_WCHAR* filename) {
  return FX_CreateFileStream(filename, FX_FILEMODE_ReadOnly);
}
IFX_MemoryStream* FX_CreateMemoryStream(uint8_t* pBuffer,
                                        size_t dwSize,
                                        FX_BOOL bTakeOver) {
  return new CFX_MemoryStream(pBuffer, dwSize, bTakeOver);
}
IFX_MemoryStream* FX_CreateMemoryStream(FX_BOOL bConsecutive) {
  return new CFX_MemoryStream(bConsecutive);
}

FX_FLOAT FXSYS_tan(FX_FLOAT a) {
  return (FX_FLOAT)tan(a);
}
FX_FLOAT FXSYS_logb(FX_FLOAT b, FX_FLOAT x) {
  return FXSYS_log(x) / FXSYS_log(b);
}
FX_FLOAT FXSYS_strtof(const FX_CHAR* pcsStr,
                      int32_t iLength,
                      int32_t* pUsedLen) {
  ASSERT(pcsStr);
  if (iLength < 0) {
    iLength = (int32_t)FXSYS_strlen(pcsStr);
  }
  CFX_WideString ws =
      CFX_WideString::FromLocal(CFX_ByteStringC(pcsStr, iLength));
  return FXSYS_wcstof(ws.c_str(), iLength, pUsedLen);
}
FX_FLOAT FXSYS_wcstof(const FX_WCHAR* pwsStr,
                      int32_t iLength,
                      int32_t* pUsedLen) {
  ASSERT(pwsStr);
  if (iLength < 0) {
    iLength = (int32_t)FXSYS_wcslen(pwsStr);
  }
  if (iLength == 0) {
    return 0.0f;
  }
  int32_t iUsedLen = 0;
  FX_BOOL bNegtive = FALSE;
  switch (pwsStr[iUsedLen]) {
    case '-':
      bNegtive = TRUE;
    case '+':
      iUsedLen++;
      break;
  }
  FX_FLOAT fValue = 0.0f;
  while (iUsedLen < iLength) {
    FX_WCHAR wch = pwsStr[iUsedLen];
    if (wch >= L'0' && wch <= L'9') {
      fValue = fValue * 10.0f + (wch - L'0');
    } else {
      break;
    }
    iUsedLen++;
  }
  if (iUsedLen < iLength && pwsStr[iUsedLen] == L'.') {
    FX_FLOAT fPrecise = 0.1f;
    while (++iUsedLen < iLength) {
      FX_WCHAR wch = pwsStr[iUsedLen];
      if (wch >= L'0' && wch <= L'9') {
        fValue += (wch - L'0') * fPrecise;
        fPrecise *= 0.1f;
      } else {
        break;
      }
    }
  }
  if (pUsedLen) {
    *pUsedLen = iUsedLen;
  }
  return bNegtive ? -fValue : fValue;
}
FX_WCHAR* FXSYS_wcsncpy(FX_WCHAR* dstStr,
                        const FX_WCHAR* srcStr,
                        size_t count) {
  ASSERT(dstStr && srcStr && count > 0);
  for (size_t i = 0; i < count; ++i)
    if ((dstStr[i] = srcStr[i]) == L'\0') {
      break;
    }
  return dstStr;
}
int32_t FXSYS_wcsnicmp(const FX_WCHAR* s1, const FX_WCHAR* s2, size_t count) {
  ASSERT(s1 && s2 && count > 0);
  FX_WCHAR wch1 = 0, wch2 = 0;
  while (count-- > 0) {
    wch1 = (FX_WCHAR)FXSYS_tolower(*s1++);
    wch2 = (FX_WCHAR)FXSYS_tolower(*s2++);
    if (wch1 != wch2) {
      break;
    }
  }
  return wch1 - wch2;
}
int32_t FXSYS_strnicmp(const FX_CHAR* s1, const FX_CHAR* s2, size_t count) {
  ASSERT(s1 && s2 && count > 0);
  FX_CHAR ch1 = 0, ch2 = 0;
  while (count-- > 0) {
    ch1 = (FX_CHAR)FXSYS_tolower(*s1++);
    ch2 = (FX_CHAR)FXSYS_tolower(*s2++);
    if (ch1 != ch2) {
      break;
    }
  }
  return ch1 - ch2;
}

uint32_t FX_HashCode_GetA(const CFX_ByteStringC& str, bool bIgnoreCase) {
  uint32_t dwHashCode = 0;
  if (bIgnoreCase) {
    for (FX_STRSIZE i = 0; i < str.GetLength(); ++i)
      dwHashCode = 31 * dwHashCode + FXSYS_tolower(str.CharAt(i));
  } else {
    for (FX_STRSIZE i = 0; i < str.GetLength(); ++i)
      dwHashCode = 31 * dwHashCode + str.CharAt(i);
  }
  return dwHashCode;
}

uint32_t FX_HashCode_GetW(const CFX_WideStringC& str, bool bIgnoreCase) {
  uint32_t dwHashCode = 0;
  if (bIgnoreCase) {
    for (FX_STRSIZE i = 0; i < str.GetLength(); ++i)
      dwHashCode = 1313 * dwHashCode + FXSYS_tolower(str.CharAt(i));
  } else {
    for (FX_STRSIZE i = 0; i < str.GetLength(); ++i)
      dwHashCode = 1313 * dwHashCode + str.CharAt(i);
  }
  return dwHashCode;
}

void* FX_Random_MT_Start(uint32_t dwSeed) {
  FX_MTRANDOMCONTEXT* pContext = FX_Alloc(FX_MTRANDOMCONTEXT, 1);
  pContext->mt[0] = dwSeed;
  uint32_t& i = pContext->mti;
  uint32_t* pBuf = pContext->mt;
  for (i = 1; i < MT_N; i++) {
    pBuf[i] = (1812433253UL * (pBuf[i - 1] ^ (pBuf[i - 1] >> 30)) + i);
  }
  pContext->bHaveSeed = TRUE;
  return pContext;
}
uint32_t FX_Random_MT_Generate(void* pContext) {
  ASSERT(pContext);
  FX_MTRANDOMCONTEXT* pMTC = static_cast<FX_MTRANDOMCONTEXT*>(pContext);
  uint32_t v;
  static uint32_t mag[2] = {0, MT_Matrix_A};
  uint32_t& mti = pMTC->mti;
  uint32_t* pBuf = pMTC->mt;
  if ((int)mti < 0 || mti >= MT_N) {
    if (mti > MT_N && !pMTC->bHaveSeed) {
      return 0;
    }
    uint32_t kk;
    for (kk = 0; kk < MT_N - MT_M; kk++) {
      v = (pBuf[kk] & MT_Upper_Mask) | (pBuf[kk + 1] & MT_Lower_Mask);
      pBuf[kk] = pBuf[kk + MT_M] ^ (v >> 1) ^ mag[v & 1];
    }
    for (; kk < MT_N - 1; kk++) {
      v = (pBuf[kk] & MT_Upper_Mask) | (pBuf[kk + 1] & MT_Lower_Mask);
      pBuf[kk] = pBuf[kk + (MT_M - MT_N)] ^ (v >> 1) ^ mag[v & 1];
    }
    v = (pBuf[MT_N - 1] & MT_Upper_Mask) | (pBuf[0] & MT_Lower_Mask);
    pBuf[MT_N - 1] = pBuf[MT_M - 1] ^ (v >> 1) ^ mag[v & 1];
    mti = 0;
  }
  v = pBuf[mti++];
  v ^= (v >> 11);
  v ^= (v << 7) & 0x9d2c5680UL;
  v ^= (v << 15) & 0xefc60000UL;
  v ^= (v >> 18);
  return v;
}
void FX_Random_MT_Close(void* pContext) {
  ASSERT(pContext);
  FX_Free(pContext);
}
void FX_Random_GenerateMT(uint32_t* pBuffer, int32_t iCount) {
  uint32_t dwSeed;
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  if (!FX_GenerateCryptoRandom(&dwSeed, 1)) {
    FX_Random_GenerateBase(&dwSeed, 1);
  }
#else
  FX_Random_GenerateBase(&dwSeed, 1);
#endif
  void* pContext = FX_Random_MT_Start(dwSeed);
  while (iCount-- > 0) {
    *pBuffer++ = FX_Random_MT_Generate(pContext);
  }
  FX_Random_MT_Close(pContext);
}
void FX_Random_GenerateBase(uint32_t* pBuffer, int32_t iCount) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  SYSTEMTIME st1, st2;
  ::GetSystemTime(&st1);
  do {
    ::GetSystemTime(&st2);
  } while (FXSYS_memcmp(&st1, &st2, sizeof(SYSTEMTIME)) == 0);
  uint32_t dwHash1 =
      FX_HashCode_GetA(CFX_ByteStringC((uint8_t*)&st1, sizeof(st1)), true);
  uint32_t dwHash2 =
      FX_HashCode_GetA(CFX_ByteStringC((uint8_t*)&st2, sizeof(st2)), true);
  ::srand((dwHash1 << 16) | (uint32_t)dwHash2);
#else
  time_t tmLast = time(nullptr);
  time_t tmCur;
  while ((tmCur = time(nullptr)) == tmLast) {
    continue;
  }

  ::srand((tmCur << 16) | (tmLast & 0xFFFF));
#endif
  while (iCount-- > 0) {
    *pBuffer++ = (uint32_t)((::rand() << 16) | (::rand() & 0xFFFF));
  }
}
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
FX_BOOL FX_GenerateCryptoRandom(uint32_t* pBuffer, int32_t iCount) {
  HCRYPTPROV hCP = 0;
  if (!::CryptAcquireContext(&hCP, nullptr, nullptr, PROV_RSA_FULL, 0) ||
      !hCP) {
    return FALSE;
  }
  ::CryptGenRandom(hCP, iCount * sizeof(uint32_t), (uint8_t*)pBuffer);
  ::CryptReleaseContext(hCP, 0);
  return TRUE;
}
#endif
void FX_Random_GenerateCrypto(uint32_t* pBuffer, int32_t iCount) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  FX_GenerateCryptoRandom(pBuffer, iCount);
#else
  FX_Random_GenerateBase(pBuffer, iCount);
#endif
}

#ifdef PDF_ENABLE_XFA
void FX_GUID_CreateV4(FX_LPGUID pGUID) {
  FX_Random_GenerateMT((uint32_t*)pGUID, 4);
  uint8_t& b = ((uint8_t*)pGUID)[6];
  b = (b & 0x0F) | 0x40;
}
const FX_CHAR* gs_FX_pHexChars = "0123456789ABCDEF";
void FX_GUID_ToString(FX_LPCGUID pGUID,
                      CFX_ByteString& bsStr,
                      FX_BOOL bSeparator) {
  FX_CHAR* pBuf = bsStr.GetBuffer(40);
  uint8_t b;
  for (int32_t i = 0; i < 16; i++) {
    b = ((const uint8_t*)pGUID)[i];
    *pBuf++ = gs_FX_pHexChars[b >> 4];
    *pBuf++ = gs_FX_pHexChars[b & 0x0F];
    if (bSeparator && (i == 3 || i == 5 || i == 7 || i == 9)) {
      *pBuf++ = L'-';
    }
  }
  bsStr.ReleaseBuffer(bSeparator ? 36 : 32);
}
#endif  // PDF_ENABLE_XFA
