// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_INCLUDE_FX_STREAM_H_
#define CORE_FXCRT_INCLUDE_FX_STREAM_H_

#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

void* FX_OpenFolder(const FX_CHAR* path);
void* FX_OpenFolder(const FX_WCHAR* path);
FX_BOOL FX_GetNextFile(void* handle,
                       CFX_ByteString& filename,
                       FX_BOOL& bFolder);
FX_BOOL FX_GetNextFile(void* handle,
                       CFX_WideString& filename,
                       FX_BOOL& bFolder);
void FX_CloseFolder(void* handle);
FX_WCHAR FX_GetFolderSeparator();

#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
#define FX_FILESIZE int32_t
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif  // O_BINARY

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif  // O_LARGEFILE

#define FX_FILESIZE off_t
#endif  // _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_

#define FX_GETBYTEOFFSET32(a) 0
#define FX_GETBYTEOFFSET40(a) 0
#define FX_GETBYTEOFFSET48(a) 0
#define FX_GETBYTEOFFSET56(a) 0
#define FX_GETBYTEOFFSET24(a) ((uint8_t)(a >> 24))
#define FX_GETBYTEOFFSET16(a) ((uint8_t)(a >> 16))
#define FX_GETBYTEOFFSET8(a) ((uint8_t)(a >> 8))
#define FX_GETBYTEOFFSET0(a) ((uint8_t)(a))
#define FX_FILEMODE_Write 0
#define FX_FILEMODE_ReadOnly 1
#define FX_FILEMODE_Truncate 2

class IFX_StreamWrite {
 public:
  virtual ~IFX_StreamWrite() {}
  virtual void Release() = 0;
  virtual FX_BOOL WriteBlock(const void* pData, size_t size) = 0;
};

class IFX_FileWrite : public IFX_StreamWrite {
 public:
  // IFX_StreamWrite:
  FX_BOOL WriteBlock(const void* pData, size_t size) override;
  virtual FX_FILESIZE GetSize() = 0;
  virtual FX_BOOL Flush() = 0;
  virtual FX_BOOL WriteBlock(const void* pData,
                             FX_FILESIZE offset,
                             size_t size) = 0;
};

class IFX_StreamRead {
 public:
  virtual ~IFX_StreamRead() {}

  virtual void Release() = 0;
  virtual FX_BOOL IsEOF() = 0;
  virtual FX_FILESIZE GetPosition() = 0;
  virtual size_t ReadBlock(void* buffer, size_t size) = 0;
};

class IFX_FileRead : public IFX_StreamRead {
 public:
  // IFX_StreamRead:
  void Release() override = 0;
  FX_BOOL IsEOF() override;
  FX_FILESIZE GetPosition() override;
  size_t ReadBlock(void* buffer, size_t size) override;

  virtual FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) = 0;
  virtual FX_FILESIZE GetSize() = 0;
};

IFX_FileRead* FX_CreateFileRead(const FX_CHAR* filename);
IFX_FileRead* FX_CreateFileRead(const FX_WCHAR* filename);

class IFX_FileStream : public IFX_FileRead, public IFX_FileWrite {
 public:
  virtual IFX_FileStream* Retain() = 0;

  // IFX_FileRead:
  void Release() override = 0;
  FX_BOOL IsEOF() override = 0;
  FX_FILESIZE GetPosition() override = 0;
  size_t ReadBlock(void* buffer, size_t size) override = 0;
  FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) override = 0;
  FX_FILESIZE GetSize() override = 0;

  // IFX_FileWrite:
  FX_BOOL WriteBlock(const void* buffer,
                     FX_FILESIZE offset,
                     size_t size) override = 0;
  FX_BOOL WriteBlock(const void* buffer, size_t size) override;
  FX_BOOL Flush() override = 0;
};

IFX_FileStream* FX_CreateFileStream(const FX_CHAR* filename, uint32_t dwModes);
IFX_FileStream* FX_CreateFileStream(const FX_WCHAR* filename, uint32_t dwModes);

#ifdef PDF_ENABLE_XFA
class IFX_FileAccess {
 public:
  virtual ~IFX_FileAccess() {}
  virtual void Release() = 0;
  virtual IFX_FileAccess* Retain() = 0;
  virtual void GetPath(CFX_WideString& wsPath) = 0;
  virtual IFX_FileStream* CreateFileStream(uint32_t dwModes) = 0;
};
IFX_FileAccess* FX_CreateDefaultFileAccess(const CFX_WideStringC& wsPath);
#endif  // PDF_ENABLE_XFA

class IFX_MemoryStream : public IFX_FileStream {
 public:
  virtual FX_BOOL IsConsecutive() const = 0;
  virtual void EstimateSize(size_t nInitSize, size_t nGrowSize) = 0;
  virtual uint8_t* GetBuffer() const = 0;
  virtual void AttachBuffer(uint8_t* pBuffer,
                            size_t nSize,
                            FX_BOOL bTakeOver = FALSE) = 0;
  virtual void DetachBuffer() = 0;
};

IFX_MemoryStream* FX_CreateMemoryStream(uint8_t* pBuffer,
                                        size_t nSize,
                                        FX_BOOL bTakeOver = FALSE);
IFX_MemoryStream* FX_CreateMemoryStream(FX_BOOL bConsecutive = FALSE);

class IFX_BufferRead : public IFX_StreamRead {
 public:
  // IFX_StreamRead:
  void Release() override = 0;
  FX_BOOL IsEOF() override = 0;
  FX_FILESIZE GetPosition() override = 0;
  size_t ReadBlock(void* buffer, size_t size) override = 0;

  virtual FX_BOOL ReadNextBlock(FX_BOOL bRestart = FALSE) = 0;
  virtual const uint8_t* GetBlockBuffer() = 0;
  virtual size_t GetBlockSize() = 0;
  virtual FX_FILESIZE GetBlockOffset() = 0;
};

#endif  // CORE_FXCRT_INCLUDE_FX_STREAM_H_
