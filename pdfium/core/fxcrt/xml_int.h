// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_XML_INT_H_
#define CORE_FXCRT_XML_INT_H_

#include <algorithm>

#include "core/fxcrt/include/fx_stream.h"

class CFX_UTF8Decoder;
class CXML_Element;

class CXML_DataBufAcc : public IFX_BufferRead {
 public:
  CXML_DataBufAcc(const uint8_t* pBuffer, size_t size);
  ~CXML_DataBufAcc() override;

  // IFX_BufferRead
  void Release() override;
  FX_BOOL IsEOF() override;
  FX_FILESIZE GetPosition() override;
  size_t ReadBlock(void* buffer, size_t size) override;
  FX_BOOL ReadNextBlock(FX_BOOL bRestart = FALSE) override;
  const uint8_t* GetBlockBuffer() override;
  size_t GetBlockSize() override;
  FX_FILESIZE GetBlockOffset() override;

 protected:
  const uint8_t* m_pBuffer;
  size_t m_dwSize;
  size_t m_dwCurPos;
};

class CXML_DataStmAcc : public IFX_BufferRead {
 public:
  explicit CXML_DataStmAcc(IFX_FileRead* pFileRead);
  ~CXML_DataStmAcc() override;

  // IFX_BufferRead
  void Release() override;
  FX_BOOL IsEOF() override;
  FX_FILESIZE GetPosition() override;
  size_t ReadBlock(void* buffer, size_t size) override;
  FX_BOOL ReadNextBlock(FX_BOOL bRestart = FALSE) override;
  const uint8_t* GetBlockBuffer() override;
  size_t GetBlockSize() override;
  FX_FILESIZE GetBlockOffset() override;

 protected:
  IFX_FileRead* m_pFileRead;
  uint8_t* m_pBuffer;
  FX_FILESIZE m_nStart;
  size_t m_dwSize;
};

class CXML_Parser {
 public:
  CXML_Parser();
  ~CXML_Parser();

  FX_BOOL Init(uint8_t* pBuffer, size_t size);
  FX_BOOL Init(IFX_FileRead* pFileRead);
  FX_BOOL Init(IFX_BufferRead* pBuffer);
  FX_BOOL Init(FX_BOOL bOwndedStream);
  FX_BOOL ReadNextBlock();
  FX_BOOL IsEOF();
  FX_BOOL HaveAvailData();
  void SkipWhiteSpaces();
  void GetName(CFX_ByteString& space, CFX_ByteString& name);
  void GetAttrValue(CFX_WideString& value);
  uint32_t GetCharRef();
  void GetTagName(CFX_ByteString& space,
                  CFX_ByteString& name,
                  FX_BOOL& bEndTag,
                  FX_BOOL bStartTag = FALSE);
  void SkipLiterals(const CFX_ByteStringC& str);
  CXML_Element* ParseElement(CXML_Element* pParent, FX_BOOL bStartTag = FALSE);
  void InsertContentSegment(FX_BOOL bCDATA,
                            const CFX_WideStringC& content,
                            CXML_Element* pElement);
  void InsertCDATASegment(CFX_UTF8Decoder& decoder, CXML_Element* pElement);

  IFX_BufferRead* m_pDataAcc;
  FX_BOOL m_bOwnedStream;
  FX_FILESIZE m_nOffset;
  FX_BOOL m_bSaveSpaceChars;
  const uint8_t* m_pBuffer;
  size_t m_dwBufferSize;
  FX_FILESIZE m_nBufferOffset;
  size_t m_dwIndex;
};

void FX_XML_SplitQualifiedName(const CFX_ByteStringC& bsFullName,
                               CFX_ByteStringC& bsSpace,
                               CFX_ByteStringC& bsName);

#endif  // CORE_FXCRT_XML_INT_H_
