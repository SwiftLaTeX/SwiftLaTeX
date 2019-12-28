// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_XML_CFX_SAXREADER_H_
#define XFA_FDE_XML_CFX_SAXREADER_H_

#include "core/fxcrt/include/fx_basic.h"

class CXFA_SAXContext;

class CFX_SAXItem {
 public:
  enum class Type {
    Unknown = 0,
    Instruction,
    Declaration,
    Comment,
    Tag,
    Text,
    CharData,
  };

  CFX_SAXItem()
      : m_pNode(nullptr),
        m_eNode(Type::Unknown),
        m_dwID(0),
        m_bSkip(FALSE),
        m_pPrev(nullptr),
        m_pNext(nullptr) {}

  CXFA_SAXContext* m_pNode;
  Type m_eNode;
  uint32_t m_dwID;
  FX_BOOL m_bSkip;
  CFX_SAXItem* m_pPrev;
  CFX_SAXItem* m_pNext;
};

class CFX_SAXFile {
 public:
  CFX_SAXFile();
  FX_BOOL StartFile(IFX_FileRead* pFile, uint32_t dwStart, uint32_t dwLen);
  FX_BOOL ReadNextBlock();
  void Reset();
  IFX_FileRead* m_pFile;
  uint32_t m_dwStart;
  uint32_t m_dwEnd;
  uint32_t m_dwCur;
  uint8_t* m_pBuf;
  uint32_t m_dwBufSize;
  uint32_t m_dwBufIndex;
};

class CFX_SAXCommentContext;
enum class CFX_SaxMode;

enum CFX_SaxParseMode {
  CFX_SaxParseMode_NotConvert_amp = 1 << 0,
  CFX_SaxParseMode_NotConvert_lt = 1 << 1,
  CFX_SaxParseMode_NotConvert_gt = 1 << 2,
  CFX_SaxParseMode_NotConvert_apos = 1 << 3,
  CFX_SaxParseMode_NotConvert_quot = 1 << 4,
  CFX_SaxParseMode_NotConvert_sharp = 1 << 5,
  CFX_SaxParseMode_NotSkipSpace = 1 << 6
};

class CXFA_SAXReaderHandler;

class CFX_SAXReader {
 public:
  CFX_SAXReader();
  ~CFX_SAXReader();

  int32_t StartParse(IFX_FileRead* pFile,
                     uint32_t dwStart = 0,
                     uint32_t dwLen = -1,
                     uint32_t dwParseMode = 0);
  int32_t ContinueParse(IFX_Pause* pPause = nullptr);
  void SkipCurrentNode();
  void SetHandler(CXFA_SAXReaderHandler* pHandler);
  void AppendData(uint8_t ch);
  void AppendName(uint8_t ch);
  void ParseText();
  void ParseNodeStart();
  void ParseInstruction();
  void ParseDeclOrComment();
  void ParseDeclNode();
  void ParseComment();
  void ParseCommentContent();
  void ParseTagName();
  void ParseTagAttributeName();
  void ParseTagAttributeEqual();
  void ParseTagAttributeValue();
  void ParseMaybeClose();
  void ParseTagClose();
  void ParseTagEnd();
  void ParseTargetData();

 private:
  void Reset();
  void Push();
  void Pop();
  FX_BOOL SkipSpace(uint8_t ch);
  void SkipNode();
  void NotifyData();
  void NotifyEnter();
  void NotifyAttribute();
  void NotifyBreak();
  void NotifyClose();
  void NotifyEnd();
  void NotifyTargetData();
  void ReallocDataBuffer();
  void ReallocNameBuffer();
  void ParseChar(uint8_t ch);

  CFX_SAXFile m_File;
  CXFA_SAXReaderHandler* m_pHandler;
  int32_t m_iState;
  CFX_SAXItem* m_pRoot;
  CFX_SAXItem* m_pCurItem;
  uint32_t m_dwItemID;
  CFX_SaxMode m_eMode;
  CFX_SaxMode m_ePrevMode;
  FX_BOOL m_bCharData;
  uint8_t m_CurByte;
  uint32_t m_dwDataOffset;
  CFX_ByteArray m_SkipStack;
  uint8_t m_SkipChar;
  uint32_t m_dwNodePos;
  uint8_t* m_pszData;
  int32_t m_iDataSize;
  int32_t m_iDataLength;
  int32_t m_iEntityStart;
  int32_t m_iDataPos;
  uint8_t* m_pszName;
  int32_t m_iNameSize;
  int32_t m_iNameLength;
  uint32_t m_dwParseMode;
  CFX_SAXCommentContext* m_pCommentContext;
};

#endif  // XFA_FDE_XML_CFX_SAXREADER_H_
