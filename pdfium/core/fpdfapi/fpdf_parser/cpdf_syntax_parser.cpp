// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/cpdf_syntax_parser.h"

#include <vector>

#include "core/fpdfapi/fpdf_parser/cpdf_boolean.h"
#include "core/fpdfapi/fpdf_parser/cpdf_crypto_handler.h"
#include "core/fpdfapi/fpdf_parser/cpdf_null.h"
#include "core/fpdfapi/fpdf_parser/fpdf_parser_utility.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdfapi/fpdf_parser/include/fpdf_parser_decode.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcrt/include/fx_ext.h"
#include "third_party/base/numerics/safe_math.h"

namespace {

struct SearchTagRecord {
  CFX_ByteStringC m_bsTag;
  FX_STRSIZE m_Offset;
};

}  // namespace

// static
int CPDF_SyntaxParser::s_CurrentRecursionDepth = 0;

CPDF_SyntaxParser::CPDF_SyntaxParser()
    : m_MetadataObjnum(0),
      m_pFileAccess(nullptr),
      m_pFileBuf(nullptr),
      m_BufSize(CPDF_ModuleMgr::kFileBufSize) {}

CPDF_SyntaxParser::~CPDF_SyntaxParser() {
  FX_Free(m_pFileBuf);
}

FX_BOOL CPDF_SyntaxParser::GetCharAt(FX_FILESIZE pos, uint8_t& ch) {
  CFX_AutoRestorer<FX_FILESIZE> save_pos(&m_Pos);
  m_Pos = pos;
  return GetNextChar(ch);
}

FX_BOOL CPDF_SyntaxParser::GetNextChar(uint8_t& ch) {
  FX_FILESIZE pos = m_Pos + m_HeaderOffset;
  if (pos >= m_FileLen)
    return FALSE;

  if (m_BufOffset >= pos || (FX_FILESIZE)(m_BufOffset + m_BufSize) <= pos) {
    FX_FILESIZE read_pos = pos;
    uint32_t read_size = m_BufSize;
    if ((FX_FILESIZE)read_size > m_FileLen)
      read_size = (uint32_t)m_FileLen;

    if ((FX_FILESIZE)(read_pos + read_size) > m_FileLen) {
      if (m_FileLen < (FX_FILESIZE)read_size) {
        read_pos = 0;
        read_size = (uint32_t)m_FileLen;
      } else {
        read_pos = m_FileLen - read_size;
      }
    }

    if (!m_pFileAccess->ReadBlock(m_pFileBuf, read_pos, read_size))
      return FALSE;

    m_BufOffset = read_pos;
  }
  ch = m_pFileBuf[pos - m_BufOffset];
  m_Pos++;
  return TRUE;
}

FX_BOOL CPDF_SyntaxParser::GetCharAtBackward(FX_FILESIZE pos, uint8_t& ch) {
  pos += m_HeaderOffset;
  if (pos >= m_FileLen)
    return FALSE;

  if (m_BufOffset >= pos || (FX_FILESIZE)(m_BufOffset + m_BufSize) <= pos) {
    FX_FILESIZE read_pos;
    if (pos < (FX_FILESIZE)m_BufSize)
      read_pos = 0;
    else
      read_pos = pos - m_BufSize + 1;

    uint32_t read_size = m_BufSize;
    if ((FX_FILESIZE)(read_pos + read_size) > m_FileLen) {
      if (m_FileLen < (FX_FILESIZE)read_size) {
        read_pos = 0;
        read_size = (uint32_t)m_FileLen;
      } else {
        read_pos = m_FileLen - read_size;
      }
    }

    if (!m_pFileAccess->ReadBlock(m_pFileBuf, read_pos, read_size))
      return FALSE;

    m_BufOffset = read_pos;
  }
  ch = m_pFileBuf[pos - m_BufOffset];
  return TRUE;
}

FX_BOOL CPDF_SyntaxParser::ReadBlock(uint8_t* pBuf, uint32_t size) {
  if (!m_pFileAccess->ReadBlock(pBuf, m_Pos + m_HeaderOffset, size))
    return FALSE;
  m_Pos += size;
  return TRUE;
}

void CPDF_SyntaxParser::GetNextWordInternal(bool* bIsNumber) {
  m_WordSize = 0;
  if (bIsNumber)
    *bIsNumber = true;

  uint8_t ch;
  if (!GetNextChar(ch))
    return;

  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!GetNextChar(ch))
        return;
    }

    if (ch != '%')
      break;

    while (1) {
      if (!GetNextChar(ch))
        return;
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  if (PDFCharIsDelimiter(ch)) {
    if (bIsNumber)
      *bIsNumber = false;

    m_WordBuffer[m_WordSize++] = ch;
    if (ch == '/') {
      while (1) {
        if (!GetNextChar(ch))
          return;

        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          m_Pos--;
          return;
        }

        if (m_WordSize < sizeof(m_WordBuffer) - 1)
          m_WordBuffer[m_WordSize++] = ch;
      }
    } else if (ch == '<') {
      if (!GetNextChar(ch))
        return;

      if (ch == '<')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    } else if (ch == '>') {
      if (!GetNextChar(ch))
        return;

      if (ch == '>')
        m_WordBuffer[m_WordSize++] = ch;
      else
        m_Pos--;
    }
    return;
  }

  while (1) {
    if (m_WordSize < sizeof(m_WordBuffer) - 1)
      m_WordBuffer[m_WordSize++] = ch;

    if (!PDFCharIsNumeric(ch)) {
      if (bIsNumber)
        *bIsNumber = false;
    }

    if (!GetNextChar(ch))
      return;

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      m_Pos--;
      break;
    }
  }
}

CFX_ByteString CPDF_SyntaxParser::ReadString() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return CFX_ByteString();

  CFX_ByteTextBuf buf;
  int32_t parlevel = 0;
  int32_t status = 0;
  int32_t iEscCode = 0;
  while (1) {
    switch (status) {
      case 0:
        if (ch == ')') {
          if (parlevel == 0) {
            return buf.MakeString();
          }
          parlevel--;
          buf.AppendChar(')');
        } else if (ch == '(') {
          parlevel++;
          buf.AppendChar('(');
        } else if (ch == '\\') {
          status = 1;
        } else {
          buf.AppendChar(ch);
        }
        break;
      case 1:
        if (ch >= '0' && ch <= '7') {
          iEscCode = FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          status = 2;
          break;
        }

        if (ch == 'n') {
          buf.AppendChar('\n');
        } else if (ch == 'r') {
          buf.AppendChar('\r');
        } else if (ch == 't') {
          buf.AppendChar('\t');
        } else if (ch == 'b') {
          buf.AppendChar('\b');
        } else if (ch == 'f') {
          buf.AppendChar('\f');
        } else if (ch == '\r') {
          status = 4;
          break;
        } else if (ch != '\n') {
          buf.AppendChar(ch);
        }
        status = 0;
        break;
      case 2:
        if (ch >= '0' && ch <= '7') {
          iEscCode =
              iEscCode * 8 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          status = 3;
        } else {
          buf.AppendChar(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 3:
        if (ch >= '0' && ch <= '7') {
          iEscCode =
              iEscCode * 8 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(ch));
          buf.AppendChar(iEscCode);
          status = 0;
        } else {
          buf.AppendChar(iEscCode);
          status = 0;
          continue;
        }
        break;
      case 4:
        status = 0;
        if (ch != '\n')
          continue;
        break;
    }

    if (!GetNextChar(ch))
      break;
  }

  GetNextChar(ch);
  return buf.MakeString();
}

CFX_ByteString CPDF_SyntaxParser::ReadHexString() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return CFX_ByteString();

  CFX_ByteTextBuf buf;
  bool bFirst = true;
  uint8_t code = 0;
  while (1) {
    if (ch == '>')
      break;

    if (std::isxdigit(ch)) {
      int val = FXSYS_toHexDigit(ch);
      if (bFirst) {
        code = val * 16;
      } else {
        code += val;
        buf.AppendByte(code);
      }
      bFirst = !bFirst;
    }

    if (!GetNextChar(ch))
      break;
  }
  if (!bFirst)
    buf.AppendByte(code);

  return buf.MakeString();
}

void CPDF_SyntaxParser::ToNextLine() {
  uint8_t ch;
  while (GetNextChar(ch)) {
    if (ch == '\n')
      break;

    if (ch == '\r') {
      GetNextChar(ch);
      if (ch != '\n')
        --m_Pos;
      break;
    }
  }
}

void CPDF_SyntaxParser::ToNextWord() {
  uint8_t ch;
  if (!GetNextChar(ch))
    return;

  while (1) {
    while (PDFCharIsWhitespace(ch)) {
      if (!GetNextChar(ch))
        return;
    }

    if (ch != '%')
      break;

    while (1) {
      if (!GetNextChar(ch))
        return;
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }
  m_Pos--;
}

CFX_ByteString CPDF_SyntaxParser::GetNextWord(bool* bIsNumber) {
  GetNextWordInternal(bIsNumber);
  return CFX_ByteString((const FX_CHAR*)m_WordBuffer, m_WordSize);
}

CFX_ByteString CPDF_SyntaxParser::GetKeyword() {
  return GetNextWord(nullptr);
}

CPDF_Object* CPDF_SyntaxParser::GetObject(CPDF_IndirectObjectHolder* pObjList,
                                          uint32_t objnum,
                                          uint32_t gennum,
                                          FX_BOOL bDecrypt) {
  CFX_AutoRestorer<int> restorer(&s_CurrentRecursionDepth);
  if (++s_CurrentRecursionDepth > kParserMaxRecursionDepth)
    return nullptr;

  FX_FILESIZE SavedObjPos = m_Pos;
  bool bIsNumber;
  CFX_ByteString word = GetNextWord(&bIsNumber);
  if (word.GetLength() == 0)
    return nullptr;

  if (bIsNumber) {
    FX_FILESIZE SavedPos = m_Pos;
    CFX_ByteString nextword = GetNextWord(&bIsNumber);
    if (bIsNumber) {
      CFX_ByteString nextword2 = GetNextWord(nullptr);
      if (nextword2 == "R")
        return new CPDF_Reference(pObjList, FXSYS_atoui(word.c_str()));
    }
    m_Pos = SavedPos;
    return new CPDF_Number(word.AsStringC());
  }

  if (word == "true" || word == "false")
    return new CPDF_Boolean(word == "true");

  if (word == "null")
    return new CPDF_Null;

  if (word == "(") {
    CFX_ByteString str = ReadString();
    if (m_pCryptoHandler && bDecrypt)
      m_pCryptoHandler->Decrypt(objnum, gennum, str);
    return new CPDF_String(str, FALSE);
  }

  if (word == "<") {
    CFX_ByteString str = ReadHexString();
    if (m_pCryptoHandler && bDecrypt)
      m_pCryptoHandler->Decrypt(objnum, gennum, str);

    return new CPDF_String(str, TRUE);
  }

  if (word == "[") {
    CPDF_Array* pArray = new CPDF_Array;
    while (CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, true))
      pArray->Add(pObj);

    return pArray;
  }

  if (word[0] == '/') {
    return new CPDF_Name(
        PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1)));
  }

  if (word == "<<") {
    int32_t nKeys = 0;
    FX_FILESIZE dwSignValuePos = 0;

    std::unique_ptr<CPDF_Dictionary, ReleaseDeleter<CPDF_Dictionary>> pDict(
        new CPDF_Dictionary);
    while (1) {
      CFX_ByteString key = GetNextWord(nullptr);
      if (key.IsEmpty())
        return nullptr;

      FX_FILESIZE SavedPos = m_Pos - key.GetLength();
      if (key == ">>")
        break;

      if (key == "endobj") {
        m_Pos = SavedPos;
        break;
      }

      if (key[0] != '/')
        continue;

      ++nKeys;
      key = PDF_NameDecode(key);
      if (key.IsEmpty())
        continue;

      if (key == "/Contents")
        dwSignValuePos = m_Pos;

      CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, true);
      if (!pObj)
        continue;

      CFX_ByteString keyNoSlash(key.raw_str() + 1, key.GetLength() - 1);
      pDict->SetAt(keyNoSlash, pObj);
    }

    // Only when this is a signature dictionary and has contents, we reset the
    // contents to the un-decrypted form.
    if (pDict->IsSignatureDict() && dwSignValuePos) {
      CFX_AutoRestorer<FX_FILESIZE> save_pos(&m_Pos);
      m_Pos = dwSignValuePos;
      pDict->SetAt("Contents", GetObject(pObjList, objnum, gennum, false));
    }

    FX_FILESIZE SavedPos = m_Pos;
    CFX_ByteString nextword = GetNextWord(nullptr);
    if (nextword != "stream") {
      m_Pos = SavedPos;
      return pDict.release();
    }
    return ReadStream(pDict.release(), objnum, gennum);
  }

  if (word == ">>")
    m_Pos = SavedObjPos;

  return nullptr;
}

CPDF_Object* CPDF_SyntaxParser::GetObjectByStrict(
    CPDF_IndirectObjectHolder* pObjList,
    uint32_t objnum,
    uint32_t gennum) {
  CFX_AutoRestorer<int> restorer(&s_CurrentRecursionDepth);
  if (++s_CurrentRecursionDepth > kParserMaxRecursionDepth)
    return nullptr;

  FX_FILESIZE SavedObjPos = m_Pos;
  bool bIsNumber;
  CFX_ByteString word = GetNextWord(&bIsNumber);
  if (word.GetLength() == 0)
    return nullptr;

  if (bIsNumber) {
    FX_FILESIZE SavedPos = m_Pos;
    CFX_ByteString nextword = GetNextWord(&bIsNumber);
    if (bIsNumber) {
      CFX_ByteString nextword2 = GetNextWord(nullptr);
      if (nextword2 == "R")
        return new CPDF_Reference(pObjList, FXSYS_atoui(word.c_str()));
    }
    m_Pos = SavedPos;
    return new CPDF_Number(word.AsStringC());
  }

  if (word == "true" || word == "false")
    return new CPDF_Boolean(word == "true");

  if (word == "null")
    return new CPDF_Null;

  if (word == "(") {
    CFX_ByteString str = ReadString();
    if (m_pCryptoHandler)
      m_pCryptoHandler->Decrypt(objnum, gennum, str);
    return new CPDF_String(str, FALSE);
  }

  if (word == "<") {
    CFX_ByteString str = ReadHexString();
    if (m_pCryptoHandler)
      m_pCryptoHandler->Decrypt(objnum, gennum, str);
    return new CPDF_String(str, TRUE);
  }

  if (word == "[") {
    std::unique_ptr<CPDF_Array, ReleaseDeleter<CPDF_Array>> pArray(
        new CPDF_Array);
    while (CPDF_Object* pObj = GetObject(pObjList, objnum, gennum, true))
      pArray->Add(pObj);

    return m_WordBuffer[0] == ']' ? pArray.release() : nullptr;
  }

  if (word[0] == '/') {
    return new CPDF_Name(
        PDF_NameDecode(CFX_ByteStringC(m_WordBuffer + 1, m_WordSize - 1)));
  }

  if (word == "<<") {
    std::unique_ptr<CPDF_Dictionary, ReleaseDeleter<CPDF_Dictionary>> pDict(
        new CPDF_Dictionary);
    while (1) {
      FX_FILESIZE SavedPos = m_Pos;
      CFX_ByteString key = GetNextWord(nullptr);
      if (key.IsEmpty())
        return nullptr;

      if (key == ">>")
        break;

      if (key == "endobj") {
        m_Pos = SavedPos;
        break;
      }

      if (key[0] != '/')
        continue;

      key = PDF_NameDecode(key);
      std::unique_ptr<CPDF_Object, ReleaseDeleter<CPDF_Object>> obj(
          GetObject(pObjList, objnum, gennum, true));
      if (!obj) {
        uint8_t ch;
        while (GetNextChar(ch) && ch != 0x0A && ch != 0x0D) {
          continue;
        }
        return nullptr;
      }

      if (key.GetLength() > 1) {
        pDict->SetAt(CFX_ByteString(key.c_str() + 1, key.GetLength() - 1),
                     obj.release());
      }
    }

    FX_FILESIZE SavedPos = m_Pos;
    CFX_ByteString nextword = GetNextWord(nullptr);
    if (nextword != "stream") {
      m_Pos = SavedPos;
      return pDict.release();
    }

    return ReadStream(pDict.release(), objnum, gennum);
  }

  if (word == ">>")
    m_Pos = SavedObjPos;

  return nullptr;
}

unsigned int CPDF_SyntaxParser::ReadEOLMarkers(FX_FILESIZE pos) {
  unsigned char byte1 = 0;
  unsigned char byte2 = 0;

  GetCharAt(pos, byte1);
  GetCharAt(pos + 1, byte2);

  if (byte1 == '\r' && byte2 == '\n')
    return 2;

  if (byte1 == '\r' || byte1 == '\n')
    return 1;

  return 0;
}

CPDF_Stream* CPDF_SyntaxParser::ReadStream(CPDF_Dictionary* pDict,
                                           uint32_t objnum,
                                           uint32_t gennum) {
  CPDF_Object* pLenObj = pDict->GetObjectBy("Length");
  FX_FILESIZE len = -1;
  CPDF_Reference* pLenObjRef = ToReference(pLenObj);

  bool differingObjNum = !pLenObjRef || (pLenObjRef->GetObjList() &&
                                         pLenObjRef->GetRefObjNum() != objnum);
  if (pLenObj && differingObjNum)
    len = pLenObj->GetInteger();

  // Locate the start of stream.
  ToNextLine();
  FX_FILESIZE streamStartPos = m_Pos;

  const CFX_ByteStringC kEndStreamStr("endstream");
  const CFX_ByteStringC kEndObjStr("endobj");

  CPDF_CryptoHandler* pCryptoHandler =
      objnum == (uint32_t)m_MetadataObjnum ? nullptr : m_pCryptoHandler.get();
  if (!pCryptoHandler) {
    FX_BOOL bSearchForKeyword = TRUE;
    if (len >= 0) {
      pdfium::base::CheckedNumeric<FX_FILESIZE> pos = m_Pos;
      pos += len;
      if (pos.IsValid() && pos.ValueOrDie() < m_FileLen)
        m_Pos = pos.ValueOrDie();

      m_Pos += ReadEOLMarkers(m_Pos);
      FXSYS_memset(m_WordBuffer, 0, kEndStreamStr.GetLength() + 1);
      GetNextWordInternal(nullptr);
      // Earlier version of PDF specification doesn't require EOL marker before
      // 'endstream' keyword. If keyword 'endstream' follows the bytes in
      // specified length, it signals the end of stream.
      if (FXSYS_memcmp(m_WordBuffer, kEndStreamStr.raw_str(),
                       kEndStreamStr.GetLength()) == 0) {
        bSearchForKeyword = FALSE;
      }
    }

    if (bSearchForKeyword) {
      // If len is not available, len needs to be calculated
      // by searching the keywords "endstream" or "endobj".
      m_Pos = streamStartPos;
      FX_FILESIZE endStreamOffset = 0;
      while (endStreamOffset >= 0) {
        endStreamOffset = FindTag(kEndStreamStr, 0);

        // Can't find "endstream".
        if (endStreamOffset < 0)
          break;

        // Stop searching when "endstream" is found.
        if (IsWholeWord(m_Pos - kEndStreamStr.GetLength(), m_FileLen,
                        kEndStreamStr, TRUE)) {
          endStreamOffset = m_Pos - streamStartPos - kEndStreamStr.GetLength();
          break;
        }
      }

      m_Pos = streamStartPos;
      FX_FILESIZE endObjOffset = 0;
      while (endObjOffset >= 0) {
        endObjOffset = FindTag(kEndObjStr, 0);

        // Can't find "endobj".
        if (endObjOffset < 0)
          break;

        // Stop searching when "endobj" is found.
        if (IsWholeWord(m_Pos - kEndObjStr.GetLength(), m_FileLen, kEndObjStr,
                        TRUE)) {
          endObjOffset = m_Pos - streamStartPos - kEndObjStr.GetLength();
          break;
        }
      }

      // Can't find "endstream" or "endobj".
      if (endStreamOffset < 0 && endObjOffset < 0) {
        pDict->Release();
        return nullptr;
      }

      if (endStreamOffset < 0 && endObjOffset >= 0) {
        // Correct the position of end stream.
        endStreamOffset = endObjOffset;
      } else if (endStreamOffset >= 0 && endObjOffset < 0) {
        // Correct the position of end obj.
        endObjOffset = endStreamOffset;
      } else if (endStreamOffset > endObjOffset) {
        endStreamOffset = endObjOffset;
      }

      len = endStreamOffset;
      int numMarkers = ReadEOLMarkers(streamStartPos + endStreamOffset - 2);
      if (numMarkers == 2) {
        len -= 2;
      } else {
        numMarkers = ReadEOLMarkers(streamStartPos + endStreamOffset - 1);
        if (numMarkers == 1) {
          len -= 1;
        }
      }

      if (len < 0) {
        pDict->Release();
        return nullptr;
      }
      pDict->SetAtInteger("Length", len);
    }
    m_Pos = streamStartPos;
  }

  if (len < 0) {
    pDict->Release();
    return nullptr;
  }

  uint8_t* pData = nullptr;
  if (len > 0) {
    pData = FX_Alloc(uint8_t, len);
    ReadBlock(pData, len);
    if (pCryptoHandler) {
      CFX_BinaryBuf dest_buf;
      dest_buf.EstimateSize(pCryptoHandler->DecryptGetSize(len));

      void* context = pCryptoHandler->DecryptStart(objnum, gennum);
      pCryptoHandler->DecryptStream(context, pData, len, dest_buf);
      pCryptoHandler->DecryptFinish(context, dest_buf);

      FX_Free(pData);
      pData = dest_buf.GetBuffer();
      len = dest_buf.GetSize();
      dest_buf.DetachBuffer();
    }
  }

  CPDF_Stream* pStream = new CPDF_Stream(pData, len, pDict);
  streamStartPos = m_Pos;
  FXSYS_memset(m_WordBuffer, 0, kEndObjStr.GetLength() + 1);

  GetNextWordInternal(nullptr);

  int numMarkers = ReadEOLMarkers(m_Pos);
  if (m_WordSize == static_cast<unsigned int>(kEndObjStr.GetLength()) &&
      numMarkers != 0 &&
      FXSYS_memcmp(m_WordBuffer, kEndObjStr.raw_str(),
                   kEndObjStr.GetLength()) == 0) {
    m_Pos = streamStartPos;
  }
  return pStream;
}

void CPDF_SyntaxParser::InitParser(IFX_FileRead* pFileAccess,
                                   uint32_t HeaderOffset) {
  FX_Free(m_pFileBuf);

  m_pFileBuf = FX_Alloc(uint8_t, m_BufSize);
  m_HeaderOffset = HeaderOffset;
  m_FileLen = pFileAccess->GetSize();
  m_Pos = 0;
  m_pFileAccess = pFileAccess;
  m_BufOffset = 0;
  pFileAccess->ReadBlock(
      m_pFileBuf, 0,
      (size_t)((FX_FILESIZE)m_BufSize > m_FileLen ? m_FileLen : m_BufSize));
}

uint32_t CPDF_SyntaxParser::GetDirectNum() {
  bool bIsNumber;
  GetNextWordInternal(&bIsNumber);
  if (!bIsNumber)
    return 0;

  m_WordBuffer[m_WordSize] = 0;
  return FXSYS_atoui(reinterpret_cast<const FX_CHAR*>(m_WordBuffer));
}

bool CPDF_SyntaxParser::IsWholeWord(FX_FILESIZE startpos,
                                    FX_FILESIZE limit,
                                    const CFX_ByteStringC& tag,
                                    FX_BOOL checkKeyword) {
  const uint32_t taglen = tag.GetLength();

  bool bCheckLeft = !PDFCharIsDelimiter(tag[0]) && !PDFCharIsWhitespace(tag[0]);
  bool bCheckRight = !PDFCharIsDelimiter(tag[taglen - 1]) &&
                     !PDFCharIsWhitespace(tag[taglen - 1]);

  uint8_t ch;
  if (bCheckRight && startpos + (int32_t)taglen <= limit &&
      GetCharAt(startpos + (int32_t)taglen, ch)) {
    if (PDFCharIsNumeric(ch) || PDFCharIsOther(ch) ||
        (checkKeyword && PDFCharIsDelimiter(ch))) {
      return false;
    }
  }

  if (bCheckLeft && startpos > 0 && GetCharAt(startpos - 1, ch)) {
    if (PDFCharIsNumeric(ch) || PDFCharIsOther(ch) ||
        (checkKeyword && PDFCharIsDelimiter(ch))) {
      return false;
    }
  }
  return true;
}

// TODO(dsinclair): Split into a SearchWordForward and SearchWordBackwards
// and drop the bool.
FX_BOOL CPDF_SyntaxParser::SearchWord(const CFX_ByteStringC& tag,
                                      FX_BOOL bWholeWord,
                                      FX_BOOL bForward,
                                      FX_FILESIZE limit) {
  int32_t taglen = tag.GetLength();
  if (taglen == 0)
    return FALSE;

  FX_FILESIZE pos = m_Pos;
  int32_t offset = 0;
  if (!bForward)
    offset = taglen - 1;

  const uint8_t* tag_data = tag.raw_str();
  uint8_t byte;
  while (1) {
    if (bForward) {
      if (limit && pos >= m_Pos + limit)
        return FALSE;

      if (!GetCharAt(pos, byte))
        return FALSE;

    } else {
      if (limit && pos <= m_Pos - limit)
        return FALSE;

      if (!GetCharAtBackward(pos, byte))
        return FALSE;
    }

    if (byte == tag_data[offset]) {
      if (bForward) {
        offset++;
        if (offset < taglen) {
          pos++;
          continue;
        }
      } else {
        offset--;
        if (offset >= 0) {
          pos--;
          continue;
        }
      }

      FX_FILESIZE startpos = bForward ? pos - taglen + 1 : pos;
      if (!bWholeWord || IsWholeWord(startpos, limit, tag, FALSE)) {
        m_Pos = startpos;
        return TRUE;
      }
    }

    if (bForward) {
      offset = byte == tag_data[0] ? 1 : 0;
      pos++;
    } else {
      offset = byte == tag_data[taglen - 1] ? taglen - 2 : taglen - 1;
      pos--;
    }

    if (pos < 0)
      return FALSE;
  }

  return FALSE;
}

int32_t CPDF_SyntaxParser::SearchMultiWord(const CFX_ByteStringC& tags,
                                           FX_BOOL bWholeWord,
                                           FX_FILESIZE limit) {
  int32_t ntags = 1;
  for (int i = 0; i < tags.GetLength(); ++i) {
    if (tags[i] == 0)
      ++ntags;
  }

  // Ensure that the input byte string happens to be nul-terminated. This
  // need not be the case, but the loop below uses this guarantee to put
  // the last pattern into the vector.
  ASSERT(tags[tags.GetLength()] == 0);
  std::vector<SearchTagRecord> patterns(ntags);
  uint32_t start = 0;
  uint32_t itag = 0;
  uint32_t max_len = 0;
  for (int i = 0; i <= tags.GetLength(); ++i) {
    if (tags[i] == 0) {
      uint32_t len = i - start;
      max_len = std::max(len, max_len);
      patterns[itag].m_bsTag = tags.Mid(start, len);
      patterns[itag].m_Offset = 0;
      start = i + 1;
      ++itag;
    }
  }

  const FX_FILESIZE pos_limit = m_Pos + limit;
  for (FX_FILESIZE pos = m_Pos; !limit || pos < pos_limit; ++pos) {
    uint8_t byte;
    if (!GetCharAt(pos, byte))
      break;

    for (int i = 0; i < ntags; ++i) {
      SearchTagRecord& pat = patterns[i];
      if (pat.m_bsTag[pat.m_Offset] != byte) {
        pat.m_Offset = (pat.m_bsTag[0] == byte) ? 1 : 0;
        continue;
      }

      ++pat.m_Offset;
      if (pat.m_Offset != pat.m_bsTag.GetLength())
        continue;

      if (!bWholeWord || IsWholeWord(pos - pat.m_bsTag.GetLength(), limit,
                                     pat.m_bsTag, FALSE)) {
        return i;
      }

      pat.m_Offset = (pat.m_bsTag[0] == byte) ? 1 : 0;
    }
  }
  return -1;
}

FX_FILESIZE CPDF_SyntaxParser::FindTag(const CFX_ByteStringC& tag,
                                       FX_FILESIZE limit) {
  int32_t taglen = tag.GetLength();
  int32_t match = 0;
  limit += m_Pos;
  FX_FILESIZE startpos = m_Pos;

  while (1) {
    uint8_t ch;
    if (!GetNextChar(ch))
      return -1;

    if (ch == tag[match]) {
      match++;
      if (match == taglen)
        return m_Pos - startpos - taglen;
    } else {
      match = ch == tag[0] ? 1 : 0;
    }

    if (limit && m_Pos == limit)
      return -1;
  }
  return -1;
}

void CPDF_SyntaxParser::SetEncrypt(
    std::unique_ptr<CPDF_CryptoHandler> pCryptoHandler) {
  m_pCryptoHandler = std::move(pCryptoHandler);
}
