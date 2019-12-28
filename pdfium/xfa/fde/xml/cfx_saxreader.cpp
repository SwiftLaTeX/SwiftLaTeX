// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/xml/cfx_saxreader.h"

#include <algorithm>

#include "xfa/fxfa/include/xfa_checksum.h"

enum class CFX_SaxMode {
  Text = 0,
  NodeStart,
  DeclOrComment,
  DeclNode,
  Comment,
  CommentContent,
  TagName,
  TagAttributeName,
  TagAttributeEqual,
  TagAttributeValue,
  TagMaybeClose,
  TagClose,
  TagEnd,
  TargetData,
  MAX
};

class CFX_SAXCommentContext {
 public:
  CFX_SAXCommentContext() : m_iHeaderCount(0), m_iTailCount(0) {}
  int32_t m_iHeaderCount;
  int32_t m_iTailCount;
};

namespace {

const uint32_t kSaxFileBufSize = 32768;

typedef void (CFX_SAXReader::*FX_SAXReader_LPFParse)();
static const FX_SAXReader_LPFParse
    g_FX_SAXReader_LPFParse[static_cast<int>(CFX_SaxMode::MAX)] = {
        &CFX_SAXReader::ParseText,
        &CFX_SAXReader::ParseNodeStart,
        &CFX_SAXReader::ParseDeclOrComment,
        &CFX_SAXReader::ParseDeclNode,
        &CFX_SAXReader::ParseComment,
        &CFX_SAXReader::ParseCommentContent,
        &CFX_SAXReader::ParseTagName,
        &CFX_SAXReader::ParseTagAttributeName,
        &CFX_SAXReader::ParseTagAttributeEqual,
        &CFX_SAXReader::ParseTagAttributeValue,
        &CFX_SAXReader::ParseMaybeClose,
        &CFX_SAXReader::ParseTagClose,
        &CFX_SAXReader::ParseTagEnd,
        &CFX_SAXReader::ParseTargetData,
};

}  // namespace

CFX_SAXFile::CFX_SAXFile()
    : m_pFile(nullptr),
      m_dwStart(0),
      m_dwEnd(0),
      m_dwCur(0),
      m_pBuf(nullptr),
      m_dwBufSize(0),
      m_dwBufIndex(0) {}
FX_BOOL CFX_SAXFile::StartFile(IFX_FileRead* pFile,
                               uint32_t dwStart,
                               uint32_t dwLen) {
  ASSERT(!m_pFile && pFile);
  uint32_t dwSize = pFile->GetSize();
  if (dwStart >= dwSize) {
    return FALSE;
  }
  if (dwLen == static_cast<uint32_t>(-1) || dwStart + dwLen > dwSize) {
    dwLen = dwSize - dwStart;
  }
  if (dwLen == 0) {
    return FALSE;
  }
  m_dwBufSize = std::min(dwLen, kSaxFileBufSize);
  m_pBuf = FX_Alloc(uint8_t, m_dwBufSize);
  if (!pFile->ReadBlock(m_pBuf, dwStart, m_dwBufSize)) {
    return FALSE;
  }
  m_dwStart = dwStart;
  m_dwEnd = dwStart + dwLen;
  m_dwCur = dwStart;
  m_pFile = pFile;
  m_dwBufIndex = 0;
  return TRUE;
}
FX_BOOL CFX_SAXFile::ReadNextBlock() {
  ASSERT(m_pFile);
  uint32_t dwSize = m_dwEnd - m_dwCur;
  if (dwSize == 0) {
    return FALSE;
  }
  m_dwBufSize = std::min(dwSize, kSaxFileBufSize);
  if (!m_pFile->ReadBlock(m_pBuf, m_dwCur, m_dwBufSize)) {
    return FALSE;
  }
  m_dwBufIndex = 0;
  return TRUE;
}
void CFX_SAXFile::Reset() {
  if (m_pBuf) {
    FX_Free(m_pBuf);
    m_pBuf = nullptr;
  }
  m_pFile = nullptr;
}
CFX_SAXReader::CFX_SAXReader()
    : m_File(),
      m_pHandler(nullptr),
      m_iState(-1),
      m_pRoot(nullptr),
      m_pCurItem(nullptr),
      m_dwItemID(0),
      m_iDataSize(256),
      m_iNameSize(256),
      m_dwParseMode(0),
      m_pCommentContext(nullptr) {
  m_pszData = FX_Alloc(uint8_t, m_iDataSize);
  m_pszName = FX_Alloc(uint8_t, m_iNameSize);
}
CFX_SAXReader::~CFX_SAXReader() {
  Reset();
  if (m_pszData) {
    FX_Free(m_pszData);
    m_pszData = nullptr;
  }
  if (m_pszName) {
    FX_Free(m_pszName);
    m_pszName = nullptr;
  }
}
void CFX_SAXReader::Reset() {
  m_File.Reset();
  CFX_SAXItem* pItem = m_pRoot;
  while (pItem) {
    CFX_SAXItem* pNext = pItem->m_pNext;
    delete pItem;
    pItem = pNext;
  }
  m_pRoot = nullptr;
  m_pCurItem = nullptr;
  m_dwItemID = 0;
  m_SkipStack.RemoveAll();
  m_SkipChar = 0;
  m_iDataLength = 0;
  m_iEntityStart = -1;
  m_iNameLength = 0;
  m_iDataPos = 0;
  if (m_pCommentContext) {
    delete m_pCommentContext;
    m_pCommentContext = nullptr;
  }
}
inline void CFX_SAXReader::Push() {
  CFX_SAXItem* pNew = new CFX_SAXItem;
  pNew->m_dwID = ++m_dwItemID;
  pNew->m_bSkip = m_pCurItem->m_bSkip;
  pNew->m_pPrev = m_pCurItem;
  m_pCurItem->m_pNext = pNew;
  m_pCurItem = pNew;
}
inline void CFX_SAXReader::Pop() {
  if (!m_pCurItem) {
    return;
  }
  CFX_SAXItem* pPrev = m_pCurItem->m_pPrev;
  pPrev->m_pNext = nullptr;
  delete m_pCurItem;
  m_pCurItem = pPrev;
}
inline void CFX_SAXReader::AppendData(uint8_t ch) {
  ReallocDataBuffer();
  m_pszData[m_iDataPos++] = ch;
}
inline void CFX_SAXReader::AppendName(uint8_t ch) {
  ReallocNameBuffer();
  m_pszName[m_iDataPos++] = ch;
}
void CFX_SAXReader::ReallocDataBuffer() {
  if (m_iDataPos < m_iDataSize) {
    return;
  }
  if (m_iDataSize <= 1024 * 1024) {
    m_iDataSize *= 2;
  } else {
    m_iDataSize += 1024 * 1024;
  }
  m_pszData = (uint8_t*)FX_Realloc(uint8_t, m_pszData, m_iDataSize);
}
void CFX_SAXReader::ReallocNameBuffer() {
  if (m_iDataPos < m_iNameSize) {
    return;
  }
  if (m_iNameSize <= 1024 * 1024) {
    m_iNameSize *= 2;
  } else {
    m_iNameSize += 1024 * 1024;
  }
  m_pszName = (uint8_t*)FX_Realloc(uint8_t, m_pszName, m_iNameSize);
}
inline FX_BOOL CFX_SAXReader::SkipSpace(uint8_t ch) {
  return (m_dwParseMode & CFX_SaxParseMode_NotSkipSpace) == 0 && ch < 0x21;
}
int32_t CFX_SAXReader::StartParse(IFX_FileRead* pFile,
                                  uint32_t dwStart,
                                  uint32_t dwLen,
                                  uint32_t dwParseMode) {
  m_iState = -1;
  Reset();
  if (!m_File.StartFile(pFile, dwStart, dwLen)) {
    return -1;
  }
  m_iState = 0;
  m_eMode = CFX_SaxMode::Text;
  m_ePrevMode = CFX_SaxMode::Text;
  m_bCharData = FALSE;
  m_dwDataOffset = 0;
  m_pRoot = m_pCurItem = new CFX_SAXItem;
  m_pCurItem->m_dwID = ++m_dwItemID;
  m_dwParseMode = dwParseMode;
  return 0;
}

int32_t CFX_SAXReader::ContinueParse(IFX_Pause* pPause) {
  if (m_iState < 0 || m_iState > 99) {
    return m_iState;
  }
  while (m_File.m_dwCur < m_File.m_dwEnd) {
    uint32_t& index = m_File.m_dwBufIndex;
    uint32_t size = m_File.m_dwBufSize;
    const uint8_t* pBuf = m_File.m_pBuf;
    while (index < size) {
      m_CurByte = pBuf[index];
      (this->*g_FX_SAXReader_LPFParse[static_cast<int>(m_eMode)])();
      index++;
    }
    m_File.m_dwCur += index;
    m_iState = (m_File.m_dwCur - m_File.m_dwStart) * 100 /
               (m_File.m_dwEnd - m_File.m_dwStart);
    if (m_File.m_dwCur >= m_File.m_dwEnd) {
      break;
    }
    if (!m_File.ReadNextBlock()) {
      m_iState = -2;
      break;
    }
    m_dwDataOffset = 0;
    if (pPause && pPause->NeedToPauseNow()) {
      break;
    }
  }
  return m_iState;
}
void CFX_SAXReader::ParseChar(uint8_t ch) {
  ReallocDataBuffer();
  m_pszData[m_iDataPos] = ch;
  if (m_iEntityStart > -1 && ch == ';') {
    int32_t iSaveEntityStart = m_iEntityStart;
    CFX_ByteString csEntity(m_pszData + m_iEntityStart + 1,
                            m_iDataPos - m_iEntityStart - 1);
    int32_t iLen = csEntity.GetLength();
    if (iLen > 0) {
      if (csEntity[0] == '#') {
        if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_sharp) == 0) {
          ch = 0;
          uint8_t w;
          if (iLen > 1 && csEntity[1] == 'x') {
            for (int32_t i = 2; i < iLen; i++) {
              w = csEntity[i];
              if (w >= '0' && w <= '9') {
                ch = (ch << 4) + w - '0';
              } else if (w >= 'A' && w <= 'F') {
                ch = (ch << 4) + w - 55;
              } else if (w >= 'a' && w <= 'f') {
                ch = (ch << 4) + w - 87;
              } else {
                break;
              }
            }
          } else {
            for (int32_t i = 1; i < iLen; i++) {
              w = csEntity[i];
              if (w < '0' || w > '9') {
                break;
              }
              ch = ch * 10 + w - '0';
            }
          }
          if (ch != 0) {
            m_pszData[m_iEntityStart++] = ch;
          }
        }
      } else {
        if (csEntity.Compare("amp") == 0) {
          if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_amp) == 0) {
            m_pszData[m_iEntityStart++] = '&';
          }
        } else if (csEntity.Compare("lt") == 0) {
          if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_lt) == 0) {
            m_pszData[m_iEntityStart++] = '<';
          }
        } else if (csEntity.Compare("gt") == 0) {
          if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_gt) == 0) {
            m_pszData[m_iEntityStart++] = '>';
          }
        } else if (csEntity.Compare("apos") == 0) {
          if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_apos) == 0) {
            m_pszData[m_iEntityStart++] = '\'';
          }
        } else if (csEntity.Compare("quot") == 0) {
          if ((m_dwParseMode & CFX_SaxParseMode_NotConvert_quot) == 0) {
            m_pszData[m_iEntityStart++] = '\"';
          }
        }
      }
    }
    if (iSaveEntityStart != m_iEntityStart) {
      m_iDataPos = m_iEntityStart;
      m_iEntityStart = -1;
    } else {
      m_iDataPos++;
      m_iEntityStart = -1;
    }
  } else {
    if (m_iEntityStart < 0 && ch == '&') {
      m_iEntityStart = m_iDataPos;
    }
    m_iDataPos++;
  }
}
void CFX_SAXReader::ParseText() {
  if (m_CurByte == '<') {
    if (m_iDataPos > 0) {
      m_iDataLength = m_iDataPos;
      m_iDataPos = 0;
      if (m_pHandler) {
        NotifyData();
      }
    }
    Push();
    m_dwNodePos = m_File.m_dwCur + m_File.m_dwBufIndex;
    m_eMode = CFX_SaxMode::NodeStart;
    return;
  }
  if (m_iDataPos < 1 && SkipSpace(m_CurByte)) {
    return;
  }
  ParseChar(m_CurByte);
}
void CFX_SAXReader::ParseNodeStart() {
  if (m_CurByte == '?') {
    m_pCurItem->m_eNode = CFX_SAXItem::Type::Instruction;
    m_eMode = CFX_SaxMode::TagName;
    return;
  }
  if (m_CurByte == '!') {
    m_eMode = CFX_SaxMode::DeclOrComment;
    return;
  }
  if (m_CurByte == '/') {
    m_eMode = CFX_SaxMode::TagEnd;
    return;
  }
  if (m_CurByte == '>') {
    Pop();
    m_eMode = CFX_SaxMode::Text;
    return;
  }
  if (m_CurByte > 0x20) {
    m_dwDataOffset = m_File.m_dwBufIndex;
    m_pCurItem->m_eNode = CFX_SAXItem::Type::Tag;
    m_eMode = CFX_SaxMode::TagName;
    AppendData(m_CurByte);
  }
}
void CFX_SAXReader::ParseDeclOrComment() {
  if (m_CurByte == '-') {
    m_eMode = CFX_SaxMode::Comment;
    m_pCurItem->m_eNode = CFX_SAXItem::Type::Comment;
    if (!m_pCommentContext)
      m_pCommentContext = new CFX_SAXCommentContext;

    m_pCommentContext->m_iHeaderCount = 1;
    m_pCommentContext->m_iTailCount = 0;
  } else {
    m_eMode = CFX_SaxMode::DeclNode;
    m_dwDataOffset = m_File.m_dwBufIndex;
    m_SkipChar = '>';
    m_SkipStack.Add('>');
    SkipNode();
  }
}
void CFX_SAXReader::ParseComment() {
  m_pCommentContext->m_iHeaderCount = 2;
  m_dwNodePos = m_File.m_dwCur + m_File.m_dwBufIndex;
  m_eMode = CFX_SaxMode::CommentContent;
}
void CFX_SAXReader::ParseCommentContent() {
  if (m_CurByte == '-') {
    m_pCommentContext->m_iTailCount++;
  } else if (m_CurByte == '>' && m_pCommentContext->m_iTailCount == 2) {
    m_iDataLength = m_iDataPos;
    m_iDataPos = 0;
    if (m_pHandler) {
      NotifyTargetData();
    }
    Pop();
    m_eMode = CFX_SaxMode::Text;
  } else {
    while (m_pCommentContext->m_iTailCount > 0) {
      AppendData('-');
      m_pCommentContext->m_iTailCount--;
    }
    AppendData(m_CurByte);
  }
}
void CFX_SAXReader::ParseDeclNode() {
  SkipNode();
}
void CFX_SAXReader::ParseTagName() {
  if (m_CurByte < 0x21 || m_CurByte == '/' || m_CurByte == '>' ||
      m_CurByte == '?') {
    m_iDataLength = m_iDataPos;
    m_iDataPos = 0;
    if (m_pHandler) {
      NotifyEnter();
    }
    if (m_CurByte < 0x21) {
      m_eMode = CFX_SaxMode::TagAttributeName;
    } else if (m_CurByte == '/' || m_CurByte == '?') {
      m_ePrevMode = m_eMode;
      m_eMode = CFX_SaxMode::TagMaybeClose;
    } else {
      if (m_pHandler) {
        NotifyBreak();
      }
      m_eMode = CFX_SaxMode::Text;
    }
  } else {
    AppendData(m_CurByte);
  }
}
void CFX_SAXReader::ParseTagAttributeName() {
  if (m_CurByte < 0x21 || m_CurByte == '=') {
    if (m_iDataPos < 1 && m_CurByte < 0x21) {
      return;
    }
    m_iNameLength = m_iDataPos;
    m_iDataPos = 0;
    m_SkipChar = 0;
    m_eMode = m_CurByte == '=' ? CFX_SaxMode::TagAttributeValue
                               : CFX_SaxMode::TagAttributeEqual;
    return;
  }
  if (m_CurByte == '/' || m_CurByte == '>' || m_CurByte == '?') {
    if (m_CurByte == '/' || m_CurByte == '?') {
      m_ePrevMode = m_eMode;
      m_eMode = CFX_SaxMode::TagMaybeClose;
    } else {
      if (m_pHandler) {
        NotifyBreak();
      }
      m_eMode = CFX_SaxMode::Text;
    }
    return;
  }
  if (m_iDataPos < 1) {
    m_dwDataOffset = m_File.m_dwBufIndex;
  }
  AppendName(m_CurByte);
}
void CFX_SAXReader::ParseTagAttributeEqual() {
  if (m_CurByte == '=') {
    m_SkipChar = 0;
    m_eMode = CFX_SaxMode::TagAttributeValue;
    return;
  } else if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
    m_iDataPos = m_iNameLength;
    AppendName(0x20);
    m_eMode = CFX_SaxMode::TargetData;
    ParseTargetData();
  }
}
void CFX_SAXReader::ParseTagAttributeValue() {
  if (m_SkipChar) {
    if (m_SkipChar == m_CurByte) {
      {
        m_iDataLength = m_iDataPos;
        m_iDataPos = 0;
        if (m_pHandler) {
          NotifyAttribute();
        }
      }
      m_SkipChar = 0;
      m_eMode = CFX_SaxMode::TagAttributeName;
      return;
    }
    ParseChar(m_CurByte);
    return;
  }
  if (m_CurByte < 0x21) {
    return;
  }
  if (m_iDataPos < 1) {
    if (m_CurByte == '\'' || m_CurByte == '\"') {
      m_SkipChar = m_CurByte;
    }
  }
}
void CFX_SAXReader::ParseMaybeClose() {
  if (m_CurByte == '>') {
    if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
      m_iNameLength = m_iDataPos;
      m_iDataPos = 0;
      if (m_pHandler) {
        NotifyTargetData();
      }
    }
    ParseTagClose();
    m_eMode = CFX_SaxMode::Text;
  } else if (m_ePrevMode == CFX_SaxMode::TagName) {
    AppendData('/');
    m_eMode = CFX_SaxMode::TagName;
    m_ePrevMode = CFX_SaxMode::Text;
    ParseTagName();
  } else if (m_ePrevMode == CFX_SaxMode::TagAttributeName) {
    AppendName('/');
    m_eMode = CFX_SaxMode::TagAttributeName;
    m_ePrevMode = CFX_SaxMode::Text;
    ParseTagAttributeName();
  } else if (m_ePrevMode == CFX_SaxMode::TargetData) {
    AppendName('?');
    m_eMode = CFX_SaxMode::TargetData;
    m_ePrevMode = CFX_SaxMode::Text;
    ParseTargetData();
  }
}
void CFX_SAXReader::ParseTagClose() {
  m_dwNodePos = m_File.m_dwCur + m_File.m_dwBufIndex;
  if (m_pHandler) {
    NotifyClose();
  }
  Pop();
}
void CFX_SAXReader::ParseTagEnd() {
  if (m_CurByte < 0x21) {
    return;
  }
  if (m_CurByte == '>') {
    Pop();
    m_dwNodePos = m_File.m_dwCur + m_File.m_dwBufIndex;
    m_iDataLength = m_iDataPos;
    m_iDataPos = 0;
    if (m_pHandler) {
      NotifyEnd();
    }
    Pop();
    m_eMode = CFX_SaxMode::Text;
  } else {
    ParseChar(m_CurByte);
  }
}
void CFX_SAXReader::ParseTargetData() {
  if (m_CurByte == '?') {
    m_ePrevMode = m_eMode;
    m_eMode = CFX_SaxMode::TagMaybeClose;
  } else {
    AppendName(m_CurByte);
  }
}
void CFX_SAXReader::SkipNode() {
  int32_t iLen = m_SkipStack.GetSize();
  if (m_SkipChar == '\'' || m_SkipChar == '\"') {
    if (m_CurByte != m_SkipChar) {
      return;
    }
    iLen--;
    ASSERT(iLen > -1);
    m_SkipStack.RemoveAt(iLen, 1);
    m_SkipChar = iLen ? m_SkipStack[iLen - 1] : 0;
    return;
  }
  switch (m_CurByte) {
    case '<':
      m_SkipChar = '>';
      m_SkipStack.Add('>');
      break;
    case '[':
      m_SkipChar = ']';
      m_SkipStack.Add(']');
      break;
    case '(':
      m_SkipChar = ')';
      m_SkipStack.Add(')');
      break;
    case '\'':
      m_SkipChar = '\'';
      m_SkipStack.Add('\'');
      break;
    case '\"':
      m_SkipChar = '\"';
      m_SkipStack.Add('\"');
      break;
    default:
      if (m_CurByte == m_SkipChar) {
        iLen--;
        m_SkipStack.RemoveAt(iLen, 1);
        m_SkipChar = iLen ? m_SkipStack[iLen - 1] : 0;
        if (iLen == 0 && m_CurByte == '>') {
          m_iDataLength = m_iDataPos;
          m_iDataPos = 0;
          if (m_iDataLength >= 9 &&
              FXSYS_memcmp(m_pszData, "[CDATA[", 7 * sizeof(uint8_t)) == 0 &&
              FXSYS_memcmp(m_pszData + m_iDataLength - 2, "]]",
                           2 * sizeof(uint8_t)) == 0) {
            Pop();
            m_iDataLength -= 9;
            m_dwDataOffset += 7;
            FXSYS_memmove(m_pszData, m_pszData + 7,
                          m_iDataLength * sizeof(uint8_t));
            m_bCharData = TRUE;
            if (m_pHandler) {
              NotifyData();
            }
            m_bCharData = FALSE;
          } else {
            Pop();
          }
          m_eMode = CFX_SaxMode::Text;
        }
      }
      break;
  }
  if (iLen > 0) {
    ParseChar(m_CurByte);
  }
}

void CFX_SAXReader::NotifyData() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Tag)
    m_pHandler->OnTagData(
        m_pCurItem->m_pNode,
        m_bCharData ? CFX_SAXItem::Type::CharData : CFX_SAXItem::Type::Text,
        CFX_ByteStringC(m_pszData, m_iDataLength),
        m_File.m_dwCur + m_dwDataOffset);
}

void CFX_SAXReader::NotifyEnter() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Tag ||
      m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
    m_pCurItem->m_pNode =
        m_pHandler->OnTagEnter(CFX_ByteStringC(m_pszData, m_iDataLength),
                               m_pCurItem->m_eNode, m_dwNodePos);
  }
}

void CFX_SAXReader::NotifyAttribute() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Tag ||
      m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
    m_pHandler->OnTagAttribute(m_pCurItem->m_pNode,
                               CFX_ByteStringC(m_pszName, m_iNameLength),
                               CFX_ByteStringC(m_pszData, m_iDataLength));
  }
}

void CFX_SAXReader::NotifyBreak() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Tag)
    m_pHandler->OnTagBreak(m_pCurItem->m_pNode);
}

void CFX_SAXReader::NotifyClose() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Tag ||
      m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
    m_pHandler->OnTagClose(m_pCurItem->m_pNode, m_dwNodePos);
  }
}

void CFX_SAXReader::NotifyEnd() {
  if (m_pCurItem->m_eNode != CFX_SAXItem::Type::Tag)
    return;

  m_pHandler->OnTagEnd(m_pCurItem->m_pNode,
                       CFX_ByteStringC(m_pszData, m_iDataLength), m_dwNodePos);
}

void CFX_SAXReader::NotifyTargetData() {
  if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Instruction) {
    m_pHandler->OnTargetData(m_pCurItem->m_pNode, m_pCurItem->m_eNode,
                             CFX_ByteStringC(m_pszName, m_iNameLength),
                             m_dwNodePos);
  } else if (m_pCurItem->m_eNode == CFX_SAXItem::Type::Comment) {
    m_pHandler->OnTargetData(m_pCurItem->m_pNode, m_pCurItem->m_eNode,
                             CFX_ByteStringC(m_pszData, m_iDataLength),
                             m_dwNodePos);
  }
}

void CFX_SAXReader::SkipCurrentNode() {
  if (!m_pCurItem)
    return;

  m_pCurItem->m_bSkip = TRUE;
}

void CFX_SAXReader::SetHandler(CXFA_SAXReaderHandler* pHandler) {
  m_pHandler = pHandler;
}
