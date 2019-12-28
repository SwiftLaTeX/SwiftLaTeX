// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/crt/fgas_stream.h"

#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN32_MOBILE_ || \
    _FX_OS_ == _FX_WIN64_
#include <io.h>
#endif

#include <algorithm>
#include <memory>

#include "xfa/fgas/crt/fgas_codepage.h"
#include "xfa/fgas/crt/fgas_system.h"

namespace {

class IFX_StreamImp {
 public:
  virtual ~IFX_StreamImp() {}

  virtual int32_t GetLength() const = 0;
  virtual int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) = 0;
  virtual int32_t GetPosition() = 0;
  virtual FX_BOOL IsEOF() const = 0;
  virtual int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) = 0;
  virtual int32_t ReadString(FX_WCHAR* pStr,
                             int32_t iMaxLength,
                             FX_BOOL& bEOS) = 0;
  virtual int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) = 0;
  virtual int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) = 0;
  virtual void Flush() = 0;
  virtual FX_BOOL SetLength(int32_t iLength) = 0;

 protected:
  IFX_StreamImp();

  uint32_t GetAccessModes() const { return m_dwAccess; }
  void SetAccessModes(uint32_t modes) { m_dwAccess = modes; }

 private:
  uint32_t m_dwAccess;
};

class CFX_FileStreamImp : public IFX_StreamImp {
 public:
  CFX_FileStreamImp();
  ~CFX_FileStreamImp() override;

  FX_BOOL LoadFile(const FX_WCHAR* pszSrcFileName, uint32_t dwAccess);

  // IFX_StreamImp:
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override;
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override;
  void Flush() override;
  FX_BOOL SetLength(int32_t iLength) override;

 protected:
  FXSYS_FILE* m_hFile;
  int32_t m_iLength;
};

class CFX_BufferStreamImp : public IFX_StreamImp {
 public:
  CFX_BufferStreamImp();
  ~CFX_BufferStreamImp() override {}

  FX_BOOL LoadBuffer(uint8_t* pData, int32_t iTotalSize, uint32_t dwAccess);

  // IFX_StreamImp:
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override;
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override;
  void Flush() override {}
  FX_BOOL SetLength(int32_t iLength) override { return FALSE; }

 protected:
  uint8_t* m_pData;
  int32_t m_iTotalSize;
  int32_t m_iPosition;
  int32_t m_iLength;
};

class CFX_FileReadStreamImp : public IFX_StreamImp {
 public:
  CFX_FileReadStreamImp();
  ~CFX_FileReadStreamImp() override {}

  FX_BOOL LoadFileRead(IFX_FileRead* pFileRead, uint32_t dwAccess);

  // IFX_StreamImp:
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override { return m_iPosition; }
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override {
    return 0;
  }
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override {
    return 0;
  }
  void Flush() override {}
  FX_BOOL SetLength(int32_t iLength) override { return FALSE; }

 protected:
  IFX_FileRead* m_pFileRead;
  int32_t m_iPosition;
  int32_t m_iLength;
};

class CFX_BufferReadStreamImp : public IFX_StreamImp {
 public:
  CFX_BufferReadStreamImp();
  ~CFX_BufferReadStreamImp() override;

  FX_BOOL LoadBufferRead(IFX_BufferRead* pBufferRead,
                         int32_t iFileSize,
                         uint32_t dwAccess,
                         FX_BOOL bReleaseBufferRead);

  // IFX_StreamImp:
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override { return m_iPosition; }
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override {
    return 0;
  }
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override {
    return 0;
  }
  void Flush() override {}
  FX_BOOL SetLength(int32_t iLength) override { return FALSE; }

 private:
  IFX_BufferRead* m_pBufferRead;
  FX_BOOL m_bReleaseBufferRead;
  int32_t m_iPosition;
  int32_t m_iBufferSize;
};

class CFX_FileWriteStreamImp : public IFX_StreamImp {
 public:
  CFX_FileWriteStreamImp();
  ~CFX_FileWriteStreamImp() override {}

  FX_BOOL LoadFileWrite(IFX_FileWrite* pFileWrite, uint32_t dwAccess);

  // IFX_StreamImp:
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override { return m_iPosition; }
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override { return 0; }
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS) override {
    return 0;
  }
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override;
  void Flush() override;
  FX_BOOL SetLength(int32_t iLength) override { return FALSE; }

 protected:
  IFX_FileWrite* m_pFileWrite;
  int32_t m_iPosition;
};

enum FX_STREAMTYPE {
  FX_SREAMTYPE_Unknown = 0,
  FX_STREAMTYPE_File,
  FX_STREAMTYPE_Buffer,
  FX_STREAMTYPE_Stream,
  FX_STREAMTYPE_BufferRead,
};

class CFX_Stream : public IFX_Stream {
 public:
  CFX_Stream();
  ~CFX_Stream() override;

  FX_BOOL LoadFile(const FX_WCHAR* pszSrcFileName, uint32_t dwAccess);
  FX_BOOL LoadBuffer(uint8_t* pData, int32_t iTotalSize, uint32_t dwAccess);
  FX_BOOL LoadFileRead(IFX_FileRead* pFileRead, uint32_t dwAccess);
  FX_BOOL LoadFileWrite(IFX_FileWrite* pFileWrite, uint32_t dwAccess);
  FX_BOOL LoadBufferRead(IFX_BufferRead* pBufferRead,
                         int32_t iFileSize,
                         uint32_t dwAccess,
                         FX_BOOL bReleaseBufferRead);

  // IFX_Stream
  void Release() override;
  IFX_Stream* Retain() override;
  uint32_t GetAccessModes() const override;
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override;
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS,
                     int32_t const* pByteSize = nullptr) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override;
  void Flush() override;
  FX_BOOL SetLength(int32_t iLength) override;
  int32_t GetBOM(uint8_t bom[4]) const override;
  uint16_t GetCodePage() const override;
  uint16_t SetCodePage(uint16_t wCodePage) override;
  IFX_Stream* CreateSharedStream(uint32_t dwAccess,
                                 int32_t iOffset,
                                 int32_t iLength) override;

 protected:
  FX_STREAMTYPE m_eStreamType;
  IFX_StreamImp* m_pStreamImp;
  uint32_t m_dwAccess;
  int32_t m_iTotalSize;
  int32_t m_iPosition;
  int32_t m_iStart;
  int32_t m_iLength;
  int32_t m_iRefCount;
};

class CFX_TextStream : public IFX_Stream {
 public:
  CFX_TextStream(IFX_Stream* pStream, FX_BOOL bDelStream);
  ~CFX_TextStream() override;

  // IFX_Stream
  void Release() override;
  IFX_Stream* Retain() override;
  uint32_t GetAccessModes() const override;
  int32_t GetLength() const override;
  int32_t Seek(FX_STREAMSEEK eSeek, int32_t iOffset) override;
  int32_t GetPosition() override;
  FX_BOOL IsEOF() const override;
  int32_t ReadData(uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t ReadString(FX_WCHAR* pStr,
                     int32_t iMaxLength,
                     FX_BOOL& bEOS,
                     int32_t const* pByteSize = nullptr) override;
  int32_t WriteData(const uint8_t* pBuffer, int32_t iBufferSize) override;
  int32_t WriteString(const FX_WCHAR* pStr, int32_t iLength) override;
  void Flush() override;
  FX_BOOL SetLength(int32_t iLength) override;
  int32_t GetBOM(uint8_t bom[4]) const override;
  uint16_t GetCodePage() const override;
  uint16_t SetCodePage(uint16_t wCodePage) override;
  IFX_Stream* CreateSharedStream(uint32_t dwAccess,
                                 int32_t iOffset,
                                 int32_t iLength) override;

 protected:
  uint16_t m_wCodePage;
  int32_t m_wBOMLength;
  uint32_t m_dwBOM;
  uint8_t* m_pBuf;
  int32_t m_iBufSize;
  FX_BOOL m_bDelStream;
  IFX_Stream* m_pStreamImp;
  int32_t m_iRefCount;
  void InitStream();
};

class CFGAS_FileRead : public IFX_FileRead {
 public:
  CFGAS_FileRead(IFX_Stream* pStream, FX_BOOL bReleaseStream);
  ~CFGAS_FileRead() override;

  // IFX_FileRead
  void Release() override;
  FX_FILESIZE GetSize() override;
  FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) override;

 protected:
  FX_BOOL m_bReleaseStream;
  IFX_Stream* m_pStream;
};

int32_t FileLength(FXSYS_FILE* file) {
  ASSERT(file);
#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_
  return _filelength(_fileno(file));
#else
  int32_t iPos = FXSYS_ftell(file);
  FXSYS_fseek(file, 0, FXSYS_SEEK_END);
  int32_t iLen = FXSYS_ftell(file);
  FXSYS_fseek(file, iPos, FXSYS_SEEK_SET);
  return iLen;
#endif
}

FX_BOOL FileSetSize(FXSYS_FILE* file, int32_t size) {
  ASSERT(file);
#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN64_
  return _chsize(_fileno(file), size) == 0;
#elif _FX_OS_ == _FX_WIN32_MOBILE_
  HANDLE hFile = _fileno(file);
  uint32_t dwPos = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);
  ::SetFilePointer(hFile, size, 0, FILE_BEGIN);
  FX_BOOL bRet = ::SetEndOfFile(hFile);
  ::SetFilePointer(hFile, (int32_t)dwPos, 0, FILE_BEGIN);
  return bRet;
#else
  return FALSE;
#endif
}

}  // namespace

IFX_Stream* IFX_Stream::CreateStream(IFX_BufferRead* pBufferRead,
                                     uint32_t dwAccess,
                                     int32_t iFileSize,
                                     FX_BOOL bReleaseBufferRead) {
  CFX_Stream* pSR = new CFX_Stream;
  if (!pSR->LoadBufferRead(pBufferRead, iFileSize, dwAccess,
                           bReleaseBufferRead)) {
    pSR->Release();
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}
IFX_Stream* IFX_Stream::CreateStream(IFX_FileRead* pFileRead,
                                     uint32_t dwAccess) {
  CFX_Stream* pSR = new CFX_Stream;
  if (!pSR->LoadFileRead(pFileRead, dwAccess)) {
    pSR->Release();
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}
IFX_Stream* IFX_Stream::CreateStream(IFX_FileWrite* pFileWrite,
                                     uint32_t dwAccess) {
  CFX_Stream* pSR = new CFX_Stream;
  if (!pSR->LoadFileWrite(pFileWrite, dwAccess)) {
    pSR->Release();
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}
IFX_Stream* IFX_Stream::CreateStream(const FX_WCHAR* pszFileName,
                                     uint32_t dwAccess) {
  CFX_Stream* pSR = new CFX_Stream;
  if (!pSR->LoadFile(pszFileName, dwAccess)) {
    pSR->Release();
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}
IFX_Stream* IFX_Stream::CreateStream(uint8_t* pData,
                                     int32_t length,
                                     uint32_t dwAccess) {
  CFX_Stream* pSR = new CFX_Stream;
  if (!pSR->LoadBuffer(pData, length, dwAccess)) {
    pSR->Release();
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}

IFX_StreamImp::IFX_StreamImp() : m_dwAccess(0) {}

CFX_FileStreamImp::CFX_FileStreamImp() : m_hFile(nullptr), m_iLength(0) {}

CFX_FileStreamImp::~CFX_FileStreamImp() {
  if (m_hFile)
    FXSYS_fclose(m_hFile);
}

FX_BOOL CFX_FileStreamImp::LoadFile(const FX_WCHAR* pszSrcFileName,
                                    uint32_t dwAccess) {
  ASSERT(!m_hFile);
  ASSERT(pszSrcFileName && FXSYS_wcslen(pszSrcFileName) > 0);
#if _FX_OS_ == _FX_WIN32_DESKTOP_ || _FX_OS_ == _FX_WIN32_MOBILE_ || \
    _FX_OS_ == _FX_WIN64_
  const FX_WCHAR* wsMode;
  if (dwAccess & FX_STREAMACCESS_Write) {
    if (dwAccess & FX_STREAMACCESS_Append) {
      wsMode = L"a+b";
    } else if (dwAccess & FX_STREAMACCESS_Truncate) {
      wsMode = L"w+b";
    } else {
      wsMode = L"r+b";
    }
  } else {
    wsMode = L"rb";
  }
  m_hFile = FXSYS_wfopen(pszSrcFileName, wsMode);

  if (!m_hFile) {
    if (dwAccess & FX_STREAMACCESS_Write) {
      if (dwAccess & FX_STREAMACCESS_Create)
        m_hFile = FXSYS_wfopen(pszSrcFileName, L"w+b");

      if (!m_hFile) {
        m_hFile = FXSYS_wfopen(pszSrcFileName, L"r+b");
        if (!m_hFile)
          return FALSE;

        if (dwAccess & FX_STREAMACCESS_Truncate)
          FileSetSize(m_hFile, 0);
      }
    } else {
      return FALSE;
    }
  }
#else
  const FX_CHAR* wsMode = "rb";
  if (dwAccess & FX_STREAMACCESS_Write) {
    if (dwAccess & FX_STREAMACCESS_Append) {
      wsMode = "a+b";
    } else if (dwAccess & FX_STREAMACCESS_Truncate) {
      wsMode = "w+b";
    } else {
      wsMode = "r+b";
    }
  }
  CFX_ByteString szFileName = CFX_ByteString::FromUnicode(pszSrcFileName);
  m_hFile = FXSYS_fopen(szFileName.c_str(), wsMode);
  if (!m_hFile) {
    if (dwAccess & FX_STREAMACCESS_Write) {
      if (dwAccess & FX_STREAMACCESS_Create) {
        m_hFile = FXSYS_fopen(szFileName.c_str(), "w+b");
      }
      if (!m_hFile) {
        m_hFile = FXSYS_fopen(szFileName.c_str(), "r+b");
        if (!m_hFile) {
          return FALSE;
        }
        if (dwAccess & FX_STREAMACCESS_Truncate) {
          FileSetSize(m_hFile, 0);
        }
      }
    } else {
      return FALSE;
    }
  }
#endif
  SetAccessModes(dwAccess);
  if ((dwAccess & (FX_STREAMACCESS_Write | FX_STREAMACCESS_Truncate)) ==
      (FX_STREAMACCESS_Write | FX_STREAMACCESS_Truncate)) {
    m_iLength = 0;
  } else {
    m_iLength = FileLength(m_hFile);
  }
  return TRUE;
}
int32_t CFX_FileStreamImp::GetLength() const {
  ASSERT(m_hFile);
  return m_iLength;
}
int32_t CFX_FileStreamImp::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  ASSERT(m_hFile);
  FXSYS_fseek(m_hFile, iOffset, eSeek);
  return FXSYS_ftell(m_hFile);
}
int32_t CFX_FileStreamImp::GetPosition() {
  ASSERT(m_hFile);
  return FXSYS_ftell(m_hFile);
}
FX_BOOL CFX_FileStreamImp::IsEOF() const {
  ASSERT(m_hFile);
  return FXSYS_ftell(m_hFile) >= m_iLength;
}
int32_t CFX_FileStreamImp::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  ASSERT(m_hFile);
  ASSERT(pBuffer && iBufferSize > 0);
  return FXSYS_fread(pBuffer, 1, iBufferSize, m_hFile);
}
int32_t CFX_FileStreamImp::ReadString(FX_WCHAR* pStr,
                                      int32_t iMaxLength,
                                      FX_BOOL& bEOS) {
  ASSERT(m_hFile);
  ASSERT(pStr && iMaxLength > 0);
  if (m_iLength <= 0) {
    return 0;
  }
  int32_t iPosition = FXSYS_ftell(m_hFile);
  int32_t iLen = std::min((m_iLength - iPosition) / 2, iMaxLength);
  if (iLen <= 0) {
    return 0;
  }
  iLen = FXSYS_fread(pStr, 2, iLen, m_hFile);
  int32_t iCount = 0;
  while (*pStr != L'\0' && iCount < iLen) {
    pStr++, iCount++;
  }
  iPosition += iCount * 2;
  if (FXSYS_ftell(m_hFile) != iPosition) {
    FXSYS_fseek(m_hFile, iPosition, 0);
  }
  bEOS = (iPosition >= m_iLength);
  return iCount;
}
int32_t CFX_FileStreamImp::WriteData(const uint8_t* pBuffer,
                                     int32_t iBufferSize) {
  ASSERT(m_hFile && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  ASSERT(pBuffer && iBufferSize > 0);
  int32_t iRet = FXSYS_fwrite(pBuffer, 1, iBufferSize, m_hFile);
  if (iRet != 0) {
    int32_t iPos = FXSYS_ftell(m_hFile);
    if (iPos > m_iLength) {
      m_iLength = iPos;
    }
  }
  return iRet;
}
int32_t CFX_FileStreamImp::WriteString(const FX_WCHAR* pStr, int32_t iLength) {
  ASSERT(m_hFile && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  ASSERT(pStr && iLength > 0);
  int32_t iRet = FXSYS_fwrite(pStr, 2, iLength, m_hFile);
  if (iRet != 0) {
    int32_t iPos = FXSYS_ftell(m_hFile);
    if (iPos > m_iLength) {
      m_iLength = iPos;
    }
  }
  return iRet;
}
void CFX_FileStreamImp::Flush() {
  ASSERT(m_hFile && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  FXSYS_fflush(m_hFile);
}
FX_BOOL CFX_FileStreamImp::SetLength(int32_t iLength) {
  ASSERT(m_hFile && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  FX_BOOL bRet = FileSetSize(m_hFile, iLength);
  m_iLength = FileLength(m_hFile);
  return bRet;
}
CFX_FileReadStreamImp::CFX_FileReadStreamImp()
    : m_pFileRead(nullptr), m_iPosition(0), m_iLength(0) {}
FX_BOOL CFX_FileReadStreamImp::LoadFileRead(IFX_FileRead* pFileRead,
                                            uint32_t dwAccess) {
  ASSERT(!m_pFileRead && pFileRead);
  if (dwAccess & FX_STREAMACCESS_Write) {
    return FALSE;
  }
  m_pFileRead = pFileRead;
  m_iLength = m_pFileRead->GetSize();
  return TRUE;
}
int32_t CFX_FileReadStreamImp::GetLength() const {
  return m_iLength;
}
int32_t CFX_FileReadStreamImp::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  switch (eSeek) {
    case FX_STREAMSEEK_Begin:
      m_iPosition = iOffset;
      break;
    case FX_STREAMSEEK_Current:
      m_iPosition += iOffset;
      break;
    case FX_STREAMSEEK_End:
      m_iPosition = m_iLength + iOffset;
      break;
  }
  if (m_iPosition < 0) {
    m_iPosition = 0;
  } else if (m_iPosition >= m_iLength) {
    m_iPosition = m_iLength;
  }
  return m_iPosition;
}
FX_BOOL CFX_FileReadStreamImp::IsEOF() const {
  return m_iPosition >= m_iLength;
}
int32_t CFX_FileReadStreamImp::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  ASSERT(m_pFileRead);
  ASSERT(pBuffer && iBufferSize > 0);
  if (iBufferSize > m_iLength - m_iPosition) {
    iBufferSize = m_iLength - m_iPosition;
  }
  if (m_pFileRead->ReadBlock(pBuffer, m_iPosition, iBufferSize)) {
    m_iPosition += iBufferSize;
    return iBufferSize;
  }
  return 0;
}
int32_t CFX_FileReadStreamImp::ReadString(FX_WCHAR* pStr,
                                          int32_t iMaxLength,
                                          FX_BOOL& bEOS) {
  ASSERT(m_pFileRead);
  ASSERT(pStr && iMaxLength > 0);
  iMaxLength = ReadData((uint8_t*)pStr, iMaxLength * 2) / 2;
  if (iMaxLength <= 0) {
    return 0;
  }
  int32_t i = 0;
  while (i < iMaxLength && pStr[i] != L'\0') {
    ++i;
  }
  bEOS = (m_iPosition >= m_iLength) || pStr[i] == L'\0';
  return i;
}
CFX_BufferReadStreamImp::CFX_BufferReadStreamImp()
    : m_pBufferRead(nullptr),
      m_bReleaseBufferRead(FALSE),
      m_iPosition(0),
      m_iBufferSize(0) {}
CFX_BufferReadStreamImp::~CFX_BufferReadStreamImp() {
  if (m_bReleaseBufferRead && m_pBufferRead) {
    m_pBufferRead->Release();
  }
}
FX_BOOL CFX_BufferReadStreamImp::LoadBufferRead(IFX_BufferRead* pBufferRead,
                                                int32_t iFileSize,
                                                uint32_t dwAccess,
                                                FX_BOOL bReleaseBufferRead) {
  ASSERT(!m_pBufferRead && pBufferRead);
  if (dwAccess & FX_STREAMACCESS_Write) {
    return FALSE;
  }
  m_bReleaseBufferRead = bReleaseBufferRead;
  m_pBufferRead = pBufferRead;
  m_iBufferSize = iFileSize;
  if (m_iBufferSize >= 0) {
    return TRUE;
  }
  if (!m_pBufferRead->ReadNextBlock(TRUE)) {
    return FALSE;
  }
  m_iBufferSize = m_pBufferRead->GetBlockSize();
  while (!m_pBufferRead->IsEOF()) {
    m_pBufferRead->ReadNextBlock(FALSE);
    m_iBufferSize += m_pBufferRead->GetBlockSize();
  }
  return TRUE;
}
int32_t CFX_BufferReadStreamImp::GetLength() const {
  return m_iBufferSize;
}
int32_t CFX_BufferReadStreamImp::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  int32_t iLength = GetLength();
  switch (eSeek) {
    case FX_STREAMSEEK_Begin:
      m_iPosition = iOffset;
      break;
    case FX_STREAMSEEK_Current:
      m_iPosition += iOffset;
      break;
    case FX_STREAMSEEK_End:
      m_iPosition = iLength + iOffset;
      break;
  }
  if (m_iPosition < 0) {
    m_iPosition = 0;
  } else if (m_iPosition >= iLength) {
    m_iPosition = iLength;
  }
  return m_iPosition;
}
FX_BOOL CFX_BufferReadStreamImp::IsEOF() const {
  return m_pBufferRead ? m_pBufferRead->IsEOF() : TRUE;
}
int32_t CFX_BufferReadStreamImp::ReadData(uint8_t* pBuffer,
                                          int32_t iBufferSize) {
  ASSERT(m_pBufferRead);
  ASSERT(pBuffer && iBufferSize > 0);
  int32_t iLength = GetLength();
  if (m_iPosition >= iLength) {
    return 0;
  }
  if (iBufferSize > iLength - m_iPosition) {
    iBufferSize = iLength - m_iPosition;
  }
  uint32_t dwBlockOffset = m_pBufferRead->GetBlockOffset();
  uint32_t dwBlockSize = m_pBufferRead->GetBlockSize();
  if (m_iPosition < (int32_t)dwBlockOffset) {
    if (!m_pBufferRead->ReadNextBlock(TRUE)) {
      return 0;
    }
    dwBlockOffset = m_pBufferRead->GetBlockOffset();
    dwBlockSize = m_pBufferRead->GetBlockSize();
  }
  while (m_iPosition < (int32_t)dwBlockOffset ||
         m_iPosition >= (int32_t)(dwBlockOffset + dwBlockSize)) {
    if (m_pBufferRead->IsEOF() || !m_pBufferRead->ReadNextBlock(FALSE)) {
      break;
    }
    dwBlockOffset = m_pBufferRead->GetBlockOffset();
    dwBlockSize = m_pBufferRead->GetBlockSize();
  }
  if (m_iPosition < (int32_t)dwBlockOffset ||
      m_iPosition >= (int32_t)(dwBlockOffset + dwBlockSize)) {
    return 0;
  }
  const uint8_t* pBufferTmp = m_pBufferRead->GetBlockBuffer();
  uint32_t dwOffsetTmp = m_iPosition - dwBlockOffset;
  uint32_t dwCopySize =
      std::min(iBufferSize, (int32_t)(dwBlockSize - dwOffsetTmp));
  FXSYS_memcpy(pBuffer, pBufferTmp + dwOffsetTmp, dwCopySize);
  dwOffsetTmp = dwCopySize;
  iBufferSize -= dwCopySize;
  while (iBufferSize > 0) {
    if (!m_pBufferRead->ReadNextBlock(FALSE)) {
      break;
    }
    dwBlockOffset = m_pBufferRead->GetBlockOffset();
    dwBlockSize = m_pBufferRead->GetBlockSize();
    pBufferTmp = m_pBufferRead->GetBlockBuffer();
    dwCopySize = std::min((uint32_t)iBufferSize, dwBlockSize);
    FXSYS_memcpy(pBuffer + dwOffsetTmp, pBufferTmp, dwCopySize);
    dwOffsetTmp += dwCopySize;
    iBufferSize -= dwCopySize;
  }
  m_iPosition += dwOffsetTmp;
  return dwOffsetTmp;
}
int32_t CFX_BufferReadStreamImp::ReadString(FX_WCHAR* pStr,
                                            int32_t iMaxLength,
                                            FX_BOOL& bEOS) {
  ASSERT(m_pBufferRead);
  ASSERT(pStr && iMaxLength > 0);
  iMaxLength = ReadData((uint8_t*)pStr, iMaxLength * 2) / 2;
  if (iMaxLength <= 0) {
    return 0;
  }
  int32_t i = 0;
  while (i < iMaxLength && pStr[i] != L'\0') {
    ++i;
  }
  bEOS = (m_iPosition >= GetLength()) || pStr[i] == L'\0';
  return i;
}
CFX_FileWriteStreamImp::CFX_FileWriteStreamImp()
    : m_pFileWrite(nullptr), m_iPosition(0) {}
FX_BOOL CFX_FileWriteStreamImp::LoadFileWrite(IFX_FileWrite* pFileWrite,
                                              uint32_t dwAccess) {
  ASSERT(!m_pFileWrite && pFileWrite);
  if (dwAccess & FX_STREAMACCESS_Read) {
    return FALSE;
  }
  if (dwAccess & FX_STREAMACCESS_Append) {
    m_iPosition = pFileWrite->GetSize();
  }
  m_pFileWrite = pFileWrite;
  return TRUE;
}
int32_t CFX_FileWriteStreamImp::GetLength() const {
  if (!m_pFileWrite) {
    return 0;
  }
  return (int32_t)m_pFileWrite->GetSize();
}
int32_t CFX_FileWriteStreamImp::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  int32_t iLength = GetLength();
  switch (eSeek) {
    case FX_STREAMSEEK_Begin:
      m_iPosition = iOffset;
      break;
    case FX_STREAMSEEK_Current:
      m_iPosition += iOffset;
      break;
    case FX_STREAMSEEK_End:
      m_iPosition = iLength + iOffset;
      break;
  }
  if (m_iPosition < 0) {
    m_iPosition = 0;
  } else if (m_iPosition >= iLength) {
    m_iPosition = iLength;
  }
  return m_iPosition;
}
FX_BOOL CFX_FileWriteStreamImp::IsEOF() const {
  return m_iPosition >= GetLength();
}
int32_t CFX_FileWriteStreamImp::WriteData(const uint8_t* pBuffer,
                                          int32_t iBufferSize) {
  if (!m_pFileWrite) {
    return 0;
  }
  if (m_pFileWrite->WriteBlock(pBuffer, m_iPosition, iBufferSize)) {
    m_iPosition += iBufferSize;
  }
  return iBufferSize;
}
int32_t CFX_FileWriteStreamImp::WriteString(const FX_WCHAR* pStr,
                                            int32_t iLength) {
  return WriteData((const uint8_t*)pStr, iLength * sizeof(FX_WCHAR));
}
void CFX_FileWriteStreamImp::Flush() {
  if (m_pFileWrite) {
    m_pFileWrite->Flush();
  }
}
CFX_BufferStreamImp::CFX_BufferStreamImp()
    : m_pData(nullptr), m_iTotalSize(0), m_iPosition(0), m_iLength(0) {}

FX_BOOL CFX_BufferStreamImp::LoadBuffer(uint8_t* pData,
                                        int32_t iTotalSize,
                                        uint32_t dwAccess) {
  ASSERT(!m_pData && pData && iTotalSize > 0);
  SetAccessModes(dwAccess);
  m_pData = pData;
  m_iTotalSize = iTotalSize;
  m_iPosition = 0;
  m_iLength = (dwAccess & FX_STREAMACCESS_Write) != 0 ? 0 : iTotalSize;
  return TRUE;
}
int32_t CFX_BufferStreamImp::GetLength() const {
  ASSERT(m_pData);
  return m_iLength;
}
int32_t CFX_BufferStreamImp::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  ASSERT(m_pData);
  if (eSeek == FX_STREAMSEEK_Begin) {
    m_iPosition = iOffset;
  } else if (eSeek == FX_STREAMSEEK_Current) {
    m_iPosition += iOffset;
  } else if (eSeek == FX_STREAMSEEK_End) {
    m_iPosition = m_iLength + iOffset;
  }
  if (m_iPosition > m_iLength) {
    m_iPosition = m_iLength;
  }
  if (m_iPosition < 0) {
    m_iPosition = 0;
  }
  return m_iPosition;
}
int32_t CFX_BufferStreamImp::GetPosition() {
  ASSERT(m_pData);
  return m_iPosition;
}
FX_BOOL CFX_BufferStreamImp::IsEOF() const {
  ASSERT(m_pData);
  return m_iPosition >= m_iLength;
}
int32_t CFX_BufferStreamImp::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  ASSERT(m_pData);
  ASSERT(pBuffer && iBufferSize > 0);
  int32_t iLen = std::min(m_iLength - m_iPosition, iBufferSize);
  if (iLen <= 0) {
    return 0;
  }
  FXSYS_memcpy(pBuffer, m_pData + m_iPosition, iLen);
  m_iPosition += iLen;
  return iLen;
}
int32_t CFX_BufferStreamImp::ReadString(FX_WCHAR* pStr,
                                        int32_t iMaxLength,
                                        FX_BOOL& bEOS) {
  ASSERT(m_pData);
  ASSERT(pStr && iMaxLength > 0);
  int32_t iLen = std::min((m_iLength - m_iPosition) / 2, iMaxLength);
  if (iLen <= 0) {
    return 0;
  }
  const FX_WCHAR* pSrc = (const FX_WCHAR*)(FX_CHAR*)(m_pData + m_iPosition);
  int32_t iCount = 0;
  while (*pSrc && iCount < iLen) {
    *pStr++ = *pSrc++;
    iCount++;
  }
  m_iPosition += iCount * 2;
  bEOS = (*pSrc == L'\0') || (m_iPosition >= m_iLength);
  return iCount;
}
int32_t CFX_BufferStreamImp::WriteData(const uint8_t* pBuffer,
                                       int32_t iBufferSize) {
  ASSERT(m_pData && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  ASSERT(pBuffer && iBufferSize > 0);
  int32_t iLen = std::min(m_iTotalSize - m_iPosition, iBufferSize);
  if (iLen <= 0) {
    return 0;
  }
  FXSYS_memcpy(m_pData + m_iPosition, pBuffer, iLen);
  m_iPosition += iLen;
  if (m_iPosition > m_iLength) {
    m_iLength = m_iPosition;
  }
  return iLen;
}
int32_t CFX_BufferStreamImp::WriteString(const FX_WCHAR* pStr,
                                         int32_t iLength) {
  ASSERT(m_pData && (GetAccessModes() & FX_STREAMACCESS_Write) != 0);
  ASSERT(pStr && iLength > 0);
  int32_t iLen = std::min((m_iTotalSize - m_iPosition) / 2, iLength);
  if (iLen <= 0) {
    return 0;
  }
  FXSYS_memcpy(m_pData + m_iPosition, pStr, iLen * 2);
  m_iPosition += iLen * 2;
  if (m_iPosition > m_iLength) {
    m_iLength = m_iPosition;
  }
  return iLen;
}
IFX_Stream* IFX_Stream::CreateTextStream(IFX_Stream* pBaseStream,
                                         FX_BOOL bDeleteOnRelease) {
  ASSERT(pBaseStream);
  return new CFX_TextStream(pBaseStream, bDeleteOnRelease);
}
CFX_TextStream::CFX_TextStream(IFX_Stream* pStream, FX_BOOL bDelStream)
    : m_wCodePage(FX_CODEPAGE_DefANSI),
      m_wBOMLength(0),
      m_dwBOM(0),
      m_pBuf(nullptr),
      m_iBufSize(0),
      m_bDelStream(bDelStream),
      m_pStreamImp(pStream),
      m_iRefCount(1) {
  ASSERT(m_pStreamImp);
  m_pStreamImp->Retain();
  InitStream();
}
CFX_TextStream::~CFX_TextStream() {
  m_pStreamImp->Release();
  if (m_bDelStream) {
    m_pStreamImp->Release();
  }
  if (m_pBuf) {
    FX_Free(m_pBuf);
  }
}
void CFX_TextStream::InitStream() {
  int32_t iPosition = m_pStreamImp->GetPosition();
  m_pStreamImp->Seek(FX_STREAMSEEK_Begin, 0);
  m_pStreamImp->ReadData((uint8_t*)&m_dwBOM, 3);
#if _FX_ENDIAN_ == _FX_LITTLE_ENDIAN_
  m_dwBOM &= 0x00FFFFFF;
  if (m_dwBOM == 0x00BFBBEF) {
    m_wBOMLength = 3;
    m_wCodePage = FX_CODEPAGE_UTF8;
  } else {
    m_dwBOM &= 0x0000FFFF;
    if (m_dwBOM == 0x0000FFFE) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16BE;
    } else if (m_dwBOM == 0x0000FEFF) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16LE;
    } else {
      m_wBOMLength = 0;
      m_dwBOM = 0;
      m_wCodePage = FXSYS_GetACP();
    }
  }
#else
  m_dwBOM &= 0xFFFFFF00;
  if (m_dwBOM == 0xEFBBBF00) {
    m_wBOMLength = 3;
    m_wCodePage = FX_CODEPAGE_UTF8;
  } else {
    m_dwBOM &= 0xFFFF0000;
    if (m_dwBOM == 0xFEFF0000) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16BE;
    } else if (m_dwBOM == 0xFFFE0000) {
      m_wBOMLength = 2;
      m_wCodePage = FX_CODEPAGE_UTF16LE;
    } else {
      m_wBOMLength = 0;
      m_dwBOM = 0;
      m_wCodePage = FXSYS_GetACP();
    }
  }
#endif
  m_pStreamImp->Seek(FX_STREAMSEEK_Begin, std::max(m_wBOMLength, iPosition));
}
void CFX_TextStream::Release() {
  if (--m_iRefCount < 1) {
    delete this;
  }
}
IFX_Stream* CFX_TextStream::Retain() {
  m_iRefCount++;
  return this;
}
uint32_t CFX_TextStream::GetAccessModes() const {
  return m_pStreamImp->GetAccessModes() | FX_STREAMACCESS_Text;
}
int32_t CFX_TextStream::GetLength() const {
  return m_pStreamImp->GetLength();
}
int32_t CFX_TextStream::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  return m_pStreamImp->Seek(eSeek, iOffset);
}
int32_t CFX_TextStream::GetPosition() {
  return m_pStreamImp->GetPosition();
}
FX_BOOL CFX_TextStream::IsEOF() const {
  return m_pStreamImp->IsEOF();
}
int32_t CFX_TextStream::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  return m_pStreamImp->ReadData(pBuffer, iBufferSize);
}
int32_t CFX_TextStream::WriteData(const uint8_t* pBuffer, int32_t iBufferSize) {
  return m_pStreamImp->WriteData(pBuffer, iBufferSize);
}
void CFX_TextStream::Flush() {
  m_pStreamImp->Flush();
}
FX_BOOL CFX_TextStream::SetLength(int32_t iLength) {
  return m_pStreamImp->SetLength(iLength);
}
uint16_t CFX_TextStream::GetCodePage() const {
  return m_wCodePage;
}
IFX_Stream* CFX_TextStream::CreateSharedStream(uint32_t dwAccess,
                                               int32_t iOffset,
                                               int32_t iLength) {
  IFX_Stream* pSR =
      m_pStreamImp->CreateSharedStream(dwAccess, iOffset, iLength);
  if (!pSR) {
    return nullptr;
  }
  if (dwAccess & FX_STREAMACCESS_Text) {
    return new CFX_TextStream(pSR, TRUE);
  }
  return pSR;
}
int32_t CFX_TextStream::GetBOM(uint8_t bom[4]) const {
  if (m_wBOMLength < 1) {
    return 0;
  }
  *(uint32_t*)bom = m_dwBOM;
  return m_wBOMLength;
}
uint16_t CFX_TextStream::SetCodePage(uint16_t wCodePage) {
  if (m_wBOMLength > 0) {
    return m_wCodePage;
  }
  uint16_t v = m_wCodePage;
  m_wCodePage = wCodePage;
  return v;
}
int32_t CFX_TextStream::ReadString(FX_WCHAR* pStr,
                                   int32_t iMaxLength,
                                   FX_BOOL& bEOS,
                                   int32_t const* pByteSize) {
  ASSERT(pStr && iMaxLength > 0);
  if (!m_pStreamImp) {
    return -1;
  }
  int32_t iLen;
  if (m_wCodePage == FX_CODEPAGE_UTF16LE ||
      m_wCodePage == FX_CODEPAGE_UTF16BE) {
    int32_t iBytes = pByteSize ? *pByteSize : iMaxLength * 2;
    iLen = m_pStreamImp->ReadData((uint8_t*)pStr, iBytes);
    iMaxLength = iLen / 2;
    if (sizeof(FX_WCHAR) > 2) {
      FX_UTF16ToWChar(pStr, iMaxLength);
    }
#if _FX_ENDIAN_ == _FX_BIG_ENDIAN_
    if (m_wCodePage == FX_CODEPAGE_UTF16LE) {
      FX_SwapByteOrder(pStr, iMaxLength);
    }
#else
    if (m_wCodePage == FX_CODEPAGE_UTF16BE) {
      FX_SwapByteOrder(pStr, iMaxLength);
    }
#endif
  } else {
    int32_t pos = m_pStreamImp->GetPosition();
    int32_t iBytes = pByteSize ? *pByteSize : iMaxLength;
    iBytes = std::min(iBytes, m_pStreamImp->GetLength() - pos);
    if (iBytes > 0) {
      if (!m_pBuf) {
        m_pBuf = FX_Alloc(uint8_t, iBytes);
        m_iBufSize = iBytes;
      } else if (iBytes > m_iBufSize) {
        m_pBuf = FX_Realloc(uint8_t, m_pBuf, iBytes);
        m_iBufSize = iBytes;
      }
      iLen = m_pStreamImp->ReadData(m_pBuf, iBytes);
      int32_t iSrc = iLen;
      int32_t iDecode = FX_DecodeString(m_wCodePage, (const FX_CHAR*)m_pBuf,
                                        &iSrc, pStr, &iMaxLength, TRUE);
      m_pStreamImp->Seek(FX_STREAMSEEK_Current, iSrc - iLen);
      if (iDecode < 1) {
        return -1;
      }
    } else {
      iMaxLength = 0;
    }
  }
  bEOS = m_pStreamImp->IsEOF();
  return iMaxLength;
}
int32_t CFX_TextStream::WriteString(const FX_WCHAR* pStr, int32_t iLength) {
  ASSERT(pStr && iLength > 0);
  if ((m_pStreamImp->GetAccessModes() & FX_STREAMACCESS_Write) == 0) {
    return -1;
  }
  if (m_wCodePage == FX_CODEPAGE_UTF8) {
    int32_t len = iLength;
    CFX_UTF8Encoder encoder;
    while (len-- > 0) {
      encoder.Input(*pStr++);
    }
    CFX_ByteStringC bsResult = encoder.GetResult();
    m_pStreamImp->WriteData((const uint8_t*)bsResult.c_str(),
                            bsResult.GetLength());
  }
  return iLength;
}
CFX_Stream::CFX_Stream()
    : m_eStreamType(FX_SREAMTYPE_Unknown),
      m_pStreamImp(nullptr),
      m_dwAccess(0),
      m_iTotalSize(0),
      m_iPosition(0),
      m_iStart(0),
      m_iLength(0),
      m_iRefCount(1) {}

CFX_Stream::~CFX_Stream() {
  if (m_eStreamType != FX_STREAMTYPE_Stream)
    delete m_pStreamImp;
}

FX_BOOL CFX_Stream::LoadFile(const FX_WCHAR* pszSrcFileName,
                             uint32_t dwAccess) {
  if (m_eStreamType != FX_SREAMTYPE_Unknown || m_pStreamImp)
    return FALSE;

  if (!pszSrcFileName || FXSYS_wcslen(pszSrcFileName) < 1)
    return FALSE;

  std::unique_ptr<CFX_FileStreamImp> pImp(new CFX_FileStreamImp());
  if (!pImp->LoadFile(pszSrcFileName, dwAccess))
    return FALSE;

  m_pStreamImp = pImp.release();
  m_eStreamType = FX_STREAMTYPE_File;
  m_dwAccess = dwAccess;
  m_iLength = m_pStreamImp->GetLength();
  return TRUE;
}

FX_BOOL CFX_Stream::LoadFileRead(IFX_FileRead* pFileRead, uint32_t dwAccess) {
  if (m_eStreamType != FX_SREAMTYPE_Unknown || m_pStreamImp)
    return FALSE;

  if (!pFileRead)
    return FALSE;

  std::unique_ptr<CFX_FileReadStreamImp> pImp(new CFX_FileReadStreamImp());
  if (!pImp->LoadFileRead(pFileRead, dwAccess))
    return FALSE;

  m_pStreamImp = pImp.release();
  m_eStreamType = FX_STREAMTYPE_File;
  m_dwAccess = dwAccess;
  m_iLength = m_pStreamImp->GetLength();
  return TRUE;
}

FX_BOOL CFX_Stream::LoadFileWrite(IFX_FileWrite* pFileWrite,
                                  uint32_t dwAccess) {
  if (m_eStreamType != FX_SREAMTYPE_Unknown || m_pStreamImp)
    return FALSE;

  if (!pFileWrite)
    return FALSE;

  std::unique_ptr<CFX_FileWriteStreamImp> pImp(new CFX_FileWriteStreamImp());
  if (!pImp->LoadFileWrite(pFileWrite, dwAccess))
    return FALSE;

  m_pStreamImp = pImp.release();
  m_eStreamType = FX_STREAMTYPE_File;
  m_dwAccess = dwAccess;
  m_iLength = m_pStreamImp->GetLength();
  return TRUE;
}

FX_BOOL CFX_Stream::LoadBuffer(uint8_t* pData,
                               int32_t iTotalSize,
                               uint32_t dwAccess) {
  if (m_eStreamType != FX_SREAMTYPE_Unknown || m_pStreamImp)
    return FALSE;

  if (!pData || iTotalSize < 1)
    return FALSE;

  std::unique_ptr<CFX_BufferStreamImp> pImp(new CFX_BufferStreamImp());
  if (!pImp->LoadBuffer(pData, iTotalSize, dwAccess))
    return FALSE;

  m_pStreamImp = pImp.release();
  m_eStreamType = FX_STREAMTYPE_Buffer;
  m_dwAccess = dwAccess;
  m_iLength = m_pStreamImp->GetLength();
  return TRUE;
}

FX_BOOL CFX_Stream::LoadBufferRead(IFX_BufferRead* pBufferRead,
                                   int32_t iFileSize,
                                   uint32_t dwAccess,
                                   FX_BOOL bReleaseBufferRead) {
  if (m_eStreamType != FX_SREAMTYPE_Unknown || m_pStreamImp)
    return FALSE;

  if (!pBufferRead)
    return FALSE;

  std::unique_ptr<CFX_BufferReadStreamImp> pImp(new CFX_BufferReadStreamImp);
  if (!pImp->LoadBufferRead(pBufferRead, iFileSize, dwAccess,
                            bReleaseBufferRead))
    return FALSE;

  m_pStreamImp = pImp.release();
  m_eStreamType = FX_STREAMTYPE_BufferRead;
  m_dwAccess = dwAccess;
  m_iLength = m_pStreamImp->GetLength();
  return TRUE;
}

void CFX_Stream::Release() {
  if (--m_iRefCount < 1) {
    delete this;
  }
}
IFX_Stream* CFX_Stream::Retain() {
  m_iRefCount++;
  return this;
}

uint32_t CFX_Stream::GetAccessModes() const {
  return m_dwAccess;
}

int32_t CFX_Stream::GetLength() const {
  if (!m_pStreamImp) {
    return -1;
  }
  if (m_eStreamType == FX_STREAMTYPE_File ||
      m_eStreamType == FX_STREAMTYPE_Buffer) {
    return m_pStreamImp->GetLength();
  }
  return m_iLength;
}
int32_t CFX_Stream::Seek(FX_STREAMSEEK eSeek, int32_t iOffset) {
  if (!m_pStreamImp) {
    return -1;
  }
  if (m_eStreamType == FX_STREAMTYPE_File ||
      m_eStreamType == FX_STREAMTYPE_Buffer) {
    return m_iPosition = m_pStreamImp->Seek(eSeek, iOffset);
  }
  int32_t iEnd = m_iStart + m_iLength;
  int32_t iPosition = m_iStart + iOffset;
  if (eSeek == FX_STREAMSEEK_Begin) {
    m_iPosition = iPosition;
  } else if (eSeek == FX_STREAMSEEK_Current) {
    m_iPosition += iOffset;
  } else if (eSeek == FX_STREAMSEEK_End) {
    m_iPosition = iEnd + iOffset;
  }
  if (m_iPosition > iEnd) {
    m_iPosition = iEnd;
  }
  if (m_iPosition < m_iStart) {
    m_iPosition = m_iStart;
  }
  return m_iPosition - m_iStart;
}
int32_t CFX_Stream::GetPosition() {
  if (!m_pStreamImp) {
    return -1;
  }
  if (m_eStreamType == FX_STREAMTYPE_File ||
      m_eStreamType == FX_STREAMTYPE_Buffer) {
    return m_iPosition = m_pStreamImp->GetPosition();
  }
  return m_iPosition - m_iStart;
}
FX_BOOL CFX_Stream::IsEOF() const {
  if (!m_pStreamImp) {
    return TRUE;
  }
  if (m_eStreamType == FX_STREAMTYPE_File ||
      m_eStreamType == FX_STREAMTYPE_Buffer) {
    return m_pStreamImp->IsEOF();
  }
  return m_iPosition >= m_iStart + m_iLength;
}
int32_t CFX_Stream::ReadData(uint8_t* pBuffer, int32_t iBufferSize) {
  ASSERT(pBuffer && iBufferSize > 0);
  if (!m_pStreamImp) {
    return -1;
  }
  int32_t iLen = std::min(m_iStart + m_iLength - m_iPosition, iBufferSize);
  if (iLen <= 0) {
    return 0;
  }
  if (m_pStreamImp->GetPosition() != m_iPosition) {
    m_pStreamImp->Seek(FX_STREAMSEEK_Begin, m_iPosition);
  }
  iLen = m_pStreamImp->ReadData(pBuffer, iLen);
  m_iPosition = m_pStreamImp->GetPosition();
  return iLen;
}
int32_t CFX_Stream::ReadString(FX_WCHAR* pStr,
                               int32_t iMaxLength,
                               FX_BOOL& bEOS,
                               int32_t const* pByteSize) {
  ASSERT(pStr && iMaxLength > 0);
  if (!m_pStreamImp) {
    return -1;
  }
  int32_t iEnd = m_iStart + m_iLength;
  int32_t iLen = iEnd - m_iPosition;
  if (pByteSize) {
    iLen = std::min(iLen, *pByteSize);
  }
  iLen = std::min(iEnd / 2, iMaxLength);
  if (iLen <= 0) {
    return 0;
  }
  if (m_pStreamImp->GetPosition() != m_iPosition) {
    m_pStreamImp->Seek(FX_STREAMSEEK_Begin, m_iPosition);
  }
  iLen = m_pStreamImp->ReadString(pStr, iLen, bEOS);
  m_iPosition = m_pStreamImp->GetPosition();
  if (iLen > 0 && m_iPosition >= iEnd) {
    bEOS = TRUE;
  }
  return iLen;
}

int32_t CFX_Stream::WriteData(const uint8_t* pBuffer, int32_t iBufferSize) {
  ASSERT(pBuffer && iBufferSize > 0);
  if (!m_pStreamImp) {
    return -1;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Write) == 0) {
    return -1;
  }
  int32_t iLen = iBufferSize;
  if (m_eStreamType == FX_STREAMTYPE_Stream) {
    iLen = std::min(m_iStart + m_iTotalSize - m_iPosition, iBufferSize);
    if (iLen <= 0) {
      return 0;
    }
  }
  int32_t iEnd = m_iStart + m_iLength;
  if (m_pStreamImp->GetPosition() != m_iPosition) {
    m_pStreamImp->Seek(FX_STREAMSEEK_Begin, m_iPosition);
  }
  iLen = m_pStreamImp->WriteData(pBuffer, iLen);
  m_iPosition = m_pStreamImp->GetPosition();
  if (m_iPosition > iEnd) {
    m_iLength = m_iPosition - m_iStart;
  }
  return iLen;
}
int32_t CFX_Stream::WriteString(const FX_WCHAR* pStr, int32_t iLength) {
  ASSERT(pStr && iLength > 0);
  if (!m_pStreamImp) {
    return -1;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Write) == 0) {
    return -1;
  }
  int32_t iLen = iLength;
  if (m_eStreamType == FX_STREAMTYPE_Stream) {
    iLen = std::min((m_iStart + m_iTotalSize - m_iPosition) / 2, iLength);
    if (iLen <= 0) {
      return 0;
    }
  }
  int32_t iEnd = m_iStart + m_iLength;
  if (m_pStreamImp->GetPosition() != m_iPosition) {
    m_pStreamImp->Seek(FX_STREAMSEEK_Begin, m_iPosition);
  }
  iLen = m_pStreamImp->WriteString(pStr, iLen);
  m_iPosition = m_pStreamImp->GetPosition();
  if (m_iPosition > iEnd) {
    m_iLength = m_iPosition - m_iStart;
  }
  return iLen;
}
void CFX_Stream::Flush() {
  if (!m_pStreamImp) {
    return;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Write) == 0) {
    return;
  }
  m_pStreamImp->Flush();
}
FX_BOOL CFX_Stream::SetLength(int32_t iLength) {
  if (!m_pStreamImp) {
    return FALSE;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Write) == 0) {
    return FALSE;
  }
  return m_pStreamImp->SetLength(iLength);
}
int32_t CFX_Stream::GetBOM(uint8_t bom[4]) const {
  if (!m_pStreamImp) {
    return -1;
  }
  return 0;
}
uint16_t CFX_Stream::GetCodePage() const {
#if _FX_ENDIAN_ == _FX_LITTLE_ENDIAN_
  return FX_CODEPAGE_UTF16LE;
#else
  return FX_CODEPAGE_UTF16BE;
#endif
}
uint16_t CFX_Stream::SetCodePage(uint16_t wCodePage) {
#if _FX_ENDIAN_ == _FX_LITTLE_ENDIAN_
  return FX_CODEPAGE_UTF16LE;
#else
  return FX_CODEPAGE_UTF16BE;
#endif
}
IFX_Stream* CFX_Stream::CreateSharedStream(uint32_t dwAccess,
                                           int32_t iOffset,
                                           int32_t iLength) {
  ASSERT(iLength > 0);
  if (!m_pStreamImp) {
    return nullptr;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Text) != 0 &&
      (dwAccess & FX_STREAMACCESS_Text) == 0) {
    return nullptr;
  }
  if ((m_dwAccess & FX_STREAMACCESS_Write) == 0 &&
      (dwAccess & FX_STREAMACCESS_Write) != 0) {
    return nullptr;
  }
  int32_t iStart = m_iStart + iOffset;
  int32_t iTotal = m_iStart + m_iLength;
  if (iStart < m_iStart || iStart >= iTotal) {
    return nullptr;
  }
  int32_t iEnd = iStart + iLength;
  if (iEnd < iStart || iEnd > iTotal) {
    return nullptr;
  }
  CFX_Stream* pShared = new CFX_Stream;
  pShared->m_eStreamType = FX_STREAMTYPE_Stream;
  pShared->m_pStreamImp = m_pStreamImp;
  pShared->m_dwAccess = dwAccess;
  pShared->m_iTotalSize = iLength;
  pShared->m_iPosition = iStart;
  pShared->m_iStart = iStart;
  pShared->m_iLength = (dwAccess & FX_STREAMACCESS_Write) != 0 ? 0 : iLength;
  if (dwAccess & FX_STREAMACCESS_Text) {
    return IFX_Stream::CreateTextStream(pShared, TRUE);
  }
  return pShared;
}
IFX_FileRead* FX_CreateFileRead(IFX_Stream* pBaseStream,
                                FX_BOOL bReleaseStream) {
  ASSERT(pBaseStream);
  return new CFGAS_FileRead(pBaseStream, bReleaseStream);
}
CFGAS_FileRead::CFGAS_FileRead(IFX_Stream* pStream, FX_BOOL bReleaseStream)
    : m_bReleaseStream(bReleaseStream), m_pStream(pStream) {
  ASSERT(m_pStream);
}
CFGAS_FileRead::~CFGAS_FileRead() {
  if (m_bReleaseStream) {
    m_pStream->Release();
  }
}
FX_FILESIZE CFGAS_FileRead::GetSize() {
  return (FX_FILESIZE)m_pStream->GetLength();
}
FX_BOOL CFGAS_FileRead::ReadBlock(void* buffer,
                                  FX_FILESIZE offset,
                                  size_t size) {
  m_pStream->Seek(FX_STREAMSEEK_Begin, (int32_t)offset);
  int32_t iLen = m_pStream->ReadData((uint8_t*)buffer, (int32_t)size);
  return iLen == (int32_t)size;
}

void CFGAS_FileRead::Release() {
  delete this;
}
