// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"

#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"

CPDF_Stream::CPDF_Stream(uint8_t* pData, uint32_t size, CPDF_Dictionary* pDict)
    : m_pDict(pDict),
      m_dwSize(size),
      m_GenNum(kMemoryBasedGenNum),
      m_pDataBuf(pData) {}

CPDF_Stream::~CPDF_Stream() {
  if (IsMemoryBased())
    FX_Free(m_pDataBuf);

  if (m_pDict)
    m_pDict->Release();
}

CPDF_Object::Type CPDF_Stream::GetType() const {
  return STREAM;
}

CPDF_Dictionary* CPDF_Stream::GetDict() const {
  return m_pDict;
}

bool CPDF_Stream::IsStream() const {
  return true;
}

CPDF_Stream* CPDF_Stream::AsStream() {
  return this;
}

const CPDF_Stream* CPDF_Stream::AsStream() const {
  return this;
}

void CPDF_Stream::InitStreamInternal(CPDF_Dictionary* pDict) {
  if (pDict) {
    if (m_pDict)
      m_pDict->Release();
    m_pDict = pDict;
  }
  if (IsMemoryBased())
    FX_Free(m_pDataBuf);

  m_GenNum = 0;
  m_pFile = nullptr;
}

void CPDF_Stream::InitStream(const uint8_t* pData,
                             uint32_t size,
                             CPDF_Dictionary* pDict) {
  InitStreamInternal(pDict);
  m_GenNum = kMemoryBasedGenNum;
  m_pDataBuf = FX_Alloc(uint8_t, size);
  if (pData)
    FXSYS_memcpy(m_pDataBuf, pData, size);

  m_dwSize = size;
  if (m_pDict)
    m_pDict->SetAtInteger("Length", size);
}

CPDF_Object* CPDF_Stream::Clone(FX_BOOL bDirect) const {
  CPDF_StreamAcc acc;
  acc.LoadAllData(this, TRUE);
  uint32_t streamSize = acc.GetSize();
  CPDF_Dictionary* pDict = GetDict();
  if (pDict)
    pDict = ToDictionary(pDict->Clone(bDirect));

  return new CPDF_Stream(acc.DetachData(), streamSize, pDict);
}

void CPDF_Stream::SetData(const uint8_t* pData,
                          uint32_t size,
                          FX_BOOL bCompressed,
                          FX_BOOL bKeepBuf) {
  if (IsMemoryBased())
    FX_Free(m_pDataBuf);
  m_GenNum = kMemoryBasedGenNum;

  if (bKeepBuf) {
    m_pDataBuf = const_cast<uint8_t*>(pData);
  } else {
    m_pDataBuf = FX_Alloc(uint8_t, size);
    if (pData) {
      FXSYS_memcpy(m_pDataBuf, pData, size);
    }
  }
  m_dwSize = size;
  if (!m_pDict)
    m_pDict = new CPDF_Dictionary;
  m_pDict->SetAtInteger("Length", size);
  if (!bCompressed) {
    m_pDict->RemoveAt("Filter");
    m_pDict->RemoveAt("DecodeParms");
  }
}

FX_BOOL CPDF_Stream::ReadRawData(FX_FILESIZE offset,
                                 uint8_t* buf,
                                 uint32_t size) const {
  if (!IsMemoryBased() && m_pFile)
    return m_pFile->ReadBlock(buf, offset, size);

  if (m_pDataBuf)
    FXSYS_memcpy(buf, m_pDataBuf + offset, size);

  return TRUE;
}

void CPDF_Stream::InitStreamFromFile(IFX_FileRead* pFile,
                                     CPDF_Dictionary* pDict) {
  InitStreamInternal(pDict);
  m_pFile = pFile;
  m_dwSize = (uint32_t)pFile->GetSize();
  if (m_pDict)
    m_pDict->SetAtInteger("Length", m_dwSize);
}

CFX_WideString CPDF_Stream::GetUnicodeText() const {
  CPDF_StreamAcc stream;
  stream.LoadAllData(this, FALSE);
  return PDF_DecodeText(stream.GetData(), stream.GetSize());
}

