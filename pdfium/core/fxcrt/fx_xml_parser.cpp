// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml_int.h"

#include <vector>

#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_xml.h"
#include "third_party/base/stl_util.h"

CXML_DataBufAcc::CXML_DataBufAcc(const uint8_t* pBuffer, size_t size)
    : m_pBuffer(pBuffer), m_dwSize(size), m_dwCurPos(0) {}

CXML_DataBufAcc::~CXML_DataBufAcc() {}

void CXML_DataBufAcc::Release() {
  delete this;
}

FX_BOOL CXML_DataBufAcc::IsEOF() {
  return m_dwCurPos >= m_dwSize;
}

FX_FILESIZE CXML_DataBufAcc::GetPosition() {
  return (FX_FILESIZE)m_dwCurPos;
}

size_t CXML_DataBufAcc::ReadBlock(void* buffer, size_t size) {
  return 0;
}

FX_BOOL CXML_DataBufAcc::ReadNextBlock(FX_BOOL bRestart) {
  if (bRestart) {
    m_dwCurPos = 0;
  }
  if (m_dwCurPos < m_dwSize) {
    m_dwCurPos = m_dwSize;
    return TRUE;
  }
  return FALSE;
}

const uint8_t* CXML_DataBufAcc::GetBlockBuffer() {
  return m_pBuffer;
}

size_t CXML_DataBufAcc::GetBlockSize() {
  return m_dwSize;
}

FX_FILESIZE CXML_DataBufAcc::GetBlockOffset() {
  return 0;
}

CXML_DataStmAcc::CXML_DataStmAcc(IFX_FileRead* pFileRead)
    : m_pFileRead(pFileRead), m_pBuffer(nullptr), m_nStart(0), m_dwSize(0) {
  ASSERT(m_pFileRead);
}

CXML_DataStmAcc::~CXML_DataStmAcc() {
  FX_Free(m_pBuffer);
}

void CXML_DataStmAcc::Release() {
  delete this;
}

FX_BOOL CXML_DataStmAcc::IsEOF() {
  return m_nStart + (FX_FILESIZE)m_dwSize >= m_pFileRead->GetSize();
}

FX_FILESIZE CXML_DataStmAcc::GetPosition() {
  return m_nStart + (FX_FILESIZE)m_dwSize;
}

size_t CXML_DataStmAcc::ReadBlock(void* buffer, size_t size) {
  return 0;
}

FX_BOOL CXML_DataStmAcc::ReadNextBlock(FX_BOOL bRestart) {
  if (bRestart) {
    m_nStart = 0;
  }
  FX_FILESIZE nLength = m_pFileRead->GetSize();
  m_nStart += (FX_FILESIZE)m_dwSize;
  if (m_nStart >= nLength) {
    return FALSE;
  }
  static const FX_FILESIZE FX_XMLDATASTREAM_BufferSize = 32 * 1024;
  m_dwSize = static_cast<size_t>(
      std::min(FX_XMLDATASTREAM_BufferSize, nLength - m_nStart));
  if (!m_pBuffer) {
    m_pBuffer = FX_Alloc(uint8_t, m_dwSize);
  }
  return m_pFileRead->ReadBlock(m_pBuffer, m_nStart, m_dwSize);
}

const uint8_t* CXML_DataStmAcc::GetBlockBuffer() {
  return (const uint8_t*)m_pBuffer;
}

size_t CXML_DataStmAcc::GetBlockSize() {
  return m_dwSize;
}

FX_FILESIZE CXML_DataStmAcc::GetBlockOffset() {
  return m_nStart;
}

CXML_Parser::CXML_Parser()
    : m_pDataAcc(nullptr),
      m_bOwnedStream(FALSE),
      m_nOffset(0),
      m_bSaveSpaceChars(FALSE),
      m_pBuffer(nullptr),
      m_dwBufferSize(0),
      m_nBufferOffset(0),
      m_dwIndex(0) {}

CXML_Parser::~CXML_Parser() {
  if (m_bOwnedStream) {
    m_pDataAcc->Release();
  }
}

FX_BOOL CXML_Parser::Init(uint8_t* pBuffer, size_t size) {
  m_pDataAcc = new CXML_DataBufAcc(pBuffer, size);
  return Init(TRUE);
}
FX_BOOL CXML_Parser::Init(IFX_FileRead* pFileRead) {
  m_pDataAcc = new CXML_DataStmAcc(pFileRead);
  return Init(TRUE);
}
FX_BOOL CXML_Parser::Init(IFX_BufferRead* pBuffer) {
  if (!pBuffer) {
    return FALSE;
  }
  m_pDataAcc = pBuffer;
  return Init(FALSE);
}
FX_BOOL CXML_Parser::Init(FX_BOOL bOwndedStream) {
  m_bOwnedStream = bOwndedStream;
  m_nOffset = 0;
  return ReadNextBlock();
}
FX_BOOL CXML_Parser::ReadNextBlock() {
  if (!m_pDataAcc->ReadNextBlock()) {
    return FALSE;
  }
  m_pBuffer = m_pDataAcc->GetBlockBuffer();
  m_dwBufferSize = m_pDataAcc->GetBlockSize();
  m_nBufferOffset = m_pDataAcc->GetBlockOffset();
  m_dwIndex = 0;
  return m_dwBufferSize > 0;
}
FX_BOOL CXML_Parser::IsEOF() {
  if (!m_pDataAcc->IsEOF()) {
    return FALSE;
  }
  return m_dwIndex >= m_dwBufferSize;
}
#define FXCRTM_XML_CHARTYPE_Normal 0x00
#define FXCRTM_XML_CHARTYPE_SpaceChar 0x01
#define FXCRTM_XML_CHARTYPE_Letter 0x02
#define FXCRTM_XML_CHARTYPE_Digital 0x04
#define FXCRTM_XML_CHARTYPE_NameIntro 0x08
#define FXCRTM_XML_CHARTYPE_NameChar 0x10
#define FXCRTM_XML_CHARTYPE_HexDigital 0x20
#define FXCRTM_XML_CHARTYPE_HexLowerLetter 0x40
#define FXCRTM_XML_CHARTYPE_HexUpperLetter 0x60
#define FXCRTM_XML_CHARTYPE_HexChar 0x60
uint8_t g_FXCRT_XML_ByteTypes[256] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00,
    0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x01, 0x01,
};
FX_BOOL g_FXCRT_XML_IsWhiteSpace(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_SpaceChar) != 0;
}
FX_BOOL g_FXCRT_XML_IsLetter(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_Letter) != 0;
}
FX_BOOL g_FXCRT_XML_IsDigital(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_Digital) != 0;
}
FX_BOOL g_FXCRT_XML_IsNameIntro(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_NameIntro) != 0;
}
FX_BOOL g_FXCRT_XML_IsNameChar(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_NameChar) != 0;
}
FX_BOOL g_FXCRT_XML_IsHexChar(uint8_t ch) {
  return (g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_HexChar) != 0;
}
void CXML_Parser::SkipWhiteSpaces() {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return;
  }
  do {
    while (m_dwIndex < m_dwBufferSize &&
           g_FXCRT_XML_IsWhiteSpace(m_pBuffer[m_dwIndex])) {
      m_dwIndex++;
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
}
void CXML_Parser::GetName(CFX_ByteString& space, CFX_ByteString& name) {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return;
  }
  CFX_ByteTextBuf buf;
  uint8_t ch;
  do {
    while (m_dwIndex < m_dwBufferSize) {
      ch = m_pBuffer[m_dwIndex];
      if (ch == ':') {
        space = buf.AsStringC();
        buf.Clear();
      } else if (g_FXCRT_XML_IsNameChar(ch)) {
        buf.AppendChar(ch);
      } else {
        break;
      }
      m_dwIndex++;
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  name = buf.AsStringC();
}
void CXML_Parser::SkipLiterals(const CFX_ByteStringC& str) {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return;
  }
  int32_t i = 0, iLen = str.GetLength();
  do {
    while (m_dwIndex < m_dwBufferSize) {
      if (str.GetAt(i) != m_pBuffer[m_dwIndex++]) {
        i = 0;
      } else {
        i++;
        if (i == iLen) {
          break;
        }
      }
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (i == iLen) {
      return;
    }
    if (m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  while (!m_pDataAcc->IsEOF()) {
    ReadNextBlock();
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwBufferSize;
  }
  m_dwIndex = m_dwBufferSize;
}
uint32_t CXML_Parser::GetCharRef() {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return 0;
  }
  uint8_t ch;
  int32_t iState = 0;
  CFX_ByteTextBuf buf;
  uint32_t code = 0;
  do {
    while (m_dwIndex < m_dwBufferSize) {
      ch = m_pBuffer[m_dwIndex];
      switch (iState) {
        case 0:
          if (ch == '#') {
            m_dwIndex++;
            iState = 2;
            break;
          }
          iState = 1;
        case 1:
          m_dwIndex++;
          if (ch == ';') {
            CFX_ByteStringC ref = buf.AsStringC();
            if (ref == "gt") {
              code = '>';
            } else if (ref == "lt") {
              code = '<';
            } else if (ref == "amp") {
              code = '&';
            } else if (ref == "apos") {
              code = '\'';
            } else if (ref == "quot") {
              code = '"';
            }
            iState = 10;
            break;
          }
          buf.AppendByte(ch);
          break;
        case 2:
          if (ch == 'x') {
            m_dwIndex++;
            iState = 4;
            break;
          }
          iState = 3;
        case 3:
          m_dwIndex++;
          if (ch == ';') {
            iState = 10;
            break;
          }
          if (g_FXCRT_XML_IsDigital(ch))
            code = code * 10 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          break;
        case 4:
          m_dwIndex++;
          if (ch == ';') {
            iState = 10;
            break;
          }
          uint8_t nHex =
              g_FXCRT_XML_ByteTypes[ch] & FXCRTM_XML_CHARTYPE_HexChar;
          if (nHex) {
            if (nHex == FXCRTM_XML_CHARTYPE_HexDigital) {
              code =
                  (code << 4) + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
            } else if (nHex == FXCRTM_XML_CHARTYPE_HexLowerLetter) {
              code = (code << 4) + ch - 87;
            } else {
              code = (code << 4) + ch - 55;
            }
          }
          break;
      }
      if (iState == 10) {
        break;
      }
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (iState == 10 || m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  return code;
}
void CXML_Parser::GetAttrValue(CFX_WideString& value) {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return;
  }
  CFX_UTF8Decoder decoder;
  uint8_t mark = 0, ch = 0;
  do {
    while (m_dwIndex < m_dwBufferSize) {
      ch = m_pBuffer[m_dwIndex];
      if (mark == 0) {
        if (ch != '\'' && ch != '"') {
          return;
        }
        mark = ch;
        m_dwIndex++;
        ch = 0;
        continue;
      }
      m_dwIndex++;
      if (ch == mark) {
        break;
      }
      if (ch == '&') {
        decoder.AppendChar(GetCharRef());
        if (IsEOF()) {
          value = decoder.GetResult();
          return;
        }
      } else {
        decoder.Input(ch);
      }
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (ch == mark || m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  value = decoder.GetResult();
}
void CXML_Parser::GetTagName(CFX_ByteString& space,
                             CFX_ByteString& name,
                             FX_BOOL& bEndTag,
                             FX_BOOL bStartTag) {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return;
  }
  bEndTag = FALSE;
  uint8_t ch;
  int32_t iState = bStartTag ? 1 : 0;
  do {
    while (m_dwIndex < m_dwBufferSize) {
      ch = m_pBuffer[m_dwIndex];
      switch (iState) {
        case 0:
          m_dwIndex++;
          if (ch != '<') {
            break;
          }
          iState = 1;
          break;
        case 1:
          if (ch == '?') {
            m_dwIndex++;
            SkipLiterals("?>");
            iState = 0;
            break;
          } else if (ch == '!') {
            m_dwIndex++;
            SkipLiterals("-->");
            iState = 0;
            break;
          }
          if (ch == '/') {
            m_dwIndex++;
            GetName(space, name);
            bEndTag = TRUE;
          } else {
            GetName(space, name);
            bEndTag = FALSE;
          }
          return;
      }
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
}
CXML_Element* CXML_Parser::ParseElement(CXML_Element* pParent,
                                        FX_BOOL bStartTag) {
  m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
  if (IsEOF()) {
    return nullptr;
  }
  CFX_ByteString tag_name, tag_space;
  FX_BOOL bEndTag;
  GetTagName(tag_space, tag_name, bEndTag, bStartTag);
  if (tag_name.IsEmpty() || bEndTag) {
    return nullptr;
  }
  CXML_Element* pElement = new CXML_Element;
  pElement->m_pParent = pParent;
  pElement->SetTag(tag_space.AsStringC(), tag_name.AsStringC());
  do {
    CFX_ByteString attr_space, attr_name;
    while (m_dwIndex < m_dwBufferSize) {
      SkipWhiteSpaces();
      if (IsEOF()) {
        break;
      }
      if (!g_FXCRT_XML_IsNameIntro(m_pBuffer[m_dwIndex])) {
        break;
      }
      GetName(attr_space, attr_name);
      SkipWhiteSpaces();
      if (IsEOF()) {
        break;
      }
      if (m_pBuffer[m_dwIndex] != '=') {
        break;
      }
      m_dwIndex++;
      SkipWhiteSpaces();
      if (IsEOF()) {
        break;
      }
      CFX_WideString attr_value;
      GetAttrValue(attr_value);
      pElement->m_AttrMap.SetAt(attr_space, attr_name, attr_value);
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  SkipWhiteSpaces();
  if (IsEOF()) {
    return pElement;
  }
  uint8_t ch = m_pBuffer[m_dwIndex++];
  if (ch == '/') {
    m_dwIndex++;
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    return pElement;
  }
  if (ch != '>') {
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    delete pElement;
    return nullptr;
  }
  SkipWhiteSpaces();
  if (IsEOF()) {
    return pElement;
  }
  CFX_UTF8Decoder decoder;
  CFX_WideTextBuf content;
  FX_BOOL bCDATA = FALSE;
  int32_t iState = 0;
  do {
    while (m_dwIndex < m_dwBufferSize) {
      ch = m_pBuffer[m_dwIndex++];
      switch (iState) {
        case 0:
          if (ch == '<') {
            iState = 1;
          } else if (ch == '&') {
            decoder.ClearStatus();
            decoder.AppendChar(GetCharRef());
          } else {
            decoder.Input(ch);
          }
          break;
        case 1:
          if (ch == '!') {
            iState = 2;
          } else if (ch == '?') {
            SkipLiterals("?>");
            SkipWhiteSpaces();
            iState = 0;
          } else if (ch == '/') {
            CFX_ByteString space, name;
            GetName(space, name);
            SkipWhiteSpaces();
            m_dwIndex++;
            iState = 10;
          } else {
            content << decoder.GetResult();
            CFX_WideString dataStr = content.MakeString();
            if (!bCDATA && !m_bSaveSpaceChars) {
              dataStr.TrimRight(L" \t\r\n");
            }
            InsertContentSegment(bCDATA, dataStr.AsStringC(), pElement);
            content.Clear();
            decoder.Clear();
            bCDATA = FALSE;
            iState = 0;
            m_dwIndex--;
            CXML_Element* pSubElement = ParseElement(pElement, TRUE);
            if (!pSubElement) {
              break;
            }
            pSubElement->m_pParent = pElement;
            pElement->m_Children.push_back(
                {CXML_Element::Element, pSubElement});
            SkipWhiteSpaces();
          }
          break;
        case 2:
          if (ch == '[') {
            SkipLiterals("]]>");
          } else if (ch == '-') {
            m_dwIndex++;
            SkipLiterals("-->");
          } else {
            SkipLiterals(">");
          }
          decoder.Clear();
          SkipWhiteSpaces();
          iState = 0;
          break;
      }
      if (iState == 10) {
        break;
      }
    }
    m_nOffset = m_nBufferOffset + (FX_FILESIZE)m_dwIndex;
    if (iState == 10 || m_dwIndex < m_dwBufferSize || IsEOF()) {
      break;
    }
  } while (ReadNextBlock());
  content << decoder.GetResult();
  CFX_WideString dataStr = content.MakeString();
  if (!m_bSaveSpaceChars) {
    dataStr.TrimRight(L" \t\r\n");
  }
  InsertContentSegment(bCDATA, dataStr.AsStringC(), pElement);
  content.Clear();
  decoder.Clear();
  bCDATA = FALSE;
  return pElement;
}
void CXML_Parser::InsertContentSegment(FX_BOOL bCDATA,
                                       const CFX_WideStringC& content,
                                       CXML_Element* pElement) {
  if (content.IsEmpty()) {
    return;
  }
  CXML_Content* pContent = new CXML_Content;
  pContent->Set(bCDATA, content);
  pElement->m_Children.push_back({CXML_Element::Content, pContent});
}
static CXML_Element* XML_ContinueParse(CXML_Parser& parser,
                                       FX_BOOL bSaveSpaceChars,
                                       FX_FILESIZE* pParsedSize) {
  parser.m_bSaveSpaceChars = bSaveSpaceChars;
  CXML_Element* pElement = parser.ParseElement(nullptr, FALSE);
  if (pParsedSize) {
    *pParsedSize = parser.m_nOffset;
  }
  return pElement;
}
CXML_Element* CXML_Element::Parse(const void* pBuffer,
                                  size_t size,
                                  FX_BOOL bSaveSpaceChars,
                                  FX_FILESIZE* pParsedSize) {
  CXML_Parser parser;
  if (!parser.Init((uint8_t*)pBuffer, size)) {
    return nullptr;
  }
  return XML_ContinueParse(parser, bSaveSpaceChars, pParsedSize);
}
CXML_Element* CXML_Element::Parse(IFX_FileRead* pFile,
                                  FX_BOOL bSaveSpaceChars,
                                  FX_FILESIZE* pParsedSize) {
  CXML_Parser parser;
  if (!parser.Init(pFile)) {
    return nullptr;
  }
  return XML_ContinueParse(parser, bSaveSpaceChars, pParsedSize);
}
CXML_Element* CXML_Element::Parse(IFX_BufferRead* pBuffer,
                                  FX_BOOL bSaveSpaceChars,
                                  FX_FILESIZE* pParsedSize) {
  CXML_Parser parser;
  if (!parser.Init(pBuffer)) {
    return nullptr;
  }
  return XML_ContinueParse(parser, bSaveSpaceChars, pParsedSize);
}
CXML_Element::CXML_Element() : m_QSpaceName(), m_TagName(), m_AttrMap() {}
CXML_Element::CXML_Element(const CFX_ByteStringC& qSpace,
                           const CFX_ByteStringC& tagName)
    : m_QSpaceName(), m_TagName(), m_AttrMap() {
  m_QSpaceName = qSpace;
  m_TagName = tagName;
}
CXML_Element::CXML_Element(const CFX_ByteStringC& qTagName)
    : m_pParent(nullptr), m_QSpaceName(), m_TagName(), m_AttrMap() {
  SetTag(qTagName);
}
CXML_Element::~CXML_Element() {
  Empty();
}
void CXML_Element::Empty() {
  RemoveChildren();
}
void CXML_Element::RemoveChildren() {
  for (const ChildRecord& record : m_Children) {
    if (record.type == Content) {
      delete static_cast<CXML_Content*>(record.child);
    } else if (record.type == Element) {
      CXML_Element* child = static_cast<CXML_Element*>(record.child);
      child->RemoveChildren();
      delete child;
    }
  }
  m_Children.clear();
}
CFX_ByteString CXML_Element::GetTagName(FX_BOOL bQualified) const {
  if (!bQualified || m_QSpaceName.IsEmpty()) {
    return m_TagName;
  }
  CFX_ByteString bsTag = m_QSpaceName;
  bsTag += ":";
  bsTag += m_TagName;
  return bsTag;
}

CFX_ByteString CXML_Element::GetNamespace(FX_BOOL bQualified) const {
  return bQualified ? m_QSpaceName : GetNamespaceURI(m_QSpaceName);
}

CFX_ByteString CXML_Element::GetNamespaceURI(
    const CFX_ByteString& qName) const {
  const CFX_WideString* pwsSpace;
  const CXML_Element* pElement = this;
  do {
    if (qName.IsEmpty()) {
      pwsSpace = pElement->m_AttrMap.Lookup("", "xmlns");
    } else {
      pwsSpace = pElement->m_AttrMap.Lookup("xmlns", qName);
    }
    if (pwsSpace) {
      break;
    }
    pElement = pElement->GetParent();
  } while (pElement);
  return pwsSpace ? FX_UTF8Encode(*pwsSpace) : CFX_ByteString();
}
void CXML_Element::GetAttrByIndex(int index,
                                  CFX_ByteString& space,
                                  CFX_ByteString& name,
                                  CFX_WideString& value) const {
  if (index < 0 || index >= m_AttrMap.GetSize()) {
    return;
  }
  CXML_AttrItem& item = m_AttrMap.GetAt(index);
  space = item.m_QSpaceName;
  name = item.m_AttrName;
  value = item.m_Value;
}
FX_BOOL CXML_Element::HasAttr(const CFX_ByteStringC& name) const {
  CFX_ByteStringC bsSpace;
  CFX_ByteStringC bsName;
  FX_XML_SplitQualifiedName(name, bsSpace, bsName);
  return !!m_AttrMap.Lookup(CFX_ByteString(bsSpace), CFX_ByteString(bsName));
}
FX_BOOL CXML_Element::GetAttrValue(const CFX_ByteStringC& name,
                                   CFX_WideString& attribute) const {
  CFX_ByteStringC bsSpace;
  CFX_ByteStringC bsName;
  FX_XML_SplitQualifiedName(name, bsSpace, bsName);
  return GetAttrValue(bsSpace, bsName, attribute);
}
FX_BOOL CXML_Element::GetAttrValue(const CFX_ByteStringC& space,
                                   const CFX_ByteStringC& name,
                                   CFX_WideString& attribute) const {
  const CFX_WideString* pValue =
      m_AttrMap.Lookup(CFX_ByteString(space), CFX_ByteString(name));
  if (pValue) {
    attribute = *pValue;
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXML_Element::GetAttrInteger(const CFX_ByteStringC& name,
                                     int& attribute) const {
  CFX_ByteStringC bsSpace;
  CFX_ByteStringC bsName;
  FX_XML_SplitQualifiedName(name, bsSpace, bsName);
  const CFX_WideString* pwsValue =
      m_AttrMap.Lookup(CFX_ByteString(bsSpace), CFX_ByteString(bsName));
  if (pwsValue) {
    attribute = pwsValue->GetInteger();
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXML_Element::GetAttrInteger(const CFX_ByteStringC& space,
                                     const CFX_ByteStringC& name,
                                     int& attribute) const {
  const CFX_WideString* pwsValue =
      m_AttrMap.Lookup(CFX_ByteString(space), CFX_ByteString(name));
  if (pwsValue) {
    attribute = pwsValue->GetInteger();
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CXML_Element::GetAttrFloat(const CFX_ByteStringC& name,
                                   FX_FLOAT& attribute) const {
  CFX_ByteStringC bsSpace, bsName;
  FX_XML_SplitQualifiedName(name, bsSpace, bsName);
  return GetAttrFloat(bsSpace, bsName, attribute);
}
FX_BOOL CXML_Element::GetAttrFloat(const CFX_ByteStringC& space,
                                   const CFX_ByteStringC& name,
                                   FX_FLOAT& attribute) const {
  const CFX_WideString* pValue =
      m_AttrMap.Lookup(CFX_ByteString(space), CFX_ByteString(name));
  if (pValue) {
    attribute = pValue->GetFloat();
    return TRUE;
  }
  return FALSE;
}
CXML_Element::ChildType CXML_Element::GetChildType(uint32_t index) const {
  return index < m_Children.size() ? m_Children[index].type : Invalid;
}
CFX_WideString CXML_Element::GetContent(uint32_t index) const {
  if (index < m_Children.size() && m_Children[index].type == Content) {
    CXML_Content* pContent =
        static_cast<CXML_Content*>(m_Children[index].child);
    if (pContent)
      return pContent->m_Content;
  }
  return CFX_WideString();
}
CXML_Element* CXML_Element::GetElement(uint32_t index) const {
  if (index < m_Children.size() && m_Children[index].type == Element) {
    return static_cast<CXML_Element*>(m_Children[index].child);
  }
  return nullptr;
}
uint32_t CXML_Element::CountElements(const CFX_ByteStringC& space,
                                     const CFX_ByteStringC& tag) const {
  int count = 0;
  for (const ChildRecord& record : m_Children) {
    if (record.type != Element)
      continue;

    CXML_Element* pKid = static_cast<CXML_Element*>(record.child);
    if ((space.IsEmpty() || pKid->m_QSpaceName == space) &&
        pKid->m_TagName == tag) {
      count++;
    }
  }
  return count;
}
CXML_Element* CXML_Element::GetElement(const CFX_ByteStringC& space,
                                       const CFX_ByteStringC& tag,
                                       int index) const {
  if (index < 0)
    return nullptr;

  for (const ChildRecord& record : m_Children) {
    if (record.type != Element)
      continue;

    CXML_Element* pKid = static_cast<CXML_Element*>(record.child);
    if ((space.IsEmpty() || pKid->m_QSpaceName == space) &&
        pKid->m_TagName == tag) {
      if (index-- == 0)
        return pKid;
    }
  }
  return nullptr;
}
uint32_t CXML_Element::FindElement(CXML_Element* pChild) const {
  int index = 0;
  for (const ChildRecord& record : m_Children) {
    if (record.type == Element &&
        static_cast<CXML_Element*>(record.child) == pChild) {
      return index;
    }
    ++index;
  }
  return (uint32_t)-1;
}

bool CXML_AttrItem::Matches(const CFX_ByteString& space,
                            const CFX_ByteString& name) const {
  return (space.IsEmpty() || m_QSpaceName == space) && m_AttrName == name;
}

CXML_AttrMap::CXML_AttrMap() {}

CXML_AttrMap::~CXML_AttrMap() {}

const CFX_WideString* CXML_AttrMap::Lookup(const CFX_ByteString& space,
                                           const CFX_ByteString& name) const {
  if (!m_pMap)
    return nullptr;

  for (const auto& item : *m_pMap) {
    if (item.Matches(space, name))
      return &item.m_Value;
  }
  return nullptr;
}

void CXML_AttrMap::SetAt(const CFX_ByteString& space,
                         const CFX_ByteString& name,
                         const CFX_WideString& value) {
  if (!m_pMap)
    m_pMap.reset(new std::vector<CXML_AttrItem>);

  for (CXML_AttrItem& item : *m_pMap) {
    if (item.Matches(space, name)) {
      item.m_Value = value;
      return;
    }
  }

  m_pMap->push_back({space, name, CFX_WideString(value)});
}

int CXML_AttrMap::GetSize() const {
  return m_pMap ? pdfium::CollectionSize<int>(*m_pMap) : 0;
}

CXML_AttrItem& CXML_AttrMap::GetAt(int index) const {
  return (*m_pMap)[index];
}
