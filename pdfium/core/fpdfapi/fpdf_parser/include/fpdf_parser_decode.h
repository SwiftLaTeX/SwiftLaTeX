// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_INCLUDE_FPDF_PARSER_DECODE_H_
#define CORE_FPDFAPI_FPDF_PARSER_INCLUDE_FPDF_PARSER_DECODE_H_

#include "core/fxcrt/include/fx_basic.h"

class CPDF_Dictionary;

// Indexed by 8-bit char code, contains unicode code points.
extern const uint16_t PDFDocEncoding[256];

CFX_ByteString PDF_NameDecode(const CFX_ByteStringC& orig);
CFX_ByteString PDF_NameDecode(const CFX_ByteString& orig);
CFX_ByteString PDF_NameEncode(const CFX_ByteString& orig);
CFX_ByteString PDF_EncodeString(const CFX_ByteString& src,
                                FX_BOOL bHex = FALSE);
CFX_WideString PDF_DecodeText(const uint8_t* pData, uint32_t size);
CFX_WideString PDF_DecodeText(const CFX_ByteString& bstr);
CFX_ByteString PDF_EncodeText(const FX_WCHAR* pString, int len = -1);
CFX_ByteString PDF_EncodeText(const CFX_WideString& str);

bool FlateEncode(const uint8_t* src_buf,
                 uint32_t src_size,
                 uint8_t** dest_buf,
                 uint32_t* dest_size);

// This used to have more parameters like the predictor and bpc, but there was
// only one caller, so the interface has been simplified, the values are hard
// coded, and dead code has been removed.
bool PngEncode(const uint8_t* src_buf,
               uint32_t src_size,
               uint8_t** dest_buf,
               uint32_t* dest_size);

uint32_t FlateDecode(const uint8_t* src_buf,
                     uint32_t src_size,
                     uint8_t*& dest_buf,
                     uint32_t& dest_size);
uint32_t RunLengthDecode(const uint8_t* src_buf,
                         uint32_t src_size,
                         uint8_t*& dest_buf,
                         uint32_t& dest_size);

// Public for testing.
uint32_t A85Decode(const uint8_t* src_buf,
                   uint32_t src_size,
                   uint8_t*& dest_buf,
                   uint32_t& dest_size);
// Public for testing.
uint32_t HexDecode(const uint8_t* src_buf,
                   uint32_t src_size,
                   uint8_t*& dest_buf,
                   uint32_t& dest_size);
// Public for testing.
uint32_t FPDFAPI_FlateOrLZWDecode(FX_BOOL bLZW,
                                  const uint8_t* src_buf,
                                  uint32_t src_size,
                                  CPDF_Dictionary* pParams,
                                  uint32_t estimated_size,
                                  uint8_t*& dest_buf,
                                  uint32_t& dest_size);
FX_BOOL PDF_DataDecode(const uint8_t* src_buf,
                       uint32_t src_size,
                       const CPDF_Dictionary* pDict,
                       uint8_t*& dest_buf,
                       uint32_t& dest_size,
                       CFX_ByteString& ImageEncoding,
                       CPDF_Dictionary*& pImageParms,
                       uint32_t estimated_size,
                       FX_BOOL bImageAcc);

#endif  // CORE_FPDFAPI_FPDF_PARSER_INCLUDE_FPDF_PARSER_DECODE_H_
