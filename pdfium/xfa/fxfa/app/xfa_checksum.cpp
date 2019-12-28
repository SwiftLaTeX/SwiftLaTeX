// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/include/xfa_checksum.h"

#include "core/fdrm/crypto/include/fx_crypt.h"

namespace {

struct FX_BASE64DATA {
  uint32_t data1 : 2;
  uint32_t data2 : 6;
  uint32_t data3 : 4;
  uint32_t data4 : 4;
  uint32_t data5 : 6;
  uint32_t data6 : 2;
  uint32_t data7 : 8;
};

const FX_CHAR g_FXBase64EncoderMap[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

void Base64EncodePiece(const FX_BASE64DATA& src,
                       int32_t iBytes,
                       FX_CHAR dst[4]) {
  dst[0] = g_FXBase64EncoderMap[src.data2];
  uint32_t b = src.data1 << 4;
  if (iBytes > 1) {
    b |= src.data4;
  }
  dst[1] = g_FXBase64EncoderMap[b];
  if (iBytes > 1) {
    b = src.data3 << 2;
    if (iBytes > 2) {
      b |= src.data6;
    }
    dst[2] = g_FXBase64EncoderMap[b];
    if (iBytes > 2) {
      dst[3] = g_FXBase64EncoderMap[src.data5];
    } else {
      dst[3] = '=';
    }
  } else {
    dst[2] = dst[3] = '=';
  }
}

int32_t Base64EncodeA(const uint8_t* pSrc, int32_t iSrcLen, FX_CHAR* pDst) {
  ASSERT(pSrc);
  if (iSrcLen < 1) {
    return 0;
  }
  if (!pDst) {
    int32_t iDstLen = iSrcLen / 3 * 4;
    if ((iSrcLen % 3) != 0) {
      iDstLen += 4;
    }
    return iDstLen;
  }
  FX_BASE64DATA srcData;
  int32_t iBytes = 3;
  FX_CHAR* pDstEnd = pDst;
  while (iSrcLen > 0) {
    if (iSrcLen > 2) {
      ((uint8_t*)&srcData)[0] = *pSrc++;
      ((uint8_t*)&srcData)[1] = *pSrc++;
      ((uint8_t*)&srcData)[2] = *pSrc++;
      iSrcLen -= 3;
    } else {
      *((uint32_t*)&srcData) = 0;
      ((uint8_t*)&srcData)[0] = *pSrc++;
      if (iSrcLen > 1) {
        ((uint8_t*)&srcData)[1] = *pSrc++;
      }
      iBytes = iSrcLen;
      iSrcLen = 0;
    }
    Base64EncodePiece(srcData, iBytes, pDstEnd);
    pDstEnd += 4;
  }
  return pDstEnd - pDst;
}

}  // namespace

CXFA_SAXReaderHandler::CXFA_SAXReaderHandler(CXFA_ChecksumContext* pContext)
    : m_pContext(pContext) {
  ASSERT(m_pContext);
}
CXFA_SAXReaderHandler::~CXFA_SAXReaderHandler() {}
CXFA_SAXContext* CXFA_SAXReaderHandler::OnTagEnter(
    const CFX_ByteStringC& bsTagName,
    CFX_SAXItem::Type eType,
    uint32_t dwStartPos) {
  UpdateChecksum(TRUE);
  if (eType != CFX_SAXItem::Type::Tag &&
      eType != CFX_SAXItem::Type::Instruction) {
    return nullptr;
  }
  m_SAXContext.m_eNode = eType;
  CFX_ByteTextBuf& textBuf = m_SAXContext.m_TextBuf;
  textBuf << "<";
  if (eType == CFX_SAXItem::Type::Instruction) {
    textBuf << "?";
  }
  textBuf << bsTagName;
  m_SAXContext.m_bsTagName = bsTagName;
  return &m_SAXContext;
}

void CXFA_SAXReaderHandler::OnTagAttribute(CXFA_SAXContext* pTag,
                                           const CFX_ByteStringC& bsAttri,
                                           const CFX_ByteStringC& bsValue) {
  if (!pTag)
    return;

  pTag->m_TextBuf << " " << bsAttri << "=\"" << bsValue << "\"";
}

void CXFA_SAXReaderHandler::OnTagBreak(CXFA_SAXContext* pTag) {
  if (!pTag)
    return;

  pTag->m_TextBuf << ">";
  UpdateChecksum(FALSE);
}

void CXFA_SAXReaderHandler::OnTagData(CXFA_SAXContext* pTag,
                                      CFX_SAXItem::Type eType,
                                      const CFX_ByteStringC& bsData,
                                      uint32_t dwStartPos) {
  if (!pTag)
    return;

  CFX_ByteTextBuf& textBuf = pTag->m_TextBuf;
  if (eType == CFX_SAXItem::Type::CharData)
    textBuf << "<![CDATA[";

  textBuf << bsData;
  if (eType == CFX_SAXItem::Type::CharData)
    textBuf << "]]>";
}

void CXFA_SAXReaderHandler::OnTagClose(CXFA_SAXContext* pTag,
                                       uint32_t dwEndPos) {
  if (!pTag)
    return;

  CFX_ByteTextBuf& textBuf = pTag->m_TextBuf;
  if (pTag->m_eNode == CFX_SAXItem::Type::Instruction)
    textBuf << "?>";
  else if (pTag->m_eNode == CFX_SAXItem::Type::Tag)
    textBuf << "></" << pTag->m_bsTagName.AsStringC() << ">";

  UpdateChecksum(FALSE);
}

void CXFA_SAXReaderHandler::OnTagEnd(CXFA_SAXContext* pTag,
                                     const CFX_ByteStringC& bsTagName,
                                     uint32_t dwEndPos) {
  if (!pTag)
    return;

  pTag->m_TextBuf << "</" << bsTagName << ">";
  UpdateChecksum(FALSE);
}

void CXFA_SAXReaderHandler::OnTargetData(CXFA_SAXContext* pTag,
                                         CFX_SAXItem::Type eType,
                                         const CFX_ByteStringC& bsData,
                                         uint32_t dwStartPos) {
  if (!pTag && eType != CFX_SAXItem::Type::Comment)
    return;

  if (eType == CFX_SAXItem::Type::Comment) {
    m_SAXContext.m_TextBuf << "<!--" << bsData << "-->";
    UpdateChecksum(FALSE);
  } else {
    pTag->m_TextBuf << " " << bsData;
  }
}

void CXFA_SAXReaderHandler::UpdateChecksum(FX_BOOL bCheckSpace) {
  int32_t iLength = m_SAXContext.m_TextBuf.GetLength();
  if (iLength < 1) {
    return;
  }
  uint8_t* pBuffer = m_SAXContext.m_TextBuf.GetBuffer();
  FX_BOOL bUpdata = TRUE;
  if (bCheckSpace) {
    bUpdata = FALSE;
    for (int32_t i = 0; i < iLength; i++) {
      bUpdata = (pBuffer[i] > 0x20);
      if (bUpdata) {
        break;
      }
    }
  }
  if (bUpdata) {
    m_pContext->Update(CFX_ByteStringC(pBuffer, iLength));
  }
  m_SAXContext.m_TextBuf.Clear();
}

CXFA_ChecksumContext::CXFA_ChecksumContext()
    : m_pSAXReader(nullptr), m_pByteContext(nullptr) {}

CXFA_ChecksumContext::~CXFA_ChecksumContext() {
  FinishChecksum();
}

void CXFA_ChecksumContext::StartChecksum() {
  FinishChecksum();
  m_pByteContext = FX_Alloc(uint8_t, 128);
  CRYPT_SHA1Start(m_pByteContext);
  m_bsChecksum.clear();
  m_pSAXReader = new CFX_SAXReader;
}

FX_BOOL CXFA_ChecksumContext::UpdateChecksum(IFX_FileRead* pSrcFile,
                                             FX_FILESIZE offset,
                                             size_t size) {
  if (!m_pSAXReader || !pSrcFile)
    return FALSE;
  if (size < 1)
    size = pSrcFile->GetSize();

  CXFA_SAXReaderHandler handler(this);
  m_pSAXReader->SetHandler(&handler);
  if (m_pSAXReader->StartParse(
          pSrcFile, (uint32_t)offset, (uint32_t)size,
          CFX_SaxParseMode_NotSkipSpace | CFX_SaxParseMode_NotConvert_amp |
              CFX_SaxParseMode_NotConvert_lt | CFX_SaxParseMode_NotConvert_gt |
              CFX_SaxParseMode_NotConvert_sharp) < 0) {
    return FALSE;
  }
  return m_pSAXReader->ContinueParse(nullptr) > 99;
}

void CXFA_ChecksumContext::FinishChecksum() {
  delete m_pSAXReader;
  m_pSAXReader = nullptr;
  if (m_pByteContext) {
    uint8_t digest[20];
    FXSYS_memset(digest, 0, 20);
    CRYPT_SHA1Finish(m_pByteContext, digest);
    int32_t nLen = Base64EncodeA(digest, 20, nullptr);
    FX_CHAR* pBuffer = m_bsChecksum.GetBuffer(nLen);
    Base64EncodeA(digest, 20, pBuffer);
    m_bsChecksum.ReleaseBuffer(nLen);
    FX_Free(m_pByteContext);
    m_pByteContext = nullptr;
  }
}

CFX_ByteString CXFA_ChecksumContext::GetChecksum() const {
  return m_bsChecksum;
}

void CXFA_ChecksumContext::Update(const CFX_ByteStringC& bsText) {
  if (m_pByteContext) {
    CRYPT_SHA1Update(m_pByteContext, bsText.raw_str(), bsText.GetLength());
  }
}
