// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STREAM_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STREAM_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_object.h"
#include "core/fxcrt/include/fx_stream.h"

class CPDF_Stream : public CPDF_Object {
 public:
  CPDF_Stream(uint8_t* pData, uint32_t size, CPDF_Dictionary* pDict);

  // CPDF_Object.
  Type GetType() const override;
  CPDF_Object* Clone(FX_BOOL bDirect = FALSE) const override;
  CPDF_Dictionary* GetDict() const override;
  CFX_WideString GetUnicodeText() const override;
  bool IsStream() const override;
  CPDF_Stream* AsStream() override;
  const CPDF_Stream* AsStream() const override;

  uint32_t GetRawSize() const { return m_dwSize; }
  uint8_t* GetRawData() const { return m_pDataBuf; }

  void SetData(const uint8_t* pData,
               uint32_t size,
               FX_BOOL bCompressed,
               FX_BOOL bKeepBuf);

  void InitStream(const uint8_t* pData, uint32_t size, CPDF_Dictionary* pDict);
  void InitStreamFromFile(IFX_FileRead* pFile, CPDF_Dictionary* pDict);

  FX_BOOL ReadRawData(FX_FILESIZE start_pos,
                      uint8_t* pBuf,
                      uint32_t buf_size) const;

  bool IsMemoryBased() const { return m_GenNum == kMemoryBasedGenNum; }

 protected:
  static const uint32_t kMemoryBasedGenNum = (uint32_t)-1;

  ~CPDF_Stream() override;

  void InitStreamInternal(CPDF_Dictionary* pDict);

  CPDF_Dictionary* m_pDict;
  uint32_t m_dwSize;
  uint32_t m_GenNum;

  union {
    uint8_t* m_pDataBuf;
    IFX_FileRead* m_pFile;
  };
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_CPDF_STREAM_H_
