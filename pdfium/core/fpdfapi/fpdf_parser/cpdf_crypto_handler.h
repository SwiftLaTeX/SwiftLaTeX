// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_CPDF_CRYPTO_HANDLER_H_
#define CORE_FPDFAPI_FPDF_PARSER_CPDF_CRYPTO_HANDLER_H_

#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_string.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Dictionary;
class CPDF_SecurityHandler;

class CPDF_CryptoHandler {
 public:
  CPDF_CryptoHandler();
  ~CPDF_CryptoHandler();

  FX_BOOL Init(CPDF_Dictionary* pEncryptDict,
               CPDF_SecurityHandler* pSecurityHandler);
  uint32_t DecryptGetSize(uint32_t src_size);
  void* DecryptStart(uint32_t objnum, uint32_t gennum);
  void Decrypt(uint32_t objnum, uint32_t gennum, CFX_ByteString& str);
  FX_BOOL DecryptStream(void* context,
                        const uint8_t* src_buf,
                        uint32_t src_size,
                        CFX_BinaryBuf& dest_buf);
  FX_BOOL DecryptFinish(void* context, CFX_BinaryBuf& dest_buf);
  uint32_t EncryptGetSize(uint32_t objnum,
                          uint32_t version,
                          const uint8_t* src_buf,
                          uint32_t src_size);
  FX_BOOL EncryptContent(uint32_t objnum,
                         uint32_t version,
                         const uint8_t* src_buf,
                         uint32_t src_size,
                         uint8_t* dest_buf,
                         uint32_t& dest_size);

  FX_BOOL Init(int cipher, const uint8_t* key, int keylen);

 protected:
  void CryptBlock(FX_BOOL bEncrypt,
                  uint32_t objnum,
                  uint32_t gennum,
                  const uint8_t* src_buf,
                  uint32_t src_size,
                  uint8_t* dest_buf,
                  uint32_t& dest_size);
  void* CryptStart(uint32_t objnum, uint32_t gennum, FX_BOOL bEncrypt);
  FX_BOOL CryptStream(void* context,
                      const uint8_t* src_buf,
                      uint32_t src_size,
                      CFX_BinaryBuf& dest_buf,
                      FX_BOOL bEncrypt);
  FX_BOOL CryptFinish(void* context, CFX_BinaryBuf& dest_buf, FX_BOOL bEncrypt);

  uint8_t m_EncryptKey[32];
  int m_KeyLen;
  int m_Cipher;
  uint8_t* m_pAESContext;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_CPDF_CRYPTO_HANDLER_H_
